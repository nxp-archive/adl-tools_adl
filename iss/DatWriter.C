//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#include <assert.h>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdexcept>
#include <sstream>
#include <locale>

#include "adl_config.h"

#include "rnumber/RNumber.h"

#include "helpers/Macros.h"
#include "helpers/AnyOption.h"
#include "helpers/FilenameUtils.h"

#if defined(ADL_HAVE_LZ4) && (ADL_HAVE_LZ4 == 1)
# include "lz4_stream/lz4_stream.h"
#endif

#include "DatWriter.h"
#include "Memory.h"
#include "WriterUtils.h"
#include "MemoryLog.h"
#include "NodeHelpers.h"
#include "SimInterface.h"
#include "TypeIO.h"

using namespace std;
using namespace rnumber;

namespace adl {

  static const string StreamName = "<stream>";

  // Add command-line options to the option database.  Note the use of logical &&
  // to ensure that we evaluate all of the additions.
  AnyOption &options();

  static bool dummy ATTRIBUTE_UNUSED = 
    options().setFlag  ("dat-multi-file",    "Causes the DAT writer to generate a file per-core, rather than all cores writing to the same file.\n"
                        "Default = false") &&
    options().setOption("dat-max-size", "The DAT writer will write to a given output file until the specified size is exceeded.\n"
                        "Each file will be suffixed with _nN, where N is a number, starting with 0.\n"
                        "Default = 0 (not activated)") &&
    options().setOption("dat-max-instrs", "The DAT writer will write to a given output file until the specified number of instructions have been logged.\n"
                        "Each file will be suffixed with _nN, where N is a number, starting with 0.\n"
                        "Default = 0 (not activated)");

# if defined(ADL_HAVE_LZ4) && (ADL_HAVE_LZ4 == 1)
  
  // Helper class for LZ4 compressed output streams.  This just stores the file
  // output stream with the compressed stream.
  class LZ4FileOutputStream : public LZ4OutputStream
  {
  public:
    LZ4FileOutputStream(unique_ptr<ofstream> ofs) : LZ4OutputStream(*ofs.get()), _ofs(std::move(ofs)) {};
    ~LZ4FileOutputStream()
    {
      close();
      delete buffer_;
      buffer_ = 0;
    }
  private:
    unique_ptr<ofstream> _ofs;
  };

# endif  
  
  // This is the per-core logging object for the DatWriter.
  struct DatWriterLogger : public WriterLogger {

    // For storing enumeration information for registers, i.e., strings
    // which are read/write for specified value
    typedef map<unsigned,std::string> EnumValues;
    typedef map<unsigned,EnumValues>  RegEnumsHash;

#   ifdef THREAD_SAFE
    boost::mutex   _io_mux;
#   endif
    string         _name;
    string         _thread_name;
    int            _thread_id;
    unsigned       _thread_instrcount;
    bool           _dep_tracking;
    RegEnumsHash   _reg_enums;
    DatWriterImpl &_w;
    CTarget       &_cur_core;
    CTarget       &_update_core;
    const IssCore *_core;   // May be 0 if this corresponds to a system node.

    static EnumValues dummy;

    DatWriterLogger(const string &name,DatWriterImpl &w,const IssCore *core) :
      _name(name),
      _thread_id(-1),
      _thread_instrcount(0),
      _dep_tracking((core && core->has_dependency_tracking())),
      _w(w),
      _cur_core(w._cur_core),
      _update_core(w._update_core),
      _core(core)
    {
    };

    ostream &o() { return *_w._o; };

    bool has_thread() const { return _thread_id >= 0; };

    const EnumValues &reg_enums(unsigned id) const
    {
      RegEnumsHash::const_iterator iter = _reg_enums.find(id);
      if (iter != _reg_enums.end()) {
        return iter->second;
      }
      return dummy;
    }

    void logCoreChange()
    {
      _w.core_update(_core,this,false);
    }

    void logCtxChange(unsigned id,const char *name,unsigned context_num,CtxType update)
    {
      switch (update) {
      case ctxUpdate:
        _update_core.set_ctx(id,context_num);
        break;
      case ctxDone:
        _update_core.remove_ctx();
        break;
      case ctxSwitch:
        o() << "\nCTX n=" << name << " cn=" << context_num;
        _w.core_tag();
        break;
      }
    }

    void logInstrPrefetch(addr_t ea)
    {
      _w.check_file_limits();
      _w.core_update(_core,this);
      _cur_core = _update_core;
      o() << "\n"
          << "I ea=0x" << hex << ea << " id=" << dec << ++_w._instrcount << " tic=" << ++_thread_instrcount << "\n";
    }

    void logInstrFromBuffer()
    {
      _w.core_update(_core,this);
      _cur_core = _update_core;
      o() << "\n"
          << "I id=" << dec << ++_w._instrcount << "\n";
    }

    void logInstrRead(unsigned id,const char *name,addr_t ea,addr_t ra,uint32_t value)
    {
      o() << hex << "M n=" << name << " t=ifetch ea=0x" << ea << " ra=0x" << ra << " d=0x" << setfill('0') << setw(8) << setfill('0') << value << dec << "\n";
      WriterLogger::logInstrRead(id,name,ea,ra,value);
    }
 
    void logInstr(const uint32_t *opc,int num_half_bytes,const char *name,Disassembler dis,uint32_t flags) 
    {
      unsigned w = (num_half_bytes+7)/8;
      RNumber opcode = RNumber(opc,w,32*w);
      unsigned shift = (32*opcode.wordCount() - 4*num_half_bytes);
      opcode >>= shift;
      if (num_half_bytes) {
        opcode.resize(4*num_half_bytes);
      }
      o() << "INSTR op=0x" << hex << setw(num_half_bytes) << opcode << "\t\t\t\t\tasm=\"";
      dis(o(),_instr_ea,opc);
      o() << "\"\n";
    }

    void logRegValue(unsigned id, const RNumber &value)
    {
      const EnumValues &evals = reg_enums(id);
      EnumValues::const_iterator iter = evals.find(value.uint32());
      if (iter == evals.end()) {
        o() << " d=0x" << hex << setfill('0') << setw(value) << value;
      } else {
        o() << " d=" << iter->second;
      }
    }

    void writeDepItem(const DepItem &d)
    {
      if (d._type == DepItem::dpMem) {
        o() << 'M';
      } else if (d._type == DepItem::dpReg) {
        o() << 'R';
      } else {
        return;
      }
      o() << d._snum;
    }

    void logDepItems()
    {
      const DepItems *di = 0;
      if (_dep_tracking && (di = _core->get_last_dependencies()) && !di->empty()) {
        bool first = true;
        o() << " dt=" << dec;
        for (DepItems::const_iterator i = di->begin(); i != di->end(); ++i) {
          if (!first) {
            o() << ',';
          }
          first = false;
          writeDepItem(*i);
        }
      }
    }

    void logInstrIssueTime(ttime_t i)
    {
      o() << "ITIME t=" << dec << i << "\n";
    }

    void logInstrCompletionTime(ttime_t c)
    {
      o() << "CTIME t=" << dec << c << "\n";
    }

    // An external write creates a new "instruction" card (top-level item).
    // Intermediate results from the actual write itself will then follow.
    void logRegExtWrite(unsigned id,const char* name, const rnumber::RNumber& value) 
    {
      o() << "\n"
          << "ERW n=" << name;
      logRegValue(id,value);
      o() << " id=" << dec << ++_w._instrcount;
      _w.core_tag();
    }
  
    // Same as above, for register files.
    void logRegfileExtWrite(unsigned id,const char* name, uint32_t index, const rnumber::RNumber& value) 
    {
      o() << "\n"
          << "ERW n=" << name << " i=" << dec << index;
      logRegValue(id,value);
      o() << " id=" << dec << ++_w._instrcount;
      _w.core_tag();
    }

    void logRegWrite(unsigned id,const char *name,const RNumber &value)
    {
      o() << "R n=" << name;
      logRegValue(id,value);
      logDepItems();
      _w.core_tag();
    }

    void logRegfileWrite(unsigned id,const char *name,uint32_t index,const RNumber &value)
    {
      o() << "R n=" << name << " i=" << dec << index ;
      logRegValue(id,value);
      logDepItems();
      _w.core_tag();
    }

    void logRegRead(unsigned id,const char *name,const RNumber &value)
    {
      o() << "R a=read n=" << name;
      logRegValue(id,value);
      if (_dep_tracking) {
        o() << " sn=" << dec << _core->get_reg_seq_num();
      }
      _w.core_tag();
    }

    void logRegfileRead(unsigned id,const char *name,uint32_t index,const RNumber &value)
    {
      o() << "R a=read n=" << name << " i=" << dec << index ;
      logRegValue(id,value);
      if (_dep_tracking) {
        o() << " sn=" << dec << _core->get_reg_seq_num();
      }
      _w.core_tag();
    }

    void logRegWriteMask(unsigned id,const char *name,const RNumber &value,const RNumber &mask)
    {
      o() << "R n=" << name;
      logRegValue(id,value);
      o() << " m=0x" << hex << setfill('0') << setw(value) << mask;
      logDepItems();
      _w.core_tag();
    }

    void logRegfileWriteMask(unsigned id,const char *name,uint32_t index,const RNumber &value,const RNumber &mask)
    {
      o() << "R n=" << name << " i=" << dec << index ;
      logRegValue(id,value);
      o() << " m=0x" << hex << setfill('0') << setw(value) << mask;
      logDepItems();
      _w.core_tag();
    }

    void logRegReadMask(unsigned id,const char *name,const RNumber &value,const RNumber &mask)
    {
      o() << "R a=read n=" << name;
      logRegValue(id,value);
      o() << " m=0x" << hex << setfill('0') << setw(value) << mask;
      if (_dep_tracking) {
        o() << " sn=" << dec << _core->get_reg_seq_num();
      }
      _w.core_tag();
    }

    void logRegfileReadMask(unsigned id,const char *name,uint32_t index,const RNumber &value,const RNumber &mask)
    {
      o() << "R a=read n=" << name << " i=" << dec << index ;
      logRegValue(id,value);
      o() << " m=0x" << hex << setfill('0') << setw(value) << mask;
      if (_dep_tracking) {
        o() << " sn=" << dec << _core->get_reg_seq_num();
      }
      _w.core_tag();
    }

    void logCoreMemWrite(unsigned id,const char *name,addr_t ea,int num_bytes)
    {
      o() << hex << "D n=" << name << " t=write ea=0x" << ea << " nb=" << dec << num_bytes;
      logDepItems();
      _w.core_tag();
    }

    void logCoreMemRead(unsigned id,const char *name,addr_t ea,int num_bytes)
    {
      o() << hex << "D n=" << name << " t=read ea=0x" << ea << " nb=" << dec << num_bytes;
      if (_dep_tracking) {
        o() << " sn=" << dec << _core->get_mem_seq_num();
      }
      _w.core_tag();
    }

    void logCoreMemWriteTyped(unsigned id,const char *name,addr_t ea,int num_bytes,CacheAccess type)
    {
      o() << hex << "D n=" << name << " t=write ea=0x" << ea << " nb=" << dec << num_bytes << " type=" << type;
      logDepItems();
      _w.core_tag();
    }

    void logCoreMemReadTyped(unsigned id,const char *name,addr_t ea,int num_bytes,CacheAccess type)
    {
      o() << hex << "D n=" << name << " t=read ea=0x" << ea << " nb=" << dec << num_bytes << " type=" << type;
      if (_dep_tracking) {
        o() << " sn=" << dec << _core->get_mem_seq_num();
      }
      _w.core_tag();
    }

    void log_used_address(unsigned id,const char *name,addr_t ra,uint32_t value)
    {
      if (_w._memlog && _w._memlog->set(*_cur_core.node(),id,ra) && (value != 0)) {
        o() << "INIT\n"
            << "MD " << "n=" << name << " ra=0x" << hex << setw(sizeof(addr_t)*2) << setfill('0') << ra << " d=0x" 
            << setw(8) << setfill('0') << value << "\t#\t(" << dec << value << ")\n"
            << "TRACE\n";
      }
    }

    // If this is a new address that we've never seen before, add a small TRACE
    // section for it.  Then, on the post-write call, print out the new value of
    // memory.  We only add the trace section if the value is non-zero, since a
    // value of zero is assumed for any unused address.
    void logMemWrite(unsigned id,const char *name,bool pre,int seq,addr_t ea,addr_t ra,uint32_t value)
    {
      if (pre) {
        log_used_address(id,name,ra,value);
      } else {
        o() << hex << "M n=" << name << " t=write ea=0x" << ea << " ra=0x" << ra << " d=0x" << setfill('0') << setw(8) << setfill('0') << value << dec;
        _w.core_tag();
      }
    }

    void logMemRead(unsigned id,const char *name,bool pre,int seq,addr_t ea,addr_t ra,uint32_t value)
    {
      if (!pre) {
        log_used_address(id,name,ra,value);
        o() << hex << "M n=" << name << " t=read ea=0x" << ea << " ra=0x" << ra << " d=0x" << setfill('0') << setw(8) << setfill('0') << value << dec;
        _w.core_tag();
      }
    }

    void logMmuTranslation(TransType tt,int seq,int set,int way,addr_t ea,addr_t ra,const MmuBase *trans)
    {
      // Note: We guard on _cur_core b/c we don't want to report translations that
      // come from the reset process.  Since post_finalize nulls-out _cur_core, this
      // should prevent this from happening.
      if (trans && _cur_core.node()) {
        FieldData ld;
        trans->report(ld);
        o() << "T n=" << trans->name() << " t=" << tt << ' ' << dec;
        if (set >= 0) {
          o() << "set=" << set << ' ';
        }
        if (way >= 0) {
          o() << "way=" << way << ' ';
        }
        if (tt != WriteTrans) {
          o() << hex << "ea=0x" << ea << " ra=0x" << ra << ' ' << dec;
        }
        _w.write_mmu_line(ld);
        _w.core_tag();
        o() << flush;
      }
    }

    void logCacheAction(const char *name,CacheAction ca,CacheAccess cac,unsigned level,
                        int set,int way,unsigned lm,addr_t ra)
    {
      o() << "C n=" << name;

      switch (ca) {
      case CacheNone:
        break;
      default:
        o() << " a=" << ca;
        break;
      }

      o() << " t=" << cac << dec;

      if (set >= 0) {
        o() << " set=" << set;
      }
      if (way >= 0) { 
        o() << " way=" << way;
      }

      o() << " lm=0x" << hex << lm << " ra=0x" << (ra & ~((addr_t)lm)) << dec;
      _w.core_tag();
    }

    void logBranchTaken(addr_t ea)
    {
      o() << "B taken=1 ea=0x" << hex << ea << "\n";
    }

    void logBreakpoint(addr_t ea,unsigned handle,PtBase *)
    {
      o() << "BP ea=0x" << hex << ea;
      _w.core_tag();
    }

    void logWatchpoint(addr_t ea,bool pre,WatchType t,int num_bytes,uint64_t value,unsigned handle,PtBase *)
    {
      if (!pre) {
        bool is_read = (t == wpRead);

        o() << "WP ea=0x" << hex << ea << dec << " t=" << ((is_read) ? "read" : "write") << " nb=" << num_bytes;
        if (!is_read) {
          o() << " d=0x" << hex << value;
        }
        _w.core_tag();
      }
    }

    void logException(unsigned handle,bool pre,const char *name)
    {
      if (pre) {
        // Could be asynchronous, hence the need to update cores.
        _w.core_update(_core,this);
        _cur_core = _update_core;

        o() << "E n=" << name;
        _w.core_tag();
      }
    }

    void logAnnotation(MsgType type,unsigned level,const string &msg,unsigned ndata,LogBase::Data data[])
    {
      if (!_core->tracing_on()) {
        // If tracing is not on then we add some comments so that the user knows
        // when and who generated this message.
        o() << "\n# " << getPathName(_core,":") << ":  Instruction " << _core->instr_count() << "\n";
      }
      o() << "A l=" << dec << level << " m=\"" << msg << "\"";
      switch (type) {
      case mInfo:
        break;
      case mWarn:
        o() << " t=Warn";
        break;
      case mError:
        o() << " t=Error";
      }
      if (ndata) {
        o() << dec;
        ForRange(ndata,i) {
          o() << ' ' << data[i];
        }
      }
      _w.core_tag();
      if (type == mError) {
        RError (getPathName(_core,":") << ":  Instr " << _core->instr_count() << ":  Level " << level << ": " << msg);
      }
    }

#   if 0
    // Only used for debugging of the merged multi-threaded trace.  It prints
    // the high-resolution timestamps used for ordering of the results.
    void logCommit(uint64_t ts)
    {
      // This turns on the use of comma-separators for the number.
      locale ol = o().imbue(std::locale(""));
      o() << "# time:  " << dec << ts << '\n';
      o().imbue(ol);
    }
#   endif

  };

  DatWriterLogger::EnumValues DatWriterLogger::dummy;


  // Allocates a stream, either with compression or not.  This is not GC'd, since
  // we know that when the DatWriterImpl goes away (which is cleaned-up), we'll want
  // to get rid of this thing, too.
  ostream *create_stream(const string &fn,bool compress)
  {
    if (compress) {
#     if defined(ADL_HAVE_LZ4) && (ADL_HAVE_LZ4 == 1)
      unique_ptr<ofstream> ofs(new ofstream(fn.c_str()));
      if (!*ofs) {
        RError("Unable to open " << fn << " for writing a DAT trace file.");
      }
      return new LZ4FileOutputStream(move(ofs));
#     else
      RError("Compressed DAT files not supported by this configuration.");
#     endif
    } else {
      auto ofs = new ofstream(fn.c_str());
      if (!*ofs) {
        RError("Unable to open " << fn << " for writing a DAT trace file.");
      }
      return ofs;
    }
  }


  //
  // The reporter class contains logic for reporting on initial and final
  // states, but is driven by the class which inherits it.
  //

  DatReporter::DatReporter(std::ostream *o,const IssNode &root) :
    _o(o),
    _memlog(nullptr),
    _root(root)
  {}

  void DatReporter::report_ctx_change(const IssNode *n,unsigned id,const char *name,unsigned context_num)
  {
    core_update(n);
    o() << "\nCTX n=" << name << " cn=" << context_num << "\n\n";
  }

  void DatReporter::report_ctx_off(const IssNode *n)
  {
    core_update(n);
    o() << "\nNOCTX\n\n";
  }

  void DatReporter::report_reg (const IssNode *n,unsigned id,const char *name,const RNumber &value,bool shared)
  {
    if (!shared) {
      core_update(n);
      // this is called by showRegs for each of the model's registers  
      o() << "RD n=" << name;
      write_reg_value(id,value);
      o() << "\n";
    }
  }

  void DatReporter::report_regfile (const IssNode *n,unsigned id,const char *name,int index,const RNumber &value,bool shared) 
  {
    if (!shared) {
      core_update(n);
      // this is called by showRegs for each of the model's register files.
      o() << "RD n=" << name << " i=" << dec << index;
      write_reg_value(id,value);
      o() << "\n";
    }
  } 

  void DatReporter::write_mmu_line(const FieldData &ld) 
  {
    ForEach(ld,i) {
      o() << " " << i->_name << "=0x" <<hex<< i->_value;
    }
  }

  void DatReporter::report_mmu (const IssNode *n,const MmuBase &mb,unsigned set,unsigned way,
                                const FieldData &ld,bool valid,bool shared,addr_t epn, bool valid_epn, addr_t rpn, bool valid_rpn)
  {
    if (!shared && (valid || show_anyway(ShowMmu))) {
      core_update(n);

      o() << "TD n=" << mb.name() << dec << " set=" << set << " way=" << way;
      write_mmu_line(ld);
      o() << "\n";
    }
  }

  void DatReporter::report_cache(const IssNode *n,const char *name,CacheType type,unsigned level,addr_t addr,
                                 unsigned set,unsigned way,bool valid,const FieldData &fd,bool shared,const byte_t *data,unsigned len)
  {
    if (!shared && (valid || show_anyway(ShowCache))) {
      core_update(n);

      o() << "CD n=" << name << " set=" << dec << set << " way=" << way << hex 
          << " ra=0x" << setfill('0') << setw(sizeof(addr_t)*2) << setfill('0') << addr << dec;
      ForEach(fd,i) {
        o() << " " << i->_name << "=" << i->_value;
      }
      if (data) {
        o() << " d=" << hex;
        bool first = true;
        for (unsigned i = 0; i < len; i += 4) {
          if (!first) o() << ",";
          first = false;
          o() << "0x" << setw(8) << setfill('0') << n->mem_read32(data+i);
        }
      }
      o() << dec << "\n";
    }
  }

  inline void DatReporter::write_mem_card(IssNode *n,const std::string &name,addr_t addr)
  {
    uint32_t v = (n) ? n->getMem(name,addr,32) : _root.getMem(0,addr,32);
    o() << "MD " << "n=" << name << " ra=0x" << hex << setw(sizeof(addr_t)*2) << setfill('0') << addr << " d=0x" 
        << setw(8) << setfill('0') << v << "\t#\t(" << dec << v << ")\n";
  }

  void DatReporter::report_memory(IssNode* n, unsigned id, const std::string &name, bool shared) 
  {
    if (!shared) {
      core_update(n);

      // Then handle used addresses.
      if (_memlog) {
        const MemInits &mem = _memlog->local_inits(*n).get_addrs(id);
        auto iter = mem.begin();
        auto end = mem.end();
      
        for ( ; iter != end; ++iter ) {
          if (iter.is_init()) {
            write_mem_card(n,name,*iter);
          }
        }
      }
    }
  }

  void DatReporter::write_mem()
  {
    const string &name = GlobalMemName;

    // Then handle used addresses.
    if (_memlog) {
      const MemInits &mem = _memlog->global_inits();
      auto iter = mem.begin();
      auto end = mem.end();
    
      for ( ; iter != end; ++iter ) {
        //addr_t x = *iter;
        if (iter.is_init()) {
          write_mem_card(0,name,*iter);
        }
      }
    }
  }

  //
  // Each implementation class basically handles a single output file, which
  // might consist of multiple cores.
  //

  DatWriterImpl::DatWriterImpl(const std::string &filename,const std::string &path,IssNode &root,
                               const MemoryLog &initMemLog,bool compress,unsigned max_size,unsigned max_instrs) : 
    DatReporter(0,root),
    _initMemLog(initMemLog),
    _compress(compress),
    _max_size(max_size),
    _max_instrs(max_instrs),
    _testcount(0),
    _instrcount(0),
    _filecount(0),
    _base_pcount(0),
    _base_icount(0),
    _update_info(0)
  {
    auto fp = split_file_name(filename);
    _fn_base = fp.first;
    _fn_ext  = fp.second;

    _o = create_stream(get_filename(),compress);

    // Walk the hierarchy, recording full core names, hashed by node pointers.
    populateCoreMap(path,root);

    if (options().getFlag("dump-options",true)) {
      o() << "#\n"
          << "# Options:\n"
          << "#   ";
      options().dump_args(o());
      o() << "\n#\n\n";
    }
  }

  DatWriterImpl::DatWriterImpl(ostream &o,IssNode &root,const MemoryLog &initMemLog) : 
    DatReporter(&o,root),
    _fn_base(StreamName),
    _initMemLog(initMemLog),
    _compress(false),
    _max_size(0),
    _max_instrs(0),
    _testcount(0),
    _instrcount(0),
    _update_info(0)
  {
    // Walk the hierarchy, recording full core names, hashed by node pointers.
    populateCoreMap("",root);
  }

  DatWriterImpl::~DatWriterImpl()
  {
    // We can only delete this if we allocated it ourselves.
    if (_fn_base != StreamName) {
      delete _o;
    }
  }

  string DatWriterImpl::get_filename()
  {
    if (_max_size || _max_instrs) {
      ostringstream s;
      s << _fn_base << "_n" << _filecount++ << _fn_ext;
      return s.str();
    } else {
      return _fn_base + _fn_ext;
    }
  }

  void DatWriterImpl::make_new_stream()
  {
    delete _o;
    _o = create_stream(get_filename(),_compress);

    o() << "TRACE\n";

    write_core_info();
  }

  // Check to see if we've exceeded any file size limits (bytes or instructions)
  // and if so, create a new file.
  void DatWriterImpl::check_file_limits()
  {
    if (!_max_size && !_max_instrs) return;

    // We only handle one condition or the other and we prioritize size over
    // instruction count.
    if (_max_size) {
      if ( _o->tellp() > (_base_pcount + _max_size)) {
        // Yes, create a new file.
        make_new_stream();
        _base_pcount = _o->tellp();
      }
    } else if (_max_instrs) {
      if (_instrcount > (_base_icount + _max_instrs)) {
        // Yes, create a new file.
        make_new_stream();
        _base_icount = _instrcount;
      }
    }
  }

  LoggingIface *DatWriterImpl::register_core(IssCore &core)
  {
    return _cores.find(&core)->second.get();
  }

  void DatWriterImpl::populateCoreMap(const string &name,IssNode &node)
  {
    string path = getPathName(&node,":");
    CoreMap::iterator iter = _cores.insert(make_pair(&node,
                                                     make_shared<DatWriterLogger>(path,*this,dynamic_cast<const IssCore *>(&node)))).first;

    // Add on context information for any contexts identified as threads.
    const CtxInfos &ctxs = node.getContextInfo();
    ForEach(ctxs,citer) {
      if ( citer->_is_thread ) {
        iter->second->_thread_name = citer->_name;
        iter->second->_thread_id   = citer->_id;
      }
    }

    // Process enums
    const RegInfos &rinfo = node.getRegInfo();
    ForEach(rinfo,ri) {
      DatWriterLogger::EnumValues  &ev = iter->second->_reg_enums[ri->_id];
      ForEach(ri->_enums,e) {
        ev.insert(*e);
      }
    }

    // Add on register enum inforation for all register/register files.

    // Then add any children.
    ForEach(node.children(),i) {
      populateCoreMap(path,**i);
    }
  }

  inline void DatWriterImpl::write_core_info()
  {
    
    if (!_update_info) {
      return;
    }
  
    const DatWriterLogger &dwl = *(_update_info);

    o() << "\nCORE n=" << dwl._name << '\n';

    if (dwl.has_thread()) {
      o() << "CTX n=" << dwl._thread_name << " cn=" << _update_core.node()->getActiveCtx(dwl._thread_id) << '\n';
    }

    o() << '\n';
  }

  void DatWriterImpl::update_core_info(const IssNode *c,DatWriterLogger *dwl,bool emit_card)
  {
    _update_core.set_node( c );
    _update_core.remove_ctx();
    _update_info = dwl;
    if (emit_card) {
      write_core_info();
    }
  }

  void DatWriterImpl::core_update(const IssNode *c,DatWriterLogger *dwl,bool emit_card)
  {
    if (_update_core.node() != c) {
      update_core_info(c,dwl,emit_card);
    }
  }

  void DatWriterImpl::core_update(const IssNode *c,bool emit_card)
  {
    if (_update_core.node() != c) {
      auto iter = _cores.find(c);
      update_core_info(c,iter->second.get(),emit_card);
    }
  }

  void DatWriterImpl::core_tag()
  {
    if (_update_core != _cur_core) {
      o() << " cn=" << _update_info->_name;
      if (_update_core.has_ctx()) {
        o() << " ctx=" << _update_core.node()->getContextInfo()[_update_core.ctx_id()]._name 
            << " ctxi=" << _update_core.ctx_num();
      }
    }
    o() << "\n";
  }

  void DatWriterImpl::post_initialize ()
  {
    _instrcount = 0;

    // This is the initialization phase, so we need to know this if we have
    // overrides on what we display.
    _is_init = true;

    // First, copy all existing init'd address information from the global log, which
    // is set by the readers.
    _memlogp.reset(new MemoryLog(_initMemLog));
    _memlog = _memlogp.get();

    o() << setfill('0');

    o() << "TEST id=" << _testcount++ << "\n"
        << "INIT\n";
  
    // registers
    write_regs();

    // mmu
    write_mmu();

    // cache
    write_cache();

    // memory
    write_memory();

    // Always start the trace section with an update on what core we're in and what its
    // contexts are.
    write_core_info();
  
    // write the trace tag
    o() << "TRACE\n\n";

  }

  void DatWriterImpl::post_finalize()
  {
    o() << flush;
    _cur_core.reset();
    _update_core.reset();
  }

  void DatWriterImpl::stream_done()
  {
    o() << flush;
  }


  void DatWriterImpl::writeResults() 
  {
    o() << "\nRESULTS\n";

    // This is the results phase, so we need to know this if we have
    // overrides on what we display.
    _is_init = false;

    // registers
    write_regs();

    // mmu
    write_mmu();

    // cache
    write_cache();

    // memory
    write_memory();
  }

  void DatWriterImpl::write_regs()
  {
    ForEach(_cores,i) {
      i->first->showRegs(*this);
    }
  }

  void DatWriterImpl::write_reg_value(unsigned id, const RNumber &value)
  {
    _update_info->logRegValue(id,value);
  }

  void DatWriterImpl::write_memory()
  {
    // Write the global memory usage.
    write_mem();

    // Write local memories.
    ForEach(_cores,i) {
      IssNode* n = const_cast<IssNode *>(i->first);
      n->showMem(*this);
    }

  }

  void DatWriterImpl::write_cache()
  {
    ForEach(_cores,i) {
      i->first->showCaches(*this);
    }
  }

  void DatWriterImpl::write_mmu()
  {
    ForEach(_cores,i) {
      i->first->showMmu(*this);
    }
  }

  //
  // The outer, public class, which encapsulates one or more per-core writers.
  //

  DatWriter::DatWriter(const std::string &filename,IssNode &root,const MemoryLog &initMemLog,bool compress) :
    Writer(filename,root,initMemLog)
  {
    bool mf = options().getFlag("dat-multi-file");

    unsigned max_size   = options().getOption("dat-max-size",0);
    unsigned max_instrs = options().getOption("dat-max-instrs",0);

    // Skip multi-file if we just have a single core.
    if (mf && !root.is_core()) {
      // Divide up the name into the extension and the base portion of the name,
      // so that we can then append the core-id easily to the base portion.  We search for the base_name 
      // in the overall path, and find the extension relative to that, in order to not have any issues with
      // "." in directories.
      auto fp = split_file_name(filename);
      createImpls(fp.first,fp.second,root,initMemLog,compress,max_size,max_instrs);
    } else {
      _impls.push_back(DatWriterImplPtr(new DatWriterImpl(filename,string(),root,initMemLog,
                                                          compress,max_size,max_instrs)));
    }
  }

  DatWriter::DatWriter(ostream &o,IssNode &root,const MemoryLog &initMemLog) : 
    Writer(StreamName,root,initMemLog)
  {
    // We don't currently support multiple streams for the stream writer.
    _impls.push_back(DatWriterImplPtr(new DatWriterImpl(o,root,initMemLog)));
  }

  DatWriter::~DatWriter()
  {
  }

  void DatWriter::createImpls(const std::string &filename,const std::string &ext,
                              IssNode &node,const MemoryLog &initMemLog,bool compress,
                              unsigned max_size,unsigned max_instrs)
  {
    if (IssCore *core = dynamic_cast<IssCore*>(&node)) {
      // Construct a filename with the core-id appended to the basename.
      ostringstream s;
      s << filename << "_" << core->getCoreId() << ext;
      _impls.push_back(DatWriterImplPtr(new DatWriterImpl(s.str(),getPathName(&node,":"),node,initMemLog,
                                                          compress,max_size,max_instrs)));
    } else {
      ForEach(node.children(),i) {
        createImpls(filename,ext,**i,initMemLog,compress,max_size,max_instrs);
      }
    }
  }

  LoggingIface *DatWriter::register_core(IssCore &core)
  {
    // We've already walked our tree, so it's just a matter of finding the
    // item...
    ForEach(_impls,i) {
      if (auto l = (*i)->register_core(core)) {
        return l;
      }
    }
    // Should never get here.
    assert(0);
  }

  void DatWriter::post_initialize ()
  {
    ForEach(_impls,i) {
      (*i)->post_initialize();
    }
  }

  void DatWriter::post_finalize()
  {
    ForEach(_impls,i) {
      (*i)->post_finalize();
    }
  }

  void DatWriter::stream_done()
  {
    ForEach(_impls,i) {
      (*i)->stream_done();
    }
  }

  void DatWriter::writeResults() 
  {
    ForEach(_impls,i) {
      (*i)->writeResults();
    }
  }

}

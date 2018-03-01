//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

    
#include <stdexcept>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <unordered_set>

#include "helpers/Macros.h"
#include "helpers/pfxstream.h"
#include "helpers/AnyOption.h"

#include "ModelInterface.h"
#include "MiscWriter.h"
#include "UvpWriter.h"
#include "WriterUtils.h"
#include "MemoryLog.h"
#include "SimInterface.h"
#include "UvpReader.h"

using namespace std;
using namespace rnumber;

namespace adl {

  const int       size       = 64; 
  const int       default_ta = 3;

  // The memory logging structure.  The purpose of this structure is to store
  // addresses that the readers initialize.  Currently the UvpWriter uses the
  // RaInfo for the value_type and the DatWriter uses the basic MemoryLog object,
  // since it only cares about whether a location is initialized or not.
  // Note:  This is not gc'd, so the UvpWriter must explicitly delete it when done.
  struct MemoryLogger : public std::map<addr_t,RaInfo> {};

# define CheckHasInstr if (!_w.hasCurInstr()) return;

  // Each UvpObj is assigned to a single core.  The complexity, though, comes from
  // the fact that there's a CoreInfo object for each core, plus each thread.  So,
  // there isn't a simple 1-1 mapping between UvpObjs and CoreInfos.  Therefore,
  // each UvpObj actually writes through the current core, as stored in the
  // UvpWriter.
  struct UvpLogger : public WriterLogger {

    UvpWriter     &_w;
    CoreInfo      &_ci;
    const IssCore &_core;

    // Used during instruction fetching to record the actual ea/ra of the first part
    // of the instruction, along with its actual memory representation.
    unsigned       _id_index;

    // Indicates that an external register write is in effect.
    bool           _ext_reg_write;

    UvpLogger(UvpWriter &w,CoreInfo &ci,const IssCore &c) :
      _w(w),
      _ci(ci),
      _core(c),
      _id_index(0)
    {
    }

    // If our update-core pointer is different than our current-core pointer,
    // then this means that we have to amend the name with the path of the
    // other core.
    string full_name(const string &name)
    {
      return (_w._updateCore != _w._curCore) ? _w._updateCore->curpath() + "::" + name : name;
    }

    void logMmuTranslation(TransType tt,int seq,int set,int way,addr_t ea,addr_t ra,const MmuBase *mb)
    {
      CheckHasInstr;
      // We only log writes for intermediate results.
      if (tt == WriteTrans) {
        InterResult &ir = _w._curInstr->add_ir(InterResult(full_name(mb->name()),mb->id(),mb->epn(),set,way));
        mb->report(ir._fd);
        _ci.addrHack(ir._fd,ir.id());
      } else if (tt == InstrTrans) {
        _w.setCurInstrTrans(mb);
      } else  {
        _w._curInstr->add_ir(InterResult(mmu_access,mb->name(),mb->id(),mb->epn(),set,way));
        _w.setCurDataTrans(seq,mb);
        if (seq == 0) {
          // Update tea, tra, tta info for the first sequence.
          _w._curInstr->setTrgAnno(ea,ra,_w.get_ta(mb));
        }
      }
    }

    // Indicates that an external write has started, so switch to using the
    // special "jam" syntax.
    void logRegExtStart()
    {
      _ext_reg_write = true;
    }

    void logRegExtStop()
    {
      _ext_reg_write = false;
    }

    // An external write creates a new "instruction" card (top-level item).
    // Intermediate results from the actual write itself will then be attached.
    void logRegExtWrite(unsigned id,const char* name, const rnumber::RNumber& value) 
    {
      InstrEntryPtr i(new InstrEntry);
      i->set_type(InstrEntry::ExtWrite);
      i->setExtWriteInfo(name,value,_w._instr_count);
      _w._curInstr = _w._curCore->add_instr(move(i));
    }

    // Same as above, for register files.
    void logRegfileExtWrite(unsigned id,const char* name, uint32_t index, const rnumber::RNumber& value) 
    {
      ostringstream ss;
      ss << full_name(name) << index;
      logRegExtWrite(id,ss.str().c_str(),value);
    }

    void logRegWrite(unsigned id,const char *name,const RNumber &value) 
    {
      CheckHasInstr;
      // Add a register update to the current instructions intermediate result list.
      // If there's any kind of suppression then we ignore the update.
      if (_ci.reg_suppress(id) == isNone) {
        string n = full_name(name);
        InterResult ir (n,value);
        _w._curInstr->add_ir(ir);
      }
    }

    void logRegfileWrite(unsigned id,const char *name,uint32_t index,const RNumber &value) 
    {
      CheckHasInstr;
      if (_ci.reg_suppress(id) == isNone) {
        ostringstream ss;
        ss << full_name(name) << index;
        logRegWrite(id,ss.str().c_str(),value);
      }
    }

    void logRegWriteMask(unsigned id,const char *name,const RNumber &value,const RNumber &mask)
    {
      logRegWrite(id,name,value);
    }

    void logRegfileWriteMask(unsigned id,const char *name,uint32_t index,const RNumber &value,const RNumber &mask)
    {
      logRegfileWrite(id,name,index,value,mask);
    }

    void logRegWrite(unsigned id,const char *name,const RNumber &value,const RNumber &mask) 
    {
      logRegWrite(id,name,value,mask);
    }

    void logRegfileWrite(unsigned id,const char *name,uint32_t index,const RNumber &value,const RNumber &mask) 
    {
      logRegfileWrite(id,name,index,value);
    }

    void logException(unsigned handle,bool pre,const char *name)
    {
      if (!pre) {
        return;
      }

      // Since this could be an asynchronous exception, we need to make sure
      // we're directing this to the appropriate core, hence the update.
      _w.updateCurr(_ci,_core);
      _w._curCore = _w._updateCore;

      ExcptData ed = _ci.excpt_data(handle);
      if (ed._ir_action == isNone) {
        if (ed._asynch || !_w.hasCurInstr()) {
          // This is registered as an asynchronous event, so we must treat is
          // as a new instruction.
          InstrEntryPtr i(new InstrEntry);
          i->set_type(InstrEntry::Asynch);
          i->setInfo(name);
          _w._curInstr = _w._curCore->add_instr(move(i));
        }
        // Add as an intermediate result in either case.
        InterResult ir (name);
        _w._curInstr->add_ir(ir);
      }
      if (_w.hasCurInstr()) {
        _w._curInstr->set_squashed(ed._squash);
      }
    }

    void log_used_address (unsigned id,addr_t ea,addr_t ra,unsigned ta,uint32_t value) 
    {
      pair<MemoryLogger::iterator,bool> ip = _w._memQList->insert(make_pair(ra,RaInfo()));
      if (ip.second) {
        // We haven't seen this address yet, so add it to our log
        // and grab its value before the write operation occurs.
        RaInfo &ri = ip.first->second;
        ri.id(id);
        ri.owner(_w._curCore);
        ri.init_value(value);
        ri.add_mapping(ea,ta,_w._curCore);
      } else {
        // We've already seen it, so just log the mapping being used.
        ip.first->second.add_mapping(ea,ta,_w._curCore);
      }
    }

    void logMemWrite(unsigned id,const char *name,bool pre,int seq,addr_t ea,addr_t ra,uint32_t value)
    {
      CheckHasInstr;
      if (pre) {
        // log the address as a used address along w/ its translation info
        log_used_address (id,ea,ra,_w.get_ta(_w.last_data_trans(seq)),value);
      } else {
        // add a memory write update to the current instructions intermediate result
        // list
        InterResult ir (memory_write,ea,ra,_w.get_ta(_w.last_data_trans(seq)),value);
        _w._curInstr->add_ir(ir);
      }
    }

    void logMemRead (unsigned id,const char *name,bool pre,int seq,addr_t ea,addr_t ra,uint32_t value)
    {
      CheckHasInstr;
      if (!pre) {
        // log the address as a used address along w/ its translation info
        log_used_address (id,ea,ra,_w.get_ta(_w.last_data_trans(seq)),value);
        // add a memory read update to the current instructions intermediate result list
        InterResult ir (memory_read,ea,ra,_w.get_ta(_w.last_data_trans(seq)),value);
        _w._curInstr->add_ir(ir);
      }
    }

    void logCoreChange()
    {
      _w.updateCurr(_ci,_core);
    }

    void logCtxChange(unsigned id,const char *name,unsigned context_num,CtxType update)
    {
      switch (update) {
      case ctxUpdate:
        if (CoreInfo *ci = _w._curCore->switch_thread(id,context_num)) {
          _w._updateCore = ci;
        }
        break;
      case ctxDone:
        _w._updateCore = _w._curCore;
        break;
      case ctxSwitch:
        if (CoreInfo *ci = _w._curCore->switch_thread(id,context_num)) {
          _w._curCore = ci;
        }
        break;
      }
    }

    void logNewInstr(addr_t ea,bool from_buf)
    {
      _w.updateCurr(_ci,_core);
      _w._curCore = _w._updateCore;

      // Create a new instruction and add to our list.
      InstrEntryPtr i(new InstrEntry());      

      // Set the cur_instr so that we can add on any intermediate results.
      _w._curInstr = _w._curCore->add_instr(move(i));

      _w._curInstr->setEffAddr(ea);
      _w._curInstr->setFromBuffer(from_buf);

      _id_index = 0;

      _w.clearTransInfo();
    }

    void logInstrPrefetch(addr_t ea)
    {
      logNewInstr(ea,false);
    }

    void logInstrFromBuffer()
    {
      logNewInstr(0,true);
    }

    void logInstrRead(unsigned id,const char *name,addr_t ea,addr_t ra,uint32_t value)
    {
      CheckHasInstr;
      if (_w._curInstr->setAddrAndData(ra,value,_w.get_ta(_w._curInstrTrans),_id_index)) {
        ++_id_index;
      }
    }

    void logInstr(const uint32_t *opc,int num_half_bytes,const char *name,Disassembler dis,uint32_t flags) 
    {
      CheckHasInstr;
      // Build the asm string
      ostringstream asml;
      dis(asml,_instr_ea,opc);
      // Store info into the current instruction.
      _w._curInstr->setInfo(asml.str(),opc,num_half_bytes,_w._instr_count++,flags);
    }

    void logBranchTaken(addr_t)
    {
      CheckHasInstr;
      _w._curInstr->setBranchTaken(true);
    }
  
    void writeMsgType(ostream &o,MsgType type)
    {
      switch (type) {
      case mInfo:
        break;
      case mWarn:
        o << " Warning:  ";
        break;
      case mError:
        o << " Error:  ";
      }
    }

    void writeMsgBody(ostream &o,const string &msg,unsigned ndata,LogBase::Data data[])
    {
      o << msg;
      ForRange(ndata,i) {
        o << ' ' << data[i];
      }
    }

    void writeMsg(ostream &o,const IssCore &core,MsgType type,unsigned level,const string &msg,
                  unsigned ndata,LogBase::Data data[])
    {
      o << getPathName(&core,":") << ":  Instr " << core.instr_count() << ":  Level " << level << ": ";
      writeMsgType(o,type);
      writeMsgBody(o,msg,ndata,data);
    }

    void logAnnotation(MsgType type,unsigned level,const string &msg,unsigned ndata,LogBase::Data data[])
    {
      // If tracing is off, or else we have something other than an info message and we haven't yet
      // fetched the first instruction, then just directly print the message into the UVP file along
      // with extra information describing where in the test the message was generated.
      if (!_core.tracing_on_r() || (type != mInfo && !_w._curInstr)) {
        _w._os << "// ";
        writeMsg(_w._os,_core,type,level,msg,ndata,data);
        _w._os << '\n';
      } else if (_w._curInstr) {
        ostringstream ss;
        writeMsgType(ss,type);
        ss << dec << level << ": ";
        writeMsgBody(ss,msg,ndata,data);
        _w._curInstr->add_ir(InterResult(annotation,ss.str()));
      }
      if (type == mError) {
        ostringstream ss;
        writeMsg(ss,_core,type,level,msg,ndata,data);
        throw runtime_error(ss.str());
      }
    }

  };

  // Helper that searches our hierarchy of CoreInfo's for the one that corresponds
  // to the given node.
  void lookup( CoreInfo* parent, const IssNode* n, CoreInfo* &found) 
  {
    if ((n) && (parent->node() == n)) {
      found = parent;
      // Do we have a thread context?  If so, then we switch to whatever's active.
      if (CoreInfo *thread = parent->get_thread(n)) {
        found = thread;
      }
      return;
    }
    ForEach((parent->children()),iter) {
      lookup(iter->get(),n,found);
    }
  }

  // If the node has contexts (threads), then switch to the appropriate core-info
  // item.
  CoreInfo *lookup_thread(CoreInfo *ci,const IssNode *n)
  {
    if (ci->is_thread_instance() || ci->is_thread_parent()) {
      return ci->parent()->get_thread(n);
    } else {
      return ci;
    }
  }

  UvpWriter::UvpWriter(const std::string& filename,IssNode &root,const MemoryLog &initMemLog) : 
    Writer(filename,root,initMemLog),
    _root(buildHierarchy(root)),
    _curCore(_root.get()),
    _updateCore(_root.get()),
    _curInstr(0),
    _ostream(0),
    _o(filename.c_str()),
    _os(_o.rdbuf()),
    _testFinished(false),
    _testcount(1001),
    _ircount(0),
    _memQList(nullptr),
    _instr_count(0),
    _use_std_log(options().getFlag("uvp-log-result-mem",false))
  {
    reset();
    initTransInfo();

    if (!_o) {
      RError("Unable to open '" << filename << "' for writing.");
    }

    size_t n = filename.find('.');
    if (n != string::npos) {
      _testname = filename.substr(0,n);
    } else {
      _testname = filename;
    }
    // Now make sure that it's a valid identifier.  If not, then replace the char with _.
    ForEach(_testname,i) {
      if (!isalnum(*i) && *i != '_') {
        *i = '_';
      }
    }
   
    _os << "format=huvp;\n"
        << "//------------------------------------------------------------------------------\n";
    if (options().getFlag("dump-options",true)) {
      _os << "//\n"
          << "// Options:\n"
          << "//   ";
      options().dump_args(_os);
      _os << "\n//\n"
          << "//------------------------------------------------------------------------------\n";
    }
  }

  UvpWriter::UvpWriter(ostream &os,IssNode &root,const MemoryLog &initMemLog) : 
    Writer("<stream>",root,initMemLog),
    _root(buildHierarchy(root)),
    _curCore(_root.get()),
    _updateCore(_root.get()),
    _curInstr(0),
    _ostream(&os),
    _os(_ostream->rdbuf()),
    _testFinished(false),
    _testcount(1001),
    _ircount(0),
    _memQList(nullptr),
    _instr_count(0),
    _use_std_log(options().getFlag("uvp-log-result-mem",false))
  {
    reset();
    initTransInfo();
  }

  UvpWriter::~UvpWriter()
  {
    DBPrint("Cleaning up the UvpWriter\n");
  }

  void UvpWriter::reset()
  {
    _memQList.reset(new MemoryLogger);

    _testFinished = false;
    _ircount = 0;

    _curCore = _updateCore = _root.get();

    _root->reset();
  }

  // Initialization of our translation caching stuff.
  void UvpWriter::initTransInfo()
  {
    _curInstrTrans = 0;
    ForRange(MaxDataTrans,i) {
      _curDataTrans[i] = 0;
    }    
  }

  void UvpWriter::setCurInstrTrans(const MmuBase *mb)
  {
    if (_curInstrTrans && _curInstrTrans->user_entry()) {
      delete _curInstrTrans;
    }
    _curInstrTrans = (mb && mb->user_entry()) ? mb->clone() : mb;
  }

  void UvpWriter::setCurDataTrans(int seq,const MmuBase *mb)
  {
    // Get rid of the old object and set up the new one.  We only need to do
    // memory management if this is a user-entry, which means it might have been
    // created on the stack.
    if (_curDataTrans[seq] && _curDataTrans[seq]->user_entry()) {
      delete _curDataTrans[seq];
    }
    _curDataTrans[seq] = (mb && mb->user_entry()) ? mb->clone() : mb;
  }

  LoggingIface *UvpWriter::register_core(IssCore &core)
  {
    CoreInfo *ci = 0;
    lookup(_root.get(),&core,ci);
    _loggers.push_back(make_uniq_p(new UvpLogger(*this,*ci,core)));
    return _loggers.back().get();
  }

  // Clear our translation caching flags.
  inline void UvpWriter::clearTransInfo()
  {
    setCurInstrTrans(0);
    ForRange(MaxDataTrans,i) {
      setCurDataTrans(i,0);
    }
  }

  CoreInfo* UvpWriter::getCoreInfo (CoreInfo &ci,const IssCore &c) 
  {
    CoreInfo *new_ci = lookup_thread(&ci,&c);
    assert(new_ci);
    return new_ci;
  }

  // Given a node, returns our corresponding CoreInfo
  // structure (uses lookup helper function).
  CoreInfo* UvpWriter::getCoreInfo (const IssNode* n) 
  {
    CoreInfo* c = 0;
    lookup(_root.get(),n,c);
    assert(c);
    return c;
  }

  void UvpWriter::updateCurr(CoreInfo &ci,const IssCore &c) 
  {
    _updateCore = getCoreInfo(ci,c);
  }

  void UvpWriter::updateCurr(const IssNode* n)
  {
    _updateCore = getCoreInfo(n);
  }

  void UvpWriter::displayResources (CoreInfo* c) 
  {
    _curCore = c;
    c->node()->showRegs(*this);
    c->node()->showMmu(*this);
    ForEach((c->children()),iter) {
      if (! (*iter)->is_thread_instance() ) {
        displayResources(iter->get());
      }
    }
  }

  // populate the initial regs and mmu state.
  void UvpWriter::post_initialize () 
  {
    reset();

    // This is the initialization phase, so we need to know this if we have
    // overrides on what we display.
    _is_init = true;

    displayResources(_root.get());
    clearTransInfo();
    _instr_count = 1;
  }

  // If we have instructions and the last one is a halt, we need to
  // remove it and set the pc back.  We need to do this for all children.
  void UvpWriter::removeHalt(CoreInfo* c) 
  {
    if (c->node()) {
      if (!c->get_instrlist().empty()) {
        UvpResValues &rv = uvp_res_values();

        bool found = false;
        if (rv.empty()) {
          // No UVP result-values specified, so just look for 0.
          if (c->get_instrlist().back()->opc().iszero()) {
            found = true;
          }
        } else {
          // Otherwise, look for any listed value.
          ForEach((rv),rv_iter) {
            const UvpResValue &r = rv_iter->second;
            if (c->get_instrlist().back()->opc() == r._value) {
              found = true;
            }
          }
        }
        if (found) {
          c->setProgramCounter(c->get_instrlist().back()->ea());
          c->get_instrlist().pop_back();
        }
      }
    }
    ForEach((c->children()),iter) {
      removeHalt(iter->get());
    }
  }

  void UvpWriter::pre_finalize () 
  {
    DBPrint("Beginning results write.\n");
    // Simulation is done.
    _testFinished = true;

    // This is the results phase, so we need to know this if we have
    // overrides on what we display.
    _is_init = false;
  
    // hack! remove the halt instruction and set back the pc
    removeHalt(_root.get());
  
    //get the final state for registers and mmus
    displayResources(_root.get());
  }

  void UvpWriter::post_finalize () 
  { 
    _o << flush; 
    DBPrint("Done with results write.\n");
    // This is to be able to reclaim some space:  We can clear our major data structures
    // because we don't need the information any more.
    
  }

  void UvpWriter::stream_done()
  {
    write_instrs(_curCore);
    _o << flush;
    _memQList->clear();
    _root->remove_instrs();
  }

  void writeDCmd (opfxstream &os,addr_t ea,addr_t ra,addr_t data,bool print_ta, uint32_t wimg,addr_t umask) {

    unsigned w = (size + 3) / 4;
  
    os.fill('0');

    os << "D {"    
       << "ea=0x"  << setw(w) << hex << ea
       << "; ra=0x"  << setw(w) << ra
       << "; d=0x" << setw(8) << data;
    if (print_ta) {
      os  << "; ta=0x"<< setw(1) << wimg;
    }
    if (umask) {
      os<< "; u=0x" << setw(8) << umask;
    }
    os<< ";};\n";
  }

  unsigned be2le(const RNumber &value,unsigned pos)
  {
    return value.size() - pos - 1;
  }

  RNumber getFieldValue(const RNumber &value,const RegFieldInfo::Bits &bits)
  {
    int twidth = 0;
    ForEach(bits,i) {
      twidth += (i->first > i->second) ? (i->first - i->second) : (i->second - i->first);
    }
    RNumber r(0,twidth);
    ForEach(bits,i) {
      int width = abs((int)(i->second - i->first));
      int start = i->first % value.size();
      int stop  = i->second % value.size();
      if (i->first > i->second) {
        // Handle little-endian stuff here.  No direct support in RNumber,
        // unfortunately.
        start = be2le(value,start);
        stop  = be2le(value,stop);
      }
      r = (r << width) | value.getField(start,stop);
    }
    return r;
  }

  void writeRCmd (opfxstream &os,const string& name, const RNumber& value, 
                  const RNumber& umask,const char* p,const RegFieldInfos *fields) 
  { 
    os << p << " : ";
 
    os.fill('0');
    os<< "R {nm=\"" << name << "\"; d=0x" << hex << setw(value) << value << ";";
    if (!umask.iszero()) {
      os << " u=0x" << setw(value) << umask << ";";
    }
    os << "};";
    if (fields) {
      // If we have fields, print out the fields and their values as a comment.
      ostringstream ss;
      int p = -81;
      ss << hex;
      ForEach((*fields),iter) {
        if ((int)ss.str().size() > (p+80)) {
          ss << "\n// ";
          p = ss.str().size();
        }
        ss << iter->_name << ":" << getFieldValue(value,iter->_bits) << ' ';
      }
      os << ss.str() << dec;
    } 
    os << '\n';
  }

  void writeTCmd (opfxstream &os,const RegSuppressHash &ignores,const char *lookup,unsigned set,unsigned way,
                  const FieldData &ld,const string& pfx,addr_t epn,bool valid_epn,addr_t rpn,bool valid_rpn) 
  {
    unsigned w =  (size + 3) / 4;
    string type = "ID"; // fix me: we need the type
    os<<pfx<<" : ";
    os.fill('0');
    os <<"T {n=\"";
    os<<lookup<<"\"; t=\""<<type<<dec<<"\"; way=" << way;
    if (valid_epn) {
      os <<hex<<"; ep=0x"<<setw(w)<<epn;
    }
    if (valid_rpn) {
      os <<hex<<"; rp=0x"<<setw(w)<<rpn;
    }
    os.addindent(8);
    os <<"; \nd={";

    uint32_t count = 1;
    os.addindent(2);
    int index = 0;
    for (auto i = ld.begin(); i != ld.end(); ++i, ++index) {
      if (ignore_type(ignores,index) == isAll) {
        continue;
      }
      os << "{\"" <<i->_name<< "\",0x" <<hex<<i->_value<<"},";
      count++;
      if (count > 5) {
        os<<'\n';
        count = 0;
      }
    }
    os <<"};};\n";
    os.subindent(10);
  }

  void writeFCmd (opfxstream &os,addr_t ea,unsigned count,unsigned gcount)
  {
    int w =  (size + 3) / 4;
    os.fill('0');

    os << "E : F {ea=0x" << setw(w) << hex << ea << "; c=" << dec << count << "; gc=" << gcount << ";";
    os.addindent(15);
  }

  void writeJamCmd(opfxstream &os,const string& asml, const RNumber& opc, 
                   int num_half_bytes, unsigned count) 
  {
    os.fill('0');

    os << "E : A {t=\"ijam\"; c=" << dec << count << ";\n";
    os.addindent(15);
    os << "data=[ asm=>\"" << asml << "\", op=>0x" << setw(num_half_bytes) << hex << opc << "];";
  }

  void writeICmd (opfxstream &os,const string& asml, addr_t ea,const InstrEntry::IData &id,
                  const RNumber& opc, int num_half_bytes, bool print_ta,bool print_bt,bool bt,
                  const TargetAnnotation *annotation,unsigned count,unsigned gcount) 
  {
    int w =  (size + 3) / 4;
    os.fill('0');

    os<< "E : I {asm=\"" << asml;

    if (annotation) {
      os << " (EA=0x" << setw (w) << hex << annotation->_tea
         << ",RA=0x" << setw (w) << hex << annotation->_tra << ")";
    }

    // write the instruction number
    os<< "\"; c=" << dec << count << "; gc=" << gcount << ";";

  
    os.addindent(15);
 
    os << "\n" << hex;
    int num_bytes = num_half_bytes/2;
    int iw = num_bytes*2;

    if (id.size() > 1) {
      // EAs are always contiguous, so we can simply calculate the second effective address as being
      // the start of the next word.
      //int nb2 = 4 - (ea & 0x3ULL);
      //int nb1 = num_bytes - nb2;

      os << "ea=0x" << setw(w) << ea << "; ra={";
      ForRange(id.size(),i) {        
        os << (i ? "," : "") << "0x" << setw(w) << id[i]._ra;
      }
      int nb1 = 4 - (ea & 0x3ULL);
      os << "}; d={";
      ForRange(id.size(),i) {        
        if (i == 0) {
          os << "0x" << setw(nb1*2) << id[i]._d;	
        } else {
          int remained = num_bytes - (4*(i-1) + nb1);
          int nb2 = remained > 4 ? 4 : remained;
          os << ",0x" << setw (nb2*2) << id[i]._d;
        }
      }
      os << "}; ";     
      
      os << "op=0x" << setw(num_half_bytes) << opc << "; ";
      if (print_ta) {
        os << "ta={0x"  << setw(1) << id[0]._wimg << ",0x" << setw(1) << id[1]._wimg << "}; ";
      }
      os << "op_size=" << dec << num_bytes*8 << ";";
    } else {
      os << "ea=0x" << setw(w) << ea
         << "; ra=0x" << setw(w)  << id[0]._ra
         << "; d=0x"  << setw(iw) << id[0]._d
         << "; op=0x" << setw(num_half_bytes) << opc;
      if (print_ta) {
        os << "; ta=0x" << setw(1)  << id[0]._wimg;
      }
      os << "; op_size=" << dec << num_bytes*8 << ";";
    }

    bool p = false;
    if (print_bt) {
      os<< "\n" << hex
        << "bt=" << ((bt) ? "\"T\"; " : "\"N\"; ");
      p = true;
    }

    if (annotation) {
      if (!p) {
        os<< "\n" << hex;
      }
      os << "tea=0x" << setw(w) << annotation->_tea << "; "
         << "tra=0x" << setw(w) << annotation->_tra << "; "
         << "tta=0x" << setw(1) << annotation->_tta << "; ";
    }
  }

  void writeACmd(opfxstream &os,const string &nm)
  {
    os << "E : A {nm=\"" << nm << "\";";
    os.addindent(15);
  }

  void writeERWCmd(opfxstream &os,const string &nm,const RNumber &value,unsigned count)
  {
    os << "E : A {t=\"write\"; c=" << dec << count << ";\n";
    os.addindent(15);
    os.fill('0');
    os << "data=[t=>\"reg\", nm=>\"" << nm << "\", d=>0x" << hex << setw(value) << value << " ];";
  }

  // Get the last data translation item, given a sequence number.
  const MmuBase *UvpWriter::last_data_trans(int seq)
  {
    while (seq >= 0) {
      if (_curDataTrans[seq]) {
        return _curDataTrans[seq];
      }
      --seq;
    }
    return 0;
  }

  // Extract the translation-attribute value from the last translation.
  unsigned UvpWriter::get_ta(const MmuBase *trans)
  {
    if (!trans) {
      return default_ta;
    } else {
      assert(_curCore);
      int id = trans->id();
      const TransAttrs &transAttrs = _curCore->transAttrs();

      unsigned ta = 0;
      if (id < (int)transAttrs.size()) {
        for (AttrList::const_iterator i = transAttrs[id].begin(), end = transAttrs[id].end(); i != end; ++i) {
          unsigned x = trans->getField(i->_index);
          ta = (ta << i->_width) | x;
        }
      }

      return ta;
    }
  }

  void UvpWriter::writeResCard(addr_t ea,addr_t ra,bool print_ta,unsigned wimg) 
  {
    MemoryLogger::iterator  iter=_memQList->find(ra);
    if (iter != _memQList->end()) {
      cerr <<" WARNING: The final real address points to an initialized memory location\n";
    }
  
    unsigned w =  (size + 3) / 4;
    _os << "R : RES {ea=0x"<<hex<<setw(w)<<ea<<"; ra=0x"<<setw(w)<<ra<<";";
    if (print_ta) {
      _os << "ta=0x"<<wimg<<";";
    }
    _os << "};\n";
  }

  LogBase &getLogger(const CoreInfo *ci)
  {
    return ci->node()->logger();
  }

  void UvpWriter::write_instrs(const CoreInfo *ci) 
  {
    const Instrs &instrlist = ci->get_instrlist(); 

    // write each instruction
    unsigned c = 0;
  
    ForEach(instrlist,iter){
      const InstrEntry &instr = **iter;

      if (instr.squashed()) {
        continue;
      }

      bool has_ir = (!instr.irs().empty());
      bool first = true;
      unordered_set <string> regs_written;
      write_instr(instr,c,instr.gcount(),ci->print_ta());
      // write each of the instructions intermediate results
      if (has_ir) {
        _os<< "\n";
        _os<< "ir={";
        _os.addindent(4);
      }
      InterResults irs = (instr.irs());
      bool unique = true;
      // write the regsister updates first, we reverse iterate so that we only display
      // the last update for a given register.
      for (InterResults::reverse_iterator iter_irs = irs.rbegin(), rend = irs.rend(); iter_irs != rend; ++iter_irs) {
        if ((*iter_irs).ir_type() == register_write) {
          unique = regs_written.insert((*iter_irs).name()).second;
          if (unique) {
            // if this reg doesn't exist in the regs_written hash, then this is the last update
            // we received for this reg for this instruction (the updates are stored lifo), so we
            // write it out, otherwise, it already existed in the hash and we suppress the
            // displaying of mulitiple updates for the same reg.
            if (!first) {
              _os<<"\n";
            }
            first = false;
            write_ir_reg_update((*iter_irs).name(),(*iter_irs).reg_value());
          }
        }
      }
      // write all other updates.
      ForEach(irs,iter_irs) {
        switch ((*iter_irs).ir_type()) {
        case annotation:
          if (!first) {
            _os<<"\n";
          }
          write_ir_annotation((*iter_irs).msg());
          // We always need to have newlines, since we're dealing with line
          // comments.  So we add our own newline and then have the next item
          // not bother with a newline.
          _os << '\n';
          first = true;
          break;
        case interrupt:
          if (!first) {
            _os<<"\n";
          }
          first = false;
          write_ir_interrupt((*iter_irs).msg());
          break;
        case memory_write:
        case memory_read:
          if (!first) {
            _os<<"\n";
          }
          first = false;
          write_ir_mem_update((*iter_irs).ea(),(*iter_irs).ra(),ci->print_ta(),(*iter_irs).wimg(),(*iter_irs).mem_value(),(*iter_irs).ir_type());
          break;
        case mmu_write:
          if (!first) {
            _os<<"\n";
          }
          first = false;
          write_ir_mmu_update(ci->tlb_suppress((*iter_irs).id()),(*iter_irs).name(),(*iter_irs).ea(),(*iter_irs).way(),(*iter_irs)._fd);
          break;
        case mmu_access:
          if (!first) {
            _os<<"\n";
          }
          write_ir_mmu_access((*iter_irs).name(),(*iter_irs).set(),(*iter_irs).way());
          // Have to have our own newline, since it's a comment.
          first = true;
          _os << '\n';
          break;
        default:
          ;
        }
      }

      if (has_ir) {
        // write the end ir delimiter
        _os<<"};";
        // reset the indentation
        _os.subindent(4);
      }
      // write the end instruction delimiter
      _os<<"};\n";
      _os.subindent(15);
    }
    // We'll only write a result card if we were in trace mode and we only do
    // this for leaf-nodes, i.e. if it's a system or has contexts which act as
    // threads, then don't print a RES card.
    if (ci->node() && ci->children().empty() && !_ostream) {
      try {
        // OK, this is a little complex here:  We enable logging of mmu translations here so that
        // we grab the instruction translation that's used to translate the program counter.
        bool o = getLogger(ci).log_direct(true);
        setCurInstrTrans(0);
        pair<addr_t,addr_t> pc = ci->getProgramCounter();
        getLogger(ci).log_direct(o);
        writeResCard(pc.first,pc.second,ci->print_ta(),get_ta(_curInstrTrans));
      }
      catch (...) {
        stringstream ss;
        ss << hex << "0x" << ci->node()->getProgramCounter();
        throw std::runtime_error("UvpWriter: unable to translate final ea for RES card: " +  ss.str()); 
      }
    }
  }

  void UvpWriter::write_instr(const InstrEntry &instr,unsigned &count,unsigned gcount,bool print_ta) 
  {
    // new instruction
    switch (instr.type()) {
    case InstrEntry::Instr:
      ++count;
      if (instr.from_buf()) {
        // This instruction wasn't fetched, so it was jammed in via exec_from_buffer.
        writeJamCmd(_os,instr.asm_(),instr.opc(),instr.num_half_bytes(),count);
      } else if (instr.valid_instr()) {
        bool bt = instr.branch_taken();  // branch taken or not
        // It looks as though Raptor is printing a bt field on any change of
        // stream, including interrupts, so we'll try that approach too, for
        // compatibility.
        bool print_bt = (bt || (instr.flags() & (ifBrUncond | ifBrCond)));
        writeICmd(_os,instr.asm_(),instr.ea(),instr.data(),instr.opc(),
                  instr.num_half_bytes(),print_ta,print_bt,bt,instr.getTrgAnno(),count,gcount);
      } else {
        // Not a valid instructin- write a fetch card only.
        writeFCmd(_os,instr.ea(),count,gcount);
      }
      break;
    case InstrEntry::Asynch:
      ++count;
      writeACmd(_os,instr.asm_());
      break;
    case InstrEntry::ExtWrite:
      writeERWCmd(_os,instr.ext_write_name(),instr.ext_write_value(),count);
      // Count not incremented for external reg-write commands.
      break;
    }
  }

  void UvpWriter::write_ir_annotation (const string& msg)
  {  
    _os<<"// " << msg;
  }

  void UvpWriter::write_ir_interrupt (const string& name)
  {  
    _os<<"[ t=>\"intr\", msg=>\""<<name<<"\"],";
  }

  void UvpWriter::write_ir_reg_update(const string& name,const RNumber& value) 
  {
    // intermediate result: register update
    _os<<"[ t=>\"reg\", rn=>\""<<name<<"\", d=>0x"<<hex<<setw(value)<<value<<"],";
  }

  void UvpWriter::write_ir_mem_update(addr_t ea,addr_t ra,bool print_ta,uint32_t wimg,uint32_t value,uint32_t type) 
  {

    // intermediate result: memory update
    if (type == memory_write) {
      _os<<"[ t=>\"store\", ra=>0x"<< hex << setw(16) << setfill('0')<<ra<<", d=>0x"<<setw(8)<<value;
      if (print_ta) {
        _os <<", ta=>0x"<<wimg;
      }
      _os <<" ],";
    } else {
      _os<<"[ t=>\"load\",  ra=>0x"<< hex << setw(16) << setfill('0')<<ra<<", d=>0x"<<setw(8)<<value;
      if (print_ta) {
        _os <<", ta=>0x"<<wimg;
      }
      _os <<" ],"; 
    }
  }

  void UvpWriter::write_ir_mmu_update(const RegSuppressHash &ignores,const string& name,addr_t epn,
                                      int way,const FieldData &fd)
  {
    _os << "[ t=>\"tlb\", rn=>\"" << name << "\", way=>" << dec << way << ", ep=>0x" << hex << epn << dec << ",\n"
        << "  d=>{" << hex;
    ForRange(fd.size(),i) {
      if (ignore_type(ignores,i) != isNone) continue;
      const FieldPair &fp = fd[i];
      _os << "{\"" << fp._name << "\",0x" << fp._value << "},";
    }
    _os << dec << "}],";
  }

  void UvpWriter::write_ir_mmu_access(const string& name,int set,int way)
  {
    _os << "// trans. hit:  " << name << ", set=" << dec << set << ", way=" << way;
  }

  void UvpWriter::writeResults(const CoreInfo* ci) 
  {  
    bool initial = true;
    _os << ci->type() << " => " << ci->name() << " {\n";
    _os.addindent(2);

    // initial state
    write_regs(initial,ci);
    write_mmu(initial,ci);
    if (ci != _root.get()) {
      write_mem(initial,ci);
    }
    // instruction stream
    write_instrs(ci);

    ForEach((ci->children()),iter) { 
      writeResults (iter->get());
    }
  
    // final state
    initial = false;
    write_regs(initial,ci);
    write_mmu(initial,ci);  
    if (ci != _root.get()) {
      write_mem(initial,ci);
    }
    _os.subindent(2);
    _os << "} // " << ci->path() << "\n";
  }

  void UvpWriter::writeResults() { 

    // write the header
    _os<<"test => "<<_testname<<" {\n";
    _os.addindent(2);
    _os << "num     = " << dec << _testcount++ << ";\n"
        << "ver     = \"\";\n"
        << "genseed = 0x1;\n"
        << "seed    = \"\";\n"
        << "genver  = {};\n"
        << '\n';
  
    // write shared memory at the global level
    write_mem(true,_root.get());
    writeResults(_root.get());
    // write shared memory at the global level
    write_mem(false,_root.get());

    _os.subindent(2);
    _os << "} \n";  
    //reset intermediate results counter
    _ircount = 0;

  }

  void UvpWriter::report_ctx_change(const IssNode *n,unsigned id,const char *name,unsigned context_num)
  {
    if (CoreInfo *ci = _curCore->switch_thread(id,context_num)) {
      _curCore = ci;
    }
  }

  void UvpWriter::report_ctx_off(const IssNode *n)
  {
    _curCore = _curCore->switch_to_parent();
  }

  void UvpWriter::report_mmu (const IssNode *n,const MmuBase &mb,unsigned set,unsigned way,
                              const FieldData &ld,bool valid,bool shared,addr_t epn, bool valid_epn, addr_t rpn, bool valid_rpn) 
  {
    updateCurr(n);
    if (!shared && (valid || show_anyway(ShowMmu))) {
      TransEntryPtr t(new TransEntry(mb.name(),mb.id(),set,way,ld,epn,valid_epn,rpn,valid_rpn));

      _curCore->add_translation(move(t),_testFinished);
    }
  }

  void UvpWriter::report_regfile (const IssNode *n,unsigned id,const char *name,int index,const RNumber& value,bool shared) 
  {
    // this is called by the model for each for each of the model's initialized regfile registers
    // convert to uvp format, ie: GPR 0 = G0
    ostringstream s;
    s << name << index;
    report_reg(n,id,s.str().c_str(),value,shared);
  }

  void UvpWriter::report_reg (const IssNode *n,unsigned id,const char *name,const RNumber& value,bool shared) 
  {  
    updateCurr(n);

    Regs &regs = _curCore->get_reglist();

    // Ignore this update if the register is to be entirely suppressed.
    if (_curCore->reg_suppress(id) == isAll) {
      return;
    }

    bool found = false;    
    ForEach (regs,iter){
      if ((*iter).name() == name) {
        (*iter).value(value);
        found = true;
        break;
      }
    }

    // We assume that if we haven't seen this register before, then this is an
    // initial value.  ie: we'll never get a final value for a register that we
    // didn't already receive an initial value for.
    if (!found) {
      RegEntry r (name,value,shared,_curCore->reg_fields(id));
      regs.push_back(r);
    }
  }

  void UvpWriter::write_mmu (bool initial,const CoreInfo *ci) 
  {
    const Translations &trans_list = (initial) ? ci->get_init_trans() : ci->get_final_trans();
    string pfx = (initial) ?  "I" : "R";
    ForEach(trans_list,iter) {
      writeTCmd(_os,ci->tlb_suppress((*iter)->id()),(*iter)->lookup(),(*iter)->set(),(*iter)->way(),
                (*iter)->_ld,pfx,(*iter)->epn(),(*iter)->valid_epn(),(*iter)->rpn(),(*iter)->valid_rpn());
    }
  }

  void UvpWriter::write_regs(bool initial,const CoreInfo *ci) 
  {
    Regs reg_list = ci->get_reglist();
    ForEach(reg_list,iter) {
      if ((*iter).shared()) {
        // do nothing
      } else if (initial) {
        writeRCmd(_os,(*iter).name(),(*iter).init_value(),(*iter).umask(),"I",(*iter).fields());
      } else {
        writeRCmd(_os,(*iter).name(),(*iter).value(),(*iter).umask(),"R",(*iter).fields());
      }
    }
  }

  void UvpWriter::write_mem(bool init,const CoreInfo *ci)
  {
    // write out all used addresses, iterating over the memQList
    // for every ra in the memQList, search the mappings in the AddrInfo
    // for a mapping.  We skip ra's that belong to other cores/threads.
    CoreInfo* root = _root.get();
  
    addr_t ra,umask;
    uint32_t data;

    if (_use_std_log) {
      // If we're using the parents' address log then print out addresses found
      // in the initial memory log.  This feature is just so that we can print a
      // UVP when the user isn't using tracing, so that we'll get addresses
      // based upon what was logged during the reading of the testcase.
      if (!ci) {
        return;
      } else {
        const MemInits &mem = _initMemLog.global_inits();
        auto iter = mem.begin();
        auto end = mem.end();

        const char *pfx = (init) ? "I : " : "R : ";
        for ( ; iter != end; ++iter ) {
          if (iter.is_init()) {
            addr_t ra = *iter;
            ci->node()->debug_data_read(data,GlobalMemId,false,ra,32);
            _os << pfx;
            writeDCmd(_os,ra,ra,data,ci->print_ta(),default_ta,0);
          }
        }        
      }
    } else {
      ForEach((*_memQList),iter) {
        if  ( ( iter->second.owner() == ci) || 
              ((ci ==  root) && (!iter->second.owner()))) {
          ;
        } else {
          continue;
        }

        ra = iter->first;

        umask =  iter->second.umask();
        unsigned id = iter->second.id();
        // write the dcard
        if (init) {
          _os<<"I : ";
          data =  iter->second.init_value(); 
        } else {
          _os <<"R : ";
          ci->node()->debug_data_read(data,id,false,ra,32);
        }
        addr_t ea; 
        unsigned ta;
        bool found_mapping = false;

        // for this used real address, lookup the mapping info.
        found_mapping = iter->second.get_mapping(ea,ta,ci);
        if (found_mapping) {
          writeDCmd(_os,ea,ra,data,ci->print_ta(),ta,umask);
        } else {
          writeDCmd(_os,ra,ra,data,ci->print_ta(),default_ta,umask);
        }   
      }
    }
  }

 
}

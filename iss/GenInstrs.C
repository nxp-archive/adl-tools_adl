//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Main generation code for all instruction stuff.
//

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <limits>
#include <assert.h>

#include "opencxx/parser/Ptree.h"
#include "opencxx/parser/PtreeUtil.h"

#include "rnumber/RNumber.h"

#include "helpers/fe_exceptions.h"
#include "helpers/Macros.h"
#include "helpers/pfxstream.h"
#include "helpers/gc_hash_map.h"
#include "helpers/CodeGen.h"
#include "helpers/StringUtils.h"
#include "helpers/BitTwiddles.h"

#include "fe/FieldSupport.h"
#include "fe/DecodeSupport.h"
#include "fe/Data.h"
#include "fe/InstrTable.h"
#include "fe/Helpers.h"

#include "asm/GenAsm.h"

#include "AttrsHelper.h"
#include "InstrWalker.h"
#include "FieldSubstWalker.h"
#include "ConfigDB.h"
#include "GenInstrs.h"
#include "GenRegs.h"
#include "MiscTypes.h"
#include "GenInstrPacket.h"
#include "GenExceptions.h"
#include "PfxFileArray.h"

using namespace std;
using namespace Opencxx;
using namespace PtreeUtil;
using namespace rnumber;

#define DEFAULT_ISS_NAMESPACE "adliss"

static const char *attr_used = " ATTRIBUTE_USED ";

namespace adl {
  
  typedef gc_vector<pair<gc_string,uint32_t> > StrUIntVect;
  typedef gc_vector<gc_string> StrVect;

  enum CodeType { cFull, cDecl, cImpl };

  gc_string getCtxControlFunc(const Context &ctx)
  {
    return ctx.name() + "_control_func";
  }

  gc_string getCtxUpdateFunc(const Context &ctx)
  {
    return ctx.name() + "_update_func";
  }

  gc_string getCommitCheckName()
  {
    return "commit_checks";
  }

  gc_string getBreakpointCheckName()
  {
    return "check_breakpoints";
  }

  gc_string getCheckForBreakPointsName()
  {
    return "checkForBreakpoint";
  }

  gc_string getCycleTally()
  {
    return "_clock_cycle_tally";
  }

  gc_string getPostFetchEnableName(const gc_string &name)
  {
    return name + "_post_fetch_enable";
  }

  gc_string getIssNamespace(const ConfigDB &config)
  {
    return (config.iss_namespace().empty() ? DEFAULT_ISS_NAMESPACE : config.iss_namespace());    
  }

  gc_string getDecodeMissLeafName()
  {
    return "illop_leaf_wrapper";
  }

  gc_string getGraphitePerfInstr()
  {
    return "_perf_instr";
  }

  gc_string getICacheTouchName(TMode tmode)
  {
    gc_string pfx;
    if (tmode == DcTrans) {
      pfx = "_dc";
    } else {
      pfx = "_jit";
    }

    return "icache_touch" + pfx;
  }

  gc_string getICacheCrossingName(TMode tmode)
  {
    gc_string pfx;
    if (tmode == DcTrans) {
      pfx = "_dc";
    } else {
      pfx = "_jit";
    }

    return "icache_line_crossing_check" + pfx;
  }

  class GenInstrBase {
  public:
    GenInstrBase (opfxstream &o,ClassData &cd,const ConfigDB &config) :
      _o(o), 
      _cd(cd),
      _config(config)
    {};

  protected:
    opfxstream     &_o;

    ClassData      &_cd;
    const ConfigDB &_config;

  public:

    bool low_mem() const
    {
      return _config.low_mem();
    }

    bool td_api() const
    {
      return _config.td_api();
    }

    bool instr_attrs() const
    {
      return _config.gen_instr_attr();
    }

    bool rollback() const
    {
      return _config.rollback_mode();
    }

    bool transactional_iss() const
    {
      return _config.iss_type() != tNormal;
    }

    bool normal_iss() const
    {
      return _config.iss_type() == tNormal;
    }

    bool logging_mem() const
    {
      return _config.mem_if() == mLogging;
    }

    bool hybrid_iss() const
    {
      return _config.iss_type() == tHybrid;
    }

    bool cached_packets() const
    {
      return _config.trans_pool_size() == 0;
    }

    bool track_reads() const
    {
      return _config.track_reads();
    }

    bool time_tagged() const
    {
      return _config.time_tagged();
    }

    bool log_td_commit() const
    {
      return td_api() && _config.log_td_commit();
    }

    bool jit_support() const
    {
      return _config.jit_support();
    }

    bool graphite_support() const
    {
      return _config.graphite_support();
    }

    bool dc_support() const
    {
      return _config.dc_support();
    }

    bool syscall_enabled() const
    {
      return _config.syscall_enabled();
    }

    bool sim_mode() const
    {
      return !_config.library_mode();
    }

    bool library_mode() const
    {
      return _config.library_mode();
    }

    bool log_reg_reads() const
    {
      return _config.log_reg_reads();
    }

    virtual bool multi_compile() const
    {
      return _config.num_compile_jobs() > 0;
    }

    gc_string headerOnly() const
    {
      return adl::headerOnly(multi_compile());
    }
 
    gc_string getMemPointsIterator(bool is_const)
    {
      return adl::getMemPointsIterator(library_mode(),is_const);
    }

    bool uses_coro_exec(const InstrSlotAlloc &isa) const
    {
      return (_config.mem_if() == mNonBlocking) && (isa._mem || isa._cache);
    }

    bool uses_coro_commit(const InstrSlotAlloc &isa) const
    {
      return (_config.mem_if() == mNonBlocking) && (isa._c_mem || isa._c_cache);
    }

    bool per_instr_td_incr() const
    {
      return _config.per_instr_td_incr();
    }

    void add_td_check(ostream &o,const gc_string &pfx,const gc_string &rc,const gc_string &mult,
                      bool pre_incr,bool post_incr,bool do_check,bool force_incr)
    {
      if (td_api()) {
        if (!time_tagged() && !graphite_support() && pre_incr && (!per_instr_td_incr() || force_incr)) {
          o << pfx << "_td_counter += (" << pfx << "_td_increment * " << mult << ") " << ";\n";
        }
        if (do_check) {
          o << "if (" << pfx << "_td_counter >= " << pfx << "_td_threshold) { " << rc << " };\n";
        }
        if (!time_tagged() && !graphite_support() && post_incr && (!per_instr_td_incr() || force_incr)) {
          o << pfx << "_td_counter += (" << pfx << "_td_increment * " << mult << ") " << ";\n";
        }
      }
    }

    bool has_decode_arg()
    {
      return !_config.decode_arg().empty();
    }

    gc_string decode_return_type(const gc_string &pfx = gc_string())
    {
      if (_config.decode_rtype().empty()) {
        return pfx + PktType + " *";
      } else {
        return _config.decode_rtype() + " ";
      }
    }

    gc_string decode_parm(bool comma)
    {
      if (!_config.decode_argtype().empty() && !_config.decode_arg().empty()) {
        return ((comma) ? ", " : "") + _config.decode_argtype() + ' ' + _config.decode_arg();
      } else {
        return gc_string();
      }
    }

    gc_string decode_arg(bool comma)
    {
      if (!_config.decode_arg().empty()) {
        return ((comma) ? ", " : "") + _config.decode_arg();
      } else {
        return gc_string();
      }
    }

    gc_string decode_return(const gc_string &name,const gc_string &arg)
    {
      if (!_config.decode_rpfx().empty()) {
        return _config.decode_rpfx() + makeFriendlyName(name) + "(" + arg + ")";
      } else {
        return arg;
      }
    }

    // If we have a post-simulation hook, we create an RAII class to generate the
    // call upon exit from a simple_run_* function.
    void writePostSim(bool has_post_sim,const gc_string &class_name)
    {
      if (!has_post_sim) return;

      _o << "struct PostSim {\n"
         << "PostSim(" << class_name << " *core) : _core(core) {};\n"
         << "~PostSim() { _core->postSim(); }\n"
         << class_name << " *_core;\n"
         << "} __post_sim__(this);\n\n";
    }

    void writeSupportFuncs(bool is_core,bool has_post_sim,const gc_string &class_name)
    {
      if (!sim_mode()) {
        return;
      }

      if (!transactional_iss()) {

        if (is_core) {

          if (time_tagged()) {
            _o << "bool isTimeTagged() const { return true; };\n\n"
               << "void addLatency(unsigned l) { _latency += l; };\n\n"
               << "ttime_t finalTime() const { return _final_time; };\n\n";
          }

          if (td_api()) {
            // The member variables are defined in the core's additional base class.
            _o << "virtual bool has_td () const { return true; };\n"          
               << "virtual uint64_t  set_td_counter(uint64_t x) { uint64_t o = _td_counter; _td_counter = x; return o; }\n\n"
               << "virtual unsigned  set_td_increment(unsigned x) { unsigned o = _td_increment; _td_increment = x; return o; }\n\n"
               << "virtual uint64_t  set_td_threshold(uint64_t x = (uint64_t)-1) { uint64_t o = _td_threshold; _td_threshold = x; return o; }\n\n"
               << "virtual void      cond_set_td_threshold(uint64_t ct,uint64_t nt) { if (_td_threshold <= ct) _td_threshold = nt; }\n\n"

               << "virtual uint64_t  get_td_counter() const { return _td_counter; };\n\n"
               << "virtual uint64_t *get_td_counter_ptr() { return &_td_counter; };\n\n"
               << "virtual unsigned  get_td_increment() const { return _td_increment; };\n\n"
               << "virtual uint64_t  get_td_threshold() const { return _td_threshold; };\n\n"

               << "uint64_t getTDCounter() const { return _td_counter; };\n\n";
          } else {
            _o << "uint64_t getTDCounter() const { return getInstrCount(); };\n\n";
          }

        }

        _o << "void simple_step()\n"
           << headerOnly()
           << "{\n"
           << "simulate();\n"
           << "}\n"
           << endHeaderOnly()

           << "ProgramStatus simple_run()\n"
           << headerOnly()
           << "{\n";
        writePostSim(has_post_sim,class_name);
        _o << "while (activeCount()) {\n";
        if (is_core) {
          add_td_check(_o,"","return ACTIVE;","1",false,true,true,true);
        }
        _o << "simulate();\n"
           << "}\n"
           << "return HALT;\n"
           << "}\n"
           << endHeaderOnly()

           << "ProgramStatus run()\n"
           << headerOnly()
           << "{\n";
        if (_config.debug_mode()) {
          _o << "try {\n";
        }
        _o << "return simple_run();\n";
        if (_config.debug_mode()) {
          _o << "}\n"
             << "catch (SimInterrupt) {\n"
             << "return BREAK;\n"
             << "}\n";
        }
        _o << "}\n"
           << endHeaderOnly();

        if (is_core) {
          _o << "bool has_sim_type(SimType type) const\n"
             << headerOnly()
             << "{\n"
             << "switch (type) {\n"
             << "case SimNormal:\n"
             << "return " << ((graphite_support()) ? "false" : "true") << ";\n"
             << "case SimDc:\n"
             << "return " << ((dc_support()) ? "true" : "false") << ";\n"
             << "case SimJit:\n"
             << "return " << ((jit_support()) ? "true" : "false") << ";\n"
             << "}\n"
             << "return false;\n"
             << "}\n\n"
             << endHeaderOnly();
        }

        _o << "ProgramStatus run(SimType type)\n"
           << headerOnly()
           << "{\n"
           << "switch (type) {\n"
           << "case SimDc: {\n"
           << "ProgramStatus ps = run_dc();\n";
        if (dc_support() && is_core) {
          _o << "DCDBG(std::cerr << \"DC Escapes           :  \" << std::dec << _dc_escapes << '\\n');\n"
             << "DCDBG(std::cerr << \"DC Compiles          :  \" << std::dec << _dc_compiles << '\\n');\n"
             << "DCDBG(std::cerr << \"DC Cold Runs         :  \" << std::dec << _cold_runs << '\\n');\n"
             << "DCDBG(std::cerr << \"DC EA Hash Collisions:  \" << std::dec << _dc_bb_hash.ea_hash_collisions() << '\\n');\n";
        }
        _o << "return ps;\n"
           << "}\n"
           << "case SimJit: {\n"
           << "ProgramStatus ps = run_jit();\n";
        if (jit_support() && is_core) {
          _o << "JDBG(std::cerr << \"JIT Threads           :  \" << std::dec << TranslationParms::mt_jit_compile() << '\\n');\n"
             << "JDBG(std::cerr << \"JIT Escapes           :  \" << std::dec << _jit_escapes << '\\n');\n"
             << "JDBG(std::cerr << \"JIT DC Compiles       :  \" << std::dec << _dc_compiles << '\\n');\n"
             << "JDBG(std::cerr << \"JIT Compiles          :  \" << std::dec << _jit.jit_compiles() << '\\n');\n"
             << "JDBG(std::cerr << \"JIT Cold Runs         :  \" << std::dec << _cold_runs << '\\n');\n"
             << "JDBG(std::cerr << \"JIT DC Runs           :  \" << std::dec << _jit_dc_runs << '\\n');\n"
             << "JDBG(std::cerr << \"JIT EA Hash Collisions:  \" << std::dec << _jit_bb_hash.ea_hash_collisions() << '\\n');\n";
        }
        _o << "return ps;\n"
           << "}\n"
           << "default:\n"
           << "return run();\n"
           << "}\n"
           << "}\n"
           << endHeaderOnly()

           << "ProgramStatus stepn(unsigned n)\n"
           << headerOnly()
           << "{\n"
           << "try {\n"
           << "for (unsigned i = 0; i != n; ++i) {\n"
           << "simulate();\n"
           << "}\n"
           << "return (activeCount()) ? ACTIVE : HALT;\n"
           << "}\n"
           << "catch (SimInterrupt) {\n"
           << "return BREAK;\n"
           << "}\n"
           << "}\n"
           << endHeaderOnly()

           << "ProgramStatus step()\n"
           << headerOnly()
           << "{\n"
           << "try {\n"
           << "simulate();\n"
           << "return (activeCount()) ? ACTIVE : HALT;\n"
           << "}\n"
           << "catch (SimInterrupt) {\n"
           << "return BREAK;\n"
           << "}\n"
           << "}\n"
           << endHeaderOnly();
      }
    }
  };

  class IssDisCall : public DisassemblerCallIF
  {
  public:
    IssDisCall(bool dis_micro_ops) : _dis_micro_ops(dis_micro_ops) {};
    
    bool disassembleMicroOps() const override { return _dis_micro_ops; };
    
    void operator()(ostream &o,const gc_string &arg,const gc_string &table_type,bool has_multiple_tables) override
    {
      o << "disassembleInternal(os,memaddr," << arg << ",false,";
      if (!has_multiple_tables || table_type.empty()) {
        o << "-1";
      } else {
        o << table_type;
      }
      o << ");\n";
    }

    bool _dis_micro_ops;
  };

  class GenInstrs : public gc, public GenInstrBase {
  public:
    GenInstrs(opfxstream &o,
              PfxFileArray *impls,
              opfxstream *jo,
              const Core &core,
              InstrWalker &walker,
              const gc_string &filename,
              Watches &watches,
              ClassData &cd,
              GenInstrPacketIF &genip,
              StrList &end_code,
              InstrSyms &instr_syms,
              const Path &path,
              const ConfigDB &config,
              const CoreConfig &coreconfig) :
      GenInstrBase(o,cd,config),
      _impls(impls),
      _jo(jo),
      _core(core), 
      _walker(walker),
      _filename(filename),
      _watches(watches),
      _genip(genip),
      _end_code(end_code),
      _coreconfig(coreconfig),
      _discall(config.disassembler()),
      _disasm(core,true,false,false,config.library_mode(),gc_string(),gc_string(),&_discall),
      _instr_syms(instr_syms),
      _path(path),
      _path_str(getJitCoreGlobal(_path)),
      _iname(gc_string("_")+AdlGenSym()->ToString()),
      _fname(gc_string("_")+AdlGenSym()->ToString()),
      _citname(0),
      _instr_table_embedded(false), //
      _class_name(getIssNamespace(_config) + "::" + getPathStr(_path)),
      _class_name_pfx(_class_name + "::"),
      _cia(0),
      _nia(0),
      _instr_count(_core.instrs().size()),
      _job_count_divisor(0),
      _min_instr_width(numeric_limits<int>::max()),
      _max_instr_width(0),
      _trans_shift(0),
      _max_dc_args(3), // Min req'd b/c decode-miss uses 3.
      _max_code_size(0),
      _min_code_size(numeric_limits<int>::max()),
      _has_commit_stack(false),
      _has_split_branches(false)
    {
      _cia = _core.cia();
      _nia = _core.nia();
      assert(_cia && _nia);

      _bt_attr        = Data::get_attr("branch_target");

      unsigned j = _config.num_compile_jobs();
      if (j) {
        _job_count_divisor = _instr_count / j;
        if (_instr_count % j) {
          ++_job_count_divisor;
        }
      }
    
      _walker.set_iname(_iname);  // Copy the instruction buffer symbol to InstrWalker class
    
    };

  private:

    PfxFileArray     *_impls;
    opfxstream       *_jo;
    const Core       &_core;
    InstrWalker      &_walker;
    const gc_string     &_filename;
    Watches          &_watches;
    GenInstrPacketIF &_genip;
    StrList          &_end_code;
    const CoreConfig &_coreconfig;

    IssDisCall        _discall;
    GenDisassembler   _disasm;        // For generating disassembler functions.
    InstrSyms        &_instr_syms;

    const Path       &_path;
    const gc_string   _path_str;
  
    gc_string         _iname;          // Actual member variable for current instruction op.

    gc_string         _fname;          // Symbol for current instruction fetched
    Ptree            *_citname;       // Symbol for current instruction table identifier.
    bool              _instr_table_embedded; // Indicate the instruction table is for micro instructions or not
  
    gc_string          _class_name;
    gc_string          _class_name_pfx;

    const Reg        *_cia;
    const Reg        *_nia;

    int               _bt_attr;

    unsigned          _instr_count;
    unsigned          _job_count_divisor;
    unsigned          _min_instr_width;
    unsigned          _max_instr_width;
    unsigned          _trans_shift;
    unsigned          _max_dc_args;
    unsigned          _max_code_size;
    unsigned          _min_code_size;

    bool              _has_commit_stack;
    bool              _has_split_branches;

    enum { DefaultLatency = 1 };

  public:

    bool trans_rethrow() const
    {
      return _config.trans_rethrow();
    }

    bool dmi_cache_support() const
    {
      return _coreconfig.dmi_cache_support();
    }

    bool multi_issue() const
    {
      return _coreconfig.issue_width() > 1;
    }

    int issue_width() const
    {
      return _coreconfig.issue_width();
    }

    Ptree *tracing_on() const
    {
      return _coreconfig.tracing_on();
    }

    Ptree *post_sim() const
    {
      return _coreconfig.post_sim();
    }

    Ptree *compl_time() const
    {
      return _coreconfig.compl_time();
    }

    Ptree *tracing_off() const
    {
      return _coreconfig.tracing_off();
    }

    double clock_cycle_ratio() const
    {
      return _coreconfig.clock_cycle_ratio();
    }

    bool have_clock_cycle_ratio() const
    {
      return (clock_cycle_ratio() != 0);
    }

    bool need_values() const
    {
      return hybrid_iss() && _coreconfig.need_packet_values();
    }

    bool need_read_op(const Instr &instr) const
    {
      return (need_values() && _coreconfig.readOpInstrs().count(instr.name()));
    }

    bool have_dt() const
    {
      return _config.dep_tracking();
    }

    const gc_string &fetch_name() const
    {
      return _iname;
    }

    // Returns true if the instruction is indicated to be the start of a split
    // branch.  This is indicated by being in the the split_branch_begin list in a
    // .ttc file.
    bool is_split_branch_begin(const Instr &instr) const
    {
      return (_coreconfig.split_branch_begin().count(instr.name()));
    }

    bool is_split_branch_end(const Instr &instr) const
    {
      return (_coreconfig.split_branch_end().count(instr.name()));
    }

    bool is_split_branch_cancel(const Instr &instr) const
    {
      return (_coreconfig.split_branch_cancel().count(instr.name()));
    }

    // We accept both instruction names and attribute names here.
    bool explicit_bb_end(const Instr &instr) const
    {
      // See if the instruction name is listed.
      if (_coreconfig.bb_end_instrs().count(instr.name())) return true;

      // Barring that, look at the attributes.
      ForEach(instr.attrs(),i) {
        if (const char *cn = Data::get_attr_name(i->first)) {
          if (_coreconfig.bb_end_instrs().count(cn)) return true;
        }
      }

      return false;
    }

    bool basic_block_end(const Instr &instr,const InstrSym &sym) const
    {
      return sym.is_branch() || sym.halts() || explicit_bb_end(instr);
    }

    // We accept both instruction names and attribute names here.
    bool optional_basic_block_end(const Instr &instr) const
    {
      // See if the instruction name is listed.
      if (_coreconfig.opt_bb_end_instrs().count(instr.name())) return true;

      // Barring that, look at the attributes.
      ForEach(instr.attrs(),i) {
        if (const char *cn = Data::get_attr_name(i->first)) {
          if (_coreconfig.opt_bb_end_instrs().count(cn)) return true;
        }
      }

      return false;
    }

    virtual bool multi_compile() const
    {
      return _impls && _config.num_compile_jobs() > 0;
    }

    bool mt_support() const
    {
      return _config.mt_support();
    }

    bool rw_locked_mem() const
    {
      return mt_support() && _config.mt_rw_locked_mem();
    }
  
    // An instruction needs to be atomic (grab a lock) if we're adding
    // multi-threading support and it sources/targets resources shared with
    // event-buses (which is what provides the multi-core communications
    // facility).
    bool is_atomic_instr(const InstrSym &sym)
    {
      return mt_support() && sym.uses_eb_res();
    }

    void writeCurLocation(opfxstream &o)
    {
      adl::writeCurLocation(o,_config,_filename);
    }

    void writeCurLocation()
    {
      writeCurLocation(_o);
    }

    void writeLineDirective(Environment *env,Ptree *expr)
    {
      adl::writeLineDirective(_o,_config,env,expr);
    }

    void writeLineDirective(opfxstream &o,Environment *env,Ptree *expr)
    {
      adl::writeLineDirective(o,_config,env,expr);
    }

    Ptree *funcBody(Environment *e,Ptree *f)
    {
      return adl::funcBody(_walker,e,f);
    }

    void genFunc(const gc_string &fname,Environment *env,Ptree *func,
                 bool isConst = false,bool isUsed = false)
    {
      adl::genFunc(_o,_config,_walker,_filename,fname,env,func,isConst,isUsed);
    }

    int get_latency(const Instr &instr) const
    {
      int latency = _coreconfig.get_latency(instr,DefaultLatency);
      if (_config.verbose_mode()) {
        cout << "Latency:  " << instr.name() << " = " << latency << "\n";
      }
      return latency;
    }

    // Note: We specifically allow for 0 latencies in order to emulate
    // instructions which are folded out by the micro-architecture.  We actually
    // allow for negative latencies, just to be able to handle weird stuff.
    int get_extra_td_latency(const Instr instr)
    {
      if (log_td_commit()) {
        int latency = get_latency(instr);
        if (latency != 1) {
          return latency - 1;
        }
      }
      return 0;
    }

    bool have_instr_attrs() const
    {
      return !_core.instr_attrs().empty();
    }

    bool disassembler() const
    {
      return _config.disassembler();
    }

    bool has_cycle_handler() const
    {
      return _core.pre_cycle_handler() || _core.post_cycle_handler();
    }

    bool instr_little_endian() const
    {
      return _core.instrEndianness() == Core::Little;
    }

    // If we've been configured to support breaking up ISS compilation into
    // multiple jobs, then this specifies what job we're in by looking at an
    // instruction count vs. the total number.  Returns -1 if this feature is not
    // enabled.
    int get_job_num(unsigned count)
    {
      if (multi_compile()) {
        return count / _job_count_divisor;
      }
      return -1;
    }

    const gc_string &qualifyName(gc_string &name,CodeType ctype)
    {
      if (ctype == cImpl) {
        name += " " + _class_name_pfx;
      }
      return name;
    }

    bool haveExceptions() const
    {
      return !_core.exceptions().empty();
    }

    bool postFetchSkip() const 
    {
      return _core.post_fetch_skip();
    }

    bool skip_field(const Instr::Field fp) 
    {
      const InstrField &field = *(fp._field);
      return (field.is_overlay() || field.is_variable() || fp._type == Instr::Field::Resrvd);  
    }

    void genSingleIssueInstrAttrFunc()
    {
      // We have instruction attributes- we need a member variable in which to
      // store the current value.
      _cd._member_vars.push_back(getInstrAttrType(_core) + ' ' + getInstrAttrVar());
      _cd._reset_stmts.push_back(getInstrAttrVar() + " = 0");
      _cd._constr_inits.push_back(getInstrAttrVar() + "(0)");

      _o << "bool instrHasAttr(InstrAttrs x) {\n"
         << "return " << getInstrAttrVar() << " & x;\n"
         << "}\n\n"
         << getInstrAttrType(_core) << ' ' << getInstrAttrGetter() << "() const {\n"
         << "return " << getInstrAttrVar() << ";\n"
         << "}\n\n";
    }

    void genInstrAttrFunc()
    {
      if (normal_iss() && !time_tagged()) {
        genSingleIssueInstrAttrFunc();
      } else if (time_tagged()) {
        // For a time-tagged ISS, we create two forms: One which returns the
        // current instruction's attributes and one which returns a prior
        // instruction's attributes, based upon the issue width.
        if (multi_issue()) {
          // For the multi-issue form, store attributes in an array, based upon
          // the issue-count index.
          _cd._member_vars.push_back("std::vector<" + getInstrAttrType(_core) + "> " + getInstrAttrVar());
          gc_string rstmt, cinit;
          MkGStr(rstmt,getInstrAttrVar() << " = std::vector<" << getInstrAttrType(_core) << ">(" << issue_width() << ",0)");
          _cd._reset_stmts.push_back(rstmt);
          MkGStr(cinit,getInstrAttrVar() << "(" << issue_width() << ",0)");
          _cd._constr_inits.push_back(cinit);

          _o << "bool instrHasAttr(InstrAttrs x) {\n"
             << "return " << getInstrAttrVar() << "[_issue_count] & x;\n"
             << "}\n\n"
             << getInstrAttrType(_core) << ' ' << getInstrAttrGetter() << "() const {\n"
             << "return " << getInstrAttrVar() << "[_issue_count];\n"
             << "}\n\n";

          _o << "bool instrHasAttr(unsigned index,InstrAttrs x) {\n"
             << "assert (index < " << issue_width() << ");\n"
             << "return " << getInstrAttrVar() << "[index] & x;\n"
             << "}\n\n"
             << getInstrAttrType(_core) << ' ' << getInstrAttrGetter() << "(unsigned index) const {\n"
             << "assert (index < " << issue_width() << ");\n"
             << "return " << getInstrAttrVar() << "[index];\n"
             << "}\n\n";
        } else {
          // For single-issue, just generate the standard function, with a form
          // that's compatible with the multi-issue form.
          genSingleIssueInstrAttrFunc();

          _o << "bool instrHasAttr(unsigned index ATTRIBUTE_UNUSED,InstrAttrs x) {\n"
             << "return instrHasAttr(x);\n"
             << "}\n\n"
             << getInstrAttrType(_core) << ' ' << getInstrAttrGetter() << "(unsigned index ATTRIBUTE_UNUSED) const {\n"
             << "return " << getInstrAttrGetter() << "();\n"
             << "}\n\n";
        }

      } else {
        _o << "bool instrHasAttr(InstrAttrs x) {\n"
           << "return (" << PktName << ") ? (" << PktName << "->" << getInstrAttrVar() << " & x) : false;\n"
           << "}\n\n"
           << getInstrAttrType(_core) << ' ' << getInstrAttrGetter() << "() const {\n"
           << "return (" << PktName << ") ? (" << PktName << "->" << getInstrAttrVar() << ") : 0;\n"
           << "}\n\n";
      }
    }

    // Generate support types for instruction attributes.
    void generateInstrAttrs()
    {
      const IntSet &instr_attrs = _core.instr_attrs();
      const IntSet &mmu_instr_attrs = _core.mmu_instr_attrs();
      if (!instr_attrs.empty()) {
        uint64_t flag = 1;
        uint64_t mmu_mask = 0;
        // The MMU instruction attributes are listed first, in order to put them
        // into the low bits, so that they can occupy the offset bits within a
        // translation.
        _o << "enum InstrAttrs {\n" << hex;
        gc_string sfx = (instr_attrs.size() > 32) ? "ull" : "";
        ForEach(mmu_instr_attrs,iter) {
          _o << getInstrAttrName(Data::get_attr_name(*iter)) << " = 0x" << flag << sfx << ",\n";
          flag <<= 1;
        }
        mmu_mask = (flag-1);
        ForEach(instr_attrs,iter) {
          if (!mmu_instr_attrs.count(*iter)) {
            _o << getInstrAttrName(Data::get_attr_name(*iter)) << " = 0x" << flag << sfx << ",\n";
            flag <<= 1;
          }
        }
        _o << "};\n\n" << dec;

        genInstrAttrFunc();

        // This returns just the MMU attributes.
        _o << getInstrAttrType(_core) << ' ' << getMmuInstrAttrGetter() << "() const {\n";
        if (mmu_instr_attrs.empty()) {
          _o << "return 0;\n";
        } else {
          _o << "return (" << getInstrAttrGetter() << "() & 0x" << hex << mmu_mask << dec << ");\n"; 
        }
        _o << "}\n";
      }
    }

    // Generate a checker for the current instruction table.
    void genITableCheck()
    {
      if (Ptree *itable = _core.itable_watch()) {
        Ptree *newa = funcBody(_core.env(),itable);
        Ptree *sym1 = AdlGenSym();
        Ptree *sym2 = AdlGenSym();
        _o << getInstrTableType() << " " << sym1 << "() " << newa << "\n\n"
           << "void " << sym2 << "() {\n"
           << "setCurrentInstrTable(" << sym1 << "());\n"
           << "}\n\n";
        _watches.push_back(Watch(_walker.regs_referenced(),sym2));
      }
    }

    void generateInstrTables()
    {
      const InstrTables &tables = _core.instrtables();
      const InstrHash &instrs = _core.instrs();

      if (tables.empty()) {
        RError ("No instruction tables found for this core.");
      }

      // Scan instructions for maximum size.
      unsigned max_width = 0;
      ForEach(instrs,i) {
        if (i->second->width() > max_width) {
          max_width = i->second->width();
        }
      }

      // Only generate multiple-instruction table information if
      // we actually have multiple instruction tables.
      if (tables.size() > 1) {
        _o << "//\n"
           << "// Instruction tables.\n"
           << "//\n";
        // Generate enums identifying instruction tables.
        _o << "enum " << getInstrTableType() << " {";
        bool first = true;
        ForEach(tables,i) {
          unsigned c = i->first;
          list_sep(_o,first);
          if (const char *n = Data::get_attr_name(c)) {
            _o << n << " = " << c;
          } else {
            RError("Unknown instruction class identifying instruction table:  " << c);
          }
        }
        _citname = AdlGenSym();
        _o << "};\n";

        gc_ostringstream ss;
        ss << getInstrTableType() << " " << _citname;
        _cd._member_vars.push_back(ss.str());

        // Get the first instruction table and use that as the reset state.
        gc_string first_table;
        ForEach(tables,i) {
          if ( i->second._pos == 0) {
            first_table = Data::get_attr_name(i->second._id);
            break;
          }
        }
        assert(first_table.size());

        gc_string init,reset;
        MkGStr(init,_citname << "(" << first_table << ")");
        MkGStr(reset,_citname << " = " << first_table);
        _cd._constr_inits.push_back(init);        
        _cd._reset_stmts.push_back(reset);
        
        // If we have an instruction-table-watch function, generate that.
        genITableCheck();

        // Now generate the switching function.
        _o << "void setCurrentInstrTable(" << getInstrTableType() << " x) {\n"
           << _citname << " = x;\n"
           << "}\n\n";
	
        // Now generate the accessor function
        _o << getInstrTableType() << " getCurrentInstrTable() const {\n"
           << "return " << _citname << ";\n"
           << "}\n\n";


      }
    }

    void writeLogCommit(ostream &o)
    {
      if (mt_support()) {
        // If generating an MT-capable model, we want to track the end of
        // instructions so that the MT logger knows how to time-stamp and order
        // them.  We need to do this for exceptions, since that ends an
        // instruction.
        o << "if (Tracing && (trace_mode() & ttInstrPrefetch)) {\n"
          << "_logger->log_commit(0);\n"
          << "}\n";
      }
    }
  
    // Generates the exception handling logic.
    void writeExceptionCheck(bool td_incr,bool td_check,bool gen_catch,bool gen_label,bool log_commit)
    {
      // Emit no code if we have no exceptions.
      if (haveExceptions()) {
        gc_string eflag = getExceptionFlag();
        if (gen_catch) {
          // The catch block handles exceptions generated by a raiseException
          // command.  We then fall to the checking code, which checks for any
          // exceptionsm, since exceptions can be set by the setException routine.
          _o << "catch (Exception current_exception) {\n"
             << "setException(current_exception);\n"
             << "}\n";
        }
        _o << "if (" << eflag << ") {\n";
        if (mt_support()) {
          _o << getExceptionLock() << ";\n";
        }
        if (gen_label) {
          _o << "GenerateException:\n";
        }

        // Note: For the code below, we're only clearing the exceptions we know
        // existed when we enter the loop, in case exception processing generates
        // further exceptions.
        _o << "unsigned original_exception_flag = " << eflag << ";\n"
           << "genExceptionsInternal(" << eflag << ");\n"
           << eflag << " &= ~original_exception_flag;\n";
        writeInstrAttrClear(_o);
        if (log_commit) {
          writeLogCommit(_o);
        }
        if (td_check || td_incr) {
          add_td_check(_o,gc_string(),"return ACTIVE;","1",td_incr,false,td_check,true);
        }
        _o << "}\n";
      }
    }
    
    // Generates post exec logic
    void writePostExec() 
    {
      // Insert any code to be run after the instruction is executed.
      if (_core.post_exec_handler()) {
        _o << "post_exec_handler();\n";
      }

      if (_core.parallel_execution() && _core.has_prefix_instruction()) {
        _o << "packetPositionReset();\n";
      }
      if (_core.post_cycle_handler()) {
        writeCycleRatioCall(_o,"post_cycle_handler");
      }
      if (have_clock_cycle_ratio()) {
        // Insert code to update the tally here.
        gc_string cc = getCycleTally();
        _o << "if (" << cc << " >= 1) {\n"
           << "double n;\n"
           << cc << " = modf(" << cc << ",&n);\n"
           << "}\n";
      }
    
      writeLogTdCommit(_o,"",false);
    }    

    void generateFwdDecls()
    {
      const InstrHash &instrs = _core.instrs();
      
      ForEach(instrs,i) {
        const Instr &instr = *i->second;
        if (!instr.is_prefix()) {
          auto x = _instr_syms.find(instr.name());
          assert(x != _instr_syms.end());
          InstrSym &sym = x->second;
          writeJitTranslationHandlerDecl(_o,instr,sym,cDecl);
          _o << ";\n\n";
        }
      }
    }

    void generateJitFwdDecls()
    {
      if (jit_support()) {
        generateFwdDecls();
      }
    }

    void generateHalt() 
    {
      if (_core.parallel_execution() && !transactional_iss()) {
        _o << getCommitQueueName() << ".commit();\n"
           << "_packet_position_reset = false;\n";
      }
    }

    gc_string getStageName(const gc_string &s)
    {
      return "_" + s + "_stage";
    }

    void genTimeTagStages()
    {
      StrSet stages;
      ForEach(_coreconfig.groups(),i) {
        const CoreConfig::Group &group = i->second;
        ForEach(group.stages(),s) {
          stages.insert(s->name());
        }
      }

      ForEach(stages,i) {
        gc_string sn = getStageName(*i);
        _cd._member_vars.push_back("ttime_t "+sn);
        _cd._constr_inits.push_back(sn+"(0)");
        _cd._reset_stmts.push_back(sn+" = 0");
      }

      // This is a pseudo stage which holds the final completion time of the
      // instruction.
      gc_string cn = "_compl_time";
      _cd._member_vars.push_back("ttime_t "+cn);
      _cd._constr_inits.push_back(cn+"(0)");
      _cd._reset_stmts.push_back(cn+" = 0");
    }

    // Write logic associated with time-tagged ISSs.
    void writeTimeTagGroupStart(const gc_string &name,unsigned latency)
    {
      if (time_tagged()) {

        _o << "void time_tag_start_" << name << "()\n"
           << "{\n";

        _o << "start_instr(" << latency << ");\n"
           << "}\n\n";

      }
    }

    void writePrintTimeTagStages(const gc_string &name,const CoreConfig::Stages &stages)
    {
      if (_config.trace_mode()) {
        _o << "if (Tracing && DebugMode) {\n";
        size_t len = 0;
        ForEach(stages,s) {
          len = max(s->name().size(),len);
        }
        _o << "std::cout\n";
        ForEach(stages,s) {
          _o << " << \"\\t" << s->name() << gc_string(len-s->name().size(),' ') << ":  \" << " << getStageName(s->name()) << " << \"\\n\"\n";
        }
        _o << " << \"\\n\";\n"
           << "}\n\n";
      }
    }

    // This writes the finish logic for a time-tagged ISSs.  The issue time is
    // based upon the times associated with the operands.  We then update times
    // for each stage based upon if there were no stalls and if a previous
    // instruction caused a stall, thus delaying this instruction.  Basically, for
    // that, we just take the max of the time in which the instruction could
    // enter, based upon its issue time, and when it could enter based upon the
    // previous instruction's egress time from that stage.
    void writeTimeTagGroupFinish(const gc_string &name,const CoreConfig::Stages &stages,const AuxFunc *allow_issue)
    {
      _o << "void time_tag_finish_" << name << "(const char *iname)\n"
         << "{\n";

      if (multi_issue()) {
        _o << "if ((_issue_count == 0) || (current_time() != _prior_issue_time)";
        if (allow_issue) {
          _o << " || !" << allow_issue->name() << "(_issue_count)";
        }
        _o << ") {\n"
           << "advance_current_time(" << getStageName(stages.front().name()) << "+1);\n";

        if (have_instr_attrs()) {
          _o << getInstrAttrVar() << "[0] = " << getInstrAttrVar() << "[_issue_count];\n";
        }

        _o << "_issue_count = 0;\n"
           << "}\n";
      } else {
        _o << "advance_current_time(" << getStageName(stages.front().name()) << "+1);\n";
        if (_core.has_caches()) {
          _o << "clear_cache_latency();\n";
        }
      }

      _o << "ttime_t issue_time = std::max(_td_counter,current_time());\n\n"

         << "if (Tracing && DebugMode) {\n"
         << "std::cout << \"Time:  \" << std::dec << issue_time << \"\\t#\" << _instr_count << \" 0x\" << std::hex << " 
         << _cia->name() << "_read().uint64() << \":\" << iname << std::dec << \" (\"";
      if (multi_issue()) {
        _o << " << \"issue-count: \" << _issue_count << \", \"";
      }
      _o << "\"op-avail-time: \" << current_time() << \", initial-time: \" << _td_counter << \")\\n\";\n"
         << "}\n\n";

      if (multi_issue()) {
        _o << "_prior_issue_time = issue_time;\n";
      }

      auto issue_stage = stages.begin();
      ForEach(stages,s) {
        if (s->tag() == "issue") {
          issue_stage = s;
        }
      }

      // We subtract 1, since a one cycle latency is implicit between stages.
      _o << "unsigned total_instr_latency = _cache_latency + _latency - 1;\n";

      if (multi_issue()) {
        _o << "if (_issue_count == 0) {\n";
      }

      gc_string memlat;
      int count = 0;
      auto exec_stage = stages.end();
      ForEach(stages,s) {
        gc_string next = ( (s+1) == stages.end() ) ? "_compl_time" : getStageName((s+1)->name());
        gc_string sn = getStageName(s->name());
     
        _o << sn << " = std::max(issue_time+" << count++ << "," << next << ")" << memlat << ";\n";    

        // Memory and instruction latency will be applied on the next stage, so
        // that we end up tracking egress times rather than entry times.
        if (s->tag() == "exec") {
          memlat = "+ total_instr_latency";
          exec_stage = s;
        }
      }

      if (multi_issue()) {
        _o << "_max_latency = total_instr_latency;\n"
           << "} else if (total_instr_latency > _max_latency) {\n";

        // For multi-issue, if we're not issuing the first instruction, then we
        // now use the max latency discovered so far, and only add this on to
        // stages which follow the exec stage.  This way, for example, a longer
        // latency instruction, such as a multiply, will add on its latency
        // properly.  Yes, this is slightly incorrect, in that the first add
        // should have been stalled, but oh, well, that's not something we can
        // easily handle, since we're executing instructions serially.
      
        if (exec_stage != stages.end()) {
          // Only worry about this if there's an exec stage.
          for (auto s = exec_stage+1; s != stages.end(); ++s) {
            gc_string sn = getStageName(s->name());         
            _o << sn << " += (total_instr_latency - _max_latency);\n";
          }
        }

        _o << "_max_latency = total_instr_latency;\n"
           << "}\n\n"
           << "_compl_time = " << getStageName((stages.end()-1)->name()) << "+1;\n";
      } else {
        _o << "_compl_time = std::max(_compl_time+1,issue_time+" << count << ")" << memlat << ";\n\n";
      }

      _o << "ttime_t log_issue_time = " << getStageName(issue_stage->name()) << ";\n";

      if (_config.trace_mode()) {
        _o << "if (Tracing && (_trace_mode & ttInstrTimes)) {\n"
           << "_logger->log_instr_issue_time(log_issue_time);\n"
          // We subtract one, because we record the completion time as when the
          // instruction is in the final stage, e.g. in the write-back stage.
           << "_logger->log_instr_completion_time(_compl_time-1);\n"
           << "}\n\n";
      }
      _o << "update_completion_time();\n";

      writePrintTimeTagStages(name,stages);

      if (multi_issue()) {
        _o << "if (++_issue_count >= " << issue_width() << ") {\n"
           << "_issue_count = 0;\n"
           << "}\n\n";
      }

      if (_core.has_caches()) {
        _o << "clear_cache_latency();\n";
      }

      _o << "}\n\n";
    }

    // Write logic associated with time-tagged ISSs.  A function is created for
    // each group which updates the specific set of stages associated with that
    // group.
    void writeTimeTagLogic()
    {
      if (time_tagged()) {

        // Create the pipeline-stage counters.  We create a set of all known
        // stages, then instantiate them.
        genTimeTagStages();      

        if (multi_issue()) {
          // If we're multi-issue, then we use a counter to specify how many
          // instructions have been issued at once.
          _cd._member_vars.push_back("unsigned _issue_count");
          _cd._constr_inits.push_back("_issue_count(0)");
          _cd._reset_stmts.push_back("_issue_count = 0");

          _cd._member_vars.push_back("unsigned _max_latency");
          _cd._constr_inits.push_back("_max_latency(0)");
          _cd._reset_stmts.push_back("_max_latency = 0");

          _cd._member_vars.push_back("unsigned _prior_issue_time");
          _cd._constr_inits.push_back("_prior_issue_time(0)");
          _cd._reset_stmts.push_back("_prior_issue_time = 0");
        }

        ForEach(_coreconfig.groups(),i) {
          const CoreConfig::Group &group = i->second;

          if (!group.stages().empty()) {
            writeTimeTagGroupStart(group.name(),group.latency());
            writeTimeTagGroupFinish(group.name(),group.stages(),group.allow_issue()._d);
          }
        }
      }
    }

    void writeTimeTagFinishCall(const Instr &instr,const char *pfx)
    {
      if (time_tagged()) {
        if (auto grp = _coreconfig.get_group(instr)) {
          _o << pfx << "time_tag_finish_" << grp->name() << "(\"" << instr.name() << "\");\n";
        } else {
          RError("Instruction '" << instr.name() << "' not covered by a group when creating a time-tagged ISS.");
        }
      }
    }

    gc_string getInstrFieldName(const InstrField &field,const SubInstrGrp *subinstr,bool orig)
    {
      const InstrField* f = field.interface() ? field.interface() : &field;
      gc_string n = f->name();
      if (subinstr) {
        n += "_" + subinstr->name();
      }
      if (is_special(field) && orig) {
        return f->name() + "_p";
      }
      return n;
    }

    // Writes the arguments needed by a handler function.
    void writeInstrHandlerArgs(ostream &o,const InstrBase &instr,bool &first,bool orig)
    {
      // List all arguments- the instruction fields.
      int numfields = instr.sizeFields();
      // Grab all instruction field values.
      int argCount=0;
      for (int i = 0; i != numfields; ++i) {
        Instr::Field fp = instr.getField(i);
        const InstrField &field = *(fp._field);
        // variable fields use for opcode
        if (skip_field(fp)) {  
          continue;
        }
        list_sep(o,first);
        o << getInstrFieldType(field,_nia->width(),orig,false) << " " << getInstrFieldName(field,fp._subinstr,orig);
        argCount++; 
      }     
    }

    // Returns true if any instruction fields for this instruction
    // need modification.
    bool hasModifiedFields(const InstrBase &instr)
    {
      int numfields = instr.sizeFields();
      for (int i = 0; i != numfields; ++i) {
        Instr::Field fp = instr.getField(i);
        const InstrField &field = *(fp._field);
        // variable fields use for opcode
        if (skip_field(fp)) {  
          continue;
        }
        if (is_special(field)) {
          return true;
        }
      }
      return false;
    }

    // Writes out a list of fields as arguments to an instruction handler.  Note
    // that 'type' is optional and will write out a parameter list if present.
    int writeInstrArgs(ostream &o,const InstrBase &instr,const gc_string &type,bool orig)
    {
      // List all arguments- the instruction fields.
      int numfields = instr.sizeFields();
      // Grab all instruction field values.
      bool first = true;
      int numargs = 0;
      for (int i = 0; i != numfields; ++i) {
        Instr::Field fp = instr.getField(i);
        const InstrField &field = *(fp._field);
        // variable fields use for opcode
        if (skip_field(fp)) {  
          continue;
        }
        list_sep(o,first);
        if (!type.empty()) {
          o << type << " ";
        }
        o << getInstrFieldName(field,fp._subinstr,orig);
        ++numargs;
      }
      return numargs;
    }

    struct InstrFieldModType {
      const gc_string _type, _fname, _value;

      InstrFieldModType(const gc_string &t,const gc_string &f,const gc_string &v) :
        _type(t), _fname(f), _value(v) {};
    };

    InstrFieldModType getInstrFieldModifier(const InstrBase &instr,const InstrField &field,
                                            bool add_field_type,const gc_string &sfx,const char *cia = 0)
    {
      gc_string fname = field.interface() ? field.interface()->name() : field.name();
      gc_string value = fname + sfx;
      
      gc_string type = getInstrFieldType(field,_nia->width(),false,true);

      int width = field.width();

      if (add_field_type) {
        value = getInstrFieldType(field,field.width(),false,true) + "(" + value + ")";
      }

      if (field.is_inverted()) {
        value = "(~" + value + ")";
      }

      if (field.is_signed()) {
        value = value + ".signExtend<" + uitogs(_nia->width()) + ">()";
        width = _nia->width();
      }

      // For efficiency, we resize via signExtend before shifting.  However, if we didn't
      // have to sign-extend, then we have to expand the value for the shift.
      if (field.shift()) {
        if (!field.is_signed()) {
          width += field.shift();
          value = "bits<" + uitogs(width) + ">(" + value + ")";
        }
        value = "(" + value + " << " + uitogs(field.shift()) + ")";
      }

      // Same for the offset- expand the value if necessary, resize and add on the
      // offset.
      if (field.offset()) {
        if (!field.is_signed()) {
          width = _nia->width();
          value = "bits<" + uitogs(width) + ">(" + value + ")";
        }
        value = "(" + value + " + " + uitogs(field.offset()) + ")";
      }
      
      if (field.addr() == InstrField::aPcRel) {
        if (transactional_iss() && !hybrid_iss()) {
          const InstrSlotAlloc &isa = _genip.instr_slots()->find(instr.name())->second;
          // For a transactional ISS, we need to read from the appropriate slot.
          auto x = isa.getSlot(_cia->name());
          value = "(" + value + " + " + makeSlotRef(x.first->_start,0,false,true,false,_genip.instrPacketWidth()) + ")";
        } else if (cia) {
          value = "(" + value + " + " + cia + ")";
        } else {
          value = "(" + value + " + " + _cia->name() + "_read())";
        }
      }
      return InstrFieldModType(type,fname,value);
    }

    // Writes any code to transform a field into the value actually seen by the action code.
    void writeInstrFieldModifiers(ostream &o,const InstrBase &instr, const char* preinsert = "", 
                                  const char* postinsert = "",const char *cia = 0)
    {
      // List all arguments- the instruction fields.
      int numfields = instr.sizeFields();
      // Grab all instruction field values.
      for (int i = 0; i != numfields; ++i) {
        Instr::Field fp = instr.getField(i);
        const InstrField &field = *(fp._field); 
        if (!is_special(field)) {
          continue;
        }

        auto r = getInstrFieldModifier(instr,field,false,"_p",cia);

        o << preinsert << r._type << " " << r._fname << " = " << r._value << postinsert << ";\n";
      }
    }

    // Writes arguments for a call to an aliased instruction's handler.
    void writeAliasInstrHandlerCall(ostream &o,const Instr &instr,const Instr::Alias *alias,const gc_string &extra = "")
    {
      o << "(" << extra;
      // We create a call to the aliased function's handler 
      // with the proper arguments.
      bool first = (extra.empty());

      ForEach(alias->_fields,i) {
        const Instr::Field &fp = *i;
        if (fp._field && fp._field->is_variable()) {  // variable fields use for opcode
          continue;
        }

        if (!first) {
          o << ", ";
        }
        first = false;
        if (const InstrField *f = aliasFieldTarget(o,_walker,instr,fp)) {
          o << f->name();
        }
      }
      o << ");\n";
    }

    void checkItemIsBranch(bool &is_branch,bool &is_cond,bool &is_halts,const UsageInfo &ui,StrSet &visited_helpers)
    {
      // First check the instruction.
      if (ui._halts) {
        is_halts = true;
      }

      ForEach(ui._trg_regs,i) {
        if ( i->regb() == _nia || i->regb()->has_attr(_bt_attr)) {
          is_branch = true;
          is_cond = i->_cond;
          return;
        }
      }

      // Then check helpers.  We track what we've already processed in order to
      // prevent run-away recursion.
      ForEach(ui._helpers,iter) {
        if (!visited_helpers.count(*iter)) {
          visited_helpers.insert(*iter);
          checkItemIsBranch(is_branch,is_cond,is_halts,_core.helpers().find(*iter)->second,visited_helpers);
        }
      }
      // TODO:
      // checkItemIsBranch(is_branch,is_cond,is_halts,_core.post_exec_handler());
    }

    // Checks whether this is a branch by seeing if we access the NIA.  We mark
    // the instruction as a conditional branch if the NIA is conditionally
    // updated.  Otherwise, it's an unconditional branch.  We also check to see if
    // we're dealing with a pc-relative branch with or without a shift.  We also
    // check for split branches by checking for the split_branch_1 and
    // split_branch_2 attributes, which indicate the first and second parts of a
    // split branch.
    void checkBranchTypeInstr(const Instr &instr, InstrSym &sym)
    {
      bool is_branch = false;
      bool is_cond   = false;
      bool is_halts  = false;

      // This checks the instruction and any helpers to see if we're dealing
      // with a branch or a halt instruction.
      StrSet visited_helpers;
      checkItemIsBranch(is_branch,is_cond,is_halts,instr.usageinfo(),visited_helpers);

      sym._halts = is_halts;

      if (is_split_branch_begin(instr)) {
        sym._sbType = InstrSym::sbFirst;
        _has_split_branches = true;
      } 

      // If not, we're done.
      if (!is_branch && !sym.is_sb_1st()) {
        return;
      }

      bool fixed_branch = false;

      // List all arguments- the instruction fields.
      int numfields = instr.sizeFields();
      // Grab all instruction field values.
      for (int i = 0; i != numfields; ++i) {
        Instr::Field fp = instr.getField(i);
        const InstrField &field = *(fp._field); 
        // If we find a field that's an addressing field, we assume that this is
        // what's used to determine a branch target.
        if (field.addr() == InstrField::aPcRel || field.addr() == InstrField::aAbs ) {
          sym._pc_rel_instr = (field.addr() == InstrField::aPcRel);
          sym._shift = field.shift();
          sym._offset = field.offset();
          sym._signed = field.is_signed();
          sym._br_op = &field;
          fixed_branch = true;
        }
      }

      if (is_split_branch_end(instr)) {
        sym._sbType = (is_cond) ? InstrSym::sbSecondCond : InstrSym::sbSecondUncond;
        _has_split_branches = true;
      } else if (is_split_branch_cancel(instr)) {
        sym._sbType = InstrSym::sbCancel;
      } else if (sym.is_sb_1st()) {
        return;
      }

      if (!fixed_branch) {
        // This is a calculated branch.
        sym._brType = InstrSym::brCalc;
      } else if (is_cond) {
        // This is a fixed, conditional branch,
        sym._brType = InstrSym::brCond;
      } else {
        // This is a fixed, unconditional branch.
        sym._brType = InstrSym::brAlways;
      }
    }

    bool hasSources(const InstrSlotAlloc &isa)
    {
      ForEach(isa._items,iter) {
        const SlotInfo &si = iter->second;
        if (si._src) {
          return true;
        }
      }
      return false;
    }

    bool hasTargets(const InstrSlotAlloc &isa)
    {
      ForEach(isa._items,iter) {
        const SlotInfo &si = iter->second;
        if (si._trg) {
          return true;
        }
      }
      return false;
    }

    // Generates the operand-read functions, if we're creating a transactional ISS.
    void generateReadOpInstrHandler(ostream& o,const InstrSlotAlloc &isa,const Instr *instr,
                                    const gc_string &instrname,bool use_switch,CodeType ctype)
    {
      gc_string prefix = "void";
      qualifyName(prefix,ctype);
      o << "// Read-operands function for \"" << instrname << "\"\n"
        << prefix << " " << isa._ro_name << "(";
      if (use_switch) {
        o << PktType << " *" << PktArg << ", uint32_t *opc, unsigned index";
      } else {
        o << "uint32_t *opc";
      }
      o << ")";
      gc_string pkt = (use_switch) ? PktArg : PktName;
      if (!declOnly(o,ctype)) {
        o << "\n{\n";
        if (hasSources(isa)) {
          if (use_switch) o << "switch (index) {\n";

          gc_ostringstream os;
          bool     first = true;
          ForRange(_core.instr_width()/32,k) {
            list_sep(os,first);
            os << "opc[" << k << "]";
          }
          gc_string opcd = os.str();

          auto alias = (instr) ? instr->get_alias() : 0;

          ForEach(isa._items,iter) {
            const UsedReg  &ur = iter->first;
            const SlotInfo &si = iter->second;

            if (si._src) {

              if (const Reg *reg = ur.reg()) {
                if (use_switch) o << "case " << si._start << ":\n";
                // Skip if it's the cia, since we've already handled that in fetch.
                if ( reg != _core.cia() ) {
                  o << pkt << "->set_slot(" << si._start << "," << reg->name() << "_read());\n";
                }
                if (use_switch) o << "break;\n";
              } else if (const RegFile *rf = ur.regfile()) {
                if (ur.is_fixed()) {
                  if (use_switch) o << "case " << si._start << ":\n";
                  o << pkt << "->set_slot(" << si._start << "," << rf->name() << "_read(";
                  adl::writeFieldAccessor(o,_core,ur._begin,instr,alias,opcd);
                  o << "));\n";
                  if (use_switch) o << "break;\n";
                } else if (ur.is_range()) {
                  for (int i = si._start,index = 0; i != si._stop; ++i,++index) {
                    if (use_switch) o << "case " << i << ":\n";
                    o << "if ( " << index << " >= ";
                    adl::writeFieldAccessor(o,_core,ur._begin,instr,alias,opcd);
                    o << " && " << index << " <= ";
                    adl::writeFieldAccessor(o,_core,ur._end,instr,alias,opcd);
                    o << " ) {\n"
                      << pkt << "->set_slot(" << i << "," << rf->name() << "_read(" << index << "));\n"
                      << "}\n";
                    if (use_switch) o << "break;\n";
                  }
                } else {
                  // We don't have a fixed element that's accessing this register
                  // file, so we have to read in all elements.
                  for (int i = si._start, index = 0; i != si._stop; ++i,++index) {
                    if (use_switch) o << "case " << i << ":\n";
                    o << pkt << "->set_slot(" << i << "," << rf->name() << "_read(" << index << "));\n"
                      << "break;\n";
                  }
                }
              } else {
                RError("Unknown used-reg type.");
              }
            } else if (use_switch) {
              o << "case " << si._start << ":\n"
                << "break;\n";
            }
          }

          if (use_switch) o << "}\n";
        }
        o << "}\n\n";
      }
    }

    void generateReadOpInstrHandler(ostream &o,const Instr &instr,CodeType ctype)
    {
      const InstrSlotAlloc &isa = _genip.instr_slots()->find(instr.name())->second;
      if (!hybrid_iss()) {
        generateReadOpInstrHandler(o,isa,&instr,instr.name(),true,ctype);
      } else if (need_read_op(instr)) {
        generateReadOpInstrHandler(o,isa,&instr,instr.name(),false,ctype);
      }
    }


    void writeArgLogEntry(ostream &o,const UsedReg &ur, const SlotInfo &si, const Instr *instr, const Instr::Alias *alias)
    {
      if (const Reg *reg = ur.reg()) {
        if (reg != _core.cia()) {
          o << "os  << \" " << reg->name() << "\";\n";
        }
      } else if (const RegFile *rf = ur.regfile()) {
        if (ur.is_fixed()) {
          if (rf->sparse()) {
            o << "os  << \" \" << " << rf->name() << "_disasm(";
          } else {
            o << "os  << \" " << rf->name() << "\" << (";
          }
          writeFieldAccessor(o,ur,instr,alias);
          if (rf->sparse()) {
            o << ".uint32()";
          }
          o << ");\n";
        } else {
          for (int i=si._start, index=0; i != si._stop; ++i, ++index) {
            if (rf->sparse()) {
              o << "os  << \" \" <<" << rf->name() << "_disasm(" << dec << index << ");\n";
            } else {
              o << "os << \" " << rf->name() << "\" << " << dec << index << ";\n";
            }
          }
        }
      } 
    }

    // Generates the instrucion arguments log function 
    void generateInstrArgHandler(ostream &o,const Instr *instr)
    {
      const InstrSlotAlloc &isa = _genip.instr_slots()->find(instr->name())->second;

      auto alias = (instr) ?  instr->get_alias() : 0;

      o << "if (direction == 0 || direction > 1) {\n";
      ForEach(isa._items,iter) {
        const UsedReg  &ur = iter->first;
        const SlotInfo &si = iter->second;
        if (si._src) {
          writeArgLogEntry(o,ur,si,instr,alias);
        }
      }
      o << "}\n"
        << "if (direction >= 1) {\n";
      ForEach(isa._items,iter) {
        const UsedReg  &ur = iter->first;
        const SlotInfo &si = iter->second;
        if (si._trg) {
          writeArgLogEntry(o,ur,si,instr,alias);
        }
      }
      o << "}\n";
    }

    // Create a function to return branch information, if this instruction is
    // considered a branch.  If it isn't, then we use a generic version which
    // just returns a default response.
    void generateBranchInfoInstrHandler(const Instr &instr)
    {
      // The action function's name is the symbol in the hash.
      const InstrSlotAlloc &isa = _genip.instr_slots()->find(instr.name())->second;

      if (isa.is_branch() && instr.is_branch()) {

        _o << "// Branch-info function for \"" << instr.name() << "\"\n"
           << "BranchInfoType " << isa._bi_name << "(" << PktType << " *" << PktArg << ") {\n"
           << "return std::make_pair(";
        switch (instr.is_branch()) {
        case BranchCond:
          _o << "BranchCond";
          break;
        case BranchUncond:
          _o << "BranchUncond";
          break;
        case BranchNone:
          // Should never get here.
          assert(0);
          ;
        }
        _o << ",";
        auto spp = isa.getSlot(isa.branch_slot());
        assert(spp.first);
        if (spp.second->conditional()) {
          // Conditional branch- return the dirty flag for this slot.
          _o << PktArg << "->is_dirty(" << isa.branch_slot()->_start << ")\n";
        } else {
          // Unconditional branch, so we can always update this item.
          _o << "true";
        }
        _o << ");\n"
           << "};\n\n";
      }
    } 

    // Writes out an assignment of instruction attributes, if we have any.  We
    // only write out the instruction's attributes which are registered as being
    // used by queries within the system.
    bool writeInstrAttrs(ostream &o,const Instr *instr,bool always_clear,const gc_string &pfx = gc_string())
    {
      bool rc = false;
      if (have_instr_attrs()) {
        int count = 0;
        ostringstream ss;
        ss << pfx << getInstrAttrVar();
        if (time_tagged() && multi_issue()) {
          ss << "[" << pfx << "_issue_count]";
        }
        ss << " = ";
        if (instr) {
          const IntSet &instr_attrs = _core.instr_attrs();
          const AttrList &attrs = instr->attrs();
          bool first = true;
          ForEach(attrs,a) {
            if (instr_attrs.count(a->first)) {
              pfx_sep(ss," | ",first);
              ss << "(" << getInstrAttrType(_core) << ")" << _class_name << "::" << getInstrAttrName(a->first);
              ++count;
            }
          }
        }
        if (count == 0) {
          ss << "0";
        } else {
          rc = true;
        }
        if (count > 0 || always_clear) {
          o << ss.str() << ";\n";
        }
      }
      return rc;
    }

    void writeInstrAttrClear(ostream &o,const gc_string &pfx = gc_string())
    {
      if (have_instr_attrs() && normal_iss()) {
        o << pfx << getInstrAttrVar();
        if (time_tagged() && multi_issue()) {
          o << "[" << pfx << "_issue_count]";
        }
        o << " = 0;\n";
      }
    }

    // Create a function for returning information about delayed register
    // writes.  We only create these functions if delays exist.  If the
    // instruction has no delayed writes, then a default version is used.
    void generateDelayedWriteInstrHandler(const Instr &instr)
    {
      // The action function's name is the symbol in the hash.
      const InstrSlotAlloc &isa = _genip.instr_slots()->find(instr.name())->second;

      if (isa.has_delays()) {
        _o << "// Delayed-write info function for \"" << instr.name() << "\"\n"
           << "int " << isa._di_name << "(" << PktType << " *" << PktArg << ",unsigned index) {\n"
           << "switch (index) {\n";
        bool wd = false;
        ForEach(isa._items,iter) {
          const UsedReg  &ur = iter->first;
          const SlotInfo &si = iter->second;

          // TODO: I'm simplifying things here and not dealing with non-fixed
          // register-file issues, but I'm assuming that those wouldn't be for
          // delayed writes, anyway.  We can fix this later if it's a problem.
          if (si._trg && ur.delayed()) {
            _o << "case " << si._start << ":\n";
            wd = true;
          }
        }
        if (wd) {
          _o << "return " << PktArg << "->get_delay(index);\n";
        }
        _o << "default:\n"
           << "return -1;\n"
           << "}\n"
           << "};\n\n";
      }
    }
    
    // If we're just supposed to emit a declaration, then end the decl with a ;
    // and return true, otherwise do nothing and return false.
    bool declOnly(ostream &o,CodeType ctype)
    {
      if (ctype == cDecl) {
        o << ";\n\n";
        return true;
      } else{
        return false;
      }
    }

    // If we need support for the micro-op offset feature, then we construct a
    // switch statement to branch to the correct micro operation.  After that,
    // we increment the counter so that an instruction can query its current
    // location.  We do not support arbitrary micro-op execution- you start at
    // a given offset and run to the end of the micro-ops in the packet.    
    void writeMicroOpOffsetPreamble(ostream &o,const Instr &instr)
    {
      if (_core.has_micro_op_offset()) {
        o << "updateMicroOpOffset();\n"
          << "switch (_current_micro_op_offset) {\n";
      }
    }

    void writeMicroOpOffsetControl(ostream &o,const Instr &instr,unsigned index)
    {
      if (_core.has_micro_op_offset()) {
        o << "case " << index << ":\n"
          << "++_current_micro_op_offset;\n";
      }
    }
    
    void writeMicroOpOffsetPostamble(ostream &o,const Instr &instr)
    {
      if (_core.has_micro_op_offset()) {
        o << "}\n";
      }
    }
    
    void writeMicroOpHandler(opfxstream &o,const Instr &instr)
    {
      writeMicroOpOffsetPreamble(o,instr);
      
      unsigned index = 0;
      ForEach(instr.getFields(),iter) {
        if (const InstrField *field = iter->_field) {
          if (field->type() == InstrField::tInstr) {
            const gc_string &field_name = field->name();
            
            // We have a micro-op, so generate code to call it.
            const gc_string &table_name = field->instrtablename();
            const InstrTable *instr_table = _core.get_instrtable(table_name);

            // Round widths to neareset 32-bit word.
            unsigned instr_table_width = instr_table->getWidth();
            unsigned num = instr_table_width/32;
            unsigned max_num = _core.instr_width()/32;

            writeMicroOpOffsetControl(o,instr,index);

            // For parallel execution, we always execute the instructions.  If
            // not parallel, then we have to check for branches and possibly
            // exit early.
            
            o << "{\n"
              << "fill_in_instr_buf(" << field_name << "," << _iname << "," << num << ","
              << max_num << "," << (_core.instrEndianness() == Core::Little) << ");\n";

            if (_core.parallel_execution()) {
              o << "micro_instr_run(" << table_name << "," << ((iter+1) == instr.getFields().end()) << ");\n";
            } else {
              o << "if (micro_instr_run(" << table_name << "," << ((iter+1) == instr.getFields().end()) << ") == esEndBasicBlock) return;\n";
            }
            
            o << "}\n";

            ++index;
          }
        }
      }

      writeMicroOpOffsetPostamble(o,instr);
    }

    // Function for creating the actual instruction action handler.  This takes
    // into account whether we're creating a transactional ISS or generating JIT
    // code.
    void generateActualExecInstrHandler(opfxstream &o,const Instr &instr,InstrSym &sym,CodeType ctype,TMode tmode)
    {
      // If we're generating a transactional ISS, then we must set the slot-allocation
      // object so that slots can be properly assigned.
      if (transactional_iss()) {
        _walker.set_slots( &(_genip.instr_slots()->find(instr.name())->second) );
      }
      if (hybrid_iss()) {
        _walker.set_hybrid_iss(true);
      }
      if (track_reads()) {
        _walker.set_track_reads(true);
      }

      gc_string prefix,finish;

      gc_string taken_sym;
      if ((jit_support() || dc_support() || graphite_support()) && (sym.is_branch() && !sym.is_uncond_branch())) {
        taken_sym = "__branch_taken__";
        prefix += " bool";
        finish = "return "+taken_sym+";\n";
        _walker.set_suffix_reg(_nia,taken_sym+" = true");
      } else {
        prefix += " void";
      }

      // Transform any ADL RTL extensions into C++.
      _walker.set_instr_item(&instr);
      Ptree *newaction = _walker.translateCode(instr.env(),instr.action());
      _walker.set_instr_item(0);

      Ptree *exec_sym = sym._exec;
      // If it's a memory instruction, then we need a special JIT handler since we
      // want it to use the faster trace-cache memory path which uses the internal
      // DMI cache.  Thus, we end up with two versions of the handler for the same
      // instruction: One used by the interpreter and one used by the JIT.
      if (tmode != NormalTrans && (is_mem_instr(instr)) && (jit_support() || dc_support())) {
        exec_sym = sym._alt_exec = ((sym._alt_exec) ? sym._alt_exec : AdlGenSym());
      }

      // At this point, if we're in JIT mode, only write this second handler if we
      // need to.
      if (! (tmode == JitTrans && !is_mem_instr(instr))) {
        generateActualExecInstrHandlerInternal(o,instr,sym,newaction,exec_sym,prefix,taken_sym,finish,ctype,tmode);
      }

      // Necessary cleanup code.
      _walker.set_slots(0);
      _walker.set_suffix_reg(0);
    }

    // Inner function for creating the actual instruction action handler.  This
    // takes into account whether we're creating a transactional ISS or generating
    // JIT code.
    void generateActualExecInstrHandlerInternal(opfxstream &o,const Instr &instr,const InstrSym &sym,
                                                Ptree *action,Ptree *exec_sym,
                                                gc_string prefix,const gc_string &taken_sym,const gc_string &finish,
                                                CodeType ctype,TMode tmode)
    {
      bool first = true;

      // prefix
      if (instr.is_prefix()) {
        return;
      }

      qualifyName(prefix,ctype);
      
      o << "// Instruction \"" << instr.name() << "\"\n"
        << prefix << " " << exec_sym << "(";
      
      writeInstrHandlerArgs(o,instr,first,false);
      o << ")";

      if (jit_support() && (ctype != cImpl)) {
        // When creating a jit, we need to make sure that the instruction handlers
        // aren't inlined out.
        o << " ATTRIBUTE_USED ";
      }

      o << "\n";

      if (!declOnly(o,ctype)) {

        o << " {\n";

        // For MT support, if this is an instruction which touches event-bus
        // resources, then grab a lock.
        if (is_atomic_instr(sym)) {
          o << "LockAtomicInstr(" << getMemLocks() << "," << getAtomicFlag() << ");\n";
        }

        // Generate dependency-tracking setup code, if necessary.
        if (tmode == NormalTrans && have_dt()) {
          genDepTrackingSetup(o,instr);
        }

        if (!taken_sym.empty()) {
          o << "bool " << taken_sym << " = false;\n";
        }

        if (const Instr::Alias *alias = instr.get_alias()) {
          // Always call the alias target with the actual exec function, not with
          // the wrapper (if one exists) b/c we've already modified our fields
          // appropriately.
          Ptree *target = _instr_syms.find(alias->_target->name())->second._exec;
          o << target;
          writeAliasInstrHandlerCall(o,instr,alias);
          o << "}\n\n";
        } else if (instr.has_micro_ops() && !instr.action()) {
          // For instructions with micro ops and no explicit action function, we
          // create our own action function which calls the micro instructions
          // directly.
          writeMicroOpHandler(o,instr);
          o << "}\n\n";
        } else {
          writeLineDirective(instr.env(),instr.action());
          // For hybrid and transactional ISSs, we create a 0'd packet, so this
          // code is redundant.
          if (postFetchSkip() && normal_iss()) {
            o << "if (_skip_instruction_commit) {\n"
              << "return;\n"
              << "}\n";
          }          
          o << action << "\n";
          writeCurLocation(o);
          if (instr.next_table()) {
            // Insert the next-table code, if applicable.
            o << _walker.translateCode(instr.env(),instr.next_table()) << '\n';
          }
          if (_config.add_branch_hook() && sym.is_branch()) {
            o << getBrHookName() << "();\n";
          }
          o << finish << "}\n\n";
        }
      }
    }

    gc_string getTransLeafParms(const gc_string &pfx)
    {
      gc_string ua = decode_return_type(pfx) + " &instr,";
      if (logging_mem()) {
        ua += "bool speculative,";
      }
    
      return ua;
    }

    void generateLeafWrapHandler(ostream &o,const Instr &instr,CodeType ctype) 
    {
      if (instr.is_pseudo() || !instr.get_nested().empty()) {
        return;
      }
      InstrSym &sym = _instr_syms[instr.name()];
      o <<  "// Leaf wrapper for instruction " << instr.name() << "\n";
      
      gc_string pfx;
      qualifyName(pfx,ctype);

      if (transactional_iss()) {
        gc_string ua = getTransLeafParms(pfx);
        o << "ATTRIBUTE_NOINLINE void " << pfx << " " << sym._leafwrap << "(addr_t instr_addr," << ua << "unsigned size" << decode_parm(true) << ")"; 
      } else {
        o << "ATTRIBUTE_NOINLINE void" << pfx << " " << sym._leafwrap << "()"; 
      }

      if (!declOnly(o,ctype)) {
        o << "{\n";
        if (transactional_iss()) {
          writeTransLeafCode(o,&instr,sym,cImpl);
        } else {
          writeLeafCode(o,&instr,sym);
        }
        o << "}\n";
      } 
    }

    void generateDecodeMissLeafWrapHandler(ostream &o,CodeType ctype)
    {
      o <<  "// Leaf wrapper for decode-miss\n";
      
      gc_string pfx;
      qualifyName(pfx,ctype);
      
      if (transactional_iss()) {
        gc_string ua = getTransLeafParms(pfx);
        o << "ATTRIBUTE_NOINLINE void " << pfx << " " << getDecodeMissLeafName() << "(addr_t instr_addr," << ua << "unsigned size" << decode_parm(true) << ")"; 
      } else {
        o << "ATTRIBUTE_NOINLINE void " << pfx << " " << getDecodeMissLeafName() << "()"; 
      }

      if (!declOnly(o,ctype)) {
        o << "{\n";
        if (transactional_iss()) {
          writeTransDecodeMissLeafCode(o,cImpl);
        } else {
          writeDecodeMissLeafCode(o);
        }
        o << "}\n\n";
      } 
    }

    // This creates a wrapper function, if relevant, which creates modified
    // instruction field values and then calls the actual instruction handler.
    void generateExecWrapHandler(ostream &o,const Instr &instr,InstrSym &sym,CodeType ctype)
    {
      if (low_mem() && !sym._leafwrap) {
        sym._leafwrap = AdlGenSym();
      }
      if (sym._ewrap) {
        // This will be the name for the outer wrapper function.
        // This should have been set by checkInstrHandlerName.

        gc_string bt;
        gc_string taken;
        if (graphite_support()) {
          if (sym.is_branch()) {
            if (sym.is_uncond_branch()) {
              taken = "true";
            } else {
              taken = "rc";
              bt = "bool rc = ";
            }
          }
        }

        gc_string pfx = (ctype == cFull) ? "inline void " : "void ";
        qualifyName(pfx,ctype);
        o << "// Wrapper for instruction \"" << instr.name() << "\"\n"
          << pfx << " " << sym._ewrap << "(";

        bool first = true;

        writeInstrHandlerArgs(o,instr,first,true);
        o << ")\n";
        if (ctype == cDecl) {
          o << ";\n";
          return;
        } 
        o << "{\n";
        // Generate any field-modification code here.	        
        writeInstrFieldModifiers(o,instr);
        o << bt << sym._exec << "(";
        writeInstrArgs(o,instr,gc_string(),false);
        o << ");\n";

        if (graphite_support()) {
          o << "if (" << getGraphitePerfModel() << ") {\n";
          if (sym.is_branch()) {
            o << getGraphitePerfModel() << "->handleBranch(" << _cia->name() << "_read().uint64()," << taken << "," << _nia->name() << "_read().uint64());\n";
          }
          writeGraphiteInterpInstrCode(o,instr,sym);
          o << getGraphitePerfModel() << "->handleInstruction(" << getGraphitePerfInstr() << ");\n"
            << "}\n";
        }
      
        o << "}\n\n";
      }
    }

    // This writes pre-fetch code support for JIT models.
    void writeJitExecHandlers()
    {
      if (jit_support() || dc_support()) {
        _o << "void pre_fetch_handler() " << attr_used << "{\n";
        writePreFetchHandler(_o);
        _o << "}\n\n";

        _o << "void post_exec_outer_handler() " << attr_used << "{\n";
        writePostExec();
        _o << "}\n\n";
      }
    }

    // For JIT handlers, only update the CIA if we need to.  We don't need to if
    // the CIA is the same as the NIA, or if the instruction doesn't reference the
    // CIA and doesn't throw any exceptions.  If there's the possibility of
    // throwing an exception, then we do need a CIA update, since the exception
    // handler might use it.
    bool needCiaUpdate(const UsageInfo &ui)
    {
      if (_cia == _nia) return false;

      return (ui._src_regs.count(_cia) || ui._exceptions || !ui._src_mems.empty() || !ui._trg_mems.empty()
              || !ui._src_caches.empty() || !ui._trg_caches.empty());
    }

    // This generates the pre-instruction code for a JIT handler.
    void writeJitPreActionCode(ostream &ss,const gc_string &pfx,const gc_string &name,
                               Instr::PostFetchEnum enable_post_fetch,bool update_cia,bool write_post_fetch,
                               unsigned num_bytes,TMode mode,CodeType ctype)
    {
      ss << "++(" << pfx << "_instr_count);\n";

      if (!time_tagged() && !graphite_support() && td_api() && per_instr_td_incr()) {
        ss << pfx << "_td_counter += " << pfx << "_td_increment;\n";
      }

      if (update_cia) {
        ss << pfx << _cia->name() << "_write(" << pfx << _nia->name() << "_read());\n";
      }

      if (_core.pre_fetch_handler() || _core.pre_cycle_handler()) {
        ss << pfx << "pre_fetch_handler();\n";
      }     

      if (write_post_fetch) {
        writePostFetchHandler(ss,pfx,name,enable_post_fetch,num_bytes,!update_cia);
      }
    }

    // This generated the external functions which call the instruction handlers
    // in JIT mode.
    void generateJitInstrFunc(const Instr &instr,const InstrSym &sym,CodeType ctype)
    {
      if (!sym.valid()) {
        // We don't handle aliases- that's done during translation time.
        return;
      }

      // No translation-time conditionals to worry about, so just generate the
      // wrapper function normally.
      generateJitInstrFuncInternal(instr,sym,sym._id,sym._log_id,sym._plog_id,sym.jit_exec_func(),ctype);
    }

    void writeJitHandlerDecl(const Instr &instr,const InstrSym &sym,int id)
    {
      if (sym.is_cond_branch()) {
        *_jo << "bool ";
      } else {
        *_jo << "void ";
      }
      *_jo << getJitInstrPrefix() << id << "(";
      writeInstrArgs(*_jo,instr,"int64_t",false);
      *_jo << ")";
    }

    // Internal function for generating the external functions which call the
    // instruction handlers in JIT mode.  This also writes out a tracing function for the instruction, which
    // logs the instruction-start.
    void generateJitInstrFuncInternal(const Instr &instr,const InstrSym &sym,int id,int log_id,int plog_id,Ptree *exec,CodeType ctype)
    {
      gc_string pfx = _path_str + "->";

      if (_config.trace_mode()) {
        *_jo << "// Core " << _core.name() << ": JIT instruction logger for instruction \"" << instr.name() << "\".\n"
             << "void " << getJitInstrPrefix() << log_id << "(addr_t ea";
        unsigned n = (_core.instr_width()/32);
        ForRange(n,i) {
          *_jo << ",uint32_t x" << i;
        }
        *_jo << ")\n{\n"
             << pfx << "get_logger()->log_instr_prefetch(ea);\n"
             << "uint32_t opc[" << n << "] = { ";
        bool first = true;
        ForRange(n,i) {
          list_sep(*_jo,first);
          *_jo << "x" << i;
        }
        *_jo << " };\n"
             << pfx << "get_logger()->log_instr(opc," << dec << instr.width()/4 <<",\"" << instr.name() <<"\","
             << pfx << sym._disasm << "," << getInstrFlags(sym) << ");\n";
        if (basic_block_end(instr,sym)) {
          *_jo << pfx << "get_logger()->log_basic_block_end(ea);\n";
        }
        *_jo << "}\n\n";

        if (sym.is_branch()) {
          *_jo << "// Core " << _core.name() << ": JIT instruction post logger for instruction \"" << instr.name() << "\".\n"
               << "void " << getJitInstrPrefix() << plog_id << "(addr_t ea)\n{\n"
               << pfx << "get_logger()->log_branch_end(ea);\n"
               << "}\n\n";
        }
      }

      *_jo << "// Core " << _core.name() << ": JIT handler for instruction \"" << instr.name() << "\".\n";
      writeJitHandlerDecl(instr,sym,id);
      *_jo << "\n{\n";

      bool rc = writeInstrAttrs(*_jo,&instr,false,pfx);
      writeJitPreActionCode(*_jo,pfx,instr.name(),instr.enable_post_fetch(),
                            needCiaUpdate(instr.usageinfo()),true,instr.fetch_width()/8,JitTrans,ctype);

      if (sym.is_cond_branch()) {
        *_jo << "bool rc = ";
      }
      *_jo << pfx << exec << "(";
      writeInstrArgs(*_jo,instr,gc_string(),false);
      *_jo << ");\n";

      if (_core.post_exec_handler() || _core.post_cycle_handler()) {
        *_jo << pfx << "post_exec_outer_handler();\n";
      }

      if (rc) {
        writeInstrAttrClear(*_jo,pfx);
      }

      writeGraphiteJitExecCode(*_jo,instr,sym);

      if (sym.is_cond_branch()) {
        *_jo << "return rc;\n";
      }

      *_jo << "}\n\n";
    }

    // This generates the generic decode-cache function used in decode-cache mode.
    void generateDcInstrFunc(ostream &o,const Instr &instr,const InstrSym &sym,TMode tmode,CodeType ctype)
    {
      if (!sym.valid()) {
        // We don't handle aliases- that's done during translation time.
        return;
      }

      if (ctype == cDecl) return;

      // We invoke the jit-exec-func b/c we use that for both JIT and DC sim.
      generateDcInstrFuncInternal(o,instr,sym,sym._id,sym.jit_exec_func(),tmode,ctype);

      // Write out the fixed version of the split-branch decode-cache handler, if
      // relevant.
      if (sym.is_sb_2nd() && (tmode == DcTrans)) {
        generateDcInstrFuncInternal(o,instr,sym,sym._id,sym.jit_exec_func(),DcTransFixed,ctype);
      }
    }

    // Write the function decl for a decode-cache handler.
    void writeDcHandlerDecl(ostream &o,const gc_string &fpfx,const char *fa1, const char *fa2, int id,TMode tmode)
    {
      o << "static " << fpfx << "DecodeReturn " << fa1 << getDcInstrPrefix(tmode) << id << "(" << _class_name << "*core," 
        << fpfx << "DecodeItem *dci)" << fa2;
    }

    // Writes the arguments used to call an instruction handler, by a decode-cache function.
    void writeDcArgs(ostream &o,const Instr &instr,const InstrSym &sym,int start_args)
    {
      bool first = true;
      int i = 0;
      int numfields = instr.sizeFields();
      int elm = start_args;
      for (; i != numfields; ++i) {
        Instr::Field fp = instr.getField(i);
        // variable fields use for opcode
        if (skip_field(fp)) {  
          continue;
        }
        list_sep(o,first);
        // Cast to signed if the field is signed, so that we get proper
        // sign-propagation.
      
        o << "dci->_args[" << elm++ << "]";
      }
    }

    // Returns the index of the start of arguments in the first element, and the instruction count for this BB in the second,
    // or -1 if the temporal-decoupling API is not active.
    pair<int,int> dcArgInfo(const Instr &instr,const InstrSym &sym)
    {
      int start_args = (instr.width()/32);
      if ((instr.width() % 32)) {
        ++start_args;
      }
      int td_arg = -1;
    
      if (td_api() && (basic_block_end(instr,sym) || optional_basic_block_end(instr))) {
        td_arg = start_args++;
      }
      return make_pair(start_args,td_arg);
    }

    // Write any code for sending instruction data to the Graphite performance model (if relevant).
    void writeGraphiteExecCode(ostream &o,const Instr &instr,const InstrSym &sym,const gc_string &trg_arg)
    {
      if (graphite_support()) {

        o << "if (core->" << getGraphitePerfModel() << ") {\n";

        if (sym.is_branch()) {
          string taken;

          if (sym.is_cond_branch()) {
            taken = "rc";
          } else {
            taken = "true";
          }
        
          o << "core->" << getGraphitePerfModel() << "->handleBranch(core->" << _cia->name() << "_read().uint64()," << taken << "," << trg_arg << ");\n";
        }

        o << "core->" << getGraphitePerfModel() << "->handleInstruction(*dci->_instr);\n"
          << "}\n";
      }
    }

    void writeGraphiteDcExecCode(ostream &o,const Instr &instr,const InstrSym &sym,unsigned pc_arg)
    {
      if (graphite_support()) {

        gc_string trg_arg;
        if (sym.is_branch()) {
          // If we have a fixed branch, then we can determine the target.
          // Otherwise, we just use the NIA.
          if (!sym.is_calc_branch()) {
            // OK, then find the field which has the actual address.
            ForRange(instr.sizeFields(),i) {
              Instr::Field fp = instr.getField(i);
              if (skip_field(fp)) {
                continue;
              }
              const InstrField &field = *(fp._field); 
              // If we find a field that's an addressing field, we assume that this is
              // what's used to determine a branch target.
              if (field.addr() == InstrField::aPcRel || field.addr() == InstrField::aAbs ) {
                MkGStr(trg_arg,"dci->_args[" << pc_arg << "]");
                break;
              }
              ++pc_arg;
            }
          } else {
            trg_arg = "core->" + _nia->name() + "_read().uint64()";
          }
        }

        writeGraphiteExecCode(o,instr,sym,trg_arg);
      }
    }

    void writeGraphiteJitExecCode(ostream &o,const Instr &instr,const InstrSym &sym)
    {
      if (graphite_support()) {
        gc_string trg_arg;

        if (sym.is_branch()) {
          if (!sym.is_calc_branch()) {
            // OK, then find the field which has the actual address.
            ForRange(instr.sizeFields(),i) {
              Instr::Field fp = instr.getField(i);
              if (skip_field(fp)) {
                continue;
              }
              const InstrField &field = *(fp._field); 
              // If we find a field that's an addressing field, we assume that this is
              // what's used to determine a branch target.
              if (field.addr() == InstrField::aPcRel || field.addr() == InstrField::aAbs ) {
                gc_string cia = _path_str + "->" + _cia->name() + "_read()";
                auto r = getInstrFieldModifier(instr,field,true,gc_string(),cia.c_str());
                trg_arg = r._value + ".uint64()";
                break;
              }
            }
          } else {
            trg_arg = _path_str + "->" + _nia->name() + "_read().uint64()";
          }
        }
      
        writeGraphiteExecCode(o,instr,sym,trg_arg);
      }
    }

    void writeDcLogCall(ostream &o,const Instr &instr,const InstrSym &sym,TMode tmode)
    {
      if (_config.trace_mode()) {
        o << "ExtCondTrace({\n"
          << "if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {\n"
          << "core->get_logger()->log_instr_prefetch(core->" << _cia->name() << "_read().uint64());\n"
          << "}\n"
          << "if (Tracing && (core->trace_mode() & ttInstr)) {\n";
        if ((_cia->width() > 32) || (tmode == JitTrans)) {
          // If we have 64-bit arguments then we have to copy them to an array of
          // 32-bit ints for logging purposes.  The JIT always uses 64-bit arguments.
          unsigned numops = (instr.width()/32);
          o << "uint32_t ops[" << numops << "] = { ";
          bool first = true;
          ForRange(numops,i) {
            list_sep(o,first);
            o << "(uint32_t)dci->_args[" << i << "]";
          }
          o << " };\n"
            << "core->get_logger()->log_instr(ops,";
        } else {
          o << "core->get_logger()->log_instr(dci->_args,";
        }
        o << dec << instr.width()/4 << ",\"" << instr.name() << "\",core->" << sym._disasm << "," << getInstrFlags(sym) << ");\n"
          << "}\n";
        if (basic_block_end(instr,sym)) {
          o << "if (Tracing && (core->trace_mode() & ttBasicBlockEnd)) {\n"
            << "core->get_logger()->log_basic_block_end(core->" << _cia->name() << "_read().uint64());\n"
            << "}\n";
        }
        o << "})\n";
      }

    }

    // Internal function for generating the external functions which call the
    // instruction handlers in JIT mode.  This also writes out a tracing function for the instruction, which
    // logs the instruction-start.
    void generateDcInstrFuncInternal(ostream &o,const Instr &instr,const InstrSym &sym,int id,Ptree *exec,TMode tmode,CodeType ctype)
    {
      gc_string pfx = "core->";

      o << "// Decode-cache handler for instruction \"" << instr.name() << "\".\n";

      gc_string fpfx = "";
      qualifyName(fpfx,ctype);    

      gc_string tpfx,dpfx;
      if (tmode == JitTrans) {
        fpfx += "Jit";
        dpfx = "_jit";
      } else {
        fpfx += "Dc";
        dpfx = "_dc";
      }

      // If everything is in a single class, then the declaration and the
      // definition are the same, so we just need this one.
      writeDcHandlerDecl(o,fpfx,"__fastcall "," FASTCALL",id,tmode);
      if (ctype == cImpl){
        // When split into multiple files, we have to issue a declaration, then the definition.
        // Unfortunately, there's a difference between Visual C++ and gcc- VisualC++ wants the definition
        // and decl to be the same and complains otherwise, but allows attributes, whereas gcc complains
        // if the definition has those attributes.
        o << ";\n";
        writeDcHandlerDecl(o,fpfx,"FASTCALL2 ","",id,tmode);
      }
      if (declOnly(o,ctype)) return;

      o << "\n{\n";

      // Note: No NIA/CIA updates for micro-ops, since we do that in the outer
      // function.  We also need to reset the micro-op mask if we're a micro-op.
      
      bool rc = writeInstrAttrs(o,&instr,false,pfx);
      writeJitPreActionCode(o,pfx,instr.name(),instr.enable_post_fetch(),
                            !instr.is_micro_op(),false,instr.fetch_width()/8,JitTrans,ctype);

      writeDcLogCall(o,instr,sym,tmode);

      if (!instr.is_micro_op()) {
        writePostFetchHandler(o,pfx,instr.name(),instr.enable_post_fetch(),instr.fetch_width()/8,false);
      }

      if (instr.has_micro_ops() && _core.has_micro_op_offset()) {
        // Reset the mask so that we don't screw up subsequent execution after a
        // branch.
        o << "core->updateMicroOpOffset();\n";
      }

      auto ap = dcArgInfo(instr,sym);

      bool ret_branch, cond_branch, imm_branch, need_rc;
      if (tmode == DcTransFixed) {
        // In this case, we know we're handling a split branch and we want to
        // generate an instruction which handles it as a fixed branch.
        ret_branch = false;
        need_rc = cond_branch = sym.is_cond_split_branch();
        imm_branch = true;
      } else {
        // We only implement direct threading for the standalone decode cache, so if
        // we're in JIT mode, treat all branches as calculated branches (just return
        // to the sim kernel).
        ret_branch  = (sym.is_calc_branch() || (tmode == JitTrans && sym.is_branch()));
        cond_branch = (sym.is_cond_branch() && tmode != JitTrans);    
        imm_branch  = (sym.is_imm_branch() && tmode != JitTrans);
        need_rc     = (sym.is_cond_branch() && (tmode != JitTrans || graphite_support()));
      }

      writeTimeTagInstrLogic(o,instr,"*core","core->");

      if (!(instr.has_micro_ops() && !instr.action())) {
        if (need_rc) {
          o << "bool rc = ";
        }
        o << pfx << exec << "(";
        writeDcArgs(o,instr,sym,ap.first);
        o << ");\n";
      }

      if (sym.is_branch() && _config.trace_mode()) {
        o << "ExtCondTrace({\n"
          << "if (Tracing && (core->trace_mode() & ttBranchTaken)) {\n"
          << "core->get_logger()->log_branch_end(core->" << _cia->name() << "_read().uint64());\n"
          << "}\n"
          << "});\n";
      }

      writeTimeTagFinishCall(instr,"core->");
      writeGraphiteDcExecCode(o,instr,sym,ap.first);
      writeLogTdCommit(_o,"core->",true);

      if (mt_support()) {
        o << "ExtCondTrace({\n"
          << "if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {\n"
          << "core->get_logger()->log_commit(0);\n"
          << "}\n"
          << "})\n";
      }

      // Deal with the td check here for immediate branches, since
      // direct-threading means we have to return out of the inner loop.
      if (td_api() && (sym.is_imm_branch() || sym.is_sb_2nd())) {
        gc_ostringstream ss1, ss2;
        ss1 << "\n"
            << "core->" << dpfx << "_decode_cache_next = 0;\n";
        if (rc) writeInstrAttrClear(ss1,pfx);
        ss1 << "return (" << fpfx << "VoidFunc)JitRetCode;\n";
        ss2 << "dci->_args[" << ap.second << "]";
        add_td_check(o,pfx,ss1.str(),ss2.str(),true,false,true,false);
      }

      if (cond_branch) {
        o << "if (rc) {\n";
      }

      if (imm_branch) {
        // If we're dealing with a fixed branch, then we return our target pointer.
        if (_core.post_exec_handler() || _core.post_cycle_handler()) {
          o << pfx << "post_exec_outer_handler();\n";
        }
        o << fpfx << "DecodeItem *bt = dci->_bt_t->bt();\n"
          << "core->" << dpfx << "_decode_cache_next = bt;\n";
        if (rc) writeInstrAttrClear(o,pfx);
        o << "return (" << fpfx << "VoidFunc)JitContCode;\n";
      }

      if (cond_branch) {
        o << "} else {\n";
      }

      if (_core.post_exec_handler() || _core.post_cycle_handler()) {
        o << pfx << "post_exec_outer_handler();\n";
      }

      // If this is an unconditional immediate branch, then we don't need an
      // end-jump because we're always going to take the branch.  If it's a
      // calculated branch or a halt then we must return back to the driver so
      // that it can either jump to the new location or quit.
      bool opt_end = optional_basic_block_end(instr);
      if (ret_branch || sym.halts() || explicit_bb_end(instr)) {
        o << "core->" << dpfx << "_decode_cache_next = 0;\n";
        if (rc) writeInstrAttrClear(o,pfx);
        // Put a td check here as well- this handles other kinds of branches,
        // making sure that we exit the simulation loop if we exceed our td threshold.
        if (td_api() && !sym.is_imm_branch()) {
          gc_ostringstream ss1, ss2;
          ss1 << "return (" << fpfx << "VoidFunc)JitRetCode;";
          ss2 << "dci->_args[" << ap.second << "]";
          add_td_check(o,pfx,ss1.str(),ss2.str(),true,false,true,false);
        }
        o << "return (" << fpfx << "VoidFunc)JitNopCode;\n";
      }
      else if (cond_branch) {
        // Else-clause for conditional branches.
        o << fpfx << "DecodeItem *bt = dci->_bt_f->bt();\n"
          << "core->" << dpfx << "_decode_cache_next = bt;\n";
        if (rc) writeInstrAttrClear(o,pfx);
        o << "return (" << fpfx << "VoidFunc)JitContCode;\n";
      } else if (!sym.is_branch()) {
        if (opt_end && td_api()) {
          gc_ostringstream ss1, ss2;
          ss1 << "core->" << dpfx << "_decode_cache_next = 0;\n";
          if (rc) writeInstrAttrClear(ss1,pfx);
          ss1 << "return (" << fpfx << "VoidFunc)JitRetCode;";
          ss2 << "dci->_args[" << ap.second << "]";
          add_td_check(o,pfx,ss1.str(),ss2.str(),true,false,true,false);
        }
        // Otherwise, just get the next item in the array and continue.
        o << "core->" << dpfx << "_decode_cache_next = &dci[1];\n";
        if (rc) writeInstrAttrClear(o,pfx);
        o << "return (" << fpfx << "VoidFunc)dci[1].func();\n";
      }

      if (cond_branch) {
        o << "}\n";
      }

      o << "}\n\n";
    }

    // Generate the post-fetch enable function, if applicable.
    void generatePostFetchEnable(opfxstream &o,const Instr &instr)
    {
      if (instr.enable_post_fetch() == Instr::PostFetchFunc) {
        o << "// Post-fetch enable for \"" << instr.name() << "\"\n";
        genFunc("bool "+getPostFetchEnableName(instr.name()),_core.env(),instr.enable_post_fetch_func()._func);
      }
    }

    // Create instruction action function and disassembler function.  Each
    // instruction's actions are implemented as inline functions.
    int generateExecInstrHandler(opfxstream &o,const Instr &instr,CodeType ctype)
    {
      // The action function's name is the symbol in the hash.
      InstrSym &sym  = _instr_syms.find(instr.name())->second;

      // We only need to do this once, but we need this information regardless
      // of JIT support, since it's also used for logging purposes.
      checkBranchTypeInstr(instr,sym);

      gc_string pfx1,pfx2;
      if (ctype != cImpl) {
        pfx1 = "static";
      }
      qualifyName(pfx2,ctype);

      if (!encap_instruction(instr)) {
        if (!instr.is_prefix()) {
          o << "// Disassembler for \"" << instr.name() << "\"\n"
            << pfx1 << " int " << pfx2 << sym._disasm << "(std::ostream &os, addr_t memaddr,const uint32_t* insn)\n";
          if (!declOnly(o,ctype)) {
            o << "{\n";
            _disasm.writeDisTableEntry(o,instr,instr.shorthands());
            o << "}\n\n";
          }
        }
        if (_config.log_instr_args_mode()) {
          o << "// InstrArgsLogger for \"" << instr.name() << "\"\n"
            << pfx1 << " int " << pfx2 << " " << sym._logarg << "(std::ostream &os, addr_t direction,const uint32_t* insn)\n";
          if (!declOnly(o,ctype)) {
            o << "{\n"
              << "// direction     0:  sources \n"
              << "// direction     1:  targets \n"
              << "// direction   >=2:  sources and targets\n\n";
            _disasm.writeDisTableEntryAccessors(o,instr,instr.shorthands());
            generateInstrArgHandler(o,&instr);
            o << "return 1;\n"
              << "}\n\n";
          }
        }
      }

      generatePostFetchEnable(o,instr);

      generateExecWrapHandler(o,instr,sym,ctype);

      if (low_mem()) {
        // Leaf wrapper and implementation are distributed to various files.
        generateLeafWrapHandler(o,instr,ctype);
      }
    
      // This is the normal instruction handler used by the interpreter.
      generateActualExecInstrHandler(o,instr,sym,ctype,NormalTrans);

      // We use the same handlers for the JIT and decode-cache.
      if (jit_support() || dc_support()) {
        bool orm = _walker.record_mem_accesses(false);
        _walker.add_memory_suffix(getJitMemSuffix(),getJitMemSuffix());

        generateActualExecInstrHandler(o,instr,sym,ctype,JitTrans);

        _walker.set_memory_suffix_default();
        _walker.record_mem_accesses(orm);
      }

      if (dc_support()) {
        generateDcInstrFunc(o,instr,sym,DcTrans,ctype);
      }  

      if (jit_support()) {
        // For the JIT, we need both a decode-cache function (for the first
        // compilation pass) and the actual JIT function (for the fast variant).
        generateDcInstrFunc(o,instr,sym,JitTrans,ctype);
      }

      if (ctype == cImpl && !transactional_iss()) {
        return 1; // Do we need this return?
      }     

      if (jit_support()) {
        // We only want this once, since it just appears in the file compiled by clang.
        generateJitInstrFunc(instr,sym,ctype);
      }

      if (instr_attrs()) {
        generateFieldInfoInstrHandler(o,instr,ctype);
        generateAttrInfoInstrHandler(o,instr,ctype);
      }

      // If a transactional ISS, then this is the driver function which is actually called.
      if (transactional_iss() && !hybrid_iss() && !instr.is_abstract()) {
        const InstrSlotAlloc &isa = _genip.instr_slots()->find(instr.name())->second;

        if (uses_coro_exec(isa)) {
          // If we use memory for this instruction and we're using a
          // non-blocking interface, we employ a coroutine.

          o << "// Coroutine driver function for \"" << instr.name() << "\"\n";
          if (ctype != cImpl) {
            o << "static ";
          }
          o << "void " << pfx2 << " " << isa._co_name << "(Coro *co," << _core.name() << " *core," << PktType << " *" << PktArg << ")";
          if (!declOnly(o,ctype)) { 
            o << "{\n";

            // Emit no code if we have no exceptions.
            if (haveExceptions()) {
              o << "try {\n";
            }

            gc_ostringstream os;
            bool first = true;
            ForRange(_core.instr_width()/32,k) {
              list_sep(os,first);
              os << PktArg << "->_opc[" << k << "]";
            }

            first = true;
            o << "core->" << sym.exec_func() << "(";
            writeFieldAccessorsTrans(o,instr,os.str(),first);
            o << ");\n";
            if (_core.post_exec_handler()) {
              o << "core->post_exec_handler(opc);\n";
            }

            // Emit no code if we have no exceptions.
            if (haveExceptions()) {
              o << "}\n"
                << "catch (Exception x) {\n"
                // << PktName "->_exdata |= x;\n"
                << "core->setException(" << PktArg << ", x);\n"
                << "}\n";
            }

            o << "core->_coro_stacks.free(" << PktArg << "->_stack);\n"
              << PktArg << "->_stack = 0;\n"
              << "}\n\n";
          }
	  
          o << "// Execution driver function for \"" << instr.name() << "\"\n"
            << "MemAccess::OpType " << pfx2 << " " << isa._ex_name << "(uint32_t *opc)";
          if (!declOnly(o,ctype)) {
            o << "\n{\n"
              << "assert(" << PktName << "->_stack);\n"
              << "_memory_request.reset();\n"
              << PktName << "->_co.call();\n";
            // If the rethrow flag is set, then if we had an exception, propagate
            // it up.
            if (trans_rethrow() && haveExceptions()) {
              o << "if (" << PktName << "->has_exceptions()) {\n"
                << "throw (Exception)" << PktName << "->_exdata;\n"
                << "}\n";
            }
            o << "return _memory_request.op();\n"
              << "}\n\n";
          }
        } else {
          // No memory is accessed, so we execute as a plain function.
          gc_ostringstream os;
          bool     first = true;
          ForRange(_core.instr_width()/32,k) {
            list_sep(os,first);
            os << "opc[" << k << "]"; 
          }
          gc_string opcd = os.str();

          first = true;

          o << "// Execution driver function for \"" << instr.name() << "\"\n"
            << "MemAccess::OpType " << pfx2 << " " << isa._ex_name << "(uint32_t *opc)";
          if (!declOnly(o,ctype)) { 
            o << "{\n";
            if (need_read_op(instr)) {
              o << isa._ro_name << "(opc);\n";
            }
            o << sym.exec_func() << "(";
            writeFieldAccessorsTrans(o,instr,opcd,first);
            o << ");\n";
            if (!instr.is_prefix() && _core.post_exec_handler()) {
              o << "post_exec_handler();\n";
            }
            o << "return MemAccess::opNone;\n"
              << "}\n\n";
          }
        }
      }

      // Return the number of jit handlers which were written.
      return 1;
    }

    // Writes operand usage statements for Graphite perf model integration.
    void writeGraphiteRegUsage(ostream &o,const UsedRegs &urs,const char *optype)
    {
      ForEach(urs,i) {
        const UsedReg &ur = *i;

        if (const Reg *reg = ur.reg()) {
          // No need to call graphite_id(), since a register's ID is the same as the graphite register id.
          o << "olist.push_back(Operand(Operand::REG, " << reg->id() << "," << optype << "));\n";
        } else if (const RegFile *rf = ur.regfile()) {
          if (ur.is_fixed()) {
            o << "olist.push_back(Operand(Operand::REG, graphite_id(" << rf->id() << ",to_uint32(" << ur._begin << "))," << optype << "));\n";
          } else {
            o << "for (unsigned index = to_uint32(" << ur._begin << "); index <= to_uint32(" << ur._end << "); ++index) {\n"
              << "olist.push_back(Operand(Operand::REG, graphite_id(" << rf->id() << ",index)," << optype << "));\n"
              << "}\n";
          }
        }
      }
    }

    // Generate code for Graphite performance model integration.  This adds an
    // instruction object with relevant operands to the Graphite basic block
    // stored with the ADL basic block.
    void writeGraphiteInstrCode(ostream &o,const Instr &instr,const InstrSym &sym)
    {
      if (graphite_support()) {
        o << "OperandList olist;\n";

        const UsageInfo &ui = instr.usageinfo();

        // If the instruction has a source or target memory, then it's a memory op
        // and we record the fact.
        if (!ui._src_mems.empty()) {
          o << "olist.push_back(Operand(Operand::MEMORY, 0, Operand::READ));\n";
        }
        if (!ui._trg_mems.empty()) {
          o << "olist.push_back(Operand(Operand::MEMORY, 0, Operand::WRITE));\n";
        }

        // Then handle registers.  Note that we create a unique ID for each register
        // element by multiplying the register number by 1000, then add on the index
        // (if any).  For now, we skip immediates, since I don't see how that can be
        // of any use.
        writeGraphiteRegUsage(o,ui._src_regs,"Operand::READ");
        writeGraphiteRegUsage(o,ui._trg_regs,"Operand::WRITE");

        o << "di->set_instr(new Instruction(" << _coreconfig.get_graphite_group(instr).name() << ",std::move(olist)));\n";
      }
    }

    // Writes operand usage statements for Graphite perf model integration.
    void writeGraphiteInterpRegUsage(ostream &o,unsigned &ocount,const UsedRegs &urs,const char *optype)
    {
      ForEach(urs,i) {
        const UsedReg &ur = *i;

        if (const Reg *reg = ur.reg()) {
          // No need to call graphite_id(), since a register's ID is the same as the graphite register id.
          o << getGraphitePerfInstr() << ".setOperand(" << ocount++ << ",Operand::REG, " << reg->id() << "," << optype << ");\n";
        } else if (const RegFile *rf = ur.regfile()) {
          if (ur.is_fixed()) {
            o << getGraphitePerfInstr() << ".setOperand(" << ocount++ << ",Operand::REG, graphite_id(" << rf->id() << ",to_uint32(" << ur._begin << "))," << optype << ");\n";
          } else {
            o << "for (unsigned index = to_uint32(" << ur._begin << "); index <= to_uint32(" << ur._end << "); ++index) {\n"
              << getGraphitePerfInstr() << ".setOperand(" << ocount++ << ",Operand::REG, graphite_id(" << rf->id() << ",index)," << optype << ");\n"
              << "}\n";
          }
        }
      }
    }

    // Generate code for Graphite performance model integration.  This is for the
    // interpreted invocation, where we modify a single object, in order to be as
    // fast as possible.
    void writeGraphiteInterpInstrCode(ostream &o,const Instr &instr,const InstrSym &sym)
    {
      if (graphite_support()) {

        unsigned ocount = 0;
        ostringstream ss;
        const UsageInfo &ui = instr.usageinfo();

        // If the instruction has a source or target memory, then it's a memory op
        // and we record the fact.
        if (!ui._src_mems.empty()) {
          ss << getGraphitePerfInstr() << ".setOperand(" << ocount++ << ",Operand::MEMORY, 0, Operand::READ);\n";
        }
        if (!ui._trg_mems.empty()) {
          ss << getGraphitePerfInstr() << ".setOperand(" << ocount++ << ",Operand::MEMORY, 0, Operand::WRITE);\n";
        }

        // Then handle registers.  Note that we create a unique ID for each register
        // element by multiplying the register number by 1000, then add on the index
        // (if any).  For now, we skip immediates, since I don't see how that can be
        // of any use.
        writeGraphiteInterpRegUsage(ss,ocount,ui._src_regs,"Operand::READ");
        writeGraphiteInterpRegUsage(ss,ocount,ui._trg_regs,"Operand::WRITE");

        o << getGraphitePerfInstr() << ".setTypeAndSize(" << _coreconfig.get_graphite_group(instr).name() << "," << ocount << ");\n"
          << ss.str();
      }
    }

    void writeJitTranslationHandlerDecl(ostream &o,const Instr &instr,const InstrSym &sym, CodeType ctype)
    {
      Ptree *fname = sym._trans;

      // Ugh!  For MSVC, we exceed some limit on the number of methods we can have
      // in a class, so to get around this we make these functions external
      // functions in a multi-jobs model.
      gc_string iss_parm = _class_name + " &iss,";

      gc_string pfx = "addr_t ";

      gc_string cpfx;
      if (multi_compile()) {
        cpfx = _class_name + "::";
      }

      o << "// JIT translation function for instruction \"" << instr.name() << "\"\n"
        << "static " << pfx << fname << "(" << iss_parm << cpfx << "MyJitBasicBlock *bb," << cpfx << "JitDecodeItem *di,addr_t pc)";
    }

    void generateJitTranslationHandler(ostream &o,const Instr &instr,CodeType ctype)
    {
      InstrSym &sym  = _instr_syms.find(instr.name())->second;

      gc_string pfx = "iss.";

      writeJitTranslationHandlerDecl(o,instr,sym,ctype);
      o << "\n{\n";
      
      unsigned bytes     = instr.width() / 8;
      unsigned words     = instr.width() / 32;
      unsigned max_words = (_core.instr_width()/32);

      // First, set up the call to the handler.  Then, deal with the special
      // case of branches.

      if (_config.trace_mode()) {
        // Add in conditional calls to the instruction logger function.
        o << "if (Tracing && (" << pfx << "trace_mode() & ttInstr)) {\n"
          << pfx << "_jit.add_log_call(" << sym._log_id << ",pc," << "di->_args," << words << "," << max_words << ");\n"
          << "}\n";
        if (sym.is_branch()) {
          o << "if (Tracing && (" << pfx << "trace_mode() && ttBranchTaken)) {\n"
            << pfx << "_jit.add_post_log_call(" << sym._plog_id << ",pc);\n"
            << "}\n";
        }
      }

      auto ap = dcArgInfo(instr,sym);
    
      unsigned num_ops = 0;
      ForRange(instr.sizeFields(),i) {
        Instr::Field fp = instr.getField(i);
        if (skip_field(fp)) {
          continue;
        }
        ++num_ops;
      }

      gc_string icount;
      if (ap.second >= 0 && (sym.is_cond_branch() | sym.is_imm_branch())) {
        o << "unsigned count = di->_args[" << ap.second << "];\n";
        icount = "count";
      } else {
        icount = "0";
      }

      if (sym.is_cond_branch()) {
        // Non-calculated conditional branches (meaning we use direct threading
        // to go to the taken side).  The untaken side is just
        // pc+instr_size and the taken is calculated from the appropriate
        // equation.  Both have to be translated, since we might cross a page.
        o << pfx << "_jit.add_cond_branch(bb," << sym._id << ",bb->bt_t(),bb->bt_f()," << icount << ","
          << num_ops << ",&di->_args[" << ap.first << "]);\n";
      } else if (sym.is_imm_branch()) {
        // Non-calculated unconditional branches (meaning we use direct
        // threading to go to the taken side; there is no untaken side).
        o << pfx << "_jit.add_uncond_branch(bb," << sym._id << ",bb->bt_t()," << icount <<","
          << num_ops << ",&di->_args[" << ap.first << "]);\n";
      } else {
        // Anything else, including calculated branches, halts, etc.
        o << pfx << "_jit.add_call(" << sym._id << "," << num_ops << ",&di->_args[" << ap.first << "]);\n";
      }

      o << "return (pc + " << bytes << ");\n"
        << "}\n";
    }

    void writeDcBranchHandlingCode(ostream &o,const Instr *instr,const InstrSym &sym,const gc_string &pfx,
                                   TMode tmode,const gc_string &bh,const gc_string &dc)
    {
      gc_string br_address;
      if (sym.br_op()) {
        br_address = sym.br_op()->name() + ".uint64()";
      }
      if (instr && instr->has_micro_op_offset()) {
        br_address += "," + instr->micro_op_offset_field() + ".uint32()";
      } else {
        br_address += ",0";
      }
      
      // We only setup direct-threading here for standalone decode-caches.
      // Otherwise, direct-threading is implemented by the LLVM portion.
      if (tmode == DcTrans) {
        if (sym.is_noncalc_branch()) {
          // For the decode-cache, we handle conditional branches and
          // unconditional branches similarly- if they're taken (which is always
          // true for unconditional branches), then we branch to the bb's branch
          // target.  Otherwise, we go to pc+<instr-size>.
          o << pfx << dc << ".set_br_trg(di," << pfx << bh << ".get_bb_or_stub(" << br_address << "," << pfx << dc << "),true);\n";
          if (sym.is_cond_branch()) {
            // If it's conditional, then we need the not-taken side calculated.
            o << pfx << dc << ".set_br_trg(di," << pfx << bh << ".get_bb_or_stub(pc+" << (instr->width()/8) << ",0," << pfx << dc << "),false);\n";
          }
        }
        else if (sym.is_sb_2nd()) {
          // The taken-side of the conditional is taken from the _sb_trg
          // (split-branch target) member variable- we have to store that into our
          // taken side.  If it's conditional, then we need the not-taken side
          // calculated.
          o << pfx << dc << ".set_br_trg(di," << pfx << "_sb_trg,true);\n";
          if (sym.is_cond_split_branch()) {
            o << pfx << dc << ".set_br_trg(di," << pfx << bh << ".get_bb_or_stub(pc+" << (instr->width()/8) << ",0," << pfx << dc << "),false);\n";
          }
        }
        else if (sym.is_sb_1st()) {
          // The taken-side of a conditional branch is calculated here and
          // stored in the _sb_trg member variable.  It's used as the target
          // during translation, unless canceled by another instruction or by
          // the end of the basic block.
          o << pfx << "_sb_trg = " << pfx << bh << ".get_bb_or_stub(" << br_address << "," << pfx << dc << ");\n";
        }
        else if (sym.is_sb_cancel()) {
          o << pfx << "_sb_trg = 0;\n";
        }
      } else {
        // However, even if we're in JIT mode, we still have to query the
        // targets, in case they cause protection faults.  This also loads them
        // into the hash as potential targets.
        if (sym.is_noncalc_branch()) {
          // For the decode-cache, we handle conditional branches and
          // unconditional branches similarly- if they're taken (which is always
          // true for unconditional branches), then we branch to the bb's branch
          // target.  Otherwise, we go to pc+<instr-size>.
          o << "bb->set_bt_t(" << pfx << bh << ".get_bb_or_stub(" << br_address << "," << pfx << "_jit));\n";
          if (sym.is_cond_branch()) {
            // If it's conditional, then we need the not-taken side calculated.
            o << "bb->set_bt_f(" << pfx << bh << ".get_bb_or_stub(pc+" << (instr->width()/8) << ",0," << pfx << "_jit));\n";
          }
        }
      }
    }

    void writeDcTranslationCode(ostream &o,const Instr *instr,const InstrSym &sym,const gc_string &pfx,TMode tmode)
    {
      if (instr) {
        gc_string dpfx,bh,dc;
        if (tmode == JitTrans) {
          dpfx = "Jit";
          bh = "_jit_bb_hash";
          dc = "_jit_decode_cache";
        } else {
          dpfx = "Dc";
          bh = "_dc_bb_hash";
          dc = "_dc_decode_cache";
        }

        gc_string di = dpfx + "DecodeItem";

        o << _class_name_pfx << di << " *di = " << pfx << dc << ".cur();\n";
        if (sym.is_sb_2nd() && tmode == DcTrans) {
          // If this is the second half of a split branch, then we select the
          // fixed form (for direct basic block handling) if we have a valid
          // target.
          o << "di->set_func((_sb_trg) ? " << getDcInstrPrefix(DcTransFixed) << sym._id << " : " << getDcInstrPrefix(DcTrans) << sym._id << ");\n";
        } else {
          o << "di->set_func(" << getDcInstrPrefix(tmode) << sym._id << ");\n";
        }

        if (tmode == JitTrans) {
          o << "di->set_jit_trans(" << sym._trans << ");\n";
        }

        // First, set up the call to the handler.  Then, deal with the special
        // case of branches.  Arguments start with the opcode (for optional
        // tracing), then proceed to the operands.
        unsigned elm = 0;
        unsigned iindex = 0;
        if (_config.trace_mode()) {
          ForRange((instr->width()/32),i) {
            o << "di->set_arg(" << elm++ << "," << pfx << _iname << "[" << iindex++ << "]);\n";
          }
          if ((instr->width() % 32)) {
            o << "di->set_arg(" << elm++ << "," << pfx << _iname << "[" << iindex << "]);\n";
          }
        }
        // If this is a immediate branch, and we implement the temporal-decoupling
        // API, then this is the count for the number of instructions in the bb.
        if (td_api() && (basic_block_end(*instr,sym) || optional_basic_block_end(*instr))) {
          o << "di->set_arg(" << elm++ << ",count+1);\n";
        }
        // OK, here are the operand args.
        ForRange(instr->sizeFields(),i) {
          Instr::Field fp = instr->getField(i);
          if (skip_field(fp)) {
            continue;
          }
          const InstrField &field = *(fp._field);
          o << "di->set_arg(" << elm++ << "," << getInstrFieldName(field,0,false) << ");\n";
        }

        writeDcBranchHandlingCode(o,instr,sym,pfx,tmode,bh,dc);

        writeGraphiteInstrCode(o,*instr,sym);

        o << pfx << dc << ".add_item();\n";
      }
    }

    // Writes code for decode cache translation for a micro-op instruction.  The
    // idea here is that we recursively call our translator, so that the basic
    // block consists of the real instruction handlers.  The idea is also that
    // we pay attention to the opcode mask (if it exists), since it's only
    // relevant at the start of a basic block, since it's used by branches.
    void writeDcTranslationMicroOpCode(ostream &o,const Instr &instr,const InstrSym &sym,
                                       const gc_string &pfx,TMode tmode)
    {
      unsigned index = 0;

      gc_string extra_fetch_arg;
      if (_max_instr_width > 32) {
        extra_fetch_arg = ",false";
      }

      writeMicroOpOffsetPreamble(o,instr);      
      
      ForEach(instr.getFields(),iter) {
        if (const InstrField *field = iter->_field) {
          if (field->type() == InstrField::tInstr) {
            const gc_string &field_name = field->name();
            
            // We have a micro-op, so generate code to call it.
            const gc_string &table_name = field->instrtablename();
            const InstrTable *instr_table = _core.get_instrtable(table_name);

            // Round widths to neareset 32-bit word.
            unsigned instr_table_width = instr_table->getWidth();
            unsigned num = instr_table_width/32;
            unsigned max_num = _core.instr_width()/32;

            writeMicroOpOffsetControl(o,instr,index);            

            o << "{\n"
              << "fill_in_instr_buf(" << field_name << "," << _iname << "," << num << ","
              << max_num << "," << (_core.instrEndianness() == Core::Little) << ");\n"
              << "addr_t tmp = pc;\n"
              << "dc_translate_instr(tmp,bb,count" << extra_fetch_arg << "," << table_name << ");\n"
              << "}\n";

            ++index;
          }
        }
      }

      writeMicroOpOffsetPostamble(o,instr);
    }
    
    Ptree *getTransFuncName(const InstrSym &sym,TMode tmode)
    {
      return (tmode == JitTrans) ? sym._jit_trans : sym._dc_trans;
    }

    void writeDcTranslationHandlerDecl(ostream &o,const Instr &instr,InstrSym &sym,TMode tmode, CodeType ctype)
    {
      gc_string ipfx;
      if (ctype == cFull) {
        ipfx += "inline ";
      }

      Ptree *fname = getTransFuncName(sym,tmode);

      // Ugh!  For MSVC, we exceed some limit on the number of methods we can have
      // in a class, so to get around this we make these functions external
      // functions in a multi-jobs model.
      gc_string iss_parm;
      if (multi_compile()) {
        iss_parm = _class_name + " &iss,";
      }

      gc_string pfx = "void ";
      qualifyName(pfx,ctype);

      gc_string bb_type;
      if (multi_compile()) {
        bb_type = _class_name + "::";
      }
      bb_type += (tmode == JitTrans) ? "MyJitBasicBlock" : "DcBasicBlock";

      o << "// Decode-cache translation function for instruction \"" << instr.name() << "\"\n"
        << ipfx << pfx << fname << "(" << iss_parm << bb_type << " *bb,addr_t pc,unsigned count";

      bool first = false;

      writeInstrHandlerArgs(o,instr,first,true);
      o << ")"; 
    }

    // Create the instruction translation function for decode-cache translation.
    // This translates encoded instructions into decode elements in the decode
    // cache.  It's used by both the standalone decode-cache and the JIT.  In the
    // case of the JIT, this is used for the first stage of use.  Hot blocks are
    // then further translated into native code via LLVM.
    void generateDcTranslationHandler(ostream &o,const Instr &instr, TMode tmode, CodeType ctype )
    {
      // The action function's name is the symbol in the hash.
      InstrSym &sym  = _instr_syms.find(instr.name())->second;

      // The translation function looks like an instruction handler, in that it
      // takes the extracted, original fields, creates any new, modified values as
      // necessary, then stores them as a decode item.

      writeDcTranslationHandlerDecl(o,instr,sym,tmode,ctype);

      if (declOnly(o,ctype)) return;

      o << "\n{\n";

      gc_string iss_arg, iss_pfx;
      if (multi_compile()) {
        iss_arg = "iss,";
        iss_pfx = "iss.";
      }

      if (const Instr::Alias *alias = instr.get_alias()) {
        // Alias:  Call the target function intead.
        Ptree *target = _instr_syms.find(alias->_target->name())->second.trans(tmode);
        o << target;
        writeAliasInstrHandlerCall(o,instr,alias,iss_arg+"bb,pc,count");
        o << "}\n\n";
      } else if (instr.has_micro_ops() && !instr.action()) {
        // Micro-ops: If we have an implicit action, then expand this
        // instruction into its constituent items.
        writeInstrFieldModifiers(o,instr,"","","pc");
        writeDcTranslationCode(o,&instr,sym,iss_pfx,tmode);
        writeDcTranslationMicroOpCode(o,instr,sym,iss_pfx,tmode);
        o << "}\n\n";
      } else {
        // Write any field modifiers here.
        writeInstrFieldModifiers(o,instr,"","","pc");
        writeDcTranslationCode(o,&instr,sym,iss_pfx,tmode);
        o << "}\n\n";
      }
    }

    // This creates the decode-miss handling function for the decode-cache.
    void generateDcDecodeMissTrans(const gc_string &msg,const InstrSym &sym)
    {
      _o << "// " << msg << ".\n"
         << "inline void " << sym._dc_trans << "(addr_t ea,uint32_t instr)\n{\n"
         << "DcDecodeItem *di = _dc_decode_cache.add_item();\n"
         << "di->set_func(dc_decode_cache_decode_miss);\n"
         << "di->_args[0] = (ea >> 32);\n"
         << "di->_args[1] = ea;\n"
         << "di->_args[2] = instr;\n"
         << "}\n\n";
    }

    // This creates the decode-miss handling function for the JIT.
    void generateJitDecodeMissTrans(const gc_string &msg,const InstrSym &sym)
    {
      _o << "// " << msg << ".\n"
         << "static addr_t " << sym._trans << "(" << _class_name << " &iss,MyJitBasicBlock *,JitDecodeItem *di,addr_t pc)\n{\n"
         << "addr_t ea = di->_args[0];\n"
         << "ea = (ea << 32) | di->_args[1];\n"
         << "iss._jit.add_call(" << sym._id << ",ea,di->_args[2]);\n"
         << "return ea;\n"
         << "}\n\n"

         << "// " << msg << ".\n"
         << "inline void " << sym._jit_trans << "(addr_t ea,uint32_t instr)\n{\n"
         << "JitDecodeItem *di = _jit_decode_cache.add_item();\n"
         << "di->set_func(jit_decode_cache_decode_miss);\n"
         << "di->set_jit_trans(" << sym._trans << ");\n"
         << "di->_args[0] = (ea >> 32);\n"
         << "di->_args[1] = ea;\n"
         << "di->_args[2] = instr;\n"
         << "}\n\n";
    }

    // Generate the code for handling a prefix instruction
    void generatePrefixInstrHandler(const Instr &instr) 
    {
      InstrSym &sym = _instr_syms.find(instr.name())->second;

      _o << "// Disassembler for prefix instruction \"" << instr.name() << "\"\n"
         << "static int " << sym._disasm << "(std::ostream &os, addr_t memaddr,const uint32_t* insn)\n{\n";
      _disasm.writeDisTableEntry(_o,instr,instr.shorthands());
      _o << "}\n\n"

         << "// Prefix Instruction \"" << instr.name() << "\"\n"
         << "inline void " << " " << sym.exec_func() << "(";

      // Write prefix call arguments
      bool first = true;
      writeInstrHandlerArgs(_o,instr,first,false);
      _o << ")\n{\n";

      genPrefixFieldSetup(_o,_core,instr,_core.name()+"::");

      if(instr.action()) {
        /*
          if (transactional_iss()) {
          _walker.set_slots( &(_genip.instr_slots()->find(instr.name())->second) );
          }
        */
        // Transform any ADL RTL extensions into C++.
        Ptree *newaction = _walker.translateCode(instr.env(),instr.action());

        writeLineDirective(instr.env(),instr.action());
        _walker.set_slots(0);
        _o << newaction << "\n";
        writeCurLocation();
      }

      if (instr.next_table()) {
        // Insert the next-table code, if applicable.
        _o << _walker.translateCode(instr.env(),instr.next_table()) << '\n';
      }

      _o << "}\n\n";
    }


    // Generates coroutine support for the commit handler, if necessary,
    // i.e. it's possible that a resource we're writing to accesses cache or
    // memory.
    void generateCommitCoroHandler(ostream& o,const InstrSlotAlloc &isa,const Instr *instr,const gc_string &instrname,CodeType ctype)
    {
      o << "// Commit coroutine driver function for \"" << instrname << "\"\n";
      if (ctype != cImpl) {
        o << "static ";
      }
      gc_string pfx = "void";
      qualifyName(pfx,ctype);
      o  << pfx << " " << isa._cc_name << "(Coro *co," << _core.name() << " *core," << PktType << " *" << PktArg << ")";
      if (!declOnly(o,ctype)) {
        o << "{\n";

        // Emit no code if we have no exceptions.
        if (haveExceptions()) {
          o << "try {\n";
        }
        o << "core->" << isa._cm_name << "(" << PktArg << "," << PktArg << "->_opc," << PktArg << "->_commit_index);\n";

        // Emit no code if we have no exceptions.
        if (haveExceptions()) {
          o << "}\n"
            << "catch (Exception x) {\n"
            // << PktName "->_exdata |= x;\n"
            << "core->setException(" << PktArg << ", x);\n"
            << "}\n";
        }

        o << "}\n\n";        
      }

      gc_string pfx2 = "MemAccess::OpType";
      qualifyName(pfx2,ctype);
      o << "// Commit driver function for \"" << instrname << "\"\n"
        << pfx2 << " " << isa._cd_name << "(" << PktType << " *" << PktArg << ", uint32_t *opc, unsigned index)\n";
      if (!declOnly(o,ctype)) {
        o << "\n{\n"
          << PktArg << "->_commit_index = index;\n"
          << PktArg << "->_cco.call();\n"
          << "if (_memory_request.op() == MemAccess::opNone) {\n"
          << PktArg << "->_cco.reset(" << isa._cc_name << ",this," << PktArg << ",_main_context," << PktArg << "->_cstack->data(),StackSize);\n"
          << "}\n"
          << "return _memory_request.op();\n"
          << "}\n\n";
      }
    }

    // Generates the commit functions, if we're creating a transactional ISS.
    void generateCommitInstrHandler(ostream& o,const InstrSlotAlloc &isa,const Instr *instr,const gc_string &instrname,CodeType ctype)
    {
      o << "// Commit function for \"" << instrname << "\"\n";
      gc_string prefix = "MemAccess::OpType ";
      qualifyName(prefix,ctype);
      o  << prefix << " " << isa._cm_name << "(" << PktType << " *" << PktArg << ", uint32_t *opc, unsigned index)";
      if (!declOnly(o,ctype)) {
        o << "\n{\n";
        if (hasTargets(isa)) {
          o << "switch (index) {\n";

          gc_ostringstream os;
          bool     first = true;
          ForRange(_core.instr_width()/32,k) {
            list_sep(os,first);
            os << "opc[" << k << "]";
          }
          gc_string opcd = os.str();

          auto alias = (instr) ? instr->get_alias() : 0;

          ForEach(isa._items,iter) {
            const UsedReg  &ur = iter->first;
            const SlotInfo &si = iter->second;

            if (si._trg) {

              if (ur.is_fixed() || ur.reg()) {
                o << "case " << si._start << ":\n";
                if (ur.conditional()) {
                  o << "if ( " << PktArg << "->is_dirty(" << si._start << ") ) {\n";
                }
              }

              gc_string wfunc;
              gc_string append;
              if ( ur.partial() ) {
                wfunc = "_mask_set";
              } else {
                wfunc = "_write";
                if (ur.delayed()) {
                  wfunc += "_delayed";
                }
                append = "._v";
              }

              // If all elements are fixed, then the commit is static.  Otherwise,
              // the commit function checks the dirty bits and writes those
              // registers back.  Note that we treat ranges just like non-fixed
              // register file accesses for now.  This might be changed later.
              if (const Reg *reg = ur.reg()) {
                o << reg->name() << wfunc << "(" << PktArg << "->_slots[" << si._start << "]" << append;
                if (ur.delayed()) {
                  o << "," << PktArg << "->_slots[" << si._start << "]._d";
                }
                o << ");\n";
              } else if (const RegFile *rf = ur.regfile()) {
                if (ur.is_fixed()) {
                  o << rf->name() << wfunc << "(";
                  adl::writeFieldAccessor(o,_core,ur._begin,instr,alias,opcd);
                  o << "," << PktArg << "->_slots[" << si._start << "]" << append;
                  if (ur.delayed()) {
                    o << "," << PktArg << "->_slots[" << si._start << "]._d";
                  }
                  o << ");\n";
                } else {
                  // We don't have a fixed element that's accessing this register
                  // file, so we have to write back all registers whose dirty bits are set.
                  for (int i = si._start,index = 0; i != si._stop; ++i,++index) {
                    o << "case " << i << ":\n"
                      << "if (" << PktArg << "->is_dirty(" << i << ")) {\n"
                      << rf->name() << wfunc << "(" << index << "," << PktArg << "->_slots[" << i << "]" << append;
                    if (ur.delayed()) {
                      o << "," << PktArg << "->_slots[" << i << "]._d";
                    }
                    o << ");\n"
                      << "}\n"
                      << "break;\n";
                  }
                }

              } else {
                RError("Unknown used-reg type.");
              }

              if (ur.is_fixed() || ur.reg()) {
                if (ur.conditional()) {
                  o << "}\n";
                }
                o << "break;\n";
              }

            } else {
              o << "case " << si._start << ":\n"
                << "break;\n";
            }
          }

          o << "}\n";
        }
        o << "return MemAccess::opNone;\n"
          << "}\n\n";
      }

      // If it's possible that write hooks access cache or memory, then we have to set up coroutine support.
      if (uses_coro_commit(isa)) {
        generateCommitCoroHandler(o,isa,instr,instrname,ctype);
      }

    }


    void generateCommitInstrHandler(ostream& o,const Instr &instr,CodeType ctype)
    {
      const InstrSlotAlloc &isa = _genip.instr_slots()->find(instr.name())->second;
      generateCommitInstrHandler(o,isa,&instr,instr.name(),ctype);
    }

    void writeOpClear(ostream &o,const UsedReg &ur,const SlotInfo &si,const Instr *instr)
    {
      gc_string type = getOpTypeEnum(ur,si);

      if (ur.reg()) {
        writeSlotReset(o,ur,si,uitogs(si._start));
      } else if (ur.regfile()) {
        gc_ostringstream os;
        bool     first = true;
        ForRange(_core.instr_width()/32,k) {
          list_sep(os,first);
          os << _iname << "[" << k << "]";
        }
        gc_string opc = os.str();

        auto alias = (instr) ? instr->get_alias() : 0;
        if (ur.is_fixed()) {
          writeSlotReset(o,ur,si,uitogs(si._start));
        } else if (ur.is_range()) {
          // For a range, we don't really know what elements will be accesssed.
          // So, we have to clear the whole range.
          o << "{\n"
            << "unsigned c = 0, i = " << si._start << ";\n"
            << "unsigned start = ";
          adl::writeFieldAccessor(o,_core,ur._begin,instr,alias,opc);
          o << ";\n"
            << "unsigned stop = ";
          adl::writeFieldAccessor(o,_core,ur._end,instr,alias,opc);
          o << ";\n"
            << "for ( ; c != start; ++i, ++c) {\n";
          writeSlotReset(o,ur,si,"i");
          o << "}\n"
            << "for ( ; c <= stop; ++i, ++c) {\n";
          writeSlotReset(o,ur,si,"i");
          o << "}\n"
            << "for ( ; i != " << si._stop << "; ++i, ++c) {\n";
          writeSlotReset(o,ur,si,"i");
          o << "}\n"
            << "}\n";
        } else {
          // We don't have a fixed element that's accessing this register
          // file, so we have to store information about all elements.
          o << "{\n"
            << "unsigned c = 0;\n"
            << "for (unsigned i = " << si._start << "; i != " << si._stop << "; ++i, ++c) {\n";
          writeSlotReset(o,ur,si,"i");
          o << "}\n"
            << "}\n";
        }
      } else {
        RError("Unknown used-reg type.");
      }

    }

    // Generates the quick-reset function, if we're creating a transactional ISS
    // with instruction caching.
    void generateQuickResetInstrHandler(ostream& o,const InstrSlotAlloc &isa,const Instr *instr,
                                        unsigned instr_width,const gc_string &instr_name,
                                        const gc_string &instr_disasm,const gc_string &instr_flags,
                                        CodeType ctype)
    {
      o << "// Quick-reset function for \"" << instr_name << "\"\n";
      gc_string prefix = "void ";
      qualifyName(prefix,ctype);
      o  << prefix << " " << isa._qr_name << "(" << PktType << " *x)";
      if (!declOnly(o,ctype)) {
        o << "\n{\n";

        // We do the prefetch logging here b/c we want to make sure that these
        // are grouped properly.  In a uADL model, fetching is actually very
        // disconnected from decode, so this is a bit of a hack to get a DAT to
        // look the same between both types of model.
        if (_config.trace_mode()) {
          o << "if (Tracing && (_trace_mode & ttInstr)) {\n"
            << "_logger->log_current_instr(x);\n"
            << "_logger->log_instr(&" << _iname << "[0]," << dec
            << instr_width << ",\"" << instr_name << "\"," << instr_disasm << "," << instr_flags << ");\n"
            << "}\n";
        }
      
        bool c_e = uses_coro_exec(isa);
        bool c_c = uses_coro_commit(isa);
        if (c_e) {
          // If we access memory, then this is going to be treated as a coroutine.
          o << "x->_co.reset(" << isa._co_name << ",this,x,_main_context,(x->_stack = _coro_stacks.alloc())->data(),StackSize);\n";
        }
        if (c_c) {
          o << "s->_cco.reset(" << isa._cc_name << ",this,x,_main_context,(x->_cstack = _coro_stacks.alloc())->data(),StackSize);\n";
          _has_commit_stack = true;
        }

        if (instr) {
          writePostFetchHandler(o,"",instr_name,instr->enable_post_fetch(),instr->fetch_width()/8,false);
        }

        ForEach(isa._items,iter) {
          const UsedReg  &ur = iter->first;
          const SlotInfo &si = iter->second;
        
          writeOpClear(o,ur,si,instr);
        }

        o << "}\n\n";
      }
    }

    void generateQuickResetInstrHandler(ostream& o,const Instr &instr,CodeType ctype)
    {
      const InstrSlotAlloc &isa = _genip.instr_slots()->find(instr.name())->second;
      InstrSym &sym = _instr_syms[instr.name()];
      generateQuickResetInstrHandler(o,isa,&instr,instr.width()/4,instr.name(),sym._disasm->ToString(),
                                     getInstrFlags(sym),ctype);
    }

    void generateQuickResetDecodeMissHandler(ostream& o,CodeType ctype)
    {
      const InstrSlotAlloc &isa = _genip.decode_miss_slots();

      generateQuickResetInstrHandler(o,isa,0,_core.instr_width()/4,"<illop>","disasm_illop","0",ctype);
    }

    void writeFieldId(ostream &o,Ptree *exp,const Instr &instr,const Instr::Alias *alias)
    {
      FieldIdWalker w(o,*this,&instr,alias);
      // We don't use the result of the translation- we just care about the field id that's stored.
      w.Translate(exp);
      o << w.field_id();
    }

    // Create a function which associates slots with instruction fields
    void generateFieldInfoInstrHandler(ostream &o,const Instr &instr,CodeType ctype) 
    {
      const InstrSlotAlloc &isa = _genip.instr_slots()->find(instr.name())->second;
      const IssUsedRegs  &items = isa._items;

      gc_string pfx1,pfx2;
      if (ctype != cImpl) {
        pfx1 = "static ";
      }
      qualifyName(pfx2,ctype);

      o << "// FieldId getter for \"" << instr.name() << "\"\n"
        << pfx1 << "int " << pfx2 << " " << isa._fi_name << "(unsigned index)\n";
      if (!declOnly(o,ctype)) {
        o << "{\n"
          << "switch(index)\n"
          << "{\n";

        ForEach(items,i) { // Generate lists of field names
          const UsedReg  &ur = i->first;
          const SlotInfo &si = i->second;
          if (ur.reg()) {
            o << "case " << si._start  << ":  // " << ur.regb()->name() << "\n"
              << "return -1;\n"
              << "break;\n";
          } else if (ur.regfile()) {
            auto alias = instr.get_alias();
            if (ur.is_fixed()) {
              o << "case " << si._start  << ":  // " << ur.regb()->name() << "\n"
                << "return ";
              writeFieldId(o,ur._begin,instr,alias);
              o << ";\n"
                << "break;\n";
            } else {
              unsigned sz = si._stop - si._start;
              unsigned n  = si._start;
              ForRange(sz,k) {
                o << "case " << n << ": // " << ur.regb()->name() << "[" << k << "]\n"
                  << "return -1;\n"
                  << "break;\n";
                n++;
              }
              if (!sz) {
                o << "case " << n << ": // " << ur.regb()->name()
                  << "return -1;\n" 
                  << "break;\n"; 
              }
            }
          }
        }
        o << "default:\n" 
          << "std::cerr << \"Attempt to access illegal operand index :\" << index << \"\\n\";\n"
          << "abort();\n"
          << "}\n"
          << "\n"
          << "return -1;\n"
          << "}\n\n";
      }
    }
    // Create function which returns the instructions's attribute list
    void generateAttrInfoInstrHandler(opfxstream &o,const Instr &instr,CodeType ctype)
    {
      gc_string instrname = instr.name();
      gc_string bld_attr_name = AdlGenSym()->ToString();
      const InstrSlotAlloc &isa = _genip.instr_slots()->find(instr.name())->second;

      gc_string pfx1,pfx2;
      if (ctype != cImpl) {
        pfx1 = "static ";
      }
      qualifyName(pfx2,ctype);
      gc_string prefix = "const Attrs::AttrList &";

      if (!declOnly(o,ctype)) {
        o << "// Build attribute list for \"" << instrname << "\"\n"
          << "static " << prefix << " " << bld_attr_name << "()\n"
          << "{\n"
          << "static Attrs::AttrList params;\n";
	
        ForEach(instr.attrs(),i) {
          AttrsHelper ah(&i->second);
          ah.gen_var(o);
          o << "params.push_back(std::make_pair(" << i->first << "," << ah.gen_constr() << "));\n";
        }
        o << "return params;\n"
          << "}\n"
          << "\n";
      }

      o << "// Attribute getter for \"" << instrname << "\"\n"
        << pfx1 << prefix << " " << pfx2 << isa._at_name << "()\n";
      if (!declOnly(o,ctype)) {
        o << "{\n"
          << "static Attrs::AttrList params = " <<  bld_attr_name << "();\n"
          << "return params;\n"
          << "}\n"
          << "\n";
      }
    }

    void calcInstrWidths()
    {
      unsigned min_bytes = 0;

      const InstrHash &instrs = _core.instrs();

      // Collect statistics about instructions.
      ForEach(instrs,i) {
        const Instr     &instr = *(i->second);
        if (!instr.is_alias()) {
          _max_instr_width = max(_max_instr_width,instr.width());
          _min_instr_width = min(_min_instr_width,instr.width());
          _max_dc_args = max(_max_dc_args,instr.sizeFields());
        }
      }

      min_bytes = (_min_instr_width/8);

      _trans_shift = 0;

      // If the smallest instruction is a power of 2 then we can reduce the size
      // of the offset-array in each code page by shifting away unused bytes.
      // For example, if all instructions are 32-bit, then we can shift an
      // address by 2 bits and thus reduce the size of the offset array by a
      // factor of 4.
      if (is_power_2(min_bytes)) {
        _trans_shift = log2(min_bytes);
      }
    }

    bool hasMultiInstrLengths() const
    {
      return _min_instr_width != _max_instr_width;
    }

    unsigned getNumDcArgs()
    {
      int max_logging_args = _core.instr_width()/32;
      if (_core.instr_width() % 32) ++max_logging_args;

      // Need one more item (potentially) for the temporal-decoupling API.
      if (td_api()) {
        ++max_logging_args;
      }

      return _max_dc_args + max_logging_args;
    }

    string getDcArgType()
    {
      return (_cia->width() > 32) ? "uint64" : "uint32";
    }

    void writeDcArgSetter(const string &arg_base)
    {
      _o << "using Base::set_arg;\n"
         << "template <size_t Nb> void set_arg(unsigned i,const bits<Nb> &x)  { _args[i] = x." << arg_base << "(); };\n"
         << "template <size_t Nb> void set_arg(unsigned i,const sbits<Nb> &x) { _args[i] = x." << arg_base << "(); };\n";
    }

    // Write decode cache parameters and stuff.
    void writeDcParms()
    {
      if (!dc_support()) {
        return;
      }

      int num_args = getNumDcArgs();
      string arg_base = getDcArgType();

      string di_extra_parm, bb_extra_parm;
      if (graphite_support()) {
        di_extra_parm = ",GraphiteDecodeItem";
      }

      _o << "struct DcDecodeItem;\n"
         << "struct DcBasicBlock;\n"
         << "\n";

      _o << "typedef void (__fastcall *DcVoidFunc)(" << _core.name() << "*,DcDecodeItem *) FASTCALL;\n"
         << "typedef std::pair<DcVoidFunc,DcDecodeItem *> DcDecodePair;\n"
         << "typedef DcVoidFunc (__fastcall *DcDecodeFunc)(" << _core.name() << "*,DcDecodeItem *) FASTCALL;\n"
         << "typedef DcVoidFunc DcDecodeReturn;\n"
         << "\n"
         << "struct DcDecodeItem : public TDcDecodeItem<" << _core.name() << ",DcBasicBlock," << arg_base << "_t," << num_args << di_extra_parm << ">\n"
         << "{\n"
         << "typedef TDcDecodeItem<" << _core.name() << ",DcBasicBlock," << arg_base << "_t," << num_args << di_extra_parm << "> Base;\n"
         << "DcDecodeItem() {};\n"
         << "DcDecodeItem(DecodeFunc f) : TDcDecodeItem<" << _core.name() << ",DcBasicBlock," << arg_base << "_t," << num_args << di_extra_parm << ">(f) {};\n";
      writeDcArgSetter(arg_base);
      _o << "};\n"
         << "struct DcBasicBlock : public TDcBasicBlock<DcDecodeItem>\n"
         << "{\n"
         << "DcBasicBlock() {};\n"
         << "DcBasicBlock(addr_t ra) : TDcBasicBlock<DcDecodeItem>(ra) {};\n"
         << "};\n\n";

      writeDcTrampolines("dc","Dc","DcDecodeItem");

      _o << "struct DcDecodeCache : public TDcDecodeCache<DcDecodeItem,DcBasicBlock> {\n"
         << "typedef TDcDecodeCache<DcDecodeItem,DcBasicBlock> Base;\n"
         << "typedef Base::DecodeElm DecodeElm;\n"
         << "typedef Base::BlockType BlockType;\n"
         << "DcDecodeCache(DecodeElm *br_stub,DecodeElm *brkpt_stub,size_t size) : Base(br_stub,brkpt_stub,size) {}\n\n";

      _o << "};\n\n"
         << "struct DcTranslate {\n"
         << headerOnly2()
         << "bool full(" << _core.name() << " &core) { return core.decode_cache_full(); };\n"
         << "void operator()(" << _core.name() << " &core,addr_t ea,DcBasicBlock *bb) {\n"
         << "core.translate_dc(ea,bb);\n"
         << "}\n"
         << endHeaderOnly()
         << "};\n";

      // Hash which stores basic blocks.
      gc_string etype;
      if (haveExceptions()) {
        etype = ",Exception";
      }
      _o << "typedef TBasicBlockHash<" << _core.name() << ",DcBasicBlock,DcDecodeCache,DcTranslate," 
         << _trans_shift << "," << _config.jit_ea_hash_size() << etype << "> DcBasicBlockHash;\n\n";

      _cd._member_vars.push_back("DcDecodeItem     _dc_decode_cache_br_stub");
      _cd._member_vars.push_back("DcDecodeItem     _dc_decode_cache_brkpt_stub");
      _cd._member_vars.push_back("DcDecodeCache    _dc_decode_cache");
      _cd._member_vars.push_back("DcBasicBlockHash _dc_bb_hash");
      _cd._member_vars.push_back("DcDecodeItem    *_dc_decode_cache_next");

      _cd._reset_stmts.push_back("_dc_bb_hash.reset(_dc_decode_cache,true)");
      _cd._reset_stmts.push_back("_dc_decode_cache.reset()");
      _cd._reset_stmts.push_back("_dc_decode_cache_next = 0");

      _cd._constr_inits.push_back("_dc_decode_cache_br_stub(dc_decode_cache_branch_trampoline)");
      _cd._constr_inits.push_back("_dc_decode_cache_brkpt_stub(dc_decode_cache_breakpoint_trampoline)");
      _cd._constr_inits.push_back("_dc_decode_cache(&_dc_decode_cache_br_stub,&_dc_decode_cache_brkpt_stub,TranslationParms::dc_buffer_size())");
      _cd._constr_inits.push_back("_dc_bb_hash(*this)");
      _cd._constr_inits.push_back("_dc_decode_cache_next(0)");  

      _cd._member_vars.push_back("unsigned _dc_escapes");
      _cd._reset_stmts.push_back("_dc_escapes = 0");
      _cd._constr_inits.push_back("_dc_escapes(0)");
    }

    void writeDcTrampolines(const gc_string &pfx,const gc_string &tpfx,const gc_string &ditype)
    {
      _o << "static " << tpfx << "DecodeReturn __fastcall " << pfx << "_decode_cache_branch_trampoline(" << _core.name() << " *core," << ditype << " *di) FASTCALL {\n"
         << "core->_" << pfx << "_decode_cache_next = 0;\n";
      // Check the td counter in case we've exceeded our count.
      if (td_api()) {
        add_td_check(_o,"core->","return (" + tpfx + "VoidFunc)JitRetCode;\n","",false,false,true,false);
      }
      _o << "return (" << tpfx << "VoidFunc)JitNopCode;\n"
         << "}\n\n"

         << "static " << tpfx << "DecodeReturn __fastcall " << pfx << "_decode_cache_breakpoint_trampoline(" << _core.name() << " *core," << ditype << " *di) FASTCALL {\n"
         << "core->_" << pfx << "_decode_cache_next = 0;\n"
         << "return (" << tpfx << "VoidFunc)JitBreakPtCode;\n"
         << "}\n\n";
    }


    // Write JIT parameters.
    void writeJitParms()
    {
      if (!jit_support()) {
        return;
      }

      int num_args = getNumDcArgs();

      // Note: The JIT has not yet been ported over to handle 64-bit
      // architectures.
      string arg_base = "uint64";

      _cd._reset_stmts.push_front("_jc.signal_reset()");

      _cd._member_vars.push_front("JitCompiler _jc");
      _cd._constr_inits.push_front("_jc(this,(void*)jit_decode_cache_branch_trampoline,(void*)jit_decode_cache_breakpoint_trampoline,_jit,TranslationParms::mt_jit_compile())");
      _cd._dstr_stmts.push_front("_jc.signal_shutdown()");

      _cd._member_vars.push_front("JitDecodeCache    _jit_decode_cache");
      _cd._reset_stmts.push_front("_jit_decode_cache.reset()");
      _cd._constr_inits.push_front("_jit_decode_cache(&_jit_decode_cache_br_stub,&_jit_decode_cache_brkpt_stub,TranslationParms::dc_buffer_size())");

      _cd._member_vars.push_front("JitDecodeItem     _jit_decode_cache_br_stub");
      _cd._constr_inits.push_front("_jit_decode_cache_br_stub(jit_decode_cache_branch_trampoline)");

      _cd._member_vars.push_front("JitDecodeItem     _jit_decode_cache_brkpt_stub");
      _cd._constr_inits.push_front("_jit_decode_cache_brkpt_stub(jit_decode_cache_breakpoint_trampoline)");

      // This pointer is used by LLVM as the global for referencing ISS state.
      _cd._member_vars.push_front(_core.name() + " *_this_ptr");
      _cd._constr_inits.push_front("_this_ptr(this)");

      // This is the main object for LLVM code generation.  We want it init'd
      // early, so we push it onto the front.
      _cd._member_vars.push_front("JitSupport _jit");
      gc_string inits;
      MkGStr(inits,"_jit(\"" << _config.jit_bcname() << "\"," << _instr_syms._id_start << "," 
             << _instr_syms._id_end << "," << _instr_syms._iid_start << "," 
             << ( (td_api()) ? _instr_syms._td_check._id : -1 )
             << ",\"" << getJitInstrPrefix() << "\",\"" << _path_str << "\",&_this_ptr,"
             << "\"" << getJitTraceVar() << "\",&_trace_mode,TranslationParms::mt_jit_compile())");
      _cd._constr_inits.push_front(inits);

      string di_extra_parm, bb_extra_parm;
      if (graphite_support()) {
        di_extra_parm = ",JitGraphiteDecodeItem";
        bb_extra_parm = ",GraphiteBlock";
      }

      _o << "struct MyJitBasicBlock;\n"
         << "struct JitDecodeItem;\n"
         << "\n";

      if (graphite_support()) {
        _o << "typedef GraphiteDecodeItem<MyJitBasicBlock,JitDecodeItem> JitGraphiteDecodeItem;\n"
           << "\n";
      }

      // Hash which stores basic blocks.
      gc_string etype;
      if (haveExceptions()) {
        etype = ",Exception";
      }
      _o << "typedef void (__fastcall *JitVoidFunc)(" << _core.name() << "*,JitDecodeItem *) FASTCALL;\n"
         << "typedef std::pair<JitVoidFunc,JitDecodeItem *> JitDecodePair;\n"
         << "typedef JitVoidFunc (__fastcall *JitDecodeFunc)(" << _core.name() << "*,JitDecodeItem *) FASTCALL;\n"
         << "typedef JitVoidFunc JitDecodeReturn;\n"
         << "typedef addr_t (*JitTransFunc)(" << _class_name << " &,MyJitBasicBlock *,JitDecodeItem *,addr_t pc);\n"
         << "\n"

         << "struct JitDecodeItem : public TJitDecodeItem<" << _core.name() << "," << arg_base << "_t," << num_args << di_extra_parm << ">\n"
         << "{\n"
         << "typedef TJitDecodeItem<" << _core.name() << "," << arg_base << "_t," << num_args << di_extra_parm << "> Base;\n"
         << "JitDecodeItem() {};\n"
         << "JitDecodeItem(JitDecodeFunc f) : TJitDecodeItem<" << _core.name() << "," << arg_base << "_t," << num_args << di_extra_parm << ">(f) {};\n";
      writeDcArgSetter(arg_base);
      _o << "};\n\n"
         << "struct MyJitBasicBlock : public TJitBasicBlock<JitDecodeItem" << bb_extra_parm << "> {\n"
         << "typedef JitDecodeItem DecodeElm;\n"
         << "MyJitBasicBlock(){};\n"
         << "MyJitBasicBlock(addr_t ra) : TJitBasicBlock<JitDecodeItem" << bb_extra_parm << ">(ra) {};\n"
         << "};\n\n"

        // TODO: Improve the full() function so that we don't reset everything
        // when we exceed either requirement.  We should only reset what's needed
        // (decode cache or LLVM compiled functions).
         << "struct JitTranslate {\n"
         << headerOnly2()
         << "bool full(" << _core.name() << " &core) { return core.jit_full(); };\n"
         << "void operator()(" << _core.name() << " &core,addr_t ea,MyJitBasicBlock *bb) {\n"
         << "core.translate_jit(ea,bb);\n"
         << "}\n"
         << endHeaderOnly()
         << "};\n\n"
         << "typedef TBasicBlockHash<" << _core.name() << ",MyJitBasicBlock,JitSupport,JitTranslate," 
         << _trans_shift << "," << _config.jit_ea_hash_size() << etype << "> JitBasicBlockHash;\n\n"
         << "typedef TJitDecodeCache<JitDecodeItem,MyJitBasicBlock> JitDecodeCache;\n\n";

      writeDcTrampolines("jit","Jit","JitDecodeItem");

      _cd._member_vars.push_back("JitBasicBlockHash _jit_bb_hash");
      _cd._reset_stmts.push_back("_jit_bb_hash.reset(_jit,false)");
      _cd._constr_inits.push_back("_jit_bb_hash(*this)");

      _cd._member_vars.push_back("JitDecodeItem *_jit_decode_cache_next");
      _cd._reset_stmts.push_back("_jit_decode_cache_next = 0");
      _cd._constr_inits.push_back("_jit_decode_cache_next(0)");
    
      _cd._member_vars.push_back("unsigned _jit_escapes");
      _cd._reset_stmts.push_back("_jit_escapes = 0");
      _cd._constr_inits.push_back("_jit_escapes(0)");

      _cd._member_vars.push_back("unsigned _jit_dc_runs");
      _cd._reset_stmts.push_back("_jit_dc_runs = 0");
      _cd._constr_inits.push_back("_jit_dc_runs(0)");
    }

    void handlerSetup()
    {
      _walker.set_class_name(_class_name,_class_name_pfx);

      // First, associate each instruction with a unique symbol name.
      // We store this information for JIT generation purposes as well.

      const InstrHash &instrs = _core.instrs();

      _instr_syms.clear();
      ForEach(instrs,i) {
        const Instr &instr = *(i->second);
        // Ignore aliases for JIT generation, so don't give it an ID.  We just
        // want a sequential sequence of IDs for all valid instructions from the
        // JIT-handler point of view.
        //
        // Note: If we have a graphite performance model, we always generate
        // instruction wrappers so that the wrapper can call the performance model
        // in interpreted mode.  For decode-cache, we create an instruction object
        // during translation and invoke it when running the basic block.
        _instr_syms.add(instr,!instr.get_alias(),_config.trace_mode(),(hasModifiedFields(instr) || graphite_support()));
      }

    }

    // This writes out various support handlers used by the JIT.
    void generateJitSupportFuncs()
    {
      if (!jit_support()) {
        return;
      }

      gc_string pfx = _path_str + "->";

      *_jo << "// Core " << _core.name() << ": JIT decode-miss function.\n"
           << "void " << getJitInstrPrefix() << _instr_syms._decode_miss._id << "(addr_t ea,unsigned instr) {\n";
      writeJitPreActionCode(*_jo,pfx,gc_string(),Instr::PostFetchTrue,_core.decode_miss_handler(),
                            true,_max_instr_width/8,JitTrans,cFull);
      *_jo << pfx << "decode_miss_handler(ea,instr);\n"
           << "}\n\n";

      if (td_api()) {
        *_jo << "// Core " << _core.name() << ": JIT temporal-decoupling check function.\n"
             << "bool " << getJitInstrPrefix() << _instr_syms._td_check._id << "(unsigned count) {\n";
        add_td_check(*_jo,pfx,"return true;","count",true,false,true,false);
        *_jo << "return false;\n"
             << "}\n\n";
      }

      // Skip if we don't have an instruction cache.  
      if (genICacheTouch(_core,_config)) {
        *_jo << "// Core " << _core.name() << ":  JIT function for touching the icache.\n"
             << "void " << getJitInstrPrefix() << _instr_syms._icache_touch._id << "(addr_t ra) {\n"
             << pfx << getJitICacheTouch() << "(ra);\n"
             << "}\n\n";

        _o << "// JIT translation function for touching the icache.\n"
           << "static addr_t " << _instr_syms._icache_touch._trans << "(" << _core.name() << " &iss,MyJitBasicBlock *bb,JitDecodeItem *di,addr_t pc) {\n"
           << "iss._jit.add_call(" << _instr_syms._icache_touch._id << ",pc);\n"
           << "return pc;\n"
           << "}\n\n";
      } else {
        _instr_syms._icache_touch.reset();
      }
    }

    void generateJitHandlerSetup()
    {
      // We need to create the global which stores the core object.
      *_jo << _class_name << " *" << _path_str << ";\n\n";
    }

    // This writes out various support functions used by the Graphite integration.
    void generateGraphiteSupportFuncs()
    {
      // We need to create a mapping function which maps
      // (register-id,register-index) -> <graphite-id>, where <graphite-id> is a
      // member of a small set of contiguous integers.  In other words, the
      // Graphite performance model's scoreboard for registers is a fixed size, so
      // rather than using a function to create a large integer for graphite-id,
      // we use a table-based approach so that the resulting Graphite scoreboard
      // can be smaller.

      int count = 0;
      if (!(_core.regfiles().empty() && _core.regs().empty())) {
        _o << "static unsigned graphite_id(unsigned id,unsigned index = 0)\n"
           << "{\n"
           << "static unsigned id_map[] = {\n";
      
        // Currently, we assign IDs consecutively to registers, then register-files
        // in alphabetical order, so there's no need to order them here first.  We
        // just need to add gaps to account for register-file size.
        ForEach(_core.regs(),i) {
          const Reg &r = *i->second;
          _o << count++ << ",\t// " << r.name() << ": " << r.id() << "\n";        
        }
        // Skip sparse register files, as we'll handle their constituent items
        // separately.
        ForEach(_core.regfiles(),i) {
          const RegFile &rf = *i->second;
          if (rf.sparse()) {
            // Put in a placeholder so that indices line up.
            _o << "-1u,\t// " << rf.name () << ": " << rf.id() << " (sparse - skipped)\n";
          } else {
            _o << count << ",\t// " << rf.name() << ": " << rf.id() << "\n";
            count += rf.size();
          }
        }
        _o << "};\n\n"
           << "switch (id) {\n";
        ForEach(_core.regfiles(),i) {
          const RegFile &rf = *i->second;
          if (!rf.sparse()) continue;
          _o << "case " << rf.id() << ":\n"
             << "switch (index) {\n";
          ForEach(rf.entries(),j) {
            const RegEntry &re = *j->second;
            _o << "case " << re.id() << ":\n";
            if (const Reg *r = re.reg().reg()) {
              _o << "return " << r->id() << ";\n";
            }
            else if (const RegFile *r = re.reg().regfile()) {
              _o << "return graphite_id(" << r->id() << "," << re.reg().index() << ");\n";
            } else {
              // If this doesn't map to a real item, then return -1 to indicate
              // that this is not really a register.
              _o << "return -1u;\n";
            }
          }
          _o << "}\n";
        }
        _o << "default:\n"
           << "return id_map[id] + index;\n"
           << "}\n"
           << "return 0;\n" // Shouldn't ever get here.
           << "}\n\n";
      }

      _o << "unsigned numGraphiteRegs() const { return " << count << "; }\n\n";

      _cd._member_vars.push_back("CoreModel *" + getGraphitePerfModel());
      _cd._constr_inits.push_back(getGraphitePerfModel() + "(0)");
      _cd._reset_stmts.push_back("if (" + getGraphitePerfModel() + ") " + getGraphitePerfModel() + "->reset()");

      _cd._member_vars.push_back("Instruction " + getGraphitePerfInstr());

      _o << "void setGraphitePerfModel(CoreModel *c) { " << getGraphitePerfModel() << " = c; };\n"
         << "CoreModel *getGraphitePerfModel() const { return " << getGraphitePerfModel() << "; };\n"
         << "bool hasGraphitePerfSupport() const { return true; };\n\n";
    }

    // This writes out various support handlers used by the decode cache.
    void generateDcSupportFuncs(TMode tmode)
    {
      if (!(jit_support() || dc_support())) {
        return;
      }

      gc_string pfx = "core->";

      gc_string tpfx,dpfx;
      if (tmode == JitTrans) {
        dpfx = "jit";
        tpfx = "Jit";

      } else {
        dpfx = "dc";
        tpfx = "Dc";
      }

      _o << "static " << tpfx << "DecodeReturn __fastcall " << dpfx << "_decode_cache_decode_miss(" << _core.name() << " *core," << tpfx << "DecodeItem *di) FASTCALL {\n";
      writeJitPreActionCode(_o,pfx,gc_string(),Instr::PostFetchTrue,true,true,_max_instr_width/8,DcTrans,cFull);
      writeInstrAttrs(_o,0,false,pfx);
      _o << "addr_t ea = di->_args[0];\n"
         << "ea = (ea << 32) | di->_args[1];\n"
         << "core->decode_miss_handler(ea,di->_args[2]);\n"
         << "core->_" << dpfx << "_decode_cache_next = 0;\n"
         << "return (" << tpfx << "VoidFunc)JitNopCode;\n"
         << "}\n\n";

      // Skip if we don't have an instruction cache.
      if (genICacheTouch(_core,_config)) {
        bool has_icache = _core.get_first_cache(InstrCache);

        _o << "// Core " << _core.name() << ":  Decode-cache function for touching the icache.\n"
           << "static " << tpfx << "DecodeReturn __fastcall " << getDcInstrPrefix(tmode) << _instr_syms._icache_touch._id << "(" << _core.name() << " *core," << tpfx << "DecodeItem *di) FASTCALL {\n"
           << "addr_t ra = di->_args[0];\n"
           << "ra = (ra << 32) | di->_args[1];\n"
           << "core->" << getJitICacheTouch() << "(ra);\n";
        // If we have an icache, then no need to exit back to the
        // simulator. Otherwise, we better do it, so that the external cache model
        // (we assume) can do what it needs to do by clearing the td threshold.
        if (!has_icache && td_api()) {
          gc_ostringstream ss1;
          // Very important- for return code to work, we have to clear the next
          // variable, but we can't clear it if we want to just continue, or else
          // this will cause a problem if the icache touch happens at the last
          // element of the decode-cache kernel- we'll just go into an infinite
          // loop.  However, we don't want to return a JotNopCode, because then
          // we'll never advance past the icache touch instruction.
          ss1 << "core->_" << dpfx << "_decode_cache_next = 0;\n"
              << "return (" << tpfx << "VoidFunc)JitRetCode;";
          add_td_check(_o,pfx,ss1.str(),"1",true,false,true,false);
        }
        _o << "return (" << tpfx << "VoidFunc)di[1].func();\n"
           << "}\n\n";
      } else {
        _instr_syms._icache_touch.reset();
      }
    }

    void generateSplitBranchSupport()
    {
      // Add in member variables for split branches, if we encountered any.
      if (_has_split_branches) {
        _cd._member_vars.push_back("DcBasicBlock *_sb_trg");
        _cd._constr_inits.push_back("_sb_trg(0)");
        _cd._reset_stmts.push_back("_sb_trg = 0");
      }
    }


    void generateInstrHandlers()
    {
      const InstrHash &instrs = _core.instrs();

      calcInstrWidths();
      writeDcParms();
      writeJitParms();

      gc_string args;

      // We add in ADL_HEADER as a define to remove some of this code if the file
      // is being used as a header.
      if (transactional_iss() && !multi_compile()) {
        _o << "#ifndef ADL_HEADER\n";
      }

      _o << "\n"
         << "// Disassembler for illops.\n"
         << "static int disasm_illop(std::ostream &os,addr_t memaddr, const unsigned *insn)\n{\n"
         << "os << \".long 0x\" << std::setfill('0') << std::setw(8) << std::hex << *insn" 
         << " << std::dec;\n"
         << "return 4;\n"
         << "}\n\n";

      if (transactional_iss() && !hybrid_iss()) {
        _o << "// Execution driver function for illops.\n"
           << "MemAccess::OpType " << _genip.decode_miss_slots()._ex_name << "(uint32_t *opc) {\n"
           << "decode_miss_handler(" << PktName << "->_addr,opc[0]);\n"
           << "return MemAccess::opNone;\n"
           << "}\n\n";

        if (!hybrid_iss()) {
          generateReadOpInstrHandler(_o,_genip.decode_miss_slots(),0,"decode-miss",true,cFull);
          generateCommitInstrHandler(_o,_genip.decode_miss_slots(),0,"decode_miss",cFull);
        }

        generateQuickResetDecodeMissHandler(_o,cFull);
      }

      if (low_mem()  && !_core.decode_retry_handler()._func) {
        generateDecodeMissLeafWrapHandler(_o,cFull);
      }
     
      if (jit_support()) {
        generateJitHandlerSetup();
        generateJitSupportFuncs();
        generateDcSupportFuncs(JitTrans);
        generateJitDecodeMissTrans("JIT decode-miss handler",_instr_syms._decode_miss);
      }

      if (dc_support()) {
        generateDcSupportFuncs(DcTrans);
        generateDcDecodeMissTrans("Decode-cache decode-miss handler",_instr_syms._decode_miss);
      }

      if (graphite_support()) {
        generateGraphiteSupportFuncs();
      }

      // Now iterate over each instruction, generating the necessary functions.
      int count = -1;
      ForEach(instrs,i) {
        ++count;
        const Instr &instr = *(i->second);

        if (skip_instruction(instr)) {
          continue;
        }

        // Direct output to an implementation file if doing multi-file compiles.  For JIT pre-phase,
        // we don't bother with this- we just split up the jit functions themselves.
        int job = get_job_num(count);
        // Non-prefix instruction
        // Abstract instructions have no encoding 
        // They maintain common action code for their aliases 
        if (transactional_iss() && !instr.is_abstract()) {
          if (job >= 0) {
            generateReadOpInstrHandler(_o,instr,cDecl);
            generateReadOpInstrHandler((*_impls)[job],instr,cImpl);
          } else {
            generateReadOpInstrHandler(_o,instr,cFull);
          }
	
          //generateReadOpInstrHandler(instr);
          generateBranchInfoInstrHandler(instr);
          if (_core.has_delayed_writes()) {
            generateDelayedWriteInstrHandler(instr);
          }
        }
        if (instr.is_prefix()) {
          generatePrefixInstrHandler(instr);
        } 

        if (job >= 0) {
          generateExecInstrHandler(_o,instr,cDecl);
          generateExecInstrHandler((*_impls)[job],instr,cImpl);
        } else {
          generateExecInstrHandler(_o,instr,cFull);
        }
	
        if (transactional_iss() && !hybrid_iss() && !instr.is_abstract()) {
          if (job >= 0) {
            generateCommitInstrHandler(_o,instr,cDecl);
            generateCommitInstrHandler((*_impls)[job],instr,cImpl);
          } else {
            generateCommitInstrHandler(_o,instr,cFull);
          }
        }
        if (transactional_iss() && cached_packets() && !instr.is_abstract()) {
          if (job >= 0) {
            generateQuickResetInstrHandler(_o,instr,cDecl);
            generateQuickResetInstrHandler((*_impls)[job],instr,cImpl);
          } else {
            generateQuickResetInstrHandler(_o,instr,cFull);
          }
        }
        if (!instr.is_prefix()) {
          if (jit_support()) {
            if (job >= 0) {
              // The JIT uses decode-cache translation for the first stage, then
              // JIT translation for hot blocks.
              generateDcTranslationHandler(_o,instr,JitTrans,cDecl);
              generateDcTranslationHandler((*_impls)[job],instr,JitTrans,cImpl);
              generateJitTranslationHandler((*_impls)[job],instr,cImpl);
            } else {
              generateDcTranslationHandler(_o,instr,JitTrans,cFull);
              generateJitTranslationHandler(_o,instr,cFull);
            }
          }
          if (dc_support()) {
            if (job >= 0) {
              generateDcTranslationHandler(_o,instr,DcTrans,cDecl);
              generateDcTranslationHandler((*_impls)[job],instr,DcTrans,cImpl);
            } else {
              generateDcTranslationHandler(_o,instr,DcTrans,cFull);
            }
          }
        }
      }

      if (dc_support()) {
        generateSplitBranchSupport();
      }

      if (transactional_iss() && !multi_compile()) {
        _o << "#endif // ADL_HEADER\n\n";
      }

    }

    void generateSubInstrs() 
    {
      const SubInstrHash  &subinstrs = _core.subinstrs();

      ForEach(subinstrs,i) {
        SubInstr &subinstr = *i->second;
        _o << "// Sub-instruction \"" << subinstr.name() << "\"\n"
           << "inline void " << subinstr.name() << "_action(";
        if (subinstr.args()) {
          _o << subinstr.args();
          bool first = false;
          writeInstrHandlerArgs(_o,subinstr,first,false);
          _o << ")\n\n"
             << _walker.translateCode(subinstr.env(),subinstr.body());
        } else {
          bool first = true;
          writeInstrHandlerArgs(_o,subinstr,first,false);
          _o << ")\n\n"
             << _walker.translateCode(subinstr.env(),subinstr.action());
        }
        writeCurLocation();
        _o << "\n\n";
      }
    }

    // Generate code for updating conexts.
    void writeContextUpdates()
    {
      _o << "void " << getCtxUpdateName() << " ()\n{\n";
      auto contexts = _core.contexts();

      if (!contexts.empty()) {
        gc_string ctxupdate = getContextUpdateVar();

        _o << "if (" << ctxupdate << ") {\n"
           << ctxupdate << " = false;\n";
        ForEach(contexts,citer) {
          const Context &ctx = *(citer->second);
          _o << getCtxUpdateFunc(ctx) << "();\n";
        }
        _o << "}\n";
      }

      _o << "}\n\n";
    }


    // This walker class substitutes instruction fields for their relevant accessors.
    class FieldIdWalker : public Walker {
    public:
      FieldIdWalker(ostream &o,GenInstrs &ginstrs,const Instr *instr,const Instr::Alias *alias) : 
        Walker((Environment*)0), 
        _o(o),
        _ginstrs(ginstrs),
        _instr(instr),
        _alias(alias),
        _field_id(-1)
      {};

      int field_id() const { return _field_id; };

      void storeId(const InstrField &ifield)
      {
        _field_id = ifield.id();
      }

      Ptree *handleVariable(Ptree *exp)
      {
        gc_string name = exp->ToString();
        int index;
        const Instr *instr = (_alias ? _alias->_target : _instr);
        if (instr && ((index = instr->getAll(name)) >= 0)) { 
          const InstrField *f = instr->getAll(index)._field;
          storeId(*f);
        }

        return exp;
      }


      Ptree *TranslateDotMember(Ptree *exp) 
      {
        Ptree *left = exp->Car();
        Ptree *member = Third(exp);

        gc_string name = left->ToString();
        int index;
        if (_instr && ((index = _instr->getAll(name)) >= 0)) {
          gc_ostringstream ss;
          const InstrField &f = *(_instr->getAll(index)._field);
          const InstrField *n = f.getField(member->ToString());
          if(n) {
            storeId(f);
          }
        }
        return Walker::TranslateDotMember(exp);
      }

      Ptree *TranslateFuncall(Ptree *exp) 
      {
        Ptree *fun = exp->Car();
        gc_string fname = fun->ToString();
        if (_instr && _instr->getAll(fname) >=0) {
          return handleVariable(fun);
        }
        if (_alias && _alias->_target->getAll(fname) >=0) {
          return handleVariable(fun);
        }
        return Walker::TranslateFuncall(exp);
      }

      Ptree *TranslateVariable(Ptree *exp)
      {
        return handleVariable(exp);
      }

    private:
      ostream            &_o;
      GenInstrs          &_ginstrs;
      bool                _little_endian;
      const Instr        *_instr;         // Instruction to search for fields.
      const Instr::Alias *_alias;         // An instruction alias, if one exists.
      int                 _field_id;      // The result.
    };   


    // This walker class substitutes instruction fields for their relevant accessors.
    class ArgSubstWalker : public Walker {
    public:
      ArgSubstWalker(ostream &o,GenInstrs &ginstrs,const Instr *instr,const Instr::Alias *alias) : 
        Walker((Environment*)0),
        _o(o),
        _ginstrs(ginstrs),
        _instr(instr),
        _alias(alias) {};

      Ptree *handleVariable(Ptree *exp)
      {
        gc_string name = exp->ToString();
        int index;
        if (_alias) {
          ForEach((_alias->_fields),i) {
            if (name  == i->_field->name()) {
              if (const InstrField *f = adl::aliasFieldTarget(_o,*this,*_instr,*i)) {
                return Ptree::Make(f->name().c_str());
              }
              else {
                return 0;
              }
            }
          }
        } else if ( _instr && ((index = _instr->getAll(name)) >= 0)) {
          gc_ostringstream ss;
          const InstrField  &f = *(_instr->getAll(index)._field);
          if (auto ef = f.encap_field()) {
            const InstrField *ifc = (ef->interface() ? ef->interface() : ef);
            return Ptree::Make("get_%s__%s(%s)",ifc->name().c_str(),f.name().c_str(),ifc->name().c_str());
          } else {
            const InstrField *ifc = (f.interface() ? f.interface() : &f);
            return Ptree::Make(ifc->name().c_str());
          }
        }
        return exp;
      }

      Ptree *TranslateDotMember(Ptree *exp) 
      {
        Ptree *left = exp->Car();
        Ptree *member = Third(exp);
        gc_string name = left->ToString();
        if (_instr && (_instr->getAll(name) >= 0)) {
          return Ptree::Make("get_%p__%p(%p)",left,member,left);
        }
        return Walker::TranslateDotMember(exp);
      }

      Ptree *TranslateFuncall(Ptree *exp)
      {
        Ptree *fun  = First(exp);
        Ptree *args = Third(exp);
        Ptree *f,*a;

        if (Match(fun,"[ %? .  %? ]", &f, &a)) {
          gc_string fname = a->ToString(); // Assume f is an instrfield expression
          if (fname == "uint32" || fname == "uint64") {
            return f;
          }
        } else {
          gc_string name = fun->ToString();
          if (_instr && (_instr->getAll(name) >= 0)) {
            int idx = _instr->getAll(name);
            const InstrField &f = *(_instr->getAll(idx)._field); 
            if (f.is_constrained()) { // table entry
              return Ptree::Make("%p__%p",fun,First(args));
            } else { // bits-slice
              return Ptree::Make("%p",fun);
            }
          }
        }
        return Walker::TranslateFuncall(exp);
      }

      Ptree *TranslateVariable(Ptree *exp)
      {
        return handleVariable(exp);
      }


      Ptree *TranslatePtree(Ptree *exp)
      {
        bool      b;
        unsigned  i;
        char     *s;
        if (get_bool(b,exp,env) || get_uint(i,exp,env) || exp->Reify(s)) {
          return exp;
        }
        return handleVariable(exp);
      }

    private:
      ostream            &_o;
      GenInstrs          &_ginstrs;
      const Instr        *_instr;  // Instruction to search for fields.
      const Instr::Alias *_alias;  // An instruction alias, if one exists.
    };   

    // write accessors for InstrArgLogger
    void writeFieldAccessor(ostream &o,const UsedReg &ur,const Instr *instr,const Instr::Alias *alias)
    {
      ArgSubstWalker w(_o,*this,instr,alias);
      o << w.Translate(ur._begin);
    }

    // Write out a list of field accessors.
    void writeFieldAccessors(ostream &o,const Instr &instr,const gc_string &arg,bool &first, char* postfix = NULL)
    {
      int argCount = 0;
      int numfields = instr.sizeFields();
      // Grab all instruction field values.
      for (int i = 0; i != numfields; ++i) {
        Instr::Field fp = instr.getField(i);
        const InstrField &field = *(fp._field);
        if (skip_field(fp)) { // variable fields use for opcode
          continue;
        }
        list_sep(o,first);
        unsigned num_words = _core.instr_width()/32;
        gc_string args = gen_field_arg(arg,num_words);
        adl::writeFieldAccessor(o,field,args,(_core.is_little_endian()? instr.width():0),false);
        argCount++;	
      }

      if (postfix) {
        if (argCount) o << ", ";
        o << postfix ;
      }

    }

    // Temporall solution for transactional iss. Write out a list of field accessors.
    void writeFieldAccessorsTrans(ostream  &o,const Instr &instr,const gc_string &arg,bool &first, char* postfix = NULL)
    {
      int argCount = 0;
      int numfields = instr.sizeFields();
      // Grab all instruction field values.
      for (int i = 0; i != numfields; ++i) {
        Instr::Field fp = instr.getField(i);
        const InstrField &field = *(fp._field);
        if (skip_field(fp)) {  // variable fields use for opcode
          continue;
        }
        list_sep(o,first);
        adl::writeFieldAccessor(o,field,arg,(_core.is_little_endian()? instr.width():0),false);
        argCount++;	
      }

      if (postfix) {
        if (argCount) o << ", ";
        o << postfix ;
      }

    }

    gc_string gen_field_arg(const gc_string& arg, unsigned num) const 
    {
      gc_string ret("");
      bool first = true;
      ForRange(num,i) {
        if(!first) {
          ret += ",";
        }	  
        ret += arg + "[" + uitogs(i) + "]";	
        first = false;
      }
      return ret;    
    }

    void writeTimeTagInstrLogic(ostream& o,const Instr &instr,const char *coreref,const char *pfx = "")
    {
      if (time_tagged()) {
        if (auto grp = _coreconfig.get_group(instr)) {
          _o << pfx << "time_tag_start_" << grp->name() << "();\n";
        } else {
          RError("Instruction '" << instr.name() << "' not covered by a group when creating a time-tagged ISS.");
        }
      }
      else if (log_td_commit()) {
        // Not time-tagged, but we do have extra latency and we're logging commit
        // times, so add it here.
        if (int extra_latency = get_extra_td_latency(instr)) {
          o << pfx << "_td_counter += " << extra_latency << ";\n";
        }
      }
    }

    // Writes out various checks that are needed at the bottom of the simulation loop.
    void writeBottomChecks()
    {

    }

    // Fetch the prefix information specific to this instruction
    void writeInstrPrefixGetters(ostream& o,const Instr &instr)
    {
      static gc_string empty_gc_string("");
      if (!instr.get_type() || instr.get_blk() == empty_gc_string) {
        return;
      }

      ForEach(instr.get_type()->getFields(),i) {
        if (!i->_field) {
          continue;
        }
        // add indexed bits which are related to a block
        const InstrField &f = *(i->_field->interface() ? i->_field->interface() : i->_field);
        if (f.is_pseudo() && !f.blks().empty() && f.index()) {
          o << _iname << "[0] |= (";
          gc_string blk_position = getPrefixIndex(f,instr,"_packet_position");
          getPrefixField(o,f,blk_position,instr.get_blk());
          unsigned shift = _core.is_little_endian() ? 32 - (instr.width() - i->_field->lowbit()): 31 - i->_field->highbit();
          o << " << " << dec << shift << ");\n";
	    
        }
      }
    }

    void writePostFetchHandler(ostream &o,const gc_string &pfx,const gc_string &name,
                               Instr::PostFetchEnum enable_post_fetch,unsigned num_bytes,
                               bool nia_only)
    {
      if (enable_post_fetch != Instr::PostFetchFalse) {
        // If we have a predicate, guard the logic using the predicate.
        if (enable_post_fetch == Instr::PostFetchFunc) {
          o << "if (" << getPostFetchEnableName(name) << "()) {\n";
        }
        // Only generate this code if the instruction allows it.
        if (_core.post_fetch_handler()) {
          // We have a post-fetch handler, so we call it.
          o << pfx << "post_fetch_handler(" << dec << num_bytes << ");\n";
        } else if (nia_only) {
          // If we don't want to use the CIA, then we can update just the NIA.
          o << pfx << _nia->name() << "_write" << getNiaSfx() << "(" << pfx << _nia->name() << "_read() + " << dec << num_bytes << ");\n";
        } else {
          if (!_instr_table_embedded) {  //
            // Default post-fetch handler:  NIA is CIA + instruction-size in bytes. 
            o << pfx << _nia->name() << "_write" << getNiaSfx() << "(" << pfx << _cia->name() << "_read() + " << dec << num_bytes << ");\n";
          } else { o << "// No PC increment for micro instructions\n"; }
        }
        if (enable_post_fetch == Instr::PostFetchFunc) {
          o << "}\n";
        }
      }
    }

    // Writes code to be executed within the decode tree for normal simulation.
    void writeInstrDecode(ostream &o,const Instr* instr) 
    {
      InstrSym &sym = _instr_syms[instr->name()];
      o << "// Instruction:  \"" << instr->name() << "\"\n";

      if (low_mem() && !instr->is_prefix()) {
        o << sym._leafwrap << "();\n";
      } else {
        writeLeafCode(o,instr,sym);
      }

      if (sym.is_branch() && _config.trace_mode()) {
        o << "if (Tracing && (_trace_mode && ttBranchTaken)) {\n"
          << "_logger->log_branch_end(" << _cia->name() << "_read().uint64());\n"
          << "}\n";
      }

      if (sym.is_branch() || sym.halts() || explicit_bb_end(*instr)) {
        o << "rc = esEndBasicBlock;\n";
      }

      if (_core.parallel_execution() && !instr->is_prefix()) { 
        if (!_instr_table_embedded) {  // No packet instruction counter increment for micro instructions
          o << "++packet_instr_ctr;\n";
          o << "_empty_commit_queue = true; // Set to true for all non-micro instructions \n";      // Empty commit queue for all instructions by default
        } 
        else { o << "// No packet instruction counter increment for micro instructions \n"; 
          o << "_empty_commit_queue = false; // Reset to false for micro instructions \n"; // For micro instructions, set _empty_commit_queue inside the instruction handler of the macro instruction
        }
      }
    }

    gc_string getInstrFlags(const InstrSym &sym)
    {
      bool first = true;
      gc_string f;
      switch (sym._brType) {
      case InstrSym::brNone:
        break;
      case InstrSym::brAlways:
      case InstrSym::brCalc:
        pfx_sep(f,"|",first);
        f += "ifBrUncond";
        break;
      case InstrSym::brCond:
        pfx_sep(f,"|",first);
        f += "ifBrCond";
        break;
      }
      if (sym._halts) {
        pfx_sep(f,"|",first);
        f += "ifHalts";
      }
      
      return (f.empty()) ? "0" : f;
    }

    void writeLeafCode(ostream& o,const Instr* instr, const InstrSym& sym) 
    {
      writeInstrAttrs(o,instr,true);
      if (!instr->is_prefix() && _core.prefix_bits()) {
        writeInstrPrefixGetters(o,*instr);
      }

      if (_config.trace_mode()) {
        o << "if (Tracing) {\n"
          << "if (_trace_mode & ttInstr) {\n"
          << "_logger->log_instr(" << _iname << "," << dec << instr->width()/4 << ",\"" << instr->name() << "\"," << sym._disasm 
          << "," << getInstrFlags(sym) << ");\n"
          << "}\n";
        if (basic_block_end(*instr,sym)) {
          o << "if (_trace_mode & ttBasicBlockEnd) {\n"
            << "_logger->log_basic_block_end(" << _cia->name() << "_read().uint64());\n"
            << "}\n";
        }
        if (_config.log_instr_args_mode() && !instr->is_prefix()) {
          o << "if (_trace_mode & ttInstrArgs) {\n"
            << "_logger->log_instr_args(" << _iname << "," << dec << instr->width()/4 << ",\"" << instr->name() << "\"," << sym._logarg
            << ");\n"
            << "}\n";
        }
        o << "}\n";
      }

      unsigned num_bytes = instr->fetch_width()/8; 

      if (!_instr_table_embedded) {  // No packet position increment for micro instructions
        packetPosIncr(o,_core,"_packet_position",*instr); 
      } else { o << "// No packet position increment for micro instructions\n"; }

      if (_walker.curr_instr_accessed()) {
        o << "_curr_instr_bytes = " << num_bytes << ";\n";
      }

      writePostFetchHandler(o,"",instr->name(),instr->enable_post_fetch(),instr->fetch_width()/8,false);

      writeTimeTagInstrLogic(o,*instr,"*this");
   

      o << sym.exec_func() << "(";
      bool first = true;
      writeFieldAccessors(o,*instr,_iname,first);
      o << ");\n";

      writeTimeTagFinishCall(*instr,"");
    }

    void writeInstrDecodeMiss(ostream &o)
    {
      o << "// Decode miss.\n";

      if (low_mem() && !_core.decode_retry_handler()._func) {
        o << getDecodeMissLeafName() << "();\n";
      } else {
        writeDecodeMissLeafCode(o);
      }
      o << "return esEndBasicBlock;\n";
    }

    // On a decode miss, for simulation, we call the decode-miss handler function.
    void writeDecodeMissLeafCode(ostream &o)
    {
      writeInstrAttrs(o,0,false);
      o << "if (Tracing && (_trace_mode & ttInstr)) {\n"
        << "_logger->log_instr(" << _iname << "," << dec << _core.instr_width()/4 <<",\"<illop>\",disasm_illop,0);\n"
        << "}\n";
      if (_core.decode_retry_handler()._func) {
        o << "if (decode_retry_handler(" << _cia->name() << "_read().uint64()," << _iname << "[0])) {\n"
          << "return esTryAgain;\n"
          << "} else {\n"
          << "decode_miss_handler(" << _cia->name() << "_read().uint64()," << _iname << "[0]);\n"
          << "}\n";
      } else {
        o << "decode_miss_handler(" << _cia->name() << "_read().uint64()," << _iname << "[0]);\n";
      }
    }



    // Allows to prefetch and access next instruction
    void writePrefetchBufferAccessors()
    {
      if (_core.pre_fetch_handler()) {
        funcBody(_core.env(),_core.pre_fetch_handler()._func);
      }
      bool has_buffer = _core.has_prefetch_buffer();
      if (!has_buffer) {
        return;
      }
      
      gc_string PrefetchMemoryName;
      unsigned buffer_size = 0;
      ForEach(_core.memories(),m) {
        if (m->second->prefetch_mem()) {
          PrefetchMemoryName = m->first;
          // The memory implementation have one additional word for handling cyclic
          // accesses. 
          buffer_size = m->second->size(); 
        }
      }
      _cd._member_vars.push_back("uint64_t _prefetch_start;");
      _cd._constr_inits.push_back("_prefetch_start(0)");
      _cd._reset_stmts.push_back("_prefetch_start = 0;");
      _cd._member_vars.push_back("int _prefetch_idx;");
      _cd._constr_inits.push_back("_prefetch_idx(-1)");
      _cd._reset_stmts.push_back("_prefetch_idx = -1;");
      _o << "inline void read_prefetch_buffer(uint64_t addr) {\n"
         << "uint64_t to_read = addr - _prefetch_start;\n" 
         << "if (addr < _prefetch_start || _prefetch_idx == -1 || to_read > " << buffer_size << ") {\n"
         << "_prefetch_idx = 0;\n"
         << "to_read = " << buffer_size << ";\n"
         << "} else {\n"
         << "to_read = (addr - _prefetch_start) & 0xFFFFFFFC;\n"
         << "if (to_read) {\n"
         << "_prefetch_idx = (_prefetch_idx + to_read)%" << buffer_size << ";\n" 
         << "} else {\n"
         << "return;"
         << "}\n"
         << "}\n"
         << "_prefetch_start = addr & 0xFFFFFFFC;\n"
         << "addr_t instr_addr = _prefetch_start + " << buffer_size << "- to_read;\n"
         << "addr_t offset = (_prefetch_idx + " << buffer_size << " - to_read)%" << buffer_size << ";\n"
         << "for( ; to_read > 0; to_read-=4, offset = (offset+4)%"<< buffer_size << ") {\n"
         << PrefetchMemoryName << "_write<4>(offset,bits<32>(instr_read32(instr_addr)));\n"
         << "instr_addr += 4;\n"
         << "}\n"  
         << PrefetchMemoryName << ".write32(" << buffer_size << "," << PrefetchMemoryName << ".read32(0));\n" 
         << "}\n\n";
    }

    // Write the various functions used for modifying the active count.
    void writeActiveCountFuncs()
    {
      gc_string ac = getActiveCallback();

      _o << "unsigned activeCount() const { return _active; };\n\n"
         << "virtual bool is_active() const { return activeCount(); };\n\n"
         << "virtual unsigned get_active_count() const { return activeCount(); };\n\n"
         << "virtual unsigned set_active_count(unsigned x)\n"
         << "{\n"
         << "if (" << ac << ") {\n"
         << "return " << ac << "->set_active_count(this,_active,x);\n"
         << "} else {\n"
         << "unsigned o = _active;\n"
         << "_active = x;\n"
         << "return o;\n"
         << "}\n"
         << "};\n\n";

      _cd._member_vars.push_back("ActiveCallback *" + ac + ";");
      _cd._constr_inits.push_back(ac + "(0)");

      _o << "void setActiveCallback(ActiveCallback *ac) { _active_callback = ac; };\n\n";
    }

    // Allows acess to last instruction decoded
    void writeLastInstrAccessor() 
    {
      gc_string lastInstr,resetLastInstr;
      MkGStr(lastInstr,"mutable uint32_t " << _iname << "[" << (_core.instr_width()-1)/32 + 1 << "]"); //
      MkGStr(resetLastInstr,"for(int i=0;i<"<<(_core.instr_width()-1)/32+1<<";++i){"<< _iname << "[i]=0;}"); //
      _cd._member_vars.push_back(lastInstr);
      _cd._reset_stmts.push_back(resetLastInstr);
      if (_walker.curr_instr_accessed()) {
        _cd._member_vars.push_back("int _curr_instr_bytes");
        _cd._constr_inits.push_back("_curr_instr_bytes(0)");
        _cd._reset_stmts.push_back("_curr_instr_bytes = 0");
        _o << "uint32_t *getCurrentInstr(int &num_bytes)\n{\n"
           << "num_bytes = _curr_instr_bytes;\n" 
           << "return (uint32_t *) &" << _iname  << "[0];\n"
           << "}\n\n";
      }
    }

    gc_string getOpTypeEnum(const UsedReg &ur,const SlotInfo &si)
    {
      bool first = true;
      gc_string r = "(";
      if (si._trg) {
        pfx_sep(r," | ",first);
        r += "OpInfo::opTrg";
      }
      if (si._src) {
        pfx_sep(r," | ",first);
        r += "OpInfo::opSrc";
      }
      if (ur.partial()) {
        pfx_sep(r," | ",first);
        r += "OpInfo::opPartial";
      }
      if (ur.conditional()) {
        pfx_sep(r," | ",first);
        r += "OpInfo::opCond";
      }
      r += ")";
      return r;
    }

    void writeSlotReset(ostream &o,const UsedReg &ur,const SlotInfo &si,const gc_string &index)
    {
      if (si._trg) {
        if (ur.partial()) {
          o << "x->clear(" << index << ");\n";
        } else {
          o << "x->clear_mask(" << index << ");\n";
        }
        if (ur.delayed()) {
          o << "x->clear_delay(" << index << ");\n";
        }
      } else if (si._src) {
        if (track_reads()) {
          o << "x->clear_mask(" << index << ");\n";
        }
      }
    }

    void writeOpEntry(ostream &o,const UsedReg &ur,const SlotInfo &si,const Instr *instr,IntSet &used_indices)
    {
      gc_string type = getOpTypeEnum(ur,si);

      if (const Reg *reg = ur.reg()) {
        o << "x->_ops[" << si._start << "]._id = " << reg->id() << ";\n"
          << "x->_ops[" << si._start << "]._index = 0;\n"
          << "x->_ops[" << si._start << "]._type = " << type << ";\n";

        // If this slot refers to the current instruction address, then store
        // this value now.
        if (!hybrid_iss() && reg == _core.cia() && si._src) {
          o << "x->set_slot(" << si._start << "," << reg->name() << "_read());\n";
        }

        writeSlotReset(o,ur,si,uitogs(si._start));
        used_indices.insert(si._start);
        if (reg->branch_target(_core)) {
          o << "x->_branch_target = " << si._start << ";\n";
        }
      } else if (const RegFile *rf = ur.regfile()) {
        gc_ostringstream os;
        bool     first = true;
        ForRange(_core.instr_width()/32,k) {
          list_sep(os,first);
          os << _iname << "[" << k << "]";
        }
        gc_string opc = os.str();

        auto alias = (instr) ? instr->get_alias() : 0;
        if (ur.is_fixed()) {
          o << "x->_ops[" << si._start << "]._id = " << rf->id() << ";\n";
          o << "x->_ops[" << si._start << "]._index = ";
          adl::writeFieldAccessor(o,_core,ur._begin,instr,alias,opc);
          o << ";\n";
          o << "x->_ops[" << si._start << "]._type = " << type << ";\n";

          writeSlotReset(o,ur,si,uitogs(si._start));
          used_indices.insert(si._start);
        } else if (ur.is_range()) {
          // For a range, we don't really know what elements will be accesssed.
          // So, we have to read the whole range.
          o << "{\n"
            << "unsigned c = 0, i = " << si._start << ";\n"
            << "unsigned start = ";
          adl::writeFieldAccessor(o,_core,ur._begin,instr,alias,opc);
          o << ";\n"
            << "unsigned stop = ";
          adl::writeFieldAccessor(o,_core,ur._end,instr,alias,opc);
          o << ";\n";

          o << "for ( ; c != start; ++i, ++c) {\n"
            << "x->_ops[i]._id = " << rf->id() << ";\n"
            << "x->_ops[i]._index = c;\n"
            << "x->_ops[i]._type = OpInfo::opNone;\n";
          writeSlotReset(o,ur,si,"i");
          o << "}\n"
            << "for ( ; c <= stop; ++i, ++c) {\n"
            << "x->_ops[i]._id = " << rf->id() << ";\n"
            << "x->_ops[i]._index = c;\n"
            << "x->_ops[i]._type = " << type << ";\n";
          writeSlotReset(o,ur,si,"i");
          o << "}\n"
            << "for ( ; i != " << si._stop << "; ++i, ++c) {\n"
            << "x->_ops[i]._id = " << rf->id() << ";\n"
            << "x->_ops[i]._index = c;\n"
            << "x->_ops[i]._type = OpInfo::opNone;\n";
          writeSlotReset(o,ur,si,"i");
          o << "}\n"
            << "}\n";
          for (int i = si._start; i != si._stop; ++i) {
            used_indices.insert(i);
          }
        } else {
          // We don't have a fixed element that's accessing this register
          // file, so we have to store information about all elements.
          o << "{\n"
            << "unsigned c = 0;\n"
            << "for (unsigned i = " << si._start << "; i != " << si._stop << "; ++i, ++c) {\n"
            << "x->_ops[i]._id = " << rf->id() << ";\n"
            << "x->_ops[i]._index = c;\n"
            << "x->_ops[i]._type = " << type << ";\n";
          writeSlotReset(o,ur,si,"i");
          o << "}\n"
            << "}\n";
          for (int i = si._start; i != si._stop; ++i) {
            used_indices.insert(i);
          }
        }
      } else {
        RError("Unknown used-reg type.");
      }

    }

    void writeUnusedClearStmts(ostream &o,const IntSet &used_indices,unsigned num_slots)
    {
      ForRange(num_slots,i) {
        if (!used_indices.count(i)) {
          o << "x->_ops[" << i << "]._type = OpInfo::opNone;\n";
        }
      }
    }

    void writeOpEntries(ostream &o,const IssUsedRegs &items,const Instr *instr,int num_slots)
    {
      IntSet used_indices;
      ForEach(items,iter) {
        const UsedReg  &ur = iter->first;
        const SlotInfo &si = iter->second;

        writeOpEntry(o,ur,si,instr,used_indices);

      }

      writeUnusedClearStmts(o,used_indices,num_slots);
    }

    gc_string getTransLeafArgs()
    {
      gc_string ua = "instr,";
      if (logging_mem()) {
        ua += "speculative,";
      }
    
      return ua;
    }

    void writeTransInstrDecode(ostream &o,const Instr *instr) {
      InstrSym &sym = _instr_syms[instr->name()];

      if (low_mem() && !instr->is_prefix()) {
        gc_string ua = getTransLeafArgs();
        o << "// Instruction:  \"" << instr->name() << "\"\n";
        o << sym._leafwrap << "(instr_addr," << ua << "size" << decode_arg(true) << ");\n";
      } else {
        writeTransLeafCode(o,instr,sym,cFull);
      }
    }
    
    // For the transactional ISS, we only decode once.  After that, we utilize a single switch statement
    // to perform any other conditional code.
    void writeTransLeafCode(ostream &o,const Instr *instr,const InstrSym &sym,CodeType ctype)  
    {
      const InstrSlotAlloc &isa = _genip.instr_slots()->find(instr->name())->second;

      gc_string coretype = getCoreTypeName(_core);

      o << "// Instruction:  \"" << instr->name() << "\"\n";

      if (_config.instr_size_check()) {
        // Unless turned off, 
        // Is the size supplied sufficient for this instruction?  This checks
        // the possibility that we can successfully decode into a smaller
        // instruction, but that we shouldn't.  For example, a value of
        // 0xdead0000 decodes to instruction 'foo' which is 4 bytes long, but we
        // were only given 2 bytes, so this can't really be 'foo'.  Thus, we
        // return a null pointer to indicate a decode problem- the user must
        // supply more data to succesfully decode.
        o << "if (size < " << dec << instr->fetch_width()/8 << ") {\n";
        if (hybrid_iss() && has_decode_arg()) {
          o << decode_arg(false) << ".decode_update(false,0);\n";
        }
        o << "return ";
        if (!low_mem())
          o << " 0";
        o << ";\n"
          << "}\n\n";
      }

      if (!instr->is_prefix() && _core.prefix_bits()) {
        writeInstrPrefixGetters(o,*instr);
      }

      if (instr) {  
        if (!_instr_table_embedded) {  // No packet position increment for micro instructions
          packetPosIncr(o,_core,"_packet_position",*instr);
        } else { o << "// No packet position increment for micro instructions\n"; }
      }

      o << PktType << " *x = _packets.alloc();\n"
        << "x->reset(" << dec << instr->id() << "," << instr->width()/8 << "," << _cia->name() << "_read().uint64()," 
        << _iname << "," << isa._num_slots << "," << sym._disasm;

      o << ");\n";

      writeInstrAttrs(o,instr,true,"x->");

      if (instr->is_prefix() && !hybrid_iss()) {
        o << coretype << "::" << isa._ex_name << "(" << _iname  << ");\n";
      }

      if (!hybrid_iss()) {
        o << "x->_read_operand  = &" << coretype << "::" << isa._ro_name << ";\n"
          << "x->_exec          = &" << coretype << "::" << isa._ex_name << ";\n";
        if (uses_coro_commit(isa)) {
          o << "x->_commit        = &" << coretype << "::" << isa._cd_name << ";\n";
        } else {
          o << "x->_commit        = &" << coretype << "::" << isa._cm_name << ";\n";
        }
      }
      if (cached_packets()) {
        o << "x->_quick_reset   = &" << coretype << "::" << isa._qr_name << ";\n";
      }
      o << "x->_branch_info   = &" << coretype << "::";
      if (isa._bi_name) {
        o << isa._bi_name;
      } else {
        o << "default_branch_info";
      }
      o << ";\n";
      if (_core.has_delayed_writes()) {
        o << "x->_delay_info    = &" << coretype << "::";
        if (isa.has_delays()) {
          o << isa._di_name;
        } else {
          o << "default_delay_info";
        }
      }
      o << ";\n";

      if (instr_attrs()) {
        o << "x->_attr_info  = &" << coretype << "::" << isa._at_name << ";\n"
          << "x->_field_info = &" << coretype << "::" << isa._fi_name << ";\n";
      }

      bool c_e = uses_coro_exec(isa);
      bool c_c = uses_coro_commit(isa);
      if (c_e) {
        // If we access memory, then this is going to be treated as a coroutine.
        o << "x->_co.reset(" << isa._co_name << ",this,x,_main_context,(x->_stack = _coro_stacks.alloc())->data(),StackSize);\n";
      }
      if (c_c) {
        o << "x->_cco.reset(" << isa._cc_name << ",this,x,_main_context,(x->_cstack = _coro_stacks.alloc())->data(),StackSize);\n";
        _has_commit_stack = true;
      }

      writeOpEntries(o,isa._items,instr,isa._num_slots);

      if (postFetchSkip()) {
        o << "if (_skip_instruction_commit) {\n"
          << "discard_packet(x);\n"
          << "x = 0;\n"
          << "}\n";
      }

      o << "instr = " << decode_return(instr->name(),"x") << ";\n";

      if (hybrid_iss()) {
        if (has_decode_arg()) {
          o << decode_arg(false) << ".decode_update(true,instr->size());\n";
        }
        o << "if (!speculative) {\n";
      }

      // We do the prefetch logging here for normal-model models b/c we want to
      // make sure that these are grouped properly.  In a normal-mode uADL model,
      // fetching is actually very disconnected from decode, so this is a bit of a
      // hack to get a DAT to look the same between both types of model.  For
      // safe-mode models, things work more like a normal ISS.
      if (_config.trace_mode()) {
        o << "if (Tracing && (_trace_mode & ttInstrPrefetch)) {\n"
          << "_logger->log_instr_prefetch(" << _cia->name() << "_read().uint64());\n"
          << "}\n";

        if (basic_block_end(*instr,sym)) {
          o << "if (Tracing && (_trace_mode & ttBasicBlockEnd)) {\n"
            << "_logger->log_basic_block_end(" << _cia->name() << "_read().uint64());\n"
            << "}\n";
        }

        o << "if (Tracing && (_trace_mode & ttInstr)) {\n"
          << "_logger->log_current_instr(x);\n"
          << "_logger->log_instr(&" << _iname << "[0]," << dec
          << instr->width()/4 << ",\"" << instr->name() << "\"," << sym._disasm << "," << getInstrFlags(sym) << ");\n"
          << "}\n";
      }

      if (_core.post_fetch_handler()) {
        // We have a post-fetch handler, so we call it.
        o << "post_fetch_handler(" << dec << instr->fetch_width()/8 << ");\n";
      } else {
        if (!_instr_table_embedded) {  //
          // Default post-fetch handler:  NIA is CIA + instruction-size in bytes.
          o << _nia->name() << "_write" << getNiaSfx() << "(instr_addr + " << dec << instr->fetch_width()/8 << ");\n";
        } else { o << "// No PC increment for micro instructions\n"; }    
      }     

      if (hybrid_iss()) {
        o << PktName << " = x;\n";
        if (has_decode_arg()) {
          o << decode_arg(false) << ".set_cur_instr(instr);\n";
        }
        if (need_read_op(*instr)) {
          o << isa._ro_name << "(" << _iname << ");\n";
        }
        o << sym.exec_func() << "(";
        bool first = true;
        writeFieldAccessors(o,*instr,_iname,first);
        o << ");\n";
        if (!instr->is_prefix() && _core.post_exec_handler()) {
          o << "post_exec_handler();\n";
        }
      
        if (sym.is_branch() && _config.trace_mode()) {
          o << "if (Tracing && (_trace_mode && ttBranchTaken)) {\n"
            << "_logger->log_branch_end(" << _cia->name() << "_read().uint64());\n"
            << "}\n";
        }

        // End of if (!speculative) block.
        o << "}\n";
      }

      o << "return ";
      if (!low_mem())
        o << " instr";
      o << ";\n";
    }

    void writeTransDecodeMiss(ostream &o)
    {
      // We can't put this in a separate function if we have the retry handler,
      // since it rlies upon a goto.
      if (low_mem() && !_core.decode_retry_handler()._func) {
        gc_string ua = getTransLeafArgs();
        o << "// Decode-miss.\n";
        o << getDecodeMissLeafName() << "(instr_addr," << ua << "size" << decode_arg(true) << ");\n";
      } else {
        writeTransDecodeMissLeafCode(o,cFull);
      }
    }

    // On a miss we specify a special label that will generate a decode miss.
    void writeTransDecodeMissLeafCode(ostream &o,CodeType ctype)
    {
      if (_core.decode_retry_handler()._func) {
        o << "if (decode_retry_handler(" << _cia->name() << "_read().uint64()," << _iname << "[0])) {\n"
          << "goto retry_decode_target;\n"
          << "}\n";
      }

      // If we have less data than the largest instruction, then it's possible
      // that this isn't really an illegal instruction, but rather just a portion
      // of a valid instruction.  For this reason, return a null if we decode an
      // illegal but it's smaller than the maximum instruction size for this core.
      if (_config.instr_size_check()) {
        o << "if (size < " << dec << _core.instr_width()/8 << ") {\n";
        if (hybrid_iss() && has_decode_arg()) {
          o << decode_arg(false) << ".decode_update(false,0);\n";
        }
        o << "return ";
        if (!low_mem())
          o << " 0";
        o << ";\n"
          << "}\n\n";
      }

      const InstrSlotAlloc &isa = _genip.decode_miss_slots();
      gc_string coretype = getCoreTypeName(_core);

      o << PktType << " *x = _packets.alloc();\n";

      writeInstrAttrs(o,0,true,"x->");

      // We'll just assume the max size.
      o << "x->reset(-1," << _core.instr_width()/8 << "," << _cia->name() << "_read().uint64(),&" << _iname << "[0]," 
        << isa._num_slots << ",disasm_illop"
        << ");\n";
      if (!hybrid_iss()) {
        o << "x->_read_operand  = &" << coretype << "::" << isa._ro_name << ";\n"
          << "x->_commit        = &" << coretype << "::" << isa._cm_name << ";\n"
          << "x->_exec          = &" << coretype << "::" << isa._ex_name << ";\n";
      }
      if (cached_packets()) {
        o << "x->_quick_reset   = &" << coretype << "::" << isa._qr_name << ";\n";
      }
      o << "x->_branch_info   = &" << coretype << "::default_branch_info;\n";
      if (_core.has_delayed_writes()) {
        o << "x->_delay_info   = &" << coretype << "::default_delay_info;\n";
      }

      writeOpEntries(o,isa._items,0,isa._num_slots);

      o << "instr = " << decode_return("decode_miss","x") << ";\n";

      if (hybrid_iss()) {
        if (has_decode_arg()) {
          o << decode_arg(false) << ".decode_update(true,instr->size());\n";
        }
        o << "if (!speculative) {\n";
      }

      if (_config.trace_mode()) {
        o << "if (Tracing && (_trace_mode & ttInstrPrefetch)) {\n"
          << "_logger->log_instr_prefetch(" << _cia->name() << "_read().uint64());\n"
          << "}\n"
          << "if (Tracing && (_trace_mode & ttInstr)) {\n"
          << "_logger->log_current_instr(x);\n"
          << "_logger->log_instr(&" << _iname << "[0]," << dec
          << _core.instr_width()/4 << ",\"<illop>\",disasm_illop,0);\n"
          << "}\n";
      }

      if (hybrid_iss()) {
        o << PktName << " = x;\n";
        if (has_decode_arg()) {
          o << decode_arg(false) << ".set_cur_instr(instr);\n";
        }
        o << "decode_miss_handler(" << _cia->name() << "_read().uint64()," << _iname << "[0]);\n"
          // End of if (!speculative) block.
          << "}\n";
      }

      o << "return ";
      if (!low_mem())
        o << " instr";
      o << ";\n";
    }

    // Writes out the tree used to translate target instructions into host code.
    void writeJitDecode(ostream &o,const Instr* instr) 
    {
      InstrSym &sym = _instr_syms[instr->name()];
      o << "// Instruction:  \"" << instr->name() << "\"\n";

      unsigned num_bytes = instr->fetch_width()/8; 

      writeICacheCrossingCheck(o,JitTrans,num_bytes);

      o << sym._jit_trans << "(" << ((multi_compile()) ? "*this," : "") << "jb,pc,count";
      bool first = false;
      writeFieldAccessors(_o,*instr,_iname,first);
      o << ");\n"
        << "pc += " << num_bytes << ";\n";

      // We consider branches and halt instructions to be the end of basic blocks,
      // so we tell the loop to stop translating.
      if (sym.is_branch() || sym.halts() || (instr && explicit_bb_end(*instr))) {
        o << "return false;\n";
      }
    }

    // On a decode miss, for JIT translation, we insert a decode-miss handler.
    void writeJitDecodeMiss(ostream &o)
    {
      gc_string add_entry;

      o << _instr_syms._decode_miss._jit_trans << "(pc," << _iname << "[0]);\n"
        // I'm just bumping the PC by the largest instruction size as a best
        // guess of what to do.
        << "pc += " << dec << _max_instr_width/8 << ";\n"
        // We'll assume that a decode miss signifies an end of a basic block.
        << "return false;\n";
    }

    // Writes out the tree used to translate target instructions into decode-cache items.
    void writeDcDecode(ostream &o,const Instr* instr) 
    {
      InstrSym &sym = _instr_syms[instr->name()];
      o << "// Instruction:  \"" << instr->name() << "\"\n";

      unsigned num_bytes = instr->fetch_width()/8; 

      writeICacheCrossingCheck(o,DcTrans,num_bytes);

      o << sym._dc_trans << "(" << ((multi_compile()) ? "*this," : "") << "bb,pc,count";
      bool first = false;
      writeFieldAccessors(_o,*instr,_iname,first);
      o << ");\n"
        << "pc += " << num_bytes << ";\n";

      // We consider branches and halt instructions to be the end of basic blocks,
      // so we tell the loop to stop translating.
      if (sym.is_branch() || sym.halts()) {
        o << "return false;\n";
      }
    }

    // On a decode miss, for decode-cache translation, we insert a decode-miss handler.
    void writeDcDecodeMiss(ostream &o)
    {
      gc_string add_entry;

      o << _instr_syms._decode_miss._dc_trans << "(pc," << _iname << "[0]);\n"
        // I'm just bumping the PC by the largest instruction size as a best
        // guess of what to do.
        << "pc += " << dec << _max_instr_width/8 << ";\n"
        // We'll assume that a decode miss signifies an end of a basic block.
        << "return false;\n";
    }

    gc_string get_instr_read()
    {
      gc_string instr_read;
      if (_core.has_prefetch_buffer()) {
        instr_read = "prefetch_instr_read32";
      } else if (logging_mem()) {
        instr_read = "get_next_instr_word";
      } else {
        instr_read = "instr_read32";
      }
      return instr_read;
    }

    // Interface structure used by the decode generator.
    struct CodeGenIF {
      CodeGenIF(ostream &o,GenInstrs &gi) : _o(o), _gi(gi) {};
      virtual ~CodeGenIF() {};

      // Return the name of the symbol used for storing fetch information.
      virtual const gc_string &fetch_name() const { return _gi.fetch_name(); };
      // Return the function name to fetch the next instruction word.
      virtual gc_string get_instr_read() { return _gi.get_instr_read(); };
      // Called to write the fetch address expression.
      virtual gc_string get_fetch_addr() { return (_gi.transactional_iss()) ? "addr" : _gi._cia->name() + "_read().uint64()"; };
      // Any extra fetch parms.
      virtual gc_string extra_fetch_parms() { return ""; };
      // Called to write the table selection expression.
      virtual void writeTableSelect() { _o << getInstrTableArg(); };
      // Called for a valid leafnode.
      virtual void writeEntry(const Instr *) = 0;
      // Called for handling a default case, e.g. a decode miss. 
      virtual void writeDefault() = 0;

      ostream   &_o;
      GenInstrs &_gi;
    };

    // Decode code generator for normal simulation.
    struct SimCodeGen : public CodeGenIF {
      SimCodeGen(ostream &o,GenInstrs &gi) : CodeGenIF(o,gi) {};
    
      void writeEntry(const Instr *instr) { _gi.writeInstrDecode(_o,instr); };
      void writeDefault() { _gi.writeInstrDecodeMiss(_o); };
    };

    // Decode code generator for a transactional ISS:  This just allocates a packet
    struct TransCodeGen : public CodeGenIF {
      TransCodeGen(ostream &o,GenInstrs &gi) : CodeGenIF(o,gi) {};

      void writeEntry(const Instr *instr) { _gi.writeTransInstrDecode(_o,instr); };
      void writeDefault() { _gi.writeTransDecodeMiss(_o); };
    };

    // Decode code generator for a JIT.  This version is used to create the translation decode tree.
    struct JitCodeGen : public CodeGenIF {
      JitCodeGen(ostream &o,GenInstrs &gi) : CodeGenIF(o,gi) {};

      void writeEntry(const Instr *instr) { _gi.writeJitDecode(_o,instr); };
      void writeDefault() { _gi.writeJitDecodeMiss(_o); };
    };

    // Decode code generator for a decode cache.  This version is used to create the translation decode tree.
    struct DcCodeGen : public CodeGenIF {
      DcCodeGen(ostream &o,GenInstrs &gi) : CodeGenIF(o,gi) {};

      gc_string get_instr_read() { return "instr_read32_nocache"; };
      gc_string get_fetch_addr() { return "pc"; };
      void writeEntry(const Instr *instr) { _gi.writeDcDecode(_o,instr); };
      void writeDefault() { _gi.writeDcDecodeMiss(_o); };
    };

    void writeDecodeTable(ostream &o,const InstrTable &table, CodeGenIF &codegen, unsigned fetched,
                          bool prefix, bool extra_fetch)
    {
      if(!table.empty()) {
        // Not a leaf node:  Create a switch statement that operates
        // on the relevant portion of the current instruction.
        unsigned to_fetch = required_fetch(table.mask());

        write_additional_fetch(o,fetched,to_fetch,prefix,codegen,extra_fetch);
        fetched = to_fetch;

        uint32_t  msk = get_submask(table.mask());
        unsigned  ind = get_index(table.mask());
    
        o << "switch (" << codegen.fetch_name() << "[" << dec << ind << "] & 0x" << hex << msk << ") {\n";
        ForEach(table,i) {
          uint32_t val = (i->first >> (i->second->getWidth() - 32*(ind+1))).uint32(); //        
          o << "case 0x" << hex << val << ": {\n";
          writeDecodeTable(o,*(i->second), codegen, to_fetch,prefix, extra_fetch);
          o << "}\n"
            << "break;\n";
        }   
        if (table.default_table()) {
          // If we have a default table, then write it out.  This arises from
          // strange conflict situations, when a field is tagged as
          // allow_conflict=true.
          o << "default: {";
          writeDecodeTable(o,*(table.default_table()),codegen,to_fetch,prefix,extra_fetch);
          o << "}\n"
            << "break;\n"
            << "}\n" << dec;
        }
        else if (const Instr *instr = table.instr()) {    
          // This then catches the case where we have an instruction + a table.
          // This can occur when we specialize a subset of the instructions, then
          // have a generic version for handling the rest of the cases.
          o << "default:\n {";
          to_fetch = num_words(_core,instr->fetch_width());
          write_additional_fetch(o,fetched, to_fetch, prefix, codegen, extra_fetch);
          codegen.writeEntry(instr);
          o << "}\n"
            << "break;\n"
            << "}\n" << dec;
        } else {
          o << "default:\n";
          codegen.writeDefault();	
          o << "break;\n"
            << "}\n" << dec;
        }
      } else if (const Instr *instr = table.instr()) {    
        // Leaf node:  Create instruction handling code here.
        unsigned to_fetch = num_words(_core,instr->fetch_width());
        write_additional_fetch(o,fetched, to_fetch,prefix,codegen,extra_fetch);
      
        // The table is the leaf node, here set the flag to control the insertion of PC increment code
        if (table.is_embedded()) { _instr_table_embedded = true; } else { _instr_table_embedded = false; } 
      
        codegen.writeEntry(instr);
      
      }
    }

    // Writes all decode tables from a hash of instruction tables.
    void writeDecodeTables(const InstrTables &tables, CodeGenIF &codegen, bool extra_fetch)
    {
      ostream &o = codegen._o;
      if (_citname) {
        // We have multiple instruction tables:  Need outer switch to
        // select the table.
        o << "switch (";
        codegen.writeTableSelect();
        o << ") {\n";
        ForEach (tables,i) {
          auto itname = Data::get_attr_name(i->first);
          o << "case " << itname << ": {\n";
          bool prefix_tbl = (_core.prefix_bits() && i->second._table->is_prefix());
	  
          if (_core.prefix_bits()) {
            writePrefixArchFetchCode(o,_core,codegen.fetch_name(),_fname,prefix_tbl);
          }
          writeDecodeTable(o,*(i->second._table), codegen,1,prefix_tbl,extra_fetch);
          o << "} break;\n";
        }
        o << "}\n";
      } else {
        // No current-instruction-table identifier means that we just
        // have a single instruction table in this core.
        const InstrTable &table = *(tables.begin()->second._table);
        writeDecodeTable(o,table,codegen,1,false,extra_fetch);
      }
    }

    void writeCycleRatioCall(ostream &o,const gc_string &fn)
    {
      if (have_clock_cycle_ratio()) {
        gc_string cc = getCycleTally();
        o << "if (" << cc << " >= 1) {\n"
          << "unsigned n = (unsigned)" << cc << ";\n"
          << "for (unsigned i = 0; i != n; ++i) {\n"
          << fn << "();\n"
          << "}\n"
          << "}\n";
      } else {
        o << fn << "();\n";
      }
    }


    void genPostFetchHandler(opfxstream &o) 
    {
      if (postFetchSkip()) {
        _walker.set_skip_commit(true);
      }
      if (_core.post_fetch_handler()) {
        // Writes to the NIA in the post-fetch handler use the "simple" form of
        // NIA update, where writes do not log a branch-taken.
        _walker.set_nia_simple_mode(true);
        genFunc("void post_fetch_handler_internal",_core.env(),_core.post_fetch_handler()._func);
        _walker.set_nia_simple_mode(false);
        o << "void post_fetch_handler(unsigned instr_size)\n{\n";
        if (postFetchSkip()) {
          // If we're using the skip-commit methodology (as opposed to a goto)
          // then we must reset this flag, just in case.
          o << "_skip_instruction_commit = false;\n";
        }
        o << "post_fetch_handler_internal(instr_size);\n";
        o << "}\n\n";
      }
      _walker.set_skip_commit(false);
    }

    bool writePreFetchHandler(opfxstream &o)
    {
      // A transactional ISS does not execute the pre-cycle routine because that
      // will be executed by the performance model itself.
      if (!transactional_iss()) {
        if (has_cycle_handler()) {
          if (have_clock_cycle_ratio()) {
            // Insert code to update the tally here.
            o << getCycleTally() << " += " << clock_cycle_ratio() << ";\n";
          }
        }
        if (_core.pre_cycle_handler()) {
          writeCycleRatioCall(o,"pre_cycle_handler");
        }
      }

      // Insert any code to be run just before the next instruction is fetched.
      // We inline it so that a call to skip_instruction() can be converted into
      // a local goto.
      bool write_skip_instr = false;
      
      if (_core.pre_fetch_handler()) {

        Ptree *pf = _core.pre_fetch_handler()._func;
        o << "{\n";
        writeLineDirective(o,_core.env(),pf);
        o << funcBody(_core.env(),pf);
        writeCurLocation(o);
        o << "\n}\n";
        write_skip_instr = _walker.has_skip();
      }
         
      return write_skip_instr;
    }

    void writePreFetchCode(bool add_logging,bool add_cia_update)
    {
      writeInstrAttrClear(_o);

      if (add_cia_update && (_cia != _nia)) {
        gc_string sfx = "";
        if (log_reg_reads()) {
          sfx = "_notrace";
        }
     
        // CIA_commit_write() is used only when it is available 
        // CIA_commit_write() is needed to correctly set the CIA value for next instruction fetch
        if (_core.is_CIA_commit_write() && _core.parallel_execution() && !transactional_iss()) {
          _o << _cia->name() << "_commit_write(" << _nia->name() << "_read" << sfx << "());\n";  
        }
        else {
          _o << _cia->name() << "_write(" << _nia->name() << "_read" << sfx << "());\n";  
        }
      }

      if (add_logging) {
        _o << "if (Tracing && (_trace_mode & ttInstrPrefetch)) {\n"
           << "_logger->log_instr_prefetch(" << _cia->name() << "_read().uint64());\n"
           << "}\n";
      }

      if (_core.has_prefetch_buffer()) {
        _o << "read_prefetch_buffer(" << _cia->name() << "_read().uint64());\n";
      }

      if (_core.decode_retry_handler()._func && !transactional_iss()) {
        _o << "do {\n";
      }

    }

    void writePostFetchCode()
    {
      gc_string instr_read = (_core.has_prefetch_buffer() ? "prefetch_instr_read32" : "instr_read32");
      if (_core.prefix_bits()) {
        // Fetch data then adapt usage to table type (prefix/non-prefix)
        _o <<  _fname << " = " << instr_read << "(" << _cia->name() << "_read().uint64());\n";
      } else { 
        _o << _iname << "[0] = " << instr_read << "(" << _cia->name() << "_read().uint64());\n";
      }
    }

    // Writes basic fetch support code for simulation.
    bool writeFetchCode()
    {
      if (_core.prefix_bits()) {
        _o << "uint32_t " << _fname << ";\n";
      }

      _o << "++_instr_count;\n";

      writePreFetchCode(true,true);

      bool write_skip_instr = writePreFetchHandler(_o);

      writePostFetchCode();

      // If time-tagged and we have instruction caches, then store the latency.
      if (time_tagged() && _core.has_caches()) {
        _o << "accum_cache_latency();\n";
      }

      return write_skip_instr; 
    }

    // Writes heartbeat checking code.
    void writeHeartbeat()
    {
      _o << "if (Tracing && Heartbeat) {\n"
         << "if ((_instr_count % Heartbeat) == 0) {\n"
         << "std::cerr << name() << \":  ic=\" << instr_count() << '\\n';"
         << "}\n"
         << "}\n\n";
    }

    void writeBreakpointCheck(bool use_nia)
    {
      // Breakpoint detection code.
      if (_config.debug_mode()) {
        _o << "{\n"
           << "if (!_breakpoints.empty()) {\n";
        if (use_nia) {
          _o << "addr_t ea = " << _nia->name() << "_read().uint64();\n";
        }
        _o << getMemPointsIterator(false) << " i = _breakpoints.find(ea);\n"
           << "if (i != _breakpoints.end()) {\n"
           << "issueBreakpoint(ea,i);\n"
           << "}\n"
           << "}\n"
           << "}\n\n";
      }
    }

    // Writes final checks, such as for breakpoints.
    void writeCommitChecks()
    {
      _o << "void " << getCommitCheckName() << "() {\n";
      _o << getCtxUpdateName() << "();\n";      
      if (!transactional_iss()) {
        // writeBreakpointCheck(true);      
        _o << "addr_t ea = " << _nia->name() << "_read().uint64();\n";
        _o << getBreakpointCheckName() << "(ea);\n";
      }
      _o << "}\n\n";

      _o << "void " << getBreakpointCheckName() << "(addr_t ea)\n{\n";
      writeBreakpointCheck(false);
      _o << "}\n\n";
    }
  
    void writeCheckForBreakPoints()
    {
      _o << "void " << getCheckForBreakPointsName() << "(addr_t ea)\n{\n";
      _o << getBreakpointCheckName() << "(ea);\n";
      _o << "}\n\n";
    }

    // When the user sets --log-td-commit, we log completion times based upon the
    // td counter. Useful for simple simulations that use an ADL model w/timing.
    // No need to do this for time-tagged ISSs, since they already log issue and
    // completion timing.
    void writeLogTdCommit(ostream &o,const gc_string &pfx,bool not_method)
    {
      if (!time_tagged() && log_td_commit()) {
        if (not_method) {
          o << "ExtCondTrace({\n";
        }
        o << "if (Tracing && (" << pfx << "trace_mode() & ttInstrTimes)) {\n";

        if (compl_time()) {
          o << pfx << "_logger->log_instr_completion_time(" << pfx << "complTime(" << pfx << "_td_counter));\n";
        } else {
          o << pfx << "_logger->log_instr_completion_time(" << pfx << "_td_counter);\n";
        }

        o << "}\n";
        if (not_method) {
          o << "})\n";
        }
      }
    }

    // Generates the meat of the model- the loop which fetches and executes
    // instructions.
    void generateSim()
    {
      const InstrTables &tables = _core.instrtables();

      _o << "\nvoid simulate()\n"
         << headerOnly()
         << "{\n";

      gc_string extra_exec_decls, extra_exec_args, extra_exec_parms;
             
      if (_core.parallel_execution()) {
        extra_exec_decls += "unsigned packet_instr_ctr = 0;\n";
        
        extra_exec_args  += ",packet_instr_ctr";
        extra_exec_parms += ",unsigned &packet_instr_ctr";

        _o << extra_exec_decls
           << "bool first_instruction_in_packet = true;\n"
           << "while (_active && (first_instruction_in_packet || getPacketPosition() != 0)) {\n"
           << "first_instruction_in_packet = false;\n";
      }

      if (_core.prefix_bits()) {
        extra_exec_decls += "uint32_t " + _fname + " = " + _iname + "[0];\n"; // To accommodate micro_instr_run(), use _iname instead of buf
        
        extra_exec_args  += "," + _fname;
        extra_exec_parms += ",uint32_t " + _fname;
      }

      gc_string tbl_exec_args, tbl_exec_parms, alt_tbl_arg;
      if (tables.size() > 1) {
        tbl_exec_args  += gc_string(",") + _citname->ToString();
        extra_exec_parms += "," + getInstrTableType() + " " + getInstrTableArg();
      }

      _o << "if (InstrMax && (_instr_count >= InstrMax)) {\n"
         << "halt();\n"
         << "}\n";

      _o << "if (_active) {\n";

      writeHeartbeat();

      if (have_dt()) {
        _o << getDepReset() << "();\n\n";
      }
  
      if (haveExceptions()) {
        _o << "try {\n";
      } else {
        _o << "{\n";
      }
      _o << "{\n";

      bool write_skip_instr = writeFetchCode();
    
      bool dr = (_core.decode_retry_handler()._func);

      if (dr) {
        _o << "} while (exec(true" << extra_exec_args << tbl_exec_args << ") == esTryAgain);\n\n";      
      } else {
        _o << "exec(true" << extra_exec_args << tbl_exec_args << ");\n\n";
      }

      // Jump target used to implement the skip_instruction() command.
      if (write_skip_instr) {
        if (normal_iss()) {
          // For hybrid/transactional ISSs, we don't use a goto, so this isn't
          // necessary.
          _o << "skip_instruction_target: ;\n";
        }
 
      }
      
      writePostExec();

      _o << "}\n";
      _o << "}\n";
     
      // We'll increment the td counter just to be consistent with how the
      // decode-cache and JIT models work.
      writeExceptionCheck(true,false,true,true,false);

      writeInstrAttrClear(_o);

      writeLogCommit(_o);

      _o << getCommitCheckName() << "();\n";

      _o << ";\n"
         << "}\n";
      if (_core.parallel_execution()) {
        _o << "}\n";
      }
      _o << "}\n\n"
         << endHeaderOnly()

         << "\nExecStatus exec(bool extra_fetch" << extra_exec_parms << ")\n"
         << headerOnly()
         << "{\n"
         << "ExecStatus rc = esNormal;\n";
      // Create the decode tree.
      SimCodeGen codegen(_o,*this);
      writeDecodeTables(tables, codegen, true);

      // Commit parallel execution.
      if (_core.parallel_execution() || _core.has_delayed_writes()) {
        _o << "// Commit updates.\n";
        if (postFetchSkip()) {
          _o << "_skip_instruction_commit = false;\n";
        }
        _o << "empty_commit_queue();\n";
      }
    
      _o << "return rc;\n"
         << "}\n\n"
         << endHeaderOnly()

         << "ProgramStatus exec_from_buffer(uint32_t *buf,unsigned n)\n"
         << headerOnly()
         << "{\n"
         << "copy_to_instr_buf(" << _iname << ",buf," << (_core.instr_width()-1)/32 + 1 << ",n," << instr_little_endian() << ");\n" //
         << extra_exec_decls
         << "try {\n"
         << "++_instr_count;\n"
         << "if (Tracing && (_trace_mode & ttInstrPrefetch)) {\n"
         << "_logger->log_instr_from_buffer();\n"
         << "}\n";
      if (haveExceptions()) {
        _o << "try {\n";
      } else {
        _o << "{\n";
      }
      _o << "exec(false" << extra_exec_args << tbl_exec_args << ");\n"
         << "}\n";
      writeExceptionCheck(false,false,true,true,false);

      _o << "return (activeCount()) ? ACTIVE : HALT;\n"
         << "}\n"
         << "catch (SimInterrupt) {\n"
         << "return BREAK;\n"
         << "}\n"
         << "}\n\n"
         << endHeaderOnly();

      // Only generate micro_instr_run() when there are more than one instruction tables.
      // Most of the below is the same as that of exec_from_buffer()
      if (tables.size() > 1) {
        if (_core.has_micro_ops()) {
          if (_core.has_micro_op_offset()) {
            
            _cd._member_vars.push_back("unsigned _current_micro_op_offset");
            _cd._constr_inits.push_back("_current_micro_op_offset(0)");

            _cd._member_vars.push_back("unsigned _next_micro_op_offset");
            _cd._constr_inits.push_back("_next_micro_op_offset(0)");
            
            _cd._reset_stmts.push_back("resetMicroOpOffset()");

            _o << "void setMicroOpOffset(unsigned o) {\n"
               << "_next_micro_op_offset = o;\n"
               << "if (Tracing && (trace_mode() && ttAnnotation)) {\n"
               << "static adl::LogBase::Data data[1] = { {\"offset\",uint64_t{0}} };\n"
               << "data[0]._value = _next_micro_op_offset;\n"
               << "_logger->log_annotation(mInfo,0,\"Micro-op offset update\",1,data);\n"
               << "}\n"
               << "};\n\n"

               << "unsigned getMicroOpOffset() const { return _current_micro_op_offset; };\n\n"

               << "template <size_t Nb> void setMicroOpOffset(const bits<Nb> &o) { setMicroOpOffset(o.uint32()); };\n"
              
               << "void resetMicroOpOffset() {\n"
               << "_next_micro_op_offset = _current_micro_op_offset = 0;\n"
               << "};\n\n"

               << "void updateMicroOpOffset() {\n"
               << "_current_micro_op_offset = _next_micro_op_offset;\n"
               << "_next_micro_op_offset = 0;\n"
               << "}\n";
          }
          
          _o << "ExecStatus micro_instr_run(" << getInstrTableType() << " nit, bool commit_flag)\n"
             << headerOnly()
             << "{\n"
             << extra_exec_decls
             << "ExecStatus rc;\n"
             << "if (Tracing && (_trace_mode & ttInstrPrefetch)) {\n"
             << "_logger->log_instr_from_buffer();\n"
             << "}\n";
          if (haveExceptions()) {
            _o << "try {\n";
          } else {
            _o << "{\n";
          }
          _o << "rc = exec(false" << extra_exec_args << ",nit);\n";

          // Only process commit_flag under certain conditions
          if ((_core.parallel_execution() || _core.has_delayed_writes()) && 
              ((_config.mem_if() != mNonBlocking) || normal_iss())) {
            _o << "if (commit_flag) {\n";
            if (!_core.has_prefix_instruction() && !transactional_iss()) {	
              _o << getCommitQueueName() << ".commit();\n";
              if (_core.has_delayed_writes()) {
                _o << getCommitQueueName() << ".next();\n";					 
              }
            }
            _o << "}\n";
          }
          
          _o << "}\n";
          writeExceptionCheck(false,false,true,true,false);
          _o << "return rc;\n"
             << "}\n\n"
             << endHeaderOnly();
        }
      }
    }

    // Write the guts of the main decode function for a transactional ISS.
    void writeTransDecodeBody(opfxstream &o,const InstrTables &tables)
    {
      o << "{\n"

        << "addr_t instr_addr;\n"
        << decode_return_type() << "instr = 0;\n";

      if (logging_mem()) {
        o << PktName << " = 0;\n";
      }

      if (haveExceptions()) {
        o << "try {\n";
      }

      bool write_skip_instr = false;
      if (!logging_mem()) {
        o << "_memory_request.reset();\n";
      }

      o << "if (Tracing && _trace_mode) {\n"
        << "_logger->log_current_instr(0);\n"
        << "}\n";

      if (_core.decode_retry_handler()._func) {
        o << "retry_decode_target: \n";
      }

      gc_string ivalue;
      if (_core.prefix_bits()) {
        ivalue = _fname;
        o << "uint32_t " << _fname << ";\n";
      } else {
        ivalue = _iname + "[0]";
      }

      if (logging_mem()) {
        if (haveExceptions()) {
          o << "try {\n";
        }
        o << "if (speculative) {\n"
          << "instr_addr = addr;\n"
          << _iname << "[0] = get_next_instr_word(instr_addr);\n"
          << "} else {\n"
          << _cia->name() << "_write(" << _nia->name() << "_read());\n";
      }
    
      o << "instr_addr = " << _cia->name() << "_read().uint64();\n"
        << "++_instr_count;\n"
        << "if (InstrMax && (_instr_count > InstrMax)) {\n"
        << "halt();\n"
        << "return 0;\n"
        << "}\n";

      write_skip_instr = writePreFetchHandler(o);

      // Jump target used to implement the skip_instruction() command.
      if (write_skip_instr && normal_iss()) {
        o << "goto normal_continue;\n"
          << "skip_instruction_target:\n"
          << "return 0;\n"
          << "normal_continue:\n";
      }

      if (logging_mem()) {
        o << ivalue << " = get_next_instr_word(instr_addr);\n\n";
      } else {
        o << ivalue << " = get_next_instr_word(addr);\n\n";
      }

      if (logging_mem()) {
        o << "}\n";
        if (haveExceptions()) {
          o << "}\n"
            << "catch (Exception x) {\n"
            << "if (Tracing && (_trace_mode & ttInstrPrefetch)) {\n"
            << "_logger->log_instr_prefetch(instr_addr);\n"
            << "}\n";
          if (has_decode_arg()) {
            o << decode_arg(false) << ".signalFetchException();\n";
          }
          o << "throw;\n"
            << "}\n";
        }
      }
       
      // Create the decode tree.
      TransCodeGen codegen(o,*this);
      writeDecodeTables(tables, codegen, true);

      if (haveExceptions()) {
        gc_string eflag = getExceptionFlag();
        o << "}\n"
          << "catch (Exception x) {\n";
        if (logging_mem()) {
          o << "if (!speculative) ";
        }
        o << "setException(x);\n"
          << "}\n"

          // Note: Only do this if we have non-level-sensitive exceptions.  For
          // external exceptions, that's handled separately by uADL.
          << "if (hasNonPendingExceptions()) {\n"
          << "genExceptionsInternal(" << eflag << ");\n"
          << "if (" << PktName << ") " << PktName << "->set_exceptions(__exception_flag);\n"
          << eflag << " = 0;\n";
        if (hybrid_iss() && has_decode_arg()) {
          o << decode_arg(false) << ".signalException(instr);\n";
        }
        o << "}\n";
      }
      o << "return instr;\n"
        << "}\n";
    }

    void writeQuickDecode()
    {
      _o << "bool quick_decode(addr_t addr,const byte_t *data,unsigned offset) {\n"

         << "_memory_request.set_result(0,(byte_t*)data,offset);\n"
         << "_memory_request.reset();\n"

         << "if (Tracing && _trace_mode) {\n"
         << "_logger->log_current_instr(0);\n"
         << "}\n"

         << "++_instr_count;\n"
         << "if (InstrMax && (_instr_count > InstrMax)) {\n"
         << "halt();\n"
         << "return false;\n"
         << "}\n";

      writePreFetchHandler(_o);

      gc_string ivalue;
      if (_core.prefix_bits()) {
        ivalue = "uint32_t " + _fname;
      } else {
        ivalue = _iname + "[0]";
      }
      
      _o << _cia->name() << "_write(addr);\n"
         << ivalue << " = get_next_instr_word(addr);\n\n";
      if (_config.trace_mode()) {
        _o << "if (Tracing && (_trace_mode & ttInstrPrefetch)) {\n"
           << "_logger->log_instr_prefetch(addr);\n"
           << "}\n";
      }
      _o << "return true;\n"
         << "}\n\n";
    }

    void writeTransDecodeFuncs()
    {
      const InstrTables &tables = _core.instrtables();

      // This prefetch translate function can be used to do an instruction
      // translation, with proper handling of exceptions, without performing
      // an actual memory read.
      _o << "PrefetchReturnType prefetch_translate(addr_t &ra,addr_t ea)\n"
         << "{\n"
         << PktName << " = 0;\n";
      if (!hybrid_iss()) {
        // For a normal-mode model, we don't want MMU logging at this point- it'll
        // just confuse the trace.
        _o << "uint64_t f = 0;\n";
        if (haveExceptions()) {
          _o << "try {\n";
        }
        _o << "f = set_tracing_flags(0);\n";
      }
      _o << "ra = instrReadTranslate(ea);\n";
      if (!hybrid_iss()) {
        _o << "f = set_tracing(f);\n";
      }
      _o << "return prActive;\n";
      if (haveExceptions() && !hybrid_iss()) {
        _o << "}\n"
           << "catch (Exception x) {\n";
        if (!hybrid_iss()) {
          _o << "set_tracing(f);\n";
        }
        _o << "if (Tracing && (_trace_mode & ttInstrPrefetch)) {\n"
           << "_logger->log_instr_prefetch(ea);\n"
           << "}\n"
           << "setException(x);\n";
        if (trans_rethrow()) {
          _o << "throw;\n";
        } else {
          _o << "return prException;\n";
        }
        _o << "}\n\n";
      }

      gc_string data, data_arg, data_arg1, read, read_arg;
      gc_string data_ra, data_ra_arg, data_ra_arg1;
      if (logging_mem()) {
        data = "data,";
        data_arg = "byte_t *data";
        data_arg1 = data_arg + ',';
        read = ",ReadData";
        read_arg = ",bool ReadData";

        data_ra = data + "ra,";
        data_ra_arg = data_arg + ",addr_t &ra";
        data_ra_arg1 = data_ra_arg + ',';
      }

      _o << "}\n\n"
        // The prefetch function, for a transactional ISS, initiates the
        // instruction fetch operation.
         << "\n";
    
      if (!logging_mem()) {
        _o << "addr_t cia_ea() const { return _cia_ea; };\n"
           << "\n";
      }

      _o << "template <size_t NB" << read_arg << ">\n"
         << "PrefetchReturnType prefetch(" << data_ra_arg << ")\n"
         << "{\n"
         << "return prefetch<NB" << read << ">(" << data_ra << _nia->name() << "_read().uint64());\n"
         << "}\n"
         << "\n"
         << "template <size_t NB" << read_arg << ">\n"
         << "PrefetchReturnType prefetch(" << data_ra_arg1 << "addr_t addr)\n"
         << "{\n"
        // Here, we want to make sure that any register modifications associated
        // with this prefetch are not tacked on to the last instruction.
         << "if (Tracing && _trace_mode) {\n"
         << "_logger->log_current_instr(0);\n"
         << "}\n"
         << PktName << " = 0;\n"
         << "uint64_t f = 0;\n";
      if (haveExceptions()) {
        _o << getExceptionFlag() << " = 0;\n"
           << "try {\n";
      }
      _o << "if (!_active) {\n"
         << "return prNotActive;\n"
         << "}\n";

      if (!hybrid_iss()) {
        writeHeartbeat();
      }

      // Write the prefetch code.  Only increment the current-instruction-address
      // register if so configured.
      if (!logging_mem()) {
        writePreFetchCode(false,_config.prefetch_cia_incr());
        _cd._member_vars.push_back("addr_t _cia_ea");

        _o << "_cia_ea = " << _cia->name() << "_read().uint64();\n";
      }

      if (logging_mem()) {
        _o << "if (ReadData) {\n";
      } else {
        _o << "{\n";
      }
      _o << "f = set_tracing_flags(0);\n"
         << "instr_read<NB>(" << data_ra << "addr);\n"
         << "set_tracing_flags(f);\n"
         << "}\n"
         << "return prActive;\n";

      if (haveExceptions()) {
        _o << "}\n"
           << "catch (Exception x) {\n"
           << "set_tracing_flags(f);\n";
        if (!hybrid_iss()) {
          _o << "if (Tracing && (_trace_mode & ttInstrPrefetch)) {\n"
             << "_logger->log_instr_prefetch(addr);\n"
             << "}\n";
        }
        _o << "setException(x);\n";
        if (trans_rethrow()) {
          _o << "throw;\n";
        } else {
          _o << "return prException;\n";
        }
        _o << "}\n";
      }
      _o << "}\n\n"
      
        // This function returns the next instruction word.
         << "uint32_t get_next_instr_word(addr_t addr) {\n"
         << "return instr_read32(addr);\n"
         << "}\n\n";

      // If we're cacheing instruction packets, then this is used to simulate a
      // fetch, even though we're not going through the full decode tree.
      if (cached_packets()) {
        writeQuickDecode();
      }

      // The decode function does the decode and returns a packet storing the
      // opcode and an identifier used to dispatch to the appropriate logic for
      // subsequent operations.  It reads directly from the memory_request's first
      // page, so this pointer must point to at least 4 bytes of storage

      gc_string tbl_parm;
      if (tables.size() > 1) {
        tbl_parm = "," + getInstrTableType() + " " + getInstrTableArg();
      }

      if (!logging_mem()) {
        _o << "\n" << decode_return_type() << "decode(" << decode_parm(false) << ")\n"
           << "{\n"
           << "return decode(cia_ea(),4" << decode_arg(true) << ");\n"
           << "}\n"

           << "\n" << decode_return_type() << "decode(addr_t cia, const byte_t *data,unsigned offset,unsigned size" << decode_parm(true) << ")\n"
           << "{\n"
           << _cia->name() << "_write(cia);\n"
           << "_memory_request.set_result(0,(byte_t*)data,offset);\n"
           << "return decode(cia,size" << decode_arg(true) << ");\n"
           << "}\n";
      }
      
      gc_string decode_decl_start, decode_decl, decode_impl_decl;
      if (logging_mem()) {
        decode_decl_start = "decode(addr_t addr,unsigned size,bool speculative";
      } else {
        decode_decl_start = "decode(addr_t addr,unsigned size";
      }
      decode_decl = decode_impl_decl = decode_decl_start + tbl_parm + decode_parm(true) + ")";

      // For compatibility, if we have multiple tables, create a form of the
      // decode function which uses the curent table type.
      if (!tbl_parm.empty()) {
        _o << "\n" << decode_return_type() << decode_decl_start + decode_parm(true) + ")"
           << "{\n"
           << "return decode(addr,size" << ( logging_mem() ? ",speculative" : "") << ",getCurrentInstrTable()" << decode_arg(true) << ");\n"        
           << "}\n\n";
      }

      
      // In splitting compilation across multiople files, we put this into the
      // first implementation file so that it cannot be inlined.  This large
      // function seems to make the inliner on some compilers go bonkers, so
      // compilation is helped tremendously by moving it.
      if (multi_compile()) {
        _o << "\n" << decode_return_type() << decode_decl << ";\n\n";
        opfxstream &j0 = ((*_impls)[0]);
        j0 << "\n" << decode_return_type(_class_name_pfx) << _class_name_pfx << decode_impl_decl << "\n";
        writeTransDecodeBody(j0,tables);        
      } else {
        // Not multi-compile, so just guard it so that the main file can be
        // viewed as both a header and an implementation file.
        _o << "\n" << decode_return_type() << decode_decl << "\n"
           << headerOnly();
        writeTransDecodeBody(_o,tables);
        _o << endHeaderOnly();
      }
    }

    // This generates the guts of the transactional ISS, which consists of a fetch 
    // function (which does the decoding).
    void generateTransSim()
    {
      // Time is recorded using a member variable so that separate cores can track time individually.
      _cd._member_vars.push_back("const ttime_t *_time");
      _cd._constr_args.push_back("const ttime_t *time");
      _cd._constr_inits.push_back("_time(time)");

      // Method to set the time pointer.
      _o << "void set_time(const ttime_t *time) { _time = time; };\n\n";

      writeTransDecodeFuncs();

      _o << "void discard_packet(" << PktType << " *" << PktArg << ") {\n"
         << "if (!" << PktArg << ") return;\n";
      if (_config.mem_if() == mNonBlocking) {
        _o << "if (" << PktArg << "->_stack) {\n"
           << "this->_coro_stacks.free(" << PktArg << "->_stack);\n"
           << PktArg << "->_stack = 0;\n"
           << "}\n";
        if (_has_commit_stack) {
          _o << "if (" << PktArg << "->_cstack) {\n"
             << "this->_coro_stacks.free(" << PktArg << "->_cstack);\n"
             << PktArg << "->_cstack = 0;\n"
             << "}\n";
        }
      }

      _o << "\n"
         << "_packets.free(" << PktArg << ");\n"
         << "}\n\n"
         << "void free_packet(" << PktType << " *" << PktArg << ") {\n"
         << "_logger->log_current_instr(" << PktArg << ");\n"
         << "\n"
         << "if (Tracing && _trace_mode) {\n"
         << "_logger->log_instr_completion_time(*_time);\n"
         << "_logger->log_commit(" << PktArg << ");\n"
         << "}\n"
         << PktArg << "->_exitems.reset();\n"
         << "discard_packet(" << PktArg << ");\n"
         << getCommitCheckName() << "();\n"
         << "}\n\n"

         << "void log_flush_pending() {\n"
         << "if (Tracing && _trace_mode) {\n"
         << "_logger->log_flush_pending();\n"
         << "}\n"
         << "}\n\n"
         << "void log_direct(bool d) {\n"
         << "if (Tracing && _trace_mode) {\n"
         << "_logger->log_direct(d);\n"
         << "}\n"
         << "}\n\n"
         << "void log_commit_pending() {\n"
         << "if (Tracing && _trace_mode) {\n"
         << "_logger->log_commit(0);\n"
         << "}\n"
         << "}\n\n"
         << "LogType log_get_pending_queue() {\n"
         << "if (Tracing && _trace_mode) {\n"
         << "return _logger->log_get_pending_queue();\n"
         << "} else {\n"
         << "return 0;\n"
         << "}\n"
         << "}\n\n"
         << "void log_append_pending_queue(LogType l) {\n"
         << "if (Tracing && _trace_mode) {\n"
         << "_logger->log_append_pending_queue(l);\n"
         << "}\n"
         << "}\n\n"
         << "void log_instr_prefetch(addr_t addr) {\n"
         << "if (Tracing && (_trace_mode & ttInstrPrefetch)) {\n"
         << "_logger->log_instr_prefetch(addr);\n"
         << "}\n"
         << "}\n\n"
         << "void log_instr_issue_time(" << PktType << " *instr,ttime_t t) {\n"
         << "if (Tracing && (_trace_mode & ttInstrPrefetch)) {\n"
         << "_logger->log_current_instr(instr);\n"
         << "_logger->log_instr_issue_time(t);\n"
         << "}\n"
         << "}\n\n"
         << "void log_current_instr(PacketLogBase *instr) {\n"
         << "if (Tracing && _trace_mode) {\n"
         << "_logger->log_current_instr(instr);\n"
         << "}\n"
         << "}\n\n"
         << "void log_commit(PacketLogBase *instr) {\n"
         << "if (Tracing && _trace_mode) {\n"
         << "_logger->log_commit(instr);\n"
         << "}\n"
         << "}\n\n";
    }

    void writeDisasmDecode(ostream &o,const Instr *instr,const gc_string &name)
    {
      InstrSym &sym = _instr_syms[instr->name()];

      unsigned num_bytes = instr->fetch_width()/8; 
      o << "// Instruction:  \"" << instr->name() << "\"\n"
        << sym._disasm << "(o,addr," << name << ");\n"
        << "return " << dec << num_bytes << ";\n";
    }

    void writeDisasmDecodeMiss(ostream &o,const gc_string &name)
    {
      o << "disasm_illop(o,addr," << name << ");\n"
        << "return 4;\n";
    }

    // This class is used to generate the optional disassembler function.
    struct DisCodeGen : CodeGenIF{
      DisCodeGen(ostream &o,GenInstrs &gi,const gc_string &name) : CodeGenIF(o,gi), _name(name) {};

      // Return the function name to fetch the next instruction word.
      virtual const gc_string &fetch_name() const { return _name; };
      virtual gc_string get_instr_read()    { return "debug_instr_read_internal"; };
      virtual gc_string get_fetch_addr()    { return "addr"; };
      virtual gc_string extra_fetch_parms() { return ",trans,32"; };
      // Called to write the table selection expression.
      virtual void writeTableSelect() { _o << "tindex"; };
      // Called for a valid leafnode.
      virtual void writeEntry(const Instr *instr) { _gi.writeDisasmDecode(_o,instr,_name); };
      // Called for handling a default case, e.g. a decode miss. 
      virtual void writeDefault() { _gi.writeDisasmDecodeMiss(_o,_name); };

      gc_string _name;
    };


    // Optionally, create a disassembler function.
    void generateDisasm()
    {
      if (disassembler()) {
        const InstrTables &tables = _core.instrtables();

        if (_citname) {
          _o << "int disassemble_table_select(int tindex) const\n"
             << "{\n"
             << "return (tindex < 0) ? " << _citname << " : tindex;\n"
             << "}\n"
             << "\n";
        }

        unsigned num_fetches = (_core.instr_width() / 32);
        if (_core.instr_width() % num_fetches) {
          ++num_fetches;
        }
        
        _o << "virtual unsigned disassemble(std::ostream &o,addr_t addr,bool trans,int tindex) const\n"
           << "{\n"
           << "uint32_t instr[" << (num_fetches+1) << "];\n";

        ForRange(num_fetches,i) {
          _o << "instr[" << i << "] = debug_instr_read_internal(addr + " << (i*4) << ",trans,32);\n";
        }

        gc_string ts = (_citname) ? "disassemble_table_select(tindex)" : "-1";
        
        _o << "return disassembleInternal(o,addr,instr,trans," << ts << ");\n"
           << "}\n\n"
      
           << "static unsigned disassembleInternal(std::ostream &o,addr_t addr,unsigned *instr,bool trans,int tindex)\n"
           << headerOnly()
           << "{\n";

        DisCodeGen codegen(_o,*this,"instr");
        writeDecodeTables(tables,codegen,false);

        _o << "return 0;\n"
           << "}\n\n"
           << endHeaderOnly();
      }
    }

    gc_string writeJitFetchUpdateCode(const gc_string &pfx = gc_string())
    {
      gc_string s;

      s += "++" + pfx + "_instr_count;\n";

      s += ((_cia != _nia) ? (pfx + _cia->name() + "_write(" + pfx + _nia->name() + "_read());\n") : "");
      return s;
    }


    void genDcSupport()
    {
      if (_config.debug_mode()) {
        // When adding or removing breakpoints, we simply invalidate any
        // translated code that matches the address of interest.  That way, we'll
        // have to retranslate and when we do, a breakpoint return will be added.
        _o << "void addDcBreakpoint(addr_t ea)\n"
           << "{\n"
           << "_dc_bb_hash.invalidate_ea(ea,_dc_decode_cache);\n"
           << "}\n\n"
           << "void removeDcBreakpoint(addr_t ea)\n"
           << "{\n"
           << "_dc_bb_hash.invalidate_ea(ea,_dc_decode_cache);\n"
           << "}\n\n";
      }

      // Check decode-cache full state.
      _o << "bool decode_cache_full() {\n"
         << "if (_dc_decode_cache.full(TranslationParms::max_bb_instr_count())) {\n"
         << "_dc_bb_hash.reset(_dc_decode_cache,true);\n"
         << "return true;\n"
         << "}\n"
         << "return false;\n"
         << "}\n\n";
    }

    void genJitSupport()
    {   
      if (_config.debug_mode()) {
        // When adding or removing breakpoints, we simply invalidate any
        // translated code that matches the address of interest.  That way, we'll
        // have to retranslate and when we do, a breakpoint return will be added.
        _o << "void addJitBreakpoint(addr_t ea)\n"
           << "{\n"
           << "_jit_bb_hash.invalidate_ea(ea,_jit);\n"
           << "}\n\n"
           << "void removeJitBreakpoint(addr_t ea)\n"
           << "{\n"
           << "_jc.signal_reset();\n"
           << "_jit_bb_hash.invalidate_ea(ea,_jit);\n"
           << "}\n\n";
      }

      // We need to override the tracing control functions in order to modify code
      // generation and to invalidate existing JIT functions.
      if (_config.trace_mode()) {
        // We only need to invalidate if there's been an actual change to the
        // flags.
        _o << "void update_jit_tracing(uint64_t orig_flags)\n"
           << "{\n"
           << "if (trace_mode() != orig_flags) {\n"
           << "_jc.signal_reset();\n"
           << "_jit.set_tracing(trace_mode());\n"
           << "_jit_bb_hash.reset(_jit,false);\n"
           << "}\n"
           << "}\n\n"

           << "uint64_t set_tracing(uint64_t flags = (uint64_t)-1)\n"
           << "{\n"
           << "uint64_t p = IssCore::set_tracing(flags);\n"
           << "update_jit_tracing(p);\n"
           << "return p;\n"
           << "}\n\n"

           << "uint64_t clear_tracing(uint64_t flags = (uint64_t)-1)\n"
           << "{\n"
           << "uint64_t p = IssCore::clear_tracing(flags);\n"
           << "update_jit_tracing(p);\n"
           << "return p;\n"
           << "}\n\n"

           << "uint64_t set_tracing_flags(uint64_t flags)\n"
           << "{\n"
           << "uint64_t p = IssCore::set_tracing_flags(flags);\n"
           << "update_jit_tracing(p);\n"
           << "return p;\n"
           << "}\n\n";
      }

      // This allows for the dumping of the generated JIT code.
      _o << "void dump_jit_code() {\n"
         << "_jit.dump();\n"
         << "}\n\n"

        // Check memory usage and reset our hash if we exceed it.  If we exceed
        // our LLVM bb count, then reset just that.  If we exceed our decode-cache
        // size, then reset everything.
         << "bool jit_full() {\n"
         << "if (_jit.bb_count() > TranslationParms::max_bb_count()) {\n"
         << "_jc.signal_reset();\n"
         << "_jit_bb_hash.reset_bbs(_jit);\n"
        // Return false in this situation, since we're not resetting the hash,
        // just clearing out compiled code in existing basic blocks.
         << "return false;\n"
         << "}\n"
         << "if (_jit_decode_cache.full(TranslationParms::max_bb_instr_count())) {\n"
         << "_jc.signal_reset();\n"
         << "_jit_decode_cache.reset();\n"
         << "_jit_bb_hash.reset(_jit,false);\n"
        // Here, we do have to return true, because the hash is cleared.
         << "return true;\n"
         << "}\n"
         << "return false;\n"
         << "}\n\n";
    }


    // Various logic used by both the decode cache and JIT.
    void generateCommonTranslationCode()
    {
      if (!(jit_support() || dc_support())) return;

      if (_config.debug_mode()) {
        // Returns true if we match against an existing breakpoint.
        _o << "bool check_for_breakpoint(addr_t addr)\n"
           << "{\n"
           << "if (!_breakpoints.empty()) {\n"
           << getMemPointsIterator(true) << " i = _breakpoints.find(addr);\n"
           << "if (i != _breakpoints.end()) {\n"
           << "return true;\n"
           << "}\n"
           << "}\n"
           << "return false;\n"
           << "}\n\n";
      }

      // This is here b/c we use it for tracking decode-cache compiles for both
      // standalone decode-cache and the JIT.
      _cd._member_vars.push_back("unsigned _dc_compiles");
      _cd._reset_stmts.push_back("_dc_compiles = 0");
      _cd._constr_inits.push_back("_dc_compiles(0)");

      _cd._member_vars.push_back("unsigned _cold_runs");
      _cd._reset_stmts.push_back("_cold_runs = 0");
      _cd._constr_inits.push_back("_cold_runs(0)");

      bool pitd  = (!time_tagged() && !graphite_support() && td_api() && per_instr_td_incr());
      bool npitd = (!time_tagged() && !graphite_support() && td_api() && !per_instr_td_incr());

      gc_string rt, rv_td, rv_ntd;
      if (td_api()) {
        rt = "bool";
        rv_td = " true";
        rv_ntd = " false";
      } else {
        rt = "void";
      }

      _o << "\n" << rt << " exec_cold_block()\n"
         << headerOnly()
         << "{\n"
         << "++_cold_runs;\n"
         << "unsigned count = 0;\n"
         << "while (1) {\n";

      // To mirror the normal simulator, check for exceeding the threshold
      // immediately.  This catches things like a fetch operation clearing the
      // threshold, for example.
      if (td_api()) {
        _o << "if (_td_counter >= _td_threshold) return true;\n";
      }

      bool write_skip_instr = writeFetchCode();

      if (pitd) {
        _o << "_td_counter += _td_increment;\n";
      }
      
      gc_string exec_args;
      if (_core.instrtables().size() > 1) {
        exec_args += gc_string(",") + _citname->ToString();
      }
      
      _o << "++count;\n"
         << "ExecStatus rc = exec(true" << exec_args << ");\n";

      if (write_skip_instr) {
        _o << "skip_instruction_target: ;\n";
      }

      writePostExec();
      writeInstrAttrClear(_o);
      writeLogCommit(_o);

      _o << getCommitCheckName() << "();\n";

      // For the normal JIT, we have the temporal-decoupling check at the end of
      // the BB, so this replicates that behavior.
      _o << "if (rc == esEndBasicBlock || (count >= TranslationParms::max_bb_instr_count())) {\n";
      if (npitd) {
        _o << "_td_counter += (_td_increment * count);\n";
      }
      _o << "return " << rv_ntd << ";\n"
         << "}\n"

         << "}\n"
         << "}\n\n"
         << endHeaderOnly();
    }

    // Writes n dispatch/escape-check entries in the trace-cache dispatch loop.
    void gen_dispatch_call(ostream &o,const gc_string &pfx,unsigned n,unsigned c)
    {    
      o << "dcf = (" << pfx << "DecodeFunc)(dcf)(this,&dci[" << dec << c << "]);\n";
      if (n) {
        o << "if (dcf > (" << pfx << "DecodeFunc)JitLastCode) {\n";
        gen_dispatch_call(o,pfx,n-1,c+1);
        o << "}\n";
      }
    }

    // Writes the unrolled-trace varient of the decode-cache kernel.
    // n:   Number of elements in each trace sequence.
    // nt:  Number of trace-sequence blocks.
    void writeDcKernel(ostream &o,const gc_string &tpfx,const gc_string &pfx,unsigned n,unsigned nt)
    {
      // This dispatch loop is based on the Nostradamus Distributor described in
      // the NO EXECUTE! blog:  http://emulators.com/docs/nx25_nostradamus.htm
      o << tpfx << "DecodeFunc dcf = (" << tpfx << "DecodeFunc)(JitLastCode+1);\n"
        << tpfx << "DecodePair dcp = bb->get_pair();\n"
        << tpfx << "DecodeItem *dci = " << pfx << "_decode_cache_next = dcp.second;\n"
        << "while (dcf >= (" << tpfx << "DecodeFunc)JitContCode) {\n"
        << "if ( (dci = " << pfx << "_decode_cache_next) != 0) {\n"
        << "dcf = dci[0].func();\n";
      gen_dispatch_call(o,tpfx,n,0);
      o << "}\n";
      for (unsigned t = 1; t < nt; ++t) {
        o << "if ( (dci = " << pfx << "_decode_cache_next) != 0) {\n"
          << "dcf = dci[0].func();\n";
        gen_dispatch_call(o,tpfx,n,0);
        o << "}\n";
      }
      o << "}\n";
    }

    unsigned getICacheLineMask()
    {
      if (const Cache *icache = _core.get_first_cache(InstrCache)) {
        return icache->linesize() - 1;
      } else {
        return _coreconfig.cache_line_size() - 1;
      }
    }

    void writeICacheTouchSetupFunc(TMode tmode)
    {
      if (!_instr_syms._icache_touch.valid()) return;

      gc_string tpfx, pfx;
      if (tmode == DcTrans) {
        tpfx = "Dc";
        pfx = "_dc";
      } else {
        tpfx = "Jit";
        pfx = "_jit";
      }

      _o << "void " << getICacheTouchName(tmode) << " (addr_t pc";
      if (tmode == JitTrans) {
        _o << ",MyJitBasicBlock *jb";
      }
      _o << ") {\n"
         << tpfx << "DecodeItem *di = " << pfx << "_decode_cache.cur();\n"
         << "di->set_func(" << getDcInstrPrefix(tmode) << _instr_syms._icache_touch._id << ");\n";
      if (tmode == JitTrans) {
        _o << "di->set_jit_trans(" << _instr_syms._icache_touch._trans << ");\n"
           << "jb->incr_dc_count();\n";
      }
      _o << "di->_args[0] = (pc >> 32);\n"
         << "di->_args[1] = pc;\n"
         << pfx << "_decode_cache.add_item();\n"
         << "}\n\n";

      gc_string extra_arg;
      if (tmode == JitTrans) {
        extra_arg = ",jb";
      }

      // If we have variable-sized instructions, then also write a function which
      // can detect a line crossing.  This is a bit complicated: If we have an
      // icache, then we touch the appropriate address.  If we don't, but we're
      // still told to generate icache touches, then we are going to use
      // instr_read32, so we just touch the initial address of the instruction if
      // we detect a crossing.
      if (hasMultiInstrLengths()) {
        unsigned lm = getICacheLineMask();

        bool has_icache = _core.get_first_cache(InstrCache);

        _o << "void " << getICacheCrossingName(tmode) << " (addr_t pc,int num_bytes,unsigned count";
        if (tmode == JitTrans) {
          _o << ",MyJitBasicBlock *jb";
        }
        _o << ") {\n"     
           << "if (count == 0) {\n"
           << getICacheTouchName(tmode) << "(pc" << extra_arg << ");\n"
           << "} else ";

        if (has_icache) {
          _o << "if ( ((pc & " << lm << ") == 0) || (count == 0)) {\n"
             << getICacheTouchName(tmode) << "(pc" << extra_arg << ");\n"
             << "}\n"
             << "if ((pc & ~" << lm << "ull) != ( (pc+num_bytes-1) & ~" << lm << "ull)) {\n"
             << getICacheTouchName(tmode) << "(pc+num_bytes-1" << extra_arg << ");\n"
             << "}\n";
        } else {
          _o << "if ( ((pc & " << lm << ") == 0) || (count == 0) || ((pc & ~" << lm << "ull) != ( (pc+num_bytes-1) & ~" << lm << "ull))) {\n"
             << getICacheTouchName(tmode) << "(pc" << extra_arg << ");\n"
             << "}\n";
        }
        _o << "}\n\n";
      }
    }

    // We have an instruction cache, so we need to touch it every time we
    // come to a new cache line.  This is only for new cache lines, however-
    // if we're fixing up an existing instruction, e.g. a branch, then the
    // cache-touching logic will already have been added.
    void writeICacheTouchLogic(TMode tmode)
    {
      if (_instr_syms._icache_touch.valid() && !hasMultiInstrLengths()) {

        unsigned lm = getICacheLineMask();

        _o << "if (extra_fetch && ((pc & " << lm << ") == 0 || (count == 0))) {\n"
           << getICacheTouchName(tmode) << "(pc";
        if (tmode == JitTrans) {
          _o << ",jb";
        }
        _o << ");\n"
           <<"}\n";
      }
    }

    void writeICacheCrossingCheck(ostream &o,TMode tmode,unsigned num_bytes)
    {
      if (_instr_syms._icache_touch.valid()) {

        if (hasMultiInstrLengths()) {
          gc_string extra_arg;
          if (tmode == JitTrans) {
            extra_arg = ",jb";
          }

          // If we have multiple instruction widths, then we have to do an icache
          // touch if we cross an icache line.
          o << getICacheCrossingName(tmode) << "(pc," << num_bytes << ",count" << extra_arg << ");\n";
        }

      }
    }

    void writeDmiEnable()
    {
      if (dmi_cache_support()) {
        if (_config.debug_mode()) {
          _o << "try {\n";
        }
        _o << getDmiCacheEnable() << "();\n";
      }
    }

    void writeDmiDisable()
    {
      if (dmi_cache_support()) {
        _o << getDmiCacheDisable() << "();\n";
        if (_config.debug_mode()) {
          _o << "}\n"
             << "catch (SimInterrupt) {\n"
             << getDmiCacheDisable() << "();\n"
             << "throw;\n"
             << "}\n";
        }
      }
    }

    // For a decode-cache model, this creates the translate function which translates
    // target code into native instruction handlers.
    void generateDcTranslate()
    {
      const InstrTables &tables = _core.instrtables();

      writeICacheTouchSetupFunc(DcTrans);

      gc_string extra_args, extra_parms;

      if (tables.size() > 1) {
        extra_parms += "," + getInstrTableType() + " " + getInstrTableArg();
        extra_args += gc_string(",") + _citname->ToString();
      }
      
      _o << headerOnly2()
         << "\naddr_t dc_translate_instr(addr_t &pc,DcBasicBlock *bb,unsigned count,bool extra_fetch" << extra_parms << ")\n"
         << "{\n";

      // Create the decode tree.
      DcCodeGen codegen(_o,*this);
    
      writeICacheTouchLogic(DcTrans);

      writeDecodeTables(tables, codegen, true);

      _o << "return true;\n"
         << "}\n\n"

        // The translate instruction translates a basic block of instructions,
        // i.e. a sequence of instructions with a single entry point (the start)
        // and a single exit point- either a branch, halt, or whatever.
         << "void translate_dc(addr_t addr,DcBasicBlock *bb)\n{\n"
         << "++_dc_compiles;\n"
         << "addr_t new_addr = addr;\n"
         << "unsigned count = 0;\n";
      if (_has_split_branches) {
        // Clear the split-branch target value when starting a basic block.  It's
        // also cleared by any instruction marked as a split-branch cancel.
        _o << "_sb_trg = 0;\n";
      }
      _o << "_dc_decode_cache.start_bb(bb," << (_config.debug_mode() ? "check_for_breakpoint(addr)" : "false") << ");\n";
      if (haveExceptions()) {
        _o << "try {\n";
      }
      
      _o << "while (1) {\n"
         << _iname << "[0] = instr_read32_nocache(new_addr);\n"
         << "bb->set_end_ra(_last_instr_ra);\n"
         << "if (!dc_translate_instr(new_addr,bb,count,true" << extra_args << ")) break;\n"
         << "count++;\n";
      if (_config.debug_mode()) {
        // If we encounter a breakpoint, then we simply finish the bb so that it
        // returns with a breakpoint code.  This is the normal case- we find a
        // breakpoint as we're translating a block.  Note that we check the *next*
        // address, so that it matches the behavior of the interpreter, where we
        // break when the NIA matches, at the bottom of the loop.
        _o << "if (check_for_breakpoint(new_addr)) {\n"
           << "bb->set_end_ra(instrReadTranslate(new_addr));\n"
           << "_dc_decode_cache.add_brkpt_stub(bb);\n"
           << "return;\n"
           << "}\n";
      }
      // If we exceed our maximum bb size, then we add on a direct jump to the
      // successor.  We also do this if we have an MMU and cross a page.
      _o << "if ( (count >= TranslationParms::max_bb_instr_count())";
      // if (_core.mmu()) {
      //   _o << " || (count > 1 && " << getInstrPageUpdateVar() << ")";
      // }
      _o << ") {\n"
         << "_dc_decode_cache.add_br_stub(bb);\n"
         << "return;\n"
         << "}\n"
         << "}\n";
      if (haveExceptions()) {
        _o << "}\n"
           << "catch (Exception current_exception) {\n"
           << "if (count == 0) throw;\n"
           << "}\n";
      }
      // Otherwise, finish the basic block.
      _o << "}\n\n";

      // Miscellaneous JIT support routines.
      genDcSupport();
       
      _o << "ProgramStatus simple_run_dc()\n"
         << "{\n";

      writePostSim(post_sim(),_class_name);
      writeDmiEnable();

      _o << "DcBasicBlock *bb = 0;\n"
         << "DcTranslate dct;\n"
         << "// The sim kernel runs until the core is inactive.  Translation occurs as a side-effect of lookup.\n"
         <<" // A null-return on a block lookup means that we got a cold-block and it was interpreted, so just loop\n"
         << "// and continue.\n"
         << "while (_active) {\n";

      if (haveExceptions()) {
        _o << "try {\n";
      } else {
        _o << "{\n";
      }

      // Just in case our threshold is already 0, we check to make sure we don't
      // continue on with executing any instructions.
      add_td_check(_o,"","return ACTIVE;","1",false,false,true,false);

      _o << "addr_t nia = " << _nia->name() << "_read().uint64();\n";
      if (haveExceptions()) {
        // We have to guard against instruction-fetch exceptions.  We need to
        // adjust the program counter, to mirror the fact that this would
        // normally have been done at the top of the simulation loop for a
        // non-jit model.
        _o << "try {\n";
      }

      // By default, look up basic blocks via the next-instruction address.  If
      // we have a micro-op mask to support branching into instruction packets,
      // then we add on the mask as a way to differentiate between basic blocks
      // starting at different micro ops.
      gc_string bb_addr = "nia";
      if (_core.has_micro_op_offset()) {
        bb_addr += ",_next_micro_op_offset";
      } else {
        bb_addr += ",0";
      }
      
      _o << "bb = _dc_bb_hash.get_bb(dct," << bb_addr << ",TranslationParms::dc_bb_hot_threshold());\n";
      if (haveExceptions()) {
        _o << "}\n"
           << "catch (Exception current_exception) {\n"
           << writeJitFetchUpdateCode()
           << "throw;\n"
           << "}\n";
      }
      _o << "if (!bb) {\n";

      if (td_api()) {
        _o << "if (exec_cold_block()) return ACTIVE;\n";
      } else {
        _o << "exec_cold_block();\n";
      }
      _o << "continue;\n"
         << "}\n";

      writeDcKernel(_o,"Dc","_dc",20,2);
   
      _o << "switch ((long)dcf) {\n"
         << "case JitNopCode:\n"
        // Check for any breakpoints which have occurred due to calculated
        // branches and such.
         << getCommitCheckName() << "();\n"
         << "break;\n"
         << "case JitRetCode:\n";
      writeExceptionCheck(true,false,false,false,false);
      _o << getCommitCheckName() << "();\n"
         << "if (InstrMax && (_instr_count > InstrMax)) {\n"
         << "halt();\n"
         << "return HALT;\n"
         << "}\n"
         << "return ACTIVE;\n";
      if (_config.debug_mode()) {
        _o << "case JitBreakPtCode: {\n"
           << "addr_t ea = " << _nia->name() << "_read().uint64();\n"
           << getMemPointsIterator(false) << " i = _breakpoints.find(ea);\n"
           << "assert(i != _breakpoints.end());\n"
           << "issueBreakpoint(ea,i);\n"
           << "} break;\n";
      }
      _o << "default: ;\n"
         << "}\n"
         << "++_dc_escapes;\n"
         << "}\n";

      writeExceptionCheck(true,true,true,true,false);

      _o << "}\n";

      writeDmiDisable();

      _o << "return HALT;\n"
         << "}\n\n"

         << "ProgramStatus run_dc()\n"
         << "{\n";
      if (_config.debug_mode()) {
        _o << "try {\n";
      }
      _o << "return simple_run_dc();\n";
      if (_config.debug_mode()) {
        _o << "}\n"
           << "catch (SimInterrupt) {\n";
        _o << "return BREAK;\n"
           << "}\n";
      }
      _o << "}\n"
         << endHeaderOnly();
    }

    // For a JIT model, this creates the translate function which translates
    // target code into native instruction handlers.
    void generateJitTranslate()
    {
      const InstrTables &tables = _core.instrtables();

      writeICacheTouchSetupFunc(JitTrans);

      _o << headerOnly2()
         << "\naddr_t jit_translate_instr(addr_t &pc,MyJitBasicBlock *jb,unsigned count,bool extra_fetch)\n"
         << "{\n";

      // Create the decode tree.
      JitCodeGen codegen(_o,*this);

      // If we have an MMU, then insert logic to break up BBs if they cross a
      // page.
      // if (_core.mmu()) {
      //   _o << getInstrPageUpdateVar() << " = false;\n";
      // }
    
      _o << _iname << "[0] = instr_read32_nocache(pc);\n";

      // if (_core.mmu()) {
      //   // Stop translating if we do cross a page.
      //   _o << "if (" << getInstrPageUpdateVar() << ") return pc;\n";
      // }
    
      _o << "jb->set_end_ra(_last_instr_ra);\n";

      writeICacheTouchLogic(JitTrans);

      _o << "jb->incr_dc_count();\n";

      writeDecodeTables(tables, codegen, true);

      _o << "return true;\n"
         << "}\n"

        // The translate instruction translates a basic block of instructions,
        // i.e. a sequence of instructions with a single entry point (the start)
        // and a single exit point- either a branch, halt, or whatever.  This
        // encodes to decode items.  LLVM encoding is then only done for really
        // hot blocks.
         << "void translate_jit(addr_t addr,MyJitBasicBlock *bb)\n{\n"
         << "++_dc_compiles;\n"
         << "addr_t new_addr = addr;\n"
         << "unsigned count = 0;\n"
         << "_jit_decode_cache.start_bb(bb);\n"
         << "if (check_for_breakpoint(addr)) {\n"
         << "bb->set_brkpt(true);\n"
         << "}\n";
      if (haveExceptions()) {
        _o << "try {\n";
      }      
      _o << "while (jit_translate_instr(new_addr,bb,count,true)) {\n"
         << "count++;\n";
      if (_config.debug_mode()) {
        // If we encounter a breakpoint, then we simply finish the bb so that it
        // returns with a breakpoint code.  This is the normal case- we find a
        // breakpoint as we're translating a block.  Note that we check the *next*
        // address, so that it matches the behavior of the interpreter, where we
        // break when the NIA matches, at the bottom of the loop.
        _o << "if (check_for_breakpoint(new_addr)) {\n"
           << "bb->set_end_ra(instrReadTranslate(new_addr));\n"
           << "_jit_decode_cache.add_brkpt_stub(bb);\n"
           << "return;\n"
           << "}\n";
      }
      // If we exceed our maximum bb size, then just record the continuation,
      // since we don't do direct threading with the JIT decode-cache.
      _o << "if ( (count >= TranslationParms::max_bb_instr_count())";
      // if (_core.mmu()) {
      //   _o << " || (count > 1 && " << getInstrPageUpdateVar() << ")";
      // }
      _o << ") {\n"
         << "_jit_decode_cache.add_br_stub(bb);\n"
         << "bb->set_bt_t(_jit_bb_hash.get_bb_or_stub(new_addr,0,_jit));\n"
         << "++count;\n"
         << "break;\n"
         << "}\n"
         << "}\n";
      if (haveExceptions()) {
        _o << "}\n"
           << "catch (Exception current_exception) {\n"
           << "if (count == 0) throw;\n"
           << "}\n";
      }
      _o << "}\n\n";

      // Miscellaneous JIT support routines.
      genJitSupport();
       
      _o << "ProgramStatus simple_run_jit()\n"
         << "{\n";

      writePostSim(post_sim(),_class_name);
      writeDmiEnable();

      _o << "MyJitBasicBlock *jb = 0;\n"
         << "JitTranslate jt;\n"
         << "// The sim kernel runs until the core is inactive.  Translation occurs as a side-effect of lookup.\n"
         <<" // A null-return on a block lookup means that we got a cold-block and it was interpreted, so just loop\n"
         << "// and continue.\n";

      _o << "while (_active) {\n";

      if (haveExceptions()) {
        _o << "try {\n";
      } else {
        _o << "{\n";
      }

      add_td_check(_o,"","return ACTIVE;","1",false,false,true,false);

      _o << "addr_t nia = " << _nia->name() << "_read().uint64();\n";
      if (haveExceptions()) {
        // We have to guard against instruction-fetch exceptions.  We need to
        // adjust the program counter, to mirror the fact that this would
        // normally have been done at the top of the simulation loop for a
        // non-jit model.
        _o << "try {\n";
      }
      // We're checking about the dc-hot-threshold b/c we'll either run with the
      // decode-cache or JIT.
      _o << "jb = _jit_bb_hash.get_bb(jt,nia,0,TranslationParms::dc_bb_hot_threshold());\n";
      if (haveExceptions()) {
        _o << "}\n"
           << "catch (Exception current_exception) {\n"
           << writeJitFetchUpdateCode()
           << "throw;\n"
           << "}\n";
      }
      _o << "unsigned code;\n"
         << "if (!jb) {\n"
         << "exec_cold_block();\n";
      add_td_check(_o,"","return ACTIVE;","",false,false,true,false);
      _o << "continue;\n"
         << "} else if (jb->is_compiled()) {\n"
         << "code = jb->exec();\n"
         << "} else {\n"
        // Here's where we check agains the jit hot-threshold and compile it if
        // relevant.  We enqueue it, then proceed with byte-code execution, since
        // the thread will compile it in the background.
         << "if (TranslationParms::mt_jit_compile()) {\n"
         << "if ( (jb->hot_count() >= TranslationParms::jit_bb_hot_threshold() && !jb->being_compiled()) ) {\n"
         << "_jc.launch_compile(jb);\n"
         << "}\n"
         << "code = exec_jit_dc_block(jb);\n"
         << "} else {\n"
         << "if (  (jb->hot_count() >= TranslationParms::jit_bb_hot_threshold()) ) {\n"
         << "_jc.compile_bb(jb);\n"
         << "code = jb->exec();\n"
         << "} else {\n"
         << "code = exec_jit_dc_block(jb);\n"
         << "}\n"
         << "}\n"
         << "}\n"
         << "switch (code) {\n"
         << "case JitNopCode:\n"
        // Check for any breakpoints which have occurred due to calculated
        // branches and such.
         << getCommitCheckName() << "();\n"
         << "break;\n"
         << "case JitRetCode:\n";
      writeExceptionCheck(true,false,false,false,false);
      _o << getCommitCheckName() << "();\n"
         << "if (InstrMax && (_instr_count > InstrMax)) {\n"
         << "halt();\n"
         << "return HALT;\n"
         << "}\n"
         << "return ACTIVE;\n";
      if (_config.debug_mode()) {
        _o << "case JitBreakPtCode: {\n"
           << "addr_t ea = " << _nia->name() << "_read().uint64();\n"
           << getMemPointsIterator(false) << " i = _breakpoints.find(ea);\n"
           << "assert(i != _breakpoints.end());\n"
           << "issueBreakpoint(ea,i);\n"
           << "} break;\n";
      }
      _o << "default: ;\n"
         << "}\n"
         << "++_jit_escapes;\n"
         << "}\n";

      writeExceptionCheck(true,true,true,true,false);

      _o << "}\n";

      writeDmiDisable();

      _o << "return HALT;\n"
         << "}\n\n"

         << "ProgramStatus run_jit()\n"
         << "{\n";
      if (_config.debug_mode()) {
        _o << "try {\n";
      }
      _o << "return simple_run_jit();\n";
      if (_config.debug_mode()) {
        _o << "}\n"
           << "catch (SimInterrupt) {\n"
           << "return BREAK;\n"
           << "}\n";
      }
      _o << "}\n";

      // This executes a single byte-compiled basic block.  We don't worry about
      // direct-threading for JIT byte-compiled functions, so we only have a
      // single nested set of dispatch statements.
      _o << "JitReturnCodes exec_jit_dc_block(MyJitBasicBlock *bb)\n{\n"
         << "++_jit_dc_runs;\n";

      writeDcKernel(_o,"Jit","_jit",20,1);

      _o << "return (JitReturnCodes)((long)dcf);\n"
         << "}\n\n"

        // Used by JIT logging functions.
         << endHeaderOnly();
    }

    void generateSimStats()
    {
      _o << "unsigned get_sim_stat(const std::string &parm) const\n"
         << headerOnly()
         << "{\n";

      if (jit_support() || dc_support()) {
        _o << "if (parm == \"dc_compiles\") {\n"
           << "return _dc_compiles;\n"
           << "}\n"         
           << "if (parm == \"cold_runs\") {\n"
           << "return _cold_runs;\n"
           << "}\n";
      }

      if (jit_support()) {
        _o << "if (parm == \"jit_escapes\") {\n"
           << "return _jit_escapes;\n"
           << "}\n"
           << "if (parm == \"jit_dc_runs\") {\n"
           << "return _jit_dc_runs;\n"
           << "}\n"         
           << "if (parm == \"jit_compiles\") {\n"
           << "return _jit.jit_compiles();\n"
           << "}\n"         
           << "if (parm == \"jit_bb_count\") {\n"
           << "return _jit_bb_hash.size();\n"
           << "}\n"
           << "if (parm == \"jit_thread_count\") {\n"
           << "return TranslationParms::mt_jit_compile();\n"
           << "}\n";
      }

      if (dc_support()) {
        _o << "if (parm == \"dc_escapes\") {\n"
           << "return _dc_escapes;\n"
           << "}\n"
           << "if (parm == \"dc_bb_count\") {\n"
           << "return _dc_bb_hash.size();\n"
           << "}\n";
      }

      _o << "return IssCore::get_sim_stat(parm);\n"
         << "}\n"
         << endHeaderOnly();
    }

    void generateInstrs()      
    {
      if (!sim_mode()) {
        return;
      }

      if (transactional_iss()) {
        generateTransSim();
      } else {
        generateSim();
      }

      generateCommonTranslationCode();
      if (jit_support()) {
        generateJitTranslate();
      }
      if (dc_support()) {
        generateDcTranslate();
      }

      generateSimStats();
      generateDisasm();
    }

    void generateSimSupportFuncs()
    {
      if (!sim_mode()) {
        return;
      }

      // Write tracing hooks, if they exist.
      genFunc("void tracingOn",_core.env(),tracing_on());
      genFunc("void tracingOff",_core.env(),tracing_off());
      genFunc("void postSim",_core.env(),post_sim());
      genFunc("uint64_t complTime",_core.env(),compl_time());

      // Create post-fetch handler, if necessary.
      genPostFetchHandler(_o);

      // Create post-exec handler, if necessary.
      if (transactional_iss()) {
        
        _o << "BranchInfoType default_branch_info(" << PktType << " *) {\n"
           << "return std::make_pair(BranchNone,false);\n"
           << "}\n\n";

        if (_core.has_delayed_writes()) {
          _o << "int default_delay_info(" << PktType << " *,unsigned) {\n"
             << "return -1;\n"
             << "}\n\n";
        }

        _walker.set_slots( &_genip.post_exec_slots() );
      }
      genFunc("void post_exec_handler",_core.env(),_core.post_exec_handler()._func);
      _walker.set_slots(0);

      // If we have a branch-taken handler, generate that.
      if (_core.branch_taken_handler()) {
        if (transactional_iss()) {
          _walker.set_slots( &_genip.branch_taken_slots() );        
        }
        _o << "// Branch-taken handler function.\n";
        genFunc("void " + getBrTakenHandlerName(),_core.env(),_core.branch_taken_handler()._func);
        _walker.set_slots(0);
      }

      // If a hybrid-iss, then generate branch-taken target-update function.
      if (hybrid_iss()) {
        if (_coreconfig.virtual_mem_interface()) {
          _o << "virtual void " << getBrTrgUpdateName() << " (addr_t target,unsigned size) {};\n\n";
        } else {
          _o << "void " << getBrTrgUpdateName() << " (addr_t target,unsigned size);\n\n";
        }
      }
      if (_config.add_branch_hook()) {
        if (_coreconfig.virtual_mem_interface()) {
          _o << "virtual void " << getBrHookName() << " () {};\n\n";
        } else {
          _o << "void " << getBrHookName() << " ();\n\n";
        }
      }

      // Create pre and post cycle routines, if necessary.
      genFunc("void pre_cycle_handler",_core.env(),_core.pre_cycle_handler()._func);
      genFunc("void post_cycle_handler",_core.env(),_core.post_cycle_handler()._func);

      // If we have a clock-cycle ratio, then create the counter variable and
      // the various reset and init code here.
      if (have_clock_cycle_ratio()) {
        gc_string cc = getCycleTally();
        _cd._member_vars.push_back("double "+cc);
        _cd._reset_stmts.push_back(cc+" = 0;");
        _cd._constr_inits.push_back(cc+"(0)");
        
      }     

      // For JIT stuff, if we have breakpoint support, add in the
      // breakpoint-mode flag.  This must be done for pre-phase and post-phase,
      // since we must have the same object size for code to work correctly.
      if (jit_support() && _config.debug_mode()) {
        _cd._member_vars.push_back("bool _breakpoint_mode");
        _cd._constr_inits.push_back("_breakpoint_mode(false)");
        _cd._reset_stmts.push_back("_breakpoint_mode = false");
      }

      // create a post packet handler if defined
      if (_core.post_packet_handler()) {
        genFunc("void post_packet_handler",_core.env(),_core.post_packet_handler()._func);
      }

      // create a post packet commit handler if defined
      if (_core.post_packet_commit_handler()) {
        genFunc("void post_packet_commit_handler",_core.env(),_core.post_packet_commit_handler()._func);
      }

      // Create retry handler function
      if (Ptree *decode_retry = _core.decode_retry_handler()._func) {
        genFunc("bool decode_retry_handler",_core.env(),decode_retry);
      }

      // Create miss-handler function.
      if (Ptree *decode_miss = _core.decode_miss_handler()._func) {
        genFunc("void decode_miss_handler",_core.env(),decode_miss,false,true);
      } else {
        _o << "void decode_miss_handler(addr_t cia,unsigned instr) ATTRIBUTE_USED\n{\n"
           << "unknownInstruction(cia,instr);\n"
           << "}\n\n";
      }

      if (mt_support()) {
        _cd._member_vars.push_back("PrivateMutexHash " + getMemLocks());

        _cd._member_vars.push_back("bool " + getAtomicFlag());
        _cd._constr_inits.push_back(getAtomicFlag() + "(false)");
        _cd._reset_stmts.push_back(getAtomicFlag() + " = false");

        if (rw_locked_mem()) {
          _cd._member_vars.push_back("mutable boost::mutex " + getMemMutex());
        }

        _o << "bool hasMultiThreadedSupport() const { return true; };\n\n"
           << "void setMemoryLocks(LockedMutexHash *ml) { " << getMemLocks() << ".set_global_hash(ml); };\n\n";
      }

      writeActiveCountFuncs();

      writeJitExecHandlers();
      
      writeLastInstrAccessor();

      writePrefetchBufferAccessors();

      writeContextUpdates();

      writeCommitChecks();
    
      writeCheckForBreakPoints();
      
      writeTimeTagLogic();
    }

    void generateSysCallSupport()
    {
      if (syscall_enabled()) {  
        // We create the external system call interface here.  This redirects all
        // calls to the installed object./ Otherwise, create any necessary helper
        // functions.
        gc_string sct = "SysCallExtHandler";
        gc_string sch = "_schandler";

        _cd._member_vars.push_back(sct + " *" + sch);
        _cd._constr_inits.push_back(sch + "(0)");
    
        _o << "bool hasExternalSysCalls() const { return true; }\n\n"

           << "void setSysCallHandler(" << sct << " *s) {\n" 
           << sch << " = s;\n"
           << "}\n\n"

           << sct << " *getSysCallHandler() const { return " << sch << "; }\n\n"

           << "bool syscall_enabled() { return " << sch << "->syscall_enabled(); };\n\n"

           << "void syscall_trigger(uint32_t syscode) { " << sch << "->syscall_trigger(*this,syscode); };\n\n"
           << "template <size_t Nb>\n"
           << "void syscall_trigger(const intbv<Nb> &x) { " << sch << "->syscall_trigger(*this,x.uint32()); };\n\n"
      
           << "void syscall_add_arg(uint64_t a) { " << sch <<"->syscall_add_arg(a); };\n\n"
           << "template <size_t Nb>\n"
           << "void syscall_add_arg(const intbv<Nb> &x) { " << sch << "->syscall_add_arg(x.uint64()); };\n\n"
      
           << "int64_t syscall_return_code() { return " << sch << "->syscall_return_code(); };\n\n"
           << "uint32_t syscall_errno() { return " << sch << "->syscall_errno(); };\n\n";

      } else {
        _o << "bool hasExternalSysCalls() const { return false; }\n\n"

           << "bool syscall_enabled() { return false; };\n\n"

           << "void syscall_trigger(uint32_t syscode) {};\n\n"
           << "template <size_t Nb>\n"
           << "void syscall_trigger(const intbv<Nb> &x) {};\n\n"
      
           << "void syscall_add_arg(uint64_t a) {};\n\n"
           << "template <size_t Nb>\n"
           << "void syscall_add_arg(const intbv<Nb> &x) {};\n\n"
      
           << "int64_t syscall_return_code() { return 0; };\n\n"
           << "uint32_t syscall_errno() { return 0; };\n\n";
      }
    }

    // Generate a single helper.  If the for_instr flag is true, then generate for an instruction, which means
    // that in transactional-iss mode, add slot index information.
    // Returns the number of slots used by the function, if relevant, or -1.
    int generateHelper(const gc_string &fname,Ptree *func,bool for_instr)
    {
      int num_slots = -1;
      writeLineDirective(_core.env(),func);
      bool is_trans = false;
      if (for_instr && transactional_iss()) {
        IfFind(x,_genip.helper_slots(),fname) {
          const SlotAlloc &s = x->second;
          // If we're a transactional ISS and the helper has recorded slots, then
          // use them in the function transformation.  Otherwise, we write the
          // function as non-transactional, assuming that this means that the
          // function is just not used by any instruction.
          _walker.set_slots( &s );
          _walker.set_hybrid_iss( hybrid_iss() );
          _walker.set_track_reads( track_reads() );
          num_slots = s._items.size();
          if (num_slots) {
            is_trans = true;
          }
        }
      }
      // If this function is used for disassembly, then we have to make it static.
      if (!is_trans && _core.disasm_helpers().count(fname)) {
        _o << "static ";
      }
      Ptree *h2 = addUsedAttr(_walker.translateCode(_core.env(),func));
      _o << h2 << "\n";
      _walker.set_slots(0);
      writeCurLocation();
      return num_slots;
    }

    Ptree *addUsedAttr(Ptree *f)
    {
      if (f->IsA(ntDeclaration)) {
        Ptree* decls = PtreeUtil::Third(f);
        if (decls->IsA(ntDeclarator)) {
          return PtreeUtil::List(First(f),Second(f),decls,Ptree::Make("ATTRIBUTE_USED"),Nth(f,3));
        }
      }
      return f;
    }

    Ptree *checkFunc(StrSet &extra_helpers,Ptree *func,const char *msg)
    {
      try {
        UsageInfo ui;
        Ptree *new_func = _core.check_func(ui,func);
        extra_helpers.add(ui._helpers);
        return new_func;
      }
      catch (parse_error &pe) {
        RError("Error parsing " << msg << ":  " << pe.what());
      }
    }

    // Checks some external hooks for their helper usage, in case we're omitting
    // unused helpers.
    void checkForExtraHelpers(StrSet &extra_helpers)
    {
      _coreconfig.set_tracing_on(checkFunc(extra_helpers,tracing_on(),"tracing-on hook"));
      _coreconfig.set_tracing_off(checkFunc(extra_helpers,tracing_off(),"tracing-off hook"));
      _coreconfig.set_post_sim(checkFunc(extra_helpers,post_sim(),"post-simulation hook"));
      _coreconfig.set_compl_time(checkFunc(extra_helpers,compl_time(),"completion-time hook"));
    }

    void removeUnnecessaryAuxFuncs()
    {
      Core &core = const_cast<Core &>(_core);

      // Bit of a hack here- if we aren't time-tagged, we don't want any of the
      // allow-issue functions.
      if (!time_tagged()) {
        ForEach(_coreconfig.groups(),i) {
          core.del_aux_func(i->second.allow_issue()._d);
        }
      }
    }

    // Generate helper code for a core.
    void generateHelpers()
    {
      bool incl_unused = _config.incl_unused_helpers();
      StrSet extra_helpers;

      checkForExtraHelpers(extra_helpers);

      ForEach(_core.type_decls(),i) {
        _o << _walker.translateCode(_core.env(),*i) << ";\n";
      }
      // Spit out helper code here.
      _o << "//\n"
         << "// Helper code.\n"
         << "//\n\n";

      const HelperMap &helpers = _core.helpers();

      ForEach (helpers,i) {
        const gc_string &fname = i->first;

        // Skip if it's an assembly helper.
        if (_core.asm_helpers().count(fname)) continue;

        // If we're not including unused helpers, then check and omit them here.
        // We include a comment to aid with debugging.
        if (!incl_unused && (_core.used_helpers().count(fname) == 0) && (extra_helpers.count(fname) == 0)) {
          _o << "//\n"
             << "// Omitting unused helper function " << fname << "\n"
             << "//\n\n";
          continue;
        }

        // The first version is for general use.
        int num_slots = generateHelper(fname,i->second._func,true);

        // If we're generating a transactional ISS, and this function is used by
        // non-instructions as well as instructions, then generate a second copy
        // without slot indices.  We only do this if the function had any slots
        // in the first place.
        if (num_slots > 0 && transactional_iss() && _core.instr_helpers().count(fname) && _core.non_instr_helpers().count(fname)) {
          generateHelper(fname,i->second._func,false);
        }
      }

      _o << "\n";

      removeUnnecessaryAuxFuncs();

      // Spit out auxiliary functions.
      _o << "//\n"
         << "// Auxiliary functions.\n"
         << "//\n\n";

      const AuxFuncList &aux_funcs = _core.aux_funcs();

      ForEach (aux_funcs,i) {
        const AuxFunc &afunc = **i;

        if (!afunc.comment().empty()) {
          _o << "// " << filter_string(afunc.comment(),"\n// ") << "\n";
        }

        // Do we have any arguments to convert to constant references?  If so, then do that first.
        Ptree *func = afunc.func();
        if (!afunc.arg_mods().empty()) {
          Ptree *orig_args = get_func_args(func);
          Ptree *new_args  = transform_args(func,afunc.arg_mods(),0);
          func = Subst(new_args,orig_args,func);
        }

        genFunc(afunc.rtype() + " " + afunc.name()->ToString(),_core.env(),func,afunc.isConst());
      }

      _walker.set_memory_suffix_default();
    }

 
    void write_additional_fetch(ostream &o,unsigned start, unsigned end, bool prefix,
                                CodeGenIF &codegen,bool extra_fetch)
    {
      if (start >= end || !extra_fetch ) {
        return;
      }

      gc_string addr = codegen.get_fetch_addr();
      unsigned pb = _core.prefix_bits();
      gc_string instr_read = codegen.get_instr_read();

      if (!prefix && (pb > 0)) {
        gc_string tmp = AdlGenSym()->ToString();
        o << "uint32_t " << tmp << ";\n";

        for(unsigned i = start; i < end; ++i) {
          if (transactional_iss() && !logging_mem()) {
            o << "_memory_request.set_result(0,_memory_request._page1, 4*" << i << ",4*" << i << ");\n";
          }
          if (!transactional_iss()) {
            o << "if (extra_fetch) {\n";
          }
          o << tmp << " = " << instr_read <<  "( " << addr << " + 4*" << i << codegen.extra_fetch_parms() << ");\n";
          if (!transactional_iss()) {
            o << "}\n";
          }
          o << codegen.fetch_name() << "[" << i << "] |= (" << tmp << " >> " << dec << pb << ");\n"
            << codegen.fetch_name() << "[" << i+1<< "] = (" << tmp << " << " << 32-pb << ");\n";
        }

      } else {
        for (unsigned i = start; i < end; ++i) {
          if (transactional_iss() && !logging_mem()) {
            o << "_memory_request.set_result(0,_memory_request._page1, 4*" << i << ",4*" << i << ");\n";
          }
          if (!transactional_iss()) {
            o << "if (extra_fetch) {\n";
          }
          o << codegen.fetch_name() << "[" << i << "] = " << instr_read <<  "( " 
            << addr << " + 4*" << i << codegen.extra_fetch_parms() << ");\n";
          if (!transactional_iss()) {
            o << "}\n";
          }
        }
      }      
    }
    
    uint32_t get_submask(RNumber mask) {
      uint32_t ret = 0;
      while(mask > 0) {
        if ((ret = mask.uint32()) > 0) {
          return ret;
        }
        mask >>= 32;
      }
      return ret;
    }
    
    unsigned get_index(RNumber mask) {
      unsigned ret = 0;
      unsigned sz  = mask.size()/32  - 1;
      while(mask > 0) {
        if (mask.uint32() > 0) {
          return (sz - ret);
        }
        mask >>= 32;
        ret++;
      }
      return ret; 
    
    }
  };

  void writeSupportFuncs(opfxstream &o,ClassData &cd,const ConfigDB &config,bool is_core,
                         bool has_post_sim,const gc_string &class_name)
  {
    GenInstrBase gi(o,cd,config);

    gi.writeSupportFuncs(is_core,has_post_sim,class_name);
  }

  GenInstrsIF::GenInstrsIF(opfxstream &o,PfxFileArray *impls,opfxstream *jo,const Core &core,InstrWalker &walker,
                           const gc_string &filename,Watches &watches,ClassData &cd,
                           GenInstrPacketIF &genip,StrList &end_code,InstrSyms &instr_syms,const Path &path,
                           const ConfigDB &config,const CoreConfig &coreconfig) :
    _impl(new GenInstrs(o,impls,jo,core,walker,filename,watches,cd,genip,end_code,instr_syms,path,config,coreconfig))
  {
  }

  ImplCall(GenInstrsIF,handlerSetup);

  ImplCall(GenInstrsIF,generateInstrAttrs);

  ImplCall(GenInstrsIF,generateInstrTables);

  ImplCall(GenInstrsIF,generateSubInstrs);

  ImplCall(GenInstrsIF,generateInstrHandlers);

  ImplCall(GenInstrsIF,generateSimSupportFuncs);

  ImplCall(GenInstrsIF,generateSysCallSupport);

  ImplCall(GenInstrsIF,generateInstrs);

  ImplCall(GenInstrsIF,generateHelpers);

  ImplCall(GenInstrsIF,generateHalt);

  ImplCall(GenInstrsIF,generateJitFwdDecls);

}

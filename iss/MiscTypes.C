//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Miscellaneous types used by the ISS generator.
//

#include <iostream>
#include <iomanip>
#include <sstream>

#include "opencxx/parser/Ptree.h"
#include "opencxx/Environment.h"

#include "fe/Data.h"
#include "fe/Helpers.h"

#include "helpers/Macros.h"
#include "helpers/pfxstream.h"
#include "helpers/CodeGen.h"

#include "ConfigDB.h"
#include "MiscTypes.h"
#include "InstrWalker.h"

using namespace std;
using namespace Opencxx;
using namespace PtreeUtil;

namespace adl {

  gc_string genFieldType(const GenericField &gf)
  {
    if (gf.bits()) {
      gc_ostringstream ss;
      ss << "bits<" << gf.bits() << ">";
      return ss.str();
    } else {
      return "int32_t";
    }
  }

  gc_string genFieldIntGet(const GenericField &gf)
  {
    if (gf.bits()) {
      return gf.name() + ".uint64()";
    } else {
      return gf.name();
    }
  }

  gc_string genFieldInit(const GenericField &gf)
  {
    gc_ostringstream ss;
    bool use_string = gf.int_reset() && (gf.resetvalue()->GetLength() > 18);
    if (gf.bits()) {
      ss << "bits<" << gf.bits() << ">(" 
         << (use_string ? "\"" : "") 
         << gf.resetvalue() 
         << (use_string ? "\"" : "") << ")";
    } else {
      ss << gf.resetvalue();
    }
    return ss.str();
  }


  void writeCurLocation(opfxstream &o,const ConfigDB &config,const gc_string &filename)
  {
    if (config.ld_okay()) {
      o << flush
        << "# " << o.linecount()+1 << " \"" << filename << "\"\n";
    }
  }

  void writeLineDirective(opfxstream &o,const ConfigDB &config,Environment *env,Ptree *expr)
  {
    if (config.ld_okay()) {
      lineDirective(o,env,expr);
    }
  }

  // Gets the function arguments.
  Ptree *funcArgs(Ptree *f)
  {
    return get_func_args(f);
  }

  // Gets the function's body after translating it.  Always use this function,
  // rather than calling the walker directly, because there's a subtlety of
  // usage here: You must translate the entire function so that the functions
  // arguments are properly recorded.  This is required in order to translate
  // bits objects correctly when slices are taken: If the arguments are not
  // recorded, then we don't know their types.
  Ptree *funcBody(InstrWalker &walker,Environment *e,Ptree *f)
  {
    return get_func_body(walker.translateCode(e,f));
  }

  // Writes out a function with a given name.  Note:  The return type should be specified within the
  // fname argument.
  void genFunc(opfxstream &o,const ConfigDB &config,InstrWalker &walker,const gc_string &filename,
               const gc_string &fname,Environment *env,Ptree *func,bool isConst,bool isUsed)
  {
    if (func) {
      o << fname << "(" << funcArgs(func) << ")";
      if (isConst) o << " const";
      if (isUsed)  o << " ATTRIBUTE_USED";
      o << "\n";
      writeLineDirective(o,config,env,func);
      o << funcBody(walker,env,func) << "\n";
      writeCurLocation(o,config,filename);
      o << "\n";
    }
  }

 
  gc_string getParmType(const Parm &p)
  {
    return (p.is_bool()) ? "bool" : (p.name() + "_t");
  }

  gc_string getParmName(const Parm &p)
  {
    return p.name();
  }

  gc_string getParmHandleName(const Parm &p)
  {
    return p.name() + "_jp";
  }

  gc_string getParmAssign(const Parm &p)
  {
    return p.name() + "_write";
  }

  gc_string getMemHandlerName(const gc_string &name)
  {
    return "_memhandler_" + name;
  }

  gc_string getJitCoreGlobal(const Path &p)
  {
    return getPathStr(p,"_")+"_core";
  }

  gc_string getJitMemSuffix()
  {
    return "_jit";
  }

  gc_string getJitBranchTarget()
  {
    return "_adl_branch_trg";
  }

  gc_string getJitInstrPrefix()
  {
    return "_adl_instr__";
  }

  gc_string getJitPcValue()
  {
    return "_adl_pc_value";
  }

  gc_string getJitInstrValue()
  {
    return "_adl_instr_value";
  }

  bool genICacheTouch(const Core &core,const ConfigDB &config)
  {
    if (core.has_caches(InstrCache) && (config.icache_touch())) return true;

    if (config.icache_touch() == tTrue) return true;

    return false;
  }

  gc_string getJitICacheTouch()
  {
    return "jit_icache_touch";
  }

  gc_string getJitLastRaMember()
  {
    return "_last_instr_ra";
  }

  gc_string getJitTraceVar()
  {
    return "GlobalJitTraceMode";
  }

  gc_string getJitBBArg()
  {
    return "bb_arg";
  }

  gc_string getJitBBIndexArg()
  {
    return "bb_index_arg";
  }

  gc_string getDcInstrPrefix(TMode tmode)
  {
    switch (tmode) {
    case JitTrans:
      return "jit_decode_cache_instr_";
    case DcTransFixed:
      return "dc_decode_cache_fixed_instr_";      
    default:
      return "dc_decode_cache_instr_";
    }
  }

  gc_string getInstrPageUpdateVar()
  {
    return "_instr_page_update";
  }

  gc_string getCommitQueueName()
  {
    return "_commitQueue";
  }

  gc_string getRestoreStackName()
  {
    return "_restoreStack";
  }

  gc_string getRollbackEnableName()
  {
    return "_rollback_enable";
  }

  gc_string getIgnoreEnableName()
  {
    return "_ignore_enable";
  }

  gc_string getInstrAttrType(const Core &core)
  {
    return (core.instr_attrs().size() > 32) ? "uint64_t" : "uint32_t";
  }

  gc_string getInstrAttrName(const gc_string &n)
  {
    return "__instr_attr_" + n;
  }

  gc_string getInstrAttrName(unsigned id)
  {
    return getInstrAttrName(Data::get_attr_name(id));
  }

  gc_string getInstrAttrVar()
  {
    return "__instr_attr";
  }

  gc_string getInstrAttrGetter()
  {
    return "getInstrAttrs";
  }

  gc_string getMmuInstrAttrGetter()
  {
    return "getMmuInstrAttrs";
  }

  gc_string getRegFileAttrType(const Core &core)
  {
    return (core.regfile_attrs().size() > 32) ? "uint64_t" : "uint32_t";
  }

  gc_string getRegFileAttrName(const gc_string &n)
  {
    return "__regfile_attr_" + n;
  }

  gc_string getRegFileAttrName(unsigned id)
  {
    return getRegFileAttrName(Data::get_attr_name(id));
  }

  gc_string getDmiCacheName(const gc_string &memname,const gc_string &type)
  {
    return "_dmi_" + memname + "_" + type;
  }

  gc_string getDmiCacheUpdateName(const gc_string &memname,const gc_string &type)
  {
    return "update_dmi_" + memname + "_" + type;
  }

  gc_string getDmiCacheInvalidateName(const gc_string &memname,const gc_string &type)
  {
    return "invalidate_dmi_" + memname + "_" + type;
  }

  gc_string getDmiCacheInvalidateName(const gc_string &memname)
  {
    return "invalidate_dmi_" + memname;
  }

  gc_string getDmiReservedCheck(const gc_string &type)
  {
    return "check_dmi_" + type + "_reserved";
  }

  gc_string getDmiCacheEnable()
  {
    return "enable_dmi_caches";
  }

  gc_string getDmiCacheDisable()
  {
    return "disable_dmi_caches";
  }

  gc_string getEventBusRecvType()
  {
    return "EventBusReceiver";
  }

  gc_string getEventBusRecvName()
  {
    return "eventBusReceiver";
  }

  gc_string getEventBusMutexType()
  {
    return "boost::mutex";
  }

  gc_string getEventBusMutexName()
  {
    return "_eb_lock";
  }

  gc_string getMemLocks()
  {
    return "_mem_locks";
  }

  gc_string getAtomicFlag()
  {
    return "_atomic_instr_flag";
  }

  gc_string getMemMutex()
  {
    return "_mem_mutex";
  }

  gc_string getActiveCallback()
  {
    return "_active_callback";
  }

  gc_string getNiaSfx()
  {
    return "_simple";
  }

  gc_string getBrTakenHandlerName()
  {
    return "branch_taken_handler";
  }

  gc_string getBrUserHookName()
  {
    return "branch_user_handler";
  }

  gc_string getBrHookName()
  {
    return "branch_handler";
  }

  gc_string getTransType(AccessType type)
  {
    switch (type) {
    case aInstr:
      return "InstrTrans";
    case aLoad:
      return "LoadTrans";
    case aStore:
      return "StoreTrans";
    }
    assert(0);
  }

  gc_string getAddrCheckHandler()
  {
    return "addr_check_handler";
  }

  gc_string getAddrCheckCall(const Core &core,const gc_string &ea,const gc_string &ra,const gc_string &pfx,
                             const gc_string &tlb,const gc_string &cond_tlb,AccessType type)
  {
    if (core.addr_check_handler()) {
      int arity = get_arity(core.addr_check_handler()._func);
      gc_string trans_type = getTransType(type);
      gc_string call = pfx + getAddrCheckHandler() + "(" + ea + "," + ra + "," + trans_type;
      if (arity > 3) {
        if (tlb.empty()) {
          // We want the extra parameters, but haven't been given info on the
          // tlb object, so we just supply invalid values.
          call += ",-1,-1,-1";
        } else {
          // Otherwise, get id, set, way from the tlb item.
          if (cond_tlb.empty()) {
            call += "," + tlb + ".id()," + tlb + ".set()," + tlb + ".way()";
          } else {
            call += ",((" + cond_tlb + ") ? " + tlb + ".id() : -1),((" + cond_tlb + ") ? " 
              + tlb + ".set() : -1),((" + cond_tlb + ") ? " + tlb + ".way() : -1)";
          }
        }
      }
      call += ");\n";
      return call;
    } else {
      return gc_string();
    }
  }

  // We start the IDs at 1 so that 0 is an invalid value.  This way, atoi's
  // return must be valid or else we won't get a proper symbol.
  unsigned InstrCoreSyms::_next_id = 1;

  void InstrSym::init()
  {
    _ewrap = 0;
    _exec = AdlGenSym();
    _alt_exec = 0;
    _disasm = AdlGenSym();
    _trans = AdlGenSym();
    _jit_trans = AdlGenSym();
    _dc_trans = AdlGenSym();
    _logarg = AdlGenSym();
    _leafwrap = 0;
    _pc_rel_instr = false;
    _signed = false;
    _shift = 0;
    _offset = 0;
    _brType = brNone;
    _sbType = sbNone;
    _br_op = 0;
    _halts = false;
    _uses_eb_res = false;
  }


  InstrSyms::InstrSyms()
  {
    _id_start = InstrCoreSyms::_next_id;

    _decode_miss._id = InstrCoreSyms::_next_id++;

    _td_check._id = InstrCoreSyms::_next_id++;

    _icache_touch._id = InstrCoreSyms::_next_id++;

    // Start of instruction IDs.
    _iid_start = InstrCoreSyms::_next_id;
  }

  // Generate a friendly name from the instruction's name.  This must be a valid identifier.
  Ptree *makeFriendlyName(const gc_string &pfx,const Instr &instr)
  {
    gc_string n = makeFriendlyName(instr.name());
    return Ptree::Make("%s_%s",pfx.c_str(),n.c_str());
  }

  void InstrSyms::add(const Instr &instr,bool valid,bool tracing_on,bool has_modified_fields)
  {
    auto ip = insert(make_pair(instr.name(),InstrSym((valid) ? InstrCoreSyms::get_next_id() : -1,
                                                  (valid && tracing_on) ? InstrCoreSyms::get_next_id() : -1,
                                                  (valid && tracing_on) ? InstrCoreSyms::get_next_id() : -1,
                                                  &instr)));
    
    InstrSym &sym = ip.first->second;
    sym._disasm = makeFriendlyName("instr_disasm",instr);
    if (has_modified_fields) {
      sym._ewrap = makeFriendlyName("instr_handler",instr);
    } else {
      sym._exec = makeFriendlyName("instr_handler",instr);
    }

    _id_end = InstrCoreSyms::_next_id;
  }


  ostream &operator<<(ostream &o,const InstrSym &i)
  {
    o << "(" << i._id << ") ";
    if (i._instr) {
      o << i._instr->name();
    } else {
      o << "<empty>";
    }
    return o;
  }

  ostream &operator<<(ostream &o,const InstrSyms &js)
  {
    ForEach(js,i) {
      o << i->second;
    }
    o << "Decode Miss:  " << js._decode_miss << "\n";
    return o;
  }

  ostream &operator<<(ostream &o,const InstrCoreSyms &is)
  {
    ForEach(is,i) {
      o << i->first->name() << ":\n\n" << i->second;
    }

    o << "\n";
    return o;
  }

  gc_string getBrTrgUpdateName()
  {
    return "branch_target_update";
  }

  gc_string getSetParmName(const gc_string &n)
  {
    return n + "_set_parm";
  }

  gc_string getGetParmName(const gc_string &n)
  {
    return n + "_get_parm";
  }

  gc_string getListParmName(const gc_string &n)
  {
    return n + "_list_parm";
  }

  gc_string getMemPointsIterator(bool lib_mode,bool is_const)
  {
    return gc_string() + ((lib_mode) ? "typename " : "") + "MemPoints::" + ((is_const) ? "const_iterator" : "iterator");
  }

  gc_string getUpdateRegFunc(const gc_string &n)
  {
    return n + "_updateReg";
  }

  gc_string getDepReset()
  {
    return "resetDepTracking";
  }

  gc_string getDepVar()
  {
    return "_dep_tracking";
  }

  gc_string getRegSeqVar()
  {
    return "_reg_seq";
  }

  gc_string getMemSeqVar()
  {
    return "_mem_seq";
  }

  gc_string getGraphiteDefaultGroup()
  {
    return "DefaultInstr";
  }

  gc_string getGraphitePerfModel()
  {
    return "_perf_model";
  }

  void writeDepItem(ostream &o,const Instr::Dependencies::DepItem &di)
  {
    o << dec;
    if (di.gmem()) {
      o << "DepItem::dpMem,0,-1,";
    }
    else if (auto mem = di.mem()) {
      o << "DepItem::dpMem," << mem->id() << ",-1,";
    }
    else if (auto reg = di.regb()) {
      o << "DepItem::dpReg," << reg->id() << ",";
      if (di._index) {
        o << "(" << di._index << "),";
      } else {
        o << "-1,";
      }
    }
    o << di._seq;
  }

  // Generates code for setting up the dependency tracking patterns.
  void genDepTrackingSetup(ostream &o,const Instr &instr)
  {
    if (auto dep = instr.dependencies()) {
      o << "{\n";
      
      // For each dependency item, add source and target information.
      ForEach(dep->exprs(),i) {
        const Instr::Dependencies::DepExpr &de = *i;
        o << getDepVar() << ".add_trg(";
        writeDepItem(o,de._trg);
        o << ");\n";
        ForEach (de._srcs,j) {
          o << getDepVar() << ".add_src(";
          writeDepItem(o,*j);
          o << ");\n";
        }
      }
      o << "}\n";
    }
  }

  void genBitsDecls(ostream &o,bool little_endian)
  {
    gc_string lepfx;
    if (little_endian) {
      lepfx = "le_";
    }

    o << "#include \"helpers/" << lepfx << "sintbv.h\"\n"
      << "#define bits adl::" << lepfx << "intbv\n"
      << "#define sbits adl::" << lepfx << "sintbv\n\n"
      << "#define ns_bits " << lepfx << "intbv\n"
      << "#define ns_sbits " << lepfx << "sintbv\n\n";
  }

  gc_string getDmiReadLatency()
  {
    return "_read_latency";
  }

  gc_string getDmiWriteLatency()
  {
    return "_write_latency";
  }

  gc_string getMemTag()
  {
    return "_mem_tag";
  }

  gc_string getCtxUpdateName()
  {
    return "check_context_updates";
  }
  
  gc_string getContextUpdateVar()
  {
    return "_context_update_event";
  }

  gc_string getInstrTableType()
  {
    return "TableType";
  }

  gc_string getInstrTableArg()
  {
    return "table_type";
  }  
  
}

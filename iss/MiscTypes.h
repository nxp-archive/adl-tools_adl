//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Miscellaneous types and functions used by the ISS generator.
//

#ifndef _MISC_TYPES_H_
#define _MISC_TYPES_H_

#include <iosfwd>

#include "helpers/gc_string.h"
#include "helpers/gc_set.h"
#include "helpers/gc_hash_map.h"

#include "fe/Types.h"

namespace Opencxx {
  class Environment;
  class Ptree;
}

namespace adl {

  class Core;
  class GenericField;
  class ConfigDB;
  class InstrWalker;
  class opfxstream;
  class RegBase;
  class Parm;
  class Instr;
  class JitInstrInfo;

  typedef gc_set<const RegBase *> RegBaseSet;

  // The type of the instruction packet used for transactional ISSs.
# define PktType "InstrPacket"

# define SlotType "InstrSlot"

  // Generic name for an argument used to store the instruction packet.
# define PktArg "instr_packet"

  // Generic name for the variable used for the instruction packet for
  // transactional ISSs.
# define PktName "_instr_packet"

  // Generic name for the variable used for the name of the core.
# define CoreName "core"

  // Default DMI page size.
  enum { DefaultDmiPageSize = 128 };

  // Attributes used for describing an access type.
  enum AccessType {aInstr,aLoad,aStore};

  // For storing information about a single translation-time conditional.
  struct TransCond : public gc {
    TransCond() : _sym(0), _cond(0), _val(false) {};

    Opencxx::Ptree *_sym;  // Symbol used for toggling the conditional during jit pre-phase.
    Opencxx::Ptree *_cond; // The conditional expression itself.

    bool            _val;  // Temporary- used for conveying the current value during code generation.
  };

  // Structure for storing information about translation-time conditionals, used by the JIT.
  struct TransConds : public gc_vector<TransCond *> {};

  // Structure for storing information about extra parameters evaluated at
  // translation time and then passed into the instruction.
  typedef TransCond ExtraParm;
  struct ExtraParms : public gc_vector<ExtraParm *> {};

  enum TMode { JitTrans = 0, DcTrans = 1, DcTransFixed = 2, NormalTrans = 3 };

  // Used for associating instruction handler symbols (Ptree names) with
  // instruction names (hashed by core).

  struct InstrSym : public gc {
    enum BranchType { brNone, brAlways, brCond, brCalc };
    enum SplitBranchType { sbNone, sbFirst, sbSecondUncond, sbSecondCond, sbCancel };

    Opencxx::Ptree *_ewrap;       // Symbol for the wrapper function for calculating field values, if applicable.
    Opencxx::Ptree *_exec;        // Symbol for the actual execution function.
    Opencxx::Ptree *_alt_exec;    // Symbol for the JIT actual execution
                                  // function, where it might be different from
                                  // the standard exec function, such as for a
                                  // memory-op, which will use the DMI cache.
    Opencxx::Ptree *_disasm;      // Symbol for the disassembler function.
    Opencxx::Ptree *_trans;       // Symbol for the translation function (for LLVM JIT).
    Opencxx::Ptree *_jit_trans;   // Symbol for the translation function (for decode-cache in JITs).
    Opencxx::Ptree *_dc_trans;    // Symbol for the translation function (for decode-cache models).
    Opencxx::Ptree *_logarg;      // Symbol for the instruction argument logger function
    Opencxx::Ptree *_leafwrap;    // Symbol for the wrapper function for the action in the leaf node of "simulate"

    int               _id;
    int               _log_id;
    int               _plog_id;
    const Instr      *_instr;
    bool              _pc_rel_instr;
    bool              _signed;
    int               _shift;
    unsigned          _offset;
    BranchType        _brType;
    SplitBranchType   _sbType;
    const InstrField *_br_op;
    bool              _halts;
    bool              _uses_eb_res; // True => Uses event-bus resources.

    InstrSym() : _id(-1), _instr(0) { init(); };
    InstrSym(int id,int log_id,int plog_id,const Instr *instr) : 
      _id(id), _log_id(log_id), _plog_id(plog_id), _instr(instr) { init(); };

    void reset() { _id = -1; };

    bool valid() const { return _id >= 0; };
    
    Opencxx::Ptree *exec_func() const { return (_ewrap) ? _ewrap : _exec; };
    Opencxx::Ptree *jit_exec_func() const { return (_alt_exec) ? _alt_exec : _exec; };

    bool is_sb_1st() const { return _sbType == sbFirst; };
    bool is_sb_2nd() const { return (_sbType == sbSecondUncond) || (_sbType == sbSecondCond); };
    bool is_sb_cancel() const { return _sbType == sbCancel; };

    bool is_branch() const { return _brType != brNone; };
    bool is_imm_branch() const { return (_brType == brAlways) || (_brType == brCond); };
    bool is_cond_branch() const { return _brType == brCond; };
    bool is_cond_split_branch() const { return _sbType == sbSecondCond; };
    bool is_uncond_branch() const { return _brType == brAlways; };
    bool is_calc_branch() const { return _brType == brCalc; };
    bool is_noncalc_branch() const { return is_imm_branch() || is_cond_branch(); };
    bool halts() const { return _halts; };
    bool uses_eb_res() const { return _uses_eb_res; };

    Opencxx::Ptree *trans(TMode t) const { return (t == JitTrans) ? _jit_trans : _dc_trans; };

    const InstrField *br_op() const { return _br_op; };

  private:
    void init();
  };

  std::ostream &operator<<(std::ostream &,const InstrSym &);

  struct InstrSyms : public gc_hash_map<gc_string,InstrSym> {

    InstrSyms();

    void add(const Instr &instr,bool valid,bool tracing_on,bool has_modified_fields);

    int                 _id_start, _id_end, _iid_start; // Min/max IDs.

    // Place decode-cache items first, so that we can ignore them in the JIT.
    InstrSym            _decode_miss;                   // Info about the decode-miss function.
    InstrSym            _td_check;                      // Info about the temporal-decoupling check function.
    InstrSym            _icache_touch;                  // Info about the icache-touch function.
  };

  std::ostream &operator<<(std::ostream &,const InstrSyms &);

  // Stores information about each compiled JIT instruction handler, plus
  // some extra info.
  struct InstrCoreSyms : public gc_hash_map<const Core *,InstrSyms> {

    static unsigned get_next_id() { return _next_id++; };
  private:
    static unsigned _next_id;
    friend struct InstrSyms;
    friend struct InstrSym;
  };

  std::ostream &operator<<(std::ostream &,const InstrCoreSyms &);

  // Generate a friendly (C-identifier) name from a supplied instruction.
  Opencxx::Ptree *makeFriendlyName(const gc_string &pfx,const Instr &instr);

  void writeCurLocation(opfxstream &o,const ConfigDB &config,const gc_string &filename);

  void writeLineDirective(opfxstream &o,const ConfigDB &config,Opencxx::Environment *env,Opencxx::Ptree *expr);

  // Gets the function arguments of a lambda function.
  Opencxx::Ptree *funcArgs(Opencxx::Ptree *f);

  // Gets a function's body after translating it.  Always use this function,
  // rather than calling the walker directly, because there's a subtlety of
  // usage here: You must translate the entire function so that the functions
  // arguments are properly recorded.  This is required in order to translate
  // bits objects correctly when slices are taken: If the arguments are not
  // recorded, then we don't know their types.
  Opencxx::Ptree *funcBody(InstrWalker &walker,Opencxx::Environment *e,Opencxx::Ptree *f);

  void genFunc(opfxstream &o,const ConfigDB &config,InstrWalker &walker,const gc_string &filename,
               const gc_string &fname,Opencxx::Environment *env,Opencxx::Ptree *func,
               bool isConst = false,bool isUsed = false);

  // Generate a type string for a field.
  gc_string genFieldType(const GenericField &);

  // Generate an integer conversion accessor for a field.
  gc_string genFieldIntGet(const GenericField &);

  // Generate a constructor for a field.
  gc_string genFieldInit(const GenericField &);

  gc_string getTraceCacheInstrPrefix();

  gc_string getParmType(const Parm &p);
  gc_string getParmName(const Parm &p);
  gc_string getParmHandleName(const Parm &p);
  gc_string getParmAssign(const Parm &p);

  gc_string getTransType(AccessType type);
  gc_string getBrTakenHandlerName();
  gc_string getAddrCheckHandler();
  gc_string getAddrCheckCall(const Core &core,const gc_string &ea,const gc_string &ra,const gc_string &pfx,
                             const gc_string &tlb,const gc_string &cond_tlb,AccessType type);

  gc_string getNiaSfx();

  gc_string getSetParmName(const gc_string &);
  gc_string getGetParmName(const gc_string &);
  gc_string getListParmName(const gc_string &);

  gc_string getMemHandlerName(const gc_string &);

  gc_string getJitCoreGlobal(const Path &);

  gc_string getJitInstrPrefix();
  gc_string getJitBranchTarget();
  gc_string getJitMemSuffix();
  gc_string getJitPcValue();
  gc_string getJitInstrValue();
  gc_string getJitLastRaMember();
  gc_string getJitTraceVar();
  gc_string getInstrPageUpdateVar();
  gc_string getJitBBArg();
  gc_string getJitBBIndexArg();

  bool genICacheTouch(const Core &core,const ConfigDB &config);
  gc_string getJitICacheTouch();

  gc_string getDcInstrPrefix(TMode tmode);

  gc_string getCommitQueueName();
  gc_string getRestoreStackName();
  gc_string getRollbackEnableName();
  gc_string getIgnoreEnableName();

  gc_string getInstrAttrType(const Core &);
  gc_string getInstrAttrName(const gc_string &n);
  gc_string getInstrAttrName(unsigned id);
  gc_string getInstrAttrVar();
  gc_string getInstrAttrGetter();
  gc_string getMmuInstrAttrGetter();

  gc_string getRegFileAttrType(const Core &);
  gc_string getRegFileAttrName(const gc_string &n);
  gc_string getRegFileAttrName(unsigned id);

  gc_string getDmiCacheName(const gc_string &memname,const gc_string &type);  
  gc_string getDmiCacheUpdateName(const gc_string &memname,const gc_string &type);
  gc_string getDmiCacheInvalidateName(const gc_string &memname,const gc_string &type);
  gc_string getDmiCacheInvalidateName(const gc_string &memname);
  gc_string getDmiCacheEnable();
  gc_string getDmiCacheDisable();
  gc_string getDmiReservedCheck(const gc_string &type);

  gc_string getEventBusRecvType();
  gc_string getEventBusRecvName();

  gc_string getEventBusMutexType();
  gc_string getEventBusMutexName();
  gc_string getMemLocks();
  gc_string getAtomicFlag();
  gc_string getMemMutex();

  gc_string getActiveCallback();

  gc_string getBrTrgUpdateName();
  gc_string getBrUserHookName();
  gc_string getBrHookName();

  gc_string getMemPointsIterator(bool lib_mode,bool is_const);

  gc_string getDepVar();
  gc_string getDepReset();
  gc_string getRegSeqVar();
  gc_string getMemSeqVar();

  gc_string getGraphiteDefaultGroup();
  gc_string getGraphitePerfModel();

  gc_string getUpdateRegFunc(const gc_string &n);

  void genDepTrackingSetup(std::ostream &o,const Instr &instr);

  void genBitsDecls(std::ostream &o,bool little_endian);

  gc_string getDmiReadLatency();
  gc_string getDmiWriteLatency();
  gc_string getMemTag();

  gc_string getCtxUpdateName();
  gc_string getContextUpdateVar();

  gc_string getInstrTableType();
  gc_string getInstrTableArg();
}

// Helper macros for implementing wrapped implementation classes.

#define ImplCall(wrap,func) void wrap::func()   \
  {                                             \
    _impl->func();                              \
  }

#define ImplCallArgs(wrap,func,args,parms) void wrap::func args \
  {                                                             \
    _impl->func parms;                                          \
  }

#endif

//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Code transformer for instruction action code.
//

#ifndef _INSTRWALKER_H_
#define _INSTRWALKER_H_

#include <string>

#include "fe/Types.h"

#include "helpers/gc_hash_map.h"
#include "helpers/gc_string.h"
#include "helpers/RtlWalker.h"
#include "helpers/BasicTypes.h"

namespace adl {

  class Parm;
  class SlotAlloc;
  class TransCond;
  class TransConds;
  class ExtraParms;
  
  typedef gc_hash_map<gc_string,const SlotAlloc *> SlotAllocPtrs;

  // Enum used for controlling parm translation:
  enum ParmTransType {
    ptNone,       // Do not do any core parm translation.
    ptRecord,     // Just record what parameters are used.
    ptTrans,      // Do actual translation, converting parms to random symbols.
  };

  // Map for mapping core parameters (string keys) to symbols.
  struct PItem {
    Opencxx::Ptree *_sym;
    const Parm     *_p;

    PItem() : _sym(0), _p(0) {};
    PItem(Opencxx::Ptree *s,const Parm *p) : _sym(s), _p(p) {};
  };
  struct ParmTrans : public gc_hash_map<gc_string,PItem> {};

// Transformations made by this class:
// 1.  Standard RtlWalker stuff (let, var).
// 2.  If rv_okay is false, a return statement may not return a value.
class InstrWalker : public RtlWalker {
public:
  InstrWalker(Opencxx::Environment *e,bool ld_okay,bool lib_mode,const Core *core = 0) :
    RtlWalker(e,ld_okay,lib_mode),
    _core(core),
    _alt_thisreg_index(0),
    _ctx_index(0),
    _has_skip(false),
    _memory_accessed(false),
    _reg_record(true),
    _has_halt(false),
    _curr_instr_accessed(false),
    _record_mem_accesses(false),
    _slots(0),
    _conditional_slot_access(false),
    _packet_width(0),
    _translating_func_args(false),
    _hybrid_iss(false),
    _track_reads(false),
    _suffix_reg(0),
    _instr_item(0),
    _trans_cond_mode(0),
    _trans_conds(0),
    _extra_parms(0),
    _skip_commit(false),
    _nia_simple_mode(false),
    _translate_parms(ptNone),
    _parm_trans(0),
    _iname("") //
  {
    set_memory_suffix_default();
  }

  struct MemAccess {
    gc_string _name;
    unsigned    _size;
    unsigned    _wsize;

    MemAccess(const gc_string &n,unsigned size,unsigned wsize = 0) : _name(n), _size(size), _wsize(wsize) {};

    bool wsz_valid() const { return _wsize > 0; };
  };

  typedef gc_list<MemAccess> MemAccesses;

  void reset();

  void reset_regs() { _regs.clear(); };

  void reset_has_halt() { _has_halt = false; }

  void set_instr_item(const Instr *ib) { _instr_item = ib; };

  void set_trans_cond_mode(const InstrBase *t) { _trans_cond_mode = t; };

  TransConds *get_trans_conds() const { return _trans_conds; };
  void clear_trans_conds() { _trans_conds = 0; };

  ExtraParms *get_extra_parms() const { return _extra_parms; };
  void clear_extra_parms() { _extra_parms = 0; };

  void set_class_name(const gc_string &cn,const gc_string &cnp);

  void set_cur_reg (const gc_string &x,bool is_ctx_res);

  void set_alt_thisreg_index(Opencxx::Ptree *x) { _alt_thisreg_index = x; };

  void set_reg_sfx(const gc_string &s) { _reg_sfx = s; };
  void clear_reg_sfx() { _reg_sfx.clear(); };

  Opencxx::Ptree *get_ctx_index() const;

  void set_packet_width(unsigned pw) { _packet_width = pw; };

  void set_cur_cache (const gc_string &x) { _cur_cache = x;; };

  void set_conditional_slot_access(bool c) { _conditional_slot_access = c; };

  void set_slots(const SlotAlloc *slots) { _slots = slots; };

  void set_hybrid_iss(bool h) { _hybrid_iss = h; };

  void set_track_reads(bool t) { _track_reads = t; };

  void set_nia_simple_mode(bool ns) { _nia_simple_mode = ns; };

  const SlotAlloc *get_slots() const { return _slots; };

  void set_model_pfx(const gc_string &m) { _m_pfx = m; };
  void clr_model_pfx() { _m_pfx.clear(); };

  bool setRegRecord(bool rr) { bool o = _reg_record; _reg_record = rr; return o; };

  void set_skip_commit(bool c) { _skip_commit = c; };

  bool has_skip() const { return _has_skip; };

  bool has_halt() const { return _has_halt; };

  ParmTransType set_trans_parms(ParmTransType p);

  ParmTrans *get_parm_trans() const { return _parm_trans; };

  bool curr_instr_accessed() const { return _curr_instr_accessed; };

  bool memory_accessed() const { return _memory_accessed; };

  void add_parm_assign(const gc_string &p) { _parm_assigns.insert(p); };

  void set_memory_suffix(const gc_string &rs,const gc_string &ws) { _memory_read_suffix = rs; _memory_write_suffix = ws; };
  void add_memory_suffix(const gc_string &rs,const gc_string &ws) { _memory_read_suffix += rs; _memory_write_suffix += ws; };
  void set_memory_suffix_default() { _memory_read_suffix = "read"; _memory_write_suffix = "write"; };

  // Controls whether we record info about memory accesses.
  bool record_mem_accesses(bool r) { bool t = _record_mem_accesses; _record_mem_accesses = r; return t; };
  const MemAccesses &mem_accesses() const { return _mem_accesses; };

  void set_suffix_reg(const RegBase *reg,const gc_string &stmt = "") { _suffix_reg = reg; _suffix_stmt = stmt; };

  typedef gc_list<const RegBase *> RegBaseList;
  const RegBaseList &regs_referenced() const { return _regs; };

  void add_slots_func(const gc_string &fn,const SlotAlloc *sa) { _slot_funcs.insert(make_pair(fn,sa)); };
  void clear_slots_funcs() { _slot_funcs.clear(); };

  // Call this to translate a block of code.
  virtual Opencxx::Ptree* translateCode(Opencxx::Environment *e,Opencxx::Ptree *);
  // Alternate entry point- if reset is true, we reset the register list, else we do not.
  Opencxx::Ptree *translateCode(Opencxx::Environment *e,Opencxx::Ptree *,bool reset);

  virtual Opencxx::Ptree* TranslateVariable(Opencxx::Ptree*);
  virtual Opencxx::Ptree* TranslateAssign(Opencxx::Ptree *exp);
  virtual Opencxx::Ptree* TranslateFuncall(Opencxx::Ptree* exp);
  virtual Opencxx::Ptree* TranslateDotMember(Opencxx::Ptree* exp);
  virtual Opencxx::Ptree* TranslateDeclarator(bool, Opencxx::PtreeDeclarator*);
  virtual Opencxx::Ptree* TranslateFunctionImplementation(Opencxx::Ptree *exp);
  virtual Opencxx::Ptree *TranslateRegWrite(PtreeRegWrite *);
  virtual Opencxx::Ptree *TranslateRegRead(PtreeRegRead *);
  virtual Opencxx::Ptree *TranslateMemWrite(PtreeMemWrite *);
  virtual Opencxx::Ptree *TranslateMemRead(PtreeMemRead *);
  virtual Opencxx::Ptree *TranslateSubInstrCall(PtreeSubInstrCall *);
  virtual Opencxx::Ptree* TranslateIf(Opencxx::Ptree*);
  virtual Opencxx::Ptree* TranslateCond(Opencxx::Ptree*);

  virtual Opencxx::Ptree* TranslateFor(Opencxx::Ptree*);
  virtual Opencxx::Ptree* TranslateSwitch(Opencxx::Ptree*);
  virtual Opencxx::Ptree* TranslateWhile(Opencxx::Ptree*);
  virtual Opencxx::Ptree* TranslateDo(Opencxx::Ptree*);
  virtual Opencxx::Ptree* TranslateStmts(Opencxx::Ptree*);

  void setVarRename(const StrStrMap &renames) { _renames.clear();_renames.insert(renames.begin(),renames.end()); };
  
  void set_iname (gc_string & name) { _iname = name; }; // Copy the instruction buffer symbol from GenInstrs class
  
protected:
  enum MsgType {Info,Warning,Error};

  Opencxx::Ptree *genMsgCode(MsgType,Opencxx::Ptree *exp);

  Opencxx::Ptree *handleVariable(Opencxx::Ptree *var,Opencxx::TypeInfo *type,Opencxx::Ptree *ctx_element);
  bool isFunction(Opencxx::Ptree *exp);
  Opencxx::Ptree *ctx_arg() const;

  bool  is_zero(Opencxx::Ptree *x) const;
  bool  is_parent_method(Opencxx::Ptree *x) const;
  bool  is_mem(Opencxx::Ptree *x) const;
  bool  is_memory(Opencxx::Ptree *x) const;
  bool  is_mmu(Opencxx::Ptree *x) const;
  bool  is_cur_reg(const RegBase *x) const;
  bool  is_cur_cache(Opencxx::Ptree *x) const;
  bool  is_halt_oper(Opencxx::Ptree *x) const;
  bool  is_context(Opencxx::Ptree *x) const;
  bool  is_prefix_info(Opencxx::Ptree *x) const;
  bool  is_enumerated(Opencxx::Ptree *r, Opencxx::Ptree *x);
  bool  is_cur_reg_enumerated(Opencxx::Ptree *x);

  RegAccess translateRegAccess(const RegAccess &ra);

  const SlotAlloc *is_slot_func(const gc_string &fn) const;
  const MmuLookup *is_mmulookup(Opencxx::Ptree *x);
  const Parm *is_parm(Opencxx::Ptree *x) const;
  const Reg *is_register(Opencxx::Ptree *x);
  const RegFile *is_regfile(Opencxx::Ptree *x);
  const RegField *is_field(Opencxx::Ptree *x,const RegBase &reg) const;
  const InstrField *is_instrfield(Opencxx::Ptree *x) const;
  const SubInstr *is_subinstr(Opencxx::Ptree *x) const;

  std::pair<Opencxx::Ptree *,bool> transformSlice(Opencxx::Ptree *slice_left,Opencxx::Ptree *slice_right);

  Opencxx::Ptree *genBranchTakenCall(const Reg *reg);
  Opencxx::Ptree *get_nia_suffix(const Reg *reg);
  Opencxx::Ptree *genSliceRead(Opencxx::Ptree *read,Opencxx::Ptree *slice_left,Opencxx::Ptree *slice_right);
  Opencxx::Ptree *genRegSliceWrite(const RegBase *reg,Opencxx::Ptree *slice_left,Opencxx::Ptree *slice_right,
                                   Opencxx::Ptree *expr,Opencxx::Ptree *ctx_elm,Opencxx::Ptree *delayed,bool direct);
  Opencxx::Ptree *genRegFileSliceWrite(const RegBase *reg,Opencxx::Ptree *index,Opencxx::Ptree *rindex,
                                       Opencxx::Ptree *slice_left,Opencxx::Ptree *slice_right,Opencxx::Ptree *expr,
                                       Opencxx::Ptree *ctx_elm,Opencxx::Ptree *delayed,bool direct);

  Opencxx::Ptree *genRegFieldAccess(const RegBase *reg,const RegField *field,
                                    Opencxx::Ptree *field_index,Opencxx::Ptree *expr,
                                    Opencxx::Ptree *ctx_elm,Opencxx::Ptree *delayed,bool direct);
  Opencxx::Ptree *genRegFileFieldAccess(const RegBase *reg,const RegField *field,Opencxx::Ptree *index,Opencxx::Ptree *rindex,
                                        Opencxx::Ptree *field_index,Opencxx::Ptree *expr,
                                        Opencxx::Ptree *ctx_elm,Opencxx::Ptree *delayed,bool direct);

  Opencxx::Ptree *genRegAccess(const Reg *reg,Opencxx::Ptree *expr,Opencxx::Ptree *ctx_elm,Opencxx::Ptree *delayed,bool direct,
                               Opencxx::Ptree *l = 0,Opencxx::Ptree *r = 0);
  Opencxx::Ptree *genRegFileAccess(const RegFile *rf,Opencxx::Ptree *index,Opencxx::Ptree *rindex,Opencxx::Ptree *expr,Opencxx::Ptree *ctx_elm,
                                   Opencxx::Ptree *delayed,bool direct,Opencxx::Ptree *left = 0,Opencxx::Ptree *right = 0);

  Opencxx::Ptree *genSlotAccess(const RegBase *reg,Opencxx::Ptree *index,bool is_assign,bool is_ref,
                                bool is_partial,bool allow_hybrids,bool direct,Opencxx::Ptree *delay);

  Opencxx::Ptree *getSlotRefIndex(const RegBase *reg,Opencxx::Ptree *index,bool direct);

  Opencxx::Ptree *getSlotUsed(const RegBase *reg,Opencxx::Ptree *index,Opencxx::Ptree *delay,bool direct,Opencxx::Ptree *expr);

  Opencxx::Ptree *getTargetUpdate(const RegBase *reg,Opencxx::Ptree *expr);

  Opencxx::Ptree *getSlotRead(const RegBase *reg,Opencxx::Ptree *index,Opencxx::Ptree *left, Opencxx::Ptree *right,bool direct);

  Opencxx::Ptree *genMmuLookupWrite(const MmuLookup *lookup,Opencxx::Ptree *args,Opencxx::Ptree *expr);

  Opencxx::Ptree *genMemoryAccess(PtreeMemRead *pm,Opencxx::Ptree *expr);

  Opencxx::Ptree *genHelperArgs();

  Opencxx::Ptree *genHelperCall(const gc_string &fname,Opencxx::Ptree *fun,const SlotAlloc *sa,
                                Opencxx::Ptree *orig_args,Opencxx::Ptree *final_args);

  Opencxx::Ptree *genDelayedWrite(Opencxx::Ptree *args);

  void generateBitsEncoding(unsigned x,Opencxx::TypeInfo &t);
  Opencxx::Ptree *genArgsTree(const SubInstr &subinstr,const SubInstrGrp &subinstrgrp);
  Opencxx::Ptree *rename(Opencxx::Ptree *var);

  Opencxx::Ptree *getHelperDefaultParameter(const gc_string &fname,unsigned pindex);

  TransCond *add_trans_cond(Opencxx::Ptree *cond);
  const InstrBase* handle_trans_cond(Opencxx::Ptree *cond,Opencxx::Ptree* &cond2);

  Opencxx::Ptree *genMicroInstrsRun(Opencxx::Ptree *exp); // Generate run() details
  Opencxx::Ptree *genMicroInstrsRunCommit(Opencxx::Ptree *exp); // Generate run_commit() details
	
  const Core       *_core;

  gc_string       _class_name, _class_name_pfx;

  // Set by user- indicates the register's name for when we're processing a read
  // or write hook.
  gc_string       _cur_reg;

  // Use this as an alternate index, if set, when translating register-file
  // accesses which were ThisReg references
  Opencxx::Ptree *_alt_thisreg_index;

  gc_string       _reg_sfx;

  // Used for in-hook functions of context registers; this is the context index symbol.
  Opencxx::Ptree   *_ctx_index;

  // Set by user- indicates the cache's name for when we're processing cache
  // hooks.
  gc_string       _cur_cache;

  bool              _has_skip;

  bool              _memory_accessed;

  // Collects names of registers/register-files as we process a code chunk.
  bool              _reg_record;
  RegBaseList       _regs;

  bool	            _has_halt;

  // Is the getCurrentInstr method used?
  bool	            _curr_instr_accessed;

  bool              _record_mem_accesses;
  MemAccesses       _mem_accesses;

  // If set, then we should use this looking up register slots.
  const SlotAlloc  *_slots;
    
  // If true, then slot accesses will be conditionalized upon the packet being non-null.
  bool              _conditional_slot_access;

  typedef gc_vector<Opencxx::Ptree *> HelperParms;
  HelperParms       _hparms;

  unsigned          _packet_width;

  // If true, then we should add the instruction packet as the first argument of any
  // function declarations encountered.
  bool              _add_instr_packet;

  // Internal flag for communicating when we're translating a function's arguments.
  bool              _translating_func_args;

  // True => Creating a hybrid ISS, rather than just a transactional ISS.
  bool              _hybrid_iss;

  // True => When creating a hybrid or transactional ISS, also insert code to
  // track information about specific read operations.
  bool              _track_reads;

  // Suffixes used for modifying a read or write to memory.
  gc_string       _memory_read_suffix, _memory_write_suffix;

  // Lists functions which are used with an instruction packet when generating
  // a transactional ISS.
  SlotAllocPtrs     _slot_funcs;
  // Keeps possible custom remaps
  StrStrMap	      _renames;
  // Can be used to prefix all resources within a model so that a function can
  // be generated external to the main class definition.
  gc_string       _m_pfx;

  // Can be used to attach a suffix to a register assignment.
  const RegBase    *_suffix_reg;
  gc_string       _suffix_stmt;

  // If set, then use this for looking up instruction fields.
  const Instr     *_instr_item;

  // If set, then examine conditionals to see if they're only dependent upon
  // instruction fields.  If so, we record information about these
  // conditionals.
  const InstrBase  *_trans_cond_mode;

  // If any translation-time conditionals are found, they're stored here.
  TransConds       *_trans_conds;

  // If any extra parameters are needed, they're stored here.
  ExtraParms       *_extra_parms;

  // If true, skip instruction is ommited and _has_skip is set
  bool		        _skip_commit;

  // If true, then NIA writes use the simple form, which doesn't log an NIA
  // write as a branch taken.
  bool              _nia_simple_mode;

  // If true, then Core parms are substituted with generated symbol names.
  ParmTransType     _translate_parms;

  // If translate-parms mode is on, then the mapping of parms to symbols is here.
  ParmTrans        *_parm_trans;

  // Parameters to transform, on assignment, into function calls to an
  // assignment function.
  StrSet            _parm_assigns;
  
  // Store the instruction buffer symbol from GenInstrs class
  gc_string _iname; 
};

}

#endif

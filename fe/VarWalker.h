//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This class will walk a Ptree, storing all variables it
// finds so that they can be examined.
//

#ifndef _VARWALKER_H_
#define _VARWALKER_H_

#include "opencxx/Walker.h"

#include "helpers/AdlTranslatingWalker.h"

#include "Types.h"
#include "Helpers.h"

namespace rnumber {
  class RNumber;
}

namespace Opencxx {
  class PtreeArray;
  class PtreeDeclarator;
}

namespace adl {

  class MMU;
  class Core;
  class Context;
  class Memory;
  class Cache;
  class InstrBase;
  class ValueHash;
  class VarVal;
  class HelperInfo;
  class Base;
  class Instr;

  // Very simple walker which records variables encountered, so that they can be
  // analyzed by the caller.  Mainly for expression processing- for more complex
  // things, such as for blocks of code, use VarWalker, which also collects
  // resource usage information.
  class SimpleVarWalker : public Opencxx::Walker {
  public:
    SimpleVarWalker(Opencxx::Environment *);
    virtual ~SimpleVarWalker();

    const VarList &get_vars() const { return _vars; };

    void reset();

    virtual Opencxx::Ptree* TranslateVariable(Opencxx::Ptree*);

  protected:
    VarList _vars;
  };

  typedef gc_list<const Aspects *> ParentList;

	class VarWalker;
	class Aspect;

	// Predicate used for testing whether to expand aspects when a beginning or
	// ending of a block is encountered.
	struct AspectPred {
		virtual ~AspectPred() {};

		virtual bool operator()(const VarWalker &vw,const Aspect &a,bool pre) const = 0;
	};
	

  // This walks over an expression.  For each variable found, we call the virtual
  // function handleVariable (which does nothing by default).  We also store a
  // list of all variables we encounter which are not declared within the
  // expression.  If we encounter a return, then we signal an error if return_okay
  // is false.  The class also handles usage tracking.
  //
  // To use this class, just call Translate() on an expression.  You
  // should reset it to clear the internal list.
  //
  // Note: This class is pretty heavy-weight, so use it only for processing
  // action code that actually needs the usage trackling.  Otherwise, use
  // SimpleVarWalker, which just records variables encountered.
  //
  // Note also that the constructor take stwo Core pointers: One constant, one
  // not-constant.  If 0 is supplied for the nonconst Core, then we won't update
  // the core with any usage info discovered during parsing.
  class VarWalker : public AdlTranslatingWalker, public CurArgHandler {
  public:
    VarWalker(Opencxx::Environment *,const Core *core,Core *nccore,bool return_ok = true);
    virtual ~VarWalker() {};
		
    void set_return_ok(bool r) { _return_okay = r; };

		void set_aspects_pred(AspectPred *a) { _aspect_pred = a; };

    void set_instr(const InstrBase *instr = 0) { _instr = instr; };

    void set_cur_reg(const RegBase *cr,int index = -1) { _cur_reg = cr; _cur_reg_index = index; };

    void set_slice_ignore(const std::pair<Opencxx::Ptree *,Opencxx::Ptree *> &p) { _slice_ignore = p; };

    bool set_track_usage(bool t) { bool ot = _track_usage; _track_usage = t; return ot; };

    // Access all escaping source variables.
    const VarList &get_srclist() const { return _sources; };
    // All escaping target variables.
    const VarList &get_trglist() const { return _targets; };
    // All escaping variables; targets and sources.
    const VarList get_escapes() const;

    // Set sources to be excluded.
    void set_excludes(ResExcludes *src,ResExcludes *trg);

    // Set information about the caller of a function, if processing a function
    // that's called by something else.
    void set_func_call_info(const gc_string &name,Opencxx::PtreeArray *args);

    // Set information about an alias.  This applies to alias parameters which
    // might have fixed values, which might influence resource usage.
    void set_alias_values(const InstrAlias *alias);

    // Source and target register information.
    const UsageInfo &usageinfo() const;

    // Clear the list of variables.
    void reset();

		// Returns true if we're at the outer-most scope of a block of code we're
		// translating.
		bool is_outer_scope() const;

    const gc_string &mo_offset_field() const { return _mo_offset_field; };

    // Called when we find a variable.
    virtual Opencxx::Ptree *handleVariable(Opencxx::Ptree *v,Opencxx::Ptree *exp);

    // Called when we find an escaping variable.  Default is to add to _vars
    // list.  The function's return value is the return value of
    // TranslateVariable.
    virtual Opencxx::Ptree* handleEscape(Opencxx::Ptree *var,Opencxx::Ptree *exp);

    virtual Opencxx::Ptree* TranslateTemplateFunction(Opencxx::Ptree* temp_def, Opencxx::Ptree* fun);
    virtual Opencxx::Ptree* TranslateUserPlain(Opencxx::Ptree*);
    virtual Opencxx::Ptree* TranslateVariable(Opencxx::Ptree*);
    virtual Opencxx::Ptree* TranslateReturn(Opencxx::Ptree*);
    virtual Opencxx::Ptree* TranslateFuncall(Opencxx::Ptree* exp);
    virtual Opencxx::Ptree* TranslateDotMember(Opencxx::Ptree*);
    virtual Opencxx::Ptree* TranslateAssign(Opencxx::Ptree *exp);
    virtual Opencxx::Ptree* TranslateUnary(Opencxx::Ptree *exp);
    virtual Opencxx::Ptree* TranslatePostfix(Opencxx::Ptree *exp);
    virtual Opencxx::Ptree* TranslateInfix(Opencxx::Ptree *exp);
    virtual Opencxx::Ptree* TranslateParen(Opencxx::Ptree* exp);
    virtual Opencxx::Ptree* TranslateEnumSpec(Opencxx::Ptree*);
    virtual Opencxx::Ptree* TranslateLabel(Opencxx::Ptree*);
    virtual Opencxx::Ptree* TranslateIf(Opencxx::Ptree*);
    virtual Opencxx::Ptree* TranslateSwitch(Opencxx::Ptree*);
    virtual Opencxx::Ptree* TranslateCase(Opencxx::Ptree* s);
    virtual Opencxx::Ptree* TranslateDeclaration(Opencxx::Ptree* def);
    virtual Opencxx::Ptree* TranslateBlock(Opencxx::Ptree* block);
    virtual Opencxx::Ptree* TranslateAssignInitializer(Opencxx::PtreeDeclarator*, Opencxx::Ptree*);
    virtual Opencxx::Ptree* TranslateArgDeclList(bool record, Opencxx::Ptree*, Opencxx::Ptree* args);
    virtual Opencxx::Ptree* TranslateFunctionImplementation(Opencxx::Ptree* impl);
    virtual Opencxx::Ptree* TranslateFor(Opencxx::Ptree* s);
    virtual Opencxx::Ptree* TranslateWhile(Opencxx::Ptree* s);
    virtual Opencxx::Ptree* TranslateDo(Opencxx::Ptree* s);
    virtual Opencxx::Ptree* TranslateBreak(Opencxx::Ptree* s);
    virtual Opencxx::Ptree* TranslateSubInstrCall(PtreeSubInstrCall* s);
    virtual Opencxx::Ptree *TranslateRegWrite(PtreeRegWrite *);
    virtual Opencxx::Ptree *TranslateRegRead(PtreeRegRead *);
    virtual Opencxx::Ptree *TranslateMemWrite(PtreeMemWrite *);
    virtual Opencxx::Ptree *TranslateMemRead(PtreeMemRead *);
    virtual Opencxx::Ptree* TranslateStmts(Opencxx::Ptree*);

    // Should only be used internally.
    bool get_variable_value(unsigned &res,Opencxx::Ptree *x);

  protected:

    ParentList getParentObj();
    void setRNumberField(rnumber::RNumber &x,unsigned l,unsigned r);
    bool isFunction(Opencxx::Ptree *exp);
    Opencxx::Ptree *inAssign() const;
    bool delayedWrite() const;
    bool directAccess() const;
    const HelperInfo *nestedHelper() const;
    Opencxx::Ptree *nestedHelperArg() const;
    bool inConditional() const;
    bool inSwitch() const;
    bool inLoopBody() const;
    bool inLoopBody2() const;
    bool inLoopGuard() const;
    bool constEval() const;
    bool exprEval() const;
    bool fullExprEval() const;
    void merge_uinfo(bool discard = false,bool merge_cond = false);
    void merge_switch_uinfo();
    void get_switch_uinfo(UsageInfo &uinfo,bool &have_result,bool &take_next,unsigned &res);
    void set_case_take_next(bool x);
    void set_case_record(bool x);
    ValueHash *values();
    bool update_if_fixed(UsedReg &ur,Opencxx::Ptree *index);
    void record_parm(Opencxx::Ptree *args,int index);
    void get_arg_values(Opencxx::PtreeArray &values,Opencxx::PtreeArray &args);

    Opencxx::Ptree *attemptExprEval(Opencxx::Ptree *left,Opencxx::Ptree *op,Opencxx::Ptree *right);
    Opencxx::Ptree *attemptExprEval(Opencxx::Ptree *left,Opencxx::Ptree *op);
    bool translate_const_expr(unsigned &res,Opencxx::Ptree *exp);
    bool update_partial_mask(UsedReg &ur,Opencxx::Ptree *portion,Opencxx::Ptree *portion_arg = 0);
    void update_parm_conv(UsedReg &ur);
    bool ignore_slice(const UsedReg &ur,Opencxx::Ptree *left,Opencxx::Ptree *right);

    void check_variable(Opencxx::Ptree *left,Opencxx::Ptree *op,Opencxx::Ptree *right);
    void check_loop_guard(Opencxx::Ptree *left,Opencxx::Ptree *op,Opencxx::Ptree *right);
    void check_incr_or_decr(Opencxx::Ptree *oprnd,Opencxx::Ptree *op);

    bool translateConstArg(Opencxx::PtreeArray &array,int n,bool &changed);
    void addUsedMem(const gc_string &mem,bool forced_write = false);
    void addUsedCache(const gc_string &cache);
    void addUsedReg(UsedReg ur);
    UsedRegs::iterator insertUsedReg(UsedRegs &regs,const UsedReg &ur);
    void update_regs(Opencxx::Ptree *loc,UsedRegList &parms,UsedRegs &dest,UsedRegs &src,Opencxx::PtreeArray &args);
    Opencxx::Ptree *checkRegFileAccess(const RegFile *regfile,Opencxx::Ptree *index,bool partial,
                                       Opencxx::Ptree *portion,Opencxx::Ptree *portion_arg = 0);
    void checkCtxAccess(const Context *ctx,Opencxx::Ptree *index,Opencxx::Ptree *member);
    void checkCtxAccess(const Context *ctx,Opencxx::Ptree *index,const RegBase *r);
    Opencxx::Ptree *checkHelper(Opencxx::Ptree *fun,Opencxx::PtreeArray &args,HelperInfo *hinfo,const gc_string &name);
    Opencxx::Ptree* handleDotMember(bool &used_field_index,bool &const_value,Opencxx::Ptree* exp,Opencxx::Ptree *field_index,Opencxx::Ptree *arg2);
    PtreeRegRead *handleRegRead(Opencxx::Ptree *var,Opencxx::Ptree *exp,Opencxx::Ptree *ctx_elm);
    PtreeRegRead *handleRegFieldRead(bool &,Opencxx::Ptree *exp,const Reg *r,Opencxx::Ptree *field,
                                     Opencxx::Ptree *field_index,bool ignore_usage=false);
    PtreeMemRead *handleMemAccess(Opencxx::Ptree *exp,const gc_string &fname,Opencxx::PtreeArray &array);

    PtreeRegRead *make_reg_obj(Opencxx::Ptree *exp,const RegBase *r);
    PtreeMemRead *make_mem_obj(Opencxx::Ptree *exp);

    void getAspects(Opencxx::PtreeArray &actions,const gc_string &l_str,bool pre);
    bool aspectCheck(const Aspect &a,const Aspects *top_parent);

    // For argument handling of nested helpers.
    virtual void setCurArg(unsigned index);
    virtual void doneWithArg();

    void newValueScope();
    void exitValueScope();

    Opencxx::Ptree *get_rename(const gc_string &n);

    const Memory     *get_mem(const gc_string &n);
    const Cache      *get_cache(const gc_string &n);
    const Context    *get_ctx(Opencxx::Ptree *x);

    const RegBase    *get_regitem(Opencxx::Ptree *x);
    const Reg *get_reg(Opencxx::Ptree *x);
    const RegFile *get_regfile(Opencxx::Ptree *x);

    const RegField *get_field(Opencxx::Ptree *x,const RegBase &reg) const;

    bool is_cur_reg(const RegBase *) const;

    bool is_instrfield(Opencxx::Ptree *x);
    bool is_field_func_call(Opencxx::Ptree *exp);
    bool is_blk(Opencxx::Ptree *x);

	Opencxx::Environment       *_outer_env;
    const Core                 *_core;
    Core                       *_nccore;

    const RegBase              *_cur_reg;        // If processing a hook, this
                                                 // is a pointer to the register
                                                 // item associated with that
                                                 // hook.
    int                         _cur_reg_index;  // If processing a hook, this
                                                 // refers to a register index,
                                                 // if relevant.
    std::pair<Opencxx::Ptree*,Opencxx::Ptree*> _slice_ignore;   // If set, then this describes
                                                 // a register slice to ignore.

    bool                        _return_okay;    // True => returns in code OK.
    bool                        _check_mem;      // Perform some error checking
                                                 // on memories.

	AspectPred                 *_aspect_pred;    // Predicate called at scope
																								 // beginnings, endings, to test
																								 // whether an aspect should be
																								 // expanded.

    tr_vector<Opencxx::Ptree *> _in_assign;      // Non-null => We're in an
                                                 // assignment statement.  Value
                                                 // is the assignment operator
                                                 // itself.

    struct DWInfo {
      bool           _in_dw;
      const UsedReg *_dr;
      const UsedMem *_dm;

      DWInfo (bool in_dw) : _in_dw(in_dw), _dr(0), _dm(0) {};
    };
    std::vector<DWInfo> _delayed_write;          // True => We're in a delayed
                                                 // write.  Ptr is to current
                                                 // used-reg or used-mem object,
                                                 // so that it can be updated
                                                 // with the delay expression.

    std::vector<bool>           _direct_access;  // True => We're in a direct
                                                 // access.

    tr_vector<const HelperInfo*> _nested_helper; // We're in a helper function
                                                 // call.  This is a pointer to
                                                 // the helper info object.

    tr_vector<Opencxx::Ptree *> _nested_helper_arg; // Used for nested helper
                                                    // function processing.
                                                    // This stores the current
                                                    // argument we're
                                                    // translating.

    enum State { sNone, sSwitch, sCond, sLoopBody, sLoopBody2, sLoopGuard };
    std::vector<State>            _state;        // State information during our
																								 // traversal.

    enum Eval { cNone, cConstEval, cExprEval };
    std::vector<Eval>             _expr_eval;    // True => Evaluate constant integer expressions.

    VarList                _sources;             // Sources not declared in the given expression.
    VarList                _targets;             // Targets not declared in the given expression.

    const InstrBase       *_instr;               // Used for finding valid
                                                 // instruction fields when doing
                                                 // source/target analysis.

    bool                     _track_usage;
    tr_list<UsageInfo>       _uinfo;             // For tracking usage
																								 // source/target regs and
																								 // helpers.

    struct SwitchUsageInfo : public UsageInfo {
      bool     _has_val;
      bool     _record;
      bool     _take_next;
      unsigned _val;
      
      SwitchUsageInfo(bool h,unsigned v,const UsageInfo &ui) : 
        UsageInfo(ui), 
        _has_val(h), 
        _record(false), 
        _take_next(false),
        _val(v) {};
    };

    tr_list<SwitchUsageInfo> _switch_uinfo;      // Used by switch statements to save their outer state for each case block.
    ResExcludes             *_src_excludes;      // Sources to be specifically excluded.
    ResExcludes             *_trg_excludes;      // Targets to be specifically excluded.

    typedef std::list<ValueHash*,traceable_allocator<ValueHash*> > ValueHashes;
    ValueHashes            _values;            // Stores variables' values on a per-scope basis.
    
    Opencxx::PtreeArray   *_func_args;         // If processing a helper
                                               // function called by something
                                               // else, then these are the
                                               // arguments of the call.
    gc_string              _func_name;         // If processing a helper fuction
                                               // called by something else, this
                                               // is the name of the function,
                                               // for error reporting purposes.

    gc_string              _mo_offset_field;   // Instruction field used to modify the micro-op mask, if applicable.
  };

}

#endif

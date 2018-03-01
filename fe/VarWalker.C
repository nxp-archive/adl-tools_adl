//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//


#include <cassert>
#include <iostream>
#include <vector>
#include <sstream>

#include "opencxx/parser/Leaf.h"
#include "opencxx/parser/LeafName.h"
#include "opencxx/parser/token-names.h"
#include "opencxx/parser/ptree-generated.h"
#include "opencxx/parser/PtreeDeclarator.h"
#include "opencxx/parser/PtreeTemplateDecl.h"
#include "opencxx/parser/PtreeExprStatement.h"
#include "opencxx/parser/PtreeBlock.h"
#include "opencxx/Environment.h"
#include "opencxx/Bind.h"
#include "opencxx/TypeInfo.h"
#include "opencxx/PtreeIter.h"
#include "opencxx/Class.h"
#include "opencxx/BindEnumName.h"

#include "helpers/fe_exceptions.h"
#include "helpers/gc_hash_map.h"
#include "helpers/Macros.h"

#include "VarWalker.h"
#include "ResWalker.h"
#include "Data.h"

using namespace std;
using namespace Opencxx;
using namespace PtreeUtil;
using namespace rnumber;

namespace adl {

  // Used to identify when a parameter does not have a fixed value.
  static Ptree *unknown_parm = Ptree::Make("<unknown>");

  static Ptree *temp_delay = Ptree::Make("<temp delay>");

  extern const char *MemoryName;

  // Returns the size of a bits or sbits declaration, or 0 if it's not such a thing.
  unsigned bits_size(Ptree *decl,Environment *env)
  {
    TypeInfo t;
    t.Set(decl->GetEncodedType(),env);
    if (t.IsTemplateClass() && (Eq(t.FullTypeName(),"bits") || Eq(t.FullTypeName(),"sbits"))) {
      TypeInfo at0;
      t.NthTemplateArgument(0,at0);
      unsigned size = 0;
      if (Ptree *p = at0.MakePtree()) {
        get_uint(size,p->Car(),env);
        return size;
      }
    }
    return 0;
  }

  // This applies the simple Eq test to two Ptree objects.  This will work for
  // leaves, but not for complex structures.
  struct PtreeEq {
    bool operator()(Opencxx::Ptree *x,Opencxx::Ptree *y) const
    {
      return Eq(x,y);
    }
  };

  SimpleVarWalker::SimpleVarWalker(Opencxx::Environment *e) :
    Walker(e)
  {
  }

  SimpleVarWalker::~SimpleVarWalker()
  {
  }

  void SimpleVarWalker::reset()
  {
    _vars.clear();
  }

  Ptree *SimpleVarWalker::TranslateVariable(Ptree *exp)
  {
    _vars.push_back(make_pair(exp,get_str_name(exp)));
    return exp;
  }

  


  // This stores information about variables' values in a piece of action code.
  // We store information about the item if it has a constant value or else if
  // it's a parameter that's never assigned.  If it's a parameter, then we store
  // information about which element it is in the parameter list.
  struct VarVal : public gc {
    enum Type { tVar, tField, tToken, tDirty };
    enum Loop { lNone, lIncr, lDecr, lEmpty };

    bool         _has_value; // True if an integer constant exists.
    unsigned     _value;  // Integer constant value, if a value is present.
    Ptree       *_orig;   // Original expression, if a field or parm.
    Environment *_env;    // Environment at type of addition.
    int          _pindex; // Parm index, if a parameter.
    Type         _type;   // General type of this variable.
    Loop         _loop;   // If involved in a loop, this indicates it.
    VarVal      *_end;    // If a loop, this stores info about the end-condition.

    VarVal() : _has_value(false), _value(0), _orig(0), _env(0), _pindex(-1), _type(tVar), _loop(lNone), _end(0) {};
    VarVal(Type t,Environment *e) : _has_value(false), _value(0), _orig(0),  _env(e), _pindex(-1), _type(t), _loop(lNone), _end(0) {};
    VarVal(Ptree *o,Environment *e,unsigned v) : _has_value(true), _value(v), _orig(o),  _env(e), _pindex(-1), _type(tVar), _loop(lNone), _end(0) {};
    VarVal(Ptree *o,Environment *e,unsigned v,Type t) : _has_value(false), _value(v), _orig(o), _env(e), _pindex(-1), _type(t), _loop(lNone), _end(0) {};

    bool is_dirty()    const { return _type == tDirty; };
    bool is_incr()     const { return _loop == lIncr; };
    bool is_empty()    const { return _loop == lEmpty; };
    bool is_loop_var() const { return _loop != lNone; };
    bool is_decr()     const { return _loop == lDecr; };
    bool is_parm()     const { return _pindex >= 0; };
    bool is_field()    const { return _type == tField; };
    bool is_range()    const { return _end; };
    bool has_value()   const { return _has_value; };

    void set_value(unsigned v) { _has_value = true; _value = v; };
    void set_as_var(unsigned v) { _has_value = true; _value = v; _type = tVar; };
    void set_field(Ptree *f) { _orig = f; _type = tField; }
    void set_dirty() { _type = tDirty; };
    void set_as_empty() { _loop = lEmpty; };
  };

  ostream &operator<<(ostream &o,const VarVal &v)
  {
    if (v._end) {
      o << "( ";
    }
    switch (v._type) {
    case VarVal::tVar:
      o << v._value;
      break;
    case VarVal::tToken:
      o << v._orig;
      break;
    case VarVal::tField:
      o << v._orig << " [field]";
      break;
    case VarVal::tDirty:
      o << "<dirty - " << v._orig << ">";
      break;
    }
    if (v._end) {
      o << " , " << *(v._end) << ") ";
      switch (v._loop) {
      case VarVal::lIncr:
        o << "(incr)";
        break;
      case VarVal::lDecr:
        o << "(decr)";
        break;
      case VarVal::lEmpty:
        o << "(empty)";
        break;
      case VarVal::lNone:
        break;
      }
    }
    if (v.is_parm()) {
      o << " [parm #" << v._pindex << "]";
    }
    return o;
  }

  // Hash for storing variables' values.  If we don't detect a conditional
  // update, then we can propagate constants in order to resolve source/target
  // usage more acurately.
  struct ValueHash : public std::tr1::unordered_map<Opencxx::Ptree *,VarVal,std::tr1::hash<Opencxx::Ptree*>,PtreeEq,traceable_allocator<VarVal> > {
    //public gc_hash_map<Opencxx::Ptree *,VarVal,__gnu_cxx::hash<Opencxx::Ptree*>,PtreeEq> {

    ValueHash(ValueHash *parent = 0) : _parent(parent) {};

    VarVal &add(Ptree *var)
    {
      return operator[](var);
    }

    VarVal &add(Ptree *var,Ptree *val,Environment *env,unsigned v)
    {
      VarVal &x = operator[](var);
      x = VarVal(val,env,v);
      return x;
    }

    VarVal &add_parm(Ptree *p,unsigned index,Environment *env)
    {
      VarVal &x = operator[](p);
      x = VarVal(p,env,0,VarVal::tToken);
      x._pindex = index;
      return x;
    }

    VarVal &add_field(Ptree *p,Ptree *f,Environment *env)
    {
      VarVal &x = operator[](p);
      x = VarVal(f,env,0,VarVal::tField);
      return x;
    }

    VarVal &add_as_type(Ptree *p,VarVal::Type t,Environment *env)
    {
      VarVal &x = operator[](p);
      x = VarVal(t,env);
      return x;
    }

    void add_dirty(Ptree *p,Environment *env)
    {
      add_as_type(p,VarVal::tDirty,env);
    }

    VarVal &add_to_top(Ptree *p,const VarVal &x)
    {
      auto iter = find(p);
      if (iter == end()) {
        auto ip = insert(make_pair(p,x));
        return ip.first->second;
      } else {
        return iter->second;
      }
    }

    // For each item in the hash item p, mark the item as dirty if it exists in
    // our current hash and doesn't exist in the environment.  If it exists in
    // the environment, then it implies that it's declared in this scope, and
    // thus does not propage up any further.  This is used when exiting a scope,
    // to mark items which have been modified and were declared in an outer
    // scope.
    void propagate_changes(ValueHash &vh)
    {
      Bind *b = 0;
      ForEach(vh,i) {
        VarVal &vv = i->second;
        if (VarVal *x = get(i->first)) {
          assert(vv._env);
          //cerr << "Checking " << i->first << '\n';
          //vv._env->Dump();
          if (!vv._env->LookupTop(i->first,b)) {
            x->_type = VarVal::tDirty;
          }
        }
      }
    }

    void mark_as_type(Ptree *p,VarVal::Type t,Environment *e)
    {
      if (VarVal *x = get(p)) {
        x->_type = t;
      } else {
        add_as_type(p,t,e);
      }
    }

    void mark_as_loop(Ptree *p,VarVal::Loop l,Environment *e)
    {
      if (VarVal *x = get(p,true)) {
        x->_loop = l;
      } else {
        VarVal &y = add(p,0,e,0);
        y._loop = l;
      }
    }

    // Mark an item as dirty, updating it if it already exists in the hash.
    void mark_as_dirty(Ptree *p,Environment *e)
    {
      mark_as_type(p,VarVal::tDirty,e);
    }

    void mark_as_incr(Ptree *p,Environment *e)
    {
      mark_as_loop(p,VarVal::lIncr,e);
    }

    void mark_as_decr(Ptree *p,Environment *e)
    {
      mark_as_loop(p,VarVal::lDecr,e);
    }

    // This searches the current scope only.
    VarVal *get_here(Ptree *x,bool dirty_ok = false)
    {
      if (!x || !x->IsLeaf()) {
        return 0;
      }
      //cout << "This scope:  " << *this << endl;
      auto iter = find(x);
      if (iter != end()) {
        if (dirty_ok || !iter->second.is_dirty()) {
          return &iter->second;
        }
      }
      return 0;
    }

    friend ostream &operator<< (ostream &o,const ValueHash &x);

    // This searches the current scope, then parent scopes.
    VarVal *get(Ptree *x,bool dirty_ok = false)
    {
      if (VarVal *v = get_here(x,dirty_ok)) {
        return v;
      } else if (_parent) {
        return _parent->get(x,dirty_ok);
      } else {
        return 0;
      }
    }

    void Display()
    {
      cerr << *this;
    }

  protected:
    ValueHash *_parent;
  };

  ostream &operator<< (ostream &o,const ValueHash &x)
  {
    o << "[\n";
    ForEach(x,iter) {
      o << "  " << iter->first << " = " << iter->second << "\n";
    }
    o << "]\n";
    return o;
  }

  VarWalker::VarWalker(Environment *e,const Core *core,Core *nccore,bool rok) : 
    AdlTranslatingWalker(e),
		_outer_env(AdlTranslatingWalker::env),
    _core(core),
    _nccore(nccore),
    _cur_reg(0),
    _cur_reg_index(-1),
    _return_okay(rok),
    _check_mem(true),
		_aspect_pred(0),
    _instr(0),
    _track_usage(true),
    _src_excludes(0),
    _trg_excludes(0),
    _func_args(0)
  {
    _uinfo.push_back(UsageInfo());
  }

  void VarWalker::reset()
  {
    _sources.clear();
    _targets.clear();
    _uinfo.back().clear();
    _in_assign.clear();
    _state.clear();
    _expr_eval.clear();
    _src_excludes = 0;
    _trg_excludes = 0;
    _instr = 0;
    _values.clear();
    _func_name.clear();
    _func_args = 0;
    _nested_helper.clear();
    _nested_helper_arg.clear();
    _delayed_write.clear();
    _direct_access.clear();
    _cur_reg = 0;
    _cur_reg_index = -1;
    _slice_ignore = make_pair((Opencxx::Ptree*)0,(Opencxx::Ptree*)0);
    _mo_offset_field.clear();
  }

	bool VarWalker::is_outer_scope() const
	{
		return env == _outer_env;
	}

  void VarWalker::set_excludes(ResExcludes *src,ResExcludes *trg)
  {
    _src_excludes = src;
    _trg_excludes = trg;
  }

  // Set information about the caller of a function, if processing a function that's called by something else.
  void VarWalker::set_func_call_info(const gc_string &name,PtreeArray *args)
  {
    _func_name = name;
    _func_args = args;
  }

  // Set information about an alias.  This is used to potentially refine usage
  // information, since an alias might call another instruction with fixed
  // values for some fields, thus allowing conditionals to be resolved.
  void VarWalker::set_alias_values(const Instr::Alias *alias)
  {
    assert(alias);
    
    newValueScope();
    ForEach(alias->_fields,a_iter) {
      if ( a_iter->_type == Instr::Field::Value) {
        Ptree *n = Ptree::Make("%s",a_iter->_field->name().c_str());
        values()->add(n,0,env,a_iter->_value);
      }
    }
  }

  const UsageInfo &VarWalker::usageinfo() const 
  { 
    assert(!_uinfo.empty()); 
    return _uinfo.back(); 
  }

  const VarList VarWalker::get_escapes() const 
  { 
    VarList x = _sources; 
    x.insert(x.end(),_targets.begin(),_targets.end()); 
    return x; 
  }

  ValueHash *VarWalker::values()
  {
    return (_values.empty()) ? 0 : _values.back();
  }

  Ptree *VarWalker::inAssign() const
  {
    return (_in_assign.empty()) ? 0 : _in_assign.back();
  }

  bool VarWalker::delayedWrite() const
  {
    return (_delayed_write.empty()) ? 0 : _delayed_write.back()._in_dw;
  }

  bool VarWalker::directAccess() const
  {
    return (_direct_access.empty()) ? 0 : _direct_access.back();
  }

  const HelperInfo *VarWalker::nestedHelper() const
  {
    return (_nested_helper.empty()) ? 0 : _nested_helper.back();
  }

  Ptree *VarWalker::nestedHelperArg() const
  {
    return (_nested_helper_arg.empty()) ? 0 : _nested_helper_arg.back();
  }

  bool VarWalker::inSwitch() const
  {
    return (_state.empty()) ? false : (_state.back() == sSwitch);
  }

  // This evaluates to true for both switch statements and if statements.  In other words,
  // any kind of conditional.
  bool VarWalker::inConditional() const
  {
    return (_state.empty()) ? false : (_state.back() == sCond || _state.back() == sSwitch);
  }

  bool VarWalker::inLoopBody() const
  {
    return (_state.empty()) ? false : (_state.back() == sLoopBody);
  }

  bool VarWalker::inLoopBody2() const
  {
    return (_state.empty()) ? false : (_state.back() == sLoopBody2);
  }

  bool VarWalker::inLoopGuard() const
  {
    return (_state.empty()) ? false : (_state.back() == sLoopGuard);
  }

  bool VarWalker::constEval() const
  {
    return (_expr_eval.empty()) ? false : _expr_eval.back() == cConstEval;
  }

  bool VarWalker::exprEval() const
  {
    return (_expr_eval.empty()) ? false : _expr_eval.back();
  }

  bool VarWalker::fullExprEval() const
  {
    return (_expr_eval.empty()) ? false : _expr_eval.back() == cExprEval;
  }

  void VarWalker::newValueScope()
  {
    if (_track_usage) {
      ValueHash *p = (_values.empty()) ? 0 : _values.back();
      _values.push_back(new ValueHash(p));
    }
  }

  void VarWalker::exitValueScope()
  {
    //cerr << "Exiting scope:  " << *values() << "\n";
    
    if (!_values.empty()) {
      ValueHash *p = _values.back();
      _values.pop_back();
      // Propagate changed values up. This is overly conservative, since we
      // might be in a conditional block we know won't be taken.  TODO: Fix
      // this.
      if (!_values.empty()) {
        values()->propagate_changes(*p);
      }
      delete p;
    }
  }

  // Normally, we push and pop value scopes at this point.  For loops, though,
  // we want to preserve variables across the 2-passes because this value scope
  // is where we track if a variable is a loop counter variable.
  Ptree* VarWalker::TranslateBlock(Ptree* block)
  {
		Ptree *x = 0;

    if (!inLoopBody2()) {
      newValueScope();
    }

		PtreeArray pre_actions, post_actions;
		getAspects(pre_actions,gc_string(),true);
		getAspects(post_actions,gc_string(),false);

		if (pre_actions.Number() || post_actions.Number()) {
			NewScope();
			ForRange(pre_actions.Number(),i) {
				pre_actions[i] = Translate(pre_actions[i]);
			}
			ForRange(post_actions.Number(),i) {
				post_actions[i] = Translate(post_actions[i]);
			}

			Ptree *block2 = AdlTranslatingWalker::TranslateBlock(block);

			pre_actions.Append(block2);
			if (post_actions.Number()) {
				ForRange((post_actions.Number()),i) {
					pre_actions.Append(post_actions[i]);
				}
			}

			if (pre_actions.Number()) {
				x = new PtreeBlock(First(block),pre_actions.All(),Third(block));
			}
			ExitScope();
		} else {
			x = AdlTranslatingWalker::TranslateBlock(block);
		}

    if (!inLoopBody()) {
      exitValueScope();
    }
    
    return x;
  }

  // This has to understand let blocks b/c their form
  // is not standard C++.  Otherwise, we ignore the
  // code.
  Ptree* VarWalker::TranslateUserPlain(Ptree*exp)
  {
    TypeInfo type;
    Ptree* keyword = First(exp);
    Ptree* rest = ListTail(exp, 1);

    NewScope();

    // Translate to take care of any other kind of nested constructs
    // that can be handled in a normal way.
    Ptree *args = Second(rest);
    Ptree *args2 = args;
    if(keyword->IsA(UserKeyword2)) {                // closure style
      args2 = TranslateArgDeclList2(true, env, false, false, 0, Second(rest));
    }
    Ptree *body = Nth(rest,3);
    Ptree *body2 = Translate(body);

    ExitScope();

    if (args == args2 && body == body2) {
      return exp;
    } else {
      return new PtreeUserPlainStatement(exp->Car(),ShallowSubst(args2,args,body2,body,exp->Cdr()));
    }
  }

  const Context *VarWalker::get_ctx(Ptree *x)
  {
    return (_core) ? _core->get_ctx(x->ToString()) : 0;
  }

  const Reg *VarWalker::get_reg(Ptree *x)
  {
    if (Eq(x,CurrentRegName)) {
      if (!_cur_reg) {
        PError(x,"No current register exists in this context, so " << CurrentRegName << " is not allowed.");
      }
      return dynamic_cast<const Reg*>(_cur_reg);
    } else if (_core) {
      return _core->get_reg(x->ToString());
    } else {
      return 0;
    }
  }

  bool VarWalker::is_cur_reg(const RegBase *r) const
  {
    return r == _cur_reg;
  }

  const RegBase *VarWalker::get_regitem(Ptree *x)
  {
    return (_core) ? _core->get_regitem(x->ToString()) : 0;
  }

  const Memory *VarWalker::get_mem(const gc_string &n)
  {
    return (_core) ? _core->get_memory(n) : 0;
  }

  const Cache *VarWalker::get_cache(const gc_string &n)
  {
    return (_core) ? _core->get_cache(n) : 0;
  }

  const RegFile *VarWalker::get_regfile(Ptree *x)
  {
    if (Eq(x,CurrentRegName)) {
      if (!_cur_reg) {
        PError(x,"No current register exists in this context, so " << CurrentRegName << " is not allowed.");
      }
      return dynamic_cast<const RegFile*>(_cur_reg);
    } else if (_core) {
      return _core->get_regfile(x->ToString());
    } else {
      return 0;
    }
  }

  const RegField *VarWalker::get_field(Ptree *x,const RegBase &reg) const
  {
    RegFieldHash::const_iterator i = reg.fields().find(x->ToString());
    if (i != reg.fields().end()) {
      return i->second;
    }
    return 0;
  }

  bool VarWalker::is_instrfield(Ptree *p)
  {
    if (_instr) {
      Ptree *f = 0;
      if (p->IsLeaf()) {
        f = p;
      } else if (Match(p,"[ [%? . %* ] %_ ]",&f)) {
        ;
      }
      gc_string n = (f) ? f->ToString() : "";
      int index = _instr->getAll(n);
      if (index >= 0) {
        return true;
      }
    }
    return false;
  }

  // Returns true if this is a function call where arguments are constants or
  // instruction fields.  Note that this does not translate the expression.
  bool VarWalker::is_field_func_call(Ptree *exp)
  {
    if (Ptree *call = get_func_call(exp,0)) {
      const char *fn = get_ident(get_func_call_name(call),0);
      if (!_core || !fn || !_core->get_helper(fn)) return false;
      Ptree *args = get_func_args(call);      
      while (args != 0) {
        Ptree *p = args->Car();

        // If this is a constant, then we're good.
        unsigned v;
        if (!translate_const_expr(v,p)) {
          // Otherwise, see if this is an instruction field.
          if (!is_instrfield(p)) {
            // Finally, see if this is a variable whose value is known.
            if (!get_variable_value(v,p)) {
              return false;
            }
          }
        }
        
        args = args->Cdr();
        if (args != 0) {
          // Skip the comma.
          args = args->Cdr();
        }
      }
      return true;
    } else {
      return false;
    }
  }
  
  bool VarWalker::is_blk(Ptree *p)
  {
    if (_instr) {
      gc_string n = p->ToString();
      ForEach(_instr->blks(),i) {
        if (*i == n) {
          return true;
        }
      }
    }
    return false;
  }

  PtreeRegRead *VarWalker::make_reg_obj(Ptree *exp,const RegBase *r)
  {
    PtreeRegRead *p = (inAssign()) ? new PtreeRegWrite(exp,is_cur_reg(r)) : new PtreeRegRead(exp,is_cur_reg(r));

    // Add to the core's master list of all accesses, for later scanning.
    if (_nccore && _track_usage) _nccore->pushRegAccess(p);

    return p;
  }

  PtreeMemRead *VarWalker::make_mem_obj(Ptree *exp)
  {
    return (inAssign()) ? new PtreeMemWrite(exp) : new PtreeMemRead(exp);
  }

  PtreeRegRead *VarWalker::handleRegRead(Ptree *var,Ptree *exp,Ptree *ctx_elm)
  {
    const RegBase *r  = 0;
    const Reg *reg    = 0;
    const RegFile *rf = 0;
    if ( (reg = get_reg(var)) ) {
      r = reg;
    } else if ( (rf = get_regfile(var)) ) {
      r = rf;
    }

    if (r) {

      // Ignore usage tracking on this for now, as we don't handle virtualizing
      // context-specific writes yet.
      if (!ctx_elm) {
        UsedReg ur(r);
        update_parm_conv(ur);
        addUsedReg(ur);
      }

      // If we have a register read operation, then return a PtreeRegRead object
      // here.
      if (reg) {
        auto pr = make_reg_obj(exp,reg);
        pr->_regaccess._reg = reg;
        pr->_regaccess._context = ctx_elm;
        return pr;
      } else if (rf && _cur_reg_index >= 0) {
        // This might look like a register read, but actually be a register-file
        // read if we're in, for example, a sparse-reg-file hook and the user used
        // ThisReg, but the item for this entry is an element of a register file.
        auto pr = make_reg_obj(exp,rf);
        pr->_regaccess._regfile = rf;
        pr->_regaccess._index = Ptree::Make("%d",_cur_reg_index);
        pr->_regaccess._context = ctx_elm;
        return pr;
      }
    }
    return 0;
  }

  Ptree *VarWalker::handleEscape(Ptree *var,Ptree *exp)
  {
    if (inAssign()) {
      _targets.push_back(make_pair(var,get_str_name(var)));
    } else {
      _sources.push_back(make_pair(var,get_str_name(var)));
    }

    if (auto pr = handleRegRead(var,exp,0)) {
      return pr;
    }

    return exp;
  }

  bool VarWalker::isFunction(Ptree *exp) 
  {
    TypeInfo t;
    exp->Typeof(this,t);
    return t.IsFunction();
  }

  Ptree *VarWalker::handleVariable(Ptree *v,Ptree *exp)
  {
    Environment *e = env;
    Bind *b = 0;
    bool found = false;

    // Special case: Is this a register-field accessor function?  If so, we want
    // to record our usage of it.  This doesn't use the usual mechanism, since
    // it's not a direct access and thus we don't create a PtreeRegRead or
    // PtreeRegWrite object.
    if (_nccore) {
      if (const RegField *rf = _nccore->is_regfield_helper(v->ToString())) {
        rf->set_field_usage(RegFieldHelperUsed);
      }
    }

    // Search through all scopes for the variable.
    while (e) {
      if (!e->LookupTop(v,b)) {
        e = e->GetOuterEnvironment();
      } else {
        found = true;
        break;
      }
    }
            
    if (!found) {
      // Try to look in the core's environment, which has references to
      // all other architectures.
      if (!_core || !_core->env()) {
        return handleEscape(v,exp);
      } else if (!_core->env()->LookupTop(v,b)) {
        return handleEscape(v,exp);
      }
    }

    return exp;
  }

  Ptree *VarWalker::TranslateVariable(Ptree *exp)
  {
    using namespace PtreeUtil;

    // If we're evaluating expressions, then try to get the value, if the value is known.
    if (fullExprEval()) {
      unsigned res;
      if (get_variable_value(res,exp)) {
        return make_leaf<LeafUNSIGNED>(res);
      }
    }

    // Handle namespace functions and methods here.
    if (isFunction(exp)) {
      return exp;
    }

    Ptree *v = exp;

    // This could be a template- check for that.
    if (!v->IsLeaf()) {
      Ptree *x = 0;
      if (Match(v,"[%? [< %* >]]",&x)) {
        v = x;
      }
    }

    return handleVariable(v,exp);
  }

  // Tries to get an integer value for a variable.  Returns true on success and
  // updates res.  Can handle simple integers and booleans.  This will return
  // false if a variable is part of a loop, since we don't know its precise
  // value at any given time.
  bool VarWalker::get_variable_value(unsigned &res,Ptree *x)
  {
    VarVal *v = 0;
    if (values() && (v = values()->get(x))) {
      if (v->has_value() && !(v->is_loop_var())) {
        res = v->_value;
        return true;
      }
    }
    return false;
  }

  // Attempt to evaluate a binary op.  If right is 0, then we assume a
  // unary op, with left holding the expression.
  Ptree *VarWalker::attemptExprEval(Ptree *left,Ptree *op,Ptree *right)
  {
    if (exprEval()) {
      AttemptExprEval<unsigned> ae;

      return ae.eval(left,op,right);
    }
    return 0;
  }

  // Attempt to evaluate a unary op.
  Ptree *VarWalker::attemptExprEval(Ptree *left,Ptree *op)
  {
    if (exprEval()) {
      AttemptExprEval<unsigned> ae;

      return ae.eval(left,op);
    }
    return 0;
  }
  
  // If we're evaluated expressions, then we strip the parentheses so that when
  // we process the values, they don't confuse us.  Otherwise, we pass on as-is.
  Ptree* VarWalker::TranslateParen(Ptree* exp)
  {
    if (exprEval()) {
      Ptree* e = PtreeUtil::Second(exp);
      Ptree* e2 = Translate(e);
      return e2;
    } else {
      return Walker::TranslateParen(exp);
    }
  }

  // This will evaluate constant expressions if we are in that mode (determined by the top value
  // in the constant-eval stack.  If so, then we handle basic expressions.  If either leaf is an
  // unsigned, we use unsigned arithmetic, otherwise we use signed arithmetic.
  Ptree *VarWalker::TranslateInfix(Ptree* exp)
  {
    Ptree* left = PtreeUtil::First(exp);
    Ptree* left2 = Translate(left);
    Ptree* right = PtreeUtil::Third(exp);
    Ptree* right2 = Translate(right);
    Ptree* op = PtreeUtil::Second(exp);

    if (Ptree *const_result = attemptExprEval(left2,op,right2)) {
      return const_result;
    }

    Ptree *exp2;
    // This occurs if we did not successfully evaluate a constant expression.
    if(left == left2 && right == right2)
      exp2 = exp;
    else
      exp2 = new PtreeInfixExpr(left2, PtreeUtil::List(PtreeUtil::Second(exp), right2));
    
    if (inLoopGuard()) {
      // If we're in a loop guard statement, analyze the expression and check to
      // see if we have a ceiling or floor expression for an existing loop
      // variable.
      Ptree *left  = First(exp2);
      Ptree *op    = Second(exp2);
      Ptree *right = Third(exp2);
      
      // Right now, we're just going to look at the lhs as the variable, so we won't catch the less
      // likely thing of 31 >= r, or something like that.
      check_loop_guard(left,op,right);
    }
    return exp2;
  }

  bool is_one(Ptree *x,Environment *env)
  {
    unsigned v;
    return (get_uint(v,x,env) && (v == 1));
  }

  // This checks to see if an incrementer or decrementer variable has a
  // loop-guard expression that we recognize.  If it's an incrementer, then we
  // expect a ceiling expression and if it's a decrementer, a floor expression.
  // Note that we're a little sloppy right now- we regard <= and < as the same
  // thing for fields, so we'll be off by one on our expression.  However, we
  // can tighten this up later, if it's really a problem.  Since we're just
  // tracking resource ranges here, it's probably not a big deal.
  void VarWalker::check_loop_guard(Ptree *left,Ptree *op,Ptree *right)
  {
    uint32_t v = 0;

    if (!left->IsLeaf()) {
      return;
    }

    // First, get the variable represented by left.  If it's not already marked as an
    // incrementer or decrementer, then we do nothing.
    if (!values()) return;
    auto var = values()->get(left,true);
    if (var) {

      bool ok = false;
      int adj = 0;

      if (var->is_incr()) {
      
        // If we have a ceiling function with an incrementer, then we consider this a valid range.
        if (Eq(op,"<=")) {
          ok = true;
        } else if (Eq(op,"<") || Eq(op,"!=")) {
          ok = true;
          adj = -1;
        }

      } else if (var->is_decr()) {

        // If we have a floor function with a decrementer, then we consider this a valid range.
        if (Eq(op,">=")) {
          ok = true;
        } else if (Eq(op,">") || Eq(op,"!=")) {
          adj = 1;
        }

      }

      if (ok) {
        VarVal &newvar = values()->add_to_top(left,*var);
        if (translate_const_expr(v,right)) {
          // If the loop looks like it has 0 iterations, mark it as an empty loop.
          if (var->has_value() && (var->_value == v) && (adj < 0)) {
            newvar.set_as_empty();
          } else {
            newvar._end = new VarVal(right,env,v+adj);
          }
        } else if (is_instrfield(right)) {
          newvar._end = new VarVal(right,env,0,VarVal::tField);
        } else {
          auto var = values()->get(right,true);
          if (var && (var->is_field() || var->is_parm())) {
            newvar._end = new VarVal(*var);
          }
        }
      }
    }
  }

  // This checks to see if an assignment is either a simple assignmeent of an integer or instruction field,
  // or else an increment/decrement type of statement, e.g. x = x + 1, etc.
  void VarWalker::check_variable(Ptree *left,Ptree *op,Ptree *right)
  {
    if (!left->IsLeaf()) {
      return;
    }
    uint32_t v = 0;
    if (!values()) return;
    if (Eq(op,"=")) {
      Ptree *x, *o, *y;
      if (get_uint(v,right,env)) {
        values()->add(left,right,env,v);
      } else if (is_instrfield(right)) {
        values()->add_field(left,right,env);
      } else if (Match(right,"[ %? %? %? ]",&x,&o,&y)) {
        VarVal::Loop l = VarVal::lNone;
        if (Eq(o,"+")) {
          l = VarVal::lIncr;
        } else if (Eq(o,"-")) {
          l = VarVal::lDecr;
        }
        if (l != VarVal::lNone) {
          if ( (Eq(left,x) && is_one(y,env)) || (Eq(left,y) && is_one(x,env)) ) {
            values()->mark_as_loop(left,l,env);
          }
        }
      } else {
        values()->mark_as_dirty(left,env);
      }
    } else if (Eq(op,"+=") || Eq(op,"-=")) {
      if (is_one(right,env)) {
        if (Eq(op,"+=")) {
          values()->mark_as_incr(left,env);
        } else {
          values()->mark_as_decr(left,env);
        }
      }
    } else {
      values()->mark_as_dirty(left,env);
    }
  }

  pair<Ptree*,Ptree *> handleSlice(Ptree *slice)
  {
    Ptree *slice_left, *slice_right;
    if (Match(slice,"[ %? , %? ]",&slice_left,&slice_right)) {
      return make_pair(slice_left,slice_right);
    } else if (Match(slice,"[ %? ]",&slice_left)) {
      return make_pair(slice_left,(Ptree*)0);
    } else {
      PError(slice,"Bad slice syntax:  Expected (x,y) or (x).");
    }
  }

  // This translates an assignment statement.  Note the order of evaluation of
  // left and right sides.  It is important that we translate the right side first
  // because we care about the order in which we scan source and target registers.
  // We want to see sources first, because we do not count as a source a register
  // that has already been a target.  If we scanned the left first, then we would
  // incorrectly view an expression such as "X = X - 1" as already having seen the
  // X as a target, and thus we wouldn't count it as a source.
  //
  // This also converts register assignments into PtreeRegAccess objects with
  // all associated data stored in the RegAccess data member.
  Ptree *VarWalker::TranslateAssign(Ptree *exp)
  {
    Ptree* op     = Second(exp);
    Ptree* right  = Third(exp);
    Ptree* right2 = Translate(right);
    Ptree *left   = First(exp);

    _in_assign.push_back(op);
    Ptree *left2 = Translate(left);
    _in_assign.pop_back();

    PtreeRegWrite *prw = 0;
    PtreeMemWrite *pmw = 0;

    // Check to see if this is a write to a register/register-file resource.  If
    // so, then we bundle the write object up so that it will encompass the
    // entire assignment expression (left and right sides).
    if ( (prw = dynamic_cast<PtreeRegWrite*>(left2)) ) {
      prw->_expr = right2;
      left2 = prw->Car();
    }

    // Similar situation for memory writes.
    if ( (pmw = dynamic_cast<PtreeMemWrite*>(left2)) ) {
      pmw->_expr = right2;
      left2 = pmw->Car();
    }


    // If we have an assignment of a constant to a variable, then we can store
    // its value.  Otherwise, we make sure to mark it as dirty in the hash.  We
    // only care about simple variable assignments here.  Also, we only do this
    // for direct assignments.  Operator assignmentrs, e.g. +=, mean that we
    // should mark the variable as dirty.  However, we do check for various
    // situations in which we have a simple increment or decrement, e.g. x = x +
    // 1, etc. and mark the variable as incremented/decremented, if that's the
    // case.  This still means that the variable is dirty, but we can use this
    // information if it's inside of a loop to see if we're accessing a range of
    // elements of a register-file.
    if (!inLoopBody2()) {
      check_variable(left2,op,right2);
    }

    Ptree *result;
    if(left == left2 && right == right2) {
      result = exp;
    } else {
      result = new PtreeAssignExpr(left2, List(Second(exp), right2));
    }
    if (prw) {
      prw->SetCar(result);
      return prw;
    } else if (pmw) {
      pmw->SetCar(result);
      return pmw;
    } else {
      return result;
    }
  }

  // If we have a ++ or --, then we set the appropriate variable to being
  // an incrementer or decrementer.
  void VarWalker::check_incr_or_decr(Ptree *oprnd,Ptree *op)
  {
    VarVal::Loop l = VarVal::lNone;
    if (Eq(op,"++")) {
      l = VarVal::lIncr;
    } else if (Eq(op,"--")) {
      l = VarVal::lDecr;
    }
    if (l != VarVal::lNone) {
      if (oprnd->IsLeaf() && values()) {
        values()->mark_as_loop(oprnd,l,env);
      }
    }
  }

  // If we're dealing with a ++ or --, then we have to mark that as making a variable dirty.
  Ptree *VarWalker::TranslateUnary(Ptree *exp)
  {
    auto exp2 = Walker::TranslateUnary(exp);

    Ptree *op2 = First(exp2);
    Ptree *left2 = Second(exp2);

    if (Ptree *const_result = attemptExprEval(left2,op2)) {
      return const_result;
    }

    if (!inLoopBody2()) {
      check_incr_or_decr(Second(exp2),First(exp2));
    }

    return exp2;
  }

  Ptree *VarWalker::TranslatePostfix(Ptree *exp)
  {
    auto exp2 = Walker::TranslateUnary(exp);

    if (!inLoopBody2()) {
      check_incr_or_decr(First(exp2),Second(exp2));
    }

    return exp2;
  }

  Ptree* VarWalker::TranslateReturn(Ptree *s)
  {
    if (!_return_okay) {
      PError(s,"return statements are not allowed here.");
    }
    
    // In case we're in a switch, this stops the processing of further items in
    // the switch block.
    if (inSwitch()) {
      set_case_take_next(false);
    }

    return Walker::TranslateReturn(s);
  }

  Ptree* VarWalker::TranslateWhile(Ptree* s)
  {
    _state.push_back(sLoopBody);
    Ptree* body = PtreeUtil::Nth(s,4);
    Ptree* body2 = Translate(body);
    _state.pop_back();

    _state.push_back(sLoopGuard);
    Ptree* cond = PtreeUtil::Third(s);
    Ptree* cond2 = Translate(cond);
    _state.pop_back();

    // Because this is a loop, we have to translate the body twice: The first
    // time updates captures all variable usage, marking any variables as dirty,
    // as necessary.  The second pass will then update any register usage to
    // reflect the fact that a variable was dirty after the register was first
    // seen, but since it's a loop, that dirty variable will then be used again
    // on the next pass.
    _state.push_back(sLoopBody2);
    body2 = Translate(body);
    _state.pop_back();

    if(cond == cond2 && body == body2)
      return s;
    else{
      Ptree* rest = PtreeUtil::ShallowSubst(cond2, cond, body2, body, s->Cdr());
      return new PtreeWhileStatement(s->Car(), rest);
    }
  }

  Ptree* VarWalker::TranslateDo(Ptree* s)
  {
    // Same justification as for while: We have to process this twice due to the
    // fact that it's a loop, so that statements which come after end up
    // affecting statements beforehand, within the body.
    _state.push_back(sLoopBody);
    Ptree* body = PtreeUtil::Second(s);
    Ptree* body2 = Translate(body);
    _state.pop_back();

    _state.push_back(sLoopGuard);
    Ptree* cond = PtreeUtil::Nth(s,4);
    Ptree* cond2 = Translate(cond);
    _state.pop_back();

    _state.push_back(sLoopBody2);
    body2 = Translate(body);
    _state.pop_back();

    if(cond == cond2 && body == body2)
      return s;
    else{
      Ptree* rest = PtreeUtil::ShallowSubst(body2, body, cond2, cond, s->Cdr());
      return new PtreeDoStatement(s->Car(), rest);
    }
  }

  Ptree* VarWalker::TranslateFor(Ptree* s)
  {
    newValueScope();
    NewScope();

    _state.push_back(sLoopBody);
    Ptree* exp1 = PtreeUtil::Third(s);
    Ptree* exp1t = Translate(exp1);

    Ptree* exp3 = PtreeUtil::Nth(s,5);
    Ptree* exp3t = Translate(exp3);

    Ptree* body = PtreeUtil::Nth(s,7);
    Ptree* body2 = Translate(body);
    _state.pop_back();

    _state.push_back(sLoopGuard);
    Ptree* exp2 = PtreeUtil::Nth(s,3);
    Ptree* exp2t = Translate(exp2);
    _state.pop_back();

    // Same justification as for while: We have to process this twice due to the
    // fact that it's a loop, so that statements which come after end up
    // affecting statements beforehand, within the body.
    _state.push_back(sLoopBody2);
    body2 = Translate(body);
    _state.pop_back();

    ExitScope();
    exitValueScope();

    if(exp1 == exp1t && exp2 == exp2t && exp3 == exp3t && body == body2)
      return s;
    else{
      Ptree* rest = PtreeUtil::ShallowSubst(exp1t, exp1, exp2t, exp2,
                                            exp3t, exp3, body2, body, s->Cdr());
      return new PtreeForStatement(s->Car(), rest);
    }
  }

  // This translates an argument with constant-expression evaluation turned on and
  // then updates it in the array.  If the result is an integer, it returns true,
  // otherwise false.
  bool VarWalker::translateConstArg(PtreeArray &array,int n,bool &changed)
  {
    _expr_eval.push_back(cConstEval);
    Ptree *p = getArg(array,n);
    if (!p) {
      return false;
    }
    Ptree *q = Translate(p);
    if (p != q) {
      setArg(array,n,q);
      changed = true;
    }
    _expr_eval.pop_back();
    unsigned x = 0;
    return get_uint(x,q,env);
  }

  void VarWalker::addUsedMem(const gc_string &mem,bool forced_write)
  {
    if (!_track_usage) return;

    assert(!_uinfo.empty());
    if (inAssign() || forced_write) {
      // We want to record various memory delays separately, so we insert a
      // delayed memory write with a dummy delay so that it won't match against
      // another item already in the set.

      if (inAssign() && delayedWrite()) {
        // Will be updated later with the actual expression. For now, save a
        // pointer.
        auto ip = _uinfo.back()._trg_mems.insert(UsedMem(mem,temp_delay));
        _delayed_write.back()._dm = &(*ip.first);
      } else {
        _uinfo.back()._trg_mems.insert(UsedMem(mem,0));
      }
    } else {
      _uinfo.back()._src_mems.insert(mem);
    }
  }

  void VarWalker::addUsedCache(const gc_string &c)
  {
    if (!_track_usage) return;

    assert(!_uinfo.empty());
    if (inAssign()) {
      _uinfo.back()._trg_caches.insert(c);
    } else {
      _uinfo.back()._src_caches.insert(c);
    }
  }

  bool check_for_exclude(const UsedReg &ur,const ResExcludes *excludes)
  {
    if (excludes) {
      IfFind(excl,(*excludes),ur) {
        excl->_handled = true;
        return true;
      }
    }
    return false;
  }

  void remove_excludes(UsedRegs &regs,ResExcludes *excludes)
  {
    if (excludes) {
      auto iter = regs.begin();
      while (iter != regs.end()) {
        if (check_for_exclude(*iter,excludes)) {
          auto tmp = iter++;
          regs.erase(tmp);
        } else {
          ++iter;
        }
      }
    }
  }

  UsedRegs::iterator addNonFixedReg(UsedRegs &regs,const UsedReg &ur)
  {
    auto ip = regs.insert(ur);
    if (ip.second) {
      // Was inserted- scan for fixed entries to the same register file.
      for ( UsedRegs::iterator iter = regs.begin(); iter != regs.end(); ) {
        if (const RegFile *rf = iter->regfile()) {
          if (iter->is_fixed() && rf->name() == ur.regb()->name()) {
            auto d = *iter++;
            regs.erase(d);
            continue;
          }
        }
        iter++;
      }
    }
    return ip.first;
  }

  // When inserting a target, we first check to see if we can find a match for
  // just the register itself against a non-fixed item.  If so, then we don't
  // bother inserting.  Otherwise, if the item being added is not-fixed, then we
  // must remove any existing entries that are to that same register file but
  // are fixed.  The idea is that if GPR(X) should be collapsed into an entry
  // for just GPR.  Note that in this case, we consider a range to be more of a
  // fixed-item than a non-fixed item, since a range can't be guaranteed to
  // include all elements of a register-file.
  UsedRegs::iterator VarWalker::insertUsedReg(UsedRegs &regs,const UsedReg &ur)
  {
    if (ur.is_fixed() || ur.is_range()) {
      // Entry being added is fixed, so ignore it if we have a non-fixed entry
      // already.  This only applies to register-files.
      auto iter = regs.end();
      if (ur.regfile()) {
        UsedReg tmp(ur.regb());
        IfFind(i,regs,tmp) {
          iter = i;
        }
      }
      if (iter == regs.end()) {
        auto ip = regs.insert(ur);
        if (!ip.second) {
          // Not inserted, so just update mask if the mask is in use.  If not
          // used, then clear the mask.
          ip.first->update_mask(ur);
          // Clear the conditional flag.  The rationale is that, at this point,
          // we know that we're targeting this resource, so it's no longer
          // really conditional.  If we're in a conditional block, then we'll
          // set this flag within the translate-if function.
          ip.first->_cond = false;
        }
        iter = ip.first;
      }
      return iter;
    } else {
      // Entry being added is not fixed, so if it's added, then scan for and remove
      // any items with the same register-file as a target.
      return addNonFixedReg(regs,ur);
    }
  }

  void VarWalker::update_regs(Ptree *loc,UsedRegList &parms,UsedRegs &dest,UsedRegs &src,PtreeArray &args)
  {
    ForEach (src,iter) {
      UsedReg ur(*iter);
      
      // Does this reference a parameter?  If so, clear that state and set it in
      // the list of parms which will be stored with the call itself.
      if (ur.is_parm()) {
        int p = ur._parm;
        ur.clear_parm();

        // The size of the array might be too small due to the use of default
        // parameter values.  If so, then resize here.
        if (p >= (int)parms.size()) {
          parms.resize(p+1);
        }
        parms[p] = ur;

        // One extra check: If this is itself a parameter, then we need to make
        // sure that it's tracked.  E.g. if we're in a helper and we've just
        // called a helper using an argument that is itself a parameter, then we
        // want to make sure we don't lose track of the fact that this is a
        // parameter.
        VarVal *v = 0;
        if (values() && (v = values()->get(getArg(args,p)))) {
          if (v->is_parm()) {
            ur._parm = v->_pindex;
          }
        }
      }

      insertUsedReg(dest,ur);
    }
  }

  // Try to extract an index, for various situations.
  Ptree *get_index(Ptree *index)
  {
    if (index->What() == ntInfixExpr || index->What() == ntPostfixExpr) {
      return First(index);
    }
    else if (index->What() == ntUnaryExpr) {
      return Second(index);
    }
    else {
      return index;
    }
  }

  // If a numeric value exists, then we want to use that, since it might have
  // been adjusted, if we're dealing with a loop variable.
  Ptree *get_value_from_var(const VarVal &var)
  {
    switch (var._type) {
    case VarVal::tVar:
      return Ptree::Make("%d",var._value);
    default:
      return var._orig;
    }
  }

  // This returns false if it looks like we have a non-access caused by an empty
  // loop.
  bool VarWalker::update_if_fixed(UsedReg &ur,Ptree *index)
  {
    // First, does the register file reference a variable that has a known value?
    // If so, then we can consider that fixed.
    // Generally, we require the index to be a simple variable, or else we
    // assume that we're not dealing with a fixed element.  The exception is for
    // loops.  This could be expanded later to handle things like a series of
    // postfix operators, or something like that.
    VarVal *v = 0;
    if (values() && (v = values()->get(get_index(index)))) {
      if (v->is_empty()) return false;
      // It's a fixed value, parm, or instruction field.
      ur._begin = get_value_from_var(*v);
      if (v->is_parm()) {
        // We use a parameter.
        ur._parm = v->_pindex;
      }
      if (v->is_range()) {
        // We've got a range variable to deal with.
        ur._end = get_value_from_var(*v->_end);
      }
      return true;
    }
    
    // Otherwise, check the expression to see if it's fixed (consists of only constants
    // and instruction fields, if applicable).
    ResWalker w(_instr);
    w.Translate(index);
    if (w.is_fixed()) {
      ur._begin = index;
    }
    return true;
  }

  void VarWalker::addUsedReg(UsedReg ur)
  {
    if (!_track_usage) return;

    assert(!_uinfo.empty());
    if (!ur.regb()) {
      return;
    }

    Ptree *op = 0;
    if ( (op = inAssign() ) ) {
      if ( ur.reg() && !Eq(op,"=")) {
        PError(op,"We only allow simple assignments to registers.  Arithmetic operator assignments are not allowed.");
      }
      if ( ur.regfile() && !Eq(op,"=") ) {
        PError(op,"We only allow simple assignments to register files.  Arithmetic operator assignments are not allowed.");
      }
    }

    if (directAccess()) {
      ur._direct = true;
    }

    if (op) {
      // Ignore if this is an item which should be excluded.
      if (check_for_exclude(ur,_trg_excludes)) {
        return;
      }

      // This means we're in the lhs of an assignment statement and thus are
      // dealing with a target register. 
      auto iter = insertUsedReg(_uinfo.back()._trg_regs,ur);

      if (delayedWrite()) {
        // Will be updated later with the actual expression. For now, save a
        // pointer.
        _delayed_write.back()._dr = &(*iter);
      }

    } else {
      // Is this source to be excluded?  If so, then don't add it.  Also, update
      // the _cond flag to indicate that this source was found.  This is used
      // for error checking to make sure that the user didn't add anything
      // that's never used as a source.
      if (check_for_exclude(ur,_src_excludes)) {
        return;
      }

      // We're in the rhs of an assigment, so it's a source.  If this register
      // is already a target, then we don't add it as a source register, since
      // it will already contain a new value, and thus isn't really a required
      // source.  However, if the target is a partial write, then we do add it
      // as a source and change the partial-write target to a full-write target.
      // For example, let's suppose we have an instruction which reads GPR(X)
      // and then writes to a portion of GPR(X).  We need to make sure that we
      // read the complete value of GPR(X) so that we can process it.  Since
      // we've already read that value, we then want to write back the complete
      // value.  One other corner case: If this source already exists, then we
      // update its mask in order to keep that up-to-date.
      auto trg = _uinfo.back()._trg_regs.find(ur);
      if (trg == _uinfo.back()._trg_regs.end()) {
        insertUsedReg(_uinfo.back()._src_regs,ur);
      } else if (trg->partial()) {
        insertUsedReg(_uinfo.back()._src_regs,ur);
        trg->_partial = false;
      } else {
        IfFind(src,_uinfo.back()._src_regs,ur) {
          src->update_mask(ur);
        }
      }
    }
  }

  // We have to take into account whether the core is in big-endian or
  // little-endian mode in order to correctly set the bits.
  void VarWalker::setRNumberField(RNumber &x,unsigned l,unsigned r)
  {
    RNumber tmp(0,x.size());
    tmp.setAll();
    if (_core && _core->is_little_endian()) {
      x.setField(x.size()-l-1,x.size()-r-1,tmp);
    } else {
      x.setField(l,r,tmp);
    }
  }

  // Return false if, for some reason, we should ignore this slice.
  bool VarWalker::ignore_slice(const UsedReg &ur,Ptree *left,Ptree *right)
  {
    // Ignore the slice if it matches the current ThisReg and the ignore-slice
    // matches, but only if it's actually set to a value.  A value of (0,0)
    // means that we skip this test.
    if (_slice_ignore.first == 0 && _slice_ignore.second == 0) 
      return false;

    return (ur.regb() == _cur_reg && Eq(left,_slice_ignore.first) && Eq(right,_slice_ignore.second));
  }

  // Updates the partial access mask of a UsedReg object.  Returns false if, for
  // some reason, the update should be skipped.
  bool VarWalker::update_partial_mask(UsedReg &ur,Ptree *portion,Ptree *portion_arg)
  {
    if (!portion) {
      return true;
    }

    assert(ur.regb());
    
    Ptree *left,*right;
    const char *field = 0;

    if (Match(portion,"[ %? , %? ]",&left,&right)) {
      // First, does the portion correspond to a slice, e.g. (x,y)?
      // If so, try to evaluate the expressions to get constant values.

      if (ignore_slice(ur,left,right)) {
        return false;
      }


      unsigned l_u,r_u;
      bool have_l = translate_const_expr(l_u,left);
      bool have_r = translate_const_expr(r_u,right);

      if (have_l && have_r) {
        // If the width is set, then constrain to the specified width in order
        // to emulate the run-time behavior.
        if (ur.regb()->width()) {
          l_u %= ur.regb()->width();
          r_u %= ur.regb()->width();
        }
        setRNumberField(ur._mask,l_u,r_u);
      }
    }
    else if (Match(portion,"[ %? ]",&left)) {
      // Single-bit access.

      unsigned c_u;
      if (translate_const_expr(c_u,left)) {
        if (ur.regb()->width()) {
          c_u %= ur.regb()->width();
        }
        setRNumberField(ur._mask,c_u,c_u);
      }
    }
    else if ( (field = get_simple_ident(portion,0)) ) {
      // If it's an identifier, then treat as a field.
      if (auto f = ur.regb()->get_field(field)) {
        // Is the register field indexed?  If so, then we can only determine
        // bits used if we can resolve the argument.  Otherwise, we count the
        // register as fully used and do not touch the mask.
        if (f->indexed()) {
          // Is indexed.  Can we resolve the expression to a constant?
          unsigned r;
          if (translate_const_expr(r,portion_arg)) {
            assert(!f->bits().empty());
            unsigned start = r*f->bits()[0].width();
            unsigned stop = start + f->bits()[0].width() - 1;
            setRNumberField(ur._mask,start-ur.regb()->offset(),stop-ur.regb()->offset());
          }
        } else {
          // Non-indexed, so just set the specific bits.
          ForEach(f->bits(),bp) {
            setRNumberField(ur._mask,bp->l()-ur.regb()->offset(),bp->r()-ur.regb()->offset());
          }
        }
      }
    }

    ur.check_if_still_partial();

    return true;
  }

  void VarWalker::update_parm_conv(UsedReg &ur)
  {
    if (Ptree *h_arg = nestedHelperArg()) {
      // Are we making a call to a helper?  If so, then check to see if we have
      // an implicit cast which might be reducing the scope of the read.
      unsigned size;
      if ( (size = bits_size(h_arg,env))) {
        if (size < ur.regb()->width()) {
          // The size of the parm is smaller than the size of the register, so
          // just use the portion that'll be used by the parm.
          setRNumberField(ur._mask,ur.regb()->width()-size,ur.regb()->width()-1);
        }
      }
    }
  }

  // Checks a register-file access to see what resource are used to compute the index (amongst other things).
  // Returns the resource expression used by the index, or just the original index, if it cannot be determined or it's
  // a loop.
  Ptree *VarWalker::checkRegFileAccess(const RegFile *regfile,Ptree *index,bool partial,Ptree *portion,Ptree *portion_arg)
  {
    // We don't bother analyzing register file accesses on the first pass
    // through a loop body b/c we'll do it on the second pass, when we've
    // figured out if any variables are loop-counter variables.
    if (!inLoopBody()) {
      UsedReg ur(regfile);
      ur._partial = partial;

      // Just return the index if it looks like this doesn't actually cause a
      // real register-file access, e.g. we're in a loop that has 0 iterations.
      if (!update_if_fixed(ur,index)) return index;
      if (update_partial_mask(ur,portion,portion_arg)) {

        // For now, only handle parameter-passing effects for non-partial accesses.
        if (!portion) {
          update_parm_conv(ur);
        }
      
        addUsedReg(ur);
      }

      return (ur._begin && !ur._end) ? ur._begin : index;
    } else {
      return index;
    }
  }

  void VarWalker::checkCtxAccess(const Context *ctx,Ptree *index,const RegBase *r)
  {
    uint32_t ix;
    if ( get_uint(ix,index,env) ) {
      if (ix >= (uint32_t)ctx->num_contexts() ) {
        PError(index,"Context index of " << index << " is out of range [0," << ctx->num_contexts()-1 << "].");
      }
    }

    if ( ! ctx->contains(*r) ) {
      PError(index,r->name() << " is not a member of the context " << ctx->name());
    }

    if (r->pseudo()) {
      PError(index,r->name() << " is a pseudo register and thus may not be accessed directly through a context.");
    }
  }

  // This checks that an access to a context is valid.  If so, it updates the
  // PtreeRegRead or PtreeRegWrite object, which should exist in the data model,
  // with the relevant context information.
  void VarWalker::checkCtxAccess(const Context *ctx,Ptree *index,Ptree *member)
  {
    if (Length(index) == 0) {
      PError(member,"No argument supplied for context access.");
    }

    // The member must be a register or register-file and must be a member of the context.
    const RegBase *r;
    if ( (r = get_reg(member)) ) {
      ;
    } else if ( (r = get_regfile(member)) ) {
      ;
    } else {
      PError(member,"Context element must be a register or register file.");
    }

    checkCtxAccess(ctx,index,r);
  }

  // Given a list of arguments to a function, this tries to get values for these
  // arguments.  If they're constants, then there's no change.  If they're
  // variables, then we try and look up their value.  If we can't get a fixed
  // value, then we just pass a dummy token in order to make sure that the item
  // is regarded as an unknown value.
  void VarWalker::get_arg_values(PtreeArray &arg_values,PtreeArray &args)
  {
    bool skip = false;
    ForRange(args.Number(),i) {
      Ptree *arg = args[i];

      unsigned v;
      bool b;
      VarVal *var = 0;

      if (skip) {
        // This is for skipping the ',' in the arg list.
        arg_values.Append(arg);
        skip = false;
        continue;
      }

      if (get_uint(v,arg,env)) {
        // It's a number, so pass the value on.
        arg_values.Append(arg);
      }
      else if (get_bool(b,arg,env)) {
        // It's a boolean, so pass the value on.
        arg_values.Append(arg);
      } 
      else if (values() && (var = values()->get(arg))) {
        //        values()->Display();
        // It has a fixed value, so pass that on, instead.
        if (var->has_value()) {
          arg_values.Append(make_leaf<LeafUNSIGNED>(var->_value));
        } else {
          arg_values.Append(var->_orig);
        }
      } 
      else if (is_instrfield(arg)) {
        arg_values.Append(arg);
      }
      else if (is_field_func_call(arg)) {
        arg_values.Append(arg);
      }      
      else {
        // See if this is an expression consisting of just instruction fields
        // and constants.  If so, then we allow it.
        ResWalker w(_instr);
        w.Translate(arg);
        if (w.is_fixed()) {
          arg_values.Append(arg);
        } else {
          // Unknown value- pass along dummy token.
          arg_values.Append(unknown_parm);
        }
      }
      skip = true;
    }
  }

  Ptree *VarWalker::checkHelper(Ptree *fun,PtreeArray &args,HelperInfo *hinfo,const gc_string &name)
  {
    // If this is a helper function, process it in order to get its usage info.
    if (hinfo) {
      if (!_track_usage) return fun;

      // This is a helper function, so record its use.
      _uinfo.back()._helpers.insert(name);

      PtreeArray arg_values;
      get_arg_values(arg_values,args);

      // Process the helper using the arguments for this call.  This allows us
      // to eliminate resources which won't be touched by this instruction by
      // doing some limited constant-expression evaluation.
      HelperInfo htmp;

      if (_nccore) {
        _nccore->process_helper(&htmp,*hinfo,name,&arg_values,_instr,StrSet(),false);
      }

      // Now we have to take its usage info and add it to our own.
      // First, handle all non-reg stuff.
      _uinfo.back().update_non_regs(htmp);
      UsedRegList parms;

      parms.resize(numArgs(args));
      // For the registers, we have to process that stuff by looking at anything
      // which references a parameter, then see if the relevant parameter is
      // constant in the call.  If it's not constant, then we add a reference to
      // the whole register file.  Otherwise, we add a reference to the
      // appropriate item.
      update_regs(fun,parms,_uinfo.back()._src_regs,htmp._src_regs,args);
      _uinfo.back()._src_regs.update_mutables(htmp._src_regs);
      remove_excludes(_uinfo.back()._src_regs,_src_excludes);

      update_regs(fun,parms,_uinfo.back()._trg_regs,htmp._trg_regs,args);
      _uinfo.back()._trg_regs.update_mutables(htmp._trg_regs);
      remove_excludes(_uinfo.back()._trg_regs,_trg_excludes);

      // Finally, replace its call object so that we can extract and easily
      // translate its usage information.
      return (fun->IsLeaf()) ? (Ptree*)new PtreeHelperCallLeaf(fun,parms) : (Ptree*)new PtreeHelperCallNonLeaf(fun,parms);
    }
    
    // Not a helper.
    return 0;
  }

  // The default version doesn't translate the function body, which we need to
  // do in order to track resource usage.
  Ptree* VarWalker::TranslateTemplateFunction(Ptree* temp_def, Ptree* fun)
  {
    env->RecordTemplateFunction(temp_def, fun);
    // We want to record the template parameters in a new scope so that we'll
    // recognize them as valid symbols within the body of the function.
    NewScope();
    TranslateArgDeclList(true,temp_def,Third(temp_def));
    Ptree *fun2 = Translate(fun);
    ExitScope();
    if (fun2 == fun) {
      return temp_def;
    } else {
      Ptree *rest = PtreeUtil::ShallowSubst(fun2,fun,temp_def->Cdr());
      return new PtreeTemplateDecl(temp_def->Car(),rest);
    }
  }

  void VarWalker::record_parm(Ptree *args,int index)
  {
    if(args == 0) {
      return;
    } else {
      Ptree *arg;
      arg = args->Car();
      Ptree *rest = args->Cdr();
      if (rest) {
        record_parm(rest->Cdr(),index+1);
      }
      Ptree *decl = Second(arg);

      // See if there's a default parameter.
      uint32_t dvi = 0;
      bool dvb = false;
      Ptree *dvalue = 0;
      if(Eq(Third(arg), '=')) {
        dvalue = Nth(arg,3);
      }

      if (PtreeDeclarator *ntd = dynamic_cast<PtreeDeclarator*>(decl)) {
        if (ntd->Name() && values()) {
          // We only add this if the parameter has a name.  A nameless
          // parameter, of course, can't be referenced, so it won't ever be
          // used.
          VarVal &var = values()->add_parm(ntd->Name(),index,env);
          // If we have parameter values, then associate a value with it.
          if (_func_args) {
            // If we don't have the right number of parameters, then this might
            // be due to the use of default parms.
            if (Ptree *av = getArg(*_func_args,index)) {
              if (av == unknown_parm) {
                var.set_dirty();
              } else {
                var._orig = av;
                bool b;
                unsigned u;
                if (get_uint(u,av,env)) {
                  var.set_value(u);
                } else if (get_bool(b,av,env)) {
                  var.set_value(b);
                }
              }
            } else if (dvalue) {
              // Parameter size mismatch, so check to see if we have a default
              // parameter value.
              if (get_uint(dvi,dvalue,env)) {
                var.set_as_var(dvi);
              } else if (get_bool(dvb,dvalue,env)) {
                var.set_as_var(dvb);
              }
            }
          }
        }
      }
    }
  }

  Ptree* VarWalker::TranslateFunctionImplementation(Ptree* impl)
  {
    newValueScope();
    Ptree *impl2 = Walker::TranslateFunctionImplementation(impl);
    exitValueScope();
    return impl2;
  }

  // Here, we need to record the parameters so that we can check for them later
  // when encountering them as indexing into register files.
  // For now, we don't record template parameters.  We may want to add this in the
  // future, if users are indexing register files using template parameters.
  Ptree* VarWalker::TranslateArgDeclList(bool record, Ptree*decl, Ptree* args)
  {
    bool is_template = Eq(decl->Car(),"template");
    Ptree *args2 = Walker::TranslateArgDeclList(record,decl,args);
    if (!is_template && record) {
      record_parm(args2,0);
    }
    return args2;
  }

  // This could be optimized a bit- we've already parsed these things.  However,
  // for simplicity, we just parse it again.  This only occurs when we hit
  // helper functions multiple times.
  Ptree *VarWalker::TranslateRegWrite(PtreeRegWrite *exp)
  {
    return Translate(exp->Car());
  }

  Ptree *VarWalker::TranslateRegRead(PtreeRegRead *exp)
  {
    return Translate(exp->Car());
  }

  Ptree *VarWalker::TranslateMemWrite(PtreeMemWrite *exp)
  {
    return Translate(exp->Car());
  }

  Ptree *VarWalker::TranslateMemRead(PtreeMemRead *exp)
  {
    return Translate(exp->Car());
  }

  Ptree* VarWalker::TranslateAssignInitializer(PtreeDeclarator*decl, Ptree* init)
  {
    Ptree* exp = PtreeUtil::Second(init);
    Ptree* exp2 = Translate(exp);
    
    Ptree *n = First(decl);
    if (n->IsLeaf() && values()) {
      unsigned v;
      if ( get_uint(v,exp2,env)) {
        // We've got a numeric initialization, so store the value here.
        values()->add(n,exp2,env,v);
      } else if (is_instrfield(exp2) || is_field_func_call(exp2)) {
        // TODO: We've either got an instruction field or a function call whose
        // arguments are constants and instruction fields.  In that case, we
        // assume the function is constant and propagate its value.  If it's not
        // constant, then we don't catch that situation right now.  We should
        // tighten this up in the future.
        values()->add_field(n,exp2,env);
      } else {
        values()->add_dirty(n,env);
      }
    }

    if(exp == exp2)
      return init;
    else
      return PtreeUtil::List(init->Car(), exp2);
  }

  // Fix the parsing error, whereby register-file slices end up looking like
  // declarations.  We rebuild them as function calls in an assignment
  // statement.
  Ptree *buildRegFileSliceAssign(Ptree *rf,Ptree *right,Ptree *index,Ptree *op,Ptree *expr,Ptree *sc)
  {
    Ptree *rl = First(right);
    Ptree *index_args = First(rl);
    if (index->IsA(BadToken)) {
      Ptree *index2 = new LeafName((LeafName&)*index);
      index_args = Subst(index2,index,index_args);
    }

    Ptree *slice_args = Second(rl);

    return new PtreeExprStatement(new PtreeAssignExpr( new PtreeFuncallExpr(new PtreeFuncallExpr(rf,index_args),slice_args),
                                                       List(op,expr)),List(sc));
  }

  // There appears to be a bug in the parser whereby a complex assignment
  // can be misinterpreted as a declaration.  We deal with that case here.
  // The issue is with something of the following form:
  //
  // GPR(RT)(x,y) = ...;
  //
  // This is a write to a slice of a register file element.
  Ptree* VarWalker::TranslateDeclaration(Ptree* def)
  {
    Ptree *left = Second(def);
    const RegFile *regfile;
    // We want to translate the left (using the default operation) first so that
    // any source registers are scanned first.  Then we'll handle the lhs so
    // that targets are processed afterwards.
    Ptree *result = Walker::TranslateDeclaration(def);
    PtreeRegWrite *pw = 0;
    Ptree *op, *expr;
    if (left && (regfile = get_regfile(left))) {    
      Ptree *right = Third(result);      
      Ptree *index,*slice;
      if (Match(right,"[ [ [( [%?] )] [( %? )] %? %? ] ]",&index,&slice,&op,&expr) ) {
        _in_assign.push_back(op);
        Ptree *rindex = checkRegFileAccess(regfile,index,true,slice);
        _in_assign.pop_back();
        Ptree *slice_left,*slice_right;
        if (Match(slice,"[ %? , %? ]",&slice_left,&slice_right)) {
          pw = new PtreeRegWrite(buildRegFileSliceAssign(left,right,index,op,expr,Nth(def,3)),is_cur_reg(regfile));
          pw->_regaccess._regfile = regfile;
          pw->_regaccess._index   = index;
          pw->_regaccess._rindex  = rindex;
          pw->_regaccess._left    = slice_left;
          pw->_regaccess._right   = slice_right;
          pw->_expr               = expr;
        } else if (Match(slice,"[ %? ]",&slice_left)) {
          pw = new PtreeRegWrite(buildRegFileSliceAssign(left,right,index,op,expr,Nth(def,3)),is_cur_reg(regfile));
          pw->_regaccess._regfile = regfile;
          pw->_regaccess._index   = index;
          pw->_regaccess._rindex  = rindex;
          pw->_regaccess._left    = slice_left;
          pw->_expr               = expr;
        } else {
          PError(def,"Malformed slice:  Expected two indices, e.g. (x,y).");
        }
      }
    }
    if (pw) {
      // This handles the semicolon.
      if (_nccore) _nccore->pushRegAccess(pw);
      return pw;
    } else {
      return result;
    }
  }

  // If we're processing arguments of a function call and we have a nested
  // helper, then put the formal parameter here.
  //
  // If we're processing a delayed write, then after the first argument, switch
  // off treating it as a target.
  void VarWalker::setCurArg(unsigned index)
  {
    if (delayedWrite() && index == 1) {
      _in_assign.pop_back();
      _in_assign.push_back(0);
    }
    if (auto hinfo = nestedHelper()) {
      int nth = index;
      Ptree *arg = NthDeclarator(Third(hinfo->_func),nth);
      // For consistancy of doneWithArg(), always add something, even if it's 0.
      Ptree *decl = (arg && !arg->IsLeaf()) ? Second(arg) : 0;
      _nested_helper_arg.push_back(decl);
    }
  }

  void VarWalker::doneWithArg()
  {
    if (nestedHelper()) {
      assert(!_nested_helper_arg.empty());
      _nested_helper_arg.pop_back();
    }
  }

  // Handles a memory access, extracting out parameters common to reads and writes.
  PtreeMemRead *VarWalker::handleMemAccess(Ptree *exp,const gc_string &fname,PtreeArray &array)
  {
    int num_args = numArgs(array);
    if (!(num_args == 2 || num_args == 3 || num_args == 5)) {
      PError(exp,"A memory access expects two, three, or five arguments:  The address, the access size, and an optional type specifier and flag/critical address.");
    }
    if (_nccore) {
      // Record the fact that we do use this type of memory.
      switch (num_args) {
      case 2:
        _nccore->set_typed_mem(Core::BasicMem);
        break;
      case 3:
        _nccore->set_typed_mem(Core::TypedMem);
        break;
      case 5:
        _nccore->set_typed_mem(Core::CritWordMem);
        break;
      }
    }
    bool changed = false;
    if (!translateConstArg(array,1,changed)) {
      PError(exp,"Second argument to a memory access must be a constant integer.");
    }
    addUsedMem(fname);

    PtreeMemRead *pm = make_mem_obj(exp);

    pm->_memaccess._mem   = get_mem(fname);

    get_uint(pm->_memaccess._size,array[2],env);

    pm->_memaccess._addr = array[0];
    
    if (num_args >= 3) {
      pm->_memaccess._type = array[4];

      if (num_args == 5) {
        pm->_memaccess._cw_addr = array[8];
      }
    }

    return pm;
  }


  Ptree* VarWalker::TranslateFuncall(Ptree* exp)
  {
    assert(!_uinfo.empty());

    // If we're referencing Mem, then we should have two arguments, the second of which
    // is a constant integer.
    Ptree *fun = exp->Car();
    Ptree *fun2 = 0;

    // If we have a template-function call, then we want to just extract the
    // name only.
    gc_string fname = extractBaseFuncName(fun);

    Ptree *arglist = exp->Cdr();
    Ptree *args    = Second(arglist);

    PtreeArray array;
    bool changed = false;
  
    const RegFile *rf = 0;

    Ptree *reg, *field, *regfile,*index;
    Ptree *ctx,*ctx_index,*creg;

    // Check to see if the call is to a helper.  If so, then we want to record
    // this helper so that nested resource usage can examine the helper's
    // parameters in order to see if implicit casting causes a change in
    // resource usage.  The usual example is a larger register access converted
    // to a smaller access, which means that upper bits are not needed.  This is
    // equivalent to a partial read.
    HelperInfo *hinfo = 0;
    if (_core) {
      if ((hinfo = _core->get_helper(fname))) {
        _nested_helper.push_back(hinfo);
      }                              
    }
    if (!hinfo) {
      // This prevents further processing within nested calls.
      _nested_helper.push_back(0);
    }
    //cout << "Nested helper depth:  " << _nested_helper.size() << endl;

    bool dw = false;
    if (Eq(fun,"delayed_write")) {
      if (!inAssign()) {
        PError(exp,"delayed_write() may only appear on the left-hand-side of an assignment.");
      }
      dw = true;
    } 
    _delayed_write.push_back(DWInfo(dw));

    bool da = false;
    if (Eq(fun,"direct_access")) {
      da = true;
    } 
    _direct_access.push_back(da);

    PtreeRegRead *pr = 0;
    PtreeMemRead *pm = 0;
    Ptree *replace = 0;

    // Populate an array of the arguments and translate.  We do not consider
    // this translation to be part of the assignment, since we wouldn't want to
    // consider these as targets.  The exception to this is the delayed_write call,
    // in which the first argument is the target of the assignment.
    if (dw) {
      _in_assign.push_back(Ptree::Make("="));
      populateArgs(array,this,exp,this,changed);
      _in_assign.pop_back();

      if (numArgs(array) != 2) {
        PError(exp,"Expected two arguments for delayed_write:  delayed_write(<reg-expr | mem-expr>,<delay>).");
      }
      // Store the delay- add to the relevant RegAccess object.
      Ptree *delay = getArg(array,1);

      // The first argument must be a register or memory expression.  We'll update it with
      // the delay information.
      if ( (pr = dynamic_cast<PtreeRegWrite *>(getArg(array,0)))) {
        array[0] = pr->Car();
        replace = pr;
      } else if ( (pm = dynamic_cast<PtreeMemWrite *>(getArg(array,0)))) {
        array[0] = pm->Car();
        replace = pm;
      } else {
        PError(exp,"Invalid register expression as first argument of delayed_write.");
      }

      // Update both the register-write/memory-write object and the used-reg object to have
      // the proper delay expression.
      if (pr) {
        pr->_regaccess._delay = delay;
        if (const UsedReg *dur = _delayed_write.back()._dr) {
          // We have to guard this, as it may be 0 if we're not currently tracking
          // resource usage.
          dur->_delay = delay;
        }
      } else if (pm) {
        pm->_memaccess._delay = delay;
        if (const UsedMem *dum = _delayed_write.back()._dm) {
          // We have to guard this, as it may be 0 if we're not currently tracking
          // resource usage.
          dum->_delay = delay;
        }
      }   

    } else if (da) {
      // Direct access- may be in an assignment or not, since either is valid.
      populateArgs(array,this,exp,this,changed);
      if (numArgs(array) != 1) {
        PError(exp,"Expected one argument for direct_access:  direct_access(<reg-expr>).");
      }
      // The first argument must be a register expression.  We'll update it with
      // the fact that it's a direct access.  Note:  Writes are subclassed from reads, so we just
      // want to get a read here, since it can be either a read or a write.
      if (!(pr = dynamic_cast<PtreeRegRead *>(getArg(array,0)))) {
        PError(exp,"Invalid register expression as first argument of direct_access:  " << getArg(array,0));
      }
      array[0] = pr->Car();
      pr->_regaccess._direct = true;
      
      replace = pr;
    } else {
      // Normal case- force a non-assignment situation.
      _in_assign.push_back(0);
      populateArgs(array,this,exp,this,changed);
      _in_assign.pop_back();
    }
    _delayed_write.pop_back();
    _direct_access.pop_back();

    if (Eq(fun,"zero")) {
      // Zero's argument must be a constant integer.
      if (!translateConstArg(array,0,changed)) {
        PError(exp,"Argument to Zero must be a constant integer.");
      }
    } else if (Eq(fun,"instrHasAttr")) {
      // The instruction-attribute test function takes a single argument, which
      // must be a valid attribute.
      int numargs = numArgs(array);
      if (numargs > 2) {
        PError(exp,"instrHasAttr takes either one or two arguments.");      
      }
      int attr_arg = (numargs == 2) ? 1 : 0;
      Ptree *arg1 = getArg(array,attr_arg);
      gc_string attr = get_str(arg1,"instrHasAttr argument.");
      int a_id = Data::get_attr(attr);
      if (a_id < 0) {
        PError(exp,"Unknown attribute '" << attr << "' in instrHasAttr call.");
      }
      // We also record the fact that this attribute is used within an
      // instruction-attribute query.
      if (_nccore) {
        _nccore->add_instr_attr(a_id);
      } else {
        PError(exp,"An instruction attribute may only be queried within a core.");
      }
    } else if (Eq(fun,"setMicroOpOffset")) {
      // If we see that this function is called with just an instruction field,
      // then we can use that in the decode cache to do direct threading.  So,
      // we evaluate that here in case we want to use that information.
      int numargs = numArgs(array);
      if (numargs != 1) {
        PError(exp,fun <<" takes only one argument.");      
      }

      if (!_instr) {
        PError(exp,fun << " may only be called within an instruction's action code.");
      }

      Ptree *arg = getArg(array,0);
      if (is_instrfield(arg)) {
        // We know _instr is set at this point.
        _mo_offset_field = arg->ToString();
      }
      
      if (_nccore) {
        _nccore->set_has_micro_op_offset();
      } else {
        PError(exp,"The micro-op mask may only be set within a core.");
      }
    } else if (Eq(fun,"signExtend") || Eq(fun,"zeroExtend") ) {
      // Check to make sure that the signExtend function's second argument is a constant.
      if (!translateConstArg(array,1,changed)) {
        PError(exp,"Second argument to extend functions must be a constant integer or constant-integer expression.");
      }
    } else if (Eq(fun,MmuName)) {
      if (numArgs(array) != 3) {
        PError(exp,"Mmu expects three arguments:  Destination array, set, and way.");
      }
      Ptree *arg1 = getArg(array,0);
      if (const char *n = get_str(arg1,0)) {
        if (_core && _core->mmu()) {
          if (!_core->mmu()->get_any_mmulookup(n)) {
            PError(exp,"Unknown lookup name in argument to MMU function:  " << n);
          }
        } else {
          PError(exp,"Call to MMU access function but no MMU is defined for this core.");
        }
      } else {
        PError(exp,"Mmu expects its first argument to be a constant name of an MMU lookup.");
      }
    } else if (const Reg *r = get_reg(fun)) {
      // This represents a slice read to a register.
      // We only care b/c if we're in an assignment, then this represents a slice write, which means
      // that we must count this variable as a source as well as a target.
      UsedReg ur(r);
      ur._partial = true;
      if (update_partial_mask(ur,array.All())) {
        addUsedReg(ur);
      } 
      if (Ptree *op = inAssign()) {
        if ( !Eq(op,"=") ) {
          PError(exp,"We only allow simple assignments to registers.  Arithmetic operator assignments are not allowed.");
        }
      }
      
      replace = pr = make_reg_obj(0,r);
      pr->_regaccess._reg   = r;
      pr->_regaccess._left  = getArg(array,0);
      pr->_regaccess._right = getArg(array,1);

      changed = true;
    } else if ((rf = get_regfile(fun))) {
      // We've encountered a register file, so we want to store this information.

      if (Ptree *op = inAssign()) {
        if ( !Eq(op,"=") ) {
          PError(exp,"We only allow simple assignments to register files.  Arithmetic operator assignments are not allowed.");
        }
      }

      if (numArgs(array) != 1) {
        PError(exp,"A register-file access expects a single argument:  The index of the element to be selected.");
      }

      Ptree *a = getArg(array,0);
      Ptree *a2 = Translate(a);
      Ptree *rindex = checkRegFileAccess(rf,a2,false,0);

      replace = pr = make_reg_obj(0,rf);
      pr->_regaccess._regfile = rf;
      pr->_regaccess._index   = a2;
      pr->_regaccess._rindex  = rindex;

      changed = true;
    } else if (Match(fun,"[ %? ( %? ) ]",&regfile,&index) && (rf = get_regfile(regfile))) {      
      // Register-file slice access.
      if (Ptree *op = inAssign()) {
        if ( !Eq(op,"=") ) {
          PError(exp,"We only allow simple assignments to register files.  Arithmetic operator assignments are not allowed.");
        }
      }

      if (numArgs(array) > 2) {
        PError(exp,"A slice may consist of only 1 or 2 arguments.");
      }

      Ptree *i2 = Translate(First(index));
      Ptree *rindex = checkRegFileAccess(rf,i2,true,array.All());

      // Now extract anything we need and create the new register-read object.
      replace = pr = make_reg_obj(0,rf);
      pr->_regaccess._regfile = rf;
      pr->_regaccess._index   = i2;
      pr->_regaccess._rindex  = rindex;
      
      pr->_regaccess._left = getArg(array,0);
      pr->_regaccess._right = getArg(array,1);

      changed = true;
    } else if (Match(fun," [ [ %? ( %? ) ] . %? ] ",&reg,&index,&field)) {
      Ptree *index2 = Translate(First(index));
      if (auto context = get_ctx(reg)) {
        if ((rf = get_regfile(field)) ) {
          // Convert to a read of a context-specific register file.
          if (numArgs(array) != 1) {
            PError(exp,"A GPR expects a single argument:  The index of the element to be selected.");
          }
          
          Ptree *a = getArg(array,0);
          Ptree *a2 = Translate(a);
          changed = true;
          checkCtxAccess(context,index2,rf);

          // Ignore usage tracking on this for now, as we don't handle virtualizing
          // context-specific writes yet.
          //checkRegFileAccess(rf,a2,false,0);

          // Now extract anything we need and create the new register-read object.
          replace = pr = make_reg_obj(0,rf);
          pr->_regaccess._regfile = rf;
          pr->_regaccess._index   = a2;
          pr->_regaccess._rindex  = a2;
          pr->_regaccess._context = index2;

        } else if ((r = get_reg(field))) {
          // A context-specific slice read.

          // Ignore usage tracking on this for now, as we don't handle virtualizing
          // context-specific writes yet.
          //          UsedReg ur(r);
          //          ur._partial = true;
          //          update_partial_mask(ur,array.All());
          //          addUsedReg(ur);
          if (Ptree *op = inAssign()) {
            if ( !Eq(op,"=") ) {
              PError(exp,"We only allow simple assignments to registers.  Arithmetic operator assignments are not allowed.");
            }
          }
          
          replace = pr = make_reg_obj(0,r);
          pr->_regaccess._reg     = r;
          pr->_regaccess._left    = getArg(array,0);
          pr->_regaccess._right   = getArg(array,1);
          pr->_regaccess._context = index2;

          changed = true;
        } else {
          PError(exp,"Unknown resource in a context-specific access.");
        }
      } else if ( Match(reg,"[[ %? ( %? ) ] . %? ]",&ctx,&ctx_index,&creg)) {
        if (auto context = get_ctx(ctx) ) {
          if ((rf = get_regfile(creg))) {
            // Convert to a read of a register field.
            if (const RegField *rfield = get_field(field,*rf)) {
              if (rfield->indexed()) {
                if (numArgs(array) != 1) {
                  PError(exp,"A GPR expects a single argument:  The index of the element to be selected.");
                }
          
                Ptree *a = getArg(array,0);
                Ptree *a2 = Translate(a);
                Ptree *ctx_index2 = Translate(ctx_index);
                changed = true;
                checkCtxAccess(context,ctx_index2,rf);

                // Ignore usage tracking on this for now, as we don't handle
                // virtualizing context-specific writes yet.
                //checkRegFileAccess(rf,index2,true,field,a2);

                // Now extract anything we need and create the new register-read object.
                replace = pr = make_reg_obj(0,rf);
                pr->_regaccess._regfile = rf;
                pr->_regaccess._index   = index2;
                pr->_regaccess._rindex  = index2;
                pr->_regaccess._field   = rfield;
                pr->_regaccess._findex  = a2;
                pr->_regaccess._context = ctx_index2;
              }
            }
          } else {
            PError(exp,"Unknown resource in a context-specific access.");
          }
        }
      } else {
        // This is a dot member expression.  This could be a reference to an
        // indexed field.  We record our parms, then translate.
        bool used_field_index,tmp;
        fun2 = handleDotMember(used_field_index,tmp,fun,getArg(array,0),getArg(array,1));
        // Fixup:  Re-do what the register-read object points to so that it gets the field index.
        if (used_field_index) {
          if (auto prf = dynamic_cast<PtreeRegRead*>(fun2)) {
            fun2 = prf->Car();
            replace = pr = prf;
          }
        }
        changed = true;
      }
    } else if (Match(fun,"[[[%? ( %? )] . %?] ( %? )]",&ctx,&ctx_index,&regfile,&index)) {
      if (auto context = get_ctx(ctx) ) {
        if ((rf = get_regfile(regfile))) {
          // Slice read of a register file.
          if (Ptree *op = inAssign()) {
            if ( !Eq(op,"=") ) {
              PError(exp,"We only allow simple assignments to register files.  Arithmetic operator assignments are not allowed.");
            }
          }

          if (numArgs(array) > 2) {
            PError(exp,"A slice may consist of only 1 or 2 arguments.");
          }

          Ptree *i2 = Translate(First(index));
          Ptree *ctx_index2 = Translate(ctx_index);
          checkCtxAccess(context,ctx_index2,rf);

          // Ignore usage tracking on this for now, as we don't handle
          // virtualizing context-specific writes yet.
          // checkRegFileAccess(rf,i2,true,array.All());

          // Now extract anything we need and create the new register-read object.
          replace = pr = make_reg_obj(0,rf);
          pr->_regaccess._regfile = rf;
          pr->_regaccess._index   = i2;
          pr->_regaccess._rindex  = i2;
          
          pr->_regaccess._left = getArg(array,0);
          pr->_regaccess._right = getArg(array,1);
          pr->_regaccess._context = ctx_index2;
          
          changed = true;

        }
      }
    } else if (fun->IsA(ntDotMemberExpr)) {
      // This is a dot member expression.  This could be a reference to an
      // indexed field or accessing a register-file's member..  We record our
      // parms, then translate.
      bool used_field_index,const_value;
      fun2 = handleDotMember(used_field_index,const_value,fun,getArg(array,0),getArg(array,1));
      if (const_value) {
        // If this was a constant, then just return, e.g. a type conversion
        // call on a variable or parameter.
        return fun2;
      }
      // Fixup:  Re-do what the register-read object points to so that it gets the field index.
      if (used_field_index) {
        if (auto prf = dynamic_cast<PtreeRegRead*>(fun2)) {
          fun2 = prf->Car();
          replace = pr = prf;
        }
      }
      changed = true;
    } else if (fname == MemoryName || get_mem(fname)) {
      replace = pm = handleMemAccess(exp,fname,array);
      changed = true;
    } else if (fname == "info" || fname == "warning" || fname == "error") {
      if (numArgs(array) < 2) {
        PError(exp,"Message function '" << fname << "' requires at least two parameters:  A message level and a message.");
      }
      // Then check for the correct format if data arguments are present.
      int data_start_arg = -1;
      for (unsigned i = 0; i < array.Number(); i += 2) {
        if (data_start_arg < 0) {
          if (Eq(array[i],AnnotationDataStart)) {
            data_start_arg = i;
          }
        } else {
          // The rest of the items must be key -> value pairs.  The keys must be constants, but
          // the values may be any type of expression.
          Ptree *key,*value;
          if (Match(array[i],"[ ( [ %? , %? ] ) ]",&key,&value)) {
            if (!get_str(key,0)) {
              PError(exp,"Argument " << i/2+1 << ": Expected a string key for (key,value) pair after the " << 
                     AnnotationDataStart << " symbol.");
            }
          } else {
            PError(exp,"Argument " << i/2+1 << ": Expected an annotation (key,value) pair after the " << 
                   AnnotationDataStart << " symbol.");
          }
        }
      }
      // Make sure that there are at least two args before the data start symbol- the first should be the level, then
      // an initial message.
      if (data_start_arg >= 0 && data_start_arg < 4) {
        PError(exp,"Message function '" << fname << "' requires at least two parameters before the data-start-symbol:  A message level and a message.");
      }
    } else if (hinfo) {
      fun2 = checkHelper(fun,array,hinfo,fname);
      changed = true;
    } else if (Eq(fun,"raiseException")) {
      _uinfo.back()._exceptions = true;
    } else if (Eq(fun,"halt")) {
      _uinfo.back()._halts = true;
    } else if (fname == "dataReadTranslate" || fname == "instrReadTranslate") {
      // If we run across a translation function, then we consider this function
      // to have accessed memory, since it's accessing the memory hierarchy.
      addUsedMem(MemoryName);
    } else if (fname == "dataWriteTranslate") {
      // Same reason as above, but since this is a write, we consider it a
      // target.
      addUsedMem(MemoryName,true);
    } else if (fname == "run") {   // run() function for micro-instructions
        if (numArgs(array) == 0) {
            PError(exp,"Expects arguments for the run() function");
        }
        if (_instr) { // Check only if (_instr) is valid
            for (unsigned i = 0; i < numArgs(array); ++i) {
                Ptree *arg = getArg(array,i);
                if (!is_instrfield(arg)) {  // Check if argument is one of the instrfield
                    PError(exp,"Invalid argument for the run() function: " << arg->ToString());
                }
                ForEach(_instr->getFields(), it) { // Check the type only if it is named
                    if ((it->_field->interface()) &&
                        (it->_field->interface()->name() == arg->ToString()) &&
                        (it->_field->type()!=InstrField::tInstr)) {
                        PError(exp,"Invalid argument type for the run() function: " << arg->ToString());
                    }
                }
            }
        }
    } else if (fname == "run_commit") {   // run_commit() function for micro-instructions
        if (numArgs(array) == 0) {
            PError(exp,"Expects arguments for the run() function");
        }
        if (_instr) { // Check only if (_instr) is valid
            for (unsigned i = 0; i < numArgs(array); ++i) {
                Ptree *arg = getArg(array,i);
                if (!is_instrfield(arg)) {  // Check if argument is one of the instrfield
                    PError(exp,"Invalid argument for the run() function: " << arg->ToString());
                }
                ForEach(_instr->getFields(), it) { // Check the type only if it is named
                    if ((it->_field->interface()) &&
                        (it->_field->interface()->name() == arg->ToString()) &&
                        (it->_field->type()!=InstrField::tInstr)) {
                        PError(exp,"Invalid argument type for the run() function: " << arg->ToString());
                    }
                }
            }
        }
    } else {		
      // If all else fails, then try to just translate the function name itself.
      // This will allow an unknown function name to be caught.
      fun2 = Translate(fun);
      if (fun2 != fun) {
        changed = true;
      }
    }

    assert(!_nested_helper.empty());
    _nested_helper.pop_back();
    //cout << "  Popping from nested_helpers." << endl;

    if (!changed) { 
      return Walker::TranslateFuncall(exp);
    } else {
      // Now see if any arguments were changed and if so, substitute
      // those in.
      Ptree *args2 = PtreeUtil::ShallowSubst(array.All(), args, arglist);
      auto pf = new PtreeFuncallExpr(fun2 == 0? fun : fun2,args2);
      if (replace) {
        replace->SetCar(pf);
        return replace;
      } else {
        return pf;
      }
    }
  }

  PtreeRegRead *VarWalker::handleRegFieldRead(bool &used_field_index,Ptree *exp,const Reg *r,Ptree *field,
                                              Ptree *field_index,bool ignore_usage)
  {
    const RegField *rfd = 0;

    // This represents a read or write to a register field.
    UsedReg ur(r);
    if ((rfd = get_field(field,*r))) {
      if (rfd->indexed()) {
        if (!field_index) {
          PError(field,"Attempt to access an indexed field as a non-indexed field.");
        }
        used_field_index = true;
      } else {
        // Get rid of the index, since we know that it's not actually part of
        // this access
        field_index = 0;
      }
      if (!ignore_usage) {
        ur._partial = rfd;
        if (update_partial_mask(ur,(rfd) ? field : 0,field_index)) {
          addUsedReg(ur);
        }
      }

      auto pr = make_reg_obj(exp,r);
      pr->_regaccess._reg     = r;
      pr->_regaccess._field   = rfd;
      pr->_regaccess._findex  = field_index;
      return pr;
    }
    return 0;
  }

  // We have this here to catch a few special cases, such as when we encounter
  // a register or register-file, where a non-mutating method is being invoked.
  // In some cases, we can still handle this as a particular element access.
  Ptree* VarWalker::handleDotMember(bool &used_field_index,bool &const_value,Ptree* exp,Ptree *field_index,Ptree *arg2)
  {
    used_field_index = false;
    const_value = false;

    Ptree* left = exp->Car();
    Ptree* left2 = left;

    if (Eq(left,MemoryName) && _check_mem) {
      PError(exp,"Memory does not have any member functions.");
    }

    bool skip = false;
    Ptree* member = Third(exp);

    gc_string name = left->ToString();

    Ptree *fname,*arg, *reg;

    PtreeRegRead *pr = 0;

    const RegField *rfd = 0;

    // Check to see if this is a register-file write to a field or a context-member access.
    if ( Match(left2,"[ %? ( %? ) ]",&fname,&arg)) {
      Ptree *ctx, *ctx_index, *regfile;
      if (Match(fname,"[[%? ( %? )] . %?]",&ctx,&ctx_index,&regfile)) {
        if (auto context = get_ctx(ctx)) {
          if ( const RegFile *rf = get_regfile(regfile)) {
            if ((rfd = get_field(member,*rf))) {
              if (rfd->indexed()) {
                PError(exp,"Attempt to access an indexed field as a non-indexed field.");
              }
            }
            Ptree *index2 = Translate(arg);
            Ptree *ctx_index2 = Translate(ctx_index);
            checkCtxAccess(context,ctx_index2,rf);
            skip = true;

            // Ignore usage tracking on this for now, as we don't handle
            // virtualizing context-specific writes yet.
            // checkRegFileAccess(rf,index2,rfd,member);
          
            pr = make_reg_obj(exp,rf);
            pr->_regaccess._regfile = rf;
            pr->_regaccess._field   = rfd;
            pr->_regaccess._index   = index2;
            pr->_regaccess._rindex  = index2;
            pr->_regaccess._context = ctx_index;
          }
        }
      }
      else if ( const RegFile *rf = get_regfile(fname)) {
        if ((rfd = get_field(member,*rf))) {
          if (rfd->indexed()) {
            if (!field_index) {
              PError(exp,"Attempt to access an indexed field as a non-indexed field.");
            }
            // Note: We can't check to see if a non-indexed field is used as an
            // indexed field because the syntax of taking a slice of a field is
            // identical.  Here, we will be using the field index as part of the
            // read expression.
            used_field_index = true;
          } else {
            // Get rid of the index, since we know that it's not actually part of
            // this access
            field_index = 0;
          }
          Ptree *index2 = Translate(First(arg));
          Ptree *rindex = checkRegFileAccess(rf,index2,rfd,(rfd) ? member : 0,field_index);
          skip = true;

          pr = make_reg_obj(exp,rf);
          pr->_regaccess._regfile = rf;
          pr->_regaccess._field   = rfd;
          pr->_regaccess._index   = index2;          
          pr->_regaccess._rindex  = rindex;
          pr->_regaccess._findex  = field_index;
        } else if (Eq(member,"size")) {
          // The size() method is static, so it doesn't require the data in the
          // register.  Thus, we don't record its usage, but we do record it as
          // a register-file read.
          skip = true;
          auto pr = new PtreeRegRead(left,is_cur_reg(rf));
          pr->_regaccess._regfile = rf;
          pr->_regaccess._index   = Translate(arg);
          pr->_regaccess._rindex  = pr->_regaccess._index;
          left2 = pr;          
        }
      } 
      else if ( const Context *ctx = get_ctx(fname) ) {
        checkCtxAccess(ctx,First(arg),member);
        pr = handleRegRead(member,member,First(arg));
      }
    } else if (const RegFile *rf = get_regfile(left)) {
      // The valid_index method is static for register-files, so ignore this
      // register-file as a source.  However, we record the register-file access
      // itself as an object.  Same for size() and hasAttr().
      if ( Eq(member,"validIndex") || Eq(member,"size") || Eq(member,"hasAttr")) {
        skip = true;
        auto pr = new PtreeRegRead(left,is_cur_reg(rf));
        pr->_regaccess._regfile = rf;
        left2 = pr;
        if (Eq(member,"hasAttr")) {
          // Second argument should be an attribute.
          if (!field_index || !arg2) {
            PError(exp,"The register-file hasAttr method expects two arguments.");
          }
          gc_string attr = get_str(arg2,"register-file hasAttr argument.");
          int a_id = Data::get_attr(attr);
          if (a_id < 0) {
            PError(exp,"Unknown attribute '" << attr << "' in register-file hasAttr call.");
          }
          // We also record the fact that this attribute is used within a
          // regfile-attribute query.
          if (_nccore) {
            _nccore->add_regfile_attr(a_id);
          } else {
            PError(exp,"A register-file may only be queried within a core.");
          }
        }
      } else {
        PError(exp,"Unknown member/field accessed for register file:  " << exp);
      }
    } else if (get_cache(name)) {
      // Record any caches used.
      addUsedCache(name);
    } else if (const Reg *r = get_reg(left)) {
      // This might be a register field access, or it could just be a member
      // access of the resulting intbv.  So, if it's not a field, translate just
      // the lhs.
      if ((pr = handleRegFieldRead(used_field_index,exp,r,member,field_index))) {
        skip = true;
      }
    } else if ( Match(left2,"[ [ %? ( %? ) ] . %? ]",&fname,&arg,&reg) ) {
      // This handles reading a register field using a specific context.  Ignore
      // usage tracking on this for now, as we don't handle virtualizing
      // context-specific writes yet.
      if (const Reg *r = get_reg(reg)) {
        if ((pr = handleRegFieldRead(used_field_index,exp,r,member,field_index,true))) {
          skip = true;
          pr->_regaccess._context = Translate(arg);
        }
      }
    } else if (isTypeConversion(member) && fullExprEval()) {
      // If this is a type conversion and we're evaluating expressions, then see
      // if the lhs is a variable or parm and substitute its value if so.
      unsigned res;
      if (get_variable_value(res,left)) {
        const_value = true;
        return make_leaf<LeafUNSIGNED>(res);
      }
    }  

    if (!skip) {
      left2 = Translate(left);
    } 

    Ptree *result;
    if (left == left2) {
      result = exp;
    } else {
      result = new PtreeDotMemberExpr(left2, exp->Cdr());
    }
    if (pr) {
      pr->SetCar(result);
      return pr;
    } else {
      return result;
    }
  }

  Ptree *VarWalker::TranslateDotMember(Ptree *exp)
  {
    bool dummy1,dummy2;
    return handleDotMember(dummy1,dummy2,exp,0,0);
  }

  // Retrieves from the environment a list of all parent objects in the
  // environment.  The current define block is the first element.
  ParentList VarWalker::getParentObj()
  {
    Environment *e = env;
    Bind *b = 0;
    ParentList parents;
    //env->DumpAll();

    // Search through all scopes for the variable.
    while (e) {
      if (e->LookupTop(parentObj,b)) {
        if (BindParent *bp = dynamic_cast<BindParent*>(b)) {
          parents.push_back(bp->parent());
        }
      }
      e = e->GetOuterEnvironment();
    }

    return parents;
  }

  // Check to see if an aspect passes its definition checks.
  bool VarWalker::aspectCheck(const Aspect &a,const Aspects *top_parent)
  {
    // If it has none, then it passes.  Otherwise, check the symbol.  If not defined, then
    // we don't expand.
    if (!a.def_checks().empty()) {
      ForEach(a.def_checks(),i) {
        if ( !is_instrfield(*i) ) {
          if (!is_blk(*i)) {
            if ( !(_core && _core->is_resource((*i)->ToString())) ) {
              return false;
            }
          }
        }
      }
    }
    // Next, check attributes.  If the parent object has any of those specified,
    // then we're OK.  Otherwise, we don't expand.
    if (!a.attrs().empty() && top_parent) {
      bool found = false;
      ForEach(a.attrs(),i) {
        if (top_parent->has_attr(*i)) {
          found = true;
          break;
        }
      }
      if (!found) {
        return false;
      }
    }
    return true;
  }

  // We can sort simply by priority b/c the don't-have-a-priority case is
  // handled by setting _priority to a maximum value.
  bool sort_aspects(const Aspect *x,const Aspect *y) 
  {
    return x->priority() < y->priority();
  }

	// Return matching aspects.  If l_str is non-empty, then match against it.
	// Otherwise, use the aspect predicate, if there is one.  If we've disabled
	// aspect expansion, then we can just return immediately.
  void VarWalker::getAspects(PtreeArray &actions,const gc_string &l_str,bool pre)
	{
    if (Data::origAction()) {
      return;
    }

    ParentList parents = getParentObj();
		AspectList al;

    const Aspects *top = (parents.empty()) ? 0 : parents.front();

    // We put the aspects into a vector so that we can then sort by priority.
    gc_vector<const Aspect *> alist;

		// For each parent, see if there are any aspects defined, then see if any of
		// those aspects match the label.  Move up to an enclosing scope if nobody
		// has set an exclusive flag.  If the label is empty, then match against the
		// aspect predicate, if one exists.  If the aspect has definition checks,
		// then only add the aspect if those pass.
		ForEach(parents,i) {
			const Aspects &a = **i;

      al.clear();
			bool ex = a.getAspects(al,l_str);
		
			ForEach(al,aiter) {
        const Aspect *aspect = *aiter;
				if (l_str.empty()) {
					// If no label, then only add if we match against the predicate,
					// assuming one exists.
					if ( _aspect_pred && (*_aspect_pred)(*this,*aspect,pre) ) {
            if (aspectCheck(*aspect,top)) {
              alist.push_back(aspect);
            }
					}
				} else {
					// Otherwise add, since the label has already been matched by
					// getAspects.
          if (aspectCheck(*aspect,top)) {
            alist.push_back(aspect);
          }
				}
			}
			if (ex) {
				// Exclusive, so stop at this scope.
				break;
			}
		}

    // Then sort actions by priority (if relevant) and add the aspects' actions
    // to the action list.
    sort(alist.begin(),alist.end(),sort_aspects);
    ForEach(alist,i) {
      actions.Append((*i)->action());
    }
	}

  // Labels are used as aspect code injection points.  For each label, we see if
  // there's a relevant aspect and insert it.  In either case, the label is
  // propagated.
  Ptree *VarWalker::TranslateLabel(Ptree *exp)
  {
    Ptree      *label  = First(exp);
    gc_string      l_str  = label->ToString();
    
		PtreeArray actions;
		getAspects(actions,l_str,false);

		if (actions.Number()) {
			actions.Append(Third(exp));
      Ptree *st = new PtreeStmts(actions.All());
      Ptree *st2 = Translate(st);
			return new PtreeLabelStatement(label,PtreeUtil::List(exp->Cdr()->Car(), st2));
		}

    return AdlTranslatingWalker::TranslateLabel(exp);
  }

  // We translate enums differently- all values are registerd in the environment
  // so that we don't consider them to be escaping variables.
  Ptree* VarWalker::TranslateEnumSpec(Ptree* spec)
  {
    Ptree *result = Walker::TranslateEnumSpec(spec);

    // Now add each enum name to the environment (this should be done by the
    // Walker in Plasma, but it's not).
    Ptree *values = Second(Third(spec));

    char* encoded_name = (char*)spec->GetEncodedName();

    while(values != 0){
      Ptree* p = values->Car();
      Ptree *opt;
      if (p->IsLeaf()) {
        opt = p;
      } else {
        opt = First(p);
        assert(opt->IsLeaf());
      }
      env->AddEntry(opt->GetPosition(),opt->GetLength(),new BindEnumName(encoded_name,opt));
      values = values->Cdr();
      if(values != 0){
        values = values->Cdr();
      }
    }  
    return result;
  }

  void VarWalker::merge_uinfo(bool discard,bool merge_cond)
  {
    assert(!_uinfo.empty());
    if (discard) {
      _uinfo.pop_back();
    } else {
      UsageInfo tmp = _uinfo.back();
      _uinfo.pop_back();
      _uinfo.back().update(tmp,merge_cond);
    }
  }

  bool VarWalker::translate_const_expr(unsigned &res,Ptree *cond)
  {
    // Try to see if we can statically evaluate the conditional.
    bool ot = set_track_usage(false);
    _expr_eval.push_back(cExprEval);
    Ptree *cond_result = Translate(cond);
    bool have_result = get_uint(res,cond_result,env);
    _expr_eval.pop_back();
    set_track_usage(ot);
    return have_result;
  }

  // In addition to tracking whether we're in a conditional, we also push on a
  // new usage-tracking item for each branch of the conditional, then merge the
  // results in with the current top item.  This is so that we don't get
  // erroneous target/source information by treating the if and then clauses as
  // sequential when in fact they're independent.
  Ptree* VarWalker::TranslateIf(Ptree* s)
  {
    Ptree* cond = PtreeUtil::Third(s);

    unsigned res = 0;
    bool have_result = translate_const_expr(res,cond);

    Ptree *cond2 = Translate(cond);

    _state.push_back(sCond);

    // We copy our existing data and use that as a starting point.  This way, we
    // properly track whether something (before the conditional) has already
    // been seen as a source or target.
    UsageInfo tmp = _uinfo.back();

    bool discard_then = (have_result && !res);
    bool discard_else = (have_result && res);
    bool keep_both = (!discard_then && !discard_else);

    // If we're not keeping both sides of the conditional, then we merge data as
    // we go.  Otherwise, merge when done.

    _uinfo.push_back(tmp);
    Ptree* then_part = PtreeUtil::Nth(s,4);
    Ptree* then_part2 = Translate(then_part);
    if (!keep_both) {
      merge_uinfo(discard_then);
    }

    _uinfo.push_back(tmp);
    Ptree* else_part = PtreeUtil::Nth(s,6);
    Ptree* else_part2 = Translate(else_part);
    if (!keep_both) {
      merge_uinfo(discard_else);
    }

    // If we are keeping data from both sides of the if/then/else block, then we
    // want to not mark items as conditional if they appear on both sides.  The
    // complication is that if an item is on both sides, but one side already
    // marked it as conditional because it as found in a nested if, then that
    // must be respected.
    if (keep_both) {
      // Fold then/else info so that common elements are not marked as
      // conditional.
      merge_uinfo( false, true );
      merge_uinfo( false, false );
    }

    //    cout << "If Usageinfo:  " << _uinfo.back() << endl;

    _state.pop_back();

    if(cond == cond2 && then_part == then_part2 && else_part == else_part2)
      return s;
    else{
      Ptree* rest = PtreeUtil::ShallowSubst(cond2, cond, then_part2, then_part,
                                            else_part2, else_part, s->Cdr());
      return new PtreeIfStatement(s->Car(), rest);
    }
  }

  void VarWalker::merge_switch_uinfo()
  {
    bool record = true;
    if (!_switch_uinfo.empty()) {
      record = _switch_uinfo.back()._record;      
    }
    //    cout << "Case Usageinfo:  " << _uinfo.back() << endl;
    if (_switch_uinfo.empty() || !_switch_uinfo.back()._has_val) {
      _uinfo.back().mark_as_cond();
    }
    merge_uinfo(!record);
  }

  void VarWalker::get_switch_uinfo(UsageInfo &uinfo,bool &have_result,bool &take_next,unsigned &result)
  {
    if (_switch_uinfo.empty()) {
      have_result = false;
      uinfo = UsageInfo();
    } else {
      have_result = _switch_uinfo.back()._has_val;
      take_next   = _switch_uinfo.back()._take_next;
      result      = _switch_uinfo.back()._val;
      uinfo       = _switch_uinfo.back();
    }
  }

  // If true, then we should take the next case block.
  void VarWalker::set_case_take_next(bool x)
  {
    if (!_switch_uinfo.empty()) {
      _switch_uinfo.back()._take_next = x;
    }
  }

  void VarWalker::set_case_record(bool x)
  {
    if (!_switch_uinfo.empty()) {
      _switch_uinfo.back()._record = x;
    }
  }

  // The parsing of case statements is really weird- some might call it a bug.
  // If two cases labels are adjacent, then they'll be nested.  However,
  // sometimes statements are not attached to a case label, even though program
  // flow will still encounter it.
  //
  // In order to handle this, case labels are treated as independent items and
  // not as a guard around a block, as an if-condition is.  When a case label is
  // encountered, it starts the evaluation of a new set of resources and it
  // dumps the prior set (if applicable).  If we hit a break that's directly in
  // the switch statement (not within a conditional or loop), we use it to tell
  // us to consider the next item to be taken as well.
  //
  // So, there are two questions to ask: Should we record a prior case item and
  // should we continue immediately to the next?  The first question is answered
  // by evaluating the case label's value.  The second is answered by whether we
  // encounter a break or not.
  Ptree* VarWalker::TranslateCase(Ptree* s)
  {
    bool have_result, take_next;
    unsigned result;
    UsageInfo tmp;

    // First, should we record the prior case block's data?
    merge_switch_uinfo();

    get_switch_uinfo(tmp,have_result,take_next,result);

    // This will record this item's data.
    _uinfo.push_back(tmp);

    // Start out assuming that we'll take the next case block as well.
    set_case_take_next(true);

    // If we have a known switch conditional value, then see if this matches.
    // If it does, record the usage information, otherwise don't bother.  If we
    // can't evaluate it at all, then assume that we should record it, since
    // it'll be a dynamically determined thing.  If we've been told to take the
    // next item, then we skip the evaluation and simply set the record flag to
    // true.
    bool record_usage = true;
    if (!take_next && have_result) {
      Ptree *c = Second(s);
      unsigned cv;
      if (get_uint(cv,c,env)) {
        if (cv != result) {
          record_usage = false;
          // We're not taking this one, so we assume we're not taking the next.
          // If we evaluate the next case label to true, then we will take it.
          set_case_take_next(false);
        }
      }
    }
    set_case_record(record_usage);

    Ptree* st = PtreeUtil::Nth(s,3);
    Ptree* st2 = Translate(st);

    if(st == st2)
      return s;
    else
      return new PtreeCaseStatement(s->Car(),
                                    PtreeUtil::ShallowSubst(st2, st, s->Cdr()));
  }

  Ptree* VarWalker::TranslateSwitch(Ptree* s)
  {
    Ptree* cond = PtreeUtil::Third(s);

    // See if we have a constant value for the switch condition.
    unsigned result = 0;
    bool have_result = translate_const_expr(result,cond);

    Ptree* cond2 = Translate(cond);

    Ptree* body = PtreeUtil::Nth(s,4);

    // This is a dummy item to start off the evaluation.  Each case label always
    // dumps what's on top of the stack first, then continues with its own state
    // (see explanation in TranslateCase).
    UsageInfo tmp;
    _uinfo.push_back(tmp);

    _state.push_back(sSwitch);
    _switch_uinfo.push_back(SwitchUsageInfo(have_result,result,_uinfo.back()));
    Ptree* body2 = Translate(body);

    // Final merge of last item.
    merge_switch_uinfo();

    _switch_uinfo.pop_back();
    _state.pop_back();

    //    cout << "Switch Usageinfo:  " << _uinfo.back() << endl;

    if(cond == cond2 && body == body2)
      return s;
    else{
      Ptree* rest = PtreeUtil::ShallowSubst(cond2, cond, body2, body, s->Cdr());
      return new PtreeSwitchStatement(s->Car(), rest);
    }
  }

  // For now, if we encounter a break within a switch, then we regard that as
  // stopping a case evaluation.  A break nested within something else won't
  // trigger a stop, so that, at worst, we'll be overly conservative.
  Ptree* VarWalker::TranslateBreak(Ptree* s)
  {
    if (inSwitch()) {
      set_case_take_next(false);
    }
    return s;
  }

  // The Car of the object contains the function we'll be translating.
  Ptree* VarWalker::TranslateSubInstrCall(PtreeSubInstrCall* s)
  {
    Ptree *fun = s->Car();

    Ptree *fun2 = 0;
    if (fun->IsA(ntFuncallExpr)) {
      Ptree *args = fun->Cdr();
      Ptree *args2 = TranslateArguments(args);
    
      if(args == args2) {
        fun2 = fun;
      } else {
        fun2 = new PtreeFuncallExpr(fun, args2);
      }
    } else {
      // Not yet a properly set up sub-instruction call, so basically just ignore.
      fun2 = fun;
    }

    if (fun2 == fun) {
      // No change- just merge in usage information.
      _uinfo.back().update(s->uinfo());
      return s;
    } else {
      // Did change- create new node.
      Ptree *s2 = new PtreeSubInstrCall(*s);
      s2->SetCar(fun2);
      return s2;
    }
  }

  Ptree* VarWalker::TranslateStmts(Ptree* block)
  {
    Ptree *block2 = AdlTranslatingWalker::TranslateBrace(block);
    if (block != block2) {
      return new PtreeStmts(block2->Car(),block2->Cdr());
    } else {
      return block2;
    }
  }

}



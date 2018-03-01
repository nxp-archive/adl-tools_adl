//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#include <assert.h>
#include <stdlib.h>
#include <vector>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <sys/types.h>
#include <sys/stat.h>

#include <boost/assign/list_of.hpp>

#include "opencxx/Walker.h"
#include "opencxx/PtreeIter.h"
#include "opencxx/Environment.h"
#include "opencxx/parser/PtreeBlock.h"
#include "opencxx/parser/PtreeArray.h"
#include "opencxx/parser/ptree-generated.h"

#include "helpers/Macros.h"
#include "helpers/CodeGen.h"
#include "helpers/BitTwiddles.h"

#include "fe/Data.h"
#include "fe/Types.h"
#include "fe/Helpers.h"
#include "fe/MsgLog.h"
#include "fe/VarWalker.h"

#include "UadlCommon.h"
#include "UadlParser.h"
#include "UadlHandlers.h"
#include "TransactionTypes.h"
#include "UadlTranslatingWalker.h"

using namespace std;
using namespace adl;
using namespace rnumber;
using namespace Opencxx;
using namespace PtreeUtil;
using namespace boost::assign;

namespace uadl {

  const gc_string OtherOperandStr("__other__");

  const gc_string FetchExceptionStr("fetch_exception");

  const gc_string DefaultInstrStr("default_instruction");

  const gc_string DecodeMissStr("decode_miss");

  static const char *RepeatStr = "Repeat";

  static string GlobalPrefix;

  // Empties for default conditions.
  static UsageInfo empty_usage;
  static InstrSlotAlloc empty_slots;

  template <typename T>
  T *isA(Resource *res)
  {
    return dynamic_cast<T*>(res);
  }

  template <typename T>
  const T *isA(const Resource *res)
  {
    return dynamic_cast<const T*>(res);
  }

  // A string set where the count function also accesses parent objects.
  struct PrntStrSet : private adl::StrSet {
    typedef adl::StrSet Base;
    typedef Base::value_type value_type;

    PrntStrSet() : _parent(0) {};
    PrntStrSet(PrntStrSet *p) : _parent(p) {};

    using adl::StrSet::insert;
    using adl::StrSet::clear;
    using adl::StrSet::empty;
    using adl::StrSet::size;

    unsigned count(const value_type &x) const
    {
      if (unsigned r = Base::count(x)) {
        return r;
      } else if (_parent) {
        return _parent->count(x);
      } else {
        return 0;
      }
    }

    PrntStrSet *_parent;
  };

  StrHashSet initValidFuncs()
  {
    StrHashSet x;

    x.insert("exec");
    x.insert("exec_and_read");
    x.insert("exec_and_write");
    x.insert("exec_done");
    x.insert("change_state");
    x.insert("stalled");
    x.insert("set_speculative");
    x.insert("flush");
    x.insert("taken_flush");
    x.insert("branch_taken");
    x.insert("branch_target");
    x.insert("has_exception");
    x.insert("commit_exceptions");
    x.insert("capacity");
    x.insert("has_next_request");
    x.insert("has_more_mem");
    x.insert("next_req_mem");
    x.insert("free_all_mem");
    x.insert("addr");
    x.insert("size");

    x.insert("can_read_ops");
    x.insert("can_write_ops");
    x.insert("read_ops");
    x.insert("write_ops");
    x.insert("allocate_ops");
    x.insert("deallocate_ops");

    x.insert("latency");
    x.insert("check_latency");
    x.insert("set_latency");
    x.insert("clear_latency");
    x.insert("decr_latency");

    x.insert("is_misaligned");
    x.insert("has_requested_mem_op");

    x.insert("is_serialized");
    x.insert("set_serialized");
    x.insert("clr_serialized");

    x.insert("thisInstr");
    x.insert("thisCore");

    x.insert("getChildId");

    x.insert("last_branch_prediction_correct");

    return x;
  }

  StrHashSet initValidVars()
  {
    StrHashSet x;

    x.insert(getIssueName());
    x.insert("true");
    x.insert("false");
    x.insert("position");
    x.insert("logger");
    x.insert("uarch");
    x.insert("Taken");
    x.insert("NotTaken");
    x.insert("Miss");

    x.insert("zero");
    x.insert("signExtend");
    x.insert("zeroExtend");
    x.insert("signedMultiply");
    x.insert("signedDivide");
    x.insert("bits");
    x.insert("Carry");
    x.insert("concat");

    x.insert("top");

    return x;
  }

  static StrHashSet ValidFuncs = initValidFuncs();

  static StrHashSet ValidVars = initValidVars();

  struct cmpUsedRegPtr {
    bool operator()(const UsedReg *x,const UsedReg *y) const
    {
      return *x < *y;
    }
  };

  // This class translates an action block into specific edge/state actions.
  class ActionWalker : public VarWalker {
  public:
    ActionWalker(Environment *e,UadlIr &ir,Core &core,InstClass *ic,MsgLog &ml) : 
      VarWalker(e,&core,0),
      _outer_env(VarWalker::env),
      _ir(ir),
      _ic(ic),
      _ml(ml),
      _action(0)
    {
      // We don't want the usual memory error checking, since it doesn't allow
      // member function calls on memories.
      _check_mem = false;

      ForEach(_ir.instClasses(),i) {
        _instr_class_enums.insert(i->second->getTypeName());
      }
    }

  protected:
    Environment *_outer_env;
    UadlIr      &_ir;
    InstClass   *_ic;
    MsgLog      &_ml;

    StrHashSet   _instr_class_enums;
    Action      *_action;
    
  public:

    bool is_operand(Ptree *x) 
    {
      return _ic && _ic->is_operand(x->ToString());
    }

    bool is_operand(const gc_string &n) 
    {
      return _ic && _ic->is_operand(n);
    }

    bool is_mgr(Ptree *x)
    {
      return _ir.res().findResourceByName(x->ToString());
    }

    bool is_func(const gc_string &n)
    {
      return _ic && _ic->is_func(n);
    }

    bool is_mgr(const gc_string &n)
    {
      return _ir.res().findResourceByName(n);
    }

		const PipelineStage *is_stage(Ptree *x)
		{
			return _ir.res().findStageByName(x->ToString());
		}

    // Returns true if we're in the outer-scope of the action block.  This is actually
    // one scope in, b/c the braces around the action code count as a scope.
    bool outer_scope() const { return env->GetOuterEnvironment() == _outer_env; }
    
    // If we come across an operand or manager, then allow it through, otherwise
    // process it and then finally error-out if it's an escape.  First, though,
    // check to see if it's a memory alias and translate it if necessary.  We've
    // already checked that memory aliases are OK, so no need to double-check.
    Ptree *handleEscape(Ptree *var,Ptree *exp)
    {
      gc_string n = var->ToString();

      if (Ptree *ma = _ir.findMemAlias(n)) {
        return ma;
      }

      if (is_operand(n)) {
        // Operands can cause exceptions on a read or write, so set the flag.
        _action->_has_possible_excpt = true;
        return exp;
      }

      if (is_mgr(n)) {
        return exp;
      }

      if (is_func(n)) {
        return exp;
      }

      if (_ic && _ic->machine().hasState(n)) {
        return exp;
      }

      if (ValidFuncs.count(n)) {
        if (n == "write_ops") {
          // Operand writes can possibly cause exceptions.
          _action->_has_possible_excpt = true;
        }
        return exp;
      }

      if (ValidVars.count(n)) {
        return exp;
      }

      if (_instr_class_enums.count(n)) {
        return exp;
      }

      if (InstClass::get_ic_attrs().count(n)) {
        return exp;
      }

      return VarWalker::handleEscape(var,exp);
    }

    // If we're at the outer-most scope, then labels are taken to be states.
    Ptree *TranslateLabel(Ptree *exp)
    {
      if (_ic) {
        Ptree *label = First(exp);
        if (outer_scope()) {
          auto a = _ic->addAction(label);
          _action = a;
          // It's a valid state, so we've added it as a state action.
          auto st =  PtreeUtil::Third(exp);
          auto st2 = Translate(st);
          // We expect an if-statement or brace-delimited block after the label and colon.
          if (!st2->IsA(ntIfStatement) && !dynamic_cast<PtreeBrace*>(st2)) {
            PError(exp,"Expected the actions for a state to be surrounded by braces.");
          }
          a->set_action(st2);
          _action = 0;
        
          // Return the item in order to maintain the overall theme of filtering.
          if(st == st2)
            return exp;
          else
            return new PtreeLabelStatement(exp->Car(),
                                           PtreeUtil::List(exp->Cdr()->Car(), st2));
        } else if (Eq(label,RepeatStr)) {
          // It's the special repeat label, so store this as a repeat statement.
          auto exp2 = VarWalker::TranslateLabel(exp);

          Ptree *st2 = PtreeUtil::Third(exp2);
          _action->_repeats.push_back(st2);

          return st2;
        }
      }
      return VarWalker::TranslateLabel(exp);
    }

    // Anything that starts with "allocate" and corresponds to a resource is
    // recorded as an allocation.
		Ptree* TranslateDotMember(Ptree* exp)
		{
			Ptree* exp2 = Walker::TranslateDotMember(exp);
			return exp2;
		}

    bool memoryQuery(Ptree *method)
    {
      return (Eq(method,"request_queue_empty") ||
              Eq(method,"request_queue_inactive") ||
              Eq(method,"next_level_actions_empty") ||
              Eq(method,"wait_cache_command"));
    }

    void checkConsistentMem(Ptree *obj,Ptree *method)
    {
      gc_string name, port;

      // This could be a dot-expression if we're dealing with ports.
      if (obj->IsA(ntDotMemberExpr)) {
        name = First(obj)->ToString();
        port = Third(obj)->ToString();
      } else {
        name = obj->ToString();
      }

      if (auto res = _ir.res().findResourceByName(name)) {
        if (res->isMemory()) {
          const Memory *mem = 0;

          // Used for indicating memory type.
          CacheType type = UnifiedCache;

          // Is this data or instruction memory?
          if (isA<Cache>(res)) {
            // If a cache, then look up the cache type in the ADL data model.
            auto cache = const_core->get_cache(res->name());
            if (!cache) {
              AddError(_ml,obj,"Unknown cache " << res->name());
              return;
            }
            type = cache->type();
          }
          else if ((mem = isA<Memory>(res)) && !port.empty()) {
            if (auto p = mem->findPortByName(port)) {
              type = p->type();
            } else {
              AddError(_ml,obj,"Unknown port within " << mem->name() << ": " << port);
              return;
            }
          }

          if (!memoryQuery(method)) {
            // Based upon the above analysis, assign first-level memories if not
            // already set.
            if ((type == UnifiedCache || type == DataCache) && !_ir._firstLevelDataMem) {
              _ir._firstLevelDataMem = dynamic_cast<const MemoryBase*>(res);
            } 
            
            if ((type == UnifiedCache || type == InstrCache) && !_ir._firstLevelInstMem) {
              _ir._firstLevelInstMem = dynamic_cast<const MemoryBase*>(res);
            } 
            
            // Now check for consistency.
            if ((type == UnifiedCache || type == DataCache) && (_ir._firstLevelDataMem != res)) {
              AddError(_ml,obj,"Multiple first-level data memories/caches:  " << _ir._firstLevelDataMem << " (current 1st) vs. " << obj);
            }
            
            if ((type == UnifiedCache || type == InstrCache) && (_ir._firstLevelInstMem != res)) {
              AddError(_ml,obj,"Multiple first-level instruction memories/caches:  " << _ir._firstLevelInstMem << " (current 1st) vs. " << obj);
            }
          }
        }
      }
    }

    // Certain global functions are allowed.  For the change-state function, we
    // require that its argument be a known state.  Also, check for various
    // kinds of space checks that we care about.
    Ptree* TranslateFuncall(Ptree* exp)
    {

      PtreeArray array;
      bool changed = false;

			Ptree *obj, *method;

      Ptree *exp2 = VarWalker::TranslateFuncall(exp);

      if (_ic) {
        Ptree *fun = exp2->Car();

        if (Match(fun,"[ %? . %? ]",&obj,&method)) {
          if (auto stage = is_stage(obj)) {
            if (Eq(method,"has_space") || Eq(method,"empty") || Eq(method,"no_capacity_check")) {
              // Found a space check, so add it.
              _action->_sc_checks.insert(stage->name());
            }
          } else {
            checkConsistentMem(obj,method);
          }
          _ic->add_func_call(method->ToString(),get_func_call_numargs(exp2));
        }
        else if (Eq(fun,"change_state")) {
          populateArgs(array,this,exp,0,changed);
        
          // Argument must be a valid state.          
          if (numArgs(array) != 1) {
            PError(exp,"Expected one argument for change_state:  change_state(<state>).");
          }
          // Check state-name validity.
          Ptree *s = getArg(array,0);
          assert(s);
          gc_string state = s->ToString();
        
          if (!_ic->machine().hasState(state)) {
            PError(exp,"Unknown state " << state);
          }

          if (!_action) {
            PError(exp,"Not currently in an action.");
          }
          // Record this as a to-state for this action.
          _action->_to_states.insert(state);
        }
        else if (Eq(fun,"exec")) {
          if (get_func_call_numargs(exp) != 0) {
            PError(exp,"Expected 0 arguments for exec().");
          }
          _action->_has_possible_excpt = _action->_has_exec = true;
        }
        else if (Eq(fun,"flush") || Eq(fun,"taken_flush")) {
          _action->_has_flush = true;
        }
        else if (Eq(fun,"exec_and_read") || Eq(fun,"exec_and_write")) {      
          populateArgs(array,this,exp,0,changed);
        
          int na = numArgs(array);
          if (na < 1) {
            PError(exp,"Expected one or more arguments for " << fun << ":  " << fun << "(<memory|cache>).");
          }

          // Do we need to tighten this up so that only the first-level
          // memory/cache is accessed?
          Ptree *arg = getArg(array,0);
          assert(arg);
          // This might be a dot-form notation to indicate a port.
          gc_string m,p;
          if (arg->IsA(ntDotMemberExpr)) {
            m = First(arg)->ToString();
            p = Third(arg)->ToString();
          } else {
            m = arg->ToString();
          }
          const MemoryBase *mb = 0;
          if (!((mb = _ir.res().findMemoryByName(m)) || (mb = _ir.res().findCacheByName(m)))) {
            PError(exp,"Expected a memory or cache argument for " << fun << ":  " << fun << "(<memory|cache>).");
          }
          if (!p.empty() && !mb->findPortByName(p)) {
            PError(exp,"Unknown port " << p);          
          }

          _action->_has_possible_excpt = _action->_has_exec = true;
        } else {
          _ic->add_func_call(fun->ToString(),get_func_call_numargs(exp2));
        }
      }
      return exp2;
    }

  };

  Ptree *get_block_body(Ptree *f)
  {
    Ptree *body = get_func_body(f);
    if (body) {
      // We expect each item to be an assignment statement
      // of the form:  edge = from-state -> to-state.
      if (Ptree *b = Second(body)) {
        return b;
      } else {
        PError(f,"Empty block-body encountered.");
      }
    } else {
      PError(f,"Malformed block.");
    }
  }

  void Base::dump() const
  {
    dump(cerr,"");
  }

  Resources::Resources() :
    _fetcher(0),
    _issueStage(0)
  {
  }

  template <class T>
  void copyResources(T &trg,const T &src)
  {
    ForEach(src,i) {
      trg.insert(make_pair(i->first,i->second->clone()));
    }
  }

  void Resources::copy(Resources &nr) const 
  {
    if (_fetcher) {
      nr._fetcher = _fetcher->clone();
    }
    copyResources(nr._pipelineStages,_pipelineStages);
    copyResources(nr._semaphores,_semaphores);
    copyResources(nr._flags,_flags);
    copyResources(nr._registerFiles,_registerFiles);
    copyResources(nr._forwardingPaths,_forwardingPaths);
    copyResources(nr._branchPredictors,_branchPredictors);
    copyResources(nr._customResources,_customResources);
    copyResources(nr._caches,_caches);
    copyResources(nr._memories,_memories);
  }

  Core *getFirstCore(SysItem *si)
  {
    assert(si);
    if (Core *core = dynamic_cast<Core*>(si)) {
      return core;
    } else if (System *sys = dynamic_cast<System*>(si)) {
      return getFirstCore(sys->items().front().second);
    } else {
      RError("Unknown type encountered for ADL object " << si->name() << ".");
    }
  }

  Core *getFirstCore (const System *sys,Core *core)
  {
    if (core) {
      return core;
    } else {
      return getFirstCore(const_cast<System*>(sys));
    }
  }

  bool is_shared(const Resources *parent_res,const gc_string &n) 
  {
    return parent_res && parent_res->findResourceByName(n);
  }

  void Resources::finalize(UadlIr &ir,Thread &thread,const System *sys,Core *core,const Resources *parent_res,
                           MsgLog &ml,const StrVect &path)
  {
    Core *mycore = getFirstCore(sys,core);

    // Now finalize the resources, but skip them if they're shared, since
    // they'll already have been finalized one level up.

    // Process our own stages first.
    ForEach(_pipelineStages,i) {
      LogPError(ml,{
          if (!is_shared(parent_res,i->first)) {
            PipelineStage &stage = *i->second;
            stage.finalize(path);
          }});
    }

    auto iter = _registerFiles.begin();
    auto end = _registerFiles.end();
    for ( ; iter != end; ) {
      if (!is_shared(parent_res,iter->first)) {
        RegisterFile &rf = *iter->second;
        LogPError(ml,rf.finalize(*mycore,ir,ml));

        // Remove if a bad item (doesn't map to a real register or register file.
        // Should this be an error, instead?
        if (rf.id() < 0) {
          auto tmp = iter++;
          _registerFiles.erase(tmp);
        } else {
          ++iter;
        }
        
        // No need to add registers to child threads, as the import step will take
        // care of it.
      } else {
        ++iter;
      }
    }
   
    ForEach(_forwardingPaths,i) {
      if (!is_shared(parent_res,i->first)) {
        LogPError(ml,i->second->finalize(*mycore,ml));
      }
    }
      
    ForEach(_branchPredictors,i) {
      if (!is_shared(parent_res,i->first)) {
        LogPError(ml,i->second->finalize(*mycore,ir,ml,ir._check_cr_args,ir._full_paths));
      }
    }
      
    ForEach(_customResources,i) {
      if (!is_shared(parent_res,i->first)) {
        LogPError(ml,i->second->finalize(*mycore,ir,ml,ir._check_cr_args,ir._full_paths));
      }
    }
      
    // TODO: we need to finalize memory-type managers in exact bottom-up order
    ForEach(_memories, i) {
      if (!is_shared(parent_res,i->first)) {
        LogPError(ml, i->second->finalize(*mycore, ml));
      }
    }
      
    ForEach(_caches,i) {
      if (!is_shared(parent_res,i->first)) {
        LogPError(ml,i->second->finalize(*mycore,*this,ir,ml));
      }
    }
      
    if (_fetcher) {
      if (thread.is_group()) {
        PError(thread.loc(),"Shared fetch units are not currently supported.");
      }
      LogPError(ml,_fetcher->finalize(*mycore,*this,ml));
    } else if (!thread.is_group()) {
      PError(thread.loc(),"No fetch unit defined.");
    }

  }

  void Resources::finalize_managers(UadlIr &ir,Thread &thread,Core *core,MsgLog &ml)
  {
    // Sort caches in ascending level.
    ForEach (_caches, i) {
      if (i->second->IssCache()) {
        auto j = _sortedCaches.begin();

        while (j != _sortedCaches.end() &&
               (*j)->IssCache()->level() < i->second->IssCache()->level())
          { j++; }

        _sortedCaches.insert(j, i->second);
      }
    }

    // After this point, ony execute for leaf-nodes (threads which map to
    // cores).
    if (!core) return;

    // We don't allow forwarding paths to share names with register items,
    // since that could cause ambiguity in transaction definitions.
    ForEach(_forwardingPaths, i) {
      if (findRegFileByName(i->first)) {
        AddError(ml, i->second->loc(), "Forwarding path '" << i->second->name() <<
                 "' conflicts with a register of the same name.");
      }
    }

    // check cross-references between managers
    // check: first-level instruction memory defined
    const MemoryPort *firstLevelInstMemPort = 0;
    auto firstLevelInstMem = dynamic_cast<const MemoryBase*>(findResourceByName(fetcher()->fetchMemory()));
    if (!firstLevelInstMem) {
      AddError(ml, _fetcher->loc(), "Unknown fetch memory/cache:  " << _fetcher->fetchMemory());
    }
    else if (!(firstLevelInstMemPort = firstLevelInstMem->findPortByName(fetcher()->fetchMemoryPort()))) {
      AddError(ml, _fetcher->loc(), "Unknown fetch memory/cache port:  " << _fetcher->fetchMemoryPort());
    }

    if (!ir._firstLevelInstMem) {
      ir._firstLevelInstMem = firstLevelInstMem;
    } else if (firstLevelInstMem != ir._firstLevelInstMem) {
      AddError(ml,thread.loc(),"Multiple first-level instruction memories/caches:  " << ir._firstLevelInstMem << " (current 1st) vs. " << firstLevelInstMem);
    }

    if (!ir._firstLevelInstMemPort) {
      ir._firstLevelInstMemPort = firstLevelInstMemPort;
    } else if (firstLevelInstMemPort != ir._firstLevelInstMemPort) {
      AddError(ml,thread.loc(),"Multiple first-level instruction ports:  " << ir._firstLevelInstMemPort << " (current 1st) vs. " << firstLevelInstMemPort);
    }

    // find issue stage
    ForEach(_pipelineStages, i) {
      const PipelineStage &stage = *i->second;

      if (stage.issue()) {
        if (!_issueStage) {
          _issueStage = &stage;
        }
        else {
          AddError(ml, stage.loc(), "More than one issue stage defined.");
          AddError(ml, _issueStage->loc(), "  First issue stage defined here.");
        }
      }
    }

    // check: if no ADL memory defined and has multiple uADL memories, must be one
    //        instruction memory and one data memory
    if (core->memories().empty() && _memories.size() > 1) {
      if (_memories.size() != 2) {
        // find the third memory
        Memories::const_iterator i = _memories.begin();
        i++; i++;

        AddError(ml, i->second->loc(), "only two memories (one for instruction" <<
                 ", one for data) are allowed when no ADL memories are defined");
      }
      else {
        // TODO: traverse the memory hierarchy to check one is instruction memory
        //       and another is data memory
      }
    }
  }

  void Resources::add_imported_reg(const gc_string &name,bool is_reg)
  {
    if (!findRegFileByName(name)) {
      _registerFiles.insert(make_pair(name,new RegisterFile(0, 0, name, is_reg)));
    }
  }

  void Resources::add_imported_cache(const gc_string &name,const adl::Cache *adl_cache)
  {
    if (!findCacheByName(name)) {
      auto ip =  _caches.insert(make_pair(name, new Cache(0, 0, name, adl_cache)));
      Cache &cache = *(ip.first->second);
      
      if (adl_cache->next()) cache._memory = adl_cache->next()->name();
    }
  }

  void Resources::add_imported_memory(const gc_string &name)
  {
    if (!findMemoryByName(name)) {
      _memories.insert(make_pair(name, new Memory(NULL, NULL, name)));
    }
  }

  // This imports resources from the ADL model which aren't explicitly specified
  // in the uADL description.  For example, the user doesn't have to list
  // registers explicitly in uADL- we'll import them automatically.
  void Resources::import_managers(const System *sys,Core *core, const Resources *parent_res, MsgLog &ml)
  {
    // Import ADL registers.
    if (sys) {
      // We have a system, so we insert shared resources.
      ForEach(sys->shared_regs(),i) {
        add_imported_reg(i->first,true);
      }
    } else {
      // We're just dealing with a core, so import all of these items.
      ForEach(core->regs(), i) {
        add_imported_reg(i->first,true);
      }
    }

    // Import ADL registers.
    if (sys) {
      // We have a system, so we insert shared resources.
      ForEach(sys->shared_regfiles(),i) {
        add_imported_reg(i->first,false);
      }
    } else {
      // We're just dealing with a core, so import all of these items.
      ForEach(core->regfiles(), i) {
        add_imported_reg(i->first,false);
      }
    }

    // import ADL caches
    if (sys) {
      // We have a system, so we insert shared resources.
      ForEach(sys->shared_caches(),i) {
        add_imported_cache(i->first,sys->get_cache(i->first));
      }
    } else {
      // We're just dealing with a core, so import all of these items.
      ForEach(core->caches(), i) {
        add_imported_cache(i->first,i->second);
      }
    }

    // import ADL memories
    if (sys) {
      // If we're dealing with a sub-system and there are no memories, then import the parents'.
      if (sys->shared_memories().empty() && _memories.empty()) {
        if (parent_res && !parent_res->_memories.empty()) {
          _memories.insert(*parent_res->_memories.begin());
        }
      } else {
        // We have a system, so we insert shared resources.
        ForEach(sys->shared_memories(),i) {
          add_imported_memory(i->first);
        }
      }
    } else {
      // We're just dealing with a core, so import all of these items.
      if (core->memories().empty() && _memories.empty()) {
        // If there are no memories, and the parent has the default memory, then
        // import it now so that the core has a memory to use.
        if (parent_res && !parent_res->_memories.empty()) {
          _memories.insert(*parent_res->_memories.begin());
        }
      } else {
        ForEach(core->memories(), i) {
          add_imported_memory(i->first);
        }
      }
    }

    // Create a global memory implicitly if we're at the top and no memory was defined.
    if (!parent_res && _memories.empty()) {
      _memories.insert(make_pair(adl::MemoryName,
                                 new Memory(NULL, NULL, adl::MemoryName)));
    }
  }

  void Resources::dump(std::ostream &o,const gc_string &pfx) const
  {
    if (_fetcher) {
      _fetcher->dump(o,pfx);
      o << "\n";
    }

    if (!_pipelineStages.empty()) {
      ForEach(_pipelineStages,i) {
        i->second->dump(o,pfx);
      }
      o << "\n";
    }

    if (!_semaphores.empty()) {
      ForEach(_semaphores,i) {
        i->second->dump(o,pfx);
      }
      o << "\n";
    }

   if (!_flags.empty()) {
      ForEach(_flags,i) {
        i->second->dump(o,pfx);
      }
      o << "\n";
    }

    if (!_registerFiles.empty()) {
      ForEach(_registerFiles,i) {
        i->second->dump(o,pfx);
      }
      o << "\n";
    }

    if (!_forwardingPaths.empty()) {
      ForEach(_forwardingPaths,i) {
        i->second->dump(o,pfx);
      }
      o << "\n";
    }

    if (!_branchPredictors.empty()) {
      ForEach(_branchPredictors,i) {
        i->second->dump(o,pfx);
      }
      o << "\n";
    }

    if (!_memories.empty()) {
      ForEach(_memories, i) i->second->dump(o, pfx);
      o << "\n";
    }

    if (!_caches.empty()) {
      ForEach(_caches,i) {
        i->second->dump(o,pfx);
      }
      o << "\n";
    }

    if (!_customResources.empty()) {
      ForEach(_customResources,i) {
        i->second->dump(o,pfx);
      }
      o << "\n";
    }

  }

  template <class T>
  const T *findByName(const gc_map<gc_string,T *> &container,const gc_string &name)
  {
    IfFind(i,container,name) {
      return i->second;
    } else {
      return 0;
    }
  }

  const Resource* Resources::findResourceByName(const gc_string &name) const
  {
    if (_fetcher && _fetcher->name() == name) {
      return _fetcher;
    }

    if (auto s = findStageByName(name)) {
      return s;
    }

    if (auto s = findSemaphoreByName(name)) {
      return s;
    }

    if (auto s = findFlagByName(name)) {
      return s;
    }

    if (auto s = findRegFileByName(name)) {
      return s;
    }

    if (auto s = findFwdPathByName(name)) {
      return s;
    }

    if (auto s = findBranchPredictorByName(name)) {
      return s;
    }

    if (auto s = findCustomResourceByName(name)) {
      return s;
    }

    if (auto s =  findMemoryByName(name)) return s;

    if (auto s = findCacheByName(name)) {
      return s;
    }

    return 0;
  }

  const PipelineStage* Resources::findStageByName(const gc_string &name) const
  {
    return findByName(_pipelineStages,name);
  }

  const Semaphore* Resources::findSemaphoreByName(const gc_string &name) const
  {
    return findByName(_semaphores,name);
  }

  const Flag* Resources::findFlagByName(const gc_string &name) const
  {
    return findByName(_flags,name);
  }

  const RegisterFile* Resources::findRegFileByName(const gc_string &name) const
  {
    return findByName(_registerFiles,name);
  }

  const ForwardingPath* Resources::findFwdPathByName(const gc_string &name) const
  {
    return findByName(_forwardingPaths,name);
  }

  const BranchPredictor* Resources::findBranchPredictorByName(const gc_string &name) const
  {
    return findByName(_branchPredictors,name);
  }

  const CustomResource* Resources::findCustomResourceByName(const gc_string &name) const
  {
    return findByName(_customResources,name);
  }

  const Cache* Resources::findCacheByName(const gc_string &name) const
  { 
    return findByName(_caches, name); 
  }

  const Memory* Resources::findMemoryByName(const gc_string &name) const
  { 
    return findByName(_memories, name); 
  }

  bool UadlIr::_full_paths = true;

  // UadlIr
  UadlIr::UadlIr(ADL_IR &iss_ir,
                 GenDriverIF *iss_driver,
                 const gc_string &iss_hfilename, 
                 const gc_string &uArchHFilename,
                 bool check_cr_args,
                 bool full_paths) :
    Base(0,0),
    _timeOut(0),
    _top(0),
    _thread(0),
    _iss_ir(iss_ir),
    _iss_hfilename(iss_hfilename), 
    _uArchHFilename(uArchHFilename),
    _packet_map(iss_driver->packet_map()),
    _instr_slots(0),
    _dm_slots(0),
    _maxRegFileId(0), 
    _maxRegWidth(0), 
    _maxOperands(0),
    _maxInstrSize(0),
    _minInstrSize(0),
    _commit_on_exception(true),
    _allow_unimpl_instrs(false),
    _check_cr_args(check_cr_args),
    _firstLevelDataMem(0),
    _firstLevelInstMem(0),
    _lastLevelInstrMem(0),
    _firstLevelInstMemPort(0),
    _firstLevelDataMemPort(0),
    _branch_hook(0),
    _pipeindex(0)
  {
    _full_paths = full_paths;
  }

  const adl::Core &UadlIr::core() const
  {
    assert(_thread->_core);
    return *_thread->_core;
  }

  adl::Core &UadlIr::core()
  {
    assert(_thread->_core);
    return *_thread->_core;
  }

  bool UadlIr::hasExceptions() const
  {     
    return !core().exceptions().empty(); 
  }


  const gc_string& UadlIr::CIAName() const
  { 
    return core().cia()->name(); 
  }


  const gc_string& UadlIr::NIAName() const
  { 
    return core().nia()->name(); 
  }

  const InstrSlotAlloc &UadlIr::get_slots(const gc_string &instrname) const
  {
    if (instrname == DecodeMissStr) {
      return *_dm_slots;
    } 
    else if (instrname == FetchExceptionStr) {
      return empty_slots;
    }
    auto slotIt = _instr_slots->find(instrname);
    assert(slotIt != _instr_slots->end());
    return slotIt->second;
  }

  // Check if name refers to a core defined by ADL
  Core *UadlIr::findCore(MsgLog &ml,Ptree *loc,const gc_string &name)
  {
    Core *core;
    if (!(core = _iss_ir.get_core(name))) {
      if (_iss_ir.cores().size() == 1) {
        // only one core defined by ADL, use it anyway
        core = _iss_ir.cores().begin()->second;
        AddWarning(ml,loc,"Core " << name << " is not defined by ADL, using core "
                   << core->name() << " instead.");
        return core;
      } else {
        return 0;
      }
    } else {
      return core;
    }
  }

  // When a core is found, this does the setup of various stuff, such as slot allocation, etc.
  void UadlIr::setupCore(Core *core,Environment *env)
  {
    // Just as with the outer-most scope, we add the uADL's environment as a
    // using-class to the ADL's core environment so that helpers declared in
    // uADL are seen when checking code.
    core->env()->AddBaseclassEnv(env);
    if (!_instr_slots) {
      // Find slot allocations for this core.
      auto iter = _packet_map.find(core);
      assert(iter != _packet_map.end());
      
      GenInstrPacketBaseIF genip(iter->second);
      
      _instr_slots = genip.instr_slots();
      _dm_slots    = &genip.decode_miss_slots();
      _maxOperands = genip.max_slots();
    }
  }

  // Check if name refers to a system defined by ADL
  const System *UadlIr::findSys(const gc_string &name)
  {
    return _iss_ir.get_sys(name);
  }

  // Handles setup of the top-level block vs. the top of the ADL hierarchy.
  void UadlIr::declareTop(MsgLog &ml,Environment *env,Ptree *loc,const gc_string &name)
  {
    if (_top) {
      _loc = loc;
      if (name != _archName) {
        PError(loc,"A core or system has already been defined with the name " << _archName);
      }
    } else {
      _top = new Thread(env,loc,name);
      _loc = loc;
      _uArchName = name;
      
      // First, see if this is a core.  If so, then we won't allow threads to be
      // declared.  If this is a system, then we expect there to be threads in the
      // design.
      if (Core *core = findCore(ml,loc,name)) {
        // It's a core.
        _top->_core = core;
        _archName = core->name();
        setupCore(core,env);
      } else if (const System *sys = findSys(name)) {
        // It's a system.
        _top->_system = sys;
        _archName = sys->name();
      } else {
        PError(loc,"Core or system " << name << " is not defined by ADL.\n");
      }
    }
  }

  // This declares a thread in the thread hierarchy.
  void UadlIr::declareThread(Thread &parent,Thread &thread,bool is_group)
  {
    // If we're declaring a new thread, then the parent thread can't already match up
    // to a core.
    if (parent._core) {
      PError(thread.loc(),"A thread or thread-group cannot be added to a thread.");
    }
    if (is_group) {
      // If it's a group, then this item must correspond to a system in the parent object.
      if (const System *sys = parent._system->get_sys(thread.name())) {
        // Item exists, so store a pointer to the system.
        thread._system = sys;
      } else {
        PError(thread.loc(),"System " << thread.name() <<" is not defined by ADL.\n");
      }
    } else {
      // Not a group, so it must correspond to just a core in ADL.
      if (Core *core = const_cast<Core*>(parent._system->get_core(thread.name()))) {
        // Item exists, so create a new thread, add it, then return it.
        thread._core = core;
        setupCore(core,_env);
      } else {
        PError(thread.loc(),"Core " << thread.name() <<" is not defined by ADL.\n");
      }      
    }
  }

  void UadlIr::add_mem_alias(const gc_string &src,Ptree *trg)
  {
    _mem_aliases[src] = trg;
  }

  bool UadlIr::has_value_queries() const
  {
    ForEach(_instClasses,i) {
      const InstClass &ic = *i->second;
      if (has_value_query(ic)) {
        return true;
      }
    }
    return false;
  }

  unsigned UadlIr::maxMemId() const
  {
    unsigned maxId = 0;

    ForEach(thread().core()->memories(), i)
      if (i->second->id() > maxId) maxId = i->second->id();

    // "+ 1" is safe for empty _memories
    if (res()._memories.size() > maxId + 1) maxId = res()._memories.size() - 1;

    return maxId;
  }


  bool UadlIr::isCoreHelper(const gc_string &name) const
  {
    const HelperMap &helpers = thread().core()->helpers();

    return (helpers.find(name) != helpers.end());
  }


  bool UadlIr::isHelper(const gc_string &name) const
  {
    return _helpers.count(name);
  }

  const HelperInfo *UadlIr::get_helper(const gc_string &name) const
  {
    HelperMap::const_iterator i = _helpers.find(name);
    return (i == _helpers.end()) ? 0 : &(i->second);
  }


  // Check is operand access of the right type. 
  void UadlIr::check_operand_access(MsgLog &ml, Ptree *func_call) {
    const gc_string name = get_func_call_name(func_call)->ToString();
    // The parameter must be a valid instruction field.
    int num_args = get_func_call_numargs(func_call);
    if (num_args == 0 || num_args > 2) {
      AddError(ml,func_call,"The function of instruction field should take 1 or 2 parameters:" 
               << func_call);
    }

    if (num_args > 0) {
      // Check first parameter.
      const char *n = get_ident(get_func_call_arg(func_call,0),"instruction-field specifier.");
      IfNotFind(tmp,thread().core()->instrfields(),n) {
        AddError(ml,func_call,"Parameter is not an instruction-field:  " << n);
      }
    }

    if (num_args > 1) {
      // Check second parameter.
      const char *n = get_ident(get_func_call_arg(func_call,1),"instruction-field specifier.");
      IfNotFind(tmp,thread().core()->instrfields(),n) {
        AddError(ml,func_call,"Parameter is not an instruction-field:  " << n);
      }
    }

    // Now check that a helper is a good one.
    IfFind(h,thread().core()->helpers(),name) {
      
      Ptree *func = h->second._func;
      
      Ptree* decl = Third(func);
      
      if (get_func_call_numargs(decl) != num_args) {
        AddError(ml,func_call,"Missmatch in number of argumenes for '" << name << "'.");
      }
      
      if (!arg_is_uint(decl,0))	{
        AddError(ml,func_call,"Not valid helper '" << name << "', first argument should be unsigned.");
      }

      if (num_args > 1 && !arg_is_uint(decl,1)) {
        AddError(ml,func_call,"Not valid helper '" << name << "', second argument should be unsigned.");
      }
    }  else {
      AddError(ml,func_call,"Not found helper '" << name << "'.");
    }
  }
 
  // This adds the instruction classes as an enum to the front-end database as
  // an additional type declaration, so that a simulation will have this code.
  // It also registers each enum element as a local resource, usable by user
  // code in the core.
  void UadlIr::addInstClassType()
  {

    ostringstream ss;

    ss << "// uADL Instruction Classes\n"
       << "enum " << getInstrClassEnumTypeName() << " {\n"
       << "unknownClass = 0,\n";

    ForEach(_instClasses,i) {
      gc_string t = i->second->getTypeName();
      ss << t << ",\n";
      thread().core()->add_local_resource(t);
    }

    ss << "maxClass\n"
       << "};\n\n";

    auto const &attrs = InstClass::get_ic_attrs();
    adl::IntMap bfvals;
    if (!attrs.empty()) {
      ss << "// uADL Instruction Class Attributes\n"
         << "enum " << getInstrClassAttrTypeName() << " {\n";
      ForEach(attrs,i) {
        ss << i->first << " = 0x" << hex << i->second << ",\n";
        thread().core()->add_local_resource(i->first);
      }
      ss << dec << "};\n\n";
    }
    
    thread().core()->add_type_decl(Ptree::Make(ss.str().c_str()));
  }

   // Push down relevant shared resources to the next level of threads.
  void UadlIr::pushSharedResources(Thread &thread,MsgLog &ml)
  {   
    ForEach(thread._threads,titer) {
      Thread &child = *titer->second;
      Resources &child_res = child._res;
      Resources &res = thread._res;
      ForEach(res._pipelineStages,i) {
        child_res._pipelineStages.insert(make_pair(i->first,i->second));
      }
      ForEach(res._semaphores,i) {
        child_res._semaphores.insert(make_pair(i->first,i->second));
      }
      ForEach(res._flags,i) {
        child_res._flags.insert(make_pair(i->first,i->second));
      }
      ForEach(res._forwardingPaths,i) {
        child_res._forwardingPaths.insert(make_pair(i->first,i->second));
      }
      ForEach(res._branchPredictors,i) {
        child_res._branchPredictors.insert(make_pair(i->first,i->second));
      }
      ForEach(res._customResources,i) {
        child_res._customResources.insert(make_pair(i->first,i->second));
      }
      ForEach(res._memories,i) {
        child_res._memories.insert(make_pair(i->first,i->second));
      }
      ForEach(res._caches,i) {
        child_res._caches.insert(make_pair(i->first,i->second));
      }
      
      // Add implicit token managers
      child.import_managers(&res,ml);

      // Recursively push resources down to the children.
      pushSharedResources(child,ml);
    }
  }

  
  // Final setup/checking.
  void UadlIr::finalize(MsgLog &ml)
  {
    // Assign bit flags to instruction class attributes.
    InstClass::finalize_attrs();

    // Create the thread hierarchy.  What we have after parsing consists of just
    // the threads' types.  We transform this into a hierarchy representing the
    // thread instances.
    LogPError(ml,createThreadHierarchy());

    // Distribute shared resources, recursively, to all constituent threads.
    _top->import_managers(0,ml);
    pushSharedResources(*_top,ml);

    // Then finalize all of these resources.
    _top->finalize(*this,0,ml,StrVect());

    Core *core = _top->get_core();

    // Take all environments and add them as base-classes, except for the last
    // one seen, which is the current environment.
    ForEach(_all_envs,i) {
      if (*i != _env) {
        _env->AddBaseclassEnv(*i);
      }
    }

    if (_mem_access) {
      AuxFunc *af = thread().core()->add_aux_func(_mem_access._p,"void","Memory-access hook.");
      Ptree *f = af->func();
      if (!arg_is_type(f,"UadlMemAccess",0)) {
        PError(f,"First argument to memory-access hook must be of type UadlMemAccess.");
      } 
      // Convert argument to a reference.
      af->add_arg_mod("uadl::UadlMemAccess &");
      _mem_access._d = af;
    }

    finalize_managers(ml);

    process_mem_aliases(*core,ml);

    ForEach(_machines,i) {
      LogPError(ml,i->second->finalize(*this,ml));
    }

    if (_machines.empty() && !_instClasses.empty()) {
      PError(loc(),"No machines were defined for this core.");
    }

    // Finalizing instruction classes is a two-step process: First, we figure
    // out what instructions go where, then check for missing instructions, and
    // add any extra instructions to a class which defines the special
    // "default_instruction" pseudo-instruction.  Then we go through and process
    // each instruction class.
    ForEach(_instClasses, i) LogPError(ml, i->second->process_instr_membership(*this, ml));

    process_unimpl_instrs(*thread().core(),ml);

    ForEach(_instClasses, i) LogPError(ml, i->second->finalize(*this, ml));

    if (!core->exceptions().empty()) {
      int fe_present = 0;
      ForEach(_instClasses,i) {
        if ( i->second->hasFetchException()) {
          ++fe_present;
        }
      }
      if (fe_present > 1) {
        AddError(ml,loc(),"More than one instruction class handles the fetch-exception pseudo-instruction.");
      }
    }

    if (core->decode_miss_handler()) {
      int dm_present = 0;
      ForEach(_instClasses,i) {
        if ( i->second->hasDecodeMiss()) {
          ++dm_present;
        }
      }
      if (dm_present > 1) {
        AddError(ml,loc(),"More than one instruction class handles the decode-miss pseudo-instruction.");
      }
    }

		// Once we've processed the instruction classes, we can then figure out the
		// scheduling order for the stages.
		if (!_instClasses.empty()) {
			processScheduling(ml);
		}

    // Check that a first-level data memory is defined.
    if (!_firstLevelDataMem)
      AddWarning(ml, loc(), "Instructions access no data memory");

    ForEach(res()._caches, i) {
      Cache *cache = i->second;

      if (cache->preemptive() &&
          (cache != firstLevelDataMem() || cache != firstLevelInstMem()))
        {
          AddWarning(ml, cache->loc(), cache->name() << ": only the first-level " <<
                     "unified memory/cache can be preemptive");
          cache->disablePreemptive();
        }
    }

    ForEach(res()._memories, i) {
      Memory *mem = i->second;

      if (mem->preemptive() &&
          (mem != firstLevelDataMem() || mem != firstLevelInstMem()))
        {
          AddWarning(ml, mem->loc(), mem->name() << ": only the first-level " <<
                     "unified memory/cache can be preemptive");
          mem->disablePreemptive();
        }
    }

    // Add in the instruction-class enum and add the enum elements as valid global
    // resources for aux functions.
    addInstClassType();

    // Finalize any auxiliary functions added by any managers.
    core->check_aux_funcs(ml);

    // Finalize all helpers which might have been added.
    process_helpers(*core,ml);

    setupLastLevelInstrMem();
  }

  void UadlIr::setupLastLevelInstrMem()
  {
    const Resource *instrMem = _firstLevelInstMem;
    while (instrMem) {
      if (auto m = isA<Memory>(instrMem)) {
        _lastLevelInstrMem = m;
        break;
      }
      else if (auto c = isA<Cache>(instrMem)) {
        instrMem = res().findResourceByName(c->nextLevelMemory());
      }
    }

  }

  void UadlIr::createThreadHierarchy()
  {
    // The new hierarchy goes here and will replace what exists at this point,
    // once complete.  This only needs to be done for a multi-threaded design,
    // since otherwise the hierarchy consists of just the top-level item.
    if (_top->is_group()) {
      _top = replicateThreads(_top->_name,*_top);
    } else{
      _thread = _top;
    }
  }

  Thread *UadlIr::replicateThreads (const gc_string &name,Thread &oh)
  {
    Thread *nh = oh.clone(name);

    if (oh.is_group()) {
      // We only support a symmetrical threading model, so we just need to take
      // the first child from the thread hierarchy and then replicate it
      // according to the ADL hierarchy.
      if (oh._threads.empty()) {
        PError(oh.loc(),oh.name() << ":  Thread group has no groups or threads defined.");
      }
      Thread *first = oh._threads.begin()->second;
      const System *sys = oh.sys();
      ForEach(sys->items(),i) {
        const SysItem *item = i->second;
        if (item->name() == first->name()) {
          Thread *nc = replicateThreads(i->first,*first);
          nc->_parent = nh;
          nh->_threads.insert(make_pair(i->first,nc));
        }
      }
    } else {
      // Nothing to do for a leaf node, but store a pointer to the first leaf
      // node encountered.
      if (!_thread) {
        _thread = nh;
      }
    }
    return nh;
  }

  void UadlIr::finalize_managers(MsgLog &ml)
  {
    _top->finalize_managers(*this,ml);

    // find maximum instruction size and minimum instruction size
    ForEach(core().instrs(), i) {
      if (_maxInstrSize < i->second->width()) _maxInstrSize = i->second->width(); 

      if (!_minInstrSize || _minInstrSize > i->second->width())
        _minInstrSize = i->second->width();
    }

    _maxInstrSize = _maxInstrSize / 8;
    _minInstrSize = _minInstrSize / 8;

    // find instruction size GCD
    bool GCDFound = false;

    for (_instrSizeGCD = _minInstrSize; !GCDFound && _instrSizeGCD > 1; _instrSizeGCD--) {
      GCDFound = true;
      
      for (adl::InstrHash::const_iterator i = core().instrs().begin();
           GCDFound && i != core().instrs().end(); i++) { 
        unsigned wb = (i->second->width() / 8);
        if ( wb % _instrSizeGCD) {
          GCDFound = false; 
        }
      }
      if (GCDFound) break;
    }

    if (!GCDFound) _instrSizeGCD = 1;
  }

  // Check any uADL helper functions.
  void UadlIr::process_helpers(Core &core,MsgLog &ml)
  {
    // Use a walker to perform the translation step.
    ActionWalker w(env(),*this,core,0,ml);
    w.set_return_ok(true);

    ForEach(_helpers,i) {
      w.Translate(i->second._func);
      VarList escapes = w.get_escapes();
      ForEach(escapes,i) {
        Ptree *e = i->first;
        AddError(ml,e,"Unknown variable/resource found in action code:  " << e);
      }
    }

    if (_branch_hook) {
      w.Translate(_branch_hook);
      VarList escapes = w.get_escapes();
      ForEach(escapes,i) {
        Ptree *e = i->first;
        AddError(ml,e,"Unknown variable/resource found in branch hook:  " << e);
      }
    }
  }

  // Check to make sure that each memory aliases is valid.  We're checking the rhs to make sure that
  // it corresponds to a valid memory type, e.g. a memory object or a memory object with port.
  void UadlIr::process_mem_aliases(Core &core, MsgLog &ml)
  {
    ForEach(_mem_aliases,i) {
      Ptree *ma = i->second;

      Ptree *mem = 0, *port = 0;
      if (ma->IsLeaf()) {
        mem = ma;
      }
      else if (Match(ma,"[ %? . %? ]",&mem,&port)) {
        ;
      } else {
        AddError(ml,i->second,"Bad memory target for memory alias " << i->first << " -> " << i->second);
        continue;
      }

      const MemoryBase *mb = 0;
      gc_string mname = mem->ToString();
      if (!(mb = res().findMemoryByName(mname))) {
        if (!(mb = res().findCacheByName(mname))) {
          AddError(ml,i->second,"Unknown target for memory alias:  " << i->first << " -> " << i->second);
          continue;
        }
      }
      if (port && !mb->findPortByName(port->ToString())) {
        AddError(ml,i->second,"Unknown port '" << port << "' for memory alias " << i->first << " -> " << i->second);
        continue;
      }
    }
  }

  void UadlIr::process_unimpl_instrs(Core &core, MsgLog &ml)
  {
    // First, figure out what instructions are not covered by any class.
    StrList unimpls;
    StrSet impls;
    InstClass *default_class = 0;

    ForEach(_instClasses, i) {
      InstClass &ic = *i->second;
      if (ic.hasDefaultInstr()) {
        default_class = &ic;
      }
      ForEach(ic.instrs(), j) {
        impls.insert(j->_name);
      }
    }

    ForEach(core.instrs(),i) {
      const Instr &instr = *i->second;
      const gc_string &iname = instr.name();
      if (instr.is_used() && !impls.count(iname)) {
        unimpls.push_back(iname);
      }
    }

    // Does any class contain the special "default_instruction" tag?  If so, add
    // all unimplemented instructions to it.
    if (default_class) {
      ForEach(unimpls,i) {
        default_class->_instrs.push_back(InstItem(*i));
      }
    } else if (!_allow_unimpl_instrs && !unimpls.empty()) {
      // No, so if we don't allow unimplemented instructions, then produce an
      // error.
      gc_string msg = "Unimplemented instructions:  ";
      ForEach(unimpls,i) {
        msg += *i + ' ';
      }
      AddError(ml,loc(),msg);
    }
  }

	struct GNode : public gc {
		enum State {
			NotVisited = -2,
			Visited = -1,
		};

		enum Type {
			First,
			Normal,
			End,
		};

		GNode(Type type) : _type(type), _state(NotVisited), _stage(0), _distance(0) {};
		GNode(const PipelineStage *stage) : _type(Normal), _state(NotVisited), _stage(stage), _distance(0) {};
		bool visited() const { return _state != NotVisited; };
		bool is_end() const { return _type == End; };
		bool is_first() const { return _type == First; };
		bool empty() const { return _edges.empty(); };

		Ptree *loc() const  { return (_stage) ? _stage->loc() : 0; };

		void add_edge(GNode *e) {
			// Avoid self-loops.
			if (e != this) {
				_edges.insert(e);
			}
		}

		void name(ostream &o) const {
			if (is_first()) {
				o << "<first>";
			}
			else if (is_end()) {
				o << "<end>";
			}
			else if (_stage) {
				o << _stage->name();
			}
			else {
				o << "<bad>";
			}
		}

		Type                 _type;
		State                _state;
		const PipelineStage *_stage;
		int                  _distance;
		gc_set<GNode *>      _edges;
	};

	typedef gc_map<gc_string,GNode *> NodeMap;
  typedef gc_list<GNode *> NodeList;
  typedef gc_list<const Thread *> ThreadList;

	ostream &operator<< (ostream &o,const GNode &n)
	{
	  n.name(o);
		o << " (" << n._distance << "):  ";
		ForEach(n._edges,i) {
			GNode &en = **i;
			en.name(o);
			o << " ";
		}
		return o;
	}

	ostream &operator<< (ostream &o,const NodeMap &nm)
	{
		ForEach(nm,i) {
			o << *(i->second) << "\n";
		}
		return o;
	}

	// The graph traversal function (see description in processScheduling).
	int traverseGraph(GNode *node)
	{
		if (node->is_end() || node->visited()) {
			// We're at the end, so our starting distance is 0.
			return node->_distance;
		} else {
			// First, make sure that, if this isn't the end node, that we have
			// out-bound edges.  Otherwise, it's an error.
			if (node->empty()) {
				if (node->is_first()) {
					PError(0,"Initial state has no exiting states.");
				} else {
					PError(node->loc(),"No exiting states for pipeline stage " << node->_stage->name());
				}
			}
			
			// Mark as visited to avoid cycles.
			node->_state = GNode::Visited;
			
			// Visit each edge.  We use the max of the resulting distances to calculate
			// our own distance.
			int distance = 0;
			ForEach((node->_edges),i) {
				GNode *to_node = *i;
				distance = max(distance,traverseGraph(to_node));
			}
			node->_distance = distance + 1;
			return node->_distance;
		}
	}

	struct SortNodes {
		bool operator()(const GNode *x,const GNode *y) const {
			return x->_distance < y->_distance;
		}
	};

  void insertAllStages(NodeMap &nodes,ThreadList &threads)
  {
    ForEach(threads,i) {
      const PipelineStages &stages = (*i)->res().pipelineStages();
      ForEach(stages,p) {
        auto ip = nodes.insert(make_pair(p->second->full_name(),(GNode*)0));
        if (ip.second) {
          ip.first->second = new GNode(p->second);
        }
      }
    }
  }

  void getThreads(ThreadList &threads,const Thread &thread)
  {
    if (thread.is_group()) {
      ForEach(thread.threads(),i) {
        getThreads(threads,*i->second);
      }
    } else {
      threads.push_back(&thread);
    }
  }

	// Determine the scheduling order of the pipeline stages.  We do this by first
	// creating a graph of all stage transitions, then doing a depth first
	// traversal, numbering the stages in terms of distance from the initial state
	// (in reverse order).  So, for example, the last stage in the pipeline (which
	// transitions back to the init state) will have a value of 0.
	//
	// To do this, we first construct the graph.  Then we mark all nodes as
	// not-visited.  Then we traverse.  Before visiting child nodes, we mark
	// the node as visited, to avoid infinite-recursion on cycles.  Then,
	// when we reach the init state (which indicates the end), we mark the node
	// and return the distance + 1, in order to increase the value as we unwind.
	// As we unwind, the node's value is set to the max of its current value, if
	// already given a distance, and the new distance, so that early-out paths are
	// essentially ignored.
	//
	// When done, we sort based upon distance to produce the scheduling order.  In
	// the trivial case, a basic in-order pipeline, the scheduling order is simply
	// the stages in reverse order.
	void UadlIr::processScheduling(MsgLog &ml)
	{
		// All nodes in the graph.
		NodeMap nodes;
		gc_string start_name = "$init$", end_name = "$end$";
		GNode *start_node = new GNode(GNode::First);
		GNode *end_node = new GNode(GNode::End);
		nodes[start_name] = start_node;
		nodes[end_name] = end_node;

    // Create a list of thread leaf-nodes.
    ThreadList threads;
    getThreads(threads,*_top);

    // We insert all stages from all threads; duplicates are naturally removed
    // via the map's insert operation.
    insertAllStages(nodes,threads);

		// For each instruction class, for each state, get the to-states.  Add an
		// edge to the corresponding graph node if it's not a self-loop.  We do this
		// for each thread in the system so that stages which are duplicated amongst
		// threads get this information.
		ForEach(_instClasses,i_iter) {
			const InstClass &ic = *(i_iter->second);
			const Actions &actions = ic.actions();

      ForEach(threads,i) {
        const Thread &thread = **i;
        ForEach(actions,a_iter) {
          const Action &action = *(a_iter->second);
          GNode *node = 0;
          if (ic.isInitState(action.state())) {
            // Initial state situation.
            node = start_node;
          } else {
            // Normal stage-mapped node situation.
            auto stage = ic.findStageBinding(thread,action.state());
            assert(stage);
            node = nodes[stage->full_name()];
          }
          assert(node);
          ForEach(action.to_states(),to_iter) {
            if (ic.isInitState(*to_iter)) {
              // End-state situation.
              node->add_edge(end_node);
            } else {
              // Normal state situation.
              auto to_stage = ic.findStageBinding(thread,*to_iter);
              if (!to_stage) {
                PError(loc(),"Could not find mapping for to-state " << *to_iter);
              }
              auto to_node = nodes[to_stage->full_name()];
              assert(to_node);
              node->add_edge(to_node);
            }
          }
        }
			}
		}

		//cout << "Pre-traversed graph:\n" << nodes << "\n";

		// Now we do the graph traversal.
		LogPError(ml,traverseGraph(start_node));

		//cout << "Post-traversed graph:\n" << nodes << "\n";

		// Now we need to sort the resulting nodes by distance.
		gc_vector<GNode*> sorted;
		ForEach(nodes,i) {
			GNode *node = i->second;
			if (node != start_node && node != end_node) {
				sorted.push_back(node);
			}
		}
		sort(sorted.begin(),sorted.end(),SortNodes());
		_sortedStages.clear();
		ForEach(sorted,i) {
			GNode *node = *i;
			if ( node->_stage ) {
				_sortedStages.push_back(node->_stage);
			}
		}
	}

  // Debugging dump.
  void UadlIr::dump(std::ostream &o,const gc_string &pfx) const
  {
    o << pfx << _uArchName << " (for architecture " << _archName << "):\n";

    gc_string pfx1 = indent(pfx);

    if (_firstLevelInstMem) {
      o << pfx1 << "First-level instruction memory:  " << _firstLevelInstMem->name() << "\n\n";
    }
    if (_firstLevelDataMem) {
      o << pfx1 << "First-level data memory:  " << _firstLevelDataMem->name() << "\n\n";
    }

    _top->dump(o,pfx1);

		if (!_sortedStages.empty()) {
      o << pfx1 << "Pipeline Scheduling Order:   ";
      ForEach(_sortedStages,i) {
        o << (*i)->full_name() << " ";
      }
      o << "\n\n";
    }
    
    if (!_machines.empty()) {
      ForEach(_machines,i) {
        i->second->dump(o,pfx1);
      }
      o << "\n";
    }

    if (!_mem_aliases.empty()) {
      gc_string pfx2 = indent(pfx1);
      o << pfx1 << "Memory Aliases:\n";
      ForEach(_mem_aliases,i) {
        o << pfx2 << i->first << " = " << i->second << "\n";
      }
      o << "\n";
    }

    if (!_instClasses.empty()) {
      ForEach(_instClasses,i) {
        i->second->dump(o,pfx1);
      }
      o << "\n";
    }

    if (_branch_hook) {
      o << pfx << "\nBranch Hook:  "
        << _branch_hook << "\n\n";
    }

    if (!_helpers.empty()) {
      o << pfx << "\nHelper Functions:";
      ForEach(_helpers,i) {
        o << '\n' << pfx1 << i->first << ":\n";
        i->second.print(o,pfx1);
      }
    }
    if (!_type_decls.empty()) {
      o << pfx << "\nUser Type Declarations:\n";
      ForEach(_type_decls,i) {
        o << pfx1 << *i << "\n";
      }
    }
    
  }


  const Thread* UadlIr::findThreadByName(const gc_string &name) const
  {
    return findByName(_top->_threads,name);
  }

  const Machine* UadlIr::findMachineByName(const gc_string &name) const
  {
    return findByName(_machines,name);
  }

  const InstClass* UadlIr::findInstClassByName(const gc_string &name) const
  {
    return findByName(_instClasses,name);
  }

  Ptree *UadlIr::findMemAlias(const gc_string &name) const
  {
    IfFind(x,_mem_aliases,name) {
      return x->second;
    } else {
      return 0;
    }
  }

  Thread::Thread(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name) : 
    NamedBase(env,loc,name), 
    _parent(0),
    _core(0), 
    _system(0) 
  {}

  const SysItem &Thread::item() const
  { 
    return (_system) ? static_cast<const SysItem&>(*_system) : static_cast<const SysItem&>(*_core);
  }

  Thread *Thread::clone(const gc_string &name) const
  {
    Thread *nt = new Thread(_env,_loc,name);
    nt->_type = _name;
    nt->_core = _core;
    nt->_system = _system;
    _res.copy(nt->_res);
    return nt;
  }

  Core * Thread::get_core() const
  {
    if (_core) {
      return _core;
    } else {
      assert(!_threads.empty());
      return _threads.begin()->second->get_core();
    }
  }

  void Thread::import_managers(const Resources *parent_res,MsgLog &ml)
  {
    if (!(_core || _system)) PError(loc(), name() << ":  No core or system defined in this uADL description.");

    if (is_group() && _threads.empty()) {
      PError(loc(),name () << ":  Thread-group has no constituent threads defined.");
    }

    _res.import_managers(_system,_core,parent_res,ml);
  }

  void Thread::finalize(UadlIr &ir,const Resources *parent_res,MsgLog &ml,StrVect path)
  {
    // We don't name the top thread.
    if (!parent_res) _name.clear();

    if (!_name.empty()) {
      path.push_back(_name);
    }

    _res.finalize(ir,*this,_system,_core,parent_res,ml,path);

    ForEach(_threads,i) {
      i->second->finalize(ir,&_res,ml,path);
    }

  }

  void Thread::finalize_managers(UadlIr &ir,adl::MsgLog &ml)
  {
    _res.finalize_managers(ir,*this,_core,ml);

    ForEach(_threads,i) {
      i->second->finalize_managers(ir,ml);
    }
  }

  void Thread::dump(ostream &o,const gc_string &pfx) const
  {
    gc_string pfx1 = pfx;
    if (!_name.empty()) {
      pfx1 = indent(pfx);
      if (is_group()) {
        o << pfx << "Thread Group:  " << name() << "\n";
      } else {
        o << pfx << "Thread:  " << name() << "\n";
      }
    }
    _res.dump(o,pfx1);
    ForEach(_threads,i) {
      i->second->dump(o,pfx1);
    }
  }

  FetchUnit *FetchUnit::clone() const
  {
    return new FetchUnit(*this);
  }

  // FetchUnit
  void FetchUnit::finalize(const Core &core,const Resources &res,MsgLog &ml)
  {
    // Check mandatory parameters.
    if (_fetchMemory.empty()) {
      AddError(ml,loc(),"No memory associated with fetch unit.");
    }

    if (_fetchMemoryPort.empty()) {
      _fetchMemoryPort = MemoryBase::defaultPortName();
    }

    if (!_nEntries) {
      AddError(ml,loc(),"No entries specified for fetch unit.");
    }

    if (!_entrySize) {
      AddError(ml,loc(),"No entry-size specified for fetch unit.");
    }

    if (_fetchSizes.empty()) {
      AddError(ml,loc(),"No fetch sizes specified for fetch unit.");
    }

    // check: entry size divides (each) instruction size
    ForEach(core.instrs(),i) {
      // NOTE: this checking implies that 8 | instruction size
      // In non-prefix architectures fetch_width == width
      if (i->second->fetch_width() % (_entrySize * 8)) {
        AddError(ml,loc(),"entry size (" << _entrySize <<
                 "-byte) does not divide instruction '" << i->first <<
                 "' size (" << i->second->width() << "-bit)");
      }
    }

    set<unsigned> sizes;
    ForEach(_fetchSizes,i) {
      unsigned fetchSize = *i;

      if (fetchSize != 1 && fetchSize != 2 && fetchSize != 4 &&
          fetchSize != 8) {
        AddError(ml,loc(),"Fetch size must be 1, 2, 4 or 8");
      }
      if (sizes.insert(fetchSize).second == false) {
        AddWarning(ml,loc(),"Fetch size " << fetchSize << " is specified multiple times.");
      }

      // check parameter correlation

      // NOTE: this implies that entry size also belongs to {1, 2, 4, 8}
      if (_entrySize && fetchSize % _entrySize)
        AddError(ml,loc(),"Fetch size must be an integer multiple of entry size (" << _entrySize << ")");

      if (_nEntries && _entrySize && fetchSize > _nEntries * _entrySize) {
        AddError(ml,loc(),"Fetch size (" << fetchSize <<
                 ") greater than total instruction buffer size (" <<
                 _nEntries << " * " << _entrySize << ")");
      }
    }

    // sort fetch sizes
    sort(_fetchSizes.begin(), _fetchSizes.end(), greater<int>());

    // check: minimal space required for a new fetch >= the greatest fetch size
    if (_minFetchEntries) {
      const unsigned minFetchSpace = _minFetchEntries * _entrySize;

      if (minFetchSpace < _fetchSizes[0]) {
        AddError(ml, loc(), "minimal space required for a new fetch (" <<
                 _minFetchEntries << " * " << _entrySize << " = " <<
                 minFetchSpace << ") less than the greatest fetch size " <<
                 _fetchSizes[0]);
      }
    }

    // check:  Make sure that any named predictors exist.
    ForEach(_branchPredictors,i) {
      const gc_string &bp = *i;
      if (!res.findBranchPredictorByName(bp)) {
        AddError(ml,loc(),"Unknown branch predictor:  " << bp);
      }
    }
  }


  void FetchUnit::dump(ostream &o,const gc_string &pfx) const
  {
    o << pfx << "Fetch Unit:  " << name() << "\n";
    gc_string pfx1 = indent(pfx);
    const gc_string fetch = _fetchMemory +
      (_fetchMemoryPort == MemoryBase::defaultPortName()
       ? "" : "." + _fetchMemoryPort);

    o << pfx1 << "Fetch Memory:  " << fetch << "\n"
      << pfx1 << "Entries     :  " << _nEntries << "\n"
      << pfx1 << "Entry Size  :  " << _entrySize << "\n";
    if (_interleaved) {
      o << pfx1 << "Interleaved :  true\n";
    }
    if (_maxFetchCount != numeric_limits<unsigned>::max()) {
      o << pfx1 << "Fetch Count :  " << _maxFetchCount << "\n";
    }
    o << pfx1 << "Fetch Sizes :  ";
    ForEach(_fetchSizes,i) {
      o << *i << ' ';
    }
    if (!_branchPredictors.empty()) {
      o << pfx1 << "Branch Predictors: ";
      ForEach(_branchPredictors,i) {
        o << " " << *i;
      }
    }
    o << "\n";
  }

  // PipelineStage

  PipelineStage *PipelineStage::clone() const
  {
    return new PipelineStage(*this);
  }

  gc_string PipelineStage::full_name(const gc_string &sep) const
  {
    bool first = true;
    gc_string fn;
    ForEach(_path,i) {
      pfx_sep(fn,sep,first);
      fn += *i;
    }
    return fn;
  }

  gc_string PipelineStage::path_name(const gc_string &sep) const
  {
    bool first = true;
    gc_string fn;
    auto i = _path.begin();
    auto e = _path.end()-1;
    for( ; i != e; ++i) {
      pfx_sep(fn,sep,first);
      fn += *i;
    }
    return fn;
  }

  ostream &operator<< (ostream &o,SchedulingType s)
  {
    switch (s) {
    case sDependent: 
      o << "dependent";
      break;
    case sLockStep:
      o << "lock-step";
      break;
    case sIndependent:
      o << "independent";
      break;
    }
    return o;
  }

  const PipelineStage *StateBinding::stage(const Thread &thread) const
  {
    auto stage = thread.res().findStageByName(_stagen);
    assert(stage);
    return stage;
  }

  bool PipelineStage::set_scheduler(const char *s)
  {
    if (strcmp(s,"dependent") == 0) {
      _scheduler = sDependent;
    }
    else if (strcmp(s,"lock_step") == 0) {
      _scheduler = sLockStep;
    }
    else if (strcmp(s,"independent") == 0) {
      _scheduler = sIndependent;
    }
    else {
      return false;
    }
    return true;
  }

  void PipelineStage::finalize(const StrVect &path)
  {
    _path.insert(_path.end(),path.begin(),path.end());
    _path.push_back(_name);
    if (!_bandwidth) {
      _bandwidth = _size;
    }
  }

  void PipelineStage::dump(ostream &o,const gc_string &pfx) const
  {
    o << pfx << "Pipeline Stage:  " << name();
    if (_path.size() > 1) {
      o << " (" << full_name() << ")";
    }
    o << "\n";
    gc_string pfx1 = indent(pfx);

    o << pfx1 << "Size          :  " << _size << "\n"
      << pfx1 << "Bandwidth     :  " << _bandwidth << "\n"
      << pfx1 << "Rigid         :  " << _rigid << "\n"
      << pfx1 << "Scheduler     :  " << _scheduler << "\n"
      << pfx1 << "Is issue stage:  " << _issue << "\n"
      << pfx1 << "Shared        :  " << _shared << "\n";
  }

  Semaphore *Semaphore::clone() const
  {
    return new Semaphore(*this);
  }

  void Semaphore::dump(ostream &o,const gc_string &pfx) const
  {
    o << pfx << "Semaphore:  " << name() << "\n";
    gc_string pfx1 = indent(pfx);

    o << pfx1 << "Count:  " << _count << "\n";
  }

  Flag *Flag::clone() const
  {
    return new Flag(*this);
  }

  void Flag::dump(ostream &o,const gc_string &pfx) const
  {
    o << pfx << "Flag:  " << name() << "\n";
  }

  RegisterFile *RegisterFile::clone() const
  {
    return new RegisterFile(*this);
  }

  // RegisterFile
  void RegisterFile::finalize(const adl::Core &core,UadlIr &ir,MsgLog &ml)
  {
    if (_latency > 1) {
      AddError(ml,loc(),"Register latency must be 0 or 1");
    }

    if (const adl::Reg *reg = core.get_reg(name())) {
      // Is a single register
      _id = reg->id();
      _size = 1;
      // Don't check hooks for aliasing information- we just care about simple
      // aliases, since hooks are evaluated during read-op or commit.
      _hasAlias = reg->has_alias(false);
      _width = reg->width();

      if (!_is_reg) {
        _is_reg = true;
        AddWarning(ml, loc(), "'" << name() <<
                   "' is not a register file, but a register");
      }
    }
    else if (const adl::RegFile *regFile = core.get_regfile(name())) {
      // Is a register file
      _id = regFile->id();
      _size = regFile->size();
      _hasAlias = regFile->sparse() || regFile->has_alias();
      _width = regFile->width();

      if (_is_reg) {
        _is_reg = false;
        AddWarning(ml, loc(), "'" << name() <<
                   "' is not a register, but a register file");
      }
    }
    else {
      AddWarning(ml, loc(), "Register " << (_is_reg ? "" : "file ") << "'" <<
                 name() << "' is not defined by ADL.  It will be ignored.");
    }

    // Update maximum register (file) id.
    if (_id > (int)ir._maxRegFileId) {
      ir._maxRegFileId = _id;
    }

    // Update maximum register bit width.
    if (width() > ir._maxRegWidth) {
      ir._maxRegWidth = width();
    }
  }


  void RegisterFile::dump(ostream &o,const gc_string &pfx) const
  {
    o << pfx << "Register" << ((_is_reg) ? "" : " File") << ":  " << name() << "\n";
    gc_string pfx1 = indent(pfx);

    o << pfx1 << "ID        :  " << _id << "\n"
      << pfx1 << "Size      :  " << _size << "\n"
      << pfx1 << "Width     :  " << _width << "\n"
      << pfx1 << "Latency   :  " << _latency << "\n"
      << pfx1 << "Is aliased:  " << _hasAlias << "\n";
  }

  ForwardingPath *ForwardingPath::clone() const
  {
    return new ForwardingPath(*this);
  }

  // ForwardingPath
  void ForwardingPath::finalize(const Core &core,MsgLog &ml)
  {
    if (_size == 0) {
      AddError(ml,loc(),"Forwarding path size must be non-zero.");
    } else if (_size >= 100) {
      AddError(ml,loc(),"Suspiciously large forwarding-path size of " << _size << ".  This is probably an error");
    }

    if (!_width) {
      // If the width isn't explicitly set, then guess the width by looking
      // at the core's program-counter width.
      _width = core.cia()->width();
    }
  }

  void ForwardingPath::dump(ostream &o,const gc_string &pfx) const
  {
    o << pfx << "Forwarding Path:  " << name() << "\n";
    gc_string pfx1 = indent(pfx);

    o << pfx1 << "Broadcast:  " << _broadcast <<"\n"
      << pfx1 << "Size     :  " << _size << "\n"
      << pfx1 << "Width    :  " << _width << "\n";
  }

  void CustomItem::checkArgs(ArgItems::iterator iter,
                             ArgItems::iterator end,
                             const adl::StrSet &defaults,
                             MsgLog &ml)
  {
    for ( ; iter != end; ++iter) {
      ArgItem &ai = *iter;
      if (defaults.count(ai._name)) {
        ai._builtin = true;
        if (_parms.count(ai._name)) {
          AddError(ml,loc(),"Argument " << ai._name << " for custom resource " << _name << " is a built-in but has an argument value specified.");
        }
        continue;
      }
      if (!ai._has_default) {
        if (!_parms.count(ai._name)) {
          AddError(ml,loc(),"Argument " << ai._name << " for custom resource " << _name << " has no default value and no value was specified.");
        }
      }
    }
  }

  void CustomItem::handleArgChecking(UadlIr &ir,MsgLog &ml,const gc_string &type,
                                     const adl::StrSet &default_tmpls,const adl::StrSet &default_constrs,
                                     bool full_path_errors)
  {
    // If we're checking arguments, then we first have to find the file.  Then
    // we parse it to figure out what the arguments are.  Then we make sure
    // that we have values for all arguments which do not have a default
    // value.
    bool found = false;
    struct stat sbuf;
    if (stat(_header.c_str(),&sbuf) != 0) {
      // Not found in current directory, so search through include paths.
      ForEach(ir.incl_paths(),i) {
        gc_string fn = *i + "/" + _header;
        if (stat(fn.c_str(),&sbuf) == 0) {
          _path = *i + "/";
          found = true;
          break;
        }
      }
    } else {
      found = true;
    }
    if (!found) {
      AddError(ml,loc(),"Could not find custom resource header '" << _header << "' in current directory or in search path.");
      return;
    }

    // We found it, so now parse the file and get the arguments.
    if (!parse_custom_resource(_args,_path+_header,type,full_path_errors)) {
      AddError(ml,loc(),"Error parsing custom resource header " << _header << ".");
      return;
    }

    if (_args._constr_args.empty()) {
      AddError(ml,loc(),"No constructor arguments for class.  First argument must be a reference to the model class.");
      return;
    }

    // For each template and constructor argument, if it doesn't have a
    // default value, make sure that a value was specified.
    checkArgs(_args._tmpl_args.begin(),_args._tmpl_args.end(),default_tmpls,ml);

    // We skip the first constructor parm b/c it should be a reference to the model class.
    ArgItems::iterator citer = _args._constr_args.begin();
    ++citer;
    checkArgs(citer,_args._constr_args.end(),default_constrs,ml);
  }

  void CustomItem::dump(ostream &o,const gc_string &pfx) const
  {
    o << pfx << "Header   :  " << _header << "\n"
      << pfx << "PreCycle :  " << hasPreCycle() << "\n"
      << pfx << "PostCycle:  " << hasPostCycle() << "\n"
      << pfx << "PostSim  :  " << hasPostSim() << "\n";
    if (!_path.empty() && UadlIr::full_paths()) {
      o << pfx << "Path       :  " << _path << "\n";
    }
    ForEach(_parms,i) {
      o << pfx << "Parm       :  " << i->first << " = " << i->second << "\n";
    }
    if (!_args.empty()) {
      o << pfx << "Args       :  " << _args << "\n";
    }
  }

  void CustomResource::dump(ostream &o,const gc_string &pfx) const
  {
    o << pfx << "Custom Resource:  " << name() << "\n";
    gc_string pfx1 = indent(pfx);

    o << pfx1 << "Type     :  " << _type << "\n";
    CustomItem::dump(o,pfx1);
  }

  CustomResource *CustomResource::clone() const
  {
    return new CustomResource(*this);
  }

  void CustomResource::finalize(const Core &core,UadlIr &ir,MsgLog &ml,
                                bool check_args,bool full_path_errors)
  {
    if (_header.empty()) {
      _header = _name + ".h";
    }
    if (_type.empty()) {
      _type = _name + "_t";
    }
    if (check_args) {
      static StrSet default_tmpls = list_of("ModelType")("InstrType");
      static StrSet default_constrs;
      handleArgChecking(ir,ml,_type,default_tmpls,default_constrs,full_path_errors);
    }
  }

  BranchPredictor *BranchPredictor::clone() const
  {
    return new BranchPredictor(*this);
  }

  // BranchPredictor
  void BranchPredictor::finalize(Core &core,UadlIr &ir,MsgLog &ml,bool check_args,bool full_path_errors)
  {
    if (_algorithm.empty()) {
      PError(loc(),"No algorithm set for branch predictor.");
    }

    if (_algorithm != "Counter") {
      // We have a custom algorithm, if we don't recognize the type.
      _custom = true;
    }

    if (_enable) {
      _enable._d = core.add_aux_func(_enable._p,"bool",
                                     "Branch predictor enable predicate.");
    }

    // Use a default name for the header file, unless otherwise specified.
    if (_custom && _header.empty()) {
      _header = _algorithm + ".h";
    }

    // If we have additional comparison criteria, then these must point to valid
    // registers (and fields, if present).
    ForEach(_reg_compares,i) {
      RegCompare &r = *i;
    
      const adl::RegBase  *rb;
      const adl::RegField *rf;
      if ((rb = const_cast<const Core&>(core).get_regitem(r._reg))) {
        if (!r._field.empty()) {
          if ((rf = rb->get_field(r._field))) {
            r._rb = rb;
            r._rf = rf;
            rf->set_field_usage(RegFieldRead);
          } else {
            AddError(ml,loc(),"Invalid field for register-comparison:  " << r);
          }
        }
      } else {
        AddError(ml,loc(),"Unknown register for register-comparison:  " << r);
      }
    }

    if (check_args && _custom) {
      static StrSet default_tmpls   = list_of("ModelType")("InstrType")("BPData")("BPredEnable");
      static StrSet default_constrs = list_of("id")("enable");
      handleArgChecking(ir,ml,_algorithm,default_tmpls,default_constrs,full_path_errors);
    }
  }


  void BranchPredictor::dump(ostream &o,const gc_string &pfx) const
  {
    o << pfx << "Branch Predictor:  " << name() << "\n";
    gc_string pfx1 = indent(pfx);
    gc_string pfx2 = indent(pfx1);

    o << pfx1 << "Algorithm    :  " << _algorithm;
    o << "\n";
    if (_custom) {
      CustomItem::dump(o,pfx1);
    } else {
      o << pfx1 << "Size         :  " << _size << "\n"
        << pfx1 << "Counter width:  " << _counterWidth << "\n";
    }

    if (_enable) {
      o << pfx1 << "Enable       :\n";
      _enable().print(o,pfx2);
    }

    if (!_reg_compares.empty()) {
      o << pfx1 << "Additional tags:\n";
      ForEach(_reg_compares,i) {
        o << pfx2 << *i << "\n";
      }
    }
  }

  ostream &operator<<(ostream &o,const BranchPredictor::RegCompare &r)
  {
    o << r._reg;
    if (!r._field.empty()) {
      o << '.' << r._field;
    }
    return o;
  }

  std::ostream &operator<< (std::ostream &o,const MemoryBase *mb)
  {
    if (!mb) {
      o << "<empty memory/cache>";
    } else {
      o << mb->name();
    }
    return o;
  }

  MemoryPort *MemoryPort::clone() const
  {
    return new MemoryPort(*this);
  }

  std::ostream &operator<< (std::ostream &o,const MemoryPort *mp)
  {
    if (!mp) {
      o << "<empty port>";
    } else {
      o << mp->name();
    }
    return o;
  }

  void MemoryPort::finalize(unsigned id,unsigned maxRequest,bool pAllowBusWait)
  {
    _id = id;
    if (_maxRequest == 0) {
      _maxRequest = maxRequest;
    }
    if (!_allowBusWaitSpecified) {
      _allowBusWait = pAllowBusWait;
    }
  }

  void MemoryPort::dump(ostream &o,const gc_string &pfx) const
  {
    o << pfx << "Port:  " << name();
    if (_type != UnifiedCache) {
      o << " (" << get_cache_type_str(_type) << ")";
    }
    o << "\n";
    gc_string pfx1 = indent(pfx);
    if (_commonBandwidth) {
      o << pfx1 << "bandwidth:         " << _addrBandwidth << "\n";  
    } else {
      o << pfx1 << "address bandwidth: " << _addrBandwidth << "\n"
        << pfx1 << "data bandwidth:    " << _dataBandwidth << "\n";
    }
    o << pfx1 << "read latency:      " << _readLatency << "\n"
      << pfx1 << "write latency:     " << _writeLatency << "\n"
      << pfx1 << "max requests:      " << _maxRequest << "\n"
      << pfx1 << "allow bus-wait:    " << _allowBusWait << "\n";
  }

  bool MemoryBase::isCache() const 
  { 
    return isA<Cache>(this);
  }

  // MemoryBase
  void MemoryBase::finalize(MsgLog &ml)
  {
    if (!_dataWidth) PError(loc(), "No data width specified.");

    if (_preemptive && !_allowBusWait) {
      AddWarning(ml, loc(), name() << ": a memory/cache that does not allow " <<
                 "bus-waiting requests cannot be preemptive");
      _preemptive = false;
    }

    // TODO: warn if both ports and bandwidth parameters are present

    if (_ports.empty())
      _ports.insert(make_pair(defaultPortName(), defaultPort()));

    // assign port ids
    unsigned portId = 0;

    ForEach(_ports, i) {
      i->second->finalize(portId++,_maxRequest,_allowBusWait);
    }
  }


  void MemoryBase::dump(ostream &o, const gc_string &pfx) const
  {
    unsigned totalAddrBandwidth = 0;
    unsigned totalDataBandwidth = 0;

    ForEach(_ports, i) {
      totalAddrBandwidth += i->second->addrBandwidth();
      totalDataBandwidth += i->second->dataBandwidth();
    }

    o << pfx << "Total address bandwidth: " << totalAddrBandwidth << "\n"
      << pfx << "Total data bandwidth:    " << totalDataBandwidth << "\n"
      << pfx << "Data width:              " << _dataWidth << "\n"
      << pfx << "Max requests:            " << _maxRequest << "\n";

    ForEach(_ports, i) {
      if (i->second->name() != defaultPortName()) {
        i->second->dump(o, pfx);
      }
    }
  }


  const MemoryPort *MemoryBase::findPortByName(const gc_string &name) const 
  {
    ForEach(_ports, i) if (i->first == name) return i->second;

    return NULL;
  }


  MemoryPort *MemoryBase::defaultPort()
  {
    if (!_defaultPort) {
      _defaultPort = new MemoryPort(defaultPortName(), false, 
                                    _addrBandwidth, _dataBandwidth, 0, 0);
    }

    return _defaultPort;
  }

  const MemoryPort *MemoryBase::defaultPort() const
  {
    return const_cast<MemoryBase*>(this)->defaultPort();
  }

  Memory *Memory::clone() const
  {
    Memory *m = new Memory(*this);
    copyResources(m->_ports,_ports);
    return m;
  }

  // Memory
  void Memory::finalize(const adl::Core &core, adl::MsgLog &ml)
  {
    MemoryBase::finalize(ml);

    if (!core.memories().empty() && !core.get_memory(name())) {
      AddError(ml, loc(), "memory '" << name() << "' is not defined by ADL");

      return;
    }
  }


  void Memory::dump(ostream &o,const gc_string &pfx) const
  {
    o << pfx << "Memory:  " << name() << "\n";
    gc_string pfx1 = indent(pfx);

    MemoryBase::dump(o,pfx1);
    o << pfx1 << "Read latency :  " << _readLatency << "\n"
      << pfx1 << "Write latency:  " << _writeLatency << "\n"
      << pfx1 << "Read-only    :  " << _readOnly << "\n";
  }

  Cache *Cache::clone() const
  {
    Cache *m = new Cache(*this);
    copyResources(m->_ports,_ports);
    return m;
  }

  // Cache
  void Cache::finalize(Core &core, const Resources &res,const UadlIr &ir, MsgLog &ml)
  {
    MemoryBase::finalize(ml);

    const adl::Core &constCore = core;

    if (!_IssCache && !(_IssCache = constCore.get_cache(name()))) {
      AddError(ml, loc(), "Cache '" << name() << "' is not defined by ADL.");

      return;
    }

    if (_memory.empty()) {
      if (_IssCache->next()) {
        _memory = _IssCache->next()->name();
      } else if (!res.memories().empty()) {
        // No memory specified, so just use the first memory that's defined in
        // uADL.
        _memory = res.memories().begin()->first;
      }
    } else {
      if (_IssCache->next()) {
        if (_memory != _IssCache->next()->name()) {
          AddError(ml,loc(),"Memory hierarchy inconsistant between ADL and uADL:  Next cache should be " << _IssCache->next()->name());
        }
      } else {
        if (constCore.get_cache(_memory)) {
          AddError(ml,loc(),"Memory hierarchy inconsistant between ADL and uADL:  Next level should be memory.");
        } else if (!ir.res().findMemoryByName(_memory)) {
          AddError(ml,loc(),"Memory hierarchy inconsistant between ADL and uADL:  Unknown next-memory specified:  " << _memory);
        } 
      }
    }

    if (const MemoryBase *mem = dynamic_cast<const MemoryBase*>(res.findResourceByName(_memory))) {
      // TODO: remove this extra finalize() once we call finalize() bottom-up
      const_cast<MemoryBase*>(mem)->finalize(ml);
      
      if (_memoryPortName.empty())
        _memoryPortName = MemoryBase::defaultPortName();
      
      if (!mem->findPortByName(_memoryPortName))
        AddError(ml, loc(), "Memory port does not exist:  " << _memoryPortName);
    } else {
      AddError(ml,loc(),"No next-level memory/cache defined for cache " << name());
    }

    if (auto ci = _config.find("write_mode")) {
      if (!(ci->_str == "WriteBack" || ci->_str == "WriteThrough")) {
        AddError(ml,loc(),"Unknown write mode '" << ci->_str << "'.");
      }
    } else {
      _config.add("write_mode","WriteThrough");
    }

    if (!_config.find("read_hit_latency")) {
      _config.add("read_hit_latency",1u);
    }
    if (!_config.find("write_hit_latency")) {
      _config.add("write_hit_latency",1u);
    }
    if (!_config.find("linefill_read_latency")) {
      _config.add("linefill_read_latency",1u);
    }
    if (!_config.find("linefill_write_latency")) {
      _config.add("linefill_write_latency",1u);
    }
    if (!_config.find("linefill_lock_holdoff_cycle")) {
      _config.add("linefill_lock_holdoff_cycle",1u);
    }
    if (!_config.find("invalidate_latency")) {
      _config.add("invalidate_latency",1u);
    }
    if (!_config.find("lock_latency")) {
      _config.add("lock_latency",1u);
    }
    if (!_config.find("unlock_latency")) {
      _config.add("unlock_latency",1u);
    }
    if (!_config.find("zero_latency")) {
      _config.add("zero_latency",1u);
    }    
    if (!_config.find("critical_word_first")) {
      _config.add("critical_word_first",true);
    }    

    if (auto lw = _config.find("linefill_access_width")) {
      if (!(lw->_val % 8 == 0)) {
        AddError(ml,loc(),"Linefill access width must be an event numer of bytes.");
      }
      if (!is_power_2(lw->_val)) {
        RError("Linefill access width must be a power of 2.");
      }
    } else {
      _config.add("linefill_access_width",64u);
    }

    if (_storeBufEnable) {
      _storeBufEnable._d = core.add_aux_func(
                                             _storeBufEnable._p, "bool",
                                             "Cache store buffer enable predicate."
                                             );
    }

    if (_linefillWidth) {
      _linefillWidth._d = core.add_aux_func(
                                             _linefillWidth._p, "unsigned",
                                             "Cache linefill width hook."
                                             );
    }

    // If we don't have a custom cache object, then only built-in configuration
    // parameters are allowed.
    if (!custom_model()) {
      ForEach(_config,i) {
        if (!i->second._builtin) {
          AddError(ml,loc(),"Found a non-builtin cache parameter, but this cache is not a custom model.");
        }
      }
    }

  }

  gc_string get_key(const gc_string &key,unsigned pad)
  {
    if (pad > key.size()) {
      gc_string pstr((pad - key.size()),' ');
      return key + pstr;
    } else {
      return key;
    }
  }

  void Cache::dump(ostream &o,const gc_string &pfx) const
  {
    o << pfx << "Cache:  " << name() << "\n";
    const gc_string pfx1 = indent(pfx);
    const gc_string pfx2 = indent(pfx1);
    const gc_string memory = _memory +
      (_memoryPortName == MemoryBase::defaultPortName()
       ? "" : "." + _memoryPortName);

   size_t mp = 16;
    ForEach(_config,i) {
      mp = max(mp,i->first.size());
    }

    MemoryBase::dump(o,pfx1);
    if (custom_model()) {
      o << pfx1 << get_key("Custom model",mp) << ":  " << _custom_class << ", " << _custom_header << "\n";
    }
    o << pfx1 << get_key("Next-level memory",mp) << ":  " << memory << "\n";

    ForEach(_config,i) {
      o << pfx1 << get_key(i->first,mp) << ":  " << i->second << "\n";
    }

    if (_linefillWidth) {
      o << pfx1 << get_key("Linefill access width hook",mp) << ":\n";
      _linefillWidth().print(o, pfx2);
    }

    if (_storeBufEnable) {
      o << pfx1 << get_key("Store buffer enable",mp) << ":\n";
      _storeBufEnable().print(o, pfx2);
    }
  }

  void StateBindings::finalize(UadlIr &ir,adl::MsgLog &ml,const Machine &machine)
  {
    ForEach(_tmp,i) {
      const gc_string &stage = i->first;
      const gc_string &state = i->second;

      if (!machine.hasRealState(state)) {
        AddError(ml,loc(),"Machine " << machine.name() << " has no state " << state << ".");
        continue;
      }

      auto stage_mgr = ir.res().findStageByName(stage);
      if (!stage_mgr) {
        AddError(ml,loc(),"State " << state << " is mapped to an undefined pipeline stage " << stage << ".");
        continue;
      }
      
      _bindings.push_back(new StateBinding(state,stage));
    }
    _tmp.clear();
  }

  void StateBindings::dump(std::ostream &o,const gc_string &pfx) const
  {
    ForEach(_bindings,i) {
      o << pfx << (*i)->state() << " = " << (*i)->stagen() << "\n";
    }    
  }

  const PipelineStage *StateBindings::getStage(const Thread &thread,const gc_string &state) const
  {
    ForEach(_bindings,i) {
      if (state == (**i).state()) {
        return (**i).stage(thread);
      }
    }
    return 0;
  }

  const PipelineStage *StateBindings::getStage(const Thread &thread,const gc_string *state) const
  {
    return (state) ? getStage(thread,*state) : 0;
  }

  const gc_string *StateBindings::getStage(const gc_string &state) const
  {
    ForEach(_bindings,i) {
      if (state == (**i).state()) {
        return &(**i).stagen();
      }
    }
    return 0;
  }

  const gc_string *StateBindings::getStage(const gc_string *state) const
  {
    return (state) ? getStage(*state) : 0;
  }

  gc_string Machine::getExceptionState(const gc_string &s) const
  {
    IfFind(x,_emapping,s) {
      return x->second;
    } else {
      return gc_string();
    }
  }

  // Returns true if a state is an exception state.  Linear search,
  // unfortunately.
  bool Machine::isExceptionState(const gc_string &s) const
  {
    ForEach(_emapping,i) {
      if (i->second == s) {
        return true;
      }
    }
    return false;
  }

  void Machine::finalize(UadlIr &ir,MsgLog &ml)
  {
    if (_stateBindings) {
      _stateBindings->finalize(ir,ml,*this);
    }

    // Make sure that all exception states are valid.
    ForEach(_emapping,i) {
      if (!_states.count(i->first)) {
        AddError(ml,loc(),"Unknown state " << i->first);
      }
      if (!_states.count(i->second)) {
        AddError(ml,loc(),"Unknown exception state " << i->second);
      }
    }
  }
  
  void Machine::dump(ostream &o,const gc_string &pfx) const
  {
    o << pfx << "Machine:  " << name() << "\n";
    gc_string pfx1 = indent(pfx);
    gc_string pfx2 = indent(pfx1);

    o << pfx1 << "Initial state:  " << _initState << "\n";

    if (!_states.empty()) {
      o << pfx1 << "States       : ";
      ForEach(_states,i) {
        o << ' ' << *i;
      }
      o << '\n';
    }

    if (_stateBindings) {
      o << pfx1 << "Default State Bindings:\n";
      _stateBindings->dump(o,pfx2);
    }

    if (!_emapping.empty()) {
      o << pfx1 << "Exception Mapping:\n";
      ForEach(_emapping,i) {
        o << pfx2 << i->first << " -> " << i->second << '\n';
      }
      o << '\n';
    }

  }

  void Machine::setInitState(const gc_string &i) 
  { 
    _initState = i; 
  }

  void Machine::addState(const gc_string &s)
  {
    _states.insert(s);
  }

  ostream &operator<<(ostream &o,const Operand &op)
  {
    o << "(";
    bool first = true;
    if (op.is_imm()) {
      first = false;
      o << "imm: " << op._f;
    } else if (op.is_const()) {
      first = false;
      o << "const: " << op.value();
    } else {
      if (op._s_u) {
        first = false;
        o << "src: " << *(op._s_u);
      } 
      if (op._t_u) {
        list_sep(o,first);
        o << "trg: " << *(op._t_u);
      }
    }
    if (!op.is_const() && !op._mask.iszero()) {
      list_sep(o,first);
      o << "mask: 0x" << hex << op._mask << dec;
    }
    o << ")";
    return o;
  }

  const OpVect* InstItem::getOperand(const gc_string &oname) const
  {
    IfFind(x,_operands,oname) {
      return &(x->second);
    } else {
      return 0;
    }
  }

  const gc_string* InstItem::getFunc(const gc_string &fname) const
  {
    IfFind(x,_funcs,fname) {
      return &(x->second);
    } else {
      return 0;
    }
  }

  void InstItem::dump(ostream &o,const gc_string &pfx) const
  {
    o << pfx << _name;
    if (has_opvs()) {
      o << "(" << dec;
      ForEach(_opvalues,o_iter) {      
        o << o_iter->first << "=" << o_iter->second << ' ';
      }
      o << ")";
    }
    o << '\n';
    if (!_operands.empty()) {
      gc_string pfx1 = indent(pfx);
      ForEach(_operands,i) {
        o << pfx1 << i->first << " = ";
        printSequence(o,i->second,", ",false);
        o << '\n';
      }
    }
    if (!_funcs.empty()) {
      gc_string pfx1 = indent(pfx);
      ForEach(_funcs,i) {
        o << pfx1 << i->first << " -> " << i->second << '\n';
      }
    }
  }

  ostream &operator<<(ostream &o,const InstList &sl)
  {
    ForEach(sl,i) {
      i->dump(o,"");
    }
    return o;
  }

	Ptree *addCapacityCheck(const PipelineStage &to_stage,Ptree *action,unsigned count)
	{
		return new PtreeIfStatement(Ptree::Make("if"),
																PtreeUtil::List(Ptree::Make("("),
                                                new PtreeFuncallExpr( new PtreeDotMemberExpr(
                                                                                             Ptree::Make("%s",to_stage.name().c_str()),
                                                                                             PtreeUtil::List(
                                                                                                             Ptree::Make("."),
                                                                                                             Ptree::Make("has_space")
                                                                                                             )
                                                                                             ),
                                                                      PtreeUtil::List(
                                                                                      Ptree::Make("("),
                                                                                      ((count != 1) ? PtreeUtil::List(Ptree::Make("%d",count)) : 0),
                                                                                      Ptree::Make(")")
                                                                                      )
                                                                      ),
																								Ptree::Make(")"),
																								action));
	}

  void Action::finalize(const UadlIr &ir,const InstClass &ic,const Core &core,MsgLog &ml)
  {
    // We have to have some to-states that aren't equal to ourself, or else this
    // is a dead-end state.
    bool dest = false;
    ForEach(_to_states,s) {
      if (*s != _state) {
        dest = true;
      }
    }
    if (!dest) {
      PError(loc(),"Action has no destination states.");
    }

		// For each to-state, we have to have a capacity check.  If we don't, then
		// we add an outer guard with those checks.  We ignore this for the init
		// state, which has infinite capacity, since it represents our end
		// condition.
		auto this_stage = ic.findStageBindingName(_state);
		ForEach(_to_states,s) {
			if (!ic.isInitState(*s)) {
				auto to_stage = ic.findStageBindingName(*s);
        if (!to_stage) {
          PError(loc(),"Could not find stage binding for state " << *s);
        }   
        gc_string this_stagen = (this_stage) ? *this_stage : gc_string();
        gc_string to_stagen = (to_stage) ? *to_stage : gc_string();
        if (this_stagen != to_stagen && _sc_checks.count(*to_stage) == 0) {
          // No capacity check for a transition to a different stage.
          // Therefore, we have to add one.  If we're presently in a lock-step
          // stage, then we should check that the destination stage has enough
          // capacity for all instructions in the current stage.
          auto to_stagep = ir.res().findStageByName(*to_stage);
          assert(to_stagep);
          auto this_stagep = (this_stage) ? ir.res().findStageByName(*this_stage) : 0;
          _action = addCapacityCheck(*to_stagep,_action,((this_stagep && this_stagep->scheduler() == sLockStep) ? this_stagep->size() : 1));
        }
			}
		}

    // If we call exec, and we have exceptions, then we must have a
    // corresponding exception state.
    if (_has_possible_excpt && (ic.has_instr_exceptions() || (ic.uses_mem() && !core.exceptions().empty()))) {
      if (ic.machine().getExceptionState(_state).empty()) {
        AddError(ml,loc(),"State may generate exceptions, but no exception state mapping exists for this state.");
      }
    }
  }

  void Action::dump(ostream &o,const gc_string &pfx) const
  {
    o << pfx << _state;
    o << ":";

    gc_string pfx1 = indent(pfx);

    if (!_to_states.empty()) {
      o << '\n' << pfx1 << "To-States:  ";
      printSequence(o,_to_states,false);
    }
    if (!_fr_states.empty()) {
      o << '\n' << pfx1 << "From-States:  ";
      printSequence(o,_fr_states,false);
    }
    o << "\n"    
      << pfx1 << "Action:\n" << _action << "\n"
      << '\n';

    if (!_repeats.empty()) {
      o << pfx1 << "Repeats:\n";
      ForEach(_repeats,i) {
        o << pfx1 << *i << '\n';
      }
      o << '\n';
    }    
  }

  void Action::display() const
  {
    dump(cerr,"");
  }

	void Actions::finalize(const UadlIr &ir,const InstClass &ic,const Core &core,MsgLog &ml)
	{
		// Finalize each action.
    ForEach((*this),i) {
      LogPError(ml,i->second->finalize(ir,ic,core,ml));
    }
	}

  void Actions::dump(ostream &o,const gc_string &pfx) const
	{
    if (!empty()) {
			gc_string pfx1 = indent(pfx);

      o << pfx << "State actions:\n";
      ForEach((*this),i) {
        i->second->dump(o,pfx1);
      }
    }
	}

  static IntMap &get_attr_map()
  {
    static IntMap attrs;
    return attrs;
  }

  const IntMap &InstClass::get_ic_attrs()
  {
    return get_attr_map();
  }

  void InstClass::add_func_call(const gc_string &func,int num_args)
  {
    _func_calls[func].insert(num_args);
  }

  bool InstClass::check_func_call(const gc_string &func,int num_args) const
  {
    IfFind(x,_func_calls,func) {
      return x->second.count(num_args);
    } else {
      return 0;
    }
  }

  const gc_set<int> *InstClass::check_func_call(const gc_string &func) const
  {
    IfFind(x,_func_calls,func) {
      return &x->second;
    } else {
      return 0;
    }
  }

  // Add the attribute to the central map, which assigns unique numbers, then
  // store the attribute itself.
  void InstClass::add_attr(Ptree *loc,const gc_string &attr)
  {
    auto & attrs = get_attr_map();
    attrs.insert(make_pair(attr,0));
    if (attrs.size() >= 32) {
      PError(loc,"Exceeded maximum number of 32 instruction attributes.");
    }
    _attrs.insert(attr);
  }

  // Assign bit-flag values to attributes.
  void InstClass::finalize_attrs()
  {
    unsigned v = 1;
    auto & attrs = get_attr_map();
    ForEach(attrs,i) {
      i->second = v;
      v <<= 1;
    }
  }

  // Get an attribute's integer value.  Returns -1 if the attribute is not valud.
  int InstClass::get_attr_value(const gc_string &a)
  {
    auto const & attrs = get_attr_map();
    IfFind(x,attrs,a) {
      return x->second;
    } else {
      return -1;
    }
  }

  gc_string getInstrClassEnumTypeName()
  {
    return "InstClass";
  }

  gc_string getInstrClassAttrTypeName()
  {
    return "InstClassAttr";
  }

  gc_string InstClass::getTypeName() const
  {
    return name() + "Class";
  }

  bool InstClass::check_instr_field(const Core &core, MsgLog *ml, Ptree *loc,
                                    const gc_string &field, bool missing_override,
                                    PrntStrSet *allows) const
  {
    // First, make sure that this is a valid field.
    if (!core.get_instrfield(field)) {
      if (ml) {
        AddError((*ml),loc,"Unknown instruction field:  " << field);
      }
      return false;
    }
    if (!missing_override) {
      // First, test to see if this is explicitly allowed by the passed in list.
      // If so, then we accept it.
      if (allows && allows->count(field)) {
        return true;
      }
      // We only allow fields which exist for all instructions.  Currently, we
      // just iterate through the instruction list.  This could be streamlined
      // by calculating a common set of fields, but I don't really think it'll
      // be a performance issue.
      InstList fails;
      ForEach(_instrs,iter) {
        auto instr = core.get_instr(iter->_name);
        // The fetch exception may be a null.
        if (!instr) continue;
        if (instr->getAll(field) < 0) {
          // Field not found for this instruction.
          fails.push_back(*iter);
        }
      }
      if (!fails.empty()) {
        if (ml) {
          AddError((*ml),loc,"Instruction field " << field << " does not exist for the following instructions in this class: " << fails);
        }
        return false;
      }
    }
    return true;
  }  

  void InstClass::process_instr_membership(UadlIr &ir,MsgLog &ml)
  {
    const Core &core = ir.core();

    // Add attribute defined instructions
    ForEach(_instr_attrs,iter) {
      int  iattr = adl::Data::get_attr(*iter);
      if (iattr == -1) {
        PError(loc(),"Undefined attribute " << *iter << "."); 
      } else {
        bool added = false;
        const adl::InstrHash &instrs = core.instrs();
        auto i_iter = instrs.begin();
        for ( ; i_iter != instrs.end(); i_iter++) {
          auto instr = i_iter->second;
          // Ignore shorthand instructions, since they're assembler-only
          // constructs.
          if (instr->has_attr(iattr) && !instr->shorthand()) {
            _instrs.push_back(instr->name());
            added = true;
          }
        }
        if (!added) {
          AddWarning(ml,loc(),"No instructions defined by attribute " << *iter << ".");
        }
      }
    }

    // Then check to see if we have the default-instr pseudo-instr.
    auto i_iter =  _instrs.begin();
    for (; i_iter != _instrs.end();) {
      InstItem &ii = *i_iter;

      if (ii._name == DefaultInstrStr) {
        // Allow the special default-instruction pseudo-instruction.  This will
        // be removed before further processing is done.
        auto tmp = i_iter++;
        _instrs.erase(tmp);
        _has_default_instr = true;
        continue;
      }
      ++i_iter;

    }

  }

  // InstClass
  void InstClass::finalize(UadlIr &ir,MsgLog &ml)
  {
    const Core &core = ir.core();

    // If the commit-on-exception behavior wasn't set, use the core's default.
    if (_commit_on_exception == tNotSet) {
      _commit_on_exception = bool2tb(ir._commit_on_exception);
    }

    if (_name == "unknown") {
      AddError(ml,loc(),"'unknown' is a reserved instruction class name.");
    }

    InstList invalid_insts;

    // Extract instruction set information.  Any instruction which doesn't exist
    // produces a warning and is removed from the list.  We generate a warning
    // so that a uADL file might potentially work with multiple architectures.
    auto i_iter =  _instrs.begin();

    for (; i_iter != _instrs.end();) {
      InstItem &ii = *i_iter;

      if (ii._name == FetchExceptionStr) {
        // Allow the special fetch-exception pseudo-instruction.
        ++i_iter;
        _instIds.insert(FetchExceptionId);
        continue;
      }
      else if (ii._name == DecodeMissStr) {
        // Allow the special decode-miss pseudo-instruction.
        ++i_iter;
        _instIds.insert(DecodeMissId);
        continue;
      }
      else if (auto instr =  const_cast<const Core&>(core).get_instr(ii._name)) {
        if (instr->is_used()) {
          ii._instr = instr;
          _instIds.insert(instr->id());

          // Then make sure that, if any operand-value constraints are present,
          // that the operands are valid (exist for this instruction).
          bool ok = true;

          ForEach(ii._opvalues, o_iter) {
            int f_index = instr->getField(o_iter->first);

            if (f_index < 0) {
              AddWarning(ml, loc(), "Instruction " << instr->name() << " does not have a field named " << o_iter->first);
              // Instruction has problems, so remove from good list.
              ok = false;
            }
          }

          if (ok) {
            if (ii.has_opvs()) _has_opvs = true;

            ++i_iter;
            continue;
          }
        }
      }

      auto tmp =  i_iter++;

      invalid_insts.splice(invalid_insts.end(), _instrs, tmp);
    }

    if (!invalid_insts.empty()) {
      gc_string msg = (invalid_insts.size() > 1) ? "Instructions " : "Instruction ";
      msg += "defined in class '" + name() + "' but not ADL:  ";
      ForEach(invalid_insts,i) {
        msg += i->_name + ' ';
      }
      AddWarning(ml,loc(),msg); 
    }

    // Get the machine binding.  If one wasn't specified, then see if more than
    // one machine is defined.  If only one machine exists, then use it,
    // otherwise produce an error.
    if (!_temp_machine) {
      if (ir.machines().size() > 1) {
        PError(loc(),"No machine binding specified for this instruction class and more than one machine exists for this core.");
      } else if (!ir.machines().empty()) {
        _machine = ir.machines().begin()->second;
      }
    } else {
      _machine = ir.findMachineByName(get_ident(_temp_machine,"machine-binding name."));
    }

    if (!_machine) {
      PError(loc(),"Unknown OSM machine specified for binding:  " << _temp_machine);
    }
    _temp_machine = 0;

    // If the user didn't specify state mappings, then use the machine's default
    // bindings.  If that doesn't exist, then we have an error.
    if (_stateBindings) {
      _stateBindings->finalize(ir,ml,*_machine);
    } else {
      _stateBindings = const_cast<Machine*>(_machine)->stateBindings();      
    }
    if (!_stateBindings) {
      AddError(ml,loc(),"No machine state -> stage bindings were specified.");
    }

    // Process the operands.
    processOperands(ir,core,ml);

    // Process the interface functions.
    processFuncs(ir,core,ml);

    // This parses the action block into state and edge actions.
    parseActionBlock(ir,ir.core(),ml);

    // Finalize all actions.
		_actions.finalize(ir,*this,ir.core(),ml);

    // Figure out cumulative from-states (used for flushing, exceptions, etc).
    assert(_machine);
    processFromStates(_machine->initState(),StrIntMap(),0);

    // Check that all to-states exist in this class.
    checkToStates(ml);
  }

  // All to-states must exist.  We also make sure that exec() is called
  // somewhere.
  void InstClass::checkToStates(MsgLog &ml)
  {
    bool exec_called = false;
    ForEach(_actions,i) {
      const Action &action = *i->second;

      ForEach(action.to_states(),j) {
        const gc_string &ts = *j;

        if (!getAction(ts)) {
          AddError(ml,action.loc(),"Destination state of " << ts << " not defined in this instruction class.");
        }
      }

      exec_called = exec_called || action.has_exec();
    }

    // It's an error to never call exec(), unless it's just for a pseudo
    // instruction like a fetch exception.
    if (!_actions.empty() && !exec_called && !(onlyFetchException() || onlyDecodeMiss())) {
      AddError(ml,loc(),"Exec() was never called in any state action for this instruction class.");
    }
  }

  // For each state, figure out what states can make it to this state.  Start
  // from the init state and proceed, in a depth-first traversal, to the end
  // (back to the init state).  We accumulate states as we go, storing them into
  // each state.  We avoid loops by tracking depth- we don't add states of lower
  // depth than what we're at.
  void InstClass::processFromStates(const gc_string &state,StrIntMap from_states,int depth)
  {
    auto action = getAction(state);

    // If it doesn't have any action, then skip.
    if (!action) return;

    // Skip if a lower depth- this indicates a circuit.  Otherwise, we might
    // have a duplicate due to an early-out situation, in which case we want to
    // record all states that might reach here.
    bool processed = false;
    IfFind(x,from_states,state) {
      processed = true;
      if (x->second <= depth) return;
    }

    // Otherwise, update current from-states.  This inserts anything new.  We
    // don't overwrite because multiple states may reach a single destination
    // state, e.g. due to early-out situations.
    ForEach(from_states,i) {
      action->_fr_states.insert(i->first);
    }

    // At this point, we can skip if we've already handled this state.
    if (processed) return;

    // Add this state to the list.
    from_states.insert(make_pair(state,depth));

    // Now go to each to-state and continue traversing.
    ForEach( action->_to_states, i) {
      processFromStates( *i, from_states, depth + 1);
    }
  }

	bool InstClass::isInitState(const gc_string &state) const
	{
		if (!_machine) return false;

		return _machine->initState() == state;
	}

  bool InstClass::is_operand(Opencxx::Ptree *o) const
  {
    return _operands.count(o->ToString());
  }

  bool InstClass::is_func(Opencxx::Ptree *f) const
  {
    return _funcs.count(f->ToString());
  }

  Action *InstClass::getAction(const gc_string &name) 
  {
    IfFind(x,_actions,name) {
      return x->second;
    } else {
      return 0;
    }
  }

  const InstItem *InstClass::getInstr(const gc_string &name) const
  {
    ForEach(_instrs,i) {
      if (i->_name == name) {
        return &(*i);
      }
    }
    return 0;
  }

  const PipelineStage *InstClass::findStageBinding(const Thread &thread,const gc_string &state) const
  {
    assert(_stateBindings);
    return _stateBindings->getStage(thread,state);
  }

  const gc_string *InstClass::findStageBindingName(const gc_string &state) const
  {
    assert(_stateBindings);
    return _stateBindings->getStage(state);
  }


  // Stores a function list, indexed by instruction name.  The empty string is
  // used to indicate the default set of operands.
  typedef gc_map<gc_string,StrStrMap> FuncMap;


  // Processes the functions block.  This just maps functions used by the
  // instruction class (the keys) to the actual helper functions called, which
  // exist in the architecture class.
  void InstClass::processFuncs(UadlIr &ir,const Core &core,MsgLog &ml)
  {
    FuncMap all_funcs;

    bool errors = false;

    ForEach(_tmp_funcs,i_iter) {
      // Create a new function-list for this instruction (or default).
      auto ip = all_funcs.insert(make_pair(i_iter->first,StrStrMap()));
      StrStrMap &funcs = ip.first->second;
      // Now process each item.
      ForEach( (i_iter->second), i ) {
        const gc_string &fn  = i->first;        
        Ptree           *trg = i->second;
        if (const char *ft = get_ident(trg,0)) {
          _funcs.insert(fn);

          // Must be a valid helper function.
          if (!ir.get_helper(ft)) {
            AddError(ml,trg,"Unknown helper function used for an interface-function target:  " << ft);
            errors = true;
          }
          funcs[fn] = ft;
        } else {
          AddError(ml,trg,"Bad value for interface-function name (expected a helper-function name):  " << trg);
          errors = true;
        }
      }
    }
    if (errors) return;
    _tmp_funcs.clear();

    // This stores a reference to the default function list (which might be empty
    // if one wasn't specified and we end up inserting one).
    StrStrMap &default_funcs = all_funcs[gc_string()];

    // Now analyze all instructions.  Each instruction cover all listed
    // functions in this class.
    ForEach(_instrs,i) {
      InstItem &item = *i;
      const Instr *instr         = i->_instr;
      
      auto f_iter = ((instr) ? all_funcs.find(instr->name()) : all_funcs.end());
      StrStrMap &funcs = (f_iter == all_funcs.end()) ? default_funcs : f_iter->second;

      ForEach(_funcs,fiter) {
        IfFind(ft,funcs,(*fiter)) {
          // Function name found in a per-instruction override.
          item._funcs[*fiter] = ft->second;
        }
        else IfFind(ft,default_funcs,(*fiter)) {
            // Function name found in the default function block.
            item._funcs[*fiter] = ft->second;
          } else {
          // Not found, so it's an error.
          AddError(ml,loc(),"Interface function " << *fiter << " not defined for instruction " << item._name << ".");
        }
      }
    }
  }

  enum OpSpecial { None, IField, IConst, AllSources, AllTargets };
  enum OpType    { oNone, oTrg, oSrc };

  struct OpItem {
    OpSpecial  _t;
    gc_string  _n;
    gc_string  _f;
    UsedReg    _ur;
    RNumber    _m;
    OpType     _ts;
    bool       _force;

    OpItem(const gc_string &n,const UsedReg &ur,const RNumber &m,OpType ts,bool force = false) :
      _t(None), _n(n), _ur(ur), _m(m), _ts(ts), _force(force) {};

    OpItem(const gc_string &n,const gc_string &f,const RNumber &m) :
      _t(IField), _n(n), _f(f), _m(m), _ts(oNone), _force(false) {};

    OpItem(const gc_string &n,OpSpecial t,const RNumber &m) :
      _t(t), _n(n), _m(m), _ts(oNone), _force(false) {};
  };
  typedef gc_vector<OpItem> OperandList;

  // Stores an operand list, indexed by instruction name.  The empty string is
  // used to indicate the default set of operands.
  typedef gc_map<gc_string,OperandList> OperandMap;

  const gc_string *checkMatch(const UsedReg *src_ur,const UsedReg *trg_ur,
                              const gc_string &name,const UsedReg &o_ur)
  {
    if (src_ur && src_ur->regb() == o_ur.regb()) {
      // We match against the begin and end, in case we have a slice, where the
      // instruction field is either at the beginning or end.  As a special exception, if the 
      // operand doesn't specify any begin or end, then also match, e.g. GPR should match against
      // any of the instruction's GPR operands. 
      if ((o_ur._begin == 0 && o_ur._end == 0) || 
          index_compare(o_ur._begin,src_ur->_begin) || index_compare(o_ur._begin,src_ur->_end)) {
        return &name;
      }
    }
    else if (trg_ur && trg_ur->regb() == o_ur.regb()) {
      // We match against the begin and end, in case we have a slice, where the
      // instruction field is either at the beginning or end.
      if ((o_ur._begin == 0 && o_ur._end == 0) || 
          index_compare(o_ur._begin,trg_ur->_begin) || index_compare(o_ur._begin,trg_ur->_end)) {
        return &name;
      }
    }
    return 0;
  }

  struct CmpOpNoMask {
    CmpOpNoMask(const Operand &op) : _op(op) {};

    bool operator()(const Operand &x)
    {
      return x.eq_no_mask(_op);
    }

    const Operand &_op;
  };

  bool findOperand(InstItem &instr,const UsedReg *src_ur_p,const UsedReg *trg_ur_p,const OperandList &operands)
  {
    bool found = false;
    ForEach(operands,iter) {
      const UsedReg *src_ur = src_ur_p;
      const UsedReg *trg_ur = trg_ur_p;
      const UsedReg &op_ur  = iter->_ur;
      const RNumber &mask   = iter->_m;

      // If an explicit type specifier was given, then 0 out the item not used
      // in order to filter the selection.
      switch (iter->_ts) {
      case oNone: 
        break;
      case oSrc:  
        trg_ur = 0;
        break;
      case oTrg:
        src_ur = 0;
        break;
      }

      // Then search for matches.
      if (iter->_t == AllSources && src_ur) {
        instr._operands[iter->_n].push_back(Operand(src_ur,0,mask));
        found = true;
      }
      else if (iter->_t == AllTargets && trg_ur) {
        instr._operands[iter->_n].push_back(Operand(0,trg_ur,mask));
        found = true;
      }
      else if (auto oname = checkMatch(src_ur,trg_ur,iter->_n,op_ur)) {
        OpVect &ops = instr._operands[*oname];
        Operand tmp_op(src_ur,trg_ur,mask);
        auto x = find_if(ops.begin(),ops.end(),CmpOpNoMask(tmp_op));
        if (x != ops.end()) {
          *x = Operand(src_ur,trg_ur,mask);
        } else {
          ops.push_back(tmp_op);
        }
        found = true;
      }
    }
    return found;
  }

  // For a given instruction used-register item, find a corresponding operand.
  // If not found, produce an error if we don't allow missing operands.
  void processInstrOperand(MsgLog &ml,Ptree *loc,InstItem &instr,StrSet &class_operands,
                           const UsedReg *src_ur,const UsedReg *trg_ur,const OperandList &operands,
                           const OperandList &default_operands,bool allow_missing)
  {
    bool found = false;
    found = findOperand(instr,src_ur,trg_ur,operands);
    if (!found) {
      if (&operands != &default_operands) {
        found = findOperand(instr,src_ur,trg_ur,default_operands);
      }
      if (!found) {
        if (allow_missing) {
          // Not found, so add to "other" class.
          instr._operands[OtherOperandStr].push_back(Operand(src_ur,trg_ur,0));
          class_operands.insert(OtherOperandStr);
        } else {
          auto ur = (src_ur) ? src_ur : trg_ur;
          AddError(ml,loc,"Instruction " << instr._name << " operand " << *ur << " does not correspond to a listed operand.");
        }
      }
    }
  }

  bool findImmOperand(InstItem &instr,const gc_string &field,const OperandList &operands)
  {
    bool found = false;
    ForEach(operands,iter) {
      const RNumber &mask = iter->_m;
      if (iter->_t == IField && field == iter->_f) {
        instr._operands[iter->_n].push_back(Operand(iter->_f,mask));
        found = true;
      }
    }
    return found;
  }

  // For handling operands which are only for immediate values.
  void processInstrImmOperand(MsgLog &ml,Ptree *loc,InstItem &instr,StrSet &class_operands,
                              const gc_string &field,const OperandList &operands,
                              const OperandList &default_operands,bool allow_missing)
  {
    bool found = false;

    found = findImmOperand(instr,field,operands);
    if (!found) {
      if (&operands != &default_operands) {
        // Not found- so search defaults.
        found = findImmOperand(instr,field,default_operands);
      }
      if (!found) {
        // Not found, but this is just an immediate, so we can ignore.
        if (!allow_missing) {
          AddError(ml,loc,"Instruction " << instr._instr->name() << " operand " << field << " does not correspond to a listed operand.");
        }
      }
    }
  }
  
  // For handling operands which just store constant values.
  void processInstrConstOperands(InstItem &instr,const OperandList &operands)
  {
    ForEach(operands,iter) {
      if (iter->_t == IConst) {
        instr._operands[iter->_n].push_back(Operand(iter->_m));
      }
    }
  }

  const UsageInfo &get_usage(const Core &core,gc_string &instrname,const Instr *instr)
  {
    if (instrname == DecodeMissStr) {
      return core.decode_miss_handler();
    }
    else if (instrname == FetchExceptionStr) {
      return empty_usage;
    }
    else {
      assert (instr);
      return instr->usageinfo();
    }
  }

  // Processes the operands.  Each operand's listed targets must be either:
  // 1.  A register
  // 2.  A register-file.
  // 3.  Register-file ( instruction-field | integer )
  // 4.  field(field-name):  Indicates an immediate value only.
  // 5.  The special keywords "sources" or "targets", to indicate all sources or all targets.
  //
  // It may exist as a tuple with a mask value and or source/target directive.  
  // Examples:
  //   (GPR(RA),0x0000ffff)
  //   (GPR(RA),source)
  //   (GPR(RA),target)
  //   (GPR(RA),source,0x0000ffff)
  //
  // Masks are only valid for source operands, so specifying a mask and 'source' is redundant and will
  // be an error if a source does not exist.
  void InstClass::processOperands(UadlIr &ir,const Core &core,MsgLog &ml)
  {
    OperandMap all_operands;

    bool errors = false;
    StrSet immediates;
    bool has_consts = false;
    Ptree *op;

    ForEach(_tmp_ops,i_iter) {
      // Create a new operand-list for this instruction (or default).
      auto ip = all_operands.insert(make_pair(i_iter->first,OperandList()));
      OperandList &operands = ip.first->second;
      // Now process each item.
      ForEach( (i_iter->second), i ) {
        gc_string  on = i->first;
        op = i->second;
        RNumber mask;
        OpType otype = oNone;
        bool force = false;

        _operands.insert(on);

        // Does it include a mask or type specifier?
        Ptree *o = 0, *m = 0, *ts = 0;
        if ( Match(op,"[ ( [ %? , %? ] ) ]",&o,&m)) {
          if (o->IsA(ntCommaExpr)) {
            // We have both a type specifier and a mask.
            op = First(o);
            ts = Third(o);
          } else {
            op = o;
          }

          // If only two items are present, then 'm' might be either a type
          // specifier or a mask.
          if (ts) {
            if (Eq(ts,"source")) {
              otype = oSrc;
            } else if (Eq(ts,"target")) {
              otype = oTrg;
            } else if (Eq(ts,"force_source")) {
              otype = oSrc;
              force = true;
            } else if (Eq(ts,"force_target")) {
              otype = oTrg;
              force = true;
            }
          }
          if (m) {
            if (Eq(m,"source")) {
              otype = oSrc;
            } else if (Eq(m,"target")) {
              otype = oTrg;
            } else if (Eq(m,"force_source")) {
              otype = oSrc;
              force = true;
            } else if (Eq(m,"force_target")) {
              otype = oTrg;
              force = true;
            } else {
              get_uintarb(m,"operand source mask.");
              mask = RNumber(m->ToString());
            }
          }
        }

        // Make sure it's valid.
        Ptree *r = 0;
        o = 0;
        unsigned val = 0;
        const RegBase *rb = 0;
        if ( op->IsLeaf() ) {
          // Single item, so must be a register or register-file or special keyword.
          if ( Eq(op,"sources")) {
            operands.push_back(OpItem(on,AllSources,mask));
          } 
          else if ( Eq(op,"targets")) {
            operands.push_back(OpItem(on,AllTargets,mask));
          }
          else if ( get_uint(val,op,env()) ) {
            // This just represents an operand with a constant value.
            operands.push_back(OpItem(on,IConst,val));
            has_consts = true;
          }
          else if ((rb = core.get_regitem(get_ident(op,"register/register-file name.")))) {
            operands.push_back(OpItem(on,UsedReg(rb),mask,otype,force));
          } 
          else {
            AddError(ml,op,"Unknown register/register-file " << op << " for operand " << on << ".");
            errors = true;
          }
        } else if ( Match(op,"[ %? ( [ %? ] ) ]",&r,&o)) {
          if (Eq(r,"field")) {
            if (auto f = core.get_instrfield(get_ident(o,"instruction-field name."))) {
              operands.push_back(OpItem(on,f->name(),mask));
              immediates.insert(f->name());
            } else {
              AddError(ml,op,"Unknown instruction field " << o << " for operand " << on << ".");
            }
          }
          else if ((rb = core.get_regitem(get_ident(r,"register/register-file name.")))) {
            // The parameter may be either an instruction-field or a specific integer.
            unsigned dummy;
            if (!get_uint(dummy,o,env())) {
              // If not an integer, then must be an instruction-field.
              if (!core.get_instrfield(get_ident(o,"instruction-field name."))) {
                AddError(ml,op,"Unknown instruction-field " << o << " for operand " << on << ".");
              }
            }
            operands.push_back(OpItem(on,UsedReg(rb,o),mask,otype,force));
          } else {
            AddError(ml,op,"Unknown register/register-file " << r << " for operand " << on << ".");
            errors = true;
          }
        } else {
          AddError(ml,op,"Invalid operand target " << op << " for operand " << on << ".");
          errors = true;
        }
      }
    }
    if (errors) return;
    _tmp_ops.clear();

    // This stores a reference to the default operand list (which might be empty
    // if one wasn't specified and we end up inserting one).
    OperandList &default_operands = all_operands[gc_string()];

    // Just for initial processing...
    _all_branches = true;

    // Usage information will come from the slot-allocation data, rather than
    // the front-end's usage information, because we want to take into account
    // things like MMU communication registers.  These might be set in the
    // pre-fetch function and thus not be considered as part of the instruction
    // itself.

    // Now analyze all instructions.  Each instruction must have an operand for
    // all operand fields, unless we allow missing operands, in which case we'll
    // allow a dummy field to be used.  Also, we don't allow duplicates- each
    // operand must be unique to the fields in the instruction.
    ForEach(_instrs,i) {
      InstItem &item = *i;
      const Instr *instr         = i->_instr;
      const InstrSlotAlloc &isa  = ir.get_slots(item._name);
      const UsageInfo &ui        = get_usage(core,item._name,instr);
      
      auto o_iter = ((instr) ? all_operands.find(instr->name()) : all_operands.end());
      OperandList &operands = (o_iter == all_operands.end()) ? default_operands : o_iter->second;

      // Since some operands can be sources and targets, we eliminate
      // duplication by putting them into a map and storing source/target
      // information.  We still store pointers to the source and target info,
      // though, so that we preserve mask information.
      gc_map<const UsedReg*,pair<const UsedReg *,const UsedReg *>,cmpUsedRegPtr> opmap;

      // We start by using the instruction's usage information directly, since
      // this breaks up sources and targets at a finer granularity, e.g. the
      // same register may be a source and a target, but might have different
      // masks.  After that, we loop over the slot-allocation info to catch
      // anything we missed, such as resources used in various hooks,
      // e.g. pre_fetch, etc.

      // First, insert all sources.  The first element of the value pair is the
      // source, so we mark it as such.
      ForEach(ui._src_regs,s_iter) {
        opmap.insert(make_pair(&*s_iter,make_pair(&*s_iter,(const UsedReg *)0)));
      }

      // Then iterate over the targets.  If the insertion occurs, then it's just
      // a target.  Otherwise, it already exists as a source, so also store the
      // target info.
      ForEach(ui._trg_regs,t_iter) {
        auto ip = opmap.insert(make_pair(&*t_iter,make_pair((const UsedReg *)0,&*t_iter)));
        if (!ip.second) {
          ip.first->second.second = &*t_iter;
        }
      }

      // Then iterate over the slot allocation data and insert anything we
      // missed.
      ForEach(isa._items,s_iter) {
        const UsedReg &ur = s_iter->first;
        const SlotInfo  &si = s_iter->second;
        auto o_iter = opmap.find(&ur);
        if (o_iter == opmap.end()) {
          pair<const UsedReg *,const UsedReg *> x(((si._src) ? &ur : 0) , ((si._trg) ? &ur : 0) );
          opmap.insert(make_pair(&ur,x));
        }
      }

      ForEach(opmap,o_iter) {
        auto const &st = o_iter->second;
        processInstrOperand(ml,loc(),item,_operands,st.first,st.second,operands,default_operands,_allow_missing_operands);
      }

      // If any immediate-only operands exist, process those.
      if (!immediates.empty() && instr) {
        ForEach(instr->getFields(),f_iter) {
          if (immediates.count(f_iter->_name)) {
            processInstrImmOperand(ml,loc(),item,_operands,f_iter->_name,operands,default_operands,_allow_missing_operands);
          }
        }
      }

      // If any constant-value operands exist, process those.
      if (has_consts && instr) {
        processInstrConstOperands(item,operands);
      }

      // If any forced operands exist, then add those in. Forced operands are
      // ones which don't actually correspond to anything used by the
      // instruction- they represent false dependencies.
      ForEach(operands,o_iter) {
        const OpItem &oi = *o_iter;
        if (oi._force && !item.getOperand(oi._n)) {
          // Add in the forced item.
          if (oi._ts == oSrc) {
            item._operands[oi._n].push_back(Operand(new UsedReg(oi._ur),0,oi._m));
          } else if (oi._ts == oTrg) {
            item._operands[oi._n].push_back(Operand(0,new UsedReg(oi._ur),oi._m));
          }
        }
      }

      // If we don't allow extra operands, then generate an error if an operand
      // isn't used by all instructions.
      if (instr && !_allow_extra_operands) {
        ForEach(operands,i) {
          if (item._operands.count(i->_n) == 0) {
            AddError(ml,loc(),"Operand " << i->_n << " not used by instruction " << instr->name());
          }
        }
      }

      // Record if we ever use memory or caches.
      if (!ui._src_mems.empty() || !ui._trg_mems.empty() || !ui._src_caches.empty() || !ui._trg_caches.empty()) {
        _uses_mem = true;
      }
      _has_exceptions = (_has_exceptions || ui._exceptions);
      _has_branch = (_has_branch || ui._trg_regs.count(core.nia()));
      _all_branches = (_all_branches && ui._trg_regs.count(core.nia()));
    }
  }

  // Given a name, return a pointer to a new action.  Note that, for now, we
  // specifically allow actions to already exist.  The reason is that we want to
  // allow the user to be able to use a defmod to tweak an action for a specific
  // edge or state.
  Action *InstClass::addAction(Ptree *id)
  {
    gc_string name = id->ToString();

    if (!_machine->hasState(name)) {
			PError(id,id << " not defined as a state of machine " << _machine->name());
    }

    auto ip =  _actions.insert(make_pair(name, new Action(name)));

    return ip.first->second;
  }

  // This parses the action block into state and edge actions.
  void InstClass::parseActionBlock(UadlIr &ir,Core &core,MsgLog &ml)
  {
    if (_action_block.empty()) {
      PError(loc(),"No actions defined for this instruction class.");
    }

    // Use a walker to perform the translation step.
    ActionWalker w(env(),ir,core,this,ml);

    ForEach(_action_block,i) {
      w.Translate(*i);
    }

    VarList escapes = w.get_escapes();
    ForEach(escapes,i) {
      Ptree *e = i->first;
      AddError(ml,e,"Unknown variable/resource found in action code:  " << e);
    }
    
    _action_block.clear();
  }

  void InstClass::dump(ostream &o,const gc_string &pfx) const
  {
    o << pfx << "Instruction Class:  " << name() << "\n";
    gc_string pfx1 = indent(pfx);
    gc_string pfx2 = indent(pfx1);

    o << pfx1 << "Instructions:\n";
    ForEach(_instrs,i) {
      i->dump(o,pfx2);
    }
    o << "\n";
    if (!_attrs.empty()) {
      o << pfx1 << "Attributes: ";
      printSequence(o,_attrs,", ",true);
    }
    if (!_instr_attrs.empty()) {
      o << pfx1 << "Instruction Attributes:  ";
      printSequence(o,_instr_attrs,", ",true);
    }
    o << pfx1 << "Commit On Exception:  " << commit_on_exception() << "\n";
    o << pfx1 << "Machine    :  " << _machine->name() << "\n";
    assert(_stateBindings);
    o << pfx1 << "State Bindings:\n";
    _stateBindings->dump(o,pfx2);

		_actions.dump(o,pfx1);
  }

  UadlParser::UadlParser(UadlIr *ir) : 
    _ir(ir)
  {}

  void UadlParser::setup(Opencxx::Environment *env)
  {
    // The outer environnment is added as a using-environment to the ADL data
    // model's outer-most environment so that we'll properly see symbols in the
    // uADL outer-most scope when checking code.
    _ir->_env = env;
    _ir->_iss_ir.env()->AddBaseclassEnv(env);
  }

  Handler *UadlParser::make_initial_handler(Environment *env)
  {
    return new OuterHandler(env,0,"uADL",false,_ir);
  }

  void UadlParser::add_function(Ptree *exp)
  {
    _ir->_iss_ir.add_decl(exp);
  }

  // Pass top-level decls to the ADL data model.
  void UadlParser::add_declaration(Ptree *exp)
  {
    _ir->_iss_ir.add_decl(exp);
  }
  
  // Create the data model during the second pass.
  void UadlParser::second_pass(MsgLog &msglog)
  {
    OuterHandler &outer = dynamic_cast<OuterHandler&>(*_handler);

    // This handles the key/value pairs and does an initial
    // populating of the data model.
    outer.process_keys(msglog);

    // Note: We proceeed even if we get errors in order to be able to show more
    // issues to the user in one session.

    // The second pass is handled by the data model and does
    // final checks and populating of data structures.
    _ir->finalize(msglog);
  }
}

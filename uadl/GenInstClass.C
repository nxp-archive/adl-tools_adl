//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This generates the instruction class and operand structures used by the uadl
// model.
//

#include <string>
#include <stdexcept>
#include <iostream>

#include "opencxx/parser/Ptree.h"
#include "opencxx/parser/PtreeUtil.h"

#include "helpers/Macros.h"
#include "helpers/CodeGen.h"
#include "helpers/BasicTypes.h"
#include "helpers/pfxstream.h"

#include "iss/MiscTypes.h"

#include "UadlParser.h"
#include "UadlWalker.h"
#include "GenInstClass.h"
#include "GenCode.h"
#include "UadlCommon.h"

using namespace std;
using namespace adl;
using namespace rnumber;
using namespace Opencxx;
using namespace PtreeUtil;

namespace uadl {

  gc_string getActionName(const gc_string &action)
  {
    return "action_" + action;
  }

  gc_string getActionName(const Action &action)
  {
    return getActionName(action.state());
  }

  gc_string getActionFunc(const gc_string &action)
  {
    return getActionName(action) + "_p";
  }

  gc_string getActionFunc(const Action &action)
  {
    return getActionFunc(action.state());
  }

  gc_string getFlushName(const gc_string &action)
  {
    return "flush_" + action;
  }

  gc_string getFlushName(const Action &action)
  {
    return getFlushName(action.state());
  }

  gc_string getFlushFunc(const gc_string &action)
  {
    return getFlushName(action) + "_p";
  }

  gc_string getFlushFunc(const Action &action)
  {
    return getFlushFunc(action.state());
  }

  gc_string getMoveName(const gc_string &from,const gc_string &to)
  {
    return "move_" + from + "__" + to;
  }

  gc_string getMoveFunc(const gc_string &from,const gc_string &to)
  {
    return getMoveName(from,to) + "_p";
  }

  gc_string getStateChangeName(const gc_string &from,const gc_string &to)
  {
    return "change_state_" + from + "__" + to;
  }

  gc_string getNullActionFunc()
  {
    return "no_move_p";
  }

  gc_string getNoMoveActionName(const gc_string &action)
  {
    return "no_move_action_" + action;
  }

  gc_string getNoMoveActionName(const Action &action)
  {
    return getNoMoveActionName(action.state());
  }

  gc_string getNoMoveActionFunc(const gc_string &action,bool has_repeats)
  {
    if (has_repeats) {
      return getNoMoveActionName(action) + "_p";
    } else {
      return getNullActionFunc();
    }
  }

  const RNumber &getMaskValue(const Operand &op,const UsedReg &ur)
  {
    return (!op._mask.iszero()) ? op._mask : ur._mask;
  }

  void writeMaskInit(std::ostream &o,const MaskInfo &mp,const Operand &op)
  {
    const RNumber &mask = getMaskValue(op,(mp._is_src) ? *op._s_u : *op._t_u);
    o << "ic->_masks[" << mp._index << "] = 0x" << hex << mask;
    if (mask.size () > 32) {
      o << "ull";
    }
    o << dec << ";\n";    
  }

  class GenInstClass : public gc {
  public:    
    GenInstClass(const UadlIr &ir, const Thread &thread, ostream &outH, ostream &outF, ostream &outC,
                 const GenConfig &config,const gc_string &iss_type) :
      _ir(ir),
      _thread(thread),
      _res(thread.res()),
      _core(ir.core()),
      _oh(outH),
      _of(outF),
      _oc(&outC),
      _config(config),
      _walker(_ir.env(),config._ld_okay,config._safeMode,ir,iss_type),
      _iss_type(iss_type),
      _has_shared_stages(has_shared_stages(ir))
    {};

  protected:
    const UadlIr    &_ir;
    const Thread    &_thread;
    const Resources &_res;
    const adl::Core &_core;
    ostream         &_oh;
    ostream         &_of;
    ostream         *_oc;

    GenConfig        _config;
    UadlWalker       _walker;
    gc_string        _iss_type;
    bool             _has_shared_stages;

    typedef gc_map<gc_string,PList> Deallocs;
    Deallocs         _deallocs;

    MaskMap           _masks;
    int               _mask_index;

  public:

    bool have_threads() const
    {
      return _ir.top().is_group();
    }

    bool is_shared(const gc_string &name)
    {
      return (_thread.parent()) ? (bool)_thread.parent()->res().findResourceByName(name) : false;
    }

    bool haveExceptions() const
    {
      return !_core.exceptions().empty();
    }

    bool is_split(const PipelineStage *stage)
    {
      return (stage && (stage->scheduler() == sLockStep) && stage->size() > 1);
    }
    
    bool is_split(const InstClass &ic,const Action &action)
    {
      auto stage = ic.findStageBinding(_thread,action.state());
      return is_split(stage);
    }

    gc_string stageName(const InstClass &ic,const Action &action)
    {
      auto stage = ic.findStageBindingName(action.state());
      return (stage) ? *stage : gc_string();
    }

    void genTemplateHeader(unsigned num,const gc_string &pfx)
    {
      *_oc << "template <";
      bool first = true;
      ForRange(num,i) {
        list_sep(*_oc,first);
        *_oc << "typename " << pfx << i;
      }
      *_oc << ">\n";
    }

    // Check to see if any of the instructions in this class can halt.
    bool checkCanHalt(const InstClass &ic)
    {
      ForEach(ic.instrs(),i) {
        const Instr *instr = i->_instr;
        if (instr && instr->usageinfo()._halts) {
          return true;
        }
      }
      return false;
    }

    // If an action's stage is not multi-issue lock-step, then we just have a
    // single action function.  Otherwise, we have action and move functions,
    // where the move function does a move to another stage.
    void genAction(const InstClass &ic,const Action &action)
    {
      const gc_string uac = getuArchClassName(_ir);

      _walker.set_cur_state(action.state());
      _walker.set_class(ic);
      _walker.set_can_halt(checkCanHalt(ic));
      auto ip = _deallocs.insert(make_pair(action.state(),PList()));
      _walker.set_dealloc(&(ip.first->second));

      gc_string estate = ic.machine().getExceptionState(action.state());
      gc_string sname  = stageName(ic,action);

      // Whether we have exceptions and want to handle them via a try/catch
      // block.  In safe mode, the exception is thrown via safe_exec, which
      // occurs before the pipeline.  So, in safe-mode, a check after an exec
      // call jumps us to a label, which handles the code.
      Ptree *action2 = _walker.translateCode(ic.env(),action.action());
      bool h_e = action.has_possible_excpt();
      bool e = (haveExceptions() && !estate.empty() && ic.getAction(estate));
      bool tc_e = (e && h_e && !_config._safeMode);
      bool l_e = (e && h_e && _config._safeMode);
      *_oc << "// State action " << action.state() << "\n"
          << "bool " << getActionName(action) << "(" << uac << " &uarch,Logger *logger,unsigned position) {\n";
      if (tc_e) {
        *_oc << "try {\n";
      }
      *_oc << action2;

      if (tc_e) {
        *_oc << "}\n"
            << "catch (" << _iss_type << "::Exception) {\n"
            << "if (logger) logger->logMessage(\"exception occurred\");\n";
        if (ic.commit_on_exception()) {
          if (ic.uses_mem()) {
            *_oc << "uarch.cancel_all_mem(*this);\n";
          }
          *_oc << getFlushName(action) << "(uarch,logger);\n"
               << "uarch.commit_exceptions(*this,logger);\n"
               << "uarch.clear_exceptions(*this);\n"
            // Note: Supplying an explicit address b/c this isn't a branch.
               << "uarch.taken_flush_" << sname << "(*this,logger,position,true,uarch.get_pc());\n";
        }
        *_oc << "return " << getStateChangeName(action.state(),estate) << "(uarch,logger,position);\n"
            << "}\n";
      }
      *_oc << "\nreturn false;\n";
      if (l_e) {
        *_oc << "HandleException: {\n"
            << "if (logger) logger->logMessage(\"exception occurred\");\n";
        if (ic.commit_on_exception()) {
          if (ic.uses_mem()) {
            *_oc << "uarch.cancel_all_mem(*this);\n";
          }
          *_oc << getFlushName(action) << "(uarch,logger);\n"
               << "uarch.clear_exceptions(*this);\n"
               << "uarch.taken_flush_" << sname << "(*this,logger,position,true,branch_target());\n";
        }
        *_oc << "return " << getStateChangeName(action.state(),estate) << "(uarch,logger,position);\n"
            << "}\n";
      }
      *_oc << "}\n"
          << "static bool " << getActionFunc(action) << "(" << getInstBaseName(_ir) << " *x," << uac << " &uarch,Logger *logger,unsigned position) {\n"
          << "return static_cast<ThisClass*>(x)->" << getActionName(action) << "(uarch,logger,position);\n"
          << "}\n\n";
    }

    // Any statements labeled as "Repeat" go into the null-action function for
    // lock-step pipelines.  This is supposed to only contain very simple
    // statements.
    void genRepeatAction(const InstClass &ic,const Action &action)
    {
      const gc_string uac = getuArchClassName(_ir);

      _walker.set_cur_state(action.state());
      _walker.set_class(ic);

      gc_string sname  = stageName(ic,action);

      *_oc << "// Repeat action " << action.state() << "\n"
           << "bool " << getNoMoveActionName(action) << "(" << uac << " &uarch,Logger *logger,unsigned position) {\n";
      ForEach(action.repeats(),i) {
        Ptree *r2 = _walker.translateCode(ic.env(),*i);
        *_oc << r2;
      }
      *_oc << "\n"
           << "return true;\n"
           << "}\n\n"
           << "static bool " << getNoMoveActionFunc(action.state(),true) << "(" << getInstBaseName(_ir) << " *x," << uac << " &uarch,Logger *logger,unsigned position) {\n"
           << "return static_cast<ThisClass*>(x)->" << getNoMoveActionName(action) << "(uarch,logger,position);\n"
           << "}\n\n";
    }

    void writeDeallocs(const Action &action)
    {
      PList &deallocs = _deallocs[action.state()];
      ForEach(deallocs,i) {
        *_oc << *i << ";\n";
      }
    }

    // For each state, also generate a flush function based upon the deallocations we've recorded.
    void genFlushFunc(const InstClass &ic,const Action &action)
    {
      const gc_string uac = getuArchClassName(_ir);

      *_oc << "// Flush action " << action.state() << "\n"
          << "void " << getFlushName(action) << "(" << uac << " &uarch,Logger *logger) {\n";
      writeDeallocs(action);
      ForEach(action.from_states(),i) {
        auto fr_action = ic.getAction(*i);
        assert(fr_action);
        writeDeallocs(*fr_action);
      }
      if (_config._ic_enabled) {
        // If we're using the instruction cache, then if we flush, we have to
        // reset the states so that they're usable if the instruction is
        // executed again, just like we do in the final transition to the init
        // state.
        *_oc << "_move = 0;\n"
             << "_action = (StateFunc)" << getActionFunc(ic.machine().initState()) << ";\n"
             << "_flush = (FlushFunc)" << getFlushFunc(ic.machine().initState()) << ";\n";
      }
      *_oc << "}\n"
          << "static void " << getFlushFunc(action) << "(" << getInstBaseName(_ir) << " *x," << uac << " &uarch,Logger *logger) {\n"
          << "return static_cast<ThisClass*>(x)->" << getFlushName(action) << "(uarch,logger);\n"
          << "}\n\n";
    }

    // This creates various convenience functions, such as single functions for
    // operating on all operands.
    void genHelpers(const InstClass &ic)
    {
      const gc_string uname = getuArchClassName(_ir);
      gc_string bname = getInstBaseName(_ir);
      bool first;

      if (ic.check_func_call("can_read_ops")) {
        *_oc << "bool can_read_ops(" << uname << " &uarch," << bname << " &ic,Logger *logger)\n"
             << "{\n"
             << "return (";
        if (ic.operands().empty()) {
          *_oc << "true";
        } else {
          first = true;
          ForEach(ic.operands(),i) {
            pfx_sep(*_oc," && ",first);
            *_oc << *i << ".can_read(uarch,ic,logger)";
          }
        }
        *_oc << ");\n"
             << "}\n\n";
      }

      if (ic.check_func_call("can_write_ops")) {      
        *_oc << "bool can_write_ops(" << uname << " &uarch," << bname << " &ic,Logger *logger)\n"
             << "{\n"
             << "return (";
        if (ic.operands().empty()) {
          *_oc << "true";
        } else {
          first = true;
          ForEach(ic.operands(),i) {
            pfx_sep(*_oc," && ",first);
            *_oc << *i << ".can_write(uarch,ic,logger)";
          }
        }
        *_oc << ");\n"
             << "}\n\n";
      }

      if (ic.check_func_call("read_ops")) {
        *_oc << "void read_ops(" << uname << " &uarch," << bname << " &ic,Logger *logger)\n"
             << "{\n";
        ForEach(ic.operands(),i) {
          *_oc << *i << ".read(uarch,ic,logger);\n";
        }
        *_oc << "}\n\n";
      }

      if (ic.check_func_call("allocate_ops")) {
        *_oc << "void allocate_ops(" << uname << " &uarch," << bname << " &ic,Logger *logger)\n"
             << "{\n";
        ForEach(ic.operands(),i) {
          *_oc << *i << ".allocate(uarch,ic,logger);\n";
        }
        *_oc << "}\n\n";
      }

      // Always generated b/c it's used by the flush routines.
      *_oc << "void deallocate_ops(" << uname << " &uarch," << bname << " &ic,Logger *logger)\n"
           << "{\n";
      ForEach(ic.operands(),i) {
        *_oc << *i << ".deallocate(uarch,ic,logger);\n";
      }
      *_oc << "}\n\n";

      if (ic.check_func_call("write_ops")) {
        *_oc << "void write_ops(" << uname << " &uarch," << bname << " &ic,Logger *logger)\n"
             << "{\n";
        ForEach(ic.operands(),i) {
          *_oc << *i << ".write(uarch,ic,logger);\n";
        }
        *_oc << "}\n\n";
      }

      // Execution is atomic if we don't access memory, so we can just always
      // return true in that case.
      if (ic.uses_mem()) {
        if (_config._safeMode) {
          *_oc << "bool exec_done() const { return mem() == 0; }\n\n";          
        } else {
          *_oc << "bool exec_done() const { return _status == adl::MemAccess::opNone; }\n\n";
        }
      } else {
        *_oc << "bool exec_done() const { return true; }\n\n";
      }

      *_oc << "bool has_exceptions() const { return _instr->has_exceptions(); };\n\n";
    }

    void genActions(const InstClass &ic)
    {
      ForEach(ic.actions(),i) {
        const Action &action = *(i->second);
        auto stage = ic.findStageBinding(_thread,action.state());

        genAction(ic,action);

        if (is_split(stage) && action.has_repeats()) {
          genRepeatAction(ic,action);
        }
      }
    }

    void genFlushFuncs(const InstClass &ic)
    {
      ForEach(ic.actions(),i) {
        const Action &action = *(i->second);
        genFlushFunc(ic,action);
      }
    }

    void writeIssueLogic(const InstClass &ic,const PipelineStage *stage)
    {
      if (stage == _res.issueStage()) {
        if (!ic.hasFetchException()) {
          *_oc << "uarch.issue(_instr);\n";
        } else {
          if (!ic.onlyFetchException()) {
            *_oc << "if (_instr) {\n"
                << "uarch.issue(_instr);\n"
                << "} else \n";
          }
          if (ic.commit_on_exception()) {
            *_oc << "{\n"
                 << "uarch.commit_fetch_exception(*this);\n";
            if (stage) {
              // If we're in a real stage, then flush behind us on a fetch exception.
              *_oc << "uarch.taken_flush_" << stage->name() << "(*this,logger,position);\n";
            } else {
              *_oc << "uarch.flush_issue(logger);\n";
            }
            *_oc << "}\n";
          }
        }
      }
    }

    void genRemoveInstr(const PipelineStage &stage)
    {
      switch (stage.scheduler()) {
      case sIndependent:
        *_oc << "uarch." << stage.name() << ".remove_instr_indep(position);\n";
        break;
      default:
        *_oc << "uarch." << stage.name() << ".remove_instr();\n";
      }
    }

    void genAddInstr(const PipelineStage &stage)
    {
      *_oc << "uarch." << stage.name() << ".add_instr(this);\n";
    }

    // This generates code to check that a branch performed correctly.  If we're
    // still speculative when we're about to commit the instruction, then we
    // have a problem and we have to flush.
    void genBranchCheck(const PipelineStage *stage)
    {
      *_oc << "if (uarch.is_current_speculative_instr(*this)) {\n"
           << "if (logger) logger->logMessage(\"forced flush!\");\n"
           << "uarch.taken_flush_" << stage->name() << "(*this,logger,position,false,0);\n"
           << "}\n";
    }

    bool isExceptionState(const InstClass &ic,const gc_string &from)
    {
      return ic.machine().isExceptionState(from);
    }

    // Creates the actual transition function, given a from and to state.
    void genTransitionFunc(const InstClass &ic,const PipelineStage *stage,const gc_string &uname,
                           const gc_string &from,const gc_string &to,bool has_repeats)
    {
      const gc_string &istate = ic.machine().initState();

      auto to_stage = ic.findStageBinding(_thread,to);
      gc_string fname;
      bool has_move;
      bool split = is_split(stage);
      // If we have a lock-step stage and we're transitioning to a new
      // stage, then the transition function is called "move_<from>_<to> and
      // is called separately.  What's called immediately just stores a
      // function pointer for the pending move.  This way, we can evaluate
      // all instructions first, then only perform moves if all instructions
      // can move.
      if (split && to_stage != stage) {
        fname = getMoveName(from,to);
        has_move = true;
      } else {
        fname = getStateChangeName(from,to);
        has_move = false;
      }
      *_oc << "bool " << fname << "(" << uname << " &uarch,Logger *logger,unsigned position) {\n"
          << "if (logger && uarch.logOsm()) { logger->logMessage(\"" << from << " -> " << to << "\"); }\n";
      if (to == istate) {
        if (_config._safeMode && ic.has_branch()) {
          genBranchCheck(stage);
        }
        genRemoveInstr(*stage);
        if (ic.uses_mem()) {
          *_oc << "uarch.free_all_mem(*this);\n";
        }
        if (_config._ic_enabled) {
          // If using the instruction cache, we have to do a reset here, since
          // when we get the instruction from the cache, all code must be
          // handled in a generic, instruction-non-specific manner.
          *_oc << "_move = 0;\n"
               << "_action = (StateFunc)" << getActionFunc(istate) << ";\n"
               << "_flush = (FlushFunc)" << getFlushFunc(istate) << ";\n";
          // Dummy reset value, so we know we're alive.
          if (_has_shared_stages) {
            *_oc << "_stage = (PipelineStageBase*)1;\n";
          }
        }
        *_oc << "uarch.commit" << (ic.hasFetchException() ? "_checked" : "") << "(this);\n"
            << "return true;\n";
      } else {
        *_oc << "_action = (StateFunc)" << getActionFunc(to) << ";\n"
             << "_flush = (FlushFunc)" << getFlushFunc(to) << ";\n";
        // If the transition is to another state on the same stage, then we
        // implement as a direct tail call b/c transitions amongst the same
        // stage are all done during the same cycle.

        if (to_stage == stage) {
          // To state in same stage, so no instruction movement and we do a
          // tail call here.
          *_oc << "return " << getActionName(to) << "(uarch,logger,position);\n";
        } else {
          // If we're leaving a stage marked as issue, then execute issue actions.
          writeIssueLogic(ic,stage);
          // Going to a different stage, so we're done.  But, we have to do
          // instruction movement as well.
          if (stage) {
            genRemoveInstr(*stage);
          }
          if (to_stage) {
            if (_has_shared_stages) {
              // If the design has shared stages then we have to track what our
              // current stage is.
              *_oc << "_stage = &uarch." << to_stage->name() << ";\n";
            }
            if (!to_stage->shared()) {
              // If a shared stage, then we assume the instruction has already
              // been added manually at a prior point.
              genAddInstr(*to_stage);
            }
          }
          *_oc << "return true;\n";
        }
      }
      *_oc << "}\n\n";
      // If this is for a lock-step stage, we need a static caller
      // function so that we can store it on _move and we need movement
      // function called by the action code, which stores the real
      // change-state function into _move.
      if (has_move) {
        // Static function for calling the move function.
        gc_string mname = getMoveName(from,to);
        *_oc << "static bool " << getMoveFunc(from,to) << "(" << getInstBaseName(_ir) << " *x," << uname << " &uarch,Logger *logger,unsigned position) {\n"
            << "return static_cast<ThisClass*>(x)->" << mname << "(uarch,logger,position);\n"
            << "}\n\n";
            
        // For lock-step, the change-state function just stores the move to
        // be done by storing the function pointer to be called.  We then
        // modify the action so that it doesn't do anything- just returns
        // true.  This prevents action code from duplicating, since we've
        // already executed it.
        *_oc << "bool " << getStateChangeName(from,to) << "(" << uname << " &uarch,Logger *logger,unsigned position) {\n"
             << "_move = (StateFunc)" << getMoveFunc(from,to) << ";\n"
             << "_action = (StateFunc)" << getNoMoveActionFunc(from,has_repeats) << ";\n"
             << "return true;\n"
             << "}\n\n";
      }
    }

    // Create transition functions.  These move the current state to a new
    // state.  For each state (action), we have to produce a to-state function.
    void genTransitionFuncs(const InstClass &ic)
    {
      const gc_string uname = getuArchClassName(_ir);

      ForEach (ic.actions(),i) {
        const Action &action = *(i->second);
        auto stage = ic.findStageBinding(_thread,action.state());
        StrSet states;

        // Generate transition functions for all normal action-code transitions.
        ForEach (action.to_states(),j) {
          const gc_string &to = *j;
          // Record what we're creating.
          states.insert(action.state()+"_"+to);
          genTransitionFunc(ic,stage,uname,action.state(),to,action.has_repeats());
        }
        // If necessary, generate for the exception state as well.  Skip if
        // we've already generated this explicitly due to user input.
        gc_string estate = ic.machine().getExceptionState(action.state());
        const Action *eaction = 0;
        if (!estate.empty() && (eaction = ic.getAction(estate)) && states.count(action.state()+"_"+estate) == 0) {
          genTransitionFunc(ic,stage,uname,action.state(),estate,eaction->has_repeats());
        }
      }
    }

    // If we're using virtual dispatch, then we need to define each operand
    // class as a variable, so that it can be referenced by the instruction
    // classes.
    void genOpVar(const gc_string &op,const gc_string &type_pfx = gc_string())
    {
      if (_config._vd_enabled) {
        *_oc << "static " << type_pfx << op << " " << getOpVar(op) << ";\n\n";
      }
    }

    void genDummyOperand()
    {
      gc_string dname = getDummyOpName();

      const gc_string uname = getuArchClassName(_ir);
      gc_string bname = getInstBaseName(_ir);

      unsigned num_fp = _res.forwardingPaths().size();

      gc_string pfx = (_config._vd_enabled) ? "virtual " : "";

      *_oc << "struct " << dname << " {\n";

      if (_config._vd_enabled) {
        *_oc << "virtual ~" << dname << "() {}\n\n";
      }

      *_oc << dname << " &thisInstr() { return *this; };\n\n"

           << pfx << "bool can_read(" << uname << " &," << bname << " &,Logger *) {\n"
           << "return true;\n"
           << "}\n\n";

      // These inquiries are for forwarding path usage (or any other class that
      // acts like a forwarding path).  If we're using virtual dispatch, then
      // these will be created in per-instruction-class operand base classes.
      if (!_config._vd_enabled) {

        // These functions have return types that depend upon operand values, so
        // they can't be created here.  They're created in the per-operand base
        // classes if we're using virtual dispatch.
        *_oc << "unsigned mask(" << bname << " &,unsigned = 0) const { return 0; }\n\n"
             << "unsigned value(" << uname << " &," << bname << " &,Logger *) const { return 0; }\n\n";
        
        for(unsigned i = 1; i <= num_fp; ++i) {
          genTemplateHeader(i,"T");
          *_oc << "bool can_read_fp(UadlArch &uarch,InstrBase &ic,Logger *logger";
          ForRange(i,j) {
            *_oc << ",T" << j << " &t" << j;
          }
          *_oc <<") {\n"
               <<"return true;\n"
               << "}\n\n";
        }

        // These reads are for forwarding path usage (or any other class that
        // acts like a forwarding path).
        for(unsigned i = 1; i <= num_fp; ++i) {
          genTemplateHeader(i,"T");
          *_oc << "void read(UadlArch &uarch,InstrBase &ic,Logger *logger";
          ForRange(i,j) {
            *_oc << ",T" << j << " &t" << j;
          }
          *_oc <<") {\n"
               << "}\n\n";
        }

        // This reads available resources and returns true if all operands have
        // been read.
        for(unsigned i = 1; i <= num_fp; ++i) {
          genTemplateHeader(i,"T");
          *_oc << "bool read_avail(UadlArch &uarch,InstrBase &ic,Logger *logger";
          ForRange(i,j) {
            *_oc << ",T" << j << " &t" << j;
          }
          *_oc <<") {\n"
               << "return true;\n"
               << "}\n\n";
        }
      }
    
      *_oc << pfx << "bool read(" << uname <<" &," << bname << " &,Logger *) {\n"
           << "return false;\n"
           << "}\n\n"

           << pfx << "void record_sources(" << uname << " &," << bname << " &,Logger *) {}\n\n"

           << pfx << "void log_read(UadlArch &," << bname << " &,Logger *,const string &,unsigned) {\n"
           << "}\n\n"
    
           << pfx << "bool can_write(" << uname << " &," << bname << " &,Logger *) {\n"
           << "return true;\n"
           << "}\n\n"
    
           << pfx << "void allocate(" << uname << " &," << bname << " &,Logger *) {\n"
           << "}\n\n"

           << pfx << "void deallocate(" << uname << " &," << bname << " &,Logger *) {\n"
           << "}\n\n"
    
           << pfx << "void write(" << uname << " &," << bname << " &,Logger *) {\n"
           << "}\n"

           << pfx << "unsigned field_value(" << uname << " &," << bname << " &,Logger *) const { return 0; }\n\n"

           << pfx << "bool is_real(" << uname << " &," << bname << " &,Logger *) const { return false; }\n\n"

           << pfx << "std::string name(UadlArch &," << bname << " &,unsigned = 0,unsigned = 0) const { return std::string(); }\n\n"

           << pfx << "RegId id(UadlArch &,const " << bname << " &,unsigned = 0,unsigned id = 0) const { return RegInvalidId; }\n\n"

           << pfx << "unsigned slot(unsigned = 0,unsigned = 0) const { return 0; }\n\n"

           << pfx << "unsigned num_sub_ops() const { return 0; }\n\n"

           << "};\n\n";
    }

    bool validMaskIndex(int index)
    {
      return index >= 0;
    }

    void genMaskConstNC(int index)
    {
      if (validMaskIndex(index)) {
        *_oc << "ic._masks[" << index << "]";
      } else {
        *_oc << "0x0";
      }
    }

    void genMaskConst(int index)
    {
      if (validMaskIndex(index)) {
        *_oc << ", ";
        genMaskConstNC(index);
      }
    }

    void genMaskConstM(int index,bool use_mask,const RegBase &r)
    {
      if (validMaskIndex(index)) {
        *_oc << ", ";
        if (use_mask) {
          *_oc << "(";
          genMaskConstNC(index);
          *_oc << " & ~mask)." << ((r.width() > 32) ? "uint64()" : "uint32()");
        } else {
          genMaskConstNC(index);
        }
      }
    }

    pair<unsigned,unsigned> get_mask_info()
    {
      unsigned max_size = 0;
      unsigned max_index = 0;
      ForEach(_masks,i) {
        max_size = max(max_size,(unsigned)i->second._size);
        max_index = max(max_index,(unsigned)i->second._index);
      }
      // Return index+1 b/c we're returning a count value.
      return make_pair(max_index+1,max_size);
    }

    const MaskMap &get_mask_map()
    {
      return _masks;
    }

    int maskBitSize(const Operand &op)
    {
      return op.ur().regb()->width();
    }

    // Return the mask index if the mask is non-zero, or -1 otherwise.  The mask
    // is hashed against the operand.
    int getMaskIndex(const Operand &op,const UsedReg &ur)
    {
      const RNumber &mask = getMaskValue(op,ur);
      if (!mask.iszero()) {
        auto ip = _masks.insert(make_pair(&op,MaskInfo(0,mask.size(),(op._s_u == &ur))));
        if (ip.second) {
          // If inserted, then assign it a real index.
          ip.first->second._index = _mask_index++;
        }
        // Return the index.
        return ip.first->second._index;
      }
      return -1;
    }

    void genMaskConst(const Operand &op,const UsedReg &ur) 
    {
      int index = getMaskIndex(op,ur);
      genMaskConstNC(index);
    }

    void genOpInquire (const Operand &op,const UsedReg &ur,const SlotAlloc &slots,
                       const char *query,bool use_mask,bool single_item)
    {
      // Use the user-defined mask if there is one, otherwise use the default
      // mask.
      int mask_index = getMaskIndex(op,ur);

      auto slot = slots.getSlot(ur).first;
      // Slot may be 0 if this is a forced source or target, in which case we
      // just ignore it for a register file.  We might have to figure out a way
      // to support that, at some point.

      const RegFile *rf = 0;
      if (const Reg *r = ur.reg()) {
        // Operand is just a simple register.
        *_oc << "if (!uarch." << getRegName(*r) << ".can_alloc(uarch,ic,logger,\"" << query << "\"";
        genMaskConstM(mask_index,use_mask,*r);
        *_oc << ")) return false;\n";
      } else if (slot && (rf = ur.regfile())) {
        // Operand is a register-file item.
        if (ur._end) {
          // Has a range.
          if (!single_item) {
            *_oc << "for (int i = " << slot->_start << "; i != " << slot->_stop << "; ++i) {\n"
                 << "if (ic.op(i).isSrc() && ";
          } else {
            *_oc << "if (";
          }
          *_oc << "!uarch." << getRegName(*rf) << ".can_alloc(uarch,ic,logger,ic.op(i)._index,\"" << query << "\"";
          genMaskConstM(mask_index,use_mask,*rf);
          *_oc << ")) return false;\n";
          if (!single_item) {
            *_oc << "}\n";
          }
        } else {
          // Just a single item.
          *_oc << "if (!uarch." << getRegName(*rf) << ".can_alloc(uarch,ic,logger,ic.op(" << slot->_start << ")._index,\"" << query << "\"";
          genMaskConstM(mask_index,use_mask,*rf);
          *_oc << ")) return false;\n";
        }
      }
    }

    void genLogAction(const UsedReg &ur,const gc_string &index,const char *logaction)
    {
      const gc_string uname = getuArchClassName(_ir);

      // A bit of a hack here to put in a static_cast, but it means that it's
      // easier for clients to call this function, as they don't have to be
      // templated to the uarch type.
      *_oc << "logger->logOpAction(" << logaction << ",static_cast<" << uname << "&>(uarch)." << getRegName(*ur.regb()) << ".name(),";
      if (ur.reg()) {
        *_oc << "-1";
      } else {
        *_oc << "ic.op(" << index << ")._index";
      }
      if (!_config._safeMode) {
        *_oc << ",ic._instr->slot(" << index << ").uint64()";
      }
      *_oc << ");\n";
    }

    void genOpAction(const UsedReg &ur,const SlotAlloc &slots,bool check_dirty,bool is_src,
                     const char *action,const char *logaction,bool single_item)
    {
      auto slot = slots.getSlot(ur).first;

      if (!slot) return;

      // We only need to actually read the operands in non-safe-mode.
      if (ur._end) {
        // Has a range, so generate a loop.
        if (!single_item) {
          *_oc << "for (int i = " << slot->_start << "; i != " << slot->_stop << "; ++i) {\n"
               << "if (ic.op(i)." << ((is_src) ? "isSrc()" : "isTrg()") << ") {\n";
        }
        if (!_config._safeMode) {
          *_oc << "ic._instr->" << action << "(uarch.get_iss(),i);\n";
        }
        *_oc << "if (logger";
        if (ur.conditional() && check_dirty) {
          *_oc << " && ic._instr->is_dirty(i)";
        }
        *_oc << ")";
        genLogAction(ur,"i",logaction);
        if (!single_item) {
          *_oc << "}\n"
               << "}\n";
        }
      } else {
        // Just a single item.
        if (!_config._safeMode) {
          *_oc << "ic._instr->" << action << "(uarch.get_iss()," << slot->_start << ");\n";
        }
        *_oc << "if (logger";
        if (ur.conditional() && check_dirty) {
          *_oc << " && ic._instr->is_dirty(" << slot->_start << ")";
        }
        *_oc << ")";
        gc_ostringstream ss;
        ss << slot->_start;
        genLogAction(ur,ss.str(),logaction);
      }
    }

    void genLogRead(const UsedReg &ur,const SlotAlloc &slots)
    {
      auto slot = slots.getSlot(ur).first;
      if (!slot) return;

      if (ur._end) {
        *_oc << "for (int i = " << slot->_start << "; i != " << slot->_stop << "; ++i) {\n"
            << "if (ic.op(i).isSrc()) {\n";
        genLogAction(ur,"i","msg");
        *_oc << "}\n"
            << "}\n";
      } else {
        gc_string index;
        MkGStr(index,slot->_start);
        genLogAction(ur,index,"msg");
      }
    }

    void genOpAlloc (const Operand &op,const UsedReg &ur,const SlotAlloc &slots,const char *action)
    {
      // Use the user-defined mask if there is one, otherwise use the default
      // mask.
      int mask_index = getMaskIndex(op,ur);

      auto slot = slots.getSlot(ur).first;

      const RegFile *rf = 0;
      if (const Reg *r = ur.reg()) {
        // Operand is just a simple register.
        *_oc << "uarch." << getRegName(*r) << "." << action << "(uarch,ic,logger";
        genMaskConst(mask_index);
        *_oc << ");\n";
      } else if (slot && (rf = ur.regfile())) {
        // Operand is a register-file item.
        if (ur._end) {
          // Has a range.
          *_oc << "for (int i = " << slot->_start << "; i != " << slot->_stop << "; ++i) {\n"
              << "if (ic.op(i).isTrg()) {\n"
              << "uarch." << getRegName(*rf) << "." << action << "(uarch,ic,logger,ic.op(i)._index";
          genMaskConst(mask_index);
          *_oc << ");\n"
              << "}\n"
              << "}\n";
        } else {
          // Just a single item.
          *_oc << "uarch." << getRegName(*rf) << "." << action << "(uarch,ic,logger,ic.op(" << slot->_start << ")._index";
          genMaskConst(mask_index);
          *_oc << ");\n";
        }
      }
    }

    void genFieldValue (const OpVect &ops,const SlotAlloc &slots)
    {
      if (ops.empty()) {
        *_oc << "return 0;\n";
      } else {
        const Operand &op = ops.front();
        if (op.is_const()) {
          *_oc << "return " << dec << op.value() << ";\n";
        } else if (op.is_imm()) {
          auto field = _ir.core().get_instrfield(op._f);
          assert(field);
          *_oc << "return uarch._iss->get_" << field->name() << "(ic.opc()[0]).uint32();\n";
        } else {
          // Return the basic index, without modification.
          auto slot = slots.getSlot(op.ur()).first;
          if (slot) {
            *_oc << "return ic.op(" << slot->_start << ")._index;\n";
          } else {
            *_oc << "return 0;\n";
          }
        }
      }      
    }

    // Returns the expected bits size for the value() method, given a list of
    // operands.  If -1 is returned, then use an unsigned value.
    int valueRetSize(const OpVect &ops)
    {
      // For the value accessor, it doesn't make sense if we have more than one
      // operand, so we'll just choose the first.
      if (ops.empty()) {
        return -1;
      } else {
        int maxsize = -1;
        ForEach(ops,i) {
          const Operand &op = *i;
          if (op.is_const()) {
            // Ignore a const value- we'll just use an unsigned return value.
            ;
          } else if (op.is_imm()) {
            // If immediate, then the behavior is the same as for field_value().
            auto field = _ir.core().get_instrfield(op._f);
            assert(field);
            maxsize = max(maxsize,(int)field->size());
          } else {
            maxsize = max(maxsize,(int)(op.ur().regb()->width()));
          }
        }
        return maxsize;
      }
    }

    // Given a list of instructions in an instruction class and an operand name,
    // get the max size for a return value.  Returns the max size, or -1 to
    // indicate that an unsigned value should be used.
    int valueRetSize(const InstClass &ic,const gc_string &op)
    {
      int maxsize = -1;
      ForEach(ic.instrs(),i) {
        const InstItem &instr = *i;
        
        if (auto ops = instr.getOperand(op)) {
          int rc = valueRetSize(*ops);
          maxsize = max(maxsize,rc);
        }
      }
      return maxsize;
    }

    gc_string valueRetType(int rs)
    {
      gc_ostringstream ss;
      if (rs < 0) {
        ss << "unsigned";
      } else {
        ss << "bits<" << dec << rs << ">";        
      }
      return ss.str();
    }

    gc_string valueRetType(const OpVect &ops,unsigned maxrs)
    {
      return valueRetType((_config._vd_enabled) ? maxrs : valueRetSize(ops));
    }

    gc_string getIndex(const SlotAlloc &slots,const UsedReg &ur,const gc_string &op_off = "op_off")
    {
      gc_ostringstream ss;
      auto slot = slots.getSlot(ur).first;        

      if (slot) {
        if (ur._end) {
          // If we have a range, then use op_off to identify the item we want.
          ss << "ic.op(" << op_off << ")._index";
        } else if (ur.regfile()) {
          ss << "ic.op(" << slot->_start << ")._index";
        } else {
          ss << "0";
        }
      } else {
        ss << "0";
      }
      return ss.str();
    }

    gc_string getTag(const gc_string &uname,const SlotAlloc &slots,const UsedReg &ur,const gc_string &op_off = "op_off")
    {
      gc_ostringstream ss;
      ss << "(static_cast<" << uname << "&>(uarch)." << getRegName(*ur.regb()) << ".last_allocator( static_cast<" << uname << "&>(uarch),"
         << getIndex(slots,ur,op_off) << "))";
      return ss.str();
    }

    void genValue (const OpVect &ops,const SlotAlloc &slots)
    {
      if (ops.empty()) {
        *_oc << "return 0;\n";
      } else {
        const Operand &op = ops.front();
        if (op.is_imm() || op.is_const()) {
          // If immediate, then the behavvior is the same as for field_value().
          genFieldValue(ops,slots);
        } else {
          // Otherwise, return the slot value if a slot exists.
          auto slot = slots.getSlot(op.ur()).first;
          if (slot) {
            *_oc << dec << "return ic._instr->slot(" << slot->_start << ");\n";
          } else {
            *_oc << "return 0;\n";
          }
        }
      }      
    }

    gc_string getId(const UsedReg &ur,const gc_string &uname,const SlotAlloc &slots,bool src,const gc_string &op_off = "op_off")
    {    
      gc_ostringstream ss;
      ss << "static_cast<" << uname << "&>(uarch)." << getRegName(*ur.regb()) << ".id(static_cast<" << uname << "&>(uarch),"
         << getIndex(slots,ur,op_off) << ")";
      return ss.str();
    }

    const char *valConv(const RegBase &r)
    {
      return (r.width() > 32) ? ".uint64()" : ".uint32()";
    }

    // This gets the ID of the register resource, taking into account aliasing.
    gc_string getId(const Operand &op,const gc_string &uname,const SlotAlloc &slots,const gc_string &op_off = "op_off")
    {
      if (op._s_u) {
        return getId(*op._s_u,uname,slots,true,op_off);
      } else {
        return getId(*op._t_u,uname,slots,false,op_off);
      }
    }

    bool is_sparse(const UsedReg &ur)
    {
      if (const RegFile *rf = ur.regfile()) {
        return rf->sparse();
      }
      return false;
    }

    // Generate id information for use by forwarding paths and such.
    void genIdInfo (const gc_string &uname,const OpVect &ops,const SlotAlloc &slots,unsigned maxrs)
    {
      gc_string bname = getInstBaseName(_ir);
      if (ops.front().is_imm() || ops.front().is_const()) {
        *_oc << "std::string name(UadlArch &," << bname << " &,unsigned = 0,unsigned = 0) const{ return std::string(); }\n\n"
             << "RegId id(UadlArch &,const " << bname << "&,unsigned = 0,unsigned = 0) const { return RegInvalidId; }\n\n"
             << "bits<32> mask(InstrBase &,unsigned = 0) const { return 0; };\n\n"
             << "unsigned slot(unsigned = 0,unsigned = 0) const { return 0; };\n\n"
             << "bool is_op_read(InstrBase &ic,unsigned = 0) const { return true; }\n\n";
      } else if (ops.size() != 1) {
        *_oc << "std::string name(UadlArch &uarch," << bname << " &ic,unsigned sub_op = 0,unsigned op_off = 0) const {\n"
             << "switch (sub_op) {\n";
        unsigned count = 0;
        ForEach(ops,i) {
          const Operand &op = *i;
          const UsedReg &ur   = op.ur();
          *_oc << "case " << count++ << ": return static_cast<" << uname << "&>(uarch)." << getRegName(*ur.regb()) 
               << ".name(static_cast<" << uname << "&>(uarch)," << getIndex(slots,ur) << ");\n";
        }
        *_oc << "}\n"
             << "assert(0);\n"
             << "return string();\n"
             << "}\n\n"

             << "RegId id(UadlArch &uarch,const " << bname << "&ic,unsigned sub_op = 0,unsigned op_off = 0) const {\n"
             << "switch (sub_op) {\n";
        count = 0;
        ForEach(ops,i) {
          const Operand &op = *i;
          *_oc << "case " << count++ << ": return " << getId(op,uname,slots) << ";\n";
        }
        *_oc << "}\n"
             << "assert(0);\n"
             << "return 0;\n"
             << "}\n\n"

             << valueRetType(ops,maxrs) << " mask(" << bname << " &ic,unsigned sub_op = 0) const {\n"
             << "switch (sub_op) {\n";
        count = 0;
        ForEach(ops,i) {
          const Operand &op = *i;
          const UsedReg &ur   = op.ur();
          *_oc << "case " << count++ << ": return ";
          genMaskConst(op,ur);
          *_oc << ";\n";
        }
        *_oc << "}\n"
             << "assert(0);\n"
             << "return 0;\n"
             << "};\n\n"

             << "tag_t get_wm_tag(" << bname << " &ic,unsigned sub_op = 0) const {\n"
             << "switch (sub_op) {\n";
        count = 0;
        ForEach(ops,i) {
          const Operand &op = *i;
          const UsedReg &ur   = op.ur();
          auto slot = slots.getSlot(ur).first;
          if (slot) {
            *_oc << "case " << count++ << ": return ic._instr->write_mask(" << slot->_start << ").uint32();\n";
          } else {
            *_oc << "case " << count++ << ": assert(0); break;\n";
          }
        }
        *_oc << "}\n"
             << "assert(0);\n"
             << "return 0;\n"
             << "};\n\n"

             << "void set_wm_tag(tag_t tag," << bname << " &ic,unsigned sub_op = 0) const {\n"
             << "switch (sub_op) {\n";
        count = 0;
        ForEach(ops,i) {
          const Operand &op = *i;
          const UsedReg &ur   = op.ur();
          auto slot = slots.getSlot(ur).first;
          if (slot) {
            *_oc << "case " << count++ << ": ic._instr->write_mask(" << slot->_start << ") = tag;\n"
                 << "break;\n";
          } else {
            *_oc << "case " << count++ << ": assert(0); break;\n";
          }
        }
        *_oc << "}\n"
             << "};\n\n"

             << "unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {\n"
             << "switch (sub_op) {\n";
        count = 0;
        ForEach(ops,i) {
          const Operand &op = *i;
          const UsedReg &ur = op.ur();
          auto slot = slots.getSlot(ur).first;
          if (slot) {
            if (ur._end) {
              *_oc << "case " << count++ << ": return op_off;\n";
            } else {
              *_oc << "case " << count++ << ": return " << slot->_start << ";\n";
            }
          } else {
            *_oc << "case " << count++ << ": assert(0); break;\n";
          }
        }
        *_oc << "}\n"
             << "assert(0);\n"
             << "return 0;\n"
             << "};\n\n"

             << "bool is_op_read(" << bname << "&ic,unsigned sub_op = 0) const {\n"
             << "return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));\n"
             << "}\n";
      } else {
        // When there's only one item for this operand, we can simplify these
        // functions.
        const Operand &op = ops.front();
        const UsedReg &ur   = op.ur();
        auto slot = slots.getSlot(ur).first;
        
        unsigned slot_start = (slot) ? slot->_start : 0;

        *_oc << "std::string name(UadlArch &uarch," << bname << " &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<" << uname << "&>(uarch)." 
             << getRegName(*ur.regb()) << ".name(static_cast<" << uname << "&>(uarch)," << getIndex(slots,ur) << "); }\n\n"

             << "RegId id(UadlArch &uarch,const " << bname << " &ic,unsigned = 0,unsigned op_off = 0) const { return " << getId(op,uname,slots) << "; }\n\n"

             << valueRetType(ops,maxrs) << " mask(" << bname << "&ic,unsigned = 0) const { return ";
        genMaskConst(op,ur);
        *_oc << "; }\n\n"

             << "tag_t get_wm_tag(" << bname << " &ic,unsigned = 0) const { return ic._instr->write_mask(" << slot_start << ").uint32(); }\n\n"

             << "void set_wm_tag(tag_t tag," << bname << " &ic,unsigned = 0) const { ic._instr->write_mask(" << slot_start << ") = tag; }\n\n"

             << "unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return ";
        if (ur._end) {
          *_oc << "op_off;";
        } else {
          *_oc << slot_start << ";";
        }
        *_oc << " }\n\n"

             << "bool is_op_read(" << bname << "&ic,unsigned = 0) const {\n"
             << "return (!ic._instr || ic._instr->is_op_read(slot()));\n"
             << "}\n";
      }
      *_oc << "unsigned num_sub_ops() const{ return " << ops.size() << "; };\n\n";
    }

    gc_string sub_op_loop_begin(const OpVect &ops)
    {
      gc_ostringstream ss;
      if (ops.size() > 1) {
        ss << "for (unsigned sub_op = 0; sub_op != " << ops.size() << "; ++sub_op) {\n";
      }
      return ss.str();
    }

    gc_string sub_op_loop_end(const OpVect &ops)
    {
      return (ops.size() > 1) ? "}\n" : "";
    }

    gc_string sub_op_string(const OpVect &ops,bool add_comma = true) 
    {
      gc_string res;
      if (ops.size() > 1) {
        if (add_comma) {
          res += ",";
        }
        res += "sub_op";
      }
      return res;
    }

    gc_string sub_op_string (int sub_op,bool add_comma = true)
    {
      gc_ostringstream ss;
      if (sub_op >= 0) {
        if (add_comma) {
          ss << ",";
        }
        ss << sub_op;
      }
      return ss.str();
    }

    bool has_sub_op(int sub_op)
    {
      return (sub_op >= 0);
    }

    void genTemplateCanRead(const gc_string &uname,const SlotAlloc &slots,const Operand *op,unsigned num_t)
    {
      if (op) {
        bool add_loop = false;
        unsigned start,stop;
        gc_string sfx;

        const UsedReg &ur = op->ur();
        auto slot = slots.getSlot(ur).first;
        if (slot) {
          add_loop = (ur._end);
          start = slot->_start;
          stop = slot->_stop;
          sfx = "i";
        } else {
          assert(0);
        }

        if (add_loop) {
          *_oc << "for (int i = " << start << "; i != " << stop << "; ++i) {\n"        
               << "if  (ic.op(i).isSrc()) {\n";
        }

        *_oc << "RegId id = " << getId(*op,uname,slots,sfx) << ";\n"
             << "tag_t tag = " << getTag(uname,slots,ur,sfx) << ";\n"
             << "if  (!(";
        ForRange(num_t,j) {
          *_oc << "t" << j << ".can_read(uarch,ic,logger,id,tag) || ";
        }
        if (add_loop) {
          *_oc << "uarch." << getRegName(*ur.regb()) << ".can_alloc(uarch,ic,logger,ic.op(i)._index,\"read\"))) return false;\n";
        } else {
          *_oc << "uarch." << getRegName(*ur.regb()) << ".can_alloc(uarch,ic,logger,";
          if (ur.regfile()) {
            *_oc << getIndex(slots,ur) << ",";
          }
          *_oc << "\"read\"))) return false;\n";
        }

        if (add_loop) {
          *_oc << "}\n"
               << "}\n";
        }
      }
    }

    void genTemplateRead(int sub_op,const gc_string &uname,const SlotAlloc &slots,const Operand *op,unsigned num_t) 
    {
      bool add_loop = false;
      unsigned start,stop;
      gc_string sfx, sfxc;
      if (op) {
        const UsedReg &ur = op->ur();
        auto slot = slots.getSlot(ur).first;
        if (slot) {
          if (ur._end) {
            add_loop = (ur._end);
            sfx = "i";
            sfxc = ",i";
          }
          start = slot->_start;
          stop = slot->_stop;
        }
      }

      if (add_loop) {
        *_oc << "for (int i = " << start << "; i != " << stop << "; ++i) {\n"
             << "if  (ic.op(i).isSrc()) {\n";
      } else {
        *_oc << "{\n";
      }

      *_oc << "RegId id = " << getId(*op,uname,slots,sfx) << ";\n"
           << "tag_t tag = " << getTag(uname,slots,op->ur(),sfx) << ";\n";

      bool first = true;
      ForRange(num_t,j) {
        if (!first) {
          *_oc << "else ";
        }
        *_oc << "if (t" << j << ".can_read(uarch,ic,logger,id,tag)) {\n"
             << "t" << j << ".read(uarch,ic,logger,*this" << sub_op_string(sub_op) << sfxc << ");\n";
        if (add_loop) {
          gc_ostringstream msg;
          msg << "\"read from \"+t" << j << ".name()";
          *_oc << "if (logger) ";           
          genLogAction(op->ur(),"i",msg.str().c_str());
        } else {
          *_oc << "if (logger) log_read(uarch,ic,logger,\"read from \"+t" << j << ".name()" << sub_op_string(sub_op) << ");\n";
        }
        *_oc << "}\n";
        first = false;
      }
      *_oc << "else {\n";
      if (add_loop) {
        genOpAction(op->ur(),slots,false,true,"read_operand","\"read\"",true);
      } else {
        *_oc << "read(uarch,ic,logger" << sub_op_string(sub_op) << ");\n";
      }
      *_oc << "}\n"
           << "}\n";
      if (add_loop) {
        *_oc << "}\n";
      }
    }

    void genTemplateReadAvail(const gc_string &uname,const SlotAlloc &slots,const UsedReg &ur,int sub_op,unsigned num_t) 
    {
      // Note: Passing 0 for logger value so that we don't get unnecessary and
      // confusing messages when we can't read a value.
      *_oc << "tag_t tag = get_wm_tag(ic" << sub_op_string(sub_op) << ");\n"
           << "RegId id = " << getId(ur,uname,slots,true) << ";\n"
           << "if (tag == 0) {\n"
           << "read(uarch,ic,logger" << sub_op_string(sub_op) << ");\n"
           << "}\n";
      ForRange(num_t,j) {
        *_oc << "else if (t" << j << ".can_read_tag(uarch,ic,0,id,tag)) {\n"
             << "t" << j << ".read(uarch,ic,logger,*this" << sub_op_string(sub_op) << ");\n"
             << "if (logger) log_read(uarch,ic,logger,\"read from \"+t" << j << ".name()" << sub_op_string(sub_op) << ");\n"
             << "} \n";
      }
      *_oc << "else {\n"
           << "if (logger) logger->logReadAvailFail(static_cast<" << uname << "&>(uarch)." << getRegName(*ur.regb()) << ".name(),";
      if (ur.reg()) {
        *_oc << "-1";
      } else {
        *_oc << getIndex(slots,ur);
      }
      *_oc << ",tag);\n"
           << "done = false;\n"
           << "}\n";
    }

    void genTemplateReadPartialAvail(const gc_string &uname,const SlotAlloc &slots,const UsedReg &ur,unsigned sub_op,unsigned num_t) 
    {
      // Note: Passing 0 for logger value so that we don't get unnecessary and
      // confusing messages when we can't read a value.
      *_oc << "tag_t tag = get_wm_tag(ic" << sub_op_string(sub_op) << ");\n"
           << "RegId id = " << getId(ur,uname,slots,true) << ";\n"
           << "if (tag == 0) {\n"
           << "read(uarch,ic,logger" << sub_op_string(sub_op) << ");\n"
           << "}\n";
      ForRange(num_t,j) {
        *_oc << "else if ((t" << j << ".can_read_tag_partial(uarch,ic,0,id,tag) & "
             << "mask(ic," << sub_op << ")) == mask(ic," << sub_op << ")) {\n"
             << "t" << j << ".read_partial(uarch,ic,logger,*this" << sub_op_string(sub_op) << ");\n"
             << "if (logger) log_read(uarch,ic,logger,\"read from \"+t" << j << ".name()" << sub_op_string(sub_op) << ");\n"
             << "} \n";
      }
      *_oc << "else {\n"
           << "if (logger) logger->logReadAvailFail(static_cast<" << uname << "&>(uarch)." << getRegName(*ur.regb()) << ".name(),";
      if (ur.reg()) {
        *_oc << "-1";
      } else {
        *_oc << getIndex(slots,ur);
      }
      *_oc << ",tag);\n"
           << "done = false;\n"
           << "}\n";
    }

    bool has_partials(const OpVect &ops)
    {
      ForEach(ops,i) {
        if (i->is_imm() || i->is_const()) {
          continue;
        }
        int mask_index = getMaskIndex(*i,i->ur());
        if (validMaskIndex(mask_index)) {
          return true;
        }
      }
      return false;
    }

    bool is_range(const Operand &op) 
    {
      const UsedReg &ur = op.ur();
      return (ur._begin && ur._end);
    }

    bool has_ranges(const OpVect &ops)
    {
      ForEach(ops,i) {
        if (i->is_imm() || i->is_const()) {
          continue;
        }
        if (is_range(*i)) {
          return true;
        }
      }
      return false;
    }

  bool has_srcs(const OpVect &ops)
    {
      ForEach(ops,i) {
        if (i->is_src()) {
          return true;
        }
      }
      return false;
    }

  void genMaskCanRead(const gc_string &uname,const SlotAlloc &slots,const Operand &op,unsigned num_t,int scount,
                        bool is_guard)
    {
      int mask_index = getMaskIndex(op,op.ur());
      bool ir = is_range(op);
      if (validMaskIndex(mask_index)) {
        const UsedReg &ur = op.ur();
        auto slot = slots.getSlot(ur).first;
        bool add_loop = (ir && slot);
        
        gc_string sfx;
        if (add_loop) {
          *_oc << "for (int i = " << slot->_start << "; i != " << slot->_stop << "; ++i) {\n"        
               << "if  (ic.op(i).isSrc()) ";
          sfx = "i";
        }

        *_oc << "{\n"
             << "bits<" << dec << maskBitSize(op) << "> mask = 0;\n";
        ForRange(num_t,j) {
          *_oc << "{\n"
               << "std::pair<bits<" << dec << maskBitSize(op) << ">,tag_t> pm = t" << j << ".can_read_partial(uarch,ic,logger," 
               << getId(op,uname,slots,sfx) << ");\n"
               << "mask |= uarch." << getRegName(*ur.regb()) << ".rm_later_partial_alloc(uarch," 
               << getIndex(slots,ur,sfx) << ",pm.first" << valConv(*ur.regb()) << ",pm.second);\n"
               << "}\n";
        }
        if (add_loop) {
          genOpInquire(op,op.ur(),slots,"read",true,true);
          *_oc << "}\n"
               << "}\n";
        } else {
          *_oc << "if (!can_read(uarch,ic,logger" << sub_op_string(scount) << ",mask))";
          if (!is_guard) {
            *_oc << "return false;\n"
                 << "}\n";
          }
        }

      } else {
        *_oc << "{\n";
        genTemplateCanRead(uname,slots,&op,num_t);
        *_oc << "}\n";
      }
    }

    void genMaskRead(const gc_string &uname,const SlotAlloc &slots,const Operand &op,unsigned num_t,int scount)
    {
      int mask_index  = getMaskIndex(op,op.ur());
      bool ir = is_range(op);
      if (validMaskIndex(mask_index)) {
        const UsedReg &ur = op.ur();
        auto slot = slots.getSlot(ur).first;
        bool add_loop = (ir && slot);

        gc_string sfx,sfxc;
        if (add_loop) {
          *_oc << "for (int i = " << slot->_start << "; i != " << slot->_stop << "; ++i) {\n"        
               << "if  (ic.op(i).isSrc()) ";
          sfx = "i";
          sfxc = ",i";
        }

        *_oc << "{\n";
        if (add_loop) {
          genOpAction(op.ur(),slots,false,true,"read_operand","\"read\"",true);
        } else {
          *_oc << "read(uarch,ic,logger" << sub_op_string(scount) << ");\n";
        }
        ForRange(num_t,j) {
          *_oc << "if (t" << j << ".can_read_partial(uarch,ic,logger," << getId(op,uname,slots,sfx) << ").first != 0) {\n"
               << "t" << j << ".read_partial(uarch,ic,logger,*this" << sub_op_string(scount) << sfxc << ");\n";
          if (add_loop) {
            gc_ostringstream msg;
            msg << "\"read from \"+t" << j << ".name()";
            *_oc << "if (logger) ";           
            genLogAction(op.ur(),"i",msg.str().c_str());
          } else {
            *_oc << "if (logger) log_read(uarch,ic,logger,\"read from \"+t" << j << ".name()" << sub_op_string(scount) << ");\n";
          }
          *_oc << "}\n";
        }
        *_oc << "}\n";

        if (add_loop) {
          *_oc << "}\n";
        }
      } else {
        genTemplateRead(scount,uname,slots,&op,num_t);
      }
    }

    void genTemplateArgs(const StrList &fps,unsigned i)
    {
      if (_config._vd_enabled) {
        auto fp_iter = fps.begin();
        ForRange(i,j) {
          *_oc << "," << *fp_iter << " &t" << j;
          ++fp_iter;
        }          
      } else {
        ForRange(i,j) {
          *_oc << ",T" << j << " &t" << j;
        }
      }
    }

    // Creates templates forms of can_read and read for use with forwarding paths.
    void genTemplateFuncs(const gc_string &uname,const gc_string &bname,const InstClass &ic,
                          const OpVect &ops,const SlotAlloc &slots)
    {
      unsigned num_fp = _res.forwardingPaths().size();

      if (ops.size() == 0 || ops.front().is_imm() || ops.front().is_const()) return;
      
      bool hs = has_srcs(ops);
      bool hp = has_partials(ops);
      bool hr = has_ranges(ops);

      auto fps = getForwardingPathList();

      // These inquiries are for forwarding path usage (or any other class that
      // acts like a forwarding path).  Note:  can_read is made available for targets and sources, because
      // in some situations, we may need to query if a register is free in order to do a write, e.g. a processor
      // which implements a write as a read-modify-write.
      for(unsigned i = 1; i <= num_fp; ++i) {
        if (!ic.check_func_call("can_read_fp",i)) continue;
        if (!_config._vd_enabled) genTemplateHeader(i,"T");
        *_oc << "bool can_read_fp(" << uname << " &uarch," << bname << " &ic,Logger *logger";
        genTemplateArgs(fps,i);
        *_oc <<") {\n";
        if (hp || hr) {
          // We have partials or ranges, so we unroll the loop and deal with
          // each partial or range as a series of queries to try and achieve a
          // complete mask
          if (ops.size() > 1) {
            int scount = 0;
            ForEach(ops,o_iter) {
              const Operand &op = *o_iter;
              genMaskCanRead(uname,slots,op,i,scount++,false);
            }
          } else {
            genMaskCanRead(uname,slots,ops.front(),i,-1,false);
          }
        } else {
          // Only full reads, so each query is an all-or-nothing thing.
          for (unsigned sub_op = 0; sub_op != ops.size(); ++sub_op) {
            *_oc << "{\n";
            genTemplateCanRead(uname,slots,&ops[sub_op],i);
            *_oc << "}\n";
          }

        }
        *_oc << "return true;\n"
             << "}\n\n";
      }

      // These reads are for forwarding path usage (or any other class that
      // acts like a forwarding path).
      for(unsigned i = 1; i <= num_fp; ++i) {
        if (!ic.check_func_call("read",i)) continue;
        if (!_config._vd_enabled) genTemplateHeader(i,"T");
        *_oc << "void read(" << uname << " &uarch," << bname << " &ic,Logger *logger";
        genTemplateArgs(fps,i);
        *_oc <<") {\n";
        if (hs) {
          if (_config._safeMode) {
            // In safe-mode, we don't need to actually grab relevant data from
            // the forwarding paths.  We already know we can get the data due to
            // the call to can_read_fp, so this is just purely for logging
            // purposes.
            *_oc << "read(uarch,ic,logger);\n";
          } else {
            if (hp || hr) {
              if (ops.size() > 1) {
                int scount = 0;
                ForEach(ops,o_iter) {
                  const Operand &op = *o_iter;
                  genMaskRead(uname,slots,op,i,scount++);
                }
              } else {
                genMaskRead(uname,slots,ops.front(),i,-1);
              }
            } else {
              if (ops.size() > 1) {
                int scount = 0;
                ForEach(ops,o_iter) {
                  const Operand &op = *o_iter;              
                  genTemplateRead(scount++,uname,slots,&op,i);
                }
              } else {
                genTemplateRead(-1,uname,slots,&ops.front(),i);
              }
            }
          }
        }
        *_oc << "}\n\n";
      }
    }

    // Creates the read_avail functions which incrementally read operands,
    // returning true if all operands have been read.
    void genReadAvailFuncs(const gc_string &uname,const SlotAlloc &slots,const gc_string &bname,const InstClass &ic,const OpVect &ops)
    {
      unsigned num_fp = _res.forwardingPaths().size();

      if (ops.size() == 0 || ops.front().is_imm() || ops.front().is_const()) return;

      auto fps = getForwardingPathList();
      
      bool hs = has_srcs(ops);
      bool ho = (ops.size() > 1);
      // These reads are for forwarding path usage (or any other class that
      // acts like a forwarding path).
      for(unsigned i = 1; i <= num_fp; ++i) {
        if (!ic.check_func_call("read_avail",i)) continue;
        if (!_config._vd_enabled) genTemplateHeader(i,"T");
        *_oc << "bool read_avail(" << uname << " &uarch," << bname << " &ic,Logger *logger";
        genTemplateArgs(fps,i);
        *_oc <<") {\n"
             << "bool done = true;\n";
        if (hs) {
          int scount = 0;
          ForEach(ops,o_iter) {
            *_oc << "if (!is_op_read(ic," << scount <<")) {\n";
            const Operand &op = *o_iter;
            int mask_index  = getMaskIndex(op,op.ur());
            if (validMaskIndex(mask_index)) {
              genTemplateReadPartialAvail(uname,slots,op.ur(),((ho) ? scount : -1),i);              
            } else {
              genTemplateReadAvail(uname,slots,op.ur(),((ho) ? scount : -1),i);
            }
            *_oc << "}\n";
            ++scount;
          }
        }
        *_oc << "return done;\n"
             << "}\n\n";
      }
    }

    // Logic to record source locations if resources in use.
    void genRecordSrcs(const gc_string &uname,const SlotAlloc &slots,const gc_string &bname,const InstClass &ic,const OpVect &ops)
    {
      if (ops.size() == 0 || ops.front().is_imm() || ops.front().is_const() || !ic.check_func_call("record_sources")) return;

      bool hs = has_srcs(ops);
      bool ho = (ops.size() > 1);

      *_oc << "void record_sources(" << uname << " &uarch," << bname << " &ic,Logger *)\n"
           << "{\n";
      if (hs) {
        int scount = 0;
        ForEach(ops,o_iter) {
          const Operand &op = *o_iter;
          const UsedReg &ur = op.ur();
          *_oc << "if (!is_op_read(ic," << scount <<")) {\n";
          *_oc << "if (!can_read(uarch,ic,0";
          if (ho) {
            *_oc << "," << scount;
          } 
          *_oc << ")) {\n"
               << "set_wm_tag(" << getTag(uname,slots,ur) << ",ic," << scount << ");\n"
               << "} else {\n"
               << "set_wm_tag(0,ic," << scount << ");\n"
               << "}\n"
               << "}\n";
          ++scount;
        }
      }
      *_oc << "}\n\n";
    }

    // An operand is just a structure which provides a common interface for
    // accessing register-operands in the micro-architecture.  Basically, we can
    // test to see if we can read or write things, and then actually read or
    // write them.
    void genOperand(const InstClass &ic,const SlotAlloc &slots,const gc_string &oname,
                    const gc_string &op_name,const OpVect &ops,unsigned maxrs)
    {
      const gc_string uname = getuArchClassName(_ir);
      gc_string bname = getInstBaseName(_ir);

      *_oc << "// Operands:\n";
      ForEach(ops,i) {
        *_oc << "//   " << *i << '\n';
      }
        
      bool hp = has_partials(ops);

      *_oc << "struct " << oname;
      if (_config._vd_enabled) {
        *_oc << " : public " << getInstrOpBaseName(ic.name(),op_name);
      }
      *_oc << " {\n"

          << "bool can_read(" << uname << " &uarch," << bname << " &ic,Logger *logger) {\n";
      ForEach(ops,i) {
        const Operand &op = *i;
        // We create a can_read function for sources *and* targets.  It may be
        // the case that we need to inquire if a target can be read, in case the
        // underlying architecture implements it as a read-modify-write
        // operation.
        if (!op.is_imm() && !op.is_const() && (op._s_u || op._t_u)) {
          genOpInquire(op,((op._s_u) ? *op._s_u : *op._t_u),slots,"read",false,false);
        }
      }
      *_oc << "return true;\n"
          << "}\n\n"

          << "bool read(" << uname << " &uarch," << bname << " &ic,Logger *logger) {\n";
      ForEach(ops,i) {
        const Operand &op = *i;
        if (!op.is_imm() && !op.is_const() && op._s_u) {
          genOpAction(*op._s_u,slots,false,true,"read_operand","\"read\"",false);
        }
      }
      *_oc << "return true;\n"
          << "}\n\n";

      *_oc << "void log_read(UadlArch &uarch," << bname << " &ic,Logger *logger,const string &msg) {\n";
      ForEach(ops,i) {
        const Operand &op = *i;
        if (!op.is_imm() && !op.is_const() && op._s_u) {
          genLogRead(*op._s_u,slots);
        }
      }
      *_oc << "}\n\n";

      // We need a mask version of can-read even if we have just a single sub-operand.
      if (hp && ops.size() == 1) {
        *_oc << "bool can_read(" << uname << " &uarch," << bname << " &ic,Logger *logger,const "
             << valueRetType(ops,maxrs) << " &mask) {\n";
      ForEach(ops,i) {
        const Operand &op = *i;
        if (!op.is_imm() && !op.is_const() && (op._s_u || op._t_u)) {
          genOpInquire(op,((op._s_u) ? *op._s_u : *op._t_u),slots,"read",true,false);
        }
      }
      *_oc << "return true;\n"
          << "}\n\n";
      }

      // When we have multiple sub-operands within an operand, we have to
      // produce versions of the read functions which can operate on a specific
      // item, so that forwarding paths can selectively read only the item of
      // interest.
      if (ops.size() > 1) {
        unsigned ocount = 0;

        *_oc << "bool can_read(" << uname << " &uarch," << bname << " &ic,Logger *logger,unsigned sub_op";
        if (hp) {
          *_oc << ",const " << valueRetType(ops,maxrs) << " &mask = " << valueRetType(ops,maxrs) << "()";
        }
        *_oc << ") {\n"
            << "switch (sub_op) {\n";
        ForEach(ops,i) {
          const Operand &op = *i;
          *_oc << "case " << ocount++ << ": ";
          if (!op.is_imm() && !op.is_const() && (op._s_u || op._t_u)) {
            genOpInquire(op,((op._s_u) ? *op._s_u : *op._t_u),slots,"read",true,false);
          }
          *_oc << "  return true;\n";
        }
        *_oc << "}\n"
            << "assert(0);\n"
             << "return 0;\n"
            << "}\n\n"

            << "bool read(" << uname << " &uarch," << bname << " &ic,Logger *logger,unsigned sub_op) {\n"
            << "switch (sub_op) {\n";
        ocount = 0;
        ForEach(ops,i) {
          const Operand &op = *i;
          *_oc << "case " << ocount++ << ": ";
          if (!op.is_imm() && !op.is_const() && op._s_u) {
            genOpAction(*op._s_u,slots,false,true,"read_operand","\"read\"",false);
          }
          *_oc << "break;\n";
        }
        *_oc << "}\n"
            << "return true;\n"
            << "}\n\n"

            << "void log_read(UadlArch &uarch," << bname << " &ic,Logger *logger,const string &msg,unsigned sub_op) {\n"
            << "switch (sub_op) {\n";
        ocount = 0;
        ForEach(ops,i) {
          const Operand &op = *i;
          *_oc << "case " << ocount++ << ": ";
          if (!op.is_imm() && !op.is_const() && op._s_u) {
            genLogRead(*op._s_u,slots);
          }
          *_oc << "break;\n";
        }
        *_oc << "}\n"
            << "}\n\n";
      }

      // This produces the versions of read() and can_read() which work with
      // forwarding paths.  Only generate if calls of these methods exist.
      genTemplateFuncs(uname,bname,ic,ops,slots);

      // This is for when we want to incrementally read what's available,
      // whether from architected state or forwarding paths.
      genReadAvailFuncs(uname,slots,bname,ic,ops);

      // Logic to record source locations if resources in use.
      genRecordSrcs(uname,slots,bname,ic,ops);

        // Old Uadl doesn't worry about WAW hazards.  Is this really OK?
      *_oc << "bool can_write(" << uname << " &uarch," << bname << " &ic,Logger *logger) {\n"
           << "return true;\n"
           << "}\n\n"

          << "void allocate(" << uname << " &uarch," << bname << " &ic,Logger *logger) {\n";
      ForEach(ops,i) {
        const Operand &op = *i;
        if (!op.is_imm() && !op.is_const() && op._t_u) {
          genOpAlloc(op,*op._t_u,slots,"allocate");
        }
      }
      *_oc << "}\n\n"

          << "void deallocate(" << uname << " &uarch," << bname << " &ic,Logger *logger) {\n";
      ForEach(ops,i) {
        const Operand &op = *i;
        if (!op.is_imm() && !op.is_const() && op._t_u) {
          genOpAlloc(op,*op._t_u,slots,"deallocate");
        }
      }
      *_oc << "}\n\n"

          << "void write(" << uname << " &uarch," << bname << " &ic,Logger *logger) {\n"
          << "deallocate(uarch,ic,logger);\n";
      ForEach(ops,i) {
        const Operand &op = *i;
        if (!op.is_imm() && !op.is_const() && op._t_u) {
          genOpAction(*op._t_u,slots,true,false,"commit","\"write\"",false);
        }
      }
      *_oc << "}\n\n";

      *_oc << "unsigned field_value(" << uname << " &uarch," << bname << " &ic,Logger *logger) const {\n";
      genFieldValue(ops,slots);
      *_oc << "}\n\n";

      if (!_config._safeMode || has_value_query(ic)) {
        *_oc << valueRetType(ops,maxrs) << " value(" << uname << " &uarch," << bname << " &ic,Logger *logger) const {\n";
        genValue(ops,slots);
        *_oc << "}\n\n";
      }

      *_oc << "bool is_real(" << uname << " &uarch," << bname << " &ic,Logger *logger) const { return true; }\n\n";

      genIdInfo(uname,ops,slots,maxrs);

      *_oc << "};\n\n";

    }

    // Generate an operand alias.
    void genOperandAlias(const gc_string &oname,const gc_string &trg_name,const OpVect &trg_ops,const OpVect &ops)
    {
      *_oc << "// Operand Alias:  " << trg_name << " -> " << oname << "\n";
      ForEach(ops,i) {
        *_oc << "//   " << *i << '\n';
      }

      *_oc << "typedef " << trg_name << " " << oname << ";\n\n";     
    }

    // To make sure that we have consistent slots, we store a list of the slots
    // for each operand in the OpVect.  Since we don't care about lookup (we
    // just care about consistency), we store them in-order, as four elements:  start and stop
    // for source, start and stop for target.
    typedef pair<const OpVect *,IntVect> OpItem;

    struct CmpOpItem {
      bool operator()(const OpItem &x,const OpItem &y) const {
        return *x.first < *y.first
          || (!(*y.first < *x.first) && x.second < y.second);
      }
    };

    typedef gc_map<OpItem,pair<gc_string,gc_string>,CmpOpItem> OpVectMap;

    // Maps instructions to the operand's name which is shared.
    typedef gc_map<const InstItem *,gc_string> InstItemMap;

    // Note: This must use the same comparison algorithm as OpVectMap, since
    // it's designed to mirror the same sharing arrangement.
    typedef gc_map<OpItem,InstItemMap,CmpOpItem> OpVectInstMap;

    void print_opvectinstmap(ostream &o,const OpVectInstMap &m)
    {
      int count = 0;
      ForEach(m,i) {
        o << count++ << ":  ";
        ForEach(i->second,j) {
          const InstItem &ii = *(j->first);
          if (ii._instr) {
            o << ii._instr->name();
          } else {
            o << "<empty-instr>";
          }
          o << "(" << j->second << ") ";
        }
        o << "\n\n";
      }
    }

    void checkMaskIndex(const InstItemMap &imap)
    {
      ForEach(imap,i) {
        //        cout << ((i->first->_instr) ? i->first->_instr->name() : "<unknown>") << ' ';
        ForEach(i->first->_operands,j) {
          const OpVect    &ops   = j->second;
          ForEach(ops,o_iter) {
            const Operand *op = &*o_iter;
            IfFind(x,_masks,op) {
              // Note: the +1 is so that we get the *next* mask index, e.g. if
              // we already find 0 in this map, then _mask_index must be set to
              // 1, since 0 is already used.
              _mask_index = max(_mask_index,x->second._index+1);
            }
          }
        }
      }
    }

    // Given an instruction item, we search through all instructions sharing
    // operands to find the highest mask index.  This becomes the new starting
    // mask index for the next operand creation.
    void resetMaskIndex(const InstItem *instr,OpVectInstMap &shared_ops)
    {
      _mask_index = 0;
      //      cout << "Reseting mask index for instruction " << ((instr->_instr) ? instr->_instr->name() : "<unknown>") << ":  ";
      ForEach(shared_ops,i) {
        const InstItemMap &imap = i->second;
        if (imap.count(instr)) {
          checkMaskIndex(imap);
        }
      }
      //      cout << ":  " << _mask_index << '\n';
    }

    // For all aliases to this instruction, replicate masks.
    void replicateMasks(const OpItem &oitem,const OpVect &ops,const InstItem *skip,OpVectInstMap &shared_ops)
    {
      IfFind(x,shared_ops,oitem) {
        // If this item is shared, then iterate over all instructions which share it, skipping the
        // one just generated.
        ForEach(x->second,i_iter) {
          const InstItem *ii = i_iter->first;
          if (skip == ii) continue;
          
          // Get the operand that's shared.
          auto a_ops = ii->getOperand(i_iter->second);
          assert(a_ops);
          auto ao_iter = a_ops->begin();

          // Now iterate over it, replicating masks.
          ForEach(ops,i) {
            IfFind(t_iter,_masks,&*i) {
              _masks.insert(make_pair(&*ao_iter,t_iter->second));
            }
            ++ao_iter;
          }
        }
      }
    }

    OpItem makeOpItem(const OpVect &ops,const SlotAlloc &slots)
    {
      IntVect o_slots;

      ForEach(ops,i) {
        const Operand &op = *i;
        if (op._s_u) {
          auto slot = slots.getSlot(*op._s_u).first;
          o_slots.push_back((slot) ? slot->_start : 0);
          o_slots.push_back((slot) ? slot->_stop : 0);
        } else {
          o_slots.push_back(-1);
          o_slots.push_back(-1);
        }
        if (op._t_u) {
          auto slot = slots.getSlot(*op._t_u).first;
          o_slots.push_back((slot) ? slot->_start : 0);
          o_slots.push_back((slot) ? slot->_stop : 0);
        } else {
          o_slots.push_back(-1);
          o_slots.push_back(-1);
        }
      }
      return OpItem(&ops,o_slots);
    }

    // Generate operand 
    void genOperandsForInstr(const InstClass &ic,const InstItem &instr,
                             OpVectMap &handled_ops,OpVectInstMap &shared_ops)
    {
      auto const &slots = _ir.get_slots(instr._name);

      resetMaskIndex(&instr,shared_ops);

      ForEach(instr._operands,i) {
        const gc_string &oname = i->first;
        const OpVect    &ops   = i->second;

        gc_string iname = makeFriendlyName(instr._name);
        gc_string instr_oname = getInstrOpName(instr._name,ic.name(),oname);

        OpItem oitem = makeOpItem(ops,slots);

        unsigned maxrs = valueRetSize(ic,oname);

        // Try to find an existing alias.  If we're using virtual dispatch, then
        // the base operand name must match as well, due to base-class usage.
        auto x = handled_ops.find(oitem);
        if (x != handled_ops.end() && (!_config._vd_enabled || (oname == x->second.second))) {
          genOperandAlias(instr_oname,x->second.first,*x->first.first,ops);
        } else {
          genOperand(ic,slots,instr_oname,oname,ops,maxrs);
          handled_ops.insert(make_pair(oitem,make_pair(instr_oname,oname)));
          replicateMasks(oitem,ops,&instr,shared_ops);
        }

        if (_config._vd_enabled) {
          genOpVar(instr_oname);
        }

      }
    }

    // Constructs a map mapping OpItems ( OpVect, slot-list ) -> list of
    // instructions sharing the operands.
    void genOperandSharingForInstr(const InstClass &ic,const InstItem &instr,OpVectInstMap &shared_ops)
    {
      auto const &slots = _ir.get_slots(instr._name);

      ForEach(instr._operands,i) {
        const OpVect    &ops   = i->second;

        OpItem oitem = makeOpItem(ops,slots);
        auto ip = shared_ops.insert(make_pair(oitem,InstItemMap()));
        ip.first->second.insert(make_pair(&instr,i->first));
      }
    }

    StrList getForwardingPathList()
    {
      StrList fps;

      ForEach(_res.forwardingPaths(),i) {
        fps.push_back(getForwardingPathType(_ir,_config._safeMode,*i->second));
      }

      return fps;
    }

    // If we're using virtual dispatch then we must generate a base-class for
    // each operand of the instruction class, in order to get various return
    // types correct, as well as handle forwarding paths.
    void genOpBaseClasses(const InstClass &ic)
    {
      if (!_config._vd_enabled) return;

      const gc_string uname = getuArchClassName(_ir);
      const gc_string bname = getInstBaseName(_ir);
      const gc_string cname = getInstBaseName(_ir);
      const gc_string bcname = getDummyOpName();

      unsigned num_fp = _res.forwardingPaths().size();
      StrList fps = getForwardingPathList();

      ForEach(ic.operands(),i) {
        const gc_string &oname = *i;

        const gc_string obname = getInstrOpBaseName(ic.name(),oname);

        unsigned rs = valueRetSize(ic,oname);

        *_oc << "struct " << obname << " : public " << bcname << " {\n"

             << "virtual " << valueRetType(rs) << " mask(" << bname << " &,unsigned = 0) const { return 0; }\n\n"

             << "virtual " << valueRetType(rs) << " value(" << uname << " &," << bname << " &,Logger *) const { return 0; }\n\n";

        for(unsigned i = 1; i <= num_fp; ++i) {
          *_oc << "virtual bool can_read_fp(" << uname << " &uarch," << cname << " &ic,Logger *logger";
          auto fp_iter = fps.begin();
          ForRange(i,j) {
            *_oc << "," << *fp_iter << " &t" << j;
            ++fp_iter;
          }
          *_oc <<") {\n"
               <<"return true;\n"
               << "}\n\n";
        }

        *_oc << "using " << bcname << "::read;\n";

        // These reads are for forwarding path usage (or any other class that
        // acts like a forwarding path).
        for(unsigned i = 1; i <= num_fp; ++i) {
          *_oc << "virtual void read(" << uname << " &uarch," << cname << " &ic,Logger *logger";
          auto fp_iter = fps.begin();
          ForRange(i,j) {
            *_oc << "," << *fp_iter << " &t" << j;
            ++fp_iter;
          }
          *_oc <<") {\n"
               << "}\n\n";
        }

        // This reads available resources and returns true if all operands have
        // been read.
        for(unsigned i = 1; i <= num_fp; ++i) {
          *_oc << "bool read_avail(" << uname << " &uarch," << cname << " &ic,Logger *logger";
          auto fp_iter = fps.begin();
          ForRange(i,j) {
            *_oc << "," << *fp_iter << " &t" << j;
            ++fp_iter;
          }
          *_oc <<") {\n"
               << "return true;\n"
               << "}\n\n";
        }

        *_oc << "};\n\n";

        genOpVar(obname);
      }
    }

    // Generates the operand structs for a given instruction class.  These are
    // used as the arguments for the instruction class object itself.
    void genOperands(const InstClass &ic)
    {
      genOpBaseClasses(ic);

      *_oc << "//\n"
          << "// Operands for instruction class " << ic.name() << "\n"
          << "//\n\n";

      // We might have duplicates due to operand-value constraints, so avoid
      // these by using a set.
      StrSet handled_instrs;

      // This stores operand information, so we can see if we've already
      // generated something that's identical.  If so, then we alias it via a
      // typedef, in order to reduce code size.
      OpVectMap handled_ops;

      // First, construct a map to understand sharing of operands.  This is
      // needed in order to get the mask position allocation correct.  For
      // example, say we have instruction I1 with operands A and B, instruction
      // I2 with operands C and D, and instruction I3 with operands A and C, and
      // A and C both have masks.  Since I3 uses A and C, C's mask position
      // cannot be at 0, but if we processed the instructions in order, we
      // wouldn't know about the sharing of C between I1 and I3, so we would
      // allocate a position of 0 to C when we encountered it in I2.  Therefore,
      // the OpVectInstMap exists to capture that sharing.
      OpVectInstMap shared_ops;

      // Construct sharing data structure here.
      ForEach(ic.instrs(),i) {
        if (!handled_instrs.count(i->_name)) {
          genOperandSharingForInstr(ic,*i,shared_ops);
          handled_instrs.insert(i->_name);
        }
      }

      // cout << "Shared Ops For " << ic.name() << ":\n";
      // print_opvectinstmap(cout,shared_ops);
      // cout << "\n\n";

      // Now construct the actual operands here.
      handled_instrs.clear();
      ForEach(ic.instrs(),i) {
        if (!handled_instrs.count(i->_name)) {
          genOperandsForInstr(ic,*i,handled_ops,shared_ops);
          handled_instrs.insert(i->_name);
        }
      }
    }

    // Used for tracking sharing of interface functions between instructions.
    typedef gc_map<gc_string,pair<gc_string,gc_string> > FuncMap;

    // Generate an interface-function functor.
    void genFunc(const InstClass &ic,StrSet &base_classes,const gc_string &fname,
                 const gc_string &src_name,const gc_string &trg_name)
    {
      const gc_string uname = getuArchClassName(_ir);
      const gc_string bname = getInstrOpBaseName(ic.name(),src_name);

      // We extract the parameters from the target helper function, then add on
      // a reference to the architecture class.
      auto helper = _ir.get_helper(trg_name);
      assert(helper);

      Ptree *ret  = Second(helper->_func);
      Ptree *decl = Third(Third(helper->_func));

      // If we're using virtual dispatch, then we need a base-class for this
      // functor.
      if (_config._vd_enabled && !base_classes.count(src_name)) {
        _of << "// Base-class Interface functor for " << ic.name() << ": " << src_name << "\n"
             << "struct " << bname << " {\n"
             << "virtual ~" << bname << "() {}\n"
             << "virtual " << ret << " operator()(" << uname << " &__uarch__" << ( (decl) ? ", " : "") << decl << ") = 0;\n"
             << "};\n\n";
        base_classes.insert(src_name);
      }

      _of << "// Interface functor:  " << src_name << " -> " << trg_name << "\n"
           << "struct " << fname;
      if (_config._vd_enabled) {
        _of << " : public " << bname;
      }
      _of << " {\n"
           << ret << " operator()(" << uname << " &__uarch__" << ( (decl) ? ", " : "") << decl << ")"
           << " {\n";
      if (!Eq(ret,"void")) {
        _of << "return ";
      }
      _of << "__uarch__." << trg_name << "(";

      while (decl != 0) {
        Ptree *p = decl->Car();
        decl = decl->Cdr();
        Ptree *arg = Second(p);
        if (Ptree *a = Second(arg)) {
          _of << a;
        } else {
          _of << arg;
        }
        if (decl != 0) {
          decl = decl->Cdr();
          _of << ", ";
        }
      }

      _of << ");\n"
           << "}\n"
           << "};\n\n";
    }

    // Generate an interface function alias.
    void genFuncAlias(const gc_string &fname,const gc_string &trg_name)
    {
      _of << "// Function Alias:  " << trg_name << " -> " << fname << "\n"
           << "typedef " << trg_name << " " << fname << ";\n\n";     
    }

    // Generate interface functors for an isntruction. 
    void genFuncsForInstr(const InstClass &ic,StrSet &base_classes,const InstItem &instr,FuncMap &handled_funcs)
    {
      const gc_string uname = getuArchClassName(_ir);

      ForEach(instr._funcs,i) {
        const gc_string &fsrc = i->first;
        const gc_string &ftrg = i->second;

        gc_string iname = makeFriendlyName(instr._name);
        gc_string instr_fname = getInstrOpName(instr._name,ic.name(),fsrc);

        // Try to find an existing alias.  If we're using virtual dispatch, then
        // the base operand name must match as well, due to base-class usage.
        auto x = handled_funcs.find(ftrg);
        if (x != handled_funcs.end() && (!_config._vd_enabled || (fsrc == x->second.first))) {
          genFuncAlias(instr_fname,x->second.second);
        } else {
          genFunc(ic,base_classes,instr_fname,fsrc,ftrg);
          handled_funcs.insert(make_pair(ftrg,make_pair(fsrc,instr_fname)));
        }

        if (_config._vd_enabled) {
          genOpVar(instr_fname,uname + "::");
        }

      }
    }

    // Generates the interface functors for a given instruction class.  These
    // are used as the arguments for the instruction class object itself.  Note
    // that we emit this into the class declaration so that we don't have to
    // modify any types, such as InstrType, which are defined within the main
    // class.
    void genFuncs(const InstClass &ic)
    {
      if (ic.funcs().empty()) return;

      _of << "//\n"
          << "// Interface functors for instruction class " << ic.name() << "\n"
          << "//\n\n";

      // We might have duplicates due to operand-value constraints, so avoid
      // these by using a set.  We also might need to generate base classes if
      // using virtual dispatch, so we do that on an as-needed basis.
      StrSet handled_instrs, base_classes;

      // This stores function information, so we can see if we've already
      // generated something that's identical.  If so, then we alias it via a
      // typedef, in order to reduce code size.
      FuncMap handled_funcs;

      // Now construct the actual operands here.
      handled_instrs.clear();
      ForEach(ic.instrs(),i) {
        if (!handled_instrs.count(i->_name)) {
          genFuncsForInstr(ic,base_classes,*i,handled_funcs);
          handled_instrs.insert(i->_name);
        }
      }
    }

    // Each instruction class is a template, with its operands as template
    // arguments.  Action code are member functions and each has a launch
    // function which is a static version that calls the member.  This allows
    // for us to use function pointers rather than member-function-pointers,
    // which are *much* faster due to stupid C++-isms.
    void genInstClass(const InstClass &ic)
    {
      *_oc << "//\n"
          << "// Instruction class " << ic.name() << "\n"
          << "//\n";

      gc_string theader;

      if (!_config._vd_enabled) {
        if (!ic.operands().empty()) {
          *_oc << "template <";
          theader = "<";
          bool first1 = true, first2 = true;
          ForEach(ic.operands(),i) {
            list_sep(theader,first1);
            theader += getOpType(*i);
            
            list_sep(*_oc,first2);
            *_oc << "class " << getOpType(*i);
          }
          ForEach(ic.funcs(),i) {
            list_sep(theader,first1);
            theader += getOpType(*i);
            
            list_sep(*_oc,first2);
            *_oc << "class " << getOpType(*i);
          }
          theader += ">";
          *_oc << ">\n";
        }
      }

      const gc_string uname = getuArchClassName(_ir);
      const gc_string cname = getInstClassName(ic);
      const gc_string bname = getInstBaseName(_ir);

      *_oc << "struct " << cname << " : public " << bname << " {\n\n"
           << "typedef struct " << cname << theader << " ThisClass;\n\n"
           << cname << " &thisInstr() { return *this; };\n\n";

      // If we're using virtual dispatch, then we need references to the
      // operands.
      if (_config._vd_enabled) {
        ForEach(ic.operands(),i) {
          *_oc << getInstrOpBaseName(ic.name(),*i) << " &" << *i << ";\n";
        }
        ForEach(ic.funcs(),i) {
          *_oc << uname << "::" << getInstrOpBaseName(ic.name(),*i) << " &" << *i << ";\n";
        }
        *_oc << "\n"
             << cname << "(";
        if (have_threads()) {
          *_oc << uname << " *uarch,";
        }
        *_oc << "unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr";
        ForEach(ic.operands(),i) {
          *_oc << ", " << getInstrOpBaseName(ic.name(),*i) << " &" << getOpParm(*i);
        }
        ForEach(ic.funcs(),i) {
          *_oc << ", " << uname << "::" << getInstrOpBaseName(ic.name(),*i) << " &" << getOpParm(*i);
        }
        *_oc << ")";
        bool first = true;
        if (!ic.operands().empty() || !ic.funcs().empty()) {
          *_oc << " : ";
        }
        ForEach(ic.operands(),i) {
          list_sep(*_oc,first);
          *_oc << *i << "(" << getOpParm(*i) << ")";
        }  
        ForEach(ic.funcs(),i) {
          list_sep(*_oc,first);
          *_oc << *i << "(" << getOpParm(*i) << ")";
        }  
        *_oc << "\n"
             << "{\n"
             << "reset(";
        if (have_threads()) {
          *_oc << "uarch,";
        }
        *_oc << "iid,cid,ic,instr);\n"
             << "}\n\n";
      } else {
        // Instantiate the operands.  These should not take any storage space, but
        // we need to instantiate them anyway.
        ForEach(ic.operands(),i) {
          *_oc << getOpType(*i) << " " << *i << ";\n";
        }
        ForEach(ic.funcs(),i) {
          *_oc << getOpType(*i) << " " << *i << ";\n";
        }
      }

      *_oc << "void reset(";
      if (have_threads()) {
        *_oc << uname << " *uarch,";
      }
      *_oc << "unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {\n"
           << bname << "::reset(iid,cid,ic);\n";
      if (have_threads()) {
        *_oc << "_uarch = uarch;\n";
      }
      *_oc << "_instr = instr;\n"
           << "_move = 0;\n"
           << "_action = (StateFunc)" << getActionFunc(ic.machine().initState()) << ";\n"
           << "_flush = (FlushFunc)" << getFlushFunc(ic.machine().initState()) << ";\n";
      // Dummy reset value, so we know we're alive.
      if (_has_shared_stages) {
        *_oc << "_stage = (PipelineStageBase*)1;\n";
      }
      *_oc << "}\n\n";

      genHelpers(ic);

      genTransitionFuncs(ic);

      _deallocs.clear();

      genActions(ic);

      genFlushFuncs(ic);

      *_oc << "};\n\n";
    }

    void genInstClassTop(const InstClass &ic)
    {
      genOperands(ic);
      genFuncs(ic);
      genInstClass(ic);
    }

    void genSingleFile()
    {
      genDummyOperand();
      ForEach(_ir.instClasses(),i) {
        const InstClass &ic = *(i->second);
        genInstClassTop(ic);
      }
    }

    void genMultiFile(const FileMap &icfiles)
    {
      ForEach(_ir.instClasses(),i) {
        const InstClass &ic = *(i->second);

        auto iter = icfiles.find(ic.name());
        assert(iter != icfiles.end());
        FileItem &f = *iter->second;
        _oc = f._o;
        
        if (!f._started) {
          genDummyOperand();
          f._started = true;
        }
        
        genInstClassTop(ic);
      }
    }
 
    void generate(const FileMap &icfiles)
    {
      if (!_config._sep_ics) {
        genSingleFile();
      } else {
        genMultiFile(icfiles);
      }
    }
  };

  GenInstClassIF::GenInstClassIF(const UadlIr &IR, const Thread &thread, ostream &outH, ostream &outF, ostream &outC,
                                 const GenConfig &config,const gc_string &iss_type) :
    _impl(new GenInstClass(IR,thread,outH,outF,outC,config,iss_type))
  {
  }

  pair<unsigned,unsigned> GenInstClassIF::get_mask_info()
  {
    return _impl->get_mask_info();
  }

  const MaskMap &GenInstClassIF::get_mask_map()
  {
    return _impl->get_mask_map();
  }

  void GenInstClassIF::generate(const FileMap &icfiles)
  {
    _impl->generate(icfiles);
  }


}

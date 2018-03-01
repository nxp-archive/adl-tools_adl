//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Code transformer for uadl instruction class code.
//

#include <stdexcept>
#include <sstream>
#include <iostream>
#include <cassert>

#include "opencxx/parser/Ptree.h"
#include "opencxx/parser/PtreeUtil.h"
#include "opencxx/parser/PtreeBlock.h"
#include "opencxx/parser/ptree-generated.h"
#include "opencxx/Environment.h"
#include "opencxx/TypeInfo.h"
#include "opencxx/Bind.h"

#include "helpers/Macros.h"
#include "helpers/CodeGen.h"

#include "fe/Helpers.h"

#include "iss/MiscTypes.h"

#include "UadlCommon.h"
#include "UadlWalker.h"
#include "UadlParser.h"
#include "GenInstClass.h"

using namespace std;
using namespace Opencxx;
using namespace PtreeUtil;

namespace uadl {

  UadlWalker::UadlWalker(Opencxx::Environment *e,bool ld_okay,bool safeMode,const UadlIr &ir,
                         const gc_string &iss_type) :
    RtlWalker(e,ld_okay,false),
    _ir(ir),
    _ic(0),
    _core_env(0),
    _deallocs(0),
    _iss_type(iss_type),
    _can_halt(false),
    _safeMode(safeMode)
  {
    ForEach(_ir.instClasses(),i) {
      _instr_class_enums.insert(i->second->getTypeName());
    }
  }

  // Safety-class:  Make sure we don't get some weird, big string.
  gc_string get_str(Ptree *x)
  {
    gc_string tmp(x->ToString());
    assert(tmp.length() < 255);
    return tmp;
  }

  void UadlWalker::set_class(const InstClass &ic) 
  { 
    _ic = &ic; 
    _core_env = _ic->env()->GetOuterEnvironment();
  }

  // Returns "issue" if no stage is bound to this state.
  gc_string UadlWalker::getStageName()
  {
    const gc_string *stage = 0;

    if (!_ic || ( (stage = _ic->findStageBindingName(_cur_state)) == 0) ) { 
      return getIssueName();
    }
    return *stage;
  }

  bool UadlWalker::is_mgr(Ptree *x)
  {
    gc_string n = get_str(x);
    return _ir.res().findResourceByName(n);
  }

  Ptree *UadlWalker::checkForResource(Ptree *v)
  {
    if (is_mgr(v) || Eq(v,"top")) {
      const char *pfx = (_ic) ? "uarch." : "";
      return Ptree::Make("%s%p",pfx,v);
    }
    return 0;
  }

  Ptree *UadlWalker::checkForInstrClass(Ptree *v)
  {
    gc_string n = get_str(v);
    if (_instr_class_enums.count(n)) {
      return Ptree::Make("%s::%p",_iss_type.c_str(),v);
    }
    return 0;
  }

  Ptree *UadlWalker::checkForInstrClassAttr(Ptree *v)
  {
    gc_string n = get_str(v);
    if (InstClass::get_ic_attrs().count(n)) {
      return Ptree::Make("%s::%p",_iss_type.c_str(),v);
    }
    return 0;
  }

  Ptree* UadlWalker::TranslateVariable(Ptree* v)
  {
    if (Ptree *r = checkForResource(v)) {
      return r;
    }

    if (Ptree *c = checkForInstrClass(v)) {
      return c;
    }

    if (Ptree *c = checkForInstrClassAttr(v)) {
      return c;
    }

    if (_ic) {
      Bind *b = 0;
      if (_core_env && _core_env->LookupTop(v,b)) {
        return Ptree::Make("%s::%p",getuArchClassName(_ir).c_str(),v);
      }
    }

    return RtlWalker::TranslateVariable(v);
  }

  // For handling generated calls.
  Ptree* UadlWalker::TranslatePtree(Ptree* v)
  {
    if (Ptree *r = checkForResource(v)) {
      return r;
    }
    
    return RtlWalker::TranslatePtree(v);
  }

  Ptree *UadlWalker::addExceptionCheck(Ptree *r)
  {
    // If in safe-mode, then the exec call doesn't throw an exception, since
    // it doesn't really do anything.  So, we have to see if we have an
    // exception at this point and then jump to the exception-handling
    // portion.
    if (_ic && _safeMode && _ir.hasExceptions() && _ic->has_exceptions() && 
        _ic->getAction(_ic->machine().getExceptionState(_cur_state))) {
      return Ptree::Make("{%p ; if (has_exceptions()) goto HandleException; }",r);
    } else {
      return r;
    }
  }

  Ptree* UadlWalker::TranslateFuncall(Ptree* exp)
  {
    Ptree *fun = exp->Car();
 
    PtreeArray array;

    gc_string fname = get_str(fun);

    if (_ic) {
      if (fname == "change_state") {
        bool changed;
        populateArgs(array,this,exp,0,changed);

        // We've already checked in the front-end that the arguments are valid, so
        // we can just use them here.
        return Ptree::Make("return %s(uarch,logger,position)",getStateChangeName(_cur_state,getArg(array,0)->ToString()).c_str());
      }
      else if (fname == "exec") {
        // If we can halt, then we call the appropriate exec routine which checks
        // for a possible halt.  If we access memory, then we call the exec_mem
        // function, which handles memory operations.  If we branch, and we're in safe-mode,
        // then we call the branch-checking version of exec.
        Ptree *r = 0;
        if (_can_halt) {
          if (!_safeMode && _ic->uses_mem()) {
            // Even if we halt, we still have to be able to handle memory
            // accesses, since the halt might just be conditional.
            r = Ptree::Make("uarch.exec_mem_%s(*this,logger,position)",getStageName().c_str());
          } else {
            r = Ptree::Make("uarch.exec_%s(*this,logger,position)",getStageName().c_str());
          }
        } else if (_ic->uses_mem()) {
          r = Ptree::Make("uarch.exec_mem(*this,logger)");
        } else if (_ic->has_branch()) {
          r = Ptree::Make("uarch.exec_branch(*this,logger)");
        } else {
          r = Ptree::Make("uarch.exec(*this,logger)");
        }
        return addExceptionCheck(r);
      }
      else if (fname == "capacity") {
        return Ptree::Make("uarch.%s.%p()",getStageName().c_str(),fun);
      }
      else if (fname == "check_latency") {
        return Ptree::Make("%p(logger)",fun);
      }
      else if (fname == "exec_and_read" || fname == "exec_and_write") {
        bool changed;
        populateArgs(array,this,exp,0,changed);
        return addExceptionCheck(Ptree::Make("uarch.%p(*this,logger,%p)",fun,array.All()));
      }
      else if (fname == "stalled" || fname == "getChildId") {
        return Ptree::Make("uarch.%p()",fun);
      }
      else if (fname == "free_all_mem" || fname == "set_speculative") {
        bool changed;
        populateArgs(array,this,exp,0,changed);
        return Ptree::Make("uarch.%p(*this %s %p)",fun,((array.Number()!=0) ? "," : ""),array.All());
      }
      else if (fname == "commit_exceptions") {
        return Ptree::Make("uarch.%p(*this,logger)",fun);
      }
      else if (fname == "branch_taken") {
        return Ptree::Make("%p(uarch)",fun);
      }
      else if (fname == "last_branch_prediction_correct") {
        bool changed;
        populateArgs(array,this,exp,0,changed);
        return Ptree::Make("uarch.%p(*this %s %p)",fun,((array.Number()!=0) ? "," : ""),array.All());
      }
      else if (fname == "flush" || fname == "taken_flush") {
        bool changed;
        populateArgs(array,this,exp,0,changed);
        return Ptree::Make("uarch.%p_%s(*this,logger,position %s %p)",fun,getStageName().c_str(),((array.Number()!=0) ? "," : ""),array.All());
      }
      else if (fname == "can_read_ops" || 
               fname == "can_write_ops" ||
               fname == "read_ops" ||
               fname == "write_ops" ||
               fname == "deallocate_ops") {
        return Ptree::Make("%p(uarch,*this,logger)",fun);
      }
      else if (fname == "allocate_ops") {
        Ptree *exp = Ptree::Make("%p(uarch,*this,logger)",fun);
        _deallocs->push_back(Ptree::Make("deallocate_ops(uarch,*this,logger)",fun));
        return exp;
      }
      else if (fun->IsA(ntDotMemberExpr)) {
        // All operands and resource methods get the same initial hidden parms:
        //
        // arg0:  uarch reference.
        // arg1:  instruction-class reference.
        // arg2:  logger pointer (will be null if no logging).
        //
        Ptree *op     = First(fun);
      
        // In some situations, we may have nesting, e.g. with memory ports.  So,
        // if this is a dot-member expression, take just the first item.
        Ptree *op1 = (op->IsA(ntDotMemberExpr)) ? First(op) : op;

        Ptree *action = Third(fun);

        bool changed;
        populateArgs(array,this,exp,0,changed);

        bool is_op = _ic->is_operand(op);
      
        const char *op1str = op1->ToString();

        bool is_res = _ir.res().findResourceByName(op1str);

        assert(_ic);

        if (is_op || is_res || (getIssueName() == op1str)) {
          Ptree *exp = Ptree::Make("%s%p.%p(uarch,*this,logger%s%p)",(is_op ? "" : "uarch."),op,action,((array.Number()==0) ? "" : ","),array.All());
          gc_string a = action->ToString();
          if (a.substr(0,8) == "allocate") {
            _deallocs->push_back(Ptree::Make("%s%p.de%p(uarch,*this,logger%s%p)",(is_op ? "" : "uarch."),op,action,((array.Number()==0) ? "" : ","),array.All()));
          }
          return exp;
        }
      }
      else if (_ic->is_func(fname)) {
        Ptree *exp2 = RtlWalker::TranslateFuncall(exp);
        Ptree *args = Third(exp2);
        return Ptree::Make("%p(%s%p)",fun,( (args) ? "uarch," : "uarch"  ),args);
      }
    }

    if (_ir.isHelper(fname)) {
      // This is a uadl helper function, so it exists in the main
      // microarchitecture class.  Thus, we have to call it with the appropriate
      // prefix if we're in an instruction class, otherwise no change.
      Ptree *exp2 = RtlWalker::TranslateFuncall(exp);
      const char *uarch_pfx = (_ic) ? "uarch." : "";
      return Ptree::Make("%s%p",uarch_pfx,exp2);
    }
    else if (_ir.isCoreHelper(fname)) {
      // This is a helper function, so it exists in the ISS.  Thus, we have to
      // call it with the appropriate prefix.
      Ptree *exp2 = RtlWalker::TranslateFuncall(exp);
      const char *pfx = (_ic) ? "uarch.get_iss()." : "_iss->";
      return Ptree::Make("%s%p",pfx,exp2);
    }

    return RtlWalker::TranslateFuncall(exp);
  }

  Ptree *UadlWalker::TranslateRegWrite(PtreeRegWrite *exp)
  {
    return exp;
  }

  Ptree *UadlWalker::TranslateRegRead(PtreeRegRead *exp)
  {
    return exp;
  }

  Ptree *UadlWalker::TranslateMemWrite(PtreeMemWrite *exp)
  {
    return exp;
  }

  Ptree *UadlWalker::TranslateMemRead(PtreeMemRead *exp)
  {
    return exp;
  }

  Ptree *UadlWalker::TranslateSubInstrCall(PtreeSubInstrCall *exp)
  {
    return exp;
  }

  Ptree* UadlWalker::TranslateStmts(Opencxx::Ptree*exp)
  {
    return exp;
  }

}

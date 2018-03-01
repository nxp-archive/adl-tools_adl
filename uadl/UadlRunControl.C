//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// The run-control interface is used for alternate interfaces, such as by the
// debugger and the scripting interface.
//

#include <assert.h>

#include "iss/ModelInterface.h"

#include "UadlArch.h"
#include "UadlRunControl.h"

#if !defined(CLI_SUPPORT) || (CLI_SUPPORT != 0)
# define CLI_SUPPORT
#else
# ifndef _MSC_VER
#   warning "CLI Support disabled"
# endif
# undef CLI_SUPPORT
#endif

#ifdef CLI_SUPPORT
#  include "UadlScriptLogger.h"
#endif

using namespace adl;
using namespace std;

namespace uadl {

  UadlRunControl::UadlRunControl() : 
    _arch(0),
    _logger(0)
  {}

  void UadlRunControl::reset(IssNode &node)
  {
    _arch->reset();
  }

  // We remove the tracing stream, because we'll control that later.  We also
  // shut off logging for now- we may enble it based upon event logging.
  void UadlRunControl::setup(ttime_t endTime,uint32_t trace_flags)
  {
    _endTime = endTime;

    assert(_arch);
    _arch->setLogStream(0);
#   ifdef CLI_SUPPORT

    _logger->setup(*_arch,trace_flags);
#   endif
  }

  uint64_t UadlRunControl::instr_count(const IssNode &node) const
  {
    return _arch->instrCount();
  }

  uint64_t UadlRunControl::get_cycle_count(IssNode &node)
  {
    return _arch->currTime();
  }

  ProgramStatus UadlRunControl::status() const
  {
    if (!_arch->is_active()) {
      return HALT;
    } else if (_arch->hasBreakpoint()) {
      return BREAK;
    } else {
      return ACTIVE;
    }
  }

  ProgramStatus UadlRunControl::run(IssNode &node,SimType stype)
  {
    _arch->run(_endTime);
    check_breakpoint();
    return status();
  }

  ProgramStatus UadlRunControl::stepn(unsigned n,IssNode &node)
  {
    for (unsigned i = 0; i != n; ++i) {
      step_model();
    }
    check_breakpoint();
    return status();
  }

  void UadlRunControl::cyclen(unsigned n)
  {
    _arch->preRun();
    for (unsigned i = 0; i != n; ++i) {
      _arch->proceed();
    }
    check_breakpoint();
  }

  void UadlRunControl::genExceptions(unsigned flags,IssNode &node)
  {
    _arch->genExceptions(flags);
  }    

  bool UadlRunControl::cancelExceptions(unsigned flags,IssNode &node)
  {
    _arch->cancelExceptions(flags);
    return true;
  }   
  
  void UadlRunControl::set_dyn_parm(IssNode &node,const std::string &parm,unsigned value)
  {
    _arch->set_dyn_parm(parm,value);
  }
  
  unsigned UadlRunControl::get_dyn_parm(const IssNode &node,const std::string &parm) const
  {
    return _arch->get_dyn_parm(parm);
  }
  
  void UadlRunControl::list_dyn_parm(const IssNode &node,adl::StrPairs &parms) const
  {
    _arch->list_dyn_parm(parms);
  }

  ScriptLogger *UadlRunControl::createScriptLogger()
  {
#   ifdef CLI_SUPPORT
    return _logger = new UadlScriptLogger(this);
#   else
	_logger = 0;
	return 0;
#   endif
  }
    
  void UadlRunControl::step_model()
  {
    _arch->preRun();
    uint64_t ic = _arch->issueCount();
    do {
      _arch->proceed();
    } while (_arch->issueCount() == ic);
  }

  void UadlRunControl::check_breakpoint()
  {
    _arch->clearBreakpoint();
  }

}

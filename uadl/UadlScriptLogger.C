//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This is the script-logger object for the uADL scripting interface.  It
// inherits from the ADL version and adds some extra events, such as for stalls
// and flushes.
//

#include <string>
#include <vector>

#include "helpers/BasicTypes.h"
#include "helpers/Macros.h"

#include "iss/ScriptingIface.h"
#include "iss/StringHelpers.h"

#include "UadlArch.h"
#include "UadlScriptLogger.h"
#include "UadlRunControl.h"

using namespace std;
using namespace adl;

namespace uadl {

# define HandlerDecl(cmd,doc) { #cmd, 0, UadlScriptLogger::cmd, doc }

# define Handler(X) string UadlScriptLogger::X(void *obj,const tCmd *cmd,const Args &args)

# define Self UadlScriptLogger *self = (UadlScriptLogger*)obj;

  // Use this macro for when errors are detected during argument processing.
  // It prints out the usage string for the command.
# define UsageError(cmd,msg) {                                     \
    std::ostringstream ss;                                         \
    ss << cmd << ": " << msg << "  " << scripter().docString(cmd); \
    throw std::runtime_error(ss.str());                            \
  }

  // Use this macro for when usage information is not appropriate.
# define ExecError(cmd,msg) {                   \
    std::ostringstream ss;                      \
    ss << cmd << ": " << msg;                   \
    throw std::runtime_error(ss.str());         \
  }

  //
  // uADL-specific commands for the scripting language.
  //
  static tCmd uadl_commands[] = {
    HandlerDecl(ccount,         "Display the current cycle count.  Usage:  ccount" ),
    HandlerDecl(commitcount,    "Display the current instruction commit count.  Usage:  commitcount" ),
    HandlerDecl(issuecount,     "Display the current instruction issue count.  Usage:  issuecount" ),
    HandlerDecl(cycle,          "Cycle the model 1 or N times.  Usage:  cycle [count]" ),
    HandlerDecl(ptraceon,       "Turn on pipeline tracing immediately.  Usage:  ptraceon [filename|stdout|stderr]"),
    HandlerDecl(ptraceoff,      "Turn off pipeline tracing.  Usage:  ptraceoff" ),
    { 0, 0, 0, 0 }
  };

  UadlScriptLogger::UadlScriptLogger(UadlRunControl *parent) : 
    _parent(parent),
    _arch(0),
    _origTraceFlags(0),
    _logTraceFlags(0)
  {
    initialize();
  }
  
  // This adds in uADL-specific commands to the interpreter.
  void UadlScriptLogger::initialize()
  {
    for (int i = 0; uadl_commands[i].name; i++) {
      uadl_commands[i].obj = this;
      scripter().addCommand(uadl_commands[i]);
    }
  }

  void UadlScriptLogger::setup(UadlArch &arch,uint32_t trace_flags)
  {
    _arch = &arch;
    _core = &_arch->iss();
    _origTraceFlags = trace_flags;
  }

  pair<FuncMap *,int> UadlScriptLogger::get_event_map(const Target &target,const string &event,const string &filter)
  {
    if (event == "stall") {
      return make_pair(&_log_stall,-1);
    } 
    else if (event == "flush") {
      return make_pair(&_log_flush,-1);
    }
    else {
      return ScriptLoggerImpl::get_event_map(target,event,filter);
    }
  }

  // We have to turn on fetch and pipeline logging to get stall information. We
  // always have to call set_tracing_flags in order to make sure that the
  // log-client gets propagated.
  void UadlScriptLogger::registerOurself()
  {
    _arch->setLogClient(this);
    _logTraceFlags = LOG_FETCH | LOG_PIPELINE;
    _arch->set_tracing_flags(_origTraceFlags | _logTraceFlags);
    ScriptLoggerImpl::registerOurself();
  }

  void UadlScriptLogger::unregisterOurself()
  {
    _arch->setLogClient(0);
    _arch->set_tracing_flags(_origTraceFlags);
    _logTraceFlags = 0;
    ScriptLoggerImpl::unregisterOurself();
  }

    // Invokes all functions for function map 'fmap' listed in _funcs with arguments specified by x.
# define Invoke(x) {                                                \
    if (!_funcs.empty()) {                                          \
      _ss.str(""); _ss << dec << ' ' << x;                          \
      ForEach(_funcs,i) {                                           \
        if (*i) { /* Check for a null (was filtered). */            \
          if (!_scripter.eval(_result,(*i)->_func + _ss.str())) {   \
            RError((*i)->_func << ":  " << _result);                \
          }                                                         \
        }                                                           \
      }                                                             \
    }                                                               \
  }
  
  void UadlScriptLogger::logStall(Logger &l,const std::string &msg)
  {
    get_matching_events(_funcs,_core,_log_stall,-1,NullFilter());

    Invoke(l.client_name() << " {" << msg << "}");
  }
  
  void UadlScriptLogger::logFlush(Logger &l,const std::string &msg)
  {
    get_matching_events(_funcs,_core,_log_flush,-1,NullFilter());
    
    Invoke(l.client_name() << " {" << msg << "}");
  }

  //---------------------------------------------------------------------------
  // ccount:  Return the current cycle count.
  //---------------------------------------------------------------------------
  Handler(ccount)
  {
    Self;

    ostringstream ss;
    ss << self->_arch->currTime();
    return ss.str();
  }

  //---------------------------------------------------------------------------
  // commitcount:  Return the current instruction commit count.
  //---------------------------------------------------------------------------
  Handler(commitcount)
  {
    Self;

    ostringstream ss;
    ss << self->_arch->commitCount();
    return ss.str();
  }

  //---------------------------------------------------------------------------
  // issuecount:  Return the current instruction iusse count.
  //---------------------------------------------------------------------------
  Handler(issuecount)
  {
    Self;

    ostringstream ss;
    ss << self->_arch->issueCount();
    return ss.str();
  }

  //---------------------------------------------------------------------------
  // cycle:  Cycle by 1 or N, the model.
  //---------------------------------------------------------------------------
  Handler(cycle)
  {
    Self;

    unsigned arg = 1;
    uint32_t n;
    if (!getUInt(n,args,arg)) {
      n = 1;
    }

    if (self->_arch->is_active()) {
      string result = "Cycling core " + self->_arch->iss().name();
      self->_parent->cyclen(n);
      return result;
    } else {
      ExecError(args[0],"The core is not active.!");
    }
  }


  //---------------------------------------------------------------------------
  // ptraceon:  Turn on pipeline tracing.
  // arg1:  filename | stdout | stderr
  //---------------------------------------------------------------------------
  Handler(ptraceon)
  {
    Self;

    unsigned arg = 1;
    
    string fn = getArg(args,arg);

    if (fn == "stdout" || fn.empty()) {
      self->_arch->setLogStream(&cout);
    }
    else if (fn == "stderr") {
      self->_arch->setLogStream(&cerr);
    }
    else {
      self->_ftrace.open(fn.c_str());
      if (!self->_ftrace) {
        RError("Unable to open trace-file '" << fn << "'.");
      }
      self->_arch->setLogStream(&self->_ftrace);
    }

    // Then turn on all tracing.  This becomes our new set of trace-flags.
    self->_origTraceFlags = (uint32_t)-1;
    self->_arch->set_tracing_flags(self->_origTraceFlags);

    return "1";
  }

  //---------------------------------------------------------------------------
  // ptraceoff:  Turn off pipeline tracing.
  //---------------------------------------------------------------------------
  Handler(ptraceoff)
  {
    Self;

    self->_arch->setLogStream(0);
    self->_arch->set_tracing_flags(self->_logTraceFlags);

    return "1";
  }

}

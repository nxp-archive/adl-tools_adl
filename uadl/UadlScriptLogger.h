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

#ifndef _UADL_SCRIPT_LOGGER_H_
#define _UADL_SCRIPT_LOGGER_H_

#include <string>
#include <sstream>
#include <fstream>

#include "iss/ScriptLoggerImpl.h"

#include "Logger.h"

namespace uadl {

  class UadlArch;
  class UadlRunControl;

# define HandlerI(X) static std::string X(void *obj,const adl::tCmd *cmd,const adl::Args &args);

  struct UadlScriptLogger : public adl::ScriptLoggerImpl, public LogClient
  {
    UadlScriptLogger(UadlRunControl *parent);

    void setup(UadlArch &arch,uint32_t trace_flags);

    virtual std::pair<adl::FuncMap *,int> get_event_map(const adl::Target &target,const std::string &event,const std::string &filter);    

    virtual void registerOurself();

    virtual void unregisterOurself();

    // Called on a stall.
    virtual void logStall(Logger &,const std::string &msg);

    // Called on a flush.
    virtual void logFlush(Logger &,const std::string &msg);

    //
    // uADL-specific commands.
    //
    HandlerI(ccount);
    HandlerI(commitcount);
    HandlerI(issuecount);
    HandlerI(cycle);
    HandlerI(ptraceon);
    HandlerI(ptraceoff);

  protected:
    void initialize();

    UadlRunControl    *_parent;
    UadlArch          *_arch;
    adl::Target        _core;

    uint32_t           _origTraceFlags; // Flags used for pipeline tracing.
    uint32_t           _logTraceFlags;  // Flags used for event-callback tracing.

    adl::FuncMap       _log_stall;
    adl::FuncMap       _log_flush;

    adl::TFuncs        _funcs;
    std::string        _result;
    std::ostringstream _ss;
    std::ofstream      _ftrace;

  };

}

#endif


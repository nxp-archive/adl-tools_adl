//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// The script logger contains functions which should be invoked upon tracing
// actions.  The implementation is hidden from the user behind an interface
// class.
//

#ifndef _SCRIPT_LOGGER_H_
#define _SCRIPT_LOGGER_H_

#include "LoggingMgr.h"

namespace adl {

  class ScriptLogger : public LogParentIface {
    
  public:

    virtual void reset() = 0;

    virtual bool empty() const = 0;

    // Add an event to be called.  Throws a run-time error if the event does not
    // exist.
    virtual void add_event(const Target &target,const std::string &event,const std::string &func,const std::string &filter,
                           unsigned filtercount,addr_t filter1,addr_t filter2) = 0;

    // Same as above, except that the filter-value is specified as an integer,
    // w/no looking performed.  A value of -1 means that the id should be
    // ignored.
    virtual void add_event(const Target &target,const std::string &event,const std::string &func,int id) = 0;

    // Remove an event.  Throws a runtime error if the event or function do not
    // exist.  Returns the index that the event was associated with, if found.
    virtual int del_event(const Target &target,const std::string &event,const std::string &func) = 0;
  };

  // Allocates and returns a new script logger.
  ScriptLogger *makeScriptLogger();

}

#endif

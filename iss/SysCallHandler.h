//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// The system call handler class implements a simple system-call
// support mechanism.  Arguments are collected by SysCallMgr and then
// dispatched to the registered SysCallHandler objects.
//

#ifndef _SYSCALLHANDLER_H_
#define _SYSCALLHANDLER_H_

#include "helpers/BasicTypes.h"

namespace adl {

  struct SysCallMgr;
  
  // The default system call interface is simply a functor.  If it can handle
  // the system call, then it should handle it and return true, otherwise it
  // should return false so that another handler can be tried.
  struct SysCallHandler {

    SysCallHandler() {};
    virtual ~SysCallHandler() {};

    // Handle the system call specified by code.  If handled, returned true,
    // else false if not handled.
    //
    // rc   :  Return code for the system call.
    // errno:  The errno value to be returned for the system call.
    // code :  The system-call identifier code.
    // core :  A reference to the core which generated the system call.
    // mgr  :  The manager object- used for obtaining the arguments.
    virtual bool operator()(int64_t &rc,uint32_t &pgm_errno,uint32_t code,IssCore &core,SysCallMgr &mgr) = 0;
  };

}

#endif


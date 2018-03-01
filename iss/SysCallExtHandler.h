//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This interface class is used to communicate emulated system-call information.
// Derive from this class and then supply a reference to each relevant IssNode
// object.  Install by calling IssNode::setSysCallHandler.  For the standalone
// ADL simulator, a default object is installed which handles standard system
// calls.
//

#ifndef _SYSCALL_EXT_HANDLER_
#define _SYSCALL_EXT_HANDLER_

#include "helpers/BasicTypes.h"

namespace adl {

  class IssCore;

  //
  // This is the external system call interface.  Similar to external memory,
  // when used, all emulated-system-call commands go through this interface
  // class, rather than to the global system, as below.  The user must register
  // this class with the relevant cores at the start of simulation.
  //
  struct SysCallExtHandler {
    virtual ~SysCallExtHandler() {};

    // Returns true if emulated system calls are enabled.
    virtual bool syscall_enabled() const = 0;

    // This triggers the actual system call and is called after arguments have
    // been set up.
    virtual void syscall_trigger(IssCore &core,uint32_t syscode) = 0;

    // Called to store an argument.
    virtual void syscall_add_arg(uint64_t a) = 0;

    // Called by the model to retrieve the return code and errno value.
    virtual int64_t syscall_return_code() = 0;
    virtual uint32_t syscall_errno() = 0;
  };

}

#endif


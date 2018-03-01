//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// The public interface consists of the ability to get and set arguments and trigger
// the occurrence of a system call.
//

#ifndef _SYSCALLMGR_H_
#define _SYSCALLMGR_H_

#include "helpers/BasicTypes.h"

namespace adl {

  class IssCore;
  class SysCallExtHandler;
  class SysCallHandler;

  //
  // Helper functions for implementing system calls.
  //

  // Reads 'len' bytes of memory from ea into buf.  Translation is performed.
  // if is_string is true, this is treated as a string- we only read up until a
  // null character.
  int read_memory(char *buf,int len,bool is_string,addr_t ea,IssCore &core);

  // Reads memory from ea into str, stopping at a null.  Translation is performed.
  std::string read_string(addr_t ea,IssCore &core);

  // Writes 'len' bytes of memory from buf into ea.  Translation is performed.
  void write_memory(const char *buf,int len,addr_t ea,IssCore &core);

  //
  // The model's interface for system calls.
  //

  //
  // A handler receives a reference to this object for querying about arguments.
  //
  struct SysCallMgr {
    
    virtual ~SysCallMgr() {};

    virtual unsigned size_args() const = 0;
    virtual uint64_t get_arg(unsigned index) = 0;
    virtual bool is_64bit() const = 0;

    // Register a system call handler.  A null pointer is simply ignored.
    virtual void addHandler(SysCallHandler *sc) = 0;
  };

  // Factory for creating a default system-call object.
  SysCallExtHandler *createDefaultSysCallMgr();

}

#endif

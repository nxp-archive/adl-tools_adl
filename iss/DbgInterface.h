//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Any debugger interface library should have its main implementation
// class derived from this object.
//

#ifndef _DBGINTERFACE_H_
#define _DBGINTERFACE_H_

namespace adl {

  class DbgInterface {
  public:
    virtual ~DbgInterface() {};

    // Should return when done- the simulator will then exit.
    // Should throw standard exceptions for error handling.
    virtual void exec() = 0;

  };

}

#endif

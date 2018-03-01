//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// ADL specific code for the GDB interface.
// The only external interface is a factory for returning a debugger
// interface which implements the GDB protocol.
//

#ifndef REMOTE_GDB_SERVER_H
#define REMOTE_GDB_SERVER_H

namespace adl {

  class DbgInterface;
  class RunControlIface;

  // Call this to check to see if GDB mode is requested.
  // Returns a debugger-interface object if true, 0 otherwise.
  DbgInterface *checkForGdbMode(IssNode &root,RunControlIface &runctrl);

}

#endif

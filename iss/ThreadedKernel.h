//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef _THREADEDKERNEL_H_
#define _THREADEDKERNEL_H_

#include "helpers/BasicTypes.h"

namespace adl {

  class IssNode;

  // A thread pool encapsulates all of the threads used for performing an MT
  // simulation.  They persist between calls to threadedSimRun() in order to
  // save on thread startup overhead.  You must create this object first, before
  // calling threadedSimRun().  After it's created, the root node cannot be
  // changed.
  //
  // Make sure to delete the thread pool in order to properly shut down the
  // threads.
  struct ThreadPool {
    virtual ~ThreadPool() {};
  };

  ThreadPool *createThreadPool(IssNode &root);

  //
  // Entry point to the threaded simulation kernel, for models compiled with MT
  // simulation support.  Not currently supported in Windows.
  //
  // Returns BREAK if a breakpoint occurred, otherwise ACTIVE.
  //
  ProgramStatus threadedSimRun(IssNode &root,SimType smode,ThreadPool *pool);


# ifdef _MSC_VER
  inline ProgramStatus threadedSimRun(IssNode &root,SimType smode) { return HALT; }
# endif

}

#endif

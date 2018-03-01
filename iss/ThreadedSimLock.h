//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This defines a simple lock type which is controlled by a global boolean.  The
// global boolean indicates whether the simulation, as a whole, should be thread
// safe or not, thus allowing us to not pay a performance penalty for locks when
// no threading is required.
//

#ifndef _THREADED_SIM_LOCK_
#define _THREADED_SIM_LOCK_

namespace adl {

  // This global should be set to true if multiple threads will be used by the
  // simulation.
  extern bool ThreadedSim;

  // This lock will only lock the mutex if ThreadedSimEnabled is true.
  template <typename Mutex>
  class AdlSimLock {

  public:
    // Make not copyable.
    AdlSimLock(AdlSimLock const&) = delete;
    AdlSimLock& operator=(AdlSimLock const&) = delete;
    
    explicit AdlSimLock(Mutex& m) : _m(m)
    {
      if (ThreadedSim) {
        _m.lock();
      }
    }

    ~AdlSimLock()
    {
      if (ThreadedSim) {
        _m.unlock();
      }
    }

  private:
    Mutex& _m;
  };

}

#endif

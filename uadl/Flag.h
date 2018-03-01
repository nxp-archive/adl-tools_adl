//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef _FLAG_H_
#define _FLAG_H_

#include <limits>

#include "Logger.h"

namespace uadl {

  //
  // A flag is like a semaphore with a value of 1, except that it allows for a delayed-release
  // semantics.  This allows an instruction to release a flag at a future event, e.g. 2 cycles
  // after the instruction is committed.  It works using a simple time-stamp; the flag is available
  // if current time is >= flag's time.
  //
  class Flag {
  public:
    Flag(const std::string &n) : _name(n), _time(0) {};

    void reset() { _time = 0; };

    const std::string &name() const { return _name; };

    bool can_allocate(UadlArch &uarch,InstrBase &,Logger *logger) const { 
      bool ok = check_allocate(uarch);
      if (logger && !ok) {
        log_acquire_fail(uarch,logger);
      }
      return ok; 
    };

    // Same query as can_allocate(), but doesn't log a fail, so is useful for
    // when flags are used for communications and not directly for
    // stalling.
    bool check_allocate(UadlArch &uarch,InstrBase &,Logger *) const { 
      return check_allocate(uarch);
    }

    void allocate(UadlArch &uarch,InstrBase &instr,Logger *logger) { 
      _time = std::numeric_limits<ttime_t>::max(); 
      if (logger) log_action(uarch,logger,"acquire ",1,instr.iid());
    };
    void deallocate(UadlArch &uarch,InstrBase &instr,Logger *logger,int delay = 0) { 
      _time = uarch.currTime() + delay;
      if (logger) log_action(uarch,logger,"release ",-1,instr.iid());
    };
    
  protected:
    void log_acquire_fail(UadlArch &uarch,Logger *logger) const;
    void log_action(UadlArch &uarch,Logger *logger,const char *msg,int count,uint64_t id);
    bool check_allocate(UadlArch &uarch) const { return uarch.currTime() >= _time; };

    std::string    _name;
    ttime_t        _time;
  };

}

#endif

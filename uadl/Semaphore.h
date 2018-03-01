//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef _SEMAPHORE_H_
#define _SEMAPHORE_H_

#include "Logger.h"

namespace uadl {

  //
  // A semaphore is basically just a generic counter.  However, each object also
  // stores a max-value and has a query to compare the current counter against
  // this maximum.
  //
  class Semaphore {
  public:
    Semaphore(const std::string &n,unsigned max) : _name(n), _max(max), _count(0) {};

    void reset() { _count = 0; };

    const std::string &name() const { return _name; };
    unsigned count(UadlArch &,InstrBase &,Logger *) const { return _count; };

    bool can_allocate(UadlArch &uarch,InstrBase &,Logger *logger) const { 
      bool ok = check_allocate();
      if (logger && !ok) {
        log_acquire_fail(uarch,logger);
      }
      return ok; 
    };

    // Same query as can_allocate(), but doesn't log a fail, so is useful for
    // when semaphores are used for communications and not directly for
    // stalling.
    bool check_allocate(UadlArch &uarch,InstrBase &,Logger *) const { 
      return check_allocate();
    }

    void allocate(UadlArch &uarch,InstrBase &instr,Logger *logger) { 
      ++_count; 
      if (logger) log_action(uarch,logger,"acquire ",_count,1,instr.iid());
    };
    void deallocate(UadlArch &uarch,InstrBase &instr,Logger *logger) { 
      // Just to be safe, make sure it's not already 0.
      if (_count) --_count; 
      if (logger) log_action(uarch,logger,"release ",_count,-1,instr.iid());
    };
    void clear(UadlArch &uarch,InstrBase &instr,Logger *logger) { 
      if (logger) log_action(uarch,logger,"clear ",0,-_count,instr.iid());
      _count = 0; 
    };
    
  protected:
    void log_acquire_fail(UadlArch &uarch,Logger *logger) const;
    void log_action(UadlArch &uarch,Logger *logger,const char *msg,unsigned cur_count,int count,uint64_t id);
    bool check_allocate() const { return _count < _max; };

    std::string    _name;
    const unsigned _max;
    unsigned       _count;
  };

}

#endif

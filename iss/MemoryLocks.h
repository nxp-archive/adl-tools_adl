//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Data structures used by models for locking areas of memory, when used in a
// multi-threaded simulation.  This is separate from the memory model itself, in
// order to place fewer constraints on the memory model.
//
// It is organized as a two-level hash: The top-level is a hash of pointers to
// mutexes, whereas the second is a hash of mutexes.  The top-level is
// core-private, to avoid locks, the second level is global to all cores in the
// simulation and is thus locked on updates.
//

#ifndef _MEMORY_LOCKS_H_
#define _MEMORY_LOCKS_H_

#include <tr1/unordered_map>

#include <boost/thread/mutex.hpp>
#include <boost/thread/locks.hpp>

#include "helpers/BasicTypes.h"

namespace adl {

  typedef boost::mutex MemMutex;
  typedef boost::unique_lock<MemMutex> MemLock;

  // The hash that's global to all cores.  The only operation is to retrieve a
  // pointer to a mutex, which is a thread-safe operation (data structure is
  // locked during the access).
  class LockedMutexHash : public std::tr1::unordered_map<addr_t,MemMutex*> {

    typedef std::tr1::unordered_map<addr_t,MemMutex*> Base;
    typedef boost::lock_guard<boost::mutex> Lock;

    // By default, lock on 4k pages.
    enum {
      PageSize = 4096,
      PageMask = (PageSize-1),
    };

  public:

    ~LockedMutexHash()
    {
      for (iterator i = begin(); i != end(); ++i) {
        delete i->second;
      }
    }

    MemMutex *get(addr_t addr) 
    {
      Lock lock(_mutex);
      std::pair<Base::iterator,bool> p = Base::insert(std::make_pair(get_page(addr),(MemMutex*)0));
      if (p.second) {
        // An actual insert happened, so allocate a mutex.
        p.first->second = new MemMutex;
      }
      return p.first->second;
    }

    MemMutex &get_instr_mutex() { return _instr_mutex; };

    static addr_t get_page(addr_t a) { return a & PageMask; };    

  private:

    MemMutex _mutex;
    MemMutex _instr_mutex;
  };

  // The private hash, not thread-safe.  Stores pointers to the mutexes which
  // actually exist in the global hash.
  class PrivateMutexHash : public std::tr1::unordered_map<addr_t,MemMutex *> {
  public:
    typedef std::tr1::unordered_map<addr_t,MemMutex *> Base;

    PrivateMutexHash() : _global_hash(0) {};

    void set_global_hash(LockedMutexHash *gh) { _global_hash = gh; };

    bool valid() const { return _global_hash; };

    MemMutex &get(addr_t addr) 
    {
      std::pair<Base::iterator,bool> p = Base::insert(std::make_pair(LockedMutexHash::get_page(addr),(MemMutex*)0));
      if (p.second) {
        // An actual insert happened, so get a mutex from the next level.
        p.first->second = _global_hash->get(addr);
      }
      return *p.first->second;
    }

    MemLock get_mem_lock(addr_t addr,bool flag)
    {
      return (valid() && flag) ? MemLock(get(addr)) : MemLock();
    }

    MemLock get_instr_lock()
    {
      return (valid()) ? MemLock(_global_hash->get_instr_mutex()) : MemLock();
    }

  private:
    LockedMutexHash *_global_hash;
  };

  struct StackFlag {

    StackFlag(bool &flag) : _flag(&flag) 
    {
      *_flag = true;
    };

    ~StackFlag()
    {
      *_flag = false;
    }
    
    bool            *_flag;
  };


  // Use this define to create a lock using the atomic instruction mutex.
  // x:  Name of MemoryLocks member variable.
# define LockAtomicInstr(x,flag) MemLock __lock__(x.get_instr_lock()); StackFlag __flags__(flag);

  // Use this define to create a lock using the memory-lock hash.
  // x:    Name of MemoryLocks member variable.
  // addr: Address to lock.
# define LockMemory(x,flag,addr) MemLock __lock__(x.get_mem_lock(addr,flag)); 

}

#endif

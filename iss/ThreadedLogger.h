//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Base classes for the multi-threaded logging manager.  Basically, the logging
// manager runs its own thread which handles logging manager client I/O.  The
// model threads send data via a queue; the data is read from the queue by this
// separate thread.  So, if there's no tracing, then the thread doesn't do
// anything.
//

#ifndef _THREADED_LOGGING_MGR_H_
#define _THREADED_LOGGING_MGR_H_

#ifdef LOCK_FREE_QUEUE

#include <boost/lockfree/queue.hpp>

#else

#include <deque>
#include <boost/thread/mutex.hpp>
#include <boost/thread/lock_guard.hpp>

#endif


#include "LoggingMgr.h"

namespace adl {

  // Return current time in nanoseconds, for ordering instructions across
  // multiple threads.
  uint64_t get_nstime();

  // Base class object for all tracing.  For each type of logging event, a class
  // is generated which derives from this one and stores its parameters.  This
  // is then placed onto a lockfree queue, to be consumed by the background
  // tracing thread.
  struct MtLogBase {
    virtual ~MtLogBase() {};

    // Called by the logging thread to perform the appropriate logging call.
    virtual void doLog(LoggingIface *iface) {};

    // Returns 0 for everything but the commit object, which should return the
    // commit-time.
    virtual uint64_t get_time_stamp() { return 0; };
  };

  class ThreadedLogger : public LoggingObjBase {
  public:
    ThreadedLogger();

    void registerLogger(LogParentIface *parent,LoggingIface* iface);
    void unregisterLogger(LogParentIface *parent);

    // Return true if we handled some logging data.
    bool handleLogging();

    // This is called when we're merging multiple cores' trace streams into a
    // single file in order to get a total-order trace.  Returns the commit
    // time-stamp of the next instruction and saves that next instruction to the
    // pending list.  Returns 0 if no elements.
    uint64_t handleMergedLogging();

    // This commits the pending trace data then discards it.
    void commitMergedLogging();

    // Dummy function for compatibility with the transactional-ISS logging features.
    // Direct logging is always on.
    bool log_direct(bool d) { return true; }

    LogType log_get_pending_queue() { return 0; }

    void log_append_pending_queue(LogType p) {}

    void log_clear_instr_reads() {}

    // Dummy function for flushing.
    void log_flush_pending() {}

    void log_discard(InstrPacketBase *) {}

    // Dummuy function for committing.
    void log_commit(InstrPacketBase *) {}
  protected:
    void run_internal();

#   ifdef LOCK_FREE_QUEUE

    typedef boost::lockfree::queue<MtLogBase *> TracingQueue;

#   else

    // The basic locked variant.
    struct TracingQueue : private std::deque<MtLogBase *> {
      typedef boost::lock_guard<boost::mutex> scoped_lock;

      typedef std::deque<MtLogBase *> Base;

      // For interface compatibility with the lock-free variant.
      TracingQueue(unsigned x) {};

      bool push(MtLogBase *v) 
      {
        scoped_lock lock(_mutex);

        Base::push_back(v);

        return true;
      }

      bool pop(MtLogBase* &v)
      {
        scoped_lock lock(_mutex);

        if (Base::empty()) return false;

        v = Base::front();
        Base::pop_front();

        return true;
      }

    private:
      boost::mutex _mutex;
    };

#   endif

    typedef std::vector<MtLogBase*> PendingList;

    LoggerClients   _clients;     // Everything to which we should broadcast our
                                  // results.
    TracingQueue    _tqueue;      // The lockfree queue.
    PendingList     _pending;     // Pending instruction's trace information.
    uint64_t        _pending_ts;  // Pending instruction time-stamp.  0 => Nothing pending.
  };

}

#endif

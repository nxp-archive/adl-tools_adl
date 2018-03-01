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
// separate thread.
//

#include <boost/thread/thread.hpp>

#include "helpers/Macros.h"

#include "ThreadedLogger.h"

using namespace std;

namespace adl {

  enum {
    MaxQueueSize = 1000000, // Maximum capacity of the lockfree queue.
  };

  uint64_t get_nstime()
  {
    struct timespec tps;
    if (clock_gettime(CLOCK_REALTIME, &tps) != 0) {
      throw runtime_error("clock_gettime");
    }
    return tps.tv_sec * 1000000000 + tps.tv_nsec;
  }
  

  ThreadedLogger::ThreadedLogger() :
    _tqueue(MaxQueueSize),
    _pending_ts(0)
  {}

  void ThreadedLogger::registerLogger(LogParentIface *parent,LoggingIface* iface)
  {
    _clients.push_back(make_pair(parent,iface));
  }
 
  void ThreadedLogger::unregisterLogger(LogParentIface *parent)
  {
    auto iter = _clients.begin();
    while (iter != _clients.end()) {
      if ((*iter).first == parent) {
        _clients.erase(iter);
        break;
      }
      iter++;
    }
  }

  bool ThreadedLogger::handleLogging()
  {
    unsigned count = 0;  
    MtLogBase *mtp = 0;

    while (_tqueue.pop(mtp)) {
      ++count;
      ForEach(_clients,i) {
        mtp->doLog(i->second);
      }
      delete mtp;
    }

    return count;
  }

  // Try to get up to the next commit message and save everything to the pending
  // list.  Return the commit's time stamp.  If we already have something
  // pending, then just return its time.
  uint64_t ThreadedLogger::handleMergedLogging()
  {
    MtLogBase *mtp = 0;
    uint64_t    ts = 0;

    // We have something pending, so return its timestamp.
    if (_pending_ts) return _pending_ts;

    while (_tqueue.pop(mtp)) {
      if ( !(ts = mtp->get_time_stamp()) ) {
        // 0 value means not a commit object.
        _pending.push_back(mtp);
      } else {
        // We don't need the commit value any more, unless we want to print out time stamps.
        delete mtp;
        //_pending.push_back(mtp);
        _pending_ts = ts;
        return ts;
      }
    }
    return 0;
  }

  // Log the pending data and then delete it.
  void ThreadedLogger::commitMergedLogging()
  {
    ForEach(_pending,iter) {
      ForEach(_clients,citer) {
        (*iter)->doLog(citer->second);
      }
      delete *iter;
    }
    _pending.clear();
    _pending_ts = 0;
  }

}

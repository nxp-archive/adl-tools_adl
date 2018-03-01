//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Further support for the standalone executable logging manager.  This contains
// non-generated, constant support infrastructure.
//

#include <assert.h>
#include <algorithm>
#include <vector>
#include <memory>
#ifndef _MSC_VER
# include <sched.h>
#endif
#include <limits>

#ifndef _MSC_VER
# include <boost/thread/thread.hpp>
# include <boost/atomic.hpp>
#endif

#include "helpers/Macros.h"

#include "ModelInterface.h"
#include "LoggingMgr.h"
#include "SimInterface.h"

using namespace std;

namespace adl {

  typedef vector<LogParentIface*> LogMgrClients;

  typedef unique_ptr<LoggingObjBase> LoggingObjBasePtr;

  struct LogInfo {
    LoggingObjBasePtr  _obj;
    IssCore           *_core;

    LogInfo (LoggingObjBasePtr &&obj,IssCore *c) : _obj(move(obj)), _core(c) {};
    LogInfo (LogInfo &&x) : _obj(move(x._obj)), _core(x._core) {};
    LogInfo &operator=(LogInfo &&x) {
      _obj = move(x._obj);
      _core = x._core;
      return *this;
    }
  };

  typedef vector<LogInfo> Loggers;

  // Call this to instantiate a new logging object.
  LoggingObjBase *createLoggingObj(IssCore &core,bool mtsafe);

  //
  // Main implementation class of the logging manager used for standalone
  // executables.  This manages multiple LoggingMgrIface objects, which each have
  // subcomponents (LoggingIface) installed for each core in the system.
  //
  // Note: The root must be established first, before any clients are added.
  class LoggingMgrImpl : public LoggingMgr {

  protected:
    IssNode       *_root;
    LogMgrClients  _clients;
    Loggers        _loggers;
    bool           _multicore;

  public:

    LoggingMgrImpl() : 
      _root(0),
      _multicore(false)
    {
    };

    bool use_mt() const { return false; };

    void installLoggers(IssNode *n)
    {
      if (n) {
        if (IssCore *c = dynamic_cast<IssCore*>(n)) {
          LoggingObjBasePtr obj(createLoggingObj(*c,use_mt()));
          if (!_loggers.empty()) _multicore = true;
          c->setLogger(obj.get());
          _loggers.push_back(LogInfo(move(obj),c));
        }
        const IssChildren &children = n->children();
        for (IssChildren::const_iterator i = children.begin(); i != children.end(); ++i) {
          installLoggers(*i);
        }
      }
    }

    void registerLogger(LogParentIface *p)
    {
      assert(p);

      // First, record the new client.
      _clients.push_back(p);

      // Then, for each logger (which corresponds to each core), inform the
      // parent, so that it can produce a logging object for the core.
      ForEach(_loggers,iter) {
        LogInfo &info = *iter;
      
        if (LoggingIface *iface = p->register_core(*info._core)) {
          // Don't add if, for some reason, we get a null pointer, so that we
          // don't have to check for that situation when using the registered
          // loggers.
          info._obj->registerLogger(p,iface);
        }
      }
    }
 
    void unregisterLogger(LogParentIface* p)
    {
      // Remove from the list of clients.
      auto iter = find(_clients.begin(),_clients.end(),p);
      if (iter != _clients.end()) {
        _clients.erase(iter);
      }

      // Now remove all interface clients associated with the parent.
      ForEach(_loggers,iter) {
        iter->_obj->unregisterLogger(p);
      }
    }

  };

# ifndef _MSC_VER

  // This is the MT variant: A child thread handles all of the I/O; the main
  // thread gets logging information and pushes it onto a lockless queue.
  class LoggingMgrMtImpl : public LoggingMgrImpl {
  public:
    LoggingMgrMtImpl() : 
      _iothread(0),
      _done(false)
    {
    }

    bool use_mt() const { return true; };

    // We're about to start getting trace information, so start a background
    // thread to receive this information and do the actual I/O.
    void startLogging()
    {
      _done = false;
      assert(_iothread == 0);
      _iothread = new boost::thread(&LoggingMgrMtImpl::run_internal,this);
    }

    void doneLogging()
    {
      if (_iothread) {
        _done = true;
        _iothread->join();
        delete _iothread;
        _iothread = 0;
      }
    }

    bool handleLogging()
    {
      bool empty = true;

      ForEach(_loggers,iter) {
        if (iter->_obj->handleLogging()) {
          empty = false;
        }
      }

      return !empty;
    }

    // Single stream (single core).  Simply grab data from the incoming queue
    // and deliver it to the writers.
    void runSingleStream()
    {
      while (!_done) {

        if (!handleLogging()) {
          sched_yield();
        }

      }

      // Once done, make sure we've taken care of any remaining elements.
      handleLogging();
    }

    bool handleMergedLogging()
    {
      int min_index = -1;
      uint64_t min_ts = numeric_limits<uint64_t>::max();
      uint64_t all_ts[_loggers.size()];

      // First, scan all loggers in order to record a full instruction and get
      // the minimum time.
      for (unsigned i = 0; i != _loggers.size(); ++i) {
        if (uint64_t ts = _loggers[i]._obj->handleMergedLogging()) {
          all_ts[i] = ts;
          // We have an instruction, so let's see if it's the current first.
          if (ts < min_ts) {
            min_ts = ts;
            min_index = i;
          }
        }
      }

      // Then commit the winner and start all over again.  We have to
      // re-evaluate, since we don't know what's come in on all of the cores.
      if (min_index >= 0) {
        _loggers[min_index]._obj->commitMergedLogging();
        return true;
      }

      return false;
    }

    // Here, we have multiple cores, and thus multiple queues of data.  We order
    // them via time stamps in order to create an ordered list of logging data.
    void runMergedStream()
    {
      while (!_done) {
        if (!handleMergedLogging()) {
          sched_yield();
        }
      }

      // Then handle any remaining items (drain the queues).
      while (handleMergedLogging());
    }

    // Run as the background thread.
    void run_internal()
    {
      if (!_multicore) {
        runSingleStream();
      } else {
        //cout << "Using merged single-stream logging.\n";
        runMergedStream();
      }
    }

    boost::thread       *_iothread;
    boost::atomic<bool>  _done;
  };

# endif

  LoggingMgr *createLoggingMgr(bool use_mt)
  {
#   ifdef _MSC_VER
    return new LoggingMgrImpl;
#   else    
    return (use_mt) ? new LoggingMgrMtImpl : new LoggingMgrImpl;
#   endif
  }

}

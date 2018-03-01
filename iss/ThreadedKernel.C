//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This contains the multi-threaded kernel.  Models have to be compiled with MT
// support in order to use this kernel, which runs all top-level core items in
// separate threads.
//

#ifndef _MSC_VER

#include <vector>

#include <boost/thread/mutex.hpp>    
#include <boost/thread/thread.hpp>
#include <boost/thread/condition.hpp>
#include <boost/thread/barrier.hpp>

#include "helpers/Macros.h"

#include "ThreadedKernel.h"
#include "ModelInterface.h"
#include "MemoryLocks.h"

#define THREAD_DEBUG1
//#define THREAD_DEBUG2

#ifdef THREAD_DEBUG1
# define TRDBG1(x) if (VerboseMode) { scoped_lock my_io_lock(io_mutex); std::cout << x << endl; }
#else
# define TRDBG1(x) 
#endif

#ifdef THREAD_DEBUG2
# define TRDBG2(x) { scoped_lock my_io_lock(io_mutex); std::cout << x << endl; }
#else
# define TRDBG2(x) 
#endif

using namespace std;
using namespace boost;

typedef mutex::scoped_lock    scoped_lock;

// Used for miscellaneous I/O.
static mutex io_mutex;

namespace adl {

  class SimThread;

  typedef list<SimThread*> SimThreads;

  // Global data, shared by all threads in an MT run.
  struct ThreadPoolImpl : public ThreadPool {

    ThreadPoolImpl(unsigned bs,IssNode &root);
    ~ThreadPoolImpl();

    void setSimMode(SimType smode)
    {
      _smode = smode;
    }

    SimType smode() const
    {
      return _smode;
    }

    void reset(IssNode &root)
    {
      // Safety check- we shouldn't mix thread pools with different systems,
      // since differing numbers of cores will mess things up.
      if (_root != &root) {
        RError("Error:  A thread pool may not be used with a different system from the one with which it was created.");
      }

      _quit = false;
      _brkpt = false;
      _wait_count = 0;
    }

    void wait_on_start()
    {
      _start_barrier.wait();
    }

    void wait_on_done()
    {
      _done_barrier.wait();
    }

    unsigned           _active_count, _wait_count;

    // Used for run-control.
    mutex              _run_mutex;
    condition_variable _run_cond;

    // Global signals across all threads.
    bool               _quit;
    bool               _brkpt;
    bool               _full_stop;

    // Used for memory locking.
    LockedMutexHash    _mem_locks;

  private:
    IssNode           *_root;
    SimType            _smode;

    // Coordinates simulation start across all threads.
    barrier            _start_barrier;
    barrier            _done_barrier;

    SimThreads         _threads;
  };


  // Represents a single simulation thread, associated with an ADL object.  We
  // only really need this class because Boost Thread isn't copyable and we're
  // not yet using C++11 with move semantics.
  class SimThread : public ActiveCallback {
  public:
    SimThread(IssNode *node,IssNode *root,int tid,ThreadPoolImpl &pool) : 
      _pool(pool),
      _tid(tid),
      _node(node), 
      _root(root),
      _mythread(0),
      _orig_tdt(_node->get_td_threshold()),
      _exception(false)
    {
      assert(node);
    };

    ~SimThread()
    {
      delete _mythread;
    }

    // Start the thread.
    void run()
    {
      _node->setMemoryLocks(&_pool._mem_locks);
      _mythread = new boost::thread(&SimThread::run_internal,this);
    }

    // Wait on the thread to finish.
    void join()
    {
      _mythread->join();
    }

  private:

    unsigned countActive(IssNode &n)
    {
      unsigned c = 0;
      ForEach(n.children(),i) {
        c += (*i)->is_active();
      }
      return c;
    }

    // Called when the activity count for this node is modified.  We then signal
    // the overall system so that any nodes which are asleep can wake up and
    // handle this event.
    unsigned set_active_count(IssCore *core,bool &active_count,unsigned new_count)
    {
      unsigned old_count;
      {
        // Lock and get the global activity count.  If 0, then we signal to
        // quit.
        scoped_lock lock(_pool._run_mutex);

        old_count = active_count;
        active_count = new_count;
       
        // If all cores in the system are now inactive, then we quit.
        _pool._quit = !_root->is_active();
        _pool._active_count = countActive(*_root);
        TRDBG1("Thread " << _tid << ":  New activity is " << active_count << ", quit is " << _pool._quit);
      }

      // Notify any sleeping nodes to wake-up, in order to evaluate their run
      // status.
      _pool._run_cond.notify_all();
      return old_count;
    }

    // Called when an exception occurs.  We note the fact and clear the td
    // threshold so that we return to run_internal.  We also make up the cores
    // in case it's asleep due to a synchronization event.
    void trigger_exception(unsigned)
    {
      _node->set_td_threshold(0);
      _exception = true;
      TRDBG1("Thread " << _tid << ":  Got exception.");
    }

    void simulate()
    {
      try {
        while (1) {
          ProgramStatus ps = _node->run(_pool.smode());
          TRDBG1("Thread " << _tid << ":  Out of run: " << _node->get_td_counter() << "/" << _node->get_td_threshold());
          switch (ps) {
          case BREAK: {
            // We got a breakpoint, so we need to shut down the other cores.  We
            // do this by setting their td thresholds to 0.
            scoped_lock lock(_pool._run_mutex);
            TRDBG1("Thread " << _tid << ":  Got a breakpoint.");
            _pool._brkpt = true;
            _root->set_td_threshold(0);
            // Wake up anybody who is sleeping due to synchronization.
            _pool._run_cond.notify_all();
            goto Done;
          }
          case ACTIVE:
            if (!_exception && !_pool._brkpt && _node->get_td_counter() >= _node->get_td_threshold()) {
              // Exceeded time quanta, so wait for others to catch up.
              scoped_lock lock(_pool._run_mutex);
              if (_pool._active_count > 1) {
                TRDBG1("Thread " << _tid << ":  Time-quanta synchronization:" << _node->get_td_counter() << "/" << _node->get_td_threshold() );
                _node->set_td_counter(0);
                if (++_pool._wait_count >= _pool._active_count) {
                  // Everybody else is waiting, so we can notify everybody to now continue.
                  TRDBG1("Thread " << _tid << ":  Waking everybody up from synch sleep.");
                  _pool._wait_count = 0;
                  _pool._run_cond.notify_all();
                } else {
                  // We have to wait here until everybody else gets here.
                  TRDBG1("Thread " << _tid << ":  Going to sleep in order to synchronize.");
                  _pool._run_cond.wait(lock);
                }
              } else {
                // Don't bother waiting if there's only one of us.  We also
                // boost up the TD threshold, since there's no need to keep
                // exiting back to the kernel.
                _node->set_td_threshold(InstrMax ? InstrMax : (::uint64_t)-1);
                _node->set_td_counter(0);
                continue;
              }
            }
            // Else fall through.
          case HALT: {
            scoped_lock lock(_pool._run_mutex);
            if (_pool._quit) {
              // We're done with the entire simulation, as the global quit flag
              // has been signaled.
              TRDBG1("Thread " << _tid << ":  Got global quit signal.");
              goto Done;
            } else if (!_node->is_active() && !_pool._quit && !_pool._brkpt) {
              // We're not active, so we go to sleep.  Note that the condition
              // variable unlocks the run mutex.
              TRDBG1("Thread " << _tid << ":  Going to sleep due to inactivity.");
              _pool._run_cond.wait(lock);
            } else if (_node->get_td_counter() >= _node->get_td_threshold()) {
              TRDBG1("Thread " << _tid << ":  Exceeded time quantum.");
              if (_exception) {
                // Cleared due to external exceptions, so restore it and
                // continue.
                TRDBG1("Thread " << _tid << ":  Restoring td threshold, applying exceptions, and continuing.");
                _node->applyExceptions();
                _node->set_td_threshold(_orig_tdt);
                _exception = false;
              } else if (InstrMax && (_node->instr_count() >= InstrMax)) {
                TRDBG1("Thread " << _tid << ":  Exceeded max instruction count.");                
                _node->set_active_count(0);
                goto Done;
              } else if (_pool._brkpt) {
                // If we have a breakpoint, then we also neeed to exit.
                goto Done;
              }
            }
            // Otherwise, we're now awake, so we should run.
            TRDBG1("Thread " << _tid <<":  Now running.");
          }
          }
        }
      Done: ;
      }
      catch (std::exception &err) {
        cerr << err.what() << endl;
      }
    }

    // The internal execution routine which is executed by the thread.  If we
    // return from run, we sleep until we're notified via the global run_cond
    // condition variable.  At that point, we re-run if we're active, sleep
    // again if we're not active, or quit if the global quit flag is set.
    void run_internal()
    {
      while (1) {
        TRDBG1("Thread " << _tid << ":  Starting to run.");

        _node->setActiveCallback(this);

        // Rendezvous with the main thread.
        _pool.wait_on_start();

        // Exit if given the appropriate signal.
        if (_pool._full_stop) return;

        // Otherwise, simulate.
        simulate();
        
        TRDBG1("Thread " << _tid << ":  Done with run.");
        
        // Outside of simulation, we don't want this used.
        _node->setActiveCallback(0);

        // Rendezvous with main thread.
        _pool.wait_on_done();
      }
    }

    ThreadPoolImpl &_pool;
    unsigned        _tid;
    IssNode        *_node;  
    IssNode        *_root;
    thread         *_mythread;
    ::uint64_t      _orig_tdt;
    bool            _exception;
  };


  ThreadPoolImpl::ThreadPoolImpl(unsigned bs,IssNode &root) : 
    _active_count(bs),
    _wait_count(0),
    _quit(false), 
    _brkpt(false),
    _full_stop(false),
    _root(&root),
    _smode(SimNormal),
    _start_barrier(bs+1),  // N+1 to account for master thread.
    _done_barrier(bs+1)
  {
    // Create the thread objects.  Lame non-C++11 means we first create the
    // objects, implying a copy, then start the threads.
    int count = 0;
    ForEach(root.children(),i) {
      _threads.push_back(new SimThread(*i,&root,count++,*this));
    }
      
    // This starts up the threads.
    ForEach(_threads,i) {
      (*i)->run();
    }
  };

  ThreadPoolImpl::~ThreadPoolImpl()
  {
    _full_stop = true;
    
    wait_on_start();

    ForEach(_threads,i) {
      (*i)->join();
    }

    ForEach(_threads,i) {
      delete *i;
    }
  }


  // Thread initialization, done only once.
  ThreadPool *createThreadPool(IssNode &root)
  {
    return new ThreadPoolImpl(root.children().size(),root);
  }

  // Main entry point for multi-threaded simulation.  It runs until some
  // completion event, e.g. breakpoint, td count exceeds threshold, all cores
  // halted, etc.  Basically, just like IssNode::run(), except that all children
  // of root are run as separate threads.  When this function completes, you can
  // call it again to run again, etc.
  ProgramStatus threadedSimRun(IssNode &root,SimType smode,ThreadPool *pool_base)
  {
    ThreadPoolImpl *pool = dynamic_cast<ThreadPoolImpl*>(pool_base);
    assert(pool);

    // Save the td threshold value.
    ::uint64_t orig_tdt = root.get_td_threshold();

    // We will use delayed exceptions, since we process the actual exception in
    // the main thread.
    root.setDelayedExternalExceptions(true);

    pool->reset(root);
    pool->setSimMode(smode);

    // Wait until all threads are ready to do work.
    pool->wait_on_start();

    // Wait until the work is done.  At this point, we've hit a breakpoint or
    // all cores have stopped.
    pool->wait_on_done();

    // No need to delay external exceptions outside of the threaded kernel.
    root.setDelayedExternalExceptions(false);

    // If the threshold was cleared, such as by a breakpoint, then we restore
    // it.
    if (!root.get_td_threshold()) {
      root.set_td_threshold(orig_tdt);
    }

    // We just want to signify if we have a breakpoint.  Otherwise, we don't
    // bother determining whether all cores are still active or not.
    return (pool->_brkpt) ? BREAK : ACTIVE;
  }

}

#else

#include <stdexcept>

#include "helpers/BasicTypes.h"

using namespace std;

namespace adl {

  class IssNode;
  
  void threadedSimRun(IssNode &root,SimType smode)
  {
    throw runtime_error("Multi-threaded simulation not currently supported on Windows.");
  }
  
}

#endif

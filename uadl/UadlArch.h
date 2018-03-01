//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
#ifndef _UADL_ARCH_H
#define _UADL_ARCH_H

#include <list>
#include <vector>
#include <string>
#include <iostream>
#include <stdexcept>

#include "iss/ModelInterface.h"

#include "Timer.h"
#include "LogControl.h"
#include "LogQueue.h"

namespace adl {
  struct IssNode;
  struct MemHandler;
  struct StrPairs;
}

namespace uadl {

  using std::list;
  using std::vector;
  using std::string;

  typedef std::vector<unsigned> FetchSizes;

  class UadlMemoryIf;
  class DynParms;
  struct LogClient;

  // Thrown by a model when an unimplemented instruction is encountered.
  struct unimpl_instr_exception : public std::runtime_error {
    unimpl_instr_exception(const std::string &msg) : std::runtime_error(msg) {};
  };

  // The main base class for a Uadl pipeline model.
  class UadlArch : public Timer, public LogControl {
  public:
    UadlArch(ttime_t startTime,std::ostream &out) :
      Timer(startTime), _issueId(1), _commits(0), _memId(0), _id_offset(0), 
      _resetFetch(true), _resetFetchId(0), _hadReset(true), _out(&out), _logclient(0), _log_queue(1000), 
      _regStalls(0), _flushCount(0), _branchCount(0), _correctPredCount(0),
      _failOnDeadlock(false), _timeOut(1000), _endTime(0), _startTime(0)
    {}

    virtual ~UadlArch() {}

    virtual void reset();

    bool hadReset() const { return _hadReset; };
    // Shouldn't generally need to be used, but if the program counter or other
    // critical registers are changed manually, e.g. via setReg, then call this
    // so that preRun() will re-evaluate necessary data.
    void forceHadReset() { _hadReset = true; _resetFetch = true; _resetFetchId = _issueId; };

    ttime_t endTime() const { return _endTime; };
    void setEndTime(ttime_t t) { _endTime = t; };

    ttime_t startTime() const { return _startTime; };
    void setStartTime(ttime_t t) { _startTime = t; };

    // Access/query methods for the underlying ISS object.
    virtual adl::IssNode& iss() = 0;
    virtual const adl::IssNode& iss() const = 0;

    // True if the node is active.
    virtual bool is_active() const = 0;

    // Must be called before the first call to proceed() in order to synchronize
    // activity.  This is called automatically by run, so you only need to call
    // it if you're calling proceed().
    virtual void preRun() = 0;

    // Advance the model by one cycle.
    virtual void proceed() {};

    // Execute the model until some stopping condition is met, e.g. breakpoint,
    // end-of-simulation, etc.  This updates end-time with the argument value,
    // then proceeds to simulate.
    virtual void run(ttime_t endTime) {};

    // Execute the model until some stopping condition is met, e.g. breakpoint,
    // end-of-simulation, etc.  This uses the current end-time value set via setEndTime.
    virtual void run() {};

    // Called at the end of simulation, to ensure proper shutdown/flusing of any
    // necessary resources.
    virtual void postSim() {};

    // Returns the number of instructions issued.  If a multi-threaded model,
    // this is the sum of all threads' issue counts.
    virtual uint64_t instrCount() const { return _commits; };

    uint64_t issueCount() const { return _issueId; }
    uint64_t commitCount() const { return _commits; }
    void incrCommitCount() { _commits++; }

    unsigned id_offset() const { return _id_offset; };

    bool logWarnings() const { return trace_flag_on(LOG_WARNINGS); };
    bool logFetch() const { return trace_flag_on(LOG_FETCH); }
    bool logPipeline() const { return trace_flag_on(LOG_PIPELINE); }
    bool logMemory() const { return trace_flag_on(LOG_MEMORY); }
    bool logOsm() const { return trace_flag_on(LOG_OSM); }
    bool logOther() const { return trace_flag_on(LOG_OTHER); }

    // Register a dynamic-parameter handler via this call.
    virtual void register_dyn_parms(DynParms &dp);    

    // Get/Set/List dynamic parameters within the model.  Throws a run-time error if a parameter
    // is invalid or the data is invalid.
    virtual void     set_dyn_parm(const std::string &parm,unsigned value);
    virtual unsigned get_dyn_parm(const std::string &parm) const;
    virtual void     list_dyn_parm(adl::StrPairs &parms) const;

    // TODO: better default behavior for a model with no (external) exceptions
    virtual void genExceptions(unsigned exception) {}
    virtual void cancelExceptions(unsigned exception) {}

    // Returns true if the uADL model has external memory.  Note that for an
    // iss-memory-mode model, this will be false, but the ISS object will return
    // true for IssNode::hasExternalMem().
    virtual bool hasExternalMem() const { return false; };

    // Set external memory handler for the ISS and uarch
    virtual void setIssMemHandler(adl::MemHandler*) {}
    virtual void setExtMemory(UadlMemoryIf &mem) {}
    virtual bool hasExtMemSet() const { return false; };

    // For safe-mode models: This allows an external entity to suppress memory
    // logging.  Useful for when an external object wants to write to access
    // memory via the core object.  In that case, you don't want to log the
    // memory access, since it doesn't correspond to an instruction.
    virtual void enableMemLogging(bool e) {};

    // Set the logging stream and client event class.  May be set to 0, which
    // will also turn off logging.
    void setLogStream(std::ostream *out) { _out = out; }
    void setLogClient(LogClient *lc) { _logclient = lc; };

    // Whether we should fail if we detect deadlock.
    bool failOnDeadlock() const { return _failOnDeadlock; };
    void setFailOnDeadlock(bool f) { _failOnDeadlock = f; };

    // TODO: check: timeOut > 0
    void setDeadLockTimeOut(ttime_t timeOut) { _timeOut = timeOut; }

    // Access the program counter.  You must use this routine, rather than going
    // directly to the ISS, in order to ensure that the pipeline model is properly
    // synchronized.
    void setProgramCounter(adl::addr_t);
    adl::addr_t getProgramCounter() const;

    // For internal use only.
    virtual void setBreakpoint() = 0;
    virtual void clearBreakpoint() = 0;
    virtual bool hasBreakpoint() const = 0;

    LogQueue &log_queue() { return _log_queue; };
    const LogQueue &log_queue() const { return _log_queue; };

    void incrRegStalls() { ++_regStalls; };
    void incrBranchCount() { ++_branchCount; };
    void incrCorrectPredCount() { ++_correctPredCount; };

    // We can only change our fetch, e.g. to recover from an error in a
    // safe-mode model, if we haven't already reset our fetch externally,
    // e.g. the user poked in a new PC value.
    bool fetchChangeOK(uint64_t id) { return id > _resetFetchId; };

    // Access to the various stall counters in the system.
    virtual unsigned regStalls() const { return _regStalls; };
    virtual unsigned fetchStalls() const { return 0; };
    virtual unsigned imemStalls() const { return 0; };
    virtual unsigned dmemStalls() const { return 0; };
    virtual unsigned flushCount() const { return _flushCount; };
    // These are only relevant for models with branch predictors.
    virtual unsigned branchCount() const { return _branchCount; };
    virtual unsigned correctPredCount() const { return _correctPredCount; };

    // This displays stall summaries to the specified stream.
    virtual void stallSummary(std::ostream &,const std::string &pfx);
  protected:

    void handleDeadlock();
    std::string getPath(const adl::IssNode *,const char *sep) const;

    uint64_t _issueId;                     // Instruction issue id
    uint64_t _commits;                     // Count of commited instructions.

    unsigned _memId;                       // For memory transactions.
    unsigned _id_offset;                   // Used for MT models to create a
                                           // unique ID for each register in the
                                           // system.

    bool     _resetFetch;                  // True => fetch unit should by synchronized against ISS's pc.
    uint64_t _resetFetchId;                // No instruction with an ID less than this is allowed to reset fetch.
    bool     _hadReset;                    // True => A reset occurred, so do any necessary synchronization before running.

    std::ostream *_out;                    // Pipeline logging output stream.
    LogClient    *_logclient;              // Optional client for logging events.

    LogQueue _log_queue;                   // For tracking resource ownership for logging
                                           // purposes.

    unsigned  _regStalls;                  // Counts stalls caused due to register resource conflicts.
    unsigned  _flushCount;                 // Count number of pipeline flushes.
    unsigned  _branchCount;                // Branch count, if we have a predictor.
    unsigned  _correctPredCount;           // Correct prediction count, if we have a predictor.

    bool      _failOnDeadlock;             // True:  Throw error on deadlock detection, otherwise try to break deadlock.
    unsigned  _timeOut;                    // Cycle count for detecting when a deadlock occurs.

    ttime_t _endTime;                      // Simulation end time.  Set to 0 to return immediately from run().
    ttime_t _startTime;                    // Used to hold off ISS execution
                                           // until the current time exceeds
                                           // this start time, in order to model
                                           // the fast-forwarding of simulation
                                           // time, but still be able to model
                                           // pipeline activity.

  private:

		typedef list<DynParms *> DynParmList;
    DynParmList _dyn_parms;                // Dynamic parameters for
                                           // configuration purposes.

  };

} // namespace uadl

#endif // _UADL_ARCH_H

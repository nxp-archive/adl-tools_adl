//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#include <iostream>
#include <iomanip>
#include <sstream>
#include <stdexcept>

#include <helpers/Macros.h>

#include <iss/ModelInterface.h>

#include "DynParms.h"
#include "UadlArch.h"
#include "UadlTypes.h"
#include "Logger.h"

using namespace adl;
using namespace std;

namespace uadl {

  using adl::InstrPacketBase;

  // UadlArch
  void UadlArch::reset()
  {
    // reset the ISS
    iss().reset();
    _issueId = 1;
    _commits = 0;
    _memId = 0;
    _resetFetch = true;
    _resetFetchId = 0;
    _hadReset = true;
    _log_queue.reset();
    _regStalls = 0;
    _flushCount = 0;
    _branchCount = 0;
    _correctPredCount = 0;
  }

  // This just throws an appropriate error message.  Deadlock-breaking is
  // handled in the generated code.
  void UadlArch::handleDeadlock()
  {
    RError("Detected deadlock at cycle " << currTime() << " after " << _timeOut << " cycles of inactivity.");
  }

  void UadlArch::setProgramCounter(addr_t addr)
  {
    // We tell the fetch unit to reset to the new address, in order to make sure
    // that the new address is taken.
    _resetFetch = true;
    iss().setProgramCounter(addr);
    preRun();
  }

  addr_t UadlArch::getProgramCounter() const
  {
    // For now, just return the ISS value, since the fetch address might be
    // incorrect due to prefetching.
    return iss().getProgramCounter();
  }

  // Register a dynamic-parameter handler via this call.
  void UadlArch::register_dyn_parms(DynParms &dp)
  {
    _dyn_parms.push_back(&dp);
  }

  void UadlArch::set_dyn_parm(const std::string &parm,unsigned value)
  {
		for (DynParmList::iterator i = _dyn_parms.begin(); i != _dyn_parms.end(); ++i) {
      DynParms &dp = **i;

      if (dp.set_dyn_parm(parm,value)) return;
    }

    // If we made it to here, then we haven't found it yet, so send it to the
    // core.  If it doesn't recognize it, then it'll throw an error.
    iss().set_dyn_parm(parm,value);

    // Assume something important may have changed, so force preRun() to
    // re-evaluate.
    _hadReset = true;
  }

  unsigned UadlArch::get_dyn_parm(const std::string &parm) const
  {
    unsigned value;
		for (DynParmList::const_iterator i = _dyn_parms.begin(); i != _dyn_parms.end(); ++i) {
      DynParms &dp = **i;

      if (dp.get_dyn_parm(value,parm)) {
        return value;
      }
    }

    // If we made it to here, then we haven't found it yet, so send it to the
    // core.  If it doesn't recognize it, then it'll throw an error.
    return iss().get_dyn_parm(parm);
  }

  void UadlArch::list_dyn_parm(adl::StrPairs &parms) const
  {
    // Do uadl parms first.
		for (DynParmList::const_iterator i = _dyn_parms.begin(); i != _dyn_parms.end(); ++i) {
      DynParms &dp = **i;
      dp.list_dyn_parm(parms);
    }

    // Then do adl parms.
    iss().list_dyn_parm(parms);
  }   

  string UadlArch::getPath(const IssNode *n,const char *sep) const
  {
    if (n && n->parent()) {
      string p = getPath(n->parent(),sep) + n->name();
      return (p.empty()) ? p : p + sep;
    }
    return string();
  }

  bool InstrBase::check_latency (Logger *logger)
  {
    if (latency() != 0) {
      if (logger) {
        ostringstream ss;
        ss << "Latency counter is " << latency();
        logger->logStall(ss.str());
      }
      return false;
    }
    return true;
  }

  void UadlArch::stallSummary(std::ostream &o,const string &pfx)
  {
    o << pfx << "Register Stalls            :  " << regStalls() << '\n'
      << pfx << "Fetch Stalls               :  " << fetchStalls() << '\n'
      << pfx << "Instruction Memory Stalls  :  " << imemStalls() << '\n'
      << pfx << "Data Memory Stalls         :  " << dmemStalls() << '\n'
      << pfx << "Flush Count                :  " << flushCount() << '\n';
    if (branchCount()) {
      o << pfx << "Branch Prediction Correct %:  " << setprecision(2) << (((double)correctPredCount())/((double)branchCount()))*100 
        << " (" << correctPredCount() << "/" << branchCount() << ")\n";
    }
  }

} // namespace uadl

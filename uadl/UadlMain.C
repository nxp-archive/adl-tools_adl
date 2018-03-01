//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

// TODO: Handle multiple tests in one run, like an ADL model does.

#include <stdlib.h>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <limits>

#include "helpers/AnyOption.h"
#include "helpers/Macros.h"
#include "helpers/FilenameUtils.h"

#include "iss/SimInterface.h"
#include "iss/SimInterface.h"
#include "iss/ModelInterface.h"
#include "iss/ExecSupport.h"

#include "UadlRunControl.h"
#include "UadlArch.h"
#include "UadlMemoryIf.h"
#include "UadlArchIf.h"
#include "UadlPlugin.h"
#include "UadlTrace.h"

using namespace std;
using namespace adl;
using namespace uadl;


static void finalize_sim(UadlArch &arch,uint64_t elapsedtime)
{
  if (!QuietMode) {
    cerr.setf(ios_base::fixed);
    double usecs = elapsedtime;
    double secs = usecs/1000000;

    uint64_t instr_count = arch.instrCount();
    uint64_t cycles      = arch.currTime();

    // TODO: print real, per-test instruction exec count, not issue count
    cerr << dec 
         << "\nSimulation finished at time " << cycles << "."
         << "\n"
         << "\nInstructions executed:  " << dec << instr_count
         << "\nWall-clock time (s)  :  " << secs
         << "\nInstructions/sec     :  " << get_speed(instr_count,secs)
         << "\nCycles/sec           :  " << get_speed(cycles,secs)
         << "\n"
         << "\n"
         << "Performance Summary:\n";
    arch.stallSummary(cerr,"   ");
    cerr << "\n";
  }
}


static UadlRunControl runctrl;


struct UadlExtMemHelper : public ExtMemHelper {

  UadlExtMemHelper(UadlArch &arch) : _arch(arch) {};

  void operator()(IssNode &root,const PluginList &plugins) const
  {
    if (_arch.hasExternalMem()) {
      // Iterate over all plugins, seeing if there's a memory handler that can
      // be installed.
      ForEach (plugins,piter) {
        if (UadlPlugin *up = dynamic_cast<UadlPlugin*>(*piter)) {
          up->createUadlMemHandler(_arch);
        }
      }

      if (!_arch.hasExtMemSet()) {
        RError("Model is configured to use an external memory, but none were installed via plugins.");
      }
      return;
    }

    // Then call the parent class, which may install a plugin if we're dealing
    // with an iss-mem-mode model.
    ExtMemHelper::operator()(root,plugins);
  }

  UadlArch &_arch;
};

int main(int argc, const char *argv[]) ATTRIBUTE_WEAK;

int main(int argc, const char *argv[])
{
  init_speed_display();
  
  try {
    // uADL-specific options.
    setupTraceOptions(options());

    options().setFlag("fail-on-deadlock", "fod","Fail the simulation if a deadlock is detected.");
    options().setOption("start-time",     "s",  "Specify a start time.");
    options().setOption("c",                    "Specify simulation cycles.");
    options().setOption("time-out",             "Specify time-out cycle count for safe mode.");

    options().useCommandArgs(argc, argv);

    // Allow unknown options for now- they'll be checked again in init().
    options().errorOnUnknowns(false);

    // Parse command line options and arguments for uADL-specific options.
    // Further processing will be done by init().
    if (!options().processCommandLine()) {
      RError("Bad command-line.");
    }

    const bool failOnDeadlock = options().getFlag("fail-on-deadlock",false);

    uint32_t trace_flags = getTraceFlags(options());
    
    // Get start/end times.  Start time is just for annotation purposes, to make
    // it easier to be compatible with another system, for example.  This is
    // different from the start-time found in UadlArch, which is used for
    // modeling the fast-forwarding of a model in time.  For this driver, we
    // don't do anything with that start-time value.  End-time specifies a
    // simulation limit.  If not specified, then we pick the max value so that
    // the simulation won't time-out.
    const ttime_t startTime = options().getOption("start-time", 0);
    const ttime_t execTime  = options().getOption("c", 0);
    const ttime_t endTime = (execTime == 0) ? numeric_limits<ttime_t>::max() : (startTime + execTime);

    // Create the microarchitecture
    // TODO: use a more meaningful name instead of "core"
    unsigned id = 0;
    UadlArch &arch = *createArch("core", id, NULL, startTime);

    // We use a diffrent run-control interface for uADL with the scripting
    // engine.
    runctrl.setArch(&arch);
    setRunControl(&runctrl);

    arch.set_tracing(trace_flags);
    arch.setFailOnDeadlock(failOnDeadlock);

    if (options().haveOption("time-out")) {
      arch.setDeadLockTimeOut(options().getOption("time-out", 0));
    }

    // ADL initialization (input, output, etc.).  Don't do external memories
    // here, b/c we need uADL memories, if the uADL model is set up for external
    // memories.  For ISS-memory-mode, the uADL model is considered to not have
    // an external memory, so we just load the standard ISS memory.
    UadlExtMemHelper mem_helper(arch);
    if (!init(&arch.iss(),argc,argv,0,true,mem_helper)) {
      return 0;
    }

    // Run the model or debugger, depending upon user options.
    if (DbgInterface *dbg = createDebugger(arch.iss())) {
      runctrl.setup(endTime,trace_flags);
      // If we have any command-line tracing options, then restore the output
      // stream, which is cleared by the run-control object by default.
      if (trace_flags) arch.setLogStream(&cout);
      dbg->exec();
      QPrint("Done.");
    }
    else {
      while (true) {
        const uint64_t starttime = getusec();
        arch.run(endTime);
        const uint64_t stoptime = getusec();
        arch.postSim();

        // Take-down of ADL stuff (output, etc.).
        handleFinalState(true, arch.iss());
        finalize_sim(arch, stoptime - starttime);    

        if (!adl::haveMoreInput()) break;

        // reset simulation
        resetSimulation(arch.iss());
        arch.reset();

        if (!adl::readNextInput(arch.iss())) break;
      }
    }

    adl::finalize();
  }
  catch (exception &err) {
    cerr << "\nError:  " << err.what() << "\n\n";

    return 1;
  }

  return 0;
}

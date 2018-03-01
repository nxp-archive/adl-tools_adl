//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#include <iostream>
#include <stdexcept>
#include <fstream>

#include "helpers/LogUsage.h"
#include "helpers/Macros.h"

#include "ExecSupport.h"
#include "SimInterface.h"
#include "ModelInterface.h"
#include "DbgInterface.h"

using namespace std;

namespace adl {

  // Does final reporting.
  static void finalize_sim(const IssNode *root,uint64_t elapsedtime)
  {
    if (!QuietMode) {
      cerr.setf(ios_base::fixed);
      double usecs = elapsedtime;
      double secs = usecs/1000000;

      uint64_t instr_count = root->instr_count();

      bool is_time_tagged  = root->isTimeTagged();
      ttime_t  final_time  = root->finalTime();

      cerr << "\nInstructions executed:  " << dec << instr_count;
      if (is_time_tagged) {
        cerr << "\nSimulation time      :  " << dec << final_time;
      }
      cerr << "\nWall-clock time (s)  :  " << secs
           << "\nInstructions/sec     :  " << get_speed(instr_count,secs)
           << "\n";
    }
  }
}

int main(int argc,const char *argv[]) ATTRIBUTE_WEAK ;

// Main entry point:  Generic to all models.
// The "weak" attribute is applied so that users may override and
// have their own entry point.
int main(int argc,const char *argv[])
{
  using namespace adl;

  init_speed_display();

  try {

    cerr << adliss::getModelPreamble() << '\n';

    adliss::resetGlobal();

    IssNode *root = 0;
    if (!(root = init(argc,argv,adliss::createTopLevelNode))) {
      return 0;
    }

    SimType smode = simMode();

    QPrint("Finished initialization.");
        
    if (DbgInterface *dbg = createDebugger(*root)) {
      dbg->exec();
      handleFinalState(true,*root);
      cerr<<"Done." << endl;
    } 
    else {
      while (1) {
        uint64_t starttime = getusec();
        runSim(*root,smode);
        uint64_t stoptime = getusec();
        QPrint("Finished simulation.");
        finalize_sim(root,stoptime-starttime);
        handleFinalState(true,*root);
        if (!haveMoreInput()) {
          break;
        }
        resetSimulation(*root);
        if (!readNextInput(*root)) {
          break;
        }
      }
    }

    // Perform any finalization steps.
    // Note:  This should be the *only* code that's executed after an alternate execution
    //        loop because it may be the case that the loop does not return normally. In such
    //        a case, the alternate loop will invoke an exit handler which calls finalize().
    finalize();
    delete root;
  }
  catch (exception &err) {
    cerr << "\nError:  " << err.what() << "\n\n";
    return 1;
  }
  return 0;
}

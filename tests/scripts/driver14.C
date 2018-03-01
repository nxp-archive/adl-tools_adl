//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Simple driver program to test that we can load an ADL ISS as a shared object
// and use the --extern-event-bus mechanism to propagate event bus data.
//

#include <string.h>
#include <assert.h>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "rnumber/RNumber.h"

#include "helpers/Macros.h"

#include "iss/SimInterface.h"
#include "iss/ModelInterface.h"
#include "iss/DbgInterface.h"
#include "iss/EventBusExtHandler.h"

using namespace std;
using namespace adl;

struct MyEventBusHandler : public EventBusExtHandler {

  // Called when event buses are registered.
  virtual void register_eventbus(int &handle,int &id,const std::string &name,const std::string &sig,EventBusHandler &handler)
  {
    pair<IndexMap::iterator,bool> ip = _busMap.insert(make_pair(name,_eventBuses.size()));

    int index;
    if (ip.second) {
      // New bus- add the item,
      index = _eventBuses.size();
      _eventBuses.push_back(EventBus(name,sig));
    } else {
      // Bus already exists.  Verify that the signatures match.
      index = ip.first->second;
      EventBus &eb = _eventBuses[index];
      if (sig != eb._sig) {
        RError("Signature mismatch when installing to bus " << name);
      }
    }

    EventBus &eb = _eventBuses[index];
    eb._handlers.push_back(&handler);
    handle = index;
    id = eb._count++;
  }

  // Called on an event bus broadcast operation.  We then broadcast this to all
  // other nodes.
  virtual void signal_eventbus(int handle,int id,const void *data,int len)
  {
    EventBus &eb = _eventBuses[handle];

    for(HandlerList::iterator i = eb._handlers.begin(); i != eb._handlers.end(); ++i) {
      (**i)(data,id);
    }    
  }

  EventBuses _eventBuses; // For storing registed event buses in this core.
  IndexMap   _busMap;     // Maps names to bus IDs.
};

int main(int argc,const char *argv[])
{
  if (argc < 3) {
    cout << "usage:  " << argv[0] << ": [-script <file-name> [<pre-exec-func>] ] | [ [-jit] [-dc] [-td <incr> <threshold>] [-initial-exception <excpt>] <input file> <output file , [output file ...] >]" << endl;
    return 1;
  }

  string script, pre_exec_func, initial_exception;
  if (strstr(argv[1],"-script")) {
    script = argv[2];
    if (argc > 3) {
      pre_exec_func = argv[3];
    }
  }

  adliss::resetGlobal();
  unsigned id = 0;
  IssNode *root = adliss::createTopLevelNode(id);
  assert(root);

  MyEventBusHandler ebh;
  root->setEventBusHandler(&ebh);

  installLoggers(root);

  TranslationParms::set_jit_bb_hot_threshold(10);
  TranslationParms::set_dc_bb_hot_threshold(10);

  if (!script.empty()) {
    DbgInterface *dbg = startCliMode(*root,script);
    if (!pre_exec_func.empty()) {
      // Test that we can evaluate a command directly with the CLI.
      evalCliCommand(pre_exec_func);
    }
     dbg->exec();
  } else {
    SimType rt = SimNormal;
    int astart;
    bool td = false;
    for (astart = 1; astart != argc; ++astart) {
      if ( strstr(argv[astart],"-initial-exception")) {
        if (astart+1 >= argc) {
          cout << "An exception name must be specified for the initial-exception option.\n";
          return 1;
        }
        initial_exception = argv[++astart];
        ++astart;
      }
      if ( strstr(argv[astart],"-jit")) {
        rt = SimJit;
      }
      else if ( strstr(argv[astart],"-dc")) {
        rt = SimDc;
      }
      else if ( strstr(argv[astart],"-td")) {
        if (astart+2 >= argc) {
          cout << "An increment and threshold value must be supplied with option -td.\n";
          return 1;
        }
        td = true;
        root->set_td_increment(atoi(argv[++astart]));
        root->set_td_threshold(strtoull(argv[++astart],0,0));
      } else {
        break;
      }
    }
    readInput(root,argv[astart++]);

    // Note:  We're specifically creating it after the reader to test that this will work.
    for (int i = astart; i != argc; ++i) {
      addWriter(createWriter(*root,argv[i]));
    }

    startLogging(*root);

    if (td) {
      int td_count = 0;
      // Report on returns caused due to temporal-decoupling time quantums.
      while (root->run(rt) == ACTIVE) {
        cout << "Temporal decoupling break:  " << ++td_count << "\n";
        root->set_td_counter(0);
      }
    } else {
      root->run(rt);
    }
    handleFinalState(true,*root);
    removeAllWriters();
    cout << "Instructions executed:  " << root->instr_count() << "\n\n";
  }

}

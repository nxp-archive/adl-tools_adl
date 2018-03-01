//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// The header file for functions used by standalone executables. 
//

#ifndef _EXEC_SUPPORT_H_
#define _EXEC_SUPPORT_H_

#include "helpers/BasicTypes.h"

namespace adl {

  struct IssNode;
  struct ScriptingControlIface;
  class DbgInterface;

  void displayPreamble(const IssNode &);

  DbgInterface *createDebugger(IssNode &root);

  void handleInitialState(bool dumpstate,IssNode &root);
  void handleFinalState(bool dumpstate,IssNode &root);

  bool haveMoreInput();

  bool readNextInput(IssNode &root);
 
  // Set a default run-control object.
  void setRunControl(ScriptingControlIface *);
 
  // Get the currrent run-control object.
  ScriptingControlIface *getRunControl();

  // Any necessary init stuff for getting speed statistics to print correctly.  Call
  // this before any other I/O.
  void init_speed_display();

  // Returns a string representing the instructions/sec value.
  std::string get_speed(uint64_t instr_count,double secs);

  // Returns the current time in microseconds.
  uint64_t getusec();

}

#endif

//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// A simple plugin to demonstrate a register callback which causes a branch to
// occur.
//

#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "rnumber/RNumber.h"

#include "helpers/Macros.h"
#include "helpers/BasicTypes.h"

#include "iss/ModelInterface.h"

#include "iss/Plugin.h"

using namespace std;
using namespace adl;

struct BranchPlugin : public Plugin {

  struct MyCb : public RegCallback, public RegReadCallback {
    MyCb() : _count(0) {};

    // Changes the program counter, effectively acting as a branch.
    virtual void operator()(unsigned index,REGTYPE value)
    {
      addr_t a = value.uint64();

      _n->setProgramCounter(a);
      ++_count;
    }

    // Returns the number of uses.
    virtual REGTYPE_t operator()(unsigned)
    {
      return _count;
    }
    
    IssNode *_n;
    unsigned _count;
  };

  struct MyExitCb : public RegReadCallback {

    // Causes an exit from the simulation kernel.
    virtual REGTYPE_t operator()(unsigned)
    {
      _n->set_td_threshold(0);
      return 100;
    }

   
    IssNode *_n;
  };

  struct MyRegfileCb : public RegCallback, public RegReadCallback {

    void setSize(unsigned n)
    {
      _d.resize(n);
    }

    virtual void operator()(unsigned index,REGTYPE value)
    {
      _d[index % _d.size()] = value.uint32();
    }

    // Returns the number of uses.
    virtual REGTYPE_t operator()(unsigned index)
    {
      return _d[index % _d.size()] * 10;
    }
    
    vector<unsigned> _d;
  };

  virtual void init(AnyOption &options) 
  {
    cout << "Loaded the branch-callback plugin.\n";
  };  

  // Called after final options processing.
  virtual void setup(AnyOption &options,IssNode &root)
  {
    if (!root.setRegCallback("BRREG",&_cb)) {
      RError("Could not install register callback.");
    }
    if (!root.setRegReadCallback("BRSTAT",&_cb)) {
      RError("Could not install register read-callback.");
    }
    _cb._n = &root;

    if (!root.setRegCallback("BRFSTAT",&_rfcb)) {
      RError("Could not install register callback.");
    }
    if (!root.setRegReadCallback("BRFSTAT",&_rfcb)) {
      RError("Could not install register read-callback.");
    }
    _rfcb.setSize(2);

    if (!root.setRegReadCallback("TDEXIT",&_ecb)) {
      RError("Could not install register read-callback.");
    }
    _ecb._n = &root;
  }

  MyCb        _cb;;
  MyRegfileCb _rfcb;
  MyExitCb    _ecb;
};

static BranchPlugin branch_plugin;

// Main entry point- returns a pointer to our service-provider object.
extern "C" Plugin *adl_plugin_entry()
{
  return &branch_plugin;
}

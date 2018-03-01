//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// A simple plugin to generate external exceptions via a register callback.
//

#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <memory>

#include "rnumber/RNumber.h"

#include "helpers/BasicTypes.h"

#include "iss/ModelInterface.h"

#include "iss/Plugin.h"

#define ForEach(container,iter) \
       for (auto iter = container.begin(), end = container.end(); iter != end; ++iter)

#define RError(x) { std::ostringstream ss; ss << x; throw std::runtime_error(ss.str()); }

using namespace std;
using namespace adl;

unsigned getExceptionHandle(const IssNode &node,const string &name)
{
  if (node.is_core()) {
    auto &info = node.getExceptionInfo();
    ForEach(info,i) {
      if (i->_name == name) {
        return i->_flag;
      }
    }
    RError("Could not find information about exception " << name );
  } else {
    return getExceptionHandle(*node.children().front(),name);
  }
}

struct ExceptionPlugin : public Plugin {

  struct MyCb : public RegCallback {
    MyCb(IssNode &node,const IssChildren &nodes,unsigned ehandle) : 
      _nodes(&nodes), 
      _ehandle(ehandle) 
    {
      if (!node.setRegCallback("EXREG",this)) {
        RError("Could not install register callback.");
      }
    };

    // Changes the program counter, effectively acting as a branch.
    virtual void operator()(unsigned index,REGTYPE value)
    {
      uint32_t c = value.uint32();

      if (c < _nodes->size()) {
        IssNode *n = (*_nodes)[c];

        cout << "Triggering exception on core " << c << "\n";
        n->genExceptions(_ehandle);
      }
    }
    
    const IssChildren *_nodes;
    unsigned           _ehandle;
  };

  virtual void init(AnyOption &options) 
  {
    cout << "Loaded the exception callback plugin.\n";
  };  

  // Called after final options processing.
  virtual void setup(AnyOption &options,IssNode &root)
  {
    int eh = getExceptionHandle(root,"External");

    if (root.is_core()) {
      RError("This plugin is designed to work with a system of cores.");
    }

    ForEach(root.children(),i) {
      _cbs.push_back(unique_ptr<MyCb>(new MyCb(**i,root.children(),eh)));
    }
  }

  vector<unique_ptr<MyCb>> _cbs;

};

static ExceptionPlugin exception_plugin;

// Main entry point- returns a pointer to our service-provider object.
extern "C" Plugin *adl_plugin_entry()
{
  return &exception_plugin;
}

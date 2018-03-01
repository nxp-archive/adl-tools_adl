//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef _DYNTEST_WRAPPER_H_
#define _DYNTEST_WRAPPER_H_

#include <iostream>
#include <sstream>
#include <stdexcept>

#include "helpers/Macros.h"
#include "helpers/BasicTypes.h"

#define ADL_HEADER
#include "iss/DefaultMem.h"

#include "uadl/UadlMemory.h"
#include "uadl/UadlArchIf.h"

// The purpose of this class is to wrap a uADL model, with initialization, into
// a single class which can be accessed through a plug-in interface.
struct DynWrapper {

  virtual ~DynWrapper() {};

  virtual uadl::UadlArch *createArch(const std::string &name,
                                     unsigned &id,
                                     uadl::UadlMemoryIf *memory = 0,
                                     adl::ttime_t startTime = 0) = 0;

  virtual adl::IssNode *init(adl::IssNode *root,int argc,const char *argv[]) = 0;

  virtual void handleFinalState(bool dumpstate,adl::IssNode &root) = 0;    

  virtual void finalize() = 0;

  virtual adl::GlobalMemoryType &get_memory() = 0;

};

typedef DynWrapper &(*MakeWrapper)();

extern "C" {

  DynWrapper &make_wrapper();

}

#endif

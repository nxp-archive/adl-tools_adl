//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#include "iss/SimInterface.h"

#include "dyntest_wrapper.h"

// Note: We're dynamically allocating the global memory because when it's
// declared as a global, we get a glibc heap corruption error upon cleanup.  I
// don't know why that's the case- looks to be some issue with how the dynamic
// linker cleans things up.  Allocating the memory object on the heap, however,
// seems to solve the problem.
static adl::GlobalMemoryType *my_global_mem = 0;

using namespace adl;
using namespace uadl;

struct DynWrapperImpl : public DynWrapper {

  UadlArch *createArch(const string &name,
                       unsigned &id,
                       UadlMemoryIf *memory = 0,
                       ttime_t startTime = 0)
  {
    return uadl::createArch("core", id, memory, startTime);
  }

  IssNode *init(IssNode *root,int argc,const char *argv[])
  {
    return adl::init(root,argc,argv,0,false);
  }

  void handleFinalState(bool dumpstate,adl::IssNode &root)
  {
    adl::handleFinalState(dumpstate,root);
  }

  void finalize()
  {
    adl::finalize();
  }

  adl::GlobalMemoryType &get_memory()
  {
    if (!my_global_mem) {
      my_global_mem = new adl::GlobalMemoryType;
    }
    return *my_global_mem;
  }

};

static DynWrapperImpl wrapper;

extern "C" DynWrapper &make_wrapper()
{
  return wrapper;
}


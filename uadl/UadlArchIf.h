//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This file is not enclosed by ifdef so that it can be included multiple times,
// with each inclusion in a (potentially) different namespace.
//
// If you are using the built-in SystemC support, then define UADL_SYSTEMC so
// that the return type will reflect this.
//

#include <string>
#include "Timer.h"
#include "UadlMemoryIf.h"

#ifdef UADL_SYSTEMC
#  include "UadlArchSystemC.h"
#  define ReturnType uadl::UadlArchSystemC
#else
#  include "UadlArch.h"
#  define ReturnType uadl::UadlArch
#endif

#define DEFAULT_UADL_MODEL_NAMESPACE uadl

#ifndef UADL_MODEL_NAMESPACE
#define UADL_MODEL_NAMESPACE DEFAULT_UADL_MODEL_NAMESPACE 
#endif

namespace UADL_MODEL_NAMESPACE {

  typedef ReturnType *(*CreateArch)(const std::string &name,
                                    unsigned &id,
                                    uadl::UadlMemoryIf *memory,
                                    adl::ttime_t startTime);
  
  extern ReturnType *createArch(const std::string &name,
                                unsigned &id,
                                uadl::UadlMemoryIf *memory = 0,
                                adl::ttime_t startTime = 0);
  
} // namespace UADL_MODEL

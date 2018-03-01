//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Some basic tracing flag setup for uADL models.
//

#ifndef _UADL_TRACE_H_
#define _UADL_TRACE_H_

#include <string>

#include "helpers/BasicTypes.h"

namespace adl {

  class AnyOption;

}

namespace uadl {

  // The boolean return is ignored, but allows this to be used in a
  // static-variable initialization chain.
  bool setupTraceOptions(adl::AnyOption &);

  // Returns a bitmask of relevant tracing flags set.
  uint32_t getTraceFlags(adl::AnyOption &);

}

#endif

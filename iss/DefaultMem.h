//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

  // This file contains the defualt internal Mem implementation
#ifndef _DEFAULT_MEM_H_
#define _DEFAULT_MEM_H_

#include "Memory.h"
#include "GlobalMemoryName.h"

namespace adl {

  // Use the defines MEM_BIT_WIDTH and MEM_LOOKUP_LEVELS to define the memory
  // characteristics.

  // Number of bits that we care about.  The address type is defined in ModuleInterface.h.
#ifndef MEM_BIT_WIDTH
# define MEM_BIT_WIDTH 64
#endif

  // Number of lookup levels.  For example, 32/2 means that we care about 32 bits of the
  // address and have 2 levels of lookup- one tree node and then the lookup within the
  // page itself.  A level of 1 thus implies a single array of bytes.
#ifndef MEM_LOOKUP_LEVELS
# define MEM_LOOKUP_LEVELS 4
#endif


  // The global memory object.  This ends up being defined only in the ISS, since
  // this file is only included by the ISS.  Use the mem_read and mem_write
  // routines to access this object from other translation units, since those functions
  // are not templated. 
  //
  // If UseBlockMemory is defined, then we use a simple, contiguous, block-based
  // memory with a run-time selectable size.  Otherwise, we use a sparse memory
  // which provides support for a full 64-bit address.
  //
# ifdef UseBlockMemory
  typedef BlockMemory GlobalMemoryType;
# else
  typedef SparseMemory<MEM_BIT_WIDTH,MEM_LOOKUP_LEVELS> GlobalMemoryType;
# endif

# ifdef ADL_HEADER
  extern GlobalMemoryType GlobalMemory;
# else
  GlobalMemoryType GlobalMemory;
# endif

# ifndef ADL_GLOBAL_MEMORY
#   define ADL_GLOBAL_MEMORY
# endif

}

#include "GlobalMemSupport.h"

#endif

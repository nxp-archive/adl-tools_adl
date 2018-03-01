//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
//
// Interface for loading of dynamic objects.
//

#ifndef _DYNLOAD_H_

#if !defined(_MSC_VER)
// At some point, we'll define this on Windows, since we're using libltdl and it should
// just work.
#  define USE_DLOADER
#endif

#ifdef USE_DLOADER
# include "ltdl.h"
#else
typedef int lt_dlhandle;
#endif

namespace adl {

  // Given a handle and a name, returns a pointer to a symbol.  Throws a runtime_error
  // if not found.
  void *lookupSymbol(lt_dlhandle handle, const char* symbol);

  // Loads Loads the specified library.  Throws a runtime_error if there is a problem.
  lt_dlhandle loadLib(const char* file_name);

  // Initializes dynamic loader module.
  void initDynLoader();

  // Shuts down dynamic loader module.
  void exitDynLoader();

}

#endif

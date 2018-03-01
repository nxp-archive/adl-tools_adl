//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
//
// Interface for loading of dynamic objects.
//

#include <string>
#include <sstream>
#include <stdexcept>

#include "helpers/Macros.h"

#include "DynLoad.h"

using namespace std;

namespace adl {

  // Given a handle and a name, returns a pointer to a symbol.  Throws a runtime_error
  // if not found.
  void *lookupSymbol(lt_dlhandle handle, const char* symbol)
  {
    void* func = 0;

#   ifdef USE_DLOADER
    func = lt_dlsym(handle, symbol);
    if (!func) {
      RError(string("lt_dlsym() failed: ") + lt_dlerror() + "\n");
    }
    return func;
#   else
	RError("lookupSymbol:  Dynamic loading not supported.");
#   endif
  }

  // Loads Loads the specified library.  Throws a runtime_error if there is a problem.
  lt_dlhandle loadLib(const char* file_name)
  {
#   ifdef USE_DLOADER
    lt_dlhandle handle = 0;

    handle = lt_dlopen(file_name);

    if(handle == 0) {
      RError(string("lt_dlopen(")+file_name+") failed:  "+lt_dlerror()+"\n");
    }

    return handle;    
#   else
	  RError("loadLib:  Dynamic loading not supported.");
#   endif
  }

  // Initializes dynamic loader module.
  void initDynLoader()
  {
#   ifdef USE_DLOADER
    if (lt_dlinit() == 0) {
      lt_dladdsearchdir(".");
    } else {
      RError("Cannnot initialize dynamic loader.");
    }
#   endif
  }

  // Shuts down dynamic loader module.
  void exitDynLoader()
  { 
#   ifdef USE_DLOADER   
    lt_dlexit();
#   endif
  }

}

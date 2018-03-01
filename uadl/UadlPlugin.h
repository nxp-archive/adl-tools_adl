//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This extends the basic plugin to support uADL models.  This is necessary to
// install a uADL memory with timing from a plugin.
//

#ifndef _UADLPLUGIN_H_
#define _UADLPLUGIN_H_

#include "iss/Plugin.h"

namespace uadl {
  
  class UadlArch;

  struct UadlPlugin : public adl::Plugin {

    // If this plugin defines an external memory handler, then create such an
    // object.  This method should call setExtMemory and setIssMemHandler.
    virtual void createUadlMemHandler(UadlArch &) {};

  };

}

#endif

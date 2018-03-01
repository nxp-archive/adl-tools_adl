//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This is the debugger register map class, which is used by the debugger interface
// to map debugger register indices to ADL model register indices.  This is a singleton
// object; a reference to the singleton may be obtained by calling dbgRegMap();
//

#include <sstream>
#include <stdexcept>

#include "helpers/Macros.h"

#include "DbgRegMap.h"
#include "NodeHelpers.h"

using namespace std;

namespace adl {

  // Look up a mapping.  Throws a runtime error if not found.
  const DbgRegInfos &DbgRegMap::find(const IssNode &node) const
  {
    auto iter = find(&node);
    if (iter == end()) {
      RError("No register mapping for " << getPathName(&node,":") << ".");
    } else {
      return iter->second;
    }
  }
  

  static DbgRegMap theInstance;

  DbgRegMap &dbgRegMap()
  {
    return theInstance;
  }

}




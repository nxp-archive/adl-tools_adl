//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#include "helpers/Macros.h"

#include "LogQueue.h"

using namespace std;

namespace uadl {
  
  void LogQueue::reset()
  {
	for (iterator i = begin(); i != end(); ++i) {
      delete *i;
    }
    clear();
  }

}

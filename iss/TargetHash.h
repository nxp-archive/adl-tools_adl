//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Support for hashing Target objects.
//

#ifndef _TARGET_HASH_H_
#define _TARGET_HASH_H_

#include "helpers/stringhash.h"

#include "NodeHelpers.h"

namespace std {
  namespace tr1 {

    template<> struct hash<adl::Target>
    {
      hash<adl::IssNode*> p_h;
      hash<int>           i_h;
      
      size_t operator()(adl::Target x) const { 
        return p_h(x.node()) ^ i_h(x.ctx_id()) ^ i_h(x.ctx_num());
      }
    };

  }
}

#endif

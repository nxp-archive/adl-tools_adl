//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Contains routines needed for intbv/rnumber interoperability.  This is put into
// a separate library so that we can make RNumber an optional requirement.
//

#include "intbv.h"

using namespace std;

namespace adl {

  // Returns an RNumber of the appropriate size.
  rnumber::RNumber createRNumber(int size,const uint32_t* x)
  {
    return rnumber::RNumber(x,_INTBV_WORDS(size),size,rnumber::RNumber::fixed);
  }

  void constructFromRNumber(uint32_t* v,size_t nw,const rnumber::RNumber &x)
  {
    size_t rn_nw = x.wordCount();
    if (nw > rn_nw) {
      size_t diff = nw - rn_nw;
      for (size_t i = 0; i != diff; ++i) {
        v[i] = 0;
      }
      for (int i = rn_nw-1; i >=0; i--)
        v[i+diff] =  x.buffer()[i]; 
    } else {
      size_t diff = rn_nw - nw;
      for (int i = nw-1; i >=0; i--) {   
        v[i] = x.buffer()[i+diff];
      }
    }
  }

}

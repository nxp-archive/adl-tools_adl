//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Hash support for C++ strings.
//

#ifndef _STRING_HASH_H_
#define _STRING_HASH_H_

#include <string.h>

#ifdef _MSC_VER

#include <hash_map>
#include <hash_set>

#else

#include <tr1/unordered_map>

namespace adl {

  struct CmpConstChar {
    bool operator()(const char *x,const char *y) const {
      return strcmp(x,y) == 0;
    }
  };

  struct HashConstChar {
    bool operator()(const char *__s) const {
      unsigned long __h = 0;
      for ( ; *__s; ++__s)
        __h = 5 * __h + *__s;
      return size_t(__h);   
    }
  };

}

#endif

#endif

//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// I/O support for miscellaneous types.
//

#ifndef _TYPE_IO_H_
#define _TYPE_IO_H_

#include <iostream>

#include "helpers/BasicTypes.h"

namespace adl {

  inline std::ostream &operator <<(std::ostream &o,TransType t)
  {
    switch (t) {
    case InstrTrans:
      o << "instr";
      break;
    case LoadTrans:
      o << "load";
      break;
    case StoreTrans:
      o << "store";
      break;
    case WriteTrans:
      o << "write";
      break;
    }    
    return o;
  }

  inline std::ostream &operator <<(std::ostream &o,CacheAccess c)
  {
    switch (c) {
    case CacheNoAccess:
      o << "none";
      break;
    case CacheIFetch:
      o << "ifetch";
      break;
    case CacheIFetchTrans:
      o << "ifetchtrans";
      break;
    case CacheRead:
      o << "read";
      break;
    case CacheStore:
    case CacheWrite:
      o << "write";
      break;
    case CacheFlush:
      o << "flush";
      break;
    case CacheTouch:
      o << "touch";
      break;
    case CacheAlloc:
      o << "alloc";
      break;
    case CacheInvalidate:
      o << "invalidate";
      break;
    case CacheLock:
      o << "lock";
      break;
    case CacheLockAddr:
      o << "lock-addr";
      break;
    case CacheUnlock:
      o << "unlock";
      break;
    case CacheLogRead:
      o << "logread";
      break;
    case CacheILogRead:
      o << "instrlogread";
      break;
    default: 
      o << std::dec << (unsigned)c;
      break;
    }
    return o;
  }

  inline std::ostream &operator <<(std::ostream &o,CacheAction c)
  {
    switch (c) {
    case CacheLoad:
      o << "load";
      break;
    case CacheHit:
      o << "hit";
      break;
    case CacheMiss:
      o << "miss";
      break;
    case CacheEvict:
      o << "evict";
      break;
    case CacheNone:
      o << "none";
      break;
    }
    return o;
  }

  inline std::ostream &operator <<(std::ostream &o,WatchType w)
  {
    switch (w) {
    case wpNone:
      o << "none";
      break;
    case wpRead:
      o << "read";
      break;
    case wpWrite:
      o << "write";
      break;
    case wpAccess:
      o << "access";
      break;
    }
    return o;
  }

}

#endif


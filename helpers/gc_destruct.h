//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// A base class for objects which need to be cleaned up, but have simple
// destructors, so they can be immediately finalized, without worrying about
// ordering.
//

#ifndef _GC_DESTRUCT_H_
#define _GC_DESTRUCT_H_

#include "gc/gc_cpp.h"

class gc_destruct: virtual public gc {
 public:
  inline gc_destruct();
  inline virtual ~gc_destruct();
 private:
  inline static void GC_cdecl cleanup( void* obj, void* clientData );
};

// Instances of classes derived from "gc_destruct" will be allocated in the
// collected heap by default.  When the collector discovers an inaccessible
// object derived from "gc_destruct" or containing a member derived from
// "gc_destruct", its destructors will be invoked.  The reason to use
// gc_destruct rather than gc_cleanup is that this will ignore interior pointers
// and immediately destroy the object.  Only use this for simple objects which
// mainly just need to reclaim memory or resources used by member variables and
// where you don't have to worry about complex cycles and such.
inline gc_destruct::~gc_destruct() {
  GC_REGISTER_FINALIZER_IGNORE_SELF(GC_base(this), 0, 0, 0, 0 );
}

inline void gc_destruct::cleanup( void* obj, void* displ ) {
  ((gc_destruct*) ((char*) obj + (ptrdiff_t) displ))->~gc_destruct();
}

inline gc_destruct::gc_destruct() {
  GC_finalization_proc oldProc;
  void* oldData;
  void* base = GC_base( (void *) this );
  if (0 != base)  {
    // Don't call the debug version, since this is a real base address.
    GC_register_finalizer_no_order
      (base, (GC_finalization_proc)cleanup,
       (void*) ((char*) this - (char*) base), 
       &oldProc, &oldData );
    if (0 != oldProc) {
      GC_register_finalizer_no_order(base, oldProc, oldData, 0, 0 );
    }
  }
}

#endif


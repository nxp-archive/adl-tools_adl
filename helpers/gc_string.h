//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// A garbage-collected C++ string.
//

#ifndef _GC_STRING_H_
#define _GC_STRING_H_

#include <string>
#include <sstream>

#include "gc/gc_allocator.h"

#include "stringhash.h"

typedef std::basic_string<char,std::char_traits<char>,gc_allocator<char> >  gc_string;
typedef std::basic_ostringstream<char,std::char_traits<char>,gc_allocator<char> > gc_ostringstream;
typedef std::basic_istringstream<char,std::char_traits<char>,gc_allocator<char> > gc_istringstream;

inline gc_string to_gstr(const std::ostringstream &os) { return gc_string(os.str().c_str()); }
inline gc_string to_gstr(const std::string &s) { return gc_string(s.c_str()); }

#ifndef _MSC_VER 

namespace std {
  namespace tr1 {

    // Hash/set support for gc'd strings.
    template<> struct hash<gc_string>
    {
      size_t operator()(const gc_string &__str) const { 
        const char *__s = __str.c_str();
        unsigned long __h = 0;
        for ( ; *__s; ++__s)
          __h = 5 * __h + *__s;
        return size_t(__h);
      }
    };
    
  }
}

#endif

#endif


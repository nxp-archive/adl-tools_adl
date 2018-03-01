//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Support utilities for writers.
//

#ifndef _WRITER_UTILS_H_
#define _WRITER_UTILS_H_

#include "helpers/stringfwd.h"

// All this does is give the user a setw() function which can take an RNumber.
// The width is then set to the number of nibbles in the RNumber object.  I
// duplicate the iomanip setw logic to ensure portability, since those classes
// are meant to be private.

struct RnSetw { int _width; };

inline RnSetw setw(const rnumber::RNumber &rn)
{ 
  RnSetw __x; 
  __x._width = rn.size()/4; 
  return __x; 
}

template<typename _CharT, typename _Traits>
inline std::basic_ostream<_CharT,_Traits>& 
operator<<(std::basic_ostream<_CharT,_Traits>& __os, RnSetw __f)
{ 
  __os.width(__f._width); 
  return __os; 
}

#endif


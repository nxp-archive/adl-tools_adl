//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
// This file contains various bit-level manipulation functions.  Where possible, we
// use GCC builtins, unless USE_NO_GCC_BUILTINS is defined.
//

#ifndef _BITTWIDDLES_H_
#define _BITTWIDDLES_H_

#include "BasicTypes.h"

// A macro for static-predict-taken conditionals.
#if !defined(__GNUC__) || defined(USE_NO_GCC_BUILTINS)  
#  define EXPECT(x,y) (x) == (y)
#else
#  define EXPECT(x,y) __builtin_expect(x,y)
#endif

// Returns true if the value is a power of 2.
inline bool is_power_2(unsigned v)
{
  return !(v & (v - 1)) && v;
}

// Counts the number of 1-bits in x.
inline int count_ones(uint64_t x)
{
#if !defined(__GNUC__) || defined(USE_NO_GCC_BUILTINS)  
	int c;
  for (c = 0; x; c++) {
    x &= x - 1; // clear the least significant bit set
  }
  return c;
# else
  return __builtin_popcountll(x);  
# endif
}

inline int count_ones(uint32_t x)
{
#if !defined(__GNUC__) || defined(USE_NO_GCC_BUILTINS)  
	int c;
	for (c = 0; x; c++) {
		x &= x - 1; // clear the least significant bit set
	}
	return c;
# else
  return __builtin_popcount(x);  
# endif
}

inline int count_ones(uint16_t x)
{
  return count_ones((uint32_t)x);
}

// Count the number of trailing 0s.
inline int count_trailing_zeros(uint32_t x)
{
#if !defined(__GNUC__) || defined(USE_NO_GCC_BUILTINS)
  int c = 0; // c will be the number of zero bits on the right,
  // so if x is 1101000 (base 2), then c will be 3
  // NOTE: if 0 == x, then c = 31.
  if (x & 0x1) {
    // special case for odd x (assumed to happen half of the time)
  } else {
    if ((x & 0xffff) == 0) {  
      x >>= 16;  
      c += 16;
    }
    if ((x & 0xff) == 0) {  
      x >>= 8;  
      c += 8;
    }
    if ((x & 0xf) == 0) {  
      x >>= 4;
      c += 4;
    }
    if ((x & 0x3) == 0) {  
      x >>= 2;
      c += 2;
    }
    if ((x & 0x1) == 0) {
      c++;
    }
  }
  return c;
# else
  return __builtin_ctz(x);
# endif
}

inline int count_trailing_zeros(uint16_t x)
{
  return count_trailing_zeros((uint32_t)x);
}

inline int count_trailing_zeros(uint64_t x)
{
#if !defined(__GNUC__) || defined(USE_NO_GCC_BUILTINS)
  int c = 0; // c will be the number of zero bits on the right,
  // so if x is 1101000 (base 2), then c will be 3
  // NOTE: if 0 == x, then c = 31.
  if (x & 0x1) {
    // special case for odd x (assumed to happen half of the time)
  } else {
    if ((x & 0xffffffffULL) == 0) {  
      x >>= 16;  
      c += 16;
    }
    if ((x & 0xffffULL) == 0) {  
      x >>= 16;  
      c += 16;
    }
    if ((x & 0xffULL) == 0) {  
      x >>= 8;  
      c += 8;
    }
    if ((x & 0xfULL) == 0) {  
      x >>= 4;
      c += 4;
    }
    if ((x & 0x3ULL) == 0) {  
      x >>= 2;
      c += 2;
    }
    if ((x & 0x1ULL) == 0) {
      c++;
    }
  }
  return c;
# else
  return __builtin_ctzll(x);
# endif
}

// Counts the number of 1-bits in x.
inline int count_leading_zeros(uint64_t x)
{
  if (x == 0) return sizeof(x)*8;
#if !defined(__GNUC__) || defined(USE_NO_GCC_BUILTINS)  
  unsigned n = 0;
  while (1) {
    if (x < 0) break;
    n++;
    x <<= 1;
  }
  return n;
# else
  return __builtin_clzll(x);  
# endif
}

inline int count_leading_zeros(uint32_t x)
{
  if (x == 0) return sizeof(x)*8;
#if !defined(__GNUC__) || defined(USE_NO_GCC_BUILTINS)  

  unsigned n = 0;
  while (1) {
    if (x < 0) break;
    n++;
    x <<= 1;
  }
  return n;
# else
  return __builtin_clz(x);  
# endif
}

inline int count_leading_zeros(uint16_t x)
{
  return count_leading_zeros((uint32_t)x);
}

// Just for completeness, so that we can generate calls to swap routines w/o
// special casing uint8_t.
inline uint16_t swap_uint8(uint8_t x)
{
  return x;
}

inline uint16_t swap_uint16(uint16_t x)
{
# if defined(_MSC_VER)
  // Microsoft builtin.
  return _byteswap_ushort(x);
# elif !defined(USE_NO_GCC_BUILTINS) && (defined(__GNUC__) && (__GNUC__ == 4) && (__GNUC_MINOR__ >= 3))
  // GCC 4.3 and above has a builtin.
  return (__builtin_bswap32(x) >> 16);
# else
  // Else use basic C.
  return  ((x & (uint16_t)0xff) << 8) | (x >> 8);
# endif
}


inline uint32_t swap_uint32(uint32_t x)
{
# if defined(_MSC_VER)
  // Microsoft builtin.
  return _byteswap_ulong(x);
# elif !defined(USE_NO_GCC_BUILTINS) && (defined(__GNUC__) && (__GNUC__ == 4) && (__GNUC_MINOR__ >= 3))
  // GCC 4.3 and above has a builtin.
  return __builtin_bswap32(x);
# else
  // Else use basic C.
  return (((x) << 24) | (((x) & 0x0000ff00UL) << 8) | (((x) & 0x00ff0000UL) >> 8) | ((x) >> 24));
# endif
}

inline uint64_t swap_uint64(uint64_t x)
{
# if defined(_MSC_VER)
  // Microsoft builtin.
  return _byteswap_uint64(x);
# elif !defined(USE_NO_GCC_BUILTINS) && (defined(__GNUC__) && (__GNUC__ == 4) && (__GNUC_MINOR__ >= 3))
  // GCC 4.3 and above has a builtin.
  return __builtin_bswap64(x);
# else
  // Else use basic C.
  return (((x) << 56) |
          (((x) & 0x000000000000ff00ULL) << 40) |
          (((x) & 0x0000000000ff0000ULL) << 24) |
          (((x) & 0x00000000ff000000ULL) <<  8) |
          (((x) & 0x000000ff00000000ULL) >>  8) |
          (((x) & 0x0000ff0000000000ULL) >> 24) |
          (((x) & 0x00ff000000000000ULL) >> 40) |
          ((x) >> 56) );
# endif
}

#endif

//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Various simple types.
//

#ifndef _BASIC_TYPES_H_
#define _BASIC_TYPES_H_

#include <iosfwd>
#ifdef _MSC_VER

// For Visual, we don't bother with these attributes.
#ifndef ATTRIBUTE_UNUSED
# define ATTRIBUTE_UNUSED
#endif

#ifndef ATTRIBUTE_USED
# define ATTRIBUTE_USED
#endif

#ifndef ATTRIBUTE_NOINLINE
# define ATTRIBUTE_NOINLINE
#endif

#ifndef ATTRIBUTE_WEAK
# define ATTRIBUTE_WEAK
#endif

#ifndef ATTRIBUTE_FLATTEN
# define ATTRIBUTE_FLATTEN
#endif

#ifndef ATTRIBUTE_INLINE
# define ATTRIBUTE_INLINE
#endif

#ifndef FASTCALL
# define FASTCALL
#endif

#ifndef FASTCALL2
# define FASTCALL2 __fastcall
#endif

// Another example of how broken Windows is:  They don't include inttypes.h!
// I guess this is because they don't support C99.  After all, it's only been
// eight years and Microsoft is at least a decade behind on everything they do.

// Optionally, we can turn this off, in case someone else has already defined
// these.
#ifndef _NO_INTTYPES_

#if _MSC_VER < 1600
/* Pre-Visual Studio 10.0 */
    typedef __int64  int64_t;
    typedef unsigned __int64 uint64_t;

    typedef __int32  int32_t;
    typedef unsigned __int32 uint32_t;

    typedef __int16 int16_t;
    typedef unsigned __int16 uint16_t;

    typedef __int8  int8_t;
    typedef unsigned __int8 uint8_t;

    typedef unsigned int u_int;

    typedef unsigned char u_char;
#else
/* Visual Studio 10.0 */
#include <stdint.h>
#endif

#endif

// Visual C++ doesn't support branch predictor hints.  Lame!
#define adl_likely(x)   (x)
#define adl_unlikely(x) (x)

typedef unsigned int u_int;

#else
# include <inttypes.h>

#ifdef _WIN32
typedef unsigned int u_int;
#endif

// Namespace in which STL extensions are stored.
#ifndef _STDEXT_
# define _STDEXT std::tr1::
#endif

// Makes porting between MSVC and modern gcc easier.
#ifndef _WIN32
#define hash_map unordered_map
#endif

// For GNU, we specify this to avoid warnings about unused variables.
#ifndef ATTRIBUTE_UNUSED
# define ATTRIBUTE_UNUSED __attribute__((unused))
#endif

#ifndef ATTRIBUTE_USED
# define ATTRIBUTE_USED __attribute__((used))
#endif

// For GNU, we use this to specify a weak binding to the linker.  Doesn't work on Windows.
#ifndef ATTRIBUTE_WEAK
# if !defined(__CYGWIN__) && !defined(__MINGW32__)
#   define ATTRIBUTE_WEAK __attribute__((weak))
# else
#   define ATTRIBUTE_WEAK
# endif
#endif

// This forces a function not to be inlined.
#ifndef ATTRIBUTE_NOINLINE
# define ATTRIBUTE_NOINLINE __attribute__((noinline))
#endif

// This forces a function to be completely flattened.
#ifndef ATTRIBUTE_FLATTEN
# define ATTRIBUTE_FLATTEN __attribute__((flatten))
#endif

// This says to inline a function.
#ifndef ATTRIBUTE_INLINE
# define ATTRIBUTE_INLINE __attribute__((always_inline))
#endif

// The fastcall stuff is not needed in 64-bit and it appears to be broken in gcc 3.4.3
#if !defined(__CYGWIN__) && !defined(__MINGW32__)
# define __fastcall
# define FASTCALL2
#else
# define FASTCALL2 __fastcall
#endif

#if !defined(FASTCALL)
#  if __WORDSIZE == 64
#   define FASTCALL
#  else
#   if (defined(__GNUC__) && __GNUC__ == 3)
#     define FASTCALL
#    else
#     define FASTCALL     __attribute__ ((fastcall))      
#   endif
#  endif
#endif

// Branch prediction hints for gcc.
#define adl_likely(x)       __builtin_expect((x),1)
#define adl_unlikely(x)     __builtin_expect((x),0)

#endif // _MSC_VER

// For anything you don't want when using Visual C++
#ifdef _MSC_VER
# define NotMSVC(x)
#else
# define NotMSVC(x) x
#endif

#if __WORDSIZE == 64
#  define IS_64BIT
#  define Is64Bit(x) x
#  define Is32Bit(x)
#else
#  define IS_32BIT
#  define Is64Bit(x)
#  define Is32Bit(x) x
#endif


namespace adl {

  typedef uint64_t addr_t;
  
  typedef uint64_t ttime_t;
  
  typedef uint8_t byte_t;

  enum { 
    DWMask   = 0x7,
    WordMask = 0x3,
    HWMask   = 0x1,
    BytesPerDW = sizeof(uint64_t),
    BytesPerWord = sizeof(uint32_t),
    BytesPerHW = sizeof(uint16_t),
    LongBits = sizeof(uint32_t) * 8,
    AddrBits = sizeof(addr_t) * 8,
  };

  // This class is specialized so that it can return the appropriate integer
  // type, given a template parameter specifying a size in bytes.
  template <size_t NB>
  struct DmiSizeType {
  };

  template <>
  struct DmiSizeType<8> {
    typedef uint64_t IntType;
  };

  template <>
  struct DmiSizeType<4> {
    typedef uint32_t IntType;
  };

  template <>
  struct DmiSizeType<2> {
    typedef uint16_t IntType;
  };

  template <>
  struct DmiSizeType<1> {
    typedef uint8_t IntType;
  };


  typedef int (*Disassembler)(std::ostream &os,addr_t pc,const uint32_t *opc);

  // Types of MMU translations.
  enum TransType {InstrTrans,LoadTrans,StoreTrans,WriteTrans};

  // Types of caches.
  enum CacheType { InstrCache, DataCache, UnifiedCache };

  // Types of branches.
  enum BranchType { BranchNone, BranchUncond, BranchCond };

  // Types for exception sensitivity.
  enum SensitivityType { EdgeSensitive, LevelSensitive };

  // Memory access type.
  enum CacheAccess {
    CacheNoAccess, CacheIFetch, CacheIFetchTrans, CacheILogRead, CacheLogRead, CacheRead,
    CacheWrite, CacheFlush, CacheStore, CacheTouch, CacheAlloc, CacheInvalidate,
    CacheLock, CacheLockAddr, CacheUnlock, MaxCacheAccess
  };

  // Actions performed by the cache.
  enum CacheAction { CacheNone, CacheHit, CacheMiss, CacheEvict, CacheLoad };

  // Types of watchpoints.
  enum WatchType { wpNone = 0, wpRead = 0x1,wpWrite = 0x2,wpAccess = 0x3 };

  // Types of messages sent by ISS.
  enum MsgType { mInfo, mWarn, mError };

  // Types of context-change updates.
  enum CtxType { ctxSwitch, ctxUpdate, ctxDone };

  // For transactional ISSs, this is the return type from prefetch.
  enum PrefetchReturnType { prNotActive, prActive, prSkip, prException };

  // Max levels supported for CacheStatus object (used by transactional ISSs)
  enum { MaxCacheStatusLevels = 3 };

  //
  // This is used for the gdb server to determine
  // what state the model is in, and so it can send
  // packets back to the client accordingly.
  //
  enum ProgramStatus {
    ACTIVE,
    BREAK,
    HALT
  };

  // Describes the different simulation options that might be available for a
  // model.
  enum SimType {
    SimNormal,          // Basic interpretive simulation.
    SimJit,             // Dynamic-binary-translation simulation.
    SimDc,              // Decode-cache simulation.
  };

  // Type of ELF file.  Note that this enum matches the template parameter used
  // by the ELF parser.
  enum ElfClass { ELFNONE = 0,ELF32 = 1,ELF64 = 2 };

  // Whether to use function descriptors when reading ELF files.
  enum ElfUseFd { ElfFdArch, ElfFdFalse, ElfFdTrue };
}

// More hacks to fix Visual C++'s total brokenness.  They don't support C++
// variable-sized stack arrays, so we have to use the alloca method for creating
// arrays.
#ifdef _MSC_VER
# define USE_ALLOCA
# if _MSC_VER <= 1310
#  include <malloc.h>
# endif
#endif

// Define to true if C99-style array declarations are allowed (size can be set
// via variables at declaration time).
#ifdef USE_ALLOCA
# define DeclArray(type,name,size) type *name = (type*)alloca(sizeof(type)*(size))
#else
# define DeclArray(type,name,size) type name[size]
#endif

// Helper macro:  Define this if we have a newer version of Boost.
#if ((BOOST_VERSION % 1000000) >= 1) && ((BOOST_VERSION % 1000) >= 53)
#  define BOOST_1_53
#endif

#endif

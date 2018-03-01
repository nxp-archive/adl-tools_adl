//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Miscellaneous macros for code simplification.
//

#ifndef _MACROS_
#define _MACROS_

// Bad MSVC++ bug:  They define min/max as macros, which interferes with the min/max defines in std.
#ifdef _MSC_VER
# define NOMINMAX
#endif

#include <memory>

// For clang, so that we don't have to compile in c++11 mode, which causes its own problems...
#ifdef ADL_JIT_COMPILE
#define decltype typeof
#endif

// For clang, so that we don't have to compile in c++11 mode, which causes its own problems...
#ifdef ADL_JIT_COMPILE
#define decltype typeof
#endif

#define AdlTypeof(x) auto

// Iterate over a data structure.  Note that this can be used for
// constant objects as well as non-constant objects because of the
// use of the 'typeof' operator.
#define ForEach(container,iter) \
  for (auto iter = container.begin(), end = container.end(); iter != end; ++iter)

#define RevForEach(container,iter) \
  for (auto iter = container.rbegin(), rend = container.rend(); iter != rend; ++iter)


// Execute code if 'z' is found in 'y'.  'x' is the iterator assigned the value.
#define IfFind(x,y,z) \
  auto x = y.find(z); if (x != y.end())

#define IfNotFind(x,y,z) \
  auto x = y.find(z); if (x == y.end())

#define ForRange(count,iter) \
  for (unsigned iter = 0; iter != (count); ++iter)

#define ForRevRange(count,iter) \
  for (int iter = count-1; iter >= 0 ; --iter)

// Throws a runtime error.  User must include sstream for this to work.
#define RError(x) { std::ostringstream ss; ss << x; throw std::runtime_error(ss.str()); }

// Displays a message to stderr and then calls exit(1) to immediately exit.
#define ErrExit(x) { std::cerr << x << '\n'; exit(1); }

#define RAssert(x,y) { if (!(x)) { RError(y); } }

#define VPrint(x) { if (VerboseMode) { std::cout << x; } }

#define QPrint(x) { if (!QuietMode) { std::cerr << x << endl; } }

#define DPrint(x) { if (DebugMode) { std::cout << "DEBUG:  " << x; } }

#ifdef DEBUG
#  define DBPrint(x) { std::cerr << x << '\n'; }
#else
#  define DBPrint(x)
#endif


#define MkStr(x,y) { std::ostringstream ss; ss << y; x = ss.str(); }
#define MkGStr(x,y) { gc_ostringstream ss; ss << y; x = ss.str(); }


// This will stringify an arbitrary value.
#define Stringify(x) #x

// Call this to expand 'x' if x is also a preprocessor definition.
#define Expand(x) Stringify(x)

#ifndef ADL_JIT_COMPILE

// For this one, you have to pass in the heap-allocated object (should be done
// as an rvalue).
template <class T>
inline std::unique_ptr<T> make_uniq_p(T *t) { return std::unique_ptr<T>(t); };

#endif

#endif


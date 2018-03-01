//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Simple forward declaration of string types.
//

#ifndef _STRINGFWD_H
#define _STRINGFWD_H

namespace std
{
  template<typename _Alloc>
    class allocator;
  
  template<class _CharT>
    struct char_traits;

  template<typename _CharT, typename _Traits = char_traits<_CharT>,
           typename _Alloc = allocator<_CharT> >
    class basic_string;

  template<> struct char_traits<char>;

  typedef basic_string<char> string;

# ifdef _GLIBCXX_USE_WCHAR_T
  template<> struct char_traits<wchar_t>;

  typedef basic_string<wchar_t> wstring;
# endif

}

#endif

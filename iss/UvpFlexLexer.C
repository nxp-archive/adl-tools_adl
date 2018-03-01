//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
//
// Support routines for the tokenizer.
//

#include <sys/stat.h>
#include <fcntl.h>
#include <sstream>
#include <stdexcept>
#include <stdio.h>
#include <string.h>

#include "helpers/Macros.h"

namespace adl {
    class  UvpParser;
    class  UvpFlexLexer;
    struct UvpToken;
    struct UvpValue;
    struct UvpList;
    struct UvpMap;
    struct UvpCmd;
}

#include "uvpparser.hh"

#undef yyFlexLexer
#define yyFlexLexer uvpFlexLexer
#include "FlexLexer.h"
#include "UvpFlexLexer.h"


using namespace std;

namespace adl {

  UvpFlexLexer::UvpFlexLexer(const std::string &fn,bool debug) :
    yyFlexLexer(),
    _fd(default_handle_value()),
    _base(0),
    _prevpos(0),
    _srcbase(0),
    _bufbase(0),
    _size(0),
    _bytes_mapped(0),
    _lookback(PAGE_SIZE)
  {
    if (PAGE_SIZE <= 0) {
      RError("Indefined page size.");
    }
    reset(fn);
    yy_flex_debug = debug;
  }

  UvpFlexLexer::~UvpFlexLexer()
  {
    closefile();
  }

  void UvpFlexLexer::closefile()
  {
    if (_fd != default_handle_value()) {
      close_mmap_file(_fd,_base,_bytes_mapped);
    }
  }

  void UvpFlexLexer::reset(const std::string &filename)
  {
    _filename = filename;
    _offset = 0;

    closefile();

    _bytes_mapped = 0;
  
    _fd = open_mmap_file(_filename.c_str(),_base,_bytes_mapped,_size);
 
    _prevpos = _base;
    _srcbase = _base;
  }
    
  void UvpFlexLexer::reset() {
    reset(_filename);
  }
 
  // This defines a new input routine which copies from an mmap'd file to a
  // buffer.  The copy is necessary b/c flex modifies its buffer.  Thus, you
  // shouldn't store pointers into this buffer, since it will be overwritten.
  // Instead, use yystr() to get a pointer to the equivalent data that's in the
  // mmap'd file.
  int UvpFlexLexer::LexerInput(char* buf, int max_size )
  {

    if (_offset >= (int)_size ) {
      return 0;
    }
    
    int result;

    result = (size_t)(_offset + max_size) < _size ? max_size : _size - _offset;
      
    memcpy(buf,_prevpos,result); 

    _srcbase = _prevpos;
    _prevpos +=  result;
    _bufbase = buf; 
    _offset += result;
    return result;
  }
      
  // This retrieves text from the mmap'd file.  Use this instead of
  // YYText() for anything that will persist, such as strings.
  const char *UvpFlexLexer::yystr()
  { 
    return _srcbase + (YYText()-_bufbase);
  }
}

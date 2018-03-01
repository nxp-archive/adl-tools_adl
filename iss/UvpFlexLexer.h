//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
//
// Header file for the subclassed tokenizer.
//

#ifndef _UVP_FLEXLEXER_H_
#define _UVP_FLEXLEXER_H_

#include <string>

#include "MmapFiles.h"

namespace adl {

#ifdef YY_DECL
# undef YY_DECL
#endif

# define YY_DECL                                                \
  int UvpFlexLexer::uvplex (YYSTYPE &yylval,UvpLocation& yyloc)

  struct UvpLocation;

  // Subclass of the standard lexer.
  //
  // Note:  In order to use this class, the following
  // preprocessor commands must precede the inclusion of
  // this file:
  //
  // #undef yyFlexLexer
  // #define yyFlexLexer uvpFlexLexer
  // #include "FlexLexer.h"
  // #include "UvpFlexLexer.h"
  //
  // This will include the standard base class, redefined 
  // according to our specification.
  //
  class UvpFlexLexer : public uvpFlexLexer {

  public:

    UvpFlexLexer(const std::string &fn,bool debug = false);
    ~UvpFlexLexer();
    
    using uvpFlexLexer::yylex;

    void reset();    
    void reset(const std::string &fn);   
    
    const std::string &filename() const { return _filename; };

    int uvplex(YYSTYPE &yylval,UvpLocation& yyloc);

  protected:
    const char *yystr();

    virtual int LexerInput( char* buf, int max_size );

  private:
    void closefile();
    
    std::string _filename;  // Current file being used
    
    MmapHandle  _fd;
    const char *_base;
    const char *_prevpos;
    const char *_srcbase;
    const char *_bufbase;
    // size of file
    size_t      _size;
    // global offset of the current file
    off_t       _offset;
    // size of the current map
    off_t      _bytes_mapped;
    //Size of lookback offset in remapping. Needed because sometimes for some reason
    //parser requests data with negarive offset
    unsigned   _lookback;
  };

}

#endif

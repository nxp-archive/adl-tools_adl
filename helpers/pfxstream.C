//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#include <stdio.h>

#include "pfxstream.h"

using namespace std;

namespace adl {

  // The streambuf is assumed to be setup correctly.  We do
  // not assume ownership of it.  Note that this class is
  // defined for output only.
  pfxbuf::pfxbuf(streambuf *sb,int indent,char iplus,char iminus) :
    streambuf(),
    _sbuf(sb),
    _lncount(1),
    _ccount(0),
    _newline(false),
    _indent(indent,' '),
    _iplus(iplus),
    _iminus(iminus)
  {
    setp(0, 0);
    setg(0, 0, 0);
  }

  void pfxbuf::resetlinecount() 
  { 
    _lncount = 1; 
  }

  void pfxbuf::resetindent()
  {
    _pfx.clear();
  }

  void pfxbuf::addindent(int x)
  {
    _pfx.append(x,' ');
  }

  void pfxbuf::subindent(int x)
  {
    if (!_pfx.empty()) {
      _pfx.erase(_pfx.size()-x,x);
    }
  }

  int pfxbuf::overflow(int c) 
  {
    if (c != EOF) {
      if (_iminus && c == _iminus && !_pfx.empty()) {
        _pfx.erase(_pfx.size()-_indent.size());
      }
      if (_newline) {
        if (_sbuf->sputn(_pfx.c_str(),_pfx.size()) != (int)_pfx.size()) {
          return EOF;
        }
        _newline = false;
        _ccount += _pfx.size();
      }
      int rc = _sbuf->sputc(c);
      ++_ccount;
      if (c == '\n') {
        ++_lncount;
        _newline = true;
      } else if (_iplus && c == _iplus) {
        _pfx.append(_indent);
      }
      return rc;
    }
    return 0;
  }

  opfxstream::opfxstream(streambuf *sb,int indent,char iplus,char iminus):
    ostream(new pfxbuf(sb,indent,iplus,iminus))
  {
  }

  opfxstream::opfxstream(ostream &o,int indent,char iplus,char iminus):
    ostream(new pfxbuf(o.rdbuf(),indent,iplus,iminus))
  {
  }

  opfxstream::~opfxstream()
  {
    delete rdbuf();
  }

}

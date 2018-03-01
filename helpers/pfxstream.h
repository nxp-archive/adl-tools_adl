//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This is a simple stream buffer which counts new lines and inserts indentation
// at the beginning of lines.  This class is only for output and requires another
// buffer to do the actual work, e.g. a file buffer to write to a file, cout's
// buffer to write to standard out, etc.
//
#ifndef _PFXSTREAM_H_
#define _PFXSTREAM_H_

#include <iostream>
#include <string>

namespace adl {

  // Line counting output buffer.  Note:  Streambuffers are
  // *not* owned by this class.
  class pfxbuf: public std::streambuf
  {
  public:
    pfxbuf(std::streambuf *sb,int indent,char iplus,char iminus);

    unsigned linecount() const { return _lncount; };
    unsigned charcount() const { return _ccount; };

    // Resets the line count.
    void resetlinecount();

    // Resets indentation to 0.
    void resetindent();

    // Adds x number of spaces to the indent string.
    void addindent(int x);

    // Removes x number of spaces from the indent string.
    void subindent(int x);

    bool hasBuf() const { return _sbuf; };
    void setBuf(std::streambuf *sb) { _sbuf = sb; };

  protected:
    int	overflow(int);

  private:
    std::streambuf *_sbuf;	 // The actual streambuf used to read and write chars
    unsigned        _lncount;  // Current line count.
    unsigned        _ccount;   // Character count (for tellp).
    bool            _newline;
    std::string     _pfx;      // Current indentation.
    std::string     _indent;   // Indentation amount.
    char            _iplus;    // Character which triggers an increase in indentation.
    char            _iminus;   // Character which triggers a decrease in indentation.
  };

  // This manages a line-count buffer for you.
  class opfxstream: public std::ostream
  {
  public:
    opfxstream(std::streambuf *sb,int indent = 0, char iplus = 0, char iminus = 0);
    opfxstream(std::ostream &o,int indent = 0, char iplus = 0, char iminus = 0);
    ~opfxstream();

    pfxbuf *rdbuf() { return static_cast<pfxbuf*>(std::ostream::rdbuf()); };
    const pfxbuf *rdbuf() const { return static_cast<const pfxbuf*>(std::ostream::rdbuf()); };

    bool hasBuf() const { return rdbuf()->hasBuf(); };
    void setBuf(std::streambuf *sb) { rdbuf()->setBuf(sb); };

    unsigned linecount() const { return rdbuf()->linecount(); };
    unsigned charcount() { return rdbuf()->charcount(); };

    void resetlinecount() { rdbuf()->resetlinecount(); };
    void resetindent() { rdbuf()->resetindent(); };

    void addindent(int x) { rdbuf()->addindent(x); };
    void subindent(int x) { rdbuf()->subindent(x); };

  };

}

#endif

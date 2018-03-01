//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
//
// Header file for the subclassed tokenizer.
//

#ifndef _UVP_PARSER_H_
#define _UVP_PARSER_H_

#include <map>
#include <vector>
#include <assert.h>
#include <memory>

#include "helpers/BasicTypes.h"

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

namespace rnumber {
  class RNumber;
}

namespace adl {
  class UvpParser;
}

void uvperror(const YYLTYPE* l,adl::UvpParser &driver,const char *m);

namespace adl {

  struct UvpLocation : public YYLTYPE {
    UvpLocation()
    {
      first_line = 1;
      first_column = 0;
      last_line = 1;
      last_column = 0;
    }

    void step ()
    {
      first_line = last_line;
      first_column = last_column;
    }

    /// Extend the current location to the COUNT next columns.
    void columns (unsigned int count = 1)
    {
      last_column += count;
    }

    /// Extend the current location to the COUNT next lines.
    void lines (unsigned int count = 1)
    {
      last_column = 1;
      last_line += count;
    }
  };

  void uvptype_error(const char *,const char *);

  // Tries to cast a generic UVP value to the specified type.  Throws
  // a runtime_error if there's a problem.
  template <class T>
  const T &get_uvptype(const UvpValue &v,const char *tmsg,const char *msg)
  {
    if (const T *t = dynamic_cast<const T*>(&v)) {
      return *t;
    } else {
      uvptype_error(tmsg,msg);
      assert(0); // Dummy.
    }
  }

  // Tries to cast a generic UVP value to the specified type.  Returns 0 if not
  // found.
  template <class T>
  const T *get_uvptype(const UvpValue &v)
  {
    if (const T *t = dynamic_cast<const T*>(&v)) {
      return t;
    } else {
      return 0;
    }
  }

  // Find a key in a map or a command.  Throws a runtime_error if the key is not
  // found.
  const UvpValue &find_key(const UvpMap &parms,const char *key,const char *msg);
  const UvpValue &find_key(const UvpCmd &cmd,const char *key,const char *msg);

  // Tries to find a key and then cast it to the appropriate type.  Throws a
  // runtime_error if not found.
  template <class T>
  const T &find_key(const UvpMap &parms,const char *key,const char *tmsg,const char *msg)
  {
    return get_uvptype<T>(find_key(parms,key,msg),tmsg,msg);
  }

  // Various conversion functions.
  std::string uvtostr(const UvpValue &v,const char *msg);
  rnumber::RNumber uvtorn(const UvpValue &v,const char *msg);
  uint64_t uvtoi64(const UvpValue &v,const char *msg);
  uint32_t uvtoi32(const UvpValue &v,const char *msg);

  // Looks up a key and returns the token value.
  std::string get_str(const UvpCmd &cmd,const char *k,const char *msg);
  rnumber::RNumber get_rn(const UvpCmd &cmd,const char *k,const char *msg);
  uint64_t get_uint64(const UvpCmd &cmd,const char *k,const char *msg);
  uint32_t get_uint32(const UvpCmd &cmd,const char *k,const char *msg);
  int32_t get_int32(const UvpCmd &cmd,const char *k,const char *msg);

  // The base UVP parsing class.  This just tracks the completion of various
  // grammar rules and then invokes various virtual functions when completed.
  // It's up to a derived class to then do something with this data, such as
  // initialize a model.  The actual parser is a bison C++ parser.  The lexer is
  // provided by flex and is also done in C++.  Both are fully re-entrant.  If
  // the option "incremental_scanning" set to false then the scanner reads from
  // a mmap'd file and then just passes around read-only strings which point
  // into this file, thus reducing overall copying.  However, this can be a
  // problem for very large files, in which case one should use incremental
  // scanning, which maps only part of the file and unmaps it when not needed.
  class UvpParser {
  public:
    UvpParser(const std::string &fn,bool trace_parsing = false,
              bool trace_scanning = false);
    virtual ~UvpParser();

    bool is_finished() const { return _finished; };

    const std::string &filename() const { return _fn; };

    void setFullPathErrors(bool f);

    // Parses the next test.  Sets _finished when it encounters the end of the file.
    void parse ();

    void reset();

    // By default, these throw a runtime_error.
    virtual void error(const YYLTYPE &loc,const char *m);

    // These are interface methods which must be overloaded by a user's derived class.
    // They're called during the parsing process as various things are recognized.
  
    // Called for the beginning and ending of a block.
    virtual void handle_block_begin(const YYLTYPE &loc,std::unique_ptr<UvpToken> type,std::unique_ptr<UvpToken> name) = 0;
    virtual void handle_block_end(const YYLTYPE &loc) = 0;

    // Called when a command has been parsed.
    virtual void handle_cmd(const YYLTYPE &loc,std::unique_ptr<UvpCmd> cmd) = 0;

    // Called when a bare parameter outside of a command has been parsed.
    virtual void handle_parm(const YYLTYPE &loc,std::unique_ptr<UvpToken> name,std::unique_ptr<UvpValue> value) = 0;
   
    void report_sob();

    void set_test_finished() { _test_finished = true; _busy = false; };
    
    void set_finished() { _finished = true; _test_finished = true; _busy = false; };
    
    bool test_finished() { return _test_finished; };

    std::string printLocation(const YYLTYPE &);

  private:

    std::string    _fn;
    bool           _trace_parsing;
    UvpFlexLexer  *_lexer;
    uvppstate     *_parser;

    YYSTYPE        _value;
    UvpLocation    _loc;

    bool           _busy;
    bool           _finished;    
    bool           _test_finished;   
    bool           _restarted;
  };
}

#endif

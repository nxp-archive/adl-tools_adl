//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
//
// Various string-manipulation helper functions.
//

#include <string>
#include <iterator>
#ifndef _MSC_VER
# include <strings.h>
#endif
#include <stdlib.h>
#include <string.h>
#include <boost/tokenizer.hpp>

#include "StringHelpers.h"

using namespace std;

#ifdef _MSC_VER
# define strtoll _strotoi64
# define strtoull _strtoui64
#endif

namespace adl {

  const string PathDelimiter = ":";

  ostream &operator<<(ostream &o,const Args &args)
  {
    ostream_iterator<string> out(o," ");
    copy(args.begin(),args.end(),out);
    return o;
  }

  // This just uses Boost for the tokenizing.
  Args tokenize(const string& str,const string& delimiters)
  {
    Args results;

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(delimiters.c_str());
    tokenizer tokens(str, sep);
    for (tokenizer::iterator tok_iter = tokens.begin(); tok_iter != tokens.end(); ++tok_iter) {
      results.push_back(*tok_iter);
    }
    return results;
  }

  // This splits a string into sub-strings based upon white-space delimiters.
  Args splitString(const string& source) 
  {
    return tokenize(source);
  }

  // This splits  string into sub-strings based upon the path delimiter.
  Args splitPath(const string &source)
  {
    return tokenize(source,PathDelimiter);
  }

  // Gets the specified argument and bumps index if that index
  // exists.  Returns an empty string and does not change index if
  // not that many arguments exists.
  string getArg(const Args &args,unsigned &index)
  {
    if (index < args.size()) {
      return args[index++];
    } else {
      return string();
    }
  }

  bool getAddr(uint64_t &addr,const string &arg)
  {
    if (arg.empty()) {
      return false;
    }
    char *endPtr = 0;
    addr = strtoull(arg.c_str(),&endPtr,0);
    return (strlen(endPtr) == 0);
  }

  // Converts a string to a uint64.
  // Returns true on success.
  bool getAddr(uint64_t &addr,const Args &args,unsigned &index)
  {
    return getAddr(addr,getArg(args,index));
  }

  bool getUInt(uint32_t &val,const string &arg)
  {
    if (arg.empty()) {
      return false;
    }
    char *endPtr = 0;
    val = strtoul(arg.c_str(),&endPtr,0);
    return (strlen(endPtr) == 0);
  }

  // Converts a string to a uint32.
  // Returns true on success.
  bool getUInt(uint32_t &val,const Args &args,unsigned &index)
  {
    return getUInt(val,getArg(args,index));    
  }

  // Returns a boolean flag if the argument matches a given value.
  bool getFlag(const Args &args,unsigned &index,const string &flag)
  {
    string arg = getArg(args,index);
    if (arg == flag) {
      return true;
    } else {
      return false;
    }
  }

}

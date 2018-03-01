//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
//
// Various string-manipulation helper functions.
//

#ifndef _STRING_HELPERS_
#define _STRING_HELPERS_

#include <iosfwd>
#include <vector>
#include <string>

#include "helpers/BasicTypes.h"

namespace adl {

  struct Args : public std::vector<std::string> {};

  std::ostream &operator<<(std::ostream &o,const Args &);

  // A simple string-based tokenize function.
  Args tokenize(const std::string& str,const std::string& delimiters = " \t");

  // Split string on whitespace.
  Args splitString(const std::string& source);

  // Split on the path delimiter used to describe ADL hierarchy paths, which is ":".
  Args splitPath(const std::string &source);

  // Gets the specified argument and bumps index if that index
  // exists.  Returns an empty string and does not change index if
  // not that many arguments exists.
  std::string getArg(const Args &args,unsigned &index);

  // Converts a string to a uint64.
  // Returns true on success.
  bool getAddr(uint64_t &addr,const std::string &arg);
  bool getAddr(uint64_t &addr,const Args &args,unsigned &index);

  // Convert a string to a uint32.
  bool getUInt(uint32_t &val,const std::string &arg);
  bool getUInt(uint32_t &val,const Args &args,unsigned &index);

  // Returns a boolean flag if the argument matches a given value.
  bool getFlag(const Args &args,unsigned &index,const std::string &flag);

}

#endif

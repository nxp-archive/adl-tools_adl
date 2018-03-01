//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Various utilities for dealing with filenames.
//

#ifndef _FILENAME_UTILS_
#define _FILENAME_UTILS_

#include <utility>

#include "stringfwd.h"

namespace adl {

  // Return the basename as a C++ string.
  std::string base_name(const std::string &filename);

  // Return the directory name as a C++ string.
  std::string dir_name(const std::string &filename);

  // Tries to return the extension on a filename (not including the ".").
  // Returns an empty string if it cannot.  If the filename is of the form
  // "name:format", then returns format, i.e. the user can explicitly specify a
  // format irrespective of the actual filename.
  std::string getExtension(const std::string &fn);

  // Given a filename of the form "name:format", returns "name".  In other
  // words, if the user has specified an explicit format, then this returns the
  // filename portion of that string.
  std::string removeFormatSpecifier(const std::string &fn);

  // Given an input name, remove the input extension, if it exists,
  // and append the specified output extension.  This also removes any
  // leading directory information.
  std::string gen_default_name(const std::string &in,const std::string &ext);

  // Return directory name/basename as a pair.
  std::pair<std::string,std::string> file_parse(const std::string &filename);

  // Split a file into its filename and extension portion.  A leading directory
  // goes with the filename.
  std::pair<std::string,std::string> split_file_name(const std::string &filename);

  // Returns the name of the current executable.  Use dir_name to then get just
  // the directory.
  std::string current_exe_name();

}

#endif

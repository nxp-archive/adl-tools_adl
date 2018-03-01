//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Various utilities for dealing with filenames.
//

#include <string>
#include <assert.h>

#ifdef _MSC_VER
# include <windows.h>
# include <stdlib.h>
# include <sstream>
# include "helpers/Macros.h"
#else
# include <libgen.h>
# include <unistd.h>
#endif

#include "FilenameUtils.h"

using namespace std;

namespace adl {

  string base_name(const string &fn)
  {
#   ifdef _MSC_VER
	  char drive[10];
	  char dir[256];
	  char file[256];
	  char ext[10];
#     if _MSC_VER >= 1400
	  if (_splitpath_s(fn.c_str(),drive,dir,file,ext)) {
		  RError ("Error trying to retrieve basename of " << fn);
	  }
#     else
	  _splitpath(fn.c_str(),drive,dir,file,ext);
#     endif
	  return string(file) + ext;
#   else
    string tmp = fn.c_str();
    return basename((char *)tmp.c_str());
#   endif
  }

  string dir_name(const string &fn)
  {
#   ifdef _MSC_VER
	  char drive[10];
	  char dir[256];
	  char file[256];
	  char ext[10];
#     if _MSC_VER >= 1400
	  if (_splitpath_s(fn.c_str(),drive,dir,file,ext)) {
		  RError ("Error trying to retrieve basename of " << fn);
	  }
#     else
	  _splitpath(fn.c_str(),drive,dir,file,ext);
#     endif
	  return string(drive)+string(dir);
#   else
    string tmp = fn.c_str();
	  return dirname((char *)tmp.c_str());
#   endif
  }

  // Tries to return the extension on a filename (not including the ".").
  // Returns an empty string if it cannot.
  string getExtension(const string &fn)
  {
    // First, look for an explicit format specifier and prefer that.
    size_t n = fn.rfind(':');
    if (n != string::npos) {
      return fn.substr(n+1);
    }

    // Otherwise, return the file extension.
    n = fn.rfind('.');
    if (n != string::npos) {
      return fn.substr(n+1);
    } 

    // Didn't find anything, so return an emtpy string.
    return string();
  }

  // Returns the name portion, if the user has specified an explict format,
  // e.g. "name:format".
  string removeFormatSpecifier(const string &fn)
  {
    size_t n = fn.rfind(':');
    if (n != string::npos) {
      return fn.substr(0,n);
    } else {
      return fn;
    }
  }

  pair<string,string> file_parse(const string &fn)
  {
    string tmp(fn);
    return make_pair(dir_name(tmp),base_name(tmp));
  }

  // Given an input name, remove the input extension, if it exists,
  // and append the specified output extension.  This also removes any
  // leading directory information.
  string gen_default_name(const string &fn,const string &ext)
  {
    string base = base_name(fn);
    size_t es = base.rfind('.');
    if (es != string::npos) {
      base.erase(es);
    }
    return base + ext;
  }

  std::pair<std::string,std::string> split_file_name(const std::string &filename)
  {
    string bn = base_name(filename);
    unsigned e = bn.find(".");
    unsigned b_bn = filename.find(bn);
    string fn = filename.substr(0,b_bn+e);
    string ext = filename.substr(b_bn+e);

    return make_pair(fn,ext);
  }

  // On Linux, we use /proc/self/exe to get our name.  On Windows, we use
  // getModuleFileName.
  string current_exe_name()
  {
#   ifdef _MSC_VER
    char buf[MAX_PATH];//always use MAX_PATH for filepaths
    GetModuleFileNameA(NULL,buf,sizeof(buf));
    return buf;
#   else
    const int BufSize = 2048;
    char buf[BufSize];
    ssize_t len = readlink("/proc/self/exe",buf,sizeof(buf)-1);
    assert(len != -1);
    buf[len] = '\0';
    return buf;
#   endif
  }

}

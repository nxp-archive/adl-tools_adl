//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Various helpers for code generation.
//

#ifndef _CODE_GEN_H_
#define _CODE_GEN_H_

#include <iosfwd>

#include "gc_string.h"
#include "stringfwd.h"

namespace adl {

  // Inserts an arbitrary prefix if not first.
  void pfx_sep(std::ostream &o,const gc_string &pfx,bool &first);
  void pfx_sep(gc_string &s,const gc_string &pfx,bool &first);

  // Add on an item, with a separator if not the first item.
  void pfx_add(gc_string &dst,const gc_string &src,const gc_string &pfx,bool &first);

  // Insert prefix at the beginning of each line.
  // If not "first", the prefix is only inserted starting at line 2.
  void indent_lines(gc_string &dst,const gc_string &src,const gc_string &pfx,
                    bool first=false,const gc_string &linesep="\n");

  // Remove any matching prefix characters from the beginning of each line.
  // If "maxline" is not 0, the number of characters stripped from line "maxline"
  // is the maximum strip length.
  void unindent_lines(gc_string &dst,const gc_string &src,const gc_string &pfx,
                      unsigned maxline=2,const gc_string &linesep="\n");

  // Simple function for inserting a list separator if not on the first object.
  void list_sep(std::ostream &o,bool &first);
  void jit_list_sep(std::ostream &o,bool &first);
  void list_sep(std::string &s,bool &first);
  void list_sep(gc_string &s,bool &first);

  // List separator function for constructor init lists.
  void constr_init_sep(std::ostream &o,bool &first);

  // Inserts a seperator for an if/then/else chain.
  void cond_sep(std::ostream &o,bool &first);

  // Convert an unsigned integer to a string.
  std::string uitoa(unsigned x);

  gc_string uitogs(unsigned x);

  // Convert a string to upper/lower case.
  gc_string lowercase(const gc_string &s);
  gc_string uppercase(const gc_string &s);

  // Filters the string, replacing any newlines with the specified prefix.
  gc_string filter_string(const gc_string &,const gc_string &pfx);

  // Generate a friendly (C-identifier) name from a supplied name.
  gc_string makeFriendlyName(const gc_string &name);

  // Create a default version to be used for a generated program.
  gc_string createDefaultVersion();  

  // Return a program name and version for a generated program's usage logging.
  // fname:            Program name to be used as a default.  The base-name w/o extension is used.
  // log_usage:
  //   "true":         Log using default info.
  //   <ident>:<ver>:  Log using that.
  //   <ident>:        Log using the program name and a default version.
  void createModelVersion(gc_string &progname,gc_string &verison,const gc_string &fname,const gc_string &log_usage);

  // For generating code, this emits a macro to define the code only when not in a header.
  gc_string headerOnly(bool multi_compile);

  // Just the preprocessor stuff, no function attribute.
  gc_string headerOnly2();

  // Ends the define.
  gc_string endHeaderOnly();

  // Define the code for the non-jit compile.  JIT will just see the decl.
  gc_string nonJitOnly();

  // Ends the define.
  gc_string endNonJitOnly();

}

#endif


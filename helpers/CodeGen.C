//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Various helpers for code generation.
//

#include <time.h>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <algorithm>

#include "helpers/gc_string.h"
#include "helpers/gc_vector.h"

#include "Macros.h"
#include "CodeGen.h"
#include "StringUtils.h"
#include "FilenameUtils.h"

using namespace std;

namespace adl {

  // Inserts an arbitrary prefix if not first.
  void pfx_sep(ostream &o,const gc_string &pfx,bool &first)
  {
    if (!first) {
      o << pfx;
    }
    first = false;
  }

  void pfx_sep(gc_string &s,const gc_string &pfx,bool &first)
  {
    if (!first) {
      s += pfx;
    }
    first = false;
  }

  // Add on an item, with a separator if not the first item.
  void pfx_add(gc_string &dst,const gc_string &src,const gc_string &pfx,bool &first)
  {
    pfx_sep(dst,pfx,first);
    dst += src;
  }

  // Insert prefix at the beginning of each line.
  // If not "first", the prefix is only inserted starting at line 2.
  void indent_lines(gc_string &dst,const gc_string &src,const gc_string &pfx,
                    bool first,const gc_string &linesep)
  {
    typedef gc_vector< gc_string > LinesType;
    LinesType lines = adl::split< LinesType, gc_string >(src, linesep, false/* omit empty */);
    LinesType::size_type count = 0;
    dst = gc_string();
    ForEach(lines,l) {
      if ((first) || (count > 0)) {
        dst += pfx;
      }
      dst += *l;
      dst += "\n";
      ++count;
    }
  }

  // Remove any matching prefix characters from the beginning of each line.
  // If "maxline" is not 0, the number of characters stripped from line "maxline"
  // is the maximum strip length.
  void unindent_lines(gc_string &dst,const gc_string &src,const gc_string &pfx,
                      unsigned maxline,const gc_string &linesep)
  {
    typedef gc_vector< gc_string > LinesType;
    LinesType lines = adl::split< LinesType,gc_string >(src, linesep, false/* omit empty */);
    LinesType::size_type count = 1;
    LinesType::size_type maxindex = src.npos;
    dst = gc_string();
    ForEach(lines,l) {
      auto startindex = l->find_first_not_of(pfx);
      if (maxline == count) {
        maxindex = startindex;
      }
      if (startindex != l->npos) {
        if (startindex > maxindex) {
          startindex = maxindex;
        }
        dst.append(*l, startindex, l->npos - startindex);
      }
      dst += "\n";
      ++count;
    }
  }

  // Simple function for inserting a list separator if not on the first object.
  void jit_list_sep(ostream &o,bool &first)
  {
    if (!first) {
      o << " \", \" << ";
    }
    first = false;
  }

  // Simple function for inserting a list separator if not on the first object.
  void list_sep(ostream &o,bool &first)
  {
    if (!first) {
      o << ", ";
    }
    first = false;
  }

  void list_sep(string &s,bool &first)
  {
    if (!first) {
      s += ", ";
    }
    first = false;
  }

  void list_sep(gc_string &s,bool &first)
  {
    if (!first) {
      s += ", ";
    }
    first = false;
  }

  // List separator function for constructor init lists.
  void constr_init_sep(ostream &o,bool &first) 
  {
    if (first) {
      o << " : ";
      first = false;
    } else {
      o << ", ";
    }
    o << "\n";
  }

  void cond_sep(ostream &o,bool &first)
  {
    if (first) {
      o << "if ";
      first = false;
    } else {
      o << "else if ";
    }
  }

  string uitoa(unsigned x) 
  {
    ostringstream s;
    s << x;
    return s.str();
  }

  gc_string uitogs(unsigned x) 
  {
    gc_ostringstream s;
    s << x;
    return s.str();
  }

  gc_string lowercase(const gc_string &s)
  {
    gc_string n = s;
    transform (n.begin(), n.end(),   // source
               n.begin(),                    // destination
               (int(*)(int))tolower);                // operation
    return n;
  }

  gc_string uppercase(const gc_string &s)
  {
    gc_string n = s;
    transform (n.begin(), n.end(),   // source
               n.begin(),                    // destination
               (int(*)(int))toupper);                // operation
    return n;
  }

  gc_string filter_string(const gc_string &s,const gc_string &pfx)
  {
    gc_string r;
    r.reserve(s.size());

    ForEach(s,i) {
      if (*i == '\n') {
        r += pfx;
      } else {
        r += *i;
      }
    }
    return r;
  }

  // Generate a friendly name from the supplid string.
  gc_string makeFriendlyName(const gc_string &o)
  {
    gc_string n = o;
    auto i = n.begin();
    for (; i != n.end(); ++i) {
      if (!((isalnum(*i)) || *i == '_')) {
        *i = '_';
      }
    }
    return n;
  }

  gc_string createDefaultVersion()
  {
    time_t result = time(0);
    struct tm *ts = localtime(&result);
    gc_ostringstream ss;
    ss << ts->tm_year+1900 << "." << ts->tm_mon+1 << "." << ts->tm_mday;
    
    return ss.str();
  }

  void createModelVersion(gc_string &progname,gc_string &version,const gc_string &fname,const gc_string &log_usage)
  {
    if (log_usage == "true" || log_usage.empty()) {
      // Default usage logging.  Generate a default program name and version
      // number to use.
      progname = gen_default_name(fname.c_str(),string()).c_str();
      version = createDefaultVersion();
    } else {
      // Data was supplied by the user, so use it.  Look for the ":" separator; to the 
      // left is the program-name, to the right is the version.
      size_t s = log_usage.find(":");
      if (s == gc_string::npos) {
        // No separator found, so create a default version and use the whole
        // thing as the program name.
        progname = log_usage;
        version = createDefaultVersion();
      } else {
        progname = log_usage.substr(0,s);
        version = log_usage.substr(s+1);
      }
    }
  }

  gc_string headerOnly(bool multi_compile)
  {
    gc_string s;
    if (multi_compile) {
      s += "ATTRIBUTE_USED";
    }
    s += 
      "\n"
      "#ifdef ADL_HEADER\n"
      ";\n"
      "#else\n";

    return s;
  }

  gc_string headerOnly2()
  {
    gc_string s = 
      "\n"
      "#ifdef ADL_HEADER\n"
      ";\n"
      "#else\n";

    return s;
  }

  gc_string endHeaderOnly()
  {
    return "#endif // ADL_HEADER\n\n";
  }

  gc_string nonJitOnly()
  {
    gc_string s = 
      " ATTRIBUTE_USED\n"
      "#ifdef ADL_JIT_COMPILE\n"
      ";\n"
      "#else\n";

    return s;
  }

  gc_string endNonJitOnly()
  {
    return "#endif // ADL_JIT_COMPILE\n\n";
  }

}

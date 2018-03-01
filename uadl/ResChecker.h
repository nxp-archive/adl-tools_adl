//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
// This class is used for checking a custom resource class.  It parses the class
// in order to determine the constructor and template arguments.
//

#ifndef _RESCHECKER_H_
#define _RESCHECKER_H_

#include "helpers/gc_string.h"
#include "helpers/gc_list.h"

namespace uadl {

  struct ArgItem : public gc {
    gc_string _name;
    bool      _has_default;
    bool      _builtin;

    ArgItem(const gc_string &n,bool h) : 
      _name(n), 
      _has_default(h), 
      _builtin(false) {};
  };

  std::ostream &operator<<(std::ostream &o,const ArgItem &a);

  typedef gc_list<ArgItem> ArgItems;

  struct ResInfo : public gc {
    ArgItems _tmpl_args, _constr_args;

    bool      _hasPreCycle, _hasPostCycle, _hasPostSim;

    ResInfo() : _hasPreCycle(false), _hasPostCycle(false), _hasPostSim(false) {};

    bool empty() const { return _tmpl_args.empty() && _constr_args.empty(); };
  };

  std::ostream &operator<<(std::ostream &o,const ResInfo &a);

  // Parse a custom resource and return information about arguments.
  //   fn:     Input file name.
  //   cname:  Name of class to parse.
  //
  // Results:
  //   return:  true => successful parsing.
  //   args:    Populated with template and constructor arguments (in order) and other miscellaneous info.
  bool parse_custom_resource(ResInfo &rinfo,const gc_string &fn,const gc_string &cname,
                             bool full_path_errors);

}

#endif

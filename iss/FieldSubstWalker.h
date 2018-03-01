//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//


#ifndef _FIELDSUBSTWALKER_H_
#define _FIELDSUBSTWALKER_H_

#include <iosfwd>

#include "helpers/gc_string.h"

#include "fe/Data.h"

namespace Opencxx {
  class Environment;
  class Ptree;
  class Walker;
}

namespace adl {

  // Given an alias field, this either writes out the accessor code or returns
  // a target field to be handled as needed.  For example, if an alias maps a constant
  // value to a field, that constant will be written.  If a different field is mapped to
  // this field, then we'll return a pointer to the original field.
  const InstrField *aliasFieldTarget(std::ostream &o,Opencxx::Walker &walker,
                                     const Instr &instr,const Instr::Field &fp, bool trans=false);
 
  void writeFieldAccessor(std::ostream &o,const InstrField &field,const gc_string &arg,
                          unsigned instr_width,bool add_sizing,const gc_string &pfx = gc_string());

  // Converts a UsedReg begin or end statement to C++ model accesses.
  void writeFieldAccessor(std::ostream &o,const Core &core,Opencxx::Ptree *expr,const Instr *instr,
                          const Instr::Alias *alias,const gc_string &arg,const gc_string &pfx = gc_string());
  
}

#endif

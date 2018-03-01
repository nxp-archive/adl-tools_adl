//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Various functions for dealing with instruction fields.
//

#ifndef _FIELD_SUPPORT_H_
#define _FIELD_SUPPORT_H_

#include <iosfwd>

#include "helpers/stringfwd.h"
#include "rnumber/RNumber.h"
#include "Types.h"

namespace adl {

  class Core;
  class InstrField;
  class RegEntry;
  class RegFile;
  class AdlTranslatingWalker;

  // Return a mask of the bits specified (inclusive).
  rnumber::RNumber makemask(unsigned start,unsigned end);

  // Return the name of the getter function of a instr field table. 
  gc_string makeInstrFldTblAccessName(const InstrField &f, const InstrField *encap=0);

  // Create disassembler helpers.
  gc_set<unsigned> instr_widths(const InstrField& f, const Core& core);
  void genEntryName(std::ostream &o,const RegEntry& entry,const RegFile &rf);
  void genDisasmHelpers(std::ostream &o,const Core &core,const gc_string &pfx = "");

  // Create instruction field accessors.  These are functions
  // which take a uint and return the field value (right justified).
  void genInstrFieldGetters(std::ostream &o,const Core &core,const gc_string &pfx = "",bool gen_proto = false,bool micro_op_only = false);
  // Create generic instruction field accessors.  These are functions
  // which take two uints and return the field value (right justified).
  // These getters are used in the transactional to access an "implementation" instruction field
  // using its interface field name and the instruction id()
  void genGenericInstrFieldGetters(std::ostream &o,const Core &core,const gc_string &pfx = "");



  // Create instruction field setters.  These are functions
  // which place a uint into the bits defined by the field, returning
  // that value.
  void genInstrFieldSetters(std::ostream &o,const Core &core,bool gen_prefix_setters,const gc_string &pfx = "");
  
  // Create instruction field clearers.  These are functions
  // which, given an instruction value, clear the specified field.
  void genInstrFieldClearers(std::ostream &o,const Core &core,const gc_string &pfx = "");

  // Create instruction field clearers.  This generates a more comprehensive list,
  // equivalent to all setters that are created.
  void genInstrFieldClearersAll(std::ostream &o,const Core &core,const gc_string &pfx);

  // TODO:Create instruction field table readers. These are functions, which given 
  // take two uints, an entry id and offset within the entry, return the field value
  void genInstrFieldTableGetters(std::ostream &o,const Core &core, const gc_string &pfx="");

  // Create instruction fields checkers. These are functions, that check argument
  // in assembly against "valid_masks" and "valid_ranges" 
  void genInstrFieldCheckers(std::ostream &o, const Core &core,const gc_string &pfx);

  // Create declarations of core data member corresponding to prefix fields. 
  // Types and other declarations go to 'o', member-variables are sent to 'mo.
  void genPrefixFieldDeclarations(std::ostream &o, std::ostream &mo,const Core &core, 
                                  const gc_string &pfx,const gc_string &cname);
      
  // Initialize data member correspoding to prefix field in core's constructor
  void genPrefixFieldCtors(const Core &core, StrList &const_inits);

  // Create instruction field setters.  These are functions
  // which place a uint into the data member corresponding to prefix field, returning
  // that value.
  void genPrefixFieldSetters(std::ostream &o, const Core &core, const gc_string &pfx);

  // Create prefix field data clearers.  These are functions
  // which set a data member to its default value
  void genPrefixFieldClearers(std::ostream &o, const Core &core, const gc_string &pfx);

  // Create instruction field accessors.  These are functions which take a uint
  // and return the data member value (right justified).  If an additional walker
  // is supplied, then it's used as a first-pass translation for the function.
  void genPrefixFieldGetters(std::ostream &o, const Core &core, const gc_string &pfx, 
                             AdlTranslatingWalker *walker,bool c_plus_plus,bool action_ok);

  // Create a call to specified field
  void getPrefixField(std::ostream &o, const InstrField &f, const gc_string &args, const gc_string &blk);

  // True if a field is modified, e.g. signed, inverted, shifted, etc.
  bool is_special(const InstrField &field);

  // Width to be used to store a modified instruction result, e.g. takes into
  // account sign extension and shifting.
  int getInstrFieldWidth(const InstrField &field,unsigned nia_width,bool orig);
  
  gc_string getInstrFieldType(const InstrField &field,unsigned nia_width,bool orig,bool use_signed);
}

#endif

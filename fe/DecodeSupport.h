// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Various functions for dealing with instruction decoder generation.
//

#ifndef _DECODE_SUPPORT_H_
#define _DECODE_SUPPORT_H_

#include <iosfwd>

#include "helpers/gc_string.h"

namespace rnumber {
  class RNumber;
}

namespace adl {

  class Core;
  class InstrField;
  class Instr;
  class ClassData;

  // Tries to find an appropriate prefix counter, else uses the default that's
  // supplied.  We also search an instruction's parent type for prefix counters.
  gc_string getPrefixIndex(const InstrField &field,const Instr &instr,const gc_string &default_index);

  // Write fetch code for prefix architecture
  void writePrefixArchFetchCode(std::ostream &o,const Core &core,const gc_string &iname,const gc_string &fname,bool prefix_table);

  // This writes out the logic which sets the special prefix variables from the
  // data in a prefix instruction.  This data is then used during subsequent
  // decoding of instructions to extend the actual size of the instruction.
  void genPrefixFieldSetup(std::ostream &o,const Core &core,const Instr &instr,const gc_string &pfx);

  // Increment the packet position for an instruction in a parallel
  // architecture.
  void packetPosIncr(std::ostream &o,const Core &core,const gc_string &pkt_pos,const Instr &instr);

  unsigned required_fetch(const rnumber::RNumber& mask);

  unsigned num_words(const Core &core,unsigned num_bits);

  // Generate the prefix counters, which can be used to index into indexed
  // prefix fields.
  void genPrefixCounters(std::ostream &o,const Core &core,ClassData &cd);

}

#endif

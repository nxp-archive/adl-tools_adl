//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// The outer driver for generating an ISS.  This basically just calls GenSimand
// opens the relevant files, etc.
//

#ifndef _GEN_DRIVER_H_
#define _GEN_DRIVER_H_

#include <string>

#include "gc/gc_cpp.h"

#include "helpers/gc_string.h"

namespace adl {

  class Data;
  class ConfigDB;
  class AnyOption;
  class GenDriver;
  class InstrPacketMap;

  struct GenDriverIF : public gc {

    GenDriverIF(const Data &data,const gc_string &tlname,const gc_string &fname,
                const gc_string &rfname,const AnyOption &options,ConfigDB &config);

    // Initiate the generation process.
    void generate();

    ConfigDB &config();
    const ConfigDB &config() const;

    // Allocate slots for all cores in the system, assuming we're creating transactional ISS.
    void doSlotAllocations();

    // Access to the instruction-slot allocation object.  This hashes core
    // pointers to allocation objects.
    const InstrPacketMap &packet_map() const;

  protected:

    GenDriver *_impl;

  };


}

#endif

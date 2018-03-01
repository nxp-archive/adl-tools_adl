//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// The main entry point to the simulator generator.  This class is in charge of
// generating the ISS source file.
//

#ifndef _GEN_SIM_H_
#define _GEN_SIM_H_

namespace adl {

  class opfxstream;
  class Data;
  class ConfigDB;
  class GenSim;
  class InstrCoreSyms;
  class SysInstrSlotAllocs;
  class InstrPacketMap;
  class PfxFileArray;

  // Main generation interface class.
  // o:      Stream used for printing the simulator source code.
  // impls:  Streams for implementation files, if an ISS will be broken up into multiple files.
  //         This may be 0.
  // data:   The ADL design database.
  // tlname: Top-level design node to use.  May be empty, in which case the first
  //         system is selected.  If no systems are found, then the first core is
  //         specified.
  // fname:  Output filename.
  // ghname: Name of Graphite header file, if applicable
  // config: The configuration database.
  struct GenSimIF {

    GenSimIF(opfxstream &o,PfxFileArray *impls,opfxstream *jo,opfxstream *gho,const Data &data,const gc_string &tlname,
             const gc_string &fname,const std::string &ghname,InstrCoreSyms &instr_core_syms,InstrPacketMap &packet_map,
             ConfigDB &config);

    // Initiate the generation process.
    void generate();

  protected:

    GenSim *_impl;

  };

}

#endif

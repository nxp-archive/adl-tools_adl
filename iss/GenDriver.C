//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// The outer driver for generating an ISS.  This basically just calls GenSim,
// and opens relevant files, etc.
//

#include <sstream>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <assert.h>
#include <memory>
#include <sys/wait.h>

#include "helpers/Macros.h"
#include "helpers/AnyOption.h"
#include "helpers/pfxstream.h"
#include "helpers/FilenameUtils.h"

#include "fe/Data.h"

#include "ConfigDB.h"
#include "GenDriver.h"
#include "GenSim.h"
#include "GenInstrPacket.h"
#include "MiscTypes.h"
#include "GenInstrPacket.h"
#include "PfxFileArray.h"

#include "adl_config.h"

using namespace std;
using namespace adl;

namespace adl {

  class GenDriver : public gc {
  public:
    GenDriver(const Data &data,const gc_string &tlname,
              const gc_string &fname,const gc_string &rfname,const AnyOption &options,ConfigDB &config) :
      _data(data),
      _tlname(tlname),
      _fname(fname),
      _real_fname(rfname),
      _options(options),
      _config(config)
    {};

  protected:
    const Data        &_data;
    gc_string          _tlname;
    gc_string          _fname;
    gc_string          _real_fname;
    const AnyOption   &_options;
    ConfigDB          &_config;
    InstrPacketMap     _packet_map;

  public:

    ConfigDB &config()
    {
      return _config;
    }

    const InstrPacketMap &packet_map() const
    {
      return _packet_map;
    }

    const ConfigDB &config() const
    {
      return _config;
    }

    bool verbose(int level = 1)
    {
      return (_config.verbose_mode() >= level);
    }

    bool multi_compile() const
    {
      return _config.num_compile_jobs() > 0;
    }

    bool graphite_support() const
    {
      return _config.graphite_support();
    }

    void create_iss(StrVect &ofns,const gc_string &ofn,const gc_string &fn,InstrCoreSyms &instr_core_syms)
    {
      ofstream out(ofn.c_str());
      if (!out) {
        RError("Unable to open output file '" << ofn << "'.");
      }

      string jfn;
      unique_ptr<opfxstream> jln;
      ofstream jout;
      if (_config.jit_support()) {
        // If we're generating for the JIT, then we have to write out the
        // instruction handler file which is then compiled by LLVM.
        jfn = dir_name(ofn.c_str())+"/"+gen_default_name(ofn.c_str(),"-jit.cc");
        jout.open(jfn.c_str());
        if (!jout) {
          RError("Unable to open output file '" << jfn << "'.");
        }
        jln.reset(new opfxstream(jout.rdbuf(),2,'{','}'));
      }

      string ghfn;
      unique_ptr<opfxstream> ghln;
      ofstream ghout;
      if (_config.graphite_support()) {
        // For handling Graphite instruction class enums.
        ghfn = dir_name(ofn.c_str())+"/"+gen_default_name(ofn.c_str(),"-graphite.h");
        ghout.open(ghfn.c_str());
        if (!ghout) {
          RError("Unable to open output file '" << ghfn << "'.");
        }
        ghln.reset(new opfxstream(ghout.rdbuf(),2,'{','}'));
      }
      
      // The line-count stream enables us to create line directives by
      // tracking our current line number.
      opfxstream oln(out.rdbuf(),2,'{','}');
      
      unique_ptr<PfxFileArray> implFiles((multi_compile()) ? new PfxFileArray(ofn.c_str(),_config.num_compile_jobs(),2,'{','}') : 0);

      // Populate the return list of all filenames that we use.
      ofns.push_back(ofn);
      if (implFiles.get()) {
        ForRange(implFiles->size(),i) {
          ofns.push_back(implFiles->filename(i).c_str());
        }
      }
      
      // Generate.
      GenSimIF gensim(oln,implFiles.get(),jln.get(),ghln.get(),_data,_tlname,fn,ghfn,instr_core_syms,_packet_map,_config);
      gensim.generate();
      out.close();
    }

    void doSlotAllocations(Core *core)
    {
      GenInstrPacketBaseIF genip(*core,_config);
      genip.doSlotAllocation();

      _packet_map[core] = genip.get_impl();
    }

    void doSlotAllocations(System *sys)
    {
      ForEach(sys->local_cores(),c_iter) {
        doSlotAllocations(c_iter->second);
      }
      ForEach(sys->local_systems(),s_iter) {
        doSlotAllocations(s_iter->second);
      }
    }

    // Iterate through all cores in the system, doing the slot allocations.
    // Store them, hashed by core pointer.  Currently, this just works with
    // cores defined at the outer scope; local cores are not processed.  Since
    // this initial processing is for the benefit of uADL (which needs slot
    // allocations up front), and uADL currently just works with a single core,
    // this is sufficient.
    void doSlotAllocations()
    {
      ForEach(_data.cores(),iter) {
        doSlotAllocations(iter->second);
      }
      ForEach(_data.systems(),iter) {
        doSlotAllocations(iter->second);
      }
    }

    void generate()
    {
      InstrCoreSyms instr_core_syms;

      cout << "Output:  " << _fname << "\n";

      gc_string output = _fname;
      gc_string fname  = (_real_fname.empty()) ? _fname : _real_fname;

      StrVect output_files, obj_files;

      create_iss(output_files,output,fname,instr_core_syms);

    }

  };

  GenDriverIF::GenDriverIF(const Data &data,const gc_string &tlname,
                           const gc_string &fname,const gc_string &rfname,
                           const AnyOption &options,ConfigDB &config) :
    _impl(new GenDriver(data,tlname,fname,rfname,options,config))
  {
  }

  ImplCall(GenDriverIF,generate);
  
  ImplCall(GenDriverIF,doSlotAllocations);

  ConfigDB &GenDriverIF::config()
  {
    return _impl->config();
  }

  const ConfigDB &GenDriverIF::config() const
  {
    return _impl->config();
  }

  const InstrPacketMap &GenDriverIF::packet_map() const
  {
    return _impl->packet_map();
  }

}

//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This is designed to be a simple example of using a barebones ISS with a
// standard reader (the DAT reader) and a standard writer (the DAT writer).  It
// also includes an external memory, which simply utilizes the ADL sparse memory
// class.
//

#include <assert.h>
#include <iostream>
#include <stdexcept>
#include <memory>

#include "helpers/Macros.h"

#include "iss/ModelInterface.h"
#include "iss/DefaultMem.h"
#include "iss/MemHandler.h"
#include "iss/MemoryLog.h"
#include "iss/LoggingMgr.h"
#include "iss/DatWriter.h"

#include "iss/DatReader.h"

using namespace std;
using namespace adl;

//
// External memory class, using ADL's sparse memory class.
//
class MyExtMemory : public MemHandler {
public:
  void reset() override
  { 
    GlobalMemory.reset();
  }

  uint64_t read64(CacheAccess ca, addr_t ra) override
  { 
    return GlobalMemory.read64(ra); 
  };
  
  uint32_t read32(CacheAccess ca, addr_t ra) override
  { 
    return GlobalMemory.read32(ra); 
  };

  uint16_t read16(CacheAccess ca, addr_t ra) override
  { 
    return GlobalMemory.read16(ra); 
  };

  uint8_t read8(CacheAccess ca, addr_t ra) override
  { 
    return GlobalMemory.read8(ra); 
  };

  void write64(addr_t ra, uint64_t v, unsigned nb) override
  { 
    GlobalMemory.write64(ra, v, nb); 
  };

  void write32(addr_t ra, uint32_t v, unsigned nb) override
  { 
    GlobalMemory.write32(ra, v, nb); 
  };

  void write16(addr_t ra, uint16_t v, unsigned nb) override
  { 
    GlobalMemory.write16(ra, v, nb); 
  };

  void write8 (addr_t ra, uint8_t v) override
  { 
    GlobalMemory.write8(ra, v); 
  };

  void readpage(CacheAccess ca, byte_t *t, size_t n, addr_t addr,addr_t crit_addr) override
  { 
    GlobalMemory.readpage(t, n, addr, crit_addr); 
  };

  void writepage(byte_t *s, size_t n, addr_t addr,addr_t crit_addr) override
  { 
    GlobalMemory.writepage(s, n, addr, crit_addr); 
  };
  
};

MyExtMemory extmem;

unique_ptr<LoggingMgr> lmgr;
unique_ptr<DatWriter>  dwriter;

IssNode *setup(const string &dat_fn,const string &out_fn,bool trace)
{
  unsigned id = 0;

  // Instantiate the model.
  IssNode *root = adliss::createTopLevelNode(id);

  // Install the external memory model.
  root->setMemHandler(&extmem);

  // Create a logging manager and install it.  This is the interface used by the
  // standard ADL writer objects and allows multiple writers to receive logging
  // data from a single core.
  lmgr.reset(createLoggingMgr(false));
  lmgr->installLoggers(root);  

  // Now create a DAT writer object if an output file was specified.  We use the
  // global memory log for communicating information about initialized memory
  // from the reader.
  if (!out_fn.empty()) {
    dwriter.reset( new DatWriter(out_fn,*root,memoryLog(),false) );
    // Register this writer with the logging manager.
    lmgr->registerLogger(dwriter.get());
  }

  // If we have a DAT file, then read it here.
  if (!dat_fn.empty()) {
    unique_ptr<Reader> dr(new DatReader(dat_fn,root));
    if (!dr->readFile()) {
      RError("Could not read DAT file '" << dat_fn << "'.");
    }
  }

  // Enable tracing, if desired.
  if (trace) {
    root->set_tracing_r();
  }

  if (dwriter) {
    // Must be called after initializing everything.
    dwriter->post_initialize();
  }

  return root;
}

int main(int argc,const char *argv[])
{
  try {
    bool trace = true;

    string dat_fn;
    string out_fn;

    for (int i = 1; i != argc; ++i) {
      if (!strcmp(argv[i],"-trace")) {
        trace = true;
      }
      else if (!strcmp(argv[i],"-no-trace")) {
        trace = false;
      }
      else if (!strcmp(argv[i],"-dat")) {
        dat_fn = argv[++i];
      }
      else if (!strcmp(argv[i],"-o")) {
        out_fn = argv[++i];
      } else {
        RError("Unknown command-line option: " << argv[i]);
      }
    }

    // Do all setup.
    unique_ptr<IssNode> root(setup(dat_fn,out_fn,trace));

    // Run the model.
    root->run();

    cout << "Simulation done.\n";

    // Finish up and write out all results.
    if (dwriter) {
      dwriter->post_finalize();
      dwriter->writeResults();
    }

    return 0;
  }
  catch (exception &err) {
    cerr << "Error:  " << err.what() << "\n\n";
    return 1;
  }
}



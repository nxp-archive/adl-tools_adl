//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#include <stdlib.h>
#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <assert.h>

#include <dlfcn.h>

#include "helpers/Macros.h"
#include "helpers/AnyOption.h"

#define ADL_HEADER
#include "iss/DefaultMem.h"
#include "iss/ModelInterface.h"
#include "iss/MemHandler.h"

#include "uadl/Timer.h"

#include "dyntest_wrapper.h"

#undef dlopen

using std::cout;
using std::cerr;
using std::string;
using std::exception;

using namespace adl;
using namespace uadl;

GlobalMemoryType *global_mem = 0;

// memory class
class mMEM_mem_t : public Memory, public MemHandler {
public:
  mMEM_mem_t(const Timer &timer, const string &name, u_int addrBandwidth = 1,
             u_int dataBandwidth = 1, u_int readLatency = 1,
             u_int writeLatency = 1, u_int maxRequest = 1) :
    Memory(timer, name, addrBandwidth, dataBandwidth, readLatency, writeLatency,
           maxRequest, true, false, false) {}

  virtual ~mMEM_mem_t() {}

  virtual void reset() { Memory::reset(); global_mem->reset(); }

  // For debug purposes, we check the access type and assert if we get what we
  // don't expect.  For example, if we get a non-debug access to the debug
  // routine, we assert.
  virtual bool getMemoryData(adl::CacheAccess type,addr_t addr, u_int size,
                             byte_t *data)
  {
    assert(type == adl::CacheLogRead || type == adl::CacheILogRead);
    return Memory::getMemoryData(type,addr,size,data);
  }

  // For our tests, we shouldn't be making any calls to set.
  virtual bool setMemoryData(addr_t addr, u_int size, const byte_t *data) 
  {
    cerr << "Debug setMemoryData used when we shouldn't be using this!\n";
    assert(0);
  }

  // We want these to skip the default implementatin, or else we'd trigger our
  // assert.
  bool readMemoryData(addr_t addr, u_int size, byte_t *data,
                      uint64_t &exception, bool cacheEnabled = false)
  { return Memory::getMemoryData(adl::CacheRead,addr,size,data); }

  bool fetchMemoryData(addr_t addr, u_int size, byte_t *data,
                       uint64_t &exception, bool cacheEnabled = false)
  { return Memory::getMemoryData(adl::CacheIFetch,addr,size,data); }

  bool writeMemoryData(addr_t addr, u_int size, const byte_t *data,
                       uint64_t &exception, bool cacheEnabled = false)
  { return Memory::setMemoryData(addr,size,data); }

  virtual uint64_t read64(CacheAccess ca, addr_t ra)
    { return global_mem->read64(ra); };
  virtual uint32_t read32(CacheAccess ca, addr_t ra)
    { return global_mem->read32(ra); };
  virtual uint16_t read16(CacheAccess ca, addr_t ra)
    { return global_mem->read16(ra); };
  virtual uint8_t read8(CacheAccess ca, addr_t ra)
    { return global_mem->read8(ra); };
  virtual void write64(addr_t ra, uint64_t v, unsigned nb)
    { global_mem->write64(ra, v, nb); };
  virtual void write32(addr_t ra, uint32_t v, unsigned nb)
    { global_mem->write32(ra, v, nb); };
  virtual void write16(addr_t ra, uint16_t v, unsigned nb)
    { global_mem->write16(ra, v, nb); };
  virtual void write8 (addr_t ra, uint8_t v)
    { global_mem->write8(ra, v); };
  virtual void readpage(CacheAccess ca, byte_t *t, size_t n, addr_t addr,addr_t crit_addr)
  { global_mem->readpage(t, n, addr,crit_addr); };
  virtual void writepage(byte_t *s, size_t n, addr_t addr,addr_t crit_addr)
  { global_mem->writepage(s, n, addr, crit_addr); };

protected:
  virtual byte_t *hasData(addr_t addr, u_int &size) const
  { return global_mem->getPage(addr) + GlobalMemoryType::pageOffset(addr); }

};


DynWrapper &loadLib(const string &lib)
{
  void *handle = 0;

  handle = dlopen(lib.c_str(),RTLD_LAZY);

  if(handle == 0) {
    RError(string("lt_dlopen(")+lib+") failed:  "+dlerror()+"\n");
  }

  MakeWrapper func = (MakeWrapper)dlsym(handle, "make_wrapper");
  if (!func) {
    RError(string("lt_dlsym() failed: ") + dlerror() + "\n");
  }  
  return (*func)();
}


// main function
int main(int argc, const char *argv[])
{
  // Note: Using our own option processing here in order to avoid link errors
  // when built to dynamically load a model.
  AnyOption options;

  try {
    options.setFlag("trace-memory", "tm", "enable memory activity tracing");
    options.setFlag("trace-pipeline", "tp", "enable pipeline tracing");
    options.setFlag("trace-stall", "ts", "enable pipeline stall tracing");
    options.setFlag("trace-operand", "to", "enable instruction operand tracing");
    options.setFlag("trace-target", "tt", "enable instruction target tracing");
    options.setFlag("trace-all", "ta", "enable all tracing information");
    options.setOption("output", "o", "specify an output file");
    options.setOption("start-time", "s", "specify a start time");
    options.setOption("load-lib", "specify a core library to load (only applicable if built with USE_DLOPEN");
    options.useCommandArgs(argc, argv);

    options.useCommandArgs(argc, argv);

    // Allow unknown options for now- they'll be checked again in init().
    options.errorOnUnknowns(false);

    // Parse command line options and arguments for uADL-specific options.
    // Further processing will be done by init().
    if (!options.processCommandLine()) {
      RError("Bad command-line.");
    }

    // parse command line options and arguments
    const bool logAll = options.getFlag("trace-all", false);

    string lib = options.getOption("load-lib");
    
    if (lib.empty()) {
      RError("No shared object specified.  Use the --load-lib=<library> option to specify this.");
    }

    const ttime_t startTime = options.getOption("start-time", 0);

    DynWrapper &dw = loadLib(lib);

    global_mem = &dw.get_memory();

    // instantiate the memory
    Timer timer;
    mMEM_mem_t mMEM(timer, "memory");

    // instantiate the microarchitecture
    unsigned id = 0;
    UadlArch &arch = *dw.createArch("core", id, &mMEM, startTime);
    arch.setIssMemHandler(&mMEM);

    if (logAll) {
      arch.set_tracing();
    } else {
      arch.clear_tracing();
    }

    // ADL initialization (input, output, etc.).
    dw.init(&arch.iss(),argc,argv);

    // run model
    while (arch.is_active()) {
      // memory pre-processing
      mMEM.preCycle();
      // core model activity
      arch.proceed();
      // memory post-processing
      mMEM.postCycle();
      // increment memory timer to be synchronous with core timer
      timer.incrTime();
    }

    cout << "time " << arch.currTime() << ": simulation terminated.  "
         << arch.issueCount() << " instructions issued.\n";

    dw.handleFinalState(true, arch.iss());
    // close the microarchitecture
    dw.finalize();
  }
  catch (exception &err) {
    cerr << "\nError:  " << err.what() << "\n\n";
    return 1;
  }
  return 0;
}


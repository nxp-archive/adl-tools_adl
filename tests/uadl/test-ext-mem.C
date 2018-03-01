//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This is a simple driver program to test uADL with an external memory.
//

#include <stdlib.h>
#include <string>
#include <sstream>
#include <iostream>
#include <stdexcept>
#include <assert.h>

#include "helpers/Macros.h"
#include "helpers/AnyOption.h"

// Note: We actually want to instantiate the memory here, so it's being included
// without ADL_HEADER defined.
#include "iss/DefaultMem.h"

#include "iss/ModelInterface.h"
#include "iss/SimInterface.h"
#include "iss/ExecSupport.h"
#include "iss/MemHandler.h"

#include "uadl/Timer.h"
#include "uadl/UadlMemory.h"
#include "uadl/UadlArchIf.h"

using std::cout;
using std::cerr;
using std::string;
using std::exception;

using namespace adl;
using namespace uadl;

// Our external memory class
class MyExtMemory : public Memory, public MemHandler {
public:
  MyExtMemory(const Timer &timer, const string &name, addr_t testExcp,
              u_int addrBandwidth = 1, u_int dataBandwidth = 1, u_int readLatency = 1,
            u_int writeLatency = 1, u_int maxRequest = 1) :
    Memory(timer, name, addrBandwidth, dataBandwidth, readLatency, writeLatency, maxRequest, true, false, false),
    _iss(0),
    _testExcp(testExcp) 
  {}

  virtual ~MyExtMemory() {}

  virtual void reset() { Memory::reset(); GlobalMemory.reset(); }

  void setISS(adl::IssNode *ISS);

  virtual uint64_t read64(CacheAccess ca, addr_t ra)
  { return GlobalMemory.read64(ra); };
  virtual uint32_t read32(CacheAccess ca, addr_t ra)
  { return GlobalMemory.read32(ra); };
  virtual uint16_t read16(CacheAccess ca, addr_t ra)
  { return GlobalMemory.read16(ra); };
  virtual uint8_t read8(CacheAccess ca, addr_t ra)
  { return GlobalMemory.read8(ra); };
  virtual void write64(addr_t ra, uint64_t v, unsigned nb)
  { prefix(ra); GlobalMemory.write64(ra, v, nb); };
  virtual void write32(addr_t ra, uint32_t v, unsigned nb)
  { prefix(ra); GlobalMemory.write32(ra, v, nb); };
  virtual void write16(addr_t ra, uint16_t v, unsigned nb)
  { prefix(ra); GlobalMemory.write16(ra, v, nb); };
  virtual void write8 (addr_t ra, uint8_t v)
  { prefix(ra); GlobalMemory.write8(ra, v); };
  virtual void readpage(CacheAccess ca, byte_t *t, size_t n, addr_t addr,addr_t crit_addr)
  { GlobalMemory.readpage(t, n, addr,crit_addr); };
  virtual void writepage(byte_t *s, size_t n, addr_t addr,addr_t crit_addr)
  { prefix(addr); GlobalMemory.writepage(s, n, addr, crit_addr); };

 
protected:
  void prefix(addr_t addr, u_int n = 1);

  virtual byte_t *hasData(addr_t addr, u_int &size) const
  { return GlobalMemory.getPage(addr) + GlobalMemoryType::pageOffset(addr); }

  adl::IssNode *_iss;
  uint64_t      _ProtectionFault;

  addr_t        _testExcp; // true if testing Protection Fault exception
};

void MyExtMemory::prefix(addr_t addr, u_int n)
{
  // throw a ProtectionFault exception if ra matches.
  if (_testExcp && _iss && addr == _testExcp) {
    _iss->genExceptionImm(_ProtectionFault);
  }
}

void MyExtMemory::setISS(adl::IssNode *iss)
{
  _iss = iss;
  _ProtectionFault = 0;

  auto ei =  iss->getExceptionInfo();

  ForEach(ei, i) {
    if (i->_name == "ProtectionFault") {
      _ProtectionFault = i->_id;
    }
  }

  if (!_ProtectionFault) {
    // turn off exception testing
    _testExcp = 0;
  }
}




int main(int argc, const char *argv[])
{
  try {
    options().setFlag("trace-all", "ta", "enable all tracing information");
    options().setOption("output", "o", "specify an output file");
    options().setOption("start-time", "s", "specify a start time");
    options().setOption("test-exception", "test Protection Fault exception");
    options().useCommandArgs(argc, argv);

    // Allow unknown options for now- they'll be checked again in init().
    options().errorOnUnknowns(false);

    // Parse command line options and arguments for uADL-specific options().
    // Further processing will be done by init().
    if (!options().processCommandLine()) {
      RError("Bad command-line.");
    }

    const addr_t testExcp = options().getOption("test-exception", 0);

    // parse command line options and arguments
    const bool logAll = options().getFlag("trace-all", false);

    const ttime_t startTime = options().getOption("start-time", 0);

    // instantiate the memory
    Timer timer;
    MyExtMemory mMEM(timer, "memory", testExcp);

    // Instantiate the microarchitecture (test setExtMemory() later)
    unsigned id = 0;
    UadlArch &arch = *createArch("core", id, 0, startTime);

    if (logAll) {
      arch.set_tracing();
    } else {
      arch.clear_tracing();
    }

    arch.setIssMemHandler(&mMEM);
    arch.setExtMemory(mMEM);
    arch.setLogStream(&std::cerr);

    // ADL initialization (input, output, etc.).
    init(&arch.iss(),argc,argv);

    mMEM.setISS(&arch.iss());

    // run model
    while (true) {
      arch.preRun();
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

      handleFinalState(true, arch.iss());

      // have more tests?
      if (!adl::haveMoreInput()) break;

      // reset simulation
      resetSimulation(arch.iss());
      arch.reset();

      if (!adl::readNextInput(arch.iss())) break;
    }

    // close the microarchitecture
    adl::finalize();
  }
  catch (exception &err) {
    cerr << "\nError:  " << err.what() << "\n\n";

    return 1;
  }

  return 0;
}

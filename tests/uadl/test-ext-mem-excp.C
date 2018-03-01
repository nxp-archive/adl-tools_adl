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
#include <limits>
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
#include "uadl/UadlArchIf.h"
#include "uadl/UadlMemory.h"

using std::cout;
using std::cerr;
using std::string;
using std::exception;

using namespace std;
using namespace adl;
using namespace uadl;


// memory class
class MyMem : public MemHandler {
public:
  MyMem(unsigned latency, addr_t testExcp) :
    _iss(0), _testExcp(testExcp), _latency(latency) {}

  virtual ~MyMem() {}

  // set latency to 1 for testing memory latency feedback
  virtual uint64_t read64(CacheAccess ca,addr_t ra)
    { prefix(ra); return GlobalMemory.read64(ra); }
  virtual uint32_t read32(CacheAccess ca,addr_t ra)
    { prefix(ra); return GlobalMemory.read32(ra); }
  virtual uint16_t read16(CacheAccess ca,addr_t ra)
    { prefix(ra); return GlobalMemory.read16(ra); }
  virtual uint8_t  read8(CacheAccess ca,addr_t ra)
    { prefix(ra); return GlobalMemory.read8(ra); }
  virtual void     write64(addr_t ra, uint64_t v, unsigned nb)
    { prefix(ra); GlobalMemory.write64(ra,v,nb); }
  virtual void     write32(addr_t ra, uint32_t v, unsigned nb)
    { prefix(ra); GlobalMemory.write32(ra,v,nb); }
  virtual void     write16(addr_t ra, uint16_t v, unsigned nb)
    { prefix(ra); GlobalMemory.write16(ra,v,nb); }
  virtual void     write8 (addr_t ra, uint8_t  v)
    { prefix(ra); GlobalMemory.write8(ra,v); }
  virtual void     readpage(CacheAccess ca,byte_t *t,size_t n,addr_t addr,addr_t crit_addr)
  { prefix(addr, n / sizeof(uint64_t)); GlobalMemory.readpage(t,n,addr,crit_addr); }
  virtual void     writepage(byte_t *s,size_t n,addr_t addr,addr_t crit_addr)
  { prefix(addr, n / sizeof(uint64_t)); GlobalMemory.writepage(s,n,addr,crit_addr); }
  virtual void     fillpage(unsigned char c,size_t n,addr_t addr)
  { prefix(addr, n / sizeof(uint64_t)); GlobalMemory.fillpage(c,n,addr); }

  virtual void     reset() { GlobalMemory.reset(); }
  
  void setISS(adl::IssNode *ISS);

protected:
  virtual byte_t *hasData(addr_t addr, u_int &size) const
    { return GlobalMemory.getPage(addr) + GlobalMemoryType::pageOffset(addr); }

private:
  void prefix(addr_t addr, u_int n = 1);

  adl::IssNode *_iss;
  uint64_t      _ProtectionFault;

  addr_t        _testExcp; // true if testing Protection Fault exception
  const u_int   _latency;
};

void MyMem::setISS(adl::IssNode *iss)
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


void MyMem::prefix(addr_t addr, u_int n)
{
  // throw a ProtectionFault exception if ra == 0x1000
  if (_testExcp && _iss && addr == _testExcp)
    _iss->genExceptionImm(_ProtectionFault);

  // set memory latency
  set_latency(_latency);

  for (u_int i = 1; i < n; i++) add_latency(_latency);
}



// main function
int main(int argc, const char *argv[])
{
  try {
    options().setFlag("trace-all", "ta", "enable all tracing information");
    options().setOption("test-exception", "test Protection Fault exception");
    options().setOption("output", "o", "specify an output file");
    options().setOption("start-time", "s", "specify a start time");
    options().setOption("latency", "specify memory read/write latency");
    options().setOption("l1d-num-sets", "specify the number of sets in the L1d cache");

    options().useCommandArgs(argc, argv);

    // Allow unknown options for now- they'll be checked again in init().
    options().errorOnUnknowns(false);

    // Parse command line options and arguments for uADL-specific options().
    // Further processing will be done by init().
    if (!options().processCommandLine()) {
      RError("Bad command-line.");
    }

    // parse command line options and arguments
    const bool logAll = options().getFlag("trace-all", false);

    // memory options
    const addr_t testExcp = options().getOption("test-exception", 0);
    const u_int latency = options().getOption("latency", 1);

    const ttime_t startTime = options().getOption("start-time", 0);

    // instantiate the memory
    MyMem mMEM(latency, testExcp);

    // instantiate the microarchitecture
    unsigned id = 0;
    UadlArch &arch = *createArch("core", id, 0, startTime);

    if (logAll) {
      arch.set_tracing();
    } else {
      arch.clear_tracing();
    }

    // Test that we properly re-evaluate necessary parameters after this call
    // (should re-evaluate because of the set_dyn_parm call).
    arch.preRun();

    const string l1dnumsets = "L1d-cache-num-sets";
    if (options().haveOption("l1d-num-sets")) {
      arch.set_dyn_parm(l1dnumsets,options().getOption("l1d-num-sets",0));
      cout << "L1d number of sets:  " << arch.iss().get_dyn_parm(l1dnumsets) << std::endl;
    } else {
      arch.forceHadReset();
    }

    mMEM.setISS(&arch.iss());
    arch.setIssMemHandler(&mMEM);

    // ADL initialization (input, output, etc.).
    init(&arch.iss(),argc,argv);

    // run model
    arch.run(numeric_limits<ttime_t>::max());

    cout << "time " << arch.currTime() << ": simulation terminated.  "
         << arch.issueCount() << " instructions issued.\n";

    handleFinalState(true, arch.iss());
    // close the microarchitecture
    adl::finalize();
  }
  catch (exception &err) {
    cerr << "\nError:  " << err.what() << "\n\n";
    return 1;
  }
  return 0;
}

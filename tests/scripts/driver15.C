//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

// This tests several scenarios:
//
// Scenario 1:
//
//  This tests a pretty specific bug: When using the decode cache, if an external
//  exception is raised which is placed onto the pending flag, then when the
//  exception is re-enabled, and thus sets the exception flag, we want to make
//  sure that we handle the exception when we're exiting the decode-cache
//  simulation loop due to exceeding the td threshold.
//
// Scenario 2:
//
//  If a hot block has a branch-and-link to a location which has a breakpoint,
//  and the td quantum is 1, then we went up returning JitRetCode and not
//  JitNopCode, so we have to check for the breakpoint in JitRetCode or else we
//  ignore the breakpoint.
//

#include <string>
#include <memory>
#include <iostream>

#include "rnumber/RNumber.h"

#include "helpers/AnyOption.h"
#include "iss/SimInterface.h"

// Note: We actually want to instantiate the memory here, so it's being included
// without ADL_HEADER defined.
#include "iss/DefaultMem.h"

#include "iss/ModelInterface.h"
#include "iss/MemHandler.h"
#include "iss/LoggingMgr.h"

using namespace std;
using namespace adl;
using namespace rnumber;

struct MyLogger : public LogParentIface, public LoggingIface {

  addr_t _instr_ea;

  LoggingIface *register_core(IssCore &core)
  {
    return this;
  }

  void logBreakpoint(addr_t ea,unsigned handle,PtBase *payload)
  {
    throw SimInterrupt();
  }

};

bool LogPageAccesses = false;

// memory class
class MyExtMemory : public MemHandler {
public:
  virtual void reset() { GlobalMemory.reset(); }

  virtual uint64_t read64(CacheAccess ca, addr_t ra)
    { return GlobalMemory.read64(ra); };
  virtual uint32_t read32(CacheAccess ca, addr_t ra)
    { return GlobalMemory.read32(ra); };
  virtual uint16_t read16(CacheAccess ca, addr_t ra)
    { return GlobalMemory.read16(ra); };
  virtual uint8_t read8(CacheAccess ca, addr_t ra)
    { return GlobalMemory.read8(ra); };
  virtual void write64(addr_t ra, uint64_t v, unsigned nb)
    { GlobalMemory.write64(ra, v, nb); };
  virtual void write32(addr_t ra, uint32_t v, unsigned nb)
    { GlobalMemory.write32(ra, v, nb); };
  virtual void write16(addr_t ra, uint16_t v, unsigned nb)
    { GlobalMemory.write16(ra, v, nb); };
  virtual void write8 (addr_t ra, uint8_t v)
    { GlobalMemory.write8(ra, v); };
  virtual void readpage(CacheAccess ca, byte_t *t, size_t n, addr_t addr,addr_t crit_addr)
  { 
    cout << "readpage:  0x" << hex << addr << ":  size " << dec << n << '\n';
    GlobalMemory.readpage(t, n, addr, crit_addr); 
  };

  virtual void writepage(byte_t *s, size_t n, addr_t addr,addr_t crit_addr)
  { 
    cout << "writepage:  0x" << hex << addr << ":  size " << dec << n << '\n';
    GlobalMemory.writepage(s, n, addr, crit_addr); 
  };
  
};

unsigned get_exception_id(IssNode &node,const string &name)
{
  const ExceptionInfos &einfo = node.getExceptionInfo();
  ForEach(einfo,i) {
    if (i->_name == name) {
      return i->_id;
    }
  }
  RError("Unknown exception name " << name);
}

 // Tests scenario 1.
void run_test1(IssNode &node)
{
  unsigned extid = get_exception_id(node,"External");

  // Step the model by one instruction using the decode-cache simulation loop.
  node.set_td_threshold(1);
  for (int i = 0; i != 100; ++i) {    
    node.set_td_counter(0);
    node.run(SimDc);
  }
  // This will be placed in pending, due to MSR.EE == 0.
  node.genExceptions(extid);
  // Enable MSR.EE, which sets the exception flag.
  node.writeReg("MSR",0,0x00040000);
  // Continue to cycle the system.  We always exit, but the test fails if the
  // loop is never exited.
  for (int i = 0; i != 100; ++i) {    
    node.set_td_counter(0);
    node.run(SimDc);
  }
}

 // Tests scenario 2.
void run_test2(IssNode &node)
{
  MyLogger myLogger;

  registerLogger(&myLogger);

  TranslationParms::set_dc_bb_hot_threshold(1);

   // Step the model by one instruction using the decode-cache simulation loop.
  node.set_td_threshold(1);
  for (int i = 0; i != 10; ++i) {    
    node.set_td_counter(0);
    if (node.run(SimDc) == BREAK) {
      // Indicate that we got the breakpoint.
      uint64_t x;
      node.getReg("GPR10",0,x);
      node.setReg("GPR10",0,x+1);
    }
  }

}

// Tests scenario 3.  This just simply runs the simulation.
void run_test3(IssNode &node)
{
  node.run();
}

void run(IssNode &node)
{
  unsigned test = options().getOption("test",1);

  switch (test) {
  case 1:
    run_test1(node);
    break;
  case 2:
    run_test2(node);
    break;
  case 3:
    run_test3(node);
    break;
  default:
    RError("Unknown test scenario:  " << test);
  }
}

int main(int argc,const char *argv[])
{
  try {
    options().setOption("test","Specify which test scenario to run");
    options().useCommandArgs(argc, argv);

    // Allow unknown options for now- they'll be checked again in init().
    options().errorOnUnknowns(false);

    // Parse command line options and arguments for uADL-specific options.
    // Further processing will be done by init().
    if (!options().processCommandLine()) {
      RError("Bad command-line.");
    }   

    options().useCommandArgs(argc,argv);

    MyExtMemory mMem;

    // instantiate the microarchitecture
    unsigned id = 0;
    auto_ptr<IssNode> root(ISS_NAMESPACE::createTopLevelNode(id));

    root->setMemHandler(&mMem);

    init(root.get(), argc, const_cast<const char**>(argv), 0, false);

    run(*root);

    handleFinalState(true, *root);
    finalize();

    cout << "Done.\n";
  }
  catch (exception &err) {
    cerr << "\nError:  " << err.what() << "\n\n";
    return 1;
  }

  return 0;
}


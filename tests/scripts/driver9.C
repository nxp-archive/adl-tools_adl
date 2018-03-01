//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This is a simple driver program that can link against a bare-bones ISS
// that does not have the standalone infrastructure.  It's like driver2, but
// tests the DmiMemHandler class.
//
// You may define IssNamespace to specify an alternate namespace to be included.
//

#include <assert.h>
#include <iostream>
#include <iomanip>
#include <stdarg.h>
#include <sstream>
#include <stdexcept>

# include "rnumber/RNumber.h"

#ifndef IssNamespace
# define IssNamespace DEFAULT_ISS_NAMESPACE
#endif

#define ISS_NAMESPACE IssNamespace

#include "helpers/Macros.h"
#include "helpers/AnyOption.h"

#include "iss/ModelInterface.h"
#include "iss/Memory.h"
#include "iss/MemHandler.h"
#include "iss/LoggingMgr.h"
#include "iss/SimInterface.h"

using namespace std;
using namespace adl;

bool Trace = false;
bool MemLogging = false;

// We include this so that we get ADL's sparse memory object, which we use for
// own memory.
#include "iss/DefaultMem.h"

struct MyMem : public adl::MemHandler {

  MyMem(IssNode *n) : _node(n) {};

  void set_node(IssNode *n) { _node = n; };

  void setLatencies(unsigned r,unsigned w) 
  {
    _read_latency = r;
    _write_latency = w;
  }

  uint64_t read64(CacheAccess ca,addr_t ra)
  {
    log_access(ca,ra,64);
    byte_t *page = 0;
    uint64_t tmp = mem.read64(ra,page);
    _node->update_dmi_cache_entry(ca,ra,page);
    return tmp;
  }

  uint32_t read32(CacheAccess ca,addr_t ra)
  {
    log_access(ca,ra,32);
    byte_t *page = 0;
    uint32_t tmp = mem.read32(ra,page);
    _node->update_dmi_cache_entry(ca,ra,page);
    return tmp;
  }

  uint16_t read16(CacheAccess ca,addr_t ra)
  {
    log_access(ca,ra,16);
    byte_t *page = 0;
    uint16_t tmp = mem.read16(ra,page);
    _node->update_dmi_cache_entry(ca,ra,page);
    return tmp;
  }

  uint8_t  read8(CacheAccess ca,addr_t ra)
  {
    log_access(ca,ra,8);
    byte_t *page = 0;
    uint8_t tmp = mem.read8(ra,page);
    _node->update_dmi_cache_entry(ca,ra,page);
    return tmp;
  }

  void write64(addr_t ra, uint64_t v, unsigned nb)
  {
    log_access(CacheWrite,ra,64);
    byte_t *page = 0;
    mem.write64(ra,v,nb,page);
    _node->update_dmi_cache_entry(CacheWrite,ra,page);
  }

  void write32(addr_t ra, uint32_t v, unsigned nb)
  {
    log_access(CacheWrite,ra,32);
    byte_t *page = 0;
    mem.write32(ra,v,nb,page);
    _node->update_dmi_cache_entry(CacheWrite,ra,page);
  }

  void write16(addr_t ra, uint16_t v, unsigned nb)
  {
    log_access(CacheWrite,ra,16);
    byte_t *page = 0;
    mem.write16(ra,v,nb,page);
    _node->update_dmi_cache_entry(CacheWrite,ra,page);
  }

  void write8 (addr_t ra, uint8_t  v)
  {
    log_access(CacheWrite,ra,8);
    byte_t *page = 0;
    mem.write8(ra,v,page);
    _node->update_dmi_cache_entry(CacheWrite,ra,page);
  }

  void readpage(CacheAccess ca,byte_t *t,size_t n,addr_t addr,addr_t crit_addr)
  {
    log_access(ca,addr,n);
    byte_t *page = 0;
    mem.readpage(t,n,addr,crit_addr,page);
    _node->update_dmi_cache_entry(ca,addr,page);
  }

  void writepage(byte_t *s,size_t n,addr_t addr,addr_t crit_addr)
  {
    log_access(CacheWrite,addr,n);
    byte_t *page = 0;
    mem.writepage(s,n,addr,crit_addr,page);
    _node->update_dmi_cache_entry(CacheWrite,addr,page);
  }

  void reset()
  {
    mem.reset();
  }

  void log_access(CacheAccess ca,addr_t ra,int size)
  {
    // Add on latencies, if relevant.
    switch (ca) {
    case CacheWrite:
      _node->addLatency(_write_latency);
      break;
    case CacheRead:
      _node->addLatency(_read_latency);
      break;
    default:
      ;
    }

    if (MemLogging) {
      switch (ca) {
      case CacheIFetch:
        cout << "# Instruction read:  0x" << hex << ra << "\n";
        break;
      case CacheWrite:
        cout << "# Data write (" << dec << size << " bits):  0x" << hex << ra << "\n";
        break;
      case CacheRead:
        cout << "# Data read (" << dec << size << " bits):  0x" << hex << ra << "\n";
        break;
      case CacheLogRead:
        cout << "# Data log read (" << dec << size << " bits):  0x" << hex << ra << "\n";
        break;
      case CacheILogRead:
        cout << "# Instruction log read:  0x" << hex << ra << "\n";
        break;
      default:
        cout << "# Type " << dec << ca << " access (" << size << " bits):  0x" << hex << ra << "\n";
        break;
      }
    }
  }

private:
  IssNode *_node;
  
  // For time-tagged ISSs on a DMI miss.
  unsigned _read_latency, _write_latency;
};


int main(int argc,const char *argv[])
{
  try {
    options().addUsage(":  [options] <dat file>");
    options().setFlag  ("help","h",    "Display help.");
    options().setFlag  ("trace","t",   "Enable tracing.");
    options().setFlag  ("jit",         "Enable JIT execution.");
    options().setFlag  ("dc",          "Enable decode-cache execution.");
    options().setFlag  ("log-mem",     "Log DMI-miss memory accesses to stdout.");
    options().setFlag  ("lax","l",     "Lax mode.");
    options().setFlag  ("debug",       "Debug mode.");
    options().setFlag  ("has-dmi",     "Make sure we have DMI enabled.");
    options().setOption("output","o",  "Specify an output file.");
    options().setOption("read-latency","Specify a read latency (time-tagged ISSs only).");
    options().setOption("write-latency","Specify a write latency (time-tagged ISSs only).");
    options().setOption("dc-hot-count", "Specify threshold at which point we consider a block to be 'hot'\nand byte-compile it for the decode-cache.");
    options().useCommandArgs(argc,argv);
    if (argc < 2 || !options().processCommandLine()) {
      throw runtime_error("Bad command-line");
    }

    Trace      = options().getFlag("trace",Trace);
    LaxMode    = options().getFlag("lax",LaxMode);
    MemLogging = options().getFlag("log-mem",MemLogging);
    DebugMode  = options().getFlag("debug",DebugMode);

    SimType st = (options().getFlag("jit",false)) ? SimJit : ((options().getFlag("dc",false)) ? SimDc : SimNormal);

    if (options().getFlag("h")) {
      options().printUsage();
      return 0;
    };

    unsigned id = 0;
    IssNode *root = adliss::createTopLevelNode(id);
    assert(root);

    MyMem mem(root);
    root->setMemHandler(&mem);

    // Only applicable for time-tagged ISSs.
    unsigned rl = options().getOption("read-latency",0);
    unsigned wl = options().getOption("write-latency",0);
    if (rl || wl) assert(root->isTimeTagged());
    mem.setLatencies(rl,wl);
    root->setDmiLatencies(rl,wl);

    // Make sure that it's enabled and that we will report having it.
    if (options().getFlag("has-dmi",false)) {
      assert(root->has_dmi_cache());
    }

    if (options().haveOption("dc-hot-count")) {
      TranslationParms::set_dc_bb_hot_threshold(options().getOption("dc-hot-count",(int)TranslationParms::dc_bb_hot_threshold()));
    }

    installLoggers(root);
  
    if (const char *n = options().getArgv(0)) {
      readInput(root,n);
    } else {
      throw runtime_error("No input file specified.");
    }

    addWriter(createWriter(*root,options().getOption("o")));

    if (Trace) {
      startLogging(*root);
    } else {
      dumpInitialState();
    }

    runSim(*root,st);

    if (Trace) {
      stopLogging(*root);
    } else {
      dumpFinalState();
    }

    removeAllWriters();
    cout << "Instructions issued:  " << dec << root->instr_count() << "\n";

    if (root->isTimeTagged()) {
      // Just checking- we should see a non-zero final time if we executed any
      // instructions.
      if (root->instr_count() > 0) {
        assert(root->finalTime() > 0);
      }
      cout << "Completion time    :  " << dec << root->finalTime() << "\n";
    }

    cout << "\n";
  }
  catch (exception &err) {
    cout << err.what() << "\n";
    options().printUsage();
    return 1;
  }
}

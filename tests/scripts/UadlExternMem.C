//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This demonstrates a very simple external memory designed to work with a uADL
// safe-mode model.  Writes to the addresses 0xa000-0xaffff actually act as
// accumulator writes, adding their values to what's already there.
//

#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>

#include "helpers/Macros.h"
#include "helpers/BasicTypes.h"

#include "iss/DefaultMem.h"
#include "iss/MemHandler.h"
#include "iss/ModelInterface.h"

#include "uadl/UadlPlugin.h"
#include "uadl/UadlArch.h"
#include "uadl/UadlMemory.h"

using namespace std;
using namespace adl;
using namespace uadl;

static uint32_t AccumMemStart = 0xa000;
static uint32_t AccumMemEnd   = 0xafff;

// The special memory handler here treats memory in the range [start,end) as
// accumulator memory if we're dealing with aligned 32-bit writes.  Memory is
// cached in the node's DMI cache if the access falls outsided of that range.
struct MyMem : public Memory, public MemHandler {

  MyMem(Timer &timer, const string &name, u_int addrBandwidth = 1,
        u_int dataBandwidth = 1, u_int readLatency = 1,
        u_int writeLatency = 1, u_int maxRequest = 1) :
    Memory(timer, name, addrBandwidth, dataBandwidth, readLatency, writeLatency,
           maxRequest, true, false, false),
    _node(0), 
    _start(0), 
    _end(0),
    _timer(timer)
  {};

  void postCycle() 
  {
    _timer.incrTime();
  }

  void setup(IssNode *node,addr_t start,addr_t end)
  {
    _node = node;
    _start = start;
    _end = end;
  }

  uint64_t read64(CacheAccess ca,addr_t ra)
  {
    byte_t *page = 0;
    uint64_t tmp = GlobalMemory.read64(ra,page);
    update_dmi_cache(ca,ra,page);
    return tmp;
  }

  uint32_t read32(CacheAccess ca,addr_t ra)
  {
    byte_t *page = 0;
    uint32_t tmp = GlobalMemory.read32(ra,page);
    update_dmi_cache(ca,ra,page);
    return tmp;
  }

  uint16_t read16(CacheAccess ca,addr_t ra)
  {
    byte_t *page = 0;
    uint16_t tmp = GlobalMemory.read16(ra,page);
    update_dmi_cache(ca,ra,page);
    return tmp;
  }

  uint8_t  read8(CacheAccess ca,addr_t ra)
  {
    byte_t *page = 0;
    uint8_t tmp = GlobalMemory.read8(ra,page);
    update_dmi_cache(ca,ra,page);
    return tmp;
  }

  void write64(addr_t ra, uint64_t v, unsigned nb)
  {
    byte_t *page = 0;
    GlobalMemory.write64(ra,v,nb,page);
    update_dmi_cache(CacheWrite,ra,page);
  }

  void write32(addr_t ra, uint32_t v, unsigned nb)
  {
    byte_t *page = 0;
    if (is_special_write(ra)) {
      update_accumulators(ra,v,nb);
    } else {
      GlobalMemory.write32(ra,v,nb,page);
    }
    update_dmi_cache(CacheWrite,ra,page);
  }

  void write16(addr_t ra, uint16_t v, unsigned nb)
  {
    byte_t *page = 0;
    GlobalMemory.write16(ra,v,nb,page);
    update_dmi_cache(CacheWrite,ra,page);
  }

  void write8 (addr_t ra, uint8_t  v)
  {
    byte_t *page = 0;
    GlobalMemory.write8(ra,v,page);
    update_dmi_cache(CacheWrite,ra,page);
  }

  void readpage(CacheAccess ca,byte_t *t,size_t n,addr_t addr,addr_t crit_addr)
  {
    byte_t *page = 0;
    GlobalMemory.readpage(t,n,addr,crit_addr,page);
    update_dmi_cache(CacheWrite,addr,page);
  }

  void writepage(byte_t *s,size_t n,addr_t addr,addr_t crit_addr)
  {
    byte_t *page = 0;
    GlobalMemory.writepage(s,n,addr,crit_addr,page);
    update_dmi_cache(CacheWrite,addr,page);
  }

  void reset()
  {
    Memory::reset();
    GlobalMemory.reset();
  }

protected:
  void update_dmi_cache(CacheAccess ca,addr_t ra,byte_t *page)
  {
    if (_node && page) {
      _node->update_dmi_cache_entry(ca,ra,page);
    }
  }

  bool is_special_mem(addr_t ra)
  {
    return (ra >= _start && ra < _end);
  }

  // We only accept special writes to aligned addresses.
  bool is_special_write(addr_t ra)
  {
    return (_node && is_special_mem(ra) && ( (ra & WordMask) == 0));
  }

  void update_accumulators(addr_t ra,uint32_t v,unsigned nb)
  {
    GlobalMemory.write32(ra,(GlobalMemory.read32(ra) + v),nb);
  }

  virtual byte_t *hasData(addr_t addr, u_int &size) const
  { 
    return GlobalMemory.getPage(addr) + GlobalMemoryType::pageOffset(addr); 
  }

  IssNode *_node;
  addr_t   _start;  // Starting address for accumulator functionality (inclusive).
  addr_t   _end;    // Ending address for accumulator functionality (exclusive).
  Timer   &_timer;  // Access to the timer so that we can increment its time.
};

static Timer timer;
static MyMem memhandler(timer,"memory");

// This class is the service provider.  For a request for a writer of type
// 'icnt', it returns an instruction-count object.  It also displays a small
// banner at load-timee so that we know that the plugin was installed.
struct ExternMemPlugin : public UadlPlugin {

  // Called when the plugin is installed.
  virtual void init(AnyOption &options) 
  {
    cout << "Loaded the uadl external-memory plugin.\n";
  };  

  // Called for installing external memories.
  virtual void createUadlMemHandler(UadlArch &arch)
  {
    arch.setExtMemory(memhandler);
    arch.setIssMemHandler(&memhandler);
    memhandler.setup(&arch.iss(),AccumMemStart,AccumMemEnd);
  }

};

static ExternMemPlugin extern_mem_plugin;

// Main entry point- returns a pointer to our service-provider object.
extern "C" UadlPlugin *adl_plugin_entry()
{
  return &extern_mem_plugin;
}

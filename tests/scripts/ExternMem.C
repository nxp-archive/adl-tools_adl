//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This demonstrates a very simple external memory.  Writes to the addresses
// 0xa000-0xaffff actually act as accumulator writes, adding their values to
// what's already there.
//

#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <errno.h>
#include <unistd.h>

#include "helpers/Macros.h"
#include "helpers/BasicTypes.h"

#include "iss/DefaultMem.h"
#include "iss/MemHandler.h"
#include "iss/SysCallExtHandler.h"
#include "iss/ModelInterface.h"
#include "iss/Plugin.h"

using namespace std;
using namespace adl;

static uint32_t AccumMemStart = 0xa000;
static uint32_t AccumMemEnd   = 0xafff;

// The special memory handler here treats memory in the range [start,end) as
// accumulator memory if we're dealing with aligned 32-bit writes.  Memory is
// cached in the node's DMI cache if the access falls outsided of that range.
//
// For system calls, all we handle are writes.
struct MyMem : public MemHandler, public SysCallExtHandler {

  // System-call reason codes
  enum syscodes {
    OsWrite     =   4,
  };


  MyMem() : _node(0), _start(0), _end(0) {};

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
    update_dmi_cache(CacheRead,addr,page);
  }

  void writepage(byte_t *s,size_t n,addr_t addr,addr_t crit_addr)
  {
    byte_t *page = 0;
    GlobalMemory.writepage(s,n,addr,crit_addr,page);
    update_dmi_cache(CacheWrite,addr,page);
  }

  void reset()
  {
    GlobalMemory.reset();
  }

  bool syscall_enabled() const { return true; };
  
  void syscall_trigger(IssCore &core,uint32_t syscode)
  {
    if (syscode == OsWrite) {
      int fd = _sc_args[1];
      int len = _sc_args[3];
      byte_t buf[len];
      addr_t ea = _sc_args[2];
      readpage(CacheRead,buf,len,ea,ea);
      _sc_args.clear();
      _sc_ret = write( fd,buf,len );      
    } else {
      cerr << "Unknown system code " << syscode << ".  Ignoring...\n";
    }
  }
  
  void syscall_add_arg(uint64_t a)
  {
    _sc_args.push_back(a);
  }
  
  int64_t syscall_return_code()
  {
    return _sc_ret;
  }
  
  uint32_t syscall_errno()
  {
    return errno;
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

  IssNode *_node;
  addr_t   _start;  // Starting address for accumulator functionality (inclusive).
  addr_t   _end;    // Ending address for accumulator functionality (exclusive).

  vector<uint64_t> _sc_args;
  int64_t          _sc_ret;
};

static MyMem memhandler;

// This class is the service provider.  For a request for a writer of type
// 'icnt', it returns an instruction-count object.  It also displays a small
// banner at load-timee so that we know that the plugin was installed.
struct ExternMemPlugin : public Plugin {

  // Called when the plugin is installed.
  virtual void init(AnyOption &options) 
  {
    cerr << "Loaded the external-memory plugin.\n";
  };  

  // Called for installing external memories.
  virtual void createMemHandler(IssNode &n)
  {
    n.setSysCallHandler(&memhandler);
    n.setMemHandler(&memhandler);
    memhandler.setup(&n,AccumMemStart,AccumMemEnd);
  }

};

static ExternMemPlugin extern_mem_plugin;

// Main entry point- returns a pointer to our service-provider object.
extern "C" Plugin *adl_plugin_entry()
{
  return &extern_mem_plugin;
}

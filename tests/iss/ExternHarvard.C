//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Demonstrates separate I and D mems.  This will install a small block memory
// and a standard sparse memory, with one or the other as the data memory and
// the other as an instruction memory.
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

struct SparseMem : public MemHandler {

  SparseMem() : _use_dmi(false), _node(0) {};

  void setup(bool use_dmi,IssNode *node)
  {
    _use_dmi = use_dmi;
    _node = node;
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
    GlobalMemory.write32(ra,v,nb,page);
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
  
protected:
  void update_dmi_cache(CacheAccess ca,addr_t ra,byte_t *page)
  {
    if (_use_dmi && _node && page) {
      _node->update_dmi_cache_entry(ca,ra,page);
    }
  }

  bool     _use_dmi;
  IssNode *_node;
};

struct BlockMem : public MemHandler {

  BlockMem() : _use_dmi(false) {};

  void setup(bool use_dmi,IssNode *node,unsigned size)
  {
    _use_dmi = use_dmi;
    _node = node;
    _mem.set_mem_size(size);
  }
  
  uint64_t read64(CacheAccess ca,addr_t ra)
  {
    update_dmi_cache(ca,ra);
    return _mem.read64(ra);
  }

  uint32_t read32(CacheAccess ca,addr_t ra)
  {
    update_dmi_cache(ca,ra);
    return _mem.read32(ra);
  }

  uint16_t read16(CacheAccess ca,addr_t ra)
  {
    update_dmi_cache(ca,ra);
    return _mem.read16(ra);
  }

  uint8_t  read8(CacheAccess ca,addr_t ra)
  {
    update_dmi_cache(ca,ra);
    return _mem.read8(ra);
  }

  void write64(addr_t ra, uint64_t v, unsigned nb)
  {
    _mem.write64(ra,v,nb);
    update_dmi_cache(CacheWrite,ra);
  }

  void write32(addr_t ra, uint32_t v, unsigned nb)
  {
    _mem.write32(ra,v,nb);
    update_dmi_cache(CacheWrite,ra);
  }

  void write16(addr_t ra, uint16_t v, unsigned nb)
  {
    _mem.write16(ra,v,nb);
    update_dmi_cache(CacheWrite,ra);
  }

  void write8 (addr_t ra, uint8_t  v)
  {
    _mem.write8(ra,v);
    update_dmi_cache(CacheWrite,ra);
  }

  void readpage(CacheAccess ca,byte_t *t,size_t n,addr_t addr,addr_t crit_addr)
  {
    _mem.readpage(t,n,addr,crit_addr);
    update_dmi_cache(ca,addr);
  }

  void writepage(byte_t *s,size_t n,addr_t addr,addr_t crit_addr)
  {
    _mem.writepage(s,n,addr,crit_addr);
    update_dmi_cache(CacheWrite,addr);
  }

  void reset()
  {
    _mem.reset();
  }
  
protected:
  void update_dmi_cache(CacheAccess ca,addr_t ra)
  {
    if (_use_dmi && _node) {
      _node->update_dmi_cache_entry(ca,ra,_mem.last_page(ra));
    }
  }

  bool         _use_dmi;
  IssNode     *_node;
  BlockMemory  _mem;
};

static SparseMem  sparsehandler;
static BlockMem   blockhandler1;
static BlockMem   blockhandler2;

bool block_mem_setup(IssNode &n)
{
  unsigned handle1 = 0,size1 = 0;
  unsigned handle2 = 0,size2 = 0;
  bool use_dmi = false;

  string name1 = "IMEM", name2 = "DMEM", name3 = "OMEM";
  
  const MemInfos &mi = n.getMemInfo();

  ForEach(mi,i) {
    if (i->_name == name1) {
      size1 = i->_size;
      handle1 = i->_id;
    }
    else if (i->_name == name2) {
      size1 = i->_size;
      handle1 = i->_id;
      use_dmi = true;
    }
    else if (i->_name == name3) {
      size2 = i->_size;
      handle2 = i->_id;      
    }
  }

  if (size1 == 0) {
    throw runtime_error("Could not find information about " + name1 + " or " + name2 + " in the core object's memory info.");
  }

  n.setMemHandler(&blockhandler1,handle1);
  blockhandler1.setup(use_dmi,&n,size1);

  if (size2) {
    n.setMemHandler(&blockhandler2,handle2);
    blockhandler2.setup(false,&n,size2);
  }

  // Install the DMI handler for the sparse memory if we don't use it here.
  // Otherwise, don't install it.
  return !use_dmi;
}

struct ExternHarvardPlugin : public Plugin {

  // Called when the plugin is installed.
  virtual void init(AnyOption &options) 
  {
    cerr << "Loaded the external-memory plugin.\n";
  };  

  // Called for installing external memories.
  virtual void createMemHandler(IssNode &n)
  {
    bool sparse_use_dmi = block_mem_setup(n);
    
    n.setMemHandler(&sparsehandler,0);
    sparsehandler.setup(sparse_use_dmi,&n);
  }

};

static ExternHarvardPlugin extern_harvard_plugin;

// Main entry point- returns a pointer to our service-provider object.
extern "C" Plugin *adl_plugin_entry()
{
  return &extern_harvard_plugin;
}

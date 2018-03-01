//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// A simple example of a plugin implementing a uart and an interrupt controller,
// both of which are memory mapped.
//

#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <memory>
#include <mutex>

#include <unordered_map>

#include <boost/function.hpp>
#include <boost/bind.hpp>

#define THREAD_SAFE_MEMORY

#include "iss/ModelInterface.h"
#include "iss/DefaultMem.h"
#include "iss/MemHandler.h"

#include "rnumber/RNumber.h"

#include "helpers/BasicTypes.h"
#include "helpers/AnyOption.h"

#include "iss/Plugin.h"

static bool Verbose          = false;
static bool OtherCoresHalted = false;

#define vout(x)     { if (Verbose) cout << "Periph:  " << x << '\n' << dec; };

//
// We're redefining this stuff in order to use C++11 features.
//

#undef ForEach
#define ForEach(container,iter) \
       for (auto iter = container.begin(), end = container.end(); iter != end; ++iter)

// Execute code if 'z' is found in 'y'.  'x' is the iterator assigned the value.
#undef IfFind
#define IfFind(x,y,z) \
  auto x = y.find(z); if (x != y.end())


#define RError(x) { std::ostringstream ss; ss << x; throw std::runtime_error(ss.str()); }

using namespace std;
using namespace adl;

// Base class for all peripherals.
struct Peripheral {
  virtual ~Peripheral() {};

  std::mutex _mutex;
};


//
// The callback stuff is a bit complex, so I'll describe it here in a bit more
// detail.  We're using boost::function to create callback objects which can
// then be easily passed around and invoked with the relevant arguments.  We're
// then using boost::bind to bind the arguments we supply to the callback to the
// actual object called, which in this case tends to be member functions of a
// class.  So, for the callback, we're finding the 'this' pointer to the member
// function, and returning that bound object as the callback, so that it can be
// called without an object pointer.
//
// An extra level of complexity is that we then have an outer level which binds
// that object and a Peripheral reference to a function.  The purpose of this
// function is to create a lock using the mutex in the Peripheral, then invoke
// the callback.  That way, relatively transparently, each call of a
// memory-mapped register is done with a lock, preventing any other threads from
// clobbering that particular object.
//

// A read callback takes the core ref and the address as parameters and
// returns the read value.
typedef boost::function<uint32_t (adl::IssCore &core,adl::addr_t)> ReadCallback;

// A write takes the core ref, address, and write value as parameters and does
// not return anything.
typedef boost::function<void (adl::IssCore &core,adl::addr_t,uint32_t)> WriteCallback;

// Lock function used for reads.
uint32_t read_lock(ReadCallback &rcb,Peripheral &p,adl::IssCore &core,adl::addr_t addr)
{
  std::lock_guard<std::mutex> lock(p._mutex);
  return rcb(core,addr);
}

// Lock function used for writes.
void write_lock(WriteCallback &wcb,Peripheral &p,adl::IssCore &core,adl::addr_t addr,uint32_t value)
{
  std::lock_guard<std::mutex> lock(p._mutex);
  return wcb(core,addr,value);
}



//
// For storing callbacks associated with memory addresses.
//
// To use:
//
//    MapRegisterRead(<MemoryMappedRegs&>,<address>,<member-ptr>)
//    MapRegisterWrite(<MemoryMappedRegs&>,<address>,<member-ptr>)
//
// So, or example, if you have a class Foo, with a read handler named
// handle_read and a write handler named handle_write, then in a Foo method:
//
// Foo::install_mem_regs(MemoryMappedRegs &mmr,addr_t addr) 
// {
//   MapRegsiterRead(mmr,addr,Foo,handle_read);
//   MapRegsiterWrite(mmr,addr,Foo,handle_write);
// }
//
class MemoryMappedRegs {
public:
  virtual ~MemoryMappedRegs() {};

  struct CallbackPair {
    ReadCallback _read;
    WriteCallback _write;

  };
  typedef std::unordered_map<unsigned,CallbackPair> RegMap;

  // Call this to register a read handler.
  template <typename T,uint32_t (T::*F)(adl::IssCore &core,adl::addr_t)>
  void register_read_cb(adl::addr_t addr,T &obj,Peripheral &p) 
  {
    ReadCallback rcb = boost::bind(F,boost::ref(obj),_1,_2);
    _reg_map[addr]._read = boost::bind(read_lock,rcb,boost::ref(p),_1,_2);
  }

  // Call this to register a write handler.
  template <typename T,void (T::*F)(adl::IssCore &core,adl::addr_t,uint32_t)>
  void register_write_cb(unsigned addr,T &obj,Peripheral &p) 
  {
    WriteCallback wcb = boost::bind(F,boost::ref(obj),_1,_2,_3);
    _reg_map[addr]._write = boost::bind(write_lock,wcb,boost::ref(p),_1,_2,_3);
  }

  // Return a read callback if it exists for a given address, else return 0.
  ReadCallback find_read(adl::addr_t addr)
  {
    IfFind(iter,_reg_map,addr) {
      return iter->second._read;
    }
    return 0;
  }

  // Return a write callback if it exists for a given address, else return 0.
  WriteCallback find_write(adl::addr_t addr)
  {
    IfFind(iter,_reg_map,addr) {
      return iter->second._write;
    }
    return 0;
  }

  // Access to the memory map.
  const RegMap &regMap() const { return _reg_map; };

  // Or call these as shortcuts.
# define MapRegisterRead(iomap,periph,addr,type,mem_fun) ((iomap).register_read_cb<type,&type::mem_fun>(addr,*this,periph))
# define MapRegisterWrite(iomap,periph,addr,type,mem_fun) ((iomap).register_write_cb<type,&type::mem_fun>(addr,*this,periph))

protected:
  // Stores the callbacks, representing memory-mapped registers.
  RegMap              _reg_map;
};


// This is the per-core object with private memory(s)
struct CoreMem : public MemHandler {
    
  CoreMem(IssCore &core,MemoryMappedRegs *mmr,bool first) : 
    _core(core),
    _memMappedRegs(mmr),
    _first(first)
  {
  }

  uint64_t read64(CacheAccess ca,addr_t ra)
  {
    byte_t *page;
    uint64_t r = GlobalMemory.read64(ra,page);
    _core.update_dmi_cache_entry(ca,ra,page);
    return r;
  }

  uint32_t read32(CacheAccess ca,addr_t ra)
  {
    // Then check for memory mapped registers.  Note that this is *not* cached,
    // since it's not "plain" memory.
    if (auto r = _memMappedRegs->find_read(ra)) {
      return (r)(_core,ra);
    }
    
    // Last, just access global memory.
    byte_t *page = 0;
    uint32_t r = GlobalMemory.read32(ra,page);
    _core.update_dmi_cache_entry(ca,ra,page);
    return r;
  }

  uint16_t read16(CacheAccess ca,addr_t ra)
  {
    byte_t *page = 0;
    uint16_t r = GlobalMemory.read16(ra,page);
    _core.update_dmi_cache_entry(ca,ra,page);
    return r;
  }
    
  uint8_t  read8(CacheAccess ca,addr_t ra)
  {
    byte_t *page = 0;
    uint8_t r = GlobalMemory.read8(ra,page);
    _core.update_dmi_cache_entry(ca,ra,page);
    return r;
  }

  void write64(addr_t ra, uint64_t v, unsigned nb)
  {
    byte_t *page = 0;
    GlobalMemory.write64(ra,v,nb,page);
    _core.update_dmi_cache_entry(CacheWrite,ra,page);
  }

  void write32(addr_t ra, uint32_t v, unsigned nb)
  {
    // Then check for memory mapped registers.  Note that this is *not* cached,
    // since it's not "plain" memory.
    if (auto w = _memMappedRegs->find_write(ra)) {
      (w)(_core,ra,v);
      return;
    }

    // Finally, just write to global memory.
    byte_t *page = 0;
    GlobalMemory.write32(ra,v,nb,page);
    _core.update_dmi_cache_entry(CacheWrite,ra,page);
  }

  void write16(addr_t ra, uint16_t v, unsigned nb)
  {
    byte_t *page = 0;
    GlobalMemory.write16(ra,v,nb,page);
    _core.update_dmi_cache_entry(CacheWrite,ra,page);
  }

  void write8 (addr_t ra, uint8_t  v)
  {
    byte_t *page = 0;
    GlobalMemory.write8(ra,v,page);
    _core.update_dmi_cache_entry(CacheWrite,ra,page);
  }

  void readpage(CacheAccess ca,byte_t *t,size_t n,addr_t addr,addr_t crit_addr)
  {
    byte_t *page = 0;
    GlobalMemory.readpage(t,n,addr,crit_addr,page);
    _core.update_dmi_cache_entry(ca,addr,page);
  }

  void writepage(byte_t *s,size_t n,addr_t addr,addr_t crit_addr)
  {
    byte_t *page = 0;
    GlobalMemory.writepage(s,n,addr,crit_addr,page);
    _core.update_dmi_cache_entry(CacheWrite,addr,page);
  }

  void reset()
  {
    if (_first) {
      GlobalMemory.reset();
    }
  }    

  IssCore          &_core;
  MemoryMappedRegs *_memMappedRegs;
  bool              _first;
};

// Master memory object- holds the master list of memory-mapped peripherals.
class MemModel {
public:

  MemModel() : 
    _memMappedRegs(new MemoryMappedRegs),
    _first(true)
  {};

  ~MemModel()
  {
    _mems.clear();
  }

  MemoryMappedRegs &memMappedRegs() { return *_memMappedRegs; };

  void init(IssCore &core)
  {
    unique_ptr<CoreMem> cm(new CoreMem{core,_memMappedRegs.get(),_first});
    _first = false;

    core.setMemHandler(cm.get());
    _mems.push_back(move(cm));
  }

private:
  std::unique_ptr<MemoryMappedRegs>      _memMappedRegs;    // Global memory-mapped register object.
  std::vector<std::unique_ptr<CoreMem> > _mems;             // Pointers to each core's memory objects.
  bool                                   _first;
};


// A very simple memory-mapped uart.  In this case, we just support output to
// standard out.  Reading from the address returns the number of characters
// written.
struct Uart : public Peripheral {

  Uart(MemModel &mem,addr_t addr) :
    _writes(0)
  {
    MemoryMappedRegs &mmr = mem.memMappedRegs();

    MapRegisterWrite(mmr,*this,addr,Uart,charWrite);
    MapRegisterRead (mmr,*this,addr,Uart,opCount);
  }

  void charWrite(IssCore &core,addr_t addr,uint32_t v)
  {
    cout << (char)v;
    ++_writes;
  }

  uint32_t opCount(IssCore &core,addr_t addr)
  {
    return _writes;
  }
  
  unsigned _writes;
};

// A very simple interrupt controller.  Triggers external excepts in other cores
// via a write to a memory mapped register.  For each bit in the value written,
// if it's a 1, we generate an exception for the corresponding core, where the
// lsb of the value corresponds to core 0.  Reading the value returns the number
// of interrupts generated from the last write.
struct Interrupt : public Peripheral {

  Interrupt(IssNode &root,MemModel &mem,unsigned ehandle,addr_t addr) :
    _nodes(&root.children()),
    _ehandle(ehandle),
    _exceptions(0)
  {
    MemoryMappedRegs &mmr = mem.memMappedRegs();

    MapRegisterWrite(mmr,*this,addr,Interrupt,genException);
    MapRegisterRead (mmr,*this,addr,Interrupt,exceptionCount);
  }

  void genException(IssCore &core,addr_t addr,uint32_t v)
  {
    _exceptions = 0;
    unsigned index = 0;
    while (v) {
      if (v & 1) {
        if (index < _nodes->size()) {
          IssNode *n = (*_nodes)[index];
          
          //cout << "Triggering exception on core " << index << "\n";
          n->genExceptions(_ehandle);
          ++_exceptions;
        }        
      }
      ++index;
      v >>= 1;
    }
  }

  uint32_t exceptionCount(IssCore &core,addr_t addr)
  {
    return _exceptions;
  }

  const IssChildren *_nodes;
  unsigned           _ehandle;
  unsigned           _exceptions;
};

unsigned getExceptionHandle(const IssNode &node,const string &name)
{
  if (node.is_core()) {
    auto &info = node.getExceptionInfo();
    ForEach(info,i) {
      if (i->_name == name) {
        return i->_flag;
      }
    }
    cerr << "Could not find information about exception " << name << endl;
    return -1;
  } else {
    return getExceptionHandle(*node.children().front(),name);
  }
}

// This class is the service provider and attaches the various model objects to
// the cores.
struct ThePlugin : public Plugin {

  ThePlugin() {};

  // Called when the plugin is installed.
  virtual void init(AnyOption &options) 
  {
    cout << "Loaded the peripheral plugin.\n";
    options.setFlag("verbose-periph", "Enable verbose output of peripherals in this plugin.");
    options.setFlag("other-cores-halted","Set all but core 0 to be halted at startup.");
  };  

  void registerCore (IssNode *node)
  {
    assert(node);
    if (node->is_core()) {
      IssCore *core = dynamic_cast<IssCore*>(node);
      _mem.init(*core);
      // If it's not the main core, then halt it, unless the command-line option
      // to keep all cores active is true.
      if ((core->getCoreId() != 0) && OtherCoresHalted) {
        core->set_active_count(0);
      }
    } else {
      ForEach(node->children(),c) {
        registerCore(*c);
      }
    }
  }

  // Called after final option processing.
  virtual void setup(AnyOption &options,IssNode &root)
  {
    if (options.getFlag("verbose-periph")) {
      Verbose = true;
    }

    if (options.getFlag("other-cores-halted")) {
      OtherCoresHalted = true;
    }

    registerCore(&root);

    _uart.reset(new Uart(_mem,0x2000));

    int eh = getExceptionHandle(root,"External");
    if (eh >= 0) {
      _interrupt.reset(new Interrupt(root,_mem,eh,0x2010));
    }
  }

  MemModel               _mem;
  unique_ptr<Peripheral> _uart;
  unique_ptr<Peripheral> _interrupt;
};

static ThePlugin the_plugin;

// Main entry point- returns a pointer to our service-provider object.
extern "C" Plugin *adl_plugin_entry()
{
  return &the_plugin;
}

//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Miscellaneous things used by generated ISS models.  This should
// only be included by generated ISS models and not by anything else.
//

#ifndef _MODELSUPPORT_H_
#define _MODELSUPPORT_H_

// For now, we're deactivating the propagation of undefined results.
#define USE_INTBV

#include <algorithm>

#include "helpers/Macros.h"

namespace adl {

  template <size_t N> class ns_bits;
  template <size_t N> class ns_sbits;

}

//We include intbv here becase Memory interface will work with it

#include "helpers/sintbv.h"

#include "helpers/TypeConv.h"

#include "ModelInterface.h"

#include "Memory.h"
#include "MemoryInterface.h"
#include "MemHandler.h"
#include "PacketPool.h"

namespace adl {

  extern bool WatchpointsEnabled;

  // Used as the return code for exec to indicate status.
  enum ExecStatus { esNormal, esEndBasicBlock, esTryAgain };

  struct InstrCountBase {
    InstrCountBase() :
      _instr_count(0)
    {}

    void reset()
    {
      _instr_count = 0;
    }

    uint64_t getInstrCount() const { return _instr_count; };
    
    uint64_t _instr_count;
  };

  // ISSs which implement the temporal decoupling API inherit from this base
  // class in order to get the relevant member variables.
  struct TemporalDecouplingBase : public InstrCountBase {
    TemporalDecouplingBase() :
      _td_counter(0), 
      _td_threshold((uint64_t)-1), 
      _td_increment(1)
    {}

    void reset()
    {
      _td_counter = 0;
      _td_threshold = (uint64_t)-1;
      _td_increment = 1;
      InstrCountBase::reset();
    }

    uint64_t _td_counter;
    uint64_t _td_threshold;
    unsigned _td_increment;
  };

  // Time-tagged ISSs inherit from this class in order to store miscellaneous
  // time information.  We re-use the temporal-decoupling counter as the core's
  // clock, so all time-tagged ISSs implement the temporal decoupling API.
  struct TimeTaggedBase : public TemporalDecouplingBase {
    TimeTaggedBase()
    {
      reset();
    }

    void reset()
    {
      _latency = 0;
      _final_time = 0;
      _cache_latency = 0;
      TemporalDecouplingBase::reset();
    }

    // This returns the current time, as seen by the current instruction.  The
    // real "current time" is stored in td_counter.  The main difference is for
    // multi-issue machines, in which one instruction may complete later than
    // the second instruction, if the second instruction has no dependencies
    // upon the first.
    ttime_t current_time() const
    {
      return _accum_time;
    }

    void clear_cache_latency()
    {
      _cache_latency = 0;
    }

    void add_cache_latency(unsigned l)
    {
      _cache_latency += l;
    }

    void accum_cache_latency()
    {
      _td_counter += _cache_latency;
      _accum_time += _cache_latency;
      _cache_latency = 0;
    }

    void start_instr(unsigned latency)
    {
      _latency = latency;
      _accum_time = _td_counter;
      _cache_latency = 0;
    }

    void accumulate_time(ttime_t x)
    {
      _accum_time = std::max(x,_accum_time);
    }

    void update_completion_time()
    {
      _final_time = std::max(current_time()+_cache_latency,_final_time);
    }

    // We use max() here in case an external peripheral advanced time, e.g. to
    // model the core yielding or being put to sleep.
    void advance_current_time(ttime_t t)
    {
      _td_counter = std::max(_td_counter,std::max(_accum_time,t));
    }
        
    ttime_t _accum_time;         // Accumulated time for the current instruction.
    ttime_t _final_time;         // Tracks the final time in the core, which might
                                 // not be the final completion time for out-of-order
                                 // models.
    unsigned _latency;           // Any extra delay latency for the instruction.
    unsigned _cache_latency;
  };

  //
  // Time-tagged ISS interface routines.
  //

  struct TagNode;

  //
  // Each register, in a time-tagged ISS, contains a tag node which represents
  // the time at which it is next available.  As an instruction executes, we
  // accumulate time as we read sources and update targets using this
  // accumulated time.  This all happens on-the-fly, in order to handle
  // conditional updates.
  //

  struct TagNode {
    TagNode() 
    { 
      reset_tag(); 
    }

    void reset_tag() 
    { 
      _tag = 0; 
      _newtag = 0; 
      _update = 0; 
    }

    // The instruction id is one less than the instruction count, since we
    // pre-increment.
    uint64_t instr_id(const TimeTaggedBase &t) const { return t._instr_count - 1; };

    void add_source(TimeTaggedBase &t) const
    {
      if (_update < instr_id(t)) {
        _tag = _newtag;
        _update = instr_id(t);
      }
      t.accumulate_time(_tag);
    }
  
    void add_target(TimeTaggedBase &t)
    {
      _newtag = t.current_time() + t._latency + t._cache_latency;
      _update = instr_id(t);
    }

    void set_cache_latency(TimeTaggedBase &t)
    {
      _tag = std::max(t.current_time(),_tag) + t._cache_latency;
      t.accumulate_time(_tag);
    }

    mutable ttime_t  _tag;
    mutable ttime_t  _newtag;
    mutable ttime_t  _update;
  };

  // For use by time-tagged ISSs.
  template <typename T>
  class Tag : public TagNode {
  public:
    typedef T value_type;

    const T &get_data() const { return const_cast<Tag<T> &>(*this).get_data(); };

    const T &get_data(TimeTaggedBase &t) const { return const_cast<Tag<T> &>(*this).get_data(t); };

    T &get_data(TimeTaggedBase &t) { add_source(t); return _data; };

    T &get_data() { return _data; };

    T &set_data(TimeTaggedBase &t) { add_source(t); add_target(t); return _data; };

    T get_value() const { return _data; };
    void set_value(const T &x) { _data = x; reset_tag(); };

    static size_t width() { return value_type::Size; };

  private:
    T _data;  
  };

  // For use by non-time-tagged ISSs, for compatibility
  // with time-tagged ISSs.
  template <typename T>
  class NoTag {
  public:
    typedef T value_type;

    const T &get_data() const { return const_cast<NoTag<T> &>(*this).get_data(); };

    T &get_data() { return _data; };

    T &set_data() { return _data; };

    T get_value() const { return _data; };
    void set_value(const T &x) { _data = x; reset_tag(); };

    static size_t width() { return value_type::Size; };

    void reset_tag() { };
  private:
    T _data;
  };

  // This class is used to handle resource-limited groups of instructions.  For
  // when we have a single resource available, we use just an integer counter.
  // For more than one resource, we have an array of counters, arranged in a
  // circular queue.
  template <size_t N,size_t Delay>
  class ResCounter {
  public:
    ResCounter() { reset(); };

    void update_time(TimeTaggedBase &t)
    {
      // Find next available.  We just use a loop here (which will be
      // unrolled) because I think that the number of units will generally
      // be relatively small.
      int x = 0;
      ttime_t s = _avail[0];
      for (int i = 1; i != N; ++i) {
        if (_avail[i] < s) {
          s = _avail[i];
          x = i;
        }
      }
      _cur = x;
      t.accumulate_time(_avail[x]);
    };

    void update_avail(TimeTaggedBase &t)
    {
      _avail[_cur] = t.current_time() + Delay;
    };

    void reset() { _cur = 0; for (int i = 0; i != N; ++i) { _avail[i] = 0; } };

  private:
    ttime_t _avail[N];
    int     _cur;
  };

  // Single element version- no array used, just an integer.
  template <size_t Delay>
  class ResCounter<1,Delay> {
  public:
    ResCounter() : _avail(0) {};

    void update_time(TimeTaggedBase &t) 
    { 
      t.accumulate_time(_avail); 
    };

    void update_avail(TimeTaggedBase &t) 
    { 
      _avail = t.current_time() + Delay; 
    };

    void reset() { _avail = 0; };

  private:
    ttime_t _avail;
  };

  // For normal operation, we do bounds checking by masking the argument.  For
  // the JIT, we turn this off to gain some speed.
# ifdef REGFILE_NO_BOUNDS_CHECK
#  define CheckBounds(x) (x)
# else
#  define CheckBounds(x) (x & (M-1))
# endif  

  // Dummy callbacks used for a default actions.
  extern RegCallback    default_reg_callback_action;
  extern ExceptCallback default_except_callback_action;

  // Class for representing a register file.  Range checking is done by
  // masking, thus M should be a power of 2, where M is the closest power-of-2
  // >= N.
  template <typename T,size_t N,size_t M>
  class RegFile {
  public:
    typedef T value_type;
    typedef T &ref_type;
    typedef T *ptr_type;

    T &operator()(unsigned x) { return _regs[CheckBounds(x)]; };
    template <size_t Nb>
    T &operator()(ns_bits<Nb> x) { return operator()(x.uint32()); };

    const T &operator()(unsigned x) const { return _regs[CheckBounds(x)]; };
    template <size_t Nb>
    const T &operator()(ns_bits<Nb> x) const { return operator()(x.uint32()); };

    size_t size() const { return N; };

    size_t width() const { return value_type::width(); };

    void reset(typename value_type::value_type rv)
    {
      for (size_t i = 0; i != N; ++i) {
        _regs[i].set_value(rv);
      }
    }
    
    void reset_tags();
  private:
    T _regs[N];
  };

  template <typename T,size_t N,size_t M>
  inline void RegFile<T,N,M>::reset_tags(void)
  {
    for (size_t i = 0; i != N; ++i) {
      _regs[i].reset_tag();
    }
  }

  //
  // Types for local-memory support.
  //

  template <size_t AddrUnit, size_t Offset, size_t MemorySize>
  struct MemoryTranslate {
    addr_t operator()(addr_t a,int seq) { return a; };
    addr_t scale_ea(addr_t ea)   { return (((ea << AddrUnit) + Offset) & (0xffffffffULL >> AddrUnit)); };
    addr_t mask_ea(addr_t ea) { return ea % MemorySize; };
    void aligned_write(addr_t,addr_t) { };
    void misaligned_write(addr_t,addr_t) { };
    void misaligned_read(addr_t,addr_t) { };
    void pre_read(addr_t,addr_t) {};
    void post_read(addr_t,addr_t) {};
    void pre_write(addr_t,addr_t) {};
    void post_write(addr_t,addr_t) {};
  };

  // Base class for external accesses.
  template <CacheAccess AccessType,typename MemType>
  struct ExternAccess {
    ExternAccess(MemType *mem = 0) : _mem(mem) {}
      
    void setHandler(MemType *mem) { _mem = mem; };
  
    uint64_t read64(addr_t ra) { return _mem->read64(AccessType,ra); };
    uint32_t read32(addr_t ra) { return _mem->read32(AccessType,ra); };
    uint16_t read16(addr_t ra) { return _mem->read16(AccessType,ra); };
    uint8_t  read8 (addr_t ra) { return _mem->read8 (AccessType,ra); };
    uint8_t  read0 (addr_t ra) { _mem->readpage (AccessType,0,0,ra,ra); return 0; };

    uint64_t read64(CacheAccess ca,addr_t ra) { return _mem->read64(ca,ra); };
    uint32_t read32(CacheAccess ca,addr_t ra) { return _mem->read32(ca,ra); };
    uint16_t read16(CacheAccess ca,addr_t ra) { return _mem->read16(ca,ra); };
    uint8_t  read8 (CacheAccess ca,addr_t ra) { return _mem->read8 (ca,ra); };
    uint8_t  read0 (CacheAccess ca,addr_t ra) { _mem->readpage (ca,0,0,ra,ra); return 0; };
  
    void write64(addr_t ra,uint64_t v,unsigned nb) { _mem->write64(ra,v,nb); };
    void write32(addr_t ra,uint32_t v,unsigned nb) { _mem->write32(ra,v,nb); };
    void write16(addr_t ra,uint16_t v,unsigned nb) { _mem->write16(ra,v,nb); };
    void write8 (addr_t ra,uint8_t  v)             { _mem->write8 (ra,v);   };
    void write0 (addr_t ra,uint8_t  v)             { _mem->writepage (0,0,ra,ra);   };

    void readpage(CacheAccess ca,byte_t *t,size_t n,addr_t addr,addr_t critical_addr) { _mem->readpage(ca,t,n,addr,critical_addr); };
    void readpage(byte_t *data,unsigned n,addr_t addr,addr_t critical_addr) { _mem->readpage(AccessType,data,n,addr,critical_addr); }
    void writepage(byte_t *s,size_t n,addr_t addr,addr_t critical_addr) { _mem->writepage(s,n,addr,critical_addr); };
    void fillpage(unsigned char c,size_t n,addr_t addr) { _mem->fillpage(c,n,addr); };

    // Not used, as the external model will populate the DMI memory, if used.
    byte_t *last_page(addr_t addr) { return 0; };

    MemType *_mem;
  };

  // For data accesses, this supplies the appropriate access-type for reads.
  template <typename MemType>
  struct ExternDataAccess : public ExternAccess<CacheRead,MemType> { 
    ExternDataAccess(MemType *mem = 0) : ExternAccess<CacheRead,MemType>(mem) {};
  };

  // For instruction accesses, this supplies the appropriate access-type for reads.
  template <typename MemType>
  struct ExternInstrAccess : public ExternAccess<CacheIFetch,MemType> { 
    ExternInstrAccess(MemType *mem = 0) : ExternAccess<CacheIFetch,MemType>(mem) {};
  };

  // For instruction translation accesses, this supplies the appropriate access-type for reads.
  template <typename MemType>
  struct ExternInstrTransAccess : public ExternAccess<CacheIFetchTrans,MemType> { 
    ExternInstrTransAccess(MemType *mem = 0) : ExternAccess<CacheIFetchTrans,MemType>(mem) {};
    ExternInstrTransAccess(const ExternInstrAccess<MemType> &e) : ExternAccess<CacheIFetchTrans,MemType>(e._mem) {};
  };

  template<typename MemType>
  struct SimpleCache
  {
    SimpleCache(MemType mem) : _mem(mem) {}
  
    uint64_t read64(unsigned seq,addr_t ea,addr_t ra) { return _mem.read64(ra); };
    uint32_t read32(unsigned seq,addr_t ea,addr_t ra) { return _mem.read32(ra); };
    uint16_t read16(unsigned seq,addr_t ea,addr_t ra) { return _mem.read16(ra); };
    uint8_t  read8 (addr_t ea,addr_t ra)              { return _mem.read8 (ra); };
    uint8_t  read0 (addr_t ea,addr_t ra)              { return _mem.read0 (ra); };
  
    void write64(addr_t ea,addr_t ra,uint64_t v,unsigned nb) { _mem.write64(ra,v,nb); };
    void write32(addr_t ea,addr_t ra,uint32_t v,unsigned nb) { _mem.write32(ra,v,nb); };
    void write16(addr_t ea,addr_t ra,uint16_t v,unsigned nb) { _mem.write16(ra,v,nb); };
    void write8 (addr_t ea,addr_t ra,uint8_t  v)             { _mem.write8 (ra,v);   };
    void write0 (addr_t ea,addr_t ra,uint8_t  v)             { _mem.write0 (ra,v);   };

    void readpage(byte_t *data,unsigned n,addr_t addr,addr_t critical_addr) { _mem.readpage(data,n,addr,critical_addr); }

    addr_t misaligned_read_offset64(addr_t ea) { return ea & DWMask; };
    addr_t misaligned_read_offset32(addr_t ea) { return ea & WordMask; };
    addr_t misaligned_read_offset16(addr_t ea) { return ea & HWMask; };

    // For DMI cache support: Return a pointer to the last page used, adjusted
    // by the specified address so that it's pointing to the start of the block
    // given by the address.
    byte_t *last_page(addr_t addr) { return _mem.last_page(addr); };

    MemType _mem;
  };

  template<typename MemType>
  struct SimpleTypedCache
  {
    SimpleTypedCache(MemType mem,int type) : _mem(mem), _type((CacheAccess)type) {}
  
    uint64_t read64(unsigned seq,addr_t ea,addr_t ra) { return _mem.read64(_type,ra); };
    uint32_t read32(unsigned seq,addr_t ea,addr_t ra) { return _mem.read32(_type,ra); };
    uint16_t read16(unsigned seq,addr_t ea,addr_t ra) { return _mem.read16(_type,ra); };
    uint8_t  read8 (addr_t ea,addr_t ra)              { return _mem.read8 (_type,ra); };
    uint8_t  read0 (addr_t ea,addr_t ra)              { return _mem.read0 (_type,ra); };
  
    void write64(addr_t ea,addr_t ra,uint64_t v,uint64_t m) { _mem.write64(ra,v,m); };
    void write32(addr_t ea,addr_t ra,uint32_t v,uint32_t m) { _mem.write32(ra,v,m); };
    void write16(addr_t ea,addr_t ra,uint16_t v,uint16_t m) { _mem.write16(ra,v,m); };
    void write8 (addr_t ea,addr_t ra,uint8_t  v)            { _mem.write8 (ra,v);   };
    void write0 (addr_t ea,addr_t ra,uint8_t  v)            { _mem.write0 (ra,v);   };

    void readpage(byte_t *data,unsigned n,addr_t addr,addr_t critical_addr) { _mem.readpage(data,n,addr,critical_addr); }

    addr_t misaligned_read_offset64(addr_t ea) { return ea & DWMask; };
    addr_t misaligned_read_offset32(addr_t ea) { return ea & WordMask; };
    addr_t misaligned_read_offset16(addr_t ea) { return ea & HWMask; };

    // For DMI cache support: Return a pointer to the last page used, adjusted
    // by the specified address so that its pointing to the start of the block
    // given by the address.
    byte_t *last_page(addr_t addr) { return _mem.last_page(addr); };

    MemType     _mem;
    CacheAccess _type;
  };

  template<typename MemType, typename Core>
  struct SimpleLoggingMemCache
  {
    SimpleLoggingMemCache(MemType mem,Core &core) : _mem(mem), _core(core) {}
  
    uint64_t read64(unsigned seq,addr_t ea,addr_t ra) 
    { 
      _core.handle_memory_request(0, MemAccess(ea,ra,8,BytesPerDW - (ra & DWMask),CacheRead)); 
      return _mem.read64(ra); 
    };

    uint32_t read32(unsigned seq,addr_t ea,addr_t ra) 
    { 
      _core.handle_memory_request(0, MemAccess(ea,ra,4,BytesPerWord - (ra & WordMask),CacheRead)); 
      return _mem.read32(ra); 
    };

    uint16_t read16(unsigned seq,addr_t ea,addr_t ra) 
    { 
      unsigned nb = ( (ra & WordMask) == 3) ? 1 : 2;
      _core.handle_memory_request(0, MemAccess(ea,ra,2,nb,CacheRead)); 
      return _mem.read16(ra); 
    };

    uint8_t  read8 (addr_t ea,addr_t ra)              
    { 
      _core.handle_memory_request(0, MemAccess(ea,ra,1,1,CacheRead)); 
      return _mem.read8 (ra); 
    };

    uint8_t  read0 (addr_t ea,addr_t ra)              
    { 
      _core.handle_memory_request(0, MemAccess(ea,ra,0,0,CacheRead)); 
      return 0;
    };
  
    void write64(addr_t ea,addr_t ra,uint64_t v,unsigned nb) { _core.handle_memory_request(0, MemAccess(ea,ra,8,nb,CacheWrite)); _mem.write64(ra,v,nb); };
    void write32(addr_t ea,addr_t ra,uint32_t v,unsigned nb) { _core.handle_memory_request(0, MemAccess(ea,ra,4,nb,CacheWrite)); _mem.write32(ra,v,nb); };
    void write16(addr_t ea,addr_t ra,uint16_t v,unsigned nb) { _core.handle_memory_request(0, MemAccess(ea,ra,2,nb,CacheWrite)); _mem.write16(ra,v,nb); };
    void write8 (addr_t ea,addr_t ra,uint8_t  v)            { _core.handle_memory_request(0, MemAccess(ea,ra,1,1,CacheWrite)); _mem.write8 (ra,v);   };
    void write0 (addr_t ea,addr_t ra,uint8_t  v)            { _core.handle_memory_request(0, MemAccess(ea,ra,0,0,CacheWrite)); };

    void readpage(byte_t *data,unsigned n,addr_t addr,addr_t critical_addr) { _mem.readpage(data,n,addr,critical_addr); }

    addr_t misaligned_read_offset64(addr_t ea) { return ea & DWMask; };
    addr_t misaligned_read_offset32(addr_t ea) { return ea & WordMask; };
    addr_t misaligned_read_offset16(addr_t ea) { return ea & HWMask; };

    MemType _mem;
    Core   &_core;
  };

  // For instruction reads only.
  template<typename MemType, typename Core>
  struct SimpleLoggingMemInstrCache
  {
    SimpleLoggingMemInstrCache(MemType mem,Core &core) : _mem(mem), _core(core) {}
  
    uint32_t read32(unsigned seq,addr_t ea,addr_t ra) { 
      //_core.handle_memory_request(0, MemAccess(ea,ra,4,BytesPerWord - (ra & WordMask),CacheIFetch)); 
      return _mem.read32(ra); 
    };
  
    void readpage(byte_t *data,unsigned n,addr_t addr,addr_t critical_addr) { _mem.readpage(data,n,addr,critical_addr); }

    addr_t misaligned_read_offset32(addr_t ea) { return ea & WordMask; };

    MemType _mem;
    Core   &_core;
  };

  // Same as above, but used when the user has typed memory.
  template<typename MemType, typename Core>
  struct SimpleLoggingMemTypedCache
  {
    SimpleLoggingMemTypedCache(MemType mem,Core &core,int type) : _mem(mem), _core(core), _type((CacheAccess)type) {}
  
    uint64_t read64(unsigned seq,addr_t ea,addr_t ra) 
    { 
      _core.handle_memory_request(0, MemAccess(ea,ra,8,BytesPerDW - (ra & DWMask),_type)); 
      return _mem.read64(ra); 
    };

    uint32_t read32(unsigned seq,addr_t ea,addr_t ra) 
    { 
      _core.handle_memory_request(0, MemAccess(ea,ra,4,BytesPerWord - (ra & WordMask),_type)); 
      return _mem.read32(ra); 
    };

    uint16_t read16(unsigned seq,addr_t ea,addr_t ra) 
    { 
      unsigned nb = ( (ra & WordMask) == 3) ? 1 : 2;
      _core.handle_memory_request(0, MemAccess(ea,ra,2,nb,_type)); 
      return _mem.read16(ra); 
    };

    uint8_t  read8 (addr_t ea,addr_t ra)              
    { 
      _core.handle_memory_request(0, MemAccess(ea,ra,1,1,_type)); 
      return _mem.read8 (ra); 
    };

    uint8_t  read0 (addr_t ea,addr_t ra)              
    { 
      _core.handle_memory_request(0, MemAccess(ea,ra,0,0,_type)); 
      return 0; 
    };
  
    void write64(addr_t ea,addr_t ra,uint64_t v,unsigned nb) { _core.handle_memory_request(0, MemAccess(ea,ra,8,nb,_type)); _mem.write64(ra,v,nb); };
    void write32(addr_t ea,addr_t ra,uint32_t v,unsigned nb) { _core.handle_memory_request(0, MemAccess(ea,ra,4,nb,_type)); _mem.write32(ra,v,nb); };
    void write16(addr_t ea,addr_t ra,uint16_t v,unsigned nb) { _core.handle_memory_request(0, MemAccess(ea,ra,2,nb,_type)); _mem.write16(ra,v,nb); };
    void write8 (addr_t ea,addr_t ra,uint8_t  v)            { _core.handle_memory_request(0, MemAccess(ea,ra,1,1,_type)); _mem.write8 (ra,v);   };
    void write0 (addr_t ea,addr_t ra,uint8_t  v)            { _core.handle_memory_request(0, MemAccess(ea,ra,0,0,_type)); };

    void readpage(byte_t *data,unsigned n,addr_t addr,addr_t critical_addr) { _mem.readpage(data,n,addr,critical_addr); }

    addr_t misaligned_read_offset64(addr_t ea) { return ea & DWMask; };
    addr_t misaligned_read_offset32(addr_t ea) { return ea & WordMask; };
    addr_t misaligned_read_offset16(addr_t ea) { return ea & HWMask; };

    MemType     _mem;
    Core       &_core;
    CacheAccess _type;
  };

  // For safe-mode models with external memory, this wrapper intercepts memory
  // calls and then pushes latency information to the performance model.
  template<typename Core>
  struct LoggingMemWrapper
  {
    LoggingMemWrapper(Core &core) : _core(core), _mem(0) {};

    // For DMI compatibility.
    LoggingMemWrapper(IssNode *node) : _core(dynamic_cast<Core&>(*node)), _mem(0) {};

    void setMemHandler(MemHandler *mh) { _mem = mh; };

    void reset() { if (_mem) _mem->reset(); };

    uint64_t read64(CacheAccess ca,addr_t ra) 
    { 
      uint64_t x = _mem->read64(ca,ra); 
      _core.handle_memory_latency(0, ca,ra,8,_mem->get_latency()); 
      return x; 
    };

    uint32_t read32(CacheAccess ca,addr_t ra)    
    { 
      uint32_t x = _mem->read32(ca,ra); 
      _core.handle_memory_latency(0, ca,ra,4,_mem->get_latency()); 
      return x; 
    };

    uint16_t read16(CacheAccess ca,addr_t ra)
    { 
      uint16_t x = _mem->read16(ca,ra); 
      _core.handle_memory_latency(0, ca,ra,2,_mem->get_latency()); 
      return x; 
    };

    uint8_t  read8(CacheAccess ca,addr_t ra)
    { 
      uint8_t x = _mem->read8(ca,ra); 
      _core.handle_memory_latency(0, ca,ra,1,_mem->get_latency()); 
      return x; 
    };

    uint8_t  read0(CacheAccess ca,addr_t ra)
    { 
      _mem->readpage(ca,0,0,ra,ra); 
      _core.handle_memory_latency(0, ca,ra,0,_mem->get_latency()); 
      return 0; 
    };

    void     write64(addr_t ra, uint64_t v, unsigned nb)
    { 
      _mem->write64(ra,v,nb); 
      _core.handle_memory_latency(0, CacheWrite,ra,8,_mem->get_latency()); 
    };

    void     write32(addr_t ra, uint32_t v, unsigned nb)
    { 
      _mem->write32(ra,v,nb); 
      _core.handle_memory_latency(0, CacheWrite,ra,4,_mem->get_latency()); 
    };

    void     write16(addr_t ra, uint16_t v, unsigned nb)
    { 
      _mem->write16(ra,v,nb); 
      _core.handle_memory_latency(0, CacheWrite,ra,2,_mem->get_latency()); 
    };

    void     write8 (addr_t ra, uint8_t  v)
    { 
      _mem->write8(ra,v); 
      _core.handle_memory_latency(0, CacheWrite,ra,1,_mem->get_latency()); 
    };

    void     write0 (addr_t ra, uint8_t  v)
    { 
      _mem->writepage(0,0,ra,ra); 
      _core.handle_memory_latency(0, CacheWrite,ra,0,_mem->get_latency()); 
    };

    void     readpage(CacheAccess ca,byte_t *t,size_t n,addr_t addr,addr_t critical_addr)
    { 
      _mem->readpage(ca,t,n,addr,critical_addr);
      _core.handle_memory_latency(0, ca == CacheIFetch ? ca : CacheRead,addr,n,_mem->get_latency()); 
    };

    void     writepage(byte_t *s,size_t n,addr_t addr,addr_t critical_addr)
    { 
      _mem->writepage(s,n,addr,critical_addr); 
      _core.handle_memory_latency(0, CacheFlush,addr,n,_mem->get_latency()); 
    };

    void     fillpage(unsigned char c,size_t n,addr_t addr)
    { 
      _mem->fillpage(c,n,addr); 
      _core.handle_memory_latency(0, CacheWrite,addr,n,_mem->get_latency()); 
    };

    Core &_core;
    MemHandler *_mem;
  };

# ifdef TRANSACTIONAL_ISS

  // For transactional ISSs, this is used to simply record addresses.
  // TransCache is then used to perform the access, with coroutine switching.
  template<typename Core>
  struct RecordCache
  {
    RecordCache(Core &core) : _core(core) {}
  
    uint64_t read64(unsigned seq,addr_t ea,addr_t ra) 
    { 
      _core.handle_memory_request(0, MemAccess(ea,ra,8,8,CacheRead)); 
      return 0;
    };

    uint32_t read32(unsigned seq,addr_t ea,addr_t ra) 
    { 
      _core.handle_memory_request(0, MemAccess(ea,ra,4,4,CacheRead)); 
      return 0;
    };

    uint16_t read16(unsigned seq,addr_t ea,addr_t ra) 
    { 
      _core.handle_memory_request(0, MemAccess(ea,ra,2,2,CacheRead)); 
      return 0;
    };

    uint8_t  read8 (addr_t ea,addr_t ra)              
    { 
      _core.handle_memory_request(0, MemAccess(ea,ra,1,1,CacheRead)); 
      return 0;
    };

    uint8_t  read0 (addr_t ea,addr_t ra)              
    { 
      _core.handle_memory_request(0, MemAccess(ea,ra,0,0,CacheRead)); 
      return 0;
    };
  
    void write64(addr_t ea,addr_t ra,uint64_t v,unsigned nb) { _core.handle_memory_request(0, MemAccess(ea,ra,8,nb,CacheWrite)); };
    void write32(addr_t ea,addr_t ra,uint32_t v,unsigned nb) { _core.handle_memory_request(0, MemAccess(ea,ra,4,nb,CacheWrite)); };
    void write16(addr_t ea,addr_t ra,uint16_t v,unsigned nb) { _core.handle_memory_request(0, MemAccess(ea,ra,2,nb,CacheWrite)); };
    void write8 (addr_t ea,addr_t ra,uint8_t  v)             { _core.handle_memory_request(0, MemAccess(ea,ra,1,1,CacheWrite));  };
    void write0 (addr_t ea,addr_t ra,uint8_t  v)             { _core.handle_memory_request(0, MemAccess(ea,ra,0,0,CacheWrite));  };

    addr_t misaligned_read_offset32(addr_t ea) { return ea & WordMask; };
    addr_t misaligned_read_offset64(addr_t ea) { return ea & DWMask; };
    addr_t misaligned_read_offset16(addr_t ea) { return ea & HWMask; };

    Core &_core;
  };

  // Same as above, for supporting when a model has memory types associated with it.
  template<typename Core>
  struct RecordCacheTyped
  {
    RecordCacheTyped(Core &core,int type) : _core(core), _type((CacheAccess)type) {}
  
    uint64_t read64(unsigned seq,addr_t ea,addr_t ra) 
    { 
      _core.handle_memory_request(0, MemAccess(ea,ra,8,8,_type)); 
      return 0;
    };

    uint32_t read32(unsigned seq,addr_t ea,addr_t ra) 
    { 
      _core.handle_memory_request(0, MemAccess(ea,ra,4,4,_type)); 
      return 0;
    };

    uint16_t read16(unsigned seq,addr_t ea,addr_t ra) 
    { 
      _core.handle_memory_request(0, MemAccess(ea,ra,2,2,_type)); 
      return 0;
    };

    uint8_t  read8 (addr_t ea,addr_t ra)              
    { 
      _core.handle_memory_request(0, MemAccess(ea,ra,1,1,_type)); 
      return 0;
    };

    uint8_t  read0 (addr_t ea,addr_t ra)              
    { 
      _core.handle_memory_request(0, MemAccess(ea,ra,0,0,_type)); 
      return 0;
    };
  
    void write64(addr_t ea,addr_t ra,uint64_t v,unsigned nb) { _core.handle_memory_request(0, MemAccess(ea,ra,8,nb,_type)); };
    void write32(addr_t ea,addr_t ra,uint32_t v,unsigned nb) { _core.handle_memory_request(0, MemAccess(ea,ra,4,nb,_type)); };
    void write16(addr_t ea,addr_t ra,uint16_t v,unsigned nb) { _core.handle_memory_request(0, MemAccess(ea,ra,2,nb,_type)); };
    void write8 (addr_t ea,addr_t ra,uint8_t  v)             { _core.handle_memory_request(0, MemAccess(ea,ra,1,1,_type));  };
    void write0 (addr_t ea,addr_t ra,uint8_t  v)             { _core.handle_memory_request(0, MemAccess(ea,ra,0,0,_type));  };

    addr_t misaligned_read_offset32(addr_t ea) { return ea & WordMask; };
    addr_t misaligned_read_offset64(addr_t ea) { return ea & DWMask; };
    addr_t misaligned_read_offset16(addr_t ea) { return ea & HWMask; };

    Core &_core;
    CacheAccess _type;
  };


  // This is used for transactional ISSs.  This is what handles the coroutine
  // swapping in order to actually interact with the pipeline model's memory
  // subsystem.
  template<typename Core>
  struct TransCache
  {
    TransCache(Core &core) : _core(core) {}
    TransCache(Core &core,int dummy) : _core(core) {}
  
    uint64_t read64(unsigned seq,addr_t ea,addr_t ra) { return _core._memory_request.trans_read64(); };
    uint32_t read32(unsigned seq,addr_t ea,addr_t ra) { return _core._memory_request.trans_read32(); };
    uint16_t read16(unsigned seq,addr_t ea,addr_t ra) { return _core._memory_request.trans_read16(); };
    uint8_t  read8 (addr_t ea,addr_t ra)              { return _core._memory_request.trans_read8 (); };
    uint8_t  read0 (addr_t ea,addr_t ra)              { return _core._memory_request.trans_read0 (); };
  
    void write64(addr_t ea,addr_t ra,uint64_t v,unsigned nb) { _core._memory_request.trans_write64(v,nb); };
    void write32(addr_t ea,addr_t ra,uint32_t v,unsigned nb) { _core._memory_request.trans_write32(v,nb); };
    void write16(addr_t ea,addr_t ra,uint16_t v,unsigned nb) { _core._memory_request.trans_write16(v,nb); };
    void write8 (addr_t ea,addr_t ra,uint8_t  v)             { _core._memory_request.trans_write8 (v);    };
    void write0 (addr_t ea,addr_t ra,uint8_t  v)             { _core._memory_request.trans_write0 (v);    };

    addr_t misaligned_read_offset32(addr_t ea) { return ea & WordMask; };
    addr_t misaligned_read_offset64(addr_t ea) { return ea & DWMask; };
    addr_t misaligned_read_offset16(addr_t ea) { return ea & HWMask; };

    Core &_core;
  };

  // A dummy cache provided for logging purposes only,
  struct NullReadCache
  {
  
    uint32_t read32(unsigned seq,addr_t ea,addr_t ra) { return 0; };
    uint64_t read64(unsigned seq,addr_t ea,addr_t ra) { return 0; };

    addr_t misaligned_read_offset32(addr_t ea) { return ea & WordMask; };
  };  

  // This is used for instruction fetches for transactional ISSs.  It doesn't use the coroutine interface,
  // for performance reasons.  Instead, this simply posts requests to the MemoryRequest object.
  template<typename Core>
  struct TransInstrPreFetchCache
  {
    TransInstrPreFetchCache(Core &core) : _core(core) {}
  
    uint64_t read64(unsigned seq,addr_t ea,addr_t ra) { _core._memory_request.addr_instr_request(seq,ea,ra,8); return 0; };
    uint32_t read32(unsigned seq,addr_t ea,addr_t ra) { _core._memory_request.addr_instr_request(seq,ea,ra,4); return 0; };
    uint32_t read16(unsigned seq,addr_t ea,addr_t ra) { _core._memory_request.addr_instr_request(seq,ea,ra,2); return 0; };
    uint32_t read8 (unsigned seq,addr_t ea,addr_t ra) { _core._memory_request.addr_instr_request(seq,ea,ra,1); return 0; };
 
    addr_t misaligned_read_offset32(addr_t ea) { return ea & WordMask; };
    addr_t misaligned_read_offset64(addr_t ea) { return ea & DWMask; };
    addr_t misaligned_read_offset16(addr_t ea) { return ea & HWMask; };

    Core &_core;
  };

  // This is the other half of the instruction-fetch mechanism for transactional ISSs.  Once the request has been
  // serviced, it assembles the read and returns the value.
  template<typename Core>
  struct TransInstrFetchCache
  {
    TransInstrFetchCache(Core &core) : _core(core) {}
  
    uint32_t read32(unsigned seq,addr_t ea,addr_t ra) { return _core._memory_request.trans_instr_read32(seq); };
    uint64_t read64(unsigned seq,addr_t ea,addr_t ra) { return _core._memory_request.trans_instr_read64(seq); };

    addr_t misaligned_read_offset32(addr_t ea) { return ea & WordMask; };
 
    Core &_core;
  };  

# endif // TRANSACTIONAL_ISS

  // Memory interface used by the transactional ISS.  This basically sets up the
  // request, then calls Core::process_memory_request for it to be actually
  // handled.  For a blocking interface, this just calls a virtual function.
  // For a non-blocking interface, this makes use of the Coroutine library to
  // switch coroutines.
  //
  // The second parameter specifies the number of instruction accesses that we
  // support.  If a single fetch can span a word, then this should be set to 2.
  // If a fetch unit will always break this up so that this doesn't happen, then
  // it can be left at 1.
  template <class Core,unsigned NumInstrAccesses>
  struct MemoryRequest {

    struct AccessData : public MemAccess {
      byte_t    *_page;
      unsigned   _offset;
    };

    MemoryRequest(Core &core) : _core(core) { reset(); };

    AccessData    _a[NumInstrAccesses];

    // in the first access returned.
    Core         &_core;

    void reset() { 
      for (unsigned i = 0;i != NumInstrAccesses; ++i) {
        _a[i].reset();
      }
    };

    MemAccess::OpType op(unsigned index) { return _a[index % NumInstrAccesses].op(); };
    MemAccess::OpType op() { return _a[0].op(); };

    const MemAccess &access(unsigned index) const { return _a[index % NumInstrAccesses]; };

    byte_t *ptr(unsigned index) const { return &_a[index % NumInstrAccesses]._page[_a[index % NumInstrAccesses]._offset]; };
    byte_t *page(unsigned index) const { return _a[index % NumInstrAccesses]._page; };

    void set_result(unsigned index,byte_t *page,unsigned offset)
    { 
      _a[index % NumInstrAccesses]._page   = page;
      _a[index % NumInstrAccesses]._offset = offset;
    }

    void cache_request(CacheAccess type,int id,addr_t ra)
    {
      _a[0]._ra = ra;
      _a[0]._c._set = -1;
      _a[0]._c._way = -1;
      _a[0]._id = id;
      _a[0]._type = type;
      _a[0]._op = MemAccess::opCache;
      _core.handle_memory_request(0,_a[0]);
      _core.process_memory_request();
      _a[0].reset();
    }

    void cache_request(CacheAccess type,int id,addr_t ra,int set,int way)
    {
      _a[0]._ra = ra;      
      _a[0]._c._set = set;
      _a[0]._c._way = way;
      _a[0]._id = id;
      _a[0]._type = type;
      _a[0]._op = MemAccess::opCache;
      _core.handle_memory_request(0,_a[0]);
      _core.process_memory_request();
      _a[0].reset();
    }

    void addr_instr_request(unsigned seq,addr_t ea,addr_t ra,unsigned size)
    {
      _a[seq % NumInstrAccesses].set(ea,ra,size,size,CacheIFetch);
    }

    uint32_t trans_instr_read32(unsigned seq)
    {
      return byte_read32(_a[seq % NumInstrAccesses]._page,_a[seq % NumInstrAccesses]._offset);
    };

    uint64_t trans_instr_read64(unsigned seq) 
    {
      return byte_read64(_a[seq % NumInstrAccesses]._page,_a[seq % NumInstrAccesses]._offset);
    };
   
    void swap_to_core()
    {
      _a[0]._op = MemAccess::opMem;
      _core.process_memory_request();;
      _a[0].reset();
    }

    uint64_t trans_read64() 
    {
      swap_to_core();
      return byte_read64(_a[0]._page,_a[0]._offset);
    };

    uint32_t trans_read32() 
    {
      swap_to_core();
      return byte_read32(_a[0]._page,_a[0]._offset);
    }

    uint16_t trans_read16() 
    {
      swap_to_core();
      return byte_read16(_a[0]._page,_a[0]._offset);
    };

    uint8_t  trans_read8 () 
    {
      swap_to_core();
      return byte_read8(_a[0]._page,_a[0]._offset);
    };

    uint8_t  trans_read0 () 
    {
      return 0;
    };
    
    void trans_write64(uint64_t v,unsigned nb)
    {  
      swap_to_core();
      byte_write64(_a[0]._page,_a[0]._offset,v,nb);
    };
    
    void trans_write32(uint32_t v,unsigned nb)
    {  
      swap_to_core();
      byte_write32(_a[0]._page,_a[0]._offset,v,nb);
    };
    
    void trans_write16(uint16_t v,unsigned nb)
    {
      swap_to_core();
      byte_write16(_a[0]._page,_a[0]._offset,v,nb);
    };
    
    void trans_write8 (uint8_t  v)
    {  
      swap_to_core();
      byte_write8(_a[0]._page,_a[0]._offset,v);
    };

    void trans_write0 (uint8_t  v)
    {  
    };
    
  };

  // Default function for when a decode error occurs.
  void unknownInstruction(addr_t cia,uint32_t instr);

  // Returns a zero of the specified size.
  template <size_t Nb>
  inline ns_bits<Nb> zero() { return ns_bits<Nb>(); };

  struct ByName {
    ByName(const std::string &v) : _v(v) {};
    bool operator()(const FieldPair &x) {
      return x._name == _v;
    };
    std::string _v;
  };

  // Lookups up a field; returns the default value if no found.
  inline uint64_t getLookupField(const FieldData &fields,const std::string &n,uint64_t def)
  {
    FieldData::const_iterator i = std::find_if(fields.begin(),fields.end(),ByName(n));
    if (i == fields.end()) {
      return def;
    } else {
      return i->_value;
    }
  }

  template <size_t Nb>
  inline intbv<Nb> getLookupField(const FieldData &fields,const std::string &n,const intbv<Nb> &def = 0) 
  {
    FieldData::const_iterator i = std::find_if(fields.begin(),fields.end(),ByName(n));
    if (i == fields.end()) {
      return def;
    } else {
      return intbv<Nb>(i->_value);
    }
  };


  // Return the index (trailing number) if a name is of the form <name><index>.  In other words,
  // sending this GPR12 would return 12, while sending it GPR0CONTROL would return -1, since 0 is
  // not at the end of the name.
  int checkRegFileName(const std::string &name,unsigned rflen,const char *rfname);

  // Conversion functions for modifying a number thing into a addr_t.
  template <size_t Nb>
  inline addr_t to_addr(const ns_bits<Nb> &x) {
    return x.uint64();
  }

  inline addr_t to_addr(addr_t x) {
    return x;
  }

  // Empty versions of classes required by the memory interface in MemoryInterface.h and Memory.h

  struct NullTranslate {
    addr_t operator()(addr_t a,int) { return a; };
    addr_t scale_ea(addr_t ea) { return ea; };
    addr_t mask_ea(addr_t ea) { return ea; };
    void aligned_write(addr_t,addr_t) { };
    void misaligned_write(addr_t,addr_t) { };
    void misaligned_read(addr_t,addr_t) { };
    void pre_read(addr_t,addr_t) {};
    void post_read(addr_t,addr_t) {};
    void pre_write(addr_t,addr_t) {};
    void post_write(addr_t,addr_t) {};
  };

  struct NullInstrLog {
    NullInstrLog(IssNode &) {};
    void operator()(bool pre,int seq,addr_t ea,addr_t ra,int num_bytes,uint64_t value) {}
  };

  struct NullReadLog {
    void operator()(bool pre,int seq,addr_t ea,addr_t ra,int num_bytes,uint64_t value) {}
  };

  struct NullWriteLog {
    void operator()(bool pre,int seq,addr_t ea,addr_t ra,int num_bytes,uint64_t value) {}
  };

  // Creates an address with the high bits (as specified from the mask) from the
  // rpn and the low-bits from the ea.
  inline addr_t make_addr(addr_t rpn,addr_t mask,addr_t ea)
  {
    return (rpn | (ea & ~mask));
  }

  // Register a handler on a given bus.  Throws a runtime_error if the signature does not
  // match against a registered bus of the same name.  Updates handle and id to reflect the 
  // handle to the bus and the unique ID to store, so that when sending, the sender's event
  // handler is not called.
  void register_eventbus(int &handle,int &id,const std::string &name,const std::string &sig,EventBusHandler &);

  // Signal an event on a bus.  The void* is a pointer to the data packet.  The
  // cast to the real data type should be okay b/c of our type-checking using the
  // signature string.
  void signal_eventbus(int handle,int id,const void *);

  // Information about an operand, plus a mask.  The mask is only valid when the
  // _is_operand flag is false.  This indicates that this item represents a side
  // effect register modification, e.g. caused by a register read or write hook.
  // If the flag is true, then it represents the actual operand, and the slot's
  // masks should be examined for what was read or written.
  template <size_t Nb>
  struct OpInfoM : public OpInfo {
    typedef intbv<Nb> mask_t;

    OpInfoM() : _is_operand(false) {};
    OpInfoM(unsigned id,int index,unsigned t,bool i,const mask_t &m) :
      OpInfo(id,index,t), 
      _is_operand(i), 
      _mask(m) 
    {};
    OpInfoM(unsigned id,int index,unsigned t,bool i) : 
      OpInfo(id,index,t), 
      _is_operand(i), 
      _mask(mask_t()) 
    {};

    bool   _is_operand;
    mask_t _mask;
  };

  template <size_t Nb>
  struct OpInfoMs : public  std::vector<OpInfoM<Nb> > {
    typedef OpInfoM<Nb> base_t;
    typedef typename base_t::mask_t mask_t;
  };



  template <class Packet,class Parent>
  void printOpInfoTrans(std::ostream &o,unsigned index,const Packet &packet,const OpInfo &op,bool print_value)
  {
    o << Parent::baseRegInfo()[op._id]._name;
    if (Parent::baseRegInfo()[op._id]._size != 0) {
      o << "[" << std::dec << op._index << "]";
    }
    if (print_value) {
      o << "=0x" << std::hex << packet.slot(index) << std::dec;
    }
  }

  template <class Packet,class Parent>
  bool printOperand(std::ostream &o,unsigned index,const Packet &packet,bool resolve) {
    if ( packet.is_dirty(index) || packet.is_op_read(index) ) {
      if (resolve) {
        // We only have data for the first item, so for the others, use the
        // hybrid printing.
        typename Parent::CoreOpInfoMs ops;
        packet.getResolvedOp(ops,index);
        if (!ops.empty()) {
					typename Parent::CoreOpInfoMs::iterator i = ops.begin();
          printOpInfoTrans<Packet,Parent>(o,index,packet,*i,true);
          for ( ++i ; i != ops.end(); ++i) {
            o << ' ';
            printOpInfoTrans<Packet,Parent>(o,index,packet,*i,false);
          }
        }
      } else {
        printOpInfoTrans<Packet,Parent>(o,index,packet,packet.getOp(index),true);        
      }
      return true;
    } else {
      return false;
    }
  }

  template <class Packet,class Parent>
  void printOpInfoHybrid(std::ostream &o,unsigned index,const Packet &packet,const OpInfo &op,bool read,bool written)
  {
    const RegInfo &ri = Parent::baseRegInfo()[op._id];
    o << ri._name;
    if (ri._size) {
      o << "[" << std::dec << op._index << "]";
    }
    o << "=(";
    if (written) {
      o << "written";
      if (op._type & OpInfo::opPartial) {
        o << "(mask=0x" << std::hex << packet.write_mask(index) << std::dec << ")";
      }
      o << ' ';
    } 
    if (read) {
      o << "read";
      if (op._type & OpInfo::opPartial) {
        o << "(mask=0x" << std::hex << packet.read_mask(index) << std::dec << ")";
      }
    }
    o << ")";
  }

  template <class Packet,class Parent>
  void printOpInfoStaticHybrid(std::ostream &o,unsigned index,const Packet &packet,const typename Parent::CoreOpInfoM &op)
  {
    const RegInfo &ri = Parent::baseRegInfo()[op._id];
    o << ri._name;
    if (ri._size) {
      o << "[" << std::dec << op._index << "]";
    }
    o << "=(";
    if (op._type & OpInfo::opTrg) {
      o << "written";

    } 
    if (op._type & OpInfo::opSrc) {
      o << " read";
    }
    if (op._mask.any()) {
      o << " (mask=0x" << std::hex << op._mask << std::dec << ")";
    }
    o << ")";
  }

  // For hybrid ISSs (which don't have values).
  template <class Packet,class Parent>
  bool printOperandH(std::ostream &o,unsigned index,const Packet &packet,bool resolve) {
    bool written = false;
    bool read    = false;
    if ( packet.is_dirty(index)) {
      written = true;
    } 
    if (packet.is_op_read(index) ) {
      read = true;
    }

    if (written || read) {
      if (resolve) {
        typename Parent::CoreOpInfoMs ops;
        packet.getResolvedOp(ops,index);
        for (typename Parent::CoreOpInfoMs::const_iterator i = ops.begin(); i != ops.end(); ++i) {
          if (i->_is_operand) {
            printOpInfoHybrid<Packet,Parent>(o,index,packet,*i,read,written);
          } else {
            printOpInfoStaticHybrid<Packet,Parent>(o,index,packet,*i);
          }
          o << ' ';
        }
      } else {
        printOpInfoHybrid<Packet,Parent>(o,index,packet,packet.getOp(index),read,written);
      }
      return true;
    } else {
      return false;
    }
  }

  //
  // Exception support class for transactional ISSs.
  //

  struct ExBase {
    virtual ~ExBase() {};
  };

  struct ExItems {

    ExItems(unsigned n) : _ref(1), _items(new ExBase*[n])
    {
      for (unsigned i = 0; i != n; ++i) {
        _items[i] = 0;
      }
    }
    ~ExItems() { delete [] _items; };

    unsigned _ref;
    ExBase **_items;
  };

  // These classes are used for delayed commit in parallel architectures

  struct CommitWrite {
    CommitWrite() : _nextp(0) {};
    virtual ~CommitWrite() {};
    virtual void  commit() const {};
    virtual void  free() {} ;

    CommitWrite *_nextp;
  };

  // A restore stack is basically the same as a commit queue, except that it acts 
  // like a stack:  Elements are pushed onto the head, so that the commit acts
  // in reverse order:  We want to restore the last instruction executed before we restore the
  // instruction before that, so that our restores happen in reverse-program order.
  struct RestoreStack {

    RestoreStack() : _head(0) {};

    // We add to the front, rather than to the back.
    void add(CommitWrite* commitWrite)
    {
      if (!_head) {
        _head = commitWrite;
        commitWrite->_nextp = 0;
      } else {
        commitWrite->_nextp = _head;
        _head = commitWrite;
      }
    }

    // Scan from the front to the back, commiting elements and freeing them as
    // we go.
    void commit (const CommitWrite *last=0)
    {
      CommitWrite *tmp = 0;
      while (_head != last) {
        tmp = _head;
        _head = _head->_nextp;
        tmp->commit();
        tmp->free();
      }
    }

    void reset()
    {
      CommitWrite *tmp = 0;
      while (_head) {
        tmp = _head;
        _head = _head->_nextp;
        tmp->free();
      }
    }

    // This is a destructive assignment- it takes the items from the argumeent
    // and clears the argument.  Our own value are lost, therefore it must be
    // clear before being used or else a leak will result.  This is simply
    // checked through an assertion.
    void set(RestoreStack &x)
    {
      assert(!_head);
      _head = x._head;
      x._head = 0;
    }

  protected:
    CommitWrite *_head;
  };

  // A simple queue of commit objects.  Items are added to the back.  Committing
  // occurs from the front to the back.  This is derived from RestoreStack,
  // since some of the functionality is in common.
  struct CommitQueue : private RestoreStack {

    CommitQueue() : _tail(0) {};

    // Add to the back of the list.
    void add(CommitWrite* commitWrite) 
    { 
      if (!_head) {
        _head = _tail = commitWrite;
      } else {
        _tail->_nextp = commitWrite;
        _tail = commitWrite;
      }
      commitWrite->_nextp = 0;
    }

    void commit() {
      RestoreStack::commit();
      _tail = 0;
    }

    void reset() {
      RestoreStack::commit();
      _tail = 0;
    }

    // This is a destructive assignment- it takes the items from the argumeent
    // and clears the argument.  Our own value are lost, therefore it must be
    // clear before being used or else a leak will result.  This is simply
    // checked through an assertion.
    void set(CommitQueue &x)
    {
      RestoreStack::set(x);
      _tail = x._tail;
      x._tail = 0;
    }
    
  protected:
    CommitWrite *_tail;
  };


  // The outer class contains D commit queues, where each commit queue
  // represents elements to commit at a particular cycle in the future.  This is
  // treated as a circular queue- elements added at current+1 are current, and
  // thus committed, at the end of the next cycle.
  //
  // D:  Number of delay slots.  Slot 0 is always 
  //     "the end of this cycle", 1 is next cycle, etc.
  template<size_t D>
  struct CommitQueues {
    CommitQueues() : _cur(0) {};

    void add(CommitWrite *commitWrite,unsigned delay)
    {

      _q[(_cur+delay)%D].add(commitWrite);
    }

    void commit()
    {
      _q[_cur].commit();
    }

    void next()
    {
      _cur = (_cur+1) % D;
    }


    CommitQueue    _q[D];
    unsigned       _cur;
  };

  template<class C,typename Func, size_t Nb, unsigned Size>
  struct CommitMemWrite : public CommitWrite {
    CommitMemWrite() : _pool(0), _next(0) {}
    void  reset(C c, Func &func, addr_t addr,const ns_bits<Nb>& value,PacketPool< CommitMemWrite<C,Func,Nb,Size>, Size> &p) 
    { _c  = c; _write = func; _addr = addr; _val = value; _pool = &p; }
    
    virtual void free() { _pool->free(this); };  
    virtual void commit() const { (static_cast<C>(_c)->*_write)(_addr,_val); }
    
    void       *_c;
    Func        _write;
    addr_t     	_addr;
    ns_bits<Nb>   	_val;
    
    PacketPool< CommitMemWrite<C,Func,Nb,Size>, Size> *_pool;
    CommitMemWrite<C,Func,Nb,Size> *_next;
  };

  template<class C,typename Func, size_t Nb, unsigned Size>
  struct CommitTypedMemWrite : public CommitWrite {
    CommitTypedMemWrite() : _pool(0), _next(0) {}
    void  reset(C c, Func &func, addr_t addr,const ns_bits<Nb>& value, CacheAccess type,PacketPool< CommitTypedMemWrite<C,Func,Nb,Size>, Size> &p) 
    { _c  = c; _write = func; _addr = addr; _val = value; _type = type; _pool = &p; }
    
    virtual void free() { _pool->free(this); };    
    virtual void commit() const { (static_cast<C>(_c)->*_write)(_addr,_val,_type); }
    
    void       *_c;
    Func        _write;
    addr_t     	_addr;
    CacheAccess _type;
    ns_bits<Nb>   	_val;
    
    PacketPool< CommitTypedMemWrite<C,Func,Nb,Size>, Size> *_pool;
    CommitTypedMemWrite<C,Func,Nb,Size> *_next;
  };


  template<typename C, typename Func, size_t Nb, unsigned Size> 
  struct CommitRegFieldWrite : public CommitWrite {
    CommitRegFieldWrite() : _pool(0), _next(0) {};

    void  reset(C c, Func &func,const ns_bits<Nb>& value, PacketPool<CommitRegFieldWrite<C,Func,Nb,Size>, Size> &p ) 
    { _c  = c; _write = func; _val = value; _pool = &p; }

    virtual void free() { _pool->free(this); };
    virtual void commit() const { (static_cast<C>(_c)->*_write)(_val); }

    void 	      *_c;
    Func         _write ;
    ns_bits<Nb>     _val;  

    PacketPool< CommitRegFieldWrite<C,Func,Nb,Size>, Size> *_pool;
    CommitRegFieldWrite<C,Func,Nb,Size> *_next;
  };

  template<typename C, typename Func, size_t Nb, unsigned Size> 
  struct CommitRegWrite : public CommitWrite {
    CommitRegWrite() : _pool(0), _next(0)  {}

    void  reset(C c, Func &func,const ns_bits<Nb>& value, PacketPool< CommitRegWrite<C,Func,Nb,Size>, Size> &p) 
    { _c  = c; _write = func; _val = value; _pool = &p; }

    virtual void free() { _pool->free(this); };
    virtual void commit() const { (static_cast<C>(_c)->*_write)(_val); }

    void 	      *_c;
    Func         _write ;
    ns_bits<Nb>     _val;  

    PacketPool< CommitRegWrite<C,Func,Nb,Size>, Size> *_pool;
    CommitRegWrite<C,Func,Nb,Size> *_next;
  };

  template<class C, typename Func, size_t Nb, unsigned Size>
  struct CommitRegSliceSet : public CommitWrite {
    CommitRegSliceSet() : _pool(0), _next(0) {};
    void reset(C c, Func &func, unsigned x, unsigned y, const ns_bits<Nb> &v, PacketPool< CommitRegSliceSet<C,Func,Nb,Size>, Size> &p)
    { _c = c; _write = func; _x = x; _y = y ; _val = v; _pool = &p; };

    virtual void free() { _pool->free(this); };
    virtual void commit() const { (static_cast<C>(_c)->*_write)(_x, _y, _val); } 

    void      *_c;
    Func       _write;
    unsigned   _x;
    unsigned   _y;
    ns_bits<Nb>   _val;
    
    PacketPool< CommitRegSliceSet<C,Func,Nb,Size>, Size> *_pool;
    CommitRegSliceSet  *_next;
  };


  template<class C, typename Func, size_t Nb, unsigned Size> 
  struct CommitRegfileWrite : public CommitWrite {
    CommitRegfileWrite() : _pool(0), _next(0) {} 
    void reset(C c,Func &func, unsigned index, ns_bits<Nb> val, PacketPool<CommitRegfileWrite<C,Func,Nb,Size>, Size> &p)  
    {_c = c;  _write = func; _index = index;  _val = val; _pool = &p; }

    virtual void free() { _pool->free(this); };
    virtual void commit() const { (static_cast<C>(_c)->*_write)(_index,_val); }

    void        *_c;
    Func        _write;
    unsigned    _index;
    ns_bits<Nb>    _val;  

    PacketPool< CommitRegfileWrite<C,Func,Nb,Size>, Size> *_pool;
    CommitRegfileWrite<C,Func,Nb,Size>	*_next;
  }; 


  template<class C, typename Func, size_t Nb, unsigned Size> 
  struct CommitRegIndexedFieldWrite : public CommitWrite {
    CommitRegIndexedFieldWrite() : _pool(0), _next(0) {} 
    void reset(C c,Func &func, unsigned index, ns_bits<Nb> val, PacketPool<CommitRegIndexedFieldWrite<C,Func,Nb,Size>, Size> &p)  
    {_c = c;  _write = func; _index = index;  _val = val; _pool = &p; }

    virtual void free() { _pool->free(this); };


    virtual void commit() const { (static_cast<C>(_c)->*_write)(_index,_val); }

    void        *_c;
    Func        _write;
    unsigned    _index;
    ns_bits<Nb>    _val;  

    PacketPool< CommitRegIndexedFieldWrite<C,Func,Nb,Size>, Size> *_pool;
    CommitRegIndexedFieldWrite<C,Func,Nb,Size>	*_next;
  }; 

  template<class C, typename Func, size_t Nb, unsigned Size> 
  struct CommitRegfileFieldWrite : public CommitWrite {
    CommitRegfileFieldWrite() : _pool(0), _next(0) {} 
    void reset(C c,Func &func, unsigned index, ns_bits<Nb> val, PacketPool<CommitRegfileFieldWrite<C,Func,Nb,Size>, Size> &p)  
    {_c = c;  _write = func; _index = index;  _val = val; _pool = &p; }

    virtual void free() { _pool->free(this); };
    virtual void commit() const { (static_cast<C>(_c)->*_write)(_index,_val); }

    void       *_c;
    Func        _write;
    unsigned    _index;
    ns_bits<Nb>    _val;  

    PacketPool< CommitRegfileFieldWrite<C,Func,Nb,Size>, Size> *_pool;
    CommitRegfileFieldWrite<C,Func,Nb,Size>	*_next;
  }; 


  template<class C, typename Func, size_t Nb, unsigned Size>
  struct CommitRegfileSliceSet : public CommitWrite {
    CommitRegfileSliceSet() : _pool(0), _next(0) {} 
    void reset(C c,Func &func, unsigned index, unsigned x, unsigned y, const ns_bits<Nb> &v, 
               PacketPool<CommitRegfileSliceSet<C,Func,Nb,Size>,Size > &p) 
    {  _c = c; _write = func;  _index = index; _x = x; _y = y; _val = v;  _pool =&p; }

    virtual void commit() const { (static_cast<C>(_c)->*_write)(_index, _x, _y, _val); }   
    virtual void free() { _pool->free(this); };

    void      *_c;
    Func       _write;
    unsigned   _index;
    unsigned   _x;
    unsigned   _y;
    ns_bits<Nb>   _val;

    PacketPool< CommitRegfileSliceSet<C,Func,Nb,Size>, Size> *_pool;
    CommitRegfileSliceSet<C,Func,Nb,Size>	*_next;
  };

  template<class C, typename Func, size_t Nb, unsigned Size>
  struct CommitRegfileIndexedFieldWrite : public CommitWrite {
    CommitRegfileIndexedFieldWrite() : _pool(0), _next(0) {} 
    void reset(C c,Func &func, unsigned index, unsigned findex, const ns_bits<Nb> &v, 
               PacketPool<CommitRegfileIndexedFieldWrite<C,Func,Nb,Size>, Size> &p) 
    {  _c = c; _write = func;  _index = index; _findex = findex; _val = v;  _pool =&p; }

    virtual void commit() const { (static_cast<C>(_c)->*_write)(_index, _findex, _val); }   
    virtual void free() { _pool->free(this); };

    void      *_c;
    Func       _write;
    unsigned   _index;
    unsigned   _findex;
    ns_bits<Nb>   _val;

    PacketPool< CommitRegfileIndexedFieldWrite<C,Func,Nb,Size>, Size> *_pool;
    CommitRegfileIndexedFieldWrite<C,Func,Nb,Size>	*_next;
  };

  template<class C, typename Func, size_t Nb, unsigned Size>
  struct CommitRegMaskSet : public CommitWrite {
    CommitRegMaskSet() : _pool(0), _next(0) {} 
    void reset(C c,Func &func,const ns_bits<Nb> &m, const ns_bits<Nb> &v, 
               PacketPool<CommitRegMaskSet<C,Func,Nb,Size>, Size> &p)
    {  _c = c; _write = func; _v = v; _m = m;  _pool =&p; }

    virtual void commit() const { (static_cast<C>(_c)->*_write)(_v, _m); }   
    virtual void free() { _pool->free(this); };

    void      *_c;
    Func       _write;
    ns_bits<Nb>   _v;
    ns_bits<Nb>   _m;

    PacketPool< CommitRegMaskSet<C,Func,Nb,Size>, Size> *_pool;
    CommitRegMaskSet <C,Func,Nb,Size>	*_next;
  };

  template<class C, typename Func, size_t Nb, unsigned Size>
  struct CommitRegFileMaskSet : public CommitWrite {
    CommitRegFileMaskSet() : _pool(0), _next(0) {} 
    void reset(C c,Func &func, unsigned index,const ns_bits<Nb> &m, const ns_bits<Nb> &v, 
               PacketPool<CommitRegFileMaskSet<C,Func,Nb,Size>, Size> &p)
    {  _c = c; _write = func;  _index = index; _v = v; _m = m;  _pool =&p; }

    virtual void commit() const { (static_cast<C>(_c)->*_write)(_index, _v, _m); }    
    virtual void free() { _pool->free(this); };

    void      *_c;
    Func       _write;
    unsigned   _index;
    ns_bits<Nb>   _v;
    ns_bits<Nb>   _m;

    PacketPool< CommitRegFileMaskSet<C,Func,Nb,Size>, Size> *_pool;
    CommitRegFileMaskSet <C,Func,Nb,Size>	*_next;
  };


  template<size_t Nb>
  struct BundleElem  {
    BundleElem() : _commit(0),_addr(0) {};
    virtual ~BundleElem() {};

    void reset(CommitWrite *commit, addr_t addr, ns_bits<Nb> &v, ns_bits<Nb> &m)
    { 
      _commit = commit; 
      _addr = addr; 
      _v = v; 
      _m = m; 
    }

    virtual ns_bits<Nb> value() const 
    { 
      return _v; 
    }
    virtual ns_bits<Nb> mask()  const 
    {
      return _m; 
    }
    virtual unsigned index() const 
    { 
      return 0;  
    }
    addr_t    addr() const 
    { 
      return _addr; 
    }
    virtual void free() = 0 ;

    ns_bits<Nb>     _v; 
    ns_bits<Nb>     _m;
    CommitWrite *_commit;
    addr_t       _addr;
  };


  template<size_t Nb>
  struct RegBundleElem : public BundleElem<Nb>  {
    typedef BundleElem<Nb> Base;
    RegBundleElem() : _next(0), _pool(0) {};
    ~RegBundleElem() {};

    void reset(CommitWrite *commit, addr_t addr, ns_bits<Nb> &v, ns_bits<Nb> &m,
               PacketPool< RegBundleElem<Nb>,1000> &pool) 
    { 
      Base::reset(commit,addr,v,m) ; 
      _pool = &pool; 
    }

    void free()            
    { 
      _pool->free(this); 
    }

    RegBundleElem<Nb> *_next;
    PacketPool< RegBundleElem<Nb>,1000> *_pool;
  };


  template<size_t Nb>
  struct RegfileBundleElem  : public BundleElem<Nb> {
    typedef BundleElem<Nb> Base;
    RegfileBundleElem() :  _next(0), _pool(0) {};
    ~RegfileBundleElem() {};

    void reset(CommitWrite *commit, addr_t addr, unsigned index, ns_bits<Nb> &v, ns_bits<Nb> &m,
               PacketPool< RegfileBundleElem<Nb>,1000> &pool) 
    { 
      Base::reset(commit,addr,v,m) ; 
      _index = index; 
      _pool = &pool; 
    }

    unsigned index() const 
    { 
      return _index; 
    }
    void free()   
    { 
      _pool->free(this); 
    }

    unsigned  _index;
    RegfileBundleElem<Nb> *_next;
    PacketPool< RegfileBundleElem<Nb>,1000> *_pool;
  };

  template<size_t Nb>
  struct ValueBundle  : public std::vector< BundleElem<Nb>* > {
    typedef std::vector< BundleElem<Nb>* > Base;
    ValueBundle() {};
      
    const BundleElem<Nb> &operator[](unsigned i) const
    { 
      return *(Base::operator[](i));
    }
    void free()  
    { 
      for(unsigned i=0; i < Base::size(); ++i) { 
        Base::operator[](i)->free();
      }
    }
  };

  template<size_t Nb,size_t D>
  struct Bundles {
    Bundles() : _cur(0) {};

    void add(BundleElem<Nb> *el, unsigned delay) 
    {
      _b[(_cur+delay)%D].push_back(el);
    }
      
    const ValueBundle<Nb> &operator[](unsigned delay) const 
    {
      return _b[(_cur+delay)%D]; 
    }

    void next() 
    {
      _cur = (_cur+1)%D;
    }

    void clear(unsigned delay) 
    {
      _b[(_cur+delay)%D].free();
      _b[(_cur+delay)%D].clear();
    }

    ValueBundle<Nb>  _b[D];
    unsigned  _cur;
  };

  //
  // Routines for modifying dynamic parameters.
  //

  // Cache parameters.
  bool set_model_cache_parm (unsigned &linesize,unsigned &linemask,unsigned &lineshift,
                             unsigned &sets,unsigned &ways,const std::string &pfx,const std::string &parm,unsigned value);
  bool get_model_cache_parm (unsigned &value,const std::string &pfx,const std::string &parm,
                             unsigned linesize,unsigned sets,unsigned ways);
  void list_model_cache_parm(const std::string &pfx,const std::string &name,StrPairs &parms);

  // MMU lookup parameters.
  bool set_model_lookup_parm (unsigned *sets,unsigned *set_mask,unsigned *set_shift,
                              unsigned &ways,const std::string &pfx,const std::string &parm,unsigned value);
  bool get_model_lookup_parm (unsigned &value,const std::string &pfx,const std::string &parm,
                              unsigned sets,unsigned ways);
  void list_model_lookup_parm(const std::string &pfx,const std::string &name,StrPairs &parms,bool fully_assoc);

  //
  // Dependency tracking support.
  //

  struct DepTracking {
    // A DtItem is used as a source or target pattern, whereas a DepItem conveys
    // final information about a particular item.  The sequence number is used
    // as a relative counter- each hit which matches (except for the sequence
    // number) bumps the counter.
    struct DtPattern : DepItem {
      DtPattern(DepItem::Type t,unsigned id,int index,unsigned snum) : 
        DepItem(t,id,snum), 
        _index(index),
        _cur_snum(0) {};

      bool match(DepItem::Type t,unsigned id,unsigned index)
      {
        if (t == _type && id == _id) {
          if (_index >= 0 && (int)index != _index) {
            return false;
          }
          if (_cur_snum < _snum) {
            ++_cur_snum;
            return false;
          }
          return true;
        }
        return false;
      }

      int      _index;
      unsigned _cur_snum;
    };
    typedef std::list<DtPattern> DtPatterns;

    struct DepTrackingItems : public DepItems {
      DepTrackingItems(DepItem::Type type,unsigned id,int index,unsigned snum) : 
        _trgp(type,id,index,snum)
      {};

      // Here, snum is used as the identifying sequence number.  If we get a
      // match, we store the value so that it can be correlated with a read
      // operation.
      void check_src(DepItem::Type type,unsigned id,unsigned index,unsigned snum)
      {
        for (DtPatterns::iterator i = _srcs.begin(); i != _srcs.end(); ++i) {
          if (i->match(type,id,index)) {
            // We found a match, so add to the list of sources.  Store the
            // sequence number for correlation purposes.
            push_back(DepItem(type,id,snum));
          }
        }
      }

      DepItems *check_trg(DepItem::Type type,unsigned id,unsigned index)
      {
        return (_trgp.match(type,id,index)) ? this : 0;
      }

      DtPattern   _trgp;
      DtPatterns  _srcs;
    };

    typedef std::list<DepTrackingItems> DepItemsList;

    const DepItems &last_hit() const { return _last_hit; };

    void clear() {
      _items.clear();
      _last_hit.clear();
    }

    void add_trg(DepItem::Type type,unsigned id,unsigned index,unsigned snum)
    {
      _items.push_back(DepTrackingItems(type,id,index,snum));
    }

    template <size_t Nb>
    void add_trg(DepItem::Type type,unsigned id,const intbv<Nb> &index,unsigned snum)
    {
      add_trg(type,id,index.uint32(),snum);
    }


    void add_src(DepItem::Type type,unsigned id,unsigned index,unsigned snum)
    {
      _items.back()._srcs.push_back(DtPattern(type,id,index,snum));
    }

    template <size_t Nb>
    void add_src(DepItem::Type type,unsigned id,const intbv<Nb> &index,unsigned snum)
    {
      add_src(type,id,index.uint32(),snum);
    }


    // Check to see if a source (register read or memory read) matches any
    // source patterns.  If so, store the item.
    void check_src(DepItem::Type type,unsigned id,int index,unsigned snum)
    {
      for (DepItemsList::iterator i = _items.begin(); i != _items.end(); ++i) {
        i->check_src(type,id,index,snum);
      }
    }

    // Check to see if this target matches any target pattern.  If it does,
    // stores the result in _last_hit.
    void check_trg(DepItem::Type type,unsigned id,int index)
    {
      _last_hit.clear();
      for (DepItemsList::iterator i = _items.begin(); i != _items.end(); ++i) {
        if (DepItems *trg = i->check_trg(type,id,index)) {
          _last_hit.swap(*trg);
        }
      }
    }

    DepItemsList    _items;
    DepItems        _last_hit;
  };

  // Copy data from an external buffer to the internal instruction buffer for
  // direct execution of instruction data, bypassing fetch.
  void copy_to_instr_buf(uint32_t *trg,uint32_t *src,unsigned t_n,unsigned s_n,bool byte_swap);


 
  // Copy the instruction-type field to the instruction buffer
  template <size_t Nb>
  void fill_in_instr_buf(intbv<Nb> &instr_field, uint32_t* instr_buf, unsigned num, unsigned max_num, bool is_little_endian)
  {
    // num = instr_table_width/32;  // The table width is round to 32-bits words, safe to /32
    // unsigned max_num = _core->instr_width()/32; // The max table width is round to 32-bits words, safe to /32.
    unsigned remainder = Nb % 32;
    remainder = remainder? remainder: 32; 
    unsigned shift = 32 - remainder;
    
    if (is_little_endian) 
    {   // Little endianess for instruction, so keep in lower bits.
        instr_buf[0] = instr_field.uint32(); 
        for (uint32_t i = 1; i < num; ++i) {
          instr_buf[i] = ( instr_field >> (i*32)).uint32();
        }
        
        // A temporary patch to deal with the problem due to width inconsistency, for some getter functions may fetch instr_buf[x>0]
        if ((num == 1) && (num < max_num)) {
            for (uint32_t i = 1; i < max_num; ++i) {
              instr_buf[i] = instr_buf[0];
            }
        }
    } 
    else 
    {   //Big endianess for instruction, so move to high bits.
        instr_buf[num-1] = instr_field.uint32() << shift;
        for (uint32_t i = 1; i < num; ++i) { instr_buf[num-1-i] = (instr_field >> ((i-1)*32+remainder)).uint32(); }
        
        // A temporary patch to deal with the problem due to width inconsistency, for some getter functions may fetch instr_buf[x>0]
        if ((num == 1) && (num < max_num)) {
            for (uint32_t i = 1; i < max_num; ++i) { instr_buf[i] = instr_buf[0]; }
        }
        
    }
  }

  
  // Display some text info
  void disp(const char *);

  // Display 32-bit number with text
  void disp(const char *, uint32_t, bool in_hex = true); 
  
}

#endif

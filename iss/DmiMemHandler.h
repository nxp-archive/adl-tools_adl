//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This DMI handler is instantiated internally within a design that's built with
// an external memory DMI cache.  Any traffic which misses in the DMI cache is
// then sent to the usual MemHandler object.
//

#ifndef _DMIMEMHANDLER_H_
#define _DMIMEMHANDLER_H_

#include "iss/MemHandler.h"

namespace adl {

  // For use with external memories.
  struct DmiExternalMem {
    // The node pointer is not used for external memories.
    DmiExternalMem(IssNode *node) : _mem(0) {};

    void setMemHandler(MemHandler *mem) { _mem = mem; };

    uint64_t read64(CacheAccess ca,addr_t ra) { return _mem->read64(ca,ra); };
    uint32_t read32(CacheAccess ca,addr_t ra) { return _mem->read32(ca,ra); };
    uint16_t read16(CacheAccess ca,addr_t ra) { return _mem->read16(ca,ra); };
    uint8_t  read8 (CacheAccess ca,addr_t ra) { return _mem->read8 (ca,ra); };
  
    void write64(addr_t ra,uint64_t v,unsigned nb) { _mem->write64(ra,v,nb); };
    void write32(addr_t ra,uint32_t v,unsigned nb) { _mem->write32(ra,v,nb); };
    void write16(addr_t ra,uint16_t v,unsigned nb) { _mem->write16(ra,v,nb); };
    void write8 (addr_t ra,uint8_t  v)            { _mem->write8 (ra,v);   };

    void readpage(CacheAccess ca,byte_t *t,size_t n,addr_t addr,addr_t critical_addr) { _mem->readpage(ca,t,n,addr,critical_addr); };
    void writepage(byte_t *s,size_t n,addr_t addr,addr_t critical_addr) { _mem->writepage(s,n,addr,critical_addr); };
    void fillpage(unsigned char c,size_t n,addr_t addr) { _mem->fillpage(c,n,addr); };

    void reset() { if (_mem) _mem->reset(); };

    MemHandler *_mem;
  };

# ifdef ADL_GLOBAL_MEMORY

  // For use with internal memories.  This is primarily just for testing
  // purposes, as there's no point because there won't be any speedup, since
  // we're not modeling a bus internally.
  //
  // On any miss, we update the DMI cache, since all memory is cacheable with an
  // internal memory.
  struct DmiInternalMem {
    DmiInternalMem(IssNode *node) : _node(node) {};

    void setMemHandler(MemHandler *mem) {};

    uint64_t read64(CacheAccess ca,addr_t ra) 
    { 
      uint64_t tmp = mem.read64(ra); 
      _node->update_dmi_cache_entry(ca,ra,mem.last_page(ra));
      return tmp;      
    }

    uint32_t read32(CacheAccess ca,addr_t ra) 
    {
      uint32_t tmp = mem.read32(ra); 
      _node->update_dmi_cache_entry(ca,ra,mem.last_page(ra));
      return tmp;      
    }

    uint16_t read16(CacheAccess ca,addr_t ra)
    {
      uint16_t tmp = mem.read16(ra); 
      _node->update_dmi_cache_entry(ca,ra,mem.last_page(ra));
      return tmp;      
    }

    uint8_t  read8 (CacheAccess ca,addr_t ra)\
    {
      uint8_t tmp = mem.read8(ra); 
      _node->update_dmi_cache_entry(ca,ra,mem.last_page(ra));
      return tmp;      
    }
  
    void write64(addr_t ra,uint64_t v,unsigned nb)
    {
      mem.write64(ra,v,nb);
      _node->update_dmi_cache_entry(CacheWrite,ra,mem.last_page(ra));
    }

    void write32(addr_t ra,uint32_t v,unsigned nb)
    {
      mem.write32(ra,v,nb);
      _node->update_dmi_cache_entry(CacheWrite,ra,mem.last_page(ra));
    }

    void write16(addr_t ra,uint16_t v,unsigned nb)
    {
      mem.write16(ra,v,nb);
      _node->update_dmi_cache_entry(CacheWrite,ra,mem.last_page(ra));
    }

    void write8 (addr_t ra,uint8_t  v)
    {
      mem.write8(ra,v);
      _node->update_dmi_cache_entry(CacheWrite,ra,mem.last_page(ra));
    }

    void readpage(CacheAccess ca,byte_t *t,size_t n,addr_t addr,addr_t critical_addr)
    {
      mem.readpage(t,n,addr,critical_addr);
      _node->update_dmi_cache_entry(ca,addr,mem.last_page(addr));
    }

    void writepage(byte_t *s,size_t n,addr_t addr,addr_t critical_addr)
    {
      mem.writepage(s,n,addr,critical_addr);
      _node->update_dmi_cache_entry(CacheWrite,addr,mem.last_page(addr));
    }

    void fillpage(unsigned char c,size_t n,addr_t addr)
    {
      mem.fillpage(c,n,addr);
      _node->update_dmi_cache_entry(CacheWrite,addr,mem.last_page(addr));
    }

    void reset() { mem.reset(); };

    IssNode *_node;
  };

# endif

  // MemType:     :  DmiExternalMem or DmiInternalMem, depending upon whether you're using
  //                 an external memory (the usual case) or the internal memory (primarily
  //                 for testing purposes).
  // EntrySizeBits:  Size of each entry in address bits, e.g. 7 means 2^7 = 128 bytes.
  // Size         :  Number of entries in the cache.
  // TimeTagged :    True for time-tagged ISSs.  Latency will be added during a
  //                 memory operation which hits.
  template <class MemType,size_t EntrySizeBits, size_t Size, bool TimeTagged> 
  class DmiMemHandler {
  public:
    struct DmiItem {
      addr_t  _addr;
      addr_t  _mem;  // This stores a byte_t*, but we have to store a 64-bit
                     // value in case we're dealing with full 64-bit addresses.
    };

    IssNode    *_node;
    MemType     _mem;

    // For time-tagged ISSs, this latency is added on a DMI-cache hit.
    unsigned _read_latency, _write_latency;

    struct DmiCache {
      enum { 
        CacheSize = Size,
        PageSize  = (1 << EntrySizeBits),
        PageShift = (EntrySizeBits),
        IndexMask = (Size-1),
        CompMask  = (0xffffffffffffffffull << PageShift),
      };
      
      DmiItem _items[CacheSize];
      
      DmiCache() { reset(); }
      void reset() {
        addr_t bad = 0xffffffffffffffffull;
        for (int i = 0; i != CacheSize; ++i, bad -= PageSize) {
          _items[i]._addr = bad;
          _items[i]._mem = 0;
        }
      }
    };

    DmiMemHandler(IssNode *node) : 
      _node(node),
      _mem(node),
      _read_latency(0),
      _write_latency(0),
      _instr_read(new DmiCache),
      _data_read(new DmiCache),
      _data_write(new DmiCache)
    {};

    ~DmiMemHandler()
    {
      delete _instr_read;
      delete _data_read;
      delete _data_write;
      _instr_read = _data_read = _data_write = 0;
    }

    static unsigned page_size() { return DmiCache::PageSize; };

    uint64_t read64(CacheAccess ca,addr_t ra);
    uint32_t read32(CacheAccess ca,addr_t ra);
    uint16_t read16(CacheAccess ca,addr_t ra);
    uint8_t  read8(CacheAccess ca,addr_t ra);
    void     write64(addr_t ra, uint64_t v, unsigned nb);
    void     write32(addr_t ra, uint32_t v, unsigned nb);
    void     write16(addr_t ra, uint16_t v, unsigned nb);
    void     write8 (addr_t ra, uint8_t  v);
    void     readpage(CacheAccess ca,byte_t *t,size_t n,addr_t addr,addr_t critical_addr);
    void     writepage(byte_t *s,size_t n,addr_t addr,addr_t critical_addr);
    void     fillpage(unsigned char c,size_t n,addr_t addr);
    void     reset();

    void update_cache_entry(CacheAccess ca,addr_t addr,byte_t *mem);
    void invalidate_cache_entry(CacheAccess ca,addr_t addr);

    void setMemHandler(MemHandler *mem) { _mem.setMemHandler(mem); };

    // Set latencies for reads and writes for when there is a DMI hit.
    void setDmiLatencies(unsigned r,unsigned w) 
    { 
      _read_latency  = r; 
      _write_latency = w; 
    };


  private:

    addr_t comp_mask() const { return DmiCache::CompMask; };

    template <size_t NB> typename DmiSizeType<NB>::IntType read(DmiCache &cache,CacheAccess ca,addr_t ra)
    {
      addr_t dw_ra = ra & ~DWMask;
      unsigned off = ra & DWMask;
      unsigned table_index = ((dw_ra + NB - 1) >> DmiCache::PageShift) & DmiCache::IndexMask;
      byte_t *p = (byte_t*)((addr_t)dw_ra + (addr_t)cache._items[table_index]._mem);
      if (  (((ra ^ cache._items[table_index]._addr) & (DmiCache::CompMask | (NB-1))) == 0)) {
        if (TimeTagged && ca == CacheRead) _node->addLatency(_read_latency);
        // This switch should be folded out b/c the conditional is a template
        // parameter
        switch (NB) {
        case 8:
          return byte_read64(p,off);
        case 4:
          return byte_read32(p,off);
        case 2:
          return byte_read16(p,off);
        default:
          return byte_read8(p,off);
        }
      } else {
        // This switch should be folded out b/c the conditional is a template
        // parameter
        switch (NB) {
        case 8:
          return _mem.read64(ca,ra);
        case 4:
          return _mem.read32(ca,ra);
        case 2:
          return _mem.read16(ca,ra);
        default:
          return _mem.read8(ca,ra);
        }
      }      
    }

    template <size_t NB> void write(DmiCache &cache,addr_t addr,typename DmiSizeType<NB>::IntType v,unsigned nb)
    {
      addr_t dw_addr = addr & ~DWMask;
      unsigned off = addr & DWMask;
      unsigned table_index = ((dw_addr + NB - 1) >> DmiCache::PageShift) & DmiCache::IndexMask;
      byte_t *p = (byte_t*)((addr_t)dw_addr + (addr_t)cache._items[table_index]._mem);
      if ( (((dw_addr ^ cache._items[table_index]._addr) & (DmiCache::CompMask | (NB-1))) == 0)) {
        if (TimeTagged) _node->addLatency(_write_latency);
        // This switch should be folded out b/c the conditional is a template
        // parameter
        switch (NB) {
        case 8:
          byte_write64(p,off,v,nb);
          break;
        case 4:
          byte_write32(p,off,v,nb);
          break;
        case 2:
          byte_write16(p,off,v,nb);
          break;
        default:
          byte_write8(p,off,v);
          break;
        }
      } else {
        // This switch should be folded out b/c the conditional is a template
        // parameter
        switch (NB) {
        case 8:
          _mem.write64(addr,v,nb);
          break;
        case 4:
          _mem.write32(addr,v,nb);
          break;
        case 2:
          _mem.write16(addr,v,nb);
          break;
        default:
          _mem.write8(addr,v);
          break;
        }
      }
    }

    // Some of these allocations are fairly arbitrary.  Shouldn't be a problem,
    // since we're basically assuming that there's a single memory; the multiple
    // caches are really just for minimizing collisions.
    DmiCache &get_cache(CacheAccess ca)
    {
      switch (ca) {
      case CacheIFetch:
      case CacheILogRead:
        return *_instr_read;
      case CacheRead:
      case CacheLogRead:
        return *_data_read;
      case CacheWrite:
      case CacheFlush:
      case CacheStore:
      case CacheTouch:
        return *_data_write;
      default:
        return *_data_read;
      }
    }

    DmiCache *_instr_read;
    DmiCache *_data_read;
    DmiCache *_data_write;    
  };

  template <class MemType, size_t EntrySizeBits, size_t Size, bool TimeTagged> 
  void DmiMemHandler<MemType,EntrySizeBits,Size,TimeTagged>::reset()
  {
    _instr_read->reset();
    _data_read->reset();
    _data_write->reset();
    _mem.reset();
  }

  template <class MemType, size_t EntrySizeBits, size_t Size, bool TimeTagged> 
  uint64_t DmiMemHandler<MemType,EntrySizeBits,Size,TimeTagged>::read64(CacheAccess ca,addr_t ra)
  {
    return read<8>(*_data_read,ca,ra);
  }

  template <class MemType, size_t EntrySizeBits, size_t Size, bool TimeTagged> 
  uint32_t DmiMemHandler<MemType,EntrySizeBits,Size,TimeTagged>::read32(CacheAccess ca,addr_t ra)
  {
    // This might be an instruction or a data read, so we must select the
    // relevant cache.
    DmiCache &cache = (ca == CacheIFetch || ca == CacheILogRead) ? *_instr_read : *_data_read;
    return read<4>(cache,ca,ra);
  }

  template <class MemType, size_t EntrySizeBits, size_t Size, bool TimeTagged> 
  uint16_t DmiMemHandler<MemType,EntrySizeBits,Size,TimeTagged>::read16(CacheAccess ca,addr_t ra)
  {
    return read<2>(*_data_read,ca,ra);
  }

  template <class MemType, size_t EntrySizeBits, size_t Size, bool TimeTagged> 
  uint8_t DmiMemHandler<MemType,EntrySizeBits,Size,TimeTagged>::read8(CacheAccess ca,addr_t ra)
  {
    return read<1>(*_data_read,ca,ra);
  }

  template <class MemType, size_t EntrySizeBits, size_t Size, bool TimeTagged> 
  void DmiMemHandler<MemType,EntrySizeBits,Size,TimeTagged>::write64(addr_t ra, uint64_t v, unsigned nb)
  {
    write<8>(*_data_write,ra,v,nb);
  }

  template <class MemType, size_t EntrySizeBits, size_t Size, bool TimeTagged> 
  void DmiMemHandler<MemType,EntrySizeBits,Size,TimeTagged>::write32(addr_t ra, uint32_t v, unsigned nb)
  {
    write<4>(*_data_write,ra,v,nb);
  }

  template <class MemType, size_t EntrySizeBits, size_t Size, bool TimeTagged> 
  void DmiMemHandler<MemType,EntrySizeBits,Size,TimeTagged>::write16(addr_t ra, uint16_t v, unsigned nb)
  {
    write<2>(*_data_write,ra,v,nb);
  }

  template <class MemType, size_t EntrySizeBits, size_t Size, bool TimeTagged> 
  void DmiMemHandler<MemType,EntrySizeBits,Size,TimeTagged>::write8(addr_t ra, uint8_t v)
  {
    write<1>(*_data_write,ra,v,0);
  }

  template <class MemType, size_t EntrySizeBits, size_t Size, bool TimeTagged> 
  void DmiMemHandler<MemType,EntrySizeBits,Size,TimeTagged>::readpage(CacheAccess ca,byte_t *t,size_t n,addr_t addr,addr_t critical_addr)
  {
    // This might be an instruction or a data read, so we must select the
    // relevant cache.
    DmiCache &cache = (ca == CacheIFetch || ca == CacheILogRead) ? *_instr_read : *_data_read;

    if (n < DmiCache::PageSize) {
      unsigned table_index = ((addr + n - 1) >> DmiCache::PageShift) & DmiCache::IndexMask;
      byte_t *p = (byte_t*)((addr_t)addr + (addr_t)cache._items[table_index]._mem);
      if (  (((addr ^ cache._items[table_index]._addr) & (DmiCache::CompMask | (n-1))) == 0)) {
        byte_pagecopy(t,0,p,0,n);
      } else {
        return _mem.readpage(ca,t,n,addr,critical_addr);
      }      
    } else {
      return _mem.readpage(ca,t,n,addr,critical_addr);
    }
  }

  template <class MemType, size_t EntrySizeBits, size_t Size, bool TimeTagged> 
  void DmiMemHandler<MemType,EntrySizeBits,Size,TimeTagged>::writepage(byte_t *s,size_t n,addr_t addr,addr_t critical_addr)
  {
    DmiCache &cache = *_data_write;

    if (n < DmiCache::PageSize) {
      unsigned table_index = ((addr + n - 1) >> DmiCache::PageShift) & DmiCache::IndexMask;
      byte_t *p = (byte_t*)((addr_t)addr + (addr_t)cache._items[table_index]._mem);
      if (  (((addr ^ cache._items[table_index]._addr) & (DmiCache::CompMask | (n-1))) == 0)) {
        byte_pagecopy(p,0,s,0,n);
      } else {
        return _mem.writepage(s,n,addr,critical_addr);
      }      
    } else {
      return _mem.writepage(s,n,addr,critical_addr);
    } 
  }

  template <class MemType, size_t EntrySizeBits, size_t Size, bool TimeTagged> 
  void DmiMemHandler<MemType,EntrySizeBits,Size,TimeTagged>::fillpage(unsigned char c,size_t n,addr_t addr)
  {
    DmiCache &cache = *_data_write;

    if (n < DmiCache::PageSize) {
      unsigned table_index = ((addr + n - 1) >> DmiCache::PageShift) & DmiCache::IndexMask;
      byte_t *p = (byte_t*)((addr_t)addr + (addr_t)cache._items[table_index]._mem);
      if (  (((addr ^ cache._items[table_index]._addr) & (DmiCache::CompMask | (n-1))) == 0)) {
        byte_pagefill(p,0,c,n);
      } else {
        _mem.fillpage(c,n,addr);
      }      
    } else {
      _mem.fillpage(c,n,addr);
    } 
  }

  template <class MemType, size_t EntrySizeBits, size_t Size, bool TimeTagged> 
  void DmiMemHandler<MemType,EntrySizeBits,Size,TimeTagged>::update_cache_entry(CacheAccess ca,addr_t addr,byte_t *mem) 
  {
    // The idea is that the mem pointer points to the exact byte addressed by
    // addr.  So, we align mem and addr based upon our own entry-size alignment,
    // which means backing up the mem pointer based upon the offset in addr,
    // then aligning addr.  Then we store these two pointers.
    mem = mem - (addr & ~comp_mask());
    DmiCache &cache = get_cache(ca);
    unsigned table_index = (addr >> DmiCache::PageShift) & DmiCache::IndexMask;
    addr_t page = addr & DmiCache::CompMask;
    cache._items[table_index]._addr = page;
    cache._items[table_index]._mem  = ((addr_t)mem - page);
  }

  template <class MemType, size_t EntrySizeBits, size_t Size, bool TimeTagged> 
  void DmiMemHandler<MemType,EntrySizeBits,Size,TimeTagged>::invalidate_cache_entry(CacheAccess ca,addr_t addr) 
  {
    DmiCache &cache = get_cache(ca);
    unsigned table_index = (addr >> DmiCache::PageShift) & DmiCache::IndexMask;
    cache._items[table_index]._addr =  0xffffffffffffffffull - (table_index  * DmiCache::PageSize);    
  }

};

#endif


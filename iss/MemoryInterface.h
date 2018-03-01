//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This header file contains routines for accessing the memory
// object.  It takes care of unaligned access, endian issues, etc.
//
// A Memory object should also have been instantiated and be visible to
// these routines.  This is the actual sparse-memory object which stores
// the data.
//

#ifndef _MEMORY_INTERFACE_H_
#define _MEMORY_INTERFACE_H_



#ifdef USE_INTBV
#define mem_bits intbv
#else
#define mem_bits bits
#endif



namespace adl {

  //
  // These do the actual work of accessing memory.  There's a generic version that
  // handles requests of arbitrary size and then a number of partial
  // specializations that handle the common cases.  It would be nice if we could
  // put these inside of the MemoryModel class, but sadly C++ doesn't allow us to
  // put partial specializations inside of anything other than a namespace scope.
  //

  template <class Translate,class CacheAccess,class Logger,size_t NB,bool Interleaved,int SplitSize = 4> 
  struct read_internal {
    mem_bits<NB*8> operator()(Translate translate,CacheAccess cache,Logger logger,addr_t addr) const 
    { 
      MemoryAccess<Translate,CacheAccess,Logger,Interleaved,SplitSize> ma;

      int nw = NB / 4;
      mem_bits<NB*8> r;
      for (int i = 0; i != nw; ++i) {
        r.set(i*32,i*32+31,ma.read32(translate,cache,logger,addr+4*i));
      }
      switch (NB%4) {
      case 3:
        r.set(nw*32+16,nw*32+23,ma.read8(translate,cache,logger,addr+4*nw+2));
      case 2:
        r.set(nw*32,nw*32+15,ma.read16(translate,cache,logger,addr+4*nw));
        break;
      case 1:
        r.set(nw*32,nw*32+7,ma.read8(translate,cache,logger,addr+4*nw));
      case 0:
        ;
      }
      return r;
    };
  };

  template <class Translate,class CacheAccess,class Logger,bool Interleaved,int SplitSize> 
  struct read_internal<Translate,CacheAccess,Logger,8,Interleaved,SplitSize> {
    mem_bits<64> operator()(Translate translate,CacheAccess cache,Logger logger,addr_t addr) const
    {
      MemoryAccess<Translate,CacheAccess,Logger,Interleaved,SplitSize> ma;
      return mem_bits<64>(ma.read64(translate,cache,logger,addr));
    }
  };

  template <class Translate,class CacheAccess,class Logger,bool Interleaved,int SplitSize> 
  struct read_internal<Translate,CacheAccess,Logger,4,Interleaved,SplitSize> {
    mem_bits<32> operator()(Translate translate,CacheAccess cache,Logger logger,addr_t addr) const
    {
      MemoryAccess<Translate,CacheAccess,Logger,Interleaved,SplitSize> ma;
      return mem_bits<32>(ma.read32(translate,cache,logger,addr));
    }
  };

  template <class Translate,class CacheAccess,class Logger,bool Interleaved,int SplitSize> 
  struct read_internal<Translate,CacheAccess,Logger,3,Interleaved,SplitSize> {
    mem_bits<24> operator()(Translate translate,CacheAccess cache,Logger logger,addr_t addr) const
    {
      MemoryAccess<Translate,CacheAccess,Logger,Interleaved,SplitSize> ma;

      mem_bits<24> r;
      r.set(0,15,ma.read16(translate,cache,logger,addr));
      r.set(16,23,ma.read8(translate,cache,logger,addr+2));
      return r;
    }
  };

  template <class Translate,class CacheAccess,class Logger,bool Interleaved,int SplitSize> 
  struct read_internal<Translate,CacheAccess,Logger,2,Interleaved,SplitSize> {
    mem_bits<16> operator()(Translate translate,CacheAccess cache,Logger logger,addr_t addr) const
    {
      MemoryAccess<Translate,CacheAccess,Logger,Interleaved,SplitSize> ma;
      return mem_bits<16>(ma.read16(translate,cache,logger,addr));
    }
  };

  template <class Translate,class CacheAccess,class Logger,bool Interleaved,int SplitSize> 
  struct read_internal<Translate,CacheAccess,Logger,1,Interleaved,SplitSize> {
    mem_bits<8> operator()(Translate translate,CacheAccess cache,Logger logger,addr_t addr) const
    {
      MemoryAccess<Translate,CacheAccess,Logger,Interleaved,SplitSize> ma;
      return mem_bits<8>(ma.read8(translate,cache,logger,addr));
    }
  };

  template <class Translate,class CacheAccess,class Logger,bool Interleaved,int SplitSize> 
  struct read_internal<Translate,CacheAccess,Logger,0,Interleaved,SplitSize> {
    mem_bits<0> operator()(Translate translate,CacheAccess cache,Logger logger,addr_t addr) const
    {
      MemoryAccess<Translate,CacheAccess,Logger,Interleaved,SplitSize> ma;
      return mem_bits<0>(ma.read0(translate,cache,logger,addr));      
    }
  };

  template <class Translate,class CacheAccess,class Logger,size_t NB,size_t Nb,bool Interleaved,int SplitSize = 4> 
  struct write_internal {
    void operator()(Translate translate,CacheAccess cache,Logger logger,addr_t ea,addr_t ra,mem_bits<Nb> value) 
    { 
      MemoryAccess<Translate,CacheAccess,Logger,Interleaved,SplitSize> ma;    

      int nw = NB / 4;
      mem_bits<NB*8> t(value);
      for (int i = 0; i != nw; ++i) {
        if (i) {
          // Don't translate on the first address, since that's already been
          // done by the caller.  We don't want to translate after the write,
          // since we don't want a trailing translation which might cause some
          // side effects.
          ra = translate(ea,0);
        }
        ma.write32(translate,cache,logger,ea,ra,value.get(i*32,i*32+31).uint32());
        ea = translate.mask_ea( ea + 4 );
      }
      switch (NB%4) {
      case 3: {
        addr_t ea2 = translate.mask_ea( ea+2 );
        addr_t ra2 = translate(ea2,0);
        ma.write8(translate,cache,logger,ea2,ra2,value.get(nw*32+16,nw*32+23).uint32());
      }
      case 2:
        ea = translate.mask_ea( ea );
        ra = translate(ea,0);        
        ma.write16(translate,cache,logger,ea,ra,value.get(nw*32,nw*32+15).uint32());
        break;
      case 1:
        ea = translate.mask_ea( ea );
        ra = translate(ea,0);        
        ma.write8(translate,cache,logger,ea,ra,value.get(nw*32,nw*32+7).uint32());
      case 0:
        // Just to remove warnings.
        break;
      }
    };
  };

  template <class Translate,class CacheAccess,class Logger,size_t Nb,bool Interleaved,int SplitSize> 
  struct write_internal<Translate,CacheAccess,Logger,8,Nb,Interleaved,SplitSize> {
    void operator()(Translate translate,CacheAccess cache,Logger logger,addr_t ea,addr_t ra,mem_bits<Nb> value) const
    {
      MemoryAccess<Translate,CacheAccess,Logger,Interleaved,SplitSize> ma;    
      ma.write64(translate,cache,logger,ea,ra,value.uint64());
    }
  };

  template <class Translate,class CacheAccess,class Logger,size_t Nb,bool Interleaved,int SplitSize> 
  struct write_internal<Translate,CacheAccess,Logger,4,Nb,Interleaved,SplitSize> {
    void operator()(Translate translate,CacheAccess cache,Logger logger,addr_t ea,addr_t ra,mem_bits<Nb> value) const
    {
      MemoryAccess<Translate,CacheAccess,Logger,Interleaved,SplitSize> ma;    

      ma.write32(translate,cache,logger,ea,ra,value.uint32());
    }
  };

  template <class Translate,class CacheAccess,class Logger,size_t Nb,bool Interleaved,int SplitSize> 
  struct write_internal<Translate,CacheAccess,Logger,3,Nb,Interleaved,SplitSize> {
    void operator()(Translate translate,CacheAccess cache,Logger logger,addr_t ea,addr_t ra,mem_bits<Nb> value) const
    {
      MemoryAccess<Translate,CacheAccess,Logger,Interleaved,SplitSize> ma;

      ma.write16(translate,cache,logger,ea,ra,value.get(0,15).uint32());

      addr_t ea2 = translate.mask_ea(ea + 2);
      addr_t ra2 = translate(ea2,0);

      ma.write8(translate,cache,logger,ea2,ra2,value.get(16,23).uint32());
    }
  };

  template <class Translate,class CacheAccess,class Logger,size_t Nb,bool Interleaved,int SplitSize> 
  struct write_internal<Translate,CacheAccess,Logger,2,Nb,Interleaved,SplitSize> {
    void operator()(Translate translate,CacheAccess cache,Logger logger,addr_t ea,addr_t ra,mem_bits<Nb> value) const
    {
      MemoryAccess<Translate,CacheAccess,Logger,Interleaved,SplitSize> ma;    

      ma.write16(translate,cache,logger,ea,ra,value.uint32());
    }
  };

  template <class Translate,class CacheAccess,class Logger,size_t Nb,bool Interleaved,int SplitSize> 
  struct write_internal<Translate,CacheAccess,Logger,1,Nb,Interleaved,SplitSize> {
    void operator()(Translate translate,CacheAccess cache,Logger logger,addr_t ea,addr_t ra,mem_bits<Nb> value) const
    {
      MemoryAccess<Translate,CacheAccess,Logger,Interleaved,SplitSize> ma;    

      ma.write8(translate,cache,logger,ea,ra,value.uint32());
    }
  };

  template <class Translate,class CacheAccess,class Logger,size_t Nb,bool Interleaved,int SplitSize> 
  struct write_internal<Translate,CacheAccess,Logger,0,Nb,Interleaved,SplitSize> {
    void operator()(Translate translate,CacheAccess cache,Logger logger,addr_t ea,addr_t ra,mem_bits<Nb> value) const
    {
      MemoryAccess<Translate,CacheAccess,Logger,Interleaved,SplitSize> ma;    

      ma.write0(translate,cache,logger,ea,ra,value.uint32());
    }
  };


  // Some functions for reading and writing a data-dependent size.
  template <typename Translate,typename CacheAccess,typename Logger,bool ByteSwap> 
  void mem_write_sized(Translate t,CacheAccess c,Logger l,addr_t ea,uint64_t value,int size)
  {
    MemoryAccess <Translate,CacheAccess,Logger,true> ma;
    ea = t.mask_ea(ea);
    addr_t ra = t(ea,0);
    switch (size) {
    case 64:
      if (ByteSwap) {
        value = swab64(value);
      }
      ma.write64(t,c,l,ea,ra,value);
      break;
    case 32: {
      if (ByteSwap) {
        uint32_t d = value;
        value = swab32(d);
      }
      ma.write32(t,c,l,ea,ra,value);
    } break;
    case 24: {
      if (ByteSwap) {
        uint32_t d = (value << 8);
        value = swab32(d);
      }
      ma.write16(t,c,l,ea,ra,value >> 8);
      ma.write8(t,c,l,ea+2,ra+2,value & 0xff);
    } break;
    case 16:
      if (ByteSwap) {
        uint16_t d = value;
        value = swab16(d);
      }
      ma.write16(t,c,l,ea,ra,value);
      break;
    case 8:
      ma.write8(t,c,l,ea,ra,value);
      break;
    default:
      RError("Unsupported memory-write size:  " << std::dec << size);
    }
  }

  template <typename Translate,typename CacheAccess,typename Logger,bool ByteSwap> 
  uint64_t mem_read_sized(Translate t,CacheAccess c,Logger l,addr_t ea,int size)
  {
    MemoryAccess <Translate,CacheAccess,Logger,true> ma;
    switch (size) {
    case 64: {
      auto d = ma.read64(t,c,l,ea);
      return (ByteSwap) ? swab64(d) : d;
    }
    case 32: {
      auto d = ma.read32(t,c,l,ea);
      return (ByteSwap) ? swab32(d) : d;      
    }
    case 24: {
      uint32_t tmp = ma.read16(t,c,l,ea);
      tmp = (tmp << 8) | ma.read8(t,c,l,ea+2);
      if (ByteSwap) {
        tmp <<= 8;
        return swab32(tmp);
      } else {
        return tmp;
      }
    } break;
    case 16: {
      auto d = ma.read16(t,c,l,ea);
      return (ByteSwap) ? swab16(d) : d;
    }
    case 8:
      return ma.read8(t,c,l,ea);
    default:
      RError("Unsupported memory-write size:  " << size);
    }
  }

}

#endif

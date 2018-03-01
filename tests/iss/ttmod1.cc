//
// ADL Version 3.17.2
//
// ISS model generated for P.
//

#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <sstream>
#include <fstream>
#include <cmath>

namespace adl {
  static const bool Tracing = 1;
}

#include "helpers/sintbv.h"
#define bits adl::intbv
#define sbits adl::sintbv

#define ns_bits intbv
#define ns_sbits sintbv

#include "iss/ModelSupport.h"

#include "iss/SysCallMgr.h"

#include "iss/SysCallExtHandler.h"

#include "iss/DecodeCacheSupport.h"

# define CondTrace(x)          { if (adl_unlikely(_trace_mode)) x; }
# define CondRegTrace(x)       { if (adl_unlikely(_trace_mode & (ttRegWrite | ttRegRead))) x; }
# define CondRegBranchTrace(x) { if (adl_unlikely(_trace_mode & (ttRegWrite | ttRegRead | ttBranchTaken))) x; }
# define ExtCondTrace(x)       { if (adl_unlikely(core->trace_mode())) x; }
# define Dummify(x) x

using namespace adl;


#include "helpers/stringhash.h"
#include "helpers/LogUsage.h"

//
// Helper decls.
//

 enum  {
      RegWidth = 32
      ,
} ;
// The global memory object.


// Use the mem_read and mem_write routines to access this object from 
// other translation units, since those functions
// are not templated.

// Include the default implementation

#include "iss/DefaultMem.h"

namespace adliss {
  
  #ifdef DC_DEBUG
  # define DCDBG(x) x
  #else
  # define DCDBG(x)
  #endif
  
  const char *getCoreName();
  const char *getModelFileName();
  const adl::AttrData &getAttrData();
  const char *getModelPreamble();
  void resetGlobal();
  void setMemorySize(unsigned size);
  
  
  struct P : public IssCore, public TimeTaggedBase {
    
    
    enum Exception {
      None,
    };
    
    enum ExceptionIndex {
      __None_Index,
    };
    
    void setException(Exception e)
    {
    }
    
    unsigned getExceptionFlag()
    {
      return 0;
    }
    
    unsigned getExceptionFlag() const
    {
      return 0;
    }
    
    void clearExceptionFlag()
    {
    }
    
    //
    // Helper code.
    //
    
     void setCrField ( bits < 3 > field , bits < 32 > x , bits < 32 > y ) ATTRIBUTE_USED {
           bits < 4 > r = ( ( x . signedLT ( y ) ) ? 0x8 : 0 ) | ( ( x . signedGT ( y ) ) ? 0x4 : 0 ) | ( ( x == y ) ? 0x2 : 0 ) | XER_SO_read() ;
          CR_slice_set(  to_uint32( 4 * field ),to_uint32( 4 * field + 3 ) ,( r )  ) ;
    }
     void setXerField ( bool ov , bool so , bool ca , const bits < 32 > & carry ) ATTRIBUTE_USED {
          if ( ov ) {
                XER_OV_write(  carry ( 0 ) ^ carry ( 1 )  ) ;
            }
          if ( so ) {
                XER_SO_write(  XER_SO_read() | XER_OV_read()  ) ;
            }
          if ( ca ) {
                XER_CA_write(  carry ( 0 )  ) ;
            }
    }
    
    //
    // Auxiliary functions.
    //
    
    //
    //Stubs for the core services.
    //
    void setMmuLookup(const std::string &lookup,unsigned set,unsigned way,const FieldData &ld) { throw std::runtime_error("No MMU exists for this design."); };
    void getMmuLookup(const std::string &lookup,unsigned set,unsigned way,FieldData &ld) const { throw std::runtime_error("No MMU exists for this design."); };
    void showMmu(ReportBase &) const {};
    
    void clear_mmu_cache() {};
    void checkInstrPerms() {};
    void checkDataPerms() {};
    void checkLoadPerms() {};
    void checkStorePerms() {};
    
    //
    // Stubs for the translation functions.
    //
    addr_t instrReadTranslateInternal(addr_t a,int seq) {  return a; };
    addr_t dataReadTranslateInternal (addr_t a,int seq) {  return a; };
    addr_t dataWriteTranslateInternal(addr_t a,int seq) {  return a; };
    
    bool instrReadTranslateInternalNs(addr_t &ra,addr_t a,bool log = false) const { ra = a; return true; };
    bool dataReadTranslateInternalNs (addr_t &ra,addr_t a,bool log = false) const { ra = a; return true; };
    bool dataWriteTranslateInternalNs(addr_t &ra,addr_t a,bool log = false) const { ra = a; return true; };
    
    void aligned_write_handler(addr_t,addr_t) { };
    void misaligned_write_handler(addr_t,addr_t) { };
    void misaligned_read_handler(addr_t,addr_t) { };
    void pre_read_handler(addr_t,addr_t) { };
    void post_read_handler(addr_t,addr_t) { };
    void pre_write_handler(addr_t,addr_t) { };
    void post_write_handler(addr_t,addr_t) { };
    
    // Commit any pending updates.
    void empty_commit_queue() {
    }
    
    void enable_rollback(bool e) { }
    
    void commit_rollback() { }
    
    void flush_rollback() { }
    
    RestoreStack get_rollback_data() { return RestoreStack(); }
    
    void set_ignore_mode(bool) { }
    
    bool ignore_enabled() { return false; }
    
    
    //
    // Memory access routines.
    //
    
    //
    // Utility functions for translating memory and manipulating addresses.
    //
    addr_t lastRealAddr() const { return _lastRealAddr; };
    
    addr_t dataReadTranslate(addr_t ea,int seq)  ATTRIBUTE_USED {
      _lastEffectiveAddr = ea;
      return _lastRealAddr = (dataReadTranslateInternal(ea,seq) & 0xffffffffULL);
    }
    
    template <size_t Nb>
    addr_t dataReadTranslate(const bits<Nb> &ea) { return dataReadTranslate(ea.uint64(),0); }
    
    addr_t dataWriteTranslate(addr_t ea,int seq)  ATTRIBUTE_USED {
      _lastEffectiveAddr = ea;
      return _lastRealAddr = (dataWriteTranslateInternal(ea,seq) & 0xffffffffULL);
    }
    
    template <size_t Nb>
    addr_t dataWriteTranslate(const bits<Nb> &ea) { return dataWriteTranslate(ea.uint64(),0); }
    
    addr_t instrReadTranslate(addr_t ea,int seq)  ATTRIBUTE_USED {
      return _lastRealAddr = (instrReadTranslateInternal(ea,seq) & 0xffffffffULL);
    }
    
    template <size_t Nb>
    addr_t instrReadTranslate(const bits<Nb> &ea) { return instrReadTranslate(ea.uint64(),0); }
    
    addr_t dataReadTranslate(addr_t ea)  ATTRIBUTE_USED {
      return dataReadTranslate(ea,0);
    }
    
    addr_t dataWriteTranslate(addr_t ea)  ATTRIBUTE_USED {
      return dataWriteTranslate(ea,0);
    }
    
    addr_t instrReadTranslate(addr_t ea)  ATTRIBUTE_USED {
      return instrReadTranslate(ea,0);
    }
    
    virtual bool extDataReadTranslate(addr_t &ra,addr_t ea,bool log = false) const {
      if (dataReadTranslateInternalNs(ra,ea,log)) {
        ra = ra& 0xffffffffULL;
        return true;
      } else {
        return false;
      }
    }
    virtual bool extDataWriteTranslate(addr_t &ra,addr_t ea,bool log = false) const {
      if (dataWriteTranslateInternalNs(ra,ea,log)) {
        ra = ra& 0xffffffffULL;
        return true;
      } else {
        return false;
      }
    }
    virtual bool extInstrReadTranslate(addr_t &ra,addr_t ea,bool log = false) const {
      if (instrReadTranslateInternalNs(ra,ea,log)) {
        ra = ra& 0xffffffffULL;
        return true;
      } else {
        return false;
      }
    }
    
    addr_t mask_ea(addr_t ea) { return ea & 0xffffffffULL; };
    addr_t mask_ra(addr_t ra) { return ra & 0xffffffffULL; };
    //
    // Functors for performing translations.
    //
    struct InstrReadTranslate {
      InstrReadTranslate(P &c) : _core(c) {};
      addr_t operator()(addr_t addr,int seq) {
        return _core.instrReadTranslate(addr,seq);
      }
      addr_t mask_ea(addr_t ea) {
        return ea & 0xffffffffULL;
      }
      addr_t scale_ea(addr_t ea) { return ea; }
      void aligned_write(addr_t ea,addr_t ra) { }
      void misaligned_write(addr_t ea,addr_t ra) { }
      void misaligned_read(addr_t ea,addr_t ra) { }
      void pre_read(addr_t ea,addr_t ra) { }
      void post_read(addr_t ea,addr_t ra) { }
      void pre_write(addr_t ea,addr_t ra) { }
      void post_write(addr_t ea,addr_t ra) { }
      P &_core;
    };
    
    struct DataReadTranslate {
      DataReadTranslate(P &c) : _core(c) {};
      addr_t operator()(addr_t addr,int seq) {
        return _core.dataReadTranslate(addr,seq);
      }
      addr_t mask_ea(addr_t ea) {
        return ea & 0xffffffffULL;
      }
      addr_t scale_ea(addr_t ea) { return ea; }
      void aligned_write(addr_t ea,addr_t ra) { }
      void misaligned_write(addr_t ea,addr_t ra) { }
      void misaligned_read(addr_t ea,addr_t ra) {
        _core.misaligned_read_handler(ea,ra);
      }
      void pre_read(addr_t ea,addr_t ra) {
        _core.pre_read_handler(ea,ra);
      }
      void post_read(addr_t ea,addr_t ra) {
        _core.post_read_handler(ea,ra);
      }
      void pre_write(addr_t ea,addr_t ra) { }
      void post_write(addr_t ea,addr_t ra) { }
      P &_core;
    };
    
    struct DataWriteTranslate {
      DataWriteTranslate(P &c) : _core(c) {};
      addr_t operator()(addr_t addr,int seq) {
        return _core.dataWriteTranslate(addr,seq);
      }
      addr_t mask_ea(addr_t ea) {
        return ea & 0xffffffffULL;
      }
      addr_t scale_ea(addr_t ea) { return ea; }
      void aligned_write(addr_t ea,addr_t ra) {
        _core.aligned_write_handler(ea,ra);
      }
      void misaligned_write(addr_t ea,addr_t ra) {
        _core.misaligned_write_handler(ea,ra);
      }
      void misaligned_read(addr_t ea,addr_t ra) { }
      void pre_read(addr_t ea,addr_t ra) { }
      void post_read(addr_t ea,addr_t ra) { }
      void pre_write(addr_t ea,addr_t ra) {
        _core.pre_write_handler(ea,ra);
      }
      void post_write(addr_t ea,addr_t ra) {
        _core.post_write_handler(ea,ra);
      }
      P &_core;
    };
    
    //
    // No-side-effect versions of functors for performing translations.
    //
    struct InstrReadTranslateNs {
      InstrReadTranslateNs(const P &c) : _core(c) {};
      addr_t operator()(addr_t addr,int seq) {
        addr_t ra;
        if (_core.extInstrReadTranslate(ra,addr,false)) {
          return ra;
        } else {
          RError("Could not translate 0x" << addr);
        }
      }
      addr_t mask_ea(addr_t ea) {
        return ea & 0xffffffffULL;
      }
      addr_t scale_ea(addr_t ea) { return ea; }
      void aligned_write(addr_t ea,addr_t ra) { }
      void misaligned_write(addr_t ea,addr_t ra) { }
      void misaligned_read(addr_t ea,addr_t ra) { }
      void pre_read(addr_t ea,addr_t ra) { }
      void post_read(addr_t ea,addr_t ra) { }
      void pre_write(addr_t ea,addr_t ra) { }
      void post_write(addr_t ea,addr_t ra) { }
      const P &_core;
    };
    
    struct DataReadTranslateNs {
      DataReadTranslateNs(const P &c) : _core(c) {};
      addr_t operator()(addr_t addr,int seq) {
        addr_t ra;
        if (_core.extDataReadTranslate(ra,addr,false)) {
          return ra;
        } else {
          RError("Could not translate 0x" << addr);
        }
      }
      addr_t mask_ea(addr_t ea) {
        return ea & 0xffffffffULL;
      }
      addr_t scale_ea(addr_t ea) { return ea; }
      void aligned_write(addr_t ea,addr_t ra) { }
      void misaligned_write(addr_t ea,addr_t ra) { }
      void misaligned_read(addr_t ea,addr_t ra) { }
      void pre_read(addr_t ea,addr_t ra) { }
      void post_read(addr_t ea,addr_t ra) { }
      void pre_write(addr_t ea,addr_t ra) { }
      void post_write(addr_t ea,addr_t ra) { }
      const P &_core;
    };
    
    struct DataWriteTranslateNs {
      DataWriteTranslateNs(const P &c) : _core(c) {};
      addr_t operator()(addr_t addr,int seq) {
        addr_t ra;
        if (_core.extDataWriteTranslate(ra,addr,false)) {
          return ra;
        } else {
          RError("Could not translate 0x" << addr);
        }
      }
      addr_t mask_ea(addr_t ea) {
        return ea & 0xffffffffULL;
      }
      addr_t scale_ea(addr_t ea) { return ea; }
      void aligned_write(addr_t ea,addr_t ra) { }
      void misaligned_write(addr_t ea,addr_t ra) { }
      void misaligned_read(addr_t ea,addr_t ra) { }
      void pre_read(addr_t ea,addr_t ra) { }
      void post_read(addr_t ea,addr_t ra) { }
      void pre_write(addr_t ea,addr_t ra) { }
      void post_write(addr_t ea,addr_t ra) { }
      const P &_core;
    };
    
    virtual uint32_t mem_debug_data_read(bool trans,addr_t addr,int size) const
    {
      NullReadLog logger;
      SimpleCache<GlobalMemoryType&> cache(mem);
      if (trans && 0) {
        DataReadTranslateNs translate(*this);
        return mem_read_sized<DataReadTranslateNs,SimpleCache<GlobalMemoryType&>,NullReadLog,0>(translate,cache,logger,addr,size);
      } else {
        NullTranslate translate;
        return mem_read_sized<NullTranslate,SimpleCache<GlobalMemoryType&>,NullReadLog,0>(translate,cache,logger,addr,size);
      }
    };
    virtual void mem_debug_data_write(bool trans,addr_t addr,uint32_t data,int size)
    {
      NullWriteLog logger;
      SimpleCache<GlobalMemoryType&> cache(mem);
      if (trans && 0) {
        DataWriteTranslateNs translate(*this);
        mem_write_sized<DataWriteTranslateNs,SimpleCache<GlobalMemoryType&>,NullWriteLog,0>(translate,cache,logger,addr,data,size);
      } else {
        NullTranslate translate;
        mem_write_sized<NullTranslate,SimpleCache<GlobalMemoryType&>,NullWriteLog,0>(translate,cache,logger,addr,data,size);
      }
    };
    
    
    struct LogmemMemoryRead {
      LogmemMemoryRead(P &c) : _core(c) {};
      void operator()(bool pre,int seq,addr_t ea,addr_t ra,int num_bytes,uint64_t value) {
        if (Tracing && (_core.trace_mode() & ttMemRead)) {
          unsigned tf = _core.trace_mode();
          _core.get_logger()->log_mem_read(0,"Mem",pre,seq,ea & ~WordMask,ra & ~WordMask,mem.read32( ra & ~WordMask));
          _core.set_trace_mode(tf);
        }
        _core.check_read_watchpoints(ea,pre,num_bytes);
      };
      P &_core;
    };
    
    struct LogmemMemoryWrite {
      LogmemMemoryWrite(P &c) : _core(c) {};
      void operator()(bool pre,int seq,addr_t ea,addr_t ra,int num_bytes,uint64_t value) {
        if (Tracing && (_core.trace_mode() & ttMemWrite)) {
          unsigned tf = _core.trace_mode();
          _core.get_logger()->log_mem_write(0,"Mem", pre, seq, ea & ~WordMask,ra & ~WordMask,mem.read32( ra & ~WordMask));
          _core.set_trace_mode(tf);
        }
        _core.check_write_watchpoints(ea,pre,num_bytes,value);
      };
      P &_core;
    };
    
    
    struct LogmemWpRead {
      LogmemWpRead(P &c) : _core(c) {};
      void operator()(bool pre,int seq,addr_t ea,addr_t ra,int num_bytes,uint64_t value) {
        _core.check_read_watchpoints(ea,pre,num_bytes);
      };
      P &_core;
    };
    
    struct LogmemWpWrite {
      LogmemWpWrite(P &c) : _core(c) {};
      void operator()(bool pre,int seq,addr_t ea,addr_t ra,int num_bytes,uint64_t value) {
        _core.check_write_watchpoints(ea,pre,num_bytes,value);
      };
      P &_core;
    };
    
    struct LogmemInstrRead {
      LogmemInstrRead(P &c) : _core(c) {};
      void operator()(bool pre,int seq,addr_t ea,addr_t ra,int num_bytes,uint64_t value) {
        if (Tracing && (_core.trace_mode() & ttInstrRead) && !pre) {
          unsigned tf = _core.trace_mode();
          _core.get_logger()->log_instr_read(0,"Mem", ea & ~WordMask,ra & ~WordMask,mem.read32( ra & ~WordMask));
          _core.set_trace_mode(tf);
        }
      };
      P &_core;
    };
    
    struct DmiItem {
      addr_t  _ea;
      byte_t *_mem;
    };
    
    struct DmiCache {
      enum { PageSize  = 128,
               PageShift = 7,
               IndexMask = 2047,
               CompMask  = 0xffffffffffffff80ull,
      };
      
      bool    _enable; // Note:  Only used for reset.
      DmiItem _items[2048];
      
      DmiCache() { reset(); }
      void enable() { _enable = true; };
      void disable() { _enable = false; };
      void reset() {
        addr_t bad = 0xffffffffffffffffull;
        for (int i = 0; i != 2048; ++i, bad -= PageSize) {
          _items[i]._ea = bad;
          _items[i]._mem = 0;
          _enable = false;
        }
      }
    };
    
    template <class Core,class Translate,class CacheAccess,class Logger,size_t NB,bool Interleaved>
    struct dmi_read_internal {
      mem_bits<NB*8> operator()(Core &core,Translate translate,CacheAccess cache,Logger logger,addr_t addr) const {
        read_internal<Translate,CacheAccess,Logger,NB,Interleaved> ri;
        return ri(translate,cache,logger,addr);
      }
    };
    
    template <class Core,class Translate,class CacheAccess,class Logger,bool Interleaved>
    struct dmi_read_internal<Core,Translate,CacheAccess,Logger,8,Interleaved> {
      mem_bits<64> operator()(Core &core,Translate translate,CacheAccess cache,Logger logger,addr_t ea) const  ATTRIBUTE_USED 
      {
        unsigned table_index = ((ea + 8 - 1) >> DmiCache::PageShift) & DmiCache::IndexMask;
        byte_t *p = (ea & ~((addr_t)DWMask)) + core._dmi_mem_read._items[table_index]._mem;
        if (  (((ea ^ core._dmi_mem_read._items[table_index]._ea) & (DmiCache::CompMask | 7)) == 0)) {
          core._latency += core._read_latency;
          return mem_bits<64>(byte_read64(p,(ea & DWMask)));
        } else {
          read_internal<Translate,CacheAccess,Logger,8,Interleaved> ri;
          mem_bits<64> x = ri(translate,cache,logger,ea);
          core.update_dmi_mem_read(core._lastEffectiveAddr,core.lastRealAddr(),cache.last_page(core.lastRealAddr() & DmiCache::CompMask));
          return x;
        }
      }
    };
    
    template <class Core,class Translate,class CacheAccess,class Logger,bool Interleaved>
    struct dmi_read_internal<Core,Translate,CacheAccess,Logger,4,Interleaved> {
      mem_bits<32> operator()(Core &core,Translate translate,CacheAccess cache,Logger logger,addr_t ea) const  ATTRIBUTE_USED 
      {
        unsigned table_index = ((ea + 4 - 1) >> DmiCache::PageShift) & DmiCache::IndexMask;
        byte_t *p = (ea & ~((addr_t)DWMask)) + core._dmi_mem_read._items[table_index]._mem;
        if (  (((ea ^ core._dmi_mem_read._items[table_index]._ea) & (DmiCache::CompMask | 3)) == 0)) {
          core._latency += core._read_latency;
          return mem_bits<32>(byte_read32(p,(ea & DWMask)));
        } else {
          read_internal<Translate,CacheAccess,Logger,4,Interleaved> ri;
          mem_bits<32> x = ri(translate,cache,logger,ea);
          core.update_dmi_mem_read(core._lastEffectiveAddr,core.lastRealAddr(),cache.last_page(core.lastRealAddr() & DmiCache::CompMask));
          return x;
        }
      }
    };
    
    template <class Core,class Translate,class CacheAccess,class Logger,bool Interleaved>
    struct dmi_read_internal<Core,Translate,CacheAccess,Logger,2,Interleaved> {
      mem_bits<16> operator()(Core &core,Translate translate,CacheAccess cache,Logger logger,addr_t ea) const  ATTRIBUTE_USED 
      {
        unsigned table_index = ((ea + 2 - 1) >> DmiCache::PageShift) & DmiCache::IndexMask;
        byte_t *p = (ea & ~((addr_t)DWMask)) + core._dmi_mem_read._items[table_index]._mem;
        if (  (((ea ^ core._dmi_mem_read._items[table_index]._ea) & (DmiCache::CompMask | 1)) == 0)) {
          core._latency += core._read_latency;
          return mem_bits<16>(byte_read16(p,(ea & DWMask)));
        } else {
          read_internal<Translate,CacheAccess,Logger,2,Interleaved> ri;
          mem_bits<16> x = ri(translate,cache,logger,ea);
          core.update_dmi_mem_read(core._lastEffectiveAddr,core.lastRealAddr(),cache.last_page(core.lastRealAddr() & DmiCache::CompMask));
          return x;
        }
      }
    };
    
    template <class Core,class Translate,class CacheAccess,class Logger,bool Interleaved>
    struct dmi_read_internal<Core,Translate,CacheAccess,Logger,1,Interleaved> {
      mem_bits<8> operator()(Core &core,Translate translate,CacheAccess cache,Logger logger,addr_t ea) const  ATTRIBUTE_USED 
      {
        unsigned table_index = ((ea + 1 - 1) >> DmiCache::PageShift) & DmiCache::IndexMask;
        byte_t *p = (ea & ~((addr_t)DWMask)) + core._dmi_mem_read._items[table_index]._mem;
        if (  (((ea ^ core._dmi_mem_read._items[table_index]._ea) & DmiCache::CompMask) == 0)) {
          core._latency += core._read_latency;
          return mem_bits<8>(byte_read8(p,(ea & DWMask)));
        } else {
          read_internal<Translate,CacheAccess,Logger,1,Interleaved> ri;
          mem_bits<8> x = ri(translate,cache,logger,ea);
          core.update_dmi_mem_read(core._lastEffectiveAddr,core.lastRealAddr(),cache.last_page(core.lastRealAddr() & DmiCache::CompMask));
          return x;
        }
      }
    };
    
    // Update an entry in the DMI cache.
    void update_dmi_mem_read(addr_t ea,addr_t ra,byte_t *p) {
      if (check_dmi_read_reserved(ea)) return;
      unsigned table_index = (ea >> DmiCache::PageShift) & DmiCache::IndexMask;
      addr_t ea_page = ea & DmiCache::CompMask;
      _dmi_mem_read._items[table_index]._ea = ea_page;
      _dmi_mem_read._items[table_index]._mem = (p - ea_page);
    }
    
    // Invalidate an entry, if it exists, in the DMI cache.
    void invalidate_dmi_mem_read(addr_t ea) {
      unsigned table_index = (ea >> DmiCache::PageShift) & DmiCache::IndexMask;
      _dmi_mem_read._items[table_index]._ea =  0xffffffffffffffffull - (table_index  * DmiCache::PageSize);
    }
    
    template <class Core,class Translate,class CacheAccess,class Logger,size_t NB,size_t Nb,bool Interleaved>
    struct dmi_write_internal {
      void operator()(Core &core,Translate translate,CacheAccess cache,Logger logger,addr_t ea,const mem_bits<Nb> &value)  ATTRIBUTE_USED {
        write_internal<Translate,CacheAccess,Logger,NB,Nb,Interleaved> wi;
        ea = translate.mask_ea(ea);
        addr_t ra = translate(ea,0);
        translate.aligned_write(ea,ra);
        wi(translate,cache,logger,ea,ra,value);
      }
    };
    
    template <class Core,class Translate,class CacheAccess,class Logger,size_t Nb,bool Interleaved>
    struct dmi_write_internal<Core,Translate,CacheAccess,Logger,8,Nb,Interleaved> {
      void operator()(Core &core,Translate translate,CacheAccess cache,Logger logger,addr_t ea,mem_bits<Nb> value) const ATTRIBUTE_USED 
      {
        unsigned table_index = ((ea + 8 - 1) >> DmiCache::PageShift) & DmiCache::IndexMask;
        byte_t *p = (ea & ~((addr_t)DWMask)) + core._dmi_mem_write._items[table_index]._mem;
        if ( (((ea ^ core._dmi_mem_write._items[table_index]._ea) & (DmiCache::CompMask | 7)) == 0)) {
          core._latency += core._write_latency;
          byte_write64(p,(ea & DWMask),value.uint64());
        } else {
          write_internal<Translate,CacheAccess,Logger,8,64,Interleaved> wi;
          ea = translate.mask_ea(ea);
          addr_t ra = translate(ea,0);
          translate.aligned_write(ea,ra);
          wi(translate,cache,logger,ea,ra,value);
          core.update_dmi_mem_write(core._lastEffectiveAddr,core.lastRealAddr(),cache.last_page(core.lastRealAddr() & DmiCache::CompMask));}
      }
    };
    
    template <class Core,class Translate,class CacheAccess,class Logger,size_t Nb,bool Interleaved>
    struct dmi_write_internal<Core,Translate,CacheAccess,Logger,4,Nb,Interleaved> {
      void operator()(Core &core,Translate translate,CacheAccess cache,Logger logger,addr_t ea,mem_bits<Nb> value) const ATTRIBUTE_USED 
      {
        unsigned table_index = ((ea + 4 - 1) >> DmiCache::PageShift) & DmiCache::IndexMask;
        byte_t *p = (ea & ~((addr_t)DWMask)) + core._dmi_mem_write._items[table_index]._mem;
        if ( (((ea ^ core._dmi_mem_write._items[table_index]._ea) & (DmiCache::CompMask | 3)) == 0)) {
          core._latency += core._write_latency;
          byte_write32(p,(ea & DWMask),value.uint32());
        } else {
          write_internal<Translate,CacheAccess,Logger,4,32,Interleaved> wi;
          ea = translate.mask_ea(ea);
          addr_t ra = translate(ea,0);
          translate.aligned_write(ea,ra);
          wi(translate,cache,logger,ea,ra,value);
          core.update_dmi_mem_write(core._lastEffectiveAddr,core.lastRealAddr(),cache.last_page(core.lastRealAddr() & DmiCache::CompMask));}
      }
    };
    
    template <class Core,class Translate,class CacheAccess,class Logger,size_t Nb,bool Interleaved>
    struct dmi_write_internal<Core,Translate,CacheAccess,Logger,2,Nb,Interleaved> {
      void operator()(Core &core,Translate translate,CacheAccess cache,Logger logger,addr_t ea,mem_bits<Nb> value) const ATTRIBUTE_USED 
      {
        unsigned table_index = ((ea + 2 - 1) >> DmiCache::PageShift) & DmiCache::IndexMask;
        byte_t *p = (ea & ~((addr_t)DWMask)) + core._dmi_mem_write._items[table_index]._mem;
        if ( (((ea ^ core._dmi_mem_write._items[table_index]._ea) & (DmiCache::CompMask | 1)) == 0)) {
          core._latency += core._write_latency;
          byte_write16(p,(ea & DWMask),value.uint32());
        } else {
          write_internal<Translate,CacheAccess,Logger,2,16,Interleaved> wi;
          ea = translate.mask_ea(ea);
          addr_t ra = translate(ea,0);
          translate.aligned_write(ea,ra);
          wi(translate,cache,logger,ea,ra,value);
          core.update_dmi_mem_write(core._lastEffectiveAddr,core.lastRealAddr(),cache.last_page(core.lastRealAddr() & DmiCache::CompMask));}
      }
    };
    
    template <class Core,class Translate,class CacheAccess,class Logger,size_t Nb,bool Interleaved>
    struct dmi_write_internal<Core,Translate,CacheAccess,Logger,1,Nb,Interleaved> {
      void operator()(Core &core,Translate translate,CacheAccess cache,Logger logger,addr_t ea,mem_bits<Nb> value) const ATTRIBUTE_USED 
      {
        unsigned table_index = ((ea + 1 - 1) >> DmiCache::PageShift) & DmiCache::IndexMask;
        byte_t *p = (ea & ~((addr_t)DWMask)) + core._dmi_mem_write._items[table_index]._mem;
        if ( (((ea ^ core._dmi_mem_write._items[table_index]._ea) & DmiCache::CompMask) == 0)) {
          core._latency += core._write_latency;
          byte_write8(p,(ea & DWMask),value.uint32());
        } else {
          write_internal<Translate,CacheAccess,Logger,1,8,Interleaved> wi;
          ea = translate.mask_ea(ea);
          addr_t ra = translate(ea,0);
          translate.aligned_write(ea,ra);
          wi(translate,cache,logger,ea,ra,value);
          core.update_dmi_mem_write(core._lastEffectiveAddr,core.lastRealAddr(),cache.last_page(core.lastRealAddr() & DmiCache::CompMask));}
      }
    };
    
    // Update an entry in the DMI cache.
    void update_dmi_mem_write(addr_t ea,addr_t ra,byte_t *p) {
      if (check_dmi_write_reserved(ea)) return;
      unsigned table_index = (ea >> DmiCache::PageShift) & DmiCache::IndexMask;
      addr_t ea_page = ea & DmiCache::CompMask;
      _dmi_mem_write._items[table_index]._ea = ea_page;
      _dmi_mem_write._items[table_index]._mem = (p - ea_page);
    }
    
    // Invalidate an entry, if it exists, in the DMI cache.
    void invalidate_dmi_mem_write(addr_t ea) {
      unsigned table_index = (ea >> DmiCache::PageShift) & DmiCache::IndexMask;
      _dmi_mem_write._items[table_index]._ea =  0xffffffffffffffffull - (table_index  * DmiCache::PageSize);
    }
    
    void invalidate_dmi_mem(addr_t ea) {
      invalidate_dmi_mem_read(ea);
      invalidate_dmi_mem_write(ea);
    }
    
    template <size_t NB>
     bits<NB*8> mem_read_jit (addr_t ea)
    {
      DataReadTranslate translate(*this);
      SimpleCache<GlobalMemoryType&> cache(mem);
      LogmemWpRead logger(*this);
      if (Tracing && adl_unlikely(_trace_mode & ttCoreMemRead)) {
        _logger->log_core_mem_read(0,"Mem",ea,NB);
      }
      dmi_read_internal<P,DataReadTranslate,SimpleCache<GlobalMemoryType&>,LogmemWpRead,NB,1> ri;
      ea = translate.scale_ea(ea);
      return ri(*this,translate,cache,logger,ea);
    };
    
    template <size_t NB>
     void mem_write_jit (addr_t ea,bits<NB*8> value)
    {
      DataWriteTranslate translate(*this);
      SimpleCache<GlobalMemoryType&> cache(mem);
      LogmemWpWrite logger(*this);
      if (Tracing && adl_unlikely(_trace_mode & ttCoreMemWrite)) {
        _logger->log_core_mem_write(0,"Mem",ea,NB);
      }
      dmi_write_internal<P,DataWriteTranslate,SimpleCache<GlobalMemoryType&>,LogmemWpWrite,NB,NB*8,1> wi;
      wi(*this,translate,cache,logger,ea,value);
    };
    
    //
    // Versions for handling addresses specified as bits objects.
    //
    template <size_t NB,size_t Nb1,size_t Nb2>
    void mem_write_jit (const bits<Nb1> &addr,const bits<Nb2> &value) {
      mem_write_jit<NB>(addr.uint64(),value);
    };
    
    template <size_t NB,size_t Nb>
    bits<NB*8> mem_read_jit (bits<Nb> addr) {
      return mem_read_jit<NB>(addr.uint64());
    };
    
    template <size_t NB>
     bits<NB*8> mem_read (addr_t ea)
    {
      DataReadTranslate translate(*this);
      SimpleCache<GlobalMemoryType&> cache(mem);
      LogmemMemoryRead logger(*this);
      if (Tracing && adl_unlikely(_trace_mode & ttCoreMemRead)) {
        _logger->log_core_mem_read(0,"Mem",ea,NB);
      }
      read_internal<DataReadTranslate,SimpleCache<GlobalMemoryType&>,LogmemMemoryRead,NB,1> ri;
      ea = translate.scale_ea(ea);
      return ri(translate,cache,logger,ea);
    };
    
    template <size_t NB>
     void mem_write (addr_t ea,bits<NB*8> value)
    {
      DataWriteTranslate translate(*this);
      SimpleCache<GlobalMemoryType&> cache(mem);
      LogmemMemoryWrite logger(*this);
      if (Tracing && adl_unlikely(_trace_mode & ttCoreMemWrite)) {
        _logger->log_core_mem_write(0,"Mem",ea,NB);
      }
      write_internal<DataWriteTranslate,SimpleCache<GlobalMemoryType&>,LogmemMemoryWrite,NB,NB*8,1> wi;
      ea = translate.mask_ea(translate.scale_ea(ea));
      addr_t ra = translate(ea,0);
      translate.aligned_write(ea,ra);
      wi(translate,cache,logger,ea,ra,value);
    };
    
    //
    // Versions for handling addresses specified as bits objects.
    //
    template <size_t NB,size_t Nb1,size_t Nb2>
    void mem_write (const bits<Nb1> &addr,const bits<Nb2> &value) {
      mem_write<NB>(addr.uint64(),value);
    };
    
    template <size_t NB,size_t Nb>
    bits<NB*8> mem_read (bits<Nb> addr) {
      return mem_read<NB>(addr.uint64());
    };
    
    uint32_t instr_read32 (addr_t ea)
    {
      InstrReadTranslate translate(*this);
      LogmemInstrRead logger(*this);
      SimpleCache<GlobalMemoryType&> cache(mem);
      MemoryAccess<InstrReadTranslate,SimpleCache<GlobalMemoryType&>,LogmemInstrRead,1> ma;
      return ma.read32(translate,cache,logger,translate.scale_ea(ea));
    }
    
    struct JitInstrMemLog {
      JitInstrMemLog(P &core) : _core(core) {};
      void operator()(bool pre,int seq,addr_t ea,addr_t ra,int num_bytes,uint64_t value) { if (seq == 0) { _core._last_instr_ra = ra; } };
      P &_core;
    };
    
    uint32_t instr_read32_nocache (addr_t ea)
    {
      InstrReadTranslate translate(*this);
      JitInstrMemLog logger(*this);
      SimpleCache<GlobalMemoryType&> cache(mem);
      MemoryAccess<InstrReadTranslate,SimpleCache<GlobalMemoryType&>,JitInstrMemLog,1> ma;
      return ma.read32(translate,cache,logger,translate.scale_ea(ea));
    }
    
    uint32_t debug_instr_read_internal(addr_t addr,bool trans,int size) const
    {
      NullReadLog logger;
      if (trans) {
        InstrReadTranslateNs translate(*this);
        SimpleCache<GlobalMemoryType&> cache(mem);
        return mem_read_sized<InstrReadTranslateNs,SimpleCache<GlobalMemoryType&>,NullReadLog,0>(translate,cache,logger,addr,size);
      } else {
        NullTranslate translate;
        SimpleCache<GlobalMemoryType&> cache(mem);
        return mem_read_sized<NullTranslate,SimpleCache<GlobalMemoryType&>,NullReadLog,0>(translate,cache,logger,addr,size);
      }
    }
    
    bool debug_instr_read(uint32_t &result,bool trans,addr_t addr,int size) const
    {
      try {
        result = debug_instr_read_internal(addr,trans,size);
        return true;
      }
      catch (...) {
        return false;
      }
    }
    
    bool debug_instr_write(bool trans,addr_t addr,uint32_t value,int size)
    {
      try {
        NullWriteLog logger;
        if (trans) {
          InstrReadTranslate translate(*this);
          SimpleCache<GlobalMemoryType&> cache(mem);
          mem_write_sized<InstrReadTranslate,SimpleCache<GlobalMemoryType&>,NullWriteLog,0>(translate,cache,logger,addr,value,size);
        } else {
          NullTranslate translate;
          SimpleCache<GlobalMemoryType&> cache(mem);
          mem_write_sized<NullTranslate,SimpleCache<GlobalMemoryType&>,NullWriteLog,0>(translate,cache,logger,addr,value,size);
        }
        return true;
      }
      catch (...) {
        return false;
      }
    }
    
    void setDmiLatencies(unsigned r,unsigned w)
    {
      _read_latency = r;
      _write_latency = w;
    }
    
    //
    // Memory initialization method.
    //
    
    template <class Translate, class Cache> void global_mem_set(Translate t,Cache c,addr_t addr, uint64_t data, unsigned size)
    {
      NullWriteLog	 l;
      DPrint(std::hex << "0x" << addr << ": 0x" << data << std::dec << "\n");
      mem_write_sized<Translate,Cache,NullWriteLog,0>(t,c,l,addr,data,size);
    }
    
    template <class Translate, class Cache> uint64_t global_mem_get(Translate t,Cache c,addr_t addr, unsigned size) const
    {
      NullReadLog	 l;
      return mem_read_sized<Translate,Cache,NullReadLog,0>(t,c,l,addr,size);
    }
    
    void base_global_mem_set(addr_t ra,uint64_t data,unsigned size)
    {
      NullTranslate t;
      SimpleCache<GlobalMemoryType&>   c(mem);
      global_mem_set(t,c,ra,data,size);
    }
    
    uint64_t base_global_mem_get(addr_t ra,unsigned size) const
    {
      NullTranslate t;
      SimpleCache<GlobalMemoryType&>   c(mem);
      return global_mem_get(t,c,ra,size);
    }
    
    void setMem(unsigned id, addr_t addr, uint64_t data, unsigned size) {
      switch (id) {
        case 0:
        base_global_mem_set(addr,data,size);
        break;
        default:
        RError("Non existing memory:  " << id);
      }
    }
    
    uint64_t getMem(unsigned id, addr_t addr,unsigned size) const {
      switch (id) {
        case 0:
        return base_global_mem_get(addr,size);
        default:
        RError("Non existing memory:  " << id);
      }
    }
    
    unsigned getMemId(const std::string &name) const {
      if (name == GlobalMemName) {
        return 0;
      }
      else {
        RError("Non existing memory name:  " << name);
      }
    }
    
    uint64_t mem_read64(addr_t addr)
    {
      return adl::mem_read64(addr);
    }
    
    uint32_t mem_read32(addr_t addr)
    {
      return adl::mem_read32(addr);
    }
    
    uint16_t mem_read16(addr_t addr)
    {
      return adl::mem_read16(addr);
    }
    
    uint8_t mem_read8(addr_t addr)
    {
      return adl::mem_read8(addr);
    }
    
    void mem_write64(addr_t addr,uint64_t value,unsigned nb)
    {
      adl::mem_write64(addr,value,nb);
    }
    
    void mem_write32(addr_t addr,uint32_t value,unsigned nb)
    {
      adl::mem_write32(addr,value,nb);
    }
    
    void mem_write16(addr_t addr,uint16_t value,unsigned nb)
    {
      adl::mem_write16(addr,value,nb);
    }
    
    void mem_write8(addr_t addr,uint8_t value)
    {
      adl::mem_write8(addr,value);
    }
    
    uint32_t mem_read32(const byte_t *buf) const
    {
      return byte_read32(buf,0);
    }
    
    void mem_write32(byte_t *buf,uint32_t v,unsigned nb = BytesPerWord)
    {
      byte_write32(buf,0,v,nb);
    }
    
    static MemInfos initMemInfo() {
      MemInfos info;
      sort_info(info);
      return info;
    }
    
    static const MemInfos &baseMemInfo() {
      static MemInfos info = initMemInfo();
      return info;
    }
    
    const MemInfos &getMemInfo() const {
      return baseMemInfo();
    }
    
    void showMem(ReportBase& rb)  {
    }
    
    bool debug_data_read (uint32_t &result,unsigned id,bool trans,addr_t addr,int size) const {
      try {
        result = mem_debug_data_read(trans,addr,size);
        return true;
      } catch (...) {
        return false;
      }
    }
    
    bool debug_data_write (unsigned id,bool trans,addr_t addr,uint32_t value,int size) {
      try {
        mem_debug_data_write(trans,addr,value,size);
        return true;
      } catch (...) {
        return false;
      }
    }
    
    void check_read_watchpoints(addr_t ea,bool pre,int num_bytes)
    {
      if (!WatchpointsEnabled) return;
      MemPoints::const_iterator i = _read_watchpoints.find(ea & ~WordMask);
      if (i != _read_watchpoints.end()) {
        _logger->log_watchpoint(ea,pre,wpRead,num_bytes,0,i->second._handle,i->second._payload);
      }
    }
    
    void check_write_watchpoints(addr_t ea,bool pre,int num_bytes,uint64_t value)
    {
      if (!WatchpointsEnabled) return;
      MemPoints::const_iterator i = _write_watchpoints.find(ea & ~WordMask);
      if (i != _write_watchpoints.end()) {
        _logger->log_watchpoint(ea,pre,wpWrite,num_bytes,value,i->second._handle,i->second._payload);
      }
    }
    
    void enable_dmi_caches () {
      _dmi_mem_read.enable();
      _dmi_mem_write.enable();
    }
    
    void disable_dmi_caches () {
      _dmi_mem_read.disable();
      _dmi_mem_write.disable();
    }
    
    std::string getInstrMemName() const { return "Mem"; }
    std::string getDataMemName() const { return "Mem"; }
    //
    // Register Data Structures
    //
    
    struct CIA_t : public Tag<bits<32> > {
      CIA_t()
      {};
    };
    
    CIA_t &CIA() { return _CIA_Reg; };
    const CIA_t &CIA() const { return _CIA_Reg; };
    CIA_t &Dbg_CIA() { return _CIA_Reg; };
    const CIA_t &Dbg_CIA() const { return _CIA_Reg; };
    
    struct CR_t : public Tag<bits<32> > {
      CR_t()
      {};
    };
    
    CR_t &CR() { return _CR_Reg; };
    const CR_t &CR() const { return _CR_Reg; };
    CR_t &Dbg_CR() { return _CR_Reg; };
    const CR_t &Dbg_CR() const { return _CR_Reg; };
    
    struct CTR_t : public Tag<bits<32> > {
      CTR_t()
      {};
    };
    
    CTR_t &CTR() { return _CTR_Reg; };
    const CTR_t &CTR() const { return _CTR_Reg; };
    CTR_t &Dbg_CTR() { return _CTR_Reg; };
    const CTR_t &Dbg_CTR() const { return _CTR_Reg; };
    
    struct HID0_t : public Tag<bits<32> > {
      HID0_t()
      {};
    };
    
    HID0_t &HID0() { return _HID0_Reg; };
    const HID0_t &HID0() const { return _HID0_Reg; };
    HID0_t &Dbg_HID0() { return _HID0_Reg; };
    const HID0_t &Dbg_HID0() const { return _HID0_Reg; };
    
    struct HID1_t : public Tag<bits<32> > {
      HID1_t()
      {};
    };
    
    HID1_t &HID1() { return _HID1_Reg; };
    const HID1_t &HID1() const { return _HID1_Reg; };
    HID1_t &Dbg_HID1() { return _HID1_Reg; };
    const HID1_t &Dbg_HID1() const { return _HID1_Reg; };
    
    struct LR_t : public Tag<bits<32> > {
      LR_t()
      {};
    };
    
    LR_t &LR() { return _LR_Reg; };
    const LR_t &LR() const { return _LR_Reg; };
    LR_t &Dbg_LR() { return _LR_Reg; };
    const LR_t &Dbg_LR() const { return _LR_Reg; };
    
    struct NIA_t : public Tag<bits<32> > {
      NIA_t()
      {};
    };
    
    NIA_t &NIA() { return _NIA_Reg; };
    const NIA_t &NIA() const { return _NIA_Reg; };
    NIA_t &Dbg_NIA() { return _NIA_Reg; };
    const NIA_t &Dbg_NIA() const { return _NIA_Reg; };
    
    struct XER_t : public Tag<bits<32> > {
      bits<32> _reserved_mask;
      XER_t() : 
      _reserved_mask("0b00011111111111111111111111111111")
      {};
    };
    
    XER_t &XER() { return _XER_Reg; };
    const XER_t &XER() const { return _XER_Reg; };
    XER_t &Dbg_XER() { return _XER_Reg; };
    const XER_t &Dbg_XER() const { return _XER_Reg; };
    
    //
    // Register File Data Structures
    //
    
    struct GPR_t : public RegFile<Tag<bits<32> >,32,32> {
      void set_value(unsigned index,const bits<32> &x) { operator()(index).set_value(x); }
      GPR_t()
      {};
      bits<32> get_value(unsigned index) const { return operator()(index).get_data(); };
      static bool validIndex(unsigned index) ATTRIBUTE_USED {
        return (index < 32);
      };
      static unsigned getReadIndex(unsigned index) ATTRIBUTE_INLINE {
        return index;
      }
      static unsigned getWriteIndex(unsigned index) ATTRIBUTE_INLINE {
        return index;
      }
      template <size_t Nb> static bool validIndex(bits<Nb> x) { return validIndex(x.uint32()); };
    };
    
    GPR_t &GPR() { return _GPR_Reg; };
    const GPR_t &GPR() const { return _GPR_Reg; };
    GPR_t &Dbg_GPR() { return _GPR_Reg; };
    const GPR_t &Dbg_GPR() const { return _GPR_Reg; };
    
    bool GPR_debug_set(unsigned index,const bits<32> &value) {
      if (index >= Dbg_GPR().size()) { return false; };
      Dbg_GPR().set_value(GPR_t::getWriteIndex(index),value);
      return true;
    }
    
    bits<32> GPR_debug_get(unsigned index,bool &rc) const {
      if (index >= Dbg_GPR().size()) { rc = false; return 0; };
      rc = true;
      return Dbg_GPR().get_value(GPR_t::getReadIndex(index));
    }
    
    struct SPR_t {
      static bool validIndex(unsigned index) ATTRIBUTE_USED {
        switch (index) {
          case 1:
          return true;
          case 8:
          return true;
          case 9:
          return true;
          case 50:
          return true;
          case 51:
          return true;
          default:
          return false;
        }
      };
      static unsigned getReadIndex(unsigned index) ATTRIBUTE_INLINE {
        return index;
      }
      static unsigned getWriteIndex(unsigned index) ATTRIBUTE_INLINE {
        return index;
      }
      template <size_t Nb> static bool validIndex(bits<Nb> x) { return validIndex(x.uint32()); };
    };
    
    SPR_t &SPR() { return _SPR_Reg; };
    const SPR_t &SPR() const { return _SPR_Reg; };
    SPR_t &Dbg_SPR() { return _SPR_Reg; };
    const SPR_t &Dbg_SPR() const { return _SPR_Reg; };
    
    bool SPR_debug_set(unsigned index,const bits<32> &value) {
      switch (index) {
        case 1:
        Dbg_XER().set_value(value);
        return true;
        case 8:
        Dbg_LR().set_value(value);
        return true;
        case 9:
        Dbg_CTR().set_value(value);
        return true;
        case 50:
        Dbg_HID0().set_value(value);
        return true;
        case 51:
        Dbg_HID1().set_value(value);
        return true;
        default:
        return false;
      }
    }
    
    bits<32> SPR_debug_get(unsigned index,bool &rc) const {
      switch (index) {
        case 1:
        return Dbg_XER().get_value();
        case 8:
        return Dbg_LR().get_value();
        case 9:
        return Dbg_CTR().get_value();
        case 50:
        return Dbg_HID0().get_value();
        case 51:
        return Dbg_HID1().get_value();
        default:
        rc = false;
        return 0;
      }
    }
    
    //
    // Register Variables
    //
    
    //
    // Register File Variables
    //
    
    //
    // Register Handling Code.
    //
    
    // Register CIA
    void CIA_trace()  ATTRIBUTE_USED {
    }
    
    void CIA_write_internal(const bits<32> &x) {
      CIA().set_data(*this) = x;
    };
    void CIA_write(const bits<32> &x) {
      CIA_write_internal(x);
      CondRegTrace(CIA_trace());
    };
    bits<32> & CIA_read_internal() {
      return CIA().get_data(*this);
    };
    
    bits<32> & CIA_read()  ATTRIBUTE_USED {
      return CIA_read_internal();
    }
    bits<32> & CIA_read_modify() {
      bits<32> &tmp = CIA_read_internal();
      CIA().add_target(*this);
      return tmp;
    }
    void CIA_reset_tag() { CIA().reset_tag(); }
    void CIA_reset_value() {
      bits<32> x("");
      CIA().set_value(x);
      CIA().reset_tag();
    };
    
    // Register CR
    void CR_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(1,"CR",CR().get_data().rnum());
      }
    }
    
    bits<32> & CR_read_internal() {
      return CR().get_data(*this);
    };
    
    bits<32> & CR_read()  ATTRIBUTE_USED {
      return CR_read_internal();
    }
    bits<32> & CR_read_modify() {
      bits<32> &tmp = CR_read_internal();
      CR().add_target(*this);
      return tmp;
    }
    void CR_reset_tag() { CR().reset_tag(); }
    void CR_reset_value() {
      bits<32> x("");
      CR().set_value(x);
      CR().reset_tag();
    };
    template <size_t Nb>
    void CR_slice_set_internal(unsigned l,unsigned r,const bits<Nb> &x) {
      CR_read_modify().set(l,r,x);
    };
    template <size_t Nb>
    void CR_slice_set(unsigned l,unsigned r,const bits<Nb> &x) {
      CR_slice_set_internal(l,r,x);
      CondRegTrace(CR_trace());
    }
    
    void CR_slice_set(unsigned l,unsigned r,uint64_t x) {
      CR_slice_set(l,r,bits<32>(x));
    };
    
    // Register CTR
    void CTR_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(2,"CTR",CTR().get_data().rnum());
      }
    }
    
    void CTR_write_internal(const bits<32> &x) {
      CTR().set_data(*this) = x;
    };
    void CTR_write(const bits<32> &x) {
      CTR_write_internal(x);
      CondRegTrace(CTR_trace());
    };
    bits<32> & CTR_read_internal() {
      return CTR().get_data(*this);
    };
    
    bits<32> & CTR_read()  ATTRIBUTE_USED {
      return CTR_read_internal();
    }
    bits<32> & CTR_read_modify() {
      bits<32> &tmp = CTR_read_internal();
      CTR().add_target(*this);
      return tmp;
    }
    void CTR_reset_tag() { CTR().reset_tag(); }
    void CTR_reset_value() {
      bits<32> x("");
      CTR().set_value(x);
      CTR().reset_tag();
    };
    
    // Register HID0
    void HID0_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(3,"HID0",HID0().get_data().rnum());
      }
    }
    
    void HID0_write_internal(const bits<32> &x) {
      HID0().set_data(*this) = x;
    };
    void HID0_write(const bits<32> &x) {
      HID0_write_internal(x);
      CondRegTrace(HID0_trace());
    };
    bits<32> & HID0_read_internal() {
      return HID0().get_data(*this);
    };
    
    bits<32> & HID0_read()  ATTRIBUTE_USED {
      return HID0_read_internal();
    }
    bits<32> & HID0_read_modify() {
      bits<32> &tmp = HID0_read_internal();
      HID0().add_target(*this);
      return tmp;
    }
    void HID0_reset_tag() { HID0().reset_tag(); }
    void HID0_reset_value() {
      bits<32> x("");
      HID0().set_value(x);
      HID0().reset_tag();
    };
    
    // Register HID1
    void HID1_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(4,"HID1",HID1().get_data().rnum());
      }
    }
    
    void HID1_write_internal(const bits<32> &x) {
      HID1().set_data(*this) = x;
    };
    void HID1_write(const bits<32> &x) {
      HID1_write_internal(x);
      CondRegTrace(HID1_trace());
    };
    bits<32> & HID1_read_internal() {
      return HID1().get_data(*this);
    };
    
    bits<32> & HID1_read()  ATTRIBUTE_USED {
      return HID1_read_internal();
    }
    bits<32> & HID1_read_modify() {
      bits<32> &tmp = HID1_read_internal();
      HID1().add_target(*this);
      return tmp;
    }
    void HID1_reset_tag() { HID1().reset_tag(); }
    void HID1_reset_value() {
      bits<32> x("");
      HID1().set_value(x);
      HID1().reset_tag();
    };
    
    // Register LR
    void LR_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(5,"LR",LR().get_data().rnum());
      }
    }
    
    void LR_write_internal(const bits<32> &x) {
      LR().set_data(*this) = x;
    };
    void LR_write(const bits<32> &x) {
      LR_write_internal(x);
      CondRegTrace(LR_trace());
    };
    bits<32> & LR_read_internal() {
      return LR().get_data(*this);
    };
    
    bits<32> & LR_read()  ATTRIBUTE_USED {
      return LR_read_internal();
    }
    bits<32> & LR_read_modify() {
      bits<32> &tmp = LR_read_internal();
      LR().add_target(*this);
      return tmp;
    }
    void LR_reset_tag() { LR().reset_tag(); }
    void LR_reset_value() {
      bits<32> x("");
      LR().set_value(x);
      LR().reset_tag();
    };
    
    // Register NIA
    void NIA_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(6,"NIA",NIA().get_data().rnum());
      }
      if (Tracing && (_trace_mode && ttBranchTaken)) {
        _logger->log_branch_taken(NIA().get_data().uint64());
      }
    }
    
    void NIA_write_internal(const bits<32> &x) {
      NIA().set_data(*this) = x;
    };
    void NIA_write(const bits<32> &x) {
      NIA_write_internal(x);
      CondRegBranchTrace(NIA_trace());
    };
    void NIA_trace_simple()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(6,"NIA",NIA().get_data().rnum());
      }
    }
    
    void NIA_write_internal_simple(const bits<32> &x) {
      NIA().set_data(*this) = x;
    };
    void NIA_write_simple(const bits<32> &x)  ATTRIBUTE_INLINE {
      NIA_write_internal_simple(x);
      CondRegBranchTrace(NIA_trace_simple());
    };
    bits<32> & NIA_read_internal() {
      return NIA().get_data(*this);
    };
    
    bits<32> & NIA_read()  ATTRIBUTE_USED {
      return NIA_read_internal();
    }
    bits<32> & NIA_read_modify() {
      bits<32> &tmp = NIA_read_internal();
      NIA().add_target(*this);
      return tmp;
    }
    void NIA_reset_tag() { NIA().reset_tag(); }
    void NIA_reset_value() {
      bits<32> x("");
      NIA().set_value(x);
      NIA().reset_tag();
    };
    
    // Register XER
    void XER_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(7,"XER",XER().get_data().rnum());
      }
    }
    
    void XER_write_internal(const bits<32> &x) {
      XER().set_data(*this) = (x & ~XER()._reserved_mask);
    };
    void XER_write(const bits<32> &x) {
      XER_write_internal(x);
      CondRegTrace(XER_trace());
    };
    bits<32> & XER_read_internal() {
      return XER().get_data(*this);
    };
    
    bits<32> & XER_read()  ATTRIBUTE_USED {
      return XER_read_internal();
    }
    bits<32> & XER_read_modify() {
      bits<32> &tmp = XER_read_internal();
      XER().add_target(*this);
      return tmp;
    }
    void XER_reset_tag() { XER().reset_tag(); }
    void XER_reset_value() {
      bits<32> x("");
      XER().set_value(x);
      XER().reset_tag();
    };
    bits<1> XER_CA_read()  ATTRIBUTE_USED {
      return XER_read(). get<1>(2,2);
    };
    
    void XER_CA_write(const bits<1> &x)  ATTRIBUTE_USED {
      XER_read_modify().set(2,2,x);
      CondRegTrace(XER_trace());
    };
    bits<1> XER_OV_read()  ATTRIBUTE_USED {
      return XER_read(). get<1>(1,1);
    };
    
    void XER_OV_write(const bits<1> &x)  ATTRIBUTE_USED {
      XER_read_modify().set(1,1,x);
      CondRegTrace(XER_trace());
    };
    bits<1> XER_SO_read()  ATTRIBUTE_USED {
      return XER_read(). get<1>(0,0);
    };
    
    void XER_SO_write(const bits<1> &x)  ATTRIBUTE_USED {
      XER_read_modify().set(0,0,x);
      CondRegTrace(XER_trace());
    };
    
    
    //
    // Register File Handling Code.
    //
    
    // Register File GPR
    void GPR_trace(unsigned index)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegfileWrite)) {
        _logger->log_regfile_write(8,"GPR",index,GPR().operator()(index).get_data().rnum());
      }
    }
    
    void GPR_write_internal(unsigned index,const bits<32> &x)  ATTRIBUTE_USED {
      GPR()(index).set_data(*this) = x;
    };
    void GPR_write_internal_traced(unsigned index,const bits<32> &x)  ATTRIBUTE_USED {
      GPR_write_internal(index,x);
      CondRegTrace(GPR_trace(index));
    }
    void GPR_write(unsigned index,const bits<32> &x) {
      GPR_write_internal_traced(index,x);
    };
    bits<32> &GPR_read_internal(unsigned index)  ATTRIBUTE_USED {
      return GPR()(index).get_data(*this);
    };
    bits<32> & GPR_read(unsigned index)  ATTRIBUTE_USED {
      return GPR_read_internal(index);
    }
    
    bits<32> & GPR_read_modify(unsigned index)  ATTRIBUTE_USED {
      bits<32> &tmp = GPR_read_internal(index);
      GPR()(index).add_target(*this);
      return tmp;
    }
    void GPR_reset_value() {
      GPR().reset(bits<32>(""));
      GPR().reset_tags();
    };
    
    // Register File SPR
    void SPR_write_internal(unsigned index,const bits<32> &x)  ATTRIBUTE_USED {
      switch (index) {
        case 1:
        XER_write(x);
        break;
        case 8:
        LR_write(x);
        break;
        case 9:
        CTR_write(x);
        break;
        case 50:
        HID0_write(x);
        break;
        case 51:
        HID1_write(x);
        break;
        default:
        std::cerr << "Attempt to access SPR(" << index << "), which is undefined.\n\n";
        abort();
      }
    };
    void SPR_write_internal_traced(unsigned index,const bits<32> &x)  ATTRIBUTE_USED {
      SPR_write_internal(index,x);
    }
    void SPR_write(unsigned index,const bits<32> &x) {
      SPR_write_internal_traced(index,x);
    };
    bits<32> SPR_read_internal(unsigned index) ATTRIBUTE_USED  {
      switch (index) {
        case 1:
        return XER_read();
        case 8:
        return LR_read();
        case 9:
        return CTR_read();
        case 50:
        return HID0_read();
        case 51:
        return HID1_read();
        default:
        return 0;
      }
    };
    bits<32> SPR_read(unsigned index)  ATTRIBUTE_USED {
      return SPR_read_internal(index);
    }
    
    
    
    bool getRegIndex(int &rindex,int &rfindex,const std::string &name) const
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      rindex = rfindex = -1;
      typedef _STDEXT hash_map<std::string,unsigned> RegNames;
      static RegNames::value_type inits[] = {
        RegNames::value_type("CIA",0),
        RegNames::value_type("CR",1),
        RegNames::value_type("CTR",2),
        RegNames::value_type("HID0",3),
        RegNames::value_type("HID1",4),
        RegNames::value_type("LR",5),
        RegNames::value_type("NIA",6),
        RegNames::value_type("XER",7),
        RegNames::value_type("GPR",8),
        RegNames::value_type("SPR",9),
      };
      static RegNames regnames(&inits[0],&inits[10]);
      RegNames::const_iterator iter = regnames.find(name);
      if (iter != regnames.end()) {
        rindex = iter->second;
        return true;
      }
      if ( (rfindex = checkRegFileName(name,3,"GPR")) >= 0 ) {
        rindex = 8;
        return true;
      }
      if ( (rfindex = checkRegFileName(name,3,"SPR")) >= 0 ) {
        rindex = 9;
        return true;
      }
      return false;
    }
    #endif // ADL_HEADER
    
    virtual bool setReg(const std::string &name,unsigned index,const rnumber::RNumber &value)
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      int rindex,rfindex;
      if (!getRegIndex(rindex,rfindex,name)) {
        return false;
      }
      return setReg(rindex,((rfindex >= 0) ? rfindex : index),value);
    }
    #endif // ADL_HEADER
    
    virtual bool setReg(unsigned rindex,unsigned index,const rnumber::RNumber &value)
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      switch (rindex) {
        case 0:
        Dbg_CIA().set_value(bits<32>(value));
        break;
        case 1:
        Dbg_CR().set_value(bits<32>(value));
        break;
        case 2:
        Dbg_CTR().set_value(bits<32>(value));
        break;
        case 3:
        Dbg_HID0().set_value(bits<32>(value));
        break;
        case 4:
        Dbg_HID1().set_value(bits<32>(value));
        break;
        case 5:
        Dbg_LR().set_value(bits<32>(value));
        break;
        case 6:
        Dbg_NIA().set_value(bits<32>(value));
        break;
        case 7:
        Dbg_XER().set_value(bits<32>(value));
        break;
        case 8:
        return GPR_debug_set(index,value);
        break;
        case 9:
        return SPR_debug_set(index,value);
        break;
        default:
        return false;
      }
      checkWatches();
      return true;
    }
    #endif // ADL_HEADER
    
    virtual bool setReg(const std::string &name,unsigned index,uint64_t value)
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      int rindex,rfindex;
      if (!getRegIndex(rindex,rfindex,name)) {
        return false;
      }
      return setReg(rindex,((rfindex >= 0) ? rfindex : index),value);
    }
    #endif // ADL_HEADER
    
    virtual bool setReg(unsigned rindex,unsigned index,uint64_t value)
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      switch (rindex) {
        case 0:
        Dbg_CIA().set_value(bits<32>(value));
        break;
        case 1:
        Dbg_CR().set_value(bits<32>(value));
        break;
        case 2:
        Dbg_CTR().set_value(bits<32>(value));
        break;
        case 3:
        Dbg_HID0().set_value(bits<32>(value));
        break;
        case 4:
        Dbg_HID1().set_value(bits<32>(value));
        break;
        case 5:
        Dbg_LR().set_value(bits<32>(value));
        break;
        case 6:
        Dbg_NIA().set_value(bits<32>(value));
        break;
        case 7:
        Dbg_XER().set_value(bits<32>(value));
        break;
        case 8:
        return GPR_debug_set(index,value);
        break;
        case 9:
        return SPR_debug_set(index,value);
        break;
        default:
        return false;
      }
      checkWatches();
      return true;
    }
    #endif // ADL_HEADER
    
    virtual bool writeReg(const std::string &name,unsigned index,const rnumber::RNumber &value)
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      int rindex,rfindex;
      if (!getRegIndex(rindex,rfindex,name)) {
        return false;
      }
      return writeReg(rindex,((rfindex >= 0) ? rfindex : index),value);
    }
    #endif // ADL_HEADER
    
    virtual bool writeReg(unsigned rindex,unsigned index,const rnumber::RNumber &value)
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      bool rc = true;
      try {
        switch (rindex) {
          case 0:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(0,"CIA",value);
          }
          CIA_write(value);
          break;
          case 2:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(2,"CTR",value);
          }
          CTR_write(value);
          break;
          case 3:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(3,"HID0",value);
          }
          HID0_write(value);
          break;
          case 4:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(4,"HID1",value);
          }
          HID1_write(value);
          break;
          case 5:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(5,"LR",value);
          }
          LR_write(value);
          break;
          case 6:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(6,"NIA",value);
          }
          NIA_write(value);
          break;
          case 7:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(7,"XER",value);
          }
          XER_write(value);
          break;
          case 8:
          if (GPR_t::validIndex(index)) {
            if (Tracing && (_trace_mode & (ttRegWrite))) {
              _logger->log_regfile_ext_write(8,"GPR",index,value);
            }
            GPR_write(index,value);
            break;
          }
          rc = false;
          break;
          case 9:
          if (SPR_t::validIndex(index)) {
            if (Tracing && (_trace_mode & (ttRegWrite))) {
              _logger->log_regfile_ext_write(9,"SPR",index,value);
            }
            SPR_write(index,value);
            break;
          }
          rc = false;
          break;
          default:
          rc = false;
        }
      }
      catch (...) {
        throw;
      }
      check_context_updates();
      return rc;
    }
    #endif // ADL_HEADER
    
    virtual bool writeReg(const std::string &name,unsigned index,uint64_t value)
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      int rindex,rfindex;
      if (!getRegIndex(rindex,rfindex,name)) {
        return false;
      }
      return writeReg(rindex,((rfindex >= 0) ? rfindex : index),value);
    }
    #endif // ADL_HEADER
    
    virtual bool writeReg(unsigned rindex,unsigned index,uint64_t value)
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      bool rc = true;
      try {
        switch (rindex) {
          case 0:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(0,"CIA",value);
          }
          CIA_write(value);
          break;
          case 2:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(2,"CTR",value);
          }
          CTR_write(value);
          break;
          case 3:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(3,"HID0",value);
          }
          HID0_write(value);
          break;
          case 4:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(4,"HID1",value);
          }
          HID1_write(value);
          break;
          case 5:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(5,"LR",value);
          }
          LR_write(value);
          break;
          case 6:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(6,"NIA",value);
          }
          NIA_write(value);
          break;
          case 7:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(7,"XER",value);
          }
          XER_write(value);
          break;
          case 8:
          if (GPR_t::validIndex(index)) {
            if (Tracing && (_trace_mode & (ttRegWrite))) {
              _logger->log_regfile_ext_write(8,"GPR",index,value);
            }
            GPR_write(index,value);
            break;
          }
          rc = false;
          break;
          case 9:
          if (SPR_t::validIndex(index)) {
            if (Tracing && (_trace_mode & (ttRegWrite))) {
              _logger->log_regfile_ext_write(9,"SPR",index,value);
            }
            SPR_write(index,value);
            break;
          }
          rc = false;
          break;
          default:
          rc = false;
        }
      }
      catch (...) {
        throw;
      }
      check_context_updates();
      return rc;
    }
    #endif // ADL_HEADER
    
    virtual bool getReg(const std::string &name,unsigned index,rnumber::RNumber &value) const
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      int rindex,rfindex;
      if (!getRegIndex(rindex,rfindex,name)) {
        return false;
      }
      return getReg(rindex,((rfindex >= 0) ? rfindex : index),value);
    }
    #endif // ADL_HEADER
    
    virtual bool getReg(unsigned rindex,unsigned index,rnumber::RNumber &value) const
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      bool rc = true;
      switch (rindex) {
        case 0:
        value = Dbg_CIA().get_value().rnum();
        break;
        case 1:
        value = Dbg_CR().get_value().rnum();
        break;
        case 2:
        value = Dbg_CTR().get_value().rnum();
        break;
        case 3:
        value = Dbg_HID0().get_value().rnum();
        break;
        case 4:
        value = Dbg_HID1().get_value().rnum();
        break;
        case 5:
        value = Dbg_LR().get_value().rnum();
        break;
        case 6:
        value = Dbg_NIA().get_value().rnum();
        break;
        case 7:
        value = Dbg_XER().get_value().rnum();
        break;
        case 8:
        value = GPR_debug_get(index,rc).rnum();
        break;
        case 9:
        value = SPR_debug_get(index,rc).rnum();
        break;
        default:
        return false;
      }
      return rc;
    }
    #endif // ADL_HEADER
    
    virtual bool getReg(const std::string &name,unsigned index,uint64_t &value) const
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      int rindex,rfindex;
      if (!getRegIndex(rindex,rfindex,name)) {
        return false;
      }
      return getReg(rindex,((rfindex >= 0) ? rfindex : index),value);
    }
    #endif // ADL_HEADER
    
    virtual bool getReg(unsigned rindex,unsigned index,uint64_t &value) const
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      bool rc = true;
      switch (rindex) {
        case 0:
        value = Dbg_CIA().get_value().uint64();
        break;
        case 1:
        value = Dbg_CR().get_value().uint64();
        break;
        case 2:
        value = Dbg_CTR().get_value().uint64();
        break;
        case 3:
        value = Dbg_HID0().get_value().uint64();
        break;
        case 4:
        value = Dbg_HID1().get_value().uint64();
        break;
        case 5:
        value = Dbg_LR().get_value().uint64();
        break;
        case 6:
        value = Dbg_NIA().get_value().uint64();
        break;
        case 7:
        value = Dbg_XER().get_value().uint64();
        break;
        case 8:
        value = GPR_debug_get(index,rc).uint64();
        break;
        case 9:
        value = SPR_debug_get(index,rc).uint64();
        break;
        default:
        return false;
      }
      return rc;
    }
    #endif // ADL_HEADER
    
    virtual bool readReg(const std::string &name,unsigned index,rnumber::RNumber &value)
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      int rindex,rfindex;
      if (!getRegIndex(rindex,rfindex,name)) {
        return false;
      }
      return readReg(rindex,((rfindex >= 0) ? rfindex : index),value);
    }
    #endif // ADL_HEADER
    
    virtual bool readReg(unsigned rindex,unsigned index,rnumber::RNumber &value)
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      uint64_t of = clear_tracing();
      bool rc = true;
      try {
        switch (rindex) {
          case 0:
          value = CIA_read().rnum();
          break;
          case 2:
          value = CTR_read().rnum();
          break;
          case 3:
          value = HID0_read().rnum();
          break;
          case 4:
          value = HID1_read().rnum();
          break;
          case 5:
          value = LR_read().rnum();
          break;
          case 6:
          value = NIA_read().rnum();
          break;
          case 7:
          value = XER_read().rnum();
          break;
          case 8:
          if (GPR_t::validIndex(index)) {
            value = GPR_read(index).rnum();
            break;
          }
          rc = false;
          break;
          case 9:
          if (SPR_t::validIndex(index)) {
            value = SPR_read(index).rnum();
            break;
          }
          rc = false;
          break;
          default:
          rc = false;
        }
      }
      catch (...) {
        set_tracing(of);
        throw;
      }
      set_tracing(of);
      return rc;
    }
    #endif // ADL_HEADER
    
    virtual bool readReg(const std::string &name,unsigned index,uint64_t &value)
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      int rindex,rfindex;
      if (!getRegIndex(rindex,rfindex,name)) {
        return false;
      }
      return readReg(rindex,((rfindex >= 0) ? rfindex : index),value);
    }
    #endif // ADL_HEADER
    
    virtual bool readReg(unsigned rindex,unsigned index,uint64_t &value)
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      uint64_t of = clear_tracing();
      bool rc = true;
      try {
        switch (rindex) {
          case 0:
          value = CIA_read().uint64();
          break;
          case 2:
          value = CTR_read().uint64();
          break;
          case 3:
          value = HID0_read().uint64();
          break;
          case 4:
          value = HID1_read().uint64();
          break;
          case 5:
          value = LR_read().uint64();
          break;
          case 6:
          value = NIA_read().uint64();
          break;
          case 7:
          value = XER_read().uint64();
          break;
          case 8:
          if (GPR_t::validIndex(index)) {
            value = GPR_read(index).uint64();
            break;
          }
          rc = false;
          break;
          case 9:
          if (SPR_t::validIndex(index)) {
            value = SPR_read(index).uint64();
            break;
          }
          rc = false;
          break;
          default:
          rc = false;
        }
      }
      catch (...) {
        set_tracing(of);
        throw;
      }
      set_tracing(of);
      return rc;
    }
    #endif // ADL_HEADER
    
    virtual void showRegs(ReportBase &rb) const 
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      rb.report_reg(this, 1,"CR",Dbg_CR().get_value().rnum(),false);
      rb.report_reg(this, 2,"CTR",Dbg_CTR().get_value().rnum(),false);
      rb.report_reg(this, 3,"HID0",Dbg_HID0().get_value().rnum(),false);
      rb.report_reg(this, 4,"HID1",Dbg_HID1().get_value().rnum(),false);
      rb.report_reg(this, 5,"LR",Dbg_LR().get_value().rnum(),false);
      rb.report_reg(this, 6,"NIA",Dbg_NIA().get_value().rnum(),false);
      rb.report_reg(this, 7,"XER",Dbg_XER().get_value().rnum(),false);
      rb.report_regfile(this,8,"GPR",0,Dbg_GPR().get_value(GPR_t::getReadIndex( 0)).rnum(),false);
      rb.report_regfile(this,8,"GPR",1,Dbg_GPR().get_value(GPR_t::getReadIndex( 1)).rnum(),false);
      rb.report_regfile(this,8,"GPR",2,Dbg_GPR().get_value(GPR_t::getReadIndex( 2)).rnum(),false);
      rb.report_regfile(this,8,"GPR",3,Dbg_GPR().get_value(GPR_t::getReadIndex( 3)).rnum(),false);
      rb.report_regfile(this,8,"GPR",4,Dbg_GPR().get_value(GPR_t::getReadIndex( 4)).rnum(),false);
      rb.report_regfile(this,8,"GPR",5,Dbg_GPR().get_value(GPR_t::getReadIndex( 5)).rnum(),false);
      rb.report_regfile(this,8,"GPR",6,Dbg_GPR().get_value(GPR_t::getReadIndex( 6)).rnum(),false);
      rb.report_regfile(this,8,"GPR",7,Dbg_GPR().get_value(GPR_t::getReadIndex( 7)).rnum(),false);
      rb.report_regfile(this,8,"GPR",8,Dbg_GPR().get_value(GPR_t::getReadIndex( 8)).rnum(),false);
      rb.report_regfile(this,8,"GPR",9,Dbg_GPR().get_value(GPR_t::getReadIndex( 9)).rnum(),false);
      rb.report_regfile(this,8,"GPR",10,Dbg_GPR().get_value(GPR_t::getReadIndex( 10)).rnum(),false);
      rb.report_regfile(this,8,"GPR",11,Dbg_GPR().get_value(GPR_t::getReadIndex( 11)).rnum(),false);
      rb.report_regfile(this,8,"GPR",12,Dbg_GPR().get_value(GPR_t::getReadIndex( 12)).rnum(),false);
      rb.report_regfile(this,8,"GPR",13,Dbg_GPR().get_value(GPR_t::getReadIndex( 13)).rnum(),false);
      rb.report_regfile(this,8,"GPR",14,Dbg_GPR().get_value(GPR_t::getReadIndex( 14)).rnum(),false);
      rb.report_regfile(this,8,"GPR",15,Dbg_GPR().get_value(GPR_t::getReadIndex( 15)).rnum(),false);
      rb.report_regfile(this,8,"GPR",16,Dbg_GPR().get_value(GPR_t::getReadIndex( 16)).rnum(),false);
      rb.report_regfile(this,8,"GPR",17,Dbg_GPR().get_value(GPR_t::getReadIndex( 17)).rnum(),false);
      rb.report_regfile(this,8,"GPR",18,Dbg_GPR().get_value(GPR_t::getReadIndex( 18)).rnum(),false);
      rb.report_regfile(this,8,"GPR",19,Dbg_GPR().get_value(GPR_t::getReadIndex( 19)).rnum(),false);
      rb.report_regfile(this,8,"GPR",20,Dbg_GPR().get_value(GPR_t::getReadIndex( 20)).rnum(),false);
      rb.report_regfile(this,8,"GPR",21,Dbg_GPR().get_value(GPR_t::getReadIndex( 21)).rnum(),false);
      rb.report_regfile(this,8,"GPR",22,Dbg_GPR().get_value(GPR_t::getReadIndex( 22)).rnum(),false);
      rb.report_regfile(this,8,"GPR",23,Dbg_GPR().get_value(GPR_t::getReadIndex( 23)).rnum(),false);
      rb.report_regfile(this,8,"GPR",24,Dbg_GPR().get_value(GPR_t::getReadIndex( 24)).rnum(),false);
      rb.report_regfile(this,8,"GPR",25,Dbg_GPR().get_value(GPR_t::getReadIndex( 25)).rnum(),false);
      rb.report_regfile(this,8,"GPR",26,Dbg_GPR().get_value(GPR_t::getReadIndex( 26)).rnum(),false);
      rb.report_regfile(this,8,"GPR",27,Dbg_GPR().get_value(GPR_t::getReadIndex( 27)).rnum(),false);
      rb.report_regfile(this,8,"GPR",28,Dbg_GPR().get_value(GPR_t::getReadIndex( 28)).rnum(),false);
      rb.report_regfile(this,8,"GPR",29,Dbg_GPR().get_value(GPR_t::getReadIndex( 29)).rnum(),false);
      rb.report_regfile(this,8,"GPR",30,Dbg_GPR().get_value(GPR_t::getReadIndex( 30)).rnum(),false);
      rb.report_regfile(this,8,"GPR",31,Dbg_GPR().get_value(GPR_t::getReadIndex( 31)).rnum(),false);
    }
    
    #endif // ADL_HEADER
    
    static RegInfos initRegInfo()
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      RegInfos tmp;
      tmp.push_back(RegInfo());
      tmp.back()._id = 0;
      tmp.back()._name = "CIA";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back()._attrs.push_back(std::make_pair(0,Attrs::Params()));
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 1;
      tmp.back()._name = "CR";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      int _sym871 = 1;
      tmp.back()._attrs.push_back(std::make_pair(10,Attrs::Params(_sym871)));
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 2;
      tmp.back()._name = "CTR";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      int _sym872 = 2;
      tmp.back()._attrs.push_back(std::make_pair(10,Attrs::Params(_sym872)));
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 3;
      tmp.back()._name = "HID0";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back()._attrs.push_back(std::make_pair(9,Attrs::Params()));
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 4;
      tmp.back()._name = "HID1";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back()._attrs.push_back(std::make_pair(9,Attrs::Params()));
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 5;
      tmp.back()._name = "LR";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      int _sym875 = 3;
      tmp.back()._attrs.push_back(std::make_pair(10,Attrs::Params(_sym875)));
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 6;
      tmp.back()._name = "NIA";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back()._attrs.push_back(std::make_pair(1,Attrs::Params()));
      int _sym877 = 4;
      tmp.back()._attrs.push_back(std::make_pair(10,Attrs::Params(_sym877)));
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 7;
      tmp.back()._name = "XER";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      int _sym878 = 5;
      tmp.back()._attrs.push_back(std::make_pair(10,Attrs::Params(_sym878)));
      tmp.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "CA";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(2,2));
      tmp.back()._fields.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "OV";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(1,1));
      tmp.back()._fields.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "SO";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(0,0));
      tmp.back()._fields.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 8;
      tmp.back()._name = "GPR";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 32;
      int _sym879 = 0;
      tmp.back()._attrs.push_back(std::make_pair(10,Attrs::Params(_sym879)));
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 9;
      tmp.back()._name = "SPR";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 1;
      tmp.back()._width = 32;
      tmp.back()._size = 1024;
      tmp.back().finalize();
      sort_info(tmp);
      return tmp;
    }
    #endif // ADL_HEADER
    
    static const RegInfos &baseRegInfo() {
      static RegInfos info = initRegInfo();
      return info;
    }
    
    virtual const RegInfos &getRegInfo() const {
      return baseRegInfo();
    }
    
    const DbgRegItem *getDefaultRegMap()
    {
      static DbgRegItem dbg_reg_items[38] = {
        { "GPR0", 32 },
        { "GPR1", 32 },
        { "GPR2", 32 },
        { "GPR3", 32 },
        { "GPR4", 32 },
        { "GPR5", 32 },
        { "GPR6", 32 },
        { "GPR7", 32 },
        { "GPR8", 32 },
        { "GPR9", 32 },
        { "GPR10", 32 },
        { "GPR11", 32 },
        { "GPR12", 32 },
        { "GPR13", 32 },
        { "GPR14", 32 },
        { "GPR15", 32 },
        { "GPR16", 32 },
        { "GPR17", 32 },
        { "GPR18", 32 },
        { "GPR19", 32 },
        { "GPR20", 32 },
        { "GPR21", 32 },
        { "GPR22", 32 },
        { "GPR23", 32 },
        { "GPR24", 32 },
        { "GPR25", 32 },
        { "GPR26", 32 },
        { "GPR27", 32 },
        { "GPR28", 32 },
        { "GPR29", 32 },
        { "GPR30", 32 },
        { "GPR31", 32 },
        { "CR", 32 },
        { "CTR", 32 },
        { "LR", 32 },
        { "NIA", 32 },
        { "XER", 32 },
        { 0, 0 }
      };
      return dbg_reg_items;
    }
    bool logReg(unsigned rindex,unsigned index) const
    {
      REGTYPE_t value;
      if (!getReg(rindex,index,value)) return false;
      const RegInfo &ri = getRegInfo()[rindex];
      if (ri.is_regfile()) {
        _logger->log_regfile_ext_write(rindex,ri._name.c_str(),index,value);
        _logger->log_regfile_write(rindex,ri._name.c_str(),index,value);
      } else {
        _logger->log_reg_ext_write(rindex,ri._name.c_str(),value);
        _logger->log_reg_write(rindex,ri._name.c_str(),value);
      }
      return true;
    }
    
    struct ResetRegLogger : public ReportBase {
      virtual void report_reg (const IssNode *n,unsigned id,const char *name,REGTYPE value,bool shared) {
        n->logger().log_reg_write(id,name,value);
      };
      virtual void report_regfile (const IssNode *n,unsigned id,const char *name,int index,REGTYPE value,bool shared) {
        n->logger().log_regfile_write(id,name,index,value);
      };
    };
    
    virtual void resetRegs(bool por_reset = true,bool log = false)
    {
      if (por_reset) {
        CIA_reset_value();
        CR_reset_value();
        CTR_reset_value();
        HID0_reset_value();
        HID1_reset_value();
        LR_reset_value();
        NIA_reset_value();
        XER_reset_value();
      }
      if (por_reset) {
        GPR_reset_value();
      }
      if (Tracing && log && (_trace_mode & ttRegWrite)) {
        ResetRegLogger rl;
        showRegs(rl);
      }
    }
    
    virtual void setProgramCounter(addr_t a)
    {
      Dbg_NIA().set_value(a);
    }
    
    virtual addr_t getProgramCounter() const 
    {
      return Dbg_NIA().get_value().uint64();
    }
    
    virtual addr_t getCurrentInstrAddr() const 
    {
      return Dbg_CIA().get_value().uint64();
    }
    
    void checkWatches() {
    }
    
    //
    // Disassembler helpers
    //
    
    static const char* SPR_disasm(unsigned x) {
      switch (x) {
        case 1: return "XER";
        case 8: return "LR";
        case 9: return "CTR";
        case 50: return "HID0";
        case 51: return "HID1";
        default: return "reserved";
      }
    }
    
    //
    // Instruction field getter functions.
    //
    
    inline static bits<1> get_AA(unsigned x0 ATTRIBUTE_UNUSED) {
      return ((x0&0x2)>>1);
      
    }
    
    inline static bits<14> get_BD(unsigned x0 ATTRIBUTE_UNUSED) {
      return ((x0&0xfffc)>>2);
      
    }
    
    inline static bits<3> get_BF(unsigned x0 ATTRIBUTE_UNUSED) {
      return ((x0&0x3800000)>>23);
      
    }
    
    inline static bits<5> get_BI(unsigned x0 ATTRIBUTE_UNUSED) {
      return ((x0&0x1f0000)>>16);
      
    }
    
    inline static bits<5> get_BO(unsigned x0 ATTRIBUTE_UNUSED) {
      return ((x0&0x3e00000)>>21);
      
    }
    
    inline static bits<16> get_D(unsigned x0 ATTRIBUTE_UNUSED) {
      return ((x0&0xffff));
      
    }
    
    inline static bits<1> get_L(unsigned x0 ATTRIBUTE_UNUSED) {
      return ((x0&0x200000)>>21);
      
    }
    
    inline static bits<24> get_LI(unsigned x0 ATTRIBUTE_UNUSED) {
      return ((x0&0x3fffffc)>>2);
      
    }
    
    inline static bits<1> get_LK(unsigned x0 ATTRIBUTE_UNUSED) {
      return ((x0&0x1));
      
    }
    
    inline static bits<5> get_MB(unsigned x0 ATTRIBUTE_UNUSED) {
      return ((x0&0x7c0)>>6);
      
    }
    
    inline static bits<5> get_ME(unsigned x0 ATTRIBUTE_UNUSED) {
      return ((x0&0x3e)>>1);
      
    }
    
    inline static bits<6> get_OPCD(unsigned x0 ATTRIBUTE_UNUSED) {
      return ((x0&0xfc000000)>>26);
      
    }
    
    inline static bits<5> get_RA(unsigned x0 ATTRIBUTE_UNUSED) {
      return ((x0&0x1f0000)>>16);
      
    }
    
    inline static bits<5> get_RB(unsigned x0 ATTRIBUTE_UNUSED) {
      return ((x0&0xf800)>>11);
      
    }
    
    inline static bits<1> get_RC(unsigned x0 ATTRIBUTE_UNUSED) {
      return ((x0&0x1));
      
    }
    
    inline static bits<5> get_RS(unsigned x0 ATTRIBUTE_UNUSED) {
      return ((x0&0x3e00000)>>21);
      
    }
    
    inline static bits<5> get_RT(unsigned x0 ATTRIBUTE_UNUSED) {
      return ((x0&0x3e00000)>>21);
      
    }
    
    inline static bits<5> get_SH(unsigned x0 ATTRIBUTE_UNUSED) {
      return ((x0&0xf800)>>11);
      
    }
    
    inline static bits<16> get_SI(unsigned x0 ATTRIBUTE_UNUSED) {
      return ((x0&0xffff));
      
    }
    
    inline static bits<10> get_SPRN(unsigned x0 ATTRIBUTE_UNUSED) {
      return ((x0&0xf800)>>6) | ((x0&0x1f0000)>>16);
      
    }
    
    inline static bits<16> get_UI(unsigned x0 ATTRIBUTE_UNUSED) {
      return ((x0&0xffff));
      
    }
    
    inline static bits<10> get_XO(unsigned x0 ATTRIBUTE_UNUSED) {
      return ((x0&0x7fe)>>1);
      
    }
    
    inline static bits<1> get_Y(unsigned x0 ATTRIBUTE_UNUSED) {
      return ((x0&0x200000)>>21);
      
    }
    
    
    //
    // Instruction field clearer functions.
    //
    
    inline static void clear_AA (unsigned *x) {
        x[0] = x[0] & (~0x2);
      
    }
    
    inline static void clear_BD (unsigned *x) {
        x[0] = x[0] & (~0xfffc);
      
    }
    
    inline static void clear_BF (unsigned *x) {
        x[0] = x[0] & (~0x3800000);
      
    }
    
    inline static void clear_BI (unsigned *x) {
        x[0] = x[0] & (~0x1f0000);
      
    }
    
    inline static void clear_BO (unsigned *x) {
        x[0] = x[0] & (~0x3e00000);
      
    }
    
    inline static void clear_D (unsigned *x) {
        x[0] = x[0] & (~0xffff);
      
    }
    
    inline static void clear_L (unsigned *x) {
        x[0] = x[0] & (~0x200000);
      
    }
    
    inline static void clear_LI (unsigned *x) {
        x[0] = x[0] & (~0x3fffffc);
      
    }
    
    inline static void clear_LK (unsigned *x) {
        x[0] = x[0] & (~0x1);
      
    }
    
    inline static void clear_MB (unsigned *x) {
        x[0] = x[0] & (~0x7c0);
      
    }
    
    inline static void clear_ME (unsigned *x) {
        x[0] = x[0] & (~0x3e);
      
    }
    
    inline static void clear_OPCD (unsigned *x) {
        x[0] = x[0] & (~0xfc000000);
      
    }
    
    inline static void clear_RA (unsigned *x) {
        x[0] = x[0] & (~0x1f0000);
      
    }
    
    inline static void clear_RB (unsigned *x) {
        x[0] = x[0] & (~0xf800);
      
    }
    
    inline static void clear_RC (unsigned *x) {
        x[0] = x[0] & (~0x1);
      
    }
    
    inline static void clear_RS (unsigned *x) {
        x[0] = x[0] & (~0x3e00000);
      
    }
    
    inline static void clear_RT (unsigned *x) {
        x[0] = x[0] & (~0x3e00000);
      
    }
    
    inline static void clear_SH (unsigned *x) {
        x[0] = x[0] & (~0xf800);
      
    }
    
    inline static void clear_SI (unsigned *x) {
        x[0] = x[0] & (~0xffff);
      
    }
    
    inline static void clear_SPRN (unsigned *x) {
        x[0] = x[0] & (~0x1ff800);
      
    }
    
    inline static void clear_UI (unsigned *x) {
        x[0] = x[0] & (~0xffff);
      
    }
    
    inline static void clear_XO (unsigned *x) {
        x[0] = x[0] & (~0x7fe);
      
    }
    
    inline static void clear_Y (unsigned *x) {
        x[0] = x[0] & (~0x200000);
      
    }
    
    
    struct DcDecodeItem;
    struct DcBasicBlock;
    
    typedef void (__fastcall *DcVoidFunc)(P*,DcDecodeItem *) FASTCALL;
    typedef std::pair<DcVoidFunc,DcDecodeItem *> DcDecodePair;
    typedef DcVoidFunc (__fastcall *DcDecodeFunc)(P*,DcDecodeItem *) FASTCALL;
    typedef DcVoidFunc DcDecodeReturn;
    
    struct DcDecodeItem : public TDcDecodeItem<P,DcBasicBlock,uint32_t,7>
    {
      typedef TDcDecodeItem<P,DcBasicBlock,uint32_t,7> Base;
      DcDecodeItem() {};
      DcDecodeItem(DecodeFunc f) : TDcDecodeItem<P,DcBasicBlock,uint32_t,7>(f) {};
      using Base::set_arg;
      template <size_t Nb> void set_arg(unsigned i,const bits<Nb> &x)  { _args[i] = x.uint32(); };
      template <size_t Nb> void set_arg(unsigned i,const sbits<Nb> &x) { _args[i] = x.uint32(); };
    };
    struct DcBasicBlock : public TDcBasicBlock<DcDecodeItem>
    {
      DcBasicBlock() {};
      DcBasicBlock(addr_t ra) : TDcBasicBlock<DcDecodeItem>(ra) {};
    };
    
    static DcDecodeReturn __fastcall dc_decode_cache_branch_trampoline(P *core,DcDecodeItem *di) FASTCALL {
      core->_dc_decode_cache_next = 0;
      if (core->_td_counter >= core->_td_threshold) { return (DcVoidFunc)JitRetCode;
         };
      return (DcVoidFunc)JitNopCode;
    }
    
    static DcDecodeReturn __fastcall dc_decode_cache_breakpoint_trampoline(P *core,DcDecodeItem *di) FASTCALL {
      core->_dc_decode_cache_next = 0;
      return (DcVoidFunc)JitBreakPtCode;
    }
    
    struct DcDecodeCache : public TDcDecodeCache<DcDecodeItem,DcBasicBlock> {
      typedef TDcDecodeCache<DcDecodeItem,DcBasicBlock> Base;
      typedef Base::DecodeElm DecodeElm;
      typedef Base::BlockType BlockType;
      DcDecodeCache(DecodeElm *br_stub,DecodeElm *brkpt_stub,size_t size) : Base(br_stub,brkpt_stub,size) {}
      
    };
    
    struct DcTranslate {
      
      #ifdef ADL_HEADER
      ;
      #else
      bool full(P &core) { return core.decode_cache_full(); };
      void operator()(P &core,addr_t ea,DcBasicBlock *bb) {
        core.translate_dc(ea,bb);
      }
      #endif // ADL_HEADER
      
    };
    typedef TBasicBlockHash<P,DcBasicBlock,DcDecodeCache,DcTranslate,2,10> DcBasicBlockHash;
    
    
    // Disassembler for illops.
    static int disasm_illop(std::ostream &os,addr_t memaddr, const unsigned *insn)
    {
      os << ".long 0x" << std::setfill('0') << std::setw(8) << std::hex << *insn << std::dec;
      return 4;
    }
    
    static DcDecodeReturn __fastcall dc_decode_cache_decode_miss(P *core,DcDecodeItem *di) FASTCALL {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      core->NIA_write_simple(core->CIA_read() + 4);
      addr_t ea = di->_args[0];
      ea = (ea << 32) | di->_args[1];
      core->decode_miss_handler(ea,di->_args[2]);
      core->_dc_decode_cache_next = 0;
      return (DcVoidFunc)JitNopCode;
    }
    
    // Decode-cache decode-miss handler.
    inline void _sym5(addr_t ea,uint32_t instr)
    {
      DcDecodeItem *di = _dc_decode_cache.add_item();
      di->set_func(dc_decode_cache_decode_miss);
      di->_args[0] = (ea >> 32);
      di->_args[1] = ea;
      di->_args[2] = instr;
    }
    
    // Disassembler for "add"
    static int instr_disasm_add(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>RB ATTRIBUTE_UNUSED = get_RB(tmpinsn[0]);
      // Instruction add
      os << "add ";
      os << std::dec << "r" << RT;
      os << ',';
      os << std::dec << "r" << RA;
      os << ',';
      os << std::dec << "r" << RB;
      return 4;
    }
    
    // Instruction "add"
     void instr_handler_add(bits<5> RT, bits<5> RA, bits<5> RB)
     {
      {
             GPR_write( to_uint32( RT ) , (  GPR_read( to_uint32( RA ) ) + GPR_read( to_uint32( RB ) )  ) ) ;
      }
    }
    
    // Decode-cache handler for instruction "add".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_6(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"add",core->instr_disasm_add,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_grp1();
      core->instr_handler_add(dci->_args[1], dci->_args[2], dci->_args[3]);
      core->time_tag_finish_grp1("add");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "add"
    inline void _sym33(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RT, bits<5> RA, bits<5> RB)
    {
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_6);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RT);
      di->set_arg(2,RA);
      di->set_arg(3,RB);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "addi"
    static int instr_disasm_addi(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      sbits<32>SI ATTRIBUTE_UNUSED = get_SI(tmpinsn[0]).signExtend<32>();
      if ((RA == 0x0)) {
        // Instruction li
        os << "li ";
        os << std::dec << "r" << RT;
        os << ',';
        os << std::dec << SI;
        return 4;
      }
      // Instruction addi
      os << "addi ";
      os << std::dec << "r" << RT;
      os << ',';
      os << std::dec << "r" << RA;
      os << ',';
      os << std::dec << SI;
      return 4;
    }
    
    // Wrapper for instruction "addi"
    inline void  instr_handler_addi(bits<5> RT, bits<5> RA, bits<16> SI_p)
    {
      sbits<32> SI = SI_p.signExtend<32>();
      _sym35(RT, RA, SI);
    }
    
    // Instruction "addi"
     void _sym35(bits<5> RT, bits<5> RA, bits<32> SI)
     {
      {
            if ( RA == 0 ) {
                   GPR_write( to_uint32( RT ) , (  SI  ) ) ;
              } else  GPR_write( to_uint32( RT ) , (  GPR_read( to_uint32( RA ) ) + SI  ) ) ;
      }
    }
    
    // Decode-cache handler for instruction "addi".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_9(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"addi",core->instr_disasm_addi,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_grp1();
      core->_sym35(dci->_args[1], dci->_args[2], dci->_args[3]);
      core->time_tag_finish_grp1("addi");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "addi"
    inline void _sym39(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RT, bits<5> RA, bits<16> SI_p)
    {
      sbits<32> SI = SI_p.signExtend<32>();
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_9);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RT);
      di->set_arg(2,RA);
      di->set_arg(3,SI);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "addic"
    static int instr_disasm_addic(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      sbits<32>SI ATTRIBUTE_UNUSED = get_SI(tmpinsn[0]).signExtend<32>();
      // Instruction addic
      os << "addic ";
      os << std::dec << "r" << RT;
      os << ',';
      os << std::dec << "r" << RA;
      os << ',';
      os << std::dec << SI;
      return 4;
    }
    
    // Wrapper for instruction "addic"
    inline void  instr_handler_addic(bits<5> RT, bits<5> RA, bits<16> SI_p)
    {
      sbits<32> SI = SI_p.signExtend<32>();
      _sym41(RT, RA, SI);
    }
    
    // Instruction "addic"
     void _sym41(bits<5> RT, bits<5> RA, bits<32> SI)
     {
      {
             AdlTypeof( Carry ( GPR_read( to_uint32( RA ) ) , SI , 0 ) ) carry = Carry ( GPR_read( to_uint32( RA ) ) , SI , 0 ) ;
             GPR_write( to_uint32( RT ) , (  GPR_read( to_uint32( RA ) ) + SI  ) ) ;
            setXerField ( false , false , true , carry ) ;
      }
    }
    
    // Decode-cache handler for instruction "addic".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_12(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"addic",core->instr_disasm_addic,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_grp1();
      core->_sym41(dci->_args[1], dci->_args[2], dci->_args[3]);
      core->time_tag_finish_grp1("addic");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "addic"
    inline void _sym45(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RT, bits<5> RA, bits<16> SI_p)
    {
      sbits<32> SI = SI_p.signExtend<32>();
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_12);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RT);
      di->set_arg(2,RA);
      di->set_arg(3,SI);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "addic."
    static int instr_disasm_addic_(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      sbits<32>SI ATTRIBUTE_UNUSED = get_SI(tmpinsn[0]).signExtend<32>();
      // Instruction addic.
      os << "addic. ";
      os << std::dec << "r" << RT;
      os << ',';
      os << std::dec << "r" << RA;
      os << ',';
      os << std::dec << SI;
      return 4;
    }
    
    // Wrapper for instruction "addic."
    inline void  instr_handler_addic_(bits<5> RT, bits<5> RA, bits<16> SI_p)
    {
      sbits<32> SI = SI_p.signExtend<32>();
      _sym47(RT, RA, SI);
    }
    
    // Instruction "addic."
     void _sym47(bits<5> RT, bits<5> RA, bits<32> SI)
     {
      {
             AdlTypeof( Carry ( GPR_read( to_uint32( RA ) ) , SI , 0 ) ) carry = Carry ( GPR_read( to_uint32( RA ) ) , SI , 0 ) ;
             GPR_write( to_uint32( RT ) , (  GPR_read( to_uint32( RA ) ) + SI  ) ) ;
            setCrField ( 0 , GPR_read( to_uint32( RT ) ) , 0 ) ;
            setXerField ( false , false , true , carry ) ;
      }
    }
    
    // Decode-cache handler for instruction "addic.".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_15(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"addic.",core->instr_disasm_addic_,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_grp1();
      core->_sym47(dci->_args[1], dci->_args[2], dci->_args[3]);
      core->time_tag_finish_grp1("addic.");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "addic."
    inline void _sym51(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RT, bits<5> RA, bits<16> SI_p)
    {
      sbits<32> SI = SI_p.signExtend<32>();
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_15);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RT);
      di->set_arg(2,RA);
      di->set_arg(3,SI);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "addis"
    static int instr_disasm_addis(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      sbits<32>SI ATTRIBUTE_UNUSED = get_SI(tmpinsn[0]).signExtend<32>();
      if ((RA == 0x0)) {
        // Instruction lis
        os << "lis ";
        os << std::dec << "r" << RT;
        os << ',';
        os << std::dec << SI;
        return 4;
      }
      // Instruction addis
      os << "addis ";
      os << std::dec << "r" << RT;
      os << ',';
      os << std::dec << "r" << RA;
      os << ',';
      os << std::dec << SI;
      return 4;
    }
    
    // Wrapper for instruction "addis"
    inline void  instr_handler_addis(bits<5> RT, bits<5> RA, bits<16> SI_p)
    {
      sbits<32> SI = SI_p.signExtend<32>();
      _sym53(RT, RA, SI);
    }
    
    // Instruction "addis"
     void _sym53(bits<5> RT, bits<5> RA, bits<32> SI)
     {
      {
            if ( RA == 0 ) {
                   GPR_write( to_uint32( RT ) , (  SI << 16  ) ) ;
              } else {
                   GPR_write( to_uint32( RT ) , (  GPR_read( to_uint32( RA ) ) + ( SI << 16 )  ) ) ;
              }
      }
    }
    
    // Decode-cache handler for instruction "addis".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_18(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"addis",core->instr_disasm_addis,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_grp1();
      core->_sym53(dci->_args[1], dci->_args[2], dci->_args[3]);
      core->time_tag_finish_grp1("addis");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "addis"
    inline void _sym57(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RT, bits<5> RA, bits<16> SI_p)
    {
      sbits<32> SI = SI_p.signExtend<32>();
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_18);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RT);
      di->set_arg(2,RA);
      di->set_arg(3,SI);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "addme"
    static int instr_disasm_addme(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      // Instruction addme
      os << "addme ";
      os << std::dec << "r" << RT;
      os << ',';
      os << std::dec << "r" << RA;
      return 4;
    }
    
    // Instruction "addme"
     void instr_handler_addme(bits<5> RT, bits<5> RA)
     {
      {
             AdlTypeof( Carry ( GPR_read( to_uint32( RA ) ) , 0xffffffff , XER_CA_read() ) ) carry = Carry ( GPR_read( to_uint32( RA ) ) , 0xffffffff , XER_CA_read() ) ;
             GPR_write( to_uint32( RT ) , (  GPR_read( to_uint32( RA ) ) + 0xffffffff + XER_CA_read()  ) ) ;
            setXerField ( false , false , true , carry ) ;
      }
    }
    
    // Decode-cache handler for instruction "addme".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_21(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"addme",core->instr_disasm_addme,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_grp1();
      core->instr_handler_addme(dci->_args[1], dci->_args[2]);
      core->time_tag_finish_grp1("addme");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "addme"
    inline void _sym63(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RT, bits<5> RA)
    {
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_21);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RT);
      di->set_arg(2,RA);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "addme."
    static int instr_disasm_addme_(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      // Instruction addme.
      os << "addme. ";
      os << std::dec << "r" << RT;
      os << ',';
      os << std::dec << "r" << RA;
      return 4;
    }
    
    // Instruction "addme."
     void instr_handler_addme_(bits<5> RT, bits<5> RA)
     {
      {
             AdlTypeof( Carry ( GPR_read( to_uint32( RA ) ) , 0xffffffff , XER_CA_read() ) ) carry = Carry ( GPR_read( to_uint32( RA ) ) , 0xffffffff , XER_CA_read() ) ;
             GPR_write( to_uint32( RT ) , (  GPR_read( to_uint32( RA ) ) + 0xffffffff + XER_CA_read()  ) ) ;
            setXerField ( false , false , true , carry ) ;
            setCrField ( 0 , GPR_read( to_uint32( RT ) ) , 0 ) ;
      }
    }
    
    // Decode-cache handler for instruction "addme.".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_24(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"addme.",core->instr_disasm_addme_,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_grp1();
      core->instr_handler_addme_(dci->_args[1], dci->_args[2]);
      core->time_tag_finish_grp1("addme.");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "addme."
    inline void _sym69(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RT, bits<5> RA)
    {
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_24);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RT);
      di->set_arg(2,RA);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "addmeo"
    static int instr_disasm_addmeo(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      // Instruction addmeo
      os << "addmeo ";
      os << std::dec << "r" << RT;
      os << ',';
      os << std::dec << "r" << RA;
      return 4;
    }
    
    // Instruction "addmeo"
     void instr_handler_addmeo(bits<5> RT, bits<5> RA)
     {
      {
             AdlTypeof( Carry ( GPR_read( to_uint32( RA ) ) , 0xffffffff , XER_CA_read() ) ) carry = Carry ( GPR_read( to_uint32( RA ) ) , 0xffffffff , XER_CA_read() ) ;
             GPR_write( to_uint32( RT ) , (  GPR_read( to_uint32( RA ) ) + 0xffffffff + XER_CA_read()  ) ) ;
            setXerField ( true , true , true , carry ) ;
      }
    }
    
    // Decode-cache handler for instruction "addmeo".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_27(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"addmeo",core->instr_disasm_addmeo,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_grp1();
      core->instr_handler_addmeo(dci->_args[1], dci->_args[2]);
      core->time_tag_finish_grp1("addmeo");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "addmeo"
    inline void _sym75(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RT, bits<5> RA)
    {
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_27);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RT);
      di->set_arg(2,RA);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "addo"
    static int instr_disasm_addo(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>RB ATTRIBUTE_UNUSED = get_RB(tmpinsn[0]);
      // Instruction addo
      os << "addo ";
      os << std::dec << "r" << RT;
      os << ',';
      os << std::dec << "r" << RA;
      os << ',';
      os << std::dec << "r" << RB;
      return 4;
    }
    
    // Instruction "addo"
     void instr_handler_addo(bits<5> RT, bits<5> RA, bits<5> RB)
     {
      {
             AdlTypeof( Carry ( GPR_read( to_uint32( RA ) ) , GPR_read( to_uint32( RB ) ) , 0 ) ) carry = Carry ( GPR_read( to_uint32( RA ) ) , GPR_read( to_uint32( RB ) ) , 0 ) ;
            setXerField ( true , true , false , carry ) ;
             GPR_write( to_uint32( RT ) , (  GPR_read( to_uint32( RA ) ) + GPR_read( to_uint32( RB ) )  ) ) ;
      }
    }
    
    // Decode-cache handler for instruction "addo".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_30(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"addo",core->instr_disasm_addo,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_grp1();
      core->instr_handler_addo(dci->_args[1], dci->_args[2], dci->_args[3]);
      core->time_tag_finish_grp1("addo");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "addo"
    inline void _sym81(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RT, bits<5> RA, bits<5> RB)
    {
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_30);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RT);
      di->set_arg(2,RA);
      di->set_arg(3,RB);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "andi."
    static int instr_disasm_andi_(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RS ATTRIBUTE_UNUSED = get_RS(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<16>UI ATTRIBUTE_UNUSED = get_UI(tmpinsn[0]);
      // Instruction andi.
      os << "andi. ";
      os << std::dec << "r" << RS;
      os << ',';
      os << std::dec << "r" << RA;
      os << ',';
      os << std::hex << std::showbase << UI << std::noshowbase;
      return 4;
    }
    
    // Instruction "andi."
     void instr_handler_andi_(bits<5> RS, bits<5> RA, bits<16> UI)
     {
      {
             GPR_write( to_uint32( RA ) , (  GPR_read( to_uint32( RS ) ) & UI  ) ) ;
            setCrField ( 0 , GPR_read( to_uint32( RA ) ) , 0 ) ;
      }
    }
    
    // Decode-cache handler for instruction "andi.".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_33(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"andi.",core->instr_disasm_andi_,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_grp1();
      core->instr_handler_andi_(dci->_args[1], dci->_args[2], dci->_args[3]);
      core->time_tag_finish_grp1("andi.");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "andi."
    inline void _sym87(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RS, bits<5> RA, bits<16> UI)
    {
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_33);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RS);
      di->set_arg(2,RA);
      di->set_arg(3,UI);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "b"
    static int instr_disasm_b(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      sbits<32>LI ATTRIBUTE_UNUSED = (get_LI(tmpinsn[0]).signExtend<32>() << 2);
      // Instruction b
      os << "b ";
      os << "0x" << std::setw(16) << std::setfill('0')  << std::hex << ((memaddr + LI.int32()) & ((1ull << 32)-1));
      return 4;
    }
    
    // Wrapper for instruction "b"
    inline void  instr_handler_b(bits<24> LI_p)
    {
      sbits<32> LI = ((LI_p.signExtend<32>() << 2) + CIA_read());
      _sym89(LI);
    }
    
    // Instruction "b"
     void _sym89(bits<32> LI)
     {
      {
             NIA_write(  LI  )  ;
      }
    }
    
    // Decode-cache handler for instruction "b".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_36(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"b",core->instr_disasm_b,ifBrUncond);
        }
        if (Tracing && (core->trace_mode() & ttBasicBlockEnd)) {
          core->get_logger()->log_basic_block_end(core->CIA_read().uint64());
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_grp1();
      core->_sym89(dci->_args[2]);
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttBranchTaken)) {
          core->get_logger()->log_branch_end(core->CIA_read().uint64());
        }
      });
      core->time_tag_finish_grp1("b");
      if (core->_td_counter >= core->_td_threshold) { 
        core->_dc_decode_cache_next = 0;
        return (DcVoidFunc)JitRetCode;
         };
      DcDecodeItem *bt = dci->_bt_t->bt();
      core->_dc_decode_cache_next = bt;
      return (DcVoidFunc)JitContCode;
    }
    
    // Decode-cache translation function for instruction "b"
    inline void _sym93(DcBasicBlock *bb,addr_t pc,unsigned count, bits<24> LI_p)
    {
      sbits<32> LI = ((LI_p.signExtend<32>() << 2) + pc);
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_36);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,count+1);
      di->set_arg(2,LI);
      _dc_decode_cache.set_br_trg(di,_dc_bb_hash.get_bb_or_stub(LI.uint64(),0,_dc_decode_cache),true);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "bc"
    static int instr_disasm_bc(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<1>Y ATTRIBUTE_UNUSED = get_Y(tmpinsn[0]);
      clear_Y(tmpinsn);
      bits<5>BO ATTRIBUTE_UNUSED = get_BO(tmpinsn[0]);
      bits<5>BI ATTRIBUTE_UNUSED = get_BI(tmpinsn[0]);
      sbits<32>BD ATTRIBUTE_UNUSED = (get_BD(tmpinsn[0]).signExtend<32>() << 2);
      if ((BO == 0xc) && (Y == (( BD < 0 ) ? 0 : 1)) && (BI == 0x0)) {
        // Instruction blt+_
        os << "blt+ ";
        os << "0x" << std::setw(16) << std::setfill('0')  << std::hex << ((memaddr + BD.int32()) & ((1ull << 32)-1));
        return 4;
      }
      if ((BO == 0x4) && (Y == (( BD < 0 ) ? 1 : 0)) && (BI == 0x2)) {
        // Instruction bne-_
        os << "bne- ";
        os << "0x" << std::setw(16) << std::setfill('0')  << std::hex << ((memaddr + BD.int32()) & ((1ull << 32)-1));
        return 4;
      }
      if ((BO == 0xc) && (Y == (( BD < 0 ) ? 1 : 0)) && (BI == 0x0)) {
        // Instruction blt-_
        os << "blt- ";
        os << "0x" << std::setw(16) << std::setfill('0')  << std::hex << ((memaddr + BD.int32()) & ((1ull << 32)-1));
        return 4;
      }
      if ((BO == 0x4) && (Y == (( BD < 0 ) ? 1 : 0)) && (BI == 0x1)) {
        // Instruction ble-_
        os << "ble- ";
        os << "0x" << std::setw(16) << std::setfill('0')  << std::hex << ((memaddr + BD.int32()) & ((1ull << 32)-1));
        return 4;
      }
      if ((BO == 0x4) && (Y == (( BD < 0 ) ? 0 : 1)) && (BI == 0x1)) {
        // Instruction ble+_
        os << "ble+ ";
        os << "0x" << std::setw(16) << std::setfill('0')  << std::hex << ((memaddr + BD.int32()) & ((1ull << 32)-1));
        return 4;
      }
      if ((BO == 0x4) && (Y == (( BD < 0 ) ? 0 : 1)) && (BI == 0x2)) {
        // Instruction bne+_
        os << "bne+ ";
        os << "0x" << std::setw(16) << std::setfill('0')  << std::hex << ((memaddr + BD.int32()) & ((1ull << 32)-1));
        return 4;
      }
      if ((BO == 0xc) && (Y == (( BD < 0 ) ? 1 : 0)) && (BI == 0x1)) {
        // Instruction bgt-_
        os << "bgt- ";
        os << "0x" << std::setw(16) << std::setfill('0')  << std::hex << ((memaddr + BD.int32()) & ((1ull << 32)-1));
        return 4;
      }
      if ((BO == 0xc) && (Y == (( BD < 0 ) ? 0 : 1)) && (BI == 0x1)) {
        // Instruction bgt+_
        os << "bgt+ ";
        os << "0x" << std::setw(16) << std::setfill('0')  << std::hex << ((memaddr + BD.int32()) & ((1ull << 32)-1));
        return 4;
      }
      if ((BO == 0x4) && (Y == (( BD < 0 ) ? 1 : 0)) && (BI == 0x0)) {
        // Instruction bge-_
        os << "bge- ";
        os << "0x" << std::setw(16) << std::setfill('0')  << std::hex << ((memaddr + BD.int32()) & ((1ull << 32)-1));
        return 4;
      }
      if ((BO == 0x4) && (Y == (( BD < 0 ) ? 0 : 1)) && (BI == 0x0)) {
        // Instruction bge+_
        os << "bge+ ";
        os << "0x" << std::setw(16) << std::setfill('0')  << std::hex << ((memaddr + BD.int32()) & ((1ull << 32)-1));
        return 4;
      }
      if ((BO == 0xc) && (Y == (( BD < 0 ) ? 1 : 0)) && (BI == 0x2)) {
        // Instruction beq-_
        os << "beq- ";
        os << "0x" << std::setw(16) << std::setfill('0')  << std::hex << ((memaddr + BD.int32()) & ((1ull << 32)-1));
        return 4;
      }
      if ((BO == 0xc) && (Y == (( BD < 0 ) ? 0 : 1)) && (BI == 0x2)) {
        // Instruction beq+_
        os << "beq+ ";
        os << "0x" << std::setw(16) << std::setfill('0')  << std::hex << ((memaddr + BD.int32()) & ((1ull << 32)-1));
        return 4;
      }
      if ((BO == 0x10) && (Y == (( BD < 0 ) ? 1 : 0)) && (BI == 0x0)) {
        // Instruction bdnz-
        os << "bdnz- ";
        os << "0x" << std::setw(16) << std::setfill('0')  << std::hex << ((memaddr + BD.int32()) & ((1ull << 32)-1));
        return 4;
      }
      if ((BO == 0x10) && (Y == (( BD < 0 ) ? 0 : 1)) && (BI == 0x0)) {
        // Instruction bdnz+
        os << "bdnz+ ";
        os << "0x" << std::setw(16) << std::setfill('0')  << std::hex << ((memaddr + BD.int32()) & ((1ull << 32)-1));
        return 4;
      }
      if ((BO == 0x4) && (BI == 0x2)) {
        // Instruction bne
        os << "bne ";
        os << "0x" << std::setw(16) << std::setfill('0')  << std::hex << ((memaddr + BD.int32()) & ((1ull << 32)-1));
        return 4;
      }
      // Instruction bc
      os << "bc ";
      os << std::hex << std::showbase << BO << std::noshowbase;
      os << ",";
      os << std::hex << std::showbase << BI << std::noshowbase;
      os << ",";
      os << "0x" << std::setw(16) << std::setfill('0')  << std::hex << ((memaddr + BD.int32()) & ((1ull << 32)-1));
      return 4;
    }
    
    // Wrapper for instruction "bc"
    inline void  instr_handler_bc(bits<5> BO, bits<5> BI, bits<14> BD_p)
    {
      sbits<32> BD = ((BD_p.signExtend<32>() << 2) + CIA_read());
      _sym95(BO, BI, BD);
    }
    
    // Instruction "bc"
     bool _sym95(bits<5> BO, bits<5> BI, bits<32> BD)
     {
      bool __branch_taken__ = false;
      {
            if ( ( BO ( 2 ) ) == 0 ) {
                   CTR_write(  CTR_read() - 1  )  ;
              }
             AdlTypeof( ( BO ( 2 ) != 0 ) || ( ( ( CTR_read() != 0 ) ^ BO ( 3 ) ) != 0 ) ) ctr_ok = ( BO ( 2 ) != 0 ) || ( ( ( CTR_read() != 0 ) ^ BO ( 3 ) ) != 0 ) ;
             AdlTypeof( ( BO ( 0 ) != 0 ) || ( CR_read() ( BI ) == BO ( 1 ) ) ) cond_ok = ( BO ( 0 ) != 0 ) || ( CR_read() ( BI ) == BO ( 1 ) ) ;
            if ( ctr_ok && cond_ok ) {
                  {   NIA_write(  BD  )   ; __branch_taken__ = true ; } ;
              }
      }
      return __branch_taken__;
    }
    
    // Decode-cache handler for instruction "bc".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_39(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"bc",core->instr_disasm_bc,ifBrCond);
        }
        if (Tracing && (core->trace_mode() & ttBasicBlockEnd)) {
          core->get_logger()->log_basic_block_end(core->CIA_read().uint64());
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_grp1();
      bool rc = core->_sym95(dci->_args[2], dci->_args[3], dci->_args[4]);
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttBranchTaken)) {
          core->get_logger()->log_branch_end(core->CIA_read().uint64());
        }
      });
      core->time_tag_finish_grp1("bc");
      if (core->_td_counter >= core->_td_threshold) { 
        core->_dc_decode_cache_next = 0;
        return (DcVoidFunc)JitRetCode;
         };
      if (rc) {
        DcDecodeItem *bt = dci->_bt_t->bt();
        core->_dc_decode_cache_next = bt;
        return (DcVoidFunc)JitContCode;
      } else {
        DcDecodeItem *bt = dci->_bt_f->bt();
        core->_dc_decode_cache_next = bt;
        return (DcVoidFunc)JitContCode;
      }
    }
    
    // Decode-cache translation function for instruction "bc"
    inline void _sym99(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> BO, bits<5> BI, bits<14> BD_p)
    {
      sbits<32> BD = ((BD_p.signExtend<32>() << 2) + pc);
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_39);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,count+1);
      di->set_arg(2,BO);
      di->set_arg(3,BI);
      di->set_arg(4,BD);
      _dc_decode_cache.set_br_trg(di,_dc_bb_hash.get_bb_or_stub(BD.uint64(),0,_dc_decode_cache),true);
      _dc_decode_cache.set_br_trg(di,_dc_bb_hash.get_bb_or_stub(pc+4,0,_dc_decode_cache),false);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "bcl"
    static int instr_disasm_bcl(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<1>Y ATTRIBUTE_UNUSED = get_Y(tmpinsn[0]);
      clear_Y(tmpinsn);
      bits<5>BO ATTRIBUTE_UNUSED = get_BO(tmpinsn[0]);
      bits<5>BI ATTRIBUTE_UNUSED = get_BI(tmpinsn[0]);
      sbits<32>BD ATTRIBUTE_UNUSED = (get_BD(tmpinsn[0]).signExtend<32>() << 2);
      // Instruction bcl
      os << "bcl ";
      os << std::hex << std::showbase << BO << std::noshowbase;
      os << ",";
      os << std::hex << std::showbase << BI << std::noshowbase;
      os << ",";
      os << "0x" << std::setw(16) << std::setfill('0')  << std::hex << ((memaddr + BD.int32()) & ((1ull << 32)-1));
      return 4;
    }
    
    // Wrapper for instruction "bcl"
    inline void  instr_handler_bcl(bits<5> BO, bits<5> BI, bits<14> BD_p)
    {
      sbits<32> BD = ((BD_p.signExtend<32>() << 2) + CIA_read());
      _sym101(BO, BI, BD);
    }
    
    // Instruction "bcl"
     bool _sym101(bits<5> BO, bits<5> BI, bits<32> BD)
     {
      bool __branch_taken__ = false;
      {
            if ( ( BO ( 2 ) ) == 0 ) {
                   CTR_write(  CTR_read() - 1  )  ;
              }
             AdlTypeof( ( BO ( 2 ) != 0 ) || ( ( ( CTR_read() != 0 ) ^ BO ( 3 ) ) != 0 ) ) ctr_ok = ( BO ( 2 ) != 0 ) || ( ( ( CTR_read() != 0 ) ^ BO ( 3 ) ) != 0 ) ;
             AdlTypeof( ( BO ( 0 ) != 0 ) || ( CR_read() ( BI ) == BO ( 1 ) ) ) cond_ok = ( BO ( 0 ) != 0 ) || ( CR_read() ( BI ) == BO ( 1 ) ) ;
             LR_write(  CIA_read() + 4  )  ;
            if ( ctr_ok && cond_ok ) {
                  {   NIA_write(  BD  )   ; __branch_taken__ = true ; } ;
              }
      }
      return __branch_taken__;
    }
    
    // Decode-cache handler for instruction "bcl".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_42(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"bcl",core->instr_disasm_bcl,ifBrCond);
        }
        if (Tracing && (core->trace_mode() & ttBasicBlockEnd)) {
          core->get_logger()->log_basic_block_end(core->CIA_read().uint64());
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_grp1();
      bool rc = core->_sym101(dci->_args[2], dci->_args[3], dci->_args[4]);
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttBranchTaken)) {
          core->get_logger()->log_branch_end(core->CIA_read().uint64());
        }
      });
      core->time_tag_finish_grp1("bcl");
      if (core->_td_counter >= core->_td_threshold) { 
        core->_dc_decode_cache_next = 0;
        return (DcVoidFunc)JitRetCode;
         };
      if (rc) {
        DcDecodeItem *bt = dci->_bt_t->bt();
        core->_dc_decode_cache_next = bt;
        return (DcVoidFunc)JitContCode;
      } else {
        DcDecodeItem *bt = dci->_bt_f->bt();
        core->_dc_decode_cache_next = bt;
        return (DcVoidFunc)JitContCode;
      }
    }
    
    // Decode-cache translation function for instruction "bcl"
    inline void _sym105(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> BO, bits<5> BI, bits<14> BD_p)
    {
      sbits<32> BD = ((BD_p.signExtend<32>() << 2) + pc);
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_42);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,count+1);
      di->set_arg(2,BO);
      di->set_arg(3,BI);
      di->set_arg(4,BD);
      _dc_decode_cache.set_br_trg(di,_dc_bb_hash.get_bb_or_stub(BD.uint64(),0,_dc_decode_cache),true);
      _dc_decode_cache.set_br_trg(di,_dc_bb_hash.get_bb_or_stub(pc+4,0,_dc_decode_cache),false);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "bclr"
    static int instr_disasm_bclr(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<1>Y ATTRIBUTE_UNUSED = get_Y(tmpinsn[0]);
      clear_Y(tmpinsn);
      bits<5>BO ATTRIBUTE_UNUSED = get_BO(tmpinsn[0]);
      bits<5>BI ATTRIBUTE_UNUSED = get_BI(tmpinsn[0]);
      if ((BO == 0x14) && (BI == 0x0)) {
        // Instruction blr
        os << "blr ";
        return 4;
      }
      // Instruction bclr
      os << "bclr ";
      os << std::hex << std::showbase << BO << std::noshowbase;
      os << ",";
      os << std::hex << std::showbase << BI << std::noshowbase;
      return 4;
    }
    
    // Instruction "bclr"
     bool instr_handler_bclr(bits<5> BO, bits<5> BI)
     {
      bool __branch_taken__ = false;
      {
            if ( BO ( 2 ) == 0 ) {
                   CTR_write(  CTR_read() - 1  )  ;
              }
             AdlTypeof( ( BO ( 2 ) != 0 ) || ( ( ( CTR_read() != 0 ) ^ BO ( 3 ) ) != 0 ) ) ctr_ok = ( BO ( 2 ) != 0 ) || ( ( ( CTR_read() != 0 ) ^ BO ( 3 ) ) != 0 ) ;
             AdlTypeof( ( BO ( 0 ) != 0 ) || ( CR_read() ( BI ) == BO ( 1 ) ) ) cond_ok = ( BO ( 0 ) != 0 ) || ( CR_read() ( BI ) == BO ( 1 ) ) ;
            if ( ctr_ok && cond_ok ) {
                  {   NIA_write(  LR_read() & ~ 0x3ULL  )   ; __branch_taken__ = true ; } ;
              }
      }
      return __branch_taken__;
    }
    
    // Decode-cache handler for instruction "bclr".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_45(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"bclr",core->instr_disasm_bclr,ifBrUncond);
        }
        if (Tracing && (core->trace_mode() & ttBasicBlockEnd)) {
          core->get_logger()->log_basic_block_end(core->CIA_read().uint64());
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_grp1();
      core->instr_handler_bclr(dci->_args[2], dci->_args[3]);
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttBranchTaken)) {
          core->get_logger()->log_branch_end(core->CIA_read().uint64());
        }
      });
      core->time_tag_finish_grp1("bclr");
      core->_dc_decode_cache_next = 0;
      if (core->_td_counter >= core->_td_threshold) { return (DcVoidFunc)JitRetCode; };
      return (DcVoidFunc)JitNopCode;
    }
    
    // Decode-cache translation function for instruction "bclr"
    inline void _sym111(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> BO, bits<5> BI)
    {
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_45);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,count+1);
      di->set_arg(2,BO);
      di->set_arg(3,BI);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "bclrl"
    static int instr_disasm_bclrl(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<1>Y ATTRIBUTE_UNUSED = get_Y(tmpinsn[0]);
      clear_Y(tmpinsn);
      bits<5>BO ATTRIBUTE_UNUSED = get_BO(tmpinsn[0]);
      bits<5>BI ATTRIBUTE_UNUSED = get_BI(tmpinsn[0]);
      // Instruction bclrl
      os << "bclrl ";
      os << std::hex << std::showbase << BO << std::noshowbase;
      os << ",";
      os << std::hex << std::showbase << BI << std::noshowbase;
      return 4;
    }
    
    // Instruction "bclrl"
     bool instr_handler_bclrl(bits<5> BO, bits<5> BI)
     {
      bool __branch_taken__ = false;
      {
            if ( BO ( 2 ) == 0 ) {
                   CTR_write(  CTR_read() - 1  )  ;
              }
             AdlTypeof( ( BO ( 2 ) != 0 ) || ( ( ( CTR_read() != 0 ) ^ BO ( 3 ) ) != 0 ) ) ctr_ok = ( BO ( 2 ) != 0 ) || ( ( ( CTR_read() != 0 ) ^ BO ( 3 ) ) != 0 ) ;
             AdlTypeof( ( BO ( 0 ) != 0 ) || ( CR_read() ( BI ) == BO ( 1 ) ) ) cond_ok = ( BO ( 0 ) != 0 ) || ( CR_read() ( BI ) == BO ( 1 ) ) ;
            if ( ctr_ok && cond_ok ) {
                  {   NIA_write(  LR_read() & ~ 0x3ULL  )   ; __branch_taken__ = true ; } ;
                   LR_write(  CIA_read() + 4  )  ;
              }
      }
      return __branch_taken__;
    }
    
    // Decode-cache handler for instruction "bclrl".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_48(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"bclrl",core->instr_disasm_bclrl,ifBrUncond);
        }
        if (Tracing && (core->trace_mode() & ttBasicBlockEnd)) {
          core->get_logger()->log_basic_block_end(core->CIA_read().uint64());
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_grp1();
      core->instr_handler_bclrl(dci->_args[2], dci->_args[3]);
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttBranchTaken)) {
          core->get_logger()->log_branch_end(core->CIA_read().uint64());
        }
      });
      core->time_tag_finish_grp1("bclrl");
      core->_dc_decode_cache_next = 0;
      if (core->_td_counter >= core->_td_threshold) { return (DcVoidFunc)JitRetCode; };
      return (DcVoidFunc)JitNopCode;
    }
    
    // Decode-cache translation function for instruction "bclrl"
    inline void _sym117(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> BO, bits<5> BI)
    {
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_48);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,count+1);
      di->set_arg(2,BO);
      di->set_arg(3,BI);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "bl"
    static int instr_disasm_bl(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      sbits<32>LI ATTRIBUTE_UNUSED = (get_LI(tmpinsn[0]).signExtend<32>() << 2);
      // Instruction bl
      os << "bl ";
      os << "0x" << std::setw(16) << std::setfill('0')  << std::hex << ((memaddr + LI.int32()) & ((1ull << 32)-1));
      return 4;
    }
    
    // Wrapper for instruction "bl"
    inline void  instr_handler_bl(bits<24> LI_p)
    {
      sbits<32> LI = ((LI_p.signExtend<32>() << 2) + CIA_read());
      _sym227(LI);
    }
    
    // Instruction "bl"
     void _sym227(bits<32> LI)
     {
      {
             NIA_write(  LI  )  ;
             LR_write(  CIA_read() + 4  )  ;
      }
    }
    
    // Decode-cache handler for instruction "bl".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_51(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"bl",core->instr_disasm_bl,ifBrUncond);
        }
        if (Tracing && (core->trace_mode() & ttBasicBlockEnd)) {
          core->get_logger()->log_basic_block_end(core->CIA_read().uint64());
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_grp1();
      core->_sym227(dci->_args[2]);
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttBranchTaken)) {
          core->get_logger()->log_branch_end(core->CIA_read().uint64());
        }
      });
      core->time_tag_finish_grp1("bl");
      if (core->_td_counter >= core->_td_threshold) { 
        core->_dc_decode_cache_next = 0;
        return (DcVoidFunc)JitRetCode;
         };
      DcDecodeItem *bt = dci->_bt_t->bt();
      core->_dc_decode_cache_next = bt;
      return (DcVoidFunc)JitContCode;
    }
    
    // Decode-cache translation function for instruction "bl"
    inline void _sym231(DcBasicBlock *bb,addr_t pc,unsigned count, bits<24> LI_p)
    {
      sbits<32> LI = ((LI_p.signExtend<32>() << 2) + pc);
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_51);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,count+1);
      di->set_arg(2,LI);
      _dc_decode_cache.set_br_trg(di,_dc_bb_hash.get_bb_or_stub(LI.uint64(),0,_dc_decode_cache),true);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "cmp"
    static int instr_disasm_cmp(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<3>BF ATTRIBUTE_UNUSED = get_BF(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>RB ATTRIBUTE_UNUSED = get_RB(tmpinsn[0]);
      // Instruction cmp
      os << "cmp ";
      os << std::dec << BF;
      os << ',';
      os << std::dec << "r" << RA;
      os << ',';
      os << std::dec << "r" << RB;
      return 4;
    }
    
    // Instruction "cmp"
     void instr_handler_cmp(bits<3> BF, bits<5> RA, bits<5> RB)
     {
      {
            setCrField ( BF , GPR_read( to_uint32( RA ) ) , GPR_read( to_uint32( RB ) ) ) ;
      }
    }
    
    // Decode-cache handler for instruction "cmp".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_54(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"cmp",core->instr_disasm_cmp,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_grp1();
      core->instr_handler_cmp(dci->_args[1], dci->_args[2], dci->_args[3]);
      core->time_tag_finish_grp1("cmp");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "cmp"
    inline void _sym339(DcBasicBlock *bb,addr_t pc,unsigned count, bits<3> BF, bits<5> RA, bits<5> RB)
    {
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_54);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,BF);
      di->set_arg(2,RA);
      di->set_arg(3,RB);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "cmpi"
    static int instr_disasm_cmpi(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<3>BF ATTRIBUTE_UNUSED = get_BF(tmpinsn[0]);
      bits<1>L ATTRIBUTE_UNUSED = get_L(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      sbits<32>SI ATTRIBUTE_UNUSED = get_SI(tmpinsn[0]).signExtend<32>();
      if ((BF == 0x0) && (L == 0x0)) {
        // Instruction cmpwi_
        os << "cmpwi ";
        os << std::dec << "r" << RA;
        os << ",";
        os << std::dec << SI;
        return 4;
      }
      if ((L == 0x0)) {
        // Instruction cmpwi
        os << "cmpwi ";
        os << std::dec << BF;
        os << ',';
        os << std::dec << "r" << RA;
        os << ',';
        os << std::dec << SI;
        return 4;
      }
      // Instruction cmpi
      os << "cmpi ";
      os << std::dec << BF;
      os << ',';
      os << std::dec << L;
      os << ',';
      os << std::dec << "r" << RA;
      os << ',';
      os << std::dec << SI;
      return 4;
    }
    
    // Wrapper for instruction "cmpi"
    inline void  instr_handler_cmpi(bits<3> BF, bits<1> L, bits<5> RA, bits<16> SI_p)
    {
      sbits<32> SI = SI_p.signExtend<32>();
      _sym341(BF, L, RA, SI);
    }
    
    // Instruction "cmpi"
     void _sym341(bits<3> BF, bits<1> L, bits<5> RA, bits<32> SI)
     {
      {
            setCrField ( BF , GPR_read( to_uint32( RA ) ) , SI ) ;
      }
    }
    
    // Decode-cache handler for instruction "cmpi".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_57(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"cmpi",core->instr_disasm_cmpi,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_grp1();
      core->_sym341(dci->_args[1], dci->_args[2], dci->_args[3], dci->_args[4]);
      core->time_tag_finish_grp1("cmpi");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "cmpi"
    inline void _sym345(DcBasicBlock *bb,addr_t pc,unsigned count, bits<3> BF, bits<1> L, bits<5> RA, bits<16> SI_p)
    {
      sbits<32> SI = SI_p.signExtend<32>();
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_57);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,BF);
      di->set_arg(2,L);
      di->set_arg(3,RA);
      di->set_arg(4,SI);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "halt"
    static int instr_disasm_halt(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      // Instruction halt
      os << "halt ";
      return 4;
    }
    
    // Instruction "halt"
     void instr_handler_halt()
     {
      {
            halt (  ) ;
      }
    }
    
    // Decode-cache handler for instruction "halt".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_60(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"halt",core->instr_disasm_halt,ifHalts);
        }
        if (Tracing && (core->trace_mode() & ttBasicBlockEnd)) {
          core->get_logger()->log_basic_block_end(core->CIA_read().uint64());
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_grp1();
      core->instr_handler_halt();
      core->time_tag_finish_grp1("halt");
      core->_dc_decode_cache_next = 0;
      if (core->_td_counter >= core->_td_threshold) { return (DcVoidFunc)JitRetCode; };
      return (DcVoidFunc)JitNopCode;
    }
    
    // Decode-cache translation function for instruction "halt"
    inline void _sym363(DcBasicBlock *bb,addr_t pc,unsigned count)
    {
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_60);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,count+1);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "lbz"
    static int instr_disasm_lbz(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      sbits<32>D ATTRIBUTE_UNUSED = get_D(tmpinsn[0]).signExtend<32>();
      // Instruction lbz
      os << "lbz ";
      os << std::dec << "r" << RT;
      os << ",";
      os << std::dec << D;
      os << "(";
      os << std::dec << "r" << RA;
      os << ")";
      return 4;
    }
    
    // Wrapper for instruction "lbz"
    inline void  instr_handler_lbz(bits<5> RT, bits<5> RA, bits<16> D_p)
    {
      sbits<32> D = D_p.signExtend<32>();
      _sym371(RT, RA, D);
    }
    
    // Instruction "lbz"
     void _sym371(bits<5> RT, bits<5> RA, bits<32> D)
     {
      {
             AdlTypeof( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) b = ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ;
             AdlTypeof( b + D ) addr = b + D ;
             GPR_write( to_uint32( RT ) , (  mem_read<1>(addr)  ) ) ;
      }
    }
    
    // Instruction "lbz"
     void _sym880(bits<5> RT, bits<5> RA, bits<32> D)
     {
      {
             AdlTypeof( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) b = ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ;
             AdlTypeof( b + D ) addr = b + D ;
             GPR_write( to_uint32( RT ) , (  mem_read_jit<1>(addr)  ) ) ;
      }
    }
    
    // Decode-cache handler for instruction "lbz".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_63(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"lbz",core->instr_disasm_lbz,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_loadstores();
      core->_sym880(dci->_args[1], dci->_args[2], dci->_args[3]);
      core->time_tag_finish_loadstores("lbz");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "lbz"
    inline void _sym375(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RT, bits<5> RA, bits<16> D_p)
    {
      sbits<32> D = D_p.signExtend<32>();
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_63);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RT);
      di->set_arg(2,RA);
      di->set_arg(3,D);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "lbzu"
    static int instr_disasm_lbzu(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      sbits<32>D ATTRIBUTE_UNUSED = get_D(tmpinsn[0]).signExtend<32>();
      // Instruction lbzu
      os << "lbzu ";
      os << std::dec << "r" << RT;
      os << ",";
      os << std::dec << D;
      os << "(";
      os << std::dec << "r" << RA;
      os << ")";
      return 4;
    }
    
    // Wrapper for instruction "lbzu"
    inline void  instr_handler_lbzu(bits<5> RT, bits<5> RA, bits<16> D_p)
    {
      sbits<32> D = D_p.signExtend<32>();
      _sym377(RT, RA, D);
    }
    
    // Instruction "lbzu"
     void _sym377(bits<5> RT, bits<5> RA, bits<32> D)
     {
      {
             AdlTypeof( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) a = ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ;
             AdlTypeof( a + D ) EA = a + D ;
             GPR_write( to_uint32( RT ) , (  mem_read<1>(EA)  ) ) ;
             GPR_write( to_uint32( RA ) , ( EA ) ) ;
      }
    }
    
    // Instruction "lbzu"
     void _sym881(bits<5> RT, bits<5> RA, bits<32> D)
     {
      {
             AdlTypeof( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) a = ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ;
             AdlTypeof( a + D ) EA = a + D ;
             GPR_write( to_uint32( RT ) , (  mem_read_jit<1>(EA)  ) ) ;
             GPR_write( to_uint32( RA ) , ( EA ) ) ;
      }
    }
    
    // Decode-cache handler for instruction "lbzu".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_66(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"lbzu",core->instr_disasm_lbzu,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_loadstores();
      core->_sym881(dci->_args[1], dci->_args[2], dci->_args[3]);
      core->time_tag_finish_loadstores("lbzu");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "lbzu"
    inline void _sym381(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RT, bits<5> RA, bits<16> D_p)
    {
      sbits<32> D = D_p.signExtend<32>();
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_66);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RT);
      di->set_arg(2,RA);
      di->set_arg(3,D);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "lhz"
    static int instr_disasm_lhz(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      sbits<32>D ATTRIBUTE_UNUSED = get_D(tmpinsn[0]).signExtend<32>();
      // Instruction lhz
      os << "lhz ";
      os << std::dec << "r" << RT;
      os << ",";
      os << std::dec << D;
      os << "(";
      os << std::dec << "r" << RA;
      os << ")";
      return 4;
    }
    
    // Wrapper for instruction "lhz"
    inline void  instr_handler_lhz(bits<5> RT, bits<5> RA, bits<16> D_p)
    {
      sbits<32> D = D_p.signExtend<32>();
      _sym383(RT, RA, D);
    }
    
    // Instruction "lhz"
     void _sym383(bits<5> RT, bits<5> RA, bits<32> D)
     {
      {
             AdlTypeof( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) b = ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ;
             AdlTypeof( b + D ) addr = b + D ;
             GPR_write( to_uint32( RT ) , (  mem_read<2>(addr)  ) ) ;
      }
    }
    
    // Instruction "lhz"
     void _sym882(bits<5> RT, bits<5> RA, bits<32> D)
     {
      {
             AdlTypeof( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) b = ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ;
             AdlTypeof( b + D ) addr = b + D ;
             GPR_write( to_uint32( RT ) , (  mem_read_jit<2>(addr)  ) ) ;
      }
    }
    
    // Decode-cache handler for instruction "lhz".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_69(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"lhz",core->instr_disasm_lhz,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_loadstores();
      core->_sym882(dci->_args[1], dci->_args[2], dci->_args[3]);
      core->time_tag_finish_loadstores("lhz");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "lhz"
    inline void _sym387(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RT, bits<5> RA, bits<16> D_p)
    {
      sbits<32> D = D_p.signExtend<32>();
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_69);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RT);
      di->set_arg(2,RA);
      di->set_arg(3,D);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "lmw"
    static int instr_disasm_lmw(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      sbits<32>D ATTRIBUTE_UNUSED = get_D(tmpinsn[0]).signExtend<32>();
      // Instruction lmw
      os << "lmw ";
      os << std::dec << "r" << RT;
      os << ",";
      os << std::dec << D;
      os << "(";
      os << std::dec << "r" << RA;
      os << ")";
      return 4;
    }
    
    // Wrapper for instruction "lmw"
    inline void  instr_handler_lmw(bits<5> RT, bits<5> RA, bits<16> D_p)
    {
      sbits<32> D = D_p.signExtend<32>();
      _sym401(RT, RA, D);
    }
    
    // Instruction "lmw"
     void _sym401(bits<5> RT, bits<5> RA, bits<32> D)
     {
      {
             AdlTypeof( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) b = ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ;
             AdlTypeof( b + D ) addr = b + D ;
             int r = RT . uint32 (  ) ;
            do {
                   GPR_write( to_uint32(r) , (  mem_read<4>(addr)  ) ) ;
                  r = r + 1 ;
                  addr = addr + 4 ;
              } while ( r <= 31 ) ;
      }
    }
    
    // Instruction "lmw"
     void _sym883(bits<5> RT, bits<5> RA, bits<32> D)
     {
      {
             AdlTypeof( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) b = ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ;
             AdlTypeof( b + D ) addr = b + D ;
             int r = RT . uint32 (  ) ;
            do {
                   GPR_write( to_uint32(r) , (  mem_read_jit<4>(addr)  ) ) ;
                  r = r + 1 ;
                  addr = addr + 4 ;
              } while ( r <= 31 ) ;
      }
    }
    
    // Decode-cache handler for instruction "lmw".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_72(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"lmw",core->instr_disasm_lmw,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_loadstores();
      core->_sym883(dci->_args[1], dci->_args[2], dci->_args[3]);
      core->time_tag_finish_loadstores("lmw");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "lmw"
    inline void _sym405(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RT, bits<5> RA, bits<16> D_p)
    {
      sbits<32> D = D_p.signExtend<32>();
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_72);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RT);
      di->set_arg(2,RA);
      di->set_arg(3,D);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "lwz"
    static int instr_disasm_lwz(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      sbits<32>D ATTRIBUTE_UNUSED = get_D(tmpinsn[0]).signExtend<32>();
      // Instruction lwz
      os << "lwz ";
      os << std::dec << "r" << RT;
      os << ",";
      os << std::dec << D;
      os << "(";
      os << std::dec << "r" << RA;
      os << ")";
      return 4;
    }
    
    // Wrapper for instruction "lwz"
    inline void  instr_handler_lwz(bits<5> RT, bits<5> RA, bits<16> D_p)
    {
      sbits<32> D = D_p.signExtend<32>();
      _sym407(RT, RA, D);
    }
    
    // Instruction "lwz"
     void _sym407(bits<5> RT, bits<5> RA, bits<32> D)
     {
      {
             AdlTypeof( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) b = ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ;
             AdlTypeof( b + D ) addr = b + D ;
             GPR_write( to_uint32( RT ) , (  mem_read<4>(addr)  ) ) ;
      }
    }
    
    // Instruction "lwz"
     void _sym884(bits<5> RT, bits<5> RA, bits<32> D)
     {
      {
             AdlTypeof( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) b = ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ;
             AdlTypeof( b + D ) addr = b + D ;
             GPR_write( to_uint32( RT ) , (  mem_read_jit<4>(addr)  ) ) ;
      }
    }
    
    // Decode-cache handler for instruction "lwz".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_75(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"lwz",core->instr_disasm_lwz,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_loadstores();
      core->_sym884(dci->_args[1], dci->_args[2], dci->_args[3]);
      core->time_tag_finish_loadstores("lwz");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "lwz"
    inline void _sym411(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RT, bits<5> RA, bits<16> D_p)
    {
      sbits<32> D = D_p.signExtend<32>();
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_75);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RT);
      di->set_arg(2,RA);
      di->set_arg(3,D);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "lwzu"
    static int instr_disasm_lwzu(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      sbits<32>D ATTRIBUTE_UNUSED = get_D(tmpinsn[0]).signExtend<32>();
      // Instruction lwzu
      os << "lwzu ";
      os << std::dec << "r" << RT;
      os << ",";
      os << std::dec << D;
      os << "(";
      os << std::dec << "r" << RA;
      os << ")";
      return 4;
    }
    
    // Wrapper for instruction "lwzu"
    inline void  instr_handler_lwzu(bits<5> RT, bits<5> RA, bits<16> D_p)
    {
      sbits<32> D = D_p.signExtend<32>();
      _sym413(RT, RA, D);
    }
    
    // Instruction "lwzu"
     void _sym413(bits<5> RT, bits<5> RA, bits<32> D)
     {
      {
             AdlTypeof( GPR_read( to_uint32( RA . uint32 (  ) ) ) + D ) addr = GPR_read( to_uint32( RA . uint32 (  ) ) ) + D ;
             GPR_write( to_uint32( RT ) , (  mem_read<4>(addr)  ) ) ;
             GPR_write( to_uint32( RA . uint32 (  ) ) , ( addr ) ) ;
      }
    }
    
    // Instruction "lwzu"
     void _sym885(bits<5> RT, bits<5> RA, bits<32> D)
     {
      {
             AdlTypeof( GPR_read( to_uint32( RA . uint32 (  ) ) ) + D ) addr = GPR_read( to_uint32( RA . uint32 (  ) ) ) + D ;
             GPR_write( to_uint32( RT ) , (  mem_read_jit<4>(addr)  ) ) ;
             GPR_write( to_uint32( RA . uint32 (  ) ) , ( addr ) ) ;
      }
    }
    
    // Decode-cache handler for instruction "lwzu".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_78(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"lwzu",core->instr_disasm_lwzu,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_loadstores();
      core->_sym885(dci->_args[1], dci->_args[2], dci->_args[3]);
      core->time_tag_finish_loadstores("lwzu");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "lwzu"
    inline void _sym417(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RT, bits<5> RA, bits<16> D_p)
    {
      sbits<32> D = D_p.signExtend<32>();
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_78);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RT);
      di->set_arg(2,RA);
      di->set_arg(3,D);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "lwzx"
    static int instr_disasm_lwzx(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>RB ATTRIBUTE_UNUSED = get_RB(tmpinsn[0]);
      // Instruction lwzx
      os << "lwzx ";
      os << std::dec << "r" << RT;
      os << ',';
      os << std::dec << "r" << RA;
      os << ',';
      os << std::dec << "r" << RB;
      return 4;
    }
    
    // Instruction "lwzx"
     void instr_handler_lwzx(bits<5> RT, bits<5> RA, bits<5> RB)
     {
      {
             AdlTypeof( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) b = ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ;
             AdlTypeof( b + GPR_read( to_uint32( RB ) ) ) addr = b + GPR_read( to_uint32( RB ) ) ;
             GPR_write( to_uint32( RT ) , (  mem_read<4>(addr)  ) ) ;
      }
    }
    
    // Instruction "lwzx"
     void _sym886(bits<5> RT, bits<5> RA, bits<5> RB)
     {
      {
             AdlTypeof( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) b = ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ;
             AdlTypeof( b + GPR_read( to_uint32( RB ) ) ) addr = b + GPR_read( to_uint32( RB ) ) ;
             GPR_write( to_uint32( RT ) , (  mem_read_jit<4>(addr)  ) ) ;
      }
    }
    
    // Decode-cache handler for instruction "lwzx".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_81(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"lwzx",core->instr_disasm_lwzx,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_loadstores();
      core->_sym886(dci->_args[1], dci->_args[2], dci->_args[3]);
      core->time_tag_finish_loadstores("lwzx");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "lwzx"
    inline void _sym423(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RT, bits<5> RA, bits<5> RB)
    {
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_81);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RT);
      di->set_arg(2,RA);
      di->set_arg(3,RB);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "mfspr"
    static int instr_disasm_mfspr(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<10>SPRN ATTRIBUTE_UNUSED = get_SPRN(tmpinsn[0]);
      if ((SPRN == 0x8)) {
        // Instruction mflr
        os << "mflr ";
        os << std::dec << "r" << RT;
        return 4;
      }
      // Instruction mfspr
      os << "mfspr ";
      os << std::dec << "r" << RT;
      os << ",";
      os << std::dec << "" << SPR_disasm(SPRN.uint32());
      return 4;
    }
    
    // Instruction "mfspr"
     void instr_handler_mfspr(bits<5> RT, bits<10> SPRN)
     {
      {
             GPR_write( to_uint32( RT ) , (  SPR_read( to_uint32( SPRN ) )  ) ) ;
      }
    }
    
    // Decode-cache handler for instruction "mfspr".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_84(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"mfspr",core->instr_disasm_mfspr,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_grp1();
      core->instr_handler_mfspr(dci->_args[1], dci->_args[2]);
      core->time_tag_finish_grp1("mfspr");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "mfspr"
    inline void _sym435(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RT, bits<10> SPRN)
    {
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_84);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RT);
      di->set_arg(2,SPRN);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "mtspr"
    static int instr_disasm_mtspr(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RS ATTRIBUTE_UNUSED = get_RS(tmpinsn[0]);
      bits<10>SPRN ATTRIBUTE_UNUSED = get_SPRN(tmpinsn[0]);
      if ((SPRN == 0x9)) {
        // Instruction mtctr
        os << "mtctr ";
        os << std::dec << "r" << RS;
        return 4;
      }
      if ((SPRN == 0x8)) {
        // Instruction mtlr
        os << "mtlr ";
        os << std::dec << "r" << RS;
        return 4;
      }
      // Instruction mtspr
      os << "mtspr ";
      os << std::dec << "" << SPR_disasm(SPRN.uint32());
      os << ",";
      os << std::dec << "r" << RS;
      return 4;
    }
    
    // Instruction "mtspr"
     void instr_handler_mtspr(bits<5> RS, bits<10> SPRN)
     {
      {
             SPR_write( to_uint32( SPRN ) , (  GPR_read( to_uint32( RS ) )  ) ) ;
      }
    }
    
    // Decode-cache handler for instruction "mtspr".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_87(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"mtspr",core->instr_disasm_mtspr,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_grp1();
      core->instr_handler_mtspr(dci->_args[1], dci->_args[2]);
      core->time_tag_finish_grp1("mtspr");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "mtspr"
    inline void _sym459(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RS, bits<10> SPRN)
    {
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_87);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RS);
      di->set_arg(2,SPRN);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "mulli"
    static int instr_disasm_mulli(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      sbits<32>SI ATTRIBUTE_UNUSED = get_SI(tmpinsn[0]).signExtend<32>();
      // Instruction mulli
      os << "mulli ";
      os << std::dec << "r" << RT;
      os << ',';
      os << std::dec << "r" << RA;
      os << ',';
      os << std::dec << SI;
      return 4;
    }
    
    // Wrapper for instruction "mulli"
    inline void  instr_handler_mulli(bits<5> RT, bits<5> RA, bits<16> SI_p)
    {
      sbits<32> SI = SI_p.signExtend<32>();
      _sym461(RT, RA, SI);
    }
    
    // Instruction "mulli"
     void _sym461(bits<5> RT, bits<5> RA, bits<32> SI)
     {
      {
             GPR_write( to_uint32( RT ) , (  GPR_read( to_uint32( RA ) ) * SI  ) ) ;
      }
    }
    
    // Decode-cache handler for instruction "mulli".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_90(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"mulli",core->instr_disasm_mulli,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_grp1();
      core->_sym461(dci->_args[1], dci->_args[2], dci->_args[3]);
      core->time_tag_finish_grp1("mulli");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "mulli"
    inline void _sym465(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RT, bits<5> RA, bits<16> SI_p)
    {
      sbits<32> SI = SI_p.signExtend<32>();
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_90);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RT);
      di->set_arg(2,RA);
      di->set_arg(3,SI);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "mullw"
    static int instr_disasm_mullw(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>RB ATTRIBUTE_UNUSED = get_RB(tmpinsn[0]);
      // Instruction mullw
      os << "mullw ";
      os << std::dec << "r" << RT;
      os << ',';
      os << std::dec << "r" << RA;
      os << ',';
      os << std::dec << "r" << RB;
      return 4;
    }
    
    // Instruction "mullw"
     void instr_handler_mullw(bits<5> RT, bits<5> RA, bits<5> RB)
     {
      {
             GPR_write( to_uint32( RT ) , (  GPR_read( to_uint32( RA ) ) * GPR_read( to_uint32( RB ) )  ) ) ;
      }
    }
    
    // Decode-cache handler for instruction "mullw".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_93(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"mullw",core->instr_disasm_mullw,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_multiplies();
      core->instr_handler_mullw(dci->_args[1], dci->_args[2], dci->_args[3]);
      core->time_tag_finish_multiplies("mullw");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "mullw"
    inline void _sym471(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RT, bits<5> RA, bits<5> RB)
    {
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_93);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RT);
      di->set_arg(2,RA);
      di->set_arg(3,RB);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "or"
    static int instr_disasm_or(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RS ATTRIBUTE_UNUSED = get_RS(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>RB ATTRIBUTE_UNUSED = get_RB(tmpinsn[0]);
      if ((RB == RS)) {
        // Instruction mr
        os << "mr ";
        os << std::dec << "r" << RA;
        os << ",";
        os << std::dec << "r" << RS;
        return 4;
      }
      // Instruction or
      os << "or ";
      os << std::dec << "r" << RA;
      os << ",";
      os << std::dec << "r" << RS;
      os << ",";
      os << std::dec << "r" << RB;
      return 4;
    }
    
    // Instruction "or"
     void instr_handler_or(bits<5> RS, bits<5> RA, bits<5> RB)
     {
      {
             GPR_write( to_uint32( RA ) , (  GPR_read( to_uint32( RS ) ) | GPR_read( to_uint32( RB ) )  ) ) ;
      }
    }
    
    // Decode-cache handler for instruction "or".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_96(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"or",core->instr_disasm_or,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_grp1();
      core->instr_handler_or(dci->_args[1], dci->_args[2], dci->_args[3]);
      core->time_tag_finish_grp1("or");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "or"
    inline void _sym477(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RS, bits<5> RA, bits<5> RB)
    {
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_96);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RS);
      di->set_arg(2,RA);
      di->set_arg(3,RB);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "ori"
    static int instr_disasm_ori(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RS ATTRIBUTE_UNUSED = get_RS(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<16>UI ATTRIBUTE_UNUSED = get_UI(tmpinsn[0]);
      // Instruction ori
      os << "ori ";
      os << std::dec << "r" << RA;
      os << ",";
      os << std::dec << "r" << RS;
      os << ",";
      os << std::hex << std::showbase << UI << std::noshowbase;
      return 4;
    }
    
    // Instruction "ori"
     void instr_handler_ori(bits<5> RS, bits<5> RA, bits<16> UI)
     {
      {
             GPR_write( to_uint32( RA ) , (  GPR_read( to_uint32( RS ) ) | UI  ) ) ;
      }
    }
    
    // Decode-cache handler for instruction "ori".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_99(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"ori",core->instr_disasm_ori,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_grp1();
      core->instr_handler_ori(dci->_args[1], dci->_args[2], dci->_args[3]);
      core->time_tag_finish_grp1("ori");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "ori"
    inline void _sym483(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RS, bits<5> RA, bits<16> UI)
    {
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_99);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RS);
      di->set_arg(2,RA);
      di->set_arg(3,UI);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "oris"
    static int instr_disasm_oris(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RS ATTRIBUTE_UNUSED = get_RS(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<16>UI ATTRIBUTE_UNUSED = get_UI(tmpinsn[0]);
      // Instruction oris
      os << "oris ";
      os << std::dec << "r" << RA;
      os << ",";
      os << std::dec << "r" << RS;
      os << ",";
      os << std::hex << std::showbase << UI << std::noshowbase;
      return 4;
    }
    
    // Instruction "oris"
     void instr_handler_oris(bits<5> RS, bits<5> RA, bits<16> UI)
     {
      {
             GPR_write( to_uint32( RA ) , (  GPR_read( to_uint32( RS ) ) | concat ( UI , zero<16>() )  ) ) ;
      }
    }
    
    // Decode-cache handler for instruction "oris".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_102(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"oris",core->instr_disasm_oris,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_grp1();
      core->instr_handler_oris(dci->_args[1], dci->_args[2], dci->_args[3]);
      core->time_tag_finish_grp1("oris");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "oris"
    inline void _sym489(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RS, bits<5> RA, bits<16> UI)
    {
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_102);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RS);
      di->set_arg(2,RA);
      di->set_arg(3,UI);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "rlwinm"
    static int instr_disasm_rlwinm(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RS ATTRIBUTE_UNUSED = get_RS(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>SH ATTRIBUTE_UNUSED = get_SH(tmpinsn[0]);
      bits<5>MB ATTRIBUTE_UNUSED = get_MB(tmpinsn[0]);
      bits<5>ME ATTRIBUTE_UNUSED = get_ME(tmpinsn[0]);
      if ((SH == 0x0) && (ME == 0x1f)) {
        // Instruction clrlwi
        os << "clrlwi ";
        os << std::dec << "r" << RA;
        os << ",";
        os << std::dec << "r" << RS;
        os << ",";
        os << std::dec << MB;
        return 4;
      }
      // Instruction rlwinm
      os << "rlwinm ";
      os << std::dec << "r" << RA;
      os << ",";
      os << std::dec << "r" << RS;
      os << ",";
      os << std::dec << SH;
      os << ",";
      os << std::dec << MB;
      os << ",";
      os << std::dec << ME;
      return 4;
    }
    
    // Instruction "rlwinm"
     void instr_handler_rlwinm(bits<5> RS, bits<5> RA, bits<5> SH, bits<5> MB, bits<5> ME)
     {
      {
             AdlTypeof( GPR_read( to_uint32( RS ) ) . left_rotate ( SH ) ) r = GPR_read( to_uint32( RS ) ) . left_rotate ( SH ) ;
             bits < 32 > m ;
            m . mask ( MB , ME ) ;
             GPR_write( to_uint32( RA ) , (  r & m  ) ) ;
      }
    }
    
    // Decode-cache handler for instruction "rlwinm".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_105(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"rlwinm",core->instr_disasm_rlwinm,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_grp1();
      core->instr_handler_rlwinm(dci->_args[1], dci->_args[2], dci->_args[3], dci->_args[4], dci->_args[5]);
      core->time_tag_finish_grp1("rlwinm");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "rlwinm"
    inline void _sym495(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RS, bits<5> RA, bits<5> SH, bits<5> MB, bits<5> ME)
    {
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_105);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RS);
      di->set_arg(2,RA);
      di->set_arg(3,SH);
      di->set_arg(4,MB);
      di->set_arg(5,ME);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "sc"
    static int instr_disasm_sc(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      // Instruction sc
      os << "sc ";
      return 4;
    }
    
    // Instruction "sc"
     void instr_handler_sc()
     {
      {
            if ( syscall_enabled (  ) ) {
                  syscall_add_arg ( GPR_read( to_uint32(1) ) ) ;
                  syscall_add_arg ( GPR_read( to_uint32(3) ) ) ;
                  syscall_add_arg ( GPR_read( to_uint32(4) ) ) ;
                  syscall_add_arg ( GPR_read( to_uint32(5) ) ) ;
                  syscall_add_arg ( GPR_read( to_uint32(6) ) ) ;
                  syscall_add_arg ( GPR_read( to_uint32(7) ) ) ;
                  syscall_trigger ( GPR_read( to_uint32(0) ) ) ;
                   GPR_write( to_uint32(3) , (  syscall_return_code (  )  ) ) ;
              }
      }
    }
    
    // Decode-cache handler for instruction "sc".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_108(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"sc",core->instr_disasm_sc,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_grp1();
      core->instr_handler_sc();
      core->time_tag_finish_grp1("sc");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "sc"
    inline void _sym507(DcBasicBlock *bb,addr_t pc,unsigned count)
    {
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_108);
      di->set_arg(0,__sym27[0]);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "srw"
    static int instr_disasm_srw(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RS ATTRIBUTE_UNUSED = get_RS(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>RB ATTRIBUTE_UNUSED = get_RB(tmpinsn[0]);
      // Instruction srw
      os << "srw ";
      os << std::dec << "r" << RA;
      os << ",";
      os << std::dec << "r" << RS;
      os << ",";
      os << std::dec << "r" << RB;
      return 4;
    }
    
    // Instruction "srw"
     void instr_handler_srw(bits<5> RS, bits<5> RA, bits<5> RB)
     {
      {
             AdlTypeof( GPR_read( to_uint32( RB ) ) .get<5>(27,31) ) n = GPR_read( to_uint32( RB ) ) .get<5>(27,31) ;
             GPR_write( to_uint32( RA ) , (  GPR_read( to_uint32( RS ) ) >> n  ) ) ;
      }
    }
    
    // Decode-cache handler for instruction "srw".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_111(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"srw",core->instr_disasm_srw,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_grp1();
      core->instr_handler_srw(dci->_args[1], dci->_args[2], dci->_args[3]);
      core->time_tag_finish_grp1("srw");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "srw"
    inline void _sym519(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RS, bits<5> RA, bits<5> RB)
    {
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_111);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RS);
      di->set_arg(2,RA);
      di->set_arg(3,RB);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "stb"
    static int instr_disasm_stb(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RS ATTRIBUTE_UNUSED = get_RS(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      sbits<32>D ATTRIBUTE_UNUSED = get_D(tmpinsn[0]).signExtend<32>();
      // Instruction stb
      os << "stb ";
      os << std::dec << "r" << RS;
      os << ",";
      os << std::dec << D;
      os << "(";
      os << std::dec << "r" << RA;
      os << ")";
      return 4;
    }
    
    // Wrapper for instruction "stb"
    inline void  instr_handler_stb(bits<5> RS, bits<5> RA, bits<16> D_p)
    {
      sbits<32> D = D_p.signExtend<32>();
      _sym521(RS, RA, D);
    }
    
    // Instruction "stb"
     void _sym521(bits<5> RS, bits<5> RA, bits<32> D)
     {
      {
            mem_write<1>( ( ( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) + D ) ,to_bits<ns_bits>( GPR_read( to_uint32( RS ) ) )) ;
      }
    }
    
    // Instruction "stb"
     void _sym887(bits<5> RS, bits<5> RA, bits<32> D)
     {
      {
            mem_write_jit<1>( ( ( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) + D ) ,to_bits<ns_bits>( GPR_read( to_uint32( RS ) ) )) ;
      }
    }
    
    // Decode-cache handler for instruction "stb".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_114(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"stb",core->instr_disasm_stb,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_loadstores();
      core->_sym887(dci->_args[1], dci->_args[2], dci->_args[3]);
      core->time_tag_finish_loadstores("stb");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "stb"
    inline void _sym525(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RS, bits<5> RA, bits<16> D_p)
    {
      sbits<32> D = D_p.signExtend<32>();
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_114);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RS);
      di->set_arg(2,RA);
      di->set_arg(3,D);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "sth"
    static int instr_disasm_sth(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RS ATTRIBUTE_UNUSED = get_RS(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      sbits<32>D ATTRIBUTE_UNUSED = get_D(tmpinsn[0]).signExtend<32>();
      // Instruction sth
      os << "sth ";
      os << std::dec << "r" << RS;
      os << ",";
      os << std::dec << D;
      os << "(";
      os << std::dec << "r" << RA;
      os << ")";
      return 4;
    }
    
    // Wrapper for instruction "sth"
    inline void  instr_handler_sth(bits<5> RS, bits<5> RA, bits<16> D_p)
    {
      sbits<32> D = D_p.signExtend<32>();
      _sym527(RS, RA, D);
    }
    
    // Instruction "sth"
     void _sym527(bits<5> RS, bits<5> RA, bits<32> D)
     {
      {
             AdlTypeof( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) b = ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ;
             AdlTypeof( b + D ) addr = b + D ;
            mem_write<2>(addr,to_bits<ns_bits>( GPR_read( to_uint32( RS ) ) )) ;
      }
    }
    
    // Instruction "sth"
     void _sym888(bits<5> RS, bits<5> RA, bits<32> D)
     {
      {
             AdlTypeof( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) b = ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ;
             AdlTypeof( b + D ) addr = b + D ;
            mem_write_jit<2>(addr,to_bits<ns_bits>( GPR_read( to_uint32( RS ) ) )) ;
      }
    }
    
    // Decode-cache handler for instruction "sth".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_117(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"sth",core->instr_disasm_sth,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_loadstores();
      core->_sym888(dci->_args[1], dci->_args[2], dci->_args[3]);
      core->time_tag_finish_loadstores("sth");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "sth"
    inline void _sym531(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RS, bits<5> RA, bits<16> D_p)
    {
      sbits<32> D = D_p.signExtend<32>();
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_117);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RS);
      di->set_arg(2,RA);
      di->set_arg(3,D);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "sthx"
    static int instr_disasm_sthx(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RS ATTRIBUTE_UNUSED = get_RS(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>RB ATTRIBUTE_UNUSED = get_RB(tmpinsn[0]);
      // Instruction sthx
      os << "sthx ";
      os << std::dec << "r" << RS;
      os << ',';
      os << std::dec << "r" << RA;
      os << ',';
      os << std::dec << "r" << RB;
      return 4;
    }
    
    // Instruction "sthx"
     void instr_handler_sthx(bits<5> RS, bits<5> RA, bits<5> RB)
     {
      {
             AdlTypeof( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) b = ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ;
             AdlTypeof( b + GPR_read( to_uint32( RB ) ) ) addr = b + GPR_read( to_uint32( RB ) ) ;
            mem_write<2>(addr,to_bits<ns_bits>( GPR_read( to_uint32( RS ) ) )) ;
      }
    }
    
    // Instruction "sthx"
     void _sym889(bits<5> RS, bits<5> RA, bits<5> RB)
     {
      {
             AdlTypeof( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) b = ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ;
             AdlTypeof( b + GPR_read( to_uint32( RB ) ) ) addr = b + GPR_read( to_uint32( RB ) ) ;
            mem_write_jit<2>(addr,to_bits<ns_bits>( GPR_read( to_uint32( RS ) ) )) ;
      }
    }
    
    // Decode-cache handler for instruction "sthx".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_120(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"sthx",core->instr_disasm_sthx,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_loadstores();
      core->_sym889(dci->_args[1], dci->_args[2], dci->_args[3]);
      core->time_tag_finish_loadstores("sthx");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "sthx"
    inline void _sym537(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RS, bits<5> RA, bits<5> RB)
    {
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_120);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RS);
      di->set_arg(2,RA);
      di->set_arg(3,RB);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "stmw"
    static int instr_disasm_stmw(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RS ATTRIBUTE_UNUSED = get_RS(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      sbits<32>D ATTRIBUTE_UNUSED = get_D(tmpinsn[0]).signExtend<32>();
      // Instruction stmw
      os << "stmw ";
      os << std::dec << "r" << RS;
      os << ",";
      os << std::dec << D;
      os << "(";
      os << std::dec << "r" << RA;
      os << ")";
      return 4;
    }
    
    // Wrapper for instruction "stmw"
    inline void  instr_handler_stmw(bits<5> RS, bits<5> RA, bits<16> D_p)
    {
      sbits<32> D = D_p.signExtend<32>();
      _sym539(RS, RA, D);
    }
    
    // Instruction "stmw"
     void _sym539(bits<5> RS, bits<5> RA, bits<32> D)
     {
      {
             AdlTypeof( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) b = ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ;
             AdlTypeof( b + D ) addr = b + D ;
             int r = RS . uint32 (  ) ;
            do {
                  mem_write<4>(addr,to_bits<ns_bits>( GPR_read( to_uint32(r) ) )) ;
                  r = r + 1 ;
                  addr = addr + 4 ;
              } while ( r <= 31 ) ;
      }
    }
    
    // Instruction "stmw"
     void _sym890(bits<5> RS, bits<5> RA, bits<32> D)
     {
      {
             AdlTypeof( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) b = ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ;
             AdlTypeof( b + D ) addr = b + D ;
             int r = RS . uint32 (  ) ;
            do {
                  mem_write_jit<4>(addr,to_bits<ns_bits>( GPR_read( to_uint32(r) ) )) ;
                  r = r + 1 ;
                  addr = addr + 4 ;
              } while ( r <= 31 ) ;
      }
    }
    
    // Decode-cache handler for instruction "stmw".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_123(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"stmw",core->instr_disasm_stmw,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_loadstores();
      core->_sym890(dci->_args[1], dci->_args[2], dci->_args[3]);
      core->time_tag_finish_loadstores("stmw");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "stmw"
    inline void _sym543(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RS, bits<5> RA, bits<16> D_p)
    {
      sbits<32> D = D_p.signExtend<32>();
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_123);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RS);
      di->set_arg(2,RA);
      di->set_arg(3,D);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "stw"
    static int instr_disasm_stw(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RS ATTRIBUTE_UNUSED = get_RS(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      sbits<32>D ATTRIBUTE_UNUSED = get_D(tmpinsn[0]).signExtend<32>();
      // Instruction stw
      os << "stw ";
      os << std::dec << "r" << RS;
      os << ",";
      os << std::dec << D;
      os << "(";
      os << std::dec << "r" << RA;
      os << ")";
      return 4;
    }
    
    // Wrapper for instruction "stw"
    inline void  instr_handler_stw(bits<5> RS, bits<5> RA, bits<16> D_p)
    {
      sbits<32> D = D_p.signExtend<32>();
      _sym545(RS, RA, D);
    }
    
    // Instruction "stw"
     void _sym545(bits<5> RS, bits<5> RA, bits<32> D)
     {
      {
             AdlTypeof( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) b = ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ;
             AdlTypeof( b + D ) addr = b + D ;
            mem_write<4>(addr,to_bits<ns_bits>( GPR_read( to_uint32( RS ) ) )) ;
      }
    }
    
    // Instruction "stw"
     void _sym891(bits<5> RS, bits<5> RA, bits<32> D)
     {
      {
             AdlTypeof( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) b = ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ;
             AdlTypeof( b + D ) addr = b + D ;
            mem_write_jit<4>(addr,to_bits<ns_bits>( GPR_read( to_uint32( RS ) ) )) ;
      }
    }
    
    // Decode-cache handler for instruction "stw".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_126(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"stw",core->instr_disasm_stw,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_loadstores();
      core->_sym891(dci->_args[1], dci->_args[2], dci->_args[3]);
      core->time_tag_finish_loadstores("stw");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "stw"
    inline void _sym549(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RS, bits<5> RA, bits<16> D_p)
    {
      sbits<32> D = D_p.signExtend<32>();
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_126);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RS);
      di->set_arg(2,RA);
      di->set_arg(3,D);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "stwu"
    static int instr_disasm_stwu(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RS ATTRIBUTE_UNUSED = get_RS(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      sbits<32>D ATTRIBUTE_UNUSED = get_D(tmpinsn[0]).signExtend<32>();
      // Instruction stwu
      os << "stwu ";
      os << std::dec << "r" << RS;
      os << ",";
      os << std::dec << D;
      os << "(";
      os << std::dec << "r" << RA;
      os << ")";
      return 4;
    }
    
    // Wrapper for instruction "stwu"
    inline void  instr_handler_stwu(bits<5> RS, bits<5> RA, bits<16> D_p)
    {
      sbits<32> D = D_p.signExtend<32>();
      _sym551(RS, RA, D);
    }
    
    // Instruction "stwu"
     void _sym551(bits<5> RS, bits<5> RA, bits<32> D)
     {
      {
             AdlTypeof( GPR_read( to_uint32( RA ) ) + D ) addr = GPR_read( to_uint32( RA ) ) + D ;
            mem_write<4>(addr,to_bits<ns_bits>( GPR_read( to_uint32( RS ) ) )) ;
             GPR_write( to_uint32( RA ) , ( addr ) ) ;
      }
    }
    
    // Instruction "stwu"
     void _sym892(bits<5> RS, bits<5> RA, bits<32> D)
     {
      {
             AdlTypeof( GPR_read( to_uint32( RA ) ) + D ) addr = GPR_read( to_uint32( RA ) ) + D ;
            mem_write_jit<4>(addr,to_bits<ns_bits>( GPR_read( to_uint32( RS ) ) )) ;
             GPR_write( to_uint32( RA ) , ( addr ) ) ;
      }
    }
    
    // Decode-cache handler for instruction "stwu".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_129(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"stwu",core->instr_disasm_stwu,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_loadstores();
      core->_sym892(dci->_args[1], dci->_args[2], dci->_args[3]);
      core->time_tag_finish_loadstores("stwu");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "stwu"
    inline void _sym555(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RS, bits<5> RA, bits<16> D_p)
    {
      sbits<32> D = D_p.signExtend<32>();
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_129);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RS);
      di->set_arg(2,RA);
      di->set_arg(3,D);
      _dc_decode_cache.add_item();
    }
    
    // Disassembler for "stwx"
    static int instr_disasm_stwx(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RS ATTRIBUTE_UNUSED = get_RS(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>RB ATTRIBUTE_UNUSED = get_RB(tmpinsn[0]);
      // Instruction stwx
      os << "stwx ";
      os << std::dec << "r" << RS;
      os << ',';
      os << std::dec << "r" << RA;
      os << ',';
      os << std::dec << "r" << RB;
      return 4;
    }
    
    // Instruction "stwx"
     void instr_handler_stwx(bits<5> RS, bits<5> RA, bits<5> RB)
     {
      {
             AdlTypeof( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) b = ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ;
             AdlTypeof( b + GPR_read( to_uint32( RB ) ) ) addr = b + GPR_read( to_uint32( RB ) ) ;
            mem_write<4>(addr,to_bits<ns_bits>( GPR_read( to_uint32( RS ) ) )) ;
      }
    }
    
    // Instruction "stwx"
     void _sym893(bits<5> RS, bits<5> RA, bits<5> RB)
     {
      {
             AdlTypeof( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) b = ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ;
             AdlTypeof( b + GPR_read( to_uint32( RB ) ) ) addr = b + GPR_read( to_uint32( RB ) ) ;
            mem_write_jit<4>(addr,to_bits<ns_bits>( GPR_read( to_uint32( RS ) ) )) ;
      }
    }
    
    // Decode-cache handler for instruction "stwx".
    static DcDecodeReturn __fastcall dc_decode_cache_instr_132(adliss::P*core,DcDecodeItem *dci) FASTCALL
    {
      ++(core->_instr_count);
      core->CIA_write(core->NIA_read());
      ExtCondTrace({
        if (Tracing && (core->trace_mode() & ttInstrPrefetch)) {
          core->get_logger()->log_instr_prefetch(core->CIA_read().uint64());
        }
        if (Tracing && (core->trace_mode() & ttInstr)) {
          core->get_logger()->log_instr(dci->_args,8,"stwx",core->instr_disasm_stwx,0);
        }
      })
      core->NIA_write_simple(core->CIA_read() + 4);
      core->time_tag_start_grp2();
      core->_sym893(dci->_args[1], dci->_args[2], dci->_args[3]);
      core->time_tag_finish_grp2("stwx");
      core->_dc_decode_cache_next = &dci[1];
      return (DcVoidFunc)dci[1].func();
    }
    
    // Decode-cache translation function for instruction "stwx"
    inline void _sym561(DcBasicBlock *bb,addr_t pc,unsigned count, bits<5> RS, bits<5> RA, bits<5> RB)
    {
      adliss::P::DcDecodeItem *di = _dc_decode_cache.cur();
      di->set_func(dc_decode_cache_instr_132);
      di->set_arg(0,__sym27[0]);
      di->set_arg(1,RS);
      di->set_arg(2,RA);
      di->set_arg(3,RB);
      _dc_decode_cache.add_item();
    }
    
    void decode_miss_handler(addr_t cia,unsigned instr) ATTRIBUTE_USED
    {
      unknownInstruction(cia,instr);
    }
    
    unsigned activeCount() const { return _active; };
    
    virtual bool is_active() const { return activeCount(); };
    
    virtual unsigned get_active_count() const { return activeCount(); };
    
    virtual unsigned set_active_count(unsigned x)
    {
      if (_active_callback) {
        return _active_callback->set_active_count(this,_active,x);
      } else {
        unsigned o = _active;
        _active = x;
        return o;
      }
    };
    
    void setActiveCallback(ActiveCallback *ac) { _active_callback = ac; };
    
    void pre_fetch_handler()  ATTRIBUTE_USED {
    }
    
    void post_exec_outer_handler()  ATTRIBUTE_USED {
    }
    
    void check_context_updates ()
    {
    }
    
    void commit_checks() {
      check_context_updates();
      addr_t ea = NIA_read().uint64();
      check_breakpoints(ea);
    }
    
    void check_breakpoints(addr_t ea)
    {
      {
        if (!_breakpoints.empty()) {
          MemPoints::iterator i = _breakpoints.find(ea);
          if (i != _breakpoints.end()) {
            issueBreakpoint(ea,i);
          }
        }
      }
      
    }
    
    void checkForBreakpoint(addr_t ea)
    {
      check_breakpoints(ea);
    }
    
    void time_tag_start_grp1()
    {
      start_instr(1);
    }
    
    void time_tag_finish_grp1(const char *iname)
    {
      advance_current_time(_Fetch_stage+1);
      ttime_t issue_time = std::max(_td_counter,current_time());
      
      if (Tracing && DebugMode) {
        std::cout << "Time:  " << std::dec << issue_time << "\t#" << _instr_count << " 0x" << std::hex << CIA_read().uint64() << ":" << iname << std::dec << " (""op-avail-time: " << current_time() << ", initial-time: " << _td_counter << ")\n";
      }
      
      unsigned total_instr_latency = _cache_latency + _latency - 1;
      _Fetch_stage = std::max(issue_time+0,_Decode_stage);
      _Decode_stage = std::max(issue_time+1,_Exec_stage);
      _Exec_stage = std::max(issue_time+2,_Writeback_stage);
      _Writeback_stage = std::max(issue_time+3,_compl_time)+ total_instr_latency;
      _compl_time = std::max(_compl_time+1,issue_time+4)+ total_instr_latency;
      
      ttime_t log_issue_time = _Fetch_stage;
      if (Tracing && (_trace_mode & ttInstrTimes)) {
        _logger->log_instr_issue_time(log_issue_time);
        _logger->log_instr_completion_time(_compl_time-1);
      }
      
      update_completion_time();
      if (Tracing && DebugMode) {
        std::cout
         << "\tFetch    :  " << _Fetch_stage << "\n"
         << "\tDecode   :  " << _Decode_stage << "\n"
         << "\tExec     :  " << _Exec_stage << "\n"
         << "\tWriteback:  " << _Writeback_stage << "\n"
         << "\n";
      }
      
    }
    
    void time_tag_start_grp2()
    {
      start_instr(3);
    }
    
    void time_tag_finish_grp2(const char *iname)
    {
      advance_current_time(_Fetch_stage+1);
      ttime_t issue_time = std::max(_td_counter,current_time());
      
      if (Tracing && DebugMode) {
        std::cout << "Time:  " << std::dec << issue_time << "\t#" << _instr_count << " 0x" << std::hex << CIA_read().uint64() << ":" << iname << std::dec << " (""op-avail-time: " << current_time() << ", initial-time: " << _td_counter << ")\n";
      }
      
      unsigned total_instr_latency = _cache_latency + _latency - 1;
      _Fetch_stage = std::max(issue_time+0,_Decode_stage);
      _Decode_stage = std::max(issue_time+1,_Exec_stage);
      _Exec_stage = std::max(issue_time+2,_Mem_stage);
      _Mem_stage = std::max(issue_time+3,_Writeback_stage);
      _Writeback_stage = std::max(issue_time+4,_compl_time)+ total_instr_latency;
      _compl_time = std::max(_compl_time+1,issue_time+5)+ total_instr_latency;
      
      ttime_t log_issue_time = _Fetch_stage;
      if (Tracing && (_trace_mode & ttInstrTimes)) {
        _logger->log_instr_issue_time(log_issue_time);
        _logger->log_instr_completion_time(_compl_time-1);
      }
      
      update_completion_time();
      if (Tracing && DebugMode) {
        std::cout
         << "\tFetch    :  " << _Fetch_stage << "\n"
         << "\tDecode   :  " << _Decode_stage << "\n"
         << "\tExec     :  " << _Exec_stage << "\n"
         << "\tMem      :  " << _Mem_stage << "\n"
         << "\tWriteback:  " << _Writeback_stage << "\n"
         << "\n";
      }
      
    }
    
    void time_tag_start_loadstores()
    {
      start_instr(2);
    }
    
    void time_tag_finish_loadstores(const char *iname)
    {
      advance_current_time(_Fetch_stage+1);
      ttime_t issue_time = std::max(_td_counter,current_time());
      
      if (Tracing && DebugMode) {
        std::cout << "Time:  " << std::dec << issue_time << "\t#" << _instr_count << " 0x" << std::hex << CIA_read().uint64() << ":" << iname << std::dec << " (""op-avail-time: " << current_time() << ", initial-time: " << _td_counter << ")\n";
      }
      
      unsigned total_instr_latency = _cache_latency + _latency - 1;
      _Fetch_stage = std::max(issue_time+0,_Decode_stage);
      _Decode_stage = std::max(issue_time+1,_Exec_stage);
      _Exec_stage = std::max(issue_time+2,_Mem_stage);
      _Mem_stage = std::max(issue_time+3,_Writeback_stage);
      _Writeback_stage = std::max(issue_time+4,_compl_time)+ total_instr_latency;
      _compl_time = std::max(_compl_time+1,issue_time+5)+ total_instr_latency;
      
      ttime_t log_issue_time = _Fetch_stage;
      if (Tracing && (_trace_mode & ttInstrTimes)) {
        _logger->log_instr_issue_time(log_issue_time);
        _logger->log_instr_completion_time(_compl_time-1);
      }
      
      update_completion_time();
      if (Tracing && DebugMode) {
        std::cout
         << "\tFetch    :  " << _Fetch_stage << "\n"
         << "\tDecode   :  " << _Decode_stage << "\n"
         << "\tExec     :  " << _Exec_stage << "\n"
         << "\tMem      :  " << _Mem_stage << "\n"
         << "\tWriteback:  " << _Writeback_stage << "\n"
         << "\n";
      }
      
    }
    
    void time_tag_start_multiplies()
    {
      start_instr(8);
    }
    
    void time_tag_finish_multiplies(const char *iname)
    {
      advance_current_time(_Fetch_stage+1);
      ttime_t issue_time = std::max(_td_counter,current_time());
      
      if (Tracing && DebugMode) {
        std::cout << "Time:  " << std::dec << issue_time << "\t#" << _instr_count << " 0x" << std::hex << CIA_read().uint64() << ":" << iname << std::dec << " (""op-avail-time: " << current_time() << ", initial-time: " << _td_counter << ")\n";
      }
      
      unsigned total_instr_latency = _cache_latency + _latency - 1;
      _Fetch_stage = std::max(issue_time+0,_Decode_stage);
      _Decode_stage = std::max(issue_time+1,_ExecMul_stage);
      _ExecMul_stage = std::max(issue_time+2,_Writeback_stage);
      _Writeback_stage = std::max(issue_time+3,_compl_time)+ total_instr_latency;
      _compl_time = std::max(_compl_time+1,issue_time+4)+ total_instr_latency;
      
      ttime_t log_issue_time = _Fetch_stage;
      if (Tracing && (_trace_mode & ttInstrTimes)) {
        _logger->log_instr_issue_time(log_issue_time);
        _logger->log_instr_completion_time(_compl_time-1);
      }
      
      update_completion_time();
      if (Tracing && DebugMode) {
        std::cout
         << "\tFetch    :  " << _Fetch_stage << "\n"
         << "\tDecode   :  " << _Decode_stage << "\n"
         << "\tExecMul  :  " << _ExecMul_stage << "\n"
         << "\tWriteback:  " << _Writeback_stage << "\n"
         << "\n";
      }
      
    }
    
    bool hasExternalSysCalls() const { return true; }
    
    void setSysCallHandler(SysCallExtHandler *s) {
      _schandler = s;
    }
    
    SysCallExtHandler *getSysCallHandler() const { return _schandler; }
    
    bool syscall_enabled() { return _schandler->syscall_enabled(); };
    
    void syscall_trigger(uint32_t syscode) { _schandler->syscall_trigger(*this,syscode); };
    
    template <size_t Nb>
    void syscall_trigger(const intbv<Nb> &x) { _schandler->syscall_trigger(*this,x.uint32()); };
    
    void syscall_add_arg(uint64_t a) { _schandler->syscall_add_arg(a); };
    
    template <size_t Nb>
    void syscall_add_arg(const intbv<Nb> &x) { _schandler->syscall_add_arg(x.uint64()); };
    
    int64_t syscall_return_code() { return _schandler->syscall_return_code(); };
    
    uint32_t syscall_errno() { return _schandler->syscall_errno(); };
    
    
    void simulate()
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      if (InstrMax && (_instr_count >= InstrMax)) {
        halt();
      }
      if (_active) {
        if (Tracing && Heartbeat) {
          if ((_instr_count % Heartbeat) == 0) {
            std::cerr << name() << ":  ic=" << instr_count() << '\n';}
        }
        
        {
          {
            ++_instr_count;
            CIA_write(NIA_read());
            if (Tracing && (_trace_mode & ttInstrPrefetch)) {
              _logger->log_instr_prefetch(CIA_read().uint64());
            }
            __sym27[0] = instr_read32(CIA_read().uint64());
            exec(true);
            
          }
        }
        commit_checks();
        ;
      }
    }
    
    #endif // ADL_HEADER
    
    
    ExecStatus exec(bool extra_fetch)
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      ExecStatus rc = esNormal;
      switch (__sym27[0] & 0xfc000000) {
        case 0x0: {
          // Instruction:  "halt"
          if (Tracing) {
            if (_trace_mode & ttInstr) {
              _logger->log_instr(__sym27,8,"halt",instr_disasm_halt,ifHalts);
            }
            if (_trace_mode & ttBasicBlockEnd) {
              _logger->log_basic_block_end(CIA_read().uint64());
            }
          }
          NIA_write_simple(CIA_read() + 4);
          time_tag_start_grp1();
          instr_handler_halt();
          time_tag_finish_grp1("halt");
          rc = esEndBasicBlock;
        }
        break;
        case 0x1c000000: {
          // Instruction:  "mulli"
          if (Tracing) {
            if (_trace_mode & ttInstr) {
              _logger->log_instr(__sym27,8,"mulli",instr_disasm_mulli,0);
            }
          }
          NIA_write_simple(CIA_read() + 4);
          time_tag_start_grp1();
          instr_handler_mulli(get_RT(__sym27[0]), get_RA(__sym27[0]), get_SI(__sym27[0]));
          time_tag_finish_grp1("mulli");
        }
        break;
        case 0x2c000000: {
          // Instruction:  "cmpi"
          if (Tracing) {
            if (_trace_mode & ttInstr) {
              _logger->log_instr(__sym27,8,"cmpi",instr_disasm_cmpi,0);
            }
          }
          NIA_write_simple(CIA_read() + 4);
          time_tag_start_grp1();
          instr_handler_cmpi(get_BF(__sym27[0]), get_L(__sym27[0]), get_RA(__sym27[0]), get_SI(__sym27[0]));
          time_tag_finish_grp1("cmpi");
        }
        break;
        case 0x30000000: {
          // Instruction:  "addic"
          if (Tracing) {
            if (_trace_mode & ttInstr) {
              _logger->log_instr(__sym27,8,"addic",instr_disasm_addic,0);
            }
          }
          NIA_write_simple(CIA_read() + 4);
          time_tag_start_grp1();
          instr_handler_addic(get_RT(__sym27[0]), get_RA(__sym27[0]), get_SI(__sym27[0]));
          time_tag_finish_grp1("addic");
        }
        break;
        case 0x34000000: {
          // Instruction:  "addic."
          if (Tracing) {
            if (_trace_mode & ttInstr) {
              _logger->log_instr(__sym27,8,"addic.",instr_disasm_addic_,0);
            }
          }
          NIA_write_simple(CIA_read() + 4);
          time_tag_start_grp1();
          instr_handler_addic_(get_RT(__sym27[0]), get_RA(__sym27[0]), get_SI(__sym27[0]));
          time_tag_finish_grp1("addic.");
        }
        break;
        case 0x38000000: {
          // Instruction:  "addi"
          if (Tracing) {
            if (_trace_mode & ttInstr) {
              _logger->log_instr(__sym27,8,"addi",instr_disasm_addi,0);
            }
          }
          NIA_write_simple(CIA_read() + 4);
          time_tag_start_grp1();
          instr_handler_addi(get_RT(__sym27[0]), get_RA(__sym27[0]), get_SI(__sym27[0]));
          time_tag_finish_grp1("addi");
        }
        break;
        case 0x3c000000: {
          // Instruction:  "addis"
          if (Tracing) {
            if (_trace_mode & ttInstr) {
              _logger->log_instr(__sym27,8,"addis",instr_disasm_addis,0);
            }
          }
          NIA_write_simple(CIA_read() + 4);
          time_tag_start_grp1();
          instr_handler_addis(get_RT(__sym27[0]), get_RA(__sym27[0]), get_SI(__sym27[0]));
          time_tag_finish_grp1("addis");
        }
        break;
        case 0x40000000: {
          switch (__sym27[0] & 0x3) {
            case 0x0: {
              // Instruction:  "bc"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"bc",instr_disasm_bc,ifBrCond);
                }
                if (_trace_mode & ttBasicBlockEnd) {
                  _logger->log_basic_block_end(CIA_read().uint64());
                }
              }
              NIA_write_simple(CIA_read() + 4);
              time_tag_start_grp1();
              instr_handler_bc(get_BO(__sym27[0]), get_BI(__sym27[0]), get_BD(__sym27[0]));
              time_tag_finish_grp1("bc");
              if (Tracing && (_trace_mode && ttBranchTaken)) {
                _logger->log_branch_end(CIA_read().uint64());
              }
              rc = esEndBasicBlock;
            }
            break;
            case 0x1: {
              // Instruction:  "bcl"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"bcl",instr_disasm_bcl,ifBrCond);
                }
                if (_trace_mode & ttBasicBlockEnd) {
                  _logger->log_basic_block_end(CIA_read().uint64());
                }
              }
              NIA_write_simple(CIA_read() + 4);
              time_tag_start_grp1();
              instr_handler_bcl(get_BO(__sym27[0]), get_BI(__sym27[0]), get_BD(__sym27[0]));
              time_tag_finish_grp1("bcl");
              if (Tracing && (_trace_mode && ttBranchTaken)) {
                _logger->log_branch_end(CIA_read().uint64());
              }
              rc = esEndBasicBlock;
            }
            break;
            default:
            // Decode miss.
            if (Tracing && (_trace_mode & ttInstr)) {
              _logger->log_instr(__sym27,8,"<illop>",disasm_illop,0);
            }
            decode_miss_handler(CIA_read().uint64(),__sym27[0]);
            return esEndBasicBlock;
            break;
          }
        }
        break;
        case 0x44000000: {
          switch (__sym27[0] & 0x7fe) {
            case 0x2: {
              // Instruction:  "sc"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"sc",instr_disasm_sc,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              time_tag_start_grp1();
              instr_handler_sc();
              time_tag_finish_grp1("sc");
            }
            break;
            default:
            // Decode miss.
            if (Tracing && (_trace_mode & ttInstr)) {
              _logger->log_instr(__sym27,8,"<illop>",disasm_illop,0);
            }
            decode_miss_handler(CIA_read().uint64(),__sym27[0]);
            return esEndBasicBlock;
            break;
          }
        }
        break;
        case 0x48000000: {
          switch (__sym27[0] & 0x3) {
            case 0x0: {
              // Instruction:  "b"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"b",instr_disasm_b,ifBrUncond);
                }
                if (_trace_mode & ttBasicBlockEnd) {
                  _logger->log_basic_block_end(CIA_read().uint64());
                }
              }
              NIA_write_simple(CIA_read() + 4);
              time_tag_start_grp1();
              instr_handler_b(get_LI(__sym27[0]));
              time_tag_finish_grp1("b");
              if (Tracing && (_trace_mode && ttBranchTaken)) {
                _logger->log_branch_end(CIA_read().uint64());
              }
              rc = esEndBasicBlock;
            }
            break;
            case 0x1: {
              // Instruction:  "bl"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"bl",instr_disasm_bl,ifBrUncond);
                }
                if (_trace_mode & ttBasicBlockEnd) {
                  _logger->log_basic_block_end(CIA_read().uint64());
                }
              }
              NIA_write_simple(CIA_read() + 4);
              time_tag_start_grp1();
              instr_handler_bl(get_LI(__sym27[0]));
              time_tag_finish_grp1("bl");
              if (Tracing && (_trace_mode && ttBranchTaken)) {
                _logger->log_branch_end(CIA_read().uint64());
              }
              rc = esEndBasicBlock;
            }
            break;
            default:
            // Decode miss.
            if (Tracing && (_trace_mode & ttInstr)) {
              _logger->log_instr(__sym27,8,"<illop>",disasm_illop,0);
            }
            decode_miss_handler(CIA_read().uint64(),__sym27[0]);
            return esEndBasicBlock;
            break;
          }
        }
        break;
        case 0x4c000000: {
          switch (__sym27[0] & 0x7ff) {
            case 0x20: {
              // Instruction:  "bclr"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"bclr",instr_disasm_bclr,ifBrUncond);
                }
                if (_trace_mode & ttBasicBlockEnd) {
                  _logger->log_basic_block_end(CIA_read().uint64());
                }
              }
              NIA_write_simple(CIA_read() + 4);
              time_tag_start_grp1();
              instr_handler_bclr(get_BO(__sym27[0]), get_BI(__sym27[0]));
              time_tag_finish_grp1("bclr");
              if (Tracing && (_trace_mode && ttBranchTaken)) {
                _logger->log_branch_end(CIA_read().uint64());
              }
              rc = esEndBasicBlock;
            }
            break;
            case 0x21: {
              // Instruction:  "bclrl"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"bclrl",instr_disasm_bclrl,ifBrUncond);
                }
                if (_trace_mode & ttBasicBlockEnd) {
                  _logger->log_basic_block_end(CIA_read().uint64());
                }
              }
              NIA_write_simple(CIA_read() + 4);
              time_tag_start_grp1();
              instr_handler_bclrl(get_BO(__sym27[0]), get_BI(__sym27[0]));
              time_tag_finish_grp1("bclrl");
              if (Tracing && (_trace_mode && ttBranchTaken)) {
                _logger->log_branch_end(CIA_read().uint64());
              }
              rc = esEndBasicBlock;
            }
            break;
            default:
            // Decode miss.
            if (Tracing && (_trace_mode & ttInstr)) {
              _logger->log_instr(__sym27,8,"<illop>",disasm_illop,0);
            }
            decode_miss_handler(CIA_read().uint64(),__sym27[0]);
            return esEndBasicBlock;
            break;
          }
        }
        break;
        case 0x54000000: {
          // Instruction:  "rlwinm"
          if (Tracing) {
            if (_trace_mode & ttInstr) {
              _logger->log_instr(__sym27,8,"rlwinm",instr_disasm_rlwinm,0);
            }
          }
          NIA_write_simple(CIA_read() + 4);
          time_tag_start_grp1();
          instr_handler_rlwinm(get_RS(__sym27[0]), get_RA(__sym27[0]), get_SH(__sym27[0]), get_MB(__sym27[0]), get_ME(__sym27[0]));
          time_tag_finish_grp1("rlwinm");
        }
        break;
        case 0x60000000: {
          // Instruction:  "ori"
          if (Tracing) {
            if (_trace_mode & ttInstr) {
              _logger->log_instr(__sym27,8,"ori",instr_disasm_ori,0);
            }
          }
          NIA_write_simple(CIA_read() + 4);
          time_tag_start_grp1();
          instr_handler_ori(get_RS(__sym27[0]), get_RA(__sym27[0]), get_UI(__sym27[0]));
          time_tag_finish_grp1("ori");
        }
        break;
        case 0x64000000: {
          // Instruction:  "oris"
          if (Tracing) {
            if (_trace_mode & ttInstr) {
              _logger->log_instr(__sym27,8,"oris",instr_disasm_oris,0);
            }
          }
          NIA_write_simple(CIA_read() + 4);
          time_tag_start_grp1();
          instr_handler_oris(get_RS(__sym27[0]), get_RA(__sym27[0]), get_UI(__sym27[0]));
          time_tag_finish_grp1("oris");
        }
        break;
        case 0x70000000: {
          // Instruction:  "andi."
          if (Tracing) {
            if (_trace_mode & ttInstr) {
              _logger->log_instr(__sym27,8,"andi.",instr_disasm_andi_,0);
            }
          }
          NIA_write_simple(CIA_read() + 4);
          time_tag_start_grp1();
          instr_handler_andi_(get_RS(__sym27[0]), get_RA(__sym27[0]), get_UI(__sym27[0]));
          time_tag_finish_grp1("andi.");
        }
        break;
        case 0x7c000000: {
          switch (__sym27[0] & 0x7fe) {
            case 0x0: {
              // Instruction:  "cmp"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"cmp",instr_disasm_cmp,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              time_tag_start_grp1();
              instr_handler_cmp(get_BF(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
              time_tag_finish_grp1("cmp");
            }
            break;
            case 0x2e: {
              // Instruction:  "lwzx"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"lwzx",instr_disasm_lwzx,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              time_tag_start_loadstores();
              instr_handler_lwzx(get_RT(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
              time_tag_finish_loadstores("lwzx");
            }
            break;
            case 0x12e: {
              // Instruction:  "stwx"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"stwx",instr_disasm_stwx,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              time_tag_start_grp2();
              instr_handler_stwx(get_RS(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
              time_tag_finish_grp2("stwx");
            }
            break;
            case 0x1d4: {
              switch (__sym27[0] & 0x1) {
                case 0x1: {
                  // Instruction:  "addme."
                  if (Tracing) {
                    if (_trace_mode & ttInstr) {
                      _logger->log_instr(__sym27,8,"addme.",instr_disasm_addme_,0);
                    }
                  }
                  NIA_write_simple(CIA_read() + 4);
                  time_tag_start_grp1();
                  instr_handler_addme_(get_RT(__sym27[0]), get_RA(__sym27[0]));
                  time_tag_finish_grp1("addme.");
                }
                break;
                default:
                 {// Instruction:  "addme"
                  if (Tracing) {
                    if (_trace_mode & ttInstr) {
                      _logger->log_instr(__sym27,8,"addme",instr_disasm_addme,0);
                    }
                  }
                  NIA_write_simple(CIA_read() + 4);
                  time_tag_start_grp1();
                  instr_handler_addme(get_RT(__sym27[0]), get_RA(__sym27[0]));
                  time_tag_finish_grp1("addme");
                }
                break;
              }
            }
            break;
            case 0x1d6: {
              // Instruction:  "mullw"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"mullw",instr_disasm_mullw,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              time_tag_start_multiplies();
              instr_handler_mullw(get_RT(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
              time_tag_finish_multiplies("mullw");
            }
            break;
            case 0x214: {
              // Instruction:  "add"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"add",instr_disasm_add,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              time_tag_start_grp1();
              instr_handler_add(get_RT(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
              time_tag_finish_grp1("add");
            }
            break;
            case 0x2a6: {
              // Instruction:  "mfspr"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"mfspr",instr_disasm_mfspr,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              time_tag_start_grp1();
              instr_handler_mfspr(get_RT(__sym27[0]), get_SPRN(__sym27[0]));
              time_tag_finish_grp1("mfspr");
            }
            break;
            case 0x32e: {
              // Instruction:  "sthx"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"sthx",instr_disasm_sthx,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              time_tag_start_loadstores();
              instr_handler_sthx(get_RS(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
              time_tag_finish_loadstores("sthx");
            }
            break;
            case 0x378: {
              // Instruction:  "or"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"or",instr_disasm_or,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              time_tag_start_grp1();
              instr_handler_or(get_RS(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
              time_tag_finish_grp1("or");
            }
            break;
            case 0x3a6: {
              // Instruction:  "mtspr"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"mtspr",instr_disasm_mtspr,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              time_tag_start_grp1();
              instr_handler_mtspr(get_RS(__sym27[0]), get_SPRN(__sym27[0]));
              time_tag_finish_grp1("mtspr");
            }
            break;
            case 0x430: {
              // Instruction:  "srw"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"srw",instr_disasm_srw,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              time_tag_start_grp1();
              instr_handler_srw(get_RS(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
              time_tag_finish_grp1("srw");
            }
            break;
            case 0x5d4: {
              // Instruction:  "addmeo"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"addmeo",instr_disasm_addmeo,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              time_tag_start_grp1();
              instr_handler_addmeo(get_RT(__sym27[0]), get_RA(__sym27[0]));
              time_tag_finish_grp1("addmeo");
            }
            break;
            case 0x614: {
              // Instruction:  "addo"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"addo",instr_disasm_addo,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              time_tag_start_grp1();
              instr_handler_addo(get_RT(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
              time_tag_finish_grp1("addo");
            }
            break;
            default:
            // Decode miss.
            if (Tracing && (_trace_mode & ttInstr)) {
              _logger->log_instr(__sym27,8,"<illop>",disasm_illop,0);
            }
            decode_miss_handler(CIA_read().uint64(),__sym27[0]);
            return esEndBasicBlock;
            break;
          }
        }
        break;
        case 0x80000000: {
          // Instruction:  "lwz"
          if (Tracing) {
            if (_trace_mode & ttInstr) {
              _logger->log_instr(__sym27,8,"lwz",instr_disasm_lwz,0);
            }
          }
          NIA_write_simple(CIA_read() + 4);
          time_tag_start_loadstores();
          instr_handler_lwz(get_RT(__sym27[0]), get_RA(__sym27[0]), get_D(__sym27[0]));
          time_tag_finish_loadstores("lwz");
        }
        break;
        case 0x84000000: {
          // Instruction:  "lwzu"
          if (Tracing) {
            if (_trace_mode & ttInstr) {
              _logger->log_instr(__sym27,8,"lwzu",instr_disasm_lwzu,0);
            }
          }
          NIA_write_simple(CIA_read() + 4);
          time_tag_start_loadstores();
          instr_handler_lwzu(get_RT(__sym27[0]), get_RA(__sym27[0]), get_D(__sym27[0]));
          time_tag_finish_loadstores("lwzu");
        }
        break;
        case 0x88000000: {
          // Instruction:  "lbz"
          if (Tracing) {
            if (_trace_mode & ttInstr) {
              _logger->log_instr(__sym27,8,"lbz",instr_disasm_lbz,0);
            }
          }
          NIA_write_simple(CIA_read() + 4);
          time_tag_start_loadstores();
          instr_handler_lbz(get_RT(__sym27[0]), get_RA(__sym27[0]), get_D(__sym27[0]));
          time_tag_finish_loadstores("lbz");
        }
        break;
        case 0x8c000000: {
          // Instruction:  "lbzu"
          if (Tracing) {
            if (_trace_mode & ttInstr) {
              _logger->log_instr(__sym27,8,"lbzu",instr_disasm_lbzu,0);
            }
          }
          NIA_write_simple(CIA_read() + 4);
          time_tag_start_loadstores();
          instr_handler_lbzu(get_RT(__sym27[0]), get_RA(__sym27[0]), get_D(__sym27[0]));
          time_tag_finish_loadstores("lbzu");
        }
        break;
        case 0x90000000: {
          // Instruction:  "stw"
          if (Tracing) {
            if (_trace_mode & ttInstr) {
              _logger->log_instr(__sym27,8,"stw",instr_disasm_stw,0);
            }
          }
          NIA_write_simple(CIA_read() + 4);
          time_tag_start_loadstores();
          instr_handler_stw(get_RS(__sym27[0]), get_RA(__sym27[0]), get_D(__sym27[0]));
          time_tag_finish_loadstores("stw");
        }
        break;
        case 0x94000000: {
          // Instruction:  "stwu"
          if (Tracing) {
            if (_trace_mode & ttInstr) {
              _logger->log_instr(__sym27,8,"stwu",instr_disasm_stwu,0);
            }
          }
          NIA_write_simple(CIA_read() + 4);
          time_tag_start_loadstores();
          instr_handler_stwu(get_RS(__sym27[0]), get_RA(__sym27[0]), get_D(__sym27[0]));
          time_tag_finish_loadstores("stwu");
        }
        break;
        case 0x98000000: {
          // Instruction:  "stb"
          if (Tracing) {
            if (_trace_mode & ttInstr) {
              _logger->log_instr(__sym27,8,"stb",instr_disasm_stb,0);
            }
          }
          NIA_write_simple(CIA_read() + 4);
          time_tag_start_loadstores();
          instr_handler_stb(get_RS(__sym27[0]), get_RA(__sym27[0]), get_D(__sym27[0]));
          time_tag_finish_loadstores("stb");
        }
        break;
        case 0xa0000000: {
          // Instruction:  "lhz"
          if (Tracing) {
            if (_trace_mode & ttInstr) {
              _logger->log_instr(__sym27,8,"lhz",instr_disasm_lhz,0);
            }
          }
          NIA_write_simple(CIA_read() + 4);
          time_tag_start_loadstores();
          instr_handler_lhz(get_RT(__sym27[0]), get_RA(__sym27[0]), get_D(__sym27[0]));
          time_tag_finish_loadstores("lhz");
        }
        break;
        case 0xb0000000: {
          // Instruction:  "sth"
          if (Tracing) {
            if (_trace_mode & ttInstr) {
              _logger->log_instr(__sym27,8,"sth",instr_disasm_sth,0);
            }
          }
          NIA_write_simple(CIA_read() + 4);
          time_tag_start_loadstores();
          instr_handler_sth(get_RS(__sym27[0]), get_RA(__sym27[0]), get_D(__sym27[0]));
          time_tag_finish_loadstores("sth");
        }
        break;
        case 0xb8000000: {
          // Instruction:  "lmw"
          if (Tracing) {
            if (_trace_mode & ttInstr) {
              _logger->log_instr(__sym27,8,"lmw",instr_disasm_lmw,0);
            }
          }
          NIA_write_simple(CIA_read() + 4);
          time_tag_start_loadstores();
          instr_handler_lmw(get_RT(__sym27[0]), get_RA(__sym27[0]), get_D(__sym27[0]));
          time_tag_finish_loadstores("lmw");
        }
        break;
        case 0xbc000000: {
          // Instruction:  "stmw"
          if (Tracing) {
            if (_trace_mode & ttInstr) {
              _logger->log_instr(__sym27,8,"stmw",instr_disasm_stmw,0);
            }
          }
          NIA_write_simple(CIA_read() + 4);
          time_tag_start_loadstores();
          instr_handler_stmw(get_RS(__sym27[0]), get_RA(__sym27[0]), get_D(__sym27[0]));
          time_tag_finish_loadstores("stmw");
        }
        break;
        default:
        // Decode miss.
        if (Tracing && (_trace_mode & ttInstr)) {
          _logger->log_instr(__sym27,8,"<illop>",disasm_illop,0);
        }
        decode_miss_handler(CIA_read().uint64(),__sym27[0]);
        return esEndBasicBlock;
        break;
      }
      return rc;
    }
    
    #endif // ADL_HEADER
    
    ProgramStatus exec_from_buffer(uint32_t *buf,unsigned n)
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      copy_to_instr_buf(__sym27,buf,1,n,0);
      try {
        ++_instr_count;
        if (Tracing && (_trace_mode & ttInstrPrefetch)) {
          _logger->log_instr_from_buffer();
        }
        {
          exec(false);
        }
        return (activeCount()) ? ACTIVE : HALT;
      }
      catch (SimInterrupt) {
        return BREAK;
      }
    }
    
    #endif // ADL_HEADER
    
    bool check_for_breakpoint(addr_t addr)
    {
      if (!_breakpoints.empty()) {
        MemPoints::const_iterator i = _breakpoints.find(addr);
        if (i != _breakpoints.end()) {
          return true;
        }
      }
      return false;
    }
    
    
    bool exec_cold_block()
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      ++_cold_runs;
      unsigned count = 0;
      while (1) {
        if (_td_counter >= _td_threshold) return true;
        ++_instr_count;
        CIA_write(NIA_read());
        if (Tracing && (_trace_mode & ttInstrPrefetch)) {
          _logger->log_instr_prefetch(CIA_read().uint64());
        }
        __sym27[0] = instr_read32(CIA_read().uint64());
        ++count;
        ExecStatus rc = exec(true);
        commit_checks();
        if (rc == esEndBasicBlock || (count >= TranslationParms::max_bb_instr_count())) {
          return  false;
        }
      }
    }
    
    #endif // ADL_HEADER
    
    
    #ifdef ADL_HEADER
    ;
    #else
    
    addr_t dc_translate_instr(addr_t &pc,DcBasicBlock *bb,unsigned count,bool extra_fetch)
    {
      switch (__sym27[0] & 0xfc000000) {
        case 0x0: {
          // Instruction:  "halt"
          _sym363(bb,pc,count);
          pc += 4;
          return false;
        }
        break;
        case 0x1c000000: {
          // Instruction:  "mulli"
          _sym465(bb,pc,count, get_RT(__sym27[0]), get_RA(__sym27[0]), get_SI(__sym27[0]));
          pc += 4;
        }
        break;
        case 0x2c000000: {
          // Instruction:  "cmpi"
          _sym345(bb,pc,count, get_BF(__sym27[0]), get_L(__sym27[0]), get_RA(__sym27[0]), get_SI(__sym27[0]));
          pc += 4;
        }
        break;
        case 0x30000000: {
          // Instruction:  "addic"
          _sym45(bb,pc,count, get_RT(__sym27[0]), get_RA(__sym27[0]), get_SI(__sym27[0]));
          pc += 4;
        }
        break;
        case 0x34000000: {
          // Instruction:  "addic."
          _sym51(bb,pc,count, get_RT(__sym27[0]), get_RA(__sym27[0]), get_SI(__sym27[0]));
          pc += 4;
        }
        break;
        case 0x38000000: {
          // Instruction:  "addi"
          _sym39(bb,pc,count, get_RT(__sym27[0]), get_RA(__sym27[0]), get_SI(__sym27[0]));
          pc += 4;
        }
        break;
        case 0x3c000000: {
          // Instruction:  "addis"
          _sym57(bb,pc,count, get_RT(__sym27[0]), get_RA(__sym27[0]), get_SI(__sym27[0]));
          pc += 4;
        }
        break;
        case 0x40000000: {
          switch (__sym27[0] & 0x3) {
            case 0x0: {
              // Instruction:  "bc"
              _sym99(bb,pc,count, get_BO(__sym27[0]), get_BI(__sym27[0]), get_BD(__sym27[0]));
              pc += 4;
              return false;
            }
            break;
            case 0x1: {
              // Instruction:  "bcl"
              _sym105(bb,pc,count, get_BO(__sym27[0]), get_BI(__sym27[0]), get_BD(__sym27[0]));
              pc += 4;
              return false;
            }
            break;
            default:
            _sym5(pc,__sym27[0]);
            pc += 4;
            return false;
            break;
          }
        }
        break;
        case 0x44000000: {
          switch (__sym27[0] & 0x7fe) {
            case 0x2: {
              // Instruction:  "sc"
              _sym507(bb,pc,count);
              pc += 4;
            }
            break;
            default:
            _sym5(pc,__sym27[0]);
            pc += 4;
            return false;
            break;
          }
        }
        break;
        case 0x48000000: {
          switch (__sym27[0] & 0x3) {
            case 0x0: {
              // Instruction:  "b"
              _sym93(bb,pc,count, get_LI(__sym27[0]));
              pc += 4;
              return false;
            }
            break;
            case 0x1: {
              // Instruction:  "bl"
              _sym231(bb,pc,count, get_LI(__sym27[0]));
              pc += 4;
              return false;
            }
            break;
            default:
            _sym5(pc,__sym27[0]);
            pc += 4;
            return false;
            break;
          }
        }
        break;
        case 0x4c000000: {
          switch (__sym27[0] & 0x7ff) {
            case 0x20: {
              // Instruction:  "bclr"
              _sym111(bb,pc,count, get_BO(__sym27[0]), get_BI(__sym27[0]));
              pc += 4;
              return false;
            }
            break;
            case 0x21: {
              // Instruction:  "bclrl"
              _sym117(bb,pc,count, get_BO(__sym27[0]), get_BI(__sym27[0]));
              pc += 4;
              return false;
            }
            break;
            default:
            _sym5(pc,__sym27[0]);
            pc += 4;
            return false;
            break;
          }
        }
        break;
        case 0x54000000: {
          // Instruction:  "rlwinm"
          _sym495(bb,pc,count, get_RS(__sym27[0]), get_RA(__sym27[0]), get_SH(__sym27[0]), get_MB(__sym27[0]), get_ME(__sym27[0]));
          pc += 4;
        }
        break;
        case 0x60000000: {
          // Instruction:  "ori"
          _sym483(bb,pc,count, get_RS(__sym27[0]), get_RA(__sym27[0]), get_UI(__sym27[0]));
          pc += 4;
        }
        break;
        case 0x64000000: {
          // Instruction:  "oris"
          _sym489(bb,pc,count, get_RS(__sym27[0]), get_RA(__sym27[0]), get_UI(__sym27[0]));
          pc += 4;
        }
        break;
        case 0x70000000: {
          // Instruction:  "andi."
          _sym87(bb,pc,count, get_RS(__sym27[0]), get_RA(__sym27[0]), get_UI(__sym27[0]));
          pc += 4;
        }
        break;
        case 0x7c000000: {
          switch (__sym27[0] & 0x7fe) {
            case 0x0: {
              // Instruction:  "cmp"
              _sym339(bb,pc,count, get_BF(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
              pc += 4;
            }
            break;
            case 0x2e: {
              // Instruction:  "lwzx"
              _sym423(bb,pc,count, get_RT(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
              pc += 4;
            }
            break;
            case 0x12e: {
              // Instruction:  "stwx"
              _sym561(bb,pc,count, get_RS(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
              pc += 4;
            }
            break;
            case 0x1d4: {
              switch (__sym27[0] & 0x1) {
                case 0x1: {
                  // Instruction:  "addme."
                  _sym69(bb,pc,count, get_RT(__sym27[0]), get_RA(__sym27[0]));
                  pc += 4;
                }
                break;
                default:
                 {// Instruction:  "addme"
                  _sym63(bb,pc,count, get_RT(__sym27[0]), get_RA(__sym27[0]));
                  pc += 4;
                }
                break;
              }
            }
            break;
            case 0x1d6: {
              // Instruction:  "mullw"
              _sym471(bb,pc,count, get_RT(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
              pc += 4;
            }
            break;
            case 0x214: {
              // Instruction:  "add"
              _sym33(bb,pc,count, get_RT(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
              pc += 4;
            }
            break;
            case 0x2a6: {
              // Instruction:  "mfspr"
              _sym435(bb,pc,count, get_RT(__sym27[0]), get_SPRN(__sym27[0]));
              pc += 4;
            }
            break;
            case 0x32e: {
              // Instruction:  "sthx"
              _sym537(bb,pc,count, get_RS(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
              pc += 4;
            }
            break;
            case 0x378: {
              // Instruction:  "or"
              _sym477(bb,pc,count, get_RS(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
              pc += 4;
            }
            break;
            case 0x3a6: {
              // Instruction:  "mtspr"
              _sym459(bb,pc,count, get_RS(__sym27[0]), get_SPRN(__sym27[0]));
              pc += 4;
            }
            break;
            case 0x430: {
              // Instruction:  "srw"
              _sym519(bb,pc,count, get_RS(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
              pc += 4;
            }
            break;
            case 0x5d4: {
              // Instruction:  "addmeo"
              _sym75(bb,pc,count, get_RT(__sym27[0]), get_RA(__sym27[0]));
              pc += 4;
            }
            break;
            case 0x614: {
              // Instruction:  "addo"
              _sym81(bb,pc,count, get_RT(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
              pc += 4;
            }
            break;
            default:
            _sym5(pc,__sym27[0]);
            pc += 4;
            return false;
            break;
          }
        }
        break;
        case 0x80000000: {
          // Instruction:  "lwz"
          _sym411(bb,pc,count, get_RT(__sym27[0]), get_RA(__sym27[0]), get_D(__sym27[0]));
          pc += 4;
        }
        break;
        case 0x84000000: {
          // Instruction:  "lwzu"
          _sym417(bb,pc,count, get_RT(__sym27[0]), get_RA(__sym27[0]), get_D(__sym27[0]));
          pc += 4;
        }
        break;
        case 0x88000000: {
          // Instruction:  "lbz"
          _sym375(bb,pc,count, get_RT(__sym27[0]), get_RA(__sym27[0]), get_D(__sym27[0]));
          pc += 4;
        }
        break;
        case 0x8c000000: {
          // Instruction:  "lbzu"
          _sym381(bb,pc,count, get_RT(__sym27[0]), get_RA(__sym27[0]), get_D(__sym27[0]));
          pc += 4;
        }
        break;
        case 0x90000000: {
          // Instruction:  "stw"
          _sym549(bb,pc,count, get_RS(__sym27[0]), get_RA(__sym27[0]), get_D(__sym27[0]));
          pc += 4;
        }
        break;
        case 0x94000000: {
          // Instruction:  "stwu"
          _sym555(bb,pc,count, get_RS(__sym27[0]), get_RA(__sym27[0]), get_D(__sym27[0]));
          pc += 4;
        }
        break;
        case 0x98000000: {
          // Instruction:  "stb"
          _sym525(bb,pc,count, get_RS(__sym27[0]), get_RA(__sym27[0]), get_D(__sym27[0]));
          pc += 4;
        }
        break;
        case 0xa0000000: {
          // Instruction:  "lhz"
          _sym387(bb,pc,count, get_RT(__sym27[0]), get_RA(__sym27[0]), get_D(__sym27[0]));
          pc += 4;
        }
        break;
        case 0xb0000000: {
          // Instruction:  "sth"
          _sym531(bb,pc,count, get_RS(__sym27[0]), get_RA(__sym27[0]), get_D(__sym27[0]));
          pc += 4;
        }
        break;
        case 0xb8000000: {
          // Instruction:  "lmw"
          _sym405(bb,pc,count, get_RT(__sym27[0]), get_RA(__sym27[0]), get_D(__sym27[0]));
          pc += 4;
        }
        break;
        case 0xbc000000: {
          // Instruction:  "stmw"
          _sym543(bb,pc,count, get_RS(__sym27[0]), get_RA(__sym27[0]), get_D(__sym27[0]));
          pc += 4;
        }
        break;
        default:
        _sym5(pc,__sym27[0]);
        pc += 4;
        return false;
        break;
      }
      return true;
    }
    
    void translate_dc(addr_t addr,DcBasicBlock *bb)
    {
      ++_dc_compiles;
      addr_t new_addr = addr;
      unsigned count = 0;
      _dc_decode_cache.start_bb(bb,check_for_breakpoint(addr));
      while (1) {
        __sym27[0] = instr_read32_nocache(new_addr);
        bb->set_end_ra(_last_instr_ra);
        if (!dc_translate_instr(new_addr,bb,count,true)) break;
        count++;
        if (check_for_breakpoint(new_addr)) {
          bb->set_end_ra(instrReadTranslate(new_addr));
          _dc_decode_cache.add_brkpt_stub(bb);
          return;
        }
        if ( (count >= TranslationParms::max_bb_instr_count())) {
          _dc_decode_cache.add_br_stub(bb);
          return;
        }
      }
    }
    
    void addDcBreakpoint(addr_t ea)
    {
      _dc_bb_hash.invalidate_ea(ea,_dc_decode_cache);
    }
    
    void removeDcBreakpoint(addr_t ea)
    {
      _dc_bb_hash.invalidate_ea(ea,_dc_decode_cache);
    }
    
    bool decode_cache_full() {
      if (_dc_decode_cache.full(TranslationParms::max_bb_instr_count())) {
        _dc_bb_hash.reset(_dc_decode_cache,true);
        return true;
      }
      return false;
    }
    
    ProgramStatus simple_run_dc()
    {
      try {
        enable_dmi_caches();
        DcBasicBlock *bb = 0;
        DcTranslate dct;
        // The sim kernel runs until the core is inactive.  Translation occurs as a side-effect of lookup.
         // A null-return on a block lookup means that we got a cold-block and it was interpreted, so just loop
        // and continue.
        while (_active) {
          {
            if (_td_counter >= _td_threshold) { return ACTIVE; };
            addr_t nia = NIA_read().uint64();
            bb = _dc_bb_hash.get_bb(dct,nia,0,TranslationParms::dc_bb_hot_threshold());
            if (!bb) {
              if (exec_cold_block()) return ACTIVE;
              continue;
            }
            DcDecodeFunc dcf = (DcDecodeFunc)(JitLastCode+1);
            DcDecodePair dcp = bb->get_pair();
            DcDecodeItem *dci = _dc_decode_cache_next = dcp.second;
            while (dcf >= (DcDecodeFunc)JitContCode) {
              if ( (dci = _dc_decode_cache_next) != 0) {
                dcf = dci[0].func();
                dcf = (DcDecodeFunc)(dcf)(this,&dci[0]);
                if (dcf > (DcDecodeFunc)JitLastCode) {
                  dcf = (DcDecodeFunc)(dcf)(this,&dci[1]);
                  if (dcf > (DcDecodeFunc)JitLastCode) {
                    dcf = (DcDecodeFunc)(dcf)(this,&dci[2]);
                    if (dcf > (DcDecodeFunc)JitLastCode) {
                      dcf = (DcDecodeFunc)(dcf)(this,&dci[3]);
                      if (dcf > (DcDecodeFunc)JitLastCode) {
                        dcf = (DcDecodeFunc)(dcf)(this,&dci[4]);
                        if (dcf > (DcDecodeFunc)JitLastCode) {
                          dcf = (DcDecodeFunc)(dcf)(this,&dci[5]);
                          if (dcf > (DcDecodeFunc)JitLastCode) {
                            dcf = (DcDecodeFunc)(dcf)(this,&dci[6]);
                            if (dcf > (DcDecodeFunc)JitLastCode) {
                              dcf = (DcDecodeFunc)(dcf)(this,&dci[7]);
                              if (dcf > (DcDecodeFunc)JitLastCode) {
                                dcf = (DcDecodeFunc)(dcf)(this,&dci[8]);
                                if (dcf > (DcDecodeFunc)JitLastCode) {
                                  dcf = (DcDecodeFunc)(dcf)(this,&dci[9]);
                                  if (dcf > (DcDecodeFunc)JitLastCode) {
                                    dcf = (DcDecodeFunc)(dcf)(this,&dci[10]);
                                    if (dcf > (DcDecodeFunc)JitLastCode) {
                                      dcf = (DcDecodeFunc)(dcf)(this,&dci[11]);
                                      if (dcf > (DcDecodeFunc)JitLastCode) {
                                        dcf = (DcDecodeFunc)(dcf)(this,&dci[12]);
                                        if (dcf > (DcDecodeFunc)JitLastCode) {
                                          dcf = (DcDecodeFunc)(dcf)(this,&dci[13]);
                                          if (dcf > (DcDecodeFunc)JitLastCode) {
                                            dcf = (DcDecodeFunc)(dcf)(this,&dci[14]);
                                            if (dcf > (DcDecodeFunc)JitLastCode) {
                                              dcf = (DcDecodeFunc)(dcf)(this,&dci[15]);
                                              if (dcf > (DcDecodeFunc)JitLastCode) {
                                                dcf = (DcDecodeFunc)(dcf)(this,&dci[16]);
                                                if (dcf > (DcDecodeFunc)JitLastCode) {
                                                  dcf = (DcDecodeFunc)(dcf)(this,&dci[17]);
                                                  if (dcf > (DcDecodeFunc)JitLastCode) {
                                                    dcf = (DcDecodeFunc)(dcf)(this,&dci[18]);
                                                    if (dcf > (DcDecodeFunc)JitLastCode) {
                                                      dcf = (DcDecodeFunc)(dcf)(this,&dci[19]);
                                                      if (dcf > (DcDecodeFunc)JitLastCode) {
                                                        dcf = (DcDecodeFunc)(dcf)(this,&dci[20]);
                                                      }
                                                    }
                                                  }
                                                }
                                              }
                                            }
                                          }
                                        }
                                      }
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
              if ( (dci = _dc_decode_cache_next) != 0) {
                dcf = dci[0].func();
                dcf = (DcDecodeFunc)(dcf)(this,&dci[0]);
                if (dcf > (DcDecodeFunc)JitLastCode) {
                  dcf = (DcDecodeFunc)(dcf)(this,&dci[1]);
                  if (dcf > (DcDecodeFunc)JitLastCode) {
                    dcf = (DcDecodeFunc)(dcf)(this,&dci[2]);
                    if (dcf > (DcDecodeFunc)JitLastCode) {
                      dcf = (DcDecodeFunc)(dcf)(this,&dci[3]);
                      if (dcf > (DcDecodeFunc)JitLastCode) {
                        dcf = (DcDecodeFunc)(dcf)(this,&dci[4]);
                        if (dcf > (DcDecodeFunc)JitLastCode) {
                          dcf = (DcDecodeFunc)(dcf)(this,&dci[5]);
                          if (dcf > (DcDecodeFunc)JitLastCode) {
                            dcf = (DcDecodeFunc)(dcf)(this,&dci[6]);
                            if (dcf > (DcDecodeFunc)JitLastCode) {
                              dcf = (DcDecodeFunc)(dcf)(this,&dci[7]);
                              if (dcf > (DcDecodeFunc)JitLastCode) {
                                dcf = (DcDecodeFunc)(dcf)(this,&dci[8]);
                                if (dcf > (DcDecodeFunc)JitLastCode) {
                                  dcf = (DcDecodeFunc)(dcf)(this,&dci[9]);
                                  if (dcf > (DcDecodeFunc)JitLastCode) {
                                    dcf = (DcDecodeFunc)(dcf)(this,&dci[10]);
                                    if (dcf > (DcDecodeFunc)JitLastCode) {
                                      dcf = (DcDecodeFunc)(dcf)(this,&dci[11]);
                                      if (dcf > (DcDecodeFunc)JitLastCode) {
                                        dcf = (DcDecodeFunc)(dcf)(this,&dci[12]);
                                        if (dcf > (DcDecodeFunc)JitLastCode) {
                                          dcf = (DcDecodeFunc)(dcf)(this,&dci[13]);
                                          if (dcf > (DcDecodeFunc)JitLastCode) {
                                            dcf = (DcDecodeFunc)(dcf)(this,&dci[14]);
                                            if (dcf > (DcDecodeFunc)JitLastCode) {
                                              dcf = (DcDecodeFunc)(dcf)(this,&dci[15]);
                                              if (dcf > (DcDecodeFunc)JitLastCode) {
                                                dcf = (DcDecodeFunc)(dcf)(this,&dci[16]);
                                                if (dcf > (DcDecodeFunc)JitLastCode) {
                                                  dcf = (DcDecodeFunc)(dcf)(this,&dci[17]);
                                                  if (dcf > (DcDecodeFunc)JitLastCode) {
                                                    dcf = (DcDecodeFunc)(dcf)(this,&dci[18]);
                                                    if (dcf > (DcDecodeFunc)JitLastCode) {
                                                      dcf = (DcDecodeFunc)(dcf)(this,&dci[19]);
                                                      if (dcf > (DcDecodeFunc)JitLastCode) {
                                                        dcf = (DcDecodeFunc)(dcf)(this,&dci[20]);
                                                      }
                                                    }
                                                  }
                                                }
                                              }
                                            }
                                          }
                                        }
                                      }
                                    }
                                  }
                                }
                              }
                            }
                          }
                        }
                      }
                    }
                  }
                }
              }
            }
            switch ((long)dcf) {
              case JitNopCode:
              commit_checks();
              break;
              case JitRetCode:
              commit_checks();
              if (InstrMax && (_instr_count > InstrMax)) {
                halt();
                return HALT;
              }
              return ACTIVE;
              case JitBreakPtCode: {
                addr_t ea = NIA_read().uint64();
                MemPoints::iterator i = _breakpoints.find(ea);
                assert(i != _breakpoints.end());
                issueBreakpoint(ea,i);
              } break;
              default: ;
            }
            ++_dc_escapes;
          }
        }
        disable_dmi_caches();
      }
      catch (SimInterrupt) {
        disable_dmi_caches();
        throw;
      }
      return HALT;
    }
    
    ProgramStatus run_dc()
    {
      try {
        return simple_run_dc();
      }
      catch (SimInterrupt) {
        return BREAK;
      }
    }
    #endif // ADL_HEADER
    
    unsigned get_sim_stat(const std::string &parm) const
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      if (parm == "dc_compiles") {
        return _dc_compiles;
      }
      if (parm == "cold_runs") {
        return _cold_runs;
      }
      if (parm == "dc_escapes") {
        return _dc_escapes;
      }
      if (parm == "dc_bb_count") {
        return _dc_bb_hash.size();
      }
      return IssCore::get_sim_stat(parm);
    }
    #endif // ADL_HEADER
    
    bool isTimeTagged() const { return true; };
    
    void addLatency(unsigned l) { _latency += l; };
    
    ttime_t finalTime() const { return _final_time; };
    
    virtual bool has_td () const { return true; };
    virtual uint64_t  set_td_counter(uint64_t x) { uint64_t o = _td_counter; _td_counter = x; return o; }
    
    virtual unsigned  set_td_increment(unsigned x) { unsigned o = _td_increment; _td_increment = x; return o; }
    
    virtual uint64_t  set_td_threshold(uint64_t x = (uint64_t)-1) { uint64_t o = _td_threshold; _td_threshold = x; return o; }
    
    virtual void      cond_set_td_threshold(uint64_t ct,uint64_t nt) { if (_td_threshold <= ct) _td_threshold = nt; }
    
    virtual uint64_t  get_td_counter() const { return _td_counter; };
    
    virtual uint64_t *get_td_counter_ptr() { return &_td_counter; };
    
    virtual unsigned  get_td_increment() const { return _td_increment; };
    
    virtual uint64_t  get_td_threshold() const { return _td_threshold; };
    
    uint64_t getTDCounter() const { return _td_counter; };
    
    void simple_step()
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      simulate();
    }
    #endif // ADL_HEADER
    
    ProgramStatus simple_run()
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      while (activeCount()) {
        if (_td_counter >= _td_threshold) { return ACTIVE; };
        simulate();
      }
      return HALT;
    }
    #endif // ADL_HEADER
    
    ProgramStatus run()
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      try {
        return simple_run();
      }
      catch (SimInterrupt) {
        return BREAK;
      }
    }
    #endif // ADL_HEADER
    
    bool has_sim_type(SimType type) const
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      switch (type) {
        case SimNormal:
        return true;
        case SimDc:
        return true;
        case SimJit:
        return false;
      }
      return false;
    }
    
    #endif // ADL_HEADER
    
    ProgramStatus run(SimType type)
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      switch (type) {
        case SimDc: {
          ProgramStatus ps = run_dc();
          DCDBG(std::cerr << "DC Escapes           :  " << std::dec << _dc_escapes << '\n');
          DCDBG(std::cerr << "DC Compiles          :  " << std::dec << _dc_compiles << '\n');
          DCDBG(std::cerr << "DC Cold Runs         :  " << std::dec << _cold_runs << '\n');
          DCDBG(std::cerr << "DC EA Hash Collisions:  " << std::dec << _dc_bb_hash.ea_hash_collisions() << '\n');
          return ps;
        }
        case SimJit: {
          ProgramStatus ps = run_jit();
          return ps;
        }
        default:
        return run();
      }
    }
    #endif // ADL_HEADER
    
    ProgramStatus stepn(unsigned n)
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      try {
        for (unsigned i = 0; i != n; ++i) {
          simulate();
        }
        return (activeCount()) ? ACTIVE : HALT;
      }
      catch (SimInterrupt) {
        return BREAK;
      }
    }
    #endif // ADL_HEADER
    
    ProgramStatus step()
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      try {
        simulate();
        return (activeCount()) ? ACTIVE : HALT;
      }
      catch (SimInterrupt) {
        return BREAK;
      }
    }
    #endif // ADL_HEADER
    
    struct MemPoint {
      unsigned _handle;
      bool     _is_tmp;
      PtBase  *_payload;
      MemPoint() : _handle(0), _is_tmp(false), _payload(0) {};
      MemPoint(unsigned h,bool t,PtBase *p) : _handle(h), _is_tmp(t), _payload(p) {}
    };
    
    typedef _STDEXT hash_map<addr_t,MemPoint> MemPoints;
    
    typedef _STDEXT hash_map<addr_t,unsigned> Reserves;
    
    void clear_dmi_read_reserved() {
      _dmi_read_reserves.clear();
    }
    
    bool check_dmi_read_reserved(addr_t ea) {
      return _dmi_read_reserves.count(ea & DmiCache::CompMask);
    }
    
    void add_dmi_read_reserved(addr_t ea) {
      _dmi_read_reserves[ea & DmiCache::CompMask]++;
    }
    
    void clear_dmi_read_reserved(addr_t ea) {
      Reserves::iterator i = _dmi_read_reserves.find(ea & DmiCache::CompMask);
      if (i != _dmi_read_reserves.end()) {
        i->second--;
        if (i->second == 0) {
          _dmi_read_reserves.erase(i);
        }
      }
    }
    
    void clear_dmi_write_reserved() {
      _dmi_write_reserves.clear();
    }
    
    bool check_dmi_write_reserved(addr_t ea) {
      return _dmi_write_reserves.count(ea & DmiCache::CompMask);
    }
    
    void add_dmi_write_reserved(addr_t ea) {
      _dmi_write_reserves[ea & DmiCache::CompMask]++;
    }
    
    void clear_dmi_write_reserved(addr_t ea) {
      Reserves::iterator i = _dmi_write_reserves.find(ea & DmiCache::CompMask);
      if (i != _dmi_write_reserves.end()) {
        i->second--;
        if (i->second == 0) {
          _dmi_write_reserves.erase(i);
        }
      }
    }
    
    void issueBreakpoint(addr_t ea,MemPoints::iterator b_iter) {
      MemPoint tmp = b_iter->second;
      if (tmp._is_tmp) { _breakpoints.erase(b_iter); }
      _logger->log_breakpoint(ea,tmp._handle,tmp._payload);
    }
    
    #ifndef ADL_HEADER
    
    void clearBreakpoints() {
      _breakpoints.clear();
      _bp_counter = 0;
    };
    
    void clearWatchpoints() {
      _read_watchpoints.clear();
      _write_watchpoints.clear();
      clear_dmi_read_reserved();
      clear_dmi_write_reserved();
      _wp_counter = 0;
    };
    
    unsigned setBreakpointInternal(addr_t a,bool is_tmp,PtBase *payload) {
      unsigned index = _bp_counter++;
      _breakpoints[a] = MemPoint(index,is_tmp,payload);
      addDcBreakpoint(a);
      return index;
    }
    
    bool clearPointsByAddr(addr_t addr, MemPoints &points,bool is_bp,bool is_read) {
      MemPoints::iterator iter;
      if ((iter = points.find(addr)) != points.end()) {
        if (is_bp) {
          removeDcBreakpoint(iter->first);
        }
        if (!is_bp) {
          if (is_read) {
            clear_dmi_read_reserved(iter->first);
          } else {
            clear_dmi_write_reserved(iter->first);
          }
        }
        points.erase(iter);
        return true;
      }
      return false;
    }
    
    bool clearPoint(MemPoints &points,unsigned h,bool is_bp,bool is_read) {
      for (MemPoints::iterator iter = points.begin(); iter != points.end(); ) {
        if (iter->second._handle == h) {
          if (is_bp) {
            removeDcBreakpoint(iter->first);
          }
          if (!is_bp) {
            if (is_read) {
              clear_dmi_read_reserved(iter->first);
            } else {
              clear_dmi_write_reserved(iter->first);
            }
          }
          points.erase(iter);
          return true;
        } else {
          ++iter;
        }
      }
      return false;
    }
    
    #endif // ADL_HEADER
    
    unsigned setBreakpoint(addr_t a,PtBase *payload = 0)
    #ifdef ADL_HEADER
    ;
    #else
    {
      return setBreakpointInternal(a,false,payload);
    }
    #endif // ADL_HEADER
    
    unsigned setTmpBreakpoint(addr_t a,PtBase *payload = 0)
    #ifdef ADL_HEADER
    ;
    #else
    {
      return setBreakpointInternal(a,true,payload);
    }
    #endif // ADL_HEADER
    
    bool clearBreakpointByAddr(addr_t addr)
    #ifdef ADL_HEADER
    ;
    #else
    {
      return clearPointsByAddr(addr, _breakpoints,true,false);
    }
    #endif // ADL_HEADER
    
    bool clearWatchpointByAddr(addr_t addr,WatchType type)
    #ifdef ADL_HEADER
    ;
    #else
    {
      switch (type) {
        case wpRead:
        return clearPointsByAddr(addr,_read_watchpoints,false,true);
        case wpWrite:
        return clearPointsByAddr(addr,_write_watchpoints,false,false);
        case wpAccess: {
          bool r = clearPointsByAddr(addr,_read_watchpoints,false,true);
          bool w = clearPointsByAddr(addr,_write_watchpoints,false,false);
          return r || w;
        }
        default:
        return false;
      }
    }
    #endif // ADL_HEADER
    
    unsigned setWatchpoint(addr_t a,WatchType type,PtBase *payload = 0)
    #ifdef ADL_HEADER
    ;
    #else
    {
      WatchpointsEnabled = true;
      unsigned index = _wp_counter++;
      if (type & wpRead) {
        _read_watchpoints[a & ~WordMask] = MemPoint(index,false,payload);
        add_dmi_read_reserved(a);
      }
      if (type & wpWrite) {
        _write_watchpoints[a & ~WordMask] = MemPoint(index,false,payload);
        add_dmi_write_reserved(a);
      }
      return index;
    }
    #endif // ADL_HEADER
    
    bool clearBreakpoint(unsigned h)
    #ifdef ADL_HEADER
    ;
    #else
    {
      return clearPoint(_breakpoints,h,true,false);
    }
    #endif // ADL_HEADER
    
    bool clearWatchpoint(unsigned h)
    #ifdef ADL_HEADER
    ;
    #else
    {
      bool rw = clearPoint(_read_watchpoints,h,false,true);
      bool ww = clearPoint(_write_watchpoints,h,false,false);
      return (rw || ww);
    }
    #endif // ADL_HEADER
    
    void setExtInstrFlag(unsigned) {};
    
    unsigned getExtInstrFlag() { return 0; };
    
    inline void halt()     {
      set_active_count(0);
    };
    
    inline void activate() { if (!_active) {
        _active = true;
      }
    };
    
    inline bool active() const { return _active; };
    
    virtual uint64_t instr_count() const { return _instr_count; };
    
    virtual void decr_instr_count(uint64_t d) { _instr_count -= d; };
    
    virtual bool isBigEndian() const { return 1; };
    
    void reset_internal(bool init_reset,bool por_reset,bool log)
    {
      _lastRealAddr = 0;
      _lastEffectiveAddr = 0;
      _dmi_mem_write.reset();
      _dmi_mem_read.reset();
      _last_instr_ra = 0;
      resetRegs(por_reset,log);
      _dc_bb_hash.reset(_dc_decode_cache,true);
      _dc_decode_cache.reset();
      _dc_decode_cache_next = 0;
      _dc_escapes = 0;
      for(int i=0;i<1;++i){__sym27[i]=0;};
      _Decode_stage = 0;
      _Exec_stage = 0;
      _ExecMul_stage = 0;
      _Fetch_stage = 0;
      _Mem_stage = 0;
      _Writeback_stage = 0;
      _compl_time = 0;
      _dc_compiles = 0;
      _cold_runs = 0;
      _active = true;
      TimeTaggedBase::reset();
      checkWatches();
    };
    
    void reset () {
      uint64_t of = clear_tracing();
      reset_internal(false,true,false);
      set_tracing(of);
    }
    
    void resetCore() {
      reset_internal(false,false,true);
    }
    
    void resetInit() {
      reset_internal(true,true,false);
    }
    
    addr_t _lastRealAddr;
    addr_t _lastEffectiveAddr;
    DmiCache _dmi_mem_write;
    DmiCache _dmi_mem_read;
    unsigned _read_latency;
    unsigned _write_latency;
    addr_t _last_instr_ra;
    CIA_t _CIA_Reg;
    CR_t _CR_Reg;
    CTR_t _CTR_Reg;
    HID0_t _HID0_Reg;
    HID1_t _HID1_Reg;
    LR_t _LR_Reg;
    NIA_t _NIA_Reg;
    XER_t _XER_Reg;
    GPR_t _GPR_Reg;
    SPR_t _SPR_Reg;
    DcDecodeItem     _dc_decode_cache_br_stub;
    DcDecodeItem     _dc_decode_cache_brkpt_stub;
    DcDecodeCache    _dc_decode_cache;
    DcBasicBlockHash _dc_bb_hash;
    DcDecodeItem    *_dc_decode_cache_next;
    unsigned _dc_escapes;
    ActiveCallback *_active_callback;;
    mutable uint32_t __sym27[1];
    ttime_t _Decode_stage;
    ttime_t _Exec_stage;
    ttime_t _ExecMul_stage;
    ttime_t _Fetch_stage;
    ttime_t _Mem_stage;
    ttime_t _Writeback_stage;
    ttime_t _compl_time;
    SysCallExtHandler *_schandler;
    unsigned _dc_compiles;
    unsigned _cold_runs;
    MemPoints _breakpoints;
    MemPoints _read_watchpoints;
    MemPoints _write_watchpoints;
    unsigned _bp_counter;
    unsigned _wp_counter;
    Reserves _dmi_read_reserves;
    Reserves _dmi_write_reserves;
    
    P(const std::string &__n, const IssNode *parent, unsigned &id, unsigned child_id = 0) : 
    IssCore(__n,"P",parent,id,child_id), 
    _lastRealAddr(0), 
    _lastEffectiveAddr(0), 
    _read_latency(0), 
    _write_latency(0), 
    _last_instr_ra(0), 
    _dc_decode_cache_br_stub(dc_decode_cache_branch_trampoline), 
    _dc_decode_cache_brkpt_stub(dc_decode_cache_breakpoint_trampoline), 
    _dc_decode_cache(&_dc_decode_cache_br_stub,&_dc_decode_cache_brkpt_stub,TranslationParms::dc_buffer_size()), 
    _dc_bb_hash(*this), 
    _dc_decode_cache_next(0), 
    _dc_escapes(0), 
    _active_callback(0), 
    _Decode_stage(0), 
    _Exec_stage(0), 
    _ExecMul_stage(0), 
    _Fetch_stage(0), 
    _Mem_stage(0), 
    _Writeback_stage(0), 
    _compl_time(0), 
    _schandler(0), 
    _dc_compiles(0), 
    _cold_runs(0), 
    _bp_counter(0), 
    _wp_counter(0)
    {
      resetInit();
    };
    
    ~P()
    {
    }
    
    virtual const char *getCoreName() const { return adliss::getCoreName(); }
    
    virtual const char *getModelFileName() const { return adliss::getModelFileName(); }
    
    virtual const adl::AttrData &getAttrData() const { return adliss::getAttrData(); }
    
    virtual const char *getModelPreamble() const { return adliss::getModelPreamble(); }
    
    virtual void resetGlobal() { adliss::resetGlobal(); }
    
    virtual void setMemorySize(unsigned size) { adliss::setMemorySize(size); }
    
  };
  
  #ifndef ADL_HEADER
  const char* getModelFileName() {
    static const char* fname = "ttmod1";
    return fname;
  }
  
  const AttrData &getAttrData() {
    static std::pair<const std::string,int> attr_inits[11] = {
      std::make_pair("cia",0),
      std::make_pair("debug",10),
      std::make_pair("doc_hidden",2),
      std::make_pair("doc_no_code",3),
      std::make_pair("doc_no_expand_exprs",4),
      std::make_pair("load",7),
      std::make_pair("nia",1),
      std::make_pair("other",5),
      std::make_pair("prefix",6),
      std::make_pair("privileged",9),
      std::make_pair("store",8),
    };
    static AttrData attr_hash(&attr_inits[0],&attr_inits[11]);
    return attr_hash;
  }
  
  #ifndef ADL_HEADER
  static bool dummy ATTRIBUTE_UNUSED = log_usage("ttmod1","2018.2.28",0,"model","make-iss");
  #endif
  
  IssNode *createTopLevelNode(unsigned &id)
  {
    return new P("P",0,id,0);
  }
  
  const char *getCoreName()
  {
    static const char* tmpName = "P";
    return tmpName;
  }
  
  void resetGlobal()
  {
    WatchpointsEnabled = false;
    resetMemory();
  }
  
  void setMemorySize(unsigned size)
  {
    GlobalMemory.set_mem_size(size);
  }
  
  static const char *ModelPreambleData = "";
  
  const char *getModelPreamble()
  {
    return ModelPreambleData;
  }
  
  #endif // ADL_HEADER
  
}

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

// The global memory object.


// Use the mem_read and mem_write routines to access this object from 
// other translation units, since those functions
// are not templated.

// Include the default implementation

#include "iss/DefaultMem.h"

namespace adliss {
  
  const char *getCoreName();
  const char *getModelFileName();
  const adl::AttrData &getAttrData();
  const char *getModelPreamble();
  void resetGlobal();
  void setMemorySize(unsigned size);
  
  
  struct P : public IssCore, public InstrCountBase {
    
    
    enum Exception {
      None,
      External = 0x1,
      Program = 0x2,
      dec = 0x4,
      pcex = 0x8,
    };
    
    enum ExceptionIndex {
      __None_Index,
      __External_Index,
      __Program_Index,
      __dec_Index,
      __pcex_Index,
    };
    
    // Fields for exception Program
    struct Program_t : public ExBase {
      bits<32> ESR;
      Program_t()
       : 
      ESR(bits<32>(0)){}
      void reset() {
        ESR = bits<32>(0);
      }
    };
    
    static ExceptionInfos initExceptionInfo() {
      ExceptionInfos info;
      info.push_back(ExceptionInfo());
      info.back()._name = "External";
      info.back()._id = External;
      info.back()._flag = 1;
      info.back()._priority = 0;
      info.back()._sensitivity = LevelSensitive;
      info.back().finalize();
      info.push_back(ExceptionInfo());
      info.back()._name = "Program";
      info.back()._id = Program;
      info.back()._flag = 2;
      info.back()._priority = 1;
      info.back()._sensitivity = EdgeSensitive;
      info.back().finalize();
      info.push_back(ExceptionInfo());
      info.back()._name = "dec";
      info.back()._id = dec;
      info.back()._flag = 4;
      info.back()._priority = 0;
      info.back()._sensitivity = EdgeSensitive;
      info.back()._attrs.push_back(std::make_pair(12,Attrs::Params()));
      info.back().finalize();
      info.push_back(ExceptionInfo());
      info.back()._name = "pcex";
      info.back()._id = pcex;
      info.back()._flag = 8;
      info.back()._priority = 0;
      info.back()._sensitivity = EdgeSensitive;
      info.back().finalize();
      sort_info(info);
      return info;
    }
    
    static const ExceptionInfos &baseExceptionInfo() {
      static ExceptionInfos info = initExceptionInfo();
      return info;
    }
    
    const ExceptionInfos &getExceptionInfo() const {
      return baseExceptionInfo();
    }
    
    void raiseException(Exception _sym648) ATTRIBUTE_USED
    #ifdef ADL_JIT_COMPILE
    ;
    #else
    {
      switch (_sym648) {
        case Program:
        _Program_data.reset();
        default:
        break;
      }
      throw _sym648;
    }
    #endif // ADL_JIT_COMPILE
    
    
    
    void genExceptionImm(unsigned id) ATTRIBUTE_USED
    #ifdef ADL_JIT_COMPILE
    ;
    #else
    {
      Exception _sym648 = (Exception)id;
      switch (_sym648) {
        case Program:
        _Program_data.reset();
        default:
        break;
      }
      throw _sym648;
    }
    #endif // ADL_JIT_COMPILE
    
    
    
    void raiseException(const Program_t& x) ATTRIBUTE_USED
    #ifdef ADL_JIT_COMPILE
    ;
    #else
    {
      _Program_data = x;
      throw Program;
    }
    #endif // ADL_JIT_COMPILE
    
    
    
    void setException(const Program_t& x) {
      _Program_data = x;
      __exception_flag |= Program;
    }
    
    void setException(Exception e)
    {
      __exception_flag |= (unsigned)e;
    }
    
    unsigned getExceptionFlag()
    {
      return __exception_flag;
    }
    
    unsigned getExceptionFlag() const
    {
      return __exception_flag;
    }
    
    void clearExceptionFlag()
    {
      __exception_flag = 0;
    }
    
    bool cancelExceptions(unsigned flag)
    {
      bool r = __exception_flag & flag;
      __exception_flag &= ~flag;
      r = r || __pending_exception_flag & flag;
      __pending_exception_flag &= ~flag;
      return r;
    }
    
    void setPendingExceptions(unsigned flag)
    {
      __pending_exception_flag |= flag;
    }
    
    bool hasCurrentPendingExceptions()
    {
      return (__exception_flag & __pending_exception_flag);
    }
    
    bool hasPendingExceptions()
    {
      return __pending_exception_flag;
    }
    
    bool hasNonPendingExceptions()
    {
      return __exception_flag & ~__pending_exception_flag;
    }
    
    void updateLevelSensitiveExceptions()
    {
      if (__pending_exception_flag) {
        setExternException(__pending_exception_flag);
      }
    }
    
    void clearEdgeSensitivePendingExceptions()
    {
      cancelExceptions(0xe);
    }
    
    // Enable predicate for External exception.
    bool _sym650()
    {
          return ( MSR_EE_read() == 1 ) ;
    }
    
    // Watch function for External exception.
    void _sym651() {
      if (__pending_exception_flag & External) {
        __pending_exception_flag &= ~External;
        setException(External);
      }
    }
    
    // Exception handler for External exception.
    void _sym649()
    {
      {
             SRR0_write(  CIA_read()  )  ;
             SRR1_write(  MSR_read()  )  ;
             MSR_write( 0 )  ;
             NIA_write_simple(  concat ( IVPR_read() .get<16>(0,15) , IVOR4_read() .get<16>(16,31) )  )  ;
             CIA_write(  NIA_read()  )  ;
      }
    }
    
    // Exception handler for Program exception.
    void _sym652(const Program_t & prog)
    {
      {
             SRR0_write(  CIA_read()  )  ;
             SRR1_write(  MSR_read()  )  ;
             MSR_write( 0 )  ;
             ESR_write(  prog . ESR  )  ;
             NIA_write_simple(  concat ( IVPR_read() .get<16>(0,15) , IVOR6_read() .get<16>(16,31) )  )  ;
             CIA_write(  NIA_read()  )  ;
      }
    }
    
    // Exception handler for dec exception.
    void _sym653()
    {
      {
             CSRR0_write(  NIA_read()  )  ;
             CSRR1_write(  MSR_read()  )  ;
             MSR_write( 0 )  ;
             NIA_write_simple(  concat ( IVPR_read() .get<16>(0,15) , IVOR10_read() .get<16>(16,31) )  )  ;
      }
    }
    
    // Exception handler for pcex exception.
    void _sym654()
    {
      {
             SRR0_write(  CIA_read()  )  ;
             SRR1_write(  MSR_read()  )  ;
             MSR_write( 0 )  ;
             NIA_write_simple(  concat ( IVPR_read() .get<16>(0,15) , IVOR7_read() .get<16>(16,31) )  )  ;
      }
    }
    
    void pre_exception_handler(Exception e)
    {
          test_pre_exception ( e ) ;
    }
    
    void post_exception_handler(Exception e)
    {
           POSTEXCPT_write(  POSTEXCPT_read() + 1  )  ;
    }
    
    ExHandle getExceptionData(unsigned exception_flag) {
      ExHandle eh;
      if (exception_flag) {
        eh.alloc(4);
      }
      if (exception_flag & Program) {
        eh()._items[__Program_Index] = new Program_t(_Program_data);
      }
      return eh;
    }
    
    void genExceptionsInternal(unsigned exception_flag) {
      try {
        if (exception_flag & Program) {
          if (Tracing && adl_unlikely(_trace_mode & ttException)) {
            _logger->log_exception(Program,true,"Program");
          }
          pre_exception_handler(Program);
          _sym652(_Program_data);
          if (Tracing && adl_unlikely(_trace_mode & ttException)) {
            _logger->log_exception(Program,false,"Program");
          }
          post_exception_handler(Program);
        }
        if (exception_flag & External) {
          if (_sym650()) {
            if (Tracing && adl_unlikely(_trace_mode & ttException)) {
              _logger->log_exception(External,true,"External");
            }
            pre_exception_handler(External);
            _sym649();
            if (Tracing && adl_unlikely(_trace_mode & ttException)) {
              _logger->log_exception(External,false,"External");
            }
            post_exception_handler(External);
          }
          else {
            __pending_exception_flag |= External;
          }
        }
        if (exception_flag & dec) {
          if (Tracing && adl_unlikely(_trace_mode & ttException)) {
            _logger->log_exception(dec,true,"dec");
          }
          pre_exception_handler(dec);
          _sym653();
          if (Tracing && adl_unlikely(_trace_mode & ttException)) {
            _logger->log_exception(dec,false,"dec");
          }
          post_exception_handler(dec);
        }
        if (exception_flag & pcex) {
          if (Tracing && adl_unlikely(_trace_mode & ttException)) {
            _logger->log_exception(pcex,true,"pcex");
          }
          pre_exception_handler(pcex);
          _sym654();
          if (Tracing && adl_unlikely(_trace_mode & ttException)) {
            _logger->log_exception(pcex,false,"pcex");
          }
          post_exception_handler(pcex);
        }
      }
      catch (Exception &e) {
        genExceptionsInternal(e);
      }
    }
    
    void genExceptions(unsigned exception_flag) {
      genExceptionsInternal(exception_flag);
    }
    
    void setExternException(unsigned exception_flag) {
      if (exception_flag & Program) {
        __exception_flag |= exception_flag;
      }
      if (exception_flag & External) {
        if (_sym650()) {
          __exception_flag |= exception_flag;
        }
        else {
          __pending_exception_flag |= External;
        }
      }
      if (exception_flag & dec) {
        __exception_flag |= exception_flag;
      }
      if (exception_flag & pcex) {
        __exception_flag |= exception_flag;
      }
    }
    
    //
    // Helper code.
    //
    
    //
    // Omitting unused helper function SItoSISat
    //
    
     void bar ( void  ) ATTRIBUTE_USED {
           int i = 10 ;
    }
     void cond_test ( int index1 , int index2 , bool flag ) ATTRIBUTE_USED {
          if ( flag ) {
                 GPR_write( to_uint32(index1) , ( 0x12345678 ) ) ;
            } else {
                 GPR_write( to_uint32(index2) , ( 0xdeadbeef ) ) ;
            }
    }
     void cr_update ( bits < 3 > field , bits < 4 > & r ) ATTRIBUTE_USED {
          CR_cr_write( to_uint32(field),(r) ) ;
    }
     void field_slice_test ( bits < 3 > t , bits < 5 > a = 0 ) ATTRIBUTE_USED {
           GPR_write( to_uint32(t) , (  GPR_read( to_uint32(a) )  ) ) ;
    }
     void register_commit (  ) ATTRIBUTE_USED {
           ICNT_write(  ICNT_read() + 1  )  ;
    }
     void register_fetch (  ) ATTRIBUTE_USED {
           FCNT_write(  FCNT_read() + 1  )  ;
    }
     void register_post_cycle (  ) ATTRIBUTE_USED {
           PCCNT_write(  PCCNT_read() + 1  )  ;
          if ( PCEXCPT_read() != 0 && PCCNT_read() == PCEXCPT_read() ) {
                raiseException ( pcex ) ;
            }
    }
     void register_pre_cycle (  ) ATTRIBUTE_USED {
           BCCNT_write(  BCCNT_read() + 1  )  ;
    }
    template < size_t Nb1 , typename Y >  void setCrField ( bits < 3 > field , bits < Nb1  > x , Y y ) {
           bits < 4 > r = ( ( x . signedLT ( y ) ) ? 0x8 : 0 ) | ( ( x . signedGT ( y ) ) ? 0x4 : 0 ) | ( ( x == y ) ? 0x2 : 0 ) ;
          cr_update ( field , r ) ;
          bar (  ) ;
    }
     void str_func ( const char * src_str , const char * action , int trg , int src ) ATTRIBUTE_USED {
           const char * s = src_str ;
          for (  int i = 0 ; i != 4 ; ++ i ) {
                 int index = * s ++ - '0' ;
                switch ( * action ++ ) {
                      case 'z' : GPR_slice_set( to_uint32(trg),  to_uint32( i * 8 ),to_uint32( i * 8 + 7 )  , ( 0 )  ); ;
                      break ;
                      case 'f' : GPR_slice_set( to_uint32(trg),  to_uint32( i * 8 ),to_uint32( i * 8 + 7 )  , ( 0xff )  ); ;
                      break ;
                      case 'v' : GPR_slice_set( to_uint32(trg),  to_uint32( i * 8 ),to_uint32( i * 8 + 7 )  , (  GPR_read( to_uint32(src) ) ( index * 8 , index * 8 + 7 )  )  ); ;
                      break ;
                  }
            }
    }
     void ternary_test1 ( bool b , int index ) ATTRIBUTE_USED {
           bits < 32 > x = ( b ) ? bits<12>( bits < 4 > ( 1 ) ) : bits < 12 > ( 0xbeef ) ;
           GPR_write( to_uint32(index) , ( x ) ) ;
    }
     void ternary_test2 ( bool b , int index ) ATTRIBUTE_USED {
           bits < 12 > z = 1 ;
           bits < 32 > x = ( b ) ? z : bits<12>( bits < 4 > ( 0xbeef ) ) ;
           GPR_write( to_uint32(index) , ( x ) ) ;
    }
     void test_pre_exception ( Exception e ) ATTRIBUTE_USED {
           PREEXCPT_write(  PREEXCPT_read() + 1  )  ;
    }
    
    //
    // Auxiliary functions.
    //
    
    void addr_check_handler(addr_t ea , addr_t ra , TransType tt)
    {
           ;
    }
    
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
    addr_t instrReadTranslateInternal(addr_t a,int seq) { addr_check_handler(a,a,InstrTrans);
       return a; };
    addr_t dataReadTranslateInternal (addr_t a,int seq) { addr_check_handler(a,a,LoadTrans);
       return a; };
    addr_t dataWriteTranslateInternal(addr_t a,int seq) { addr_check_handler(a,a,StoreTrans);
       return a; };
    
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
      _commitQueue.commit();
      _commitQueue.next();
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
      return _lastRealAddr = (dataReadTranslateInternal(ea,seq) & 0xffffffffULL);
    }
    
    template <size_t Nb>
    addr_t dataReadTranslate(const bits<Nb> &ea) { return dataReadTranslate(ea.uint64(),0); }
    
    addr_t dataWriteTranslate(addr_t ea,int seq)  ATTRIBUTE_USED {
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
    
    template <size_t NB>
     bits<NB*8> mem_read (addr_t ea,int type = CacheRead,bool has_cw_addr = false,addr_t cw_addr = 0)
    {
      DataReadTranslate translate(*this);
      SimpleCache<GlobalMemoryType&> cache(mem);
      LogmemMemoryRead logger(*this);
      if (Tracing && adl_unlikely(_trace_mode & ttCoreMemRead)) {
        if (has_cw_addr || type != CacheRead) {
          _logger->log_core_mem_read_typed(0,"Mem",((has_cw_addr) ? cw_addr : ea),NB,(CacheAccess)type);
        } else {
          _logger->log_core_mem_read(0,"Mem",ea,NB);
        }
      }
      read_internal<DataReadTranslate,SimpleCache<GlobalMemoryType&>,LogmemMemoryRead,NB,1> ri;
      ea = translate.scale_ea(ea);
      return ri(translate,cache,logger,ea);
    };
    
    template <size_t NB>
     void mem_write (addr_t ea,bits<NB*8> value,int type = CacheWrite,bool has_cw_addr = false,addr_t cw_addr = 0)
    {
      DataWriteTranslate translate(*this);
      SimpleCache<GlobalMemoryType&> cache(mem);
      LogmemMemoryWrite logger(*this);
      if (Tracing && adl_unlikely(_trace_mode & ttCoreMemWrite)) {
        if (has_cw_addr || type != CacheWrite) {
          _logger->log_core_mem_write_typed(0,"Mem",((has_cw_addr) ? cw_addr : ea),NB,(CacheAccess)type);
        } else {
          _logger->log_core_mem_write(0,"Mem",ea,NB);
        }
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
    void mem_write (const bits<Nb1> &addr,const bits<Nb2> &value,int type = CacheWrite) {
      mem_write<NB>(addr.uint64(),value,type,false,0);
    };
    
    template <size_t NB,size_t Nb>
    bits<NB*8> mem_read (bits<Nb> addr,int type = CacheRead) {
      return mem_read<NB>(addr.uint64(),type,false,0);
    };
    
    
    template <size_t NB,size_t Nb1,size_t Nb2,size_t Nb3>
    void mem_write (const bits<Nb1> &addr,const bits<Nb2> &value,int type = CacheWrite,bool has_cw_addr = false,const bits<Nb3> &cw_addr = 0) {
      mem_write<NB>(addr.uint64(),value,type,has_cw_addr,cw_addr.uint64());
    };
    
    template <size_t NB,size_t Nb,size_t Nb3>
    bits<NB*8> mem_read (bits<Nb> addr,int type = CacheRead,bool has_cw_addr = false,const bits<Nb3> &cw_addr = 0) {
      return mem_read<NB>(addr.uint64(),type,has_cw_addr,cw_addr.uint64());
    };
    
    uint32_t instr_read32 (addr_t ea)
    {
      InstrReadTranslate translate(*this);
      LogmemInstrRead logger(*this);
      SimpleCache<GlobalMemoryType&> cache(mem);
      MemoryAccess<InstrReadTranslate,SimpleCache<GlobalMemoryType&>,LogmemInstrRead,1> ma;
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
    
    std::string getInstrMemName() const { return "Mem"; }
    std::string getDataMemName() const { return "Mem"; }
    //
    // Register Data Structures
    //
    
    struct AR_R_t : public NoTag<bits<32> > {
      AR_R_t()
      {};
    };
    
    AR_R_t &AR_R() { return _AR_R_Reg; };
    const AR_R_t &AR_R() const { return _AR_R_Reg; };
    AR_R_t &Dbg_AR_R() { return _AR_R_Reg; };
    const AR_R_t &Dbg_AR_R() const { return _AR_R_Reg; };
    
    struct AR_W_t : public NoTag<bits<32> > {
      AR_W_t()
      {};
    };
    
    AR_W_t &AR_W() { return _AR_W_Reg; };
    const AR_W_t &AR_W() const { return _AR_W_Reg; };
    AR_W_t &Dbg_AR_W() { return _AR_W_Reg; };
    const AR_W_t &Dbg_AR_W() const { return _AR_W_Reg; };
    
    struct BCCNT_t : public NoTag<bits<32> > {
      BCCNT_t()
      {};
    };
    
    BCCNT_t &BCCNT() { return _BCCNT_Reg; };
    const BCCNT_t &BCCNT() const { return _BCCNT_Reg; };
    BCCNT_t &Dbg_BCCNT() { return _BCCNT_Reg; };
    const BCCNT_t &Dbg_BCCNT() const { return _BCCNT_Reg; };
    
    struct BranchTaken_t : public NoTag<bits<32> > {
      BranchTaken_t()
      {};
    };
    
    BranchTaken_t &BranchTaken() { return _BranchTaken_Reg; };
    const BranchTaken_t &BranchTaken() const { return _BranchTaken_Reg; };
    BranchTaken_t &Dbg_BranchTaken() { return _BranchTaken_Reg; };
    const BranchTaken_t &Dbg_BranchTaken() const { return _BranchTaken_Reg; };
    
    struct CIA_t : public NoTag<bits<32> > {
      CIA_t()
      {};
    };
    
    CIA_t &CIA() { return _CIA_Reg; };
    const CIA_t &CIA() const { return _CIA_Reg; };
    CIA_t &Dbg_CIA() { return _CIA_Reg; };
    const CIA_t &Dbg_CIA() const { return _CIA_Reg; };
    
    struct CR_t : public NoTag<bits<32> > {
      CR_t()
      {};
    };
    
    CR_t &CR() { return _CR_Reg; };
    const CR_t &CR() const { return _CR_Reg; };
    CR_t &Dbg_CR() { return _CR_Reg; };
    const CR_t &Dbg_CR() const { return _CR_Reg; };
    
    struct CSRR0_t : public NoTag<bits<32> > {
      CSRR0_t()
      {};
    };
    
    CSRR0_t &CSRR0() { return _CSRR0_Reg; };
    const CSRR0_t &CSRR0() const { return _CSRR0_Reg; };
    CSRR0_t &Dbg_CSRR0() { return _CSRR0_Reg; };
    const CSRR0_t &Dbg_CSRR0() const { return _CSRR0_Reg; };
    
    struct CSRR1_t : public NoTag<bits<32> > {
      CSRR1_t()
      {};
    };
    
    CSRR1_t &CSRR1() { return _CSRR1_Reg; };
    const CSRR1_t &CSRR1() const { return _CSRR1_Reg; };
    CSRR1_t &Dbg_CSRR1() { return _CSRR1_Reg; };
    const CSRR1_t &Dbg_CSRR1() const { return _CSRR1_Reg; };
    
    struct CTR_t : public NoTag<bits<32> > {
      CTR_t()
      {};
    };
    
    CTR_t &CTR() { return _CTR_Reg; };
    const CTR_t &CTR() const { return _CTR_Reg; };
    CTR_t &Dbg_CTR() { return _CTR_Reg; };
    const CTR_t &Dbg_CTR() const { return _CTR_Reg; };
    
    struct DEC_t : public NoTag<bits<32> > {
      DEC_t()
      {};
    };
    
    DEC_t &DEC() { return _DEC_Reg; };
    const DEC_t &DEC() const { return _DEC_Reg; };
    DEC_t &Dbg_DEC() { return _DEC_Reg; };
    const DEC_t &Dbg_DEC() const { return _DEC_Reg; };
    
    struct ESR_t : public NoTag<bits<32> > {
      ESR_t()
      {};
    };
    
    ESR_t &ESR() { return _ESR_Reg; };
    const ESR_t &ESR() const { return _ESR_Reg; };
    ESR_t &Dbg_ESR() { return _ESR_Reg; };
    const ESR_t &Dbg_ESR() const { return _ESR_Reg; };
    
    struct EWTEST_t : public NoTag<bits<32> > {
      EWTEST_t()
      {};
    };
    
    EWTEST_t &EWTEST() { return _EWTEST_Reg; };
    const EWTEST_t &EWTEST() const { return _EWTEST_Reg; };
    EWTEST_t &Dbg_EWTEST() { return _EWTEST_Reg; };
    const EWTEST_t &Dbg_EWTEST() const { return _EWTEST_Reg; };
    
    struct FBCTRL_t : public NoTag<bits<32> > {
      FBCTRL_t()
      {};
    };
    
    FBCTRL_t &FBCTRL() { return _FBCTRL_Reg; };
    const FBCTRL_t &FBCTRL() const { return _FBCTRL_Reg; };
    FBCTRL_t &Dbg_FBCTRL() { return _FBCTRL_Reg; };
    const FBCTRL_t &Dbg_FBCTRL() const { return _FBCTRL_Reg; };
    
    struct FCNT_t : public NoTag<bits<32> > {
      FCNT_t()
      {};
    };
    
    FCNT_t &FCNT() { return _FCNT_Reg; };
    const FCNT_t &FCNT() const { return _FCNT_Reg; };
    FCNT_t &Dbg_FCNT() { return _FCNT_Reg; };
    const FCNT_t &Dbg_FCNT() const { return _FCNT_Reg; };
    
    struct FOO1_t : public NoTag<bits<32> > {
      bits<32> _reserved_mask;
      bits<32> _readonly_mask;
      FOO1_t() : 
      _reserved_mask("0b00000000000000000000000011111111"), 
      _readonly_mask("0b00000000111111110000000000000000")
      {};
    };
    
    FOO1_t &FOO1() { return _FOO1_Reg; };
    const FOO1_t &FOO1() const { return _FOO1_Reg; };
    FOO1_t &Dbg_FOO1() { return _FOO1_Reg; };
    const FOO1_t &Dbg_FOO1() const { return _FOO1_Reg; };
    
    struct FOO11_t : public NoTag<bits<32> > {
      FOO11_t()
      {};
    };
    
    FOO11_t &FOO11() { return _FOO11_Reg; };
    const FOO11_t &FOO11() const { return _FOO11_Reg; };
    FOO11_t &Dbg_FOO11() { return _FOO11_Reg; };
    const FOO11_t &Dbg_FOO11() const { return _FOO11_Reg; };
    
    struct FOO12_t : public NoTag<bits<32> > {
      FOO12_t()
      {};
    };
    
    FOO12_t &FOO12() { return _FOO12_Reg; };
    const FOO12_t &FOO12() const { return _FOO12_Reg; };
    FOO12_t &Dbg_FOO12() { return _FOO12_Reg; };
    const FOO12_t &Dbg_FOO12() const { return _FOO12_Reg; };
    
    struct FOO13_t : public NoTag<bits<32> > {
      FOO13_t()
      {};
    };
    
    FOO13_t &FOO13() { return _FOO13_Reg; };
    const FOO13_t &FOO13() const { return _FOO13_Reg; };
    FOO13_t &Dbg_FOO13() { return _FOO13_Reg; };
    const FOO13_t &Dbg_FOO13() const { return _FOO13_Reg; };
    
    struct FOO14_t : public NoTag<bits<32> > {
      bits<32> _reserved_mask;
      FOO14_t() : 
      _reserved_mask("0b00001111111111111111111111111111")
      {};
    };
    
    FOO14_t &FOO14() { return _FOO14_Reg; };
    const FOO14_t &FOO14() const { return _FOO14_Reg; };
    FOO14_t &Dbg_FOO14() { return _FOO14_Reg; };
    const FOO14_t &Dbg_FOO14() const { return _FOO14_Reg; };
    
    struct FOO2_t : public NoTag<bits<64> > {
      FOO2_t()
      {};
    };
    
    FOO2_t &FOO2() { return _FOO2_Reg; };
    const FOO2_t &FOO2() const { return _FOO2_Reg; };
    FOO2_t &Dbg_FOO2() { return _FOO2_Reg; };
    const FOO2_t &Dbg_FOO2() const { return _FOO2_Reg; };
    
    struct FOO3_t : public NoTag<bits<32> > {
      FOO3_t()
      {};
    };
    
    FOO3_t &FOO3() { return _FOO3_Reg; };
    const FOO3_t &FOO3() const { return _FOO3_Reg; };
    FOO3_t &Dbg_FOO3() { return _FOO3_Reg; };
    const FOO3_t &Dbg_FOO3() const { return _FOO3_Reg; };
    
    struct FOO5_t : public NoTag<bits<32> > {
      FOO5_t()
      {};
    };
    
    FOO5_t &FOO5() { return _FOO5_Reg; };
    const FOO5_t &FOO5() const { return _FOO5_Reg; };
    FOO5_t &Dbg_FOO5() { return _FOO5_Reg; };
    const FOO5_t &Dbg_FOO5() const { return _FOO5_Reg; };
    
    struct FOO5_RD_t : public NoTag<bits<32> > {
      FOO5_RD_t()
      {};
    };
    
    FOO5_RD_t &FOO5_RD() { return _FOO5_RD_Reg; };
    const FOO5_RD_t &FOO5_RD() const { return _FOO5_RD_Reg; };
    FOO5_RD_t &Dbg_FOO5_RD() { return _FOO5_RD_Reg; };
    const FOO5_RD_t &Dbg_FOO5_RD() const { return _FOO5_RD_Reg; };
    
    struct FOO5_WR_t : public NoTag<bits<32> > {
      FOO5_WR_t()
      {};
    };
    
    FOO5_WR_t &FOO5_WR() { return _FOO5_WR_Reg; };
    const FOO5_WR_t &FOO5_WR() const { return _FOO5_WR_Reg; };
    FOO5_WR_t &Dbg_FOO5_WR() { return _FOO5_WR_Reg; };
    const FOO5_WR_t &Dbg_FOO5_WR() const { return _FOO5_WR_Reg; };
    
    struct FOO7_t : public NoTag<bits<32> > {
      bits<32> _reserved_mask;
      FOO7_t() : 
      _reserved_mask("0b00000000000000001111111111111111")
      {};
    };
    
    FOO7_t &FOO7() { return _FOO7_Reg; };
    const FOO7_t &FOO7() const { return _FOO7_Reg; };
    FOO7_t &Dbg_FOO7() { return _FOO7_Reg; };
    const FOO7_t &Dbg_FOO7() const { return _FOO7_Reg; };
    
    struct FOO9_t : public NoTag<bits<32> > {
      FOO9_t()
      {};
    };
    
    FOO9_t &FOO9() { return _FOO9_Reg; };
    const FOO9_t &FOO9() const { return _FOO9_Reg; };
    FOO9_t &Dbg_FOO9() { return _FOO9_Reg; };
    const FOO9_t &Dbg_FOO9() const { return _FOO9_Reg; };
    
    struct ICNT_t : public NoTag<bits<32> > {
      ICNT_t()
      {};
    };
    
    ICNT_t &ICNT() { return _ICNT_Reg; };
    const ICNT_t &ICNT() const { return _ICNT_Reg; };
    ICNT_t &Dbg_ICNT() { return _ICNT_Reg; };
    const ICNT_t &Dbg_ICNT() const { return _ICNT_Reg; };
    
    struct ITEST_t : public NoTag<bits<32> > {
      ITEST_t()
      {};
    };
    
    ITEST_t &ITEST() { return _ITEST_Reg; };
    const ITEST_t &ITEST() const { return _ITEST_Reg; };
    ITEST_t &Dbg_ITEST() { return _ITEST_Reg; };
    const ITEST_t &Dbg_ITEST() const { return _ITEST_Reg; };
    
    struct IVOR10_t : public NoTag<bits<32> > {
      IVOR10_t()
      {};
    };
    
    IVOR10_t &IVOR10() { return _IVOR10_Reg; };
    const IVOR10_t &IVOR10() const { return _IVOR10_Reg; };
    IVOR10_t &Dbg_IVOR10() { return _IVOR10_Reg; };
    const IVOR10_t &Dbg_IVOR10() const { return _IVOR10_Reg; };
    
    struct IVOR4_t : public NoTag<bits<32> > {
      IVOR4_t()
      {};
    };
    
    IVOR4_t &IVOR4() { return _IVOR4_Reg; };
    const IVOR4_t &IVOR4() const { return _IVOR4_Reg; };
    IVOR4_t &Dbg_IVOR4() { return _IVOR4_Reg; };
    const IVOR4_t &Dbg_IVOR4() const { return _IVOR4_Reg; };
    
    struct IVOR6_t : public NoTag<bits<32> > {
      IVOR6_t()
      {};
    };
    
    IVOR6_t &IVOR6() { return _IVOR6_Reg; };
    const IVOR6_t &IVOR6() const { return _IVOR6_Reg; };
    IVOR6_t &Dbg_IVOR6() { return _IVOR6_Reg; };
    const IVOR6_t &Dbg_IVOR6() const { return _IVOR6_Reg; };
    
    struct IVOR7_t : public NoTag<bits<32> > {
      IVOR7_t()
      {};
    };
    
    IVOR7_t &IVOR7() { return _IVOR7_Reg; };
    const IVOR7_t &IVOR7() const { return _IVOR7_Reg; };
    IVOR7_t &Dbg_IVOR7() { return _IVOR7_Reg; };
    const IVOR7_t &Dbg_IVOR7() const { return _IVOR7_Reg; };
    
    struct IVPR_t : public NoTag<bits<32> > {
      IVPR_t()
      {};
    };
    
    IVPR_t &IVPR() { return _IVPR_Reg; };
    const IVPR_t &IVPR() const { return _IVPR_Reg; };
    IVPR_t &Dbg_IVPR() { return _IVPR_Reg; };
    const IVPR_t &Dbg_IVPR() const { return _IVPR_Reg; };
    
    struct MSR_t : public NoTag<bits<32> > {
      bits<32> _reserved_mask;
      MSR_t() : 
      _reserved_mask("0b11111111100001110111111111111111")
      {};
    };
    
    MSR_t &MSR() { return _MSR_Reg; };
    const MSR_t &MSR() const { return _MSR_Reg; };
    MSR_t &Dbg_MSR() { return _MSR_Reg; };
    const MSR_t &Dbg_MSR() const { return _MSR_Reg; };
    
    struct NIA_t : public NoTag<bits<32> > {
      NIA_t()
      {};
    };
    
    NIA_t &NIA() { return _NIA_Reg; };
    const NIA_t &NIA() const { return _NIA_Reg; };
    NIA_t &Dbg_NIA() { return _NIA_Reg; };
    const NIA_t &Dbg_NIA() const { return _NIA_Reg; };
    
    struct PCCNT_t : public NoTag<bits<32> > {
      PCCNT_t()
      {};
    };
    
    PCCNT_t &PCCNT() { return _PCCNT_Reg; };
    const PCCNT_t &PCCNT() const { return _PCCNT_Reg; };
    PCCNT_t &Dbg_PCCNT() { return _PCCNT_Reg; };
    const PCCNT_t &Dbg_PCCNT() const { return _PCCNT_Reg; };
    
    struct PCEXCPT_t : public NoTag<bits<32> > {
      PCEXCPT_t()
      {};
    };
    
    PCEXCPT_t &PCEXCPT() { return _PCEXCPT_Reg; };
    const PCEXCPT_t &PCEXCPT() const { return _PCEXCPT_Reg; };
    PCEXCPT_t &Dbg_PCEXCPT() { return _PCEXCPT_Reg; };
    const PCEXCPT_t &Dbg_PCEXCPT() const { return _PCEXCPT_Reg; };
    
    struct POSTEXCPT_t : public NoTag<bits<32> > {
      POSTEXCPT_t()
      {};
    };
    
    POSTEXCPT_t &POSTEXCPT() { return _POSTEXCPT_Reg; };
    const POSTEXCPT_t &POSTEXCPT() const { return _POSTEXCPT_Reg; };
    POSTEXCPT_t &Dbg_POSTEXCPT() { return _POSTEXCPT_Reg; };
    const POSTEXCPT_t &Dbg_POSTEXCPT() const { return _POSTEXCPT_Reg; };
    
    struct PREEXCPT_t : public NoTag<bits<32> > {
      PREEXCPT_t()
      {};
    };
    
    PREEXCPT_t &PREEXCPT() { return _PREEXCPT_Reg; };
    const PREEXCPT_t &PREEXCPT() const { return _PREEXCPT_Reg; };
    PREEXCPT_t &Dbg_PREEXCPT() { return _PREEXCPT_Reg; };
    const PREEXCPT_t &Dbg_PREEXCPT() const { return _PREEXCPT_Reg; };
    
    struct SRR0_t : public NoTag<bits<32> > {
      SRR0_t()
      {};
    };
    
    SRR0_t &SRR0() { return _SRR0_Reg; };
    const SRR0_t &SRR0() const { return _SRR0_Reg; };
    SRR0_t &Dbg_SRR0() { return _SRR0_Reg; };
    const SRR0_t &Dbg_SRR0() const { return _SRR0_Reg; };
    
    struct SRR1_t : public NoTag<bits<32> > {
      SRR1_t()
      {};
    };
    
    SRR1_t &SRR1() { return _SRR1_Reg; };
    const SRR1_t &SRR1() const { return _SRR1_Reg; };
    SRR1_t &Dbg_SRR1() { return _SRR1_Reg; };
    const SRR1_t &Dbg_SRR1() const { return _SRR1_Reg; };
    
    struct Slice_t : public NoTag<bits<32> > {
      Slice_t()
      {};
    };
    
    Slice_t &Slice() { return _Slice_Reg; };
    const Slice_t &Slice() const { return _Slice_Reg; };
    Slice_t &Dbg_Slice() { return _Slice_Reg; };
    const Slice_t &Dbg_Slice() const { return _Slice_Reg; };
    
    struct XER_t : public NoTag<bits<32> > {
      XER_t()
      {};
    };
    
    XER_t &XER() { return _XER_Reg; };
    const XER_t &XER() const { return _XER_Reg; };
    XER_t &Dbg_XER() { return _XER_Reg; };
    const XER_t &Dbg_XER() const { return _XER_Reg; };
    
    struct tmpESR_t : public NoTag<bits<32> > {
      tmpESR_t()
      {};
    };
    
    tmpESR_t &tmpESR() { return _tmpESR_Reg; };
    const tmpESR_t &tmpESR() const { return _tmpESR_Reg; };
    tmpESR_t &Dbg_tmpESR() { return _tmpESR_Reg; };
    const tmpESR_t &Dbg_tmpESR() const { return _tmpESR_Reg; };
    
    //
    // Register File Data Structures
    //
    
    struct GPR_t : public RegFile<NoTag<bits<32> >,32,32> {
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
          case 9:
          return true;
          case 22:
          return true;
          case 26:
          return true;
          case 27:
          return true;
          case 30:
          return true;
          case 31:
          return true;
          case 58:
          return true;
          case 59:
          return true;
          case 62:
          return true;
          case 100:
          return true;
          case 101:
          return true;
          case 200:
          return true;
          case 201:
          return true;
          case 202:
          return true;
          case 208:
          return true;
          case 209:
          return true;
          case 210:
          return true;
          case 211:
          return true;
          case 230:
          return true;
          case 231:
          return true;
          case 232:
          return true;
          case 233:
          return true;
          case 234:
          return true;
          case 235:
          return true;
          case 250:
          return true;
          case 251:
          return true;
          case 301:
          return true;
          case 302:
          return true;
          case 303:
          return true;
          case 304:
          return true;
          case 400:
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
        case 9:
        Dbg_CTR().set_value(value);
        return true;
        case 22:
        Dbg_DEC().set_value(value);
        return true;
        case 26:
        Dbg_SRR0().set_value(value);
        return true;
        case 27:
        Dbg_SRR1().set_value(value);
        return true;
        case 30:
        Dbg_FOO1().set_value(value);
        return true;
        case 31:
        Dbg_FOO2().set_value(value);
        return true;
        case 58:
        Dbg_CSRR0().set_value(value);
        return true;
        case 59:
        Dbg_CSRR1().set_value(value);
        return true;
        case 62:
        Dbg_ESR().set_value(value);
        return true;
        case 100:
        return GPR_debug_set(20,value);
        case 101:
        return GPR_debug_set(21,value);
        case 200:
        Dbg_GPR()(2).set_value(value);
        return true;
        case 201:
        Dbg_FOO3().set_value(value);
        return true;
        case 202:
        Dbg_FOO5().set_value(value);
        return true;
        case 208:
        Dbg_FOO1().set_value(value);
        return true;
        case 209:
        Dbg_FOO9().set_value(value);
        return true;
        case 210:
        Dbg_FOO9().set_value(value);
        return true;
        case 211:
        Dbg_FOO11().set_value(value);
        return true;
        case 230:
        AR(0).set_value(value);
        return true;
        case 231:
        AR(1).set_value(value);
        return true;
        case 232:
        AR(2).set_value(value);
        return true;
        case 233:
        return ARF_debug_set(0,value);
        case 234:
        return ARF_debug_set(1,value);
        case 235:
        return ARF_debug_set(2,value);
        case 250:
        return false;
        case 251:
        Dbg_BranchTaken().set_value(value);
        return true;
        case 301:
        Dbg_FOO2().set_value(value);
        return true;
        case 302:
        return false;
        case 303:
        return false;
        case 304:
        Dbg_ITEST().set_value(value);
        return true;
        case 400:
        Dbg_EWTEST().set_value(value);
        return true;
        default:
        return false;
      }
    }
    
    bits<32> SPR_debug_get(unsigned index,bool &rc) const {
      switch (index) {
        case 1:
        return Dbg_XER().get_value();
        case 9:
        return Dbg_CTR().get_value();
        case 22:
        return Dbg_DEC().get_value();
        case 26:
        return Dbg_SRR0().get_value();
        case 27:
        return Dbg_SRR1().get_value();
        case 30:
        return Dbg_FOO1().get_value();
        case 31:
        return Dbg_FOO2().get_value();
        case 58:
        return Dbg_CSRR0().get_value();
        case 59:
        return Dbg_CSRR1().get_value();
        case 62:
        return Dbg_ESR().get_value();
        case 100:
        return GPR_debug_get(20,rc);
        case 101:
        return GPR_debug_get(21,rc);
        case 200:
        return Dbg_GPR()(2).get_value();
        case 201:
        return Dbg_FOO3().get_value();
        case 202:
        return Dbg_FOO5().get_value();
        case 208:
        return Dbg_FOO1().get_value();
        case 209:
        return Dbg_FOO9().get_value();
        case 210:
        return Dbg_FOO9().get_value();
        case 211:
        return Dbg_FOO11().get_value();
        case 230:
        return AR(0).get_value();
        case 231:
        return AR(1).get_value();
        case 232:
        return AR(2).get_value();
        case 233:
        return ARF_debug_get(0,rc);
        case 234:
        return ARF_debug_get(1,rc);
        case 235:
        return ARF_debug_get(2,rc);
        case 250:
        rc = 0;
        return 0;
        case 251:
        return Dbg_BranchTaken().get_value();
        case 301:
        return Dbg_FOO2().get_value();
        case 302:
        rc = 0;
        return 0;
        case 303:
        rc = 0;
        return 0;
        case 304:
        return Dbg_ITEST().get_value();
        case 400:
        return Dbg_EWTEST().get_value();
        default:
        rc = false;
        return 0;
      }
    }
    
    struct VPR_t : public RegFile<NoTag<bits<128> >,32,32> {
      void set_value(unsigned index,const bits<128> &x) { operator()(index).set_value(x); }
      VPR_t()
      {};
      bits<128> get_value(unsigned index) const { return operator()(index).get_data(); };
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
    
    VPR_t &VPR() { return _VPR_Reg; };
    const VPR_t &VPR() const { return _VPR_Reg; };
    VPR_t &Dbg_VPR() { return _VPR_Reg; };
    const VPR_t &Dbg_VPR() const { return _VPR_Reg; };
    
    bool VPR_debug_set(unsigned index,const bits<128> &value) {
      if (index >= Dbg_VPR().size()) { return false; };
      Dbg_VPR().set_value(VPR_t::getWriteIndex(index),value);
      return true;
    }
    
    bits<128> VPR_debug_get(unsigned index,bool &rc) const {
      if (index >= Dbg_VPR().size()) { rc = false; return 0; };
      rc = true;
      return Dbg_VPR().get_value(VPR_t::getReadIndex(index));
    }
    
    struct XPR_t : public RegFile<NoTag<bits<32> >,32,32> {
      void set_value(unsigned index,const bits<32> &x) { operator()(index).set_value(x); }
      XPR_t()
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
    
    XPR_t &XPR() { return _XPR_Reg; };
    const XPR_t &XPR() const { return _XPR_Reg; };
    XPR_t &Dbg_XPR() { return _XPR_Reg; };
    const XPR_t &Dbg_XPR() const { return _XPR_Reg; };
    
    bool XPR_debug_set(unsigned index,const bits<32> &value) {
      if (index >= Dbg_XPR().size()) { return false; };
      Dbg_XPR().set_value(XPR_t::getWriteIndex(index),value);
      return true;
    }
    
    bits<32> XPR_debug_get(unsigned index,bool &rc) const {
      if (index >= Dbg_XPR().size()) { rc = false; return 0; };
      rc = true;
      return Dbg_XPR().get_value(XPR_t::getReadIndex(index));
    }
    
    //
    // Register Data Structures
    //
    
    struct AR_t : public NoTag<bits<32> > {
      AR_t()
      {};
    };
    
    AR_t &AR() { return foobar(foobar._foobar_active)._AR_Reg; };
    const AR_t &AR() const { return foobar(foobar._foobar_active)._AR_Reg; };
    AR_t &Dbg_AR() { return foobar(foobar._foobar_debug)._AR_Reg; };
    const AR_t &Dbg_AR() const { return foobar(foobar._foobar_debug)._AR_Reg; };
    AR_t &AR(unsigned ctx_index) { return foobar(ctx_index)._AR_Reg; };
    const AR_t &AR(unsigned ctx_index) const { return foobar(ctx_index)._AR_Reg; };
    
    struct BR_t : public NoTag<bits<32> > {
      BR_t()
      {};
    };
    
    BR_t &BR() { return foobar(foobar._foobar_active)._BR_Reg; };
    const BR_t &BR() const { return foobar(foobar._foobar_active)._BR_Reg; };
    BR_t &Dbg_BR() { return foobar(foobar._foobar_debug)._BR_Reg; };
    const BR_t &Dbg_BR() const { return foobar(foobar._foobar_debug)._BR_Reg; };
    BR_t &BR(unsigned ctx_index) { return foobar(ctx_index)._BR_Reg; };
    const BR_t &BR(unsigned ctx_index) const { return foobar(ctx_index)._BR_Reg; };
    
    struct DR_t : public NoTag<bits<32> > {
      DR_t()
      {};
    };
    
    DR_t &DR() { return foobar(foobar._foobar_active)._DR_Reg; };
    const DR_t &DR() const { return foobar(foobar._foobar_active)._DR_Reg; };
    DR_t &Dbg_DR() { return foobar(foobar._foobar_debug)._DR_Reg; };
    const DR_t &Dbg_DR() const { return foobar(foobar._foobar_debug)._DR_Reg; };
    DR_t &DR(unsigned ctx_index) { return foobar(ctx_index)._DR_Reg; };
    const DR_t &DR(unsigned ctx_index) const { return foobar(ctx_index)._DR_Reg; };
    
    //
    // Register File Data Structures
    //
    
    struct ARF_t : public RegFile<NoTag<bits<32> >,32,32> {
      void set_value(unsigned index,const bits<32> &x) { operator()(index).set_value(x); }
      ARF_t()
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
    
    ARF_t &ARF() { return foobar(foobar._foobar_active)._ARF_Reg; };
    const ARF_t &ARF() const { return foobar(foobar._foobar_active)._ARF_Reg; };
    ARF_t &Dbg_ARF() { return foobar(foobar._foobar_debug)._ARF_Reg; };
    const ARF_t &Dbg_ARF() const { return foobar(foobar._foobar_debug)._ARF_Reg; };
    ARF_t &ARF(unsigned ctx_index) { return foobar(ctx_index)._ARF_Reg; };
    const ARF_t &ARF(unsigned ctx_index) const { return foobar(ctx_index)._ARF_Reg; };
    
    bool ARF_debug_set(unsigned index,const bits<32> &value) {
      if (index >= Dbg_ARF().size()) { return false; };
      Dbg_ARF().set_value(ARF_t::getWriteIndex(index),value);
      return true;
    }
    
    bits<32> ARF_debug_get(unsigned index,bool &rc) const {
      if (index >= Dbg_ARF().size()) { rc = false; return 0; };
      rc = true;
      return Dbg_ARF().get_value(ARF_t::getReadIndex(index));
    }
    
    struct ARF16_t {
      static bool validIndex(unsigned index) ATTRIBUTE_USED {
        return (index < 16);
      };
      static unsigned getReadIndex(unsigned index) ATTRIBUTE_INLINE {
        switch (index) {
          case 0:
          return 0;
          break;
          case 1:
          return 1;
          break;
          case 2:
          return 2;
          break;
          case 3:
          return 3;
          break;
          case 4:
          return 4;
          break;
          case 5:
          return 5;
          break;
          case 6:
          return 6;
          break;
          case 7:
          return 7;
          break;
          case 8:
          return 24;
          break;
          case 9:
          return 25;
          break;
          case 10:
          return 26;
          break;
          case 11:
          return 27;
          break;
          case 12:
          return 28;
          break;
          case 13:
          return 29;
          break;
          case 14:
          return 30;
          break;
          case 15:
          return 31;
          break;
          default:
          std::cerr << "Bad register access ARF16(" << index << ").\n\n";
          abort();
        };
      }
      static unsigned getWriteIndex(unsigned index) ATTRIBUTE_INLINE {
        switch (index) {
          case 0:
          return 0;
          break;
          case 1:
          return 1;
          break;
          case 2:
          return 2;
          break;
          case 3:
          return 3;
          break;
          case 4:
          return 4;
          break;
          case 5:
          return 5;
          break;
          case 6:
          return 6;
          break;
          case 7:
          return 7;
          break;
          case 8:
          return 24;
          break;
          case 9:
          return 25;
          break;
          case 10:
          return 26;
          break;
          case 11:
          return 27;
          break;
          case 12:
          return 28;
          break;
          case 13:
          return 29;
          break;
          case 14:
          return 30;
          break;
          case 15:
          return 31;
          break;
          default:
          std::cerr << "Bad register access ARF16(" << index << ").\n\n";
          abort();
        };
      }
      template <size_t Nb> static bool validIndex(bits<Nb> x) { return validIndex(x.uint32()); };
    };
    
    ARF16_t &ARF16() { return foobar(foobar._foobar_active)._ARF16_Reg; };
    const ARF16_t &ARF16() const { return foobar(foobar._foobar_active)._ARF16_Reg; };
    ARF16_t &Dbg_ARF16() { return foobar(foobar._foobar_debug)._ARF16_Reg; };
    const ARF16_t &Dbg_ARF16() const { return foobar(foobar._foobar_debug)._ARF16_Reg; };
    ARF16_t &ARF16(unsigned ctx_index) { return foobar(ctx_index)._ARF16_Reg; };
    const ARF16_t &ARF16(unsigned ctx_index) const { return foobar(ctx_index)._ARF16_Reg; };
    
    bool ARF16_debug_set(unsigned index,const bits<32> &value) {
      if (index >= Dbg_ARF().size()) { return false; };
      Dbg_ARF().set_value(ARF16_t::getWriteIndex(index),value);
      return true;
    }
    
    bits<32> ARF16_debug_get(unsigned index,bool &rc) const {
      if (index >= Dbg_ARF().size()) { rc = false; return 0; };
      rc = true;
      return Dbg_ARF().get_value(ARF16_t::getReadIndex(index));
    }
    
    struct CRF_t : public RegFile<NoTag<bits<32> >,8,8> {
      void set_value(unsigned index,const bits<32> &x) { operator()(index).set_value(x); }
      CRF_t()
      {};
      bits<32> get_value(unsigned index) const { return operator()(index).get_data(); };
      static bool validIndex(unsigned index) ATTRIBUTE_USED {
        return (index < 8);
      };
      static unsigned getReadIndex(unsigned index) ATTRIBUTE_INLINE {
        return index;
      }
      static unsigned getWriteIndex(unsigned index) ATTRIBUTE_INLINE {
        return index;
      }
      template <size_t Nb> static bool validIndex(bits<Nb> x) { return validIndex(x.uint32()); };
    };
    
    CRF_t &CRF() { return foobar(foobar._foobar_active)._CRF_Reg; };
    const CRF_t &CRF() const { return foobar(foobar._foobar_active)._CRF_Reg; };
    CRF_t &Dbg_CRF() { return foobar(foobar._foobar_debug)._CRF_Reg; };
    const CRF_t &Dbg_CRF() const { return foobar(foobar._foobar_debug)._CRF_Reg; };
    CRF_t &CRF(unsigned ctx_index) { return foobar(ctx_index)._CRF_Reg; };
    const CRF_t &CRF(unsigned ctx_index) const { return foobar(ctx_index)._CRF_Reg; };
    
    bool CRF_debug_set(unsigned index,const bits<32> &value) {
      if (index >= Dbg_CRF().size()) { return false; };
      Dbg_CRF().set_value(CRF_t::getWriteIndex(index),value);
      return true;
    }
    
    bits<32> CRF_debug_get(unsigned index,bool &rc) const {
      if (index >= Dbg_CRF().size()) { rc = false; return 0; };
      rc = true;
      return Dbg_CRF().get_value(CRF_t::getReadIndex(index));
    }
    
    //
    // Register Variables
    //
    
    //
    // Register File Variables
    //
    
    struct foobar_context_t {
      
      //
      // Register Variables
      //
      
      //
      // Register File Variables
      //
      
      AR_t _AR_Reg;
      BR_t _BR_Reg;
      DR_t _DR_Reg;
      ARF_t _ARF_Reg;
      ARF16_t _ARF16_Reg;
      CRF_t _CRF_Reg;
      
    };
    
    struct foobar_obj_t {
      
      foobar_obj_t(P &core) : _core(core) {
        reset();
      };
      void reset() {_foobar_active = _foobar_debug = 0;}
      unsigned num_contexts() const { return 3; };
      unsigned active_context() const { return _foobar_active; };
      
      unsigned debug_context() const { return _foobar_debug; };
      
      foobar_context_t &operator()(unsigned index) { return _ctx[index]; };
      const foobar_context_t &operator()(unsigned index) const { return _ctx[index]; }
      
      foobar_context_t &operator()() { return _ctx[_foobar_active]; };
      const foobar_context_t &operator()() const { return _ctx[_foobar_active]; };
      
      void read(unsigned index,addr_t addr) {
        assert(index < num_contexts());
        _core.AR_ctx_write(index,_core.mem_read<4>(addr));
        addr += 4;
        _core.BR_ctx_write(index,_core.mem_read<4>(addr));
        addr += 4;
        _core.DR_ctx_write(index,_core.mem_read<4>(addr));
        addr += 4;
        for (unsigned i = 0; i != 32; ++i) {
          _core.ARF_ctx_write(index,i,_core.mem_read<4>(addr));
          addr += 4;
        }
        for (unsigned i = 0; i != 8; ++i) {
          _core.CRF_ctx_write(index,i,_core.mem_read<4>(addr));
          addr += 4;
        }
      }
      
      void write(unsigned index,addr_t addr) {
        assert(index < num_contexts());
        _core.mem_write<4>(addr,_core.AR_ctx_read(index));
        addr += 4;
        _core.mem_write<4>(addr,_core.BR_ctx_read(index));
        addr += 4;
        _core.mem_write<4>(addr,_core.DR_ctx_read(index));
        addr += 4;
        for (unsigned i = 0; i != 32; ++i) {
          _core.mem_write<4>(addr,_core.ARF_ctx_read(index,i));
          addr += 4;
        }
        for (unsigned i = 0; i != 8; ++i) {
          _core.mem_write<4>(addr,_core.CRF_ctx_read(index,i));
          addr += 4;
        }
      }
      
      void clear(unsigned index) {
        assert(index < num_contexts());
        _core.AR_ctx_write(index,0);
        _core.BR_ctx_write(index,0);
        _core.DR_ctx_write(index,0);
        for (unsigned i = 0; i != 32; ++i) {
          _core.ARF_ctx_write(index,i,0);
        }
        for (unsigned i = 0; i != 8; ++i) {
          _core.CRF_ctx_write(index,i,0);
        }
      }
      
      P &_core;
      foobar_context_t _ctx[3];
      unsigned _foobar_active;
      unsigned _foobar_debug;
      
    };
    
    unsigned foobar_control_predicate()
    {
          return FBCTRL_read() . uint32 (  ) ;
    }
    
    void foobar_control_func() {
      foobar._foobar_active = foobar_control_predicate();
      assert(foobar._foobar_active < 3);
    }
    
    void _sym655() {
      _context_update_event = true;
    }
    
    void foobar_update_func() {
      foobar_control_func();
      if (Tracing && trace_mode()) {
        _logger->log_ctx_change(0,"foobar",foobar._foobar_active,ctxSwitch);
      }
    }
    
    bool foobar_check_change(unsigned index) {
      if (Tracing && trace_mode()) {
        if (index != foobar._foobar_active) {
          _logger->log_ctx_change(0,"foobar",index,ctxUpdate);
          return true;
        }
      }
      return false;
    }
    
    void foobar_change_back(bool log) {
      if (Tracing && trace_mode() && log) {
        _logger->log_ctx_change(0,"foobar",foobar._foobar_active,ctxDone);
      }
    }
    
    
    CtxInfos initCtxInfo() const {
      CtxInfos tmp;
      tmp.push_back(CtxInfo());
      tmp.back()._id = 0;
      tmp.back()._name = "foobar";
      tmp.back()._num_contexts = 3;
      tmp.back()._is_thread = 0;
      tmp.back()._size = 172;
      tmp.back().finalize();
      sort_info(tmp);
      return tmp;
    }
    
    const CtxInfos &getContextInfo() const {
      static CtxInfos info = initCtxInfo();
      return info;
    };
    
    unsigned getCtxId(const std::string &name) const {
      if (name == "foobar") {
        return 0;
      }
      else {
        return IssNode::getCtxId(name);
      }
    }
    
    unsigned setDbgCtx(unsigned id,unsigned cn) {
      if (id == 0) {
        unsigned old = foobar._foobar_debug;
        foobar._foobar_debug = cn;
        return old;
      }
      else {
        return IssNode::setDbgCtx(id,cn);
      }
    }
    
    void setActiveCtxToDbg(unsigned id) {
      if (id == 0) {
        foobar._foobar_active = foobar._foobar_debug;
      }
      else {
        return IssNode::setActiveCtxToDbg(id);
      }
    }
    
    void restoreActiveCtx(unsigned id) {
      if (id == 0) {
        foobar_control_func();
      }
      else {
        return IssNode::restoreActiveCtx(id);
      }
    }
    
    unsigned activeContext(unsigned id) const {
      if (id == 0) {
        return foobar._foobar_active;
      }
      else {
        RError("Unknown context ID " << id << ".");
      }
    }
    
    unsigned getActiveCtx(unsigned id) const {
      return activeContext(id);
    }
    
    void writeContext(unsigned id,unsigned index,addr_t addr) {
      if (id == 0) {
        foobar.write(index,addr);
      }
      else {
        RError("Unknown context ID " << id << ".");
      }
    }
    
    void readContext(unsigned id,unsigned index,addr_t addr) {
      if (id == 0) {
        foobar.read(index,addr);
      }
      else {
        RError("Unknown context ID " << id << ".");
      }
    }
    
    void clearContext(unsigned id,unsigned index) {
      if (id == 0) {
        foobar.clear(index);
      }
      else {
        RError("Unknown context ID " << id << ".");
      }
    }
    
    //
    // Register Handling Code.
    //
    
    // Register AR
    void AR_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(0,"AR",AR().get_data().rnum());
      }
    }
    
    void AR_trace(unsigned ctx_index)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(0,"AR",AR(ctx_index).get_data().rnum());
      }
    }
    
    void AR_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(0,"AR",AR().get_data().rnum());
      }
    }
    
    void AR_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(0,"AR",AR().get_data().rnum(),mask.rnum());
      }
    }
    
    void AR_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(0,"AR",AR().get_data().rnum(),mask.rnum());
      }
    }
    
    void AR_write_internal(uint32_t ctx_index ,const bits<32> &x)  ATTRIBUTE_USED {
      AR_write_hook(ctx_index , x);
    };
    void AR_write_hook(uint32_t _sym656, bits < 32 > x)  ATTRIBUTE_USED {
      {
             AR_W_write(  AR_W_read() + 1  )  ;
            AR_write_inhook (  _sym656 ,  x ) ;
      }
    };
    void AR_write(const bits<32> &x) {
      AR_write_internal(foobar._foobar_active,x);
    };
    void AR_write_inhook(uint32_t ctx_index ,const bits<32> &x)  ATTRIBUTE_USED {
      AR(ctx_index).set_data() = x;
      CondRegTrace(AR_trace());
    }
    void AR_ctx_write(uint32_t ctx_index ,const bits<32> &x)  ATTRIBUTE_USED {
      AR_write_internal(ctx_index,x);
      {
        bool change = foobar_check_change(ctx_index);
        CondRegTrace(AR_trace(ctx_index));
        foobar_change_back(change);
      }
    }
    bits<32> AR_read_internal(uint32_t _sym656)  ATTRIBUTE_USED {
      {
             AR_R_write(  AR_R_read() + 1  )  ;
            return AR_read_inhook(_sym656) ;
      }
    };
    
    bits<32> &AR_read_inhook(uint32_t ctx_index)  ATTRIBUTE_USED {
      return AR(ctx_index).get_data();
    }
    
    bits<32> AR_read()  ATTRIBUTE_USED {
      CondRegTrace(AR_read_trace());
      return AR_read_internal(foobar._foobar_active);
    }
    bits<32> AR_read_notrace() {
      return AR_read_internal(foobar._foobar_active);
    }
    bits<32> AR_ctx_read(uint32_t ctx_index) {
      return AR_read_internal(ctx_index);
    }
    void AR_reset_value(uint32_t ctx_index) {
      bits<32> x("");
      AR().set_value(x);
      AR().reset_tag();
    };
    void AR_A_write(const bits<16> &x)  ATTRIBUTE_USED {
      bits<32>tmp = AR_read();
      tmp.set(0,15,x);
      AR_write(tmp);
    };
    void AR_A_ctx_write(unsigned ctx_index,const bits<16> &x) {
      bits<32> tmp = AR_read_internal(ctx_index);
      tmp.set(0,15,x);
      AR_write_internal(ctx_index,tmp);
      {
        bool change = foobar_check_change(ctx_index);
        CondRegTrace(AR_trace(ctx_index));
        foobar_change_back(change);
      }
    }
    void AR_A_write_inhook(uint32_t ctx_index ,const bits<16> &x)  ATTRIBUTE_USED {
      AR(ctx_index).set_data().set(0,15,x);
      CondRegTrace(
      {
        bits<32> mask;
        mask.mask(0,15);
        AR_trace(mask);
      }
      );
    };
    bits<8> AR_C_read(unsigned i)  ATTRIBUTE_USED {
      CondRegTrace(
      {
        bits<32> mask;
        mask.mask(((i%32)*8),(((i%32)*8)+8-1));
        AR_read_trace(mask);
      }
      );
      return AR_read_notrace(). get<8>(((i%32)*8),(((i%32)*8)+8-1));
    };
    bits<8> AR_C_ctx_read(unsigned ctx_index,unsigned i) {
      return AR_read_internal(ctx_index). get<8>(((i%32)*8),(((i%32)*8)+8-1));
    };
    template <size_t Nb>
    void AR_slice_set_internal(unsigned l,unsigned r,const bits<Nb> &x) {
      bits<32> tmp = AR_read();
      tmp.set(l,r,x);
      AR_write(tmp);
    };
    template <size_t Nb>
    void AR_slice_set(unsigned l,unsigned r,const bits<Nb> &x) {
      AR_slice_set_internal(l,r,x);
    }
    
    void AR_slice_set(unsigned l,unsigned r,uint64_t x) {
      AR_slice_set(l,r,bits<32>(x));
    };
    template <size_t Nb>
    void AR_slice_set_inhook(unsigned l,unsigned r,const bits<Nb> &x) {
      AR().set_data().set(l,r,x);
      CondRegTrace(
      {
        bits<32> mask;
        mask.mask(l,r);
        AR_trace(mask);
      }
      );
    }
    void AR_slice_set_inhook(unsigned l,unsigned r,uint64_t x) {
      AR_slice_set_inhook(l,r,bits<32>(x));
    };
    template <size_t Nb>
    void AR_ctx_slice_set(uint32_t ctx_index ,unsigned l,unsigned r,const bits<Nb> &x) {
      bits<32> tmp = AR_read_internal(ctx_index);
      tmp.set(l,r,x);
      AR_write_internal(ctx_index,tmp);
      {
        bool change = foobar_check_change(ctx_index);
        CondRegTrace(AR_trace(ctx_index));
        foobar_change_back(change);
      }
    }
    void AR_ctx_slice_set(uint32_t ctx_index ,unsigned l,unsigned r,uint64_t x) {
      AR_ctx_slice_set(ctx_index ,l,r,bits<32>(x));
    }
    
    // Register AR_R
    void AR_R_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(1,"AR_R",AR_R().get_data().rnum());
      }
    }
    
    void AR_R_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(1,"AR_R",AR_R().get_data().rnum());
      }
    }
    
    void AR_R_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(1,"AR_R",AR_R().get_data().rnum(),mask.rnum());
      }
    }
    
    void AR_R_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(1,"AR_R",AR_R().get_data().rnum(),mask.rnum());
      }
    }
    
    void AR_R_write_internal(const bits<32> &x) {
      AR_R().set_data() = x;
    };
    void AR_R_write(const bits<32> &x) {
      AR_R_write_internal(x);
      CondRegTrace(AR_R_trace());
    };
    bits<32> & AR_R_read_internal() {
      return AR_R().get_data();
    };
    
    bits<32> & AR_R_read()  ATTRIBUTE_USED {
      CondRegTrace(AR_R_read_trace());
      return AR_R_read_internal();
    }
    bits<32> & AR_R_read_notrace() {
      return AR_R_read_internal();
    }
    void AR_R_reset_value() {
      bits<32> x("");
      AR_R().set_value(x);
      AR_R().reset_tag();
    };
    
    // Register AR_W
    void AR_W_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(2,"AR_W",AR_W().get_data().rnum());
      }
    }
    
    void AR_W_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(2,"AR_W",AR_W().get_data().rnum());
      }
    }
    
    void AR_W_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(2,"AR_W",AR_W().get_data().rnum(),mask.rnum());
      }
    }
    
    void AR_W_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(2,"AR_W",AR_W().get_data().rnum(),mask.rnum());
      }
    }
    
    void AR_W_write_internal(const bits<32> &x) {
      AR_W().set_data() = x;
    };
    void AR_W_write(const bits<32> &x) {
      AR_W_write_internal(x);
      CondRegTrace(AR_W_trace());
    };
    bits<32> & AR_W_read_internal() {
      return AR_W().get_data();
    };
    
    bits<32> & AR_W_read()  ATTRIBUTE_USED {
      CondRegTrace(AR_W_read_trace());
      return AR_W_read_internal();
    }
    bits<32> & AR_W_read_notrace() {
      return AR_W_read_internal();
    }
    void AR_W_reset_value() {
      bits<32> x("");
      AR_W().set_value(x);
      AR_W().reset_tag();
    };
    
    // Register BCCNT
    void BCCNT_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(3,"BCCNT",BCCNT().get_data().rnum());
      }
    }
    
    void BCCNT_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(3,"BCCNT",BCCNT().get_data().rnum());
      }
    }
    
    void BCCNT_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(3,"BCCNT",BCCNT().get_data().rnum(),mask.rnum());
      }
    }
    
    void BCCNT_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(3,"BCCNT",BCCNT().get_data().rnum(),mask.rnum());
      }
    }
    
    void BCCNT_write_internal(const bits<32> &x) {
      BCCNT().set_data() = x;
    };
    void BCCNT_write(const bits<32> &x) {
      BCCNT_write_internal(x);
      CondRegTrace(BCCNT_trace());
    };
    bits<32> & BCCNT_read_internal() {
      return BCCNT().get_data();
    };
    
    bits<32> & BCCNT_read()  ATTRIBUTE_USED {
      CondRegTrace(BCCNT_read_trace());
      return BCCNT_read_internal();
    }
    bits<32> & BCCNT_read_notrace() {
      return BCCNT_read_internal();
    }
    void BCCNT_reset_value() {
      bits<32> x("");
      BCCNT().set_value(x);
      BCCNT().reset_tag();
    };
    
    // Register BR
    void BR_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(4,"BR",BR().get_data().rnum());
      }
    }
    
    void BR_trace(unsigned ctx_index)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(4,"BR",BR(ctx_index).get_data().rnum());
      }
    }
    
    void BR_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(4,"BR",BR().get_data().rnum());
      }
    }
    
    void BR_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(4,"BR",BR().get_data().rnum(),mask.rnum());
      }
    }
    
    void BR_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(4,"BR",BR().get_data().rnum(),mask.rnum());
      }
    }
    
    void BR_write_internal(uint32_t ctx_index ,const bits<32> &x) {
      BR(ctx_index).set_data() = x;
    };
    void BR_write(const bits<32> &x) {
      BR_write_internal(foobar._foobar_active,x);
      CondRegTrace(BR_trace());
    };
    void BR_write_inhook(uint32_t ctx_index ,const bits<32> &x)  ATTRIBUTE_USED {
      BR(ctx_index).set_data() = x;
      CondRegTrace(BR_trace());
    }
    void BR_ctx_write(uint32_t ctx_index ,const bits<32> &x)  ATTRIBUTE_USED {
      BR_write_internal(ctx_index,x);
      {
        bool change = foobar_check_change(ctx_index);
        CondRegTrace(BR_trace(ctx_index));
        foobar_change_back(change);
      }
    }
    bits<32> & BR_read_internal(uint32_t ctx_index) {
      return BR(ctx_index).get_data();
    };
    
    bits<32> & BR_read()  ATTRIBUTE_USED {
      CondRegTrace(BR_read_trace());
      return BR_read_internal(foobar._foobar_active);
    }
    bits<32> & BR_read_notrace() {
      return BR_read_internal(foobar._foobar_active);
    }
    bits<32> & BR_ctx_read(uint32_t ctx_index) {
      return BR_read_internal(ctx_index);
    }
    void BR_reset_value(uint32_t ctx_index) {
      bits<32> x("");
      BR().set_value(x);
      BR().reset_tag();
    };
    bits<16> BR_B_read()  ATTRIBUTE_USED {
      CondRegTrace(
      {
        bits<32> mask;
        mask.mask(16,31);
        BR_read_trace(mask);
      }
      );
      return BR_read_notrace(). get<16>(16,31);
    };
    
    void BR_B_write(const bits<16> &x)  ATTRIBUTE_USED {
      BR_read_notrace().set(16,31,x);
      CondRegTrace(
      {
        bits<32> mask;
        mask.mask(16,31);
        BR_trace(mask);
      }
      );
    };
    bits<16> BR_B_ctx_read(unsigned ctx_index) {
      return BR_read_internal(ctx_index). get<16>(16,31);
    };
    void BR_B_ctx_write(unsigned ctx_index,const bits<16> &x) {
      BR_B_write_inhook(ctx_index,x);
      {
        bool change = foobar_check_change(ctx_index);
        CondRegTrace(BR_trace(ctx_index));
        foobar_change_back(change);
      }
    }
    void BR_B_write_inhook(uint32_t ctx_index ,const bits<16> &x)  ATTRIBUTE_USED {
      BR(ctx_index).set_data().set(16,31,x);
      CondRegTrace(
      {
        bits<32> mask;
        mask.mask(16,31);
        BR_trace(mask);
      }
      );
    };
    void BR_C_write(unsigned i,const bits<8> &x)  ATTRIBUTE_USED {
      BR_read_notrace().set<8>(((i%32)*8),(((i%32)*8)+8-1),x);
      CondRegTrace(
      {
        bits<32> mask;
        mask.mask(((i%32)*8),(((i%32)*8)+8-1));
        BR_trace(mask);
      }
      );
    };
    void BR_C_ctx_write(unsigned ctx_index,unsigned i,const bits<8> &x) {
      BR_C_write_inhook(ctx_index,i,x);
      {
        bool change = foobar_check_change(ctx_index);
        CondRegTrace(BR_trace(ctx_index));
        foobar_change_back(change);
      }
    }
    void BR_C_write_inhook(uint32_t ctx_index ,unsigned i,const bits<8> &x)  ATTRIBUTE_USED {
      BR(ctx_index).set_data().set<8>(((i%32)*8),(((i%32)*8)+8-1),x);
      CondRegTrace(
      {
        bits<32> mask;
        mask.mask(((i%32)*8),(((i%32)*8)+8-1));
        BR_trace(mask);
      }
      );
    };
    
    // Register BranchTaken
    void BranchTaken_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(5,"BranchTaken",BranchTaken().get_data().rnum());
      }
    }
    
    void BranchTaken_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(5,"BranchTaken",BranchTaken().get_data().rnum());
      }
    }
    
    void BranchTaken_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(5,"BranchTaken",BranchTaken().get_data().rnum(),mask.rnum());
      }
    }
    
    void BranchTaken_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(5,"BranchTaken",BranchTaken().get_data().rnum(),mask.rnum());
      }
    }
    
    void BranchTaken_write_internal(const bits<32> &x) {
      BranchTaken().set_data() = x;
    };
    void BranchTaken_write(const bits<32> &x) {
      BranchTaken_write_internal(x);
      CondRegTrace(BranchTaken_trace());
    };
    bits<32> & BranchTaken_read_internal() {
      return BranchTaken().get_data();
    };
    
    bits<32> & BranchTaken_read()  ATTRIBUTE_USED {
      CondRegTrace(BranchTaken_read_trace());
      return BranchTaken_read_internal();
    }
    bits<32> & BranchTaken_read_notrace() {
      return BranchTaken_read_internal();
    }
    void BranchTaken_reset_value() {
      bits<32> x("");
      BranchTaken().set_value(x);
      BranchTaken().reset_tag();
    };
    
    // Register CIA
    void CIA_trace()  ATTRIBUTE_USED {
    }
    
    void CIA_read_trace()  ATTRIBUTE_USED {
    }
    
    void CIA_trace(const bits<32> &mask)  ATTRIBUTE_USED {
    }
    
    void CIA_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
    }
    
    void CIA_write_internal(const bits<32> &x) {
      CIA().set_data() = x;
    };
    void CIA_write(const bits<32> &x) {
      CIA_write_internal(x);
      CondRegTrace(CIA_trace());
    };
    bits<32> & CIA_read_internal() {
      return CIA().get_data();
    };
    
    bits<32> & CIA_read()  ATTRIBUTE_USED {
      CondRegTrace(CIA_read_trace());
      return CIA_read_internal();
    }
    bits<32> & CIA_read_notrace() {
      return CIA_read_internal();
    }
    void CIA_reset_value() {
      bits<32> x("");
      CIA().set_value(x);
      CIA().reset_tag();
    };
    
    // Register CR
    void CR_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(7,"CR",CR().get_data().rnum());
      }
    }
    
    void CR_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(7,"CR",CR().get_data().rnum());
      }
    }
    
    void CR_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(7,"CR",CR().get_data().rnum(),mask.rnum());
      }
    }
    
    void CR_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(7,"CR",CR().get_data().rnum(),mask.rnum());
      }
    }
    
    bits<32> & CR_read_internal() {
      return CR().get_data();
    };
    
    bits<32> & CR_read()  ATTRIBUTE_USED {
      CondRegTrace(CR_read_trace());
      return CR_read_internal();
    }
    bits<32> & CR_read_notrace() {
      return CR_read_internal();
    }
    void CR_reset_value() {
      bits<32> x("");
      CR().set_value(x);
      CR().reset_tag();
    };
    void CR_cr_write(unsigned i,const bits<4> &x)  ATTRIBUTE_USED {
      CR_read_notrace().set<4>(((i%32)*4),(((i%32)*4)+4-1),x);
      CondRegTrace(
      {
        bits<32> mask;
        mask.mask(((i%32)*4),(((i%32)*4)+4-1));
        CR_trace(mask);
      }
      );
    };
    template <size_t Nb>
    void CR_slice_set_internal(unsigned l,unsigned r,const bits<Nb> &x) {
      CR_read_notrace().set(l,r,x);
    };
    template <size_t Nb>
    void CR_slice_set(unsigned l,unsigned r,const bits<Nb> &x) {
      CR_slice_set_internal(l,r,x);
      CondRegTrace(
      {
        bits<32> mask;
        mask.mask(l,r);
        CR_trace(mask);
      }
      );
    }
    
    void CR_slice_set(unsigned l,unsigned r,uint64_t x) {
      CR_slice_set(l,r,bits<32>(x));
    };
    
    // Register CRFL5
    void CRFL5_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(8,"CRFL5",CRF_read_internal(5).rnum());
      }
    }
    
    void CRFL5_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
      }
    }
    
    void CRFL5_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(8,"CRFL5",CRF_read_internal(5).rnum(),mask.rnum());
      }
    }
    
    void CRFL5_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
      }
    }
    
    void CRFL5_write_internal(const bits<32> &x) {
      CRF_write_internal(foobar._foobar_active,5, x);
    };
    void CRFL5_write(const bits<32> &x) {
      CRFL5_write_internal(x);
      CondRegTrace(CRFL5_trace());
    };
    void CRFL5_write_inhook(const bits<32> &x)  ATTRIBUTE_USED {
      CRFL5_write_internal(x);
    }
    const bits<32> & CRFL5_read_internal()  ATTRIBUTE_USED {
      return CRF_read_internal(foobar._foobar_active,5);
    };
    
    const bits<32> & CRFL5_read()  ATTRIBUTE_USED {
      CondRegTrace(CRFL5_read_trace());
      return CRFL5_read_internal();
    }
    const bits<32> & CRFL5_read_notrace() {
      return CRFL5_read_internal();
    }
    void CRFL5_reset_value() {
      bits<32> x("");
      CRF_write_internal(foobar._foobar_active,5, x);
    };
    
    // Register CSRR0
    void CSRR0_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(9,"CSRR0",CSRR0().get_data().rnum());
      }
    }
    
    void CSRR0_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(9,"CSRR0",CSRR0().get_data().rnum());
      }
    }
    
    void CSRR0_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(9,"CSRR0",CSRR0().get_data().rnum(),mask.rnum());
      }
    }
    
    void CSRR0_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(9,"CSRR0",CSRR0().get_data().rnum(),mask.rnum());
      }
    }
    
    void CSRR0_write_internal(const bits<32> &x) {
      CSRR0().set_data() = x;
    };
    void CSRR0_write(const bits<32> &x) {
      CSRR0_write_internal(x);
      CondRegTrace(CSRR0_trace());
    };
    bits<32> & CSRR0_read_internal() {
      return CSRR0().get_data();
    };
    
    bits<32> & CSRR0_read()  ATTRIBUTE_USED {
      CondRegTrace(CSRR0_read_trace());
      return CSRR0_read_internal();
    }
    bits<32> & CSRR0_read_notrace() {
      return CSRR0_read_internal();
    }
    void CSRR0_reset_value() {
      bits<32> x("");
      CSRR0().set_value(x);
      CSRR0().reset_tag();
    };
    
    // Register CSRR1
    void CSRR1_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(10,"CSRR1",CSRR1().get_data().rnum());
      }
    }
    
    void CSRR1_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(10,"CSRR1",CSRR1().get_data().rnum());
      }
    }
    
    void CSRR1_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(10,"CSRR1",CSRR1().get_data().rnum(),mask.rnum());
      }
    }
    
    void CSRR1_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(10,"CSRR1",CSRR1().get_data().rnum(),mask.rnum());
      }
    }
    
    void CSRR1_write_internal(const bits<32> &x) {
      CSRR1().set_data() = x;
    };
    void CSRR1_write(const bits<32> &x) {
      CSRR1_write_internal(x);
      CondRegTrace(CSRR1_trace());
    };
    bits<32> & CSRR1_read_internal() {
      return CSRR1().get_data();
    };
    
    bits<32> & CSRR1_read()  ATTRIBUTE_USED {
      CondRegTrace(CSRR1_read_trace());
      return CSRR1_read_internal();
    }
    bits<32> & CSRR1_read_notrace() {
      return CSRR1_read_internal();
    }
    void CSRR1_reset_value() {
      bits<32> x("");
      CSRR1().set_value(x);
      CSRR1().reset_tag();
    };
    
    // Register CTR
    void CTR_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(11,"CTR",CTR().get_data().rnum());
      }
    }
    
    void CTR_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(11,"CTR",CTR().get_data().rnum());
      }
    }
    
    void CTR_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(11,"CTR",CTR().get_data().rnum(),mask.rnum());
      }
    }
    
    void CTR_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(11,"CTR",CTR().get_data().rnum(),mask.rnum());
      }
    }
    
    void CTR_write_internal(const bits<32> &x) {
      CTR().set_data() = x;
    };
    
    void CTR_write_delayed(const bits<32> &x,unsigned d) {
      decltype(&P::CTR_commit_write) pCommit = &P::CTR_commit_write;
      static PacketPool<CommitRegWrite<decltype(this),decltype(pCommit),32,1000>, 1000>   pool;
      CommitRegWrite<decltype(this),decltype(pCommit),32,1000> *cmt = pool.alloc();
      cmt->reset(this,pCommit,x,pool);
      _commitQueue.add(cmt,d);
    }
    
    void CTR_write(const bits<32> &x) {
      CTR_write_internal(x);
      CondRegTrace(CTR_trace());
    };
    void CTR_commit_write(const bits<32> &x) {
      CTR_write_internal(x);
      CondRegTrace(CTR_trace());
    };
    bits<32> & CTR_read_internal() {
      return CTR().get_data();
    };
    
    bits<32> & CTR_read()  ATTRIBUTE_USED {
      CondRegTrace(CTR_read_trace());
      return CTR_read_internal();
    }
    bits<32> & CTR_read_notrace() {
      return CTR_read_internal();
    }
    void CTR_reset_value() {
      bits<32> x("");
      CTR().set_value(x);
      CTR().reset_tag();
    };
    
    // Register DEC
    void DEC_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(12,"DEC",DEC().get_data().rnum());
      }
    }
    
    void DEC_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(12,"DEC",DEC().get_data().rnum());
      }
    }
    
    void DEC_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(12,"DEC",DEC().get_data().rnum(),mask.rnum());
      }
    }
    
    void DEC_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(12,"DEC",DEC().get_data().rnum(),mask.rnum());
      }
    }
    
    void DEC_write_internal(const bits<32> &x) {
      DEC().set_data() = x;
    };
    void DEC_write(const bits<32> &x) {
      DEC_write_internal(x);
      CondRegTrace(DEC_trace());
    };
    bits<32> & DEC_read_internal() {
      return DEC().get_data();
    };
    
    bits<32> & DEC_read()  ATTRIBUTE_USED {
      CondRegTrace(DEC_read_trace());
      return DEC_read_internal();
    }
    bits<32> & DEC_read_notrace() {
      return DEC_read_internal();
    }
    void DEC_reset_value() {
      bits<32> x("");
      DEC().set_value(x);
      DEC().reset_tag();
    };
    
    // Register DR
    void DR_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(13,"DR",DR().get_data().rnum());
      }
    }
    
    void DR_trace(unsigned ctx_index)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(13,"DR",DR(ctx_index).get_data().rnum());
      }
    }
    
    void DR_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(13,"DR",DR().get_data().rnum());
      }
    }
    
    void DR_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(13,"DR",DR().get_data().rnum(),mask.rnum());
      }
    }
    
    void DR_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(13,"DR",DR().get_data().rnum(),mask.rnum());
      }
    }
    
    void DR_write_internal(uint32_t ctx_index ,const bits<32> &x) {
      DR(ctx_index).set_data() = x;
    };
    void DR_write(const bits<32> &x) {
      DR_write_internal(foobar._foobar_active,x);
      CondRegTrace(DR_trace());
    };
    void DR_write_inhook(uint32_t ctx_index ,const bits<32> &x)  ATTRIBUTE_USED {
      DR(ctx_index).set_data() = x;
      CondRegTrace(DR_trace());
    }
    void DR_ctx_write(uint32_t ctx_index ,const bits<32> &x)  ATTRIBUTE_USED {
      DR_write_internal(ctx_index,x);
      {
        bool change = foobar_check_change(ctx_index);
        CondRegTrace(DR_trace(ctx_index));
        foobar_change_back(change);
      }
    }
    bits<32> & DR_read_internal(uint32_t ctx_index) {
      return DR(ctx_index).get_data();
    };
    
    bits<32> & DR_read()  ATTRIBUTE_USED {
      CondRegTrace(DR_read_trace());
      return DR_read_internal(foobar._foobar_active);
    }
    bits<32> & DR_read_notrace() {
      return DR_read_internal(foobar._foobar_active);
    }
    bits<32> & DR_ctx_read(uint32_t ctx_index) {
      return DR_read_internal(ctx_index);
    }
    void DR_reset_value(uint32_t ctx_index) {
      bits<32> x("");
      DR().set_value(x);
      DR().reset_tag();
    };
    
    // Register DRL
    void DRL_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        DR_trace();
      }
    }
    
    void DRL_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        DR_read_trace();
      }
    }
    
    void DRL_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        DR_trace(mask);
      }
    }
    
    void DRL_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        DR_read_trace(mask);
      }
    }
    
    void DRL_write_internal(const bits<32> &x) {
      DR_write_internal(foobar._foobar_active,x);
    };
    void DRL_write(const bits<32> &x) {
      DRL_write_internal(x);
      CondRegTrace(DRL_trace());
    };
    void DRL_write_inhook(const bits<32> &x)  ATTRIBUTE_USED {
      DRL_write_internal(x);
    }
    const bits<32> & DRL_read_internal()  ATTRIBUTE_USED {
      return DR_read_internal(foobar._foobar_active);
    };
    
    const bits<32> & DRL_read()  ATTRIBUTE_USED {
      CondRegTrace(DRL_read_trace());
      return DRL_read_internal();
    }
    const bits<32> & DRL_read_notrace() {
      return DRL_read_internal();
    }
    void DRL_reset_value() {
      bits<32> x("");
      DR_write_internal(foobar._foobar_active,x);
    };
    
    // Register ESR
    void ESR_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(15,"ESR",ESR().get_data().rnum());
      }
    }
    
    void ESR_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(15,"ESR",ESR().get_data().rnum());
      }
    }
    
    void ESR_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(15,"ESR",ESR().get_data().rnum(),mask.rnum());
      }
    }
    
    void ESR_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(15,"ESR",ESR().get_data().rnum(),mask.rnum());
      }
    }
    
    void ESR_write_internal(const bits<32> &x) {
      ESR().set_data() = x;
    };
    void ESR_write(const bits<32> &x) {
      ESR_write_internal(x);
      CondRegTrace(ESR_trace());
    };
    bits<32> & ESR_read_internal() {
      return ESR().get_data();
    };
    
    bits<32> & ESR_read()  ATTRIBUTE_USED {
      CondRegTrace(ESR_read_trace());
      return ESR_read_internal();
    }
    bits<32> & ESR_read_notrace() {
      return ESR_read_internal();
    }
    void ESR_reset_value() {
      bits<32> x("");
      ESR().set_value(x);
      ESR().reset_tag();
    };
    
    // Register EWTEST
    void EWTEST_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(16,"EWTEST",EWTEST().get_data().rnum());
      }
    }
    
    void EWTEST_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(16,"EWTEST",EWTEST().get_data().rnum());
      }
    }
    
    void EWTEST_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(16,"EWTEST",EWTEST().get_data().rnum(),mask.rnum());
      }
    }
    
    void EWTEST_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(16,"EWTEST",EWTEST().get_data().rnum(),mask.rnum());
      }
    }
    
    void EWTEST_write_internal(const bits<32> &x) {
      EWTEST().set_data() = x;
    };
    void EWTEST_write(const bits<32> &x) {
      EWTEST_write_internal(x);
      CondRegTrace(EWTEST_trace());
    };
    void EWTEST_write_inhook(const bits<32> &x)  ATTRIBUTE_USED {
      EWTEST().set_data() = x;
      CondRegTrace(EWTEST_trace());
    }
    void EWTEST_external_write(bits < 32 > x) {
      {
            EWTEST_write_inhook (   EWTEST_read_inhook() & ~ x  ) ;
      }
    }
    
    bits<32> & EWTEST_read_internal() {
      return EWTEST().get_data();
    };
    
    bits<32> &EWTEST_read_inhook()  ATTRIBUTE_USED {
      return EWTEST().get_data();
    }
    
    bits<32> & EWTEST_read()  ATTRIBUTE_USED {
      CondRegTrace(EWTEST_read_trace());
      return EWTEST_read_internal();
    }
    bits<32> & EWTEST_read_notrace() {
      return EWTEST_read_internal();
    }
    void EWTEST_reset_value() {
      bits<32> x("");
      EWTEST().set_value(x);
      EWTEST().reset_tag();
    };
    
    // Register FBCTRL
    void FBCTRL_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(17,"FBCTRL",FBCTRL().get_data().rnum());
      }
    }
    
    void FBCTRL_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(17,"FBCTRL",FBCTRL().get_data().rnum());
      }
    }
    
    void FBCTRL_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(17,"FBCTRL",FBCTRL().get_data().rnum(),mask.rnum());
      }
    }
    
    void FBCTRL_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(17,"FBCTRL",FBCTRL().get_data().rnum(),mask.rnum());
      }
    }
    
    bits<32> & FBCTRL_read_internal() {
      return FBCTRL().get_data();
    };
    
    bits<32> & FBCTRL_read()  ATTRIBUTE_USED {
      CondRegTrace(FBCTRL_read_trace());
      return FBCTRL_read_internal();
    }
    bits<32> & FBCTRL_read_notrace() {
      return FBCTRL_read_internal();
    }
    void FBCTRL_reset_value() {
      bits<32> x("");
      FBCTRL().set_value(x);
      FBCTRL().reset_tag();
    };
    
    // Register FCNT
    void FCNT_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(18,"FCNT",FCNT().get_data().rnum());
      }
    }
    
    void FCNT_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(18,"FCNT",FCNT().get_data().rnum());
      }
    }
    
    void FCNT_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(18,"FCNT",FCNT().get_data().rnum(),mask.rnum());
      }
    }
    
    void FCNT_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(18,"FCNT",FCNT().get_data().rnum(),mask.rnum());
      }
    }
    
    void FCNT_write_internal(const bits<32> &x) {
      FCNT().set_data() = x;
    };
    void FCNT_write(const bits<32> &x) {
      FCNT_write_internal(x);
      CondRegTrace(FCNT_trace());
    };
    bits<32> & FCNT_read_internal() {
      return FCNT().get_data();
    };
    
    bits<32> & FCNT_read()  ATTRIBUTE_USED {
      CondRegTrace(FCNT_read_trace());
      return FCNT_read_internal();
    }
    bits<32> & FCNT_read_notrace() {
      return FCNT_read_internal();
    }
    void FCNT_reset_value() {
      bits<32> x("");
      FCNT().set_value(x);
      FCNT().reset_tag();
    };
    
    // Register FOO1
    void FOO1_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(19,"FOO1",FOO1().get_data().rnum());
      }
    }
    
    void FOO1_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(19,"FOO1",FOO1().get_data().rnum());
      }
    }
    
    void FOO1_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(19,"FOO1",FOO1().get_data().rnum(),mask.rnum());
      }
    }
    
    void FOO1_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(19,"FOO1",FOO1().get_data().rnum(),mask.rnum());
      }
    }
    
    void FOO1_write_internal(const bits<32> &x) {
      FOO1().set_data() = ((x & ~FOO1()._reserved_mask) & ~FOO1()._readonly_mask) | (FOO1().get_data() & FOO1()._readonly_mask);
    };
    
    void FOO1_write_delayed(const bits<32> &x,unsigned d) {
      decltype(&P::FOO1_commit_write) pCommit = &P::FOO1_commit_write;
      static PacketPool<CommitRegWrite<decltype(this),decltype(pCommit),32,1000>, 1000>   pool;
      CommitRegWrite<decltype(this),decltype(pCommit),32,1000> *cmt = pool.alloc();
      cmt->reset(this,pCommit,x,pool);
      _commitQueue.add(cmt,d);
    }
    
    void FOO1_write(const bits<32> &x) {
      FOO1_write_internal(x);
      CondRegTrace(FOO1_trace());
    };
    void FOO1_commit_write(const bits<32> &x) {
      FOO1_write_internal(x);
      CondRegTrace(FOO1_trace());
    };
    bits<32> & FOO1_read_internal() {
      return FOO1().get_data();
    };
    
    bits<32> & FOO1_read()  ATTRIBUTE_USED {
      CondRegTrace(FOO1_read_trace());
      return FOO1_read_internal();
    }
    bits<32> & FOO1_read_notrace() {
      return FOO1_read_internal();
    }
    void FOO1_reset_value() {
      bits<32> x("0xdead1100");
      FOO1().set_value(x);
      FOO1().reset_tag();
    };
    bits<8> FOO1_A_read()  ATTRIBUTE_USED {
      CondRegTrace(
      {
        bits<32> mask;
        mask.mask(0,7);
        FOO1_read_trace(mask);
      }
      );
      return FOO1_read_notrace(). get<8>(0,7);
    };
    
    
    void FOO1_C_write_delayed(const bits<8> &x,unsigned d)  ATTRIBUTE_USED {
      decltype(&P::FOO1_C_commit_write) pCommit = &P::FOO1_C_commit_write;
      static PacketPool<CommitRegFieldWrite<decltype(this),decltype(pCommit),32,1000>, 1000>   pool;
      CommitRegFieldWrite<decltype(this),decltype(pCommit),32,1000> *cmt = pool.alloc();
      cmt->reset(this,pCommit,x,pool);
      _commitQueue.add(cmt,d);
    }
    
    void FOO1_C_write(const bits<8> &x)  ATTRIBUTE_USED {
      FOO1_C_commit_write(x);
    }
    
    void FOO1_C_commit_write(const bits<8> &x)  ATTRIBUTE_USED {
      FOO1_read_notrace().set(16,23,x);
      CondRegTrace(
      {
        bits<32> mask;
        mask.mask(16,23);
        FOO1_trace(mask);
      }
      );
    };
    
    // Register FOO10
    void FOO10_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        FOO9_trace();
      }
    }
    
    void FOO10_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        FOO9_read_trace();
      }
    }
    
    void FOO10_trace(const bits<16> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        FOO9_trace(mask);
      }
    }
    
    void FOO10_read_trace(const bits<16> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        FOO9_read_trace(mask);
      }
    }
    
    void FOO10_write_internal(const bits<16> &x) {
      FOO9_slice_set(0,15,x);
    };
    void FOO10_write(const bits<16> &x) {
      FOO10_write_internal(x);
      CondRegTrace(FOO10_trace());
    };
    void FOO10_write_inhook(const bits<16> &x)  ATTRIBUTE_USED {
      FOO10_write_internal(x);
    }
    bits<16> FOO10_read_internal()  ATTRIBUTE_USED {
      return FOO9_read_internal()(0,15);
    };
    
    bits<16> FOO10_read()  ATTRIBUTE_USED {
      CondRegTrace(FOO10_read_trace());
      return FOO10_read_internal();
    }
    bits<16> FOO10_read_notrace() {
      return FOO10_read_internal();
    }
    void FOO10_reset_value() {
      bits<16> x("");
      FOO9_write_internal(x);
    };
    
    // Register FOO11
    void FOO11_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(21,"FOO11",FOO11().get_data().rnum());
      }
    }
    
    void FOO11_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(21,"FOO11",FOO11().get_data().rnum());
      }
    }
    
    void FOO11_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(21,"FOO11",FOO11().get_data().rnum(),mask.rnum());
      }
    }
    
    void FOO11_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(21,"FOO11",FOO11().get_data().rnum(),mask.rnum());
      }
    }
    
    void FOO11_write_internal(const bits<32> &x)  ATTRIBUTE_USED {
      FOO11_write_hook( x);
    };
    void FOO11_write_hook(bits < 32 > x)  ATTRIBUTE_USED {
      {
            FOO11_write_inhook (  x ) ;
            raiseException ( Program ) ;
      }
    };
    void FOO11_write(const bits<32> &x) {
      FOO11_write_internal(x);
    };
    void FOO11_write_inhook(const bits<32> &x)  ATTRIBUTE_USED {
      FOO11().set_data() = x;
      CondRegTrace(FOO11_trace());
    }
    bits<32> & FOO11_read_internal() {
      return FOO11().get_data();
    };
    
    bits<32> &FOO11_read_inhook()  ATTRIBUTE_USED {
      return FOO11().get_data();
    }
    
    bits<32> & FOO11_read()  ATTRIBUTE_USED {
      CondRegTrace(FOO11_read_trace());
      return FOO11_read_internal();
    }
    bits<32> & FOO11_read_notrace() {
      return FOO11_read_internal();
    }
    void FOO11_reset_value() {
      bits<32> x("");
      FOO11().set_value(x);
      FOO11().reset_tag();
    };
    
    // Register FOO12
    void FOO12_reset_value() {
      bits<32> x("");
      FOO12().set_value(x);
      FOO12().reset_tag();
    };
    
    // Register FOO13
    void FOO13_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(23,"FOO13",FOO13().get_data().rnum());
      }
    }
    
    void FOO13_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(23,"FOO13",FOO13().get_data().rnum());
      }
    }
    
    void FOO13_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(23,"FOO13",FOO13().get_data().rnum(),mask.rnum());
      }
    }
    
    void FOO13_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(23,"FOO13",FOO13().get_data().rnum(),mask.rnum());
      }
    }
    
    void FOO13_write_internal(const bits<32> &x)  ATTRIBUTE_USED {
      FOO13_write_hook( x);
    };
    void FOO13_write_hook(bits < 32 > x)  ATTRIBUTE_USED {
      {
            if ( MSR_read() == 0 ) {
                  FOO13_write_inhook (  x ) ;
              }
      }
    };
    void FOO13_write(const bits<32> &x) {
      FOO13_write_internal(x);
    };
    void FOO13_write_inhook(const bits<32> &x)  ATTRIBUTE_USED {
      FOO13().set_data() = x;
      CondRegTrace(FOO13_trace());
    }
    bits<32> FOO13_read_internal()  ATTRIBUTE_USED {
      {
            if ( MSR_read() == 0 ) {
                  return FOO13_read_inhook() ;
              } else {
                  return 0 ;
              }
      }
    };
    
    bits<32> &FOO13_read_inhook()  ATTRIBUTE_USED {
      return FOO13().get_data();
    }
    
    bits<32> FOO13_read()  ATTRIBUTE_USED {
      CondRegTrace(FOO13_read_trace());
      return FOO13_read_internal();
    }
    bits<32> FOO13_read_notrace() {
      return FOO13_read_internal();
    }
    void FOO13_reset_value() {
      bits<32> x("");
      FOO13().set_value(x);
      FOO13().reset_tag();
    };
    
    // Register FOO14
    void FOO14_reset_value() {
      bits<32> x("");
      FOO14().set_value(x);
      FOO14().reset_tag();
    };
    
    // Register FOO2
    void FOO2_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(25,"FOO2",FOO2().get_data().rnum());
      }
    }
    
    void FOO2_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(25,"FOO2",FOO2().get_data().rnum());
      }
    }
    
    void FOO2_trace(const bits<64> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(25,"FOO2",FOO2().get_data().rnum(),mask.rnum());
      }
    }
    
    void FOO2_read_trace(const bits<64> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(25,"FOO2",FOO2().get_data().rnum(),mask.rnum());
      }
    }
    
    void FOO2_write_internal(const bits<64> &x) {
      FOO2().set_data() = x;
    };
    
    void FOO2_write_delayed(const bits<64> &x,unsigned d) {
      decltype(&P::FOO2_commit_write) pCommit = &P::FOO2_commit_write;
      static PacketPool<CommitRegWrite<decltype(this),decltype(pCommit),64,1000>, 1000>   pool;
      CommitRegWrite<decltype(this),decltype(pCommit),64,1000> *cmt = pool.alloc();
      cmt->reset(this,pCommit,x,pool);
      _commitQueue.add(cmt,d);
    }
    
    void FOO2_write(const bits<64> &x) {
      FOO2_write_internal(x);
      CondRegTrace(FOO2_trace());
    };
    void FOO2_commit_write(const bits<64> &x) {
      FOO2_write_internal(x);
      CondRegTrace(FOO2_trace());
    };
    bits<64> & FOO2_read_internal() {
      return FOO2().get_data();
    };
    
    bits<64> & FOO2_read()  ATTRIBUTE_USED {
      CondRegTrace(FOO2_read_trace());
      return FOO2_read_internal();
    }
    bits<64> & FOO2_read_notrace() {
      return FOO2_read_internal();
    }
    void FOO2_reset_value() {
      bits<64> x("");
      FOO2().set_value(x);
      FOO2().reset_tag();
    };
    bits<8> FOO2_X_read(unsigned i)  ATTRIBUTE_USED {
      CondRegTrace(
      {
        bits<64> mask;
        mask.mask(((i%64)*8),(((i%64)*8)+8-1));
        FOO2_read_trace(mask);
      }
      );
      return FOO2_read_notrace(). get<8>(((i%64)*8),(((i%64)*8)+8-1));
    };
    
    void FOO2_X_write_delayed(unsigned i,const bits<8> &x,unsigned d)  ATTRIBUTE_USED {
      decltype(&P::FOO2_X_commit_write) pCommit = &P::FOO2_X_commit_write;
      static PacketPool<CommitRegIndexedFieldWrite<decltype(this),decltype(pCommit),64,1000>, 1000>   pool;
      CommitRegIndexedFieldWrite<decltype(this),decltype(pCommit),64,1000> *cmt = pool.alloc();
      cmt->reset(this,pCommit,i,x,pool);
      _commitQueue.add(cmt,d);
    }
    
    void FOO2_X_write(unsigned i,const bits<8> &x)  ATTRIBUTE_USED {
      FOO2_X_commit_write(i,x);
    }
    
    void FOO2_X_commit_write(unsigned i,const bits<8> &x)  ATTRIBUTE_USED {
      FOO2_read_notrace().set<8>(((i%64)*8),(((i%64)*8)+8-1),x);
      CondRegTrace(
      {
        bits<64> mask;
        mask.mask(((i%64)*8),(((i%64)*8)+8-1));
        FOO2_trace(mask);
      }
      );
    };
    bits<16> FOO2_Y_read(unsigned i)  ATTRIBUTE_USED {
      CondRegTrace(
      {
        bits<64> mask;
        mask.mask(((i%64)*16),(((i%64)*16)+16-1));
        FOO2_read_trace(mask);
      }
      );
      return FOO2_read_notrace(). get<16>(((i%64)*16),(((i%64)*16)+16-1));
    };
    
    void FOO2_Y_write_delayed(unsigned i,const bits<16> &x,unsigned d)  ATTRIBUTE_USED {
      decltype(&P::FOO2_Y_commit_write) pCommit = &P::FOO2_Y_commit_write;
      static PacketPool<CommitRegIndexedFieldWrite<decltype(this),decltype(pCommit),64,1000>, 1000>   pool;
      CommitRegIndexedFieldWrite<decltype(this),decltype(pCommit),64,1000> *cmt = pool.alloc();
      cmt->reset(this,pCommit,i,x,pool);
      _commitQueue.add(cmt,d);
    }
    
    void FOO2_Y_write(unsigned i,const bits<16> &x)  ATTRIBUTE_USED {
      FOO2_Y_commit_write(i,x);
    }
    
    void FOO2_Y_commit_write(unsigned i,const bits<16> &x)  ATTRIBUTE_USED {
      FOO2_read_notrace().set<16>(((i%64)*16),(((i%64)*16)+16-1),x);
      CondRegTrace(
      {
        bits<64> mask;
        mask.mask(((i%64)*16),(((i%64)*16)+16-1));
        FOO2_trace(mask);
      }
      );
    };
    bits<32> FOO2_Z_read(unsigned i)  ATTRIBUTE_USED {
      CondRegTrace(
      {
        bits<64> mask;
        mask.mask(((i%64)*32),(((i%64)*32)+32-1));
        FOO2_read_trace(mask);
      }
      );
      return FOO2_read_notrace(). get<32>(((i%64)*32),(((i%64)*32)+32-1));
    };
    
    void FOO2_Z_write_delayed(unsigned i,const bits<32> &x,unsigned d)  ATTRIBUTE_USED {
      decltype(&P::FOO2_Z_commit_write) pCommit = &P::FOO2_Z_commit_write;
      static PacketPool<CommitRegIndexedFieldWrite<decltype(this),decltype(pCommit),64,1000>, 1000>   pool;
      CommitRegIndexedFieldWrite<decltype(this),decltype(pCommit),64,1000> *cmt = pool.alloc();
      cmt->reset(this,pCommit,i,x,pool);
      _commitQueue.add(cmt,d);
    }
    
    void FOO2_Z_write(unsigned i,const bits<32> &x)  ATTRIBUTE_USED {
      FOO2_Z_commit_write(i,x);
    }
    
    void FOO2_Z_commit_write(unsigned i,const bits<32> &x)  ATTRIBUTE_USED {
      FOO2_read_notrace().set<32>(((i%64)*32),(((i%64)*32)+32-1),x);
      CondRegTrace(
      {
        bits<64> mask;
        mask.mask(((i%64)*32),(((i%64)*32)+32-1));
        FOO2_trace(mask);
      }
      );
    };
    
    // Register FOO3
    void FOO3_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(26,"FOO3",FOO3().get_data().rnum());
      }
    }
    
    void FOO3_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(26,"FOO3",FOO3().get_data().rnum());
      }
    }
    
    void FOO3_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(26,"FOO3",FOO3().get_data().rnum(),mask.rnum());
      }
    }
    
    void FOO3_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(26,"FOO3",FOO3().get_data().rnum(),mask.rnum());
      }
    }
    
    void FOO3_write_internal(const bits<32> &x) {
      FOO3().set_data() = x;
    };
    void FOO3_write(const bits<32> &x) {
      FOO3_write_internal(x);
      CondRegTrace(FOO3_trace());
    };
    bits<32> & FOO3_read_internal() {
      return FOO3().get_data();
    };
    
    bits<32> & FOO3_read()  ATTRIBUTE_USED {
      CondRegTrace(FOO3_read_trace());
      return FOO3_read_internal();
    }
    bits<32> & FOO3_read_notrace() {
      return FOO3_read_internal();
    }
    void FOO3_reset_value() {
      bits<32> x("");
      FOO3().set_value(x);
      FOO3().reset_tag();
    };
    
    // Register FOO4
    void FOO4_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(27,"FOO4",GPR_read_internal(2).rnum());
      }
    }
    
    void FOO4_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
      }
    }
    
    void FOO4_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(27,"FOO4",GPR_read_internal(2).rnum(),mask.rnum());
      }
    }
    
    void FOO4_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
      }
    }
    
    void FOO4_write_internal(const bits<32> &x) {
      GPR_write_internal(2, x);
    };
    void FOO4_write(const bits<32> &x) {
      FOO4_write_internal(x);
      CondRegTrace(FOO4_trace());
    };
    void FOO4_write_inhook(const bits<32> &x)  ATTRIBUTE_USED {
      FOO4_write_internal(x);
    }
    const bits<32> & FOO4_read_internal()  ATTRIBUTE_USED {
      return GPR_read_internal(2);
    };
    
    const bits<32> & FOO4_read()  ATTRIBUTE_USED {
      CondRegTrace(FOO4_read_trace());
      return FOO4_read_internal();
    }
    const bits<32> & FOO4_read_notrace() {
      return FOO4_read_internal();
    }
    void FOO4_reset_value() {
      bits<32> x("");
      GPR_write_internal(2, x);
    };
    
    // Register FOO5
    void FOO5_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(28,"FOO5",FOO5().get_data().rnum());
      }
    }
    
    void FOO5_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(28,"FOO5",FOO5().get_data().rnum());
      }
    }
    
    void FOO5_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(28,"FOO5",FOO5().get_data().rnum(),mask.rnum());
      }
    }
    
    void FOO5_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(28,"FOO5",FOO5().get_data().rnum(),mask.rnum());
      }
    }
    
    void FOO5_write_internal(const bits<32> &x)  ATTRIBUTE_USED {
      FOO5_write_hook( x);
    };
    void FOO5_write_hook(bits < 32 > x)  ATTRIBUTE_USED {
      {
             FOO5_WR_write(  FOO5_WR_read() + 1  )  ;
            FOO5_write_inhook (  x ) ;
      }
    };
    void FOO5_write(const bits<32> &x) {
      FOO5_write_internal(x);
    };
    void FOO5_write_inhook(const bits<32> &x)  ATTRIBUTE_USED {
      FOO5().set_data() = x;
      CondRegTrace(FOO5_trace());
    }
    bits<32> FOO5_read_internal()  ATTRIBUTE_USED {
      {
             AdlTypeof( FOO5_read_inhook() ) x = FOO5_read_inhook() ;
            x = x + 0 ;
             FOO5_RD_write(  FOO5_RD_read() + 1  )  ;
            return x ;
      }
    };
    
    bits<32> &FOO5_read_inhook()  ATTRIBUTE_USED {
      return FOO5().get_data();
    }
    
    bits<32> FOO5_read()  ATTRIBUTE_USED {
      CondRegTrace(FOO5_read_trace());
      return FOO5_read_internal();
    }
    bits<32> FOO5_read_notrace() {
      return FOO5_read_internal();
    }
    void FOO5_reset_value() {
      bits<32> x("");
      FOO5().set_value(x);
      FOO5().reset_tag();
    };
    bits<16> FOO5_A_read()  ATTRIBUTE_USED {
      CondRegTrace(
      {
        bits<32> mask;
        mask.mask(0,15);
        FOO5_read_trace(mask);
      }
      );
      return FOO5_read_notrace(). get<16>(0,15);
    };
    
    void FOO5_A_write(const bits<16> &x)  ATTRIBUTE_USED {
      bits<32>tmp = FOO5_read();
      tmp.set(0,15,x);
      FOO5_write(tmp);
    };
    void FOO5_A_write_inhook(const bits<16> &x)  ATTRIBUTE_USED {
      FOO5().set_data().set(0,15,x);
      CondRegTrace(
      {
        bits<32> mask;
        mask.mask(0,15);
        FOO5_trace(mask);
      }
      );
    };
    bits<16> getfield_FOO5_AA(const bits<32> &x) {
      return x. get<16>(0,15);
    }
    
    bits<32> setfield_FOO5_AA(bits<32> x,const bits<16> &v) {
      x.set(0,15,v);
      return x;
    }
    
    template <size_t Nb>
    void FOO5_slice_set_internal(unsigned l,unsigned r,const bits<Nb> &x) {
      bits<32> tmp = FOO5_read();
      tmp.set(l,r,x);
      FOO5_write(tmp);
    };
    template <size_t Nb>
    void FOO5_slice_set(unsigned l,unsigned r,const bits<Nb> &x) {
      FOO5_slice_set_internal(l,r,x);
    }
    
    void FOO5_slice_set(unsigned l,unsigned r,uint64_t x) {
      FOO5_slice_set(l,r,bits<32>(x));
    };
    template <size_t Nb>
    void FOO5_slice_set_inhook(unsigned l,unsigned r,const bits<Nb> &x) {
      FOO5().set_data().set(l,r,x);
      CondRegTrace(
      {
        bits<32> mask;
        mask.mask(l,r);
        FOO5_trace(mask);
      }
      );
    }
    void FOO5_slice_set_inhook(unsigned l,unsigned r,uint64_t x) {
      FOO5_slice_set_inhook(l,r,bits<32>(x));
    };
    
    // Register FOO5_RD
    void FOO5_RD_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(29,"FOO5_RD",FOO5_RD().get_data().rnum());
      }
    }
    
    void FOO5_RD_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(29,"FOO5_RD",FOO5_RD().get_data().rnum());
      }
    }
    
    void FOO5_RD_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(29,"FOO5_RD",FOO5_RD().get_data().rnum(),mask.rnum());
      }
    }
    
    void FOO5_RD_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(29,"FOO5_RD",FOO5_RD().get_data().rnum(),mask.rnum());
      }
    }
    
    void FOO5_RD_write_internal(const bits<32> &x) {
      FOO5_RD().set_data() = x;
    };
    void FOO5_RD_write(const bits<32> &x) {
      FOO5_RD_write_internal(x);
      CondRegTrace(FOO5_RD_trace());
    };
    bits<32> & FOO5_RD_read_internal() {
      return FOO5_RD().get_data();
    };
    
    bits<32> & FOO5_RD_read()  ATTRIBUTE_USED {
      CondRegTrace(FOO5_RD_read_trace());
      return FOO5_RD_read_internal();
    }
    bits<32> & FOO5_RD_read_notrace() {
      return FOO5_RD_read_internal();
    }
    void FOO5_RD_reset_value() {
      bits<32> x("");
      FOO5_RD().set_value(x);
      FOO5_RD().reset_tag();
    };
    
    // Register FOO5_WR
    void FOO5_WR_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(30,"FOO5_WR",FOO5_WR().get_data().rnum());
      }
    }
    
    void FOO5_WR_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(30,"FOO5_WR",FOO5_WR().get_data().rnum());
      }
    }
    
    void FOO5_WR_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(30,"FOO5_WR",FOO5_WR().get_data().rnum(),mask.rnum());
      }
    }
    
    void FOO5_WR_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(30,"FOO5_WR",FOO5_WR().get_data().rnum(),mask.rnum());
      }
    }
    
    void FOO5_WR_write_internal(const bits<32> &x) {
      FOO5_WR().set_data() = x;
    };
    void FOO5_WR_write(const bits<32> &x) {
      FOO5_WR_write_internal(x);
      CondRegTrace(FOO5_WR_trace());
    };
    bits<32> & FOO5_WR_read_internal() {
      return FOO5_WR().get_data();
    };
    
    bits<32> & FOO5_WR_read()  ATTRIBUTE_USED {
      CondRegTrace(FOO5_WR_read_trace());
      return FOO5_WR_read_internal();
    }
    bits<32> & FOO5_WR_read_notrace() {
      return FOO5_WR_read_internal();
    }
    void FOO5_WR_reset_value() {
      bits<32> x("");
      FOO5_WR().set_value(x);
      FOO5_WR().reset_tag();
    };
    
    // Register FOO6
    void FOO6_reset_value() {
      bits<32> x("");
      FOO5_write_internal(x);
    };
    
    // Register FOO7
    void FOO7_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(32,"FOO7",FOO7().get_data().rnum());
      }
    }
    
    void FOO7_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(32,"FOO7",FOO7().get_data().rnum());
      }
    }
    
    void FOO7_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(32,"FOO7",FOO7().get_data().rnum(),mask.rnum());
      }
    }
    
    void FOO7_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(32,"FOO7",FOO7().get_data().rnum(),mask.rnum());
      }
    }
    
    void FOO7_write_internal(const bits<32> &x)  ATTRIBUTE_USED {
      FOO7_write_hook( x);
    };
    void FOO7_write_hook(bits < 32 > x)  ATTRIBUTE_USED {
      {
            FOO7_write_inhook (  x ) ;
      }
    };
    void FOO7_write(const bits<32> &x) {
      FOO7_write_internal(x);
    };
    void FOO7_write_inhook(const bits<32> &x)  ATTRIBUTE_USED {
      FOO7().set_data() = (x & ~FOO7()._reserved_mask);
      CondRegTrace(FOO7_trace());
    }
    void FOO7_reset_value() {
      bits<32> x("");
      FOO7().set_value(x);
      FOO7().reset_tag();
    };
    
    // Register FOO8
    void FOO8_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        FOO1_trace();
      }
    }
    
    void FOO8_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        FOO1_read_trace();
      }
    }
    
    void FOO8_trace(const bits<64> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        FOO1_trace(mask);
      }
    }
    
    void FOO8_read_trace(const bits<64> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        FOO1_read_trace(mask);
      }
    }
    
    void FOO8_write_internal(const bits<64> &x) {
      FOO1_write_internal(x);
    };
    void FOO8_write(const bits<64> &x) {
      FOO8_write_internal(x);
      CondRegTrace(FOO8_trace());
    };
    void FOO8_write_inhook(const bits<64> &x)  ATTRIBUTE_USED {
      FOO8_write_internal(x);
    }
    bits<64> FOO8_read_internal()  ATTRIBUTE_USED {
      return FOO1_read_internal();
    };
    
    bits<64> FOO8_read()  ATTRIBUTE_USED {
      CondRegTrace(FOO8_read_trace());
      return FOO8_read_internal();
    }
    bits<64> FOO8_read_notrace() {
      return FOO8_read_internal();
    }
    void FOO8_reset_value() {
      bits<64> x("");
      FOO1_write_internal(x);
    };
    
    // Register FOO9
    void FOO9_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(34,"FOO9",FOO9().get_data().rnum());
      }
    }
    
    void FOO9_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(34,"FOO9",FOO9().get_data().rnum());
      }
    }
    
    void FOO9_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(34,"FOO9",FOO9().get_data().rnum(),mask.rnum());
      }
    }
    
    void FOO9_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(34,"FOO9",FOO9().get_data().rnum(),mask.rnum());
      }
    }
    
    void FOO9_write_internal(const bits<32> &x) {
      FOO9().set_data() = x;
    };
    void FOO9_write(const bits<32> &x) {
      FOO9_write_internal(x);
      CondRegTrace(FOO9_trace());
    };
    bits<32> & FOO9_read_internal() {
      return FOO9().get_data();
    };
    
    bits<32> & FOO9_read()  ATTRIBUTE_USED {
      CondRegTrace(FOO9_read_trace());
      return FOO9_read_internal();
    }
    bits<32> & FOO9_read_notrace() {
      return FOO9_read_internal();
    }
    void FOO9_reset_value() {
      bits<32> x("");
      FOO9().set_value(x);
      FOO9().reset_tag();
    };
    template <size_t Nb>
    void FOO9_slice_set_internal(unsigned l,unsigned r,const bits<Nb> &x) {
      FOO9_read_notrace().set(l,r,x);
    };
    template <size_t Nb>
    void FOO9_slice_set(unsigned l,unsigned r,const bits<Nb> &x) {
      FOO9_slice_set_internal(l,r,x);
      CondRegTrace(
      {
        bits<32> mask;
        mask.mask(l,r);
        FOO9_trace(mask);
      }
      );
    }
    
    void FOO9_slice_set(unsigned l,unsigned r,uint64_t x) {
      FOO9_slice_set(l,r,bits<32>(x));
    };
    
    // Register ICNT
    void ICNT_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(35,"ICNT",ICNT().get_data().rnum());
      }
    }
    
    void ICNT_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(35,"ICNT",ICNT().get_data().rnum());
      }
    }
    
    void ICNT_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(35,"ICNT",ICNT().get_data().rnum(),mask.rnum());
      }
    }
    
    void ICNT_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(35,"ICNT",ICNT().get_data().rnum(),mask.rnum());
      }
    }
    
    void ICNT_write_internal(const bits<32> &x) {
      ICNT().set_data() = x;
    };
    void ICNT_write(const bits<32> &x) {
      ICNT_write_internal(x);
      CondRegTrace(ICNT_trace());
    };
    bits<32> & ICNT_read_internal() {
      return ICNT().get_data();
    };
    
    bits<32> & ICNT_read()  ATTRIBUTE_USED {
      CondRegTrace(ICNT_read_trace());
      return ICNT_read_internal();
    }
    bits<32> & ICNT_read_notrace() {
      return ICNT_read_internal();
    }
    void ICNT_reset_value() {
      bits<32> x("");
      ICNT().set_value(x);
      ICNT().reset_tag();
    };
    
    // Register ITEST
    void ITEST_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(36,"ITEST",ITEST().get_data().rnum());
      }
    }
    
    void ITEST_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(36,"ITEST",ITEST().get_data().rnum());
      }
    }
    
    void ITEST_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(36,"ITEST",ITEST().get_data().rnum(),mask.rnum());
      }
    }
    
    void ITEST_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(36,"ITEST",ITEST().get_data().rnum(),mask.rnum());
      }
    }
    
    bits<32> & ITEST_read_internal() {
      return ITEST().get_data();
    };
    
    bits<32> & ITEST_read()  ATTRIBUTE_USED {
      CondRegTrace(ITEST_read_trace());
      return ITEST_read_internal();
    }
    bits<32> & ITEST_read_notrace() {
      return ITEST_read_internal();
    }
    void ITEST_reset_value() {
      bits<32> x("");
      ITEST().set_value(x);
      ITEST().reset_tag();
    };
    
    // Register IVOR10
    void IVOR10_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(37,"IVOR10",IVOR10().get_data().rnum());
      }
    }
    
    void IVOR10_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(37,"IVOR10",IVOR10().get_data().rnum());
      }
    }
    
    void IVOR10_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(37,"IVOR10",IVOR10().get_data().rnum(),mask.rnum());
      }
    }
    
    void IVOR10_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(37,"IVOR10",IVOR10().get_data().rnum(),mask.rnum());
      }
    }
    
    bits<32> & IVOR10_read_internal() {
      return IVOR10().get_data();
    };
    
    bits<32> & IVOR10_read()  ATTRIBUTE_USED {
      CondRegTrace(IVOR10_read_trace());
      return IVOR10_read_internal();
    }
    bits<32> & IVOR10_read_notrace() {
      return IVOR10_read_internal();
    }
    void IVOR10_reset_value() {
      bits<32> x("");
      IVOR10().set_value(x);
      IVOR10().reset_tag();
    };
    
    // Register IVOR4
    void IVOR4_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(38,"IVOR4",IVOR4().get_data().rnum());
      }
    }
    
    void IVOR4_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(38,"IVOR4",IVOR4().get_data().rnum());
      }
    }
    
    void IVOR4_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(38,"IVOR4",IVOR4().get_data().rnum(),mask.rnum());
      }
    }
    
    void IVOR4_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(38,"IVOR4",IVOR4().get_data().rnum(),mask.rnum());
      }
    }
    
    bits<32> & IVOR4_read_internal() {
      return IVOR4().get_data();
    };
    
    bits<32> & IVOR4_read()  ATTRIBUTE_USED {
      CondRegTrace(IVOR4_read_trace());
      return IVOR4_read_internal();
    }
    bits<32> & IVOR4_read_notrace() {
      return IVOR4_read_internal();
    }
    void IVOR4_reset_value() {
      bits<32> x("");
      IVOR4().set_value(x);
      IVOR4().reset_tag();
    };
    
    // Register IVOR6
    void IVOR6_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(39,"IVOR6",IVOR6().get_data().rnum());
      }
    }
    
    void IVOR6_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(39,"IVOR6",IVOR6().get_data().rnum());
      }
    }
    
    void IVOR6_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(39,"IVOR6",IVOR6().get_data().rnum(),mask.rnum());
      }
    }
    
    void IVOR6_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(39,"IVOR6",IVOR6().get_data().rnum(),mask.rnum());
      }
    }
    
    bits<32> & IVOR6_read_internal() {
      return IVOR6().get_data();
    };
    
    bits<32> & IVOR6_read()  ATTRIBUTE_USED {
      CondRegTrace(IVOR6_read_trace());
      return IVOR6_read_internal();
    }
    bits<32> & IVOR6_read_notrace() {
      return IVOR6_read_internal();
    }
    void IVOR6_reset_value() {
      bits<32> x("");
      IVOR6().set_value(x);
      IVOR6().reset_tag();
    };
    
    // Register IVOR7
    void IVOR7_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(40,"IVOR7",IVOR7().get_data().rnum());
      }
    }
    
    void IVOR7_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(40,"IVOR7",IVOR7().get_data().rnum());
      }
    }
    
    void IVOR7_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(40,"IVOR7",IVOR7().get_data().rnum(),mask.rnum());
      }
    }
    
    void IVOR7_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(40,"IVOR7",IVOR7().get_data().rnum(),mask.rnum());
      }
    }
    
    bits<32> & IVOR7_read_internal() {
      return IVOR7().get_data();
    };
    
    bits<32> & IVOR7_read()  ATTRIBUTE_USED {
      CondRegTrace(IVOR7_read_trace());
      return IVOR7_read_internal();
    }
    bits<32> & IVOR7_read_notrace() {
      return IVOR7_read_internal();
    }
    void IVOR7_reset_value() {
      bits<32> x("");
      IVOR7().set_value(x);
      IVOR7().reset_tag();
    };
    
    // Register IVPR
    void IVPR_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(41,"IVPR",IVPR().get_data().rnum());
      }
    }
    
    void IVPR_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(41,"IVPR",IVPR().get_data().rnum());
      }
    }
    
    void IVPR_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(41,"IVPR",IVPR().get_data().rnum(),mask.rnum());
      }
    }
    
    void IVPR_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(41,"IVPR",IVPR().get_data().rnum(),mask.rnum());
      }
    }
    
    bits<32> & IVPR_read_internal() {
      return IVPR().get_data();
    };
    
    bits<32> & IVPR_read()  ATTRIBUTE_USED {
      CondRegTrace(IVPR_read_trace());
      return IVPR_read_internal();
    }
    bits<32> & IVPR_read_notrace() {
      return IVPR_read_internal();
    }
    void IVPR_reset_value() {
      bits<32> x("");
      IVPR().set_value(x);
      IVPR().reset_tag();
    };
    
    // Register MSR
    void MSR_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(42,"MSR",MSR().get_data().rnum());
      }
    }
    
    void MSR_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(42,"MSR",MSR().get_data().rnum());
      }
    }
    
    void MSR_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(42,"MSR",MSR().get_data().rnum(),mask.rnum());
      }
    }
    
    void MSR_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(42,"MSR",MSR().get_data().rnum(),mask.rnum());
      }
    }
    
    void MSR_write_internal(const bits<32> &x) {
      MSR().set_data() = (x & ~MSR()._reserved_mask);
      {
        _sym651();
      }
    };
    void MSR_write(const bits<32> &x) {
      MSR_write_internal(x);
      CondRegTrace(MSR_trace());
    };
    bits<32> & MSR_read_internal() {
      return MSR().get_data();
    };
    
    bits<32> & MSR_read()  ATTRIBUTE_USED {
      CondRegTrace(MSR_read_trace());
      return MSR_read_internal();
    }
    bits<32> & MSR_read_notrace() {
      return MSR_read_internal();
    }
    void MSR_reset_value() {
      bits<32> x("");
      MSR().set_value(x);
      MSR().reset_tag();
    };
    bits<1> MSR_EE_read()  ATTRIBUTE_USED {
      CondRegTrace(
      {
        bits<32> mask;
        mask.mask(16,16);
        MSR_read_trace(mask);
      }
      );
      return MSR_read_notrace(). get<1>(16,16);
    };
    
    
    // Register NIA
    void NIA_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(43,"NIA",NIA().get_data().rnum());
      }
      if (Tracing && (_trace_mode && ttBranchTaken)) {
        _logger->log_branch_taken(NIA().get_data().uint64());
      }
      branch_taken_handler();
    }
    
    void NIA_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(43,"NIA",NIA().get_data().rnum());
      }
    }
    
    void NIA_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(43,"NIA",NIA().get_data().rnum(),mask.rnum());
      }
      if (Tracing && (_trace_mode && ttBranchTaken)) {
        _logger->log_branch_taken(NIA().get_data().uint64());
      }
      branch_taken_handler();
    }
    
    void NIA_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(43,"NIA",NIA().get_data().rnum(),mask.rnum());
      }
    }
    
    void NIA_write_internal(const bits<32> &x) {
      NIA().set_data() = x;
    };
    void NIA_write(const bits<32> &x) {
      NIA_write_internal(x);
      CondRegBranchTrace(NIA_trace());
    };
    void NIA_trace_simple()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(43,"NIA",NIA().get_data().rnum());
      }
    }
    
    void NIA_read_trace_simple()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(43,"NIA",NIA().get_data().rnum());
      }
    }
    
    void NIA_trace_simple(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(43,"NIA",NIA().get_data().rnum(),mask.rnum());
      }
    }
    
    void NIA_read_trace_simple(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(43,"NIA",NIA().get_data().rnum(),mask.rnum());
      }
    }
    
    void NIA_write_internal_simple(const bits<32> &x) {
      NIA().set_data() = x;
    };
    void NIA_write_simple(const bits<32> &x)  ATTRIBUTE_INLINE {
      NIA_write_internal_simple(x);
      CondRegBranchTrace(NIA_trace_simple());
    };
    bits<32> & NIA_read_internal() {
      return NIA().get_data();
    };
    
    bits<32> & NIA_read()  ATTRIBUTE_USED {
      CondRegTrace(NIA_read_trace());
      return NIA_read_internal();
    }
    bits<32> & NIA_read_notrace() {
      return NIA_read_internal();
    }
    void NIA_reset_value() {
      bits<32> x("");
      NIA().set_value(x);
      NIA().reset_tag();
    };
    
    // Register PCCNT
    void PCCNT_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(44,"PCCNT",PCCNT().get_data().rnum());
      }
    }
    
    void PCCNT_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(44,"PCCNT",PCCNT().get_data().rnum());
      }
    }
    
    void PCCNT_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(44,"PCCNT",PCCNT().get_data().rnum(),mask.rnum());
      }
    }
    
    void PCCNT_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(44,"PCCNT",PCCNT().get_data().rnum(),mask.rnum());
      }
    }
    
    void PCCNT_write_internal(const bits<32> &x) {
      PCCNT().set_data() = x;
    };
    void PCCNT_write(const bits<32> &x) {
      PCCNT_write_internal(x);
      CondRegTrace(PCCNT_trace());
    };
    bits<32> & PCCNT_read_internal() {
      return PCCNT().get_data();
    };
    
    bits<32> & PCCNT_read()  ATTRIBUTE_USED {
      CondRegTrace(PCCNT_read_trace());
      return PCCNT_read_internal();
    }
    bits<32> & PCCNT_read_notrace() {
      return PCCNT_read_internal();
    }
    void PCCNT_reset_value() {
      bits<32> x("");
      PCCNT().set_value(x);
      PCCNT().reset_tag();
    };
    
    // Register PCEXCPT
    void PCEXCPT_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(45,"PCEXCPT",PCEXCPT().get_data().rnum());
      }
    }
    
    void PCEXCPT_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(45,"PCEXCPT",PCEXCPT().get_data().rnum());
      }
    }
    
    void PCEXCPT_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(45,"PCEXCPT",PCEXCPT().get_data().rnum(),mask.rnum());
      }
    }
    
    void PCEXCPT_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(45,"PCEXCPT",PCEXCPT().get_data().rnum(),mask.rnum());
      }
    }
    
    bits<32> & PCEXCPT_read_internal() {
      return PCEXCPT().get_data();
    };
    
    bits<32> & PCEXCPT_read()  ATTRIBUTE_USED {
      CondRegTrace(PCEXCPT_read_trace());
      return PCEXCPT_read_internal();
    }
    bits<32> & PCEXCPT_read_notrace() {
      return PCEXCPT_read_internal();
    }
    void PCEXCPT_reset_value() {
      bits<32> x("");
      PCEXCPT().set_value(x);
      PCEXCPT().reset_tag();
    };
    
    // Register POSTEXCPT
    void POSTEXCPT_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(46,"POSTEXCPT",POSTEXCPT().get_data().rnum());
      }
    }
    
    void POSTEXCPT_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(46,"POSTEXCPT",POSTEXCPT().get_data().rnum());
      }
    }
    
    void POSTEXCPT_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(46,"POSTEXCPT",POSTEXCPT().get_data().rnum(),mask.rnum());
      }
    }
    
    void POSTEXCPT_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(46,"POSTEXCPT",POSTEXCPT().get_data().rnum(),mask.rnum());
      }
    }
    
    void POSTEXCPT_write_internal(const bits<32> &x) {
      POSTEXCPT().set_data() = x;
    };
    void POSTEXCPT_write(const bits<32> &x) {
      POSTEXCPT_write_internal(x);
      CondRegTrace(POSTEXCPT_trace());
    };
    bits<32> & POSTEXCPT_read_internal() {
      return POSTEXCPT().get_data();
    };
    
    bits<32> & POSTEXCPT_read()  ATTRIBUTE_USED {
      CondRegTrace(POSTEXCPT_read_trace());
      return POSTEXCPT_read_internal();
    }
    bits<32> & POSTEXCPT_read_notrace() {
      return POSTEXCPT_read_internal();
    }
    void POSTEXCPT_reset_value() {
      bits<32> x("");
      POSTEXCPT().set_value(x);
      POSTEXCPT().reset_tag();
    };
    
    // Register PREEXCPT
    void PREEXCPT_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(47,"PREEXCPT",PREEXCPT().get_data().rnum());
      }
    }
    
    void PREEXCPT_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(47,"PREEXCPT",PREEXCPT().get_data().rnum());
      }
    }
    
    void PREEXCPT_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(47,"PREEXCPT",PREEXCPT().get_data().rnum(),mask.rnum());
      }
    }
    
    void PREEXCPT_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(47,"PREEXCPT",PREEXCPT().get_data().rnum(),mask.rnum());
      }
    }
    
    void PREEXCPT_write_internal(const bits<32> &x) {
      PREEXCPT().set_data() = x;
    };
    void PREEXCPT_write(const bits<32> &x) {
      PREEXCPT_write_internal(x);
      CondRegTrace(PREEXCPT_trace());
    };
    bits<32> & PREEXCPT_read_internal() {
      return PREEXCPT().get_data();
    };
    
    bits<32> & PREEXCPT_read()  ATTRIBUTE_USED {
      CondRegTrace(PREEXCPT_read_trace());
      return PREEXCPT_read_internal();
    }
    bits<32> & PREEXCPT_read_notrace() {
      return PREEXCPT_read_internal();
    }
    void PREEXCPT_reset_value() {
      bits<32> x("");
      PREEXCPT().set_value(x);
      PREEXCPT().reset_tag();
    };
    
    // Register SRR0
    void SRR0_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(48,"SRR0",SRR0().get_data().rnum());
      }
    }
    
    void SRR0_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(48,"SRR0",SRR0().get_data().rnum());
      }
    }
    
    void SRR0_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(48,"SRR0",SRR0().get_data().rnum(),mask.rnum());
      }
    }
    
    void SRR0_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(48,"SRR0",SRR0().get_data().rnum(),mask.rnum());
      }
    }
    
    void SRR0_write_internal(const bits<32> &x) {
      SRR0().set_data() = x;
    };
    void SRR0_write(const bits<32> &x) {
      SRR0_write_internal(x);
      CondRegTrace(SRR0_trace());
    };
    bits<32> & SRR0_read_internal() {
      return SRR0().get_data();
    };
    
    bits<32> & SRR0_read()  ATTRIBUTE_USED {
      CondRegTrace(SRR0_read_trace());
      return SRR0_read_internal();
    }
    bits<32> & SRR0_read_notrace() {
      return SRR0_read_internal();
    }
    void SRR0_reset_value() {
      bits<32> x("");
      SRR0().set_value(x);
      SRR0().reset_tag();
    };
    
    // Register SRR1
    void SRR1_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(49,"SRR1",SRR1().get_data().rnum());
      }
    }
    
    void SRR1_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(49,"SRR1",SRR1().get_data().rnum());
      }
    }
    
    void SRR1_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(49,"SRR1",SRR1().get_data().rnum(),mask.rnum());
      }
    }
    
    void SRR1_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(49,"SRR1",SRR1().get_data().rnum(),mask.rnum());
      }
    }
    
    void SRR1_write_internal(const bits<32> &x) {
      SRR1().set_data() = x;
    };
    void SRR1_write(const bits<32> &x) {
      SRR1_write_internal(x);
      CondRegTrace(SRR1_trace());
    };
    bits<32> & SRR1_read_internal() {
      return SRR1().get_data();
    };
    
    bits<32> & SRR1_read()  ATTRIBUTE_USED {
      CondRegTrace(SRR1_read_trace());
      return SRR1_read_internal();
    }
    bits<32> & SRR1_read_notrace() {
      return SRR1_read_internal();
    }
    void SRR1_reset_value() {
      bits<32> x("");
      SRR1().set_value(x);
      SRR1().reset_tag();
    };
    
    // Register Slice
    void Slice_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(50,"Slice",Slice().get_data().rnum());
      }
    }
    
    void Slice_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(50,"Slice",Slice().get_data().rnum());
      }
    }
    
    void Slice_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(50,"Slice",Slice().get_data().rnum(),mask.rnum());
      }
    }
    
    void Slice_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(50,"Slice",Slice().get_data().rnum(),mask.rnum());
      }
    }
    
    bits<32> & Slice_read_internal() {
      return Slice().get_data();
    };
    
    bits<32> & Slice_read()  ATTRIBUTE_USED {
      CondRegTrace(Slice_read_trace());
      return Slice_read_internal();
    }
    bits<32> & Slice_read_notrace() {
      return Slice_read_internal();
    }
    void Slice_reset_value() {
      bits<32> x("");
      Slice().set_value(x);
      Slice().reset_tag();
    };
    
    // Register XER
    void XER_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(51,"XER",XER().get_data().rnum());
      }
    }
    
    void XER_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(51,"XER",XER().get_data().rnum());
      }
    }
    
    void XER_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(51,"XER",XER().get_data().rnum(),mask.rnum());
      }
    }
    
    void XER_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(51,"XER",XER().get_data().rnum(),mask.rnum());
      }
    }
    
    void XER_write_internal(const bits<32> &x) {
      XER().set_data() = x;
    };
    
    void XER_write_delayed(const bits<32> &x,unsigned d) {
      decltype(&P::XER_commit_write) pCommit = &P::XER_commit_write;
      static PacketPool<CommitRegWrite<decltype(this),decltype(pCommit),32,1000>, 1000>   pool;
      CommitRegWrite<decltype(this),decltype(pCommit),32,1000> *cmt = pool.alloc();
      cmt->reset(this,pCommit,x,pool);
      _commitQueue.add(cmt,d);
    }
    
    void XER_write(const bits<32> &x) {
      XER_write_internal(x);
      CondRegTrace(XER_trace());
    };
    void XER_commit_write(const bits<32> &x) {
      XER_write_internal(x);
      CondRegTrace(XER_trace());
    };
    bits<32> & XER_read_internal() {
      return XER().get_data();
    };
    
    bits<32> & XER_read()  ATTRIBUTE_USED {
      CondRegTrace(XER_read_trace());
      return XER_read_internal();
    }
    bits<32> & XER_read_notrace() {
      return XER_read_internal();
    }
    void XER_reset_value() {
      bits<32> x("");
      XER().set_value(x);
      XER().reset_tag();
    };
    
    template <size_t Nb>
    void XER_slice_set_delayed(unsigned l,unsigned r,const bits<Nb> &x,unsigned d) {
      typedef void (P::*Functor)(unsigned,unsigned,const bits<Nb>&);
      Functor pCommit = &P::XER_slice_commit_write;
      static PacketPool<CommitRegSliceSet<decltype(this),decltype(pCommit),32,1000>, 1000>   pool;
      CommitRegSliceSet<decltype(this),decltype(pCommit),32,1000> *cmt = pool.alloc();
      cmt->reset(this,pCommit,l,r,x,pool);
      _commitQueue.add(cmt,d);
    }
    
    void XER_slice_set_delayed(unsigned l,unsigned r,uint64_t x,unsigned d)  ATTRIBUTE_USED {
      XER_slice_set_delayed(l,r,bits<32>(x),d);
    }
    template <size_t Nb>
    void XER_slice_set_internal(unsigned l,unsigned r,const bits<Nb> &x) {
      XER_slice_commit_write(l,r,x);
    }
    
    
    template <size_t Nb>
    void XER_slice_commit_write(unsigned l,unsigned r,const bits<Nb> &x) {
      XER_read_notrace().set(l,r,x);
      CondRegTrace(
      {
        bits<32> mask;
        mask.mask(l,r);
        XER_trace(mask);
      }
      );
    };
    template <size_t Nb>
    void XER_slice_set(unsigned l,unsigned r,const bits<Nb> &x) {
      XER_slice_set_internal(l,r,x);
      CondRegTrace(
      {
        bits<32> mask;
        mask.mask(l,r);
        XER_trace(mask);
      }
      );
    }
    
    void XER_slice_set(unsigned l,unsigned r,uint64_t x) {
      XER_slice_set(l,r,bits<32>(x));
    };
    
    // Register tmpESR
    void tmpESR_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write(52,"tmpESR",tmpESR().get_data().rnum());
      }
    }
    
    void tmpESR_read_trace()  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read(52,"tmpESR",tmpESR().get_data().rnum());
      }
    }
    
    void tmpESR_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegWrite)) {
        _logger->log_reg_write_mask(52,"tmpESR",tmpESR().get_data().rnum(),mask.rnum());
      }
    }
    
    void tmpESR_read_trace(const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegRead)) {
        _logger->log_reg_read_mask(52,"tmpESR",tmpESR().get_data().rnum(),mask.rnum());
      }
    }
    
    void tmpESR_write_internal(const bits<32> &x) {
      tmpESR().set_data() = x;
    };
    void tmpESR_write(const bits<32> &x) {
      tmpESR_write_internal(x);
      CondRegTrace(tmpESR_trace());
    };
    bits<32> & tmpESR_read_internal() {
      return tmpESR().get_data();
    };
    
    bits<32> & tmpESR_read()  ATTRIBUTE_USED {
      CondRegTrace(tmpESR_read_trace());
      return tmpESR_read_internal();
    }
    bits<32> & tmpESR_read_notrace() {
      return tmpESR_read_internal();
    }
    void tmpESR_reset_value() {
      bits<32> x("");
      tmpESR().set_value(x);
      tmpESR().reset_tag();
    };
    template <size_t Nb>
    void tmpESR_slice_set_internal(unsigned l,unsigned r,const bits<Nb> &x) {
      tmpESR_read_notrace().set(l,r,x);
    };
    template <size_t Nb>
    void tmpESR_slice_set(unsigned l,unsigned r,const bits<Nb> &x) {
      tmpESR_slice_set_internal(l,r,x);
      CondRegTrace(
      {
        bits<32> mask;
        mask.mask(l,r);
        tmpESR_trace(mask);
      }
      );
    }
    
    void tmpESR_slice_set(unsigned l,unsigned r,uint64_t x) {
      tmpESR_slice_set(l,r,bits<32>(x));
    };
    
    
    //
    // Register File Handling Code.
    //
    
    // Register File ARF
    void ARF_trace(unsigned index)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegfileWrite)) {
        _logger->log_regfile_write(53,"A",index,ARF().operator()(index).get_data().rnum());
      }
    }
    
    void ARF_trace(unsigned ctx_index,unsigned index) {
      if (Tracing && (_trace_mode & ttRegfileWrite)) {
        _logger->log_regfile_write(53,"A",index,ARF(ctx_index).operator()(index).get_data().rnum());
      }
    }
    
    void ARF_read_trace(unsigned index) {
      if (Tracing && (_trace_mode & ttRegfileRead)) {
        _logger->log_regfile_read(53,"A",index,ARF().operator()(index).get_data().rnum());
      }
    };
    void ARF_trace(unsigned index,const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegfileWrite)) {
        _logger->log_regfile_write_mask(53,"A",index,ARF().operator()(index).get_data().rnum(),mask.rnum());
      }
    }
    
    void ARF_read_trace(unsigned index,const bits<32> &mask) {
      if (Tracing && (_trace_mode & ttRegfileRead)) {
        _logger->log_regfile_read_mask(53,"A",index,ARF().operator()(index).get_data().rnum(),mask.rnum());
      }
    };
    void ARF_write_hook(uint32_t _sym659, unsigned index , bits < 32 > x)  ATTRIBUTE_USED {
      {
             AR_W_write(  AR_W_read() + 1  )  ;
            ARF_write_inhook(  _sym659 ,  index , ( x ) ) ;
      }
    }
    void ARF_write_internal(uint32_t ctx_index ,unsigned index,const bits<32> &x)  ATTRIBUTE_USED {
      ARF_write_hook(ctx_index ,index,x);
    };
    void ARF_write_inhook(uint32_t ctx_index ,unsigned index,const bits<32> &x)  ATTRIBUTE_USED {
      ARF(ctx_index)(index).set_data() = x;
    }
    void ARF_ctx_write(uint32_t ctx_index ,unsigned index,const bits<32> &x) {
      ARF_write_internal(ctx_index,index,x);
      {
        bool change = foobar_check_change(ctx_index);
        CondRegTrace(ARF_trace(ctx_index ,index));
        foobar_change_back(change);
      }
    }
    void ARF_write_internal_traced(uint32_t ctx_index ,unsigned index,const bits<32> &x)  ATTRIBUTE_USED {
      ARF_write_internal(ctx_index ,index,x);
      CondRegTrace(ARF_trace(index));
    }
    void ARF_write(unsigned index,const bits<32> &x) {
      ARF_write_internal_traced(foobar._foobar_active,index,x);
    };
    bits<32> ARF_read_internal(uint32_t _sym659, unsigned index)  ATTRIBUTE_USED {
      {
             AR_R_write(  AR_R_read() + 1  )  ;
            return ARF_read_inhook( _sym659 ,  to_uint32(index)) ;
      }
    };
    bits<32> &ARF_read_inhook(uint32_t ctx_index ,unsigned index)  ATTRIBUTE_USED {
      return ARF(ctx_index)(index).get_data();
    }
    
    bits<32> ARF_read(unsigned index)  ATTRIBUTE_USED {
      CondRegTrace(ARF_read_trace(index));
      return ARF_read_internal(foobar._foobar_active,index);
    }
    
    bits<32> ARF_read_notrace(unsigned index)  ATTRIBUTE_USED {
      return ARF_read_internal(foobar._foobar_active,index);
    }
    bits<32> ARF_ctx_read(unsigned ctx_index,unsigned index) {
      return ARF_read_internal(ctx_index,index);
    }
    bits<32> ARF_read_internal(unsigned index)  ATTRIBUTE_USED { return ARF_read_internal(foobar._foobar_active,index); }
    
    void ARF_A_write(unsigned index, const bits<16> &x)  ATTRIBUTE_USED {
      bits<32>tmp = ARF_read(index);
      tmp.set(0,15,x);
      ARF_write(index, tmp);
    };
    void ARF_A_ctx_write(uint32_t ctx_index ,unsigned index,const bits<16> &x) {
      bits<32> tmp = ARF_read_internal(ctx_index ,index);
      tmp.set(0,15,x);
      ARF_write_internal(ctx_index ,index,tmp);
      {
        bool change = foobar_check_change(ctx_index);
        CondRegTrace(ARF_trace(ctx_index ,index));
        foobar_change_back(change);
      }
    }
    void ARF_A_write_inhook(uint32_t ctx_index ,unsigned index,const bits<16> &x)  ATTRIBUTE_USED  ATTRIBUTE_USED {
      ARF(ctx_index)(index).set_data().set(0,15,x);
      CondRegTrace(
      {
        bits<32> mask;
        mask.mask(0,15);
        ARF_trace(index,mask);
      }
      );
    };
    bits<16> ARF_B_read(unsigned index)  ATTRIBUTE_USED {
      CondRegTrace(
      {
        bits<32> mask;
        mask.mask(16,31);
        ARF_read_trace(index,mask);
      }
      );
      return ARF_read_notrace(index). get<16>(16,31);
    }
    
    bits<16> ARF_B_ctx_read(unsigned ctx_index,unsigned index) {
      return ARF_read_internal(ctx_index,index). get<16>(16,31);
    };
    void ARF_B_write_inhook(uint32_t ctx_index ,unsigned index,const bits<16> &x)  ATTRIBUTE_USED  ATTRIBUTE_USED {
      ARF(ctx_index)(index).set_data().set(16,31,x);
      CondRegTrace(
      {
        bits<32> mask;
        mask.mask(16,31);
        ARF_trace(index,mask);
      }
      );
    };
    bits<8> ARF_C_read(unsigned index,unsigned i)  ATTRIBUTE_USED {
      CondRegTrace(
      {
        bits<32> mask;
        mask.mask(((i%32)*8),(((i%32)*8)+8-1));
        ARF_read_trace(index,mask);
      }
      );
      return ARF_read_notrace(index).get<8>(((i%32)*8),(((i%32)*8)+8-1));
    };
    
    void ARF_C_write(unsigned index, unsigned i,const bits<8> &x)  ATTRIBUTE_USED {
      bits<32>tmp = ARF_read(index);
      tmp.set<8>(((i%32)*8),(((i%32)*8)+8-1),x);
      ARF_write(index, tmp);
    };
    bits<8> ARF_C_ctx_read(uint32_t ctx_index ,unsigned index,unsigned i) {
      return ARF_read_internal(ctx_index,index).get<8>(((i%32)*8),(((i%32)*8)+8-1));
    };
    void ARF_C_ctx_write(uint32_t ctx_index ,unsigned index,unsigned i,const bits<8> &x) {
      bits<32> tmp = ARF_read_internal(ctx_index,index);
      tmp.set<8>(i*8,(i*8)+8-1,x);
      ARF_write_internal(ctx_index,index,tmp);
      {
        bool change = foobar_check_change(ctx_index);
        CondRegTrace(ARF_trace(ctx_index ,index));
        foobar_change_back(change);
      }
    }
    void ARF_C_write_inhook(uint32_t ctx_index ,unsigned index,unsigned i,const bits<8> &x)  ATTRIBUTE_USED {
      ARF(ctx_index)(index).set_data().set<8>(((i%32)*8),(((i%32)*8)+8-1),x);
      CondRegTrace(
      {
        bits<32> mask;
        mask.mask(((i%32)*8),(((i%32)*8)+8-1));
        ARF_trace(index,mask);
      }
      );
    };
    template <size_t Nb>
    void ARF_slice_set_internal(unsigned index, unsigned l,unsigned r,const bits<Nb> &x) {
      bits<32> tmp = ARF_read(index);
      tmp.set(l,r,x);
      ARF_write(index, tmp);
    };
    template <size_t Nb>
    void ARF_slice_set(unsigned index, unsigned l,unsigned r,const bits<Nb> &x) {
      ARF_slice_set_internal(index, l,r,x);
    }
    
    void ARF_slice_set(unsigned index, unsigned l,unsigned r,uint64_t x) {
      ARF_slice_set(index, l,r,bits<32>(x));
    };
    template <size_t Nb>
    void ARF_slice_set_inhook(unsigned index, unsigned l,unsigned r,const bits<Nb> &x) {
      ARF()(index).set_data().set(l,r,x);
      CondRegTrace(
      {
        bits<32> mask;
        mask.mask(l,r);
        ARF_trace(index,mask);
      }
      );
    }
    void ARF_slice_set_inhook(unsigned index, unsigned l,unsigned r,uint64_t x) {
      ARF_slice_set_inhook(index, l,r,bits<32>(x));
    };
    template <size_t Nb>
    void ARF_ctx_slice_set(uint32_t ctx_index ,unsigned index,unsigned x,unsigned y,const bits<Nb> &v) {
      bits<32> tmp = ARF_read_internal(ctx_index ,index);
      tmp.set(x,y,v);
      ARF_write_internal(ctx_index ,index,tmp);
      {
        bool change = foobar_check_change(ctx_index);
        CondRegTrace(ARF_trace(ctx_index ,index));
        foobar_change_back(change);
      }
    }
    void ARF_ctx_slice_set(uint32_t ctx_index ,unsigned index,unsigned x,unsigned y,uint64_t v) {
      ARF_ctx_slice_set(ctx_index ,index,x,y,bits<32>(v));
    }
    void ARF_reset_value(uint32_t ctx_index) {
      ARF().reset(bits<32>(""));
      ARF(ctx_index).reset_tags();
    };
    
    // Register File ARF16
    void ARF16_write_internal(uint32_t ctx_index ,unsigned index,const bits<32> &x)  ATTRIBUTE_USED {
      ARF_write_internal_traced(ctx_index ,ARF16_t::getWriteIndex(index),x);
    };
    void ARF16_write_internal_traced(uint32_t ctx_index ,unsigned index,const bits<32> &x)  ATTRIBUTE_USED {
      ARF16_write_internal(ctx_index ,index,x);
    }
    void ARF16_write(unsigned index,const bits<32> &x) {
      ARF16_write_internal_traced(foobar._foobar_active,index,x);
    };
    bits<32> ARF16_read_internal(uint32_t ctx_index ,unsigned index)  ATTRIBUTE_USED {
      return ARF(ctx_index)(ARF16_t::getReadIndex(index)).get_data();
    };
    bits<32> ARF16_read(unsigned index)  ATTRIBUTE_USED {
      return ARF16_read_internal(foobar._foobar_active,index);
    }
    
    bits<32> ARF16_read_notrace(unsigned index)  ATTRIBUTE_USED {
      return ARF16_read_internal(foobar._foobar_active,index);
    }
    
    // Register File CRF
    void CRF_trace(unsigned index)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegfileWrite)) {
        _logger->log_regfile_write(55,"C",index,CRF().operator()(index).get_data().rnum());
      }
    }
    
    void CRF_trace(unsigned ctx_index,unsigned index) {
      if (Tracing && (_trace_mode & ttRegfileWrite)) {
        _logger->log_regfile_write(55,"C",index,CRF(ctx_index).operator()(index).get_data().rnum());
      }
    }
    
    void CRF_read_trace(unsigned index) {
      if (Tracing && (_trace_mode & ttRegfileRead)) {
        _logger->log_regfile_read(55,"C",index,CRF().operator()(index).get_data().rnum());
      }
    };
    void CRF_trace(unsigned index,const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegfileWrite)) {
        _logger->log_regfile_write_mask(55,"C",index,CRF().operator()(index).get_data().rnum(),mask.rnum());
      }
    }
    
    void CRF_read_trace(unsigned index,const bits<32> &mask) {
      if (Tracing && (_trace_mode & ttRegfileRead)) {
        _logger->log_regfile_read_mask(55,"C",index,CRF().operator()(index).get_data().rnum(),mask.rnum());
      }
    };
    void CRF_write_internal(uint32_t ctx_index ,unsigned index,const bits<32> &x)  ATTRIBUTE_USED {
      CRF(ctx_index)(index).set_data() = x;
    };
    void CRF_write_inhook(uint32_t ctx_index ,unsigned index,const bits<32> &x)  ATTRIBUTE_USED {
      CRF(ctx_index)(index).set_data() = x;
    }
    void CRF_ctx_write(uint32_t ctx_index ,unsigned index,const bits<32> &x) {
      CRF_write_internal(ctx_index,index,x);
      {
        bool change = foobar_check_change(ctx_index);
        CondRegTrace(CRF_trace(ctx_index ,index));
        foobar_change_back(change);
      }
    }
    void CRF_write_internal_traced(uint32_t ctx_index ,unsigned index,const bits<32> &x)  ATTRIBUTE_USED {
      CRF_write_internal(ctx_index ,index,x);
      CondRegTrace(CRF_trace(index));
    }
    void CRF_write(unsigned index,const bits<32> &x) {
      CRF_write_internal_traced(foobar._foobar_active,index,x);
    };
    bits<32> &CRF_read_internal(uint32_t ctx_index ,unsigned index)  ATTRIBUTE_USED {
      return CRF(ctx_index)(index).get_data();
    };
    bits<32> & CRF_read(unsigned index)  ATTRIBUTE_USED {
      CondRegTrace(CRF_read_trace(index));
      return CRF_read_internal(foobar._foobar_active,index);
    }
    
    bits<32> & CRF_read_notrace(unsigned index)  ATTRIBUTE_USED {
      return CRF_read_internal(foobar._foobar_active,index);
    }
    bits<32> & CRF_ctx_read(unsigned ctx_index,unsigned index) {
      return CRF_read_internal(ctx_index,index);
    }
    bits<32> & CRF_read_internal(unsigned index)  ATTRIBUTE_USED { return CRF_read_internal(foobar._foobar_active,index); }
    
    void CRF_reset_value(uint32_t ctx_index) {
      CRF().reset(bits<32>(""));
      CRF(ctx_index).reset_tags();
    };
    
    // Register File GPR
    void GPR_trace(unsigned index)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegfileWrite)) {
        _logger->log_regfile_write(56,"GPR",index,GPR().operator()(index).get_data().rnum());
      }
    }
    
    void GPR_read_trace(unsigned index) {
      if (Tracing && (_trace_mode & ttRegfileRead)) {
        _logger->log_regfile_read(56,"GPR",index,GPR().operator()(index).get_data().rnum());
      }
    };
    void GPR_trace(unsigned index,const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegfileWrite)) {
        _logger->log_regfile_write_mask(56,"GPR",index,GPR().operator()(index).get_data().rnum(),mask.rnum());
      }
    }
    
    void GPR_read_trace(unsigned index,const bits<32> &mask) {
      if (Tracing && (_trace_mode & ttRegfileRead)) {
        _logger->log_regfile_read_mask(56,"GPR",index,GPR().operator()(index).get_data().rnum(),mask.rnum());
      }
    };
    void GPR_write_internal(unsigned index,const bits<32> &x)  ATTRIBUTE_USED {
      GPR()(index).set_data() = x;
    };
    void GPR_write_internal_traced(unsigned index,const bits<32> &x)  ATTRIBUTE_USED {
      GPR_write_internal(index,x);
      CondRegTrace(GPR_trace(index));
    }
    
    void GPR_write_delayed(unsigned index,const bits<32> &x,unsigned d)  ATTRIBUTE_USED {
      decltype(&P::GPR_commit_write) pCommit = &P::GPR_commit_write;
      static PacketPool<CommitRegfileWrite<decltype(this),decltype(pCommit),32,1000>, 1000>   pool;
      CommitRegfileWrite<decltype(this),decltype(pCommit),32,1000> *cmt = pool.alloc();
      cmt->reset(this,pCommit,index,x,pool);
      _commitQueue.add(cmt,d);
    }
    
    void GPR_write(unsigned index,const bits<32> &x) {
      GPR_write_internal_traced(index,x);
    };
    void GPR_commit_write(unsigned index,const bits<32> &x)  ATTRIBUTE_USED {
      GPR_write_internal_traced(index,x);
    };
    bits<32> &GPR_read_internal(unsigned index)  ATTRIBUTE_USED {
      return GPR()(index).get_data();
    };
    bits<32> & GPR_read(unsigned index)  ATTRIBUTE_USED {
      CondRegTrace(GPR_read_trace(index));
      return GPR_read_internal(index);
    }
    
    bits<32> & GPR_read_notrace(unsigned index)  ATTRIBUTE_USED {
      return GPR_read_internal(index);
    }
    
    template <size_t Nb>
    void GPR_slice_set_delayed(unsigned index, unsigned l,unsigned r,const bits<Nb> &x,unsigned d) {
      typedef void (P::*Functor)(unsigned index, unsigned,unsigned,const bits<Nb>&);
      Functor pCommit = &P::GPR_slice_commit_write;
      static PacketPool<CommitRegfileSliceSet<decltype(this),decltype(pCommit),32,1000>, 1000>   pool;
      CommitRegfileSliceSet<decltype(this),decltype(pCommit),32,1000> *cmt = pool.alloc();
      cmt->reset(this,pCommit,index, l,r,x,pool);
      _commitQueue.add(cmt,d);
    }
    
    void GPR_slice_set_delayed(unsigned index, unsigned l,unsigned r,uint64_t x,unsigned d)  ATTRIBUTE_USED {
      GPR_slice_set_delayed(index, l,r,bits<32>(x),d);
    }
    template <size_t Nb>
    void GPR_slice_set_internal(unsigned index, unsigned l,unsigned r,const bits<Nb> &x) {
      GPR_slice_commit_write(index, l,r,x);
    }
    
    
    template <size_t Nb>
    void GPR_slice_commit_write(unsigned index, unsigned l,unsigned r,const bits<Nb> &x) {
      GPR_read_notrace(index).set(l,r,x);
      CondRegTrace(
      {
        bits<32> mask;
        mask.mask(l,r);
        GPR_trace(index,mask);
      }
      );
    };
    template <size_t Nb>
    void GPR_slice_set(unsigned index, unsigned l,unsigned r,const bits<Nb> &x) {
      GPR_slice_set_internal(index, l,r,x);
      CondRegTrace(
      {
        bits<32> mask;
        mask.mask(l,r);
        GPR_trace(index,mask);
      }
      );
    }
    
    void GPR_slice_set(unsigned index, unsigned l,unsigned r,uint64_t x) {
      GPR_slice_set(index, l,r,bits<32>(x));
    };
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
        case 9:
        CTR_write(x);
        break;
        case 22:
        DEC_write(x);
        break;
        case 26:
        SRR0_write(x);
        break;
        case 27:
        SRR1_write(x);
        break;
        case 30:
        FOO1_write(x);
        break;
        case 31:
        FOO2_write(x);
        break;
        case 58:
        CSRR0_write(x);
        break;
        case 59:
        CSRR1_write(x);
        break;
        case 62:
        ESR_write(x);
        break;
        case 100:
        GPR_write(20, x);
        break;
        case 101:
        GPR_write(21, x);
        break;
        case 200:
        FOO4_write(x);
        break;
        case 201:
        FOO3_write(x);
        break;
        case 202:
        FOO5_write(x);
        break;
        case 208:
        FOO8_write(x);
        break;
        case 209:
        FOO9_write(x);
        break;
        case 210:
        FOO10_write(x);
        break;
        case 211:
        FOO11_write(x);
        break;
        case 230:
        AR_ctx_write(0,x);
        break;
        case 231:
        AR_ctx_write(1,x);
        break;
        case 232:
        AR_ctx_write(2,x);
        break;
        case 233:
        ARF_ctx_write(0,0, x);
        break;
        case 234:
        ARF_ctx_write(1,1, x);
        break;
        case 235:
        ARF_ctx_write(2,2, x);
        break;
        case 250:
        SPR_write_250(x);
        break;
        case 251:
        BranchTaken_write(x);
        break;
        case 301:
        SPR_write_301(x);
        break;
        case 302:
        SPR_write_302(x);
        break;
        case 303:
        SPR_write_303(x);
        break;
        case 304:
        // Write is ignored.
        break;
        case 400:
        EWTEST_write(x);
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
        case 9:
        return CTR_read();
        case 22:
        return DEC_read();
        case 26:
        return SRR0_read();
        case 27:
        return SRR1_read();
        case 30:
        return FOO1_read();
        case 31:
        return FOO2_read();
        case 58:
        return CSRR0_read();
        case 59:
        return CSRR1_read();
        case 62:
        return ESR_read();
        case 100:
        return GPR_read(20);
        case 101:
        return GPR_read(21);
        case 200:
        return FOO4_read();
        case 201:
        return FOO3_read();
        case 202:
        return FOO5_read();
        case 208:
        return FOO8_read();
        case 209:
        return FOO9_read();
        case 210:
        return FOO10_read();
        case 211:
        return FOO11_read();
        case 230:
        return AR_ctx_read(0);
        case 231:
        return AR_ctx_read(1);
        case 232:
        return AR_ctx_read(2);
        case 233:
        return ARF_ctx_read(0, 0);
        case 234:
        return ARF_ctx_read(1, 1);
        case 235:
        return ARF_ctx_read(2, 2);
        case 250:
        return SPR_read_250();
        case 251:
        return BranchTaken_read();
        case 301:
        return SPR_read_301();
        case 302:
        return SPR_read_302();
        case 303:
        return SPR_read_303();
        case 304:
        return ITEST_read();
        case 400:
        return EWTEST_read();
        default:
        return 0;
      }
    };
    bits<32> SPR_read(unsigned index)  ATTRIBUTE_USED {
      return SPR_read_internal(index);
    }
    
    bits<32> SPR_read_notrace(unsigned index)  ATTRIBUTE_USED {
      return SPR_read_internal(index);
    }
    bits<32> SPR_read_250()
    {
           Program_t prog ;
          prog . ESR . set ( 5 , 5 , 1 ) ;
          raiseException ( prog ) ;
          return 0 ;
    }
    
    void SPR_write_250(bits < 32 > value)
    {
           Program_t prog ;
          prog . ESR . set ( 5 , 5 , 1 ) ;
          raiseException ( prog ) ;
    }
    
    bits<32> SPR_read_301()
    {
           FOO2_write(  FOO2_read() - 1  )  ;
          return FOO3_read() ;
    }
    
    void SPR_write_301(bits < 32 > x)
    {
           FOO2_write(  FOO2_read() + 1  )  ;
           FOO3_write( x )  ;
    }
    
    bits<32> SPR_read_302()
    {
          return DEC_read() ;
    }
    
    void SPR_write_302(bits < 32 > x)
    {
           DEC_write( x )  ;
    }
    
    bits<32> SPR_read_303()
    {
          return VPR_read( to_uint32(0) ) .get<32>(0,31) + VPR_read( to_uint32(0) ) .get<32>(96,127) ;
    }
    
    void SPR_write_303(bits < 32 > x)
    {
          VPR_slice_set( to_uint32(0), to_uint32(0),to_uint32(31) , ( x )  ); ;
          VPR_slice_set( to_uint32(0), to_uint32(96),to_uint32(127) , ( x )  ); ;
    }
    
    
    // Register File VPR
    void VPR_trace(unsigned index)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegfileWrite)) {
        _logger->log_regfile_write(58,"VPR",index,VPR().operator()(index).get_data().rnum());
      }
    }
    
    void VPR_read_trace(unsigned index) {
      if (Tracing && (_trace_mode & ttRegfileRead)) {
        _logger->log_regfile_read(58,"VPR",index,VPR().operator()(index).get_data().rnum());
      }
    };
    void VPR_trace(unsigned index,const bits<128> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegfileWrite)) {
        _logger->log_regfile_write_mask(58,"VPR",index,VPR().operator()(index).get_data().rnum(),mask.rnum());
      }
    }
    
    void VPR_read_trace(unsigned index,const bits<128> &mask) {
      if (Tracing && (_trace_mode & ttRegfileRead)) {
        _logger->log_regfile_read_mask(58,"VPR",index,VPR().operator()(index).get_data().rnum(),mask.rnum());
      }
    };
    void VPR_write_internal(unsigned index,const bits<128> &x)  ATTRIBUTE_USED {
      VPR()(index).set_data() = x;
    };
    void VPR_write_internal_traced(unsigned index,const bits<128> &x)  ATTRIBUTE_USED {
      VPR_write_internal(index,x);
      CondRegTrace(VPR_trace(index));
    }
    
    void VPR_write_delayed(unsigned index,const bits<128> &x,unsigned d)  ATTRIBUTE_USED {
      decltype(&P::VPR_commit_write) pCommit = &P::VPR_commit_write;
      static PacketPool<CommitRegfileWrite<decltype(this),decltype(pCommit),128,1000>, 1000>   pool;
      CommitRegfileWrite<decltype(this),decltype(pCommit),128,1000> *cmt = pool.alloc();
      cmt->reset(this,pCommit,index,x,pool);
      _commitQueue.add(cmt,d);
    }
    
    void VPR_write(unsigned index,const bits<128> &x) {
      VPR_write_internal_traced(index,x);
    };
    void VPR_commit_write(unsigned index,const bits<128> &x)  ATTRIBUTE_USED {
      VPR_write_internal_traced(index,x);
    };
    bits<128> &VPR_read_internal(unsigned index)  ATTRIBUTE_USED {
      return VPR()(index).get_data();
    };
    bits<128> & VPR_read(unsigned index)  ATTRIBUTE_USED {
      CondRegTrace(VPR_read_trace(index));
      return VPR_read_internal(index);
    }
    
    bits<128> & VPR_read_notrace(unsigned index)  ATTRIBUTE_USED {
      return VPR_read_internal(index);
    }
    bits<8> VPR_B_read(unsigned index,unsigned i)  ATTRIBUTE_USED {
      CondRegTrace(
      {
        bits<128> mask;
        mask.mask(((i%128)*8),(((i%128)*8)+8-1));
        VPR_read_trace(index,mask);
      }
      );
      return VPR_read_notrace(index).get<8>(((i%128)*8),(((i%128)*8)+8-1));
    };
    
    
    void VPR_B_write_delayed(unsigned index, unsigned i,const bits<8> &x,unsigned d)  ATTRIBUTE_USED {
      decltype(&P::VPR_B_commit_write) pCommit = &P::VPR_B_commit_write;
      static PacketPool<CommitRegfileIndexedFieldWrite<decltype(this),decltype(pCommit),128,1000>, 1000>   pool;
      CommitRegfileIndexedFieldWrite<decltype(this),decltype(pCommit),128,1000> *cmt = pool.alloc();
      cmt->reset(this,pCommit,index, i,x,pool);
      _commitQueue.add(cmt,d);
    }
    
    void VPR_B_write(unsigned index, unsigned i,const bits<8> &x)  ATTRIBUTE_USED {
      VPR_B_commit_write(index, i,x);
    }
    
    void VPR_B_commit_write(unsigned index, unsigned i,const bits<8> &x)  ATTRIBUTE_USED {
      VPR_read_notrace(index).set<8>(((i%128)*8),(((i%128)*8)+8-1),x);
      CondRegTrace(
      {
        bits<128> mask;
        mask.mask(((i%128)*8),(((i%128)*8)+8-1));
        VPR_trace(index,mask);
      }
      );
    };
    bits<16> VPR_H_read(unsigned index,unsigned i)  ATTRIBUTE_USED {
      CondRegTrace(
      {
        bits<128> mask;
        mask.mask(((i%128)*16),(((i%128)*16)+16-1));
        VPR_read_trace(index,mask);
      }
      );
      return VPR_read_notrace(index).get<16>(((i%128)*16),(((i%128)*16)+16-1));
    };
    
    
    void VPR_H_write_delayed(unsigned index, unsigned i,const bits<16> &x,unsigned d)  ATTRIBUTE_USED {
      decltype(&P::VPR_H_commit_write) pCommit = &P::VPR_H_commit_write;
      static PacketPool<CommitRegfileIndexedFieldWrite<decltype(this),decltype(pCommit),128,1000>, 1000>   pool;
      CommitRegfileIndexedFieldWrite<decltype(this),decltype(pCommit),128,1000> *cmt = pool.alloc();
      cmt->reset(this,pCommit,index, i,x,pool);
      _commitQueue.add(cmt,d);
    }
    
    void VPR_H_write(unsigned index, unsigned i,const bits<16> &x)  ATTRIBUTE_USED {
      VPR_H_commit_write(index, i,x);
    }
    
    void VPR_H_commit_write(unsigned index, unsigned i,const bits<16> &x)  ATTRIBUTE_USED {
      VPR_read_notrace(index).set<16>(((i%128)*16),(((i%128)*16)+16-1),x);
      CondRegTrace(
      {
        bits<128> mask;
        mask.mask(((i%128)*16),(((i%128)*16)+16-1));
        VPR_trace(index,mask);
      }
      );
    };
    bits<32> VPR_W_read(unsigned index,unsigned i)  ATTRIBUTE_USED {
      CondRegTrace(
      {
        bits<128> mask;
        mask.mask(((i%128)*32),(((i%128)*32)+32-1));
        VPR_read_trace(index,mask);
      }
      );
      return VPR_read_notrace(index).get<32>(((i%128)*32),(((i%128)*32)+32-1));
    };
    
    
    void VPR_W_write_delayed(unsigned index, unsigned i,const bits<32> &x,unsigned d)  ATTRIBUTE_USED {
      decltype(&P::VPR_W_commit_write) pCommit = &P::VPR_W_commit_write;
      static PacketPool<CommitRegfileIndexedFieldWrite<decltype(this),decltype(pCommit),128,1000>, 1000>   pool;
      CommitRegfileIndexedFieldWrite<decltype(this),decltype(pCommit),128,1000> *cmt = pool.alloc();
      cmt->reset(this,pCommit,index, i,x,pool);
      _commitQueue.add(cmt,d);
    }
    
    void VPR_W_write(unsigned index, unsigned i,const bits<32> &x)  ATTRIBUTE_USED {
      VPR_W_commit_write(index, i,x);
    }
    
    void VPR_W_commit_write(unsigned index, unsigned i,const bits<32> &x)  ATTRIBUTE_USED {
      VPR_read_notrace(index).set<32>(((i%128)*32),(((i%128)*32)+32-1),x);
      CondRegTrace(
      {
        bits<128> mask;
        mask.mask(((i%128)*32),(((i%128)*32)+32-1));
        VPR_trace(index,mask);
      }
      );
    };
    
    template <size_t Nb>
    void VPR_slice_set_delayed(unsigned index, unsigned l,unsigned r,const bits<Nb> &x,unsigned d) {
      typedef void (P::*Functor)(unsigned index, unsigned,unsigned,const bits<Nb>&);
      Functor pCommit = &P::VPR_slice_commit_write;
      static PacketPool<CommitRegfileSliceSet<decltype(this),decltype(pCommit),128,1000>, 1000>   pool;
      CommitRegfileSliceSet<decltype(this),decltype(pCommit),128,1000> *cmt = pool.alloc();
      cmt->reset(this,pCommit,index, l,r,x,pool);
      _commitQueue.add(cmt,d);
    }
    
    void VPR_slice_set_delayed(unsigned index, unsigned l,unsigned r,uint64_t x,unsigned d)  ATTRIBUTE_USED {
      VPR_slice_set_delayed(index, l,r,bits<128>(x),d);
    }
    template <size_t Nb>
    void VPR_slice_set_internal(unsigned index, unsigned l,unsigned r,const bits<Nb> &x) {
      VPR_slice_commit_write(index, l,r,x);
    }
    
    
    template <size_t Nb>
    void VPR_slice_commit_write(unsigned index, unsigned l,unsigned r,const bits<Nb> &x) {
      VPR_read_notrace(index).set(l,r,x);
      CondRegTrace(
      {
        bits<128> mask;
        mask.mask(l,r);
        VPR_trace(index,mask);
      }
      );
    };
    template <size_t Nb>
    void VPR_slice_set(unsigned index, unsigned l,unsigned r,const bits<Nb> &x) {
      VPR_slice_set_internal(index, l,r,x);
      CondRegTrace(
      {
        bits<128> mask;
        mask.mask(l,r);
        VPR_trace(index,mask);
      }
      );
    }
    
    void VPR_slice_set(unsigned index, unsigned l,unsigned r,uint64_t x) {
      VPR_slice_set(index, l,r,bits<128>(x));
    };
    void VPR_reset_value() {
      VPR().reset(bits<128>(""));
      VPR().reset_tags();
    };
    
    // Register File XPR
    void XPR_trace(unsigned index)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegfileWrite)) {
        _logger->log_regfile_write(59,"XPR",index,XPR().operator()(index).get_data().rnum());
      }
    }
    
    void XPR_read_trace(unsigned index) {
      if (Tracing && (_trace_mode & ttRegfileRead)) {
        _logger->log_regfile_read(59,"XPR",index,XPR().operator()(index).get_data().rnum());
      }
    };
    void XPR_trace(unsigned index,const bits<32> &mask)  ATTRIBUTE_USED {
      if (Tracing && (_trace_mode & ttRegfileWrite)) {
        _logger->log_regfile_write_mask(59,"XPR",index,XPR().operator()(index).get_data().rnum(),mask.rnum());
      }
    }
    
    void XPR_read_trace(unsigned index,const bits<32> &mask) {
      if (Tracing && (_trace_mode & ttRegfileRead)) {
        _logger->log_regfile_read_mask(59,"XPR",index,XPR().operator()(index).get_data().rnum(),mask.rnum());
      }
    };
    void XPR_write_hook(unsigned index , bits < 32 > x)  ATTRIBUTE_USED {
      {
             FOO5_WR_write(  FOO5_WR_read() + 1  )  ;
            XPR_write_inhook(  index , ( x ) ) ;
      }
    }
    void XPR_write_internal(unsigned index,const bits<32> &x)  ATTRIBUTE_USED {
      XPR_write_hook(index,x);
    };
    void XPR_write_inhook(unsigned index,const bits<32> &x)  ATTRIBUTE_USED {
      XPR()(index).set_data() = x;
    }
    void XPR_write_internal_traced(unsigned index,const bits<32> &x)  ATTRIBUTE_USED {
      XPR_write_internal(index,x);
      CondRegTrace(XPR_trace(index));
    }
    
    void XPR_write_delayed(unsigned index,const bits<32> &x,unsigned d)  ATTRIBUTE_USED {
      decltype(&P::XPR_commit_write) pCommit = &P::XPR_commit_write;
      static PacketPool<CommitRegfileWrite<decltype(this),decltype(pCommit),32,1000>, 1000>   pool;
      CommitRegfileWrite<decltype(this),decltype(pCommit),32,1000> *cmt = pool.alloc();
      cmt->reset(this,pCommit,index,x,pool);
      _commitQueue.add(cmt,d);
    }
    
    void XPR_write(unsigned index,const bits<32> &x) {
      XPR_write_internal_traced(index,x);
    };
    void XPR_commit_write(unsigned index,const bits<32> &x)  ATTRIBUTE_USED {
      XPR_write_internal_traced(index,x);
    };
    bits<32> XPR_read_internal(unsigned index)  ATTRIBUTE_USED {
      {
             FOO5_RD_write(  FOO5_RD_read() + 1  )  ;
            return XPR_read_inhook( to_uint32(index)) ;
      }
    };
    bits<32> &XPR_read_inhook(unsigned index)  ATTRIBUTE_USED {
      return XPR()(index).get_data();
    }
    
    bits<32> XPR_read(unsigned index)  ATTRIBUTE_USED {
      CondRegTrace(XPR_read_trace(index));
      return XPR_read_internal(index);
    }
    
    bits<32> XPR_read_notrace(unsigned index)  ATTRIBUTE_USED {
      return XPR_read_internal(index);
    }
    bits<16> XPR_A_read(unsigned index)  ATTRIBUTE_USED {
      CondRegTrace(
      {
        bits<32> mask;
        mask.mask(0,15);
        XPR_read_trace(index,mask);
      }
      );
      return XPR_read_notrace(index). get<16>(0,15);
    }
    
    
    void XPR_A_write_delayed(unsigned index, const bits<16> &x,unsigned d)  ATTRIBUTE_USED {
      decltype(&P::XPR_A_commit_write) pCommit = &P::XPR_A_commit_write;
      static PacketPool<CommitRegfileFieldWrite<decltype(this),decltype(pCommit),32,1000>, 1000>   pool;
      CommitRegfileFieldWrite<decltype(this),decltype(pCommit),32,1000> *cmt = pool.alloc();
      cmt->reset(this,pCommit,index, x,pool);
      _commitQueue.add(cmt,d);
    }
    
    void XPR_A_write(unsigned index, const bits<16> &x)  ATTRIBUTE_USED {
      XPR_A_commit_write(index, x);
    }
    
    void XPR_A_commit_write(unsigned index, const bits<16> &x)  ATTRIBUTE_USED {
      bits<32>tmp = XPR_read(index);
      tmp.set(0,15,x);
      XPR_write(index, tmp);
    };
    void XPR_A_write_inhook(unsigned index,const bits<16> &x)  ATTRIBUTE_USED  ATTRIBUTE_USED {
      XPR()(index).set_data().set(0,15,x);
      CondRegTrace(
      {
        bits<32> mask;
        mask.mask(0,15);
        XPR_trace(index,mask);
      }
      );
    };
    
    template <size_t Nb>
    void XPR_slice_set_delayed(unsigned index, unsigned l,unsigned r,const bits<Nb> &x,unsigned d) {
      typedef void (P::*Functor)(unsigned index, unsigned,unsigned,const bits<Nb>&);
      Functor pCommit = &P::XPR_slice_commit_write;
      static PacketPool<CommitRegfileSliceSet<decltype(this),decltype(pCommit),32,1000>, 1000>   pool;
      CommitRegfileSliceSet<decltype(this),decltype(pCommit),32,1000> *cmt = pool.alloc();
      cmt->reset(this,pCommit,index, l,r,x,pool);
      _commitQueue.add(cmt,d);
    }
    
    void XPR_slice_set_delayed(unsigned index, unsigned l,unsigned r,uint64_t x,unsigned d)  ATTRIBUTE_USED {
      XPR_slice_set_delayed(index, l,r,bits<32>(x),d);
    }
    template <size_t Nb>
    void XPR_slice_set_internal(unsigned index, unsigned l,unsigned r,const bits<Nb> &x) {
      XPR_slice_commit_write(index, l,r,x);
    }
    
    
    template <size_t Nb>
    void XPR_slice_commit_write(unsigned index, unsigned l,unsigned r,const bits<Nb> &x) {
      bits<32> tmp = XPR_read(index);
      tmp.set(l,r,x);
      XPR_write(index, tmp);
    };
    template <size_t Nb>
    void XPR_slice_set(unsigned index, unsigned l,unsigned r,const bits<Nb> &x) {
      XPR_slice_set_internal(index, l,r,x);
    }
    
    void XPR_slice_set(unsigned index, unsigned l,unsigned r,uint64_t x) {
      XPR_slice_set(index, l,r,bits<32>(x));
    };
    template <size_t Nb>
    void XPR_slice_set_inhook(unsigned index, unsigned l,unsigned r,const bits<Nb> &x) {
      XPR()(index).set_data().set(l,r,x);
      CondRegTrace(
      {
        bits<32> mask;
        mask.mask(l,r);
        XPR_trace(index,mask);
      }
      );
    }
    void XPR_slice_set_inhook(unsigned index, unsigned l,unsigned r,uint64_t x) {
      XPR_slice_set_inhook(index, l,r,bits<32>(x));
    };
    void XPR_reset_value() {
      XPR().reset(bits<32>(""));
      XPR().reset_tags();
    };
    
    
    bool getRegIndex(int &rindex,int &rfindex,const std::string &name) const
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      rindex = rfindex = -1;
      typedef _STDEXT hash_map<std::string,unsigned> RegNames;
      static RegNames::value_type inits[] = {
        RegNames::value_type("AR_R",1),
        RegNames::value_type("AR_W",2),
        RegNames::value_type("BCCNT",3),
        RegNames::value_type("BranchTaken",5),
        RegNames::value_type("CIA",6),
        RegNames::value_type("CR",7),
        RegNames::value_type("CRFL5",8),
        RegNames::value_type("CSRR0",9),
        RegNames::value_type("CSRR1",10),
        RegNames::value_type("CTR",11),
        RegNames::value_type("DEC",12),
        RegNames::value_type("DRL",14),
        RegNames::value_type("ESR",15),
        RegNames::value_type("EWTEST",16),
        RegNames::value_type("FBCTRL",17),
        RegNames::value_type("FCNT",18),
        RegNames::value_type("FOO1",19),
        RegNames::value_type("FOO10",20),
        RegNames::value_type("FOO11",21),
        RegNames::value_type("FOO12",22),
        RegNames::value_type("FOO13",23),
        RegNames::value_type("FOO14",24),
        RegNames::value_type("FOO2",25),
        RegNames::value_type("FOO3",26),
        RegNames::value_type("FOO4",27),
        RegNames::value_type("FOO5",28),
        RegNames::value_type("FOO5_RD",29),
        RegNames::value_type("FOO5_WR",30),
        RegNames::value_type("FOO6",31),
        RegNames::value_type("FOO7",32),
        RegNames::value_type("FOO8",33),
        RegNames::value_type("FOO9",34),
        RegNames::value_type("ICNT",35),
        RegNames::value_type("ITEST",36),
        RegNames::value_type("IVOR10",37),
        RegNames::value_type("IVOR4",38),
        RegNames::value_type("IVOR6",39),
        RegNames::value_type("IVOR7",40),
        RegNames::value_type("IVPR",41),
        RegNames::value_type("MSR",42),
        RegNames::value_type("NIA",43),
        RegNames::value_type("PCCNT",44),
        RegNames::value_type("PCEXCPT",45),
        RegNames::value_type("POSTEXCPT",46),
        RegNames::value_type("PREEXCPT",47),
        RegNames::value_type("SRR0",48),
        RegNames::value_type("SRR1",49),
        RegNames::value_type("Slice",50),
        RegNames::value_type("XER",51),
        RegNames::value_type("tmpESR",52),
        RegNames::value_type("AR",0),
        RegNames::value_type("BR",4),
        RegNames::value_type("DR",13),
        RegNames::value_type("GPR",56),
        RegNames::value_type("SPR",57),
        RegNames::value_type("VPR",58),
        RegNames::value_type("XPR",59),
        RegNames::value_type("A",53),
        RegNames::value_type("ARF16",54),
        RegNames::value_type("C",55),
      };
      static RegNames regnames(&inits[0],&inits[60]);
      RegNames::const_iterator iter = regnames.find(name);
      if (iter != regnames.end()) {
        rindex = iter->second;
        return true;
      }
      if ( (rfindex = checkRegFileName(name,3,"GPR")) >= 0 ) {
        rindex = 56;
        return true;
      }
      if ( (rfindex = checkRegFileName(name,3,"SPR")) >= 0 ) {
        rindex = 57;
        return true;
      }
      if ( (rfindex = checkRegFileName(name,3,"VPR")) >= 0 ) {
        rindex = 58;
        return true;
      }
      if ( (rfindex = checkRegFileName(name,3,"XPR")) >= 0 ) {
        rindex = 59;
        return true;
      }
      if ( (rfindex = checkRegFileName(name,1,"A")) >= 0 ) {
        rindex = 53;
        return true;
      }
      if ( (rfindex = checkRegFileName(name,5,"ARF16")) >= 0 ) {
        rindex = 54;
        return true;
      }
      if ( (rfindex = checkRegFileName(name,1,"C")) >= 0 ) {
        rindex = 55;
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
        case 1:
        Dbg_AR_R().set_value(bits<32>(value));
        break;
        case 2:
        Dbg_AR_W().set_value(bits<32>(value));
        break;
        case 3:
        Dbg_BCCNT().set_value(bits<32>(value));
        break;
        case 5:
        Dbg_BranchTaken().set_value(bits<32>(value));
        break;
        case 6:
        Dbg_CIA().set_value(bits<32>(value));
        break;
        case 7:
        Dbg_CR().set_value(bits<32>(value));
        break;
        case 8:
        Dbg_CRF()(5).set_value(bits<32>(value));
        break;
        case 9:
        Dbg_CSRR0().set_value(bits<32>(value));
        break;
        case 10:
        Dbg_CSRR1().set_value(bits<32>(value));
        break;
        case 11:
        Dbg_CTR().set_value(bits<32>(value));
        break;
        case 12:
        Dbg_DEC().set_value(bits<32>(value));
        break;
        case 14:
        Dbg_DR().set_value(bits<32>(value));
        break;
        case 15:
        Dbg_ESR().set_value(bits<32>(value));
        break;
        case 16:
        Dbg_EWTEST().set_value(bits<32>(value));
        break;
        case 17:
        Dbg_FBCTRL().set_value(bits<32>(value));
        break;
        case 18:
        Dbg_FCNT().set_value(bits<32>(value));
        break;
        case 19:
        Dbg_FOO1().set_value(bits<32>(value));
        break;
        case 20:
        {
          bits<16> value2 = value >> 0;
          value2 = value2(0,15);
          Dbg_FOO9().set_value(bits<16>(value2));
        } break;
        case 21:
        Dbg_FOO11().set_value(bits<32>(value));
        break;
        case 22:
        Dbg_FOO12().set_value(bits<32>(value));
        break;
        case 23:
        Dbg_FOO13().set_value(bits<32>(value));
        break;
        case 24:
        Dbg_FOO14().set_value(bits<32>(value));
        break;
        case 25:
        Dbg_FOO2().set_value(bits<64>(value));
        break;
        case 26:
        Dbg_FOO3().set_value(bits<32>(value));
        break;
        case 27:
        Dbg_GPR()(2).set_value(bits<32>(value));
        break;
        case 28:
        Dbg_FOO5().set_value(bits<32>(value));
        break;
        case 29:
        Dbg_FOO5_RD().set_value(bits<32>(value));
        break;
        case 30:
        Dbg_FOO5_WR().set_value(bits<32>(value));
        break;
        case 31:
        Dbg_FOO5().set_value(bits<32>(value));
        break;
        case 32:
        Dbg_FOO7().set_value(bits<32>(value));
        break;
        case 33:
        Dbg_FOO1().set_value(bits<64>(value));
        break;
        case 34:
        Dbg_FOO9().set_value(bits<32>(value));
        break;
        case 35:
        Dbg_ICNT().set_value(bits<32>(value));
        break;
        case 36:
        Dbg_ITEST().set_value(bits<32>(value));
        break;
        case 37:
        Dbg_IVOR10().set_value(bits<32>(value));
        break;
        case 38:
        Dbg_IVOR4().set_value(bits<32>(value));
        break;
        case 39:
        Dbg_IVOR6().set_value(bits<32>(value));
        break;
        case 40:
        Dbg_IVOR7().set_value(bits<32>(value));
        break;
        case 41:
        Dbg_IVPR().set_value(bits<32>(value));
        break;
        case 42:
        Dbg_MSR().set_value(bits<32>(value));
        break;
        case 43:
        Dbg_NIA().set_value(bits<32>(value));
        break;
        case 44:
        Dbg_PCCNT().set_value(bits<32>(value));
        break;
        case 45:
        Dbg_PCEXCPT().set_value(bits<32>(value));
        break;
        case 46:
        Dbg_POSTEXCPT().set_value(bits<32>(value));
        break;
        case 47:
        Dbg_PREEXCPT().set_value(bits<32>(value));
        break;
        case 48:
        Dbg_SRR0().set_value(bits<32>(value));
        break;
        case 49:
        Dbg_SRR1().set_value(bits<32>(value));
        break;
        case 50:
        Dbg_Slice().set_value(bits<32>(value));
        break;
        case 51:
        Dbg_XER().set_value(bits<32>(value));
        break;
        case 52:
        Dbg_tmpESR().set_value(bits<32>(value));
        break;
        case 0:
        Dbg_AR().set_value(bits<32>(value));
        break;
        case 4:
        Dbg_BR().set_value(bits<32>(value));
        break;
        case 13:
        Dbg_DR().set_value(bits<32>(value));
        break;
        case 56:
        return GPR_debug_set(index,value);
        break;
        case 57:
        return SPR_debug_set(index,value);
        break;
        case 58:
        return VPR_debug_set(index,value);
        break;
        case 59:
        return XPR_debug_set(index,value);
        break;
        case 53:
        return ARF_debug_set(index,value);
        break;
        case 54:
        return ARF16_debug_set(index,value);
        break;
        case 55:
        return CRF_debug_set(index,value);
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
        case 1:
        Dbg_AR_R().set_value(bits<32>(value));
        break;
        case 2:
        Dbg_AR_W().set_value(bits<32>(value));
        break;
        case 3:
        Dbg_BCCNT().set_value(bits<32>(value));
        break;
        case 5:
        Dbg_BranchTaken().set_value(bits<32>(value));
        break;
        case 6:
        Dbg_CIA().set_value(bits<32>(value));
        break;
        case 7:
        Dbg_CR().set_value(bits<32>(value));
        break;
        case 8:
        Dbg_CRF()(5).set_value(bits<32>(value));
        break;
        case 9:
        Dbg_CSRR0().set_value(bits<32>(value));
        break;
        case 10:
        Dbg_CSRR1().set_value(bits<32>(value));
        break;
        case 11:
        Dbg_CTR().set_value(bits<32>(value));
        break;
        case 12:
        Dbg_DEC().set_value(bits<32>(value));
        break;
        case 14:
        Dbg_DR().set_value(bits<32>(value));
        break;
        case 15:
        Dbg_ESR().set_value(bits<32>(value));
        break;
        case 16:
        Dbg_EWTEST().set_value(bits<32>(value));
        break;
        case 17:
        Dbg_FBCTRL().set_value(bits<32>(value));
        break;
        case 18:
        Dbg_FCNT().set_value(bits<32>(value));
        break;
        case 19:
        Dbg_FOO1().set_value(bits<32>(value));
        break;
        case 20:
        {
          bits<16> value2 = value >> 0;
          value2 = value2(0,15);
          Dbg_FOO9().set_value(bits<16>(value2));
        } break;
        case 21:
        Dbg_FOO11().set_value(bits<32>(value));
        break;
        case 22:
        Dbg_FOO12().set_value(bits<32>(value));
        break;
        case 23:
        Dbg_FOO13().set_value(bits<32>(value));
        break;
        case 24:
        Dbg_FOO14().set_value(bits<32>(value));
        break;
        case 25:
        Dbg_FOO2().set_value(bits<64>(value));
        break;
        case 26:
        Dbg_FOO3().set_value(bits<32>(value));
        break;
        case 27:
        Dbg_GPR()(2).set_value(bits<32>(value));
        break;
        case 28:
        Dbg_FOO5().set_value(bits<32>(value));
        break;
        case 29:
        Dbg_FOO5_RD().set_value(bits<32>(value));
        break;
        case 30:
        Dbg_FOO5_WR().set_value(bits<32>(value));
        break;
        case 31:
        Dbg_FOO5().set_value(bits<32>(value));
        break;
        case 32:
        Dbg_FOO7().set_value(bits<32>(value));
        break;
        case 33:
        Dbg_FOO1().set_value(bits<64>(value));
        break;
        case 34:
        Dbg_FOO9().set_value(bits<32>(value));
        break;
        case 35:
        Dbg_ICNT().set_value(bits<32>(value));
        break;
        case 36:
        Dbg_ITEST().set_value(bits<32>(value));
        break;
        case 37:
        Dbg_IVOR10().set_value(bits<32>(value));
        break;
        case 38:
        Dbg_IVOR4().set_value(bits<32>(value));
        break;
        case 39:
        Dbg_IVOR6().set_value(bits<32>(value));
        break;
        case 40:
        Dbg_IVOR7().set_value(bits<32>(value));
        break;
        case 41:
        Dbg_IVPR().set_value(bits<32>(value));
        break;
        case 42:
        Dbg_MSR().set_value(bits<32>(value));
        break;
        case 43:
        Dbg_NIA().set_value(bits<32>(value));
        break;
        case 44:
        Dbg_PCCNT().set_value(bits<32>(value));
        break;
        case 45:
        Dbg_PCEXCPT().set_value(bits<32>(value));
        break;
        case 46:
        Dbg_POSTEXCPT().set_value(bits<32>(value));
        break;
        case 47:
        Dbg_PREEXCPT().set_value(bits<32>(value));
        break;
        case 48:
        Dbg_SRR0().set_value(bits<32>(value));
        break;
        case 49:
        Dbg_SRR1().set_value(bits<32>(value));
        break;
        case 50:
        Dbg_Slice().set_value(bits<32>(value));
        break;
        case 51:
        Dbg_XER().set_value(bits<32>(value));
        break;
        case 52:
        Dbg_tmpESR().set_value(bits<32>(value));
        break;
        case 0:
        Dbg_AR().set_value(bits<32>(value));
        break;
        case 4:
        Dbg_BR().set_value(bits<32>(value));
        break;
        case 13:
        Dbg_DR().set_value(bits<32>(value));
        break;
        case 56:
        return GPR_debug_set(index,value);
        break;
        case 57:
        return SPR_debug_set(index,value);
        break;
        case 58:
        return VPR_debug_set(index,value);
        break;
        case 59:
        return XPR_debug_set(index,value);
        break;
        case 53:
        return ARF_debug_set(index,value);
        break;
        case 54:
        return ARF16_debug_set(index,value);
        break;
        case 55:
        return CRF_debug_set(index,value);
        break;
        default:
        return false;
      }
      checkWatches();
      return true;
    }
    #endif // ADL_HEADER
    
    virtual bool setRegExt(const std::string &name,unsigned index,const rnumber::RNumber &value)
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      int rindex,rfindex;
      if (!getRegIndex(rindex,rfindex,name)) {
        return false;
      }
      return setRegExt(rindex,((rfindex >= 0) ? rfindex : index),value);
    }
    #endif // ADL_HEADER
    
    virtual bool setRegExt(unsigned rindex,unsigned index,const rnumber::RNumber &value)
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      switch (rindex) {
        case 1:
        Dbg_AR_R().set_value(bits<32>(value));
        break;
        case 2:
        Dbg_AR_W().set_value(bits<32>(value));
        break;
        case 3:
        Dbg_BCCNT().set_value(bits<32>(value));
        break;
        case 5:
        Dbg_BranchTaken().set_value(bits<32>(value));
        break;
        case 6:
        Dbg_CIA().set_value(bits<32>(value));
        break;
        case 7:
        Dbg_CR().set_value(bits<32>(value));
        break;
        case 8:
        Dbg_CRF()(5).set_value(bits<32>(value));
        break;
        case 9:
        Dbg_CSRR0().set_value(bits<32>(value));
        break;
        case 10:
        Dbg_CSRR1().set_value(bits<32>(value));
        break;
        case 11:
        Dbg_CTR().set_value(bits<32>(value));
        break;
        case 12:
        Dbg_DEC().set_value(bits<32>(value));
        break;
        case 14:
        Dbg_DR().set_value(bits<32>(value));
        break;
        case 15:
        Dbg_ESR().set_value(bits<32>(value));
        break;
        case 16:
        EWTEST_external_write(value);
        break;
        case 17:
        Dbg_FBCTRL().set_value(bits<32>(value));
        break;
        case 18:
        Dbg_FCNT().set_value(bits<32>(value));
        break;
        case 19:
        Dbg_FOO1().set_value(bits<32>(value));
        break;
        case 20:
        {
          bits<16> value2 = value >> 0;
          value2 = value2(0,15);
          Dbg_FOO9().set_value(bits<16>(value2));
        } break;
        case 21:
        Dbg_FOO11().set_value(bits<32>(value));
        break;
        case 22:
        Dbg_FOO12().set_value(bits<32>(value));
        break;
        case 23:
        Dbg_FOO13().set_value(bits<32>(value));
        break;
        case 24:
        Dbg_FOO14().set_value(bits<32>(value));
        break;
        case 25:
        Dbg_FOO2().set_value(bits<64>(value));
        break;
        case 26:
        Dbg_FOO3().set_value(bits<32>(value));
        break;
        case 27:
        Dbg_GPR()(2).set_value(bits<32>(value));
        break;
        case 28:
        Dbg_FOO5().set_value(bits<32>(value));
        break;
        case 29:
        Dbg_FOO5_RD().set_value(bits<32>(value));
        break;
        case 30:
        Dbg_FOO5_WR().set_value(bits<32>(value));
        break;
        case 31:
        Dbg_FOO5().set_value(bits<32>(value));
        break;
        case 32:
        Dbg_FOO7().set_value(bits<32>(value));
        break;
        case 33:
        Dbg_FOO1().set_value(bits<64>(value));
        break;
        case 34:
        Dbg_FOO9().set_value(bits<32>(value));
        break;
        case 35:
        Dbg_ICNT().set_value(bits<32>(value));
        break;
        case 36:
        Dbg_ITEST().set_value(bits<32>(value));
        break;
        case 37:
        Dbg_IVOR10().set_value(bits<32>(value));
        break;
        case 38:
        Dbg_IVOR4().set_value(bits<32>(value));
        break;
        case 39:
        Dbg_IVOR6().set_value(bits<32>(value));
        break;
        case 40:
        Dbg_IVOR7().set_value(bits<32>(value));
        break;
        case 41:
        Dbg_IVPR().set_value(bits<32>(value));
        break;
        case 42:
        Dbg_MSR().set_value(bits<32>(value));
        break;
        case 43:
        Dbg_NIA().set_value(bits<32>(value));
        break;
        case 44:
        Dbg_PCCNT().set_value(bits<32>(value));
        break;
        case 45:
        Dbg_PCEXCPT().set_value(bits<32>(value));
        break;
        case 46:
        Dbg_POSTEXCPT().set_value(bits<32>(value));
        break;
        case 47:
        Dbg_PREEXCPT().set_value(bits<32>(value));
        break;
        case 48:
        Dbg_SRR0().set_value(bits<32>(value));
        break;
        case 49:
        Dbg_SRR1().set_value(bits<32>(value));
        break;
        case 50:
        Dbg_Slice().set_value(bits<32>(value));
        break;
        case 51:
        Dbg_XER().set_value(bits<32>(value));
        break;
        case 52:
        Dbg_tmpESR().set_value(bits<32>(value));
        break;
        case 0:
        Dbg_AR().set_value(bits<32>(value));
        break;
        case 4:
        Dbg_BR().set_value(bits<32>(value));
        break;
        case 13:
        Dbg_DR().set_value(bits<32>(value));
        break;
        case 56:
        return GPR_debug_set(index,value);
        break;
        case 57:
        return SPR_debug_set(index,value);
        break;
        case 58:
        return VPR_debug_set(index,value);
        break;
        case 59:
        return XPR_debug_set(index,value);
        break;
        case 53:
        return ARF_debug_set(index,value);
        break;
        case 54:
        return ARF16_debug_set(index,value);
        break;
        case 55:
        return CRF_debug_set(index,value);
        break;
        default:
        return false;
      }
      checkWatches();
      return true;
    }
    #endif // ADL_HEADER
    
    virtual bool setRegExt(const std::string &name,unsigned index,uint64_t value)
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      int rindex,rfindex;
      if (!getRegIndex(rindex,rfindex,name)) {
        return false;
      }
      return setRegExt(rindex,((rfindex >= 0) ? rfindex : index),value);
    }
    #endif // ADL_HEADER
    
    virtual bool setRegExt(unsigned rindex,unsigned index,uint64_t value)
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      switch (rindex) {
        case 1:
        Dbg_AR_R().set_value(bits<32>(value));
        break;
        case 2:
        Dbg_AR_W().set_value(bits<32>(value));
        break;
        case 3:
        Dbg_BCCNT().set_value(bits<32>(value));
        break;
        case 5:
        Dbg_BranchTaken().set_value(bits<32>(value));
        break;
        case 6:
        Dbg_CIA().set_value(bits<32>(value));
        break;
        case 7:
        Dbg_CR().set_value(bits<32>(value));
        break;
        case 8:
        Dbg_CRF()(5).set_value(bits<32>(value));
        break;
        case 9:
        Dbg_CSRR0().set_value(bits<32>(value));
        break;
        case 10:
        Dbg_CSRR1().set_value(bits<32>(value));
        break;
        case 11:
        Dbg_CTR().set_value(bits<32>(value));
        break;
        case 12:
        Dbg_DEC().set_value(bits<32>(value));
        break;
        case 14:
        Dbg_DR().set_value(bits<32>(value));
        break;
        case 15:
        Dbg_ESR().set_value(bits<32>(value));
        break;
        case 16:
        EWTEST_external_write(value);
        break;
        case 17:
        Dbg_FBCTRL().set_value(bits<32>(value));
        break;
        case 18:
        Dbg_FCNT().set_value(bits<32>(value));
        break;
        case 19:
        Dbg_FOO1().set_value(bits<32>(value));
        break;
        case 20:
        {
          bits<16> value2 = value >> 0;
          value2 = value2(0,15);
          Dbg_FOO9().set_value(bits<16>(value2));
        } break;
        case 21:
        Dbg_FOO11().set_value(bits<32>(value));
        break;
        case 22:
        Dbg_FOO12().set_value(bits<32>(value));
        break;
        case 23:
        Dbg_FOO13().set_value(bits<32>(value));
        break;
        case 24:
        Dbg_FOO14().set_value(bits<32>(value));
        break;
        case 25:
        Dbg_FOO2().set_value(bits<64>(value));
        break;
        case 26:
        Dbg_FOO3().set_value(bits<32>(value));
        break;
        case 27:
        Dbg_GPR()(2).set_value(bits<32>(value));
        break;
        case 28:
        Dbg_FOO5().set_value(bits<32>(value));
        break;
        case 29:
        Dbg_FOO5_RD().set_value(bits<32>(value));
        break;
        case 30:
        Dbg_FOO5_WR().set_value(bits<32>(value));
        break;
        case 31:
        Dbg_FOO5().set_value(bits<32>(value));
        break;
        case 32:
        Dbg_FOO7().set_value(bits<32>(value));
        break;
        case 33:
        Dbg_FOO1().set_value(bits<64>(value));
        break;
        case 34:
        Dbg_FOO9().set_value(bits<32>(value));
        break;
        case 35:
        Dbg_ICNT().set_value(bits<32>(value));
        break;
        case 36:
        Dbg_ITEST().set_value(bits<32>(value));
        break;
        case 37:
        Dbg_IVOR10().set_value(bits<32>(value));
        break;
        case 38:
        Dbg_IVOR4().set_value(bits<32>(value));
        break;
        case 39:
        Dbg_IVOR6().set_value(bits<32>(value));
        break;
        case 40:
        Dbg_IVOR7().set_value(bits<32>(value));
        break;
        case 41:
        Dbg_IVPR().set_value(bits<32>(value));
        break;
        case 42:
        Dbg_MSR().set_value(bits<32>(value));
        break;
        case 43:
        Dbg_NIA().set_value(bits<32>(value));
        break;
        case 44:
        Dbg_PCCNT().set_value(bits<32>(value));
        break;
        case 45:
        Dbg_PCEXCPT().set_value(bits<32>(value));
        break;
        case 46:
        Dbg_POSTEXCPT().set_value(bits<32>(value));
        break;
        case 47:
        Dbg_PREEXCPT().set_value(bits<32>(value));
        break;
        case 48:
        Dbg_SRR0().set_value(bits<32>(value));
        break;
        case 49:
        Dbg_SRR1().set_value(bits<32>(value));
        break;
        case 50:
        Dbg_Slice().set_value(bits<32>(value));
        break;
        case 51:
        Dbg_XER().set_value(bits<32>(value));
        break;
        case 52:
        Dbg_tmpESR().set_value(bits<32>(value));
        break;
        case 0:
        Dbg_AR().set_value(bits<32>(value));
        break;
        case 4:
        Dbg_BR().set_value(bits<32>(value));
        break;
        case 13:
        Dbg_DR().set_value(bits<32>(value));
        break;
        case 56:
        return GPR_debug_set(index,value);
        break;
        case 57:
        return SPR_debug_set(index,value);
        break;
        case 58:
        return VPR_debug_set(index,value);
        break;
        case 59:
        return XPR_debug_set(index,value);
        break;
        case 53:
        return ARF_debug_set(index,value);
        break;
        case 54:
        return ARF16_debug_set(index,value);
        break;
        case 55:
        return CRF_debug_set(index,value);
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
          case 1:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(1,"AR_R",value);
          }
          AR_R_write(value);
          break;
          case 2:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(2,"AR_W",value);
          }
          AR_W_write(value);
          break;
          case 3:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(3,"BCCNT",value);
          }
          BCCNT_write(value);
          break;
          case 5:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(5,"BranchTaken",value);
          }
          BranchTaken_write(value);
          break;
          case 6:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(6,"CIA",value);
          }
          CIA_write(value);
          break;
          case 8:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(8,"CRFL5",value);
          }
          CRFL5_write(value);
          break;
          case 9:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(9,"CSRR0",value);
          }
          CSRR0_write(value);
          break;
          case 10:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(10,"CSRR1",value);
          }
          CSRR1_write(value);
          break;
          case 11:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(11,"CTR",value);
          }
          CTR_write(value);
          break;
          case 12:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(12,"DEC",value);
          }
          DEC_write(value);
          break;
          case 14:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(14,"DRL",value);
          }
          DRL_write(value);
          break;
          case 15:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(15,"ESR",value);
          }
          ESR_write(value);
          break;
          case 16:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(16,"EWTEST",value);
          }
          EWTEST_write(value);
          break;
          case 18:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(18,"FCNT",value);
          }
          FCNT_write(value);
          break;
          case 19:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(19,"FOO1",value);
          }
          FOO1_write(value);
          break;
          case 20:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(20,"FOO10",value);
          }
          FOO10_write(value);
          break;
          case 21:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(21,"FOO11",value);
          }
          FOO11_write(value);
          break;
          case 23:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(23,"FOO13",value);
          }
          FOO13_write(value);
          break;
          case 25:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(25,"FOO2",value);
          }
          FOO2_write(value);
          break;
          case 26:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(26,"FOO3",value);
          }
          FOO3_write(value);
          break;
          case 27:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(27,"FOO4",value);
          }
          FOO4_write(value);
          break;
          case 28:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(28,"FOO5",value);
          }
          FOO5_write(value);
          break;
          case 29:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(29,"FOO5_RD",value);
          }
          FOO5_RD_write(value);
          break;
          case 30:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(30,"FOO5_WR",value);
          }
          FOO5_WR_write(value);
          break;
          case 32:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(32,"FOO7",value);
          }
          FOO7_write(value);
          break;
          case 33:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(33,"FOO8",value);
          }
          FOO8_write(value);
          break;
          case 34:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(34,"FOO9",value);
          }
          FOO9_write(value);
          break;
          case 35:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(35,"ICNT",value);
          }
          ICNT_write(value);
          break;
          case 42:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(42,"MSR",value);
          }
          MSR_write(value);
          break;
          case 43:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(43,"NIA",value);
          }
          NIA_write(value);
          break;
          case 44:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(44,"PCCNT",value);
          }
          PCCNT_write(value);
          break;
          case 46:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(46,"POSTEXCPT",value);
          }
          POSTEXCPT_write(value);
          break;
          case 47:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(47,"PREEXCPT",value);
          }
          PREEXCPT_write(value);
          break;
          case 48:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(48,"SRR0",value);
          }
          SRR0_write(value);
          break;
          case 49:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(49,"SRR1",value);
          }
          SRR1_write(value);
          break;
          case 51:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(51,"XER",value);
          }
          XER_write(value);
          break;
          case 52:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(52,"tmpESR",value);
          }
          tmpESR_write(value);
          break;
          case 0:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(0,"AR",value);
          }
          AR_write(value);
          break;
          case 13:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(13,"DR",value);
          }
          DR_write(value);
          break;
          case 56:
          if (GPR_t::validIndex(index)) {
            if (Tracing && (_trace_mode & (ttRegWrite))) {
              _logger->log_regfile_ext_write(56,"GPR",index,value);
            }
            GPR_write(index,value);
            break;
          }
          rc = false;
          break;
          case 57:
          if (SPR_t::validIndex(index)) {
            if (Tracing && (_trace_mode & (ttRegWrite))) {
              _logger->log_regfile_ext_write(57,"SPR",index,value);
            }
            SPR_write(index,value);
            break;
          }
          rc = false;
          break;
          case 58:
          if (VPR_t::validIndex(index)) {
            if (Tracing && (_trace_mode & (ttRegWrite))) {
              _logger->log_regfile_ext_write(58,"VPR",index,value);
            }
            VPR_write(index,value);
            break;
          }
          rc = false;
          break;
          case 59:
          if (XPR_t::validIndex(index)) {
            if (Tracing && (_trace_mode & (ttRegWrite))) {
              _logger->log_regfile_ext_write(59,"XPR",index,value);
            }
            XPR_write(index,value);
            break;
          }
          rc = false;
          break;
          case 53:
          if (ARF_t::validIndex(index)) {
            if (Tracing && (_trace_mode & (ttRegWrite))) {
              _logger->log_regfile_ext_write(53,"ARF",index,value);
            }
            ARF_write(index,value);
            break;
          }
          rc = false;
          break;
          case 54:
          if (ARF16_t::validIndex(index)) {
            if (Tracing && (_trace_mode & (ttRegWrite))) {
              _logger->log_regfile_ext_write(54,"ARF16",index,value);
            }
            ARF16_write(index,value);
            break;
          }
          rc = false;
          break;
          case 55:
          if (CRF_t::validIndex(index)) {
            if (Tracing && (_trace_mode & (ttRegWrite))) {
              _logger->log_regfile_ext_write(55,"CRF",index,value);
            }
            CRF_write(index,value);
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
          case 1:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(1,"AR_R",value);
          }
          AR_R_write(value);
          break;
          case 2:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(2,"AR_W",value);
          }
          AR_W_write(value);
          break;
          case 3:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(3,"BCCNT",value);
          }
          BCCNT_write(value);
          break;
          case 5:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(5,"BranchTaken",value);
          }
          BranchTaken_write(value);
          break;
          case 6:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(6,"CIA",value);
          }
          CIA_write(value);
          break;
          case 8:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(8,"CRFL5",value);
          }
          CRFL5_write(value);
          break;
          case 9:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(9,"CSRR0",value);
          }
          CSRR0_write(value);
          break;
          case 10:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(10,"CSRR1",value);
          }
          CSRR1_write(value);
          break;
          case 11:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(11,"CTR",value);
          }
          CTR_write(value);
          break;
          case 12:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(12,"DEC",value);
          }
          DEC_write(value);
          break;
          case 14:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(14,"DRL",value);
          }
          DRL_write(value);
          break;
          case 15:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(15,"ESR",value);
          }
          ESR_write(value);
          break;
          case 16:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(16,"EWTEST",value);
          }
          EWTEST_write(value);
          break;
          case 18:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(18,"FCNT",value);
          }
          FCNT_write(value);
          break;
          case 19:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(19,"FOO1",value);
          }
          FOO1_write(value);
          break;
          case 20:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(20,"FOO10",value);
          }
          FOO10_write(value);
          break;
          case 21:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(21,"FOO11",value);
          }
          FOO11_write(value);
          break;
          case 23:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(23,"FOO13",value);
          }
          FOO13_write(value);
          break;
          case 25:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(25,"FOO2",value);
          }
          FOO2_write(value);
          break;
          case 26:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(26,"FOO3",value);
          }
          FOO3_write(value);
          break;
          case 27:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(27,"FOO4",value);
          }
          FOO4_write(value);
          break;
          case 28:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(28,"FOO5",value);
          }
          FOO5_write(value);
          break;
          case 29:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(29,"FOO5_RD",value);
          }
          FOO5_RD_write(value);
          break;
          case 30:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(30,"FOO5_WR",value);
          }
          FOO5_WR_write(value);
          break;
          case 32:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(32,"FOO7",value);
          }
          FOO7_write(value);
          break;
          case 33:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(33,"FOO8",value);
          }
          FOO8_write(value);
          break;
          case 34:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(34,"FOO9",value);
          }
          FOO9_write(value);
          break;
          case 35:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(35,"ICNT",value);
          }
          ICNT_write(value);
          break;
          case 42:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(42,"MSR",value);
          }
          MSR_write(value);
          break;
          case 43:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(43,"NIA",value);
          }
          NIA_write(value);
          break;
          case 44:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(44,"PCCNT",value);
          }
          PCCNT_write(value);
          break;
          case 46:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(46,"POSTEXCPT",value);
          }
          POSTEXCPT_write(value);
          break;
          case 47:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(47,"PREEXCPT",value);
          }
          PREEXCPT_write(value);
          break;
          case 48:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(48,"SRR0",value);
          }
          SRR0_write(value);
          break;
          case 49:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(49,"SRR1",value);
          }
          SRR1_write(value);
          break;
          case 51:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(51,"XER",value);
          }
          XER_write(value);
          break;
          case 52:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(52,"tmpESR",value);
          }
          tmpESR_write(value);
          break;
          case 0:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(0,"AR",value);
          }
          AR_write(value);
          break;
          case 13:
          if (Tracing && (_trace_mode & (ttRegWrite))) {
            _logger->log_reg_ext_write(13,"DR",value);
          }
          DR_write(value);
          break;
          case 56:
          if (GPR_t::validIndex(index)) {
            if (Tracing && (_trace_mode & (ttRegWrite))) {
              _logger->log_regfile_ext_write(56,"GPR",index,value);
            }
            GPR_write(index,value);
            break;
          }
          rc = false;
          break;
          case 57:
          if (SPR_t::validIndex(index)) {
            if (Tracing && (_trace_mode & (ttRegWrite))) {
              _logger->log_regfile_ext_write(57,"SPR",index,value);
            }
            SPR_write(index,value);
            break;
          }
          rc = false;
          break;
          case 58:
          if (VPR_t::validIndex(index)) {
            if (Tracing && (_trace_mode & (ttRegWrite))) {
              _logger->log_regfile_ext_write(58,"VPR",index,value);
            }
            VPR_write(index,value);
            break;
          }
          rc = false;
          break;
          case 59:
          if (XPR_t::validIndex(index)) {
            if (Tracing && (_trace_mode & (ttRegWrite))) {
              _logger->log_regfile_ext_write(59,"XPR",index,value);
            }
            XPR_write(index,value);
            break;
          }
          rc = false;
          break;
          case 53:
          if (ARF_t::validIndex(index)) {
            if (Tracing && (_trace_mode & (ttRegWrite))) {
              _logger->log_regfile_ext_write(53,"ARF",index,value);
            }
            ARF_write(index,value);
            break;
          }
          rc = false;
          break;
          case 54:
          if (ARF16_t::validIndex(index)) {
            if (Tracing && (_trace_mode & (ttRegWrite))) {
              _logger->log_regfile_ext_write(54,"ARF16",index,value);
            }
            ARF16_write(index,value);
            break;
          }
          rc = false;
          break;
          case 55:
          if (CRF_t::validIndex(index)) {
            if (Tracing && (_trace_mode & (ttRegWrite))) {
              _logger->log_regfile_ext_write(55,"CRF",index,value);
            }
            CRF_write(index,value);
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
        case 1:
        value = Dbg_AR_R().get_value().rnum();
        break;
        case 2:
        value = Dbg_AR_W().get_value().rnum();
        break;
        case 3:
        value = Dbg_BCCNT().get_value().rnum();
        break;
        case 5:
        value = Dbg_BranchTaken().get_value().rnum();
        break;
        case 6:
        value = Dbg_CIA().get_value().rnum();
        break;
        case 7:
        value = Dbg_CR().get_value().rnum();
        break;
        case 8:
        value = Dbg_CRF()(5).get_value().rnum();
        break;
        case 9:
        value = Dbg_CSRR0().get_value().rnum();
        break;
        case 10:
        value = Dbg_CSRR1().get_value().rnum();
        break;
        case 11:
        value = Dbg_CTR().get_value().rnum();
        break;
        case 12:
        value = Dbg_DEC().get_value().rnum();
        break;
        case 14:
        value = Dbg_DR().get_value().rnum();
        break;
        case 15:
        value = Dbg_ESR().get_value().rnum();
        break;
        case 16:
        value = Dbg_EWTEST().get_value().rnum();
        break;
        case 17:
        value = Dbg_FBCTRL().get_value().rnum();
        break;
        case 18:
        value = Dbg_FCNT().get_value().rnum();
        break;
        case 19:
        value = Dbg_FOO1().get_value().rnum();
        break;
        case 20:
        value = (Dbg_FOO9().get_value()(0,15) << 0).rnum();
        break;
        case 21:
        value = Dbg_FOO11().get_value().rnum();
        break;
        case 22:
        value = Dbg_FOO12().get_value().rnum();
        break;
        case 23:
        value = Dbg_FOO13().get_value().rnum();
        break;
        case 24:
        value = Dbg_FOO14().get_value().rnum();
        break;
        case 25:
        value = Dbg_FOO2().get_value().rnum();
        break;
        case 26:
        value = Dbg_FOO3().get_value().rnum();
        break;
        case 27:
        value = Dbg_GPR()(2).get_value().rnum();
        break;
        case 28:
        value = Dbg_FOO5().get_value().rnum();
        break;
        case 29:
        value = Dbg_FOO5_RD().get_value().rnum();
        break;
        case 30:
        value = Dbg_FOO5_WR().get_value().rnum();
        break;
        case 31:
        value = Dbg_FOO5().get_value().rnum();
        break;
        case 32:
        value = Dbg_FOO7().get_value().rnum();
        break;
        case 33:
        value = Dbg_FOO1().get_value().rnum();
        break;
        case 34:
        value = Dbg_FOO9().get_value().rnum();
        break;
        case 35:
        value = Dbg_ICNT().get_value().rnum();
        break;
        case 36:
        value = Dbg_ITEST().get_value().rnum();
        break;
        case 37:
        value = Dbg_IVOR10().get_value().rnum();
        break;
        case 38:
        value = Dbg_IVOR4().get_value().rnum();
        break;
        case 39:
        value = Dbg_IVOR6().get_value().rnum();
        break;
        case 40:
        value = Dbg_IVOR7().get_value().rnum();
        break;
        case 41:
        value = Dbg_IVPR().get_value().rnum();
        break;
        case 42:
        value = Dbg_MSR().get_value().rnum();
        break;
        case 43:
        value = Dbg_NIA().get_value().rnum();
        break;
        case 44:
        value = Dbg_PCCNT().get_value().rnum();
        break;
        case 45:
        value = Dbg_PCEXCPT().get_value().rnum();
        break;
        case 46:
        value = Dbg_POSTEXCPT().get_value().rnum();
        break;
        case 47:
        value = Dbg_PREEXCPT().get_value().rnum();
        break;
        case 48:
        value = Dbg_SRR0().get_value().rnum();
        break;
        case 49:
        value = Dbg_SRR1().get_value().rnum();
        break;
        case 50:
        value = Dbg_Slice().get_value().rnum();
        break;
        case 51:
        value = Dbg_XER().get_value().rnum();
        break;
        case 52:
        value = Dbg_tmpESR().get_value().rnum();
        break;
        case 0:
        value = Dbg_AR().get_value().rnum();
        break;
        case 4:
        value = Dbg_BR().get_value().rnum();
        break;
        case 13:
        value = Dbg_DR().get_value().rnum();
        break;
        case 56:
        value = GPR_debug_get(index,rc).rnum();
        break;
        case 57:
        value = SPR_debug_get(index,rc).rnum();
        break;
        case 58:
        value = VPR_debug_get(index,rc).rnum();
        break;
        case 59:
        value = XPR_debug_get(index,rc).rnum();
        break;
        case 53:
        value = ARF_debug_get(index,rc).rnum();
        break;
        case 54:
        value = ARF16_debug_get(index,rc).rnum();
        break;
        case 55:
        value = CRF_debug_get(index,rc).rnum();
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
        case 1:
        value = Dbg_AR_R().get_value().uint64();
        break;
        case 2:
        value = Dbg_AR_W().get_value().uint64();
        break;
        case 3:
        value = Dbg_BCCNT().get_value().uint64();
        break;
        case 5:
        value = Dbg_BranchTaken().get_value().uint64();
        break;
        case 6:
        value = Dbg_CIA().get_value().uint64();
        break;
        case 7:
        value = Dbg_CR().get_value().uint64();
        break;
        case 8:
        value = Dbg_CRF()(5).get_value().uint64();
        break;
        case 9:
        value = Dbg_CSRR0().get_value().uint64();
        break;
        case 10:
        value = Dbg_CSRR1().get_value().uint64();
        break;
        case 11:
        value = Dbg_CTR().get_value().uint64();
        break;
        case 12:
        value = Dbg_DEC().get_value().uint64();
        break;
        case 14:
        value = Dbg_DR().get_value().uint64();
        break;
        case 15:
        value = Dbg_ESR().get_value().uint64();
        break;
        case 16:
        value = Dbg_EWTEST().get_value().uint64();
        break;
        case 17:
        value = Dbg_FBCTRL().get_value().uint64();
        break;
        case 18:
        value = Dbg_FCNT().get_value().uint64();
        break;
        case 19:
        value = Dbg_FOO1().get_value().uint64();
        break;
        case 20:
        value = (Dbg_FOO9().get_value()(0,15) << 0).uint64();
        break;
        case 21:
        value = Dbg_FOO11().get_value().uint64();
        break;
        case 22:
        value = Dbg_FOO12().get_value().uint64();
        break;
        case 23:
        value = Dbg_FOO13().get_value().uint64();
        break;
        case 24:
        value = Dbg_FOO14().get_value().uint64();
        break;
        case 25:
        value = Dbg_FOO2().get_value().uint64();
        break;
        case 26:
        value = Dbg_FOO3().get_value().uint64();
        break;
        case 27:
        value = Dbg_GPR()(2).get_value().uint64();
        break;
        case 28:
        value = Dbg_FOO5().get_value().uint64();
        break;
        case 29:
        value = Dbg_FOO5_RD().get_value().uint64();
        break;
        case 30:
        value = Dbg_FOO5_WR().get_value().uint64();
        break;
        case 31:
        value = Dbg_FOO5().get_value().uint64();
        break;
        case 32:
        value = Dbg_FOO7().get_value().uint64();
        break;
        case 33:
        value = Dbg_FOO1().get_value().uint64();
        break;
        case 34:
        value = Dbg_FOO9().get_value().uint64();
        break;
        case 35:
        value = Dbg_ICNT().get_value().uint64();
        break;
        case 36:
        value = Dbg_ITEST().get_value().uint64();
        break;
        case 37:
        value = Dbg_IVOR10().get_value().uint64();
        break;
        case 38:
        value = Dbg_IVOR4().get_value().uint64();
        break;
        case 39:
        value = Dbg_IVOR6().get_value().uint64();
        break;
        case 40:
        value = Dbg_IVOR7().get_value().uint64();
        break;
        case 41:
        value = Dbg_IVPR().get_value().uint64();
        break;
        case 42:
        value = Dbg_MSR().get_value().uint64();
        break;
        case 43:
        value = Dbg_NIA().get_value().uint64();
        break;
        case 44:
        value = Dbg_PCCNT().get_value().uint64();
        break;
        case 45:
        value = Dbg_PCEXCPT().get_value().uint64();
        break;
        case 46:
        value = Dbg_POSTEXCPT().get_value().uint64();
        break;
        case 47:
        value = Dbg_PREEXCPT().get_value().uint64();
        break;
        case 48:
        value = Dbg_SRR0().get_value().uint64();
        break;
        case 49:
        value = Dbg_SRR1().get_value().uint64();
        break;
        case 50:
        value = Dbg_Slice().get_value().uint64();
        break;
        case 51:
        value = Dbg_XER().get_value().uint64();
        break;
        case 52:
        value = Dbg_tmpESR().get_value().uint64();
        break;
        case 0:
        value = Dbg_AR().get_value().uint64();
        break;
        case 4:
        value = Dbg_BR().get_value().uint64();
        break;
        case 13:
        value = Dbg_DR().get_value().uint64();
        break;
        case 56:
        value = GPR_debug_get(index,rc).uint64();
        break;
        case 57:
        value = SPR_debug_get(index,rc).uint64();
        break;
        case 58:
        value = VPR_debug_get(index,rc).uint64();
        break;
        case 59:
        value = XPR_debug_get(index,rc).uint64();
        break;
        case 53:
        value = ARF_debug_get(index,rc).uint64();
        break;
        case 54:
        value = ARF16_debug_get(index,rc).uint64();
        break;
        case 55:
        value = CRF_debug_get(index,rc).uint64();
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
          case 1:
          value = AR_R_read().rnum();
          break;
          case 2:
          value = AR_W_read().rnum();
          break;
          case 3:
          value = BCCNT_read().rnum();
          break;
          case 5:
          value = BranchTaken_read().rnum();
          break;
          case 6:
          value = CIA_read().rnum();
          break;
          case 8:
          value = CRFL5_read().rnum();
          break;
          case 9:
          value = CSRR0_read().rnum();
          break;
          case 10:
          value = CSRR1_read().rnum();
          break;
          case 11:
          value = CTR_read().rnum();
          break;
          case 12:
          value = DEC_read().rnum();
          break;
          case 14:
          value = DRL_read().rnum();
          break;
          case 15:
          value = ESR_read().rnum();
          break;
          case 16:
          value = EWTEST_read().rnum();
          break;
          case 17:
          value = FBCTRL_read().rnum();
          break;
          case 18:
          value = FCNT_read().rnum();
          break;
          case 19:
          value = FOO1_read().rnum();
          break;
          case 20:
          value = FOO10_read().rnum();
          break;
          case 21:
          value = FOO11_read().rnum();
          break;
          case 23:
          value = FOO13_read().rnum();
          break;
          case 25:
          value = FOO2_read().rnum();
          break;
          case 26:
          value = FOO3_read().rnum();
          break;
          case 27:
          value = FOO4_read().rnum();
          break;
          case 28:
          value = FOO5_read().rnum();
          break;
          case 29:
          value = FOO5_RD_read().rnum();
          break;
          case 30:
          value = FOO5_WR_read().rnum();
          break;
          case 33:
          value = FOO8_read().rnum();
          break;
          case 34:
          value = FOO9_read().rnum();
          break;
          case 35:
          value = ICNT_read().rnum();
          break;
          case 36:
          value = ITEST_read().rnum();
          break;
          case 42:
          value = MSR_read().rnum();
          break;
          case 43:
          value = NIA_read().rnum();
          break;
          case 44:
          value = PCCNT_read().rnum();
          break;
          case 45:
          value = PCEXCPT_read().rnum();
          break;
          case 46:
          value = POSTEXCPT_read().rnum();
          break;
          case 47:
          value = PREEXCPT_read().rnum();
          break;
          case 48:
          value = SRR0_read().rnum();
          break;
          case 49:
          value = SRR1_read().rnum();
          break;
          case 50:
          value = Slice_read().rnum();
          break;
          case 51:
          value = XER_read().rnum();
          break;
          case 0:
          value = AR_read().rnum();
          break;
          case 4:
          value = BR_read().rnum();
          break;
          case 13:
          value = DR_read().rnum();
          break;
          case 56:
          if (GPR_t::validIndex(index)) {
            value = GPR_read(index).rnum();
            break;
          }
          rc = false;
          break;
          case 57:
          if (SPR_t::validIndex(index)) {
            value = SPR_read(index).rnum();
            break;
          }
          rc = false;
          break;
          case 58:
          if (VPR_t::validIndex(index)) {
            value = VPR_read(index).rnum();
            break;
          }
          rc = false;
          break;
          case 59:
          if (XPR_t::validIndex(index)) {
            value = XPR_read(index).rnum();
            break;
          }
          rc = false;
          break;
          case 53:
          if (ARF_t::validIndex(index)) {
            value = ARF_read(index).rnum();
            break;
          }
          rc = false;
          break;
          case 54:
          if (ARF16_t::validIndex(index)) {
            value = ARF16_read(index).rnum();
            break;
          }
          rc = false;
          break;
          case 55:
          if (CRF_t::validIndex(index)) {
            value = CRF_read(index).rnum();
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
          case 1:
          value = AR_R_read().uint64();
          break;
          case 2:
          value = AR_W_read().uint64();
          break;
          case 3:
          value = BCCNT_read().uint64();
          break;
          case 5:
          value = BranchTaken_read().uint64();
          break;
          case 6:
          value = CIA_read().uint64();
          break;
          case 8:
          value = CRFL5_read().uint64();
          break;
          case 9:
          value = CSRR0_read().uint64();
          break;
          case 10:
          value = CSRR1_read().uint64();
          break;
          case 11:
          value = CTR_read().uint64();
          break;
          case 12:
          value = DEC_read().uint64();
          break;
          case 14:
          value = DRL_read().uint64();
          break;
          case 15:
          value = ESR_read().uint64();
          break;
          case 16:
          value = EWTEST_read().uint64();
          break;
          case 17:
          value = FBCTRL_read().uint64();
          break;
          case 18:
          value = FCNT_read().uint64();
          break;
          case 19:
          value = FOO1_read().uint64();
          break;
          case 20:
          value = FOO10_read().uint64();
          break;
          case 21:
          value = FOO11_read().uint64();
          break;
          case 23:
          value = FOO13_read().uint64();
          break;
          case 25:
          value = FOO2_read().uint64();
          break;
          case 26:
          value = FOO3_read().uint64();
          break;
          case 27:
          value = FOO4_read().uint64();
          break;
          case 28:
          value = FOO5_read().uint64();
          break;
          case 29:
          value = FOO5_RD_read().uint64();
          break;
          case 30:
          value = FOO5_WR_read().uint64();
          break;
          case 33:
          value = FOO8_read().uint64();
          break;
          case 34:
          value = FOO9_read().uint64();
          break;
          case 35:
          value = ICNT_read().uint64();
          break;
          case 36:
          value = ITEST_read().uint64();
          break;
          case 42:
          value = MSR_read().uint64();
          break;
          case 43:
          value = NIA_read().uint64();
          break;
          case 44:
          value = PCCNT_read().uint64();
          break;
          case 45:
          value = PCEXCPT_read().uint64();
          break;
          case 46:
          value = POSTEXCPT_read().uint64();
          break;
          case 47:
          value = PREEXCPT_read().uint64();
          break;
          case 48:
          value = SRR0_read().uint64();
          break;
          case 49:
          value = SRR1_read().uint64();
          break;
          case 50:
          value = Slice_read().uint64();
          break;
          case 51:
          value = XER_read().uint64();
          break;
          case 0:
          value = AR_read().uint64();
          break;
          case 4:
          value = BR_read().uint64();
          break;
          case 13:
          value = DR_read().uint64();
          break;
          case 56:
          if (GPR_t::validIndex(index)) {
            value = GPR_read(index).uint64();
            break;
          }
          rc = false;
          break;
          case 57:
          if (SPR_t::validIndex(index)) {
            value = SPR_read(index).uint64();
            break;
          }
          rc = false;
          break;
          case 58:
          if (VPR_t::validIndex(index)) {
            value = VPR_read(index).uint64();
            break;
          }
          rc = false;
          break;
          case 59:
          if (XPR_t::validIndex(index)) {
            value = XPR_read(index).uint64();
            break;
          }
          rc = false;
          break;
          case 53:
          if (ARF_t::validIndex(index)) {
            value = ARF_read(index).uint64();
            break;
          }
          rc = false;
          break;
          case 54:
          if (ARF16_t::validIndex(index)) {
            value = ARF16_read(index).uint64();
            break;
          }
          rc = false;
          break;
          case 55:
          if (CRF_t::validIndex(index)) {
            value = CRF_read(index).uint64();
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
      rb.report_ctx_off(this);
      rb.report_reg(this, 1,"AR_R",Dbg_AR_R().get_value().rnum(),false);
      rb.report_reg(this, 2,"AR_W",Dbg_AR_W().get_value().rnum(),false);
      rb.report_reg(this, 3,"BCCNT",Dbg_BCCNT().get_value().rnum(),false);
      rb.report_reg(this, 5,"BranchTaken",Dbg_BranchTaken().get_value().rnum(),false);
      rb.report_reg(this, 7,"CR",Dbg_CR().get_value().rnum(),false);
      rb.report_reg(this, 8,"CRFL5",Dbg_CRF()(5).get_value().rnum(),false);
      rb.report_reg(this, 9,"CSRR0",Dbg_CSRR0().get_value().rnum(),false);
      rb.report_reg(this, 10,"CSRR1",Dbg_CSRR1().get_value().rnum(),false);
      rb.report_reg(this, 11,"CTR",Dbg_CTR().get_value().rnum(),false);
      rb.report_reg(this, 12,"DEC",Dbg_DEC().get_value().rnum(),false);
      rb.report_reg(this, 15,"ESR",Dbg_ESR().get_value().rnum(),false);
      rb.report_reg(this, 16,"EWTEST",Dbg_EWTEST().get_value().rnum(),false);
      rb.report_reg(this, 17,"FBCTRL",Dbg_FBCTRL().get_value().rnum(),false);
      rb.report_reg(this, 18,"FCNT",Dbg_FCNT().get_value().rnum(),false);
      rb.report_reg(this, 19,"FOO1",Dbg_FOO1().get_value().rnum(),false);
      rb.report_reg(this, 21,"FOO11",Dbg_FOO11().get_value().rnum(),false);
      rb.report_reg(this, 22,"FOO12",Dbg_FOO12().get_value().rnum(),false);
      rb.report_reg(this, 23,"FOO13",Dbg_FOO13().get_value().rnum(),false);
      rb.report_reg(this, 24,"FOO14",Dbg_FOO14().get_value().rnum(),false);
      rb.report_reg(this, 25,"FOO2",Dbg_FOO2().get_value().rnum(),false);
      rb.report_reg(this, 26,"FOO3",Dbg_FOO3().get_value().rnum(),false);
      rb.report_reg(this, 27,"FOO4",Dbg_GPR()(2).get_value().rnum(),false);
      rb.report_reg(this, 28,"FOO5",Dbg_FOO5().get_value().rnum(),false);
      rb.report_reg(this, 29,"FOO5_RD",Dbg_FOO5_RD().get_value().rnum(),false);
      rb.report_reg(this, 30,"FOO5_WR",Dbg_FOO5_WR().get_value().rnum(),false);
      rb.report_reg(this, 32,"FOO7",Dbg_FOO7().get_value().rnum(),false);
      rb.report_reg(this, 34,"FOO9",Dbg_FOO9().get_value().rnum(),false);
      rb.report_reg(this, 35,"ICNT",Dbg_ICNT().get_value().rnum(),false);
      rb.report_reg(this, 36,"ITEST",Dbg_ITEST().get_value().rnum(),false);
      rb.report_reg(this, 37,"IVOR10",Dbg_IVOR10().get_value().rnum(),false);
      rb.report_reg(this, 38,"IVOR4",Dbg_IVOR4().get_value().rnum(),false);
      rb.report_reg(this, 39,"IVOR6",Dbg_IVOR6().get_value().rnum(),false);
      rb.report_reg(this, 40,"IVOR7",Dbg_IVOR7().get_value().rnum(),false);
      rb.report_reg(this, 41,"IVPR",Dbg_IVPR().get_value().rnum(),false);
      rb.report_reg(this, 42,"MSR",Dbg_MSR().get_value().rnum(),false);
      rb.report_reg(this, 43,"NIA",Dbg_NIA().get_value().rnum(),false);
      rb.report_reg(this, 44,"PCCNT",Dbg_PCCNT().get_value().rnum(),false);
      rb.report_reg(this, 45,"PCEXCPT",Dbg_PCEXCPT().get_value().rnum(),false);
      rb.report_reg(this, 46,"POSTEXCPT",Dbg_POSTEXCPT().get_value().rnum(),false);
      rb.report_reg(this, 47,"PREEXCPT",Dbg_PREEXCPT().get_value().rnum(),false);
      rb.report_reg(this, 48,"SRR0",Dbg_SRR0().get_value().rnum(),false);
      rb.report_reg(this, 49,"SRR1",Dbg_SRR1().get_value().rnum(),false);
      rb.report_reg(this, 50,"Slice",Dbg_Slice().get_value().rnum(),false);
      rb.report_reg(this, 51,"XER",Dbg_XER().get_value().rnum(),false);
      rb.report_reg(this, 52,"tmpESR",Dbg_tmpESR().get_value().rnum(),false);
      rb.report_regfile(this,56,"GPR",0,Dbg_GPR().get_value(GPR_t::getReadIndex( 0)).rnum(),false);
      rb.report_regfile(this,56,"GPR",1,Dbg_GPR().get_value(GPR_t::getReadIndex( 1)).rnum(),false);
      rb.report_regfile(this,56,"GPR",2,Dbg_GPR().get_value(GPR_t::getReadIndex( 2)).rnum(),false);
      rb.report_regfile(this,56,"GPR",3,Dbg_GPR().get_value(GPR_t::getReadIndex( 3)).rnum(),false);
      rb.report_regfile(this,56,"GPR",4,Dbg_GPR().get_value(GPR_t::getReadIndex( 4)).rnum(),false);
      rb.report_regfile(this,56,"GPR",5,Dbg_GPR().get_value(GPR_t::getReadIndex( 5)).rnum(),false);
      rb.report_regfile(this,56,"GPR",6,Dbg_GPR().get_value(GPR_t::getReadIndex( 6)).rnum(),false);
      rb.report_regfile(this,56,"GPR",7,Dbg_GPR().get_value(GPR_t::getReadIndex( 7)).rnum(),false);
      rb.report_regfile(this,56,"GPR",8,Dbg_GPR().get_value(GPR_t::getReadIndex( 8)).rnum(),false);
      rb.report_regfile(this,56,"GPR",9,Dbg_GPR().get_value(GPR_t::getReadIndex( 9)).rnum(),false);
      rb.report_regfile(this,56,"GPR",10,Dbg_GPR().get_value(GPR_t::getReadIndex( 10)).rnum(),false);
      rb.report_regfile(this,56,"GPR",11,Dbg_GPR().get_value(GPR_t::getReadIndex( 11)).rnum(),false);
      rb.report_regfile(this,56,"GPR",12,Dbg_GPR().get_value(GPR_t::getReadIndex( 12)).rnum(),false);
      rb.report_regfile(this,56,"GPR",13,Dbg_GPR().get_value(GPR_t::getReadIndex( 13)).rnum(),false);
      rb.report_regfile(this,56,"GPR",14,Dbg_GPR().get_value(GPR_t::getReadIndex( 14)).rnum(),false);
      rb.report_regfile(this,56,"GPR",15,Dbg_GPR().get_value(GPR_t::getReadIndex( 15)).rnum(),false);
      rb.report_regfile(this,56,"GPR",16,Dbg_GPR().get_value(GPR_t::getReadIndex( 16)).rnum(),false);
      rb.report_regfile(this,56,"GPR",17,Dbg_GPR().get_value(GPR_t::getReadIndex( 17)).rnum(),false);
      rb.report_regfile(this,56,"GPR",18,Dbg_GPR().get_value(GPR_t::getReadIndex( 18)).rnum(),false);
      rb.report_regfile(this,56,"GPR",19,Dbg_GPR().get_value(GPR_t::getReadIndex( 19)).rnum(),false);
      rb.report_regfile(this,56,"GPR",20,Dbg_GPR().get_value(GPR_t::getReadIndex( 20)).rnum(),false);
      rb.report_regfile(this,56,"GPR",21,Dbg_GPR().get_value(GPR_t::getReadIndex( 21)).rnum(),false);
      rb.report_regfile(this,56,"GPR",22,Dbg_GPR().get_value(GPR_t::getReadIndex( 22)).rnum(),false);
      rb.report_regfile(this,56,"GPR",23,Dbg_GPR().get_value(GPR_t::getReadIndex( 23)).rnum(),false);
      rb.report_regfile(this,56,"GPR",24,Dbg_GPR().get_value(GPR_t::getReadIndex( 24)).rnum(),false);
      rb.report_regfile(this,56,"GPR",25,Dbg_GPR().get_value(GPR_t::getReadIndex( 25)).rnum(),false);
      rb.report_regfile(this,56,"GPR",26,Dbg_GPR().get_value(GPR_t::getReadIndex( 26)).rnum(),false);
      rb.report_regfile(this,56,"GPR",27,Dbg_GPR().get_value(GPR_t::getReadIndex( 27)).rnum(),false);
      rb.report_regfile(this,56,"GPR",28,Dbg_GPR().get_value(GPR_t::getReadIndex( 28)).rnum(),false);
      rb.report_regfile(this,56,"GPR",29,Dbg_GPR().get_value(GPR_t::getReadIndex( 29)).rnum(),false);
      rb.report_regfile(this,56,"GPR",30,Dbg_GPR().get_value(GPR_t::getReadIndex( 30)).rnum(),false);
      rb.report_regfile(this,56,"GPR",31,Dbg_GPR().get_value(GPR_t::getReadIndex( 31)).rnum(),false);
      rb.report_regfile(this,58,"VPR",0,Dbg_VPR().get_value(VPR_t::getReadIndex( 0)).rnum(),false);
      rb.report_regfile(this,58,"VPR",1,Dbg_VPR().get_value(VPR_t::getReadIndex( 1)).rnum(),false);
      rb.report_regfile(this,58,"VPR",2,Dbg_VPR().get_value(VPR_t::getReadIndex( 2)).rnum(),false);
      rb.report_regfile(this,58,"VPR",3,Dbg_VPR().get_value(VPR_t::getReadIndex( 3)).rnum(),false);
      rb.report_regfile(this,58,"VPR",4,Dbg_VPR().get_value(VPR_t::getReadIndex( 4)).rnum(),false);
      rb.report_regfile(this,58,"VPR",5,Dbg_VPR().get_value(VPR_t::getReadIndex( 5)).rnum(),false);
      rb.report_regfile(this,58,"VPR",6,Dbg_VPR().get_value(VPR_t::getReadIndex( 6)).rnum(),false);
      rb.report_regfile(this,58,"VPR",7,Dbg_VPR().get_value(VPR_t::getReadIndex( 7)).rnum(),false);
      rb.report_regfile(this,58,"VPR",8,Dbg_VPR().get_value(VPR_t::getReadIndex( 8)).rnum(),false);
      rb.report_regfile(this,58,"VPR",9,Dbg_VPR().get_value(VPR_t::getReadIndex( 9)).rnum(),false);
      rb.report_regfile(this,58,"VPR",10,Dbg_VPR().get_value(VPR_t::getReadIndex( 10)).rnum(),false);
      rb.report_regfile(this,58,"VPR",11,Dbg_VPR().get_value(VPR_t::getReadIndex( 11)).rnum(),false);
      rb.report_regfile(this,58,"VPR",12,Dbg_VPR().get_value(VPR_t::getReadIndex( 12)).rnum(),false);
      rb.report_regfile(this,58,"VPR",13,Dbg_VPR().get_value(VPR_t::getReadIndex( 13)).rnum(),false);
      rb.report_regfile(this,58,"VPR",14,Dbg_VPR().get_value(VPR_t::getReadIndex( 14)).rnum(),false);
      rb.report_regfile(this,58,"VPR",15,Dbg_VPR().get_value(VPR_t::getReadIndex( 15)).rnum(),false);
      rb.report_regfile(this,58,"VPR",16,Dbg_VPR().get_value(VPR_t::getReadIndex( 16)).rnum(),false);
      rb.report_regfile(this,58,"VPR",17,Dbg_VPR().get_value(VPR_t::getReadIndex( 17)).rnum(),false);
      rb.report_regfile(this,58,"VPR",18,Dbg_VPR().get_value(VPR_t::getReadIndex( 18)).rnum(),false);
      rb.report_regfile(this,58,"VPR",19,Dbg_VPR().get_value(VPR_t::getReadIndex( 19)).rnum(),false);
      rb.report_regfile(this,58,"VPR",20,Dbg_VPR().get_value(VPR_t::getReadIndex( 20)).rnum(),false);
      rb.report_regfile(this,58,"VPR",21,Dbg_VPR().get_value(VPR_t::getReadIndex( 21)).rnum(),false);
      rb.report_regfile(this,58,"VPR",22,Dbg_VPR().get_value(VPR_t::getReadIndex( 22)).rnum(),false);
      rb.report_regfile(this,58,"VPR",23,Dbg_VPR().get_value(VPR_t::getReadIndex( 23)).rnum(),false);
      rb.report_regfile(this,58,"VPR",24,Dbg_VPR().get_value(VPR_t::getReadIndex( 24)).rnum(),false);
      rb.report_regfile(this,58,"VPR",25,Dbg_VPR().get_value(VPR_t::getReadIndex( 25)).rnum(),false);
      rb.report_regfile(this,58,"VPR",26,Dbg_VPR().get_value(VPR_t::getReadIndex( 26)).rnum(),false);
      rb.report_regfile(this,58,"VPR",27,Dbg_VPR().get_value(VPR_t::getReadIndex( 27)).rnum(),false);
      rb.report_regfile(this,58,"VPR",28,Dbg_VPR().get_value(VPR_t::getReadIndex( 28)).rnum(),false);
      rb.report_regfile(this,58,"VPR",29,Dbg_VPR().get_value(VPR_t::getReadIndex( 29)).rnum(),false);
      rb.report_regfile(this,58,"VPR",30,Dbg_VPR().get_value(VPR_t::getReadIndex( 30)).rnum(),false);
      rb.report_regfile(this,58,"VPR",31,Dbg_VPR().get_value(VPR_t::getReadIndex( 31)).rnum(),false);
      rb.report_regfile(this,59,"XPR",0,Dbg_XPR().get_value(XPR_t::getReadIndex( 0)).rnum(),false);
      rb.report_regfile(this,59,"XPR",1,Dbg_XPR().get_value(XPR_t::getReadIndex( 1)).rnum(),false);
      rb.report_regfile(this,59,"XPR",2,Dbg_XPR().get_value(XPR_t::getReadIndex( 2)).rnum(),false);
      rb.report_regfile(this,59,"XPR",3,Dbg_XPR().get_value(XPR_t::getReadIndex( 3)).rnum(),false);
      rb.report_regfile(this,59,"XPR",4,Dbg_XPR().get_value(XPR_t::getReadIndex( 4)).rnum(),false);
      rb.report_regfile(this,59,"XPR",5,Dbg_XPR().get_value(XPR_t::getReadIndex( 5)).rnum(),false);
      rb.report_regfile(this,59,"XPR",6,Dbg_XPR().get_value(XPR_t::getReadIndex( 6)).rnum(),false);
      rb.report_regfile(this,59,"XPR",7,Dbg_XPR().get_value(XPR_t::getReadIndex( 7)).rnum(),false);
      rb.report_regfile(this,59,"XPR",8,Dbg_XPR().get_value(XPR_t::getReadIndex( 8)).rnum(),false);
      rb.report_regfile(this,59,"XPR",9,Dbg_XPR().get_value(XPR_t::getReadIndex( 9)).rnum(),false);
      rb.report_regfile(this,59,"XPR",10,Dbg_XPR().get_value(XPR_t::getReadIndex( 10)).rnum(),false);
      rb.report_regfile(this,59,"XPR",11,Dbg_XPR().get_value(XPR_t::getReadIndex( 11)).rnum(),false);
      rb.report_regfile(this,59,"XPR",12,Dbg_XPR().get_value(XPR_t::getReadIndex( 12)).rnum(),false);
      rb.report_regfile(this,59,"XPR",13,Dbg_XPR().get_value(XPR_t::getReadIndex( 13)).rnum(),false);
      rb.report_regfile(this,59,"XPR",14,Dbg_XPR().get_value(XPR_t::getReadIndex( 14)).rnum(),false);
      rb.report_regfile(this,59,"XPR",15,Dbg_XPR().get_value(XPR_t::getReadIndex( 15)).rnum(),false);
      rb.report_regfile(this,59,"XPR",16,Dbg_XPR().get_value(XPR_t::getReadIndex( 16)).rnum(),false);
      rb.report_regfile(this,59,"XPR",17,Dbg_XPR().get_value(XPR_t::getReadIndex( 17)).rnum(),false);
      rb.report_regfile(this,59,"XPR",18,Dbg_XPR().get_value(XPR_t::getReadIndex( 18)).rnum(),false);
      rb.report_regfile(this,59,"XPR",19,Dbg_XPR().get_value(XPR_t::getReadIndex( 19)).rnum(),false);
      rb.report_regfile(this,59,"XPR",20,Dbg_XPR().get_value(XPR_t::getReadIndex( 20)).rnum(),false);
      rb.report_regfile(this,59,"XPR",21,Dbg_XPR().get_value(XPR_t::getReadIndex( 21)).rnum(),false);
      rb.report_regfile(this,59,"XPR",22,Dbg_XPR().get_value(XPR_t::getReadIndex( 22)).rnum(),false);
      rb.report_regfile(this,59,"XPR",23,Dbg_XPR().get_value(XPR_t::getReadIndex( 23)).rnum(),false);
      rb.report_regfile(this,59,"XPR",24,Dbg_XPR().get_value(XPR_t::getReadIndex( 24)).rnum(),false);
      rb.report_regfile(this,59,"XPR",25,Dbg_XPR().get_value(XPR_t::getReadIndex( 25)).rnum(),false);
      rb.report_regfile(this,59,"XPR",26,Dbg_XPR().get_value(XPR_t::getReadIndex( 26)).rnum(),false);
      rb.report_regfile(this,59,"XPR",27,Dbg_XPR().get_value(XPR_t::getReadIndex( 27)).rnum(),false);
      rb.report_regfile(this,59,"XPR",28,Dbg_XPR().get_value(XPR_t::getReadIndex( 28)).rnum(),false);
      rb.report_regfile(this,59,"XPR",29,Dbg_XPR().get_value(XPR_t::getReadIndex( 29)).rnum(),false);
      rb.report_regfile(this,59,"XPR",30,Dbg_XPR().get_value(XPR_t::getReadIndex( 30)).rnum(),false);
      rb.report_regfile(this,59,"XPR",31,Dbg_XPR().get_value(XPR_t::getReadIndex( 31)).rnum(),false);
      for (unsigned ctx_index = 0; ctx_index != 3; ++ctx_index) {
        rb.report_ctx_change(this,0,"foobar",ctx_index);
        rb.report_reg(this, 0,"AR",AR(ctx_index).get_value().rnum(),false);
        rb.report_reg(this, 4,"BR",BR(ctx_index).get_value().rnum(),false);
        rb.report_reg(this, 13,"DR",DR(ctx_index).get_value().rnum(),false);
        rb.report_regfile(this,53,"A",0,ARF(ctx_index).get_value(ARF_t::getReadIndex( 0)).rnum(),false);
        rb.report_regfile(this,53,"A",1,ARF(ctx_index).get_value(ARF_t::getReadIndex( 1)).rnum(),false);
        rb.report_regfile(this,53,"A",2,ARF(ctx_index).get_value(ARF_t::getReadIndex( 2)).rnum(),false);
        rb.report_regfile(this,53,"A",3,ARF(ctx_index).get_value(ARF_t::getReadIndex( 3)).rnum(),false);
        rb.report_regfile(this,53,"A",4,ARF(ctx_index).get_value(ARF_t::getReadIndex( 4)).rnum(),false);
        rb.report_regfile(this,53,"A",5,ARF(ctx_index).get_value(ARF_t::getReadIndex( 5)).rnum(),false);
        rb.report_regfile(this,53,"A",6,ARF(ctx_index).get_value(ARF_t::getReadIndex( 6)).rnum(),false);
        rb.report_regfile(this,53,"A",7,ARF(ctx_index).get_value(ARF_t::getReadIndex( 7)).rnum(),false);
        rb.report_regfile(this,53,"A",8,ARF(ctx_index).get_value(ARF_t::getReadIndex( 8)).rnum(),false);
        rb.report_regfile(this,53,"A",9,ARF(ctx_index).get_value(ARF_t::getReadIndex( 9)).rnum(),false);
        rb.report_regfile(this,53,"A",10,ARF(ctx_index).get_value(ARF_t::getReadIndex( 10)).rnum(),false);
        rb.report_regfile(this,53,"A",11,ARF(ctx_index).get_value(ARF_t::getReadIndex( 11)).rnum(),false);
        rb.report_regfile(this,53,"A",12,ARF(ctx_index).get_value(ARF_t::getReadIndex( 12)).rnum(),false);
        rb.report_regfile(this,53,"A",13,ARF(ctx_index).get_value(ARF_t::getReadIndex( 13)).rnum(),false);
        rb.report_regfile(this,53,"A",14,ARF(ctx_index).get_value(ARF_t::getReadIndex( 14)).rnum(),false);
        rb.report_regfile(this,53,"A",15,ARF(ctx_index).get_value(ARF_t::getReadIndex( 15)).rnum(),false);
        rb.report_regfile(this,53,"A",16,ARF(ctx_index).get_value(ARF_t::getReadIndex( 16)).rnum(),false);
        rb.report_regfile(this,53,"A",17,ARF(ctx_index).get_value(ARF_t::getReadIndex( 17)).rnum(),false);
        rb.report_regfile(this,53,"A",18,ARF(ctx_index).get_value(ARF_t::getReadIndex( 18)).rnum(),false);
        rb.report_regfile(this,53,"A",19,ARF(ctx_index).get_value(ARF_t::getReadIndex( 19)).rnum(),false);
        rb.report_regfile(this,53,"A",20,ARF(ctx_index).get_value(ARF_t::getReadIndex( 20)).rnum(),false);
        rb.report_regfile(this,53,"A",21,ARF(ctx_index).get_value(ARF_t::getReadIndex( 21)).rnum(),false);
        rb.report_regfile(this,53,"A",22,ARF(ctx_index).get_value(ARF_t::getReadIndex( 22)).rnum(),false);
        rb.report_regfile(this,53,"A",23,ARF(ctx_index).get_value(ARF_t::getReadIndex( 23)).rnum(),false);
        rb.report_regfile(this,53,"A",24,ARF(ctx_index).get_value(ARF_t::getReadIndex( 24)).rnum(),false);
        rb.report_regfile(this,53,"A",25,ARF(ctx_index).get_value(ARF_t::getReadIndex( 25)).rnum(),false);
        rb.report_regfile(this,53,"A",26,ARF(ctx_index).get_value(ARF_t::getReadIndex( 26)).rnum(),false);
        rb.report_regfile(this,53,"A",27,ARF(ctx_index).get_value(ARF_t::getReadIndex( 27)).rnum(),false);
        rb.report_regfile(this,53,"A",28,ARF(ctx_index).get_value(ARF_t::getReadIndex( 28)).rnum(),false);
        rb.report_regfile(this,53,"A",29,ARF(ctx_index).get_value(ARF_t::getReadIndex( 29)).rnum(),false);
        rb.report_regfile(this,53,"A",30,ARF(ctx_index).get_value(ARF_t::getReadIndex( 30)).rnum(),false);
        rb.report_regfile(this,53,"A",31,ARF(ctx_index).get_value(ARF_t::getReadIndex( 31)).rnum(),false);
        rb.report_regfile(this,55,"C",0,CRF(ctx_index).get_value(CRF_t::getReadIndex( 0)).rnum(),false);
        rb.report_regfile(this,55,"C",1,CRF(ctx_index).get_value(CRF_t::getReadIndex( 1)).rnum(),false);
        rb.report_regfile(this,55,"C",2,CRF(ctx_index).get_value(CRF_t::getReadIndex( 2)).rnum(),false);
        rb.report_regfile(this,55,"C",3,CRF(ctx_index).get_value(CRF_t::getReadIndex( 3)).rnum(),false);
        rb.report_regfile(this,55,"C",4,CRF(ctx_index).get_value(CRF_t::getReadIndex( 4)).rnum(),false);
        rb.report_regfile(this,55,"C",5,CRF(ctx_index).get_value(CRF_t::getReadIndex( 5)).rnum(),false);
        rb.report_regfile(this,55,"C",6,CRF(ctx_index).get_value(CRF_t::getReadIndex( 6)).rnum(),false);
        rb.report_regfile(this,55,"C",7,CRF(ctx_index).get_value(CRF_t::getReadIndex( 7)).rnum(),false);
      }
      rb.report_ctx_off(this);
    }
    
    #endif // ADL_HEADER
    
    static RegInfos initRegInfo()
    
    #ifdef ADL_HEADER
    ;
    #else
    {
      RegInfos tmp;
      tmp.push_back(RegInfo());
      tmp.back()._id = 1;
      tmp.back()._name = "AR_R";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 2;
      tmp.back()._name = "AR_W";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 3;
      tmp.back()._name = "BCCNT";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 5;
      tmp.back()._name = "BranchTaken";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 6;
      tmp.back()._name = "CIA";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back()._attrs.push_back(std::make_pair(0,Attrs::Params()));
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 7;
      tmp.back()._name = "CR";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "cr";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(0,3));
      tmp.back()._fields.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "crb";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(0,0));
      tmp.back()._fields.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 8;
      tmp.back()._name = "CRFL5";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 1;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 9;
      tmp.back()._name = "CSRR0";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 10;
      tmp.back()._name = "CSRR1";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 11;
      tmp.back()._name = "CTR";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 12;
      tmp.back()._name = "DEC";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 14;
      tmp.back()._name = "DRL";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 1;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 15;
      tmp.back()._name = "ESR";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 16;
      tmp.back()._name = "EWTEST";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 17;
      tmp.back()._name = "FBCTRL";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 18;
      tmp.back()._name = "FCNT";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 19;
      tmp.back()._name = "FOO1";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back()._attrs.push_back(std::make_pair(10,Attrs::Params()));
      tmp.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "A";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(32,39));
      tmp.back()._fields.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "B";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(40,47));
      tmp.back()._fields.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "C";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(48,55));
      tmp.back()._fields.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 20;
      tmp.back()._name = "FOO10";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 1;
      tmp.back()._sparse = 0;
      tmp.back()._width = 16;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 21;
      tmp.back()._name = "FOO11";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 22;
      tmp.back()._name = "FOO12";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 23;
      tmp.back()._name = "FOO13";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 24;
      tmp.back()._name = "FOO14";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "A";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(0,0));
      tmp.back()._fields.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "B";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(1,1));
      tmp.back()._fields.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "C";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(2,2));
      tmp.back()._fields.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "D";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(3,3));
      tmp.back()._fields.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 25;
      tmp.back()._name = "FOO2";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 64;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "X";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(0,7));
      tmp.back()._fields.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "Y";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(0,15));
      tmp.back()._fields.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "Z";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(0,31));
      tmp.back()._fields.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 26;
      tmp.back()._name = "FOO3";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back()._attrs.push_back(std::make_pair(11,Attrs::Params()));
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 27;
      tmp.back()._name = "FOO4";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 1;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 28;
      tmp.back()._name = "FOO5";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "A";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(0,15));
      tmp.back()._fields.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "AA";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(0,15));
      tmp.back()._fields.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "B";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(16,31));
      tmp.back()._fields.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 29;
      tmp.back()._name = "FOO5_RD";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 30;
      tmp.back()._name = "FOO5_WR";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 31;
      tmp.back()._name = "FOO6";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 1;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 32;
      tmp.back()._name = "FOO7";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "A";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(0,15));
      tmp.back()._fields.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 33;
      tmp.back()._name = "FOO8";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 1;
      tmp.back()._sparse = 0;
      tmp.back()._width = 64;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 34;
      tmp.back()._name = "FOO9";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 35;
      tmp.back()._name = "ICNT";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 36;
      tmp.back()._name = "ITEST";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 37;
      tmp.back()._name = "IVOR10";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 38;
      tmp.back()._name = "IVOR4";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 39;
      tmp.back()._name = "IVOR6";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 40;
      tmp.back()._name = "IVOR7";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 41;
      tmp.back()._name = "IVPR";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 42;
      tmp.back()._name = "MSR";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "AS";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(12,12));
      tmp.back()._fields.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "DR";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(11,11));
      tmp.back()._fields.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "EE";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(16,16));
      tmp.back()._fields.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "IR";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(10,10));
      tmp.back()._fields.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "PR";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(9,9));
      tmp.back()._fields.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 43;
      tmp.back()._name = "NIA";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back()._attrs.push_back(std::make_pair(1,Attrs::Params()));
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 44;
      tmp.back()._name = "PCCNT";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 45;
      tmp.back()._name = "PCEXCPT";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 46;
      tmp.back()._name = "POSTEXCPT";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 47;
      tmp.back()._name = "PREEXCPT";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 48;
      tmp.back()._name = "SRR0";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 49;
      tmp.back()._name = "SRR1";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 50;
      tmp.back()._name = "Slice";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 51;
      tmp.back()._name = "XER";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 52;
      tmp.back()._name = "tmpESR";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 0;
      tmp.back()._name = "AR";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "A";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(0,15));
      tmp.back()._fields.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "B";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(16,31));
      tmp.back()._fields.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "C";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(0,7));
      tmp.back()._fields.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 4;
      tmp.back()._name = "BR";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "A";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(0,15));
      tmp.back()._fields.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "B";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(16,31));
      tmp.back()._fields.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "C";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(0,7));
      tmp.back()._fields.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 13;
      tmp.back()._name = "DR";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 0;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 56;
      tmp.back()._name = "GPR";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 32;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 57;
      tmp.back()._name = "SPR";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 1;
      tmp.back()._width = 32;
      tmp.back()._size = 1024;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 58;
      tmp.back()._name = "VPR";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 128;
      tmp.back()._size = 32;
      tmp.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "B";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(0,7));
      tmp.back()._fields.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "H";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(0,15));
      tmp.back()._fields.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "W";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(0,31));
      tmp.back()._fields.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 59;
      tmp.back()._name = "XPR";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 32;
      tmp.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "A";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(0,15));
      tmp.back()._fields.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "B";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(16,31));
      tmp.back()._fields.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 53;
      tmp.back()._name = "ARF";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 32;
      tmp.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "A";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(0,15));
      tmp.back()._fields.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "B";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(16,31));
      tmp.back()._fields.back().finalize();
      tmp.back()._fields.push_back(RegFieldInfo());
      tmp.back()._fields.back()._name = "C";
      tmp.back()._fields.back()._bits.push_back(std::make_pair(0,7));
      tmp.back()._fields.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 54;
      tmp.back()._name = "ARF16";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 1;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 16;
      tmp.back().finalize();
      tmp.push_back(RegInfo());
      tmp.back()._id = 55;
      tmp.back()._name = "CRF";
      tmp.back()._shared = 0;
      tmp.back()._pseudo = 0;
      tmp.back()._sparse = 0;
      tmp.back()._width = 32;
      tmp.back()._size = 8;
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
      unsigned __old__ = foobar._foobar_active; 
      for (foobar._foobar_active = 0; foobar._foobar_active != 3; ++foobar._foobar_active) {
        if (por_reset) {
          AR_reset_value(foobar._foobar_active);
          BR_reset_value(foobar._foobar_active);
          DR_reset_value(foobar._foobar_active);
        }
        if (por_reset) {
          ARF_reset_value(foobar._foobar_active);
          CRF_reset_value(foobar._foobar_active);
        }
      }
      foobar._foobar_active = __old__;
      FOO1_reset_value();
      if (por_reset) {
        AR_R_reset_value();
        AR_W_reset_value();
        BCCNT_reset_value();
        BranchTaken_reset_value();
        CIA_reset_value();
        CR_reset_value();
        CSRR0_reset_value();
        CSRR1_reset_value();
        CTR_reset_value();
        DEC_reset_value();
        ESR_reset_value();
        EWTEST_reset_value();
        FBCTRL_reset_value();
        FCNT_reset_value();
        FOO11_reset_value();
        FOO12_reset_value();
        FOO13_reset_value();
        FOO14_reset_value();
        FOO2_reset_value();
        FOO3_reset_value();
        FOO5_reset_value();
        FOO5_RD_reset_value();
        FOO5_WR_reset_value();
        FOO7_reset_value();
        FOO9_reset_value();
        ICNT_reset_value();
        ITEST_reset_value();
        IVOR10_reset_value();
        IVOR4_reset_value();
        IVOR6_reset_value();
        IVOR7_reset_value();
        IVPR_reset_value();
        MSR_reset_value();
        NIA_reset_value();
        PCCNT_reset_value();
        PCEXCPT_reset_value();
        POSTEXCPT_reset_value();
        PREEXCPT_reset_value();
        SRR0_reset_value();
        SRR1_reset_value();
        Slice_reset_value();
        XER_reset_value();
        tmpESR_reset_value();
      }
      if (por_reset) {
        GPR_reset_value();
        VPR_reset_value();
        XPR_reset_value();
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
      // We have watch-functions.
      _sym651();
      foobar_update_func();
    }
    
    //
    // Disassembler helpers
    //
    
    static const char* SPR_disasm(unsigned x) {
      switch (x) {
        case 1: return "XER";
        case 9: return "CTR";
        case 22: return "DEC";
        case 26: return "SRR0";
        case 27: return "SRR1";
        case 30: return "FOO1";
        case 31: return "FOO2";
        case 58: return "CSRR0";
        case 59: return "CSRR1";
        case 62: return "ESR";
        case 100: return "GPR20";
        case 101: return "GPR21";
        case 200: return "FOO4";
        case 201: return "FOO3";
        case 202: return "FOO5";
        case 208: return "FOO8";
        case 209: return "FOO9";
        case 210: return "FOO10";
        case 211: return "FOO11";
        case 230: return "AR";
        case 231: return "AR";
        case 232: return "AR";
        case 233: return "A0";
        case 234: return "A1";
        case 235: return "A2";
        case 250: return "SPR250";
        case 251: return "BranchTaken";
        case 301: return "FOO2";
        case 302: return "SPR302";
        case 303: return "SPR303";
        case 304: return "ITEST";
        case 400: return "EWTEST";
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
    
    inline static bits<5> get_TO(unsigned x0 ATTRIBUTE_UNUSED) {
      return ((x0&0x3e00000)>>21);
      
    }
    
    inline static bits<16> get_UI(unsigned x0 ATTRIBUTE_UNUSED) {
      return ((x0&0xffff));
      
    }
    
    inline static bits<10> get_XO(unsigned x0 ATTRIBUTE_UNUSED) {
      return ((x0&0x7fe)>>1);
      
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
    
    inline static void clear_TO (unsigned *x) {
        x[0] = x[0] & (~0x3e00000);
      
    }
    
    inline static void clear_UI (unsigned *x) {
        x[0] = x[0] & (~0xffff);
      
    }
    
    inline static void clear_XO (unsigned *x) {
        x[0] = x[0] & (~0x7fe);
      
    }
    
    
    
    // Disassembler for illops.
    static int disasm_illop(std::ostream &os,addr_t memaddr, const unsigned *insn)
    {
      os << ".long 0x" << std::setfill('0') << std::setw(8) << std::hex << *insn << std::dec;
      return 4;
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
      os << std::dec << RT;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::dec << RB;
      return 4;
    }
    
    // Instruction "add"
     void instr_handler_add(bits<5> RT, bits<5> RA, bits<5> RB)
     {
      {
             GPR_write( to_uint32( RT . uint32 (  ) ) , (  GPR_read( to_uint32( RA . uint32 (  ) ) ) + GPR_read( to_uint32( RB . uint32 (  ) ) )  ) ) ;
      }
    }
    
    // Disassembler for "addi"
    static int instr_disasm_addi(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<16>SI ATTRIBUTE_UNUSED = get_SI(tmpinsn[0]);
      // Instruction addi
      os << "addi ";
      os << std::dec << RT;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::hex << std::showbase << SI << std::noshowbase;
      return 4;
    }
    
    // Instruction "addi"
     void instr_handler_addi(bits<5> RT, bits<5> RA, bits<16> SI)
     {
      {
             AdlTypeof( SI .signExtend<32>() ) si = SI .signExtend<32>() ;
            if ( RA == 0 ) {
                   GPR_write( to_uint32( RT ) , ( si ) ) ;
              } else {
                   GPR_write( to_uint32( RT ) , (  GPR_read( to_uint32( RA ) ) + si  ) ) ;
              }
      }
    }
    
    // Disassembler for "addic."
    static int instr_disasm_addic_(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<16>SI ATTRIBUTE_UNUSED = get_SI(tmpinsn[0]);
      // Instruction addic.
      os << "addic. ";
      os << std::dec << RT;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::hex << std::showbase << SI << std::noshowbase;
      return 4;
    }
    
    // Instruction "addic."
     void instr_handler_addic_(bits<5> RT, bits<5> RA, bits<16> SI)
     {
      {
             AdlTypeof( SI .signExtend<32>() ) si = SI .signExtend<32>() ;
             GPR_write( to_uint32( RT ) , (  GPR_read( to_uint32( RA ) ) + si  ) ) ;
            setCrField ( 0 , GPR_read( to_uint32( RT ) ) , 0 ) ;
      }
    }
    
    // Disassembler for "addis"
    static int instr_disasm_addis(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<16>SI ATTRIBUTE_UNUSED = get_SI(tmpinsn[0]);
      // Instruction addis
      os << "addis ";
      os << std::dec << RT;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::hex << std::showbase << SI << std::noshowbase;
      return 4;
    }
    
    // Instruction "addis"
     void instr_handler_addis(bits<5> RT, bits<5> RA, bits<16> SI)
     {
      {
             bits < 32 > si = SI ;
            if ( RA == 0 ) {
                   GPR_write( to_uint32( RT ) , (  si << 16  ) ) ;
              } else {
                   GPR_write( to_uint32( RT ) , (  GPR_read( to_uint32( RA ) ) + ( si << 16 )  ) ) ;
              }
      }
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
      _sym53(LI);
    }
    
    // Instruction "b"
     void _sym53(bits<32> LI)
     {
      {
             NIA_write(  LI  )  ;
      }
    }
    
    // Disassembler for "bc"
    static int instr_disasm_bc(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>BO ATTRIBUTE_UNUSED = get_BO(tmpinsn[0]);
      bits<5>BI ATTRIBUTE_UNUSED = get_BI(tmpinsn[0]);
      sbits<32>BD ATTRIBUTE_UNUSED = (get_BD(tmpinsn[0]).signExtend<32>() << 2);
      // Instruction bc
      os << "bc ";
      os << std::dec << BO;
      os << ',';
      os << std::dec << BI;
      os << ',';
      os << "0x" << std::setw(16) << std::setfill('0')  << std::hex << ((memaddr + BD.int32()) & ((1ull << 32)-1));
      return 4;
    }
    
    // Wrapper for instruction "bc"
    inline void  instr_handler_bc(bits<5> BO, bits<5> BI, bits<14> BD_p)
    {
      sbits<32> BD = ((BD_p.signExtend<32>() << 2) + CIA_read());
      _sym59(BO, BI, BD);
    }
    
    // Instruction "bc"
     void _sym59(bits<5> BO, bits<5> BI, bits<32> BD)
     {
      {
            if ( ( BO ( 2 ) ) == 0 ) {
                   CTR_write(  CTR_read() - 1  )  ;
              }
             AdlTypeof( ( BO ( 2 ) != 0 ) || ( ( ( CTR_read() != 0 ) ^ BO ( 3 ) ) != 0 ) ) ctr_ok = ( BO ( 2 ) != 0 ) || ( ( ( CTR_read() != 0 ) ^ BO ( 3 ) ) != 0 ) ;
             AdlTypeof( ( BO ( 0 ) != 0 ) || ( ( CR_read() ( BI ) == BO ( 1 ) ) != 0 ) ) cond_ok = ( BO ( 0 ) != 0 ) || ( ( CR_read() ( BI ) == BO ( 1 ) ) != 0 ) ;
            if ( ctr_ok && cond_ok ) {
                   NIA_write(  BD  )  ;
              }
      }
    }
    
    // Disassembler for "bc2"
    static int instr_disasm_bc2(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>BO ATTRIBUTE_UNUSED = get_BO(tmpinsn[0]);
      bits<5>BI ATTRIBUTE_UNUSED = get_BI(tmpinsn[0]);
      sbits<32>BD ATTRIBUTE_UNUSED = (get_BD(tmpinsn[0]).signExtend<32>() << 2);
      // Instruction bc2
      os << "bc2 ";
      os << std::dec << BO;
      os << ',';
      os << std::dec << BI;
      os << ',';
      os << "0x" << std::setw(16) << std::setfill('0')  << std::hex << ((memaddr + BD.int32()) & ((1ull << 32)-1));
      return 4;
    }
    
    // Wrapper for instruction "bc2"
    inline void  instr_handler_bc2(bits<5> BO, bits<5> BI, bits<14> BD_p)
    {
      sbits<32> BD = ((BD_p.signExtend<32>() << 2) + CIA_read());
      _sym65(BO, BI, BD);
    }
    
    // Instruction "bc2"
     void _sym65(bits<5> BO, bits<5> BI, bits<32> BD)
     {
      {
             AdlTypeof( ( BO ( 0 ) != 0 ) || ( ( CR_read() ( BI ) == BO ( 1 ) ) != 0 ) ) cond_ok = ( BO ( 0 ) != 0 ) || ( ( CR_read() ( BI ) == BO ( 1 ) ) != 0 ) ;
            if ( cond_ok ) {
                   NIA_write(  BD  )  ;
              }
      }
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
      os << std::dec << RA;
      os << ',';
      os << std::dec << RB;
      return 4;
    }
    
    // Instruction "cmp"
     void instr_handler_cmp(bits<3> BF, bits<5> RA, bits<5> RB)
     {
      {
            setCrField ( BF , GPR_read( to_uint32( RA ) ) , GPR_read( to_uint32( RB ) ) ) ;
      }
    }
    
    // Disassembler for "cmpi"
    static int instr_disasm_cmpi(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<3>BF ATTRIBUTE_UNUSED = get_BF(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<16>SI ATTRIBUTE_UNUSED = get_SI(tmpinsn[0]);
      // Instruction cmpi
      os << "cmpi ";
      os << std::dec << BF;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::hex << std::showbase << SI << std::noshowbase;
      return 4;
    }
    
    // Instruction "cmpi"
     void instr_handler_cmpi(bits<3> BF, bits<5> RA, bits<16> SI)
     {
      {
             AdlTypeof( SI .signExtend<32>() ) si = SI .signExtend<32>() ;
            setCrField ( BF , GPR_read( to_uint32( RA ) ) , si ) ;
      }
    }
    
    // Disassembler for "crand"
    static int instr_disasm_crand(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>RB ATTRIBUTE_UNUSED = get_RB(tmpinsn[0]);
      // Instruction crand
      os << "crand ";
      os << std::dec << RT;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::dec << RB;
      return 4;
    }
    
    // Instruction "crand"
     void instr_handler_crand(bits<5> RT, bits<5> RA, bits<5> RB)
     {
      {
            if ( RB == 0 ) {
                   GPR_write( to_uint32( RT ) , (  FOO2_X_read( to_uint32( RA ))  ) ) ;
              } else if ( RB == 1 ) {
                   GPR_write( to_uint32( RT ) , (  FOO2_Y_read( to_uint32( RA ))  ) ) ;
              } else {
                   GPR_write( to_uint32( RT ) , (  FOO2_Z_read( to_uint32( RA ))  ) ) ;
              }
      }
    }
    
    // Disassembler for "crandc"
    static int instr_disasm_crandc(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RS ATTRIBUTE_UNUSED = get_RS(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>RB ATTRIBUTE_UNUSED = get_RB(tmpinsn[0]);
      // Instruction crandc
      os << "crandc ";
      os << std::dec << RS;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::dec << RB;
      return 4;
    }
    
    // Instruction "crandc"
     void instr_handler_crandc(bits<5> RS, bits<5> RA, bits<5> RB)
     {
      {
            if ( RB == 0 ) {
                  FOO2_X_write( to_uint32( RA ),( GPR_read( to_uint32( RS ) ) ) ) ;
              } else if ( RB == 1 ) {
                  FOO2_Y_write( to_uint32( RA ),( GPR_read( to_uint32( RS ) ) ) ) ;
              } else {
                  FOO2_Z_write( to_uint32( RA ),( GPR_read( to_uint32( RS ) ) ) ) ;
              }
      }
    }
    
    // Disassembler for "creqv"
    static int instr_disasm_creqv(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>RB ATTRIBUTE_UNUSED = get_RB(tmpinsn[0]);
      // Instruction creqv
      os << "creqv ";
      os << std::dec << RT;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::dec << RB;
      return 4;
    }
    
    // Instruction "creqv"
     void instr_handler_creqv(bits<5> RT, bits<5> RA, bits<5> RB)
     {
      {
            cond_test ( RA . uint32 (  ) , RB . uint32 (  ) , true ) ;
      }
    }
    
    // Disassembler for "crnand"
    static int instr_disasm_crnand(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>RB ATTRIBUTE_UNUSED = get_RB(tmpinsn[0]);
      // Instruction crnand
      os << "crnand ";
      os << std::dec << RT;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::dec << RB;
      return 4;
    }
    
    // Instruction "crnand"
     void instr_handler_crnand(bits<5> RT, bits<5> RA, bits<5> RB)
     {
      {
            cond_test ( RA . uint32 (  ) , RB . uint32 (  ) , false ) ;
      }
    }
    
    // Disassembler for "crnor"
    static int instr_disasm_crnor(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>RB ATTRIBUTE_UNUSED = get_RB(tmpinsn[0]);
      // Instruction crnor
      os << "crnor ";
      os << std::dec << RT;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::dec << RB;
      return 4;
    }
    
    // Instruction "crnor"
     void instr_handler_crnor(bits<5> RT, bits<5> RA, bits<5> RB)
     {
      {
            field_slice_test ( RT .get<3>(0,2) , RA ) ;
            field_slice_test ( RT ) ;
      }
    }
    
    // Disassembler for "cror"
    static int instr_disasm_cror(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>RB ATTRIBUTE_UNUSED = get_RB(tmpinsn[0]);
      // Instruction cror
      os << "cror ";
      os << std::dec << RT;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::dec << RB;
      return 4;
    }
    
    // Instruction "cror"
     void instr_handler_cror(bits<5> RT, bits<5> RA, bits<5> RB)
     {
      {
             CTR_write_delayed(  GPR_read( to_uint32( RA ) ) + GPR_read( to_uint32( RB ) )  ,to_uint32(1))  ;
             GPR_write_delayed( to_uint32( RT ) , (  GPR_read( to_uint32( RA ) ) + GPR_read( to_uint32( RB ) )  ) ,to_uint32(2)) ;
            FOO1_C_write_delayed(  GPR_read( to_uint32( RA ) )  ,to_uint32(1)) ;
            FOO2_X_write_delayed( to_uint32(1),( GPR_read( to_uint32( RB ) ) ) ,to_uint32(2)) ;
            XER_slice_set_delayed( to_uint32(0),to_uint32(15),( 0x1234 ) ,to_uint32(3) ) ;
            XPR_A_write_delayed( to_uint32(1),0x4321 ,to_uint32(2)) ;
            VPR_H_write_delayed( to_uint32(4),to_uint32(1),0xdead ,to_uint32(3)) ;
            GPR_slice_set_delayed( to_uint32( RT + 1 ), to_uint32(8),to_uint32(23) , (  2 * GPR_read( to_uint32( RA ) )  ) ,to_uint32(3) ); ;
      }
    }
    
    // Disassembler for "crorc"
    static int instr_disasm_crorc(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>RB ATTRIBUTE_UNUSED = get_RB(tmpinsn[0]);
      // Instruction crorc
      os << "crorc ";
      os << std::dec << RT;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::dec << RB;
      return 4;
    }
    
    // Instruction "crorc"
     void instr_handler_crorc(bits<5> RT, bits<5> RA, bits<5> RB)
     {
      {
            if ( RT == 0 ) {
                  enable_rollback ( RA . uint32 (  ) ) ;
              } else if ( RT == 1 ) {
                   AdlTypeof( NIA_read() ) nia = NIA_read() ;
                  commit_rollback (  ) ;
                   NIA_write( nia )  ;
              } else if ( RT == 2 ) {
                  flush_rollback (  ) ;
              }
      }
    }
    
    // Disassembler for "crxor"
    static int instr_disasm_crxor(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>RB ATTRIBUTE_UNUSED = get_RB(tmpinsn[0]);
      // Instruction crxor
      os << "crxor ";
      os << std::dec << RT;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::dec << RB;
      return 4;
    }
    
    // Instruction "crxor"
     void instr_handler_crxor(bits<5> RT, bits<5> RA, bits<5> RB)
     {
      {
            str_func ( "3120" , "zvvv" , RT . uint32 (  ) , RA . uint32 (  ) ) ;
      }
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
    
    // Disassembler for "lmw"
    static int instr_disasm_lmw(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<16>D ATTRIBUTE_UNUSED = get_D(tmpinsn[0]);
      // Instruction lmw
      os << "lmw ";
      os << std::dec << RT;
      os << ",";
      os << std::hex << std::showbase << D << std::noshowbase;
      os << "(";
      os << std::dec << RA;
      os << ")";
      return 4;
    }
    
    // Instruction "lmw"
     void instr_handler_lmw(bits<5> RT, bits<5> RA, bits<16> D)
     {
      {
             AdlTypeof( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) b = ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ;
             AdlTypeof( b + D ) addr = b + D ;
             bits < 512 > mem_data = mem_read<64>(addr) ;
            if ( RT < 16 ) {
                   GPR_write( to_uint32(0) , (  mem_data.get<32>(0,31)  ) ) ;
                   GPR_write( to_uint32(1) , (  mem_data.get<32>(32,63)  ) ) ;
                   GPR_write( to_uint32(2) , (  mem_data.get<32>(64,95)  ) ) ;
                   GPR_write( to_uint32(3) , (  mem_data.get<32>(96,127)  ) ) ;
                   GPR_write( to_uint32(4) , (  mem_data.get<32>(128,159)  ) ) ;
                   GPR_write( to_uint32(5) , (  mem_data.get<32>(160,191)  ) ) ;
                   GPR_write( to_uint32(6) , (  mem_data.get<32>(192,223)  ) ) ;
                   GPR_write( to_uint32(7) , (  mem_data.get<32>(224,255)  ) ) ;
                   GPR_write( to_uint32(8) , (  mem_data.get<32>(256,287)  ) ) ;
                   GPR_write( to_uint32(9) , (  mem_data.get<32>(288,319)  ) ) ;
                   GPR_write( to_uint32(10) , (  mem_data.get<32>(320,351)  ) ) ;
                   GPR_write( to_uint32(11) , (  mem_data.get<32>(352,383)  ) ) ;
                   GPR_write( to_uint32(12) , (  mem_data.get<32>(384,415)  ) ) ;
                   GPR_write( to_uint32(13) , (  mem_data.get<32>(416,447)  ) ) ;
                   GPR_write( to_uint32(14) , (  mem_data.get<32>(448,479)  ) ) ;
                   GPR_write( to_uint32(15) , (  mem_data.get<32>(480,511)  ) ) ;
              } else {
                   GPR_write( to_uint32(16) , (  mem_data.get<32>(0,31)  ) ) ;
                   GPR_write( to_uint32(17) , (  mem_data.get<32>(32,63)  ) ) ;
                   GPR_write( to_uint32(18) , (  mem_data.get<32>(64,95)  ) ) ;
                   GPR_write( to_uint32(19) , (  mem_data.get<32>(96,127)  ) ) ;
                   GPR_write( to_uint32(20) , (  mem_data.get<32>(128,159)  ) ) ;
                   GPR_write( to_uint32(21) , (  mem_data.get<32>(160,191)  ) ) ;
                   GPR_write( to_uint32(22) , (  mem_data.get<32>(192,223)  ) ) ;
                   GPR_write( to_uint32(23) , (  mem_data.get<32>(224,255)  ) ) ;
                   GPR_write( to_uint32(24) , (  mem_data.get<32>(256,287)  ) ) ;
                   GPR_write( to_uint32(25) , (  mem_data.get<32>(288,319)  ) ) ;
                   GPR_write( to_uint32(26) , (  mem_data.get<32>(320,351)  ) ) ;
                   GPR_write( to_uint32(27) , (  mem_data.get<32>(352,383)  ) ) ;
                   GPR_write( to_uint32(28) , (  mem_data.get<32>(384,415)  ) ) ;
                   GPR_write( to_uint32(29) , (  mem_data.get<32>(416,447)  ) ) ;
                   GPR_write( to_uint32(30) , (  mem_data.get<32>(448,479)  ) ) ;
                   GPR_write( to_uint32(31) , (  mem_data.get<32>(480,511)  ) ) ;
              }
      }
    }
    
    // Disassembler for "lvx"
    static int instr_disasm_lvx(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>RB ATTRIBUTE_UNUSED = get_RB(tmpinsn[0]);
      // Instruction lvx
      os << "lvx ";
      os << std::dec << RT;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::dec << RB;
      return 4;
    }
    
    // Instruction "lvx"
     void instr_handler_lvx(bits<5> RT, bits<5> RA, bits<5> RB)
     {
      {
             AdlTypeof( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) a = ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ;
             AdlTypeof( a + GPR_read( to_uint32( RB ) ) ) ea = a + GPR_read( to_uint32( RB ) ) ;
             AdlTypeof(ea) orig_ea = ea ;
            ea = ea & ~ 0xfULL ;
             VPR_write( to_uint32( RT ) , (  mem_read<16>(ea,CacheRead,true,orig_ea)  ) ) ;
      }
    }
    
    // Disassembler for "lwarx"
    static int instr_disasm_lwarx(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>RB ATTRIBUTE_UNUSED = get_RB(tmpinsn[0]);
      // Instruction lwarx
      os << "lwarx ";
      os << std::dec << RT;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::dec << RB;
      return 4;
    }
    
    // Instruction "lwarx"
     void instr_handler_lwarx(bits<5> RT, bits<5> RA, bits<5> RB)
     {
      {
             AdlTypeof( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) b = ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ;
             AdlTypeof( b + GPR_read( to_uint32( RB ) ) ) addr = b + GPR_read( to_uint32( RB ) ) ;
             GPR_write( to_uint32( RT ) , (  mem_read<4>(addr , MaxCacheAccess + 10 )  ) ) ;
             XPR_write( to_uint32( CTR_read() ) , ( addr ) ) ;
      }
    }
    
    // Disassembler for "lwz"
    static int instr_disasm_lwz(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<16>D ATTRIBUTE_UNUSED = get_D(tmpinsn[0]);
      // Instruction lwz
      os << "lwz ";
      os << std::dec << RT;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::hex << std::showbase << D << std::noshowbase;
      return 4;
    }
    
    // Instruction "lwz"
     void instr_handler_lwz(bits<5> RT, bits<5> RA, bits<16> D)
     {
      {
             AdlTypeof( D .signExtend<32>() ) d = D .signExtend<32>() ;
             AdlTypeof( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) b = ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ;
             AdlTypeof( b + d ) addr = b + d ;
            if ( addr % 4 != 0 ) {
                  raiseException ( Program ) ;
              }
             GPR_write( to_uint32( RT ) , (  mem_read<4>(addr)  ) ) ;
      }
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
      os << std::dec << RT;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::dec << RB;
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
    
    // Disassembler for "mcrxr"
    static int instr_disasm_mcrxr(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<3>BF ATTRIBUTE_UNUSED = get_BF(tmpinsn[0]);
      // Instruction mcrxr
      os << "mcrxr ";
      os << std::dec << BF;
      return 4;
    }
    
    // Instruction "mcrxr"
     void instr_handler_mcrxr(bits<3> BF)
     {
      {
            CR_slice_set(  to_uint32( 4 * BF + 32 ),to_uint32( 4 * BF + 35 ) ,(  XER_read() .get<4>(32,35)  )  ) ;
            XER_slice_set( to_uint32(32),to_uint32(35),( 0 )  ) ;
      }
    }
    
    // Disassembler for "mfspr"
    static int instr_disasm_mfspr(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<10>SPRN ATTRIBUTE_UNUSED = get_SPRN(tmpinsn[0]);
      // Instruction mfspr
      os << "mfspr ";
      os << std::dec << RT;
      os << ',';
      os << std::hex << std::showbase << SPRN << std::noshowbase;
      return 4;
    }
    
    // Instruction "mfspr"
     void instr_handler_mfspr(bits<5> RT, bits<10> SPRN)
     {
      {
            if ( ! SPR() . validIndex ( SPRN ) ) {
                  tmpESR_slice_set( to_uint32(4),to_uint32(4),( 1 )  ) ;
                   Program_t prog ;
                  prog . ESR . set ( 4 , 4 , 1 ) ;
                  raiseException ( prog ) ;
              }
             GPR_write( to_uint32( RT ) , (  SPR_read( to_uint32( SPRN ) )  ) ) ;
      }
    }
    
    // Disassembler for "mtmsr"
    static int instr_disasm_mtmsr(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RS ATTRIBUTE_UNUSED = get_RS(tmpinsn[0]);
      // Instruction mtmsr
      os << "mtmsr ";
      os << std::dec << RS;
      return 4;
    }
    
    // Instruction "mtmsr"
     void instr_handler_mtmsr(bits<5> RS)
     {
      {
             MSR_write(  GPR_read( to_uint32( RS ) )  )  ;
      }
    }
    
    // Disassembler for "mtspr"
    static int instr_disasm_mtspr(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RS ATTRIBUTE_UNUSED = get_RS(tmpinsn[0]);
      bits<10>SPRN ATTRIBUTE_UNUSED = get_SPRN(tmpinsn[0]);
      // Instruction mtspr
      os << "mtspr ";
      os << std::dec << RS;
      os << ',';
      os << std::hex << std::showbase << SPRN << std::noshowbase;
      return 4;
    }
    
    // Instruction "mtspr"
     void instr_handler_mtspr(bits<5> RS, bits<10> SPRN)
     {
      {
            if ( ! SPR() . validIndex ( SPRN ) ) {
                   Program_t prog ;
                  if ( FOO1_A_read() != 0 ) {
                        tmpESR_slice_set( to_uint32(4),to_uint32(4),( 1 )  ) ;
                        prog . ESR . set ( 4 , 4 , 1 ) ;
                        raiseException ( prog ) ;
                    } else {
                         tmpESR_write( 0x00f00000 )  ;
                        prog . ESR = 0x00f00000 ;
                        raiseException ( prog ) ;
                    }
              }
             SPR_write( to_uint32( SPRN ) , (  GPR_read( to_uint32( RS ) )  ) ) ;
      }
    }
    
    // Disassembler for "mulhw"
    static int instr_disasm_mulhw(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>RB ATTRIBUTE_UNUSED = get_RB(tmpinsn[0]);
      // Instruction mulhw
      os << "mulhw ";
      os << std::dec << RT;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::dec << RB;
      return 4;
    }
    
    // Instruction "mulhw"
     void instr_handler_mulhw(bits<5> RT, bits<5> RA, bits<5> RB)
     {
      {
             bits < 64 > a = GPR_read( to_uint32( RA ) ) ;
             bits < 64 > b = GPR_read( to_uint32( RB ) ) ;
             GPR_write( to_uint32( RT ) , (  ( a * b ) .get<32>(0,31)  ) ) ;
      }
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
      os << std::dec << RT;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::dec << RB;
      return 4;
    }
    
    // Instruction "mullw"
     void instr_handler_mullw(bits<5> RT, bits<5> RA, bits<5> RB)
     {
      {
             GPR_write( to_uint32( RT ) , (  GPR_read( to_uint32( RA ) ) * GPR_read( to_uint32( RB ) )  ) ) ;
      }
    }
    
    // Disassembler for "or"
    static int instr_disasm_or(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RS ATTRIBUTE_UNUSED = get_RS(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>RB ATTRIBUTE_UNUSED = get_RB(tmpinsn[0]);
      // Instruction or
      os << "or ";
      os << std::dec << RS;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::dec << RB;
      return 4;
    }
    
    // Instruction "or"
     void instr_handler_or(bits<5> RS, bits<5> RA, bits<5> RB)
     {
      {
             GPR_write( to_uint32( RA ) , (  GPR_read( to_uint32( RS ) ) | GPR_read( to_uint32( RB ) )  ) ) ;
      }
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
      os << std::dec << RS;
      os << ',';
      os << std::dec << RA;
      os << ',';
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
    
    // Disassembler for "rfci"
    static int instr_disasm_rfci(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RS ATTRIBUTE_UNUSED = get_RS(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>RB ATTRIBUTE_UNUSED = get_RB(tmpinsn[0]);
      // Instruction rfci
      os << "rfci ";
      os << std::dec << RS;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::dec << RB;
      return 4;
    }
    
    // Instruction "rfci"
     void instr_handler_rfci(bits<5> RS, bits<5> RA, bits<5> RB)
     {
      {
             MSR_write(  CSRR1_read()  )  ;
             NIA_write(  CSRR0_read()  )  ;
      }
    }
    
    // Disassembler for "rfi"
    static int instr_disasm_rfi(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RS ATTRIBUTE_UNUSED = get_RS(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>RB ATTRIBUTE_UNUSED = get_RB(tmpinsn[0]);
      // Instruction rfi
      os << "rfi ";
      os << std::dec << RS;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::dec << RB;
      return 4;
    }
    
    // Instruction "rfi"
     void instr_handler_rfi(bits<5> RS, bits<5> RA, bits<5> RB)
     {
      {
             MSR_write(  SRR1_read()  )  ;
             NIA_write(  SRR0_read()  )  ;
      }
    }
    
    // Disassembler for "rlwimi"
    static int instr_disasm_rlwimi(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RS ATTRIBUTE_UNUSED = get_RS(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>SH ATTRIBUTE_UNUSED = get_SH(tmpinsn[0]);
      bits<5>MB ATTRIBUTE_UNUSED = get_MB(tmpinsn[0]);
      bits<5>ME ATTRIBUTE_UNUSED = get_ME(tmpinsn[0]);
      // Instruction rlwimi
      os << "rlwimi ";
      os << std::dec << RS;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::dec << SH;
      os << ',';
      os << std::dec << MB;
      os << ',';
      os << std::dec << ME;
      return 4;
    }
    
    // Instruction "rlwimi"
     void instr_handler_rlwimi(bits<5> RS, bits<5> RA, bits<5> SH, bits<5> MB, bits<5> ME)
     {
      {
             int slice = Slice_read() . uint32 (  ) ;
             AdlTypeof( GPR_read( to_uint32( RS ) ) . left_rotate ( SH ) ) r = GPR_read( to_uint32( RS ) ) . left_rotate ( SH ) ;
             bits < 32 > m ;
            m . mask ( MB , ME ) ;
            GPR_slice_set( to_uint32(RA), to_uint32(slice),to_uint32(31) , (  ( r & m ) | ( GPR_read( to_uint32( RA ) ) & ~ m )  )  );
      }
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
      // Instruction rlwinm
      os << "rlwinm ";
      os << std::dec << RS;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::dec << SH;
      os << ',';
      os << std::dec << MB;
      os << ',';
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
    
    // Disassembler for "stmw"
    static int instr_disasm_stmw(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RS ATTRIBUTE_UNUSED = get_RS(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<16>D ATTRIBUTE_UNUSED = get_D(tmpinsn[0]);
      // Instruction stmw
      os << "stmw ";
      os << std::dec << RS;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::hex << std::showbase << D << std::noshowbase;
      return 4;
    }
    
    // Instruction "stmw"
     void instr_handler_stmw(bits<5> RS, bits<5> RA, bits<16> D)
     {
      {
             AdlTypeof( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) b = ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ;
             AdlTypeof( b + D ) addr = b + D ;
             int r = RS . uint32 (  ) ;
            if ( ( addr & 0x7 ) != 0 ) {
                  mem_write<4>(addr,to_bits<ns_bits>( GPR_read( to_uint32(r) ) )) ;
                  r = r + 1 ;
                  addr = addr + 4 ;
              }
            do {
                  mem_write<8>(addr,to_bits<ns_bits>( concat ( GPR_read( to_uint32(r) ) , GPR_read( to_uint32( r + 1 ) ) ) )) ;
                  r = r + 2 ;
                  addr = addr + 8 ;
              } while ( r <= 30 ) ;
            if ( r == 31 ) {
                  mem_write<4>(addr,to_bits<ns_bits>( GPR_read( to_uint32(31) ) )) ;
              }
      }
    }
    
    // Disassembler for "stvx"
    static int instr_disasm_stvx(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RS ATTRIBUTE_UNUSED = get_RS(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>RB ATTRIBUTE_UNUSED = get_RB(tmpinsn[0]);
      // Instruction stvx
      os << "stvx ";
      os << std::dec << RS;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::dec << RB;
      return 4;
    }
    
    // Instruction "stvx"
     void instr_handler_stvx(bits<5> RS, bits<5> RA, bits<5> RB)
     {
      {
             AdlTypeof( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) a = ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ;
             AdlTypeof( a + GPR_read( to_uint32( RB ) ) ) ea = a + GPR_read( to_uint32( RB ) ) ;
             AdlTypeof(ea) orig_ea = ea ;
            ea = ea & ~ 0xfULL ;
            mem_write<16>(ea,to_bits<ns_bits>( VPR_read( to_uint32( RS ) ) ),CacheWrite,true,orig_ea) ;
      }
    }
    
    // Disassembler for "stw"
    static int instr_disasm_stw(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RS ATTRIBUTE_UNUSED = get_RS(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<16>D ATTRIBUTE_UNUSED = get_D(tmpinsn[0]);
      // Instruction stw
      os << "stw ";
      os << std::dec << RS;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::hex << std::showbase << D << std::noshowbase;
      return 4;
    }
    
    // Instruction "stw"
     void instr_handler_stw(bits<5> RS, bits<5> RA, bits<16> D)
     {
      {
             AdlTypeof( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) b = ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ;
             AdlTypeof( D .signExtend<32>() ) d = D .signExtend<32>() ;
             AdlTypeof( b + d ) addr = b + d ;
            mem_write<4>(addr,to_bits<ns_bits>( GPR_read( to_uint32( RS ) ) )) ;
      }
    }
    
    // Disassembler for "stwcx."
    static int instr_disasm_stwcx_(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RS ATTRIBUTE_UNUSED = get_RS(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>RB ATTRIBUTE_UNUSED = get_RB(tmpinsn[0]);
      // Instruction stwcx.
      os << "stwcx. ";
      os << std::dec << RS;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::dec << RB;
      return 4;
    }
    
    // Instruction "stwcx."
     void instr_handler_stwcx_(bits<5> RS, bits<5> RA, bits<5> RB)
     {
      {
             AdlTypeof( ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ) b = ( RA == 0 ) ? 0 : GPR_read( to_uint32( RA ) ) ;
             AdlTypeof( b + GPR_read( to_uint32( RB ) ) ) addr = b + GPR_read( to_uint32( RB ) ) ;
            mem_write<4>(addr,to_bits<ns_bits>( GPR_read( to_uint32( RS ) ) ) , MaxCacheAccess + 11 ) ;
      }
    }
    
    // Disassembler for "stwu"
    static int instr_disasm_stwu(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RS ATTRIBUTE_UNUSED = get_RS(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<16>D ATTRIBUTE_UNUSED = get_D(tmpinsn[0]);
      // Instruction stwu
      os << "stwu ";
      os << std::dec << RS;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::hex << std::showbase << D << std::noshowbase;
      return 4;
    }
    
    // Instruction "stwu"
     void instr_handler_stwu(bits<5> RS, bits<5> RA, bits<16> D)
     {
      {
             AdlTypeof( D .signExtend<32>() ) d = D .signExtend<32>() ;
             AdlTypeof( GPR_read( to_uint32( RA ) ) + d ) addr = GPR_read( to_uint32( RA ) ) + d ;
            mem_write<4>(addr,to_bits<ns_bits>( GPR_read( to_uint32( RS ) ) )) ;
             GPR_write( to_uint32( RA ) , ( addr ) ) ;
      }
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
      os << std::dec << RS;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::dec << RB;
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
    
    // Disassembler for "tw"
    static int instr_disasm_tw(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>RB ATTRIBUTE_UNUSED = get_RB(tmpinsn[0]);
      // Instruction tw
      os << "tw ";
      os << std::dec << RA;
      os << ',';
      os << std::dec << RB;
      return 4;
    }
    
    // Instruction "tw"
     void instr_handler_tw(bits<5> RA, bits<5> RB)
     {
      {
             FOO1_write(  GPR_read( to_uint32( RA ) )  )  ;
             GPR_write( to_uint32( RB ) , (  FOO1_A_read() + 1  ) ) ;
            FOO1_C_write(  GPR_read( to_uint32( RB + 1 ) )  ) ;
             AdlTypeof( FOO1_A_read() .get<4>(0,3) . uint32 (  ) ) xx = FOO1_A_read() .get<4>(0,3) . uint32 (  ) ;
             GPR_write( to_uint32(31) , (  xx + 1  ) ) ;
             GPR_write( to_uint32( RB + 7 ) , (  XPR_A_read( to_uint32(4)) .get<8>(8,15)  ) ) ;
             GPR_write( to_uint32( RB + 1 ) , (  concat ( FOO3_read() .get<16>(48,63) , FOO3_read() .get<16>(32,47) )  ) ) ;
             AdlTypeof( FOO3_read() ) a = FOO3_read() ;
             AdlTypeof( a.get<32>(0,31) ) b = a.get<32>(0,31) ;
            ternary_test1 ( true , ( RB + 2 ) . uint32 (  ) ) ;
            ternary_test2 ( false , ( RB + 3 ) . uint32 (  ) ) ;
             int m = 32 ;
             GPR_write( to_uint32( RB + 4 ) , (  FOO3_read() ( m , m + 11 )  ) ) ;
             GPR_write( to_uint32( RB + 4 ) , (  GPR_read( to_uint32( RB + 4 ) ) + FOO3_read() ( 63 )  ) ) ;
            GPR_slice_set( to_uint32( RB + 5 ), to_uint32(16),to_uint32(31) , (  FOO3_read() .get<16>(16,31)  )  ); ;
             unsigned x = FOO3_read() .get<2>(0,1) . uint32 (  ) ;
            GPR_slice_set( to_uint32( RB + 5 ), to_uint32(0),to_uint32(1) , ( x )  ); ;
            GPR_slice_set( to_uint32( RB + 5 ), to_uint32(2) , to_uint32(2) , (  FOO3_read() ( 2 )  )  ); ;
            GPR_slice_set( to_uint32( RB + 5 ), to_uint32(3) , to_uint32(3) , (  FOO3_read() ( 3 )  )  ); ;
            {
                   unsigned a = 4 , b = 7 ;
                  GPR_slice_set( to_uint32( RB + 5 ), to_uint32(a),to_uint32(b) , (  FOO3_read() ( a + 16 , b + 16 )  )  ); ;
              }
            {
                   bits < 5 > a = 8 , b = 11 ;
                  GPR_slice_set( to_uint32( RB + 5 ), to_uint32(a),to_uint32(b) , (  FOO3_read() ( a + 16 , b + 16 )  )  ); ;
              }
            AR_ctx_write( to_uint32(0) ,  BR_ctx_read(to_uint32(1))  ) ;
            ARF_ctx_write( to_uint32(0) , to_uint32(10) , (  ARF_ctx_read( to_uint32(1) , to_uint32(12) )  ) ) ;
             ARF16_write( to_uint32(10) , (  ARF16_read( to_uint32(3) )  ) ) ;
            AR_A_ctx_write(to_uint32( 0 ) ,  BR_B_ctx_read(to_uint32( 1 )) ) ;
            BR_C_ctx_write(to_uint32( 0 ) , to_uint32(1),( AR_C_ctx_read(to_uint32( 1 ) , to_uint32(2)) )) ;
            BR_B_ctx_write(to_uint32( 1 ) ,  GPR_read( to_uint32( RA ) ) .get<16>(0,15) ) ;
            ARF_A_ctx_write(to_uint32( 0 ),to_uint32( 13 ), ARF_B_ctx_read(to_uint32( 1 ),to_uint32( 14 )) ) ;
            ARF_C_ctx_write(to_uint32( 0 ),to_uint32(14),to_uint32(1), ARF_C_ctx_read(to_uint32( 1 ),to_uint32(15),to_uint32(2)) ) ;
            AR_ctx_slice_set( to_uint32(1),to_uint32(16),to_uint32(31),(  BR_ctx_read(to_uint32(0)) .get<16>(16,31)  ) ) ;
            ARF_ctx_slice_set(to_uint32( 0 ) , to_uint32(15), to_uint32(16),to_uint32(31) , (  ARF_ctx_read( to_uint32( 1 ) , to_uint32(16) ) .get<16>(0,15)  ) ); ;
            {
               using namespace std;
              ostringstream ss;
              ss <<  "This is a warning message and should always show up." ;
              _logger->log_annotation(mWarn,1,ss.str());
               }
             ;
             int i = 7 ;
            if (Tracing && (_trace_mode & ttAnnotation)) {
               using namespace std;
              ostringstream ss;
              ss <<  "This is an info message with data and should only show up when tracing is on."  ;
              LogBase::Data data[] = { LogBase::Data("ea",(1)), LogBase::Data("ra",(2 + i)), LogBase::Data("stuff",(0xdeadbeef12345678ull)), LogBase::Data("str1",("foo")), LogBase::Data("str2",("bar")) };
              _logger->log_annotation(mInfo,2,ss.str(),5,data);
               }
             ;
             CRFL5_write( 10 )  ;
             GPR_write( to_uint32( RB + 8 ) , (  CRFL5_read()  ) ) ;
             DRL_write( 20 )  ;
             GPR_write( to_uint32( RB + 9 ) , (  DRL_read()  ) ) ;
      }
    }
    
    // Disassembler for "twi"
    static int instr_disasm_twi(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>TO ATTRIBUTE_UNUSED = get_TO(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<16>SI ATTRIBUTE_UNUSED = get_SI(tmpinsn[0]);
      // Instruction twi
      os << "twi ";
      os << std::dec << TO;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::hex << std::showbase << SI << std::noshowbase;
      return 4;
    }
    
    // Instruction "twi"
     void instr_handler_twi(bits<5> TO, bits<5> RA, bits<16> SI)
     {
      {
            FOO5_A_write(  FOO5_A_read() + 10  ) ;
            FOO5_slice_set( to_uint32(16),to_uint32(31),(  SI * 3  )  ) ;
            XPR_A_write( to_uint32( RA ), FOO5_A_read()  ) ;
            XPR_slice_set( to_uint32(RA), to_uint32(16),to_uint32(31) , (  SI * 5  )  );
             FOO7_write(  FOO5_read()  )  ;
             GPR_write( to_uint32( RA ) , (  SI .get<3>(13,15) + bits < 3 > ( 5 )  ) ) ;
             AdlTypeof(0xdeadbeef) x = 0xdeadbeef ;
             bits < 32 > y = getfield_FOO5_AA ( x ) ;
            y = setfield_FOO5_AA ( y , 0x1234 ) ;
             GPR_write( to_uint32( RA + 1 ) , ( y ) ) ;
      }
    }
    
    // Disassembler for "vaddubm"
    static int instr_disasm_vaddubm(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>RB ATTRIBUTE_UNUSED = get_RB(tmpinsn[0]);
      // Instruction vaddubm
      os << "vaddubm ";
      os << std::dec << RT;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::dec << RB;
      return 4;
    }
    
    // Instruction "vaddubm"
     void instr_handler_vaddubm(bits<5> RT, bits<5> RA, bits<5> RB)
     {
      {
            for (  unsigned i = 0 ; i != VPR_read( to_uint32( RT ) ) . size (  ) / 8 ; ++ i ) {
                  VPR_B_write( to_uint32( RT ),to_uint32(i), VPR_B_read( to_uint32( RA ),to_uint32(i)) + VPR_B_read( to_uint32( RB ),to_uint32(i))  ) ;
              }
      }
    }
    
    // Disassembler for "vadduhm"
    static int instr_disasm_vadduhm(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>RB ATTRIBUTE_UNUSED = get_RB(tmpinsn[0]);
      // Instruction vadduhm
      os << "vadduhm ";
      os << std::dec << RT;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::dec << RB;
      return 4;
    }
    
    // Instruction "vadduhm"
     void instr_handler_vadduhm(bits<5> RT, bits<5> RA, bits<5> RB)
     {
      {
            for (  unsigned i = 0 ; i != VPR_read( to_uint32( RT ) ) . size (  ) / 16 ; ++ i ) {
                  VPR_H_write( to_uint32( RT ),to_uint32(i), VPR_H_read( to_uint32( RA ),to_uint32(i)) + VPR_H_read( to_uint32( RB ),to_uint32(i))  ) ;
              }
      }
    }
    
    // Disassembler for "vadduwm"
    static int instr_disasm_vadduwm(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>RB ATTRIBUTE_UNUSED = get_RB(tmpinsn[0]);
      // Instruction vadduwm
      os << "vadduwm ";
      os << std::dec << RT;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::dec << RB;
      return 4;
    }
    
    // Instruction "vadduwm"
     void instr_handler_vadduwm(bits<5> RT, bits<5> RA, bits<5> RB)
     {
      {
            for (  unsigned i = 0 ; i != VPR_read( to_uint32( RT ) ) . size (  ) / 32 ; ++ i ) {
                  VPR_W_write( to_uint32( RT ),to_uint32(i), VPR_W_read( to_uint32( RA ),to_uint32(i)) + VPR_W_read( to_uint32( RB ),to_uint32(i))  ) ;
              }
      }
    }
    
    // Disassembler for "xor"
    static int instr_disasm_xor(std::ostream &os, addr_t memaddr,const uint32_t* insn)
    {
      unsigned tmpinsn[1] ATTRIBUTE_UNUSED = {insn[0]};
      
      bits<5>RT ATTRIBUTE_UNUSED = get_RT(tmpinsn[0]);
      bits<5>RA ATTRIBUTE_UNUSED = get_RA(tmpinsn[0]);
      bits<5>RB ATTRIBUTE_UNUSED = get_RB(tmpinsn[0]);
      // Instruction xor
      os << "xor ";
      os << std::dec << RT;
      os << ',';
      os << std::dec << RA;
      os << ',';
      os << std::dec << RB;
      return 4;
    }
    
    // Instruction "xor"
     void instr_handler_xor(bits<5> RT, bits<5> RA, bits<5> RB)
     {
      {
             GPR_write( to_uint32( RA ) , (  GPR_read( to_uint32( RB ) )  ) ) ;
             GPR_write( to_uint32( RT ) , (  GPR_read( to_uint32( RA ) ) * 2  ) ) ;
      }
    }
    
    void post_exec_handler()
    {
          register_commit (  ) ;
    }
    
    // Branch-taken handler function.
    void branch_taken_handler()
    {
           BranchTaken_write(  BranchTaken_read() + 1  )  ;
    }
    
    void pre_cycle_handler()
    {
          register_pre_cycle (  ) ;
    }
    
    void post_cycle_handler()
    {
          register_post_cycle (  ) ;
    }
    
    void decode_miss_handler(addr_t ea , unsigned ) ATTRIBUTE_USED
    {
          tmpESR_slice_set( to_uint32(4),to_uint32(4),( 1 )  ) ;
           Program_t prog ;
          prog . ESR . set ( 4 , 4 , 1 ) ;
          raiseException ( prog ) ;
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
    
    void check_context_updates ()
    {
      if (_context_update_event) {
        _context_update_event = false;
        foobar_update_func();
      }
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
        
        try {
          {
            ++_instr_count;
            CIA_write(NIA_read_notrace());
            if (Tracing && (_trace_mode & ttInstrPrefetch)) {
              _logger->log_instr_prefetch(CIA_read().uint64());
            }
            pre_cycle_handler();
            {
              {
                    register_fetch (  ) ;
                    if ( DEC_read() > 0 ) {
                           DEC_write(  DEC_read() - 1  )  ;
                          if ( DEC_read() == 0 ) {
                                setException ( dec ) ;
                            }
                      }
              }
            }
            __sym27[0] = instr_read32(CIA_read().uint64());
            exec(true);
            
            post_exec_handler();
            post_cycle_handler();
          }
        }
        catch (Exception current_exception) {
          setException(current_exception);
        }
        if (__exception_flag) {
          GenerateException:
          unsigned original_exception_flag = __exception_flag;
          genExceptionsInternal(__exception_flag);
          __exception_flag &= ~original_exception_flag;
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
          instr_handler_halt();
          rc = esEndBasicBlock;
        }
        break;
        case 0xc000000: {
          // Instruction:  "twi"
          if (Tracing) {
            if (_trace_mode & ttInstr) {
              _logger->log_instr(__sym27,8,"twi",instr_disasm_twi,0);
            }
          }
          NIA_write_simple(CIA_read() + 4);
          instr_handler_twi(get_TO(__sym27[0]), get_RA(__sym27[0]), get_SI(__sym27[0]));
        }
        break;
        case 0x10000000: {
          switch (__sym27[0] & 0x7fe) {
            case 0x0: {
              // Instruction:  "vaddubm"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"vaddubm",instr_disasm_vaddubm,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              instr_handler_vaddubm(get_RT(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
            }
            break;
            case 0x40: {
              // Instruction:  "vadduhm"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"vadduhm",instr_disasm_vadduhm,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              instr_handler_vadduhm(get_RT(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
            }
            break;
            case 0x80: {
              // Instruction:  "vadduwm"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"vadduwm",instr_disasm_vadduwm,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              instr_handler_vadduwm(get_RT(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
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
        case 0x2c000000: {
          // Instruction:  "cmpi"
          if (Tracing) {
            if (_trace_mode & ttInstr) {
              _logger->log_instr(__sym27,8,"cmpi",instr_disasm_cmpi,0);
            }
          }
          NIA_write_simple(CIA_read() + 4);
          instr_handler_cmpi(get_BF(__sym27[0]), get_RA(__sym27[0]), get_SI(__sym27[0]));
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
          instr_handler_addic_(get_RT(__sym27[0]), get_RA(__sym27[0]), get_SI(__sym27[0]));
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
          instr_handler_addi(get_RT(__sym27[0]), get_RA(__sym27[0]), get_SI(__sym27[0]));
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
          instr_handler_addis(get_RT(__sym27[0]), get_RA(__sym27[0]), get_SI(__sym27[0]));
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
              instr_handler_bc(get_BO(__sym27[0]), get_BI(__sym27[0]), get_BD(__sym27[0]));
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
          switch (__sym27[0] & 0x3) {
            case 0x0: {
              // Instruction:  "bc2"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"bc2",instr_disasm_bc2,ifBrCond);
                }
                if (_trace_mode & ttBasicBlockEnd) {
                  _logger->log_basic_block_end(CIA_read().uint64());
                }
              }
              NIA_write_simple(CIA_read() + 4);
              instr_handler_bc2(get_BO(__sym27[0]), get_BI(__sym27[0]), get_BD(__sym27[0]));
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
              instr_handler_b(get_LI(__sym27[0]));
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
          switch (__sym27[0] & 0x7fe) {
            case 0x42: {
              // Instruction:  "crnor"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"crnor",instr_disasm_crnor,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              instr_handler_crnor(get_RT(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
            }
            break;
            case 0x64: {
              // Instruction:  "rfi"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"rfi",instr_disasm_rfi,ifBrUncond);
                }
                if (_trace_mode & ttBasicBlockEnd) {
                  _logger->log_basic_block_end(CIA_read().uint64());
                }
              }
              NIA_write_simple(CIA_read() + 4);
              instr_handler_rfi(get_RS(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
              if (Tracing && (_trace_mode && ttBranchTaken)) {
                _logger->log_branch_end(CIA_read().uint64());
              }
              rc = esEndBasicBlock;
            }
            break;
            case 0x66: {
              // Instruction:  "rfci"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"rfci",instr_disasm_rfci,ifBrUncond);
                }
                if (_trace_mode & ttBasicBlockEnd) {
                  _logger->log_basic_block_end(CIA_read().uint64());
                }
              }
              NIA_write_simple(CIA_read() + 4);
              instr_handler_rfci(get_RS(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
              if (Tracing && (_trace_mode && ttBranchTaken)) {
                _logger->log_branch_end(CIA_read().uint64());
              }
              rc = esEndBasicBlock;
            }
            break;
            case 0x102: {
              // Instruction:  "crandc"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"crandc",instr_disasm_crandc,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              instr_handler_crandc(get_RS(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
            }
            break;
            case 0x182: {
              // Instruction:  "crxor"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"crxor",instr_disasm_crxor,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              instr_handler_crxor(get_RT(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
            }
            break;
            case 0x1c2: {
              // Instruction:  "crnand"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"crnand",instr_disasm_crnand,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              instr_handler_crnand(get_RT(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
            }
            break;
            case 0x202: {
              // Instruction:  "crand"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"crand",instr_disasm_crand,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              instr_handler_crand(get_RT(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
            }
            break;
            case 0x242: {
              // Instruction:  "creqv"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"creqv",instr_disasm_creqv,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              instr_handler_creqv(get_RT(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
            }
            break;
            case 0x342: {
              // Instruction:  "crorc"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"crorc",instr_disasm_crorc,ifBrUncond);
                }
                if (_trace_mode & ttBasicBlockEnd) {
                  _logger->log_basic_block_end(CIA_read().uint64());
                }
              }
              NIA_write_simple(CIA_read() + 4);
              instr_handler_crorc(get_RT(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
              if (Tracing && (_trace_mode && ttBranchTaken)) {
                _logger->log_branch_end(CIA_read().uint64());
              }
              rc = esEndBasicBlock;
            }
            break;
            case 0x382: {
              // Instruction:  "cror"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"cror",instr_disasm_cror,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              instr_handler_cror(get_RT(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
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
        case 0x50000000: {
          // Instruction:  "rlwimi"
          if (Tracing) {
            if (_trace_mode & ttInstr) {
              _logger->log_instr(__sym27,8,"rlwimi",instr_disasm_rlwimi,0);
            }
          }
          NIA_write_simple(CIA_read() + 4);
          instr_handler_rlwimi(get_RS(__sym27[0]), get_RA(__sym27[0]), get_SH(__sym27[0]), get_MB(__sym27[0]), get_ME(__sym27[0]));
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
          instr_handler_rlwinm(get_RS(__sym27[0]), get_RA(__sym27[0]), get_SH(__sym27[0]), get_MB(__sym27[0]), get_ME(__sym27[0]));
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
          instr_handler_ori(get_RS(__sym27[0]), get_RA(__sym27[0]), get_UI(__sym27[0]));
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
              instr_handler_cmp(get_BF(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
            }
            break;
            case 0x8: {
              // Instruction:  "tw"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"tw",instr_disasm_tw,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              instr_handler_tw(get_RA(__sym27[0]), get_RB(__sym27[0]));
            }
            break;
            case 0x28: {
              // Instruction:  "lwarx"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"lwarx",instr_disasm_lwarx,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              instr_handler_lwarx(get_RT(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
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
              instr_handler_lwzx(get_RT(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
            }
            break;
            case 0x96: {
              // Instruction:  "mulhw"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"mulhw",instr_disasm_mulhw,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              instr_handler_mulhw(get_RT(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
            }
            break;
            case 0xce: {
              // Instruction:  "lvx"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"lvx",instr_disasm_lvx,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              instr_handler_lvx(get_RT(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
            }
            break;
            case 0x124: {
              // Instruction:  "mtmsr"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"mtmsr",instr_disasm_mtmsr,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              instr_handler_mtmsr(get_RS(__sym27[0]));
            }
            break;
            case 0x12c: {
              // Instruction:  "stwcx."
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"stwcx.",instr_disasm_stwcx_,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              instr_handler_stwcx_(get_RS(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
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
              instr_handler_stwx(get_RS(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
            }
            break;
            case 0x1ce: {
              // Instruction:  "stvx"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"stvx",instr_disasm_stvx,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              instr_handler_stvx(get_RS(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
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
              instr_handler_mullw(get_RT(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
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
              instr_handler_add(get_RT(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
            }
            break;
            case 0x278: {
              // Instruction:  "xor"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"xor",instr_disasm_xor,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              instr_handler_xor(get_RT(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
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
              instr_handler_mfspr(get_RT(__sym27[0]), get_SPRN(__sym27[0]));
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
              instr_handler_or(get_RS(__sym27[0]), get_RA(__sym27[0]), get_RB(__sym27[0]));
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
              instr_handler_mtspr(get_RS(__sym27[0]), get_SPRN(__sym27[0]));
            }
            break;
            case 0x400: {
              // Instruction:  "mcrxr"
              if (Tracing) {
                if (_trace_mode & ttInstr) {
                  _logger->log_instr(__sym27,8,"mcrxr",instr_disasm_mcrxr,0);
                }
              }
              NIA_write_simple(CIA_read() + 4);
              instr_handler_mcrxr(get_BF(__sym27[0]));
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
          instr_handler_lwz(get_RT(__sym27[0]), get_RA(__sym27[0]), get_D(__sym27[0]));
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
          instr_handler_stw(get_RS(__sym27[0]), get_RA(__sym27[0]), get_D(__sym27[0]));
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
          instr_handler_stwu(get_RS(__sym27[0]), get_RA(__sym27[0]), get_D(__sym27[0]));
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
          instr_handler_lmw(get_RT(__sym27[0]), get_RA(__sym27[0]), get_D(__sym27[0]));
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
          instr_handler_stmw(get_RS(__sym27[0]), get_RA(__sym27[0]), get_D(__sym27[0]));
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
      // Commit updates.
      empty_commit_queue();
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
        try {
          exec(false);
        }
        catch (Exception current_exception) {
          setException(current_exception);
        }
        if (__exception_flag) {
          GenerateException:
          unsigned original_exception_flag = __exception_flag;
          genExceptionsInternal(__exception_flag);
          __exception_flag &= ~original_exception_flag;
        }
        return (activeCount()) ? ACTIVE : HALT;
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
      return IssCore::get_sim_stat(parm);
    }
    #endif // ADL_HEADER
    
    uint64_t getTDCounter() const { return getInstrCount(); };
    
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
        return false;
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
      _wp_counter = 0;
    };
    
    unsigned setBreakpointInternal(addr_t a,bool is_tmp,PtBase *payload) {
      unsigned index = _bp_counter++;
      _breakpoints[a] = MemPoint(index,is_tmp,payload);
      return index;
    }
    
    bool clearPointsByAddr(addr_t addr, MemPoints &points,bool is_bp,bool is_read) {
      MemPoints::iterator iter;
      if ((iter = points.find(addr)) != points.end()) {
        points.erase(iter);
        return true;
      }
      return false;
    }
    
    bool clearPoint(MemPoints &points,unsigned h,bool is_bp,bool is_read) {
      for (MemPoints::iterator iter = points.begin(); iter != points.end(); ) {
        if (iter->second._handle == h) {
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
      }
      if (type & wpWrite) {
        _write_watchpoints[a & ~WordMask] = MemPoint(index,false,payload);
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
      __exception_flag = 0;
      __pending_exception_flag = 0;
      _lastRealAddr = 0;
      foobar.reset();;
      resetRegs(por_reset,log);
      for(int i=0;i<1;++i){__sym27[i]=0;};
      _active = true;
      InstrCountBase::reset();
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
    
    Program_t _Program_data;
    unsigned __exception_flag;
    unsigned __pending_exception_flag;
    CommitQueues<4>  _commitQueue;
    addr_t _lastRealAddr;
    AR_R_t _AR_R_Reg;
    AR_W_t _AR_W_Reg;
    BCCNT_t _BCCNT_Reg;
    BranchTaken_t _BranchTaken_Reg;
    CIA_t _CIA_Reg;
    CR_t _CR_Reg;
    CSRR0_t _CSRR0_Reg;
    CSRR1_t _CSRR1_Reg;
    CTR_t _CTR_Reg;
    DEC_t _DEC_Reg;
    ESR_t _ESR_Reg;
    EWTEST_t _EWTEST_Reg;
    FBCTRL_t _FBCTRL_Reg;
    FCNT_t _FCNT_Reg;
    FOO1_t _FOO1_Reg;
    FOO11_t _FOO11_Reg;
    FOO12_t _FOO12_Reg;
    FOO13_t _FOO13_Reg;
    FOO14_t _FOO14_Reg;
    FOO2_t _FOO2_Reg;
    FOO3_t _FOO3_Reg;
    FOO5_t _FOO5_Reg;
    FOO5_RD_t _FOO5_RD_Reg;
    FOO5_WR_t _FOO5_WR_Reg;
    FOO7_t _FOO7_Reg;
    FOO9_t _FOO9_Reg;
    ICNT_t _ICNT_Reg;
    ITEST_t _ITEST_Reg;
    IVOR10_t _IVOR10_Reg;
    IVOR4_t _IVOR4_Reg;
    IVOR6_t _IVOR6_Reg;
    IVOR7_t _IVOR7_Reg;
    IVPR_t _IVPR_Reg;
    MSR_t _MSR_Reg;
    NIA_t _NIA_Reg;
    PCCNT_t _PCCNT_Reg;
    PCEXCPT_t _PCEXCPT_Reg;
    POSTEXCPT_t _POSTEXCPT_Reg;
    PREEXCPT_t _PREEXCPT_Reg;
    SRR0_t _SRR0_Reg;
    SRR1_t _SRR1_Reg;
    Slice_t _Slice_Reg;
    XER_t _XER_Reg;
    tmpESR_t _tmpESR_Reg;
    GPR_t _GPR_Reg;
    SPR_t _SPR_Reg;
    VPR_t _VPR_Reg;
    XPR_t _XPR_Reg;
    foobar_obj_t foobar;
    bool _context_update_event;
    ActiveCallback *_active_callback;;
    mutable uint32_t __sym27[1];
    SysCallExtHandler *_schandler;
    MemPoints _breakpoints;
    MemPoints _read_watchpoints;
    MemPoints _write_watchpoints;
    unsigned _bp_counter;
    unsigned _wp_counter;
    
    P(const std::string &__n, const IssNode *parent, unsigned &id, unsigned child_id = 0) : 
    IssCore(__n,"P",parent,id,child_id), 
    __exception_flag(0), 
    __pending_exception_flag(0), 
    _lastRealAddr(0), 
    foobar(*this), 
    _context_update_event(false), 
    _active_callback(0), 
    _schandler(0), 
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
    static const char* fname = "mod3rp";
    return fname;
  }
  
  const AttrData &getAttrData() {
    static std::pair<const std::string,int> attr_inits[13] = {
      std::make_pair("asynchronous",12),
      std::make_pair("cia",0),
      std::make_pair("doc_hidden",2),
      std::make_pair("doc_no_code",3),
      std::make_pair("doc_no_expand_exprs",4),
      std::make_pair("indeterminate",11),
      std::make_pair("load",7),
      std::make_pair("nia",1),
      std::make_pair("other",5),
      std::make_pair("prefix",6),
      std::make_pair("squash",9),
      std::make_pair("store",8),
      std::make_pair("unarchitected",10),
    };
    static AttrData attr_hash(&attr_inits[0],&attr_inits[13]);
    return attr_hash;
  }
  
  #ifndef ADL_HEADER
  static bool dummy ATTRIBUTE_UNUSED = log_usage("mod3rp","2018.2.28",0,"model","make-iss");
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

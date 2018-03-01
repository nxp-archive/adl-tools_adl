//
// ADL/Uadl version 3.17.2
//
// Cycle-accurate/approximate model generated for PMT
//

#ifdef UADL_SEPARATE_COMPILE
#  define ADL_HEADER
#endif
#include "dlx-mt1-safe-iss.cc"
#define _OSM_Thread_IMPLEMENTATION_H
#include "dlx-mt1-safe.h"

using namespace std;
using namespace adl;
using namespace adliss;




void adliss::PMT::Thread::synchronizeActivation()
{
  _uarch->force_reset_fetch();
}

void adliss::PMT::Thread::handle_memory_request(unsigned memId, const adl::MemAccess &ma) {
  _uarch->handle_memory_request(memId,ma);
};

void adliss::PMT::Thread::enable_mem_logging(bool e) {
  _uarch->_log_mem = e;
};

void adliss::PMT::Thread::branch_target_update(addr_t target,unsigned size)
{
  if (_uarch->is_executing(*_uarch->_cur_instr)) {
    _uarch->set_branch_target(target);
  }
}

void adliss::PMT::Thread::branch_handler()
{
  if (_uarch->is_executing(*_uarch->_cur_instr)) {
    _uarch->_cur_instr->set_branch_finish();
    _uarch->_speculative = !_uarch->last_branch_prediction_correct(*_uarch->_cur_instr);
    _uarch->_spec_tag = _uarch->_cur_instr->iid();
  }
  _uarch->branch_user_handler(*_uarch->_cur_instr);
}

namespace uadl {
  
  const bool Tracing = true;
  
  static bool no_move_p(Inst_PMT *,OSM_PMT &,Logger *,unsigned) ATTRIBUTE_UNUSED;
  static bool no_move_p(Inst_PMT *,OSM_PMT &,Logger *,unsigned) {
    return true;
  }
  
  struct DummyOp {
    DummyOp &thisInstr() { return *this; };
    
    bool can_read(OSM_Thread &,Inst_PMT &,Logger *) {
      return true;
    }
    
    unsigned mask(Inst_PMT &,unsigned = 0) const { return 0; }
    
    unsigned value(OSM_Thread &,Inst_PMT &,Logger *) const { return 0; }
    
    template <typename T0>
    bool can_read_fp(UadlArch &uarch,InstrBase &ic,Logger *logger,T0 &t0) {
      return true;
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(UadlArch &uarch,InstrBase &ic,Logger *logger,T0 &t0,T1 &t1) {
      return true;
    }
    
    template <typename T0>
    void read(UadlArch &uarch,InstrBase &ic,Logger *logger,T0 &t0) {
    }
    
    template <typename T0, typename T1>
    void read(UadlArch &uarch,InstrBase &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    template <typename T0>
    bool read_avail(UadlArch &uarch,InstrBase &ic,Logger *logger,T0 &t0) {
      return true;
    }
    
    template <typename T0, typename T1>
    bool read_avail(UadlArch &uarch,InstrBase &ic,Logger *logger,T0 &t0,T1 &t1) {
      return true;
    }
    
    bool read(OSM_Thread &,Inst_PMT &,Logger *) {
      return false;
    }
    
    void record_sources(OSM_Thread &,Inst_PMT &,Logger *) {}
    
    void log_read(UadlArch &,Inst_PMT &,Logger *,const string &,unsigned) {
    }
    
    bool can_write(OSM_Thread &,Inst_PMT &,Logger *) {
      return true;
    }
    
    void allocate(OSM_Thread &,Inst_PMT &,Logger *) {
    }
    
    void deallocate(OSM_Thread &,Inst_PMT &,Logger *) {
    }
    
    void write(OSM_Thread &,Inst_PMT &,Logger *) {
    }
    unsigned field_value(OSM_Thread &,Inst_PMT &,Logger *) const { return 0; }
    
    bool is_real(OSM_Thread &,Inst_PMT &,Logger *) const { return false; }
    
    std::string name(UadlArch &,Inst_PMT &,unsigned = 0,unsigned = 0) const { return std::string(); }
    
    RegId id(UadlArch &,const Inst_PMT &,unsigned = 0,unsigned id = 0) const { return RegInvalidId; }
    
    unsigned slot(unsigned = 0,unsigned = 0) const { return 0; }
    
    unsigned num_sub_ops() const { return 0; }
    
  };
  
  //
  // Operands for instruction class branch
  //
  
  // Operands:
  //   (src: CIA)
  //   (src: CR/p)
  //   (src: CTR?)
  struct bc_branch_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.CIA.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).CIA.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).CR.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).CTR.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).CIA.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).CR.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).CTR.name(),-1);
    }
    
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.CIA.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 2: if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).CIA.name(),-1);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).CR.name(),-1);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).CTR.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).CIA.name(),-1);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).CR.name(),-1);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).CTR.name(),-1);
        break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).CIA.id(static_cast<OSM_Thread&>(uarch),0);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).CIA.last_allocator( static_cast<OSM_Thread&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CIA.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_Thread&>(uarch).CR.id(static_cast<OSM_Thread&>(uarch),0);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).CR.last_allocator( static_cast<OSM_Thread&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_Thread&>(uarch).CTR.id(static_cast<OSM_Thread&>(uarch),0);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).CTR.last_allocator( static_cast<OSM_Thread&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CTR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).CIA.name(static_cast<OSM_Thread&>(uarch),0);
        case 1: return static_cast<OSM_Thread&>(uarch).CR.name(static_cast<OSM_Thread&>(uarch),0);
        case 2: return static_cast<OSM_Thread&>(uarch).CTR.name(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_PMT&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).CIA.id(static_cast<OSM_Thread&>(uarch),0);
        case 1: return static_cast<OSM_Thread&>(uarch).CR.id(static_cast<OSM_Thread&>(uarch),0);
        case 2: return static_cast<OSM_Thread&>(uarch).CTR.id(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
        case 2: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
        case 2: return ic._instr->write_mask(2).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: ic._instr->write_mask(0) = tag;
        break;
        case 1: ic._instr->write_mask(1) = tag;
        break;
        case 2: ic._instr->write_mask(2) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 1;
        case 2: return 2;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_PMT&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 3; };
    
  };
  
  // Operands:
  //   (trg: CTR?)
  //   (trg: NIA?)
  struct bc_branch_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).CTR.id(static_cast<OSM_Thread&>(uarch),0);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).CTR.last_allocator( static_cast<OSM_Thread&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CTR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_Thread&>(uarch).NIA.id(static_cast<OSM_Thread&>(uarch),0);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).NIA.last_allocator( static_cast<OSM_Thread&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.NIA.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.CTR.allocate(uarch,ic,logger);
      uarch.NIA.allocate(uarch,ic,logger);
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.CTR.deallocate(uarch,ic,logger);
      uarch.NIA.deallocate(uarch,ic,logger);
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger && ic._instr->is_dirty(2))logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).CTR.name(),-1);
      if (logger && ic._instr->is_dirty(3))logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).NIA.name(),-1);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(2)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).CTR.name(static_cast<OSM_Thread&>(uarch),0);
        case 1: return static_cast<OSM_Thread&>(uarch).NIA.name(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_PMT&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).CTR.id(static_cast<OSM_Thread&>(uarch),0);
        case 1: return static_cast<OSM_Thread&>(uarch).NIA.id(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(2).uint32();
        case 1: return ic._instr->write_mask(3).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: ic._instr->write_mask(2) = tag;
        break;
        case 1: ic._instr->write_mask(3) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 2;
        case 1: return 3;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_PMT&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  // Operands:
  //   (src: CIA)
  struct b_branch_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.CIA.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).CIA.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).CIA.name(),-1);
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).CIA.id(static_cast<OSM_Thread&>(uarch),0);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).CIA.last_allocator( static_cast<OSM_Thread&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CIA.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_Thread&>(uarch).CIA.name(static_cast<OSM_Thread&>(uarch),0); }
    
    RegId id(UadlArch &uarch,const Inst_PMT &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_Thread&>(uarch).CIA.id(static_cast<OSM_Thread&>(uarch),0); }
    
    bits<32> mask(Inst_PMT&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned = 0) const { return ic._instr->write_mask(0).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned = 0) const { ic._instr->write_mask(0) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 0; }
    
    bool is_op_read(Inst_PMT&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (trg: NIA)
  struct b_branch_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).NIA.id(static_cast<OSM_Thread&>(uarch),0);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).NIA.last_allocator( static_cast<OSM_Thread&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.NIA.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.NIA.allocate(uarch,ic,logger);
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.NIA.deallocate(uarch,ic,logger);
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).NIA.name(),-1);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_Thread&>(uarch).NIA.name(static_cast<OSM_Thread&>(uarch),0); }
    
    RegId id(UadlArch &uarch,const Inst_PMT &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_Thread&>(uarch).NIA.id(static_cast<OSM_Thread&>(uarch),0); }
    
    bits<32> mask(Inst_PMT&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned = 0) const { return ic._instr->write_mask(1).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned = 0) const { ic._instr->write_mask(1) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 1; }
    
    bool is_op_read(Inst_PMT&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operand Alias:  b_branch_Src -> bl_branch_Src
  //   (src: CIA)
  typedef b_branch_Src bl_branch_Src;
  
  // Operands:
  //   (trg: LR)
  //   (trg: NIA)
  struct bl_branch_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.LR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.LR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).LR.id(static_cast<OSM_Thread&>(uarch),0);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).LR.last_allocator( static_cast<OSM_Thread&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.LR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_Thread&>(uarch).NIA.id(static_cast<OSM_Thread&>(uarch),0);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).NIA.last_allocator( static_cast<OSM_Thread&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.NIA.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.LR.allocate(uarch,ic,logger);
      uarch.NIA.allocate(uarch,ic,logger);
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.LR.deallocate(uarch,ic,logger);
      uarch.NIA.deallocate(uarch,ic,logger);
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).LR.name(),-1);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).NIA.name(),-1);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).LR.name(static_cast<OSM_Thread&>(uarch),0);
        case 1: return static_cast<OSM_Thread&>(uarch).NIA.name(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_PMT&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).LR.id(static_cast<OSM_Thread&>(uarch),0);
        case 1: return static_cast<OSM_Thread&>(uarch).NIA.id(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(1).uint32();
        case 1: return ic._instr->write_mask(2).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: ic._instr->write_mask(1) = tag;
        break;
        case 1: ic._instr->write_mask(2) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 1;
        case 1: return 2;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_PMT&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  // Operands:
  //   (src: CR/p)
  //   (src: CTR?)
  //   (src: LR?)
  struct bclr_branch_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.LR.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).CR.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).CTR.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).LR.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).CR.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).CTR.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).LR.name(),-1);
    }
    
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 2: if (!uarch.LR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).CR.name(),-1);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).CTR.name(),-1);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).LR.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).CR.name(),-1);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).CTR.name(),-1);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).LR.name(),-1);
        break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).CR.id(static_cast<OSM_Thread&>(uarch),0);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).CR.last_allocator( static_cast<OSM_Thread&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_Thread&>(uarch).CTR.id(static_cast<OSM_Thread&>(uarch),0);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).CTR.last_allocator( static_cast<OSM_Thread&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CTR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_Thread&>(uarch).LR.id(static_cast<OSM_Thread&>(uarch),0);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).LR.last_allocator( static_cast<OSM_Thread&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.LR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).CR.name(static_cast<OSM_Thread&>(uarch),0);
        case 1: return static_cast<OSM_Thread&>(uarch).CTR.name(static_cast<OSM_Thread&>(uarch),0);
        case 2: return static_cast<OSM_Thread&>(uarch).LR.name(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_PMT&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).CR.id(static_cast<OSM_Thread&>(uarch),0);
        case 1: return static_cast<OSM_Thread&>(uarch).CTR.id(static_cast<OSM_Thread&>(uarch),0);
        case 2: return static_cast<OSM_Thread&>(uarch).LR.id(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
        case 2: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
        case 2: return ic._instr->write_mask(2).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: ic._instr->write_mask(0) = tag;
        break;
        case 1: ic._instr->write_mask(1) = tag;
        break;
        case 2: ic._instr->write_mask(2) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 1;
        case 2: return 2;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_PMT&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 3; };
    
  };
  
  // Operands:
  //   (trg: CTR?)
  //   (trg: NIA?)
  struct bclr_branch_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).CTR.id(static_cast<OSM_Thread&>(uarch),0);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).CTR.last_allocator( static_cast<OSM_Thread&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CTR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_Thread&>(uarch).NIA.id(static_cast<OSM_Thread&>(uarch),0);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).NIA.last_allocator( static_cast<OSM_Thread&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.NIA.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.CTR.allocate(uarch,ic,logger);
      uarch.NIA.allocate(uarch,ic,logger);
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.CTR.deallocate(uarch,ic,logger);
      uarch.NIA.deallocate(uarch,ic,logger);
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger && ic._instr->is_dirty(1))logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).CTR.name(),-1);
      if (logger && ic._instr->is_dirty(3))logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).NIA.name(),-1);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).CTR.name(static_cast<OSM_Thread&>(uarch),0);
        case 1: return static_cast<OSM_Thread&>(uarch).NIA.name(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_PMT&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).CTR.id(static_cast<OSM_Thread&>(uarch),0);
        case 1: return static_cast<OSM_Thread&>(uarch).NIA.id(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(1).uint32();
        case 1: return ic._instr->write_mask(3).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: ic._instr->write_mask(1) = tag;
        break;
        case 1: ic._instr->write_mask(3) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 1;
        case 1: return 3;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_PMT&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  //
  // Instruction class branch
  //
  template <class Src_t, class Trg_t>
  struct Inst_branch : public Inst_PMT {
    
    typedef struct Inst_branch<Src_t, Trg_t> ThisClass;
    
    Inst_branch &thisInstr() { return *this; };
    
    Src_t Src;
    Trg_t Trg;
    void reset(OSM_Thread *uarch,unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_PMT::reset(iid,cid,ic);
      _uarch = uarch;
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    
    void allocate_ops(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger)
    {
      Src.allocate(uarch,ic,logger);
      Trg.allocate(uarch,ic,logger);
    }
    
    void deallocate_ops(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger)
    {
      Src.deallocate(uarch,ic,logger);
      Trg.deallocate(uarch,ic,logger);
    }
    
    void write_ops(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger)
    {
      Src.write(uarch,ic,logger);
      Trg.write(uarch,ic,logger);
    }
    
    bool exec_done() const { return true; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool move_S_EX__S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EX__S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EX__S_MM(uarch,logger,position);
    }
    
    bool change_state_S_EX__S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EX__S_MM_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_EX__S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      return action_S_EXe(uarch,logger,position);
    }
    
    bool move_S_EXe__S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EXe__S_MMe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EXe__S_MMe(uarch,logger,position);
    }
    
    bool change_state_S_EXe__S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EXe__S_MMe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_ID__S_EX(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_IDe"); }
      _action = (StateFunc)action_S_IDe_p;
      _flush = (FlushFunc)flush_S_IDe_p;
      return action_S_IDe(uarch,logger,position);
    }
    
    bool change_state_S_IDe__S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDe -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool move_S_MM__S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MM__S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MM__S_WB(uarch,logger,position);
    }
    
    bool change_state_S_MM__S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MM__S_WB_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_MM__S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      return action_S_MMe(uarch,logger,position);
    }
    
    bool move_S_MMe__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MMe__S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MMe__S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_MMe__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MMe__S_WBe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool move_S_WB__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      if (uarch.is_current_speculative_instr(*this)) {
        if (logger) logger->logMessage("forced flush!");
        uarch.taken_flush_mWB(*this,logger,position,false,0);
      }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WB__S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WB__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WB__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WB__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_WB__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool move_S_WBe__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      if (uarch.is_current_speculative_instr(*this)) {
        if (logger) logger->logMessage("forced flush!");
        uarch.taken_flush_mWB(*this,logger,position,false,0);
      }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WBe__S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WBe__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WBe__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, uarch.mEX.capacity() ) ) {
            return change_state_S_EX__S_MM(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EX_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_EXe__S_MMe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( Src.can_read_fp(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) && Trg.can_write(uarch,*this,logger) && uarch.mEX.has_space(uarch,*this,logger) ) {
            Src.read(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) ;
            allocate_ops(uarch,*this,logger) ;
            uarch.exec_branch(*this,logger) ;
            write_ops(uarch,*this,logger) ;
            if ( ! uarch.last_branch_prediction_correct(*this  ) ) {
                  uarch.taken_flush_mID(*this,logger,position  ) ;
              }
            uarch.BP.update(uarch,*this,logger) ;
            return change_state_S_ID__S_EX(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        flush_S_ID(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mID(*this,logger,position,true,branch_target());
        return change_state_S_ID__S_IDe(uarch,logger,position);
      }
    }
    static bool action_S_ID_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_IDe
    bool action_S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mEX.has_space(uarch,*this,logger) ) {
            return change_state_S_IDe__S_EXe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_IDe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IDe(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.has_space(uarch,*this,logger) ) if ( uarch. OSM_Thread::issueCheck ( * this , position , adliss::PMT::Thread::branch_instrs | adliss::PMT::Thread::load_instrs | adliss::PMT::Thread::store_instrs | adliss::PMT::Thread::special_instrs ) ) {
            return change_state_S_INIT__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_MM
    bool action_S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, uarch.mMM.capacity() ) ) {
            return change_state_S_MM__S_WB(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // State action S_MMe
    bool action_S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_MMe__S_WBe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MMe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MMe(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WB__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WB(uarch,logger,position);
    }
    
    // State action S_WBe
    bool action_S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WBe__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WBe(uarch,logger,position);
    }
    
    // Flush action S_EX
    void flush_S_EX(OSM_Thread &uarch,Logger *logger) {
      deallocate_ops(uarch,*this,logger);
    }
    static void flush_S_EX_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_EXe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_Thread &uarch,Logger *logger) {
      deallocate_ops(uarch,*this,logger);
    }
    static void flush_S_ID_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_IDe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_Thread &uarch,Logger *logger) {
      deallocate_ops(uarch,*this,logger);
    }
    static void flush_S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_MMe
    void flush_S_MMe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_MMe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMe(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_Thread &uarch,Logger *logger) {
      deallocate_ops(uarch,*this,logger);
    }
    static void flush_S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBe(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class deadlock
  //
  
  // Operands:
  //   (src: CR/p)
  struct crxor_deadlock_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).CR.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).CR.name(),-1);
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).CR.id(static_cast<OSM_Thread&>(uarch),0);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).CR.last_allocator( static_cast<OSM_Thread&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_Thread&>(uarch).CR.name(static_cast<OSM_Thread&>(uarch),0); }
    
    RegId id(UadlArch &uarch,const Inst_PMT &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_Thread&>(uarch).CR.id(static_cast<OSM_Thread&>(uarch),0); }
    
    bits<32> mask(Inst_PMT&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned = 0) const { return ic._instr->write_mask(0).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned = 0) const { ic._instr->write_mask(0) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 0; }
    
    bool is_op_read(Inst_PMT&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (trg: CR)
  struct crxor_deadlock_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).CR.id(static_cast<OSM_Thread&>(uarch),0);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).CR.last_allocator( static_cast<OSM_Thread&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.CR.allocate(uarch,ic,logger);
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.CR.deallocate(uarch,ic,logger);
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).CR.name(),-1);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_Thread&>(uarch).CR.name(static_cast<OSM_Thread&>(uarch),0); }
    
    RegId id(UadlArch &uarch,const Inst_PMT &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_Thread&>(uarch).CR.id(static_cast<OSM_Thread&>(uarch),0); }
    
    bits<32> mask(Inst_PMT&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned = 0) const { return ic._instr->write_mask(0).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned = 0) const { ic._instr->write_mask(0) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 0; }
    
    bool is_op_read(Inst_PMT&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  //
  // Instruction class deadlock
  //
  template <class Src_t, class Trg_t>
  struct Inst_deadlock : public Inst_PMT {
    
    typedef struct Inst_deadlock<Src_t, Trg_t> ThisClass;
    
    Inst_deadlock &thisInstr() { return *this; };
    
    Src_t Src;
    Trg_t Trg;
    void reset(OSM_Thread *uarch,unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_PMT::reset(iid,cid,ic);
      _uarch = uarch;
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    
    void deallocate_ops(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger)
    {
      Src.deallocate(uarch,ic,logger);
      Trg.deallocate(uarch,ic,logger);
    }
    
    void write_ops(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger)
    {
      Src.write(uarch,ic,logger);
      Trg.write(uarch,ic,logger);
    }
    
    bool exec_done() const { return true; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool move_S_EX__S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EX__S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EX__S_MM(uarch,logger,position);
    }
    
    bool change_state_S_EX__S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EX__S_MM_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_EX__S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      return action_S_EXe(uarch,logger,position);
    }
    
    bool move_S_EXe__S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EXe__S_MMe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EXe__S_MMe(uarch,logger,position);
    }
    
    bool change_state_S_EXe__S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EXe__S_MMe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_ID__S_EX(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_IDe"); }
      _action = (StateFunc)action_S_IDe_p;
      _flush = (FlushFunc)flush_S_IDe_p;
      return action_S_IDe(uarch,logger,position);
    }
    
    bool change_state_S_IDe__S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDe -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool move_S_MM__S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MM__S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MM__S_WB(uarch,logger,position);
    }
    
    bool change_state_S_MM__S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MM__S_WB_p;
      _action = (StateFunc)no_move_action_S_MM_p;
      return true;
    }
    
    bool change_state_S_MM__S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      return action_S_MMe(uarch,logger,position);
    }
    
    bool move_S_MMe__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MMe__S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MMe__S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_MMe__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MMe__S_WBe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool move_S_WB__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WB__S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WB__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WB__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WB__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_WB__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool move_S_WBe__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WBe__S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WBe__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WBe__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, 2 ) ) {
            uarch.exec(*this,logger) ;
            uarch.FwdEX.write(uarch,*this,logger, Trg ) ;
            return change_state_S_EX__S_MM(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        flush_S_EX(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mEX(*this,logger,position,true,branch_target());
        return change_state_S_EX__S_EXe(uarch,logger,position);
      }
    }
    static bool action_S_EX_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_EXe__S_MMe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( Src.can_read_fp(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) && Trg.can_write(uarch,*this,logger) && uarch.mEX.has_space(uarch,*this,logger) && uarch.all_stall.can_allocate(uarch,*this,logger) && uarch.dead_flag.can_allocate(uarch,*this,logger) ) {
            Src.read(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) ;
            Trg.allocate(uarch,*this,logger) ;
            return change_state_S_ID__S_EX(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        flush_S_ID(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mID(*this,logger,position,true,branch_target());
        return change_state_S_ID__S_IDe(uarch,logger,position);
      }
    }
    static bool action_S_ID_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_IDe
    bool action_S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mEX.has_space(uarch,*this,logger) ) {
            return change_state_S_IDe__S_EXe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_IDe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IDe(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.has_space(uarch,*this,logger) ) if ( uarch. OSM_Thread::issueCheck ( * this , position , adliss::PMT::Thread::muldiv_instrs | adliss::PMT::Thread::branch_instrs ) ) {
            uarch.dead_flag.allocate(uarch,*this,logger) ;
            return change_state_S_INIT__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_MM
    bool action_S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            uarch.FwdMM.write(uarch,*this,logger, Trg ) ;
            return change_state_S_MM__S_WB(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        flush_S_MM(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mMM(*this,logger,position,true,branch_target());
        return change_state_S_MM__S_MMe(uarch,logger,position);
      }
    }
    static bool action_S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // Repeat action S_MM
    bool no_move_action_S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      uarch.FwdMM.write(uarch,*this,logger, Trg ) ;
      return true;
    }
    
    static bool no_move_action_S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->no_move_action_S_MM(uarch,logger,position);
    }
    
    // State action S_MMe
    bool action_S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_MMe__S_WBe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MMe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MMe(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      {
            write_ops(uarch,*this,logger) ;
            return change_state_S_WB__S_INIT(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        flush_S_WB(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mWB(*this,logger,position,true,branch_target());
        return change_state_S_WB__S_WBe(uarch,logger,position);
      }
    }
    static bool action_S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WB(uarch,logger,position);
    }
    
    // State action S_WBe
    bool action_S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WBe__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WBe(uarch,logger,position);
    }
    
    // Flush action S_EX
    void flush_S_EX(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      uarch.dead_flag.deallocate(uarch,*this,logger);
    }
    static void flush_S_EX_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_EXe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      uarch.dead_flag.deallocate(uarch,*this,logger);
    }
    static void flush_S_ID_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_IDe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_Thread &uarch,Logger *logger) {
      uarch.dead_flag.deallocate(uarch,*this,logger);
    }
    static void flush_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      uarch.dead_flag.deallocate(uarch,*this,logger);
    }
    static void flush_S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_MMe
    void flush_S_MMe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_MMe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMe(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      uarch.dead_flag.deallocate(uarch,*this,logger);
    }
    static void flush_S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBe(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class decode_miss
  //
  
  //
  // Instruction class decode_miss
  //
  template <class Src_t, class Trg_t>
  struct Inst_decode_miss : public Inst_PMT {
    
    typedef struct Inst_decode_miss<Src_t, Trg_t> ThisClass;
    
    Inst_decode_miss &thisInstr() { return *this; };
    
    Src_t Src;
    Trg_t Trg;
    void reset(OSM_Thread *uarch,unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_PMT::reset(iid,cid,ic);
      _uarch = uarch;
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    
    bool can_read_ops(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger)
    {
      return (Src.can_read(uarch,ic,logger) && Trg.can_read(uarch,ic,logger));
    }
    
    bool can_write_ops(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger)
    {
      return (Src.can_write(uarch,ic,logger) && Trg.can_write(uarch,ic,logger));
    }
    
    void read_ops(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger)
    {
      Src.read(uarch,ic,logger);
      Trg.read(uarch,ic,logger);
    }
    
    void deallocate_ops(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger)
    {
      Src.deallocate(uarch,ic,logger);
      Trg.deallocate(uarch,ic,logger);
    }
    
    bool exec_done() const { return true; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool move_S_EXe__S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EXe__S_MMe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EXe__S_MMe(uarch,logger,position);
    }
    
    bool change_state_S_EXe__S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EXe__S_MMe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_ID__S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_IDe"); }
      _action = (StateFunc)action_S_IDe_p;
      _flush = (FlushFunc)flush_S_IDe_p;
      return action_S_IDe(uarch,logger,position);
    }
    
    bool change_state_S_IDe__S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDe -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool move_S_MMe__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MMe__S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MMe__S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_MMe__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MMe__S_WBe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool move_S_WBe__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WBe__S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WBe__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WBe__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_EXe__S_MMe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( can_read_ops(uarch,*this,logger) && can_write_ops(uarch,*this,logger) && uarch.mEX.has_space(uarch,*this,logger) ) {
            read_ops(uarch,*this,logger) ;
            uarch.exec(*this,logger) ;
            return change_state_S_ID__S_EXe(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        flush_S_ID(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mID(*this,logger,position,true,branch_target());
        return change_state_S_ID__S_IDe(uarch,logger,position);
      }
    }
    static bool action_S_ID_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_IDe
    bool action_S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mEX.has_space(uarch,*this,logger) ) {
            return change_state_S_IDe__S_EXe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_IDe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IDe(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.has_space(uarch,*this,logger) ) {
            return change_state_S_INIT__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_MMe
    bool action_S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_MMe__S_WBe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MMe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MMe(uarch,logger,position);
    }
    
    // State action S_WBe
    bool action_S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WBe__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WBe(uarch,logger,position);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_EXe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_ID_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_IDe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MMe
    void flush_S_MMe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_MMe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMe(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBe(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class fetch_exception
  //
  
  //
  // Instruction class fetch_exception
  //
  struct Inst_fetch_exception : public Inst_PMT {
    
    typedef struct Inst_fetch_exception ThisClass;
    
    Inst_fetch_exception &thisInstr() { return *this; };
    
    void reset(OSM_Thread *uarch,unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_PMT::reset(iid,cid,ic);
      _uarch = uarch;
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    
    void deallocate_ops(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger)
    {
    }
    
    bool exec_done() const { return true; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool move_S_EX__S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EX__S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EX__S_MM(uarch,logger,position);
    }
    
    bool change_state_S_EX__S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EX__S_MM_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_ID__S_EX(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
      {
        uarch.commit_fetch_exception(*this);
        uarch.taken_flush_mID(*this,logger,position);
      }
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool move_S_MM__S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MM__S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MM__S_WB(uarch,logger,position);
    }
    
    bool change_state_S_MM__S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MM__S_WB_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool move_S_WB__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit_checked(this);
      return true;
    }
    
    static bool move_S_WB__S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WB__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WB__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WB__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_EX__S_MM(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EX_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mEX.has_space(uarch,*this,logger) ) {
            return change_state_S_ID__S_EX(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_ID_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.has_space(uarch,*this,logger) ) {
            return change_state_S_INIT__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_MM
    bool action_S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_MM__S_WB(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WB__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WB(uarch,logger,position);
    }
    
    // Flush action S_EX
    void flush_S_EX(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_EX_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_ID_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class lmw
  //
  
  // Operands:
  //   (src: GPR(RA))
  struct lmw_lmw_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(0)._index); }
    
    RegId id(UadlArch &uarch,const Inst_PMT &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index); }
    
    bits<32> mask(Inst_PMT&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned = 0) const { return ic._instr->write_mask(0).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned = 0) const { ic._instr->write_mask(0) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 0; }
    
    bool is_op_read(Inst_PMT&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (trg: GPR(RT . uint32 (  ),31))
  struct lmw_lmw_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      for (int i = 1; i != 33; ++i) {
        if (ic.op(i).isSrc() && !uarch.GPR.can_alloc(uarch,ic,logger,ic.op(i)._index,"read")) return false;
      }
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        for (int i = 1; i != 33; ++i) {
          if  (ic.op(i).isSrc()) {
            RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(i)._index);
            tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(i)._index));
            if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(i)._index,"read"))) return false;
          }
        }
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      for (int i = 1; i != 33; ++i) {
        if (ic.op(i).isTrg()) {
          uarch.GPR.allocate(uarch,ic,logger,ic.op(i)._index);
        }
      }
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      for (int i = 1; i != 33; ++i) {
        if (ic.op(i).isTrg()) {
          uarch.GPR.deallocate(uarch,ic,logger,ic.op(i)._index);
        }
      }
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      for (int i = 1; i != 33; ++i) {
        if (ic.op(i).isTrg()) {
          if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(i)._index);
        }
      }
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(op_off)._index); }
    
    RegId id(UadlArch &uarch,const Inst_PMT &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(op_off)._index); }
    
    bits<32> mask(Inst_PMT&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned = 0) const { return ic._instr->write_mask(1).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned = 0) const { ic._instr->write_mask(1) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return op_off; }
    
    bool is_op_read(Inst_PMT&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  //
  // Instruction class lmw
  //
  template <class Src_t, class Trg_t>
  struct Inst_lmw : public Inst_PMT {
    
    typedef struct Inst_lmw<Src_t, Trg_t> ThisClass;
    
    Inst_lmw &thisInstr() { return *this; };
    
    Src_t Src;
    Trg_t Trg;
    void reset(OSM_Thread *uarch,unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_PMT::reset(iid,cid,ic);
      _uarch = uarch;
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    
    void deallocate_ops(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger)
    {
      Src.deallocate(uarch,ic,logger);
      Trg.deallocate(uarch,ic,logger);
    }
    
    void write_ops(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger)
    {
      Src.write(uarch,ic,logger);
      Trg.write(uarch,ic,logger);
    }
    
    bool exec_done() const { return mem() == 0; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool move_S_EX__S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EX__S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EX__S_MM(uarch,logger,position);
    }
    
    bool change_state_S_EX__S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EX__S_MM_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_EX__S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      return action_S_EXe(uarch,logger,position);
    }
    
    bool move_S_EXe__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mEX.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_EXe__S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EXe__S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_EXe__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EXe__S_WBe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_ID__S_EX(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_IDe"); }
      _action = (StateFunc)action_S_IDe_p;
      _flush = (FlushFunc)flush_S_IDe_p;
      return action_S_IDe(uarch,logger,position);
    }
    
    bool change_state_S_IDe__S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDe -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool change_state_S_MM__S_MMp(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMp"); }
      _action = (StateFunc)action_S_MMp_p;
      _flush = (FlushFunc)flush_S_MMp_p;
      return action_S_MMp(uarch,logger,position);
    }
    
    bool change_state_S_MMp__S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMp -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      return action_S_MM(uarch,logger,position);
    }
    
    bool move_S_MMp__S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMp -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MMp__S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MMp__S_WB(uarch,logger,position);
    }
    
    bool change_state_S_MMp__S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MMp__S_WB_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool move_S_WB__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WB__S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WB__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WB__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WB__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_WB__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool move_S_WBe__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WBe__S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WBe__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WBe__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger) && uarch.L1d.can_request_read(uarch,*this,logger) && ! uarch.stalled() ) {
            {uarch.exec_mem(*this,logger) ; if (has_exceptions()) goto HandleException; } ;
            uarch.L1d.send_read_request(uarch,*this,logger, 8 ) ;
            next_req_mem (  ) ;
            return change_state_S_EX__S_MM(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_EX(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mEX(*this,logger,position,true,branch_target());
        return change_state_S_EX__S_EXe(uarch,logger,position);
      }
    }
    static bool action_S_EX_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_EXe__S_WBe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mEX.has_space(uarch,*this,logger) ) if ( Src.can_read_fp(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) && Trg.can_write(uarch,*this,logger) && uarch.all_stall.can_allocate(uarch,*this,logger) && ! uarch.stalled() ) {
            Src.read(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) ;
            Trg.allocate(uarch,*this,logger) ;
            return change_state_S_ID__S_EX(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_ID(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mID(*this,logger,position,true,branch_target());
        return change_state_S_ID__S_IDe(uarch,logger,position);
      }
    }
    static bool action_S_ID_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_IDe
    bool action_S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mEX.has_space(uarch,*this,logger) ) {
            return change_state_S_IDe__S_EXe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_IDe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IDe(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.has_space(uarch,*this,logger) ) if ( uarch. OSM_Thread::loadStoreIssueCheck ( * this , position , adliss::PMT::Thread::special_instrs | adliss::PMT::Thread::store_instrs | adliss::PMT::Thread::load_instrs | adliss::PMT::Thread::muldiv_instrs | adliss::PMT::Thread::branch_instrs ) ) {
            return change_state_S_INIT__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_MM
    bool action_S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.L1d.can_read(uarch,*this,logger) ) {
            uarch.exec_and_read(*this,logger, uarch.L1d , 2 ) ;
            return change_state_S_MM__S_MMp(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // State action S_MMp
    bool action_S_MMp(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( exec_done (  ) && uarch.mWB.has_space(uarch,*this,logger) ) {
            return change_state_S_MMp__S_WB(uarch,logger,position) ;
      } else if ( uarch.L1d.can_request_read(uarch,*this,logger) ) {
            uarch.L1d.send_read_request(uarch,*this,logger, 8 ) ;
            next_req_mem (  ) ;
            return change_state_S_MMp__S_MM(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MMp_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MMp(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      {
            write_ops(uarch,*this,logger) ;
            return change_state_S_WB__S_INIT(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_WB(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mWB(*this,logger,position,true,branch_target());
        return change_state_S_WB__S_WBe(uarch,logger,position);
      }
    }
    static bool action_S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WB(uarch,logger,position);
    }
    
    // State action S_WBe
    bool action_S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WBe__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WBe(uarch,logger,position);
    }
    
    // Flush action S_EX
    void flush_S_EX(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_EX_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_EXe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_ID_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_IDe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_MMp
    void flush_S_MMp(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_MMp_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMp(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBe(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class load
  //
  
  // Operands:
  //   (src: GPR(RA))
  struct lbz_load_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(0)._index); }
    
    RegId id(UadlArch &uarch,const Inst_PMT &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index); }
    
    bits<32> mask(Inst_PMT&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned = 0) const { return ic._instr->write_mask(0).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned = 0) const { ic._instr->write_mask(0) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 0; }
    
    bool is_op_read(Inst_PMT&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (trg: GPR(RT))
  struct lbz_load_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(1)._index);
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(1)._index);
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(1)._index); }
    
    RegId id(UadlArch &uarch,const Inst_PMT &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index); }
    
    bits<32> mask(Inst_PMT&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned = 0) const { return ic._instr->write_mask(1).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned = 0) const { ic._instr->write_mask(1) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 1; }
    
    bool is_op_read(Inst_PMT&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operand Alias:  lbz_load_Src -> lhz_load_Src
  //   (src: GPR(RA))
  typedef lbz_load_Src lhz_load_Src;
  
  // Operand Alias:  lbz_load_Trg -> lhz_load_Trg
  //   (trg: GPR(RT))
  typedef lbz_load_Trg lhz_load_Trg;
  
  // Operand Alias:  lbz_load_Src -> lwz_load_Src
  //   (src: GPR(RA))
  typedef lbz_load_Src lwz_load_Src;
  
  // Operand Alias:  lbz_load_Trg -> lwz_load_Trg
  //   (trg: GPR(RT))
  typedef lbz_load_Trg lwz_load_Trg;
  
  // Operands:
  //   (src: GPR(RA . uint32 (  )))
  struct lwzu_load_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(0)._index); }
    
    RegId id(UadlArch &uarch,const Inst_PMT &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index); }
    
    bits<32> mask(Inst_PMT&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned = 0) const { return ic._instr->write_mask(0).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned = 0) const { ic._instr->write_mask(0) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 0; }
    
    bool is_op_read(Inst_PMT&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (trg: GPR(RA . uint32 (  )))
  //   (trg: GPR(RT))
  struct lwzu_load_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(0)._index);
      uarch.GPR.allocate(uarch,ic,logger,ic.op(1)._index);
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(0)._index);
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(1)._index);
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_PMT&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: ic._instr->write_mask(0) = tag;
        break;
        case 1: ic._instr->write_mask(1) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 1;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_PMT&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  // Operands:
  //   (trg: GPR(RA . uint32 (  )))
  struct lwzu_load_TrgEA {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(0)._index);
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(0)._index);
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(0)._index); }
    
    RegId id(UadlArch &uarch,const Inst_PMT &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index); }
    
    bits<32> mask(Inst_PMT&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned = 0) const { return ic._instr->write_mask(0).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned = 0) const { ic._instr->write_mask(0) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 0; }
    
    bool is_op_read(Inst_PMT&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  struct lwzx_load_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
    }
    
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
        break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_PMT&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: ic._instr->write_mask(0) = tag;
        break;
        case 1: ic._instr->write_mask(1) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 1;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_PMT&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  // Operands:
  //   (trg: GPR(RT))
  struct lwzx_load_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(2)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(2)._index);
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(2)._index);
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(2)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(2)._index); }
    
    RegId id(UadlArch &uarch,const Inst_PMT &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(2)._index); }
    
    bits<32> mask(Inst_PMT&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned = 0) const { return ic._instr->write_mask(2).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned = 0) const { ic._instr->write_mask(2) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 2; }
    
    bool is_op_read(Inst_PMT&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  //
  // Instruction class load
  //
  template <class Src_t, class Trg_t, class TrgEA_t>
  struct Inst_load : public Inst_PMT {
    
    typedef struct Inst_load<Src_t, Trg_t, TrgEA_t> ThisClass;
    
    Inst_load &thisInstr() { return *this; };
    
    Src_t Src;
    Trg_t Trg;
    TrgEA_t TrgEA;
    void reset(OSM_Thread *uarch,unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_PMT::reset(iid,cid,ic);
      _uarch = uarch;
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    
    void deallocate_ops(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger)
    {
      Src.deallocate(uarch,ic,logger);
      Trg.deallocate(uarch,ic,logger);
      TrgEA.deallocate(uarch,ic,logger);
    }
    
    void write_ops(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger)
    {
      Src.write(uarch,ic,logger);
      Trg.write(uarch,ic,logger);
      TrgEA.write(uarch,ic,logger);
    }
    
    bool exec_done() const { return mem() == 0; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool move_S_EX__S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EX__S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EX__S_MM(uarch,logger,position);
    }
    
    bool change_state_S_EX__S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EX__S_MM_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_EX__S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      return action_S_EXe(uarch,logger,position);
    }
    
    bool move_S_EXe__S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EXe__S_MMe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EXe__S_MMe(uarch,logger,position);
    }
    
    bool change_state_S_EXe__S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EXe__S_MMe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_ID__S_EX(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_IDe"); }
      _action = (StateFunc)action_S_IDe_p;
      _flush = (FlushFunc)flush_S_IDe_p;
      return action_S_IDe(uarch,logger,position);
    }
    
    bool change_state_S_IDe__S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDe -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool change_state_S_MM__S_MMp(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMp"); }
      _action = (StateFunc)action_S_MMp_p;
      _flush = (FlushFunc)flush_S_MMp_p;
      return action_S_MMp(uarch,logger,position);
    }
    
    bool change_state_S_MM__S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      return action_S_MMe(uarch,logger,position);
    }
    
    bool move_S_MMe__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MMe__S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MMe__S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_MMe__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MMe__S_WBe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_MMp__S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMp -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      return action_S_MM(uarch,logger,position);
    }
    
    bool move_S_MMp__S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMp -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MMp__S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MMp__S_WB(uarch,logger,position);
    }
    
    bool change_state_S_MMp__S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MMp__S_WB_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_WB__S_WBp(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBp"); }
      _action = (StateFunc)action_S_WBp_p;
      _flush = (FlushFunc)flush_S_WBp_p;
      return action_S_WBp(uarch,logger,position);
    }
    
    bool change_state_S_WB__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool move_S_WBe__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WBe__S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WBe__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WBe__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool move_S_WBp__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBp -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WBp__S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WBp__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WBp__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WBp__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger) && uarch.L1d.can_request_read(uarch,*this,logger) && ! uarch.stalled() ) {
            {uarch.exec_mem(*this,logger) ; if (has_exceptions()) goto HandleException; } ;
            uarch.FwdEX.write(uarch,*this,logger, TrgEA ) ;
            uarch.L1d.send_read_request(uarch,*this,logger) ;
            return change_state_S_EX__S_MM(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_EX(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mEX(*this,logger,position,true,branch_target());
        return change_state_S_EX__S_EXe(uarch,logger,position);
      }
    }
    static bool action_S_EX_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_EXe__S_MMe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( Src.can_read_fp(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) && Trg.can_write(uarch,*this,logger) && uarch.mEX.has_space(uarch,*this,logger) && uarch.hid_stall.can_allocate(uarch,*this,logger) && ! uarch.stalled() ) {
            Src.read(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) ;
            Trg.allocate(uarch,*this,logger) ;
            return change_state_S_ID__S_EX(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_ID(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mID(*this,logger,position,true,branch_target());
        return change_state_S_ID__S_IDe(uarch,logger,position);
      }
    }
    static bool action_S_ID_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_IDe
    bool action_S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mEX.has_space(uarch,*this,logger) ) {
            return change_state_S_IDe__S_EXe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_IDe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IDe(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.has_space(uarch,*this,logger) ) if ( uarch. OSM_Thread::loadStoreIssueCheck ( * this , position , adliss::PMT::Thread::special_instrs | adliss::PMT::Thread::store_instrs | adliss::PMT::Thread::load_instrs | adliss::PMT::Thread::muldiv_instrs | adliss::PMT::Thread::branch_instrs ) ) {
            return change_state_S_INIT__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_MM
    bool action_S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.L1d.can_read(uarch,*this,logger) ) {
            { uarch.exec_and_read(*this,logger, uarch.L1d )  ; if (has_exceptions()) goto HandleException; } ;
            uarch.FwdMM.write(uarch,*this,logger, Trg ) ;
            return change_state_S_MM__S_MMp(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_MM(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mMM(*this,logger,position,true,branch_target());
        return change_state_S_MM__S_MMe(uarch,logger,position);
      }
    }
    static bool action_S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // State action S_MMe
    bool action_S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_MMe__S_WBe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MMe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MMe(uarch,logger,position);
    }
    
    // State action S_MMp
    bool action_S_MMp(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( exec_done (  ) && uarch.mWB.has_space(uarch,*this,logger, uarch.mMM.capacity() ) ) {
            return change_state_S_MMp__S_WB(uarch,logger,position) ;
      } else if ( uarch.L1d.can_request_read(uarch,*this,logger) ) {
            uarch.L1d.send_read_request(uarch,*this,logger) ;
            return change_state_S_MMp__S_MM(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MMp_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MMp(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      {
            write_ops(uarch,*this,logger) ;
            return change_state_S_WB__S_WBp(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_WB(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mWB(*this,logger,position,true,branch_target());
        return change_state_S_WB__S_WBe(uarch,logger,position);
      }
    }
    static bool action_S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WB(uarch,logger,position);
    }
    
    // State action S_WBe
    bool action_S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WBe__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WBe(uarch,logger,position);
    }
    
    // State action S_WBp
    bool action_S_WBp(OSM_Thread &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WBp__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WBp_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WBp(uarch,logger,position);
    }
    
    // Flush action S_EX
    void flush_S_EX(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_EX_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_EXe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_ID_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_IDe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_MMe
    void flush_S_MMe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_MMe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMe(uarch,logger);
    }
    
    // Flush action S_MMp
    void flush_S_MMp(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_MMp_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMp(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBe(uarch,logger);
    }
    
    // Flush action S_WBp
    void flush_S_WBp(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_WBp_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBp(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class mtmsr
  //
  
  // Operands:
  //   (src: GPR(RS))
  struct mtmsr_mtmsr_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(0)._index); }
    
    RegId id(UadlArch &uarch,const Inst_PMT &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index); }
    
    bits<32> mask(Inst_PMT&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned = 0) const { return ic._instr->write_mask(0).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned = 0) const { ic._instr->write_mask(0) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 0; }
    
    bool is_op_read(Inst_PMT&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (trg: MSR)
  struct mtmsr_mtmsr_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.MSR.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).MSR.id(static_cast<OSM_Thread&>(uarch),0);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).MSR.last_allocator( static_cast<OSM_Thread&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.MSR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.MSR.allocate(uarch,ic,logger);
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.MSR.deallocate(uarch,ic,logger);
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).MSR.name(),-1);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_Thread&>(uarch).MSR.name(static_cast<OSM_Thread&>(uarch),0); }
    
    RegId id(UadlArch &uarch,const Inst_PMT &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_Thread&>(uarch).MSR.id(static_cast<OSM_Thread&>(uarch),0); }
    
    bits<32> mask(Inst_PMT&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned = 0) const { return ic._instr->write_mask(1).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned = 0) const { ic._instr->write_mask(1) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 1; }
    
    bool is_op_read(Inst_PMT&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  //
  // Instruction class mtmsr
  //
  template <class Src_t, class Trg_t>
  struct Inst_mtmsr : public Inst_PMT {
    
    typedef struct Inst_mtmsr<Src_t, Trg_t> ThisClass;
    
    Inst_mtmsr &thisInstr() { return *this; };
    
    Src_t Src;
    Trg_t Trg;
    void reset(OSM_Thread *uarch,unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_PMT::reset(iid,cid,ic);
      _uarch = uarch;
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    
    void deallocate_ops(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger)
    {
      Src.deallocate(uarch,ic,logger);
      Trg.deallocate(uarch,ic,logger);
    }
    
    void write_ops(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger)
    {
      Src.write(uarch,ic,logger);
      Trg.write(uarch,ic,logger);
    }
    
    bool exec_done() const { return true; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool move_S_EX__S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EX__S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EX__S_MM(uarch,logger,position);
    }
    
    bool change_state_S_EX__S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EX__S_MM_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_EX__S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      return action_S_EXe(uarch,logger,position);
    }
    
    bool move_S_EXe__S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EXe__S_MMe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EXe__S_MMe(uarch,logger,position);
    }
    
    bool change_state_S_EXe__S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EXe__S_MMe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_ID__S_EX(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_IDe"); }
      _action = (StateFunc)action_S_IDe_p;
      _flush = (FlushFunc)flush_S_IDe_p;
      return action_S_IDe(uarch,logger,position);
    }
    
    bool change_state_S_IDe__S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDe -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool move_S_MM__S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MM__S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MM__S_WB(uarch,logger,position);
    }
    
    bool change_state_S_MM__S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MM__S_WB_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_MM__S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      return action_S_MMe(uarch,logger,position);
    }
    
    bool move_S_MMe__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MMe__S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MMe__S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_MMe__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MMe__S_WBe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_WB__S_WBp(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBp"); }
      _action = (StateFunc)action_S_WBp_p;
      _flush = (FlushFunc)flush_S_WBp_p;
      return action_S_WBp(uarch,logger,position);
    }
    
    bool change_state_S_WB__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool move_S_WBe__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WBe__S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WBe__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WBe__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool move_S_WBp__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBp -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WBp__S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WBp__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WBp__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WBp__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, 2 ) ) {
            uarch.exec(*this,logger) ;
            return change_state_S_EX__S_MM(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        flush_S_EX(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mEX(*this,logger,position,true,branch_target());
        return change_state_S_EX__S_EXe(uarch,logger,position);
      }
    }
    static bool action_S_EX_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_EXe__S_MMe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      {
            if ( Src.can_read_fp(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) && uarch.all_stall.can_allocate(uarch,*this,logger) && uarch.hid_stall.can_allocate(uarch,*this,logger) && uarch.mEX.empty(uarch,*this,logger) && uarch.mMM.empty(uarch,*this,logger) && uarch.mWB.empty(uarch,*this,logger) ) {
                  Src.read(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) ;
                  Trg.allocate(uarch,*this,logger) ;
                  uarch.all_stall.allocate(uarch,*this,logger) ;
                  uarch.hid_stall.allocate(uarch,*this,logger) ;
                  return change_state_S_ID__S_EX(uarch,logger,position) ;
              }
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        flush_S_ID(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mID(*this,logger,position,true,branch_target());
        return change_state_S_ID__S_IDe(uarch,logger,position);
      }
    }
    static bool action_S_ID_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_IDe
    bool action_S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mEX.has_space(uarch,*this,logger) ) {
            return change_state_S_IDe__S_EXe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_IDe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IDe(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.empty(uarch,*this,logger) ) {
            return change_state_S_INIT__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_MM
    bool action_S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_MM__S_WB(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // State action S_MMe
    bool action_S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_MMe__S_WBe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MMe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MMe(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      {
            write_ops(uarch,*this,logger) ;
            return change_state_S_WB__S_WBp(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        flush_S_WB(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mWB(*this,logger,position,true,branch_target());
        return change_state_S_WB__S_WBe(uarch,logger,position);
      }
    }
    static bool action_S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WB(uarch,logger,position);
    }
    
    // State action S_WBe
    bool action_S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WBe__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WBe(uarch,logger,position);
    }
    
    // State action S_WBp
    bool action_S_WBp(OSM_Thread &uarch,Logger *logger,unsigned position) {
      {
            uarch.all_stall.deallocate(uarch,*this,logger) ;
            uarch.hid_stall.deallocate(uarch,*this,logger, uarch.mWB.capacity() ) ;
            return change_state_S_WBp__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WBp_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WBp(uarch,logger,position);
    }
    
    // Flush action S_EX
    void flush_S_EX(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      uarch.all_stall.deallocate(uarch,*this,logger);
      uarch.hid_stall.deallocate(uarch,*this,logger);
    }
    static void flush_S_EX_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_EXe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      uarch.all_stall.deallocate(uarch,*this,logger);
      uarch.hid_stall.deallocate(uarch,*this,logger);
    }
    static void flush_S_ID_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_IDe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      uarch.all_stall.deallocate(uarch,*this,logger);
      uarch.hid_stall.deallocate(uarch,*this,logger);
    }
    static void flush_S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_MMe
    void flush_S_MMe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_MMe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMe(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      uarch.all_stall.deallocate(uarch,*this,logger);
      uarch.hid_stall.deallocate(uarch,*this,logger);
    }
    static void flush_S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBe(uarch,logger);
    }
    
    // Flush action S_WBp
    void flush_S_WBp(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      uarch.all_stall.deallocate(uarch,*this,logger);
      uarch.hid_stall.deallocate(uarch,*this,logger);
    }
    static void flush_S_WBp_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBp(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class mtspr_hid
  //
  
  // Operands:
  //   (src: GPR(RS))
  struct mtspr_mtspr_hid_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(0)._index); }
    
    RegId id(UadlArch &uarch,const Inst_PMT &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index); }
    
    bits<32> mask(Inst_PMT&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned = 0) const { return ic._instr->write_mask(0).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned = 0) const { ic._instr->write_mask(0) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 0; }
    
    bool is_op_read(Inst_PMT&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (trg: SPR(SPRN))
  struct mtspr_mtspr_hid_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.SPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).SPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).SPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.SPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.SPR.allocate(uarch,ic,logger,ic.op(1)._index);
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.SPR.deallocate(uarch,ic,logger,ic.op(1)._index);
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).SPR.name(),ic.op(1)._index);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_Thread&>(uarch).SPR.name(static_cast<OSM_Thread&>(uarch),ic.op(1)._index); }
    
    RegId id(UadlArch &uarch,const Inst_PMT &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_Thread&>(uarch).SPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index); }
    
    bits<32> mask(Inst_PMT&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned = 0) const { return ic._instr->write_mask(1).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned = 0) const { ic._instr->write_mask(1) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 1; }
    
    bool is_op_read(Inst_PMT&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  //
  // Instruction class mtspr_hid
  //
  template <class Src_t, class Trg_t>
  struct Inst_mtspr_hid : public Inst_PMT {
    
    typedef struct Inst_mtspr_hid<Src_t, Trg_t> ThisClass;
    
    Inst_mtspr_hid &thisInstr() { return *this; };
    
    Src_t Src;
    Trg_t Trg;
    void reset(OSM_Thread *uarch,unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_PMT::reset(iid,cid,ic);
      _uarch = uarch;
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    
    void deallocate_ops(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger)
    {
      Src.deallocate(uarch,ic,logger);
      Trg.deallocate(uarch,ic,logger);
    }
    
    void write_ops(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger)
    {
      Src.write(uarch,ic,logger);
      Trg.write(uarch,ic,logger);
    }
    
    bool exec_done() const { return true; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool change_state_S_EX__S_EXp(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_EXp"); }
      _action = (StateFunc)action_S_EXp_p;
      _flush = (FlushFunc)flush_S_EXp_p;
      return action_S_EXp(uarch,logger,position);
    }
    
    bool change_state_S_EX__S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      return action_S_EXe(uarch,logger,position);
    }
    
    bool move_S_EXe__S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EXe__S_MMe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EXe__S_MMe(uarch,logger,position);
    }
    
    bool change_state_S_EXe__S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EXe__S_MMe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool move_S_EXp__S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXp -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EXp__S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EXp__S_MM(uarch,logger,position);
    }
    
    bool change_state_S_EXp__S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EXp__S_MM_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_ID__S_EX(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_IDe"); }
      _action = (StateFunc)action_S_IDe_p;
      _flush = (FlushFunc)flush_S_IDe_p;
      return action_S_IDe(uarch,logger,position);
    }
    
    bool change_state_S_IDe__S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDe -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool move_S_MM__S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MM__S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MM__S_WB(uarch,logger,position);
    }
    
    bool change_state_S_MM__S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MM__S_WB_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_MM__S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      return action_S_MMe(uarch,logger,position);
    }
    
    bool move_S_MMe__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MMe__S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MMe__S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_MMe__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MMe__S_WBe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_WB__S_WBp(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBp"); }
      _action = (StateFunc)action_S_WBp_p;
      _flush = (FlushFunc)flush_S_WBp_p;
      return action_S_WBp(uarch,logger,position);
    }
    
    bool change_state_S_WB__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool move_S_WBe__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WBe__S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WBe__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WBe__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool move_S_WBp__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBp -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WBp__S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WBp__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WBp__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WBp__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.L1d.can_force_lock(uarch,*this,logger) ) {
            uarch.L1d.allocate_force_lock(uarch,*this,logger) ;
            uarch.Fetcher.set_cancel(uarch,*this,logger, true ) ;
            uarch.Fetcher.cancel(uarch,*this,logger) ;
            uarch.exec(*this,logger) ;
            set_latency ( uarch.mEX.capacity() ) ;
            return change_state_S_EX__S_EXp(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        flush_S_EX(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mEX(*this,logger,position,true,branch_target());
        return change_state_S_EX__S_EXe(uarch,logger,position);
      }
    }
    static bool action_S_EX_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_EXe__S_MMe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_EXp
    bool action_S_EXp(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( check_latency(logger) && uarch.mMM.has_space(uarch,*this,logger, uarch.mEX.capacity() ) ) {
            return change_state_S_EXp__S_MM(uarch,logger,position) ;
      } else {
            decr_latency (  ) ;
      }
      return false;
    }
    static bool action_S_EXp_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXp(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      {
            if ( Src.can_read_fp(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) && Trg.can_write(uarch,*this,logger) && uarch.mEX.has_space(uarch,*this,logger) ) {
                  Src.read(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) ;
                  Trg.allocate(uarch,*this,logger) ;
                  uarch.hid_stall.allocate(uarch,*this,logger) ;
                  return change_state_S_ID__S_EX(uarch,logger,position) ;
              }
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        flush_S_ID(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mID(*this,logger,position,true,branch_target());
        return change_state_S_ID__S_IDe(uarch,logger,position);
      }
    }
    static bool action_S_ID_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_IDe
    bool action_S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mEX.has_space(uarch,*this,logger) ) {
            return change_state_S_IDe__S_EXe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_IDe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IDe(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.empty(uarch,*this,logger) ) {
            return change_state_S_INIT__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_MM
    bool action_S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_MM__S_WB(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // State action S_MMe
    bool action_S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_MMe__S_WBe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MMe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MMe(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      {
            write_ops(uarch,*this,logger) ;
            return change_state_S_WB__S_WBp(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        flush_S_WB(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mWB(*this,logger,position,true,branch_target());
        return change_state_S_WB__S_WBe(uarch,logger,position);
      }
    }
    static bool action_S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WB(uarch,logger,position);
    }
    
    // State action S_WBe
    bool action_S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WBe__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WBe(uarch,logger,position);
    }
    
    // State action S_WBp
    bool action_S_WBp(OSM_Thread &uarch,Logger *logger,unsigned position) {
      {
            uarch.L1d.deallocate_force_lock(uarch,*this,logger) ;
            uarch.hid_stall.deallocate(uarch,*this,logger, uarch.mWB.capacity() ) ;
            return change_state_S_WBp__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WBp_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WBp(uarch,logger,position);
    }
    
    // Flush action S_EX
    void flush_S_EX(OSM_Thread &uarch,Logger *logger) {
      uarch.L1d.deallocate_force_lock(uarch,*this,logger);
      Trg.deallocate(uarch,*this,logger);
      uarch.hid_stall.deallocate(uarch,*this,logger);
    }
    static void flush_S_EX_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_EXe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_EXp
    void flush_S_EXp(OSM_Thread &uarch,Logger *logger) {
      uarch.L1d.deallocate_force_lock(uarch,*this,logger);
      Trg.deallocate(uarch,*this,logger);
      uarch.hid_stall.deallocate(uarch,*this,logger);
    }
    static void flush_S_EXp_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXp(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      uarch.hid_stall.deallocate(uarch,*this,logger);
    }
    static void flush_S_ID_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_IDe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_Thread &uarch,Logger *logger) {
      uarch.L1d.deallocate_force_lock(uarch,*this,logger);
      Trg.deallocate(uarch,*this,logger);
      uarch.hid_stall.deallocate(uarch,*this,logger);
    }
    static void flush_S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_MMe
    void flush_S_MMe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_MMe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMe(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_Thread &uarch,Logger *logger) {
      uarch.L1d.deallocate_force_lock(uarch,*this,logger);
      Trg.deallocate(uarch,*this,logger);
      uarch.hid_stall.deallocate(uarch,*this,logger);
    }
    static void flush_S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBe(uarch,logger);
    }
    
    // Flush action S_WBp
    void flush_S_WBp(OSM_Thread &uarch,Logger *logger) {
      uarch.L1d.deallocate_force_lock(uarch,*this,logger);
      Trg.deallocate(uarch,*this,logger);
      uarch.hid_stall.deallocate(uarch,*this,logger);
    }
    static void flush_S_WBp_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBp(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class muldiv
  //
  
  // Operands:
  //   (src: GPR(RA))
  struct mulli_muldiv_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(0)._index); }
    
    RegId id(UadlArch &uarch,const Inst_PMT &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index); }
    
    bits<32> mask(Inst_PMT&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned = 0) const { return ic._instr->write_mask(0).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned = 0) const { ic._instr->write_mask(0) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 0; }
    
    bool is_op_read(Inst_PMT&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (trg: GPR(RT))
  struct mulli_muldiv_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(1)._index);
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(1)._index);
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(1)._index); }
    
    RegId id(UadlArch &uarch,const Inst_PMT &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index); }
    
    bits<32> mask(Inst_PMT&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned = 0) const { return ic._instr->write_mask(1).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned = 0) const { ic._instr->write_mask(1) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 1; }
    
    bool is_op_read(Inst_PMT&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  struct mullw_muldiv_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
    }
    
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
        break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_PMT&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: ic._instr->write_mask(0) = tag;
        break;
        case 1: ic._instr->write_mask(1) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 1;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_PMT&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  // Operands:
  //   (trg: GPR(RT))
  struct mullw_muldiv_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(2)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(2)._index);
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(2)._index);
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(2)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(2)._index); }
    
    RegId id(UadlArch &uarch,const Inst_PMT &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(2)._index); }
    
    bits<32> mask(Inst_PMT&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned = 0) const { return ic._instr->write_mask(2).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned = 0) const { ic._instr->write_mask(2) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 2; }
    
    bool is_op_read(Inst_PMT&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  //
  // Instruction class muldiv
  //
  template <class Src_t, class Trg_t>
  struct Inst_muldiv : public Inst_PMT {
    
    typedef struct Inst_muldiv<Src_t, Trg_t> ThisClass;
    
    Inst_muldiv &thisInstr() { return *this; };
    
    Src_t Src;
    Trg_t Trg;
    void reset(OSM_Thread *uarch,unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_PMT::reset(iid,cid,ic);
      _uarch = uarch;
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    
    void deallocate_ops(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger)
    {
      Src.deallocate(uarch,ic,logger);
      Trg.deallocate(uarch,ic,logger);
    }
    
    void write_ops(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger)
    {
      Src.write(uarch,ic,logger);
      Trg.write(uarch,ic,logger);
    }
    
    bool exec_done() const { return true; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool move_S_EX__S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EX__S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EX__S_MM(uarch,logger,position);
    }
    
    bool change_state_S_EX__S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EX__S_MM_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_EX__S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      return action_S_EXe(uarch,logger,position);
    }
    
    bool move_S_EXe__S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EXe__S_MMe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EXe__S_MMe(uarch,logger,position);
    }
    
    bool change_state_S_EXe__S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EXe__S_MMe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_ID__S_EX(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_IDe"); }
      _action = (StateFunc)action_S_IDe_p;
      _flush = (FlushFunc)flush_S_IDe_p;
      return action_S_IDe(uarch,logger,position);
    }
    
    bool change_state_S_IDe__S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDe -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool move_S_MM__S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MM__S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MM__S_WB(uarch,logger,position);
    }
    
    bool change_state_S_MM__S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MM__S_WB_p;
      _action = (StateFunc)no_move_action_S_MM_p;
      return true;
    }
    
    bool change_state_S_MM__S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      return action_S_MMe(uarch,logger,position);
    }
    
    bool move_S_MMe__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MMe__S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MMe__S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_MMe__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MMe__S_WBe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool move_S_WB__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WB__S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WB__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WB__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WB__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_WB__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool move_S_WBe__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WBe__S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WBe__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WBe__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, 2 ) ) {
            uarch.exec(*this,logger) ;
            set_latency ( 4 ) ;
            return change_state_S_EX__S_MM(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        flush_S_EX(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mEX(*this,logger,position,true,branch_target());
        return change_state_S_EX__S_EXe(uarch,logger,position);
      }
    }
    static bool action_S_EX_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_EXe__S_MMe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( Src.can_read_fp(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) && Trg.can_write(uarch,*this,logger) && uarch.mEX.has_space(uarch,*this,logger) && uarch.all_stall.can_allocate(uarch,*this,logger) ) {
            Src.read(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) ;
            Trg.allocate(uarch,*this,logger) ;
            return change_state_S_ID__S_EX(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        flush_S_ID(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mID(*this,logger,position,true,branch_target());
        return change_state_S_ID__S_IDe(uarch,logger,position);
      }
    }
    static bool action_S_ID_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_IDe
    bool action_S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mEX.has_space(uarch,*this,logger) ) {
            return change_state_S_IDe__S_EXe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_IDe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IDe(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.has_space(uarch,*this,logger) ) if ( uarch. OSM_Thread::issueCheck ( * this , position , adliss::PMT::Thread::muldiv_instrs | adliss::PMT::Thread::branch_instrs ) ) {
            return change_state_S_INIT__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_MM
    bool action_S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( check_latency(logger) && uarch.mWB.has_space(uarch,*this,logger) ) {
            uarch.FwdMM.write(uarch,*this,logger, Trg ) ;
            return change_state_S_MM__S_WB(uarch,logger,position) ;
      } else {
            decr_latency (  ) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        flush_S_MM(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mMM(*this,logger,position,true,branch_target());
        return change_state_S_MM__S_MMe(uarch,logger,position);
      }
    }
    static bool action_S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // Repeat action S_MM
    bool no_move_action_S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      uarch.FwdMM.write(uarch,*this,logger, Trg ) ;
      return true;
    }
    
    static bool no_move_action_S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->no_move_action_S_MM(uarch,logger,position);
    }
    
    // State action S_MMe
    bool action_S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_MMe__S_WBe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MMe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MMe(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      {
            write_ops(uarch,*this,logger) ;
            return change_state_S_WB__S_INIT(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        flush_S_WB(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mWB(*this,logger,position,true,branch_target());
        return change_state_S_WB__S_WBe(uarch,logger,position);
      }
    }
    static bool action_S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WB(uarch,logger,position);
    }
    
    // State action S_WBe
    bool action_S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WBe__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WBe(uarch,logger,position);
    }
    
    // Flush action S_EX
    void flush_S_EX(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_EX_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_EXe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_ID_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_IDe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_MMe
    void flush_S_MMe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_MMe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMe(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBe(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class nop
  //
  
  // Operands:
  //   (trg: GPR(RA))
  //   (src: GPR(RS))
  struct ori_nop___other__ {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
    }
    
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
        break;
      }
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(0)._index);
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(0)._index);
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_PMT&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: ic._instr->write_mask(0) = tag;
        break;
        case 1: ic._instr->write_mask(1) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 1;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_PMT&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  //
  // Instruction class nop
  //
  template <class __other___t>
  struct Inst_nop : public Inst_PMT {
    
    typedef struct Inst_nop<__other___t> ThisClass;
    
    Inst_nop &thisInstr() { return *this; };
    
    __other___t __other__;
    void reset(OSM_Thread *uarch,unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_PMT::reset(iid,cid,ic);
      _uarch = uarch;
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    
    void deallocate_ops(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger)
    {
      __other__.deallocate(uarch,ic,logger);
    }
    
    bool exec_done() const { return true; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool move_S_EX__S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EX__S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EX__S_MM(uarch,logger,position);
    }
    
    bool change_state_S_EX__S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EX__S_MM_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_ID__S_EX(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool move_S_MM__S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MM__S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MM__S_WB(uarch,logger,position);
    }
    
    bool change_state_S_MM__S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MM__S_WB_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool move_S_WB__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WB__S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WB__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WB__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WB__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, 2 ) ) {
            uarch.exec(*this,logger) ;
            return change_state_S_EX__S_MM(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EX_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mEX.has_space(uarch,*this,logger) ) {
            return change_state_S_ID__S_EX(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_ID_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.has_space(uarch,*this,logger) ) if ( uarch. OSM_Thread::issueCheck ( * this , position , 0 ) ) {
            return change_state_S_INIT__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_MM
    bool action_S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_MM__S_WB(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WB__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WB(uarch,logger,position);
    }
    
    // Flush action S_EX
    void flush_S_EX(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_EX_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_ID_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class sfx
  //
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: XER[mask 0x80000000]/p)
  struct addic__sfx_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).XER.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).XER.name(),-1);
    }
    
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
        case 1: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).XER.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).XER.name(),-1);
        break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,1,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).XER.name(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_PMT&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return ic._masks[0];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(1).uint32();
        case 1: return ic._instr->write_mask(3).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: ic._instr->write_mask(1) = tag;
        break;
        case 1: ic._instr->write_mask(3) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 1;
        case 1: return 3;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_PMT&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  // Operands:
  //   (trg: CR[mask 0xf0000000]/p)
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0x20000000]/p)
  struct addic__sfx_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[2])) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.CR.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint32())) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
          return true;
        case 2: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[2] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
        case 2: break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
        case 2: break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_Thread&>(uarch).CR.id(static_cast<OSM_Thread&>(uarch),0));
          mask |= uarch.CR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_Thread&>(uarch).CR.id(static_cast<OSM_Thread&>(uarch),0));
          mask |= uarch.CR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,0,mask))return false;
      }
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(2)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read"))) return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,2,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.CR.allocate(uarch,ic,logger, ic._masks[1]);
      uarch.GPR.allocate(uarch,ic,logger,ic.op(2)._index);
      uarch.XER.allocate(uarch,ic,logger, ic._masks[2]);
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.CR.deallocate(uarch,ic,logger, ic._masks[1]);
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(2)._index);
      uarch.XER.deallocate(uarch,ic,logger, ic._masks[2]);
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).CR.name(),-1);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).XER.name(),-1);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).CR.name(static_cast<OSM_Thread&>(uarch),0);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
        case 2: return static_cast<OSM_Thread&>(uarch).XER.name(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_PMT&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).CR.id(static_cast<OSM_Thread&>(uarch),0);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
        case 2: return static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._masks[1];
        case 1: return 0x0;
        case 2: return ic._masks[2];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(2).uint32();
        case 2: return ic._instr->write_mask(3).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: ic._instr->write_mask(0) = tag;
        break;
        case 1: ic._instr->write_mask(2) = tag;
        break;
        case 2: ic._instr->write_mask(3) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 2;
        case 2: return 3;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_PMT&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 3; };
    
  };
  
  // Operands:
  //   (src: GPR(RA))
  struct addic_sfx_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(0)._index); }
    
    RegId id(UadlArch &uarch,const Inst_PMT &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index); }
    
    bits<32> mask(Inst_PMT&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned = 0) const { return ic._instr->write_mask(0).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned = 0) const { ic._instr->write_mask(0) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 0; }
    
    bool is_op_read(Inst_PMT&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0x20000000]/p)
  struct addic_sfx_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
        case 1: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,1,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(1)._index);
      uarch.XER.allocate(uarch,ic,logger, ic._masks[0]);
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(1)._index);
      uarch.XER.deallocate(uarch,ic,logger, ic._masks[0]);
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).XER.name(),-1);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).XER.name(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_PMT&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return ic._masks[0];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(1).uint32();
        case 1: return ic._instr->write_mask(2).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: ic._instr->write_mask(1) = tag;
        break;
        case 1: ic._instr->write_mask(2) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 1;
        case 1: return 2;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_PMT&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  // Operand Alias:  addic_sfx_Src -> addi_sfx_Src
  //   (src: GPR(RA)?)
  typedef addic_sfx_Src addi_sfx_Src;
  
  // Operands:
  //   (trg: GPR(RT))
  struct addi_sfx_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(1)._index);
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(1)._index);
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(1)._index); }
    
    RegId id(UadlArch &uarch,const Inst_PMT &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index); }
    
    bits<32> mask(Inst_PMT&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned = 0) const { return ic._instr->write_mask(1).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned = 0) const { ic._instr->write_mask(1) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 1; }
    
    bool is_op_read(Inst_PMT&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operand Alias:  addic_sfx_Src -> addis_sfx_Src
  //   (src: GPR(RA)?)
  typedef addic_sfx_Src addis_sfx_Src;
  
  // Operand Alias:  addi_sfx_Trg -> addis_sfx_Trg
  //   (trg: GPR(RT))
  typedef addi_sfx_Trg addis_sfx_Trg;
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: XER[mask 0x20000000]/p)
  struct addme_sfx_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).XER.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).XER.name(),-1);
    }
    
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
          return true;
        case 1: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).XER.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).XER.name(),-1);
        break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,1,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).XER.name(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_PMT&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return ic._masks[1];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(2).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: ic._instr->write_mask(0) = tag;
        break;
        case 1: ic._instr->write_mask(2) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 2;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_PMT&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  // Operand Alias:  addic_sfx_Trg -> addme_sfx_Trg
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0x20000000]/p)
  typedef addic_sfx_Trg addme_sfx_Trg;
  
  // Operand Alias:  addic__sfx_Src -> addme__sfx_Src
  //   (src: GPR(RA))
  //   (src: XER[mask 0xa0000000]/p)
  typedef addic__sfx_Src addme__sfx_Src;
  
  // Operand Alias:  addic__sfx_Trg -> addme__sfx_Trg
  //   (trg: CR[mask 0xf0000000]/p)
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0x20000000]/p)
  typedef addic__sfx_Trg addme__sfx_Trg;
  
  // Operand Alias:  addme_sfx_Src -> addmeo_sfx_Src
  //   (src: GPR(RA))
  //   (src: XER[mask 0xe0000000]/p)
  typedef addme_sfx_Src addmeo_sfx_Src;
  
  // Operand Alias:  addic_sfx_Trg -> addmeo_sfx_Trg
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0xe0000000]/p)
  typedef addic_sfx_Trg addmeo_sfx_Trg;
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: XER[mask 0xc0000000]/p)
  struct addo_sfx_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).XER.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).XER.name(),-1);
    }
    
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
        case 2: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).XER.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).XER.name(),-1);
        break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,2,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        case 2: return static_cast<OSM_Thread&>(uarch).XER.name(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_PMT&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        case 2: return static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
        case 2: return ic._masks[0];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
        case 2: return ic._instr->write_mask(3).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: ic._instr->write_mask(0) = tag;
        break;
        case 1: ic._instr->write_mask(1) = tag;
        break;
        case 2: ic._instr->write_mask(3) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 1;
        case 2: return 3;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_PMT&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 3; };
    
  };
  
  // Operands:
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0xc0000000]/p)
  struct addo_sfx_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
          return true;
        case 1: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(2)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read"))) return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,1,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(2)._index);
      uarch.XER.allocate(uarch,ic,logger, ic._masks[1]);
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(2)._index);
      uarch.XER.deallocate(uarch,ic,logger, ic._masks[1]);
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).XER.name(),-1);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(2)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).XER.name(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_PMT&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return ic._masks[1];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(2).uint32();
        case 1: return ic._instr->write_mask(3).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: ic._instr->write_mask(2) = tag;
        break;
        case 1: ic._instr->write_mask(3) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 2;
        case 1: return 3;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_PMT&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  // Operands:
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  struct or_sfx_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index);
    }
    
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index);
        break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(2)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_PMT&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(1).uint32();
        case 1: return ic._instr->write_mask(2).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: ic._instr->write_mask(1) = tag;
        break;
        case 1: ic._instr->write_mask(2) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 1;
        case 1: return 2;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_PMT&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  // Operands:
  //   (trg: GPR(RA))
  struct or_sfx_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(0)._index);
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(0)._index);
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(0)._index); }
    
    RegId id(UadlArch &uarch,const Inst_PMT &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index); }
    
    bits<32> mask(Inst_PMT&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned = 0) const { return ic._instr->write_mask(0).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned = 0) const { ic._instr->write_mask(0) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 0; }
    
    bool is_op_read(Inst_PMT&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (src: GPR(RS))
  struct ori_sfx_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(1)._index); }
    
    RegId id(UadlArch &uarch,const Inst_PMT &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index); }
    
    bits<32> mask(Inst_PMT&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned = 0) const { return ic._instr->write_mask(1).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned = 0) const { ic._instr->write_mask(1) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 1; }
    
    bool is_op_read(Inst_PMT&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operand Alias:  or_sfx_Trg -> ori_sfx_Trg
  //   (trg: GPR(RA))
  typedef or_sfx_Trg ori_sfx_Trg;
  
  // Operands:
  //   (src: GPR(RS))
  //   (src: XER[mask 0x80000000]/p)
  struct andi__sfx_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).XER.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index);
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).XER.name(),-1);
    }
    
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
          return true;
        case 1: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).XER.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).XER.name(),-1);
        break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(2)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read"))) return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,1,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(2)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).XER.name(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_PMT&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return ic._masks[0];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(2).uint32();
        case 1: return ic._instr->write_mask(3).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: ic._instr->write_mask(2) = tag;
        break;
        case 1: ic._instr->write_mask(3) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 2;
        case 1: return 3;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_PMT&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  // Operands:
  //   (trg: CR[mask 0xf0000000]/p)
  //   (trg: GPR(RA))
  struct andi__sfx_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.CR.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint32())) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_Thread&>(uarch).CR.id(static_cast<OSM_Thread&>(uarch),0));
          mask |= uarch.CR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_Thread&>(uarch).CR.id(static_cast<OSM_Thread&>(uarch),0));
          mask |= uarch.CR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,0,mask))return false;
      }
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.CR.allocate(uarch,ic,logger, ic._masks[1]);
      uarch.GPR.allocate(uarch,ic,logger,ic.op(1)._index);
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.CR.deallocate(uarch,ic,logger, ic._masks[1]);
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(1)._index);
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).CR.name(),-1);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).CR.name(static_cast<OSM_Thread&>(uarch),0);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_PMT&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).CR.id(static_cast<OSM_Thread&>(uarch),0);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._masks[1];
        case 1: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: ic._instr->write_mask(0) = tag;
        break;
        case 1: ic._instr->write_mask(1) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 1;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_PMT&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  // Operand Alias:  ori_sfx_Src -> oris_sfx_Src
  //   (src: GPR(RS))
  typedef ori_sfx_Src oris_sfx_Src;
  
  // Operand Alias:  or_sfx_Trg -> oris_sfx_Trg
  //   (trg: GPR(RA))
  typedef or_sfx_Trg oris_sfx_Trg;
  
  // Operand Alias:  ori_sfx_Src -> rlwinm_sfx_Src
  //   (src: GPR(RS))
  typedef ori_sfx_Src rlwinm_sfx_Src;
  
  // Operand Alias:  or_sfx_Trg -> rlwinm_sfx_Trg
  //   (trg: GPR(RA))
  typedef or_sfx_Trg rlwinm_sfx_Trg;
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  struct add_sfx_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
    }
    
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
        break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_PMT&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: ic._instr->write_mask(0) = tag;
        break;
        case 1: ic._instr->write_mask(1) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 1;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_PMT&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  // Operands:
  //   (trg: GPR(RT))
  struct add_sfx_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(2)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(2)._index);
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(2)._index);
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(2)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(2)._index); }
    
    RegId id(UadlArch &uarch,const Inst_PMT &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(2)._index); }
    
    bits<32> mask(Inst_PMT&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned = 0) const { return ic._instr->write_mask(2).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned = 0) const { ic._instr->write_mask(2) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 2; }
    
    bool is_op_read(Inst_PMT&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: XER[mask 0x80000000]/p)
  struct cmpi_sfx_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).XER.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).XER.name(),-1);
    }
    
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
        case 1: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).XER.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).XER.name(),-1);
        break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,1,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).XER.name(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_PMT&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return ic._masks[0];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(1).uint32();
        case 1: return ic._instr->write_mask(2).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: ic._instr->write_mask(1) = tag;
        break;
        case 1: ic._instr->write_mask(2) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 1;
        case 1: return 2;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_PMT&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  // Operands:
  //   (trg: CR/p)
  struct cmpi_sfx_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).CR.id(static_cast<OSM_Thread&>(uarch),0);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).CR.last_allocator( static_cast<OSM_Thread&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.CR.allocate(uarch,ic,logger);
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.CR.deallocate(uarch,ic,logger);
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).CR.name(),-1);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_Thread&>(uarch).CR.name(static_cast<OSM_Thread&>(uarch),0); }
    
    RegId id(UadlArch &uarch,const Inst_PMT &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_Thread&>(uarch).CR.id(static_cast<OSM_Thread&>(uarch),0); }
    
    bits<32> mask(Inst_PMT&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned = 0) const { return ic._instr->write_mask(0).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned = 0) const { ic._instr->write_mask(0) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 0; }
    
    bool is_op_read(Inst_PMT&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: XER[mask 0x80000000]/p)
  struct cmp_sfx_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).XER.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index);
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).XER.name(),-1);
    }
    
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
          return true;
        case 2: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).XER.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).XER.name(),-1);
        break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(2)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read"))) return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,2,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
        case 2: return static_cast<OSM_Thread&>(uarch).XER.name(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_PMT&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
        case 2: return static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
        case 2: return ic._masks[1];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(1).uint32();
        case 1: return ic._instr->write_mask(2).uint32();
        case 2: return ic._instr->write_mask(3).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: ic._instr->write_mask(1) = tag;
        break;
        case 1: ic._instr->write_mask(2) = tag;
        break;
        case 2: ic._instr->write_mask(3) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_PMT&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 3; };
    
  };
  
  // Operand Alias:  cmpi_sfx_Trg -> cmp_sfx_Trg
  //   (trg: CR/p)
  typedef cmpi_sfx_Trg cmp_sfx_Trg;
  
  // Operands:
  //   (src: SPR(SPRN))
  struct mfspr_sfx_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.SPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).SPR.name(),ic.op(1)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).SPR.name(),ic.op(1)._index);
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).SPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).SPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.SPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_Thread&>(uarch).SPR.name(static_cast<OSM_Thread&>(uarch),ic.op(1)._index); }
    
    RegId id(UadlArch &uarch,const Inst_PMT &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_Thread&>(uarch).SPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index); }
    
    bits<32> mask(Inst_PMT&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned = 0) const { return ic._instr->write_mask(1).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned = 0) const { ic._instr->write_mask(1) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 1; }
    
    bool is_op_read(Inst_PMT&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (trg: GPR(RT))
  struct mfspr_sfx_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(0)._index);
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(0)._index);
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(0)._index); }
    
    RegId id(UadlArch &uarch,const Inst_PMT &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index); }
    
    bits<32> mask(Inst_PMT&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned = 0) const { return ic._instr->write_mask(0).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned = 0) const { ic._instr->write_mask(0) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 0; }
    
    bool is_op_read(Inst_PMT&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (src: GPR(RB)[mask 0x0000001f]/p)
  //   (src: GPR(RS))
  struct srw_sfx_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read", ic._masks[0])) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index);
    }
    
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read", (ic._masks[0] & ~mask).uint32())) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index);
        break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index));
          mask |= uarch.GPR.rm_later_partial_alloc(uarch,ic.op(1)._index,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index));
          mask |= uarch.GPR.rm_later_partial_alloc(uarch,ic.op(1)._index,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,0,mask))return false;
      }
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(2)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_PMT&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._masks[0];
        case 1: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(1).uint32();
        case 1: return ic._instr->write_mask(2).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: ic._instr->write_mask(1) = tag;
        break;
        case 1: ic._instr->write_mask(2) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 1;
        case 1: return 2;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_PMT&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  // Operand Alias:  or_sfx_Trg -> srw_sfx_Trg
  //   (trg: GPR(RA))
  typedef or_sfx_Trg srw_sfx_Trg;
  
  // Operands:
  //   (src: GPR(RS))
  struct mtspr_sfx_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(0)._index); }
    
    RegId id(UadlArch &uarch,const Inst_PMT &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index); }
    
    bits<32> mask(Inst_PMT&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned = 0) const { return ic._instr->write_mask(0).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned = 0) const { ic._instr->write_mask(0) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 0; }
    
    bool is_op_read(Inst_PMT&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (trg: SPR(SPRN))
  struct mtspr_sfx_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.SPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).SPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).SPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.SPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.SPR.allocate(uarch,ic,logger,ic.op(1)._index);
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.SPR.deallocate(uarch,ic,logger,ic.op(1)._index);
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).SPR.name(),ic.op(1)._index);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_Thread&>(uarch).SPR.name(static_cast<OSM_Thread&>(uarch),ic.op(1)._index); }
    
    RegId id(UadlArch &uarch,const Inst_PMT &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_Thread&>(uarch).SPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index); }
    
    bits<32> mask(Inst_PMT&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned = 0) const { return ic._instr->write_mask(1).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned = 0) const { ic._instr->write_mask(1) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 1; }
    
    bool is_op_read(Inst_PMT&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  //
  // Instruction class sfx
  //
  template <class Src_t, class Trg_t>
  struct Inst_sfx : public Inst_PMT {
    
    typedef struct Inst_sfx<Src_t, Trg_t> ThisClass;
    
    Inst_sfx &thisInstr() { return *this; };
    
    Src_t Src;
    Trg_t Trg;
    void reset(OSM_Thread *uarch,unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_PMT::reset(iid,cid,ic);
      _uarch = uarch;
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    
    void deallocate_ops(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger)
    {
      Src.deallocate(uarch,ic,logger);
      Trg.deallocate(uarch,ic,logger);
    }
    
    void write_ops(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger)
    {
      Src.write(uarch,ic,logger);
      Trg.write(uarch,ic,logger);
    }
    
    bool exec_done() const { return true; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool move_S_EX__S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EX__S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EX__S_MM(uarch,logger,position);
    }
    
    bool change_state_S_EX__S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EX__S_MM_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_EX__S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      return action_S_EXe(uarch,logger,position);
    }
    
    bool move_S_EXe__S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EXe__S_MMe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EXe__S_MMe(uarch,logger,position);
    }
    
    bool change_state_S_EXe__S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EXe__S_MMe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_ID__S_EX(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_IDe"); }
      _action = (StateFunc)action_S_IDe_p;
      _flush = (FlushFunc)flush_S_IDe_p;
      return action_S_IDe(uarch,logger,position);
    }
    
    bool change_state_S_IDe__S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDe -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool move_S_MM__S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MM__S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MM__S_WB(uarch,logger,position);
    }
    
    bool change_state_S_MM__S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MM__S_WB_p;
      _action = (StateFunc)no_move_action_S_MM_p;
      return true;
    }
    
    bool change_state_S_MM__S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      return action_S_MMe(uarch,logger,position);
    }
    
    bool move_S_MMe__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MMe__S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MMe__S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_MMe__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MMe__S_WBe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool move_S_WB__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WB__S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WB__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WB__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WB__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_WB__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool move_S_WBe__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WBe__S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WBe__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WBe__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, 2 ) ) {
            uarch.exec_mEX(*this,logger,position) ;
            uarch.FwdEX.write(uarch,*this,logger, Trg ) ;
            return change_state_S_EX__S_MM(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        flush_S_EX(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mEX(*this,logger,position,true,branch_target());
        return change_state_S_EX__S_EXe(uarch,logger,position);
      }
    }
    static bool action_S_EX_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_EXe__S_MMe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( Src.can_read_fp(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) && Trg.can_write(uarch,*this,logger) && uarch.mEX.has_space(uarch,*this,logger) && uarch.all_stall.can_allocate(uarch,*this,logger) ) {
            Src.read(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) ;
            Trg.allocate(uarch,*this,logger) ;
            return change_state_S_ID__S_EX(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        flush_S_ID(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mID(*this,logger,position,true,branch_target());
        return change_state_S_ID__S_IDe(uarch,logger,position);
      }
    }
    static bool action_S_ID_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_IDe
    bool action_S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mEX.has_space(uarch,*this,logger) ) {
            return change_state_S_IDe__S_EXe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_IDe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IDe(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.has_space(uarch,*this,logger) ) if ( uarch. OSM_Thread::issueCheck ( * this , position , adliss::PMT::Thread::muldiv_instrs ) ) {
            return change_state_S_INIT__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_MM
    bool action_S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            uarch.FwdMM.write(uarch,*this,logger, Trg ) ;
            return change_state_S_MM__S_WB(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        flush_S_MM(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mMM(*this,logger,position,true,branch_target());
        return change_state_S_MM__S_MMe(uarch,logger,position);
      }
    }
    static bool action_S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // Repeat action S_MM
    bool no_move_action_S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      uarch.FwdMM.write(uarch,*this,logger, Trg ) ;
      return true;
    }
    
    static bool no_move_action_S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->no_move_action_S_MM(uarch,logger,position);
    }
    
    // State action S_MMe
    bool action_S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_MMe__S_WBe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MMe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MMe(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      {
            write_ops(uarch,*this,logger) ;
            return change_state_S_WB__S_INIT(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        flush_S_WB(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mWB(*this,logger,position,true,branch_target());
        return change_state_S_WB__S_WBe(uarch,logger,position);
      }
    }
    static bool action_S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WB(uarch,logger,position);
    }
    
    // State action S_WBe
    bool action_S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WBe__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WBe(uarch,logger,position);
    }
    
    // Flush action S_EX
    void flush_S_EX(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_EX_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_EXe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_ID_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_IDe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_MMe
    void flush_S_MMe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_MMe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMe(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBe(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class stmw
  //
  
  // Operands:
  //   (src: GPR(RA), mask: 0xffffffff)
  //   (src: GPR(RS . uint32 (  ),31), mask: 0xffffffff)
  struct stmw_stmw_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read", ic._masks[0])) return false;
      for (int i = 1; i != 33; ++i) {
        if (ic.op(i).isSrc() && !uarch.GPR.can_alloc(uarch,ic,logger,ic.op(i)._index,"read", ic._masks[1])) return false;
      }
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
      for (int i = 1; i != 33; ++i) {
        if (ic.op(i).isSrc()) {
          if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(i)._index);
        }
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
      for (int i = 1; i != 33; ++i) {
        if (ic.op(i).isSrc()) {
          logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(i)._index);
        }
      }
    }
    
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read", (ic._masks[0] & ~mask).uint32())) return false;
          return true;
        case 1: for (int i = 1; i != 33; ++i) {
          if (ic.op(i).isSrc() && !uarch.GPR.can_alloc(uarch,ic,logger,ic.op(i)._index,"read", (ic._masks[1] & ~mask).uint32())) return false;
        }
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: for (int i = 1; i != 33; ++i) {
          if (ic.op(i).isSrc()) {
            if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(i)._index);
          }
        }
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: for (int i = 1; i != 33; ++i) {
          if (ic.op(i).isSrc()) {
            logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(i)._index);
          }
        }
        break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index));
          mask |= uarch.GPR.rm_later_partial_alloc(uarch,ic.op(0)._index,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index));
          mask |= uarch.GPR.rm_later_partial_alloc(uarch,ic.op(0)._index,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,0,mask))return false;
      }
      for (int i = 1; i != 33; ++i) {
        if  (ic.op(i).isSrc()) {
          bits<32> mask = 0;
          {
            std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(i)._index));
            mask |= uarch.GPR.rm_later_partial_alloc(uarch,ic.op(i)._index,pm.first.uint32(),pm.second);
          }
          {
            std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(i)._index));
            mask |= uarch.GPR.rm_later_partial_alloc(uarch,ic.op(i)._index,pm.first.uint32(),pm.second);
          }
          if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(i)._index,"read", (ic._masks[1] & ~mask).uint32())) return false;
        }
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(op_off)._index);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_PMT&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(op_off)._index);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._masks[0];
        case 1: return ic._masks[1];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: ic._instr->write_mask(0) = tag;
        break;
        case 1: ic._instr->write_mask(1) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return op_off;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_PMT&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  // Operands:
  //   (src: CR, mask: 0xffffffff)
  struct stmw_stmw_SrcCR {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read", ic._masks[2])) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,const bits<32> &mask) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read", (ic._masks[2] & ~mask).uint32())) return false;
      return true;
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_Thread&>(uarch).CR.id(static_cast<OSM_Thread&>(uarch),0));
          mask |= uarch.CR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_Thread&>(uarch).CR.id(static_cast<OSM_Thread&>(uarch),0));
          mask |= uarch.CR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return 0;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_Thread&>(uarch).CR.name(static_cast<OSM_Thread&>(uarch),0); }
    
    RegId id(UadlArch &uarch,const Inst_PMT &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_Thread&>(uarch).CR.id(static_cast<OSM_Thread&>(uarch),0); }
    
    bits<32> mask(Inst_PMT&ic,unsigned = 0) const { return ic._masks[2]; }
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned = 0) const { return ic._instr->write_mask(0).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned = 0) const { ic._instr->write_mask(0) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 0; }
    
    bool is_op_read(Inst_PMT&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  //
  // Instruction class stmw
  //
  template <class Src_t, class SrcCR_t, class Trg_t>
  struct Inst_stmw : public Inst_PMT {
    
    typedef struct Inst_stmw<Src_t, SrcCR_t, Trg_t> ThisClass;
    
    Inst_stmw &thisInstr() { return *this; };
    
    Src_t Src;
    SrcCR_t SrcCR;
    Trg_t Trg;
    void reset(OSM_Thread *uarch,unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_PMT::reset(iid,cid,ic);
      _uarch = uarch;
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    
    void deallocate_ops(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger)
    {
      Src.deallocate(uarch,ic,logger);
      SrcCR.deallocate(uarch,ic,logger);
      Trg.deallocate(uarch,ic,logger);
    }
    
    void write_ops(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger)
    {
      Src.write(uarch,ic,logger);
      SrcCR.write(uarch,ic,logger);
      Trg.write(uarch,ic,logger);
    }
    
    bool exec_done() const { return mem() == 0; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool move_S_EX__S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EX__S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EX__S_MM(uarch,logger,position);
    }
    
    bool change_state_S_EX__S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EX__S_MM_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_EX__S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      return action_S_EXe(uarch,logger,position);
    }
    
    bool move_S_EXe__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mEX.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_EXe__S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EXe__S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_EXe__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EXe__S_WBe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_ID__S_EX(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_IDe"); }
      _action = (StateFunc)action_S_IDe_p;
      _flush = (FlushFunc)flush_S_IDe_p;
      return action_S_IDe(uarch,logger,position);
    }
    
    bool change_state_S_IDe__S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDe -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool change_state_S_MM__S_MMp(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMp"); }
      _action = (StateFunc)action_S_MMp_p;
      _flush = (FlushFunc)flush_S_MMp_p;
      return action_S_MMp(uarch,logger,position);
    }
    
    bool change_state_S_MMp__S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMp -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      return action_S_MM(uarch,logger,position);
    }
    
    bool move_S_MMp__S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMp -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MMp__S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MMp__S_WB(uarch,logger,position);
    }
    
    bool change_state_S_MMp__S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MMp__S_WB_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool move_S_WB__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WB__S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WB__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WB__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WB__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_WB__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool move_S_WBe__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WBe__S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WBe__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WBe__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger) && uarch.L1d.can_request_write(uarch,*this,logger) && ! uarch.stalled() ) {
            {uarch.exec_mem(*this,logger) ; if (has_exceptions()) goto HandleException; } ;
            uarch.L1d.send_write_request(uarch,*this,logger, 8 ) ;
            next_req_mem (  ) ;
            return change_state_S_EX__S_MM(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_EX(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mEX(*this,logger,position,true,branch_target());
        return change_state_S_EX__S_EXe(uarch,logger,position);
      }
    }
    static bool action_S_EX_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_EXe__S_WBe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( Src.can_read_fp(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) && Trg.can_write(uarch,*this,logger) && SrcCR.can_read_fp(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) && uarch.mEX.has_space(uarch,*this,logger) && uarch.all_stall.can_allocate(uarch,*this,logger) && ! uarch.stalled() ) {
            Src.read(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) ;
            SrcCR.read(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) ;
            Trg.allocate(uarch,*this,logger) ;
            return change_state_S_ID__S_EX(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_ID(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mID(*this,logger,position,true,branch_target());
        return change_state_S_ID__S_IDe(uarch,logger,position);
      }
    }
    static bool action_S_ID_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_IDe
    bool action_S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mEX.has_space(uarch,*this,logger) ) {
            return change_state_S_IDe__S_EXe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_IDe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IDe(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.has_space(uarch,*this,logger) ) if ( uarch. OSM_Thread::loadStoreIssueCheck ( * this , position , adliss::PMT::Thread::special_instrs | adliss::PMT::Thread::store_instrs | adliss::PMT::Thread::load_instrs | adliss::PMT::Thread::muldiv_instrs | adliss::PMT::Thread::branch_instrs ) ) {
            return change_state_S_INIT__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_MM
    bool action_S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.L1d.can_write(uarch,*this,logger) ) {
            uarch.exec_and_write(*this,logger, uarch.L1d , 2 , 4 ) ;
            return change_state_S_MM__S_MMp(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // State action S_MMp
    bool action_S_MMp(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( exec_done (  ) && uarch.mWB.has_space(uarch,*this,logger) ) {
            return change_state_S_MMp__S_WB(uarch,logger,position) ;
      } else if ( uarch.L1d.can_request_write(uarch,*this,logger) ) {
            uarch.L1d.send_write_request(uarch,*this,logger, 8 ) ;
            next_req_mem (  ) ;
            return change_state_S_MMp__S_MM(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MMp_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MMp(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      {
            write_ops(uarch,*this,logger) ;
            return change_state_S_WB__S_INIT(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_WB(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mWB(*this,logger,position,true,branch_target());
        return change_state_S_WB__S_WBe(uarch,logger,position);
      }
    }
    static bool action_S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WB(uarch,logger,position);
    }
    
    // State action S_WBe
    bool action_S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WBe__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WBe(uarch,logger,position);
    }
    
    // Flush action S_EX
    void flush_S_EX(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_EX_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_EXe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_ID_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_IDe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_MMp
    void flush_S_MMp(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_MMp_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMp(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBe(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class store
  //
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: GPR(RS))
  struct stb_store_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
    }
    
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
        break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_PMT&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: ic._instr->write_mask(0) = tag;
        break;
        case 1: ic._instr->write_mask(1) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 1;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_PMT&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  // Operand Alias:  stb_store_Src -> sth_store_Src
  //   (src: GPR(RA))
  //   (src: GPR(RS))
  typedef stb_store_Src sth_store_Src;
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  struct sthx_store_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index);
    }
    
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
        case 2: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index);
        break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(2)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        case 2: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_PMT&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        case 2: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
        case 2: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
        case 2: return ic._instr->write_mask(2).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: ic._instr->write_mask(0) = tag;
        break;
        case 1: ic._instr->write_mask(1) = tag;
        break;
        case 2: ic._instr->write_mask(2) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 1;
        case 2: return 2;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_PMT&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 3; };
    
  };
  
  // Operand Alias:  stb_store_Src -> stw_store_Src
  //   (src: GPR(RA))
  //   (src: GPR(RS))
  typedef stb_store_Src stw_store_Src;
  
  // Operand Alias:  stb_store_Src -> stwu_store_Src
  //   (src: GPR(RA))
  //   (src: GPR(RS))
  typedef stb_store_Src stwu_store_Src;
  
  // Operands:
  //   (trg: GPR(RA))
  struct stwu_store_TrgEA {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(0)._index);
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(0)._index);
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index);
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(0)._index); }
    
    RegId id(UadlArch &uarch,const Inst_PMT &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index); }
    
    bits<32> mask(Inst_PMT&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned = 0) const { return ic._instr->write_mask(0).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned = 0) const { ic._instr->write_mask(0) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 0; }
    
    bool is_op_read(Inst_PMT&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operand Alias:  sthx_store_Src -> stwx_store_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  typedef sthx_store_Src stwx_store_Src;
  
  //
  // Instruction class store
  //
  template <class Src_t, class TrgEA_t>
  struct Inst_store : public Inst_PMT {
    
    typedef struct Inst_store<Src_t, TrgEA_t> ThisClass;
    
    Inst_store &thisInstr() { return *this; };
    
    Src_t Src;
    TrgEA_t TrgEA;
    void reset(OSM_Thread *uarch,unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_PMT::reset(iid,cid,ic);
      _uarch = uarch;
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    
    void deallocate_ops(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger)
    {
      Src.deallocate(uarch,ic,logger);
      TrgEA.deallocate(uarch,ic,logger);
    }
    
    void write_ops(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger)
    {
      Src.write(uarch,ic,logger);
      TrgEA.write(uarch,ic,logger);
    }
    
    bool exec_done() const { return mem() == 0; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool move_S_EX__S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EX__S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EX__S_MM(uarch,logger,position);
    }
    
    bool change_state_S_EX__S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EX__S_MM_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_EX__S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      return action_S_EXe(uarch,logger,position);
    }
    
    bool move_S_EXe__S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EXe__S_MMe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EXe__S_MMe(uarch,logger,position);
    }
    
    bool change_state_S_EXe__S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EXe__S_MMe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_ID__S_EX(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_IDe"); }
      _action = (StateFunc)action_S_IDe_p;
      _flush = (FlushFunc)flush_S_IDe_p;
      return action_S_IDe(uarch,logger,position);
    }
    
    bool change_state_S_IDe__S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDe -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool change_state_S_MM__S_MMp(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMp"); }
      _action = (StateFunc)action_S_MMp_p;
      _flush = (FlushFunc)flush_S_MMp_p;
      return action_S_MMp(uarch,logger,position);
    }
    
    bool change_state_S_MM__S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      return action_S_MMe(uarch,logger,position);
    }
    
    bool move_S_MMe__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MMe__S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MMe__S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_MMe__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MMe__S_WBe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_MMp__S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMp -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      return action_S_MM(uarch,logger,position);
    }
    
    bool move_S_MMp__S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMp -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MMp__S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MMp__S_WB(uarch,logger,position);
    }
    
    bool change_state_S_MMp__S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MMp__S_WB_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_WB__S_WBp(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBp"); }
      _action = (StateFunc)action_S_WBp_p;
      _flush = (FlushFunc)flush_S_WBp_p;
      return action_S_WBp(uarch,logger,position);
    }
    
    bool change_state_S_WB__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool move_S_WBe__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WBe__S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WBe__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WBe__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool move_S_WBp__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBp -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WBp__S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WBp__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WBp__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WBp__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, uarch.mEX.capacity() ) && uarch.L1d.can_request_write(uarch,*this,logger) && ! uarch.stalled() ) {
            {uarch.exec_mem(*this,logger) ; if (has_exceptions()) goto HandleException; } ;
            uarch.FwdEX.write(uarch,*this,logger, TrgEA ) ;
            uarch.L1d.send_write_request(uarch,*this,logger) ;
            return change_state_S_EX__S_MM(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_EX(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mEX(*this,logger,position,true,branch_target());
        return change_state_S_EX__S_EXe(uarch,logger,position);
      }
    }
    static bool action_S_EX_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_EXe__S_MMe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( Src.can_read_fp(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) && TrgEA.can_write(uarch,*this,logger) && uarch.mEX.has_space(uarch,*this,logger) && uarch.hid_stall.can_allocate(uarch,*this,logger) && ! uarch.stalled() ) {
            Src.read(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) ;
            TrgEA.allocate(uarch,*this,logger) ;
            return change_state_S_ID__S_EX(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_ID(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mID(*this,logger,position,true,branch_target());
        return change_state_S_ID__S_IDe(uarch,logger,position);
      }
    }
    static bool action_S_ID_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_IDe
    bool action_S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mEX.has_space(uarch,*this,logger) ) {
            return change_state_S_IDe__S_EXe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_IDe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IDe(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.has_space(uarch,*this,logger) ) if ( uarch. OSM_Thread::loadStoreIssueCheck ( * this , position , adliss::PMT::Thread::special_instrs | adliss::PMT::Thread::store_instrs | adliss::PMT::Thread::load_instrs | adliss::PMT::Thread::muldiv_instrs | adliss::PMT::Thread::branch_instrs ) ) {
            return change_state_S_INIT__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_MM
    bool action_S_MM(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.L1d.can_write(uarch,*this,logger) ) {
            { uarch.exec_and_write(*this,logger, uarch.L1d )  ; if (has_exceptions()) goto HandleException; } ;
            return change_state_S_MM__S_MMp(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_MM(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mMM(*this,logger,position,true,branch_target());
        return change_state_S_MM__S_MMe(uarch,logger,position);
      }
    }
    static bool action_S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // State action S_MMe
    bool action_S_MMe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_MMe__S_WBe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MMe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MMe(uarch,logger,position);
    }
    
    // State action S_MMp
    bool action_S_MMp(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( exec_done (  ) && uarch.mWB.has_space(uarch,*this,logger, uarch.mMM.capacity() ) ) {
            return change_state_S_MMp__S_WB(uarch,logger,position) ;
      } else if ( uarch.L1d.can_request_write(uarch,*this,logger) ) {
            uarch.L1d.send_write_request(uarch,*this,logger) ;
            return change_state_S_MMp__S_MM(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MMp_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MMp(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      {
            write_ops(uarch,*this,logger) ;
            return change_state_S_WB__S_WBp(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_WB(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mWB(*this,logger,position,true,branch_target());
        return change_state_S_WB__S_WBe(uarch,logger,position);
      }
    }
    static bool action_S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WB(uarch,logger,position);
    }
    
    // State action S_WBe
    bool action_S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WBe__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WBe(uarch,logger,position);
    }
    
    // State action S_WBp
    bool action_S_WBp(OSM_Thread &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WBp__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WBp_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WBp(uarch,logger,position);
    }
    
    // Flush action S_EX
    void flush_S_EX(OSM_Thread &uarch,Logger *logger) {
      TrgEA.deallocate(uarch,*this,logger);
    }
    static void flush_S_EX_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_EXe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_Thread &uarch,Logger *logger) {
      TrgEA.deallocate(uarch,*this,logger);
    }
    static void flush_S_ID_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_IDe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_Thread &uarch,Logger *logger) {
      TrgEA.deallocate(uarch,*this,logger);
    }
    static void flush_S_MM_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_MMe
    void flush_S_MMe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_MMe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMe(uarch,logger);
    }
    
    // Flush action S_MMp
    void flush_S_MMp(OSM_Thread &uarch,Logger *logger) {
      TrgEA.deallocate(uarch,*this,logger);
    }
    static void flush_S_MMp_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMp(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_Thread &uarch,Logger *logger) {
      TrgEA.deallocate(uarch,*this,logger);
    }
    static void flush_S_WB_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_WBe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBe(uarch,logger);
    }
    
    // Flush action S_WBp
    void flush_S_WBp(OSM_Thread &uarch,Logger *logger) {
      TrgEA.deallocate(uarch,*this,logger);
    }
    static void flush_S_WBp_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBp(uarch,logger);
    }
    
  };
  
  byte_t *Mem_mem_t::hasData(addr_t addr, unsigned &size) const {
    return GlobalMemory.getPage(addr) + GlobalMemoryType::pageOffset(addr);
  }
  
  static CacheConfigMap::value_type _sym957[] = {
    CacheConfigMap::value_type("consecutive_linefills",CacheConfigItem(1)),
    CacheConfigMap::value_type("critical_word_first",CacheConfigItem(1)),
    CacheConfigMap::value_type("evict_lock_cycle",CacheConfigItem(1)),
    CacheConfigMap::value_type("invalidate_latency",CacheConfigItem(1)),
    CacheConfigMap::value_type("linefill_access_width",CacheConfigItem(64)),
    CacheConfigMap::value_type("linefill_lazy_writeback",CacheConfigItem(0)),
    CacheConfigMap::value_type("linefill_lock_cycle",CacheConfigItem(0)),
    CacheConfigMap::value_type("linefill_lock_holdoff_cycle",CacheConfigItem(2)),
    CacheConfigMap::value_type("linefill_lock_on_valid",CacheConfigItem(1)),
    CacheConfigMap::value_type("linefill_read_latency",CacheConfigItem(0)),
    CacheConfigMap::value_type("linefill_write_latency",CacheConfigItem(1)),
    CacheConfigMap::value_type("lock_latency",CacheConfigItem(1)),
    CacheConfigMap::value_type("read_hit_latency",CacheConfigItem(0)),
    CacheConfigMap::value_type("store_buffer_size",CacheConfigItem(4)),
    CacheConfigMap::value_type("streaming_linefill",CacheConfigItem(1)),
    CacheConfigMap::value_type("taken_to_write_latency",CacheConfigItem(2)),
    CacheConfigMap::value_type("unlock_latency",CacheConfigItem(1)),
    CacheConfigMap::value_type("write_hit_latency",CacheConfigItem(1)),
    CacheConfigMap::value_type("write_mode",CacheConfigItem("WriteThrough")),
    CacheConfigMap::value_type("write_to_next_level_request_latency",CacheConfigItem(1)),
    CacheConfigMap::value_type("zero_latency",CacheConfigItem(1))
  };
  
  L1d_cache_t::L1d_cache_t(OSM_PMT &uarch, UadlMemoryIf &memory) :
  MyCache(
      uarch,
      "L1d",
      memory,
      0,
      128,
      1, // address bandwidth
      1, // data bandwidth
      2, // max requests
      1, // allow bus wait
      0, // pre-emptive
      CacheConfigMap(&_sym957[0],&_sym957[21]),
      0  // with-data
    ),
    _uarch(uarch)
  {
  }
  
  void L1d_cache_t::reset_has_data()
  {
    if (_uarch.hadReset()) {
      // set up data array status if this is the first time step
      unsigned num_sets = _uarch._iss->t0->L1d_cache_get_sets();
      unsigned num_ways = _uarch._iss->t0->L1d_cache_get_ways();
      _hasData = HasData(num_sets,BVect(num_ways,false));
      for (unsigned set = 0; set < num_sets; set++) {
        for (unsigned way = 0; way < num_ways; way++) {
          _hasData[set][way] = _uarch._iss->t0->L1d_cache._array[set][way].valid;
        }
      }
    }
  }
  
  bool L1d_cache_t::lookup(addr_t addr, unsigned &set, unsigned &way, unsigned &offset) const
  {
    const bool hit = _uarch._iss->t0->L1d_cache_find(adl::CacheLogRead, (int&)set, (int&)way, addr);
    if (hit) offset = _uarch._iss->t0->L1d_cache_get_offset(addr);
    return hit;
  }
  
  addr_t L1d_cache_t::getTagAddr(unsigned set, unsigned way)
  {
    return _uarch._iss->t0->L1d_cache_get_tag(set, way);
  }
  
  void L1d_cache_t::resize() {
    unsigned num_sets = _uarch._iss->t0->L1d_cache_get_sets();
    unsigned num_ways = _uarch._iss->t0->L1d_cache_get_ways();
    _hasData = HasData(num_sets,std::vector<bool>(num_ways));
  }
  
  void L1d_cache_t::operationBegin(uadl::MemoryRequest &request)
  {
    Cache::operationBegin(request);
    
    if (_uarch._iss->t0->L1d_cache_begin(
            request.type, request.action,
            request.hint._cacheStatus,
            request.set, request.way, request.addr, request.replaceLine,
            hasCommand()
        ))
    {
      request.offset = _uarch._iss->t0->L1d_cache_get_offset(request.addr);
    } else {
      request.action = adl::CacheNone;
    }
    request.lookupDone = true;
  }
  
  void L1d_cache_t::operationEnd(const uadl::MemoryRequest &request) const
  {
    if (request.action != adl::CacheNone) {
      _uarch._iss->t0->L1d_cache_end(request.type,request.action, request.set, request.way,request.addr, hasCommand());
    }
  }
  
  
  void L1d_cache_t::clearValid(unsigned set, unsigned way)
  {
    _uarch._iss->t0->L1d_cache._array[set][way].valid = false;
  }
  
  bool L1d_cache_t::is_write_through(addr_t addr)
  {
    return _uarch._iss->t0->L1d.is_write_through(adl::CacheWrite,addr);
  }
  
  static CacheConfigMap::value_type _sym958[] = {
    CacheConfigMap::value_type("critical_word_first",CacheConfigItem(1)),
    CacheConfigMap::value_type("evict_lock_cycle",CacheConfigItem(1)),
    CacheConfigMap::value_type("invalidate_latency",CacheConfigItem(1)),
    CacheConfigMap::value_type("linefill_access_width",CacheConfigItem(64)),
    CacheConfigMap::value_type("linefill_lazy_writeback",CacheConfigItem(1)),
    CacheConfigMap::value_type("linefill_lock_cycle",CacheConfigItem(1)),
    CacheConfigMap::value_type("linefill_lock_holdoff_cycle",CacheConfigItem(2)),
    CacheConfigMap::value_type("linefill_lock_on_valid",CacheConfigItem(1)),
    CacheConfigMap::value_type("linefill_read_latency",CacheConfigItem(0)),
    CacheConfigMap::value_type("linefill_write_latency",CacheConfigItem(1)),
    CacheConfigMap::value_type("lock_latency",CacheConfigItem(1)),
    CacheConfigMap::value_type("read_hit_latency",CacheConfigItem(0)),
    CacheConfigMap::value_type("streaming_linefill",CacheConfigItem(1)),
    CacheConfigMap::value_type("taken_to_write_latency",CacheConfigItem(2)),
    CacheConfigMap::value_type("unlock_latency",CacheConfigItem(1)),
    CacheConfigMap::value_type("write_hit_latency",CacheConfigItem(1)),
    CacheConfigMap::value_type("write_mode",CacheConfigItem("WriteThrough")),
    CacheConfigMap::value_type("write_to_next_level_request_latency",CacheConfigItem(1)),
    CacheConfigMap::value_type("zero_latency",CacheConfigItem(1))
  };
  
  L1i_cache_t::L1i_cache_t(OSM_PMT &uarch, UadlMemoryIf &memory) :
  Cache(
      uarch,
      "L1i",
      memory,
      1,
      128,
      4, // address bandwidth
      4, // data bandwidth
      4, // max requests
      1, // allow bus wait
      0, // pre-emptive
      CacheConfigMap(&_sym958[0],&_sym958[19]),
      0  // with-data
    ),
    _uarch(uarch)
  {
  }
  
  void L1i_cache_t::reset_has_data()
  {
    if (_uarch.hadReset()) {
      // set up data array status if this is the first time step
      unsigned num_sets = _uarch._iss->t0->L1i_cache_get_sets();
      unsigned num_ways = _uarch._iss->t0->L1i_cache_get_ways();
      _hasData = HasData(num_sets,BVect(num_ways,false));
      for (unsigned set = 0; set < num_sets; set++) {
        for (unsigned way = 0; way < num_ways; way++) {
          _hasData[set][way] = _uarch._iss->t0->L1i_cache._array[set][way].valid;
        }
      }
    }
  }
  
  bool L1i_cache_t::lookup(addr_t addr, unsigned &set, unsigned &way, unsigned &offset) const
  {
    const bool hit = _uarch._iss->t0->L1i_cache_find(adl::CacheLogRead, (int&)set, (int&)way, addr);
    if (hit) offset = _uarch._iss->t0->L1i_cache_get_offset(addr);
    return hit;
  }
  
  addr_t L1i_cache_t::getTagAddr(unsigned set, unsigned way)
  {
    return _uarch._iss->t0->L1i_cache_get_tag(set, way);
  }
  
  void L1i_cache_t::resize() {
    unsigned num_sets = _uarch._iss->t0->L1i_cache_get_sets();
    unsigned num_ways = _uarch._iss->t0->L1i_cache_get_ways();
    _hasData = HasData(num_sets,std::vector<bool>(num_ways));
  }
  
  void L1i_cache_t::operationBegin(uadl::MemoryRequest &request)
  {
    Cache::operationBegin(request);
    
    if (_uarch._iss->t0->L1i_cache_begin(
            request.type, request.action,
            request.hint._cacheStatus,
            request.set, request.way, request.addr, request.replaceLine,
            hasCommand()
        ))
    {
      request.offset = _uarch._iss->t0->L1i_cache_get_offset(request.addr);
    } else {
      request.action = adl::CacheNone;
    }
    request.lookupDone = true;
  }
  
  void L1i_cache_t::operationEnd(const uadl::MemoryRequest &request) const
  {
    if (request.action != adl::CacheNone) {
      _uarch._iss->t0->L1i_cache_end(request.type,request.action, request.set, request.way,request.addr, hasCommand());
    }
  }
  
  
  void L1i_cache_t::clearValid(unsigned set, unsigned way)
  {
    _uarch._iss->t0->L1i_cache._array[set][way].valid = false;
  }
  
  bool L1i_cache_t::storeBufEnabled()
  {
    return _uarch._iss->t0->_sym361();
  }
  
  bool L1i_cache_t::is_write_through(addr_t addr)
  {
    return _uarch._iss->t0->L1i.is_write_through(adl::CacheWrite,addr);
  }
  
  addr_t Inst_PMT::addr() const
  {
    return _instr->_addr;
  }
  
  unsigned Inst_PMT::size() const
  {
    return _instr ? _instr->_size : 0;
  }
  
  unsigned Inst_PMT::instr_id() const
  {
    return _instr->_id;
  }
  
  int Inst_PMT::thread_id() const
  {
    return _uarch->getChildId();
  }
  
  bool Inst_PMT::has_exceptions() const
  {
    return _instr && _instr->has_exceptions();
  };
  
  std::pair<adl::BranchType,bool> Inst_PMT::branch_info(OSM_Thread&uarch) const
  {
    return _instr->branch_info(*uarch._iss);
  }
  
  bool Inst_PMT::is_branch(OSM_Thread&uarch) const
  {
    const pair<adl::BranchType, bool> x = _instr->branch_info(*uarch._iss);
    
    return x.first;
  }
  
  bool Inst_PMT::branch_taken(OSM_Thread&uarch) const
  {
    const pair<adl::BranchType, bool> x = _instr->branch_info(*uarch._iss);
    
    return (x.first && x.second);
  }
  
  addr_t Inst_PMT::branch_next_addr(OSM_Thread&uarch) const
  {
    if (branch_taken(uarch)) {
      return branch_target();
    } else {
      return addr()+size();
    }
  }
  
  inline const adl::OpInfo &Inst_PMT::op(unsigned index) const { return _instr->op(index); };
  
  inline const uint32_t *Inst_PMT::opc() const { return _instr->opc(); };
  
  unsigned OSM_Thread::_ic_attrs[] = {
    0, // unknownClass
    0x1, // branchClass
    0x8, // deadlockClass
    0x0, // decode_missClass
    0x0, // fetch_exceptionClass
    0x0, // lmwClass
    0x2, // loadClass
    0x10, // mtmsrClass
    0x10, // mtspr_hidClass
    0x4, // muldivClass
    0x0, // nopClass
    0x8, // sfxClass
    0x0, // stmwClass
    0x20, // storeClass
    0, // maxClass
  };
  
  unsigned OSM_Thread::getInstrClassAttrByName(const std::string &attr)
  {
    if (attr == "branch_instrs") return 0x1;
    if (attr == "load_instrs") return 0x2;
    if (attr == "muldiv_instrs") return 0x4;
    if (attr == "sfx_instrs") return 0x8;
    if (attr == "special_instrs") return 0x10;
    if (attr == "store_instrs") return 0x20;
    return 0;
  }
  
  void OSM_Thread::alloc_mem_access_data()
  {
    for (MemAccessPool::iterator iter = _mem_access_pool.begin(); iter != _mem_access_pool.end(); ++iter) {
      (*iter)->_hint = new CacheStatusHint;
    }
  }
  
  UadlMemAccess &OSM_Thread::add_memory_request(InstrBase &instr)
  {
    return *instr.mem();
  }
  
  void OSM_Thread::set_memory_result(InstrBase &instr)
  {
  }
  
  void OSM_Thread::free_mem(InstrBase &instr)
  {
    if (instr.mem()) {
      UadlMemAccess *ma = instr.get_mem_and_clear();
      _mem_access_pool.free(ma);
    }
  }
  
  void OSM_Thread::free_all_mem(InstrBase &instr)
  {
    if (instr.mem()) {
      _mem_access_pool.free_all(instr.mem(),instr.mtail());
    }
  }
  
  void OSM_Thread::handle_memory_request(unsigned memId,const adl::MemAccess &m)
  {
    if (!_log_mem) return;
    UadlMemAccess *ma = _mem_access_pool.alloc();
    CacheStatusHint *hint = ma->_hint;
    _iss->update_data_cache_status(hint->_cacheStatus,m._type,m._ra);
    hint->_storeBufEnabled = L1d_obj.storeBufEnabled();
    ma->reset(_memId++,m);
    _cur_instr->add_mem(ma);
  }
  
  static unsigned initFetchSizes[] = { 8, 4,  };
  FetchSizes OSM_Thread::_fetchSizes(&initFetchSizes[0],&initFetchSizes[2]);
  
  void OSM_Thread::branch_user_handler(InstrType & instr)
  {
    {
          set_speculative ( instr , ( instr . branch_taken ( thisCore (  ) ) != BP . last_predict_taken ( thisCore (  ) , instr ) ) ) ;
    }}
  
  //
  // User-defined helper functions.
  //
  
   bool OSM_Thread:: issueCheck ( InstrType & instr , unsigned position , unsigned attrs ) {
        if ( getChildId (  ) == 0 && position && ! top . t1 . Issue . empty (  ) ) return false ;
        return ( attrs ) ? ( mID . has_space (  ) && ! mID . has_attr ( * this , instr , 0 , 0 , attrs ) ) : true ;
  }
  
   bool OSM_Thread:: loadStoreIssueCheck ( InstrType & instr , unsigned position , unsigned attrs ) {
         bool ols = ( getChildId (  ) == 0 ) ? top . t1 . Issue . has_attr ( * this , instr , 0 , ( adliss::PMT::Thread::load_instrs | adliss::PMT::Thread::store_instrs ) ) : top . t0 . Issue . has_attr ( * this , instr , 0 , ( adliss::PMT::Thread::load_instrs | adliss::PMT::Thread::store_instrs ) ) ;
        if ( ols && ! _iss-> loadStoreProceed ( getChildId (  ) , 4 ) ) return false ;
         bool ok = ( attrs ) ? ( mID . has_space (  ) && ! mID . has_attr ( * this , instr , 0 , 0 , attrs ) ) : true ;
        if ( ok ) {
              _iss-> loadStoreIncr ( 4 , 2 ) ;
          }
        return ok ;
  }
  
  // Factory function for instruction "add".
  Inst_PMT *OSM_Thread::get_instr_add(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<add_sfx_Src, add_sfx_Trg> *ic = (Inst_sfx<add_sfx_Src, add_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "addi".
  Inst_PMT *OSM_Thread::get_instr_addi(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addi_sfx_Src, addi_sfx_Trg> *ic = (Inst_sfx<addi_sfx_Src, addi_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "addic".
  Inst_PMT *OSM_Thread::get_instr_addic(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addic_sfx_Src, addic_sfx_Trg> *ic = (Inst_sfx<addic_sfx_Src, addic_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::sfxClass,p);
    ic->_masks[0] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "addic.".
  Inst_PMT *OSM_Thread::get_instr_addic_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addic__sfx_Src, addic__sfx_Trg> *ic = (Inst_sfx<addic__sfx_Src, addic__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    ic->_masks[1] = 0xf0000000;
    ic->_masks[2] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "addis".
  Inst_PMT *OSM_Thread::get_instr_addis(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addis_sfx_Src, addis_sfx_Trg> *ic = (Inst_sfx<addis_sfx_Src, addis_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "addme".
  Inst_PMT *OSM_Thread::get_instr_addme(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addme_sfx_Src, addme_sfx_Trg> *ic = (Inst_sfx<addme_sfx_Src, addme_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::sfxClass,p);
    ic->_masks[1] = 0x20000000;
    ic->_masks[0] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "addme.".
  Inst_PMT *OSM_Thread::get_instr_addme_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addme__sfx_Src, addme__sfx_Trg> *ic = (Inst_sfx<addme__sfx_Src, addme__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::sfxClass,p);
    ic->_masks[0] = 0xa0000000;
    ic->_masks[1] = 0xf0000000;
    ic->_masks[2] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "addmeo".
  Inst_PMT *OSM_Thread::get_instr_addmeo(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addmeo_sfx_Src, addmeo_sfx_Trg> *ic = (Inst_sfx<addmeo_sfx_Src, addmeo_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::sfxClass,p);
    ic->_masks[1] = 0xe0000000;
    ic->_masks[0] = 0xe0000000;
    return ic;
  }
  
  // Factory function for instruction "addo".
  Inst_PMT *OSM_Thread::get_instr_addo(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addo_sfx_Src, addo_sfx_Trg> *ic = (Inst_sfx<addo_sfx_Src, addo_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::sfxClass,p);
    ic->_masks[0] = 0xc0000000;
    ic->_masks[1] = 0xc0000000;
    return ic;
  }
  
  // Factory function for instruction "andi.".
  Inst_PMT *OSM_Thread::get_instr_andi_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<andi__sfx_Src, andi__sfx_Trg> *ic = (Inst_sfx<andi__sfx_Src, andi__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    ic->_masks[1] = 0xf0000000;
    return ic;
  }
  
  // Factory function for instruction "b".
  Inst_PMT *OSM_Thread::get_instr_b(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_branch<b_branch_Src, b_branch_Trg> *ic = (Inst_branch<b_branch_Src, b_branch_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::branchClass,p);
    return ic;
  }
  
  // Factory function for instruction "bc".
  Inst_PMT *OSM_Thread::get_instr_bc(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_branch<bc_branch_Src, bc_branch_Trg> *ic = (Inst_branch<bc_branch_Src, bc_branch_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::branchClass,p);
    return ic;
  }
  
  // Factory function for instruction "bcl".
  Inst_PMT *OSM_Thread::get_instr_bcl(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_bcl' is not implemented in this model.");
  }
  
  // Factory function for instruction "bclr".
  Inst_PMT *OSM_Thread::get_instr_bclr(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_branch<bclr_branch_Src, bclr_branch_Trg> *ic = (Inst_branch<bclr_branch_Src, bclr_branch_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::branchClass,p);
    return ic;
  }
  
  // Factory function for instruction "bclrl".
  Inst_PMT *OSM_Thread::get_instr_bclrl(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_bclrl' is not implemented in this model.");
  }
  
  // Factory function for instruction "bl".
  Inst_PMT *OSM_Thread::get_instr_bl(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_branch<bl_branch_Src, bl_branch_Trg> *ic = (Inst_branch<bl_branch_Src, bl_branch_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::branchClass,p);
    return ic;
  }
  
  // Factory function for instruction "cmp".
  Inst_PMT *OSM_Thread::get_instr_cmp(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<cmp_sfx_Src, cmp_sfx_Trg> *ic = (Inst_sfx<cmp_sfx_Src, cmp_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::sfxClass,p);
    ic->_masks[1] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "cmpi".
  Inst_PMT *OSM_Thread::get_instr_cmpi(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<cmpi_sfx_Src, cmpi_sfx_Trg> *ic = (Inst_sfx<cmpi_sfx_Src, cmpi_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "crxor".
  Inst_PMT *OSM_Thread::get_instr_crxor(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_deadlock<crxor_deadlock_Src, crxor_deadlock_Trg> *ic = (Inst_deadlock<crxor_deadlock_Src, crxor_deadlock_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::deadlockClass,p);
    return ic;
  }
  
  // Factory function for instruction "halt".
  Inst_PMT *OSM_Thread::get_instr_halt(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<DummyOp, DummyOp> *ic = (Inst_sfx<DummyOp, DummyOp>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "lbz".
  Inst_PMT *OSM_Thread::get_instr_lbz(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lbz_load_Src, lbz_load_Trg, DummyOp> *ic = (Inst_load<lbz_load_Src, lbz_load_Trg, DummyOp>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::loadClass,p);
    return ic;
  }
  
  // Factory function for instruction "lbzu".
  Inst_PMT *OSM_Thread::get_instr_lbzu(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_lbzu' is not implemented in this model.");
  }
  
  // Factory function for instruction "lhz".
  Inst_PMT *OSM_Thread::get_instr_lhz(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lhz_load_Src, lhz_load_Trg, DummyOp> *ic = (Inst_load<lhz_load_Src, lhz_load_Trg, DummyOp>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::loadClass,p);
    return ic;
  }
  
  // Factory function for instruction "lmw".
  Inst_PMT *OSM_Thread::get_instr_lmw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_lmw<lmw_lmw_Src, lmw_lmw_Trg> *ic = (Inst_lmw<lmw_lmw_Src, lmw_lmw_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::lmwClass,p);
    return ic;
  }
  
  // Factory function for instruction "lwz".
  Inst_PMT *OSM_Thread::get_instr_lwz(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lwz_load_Src, lwz_load_Trg, DummyOp> *ic = (Inst_load<lwz_load_Src, lwz_load_Trg, DummyOp>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::loadClass,p);
    return ic;
  }
  
  // Factory function for instruction "lwzu".
  Inst_PMT *OSM_Thread::get_instr_lwzu(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lwzu_load_Src, lwzu_load_Trg, lwzu_load_TrgEA> *ic = (Inst_load<lwzu_load_Src, lwzu_load_Trg, lwzu_load_TrgEA>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::loadClass,p);
    return ic;
  }
  
  // Factory function for instruction "lwzx".
  Inst_PMT *OSM_Thread::get_instr_lwzx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lwzx_load_Src, lwzx_load_Trg, DummyOp> *ic = (Inst_load<lwzx_load_Src, lwzx_load_Trg, DummyOp>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::loadClass,p);
    return ic;
  }
  
  // Factory function for instruction "mfspr".
  Inst_PMT *OSM_Thread::get_instr_mfspr(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<mfspr_sfx_Src, mfspr_sfx_Trg> *ic = (Inst_sfx<mfspr_sfx_Src, mfspr_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "mtmsr".
  Inst_PMT *OSM_Thread::get_instr_mtmsr(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_mtmsr<mtmsr_mtmsr_Src, mtmsr_mtmsr_Trg> *ic = (Inst_mtmsr<mtmsr_mtmsr_Src, mtmsr_mtmsr_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::mtmsrClass,p);
    return ic;
  }
  
  // Factory function for instruction "mtspr".
  Inst_PMT *OSM_Thread::get_instr_mtspr(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    if ((_iss->get_SPRN(p->_opc[0]) == 51)) {
      Inst_mtspr_hid<mtspr_mtspr_hid_Src, mtspr_mtspr_hid_Trg> *ic = (Inst_mtspr_hid<mtspr_mtspr_hid_Src, mtspr_mtspr_hid_Trg>*)_instr_pool.alloc();
      ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::mtspr_hidClass,p);
      return ic;
    }
    if ((_iss->get_SPRN(p->_opc[0]) == 50)) {
      Inst_mtspr_hid<mtspr_mtspr_hid_Src, mtspr_mtspr_hid_Trg> *ic = (Inst_mtspr_hid<mtspr_mtspr_hid_Src, mtspr_mtspr_hid_Trg>*)_instr_pool.alloc();
      ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::mtspr_hidClass,p);
      return ic;
    }
    Inst_sfx<mtspr_sfx_Src, mtspr_sfx_Trg> *ic = (Inst_sfx<mtspr_sfx_Src, mtspr_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "mulli".
  Inst_PMT *OSM_Thread::get_instr_mulli(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_muldiv<mulli_muldiv_Src, mulli_muldiv_Trg> *ic = (Inst_muldiv<mulli_muldiv_Src, mulli_muldiv_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::muldivClass,p);
    return ic;
  }
  
  // Factory function for instruction "mullw".
  Inst_PMT *OSM_Thread::get_instr_mullw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_muldiv<mullw_muldiv_Src, mullw_muldiv_Trg> *ic = (Inst_muldiv<mullw_muldiv_Src, mullw_muldiv_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::muldivClass,p);
    return ic;
  }
  
  // Factory function for instruction "or".
  Inst_PMT *OSM_Thread::get_instr_or(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<or_sfx_Src, or_sfx_Trg> *ic = (Inst_sfx<or_sfx_Src, or_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "ori".
  Inst_PMT *OSM_Thread::get_instr_ori(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    if ((_iss->get_RA(p->_opc[0]) == 0) && (_iss->get_RS(p->_opc[0]) == 0) && (_iss->get_UI(p->_opc[0]) == 0)) {
      Inst_nop<ori_nop___other__> *ic = (Inst_nop<ori_nop___other__>*)_instr_pool.alloc();
      ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::nopClass,p);
      return ic;
    }
    Inst_sfx<ori_sfx_Src, ori_sfx_Trg> *ic = (Inst_sfx<ori_sfx_Src, ori_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "oris".
  Inst_PMT *OSM_Thread::get_instr_oris(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<oris_sfx_Src, oris_sfx_Trg> *ic = (Inst_sfx<oris_sfx_Src, oris_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "rfi".
  Inst_PMT *OSM_Thread::get_instr_rfi(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_rfi' is not implemented in this model.");
  }
  
  // Factory function for instruction "rlwinm".
  Inst_PMT *OSM_Thread::get_instr_rlwinm(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<rlwinm_sfx_Src, rlwinm_sfx_Trg> *ic = (Inst_sfx<rlwinm_sfx_Src, rlwinm_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "sc".
  Inst_PMT *OSM_Thread::get_instr_sc(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_sc' is not implemented in this model.");
  }
  
  // Factory function for instruction "srw".
  Inst_PMT *OSM_Thread::get_instr_srw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<srw_sfx_Src, srw_sfx_Trg> *ic = (Inst_sfx<srw_sfx_Src, srw_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::sfxClass,p);
    ic->_masks[0] = 0x1f;
    return ic;
  }
  
  // Factory function for instruction "stb".
  Inst_PMT *OSM_Thread::get_instr_stb(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<stb_store_Src, DummyOp> *ic = (Inst_store<stb_store_Src, DummyOp>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::storeClass,p);
    return ic;
  }
  
  // Factory function for instruction "sth".
  Inst_PMT *OSM_Thread::get_instr_sth(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<sth_store_Src, DummyOp> *ic = (Inst_store<sth_store_Src, DummyOp>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::storeClass,p);
    return ic;
  }
  
  // Factory function for instruction "sthx".
  Inst_PMT *OSM_Thread::get_instr_sthx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<sthx_store_Src, DummyOp> *ic = (Inst_store<sthx_store_Src, DummyOp>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::storeClass,p);
    return ic;
  }
  
  // Factory function for instruction "stmw".
  Inst_PMT *OSM_Thread::get_instr_stmw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_stmw<stmw_stmw_Src, stmw_stmw_SrcCR, DummyOp> *ic = (Inst_stmw<stmw_stmw_Src, stmw_stmw_SrcCR, DummyOp>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::stmwClass,p);
    ic->_masks[0] = 0xffffffff;
    ic->_masks[1] = 0xffffffff;
    ic->_masks[2] = 0xffffffff;
    return ic;
  }
  
  // Factory function for instruction "stw".
  Inst_PMT *OSM_Thread::get_instr_stw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<stw_store_Src, DummyOp> *ic = (Inst_store<stw_store_Src, DummyOp>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::storeClass,p);
    return ic;
  }
  
  // Factory function for instruction "stwu".
  Inst_PMT *OSM_Thread::get_instr_stwu(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<stwu_store_Src, stwu_store_TrgEA> *ic = (Inst_store<stwu_store_Src, stwu_store_TrgEA>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::storeClass,p);
    return ic;
  }
  
  // Factory function for instruction "stwx".
  Inst_PMT *OSM_Thread::get_instr_stwx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<stwx_store_Src, DummyOp> *ic = (Inst_store<stwx_store_Src, DummyOp>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::storeClass,p);
    return ic;
  }
  
  // Factory function for instruction "decode_miss".
  Inst_PMT *OSM_Thread::get_instr_decode_miss(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_decode_miss<DummyOp, DummyOp> *ic = (Inst_decode_miss<DummyOp, DummyOp>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::Thread::decode_missClass,p);
    return ic;
  }
  
  // Factory function for instruction "fetch_exception".
  Inst_PMT *OSM_Thread::get_instr_fetch_exception(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_fetch_exception *ic = (Inst_fetch_exception*)_instr_pool.alloc();
    ic->reset(this,_issueId,_commits,adliss::PMT::Thread::fetch_exceptionClass,p);
    return ic;
  }
  
  void OSM_Thread::set_branch_target(addr_t addr) {
    _branch_target = addr;
    _cur_instr->set_branch_target(addr);
  }
  
  void OSM_Thread::signalException(Inst_PMT *instr) {
    if (Fetcher.logger()) Fetcher.logger()->logMessage("commiting exception");
    if (_speculative || !instr) return;
    instr->set_branch_target(get_pc());
    _speculative = true;
    _spec_tag = instr->iid();
  }
  
  addr_t OSM_Thread::get_pc() const {
    return _iss->NIA_read().uint64();
  }
  
  void OSM_Thread::set_pc(addr_t a) {
    _iss->NIA_write_simple(a);
  }
  
  void OSM_Thread::check_breakpoints(addr_t ea)
  {
    _iss->check_breakpoints(ea);
  };
  
  void OSM_Thread::clear_exceptions(Inst_PMT &instr)
  {
    if (!instr.is_fetch_exception()) {
      instr._instr->clear_exceptions();
    }
  }
  
  void OSM_Thread::commit_exceptions(Inst_PMT &instr,Logger *logger)
  {
    if (_speculative) return;
    if (logger) logger->logMessage("commiting exception");
    if (instr.is_fetch_exception()) {
      commit_fetch_exception(instr);
    } else {
      instr._instr->commit_exceptions(*_iss);
    }
    instr.set_branch_target(get_pc());
  };
  
  void OSM_Thread::cancel_all_mem(Inst_PMT &instr)
  {
    UadlMemAccess *ma = instr.mem();
    while (ma) {
      if (ma->valid_request_id()) {
        L1d_obj.cancel(ma->request_id(),true);
      }
      ma = ma->_next;
    }
  }
  
  void OSM_Thread::commit_fetch_exception(Inst_PMT&instr)
  {
    _fetchExceptionInProgress = false;
  }
  
  void OSM_Thread::exec_mEX(Inst_PMT&instr,Logger *logger,unsigned position)
  {
    exec(instr,logger);
  }
  
  void OSM_Thread::exec_mem_mEX(Inst_PMT&instr,Logger *logger,unsigned position)
  {
    exec_mem(instr,logger);
  }
  
  void OSM_Thread::exec_mID(Inst_PMT&instr,Logger *logger,unsigned position)
  {
    exec(instr,logger);
  }
  
  void OSM_Thread::exec_mem_mID(Inst_PMT&instr,Logger *logger,unsigned position)
  {
    exec_mem(instr,logger);
  }
  
  void OSM_Thread::exec_mMM(Inst_PMT&instr,Logger *logger,unsigned position)
  {
    exec(instr,logger);
  }
  
  void OSM_Thread::exec_mem_mMM(Inst_PMT&instr,Logger *logger,unsigned position)
  {
    exec_mem(instr,logger);
  }
  
  void OSM_Thread::exec_mWB(Inst_PMT&instr,Logger *logger,unsigned position)
  {
    exec(instr,logger);
  }
  
  void OSM_Thread::exec_mem_mWB(Inst_PMT&instr,Logger *logger,unsigned position)
  {
    exec_mem(instr,logger);
  }
  
  void OSM_Thread::flush_issue_stage(Logger *logger,unsigned position,bool given_addr,addr_t addr)
  {
    if (!Issue.empty()) {
      Inst_PMT*instr = Issue.instr();
      if (!instr->is_fetch_exception()) Fetcher.removeInstr(instr->_instr->size());
      Issue.remove_instr();
      discard(instr);
    }
    flush_issue(logger,given_addr,addr);
  }
  
  void OSM_Thread::flush_mWB(Inst_PMT&ic,Logger *logger,unsigned position)
  {
    _speculative = false;
    if (ic.iid() >= _resetFetchId) {
    }
    taken_flush_mWB(ic,logger,position);
  }
  
  void OSM_Thread::taken_flush_mWB(Inst_PMT&ic,Logger *logger,unsigned position,bool given_addr,addr_t addr)
  {
    _fetchException = 0;
    if (ic.iid() >= _resetFetchId) {
      if (!given_addr && ic._instr) {
      }
    }
    _speculative = false;
    unsigned orig_size = mWB.size();
    for (int i = position+1; i < 2; ++i) {
      if (Inst_PMT *instr = mWB.remove_instr(i,getChildId())) {
        instr->flush(*this,logger);
        discard(instr);
      }
    }
    mWB.cleanup(orig_size);
    {
      unsigned orig_size = mMM.size();
      for (unsigned i = 0; i != 2; ++i) {
        if (Inst_PMT *instr = mMM[i]) {
          if (mMM.remove_instr(i,getChildId())) {
            (instr->_flush)(instr,*this,logger);
            discard(instr);
          }
        }
      }
      mMM.cleanup(orig_size);
    }
    {
      unsigned orig_size = mEX.size();
      for (unsigned i = 0; i != 2; ++i) {
        if (Inst_PMT *instr = mEX[i]) {
          if (mEX.remove_instr(i,getChildId())) {
            (instr->_flush)(instr,*this,logger);
            discard(instr);
          }
        }
      }
      mEX.cleanup(orig_size);
    }
    {
      unsigned orig_size = mID.size();
      for (unsigned i = 0; i != 2; ++i) {
        if (Inst_PMT *instr = mID[i]) {
          if (mID.remove_instr(i,getChildId())) {
            (instr->_flush)(instr,*this,logger);
            Fetcher.removeInstr(instr->size());
            discard(instr);
          }
        }
      }
      mID.cleanup(orig_size);
    }
    flush_issue_stage(logger,position,given_addr,addr);
  }
  
  void OSM_Thread::flush_mMM(Inst_PMT&ic,Logger *logger,unsigned position)
  {
    _speculative = false;
    if (ic.iid() >= _resetFetchId) {
    }
    taken_flush_mMM(ic,logger,position);
  }
  
  void OSM_Thread::taken_flush_mMM(Inst_PMT&ic,Logger *logger,unsigned position,bool given_addr,addr_t addr)
  {
    _fetchException = 0;
    if (ic.iid() >= _resetFetchId) {
      if (!given_addr && ic._instr) {
      }
    }
    _speculative = false;
    unsigned orig_size = mMM.size();
    for (int i = position+1; i < 2; ++i) {
      if (Inst_PMT *instr = mMM.remove_instr(i,getChildId())) {
        instr->flush(*this,logger);
        discard(instr);
      }
    }
    mMM.cleanup(orig_size);
    {
      unsigned orig_size = mEX.size();
      for (unsigned i = 0; i != 2; ++i) {
        if (Inst_PMT *instr = mEX[i]) {
          if (mEX.remove_instr(i,getChildId())) {
            (instr->_flush)(instr,*this,logger);
            discard(instr);
          }
        }
      }
      mEX.cleanup(orig_size);
    }
    {
      unsigned orig_size = mID.size();
      for (unsigned i = 0; i != 2; ++i) {
        if (Inst_PMT *instr = mID[i]) {
          if (mID.remove_instr(i,getChildId())) {
            (instr->_flush)(instr,*this,logger);
            Fetcher.removeInstr(instr->size());
            discard(instr);
          }
        }
      }
      mID.cleanup(orig_size);
    }
    flush_issue_stage(logger,position,given_addr,addr);
  }
  
  void OSM_Thread::flush_mEX(Inst_PMT&ic,Logger *logger,unsigned position)
  {
    _speculative = false;
    if (ic.iid() >= _resetFetchId) {
    }
    taken_flush_mEX(ic,logger,position);
  }
  
  void OSM_Thread::taken_flush_mEX(Inst_PMT&ic,Logger *logger,unsigned position,bool given_addr,addr_t addr)
  {
    _fetchException = 0;
    if (ic.iid() >= _resetFetchId) {
      if (!given_addr && ic._instr) {
      }
    }
    _speculative = false;
    unsigned orig_size = mEX.size();
    for (int i = position+1; i < 2; ++i) {
      if (Inst_PMT *instr = mEX.remove_instr(i,getChildId())) {
        instr->flush(*this,logger);
        discard(instr);
      }
    }
    mEX.cleanup(orig_size);
    {
      unsigned orig_size = mID.size();
      for (unsigned i = 0; i != 2; ++i) {
        if (Inst_PMT *instr = mID[i]) {
          if (mID.remove_instr(i,getChildId())) {
            (instr->_flush)(instr,*this,logger);
            Fetcher.removeInstr(instr->size());
            discard(instr);
          }
        }
      }
      mID.cleanup(orig_size);
    }
    flush_issue_stage(logger,position,given_addr,addr);
  }
  
  void OSM_Thread::flush_mID(Inst_PMT&ic,Logger *logger,unsigned position)
  {
    _speculative = false;
    if (ic.iid() >= _resetFetchId) {
    }
    taken_flush_mID(ic,logger,position);
  }
  
  void OSM_Thread::taken_flush_mID(Inst_PMT&ic,Logger *logger,unsigned position,bool given_addr,addr_t addr)
  {
    _fetchException = 0;
    if (ic.iid() >= _resetFetchId) {
      if (!given_addr && ic._instr) {
      }
    }
    _speculative = false;
    unsigned orig_size = mID.size();
    position = 0;
    for (int i = position+1; i < 2; ++i) {
      if (Inst_PMT *instr = mID.remove_instr(i,getChildId())) {
        instr->flush(*this,logger);
        Fetcher.removeInstr(instr->size());
        discard(instr);
      }
    }
    mID.cleanup(orig_size);
    flush_issue_stage(logger,position,given_addr,addr);
  }
  
  void OSM_Thread::taken_flush_Issue(Inst_PMT&,Logger *logger,unsigned,bool given_addr,addr_t addr)
  {
    flush_issue(logger,given_addr,addr);
  }
  
  void OSM_Thread::flush_issue(Logger *logger,bool given_addr,addr_t addr)
  {
    if (logger) logger->logFlush("pipeline");
    _iss->log_flush_pending();
    Fetcher.flush((given_addr) ? addr : get_pc());
    ++_flushCount;
    _fetchExceptionInProgress = false;
  }
  
  bool OSM_Thread::hasExternException()
  {
    return _iss->hasCurrentPendingExceptions();
  }
  
  bool OSM_Thread::hasPendingExternException()
  {
    return _iss->hasPendingExceptions();
  }
  
  void OSM_Thread::genExceptions(unsigned flag)
  {
    _iss->setPendingExceptions(flag);
    _iss->setExternException(flag);
  }
  
  void OSM_Thread::cancelExceptions(unsigned flag)
  {
    _iss->cancelExceptions(flag);
  }
  
  void OSM_Thread::raiseExternException()
  {
    // logging hack- we want to immediately see the logging effects.
    _iss->log_direct(true);
    _iss->genExceptions(_iss->getExceptionFlag());
    _iss->clearEdgeSensitivePendingExceptions();
    _iss->log_direct(false);
    flush_issue(Fetcher.logger(),false,0);
  }
  
  void OSM_Thread::log_instr_read(addr_t ea,addr_t ra) {
    _iss->log_instr_read(ea,ra);
  }
  bool OSM_Thread::issueInquire()
  {
    return Issue.empty() && mID.has_space();
  }
  
  Inst_PMT *OSM_Thread::issueInstruction()
  {
    Inst_PMT *instr = 0;
    
    if (FetchData fd = Fetcher.instrData(_iss->tracing_on())) {
      instr = _iss->decode(fd._ea,fd._size,_speculative,*this);
      if (!instr) return 0;
      if (is_executing(*instr)) {
        incrCommitCount();
        try {
          check_breakpoints(get_pc());
        }
        catch (SimInterrupt) {
          setBreakpoint();
        }
      }
    } else if (!Fetcher.empty()) {
      return 0;
    }
    else if (hasFetchException() && !fetchExceptionInProgress()) {
      if (!_speculative) {
        instr = _iss->decode(Fetcher.nextFetchAddr(),0,_speculative,*this);
      }
      _fetchExceptionInProgress = true;
      Inst_PMT *fe = get_instr_fetch_exception(0);
      _fetchLogData = _iss->log_get_pending_queue();
      fe->set_fetch_exception(_fetchException,_fetchExceptionHandle,Fetcher.nextFetchAddr(),_fetchLogData);
      _fetchLogData = 0;
      _fetchException = 0;
      return fe;
    }
    
    return instr;
  }
  
  void OSM_Thread::issue(InstrPacket *instr)
  {
    _iss->log_instr_issue_time(instr,currTime());
    Fetcher.removeInstr(instr->size());
  }
  
  void OSM_Thread::commit(Inst_PMT*instr)
  {
    _iss->free_packet(instr->_instr);
    free_instr(instr);
  }
  
  void OSM_Thread::commit_checked(Inst_PMT*instr)
  {
    if (instr->is_fetch_exception()) {
      _iss->log_commit(instr->_fedata);
      instr->discard_fetch_exception();
    } else {
      _iss->free_packet(instr->_instr);
    }
    free_instr(instr);
  }
  
  void OSM_Thread::discard(Inst_PMT*instr)
  {
    cancel_all_mem(*instr);
    free_all_mem(*instr);
    _iss->discard_packet(instr->_instr);
    free_instr(instr);
  }
  
  void OSM_Thread::reset_fetch()
  {
    if (_resetFetch) {
      _resetFetch = false;
      Fetcher.reset(get_pc());
    }
  }
  
  void OSM_Thread::force_reset_fetch()
  {
    Fetcher.reset(get_pc());
  }
  
  addr_t OSM_Thread::ea(addr_t addr) { return _iss->mask_ea(addr); }
  
  bool OSM_Thread::fetchTranslate(addr_t ea, addr_t &ra)
  {
    try {
      _iss->prefetch_translate(ra, ea);
      return true;
    }
    catch (adliss::PMT::Thread::Exception e) {
      return false;
    }
  }
  
  bool OSM_Thread::fetchTranslate(addr_t ea, addr_t &ra, unsigned frontPadSize, unsigned size,byte_t *data)
  {
    PrefetchReturnType prefetchStatus = adl::prNotActive;
      const uint64_t oldExceptionFlag = _iss->getExceptionFlag();
    
    try {
      if (frontPadSize) {
        addr_t addr = ea + frontPadSize;
        
        // Invalidate MMU cache to force a new TLB lookup.
        _iss->checkInstrPerms();
        prefetchStatus = _iss->prefetch_translate(addr, addr);
      }
      
      if (prefetchStatus != adl::prException) {
        switch (size) {
          case 8:
          do {
            prefetchStatus = _iss->prefetch<8, true>
            (data, ra, ea);
          } while (prefetchStatus == adl::prSkip);
          
          break;
          
          case 4:
          do {
            prefetchStatus = _iss->prefetch<4, true>
            (data, ra, ea);
          } while (prefetchStatus == adl::prSkip);
          
          break;
          
          default: assert(false); // should never reach here
        }
        
        assert(prefetchStatus != adl::prNotActive);
      }
      
    }
    catch (adliss::PMT::Thread::Exception e) {
      _fetchException = oldExceptionFlag ^ (unsigned)e;
      _iss->cancelExceptions(_fetchException);
      throw 0;
    }
    
    return true;
  }
  
  
  void OSM_Thread::updateInstrCacheStatus(CacheStatusHint &hint,addr_t addr)
  {
    _iss->update_instr_cache_status(hint._cacheStatus,CacheIFetch,addr);
  }
  
  bool OSM_Thread::fetchCacheEnabled(addr_t addr)
  {
    return _iss->L1i_cache_enabled(adl::CacheIFetch,addr);
  }
  
  bool OSM_Thread::issueInstructions()
  {
    if (currTime() < _startTime) return false;
    if (!Issue.empty() && Issue.instr()->action(*this,((Tracing && logPipeline()) ? Issue.logger(Fetcher.predTaken(),Fetcher.predTrg()): 0),0)) {
      Issue.remove_instr();
    }
    if (!hasExternException()) {
      if (_iss->is_active() && !fetchExceptionInProgress() && issueInquire()) {
        if ((Issue.add_instr(issueInstruction()))) {
          if (Issue.instr()->action(*this,((Tracing && logPipeline()) ? Issue.logger(Fetcher.predTaken(),Fetcher.predTrg()): 0),0)) {;
            Issue.remove_instr();
            return true;
          }
        } else {
        }
      }
    } else if (instr_alloc_count() == 0) {
      raiseExternException();
    }
    return false;
  }
  
  void OSM_Thread::update_tracing()
  {
    unsigned tflags ATTRIBUTE_UNUSED = (_out) ? LogAll : LogNone;
    _flags = top.tracing_flags();
    if (logFetch()) {
      Fetcher.createLogger(_logclient,tflags);
    } else {
      Fetcher.removeLogger();
    }
    if (logPipeline()) {
      Issue.createLogger(_logclient,tflags);
    } else {
      Issue.removeLogger();
    }
  }
  
  void OSM_Thread::report_and_clear_top(ostream &o,const string &pfx)
  {
    if (_speculative) {
      *_out << pfx << "[" << _iss->name() << " speculative on iid=" << dec << _spec_tag << "]\n";
    }
    if (_iss->hasPendingExceptions()) {
      *_out << pfx << "[external exception]\n";
    }
    Fetcher.report_and_clear(o,pfx);
    Issue.report_and_clear(o,pfx);
  }
  
  void OSM_Thread::report_and_clear_bottom(ostream &o,const string &pfx)
  {
  }
  
  adl::IssNode& OSM_Thread::iss() { return *_iss; }
  const adl::IssNode& OSM_Thread::iss() const { return *_iss; }
  ISS_PMT_Thread& OSM_Thread::get_iss() { return *_iss; }
  
  void OSM_Thread::preCycle()
  {
    _iss->updateLevelSensitiveExceptions();
    Fetcher.receiveInstrData();
    if (logPipeline()) {
      Fetcher.instrBufReport();
    }
  }
  
  void OSM_Thread::postCycle()
  {
    if (_iss->activeCount()) Fetcher.sendFetchRequests();
    incrTime();
  }
  
  OSM_Thread::~OSM_Thread() {
  }
  
  OSM_Thread::OSM_Thread(unsigned &id, unsigned &thread_id, std::ostream &out, ttime_t startTime, PipelineStage<2,0,OSM_Thread,Inst_PMT,1> &p_mEX, PipelineStage<2,0,OSM_Thread,Inst_PMT,1> &p_mID, PipelineStage<2,0,OSM_Thread,Inst_PMT,1> &p_mMM, PipelineStage<2,0,OSM_Thread,Inst_PMT,1> &p_mWB, Mem_mem_t &p_Mem_obj, L1d_cache_t &p_L1d_obj, L1i_cache_t &p_L1i_obj, RegFileResource<uint32_t,2> &p_BARs, RegResource<uint32_t> &p_BootAddr, RegResource<uint32_t> &p_CBTEST2, RegResource<uint32_t> &p_CCR, RegFileResource<uint32_t,2> &p_FOOs, RegResource<uint32_t> &p_HID0, RegResource<uint32_t> &p_LoadStoreCount, RegResource<uint32_t> &p_LoadStoreIndex, RegFileResource<uint32_t,2> &p_MSRs, RegFileResource<uint32_t,2> &p_NIAs, RegFileResource<uint32_t,2> &p_TCR, RegResource<uint32_t> &p_TEN, Semaphore &p_all_stall, Flag &p_hid_stall, ForwardingPath<Inst_PMT,1,0,6,32> &p_FwdEX, ForwardingPath<Inst_PMT,1,0,6,32> &p_FwdMM, bool &p__stall, unsigned &p__deadlockCount, OSM_PMT &top_p, ISS_PMT_Thread *iss) : 
  UadlArch(startTime,out), 
  _breakpoint(false), 
  _thread_id(thread_id++), 
  _iss(iss), 
  BP(*this,0,BPredEnableDefault(),8,
  2), 
  mEX(p_mEX), 
  mID(p_mID), 
  mMM(p_mMM), 
  mWB(p_mWB), 
  Mem_obj(p_Mem_obj), 
  Mem(Mem_obj), 
  _log_mem(true), 
  L1d_obj(p_L1d_obj), 
  L1d(L1d_obj), 
  L1i_obj(p_L1i_obj), 
  L1i(L1i_obj), 
  BARs(p_BARs), 
  BootAddr(p_BootAddr), 
  CBTEST1("CBTEST1"), 
  CBTEST2(p_CBTEST2), 
  CCR(p_CCR), 
  CIA("CIA"), 
  CR("CR"), 
  CTR("CTR"), 
  ESR("ESR"), 
  FOO("FOO"), 
  FOOs(p_FOOs), 
  GPR("GPR"), 
  HID0(p_HID0), 
  HID1("HID1"), 
  IAR("IAR"), 
  IVOR4("IVOR4"), 
  IVOR7("IVOR7"), 
  IVPR("IVPR"), 
  LR("LR"), 
  LoadStoreCount(p_LoadStoreCount), 
  LoadStoreIndex(p_LoadStoreIndex), 
  MSR("MSR"), 
  MSRs(p_MSRs), 
  NIA("NIA"), 
  NIAs(p_NIAs), 
  PTN("PTN"), 
  SRR0("SRR0"), 
  SRR1("SRR1"), 
  STEST("STEST"), 
  STUFFs("STUFFs"), 
  TCR(p_TCR), 
  TEN(p_TEN), 
  UFOO("UFOO"), 
  XER("XER"), 
  XERU("XERU"), 
  all_stall(p_all_stall), 
  dead_flag("dead_flag",1), 
  hid_stall(p_hid_stall), 
  FwdEX(p_FwdEX), 
  FwdMM(p_FwdMM), 
  Fetcher(*this,getPath(_iss,".")+"Fetcher",22,L1i_obj,0,_fetchSizes,&Mem_obj), 
  _cur_instr(0), 
  _last_instr(0), 
  _speculative(false), 
  _branch_target(0), 
  _spec_tag(0), 
  Issue("         "), 
  _fetchException(0), 
  _fetchExceptionInProgress(false), 
  _fetchLogData(0), 
  _stall(p__stall), 
  _stage_loggers(8), 
  _deadlockCount(p__deadlockCount), 
  top(top_p)
  {
    alloc_mem_access_data();
    _iss->_uarch = this;
    _id_offset = 35 * _thread_id;
    L1d_obj.set_level(0);
    L1i_obj.set_level(0);
  };
  
  void OSM_Thread::reset()
  {
    UadlArch::reset();
    _breakpoint = false;
    BP.reset();
    _mem_access_pool.reset();
    _log_mem = true;
    CBTEST1.reset();
    CIA.reset();
    CR.reset();
    CTR.reset();
    ESR.reset();
    FOO.reset();
    GPR.reset();
    HID1.reset();
    IAR.reset();
    IVOR4.reset();
    IVOR7.reset();
    IVPR.reset();
    LR.reset();
    MSR.reset();
    NIA.reset();
    PTN.reset();
    SRR0.reset();
    SRR1.reset();
    STEST.reset();
    STUFFs.reset();
    UFOO.reset();
    XER.reset();
    XERU.reset();
    dead_flag.reset();
    Fetcher.full_reset();
    _cur_instr = 0;
    _last_instr = 0;
    _speculative = false;
    _branch_target = 0;;
    _spec_tag = 0;
    Issue.reset();
    _instr_pool.reset();
    _fetchException = 0;
    _fetchExceptionInProgress = false;
    _fetchLogData = 0;
  }
  
  bool OSM_Thread::active() const
  {
    if (breakpoint()) return false;
    return (_iss->activeCount() || instr_alloc_count() || Mem_obj.isActive() || L1d_obj.isActive() || L1i_obj.isActive());
  }
  
  bool OSM_Thread::is_active() const
  {
    return active();
  }
  
  void OSM_Thread::setBreakpoint()
  {
    _breakpoint = true;
  }
  
  void OSM_Thread::clearBreakpoint()
  {
    _breakpoint = false;
  }
  
  bool OSM_Thread::breakpoint () const
  {
    return _breakpoint
    ;
  }
  
  bool OSM_Thread::hasBreakpoint() const
  {
    return breakpoint();
  }
  
  void OSM_Thread::preRun()
  {
    reset_fetch();
    _hadReset = false;
  }
  
  void OSM_Thread::postSim()
  {
  }
  
  unsigned OSM_Thread::fetchStalls() const { return Fetcher.fetchStalls(); }
  
  unsigned OSM_Thread::dmemStalls() const { return L1d_obj.memStalls(0); }
  
  unsigned OSM_Thread::imemStalls() const { return L1i_obj.memStalls(0); }
  
  unsigned OSM_PMT::_ic_attrs[] = {
    0, // unknownClass
    0x1, // branchClass
    0x8, // deadlockClass
    0x0, // decode_missClass
    0x0, // fetch_exceptionClass
    0x0, // lmwClass
    0x2, // loadClass
    0x10, // mtmsrClass
    0x10, // mtspr_hidClass
    0x4, // muldivClass
    0x0, // nopClass
    0x8, // sfxClass
    0x0, // stmwClass
    0x20, // storeClass
    0, // maxClass
  };
  
  unsigned OSM_PMT::getInstrClassAttrByName(const std::string &attr)
  {
    if (attr == "branch_instrs") return 0x1;
    if (attr == "load_instrs") return 0x2;
    if (attr == "muldiv_instrs") return 0x4;
    if (attr == "sfx_instrs") return 0x8;
    if (attr == "special_instrs") return 0x10;
    if (attr == "store_instrs") return 0x20;
    return 0;
  }
  
  bool OSM_PMT::issueInstructions()
  {
    bool i0;
    bool i1;
    do {
      i0 = t0.issueInstructions();
      i1 = t1.issueInstructions();
    } while (i0 || i1);
    return true;
  }
  
  uint64_t OSM_PMT::instrCount() const
  {
    return t0.instrCount() + t1.instrCount();
  }
  
  void OSM_PMT::update_tracing()
  {
    unsigned tflags ATTRIBUTE_UNUSED = (_out) ? LogAll : LogNone;
    t0.update_tracing();
    t1.update_tracing();
    if (logPipeline()) {
      mWB.createLogger(_logclient,tflags);
      _stage_loggers.set(0,mWB.logger(0));
      _stage_loggers.set(1,mWB.logger(1));
      mMM.createLogger(_logclient,tflags);
      _stage_loggers.set(2,mMM.logger(0));
      _stage_loggers.set(3,mMM.logger(1));
      mEX.createLogger(_logclient,tflags);
      _stage_loggers.set(4,mEX.logger(0));
      _stage_loggers.set(5,mEX.logger(1));
      mID.createLogger(_logclient,tflags);
      _stage_loggers.set(6,mID.logger(0));
      _stage_loggers.set(7,mID.logger(1));
    } else {
      mWB.removeLogger();
      mMM.removeLogger();
      mEX.removeLogger();
      mID.removeLogger();
      _stage_loggers.reset();
    }
    if (logMemory()) {
      L1d_obj.createLogger(_logclient,tflags);
    } else {
      L1d_obj.removeLogger();
    }
    if (logMemory()) {
      L1i_obj.createLogger(_logclient,tflags);
    } else {
      L1i_obj.removeLogger();
    }
    if (logMemory()) {
      Mem_obj.createLogger(_logclient,tflags);
    } else {
      Mem_obj.removeLogger();
    }
  }
  
  void OSM_PMT::report_and_clear(ostream &o,const string &pfx)
  {
    *_out << "time " << dec << currTime() << ":\n";
    report_and_clear_top(o,pfx);
    mID.report_and_clear(o,pfx);
    mEX.report_and_clear(o,pfx);
    mMM.report_and_clear(o,pfx);
    mWB.report_and_clear(o,pfx);
    *_out << '\n';
    report_and_clear_bottom(o,pfx);
  }
  
  void OSM_PMT::report_and_clear_top(ostream &o,const string &pfx)
  {
    t0.report_and_clear_top(o,pfx);
    t1.report_and_clear_top(o,pfx);
  }
  
  void OSM_PMT::report_and_clear_bottom(ostream &o,const string &pfx)
  {
    t0.report_and_clear_bottom(o,pfx);
    t1.report_and_clear_bottom(o,pfx);
    L1d_obj.report_and_clear(o,pfx);
    *_out << '\n';
    L1i_obj.report_and_clear(o,pfx);
    *_out << '\n';
    Mem_obj.report_and_clear(o,pfx);
    *_out << '\n';
  }
  
  void OSM_PMT::breakDeadlock()
  {
    if (!_failOnDeadlock) {
      for (unsigned i = 0; i != 2; ++i) {
        Logger *logger = ((Tracing && logPipeline()) ? _stage_loggers.get(0+mWB.headp()) : 0);
        if (Inst_PMT *instr = mWB.head()) {
          if (logger) logger->logMessage("discarding instruction to break deadlock.");
          instr->flush(*instr->_uarch,logger);
          mWB.remove_instr();
          instr->_uarch->discard(instr);
          _deadlockCount = 0;
          return;
        }
      }
      for (unsigned i = 0; i != 2; ++i) {
        Logger *logger = ((Tracing && logPipeline()) ? _stage_loggers.get(2+mMM.headp()) : 0);
        if (Inst_PMT *instr = mMM.head()) {
          if (logger) logger->logMessage("discarding instruction to break deadlock.");
          instr->flush(*instr->_uarch,logger);
          mMM.remove_instr();
          instr->_uarch->discard(instr);
          _deadlockCount = 0;
          return;
        }
      }
      for (unsigned i = 0; i != 2; ++i) {
        Logger *logger = ((Tracing && logPipeline()) ? _stage_loggers.get(4+mEX.headp()) : 0);
        if (Inst_PMT *instr = mEX.head()) {
          if (logger) logger->logMessage("discarding instruction to break deadlock.");
          instr->flush(*instr->_uarch,logger);
          mEX.remove_instr();
          instr->_uarch->discard(instr);
          _deadlockCount = 0;
          return;
        }
      }
      for (unsigned i = 0; i != 2; ++i) {
        Logger *logger = ((Tracing && logPipeline()) ? _stage_loggers.get(6+mID.headp()) : 0);
        if (Inst_PMT *instr = mID.head()) {
          if (logger) logger->logMessage("discarding instruction to break deadlock.");
          instr->flush(*instr->_uarch,logger);
          mID.remove_instr();
          instr->_uarch->discard(instr);
          _deadlockCount = 0;
          return;
        }
      }
    }
    handleDeadlock();
  }
  
  adl::IssNode& OSM_PMT::iss() { return *_iss; }
  const adl::IssNode& OSM_PMT::iss() const { return *_iss; }
  ISS_PMT& OSM_PMT::get_iss() { return *_iss; }
  
  void OSM_PMT::preCycle()
  {
    Mem_obj.preCycle();
    L1d_obj.preCycle();
    L1i_obj.preCycle();
    t0.preCycle();
    t1.preCycle();
  }
  
  void OSM_PMT::postCycle()
  {
    Mem_obj.postCycle();
    L1i_obj.postCycle();
    L1d_obj.postCycle();
    FwdEX.postCycle();
    FwdMM.postCycle();
    t0.postCycle();
    t1.postCycle();
    if (logMemory()) Mem_obj.logAllRequests();
    if (logMemory()) L1i_obj.logAllRequests();
    if (logMemory()) L1d_obj.logAllRequests();
    if (tracing_on() && _out) {
      report_and_clear(*_out,string());
    }
    incrTime();
  }
  
  void OSM_PMT::cycle()
  {
    try {
      preCycle();
      _stall = false;
      ++_deadlockCount;
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(0+mWB.pos(0)) : 0);
        bool r0 = (!mWB[0] || mWB[0]->action(*mWB[0]->_uarch,logger0,0));
        Logger *logger1 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(0+mWB.pos(1)) : 0);
        bool r1 = (!mWB[1] || mWB[1]->action(*mWB[1]->_uarch,logger1,1));
        if (r0 && r1) {
          if (mWB.head()) mWB.head()->move(*mWB.head()->_uarch,logger0,0);
          if (mWB.head()) mWB.head()->move(*mWB.head()->_uarch,logger1,1);
        } else {
          _stall = true;
        }
      }
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(2+mMM.pos(0)) : 0);
        bool r0 = (!mMM[0] || mMM[0]->action(*mMM[0]->_uarch,logger0,0));
        Logger *logger1 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(2+mMM.pos(1)) : 0);
        bool r1 = (!mMM[1] || mMM[1]->action(*mMM[1]->_uarch,logger1,1));
        if (r0 && r1) {
          if (mMM.head()) mMM.head()->move(*mMM.head()->_uarch,logger0,0);
          if (mMM.head()) mMM.head()->move(*mMM.head()->_uarch,logger1,1);
        } else {
          _stall = true;
        }
      }
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(4+mEX.pos(0)) : 0);
        bool r0 = (!mEX[0] || mEX[0]->action(*mEX[0]->_uarch,logger0,0));
        Logger *logger1 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(4+mEX.pos(1)) : 0);
        bool r1 = (!mEX[1] || mEX[1]->action(*mEX[1]->_uarch,logger1,1));
        if (r0 && r1) {
          if (mEX.head()) mEX.head()->move(*mEX.head()->_uarch,logger0,0);
          if (mEX.head()) mEX.head()->move(*mEX.head()->_uarch,logger1,1);
        } else {
          _stall = true;
        }
      }
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(6+mID.pos(0)) : 0);
        Logger *logger1 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(6+mID.pos(1)) : 0);
        if (!mID.empty()) {
          if (mID.head()->action(*mID.head()->_uarch,logger0,0)) {
            if (!mID.empty()) {
              if (mID.head()->action(*mID.head()->_uarch,logger1,1)) {
              } else {
                _stall = true;
              }
            }
          } else {
            _stall = true;
          }
        }
      }
      checkForDeadlock();
      issueInstructions();
      postCycle();
    }
    catch (SimInterrupt) {
      postCycle();
      throw;
    }
  }
  
  void OSM_PMT::simulate()
  {
    while (active() && (currTime() < _endTime )) {
      cycle();
    }
  }
  
  OSM_PMT::~OSM_PMT() {
    delete _iss;
  }
  
  OSM_PMT::OSM_PMT(unsigned &id, unsigned &thread_id, std::ostream &out, ttime_t startTime) : 
  UadlArch(startTime,out), 
  _iss(new ISS_PMT(*this,id)), 
  mEX(getPath(_iss,".") + "mEX"), 
  mID(getPath(_iss,".") + "mID"), 
  mMM(getPath(_iss,".") + "mMM"), 
  mWB(getPath(_iss,".") + "mWB"), 
  Mem_obj(*this, logWarnings()), 
  L1d_obj(*this,Mem_obj), 
  L1i_obj(*this,Mem_obj), 
  BARs("BARs"), 
  BootAddr("BootAddr"), 
  CBTEST2("CBTEST2"), 
  CCR("CCR"), 
  FOOs("FOOs"), 
  HID0("HID0"), 
  LoadStoreCount("LoadStoreCount"), 
  LoadStoreIndex("LoadStoreIndex"), 
  MSRs("MSRs"), 
  NIAs("NIAs"), 
  TCR("TCR"), 
  TEN("TEN"), 
  all_stall("all_stall",1), 
  hid_stall("hid_stall"), 
  FwdEX("FwdEX"), 
  FwdMM("FwdMM"), 
  _stall(false), 
  _stage_loggers(8), 
  _deadlockCount(0), 
  top(*this), 
  t0(id, thread_id, out, startTime, mEX, mID, mMM, mWB, Mem_obj, L1d_obj, L1i_obj, BARs, BootAddr, CBTEST2, CCR, FOOs, HID0, LoadStoreCount, LoadStoreIndex, MSRs, NIAs, TCR, TEN, all_stall, hid_stall, FwdEX, FwdMM, _stall, _deadlockCount, *this,static_cast<ISS_PMT_Thread*>(_iss->t0)), 
  t1(id, thread_id, out, startTime, mEX, mID, mMM, mWB, Mem_obj, L1d_obj, L1i_obj, BARs, BootAddr, CBTEST2, CCR, FOOs, HID0, LoadStoreCount, LoadStoreIndex, MSRs, NIAs, TCR, TEN, all_stall, hid_stall, FwdEX, FwdMM, _stall, _deadlockCount, *this,static_cast<ISS_PMT_Thread*>(_iss->t1))
  {
    _timeOut = 200;
    L1d_obj.set_level(0);
    L1i_obj.set_level(0);
  };
  
  void OSM_PMT::reset()
  {
    UadlArch::reset();
    mEX.reset();
    mID.reset();
    mMM.reset();
    mWB.reset();
    Mem_obj.reset();
    L1d_obj.reset();
    L1i_obj.reset();
    BARs.reset();
    BootAddr.reset();
    CBTEST2.reset();
    CCR.reset();
    FOOs.reset();
    HID0.reset();
    LoadStoreCount.reset();
    LoadStoreIndex.reset();
    MSRs.reset();
    NIAs.reset();
    TCR.reset();
    TEN.reset();
    all_stall.reset();
    hid_stall.reset();
    FwdEX.reset();
    FwdMM.reset();
    _stall = false;
    _timeOut = 200;
    _deadlockCount = 0;
    t0.reset();
    t1.reset();
  }
  
  bool OSM_PMT::active() const
  {
    if (breakpoint()) return false;
    return t0.active() || t1.active();
  }
  
  bool OSM_PMT::is_active() const
  {
    return active();
  }
  
  void OSM_PMT::setBreakpoint()
  {
    t0.setBreakpoint();
    t1.setBreakpoint();
  }
  
  void OSM_PMT::clearBreakpoint()
  {
    t0.clearBreakpoint();
    t1.clearBreakpoint();
  }
  
  bool OSM_PMT::breakpoint () const
  {
    return t0.breakpoint() || t1.breakpoint();
  }
  
  bool OSM_PMT::hasBreakpoint() const
  {
    return breakpoint();
  }
  
  void OSM_PMT::preRun()
  {
    t0.preRun();
    t1.preRun();
    L1d_obj.reset_has_data();
    L1i_obj.reset_has_data();
    _hadReset = false;
  }
  
  void OSM_PMT::proceed()
  {
    try {
      cycle();
    }
    catch (SimInterrupt) {
      setBreakpoint();
    }
  }
  
  void OSM_PMT::run(ttime_t endTime)
  {
    try {
      preRun();
      _endTime = endTime;
      simulate();
    }
    catch (SimInterrupt) {
      setBreakpoint();
    }
  }
  
  void OSM_PMT::run()
  {
    try {
      preRun();
      simulate();
    }
    catch (SimInterrupt) {
      setBreakpoint();
    }
  }
  
  void OSM_PMT::postSim()
  {
    t0.postSim();
    t1.postSim();
    L1i_obj.postSim();
    L1d_obj.postSim();
  }
  
  unsigned OSM_PMT::fetchStalls() const {
    return t0.fetchStalls() + t1.fetchStalls();
  }
  
  unsigned OSM_PMT::dmemStalls() const {
    return t0.dmemStalls() + t1.dmemStalls();
  }
  
  unsigned OSM_PMT::imemStalls() const {
    return t0.imemStalls() + t1.imemStalls();
  }
  
  UadlArch *createArch(const string &name, unsigned &id,UadlMemoryIf *memory = NULL,ttime_t startTime = 0)
  {
    if (memory) {
      cerr << "Warning:  An external memory was specified but the model was created to use internal memory.\n";}
    
    unsigned thread_id = 0;
    return new OSM_PMT(id,thread_id,std::cout,startTime);
  }
  
  
  
} // namespace uadl

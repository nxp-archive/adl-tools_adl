//
// ADL/Uadl version 3.17.2
//
// Cycle-accurate/approximate model generated for PMT
//

#ifdef UADL_SEPARATE_COMPILE
#  define ADL_HEADER
#endif
#include "dlx-mt3-iss.cc"
#define _OSM_Thread_IMPLEMENTATION_H
#include "dlx-mt3.h"

using namespace std;
using namespace adl;
using namespace adliss;




void adliss::PMT::SMT::Thread::synchronizeActivation()
{
  _uarch->force_reset_fetch();
}

uint32_t adliss::PMT::SMT::Thread::getMemoryData32(CacheAccess type, addr_t addr)
{
  byte_t data[4] = {0,0,0,0};
  
  _uarch->getMemoryData(type, addr, 4, data);
  return byte_read32(data, 0);
}

void adliss::PMT::SMT::Thread::handle_memory_request(unsigned memId, const adl::MemAccess &ma) {
  _uarch->handle_memory_request(memId,ma);
};

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
    
    template <typename T0>
    void read(UadlArch &uarch,InstrBase &ic,Logger *logger,T0 &t0) {
    }
    
    template <typename T0>
    bool read_avail(UadlArch &uarch,InstrBase &ic,Logger *logger,T0 &t0) {
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
  // Operands for instruction class cfx
  //
  
  // Operands:
  //   (src: GPR(RA))
  struct mulli_cfx_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
    }
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic);
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        if (tag == 0) {
          read(uarch,ic,logger);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name());
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(0)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
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
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(0);
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
  struct mulli_cfx_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
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
      ic._instr->commit(uarch.get_iss(),1);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(1);
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
  struct mullw_cfx_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
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
        case 0: ic._instr->read_operand(uarch.get_iss(),0);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(0)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
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
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(0);
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
  struct mullw_cfx_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
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
      ic._instr->commit(uarch.get_iss(),2);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(2)._index;
    }
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(2);
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
  // Instruction class cfx
  //
  template <class Src_t, class Trg_t>
  struct Inst_cfx : public Inst_PMT {
    
    typedef struct Inst_cfx<Src_t, Trg_t> ThisClass;
    
    Inst_cfx &thisInstr() { return *this; };
    
    Src_t Src;
    Trg_t Trg;
    void reset(OSM_Thread *uarch,unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_PMT::reset(iid,cid,ic);
      _uarch = uarch;
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      _stage = (PipelineStageBase*)1;
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
    
    bool change_state_S_CX0__S_CX1(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_CX0 -> S_CX1"); }
      _action = (StateFunc)action_S_CX1_p;
      _flush = (FlushFunc)flush_S_CX1_p;
      uarch.mCX0.remove_instr();
      _stage = &uarch.mCX1;
      uarch.mCX1.add_instr(this);
      return true;
    }
    
    bool change_state_S_CX0__S_CX0e(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_CX0 -> S_CX0e"); }
      _action = (StateFunc)action_S_CX0e_p;
      _flush = (FlushFunc)flush_S_CX0e_p;
      return action_S_CX0e(uarch,logger,position);
    }
    
    bool change_state_S_CX0e__S_CX1e(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_CX0e -> S_CX1e"); }
      _action = (StateFunc)action_S_CX1e_p;
      _flush = (FlushFunc)flush_S_CX1e_p;
      uarch.mCX0.remove_instr();
      _stage = &uarch.mCX1;
      uarch.mCX1.add_instr(this);
      return true;
    }
    
    bool change_state_S_CX1__S_CX2(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_CX1 -> S_CX2"); }
      _action = (StateFunc)action_S_CX2_p;
      _flush = (FlushFunc)flush_S_CX2_p;
      uarch.mCX1.remove_instr();
      _stage = &uarch.mCX2;
      uarch.mCX2.add_instr(this);
      return true;
    }
    
    bool change_state_S_CX1__S_CX1e(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_CX1 -> S_CX1e"); }
      _action = (StateFunc)action_S_CX1e_p;
      _flush = (FlushFunc)flush_S_CX1e_p;
      return action_S_CX1e(uarch,logger,position);
    }
    
    bool change_state_S_CX1e__S_CX2e(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_CX1e -> S_CX2e"); }
      _action = (StateFunc)action_S_CX2e_p;
      _flush = (FlushFunc)flush_S_CX2e_p;
      uarch.mCX1.remove_instr();
      _stage = &uarch.mCX2;
      uarch.mCX2.add_instr(this);
      return true;
    }
    
    bool change_state_S_CX2__S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_CX2 -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mCX2.remove_instr();
      _stage = &uarch.mWB;
      return true;
    }
    
    bool change_state_S_CX2__S_CX2e(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_CX2 -> S_CX2e"); }
      _action = (StateFunc)action_S_CX2e_p;
      _flush = (FlushFunc)flush_S_CX2e_p;
      return action_S_CX2e(uarch,logger,position);
    }
    
    bool change_state_S_CX2e__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_CX2e -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mCX2.remove_instr();
      _stage = &uarch.mWB;
      return true;
    }
    
    bool change_state_S_ID__S_CX0(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_CX0"); }
      _action = (StateFunc)action_S_CX0_p;
      _flush = (FlushFunc)flush_S_CX0_p;
      uarch.mID.remove_instr();
      _stage = &uarch.mCX0;
      uarch.mCX0.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_IDe"); }
      _action = (StateFunc)action_S_IDe_p;
      _flush = (FlushFunc)flush_S_IDe_p;
      return action_S_IDe(uarch,logger,position);
    }
    
    bool change_state_S_IDe__S_CX0e(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDe -> S_CX0e"); }
      _action = (StateFunc)action_S_CX0e_p;
      _flush = (FlushFunc)flush_S_CX0e_p;
      uarch.mID.remove_instr();
      _stage = &uarch.mCX0;
      uarch.mCX0.add_instr(this);
      return true;
    }
    
    bool change_state_S_IF__S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IF -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mIF.remove_instr();
      _stage = &uarch.mID;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_IF(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_IF"); }
      _action = (StateFunc)action_S_IF_p;
      _flush = (FlushFunc)flush_S_IF_p;
      uarch.issue(_instr);
      _stage = &uarch.mIF;
      uarch.mIF.add_instr(this);
      return true;
    }
    
    bool change_state_S_WB__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    bool change_state_S_WB__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    // State action S_CX0
    bool action_S_CX0(OSM_Thread &uarch,Logger *logger,unsigned position) {
      try {
        if ( Src.read_avail(uarch,*this,logger, uarch.FwdCB ) && uarch.mCX1.has_space(uarch,*this,logger) ) {
              uarch.exec(*this,logger) ;
              return change_state_S_CX0__S_CX1(uarch,logger,position) ;
        }}
      catch (adliss::PMT::SMT::Thread::Exception) {
        if (logger) logger->logMessage("exception occurred");
        flush_S_CX0(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mCX0(*this,logger,position,true,uarch.get_pc());
        return change_state_S_CX0__S_CX0e(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_CX0_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_CX0(uarch,logger,position);
    }
    
    // State action S_CX0e
    bool action_S_CX0e(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mCX1.has_space(uarch,*this,logger) ) {
            return change_state_S_CX0e__S_CX1e(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_CX0e_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_CX0e(uarch,logger,position);
    }
    
    // State action S_CX1
    bool action_S_CX1(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mCX2.has_space(uarch,*this,logger) ) {
            return change_state_S_CX1__S_CX2(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_CX1_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_CX1(uarch,logger,position);
    }
    
    // State action S_CX1e
    bool action_S_CX1e(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mCX2.has_space(uarch,*this,logger) ) {
            return change_state_S_CX1e__S_CX2e(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_CX1e_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_CX1e(uarch,logger,position);
    }
    
    // State action S_CX2
    bool action_S_CX2(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.no_capacity_check(uarch,*this,logger) ) {
            return change_state_S_CX2__S_WB(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_CX2_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_CX2(uarch,logger,position);
    }
    
    // State action S_CX2e
    bool action_S_CX2e(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger) ) {
            return change_state_S_CX2e__S_WBe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_CX2e_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_CX2e(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      try {
        if ( Trg.can_write(uarch,*this,logger) && uarch.mWB.has_space(uarch,*this,logger) && uarch.mCX0.has_space(uarch,*this,logger) ) {
              Src.record_sources(uarch,*this,logger) ;
              Trg.allocate(uarch,*this,logger) ;
              uarch.mWB.add_instr(uarch,*this,logger) ;
              return change_state_S_ID__S_CX0(uarch,logger,position) ;
        }}
      catch (adliss::PMT::SMT::Thread::Exception) {
        if (logger) logger->logMessage("exception occurred");
        flush_S_ID(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mID(*this,logger,position,true,uarch.get_pc());
        return change_state_S_ID__S_IDe(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_ID_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_IDe
    bool action_S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mCX0.has_space(uarch,*this,logger) ) {
            return change_state_S_IDe__S_CX0e(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_IDe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IDe(uarch,logger,position);
    }
    
    // State action S_IF
    bool action_S_IF(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.has_space(uarch,*this,logger) && uarch. OSM_Thread::smtIssueCheck ( thisInstr (  ) ) ) {
            return change_state_S_IF__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_IF_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IF(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mIF.has_space(uarch,*this,logger) ) {
            return change_state_S_INIT__S_IF(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      try {
        {
              uarch.FwdCB.write(uarch,*this,logger, Trg ) ;
              write_ops(uarch,*this,logger) ;
              return change_state_S_WB__S_INIT(uarch,logger,position) ;
        }}
      catch (adliss::PMT::SMT::Thread::Exception) {
        if (logger) logger->logMessage("exception occurred");
        flush_S_WB(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mWB(*this,logger,position,true,uarch.get_pc());
        return change_state_S_WB__S_WBe(uarch,logger,position);
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
    
    // Flush action S_CX0
    void flush_S_CX0(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_CX0_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_CX0(uarch,logger);
    }
    
    // Flush action S_CX0e
    void flush_S_CX0e(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_CX0e_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_CX0e(uarch,logger);
    }
    
    // Flush action S_CX1
    void flush_S_CX1(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_CX1_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_CX1(uarch,logger);
    }
    
    // Flush action S_CX1e
    void flush_S_CX1e(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_CX1e_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_CX1e(uarch,logger);
    }
    
    // Flush action S_CX2
    void flush_S_CX2(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_CX2_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_CX2(uarch,logger);
    }
    
    // Flush action S_CX2e
    void flush_S_CX2e(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_CX2e_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_CX2e(uarch,logger);
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
    
    // Flush action S_IF
    void flush_S_IF(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_IF_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IF(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
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
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
    }
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic);
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        if (tag == 0) {
          read(uarch,ic,logger);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name());
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(0)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
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
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(0);
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
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
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
      ic._instr->commit(uarch.get_iss(),1);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(1);
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
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
    }
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic);
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        if (tag == 0) {
          read(uarch,ic,logger);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name());
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(0)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
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
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(0);
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
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
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
      ic._instr->commit(uarch.get_iss(),0);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      ic._instr->commit(uarch.get_iss(),1);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(0);
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
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  struct lwzx_load_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
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
        case 0: ic._instr->read_operand(uarch.get_iss(),0);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(0)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
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
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(0);
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
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
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
      ic._instr->commit(uarch.get_iss(),2);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(2)._index;
    }
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(2);
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
  template <class Src_t, class Trg_t>
  struct Inst_load : public Inst_PMT {
    
    typedef struct Inst_load<Src_t, Trg_t> ThisClass;
    
    Inst_load &thisInstr() { return *this; };
    
    Src_t Src;
    Trg_t Trg;
    void reset(OSM_Thread *uarch,unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_PMT::reset(iid,cid,ic);
      _uarch = uarch;
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      _stage = (PipelineStageBase*)1;
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
    
    bool exec_done() const { return _status == adl::MemAccess::opNone; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool change_state_S_ID__S_LS0(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_LS0"); }
      _action = (StateFunc)action_S_LS0_p;
      _flush = (FlushFunc)flush_S_LS0_p;
      uarch.mID.remove_instr();
      _stage = &uarch.mLS0;
      uarch.mLS0.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_IDe"); }
      _action = (StateFunc)action_S_IDe_p;
      _flush = (FlushFunc)flush_S_IDe_p;
      return action_S_IDe(uarch,logger,position);
    }
    
    bool change_state_S_IDe__S_LS0e(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDe -> S_LS0e"); }
      _action = (StateFunc)action_S_LS0e_p;
      _flush = (FlushFunc)flush_S_LS0e_p;
      uarch.mID.remove_instr();
      _stage = &uarch.mLS0;
      uarch.mLS0.add_instr(this);
      return true;
    }
    
    bool change_state_S_IF__S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IF -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mIF.remove_instr();
      _stage = &uarch.mID;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_IF(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_IF"); }
      _action = (StateFunc)action_S_IF_p;
      _flush = (FlushFunc)flush_S_IF_p;
      uarch.issue(_instr);
      _stage = &uarch.mIF;
      uarch.mIF.add_instr(this);
      return true;
    }
    
    bool change_state_S_LS0__S_LS1(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_LS0 -> S_LS1"); }
      _action = (StateFunc)action_S_LS1_p;
      _flush = (FlushFunc)flush_S_LS1_p;
      uarch.mLS0.remove_instr();
      _stage = &uarch.mLS1;
      uarch.mLS1.add_instr(this);
      return true;
    }
    
    bool change_state_S_LS0__S_LS0e(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_LS0 -> S_LS0e"); }
      _action = (StateFunc)action_S_LS0e_p;
      _flush = (FlushFunc)flush_S_LS0e_p;
      return action_S_LS0e(uarch,logger,position);
    }
    
    bool change_state_S_LS0e__S_LS1e(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_LS0e -> S_LS1e"); }
      _action = (StateFunc)action_S_LS1e_p;
      _flush = (FlushFunc)flush_S_LS1e_p;
      uarch.mLS0.remove_instr();
      _stage = &uarch.mLS1;
      uarch.mLS1.add_instr(this);
      return true;
    }
    
    bool change_state_S_LS1__S_LS1p(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_LS1 -> S_LS1p"); }
      _action = (StateFunc)action_S_LS1p_p;
      _flush = (FlushFunc)flush_S_LS1p_p;
      return action_S_LS1p(uarch,logger,position);
    }
    
    bool change_state_S_LS1__S_LS1e(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_LS1 -> S_LS1e"); }
      _action = (StateFunc)action_S_LS1e_p;
      _flush = (FlushFunc)flush_S_LS1e_p;
      return action_S_LS1e(uarch,logger,position);
    }
    
    bool change_state_S_LS1e__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_LS1e -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mLS1.remove_instr();
      _stage = &uarch.mWB;
      return true;
    }
    
    bool change_state_S_LS1p__S_LS1(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_LS1p -> S_LS1"); }
      _action = (StateFunc)action_S_LS1_p;
      _flush = (FlushFunc)flush_S_LS1_p;
      return action_S_LS1(uarch,logger,position);
    }
    
    bool change_state_S_LS1p__S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_LS1p -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mLS1.remove_instr();
      _stage = &uarch.mWB;
      return true;
    }
    
    bool change_state_S_WB__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      uarch.commit(this);
      return true;
    }
    
    bool change_state_S_WB__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      uarch.commit(this);
      return true;
    }
    
    // State action S_ID
    bool action_S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      try {
        if ( Trg.can_write(uarch,*this,logger) && uarch.mWB.has_space(uarch,*this,logger) && uarch.mLS0.has_space(uarch,*this,logger) ) {
              Src.record_sources(uarch,*this,logger) ;
              Trg.allocate(uarch,*this,logger) ;
              uarch.mWB.add_instr(uarch,*this,logger) ;
              return change_state_S_ID__S_LS0(uarch,logger,position) ;
        }}
      catch (adliss::PMT::SMT::Thread::Exception) {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_ID(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mID(*this,logger,position,true,uarch.get_pc());
        return change_state_S_ID__S_IDe(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_ID_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_IDe
    bool action_S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mLS0.has_space(uarch,*this,logger) ) {
            return change_state_S_IDe__S_LS0e(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_IDe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IDe(uarch,logger,position);
    }
    
    // State action S_IF
    bool action_S_IF(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.has_space(uarch,*this,logger) && uarch. OSM_Thread::smtIssueCheck ( thisInstr (  ) ) ) {
            return change_state_S_IF__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_IF_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IF(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mIF.has_space(uarch,*this,logger) ) {
            return change_state_S_INIT__S_IF(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_LS0
    bool action_S_LS0(OSM_Thread &uarch,Logger *logger,unsigned position) {
      try {
        if ( Src.read_avail(uarch,*this,logger, uarch.FwdCB ) && uarch.mLS1.has_space(uarch,*this,logger) && uarch.Mem.can_request_read(uarch,*this,logger) ) {
              uarch.exec_mem(*this,logger) ;
              uarch.Mem.send_read_request(uarch,*this,logger) ;
              return change_state_S_LS0__S_LS1(uarch,logger,position) ;
        }}
      catch (adliss::PMT::SMT::Thread::Exception) {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_LS0(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mLS0(*this,logger,position,true,uarch.get_pc());
        return change_state_S_LS0__S_LS0e(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_LS0_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_LS0(uarch,logger,position);
    }
    
    // State action S_LS0e
    bool action_S_LS0e(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mLS1.has_space(uarch,*this,logger) ) {
            return change_state_S_LS0e__S_LS1e(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_LS0e_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_LS0e(uarch,logger,position);
    }
    
    // State action S_LS1
    bool action_S_LS1(OSM_Thread &uarch,Logger *logger,unsigned position) {
      try {
        if ( uarch.Mem.can_read(uarch,*this,logger) ) {
              uarch.exec_and_read(*this,logger, uarch.Mem ) ;
              return change_state_S_LS1__S_LS1p(uarch,logger,position) ;
        }}
      catch (adliss::PMT::SMT::Thread::Exception) {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_LS1(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mLS1(*this,logger,position,true,uarch.get_pc());
        return change_state_S_LS1__S_LS1e(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_LS1_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_LS1(uarch,logger,position);
    }
    
    // State action S_LS1e
    bool action_S_LS1e(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger) ) {
            return change_state_S_LS1e__S_WBe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_LS1e_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_LS1e(uarch,logger,position);
    }
    
    // State action S_LS1p
    bool action_S_LS1p(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( exec_done (  ) && uarch.mWB.no_capacity_check(uarch,*this,logger) ) {
            return change_state_S_LS1p__S_WB(uarch,logger,position) ;
      } else if ( uarch.Mem.can_request_read(uarch,*this,logger) ) {
            uarch.Mem.send_read_request(uarch,*this,logger) ;
            return change_state_S_LS1p__S_LS1(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_LS1p_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_LS1p(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      try {
        {
              uarch.FwdCB.write(uarch,*this,logger, Trg ) ;
              write_ops(uarch,*this,logger) ;
              return change_state_S_WB__S_INIT(uarch,logger,position) ;
        }}
      catch (adliss::PMT::SMT::Thread::Exception) {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_WB(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mWB(*this,logger,position,true,uarch.get_pc());
        return change_state_S_WB__S_WBe(uarch,logger,position);
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
    
    // Flush action S_IF
    void flush_S_IF(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_IF_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IF(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_LS0
    void flush_S_LS0(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_LS0_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_LS0(uarch,logger);
    }
    
    // Flush action S_LS0e
    void flush_S_LS0e(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_LS0e_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_LS0e(uarch,logger);
    }
    
    // Flush action S_LS1
    void flush_S_LS1(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_LS1_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_LS1(uarch,logger);
    }
    
    // Flush action S_LS1e
    void flush_S_LS1e(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_LS1e_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_LS1e(uarch,logger);
    }
    
    // Flush action S_LS1p
    void flush_S_LS1p(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_LS1p_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_LS1p(uarch,logger);
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
  // Operands for instruction class serializing
  //
  
  // Operands:
  //   (src: CIA)
  //   (src: CR/p)
  //   (src: CTR?)
  struct bc_serializing_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.CIA.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).CIA.name(),-1,ic._instr->slot(0).uint64());
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).CR.name(),-1,ic._instr->slot(1).uint64());
      ic._instr->read_operand(uarch.get_iss(),2);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).CTR.name(),-1,ic._instr->slot(2).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).CIA.name(),-1,ic._instr->slot(0).uint64());
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).CR.name(),-1,ic._instr->slot(1).uint64());
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).CTR.name(),-1,ic._instr->slot(2).uint64());
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
        case 0: ic._instr->read_operand(uarch.get_iss(),0);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).CIA.name(),-1,ic._instr->slot(0).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).CR.name(),-1,ic._instr->slot(1).uint64());
        break;
        case 2: ic._instr->read_operand(uarch.get_iss(),2);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).CTR.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).CIA.name(),-1,ic._instr->slot(0).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).CR.name(),-1,ic._instr->slot(1).uint64());
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).CTR.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_Thread&>(uarch).CIA.id(static_cast<OSM_Thread&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).CIA.name(),-1,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_Thread&>(uarch).CR.id(static_cast<OSM_Thread&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).CR.name(),-1,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,2)) {
        tag_t tag = get_wm_tag(ic,2);
        RegId id = static_cast<OSM_Thread&>(uarch).CTR.id(static_cast<OSM_Thread&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,2);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),2);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).CTR.name(),-1,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).CIA.last_allocator( static_cast<OSM_Thread&>(uarch),0)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).CR.last_allocator( static_cast<OSM_Thread&>(uarch),0)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
      if (!is_op_read(ic,2)) {
        if (!can_read(uarch,ic,0,2)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).CTR.last_allocator( static_cast<OSM_Thread&>(uarch),0)),ic,2);
        } else {
          set_wm_tag(0,ic,2);
        }
      }
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
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(0);
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
  struct bc_serializing_Trg {
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
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
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
      ic._instr->commit(uarch.get_iss(),2);
      if (logger && ic._instr->is_dirty(2))logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).CTR.name(),-1,ic._instr->slot(2).uint64());
      ic._instr->commit(uarch.get_iss(),3);
      if (logger && ic._instr->is_dirty(3))logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).NIA.name(),-1,ic._instr->slot(3).uint64());
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(2)._index;
    }
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(2);
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
  struct b_serializing_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.CIA.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).CIA.name(),-1,ic._instr->slot(0).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).CIA.name(),-1,ic._instr->slot(0).uint64());
    }
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic);
        RegId id = static_cast<OSM_Thread&>(uarch).CIA.id(static_cast<OSM_Thread&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name());
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).CIA.name(),-1,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).CIA.last_allocator( static_cast<OSM_Thread&>(uarch),0)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
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
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(0);
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
  struct b_serializing_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
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
      ic._instr->commit(uarch.get_iss(),1);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).NIA.name(),-1,ic._instr->slot(1).uint64());
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(1);
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
  
  // Operand Alias:  b_serializing_Src -> bl_serializing_Src
  //   (src: CIA)
  typedef b_serializing_Src bl_serializing_Src;
  
  // Operands:
  //   (trg: LR)
  //   (trg: NIA)
  struct bl_serializing_Trg {
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
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
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
      ic._instr->commit(uarch.get_iss(),1);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).LR.name(),-1,ic._instr->slot(1).uint64());
      ic._instr->commit(uarch.get_iss(),2);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).NIA.name(),-1,ic._instr->slot(2).uint64());
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(1);
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
  struct bclr_serializing_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.LR.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).CR.name(),-1,ic._instr->slot(0).uint64());
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).CTR.name(),-1,ic._instr->slot(1).uint64());
      ic._instr->read_operand(uarch.get_iss(),2);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).LR.name(),-1,ic._instr->slot(2).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).CR.name(),-1,ic._instr->slot(0).uint64());
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).CTR.name(),-1,ic._instr->slot(1).uint64());
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).LR.name(),-1,ic._instr->slot(2).uint64());
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
        case 0: ic._instr->read_operand(uarch.get_iss(),0);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).CR.name(),-1,ic._instr->slot(0).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).CTR.name(),-1,ic._instr->slot(1).uint64());
        break;
        case 2: ic._instr->read_operand(uarch.get_iss(),2);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).LR.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).CR.name(),-1,ic._instr->slot(0).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).CTR.name(),-1,ic._instr->slot(1).uint64());
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).LR.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_Thread&>(uarch).CR.id(static_cast<OSM_Thread&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).CR.name(),-1,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_Thread&>(uarch).CTR.id(static_cast<OSM_Thread&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).CTR.name(),-1,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,2)) {
        tag_t tag = get_wm_tag(ic,2);
        RegId id = static_cast<OSM_Thread&>(uarch).LR.id(static_cast<OSM_Thread&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,2);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),2);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).LR.name(),-1,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).CR.last_allocator( static_cast<OSM_Thread&>(uarch),0)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).CTR.last_allocator( static_cast<OSM_Thread&>(uarch),0)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
      if (!is_op_read(ic,2)) {
        if (!can_read(uarch,ic,0,2)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).LR.last_allocator( static_cast<OSM_Thread&>(uarch),0)),ic,2);
        } else {
          set_wm_tag(0,ic,2);
        }
      }
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
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(0);
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
  struct bclr_serializing_Trg {
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
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
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
      ic._instr->commit(uarch.get_iss(),1);
      if (logger && ic._instr->is_dirty(1))logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).CTR.name(),-1,ic._instr->slot(1).uint64());
      ic._instr->commit(uarch.get_iss(),3);
      if (logger && ic._instr->is_dirty(3))logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).NIA.name(),-1,ic._instr->slot(3).uint64());
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(1);
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
  // Instruction class serializing
  //
  template <class Src_t, class Trg_t>
  struct Inst_serializing : public Inst_PMT {
    
    typedef struct Inst_serializing<Src_t, Trg_t> ThisClass;
    
    Inst_serializing &thisInstr() { return *this; };
    
    Src_t Src;
    Trg_t Trg;
    void reset(OSM_Thread *uarch,unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_PMT::reset(iid,cid,ic);
      _uarch = uarch;
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      _stage = (PipelineStageBase*)1;
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
    
    bool change_state_S_BR__S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_BR -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mBR.remove_instr();
      _stage = &uarch.mWB;
      return true;
    }
    
    bool change_state_S_BR__S_BRe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_BR -> S_BRe"); }
      _action = (StateFunc)action_S_BRe_p;
      _flush = (FlushFunc)flush_S_BRe_p;
      return action_S_BRe(uarch,logger,position);
    }
    
    bool change_state_S_BRe__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_BRe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mBR.remove_instr();
      _stage = &uarch.mWB;
      return true;
    }
    
    bool change_state_S_ID__S_BR(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_BR"); }
      _action = (StateFunc)action_S_BR_p;
      _flush = (FlushFunc)flush_S_BR_p;
      uarch.mID.remove_instr();
      _stage = &uarch.mBR;
      uarch.mBR.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_IDe"); }
      _action = (StateFunc)action_S_IDe_p;
      _flush = (FlushFunc)flush_S_IDe_p;
      return action_S_IDe(uarch,logger,position);
    }
    
    bool change_state_S_IDe__S_BRe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDe -> S_BRe"); }
      _action = (StateFunc)action_S_BRe_p;
      _flush = (FlushFunc)flush_S_BRe_p;
      uarch.mID.remove_instr();
      _stage = &uarch.mBR;
      uarch.mBR.add_instr(this);
      return true;
    }
    
    bool change_state_S_IF__S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IF -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mIF.remove_instr();
      _stage = &uarch.mID;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_IF(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_IF"); }
      _action = (StateFunc)action_S_IF_p;
      _flush = (FlushFunc)flush_S_IF_p;
      uarch.issue(_instr);
      _stage = &uarch.mIF;
      uarch.mIF.add_instr(this);
      return true;
    }
    
    bool change_state_S_WB__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    bool change_state_S_WB__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    // State action S_BR
    bool action_S_BR(OSM_Thread &uarch,Logger *logger,unsigned position) {
      try {
        if ( Src.read_avail(uarch,*this,logger, uarch.FwdCB ) && uarch.mWB.no_capacity_check(uarch,*this,logger) ) {
              return change_state_S_BR__S_WB(uarch,logger,position) ;
        }}
      catch (adliss::PMT::SMT::Thread::Exception) {
        if (logger) logger->logMessage("exception occurred");
        flush_S_BR(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mBR(*this,logger,position,true,uarch.get_pc());
        return change_state_S_BR__S_BRe(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_BR_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_BR(uarch,logger,position);
    }
    
    // State action S_BRe
    bool action_S_BRe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger) ) {
            return change_state_S_BRe__S_WBe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_BRe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_BRe(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      try {
        if ( Trg.can_write(uarch,*this,logger) && uarch.mWB.has_space(uarch,*this,logger) && uarch.mBR.has_space(uarch,*this,logger) ) {
              Src.record_sources(uarch,*this,logger) ;
              Trg.allocate(uarch,*this,logger) ;
              uarch.mWB.add_instr(uarch,*this,logger) ;
              return change_state_S_ID__S_BR(uarch,logger,position) ;
        }}
      catch (adliss::PMT::SMT::Thread::Exception) {
        if (logger) logger->logMessage("exception occurred");
        flush_S_ID(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mID(*this,logger,position,true,uarch.get_pc());
        return change_state_S_ID__S_IDe(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_ID_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_IDe
    bool action_S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mBR.has_space(uarch,*this,logger) ) {
            return change_state_S_IDe__S_BRe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_IDe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IDe(uarch,logger,position);
    }
    
    // State action S_IF
    bool action_S_IF(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.has_space(uarch,*this,logger) && uarch. OSM_Thread::smtIssueCheck ( thisInstr (  ) ) ) {
            return change_state_S_IF__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_IF_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IF(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mIF.has_space(uarch,*this,logger) ) {
            return change_state_S_INIT__S_IF(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      try {
        {
              uarch.exec_mWB(*this,logger,position) ;
              uarch.FwdCB.write(uarch,*this,logger, Trg ) ;
              write_ops(uarch,*this,logger) ;
              if ( branch_taken(uarch) ) {
                    uarch.taken_flush_mWB(*this,logger,position  ) ;
                }
              return change_state_S_WB__S_INIT(uarch,logger,position) ;
        }}
      catch (adliss::PMT::SMT::Thread::Exception) {
        if (logger) logger->logMessage("exception occurred");
        flush_S_WB(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mWB(*this,logger,position,true,uarch.get_pc());
        return change_state_S_WB__S_WBe(uarch,logger,position);
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
    
    // Flush action S_BR
    void flush_S_BR(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_BR_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_BR(uarch,logger);
    }
    
    // Flush action S_BRe
    void flush_S_BRe(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_BRe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_BRe(uarch,logger);
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
    
    // Flush action S_IF
    void flush_S_IF(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_IF_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IF(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
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
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      ic._instr->read_operand(uarch.get_iss(),3);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
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
        case 0: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),3);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if ((t0.can_read_tag_partial(uarch,ic,0,id,tag) & mask(ic,1)) == mask(ic,1)) {
          t0.read_partial(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).XER.name(),-1,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).XER.last_allocator( static_cast<OSM_Thread&>(uarch),0)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
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
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(1);
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
  //   (trg: CR[mask 0xf0000000]/p, mask: 0xffffffff)
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0x20000000]/p)
  //   (src: XER[mask 0x80000000]/p, trg: XER[mask 0x20000000]/p, mask: 0xffffffff)
  struct addic__sfx_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[2])) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[3])) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),3);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
    }
    
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.CR.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint32())) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
          return true;
        case 2: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[2] & ~mask).uint32())) return false;
          return true;
        case 3: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[3] & ~mask).uint32())) return false;
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
        case 3: ic._instr->read_operand(uarch.get_iss(),3);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
        case 2: break;
        case 3: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_Thread&>(uarch).CR.id(static_cast<OSM_Thread&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if ((t0.can_read_tag_partial(uarch,ic,0,id,tag) & mask(ic,0)) == mask(ic,0)) {
          t0.read_partial(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).CR.name(),-1,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,2)) {
        tag_t tag = get_wm_tag(ic,2);
        RegId id = static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,2);
        }
        else if ((t0.can_read_tag_partial(uarch,ic,0,id,tag) & mask(ic,2)) == mask(ic,2)) {
          t0.read_partial(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),2);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).XER.name(),-1,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,3)) {
        tag_t tag = get_wm_tag(ic,3);
        RegId id = static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,3);
        }
        else if ((t0.can_read_tag_partial(uarch,ic,0,id,tag) & mask(ic,3)) == mask(ic,3)) {
          t0.read_partial(uarch,ic,logger,*this,3);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),3);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).XER.name(),-1,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).CR.last_allocator( static_cast<OSM_Thread&>(uarch),0)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(2)._index)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
      if (!is_op_read(ic,2)) {
        if (!can_read(uarch,ic,0,2)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).XER.last_allocator( static_cast<OSM_Thread&>(uarch),0)),ic,2);
        } else {
          set_wm_tag(0,ic,2);
        }
      }
      if (!is_op_read(ic,3)) {
        if (!can_read(uarch,ic,0,3)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).XER.last_allocator( static_cast<OSM_Thread&>(uarch),0)),ic,3);
        } else {
          set_wm_tag(0,ic,3);
        }
      }
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.CR.allocate(uarch,ic,logger, ic._masks[1]);
      uarch.GPR.allocate(uarch,ic,logger,ic.op(2)._index);
      uarch.XER.allocate(uarch,ic,logger, ic._masks[2]);
      uarch.XER.allocate(uarch,ic,logger, ic._masks[3]);
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.CR.deallocate(uarch,ic,logger, ic._masks[1]);
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(2)._index);
      uarch.XER.deallocate(uarch,ic,logger, ic._masks[2]);
      uarch.XER.deallocate(uarch,ic,logger, ic._masks[3]);
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),0);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).CR.name(),-1,ic._instr->slot(0).uint64());
      ic._instr->commit(uarch.get_iss(),2);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
      ic._instr->commit(uarch.get_iss(),3);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
      ic._instr->commit(uarch.get_iss(),3);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(0);
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).CR.name(static_cast<OSM_Thread&>(uarch),0);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
        case 2: return static_cast<OSM_Thread&>(uarch).XER.name(static_cast<OSM_Thread&>(uarch),0);
        case 3: return static_cast<OSM_Thread&>(uarch).XER.name(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_PMT&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).CR.id(static_cast<OSM_Thread&>(uarch),0);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
        case 2: return static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0);
        case 3: return static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._masks[1];
        case 1: return 0x0;
        case 2: return ic._masks[2];
        case 3: return ic._masks[3];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(2).uint32();
        case 2: return ic._instr->write_mask(3).uint32();
        case 3: return ic._instr->write_mask(3).uint32();
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
        case 3: ic._instr->write_mask(3) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 2;
        case 2: return 3;
        case 3: return 3;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_PMT&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 4; };
    
  };
  
  // Operands:
  //   (src: GPR(RA)?)
  struct addi_sfx_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
    }
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic);
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        if (tag == 0) {
          read(uarch,ic,logger);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name());
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(0)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
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
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(0);
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
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
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
      ic._instr->commit(uarch.get_iss(),1);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(1);
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
  
  // Operand Alias:  addi_sfx_Src -> addis_sfx_Src
  //   (src: GPR(RA)?)
  typedef addi_sfx_Src addis_sfx_Src;
  
  // Operand Alias:  addi_sfx_Trg -> addis_sfx_Trg
  //   (trg: GPR(RT))
  typedef addi_sfx_Trg addis_sfx_Trg;
  
  // Operands:
  //   (src: GPR(RS))
  struct ori_sfx_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
    }
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic);
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        if (tag == 0) {
          read(uarch,ic,logger);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name());
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
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
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(1);
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
  //   (trg: GPR(RA))
  struct ori_sfx_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
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
      ic._instr->commit(uarch.get_iss(),0);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(0);
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
  //   (src: XER[mask 0x80000000]/p)
  struct andi__sfx_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),2);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
      ic._instr->read_operand(uarch.get_iss(),3);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
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
        case 0: ic._instr->read_operand(uarch.get_iss(),2);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),3);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if ((t0.can_read_tag_partial(uarch,ic,0,id,tag) & mask(ic,1)) == mask(ic,1)) {
          t0.read_partial(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).XER.name(),-1,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(2)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).XER.last_allocator( static_cast<OSM_Thread&>(uarch),0)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
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
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(2);
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
  //   (trg: CR[mask 0xf0000000]/p, mask: 0xffffffff)
  //   (trg: GPR(RA))
  //   (src: XER[mask 0x80000000]/p, mask: 0xffffffff)
  struct andi__sfx_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[2])) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),3);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
    }
    
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.CR.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint32())) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
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
        case 2: ic._instr->read_operand(uarch.get_iss(),3);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
        case 2: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_Thread&>(uarch).CR.id(static_cast<OSM_Thread&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if ((t0.can_read_tag_partial(uarch,ic,0,id,tag) & mask(ic,0)) == mask(ic,0)) {
          t0.read_partial(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).CR.name(),-1,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,2)) {
        tag_t tag = get_wm_tag(ic,2);
        RegId id = static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,2);
        }
        else if ((t0.can_read_tag_partial(uarch,ic,0,id,tag) & mask(ic,2)) == mask(ic,2)) {
          t0.read_partial(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),2);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).XER.name(),-1,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).CR.last_allocator( static_cast<OSM_Thread&>(uarch),0)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
      if (!is_op_read(ic,2)) {
        if (!can_read(uarch,ic,0,2)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).XER.last_allocator( static_cast<OSM_Thread&>(uarch),0)),ic,2);
        } else {
          set_wm_tag(0,ic,2);
        }
      }
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
      ic._instr->commit(uarch.get_iss(),0);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).CR.name(),-1,ic._instr->slot(0).uint64());
      ic._instr->commit(uarch.get_iss(),1);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(0);
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).CR.name(static_cast<OSM_Thread&>(uarch),0);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.name(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        case 2: return static_cast<OSM_Thread&>(uarch).XER.name(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_PMT&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).CR.id(static_cast<OSM_Thread&>(uarch),0);
        case 1: return static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
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
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  struct add_sfx_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
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
        case 0: ic._instr->read_operand(uarch.get_iss(),0);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(0)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
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
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(0);
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
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
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
      ic._instr->commit(uarch.get_iss(),2);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(2)._index;
    }
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(2);
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
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  struct or_sfx_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      ic._instr->read_operand(uarch.get_iss(),2);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
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
        case 0: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),2);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(2)._index)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
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
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(1);
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
  
  // Operand Alias:  ori_sfx_Trg -> or_sfx_Trg
  //   (trg: GPR(RA))
  typedef ori_sfx_Trg or_sfx_Trg;
  
  // Operand Alias:  ori_sfx_Src -> rlwinm_sfx_Src
  //   (src: GPR(RS))
  typedef ori_sfx_Src rlwinm_sfx_Src;
  
  // Operand Alias:  ori_sfx_Trg -> rlwinm_sfx_Trg
  //   (trg: GPR(RA))
  typedef ori_sfx_Trg rlwinm_sfx_Trg;
  
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
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      ic._instr->read_operand(uarch.get_iss(),2);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(2).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(2).uint64());
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
        case 0: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),2);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if ((t0.can_read_tag_partial(uarch,ic,0,id,tag) & mask(ic,1)) == mask(ic,1)) {
          t0.read_partial(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).XER.name(),-1,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).XER.last_allocator( static_cast<OSM_Thread&>(uarch),0)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
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
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(1);
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
  //   (trg: CR/p, mask: 0xffffffff)
  //   (src: XER[mask 0x80000000]/p, mask: 0xffffffff)
  struct cmpi_sfx_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[2])) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),2);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(2).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(2).uint64());
    }
    
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.CR.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint32())) return false;
          return true;
        case 1: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[2] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: ic._instr->read_operand(uarch.get_iss(),2);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_Thread&>(uarch).CR.id(static_cast<OSM_Thread&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if ((t0.can_read_tag_partial(uarch,ic,0,id,tag) & mask(ic,0)) == mask(ic,0)) {
          t0.read_partial(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).CR.name(),-1,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if ((t0.can_read_tag_partial(uarch,ic,0,id,tag) & mask(ic,1)) == mask(ic,1)) {
          t0.read_partial(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).XER.name(),-1,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).CR.last_allocator( static_cast<OSM_Thread&>(uarch),0)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).XER.last_allocator( static_cast<OSM_Thread&>(uarch),0)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.CR.allocate(uarch,ic,logger, ic._masks[1]);
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.CR.deallocate(uarch,ic,logger, ic._masks[1]);
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),0);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).CR.name(),-1,ic._instr->slot(0).uint64());
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(0);
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).CR.name(static_cast<OSM_Thread&>(uarch),0);
        case 1: return static_cast<OSM_Thread&>(uarch).XER.name(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_PMT&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).CR.id(static_cast<OSM_Thread&>(uarch),0);
        case 1: return static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._masks[1];
        case 1: return ic._masks[2];
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
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: XER[mask 0x80000000]/p)
  struct cmp_sfx_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      ic._instr->read_operand(uarch.get_iss(),2);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
      ic._instr->read_operand(uarch.get_iss(),3);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
    }
    
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
          return true;
        case 2: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),2);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
        break;
        case 2: ic._instr->read_operand(uarch.get_iss(),3);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,2)) {
        tag_t tag = get_wm_tag(ic,2);
        RegId id = static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,2);
        }
        else if ((t0.can_read_tag_partial(uarch,ic,0,id,tag) & mask(ic,2)) == mask(ic,2)) {
          t0.read_partial(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),2);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).XER.name(),-1,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(2)._index)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
      if (!is_op_read(ic,2)) {
        if (!can_read(uarch,ic,0,2)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).XER.last_allocator( static_cast<OSM_Thread&>(uarch),0)),ic,2);
        } else {
          set_wm_tag(0,ic,2);
        }
      }
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
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(1);
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
        case 2: return ic._masks[0];
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
  
  // Operands:
  //   (trg: CR/p, mask: 0xffffffff)
  //   (src: XER[mask 0x80000000]/p, mask: 0xffffffff)
  struct cmp_sfx_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[2])) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),3);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
    }
    
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.CR.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint32())) return false;
          return true;
        case 1: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[2] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: ic._instr->read_operand(uarch.get_iss(),3);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_Thread&>(uarch).CR.id(static_cast<OSM_Thread&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if ((t0.can_read_tag_partial(uarch,ic,0,id,tag) & mask(ic,0)) == mask(ic,0)) {
          t0.read_partial(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).CR.name(),-1,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if ((t0.can_read_tag_partial(uarch,ic,0,id,tag) & mask(ic,1)) == mask(ic,1)) {
          t0.read_partial(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).XER.name(),-1,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).CR.last_allocator( static_cast<OSM_Thread&>(uarch),0)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).XER.last_allocator( static_cast<OSM_Thread&>(uarch),0)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
    }
    
    bool can_write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.CR.allocate(uarch,ic,logger, ic._masks[1]);
    }
    
    void deallocate(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      uarch.CR.deallocate(uarch,ic,logger, ic._masks[1]);
    }
    
    void write(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),0);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).CR.name(),-1,ic._instr->slot(0).uint64());
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(0);
    }
    
    bool is_real(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_PMT &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).CR.name(static_cast<OSM_Thread&>(uarch),0);
        case 1: return static_cast<OSM_Thread&>(uarch).XER.name(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_PMT&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_Thread&>(uarch).CR.id(static_cast<OSM_Thread&>(uarch),0);
        case 1: return static_cast<OSM_Thread&>(uarch).XER.id(static_cast<OSM_Thread&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._masks[1];
        case 1: return ic._masks[2];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(3).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_PMT &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: ic._instr->write_mask(0) = tag;
        break;
        case 1: ic._instr->write_mask(3) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
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
  //   (src: GPR(RS))
  struct mtspr_sfx_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
    }
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic);
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        if (tag == 0) {
          read(uarch,ic,logger);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name());
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(0)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
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
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(0);
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
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
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
      ic._instr->commit(uarch.get_iss(),1);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).SPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(1);
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
  //   (src: SPR(SPRN))
  struct mfspr_sfx_Src {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.SPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).SPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).SPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
    }
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic);
        RegId id = static_cast<OSM_Thread&>(uarch).SPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        if (tag == 0) {
          read(uarch,ic,logger);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name());
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).SPR.name(),ic.op(1)._index,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).SPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
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
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(1);
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
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
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
      ic._instr->commit(uarch.get_iss(),0);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(0);
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
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      ic._instr->read_operand(uarch.get_iss(),2);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
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
        case 0: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),2);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if ((t0.can_read_tag_partial(uarch,ic,0,id,tag) & mask(ic,0)) == mask(ic,0)) {
          t0.read_partial(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(2)._index)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
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
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(1);
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
  
  // Operand Alias:  ori_sfx_Trg -> srw_sfx_Trg
  //   (trg: GPR(RA))
  typedef ori_sfx_Trg srw_sfx_Trg;
  
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
      _stage = (PipelineStageBase*)1;
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
    
    bool change_state_S_EX0__S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX0 -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mEX0.remove_instr_indep(position);
      _stage = &uarch.mWB;
      return true;
    }
    
    bool change_state_S_EX0__S_EX0e(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX0 -> S_EX0e"); }
      _action = (StateFunc)action_S_EX0e_p;
      _flush = (FlushFunc)flush_S_EX0e_p;
      return action_S_EX0e(uarch,logger,position);
    }
    
    bool change_state_S_EX0e__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX0e -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mEX0.remove_instr_indep(position);
      _stage = &uarch.mWB;
      return true;
    }
    
    bool change_state_S_ID__S_EX0(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX0"); }
      _action = (StateFunc)action_S_EX0_p;
      _flush = (FlushFunc)flush_S_EX0_p;
      uarch.mID.remove_instr();
      _stage = &uarch.mEX0;
      uarch.mEX0.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_IDe"); }
      _action = (StateFunc)action_S_IDe_p;
      _flush = (FlushFunc)flush_S_IDe_p;
      return action_S_IDe(uarch,logger,position);
    }
    
    bool change_state_S_IDe__S_EX0e(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDe -> S_EX0e"); }
      _action = (StateFunc)action_S_EX0e_p;
      _flush = (FlushFunc)flush_S_EX0e_p;
      uarch.mID.remove_instr();
      _stage = &uarch.mEX0;
      uarch.mEX0.add_instr(this);
      return true;
    }
    
    bool change_state_S_IF__S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IF -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mIF.remove_instr();
      _stage = &uarch.mID;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_IF(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_IF"); }
      _action = (StateFunc)action_S_IF_p;
      _flush = (FlushFunc)flush_S_IF_p;
      uarch.issue(_instr);
      _stage = &uarch.mIF;
      uarch.mIF.add_instr(this);
      return true;
    }
    
    bool change_state_S_WB__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    bool change_state_S_WB__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    // State action S_EX0
    bool action_S_EX0(OSM_Thread &uarch,Logger *logger,unsigned position) {
      try {
        if ( Src.read_avail(uarch,*this,logger, uarch.FwdCB ) && uarch.mWB.no_capacity_check(uarch,*this,logger) ) {
              uarch.exec_mEX0(*this,logger,position) ;
              return change_state_S_EX0__S_WB(uarch,logger,position) ;
        }}
      catch (adliss::PMT::SMT::Thread::Exception) {
        if (logger) logger->logMessage("exception occurred");
        flush_S_EX0(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mEX0(*this,logger,position,true,uarch.get_pc());
        return change_state_S_EX0__S_EX0e(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_EX0_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX0(uarch,logger,position);
    }
    
    // State action S_EX0e
    bool action_S_EX0e(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger) ) {
            return change_state_S_EX0e__S_WBe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EX0e_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX0e(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      try {
        if ( Trg.can_write(uarch,*this,logger) && uarch.mWB.has_space(uarch,*this,logger) && uarch.mEX0.has_space(uarch,*this,logger) ) {
              Src.record_sources(uarch,*this,logger) ;
              Trg.allocate(uarch,*this,logger) ;
              uarch.mWB.add_instr(uarch,*this,logger) ;
              return change_state_S_ID__S_EX0(uarch,logger,position) ;
        }}
      catch (adliss::PMT::SMT::Thread::Exception) {
        if (logger) logger->logMessage("exception occurred");
        flush_S_ID(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mID(*this,logger,position,true,uarch.get_pc());
        return change_state_S_ID__S_IDe(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_ID_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_IDe
    bool action_S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mEX0.has_space(uarch,*this,logger) ) {
            return change_state_S_IDe__S_EX0e(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_IDe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IDe(uarch,logger,position);
    }
    
    // State action S_IF
    bool action_S_IF(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.has_space(uarch,*this,logger) && uarch. OSM_Thread::smtIssueCheck ( thisInstr (  ) ) ) {
            return change_state_S_IF__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_IF_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IF(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mIF.has_space(uarch,*this,logger) ) {
            return change_state_S_INIT__S_IF(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      try {
        {
              uarch.FwdCB.write(uarch,*this,logger, Trg ) ;
              write_ops(uarch,*this,logger) ;
              return change_state_S_WB__S_INIT(uarch,logger,position) ;
        }}
      catch (adliss::PMT::SMT::Thread::Exception) {
        if (logger) logger->logMessage("exception occurred");
        flush_S_WB(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mWB(*this,logger,position,true,uarch.get_pc());
        return change_state_S_WB__S_WBe(uarch,logger,position);
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
    
    // Flush action S_EX0
    void flush_S_EX0(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_EX0_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX0(uarch,logger);
    }
    
    // Flush action S_EX0e
    void flush_S_EX0e(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_EX0e_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX0e(uarch,logger);
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
    
    // Flush action S_IF
    void flush_S_IF(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_IF_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IF(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
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
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
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
        case 0: ic._instr->read_operand(uarch.get_iss(),0);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(0)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
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
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(0);
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
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      ic._instr->read_operand(uarch.get_iss(),2);
      if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
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
        case 0: ic._instr->read_operand(uarch.get_iss(),0);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
        case 2: ic._instr->read_operand(uarch.get_iss(),2);
        if (logger)logger->logOpAction("read",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(0)._index);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(1)._index);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(1)._index,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,2)) {
        tag_t tag = get_wm_tag(ic,2);
        RegId id = static_cast<OSM_Thread&>(uarch).GPR.id(static_cast<OSM_Thread&>(uarch),ic.op(2)._index);
        if (tag == 0) {
          read(uarch,ic,logger,2);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),2);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(2)._index,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(0)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(1)._index)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
      if (!is_op_read(ic,2)) {
        if (!can_read(uarch,ic,0,2)) {
          set_wm_tag((static_cast<OSM_Thread&>(uarch).GPR.last_allocator( static_cast<OSM_Thread&>(uarch),ic.op(2)._index)),ic,2);
        } else {
          set_wm_tag(0,ic,2);
        }
      }
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
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(0);
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
  struct stwu_store_Trg {
    bool can_read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_PMT &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0>
    bool read_avail(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger,T0 &t0) {
      bool done = true;
      return done;
    }
    
    void record_sources(OSM_Thread &uarch,Inst_PMT &ic,Logger *)
    {
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
      ic._instr->commit(uarch.get_iss(),0);
      if (logger)logger->logOpAction("write",static_cast<OSM_Thread&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
    }
    
    unsigned field_value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bits<32> value(OSM_Thread &uarch,Inst_PMT &ic,Logger *logger) const {
      return ic._instr->slot(0);
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
  template <class Src_t, class Trg_t>
  struct Inst_store : public Inst_PMT {
    
    typedef struct Inst_store<Src_t, Trg_t> ThisClass;
    
    Inst_store &thisInstr() { return *this; };
    
    Src_t Src;
    Trg_t Trg;
    void reset(OSM_Thread *uarch,unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_PMT::reset(iid,cid,ic);
      _uarch = uarch;
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      _stage = (PipelineStageBase*)1;
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
    
    bool exec_done() const { return _status == adl::MemAccess::opNone; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool change_state_S_ID__S_LS0(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_LS0"); }
      _action = (StateFunc)action_S_LS0_p;
      _flush = (FlushFunc)flush_S_LS0_p;
      uarch.mID.remove_instr();
      _stage = &uarch.mLS0;
      uarch.mLS0.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_IDe"); }
      _action = (StateFunc)action_S_IDe_p;
      _flush = (FlushFunc)flush_S_IDe_p;
      return action_S_IDe(uarch,logger,position);
    }
    
    bool change_state_S_IDe__S_LS0e(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDe -> S_LS0e"); }
      _action = (StateFunc)action_S_LS0e_p;
      _flush = (FlushFunc)flush_S_LS0e_p;
      uarch.mID.remove_instr();
      _stage = &uarch.mLS0;
      uarch.mLS0.add_instr(this);
      return true;
    }
    
    bool change_state_S_IF__S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IF -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mIF.remove_instr();
      _stage = &uarch.mID;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_IF(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_IF"); }
      _action = (StateFunc)action_S_IF_p;
      _flush = (FlushFunc)flush_S_IF_p;
      uarch.issue(_instr);
      _stage = &uarch.mIF;
      uarch.mIF.add_instr(this);
      return true;
    }
    
    bool change_state_S_LS0__S_LS1(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_LS0 -> S_LS1"); }
      _action = (StateFunc)action_S_LS1_p;
      _flush = (FlushFunc)flush_S_LS1_p;
      uarch.mLS0.remove_instr();
      _stage = &uarch.mLS1;
      uarch.mLS1.add_instr(this);
      return true;
    }
    
    bool change_state_S_LS0__S_LS0e(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_LS0 -> S_LS0e"); }
      _action = (StateFunc)action_S_LS0e_p;
      _flush = (FlushFunc)flush_S_LS0e_p;
      return action_S_LS0e(uarch,logger,position);
    }
    
    bool change_state_S_LS0e__S_LS1e(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_LS0e -> S_LS1e"); }
      _action = (StateFunc)action_S_LS1e_p;
      _flush = (FlushFunc)flush_S_LS1e_p;
      uarch.mLS0.remove_instr();
      _stage = &uarch.mLS1;
      uarch.mLS1.add_instr(this);
      return true;
    }
    
    bool change_state_S_LS1__S_LS1p(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_LS1 -> S_LS1p"); }
      _action = (StateFunc)action_S_LS1p_p;
      _flush = (FlushFunc)flush_S_LS1p_p;
      return action_S_LS1p(uarch,logger,position);
    }
    
    bool change_state_S_LS1__S_LS1e(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_LS1 -> S_LS1e"); }
      _action = (StateFunc)action_S_LS1e_p;
      _flush = (FlushFunc)flush_S_LS1e_p;
      return action_S_LS1e(uarch,logger,position);
    }
    
    bool change_state_S_LS1e__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_LS1e -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mLS1.remove_instr();
      _stage = &uarch.mWB;
      return true;
    }
    
    bool change_state_S_LS1p__S_LS1(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_LS1p -> S_LS1"); }
      _action = (StateFunc)action_S_LS1_p;
      _flush = (FlushFunc)flush_S_LS1_p;
      return action_S_LS1(uarch,logger,position);
    }
    
    bool change_state_S_LS1p__S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_LS1p -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mLS1.remove_instr();
      _stage = &uarch.mWB;
      return true;
    }
    
    bool change_state_S_WB__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      uarch.commit(this);
      return true;
    }
    
    bool change_state_S_WB__S_WBe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      uarch.commit(this);
      return true;
    }
    
    // State action S_ID
    bool action_S_ID(OSM_Thread &uarch,Logger *logger,unsigned position) {
      try {
        if ( Trg.can_write(uarch,*this,logger) && uarch.mWB.has_space(uarch,*this,logger) && uarch.mLS0.has_space(uarch,*this,logger) ) {
              Src.record_sources(uarch,*this,logger) ;
              Trg.allocate(uarch,*this,logger) ;
              uarch.mWB.add_instr(uarch,*this,logger) ;
              return change_state_S_ID__S_LS0(uarch,logger,position) ;
        }}
      catch (adliss::PMT::SMT::Thread::Exception) {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_ID(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mID(*this,logger,position,true,uarch.get_pc());
        return change_state_S_ID__S_IDe(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_ID_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_IDe
    bool action_S_IDe(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mLS0.has_space(uarch,*this,logger) ) {
            return change_state_S_IDe__S_LS0e(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_IDe_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IDe(uarch,logger,position);
    }
    
    // State action S_IF
    bool action_S_IF(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.has_space(uarch,*this,logger) && uarch. OSM_Thread::smtIssueCheck ( thisInstr (  ) ) ) {
            return change_state_S_IF__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_IF_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IF(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mIF.has_space(uarch,*this,logger) ) {
            return change_state_S_INIT__S_IF(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_LS0
    bool action_S_LS0(OSM_Thread &uarch,Logger *logger,unsigned position) {
      try {
        if ( Src.read_avail(uarch,*this,logger, uarch.FwdCB ) && uarch.mLS1.has_space(uarch,*this,logger) && uarch.Mem.can_request_write(uarch,*this,logger) ) {
              uarch.exec_mem(*this,logger) ;
              uarch.Mem.send_write_request(uarch,*this,logger) ;
              return change_state_S_LS0__S_LS1(uarch,logger,position) ;
        }}
      catch (adliss::PMT::SMT::Thread::Exception) {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_LS0(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mLS0(*this,logger,position,true,uarch.get_pc());
        return change_state_S_LS0__S_LS0e(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_LS0_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_LS0(uarch,logger,position);
    }
    
    // State action S_LS0e
    bool action_S_LS0e(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mLS1.has_space(uarch,*this,logger) ) {
            return change_state_S_LS0e__S_LS1e(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_LS0e_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_LS0e(uarch,logger,position);
    }
    
    // State action S_LS1
    bool action_S_LS1(OSM_Thread &uarch,Logger *logger,unsigned position) {
      try {
        if ( uarch.Mem.can_read(uarch,*this,logger) ) {
              uarch.exec_and_write(*this,logger, uarch.Mem ) ;
              return change_state_S_LS1__S_LS1p(uarch,logger,position) ;
        }}
      catch (adliss::PMT::SMT::Thread::Exception) {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_LS1(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mLS1(*this,logger,position,true,uarch.get_pc());
        return change_state_S_LS1__S_LS1e(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_LS1_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_LS1(uarch,logger,position);
    }
    
    // State action S_LS1e
    bool action_S_LS1e(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger) ) {
            return change_state_S_LS1e__S_WBe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_LS1e_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_LS1e(uarch,logger,position);
    }
    
    // State action S_LS1p
    bool action_S_LS1p(OSM_Thread &uarch,Logger *logger,unsigned position) {
      if ( exec_done (  ) && uarch.mWB.no_capacity_check(uarch,*this,logger) ) {
            return change_state_S_LS1p__S_WB(uarch,logger,position) ;
      } else if ( uarch.Mem.can_request_write(uarch,*this,logger) ) {
            uarch.Mem.send_write_request(uarch,*this,logger) ;
            return change_state_S_LS1p__S_LS1(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_LS1p_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_LS1p(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_Thread &uarch,Logger *logger,unsigned position) {
      try {
        {
              uarch.FwdCB.write(uarch,*this,logger, Trg ) ;
              write_ops(uarch,*this,logger) ;
              return change_state_S_WB__S_INIT(uarch,logger,position) ;
        }}
      catch (adliss::PMT::SMT::Thread::Exception) {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_WB(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mWB(*this,logger,position,true,uarch.get_pc());
        return change_state_S_WB__S_WBe(uarch,logger,position);
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
    
    // Flush action S_IF
    void flush_S_IF(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_IF_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IF(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_LS0
    void flush_S_LS0(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_LS0_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_LS0(uarch,logger);
    }
    
    // Flush action S_LS0e
    void flush_S_LS0e(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_LS0e_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_LS0e(uarch,logger);
    }
    
    // Flush action S_LS1
    void flush_S_LS1(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_LS1_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_LS1(uarch,logger);
    }
    
    // Flush action S_LS1e
    void flush_S_LS1e(OSM_Thread &uarch,Logger *logger) {
    }
    static void flush_S_LS1e_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_LS1e(uarch,logger);
    }
    
    // Flush action S_LS1p
    void flush_S_LS1p(OSM_Thread &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_LS1p_p(Inst_PMT *x,OSM_Thread &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_LS1p(uarch,logger);
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
  
  byte_t *Mem_mem_t::hasData(addr_t addr, unsigned &size) const {
    return GlobalMemory.getPage(addr) + GlobalMemoryType::pageOffset(addr);
  }
  
  static CacheConfigMap::value_type _sym945[] = {
    CacheConfigMap::value_type("critical_word_first",CacheConfigItem(1)),
    CacheConfigMap::value_type("invalidate_latency",CacheConfigItem(1)),
    CacheConfigMap::value_type("linefill_access_width",CacheConfigItem(64)),
    CacheConfigMap::value_type("linefill_lock_holdoff_cycle",CacheConfigItem(1)),
    CacheConfigMap::value_type("linefill_read_latency",CacheConfigItem(1)),
    CacheConfigMap::value_type("linefill_write_latency",CacheConfigItem(1)),
    CacheConfigMap::value_type("lock_latency",CacheConfigItem(1)),
    CacheConfigMap::value_type("read_hit_latency",CacheConfigItem(1)),
    CacheConfigMap::value_type("unlock_latency",CacheConfigItem(1)),
    CacheConfigMap::value_type("write_hit_latency",CacheConfigItem(1)),
    CacheConfigMap::value_type("write_mode",CacheConfigItem("WriteThrough")),
    CacheConfigMap::value_type("zero_latency",CacheConfigItem(1))
  };
  
  L1d_cache_t::L1d_cache_t(OSM_Thread &uarch, UadlMemoryIf &memory) :
  Cache(
      uarch,
      "L1d",
      memory,
      0,
      128,
      1, // address bandwidth
      1, // data bandwidth
      1, // max requests
      1, // allow bus wait
      0, // pre-emptive
      CacheConfigMap(&_sym945[0],&_sym945[12]),
      1  // with-data
    ),
    _uarch(uarch)
  {
  }
  
  void L1d_cache_t::reset_has_data()
  {
    if (_uarch.hadReset()) {
      // set up data array status if this is the first time step
      unsigned num_sets = _uarch._iss->L1d_cache_get_sets();
      unsigned num_ways = _uarch._iss->L1d_cache_get_ways();
      _hasData = HasData(num_sets,BVect(num_ways,false));
      for (unsigned set = 0; set < num_sets; set++) {
        for (unsigned way = 0; way < num_ways; way++) {
          _hasData[set][way] = _uarch._iss->L1d_cache._array[set][way].valid;
        }
      }
    }
  }
  
  bool L1d_cache_t::lookup(addr_t addr, unsigned &set, unsigned &way, unsigned &offset) const
  {
    const bool hit = _uarch._iss->L1d_cache_find(adl::CacheLogRead, (int&)set, (int&)way, addr);
    if (hit) offset = _uarch._iss->L1d_cache_get_offset(addr);
    return hit;
  }
  
  byte_t* L1d_cache_t::cachePosition(unsigned set, unsigned way, unsigned offset) const
  {
    return _uarch._iss->L1d_cache_data_array(set, way) + offset;
  }
  
  addr_t L1d_cache_t::getTagAddr(unsigned set, unsigned way)
  {
    return _uarch._iss->L1d_cache_get_tag(set, way);
  }
  
  void L1d_cache_t::resize() {
    unsigned num_sets = _uarch._iss->L1d_cache_get_sets();
    unsigned num_ways = _uarch._iss->L1d_cache_get_ways();
    _hasData = HasData(num_sets,std::vector<bool>(num_ways));
  }
  
  void L1d_cache_t::operationBegin(uadl::MemoryRequest &request)
  {
    Cache::operationBegin(request);
    
    if (_uarch._iss->L1d_cache_begin(
            request.type, request.action,
            adl::CacheStatus(),
            request.set, request.way, request.addr, request.replaceLine,
            hasCommand()
        ))
    {
      request.offset = _uarch._iss->L1d_cache_get_offset(request.addr);
    } else {
      request.action = adl::CacheNone;
    }
    request.lookupDone = true;
  }
  
  void L1d_cache_t::operationEnd(const uadl::MemoryRequest &request) const
  {
    if (request.action != adl::CacheNone) {
      _uarch._iss->L1d_cache_end(request.type,request.action, request.set, request.way,request.addr, hasCommand());
    }
  }
  
  
  void L1d_cache_t::clearValid(unsigned set, unsigned way)
  {
    _uarch._iss->L1d_cache._array[set][way].valid = false;
  }
  
  bool L1d_cache_t::is_write_through(addr_t addr)
  {
    return _uarch._iss->L1d.is_write_through(adl::CacheWrite,addr);
  }
  
  static CacheConfigMap::value_type _sym946[] = {
    CacheConfigMap::value_type("critical_word_first",CacheConfigItem(1)),
    CacheConfigMap::value_type("invalidate_latency",CacheConfigItem(1)),
    CacheConfigMap::value_type("linefill_access_width",CacheConfigItem(64)),
    CacheConfigMap::value_type("linefill_lock_holdoff_cycle",CacheConfigItem(1)),
    CacheConfigMap::value_type("linefill_read_latency",CacheConfigItem(1)),
    CacheConfigMap::value_type("linefill_write_latency",CacheConfigItem(1)),
    CacheConfigMap::value_type("lock_latency",CacheConfigItem(1)),
    CacheConfigMap::value_type("read_hit_latency",CacheConfigItem(1)),
    CacheConfigMap::value_type("unlock_latency",CacheConfigItem(1)),
    CacheConfigMap::value_type("write_hit_latency",CacheConfigItem(1)),
    CacheConfigMap::value_type("write_mode",CacheConfigItem("WriteThrough")),
    CacheConfigMap::value_type("zero_latency",CacheConfigItem(1))
  };
  
  L1i_cache_t::L1i_cache_t(OSM_Thread &uarch, UadlMemoryIf &memory) :
  Cache(
      uarch,
      "L1i",
      memory,
      0,
      128,
      1, // address bandwidth
      1, // data bandwidth
      1, // max requests
      1, // allow bus wait
      0, // pre-emptive
      CacheConfigMap(&_sym946[0],&_sym946[12]),
      1  // with-data
    ),
    _uarch(uarch)
  {
  }
  
  void L1i_cache_t::reset_has_data()
  {
    if (_uarch.hadReset()) {
      // set up data array status if this is the first time step
      unsigned num_sets = _uarch._iss->L1i_cache_get_sets();
      unsigned num_ways = _uarch._iss->L1i_cache_get_ways();
      _hasData = HasData(num_sets,BVect(num_ways,false));
      for (unsigned set = 0; set < num_sets; set++) {
        for (unsigned way = 0; way < num_ways; way++) {
          _hasData[set][way] = _uarch._iss->L1i_cache._array[set][way].valid;
        }
      }
    }
  }
  
  bool L1i_cache_t::lookup(addr_t addr, unsigned &set, unsigned &way, unsigned &offset) const
  {
    const bool hit = _uarch._iss->L1i_cache_find(adl::CacheLogRead, (int&)set, (int&)way, addr);
    if (hit) offset = _uarch._iss->L1i_cache_get_offset(addr);
    return hit;
  }
  
  byte_t* L1i_cache_t::cachePosition(unsigned set, unsigned way, unsigned offset) const
  {
    return _uarch._iss->L1i_cache_data_array(set, way) + offset;
  }
  
  addr_t L1i_cache_t::getTagAddr(unsigned set, unsigned way)
  {
    return _uarch._iss->L1i_cache_get_tag(set, way);
  }
  
  void L1i_cache_t::resize() {
    unsigned num_sets = _uarch._iss->L1i_cache_get_sets();
    unsigned num_ways = _uarch._iss->L1i_cache_get_ways();
    _hasData = HasData(num_sets,std::vector<bool>(num_ways));
  }
  
  void L1i_cache_t::operationBegin(uadl::MemoryRequest &request)
  {
    Cache::operationBegin(request);
    
    if (_uarch._iss->L1i_cache_begin(
            request.type, request.action,
            adl::CacheStatus(),
            request.set, request.way, request.addr, request.replaceLine,
            hasCommand()
        ))
    {
      request.offset = _uarch._iss->L1i_cache_get_offset(request.addr);
    } else {
      request.action = adl::CacheNone;
    }
    request.lookupDone = true;
  }
  
  void L1i_cache_t::operationEnd(const uadl::MemoryRequest &request) const
  {
    if (request.action != adl::CacheNone) {
      _uarch._iss->L1i_cache_end(request.type,request.action, request.set, request.way,request.addr, hasCommand());
    }
  }
  
  
  void L1i_cache_t::clearValid(unsigned set, unsigned way)
  {
    _uarch._iss->L1i_cache._array[set][way].valid = false;
  }
  
  bool L1i_cache_t::is_write_through(addr_t addr)
  {
    return _uarch._iss->L1i.is_write_through(adl::CacheWrite,addr);
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
  
  addr_t Inst_PMT::branch_target() const
  {
    return _instr->branch_target();
  }
  
  inline const adl::OpInfo &Inst_PMT::op(unsigned index) const { return _instr->op(index); };
  
  inline const uint32_t *Inst_PMT::opc() const { return _instr->opc(); };
  
  unsigned OSM_Thread::getInstrClassAttrByName(const std::string &attr)
  {
    return 0;
  }
  
  void OSM_Thread::alloc_mem_access_data()
  {
    for (MemAccessPool::iterator iter = _mem_access_pool.begin(); iter != _mem_access_pool.end(); ++iter) {
      (*iter)->alloc(16);
    }
  }
  
  UadlMemAccess &OSM_Thread::add_memory_request(InstrBase &instr)
  {
    return *instr.mem();
  }
  
  void OSM_Thread::set_memory_result(InstrBase &instr)
  {
    UadlMemAccess *ma = instr.mem();
    _iss->set_memory_result(0,ma->data(),ma->offset());
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
    if (!_log_mem || _ignore_mem) return;
    UadlMemAccess *ma = _mem_access_pool.alloc();
    ma->reset(_memId++,m);
    _cur_instr->add_mem(ma);
  }
  
  void OSM_Thread::getMemoryData(byte_t *data,UadlMemAccess *ma,addr_t addr,unsigned size)
  {
    unsigned data_size;
    if (!ma || !ma->has_valid_data()) return;
    
    unsigned s;
    if (ma->_nb > size) {
      s = ma->_nb;
    } else {
      s = size;
    }
    if (addr >= ma->_ra && addr < ma->_ra + s) {
      unsigned offset = addr - ma->_ra;
      data_size = min(ma->_ra + ma->_nb - addr,(addr_t)size);
      memcpy(data,ma->_data+offset,data_size);
    } else if (addr < ma->_ra && addr + s > ma->_ra) {
      unsigned offset = ma->_ra - addr;
      data_size = min((addr_t)ma->_nb,addr + size - ma->_ra);
      memcpy(data+offset,ma->_data+ma->_offset,data_size);
    }
  }
  
  void OSM_Thread::getMemoryData(CacheAccess type, addr_t addr, unsigned size, byte_t *data)
  {
    if (type == adl::CacheLogRead) {
      // get data from memory/cache
      if (Mem_obj.getMemoryData(type, addr, size, data)) {
        // augment with instruction memory data
        
        if (Inst_PMT *instr = mWB[0]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mWB[1]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mWB[2]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mWB[3]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mWB[0]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mWB[1]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mWB[2]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mWB[3]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mCX2[0]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mBR[0]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mEX0[0]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mEX0[1]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mLS1[0]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mBR[0]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mEX0[0]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mEX0[1]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mLS1[0]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mLS0[0]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mLS0[0]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mCX1[0]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mCX0[0]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mID[0]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mID[1]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mID[2]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mID[3]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mID[0]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mID[1]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mID[2]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mID[3]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mIF[0]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mIF[0]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mIF[0]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_PMT *instr = mIF[0]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
      } else {
        Mem_obj.getMemoryData(type, addr, size, data);
      }
    } else {
      Mem_obj.getMemoryData(type, addr, size, data);
    }
  }
  
  static unsigned initFetchSizes[] = { 4,  };
  FetchSizes OSM_Thread::_fetchSizes(&initFetchSizes[0],&initFetchSizes[1]);
  
  //
  // User-defined helper functions.
  //
  
   bool OSM_Thread:: smtIssueCheck ( InstrType & instr ) {
         unsigned tid = getChildId (  ) ;
         bool has_other ;
        switch ( tid ) {
              case 0 : has_other = ( top . s0 . t1 . mIF . size (  ) ) ;
              break ;
              case 1 : has_other = ( top . s0 . t0 . mIF . size (  ) ) ;
              break ;
              case 2 : has_other = ( top . s1 . t1 . mIF . size (  ) ) ;
              break ;
              case 3 : has_other = ( top . s1 . t0 . mIF . size (  ) ) ;
              break ;
          }
        return _iss-> smtProceed ( tid , has_other , 2 ) ;
  }
  
  // Factory function for instruction "add".
  Inst_PMT *OSM_Thread::get_instr_add(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<add_sfx_Src, add_sfx_Trg> *ic = (Inst_sfx<add_sfx_Src, add_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::SMT::Thread::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "addi".
  Inst_PMT *OSM_Thread::get_instr_addi(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addi_sfx_Src, addi_sfx_Trg> *ic = (Inst_sfx<addi_sfx_Src, addi_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::SMT::Thread::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "addic".
  Inst_PMT *OSM_Thread::get_instr_addic(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_addic' is not implemented in this model.");
  }
  
  // Factory function for instruction "addic.".
  Inst_PMT *OSM_Thread::get_instr_addic_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addic__sfx_Src, addic__sfx_Trg> *ic = (Inst_sfx<addic__sfx_Src, addic__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::SMT::Thread::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    ic->_masks[1] = 0xffffffff;
    ic->_masks[2] = 0x20000000;
    ic->_masks[3] = 0xffffffff;
    return ic;
  }
  
  // Factory function for instruction "addis".
  Inst_PMT *OSM_Thread::get_instr_addis(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addis_sfx_Src, addis_sfx_Trg> *ic = (Inst_sfx<addis_sfx_Src, addis_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::SMT::Thread::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "addme".
  Inst_PMT *OSM_Thread::get_instr_addme(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_addme' is not implemented in this model.");
  }
  
  // Factory function for instruction "addme.".
  Inst_PMT *OSM_Thread::get_instr_addme_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_addme_' is not implemented in this model.");
  }
  
  // Factory function for instruction "addmeo".
  Inst_PMT *OSM_Thread::get_instr_addmeo(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_addmeo' is not implemented in this model.");
  }
  
  // Factory function for instruction "addo".
  Inst_PMT *OSM_Thread::get_instr_addo(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_addo' is not implemented in this model.");
  }
  
  // Factory function for instruction "andi.".
  Inst_PMT *OSM_Thread::get_instr_andi_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<andi__sfx_Src, andi__sfx_Trg> *ic = (Inst_sfx<andi__sfx_Src, andi__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::SMT::Thread::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    ic->_masks[1] = 0xffffffff;
    ic->_masks[2] = 0xffffffff;
    return ic;
  }
  
  // Factory function for instruction "b".
  Inst_PMT *OSM_Thread::get_instr_b(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_serializing<b_serializing_Src, b_serializing_Trg> *ic = (Inst_serializing<b_serializing_Src, b_serializing_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::SMT::Thread::serializingClass,p);
    return ic;
  }
  
  // Factory function for instruction "bc".
  Inst_PMT *OSM_Thread::get_instr_bc(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_serializing<bc_serializing_Src, bc_serializing_Trg> *ic = (Inst_serializing<bc_serializing_Src, bc_serializing_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::SMT::Thread::serializingClass,p);
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
    Inst_serializing<bclr_serializing_Src, bclr_serializing_Trg> *ic = (Inst_serializing<bclr_serializing_Src, bclr_serializing_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::SMT::Thread::serializingClass,p);
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
    Inst_serializing<bl_serializing_Src, bl_serializing_Trg> *ic = (Inst_serializing<bl_serializing_Src, bl_serializing_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::SMT::Thread::serializingClass,p);
    return ic;
  }
  
  // Factory function for instruction "cmp".
  Inst_PMT *OSM_Thread::get_instr_cmp(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<cmp_sfx_Src, cmp_sfx_Trg> *ic = (Inst_sfx<cmp_sfx_Src, cmp_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::SMT::Thread::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    ic->_masks[1] = 0xffffffff;
    ic->_masks[2] = 0xffffffff;
    return ic;
  }
  
  // Factory function for instruction "cmpi".
  Inst_PMT *OSM_Thread::get_instr_cmpi(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<cmpi_sfx_Src, cmpi_sfx_Trg> *ic = (Inst_sfx<cmpi_sfx_Src, cmpi_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::SMT::Thread::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    ic->_masks[1] = 0xffffffff;
    ic->_masks[2] = 0xffffffff;
    return ic;
  }
  
  // Factory function for instruction "halt".
  Inst_PMT *OSM_Thread::get_instr_halt(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<DummyOp, DummyOp> *ic = (Inst_sfx<DummyOp, DummyOp>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::SMT::Thread::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "lbz".
  Inst_PMT *OSM_Thread::get_instr_lbz(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lbz_load_Src, lbz_load_Trg> *ic = (Inst_load<lbz_load_Src, lbz_load_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::SMT::Thread::loadClass,p);
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
    Inst_load<lhz_load_Src, lhz_load_Trg> *ic = (Inst_load<lhz_load_Src, lhz_load_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::SMT::Thread::loadClass,p);
    return ic;
  }
  
  // Factory function for instruction "lmw".
  Inst_PMT *OSM_Thread::get_instr_lmw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_lmw' is not implemented in this model.");
  }
  
  // Factory function for instruction "lwz".
  Inst_PMT *OSM_Thread::get_instr_lwz(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lwz_load_Src, lwz_load_Trg> *ic = (Inst_load<lwz_load_Src, lwz_load_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::SMT::Thread::loadClass,p);
    return ic;
  }
  
  // Factory function for instruction "lwzu".
  Inst_PMT *OSM_Thread::get_instr_lwzu(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lwzu_load_Src, lwzu_load_Trg> *ic = (Inst_load<lwzu_load_Src, lwzu_load_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::SMT::Thread::loadClass,p);
    return ic;
  }
  
  // Factory function for instruction "lwzx".
  Inst_PMT *OSM_Thread::get_instr_lwzx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lwzx_load_Src, lwzx_load_Trg> *ic = (Inst_load<lwzx_load_Src, lwzx_load_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::SMT::Thread::loadClass,p);
    return ic;
  }
  
  // Factory function for instruction "mfspr".
  Inst_PMT *OSM_Thread::get_instr_mfspr(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<mfspr_sfx_Src, mfspr_sfx_Trg> *ic = (Inst_sfx<mfspr_sfx_Src, mfspr_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::SMT::Thread::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "mtmsr".
  Inst_PMT *OSM_Thread::get_instr_mtmsr(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_mtmsr' is not implemented in this model.");
  }
  
  // Factory function for instruction "mtspr".
  Inst_PMT *OSM_Thread::get_instr_mtspr(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<mtspr_sfx_Src, mtspr_sfx_Trg> *ic = (Inst_sfx<mtspr_sfx_Src, mtspr_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::SMT::Thread::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "mulli".
  Inst_PMT *OSM_Thread::get_instr_mulli(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_cfx<mulli_cfx_Src, mulli_cfx_Trg> *ic = (Inst_cfx<mulli_cfx_Src, mulli_cfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::SMT::Thread::cfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "mullw".
  Inst_PMT *OSM_Thread::get_instr_mullw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_cfx<mullw_cfx_Src, mullw_cfx_Trg> *ic = (Inst_cfx<mullw_cfx_Src, mullw_cfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::SMT::Thread::cfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "or".
  Inst_PMT *OSM_Thread::get_instr_or(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<or_sfx_Src, or_sfx_Trg> *ic = (Inst_sfx<or_sfx_Src, or_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::SMT::Thread::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "ori".
  Inst_PMT *OSM_Thread::get_instr_ori(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<ori_sfx_Src, ori_sfx_Trg> *ic = (Inst_sfx<ori_sfx_Src, ori_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::SMT::Thread::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "oris".
  Inst_PMT *OSM_Thread::get_instr_oris(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_oris' is not implemented in this model.");
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
    ic->reset(this,_issueId++,_commits,adliss::PMT::SMT::Thread::sfxClass,p);
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
    ic->reset(this,_issueId++,_commits,adliss::PMT::SMT::Thread::sfxClass,p);
    ic->_masks[0] = 0x1f;
    return ic;
  }
  
  // Factory function for instruction "stb".
  Inst_PMT *OSM_Thread::get_instr_stb(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<stb_store_Src, DummyOp> *ic = (Inst_store<stb_store_Src, DummyOp>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::SMT::Thread::storeClass,p);
    return ic;
  }
  
  // Factory function for instruction "sth".
  Inst_PMT *OSM_Thread::get_instr_sth(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<sth_store_Src, DummyOp> *ic = (Inst_store<sth_store_Src, DummyOp>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::SMT::Thread::storeClass,p);
    return ic;
  }
  
  // Factory function for instruction "sthx".
  Inst_PMT *OSM_Thread::get_instr_sthx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<sthx_store_Src, DummyOp> *ic = (Inst_store<sthx_store_Src, DummyOp>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::SMT::Thread::storeClass,p);
    return ic;
  }
  
  // Factory function for instruction "stmw".
  Inst_PMT *OSM_Thread::get_instr_stmw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_stmw' is not implemented in this model.");
  }
  
  // Factory function for instruction "stw".
  Inst_PMT *OSM_Thread::get_instr_stw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<stw_store_Src, DummyOp> *ic = (Inst_store<stw_store_Src, DummyOp>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::SMT::Thread::storeClass,p);
    return ic;
  }
  
  // Factory function for instruction "stwu".
  Inst_PMT *OSM_Thread::get_instr_stwu(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<stwu_store_Src, stwu_store_Trg> *ic = (Inst_store<stwu_store_Src, stwu_store_Trg>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::SMT::Thread::storeClass,p);
    return ic;
  }
  
  // Factory function for instruction "stwx".
  Inst_PMT *OSM_Thread::get_instr_stwx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<stwx_store_Src, DummyOp> *ic = (Inst_store<stwx_store_Src, DummyOp>*)_instr_pool.alloc();
    ic->reset(this,_issueId++,_commits,adliss::PMT::SMT::Thread::storeClass,p);
    return ic;
  }
  
  // Factory function for instruction "decode_miss".
  Inst_PMT *OSM_Thread::get_instr_decode_miss(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_decode_miss' is not implemented in this model.");
  }
  
  // Factory function for instruction "fetch_exception".
  Inst_PMT *OSM_Thread::get_instr_fetch_exception(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_fetch_exception' is not implemented in this model.");
  }
  
  void OSM_Thread::exec(Inst_PMT&instr,Logger *logger)
  {
    instr._instr->exec(*_iss);
    if (logger) {
      logger->logMessage("exec");
      std::pair<adl::BranchType,bool> bi = instr.branch_info(*this);
      if (bi.first) logger->logBranch(bi.second,(bi.second) ? instr.branch_target() : 0);
    }
  }
  
  void OSM_Thread::exec_mem(Inst_PMT&instr,Logger *logger)
  {
    if (logger) { logger->logMessage("exec"); };
    set_cur_instr(&instr);
    instr._status = instr._instr->exec(*_iss);
  }
  
  void OSM_Thread::exec_mem_no_ma(Inst_PMT&instr,Logger *logger)
  {
    if (logger) { logger->logMessage("exec"); };
    struct IgnoreMem {
      IgnoreMem(OSM_Thread &x) : _x(x) { _x._ignore_mem = true; };
      ~IgnoreMem() { _x._ignore_mem = false; };
      OSM_Thread &_x;
    } ignore_mem(*this);
    
    instr._status = instr._instr->exec(*_iss);
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
    if (logger) logger->logMessage("commiting exception");
    if (instr.is_fetch_exception()) {
      commit_fetch_exception(instr);
    } else {
      _iss->CIA_write(instr._instr->addr());
      set_pc(instr._instr->addr() + instr._instr->size());
      instr._instr->commit_exceptions(*_iss);
    }
  };
  
  void OSM_Thread::cancel_all_mem(Inst_PMT &instr)
  {
    UadlMemAccess *ma = instr.mem();
    while (ma) {
      if (ma->valid_request_id()) {
        Mem_obj.cancel(ma->request_id(),true);
      }
      ma = ma->_next;
    }
  }
  
  void OSM_Thread::commit_fetch_exception(Inst_PMT&instr)
  {
    FetchExceptionData &fedata = *instr._fedata;
    _iss->CIA_write(fedata._addr);
    _iss->log_current_instr(&fedata);
    _iss->genExceptionsInternal(fedata._exdata,fedata._exitems);
    _fetchExceptionInProgress = false;
  }
  
  void OSM_Thread::exec_mBR(Inst_PMT&instr,Logger *logger,unsigned position)
  {
    exec(instr,logger);
    if (_iss->activeCount() == 0) flush_mBR(instr,logger,position);
  }
  
  void OSM_Thread::exec_mem_mBR(Inst_PMT&instr,Logger *logger,unsigned position)
  {
    exec_mem(instr,logger);
    if (_iss->activeCount() == 0) flush_mBR(instr,logger,position);
  }
  
  void OSM_Thread::exec_mCX0(Inst_PMT&instr,Logger *logger,unsigned position)
  {
    exec(instr,logger);
    if (_iss->activeCount() == 0) flush_mCX0(instr,logger,position);
  }
  
  void OSM_Thread::exec_mem_mCX0(Inst_PMT&instr,Logger *logger,unsigned position)
  {
    exec_mem(instr,logger);
    if (_iss->activeCount() == 0) flush_mCX0(instr,logger,position);
  }
  
  void OSM_Thread::exec_mCX1(Inst_PMT&instr,Logger *logger,unsigned position)
  {
    exec(instr,logger);
    if (_iss->activeCount() == 0) flush_mCX1(instr,logger,position);
  }
  
  void OSM_Thread::exec_mem_mCX1(Inst_PMT&instr,Logger *logger,unsigned position)
  {
    exec_mem(instr,logger);
    if (_iss->activeCount() == 0) flush_mCX1(instr,logger,position);
  }
  
  void OSM_Thread::exec_mCX2(Inst_PMT&instr,Logger *logger,unsigned position)
  {
    exec(instr,logger);
    if (_iss->activeCount() == 0) flush_mCX2(instr,logger,position);
  }
  
  void OSM_Thread::exec_mem_mCX2(Inst_PMT&instr,Logger *logger,unsigned position)
  {
    exec_mem(instr,logger);
    if (_iss->activeCount() == 0) flush_mCX2(instr,logger,position);
  }
  
  void OSM_Thread::exec_mEX0(Inst_PMT&instr,Logger *logger,unsigned position)
  {
    exec(instr,logger);
    if (_iss->activeCount() == 0) flush_mEX0(instr,logger,position);
  }
  
  void OSM_Thread::exec_mem_mEX0(Inst_PMT&instr,Logger *logger,unsigned position)
  {
    exec_mem(instr,logger);
    if (_iss->activeCount() == 0) flush_mEX0(instr,logger,position);
  }
  
  void OSM_Thread::exec_mID(Inst_PMT&instr,Logger *logger,unsigned position)
  {
    exec(instr,logger);
    if (_iss->activeCount() == 0) flush_mID(instr,logger,position);
  }
  
  void OSM_Thread::exec_mem_mID(Inst_PMT&instr,Logger *logger,unsigned position)
  {
    exec_mem(instr,logger);
    if (_iss->activeCount() == 0) flush_mID(instr,logger,position);
  }
  
  void OSM_Thread::exec_mIF(Inst_PMT&instr,Logger *logger,unsigned position)
  {
    exec(instr,logger);
    if (_iss->activeCount() == 0) flush_mIF(instr,logger,position);
  }
  
  void OSM_Thread::exec_mem_mIF(Inst_PMT&instr,Logger *logger,unsigned position)
  {
    exec_mem(instr,logger);
    if (_iss->activeCount() == 0) flush_mIF(instr,logger,position);
  }
  
  void OSM_Thread::exec_mLS0(Inst_PMT&instr,Logger *logger,unsigned position)
  {
    exec(instr,logger);
    if (_iss->activeCount() == 0) flush_mLS0(instr,logger,position);
  }
  
  void OSM_Thread::exec_mem_mLS0(Inst_PMT&instr,Logger *logger,unsigned position)
  {
    exec_mem(instr,logger);
    if (_iss->activeCount() == 0) flush_mLS0(instr,logger,position);
  }
  
  void OSM_Thread::exec_mLS1(Inst_PMT&instr,Logger *logger,unsigned position)
  {
    exec(instr,logger);
    if (_iss->activeCount() == 0) flush_mLS1(instr,logger,position);
  }
  
  void OSM_Thread::exec_mem_mLS1(Inst_PMT&instr,Logger *logger,unsigned position)
  {
    exec_mem(instr,logger);
    if (_iss->activeCount() == 0) flush_mLS1(instr,logger,position);
  }
  
  void OSM_Thread::exec_mWB(Inst_PMT&instr,Logger *logger,unsigned position)
  {
    exec(instr,logger);
    if (_iss->activeCount() == 0) flush_mWB(instr,logger,position);
  }
  
  void OSM_Thread::exec_mem_mWB(Inst_PMT&instr,Logger *logger,unsigned position)
  {
    exec_mem(instr,logger);
    if (_iss->activeCount() == 0) flush_mWB(instr,logger,position);
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
    if (ic.iid() >= _resetFetchId) {
      set_pc(ic._instr->addr() + ic._instr->size());
    }
    taken_flush_mWB(ic,logger,position);
  }
  
  void OSM_Thread::taken_flush_mWB(Inst_PMT&ic,Logger *logger,unsigned position,bool given_addr,addr_t addr)
  {
    _fetchException = 0;
    if (ic.iid() >= _resetFetchId) {
      if (!given_addr && ic._instr) {
        if (ic.branch_taken(*this)) {
          set_pc(ic.branch_target());
        } else {
          set_pc(ic._instr->addr() + ic._instr->size());
        }
      }
    }
    unsigned orig_size = mWB.size();
    position = 0;
    for (int i = position+1; i < 4; ++i) {
      if (Inst_PMT *instr = mWB.remove_instr(i,getChildId())) {
        if (instr->is_alive()) {
          instr->flush(*this,logger);
          discard(instr);
          instr->set_dead();
        }
      }
    }
    mWB.cleanup(orig_size);
    {
      unsigned orig_size = mCX2.size();
      for (unsigned i = 0; i != 1; ++i) {
        if (Inst_PMT *instr = mCX2[i]) {
          if (instr->is_alive()) {
            if (mCX2.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mCX2.remove_instr(i);
          }
        }
      }
      mCX2.cleanup(orig_size);
    }
    {
      unsigned orig_size = mBR.size();
      for (unsigned i = 0; i != 1; ++i) {
        if (Inst_PMT *instr = mBR[i]) {
          if (instr->is_alive()) {
            if (mBR.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mBR.remove_instr(i);
          }
        }
      }
      mBR.cleanup(orig_size);
    }
    {
      unsigned orig_size = mEX0.size();
      for (unsigned i = 0; i != 2; ++i) {
        if (Inst_PMT *instr = mEX0[i]) {
          if (instr->is_alive()) {
            if (mEX0.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mEX0.remove_instr(i);
          }
        }
      }
      mEX0.cleanup(orig_size);
    }
    {
      unsigned orig_size = mLS1.size();
      for (unsigned i = 0; i != 1; ++i) {
        if (Inst_PMT *instr = mLS1[i]) {
          if (instr->is_alive()) {
            if (mLS1.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mLS1.remove_instr(i);
          }
        }
      }
      mLS1.cleanup(orig_size);
    }
    {
      unsigned orig_size = mLS0.size();
      for (unsigned i = 0; i != 1; ++i) {
        if (Inst_PMT *instr = mLS0[i]) {
          if (instr->is_alive()) {
            if (mLS0.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mLS0.remove_instr(i);
          }
        }
      }
      mLS0.cleanup(orig_size);
    }
    {
      unsigned orig_size = mCX1.size();
      for (unsigned i = 0; i != 1; ++i) {
        if (Inst_PMT *instr = mCX1[i]) {
          if (instr->is_alive()) {
            if (mCX1.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mCX1.remove_instr(i);
          }
        }
      }
      mCX1.cleanup(orig_size);
    }
    {
      unsigned orig_size = mCX0.size();
      for (unsigned i = 0; i != 1; ++i) {
        if (Inst_PMT *instr = mCX0[i]) {
          if (instr->is_alive()) {
            if (mCX0.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mCX0.remove_instr(i);
          }
        }
      }
      mCX0.cleanup(orig_size);
    }
    {
      unsigned orig_size = mID.size();
      for (unsigned i = 0; i != 4; ++i) {
        if (Inst_PMT *instr = mID[i]) {
          if (instr->is_alive()) {
            if (mID.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mID.remove_instr(i);
          }
        }
      }
      mID.cleanup(orig_size);
    }
    for (unsigned i = 0; i != 1; ++i) {
      if (Inst_PMT *instr = mIF[i]) {
        if (instr->is_alive()) {
          (instr->_flush)(instr,*this,logger);
          discard(instr);
          instr->set_dead();
        }
      }
    }
    mIF.reset();
    flush_issue_stage(logger,position,given_addr,addr);
  }
  
  void OSM_Thread::flush_mCX2(Inst_PMT&ic,Logger *logger,unsigned position)
  {
    if (ic.iid() >= _resetFetchId) {
      set_pc(ic._instr->addr() + ic._instr->size());
    }
    taken_flush_mCX2(ic,logger,position);
  }
  
  void OSM_Thread::taken_flush_mCX2(Inst_PMT&ic,Logger *logger,unsigned position,bool given_addr,addr_t addr)
  {
    _fetchException = 0;
    if (ic.iid() >= _resetFetchId) {
      if (!given_addr && ic._instr) {
        if (ic.branch_taken(*this)) {
          set_pc(ic.branch_target());
        } else {
          set_pc(ic._instr->addr() + ic._instr->size());
        }
      }
    }
    unsigned orig_size = mCX2.size();
    mCX2.cleanup(orig_size);
    {
      unsigned orig_size = mBR.size();
      for (unsigned i = 0; i != 1; ++i) {
        if (Inst_PMT *instr = mBR[i]) {
          if (instr->is_alive()) {
            if (mBR.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mBR.remove_instr(i);
          }
        }
      }
      mBR.cleanup(orig_size);
    }
    {
      unsigned orig_size = mEX0.size();
      for (unsigned i = 0; i != 2; ++i) {
        if (Inst_PMT *instr = mEX0[i]) {
          if (instr->is_alive()) {
            if (mEX0.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mEX0.remove_instr(i);
          }
        }
      }
      mEX0.cleanup(orig_size);
    }
    {
      unsigned orig_size = mLS1.size();
      for (unsigned i = 0; i != 1; ++i) {
        if (Inst_PMT *instr = mLS1[i]) {
          if (instr->is_alive()) {
            if (mLS1.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mLS1.remove_instr(i);
          }
        }
      }
      mLS1.cleanup(orig_size);
    }
    {
      unsigned orig_size = mLS0.size();
      for (unsigned i = 0; i != 1; ++i) {
        if (Inst_PMT *instr = mLS0[i]) {
          if (instr->is_alive()) {
            if (mLS0.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mLS0.remove_instr(i);
          }
        }
      }
      mLS0.cleanup(orig_size);
    }
    {
      unsigned orig_size = mCX1.size();
      for (unsigned i = 0; i != 1; ++i) {
        if (Inst_PMT *instr = mCX1[i]) {
          if (instr->is_alive()) {
            if (mCX1.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mCX1.remove_instr(i);
          }
        }
      }
      mCX1.cleanup(orig_size);
    }
    {
      unsigned orig_size = mCX0.size();
      for (unsigned i = 0; i != 1; ++i) {
        if (Inst_PMT *instr = mCX0[i]) {
          if (instr->is_alive()) {
            if (mCX0.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mCX0.remove_instr(i);
          }
        }
      }
      mCX0.cleanup(orig_size);
    }
    {
      unsigned orig_size = mID.size();
      for (unsigned i = 0; i != 4; ++i) {
        if (Inst_PMT *instr = mID[i]) {
          if (instr->is_alive()) {
            if (mID.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mID.remove_instr(i);
          }
        }
      }
      mID.cleanup(orig_size);
    }
    for (unsigned i = 0; i != 1; ++i) {
      if (Inst_PMT *instr = mIF[i]) {
        if (instr->is_alive()) {
          (instr->_flush)(instr,*this,logger);
          discard(instr);
          instr->set_dead();
        }
      }
    }
    mIF.reset();
    flush_issue_stage(logger,position,given_addr,addr);
  }
  
  void OSM_Thread::flush_mBR(Inst_PMT&ic,Logger *logger,unsigned position)
  {
    if (ic.iid() >= _resetFetchId) {
      set_pc(ic._instr->addr() + ic._instr->size());
    }
    taken_flush_mBR(ic,logger,position);
  }
  
  void OSM_Thread::taken_flush_mBR(Inst_PMT&ic,Logger *logger,unsigned position,bool given_addr,addr_t addr)
  {
    _fetchException = 0;
    if (ic.iid() >= _resetFetchId) {
      if (!given_addr && ic._instr) {
        if (ic.branch_taken(*this)) {
          set_pc(ic.branch_target());
        } else {
          set_pc(ic._instr->addr() + ic._instr->size());
        }
      }
    }
    unsigned orig_size = mBR.size();
    mBR.cleanup(orig_size);
    {
      unsigned orig_size = mEX0.size();
      for (unsigned i = 0; i != 2; ++i) {
        if (Inst_PMT *instr = mEX0[i]) {
          if (instr->is_alive()) {
            if (mEX0.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mEX0.remove_instr(i);
          }
        }
      }
      mEX0.cleanup(orig_size);
    }
    {
      unsigned orig_size = mLS1.size();
      for (unsigned i = 0; i != 1; ++i) {
        if (Inst_PMT *instr = mLS1[i]) {
          if (instr->is_alive()) {
            if (mLS1.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mLS1.remove_instr(i);
          }
        }
      }
      mLS1.cleanup(orig_size);
    }
    {
      unsigned orig_size = mLS0.size();
      for (unsigned i = 0; i != 1; ++i) {
        if (Inst_PMT *instr = mLS0[i]) {
          if (instr->is_alive()) {
            if (mLS0.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mLS0.remove_instr(i);
          }
        }
      }
      mLS0.cleanup(orig_size);
    }
    {
      unsigned orig_size = mCX1.size();
      for (unsigned i = 0; i != 1; ++i) {
        if (Inst_PMT *instr = mCX1[i]) {
          if (instr->is_alive()) {
            if (mCX1.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mCX1.remove_instr(i);
          }
        }
      }
      mCX1.cleanup(orig_size);
    }
    {
      unsigned orig_size = mCX0.size();
      for (unsigned i = 0; i != 1; ++i) {
        if (Inst_PMT *instr = mCX0[i]) {
          if (instr->is_alive()) {
            if (mCX0.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mCX0.remove_instr(i);
          }
        }
      }
      mCX0.cleanup(orig_size);
    }
    {
      unsigned orig_size = mID.size();
      for (unsigned i = 0; i != 4; ++i) {
        if (Inst_PMT *instr = mID[i]) {
          if (instr->is_alive()) {
            if (mID.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mID.remove_instr(i);
          }
        }
      }
      mID.cleanup(orig_size);
    }
    for (unsigned i = 0; i != 1; ++i) {
      if (Inst_PMT *instr = mIF[i]) {
        if (instr->is_alive()) {
          (instr->_flush)(instr,*this,logger);
          discard(instr);
          instr->set_dead();
        }
      }
    }
    mIF.reset();
    flush_issue_stage(logger,position,given_addr,addr);
  }
  
  void OSM_Thread::flush_mEX0(Inst_PMT&ic,Logger *logger,unsigned position)
  {
    if (ic.iid() >= _resetFetchId) {
      set_pc(ic._instr->addr() + ic._instr->size());
    }
    taken_flush_mEX0(ic,logger,position);
  }
  
  void OSM_Thread::taken_flush_mEX0(Inst_PMT&ic,Logger *logger,unsigned position,bool given_addr,addr_t addr)
  {
    _fetchException = 0;
    if (ic.iid() >= _resetFetchId) {
      if (!given_addr && ic._instr) {
        if (ic.branch_taken(*this)) {
          set_pc(ic.branch_target());
        } else {
          set_pc(ic._instr->addr() + ic._instr->size());
        }
      }
    }
    unsigned orig_size = mEX0.size();
    mEX0.cleanup(orig_size);
    {
      unsigned orig_size = mLS1.size();
      for (unsigned i = 0; i != 1; ++i) {
        if (Inst_PMT *instr = mLS1[i]) {
          if (instr->is_alive()) {
            if (mLS1.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mLS1.remove_instr(i);
          }
        }
      }
      mLS1.cleanup(orig_size);
    }
    {
      unsigned orig_size = mLS0.size();
      for (unsigned i = 0; i != 1; ++i) {
        if (Inst_PMT *instr = mLS0[i]) {
          if (instr->is_alive()) {
            if (mLS0.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mLS0.remove_instr(i);
          }
        }
      }
      mLS0.cleanup(orig_size);
    }
    {
      unsigned orig_size = mCX1.size();
      for (unsigned i = 0; i != 1; ++i) {
        if (Inst_PMT *instr = mCX1[i]) {
          if (instr->is_alive()) {
            if (mCX1.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mCX1.remove_instr(i);
          }
        }
      }
      mCX1.cleanup(orig_size);
    }
    {
      unsigned orig_size = mCX0.size();
      for (unsigned i = 0; i != 1; ++i) {
        if (Inst_PMT *instr = mCX0[i]) {
          if (instr->is_alive()) {
            if (mCX0.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mCX0.remove_instr(i);
          }
        }
      }
      mCX0.cleanup(orig_size);
    }
    {
      unsigned orig_size = mID.size();
      for (unsigned i = 0; i != 4; ++i) {
        if (Inst_PMT *instr = mID[i]) {
          if (instr->is_alive()) {
            if (mID.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mID.remove_instr(i);
          }
        }
      }
      mID.cleanup(orig_size);
    }
    for (unsigned i = 0; i != 1; ++i) {
      if (Inst_PMT *instr = mIF[i]) {
        if (instr->is_alive()) {
          (instr->_flush)(instr,*this,logger);
          discard(instr);
          instr->set_dead();
        }
      }
    }
    mIF.reset();
    flush_issue_stage(logger,position,given_addr,addr);
  }
  
  void OSM_Thread::flush_mLS1(Inst_PMT&ic,Logger *logger,unsigned position)
  {
    if (ic.iid() >= _resetFetchId) {
      set_pc(ic._instr->addr() + ic._instr->size());
    }
    taken_flush_mLS1(ic,logger,position);
  }
  
  void OSM_Thread::taken_flush_mLS1(Inst_PMT&ic,Logger *logger,unsigned position,bool given_addr,addr_t addr)
  {
    _fetchException = 0;
    if (ic.iid() >= _resetFetchId) {
      if (!given_addr && ic._instr) {
        if (ic.branch_taken(*this)) {
          set_pc(ic.branch_target());
        } else {
          set_pc(ic._instr->addr() + ic._instr->size());
        }
      }
    }
    unsigned orig_size = mLS1.size();
    mLS1.cleanup(orig_size);
    {
      unsigned orig_size = mLS0.size();
      for (unsigned i = 0; i != 1; ++i) {
        if (Inst_PMT *instr = mLS0[i]) {
          if (instr->is_alive()) {
            if (mLS0.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mLS0.remove_instr(i);
          }
        }
      }
      mLS0.cleanup(orig_size);
    }
    {
      unsigned orig_size = mCX1.size();
      for (unsigned i = 0; i != 1; ++i) {
        if (Inst_PMT *instr = mCX1[i]) {
          if (instr->is_alive()) {
            if (mCX1.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mCX1.remove_instr(i);
          }
        }
      }
      mCX1.cleanup(orig_size);
    }
    {
      unsigned orig_size = mCX0.size();
      for (unsigned i = 0; i != 1; ++i) {
        if (Inst_PMT *instr = mCX0[i]) {
          if (instr->is_alive()) {
            if (mCX0.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mCX0.remove_instr(i);
          }
        }
      }
      mCX0.cleanup(orig_size);
    }
    {
      unsigned orig_size = mID.size();
      for (unsigned i = 0; i != 4; ++i) {
        if (Inst_PMT *instr = mID[i]) {
          if (instr->is_alive()) {
            if (mID.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mID.remove_instr(i);
          }
        }
      }
      mID.cleanup(orig_size);
    }
    for (unsigned i = 0; i != 1; ++i) {
      if (Inst_PMT *instr = mIF[i]) {
        if (instr->is_alive()) {
          (instr->_flush)(instr,*this,logger);
          discard(instr);
          instr->set_dead();
        }
      }
    }
    mIF.reset();
    flush_issue_stage(logger,position,given_addr,addr);
  }
  
  void OSM_Thread::flush_mLS0(Inst_PMT&ic,Logger *logger,unsigned position)
  {
    if (ic.iid() >= _resetFetchId) {
      set_pc(ic._instr->addr() + ic._instr->size());
    }
    taken_flush_mLS0(ic,logger,position);
  }
  
  void OSM_Thread::taken_flush_mLS0(Inst_PMT&ic,Logger *logger,unsigned position,bool given_addr,addr_t addr)
  {
    _fetchException = 0;
    if (ic.iid() >= _resetFetchId) {
      if (!given_addr && ic._instr) {
        if (ic.branch_taken(*this)) {
          set_pc(ic.branch_target());
        } else {
          set_pc(ic._instr->addr() + ic._instr->size());
        }
      }
    }
    unsigned orig_size = mLS0.size();
    mLS0.cleanup(orig_size);
    {
      unsigned orig_size = mCX1.size();
      for (unsigned i = 0; i != 1; ++i) {
        if (Inst_PMT *instr = mCX1[i]) {
          if (instr->is_alive()) {
            if (mCX1.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mCX1.remove_instr(i);
          }
        }
      }
      mCX1.cleanup(orig_size);
    }
    {
      unsigned orig_size = mCX0.size();
      for (unsigned i = 0; i != 1; ++i) {
        if (Inst_PMT *instr = mCX0[i]) {
          if (instr->is_alive()) {
            if (mCX0.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mCX0.remove_instr(i);
          }
        }
      }
      mCX0.cleanup(orig_size);
    }
    {
      unsigned orig_size = mID.size();
      for (unsigned i = 0; i != 4; ++i) {
        if (Inst_PMT *instr = mID[i]) {
          if (instr->is_alive()) {
            if (mID.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mID.remove_instr(i);
          }
        }
      }
      mID.cleanup(orig_size);
    }
    for (unsigned i = 0; i != 1; ++i) {
      if (Inst_PMT *instr = mIF[i]) {
        if (instr->is_alive()) {
          (instr->_flush)(instr,*this,logger);
          discard(instr);
          instr->set_dead();
        }
      }
    }
    mIF.reset();
    flush_issue_stage(logger,position,given_addr,addr);
  }
  
  void OSM_Thread::flush_mCX1(Inst_PMT&ic,Logger *logger,unsigned position)
  {
    if (ic.iid() >= _resetFetchId) {
      set_pc(ic._instr->addr() + ic._instr->size());
    }
    taken_flush_mCX1(ic,logger,position);
  }
  
  void OSM_Thread::taken_flush_mCX1(Inst_PMT&ic,Logger *logger,unsigned position,bool given_addr,addr_t addr)
  {
    _fetchException = 0;
    if (ic.iid() >= _resetFetchId) {
      if (!given_addr && ic._instr) {
        if (ic.branch_taken(*this)) {
          set_pc(ic.branch_target());
        } else {
          set_pc(ic._instr->addr() + ic._instr->size());
        }
      }
    }
    unsigned orig_size = mCX1.size();
    mCX1.cleanup(orig_size);
    {
      unsigned orig_size = mCX0.size();
      for (unsigned i = 0; i != 1; ++i) {
        if (Inst_PMT *instr = mCX0[i]) {
          if (instr->is_alive()) {
            if (mCX0.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mCX0.remove_instr(i);
          }
        }
      }
      mCX0.cleanup(orig_size);
    }
    {
      unsigned orig_size = mID.size();
      for (unsigned i = 0; i != 4; ++i) {
        if (Inst_PMT *instr = mID[i]) {
          if (instr->is_alive()) {
            if (mID.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mID.remove_instr(i);
          }
        }
      }
      mID.cleanup(orig_size);
    }
    for (unsigned i = 0; i != 1; ++i) {
      if (Inst_PMT *instr = mIF[i]) {
        if (instr->is_alive()) {
          (instr->_flush)(instr,*this,logger);
          discard(instr);
          instr->set_dead();
        }
      }
    }
    mIF.reset();
    flush_issue_stage(logger,position,given_addr,addr);
  }
  
  void OSM_Thread::flush_mCX0(Inst_PMT&ic,Logger *logger,unsigned position)
  {
    if (ic.iid() >= _resetFetchId) {
      set_pc(ic._instr->addr() + ic._instr->size());
    }
    taken_flush_mCX0(ic,logger,position);
  }
  
  void OSM_Thread::taken_flush_mCX0(Inst_PMT&ic,Logger *logger,unsigned position,bool given_addr,addr_t addr)
  {
    _fetchException = 0;
    if (ic.iid() >= _resetFetchId) {
      if (!given_addr && ic._instr) {
        if (ic.branch_taken(*this)) {
          set_pc(ic.branch_target());
        } else {
          set_pc(ic._instr->addr() + ic._instr->size());
        }
      }
    }
    unsigned orig_size = mCX0.size();
    mCX0.cleanup(orig_size);
    {
      unsigned orig_size = mID.size();
      for (unsigned i = 0; i != 4; ++i) {
        if (Inst_PMT *instr = mID[i]) {
          if (instr->is_alive()) {
            if (mID.remove_instr(i,getChildId())) {
              (instr->_flush)(instr,*this,logger);
              discard(instr);
              instr->set_dead();
            }
          }
          else {
            mID.remove_instr(i);
          }
        }
      }
      mID.cleanup(orig_size);
    }
    for (unsigned i = 0; i != 1; ++i) {
      if (Inst_PMT *instr = mIF[i]) {
        if (instr->is_alive()) {
          (instr->_flush)(instr,*this,logger);
          discard(instr);
          instr->set_dead();
        }
      }
    }
    mIF.reset();
    flush_issue_stage(logger,position,given_addr,addr);
  }
  
  void OSM_Thread::flush_mID(Inst_PMT&ic,Logger *logger,unsigned position)
  {
    if (ic.iid() >= _resetFetchId) {
      set_pc(ic._instr->addr() + ic._instr->size());
    }
    taken_flush_mID(ic,logger,position);
  }
  
  void OSM_Thread::taken_flush_mID(Inst_PMT&ic,Logger *logger,unsigned position,bool given_addr,addr_t addr)
  {
    _fetchException = 0;
    if (ic.iid() >= _resetFetchId) {
      if (!given_addr && ic._instr) {
        if (ic.branch_taken(*this)) {
          set_pc(ic.branch_target());
        } else {
          set_pc(ic._instr->addr() + ic._instr->size());
        }
      }
    }
    unsigned orig_size = mID.size();
    position = 0;
    for (int i = position+1; i < 4; ++i) {
      if (Inst_PMT *instr = mID.remove_instr(i,getChildId())) {
        if (instr->is_alive()) {
          instr->flush(*this,logger);
          discard(instr);
          instr->set_dead();
        }
      }
    }
    mID.cleanup(orig_size);
    for (unsigned i = 0; i != 1; ++i) {
      if (Inst_PMT *instr = mIF[i]) {
        if (instr->is_alive()) {
          (instr->_flush)(instr,*this,logger);
          discard(instr);
          instr->set_dead();
        }
      }
    }
    mIF.reset();
    flush_issue_stage(logger,position,given_addr,addr);
  }
  
  void OSM_Thread::flush_mIF(Inst_PMT&ic,Logger *logger,unsigned position)
  {
    if (ic.iid() >= _resetFetchId) {
      set_pc(ic._instr->addr() + ic._instr->size());
    }
    taken_flush_mIF(ic,logger,position);
  }
  
  void OSM_Thread::taken_flush_mIF(Inst_PMT&ic,Logger *logger,unsigned position,bool given_addr,addr_t addr)
  {
    _fetchException = 0;
    if (ic.iid() >= _resetFetchId) {
      if (!given_addr && ic._instr) {
        if (ic.branch_taken(*this)) {
          set_pc(ic.branch_target());
        } else {
          set_pc(ic._instr->addr() + ic._instr->size());
        }
      }
    }
    mIF.update_tail();
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
    // Hack for genExceptions() to get correct ISS state.
    _iss->NIA_write(Fetcher.nextInstrAddr());
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
    return Issue.empty() && mIF.has_space();
  }
  
  Inst_PMT *OSM_Thread::issueInstruction()
  {
    Inst_PMT *instr = 0;
    
    if (FetchData fd = Fetcher.instrData(_iss->tracing_on())) {
      instr = _iss->decode(fd._ea, fd._data, 0, fd._size, *this);
      Fetcher.decodeUpdate(instr, instr ? instr->size() : 0);
      if (!instr) return 0;
    } else if (!Fetcher.empty()) {
      return 0;
    }
    else if (hasFetchException()) {
      _fetchExceptionInProgress = true;
      Inst_PMT *fe = get_instr_fetch_exception(0);
      fe->set_fetch_exception(_fetchException,_fetchExceptionHandle,Fetcher.nextFetchAddr(),_fetchLogData);
      _fetchLogData = 0;
      _fetchException = 0;
      return fe;
    }
    
    return instr;
  }
  
  void OSM_Thread::issue(InstrPacket *instr)
  {
    Fetcher.removeInstr(instr->size());
  }
  
  void OSM_Thread::commit(Inst_PMT*instr)
  {
    if (instr->is_alive()) {
      addr_t ia = instr->addr();
      _iss->free_packet(instr->_instr);
      free_instr(instr);
      incrCommitCount();
      check_breakpoints(ia);
      instr->set_dead();
    }
  }
  
  void OSM_Thread::commit_checked(Inst_PMT*instr)
  {
    if (instr->is_alive()) {
      bool check_bp = false;
      addr_t ia;
      if (instr->is_fetch_exception()) {
        _iss->log_commit(instr->_fedata);
        instr->discard_fetch_exception();
      } else {
        check_bp = true;
        ia = instr->addr();
        _iss->free_packet(instr->_instr);
      }
      free_instr(instr);
      incrCommitCount();
      if (check_bp) {
        check_breakpoints(ia);
      }
      instr->set_dead();
    }
  }
  
  void OSM_Thread::discard(Inst_PMT*instr)
  {
    if (instr->is_alive()) {
      cancel_all_mem(*instr);
      free_all_mem(*instr);
      _iss->discard_packet(instr->_instr);
      free_instr(instr);
      instr->set_dead();
    }
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
    catch (adliss::PMT::SMT::Thread::Exception e) {
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
          case 4:
          do {
            prefetchStatus = _iss->prefetch<4>(ea);
          } while (prefetchStatus == adl::prSkip);
          
          break;
          
          default: assert(false); // should never reach here
        }
        
        assert(prefetchStatus != adl::prNotActive);
      }
      
    }
    catch (adliss::PMT::SMT::Thread::Exception e) {
      _fetchExceptionInProgress = false;
      _fetchException = oldExceptionFlag ^ (unsigned)e;
      _fetchExceptionHandle = _iss->getExceptionData(_fetchException);
      _fetchLogData = _iss->log_get_pending_queue();
      _iss->cancelExceptions(_fetchException);
      throw 0;
    }
    
    ra = _iss->memory_request(0)._ra;
    
    return true;
  }
  
  
  bool OSM_Thread::fetchCacheEnabled(addr_t addr)
  {
    return false;
  }
  
  bool OSM_Thread::issueInstructions()
  {
    if (!Issue.empty() && Issue.instr()->action(*this,((Tracing && logPipeline()) ? Issue.logger(false,0): 0),0)) {
      Issue.remove_instr();
    }
    if (!hasExternException()) {
      while (_iss->is_active() && !fetchExceptionInProgress() && issueInquire()) {
        if ((Issue.add_instr(issueInstruction()))) {
          _iss->log_instr_issue_time(Issue.instr()->_instr,currTime());
          if (Issue.instr()->action(*this,((Tracing && logPipeline()) ? Issue.logger(false,0): 0),0)) {;
            Issue.remove_instr();
          }
        } else {
          break;
        }
      }
    } else if (instr_alloc_count() == 0) {
      raiseExternException();
    }
    return true;
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
  }
  
  void OSM_Thread::report_and_clear_top(ostream &o,const string &pfx)
  {
    if (_iss->hasPendingExceptions()) {
      *_out << pfx << "[external exception]\n";
    }
    Fetcher.report_and_clear(o,pfx);
    Issue.report_and_clear(o,pfx);
  }
  
  void OSM_Thread::report_and_clear_bottom(ostream &o,const string &pfx)
  {
    L1d_obj.report_and_clear(o,pfx);
    *_out << '\n';
    L1i_obj.report_and_clear(o,pfx);
    *_out << '\n';
  }
  
  adl::IssNode& OSM_Thread::iss() { return *_iss; }
  const adl::IssNode& OSM_Thread::iss() const { return *_iss; }
  ISS_PMT_SMT_Thread& OSM_Thread::get_iss() { return *_iss; }
  
  void OSM_Thread::preCycle()
  {
    L1d_obj.preCycle();
    L1i_obj.preCycle();
    _iss->updateLevelSensitiveExceptions();
    Fetcher.receiveInstrData();
    if (logPipeline()) {
      Fetcher.instrBufReport();
    }
  }
  
  void OSM_Thread::postCycle()
  {
    if (_iss->activeCount()) Fetcher.sendFetchRequests();
    L1i_obj.postCycle();
    L1d_obj.postCycle();
    if (logMemory()) L1i_obj.logAllRequests();
    if (logMemory()) L1d_obj.logAllRequests();
    incrTime();
  }
  
  OSM_Thread::~OSM_Thread() {
  }
  
  OSM_Thread::OSM_Thread(unsigned &id, unsigned &thread_id, std::ostream &out, ttime_t startTime, PipelineStage<1,0,OSM_Thread,Inst_PMT,1> &p_mBR, PipelineStage<1,0,OSM_Thread,Inst_PMT,1> &p_mCX0, PipelineStage<1,0,OSM_Thread,Inst_PMT,1> &p_mCX1, PipelineStage<1,0,OSM_Thread,Inst_PMT,1> &p_mCX2, PipelineStage<2,0,OSM_Thread,Inst_PMT,1> &p_mEX0, PipelineStage<4,0,OSM_Thread,Inst_PMT,1> &p_mID, PipelineStage<1,0,OSM_Thread,Inst_PMT,1> &p_mLS0, PipelineStage<1,0,OSM_Thread,Inst_PMT,1> &p_mLS1, PipelineStage<4,0,OSM_Thread,Inst_PMT,1> &p_mWB, Mem_mem_t &p_Mem_obj, RegFileResource<uint32_t,4> &p_BARs, RegResource<uint32_t> &p_CfxIndex, RegFileResource<uint32_t,4> &p_FOOs, RegResource<uint32_t> &p_HID0, RegFileResource<uint32_t,4> &p_MSRs, RegFileResource<uint32_t,4> &p_NIAs, RegResource<uint32_t> &p_SmtIndex, RegFileResource<uint32_t,4> &p_TCR, RegResource<uint32_t> &p_TEN, ForwardingPath<Inst_PMT,0,1,10,32> &p_FwdCB, bool &p__stall, OSM_PMT &top_p, ISS_PMT_SMT_Thread *iss) : 
  UadlArch(startTime,out), 
  _breakpoint(false), 
  _thread_id(thread_id++), 
  _iss(iss), 
  mBR(p_mBR), 
  mCX0(p_mCX0), 
  mCX1(p_mCX1), 
  mCX2(p_mCX2), 
  mEX0(p_mEX0), 
  mID(p_mID), 
  mIF(getPath(_iss,".") + "mIF"), 
  mLS0(p_mLS0), 
  mLS1(p_mLS1), 
  mWB(p_mWB), 
  Mem_obj(p_Mem_obj), 
  Mem(Mem_obj), 
  _log_mem(true), 
  L1d_obj(*this,Mem_obj), 
  L1d(L1d_obj), 
  L1i_obj(*this,Mem_obj), 
  L1i(L1i_obj), 
  BARs(p_BARs), 
  BootAddr("BootAddr"), 
  CBTEST1("CBTEST1"), 
  CBTEST2("CBTEST2"), 
  CCR("CCR"), 
  CIA("CIA"), 
  CR("CR"), 
  CTR("CTR"), 
  CfxIndex(p_CfxIndex), 
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
  MSR("MSR"), 
  MSRs(p_MSRs), 
  NIA("NIA"), 
  NIAs(p_NIAs), 
  PTN("PTN"), 
  SRR0("SRR0"), 
  SRR1("SRR1"), 
  STEST("STEST"), 
  STUFFs("STUFFs"), 
  SmtIndex(p_SmtIndex), 
  TCR(p_TCR), 
  TEN(p_TEN), 
  UFOO("UFOO"), 
  XER("XER"), 
  XERU("XERU"), 
  FwdCB(p_FwdCB), 
  Fetcher(*this,getPath(_iss,".")+"Fetcher",8,Mem_obj,0,_fetchSizes,&Mem_obj), 
  _cur_instr(0), 
  _ignore_mem(false), 
  Issue("         "), 
  _fetchException(0), 
  _fetchExceptionInProgress(false), 
  _fetchLogData(0), 
  _stall(p__stall), 
  _stage_loggers(33), 
  top(top_p)
  {
    alloc_mem_access_data();
    _iss->_uarch = this;
    _id_offset = 35 * _thread_id;
    Mem_obj.set_level(0);
    Mem_obj.set_level(0);
  };
  
  void OSM_Thread::reset()
  {
    UadlArch::reset();
    _breakpoint = false;
    mIF.reset();
    _mem_access_pool.reset();
    _log_mem = true;
    L1d_obj.reset();
    L1i_obj.reset();
    BootAddr.reset();
    CBTEST1.reset();
    CBTEST2.reset();
    CCR.reset();
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
    Fetcher.full_reset();
    _cur_instr = 0;
    _ignore_mem = false;
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
    L1d_obj.reset_has_data();
    L1i_obj.reset_has_data();
    reset_fetch();
    _hadReset = false;
  }
  
  void OSM_Thread::postSim()
  {
    L1i_obj.postSim();
    L1d_obj.postSim();
  }
  
  unsigned OSM_Thread::fetchStalls() const { return Fetcher.fetchStalls(); }
  
  unsigned OSM_Thread::dmemStalls() const { return Mem_obj.memStalls(0); }
  
  unsigned OSM_Thread::imemStalls() const { return Mem_obj.memStalls(0); }
  
  unsigned OSM_SMT::getInstrClassAttrByName(const std::string &attr)
  {
    return 0;
  }
  
  bool OSM_SMT::issueInstructions()
  {
    t0.issueInstructions();
    t1.issueInstructions();
    return true;
  }
  
  uint64_t OSM_SMT::instrCount() const
  {
    return t0.instrCount() + t1.instrCount();
  }
  
  void OSM_SMT::update_tracing()
  {
    unsigned tflags ATTRIBUTE_UNUSED = (_out) ? LogAll : LogNone;
    _flags = top.tracing_flags();
    t0.update_tracing();
    t1.update_tracing();
  }
  
  void OSM_SMT::report_and_clear_top(ostream &o,const string &pfx)
  {
    t0.report_and_clear_top(o,pfx);
    t1.report_and_clear_top(o,pfx);
  }
  
  void OSM_SMT::report_and_clear_bottom(ostream &o,const string &pfx)
  {
    t0.report_and_clear_bottom(o,pfx);
    t1.report_and_clear_bottom(o,pfx);
  }
  
  adl::IssNode& OSM_SMT::iss() { return *_iss; }
  const adl::IssNode& OSM_SMT::iss() const { return *_iss; }
  ISS_PMT_SMT& OSM_SMT::get_iss() { return *_iss; }
  
  void OSM_SMT::preCycle()
  {
    t0.preCycle();
    t1.preCycle();
  }
  
  void OSM_SMT::postCycle()
  {
    FwdCB.postCycle();
    t0.postCycle();
    t1.postCycle();
    incrTime();
  }
  
  OSM_SMT::~OSM_SMT() {
  }
  
  OSM_SMT::OSM_SMT(unsigned &id, unsigned &thread_id, std::ostream &out, ttime_t startTime, PipelineStage<1,0,OSM_Thread,Inst_PMT,1> &p_mCX0, PipelineStage<1,0,OSM_Thread,Inst_PMT,1> &p_mCX1, PipelineStage<1,0,OSM_Thread,Inst_PMT,1> &p_mCX2, Mem_mem_t &p_Mem_obj, RegFileResource<uint32_t,4> &p_BARs, RegResource<uint32_t> &p_CfxIndex, RegFileResource<uint32_t,4> &p_FOOs, RegResource<uint32_t> &p_HID0, RegFileResource<uint32_t,4> &p_MSRs, RegFileResource<uint32_t,4> &p_NIAs, RegFileResource<uint32_t,4> &p_TCR, RegResource<uint32_t> &p_TEN, bool &p__stall, OSM_PMT &top_p, ISS_PMT_SMT *iss) : 
  UadlArch(startTime,out), 
  _iss(iss), 
  mBR(getPath(_iss,".") + "mBR"), 
  mCX0(p_mCX0), 
  mCX1(p_mCX1), 
  mCX2(p_mCX2), 
  mEX0(getPath(_iss,".") + "mEX0"), 
  mID(getPath(_iss,".") + "mID"), 
  mLS0(getPath(_iss,".") + "mLS0"), 
  mLS1(getPath(_iss,".") + "mLS1"), 
  mWB(getPath(_iss,".") + "mWB"), 
  Mem_obj(p_Mem_obj), 
  BARs(p_BARs), 
  CfxIndex(p_CfxIndex), 
  FOOs(p_FOOs), 
  HID0(p_HID0), 
  MSRs(p_MSRs), 
  NIAs(p_NIAs), 
  SmtIndex("SmtIndex"), 
  TCR(p_TCR), 
  TEN(p_TEN), 
  FwdCB("FwdCB"), 
  _stall(p__stall), 
  _stage_loggers(33), 
  top(top_p), 
  t0(id, thread_id, out, startTime, mBR, mCX0, mCX1, mCX2, mEX0, mID, mLS0, mLS1, mWB, Mem_obj, BARs, CfxIndex, FOOs, HID0, MSRs, NIAs, SmtIndex, TCR, TEN, FwdCB, _stall, top_p,static_cast<ISS_PMT_SMT_Thread*>(_iss->t0)), 
  t1(id, thread_id, out, startTime, mBR, mCX0, mCX1, mCX2, mEX0, mID, mLS0, mLS1, mWB, Mem_obj, BARs, CfxIndex, FOOs, HID0, MSRs, NIAs, SmtIndex, TCR, TEN, FwdCB, _stall, top_p,static_cast<ISS_PMT_SMT_Thread*>(_iss->t1))
  {
    Mem_obj.set_level(0);
    Mem_obj.set_level(0);
  };
  
  void OSM_SMT::reset()
  {
    UadlArch::reset();
    mBR.reset();
    mEX0.reset();
    mID.reset();
    mLS0.reset();
    mLS1.reset();
    mWB.reset();
    SmtIndex.reset();
    FwdCB.reset();
    t0.reset();
    t1.reset();
  }
  
  bool OSM_SMT::active() const
  {
    if (breakpoint()) return false;
    return t0.active() || t1.active();
  }
  
  bool OSM_SMT::is_active() const
  {
    return active();
  }
  
  void OSM_SMT::setBreakpoint()
  {
    t0.setBreakpoint();
    t1.setBreakpoint();
  }
  
  void OSM_SMT::clearBreakpoint()
  {
    t0.clearBreakpoint();
    t1.clearBreakpoint();
  }
  
  bool OSM_SMT::breakpoint () const
  {
    return t0.breakpoint() || t1.breakpoint();
  }
  
  bool OSM_SMT::hasBreakpoint() const
  {
    return breakpoint();
  }
  
  void OSM_SMT::preRun()
  {
    t0.preRun();
    t1.preRun();
    _hadReset = false;
  }
  
  void OSM_SMT::postSim()
  {
    t0.postSim();
    t1.postSim();
  }
  
  unsigned OSM_SMT::fetchStalls() const {
    return t0.fetchStalls() + t1.fetchStalls();
  }
  
  unsigned OSM_SMT::dmemStalls() const {
    return t0.dmemStalls() + t1.dmemStalls();
  }
  
  unsigned OSM_SMT::imemStalls() const {
    return t0.imemStalls() + t1.imemStalls();
  }
  
  unsigned OSM_PMT::getInstrClassAttrByName(const std::string &attr)
  {
    return 0;
  }
  
  bool OSM_PMT::issueInstructions()
  {
    s0.issueInstructions();
    s1.issueInstructions();
    return true;
  }
  
  uint64_t OSM_PMT::instrCount() const
  {
    return s0.instrCount() + s1.instrCount();
  }
  
  void OSM_PMT::update_tracing()
  {
    unsigned tflags ATTRIBUTE_UNUSED = (_out) ? LogAll : LogNone;
    s0.update_tracing();
    s1.update_tracing();
    if (logPipeline()) {
      s0.mWB.createLogger(_logclient,tflags);
      _stage_loggers.set(0,s0.mWB.logger(0));
      _stage_loggers.set(1,s0.mWB.logger(1));
      _stage_loggers.set(2,s0.mWB.logger(2));
      _stage_loggers.set(3,s0.mWB.logger(3));
      s1.mWB.createLogger(_logclient,tflags);
      _stage_loggers.set(4,s1.mWB.logger(0));
      _stage_loggers.set(5,s1.mWB.logger(1));
      _stage_loggers.set(6,s1.mWB.logger(2));
      _stage_loggers.set(7,s1.mWB.logger(3));
      mCX2.createLogger(_logclient,tflags);
      _stage_loggers.set(8,mCX2.logger(0));
      s0.mBR.createLogger(_logclient,tflags);
      _stage_loggers.set(9,s0.mBR.logger(0));
      s0.mEX0.createLogger(_logclient,tflags);
      _stage_loggers.set(10,s0.mEX0.logger(0));
      _stage_loggers.set(11,s0.mEX0.logger(1));
      s0.mLS1.createLogger(_logclient,tflags);
      _stage_loggers.set(12,s0.mLS1.logger(0));
      s1.mBR.createLogger(_logclient,tflags);
      _stage_loggers.set(13,s1.mBR.logger(0));
      s1.mEX0.createLogger(_logclient,tflags);
      _stage_loggers.set(14,s1.mEX0.logger(0));
      _stage_loggers.set(15,s1.mEX0.logger(1));
      s1.mLS1.createLogger(_logclient,tflags);
      _stage_loggers.set(16,s1.mLS1.logger(0));
      s0.mLS0.createLogger(_logclient,tflags);
      _stage_loggers.set(17,s0.mLS0.logger(0));
      s1.mLS0.createLogger(_logclient,tflags);
      _stage_loggers.set(18,s1.mLS0.logger(0));
      mCX1.createLogger(_logclient,tflags);
      _stage_loggers.set(19,mCX1.logger(0));
      mCX0.createLogger(_logclient,tflags);
      _stage_loggers.set(20,mCX0.logger(0));
      s0.mID.createLogger(_logclient,tflags);
      _stage_loggers.set(21,s0.mID.logger(0));
      _stage_loggers.set(22,s0.mID.logger(1));
      _stage_loggers.set(23,s0.mID.logger(2));
      _stage_loggers.set(24,s0.mID.logger(3));
      s1.mID.createLogger(_logclient,tflags);
      _stage_loggers.set(25,s1.mID.logger(0));
      _stage_loggers.set(26,s1.mID.logger(1));
      _stage_loggers.set(27,s1.mID.logger(2));
      _stage_loggers.set(28,s1.mID.logger(3));
      s1.t1.mIF.createLogger(_logclient,tflags);
      _stage_loggers.set(29,s1.t1.mIF.logger(0));
      s1.t0.mIF.createLogger(_logclient,tflags);
      _stage_loggers.set(30,s1.t0.mIF.logger(0));
      s0.t1.mIF.createLogger(_logclient,tflags);
      _stage_loggers.set(31,s0.t1.mIF.logger(0));
      s0.t0.mIF.createLogger(_logclient,tflags);
      _stage_loggers.set(32,s0.t0.mIF.logger(0));
    } else {
      s0.mWB.removeLogger();
      s1.mWB.removeLogger();
      mCX2.removeLogger();
      s0.mBR.removeLogger();
      s0.mEX0.removeLogger();
      s0.mLS1.removeLogger();
      s1.mBR.removeLogger();
      s1.mEX0.removeLogger();
      s1.mLS1.removeLogger();
      s0.mLS0.removeLogger();
      s1.mLS0.removeLogger();
      mCX1.removeLogger();
      mCX0.removeLogger();
      s0.mID.removeLogger();
      s1.mID.removeLogger();
      s1.t1.mIF.removeLogger();
      s1.t0.mIF.removeLogger();
      s0.t1.mIF.removeLogger();
      s0.t0.mIF.removeLogger();
      _stage_loggers.reset();
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
    s0.t0.mIF.report_and_clear(o,pfx);
    s0.t1.mIF.report_and_clear(o,pfx);
    s1.t0.mIF.report_and_clear(o,pfx);
    s1.t1.mIF.report_and_clear(o,pfx);
    s1.mID.report_and_clear(o,pfx);
    s0.mID.report_and_clear(o,pfx);
    mCX0.report_and_clear(o,pfx);
    mCX1.report_and_clear(o,pfx);
    s1.mLS0.report_and_clear(o,pfx);
    s0.mLS0.report_and_clear(o,pfx);
    s1.mLS1.report_and_clear(o,pfx);
    s1.mEX0.report_and_clear(o,pfx);
    s1.mBR.report_and_clear(o,pfx);
    s0.mLS1.report_and_clear(o,pfx);
    s0.mEX0.report_and_clear(o,pfx);
    s0.mBR.report_and_clear(o,pfx);
    mCX2.report_and_clear(o,pfx);
    s1.mWB.report_and_clear(o,pfx);
    s0.mWB.report_and_clear(o,pfx);
    *_out << '\n';
    report_and_clear_bottom(o,pfx);
  }
  
  void OSM_PMT::report_and_clear_top(ostream &o,const string &pfx)
  {
    s0.report_and_clear_top(o,pfx);
    s1.report_and_clear_top(o,pfx);
  }
  
  void OSM_PMT::report_and_clear_bottom(ostream &o,const string &pfx)
  {
    s0.report_and_clear_bottom(o,pfx);
    s1.report_and_clear_bottom(o,pfx);
    Mem_obj.report_and_clear(o,pfx);
    *_out << '\n';
  }
  
  adl::IssNode& OSM_PMT::iss() { return *_iss; }
  const adl::IssNode& OSM_PMT::iss() const { return *_iss; }
  ISS_PMT& OSM_PMT::get_iss() { return *_iss; }
  
  void OSM_PMT::preCycle()
  {
    Mem_obj.preCycle();
    s0.preCycle();
    s1.preCycle();
  }
  
  void OSM_PMT::postCycle()
  {
    Mem_obj.postCycle();
    s0.postCycle();
    s1.postCycle();
    if (logMemory()) Mem_obj.logAllRequests();
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
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(0+s0.mWB.pos(0)) : 0);
        Logger *logger1 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(0+s0.mWB.pos(1)) : 0);
        Logger *logger2 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(0+s0.mWB.pos(2)) : 0);
        Logger *logger3 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(0+s0.mWB.pos(3)) : 0);
        if (!s0.mWB.empty() && (s0.mWB.head()->is_in_stage(&s0.mWB))) {
          if (s0.mWB.head()->action(*s0.mWB.head()->_uarch,logger0,0)) {
            if (!s0.mWB.empty() && (s0.mWB.head()->is_in_stage(&s0.mWB))) {
              if (s0.mWB.head()->action(*s0.mWB.head()->_uarch,logger1,1)) {
              } else {
                _stall = true;
              }
            }
          } else {
            _stall = true;
          }
        }
      }
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(4+s1.mWB.pos(0)) : 0);
        Logger *logger1 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(4+s1.mWB.pos(1)) : 0);
        Logger *logger2 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(4+s1.mWB.pos(2)) : 0);
        Logger *logger3 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(4+s1.mWB.pos(3)) : 0);
        if (!s1.mWB.empty() && (s1.mWB.head()->is_in_stage(&s1.mWB))) {
          if (s1.mWB.head()->action(*s1.mWB.head()->_uarch,logger0,0)) {
            if (!s1.mWB.empty() && (s1.mWB.head()->is_in_stage(&s1.mWB))) {
              if (s1.mWB.head()->action(*s1.mWB.head()->_uarch,logger1,1)) {
              } else {
                _stall = true;
              }
            }
          } else {
            _stall = true;
          }
        }
      }
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(8+mCX2.pos(0)) : 0);
        if (!mCX2.empty()) {
          if (mCX2.head()->action(*mCX2.head()->_uarch,logger0,0)) {
          } else {
            _stall = true;
          }
        }
      }
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(9+s0.mBR.pos(0)) : 0);
        if (!s0.mBR.empty()) {
          if (s0.mBR.head()->action(*s0.mBR.head()->_uarch,logger0,0)) {
          } else {
            _stall = true;
          }
        }
      }
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(10+s0.mEX0.pos(0)) : 0);
        Logger *logger1 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(10+s0.mEX0.pos(1)) : 0);
        unsigned sindex = 0;
        if (s0.mEX0[sindex]) {
          if (!s0.mEX0[sindex]->action(*s0.mEX0[sindex]->_uarch,logger0,sindex)) {
            _stall = true;
            ++sindex;
          }
          if (s0.mEX0[sindex]) {
            if (!s0.mEX0[sindex]->action(*s0.mEX0[sindex]->_uarch,logger1,sindex)) {
              _stall = true;
              ++sindex;
            }
          }
        }
      }
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(12+s0.mLS1.pos(0)) : 0);
        if (!s0.mLS1.empty()) {
          if (s0.mLS1.head()->action(*s0.mLS1.head()->_uarch,logger0,0)) {
          } else {
            _stall = true;
          }
        }
      }
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(13+s1.mBR.pos(0)) : 0);
        if (!s1.mBR.empty()) {
          if (s1.mBR.head()->action(*s1.mBR.head()->_uarch,logger0,0)) {
          } else {
            _stall = true;
          }
        }
      }
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(14+s1.mEX0.pos(0)) : 0);
        Logger *logger1 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(14+s1.mEX0.pos(1)) : 0);
        unsigned sindex = 0;
        if (s1.mEX0[sindex]) {
          if (!s1.mEX0[sindex]->action(*s1.mEX0[sindex]->_uarch,logger0,sindex)) {
            _stall = true;
            ++sindex;
          }
          if (s1.mEX0[sindex]) {
            if (!s1.mEX0[sindex]->action(*s1.mEX0[sindex]->_uarch,logger1,sindex)) {
              _stall = true;
              ++sindex;
            }
          }
        }
      }
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(16+s1.mLS1.pos(0)) : 0);
        if (!s1.mLS1.empty()) {
          if (s1.mLS1.head()->action(*s1.mLS1.head()->_uarch,logger0,0)) {
          } else {
            _stall = true;
          }
        }
      }
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(17+s0.mLS0.pos(0)) : 0);
        if (!s0.mLS0.empty()) {
          if (s0.mLS0.head()->action(*s0.mLS0.head()->_uarch,logger0,0)) {
          } else {
            _stall = true;
          }
        }
      }
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(18+s1.mLS0.pos(0)) : 0);
        if (!s1.mLS0.empty()) {
          if (s1.mLS0.head()->action(*s1.mLS0.head()->_uarch,logger0,0)) {
          } else {
            _stall = true;
          }
        }
      }
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(19+mCX1.pos(0)) : 0);
        if (!mCX1.empty()) {
          if (mCX1.head()->action(*mCX1.head()->_uarch,logger0,0)) {
          } else {
            _stall = true;
          }
        }
      }
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(20+mCX0.pos(0)) : 0);
        if (!mCX0.empty()) {
          if (mCX0.head()->action(*mCX0.head()->_uarch,logger0,0)) {
          } else {
            _stall = true;
          }
        }
      }
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(21+s0.mID.pos(0)) : 0);
        Logger *logger1 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(21+s0.mID.pos(1)) : 0);
        Logger *logger2 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(21+s0.mID.pos(2)) : 0);
        Logger *logger3 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(21+s0.mID.pos(3)) : 0);
        if (!s0.mID.empty()) {
          if (s0.mID.head()->action(*s0.mID.head()->_uarch,logger0,0)) {
            if (!s0.mID.empty()) {
              if (s0.mID.head()->action(*s0.mID.head()->_uarch,logger1,1)) {
              } else {
                _stall = true;
              }
            }
          } else {
            _stall = true;
          }
        }
      }
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(25+s1.mID.pos(0)) : 0);
        Logger *logger1 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(25+s1.mID.pos(1)) : 0);
        Logger *logger2 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(25+s1.mID.pos(2)) : 0);
        Logger *logger3 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(25+s1.mID.pos(3)) : 0);
        if (!s1.mID.empty()) {
          if (s1.mID.head()->action(*s1.mID.head()->_uarch,logger0,0)) {
            if (!s1.mID.empty()) {
              if (s1.mID.head()->action(*s1.mID.head()->_uarch,logger1,1)) {
              } else {
                _stall = true;
              }
            }
          } else {
            _stall = true;
          }
        }
      }
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(29+s1.t1.mIF.pos(0)) : 0);
        if (!s1.t1.mIF.empty()) {
          if (s1.t1.mIF.head()->action(*s1.t1.mIF.head()->_uarch,logger0,0)) {
          } else {
            _stall = true;
          }
        }
      }
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(30+s1.t0.mIF.pos(0)) : 0);
        if (!s1.t0.mIF.empty()) {
          if (s1.t0.mIF.head()->action(*s1.t0.mIF.head()->_uarch,logger0,0)) {
          } else {
            _stall = true;
          }
        }
      }
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(31+s0.t1.mIF.pos(0)) : 0);
        if (!s0.t1.mIF.empty()) {
          if (s0.t1.mIF.head()->action(*s0.t1.mIF.head()->_uarch,logger0,0)) {
          } else {
            _stall = true;
          }
        }
      }
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(32+s0.t0.mIF.pos(0)) : 0);
        if (!s0.t0.mIF.empty()) {
          if (s0.t0.mIF.head()->action(*s0.t0.mIF.head()->_uarch,logger0,0)) {
          } else {
            _stall = true;
          }
        }
      }
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
  mCX0(getPath(_iss,".") + "mCX0"), 
  mCX1(getPath(_iss,".") + "mCX1"), 
  mCX2(getPath(_iss,".") + "mCX2"), 
  Mem_obj(*this, logWarnings()), 
  BARs("BARs"), 
  CfxIndex("CfxIndex"), 
  FOOs("FOOs"), 
  HID0("HID0"), 
  MSRs("MSRs"), 
  NIAs("NIAs"), 
  TCR("TCR"), 
  TEN("TEN"), 
  _stall(false), 
  _stage_loggers(33), 
  top(*this), 
  s0(id, thread_id, out, startTime, mCX0, mCX1, mCX2, Mem_obj, BARs, CfxIndex, FOOs, HID0, MSRs, NIAs, TCR, TEN, _stall, *this,static_cast<ISS_PMT_SMT*>(_iss->s0)), 
  s1(id, thread_id, out, startTime, mCX0, mCX1, mCX2, Mem_obj, BARs, CfxIndex, FOOs, HID0, MSRs, NIAs, TCR, TEN, _stall, *this,static_cast<ISS_PMT_SMT*>(_iss->s1))
  {
    Mem_obj.set_level(0);
    Mem_obj.set_level(0);
  };
  
  void OSM_PMT::reset()
  {
    UadlArch::reset();
    mCX0.reset();
    mCX1.reset();
    mCX2.reset();
    Mem_obj.reset();
    BARs.reset();
    CfxIndex.reset();
    FOOs.reset();
    HID0.reset();
    MSRs.reset();
    NIAs.reset();
    TCR.reset();
    TEN.reset();
    _stall = false;
    s0.reset();
    s1.reset();
  }
  
  bool OSM_PMT::active() const
  {
    if (breakpoint()) return false;
    return s0.active() || s1.active();
  }
  
  bool OSM_PMT::is_active() const
  {
    return active();
  }
  
  void OSM_PMT::setBreakpoint()
  {
    s0.setBreakpoint();
    s1.setBreakpoint();
  }
  
  void OSM_PMT::clearBreakpoint()
  {
    s0.clearBreakpoint();
    s1.clearBreakpoint();
  }
  
  bool OSM_PMT::breakpoint () const
  {
    return s0.breakpoint() || s1.breakpoint();
  }
  
  bool OSM_PMT::hasBreakpoint() const
  {
    return breakpoint();
  }
  
  void OSM_PMT::preRun()
  {
    s0.preRun();
    s1.preRun();
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
    s0.postSim();
    s1.postSim();
  }
  
  unsigned OSM_PMT::fetchStalls() const {
    return s0.fetchStalls() + s1.fetchStalls();
  }
  
  unsigned OSM_PMT::dmemStalls() const {
    return s0.dmemStalls() + s1.dmemStalls();
  }
  
  unsigned OSM_PMT::imemStalls() const {
    return s0.imemStalls() + s1.imemStalls();
  }
  
  UadlArch *createArch(const string &name, unsigned &id,UadlMemoryIf *memory = NULL,ttime_t startTime = 0)
  {
    if (memory) {
      cerr << "Warning:  An external memory was specified but the model was created to use internal memory.\n";}
    
    unsigned thread_id = 0;
    return new OSM_PMT(id,thread_id,std::cout,startTime);
  }
  
  
  
} // namespace uadl

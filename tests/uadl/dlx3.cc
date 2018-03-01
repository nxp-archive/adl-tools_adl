//
// ADL/Uadl version 3.17.2
//
// Cycle-accurate/approximate model generated for P
//

#ifdef UADL_SEPARATE_COMPILE
#  define ADL_HEADER
#endif
#include "dlx3-iss.cc"
#define _OSM_P_IMPLEMENTATION_H
#include "dlx3.h"

using namespace std;
using namespace adl;
using namespace adliss;




void adliss::P::synchronizeActivation()
{
  _uarch->force_reset_fetch();
}

uint32_t adliss::P::getMemoryData32(CacheAccess type, addr_t addr)
{
  byte_t data[4] = {0,0,0,0};
  
  _uarch->getMemoryData(type, addr, 4, data);
  return byte_read32(data, 0);
}

void adliss::P::handle_memory_request(unsigned memId, const adl::MemAccess &ma) {
  _uarch->handle_memory_request(memId,ma);
};

namespace uadl {
  
  const bool Tracing = true;
  
  static bool no_move_p(Inst_P *,OSM_P &,Logger *,unsigned) ATTRIBUTE_UNUSED;
  static bool no_move_p(Inst_P *,OSM_P &,Logger *,unsigned) {
    return true;
  }
  
  struct DummyOp {
    DummyOp &thisInstr() { return *this; };
    
    bool can_read(OSM_P &,Inst_P &,Logger *) {
      return true;
    }
    
    unsigned mask(Inst_P &,unsigned = 0) const { return 0; }
    
    unsigned value(OSM_P &,Inst_P &,Logger *) const { return 0; }
    
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
    
    bool read(OSM_P &,Inst_P &,Logger *) {
      return false;
    }
    
    void record_sources(OSM_P &,Inst_P &,Logger *) {}
    
    void log_read(UadlArch &,Inst_P &,Logger *,const string &,unsigned) {
    }
    
    bool can_write(OSM_P &,Inst_P &,Logger *) {
      return true;
    }
    
    void allocate(OSM_P &,Inst_P &,Logger *) {
    }
    
    void deallocate(OSM_P &,Inst_P &,Logger *) {
    }
    
    void write(OSM_P &,Inst_P &,Logger *) {
    }
    unsigned field_value(OSM_P &,Inst_P &,Logger *) const { return 0; }
    
    bool is_real(OSM_P &,Inst_P &,Logger *) const { return false; }
    
    std::string name(UadlArch &,Inst_P &,unsigned = 0,unsigned = 0) const { return std::string(); }
    
    RegId id(UadlArch &,const Inst_P &,unsigned = 0,unsigned id = 0) const { return RegInvalidId; }
    
    unsigned slot(unsigned = 0,unsigned = 0) const { return 0; }
    
    unsigned num_sub_ops() const { return 0; }
    
  };
  
  //
  // Operands for instruction class cfx
  //
  
  // Operands:
  //   (src: GPR(RA))
  struct mulli_cfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic);
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        if (tag == 0) {
          read(uarch,ic,logger);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name());
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(0);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(0)._index); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(0).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(0) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 0; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (trg: GPR(RT))
  struct mulli_cfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(1)._index);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(1)._index);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),1);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(1);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(1)._index); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(1).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(1) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 1; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  struct mullw_cfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: ic._instr->read_operand(uarch.get_iss(),0);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(0);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(1)._index);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned sub_op = 0) const {
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
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  // Operands:
  //   (trg: GPR(RT))
  struct mullw_cfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(2)._index);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(2)._index);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),2);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(2)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(2);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(2)._index); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(2).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(2) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 2; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  //
  // Instruction class cfx
  //
  template <class Src_t, class Trg_t>
  struct Inst_cfx : public Inst_P {
    
    typedef struct Inst_cfx<Src_t, Trg_t> ThisClass;
    
    Inst_cfx &thisInstr() { return *this; };
    
    Src_t Src;
    Trg_t Trg;
    void reset(unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_P::reset(iid,cid,ic);
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      _stage = (PipelineStageBase*)1;
    }
    
    void deallocate_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      Src.deallocate(uarch,ic,logger);
      Trg.deallocate(uarch,ic,logger);
    }
    
    void write_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      Src.write(uarch,ic,logger);
      Trg.write(uarch,ic,logger);
    }
    
    bool exec_done() const { return true; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool change_state_S_EX0__S_EX0p(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX0 -> S_EX0p"); }
      _action = (StateFunc)action_S_EX0p_p;
      _flush = (FlushFunc)flush_S_EX0p_p;
      return action_S_EX0p(uarch,logger,position);
    }
    
    bool change_state_S_EX0p__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX0p -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mEX0.remove_instr_indep(position);
      _stage = &uarch.mWB;
      return true;
    }
    
    bool change_state_S_ID__S_EX0(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX0"); }
      _action = (StateFunc)action_S_EX0_p;
      _flush = (FlushFunc)flush_S_EX0_p;
      uarch.mID.remove_instr();
      _stage = &uarch.mEX0;
      uarch.mEX0.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.issue(_instr);
      _stage = &uarch.mID;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool change_state_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    // State action S_EX0
    bool action_S_EX0(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( Src.read_avail(uarch,*this,logger, uarch.FwdCB ) ) {
            set_latency ( 6 ) ;
            return change_state_S_EX0__S_EX0p(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EX0_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX0(uarch,logger,position);
    }
    
    // State action S_EX0p
    bool action_S_EX0p(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.no_capacity_check(uarch,*this,logger) && check_latency(logger) ) {
            uarch.exec(*this,logger) ;
            return change_state_S_EX0p__S_WB(uarch,logger,position) ;
      } else {
            decr_latency (  ) ;
      }
      return false;
    }
    static bool action_S_EX0p_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX0p(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( Trg.can_write(uarch,*this,logger) && uarch.mWB.has_space(uarch,*this,logger) && uarch.mEX0.has_space(uarch,*this,logger) ) {
            Src.record_sources(uarch,*this,logger) ;
            Trg.allocate(uarch,*this,logger) ;
            uarch.mWB.add_instr(uarch,*this,logger) ;
            return change_state_S_ID__S_EX0(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.has_space(uarch,*this,logger) ) {
            return change_state_S_INIT__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      {
            uarch.FwdCB.write(uarch,*this,logger, Trg ) ;
            write_ops(uarch,*this,logger) ;
            return change_state_S_WB__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WB(uarch,logger,position);
    }
    
    // Flush action S_EX0
    void flush_S_EX0(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_EX0_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX0(uarch,logger);
    }
    
    // Flush action S_EX0p
    void flush_S_EX0p(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_EX0p_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX0p(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class load
  //
  
  // Operands:
  //   (src: GPR(RA))
  struct lbz_load_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic);
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        if (tag == 0) {
          read(uarch,ic,logger);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name());
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(0);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(0)._index); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(0).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(0) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 0; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (trg: GPR(RT))
  struct lbz_load_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(1)._index);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(1)._index);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),1);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(1);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(1)._index); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(1).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(1) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 1; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
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
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  struct lwzx_load_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: ic._instr->read_operand(uarch.get_iss(),0);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(0);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(1)._index);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned sub_op = 0) const {
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
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  // Operands:
  //   (trg: GPR(RT))
  struct lwzx_load_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(2)._index);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(2)._index);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),2);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(2)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(2);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(2)._index); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(2).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(2) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 2; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (src: GPR(RA . uint32 (  )))
  struct lwzu_load_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic);
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        if (tag == 0) {
          read(uarch,ic,logger);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name());
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(0);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(0)._index); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(0).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(0) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 0; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (trg: GPR(RA . uint32 (  )))
  //   (trg: GPR(RT))
  struct lwzu_load_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(0)._index);
      uarch.GPR.allocate(uarch,ic,logger,ic.op(1)._index);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(0)._index);
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(1)._index);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),0);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      ic._instr->commit(uarch.get_iss(),1);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(0);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(1)._index);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned sub_op = 0) const {
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
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  //
  // Instruction class load
  //
  template <class Src_t, class Trg_t>
  struct Inst_load : public Inst_P {
    
    typedef struct Inst_load<Src_t, Trg_t> ThisClass;
    
    Inst_load &thisInstr() { return *this; };
    
    Src_t Src;
    Trg_t Trg;
    void reset(unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_P::reset(iid,cid,ic);
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      _stage = (PipelineStageBase*)1;
    }
    
    void deallocate_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      Src.deallocate(uarch,ic,logger);
      Trg.deallocate(uarch,ic,logger);
    }
    
    void write_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      Src.write(uarch,ic,logger);
      Trg.write(uarch,ic,logger);
    }
    
    bool exec_done() const { return _status == adl::MemAccess::opNone; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool change_state_S_ID__S_LS0(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_LS0"); }
      _action = (StateFunc)action_S_LS0_p;
      _flush = (FlushFunc)flush_S_LS0_p;
      uarch.mID.remove_instr();
      _stage = &uarch.mLS0;
      uarch.mLS0.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.issue(_instr);
      _stage = &uarch.mID;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool change_state_S_LS0__S_LS1(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_LS0 -> S_LS1"); }
      _action = (StateFunc)action_S_LS1_p;
      _flush = (FlushFunc)flush_S_LS1_p;
      uarch.mLS0.remove_instr();
      _stage = &uarch.mLS1;
      uarch.mLS1.add_instr(this);
      return true;
    }
    
    bool change_state_S_LS1__S_LS1p(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_LS1 -> S_LS1p"); }
      _action = (StateFunc)action_S_LS1p_p;
      _flush = (FlushFunc)flush_S_LS1p_p;
      return action_S_LS1p(uarch,logger,position);
    }
    
    bool change_state_S_LS1p__S_LS1(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_LS1p -> S_LS1"); }
      _action = (StateFunc)action_S_LS1_p;
      _flush = (FlushFunc)flush_S_LS1_p;
      return action_S_LS1(uarch,logger,position);
    }
    
    bool change_state_S_LS1p__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_LS1p -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mLS1.remove_instr();
      _stage = &uarch.mWB;
      return true;
    }
    
    bool change_state_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      uarch.commit(this);
      return true;
    }
    
    // State action S_ID
    bool action_S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( Trg.can_write(uarch,*this,logger) && uarch.mWB.has_space(uarch,*this,logger) && uarch.mLS0.has_space(uarch,*this,logger) ) {
            Src.record_sources(uarch,*this,logger) ;
            Trg.allocate(uarch,*this,logger) ;
            uarch.mWB.add_instr(uarch,*this,logger) ;
            return change_state_S_ID__S_LS0(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.has_space(uarch,*this,logger) ) {
            return change_state_S_INIT__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_LS0
    bool action_S_LS0(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( Src.read_avail(uarch,*this,logger, uarch.FwdCB ) && uarch.mLS1.has_space(uarch,*this,logger) && uarch.mMEM.can_request_read(uarch,*this,logger) ) {
            uarch.exec_mem(*this,logger) ;
            uarch.mMEM.send_read_request(uarch,*this,logger) ;
            return change_state_S_LS0__S_LS1(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_LS0_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_LS0(uarch,logger,position);
    }
    
    // State action S_LS1
    bool action_S_LS1(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMEM.can_read(uarch,*this,logger) ) {
            uarch.exec_and_read(*this,logger, uarch.mMEM ) ;
            return change_state_S_LS1__S_LS1p(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_LS1_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_LS1(uarch,logger,position);
    }
    
    // State action S_LS1p
    bool action_S_LS1p(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( exec_done (  ) && uarch.mWB.no_capacity_check(uarch,*this,logger) ) {
            return change_state_S_LS1p__S_WB(uarch,logger,position) ;
      } else if ( uarch.mMEM.can_request_read(uarch,*this,logger) ) {
            uarch.mMEM.send_read_request(uarch,*this,logger) ;
            return change_state_S_LS1p__S_LS1(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_LS1p_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_LS1p(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      {
            uarch.FwdCB.write(uarch,*this,logger, Trg ) ;
            write_ops(uarch,*this,logger) ;
            return change_state_S_WB__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WB(uarch,logger,position);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_LS0
    void flush_S_LS0(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_LS0_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_LS0(uarch,logger);
    }
    
    // Flush action S_LS1
    void flush_S_LS1(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_LS1_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_LS1(uarch,logger);
    }
    
    // Flush action S_LS1p
    void flush_S_LS1p(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_LS1p_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_LS1p(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
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
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CIA.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CIA.name(),-1,ic._instr->slot(0).uint64());
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CR.name(),-1,ic._instr->slot(1).uint64());
      ic._instr->read_operand(uarch.get_iss(),2);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CTR.name(),-1,ic._instr->slot(2).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CIA.name(),-1,ic._instr->slot(0).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CR.name(),-1,ic._instr->slot(1).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CTR.name(),-1,ic._instr->slot(2).uint64());
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
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
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: ic._instr->read_operand(uarch.get_iss(),0);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CIA.name(),-1,ic._instr->slot(0).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CR.name(),-1,ic._instr->slot(1).uint64());
        break;
        case 2: ic._instr->read_operand(uarch.get_iss(),2);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CTR.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CIA.name(),-1,ic._instr->slot(0).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CR.name(),-1,ic._instr->slot(1).uint64());
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CTR.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_P&>(uarch).CIA.id(static_cast<OSM_P&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).CIA.name(),-1,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).CR.name(),-1,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,2)) {
        tag_t tag = get_wm_tag(ic,2);
        RegId id = static_cast<OSM_P&>(uarch).CTR.id(static_cast<OSM_P&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,2);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),2);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).CTR.name(),-1,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).CIA.last_allocator( static_cast<OSM_P&>(uarch),0)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).CR.last_allocator( static_cast<OSM_P&>(uarch),0)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
      if (!is_op_read(ic,2)) {
        if (!can_read(uarch,ic,0,2)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).CTR.last_allocator( static_cast<OSM_P&>(uarch),0)),ic,2);
        } else {
          set_wm_tag(0,ic,2);
        }
      }
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(0);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CIA.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).CR.name(static_cast<OSM_P&>(uarch),0);
        case 2: return static_cast<OSM_P&>(uarch).CTR.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CIA.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        case 2: return static_cast<OSM_P&>(uarch).CTR.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
        case 2: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
        case 2: return ic._instr->write_mask(2).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned sub_op = 0) const {
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
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 3; };
    
  };
  
  // Operands:
  //   (trg: CTR?)
  //   (trg: NIA?)
  struct bc_serializing_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CTR.allocate(uarch,ic,logger);
      uarch.NIA.allocate(uarch,ic,logger);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CTR.deallocate(uarch,ic,logger);
      uarch.NIA.deallocate(uarch,ic,logger);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),2);
      if (logger && ic._instr->is_dirty(2))logger->logOpAction("write",static_cast<OSM_P&>(uarch).CTR.name(),-1,ic._instr->slot(2).uint64());
      ic._instr->commit(uarch.get_iss(),3);
      if (logger && ic._instr->is_dirty(3))logger->logOpAction("write",static_cast<OSM_P&>(uarch).NIA.name(),-1,ic._instr->slot(3).uint64());
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(2)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(2);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CTR.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).NIA.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CTR.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).NIA.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(2).uint32();
        case 1: return ic._instr->write_mask(3).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned sub_op = 0) const {
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
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  // Operands:
  //   (src: CIA)
  struct b_serializing_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CIA.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CIA.name(),-1,ic._instr->slot(0).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CIA.name(),-1,ic._instr->slot(0).uint64());
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic);
        RegId id = static_cast<OSM_P&>(uarch).CIA.id(static_cast<OSM_P&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name());
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).CIA.name(),-1,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).CIA.last_allocator( static_cast<OSM_P&>(uarch),0)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(0);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).CIA.name(static_cast<OSM_P&>(uarch),0); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).CIA.id(static_cast<OSM_P&>(uarch),0); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(0).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(0) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 0; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (trg: NIA)
  struct b_serializing_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.NIA.allocate(uarch,ic,logger);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.NIA.deallocate(uarch,ic,logger);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),1);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).NIA.name(),-1,ic._instr->slot(1).uint64());
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(1);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).NIA.name(static_cast<OSM_P&>(uarch),0); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).NIA.id(static_cast<OSM_P&>(uarch),0); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(1).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(1) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 1; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
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
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.LR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.LR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.LR.allocate(uarch,ic,logger);
      uarch.NIA.allocate(uarch,ic,logger);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.LR.deallocate(uarch,ic,logger);
      uarch.NIA.deallocate(uarch,ic,logger);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),1);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).LR.name(),-1,ic._instr->slot(1).uint64());
      ic._instr->commit(uarch.get_iss(),2);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).NIA.name(),-1,ic._instr->slot(2).uint64());
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(1);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).LR.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).NIA.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).LR.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).NIA.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(1).uint32();
        case 1: return ic._instr->write_mask(2).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned sub_op = 0) const {
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
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  // Operands:
  //   (src: CR/p)
  //   (src: CTR?)
  //   (src: LR?)
  struct bclr_serializing_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.LR.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CR.name(),-1,ic._instr->slot(0).uint64());
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CTR.name(),-1,ic._instr->slot(1).uint64());
      ic._instr->read_operand(uarch.get_iss(),2);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).LR.name(),-1,ic._instr->slot(2).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CR.name(),-1,ic._instr->slot(0).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CTR.name(),-1,ic._instr->slot(1).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).LR.name(),-1,ic._instr->slot(2).uint64());
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
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
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: ic._instr->read_operand(uarch.get_iss(),0);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CR.name(),-1,ic._instr->slot(0).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CTR.name(),-1,ic._instr->slot(1).uint64());
        break;
        case 2: ic._instr->read_operand(uarch.get_iss(),2);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).LR.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CR.name(),-1,ic._instr->slot(0).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CTR.name(),-1,ic._instr->slot(1).uint64());
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).LR.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).CR.name(),-1,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_P&>(uarch).CTR.id(static_cast<OSM_P&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).CTR.name(),-1,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,2)) {
        tag_t tag = get_wm_tag(ic,2);
        RegId id = static_cast<OSM_P&>(uarch).LR.id(static_cast<OSM_P&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,2);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),2);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).LR.name(),-1,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).CR.last_allocator( static_cast<OSM_P&>(uarch),0)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).CTR.last_allocator( static_cast<OSM_P&>(uarch),0)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
      if (!is_op_read(ic,2)) {
        if (!can_read(uarch,ic,0,2)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).LR.last_allocator( static_cast<OSM_P&>(uarch),0)),ic,2);
        } else {
          set_wm_tag(0,ic,2);
        }
      }
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(0);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).CTR.name(static_cast<OSM_P&>(uarch),0);
        case 2: return static_cast<OSM_P&>(uarch).LR.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).CTR.id(static_cast<OSM_P&>(uarch),0);
        case 2: return static_cast<OSM_P&>(uarch).LR.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
        case 2: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
        case 2: return ic._instr->write_mask(2).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned sub_op = 0) const {
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
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 3; };
    
  };
  
  // Operands:
  //   (trg: CTR?)
  //   (trg: NIA?)
  struct bclr_serializing_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CTR.allocate(uarch,ic,logger);
      uarch.NIA.allocate(uarch,ic,logger);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CTR.deallocate(uarch,ic,logger);
      uarch.NIA.deallocate(uarch,ic,logger);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),1);
      if (logger && ic._instr->is_dirty(1))logger->logOpAction("write",static_cast<OSM_P&>(uarch).CTR.name(),-1,ic._instr->slot(1).uint64());
      ic._instr->commit(uarch.get_iss(),3);
      if (logger && ic._instr->is_dirty(3))logger->logOpAction("write",static_cast<OSM_P&>(uarch).NIA.name(),-1,ic._instr->slot(3).uint64());
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(1);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CTR.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).NIA.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CTR.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).NIA.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(1).uint32();
        case 1: return ic._instr->write_mask(3).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned sub_op = 0) const {
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
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  //
  // Instruction class serializing
  //
  template <class Src_t, class Trg_t>
  struct Inst_serializing : public Inst_P {
    
    typedef struct Inst_serializing<Src_t, Trg_t> ThisClass;
    
    Inst_serializing &thisInstr() { return *this; };
    
    Src_t Src;
    Trg_t Trg;
    void reset(unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_P::reset(iid,cid,ic);
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      _stage = (PipelineStageBase*)1;
    }
    
    void deallocate_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      Src.deallocate(uarch,ic,logger);
      Trg.deallocate(uarch,ic,logger);
    }
    
    void write_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      Src.write(uarch,ic,logger);
      Trg.write(uarch,ic,logger);
    }
    
    bool exec_done() const { return true; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool change_state_S_BR__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_BR -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mBR.remove_instr();
      _stage = &uarch.mWB;
      return true;
    }
    
    bool change_state_S_ID__S_BR(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_BR"); }
      _action = (StateFunc)action_S_BR_p;
      _flush = (FlushFunc)flush_S_BR_p;
      uarch.mID.remove_instr();
      _stage = &uarch.mBR;
      uarch.mBR.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.issue(_instr);
      _stage = &uarch.mID;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool change_state_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    // State action S_BR
    bool action_S_BR(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( Src.read_avail(uarch,*this,logger, uarch.FwdCB ) && uarch.mWB.no_capacity_check(uarch,*this,logger) ) {
            return change_state_S_BR__S_WB(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_BR_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_BR(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( Trg.can_write(uarch,*this,logger) && uarch.mWB.has_space(uarch,*this,logger) && uarch.mBR.has_space(uarch,*this,logger) ) {
            Src.record_sources(uarch,*this,logger) ;
            Trg.allocate(uarch,*this,logger) ;
            uarch.mWB.add_instr(uarch,*this,logger) ;
            return change_state_S_ID__S_BR(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.has_space(uarch,*this,logger) ) {
            return change_state_S_INIT__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      {
            uarch.exec_mWB(*this,logger,position) ;
            uarch.FwdCB.write(uarch,*this,logger, Trg ) ;
            write_ops(uarch,*this,logger) ;
            if ( branch_taken(uarch) ) {
                  uarch.taken_flush_mWB(*this,logger,position  ) ;
              }
            return change_state_S_WB__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WB(uarch,logger,position);
    }
    
    // Flush action S_BR
    void flush_S_BR(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_BR_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_BR(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
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
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      ic._instr->read_operand(uarch.get_iss(),3);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
        case 1: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),3);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if ((t0.can_read_tag_partial(uarch,ic,0,id,tag) & mask(ic,1)) == mask(ic,1)) {
          t0.read_partial(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).XER.name(),-1,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).XER.last_allocator( static_cast<OSM_P&>(uarch),0)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(1);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_P&>(uarch).XER.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return ic._masks[0];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(1).uint32();
        case 1: return ic._instr->write_mask(3).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned sub_op = 0) const {
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
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
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
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[2])) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[3])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),3);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
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
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
        case 2: break;
        case 3: ic._instr->read_operand(uarch.get_iss(),3);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
        case 2: break;
        case 3: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if ((t0.can_read_tag_partial(uarch,ic,0,id,tag) & mask(ic,0)) == mask(ic,0)) {
          t0.read_partial(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).CR.name(),-1,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,2)) {
        tag_t tag = get_wm_tag(ic,2);
        RegId id = static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,2);
        }
        else if ((t0.can_read_tag_partial(uarch,ic,0,id,tag) & mask(ic,2)) == mask(ic,2)) {
          t0.read_partial(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),2);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).XER.name(),-1,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,3)) {
        tag_t tag = get_wm_tag(ic,3);
        RegId id = static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,3);
        }
        else if ((t0.can_read_tag_partial(uarch,ic,0,id,tag) & mask(ic,3)) == mask(ic,3)) {
          t0.read_partial(uarch,ic,logger,*this,3);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),3);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).XER.name(),-1,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).CR.last_allocator( static_cast<OSM_P&>(uarch),0)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(2)._index)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
      if (!is_op_read(ic,2)) {
        if (!can_read(uarch,ic,0,2)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).XER.last_allocator( static_cast<OSM_P&>(uarch),0)),ic,2);
        } else {
          set_wm_tag(0,ic,2);
        }
      }
      if (!is_op_read(ic,3)) {
        if (!can_read(uarch,ic,0,3)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).XER.last_allocator( static_cast<OSM_P&>(uarch),0)),ic,3);
        } else {
          set_wm_tag(0,ic,3);
        }
      }
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CR.allocate(uarch,ic,logger, ic._masks[1]);
      uarch.GPR.allocate(uarch,ic,logger,ic.op(2)._index);
      uarch.XER.allocate(uarch,ic,logger, ic._masks[2]);
      uarch.XER.allocate(uarch,ic,logger, ic._masks[3]);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CR.deallocate(uarch,ic,logger, ic._masks[1]);
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(2)._index);
      uarch.XER.deallocate(uarch,ic,logger, ic._masks[2]);
      uarch.XER.deallocate(uarch,ic,logger, ic._masks[3]);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),0);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).CR.name(),-1,ic._instr->slot(0).uint64());
      ic._instr->commit(uarch.get_iss(),2);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
      ic._instr->commit(uarch.get_iss(),3);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
      ic._instr->commit(uarch.get_iss(),3);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(0);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        case 2: return static_cast<OSM_P&>(uarch).XER.name(static_cast<OSM_P&>(uarch),0);
        case 3: return static_cast<OSM_P&>(uarch).XER.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        case 2: return static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0);
        case 3: return static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._masks[1];
        case 1: return 0x0;
        case 2: return ic._masks[2];
        case 3: return ic._masks[3];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(2).uint32();
        case 2: return ic._instr->write_mask(3).uint32();
        case 3: return ic._instr->write_mask(3).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned sub_op = 0) const {
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
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 4; };
    
  };
  
  // Operands:
  //   (src: GPR(RA)?)
  struct addi_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic);
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        if (tag == 0) {
          read(uarch,ic,logger);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name());
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(0);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(0)._index); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(0).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(0) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 0; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (trg: GPR(RT))
  struct addi_sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(1)._index);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(1)._index);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),1);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(1);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(1)._index); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(1).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(1) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 1; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
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
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic);
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        if (tag == 0) {
          read(uarch,ic,logger);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name());
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(1);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(1)._index); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(1).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(1) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 1; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (trg: GPR(RA))
  struct ori_sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(0)._index);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(0)._index);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),0);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(0);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(0)._index); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(0).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(0) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 0; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (src: GPR(RS))
  //   (src: XER[mask 0x80000000]/p)
  struct andi__sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),2);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
      ic._instr->read_operand(uarch.get_iss(),3);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
          return true;
        case 1: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: ic._instr->read_operand(uarch.get_iss(),2);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),3);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if ((t0.can_read_tag_partial(uarch,ic,0,id,tag) & mask(ic,1)) == mask(ic,1)) {
          t0.read_partial(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).XER.name(),-1,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(2)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).XER.last_allocator( static_cast<OSM_P&>(uarch),0)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(2)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(2);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        case 1: return static_cast<OSM_P&>(uarch).XER.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        case 1: return static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return ic._masks[0];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(2).uint32();
        case 1: return ic._instr->write_mask(3).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned sub_op = 0) const {
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
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  // Operands:
  //   (trg: CR[mask 0xf0000000]/p, mask: 0xffffffff)
  //   (trg: GPR(RA))
  //   (src: XER[mask 0x80000000]/p, mask: 0xffffffff)
  struct andi__sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[2])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),3);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
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
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
        case 2: ic._instr->read_operand(uarch.get_iss(),3);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if ((t0.can_read_tag_partial(uarch,ic,0,id,tag) & mask(ic,0)) == mask(ic,0)) {
          t0.read_partial(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).CR.name(),-1,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,2)) {
        tag_t tag = get_wm_tag(ic,2);
        RegId id = static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,2);
        }
        else if ((t0.can_read_tag_partial(uarch,ic,0,id,tag) & mask(ic,2)) == mask(ic,2)) {
          t0.read_partial(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),2);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).XER.name(),-1,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).CR.last_allocator( static_cast<OSM_P&>(uarch),0)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
      if (!is_op_read(ic,2)) {
        if (!can_read(uarch,ic,0,2)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).XER.last_allocator( static_cast<OSM_P&>(uarch),0)),ic,2);
        } else {
          set_wm_tag(0,ic,2);
        }
      }
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CR.allocate(uarch,ic,logger, ic._masks[1]);
      uarch.GPR.allocate(uarch,ic,logger,ic.op(1)._index);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CR.deallocate(uarch,ic,logger, ic._masks[1]);
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(1)._index);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),0);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).CR.name(),-1,ic._instr->slot(0).uint64());
      ic._instr->commit(uarch.get_iss(),1);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(0);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 2: return static_cast<OSM_P&>(uarch).XER.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 2: return static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._masks[1];
        case 1: return 0x0;
        case 2: return ic._masks[2];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
        case 2: return ic._instr->write_mask(3).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned sub_op = 0) const {
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
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 3; };
    
  };
  
  // Operand Alias:  ori_sfx_Src -> rlwinm_sfx_Src
  //   (src: GPR(RS))
  typedef ori_sfx_Src rlwinm_sfx_Src;
  
  // Operand Alias:  ori_sfx_Trg -> rlwinm_sfx_Trg
  //   (trg: GPR(RA))
  typedef ori_sfx_Trg rlwinm_sfx_Trg;
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  struct add_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: ic._instr->read_operand(uarch.get_iss(),0);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(0);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(1)._index);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned sub_op = 0) const {
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
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  // Operands:
  //   (trg: GPR(RT))
  struct add_sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(2)._index);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(2)._index);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),2);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(2)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(2);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(2)._index); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(2).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(2) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 2; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  struct or_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      ic._instr->read_operand(uarch.get_iss(),2);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),2);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(2)._index)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(1);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(2)._index);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(1).uint32();
        case 1: return ic._instr->write_mask(2).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned sub_op = 0) const {
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
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  // Operand Alias:  ori_sfx_Trg -> or_sfx_Trg
  //   (trg: GPR(RA))
  typedef ori_sfx_Trg or_sfx_Trg;
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: XER[mask 0x80000000]/p)
  struct cmpi_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      ic._instr->read_operand(uarch.get_iss(),2);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(2).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(2).uint64());
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
        case 1: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),2);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if ((t0.can_read_tag_partial(uarch,ic,0,id,tag) & mask(ic,1)) == mask(ic,1)) {
          t0.read_partial(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).XER.name(),-1,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).XER.last_allocator( static_cast<OSM_P&>(uarch),0)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(1);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_P&>(uarch).XER.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return ic._masks[0];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(1).uint32();
        case 1: return ic._instr->write_mask(2).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned sub_op = 0) const {
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
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  // Operands:
  //   (trg: CR/p, mask: 0xffffffff)
  //   (src: XER[mask 0x80000000]/p, mask: 0xffffffff)
  struct cmpi_sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[2])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),2);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(2).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(2).uint64());
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.CR.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint32())) return false;
          return true;
        case 1: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[2] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: ic._instr->read_operand(uarch.get_iss(),2);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if ((t0.can_read_tag_partial(uarch,ic,0,id,tag) & mask(ic,0)) == mask(ic,0)) {
          t0.read_partial(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).CR.name(),-1,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if ((t0.can_read_tag_partial(uarch,ic,0,id,tag) & mask(ic,1)) == mask(ic,1)) {
          t0.read_partial(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).XER.name(),-1,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).CR.last_allocator( static_cast<OSM_P&>(uarch),0)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).XER.last_allocator( static_cast<OSM_P&>(uarch),0)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CR.allocate(uarch,ic,logger, ic._masks[1]);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CR.deallocate(uarch,ic,logger, ic._masks[1]);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),0);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).CR.name(),-1,ic._instr->slot(0).uint64());
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(0);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).XER.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._masks[1];
        case 1: return ic._masks[2];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(2).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned sub_op = 0) const {
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
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: XER[mask 0x80000000]/p)
  struct cmp_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      ic._instr->read_operand(uarch.get_iss(),2);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
      ic._instr->read_operand(uarch.get_iss(),3);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
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
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),2);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
        break;
        case 2: ic._instr->read_operand(uarch.get_iss(),3);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,2)) {
        tag_t tag = get_wm_tag(ic,2);
        RegId id = static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,2);
        }
        else if ((t0.can_read_tag_partial(uarch,ic,0,id,tag) & mask(ic,2)) == mask(ic,2)) {
          t0.read_partial(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),2);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).XER.name(),-1,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(2)._index)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
      if (!is_op_read(ic,2)) {
        if (!can_read(uarch,ic,0,2)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).XER.last_allocator( static_cast<OSM_P&>(uarch),0)),ic,2);
        } else {
          set_wm_tag(0,ic,2);
        }
      }
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(1);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        case 2: return static_cast<OSM_P&>(uarch).XER.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        case 2: return static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
        case 2: return ic._masks[0];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(1).uint32();
        case 1: return ic._instr->write_mask(2).uint32();
        case 2: return ic._instr->write_mask(3).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned sub_op = 0) const {
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
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 3; };
    
  };
  
  // Operands:
  //   (trg: CR/p, mask: 0xffffffff)
  //   (src: XER[mask 0x80000000]/p, mask: 0xffffffff)
  struct cmp_sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[2])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),3);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.CR.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint32())) return false;
          return true;
        case 1: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[2] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: ic._instr->read_operand(uarch.get_iss(),3);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if ((t0.can_read_tag_partial(uarch,ic,0,id,tag) & mask(ic,0)) == mask(ic,0)) {
          t0.read_partial(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).CR.name(),-1,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if ((t0.can_read_tag_partial(uarch,ic,0,id,tag) & mask(ic,1)) == mask(ic,1)) {
          t0.read_partial(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).XER.name(),-1,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).CR.last_allocator( static_cast<OSM_P&>(uarch),0)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).XER.last_allocator( static_cast<OSM_P&>(uarch),0)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CR.allocate(uarch,ic,logger, ic._masks[1]);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CR.deallocate(uarch,ic,logger, ic._masks[1]);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),0);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).CR.name(),-1,ic._instr->slot(0).uint64());
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(0);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).XER.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._masks[1];
        case 1: return ic._masks[2];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(3).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned sub_op = 0) const {
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
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  // Operands:
  //   (src: SPR(SPRN))
  struct mfspr_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.SPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).SPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).SPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic);
        RegId id = static_cast<OSM_P&>(uarch).SPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        if (tag == 0) {
          read(uarch,ic,logger);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name());
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).SPR.name(),ic.op(1)._index,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).SPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(1);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).SPR.name(static_cast<OSM_P&>(uarch),ic.op(1)._index); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).SPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(1).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(1) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 1; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (trg: GPR(RT))
  struct mfspr_sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(0)._index);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(0)._index);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),0);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(0);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(0)._index); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(0).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(0) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 0; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (src: GPR(RB)[mask 0x0000001f]/p)
  //   (src: GPR(RS))
  struct srw_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read", ic._masks[0])) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      ic._instr->read_operand(uarch.get_iss(),2);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read", (ic._masks[0] & ~mask).uint32())) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),2);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if ((t0.can_read_tag_partial(uarch,ic,0,id,tag) & mask(ic,0)) == mask(ic,0)) {
          t0.read_partial(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(2)._index)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(1);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(2)._index);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._masks[0];
        case 1: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(1).uint32();
        case 1: return ic._instr->write_mask(2).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned sub_op = 0) const {
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
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 2; };
    
  };
  
  // Operand Alias:  ori_sfx_Trg -> srw_sfx_Trg
  //   (trg: GPR(RA))
  typedef ori_sfx_Trg srw_sfx_Trg;
  
  // Operands:
  //   (src: GPR(RS))
  struct mtspr_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic);
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        if (tag == 0) {
          read(uarch,ic,logger);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name());
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(0);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(0)._index); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(0).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(0) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 0; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (trg: SPR(SPRN))
  struct mtspr_sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.SPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.SPR.allocate(uarch,ic,logger,ic.op(1)._index);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.SPR.deallocate(uarch,ic,logger,ic.op(1)._index);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),1);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).SPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(1);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).SPR.name(static_cast<OSM_P&>(uarch),ic.op(1)._index); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).SPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(1).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(1) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 1; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  //
  // Instruction class sfx
  //
  template <class Src_t, class Trg_t>
  struct Inst_sfx : public Inst_P {
    
    typedef struct Inst_sfx<Src_t, Trg_t> ThisClass;
    
    Inst_sfx &thisInstr() { return *this; };
    
    Src_t Src;
    Trg_t Trg;
    void reset(unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_P::reset(iid,cid,ic);
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      _stage = (PipelineStageBase*)1;
    }
    
    void deallocate_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      Src.deallocate(uarch,ic,logger);
      Trg.deallocate(uarch,ic,logger);
    }
    
    void write_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      Src.write(uarch,ic,logger);
      Trg.write(uarch,ic,logger);
    }
    
    bool exec_done() const { return true; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool change_state_S_EX0__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX0 -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mEX0.remove_instr_indep(position);
      _stage = &uarch.mWB;
      return true;
    }
    
    bool change_state_S_ID__S_EX0(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX0"); }
      _action = (StateFunc)action_S_EX0_p;
      _flush = (FlushFunc)flush_S_EX0_p;
      uarch.mID.remove_instr();
      _stage = &uarch.mEX0;
      uarch.mEX0.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.issue(_instr);
      _stage = &uarch.mID;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool change_state_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    // State action S_EX0
    bool action_S_EX0(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( Src.read_avail(uarch,*this,logger, uarch.FwdCB ) && uarch.mWB.no_capacity_check(uarch,*this,logger) ) {
            uarch.exec(*this,logger) ;
            return change_state_S_EX0__S_WB(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EX0_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX0(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( Trg.can_write(uarch,*this,logger) && uarch.mWB.has_space(uarch,*this,logger) && uarch.mEX0.has_space(uarch,*this,logger) ) {
            Src.record_sources(uarch,*this,logger) ;
            Trg.allocate(uarch,*this,logger) ;
            uarch.mWB.add_instr(uarch,*this,logger) ;
            return change_state_S_ID__S_EX0(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.has_space(uarch,*this,logger) ) {
            return change_state_S_INIT__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      {
            uarch.FwdCB.write(uarch,*this,logger, Trg ) ;
            write_ops(uarch,*this,logger) ;
            return change_state_S_WB__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WB(uarch,logger,position);
    }
    
    // Flush action S_EX0
    void flush_S_EX0(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_EX0_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX0(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class store
  //
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: GPR(RS))
  struct stb_store_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: ic._instr->read_operand(uarch.get_iss(),0);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(0);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(1)._index);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned sub_op = 0) const {
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
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
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
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      ic._instr->read_operand(uarch.get_iss(),2);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
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
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: ic._instr->read_operand(uarch.get_iss(),0);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
        case 2: ic._instr->read_operand(uarch.get_iss(),2);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
        break;
      }
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      if (!is_op_read(ic,0)) {
        tag_t tag = get_wm_tag(ic,0);
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        if (tag == 0) {
          read(uarch,ic,logger,0);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,1)) {
        tag_t tag = get_wm_tag(ic,1);
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        if (tag == 0) {
          read(uarch,ic,logger,1);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,tag);
          done = false;
        }
      }
      if (!is_op_read(ic,2)) {
        tag_t tag = get_wm_tag(ic,2);
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        if (tag == 0) {
          read(uarch,ic,logger,2);
        }
        else if (t0.can_read_tag(uarch,ic,0,id,tag)) {
          t0.read(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),2);
        } 
        else {
          if (logger) logger->logReadAvailFail(static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,tag);
          done = false;
        }
      }
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
      if (!is_op_read(ic,0)) {
        if (!can_read(uarch,ic,0,0)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index)),ic,0);
        } else {
          set_wm_tag(0,ic,0);
        }
      }
      if (!is_op_read(ic,1)) {
        if (!can_read(uarch,ic,0,1)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index)),ic,1);
        } else {
          set_wm_tag(0,ic,1);
        }
      }
      if (!is_op_read(ic,2)) {
        if (!can_read(uarch,ic,0,2)) {
          set_wm_tag((static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(2)._index)),ic,2);
        } else {
          set_wm_tag(0,ic,2);
        }
      }
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(0);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 2: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(2)._index);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 2: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
        case 2: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
        case 2: return ic._instr->write_mask(2).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned sub_op = 0) const {
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
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 3; };
    
  };
  
  // Operand Alias:  stb_store_Src -> stw_store_Src
  //   (src: GPR(RA))
  //   (src: GPR(RS))
  typedef stb_store_Src stw_store_Src;
  
  // Operand Alias:  sthx_store_Src -> stwx_store_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  typedef sthx_store_Src stwx_store_Src;
  
  // Operand Alias:  stb_store_Src -> stwu_store_Src
  //   (src: GPR(RA))
  //   (src: GPR(RS))
  typedef stb_store_Src stwu_store_Src;
  
  // Operands:
  //   (trg: GPR(RA))
  struct stwu_store_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0>
    bool read_avail(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0) {
      bool done = true;
      return done;
    }
    
    void record_sources(OSM_P &uarch,Inst_P &ic,Logger *)
    {
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(0)._index);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(0)._index);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),0);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(0);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(0)._index); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(0).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(0) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 0; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  //
  // Instruction class store
  //
  template <class Src_t, class Trg_t>
  struct Inst_store : public Inst_P {
    
    typedef struct Inst_store<Src_t, Trg_t> ThisClass;
    
    Inst_store &thisInstr() { return *this; };
    
    Src_t Src;
    Trg_t Trg;
    void reset(unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_P::reset(iid,cid,ic);
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      _stage = (PipelineStageBase*)1;
    }
    
    void deallocate_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      Src.deallocate(uarch,ic,logger);
      Trg.deallocate(uarch,ic,logger);
    }
    
    void write_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      Src.write(uarch,ic,logger);
      Trg.write(uarch,ic,logger);
    }
    
    bool exec_done() const { return _status == adl::MemAccess::opNone; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool change_state_S_ID__S_LS0(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_LS0"); }
      _action = (StateFunc)action_S_LS0_p;
      _flush = (FlushFunc)flush_S_LS0_p;
      uarch.mID.remove_instr();
      _stage = &uarch.mLS0;
      uarch.mLS0.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.issue(_instr);
      _stage = &uarch.mID;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool change_state_S_LS0__S_LS1(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_LS0 -> S_LS1"); }
      _action = (StateFunc)action_S_LS1_p;
      _flush = (FlushFunc)flush_S_LS1_p;
      uarch.mLS0.remove_instr();
      _stage = &uarch.mLS1;
      uarch.mLS1.add_instr(this);
      return true;
    }
    
    bool change_state_S_LS1__S_LS1p(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_LS1 -> S_LS1p"); }
      _action = (StateFunc)action_S_LS1p_p;
      _flush = (FlushFunc)flush_S_LS1p_p;
      return action_S_LS1p(uarch,logger,position);
    }
    
    bool change_state_S_LS1p__S_LS1(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_LS1p -> S_LS1"); }
      _action = (StateFunc)action_S_LS1_p;
      _flush = (FlushFunc)flush_S_LS1_p;
      return action_S_LS1(uarch,logger,position);
    }
    
    bool change_state_S_LS1p__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_LS1p -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mLS1.remove_instr();
      _stage = &uarch.mWB;
      return true;
    }
    
    bool change_state_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      uarch.commit(this);
      return true;
    }
    
    // State action S_ID
    bool action_S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( Trg.can_write(uarch,*this,logger) && uarch.mWB.has_space(uarch,*this,logger) && uarch.mLS0.has_space(uarch,*this,logger) ) {
            Src.record_sources(uarch,*this,logger) ;
            Trg.allocate(uarch,*this,logger) ;
            uarch.mWB.add_instr(uarch,*this,logger) ;
            return change_state_S_ID__S_LS0(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.has_space(uarch,*this,logger) ) {
            return change_state_S_INIT__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_LS0
    bool action_S_LS0(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( Src.read_avail(uarch,*this,logger, uarch.FwdCB ) && uarch.mLS1.has_space(uarch,*this,logger) && uarch.mMEM.can_request_write(uarch,*this,logger) ) {
            uarch.exec_mem(*this,logger) ;
            uarch.mMEM.send_write_request(uarch,*this,logger) ;
            return change_state_S_LS0__S_LS1(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_LS0_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_LS0(uarch,logger,position);
    }
    
    // State action S_LS1
    bool action_S_LS1(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMEM.can_read(uarch,*this,logger) ) {
            uarch.exec_and_write(*this,logger, uarch.mMEM ) ;
            return change_state_S_LS1__S_LS1p(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_LS1_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_LS1(uarch,logger,position);
    }
    
    // State action S_LS1p
    bool action_S_LS1p(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( exec_done (  ) && uarch.mWB.no_capacity_check(uarch,*this,logger) ) {
            return change_state_S_LS1p__S_WB(uarch,logger,position) ;
      } else if ( uarch.mMEM.can_request_write(uarch,*this,logger) ) {
            uarch.mMEM.send_write_request(uarch,*this,logger) ;
            return change_state_S_LS1p__S_LS1(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_LS1p_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_LS1p(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      {
            uarch.FwdCB.write(uarch,*this,logger, Trg ) ;
            write_ops(uarch,*this,logger) ;
            return change_state_S_WB__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WB(uarch,logger,position);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_LS0
    void flush_S_LS0(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_LS0_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_LS0(uarch,logger);
    }
    
    // Flush action S_LS1
    void flush_S_LS1(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_LS1_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_LS1(uarch,logger);
    }
    
    // Flush action S_LS1p
    void flush_S_LS1p(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_LS1p_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_LS1p(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
  };
  
  byte_t *mMEM_mem_t::hasData(addr_t addr, unsigned &size) const {
    return GlobalMemory.getPage(addr) + GlobalMemoryType::pageOffset(addr);
  }
  
  addr_t Inst_P::addr() const
  {
    return _instr->_addr;
  }
  
  unsigned Inst_P::size() const
  {
    return _instr ? _instr->_size : 0;
  }
  
  unsigned Inst_P::instr_id() const
  {
    return _instr->_id;
  }
  
  bool Inst_P::has_exceptions() const
  {
    return _instr && _instr->has_exceptions();
  };
  
  std::pair<adl::BranchType,bool> Inst_P::branch_info(OSM_P&uarch) const
  {
    return _instr->branch_info(*uarch._iss);
  }
  
  bool Inst_P::is_branch(OSM_P&uarch) const
  {
    const pair<adl::BranchType, bool> x = _instr->branch_info(*uarch._iss);
    
    return x.first;
  }
  
  bool Inst_P::branch_taken(OSM_P&uarch) const
  {
    const pair<adl::BranchType, bool> x = _instr->branch_info(*uarch._iss);
    
    return (x.first && x.second);
  }
  
  addr_t Inst_P::branch_next_addr(OSM_P&uarch) const
  {
    if (branch_taken(uarch)) {
      return branch_target();
    } else {
      return addr()+size();
    }
  }
  
  addr_t Inst_P::branch_target() const
  {
    return _instr->branch_target();
  }
  
  inline const adl::OpInfo &Inst_P::op(unsigned index) const { return _instr->op(index); };
  
  inline const uint32_t *Inst_P::opc() const { return _instr->opc(); };
  
  unsigned OSM_P::getInstrClassAttrByName(const std::string &attr)
  {
    return 0;
  }
  
  void OSM_P::alloc_mem_access_data()
  {
    for (MemAccessPool::iterator iter = _mem_access_pool.begin(); iter != _mem_access_pool.end(); ++iter) {
      (*iter)->alloc(16);
    }
  }
  
  UadlMemAccess &OSM_P::add_memory_request(InstrBase &instr)
  {
    return *instr.mem();
  }
  
  void OSM_P::set_memory_result(InstrBase &instr)
  {
    UadlMemAccess *ma = instr.mem();
    _iss->set_memory_result(0,ma->data(),ma->offset());
  }
  
  void OSM_P::free_mem(InstrBase &instr)
  {
    if (instr.mem()) {
      UadlMemAccess *ma = instr.get_mem_and_clear();
      _mem_access_pool.free(ma);
    }
  }
  
  void OSM_P::free_all_mem(InstrBase &instr)
  {
    if (instr.mem()) {
      _mem_access_pool.free_all(instr.mem(),instr.mtail());
    }
  }
  
  void OSM_P::handle_memory_request(unsigned memId,const adl::MemAccess &m)
  {
    if (!_log_mem || _ignore_mem) return;
    UadlMemAccess *ma = _mem_access_pool.alloc();
    ma->reset(_memId++,m);
    _cur_instr->add_mem(ma);
  }
  
  void OSM_P::getMemoryData(byte_t *data,UadlMemAccess *ma,addr_t addr,unsigned size)
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
  
  void OSM_P::getMemoryData(CacheAccess type, addr_t addr, unsigned size, byte_t *data)
  {
    if (type == adl::CacheLogRead) {
      // get data from memory/cache
      if (mMEM_obj.getMemoryData(type, addr, size, data)) {
        // augment with instruction memory data
        
        if (Inst_P *instr = mWB[0]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_P *instr = mWB[1]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_P *instr = mWB[2]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_P *instr = mWB[3]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_P *instr = mBR[0]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_P *instr = mEX0[0]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_P *instr = mEX0[1]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_P *instr = mLS1[0]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_P *instr = mLS0[0]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_P *instr = mID[0]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_P *instr = mID[1]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_P *instr = mID[2]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_P *instr = mID[3]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
      } else {
        mMEM_obj.getMemoryData(type, addr, size, data);
      }
    } else {
      mMEM_obj.getMemoryData(type, addr, size, data);
    }
  }
  
  static unsigned initFetchSizes[] = { 4,  };
  FetchSizes OSM_P::_fetchSizes(&initFetchSizes[0],&initFetchSizes[1]);
  
  // Factory function for instruction "add".
  Inst_P *OSM_P::get_instr_add(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<add_sfx_Src, add_sfx_Trg> *ic = (Inst_sfx<add_sfx_Src, add_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "addi".
  Inst_P *OSM_P::get_instr_addi(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addi_sfx_Src, addi_sfx_Trg> *ic = (Inst_sfx<addi_sfx_Src, addi_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "addic".
  Inst_P *OSM_P::get_instr_addic(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_addic' is not implemented in this model.");
  }
  
  // Factory function for instruction "addic.".
  Inst_P *OSM_P::get_instr_addic_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addic__sfx_Src, addic__sfx_Trg> *ic = (Inst_sfx<addic__sfx_Src, addic__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    ic->_masks[1] = 0xffffffff;
    ic->_masks[2] = 0x20000000;
    ic->_masks[3] = 0xffffffff;
    return ic;
  }
  
  // Factory function for instruction "addis".
  Inst_P *OSM_P::get_instr_addis(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addis_sfx_Src, addis_sfx_Trg> *ic = (Inst_sfx<addis_sfx_Src, addis_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "addme".
  Inst_P *OSM_P::get_instr_addme(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_addme' is not implemented in this model.");
  }
  
  // Factory function for instruction "addme.".
  Inst_P *OSM_P::get_instr_addme_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_addme_' is not implemented in this model.");
  }
  
  // Factory function for instruction "addmeo".
  Inst_P *OSM_P::get_instr_addmeo(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_addmeo' is not implemented in this model.");
  }
  
  // Factory function for instruction "addo".
  Inst_P *OSM_P::get_instr_addo(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_addo' is not implemented in this model.");
  }
  
  // Factory function for instruction "andi.".
  Inst_P *OSM_P::get_instr_andi_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<andi__sfx_Src, andi__sfx_Trg> *ic = (Inst_sfx<andi__sfx_Src, andi__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    ic->_masks[1] = 0xffffffff;
    ic->_masks[2] = 0xffffffff;
    return ic;
  }
  
  // Factory function for instruction "b".
  Inst_P *OSM_P::get_instr_b(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_serializing<b_serializing_Src, b_serializing_Trg> *ic = (Inst_serializing<b_serializing_Src, b_serializing_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::serializingClass,p);
    return ic;
  }
  
  // Factory function for instruction "bc".
  Inst_P *OSM_P::get_instr_bc(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_serializing<bc_serializing_Src, bc_serializing_Trg> *ic = (Inst_serializing<bc_serializing_Src, bc_serializing_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::serializingClass,p);
    return ic;
  }
  
  // Factory function for instruction "bcl".
  Inst_P *OSM_P::get_instr_bcl(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_bcl' is not implemented in this model.");
  }
  
  // Factory function for instruction "bclr".
  Inst_P *OSM_P::get_instr_bclr(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_serializing<bclr_serializing_Src, bclr_serializing_Trg> *ic = (Inst_serializing<bclr_serializing_Src, bclr_serializing_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::serializingClass,p);
    return ic;
  }
  
  // Factory function for instruction "bclrl".
  Inst_P *OSM_P::get_instr_bclrl(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_bclrl' is not implemented in this model.");
  }
  
  // Factory function for instruction "bl".
  Inst_P *OSM_P::get_instr_bl(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_serializing<bl_serializing_Src, bl_serializing_Trg> *ic = (Inst_serializing<bl_serializing_Src, bl_serializing_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::serializingClass,p);
    return ic;
  }
  
  // Factory function for instruction "cmp".
  Inst_P *OSM_P::get_instr_cmp(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<cmp_sfx_Src, cmp_sfx_Trg> *ic = (Inst_sfx<cmp_sfx_Src, cmp_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    ic->_masks[1] = 0xffffffff;
    ic->_masks[2] = 0xffffffff;
    return ic;
  }
  
  // Factory function for instruction "cmpi".
  Inst_P *OSM_P::get_instr_cmpi(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<cmpi_sfx_Src, cmpi_sfx_Trg> *ic = (Inst_sfx<cmpi_sfx_Src, cmpi_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    ic->_masks[1] = 0xffffffff;
    ic->_masks[2] = 0xffffffff;
    return ic;
  }
  
  // Factory function for instruction "halt".
  Inst_P *OSM_P::get_instr_halt(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_serializing<DummyOp, DummyOp> *ic = (Inst_serializing<DummyOp, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::serializingClass,p);
    return ic;
  }
  
  // Factory function for instruction "lbz".
  Inst_P *OSM_P::get_instr_lbz(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lbz_load_Src, lbz_load_Trg> *ic = (Inst_load<lbz_load_Src, lbz_load_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::loadClass,p);
    return ic;
  }
  
  // Factory function for instruction "lbzu".
  Inst_P *OSM_P::get_instr_lbzu(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_lbzu' is not implemented in this model.");
  }
  
  // Factory function for instruction "lhz".
  Inst_P *OSM_P::get_instr_lhz(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lhz_load_Src, lhz_load_Trg> *ic = (Inst_load<lhz_load_Src, lhz_load_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::loadClass,p);
    return ic;
  }
  
  // Factory function for instruction "lmw".
  Inst_P *OSM_P::get_instr_lmw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_lmw' is not implemented in this model.");
  }
  
  // Factory function for instruction "lwz".
  Inst_P *OSM_P::get_instr_lwz(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lwz_load_Src, lwz_load_Trg> *ic = (Inst_load<lwz_load_Src, lwz_load_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::loadClass,p);
    return ic;
  }
  
  // Factory function for instruction "lwzu".
  Inst_P *OSM_P::get_instr_lwzu(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lwzu_load_Src, lwzu_load_Trg> *ic = (Inst_load<lwzu_load_Src, lwzu_load_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::loadClass,p);
    return ic;
  }
  
  // Factory function for instruction "lwzx".
  Inst_P *OSM_P::get_instr_lwzx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lwzx_load_Src, lwzx_load_Trg> *ic = (Inst_load<lwzx_load_Src, lwzx_load_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::loadClass,p);
    return ic;
  }
  
  // Factory function for instruction "mfspr".
  Inst_P *OSM_P::get_instr_mfspr(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<mfspr_sfx_Src, mfspr_sfx_Trg> *ic = (Inst_sfx<mfspr_sfx_Src, mfspr_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "mtspr".
  Inst_P *OSM_P::get_instr_mtspr(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<mtspr_sfx_Src, mtspr_sfx_Trg> *ic = (Inst_sfx<mtspr_sfx_Src, mtspr_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "mulli".
  Inst_P *OSM_P::get_instr_mulli(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_cfx<mulli_cfx_Src, mulli_cfx_Trg> *ic = (Inst_cfx<mulli_cfx_Src, mulli_cfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::cfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "mullw".
  Inst_P *OSM_P::get_instr_mullw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_cfx<mullw_cfx_Src, mullw_cfx_Trg> *ic = (Inst_cfx<mullw_cfx_Src, mullw_cfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::cfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "or".
  Inst_P *OSM_P::get_instr_or(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<or_sfx_Src, or_sfx_Trg> *ic = (Inst_sfx<or_sfx_Src, or_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "ori".
  Inst_P *OSM_P::get_instr_ori(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<ori_sfx_Src, ori_sfx_Trg> *ic = (Inst_sfx<ori_sfx_Src, ori_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "oris".
  Inst_P *OSM_P::get_instr_oris(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_oris' is not implemented in this model.");
  }
  
  // Factory function for instruction "rlwinm".
  Inst_P *OSM_P::get_instr_rlwinm(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<rlwinm_sfx_Src, rlwinm_sfx_Trg> *ic = (Inst_sfx<rlwinm_sfx_Src, rlwinm_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "sc".
  Inst_P *OSM_P::get_instr_sc(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_sc' is not implemented in this model.");
  }
  
  // Factory function for instruction "srw".
  Inst_P *OSM_P::get_instr_srw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<srw_sfx_Src, srw_sfx_Trg> *ic = (Inst_sfx<srw_sfx_Src, srw_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x1f;
    return ic;
  }
  
  // Factory function for instruction "stb".
  Inst_P *OSM_P::get_instr_stb(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<stb_store_Src, DummyOp> *ic = (Inst_store<stb_store_Src, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::storeClass,p);
    return ic;
  }
  
  // Factory function for instruction "sth".
  Inst_P *OSM_P::get_instr_sth(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<sth_store_Src, DummyOp> *ic = (Inst_store<sth_store_Src, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::storeClass,p);
    return ic;
  }
  
  // Factory function for instruction "sthx".
  Inst_P *OSM_P::get_instr_sthx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<sthx_store_Src, DummyOp> *ic = (Inst_store<sthx_store_Src, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::storeClass,p);
    return ic;
  }
  
  // Factory function for instruction "stmw".
  Inst_P *OSM_P::get_instr_stmw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_stmw' is not implemented in this model.");
  }
  
  // Factory function for instruction "stw".
  Inst_P *OSM_P::get_instr_stw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<stw_store_Src, DummyOp> *ic = (Inst_store<stw_store_Src, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::storeClass,p);
    return ic;
  }
  
  // Factory function for instruction "stwu".
  Inst_P *OSM_P::get_instr_stwu(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<stwu_store_Src, stwu_store_Trg> *ic = (Inst_store<stwu_store_Src, stwu_store_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::storeClass,p);
    return ic;
  }
  
  // Factory function for instruction "stwx".
  Inst_P *OSM_P::get_instr_stwx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<stwx_store_Src, DummyOp> *ic = (Inst_store<stwx_store_Src, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::storeClass,p);
    return ic;
  }
  
  // Factory function for instruction "decode_miss".
  Inst_P *OSM_P::get_instr_decode_miss(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_decode_miss' is not implemented in this model.");
  }
  
  void OSM_P::exec(Inst_P&instr,Logger *logger)
  {
    instr._instr->exec(*_iss);
    if (logger) {
      logger->logMessage("exec");
      std::pair<adl::BranchType,bool> bi = instr.branch_info(*this);
      if (bi.first) logger->logBranch(bi.second,(bi.second) ? instr.branch_target() : 0);
    }
  }
  
  void OSM_P::exec_mem(Inst_P&instr,Logger *logger)
  {
    if (logger) { logger->logMessage("exec"); };
    set_cur_instr(&instr);
    instr._status = instr._instr->exec(*_iss);
  }
  
  void OSM_P::exec_mem_no_ma(Inst_P&instr,Logger *logger)
  {
    if (logger) { logger->logMessage("exec"); };
    struct IgnoreMem {
      IgnoreMem(OSM_P &x) : _x(x) { _x._ignore_mem = true; };
      ~IgnoreMem() { _x._ignore_mem = false; };
      OSM_P &_x;
    } ignore_mem(*this);
    
    instr._status = instr._instr->exec(*_iss);
  }
  
  addr_t OSM_P::get_pc() const {
    return _iss->NIA_read().uint64();
  }
  
  void OSM_P::set_pc(addr_t a) {
    _iss->NIA_write_simple(a);
  }
  
  void OSM_P::check_breakpoints(addr_t ea)
  {
    _iss->check_breakpoints(ea);
  };
  
  void OSM_P::clear_exceptions(Inst_P &instr)
  {
    if (!instr.is_fetch_exception()) {
      instr._instr->clear_exceptions();
    }
  }
  
  void OSM_P::commit_exceptions(Inst_P &instr,Logger *logger)
  {
  };
  
  void OSM_P::cancel_all_mem(Inst_P &instr)
  {
    UadlMemAccess *ma = instr.mem();
    while (ma) {
      if (ma->valid_request_id()) {
        mMEM_obj.cancel(ma->request_id(),true);
      }
      ma = ma->_next;
    }
  }
  
  void OSM_P::commit_fetch_exception(Inst_P&instr)
  {
  }
  
  void OSM_P::exec_mBR(Inst_P&instr,Logger *logger,unsigned position)
  {
    exec(instr,logger);
    if (_iss->activeCount() == 0) flush_mBR(instr,logger,position);
  }
  
  void OSM_P::exec_mem_mBR(Inst_P&instr,Logger *logger,unsigned position)
  {
    exec_mem(instr,logger);
    if (_iss->activeCount() == 0) flush_mBR(instr,logger,position);
  }
  
  void OSM_P::exec_mEX0(Inst_P&instr,Logger *logger,unsigned position)
  {
    exec(instr,logger);
    if (_iss->activeCount() == 0) flush_mEX0(instr,logger,position);
  }
  
  void OSM_P::exec_mem_mEX0(Inst_P&instr,Logger *logger,unsigned position)
  {
    exec_mem(instr,logger);
    if (_iss->activeCount() == 0) flush_mEX0(instr,logger,position);
  }
  
  void OSM_P::exec_mID(Inst_P&instr,Logger *logger,unsigned position)
  {
    exec(instr,logger);
    if (_iss->activeCount() == 0) flush_mID(instr,logger,position);
  }
  
  void OSM_P::exec_mem_mID(Inst_P&instr,Logger *logger,unsigned position)
  {
    exec_mem(instr,logger);
    if (_iss->activeCount() == 0) flush_mID(instr,logger,position);
  }
  
  void OSM_P::exec_mLS0(Inst_P&instr,Logger *logger,unsigned position)
  {
    exec(instr,logger);
    if (_iss->activeCount() == 0) flush_mLS0(instr,logger,position);
  }
  
  void OSM_P::exec_mem_mLS0(Inst_P&instr,Logger *logger,unsigned position)
  {
    exec_mem(instr,logger);
    if (_iss->activeCount() == 0) flush_mLS0(instr,logger,position);
  }
  
  void OSM_P::exec_mLS1(Inst_P&instr,Logger *logger,unsigned position)
  {
    exec(instr,logger);
    if (_iss->activeCount() == 0) flush_mLS1(instr,logger,position);
  }
  
  void OSM_P::exec_mem_mLS1(Inst_P&instr,Logger *logger,unsigned position)
  {
    exec_mem(instr,logger);
    if (_iss->activeCount() == 0) flush_mLS1(instr,logger,position);
  }
  
  void OSM_P::exec_mWB(Inst_P&instr,Logger *logger,unsigned position)
  {
    exec(instr,logger);
    if (_iss->activeCount() == 0) flush_mWB(instr,logger,position);
  }
  
  void OSM_P::exec_mem_mWB(Inst_P&instr,Logger *logger,unsigned position)
  {
    exec_mem(instr,logger);
    if (_iss->activeCount() == 0) flush_mWB(instr,logger,position);
  }
  
  void OSM_P::flush_issue_stage(Logger *logger,unsigned position,bool given_addr,addr_t addr)
  {
    if (!Issue.empty()) {
      Inst_P*instr = Issue.instr();
      if (!instr->is_fetch_exception()) Fetcher.removeInstr(instr->_instr->size());
      Issue.remove_instr();
      discard(instr);
    }
    flush_issue(logger,given_addr,addr);
  }
  
  void OSM_P::flush_mWB(Inst_P&ic,Logger *logger,unsigned position)
  {
    if (ic.iid() >= _resetFetchId) {
      set_pc(ic._instr->addr() + ic._instr->size());
    }
    taken_flush_mWB(ic,logger,position);
  }
  
  void OSM_P::taken_flush_mWB(Inst_P&ic,Logger *logger,unsigned position,bool given_addr,addr_t addr)
  {
    if (ic.iid() >= _resetFetchId) {
      if (!given_addr && ic._instr) {
        if (ic.branch_taken(*this)) {
          set_pc(ic.branch_target());
        } else {
          set_pc(ic._instr->addr() + ic._instr->size());
        }
      }
    }
    position = 0;
    for (int i = position+1; i < 4; ++i) {
      if (Inst_P *instr = mWB.remove_instr(i)) {
        if (instr->is_alive()) {
          instr->flush(*this,logger);
          discard(instr);
          instr->set_dead();
        }
      }
    }
    mWB.update_tail();
    for (unsigned i = 0; i != 1; ++i) {
      if (Inst_P *instr = mBR[i]) {
        if (instr->is_alive()) {
          (instr->_flush)(instr,*this,logger);
          discard(instr);
          instr->set_dead();
        }
      }
    }
    mBR.reset();
    for (unsigned i = 0; i != 2; ++i) {
      if (Inst_P *instr = mEX0[i]) {
        if (instr->is_alive()) {
          (instr->_flush)(instr,*this,logger);
          discard(instr);
          instr->set_dead();
        }
      }
    }
    mEX0.reset();
    for (unsigned i = 0; i != 1; ++i) {
      if (Inst_P *instr = mLS1[i]) {
        if (instr->is_alive()) {
          (instr->_flush)(instr,*this,logger);
          discard(instr);
          instr->set_dead();
        }
      }
    }
    mLS1.reset();
    for (unsigned i = 0; i != 1; ++i) {
      if (Inst_P *instr = mLS0[i]) {
        if (instr->is_alive()) {
          (instr->_flush)(instr,*this,logger);
          discard(instr);
          instr->set_dead();
        }
      }
    }
    mLS0.reset();
    for (unsigned i = 0; i != 4; ++i) {
      if (Inst_P *instr = mID[i]) {
        if (instr->is_alive()) {
          (instr->_flush)(instr,*this,logger);
          discard(instr);
          instr->set_dead();
        }
      }
    }
    mID.reset();
    flush_issue_stage(logger,position,given_addr,addr);
  }
  
  void OSM_P::flush_mBR(Inst_P&ic,Logger *logger,unsigned position)
  {
    if (ic.iid() >= _resetFetchId) {
      set_pc(ic._instr->addr() + ic._instr->size());
    }
    taken_flush_mBR(ic,logger,position);
  }
  
  void OSM_P::taken_flush_mBR(Inst_P&ic,Logger *logger,unsigned position,bool given_addr,addr_t addr)
  {
    if (ic.iid() >= _resetFetchId) {
      if (!given_addr && ic._instr) {
        if (ic.branch_taken(*this)) {
          set_pc(ic.branch_target());
        } else {
          set_pc(ic._instr->addr() + ic._instr->size());
        }
      }
    }
    mBR.update_tail();
    for (unsigned i = 0; i != 2; ++i) {
      if (Inst_P *instr = mEX0[i]) {
        if (instr->is_alive()) {
          (instr->_flush)(instr,*this,logger);
          discard(instr);
          instr->set_dead();
        }
      }
    }
    mEX0.reset();
    for (unsigned i = 0; i != 1; ++i) {
      if (Inst_P *instr = mLS1[i]) {
        if (instr->is_alive()) {
          (instr->_flush)(instr,*this,logger);
          discard(instr);
          instr->set_dead();
        }
      }
    }
    mLS1.reset();
    for (unsigned i = 0; i != 1; ++i) {
      if (Inst_P *instr = mLS0[i]) {
        if (instr->is_alive()) {
          (instr->_flush)(instr,*this,logger);
          discard(instr);
          instr->set_dead();
        }
      }
    }
    mLS0.reset();
    for (unsigned i = 0; i != 4; ++i) {
      if (Inst_P *instr = mID[i]) {
        if (instr->is_alive()) {
          (instr->_flush)(instr,*this,logger);
          discard(instr);
          instr->set_dead();
        }
      }
    }
    mID.reset();
    flush_issue_stage(logger,position,given_addr,addr);
  }
  
  void OSM_P::flush_mEX0(Inst_P&ic,Logger *logger,unsigned position)
  {
    if (ic.iid() >= _resetFetchId) {
      set_pc(ic._instr->addr() + ic._instr->size());
    }
    taken_flush_mEX0(ic,logger,position);
  }
  
  void OSM_P::taken_flush_mEX0(Inst_P&ic,Logger *logger,unsigned position,bool given_addr,addr_t addr)
  {
    if (ic.iid() >= _resetFetchId) {
      if (!given_addr && ic._instr) {
        if (ic.branch_taken(*this)) {
          set_pc(ic.branch_target());
        } else {
          set_pc(ic._instr->addr() + ic._instr->size());
        }
      }
    }
    mEX0.update_tail();
    for (unsigned i = 0; i != 1; ++i) {
      if (Inst_P *instr = mLS1[i]) {
        if (instr->is_alive()) {
          (instr->_flush)(instr,*this,logger);
          discard(instr);
          instr->set_dead();
        }
      }
    }
    mLS1.reset();
    for (unsigned i = 0; i != 1; ++i) {
      if (Inst_P *instr = mLS0[i]) {
        if (instr->is_alive()) {
          (instr->_flush)(instr,*this,logger);
          discard(instr);
          instr->set_dead();
        }
      }
    }
    mLS0.reset();
    for (unsigned i = 0; i != 4; ++i) {
      if (Inst_P *instr = mID[i]) {
        if (instr->is_alive()) {
          (instr->_flush)(instr,*this,logger);
          discard(instr);
          instr->set_dead();
        }
      }
    }
    mID.reset();
    flush_issue_stage(logger,position,given_addr,addr);
  }
  
  void OSM_P::flush_mLS1(Inst_P&ic,Logger *logger,unsigned position)
  {
    if (ic.iid() >= _resetFetchId) {
      set_pc(ic._instr->addr() + ic._instr->size());
    }
    taken_flush_mLS1(ic,logger,position);
  }
  
  void OSM_P::taken_flush_mLS1(Inst_P&ic,Logger *logger,unsigned position,bool given_addr,addr_t addr)
  {
    if (ic.iid() >= _resetFetchId) {
      if (!given_addr && ic._instr) {
        if (ic.branch_taken(*this)) {
          set_pc(ic.branch_target());
        } else {
          set_pc(ic._instr->addr() + ic._instr->size());
        }
      }
    }
    mLS1.update_tail();
    for (unsigned i = 0; i != 1; ++i) {
      if (Inst_P *instr = mLS0[i]) {
        if (instr->is_alive()) {
          (instr->_flush)(instr,*this,logger);
          discard(instr);
          instr->set_dead();
        }
      }
    }
    mLS0.reset();
    for (unsigned i = 0; i != 4; ++i) {
      if (Inst_P *instr = mID[i]) {
        if (instr->is_alive()) {
          (instr->_flush)(instr,*this,logger);
          discard(instr);
          instr->set_dead();
        }
      }
    }
    mID.reset();
    flush_issue_stage(logger,position,given_addr,addr);
  }
  
  void OSM_P::flush_mLS0(Inst_P&ic,Logger *logger,unsigned position)
  {
    if (ic.iid() >= _resetFetchId) {
      set_pc(ic._instr->addr() + ic._instr->size());
    }
    taken_flush_mLS0(ic,logger,position);
  }
  
  void OSM_P::taken_flush_mLS0(Inst_P&ic,Logger *logger,unsigned position,bool given_addr,addr_t addr)
  {
    if (ic.iid() >= _resetFetchId) {
      if (!given_addr && ic._instr) {
        if (ic.branch_taken(*this)) {
          set_pc(ic.branch_target());
        } else {
          set_pc(ic._instr->addr() + ic._instr->size());
        }
      }
    }
    mLS0.update_tail();
    for (unsigned i = 0; i != 4; ++i) {
      if (Inst_P *instr = mID[i]) {
        if (instr->is_alive()) {
          (instr->_flush)(instr,*this,logger);
          discard(instr);
          instr->set_dead();
        }
      }
    }
    mID.reset();
    flush_issue_stage(logger,position,given_addr,addr);
  }
  
  void OSM_P::flush_mID(Inst_P&ic,Logger *logger,unsigned position)
  {
    if (ic.iid() >= _resetFetchId) {
      set_pc(ic._instr->addr() + ic._instr->size());
    }
    taken_flush_mID(ic,logger,position);
  }
  
  void OSM_P::taken_flush_mID(Inst_P&ic,Logger *logger,unsigned position,bool given_addr,addr_t addr)
  {
    if (ic.iid() >= _resetFetchId) {
      if (!given_addr && ic._instr) {
        if (ic.branch_taken(*this)) {
          set_pc(ic.branch_target());
        } else {
          set_pc(ic._instr->addr() + ic._instr->size());
        }
      }
    }
    position = 0;
    for (int i = position+1; i < 4; ++i) {
      if (Inst_P *instr = mID.remove_instr(i)) {
        if (instr->is_alive()) {
          instr->flush(*this,logger);
          discard(instr);
          instr->set_dead();
        }
      }
    }
    mID.update_tail();
    flush_issue_stage(logger,position,given_addr,addr);
  }
  
  void OSM_P::taken_flush_Issue(Inst_P&,Logger *logger,unsigned,bool given_addr,addr_t addr)
  {
    flush_issue(logger,given_addr,addr);
  }
  
  void OSM_P::flush_issue(Logger *logger,bool given_addr,addr_t addr)
  {
    if (logger) logger->logFlush("pipeline");
    _iss->log_flush_pending();
    Fetcher.flush((given_addr) ? addr : get_pc());
    ++_flushCount;
  }
  
  void OSM_P::log_instr_read(addr_t ea,addr_t ra) {
    _iss->log_instr_read(ea,ra);
  }
  bool OSM_P::issueInquire()
  {
    return Issue.empty() && mID.has_space();
  }
  
  Inst_P *OSM_P::issueInstruction()
  {
    Inst_P *instr = 0;
    
    if (FetchData fd = Fetcher.instrData(_iss->tracing_on())) {
      instr = _iss->decode(fd._ea, fd._data, 0, fd._size, *this);
      Fetcher.decodeUpdate(instr, instr ? instr->size() : 0);
      if (!instr) return 0;
    } else if (!Fetcher.empty()) {
      return 0;
    }
    
    return instr;
  }
  
  void OSM_P::issue(InstrPacket *instr)
  {
    Fetcher.removeInstr(instr->size());
  }
  
  void OSM_P::commit(Inst_P*instr)
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
  
  void OSM_P::commit_checked(Inst_P*instr)
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
  
  void OSM_P::discard(Inst_P*instr)
  {
    if (instr->is_alive()) {
      cancel_all_mem(*instr);
      free_all_mem(*instr);
      _iss->discard_packet(instr->_instr);
      free_instr(instr);
      instr->set_dead();
    }
  }
  
  void OSM_P::reset_fetch()
  {
    if (_resetFetch) {
      _resetFetch = false;
      Fetcher.reset(get_pc());
    }
  }
  
  void OSM_P::force_reset_fetch()
  {
    Fetcher.reset(get_pc());
  }
  
  addr_t OSM_P::ea(addr_t addr) { return _iss->mask_ea(addr); }
  
  bool OSM_P::fetchTranslate(addr_t ea, addr_t &ra)
  {
    _iss->prefetch_translate(ra, ea);
    return true;
  }
  
  bool OSM_P::fetchTranslate(addr_t ea, addr_t &ra, unsigned frontPadSize, unsigned size,byte_t *data)
  {
    PrefetchReturnType prefetchStatus = adl::prNotActive;
    
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
    
    ra = _iss->memory_request(0)._ra;
    
    return true;
  }
  
  
  bool OSM_P::fetchCacheEnabled(addr_t addr)
  {
    return false;
  }
  
  bool OSM_P::issueInstructions()
  {
    if (!Issue.empty() && Issue.instr()->action(*this,((Tracing && logPipeline()) ? Issue.logger(false,0): 0),0)) {
      Issue.remove_instr();
    }
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
    return true;
  }
  
  void OSM_P::update_tracing()
  {
    unsigned tflags ATTRIBUTE_UNUSED = (_out) ? LogAll : LogNone;
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
    if (logPipeline()) {
      mWB.createLogger(_logclient,tflags);
      _stage_loggers.set(0,mWB.logger(0));
      _stage_loggers.set(1,mWB.logger(1));
      _stage_loggers.set(2,mWB.logger(2));
      _stage_loggers.set(3,mWB.logger(3));
      mBR.createLogger(_logclient,tflags);
      _stage_loggers.set(4,mBR.logger(0));
      mEX0.createLogger(_logclient,tflags);
      _stage_loggers.set(5,mEX0.logger(0));
      _stage_loggers.set(6,mEX0.logger(1));
      mLS1.createLogger(_logclient,tflags);
      _stage_loggers.set(7,mLS1.logger(0));
      mLS0.createLogger(_logclient,tflags);
      _stage_loggers.set(8,mLS0.logger(0));
      mID.createLogger(_logclient,tflags);
      _stage_loggers.set(9,mID.logger(0));
      _stage_loggers.set(10,mID.logger(1));
      _stage_loggers.set(11,mID.logger(2));
      _stage_loggers.set(12,mID.logger(3));
    } else {
      mWB.removeLogger();
      mBR.removeLogger();
      mEX0.removeLogger();
      mLS1.removeLogger();
      mLS0.removeLogger();
      mID.removeLogger();
      _stage_loggers.reset();
    }
    if (logMemory()) {
      mMEM_obj.createLogger(_logclient,tflags);
    } else {
      mMEM_obj.removeLogger();
    }
  }
  
  void OSM_P::report_and_clear(ostream &o,const string &pfx)
  {
    *_out << "time " << dec << currTime() << ":\n";
    report_and_clear_top(o,pfx);
    mID.report_and_clear(o,pfx);
    mLS0.report_and_clear(o,pfx);
    mLS1.report_and_clear(o,pfx);
    mEX0.report_and_clear(o,pfx);
    mBR.report_and_clear(o,pfx);
    mWB.report_and_clear(o,pfx);
    *_out << '\n';
    report_and_clear_bottom(o,pfx);
  }
  
  void OSM_P::report_and_clear_top(ostream &o,const string &pfx)
  {
    Fetcher.report_and_clear(o,pfx);
    Issue.report_and_clear(o,pfx);
  }
  
  void OSM_P::report_and_clear_bottom(ostream &o,const string &pfx)
  {
    mMEM_obj.report_and_clear(o,pfx);
    *_out << '\n';
  }
  
  adl::IssNode& OSM_P::iss() { return *_iss; }
  const adl::IssNode& OSM_P::iss() const { return *_iss; }
  ISS_P& OSM_P::get_iss() { return *_iss; }
  
  void OSM_P::preCycle()
  {
    mMEM_obj.preCycle();
    Fetcher.receiveInstrData();
    if (logPipeline()) {
      Fetcher.instrBufReport();
    }
  }
  
  void OSM_P::postCycle()
  {
    if (_iss->activeCount()) Fetcher.sendFetchRequests();
    mMEM_obj.postCycle();
    FwdCB.postCycle();
    if (logMemory()) mMEM_obj.logAllRequests();
    if (tracing_on() && _out) {
      report_and_clear(*_out,string());
    }
    incrTime();
  }
  
  void OSM_P::cycle()
  {
    try {
      preCycle();
      _stall = false;
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(0+mWB.pos(0)) : 0);
        Logger *logger1 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(0+mWB.pos(1)) : 0);
        Logger *logger2 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(0+mWB.pos(2)) : 0);
        Logger *logger3 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(0+mWB.pos(3)) : 0);
        if (!mWB.empty() && (mWB.head()->is_in_stage(&mWB))) {
          if (mWB.head()->action(*this,logger0,0)) {
            if (!mWB.empty() && (mWB.head()->is_in_stage(&mWB))) {
              if (mWB.head()->action(*this,logger1,1)) {
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
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(4+mBR.pos(0)) : 0);
        if (!mBR.empty()) {
          if (mBR.head()->action(*this,logger0,0)) {
          } else {
            _stall = true;
          }
        }
      }
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(5+mEX0.pos(0)) : 0);
        Logger *logger1 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(5+mEX0.pos(1)) : 0);
        unsigned sindex = 0;
        if (mEX0[sindex]) {
          if (!mEX0[sindex]->action(*this,logger0,sindex)) {
            _stall = true;
            ++sindex;
          }
          if (mEX0[sindex]) {
            if (!mEX0[sindex]->action(*this,logger1,sindex)) {
              _stall = true;
              ++sindex;
            }
          }
        }
      }
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(7+mLS1.pos(0)) : 0);
        if (!mLS1.empty()) {
          if (mLS1.head()->action(*this,logger0,0)) {
          } else {
            _stall = true;
          }
        }
      }
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(8+mLS0.pos(0)) : 0);
        if (!mLS0.empty()) {
          if (mLS0.head()->action(*this,logger0,0)) {
          } else {
            _stall = true;
          }
        }
      }
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(9+mID.pos(0)) : 0);
        Logger *logger1 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(9+mID.pos(1)) : 0);
        Logger *logger2 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(9+mID.pos(2)) : 0);
        Logger *logger3 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(9+mID.pos(3)) : 0);
        if (!mID.empty()) {
          if (mID.head()->action(*this,logger0,0)) {
            if (!mID.empty()) {
              if (mID.head()->action(*this,logger1,1)) {
              } else {
                _stall = true;
              }
            }
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
  
  void OSM_P::simulate()
  {
    while (active() && (currTime() < _endTime )) {
      cycle();
    }
  }
  
  OSM_P::~OSM_P() {
    delete _iss;
  }
  
  OSM_P::OSM_P(unsigned &id, unsigned &thread_id, std::ostream &out, ttime_t startTime) : 
  UadlArch(startTime,out), 
  _breakpoint(false), 
  _thread_id(thread_id++), 
  _iss(new ISS_P(*this,id)), 
  mBR(getPath(_iss,".") + "mBR"), 
  mEX0(getPath(_iss,".") + "mEX0"), 
  mID(getPath(_iss,".") + "mID"), 
  mLS0(getPath(_iss,".") + "mLS0"), 
  mLS1(getPath(_iss,".") + "mLS1"), 
  mWB(getPath(_iss,".") + "mWB"), 
  mMEM_obj(*this, logWarnings()), 
  mMEM(mMEM_obj), 
  _log_mem(true), 
  CIA("CIA"), 
  CR("CR"), 
  CTR("CTR"), 
  GPR("GPR"), 
  HID0("HID0"), 
  HID1("HID1"), 
  LR("LR"), 
  NIA("NIA"), 
  XER("XER"), 
  FwdCB("FwdCB"), 
  Fetcher(*this,getPath(_iss,".")+"Fetcher",8,mMEM_obj,0,_fetchSizes,&mMEM_obj), 
  _cur_instr(0), 
  _ignore_mem(false), 
  Issue("         "), 
  _stall(false), 
  _stage_loggers(13), 
  top(*this)
  {
    alloc_mem_access_data();
    _iss->_uarch = this;
    mMEM_obj.set_level(0);
    mMEM_obj.set_level(0);
  };
  
  void OSM_P::reset()
  {
    UadlArch::reset();
    _breakpoint = false;
    mBR.reset();
    mEX0.reset();
    mID.reset();
    mLS0.reset();
    mLS1.reset();
    mWB.reset();
    mMEM_obj.reset();
    _mem_access_pool.reset();
    _log_mem = true;
    CIA.reset();
    CR.reset();
    CTR.reset();
    GPR.reset();
    HID0.reset();
    HID1.reset();
    LR.reset();
    NIA.reset();
    XER.reset();
    FwdCB.reset();
    Fetcher.full_reset();
    _cur_instr = 0;
    _ignore_mem = false;
    Issue.reset();
    _instr_pool.reset();
    _stall = false;
  }
  
  bool OSM_P::active() const
  {
    if (breakpoint()) return false;
    return (_iss->activeCount() || instr_alloc_count() || mMEM_obj.isActive());
  }
  
  bool OSM_P::is_active() const
  {
    return active();
  }
  
  void OSM_P::setBreakpoint()
  {
    _breakpoint = true;
  }
  
  void OSM_P::clearBreakpoint()
  {
    _breakpoint = false;
  }
  
  bool OSM_P::breakpoint () const
  {
    return _breakpoint
    ;
  }
  
  bool OSM_P::hasBreakpoint() const
  {
    return breakpoint();
  }
  
  void OSM_P::preRun()
  {
    reset_fetch();
    _hadReset = false;
  }
  
  void OSM_P::proceed()
  {
    try {
      cycle();
    }
    catch (SimInterrupt) {
      setBreakpoint();
    }
  }
  
  void OSM_P::run(ttime_t endTime)
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
  
  void OSM_P::run()
  {
    try {
      preRun();
      simulate();
    }
    catch (SimInterrupt) {
      setBreakpoint();
    }
  }
  
  void OSM_P::postSim()
  {
  }
  
  unsigned OSM_P::fetchStalls() const { return Fetcher.fetchStalls(); }
  
  unsigned OSM_P::dmemStalls() const { return mMEM_obj.memStalls(0); }
  
  unsigned OSM_P::imemStalls() const { return mMEM_obj.memStalls(0); }
  
  UadlArch *createArch(const string &name, unsigned &id,UadlMemoryIf *memory = NULL,ttime_t startTime = 0)
  {
    if (memory) {
      cerr << "Warning:  An external memory was specified but the model was created to use internal memory.\n";}
    
    unsigned thread_id = 0;
    return new OSM_P(id,thread_id,std::cout,startTime);
  }
  
  
  
} // namespace uadl

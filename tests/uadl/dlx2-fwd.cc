//
// ADL/Uadl version 3.17.2
//
// Cycle-accurate/approximate model generated for P
//

#ifdef UADL_SEPARATE_COMPILE
#  define ADL_HEADER
#endif
#include "dlx2-fwd-iss.cc"
#define _OSM_P_IMPLEMENTATION_H
#include "dlx2-fwd.h"

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
  // Operands for instruction class branch
  //
  
  // Operands:
  //   (src: CIA)
  //   (src: CR/p)
  //   (src: CTR?)
  struct bc_branch_Src {
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
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).CIA.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).CIA.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CIA.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).CR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).CTR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).CTR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CTR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).CIA.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).CIA.last_allocator( static_cast<OSM_P&>(uarch),0));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),0);
        }
        else {
          read(uarch,ic,logger,0);
        }
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).CR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),1);
        }
        else {
          read(uarch,ic,logger,1);
        }
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).CTR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).CTR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),2);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),2);
        }
        else {
          read(uarch,ic,logger,2);
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
  struct bc_branch_Trg {
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
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).CTR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).CTR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CTR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).NIA.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).NIA.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.NIA.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
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
  struct b_branch_Src {
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
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).CIA.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).CIA.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CIA.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).CIA.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).CIA.last_allocator( static_cast<OSM_P&>(uarch),0));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name());
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name());
        }
        else {
          read(uarch,ic,logger);
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
  struct b_branch_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).NIA.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).NIA.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.NIA.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
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
  
  // Operand Alias:  b_branch_Src -> bl_branch_Src
  //   (src: CIA)
  typedef b_branch_Src bl_branch_Src;
  
  // Operands:
  //   (trg: LR)
  //   (trg: NIA)
  struct bl_branch_Trg {
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
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).LR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).LR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.LR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).NIA.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).NIA.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.NIA.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
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
  struct bclr_branch_Src {
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
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).CR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).CTR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).CTR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CTR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).LR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).LR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.LR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).CR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),0);
        }
        else {
          read(uarch,ic,logger,0);
        }
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).CTR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).CTR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),1);
        }
        else {
          read(uarch,ic,logger,1);
        }
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).LR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).LR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),2);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),2);
        }
        else {
          read(uarch,ic,logger,2);
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
  struct bclr_branch_Trg {
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
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).CTR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).CTR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CTR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).NIA.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).NIA.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.NIA.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
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
  // Instruction class branch
  //
  template <class Src_t, class Trg_t>
  struct Inst_branch : public Inst_P {
    
    typedef struct Inst_branch<Src_t, Trg_t> ThisClass;
    
    Inst_branch &thisInstr() { return *this; };
    
    Src_t Src;
    Trg_t Trg;
    void reset(unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_P::reset(iid,cid,ic);
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    
    void allocate_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      Src.allocate(uarch,ic,logger);
      Trg.allocate(uarch,ic,logger);
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
    
    bool move_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EX__S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EX__S_MM(uarch,logger,position);
    }
    
    bool change_state_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EX__S_MM_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_EX__S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      return action_S_EXe(uarch,logger,position);
    }
    
    bool move_S_EXe__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EXe__S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EXe__S_MMe(uarch,logger,position);
    }
    
    bool change_state_S_EXe__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EXe__S_MMe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_ID__S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_IDe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_IDe"); }
      _action = (StateFunc)action_S_IDe_p;
      _flush = (FlushFunc)flush_S_IDe_p;
      return action_S_IDe(uarch,logger,position);
    }
    
    bool change_state_S_IDe__S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDe -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool move_S_MM__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MM__S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MM__S_WB(uarch,logger,position);
    }
    
    bool change_state_S_MM__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MM__S_WB_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_MM__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      return action_S_MMe(uarch,logger,position);
    }
    
    bool move_S_MMe__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MMe__S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MMe__S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_MMe__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MMe__S_WBe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool move_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WB__S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WB__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WB__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_WB__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool move_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WBe__S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WBe__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WBe__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, uarch.mEX.capacity() ) ) {
            return change_state_S_EX__S_MM(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_EXe__S_MMe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      try {
        if ( Src.can_read_fp(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) && Trg.can_write(uarch,*this,logger) && uarch.mEX.has_space(uarch,*this,logger) ) {
              Src.read(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) ;
              allocate_ops(uarch,*this,logger) ;
              uarch.exec_branch(*this,logger) ;
              write_ops(uarch,*this,logger) ;
               addr_t trg = branch_target (  ) ;
              if ( ! uarch.last_branch_prediction_correct(*this  ) ) {
                    uarch.taken_flush_mID(*this,logger,position  ) ;
                }
              uarch.BP.update(uarch,*this,logger) ;
              return change_state_S_ID__S_EX(uarch,logger,position) ;
        }}
      catch (adliss::P::Exception) {
        if (logger) logger->logMessage("exception occurred");
        flush_S_ID(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mID(*this,logger,position,true,uarch.get_pc());
        return change_state_S_ID__S_IDe(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_IDe
    bool action_S_IDe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mEX.has_space(uarch,*this,logger) ) {
            return change_state_S_IDe__S_EXe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_IDe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IDe(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.has_space(uarch,*this,logger) && ! uarch.mID.has_attr(uarch,*this,logger, 0 , adliss::P::branch_instrs | adliss::P::load_instrs | adliss::P::store_instrs | adliss::P::special_instrs | adliss::P::serialized_instrs ) ) {
            return change_state_S_INIT__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_MM
    bool action_S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, uarch.mMM.capacity() ) ) {
            return change_state_S_MM__S_WB(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // State action S_MMe
    bool action_S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_MMe__S_WBe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MMe(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WB__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WB(uarch,logger,position);
    }
    
    // State action S_WBe
    bool action_S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WBe__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WBe(uarch,logger,position);
    }
    
    // Flush action S_EX
    void flush_S_EX(OSM_P &uarch,Logger *logger) {
      deallocate_ops(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
      deallocate_ops(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_IDe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_P &uarch,Logger *logger) {
      deallocate_ops(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_MMe
    void flush_S_MMe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMe(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_P &uarch,Logger *logger) {
      deallocate_ops(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBe(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class decode_miss
  //
  
  // Operands:
  //   (src: ESR)
  struct decode_miss_decode_miss_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.ESR.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).ESR.name(),-1,ic._instr->slot(0).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).ESR.name(),-1,ic._instr->slot(0).uint64());
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
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).ESR.name(static_cast<OSM_P&>(uarch),0); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).ESR.id(static_cast<OSM_P&>(uarch),0); }
    
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
  // Instruction class decode_miss
  //
  template <class Src_t, class Trg_t>
  struct Inst_decode_miss : public Inst_P {
    
    typedef struct Inst_decode_miss<Src_t, Trg_t> ThisClass;
    
    Inst_decode_miss &thisInstr() { return *this; };
    
    Src_t Src;
    Trg_t Trg;
    void reset(unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_P::reset(iid,cid,ic);
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    
    bool can_read_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      return (Src.can_read(uarch,ic,logger) && Trg.can_read(uarch,ic,logger));
    }
    
    bool can_write_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      return (Src.can_write(uarch,ic,logger) && Trg.can_write(uarch,ic,logger));
    }
    
    void read_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      Src.read(uarch,ic,logger);
      Trg.read(uarch,ic,logger);
    }
    
    void deallocate_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      Src.deallocate(uarch,ic,logger);
      Trg.deallocate(uarch,ic,logger);
    }
    
    bool exec_done() const { return true; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool move_S_EXe__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EXe__S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EXe__S_MMe(uarch,logger,position);
    }
    
    bool change_state_S_EXe__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EXe__S_MMe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_ID__S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_IDe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_IDe"); }
      _action = (StateFunc)action_S_IDe_p;
      _flush = (FlushFunc)flush_S_IDe_p;
      return action_S_IDe(uarch,logger,position);
    }
    
    bool change_state_S_IDe__S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDe -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool move_S_MMe__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MMe__S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MMe__S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_MMe__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MMe__S_WBe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool move_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WBe__S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WBe__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WBe__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_EXe__S_MMe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      try {
        if ( can_read_ops(uarch,*this,logger) && can_write_ops(uarch,*this,logger) && uarch.mEX.has_space(uarch,*this,logger) ) {
              read_ops(uarch,*this,logger) ;
              uarch.exec(*this,logger) ;
              return change_state_S_ID__S_EXe(uarch,logger,position) ;
        }}
      catch (adliss::P::Exception) {
        if (logger) logger->logMessage("exception occurred");
        flush_S_ID(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mID(*this,logger,position,true,uarch.get_pc());
        return change_state_S_ID__S_IDe(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_IDe
    bool action_S_IDe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mEX.has_space(uarch,*this,logger) ) {
            return change_state_S_IDe__S_EXe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_IDe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IDe(uarch,logger,position);
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
    
    // State action S_MMe
    bool action_S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_MMe__S_WBe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MMe(uarch,logger,position);
    }
    
    // State action S_WBe
    bool action_S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WBe__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WBe(uarch,logger,position);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_IDe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MMe
    void flush_S_MMe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMe(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBe(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class fetch_exception
  //
  
  //
  // Instruction class fetch_exception
  //
  struct Inst_fetch_exception : public Inst_P {
    
    typedef struct Inst_fetch_exception ThisClass;
    
    Inst_fetch_exception &thisInstr() { return *this; };
    
    void reset(unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_P::reset(iid,cid,ic);
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    
    void deallocate_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
    }
    
    bool exec_done() const { return true; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool move_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EX__S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EX__S_MM(uarch,logger,position);
    }
    
    bool change_state_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EX__S_MM_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_ID__S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
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
    
    bool change_state_S_INIT__S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool move_S_MM__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MM__S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MM__S_WB(uarch,logger,position);
    }
    
    bool change_state_S_MM__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MM__S_WB_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool move_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      uarch.commit_checked(this);
      return true;
    }
    
    static bool move_S_WB__S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WB__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WB__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_EX__S_MM(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mEX.has_space(uarch,*this,logger) ) {
            return change_state_S_ID__S_EX(uarch,logger,position) ;
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
    
    // State action S_MM
    bool action_S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_MM__S_WB(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WB__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WB(uarch,logger,position);
    }
    
    // Flush action S_EX
    void flush_S_EX(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class isync
  //
  
  //
  // Instruction class isync
  //
  struct Inst_isync : public Inst_P {
    
    typedef struct Inst_isync ThisClass;
    
    Inst_isync &thisInstr() { return *this; };
    
    void reset(unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_P::reset(iid,cid,ic);
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    
    void deallocate_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
    }
    
    bool exec_done() const { return true; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool move_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EX__S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EX__S_MM(uarch,logger,position);
    }
    
    bool change_state_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EX__S_MM_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_ID__S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool move_S_MM__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MM__S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MM__S_WB(uarch,logger,position);
    }
    
    bool change_state_S_MM__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MM__S_WB_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool move_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WB__S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WB__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WB__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_EX__S_MM(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mEX.empty(uarch,*this,logger) && uarch.mMM.empty(uarch,*this,logger) && uarch.mWB.empty(uarch,*this,logger) ) {
            uarch.exec_mID(*this,logger,position) ;
            uarch.flush_mID(*this,logger,position  ) ;
            return change_state_S_ID__S_EX(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.empty(uarch,*this,logger) ) {
            uarch.set_speculative(*this  ) ;
            return change_state_S_INIT__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_MM
    bool action_S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_MM__S_WB(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WB__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WB(uarch,logger,position);
    }
    
    // Flush action S_EX
    void flush_S_EX(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class lmw
  //
  
  // Operands:
  //   (src: FOO[mask 0x00100000]/p)
  //   (src: GPR(RA))
  //   (src: dsESR?)
  //   (src: dsFoo[mask 0xff000000]/p?)
  struct lmw_lmw_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.FOO.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
      if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.dsFoo.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).FOO.name(),-1,ic._instr->slot(0).uint64());
      ic._instr->read_operand(uarch.get_iss(),3);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index,ic._instr->slot(3).uint64());
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsESR.name(),-1,ic._instr->slot(1).uint64());
      ic._instr->read_operand(uarch.get_iss(),2);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsFoo.name(),-1,ic._instr->slot(2).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).FOO.name(),-1,ic._instr->slot(0).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index,ic._instr->slot(3).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsESR.name(),-1,ic._instr->slot(1).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsFoo.name(),-1,ic._instr->slot(2).uint64());
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.FOO.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
          return true;
        case 2: if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 3: if (!uarch.dsFoo.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: ic._instr->read_operand(uarch.get_iss(),0);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).FOO.name(),-1,ic._instr->slot(0).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),3);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index,ic._instr->slot(3).uint64());
        break;
        case 2: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsESR.name(),-1,ic._instr->slot(1).uint64());
        break;
        case 3: ic._instr->read_operand(uarch.get_iss(),2);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsFoo.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).FOO.name(),-1,ic._instr->slot(0).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index,ic._instr->slot(3).uint64());
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsESR.name(),-1,ic._instr->slot(1).uint64());
        break;
        case 3: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsFoo.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.FOO.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.FOO.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,0,mask))return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(3)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).dsESR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.dsESR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsFoo.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsFoo.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,3,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        read(uarch,ic,logger,0);
        if (t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t0.read_partial(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        }
        if (t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t1.read_partial(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),0);
        }
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(3)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),1);
        }
        else {
          read(uarch,ic,logger,1);
        }
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).dsESR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),2);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),2);
        }
        else {
          read(uarch,ic,logger,2);
        }
      }
      {
        read(uarch,ic,logger,3);
        if (t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t0.read_partial(uarch,ic,logger,*this,3);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),3);
        }
        if (t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t1.read_partial(uarch,ic,logger,*this,3);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),3);
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
        case 0: return static_cast<OSM_P&>(uarch).FOO.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 2: return static_cast<OSM_P&>(uarch).dsESR.name(static_cast<OSM_P&>(uarch),0);
        case 3: return static_cast<OSM_P&>(uarch).dsFoo.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 2: return static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0);
        case 3: return static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._masks[0];
        case 1: return 0x0;
        case 2: return 0x0;
        case 3: return ic._masks[1];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(3).uint32();
        case 2: return ic._instr->write_mask(1).uint32();
        case 3: return ic._instr->write_mask(2).uint32();
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
        case 2: ic._instr->write_mask(1) = tag;
        break;
        case 3: ic._instr->write_mask(2) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 3;
        case 2: return 1;
        case 3: return 2;
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
  //   (trg: GPR(RT . uint32 (  ),31))
  struct lmw_lmw_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      for (int i = 4; i != 36; ++i) {
        if (ic.op(i).isSrc() && !uarch.GPR.can_alloc(uarch,ic,logger,ic.op(i)._index,"read")) return false;
      }
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        for (int i = 4; i != 36; ++i) {
          if  (ic.op(i).isSrc()) {
            RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(i)._index);
            tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(i)._index));
            if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(i)._index,"read"))) return false;
          }
        }
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      for (int i = 4; i != 36; ++i) {
        if (ic.op(i).isTrg()) {
          uarch.GPR.allocate(uarch,ic,logger,ic.op(i)._index);
        }
      }
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      for (int i = 4; i != 36; ++i) {
        if (ic.op(i).isTrg()) {
          uarch.GPR.deallocate(uarch,ic,logger,ic.op(i)._index);
        }
      }
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      for (int i = 4; i != 36; ++i) {
        if (ic.op(i).isTrg()) {
          ic._instr->commit(uarch.get_iss(),i);
          if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(i)._index,ic._instr->slot(i).uint64());
        }
      }
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(4)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(4);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(op_off)._index); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(op_off)._index); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(4).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(4) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return op_off; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  //
  // Instruction class lmw
  //
  template <class Src_t, class Trg_t>
  struct Inst_lmw : public Inst_P {
    
    typedef struct Inst_lmw<Src_t, Trg_t> ThisClass;
    
    Inst_lmw &thisInstr() { return *this; };
    
    Src_t Src;
    Trg_t Trg;
    void reset(unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_P::reset(iid,cid,ic);
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
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
    
    bool move_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EX__S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EX__S_MM(uarch,logger,position);
    }
    
    bool change_state_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EX__S_MM_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_EX__S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      return action_S_EXe(uarch,logger,position);
    }
    
    bool move_S_EXe__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mEX.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_EXe__S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EXe__S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_EXe__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EXe__S_WBe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_ID__S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_IDe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_IDe"); }
      _action = (StateFunc)action_S_IDe_p;
      _flush = (FlushFunc)flush_S_IDe_p;
      return action_S_IDe(uarch,logger,position);
    }
    
    bool change_state_S_IDe__S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDe -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool change_state_S_MM__S_MMp(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMp"); }
      _action = (StateFunc)action_S_MMp_p;
      _flush = (FlushFunc)flush_S_MMp_p;
      return action_S_MMp(uarch,logger,position);
    }
    
    bool change_state_S_MMp__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMp -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      return action_S_MM(uarch,logger,position);
    }
    
    bool move_S_MMp__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMp -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MMp__S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MMp__S_WB(uarch,logger,position);
    }
    
    bool change_state_S_MMp__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MMp__S_WB_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool move_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WB__S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WB__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WB__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_WB__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool move_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WBe__S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WBe__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WBe__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      try {
        if ( uarch.mMM.has_space(uarch,*this,logger) && uarch.Mem.can_request_read(uarch,*this,logger) ) {
              uarch.exec_mem(*this,logger) ;
              uarch.Mem.send_read_request(uarch,*this,logger, 8 ) ;
              next_req_mem (  ) ;
              return change_state_S_EX__S_MM(uarch,logger,position) ;
        }}
      catch (adliss::P::Exception) {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_EX(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mEX(*this,logger,position,true,uarch.get_pc());
        return change_state_S_EX__S_EXe(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_EXe__S_WBe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      try {
        if ( uarch.mEX.has_space(uarch,*this,logger) ) if ( Src.can_read_fp(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) && Trg.can_write(uarch,*this,logger) && uarch.all_stall.can_allocate(uarch,*this,logger) ) {
              Src.read(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) ;
              Trg.allocate(uarch,*this,logger) ;
              return change_state_S_ID__S_EX(uarch,logger,position) ;
        }}
      catch (adliss::P::Exception) {
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
    static bool action_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_IDe
    bool action_S_IDe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mEX.has_space(uarch,*this,logger) ) {
            return change_state_S_IDe__S_EXe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_IDe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IDe(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.has_space(uarch,*this,logger) && ! uarch.mID.has_attr(uarch,*this,logger, 0 , adliss::P::special_instrs | adliss::P::store_instrs | adliss::P::load_instrs | adliss::P::muldiv_instrs | adliss::P::serialized_instrs | adliss::P::branch_instrs ) ) {
            return change_state_S_INIT__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_MM
    bool action_S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.Mem.can_read(uarch,*this,logger) ) {
            uarch.exec_and_read(*this,logger, uarch.Mem , 2 ) ;
            return change_state_S_MM__S_MMp(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // State action S_MMp
    bool action_S_MMp(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( exec_done (  ) && uarch.mWB.has_space(uarch,*this,logger) ) {
            return change_state_S_MMp__S_WB(uarch,logger,position) ;
      } else if ( uarch.Mem.can_request_read(uarch,*this,logger) ) {
            uarch.Mem.send_read_request(uarch,*this,logger, 8 ) ;
            next_req_mem (  ) ;
            return change_state_S_MMp__S_MM(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MMp_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MMp(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      try {
        {
              write_ops(uarch,*this,logger) ;
              return change_state_S_WB__S_INIT(uarch,logger,position) ;
        }}
      catch (adliss::P::Exception) {
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
    static bool action_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WB(uarch,logger,position);
    }
    
    // State action S_WBe
    bool action_S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WBe__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WBe(uarch,logger,position);
    }
    
    // Flush action S_EX
    void flush_S_EX(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_IDe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_MMp
    void flush_S_MMp(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_MMp_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMp(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBe(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class load
  //
  
  // Operands:
  //   (src: FOO[mask 0x00100000]/p)
  //   (src: GPR(RA))
  //   (src: dsESR?)
  //   (src: dsFoo[mask 0xff000000]/p?)
  struct lbz_load_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.FOO.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
      if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.dsFoo.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).FOO.name(),-1,ic._instr->slot(0).uint64());
      ic._instr->read_operand(uarch.get_iss(),3);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index,ic._instr->slot(3).uint64());
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsESR.name(),-1,ic._instr->slot(1).uint64());
      ic._instr->read_operand(uarch.get_iss(),2);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsFoo.name(),-1,ic._instr->slot(2).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).FOO.name(),-1,ic._instr->slot(0).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index,ic._instr->slot(3).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsESR.name(),-1,ic._instr->slot(1).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsFoo.name(),-1,ic._instr->slot(2).uint64());
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.FOO.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
          return true;
        case 2: if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 3: if (!uarch.dsFoo.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: ic._instr->read_operand(uarch.get_iss(),0);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).FOO.name(),-1,ic._instr->slot(0).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),3);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index,ic._instr->slot(3).uint64());
        break;
        case 2: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsESR.name(),-1,ic._instr->slot(1).uint64());
        break;
        case 3: ic._instr->read_operand(uarch.get_iss(),2);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsFoo.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).FOO.name(),-1,ic._instr->slot(0).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index,ic._instr->slot(3).uint64());
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsESR.name(),-1,ic._instr->slot(1).uint64());
        break;
        case 3: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsFoo.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.FOO.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.FOO.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,0,mask))return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(3)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).dsESR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.dsESR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsFoo.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsFoo.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,3,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        read(uarch,ic,logger,0);
        if (t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t0.read_partial(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        }
        if (t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t1.read_partial(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),0);
        }
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(3)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),1);
        }
        else {
          read(uarch,ic,logger,1);
        }
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).dsESR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),2);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),2);
        }
        else {
          read(uarch,ic,logger,2);
        }
      }
      {
        read(uarch,ic,logger,3);
        if (t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t0.read_partial(uarch,ic,logger,*this,3);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),3);
        }
        if (t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t1.read_partial(uarch,ic,logger,*this,3);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),3);
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
        case 0: return static_cast<OSM_P&>(uarch).FOO.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 2: return static_cast<OSM_P&>(uarch).dsESR.name(static_cast<OSM_P&>(uarch),0);
        case 3: return static_cast<OSM_P&>(uarch).dsFoo.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 2: return static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0);
        case 3: return static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._masks[0];
        case 1: return 0x0;
        case 2: return 0x0;
        case 3: return ic._masks[1];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(3).uint32();
        case 2: return ic._instr->write_mask(1).uint32();
        case 3: return ic._instr->write_mask(2).uint32();
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
        case 2: ic._instr->write_mask(1) = tag;
        break;
        case 3: ic._instr->write_mask(2) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 3;
        case 2: return 1;
        case 3: return 2;
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
  //   (trg: GPR(RT))
  struct lbz_load_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(4)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(4)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(4)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(4)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(4)._index);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(4)._index);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),4);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index,ic._instr->slot(4).uint64());
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(4)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(4);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(4)._index); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(4)._index); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(4).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(4) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 4; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operand Alias:  lbz_load_Src -> lhz_load_Src
  //   (src: FOO[mask 0x00100000]/p)
  //   (src: GPR(RA))
  //   (src: dsESR?)
  //   (src: dsFoo[mask 0xff000000]/p?)
  typedef lbz_load_Src lhz_load_Src;
  
  // Operand Alias:  lbz_load_Trg -> lhz_load_Trg
  //   (trg: GPR(RT))
  typedef lbz_load_Trg lhz_load_Trg;
  
  // Operand Alias:  lbz_load_Src -> lwz_load_Src
  //   (src: FOO[mask 0x00100000]/p)
  //   (src: GPR(RA))
  //   (src: dsESR?)
  //   (src: dsFoo[mask 0xff000000]/p?)
  typedef lbz_load_Src lwz_load_Src;
  
  // Operand Alias:  lbz_load_Trg -> lwz_load_Trg
  //   (trg: GPR(RT))
  typedef lbz_load_Trg lwz_load_Trg;
  
  // Operands:
  //   (src: FOO[mask 0x00100000]/p)
  //   (src: GPR(RA . uint32 (  )))
  //   (src: dsESR?)
  //   (src: dsFoo[mask 0xff000000]/p?)
  struct lwzu_load_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.FOO.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
      if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.dsFoo.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).FOO.name(),-1,ic._instr->slot(0).uint64());
      ic._instr->read_operand(uarch.get_iss(),3);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index,ic._instr->slot(3).uint64());
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsESR.name(),-1,ic._instr->slot(1).uint64());
      ic._instr->read_operand(uarch.get_iss(),2);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsFoo.name(),-1,ic._instr->slot(2).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).FOO.name(),-1,ic._instr->slot(0).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index,ic._instr->slot(3).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsESR.name(),-1,ic._instr->slot(1).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsFoo.name(),-1,ic._instr->slot(2).uint64());
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.FOO.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
          return true;
        case 2: if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 3: if (!uarch.dsFoo.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: ic._instr->read_operand(uarch.get_iss(),0);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).FOO.name(),-1,ic._instr->slot(0).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),3);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index,ic._instr->slot(3).uint64());
        break;
        case 2: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsESR.name(),-1,ic._instr->slot(1).uint64());
        break;
        case 3: ic._instr->read_operand(uarch.get_iss(),2);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsFoo.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).FOO.name(),-1,ic._instr->slot(0).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index,ic._instr->slot(3).uint64());
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsESR.name(),-1,ic._instr->slot(1).uint64());
        break;
        case 3: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsFoo.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.FOO.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.FOO.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,0,mask))return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(3)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).dsESR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.dsESR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsFoo.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsFoo.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,3,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        read(uarch,ic,logger,0);
        if (t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t0.read_partial(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        }
        if (t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t1.read_partial(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),0);
        }
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(3)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),1);
        }
        else {
          read(uarch,ic,logger,1);
        }
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).dsESR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),2);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),2);
        }
        else {
          read(uarch,ic,logger,2);
        }
      }
      {
        read(uarch,ic,logger,3);
        if (t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t0.read_partial(uarch,ic,logger,*this,3);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),3);
        }
        if (t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t1.read_partial(uarch,ic,logger,*this,3);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),3);
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
        case 0: return static_cast<OSM_P&>(uarch).FOO.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 2: return static_cast<OSM_P&>(uarch).dsESR.name(static_cast<OSM_P&>(uarch),0);
        case 3: return static_cast<OSM_P&>(uarch).dsFoo.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 2: return static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0);
        case 3: return static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._masks[0];
        case 1: return 0x0;
        case 2: return 0x0;
        case 3: return ic._masks[1];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(3).uint32();
        case 2: return ic._instr->write_mask(1).uint32();
        case 3: return ic._instr->write_mask(2).uint32();
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
        case 2: ic._instr->write_mask(1) = tag;
        break;
        case 3: ic._instr->write_mask(2) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 3;
        case 2: return 1;
        case 3: return 2;
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
  //   (trg: GPR(RA . uint32 (  )))
  //   (trg: GPR(RT))
  struct lwzu_load_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(4)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(4)._index,"read")) return false;
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
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(3)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(4)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(4)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(4)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(3)._index);
      uarch.GPR.allocate(uarch,ic,logger,ic.op(4)._index);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(3)._index);
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(4)._index);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),3);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index,ic._instr->slot(3).uint64());
      ic._instr->commit(uarch.get_iss(),4);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index,ic._instr->slot(4).uint64());
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(3)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(3);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(4)._index);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(4)._index);
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
        case 0: return ic._instr->write_mask(3).uint32();
        case 1: return ic._instr->write_mask(4).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: ic._instr->write_mask(3) = tag;
        break;
        case 1: ic._instr->write_mask(4) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 3;
        case 1: return 4;
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
  //   (trg: GPR(RA . uint32 (  )))
  struct lwzu_load_TrgEA {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(3)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(3)._index);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(3)._index);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),3);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index,ic._instr->slot(3).uint64());
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(3)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(3);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(3)._index); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(3).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(3) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 3; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (src: FOO[mask 0x00100000]/p)
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: dsESR?)
  //   (src: dsFoo[mask 0xff000000]/p?)
  struct lwzx_load_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.FOO.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(4)._index,"read")) return false;
      if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.dsFoo.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).FOO.name(),-1,ic._instr->slot(0).uint64());
      ic._instr->read_operand(uarch.get_iss(),3);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index,ic._instr->slot(3).uint64());
      ic._instr->read_operand(uarch.get_iss(),4);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index,ic._instr->slot(4).uint64());
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsESR.name(),-1,ic._instr->slot(1).uint64());
      ic._instr->read_operand(uarch.get_iss(),2);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsFoo.name(),-1,ic._instr->slot(2).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).FOO.name(),-1,ic._instr->slot(0).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index,ic._instr->slot(3).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index,ic._instr->slot(4).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsESR.name(),-1,ic._instr->slot(1).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsFoo.name(),-1,ic._instr->slot(2).uint64());
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.FOO.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
          return true;
        case 2: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(4)._index,"read")) return false;
          return true;
        case 3: if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 4: if (!uarch.dsFoo.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: ic._instr->read_operand(uarch.get_iss(),0);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).FOO.name(),-1,ic._instr->slot(0).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),3);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index,ic._instr->slot(3).uint64());
        break;
        case 2: ic._instr->read_operand(uarch.get_iss(),4);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index,ic._instr->slot(4).uint64());
        break;
        case 3: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsESR.name(),-1,ic._instr->slot(1).uint64());
        break;
        case 4: ic._instr->read_operand(uarch.get_iss(),2);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsFoo.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).FOO.name(),-1,ic._instr->slot(0).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index,ic._instr->slot(3).uint64());
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index,ic._instr->slot(4).uint64());
        break;
        case 3: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsESR.name(),-1,ic._instr->slot(1).uint64());
        break;
        case 4: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsFoo.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.FOO.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.FOO.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,0,mask))return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(3)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(4)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(4)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(4)._index,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).dsESR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.dsESR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsFoo.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsFoo.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,4,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        read(uarch,ic,logger,0);
        if (t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t0.read_partial(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        }
        if (t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t1.read_partial(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),0);
        }
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(3)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),1);
        }
        else {
          read(uarch,ic,logger,1);
        }
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(4)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(4)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),2);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),2);
        }
        else {
          read(uarch,ic,logger,2);
        }
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).dsESR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,3);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),3);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,3);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),3);
        }
        else {
          read(uarch,ic,logger,3);
        }
      }
      {
        read(uarch,ic,logger,4);
        if (t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t0.read_partial(uarch,ic,logger,*this,4);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),4);
        }
        if (t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t1.read_partial(uarch,ic,logger,*this,4);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),4);
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
        case 0: return static_cast<OSM_P&>(uarch).FOO.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 2: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(4)._index);
        case 3: return static_cast<OSM_P&>(uarch).dsESR.name(static_cast<OSM_P&>(uarch),0);
        case 4: return static_cast<OSM_P&>(uarch).dsFoo.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 2: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(4)._index);
        case 3: return static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0);
        case 4: return static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._masks[0];
        case 1: return 0x0;
        case 2: return 0x0;
        case 3: return 0x0;
        case 4: return ic._masks[1];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(3).uint32();
        case 2: return ic._instr->write_mask(4).uint32();
        case 3: return ic._instr->write_mask(1).uint32();
        case 4: return ic._instr->write_mask(2).uint32();
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
        case 2: ic._instr->write_mask(4) = tag;
        break;
        case 3: ic._instr->write_mask(1) = tag;
        break;
        case 4: ic._instr->write_mask(2) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 3;
        case 2: return 4;
        case 3: return 1;
        case 4: return 2;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 5; };
    
  };
  
  // Operands:
  //   (trg: GPR(RT))
  struct lwzx_load_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(5)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(5)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(5)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(5)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(5)._index);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(5)._index);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),5);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(5)._index,ic._instr->slot(5).uint64());
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(5)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(5);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(5)._index); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(5)._index); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(5).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(5) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 5; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  //
  // Instruction class load
  //
  template <class Src_t, class Trg_t, class TrgEA_t>
  struct Inst_load : public Inst_P {
    
    typedef struct Inst_load<Src_t, Trg_t, TrgEA_t> ThisClass;
    
    Inst_load &thisInstr() { return *this; };
    
    Src_t Src;
    Trg_t Trg;
    TrgEA_t TrgEA;
    void reset(unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_P::reset(iid,cid,ic);
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    
    void deallocate_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      Src.deallocate(uarch,ic,logger);
      Trg.deallocate(uarch,ic,logger);
      TrgEA.deallocate(uarch,ic,logger);
    }
    
    void write_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      Src.write(uarch,ic,logger);
      Trg.write(uarch,ic,logger);
      TrgEA.write(uarch,ic,logger);
    }
    
    bool exec_done() const { return _status == adl::MemAccess::opNone; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool move_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EX__S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EX__S_MM(uarch,logger,position);
    }
    
    bool change_state_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EX__S_MM_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_EX__S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      return action_S_EXe(uarch,logger,position);
    }
    
    bool move_S_EXe__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EXe__S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EXe__S_MMe(uarch,logger,position);
    }
    
    bool change_state_S_EXe__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EXe__S_MMe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_ID__S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_IDe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_IDe"); }
      _action = (StateFunc)action_S_IDe_p;
      _flush = (FlushFunc)flush_S_IDe_p;
      return action_S_IDe(uarch,logger,position);
    }
    
    bool change_state_S_IDe__S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDe -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool change_state_S_MM__S_MMp(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMp"); }
      _action = (StateFunc)action_S_MMp_p;
      _flush = (FlushFunc)flush_S_MMp_p;
      return action_S_MMp(uarch,logger,position);
    }
    
    bool change_state_S_MM__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      return action_S_MMe(uarch,logger,position);
    }
    
    bool move_S_MMe__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MMe__S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MMe__S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_MMe__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MMe__S_WBe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_MMp__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMp -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      return action_S_MM(uarch,logger,position);
    }
    
    bool move_S_MMp__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMp -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MMp__S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MMp__S_WB(uarch,logger,position);
    }
    
    bool change_state_S_MMp__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MMp__S_WB_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_WB__S_WBp(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBp"); }
      _action = (StateFunc)action_S_WBp_p;
      _flush = (FlushFunc)flush_S_WBp_p;
      return action_S_WBp(uarch,logger,position);
    }
    
    bool change_state_S_WB__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool move_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WBe__S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WBe__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WBe__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool move_S_WBp__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBp -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WBp__S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WBp__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WBp__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WBp__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      try {
        if ( uarch.mMM.has_space(uarch,*this,logger) && uarch.Mem.can_request_read(uarch,*this,logger) ) {
              uarch.exec_mem(*this,logger) ;
              uarch.FwdEX.write(uarch,*this,logger, TrgEA ) ;
              uarch.Mem.send_read_request(uarch,*this,logger) ;
              return change_state_S_EX__S_MM(uarch,logger,position) ;
        }}
      catch (adliss::P::Exception) {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_EX(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mEX(*this,logger,position,true,uarch.get_pc());
        return change_state_S_EX__S_EXe(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_EXe__S_MMe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      try {
        if ( Src.can_read_fp(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) && Trg.can_write(uarch,*this,logger) && uarch.mEX.has_space(uarch,*this,logger) && uarch.hid_stall.can_allocate(uarch,*this,logger) ) {
              Src.read(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) ;
              Trg.allocate(uarch,*this,logger) ;
              return change_state_S_ID__S_EX(uarch,logger,position) ;
        }}
      catch (adliss::P::Exception) {
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
    static bool action_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_IDe
    bool action_S_IDe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mEX.has_space(uarch,*this,logger) ) {
            return change_state_S_IDe__S_EXe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_IDe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IDe(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.has_space(uarch,*this,logger) && ! uarch.mID.has_attr(uarch,*this,logger, 0 , adliss::P::special_instrs | adliss::P::store_instrs | adliss::P::load_instrs | adliss::P::muldiv_instrs | adliss::P::serialized_instrs | adliss::P::branch_instrs ) ) {
            return change_state_S_INIT__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_MM
    bool action_S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      try {
        if ( uarch.Mem.can_read(uarch,*this,logger) ) {
              uarch.exec_and_read(*this,logger, uarch.Mem ) ;
              uarch.FwdMM.write(uarch,*this,logger, Trg ) ;
              return change_state_S_MM__S_MMp(uarch,logger,position) ;
        }}
      catch (adliss::P::Exception) {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_MM(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mMM(*this,logger,position,true,uarch.get_pc());
        return change_state_S_MM__S_MMe(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // State action S_MMe
    bool action_S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_MMe__S_WBe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MMe(uarch,logger,position);
    }
    
    // State action S_MMp
    bool action_S_MMp(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( exec_done (  ) && uarch.mWB.has_space(uarch,*this,logger, uarch.mMM.capacity() ) ) {
            return change_state_S_MMp__S_WB(uarch,logger,position) ;
      } else if ( uarch.Mem.can_request_read(uarch,*this,logger) ) {
            uarch.Mem.send_read_request(uarch,*this,logger) ;
            return change_state_S_MMp__S_MM(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MMp_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MMp(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      try {
        {
              write_ops(uarch,*this,logger) ;
              return change_state_S_WB__S_WBp(uarch,logger,position) ;
        }}
      catch (adliss::P::Exception) {
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
    static bool action_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WB(uarch,logger,position);
    }
    
    // State action S_WBe
    bool action_S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WBe__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WBe(uarch,logger,position);
    }
    
    // State action S_WBp
    bool action_S_WBp(OSM_P &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WBp__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WBp_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WBp(uarch,logger,position);
    }
    
    // Flush action S_EX
    void flush_S_EX(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_IDe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_MMe
    void flush_S_MMe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMe(uarch,logger);
    }
    
    // Flush action S_MMp
    void flush_S_MMp(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_MMp_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMp(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBe(uarch,logger);
    }
    
    // Flush action S_WBp
    void flush_S_WBp(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_WBp_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBp(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class mtmsr
  //
  
  // Operands:
  //   (src: GPR(RS))
  struct mtmsr_mtmsr_Src {
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
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name());
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name());
        }
        else {
          read(uarch,ic,logger);
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
  //   (trg: MSR)
  struct mtmsr_mtmsr_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.MSR.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).MSR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).MSR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.MSR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.MSR.allocate(uarch,ic,logger);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.MSR.deallocate(uarch,ic,logger);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),1);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).MSR.name(),-1,ic._instr->slot(1).uint64());
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(1);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).MSR.name(static_cast<OSM_P&>(uarch),0); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).MSR.id(static_cast<OSM_P&>(uarch),0); }
    
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
  // Instruction class mtmsr
  //
  template <class Src_t, class Trg_t>
  struct Inst_mtmsr : public Inst_P {
    
    typedef struct Inst_mtmsr<Src_t, Trg_t> ThisClass;
    
    Inst_mtmsr &thisInstr() { return *this; };
    
    Src_t Src;
    Trg_t Trg;
    void reset(unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_P::reset(iid,cid,ic);
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
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
    
    bool move_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EX__S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EX__S_MM(uarch,logger,position);
    }
    
    bool change_state_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EX__S_MM_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_EX__S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      return action_S_EXe(uarch,logger,position);
    }
    
    bool move_S_EXe__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EXe__S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EXe__S_MMe(uarch,logger,position);
    }
    
    bool change_state_S_EXe__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EXe__S_MMe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_ID__S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_IDe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_IDe"); }
      _action = (StateFunc)action_S_IDe_p;
      _flush = (FlushFunc)flush_S_IDe_p;
      return action_S_IDe(uarch,logger,position);
    }
    
    bool change_state_S_IDe__S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDe -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool move_S_MM__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MM__S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MM__S_WB(uarch,logger,position);
    }
    
    bool change_state_S_MM__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MM__S_WB_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_MM__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      return action_S_MMe(uarch,logger,position);
    }
    
    bool move_S_MMe__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MMe__S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MMe__S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_MMe__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MMe__S_WBe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_WB__S_WBp(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBp"); }
      _action = (StateFunc)action_S_WBp_p;
      _flush = (FlushFunc)flush_S_WBp_p;
      return action_S_WBp(uarch,logger,position);
    }
    
    bool change_state_S_WB__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool move_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WBe__S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WBe__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WBe__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool move_S_WBp__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBp -> S_INIT"); }
      uarch.mWB.remove_instr();
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WBp__S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WBp__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WBp__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WBp__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      try {
        if ( uarch.mMM.has_space(uarch,*this,logger, 2 ) ) {
              uarch.exec(*this,logger) ;
              return change_state_S_EX__S_MM(uarch,logger,position) ;
        }}
      catch (adliss::P::Exception) {
        if (logger) logger->logMessage("exception occurred");
        flush_S_EX(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mEX(*this,logger,position,true,uarch.get_pc());
        return change_state_S_EX__S_EXe(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_EXe__S_MMe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      try {
        {
              if ( Src.can_read_fp(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) && uarch.all_stall.can_allocate(uarch,*this,logger) && uarch.hid_stall.can_allocate(uarch,*this,logger) && uarch.mEX.empty(uarch,*this,logger) && uarch.mMM.empty(uarch,*this,logger) && uarch.mWB.empty(uarch,*this,logger) ) {
                    Src.read(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) ;
                    Trg.allocate(uarch,*this,logger) ;
                    uarch.all_stall.allocate(uarch,*this,logger) ;
                    uarch.hid_stall.allocate(uarch,*this,logger) ;
                    return change_state_S_ID__S_EX(uarch,logger,position) ;
                }
        }}
      catch (adliss::P::Exception) {
        if (logger) logger->logMessage("exception occurred");
        flush_S_ID(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mID(*this,logger,position,true,uarch.get_pc());
        return change_state_S_ID__S_IDe(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_IDe
    bool action_S_IDe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mEX.has_space(uarch,*this,logger) ) {
            return change_state_S_IDe__S_EXe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_IDe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IDe(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.empty(uarch,*this,logger) ) {
            return change_state_S_INIT__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_MM
    bool action_S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_MM__S_WB(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // State action S_MMe
    bool action_S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_MMe__S_WBe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MMe(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      try {
        {
              write_ops(uarch,*this,logger) ;
              return change_state_S_WB__S_WBp(uarch,logger,position) ;
        }}
      catch (adliss::P::Exception) {
        if (logger) logger->logMessage("exception occurred");
        flush_S_WB(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mWB(*this,logger,position,true,uarch.get_pc());
        return change_state_S_WB__S_WBe(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WB(uarch,logger,position);
    }
    
    // State action S_WBe
    bool action_S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WBe__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WBe(uarch,logger,position);
    }
    
    // State action S_WBp
    bool action_S_WBp(OSM_P &uarch,Logger *logger,unsigned position) {
      {
            uarch.all_stall.deallocate(uarch,*this,logger) ;
            uarch.hid_stall.deallocate(uarch,*this,logger, uarch.mWB.capacity() ) ;
            return change_state_S_WBp__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WBp_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WBp(uarch,logger,position);
    }
    
    // Flush action S_EX
    void flush_S_EX(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      uarch.all_stall.deallocate(uarch,*this,logger);
      uarch.hid_stall.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      uarch.all_stall.deallocate(uarch,*this,logger);
      uarch.hid_stall.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_IDe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      uarch.all_stall.deallocate(uarch,*this,logger);
      uarch.hid_stall.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_MMe
    void flush_S_MMe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMe(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      uarch.all_stall.deallocate(uarch,*this,logger);
      uarch.hid_stall.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBe(uarch,logger);
    }
    
    // Flush action S_WBp
    void flush_S_WBp(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      uarch.all_stall.deallocate(uarch,*this,logger);
      uarch.hid_stall.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_WBp_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBp(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class mtspr_hid
  //
  
  // Operands:
  //   (src: GPR(RS))
  //   (src: MSR[mask 0x00400000]/p)
  struct mtspr_mtspr_hid_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.MSR.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).MSR.name(),-1,ic._instr->slot(1).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).MSR.name(),-1,ic._instr->slot(1).uint64());
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
          return true;
        case 1: if (!uarch.MSR.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
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
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).MSR.name(),-1,ic._instr->slot(1).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).MSR.name(),-1,ic._instr->slot(1).uint64());
        break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).MSR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.MSR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).MSR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.MSR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,1,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),0);
        }
        else {
          read(uarch,ic,logger,0);
        }
      }
      {
        read(uarch,ic,logger,1);
        if (t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).MSR.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t0.read_partial(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        }
        if (t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).MSR.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t1.read_partial(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),1);
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
        case 1: return static_cast<OSM_P&>(uarch).MSR.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_P&>(uarch).MSR.id(static_cast<OSM_P&>(uarch),0);
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
  //   (trg: SPR(SPRN))
  struct mtspr_mtspr_hid_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.SPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).SPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).SPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(2)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.SPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.SPR.allocate(uarch,ic,logger,ic.op(2)._index);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.SPR.deallocate(uarch,ic,logger,ic.op(2)._index);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),2);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).SPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(2)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(2);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).SPR.name(static_cast<OSM_P&>(uarch),ic.op(2)._index); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).SPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index); }
    
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
  // Instruction class mtspr_hid
  //
  template <class Src_t, class Trg_t>
  struct Inst_mtspr_hid : public Inst_P {
    
    typedef struct Inst_mtspr_hid<Src_t, Trg_t> ThisClass;
    
    Inst_mtspr_hid &thisInstr() { return *this; };
    
    Src_t Src;
    Trg_t Trg;
    void reset(unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_P::reset(iid,cid,ic);
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
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
    
    bool change_state_S_EX__S_EXp(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_EXp"); }
      _action = (StateFunc)action_S_EXp_p;
      _flush = (FlushFunc)flush_S_EXp_p;
      return action_S_EXp(uarch,logger,position);
    }
    
    bool change_state_S_EX__S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      return action_S_EXe(uarch,logger,position);
    }
    
    bool move_S_EXe__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EXe__S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EXe__S_MMe(uarch,logger,position);
    }
    
    bool change_state_S_EXe__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EXe__S_MMe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool move_S_EXp__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXp -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EXp__S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EXp__S_MM(uarch,logger,position);
    }
    
    bool change_state_S_EXp__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EXp__S_MM_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_ID__S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_IDe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_IDe"); }
      _action = (StateFunc)action_S_IDe_p;
      _flush = (FlushFunc)flush_S_IDe_p;
      return action_S_IDe(uarch,logger,position);
    }
    
    bool change_state_S_IDe__S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDe -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool move_S_MM__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MM__S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MM__S_WB(uarch,logger,position);
    }
    
    bool change_state_S_MM__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MM__S_WB_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_MM__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      return action_S_MMe(uarch,logger,position);
    }
    
    bool move_S_MMe__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MMe__S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MMe__S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_MMe__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MMe__S_WBe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_WB__S_WBp(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBp"); }
      _action = (StateFunc)action_S_WBp_p;
      _flush = (FlushFunc)flush_S_WBp_p;
      return action_S_WBp(uarch,logger,position);
    }
    
    bool change_state_S_WB__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool move_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WBe__S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WBe__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WBe__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool move_S_WBp__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBp -> S_INIT"); }
      uarch.mWB.remove_instr();
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WBp__S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WBp__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WBp__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WBp__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      try {
        if ( uarch.Mem.can_force_lock(uarch,*this,logger) ) {
              uarch.Mem.allocate_force_lock(uarch,*this,logger) ;
              uarch.Fetcher.set_cancel(uarch,*this,logger, true ) ;
              uarch.Fetcher.cancel(uarch,*this,logger) ;
              uarch.exec(*this,logger) ;
              set_latency ( uarch.mEX.capacity() ) ;
              return change_state_S_EX__S_EXp(uarch,logger,position) ;
        }}
      catch (adliss::P::Exception) {
        if (logger) logger->logMessage("exception occurred");
        flush_S_EX(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mEX(*this,logger,position,true,uarch.get_pc());
        return change_state_S_EX__S_EXe(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_EXe__S_MMe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_EXp
    bool action_S_EXp(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( check_latency(logger) && uarch.mMM.has_space(uarch,*this,logger, uarch.mEX.capacity() ) ) {
            return change_state_S_EXp__S_MM(uarch,logger,position) ;
      } else {
            decr_latency (  ) ;
      }
      return false;
    }
    static bool action_S_EXp_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXp(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      try {
        {
              if ( Src.can_read_fp(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) && Trg.can_write(uarch,*this,logger) && uarch.mEX.has_space(uarch,*this,logger) ) {
                    Src.read(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) ;
                    Trg.allocate(uarch,*this,logger) ;
                    uarch.hid_stall.allocate(uarch,*this,logger) ;
                    return change_state_S_ID__S_EX(uarch,logger,position) ;
                }
        }}
      catch (adliss::P::Exception) {
        if (logger) logger->logMessage("exception occurred");
        flush_S_ID(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mID(*this,logger,position,true,uarch.get_pc());
        return change_state_S_ID__S_IDe(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_IDe
    bool action_S_IDe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mEX.has_space(uarch,*this,logger) ) {
            return change_state_S_IDe__S_EXe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_IDe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IDe(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.empty(uarch,*this,logger) ) {
            return change_state_S_INIT__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_MM
    bool action_S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_MM__S_WB(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // State action S_MMe
    bool action_S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_MMe__S_WBe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MMe(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      try {
        {
              write_ops(uarch,*this,logger) ;
              return change_state_S_WB__S_WBp(uarch,logger,position) ;
        }}
      catch (adliss::P::Exception) {
        if (logger) logger->logMessage("exception occurred");
        flush_S_WB(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mWB(*this,logger,position,true,uarch.get_pc());
        return change_state_S_WB__S_WBe(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WB(uarch,logger,position);
    }
    
    // State action S_WBe
    bool action_S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WBe__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WBe(uarch,logger,position);
    }
    
    // State action S_WBp
    bool action_S_WBp(OSM_P &uarch,Logger *logger,unsigned position) {
      {
            uarch.Mem.deallocate_force_lock(uarch,*this,logger) ;
            uarch.hid_stall.deallocate(uarch,*this,logger, uarch.mWB.capacity() ) ;
            return change_state_S_WBp__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WBp_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WBp(uarch,logger,position);
    }
    
    // Flush action S_EX
    void flush_S_EX(OSM_P &uarch,Logger *logger) {
      uarch.Mem.deallocate_force_lock(uarch,*this,logger);
      Trg.deallocate(uarch,*this,logger);
      uarch.hid_stall.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_EXp
    void flush_S_EXp(OSM_P &uarch,Logger *logger) {
      uarch.Mem.deallocate_force_lock(uarch,*this,logger);
      Trg.deallocate(uarch,*this,logger);
      uarch.hid_stall.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_EXp_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXp(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      uarch.hid_stall.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_IDe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_P &uarch,Logger *logger) {
      uarch.Mem.deallocate_force_lock(uarch,*this,logger);
      Trg.deallocate(uarch,*this,logger);
      uarch.hid_stall.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_MMe
    void flush_S_MMe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMe(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_P &uarch,Logger *logger) {
      uarch.Mem.deallocate_force_lock(uarch,*this,logger);
      Trg.deallocate(uarch,*this,logger);
      uarch.hid_stall.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBe(uarch,logger);
    }
    
    // Flush action S_WBp
    void flush_S_WBp(OSM_P &uarch,Logger *logger) {
      uarch.Mem.deallocate_force_lock(uarch,*this,logger);
      Trg.deallocate(uarch,*this,logger);
      uarch.hid_stall.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_WBp_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBp(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class muldiv
  //
  
  // Operands:
  //   (src: GPR(RA))
  struct mulli_muldiv_Src {
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
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name());
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name());
        }
        else {
          read(uarch,ic,logger);
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
  struct mulli_muldiv_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
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
  struct mullw_muldiv_Src {
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
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),0);
        }
        else {
          read(uarch,ic,logger,0);
        }
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),1);
        }
        else {
          read(uarch,ic,logger,1);
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
  struct mullw_muldiv_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(2)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
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
  // Instruction class muldiv
  //
  template <class Src_t, class Trg_t>
  struct Inst_muldiv : public Inst_P {
    
    typedef struct Inst_muldiv<Src_t, Trg_t> ThisClass;
    
    Inst_muldiv &thisInstr() { return *this; };
    
    Src_t Src;
    Trg_t Trg;
    void reset(unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_P::reset(iid,cid,ic);
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
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
    
    bool move_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EX__S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EX__S_MM(uarch,logger,position);
    }
    
    bool change_state_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EX__S_MM_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_EX__S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      return action_S_EXe(uarch,logger,position);
    }
    
    bool move_S_EXe__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EXe__S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EXe__S_MMe(uarch,logger,position);
    }
    
    bool change_state_S_EXe__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EXe__S_MMe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_ID__S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_IDe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_IDe"); }
      _action = (StateFunc)action_S_IDe_p;
      _flush = (FlushFunc)flush_S_IDe_p;
      return action_S_IDe(uarch,logger,position);
    }
    
    bool change_state_S_IDe__S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDe -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool move_S_MM__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MM__S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MM__S_WB(uarch,logger,position);
    }
    
    bool change_state_S_MM__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MM__S_WB_p;
      _action = (StateFunc)no_move_action_S_MM_p;
      return true;
    }
    
    bool change_state_S_MM__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      return action_S_MMe(uarch,logger,position);
    }
    
    bool move_S_MMe__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MMe__S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MMe__S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_MMe__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MMe__S_WBe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool move_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WB__S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WB__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WB__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_WB__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool move_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WBe__S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WBe__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WBe__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      try {
        if ( uarch.mMM.has_space(uarch,*this,logger, 2 ) ) {
              uarch.exec(*this,logger) ;
              set_latency ( 4 ) ;
              return change_state_S_EX__S_MM(uarch,logger,position) ;
        }}
      catch (adliss::P::Exception) {
        if (logger) logger->logMessage("exception occurred");
        flush_S_EX(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mEX(*this,logger,position,true,uarch.get_pc());
        return change_state_S_EX__S_EXe(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_EXe__S_MMe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      try {
        if ( Src.can_read_fp(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) && Trg.can_write(uarch,*this,logger) && uarch.mEX.has_space(uarch,*this,logger) && uarch.all_stall.can_allocate(uarch,*this,logger) ) {
              Src.read(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) ;
              Trg.allocate(uarch,*this,logger) ;
              return change_state_S_ID__S_EX(uarch,logger,position) ;
        }}
      catch (adliss::P::Exception) {
        if (logger) logger->logMessage("exception occurred");
        flush_S_ID(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mID(*this,logger,position,true,uarch.get_pc());
        return change_state_S_ID__S_IDe(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_IDe
    bool action_S_IDe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mEX.has_space(uarch,*this,logger) ) {
            return change_state_S_IDe__S_EXe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_IDe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IDe(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.has_space(uarch,*this,logger) && ! uarch.mID.has_attr(uarch,*this,logger, 0 , adliss::P::muldiv_instrs | adliss::P::branch_instrs ) ) {
            return change_state_S_INIT__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_MM
    bool action_S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      try {
        if ( check_latency(logger) && uarch.mWB.has_space(uarch,*this,logger) ) {
              uarch.FwdMM.write(uarch,*this,logger, Trg ) ;
              return change_state_S_MM__S_WB(uarch,logger,position) ;
        } else {
              decr_latency (  ) ;
        }}
      catch (adliss::P::Exception) {
        if (logger) logger->logMessage("exception occurred");
        flush_S_MM(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mMM(*this,logger,position,true,uarch.get_pc());
        return change_state_S_MM__S_MMe(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // Repeat action S_MM
    bool no_move_action_S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      uarch.FwdMM.write(uarch,*this,logger, Trg ) ;
      return true;
    }
    
    static bool no_move_action_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->no_move_action_S_MM(uarch,logger,position);
    }
    
    // State action S_MMe
    bool action_S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_MMe__S_WBe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MMe(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      try {
        {
              write_ops(uarch,*this,logger) ;
              return change_state_S_WB__S_INIT(uarch,logger,position) ;
        }}
      catch (adliss::P::Exception) {
        if (logger) logger->logMessage("exception occurred");
        flush_S_WB(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mWB(*this,logger,position,true,uarch.get_pc());
        return change_state_S_WB__S_WBe(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WB(uarch,logger,position);
    }
    
    // State action S_WBe
    bool action_S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WBe__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WBe(uarch,logger,position);
    }
    
    // Flush action S_EX
    void flush_S_EX(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_IDe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_MMe
    void flush_S_MMe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMe(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
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
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
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
        case 1: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
      }
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
  // Instruction class nop
  //
  template <class __other___t>
  struct Inst_nop : public Inst_P {
    
    typedef struct Inst_nop<__other___t> ThisClass;
    
    Inst_nop &thisInstr() { return *this; };
    
    __other___t __other__;
    void reset(unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_P::reset(iid,cid,ic);
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    
    void deallocate_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      __other__.deallocate(uarch,ic,logger);
    }
    
    bool exec_done() const { return true; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool move_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EX__S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EX__S_MM(uarch,logger,position);
    }
    
    bool change_state_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EX__S_MM_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_ID__S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool move_S_MM__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MM__S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MM__S_WB(uarch,logger,position);
    }
    
    bool change_state_S_MM__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MM__S_WB_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool move_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WB__S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WB__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WB__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, 2 ) ) {
            uarch.exec(*this,logger) ;
            return change_state_S_EX__S_MM(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mEX.has_space(uarch,*this,logger) ) {
            return change_state_S_ID__S_EX(uarch,logger,position) ;
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
    
    // State action S_MM
    bool action_S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_MM__S_WB(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WB__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WB(uarch,logger,position);
    }
    
    // Flush action S_EX
    void flush_S_EX(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class rfi
  //
  
  // Operands:
  //   (src: SRR0)
  //   (src: SRR1)
  struct rfi_rfi_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.SRR0.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.SRR1.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),2);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).SRR0.name(),-1,ic._instr->slot(2).uint64());
      ic._instr->read_operand(uarch.get_iss(),3);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).SRR1.name(),-1,ic._instr->slot(3).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).SRR0.name(),-1,ic._instr->slot(2).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).SRR1.name(),-1,ic._instr->slot(3).uint64());
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.SRR0.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.SRR1.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: ic._instr->read_operand(uarch.get_iss(),2);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).SRR0.name(),-1,ic._instr->slot(2).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),3);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).SRR1.name(),-1,ic._instr->slot(3).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).SRR0.name(),-1,ic._instr->slot(2).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).SRR1.name(),-1,ic._instr->slot(3).uint64());
        break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).SRR0.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).SRR0.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.SRR0.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).SRR1.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).SRR1.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.SRR1.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).SRR0.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).SRR0.last_allocator( static_cast<OSM_P&>(uarch),0));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),0);
        }
        else {
          read(uarch,ic,logger,0);
        }
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).SRR1.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).SRR1.last_allocator( static_cast<OSM_P&>(uarch),0));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),1);
        }
        else {
          read(uarch,ic,logger,1);
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
        case 0: return static_cast<OSM_P&>(uarch).SRR0.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).SRR1.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).SRR0.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).SRR1.id(static_cast<OSM_P&>(uarch),0);
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
  //   (trg: MSR)
  //   (trg: NIA)
  struct rfi_rfi_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.MSR.can_alloc(uarch,ic,logger,"read")) return false;
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
        case 0: if (!uarch.MSR.can_alloc(uarch,ic,logger,"read")) return false;
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
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).MSR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).MSR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.MSR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).NIA.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).NIA.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.NIA.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.MSR.allocate(uarch,ic,logger);
      uarch.NIA.allocate(uarch,ic,logger);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.MSR.deallocate(uarch,ic,logger);
      uarch.NIA.deallocate(uarch,ic,logger);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),0);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).MSR.name(),-1,ic._instr->slot(0).uint64());
      ic._instr->commit(uarch.get_iss(),1);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).NIA.name(),-1,ic._instr->slot(1).uint64());
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
        case 0: return static_cast<OSM_P&>(uarch).MSR.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).NIA.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).MSR.id(static_cast<OSM_P&>(uarch),0);
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
  // Instruction class rfi
  //
  template <class Src_t, class Trg_t>
  struct Inst_rfi : public Inst_P {
    
    typedef struct Inst_rfi<Src_t, Trg_t> ThisClass;
    
    Inst_rfi &thisInstr() { return *this; };
    
    Src_t Src;
    Trg_t Trg;
    void reset(unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_P::reset(iid,cid,ic);
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    
    bool can_write_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      return (Src.can_write(uarch,ic,logger) && Trg.can_write(uarch,ic,logger));
    }
    
    void allocate_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      Src.allocate(uarch,ic,logger);
      Trg.allocate(uarch,ic,logger);
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
    
    bool move_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EX__S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EX__S_MM(uarch,logger,position);
    }
    
    bool change_state_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EX__S_MM_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_ID__S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool move_S_MM__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MM__S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MM__S_WB(uarch,logger,position);
    }
    
    bool change_state_S_MM__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MM__S_WB_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool move_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WB__S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WB__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WB__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, uarch.mEX.capacity() ) ) {
            uarch.exec_branch(*this,logger) ;
            write_ops(uarch,*this,logger) ;
            uarch.taken_flush_mEX(*this,logger,position  ) ;
            return change_state_S_EX__S_MM(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( Src.can_read_fp(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) && can_write_ops(uarch,*this,logger) && uarch.mEX.has_space(uarch,*this,logger) ) {
            Src.read(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) ;
            allocate_ops(uarch,*this,logger) ;
            return change_state_S_ID__S_EX(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.empty(uarch,*this,logger) ) {
            return change_state_S_INIT__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_MM
    bool action_S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_MM__S_WB(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WB__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WB(uarch,logger,position);
    }
    
    // Flush action S_EX
    void flush_S_EX(OSM_P &uarch,Logger *logger) {
      deallocate_ops(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
      deallocate_ops(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_P &uarch,Logger *logger) {
      deallocate_ops(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_P &uarch,Logger *logger) {
      deallocate_ops(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
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
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,1,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),0);
        }
        else {
          read(uarch,ic,logger,0);
        }
      }
      {
        read(uarch,ic,logger,1);
        if (t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t0.read_partial(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        }
        if (t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t1.read_partial(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),1);
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
  //   (trg: CR[mask 0xf0000000]/p)
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0x20000000]/p)
  struct addic__sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[2])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
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
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
        case 2: break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
        case 2: break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.CR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.CR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,0,mask))return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(2)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read"))) return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,2,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CR.allocate(uarch,ic,logger, ic._masks[1]);
      uarch.GPR.allocate(uarch,ic,logger,ic.op(2)._index);
      uarch.XER.allocate(uarch,ic,logger, ic._masks[2]);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CR.deallocate(uarch,ic,logger, ic._masks[1]);
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(2)._index);
      uarch.XER.deallocate(uarch,ic,logger, ic._masks[2]);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),0);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).CR.name(),-1,ic._instr->slot(0).uint64());
      ic._instr->commit(uarch.get_iss(),2);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
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
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
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
        case 1: return ic._instr->write_mask(2).uint32();
        case 2: return ic._instr->write_mask(3).uint32();
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
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 3; };
    
  };
  
  // Operands:
  //   (src: GPR(RA))
  struct addic_sfx_Src {
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
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name());
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name());
        }
        else {
          read(uarch,ic,logger);
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
  //   (trg: XER[mask 0x20000000]/p)
  struct addic_sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
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
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,1,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(1)._index);
      uarch.XER.allocate(uarch,ic,logger, ic._masks[0]);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(1)._index);
      uarch.XER.deallocate(uarch,ic,logger, ic._masks[0]);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),1);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      ic._instr->commit(uarch.get_iss(),2);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(2).uint64());
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
  
  // Operand Alias:  addic_sfx_Src -> addi_sfx_Src
  //   (src: GPR(RA)?)
  typedef addic_sfx_Src addi_sfx_Src;
  
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
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
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
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      ic._instr->read_operand(uarch.get_iss(),2);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(2).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(2).uint64());
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
          return true;
        case 1: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint32())) return false;
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
        case 1: ic._instr->read_operand(uarch.get_iss(),2);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,1,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),0);
        }
        else {
          read(uarch,ic,logger,0);
        }
      }
      {
        read(uarch,ic,logger,1);
        if (t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t0.read_partial(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        }
        if (t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t1.read_partial(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),1);
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
        case 1: return static_cast<OSM_P&>(uarch).XER.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return ic._masks[1];
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
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      ic._instr->read_operand(uarch.get_iss(),3);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
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
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: ic._instr->read_operand(uarch.get_iss(),0);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index,ic._instr->slot(1).uint64());
        break;
        case 2: ic._instr->read_operand(uarch.get_iss(),3);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
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
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
        break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,2,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),0);
        }
        else {
          read(uarch,ic,logger,0);
        }
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),1);
        }
        else {
          read(uarch,ic,logger,1);
        }
      }
      {
        read(uarch,ic,logger,2);
        if (t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t0.read_partial(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),2);
        }
        if (t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t1.read_partial(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),2);
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
        case 2: return static_cast<OSM_P&>(uarch).XER.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
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
  
  // Operands:
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0xc0000000]/p)
  struct addo_sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
          return true;
        case 1: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint32())) return false;
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
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(2)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read"))) return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,1,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(2)._index);
      uarch.XER.allocate(uarch,ic,logger, ic._masks[1]);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(2)._index);
      uarch.XER.deallocate(uarch,ic,logger, ic._masks[1]);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),2);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
      ic._instr->commit(uarch.get_iss(),3);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).XER.name(),-1,ic._instr->slot(3).uint64());
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
        case 1: return ic._masks[1];
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
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name());
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name());
        }
        else {
          read(uarch,ic,logger);
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
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
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
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(2)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read"))) return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,1,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(2)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),0);
        }
        else {
          read(uarch,ic,logger,0);
        }
      }
      {
        read(uarch,ic,logger,1);
        if (t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t0.read_partial(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        }
        if (t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t1.read_partial(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),1);
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
  //   (trg: CR[mask 0xf0000000]/p)
  //   (trg: GPR(RA))
  struct andi__sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.CR.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint32())) return false;
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
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.CR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.CR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,0,mask))return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
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
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._masks[1];
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
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),0);
        }
        else {
          read(uarch,ic,logger,0);
        }
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),1);
        }
        else {
          read(uarch,ic,logger,1);
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
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(2)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
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
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,1,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),0);
        }
        else {
          read(uarch,ic,logger,0);
        }
      }
      {
        read(uarch,ic,logger,1);
        if (t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t0.read_partial(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        }
        if (t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t1.read_partial(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),1);
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
  //   (trg: CR/p)
  struct cmpi_sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).CR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CR.allocate(uarch,ic,logger);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CR.deallocate(uarch,ic,logger);
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
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).CR.name(static_cast<OSM_P&>(uarch),0); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0); }
    
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
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: XER[mask 0x80000000]/p)
  struct cmp_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
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
        case 2: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint32())) return false;
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
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(2)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read"))) return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.XER.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,2,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),0);
        }
        else {
          read(uarch,ic,logger,0);
        }
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(2)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),1);
        }
        else {
          read(uarch,ic,logger,1);
        }
      }
      {
        read(uarch,ic,logger,2);
        if (t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t0.read_partial(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),2);
        }
        if (t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t1.read_partial(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),2);
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
        case 2: return ic._masks[1];
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
  
  // Operand Alias:  cmpi_sfx_Trg -> cmp_sfx_Trg
  //   (trg: CR/p)
  typedef cmpi_sfx_Trg cmp_sfx_Trg;
  
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
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(2)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),0);
        }
        else {
          read(uarch,ic,logger,0);
        }
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(2)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),1);
        }
        else {
          read(uarch,ic,logger,1);
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
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).SPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).SPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.SPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).SPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).SPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name());
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name());
        }
        else {
          read(uarch,ic,logger);
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
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
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
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index));
          mask |= uarch.GPR.rm_later_partial_alloc(uarch,ic.op(1)._index,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index));
          mask |= uarch.GPR.rm_later_partial_alloc(uarch,ic.op(1)._index,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,0,mask))return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(2)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        read(uarch,ic,logger,0);
        if (t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index)).first != 0) {
          t0.read_partial(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        }
        if (t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index)).first != 0) {
          t1.read_partial(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),0);
        }
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(2)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),1);
        }
        else {
          read(uarch,ic,logger,1);
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
  //   (src: MSR[mask 0x00400000]/p)
  struct mtspr_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.MSR.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).MSR.name(),-1,ic._instr->slot(1).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).MSR.name(),-1,ic._instr->slot(1).uint64());
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
          return true;
        case 1: if (!uarch.MSR.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
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
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).MSR.name(),-1,ic._instr->slot(1).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index,ic._instr->slot(0).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).MSR.name(),-1,ic._instr->slot(1).uint64());
        break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).MSR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.MSR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).MSR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.MSR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,1,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),0);
        }
        else {
          read(uarch,ic,logger,0);
        }
      }
      {
        read(uarch,ic,logger,1);
        if (t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).MSR.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t0.read_partial(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        }
        if (t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).MSR.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t1.read_partial(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),1);
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
        case 1: return static_cast<OSM_P&>(uarch).MSR.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_P&>(uarch).MSR.id(static_cast<OSM_P&>(uarch),0);
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
  //   (trg: SPR(SPRN))
  struct mtspr_sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.SPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).SPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).SPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(2)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.SPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.SPR.allocate(uarch,ic,logger,ic.op(2)._index);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.SPR.deallocate(uarch,ic,logger,ic.op(2)._index);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),2);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).SPR.name(),ic.op(2)._index,ic._instr->slot(2).uint64());
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(2)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(2);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).SPR.name(static_cast<OSM_P&>(uarch),ic.op(2)._index); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).SPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(2).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(2) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 2; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operand Alias:  add_sfx_Src -> tw_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef add_sfx_Src tw_sfx_Src;
  
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
    
    bool move_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EX__S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EX__S_MM(uarch,logger,position);
    }
    
    bool change_state_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EX__S_MM_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_EX__S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      return action_S_EXe(uarch,logger,position);
    }
    
    bool move_S_EXe__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EXe__S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EXe__S_MMe(uarch,logger,position);
    }
    
    bool change_state_S_EXe__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EXe__S_MMe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_ID__S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_IDe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_IDe"); }
      _action = (StateFunc)action_S_IDe_p;
      _flush = (FlushFunc)flush_S_IDe_p;
      return action_S_IDe(uarch,logger,position);
    }
    
    bool change_state_S_IDe__S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDe -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool move_S_MM__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MM__S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MM__S_WB(uarch,logger,position);
    }
    
    bool change_state_S_MM__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MM__S_WB_p;
      _action = (StateFunc)no_move_action_S_MM_p;
      return true;
    }
    
    bool change_state_S_MM__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      return action_S_MMe(uarch,logger,position);
    }
    
    bool move_S_MMe__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MMe__S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MMe__S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_MMe__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MMe__S_WBe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool move_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WB__S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WB__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WB__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_WB__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool move_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WBe__S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WBe__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WBe__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      try {
        if ( uarch.mMM.has_space(uarch,*this,logger, 2 ) ) {
              uarch.exec(*this,logger) ;
              uarch.FwdEX.write(uarch,*this,logger, Trg ) ;
              return change_state_S_EX__S_MM(uarch,logger,position) ;
        }}
      catch (adliss::P::Exception) {
        if (logger) logger->logMessage("exception occurred");
        flush_S_EX(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mEX(*this,logger,position,true,uarch.get_pc());
        return change_state_S_EX__S_EXe(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_EXe__S_MMe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      try {
        if ( Src.can_read_fp(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) && Trg.can_write(uarch,*this,logger) && uarch.mEX.has_space(uarch,*this,logger) && uarch.all_stall.can_allocate(uarch,*this,logger) ) {
              Src.read(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) ;
              Trg.allocate(uarch,*this,logger) ;
              return change_state_S_ID__S_EX(uarch,logger,position) ;
        }}
      catch (adliss::P::Exception) {
        if (logger) logger->logMessage("exception occurred");
        flush_S_ID(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mID(*this,logger,position,true,uarch.get_pc());
        return change_state_S_ID__S_IDe(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_IDe
    bool action_S_IDe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mEX.has_space(uarch,*this,logger) ) {
            return change_state_S_IDe__S_EXe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_IDe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IDe(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.has_space(uarch,*this,logger) && ! uarch.mID.has_attr(uarch,*this,logger, 0 , adliss::P::muldiv_instrs | adliss::P::branch_instrs ) ) {
            return change_state_S_INIT__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_MM
    bool action_S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      try {
        if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
              uarch.FwdMM.write(uarch,*this,logger, Trg ) ;
              return change_state_S_MM__S_WB(uarch,logger,position) ;
        }}
      catch (adliss::P::Exception) {
        if (logger) logger->logMessage("exception occurred");
        flush_S_MM(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mMM(*this,logger,position,true,uarch.get_pc());
        return change_state_S_MM__S_MMe(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // Repeat action S_MM
    bool no_move_action_S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      uarch.FwdMM.write(uarch,*this,logger, Trg ) ;
      return true;
    }
    
    static bool no_move_action_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->no_move_action_S_MM(uarch,logger,position);
    }
    
    // State action S_MMe
    bool action_S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_MMe__S_WBe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MMe(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      try {
        {
              write_ops(uarch,*this,logger) ;
              return change_state_S_WB__S_INIT(uarch,logger,position) ;
        }}
      catch (adliss::P::Exception) {
        if (logger) logger->logMessage("exception occurred");
        flush_S_WB(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mWB(*this,logger,position,true,uarch.get_pc());
        return change_state_S_WB__S_WBe(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WB(uarch,logger,position);
    }
    
    // State action S_WBe
    bool action_S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WBe__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WBe(uarch,logger,position);
    }
    
    // Flush action S_EX
    void flush_S_EX(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_IDe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_MMe
    void flush_S_MMe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMe(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBe(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class stmw
  //
  
  // Operands:
  //   (src: FOO[mask 0x00100000]/p)
  //   (src: GPR(RA), mask: 0xffffffff)
  //   (src: GPR(RS . uint32 (  ),31), mask: 0xffffffff)
  //   (src: dsESR?)
  //   (src: dsFoo[mask 0xff000000]/p?)
  struct stmw_stmw_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.FOO.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read", ic._masks[1])) return false;
      for (int i = 4; i != 36; ++i) {
        if (ic.op(i).isSrc() && !uarch.GPR.can_alloc(uarch,ic,logger,ic.op(i)._index,"read", ic._masks[2])) return false;
      }
      if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.dsFoo.can_alloc(uarch,ic,logger,"read", ic._masks[3])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).FOO.name(),-1,ic._instr->slot(0).uint64());
      ic._instr->read_operand(uarch.get_iss(),3);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index,ic._instr->slot(3).uint64());
      for (int i = 4; i != 36; ++i) {
        if (ic.op(i).isSrc()) {
          ic._instr->read_operand(uarch.get_iss(),i);
          if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(i)._index,ic._instr->slot(i).uint64());
        }
      }
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsESR.name(),-1,ic._instr->slot(1).uint64());
      ic._instr->read_operand(uarch.get_iss(),2);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsFoo.name(),-1,ic._instr->slot(2).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).FOO.name(),-1,ic._instr->slot(0).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index,ic._instr->slot(3).uint64());
      for (int i = 4; i != 36; ++i) {
        if (ic.op(i).isSrc()) {
          logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(i)._index,ic._instr->slot(i).uint64());
        }
      }
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsESR.name(),-1,ic._instr->slot(1).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsFoo.name(),-1,ic._instr->slot(2).uint64());
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.FOO.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read", (ic._masks[1] & ~mask).uint32())) return false;
          return true;
        case 2: for (int i = 4; i != 36; ++i) {
          if (ic.op(i).isSrc() && !uarch.GPR.can_alloc(uarch,ic,logger,ic.op(i)._index,"read", (ic._masks[2] & ~mask).uint32())) return false;
        }
          return true;
        case 3: if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 4: if (!uarch.dsFoo.can_alloc(uarch,ic,logger,"read", (ic._masks[3] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: ic._instr->read_operand(uarch.get_iss(),0);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).FOO.name(),-1,ic._instr->slot(0).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),3);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index,ic._instr->slot(3).uint64());
        break;
        case 2: for (int i = 4; i != 36; ++i) {
          if (ic.op(i).isSrc()) {
            ic._instr->read_operand(uarch.get_iss(),i);
            if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(i)._index,ic._instr->slot(i).uint64());
          }
        }
        break;
        case 3: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsESR.name(),-1,ic._instr->slot(1).uint64());
        break;
        case 4: ic._instr->read_operand(uarch.get_iss(),2);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsFoo.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).FOO.name(),-1,ic._instr->slot(0).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index,ic._instr->slot(3).uint64());
        break;
        case 2: for (int i = 4; i != 36; ++i) {
          if (ic.op(i).isSrc()) {
            logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(i)._index,ic._instr->slot(i).uint64());
          }
        }
        break;
        case 3: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsESR.name(),-1,ic._instr->slot(1).uint64());
        break;
        case 4: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsFoo.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.FOO.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.FOO.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,0,mask))return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index));
          mask |= uarch.GPR.rm_later_partial_alloc(uarch,ic.op(3)._index,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index));
          mask |= uarch.GPR.rm_later_partial_alloc(uarch,ic.op(3)._index,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,1,mask))return false;
      }
      for (int i = 4; i != 36; ++i) {
        if  (ic.op(i).isSrc()) {
          bits<32> mask = 0;
          {
            std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(i)._index));
            mask |= uarch.GPR.rm_later_partial_alloc(uarch,ic.op(i)._index,pm.first.uint32(),pm.second);
          }
          {
            std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(i)._index));
            mask |= uarch.GPR.rm_later_partial_alloc(uarch,ic.op(i)._index,pm.first.uint32(),pm.second);
          }
          if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(i)._index,"read", (ic._masks[2] & ~mask).uint32())) return false;
        }
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).dsESR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.dsESR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsFoo.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsFoo.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,4,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        read(uarch,ic,logger,0);
        if (t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t0.read_partial(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        }
        if (t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t1.read_partial(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),0);
        }
      }
      {
        read(uarch,ic,logger,1);
        if (t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index)).first != 0) {
          t0.read_partial(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        }
        if (t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index)).first != 0) {
          t1.read_partial(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),1);
        }
      }
      for (int i = 4; i != 36; ++i) {
        if  (ic.op(i).isSrc()) {
          ic._instr->read_operand(uarch.get_iss(),i);
          if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(i)._index,ic._instr->slot(i).uint64());
          if (t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(i)._index)).first != 0) {
            t0.read_partial(uarch,ic,logger,*this,2,i);
            if (logger) logger->logOpAction("read from "+t0.name(),static_cast<OSM_P&>(uarch).GPR.name(),ic.op(i)._index,ic._instr->slot(i).uint64());
          }
          if (t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(i)._index)).first != 0) {
            t1.read_partial(uarch,ic,logger,*this,2,i);
            if (logger) logger->logOpAction("read from "+t1.name(),static_cast<OSM_P&>(uarch).GPR.name(),ic.op(i)._index,ic._instr->slot(i).uint64());
          }
        }
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).dsESR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,3);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),3);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,3);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),3);
        }
        else {
          read(uarch,ic,logger,3);
        }
      }
      {
        read(uarch,ic,logger,4);
        if (t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t0.read_partial(uarch,ic,logger,*this,4);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),4);
        }
        if (t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t1.read_partial(uarch,ic,logger,*this,4);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),4);
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
        case 0: return static_cast<OSM_P&>(uarch).FOO.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 2: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(op_off)._index);
        case 3: return static_cast<OSM_P&>(uarch).dsESR.name(static_cast<OSM_P&>(uarch),0);
        case 4: return static_cast<OSM_P&>(uarch).dsFoo.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 2: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(op_off)._index);
        case 3: return static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0);
        case 4: return static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._masks[0];
        case 1: return ic._masks[1];
        case 2: return ic._masks[2];
        case 3: return 0x0;
        case 4: return ic._masks[3];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(3).uint32();
        case 2: return ic._instr->write_mask(4).uint32();
        case 3: return ic._instr->write_mask(1).uint32();
        case 4: return ic._instr->write_mask(2).uint32();
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
        case 2: ic._instr->write_mask(4) = tag;
        break;
        case 3: ic._instr->write_mask(1) = tag;
        break;
        case 4: ic._instr->write_mask(2) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 3;
        case 2: return op_off;
        case 3: return 1;
        case 4: return 2;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 5; };
    
  };
  
  // Operands:
  //   (src: CR, mask: 0xffffffff)
  struct stmw_stmw_SrcCR {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read", ic._masks[4])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,const bits<32> &mask) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read", (ic._masks[4] & ~mask).uint32())) return false;
      return true;
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.CR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.CR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        read(uarch,ic,logger);
        if (t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t0.read_partial(uarch,ic,logger,*this);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name());
        }
        if (t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t1.read_partial(uarch,ic,logger,*this);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name());
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
      return 0;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return 0;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).CR.name(static_cast<OSM_P&>(uarch),0); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return ic._masks[4]; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(0).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(0) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 0; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  //
  // Instruction class stmw
  //
  template <class Src_t, class SrcCR_t, class Trg_t>
  struct Inst_stmw : public Inst_P {
    
    typedef struct Inst_stmw<Src_t, SrcCR_t, Trg_t> ThisClass;
    
    Inst_stmw &thisInstr() { return *this; };
    
    Src_t Src;
    SrcCR_t SrcCR;
    Trg_t Trg;
    void reset(unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_P::reset(iid,cid,ic);
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    
    void deallocate_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      Src.deallocate(uarch,ic,logger);
      SrcCR.deallocate(uarch,ic,logger);
      Trg.deallocate(uarch,ic,logger);
    }
    
    void write_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      Src.write(uarch,ic,logger);
      SrcCR.write(uarch,ic,logger);
      Trg.write(uarch,ic,logger);
    }
    
    bool exec_done() const { return _status == adl::MemAccess::opNone; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool move_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EX__S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EX__S_MM(uarch,logger,position);
    }
    
    bool change_state_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EX__S_MM_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_EX__S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      return action_S_EXe(uarch,logger,position);
    }
    
    bool move_S_EXe__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mEX.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_EXe__S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EXe__S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_EXe__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EXe__S_WBe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_ID__S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_IDe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_IDe"); }
      _action = (StateFunc)action_S_IDe_p;
      _flush = (FlushFunc)flush_S_IDe_p;
      return action_S_IDe(uarch,logger,position);
    }
    
    bool change_state_S_IDe__S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDe -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool change_state_S_MM__S_MMp(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMp"); }
      _action = (StateFunc)action_S_MMp_p;
      _flush = (FlushFunc)flush_S_MMp_p;
      return action_S_MMp(uarch,logger,position);
    }
    
    bool change_state_S_MMp__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMp -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      return action_S_MM(uarch,logger,position);
    }
    
    bool move_S_MMp__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMp -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MMp__S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MMp__S_WB(uarch,logger,position);
    }
    
    bool change_state_S_MMp__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MMp__S_WB_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool move_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WB__S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WB__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WB__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_WB__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool move_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WBe__S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WBe__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WBe__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      try {
        if ( uarch.mMM.has_space(uarch,*this,logger) && uarch.Mem.can_request_write(uarch,*this,logger) ) {
              uarch.exec_mem(*this,logger) ;
              uarch.Mem.send_write_request(uarch,*this,logger, 8 ) ;
              next_req_mem (  ) ;
              return change_state_S_EX__S_MM(uarch,logger,position) ;
        }}
      catch (adliss::P::Exception) {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_EX(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mEX(*this,logger,position,true,uarch.get_pc());
        return change_state_S_EX__S_EXe(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_EXe__S_WBe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      try {
        if ( Src.can_read_fp(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) && Trg.can_write(uarch,*this,logger) && SrcCR.can_read_fp(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) && uarch.mEX.has_space(uarch,*this,logger) && uarch.all_stall.can_allocate(uarch,*this,logger) ) {
              Src.read(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) ;
              SrcCR.read(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) ;
              Trg.allocate(uarch,*this,logger) ;
              return change_state_S_ID__S_EX(uarch,logger,position) ;
        }}
      catch (adliss::P::Exception) {
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
    static bool action_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_IDe
    bool action_S_IDe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mEX.has_space(uarch,*this,logger) ) {
            return change_state_S_IDe__S_EXe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_IDe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IDe(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.has_space(uarch,*this,logger) && ! uarch.mID.has_attr(uarch,*this,logger, 0 , adliss::P::special_instrs | adliss::P::store_instrs | adliss::P::load_instrs | adliss::P::muldiv_instrs | adliss::P::serialized_instrs | adliss::P::branch_instrs ) ) {
            return change_state_S_INIT__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_MM
    bool action_S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.Mem.can_write(uarch,*this,logger) ) {
            uarch.exec_and_write(*this,logger, uarch.Mem , 2 , 4 ) ;
            return change_state_S_MM__S_MMp(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // State action S_MMp
    bool action_S_MMp(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( exec_done (  ) && uarch.mWB.has_space(uarch,*this,logger) ) {
            return change_state_S_MMp__S_WB(uarch,logger,position) ;
      } else if ( uarch.Mem.can_request_write(uarch,*this,logger) ) {
            uarch.Mem.send_write_request(uarch,*this,logger, 8 ) ;
            next_req_mem (  ) ;
            return change_state_S_MMp__S_MM(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MMp_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MMp(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      try {
        {
              write_ops(uarch,*this,logger) ;
              return change_state_S_WB__S_INIT(uarch,logger,position) ;
        }}
      catch (adliss::P::Exception) {
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
    static bool action_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WB(uarch,logger,position);
    }
    
    // State action S_WBe
    bool action_S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WBe__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WBe(uarch,logger,position);
    }
    
    // Flush action S_EX
    void flush_S_EX(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_IDe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_MMp
    void flush_S_MMp(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_MMp_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMp(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBe(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class store
  //
  
  // Operands:
  //   (src: FOO[mask 0x00100000]/p)
  //   (src: GPR(RA))
  //   (src: GPR(RS))
  //   (src: dsESR?)
  //   (src: dsFoo[mask 0xff000000]/p?)
  struct stb_store_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.FOO.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(4)._index,"read")) return false;
      if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.dsFoo.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).FOO.name(),-1,ic._instr->slot(0).uint64());
      ic._instr->read_operand(uarch.get_iss(),3);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index,ic._instr->slot(3).uint64());
      ic._instr->read_operand(uarch.get_iss(),4);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index,ic._instr->slot(4).uint64());
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsESR.name(),-1,ic._instr->slot(1).uint64());
      ic._instr->read_operand(uarch.get_iss(),2);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsFoo.name(),-1,ic._instr->slot(2).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).FOO.name(),-1,ic._instr->slot(0).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index,ic._instr->slot(3).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index,ic._instr->slot(4).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsESR.name(),-1,ic._instr->slot(1).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsFoo.name(),-1,ic._instr->slot(2).uint64());
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.FOO.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
          return true;
        case 2: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(4)._index,"read")) return false;
          return true;
        case 3: if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 4: if (!uarch.dsFoo.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: ic._instr->read_operand(uarch.get_iss(),0);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).FOO.name(),-1,ic._instr->slot(0).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),3);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index,ic._instr->slot(3).uint64());
        break;
        case 2: ic._instr->read_operand(uarch.get_iss(),4);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index,ic._instr->slot(4).uint64());
        break;
        case 3: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsESR.name(),-1,ic._instr->slot(1).uint64());
        break;
        case 4: ic._instr->read_operand(uarch.get_iss(),2);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsFoo.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).FOO.name(),-1,ic._instr->slot(0).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index,ic._instr->slot(3).uint64());
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index,ic._instr->slot(4).uint64());
        break;
        case 3: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsESR.name(),-1,ic._instr->slot(1).uint64());
        break;
        case 4: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsFoo.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.FOO.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.FOO.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,0,mask))return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(3)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(4)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(4)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(4)._index,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).dsESR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.dsESR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsFoo.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsFoo.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,4,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        read(uarch,ic,logger,0);
        if (t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t0.read_partial(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        }
        if (t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t1.read_partial(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),0);
        }
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(3)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),1);
        }
        else {
          read(uarch,ic,logger,1);
        }
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(4)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(4)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),2);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),2);
        }
        else {
          read(uarch,ic,logger,2);
        }
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).dsESR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,3);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),3);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,3);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),3);
        }
        else {
          read(uarch,ic,logger,3);
        }
      }
      {
        read(uarch,ic,logger,4);
        if (t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t0.read_partial(uarch,ic,logger,*this,4);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),4);
        }
        if (t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t1.read_partial(uarch,ic,logger,*this,4);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),4);
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
        case 0: return static_cast<OSM_P&>(uarch).FOO.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 2: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(4)._index);
        case 3: return static_cast<OSM_P&>(uarch).dsESR.name(static_cast<OSM_P&>(uarch),0);
        case 4: return static_cast<OSM_P&>(uarch).dsFoo.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 2: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(4)._index);
        case 3: return static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0);
        case 4: return static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._masks[0];
        case 1: return 0x0;
        case 2: return 0x0;
        case 3: return 0x0;
        case 4: return ic._masks[1];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(3).uint32();
        case 2: return ic._instr->write_mask(4).uint32();
        case 3: return ic._instr->write_mask(1).uint32();
        case 4: return ic._instr->write_mask(2).uint32();
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
        case 2: ic._instr->write_mask(4) = tag;
        break;
        case 3: ic._instr->write_mask(1) = tag;
        break;
        case 4: ic._instr->write_mask(2) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 3;
        case 2: return 4;
        case 3: return 1;
        case 4: return 2;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 5; };
    
  };
  
  // Operand Alias:  stb_store_Src -> sth_store_Src
  //   (src: FOO[mask 0x00100000]/p)
  //   (src: GPR(RA))
  //   (src: GPR(RS))
  //   (src: dsESR?)
  //   (src: dsFoo[mask 0xff000000]/p?)
  typedef stb_store_Src sth_store_Src;
  
  // Operands:
  //   (src: FOO[mask 0x00100000]/p)
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  //   (src: dsESR?)
  //   (src: dsFoo[mask 0xff000000]/p?)
  struct sthx_store_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.FOO.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(4)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(5)._index,"read")) return false;
      if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.dsFoo.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      ic._instr->read_operand(uarch.get_iss(),0);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).FOO.name(),-1,ic._instr->slot(0).uint64());
      ic._instr->read_operand(uarch.get_iss(),3);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index,ic._instr->slot(3).uint64());
      ic._instr->read_operand(uarch.get_iss(),4);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index,ic._instr->slot(4).uint64());
      ic._instr->read_operand(uarch.get_iss(),5);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(5)._index,ic._instr->slot(5).uint64());
      ic._instr->read_operand(uarch.get_iss(),1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsESR.name(),-1,ic._instr->slot(1).uint64());
      ic._instr->read_operand(uarch.get_iss(),2);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsFoo.name(),-1,ic._instr->slot(2).uint64());
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).FOO.name(),-1,ic._instr->slot(0).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index,ic._instr->slot(3).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index,ic._instr->slot(4).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(5)._index,ic._instr->slot(5).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsESR.name(),-1,ic._instr->slot(1).uint64());
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsFoo.name(),-1,ic._instr->slot(2).uint64());
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.FOO.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
          return true;
        case 2: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(4)._index,"read")) return false;
          return true;
        case 3: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(5)._index,"read")) return false;
          return true;
        case 4: if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 5: if (!uarch.dsFoo.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: ic._instr->read_operand(uarch.get_iss(),0);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).FOO.name(),-1,ic._instr->slot(0).uint64());
        break;
        case 1: ic._instr->read_operand(uarch.get_iss(),3);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index,ic._instr->slot(3).uint64());
        break;
        case 2: ic._instr->read_operand(uarch.get_iss(),4);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index,ic._instr->slot(4).uint64());
        break;
        case 3: ic._instr->read_operand(uarch.get_iss(),5);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(5)._index,ic._instr->slot(5).uint64());
        break;
        case 4: ic._instr->read_operand(uarch.get_iss(),1);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsESR.name(),-1,ic._instr->slot(1).uint64());
        break;
        case 5: ic._instr->read_operand(uarch.get_iss(),2);
        if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsFoo.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).FOO.name(),-1,ic._instr->slot(0).uint64());
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index,ic._instr->slot(3).uint64());
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index,ic._instr->slot(4).uint64());
        break;
        case 3: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(5)._index,ic._instr->slot(5).uint64());
        break;
        case 4: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsESR.name(),-1,ic._instr->slot(1).uint64());
        break;
        case 5: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsFoo.name(),-1,ic._instr->slot(2).uint64());
        break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.FOO.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.FOO.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,0,mask))return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(3)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(4)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(4)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(4)._index,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(5)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(5)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(5)._index,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).dsESR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.dsESR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsFoo.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsFoo.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,5,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        read(uarch,ic,logger,0);
        if (t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t0.read_partial(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),0);
        }
        if (t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t1.read_partial(uarch,ic,logger,*this,0);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),0);
        }
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(3)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),1);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,1);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),1);
        }
        else {
          read(uarch,ic,logger,1);
        }
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(4)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(4)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),2);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,2);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),2);
        }
        else {
          read(uarch,ic,logger,2);
        }
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(5)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(5)._index));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,3);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),3);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,3);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),3);
        }
        else {
          read(uarch,ic,logger,3);
        }
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).dsESR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if (t0.can_read(uarch,ic,logger,id,tag)) {
          t0.read(uarch,ic,logger,*this,4);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),4);
        }
        else if (t1.can_read(uarch,ic,logger,id,tag)) {
          t1.read(uarch,ic,logger,*this,4);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),4);
        }
        else {
          read(uarch,ic,logger,4);
        }
      }
      {
        read(uarch,ic,logger,5);
        if (t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t0.read_partial(uarch,ic,logger,*this,5);
          if (logger) log_read(uarch,ic,logger,"read from "+t0.name(),5);
        }
        if (t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0)).first != 0) {
          t1.read_partial(uarch,ic,logger,*this,5);
          if (logger) log_read(uarch,ic,logger,"read from "+t1.name(),5);
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
        case 0: return static_cast<OSM_P&>(uarch).FOO.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 2: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(4)._index);
        case 3: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(5)._index);
        case 4: return static_cast<OSM_P&>(uarch).dsESR.name(static_cast<OSM_P&>(uarch),0);
        case 5: return static_cast<OSM_P&>(uarch).dsFoo.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).FOO.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 2: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(4)._index);
        case 3: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(5)._index);
        case 4: return static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0);
        case 5: return static_cast<OSM_P&>(uarch).dsFoo.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._masks[0];
        case 1: return 0x0;
        case 2: return 0x0;
        case 3: return 0x0;
        case 4: return 0x0;
        case 5: return ic._masks[1];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(3).uint32();
        case 2: return ic._instr->write_mask(4).uint32();
        case 3: return ic._instr->write_mask(5).uint32();
        case 4: return ic._instr->write_mask(1).uint32();
        case 5: return ic._instr->write_mask(2).uint32();
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
        case 2: ic._instr->write_mask(4) = tag;
        break;
        case 3: ic._instr->write_mask(5) = tag;
        break;
        case 4: ic._instr->write_mask(1) = tag;
        break;
        case 5: ic._instr->write_mask(2) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 3;
        case 2: return 4;
        case 3: return 5;
        case 4: return 1;
        case 5: return 2;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 6; };
    
  };
  
  // Operand Alias:  stb_store_Src -> stw_store_Src
  //   (src: FOO[mask 0x00100000]/p)
  //   (src: GPR(RA))
  //   (src: GPR(RS))
  //   (src: dsESR?)
  //   (src: dsFoo[mask 0xff000000]/p?)
  typedef stb_store_Src stw_store_Src;
  
  // Operand Alias:  stb_store_Src -> stwu_store_Src
  //   (src: FOO[mask 0x00100000]/p)
  //   (src: GPR(RA))
  //   (src: GPR(RS))
  //   (src: dsESR?)
  //   (src: dsFoo[mask 0xff000000]/p?)
  typedef stb_store_Src stwu_store_Src;
  
  // Operands:
  //   (trg: GPR(RA))
  struct stwu_store_TrgEA {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(3)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(3)._index);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(3)._index);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      ic._instr->commit(uarch.get_iss(),3);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index,ic._instr->slot(3).uint64());
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(3)._index;
    }
    
    bits<32> value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic._instr->slot(3);
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(3)._index); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(3).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(3) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 3; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operand Alias:  sthx_store_Src -> stwx_store_Src
  //   (src: FOO[mask 0x00100000]/p)
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  //   (src: dsESR?)
  //   (src: dsFoo[mask 0xff000000]/p?)
  typedef sthx_store_Src stwx_store_Src;
  
  //
  // Instruction class store
  //
  template <class Src_t, class TrgEA_t>
  struct Inst_store : public Inst_P {
    
    typedef struct Inst_store<Src_t, TrgEA_t> ThisClass;
    
    Inst_store &thisInstr() { return *this; };
    
    Src_t Src;
    TrgEA_t TrgEA;
    void reset(unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_P::reset(iid,cid,ic);
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    
    void deallocate_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      Src.deallocate(uarch,ic,logger);
      TrgEA.deallocate(uarch,ic,logger);
    }
    
    void write_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      Src.write(uarch,ic,logger);
      TrgEA.write(uarch,ic,logger);
    }
    
    bool exec_done() const { return _status == adl::MemAccess::opNone; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool move_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EX__S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EX__S_MM(uarch,logger,position);
    }
    
    bool change_state_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EX__S_MM_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_EX__S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      return action_S_EXe(uarch,logger,position);
    }
    
    bool move_S_EXe__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    static bool move_S_EXe__S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_EXe__S_MMe(uarch,logger,position);
    }
    
    bool change_state_S_EXe__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_EXe__S_MMe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_ID__S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_IDe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_IDe"); }
      _action = (StateFunc)action_S_IDe_p;
      _flush = (FlushFunc)flush_S_IDe_p;
      return action_S_IDe(uarch,logger,position);
    }
    
    bool change_state_S_IDe__S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDe -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      uarch.issue(_instr);
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool change_state_S_MM__S_MMp(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMp"); }
      _action = (StateFunc)action_S_MMp_p;
      _flush = (FlushFunc)flush_S_MMp_p;
      return action_S_MMp(uarch,logger,position);
    }
    
    bool change_state_S_MM__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      return action_S_MMe(uarch,logger,position);
    }
    
    bool move_S_MMe__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MMe__S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MMe__S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_MMe__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MMe__S_WBe_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_MMp__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMp -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      return action_S_MM(uarch,logger,position);
    }
    
    bool move_S_MMp__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMp -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    static bool move_S_MMp__S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_MMp__S_WB(uarch,logger,position);
    }
    
    bool change_state_S_MMp__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_MMp__S_WB_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool change_state_S_WB__S_WBp(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBp"); }
      _action = (StateFunc)action_S_WBp_p;
      _flush = (FlushFunc)flush_S_WBp_p;
      return action_S_WBp(uarch,logger,position);
    }
    
    bool change_state_S_WB__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool move_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WBe__S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WBe__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WBe__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    bool move_S_WBp__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBp -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
      uarch.commit(this);
      return true;
    }
    
    static bool move_S_WBp__S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->move_S_WBp__S_INIT(uarch,logger,position);
    }
    
    bool change_state_S_WBp__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      _move = (StateFunc)move_S_WBp__S_INIT_p;
      _action = (StateFunc)no_move_p;
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      try {
        if ( uarch.mMM.has_space(uarch,*this,logger, uarch.mEX.capacity() ) && uarch.Mem.can_request_write(uarch,*this,logger) ) {
              uarch.exec_mem(*this,logger) ;
              uarch.FwdEX.write(uarch,*this,logger, TrgEA ) ;
              uarch.Mem.send_write_request(uarch,*this,logger) ;
              return change_state_S_EX__S_MM(uarch,logger,position) ;
        }}
      catch (adliss::P::Exception) {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_EX(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mEX(*this,logger,position,true,uarch.get_pc());
        return change_state_S_EX__S_EXe(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_EXe__S_MMe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      try {
        if ( Src.can_read_fp(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) && TrgEA.can_write(uarch,*this,logger) && uarch.mEX.has_space(uarch,*this,logger) && uarch.hid_stall.can_allocate(uarch,*this,logger) ) {
              Src.read(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) ;
              TrgEA.allocate(uarch,*this,logger) ;
              return change_state_S_ID__S_EX(uarch,logger,position) ;
        }}
      catch (adliss::P::Exception) {
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
    static bool action_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_ID(uarch,logger,position);
    }
    
    // State action S_IDe
    bool action_S_IDe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mEX.has_space(uarch,*this,logger) ) {
            return change_state_S_IDe__S_EXe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_IDe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IDe(uarch,logger,position);
    }
    
    // State action S_INIT
    bool action_S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mID.has_space(uarch,*this,logger) && ! uarch.mID.has_attr(uarch,*this,logger, 0 , adliss::P::special_instrs | adliss::P::store_instrs | adliss::P::load_instrs | adliss::P::muldiv_instrs | adliss::P::serialized_instrs | adliss::P::branch_instrs ) ) {
            return change_state_S_INIT__S_ID(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_INIT(uarch,logger,position);
    }
    
    // State action S_MM
    bool action_S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      try {
        if ( uarch.Mem.can_write(uarch,*this,logger) ) {
              uarch.exec_and_write(*this,logger, uarch.Mem ) ;
              return change_state_S_MM__S_MMp(uarch,logger,position) ;
        }}
      catch (adliss::P::Exception) {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_MM(uarch,logger);
        uarch.commit_exceptions(*this,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mMM(*this,logger,position,true,uarch.get_pc());
        return change_state_S_MM__S_MMe(uarch,logger,position);
      }
      
      return false;
    }
    static bool action_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // State action S_MMe
    bool action_S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger, 2 ) ) {
            return change_state_S_MMe__S_WBe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MMe(uarch,logger,position);
    }
    
    // State action S_MMp
    bool action_S_MMp(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( exec_done (  ) && uarch.mWB.has_space(uarch,*this,logger, uarch.mMM.capacity() ) ) {
            return change_state_S_MMp__S_WB(uarch,logger,position) ;
      } else if ( uarch.Mem.can_request_write(uarch,*this,logger) ) {
            uarch.Mem.send_write_request(uarch,*this,logger) ;
            return change_state_S_MMp__S_MM(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MMp_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MMp(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      try {
        {
              write_ops(uarch,*this,logger) ;
              return change_state_S_WB__S_WBp(uarch,logger,position) ;
        }}
      catch (adliss::P::Exception) {
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
    static bool action_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WB(uarch,logger,position);
    }
    
    // State action S_WBe
    bool action_S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WBe__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WBe(uarch,logger,position);
    }
    
    // State action S_WBp
    bool action_S_WBp(OSM_P &uarch,Logger *logger,unsigned position) {
      {
            return change_state_S_WBp__S_INIT(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_WBp_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_WBp(uarch,logger,position);
    }
    
    // Flush action S_EX
    void flush_S_EX(OSM_P &uarch,Logger *logger) {
      TrgEA.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
      TrgEA.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_IDe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_P &uarch,Logger *logger) {
      TrgEA.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_MMe
    void flush_S_MMe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMe(uarch,logger);
    }
    
    // Flush action S_MMp
    void flush_S_MMp(OSM_P &uarch,Logger *logger) {
      TrgEA.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_MMp_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMp(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_P &uarch,Logger *logger) {
      TrgEA.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_P &uarch,Logger *logger) {
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBe(uarch,logger);
    }
    
    // Flush action S_WBp
    void flush_S_WBp(OSM_P &uarch,Logger *logger) {
      TrgEA.deallocate(uarch,*this,logger);
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    static void flush_S_WBp_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBp(uarch,logger);
    }
    
  };
  
  byte_t *Mem_mem_t::hasData(addr_t addr, unsigned &size) const {
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
  
  unsigned OSM_P::_ic_attrs[] = {
    0, // unknownClass
    0x1, // branchClass
    0x0, // decode_missClass
    0x0, // fetch_exceptionClass
    0x8, // isyncClass
    0x0, // lmwClass
    0x2, // loadClass
    0x20, // mtmsrClass
    0x20, // mtspr_hidClass
    0x4, // muldivClass
    0x0, // nopClass
    0x0, // rfiClass
    0x10, // sfxClass
    0x0, // stmwClass
    0x40, // storeClass
    0, // maxClass
  };
  
  unsigned OSM_P::getInstrClassAttrByName(const std::string &attr)
  {
    if (attr == "branch_instrs") return 0x1;
    if (attr == "load_instrs") return 0x2;
    if (attr == "muldiv_instrs") return 0x4;
    if (attr == "serialized_instrs") return 0x8;
    if (attr == "sfx_instrs") return 0x10;
    if (attr == "special_instrs") return 0x20;
    if (attr == "store_instrs") return 0x40;
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
      if (Mem_obj.getMemoryData(type, addr, size, data)) {
        // augment with instruction memory data
        
        if (Inst_P *instr = mWB[0]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_P *instr = mWB[1]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_P *instr = mMM[0]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_P *instr = mMM[1]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_P *instr = mEX[0]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_P *instr = mEX[1]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_P *instr = mID[0]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
        if (Inst_P *instr = mID[1]) {
          getMemoryData(data,instr->mem(),addr,size);
        }
      } else {
        Mem_obj.getMemoryData(type, addr, size, data);
      }
    } else {
      Mem_obj.getMemoryData(type, addr, size, data);
    }
  }
  
  static unsigned initFetchSizes[] = { 8, 4,  };
  FetchSizes OSM_P::_fetchSizes(&initFetchSizes[0],&initFetchSizes[2]);
  
  // Factory function for instruction "add".
  Inst_P *OSM_P::get_instr_add(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<add_sfx_Src, add_sfx_Trg> *ic = new Inst_sfx<add_sfx_Src, add_sfx_Trg>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "addi".
  Inst_P *OSM_P::get_instr_addi(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addi_sfx_Src, addi_sfx_Trg> *ic = new Inst_sfx<addi_sfx_Src, addi_sfx_Trg>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "addic".
  Inst_P *OSM_P::get_instr_addic(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addic_sfx_Src, addic_sfx_Trg> *ic = new Inst_sfx<addic_sfx_Src, addic_sfx_Trg>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "addic.".
  Inst_P *OSM_P::get_instr_addic_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addic__sfx_Src, addic__sfx_Trg> *ic = new Inst_sfx<addic__sfx_Src, addic__sfx_Trg>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    ic->_masks[1] = 0xf0000000;
    ic->_masks[2] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "addis".
  Inst_P *OSM_P::get_instr_addis(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addis_sfx_Src, addis_sfx_Trg> *ic = new Inst_sfx<addis_sfx_Src, addis_sfx_Trg>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "addme".
  Inst_P *OSM_P::get_instr_addme(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addme_sfx_Src, addme_sfx_Trg> *ic = new Inst_sfx<addme_sfx_Src, addme_sfx_Trg>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0x20000000;
    ic->_masks[0] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "addme.".
  Inst_P *OSM_P::get_instr_addme_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addme__sfx_Src, addme__sfx_Trg> *ic = new Inst_sfx<addme__sfx_Src, addme__sfx_Trg>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0xa0000000;
    ic->_masks[1] = 0xf0000000;
    ic->_masks[2] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "addmeo".
  Inst_P *OSM_P::get_instr_addmeo(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addmeo_sfx_Src, addmeo_sfx_Trg> *ic = new Inst_sfx<addmeo_sfx_Src, addmeo_sfx_Trg>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0xe0000000;
    ic->_masks[0] = 0xe0000000;
    return ic;
  }
  
  // Factory function for instruction "addo".
  Inst_P *OSM_P::get_instr_addo(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addo_sfx_Src, addo_sfx_Trg> *ic = new Inst_sfx<addo_sfx_Src, addo_sfx_Trg>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0xc0000000;
    ic->_masks[1] = 0xc0000000;
    return ic;
  }
  
  // Factory function for instruction "andi.".
  Inst_P *OSM_P::get_instr_andi_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<andi__sfx_Src, andi__sfx_Trg> *ic = new Inst_sfx<andi__sfx_Src, andi__sfx_Trg>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    ic->_masks[1] = 0xf0000000;
    return ic;
  }
  
  // Factory function for instruction "b".
  Inst_P *OSM_P::get_instr_b(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_branch<b_branch_Src, b_branch_Trg> *ic = new Inst_branch<b_branch_Src, b_branch_Trg>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::branchClass,p);
    return ic;
  }
  
  // Factory function for instruction "bc".
  Inst_P *OSM_P::get_instr_bc(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_branch<bc_branch_Src, bc_branch_Trg> *ic = new Inst_branch<bc_branch_Src, bc_branch_Trg>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::branchClass,p);
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
    Inst_branch<bclr_branch_Src, bclr_branch_Trg> *ic = new Inst_branch<bclr_branch_Src, bclr_branch_Trg>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::branchClass,p);
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
    Inst_branch<bl_branch_Src, bl_branch_Trg> *ic = new Inst_branch<bl_branch_Src, bl_branch_Trg>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::branchClass,p);
    return ic;
  }
  
  // Factory function for instruction "cmp".
  Inst_P *OSM_P::get_instr_cmp(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<cmp_sfx_Src, cmp_sfx_Trg> *ic = new Inst_sfx<cmp_sfx_Src, cmp_sfx_Trg>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "cmpi".
  Inst_P *OSM_P::get_instr_cmpi(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<cmpi_sfx_Src, cmpi_sfx_Trg> *ic = new Inst_sfx<cmpi_sfx_Src, cmpi_sfx_Trg>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "dcbtep".
  Inst_P *OSM_P::get_instr_dcbtep(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_dcbtep' is not implemented in this model.");
  }
  
  // Factory function for instruction "dsn".
  Inst_P *OSM_P::get_instr_dsn(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_dsn' is not implemented in this model.");
  }
  
  // Factory function for instruction "evstdd".
  Inst_P *OSM_P::get_instr_evstdd(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_evstdd' is not implemented in this model.");
  }
  
  // Factory function for instruction "halt".
  Inst_P *OSM_P::get_instr_halt(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_isync *ic = new Inst_isync;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::isyncClass,p);
    return ic;
  }
  
  // Factory function for instruction "isync".
  Inst_P *OSM_P::get_instr_isync(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_isync *ic = new Inst_isync;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::isyncClass,p);
    return ic;
  }
  
  // Factory function for instruction "lbz".
  Inst_P *OSM_P::get_instr_lbz(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lbz_load_Src, lbz_load_Trg, DummyOp> *ic = new Inst_load<lbz_load_Src, lbz_load_Trg, DummyOp>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::loadClass,p);
    ic->_masks[0] = 0x100000;
    ic->_masks[1] = 0xff000000;
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
    Inst_load<lhz_load_Src, lhz_load_Trg, DummyOp> *ic = new Inst_load<lhz_load_Src, lhz_load_Trg, DummyOp>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::loadClass,p);
    ic->_masks[0] = 0x100000;
    ic->_masks[1] = 0xff000000;
    return ic;
  }
  
  // Factory function for instruction "lmw".
  Inst_P *OSM_P::get_instr_lmw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_lmw<lmw_lmw_Src, lmw_lmw_Trg> *ic = new Inst_lmw<lmw_lmw_Src, lmw_lmw_Trg>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::lmwClass,p);
    ic->_masks[0] = 0x100000;
    ic->_masks[1] = 0xff000000;
    return ic;
  }
  
  // Factory function for instruction "lvx".
  Inst_P *OSM_P::get_instr_lvx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_lvx' is not implemented in this model.");
  }
  
  // Factory function for instruction "lwarx".
  Inst_P *OSM_P::get_instr_lwarx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_lwarx' is not implemented in this model.");
  }
  
  // Factory function for instruction "lwdx".
  Inst_P *OSM_P::get_instr_lwdx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_lwdx' is not implemented in this model.");
  }
  
  // Factory function for instruction "lwepx".
  Inst_P *OSM_P::get_instr_lwepx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_lwepx' is not implemented in this model.");
  }
  
  // Factory function for instruction "lwz".
  Inst_P *OSM_P::get_instr_lwz(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lwz_load_Src, lwz_load_Trg, DummyOp> *ic = new Inst_load<lwz_load_Src, lwz_load_Trg, DummyOp>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::loadClass,p);
    ic->_masks[0] = 0x100000;
    ic->_masks[1] = 0xff000000;
    return ic;
  }
  
  // Factory function for instruction "lwzu".
  Inst_P *OSM_P::get_instr_lwzu(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lwzu_load_Src, lwzu_load_Trg, lwzu_load_TrgEA> *ic = new Inst_load<lwzu_load_Src, lwzu_load_Trg, lwzu_load_TrgEA>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::loadClass,p);
    ic->_masks[0] = 0x100000;
    ic->_masks[1] = 0xff000000;
    return ic;
  }
  
  // Factory function for instruction "lwzx".
  Inst_P *OSM_P::get_instr_lwzx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lwzx_load_Src, lwzx_load_Trg, DummyOp> *ic = new Inst_load<lwzx_load_Src, lwzx_load_Trg, DummyOp>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::loadClass,p);
    ic->_masks[0] = 0x100000;
    ic->_masks[1] = 0xff000000;
    return ic;
  }
  
  // Factory function for instruction "mfmsr".
  Inst_P *OSM_P::get_instr_mfmsr(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_mfmsr' is not implemented in this model.");
  }
  
  // Factory function for instruction "mfspr".
  Inst_P *OSM_P::get_instr_mfspr(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<mfspr_sfx_Src, mfspr_sfx_Trg> *ic = new Inst_sfx<mfspr_sfx_Src, mfspr_sfx_Trg>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "mtmsr".
  Inst_P *OSM_P::get_instr_mtmsr(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_mtmsr<mtmsr_mtmsr_Src, mtmsr_mtmsr_Trg> *ic = new Inst_mtmsr<mtmsr_mtmsr_Src, mtmsr_mtmsr_Trg>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::mtmsrClass,p);
    return ic;
  }
  
  // Factory function for instruction "mtspr".
  Inst_P *OSM_P::get_instr_mtspr(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    if ((_iss->get_SPRN(p->_opc[0]) == 51)) {
      Inst_mtspr_hid<mtspr_mtspr_hid_Src, mtspr_mtspr_hid_Trg> *ic = new Inst_mtspr_hid<mtspr_mtspr_hid_Src, mtspr_mtspr_hid_Trg>;
      _instr_alloc_count++;
      ic->reset(_issueId++,_commits,adliss::P::mtspr_hidClass,p);
      return ic;
    }
    if ((_iss->get_SPRN(p->_opc[0]) == 50)) {
      Inst_mtspr_hid<mtspr_mtspr_hid_Src, mtspr_mtspr_hid_Trg> *ic = new Inst_mtspr_hid<mtspr_mtspr_hid_Src, mtspr_mtspr_hid_Trg>;
      _instr_alloc_count++;
      ic->reset(_issueId++,_commits,adliss::P::mtspr_hidClass,p);
      ic->_masks[0] = 0x400000;
      return ic;
    }
    Inst_sfx<mtspr_sfx_Src, mtspr_sfx_Trg> *ic = new Inst_sfx<mtspr_sfx_Src, mtspr_sfx_Trg>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x400000;
    return ic;
  }
  
  // Factory function for instruction "mulli".
  Inst_P *OSM_P::get_instr_mulli(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_muldiv<mulli_muldiv_Src, mulli_muldiv_Trg> *ic = new Inst_muldiv<mulli_muldiv_Src, mulli_muldiv_Trg>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::muldivClass,p);
    return ic;
  }
  
  // Factory function for instruction "mullw".
  Inst_P *OSM_P::get_instr_mullw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_muldiv<mullw_muldiv_Src, mullw_muldiv_Trg> *ic = new Inst_muldiv<mullw_muldiv_Src, mullw_muldiv_Trg>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::muldivClass,p);
    return ic;
  }
  
  // Factory function for instruction "or".
  Inst_P *OSM_P::get_instr_or(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<or_sfx_Src, or_sfx_Trg> *ic = new Inst_sfx<or_sfx_Src, or_sfx_Trg>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "ori".
  Inst_P *OSM_P::get_instr_ori(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    if ((_iss->get_RA(p->_opc[0]) == 0) && (_iss->get_RS(p->_opc[0]) == 0) && (_iss->get_UI(p->_opc[0]) == 0)) {
      Inst_nop<ori_nop___other__> *ic = new Inst_nop<ori_nop___other__>;
      _instr_alloc_count++;
      ic->reset(_issueId++,_commits,adliss::P::nopClass,p);
      return ic;
    }
    Inst_sfx<ori_sfx_Src, ori_sfx_Trg> *ic = new Inst_sfx<ori_sfx_Src, ori_sfx_Trg>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "oris".
  Inst_P *OSM_P::get_instr_oris(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_oris' is not implemented in this model.");
  }
  
  // Factory function for instruction "rfci".
  Inst_P *OSM_P::get_instr_rfci(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_rfci' is not implemented in this model.");
  }
  
  // Factory function for instruction "rfi".
  Inst_P *OSM_P::get_instr_rfi(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_rfi<rfi_rfi_Src, rfi_rfi_Trg> *ic = new Inst_rfi<rfi_rfi_Src, rfi_rfi_Trg>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::rfiClass,p);
    return ic;
  }
  
  // Factory function for instruction "rlwinm".
  Inst_P *OSM_P::get_instr_rlwinm(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<rlwinm_sfx_Src, rlwinm_sfx_Trg> *ic = new Inst_sfx<rlwinm_sfx_Src, rlwinm_sfx_Trg>;
    _instr_alloc_count++;
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
    Inst_sfx<srw_sfx_Src, srw_sfx_Trg> *ic = new Inst_sfx<srw_sfx_Src, srw_sfx_Trg>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x1f;
    return ic;
  }
  
  // Factory function for instruction "stb".
  Inst_P *OSM_P::get_instr_stb(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<stb_store_Src, DummyOp> *ic = new Inst_store<stb_store_Src, DummyOp>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::storeClass,p);
    ic->_masks[0] = 0x100000;
    ic->_masks[1] = 0xff000000;
    return ic;
  }
  
  // Factory function for instruction "sth".
  Inst_P *OSM_P::get_instr_sth(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<sth_store_Src, DummyOp> *ic = new Inst_store<sth_store_Src, DummyOp>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::storeClass,p);
    ic->_masks[0] = 0x100000;
    ic->_masks[1] = 0xff000000;
    return ic;
  }
  
  // Factory function for instruction "sthx".
  Inst_P *OSM_P::get_instr_sthx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<sthx_store_Src, DummyOp> *ic = new Inst_store<sthx_store_Src, DummyOp>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::storeClass,p);
    ic->_masks[0] = 0x100000;
    ic->_masks[1] = 0xff000000;
    return ic;
  }
  
  // Factory function for instruction "stmw".
  Inst_P *OSM_P::get_instr_stmw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_stmw<stmw_stmw_Src, stmw_stmw_SrcCR, DummyOp> *ic = new Inst_stmw<stmw_stmw_Src, stmw_stmw_SrcCR, DummyOp>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::stmwClass,p);
    ic->_masks[0] = 0x100000;
    ic->_masks[1] = 0xffffffff;
    ic->_masks[2] = 0xffffffff;
    ic->_masks[3] = 0xff000000;
    ic->_masks[4] = 0xffffffff;
    return ic;
  }
  
  // Factory function for instruction "stvx".
  Inst_P *OSM_P::get_instr_stvx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_stvx' is not implemented in this model.");
  }
  
  // Factory function for instruction "stw".
  Inst_P *OSM_P::get_instr_stw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<stw_store_Src, DummyOp> *ic = new Inst_store<stw_store_Src, DummyOp>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::storeClass,p);
    ic->_masks[0] = 0x100000;
    ic->_masks[1] = 0xff000000;
    return ic;
  }
  
  // Factory function for instruction "stwdx".
  Inst_P *OSM_P::get_instr_stwdx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_stwdx' is not implemented in this model.");
  }
  
  // Factory function for instruction "stwu".
  Inst_P *OSM_P::get_instr_stwu(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<stwu_store_Src, stwu_store_TrgEA> *ic = new Inst_store<stwu_store_Src, stwu_store_TrgEA>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::storeClass,p);
    ic->_masks[0] = 0x100000;
    ic->_masks[1] = 0xff000000;
    return ic;
  }
  
  // Factory function for instruction "stwx".
  Inst_P *OSM_P::get_instr_stwx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<stwx_store_Src, DummyOp> *ic = new Inst_store<stwx_store_Src, DummyOp>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::storeClass,p);
    ic->_masks[0] = 0x100000;
    ic->_masks[1] = 0xff000000;
    return ic;
  }
  
  // Factory function for instruction "tlbre".
  Inst_P *OSM_P::get_instr_tlbre(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_tlbre' is not implemented in this model.");
  }
  
  // Factory function for instruction "tlbsx".
  Inst_P *OSM_P::get_instr_tlbsx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_tlbsx' is not implemented in this model.");
  }
  
  // Factory function for instruction "tlbwe".
  Inst_P *OSM_P::get_instr_tlbwe(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_tlbwe' is not implemented in this model.");
  }
  
  // Factory function for instruction "tw".
  Inst_P *OSM_P::get_instr_tw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<tw_sfx_Src, DummyOp> *ic = new Inst_sfx<tw_sfx_Src, DummyOp>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "wait".
  Inst_P *OSM_P::get_instr_wait(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_wait' is not implemented in this model.");
  }
  
  // Factory function for instruction "wrteei".
  Inst_P *OSM_P::get_instr_wrteei(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_wrteei' is not implemented in this model.");
  }
  
  // Factory function for instruction "decode_miss".
  Inst_P *OSM_P::get_instr_decode_miss(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_decode_miss<decode_miss_decode_miss_Src, DummyOp> *ic = new Inst_decode_miss<decode_miss_decode_miss_Src, DummyOp>;
    _instr_alloc_count++;
    ic->reset(_issueId++,_commits,adliss::P::decode_missClass,p);
    return ic;
  }
  
  // Factory function for instruction "fetch_exception".
  Inst_P *OSM_P::get_instr_fetch_exception(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_fetch_exception *ic = new Inst_fetch_exception;
    _instr_alloc_count++;
    ic->reset(_issueId,_commits,adliss::P::fetch_exceptionClass,p);
    return ic;
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
    if (logger) logger->logMessage("commiting exception");
    if (instr.is_fetch_exception()) {
      commit_fetch_exception(instr);
    } else {
      _iss->CIA_write(instr._instr->addr());
      set_pc(instr._instr->addr() + instr._instr->size());
      instr._instr->commit_exceptions(*_iss);
    }
  };
  
  void OSM_P::cancel_all_mem(Inst_P &instr)
  {
    UadlMemAccess *ma = instr.mem();
    while (ma) {
      if (ma->valid_request_id()) {
        Mem_obj.cancel(ma->request_id(),true);
      }
      ma = ma->_next;
    }
  }
  
  void OSM_P::commit_fetch_exception(Inst_P&instr)
  {
    FetchExceptionData &fedata = *instr._fedata;
    _iss->CIA_write(fedata._addr);
    _iss->log_current_instr(&fedata);
    _iss->genExceptionsInternal(fedata._exdata,fedata._exitems);
    _fetchExceptionInProgress = false;
  }
  
  void OSM_P::exec_mEX(Inst_P&instr,Logger *logger,unsigned position)
  {
    exec(instr,logger);
    if (_iss->activeCount() == 0) flush_mEX(instr,logger,position);
  }
  
  void OSM_P::exec_mem_mEX(Inst_P&instr,Logger *logger,unsigned position)
  {
    exec_mem(instr,logger);
    if (_iss->activeCount() == 0) flush_mEX(instr,logger,position);
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
  
  void OSM_P::exec_mMM(Inst_P&instr,Logger *logger,unsigned position)
  {
    exec(instr,logger);
    if (_iss->activeCount() == 0) flush_mMM(instr,logger,position);
  }
  
  void OSM_P::exec_mem_mMM(Inst_P&instr,Logger *logger,unsigned position)
  {
    exec_mem(instr,logger);
    if (_iss->activeCount() == 0) flush_mMM(instr,logger,position);
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
    for (int i = position+1; i < 2; ++i) {
      if (Inst_P *instr = mWB.remove_instr(i)) {
        instr->flush(*this,logger);
        discard(instr);
      }
    }
    mWB.update_tail();
    for (unsigned i = 0; i != 2; ++i) {
      if (Inst_P *instr = mMM[i]) {
        (instr->_flush)(instr,*this,logger);
        discard(instr);
      }
    }
    mMM.reset();
    for (unsigned i = 0; i != 2; ++i) {
      if (Inst_P *instr = mEX[i]) {
        (instr->_flush)(instr,*this,logger);
        discard(instr);
      }
    }
    mEX.reset();
    for (unsigned i = 0; i != 2; ++i) {
      if (Inst_P *instr = mID[i]) {
        (instr->_flush)(instr,*this,logger);
        Fetcher.removeInstr(instr->size());
        discard(instr);
      }
    }
    mID.reset();
    flush_issue_stage(logger,position,given_addr,addr);
  }
  
  void OSM_P::flush_mMM(Inst_P&ic,Logger *logger,unsigned position)
  {
    if (ic.iid() >= _resetFetchId) {
      set_pc(ic._instr->addr() + ic._instr->size());
    }
    taken_flush_mMM(ic,logger,position);
  }
  
  void OSM_P::taken_flush_mMM(Inst_P&ic,Logger *logger,unsigned position,bool given_addr,addr_t addr)
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
    for (int i = position+1; i < 2; ++i) {
      if (Inst_P *instr = mMM.remove_instr(i)) {
        instr->flush(*this,logger);
        discard(instr);
      }
    }
    mMM.update_tail();
    for (unsigned i = 0; i != 2; ++i) {
      if (Inst_P *instr = mEX[i]) {
        (instr->_flush)(instr,*this,logger);
        discard(instr);
      }
    }
    mEX.reset();
    for (unsigned i = 0; i != 2; ++i) {
      if (Inst_P *instr = mID[i]) {
        (instr->_flush)(instr,*this,logger);
        Fetcher.removeInstr(instr->size());
        discard(instr);
      }
    }
    mID.reset();
    flush_issue_stage(logger,position,given_addr,addr);
  }
  
  void OSM_P::flush_mEX(Inst_P&ic,Logger *logger,unsigned position)
  {
    if (ic.iid() >= _resetFetchId) {
      set_pc(ic._instr->addr() + ic._instr->size());
    }
    taken_flush_mEX(ic,logger,position);
  }
  
  void OSM_P::taken_flush_mEX(Inst_P&ic,Logger *logger,unsigned position,bool given_addr,addr_t addr)
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
    for (int i = position+1; i < 2; ++i) {
      if (Inst_P *instr = mEX.remove_instr(i)) {
        instr->flush(*this,logger);
        discard(instr);
      }
    }
    mEX.update_tail();
    for (unsigned i = 0; i != 2; ++i) {
      if (Inst_P *instr = mID[i]) {
        (instr->_flush)(instr,*this,logger);
        Fetcher.removeInstr(instr->size());
        discard(instr);
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
    position = 0;
    for (int i = position+1; i < 2; ++i) {
      if (Inst_P *instr = mID.remove_instr(i)) {
        instr->flush(*this,logger);
        Fetcher.removeInstr(instr->size());
        discard(instr);
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
    _fetchExceptionInProgress = false;
  }
  
  bool OSM_P::hasExternException()
  {
    return _iss->hasCurrentPendingExceptions();
  }
  
  bool OSM_P::hasPendingExternException()
  {
    return _iss->hasPendingExceptions();
  }
  
  void OSM_P::genExceptions(unsigned flag)
  {
    _iss->setPendingExceptions(flag);
    _iss->setExternException(flag);
  }
  
  void OSM_P::cancelExceptions(unsigned flag)
  {
    _iss->cancelExceptions(flag);
  }
  
  void OSM_P::raiseExternException()
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
      if ((instr = _instr_cache.find(fd._ra))) {
        if ((fd._size < instr->size()) || !_iss->quick_decode(fd._ea,fd._data,0)) return 0;
        instr->reset(_issueId++,_commits);
        instr->_instr->quick_reset(*_iss);
        _instr_alloc_count++;
      } else {
        instr = _iss->decode(fd._ea, fd._data, 0, fd._size, *this);
      }
      Fetcher.decodeUpdate(instr, instr ? instr->size() : 0);
      if (!instr) return 0;
      instr->_ra = fd._ra;
      instr->set_prediction_data(Fetcher.predTaken(),Fetcher.predTrg());
    } else if (!Fetcher.empty()) {
      return 0;
    }
    else if (hasFetchException()) {
      _fetchExceptionInProgress = true;
      Inst_P *fe = get_instr_fetch_exception(0);
      fe->set_fetch_exception(_fetchException,_fetchExceptionHandle,Fetcher.nextFetchAddr(),_fetchLogData);
      _fetchLogData = 0;
      _fetchException = 0;
      return fe;
    }
    
    return instr;
  }
  
  void OSM_P::issue(InstrPacket *instr)
  {
    _iss->log_instr_issue_time(instr,currTime());
    Fetcher.removeInstr(instr->size());
  }
  
  void OSM_P::commit(Inst_P*instr)
  {
    addr_t ia = instr->addr();
    _iss->free_packet(instr->_instr);
    free_instr(instr);
    incrCommitCount();
    check_breakpoints(ia);
  }
  
  void OSM_P::commit_checked(Inst_P*instr)
  {
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
  }
  
  void OSM_P::discard(Inst_P*instr)
  {
    cancel_all_mem(*instr);
    free_all_mem(*instr);
    _iss->discard_packet(instr->_instr);
    free_instr(instr);
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
    try {
      _iss->prefetch_translate(ra, ea);
      return true;
    }
    catch (adliss::P::Exception e) {
      return false;
    }
  }
  
  bool OSM_P::fetchTranslate(addr_t ea, addr_t &ra, unsigned frontPadSize, unsigned size,byte_t *data)
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
            prefetchStatus = _iss->prefetch<8>(ea);
          } while (prefetchStatus == adl::prSkip);
          
          break;
          
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
    catch (adliss::P::Exception e) {
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
  
  
  bool OSM_P::fetchCacheEnabled(addr_t addr)
  {
    return false;
  }
  
  bool OSM_P::issueInstructions()
  {
    if (!Issue.empty() && Issue.instr()->action(*this,((Tracing && logPipeline()) ? Issue.logger(Fetcher.predTaken(),Fetcher.predTrg()): 0),0)) {
      Issue.remove_instr();
    }
    if (!hasExternException()) {
      while (_iss->is_active() && !fetchExceptionInProgress() && issueInquire()) {
        if ((Issue.add_instr(issueInstruction()))) {
          if (Issue.instr()->action(*this,((Tracing && logPipeline()) ? Issue.logger(Fetcher.predTaken(),Fetcher.predTrg()): 0),0)) {;
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
      Mem_obj.createLogger(_logclient,tflags);
    } else {
      Mem_obj.removeLogger();
    }
  }
  
  void OSM_P::report_and_clear(ostream &o,const string &pfx)
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
  
  void OSM_P::report_and_clear_top(ostream &o,const string &pfx)
  {
    if (_iss->hasPendingExceptions()) {
      *_out << pfx << "[external exception]\n";
    }
    Fetcher.report_and_clear(o,pfx);
    Issue.report_and_clear(o,pfx);
  }
  
  void OSM_P::report_and_clear_bottom(ostream &o,const string &pfx)
  {
    Mem_obj.report_and_clear(o,pfx);
    *_out << '\n';
  }
  
  adl::IssNode& OSM_P::iss() { return *_iss; }
  const adl::IssNode& OSM_P::iss() const { return *_iss; }
  ISS_P& OSM_P::get_iss() { return *_iss; }
  
  void OSM_P::preCycle()
  {
    Mem_obj.preCycle();
    _iss->updateLevelSensitiveExceptions();
    Fetcher.receiveInstrData();
    if (logPipeline()) {
      Fetcher.instrBufReport();
    }
  }
  
  void OSM_P::postCycle()
  {
    if (_iss->activeCount()) Fetcher.sendFetchRequests();
    Mem_obj.postCycle();
    FwdEX.postCycle();
    FwdMM.postCycle();
    if (logMemory()) Mem_obj.logAllRequests();
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
        bool r0 = (!mWB[0] || mWB[0]->action(*this,logger0,0));
        Logger *logger1 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(0+mWB.pos(1)) : 0);
        bool r1 = (!mWB[1] || mWB[1]->action(*this,logger1,1));
        if (r0 && r1) {
          if (mWB.head()) mWB.head()->move(*this,logger0,0);
          if (mWB.head()) mWB.head()->move(*this,logger1,1);
        } else {
          _stall = true;
        }
      }
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(2+mMM.pos(0)) : 0);
        bool r0 = (!mMM[0] || mMM[0]->action(*this,logger0,0));
        Logger *logger1 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(2+mMM.pos(1)) : 0);
        bool r1 = (!mMM[1] || mMM[1]->action(*this,logger1,1));
        if (r0 && r1) {
          if (mMM.head()) mMM.head()->move(*this,logger0,0);
          if (mMM.head()) mMM.head()->move(*this,logger1,1);
        } else {
          _stall = true;
        }
      }
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(4+mEX.pos(0)) : 0);
        bool r0 = (!mEX[0] || mEX[0]->action(*this,logger0,0));
        Logger *logger1 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(4+mEX.pos(1)) : 0);
        bool r1 = (!mEX[1] || mEX[1]->action(*this,logger1,1));
        if (r0 && r1) {
          if (mEX.head()) mEX.head()->move(*this,logger0,0);
          if (mEX.head()) mEX.head()->move(*this,logger1,1);
        } else {
          _stall = true;
        }
      }
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(6+mID.pos(0)) : 0);
        Logger *logger1 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(6+mID.pos(1)) : 0);
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
  BP(*this,0,BPredEnableDefault(),8,
  2), 
  mEX(getPath(_iss,".") + "mEX"), 
  mID(getPath(_iss,".") + "mID"), 
  mMM(getPath(_iss,".") + "mMM"), 
  mWB(getPath(_iss,".") + "mWB"), 
  Mem_obj(*this, logWarnings()), 
  Mem(Mem_obj), 
  _log_mem(true), 
  CIA("CIA"), 
  CR("CR"), 
  CSRR0("CSRR0"), 
  CSRR1("CSRR1"), 
  CTR("CTR"), 
  DAC("DAC"), 
  DCTRL("DCTRL"), 
  DSR("DSR"), 
  Decoration("Decoration"), 
  ELPC("ELPC"), 
  ESR("ESR"), 
  FOO("FOO"), 
  GPR("GPR"), 
  HDBCR0("HDBCR0"), 
  HID0("HID0"), 
  HID1("HID1"), 
  HitReg("HitReg"), 
  IVOR15("IVOR15"), 
  IVOR4("IVOR4"), 
  IVOR5("IVOR5"), 
  IVOR6("IVOR6"), 
  IVOR7("IVOR7"), 
  IVOR8("IVOR8"), 
  IVPR("IVPR"), 
  LR("LR"), 
  MAS0("MAS0"), 
  MAS1("MAS1"), 
  MAS2("MAS2"), 
  MSR("MSR"), 
  NIA("NIA"), 
  PFAULT("PFAULT"), 
  PID0("PID0"), 
  PID1("PID1"), 
  ProgFault("ProgFault"), 
  RESRV("RESRV"), 
  SRR0("SRR0"), 
  SRR1("SRR1"), 
  WatchDogTrigger("WatchDogTrigger"), 
  XER("XER"), 
  dsESR("dsESR"), 
  dsFoo("dsFoo"), 
  all_stall("all_stall",1), 
  hid_stall("hid_stall"), 
  FwdEX("FwdEX"), 
  FwdMM("FwdMM"), 
  Fetcher(*this,getPath(_iss,".")+"Fetcher",22,Mem_obj,0,_fetchSizes,&Mem_obj), 
  _cur_instr(0), 
  _ignore_mem(false), 
  Issue("         "), 
  _instr_alloc_count(0), 
  _fetchException(0), 
  _fetchExceptionInProgress(false), 
  _fetchLogData(0), 
  _stall(false), 
  _stage_loggers(8), 
  top(*this)
  {
    alloc_mem_access_data();
    _iss->_uarch = this;
    Mem_obj.set_level(0);
    Mem_obj.set_level(0);
  };
  
  void OSM_P::reset()
  {
    UadlArch::reset();
    _breakpoint = false;
    BP.reset();
    mEX.reset();
    mID.reset();
    mMM.reset();
    mWB.reset();
    Mem_obj.reset();
    _mem_access_pool.reset();
    _log_mem = true;
    CIA.reset();
    CR.reset();
    CSRR0.reset();
    CSRR1.reset();
    CTR.reset();
    DAC.reset();
    DCTRL.reset();
    DSR.reset();
    Decoration.reset();
    ELPC.reset();
    ESR.reset();
    FOO.reset();
    GPR.reset();
    HDBCR0.reset();
    HID0.reset();
    HID1.reset();
    HitReg.reset();
    IVOR15.reset();
    IVOR4.reset();
    IVOR5.reset();
    IVOR6.reset();
    IVOR7.reset();
    IVOR8.reset();
    IVPR.reset();
    LR.reset();
    MAS0.reset();
    MAS1.reset();
    MAS2.reset();
    MSR.reset();
    NIA.reset();
    PFAULT.reset();
    PID0.reset();
    PID1.reset();
    ProgFault.reset();
    RESRV.reset();
    SRR0.reset();
    SRR1.reset();
    WatchDogTrigger.reset();
    XER.reset();
    dsESR.reset();
    dsFoo.reset();
    all_stall.reset();
    hid_stall.reset();
    FwdEX.reset();
    FwdMM.reset();
    Fetcher.full_reset();
    _cur_instr = 0;
    _ignore_mem = false;
    Issue.reset();
    _instr_cache.reset();
    _instr_alloc_count = 0;
    _fetchException = 0;
    _fetchExceptionInProgress = false;
    _fetchLogData = 0;
    _stall = false;
  }
  
  bool OSM_P::active() const
  {
    if (breakpoint()) return false;
    return (_iss->activeCount() || instr_alloc_count() || Mem_obj.isActive());
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
  
  unsigned OSM_P::dmemStalls() const { return Mem_obj.memStalls(0); }
  
  unsigned OSM_P::imemStalls() const { return Mem_obj.memStalls(0); }
  
  UadlArch *createArch(const string &name, unsigned &id,UadlMemoryIf *memory = NULL,ttime_t startTime = 0)
  {
    if (memory) {
      cerr << "Warning:  An external memory was specified but the model was created to use internal memory.\n";}
    
    unsigned thread_id = 0;
    return new OSM_P(id,thread_id,std::cout,startTime);
  }
  
  
  
} // namespace uadl

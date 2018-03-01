//
// ADL/Uadl version 3.17.2
//
// Cycle-accurate/approximate model generated for P
//

#ifdef UADL_SEPARATE_COMPILE
#  define ADL_HEADER
#endif
#include "dlx-zl2-safe-iss.cc"
#define _OSM_P_IMPLEMENTATION_H
#include "dlx-zl2-safe.h"

using namespace std;
using namespace adl;
using namespace adliss;




void adliss::P::synchronizeActivation()
{
  _uarch->force_reset_fetch();
}

void adliss::P::handle_memory_request(unsigned memId, const adl::MemAccess &ma) {
  _uarch->handle_memory_request(memId,ma);
};

void adliss::P::enable_mem_logging(bool e) {
  _uarch->_log_mem = e;
};

void adliss::P::branch_target_update(addr_t target,unsigned size)
{
  if (_uarch->is_executing(*_uarch->_cur_instr)) {
    _uarch->set_branch_target(target);
    _uarch->_speculative = true;
    _uarch->_spec_tag = _uarch->_cur_instr->iid();
  }
}

void adliss::P::branch_handler()
{
  if (_uarch->is_executing(*_uarch->_cur_instr)) {
  }
}

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
  //   (src: CTR?, trg: CTR?)
  //   (trg: NIA?)
  struct bc_branch___other__ {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CIA.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CIA.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CR.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CTR.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CIA.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CR.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CTR.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.CIA.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 2: if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 3: if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CIA.name(),-1);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CR.name(),-1);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CTR.name(),-1);
        break;
        case 3: break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CIA.name(),-1);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CR.name(),-1);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CTR.name(),-1);
        break;
        case 3: break;
      }
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
      if (logger && ic._instr->is_dirty(2))logger->logOpAction("write",static_cast<OSM_P&>(uarch).CTR.name(),-1);
      if (logger && ic._instr->is_dirty(3))logger->logOpAction("write",static_cast<OSM_P&>(uarch).NIA.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CIA.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).CR.name(static_cast<OSM_P&>(uarch),0);
        case 2: return static_cast<OSM_P&>(uarch).CTR.name(static_cast<OSM_P&>(uarch),0);
        case 3: return static_cast<OSM_P&>(uarch).NIA.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CIA.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        case 2: return static_cast<OSM_P&>(uarch).CTR.id(static_cast<OSM_P&>(uarch),0);
        case 3: return static_cast<OSM_P&>(uarch).NIA.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
        case 2: return 0x0;
        case 3: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
        case 2: return ic._instr->write_mask(2).uint32();
        case 3: return ic._instr->write_mask(3).uint32();
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
        case 3: ic._instr->write_mask(3) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 1;
        case 2: return 2;
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
  //   (src: CIA)
  //   (trg: NIA)
  struct b_branch___other__ {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CIA.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CIA.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CIA.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.CIA.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CIA.name(),-1);
        break;
        case 1: break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CIA.name(),-1);
        break;
        case 1: break;
      }
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
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).NIA.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CIA.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).NIA.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CIA.id(static_cast<OSM_P&>(uarch),0);
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
  
  // Operands:
  //   (src: CIA)
  //   (trg: LR)
  //   (trg: NIA)
  struct bl_branch___other__ {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CIA.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.LR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CIA.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CIA.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.CIA.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.LR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 2: if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CIA.name(),-1);
        break;
        case 1: break;
        case 2: break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CIA.name(),-1);
        break;
        case 1: break;
        case 2: break;
      }
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
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).LR.name(),-1);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).NIA.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CIA.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).LR.name(static_cast<OSM_P&>(uarch),0);
        case 2: return static_cast<OSM_P&>(uarch).NIA.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CIA.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).LR.id(static_cast<OSM_P&>(uarch),0);
        case 2: return static_cast<OSM_P&>(uarch).NIA.id(static_cast<OSM_P&>(uarch),0);
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
  //   (src: CIA)
  //   (src: CR/p)
  //   (src: CTR?, trg: CTR?)
  //   (trg: LR)
  //   (trg: NIA?)
  struct bcl_branch___other__ {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CIA.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.LR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CIA.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CR.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CTR.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CIA.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CR.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CTR.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.CIA.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 2: if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 3: if (!uarch.LR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 4: if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CIA.name(),-1);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CR.name(),-1);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CTR.name(),-1);
        break;
        case 3: break;
        case 4: break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CIA.name(),-1);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CR.name(),-1);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CTR.name(),-1);
        break;
        case 3: break;
        case 4: break;
      }
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CTR.allocate(uarch,ic,logger);
      uarch.LR.allocate(uarch,ic,logger);
      uarch.NIA.allocate(uarch,ic,logger);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CTR.deallocate(uarch,ic,logger);
      uarch.LR.deallocate(uarch,ic,logger);
      uarch.NIA.deallocate(uarch,ic,logger);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger && ic._instr->is_dirty(2))logger->logOpAction("write",static_cast<OSM_P&>(uarch).CTR.name(),-1);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).LR.name(),-1);
      if (logger && ic._instr->is_dirty(4))logger->logOpAction("write",static_cast<OSM_P&>(uarch).NIA.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CIA.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).CR.name(static_cast<OSM_P&>(uarch),0);
        case 2: return static_cast<OSM_P&>(uarch).CTR.name(static_cast<OSM_P&>(uarch),0);
        case 3: return static_cast<OSM_P&>(uarch).LR.name(static_cast<OSM_P&>(uarch),0);
        case 4: return static_cast<OSM_P&>(uarch).NIA.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CIA.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        case 2: return static_cast<OSM_P&>(uarch).CTR.id(static_cast<OSM_P&>(uarch),0);
        case 3: return static_cast<OSM_P&>(uarch).LR.id(static_cast<OSM_P&>(uarch),0);
        case 4: return static_cast<OSM_P&>(uarch).NIA.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
        case 2: return 0x0;
        case 3: return 0x0;
        case 4: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
        case 2: return ic._instr->write_mask(2).uint32();
        case 3: return ic._instr->write_mask(3).uint32();
        case 4: return ic._instr->write_mask(4).uint32();
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
        case 3: ic._instr->write_mask(3) = tag;
        break;
        case 4: ic._instr->write_mask(4) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 1;
        case 2: return 2;
        case 3: return 3;
        case 4: return 4;
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
  //   (src: CR/p)
  //   (src: CTR?, trg: CTR?)
  //   (src: LR?)
  //   (trg: NIA?)
  struct bclr_branch___other__ {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.LR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CR.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CTR.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).LR.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CR.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CTR.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).LR.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 2: if (!uarch.LR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 3: if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CR.name(),-1);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CTR.name(),-1);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).LR.name(),-1);
        break;
        case 3: break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CR.name(),-1);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CTR.name(),-1);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).LR.name(),-1);
        break;
        case 3: break;
      }
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
      if (logger && ic._instr->is_dirty(1))logger->logOpAction("write",static_cast<OSM_P&>(uarch).CTR.name(),-1);
      if (logger && ic._instr->is_dirty(3))logger->logOpAction("write",static_cast<OSM_P&>(uarch).NIA.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).CTR.name(static_cast<OSM_P&>(uarch),0);
        case 2: return static_cast<OSM_P&>(uarch).LR.name(static_cast<OSM_P&>(uarch),0);
        case 3: return static_cast<OSM_P&>(uarch).NIA.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).CTR.id(static_cast<OSM_P&>(uarch),0);
        case 2: return static_cast<OSM_P&>(uarch).LR.id(static_cast<OSM_P&>(uarch),0);
        case 3: return static_cast<OSM_P&>(uarch).NIA.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
        case 2: return 0x0;
        case 3: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
        case 2: return ic._instr->write_mask(2).uint32();
        case 3: return ic._instr->write_mask(3).uint32();
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
        case 3: ic._instr->write_mask(3) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 1;
        case 2: return 2;
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
  //   (src: CR/p)
  //   (src: CTR?)
  //   (trg: NIA?)
  struct bcctr_branch___other__ {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CR.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CTR.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CR.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CTR.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 2: if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CR.name(),-1);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CTR.name(),-1);
        break;
        case 2: break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CR.name(),-1);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CTR.name(),-1);
        break;
        case 2: break;
      }
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
      if (logger && ic._instr->is_dirty(2))logger->logOpAction("write",static_cast<OSM_P&>(uarch).NIA.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).CTR.name(static_cast<OSM_P&>(uarch),0);
        case 2: return static_cast<OSM_P&>(uarch).NIA.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).CTR.id(static_cast<OSM_P&>(uarch),0);
        case 2: return static_cast<OSM_P&>(uarch).NIA.id(static_cast<OSM_P&>(uarch),0);
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
  //   (src: CIA)
  //   (src: CR/p)
  //   (src: CTR?)
  //   (trg: LR)
  //   (trg: NIA?)
  struct bcctrl_branch___other__ {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CIA.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.LR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CIA.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CR.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CTR.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CIA.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CR.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CTR.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.CIA.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 2: if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 3: if (!uarch.LR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 4: if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CIA.name(),-1);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CR.name(),-1);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CTR.name(),-1);
        break;
        case 3: break;
        case 4: break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CIA.name(),-1);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CR.name(),-1);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CTR.name(),-1);
        break;
        case 3: break;
        case 4: break;
      }
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
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).LR.name(),-1);
      if (logger && ic._instr->is_dirty(4))logger->logOpAction("write",static_cast<OSM_P&>(uarch).NIA.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CIA.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).CR.name(static_cast<OSM_P&>(uarch),0);
        case 2: return static_cast<OSM_P&>(uarch).CTR.name(static_cast<OSM_P&>(uarch),0);
        case 3: return static_cast<OSM_P&>(uarch).LR.name(static_cast<OSM_P&>(uarch),0);
        case 4: return static_cast<OSM_P&>(uarch).NIA.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CIA.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        case 2: return static_cast<OSM_P&>(uarch).CTR.id(static_cast<OSM_P&>(uarch),0);
        case 3: return static_cast<OSM_P&>(uarch).LR.id(static_cast<OSM_P&>(uarch),0);
        case 4: return static_cast<OSM_P&>(uarch).NIA.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
        case 2: return 0x0;
        case 3: return 0x0;
        case 4: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
        case 2: return ic._instr->write_mask(2).uint32();
        case 3: return ic._instr->write_mask(3).uint32();
        case 4: return ic._instr->write_mask(4).uint32();
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
        case 3: ic._instr->write_mask(3) = tag;
        break;
        case 4: ic._instr->write_mask(4) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 1;
        case 2: return 2;
        case 3: return 3;
        case 4: return 4;
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
  //   (src: CIA)
  //   (src: CR/p)
  //   (src: CTR?, trg: CTR?)
  //   (src: LR?, trg: LR)
  //   (trg: NIA?)
  struct bclrl_branch___other__ {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CIA.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.LR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CIA.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CR.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CTR.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).LR.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CIA.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CR.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CTR.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).LR.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.CIA.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 2: if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 3: if (!uarch.LR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 4: if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CIA.name(),-1);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CR.name(),-1);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CTR.name(),-1);
        break;
        case 3: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).LR.name(),-1);
        break;
        case 4: break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CIA.name(),-1);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CR.name(),-1);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CTR.name(),-1);
        break;
        case 3: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).LR.name(),-1);
        break;
        case 4: break;
      }
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CTR.allocate(uarch,ic,logger);
      uarch.LR.allocate(uarch,ic,logger);
      uarch.NIA.allocate(uarch,ic,logger);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CTR.deallocate(uarch,ic,logger);
      uarch.LR.deallocate(uarch,ic,logger);
      uarch.NIA.deallocate(uarch,ic,logger);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger && ic._instr->is_dirty(2))logger->logOpAction("write",static_cast<OSM_P&>(uarch).CTR.name(),-1);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).LR.name(),-1);
      if (logger && ic._instr->is_dirty(4))logger->logOpAction("write",static_cast<OSM_P&>(uarch).NIA.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CIA.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).CR.name(static_cast<OSM_P&>(uarch),0);
        case 2: return static_cast<OSM_P&>(uarch).CTR.name(static_cast<OSM_P&>(uarch),0);
        case 3: return static_cast<OSM_P&>(uarch).LR.name(static_cast<OSM_P&>(uarch),0);
        case 4: return static_cast<OSM_P&>(uarch).NIA.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CIA.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        case 2: return static_cast<OSM_P&>(uarch).CTR.id(static_cast<OSM_P&>(uarch),0);
        case 3: return static_cast<OSM_P&>(uarch).LR.id(static_cast<OSM_P&>(uarch),0);
        case 4: return static_cast<OSM_P&>(uarch).NIA.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
        case 2: return 0x0;
        case 3: return 0x0;
        case 4: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
        case 2: return ic._instr->write_mask(2).uint32();
        case 3: return ic._instr->write_mask(3).uint32();
        case 4: return ic._instr->write_mask(4).uint32();
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
        case 3: ic._instr->write_mask(3) = tag;
        break;
        case 4: ic._instr->write_mask(4) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 1;
        case 2: return 2;
        case 3: return 3;
        case 4: return 4;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 5; };
    
  };
  
  //
  // Instruction class branch
  //
  template <class __other___t>
  struct Inst_branch : public Inst_P {
    
    typedef struct Inst_branch<__other___t> ThisClass;
    
    Inst_branch &thisInstr() { return *this; };
    
    __other___t __other__;
    void reset(unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_P::reset(iid,cid,ic);
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    
    bool can_read_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      return (__other__.can_read(uarch,ic,logger));
    }
    
    bool can_write_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      return (__other__.can_write(uarch,ic,logger));
    }
    
    void read_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      __other__.read(uarch,ic,logger);
    }
    
    void allocate_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      __other__.allocate(uarch,ic,logger);
    }
    
    void deallocate_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      __other__.deallocate(uarch,ic,logger);
    }
    
    void write_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      __other__.write(uarch,ic,logger);
    }
    
    bool exec_done() const { return true; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool change_state_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    bool change_state_S_EX__S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      return action_S_EXe(uarch,logger,position);
    }
    
    bool change_state_S_EXe__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
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
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.issue(_instr);
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool change_state_S_MM__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    bool change_state_S_MM__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      return action_S_MMe(uarch,logger,position);
    }
    
    bool change_state_S_MMe__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    bool change_state_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      if (uarch.is_current_speculative_instr(*this)) {
        if (logger) logger->logMessage("forced flush!");
        uarch.taken_flush_mWB(*this,logger,position,false,0);
      }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    bool change_state_S_WB__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      if (uarch.is_current_speculative_instr(*this)) {
        if (logger) logger->logMessage("forced flush!");
        uarch.taken_flush_mWB(*this,logger,position,false,0);
      }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger) ) {
            return change_state_S_EX__S_MM(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger) ) {
            return change_state_S_EXe__S_MMe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( can_read_ops(uarch,*this,logger) && can_write_ops(uarch,*this,logger) && uarch.msync_stall.can_allocate(uarch,*this,logger) && uarch.mEX.has_space(uarch,*this,logger) ) {
            read_ops(uarch,*this,logger) ;
            allocate_ops(uarch,*this,logger) ;
            uarch.exec_branch(*this,logger) ;
            write_ops(uarch,*this,logger) ;
            if ( branch_taken(uarch) ) {
                  uarch.taken_flush_mID(*this,logger,position  ) ;
              }
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
    
    // State action S_MM
    bool action_S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger) ) {
            return change_state_S_MM__S_WB(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // State action S_MMe
    bool action_S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger) ) {
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
    }
    static void flush_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
      deallocate_ops(uarch,*this,logger);
    }
    static void flush_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_IDe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_P &uarch,Logger *logger) {
      deallocate_ops(uarch,*this,logger);
    }
    static void flush_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_MMe
    void flush_S_MMe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMe(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_P &uarch,Logger *logger) {
      deallocate_ops(uarch,*this,logger);
    }
    static void flush_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBe(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class dispatchSerial
  //
  
  //
  // Instruction class dispatchSerial
  //
  struct Inst_dispatchSerial : public Inst_P {
    
    typedef struct Inst_dispatchSerial ThisClass;
    
    Inst_dispatchSerial &thisInstr() { return *this; };
    
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
    
    void write_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
    }
    
    bool exec_done() const { return true; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool change_state_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    bool change_state_S_EX__S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      return action_S_EXe(uarch,logger,position);
    }
    
    bool change_state_S_EXe__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
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
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.issue(_instr);
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool change_state_S_MM__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    bool change_state_S_MM__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      return action_S_MMe(uarch,logger,position);
    }
    
    bool change_state_S_MMe__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    bool change_state_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    bool change_state_S_WB__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger) ) {
            uarch.exec(*this,logger) ;
            uarch.flush_mEX(*this,logger,position  ) ;
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
    static bool action_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger) ) {
            return change_state_S_EXe__S_MMe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.msync_stall.can_allocate(uarch,*this,logger) && uarch.mEX.has_space(uarch,*this,logger) ) {
            return change_state_S_ID__S_EX(uarch,logger,position) ;
            uarch.msync_stall.allocate(uarch,*this,logger) ;
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
      if ( uarch.mWB.has_space(uarch,*this,logger) ) {
            return change_state_S_MM__S_WB(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // State action S_MMe
    bool action_S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger) ) {
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
      uarch.msync_stall.deallocate(uarch,*this,logger);
    }
    static void flush_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
      uarch.msync_stall.deallocate(uarch,*this,logger);
    }
    static void flush_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_IDe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_P &uarch,Logger *logger) {
      uarch.msync_stall.deallocate(uarch,*this,logger);
    }
    static void flush_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_MMe
    void flush_S_MMe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMe(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_P &uarch,Logger *logger) {
      uarch.msync_stall.deallocate(uarch,*this,logger);
    }
    static void flush_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_P &uarch,Logger *logger) {
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
    
    bool change_state_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
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
    
    bool change_state_S_MM__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    bool change_state_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit_checked(this);
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger) ) {
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
            uarch.commit_exceptions(*this,logger) ;
            uarch.taken_flush_mID(*this,logger,position  ) ;
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
      if ( uarch.mWB.has_space(uarch,*this,logger) ) {
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
    }
    static void flush_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
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
    
    // Flush action S_MM
    void flush_S_MM(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class jump
  //
  
  // Operands:
  //   (trg: MSR)
  //   (trg: NIA)
  //   (src: SRR0)
  //   (src: SRR1)
  struct rfi_jump___other__ {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.MSR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.SRR0.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.SRR1.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).SRR0.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).SRR1.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).SRR0.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).SRR1.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.MSR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 2: if (!uarch.SRR0.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 3: if (!uarch.SRR1.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).SRR0.name(),-1);
        break;
        case 3: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).SRR1.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: break;
        case 1: break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).SRR0.name(),-1);
        break;
        case 3: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).SRR1.name(),-1);
        break;
      }
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
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).MSR.name(),-1);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).NIA.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).MSR.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).NIA.name(static_cast<OSM_P&>(uarch),0);
        case 2: return static_cast<OSM_P&>(uarch).SRR0.name(static_cast<OSM_P&>(uarch),0);
        case 3: return static_cast<OSM_P&>(uarch).SRR1.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).MSR.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).NIA.id(static_cast<OSM_P&>(uarch),0);
        case 2: return static_cast<OSM_P&>(uarch).SRR0.id(static_cast<OSM_P&>(uarch),0);
        case 3: return static_cast<OSM_P&>(uarch).SRR1.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
        case 2: return 0x0;
        case 3: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
        case 2: return ic._instr->write_mask(2).uint32();
        case 3: return ic._instr->write_mask(3).uint32();
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
        case 3: ic._instr->write_mask(3) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 1;
        case 2: return 2;
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
  //   (src: CSRR0/p)
  //   (src: CSRR1/p)
  //   (trg: ESR?)
  //   (trg: MSR)
  //   (trg: NIA)
  struct rfci_jump___other__ {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CSRR0.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.CSRR1.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.ESR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.MSR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CSRR0.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CSRR1.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CSRR0.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CSRR1.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.CSRR0.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.CSRR1.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 2: if (!uarch.ESR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 3: if (!uarch.MSR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 4: if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CSRR0.name(),-1);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CSRR1.name(),-1);
        break;
        case 2: break;
        case 3: break;
        case 4: break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CSRR0.name(),-1);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CSRR1.name(),-1);
        break;
        case 2: break;
        case 3: break;
        case 4: break;
      }
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.ESR.allocate(uarch,ic,logger);
      uarch.MSR.allocate(uarch,ic,logger);
      uarch.NIA.allocate(uarch,ic,logger);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.ESR.deallocate(uarch,ic,logger);
      uarch.MSR.deallocate(uarch,ic,logger);
      uarch.NIA.deallocate(uarch,ic,logger);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger && ic._instr->is_dirty(2))logger->logOpAction("write",static_cast<OSM_P&>(uarch).ESR.name(),-1);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).MSR.name(),-1);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).NIA.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CSRR0.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).CSRR1.name(static_cast<OSM_P&>(uarch),0);
        case 2: return static_cast<OSM_P&>(uarch).ESR.name(static_cast<OSM_P&>(uarch),0);
        case 3: return static_cast<OSM_P&>(uarch).MSR.name(static_cast<OSM_P&>(uarch),0);
        case 4: return static_cast<OSM_P&>(uarch).NIA.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CSRR0.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).CSRR1.id(static_cast<OSM_P&>(uarch),0);
        case 2: return static_cast<OSM_P&>(uarch).ESR.id(static_cast<OSM_P&>(uarch),0);
        case 3: return static_cast<OSM_P&>(uarch).MSR.id(static_cast<OSM_P&>(uarch),0);
        case 4: return static_cast<OSM_P&>(uarch).NIA.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
        case 2: return 0x0;
        case 3: return 0x0;
        case 4: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
        case 2: return ic._instr->write_mask(2).uint32();
        case 3: return ic._instr->write_mask(3).uint32();
        case 4: return ic._instr->write_mask(4).uint32();
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
        case 3: ic._instr->write_mask(3) = tag;
        break;
        case 4: ic._instr->write_mask(4) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 1;
        case 2: return 2;
        case 3: return 3;
        case 4: return 4;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 5; };
    
  };
  
  //
  // Instruction class jump
  //
  template <class __other___t>
  struct Inst_jump : public Inst_P {
    
    typedef struct Inst_jump<__other___t> ThisClass;
    
    Inst_jump &thisInstr() { return *this; };
    
    __other___t __other__;
    void reset(unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_P::reset(iid,cid,ic);
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    
    bool can_read_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      return (__other__.can_read(uarch,ic,logger));
    }
    
    bool can_write_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      return (__other__.can_write(uarch,ic,logger));
    }
    
    void read_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      __other__.read(uarch,ic,logger);
    }
    
    void allocate_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      __other__.allocate(uarch,ic,logger);
    }
    
    void deallocate_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      __other__.deallocate(uarch,ic,logger);
    }
    
    void write_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      __other__.write(uarch,ic,logger);
    }
    
    bool exec_done() const { return true; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool change_state_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    bool change_state_S_EX__S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      return action_S_EXe(uarch,logger,position);
    }
    
    bool change_state_S_EXe__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
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
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.issue(_instr);
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool change_state_S_MM__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    bool change_state_S_MM__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      return action_S_MMe(uarch,logger,position);
    }
    
    bool change_state_S_MMe__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    bool change_state_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      if (uarch.is_current_speculative_instr(*this)) {
        if (logger) logger->logMessage("forced flush!");
        uarch.taken_flush_mWB(*this,logger,position,false,0);
      }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    bool change_state_S_WB__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      if (uarch.is_current_speculative_instr(*this)) {
        if (logger) logger->logMessage("forced flush!");
        uarch.taken_flush_mWB(*this,logger,position,false,0);
      }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger) ) {
            return change_state_S_EX__S_MM(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger) ) {
            return change_state_S_EXe__S_MMe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( can_read_ops(uarch,*this,logger) && can_write_ops(uarch,*this,logger) && uarch.msync_stall.can_allocate(uarch,*this,logger) && uarch.mEX.has_space(uarch,*this,logger) ) {
            read_ops(uarch,*this,logger) ;
            allocate_ops(uarch,*this,logger) ;
            {uarch.exec_branch(*this,logger) ; if (has_exceptions()) goto HandleException; } ;
            write_ops(uarch,*this,logger) ;
            if ( branch_taken(uarch) ) {
                  uarch.taken_flush_mID(*this,logger,position  ) ;
              }
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
    
    // State action S_MM
    bool action_S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger) ) {
            return change_state_S_MM__S_WB(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // State action S_MMe
    bool action_S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger) ) {
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
    }
    static void flush_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
      deallocate_ops(uarch,*this,logger);
    }
    static void flush_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_IDe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_P &uarch,Logger *logger) {
      deallocate_ops(uarch,*this,logger);
    }
    static void flush_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_MMe
    void flush_S_MMe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMe(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_P &uarch,Logger *logger) {
      deallocate_ops(uarch,*this,logger);
    }
    static void flush_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBe(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class lmw
  //
  
  // Operands:
  //   (src: GPR(RA))
  struct lmw_lmw_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
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
      read(uarch,ic,logger);
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
  //   (trg: GPR(RT . uint32 (  ),31)/p)
  struct lmw_lmw_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      for (int i = 1; i != 33; ++i) {
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
        for (int i = 1; i != 33; ++i) {
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
      for (int i = 1; i != 33; ++i) {
        if (ic.op(i).isTrg()) {
          uarch.GPR.allocate(uarch,ic,logger,ic.op(i)._index);
        }
      }
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      for (int i = 1; i != 33; ++i) {
        if (ic.op(i).isTrg()) {
          uarch.GPR.deallocate(uarch,ic,logger,ic.op(i)._index);
        }
      }
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      for (int i = 1; i != 33; ++i) {
        if (ic.op(i).isTrg()) {
          if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(i)._index);
        }
      }
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(op_off)._index); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(op_off)._index); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return 0x0; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(1).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(1) = tag; }
    
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
    
    bool exec_done() const { return mem() == 0; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool change_state_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    bool change_state_S_EX__S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      return action_S_EXe(uarch,logger,position);
    }
    
    bool change_state_S_EXe__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_IDp(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_IDp"); }
      _action = (StateFunc)action_S_IDp_p;
      _flush = (FlushFunc)flush_S_IDp_p;
      return action_S_IDp(uarch,logger,position);
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
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_IDp__S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDp -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_IDp__S_IDe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDp -> S_IDe"); }
      _action = (StateFunc)action_S_IDe_p;
      _flush = (FlushFunc)flush_S_IDe_p;
      return action_S_IDe(uarch,logger,position);
    }
    
    bool change_state_S_INIT__S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.issue(_instr);
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
    
    bool change_state_S_MMe__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    bool change_state_S_MMp__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMp -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      return action_S_MM(uarch,logger,position);
    }
    
    bool change_state_S_MMp__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMp -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    bool change_state_S_MMp__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMp -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      return action_S_MMe(uarch,logger,position);
    }
    
    bool change_state_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      uarch.commit(this);
      return true;
    }
    
    bool change_state_S_WB__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      uarch.commit(this);
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger) ) {
            if ( has_next_request (  ) && uarch.L1d.can_request_read(uarch,*this,logger) ) {
                  uarch.L1d.send_read_request(uarch,*this,logger, 8 ) ;
                  next_req_mem (  ) ;
              }
            return change_state_S_EX__S_MM(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger) ) {
            return change_state_S_EXe__S_MMe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( Src.can_read_fp(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) && Trg.can_write(uarch,*this,logger) ) {
            Src.read(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) ;
            Trg.allocate(uarch,*this,logger) ;
            return change_state_S_ID__S_IDp(uarch,logger,position) ;
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
    
    // State action S_IDp
    bool action_S_IDp(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.lsamd_stall.can_allocate(uarch,*this,logger) && uarch.msync_stall.can_allocate(uarch,*this,logger) && uarch.mEX.has_space(uarch,*this,logger) ) {
            {uarch.exec_mem(*this,logger) ; if (has_exceptions()) goto HandleException; } ;
            uarch.L1d.send_read_request(uarch,*this,logger, 8 ) ;
            next_req_mem (  ) ;
            return change_state_S_IDp__S_EX(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_IDp(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mID(*this,logger,position,true,branch_target());
        return change_state_S_IDp__S_IDe(uarch,logger,position);
      }
    }
    static bool action_S_IDp_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IDp(uarch,logger,position);
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
      if ( uarch.L1d.can_read(uarch,*this,logger) ) {
            { uarch.exec_and_read(*this,logger, uarch.L1d , 2 )  ; if (has_exceptions()) goto HandleException; } ;
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
    static bool action_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // State action S_MMe
    bool action_S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger) ) {
            return change_state_S_MMe__S_WBe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MMe(uarch,logger,position);
    }
    
    // State action S_MMp
    bool action_S_MMp(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( exec_done (  ) && uarch.mWB.has_space(uarch,*this,logger) ) {
            return change_state_S_MMp__S_WB(uarch,logger,position) ;
      } else if ( uarch.L1d.can_request_read(uarch,*this,logger) ) {
            uarch.L1d.send_read_request(uarch,*this,logger, 8 ) ;
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
    }
    static void flush_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_IDe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_IDp
    void flush_S_IDp(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_IDp_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDp(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_MMe
    void flush_S_MMe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMe(uarch,logger);
    }
    
    // Flush action S_MMp
    void flush_S_MMp(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_MMp_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMp(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBe(uarch,logger);
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
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
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
      read(uarch,ic,logger);
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
  //   (trg: GPR(RT)/p)
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
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(1)._index;
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
  
  // Operand Alias:  lbz_load_Src -> lbzu_load_Src
  //   (src: GPR(RA))
  typedef lbz_load_Src lbzu_load_Src;
  
  // Operands:
  //   (trg: GPR(RA)/p)
  //   (trg: GPR(RT)/p)
  struct lbzu_load_Trg {
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
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
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
  //   (trg: GPR(RA)/p)
  struct lbzu_load_TrgEA {
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
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
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
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  struct lbzux_load_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
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
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
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
      read(uarch,ic,logger);
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
  //   (trg: GPR(RA)/p)
  //   (trg: GPR(RT)/p)
  struct lbzux_load_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
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
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
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
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
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
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(0)._index);
      uarch.GPR.allocate(uarch,ic,logger,ic.op(2)._index);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(0)._index);
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(2)._index);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(2)._index);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
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
  
  // Operand Alias:  lbzu_load_TrgEA -> lbzux_load_TrgEA
  //   (trg: GPR(RA)/p)
  typedef lbzu_load_TrgEA lbzux_load_TrgEA;
  
  // Operand Alias:  lbzux_load_Src -> lbzx_load_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef lbzux_load_Src lbzx_load_Src;
  
  // Operands:
  //   (trg: GPR(RT)/p)
  struct lbzx_load_Trg {
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
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(2)._index;
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
  
  // Operand Alias:  lbz_load_Src -> lhz_load_Src
  //   (src: GPR(RA))
  typedef lbz_load_Src lhz_load_Src;
  
  // Operand Alias:  lbz_load_Trg -> lhz_load_Trg
  //   (trg: GPR(RT)/p)
  typedef lbz_load_Trg lhz_load_Trg;
  
  // Operand Alias:  lbz_load_Src -> lhzu_load_Src
  //   (src: GPR(RA))
  typedef lbz_load_Src lhzu_load_Src;
  
  // Operand Alias:  lbzu_load_Trg -> lhzu_load_Trg
  //   (trg: GPR(RA)/p)
  //   (trg: GPR(RT)/p)
  typedef lbzu_load_Trg lhzu_load_Trg;
  
  // Operand Alias:  lbzu_load_TrgEA -> lhzu_load_TrgEA
  //   (trg: GPR(RA)/p)
  typedef lbzu_load_TrgEA lhzu_load_TrgEA;
  
  // Operand Alias:  lbzux_load_Src -> lhzux_load_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef lbzux_load_Src lhzux_load_Src;
  
  // Operand Alias:  lbzux_load_Trg -> lhzux_load_Trg
  //   (trg: GPR(RA)/p)
  //   (trg: GPR(RT)/p)
  typedef lbzux_load_Trg lhzux_load_Trg;
  
  // Operand Alias:  lbzu_load_TrgEA -> lhzux_load_TrgEA
  //   (trg: GPR(RA)/p)
  typedef lbzu_load_TrgEA lhzux_load_TrgEA;
  
  // Operand Alias:  lbzux_load_Src -> lhzx_load_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef lbzux_load_Src lhzx_load_Src;
  
  // Operand Alias:  lbzx_load_Trg -> lhzx_load_Trg
  //   (trg: GPR(RT)/p)
  typedef lbzx_load_Trg lhzx_load_Trg;
  
  // Operand Alias:  lbz_load_Src -> lha_load_Src
  //   (src: GPR(RA))
  typedef lbz_load_Src lha_load_Src;
  
  // Operand Alias:  lbz_load_Trg -> lha_load_Trg
  //   (trg: GPR(RT)/p)
  typedef lbz_load_Trg lha_load_Trg;
  
  // Operand Alias:  lbzux_load_Src -> lhax_load_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef lbzux_load_Src lhax_load_Src;
  
  // Operand Alias:  lbzx_load_Trg -> lhax_load_Trg
  //   (trg: GPR(RT)/p)
  typedef lbzx_load_Trg lhax_load_Trg;
  
  // Operand Alias:  lbz_load_Src -> lwz_load_Src
  //   (src: GPR(RA))
  typedef lbz_load_Src lwz_load_Src;
  
  // Operand Alias:  lbz_load_Trg -> lwz_load_Trg
  //   (trg: GPR(RT)/p)
  typedef lbz_load_Trg lwz_load_Trg;
  
  // Operand Alias:  lbz_load_Src -> lwzu_load_Src
  //   (src: GPR(RA))
  typedef lbz_load_Src lwzu_load_Src;
  
  // Operand Alias:  lbzu_load_Trg -> lwzu_load_Trg
  //   (trg: GPR(RA)/p)
  //   (trg: GPR(RT)/p)
  typedef lbzu_load_Trg lwzu_load_Trg;
  
  // Operand Alias:  lbzu_load_TrgEA -> lwzu_load_TrgEA
  //   (trg: GPR(RA)/p)
  typedef lbzu_load_TrgEA lwzu_load_TrgEA;
  
  // Operand Alias:  lbzux_load_Src -> lwzux_load_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef lbzux_load_Src lwzux_load_Src;
  
  // Operand Alias:  lbzux_load_Trg -> lwzux_load_Trg
  //   (trg: GPR(RA)/p)
  //   (trg: GPR(RT)/p)
  typedef lbzux_load_Trg lwzux_load_Trg;
  
  // Operand Alias:  lbzu_load_TrgEA -> lwzux_load_TrgEA
  //   (trg: GPR(RA)/p)
  typedef lbzu_load_TrgEA lwzux_load_TrgEA;
  
  // Operand Alias:  lbzux_load_Src -> lwzx_load_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef lbzux_load_Src lwzx_load_Src;
  
  // Operand Alias:  lbzx_load_Trg -> lwzx_load_Trg
  //   (trg: GPR(RT)/p)
  typedef lbzx_load_Trg lwzx_load_Trg;
  
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
    
    bool exec_done() const { return mem() == 0; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool change_state_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    bool change_state_S_EX__S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      return action_S_EXe(uarch,logger,position);
    }
    
    bool change_state_S_EXe__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_IDp(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_IDp"); }
      _action = (StateFunc)action_S_IDp_p;
      _flush = (FlushFunc)flush_S_IDp_p;
      return action_S_IDp(uarch,logger,position);
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
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_IDp__S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDp -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_IDp__S_IDe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDp -> S_IDe"); }
      _action = (StateFunc)action_S_IDe_p;
      _flush = (FlushFunc)flush_S_IDe_p;
      return action_S_IDe(uarch,logger,position);
    }
    
    bool change_state_S_INIT__S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.issue(_instr);
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool change_state_S_MM__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      return action_S_MM(uarch,logger,position);
    }
    
    bool change_state_S_MM__S_MMp(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMp"); }
      _action = (StateFunc)action_S_MMp_p;
      _flush = (FlushFunc)flush_S_MMp_p;
      return action_S_MMp(uarch,logger,position);
    }
    
    bool change_state_S_MM__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    bool change_state_S_MM__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      return action_S_MMe(uarch,logger,position);
    }
    
    bool change_state_S_MMe__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    bool change_state_S_MMp__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMp -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      return action_S_MM(uarch,logger,position);
    }
    
    bool change_state_S_MMp__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMp -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      return action_S_MMe(uarch,logger,position);
    }
    
    bool change_state_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      uarch.commit(this);
      return true;
    }
    
    bool change_state_S_WB__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      uarch.commit(this);
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, uarch.mEX.capacity() ) ) {
            uarch.FwdEX.write(uarch,*this,logger, TrgEA ) ;
            if ( ! has_next_request (  ) ) {
                  if ( has_more_mem (  ) && uarch.L1d.can_read(uarch,*this,logger) ) {
                        { uarch.exec_and_read(*this,logger, uarch.L1d )  ; if (has_exceptions()) goto HandleException; } ;
                    }
              } else if ( uarch.L1d.can_request_read(uarch,*this,logger) ) {
                  uarch.L1d.send_read_request(uarch,*this,logger) ;
              }
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
    static bool action_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger) ) {
            return change_state_S_EXe__S_MMe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( Src.can_read_fp(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) && Trg.can_write(uarch,*this,logger) ) {
            Src.read(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) ;
            Trg.allocate(uarch,*this,logger) ;
            return change_state_S_ID__S_IDp(uarch,logger,position) ;
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
    
    // State action S_IDp
    bool action_S_IDp(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.lsamd_stall.can_allocate(uarch,*this,logger) && uarch.msync_stall.can_allocate(uarch,*this,logger) && uarch.mEX.has_space(uarch,*this,logger) && uarch.L1d.can_request_read(uarch,*this,logger) && ! uarch.stalled() ) {
            {uarch.exec_mem(*this,logger) ; if (has_exceptions()) goto HandleException; } ;
            uarch.L1d.send_read_request(uarch,*this,logger) ;
            return change_state_S_IDp__S_EX(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_IDp(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mID(*this,logger,position,true,branch_target());
        return change_state_S_IDp__S_IDe(uarch,logger,position);
      }
    }
    static bool action_S_IDp_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IDp(uarch,logger,position);
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
      if ( exec_done (  ) && uarch.mWB.has_space(uarch,*this,logger, uarch.mMM.capacity() ) ) {
            uarch.FwdMM.write(uarch,*this,logger, TrgEA ) ;
            uarch.FwdMM.write(uarch,*this,logger, Trg ) ;
            return change_state_S_MM__S_WB(uarch,logger,position) ;
      } else if ( ! has_next_request (  ) ) {
            if ( uarch.L1d.can_read(uarch,*this,logger) ) {
                  { uarch.exec_and_read(*this,logger, uarch.L1d )  ; if (has_exceptions()) goto HandleException; } ;
                  return change_state_S_MM__S_MM(uarch,logger,position) ;
              }
      } else if ( uarch.L1d.can_request_read(uarch,*this,logger) ) {
            uarch.L1d.send_read_request(uarch,*this,logger) ;
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
    static bool action_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // State action S_MMe
    bool action_S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger) ) {
            return change_state_S_MMe__S_WBe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MMe(uarch,logger,position);
    }
    
    // State action S_MMp
    bool action_S_MMp(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.L1d.can_read(uarch,*this,logger) ) {
            { uarch.exec_and_read(*this,logger, uarch.L1d )  ; if (has_exceptions()) goto HandleException; } ;
            uarch.FwdMM.write(uarch,*this,logger, TrgEA ) ;
            return change_state_S_MMp__S_MM(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_MMp(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mMM(*this,logger,position,true,branch_target());
        return change_state_S_MMp__S_MMe(uarch,logger,position);
      }
    }
    static bool action_S_MMp_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MMp(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
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
    }
    static void flush_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_IDe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_IDp
    void flush_S_IDp(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_IDp_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDp(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_MMe
    void flush_S_MMe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMe(uarch,logger);
    }
    
    // Flush action S_MMp
    void flush_S_MMp(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_MMp_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMp(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBe(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class mftspr
  //
  
  // Operands:
  //   (src: SPR(SPRN)?)
  struct mfspr_mftspr_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.SPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).SPR.name(),ic.op(1)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).SPR.name(),ic.op(1)._index);
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
      read(uarch,ic,logger);
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
  //   (trg: GPR(RT)/p?)
  struct mfspr_mftspr_Trg {
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
      if (logger && ic._instr->is_dirty(0))logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
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
  //   (const: 1)
  struct mtspr_mftspr_CheckDelay {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
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
      return 1;
    }
    
    unsigned value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return 1;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &,Inst_P &,unsigned = 0,unsigned = 0) const{ return std::string(); }
    
    RegId id(UadlArch &,const Inst_P&,unsigned = 0,unsigned = 0) const { return RegInvalidId; }
    
    bits<32> mask(InstrBase &,unsigned = 0) const { return 0; };
    
    unsigned slot(unsigned = 0,unsigned = 0) const { return 0; };
    
    bool is_op_read(InstrBase &ic,unsigned = 0) const { return true; }
    
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (src: GPR(RS)/p?)
  struct mtspr_mftspr_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
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
      read(uarch,ic,logger);
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
  //   (trg: SPR(SPRN)?)
  struct mtspr_mftspr_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.SPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
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
        RegId id = static_cast<OSM_P&>(uarch).SPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).SPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.SPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
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
      uarch.SPR.allocate(uarch,ic,logger,ic.op(1)._index);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.SPR.deallocate(uarch,ic,logger,ic.op(1)._index);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger && ic._instr->is_dirty(1))logger->logOpAction("write",static_cast<OSM_P&>(uarch).SPR.name(),ic.op(1)._index);
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
  // Instruction class mftspr
  //
  template <class CheckDelay_t, class Src_t, class Trg_t>
  struct Inst_mftspr : public Inst_P {
    
    typedef struct Inst_mftspr<CheckDelay_t, Src_t, Trg_t> ThisClass;
    
    Inst_mftspr &thisInstr() { return *this; };
    
    CheckDelay_t CheckDelay;
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
      CheckDelay.deallocate(uarch,ic,logger);
      Src.deallocate(uarch,ic,logger);
      Trg.deallocate(uarch,ic,logger);
    }
    
    void write_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      CheckDelay.write(uarch,ic,logger);
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
    
    bool change_state_S_EXe__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    bool change_state_S_EXp__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXp -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    bool change_state_S_EXp__S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXp -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      return action_S_EXe(uarch,logger,position);
    }
    
    bool change_state_S_ID__S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
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
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.issue(_instr);
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool change_state_S_MM__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    bool change_state_S_MM__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      return action_S_MMe(uarch,logger,position);
    }
    
    bool change_state_S_MMe__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    bool change_state_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    bool change_state_S_WB__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      {
            {uarch.exec(*this,logger) ; if (has_exceptions()) goto HandleException; } ;
            if ( CheckDelay.value(uarch,*this,logger) && ( Trg.field_value(uarch,*this,logger) == 284 || Trg.field_value(uarch,*this,logger) == 285 ) ) {
                  set_latency ( 4 ) ;
              }
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
    static bool action_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger) ) {
            return change_state_S_EXe__S_MMe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_EXp
    bool action_S_EXp(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger) ) if ( check_latency(logger) && uarch.mWB.has_space(uarch,*this,logger) ) {
            uarch.FwdEX.write(uarch,*this,logger, Trg ) ;
            return change_state_S_EXp__S_MM(uarch,logger,position) ;
      } else {
            decr_latency (  ) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        flush_S_EXp(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mEX(*this,logger,position,true,branch_target());
        return change_state_S_EXp__S_EXe(uarch,logger,position);
      }
    }
    static bool action_S_EXp_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXp(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( Src.can_read_fp(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) && Trg.can_write(uarch,*this,logger) && uarch.msync_stall.can_allocate(uarch,*this,logger) && uarch.mEX.empty(uarch,*this,logger) && uarch.mWB.empty(uarch,*this,logger) ) {
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
    
    // State action S_MM
    bool action_S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger) ) {
            return change_state_S_MM__S_WB(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // State action S_MMe
    bool action_S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger) ) {
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
    }
    static void flush_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_EXp
    void flush_S_EXp(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_EXp_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXp(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_IDe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_MMe
    void flush_S_MMe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMe(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBe(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class sfx
  //
  
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
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
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
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
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
      read(uarch,ic,logger);
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
  //   (trg: GPR(RT)/p)
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
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(2)._index;
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
  
  // Operand Alias:  add_sfx_Src -> addc_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef add_sfx_Src addc_sfx_Src;
  
  // Operands:
  //   (trg: GPR(RT)/p)
  //   (trg: XER[mask 0x20000000]/p)
  struct addc_sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
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
      uarch.XER.allocate(uarch,ic,logger, ic._masks[0]);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(2)._index);
      uarch.XER.deallocate(uarch,ic,logger, ic._masks[0]);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).XER.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(2)._index;
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
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: XER[mask 0xc0000000]/p)
  struct addco_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
        case 2: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
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
      read(uarch,ic,logger);
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
        case 2: return ic._masks[1];
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
  
  // Operand Alias:  addc_sfx_Trg -> addco_sfx_Trg
  //   (trg: GPR(RT)/p)
  //   (trg: XER[mask 0xe0000000]/p)
  typedef addc_sfx_Trg addco_sfx_Trg;
  
  // Operand Alias:  addco_sfx_Src -> adde_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: XER[mask 0x20000000]/p)
  typedef addco_sfx_Src adde_sfx_Src;
  
  // Operand Alias:  addc_sfx_Trg -> adde_sfx_Trg
  //   (trg: GPR(RT)/p)
  //   (trg: XER[mask 0x20000000]/p)
  typedef addc_sfx_Trg adde_sfx_Trg;
  
  // Operand Alias:  addco_sfx_Src -> addeo_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: XER[mask 0xe0000000]/p)
  typedef addco_sfx_Src addeo_sfx_Src;
  
  // Operand Alias:  addc_sfx_Trg -> addeo_sfx_Trg
  //   (trg: GPR(RT)/p)
  //   (trg: XER[mask 0xe0000000]/p)
  typedef addc_sfx_Trg addeo_sfx_Trg;
  
  // Operands:
  //   (src: GPR(RA)?)
  struct addi_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
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
      read(uarch,ic,logger);
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
  //   (trg: GPR(RT)/p)
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
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(1)._index;
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
  
  // Operand Alias:  addi_sfx_Src -> addic_sfx_Src
  //   (src: GPR(RA))
  typedef addi_sfx_Src addic_sfx_Src;
  
  // Operands:
  //   (trg: GPR(RT)/p)
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
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).XER.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(1)._index;
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
  
  // Operand Alias:  addi_sfx_Src -> addis_sfx_Src
  //   (src: GPR(RA))
  typedef addi_sfx_Src addis_sfx_Src;
  
  // Operand Alias:  addi_sfx_Trg -> addis_sfx_Trg
  //   (trg: GPR(RT)/p)
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
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
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
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
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
      read(uarch,ic,logger);
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
  //   (trg: GPR(RT)/p)
  //   (trg: XER[mask 0x20000000]/p)
  typedef addic_sfx_Trg addme_sfx_Trg;
  
  // Operand Alias:  addme_sfx_Src -> addmeo_sfx_Src
  //   (src: GPR(RA))
  //   (src: XER[mask 0xe0000000]/p)
  typedef addme_sfx_Src addmeo_sfx_Src;
  
  // Operand Alias:  addic_sfx_Trg -> addmeo_sfx_Trg
  //   (trg: GPR(RT)/p)
  //   (trg: XER[mask 0xe0000000]/p)
  typedef addic_sfx_Trg addmeo_sfx_Trg;
  
  // Operand Alias:  addco_sfx_Src -> addo_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: XER[mask 0xc0000000]/p)
  typedef addco_sfx_Src addo_sfx_Src;
  
  // Operand Alias:  addc_sfx_Trg -> addo_sfx_Trg
  //   (trg: GPR(RT)/p)
  //   (trg: XER[mask 0xc0000000]/p)
  typedef addc_sfx_Trg addo_sfx_Trg;
  
  // Operand Alias:  addme_sfx_Src -> addze_sfx_Src
  //   (src: GPR(RA))
  //   (src: XER[mask 0x20000000]/p)
  typedef addme_sfx_Src addze_sfx_Src;
  
  // Operand Alias:  addic_sfx_Trg -> addze_sfx_Trg
  //   (trg: GPR(RT)/p)
  //   (trg: XER[mask 0x20000000]/p)
  typedef addic_sfx_Trg addze_sfx_Trg;
  
  // Operand Alias:  addme_sfx_Src -> addzeo_sfx_Src
  //   (src: GPR(RA))
  //   (src: XER[mask 0xe0000000]/p)
  typedef addme_sfx_Src addzeo_sfx_Src;
  
  // Operand Alias:  addic_sfx_Trg -> addzeo_sfx_Trg
  //   (trg: GPR(RT)/p)
  //   (trg: XER[mask 0xe0000000]/p)
  typedef addic_sfx_Trg addzeo_sfx_Trg;
  
  // Operands:
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  struct and_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
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
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
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
      read(uarch,ic,logger);
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
  
  // Operands:
  //   (trg: GPR(RA)/p)
  struct and_sfx_Trg {
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
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
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
  
  // Operand Alias:  and_sfx_Src -> andc_sfx_Src
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  typedef and_sfx_Src andc_sfx_Src;
  
  // Operand Alias:  and_sfx_Trg -> andc_sfx_Trg
  //   (trg: GPR(RA)/p)
  typedef and_sfx_Trg andc_sfx_Trg;
  
  // Operands:
  //   (src: GPR(RS))
  struct cntlzw_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
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
      read(uarch,ic,logger);
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
  
  // Operand Alias:  and_sfx_Trg -> cntlzw_sfx_Trg
  //   (trg: GPR(RA)/p)
  typedef and_sfx_Trg cntlzw_sfx_Trg;
  
  // Operand Alias:  and_sfx_Src -> eqv_sfx_Src
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  typedef and_sfx_Src eqv_sfx_Src;
  
  // Operand Alias:  and_sfx_Trg -> eqv_sfx_Trg
  //   (trg: GPR(RA)/p)
  typedef and_sfx_Trg eqv_sfx_Trg;
  
  // Operands:
  //   (src: GPR(RS)[mask 0x000000ff]/p)
  struct extsb_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,const bits<32> &mask) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read", (ic._masks[0] & ~mask).uint32())) return false;
      return true;
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
        if (!can_read(uarch,ic,logger,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
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
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(1)._index); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return ic._masks[0]; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(1).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(1) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 1; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operand Alias:  and_sfx_Trg -> extsb_sfx_Trg
  //   (trg: GPR(RA)/p)
  typedef and_sfx_Trg extsb_sfx_Trg;
  
  // Operands:
  //   (src: GPR(RS)[mask 0x0000ffff]/p)
  struct extsh_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read", ic._masks[1])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,const bits<32> &mask) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read", (ic._masks[1] & ~mask).uint32())) return false;
      return true;
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
        if (!can_read(uarch,ic,logger,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
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
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(1)._index); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return ic._masks[1]; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(1).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(1) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 1; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operand Alias:  and_sfx_Trg -> extsh_sfx_Trg
  //   (trg: GPR(RA)/p)
  typedef and_sfx_Trg extsh_sfx_Trg;
  
  // Operand Alias:  and_sfx_Src -> nand_sfx_Src
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  typedef and_sfx_Src nand_sfx_Src;
  
  // Operand Alias:  and_sfx_Trg -> nand_sfx_Trg
  //   (trg: GPR(RA)/p)
  typedef and_sfx_Trg nand_sfx_Trg;
  
  // Operand Alias:  addi_sfx_Src -> neg_sfx_Src
  //   (src: GPR(RA))
  typedef addi_sfx_Src neg_sfx_Src;
  
  // Operand Alias:  addi_sfx_Trg -> neg_sfx_Trg
  //   (trg: GPR(RT)/p)
  typedef addi_sfx_Trg neg_sfx_Trg;
  
  // Operand Alias:  addme_sfx_Src -> nego_sfx_Src
  //   (src: GPR(RA))
  //   (src: XER[mask 0xc0000000]/p)
  typedef addme_sfx_Src nego_sfx_Src;
  
  // Operand Alias:  addic_sfx_Trg -> nego_sfx_Trg
  //   (trg: GPR(RT)/p)
  //   (trg: XER[mask 0xc0000000]/p)
  typedef addic_sfx_Trg nego_sfx_Trg;
  
  // Operand Alias:  and_sfx_Src -> nor_sfx_Src
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  typedef and_sfx_Src nor_sfx_Src;
  
  // Operand Alias:  and_sfx_Trg -> nor_sfx_Trg
  //   (trg: GPR(RA)/p)
  typedef and_sfx_Trg nor_sfx_Trg;
  
  // Operand Alias:  and_sfx_Src -> or_sfx_Src
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  typedef and_sfx_Src or_sfx_Src;
  
  // Operand Alias:  and_sfx_Trg -> or_sfx_Trg
  //   (trg: GPR(RA)/p)
  typedef and_sfx_Trg or_sfx_Trg;
  
  // Operand Alias:  and_sfx_Src -> orc_sfx_Src
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  typedef and_sfx_Src orc_sfx_Src;
  
  // Operand Alias:  and_sfx_Trg -> orc_sfx_Trg
  //   (trg: GPR(RA)/p)
  typedef and_sfx_Trg orc_sfx_Trg;
  
  // Operand Alias:  cntlzw_sfx_Src -> ori_sfx_Src
  //   (src: GPR(RS))
  typedef cntlzw_sfx_Src ori_sfx_Src;
  
  // Operand Alias:  and_sfx_Trg -> ori_sfx_Trg
  //   (trg: GPR(RA)/p)
  typedef and_sfx_Trg ori_sfx_Trg;
  
  // Operand Alias:  cntlzw_sfx_Src -> oris_sfx_Src
  //   (src: GPR(RS))
  typedef cntlzw_sfx_Src oris_sfx_Src;
  
  // Operand Alias:  and_sfx_Trg -> oris_sfx_Trg
  //   (trg: GPR(RA)/p)
  typedef and_sfx_Trg oris_sfx_Trg;
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: GPR(RS))
  struct rlwimi_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
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
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
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
      read(uarch,ic,logger);
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
  
  // Operand Alias:  and_sfx_Trg -> rlwimi_sfx_Trg
  //   (trg: GPR(RA)/p)
  typedef and_sfx_Trg rlwimi_sfx_Trg;
  
  // Operand Alias:  cntlzw_sfx_Src -> rlwinm_sfx_Src
  //   (src: GPR(RS))
  typedef cntlzw_sfx_Src rlwinm_sfx_Src;
  
  // Operand Alias:  and_sfx_Trg -> rlwinm_sfx_Trg
  //   (trg: GPR(RA)/p)
  typedef and_sfx_Trg rlwinm_sfx_Trg;
  
  // Operands:
  //   (src: GPR(RB)[mask 0x0000001f]/p)
  //   (src: GPR(RS))
  struct rlwnm_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read", ic._masks[2])) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read", (ic._masks[2] & ~mask).uint32())) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
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
      read(uarch,ic,logger);
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
        case 0: return ic._masks[2];
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
  
  // Operand Alias:  and_sfx_Trg -> rlwnm_sfx_Trg
  //   (trg: GPR(RA)/p)
  typedef and_sfx_Trg rlwnm_sfx_Trg;
  
  // Operands:
  //   (src: GPR(RB)[mask 0x0000003f]/p)
  //   (src: GPR(RS))
  struct slw_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read", ic._masks[3])) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read", (ic._masks[3] & ~mask).uint32())) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
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
      read(uarch,ic,logger);
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
        case 0: return ic._masks[3];
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
  
  // Operand Alias:  and_sfx_Trg -> slw_sfx_Trg
  //   (trg: GPR(RA)/p)
  typedef and_sfx_Trg slw_sfx_Trg;
  
  // Operand Alias:  slw_sfx_Src -> sraw_sfx_Src
  //   (src: GPR(RB)[mask 0x0000003f]/p)
  //   (src: GPR(RS))
  typedef slw_sfx_Src sraw_sfx_Src;
  
  // Operands:
  //   (trg: GPR(RA)/p)
  //   (trg: XER[mask 0x20000000]/p)
  struct sraw_sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[4])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
          return true;
        case 1: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[4] & ~mask).uint32())) return false;
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
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(0)._index);
      uarch.XER.allocate(uarch,ic,logger, ic._masks[4]);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(0)._index);
      uarch.XER.deallocate(uarch,ic,logger, ic._masks[4]);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).XER.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
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
        case 1: return ic._masks[4];
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
  
  // Operand Alias:  cntlzw_sfx_Src -> srawi_sfx_Src
  //   (src: GPR(RS))
  typedef cntlzw_sfx_Src srawi_sfx_Src;
  
  // Operands:
  //   (trg: GPR(RA)/p)
  //   (trg: XER[mask 0x20000000]/p)
  struct srawi_sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
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
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
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
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(0)._index);
      uarch.XER.allocate(uarch,ic,logger, ic._masks[0]);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(0)._index);
      uarch.XER.deallocate(uarch,ic,logger, ic._masks[0]);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).XER.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
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
        case 1: return ic._masks[0];
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
  
  // Operand Alias:  slw_sfx_Src -> srw_sfx_Src
  //   (src: GPR(RB)[mask 0x0000003f]/p)
  //   (src: GPR(RS))
  typedef slw_sfx_Src srw_sfx_Src;
  
  // Operand Alias:  and_sfx_Trg -> srw_sfx_Trg
  //   (trg: GPR(RA)/p)
  typedef and_sfx_Trg srw_sfx_Trg;
  
  // Operand Alias:  add_sfx_Src -> subf_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef add_sfx_Src subf_sfx_Src;
  
  // Operand Alias:  add_sfx_Trg -> subf_sfx_Trg
  //   (trg: GPR(RT)/p)
  typedef add_sfx_Trg subf_sfx_Trg;
  
  // Operand Alias:  add_sfx_Src -> subfc_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef add_sfx_Src subfc_sfx_Src;
  
  // Operand Alias:  addc_sfx_Trg -> subfc_sfx_Trg
  //   (trg: GPR(RT)/p)
  //   (trg: XER[mask 0x20000000]/p)
  typedef addc_sfx_Trg subfc_sfx_Trg;
  
  // Operand Alias:  addco_sfx_Src -> subfco_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: XER[mask 0xc0000000]/p)
  typedef addco_sfx_Src subfco_sfx_Src;
  
  // Operand Alias:  addc_sfx_Trg -> subfco_sfx_Trg
  //   (trg: GPR(RT)/p)
  //   (trg: XER[mask 0xe0000000]/p)
  typedef addc_sfx_Trg subfco_sfx_Trg;
  
  // Operand Alias:  addco_sfx_Src -> subfe_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: XER[mask 0x20000000]/p)
  typedef addco_sfx_Src subfe_sfx_Src;
  
  // Operand Alias:  addc_sfx_Trg -> subfe_sfx_Trg
  //   (trg: GPR(RT)/p)
  //   (trg: XER[mask 0x20000000]/p)
  typedef addc_sfx_Trg subfe_sfx_Trg;
  
  // Operand Alias:  addco_sfx_Src -> subfeo_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: XER[mask 0xe0000000]/p)
  typedef addco_sfx_Src subfeo_sfx_Src;
  
  // Operand Alias:  addc_sfx_Trg -> subfeo_sfx_Trg
  //   (trg: GPR(RT)/p)
  //   (trg: XER[mask 0xe0000000]/p)
  typedef addc_sfx_Trg subfeo_sfx_Trg;
  
  // Operand Alias:  addi_sfx_Src -> subfic_sfx_Src
  //   (src: GPR(RA))
  typedef addi_sfx_Src subfic_sfx_Src;
  
  // Operand Alias:  addic_sfx_Trg -> subfic_sfx_Trg
  //   (trg: GPR(RT)/p)
  //   (trg: XER[mask 0x20000000]/p)
  typedef addic_sfx_Trg subfic_sfx_Trg;
  
  // Operand Alias:  addme_sfx_Src -> subfme_sfx_Src
  //   (src: GPR(RA))
  //   (src: XER[mask 0x20000000]/p)
  typedef addme_sfx_Src subfme_sfx_Src;
  
  // Operand Alias:  addic_sfx_Trg -> subfme_sfx_Trg
  //   (trg: GPR(RT)/p)
  //   (trg: XER[mask 0x20000000]/p)
  typedef addic_sfx_Trg subfme_sfx_Trg;
  
  // Operand Alias:  addme_sfx_Src -> subfmeo_sfx_Src
  //   (src: GPR(RA))
  //   (src: XER[mask 0xe0000000]/p)
  typedef addme_sfx_Src subfmeo_sfx_Src;
  
  // Operand Alias:  addic_sfx_Trg -> subfmeo_sfx_Trg
  //   (trg: GPR(RT)/p)
  //   (trg: XER[mask 0xe0000000]/p)
  typedef addic_sfx_Trg subfmeo_sfx_Trg;
  
  // Operand Alias:  addco_sfx_Src -> subfo_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: XER[mask 0xc0000000]/p)
  typedef addco_sfx_Src subfo_sfx_Src;
  
  // Operand Alias:  addc_sfx_Trg -> subfo_sfx_Trg
  //   (trg: GPR(RT)/p)
  //   (trg: XER[mask 0xc0000000]/p)
  typedef addc_sfx_Trg subfo_sfx_Trg;
  
  // Operand Alias:  addme_sfx_Src -> subfze_sfx_Src
  //   (src: GPR(RA))
  //   (src: XER[mask 0x20000000]/p)
  typedef addme_sfx_Src subfze_sfx_Src;
  
  // Operand Alias:  addic_sfx_Trg -> subfze_sfx_Trg
  //   (trg: GPR(RT)/p)
  //   (trg: XER[mask 0x20000000]/p)
  typedef addic_sfx_Trg subfze_sfx_Trg;
  
  // Operand Alias:  addme_sfx_Src -> subfzeo_sfx_Src
  //   (src: GPR(RA))
  //   (src: XER[mask 0xe0000000]/p)
  typedef addme_sfx_Src subfzeo_sfx_Src;
  
  // Operand Alias:  addic_sfx_Trg -> subfzeo_sfx_Trg
  //   (trg: GPR(RT)/p)
  //   (trg: XER[mask 0xe0000000]/p)
  typedef addic_sfx_Trg subfzeo_sfx_Trg;
  
  // Operand Alias:  and_sfx_Src -> xor_sfx_Src
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  typedef and_sfx_Src xor_sfx_Src;
  
  // Operand Alias:  and_sfx_Trg -> xor_sfx_Trg
  //   (trg: GPR(RA)/p)
  typedef and_sfx_Trg xor_sfx_Trg;
  
  // Operand Alias:  cntlzw_sfx_Src -> xori_sfx_Src
  //   (src: GPR(RS))
  typedef cntlzw_sfx_Src xori_sfx_Src;
  
  // Operand Alias:  and_sfx_Trg -> xori_sfx_Trg
  //   (trg: GPR(RA)/p)
  typedef and_sfx_Trg xori_sfx_Trg;
  
  // Operand Alias:  cntlzw_sfx_Src -> xoris_sfx_Src
  //   (src: GPR(RS))
  typedef cntlzw_sfx_Src xoris_sfx_Src;
  
  // Operand Alias:  and_sfx_Trg -> xoris_sfx_Trg
  //   (trg: GPR(RA)/p)
  typedef and_sfx_Trg xoris_sfx_Trg;
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RT))
  //   (src: XER[mask 0x80000000]/p)
  struct add__sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
          return true;
        case 2: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
          return true;
        case 3: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
        break;
        case 3: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
        break;
        case 3: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
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
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(3)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read"))) return false;
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
        if (!can_read(uarch,ic,logger,3,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
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
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        case 2: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 3: return static_cast<OSM_P&>(uarch).XER.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        case 2: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 3: return static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
        case 2: return 0x0;
        case 3: return ic._masks[0];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(1).uint32();
        case 1: return ic._instr->write_mask(2).uint32();
        case 2: return ic._instr->write_mask(3).uint32();
        case 3: return ic._instr->write_mask(4).uint32();
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
        case 3: ic._instr->write_mask(4) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 3: return 4;
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
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  struct add__sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
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
        RegId id = static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).CR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
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
      uarch.CR.allocate(uarch,ic,logger);
      uarch.GPR.allocate(uarch,ic,logger,ic.op(3)._index);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CR.deallocate(uarch,ic,logger);
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(3)._index);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).CR.name(),-1);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(3)._index);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
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
  
  // Operand Alias:  add__sfx_Src -> addc__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RT))
  //   (src: XER[mask 0x80000000]/p)
  typedef add__sfx_Src addc__sfx_Src;
  
  // Operands:
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0x20000000]/p)
  struct addc__sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
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
        case 0: if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
          return true;
        case 2: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint32())) return false;
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
        RegId id = static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).CR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(3)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read"))) return false;
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
      uarch.CR.allocate(uarch,ic,logger);
      uarch.GPR.allocate(uarch,ic,logger,ic.op(3)._index);
      uarch.XER.allocate(uarch,ic,logger, ic._masks[1]);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CR.deallocate(uarch,ic,logger);
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(3)._index);
      uarch.XER.deallocate(uarch,ic,logger, ic._masks[1]);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).CR.name(),-1);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).XER.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 2: return static_cast<OSM_P&>(uarch).XER.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
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
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(3).uint32();
        case 2: return ic._instr->write_mask(4).uint32();
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
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 3;
        case 2: return 4;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 3; };
    
  };
  
  // Operand Alias:  add__sfx_Src -> addco__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RT))
  //   (src: XER[mask 0xc0000000]/p)
  typedef add__sfx_Src addco__sfx_Src;
  
  // Operand Alias:  addc__sfx_Trg -> addco__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0xe0000000]/p)
  typedef addc__sfx_Trg addco__sfx_Trg;
  
  // Operand Alias:  add__sfx_Src -> adde__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RT))
  //   (src: XER[mask 0xa0000000]/p)
  typedef add__sfx_Src adde__sfx_Src;
  
  // Operand Alias:  addc__sfx_Trg -> adde__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0x20000000]/p)
  typedef addc__sfx_Trg adde__sfx_Trg;
  
  // Operand Alias:  add__sfx_Src -> addeo__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RT))
  //   (src: XER[mask 0xe0000000]/p)
  typedef add__sfx_Src addeo__sfx_Src;
  
  // Operand Alias:  addc__sfx_Trg -> addeo__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0xe0000000]/p)
  typedef addc__sfx_Trg addeo__sfx_Trg;
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: GPR(RT))
  //   (src: XER[mask 0x80000000]/p)
  struct addic__sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
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
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
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
      read(uarch,ic,logger);
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
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0x20000000]/p)
  struct addic__sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
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
        case 0: if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
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
        RegId id = static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).CR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CR.can_alloc(uarch,ic,logger,"read"))) return false;
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
      uarch.CR.allocate(uarch,ic,logger);
      uarch.GPR.allocate(uarch,ic,logger,ic.op(2)._index);
      uarch.XER.allocate(uarch,ic,logger, ic._masks[1]);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CR.deallocate(uarch,ic,logger);
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(2)._index);
      uarch.XER.deallocate(uarch,ic,logger, ic._masks[1]);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).CR.name(),-1);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).XER.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
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
        case 0: return 0x0;
        case 1: return 0x0;
        case 2: return ic._masks[1];
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
  
  // Operand Alias:  addic__sfx_Src -> addme__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RT))
  //   (src: XER[mask 0xa0000000]/p)
  typedef addic__sfx_Src addme__sfx_Src;
  
  // Operand Alias:  addic__sfx_Trg -> addme__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0x20000000]/p)
  typedef addic__sfx_Trg addme__sfx_Trg;
  
  // Operand Alias:  addic__sfx_Src -> addmeo__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RT))
  //   (src: XER[mask 0xe0000000]/p)
  typedef addic__sfx_Src addmeo__sfx_Src;
  
  // Operand Alias:  addic__sfx_Trg -> addmeo__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0xe0000000]/p)
  typedef addic__sfx_Trg addmeo__sfx_Trg;
  
  // Operand Alias:  add__sfx_Src -> addo__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RT))
  //   (src: XER[mask 0xc0000000]/p)
  typedef add__sfx_Src addo__sfx_Src;
  
  // Operand Alias:  addc__sfx_Trg -> addo__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0xc0000000]/p)
  typedef addc__sfx_Trg addo__sfx_Trg;
  
  // Operand Alias:  addic__sfx_Src -> addze__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RT))
  //   (src: XER[mask 0xa0000000]/p)
  typedef addic__sfx_Src addze__sfx_Src;
  
  // Operand Alias:  addic__sfx_Trg -> addze__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0x20000000]/p)
  typedef addic__sfx_Trg addze__sfx_Trg;
  
  // Operand Alias:  addic__sfx_Src -> addzeo__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RT))
  //   (src: XER[mask 0xe0000000]/p)
  typedef addic__sfx_Src addzeo__sfx_Src;
  
  // Operand Alias:  addic__sfx_Trg -> addzeo__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0xe0000000]/p)
  typedef addic__sfx_Trg addzeo__sfx_Trg;
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  //   (src: XER[mask 0x80000000]/p)
  struct and__sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
          return true;
        case 2: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
          return true;
        case 3: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
        break;
        case 3: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
        break;
        case 3: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
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
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(3)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read"))) return false;
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
        if (!can_read(uarch,ic,logger,3,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
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
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        case 2: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 3: return static_cast<OSM_P&>(uarch).XER.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        case 2: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 3: return static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
        case 2: return 0x0;
        case 3: return ic._masks[0];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(1).uint32();
        case 1: return ic._instr->write_mask(2).uint32();
        case 2: return ic._instr->write_mask(3).uint32();
        case 3: return ic._instr->write_mask(4).uint32();
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
        case 3: ic._instr->write_mask(4) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 3: return 4;
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
  //   (trg: CR/p)
  //   (trg: GPR(RA))
  struct and__sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
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
        case 0: if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
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
        RegId id = static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).CR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CR.can_alloc(uarch,ic,logger,"read"))) return false;
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
      uarch.CR.allocate(uarch,ic,logger);
      uarch.GPR.allocate(uarch,ic,logger,ic.op(1)._index);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CR.deallocate(uarch,ic,logger);
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(1)._index);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).CR.name(),-1);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
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
  
  // Operand Alias:  and__sfx_Src -> andc__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  //   (src: XER[mask 0x80000000]/p)
  typedef and__sfx_Src andc__sfx_Src;
  
  // Operand Alias:  and__sfx_Trg -> andc__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RA))
  typedef and__sfx_Trg andc__sfx_Trg;
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: GPR(RS))
  //   (src: XER[mask 0x80000000]/p)
  struct andi__sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
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
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
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
      read(uarch,ic,logger);
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
  
  // Operand Alias:  and__sfx_Trg -> andi__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RA))
  typedef and__sfx_Trg andi__sfx_Trg;
  
  // Operand Alias:  andi__sfx_Src -> andis__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RS))
  //   (src: XER[mask 0x80000000]/p)
  typedef andi__sfx_Src andis__sfx_Src;
  
  // Operand Alias:  and__sfx_Trg -> andis__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RA))
  typedef and__sfx_Trg andis__sfx_Trg;
  
  // Operands:
  //   (src: GPR(RS))
  //   (src: XER[mask 0x80000000]/p)
  struct cntlzw__sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[2])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
          return true;
        case 1: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[2] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
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
      read(uarch,ic,logger);
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
        case 1: return ic._masks[2];
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
  
  // Operand Alias:  and__sfx_Trg -> cntlzw__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RA)/p)
  typedef and__sfx_Trg cntlzw__sfx_Trg;
  
  // Operand Alias:  and__sfx_Src -> eqv__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  //   (src: XER[mask 0x80000000]/p)
  typedef and__sfx_Src eqv__sfx_Src;
  
  // Operand Alias:  and__sfx_Trg -> eqv__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RA))
  typedef and__sfx_Trg eqv__sfx_Trg;
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: GPR(RS)[mask 0x000000ff]/p)
  //   (src: XER[mask 0x80000000]/p)
  struct extsb__sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read", ic._masks[3])) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[4])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read", (ic._masks[3] & ~mask).uint32())) return false;
          return true;
        case 2: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[4] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
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
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index));
          mask |= uarch.GPR.rm_later_partial_alloc(uarch,ic.op(2)._index,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index));
          mask |= uarch.GPR.rm_later_partial_alloc(uarch,ic.op(2)._index,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,1,mask))return false;
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
      read(uarch,ic,logger);
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
        case 1: return ic._masks[3];
        case 2: return ic._masks[4];
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
  
  // Operand Alias:  and__sfx_Trg -> extsb__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RA))
  typedef and__sfx_Trg extsb__sfx_Trg;
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: GPR(RS)[mask 0x0000ffff]/p)
  //   (src: XER[mask 0x80000000]/p)
  struct extsh__sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read", ic._masks[5])) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[6])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read", (ic._masks[5] & ~mask).uint32())) return false;
          return true;
        case 2: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[6] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
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
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index));
          mask |= uarch.GPR.rm_later_partial_alloc(uarch,ic.op(2)._index,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index));
          mask |= uarch.GPR.rm_later_partial_alloc(uarch,ic.op(2)._index,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,1,mask))return false;
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
      read(uarch,ic,logger);
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
        case 1: return ic._masks[5];
        case 2: return ic._masks[6];
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
  
  // Operand Alias:  and__sfx_Trg -> extsh__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RA))
  typedef and__sfx_Trg extsh__sfx_Trg;
  
  // Operand Alias:  and__sfx_Src -> nand__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  //   (src: XER[mask 0x80000000]/p)
  typedef and__sfx_Src nand__sfx_Src;
  
  // Operand Alias:  and__sfx_Trg -> nand__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RA))
  typedef and__sfx_Trg nand__sfx_Trg;
  
  // Operand Alias:  addic__sfx_Src -> neg__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RT))
  //   (src: XER[mask 0x80000000]/p)
  typedef addic__sfx_Src neg__sfx_Src;
  
  // Operands:
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  struct neg__sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
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
        RegId id = static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).CR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CR.can_alloc(uarch,ic,logger,"read"))) return false;
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
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CR.allocate(uarch,ic,logger);
      uarch.GPR.allocate(uarch,ic,logger,ic.op(2)._index);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CR.deallocate(uarch,ic,logger);
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(2)._index);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).CR.name(),-1);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(2)._index);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
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
  
  // Operand Alias:  addic__sfx_Src -> nego__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RT))
  //   (src: XER[mask 0xc0000000]/p)
  typedef addic__sfx_Src nego__sfx_Src;
  
  // Operand Alias:  addic__sfx_Trg -> nego__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0xc0000000]/p)
  typedef addic__sfx_Trg nego__sfx_Trg;
  
  // Operand Alias:  and__sfx_Src -> nor__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  //   (src: XER[mask 0x80000000]/p)
  typedef and__sfx_Src nor__sfx_Src;
  
  // Operand Alias:  and__sfx_Trg -> nor__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RA))
  typedef and__sfx_Trg nor__sfx_Trg;
  
  // Operand Alias:  and__sfx_Src -> or__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  //   (src: XER[mask 0x80000000]/p)
  typedef and__sfx_Src or__sfx_Src;
  
  // Operand Alias:  and__sfx_Trg -> or__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RA))
  typedef and__sfx_Trg or__sfx_Trg;
  
  // Operand Alias:  and__sfx_Src -> orc__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  //   (src: XER[mask 0x80000000]/p)
  typedef and__sfx_Src orc__sfx_Src;
  
  // Operand Alias:  and__sfx_Trg -> orc__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RA))
  typedef and__sfx_Trg orc__sfx_Trg;
  
  // Operand Alias:  andi__sfx_Src -> rlwimi__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RS))
  //   (src: XER[mask 0x80000000]/p)
  typedef andi__sfx_Src rlwimi__sfx_Src;
  
  // Operand Alias:  and__sfx_Trg -> rlwimi__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RA))
  typedef and__sfx_Trg rlwimi__sfx_Trg;
  
  // Operand Alias:  andi__sfx_Src -> rlwinm__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RS))
  //   (src: XER[mask 0x80000000]/p)
  typedef andi__sfx_Src rlwinm__sfx_Src;
  
  // Operand Alias:  and__sfx_Trg -> rlwinm__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RA))
  typedef and__sfx_Trg rlwinm__sfx_Trg;
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: GPR(RB)[mask 0x0000001f]/p)
  //   (src: GPR(RS))
  //   (src: XER[mask 0x80000000]/p)
  struct rlwnm__sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read", ic._masks[7])) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[8])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read", (ic._masks[7] & ~mask).uint32())) return false;
          return true;
        case 2: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
          return true;
        case 3: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[8] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
        break;
        case 3: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
        break;
        case 3: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
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
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index));
          mask |= uarch.GPR.rm_later_partial_alloc(uarch,ic.op(2)._index,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index));
          mask |= uarch.GPR.rm_later_partial_alloc(uarch,ic.op(2)._index,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,1,mask))return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(3)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read"))) return false;
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
        if (!can_read(uarch,ic,logger,3,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
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
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        case 2: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 3: return static_cast<OSM_P&>(uarch).XER.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        case 2: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 3: return static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return ic._masks[7];
        case 2: return 0x0;
        case 3: return ic._masks[8];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(1).uint32();
        case 1: return ic._instr->write_mask(2).uint32();
        case 2: return ic._instr->write_mask(3).uint32();
        case 3: return ic._instr->write_mask(4).uint32();
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
        case 3: ic._instr->write_mask(4) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 3: return 4;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 4; };
    
  };
  
  // Operand Alias:  and__sfx_Trg -> rlwnm__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RA))
  typedef and__sfx_Trg rlwnm__sfx_Trg;
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: GPR(RB)[mask 0x0000003f]/p)
  //   (src: GPR(RS))
  //   (src: XER[mask 0x80000000]/p)
  struct slw__sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read", ic._masks[9])) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[10])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read", (ic._masks[9] & ~mask).uint32())) return false;
          return true;
        case 2: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
          return true;
        case 3: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[10] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
        break;
        case 3: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
        break;
        case 3: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
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
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index));
          mask |= uarch.GPR.rm_later_partial_alloc(uarch,ic.op(2)._index,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index));
          mask |= uarch.GPR.rm_later_partial_alloc(uarch,ic.op(2)._index,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,1,mask))return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(3)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read"))) return false;
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
        if (!can_read(uarch,ic,logger,3,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
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
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        case 2: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 3: return static_cast<OSM_P&>(uarch).XER.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        case 2: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 3: return static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return ic._masks[9];
        case 2: return 0x0;
        case 3: return ic._masks[10];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(1).uint32();
        case 1: return ic._instr->write_mask(2).uint32();
        case 2: return ic._instr->write_mask(3).uint32();
        case 3: return ic._instr->write_mask(4).uint32();
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
        case 3: ic._instr->write_mask(4) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 3: return 4;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 4; };
    
  };
  
  // Operand Alias:  and__sfx_Trg -> slw__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RA))
  typedef and__sfx_Trg slw__sfx_Trg;
  
  // Operand Alias:  slw__sfx_Src -> sraw__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB)[mask 0x0000003f]/p)
  //   (src: GPR(RS))
  //   (src: XER[mask 0x80000000]/p)
  typedef slw__sfx_Src sraw__sfx_Src;
  
  // Operands:
  //   (trg: CR/p)
  //   (trg: GPR(RA))
  //   (trg: XER[mask 0x20000000]/p)
  struct sraw__sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[11])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
        case 2: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[11] & ~mask).uint32())) return false;
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
        RegId id = static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).CR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CR.can_alloc(uarch,ic,logger,"read"))) return false;
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
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CR.allocate(uarch,ic,logger);
      uarch.GPR.allocate(uarch,ic,logger,ic.op(1)._index);
      uarch.XER.allocate(uarch,ic,logger, ic._masks[11]);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CR.deallocate(uarch,ic,logger);
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(1)._index);
      uarch.XER.deallocate(uarch,ic,logger, ic._masks[11]);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).CR.name(),-1);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).XER.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
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
        case 0: return 0x0;
        case 1: return 0x0;
        case 2: return ic._masks[11];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
        case 2: return ic._instr->write_mask(4).uint32();
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
        case 2: ic._instr->write_mask(4) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 1;
        case 2: return 4;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 3; };
    
  };
  
  // Operand Alias:  andi__sfx_Src -> srawi__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RS))
  //   (src: XER[mask 0x80000000]/p)
  typedef andi__sfx_Src srawi__sfx_Src;
  
  // Operands:
  //   (trg: CR/p)
  //   (trg: GPR(RA))
  //   (trg: XER[mask 0x20000000]/p)
  struct srawi__sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[3])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
        case 2: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[3] & ~mask).uint32())) return false;
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
        RegId id = static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).CR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CR.can_alloc(uarch,ic,logger,"read"))) return false;
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
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CR.allocate(uarch,ic,logger);
      uarch.GPR.allocate(uarch,ic,logger,ic.op(1)._index);
      uarch.XER.allocate(uarch,ic,logger, ic._masks[3]);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CR.deallocate(uarch,ic,logger);
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(1)._index);
      uarch.XER.deallocate(uarch,ic,logger, ic._masks[3]);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).CR.name(),-1);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).XER.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
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
        case 0: return 0x0;
        case 1: return 0x0;
        case 2: return ic._masks[3];
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
  
  // Operand Alias:  slw__sfx_Src -> srw__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB)[mask 0x0000003f]/p)
  //   (src: GPR(RS))
  //   (src: XER[mask 0x80000000]/p)
  typedef slw__sfx_Src srw__sfx_Src;
  
  // Operand Alias:  and__sfx_Trg -> srw__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RA))
  typedef and__sfx_Trg srw__sfx_Trg;
  
  // Operand Alias:  add__sfx_Src -> subf__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RT))
  //   (src: XER[mask 0x80000000]/p)
  typedef add__sfx_Src subf__sfx_Src;
  
  // Operand Alias:  add__sfx_Trg -> subf__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  typedef add__sfx_Trg subf__sfx_Trg;
  
  // Operand Alias:  add__sfx_Src -> subfc__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RT))
  //   (src: XER[mask 0x80000000]/p)
  typedef add__sfx_Src subfc__sfx_Src;
  
  // Operand Alias:  addc__sfx_Trg -> subfc__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0x20000000]/p)
  typedef addc__sfx_Trg subfc__sfx_Trg;
  
  // Operand Alias:  add__sfx_Src -> subfco__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RT))
  //   (src: XER[mask 0xc0000000]/p)
  typedef add__sfx_Src subfco__sfx_Src;
  
  // Operand Alias:  addc__sfx_Trg -> subfco__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0xe0000000]/p)
  typedef addc__sfx_Trg subfco__sfx_Trg;
  
  // Operand Alias:  add__sfx_Src -> subfe__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RT))
  //   (src: XER[mask 0xa0000000]/p)
  typedef add__sfx_Src subfe__sfx_Src;
  
  // Operand Alias:  addc__sfx_Trg -> subfe__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0x20000000]/p)
  typedef addc__sfx_Trg subfe__sfx_Trg;
  
  // Operand Alias:  add__sfx_Src -> subfeo__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RT))
  //   (src: XER[mask 0xe0000000]/p)
  typedef add__sfx_Src subfeo__sfx_Src;
  
  // Operand Alias:  addc__sfx_Trg -> subfeo__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0xe0000000]/p)
  typedef addc__sfx_Trg subfeo__sfx_Trg;
  
  // Operand Alias:  addic__sfx_Src -> subfme__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RT))
  //   (src: XER[mask 0xa0000000]/p)
  typedef addic__sfx_Src subfme__sfx_Src;
  
  // Operand Alias:  addic__sfx_Trg -> subfme__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0x20000000]/p)
  typedef addic__sfx_Trg subfme__sfx_Trg;
  
  // Operand Alias:  addic__sfx_Src -> subfmeo__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RT))
  //   (src: XER[mask 0xe0000000]/p)
  typedef addic__sfx_Src subfmeo__sfx_Src;
  
  // Operand Alias:  addic__sfx_Trg -> subfmeo__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0xe0000000]/p)
  typedef addic__sfx_Trg subfmeo__sfx_Trg;
  
  // Operand Alias:  add__sfx_Src -> subfo__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RT))
  //   (src: XER[mask 0xc0000000]/p)
  typedef add__sfx_Src subfo__sfx_Src;
  
  // Operand Alias:  addc__sfx_Trg -> subfo__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0xc0000000]/p)
  typedef addc__sfx_Trg subfo__sfx_Trg;
  
  // Operand Alias:  addic__sfx_Src -> subfze__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RT))
  //   (src: XER[mask 0xa0000000]/p)
  typedef addic__sfx_Src subfze__sfx_Src;
  
  // Operand Alias:  addic__sfx_Trg -> subfze__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0x20000000]/p)
  typedef addic__sfx_Trg subfze__sfx_Trg;
  
  // Operand Alias:  addic__sfx_Src -> subfzeo__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RT))
  //   (src: XER[mask 0xe0000000]/p)
  typedef addic__sfx_Src subfzeo__sfx_Src;
  
  // Operand Alias:  addic__sfx_Trg -> subfzeo__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0xe0000000]/p)
  typedef addic__sfx_Trg subfzeo__sfx_Trg;
  
  // Operand Alias:  and__sfx_Src -> xor__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  //   (src: XER[mask 0x80000000]/p)
  typedef and__sfx_Src xor__sfx_Src;
  
  // Operand Alias:  and__sfx_Trg -> xor__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RA))
  typedef and__sfx_Trg xor__sfx_Trg;
  
  // Operands:
  //   (src: CR)
  //   (src: GPR(RA))
  //   (src: XER[mask 0x80000000]/p)
  struct cmpli_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CR.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CR.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
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
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CR.name(),-1);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CR.name(),-1);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
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
      read(uarch,ic,logger);
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
  //   (trg: CR)
  struct cmpli_sfx_Trg {
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
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).CR.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
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
  //   (src: XER[mask 0x80000000]/p)
  struct cmpi_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
        case 1: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
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
      read(uarch,ic,logger);
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
        case 1: return ic._masks[1];
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
  
  // Operand Alias:  cmpli_sfx_Trg -> cmpi_sfx_Trg
  //   (trg: CR/p)
  typedef cmpli_sfx_Trg cmpi_sfx_Trg;
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: XER[mask 0x80000000]/p)
  struct cmp_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[2])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
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
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
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
      read(uarch,ic,logger);
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
        case 2: return ic._masks[2];
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
  
  // Operand Alias:  cmpli_sfx_Trg -> cmp_sfx_Trg
  //   (trg: CR/p)
  typedef cmpli_sfx_Trg cmp_sfx_Trg;
  
  // Operands:
  //   (src: CR)
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: XER[mask 0x80000000]/p)
  struct cmpl_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[3])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CR.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CR.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
        case 2: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
          return true;
        case 3: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[3] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CR.name(),-1);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 3: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CR.name(),-1);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 3: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
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
        if (!can_read(uarch,ic,logger,3,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
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
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 2: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        case 3: return static_cast<OSM_P&>(uarch).XER.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 2: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        case 3: return static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
        case 2: return 0x0;
        case 3: return ic._masks[3];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
        case 2: return ic._instr->write_mask(2).uint32();
        case 3: return ic._instr->write_mask(3).uint32();
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
        case 3: ic._instr->write_mask(3) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 1;
        case 2: return 2;
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
  
  // Operand Alias:  cmpli_sfx_Trg -> cmpl_sfx_Trg
  //   (trg: CR)
  typedef cmpli_sfx_Trg cmpl_sfx_Trg;
  
  // Operands:
  //   (src: CR/p)
  struct crxor_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CR.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CR.name(),-1);
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
      read(uarch,ic,logger);
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
  
  // Operand Alias:  cmpli_sfx_Trg -> crxor_sfx_Trg
  //   (trg: CR)
  typedef cmpli_sfx_Trg crxor_sfx_Trg;
  
  // Operand Alias:  crxor_sfx_Src -> creqv_sfx_Src
  //   (src: CR/p)
  typedef crxor_sfx_Src creqv_sfx_Src;
  
  // Operand Alias:  cmpli_sfx_Trg -> creqv_sfx_Trg
  //   (trg: CR)
  typedef cmpli_sfx_Trg creqv_sfx_Trg;
  
  // Operand Alias:  addi_sfx_Src -> mulli_sfx_Src
  //   (src: GPR(RA))
  typedef addi_sfx_Src mulli_sfx_Src;
  
  // Operand Alias:  addi_sfx_Trg -> mulli_sfx_Trg
  //   (trg: GPR(RT)/p)
  typedef addi_sfx_Trg mulli_sfx_Trg;
  
  // Operands:
  //   (trg: NIA)
  struct ba_sfx_Trg {
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
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).NIA.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).NIA.name(static_cast<OSM_P&>(uarch),0); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).NIA.id(static_cast<OSM_P&>(uarch),0); }
    
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
  //   (src: CR/p)
  //   (src: CTR?)
  struct bca_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CR.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CTR.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CR.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CTR.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CR.name(),-1);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CTR.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CR.name(),-1);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CTR.name(),-1);
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
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
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
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).CTR.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).CTR.id(static_cast<OSM_P&>(uarch),0);
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
  //   (trg: CTR?)
  //   (trg: NIA?)
  struct bca_sfx_Trg {
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
      if (logger && ic._instr->is_dirty(1))logger->logOpAction("write",static_cast<OSM_P&>(uarch).CTR.name(),-1);
      if (logger && ic._instr->is_dirty(2))logger->logOpAction("write",static_cast<OSM_P&>(uarch).NIA.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(1)._index;
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
  //   (src: CIA)
  //   (src: CR/p)
  //   (src: CTR?)
  struct bcla_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CIA.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CIA.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CR.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CTR.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CIA.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CR.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CTR.name(),-1);
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
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CIA.name(),-1);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CR.name(),-1);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CTR.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CIA.name(),-1);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CR.name(),-1);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CTR.name(),-1);
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
      read(uarch,ic,logger);
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
  //   (trg: LR)
  //   (trg: NIA?)
  struct bcla_sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
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
        case 0: if (!uarch.CTR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.LR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 2: if (!uarch.NIA.can_alloc(uarch,ic,logger,"read")) return false;
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
        RegId id = static_cast<OSM_P&>(uarch).CTR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).CTR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CTR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
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
      uarch.CTR.allocate(uarch,ic,logger);
      uarch.LR.allocate(uarch,ic,logger);
      uarch.NIA.allocate(uarch,ic,logger);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CTR.deallocate(uarch,ic,logger);
      uarch.LR.deallocate(uarch,ic,logger);
      uarch.NIA.deallocate(uarch,ic,logger);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger && ic._instr->is_dirty(2))logger->logOpAction("write",static_cast<OSM_P&>(uarch).CTR.name(),-1);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).LR.name(),-1);
      if (logger && ic._instr->is_dirty(4))logger->logOpAction("write",static_cast<OSM_P&>(uarch).NIA.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(2)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CTR.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).LR.name(static_cast<OSM_P&>(uarch),0);
        case 2: return static_cast<OSM_P&>(uarch).NIA.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CTR.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).LR.id(static_cast<OSM_P&>(uarch),0);
        case 2: return static_cast<OSM_P&>(uarch).NIA.id(static_cast<OSM_P&>(uarch),0);
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
        case 0: return ic._instr->write_mask(2).uint32();
        case 1: return ic._instr->write_mask(3).uint32();
        case 2: return ic._instr->write_mask(4).uint32();
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
        case 2: ic._instr->write_mask(4) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 2;
        case 1: return 3;
        case 2: return 4;
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
  //   (src: CIA)
  struct bla_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CIA.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CIA.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CIA.name(),-1);
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
      read(uarch,ic,logger);
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
  //   (trg: LR)
  //   (trg: NIA)
  struct bla_sfx_Trg {
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
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).LR.name(),-1);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).NIA.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(1)._index;
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
  
  // Operand Alias:  cntlzw_sfx_Src -> cntlzd_sfx_Src
  //   (src: GPR(RS)?)
  typedef cntlzw_sfx_Src cntlzd_sfx_Src;
  
  // Operand Alias:  srawi_sfx_Trg -> cntlzd_sfx_Trg
  //   (trg: GPR(RA)/p?)
  //   (trg: XER[mask 0x80000000]/p?)
  typedef srawi_sfx_Trg cntlzd_sfx_Trg;
  
  // Operand Alias:  cntlzw__sfx_Src -> cntlzd__sfx_Src
  //   (src: GPR(RS)?)
  //   (src: XER[mask 0x80000000]/p)
  typedef cntlzw__sfx_Src cntlzd__sfx_Src;
  
  // Operand Alias:  srawi__sfx_Trg -> cntlzd__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RA)/p?)
  //   (trg: XER[mask 0x80000000]/p?)
  typedef srawi__sfx_Trg cntlzd__sfx_Trg;
  
  // Operand Alias:  crxor_sfx_Src -> crand_sfx_Src
  //   (src: CR/p)
  typedef crxor_sfx_Src crand_sfx_Src;
  
  // Operand Alias:  cmpli_sfx_Trg -> crand_sfx_Trg
  //   (trg: CR)
  typedef cmpli_sfx_Trg crand_sfx_Trg;
  
  // Operand Alias:  crxor_sfx_Src -> crandc_sfx_Src
  //   (src: CR/p)
  typedef crxor_sfx_Src crandc_sfx_Src;
  
  // Operand Alias:  cmpli_sfx_Trg -> crandc_sfx_Trg
  //   (trg: CR)
  typedef cmpli_sfx_Trg crandc_sfx_Trg;
  
  // Operand Alias:  crxor_sfx_Src -> crnand_sfx_Src
  //   (src: CR/p)
  typedef crxor_sfx_Src crnand_sfx_Src;
  
  // Operand Alias:  cmpli_sfx_Trg -> crnand_sfx_Trg
  //   (trg: CR)
  typedef cmpli_sfx_Trg crnand_sfx_Trg;
  
  // Operand Alias:  crxor_sfx_Src -> crnor_sfx_Src
  //   (src: CR/p)
  typedef crxor_sfx_Src crnor_sfx_Src;
  
  // Operand Alias:  cmpli_sfx_Trg -> crnor_sfx_Trg
  //   (trg: CR)
  typedef cmpli_sfx_Trg crnor_sfx_Trg;
  
  // Operand Alias:  crxor_sfx_Src -> cror_sfx_Src
  //   (src: CR/p)
  typedef crxor_sfx_Src cror_sfx_Src;
  
  // Operand Alias:  cmpli_sfx_Trg -> cror_sfx_Trg
  //   (trg: CR)
  typedef cmpli_sfx_Trg cror_sfx_Trg;
  
  // Operand Alias:  crxor_sfx_Src -> crorc_sfx_Src
  //   (src: CR/p)
  typedef crxor_sfx_Src crorc_sfx_Src;
  
  // Operand Alias:  cmpli_sfx_Trg -> crorc_sfx_Trg
  //   (trg: CR)
  typedef cmpli_sfx_Trg crorc_sfx_Trg;
  
  // Operand Alias:  add_sfx_Src -> dcba_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef add_sfx_Src dcba_sfx_Src;
  
  // Operand Alias:  add_sfx_Src -> dcbf_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef add_sfx_Src dcbf_sfx_Src;
  
  // Operand Alias:  add_sfx_Src -> dcbi_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef add_sfx_Src dcbi_sfx_Src;
  
  // Operand Alias:  add_sfx_Src -> dcbst_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef add_sfx_Src dcbst_sfx_Src;
  
  // Operand Alias:  add_sfx_Src -> dcbt_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef add_sfx_Src dcbt_sfx_Src;
  
  // Operand Alias:  add_sfx_Src -> dcbz_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef add_sfx_Src dcbz_sfx_Src;
  
  // Operand Alias:  add_sfx_Src -> divd_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef add_sfx_Src divd_sfx_Src;
  
  // Operand Alias:  add_sfx_Trg -> divd_sfx_Trg
  //   (trg: GPR(RT)/p)
  typedef add_sfx_Trg divd_sfx_Trg;
  
  // Operand Alias:  add__sfx_Src -> divd__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RT))
  //   (src: XER[mask 0x80000000]/p)
  typedef add__sfx_Src divd__sfx_Src;
  
  // Operand Alias:  add__sfx_Trg -> divd__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  typedef add__sfx_Trg divd__sfx_Trg;
  
  // Operand Alias:  addco_sfx_Src -> divdo_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: XER[mask 0xc0000000]/p)
  typedef addco_sfx_Src divdo_sfx_Src;
  
  // Operand Alias:  addc_sfx_Trg -> divdo_sfx_Trg
  //   (trg: GPR(RT)/p)
  //   (trg: XER[mask 0xc0000000])
  typedef addc_sfx_Trg divdo_sfx_Trg;
  
  // Operand Alias:  add__sfx_Src -> divdo__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RT))
  //   (src: XER[mask 0xc0000000]/p)
  typedef add__sfx_Src divdo__sfx_Src;
  
  // Operand Alias:  addc__sfx_Trg -> divdo__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0xc0000000])
  typedef addc__sfx_Trg divdo__sfx_Trg;
  
  // Operand Alias:  add_sfx_Src -> divdu_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef add_sfx_Src divdu_sfx_Src;
  
  // Operand Alias:  add_sfx_Trg -> divdu_sfx_Trg
  //   (trg: GPR(RT)/p)
  typedef add_sfx_Trg divdu_sfx_Trg;
  
  // Operand Alias:  add__sfx_Src -> divdu__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RT))
  //   (src: XER[mask 0x80000000]/p)
  typedef add__sfx_Src divdu__sfx_Src;
  
  // Operand Alias:  add__sfx_Trg -> divdu__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  typedef add__sfx_Trg divdu__sfx_Trg;
  
  // Operand Alias:  addco_sfx_Src -> divduo_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: XER[mask 0xc0000000]/p)
  typedef addco_sfx_Src divduo_sfx_Src;
  
  // Operand Alias:  addc_sfx_Trg -> divduo_sfx_Trg
  //   (trg: GPR(RT)/p)
  //   (trg: XER[mask 0xc0000000])
  typedef addc_sfx_Trg divduo_sfx_Trg;
  
  // Operand Alias:  add__sfx_Src -> divduo__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RT))
  //   (src: XER[mask 0xc0000000]/p)
  typedef add__sfx_Src divduo__sfx_Src;
  
  // Operand Alias:  addc__sfx_Trg -> divduo__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0xc0000000])
  typedef addc__sfx_Trg divduo__sfx_Trg;
  
  // Operand Alias:  add__sfx_Src -> divw__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RT))
  //   (src: XER[mask 0x80000000]/p)
  typedef add__sfx_Src divw__sfx_Src;
  
  // Operand Alias:  add__sfx_Trg -> divw__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  typedef add__sfx_Trg divw__sfx_Trg;
  
  // Operand Alias:  addco_sfx_Src -> divwo_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: XER[mask 0xc0000000]/p)
  typedef addco_sfx_Src divwo_sfx_Src;
  
  // Operand Alias:  addc_sfx_Trg -> divwo_sfx_Trg
  //   (trg: GPR(RT)/p)
  //   (trg: XER[mask 0xc0000000])
  typedef addc_sfx_Trg divwo_sfx_Trg;
  
  // Operand Alias:  add__sfx_Src -> divwo__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RT))
  //   (src: XER[mask 0xc0000000]/p)
  typedef add__sfx_Src divwo__sfx_Src;
  
  // Operand Alias:  addc__sfx_Trg -> divwo__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0xc0000000])
  typedef addc__sfx_Trg divwo__sfx_Trg;
  
  // Operand Alias:  add_sfx_Src -> divwu_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef add_sfx_Src divwu_sfx_Src;
  
  // Operand Alias:  add_sfx_Trg -> divwu_sfx_Trg
  //   (trg: GPR(RT)/p)
  typedef add_sfx_Trg divwu_sfx_Trg;
  
  // Operand Alias:  add__sfx_Src -> divwu__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RT))
  //   (src: XER[mask 0x80000000]/p)
  typedef add__sfx_Src divwu__sfx_Src;
  
  // Operand Alias:  add__sfx_Trg -> divwu__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  typedef add__sfx_Trg divwu__sfx_Trg;
  
  // Operand Alias:  addco_sfx_Src -> divwuo_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: XER[mask 0xc0000000]/p)
  typedef addco_sfx_Src divwuo_sfx_Src;
  
  // Operand Alias:  addc_sfx_Trg -> divwuo_sfx_Trg
  //   (trg: GPR(RT)/p)
  //   (trg: XER[mask 0xc0000000])
  typedef addc_sfx_Trg divwuo_sfx_Trg;
  
  // Operand Alias:  add__sfx_Src -> divwuo__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RT))
  //   (src: XER[mask 0xc0000000]/p)
  typedef add__sfx_Src divwuo__sfx_Src;
  
  // Operand Alias:  addc__sfx_Trg -> divwuo__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0xc0000000])
  typedef addc__sfx_Trg divwuo__sfx_Trg;
  
  // Operand Alias:  cntlzw_sfx_Src -> extsw_sfx_Src
  //   (src: GPR(RS)/p)
  typedef cntlzw_sfx_Src extsw_sfx_Src;
  
  // Operand Alias:  and_sfx_Trg -> extsw_sfx_Trg
  //   (trg: GPR(RA)/p)
  typedef and_sfx_Trg extsw_sfx_Trg;
  
  // Operand Alias:  andi__sfx_Src -> extsw__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RS)/p)
  //   (src: XER[mask 0x80000000]/p)
  typedef andi__sfx_Src extsw__sfx_Src;
  
  // Operand Alias:  and__sfx_Trg -> extsw__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RA))
  typedef and__sfx_Trg extsw__sfx_Trg;
  
  // Operand Alias:  add_sfx_Src -> icbi_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef add_sfx_Src icbi_sfx_Src;
  
  // Operand Alias:  add_sfx_Src -> icbt_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef add_sfx_Src icbt_sfx_Src;
  
  // Operand Alias:  addi_sfx_Src -> ld_sfx_Src
  //   (src: GPR(RA))
  typedef addi_sfx_Src ld_sfx_Src;
  
  // Operand Alias:  addi_sfx_Trg -> ld_sfx_Trg
  //   (trg: GPR(RT)/p)
  typedef addi_sfx_Trg ld_sfx_Trg;
  
  // Operand Alias:  addi_sfx_Src -> ldu_sfx_Src
  //   (src: GPR(RA))
  typedef addi_sfx_Src ldu_sfx_Src;
  
  // Operands:
  //   (trg: GPR(RA)/p)
  //   (trg: GPR(RT)/p)
  struct ldu_sfx_Trg {
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
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
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
  
  // Operand Alias:  add_sfx_Src -> ldux_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef add_sfx_Src ldux_sfx_Src;
  
  // Operands:
  //   (trg: GPR(RA)/p)
  //   (trg: GPR(RT)/p)
  struct ldux_sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
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
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
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
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
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
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.allocate(uarch,ic,logger,ic.op(0)._index);
      uarch.GPR.allocate(uarch,ic,logger,ic.op(2)._index);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(0)._index);
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(2)._index);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(2)._index);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
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
  
  // Operand Alias:  add_sfx_Src -> ldx_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef add_sfx_Src ldx_sfx_Src;
  
  // Operand Alias:  add_sfx_Trg -> ldx_sfx_Trg
  //   (trg: GPR(RT)/p)
  typedef add_sfx_Trg ldx_sfx_Trg;
  
  // Operand Alias:  addi_sfx_Src -> lhau_sfx_Src
  //   (src: GPR(RA))
  typedef addi_sfx_Src lhau_sfx_Src;
  
  // Operand Alias:  ldu_sfx_Trg -> lhau_sfx_Trg
  //   (trg: GPR(RA)/p)
  //   (trg: GPR(RT)/p)
  typedef ldu_sfx_Trg lhau_sfx_Trg;
  
  // Operand Alias:  add_sfx_Src -> lhaux_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef add_sfx_Src lhaux_sfx_Src;
  
  // Operand Alias:  ldux_sfx_Trg -> lhaux_sfx_Trg
  //   (trg: GPR(RA)/p)
  //   (trg: GPR(RT)/p)
  typedef ldux_sfx_Trg lhaux_sfx_Trg;
  
  // Operand Alias:  add_sfx_Src -> lhbrx_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef add_sfx_Src lhbrx_sfx_Src;
  
  // Operand Alias:  add_sfx_Trg -> lhbrx_sfx_Trg
  //   (trg: GPR(RT)/p)
  typedef add_sfx_Trg lhbrx_sfx_Trg;
  
  // Operand Alias:  addi_sfx_Src -> lwa_sfx_Src
  //   (src: GPR(RA))
  typedef addi_sfx_Src lwa_sfx_Src;
  
  // Operand Alias:  addi_sfx_Trg -> lwa_sfx_Trg
  //   (trg: GPR(RT)/p)
  typedef addi_sfx_Trg lwa_sfx_Trg;
  
  // Operand Alias:  add_sfx_Src -> lwarx_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef add_sfx_Src lwarx_sfx_Src;
  
  // Operands:
  //   (trg: GPR(RT)/p)
  //   (trg: RESRV)
  struct lwarx_sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      if (!uarch.RESRV.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
          return true;
        case 1: if (!uarch.RESRV.can_alloc(uarch,ic,logger,"read")) return false;
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
        RegId id = static_cast<OSM_P&>(uarch).RESRV.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).RESRV.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.RESRV.can_alloc(uarch,ic,logger,"read"))) return false;
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
      uarch.RESRV.allocate(uarch,ic,logger);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(2)._index);
      uarch.RESRV.deallocate(uarch,ic,logger);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).RESRV.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(2)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        case 1: return static_cast<OSM_P&>(uarch).RESRV.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        case 1: return static_cast<OSM_P&>(uarch).RESRV.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<64> mask(Inst_P &ic,unsigned sub_op = 0) const {
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
  
  // Operand Alias:  add_sfx_Src -> lwaux_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef add_sfx_Src lwaux_sfx_Src;
  
  // Operand Alias:  ldux_sfx_Trg -> lwaux_sfx_Trg
  //   (trg: GPR(RA)/p)
  //   (trg: GPR(RT)/p)
  typedef ldux_sfx_Trg lwaux_sfx_Trg;
  
  // Operand Alias:  add_sfx_Src -> lwax_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef add_sfx_Src lwax_sfx_Src;
  
  // Operand Alias:  add_sfx_Trg -> lwax_sfx_Trg
  //   (trg: GPR(RT)/p)
  typedef add_sfx_Trg lwax_sfx_Trg;
  
  // Operand Alias:  add_sfx_Src -> lwbrx_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef add_sfx_Src lwbrx_sfx_Src;
  
  // Operand Alias:  add_sfx_Trg -> lwbrx_sfx_Trg
  //   (trg: GPR(RT)/p)
  typedef add_sfx_Trg lwbrx_sfx_Trg;
  
  // Operand Alias:  crxor_sfx_Src -> mcrf_sfx_Src
  //   (src: CR/p)
  typedef crxor_sfx_Src mcrf_sfx_Src;
  
  // Operand Alias:  cmpli_sfx_Trg -> mcrf_sfx_Trg
  //   (trg: CR)
  typedef cmpli_sfx_Trg mcrf_sfx_Trg;
  
  // Operands:
  //   (src: XER[mask 0xf0000000]/p)
  struct mcrxr_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,const bits<32> &mask) {
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
      return true;
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
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
        if (!can_read(uarch,ic,logger,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
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
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).XER.name(static_cast<OSM_P&>(uarch),0); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return ic._masks[0]; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(1).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(1) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 1; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (trg: CR/p)
  //   (trg: XER[mask 0xf0000000]/p)
  struct mcrxr_sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
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
        case 0: if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
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
        RegId id = static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).CR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CR.can_alloc(uarch,ic,logger,"read"))) return false;
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
      uarch.CR.allocate(uarch,ic,logger);
      uarch.XER.allocate(uarch,ic,logger, ic._masks[1]);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CR.deallocate(uarch,ic,logger);
      uarch.XER.deallocate(uarch,ic,logger, ic._masks[1]);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).CR.name(),-1);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).XER.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
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
        case 0: return 0x0;
        case 1: return ic._masks[1];
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
  
  // Operand Alias:  crxor_sfx_Src -> mfcr_sfx_Src
  //   (src: CR)
  typedef crxor_sfx_Src mfcr_sfx_Src;
  
  // Operand Alias:  addi_sfx_Trg -> mfcr_sfx_Trg
  //   (trg: GPR(RT)/p)
  typedef addi_sfx_Trg mfcr_sfx_Trg;
  
  // Operands:
  //   (src: MSR)
  struct mfmsr_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.MSR.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).MSR.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).MSR.name(),-1);
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
      read(uarch,ic,logger);
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
  
  // Operands:
  //   (trg: GPR(RT)/p)
  struct mfmsr_sfx_Trg {
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
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
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
  //   (src: CR?)
  //   (src: GPR(RS)/p?)
  struct mtcrf_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CR.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CR.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CR.name(),-1);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CR.name(),-1);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
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
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
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
  
  // Operand Alias:  cmpli_sfx_Trg -> mtcrf_sfx_Trg
  //   (trg: CR?)
  typedef cmpli_sfx_Trg mtcrf_sfx_Trg;
  
  // Operands:
  //   (src: GPR(RS))
  struct mtmsr_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
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
      read(uarch,ic,logger);
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
  struct mtmsr_sfx_Trg {
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
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).MSR.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(1)._index;
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
  
  // Operand Alias:  mtcrf_sfx_Src -> mtocrf_sfx_Src
  //   (src: CR?)
  //   (src: GPR(RS)/p?)
  typedef mtcrf_sfx_Src mtocrf_sfx_Src;
  
  // Operand Alias:  cmpli_sfx_Trg -> mtocrf_sfx_Trg
  //   (trg: CR?)
  typedef cmpli_sfx_Trg mtocrf_sfx_Trg;
  
  // Operand Alias:  add_sfx_Src -> mulhd_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef add_sfx_Src mulhd_sfx_Src;
  
  // Operand Alias:  add_sfx_Trg -> mulhd_sfx_Trg
  //   (trg: GPR(RT)/p)
  typedef add_sfx_Trg mulhd_sfx_Trg;
  
  // Operand Alias:  add__sfx_Src -> mulhd__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RT))
  //   (src: XER[mask 0x80000000]/p)
  typedef add__sfx_Src mulhd__sfx_Src;
  
  // Operand Alias:  add__sfx_Trg -> mulhd__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  typedef add__sfx_Trg mulhd__sfx_Trg;
  
  // Operand Alias:  add_sfx_Src -> mulhdu_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef add_sfx_Src mulhdu_sfx_Src;
  
  // Operand Alias:  add_sfx_Trg -> mulhdu_sfx_Trg
  //   (trg: GPR(RT)/p)
  typedef add_sfx_Trg mulhdu_sfx_Trg;
  
  // Operand Alias:  add__sfx_Src -> mulhdu__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RT))
  //   (src: XER[mask 0x80000000]/p)
  typedef add__sfx_Src mulhdu__sfx_Src;
  
  // Operand Alias:  add__sfx_Trg -> mulhdu__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  typedef add__sfx_Trg mulhdu__sfx_Trg;
  
  // Operand Alias:  add__sfx_Src -> mulhw__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RT))
  //   (src: XER[mask 0x80000000]/p)
  typedef add__sfx_Src mulhw__sfx_Src;
  
  // Operand Alias:  add__sfx_Trg -> mulhw__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  typedef add__sfx_Trg mulhw__sfx_Trg;
  
  // Operand Alias:  add_sfx_Src -> mulhwu_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef add_sfx_Src mulhwu_sfx_Src;
  
  // Operand Alias:  add_sfx_Trg -> mulhwu_sfx_Trg
  //   (trg: GPR(RT)/p)
  typedef add_sfx_Trg mulhwu_sfx_Trg;
  
  // Operand Alias:  add__sfx_Src -> mulhwu__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RT))
  //   (src: XER[mask 0x80000000]/p)
  typedef add__sfx_Src mulhwu__sfx_Src;
  
  // Operand Alias:  add__sfx_Trg -> mulhwu__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  typedef add__sfx_Trg mulhwu__sfx_Trg;
  
  // Operand Alias:  add_sfx_Src -> mulld_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef add_sfx_Src mulld_sfx_Src;
  
  // Operand Alias:  add_sfx_Trg -> mulld_sfx_Trg
  //   (trg: GPR(RT)/p)
  typedef add_sfx_Trg mulld_sfx_Trg;
  
  // Operand Alias:  add__sfx_Src -> mulld__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RT))
  //   (src: XER[mask 0x80000000]/p)
  typedef add__sfx_Src mulld__sfx_Src;
  
  // Operand Alias:  add__sfx_Trg -> mulld__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  typedef add__sfx_Trg mulld__sfx_Trg;
  
  // Operand Alias:  addco_sfx_Src -> mulldo_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: XER[mask 0xc0000000]/p)
  typedef addco_sfx_Src mulldo_sfx_Src;
  
  // Operand Alias:  addc_sfx_Trg -> mulldo_sfx_Trg
  //   (trg: GPR(RT)/p)
  //   (trg: XER[mask 0xc0000000])
  typedef addc_sfx_Trg mulldo_sfx_Trg;
  
  // Operand Alias:  add__sfx_Src -> mulldo__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RT))
  //   (src: XER[mask 0xc0000000]/p)
  typedef add__sfx_Src mulldo__sfx_Src;
  
  // Operand Alias:  addc__sfx_Trg -> mulldo__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0xc0000000])
  typedef addc__sfx_Trg mulldo__sfx_Trg;
  
  // Operand Alias:  slw_sfx_Src -> rldcl_sfx_Src
  //   (src: GPR(RB)[mask 0x0000003f]/p)
  //   (src: GPR(RS))
  typedef slw_sfx_Src rldcl_sfx_Src;
  
  // Operand Alias:  and_sfx_Trg -> rldcl_sfx_Trg
  //   (trg: GPR(RA)/p)
  typedef and_sfx_Trg rldcl_sfx_Trg;
  
  // Operand Alias:  slw__sfx_Src -> rldcl__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB)[mask 0x0000003f]/p)
  //   (src: GPR(RS))
  //   (src: XER[mask 0x80000000]/p)
  typedef slw__sfx_Src rldcl__sfx_Src;
  
  // Operand Alias:  and__sfx_Trg -> rldcl__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RA))
  typedef and__sfx_Trg rldcl__sfx_Trg;
  
  // Operand Alias:  slw_sfx_Src -> rldcr_sfx_Src
  //   (src: GPR(RB)[mask 0x0000003f]/p)
  //   (src: GPR(RS))
  typedef slw_sfx_Src rldcr_sfx_Src;
  
  // Operand Alias:  and_sfx_Trg -> rldcr_sfx_Trg
  //   (trg: GPR(RA)/p)
  typedef and_sfx_Trg rldcr_sfx_Trg;
  
  // Operand Alias:  slw__sfx_Src -> rldcr__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB)[mask 0x0000003f]/p)
  //   (src: GPR(RS))
  //   (src: XER[mask 0x80000000]/p)
  typedef slw__sfx_Src rldcr__sfx_Src;
  
  // Operand Alias:  and__sfx_Trg -> rldcr__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RA))
  typedef and__sfx_Trg rldcr__sfx_Trg;
  
  // Operand Alias:  cntlzw_sfx_Src -> rldic_sfx_Src
  //   (src: GPR(RS))
  typedef cntlzw_sfx_Src rldic_sfx_Src;
  
  // Operand Alias:  and_sfx_Trg -> rldic_sfx_Trg
  //   (trg: GPR(RA)/p)
  typedef and_sfx_Trg rldic_sfx_Trg;
  
  // Operand Alias:  andi__sfx_Src -> rldic__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RS))
  //   (src: XER[mask 0x80000000]/p)
  typedef andi__sfx_Src rldic__sfx_Src;
  
  // Operand Alias:  and__sfx_Trg -> rldic__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RA))
  typedef and__sfx_Trg rldic__sfx_Trg;
  
  // Operand Alias:  cntlzw_sfx_Src -> rldicl_sfx_Src
  //   (src: GPR(RS))
  typedef cntlzw_sfx_Src rldicl_sfx_Src;
  
  // Operand Alias:  and_sfx_Trg -> rldicl_sfx_Trg
  //   (trg: GPR(RA)/p)
  typedef and_sfx_Trg rldicl_sfx_Trg;
  
  // Operand Alias:  andi__sfx_Src -> rldicl__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RS))
  //   (src: XER[mask 0x80000000]/p)
  typedef andi__sfx_Src rldicl__sfx_Src;
  
  // Operand Alias:  and__sfx_Trg -> rldicl__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RA))
  typedef and__sfx_Trg rldicl__sfx_Trg;
  
  // Operand Alias:  cntlzw_sfx_Src -> rldicr_sfx_Src
  //   (src: GPR(RS))
  typedef cntlzw_sfx_Src rldicr_sfx_Src;
  
  // Operand Alias:  and_sfx_Trg -> rldicr_sfx_Trg
  //   (trg: GPR(RA)/p)
  typedef and_sfx_Trg rldicr_sfx_Trg;
  
  // Operand Alias:  andi__sfx_Src -> rldicr__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RS))
  //   (src: XER[mask 0x80000000]/p)
  typedef andi__sfx_Src rldicr__sfx_Src;
  
  // Operand Alias:  and__sfx_Trg -> rldicr__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RA))
  typedef and__sfx_Trg rldicr__sfx_Trg;
  
  // Operand Alias:  rlwimi_sfx_Src -> rldimi_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RS))
  typedef rlwimi_sfx_Src rldimi_sfx_Src;
  
  // Operand Alias:  and_sfx_Trg -> rldimi_sfx_Trg
  //   (trg: GPR(RA)/p)
  typedef and_sfx_Trg rldimi_sfx_Trg;
  
  // Operand Alias:  andi__sfx_Src -> rldimi__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RS))
  //   (src: XER[mask 0x80000000]/p)
  typedef andi__sfx_Src rldimi__sfx_Src;
  
  // Operand Alias:  and__sfx_Trg -> rldimi__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RA))
  typedef and__sfx_Trg rldimi__sfx_Trg;
  
  // Operands:
  //   (src: GPR(0)?)
  //   (src: GPR(1)?)
  //   (src: GPR(3)?)
  //   (src: GPR(4)?)
  //   (src: GPR(5)?)
  //   (src: GPR(6)?)
  //   (src: GPR(7)?)
  struct sc_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(4)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(5)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(6)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(5)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(6)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(5)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(6)._index);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
        case 2: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
          return true;
        case 3: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
          return true;
        case 4: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(4)._index,"read")) return false;
          return true;
        case 5: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(5)._index,"read")) return false;
          return true;
        case 6: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(6)._index,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 3: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
        break;
        case 4: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index);
        break;
        case 5: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(5)._index);
        break;
        case 6: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(6)._index);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 3: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
        break;
        case 4: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index);
        break;
        case 5: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(5)._index);
        break;
        case 6: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(6)._index);
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
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(2)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read"))) return false;
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
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(6)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(6)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(6)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
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
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 2: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        case 3: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 4: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(4)._index);
        case 5: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(5)._index);
        case 6: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(6)._index);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 2: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        case 3: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 4: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(4)._index);
        case 5: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(5)._index);
        case 6: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(6)._index);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
        case 2: return 0x0;
        case 3: return 0x0;
        case 4: return 0x0;
        case 5: return 0x0;
        case 6: return 0x0;
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
        case 2: return ic._instr->write_mask(2).uint32();
        case 3: return ic._instr->write_mask(3).uint32();
        case 4: return ic._instr->write_mask(4).uint32();
        case 5: return ic._instr->write_mask(5).uint32();
        case 6: return ic._instr->write_mask(6).uint32();
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
        case 3: ic._instr->write_mask(3) = tag;
        break;
        case 4: ic._instr->write_mask(4) = tag;
        break;
        case 5: ic._instr->write_mask(5) = tag;
        break;
        case 6: ic._instr->write_mask(6) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 1;
        case 2: return 2;
        case 3: return 3;
        case 4: return 4;
        case 5: return 5;
        case 6: return 6;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 7; };
    
  };
  
  // Operands:
  //   (trg: GPR(3)?)
  struct sc_sfx_Trg {
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
      if (logger && ic._instr->is_dirty(2))logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(2)._index;
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
  //   (src: GPR(RB)[mask 0x0000007f]/p)
  //   (src: GPR(RS))
  struct sld_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read", ic._masks[5])) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read", (ic._masks[5] & ~mask).uint32())) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
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
      read(uarch,ic,logger);
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
        case 0: return ic._masks[5];
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
  
  // Operand Alias:  and_sfx_Trg -> sld_sfx_Trg
  //   (trg: GPR(RA)/p)
  typedef and_sfx_Trg sld_sfx_Trg;
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: GPR(RB)[mask 0x0000007f]/p)
  //   (src: GPR(RS))
  //   (src: XER[mask 0x80000000]/p)
  struct sld__sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read", ic._masks[12])) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[13])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read", (ic._masks[12] & ~mask).uint32())) return false;
          return true;
        case 2: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
          return true;
        case 3: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[13] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
        break;
        case 3: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
        break;
        case 3: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
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
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index));
          mask |= uarch.GPR.rm_later_partial_alloc(uarch,ic.op(2)._index,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index));
          mask |= uarch.GPR.rm_later_partial_alloc(uarch,ic.op(2)._index,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,1,mask))return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(3)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read"))) return false;
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
        if (!can_read(uarch,ic,logger,3,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
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
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        case 2: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 3: return static_cast<OSM_P&>(uarch).XER.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        case 2: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 3: return static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return ic._masks[12];
        case 2: return 0x0;
        case 3: return ic._masks[13];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(1).uint32();
        case 1: return ic._instr->write_mask(2).uint32();
        case 2: return ic._instr->write_mask(3).uint32();
        case 3: return ic._instr->write_mask(4).uint32();
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
        case 3: ic._instr->write_mask(4) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 3: return 4;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 4; };
    
  };
  
  // Operand Alias:  and__sfx_Trg -> sld__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RA))
  typedef and__sfx_Trg sld__sfx_Trg;
  
  // Operand Alias:  sld_sfx_Src -> srad_sfx_Src
  //   (src: GPR(RB)[mask 0x0000007f]/p)
  //   (src: GPR(RS))
  typedef sld_sfx_Src srad_sfx_Src;
  
  // Operand Alias:  sraw_sfx_Trg -> srad_sfx_Trg
  //   (trg: GPR(RA)/p)
  //   (trg: XER[mask 0x20000000]/p)
  typedef sraw_sfx_Trg srad_sfx_Trg;
  
  // Operand Alias:  sld__sfx_Src -> srad__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB)[mask 0x0000007f]/p)
  //   (src: GPR(RS))
  //   (src: XER[mask 0x80000000]/p)
  typedef sld__sfx_Src srad__sfx_Src;
  
  // Operand Alias:  sraw__sfx_Trg -> srad__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RA))
  //   (trg: XER[mask 0x20000000]/p)
  typedef sraw__sfx_Trg srad__sfx_Trg;
  
  // Operand Alias:  cntlzw_sfx_Src -> sradi_sfx_Src
  //   (src: GPR(RS))
  typedef cntlzw_sfx_Src sradi_sfx_Src;
  
  // Operand Alias:  srawi_sfx_Trg -> sradi_sfx_Trg
  //   (trg: GPR(RA)/p)
  //   (trg: XER[mask 0x20000000]/p)
  typedef srawi_sfx_Trg sradi_sfx_Trg;
  
  // Operand Alias:  andi__sfx_Src -> sradi__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RS))
  //   (src: XER[mask 0x80000000]/p)
  typedef andi__sfx_Src sradi__sfx_Src;
  
  // Operand Alias:  srawi__sfx_Trg -> sradi__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RA))
  //   (trg: XER[mask 0x20000000]/p)
  typedef srawi__sfx_Trg sradi__sfx_Trg;
  
  // Operand Alias:  sld_sfx_Src -> srd_sfx_Src
  //   (src: GPR(RB)[mask 0x0000007f]/p)
  //   (src: GPR(RS))
  typedef sld_sfx_Src srd_sfx_Src;
  
  // Operand Alias:  and_sfx_Trg -> srd_sfx_Trg
  //   (trg: GPR(RA)/p)
  typedef and_sfx_Trg srd_sfx_Trg;
  
  // Operand Alias:  sld__sfx_Src -> srd__sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB)[mask 0x0000007f]/p)
  //   (src: GPR(RS))
  //   (src: XER[mask 0x80000000]/p)
  typedef sld__sfx_Src srd__sfx_Src;
  
  // Operand Alias:  and__sfx_Trg -> srd__sfx_Trg
  //   (trg: CR/p)
  //   (trg: GPR(RA))
  typedef and__sfx_Trg srd__sfx_Trg;
  
  // Operand Alias:  rlwimi_sfx_Src -> std_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RS))
  typedef rlwimi_sfx_Src std_sfx_Src;
  
  // Operands:
  //   (trg: dsESR[mask 0x00800000]/p)
  struct std_sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,const bits<32> &mask) {
      if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
      return true;
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsESR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsESR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,mask))return false;
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
      uarch.dsESR.allocate(uarch,ic,logger, ic._masks[0]);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.dsESR.deallocate(uarch,ic,logger, ic._masks[0]);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).dsESR.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(2)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).dsESR.name(static_cast<OSM_P&>(uarch),0); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return ic._masks[0]; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(2).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(2) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 2; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operand Alias:  rlwimi_sfx_Src -> stdu_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RS))
  typedef rlwimi_sfx_Src stdu_sfx_Src;
  
  // Operands:
  //   (trg: GPR(RA)/p)
  //   (trg: dsESR[mask 0x00800000]/p)
  struct stdu_sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
          return true;
        case 1: if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint32())) return false;
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
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsESR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsESR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
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
      uarch.GPR.allocate(uarch,ic,logger,ic.op(0)._index);
      uarch.dsESR.allocate(uarch,ic,logger, ic._masks[1]);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(0)._index);
      uarch.dsESR.deallocate(uarch,ic,logger, ic._masks[1]);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).dsESR.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_P&>(uarch).dsESR.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0);
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
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  struct stdux_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
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
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
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
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(2)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
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
  
  // Operands:
  //   (trg: GPR(RA)/p)
  //   (trg: dsESR[mask 0x00800000]/p)
  struct stdux_sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
          return true;
        case 1: if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
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
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsESR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsESR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
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
      uarch.GPR.allocate(uarch,ic,logger,ic.op(0)._index);
      uarch.dsESR.allocate(uarch,ic,logger, ic._masks[0]);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(0)._index);
      uarch.dsESR.deallocate(uarch,ic,logger, ic._masks[0]);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).dsESR.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_P&>(uarch).dsESR.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0);
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
  
  // Operand Alias:  stdux_sfx_Src -> stdx_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  typedef stdux_sfx_Src stdx_sfx_Src;
  
  // Operands:
  //   (trg: dsESR[mask 0x00800000]/p)
  struct stdx_sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,const bits<32> &mask) {
      if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint32())) return false;
      return true;
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsESR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsESR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,mask))return false;
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
      uarch.dsESR.allocate(uarch,ic,logger, ic._masks[1]);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.dsESR.deallocate(uarch,ic,logger, ic._masks[1]);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).dsESR.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(3)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).dsESR.name(static_cast<OSM_P&>(uarch),0); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return ic._masks[1]; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(3).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(3) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 3; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RS)[mask 0x0000ffff]/p)
  struct sthbrx_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read", ic._masks[2])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
        case 2: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read", (ic._masks[2] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
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
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index));
          mask |= uarch.GPR.rm_later_partial_alloc(uarch,ic.op(2)._index,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index));
          mask |= uarch.GPR.rm_later_partial_alloc(uarch,ic.op(2)._index,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,2,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
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
        case 2: return ic._masks[2];
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
  
  // Operand Alias:  stdx_sfx_Trg -> sthbrx_sfx_Trg
  //   (trg: dsESR[mask 0x00800000]/p)
  typedef stdx_sfx_Trg sthbrx_sfx_Trg;
  
  // Operand Alias:  stdux_sfx_Src -> stwbrx_sfx_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  typedef stdux_sfx_Src stwbrx_sfx_Src;
  
  // Operand Alias:  stdx_sfx_Trg -> stwbrx_sfx_Trg
  //   (trg: dsESR[mask 0x00800000]/p)
  typedef stdx_sfx_Trg stwbrx_sfx_Trg;
  
  // Operands:
  //   (src: CR)
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RS)?)
  //   (src: RESRV/p)
  //   (src: XER[mask 0x80000000]/p)
  struct stwcx__sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
      if (!uarch.RESRV.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CR.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).RESRV.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CR.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).RESRV.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<64> &mask = bits<64>()) {
      switch (sub_op) {
        case 0: if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
        case 2: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
          return true;
        case 3: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
          return true;
        case 4: if (!uarch.RESRV.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 5: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).CR.name(),-1);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 3: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
        break;
        case 4: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).RESRV.name(),-1);
        break;
        case 5: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).CR.name(),-1);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 3: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
        break;
        case 4: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).RESRV.name(),-1);
        break;
        case 5: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
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
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(3)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).RESRV.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).RESRV.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.RESRV.can_alloc(uarch,ic,logger,"read"))) return false;
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
        if (!can_read(uarch,ic,logger,5,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
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
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 2: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        case 3: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 4: return static_cast<OSM_P&>(uarch).RESRV.name(static_cast<OSM_P&>(uarch),0);
        case 5: return static_cast<OSM_P&>(uarch).XER.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 2: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        case 3: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 4: return static_cast<OSM_P&>(uarch).RESRV.id(static_cast<OSM_P&>(uarch),0);
        case 5: return static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<64> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
        case 2: return 0x0;
        case 3: return 0x0;
        case 4: return 0x0;
        case 5: return ic._masks[0];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(1).uint32();
        case 2: return ic._instr->write_mask(2).uint32();
        case 3: return ic._instr->write_mask(3).uint32();
        case 4: return ic._instr->write_mask(4).uint32();
        case 5: return ic._instr->write_mask(5).uint32();
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
        case 3: ic._instr->write_mask(3) = tag;
        break;
        case 4: ic._instr->write_mask(4) = tag;
        break;
        case 5: ic._instr->write_mask(5) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 1;
        case 2: return 2;
        case 3: return 3;
        case 4: return 4;
        case 5: return 5;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 6; };
    
  };
  
  // Operands:
  //   (trg: CR)
  //   (trg: RESRV[mask 0x0000000000000001]/p?)
  //   (trg: dsESR[mask 0x00800000]/p)
  struct stwcx__sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.RESRV.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read", ic._masks[2])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<64> &mask = bits<64>()) {
      switch (sub_op) {
        case 0: if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.RESRV.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint64())) return false;
          return true;
        case 2: if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read", (ic._masks[2] & ~mask).uint32())) return false;
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
        RegId id = static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).CR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      {
        bits<64> mask = 0;
        {
          std::pair<bits<64>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).RESRV.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.RESRV.rm_later_partial_alloc(uarch,0,pm.first.uint64(),pm.second);
        }
        {
          std::pair<bits<64>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).RESRV.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.RESRV.rm_later_partial_alloc(uarch,0,pm.first.uint64(),pm.second);
        }
        if (!can_read(uarch,ic,logger,1,mask))return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsESR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsESR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
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
      uarch.CR.allocate(uarch,ic,logger);
      uarch.RESRV.allocate(uarch,ic,logger, ic._masks[1]);
      uarch.dsESR.allocate(uarch,ic,logger, ic._masks[2]);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CR.deallocate(uarch,ic,logger);
      uarch.RESRV.deallocate(uarch,ic,logger, ic._masks[1]);
      uarch.dsESR.deallocate(uarch,ic,logger, ic._masks[2]);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).CR.name(),-1);
      if (logger && ic._instr->is_dirty(4))logger->logOpAction("write",static_cast<OSM_P&>(uarch).RESRV.name(),-1);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).dsESR.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).RESRV.name(static_cast<OSM_P&>(uarch),0);
        case 2: return static_cast<OSM_P&>(uarch).dsESR.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).RESRV.id(static_cast<OSM_P&>(uarch),0);
        case 2: return static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<64> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return ic._masks[1];
        case 2: return ic._masks[2];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(4).uint32();
        case 2: return ic._instr->write_mask(6).uint32();
      }
      assert(0);
      return 0;
    };
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: ic._instr->write_mask(0) = tag;
        break;
        case 1: ic._instr->write_mask(4) = tag;
        break;
        case 2: ic._instr->write_mask(6) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 4;
        case 2: return 6;
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
  //   (src: DBCR0[mask 0x41000000]/p?)
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: MSR[mask 0x00000200]/p?)
  struct tw_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.DBCR0.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      if (!uarch.MSR.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).DBCR0.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).MSR.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).DBCR0.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).MSR.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.DBCR0.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
        case 2: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
          return true;
        case 3: if (!uarch.MSR.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).DBCR0.name(),-1);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 3: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).MSR.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).DBCR0.name(),-1);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 3: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).MSR.name(),-1);
        break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).DBCR0.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.DBCR0.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).DBCR0.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.DBCR0.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,0,mask))return false;
      }
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
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).MSR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.MSR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).MSR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.MSR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,3,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
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
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).DBCR0.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 2: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        case 3: return static_cast<OSM_P&>(uarch).MSR.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).DBCR0.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 2: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        case 3: return static_cast<OSM_P&>(uarch).MSR.id(static_cast<OSM_P&>(uarch),0);
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
        case 1: return ic._instr->write_mask(1).uint32();
        case 2: return ic._instr->write_mask(2).uint32();
        case 3: return ic._instr->write_mask(3).uint32();
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
        case 3: ic._instr->write_mask(3) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 1;
        case 2: return 2;
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
  //   (src: DBCR0[mask 0x41000000]/p?)
  //   (src: GPR(RA))
  //   (src: MSR[mask 0x00000200]/p?)
  struct twi_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.DBCR0.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.MSR.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).DBCR0.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).MSR.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).DBCR0.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).MSR.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.DBCR0.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
        case 2: if (!uarch.MSR.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).DBCR0.name(),-1);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).MSR.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).DBCR0.name(),-1);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).MSR.name(),-1);
        break;
      }
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).DBCR0.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.DBCR0.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).DBCR0.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.DBCR0.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,0,mask))return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(1)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read"))) return false;
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
        if (!can_read(uarch,ic,logger,2,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
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
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).DBCR0.name(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 2: return static_cast<OSM_P&>(uarch).MSR.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).DBCR0.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 2: return static_cast<OSM_P&>(uarch).MSR.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._masks[0];
        case 1: return 0x0;
        case 2: return ic._masks[1];
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
  //   (src: GPR(RS)[mask 0x00008000]/p?)
  struct wrtee_sfx_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,const bits<32> &mask) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read", (ic._masks[0] & ~mask).uint32())) return false;
      return true;
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index));
          mask |= uarch.GPR.rm_later_partial_alloc(uarch,ic.op(0)._index,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index));
          mask |= uarch.GPR.rm_later_partial_alloc(uarch,ic.op(0)._index,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
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
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(0)._index); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return ic._masks[0]; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(0).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(0) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 0; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (trg: MSR[mask 0x00008000]/p?)
  struct wrtee_sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.MSR.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,const bits<32> &mask) {
      if (!uarch.MSR.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint32())) return false;
      return true;
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
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
        if (!can_read(uarch,ic,logger,mask))return false;
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
      uarch.MSR.allocate(uarch,ic,logger, ic._masks[1]);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.MSR.deallocate(uarch,ic,logger, ic._masks[1]);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger && ic._instr->is_dirty(1))logger->logOpAction("write",static_cast<OSM_P&>(uarch).MSR.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(1)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).MSR.name(static_cast<OSM_P&>(uarch),0); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).MSR.id(static_cast<OSM_P&>(uarch),0); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return ic._masks[1]; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(1).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(1) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 1; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (trg: MSR[mask 0x00008000]/p?)
  struct wrteei_sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.MSR.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,const bits<32> &mask) {
      if (!uarch.MSR.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
      return true;
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
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
        if (!can_read(uarch,ic,logger,mask))return false;
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
      uarch.MSR.allocate(uarch,ic,logger, ic._masks[0]);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.MSR.deallocate(uarch,ic,logger, ic._masks[0]);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger && ic._instr->is_dirty(0))logger->logOpAction("write",static_cast<OSM_P&>(uarch).MSR.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).MSR.name(static_cast<OSM_P&>(uarch),0); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).MSR.id(static_cast<OSM_P&>(uarch),0); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return ic._masks[0]; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(0).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(0) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 0; }
    
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
    
    bool exec_done() const { return mem() == 0; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool change_state_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    bool change_state_S_EX__S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      return action_S_EXe(uarch,logger,position);
    }
    
    bool change_state_S_EXe__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
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
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.issue(_instr);
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool change_state_S_MM__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    bool change_state_S_MM__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      return action_S_MMe(uarch,logger,position);
    }
    
    bool change_state_S_MMe__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    bool change_state_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      if (uarch.is_current_speculative_instr(*this)) {
        if (logger) logger->logMessage("forced flush!");
        uarch.taken_flush_mWB(*this,logger,position,false,0);
      }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      uarch.commit(this);
      return true;
    }
    
    bool change_state_S_WB__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      if (uarch.is_current_speculative_instr(*this)) {
        if (logger) logger->logMessage("forced flush!");
        uarch.taken_flush_mWB(*this,logger,position,false,0);
      }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      uarch.commit(this);
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger) ) {
            {uarch.exec_mEX(*this,logger,position) ; if (has_exceptions()) goto HandleException; } ;
            uarch.FwdEX.write(uarch,*this,logger, Trg ) ;
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
    static bool action_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger) ) {
            return change_state_S_EXe__S_MMe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( Src.can_read_fp(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) && Trg.can_write(uarch,*this,logger) && uarch.msync_stall.can_allocate(uarch,*this,logger) && uarch.mEX.has_space(uarch,*this,logger) ) {
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
    
    // State action S_MM
    bool action_S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger) ) {
            {uarch.exec_mMM(*this,logger,position) ; if (has_exceptions()) goto HandleException; } ;
            uarch.FwdMM.write(uarch,*this,logger, Trg ) ;
            return change_state_S_MM__S_WB(uarch,logger,position) ;
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
    static bool action_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // State action S_MMe
    bool action_S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger) ) {
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
    }
    static void flush_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_IDe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_MMe
    void flush_S_MMe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMe(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBe(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class signedMulCr
  //
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RT))
  //   (src: XER[mask 0x80000000]/p)
  struct mullw__signedMulCr_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
          return true;
        case 2: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
          return true;
        case 3: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
        break;
        case 3: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
        break;
        case 3: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
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
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(3)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read"))) return false;
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
        if (!can_read(uarch,ic,logger,3,mask))return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
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
        case 2: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 3: return static_cast<OSM_P&>(uarch).XER.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(1)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        case 2: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 3: return static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
        case 1: return 0x0;
        case 2: return 0x0;
        case 3: return ic._masks[0];
      }
      assert(0);
      return 0;
    };
    
    tag_t get_wm_tag(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return ic._instr->write_mask(1).uint32();
        case 1: return ic._instr->write_mask(2).uint32();
        case 2: return ic._instr->write_mask(3).uint32();
        case 3: return ic._instr->write_mask(4).uint32();
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
        case 3: ic._instr->write_mask(4) = tag;
        break;
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 1;
        case 1: return 2;
        case 2: return 3;
        case 3: return 4;
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
  //   (src: GPR(RA))
  struct mullw__signedMulCr_Src1 {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
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
      read(uarch,ic,logger);
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
  //   (src: GPR(RB))
  struct mullw__signedMulCr_Src2 {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
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
      read(uarch,ic,logger);
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
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  struct mullw__signedMulCr_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
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
        RegId id = static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).CR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
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
      uarch.CR.allocate(uarch,ic,logger);
      uarch.GPR.allocate(uarch,ic,logger,ic.op(3)._index);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CR.deallocate(uarch,ic,logger);
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(3)._index);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).CR.name(),-1);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
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
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(3)._index);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
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
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: XER[mask 0xc0000000]/p)
  struct mullwo_signedMulCr_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
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
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).XER.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).XER.name(),-1);
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
      read(uarch,ic,logger);
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
  //   (src: GPR(RA))
  struct mullwo_signedMulCr_Src1 {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
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
      read(uarch,ic,logger);
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
  //   (src: GPR(RB))
  struct mullwo_signedMulCr_Src2 {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
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
      read(uarch,ic,logger);
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
  //   (trg: GPR(RT)/p)
  //   (trg: XER[mask 0xc0000000])
  struct mullwo_signedMulCr_Trg {
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
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).XER.name(),-1);
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
  
  // Operand Alias:  mullw__signedMulCr_Src -> mullwo__signedMulCr_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RT))
  //   (src: XER[mask 0xc0000000]/p)
  typedef mullw__signedMulCr_Src mullwo__signedMulCr_Src;
  
  // Operand Alias:  mullw__signedMulCr_Src1 -> mullwo__signedMulCr_Src1
  //   (src: GPR(RA))
  typedef mullw__signedMulCr_Src1 mullwo__signedMulCr_Src1;
  
  // Operand Alias:  mullw__signedMulCr_Src2 -> mullwo__signedMulCr_Src2
  //   (src: GPR(RB))
  typedef mullw__signedMulCr_Src2 mullwo__signedMulCr_Src2;
  
  // Operands:
  //   (trg: CR/p)
  //   (trg: GPR(RT))
  //   (trg: XER[mask 0xc0000000])
  struct mullwo__signedMulCr_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
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
        case 0: if (!uarch.CR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
          return true;
        case 2: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint32())) return false;
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
        RegId id = static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        tag_t tag = (static_cast<OSM_P&>(uarch).CR.last_allocator( static_cast<OSM_P&>(uarch),0));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.CR.can_alloc(uarch,ic,logger,"read"))) return false;
      }
      {
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(3)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read"))) return false;
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
      uarch.CR.allocate(uarch,ic,logger);
      uarch.GPR.allocate(uarch,ic,logger,ic.op(3)._index);
      uarch.XER.allocate(uarch,ic,logger, ic._masks[1]);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CR.deallocate(uarch,ic,logger);
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(3)._index);
      uarch.XER.deallocate(uarch,ic,logger, ic._masks[1]);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).CR.name(),-1);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).XER.name(),-1);
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
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(3)._index);
        case 2: return static_cast<OSM_P&>(uarch).XER.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).CR.id(static_cast<OSM_P&>(uarch),0);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(3)._index);
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
        case 0: return ic._instr->write_mask(0).uint32();
        case 1: return ic._instr->write_mask(3).uint32();
        case 2: return ic._instr->write_mask(4).uint32();
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
      }
    };
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return 0;
        case 1: return 3;
        case 2: return 4;
      }
      assert(0);
      return 0;
    };
    
    bool is_op_read(Inst_P&ic,unsigned sub_op = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot(sub_op)));
    }
    unsigned num_sub_ops() const{ return 3; };
    
  };
  
  //
  // Instruction class signedMulCr
  //
  template <class Src_t, class Src1_t, class Src2_t, class Trg_t>
  struct Inst_signedMulCr : public Inst_P {
    
    typedef struct Inst_signedMulCr<Src_t, Src1_t, Src2_t, Trg_t> ThisClass;
    
    Inst_signedMulCr &thisInstr() { return *this; };
    
    Src_t Src;
    Src1_t Src1;
    Src2_t Src2;
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
      Src1.deallocate(uarch,ic,logger);
      Src2.deallocate(uarch,ic,logger);
      Trg.deallocate(uarch,ic,logger);
    }
    
    void write_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      Src.write(uarch,ic,logger);
      Src1.write(uarch,ic,logger);
      Src2.write(uarch,ic,logger);
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
    
    bool change_state_S_EXe__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    bool change_state_S_EXp__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXp -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    bool change_state_S_EXp__S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXp -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      return action_S_EXe(uarch,logger,position);
    }
    
    bool change_state_S_ID__S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
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
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.issue(_instr);
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool change_state_S_MM__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    bool change_state_S_MM__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      return action_S_MMe(uarch,logger,position);
    }
    
    bool change_state_S_MMe__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    bool change_state_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    bool change_state_S_WB__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      {
            uarch.exec(*this,logger) ;
            set_latency ( uarch. OSM_P::signed_mul_div_delay ( Src1.value(uarch,*this,logger) , Src2.value(uarch,*this,logger) ) ) ;
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
    static bool action_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger) ) {
            return change_state_S_EXe__S_MMe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_EXp
    bool action_S_EXp(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger) ) if ( check_latency(logger) && uarch.mWB.has_space(uarch,*this,logger) ) {
            uarch.FwdEX.write(uarch,*this,logger, Trg ) ;
            return change_state_S_EXp__S_MM(uarch,logger,position) ;
      } else {
            decr_latency (  ) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        flush_S_EXp(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mEX(*this,logger,position,true,branch_target());
        return change_state_S_EXp__S_EXe(uarch,logger,position);
      }
    }
    static bool action_S_EXp_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXp(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( Src.can_read_fp(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) && Trg.can_write(uarch,*this,logger) && uarch.msync_stall.can_allocate(uarch,*this,logger) && uarch.CR.can_read(uarch,*this,logger, 0xff000000 ) && uarch.mEX.has_space(uarch,*this,logger) ) {
            Src.read(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) ;
            Trg.allocate(uarch,*this,logger) ;
            uarch.lsamd_stall.allocate(uarch,*this,logger) ;
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
    
    // State action S_MM
    bool action_S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger) ) {
            return change_state_S_MM__S_WB(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // State action S_MMe
    bool action_S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger) ) {
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
            write_ops(uarch,*this,logger) ;
            uarch.lsamd_stall.deallocate(uarch,*this,logger) ;
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
      uarch.lsamd_stall.deallocate(uarch,*this,logger);
    }
    static void flush_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_EXp
    void flush_S_EXp(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      uarch.lsamd_stall.deallocate(uarch,*this,logger);
    }
    static void flush_S_EXp_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXp(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      uarch.lsamd_stall.deallocate(uarch,*this,logger);
    }
    static void flush_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_IDe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      uarch.lsamd_stall.deallocate(uarch,*this,logger);
    }
    static void flush_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_MMe
    void flush_S_MMe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMe(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      uarch.lsamd_stall.deallocate(uarch,*this,logger);
    }
    static void flush_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBe(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class signedMulDiv
  //
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  struct mullw_signedMulDiv_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
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
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
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
      read(uarch,ic,logger);
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
  //   (src: GPR(RA))
  struct mullw_signedMulDiv_Src1 {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
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
      read(uarch,ic,logger);
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
  //   (src: GPR(RB))
  struct mullw_signedMulDiv_Src2 {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
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
      read(uarch,ic,logger);
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
  //   (trg: GPR(RT)/p)
  struct mullw_signedMulDiv_Trg {
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
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
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
  
  // Operand Alias:  mullw_signedMulDiv_Src -> mulhw_signedMulDiv_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef mullw_signedMulDiv_Src mulhw_signedMulDiv_Src;
  
  // Operand Alias:  mullw_signedMulDiv_Src1 -> mulhw_signedMulDiv_Src1
  //   (src: GPR(RA))
  typedef mullw_signedMulDiv_Src1 mulhw_signedMulDiv_Src1;
  
  // Operand Alias:  mullw_signedMulDiv_Src2 -> mulhw_signedMulDiv_Src2
  //   (src: GPR(RB))
  typedef mullw_signedMulDiv_Src2 mulhw_signedMulDiv_Src2;
  
  // Operand Alias:  mullw_signedMulDiv_Trg -> mulhw_signedMulDiv_Trg
  //   (trg: GPR(RT)/p)
  typedef mullw_signedMulDiv_Trg mulhw_signedMulDiv_Trg;
  
  // Operand Alias:  mullw_signedMulDiv_Src -> divw_signedMulDiv_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  typedef mullw_signedMulDiv_Src divw_signedMulDiv_Src;
  
  // Operand Alias:  mullw_signedMulDiv_Src1 -> divw_signedMulDiv_Src1
  //   (src: GPR(RA))
  typedef mullw_signedMulDiv_Src1 divw_signedMulDiv_Src1;
  
  // Operand Alias:  mullw_signedMulDiv_Src2 -> divw_signedMulDiv_Src2
  //   (src: GPR(RB))
  typedef mullw_signedMulDiv_Src2 divw_signedMulDiv_Src2;
  
  // Operand Alias:  mullw_signedMulDiv_Trg -> divw_signedMulDiv_Trg
  //   (trg: GPR(RT)/p)
  typedef mullw_signedMulDiv_Trg divw_signedMulDiv_Trg;
  
  //
  // Instruction class signedMulDiv
  //
  template <class Src_t, class Src1_t, class Src2_t, class Trg_t>
  struct Inst_signedMulDiv : public Inst_P {
    
    typedef struct Inst_signedMulDiv<Src_t, Src1_t, Src2_t, Trg_t> ThisClass;
    
    Inst_signedMulDiv &thisInstr() { return *this; };
    
    Src_t Src;
    Src1_t Src1;
    Src2_t Src2;
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
      Src1.deallocate(uarch,ic,logger);
      Src2.deallocate(uarch,ic,logger);
      Trg.deallocate(uarch,ic,logger);
    }
    
    void write_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      Src.write(uarch,ic,logger);
      Src1.write(uarch,ic,logger);
      Src2.write(uarch,ic,logger);
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
    
    bool change_state_S_EXe__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    bool change_state_S_EXp__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXp -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    bool change_state_S_EXp__S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXp -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      return action_S_EXe(uarch,logger,position);
    }
    
    bool change_state_S_ID__S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
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
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_INIT__S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.issue(_instr);
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool change_state_S_MM__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    bool change_state_S_MM__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      return action_S_MMe(uarch,logger,position);
    }
    
    bool change_state_S_MMe__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    bool change_state_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    bool change_state_S_WB__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      {
            uarch.exec(*this,logger) ;
            set_latency ( uarch. OSM_P::signed_mul_div_delay ( Src1.value(uarch,*this,logger) , Src2.value(uarch,*this,logger) ) ) ;
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
    static bool action_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger) ) {
            return change_state_S_EXe__S_MMe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_EXp
    bool action_S_EXp(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger) ) if ( check_latency(logger) && uarch.mWB.has_space(uarch,*this,logger) ) {
            uarch.FwdEX.write(uarch,*this,logger, Trg ) ;
            return change_state_S_EXp__S_MM(uarch,logger,position) ;
      } else {
            decr_latency (  ) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        flush_S_EXp(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mEX(*this,logger,position,true,branch_target());
        return change_state_S_EXp__S_EXe(uarch,logger,position);
      }
    }
    static bool action_S_EXp_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXp(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( Src.can_read_fp(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) && Trg.can_write(uarch,*this,logger) && uarch.msync_stall.can_allocate(uarch,*this,logger) && uarch.mEX.has_space(uarch,*this,logger) ) {
            Src.read(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) ;
            Trg.allocate(uarch,*this,logger) ;
            uarch.lsamd_stall.allocate(uarch,*this,logger) ;
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
    
    // State action S_MM
    bool action_S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger) ) {
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
    static bool action_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // State action S_MMe
    bool action_S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger) ) {
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
            write_ops(uarch,*this,logger) ;
            uarch.lsamd_stall.deallocate(uarch,*this,logger) ;
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
      uarch.lsamd_stall.deallocate(uarch,*this,logger);
    }
    static void flush_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_EXp
    void flush_S_EXp(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      uarch.lsamd_stall.deallocate(uarch,*this,logger);
    }
    static void flush_S_EXp_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXp(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      uarch.lsamd_stall.deallocate(uarch,*this,logger);
    }
    static void flush_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_IDe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      uarch.lsamd_stall.deallocate(uarch,*this,logger);
    }
    static void flush_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_MMe
    void flush_S_MMe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMe(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      uarch.lsamd_stall.deallocate(uarch,*this,logger);
    }
    static void flush_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBe(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class stmw
  //
  
  // Operands:
  //   (src: GPR(RA))
  //   (src: GPR(RS . uint32 (  ),31))
  struct stmw_stmw_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      for (int i = 1; i != 33; ++i) {
        if (ic.op(i).isSrc() && !uarch.GPR.can_alloc(uarch,ic,logger,ic.op(i)._index,"read")) return false;
      }
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      for (int i = 1; i != 33; ++i) {
        if (ic.op(i).isSrc()) {
          if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(i)._index);
        }
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      for (int i = 1; i != 33; ++i) {
        if (ic.op(i).isSrc()) {
          logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(i)._index);
        }
      }
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
          return true;
        case 1: for (int i = 1; i != 33; ++i) {
          if (ic.op(i).isSrc() && !uarch.GPR.can_alloc(uarch,ic,logger,ic.op(i)._index,"read")) return false;
        }
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: for (int i = 1; i != 33; ++i) {
          if (ic.op(i).isSrc()) {
            if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(i)._index);
          }
        }
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: for (int i = 1; i != 33; ++i) {
          if (ic.op(i).isSrc()) {
            logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(i)._index);
          }
        }
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
        for (int i = 1; i != 33; ++i) {
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
      read(uarch,ic,logger);
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
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(op_off)._index);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(op_off)._index);
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
        case 1: return op_off;
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
  //   (trg: dsESR[mask 0x00800000]/p)
  struct stmw_stmw_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,const bits<32> &mask) {
      if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
      return true;
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsESR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsESR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,mask))return false;
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
      uarch.dsESR.allocate(uarch,ic,logger, ic._masks[0]);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.dsESR.deallocate(uarch,ic,logger, ic._masks[0]);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).dsESR.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(33)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).dsESR.name(static_cast<OSM_P&>(uarch),0); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return ic._masks[0]; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(33).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(33) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 33; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  //
  // Instruction class stmw
  //
  template <class Src_t, class Trg_t>
  struct Inst_stmw : public Inst_P {
    
    typedef struct Inst_stmw<Src_t, Trg_t> ThisClass;
    
    Inst_stmw &thisInstr() { return *this; };
    
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
    
    bool exec_done() const { return mem() == 0; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool change_state_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    bool change_state_S_EX__S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      return action_S_EXe(uarch,logger,position);
    }
    
    bool change_state_S_EXe__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_IDp(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_IDp"); }
      _action = (StateFunc)action_S_IDp_p;
      _flush = (FlushFunc)flush_S_IDp_p;
      return action_S_IDp(uarch,logger,position);
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
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_IDp__S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDp -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_IDp__S_IDe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDp -> S_IDe"); }
      _action = (StateFunc)action_S_IDe_p;
      _flush = (FlushFunc)flush_S_IDe_p;
      return action_S_IDe(uarch,logger,position);
    }
    
    bool change_state_S_INIT__S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.issue(_instr);
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
    
    bool change_state_S_MMe__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    bool change_state_S_MMp__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMp -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      return action_S_MM(uarch,logger,position);
    }
    
    bool change_state_S_MMp__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMp -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    bool change_state_S_MMp__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMp -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      return action_S_MMe(uarch,logger,position);
    }
    
    bool change_state_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      uarch.commit(this);
      return true;
    }
    
    bool change_state_S_WB__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      uarch.commit(this);
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger) ) {
            if ( has_next_request (  ) && uarch.L1d.can_request_write(uarch,*this,logger) ) {
                  uarch.L1d.send_write_request(uarch,*this,logger, 8 ) ;
                  next_req_mem (  ) ;
              }
            return change_state_S_EX__S_MM(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger) ) {
            return change_state_S_EXe__S_MMe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( Src.can_read_fp(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) && Trg.can_write(uarch,*this,logger) && uarch.mEX.has_space(uarch,*this,logger) ) {
            Src.read(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) ;
            Trg.allocate(uarch,*this,logger) ;
            return change_state_S_ID__S_IDp(uarch,logger,position) ;
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
    
    // State action S_IDp
    bool action_S_IDp(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mEX.has_space(uarch,*this,logger) && uarch.lsamd_stall.can_allocate(uarch,*this,logger) && uarch.msync_stall.can_allocate(uarch,*this,logger) && uarch.L1d.can_request_write(uarch,*this,logger) ) {
            {uarch.exec_mem(*this,logger) ; if (has_exceptions()) goto HandleException; } ;
            uarch.L1d.send_write_request(uarch,*this,logger, 8 ) ;
            next_req_mem (  ) ;
            return change_state_S_IDp__S_EX(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_IDp(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mID(*this,logger,position,true,branch_target());
        return change_state_S_IDp__S_IDe(uarch,logger,position);
      }
    }
    static bool action_S_IDp_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IDp(uarch,logger,position);
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
      if ( uarch.L1d.can_write(uarch,*this,logger) ) {
            { uarch.exec_and_write(*this,logger, uarch.L1d , 2 , 4 )  ; if (has_exceptions()) goto HandleException; } ;
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
    static bool action_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // State action S_MMe
    bool action_S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger) ) {
            return change_state_S_MMe__S_WBe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MMe(uarch,logger,position);
    }
    
    // State action S_MMp
    bool action_S_MMp(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( exec_done (  ) && uarch.mWB.has_space(uarch,*this,logger) ) {
            return change_state_S_MMp__S_WB(uarch,logger,position) ;
      } else if ( uarch.L1d.can_request_write(uarch,*this,logger) ) {
            uarch.L1d.send_write_request(uarch,*this,logger, 8 ) ;
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
    }
    static void flush_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_IDe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_IDp
    void flush_S_IDp(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_IDp_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDp(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_MMe
    void flush_S_MMe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMe(uarch,logger);
    }
    
    // Flush action S_MMp
    void flush_S_MMp(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_MMp_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMp(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
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
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
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
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
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
      read(uarch,ic,logger);
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
  //   (trg: dsESR[mask 0x00800000]/p)
  struct stb_store_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,const bits<32> &mask) {
      if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
      return true;
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsESR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsESR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,mask))return false;
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
      uarch.dsESR.allocate(uarch,ic,logger, ic._masks[0]);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.dsESR.deallocate(uarch,ic,logger, ic._masks[0]);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).dsESR.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(2)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).dsESR.name(static_cast<OSM_P&>(uarch),0); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return ic._masks[0]; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(2).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(2) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 2; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operand Alias:  stb_store_Src -> stbu_store_Src
  //   (src: GPR(RA))
  //   (src: GPR(RS))
  typedef stb_store_Src stbu_store_Src;
  
  // Operands:
  //   (trg: GPR(RA)/p)
  //   (trg: dsESR[mask 0x00800000]/p)
  struct stbu_store_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
          return true;
        case 1: if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read", (ic._masks[1] & ~mask).uint32())) return false;
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
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsESR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsESR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
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
      uarch.GPR.allocate(uarch,ic,logger,ic.op(0)._index);
      uarch.dsESR.allocate(uarch,ic,logger, ic._masks[1]);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(0)._index);
      uarch.dsESR.deallocate(uarch,ic,logger, ic._masks[1]);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).dsESR.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_P&>(uarch).dsESR.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0);
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
  
  // Operands:
  //   (trg: GPR(RA)/p)
  struct stbu_store_TrgEA {
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
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
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
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  struct stbux_store_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
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
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(1)._index);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(2)._index);
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
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(2)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(2)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read"))) return false;
      }
      return true;
    }
    
    template <typename T0, typename T1>
    void read(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      read(uarch,ic,logger);
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
  
  // Operands:
  //   (trg: GPR(RA)/p)
  //   (trg: dsESR[mask 0x00800000]/p)
  struct stbux_store_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
      if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read", ic._masks[2])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read")) return false;
          return true;
        case 1: if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read", (ic._masks[2] & ~mask).uint32())) return false;
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
        RegId id = static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        tag_t tag = (static_cast<OSM_P&>(uarch).GPR.last_allocator( static_cast<OSM_P&>(uarch),ic.op(0)._index));
        if  (!(t0.can_read(uarch,ic,logger,id,tag) || t1.can_read(uarch,ic,logger,id,tag) || uarch.GPR.can_alloc(uarch,ic,logger,ic.op(0)._index,"read"))) return false;
      }
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsESR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsESR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
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
      uarch.GPR.allocate(uarch,ic,logger,ic.op(0)._index);
      uarch.dsESR.allocate(uarch,ic,logger, ic._masks[2]);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(0)._index);
      uarch.dsESR.deallocate(uarch,ic,logger, ic._masks[2]);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).dsESR.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(0)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.name(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_P&>(uarch).dsESR.name(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return string();
    }
    
    RegId id(UadlArch &uarch,const Inst_P&ic,unsigned sub_op = 0,unsigned op_off = 0) const {
      switch (sub_op) {
        case 0: return static_cast<OSM_P&>(uarch).GPR.id(static_cast<OSM_P&>(uarch),ic.op(0)._index);
        case 1: return static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0);
      }
      assert(0);
      return 0;
    }
    
    bits<32> mask(Inst_P &ic,unsigned sub_op = 0) const {
      switch (sub_op) {
        case 0: return 0x0;
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
  
  // Operand Alias:  stbu_store_TrgEA -> stbux_store_TrgEA
  //   (trg: GPR(RA)/p)
  typedef stbu_store_TrgEA stbux_store_TrgEA;
  
  // Operand Alias:  stbux_store_Src -> stbx_store_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  typedef stbux_store_Src stbx_store_Src;
  
  // Operands:
  //   (trg: dsESR[mask 0x00800000]/p)
  struct stbx_store_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read", ic._masks[3])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,const bits<32> &mask) {
      if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read", (ic._masks[3] & ~mask).uint32())) return false;
      return true;
    }
    
    template <typename T0, typename T1>
    bool can_read_fp(OSM_P &uarch,Inst_P &ic,Logger *logger,T0 &t0,T1 &t1) {
      {
        bits<32> mask = 0;
        {
          std::pair<bits<32>,tag_t> pm = t0.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsESR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        {
          std::pair<bits<32>,tag_t> pm = t1.can_read_partial(uarch,ic,logger,static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0));
          mask |= uarch.dsESR.rm_later_partial_alloc(uarch,0,pm.first.uint32(),pm.second);
        }
        if (!can_read(uarch,ic,logger,mask))return false;
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
      uarch.dsESR.allocate(uarch,ic,logger, ic._masks[3]);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.dsESR.deallocate(uarch,ic,logger, ic._masks[3]);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).dsESR.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(3)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).dsESR.name(static_cast<OSM_P&>(uarch),0); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).dsESR.id(static_cast<OSM_P&>(uarch),0); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return ic._masks[3]; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(3).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(3) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 3; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operand Alias:  stb_store_Src -> sth_store_Src
  //   (src: GPR(RA))
  //   (src: GPR(RS))
  typedef stb_store_Src sth_store_Src;
  
  // Operand Alias:  stb_store_Trg -> sth_store_Trg
  //   (trg: dsESR[mask 0x00800000]/p)
  typedef stb_store_Trg sth_store_Trg;
  
  // Operand Alias:  stb_store_Src -> sthu_store_Src
  //   (src: GPR(RA))
  //   (src: GPR(RS))
  typedef stb_store_Src sthu_store_Src;
  
  // Operand Alias:  stbu_store_Trg -> sthu_store_Trg
  //   (trg: GPR(RA)/p)
  //   (trg: dsESR[mask 0x00800000]/p)
  typedef stbu_store_Trg sthu_store_Trg;
  
  // Operand Alias:  stbu_store_TrgEA -> sthu_store_TrgEA
  //   (trg: GPR(RA)/p)
  typedef stbu_store_TrgEA sthu_store_TrgEA;
  
  // Operand Alias:  stbux_store_Src -> sthux_store_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  typedef stbux_store_Src sthux_store_Src;
  
  // Operand Alias:  stbux_store_Trg -> sthux_store_Trg
  //   (trg: GPR(RA)/p)
  //   (trg: dsESR[mask 0x00800000]/p)
  typedef stbux_store_Trg sthux_store_Trg;
  
  // Operand Alias:  stbu_store_TrgEA -> sthux_store_TrgEA
  //   (trg: GPR(RA)/p)
  typedef stbu_store_TrgEA sthux_store_TrgEA;
  
  // Operand Alias:  stbux_store_Src -> sthx_store_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  typedef stbux_store_Src sthx_store_Src;
  
  // Operand Alias:  stbx_store_Trg -> sthx_store_Trg
  //   (trg: dsESR[mask 0x00800000]/p)
  typedef stbx_store_Trg sthx_store_Trg;
  
  // Operand Alias:  stb_store_Src -> stw_store_Src
  //   (src: GPR(RA))
  //   (src: GPR(RS))
  typedef stb_store_Src stw_store_Src;
  
  // Operand Alias:  stb_store_Trg -> stw_store_Trg
  //   (trg: dsESR[mask 0x00800000]/p)
  typedef stb_store_Trg stw_store_Trg;
  
  // Operand Alias:  stb_store_Src -> stwu_store_Src
  //   (src: GPR(RA))
  //   (src: GPR(RS))
  typedef stb_store_Src stwu_store_Src;
  
  // Operand Alias:  stbu_store_Trg -> stwu_store_Trg
  //   (trg: GPR(RA)/p)
  //   (trg: dsESR[mask 0x00800000]/p)
  typedef stbu_store_Trg stwu_store_Trg;
  
  // Operand Alias:  stbu_store_TrgEA -> stwu_store_TrgEA
  //   (trg: GPR(RA)/p)
  typedef stbu_store_TrgEA stwu_store_TrgEA;
  
  // Operand Alias:  stbux_store_Src -> stwux_store_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  typedef stbux_store_Src stwux_store_Src;
  
  // Operand Alias:  stbux_store_Trg -> stwux_store_Trg
  //   (trg: GPR(RA)/p)
  //   (trg: dsESR[mask 0x00800000]/p)
  typedef stbux_store_Trg stwux_store_Trg;
  
  // Operand Alias:  stbu_store_TrgEA -> stwux_store_TrgEA
  //   (trg: GPR(RA)/p)
  typedef stbu_store_TrgEA stwux_store_TrgEA;
  
  // Operand Alias:  stbux_store_Src -> stwx_store_Src
  //   (src: GPR(RA))
  //   (src: GPR(RB))
  //   (src: GPR(RS))
  typedef stbux_store_Src stwx_store_Src;
  
  // Operand Alias:  stbx_store_Trg -> stwx_store_Trg
  //   (trg: dsESR[mask 0x00800000]/p)
  typedef stbx_store_Trg stwx_store_Trg;
  
  //
  // Instruction class store
  //
  template <class Src_t, class Trg_t, class TrgEA_t>
  struct Inst_store : public Inst_P {
    
    typedef struct Inst_store<Src_t, Trg_t, TrgEA_t> ThisClass;
    
    Inst_store &thisInstr() { return *this; };
    
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
    
    bool exec_done() const { return mem() == 0; }
    
    bool has_exceptions() const { return _instr->has_exceptions(); };
    
    bool change_state_S_EX__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    bool change_state_S_EX__S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EX -> S_EXe"); }
      _action = (StateFunc)action_S_EXe_p;
      _flush = (FlushFunc)flush_S_EXe_p;
      return action_S_EXe(uarch,logger,position);
    }
    
    bool change_state_S_EXe__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_EXe -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      uarch.mEX.remove_instr();
      uarch.mMM.add_instr(this);
      return true;
    }
    
    bool change_state_S_ID__S_IDp(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_ID -> S_IDp"); }
      _action = (StateFunc)action_S_IDp_p;
      _flush = (FlushFunc)flush_S_IDp_p;
      return action_S_IDp(uarch,logger,position);
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
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_IDp__S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDp -> S_EX"); }
      _action = (StateFunc)action_S_EX_p;
      _flush = (FlushFunc)flush_S_EX_p;
      uarch.mID.remove_instr();
      uarch.mEX.add_instr(this);
      return true;
    }
    
    bool change_state_S_IDp__S_IDe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_IDp -> S_IDe"); }
      _action = (StateFunc)action_S_IDe_p;
      _flush = (FlushFunc)flush_S_IDe_p;
      return action_S_IDe(uarch,logger,position);
    }
    
    bool change_state_S_INIT__S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_INIT -> S_ID"); }
      _action = (StateFunc)action_S_ID_p;
      _flush = (FlushFunc)flush_S_ID_p;
      uarch.issue(_instr);
      uarch.mID.add_instr(this);
      return true;
    }
    
    bool change_state_S_MM__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      return action_S_MM(uarch,logger,position);
    }
    
    bool change_state_S_MM__S_MMp(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMp"); }
      _action = (StateFunc)action_S_MMp_p;
      _flush = (FlushFunc)flush_S_MMp_p;
      return action_S_MMp(uarch,logger,position);
    }
    
    bool change_state_S_MM__S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_WB"); }
      _action = (StateFunc)action_S_WB_p;
      _flush = (FlushFunc)flush_S_WB_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    bool change_state_S_MM__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MM -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      return action_S_MMe(uarch,logger,position);
    }
    
    bool change_state_S_MMe__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMe -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      uarch.mMM.remove_instr();
      uarch.mWB.add_instr(this);
      return true;
    }
    
    bool change_state_S_MMp__S_MM(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMp -> S_MM"); }
      _action = (StateFunc)action_S_MM_p;
      _flush = (FlushFunc)flush_S_MM_p;
      return action_S_MM(uarch,logger,position);
    }
    
    bool change_state_S_MMp__S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_MMp -> S_MMe"); }
      _action = (StateFunc)action_S_MMe_p;
      _flush = (FlushFunc)flush_S_MMe_p;
      return action_S_MMe(uarch,logger,position);
    }
    
    bool change_state_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      uarch.commit(this);
      return true;
    }
    
    bool change_state_S_WB__S_WBe(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_WBe"); }
      _action = (StateFunc)action_S_WBe_p;
      _flush = (FlushFunc)flush_S_WBe_p;
      return action_S_WBe(uarch,logger,position);
    }
    
    bool change_state_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      uarch.commit(this);
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, uarch.mEX.capacity() ) ) {
            uarch.FwdEX.write(uarch,*this,logger, TrgEA ) ;
            if ( ! has_next_request (  ) ) {
                  if ( uarch.L1d.can_write(uarch,*this,logger) ) {
                        { uarch.exec_and_write(*this,logger, uarch.L1d )  ; if (has_exceptions()) goto HandleException; } ;
                    }
              } else if ( uarch.L1d.can_request_write(uarch,*this,logger) ) {
                  uarch.L1d.send_write_request(uarch,*this,logger) ;
              }
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
    static bool action_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EX(uarch,logger,position);
    }
    
    // State action S_EXe
    bool action_S_EXe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger) ) {
            return change_state_S_EXe__S_MMe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_EXe(uarch,logger,position);
    }
    
    // State action S_ID
    bool action_S_ID(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( Src.can_read_fp(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) && uarch.mEX.has_space(uarch,*this,logger) ) {
            Src.read(uarch,*this,logger, uarch.FwdEX , uarch.FwdMM ) ;
            Trg.allocate(uarch,*this,logger) ;
            return change_state_S_ID__S_IDp(uarch,logger,position) ;
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
    
    // State action S_IDp
    bool action_S_IDp(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mEX.has_space(uarch,*this,logger, uarch.mID.capacity() ) && uarch.L1d.can_request_write(uarch,*this,logger) ) {
            {uarch.exec_mem(*this,logger) ; if (has_exceptions()) goto HandleException; } ;
            uarch.L1d.send_write_request(uarch,*this,logger) ;
            return change_state_S_IDp__S_EX(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_IDp(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mID(*this,logger,position,true,branch_target());
        return change_state_S_IDp__S_IDe(uarch,logger,position);
      }
    }
    static bool action_S_IDp_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_IDp(uarch,logger,position);
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
      if ( exec_done (  ) && uarch.mWB.has_space(uarch,*this,logger, uarch.mMM.capacity() ) ) {
            uarch.FwdMM.write(uarch,*this,logger, TrgEA ) ;
            return change_state_S_MM__S_WB(uarch,logger,position) ;
      } else if ( ! has_next_request (  ) ) {
            if ( uarch.L1d.can_write(uarch,*this,logger) ) {
                  { uarch.exec_and_write(*this,logger, uarch.L1d )  ; if (has_exceptions()) goto HandleException; } ;
                  return change_state_S_MM__S_MM(uarch,logger,position) ;
              }
      } else if ( uarch.L1d.can_request_write(uarch,*this,logger) ) {
            uarch.L1d.send_write_request(uarch,*this,logger) ;
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
    static bool action_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MM(uarch,logger,position);
    }
    
    // State action S_MMe
    bool action_S_MMe(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mWB.has_space(uarch,*this,logger) ) {
            return change_state_S_MMe__S_WBe(uarch,logger,position) ;
      }
      return false;
    }
    static bool action_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MMe(uarch,logger,position);
    }
    
    // State action S_MMp
    bool action_S_MMp(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.L1d.can_write(uarch,*this,logger) ) {
            { uarch.exec_and_write(*this,logger, uarch.L1d )  ; if (has_exceptions()) goto HandleException; } ;
            uarch.FwdMM.write(uarch,*this,logger, TrgEA ) ;
            return change_state_S_MMp__S_MM(uarch,logger,position) ;
      }
      return false;
      HandleException: {
        if (logger) logger->logMessage("exception occurred");
        uarch.cancel_all_mem(*this);
        flush_S_MMp(uarch,logger);
        uarch.clear_exceptions(*this);
        uarch.taken_flush_mMM(*this,logger,position,true,branch_target());
        return change_state_S_MMp__S_MMe(uarch,logger,position);
      }
    }
    static bool action_S_MMp_p(Inst_P *x,OSM_P &uarch,Logger *logger,unsigned position) {
      return static_cast<ThisClass*>(x)->action_S_MMp(uarch,logger,position);
    }
    
    // State action S_WB
    bool action_S_WB(OSM_P &uarch,Logger *logger,unsigned position) {
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
    }
    static void flush_S_EX_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EX(uarch,logger);
    }
    
    // Flush action S_EXe
    void flush_S_EXe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_ID_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_ID(uarch,logger);
    }
    
    // Flush action S_IDe
    void flush_S_IDe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_IDe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDe(uarch,logger);
    }
    
    // Flush action S_IDp
    void flush_S_IDp(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_IDp_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_IDp(uarch,logger);
    }
    
    // Flush action S_INIT
    void flush_S_INIT(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_INIT_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_INIT(uarch,logger);
    }
    
    // Flush action S_MM
    void flush_S_MM(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_MM_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MM(uarch,logger);
    }
    
    // Flush action S_MMe
    void flush_S_MMe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMe(uarch,logger);
    }
    
    // Flush action S_MMp
    void flush_S_MMp(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_MMp_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMp(uarch,logger);
    }
    
    // Flush action S_WB
    void flush_S_WB(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
    }
    static void flush_S_WB_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WB(uarch,logger);
    }
    
    // Flush action S_WBe
    void flush_S_WBe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_WBe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBe(uarch,logger);
    }
    
  };
  
  byte_t *Mem_mem_t::hasData(addr_t addr, unsigned &size) const {
    return GlobalMemory.getPage(addr) + GlobalMemoryType::pageOffset(addr);
  }
  
  static CacheConfigMap::value_type _sym3705[] = {
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
  
  L1d_cache_t::L1d_cache_t(OSM_P &uarch, UadlMemoryIf &memory) :
  Cache(
      uarch,
      "L1d",
      memory,
      0,
      32,
      1, // address bandwidth
      1, // data bandwidth
      1, // max requests
      1, // allow bus wait
      0, // pre-emptive
      CacheConfigMap(&_sym3705[0],&_sym3705[12]),
      0  // with-data
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
            request.hint._cacheStatus,
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
  
  static CacheConfigMap::value_type _sym3706[] = {
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
  
  L1i_cache_t::L1i_cache_t(OSM_P &uarch, UadlMemoryIf &memory) :
  Cache(
      uarch,
      "L1i",
      memory,
      0,
      32,
      1, // address bandwidth
      1, // data bandwidth
      1, // max requests
      1, // allow bus wait
      0, // pre-emptive
      CacheConfigMap(&_sym3706[0],&_sym3706[12]),
      0  // with-data
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
            request.hint._cacheStatus,
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
  
  static CacheConfigMap::value_type _sym3707[] = {
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
  
  L2_cache_t::L2_cache_t(OSM_P &uarch, UadlMemoryIf &memory) :
  Cache(
      uarch,
      "L2",
      memory,
      0,
      32,
      1, // address bandwidth
      1, // data bandwidth
      1, // max requests
      1, // allow bus wait
      0, // pre-emptive
      CacheConfigMap(&_sym3707[0],&_sym3707[12]),
      0  // with-data
    ),
    _uarch(uarch)
  {
  }
  
  void L2_cache_t::reset_has_data()
  {
    if (_uarch.hadReset()) {
      // set up data array status if this is the first time step
      unsigned num_sets = _uarch._iss->L2_cache_get_sets();
      unsigned num_ways = _uarch._iss->L2_cache_get_ways();
      _hasData = HasData(num_sets,BVect(num_ways,false));
      for (unsigned set = 0; set < num_sets; set++) {
        for (unsigned way = 0; way < num_ways; way++) {
          _hasData[set][way] = _uarch._iss->L2_cache._array[set][way].valid;
        }
      }
    }
  }
  
  bool L2_cache_t::lookup(addr_t addr, unsigned &set, unsigned &way, unsigned &offset) const
  {
    const bool hit = _uarch._iss->L2_cache_find(adl::CacheLogRead, (int&)set, (int&)way, addr);
    if (hit) offset = _uarch._iss->L2_cache_get_offset(addr);
    return hit;
  }
  
  addr_t L2_cache_t::getTagAddr(unsigned set, unsigned way)
  {
    return _uarch._iss->L2_cache_get_tag(set, way);
  }
  
  void L2_cache_t::resize() {
    unsigned num_sets = _uarch._iss->L2_cache_get_sets();
    unsigned num_ways = _uarch._iss->L2_cache_get_ways();
    _hasData = HasData(num_sets,std::vector<bool>(num_ways));
  }
  
  void L2_cache_t::operationBegin(uadl::MemoryRequest &request)
  {
    Cache::operationBegin(request);
    
    if (_uarch._iss->L2_cache_begin(
            request.type, request.action,
            request.hint._cacheStatus,
            request.set, request.way, request.addr, request.replaceLine,
            hasCommand()
        ))
    {
      request.offset = _uarch._iss->L2_cache_get_offset(request.addr);
    } else {
      request.action = adl::CacheNone;
    }
    request.lookupDone = true;
  }
  
  void L2_cache_t::operationEnd(const uadl::MemoryRequest &request) const
  {
    if (request.action != adl::CacheNone) {
      _uarch._iss->L2_cache_end(request.type,request.action, request.set, request.way,request.addr, hasCommand());
    }
  }
  
  
  void L2_cache_t::clearValid(unsigned set, unsigned way)
  {
    _uarch._iss->L2_cache._array[set][way].valid = false;
  }
  
  bool L2_cache_t::is_write_through(addr_t addr)
  {
    return _uarch._iss->L2.is_write_through(adl::CacheWrite,addr);
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
  
  inline const adl::OpInfo &Inst_P::op(unsigned index) const { return _instr->op(index); };
  
  inline const uint32_t *Inst_P::opc() const { return _instr->opc(); };
  
  unsigned OSM_P::getInstrClassAttrByName(const std::string &attr)
  {
    return 0;
  }
  
  void OSM_P::alloc_mem_access_data()
  {
    for (MemAccessPool::iterator iter = _mem_access_pool.begin(); iter != _mem_access_pool.end(); ++iter) {
      (*iter)->_hint = new CacheStatusHint;
    }
  }
  
  UadlMemAccess &OSM_P::add_memory_request(InstrBase &instr)
  {
    return *instr.mem();
  }
  
  void OSM_P::set_memory_result(InstrBase &instr)
  {
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
    if (!_log_mem) return;
    UadlMemAccess *ma = _mem_access_pool.alloc();
    CacheStatusHint *hint = ma->_hint;
    _iss->update_data_cache_status(hint->_cacheStatus,m._type,m._ra);
    hint->_storeBufEnabled = L1d_obj.storeBufEnabled();
    ma->reset(_memId++,m);
    _cur_instr->add_mem(ma);
  }
  
  static unsigned initFetchSizes[] = { 8, 4,  };
  FetchSizes OSM_P::_fetchSizes(&initFetchSizes[0],&initFetchSizes[2]);
  
  //
  // User-defined helper functions.
  //
  
   unsigned OSM_P:: signed_mul_div_delay ( bits < 32 > x , bits < 32 > y ) {
        return ( x == 0 || y == 0 ) ? 0 : 9 ;
  }
  
  // Factory function for instruction "add".
  Inst_P *OSM_P::get_instr_add(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<add_sfx_Src, add_sfx_Trg> *ic = (Inst_sfx<add_sfx_Src, add_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "add.".
  Inst_P *OSM_P::get_instr_add_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<add__sfx_Src, add__sfx_Trg> *ic = (Inst_sfx<add__sfx_Src, add__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "addc".
  Inst_P *OSM_P::get_instr_addc(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addc_sfx_Src, addc_sfx_Trg> *ic = (Inst_sfx<addc_sfx_Src, addc_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "addc.".
  Inst_P *OSM_P::get_instr_addc_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addc__sfx_Src, addc__sfx_Trg> *ic = (Inst_sfx<addc__sfx_Src, addc__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    ic->_masks[1] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "addco".
  Inst_P *OSM_P::get_instr_addco(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addco_sfx_Src, addco_sfx_Trg> *ic = (Inst_sfx<addco_sfx_Src, addco_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0xc0000000;
    ic->_masks[0] = 0xe0000000;
    return ic;
  }
  
  // Factory function for instruction "addco.".
  Inst_P *OSM_P::get_instr_addco_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addco__sfx_Src, addco__sfx_Trg> *ic = (Inst_sfx<addco__sfx_Src, addco__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0xc0000000;
    ic->_masks[1] = 0xe0000000;
    return ic;
  }
  
  // Factory function for instruction "adde".
  Inst_P *OSM_P::get_instr_adde(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<adde_sfx_Src, adde_sfx_Trg> *ic = (Inst_sfx<adde_sfx_Src, adde_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0x20000000;
    ic->_masks[0] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "adde.".
  Inst_P *OSM_P::get_instr_adde_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<adde__sfx_Src, adde__sfx_Trg> *ic = (Inst_sfx<adde__sfx_Src, adde__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0xa0000000;
    ic->_masks[1] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "addeo".
  Inst_P *OSM_P::get_instr_addeo(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addeo_sfx_Src, addeo_sfx_Trg> *ic = (Inst_sfx<addeo_sfx_Src, addeo_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0xe0000000;
    ic->_masks[0] = 0xe0000000;
    return ic;
  }
  
  // Factory function for instruction "addeo.".
  Inst_P *OSM_P::get_instr_addeo_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addeo__sfx_Src, addeo__sfx_Trg> *ic = (Inst_sfx<addeo__sfx_Src, addeo__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0xe0000000;
    ic->_masks[1] = 0xe0000000;
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
    Inst_sfx<addic_sfx_Src, addic_sfx_Trg> *ic = (Inst_sfx<addic_sfx_Src, addic_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "addic.".
  Inst_P *OSM_P::get_instr_addic_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addic__sfx_Src, addic__sfx_Trg> *ic = (Inst_sfx<addic__sfx_Src, addic__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    ic->_masks[1] = 0x20000000;
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
    Inst_sfx<addme_sfx_Src, addme_sfx_Trg> *ic = (Inst_sfx<addme_sfx_Src, addme_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0x20000000;
    ic->_masks[0] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "addme.".
  Inst_P *OSM_P::get_instr_addme_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addme__sfx_Src, addme__sfx_Trg> *ic = (Inst_sfx<addme__sfx_Src, addme__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0xa0000000;
    ic->_masks[1] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "addmeo".
  Inst_P *OSM_P::get_instr_addmeo(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addmeo_sfx_Src, addmeo_sfx_Trg> *ic = (Inst_sfx<addmeo_sfx_Src, addmeo_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0xe0000000;
    ic->_masks[0] = 0xe0000000;
    return ic;
  }
  
  // Factory function for instruction "addmeo.".
  Inst_P *OSM_P::get_instr_addmeo_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addmeo__sfx_Src, addmeo__sfx_Trg> *ic = (Inst_sfx<addmeo__sfx_Src, addmeo__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0xe0000000;
    ic->_masks[1] = 0xe0000000;
    return ic;
  }
  
  // Factory function for instruction "addo".
  Inst_P *OSM_P::get_instr_addo(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addo_sfx_Src, addo_sfx_Trg> *ic = (Inst_sfx<addo_sfx_Src, addo_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0xc0000000;
    ic->_masks[0] = 0xc0000000;
    return ic;
  }
  
  // Factory function for instruction "addo.".
  Inst_P *OSM_P::get_instr_addo_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addo__sfx_Src, addo__sfx_Trg> *ic = (Inst_sfx<addo__sfx_Src, addo__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0xc0000000;
    ic->_masks[1] = 0xc0000000;
    return ic;
  }
  
  // Factory function for instruction "addze".
  Inst_P *OSM_P::get_instr_addze(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addze_sfx_Src, addze_sfx_Trg> *ic = (Inst_sfx<addze_sfx_Src, addze_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0x20000000;
    ic->_masks[0] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "addze.".
  Inst_P *OSM_P::get_instr_addze_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addze__sfx_Src, addze__sfx_Trg> *ic = (Inst_sfx<addze__sfx_Src, addze__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0xa0000000;
    ic->_masks[1] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "addzeo".
  Inst_P *OSM_P::get_instr_addzeo(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addzeo_sfx_Src, addzeo_sfx_Trg> *ic = (Inst_sfx<addzeo_sfx_Src, addzeo_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0xe0000000;
    ic->_masks[0] = 0xe0000000;
    return ic;
  }
  
  // Factory function for instruction "addzeo.".
  Inst_P *OSM_P::get_instr_addzeo_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addzeo__sfx_Src, addzeo__sfx_Trg> *ic = (Inst_sfx<addzeo__sfx_Src, addzeo__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0xe0000000;
    ic->_masks[1] = 0xe0000000;
    return ic;
  }
  
  // Factory function for instruction "and".
  Inst_P *OSM_P::get_instr_and(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<and_sfx_Src, and_sfx_Trg> *ic = (Inst_sfx<and_sfx_Src, and_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "and.".
  Inst_P *OSM_P::get_instr_and_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<and__sfx_Src, and__sfx_Trg> *ic = (Inst_sfx<and__sfx_Src, and__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "andc".
  Inst_P *OSM_P::get_instr_andc(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<andc_sfx_Src, andc_sfx_Trg> *ic = (Inst_sfx<andc_sfx_Src, andc_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "andc.".
  Inst_P *OSM_P::get_instr_andc_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<andc__sfx_Src, andc__sfx_Trg> *ic = (Inst_sfx<andc__sfx_Src, andc__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "andi.".
  Inst_P *OSM_P::get_instr_andi_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<andi__sfx_Src, andi__sfx_Trg> *ic = (Inst_sfx<andi__sfx_Src, andi__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "andis.".
  Inst_P *OSM_P::get_instr_andis_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<andis__sfx_Src, andis__sfx_Trg> *ic = (Inst_sfx<andis__sfx_Src, andis__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "b".
  Inst_P *OSM_P::get_instr_b(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_branch<b_branch___other__> *ic = (Inst_branch<b_branch___other__>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::branchClass,p);
    return ic;
  }
  
  // Factory function for instruction "ba".
  Inst_P *OSM_P::get_instr_ba(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<DummyOp, ba_sfx_Trg> *ic = (Inst_sfx<DummyOp, ba_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "bc".
  Inst_P *OSM_P::get_instr_bc(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_branch<bc_branch___other__> *ic = (Inst_branch<bc_branch___other__>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::branchClass,p);
    return ic;
  }
  
  // Factory function for instruction "bca".
  Inst_P *OSM_P::get_instr_bca(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<bca_sfx_Src, bca_sfx_Trg> *ic = (Inst_sfx<bca_sfx_Src, bca_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "bcctr".
  Inst_P *OSM_P::get_instr_bcctr(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_branch<bcctr_branch___other__> *ic = (Inst_branch<bcctr_branch___other__>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::branchClass,p);
    return ic;
  }
  
  // Factory function for instruction "bcctrl".
  Inst_P *OSM_P::get_instr_bcctrl(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_branch<bcctrl_branch___other__> *ic = (Inst_branch<bcctrl_branch___other__>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::branchClass,p);
    return ic;
  }
  
  // Factory function for instruction "bcl".
  Inst_P *OSM_P::get_instr_bcl(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_branch<bcl_branch___other__> *ic = (Inst_branch<bcl_branch___other__>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::branchClass,p);
    return ic;
  }
  
  // Factory function for instruction "bcla".
  Inst_P *OSM_P::get_instr_bcla(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<bcla_sfx_Src, bcla_sfx_Trg> *ic = (Inst_sfx<bcla_sfx_Src, bcla_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "bclr".
  Inst_P *OSM_P::get_instr_bclr(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_branch<bclr_branch___other__> *ic = (Inst_branch<bclr_branch___other__>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::branchClass,p);
    return ic;
  }
  
  // Factory function for instruction "bclrl".
  Inst_P *OSM_P::get_instr_bclrl(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_branch<bclrl_branch___other__> *ic = (Inst_branch<bclrl_branch___other__>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::branchClass,p);
    return ic;
  }
  
  // Factory function for instruction "bl".
  Inst_P *OSM_P::get_instr_bl(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_branch<bl_branch___other__> *ic = (Inst_branch<bl_branch___other__>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::branchClass,p);
    return ic;
  }
  
  // Factory function for instruction "bla".
  Inst_P *OSM_P::get_instr_bla(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<bla_sfx_Src, bla_sfx_Trg> *ic = (Inst_sfx<bla_sfx_Src, bla_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "cmp".
  Inst_P *OSM_P::get_instr_cmp(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<cmp_sfx_Src, cmp_sfx_Trg> *ic = (Inst_sfx<cmp_sfx_Src, cmp_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[2] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "cmpi".
  Inst_P *OSM_P::get_instr_cmpi(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<cmpi_sfx_Src, cmpi_sfx_Trg> *ic = (Inst_sfx<cmpi_sfx_Src, cmpi_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "cmpl".
  Inst_P *OSM_P::get_instr_cmpl(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<cmpl_sfx_Src, cmpl_sfx_Trg> *ic = (Inst_sfx<cmpl_sfx_Src, cmpl_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[3] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "cmpli".
  Inst_P *OSM_P::get_instr_cmpli(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<cmpli_sfx_Src, cmpli_sfx_Trg> *ic = (Inst_sfx<cmpli_sfx_Src, cmpli_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "cntlzd".
  Inst_P *OSM_P::get_instr_cntlzd(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<cntlzd_sfx_Src, cntlzd_sfx_Trg> *ic = (Inst_sfx<cntlzd_sfx_Src, cntlzd_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "cntlzd.".
  Inst_P *OSM_P::get_instr_cntlzd_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<cntlzd__sfx_Src, cntlzd__sfx_Trg> *ic = (Inst_sfx<cntlzd__sfx_Src, cntlzd__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[2] = 0x80000000;
    ic->_masks[3] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "cntlzw".
  Inst_P *OSM_P::get_instr_cntlzw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<cntlzw_sfx_Src, cntlzw_sfx_Trg> *ic = (Inst_sfx<cntlzw_sfx_Src, cntlzw_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "cntlzw.".
  Inst_P *OSM_P::get_instr_cntlzw_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<cntlzw__sfx_Src, cntlzw__sfx_Trg> *ic = (Inst_sfx<cntlzw__sfx_Src, cntlzw__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[2] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "crand".
  Inst_P *OSM_P::get_instr_crand(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<crand_sfx_Src, crand_sfx_Trg> *ic = (Inst_sfx<crand_sfx_Src, crand_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "crandc".
  Inst_P *OSM_P::get_instr_crandc(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<crandc_sfx_Src, crandc_sfx_Trg> *ic = (Inst_sfx<crandc_sfx_Src, crandc_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "creqv".
  Inst_P *OSM_P::get_instr_creqv(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<creqv_sfx_Src, creqv_sfx_Trg> *ic = (Inst_sfx<creqv_sfx_Src, creqv_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "crnand".
  Inst_P *OSM_P::get_instr_crnand(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<crnand_sfx_Src, crnand_sfx_Trg> *ic = (Inst_sfx<crnand_sfx_Src, crnand_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "crnor".
  Inst_P *OSM_P::get_instr_crnor(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<crnor_sfx_Src, crnor_sfx_Trg> *ic = (Inst_sfx<crnor_sfx_Src, crnor_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "cror".
  Inst_P *OSM_P::get_instr_cror(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<cror_sfx_Src, cror_sfx_Trg> *ic = (Inst_sfx<cror_sfx_Src, cror_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "crorc".
  Inst_P *OSM_P::get_instr_crorc(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<crorc_sfx_Src, crorc_sfx_Trg> *ic = (Inst_sfx<crorc_sfx_Src, crorc_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "crxor".
  Inst_P *OSM_P::get_instr_crxor(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<crxor_sfx_Src, crxor_sfx_Trg> *ic = (Inst_sfx<crxor_sfx_Src, crxor_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "dcba".
  Inst_P *OSM_P::get_instr_dcba(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<dcba_sfx_Src, DummyOp> *ic = (Inst_sfx<dcba_sfx_Src, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "dcbf".
  Inst_P *OSM_P::get_instr_dcbf(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<dcbf_sfx_Src, DummyOp> *ic = (Inst_sfx<dcbf_sfx_Src, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "dcbi".
  Inst_P *OSM_P::get_instr_dcbi(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<dcbi_sfx_Src, DummyOp> *ic = (Inst_sfx<dcbi_sfx_Src, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "dcbst".
  Inst_P *OSM_P::get_instr_dcbst(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<dcbst_sfx_Src, DummyOp> *ic = (Inst_sfx<dcbst_sfx_Src, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "dcbt".
  Inst_P *OSM_P::get_instr_dcbt(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<dcbt_sfx_Src, DummyOp> *ic = (Inst_sfx<dcbt_sfx_Src, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "dcbz".
  Inst_P *OSM_P::get_instr_dcbz(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<dcbz_sfx_Src, DummyOp> *ic = (Inst_sfx<dcbz_sfx_Src, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "divd".
  Inst_P *OSM_P::get_instr_divd(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<divd_sfx_Src, divd_sfx_Trg> *ic = (Inst_sfx<divd_sfx_Src, divd_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "divd.".
  Inst_P *OSM_P::get_instr_divd_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<divd__sfx_Src, divd__sfx_Trg> *ic = (Inst_sfx<divd__sfx_Src, divd__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "divdo".
  Inst_P *OSM_P::get_instr_divdo(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<divdo_sfx_Src, divdo_sfx_Trg> *ic = (Inst_sfx<divdo_sfx_Src, divdo_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0xc0000000;
    ic->_masks[0] = 0xc0000000;
    return ic;
  }
  
  // Factory function for instruction "divdo.".
  Inst_P *OSM_P::get_instr_divdo_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<divdo__sfx_Src, divdo__sfx_Trg> *ic = (Inst_sfx<divdo__sfx_Src, divdo__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0xc0000000;
    ic->_masks[1] = 0xc0000000;
    return ic;
  }
  
  // Factory function for instruction "divdu".
  Inst_P *OSM_P::get_instr_divdu(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<divdu_sfx_Src, divdu_sfx_Trg> *ic = (Inst_sfx<divdu_sfx_Src, divdu_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "divdu.".
  Inst_P *OSM_P::get_instr_divdu_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<divdu__sfx_Src, divdu__sfx_Trg> *ic = (Inst_sfx<divdu__sfx_Src, divdu__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "divduo".
  Inst_P *OSM_P::get_instr_divduo(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<divduo_sfx_Src, divduo_sfx_Trg> *ic = (Inst_sfx<divduo_sfx_Src, divduo_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0xc0000000;
    ic->_masks[0] = 0xc0000000;
    return ic;
  }
  
  // Factory function for instruction "divduo.".
  Inst_P *OSM_P::get_instr_divduo_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<divduo__sfx_Src, divduo__sfx_Trg> *ic = (Inst_sfx<divduo__sfx_Src, divduo__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0xc0000000;
    ic->_masks[1] = 0xc0000000;
    return ic;
  }
  
  // Factory function for instruction "divw".
  Inst_P *OSM_P::get_instr_divw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_signedMulDiv<divw_signedMulDiv_Src, divw_signedMulDiv_Src1, divw_signedMulDiv_Src2, divw_signedMulDiv_Trg> *ic = (Inst_signedMulDiv<divw_signedMulDiv_Src, divw_signedMulDiv_Src1, divw_signedMulDiv_Src2, divw_signedMulDiv_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::signedMulDivClass,p);
    return ic;
  }
  
  // Factory function for instruction "divw.".
  Inst_P *OSM_P::get_instr_divw_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<divw__sfx_Src, divw__sfx_Trg> *ic = (Inst_sfx<divw__sfx_Src, divw__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "divwo".
  Inst_P *OSM_P::get_instr_divwo(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<divwo_sfx_Src, divwo_sfx_Trg> *ic = (Inst_sfx<divwo_sfx_Src, divwo_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0xc0000000;
    ic->_masks[0] = 0xc0000000;
    return ic;
  }
  
  // Factory function for instruction "divwo.".
  Inst_P *OSM_P::get_instr_divwo_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<divwo__sfx_Src, divwo__sfx_Trg> *ic = (Inst_sfx<divwo__sfx_Src, divwo__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0xc0000000;
    ic->_masks[1] = 0xc0000000;
    return ic;
  }
  
  // Factory function for instruction "divwu".
  Inst_P *OSM_P::get_instr_divwu(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<divwu_sfx_Src, divwu_sfx_Trg> *ic = (Inst_sfx<divwu_sfx_Src, divwu_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "divwu.".
  Inst_P *OSM_P::get_instr_divwu_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<divwu__sfx_Src, divwu__sfx_Trg> *ic = (Inst_sfx<divwu__sfx_Src, divwu__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "divwuo".
  Inst_P *OSM_P::get_instr_divwuo(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<divwuo_sfx_Src, divwuo_sfx_Trg> *ic = (Inst_sfx<divwuo_sfx_Src, divwuo_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0xc0000000;
    ic->_masks[0] = 0xc0000000;
    return ic;
  }
  
  // Factory function for instruction "divwuo.".
  Inst_P *OSM_P::get_instr_divwuo_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<divwuo__sfx_Src, divwuo__sfx_Trg> *ic = (Inst_sfx<divwuo__sfx_Src, divwuo__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0xc0000000;
    ic->_masks[1] = 0xc0000000;
    return ic;
  }
  
  // Factory function for instruction "eqv".
  Inst_P *OSM_P::get_instr_eqv(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<eqv_sfx_Src, eqv_sfx_Trg> *ic = (Inst_sfx<eqv_sfx_Src, eqv_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "eqv.".
  Inst_P *OSM_P::get_instr_eqv_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<eqv__sfx_Src, eqv__sfx_Trg> *ic = (Inst_sfx<eqv__sfx_Src, eqv__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "extsb".
  Inst_P *OSM_P::get_instr_extsb(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<extsb_sfx_Src, extsb_sfx_Trg> *ic = (Inst_sfx<extsb_sfx_Src, extsb_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0xff;
    return ic;
  }
  
  // Factory function for instruction "extsb.".
  Inst_P *OSM_P::get_instr_extsb_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<extsb__sfx_Src, extsb__sfx_Trg> *ic = (Inst_sfx<extsb__sfx_Src, extsb__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[3] = 0xff;
    ic->_masks[4] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "extsh".
  Inst_P *OSM_P::get_instr_extsh(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<extsh_sfx_Src, extsh_sfx_Trg> *ic = (Inst_sfx<extsh_sfx_Src, extsh_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0xffff;
    return ic;
  }
  
  // Factory function for instruction "extsh.".
  Inst_P *OSM_P::get_instr_extsh_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<extsh__sfx_Src, extsh__sfx_Trg> *ic = (Inst_sfx<extsh__sfx_Src, extsh__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[5] = 0xffff;
    ic->_masks[6] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "extsw".
  Inst_P *OSM_P::get_instr_extsw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<extsw_sfx_Src, extsw_sfx_Trg> *ic = (Inst_sfx<extsw_sfx_Src, extsw_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "extsw.".
  Inst_P *OSM_P::get_instr_extsw_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<extsw__sfx_Src, extsw__sfx_Trg> *ic = (Inst_sfx<extsw__sfx_Src, extsw__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "halt".
  Inst_P *OSM_P::get_instr_halt(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<DummyOp, DummyOp> *ic = (Inst_sfx<DummyOp, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "icbi".
  Inst_P *OSM_P::get_instr_icbi(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<icbi_sfx_Src, DummyOp> *ic = (Inst_sfx<icbi_sfx_Src, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "icbt".
  Inst_P *OSM_P::get_instr_icbt(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<icbt_sfx_Src, DummyOp> *ic = (Inst_sfx<icbt_sfx_Src, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "isync".
  Inst_P *OSM_P::get_instr_isync(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<DummyOp, DummyOp> *ic = (Inst_sfx<DummyOp, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "lbz".
  Inst_P *OSM_P::get_instr_lbz(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lbz_load_Src, lbz_load_Trg, DummyOp> *ic = (Inst_load<lbz_load_Src, lbz_load_Trg, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::loadClass,p);
    return ic;
  }
  
  // Factory function for instruction "lbzu".
  Inst_P *OSM_P::get_instr_lbzu(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lbzu_load_Src, lbzu_load_Trg, lbzu_load_TrgEA> *ic = (Inst_load<lbzu_load_Src, lbzu_load_Trg, lbzu_load_TrgEA>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::loadClass,p);
    return ic;
  }
  
  // Factory function for instruction "lbzux".
  Inst_P *OSM_P::get_instr_lbzux(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lbzux_load_Src, lbzux_load_Trg, lbzux_load_TrgEA> *ic = (Inst_load<lbzux_load_Src, lbzux_load_Trg, lbzux_load_TrgEA>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::loadClass,p);
    return ic;
  }
  
  // Factory function for instruction "lbzx".
  Inst_P *OSM_P::get_instr_lbzx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lbzx_load_Src, lbzx_load_Trg, DummyOp> *ic = (Inst_load<lbzx_load_Src, lbzx_load_Trg, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::loadClass,p);
    return ic;
  }
  
  // Factory function for instruction "ld".
  Inst_P *OSM_P::get_instr_ld(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<ld_sfx_Src, ld_sfx_Trg> *ic = (Inst_sfx<ld_sfx_Src, ld_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "ldu".
  Inst_P *OSM_P::get_instr_ldu(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<ldu_sfx_Src, ldu_sfx_Trg> *ic = (Inst_sfx<ldu_sfx_Src, ldu_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "ldux".
  Inst_P *OSM_P::get_instr_ldux(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<ldux_sfx_Src, ldux_sfx_Trg> *ic = (Inst_sfx<ldux_sfx_Src, ldux_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "ldx".
  Inst_P *OSM_P::get_instr_ldx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<ldx_sfx_Src, ldx_sfx_Trg> *ic = (Inst_sfx<ldx_sfx_Src, ldx_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "lha".
  Inst_P *OSM_P::get_instr_lha(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lha_load_Src, lha_load_Trg, DummyOp> *ic = (Inst_load<lha_load_Src, lha_load_Trg, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::loadClass,p);
    return ic;
  }
  
  // Factory function for instruction "lhau".
  Inst_P *OSM_P::get_instr_lhau(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<lhau_sfx_Src, lhau_sfx_Trg> *ic = (Inst_sfx<lhau_sfx_Src, lhau_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "lhaux".
  Inst_P *OSM_P::get_instr_lhaux(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<lhaux_sfx_Src, lhaux_sfx_Trg> *ic = (Inst_sfx<lhaux_sfx_Src, lhaux_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "lhax".
  Inst_P *OSM_P::get_instr_lhax(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lhax_load_Src, lhax_load_Trg, DummyOp> *ic = (Inst_load<lhax_load_Src, lhax_load_Trg, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::loadClass,p);
    return ic;
  }
  
  // Factory function for instruction "lhbrx".
  Inst_P *OSM_P::get_instr_lhbrx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<lhbrx_sfx_Src, lhbrx_sfx_Trg> *ic = (Inst_sfx<lhbrx_sfx_Src, lhbrx_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "lhz".
  Inst_P *OSM_P::get_instr_lhz(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lhz_load_Src, lhz_load_Trg, DummyOp> *ic = (Inst_load<lhz_load_Src, lhz_load_Trg, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::loadClass,p);
    return ic;
  }
  
  // Factory function for instruction "lhzu".
  Inst_P *OSM_P::get_instr_lhzu(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lhzu_load_Src, lhzu_load_Trg, lhzu_load_TrgEA> *ic = (Inst_load<lhzu_load_Src, lhzu_load_Trg, lhzu_load_TrgEA>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::loadClass,p);
    return ic;
  }
  
  // Factory function for instruction "lhzux".
  Inst_P *OSM_P::get_instr_lhzux(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lhzux_load_Src, lhzux_load_Trg, lhzux_load_TrgEA> *ic = (Inst_load<lhzux_load_Src, lhzux_load_Trg, lhzux_load_TrgEA>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::loadClass,p);
    return ic;
  }
  
  // Factory function for instruction "lhzx".
  Inst_P *OSM_P::get_instr_lhzx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lhzx_load_Src, lhzx_load_Trg, DummyOp> *ic = (Inst_load<lhzx_load_Src, lhzx_load_Trg, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::loadClass,p);
    return ic;
  }
  
  // Factory function for instruction "lmw".
  Inst_P *OSM_P::get_instr_lmw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_lmw<lmw_lmw_Src, lmw_lmw_Trg> *ic = (Inst_lmw<lmw_lmw_Src, lmw_lmw_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::lmwClass,p);
    return ic;
  }
  
  // Factory function for instruction "lwa".
  Inst_P *OSM_P::get_instr_lwa(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<lwa_sfx_Src, lwa_sfx_Trg> *ic = (Inst_sfx<lwa_sfx_Src, lwa_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "lwarx".
  Inst_P *OSM_P::get_instr_lwarx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<lwarx_sfx_Src, lwarx_sfx_Trg> *ic = (Inst_sfx<lwarx_sfx_Src, lwarx_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "lwaux".
  Inst_P *OSM_P::get_instr_lwaux(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<lwaux_sfx_Src, lwaux_sfx_Trg> *ic = (Inst_sfx<lwaux_sfx_Src, lwaux_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "lwax".
  Inst_P *OSM_P::get_instr_lwax(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<lwax_sfx_Src, lwax_sfx_Trg> *ic = (Inst_sfx<lwax_sfx_Src, lwax_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "lwbrx".
  Inst_P *OSM_P::get_instr_lwbrx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<lwbrx_sfx_Src, lwbrx_sfx_Trg> *ic = (Inst_sfx<lwbrx_sfx_Src, lwbrx_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "lwz".
  Inst_P *OSM_P::get_instr_lwz(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lwz_load_Src, lwz_load_Trg, DummyOp> *ic = (Inst_load<lwz_load_Src, lwz_load_Trg, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::loadClass,p);
    return ic;
  }
  
  // Factory function for instruction "lwzu".
  Inst_P *OSM_P::get_instr_lwzu(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lwzu_load_Src, lwzu_load_Trg, lwzu_load_TrgEA> *ic = (Inst_load<lwzu_load_Src, lwzu_load_Trg, lwzu_load_TrgEA>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::loadClass,p);
    return ic;
  }
  
  // Factory function for instruction "lwzux".
  Inst_P *OSM_P::get_instr_lwzux(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lwzux_load_Src, lwzux_load_Trg, lwzux_load_TrgEA> *ic = (Inst_load<lwzux_load_Src, lwzux_load_Trg, lwzux_load_TrgEA>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::loadClass,p);
    return ic;
  }
  
  // Factory function for instruction "lwzx".
  Inst_P *OSM_P::get_instr_lwzx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lwzx_load_Src, lwzx_load_Trg, DummyOp> *ic = (Inst_load<lwzx_load_Src, lwzx_load_Trg, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::loadClass,p);
    return ic;
  }
  
  // Factory function for instruction "mbar".
  Inst_P *OSM_P::get_instr_mbar(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<DummyOp, DummyOp> *ic = (Inst_sfx<DummyOp, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "mcrf".
  Inst_P *OSM_P::get_instr_mcrf(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<mcrf_sfx_Src, mcrf_sfx_Trg> *ic = (Inst_sfx<mcrf_sfx_Src, mcrf_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "mcrxr".
  Inst_P *OSM_P::get_instr_mcrxr(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<mcrxr_sfx_Src, mcrxr_sfx_Trg> *ic = (Inst_sfx<mcrxr_sfx_Src, mcrxr_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0xf0000000;
    ic->_masks[1] = 0xf0000000;
    return ic;
  }
  
  // Factory function for instruction "mfcr".
  Inst_P *OSM_P::get_instr_mfcr(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<mfcr_sfx_Src, mfcr_sfx_Trg> *ic = (Inst_sfx<mfcr_sfx_Src, mfcr_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "mfmsr".
  Inst_P *OSM_P::get_instr_mfmsr(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<mfmsr_sfx_Src, mfmsr_sfx_Trg> *ic = (Inst_sfx<mfmsr_sfx_Src, mfmsr_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "mfspr".
  Inst_P *OSM_P::get_instr_mfspr(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_mftspr<DummyOp, mfspr_mftspr_Src, mfspr_mftspr_Trg> *ic = (Inst_mftspr<DummyOp, mfspr_mftspr_Src, mfspr_mftspr_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::mftsprClass,p);
    return ic;
  }
  
  // Factory function for instruction "msync".
  Inst_P *OSM_P::get_instr_msync(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_dispatchSerial *ic = (Inst_dispatchSerial*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::dispatchSerialClass,p);
    return ic;
  }
  
  // Factory function for instruction "mtcrf".
  Inst_P *OSM_P::get_instr_mtcrf(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<mtcrf_sfx_Src, mtcrf_sfx_Trg> *ic = (Inst_sfx<mtcrf_sfx_Src, mtcrf_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "mtmsr".
  Inst_P *OSM_P::get_instr_mtmsr(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<mtmsr_sfx_Src, mtmsr_sfx_Trg> *ic = (Inst_sfx<mtmsr_sfx_Src, mtmsr_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "mtocrf".
  Inst_P *OSM_P::get_instr_mtocrf(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<mtocrf_sfx_Src, mtocrf_sfx_Trg> *ic = (Inst_sfx<mtocrf_sfx_Src, mtocrf_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "mtspr".
  Inst_P *OSM_P::get_instr_mtspr(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_mftspr<mtspr_mftspr_CheckDelay, mtspr_mftspr_Src, mtspr_mftspr_Trg> *ic = (Inst_mftspr<mtspr_mftspr_CheckDelay, mtspr_mftspr_Src, mtspr_mftspr_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::mftsprClass,p);
    return ic;
  }
  
  // Factory function for instruction "mulhd".
  Inst_P *OSM_P::get_instr_mulhd(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<mulhd_sfx_Src, mulhd_sfx_Trg> *ic = (Inst_sfx<mulhd_sfx_Src, mulhd_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "mulhd.".
  Inst_P *OSM_P::get_instr_mulhd_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<mulhd__sfx_Src, mulhd__sfx_Trg> *ic = (Inst_sfx<mulhd__sfx_Src, mulhd__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "mulhdu".
  Inst_P *OSM_P::get_instr_mulhdu(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<mulhdu_sfx_Src, mulhdu_sfx_Trg> *ic = (Inst_sfx<mulhdu_sfx_Src, mulhdu_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "mulhdu.".
  Inst_P *OSM_P::get_instr_mulhdu_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<mulhdu__sfx_Src, mulhdu__sfx_Trg> *ic = (Inst_sfx<mulhdu__sfx_Src, mulhdu__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "mulhw".
  Inst_P *OSM_P::get_instr_mulhw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_signedMulDiv<mulhw_signedMulDiv_Src, mulhw_signedMulDiv_Src1, mulhw_signedMulDiv_Src2, mulhw_signedMulDiv_Trg> *ic = (Inst_signedMulDiv<mulhw_signedMulDiv_Src, mulhw_signedMulDiv_Src1, mulhw_signedMulDiv_Src2, mulhw_signedMulDiv_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::signedMulDivClass,p);
    return ic;
  }
  
  // Factory function for instruction "mulhw.".
  Inst_P *OSM_P::get_instr_mulhw_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<mulhw__sfx_Src, mulhw__sfx_Trg> *ic = (Inst_sfx<mulhw__sfx_Src, mulhw__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "mulhwu".
  Inst_P *OSM_P::get_instr_mulhwu(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<mulhwu_sfx_Src, mulhwu_sfx_Trg> *ic = (Inst_sfx<mulhwu_sfx_Src, mulhwu_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "mulhwu.".
  Inst_P *OSM_P::get_instr_mulhwu_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<mulhwu__sfx_Src, mulhwu__sfx_Trg> *ic = (Inst_sfx<mulhwu__sfx_Src, mulhwu__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "mulld".
  Inst_P *OSM_P::get_instr_mulld(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<mulld_sfx_Src, mulld_sfx_Trg> *ic = (Inst_sfx<mulld_sfx_Src, mulld_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "mulld.".
  Inst_P *OSM_P::get_instr_mulld_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<mulld__sfx_Src, mulld__sfx_Trg> *ic = (Inst_sfx<mulld__sfx_Src, mulld__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "mulldo".
  Inst_P *OSM_P::get_instr_mulldo(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<mulldo_sfx_Src, mulldo_sfx_Trg> *ic = (Inst_sfx<mulldo_sfx_Src, mulldo_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0xc0000000;
    ic->_masks[0] = 0xc0000000;
    return ic;
  }
  
  // Factory function for instruction "mulldo.".
  Inst_P *OSM_P::get_instr_mulldo_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<mulldo__sfx_Src, mulldo__sfx_Trg> *ic = (Inst_sfx<mulldo__sfx_Src, mulldo__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0xc0000000;
    ic->_masks[1] = 0xc0000000;
    return ic;
  }
  
  // Factory function for instruction "mulli".
  Inst_P *OSM_P::get_instr_mulli(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<mulli_sfx_Src, mulli_sfx_Trg> *ic = (Inst_sfx<mulli_sfx_Src, mulli_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "mullw".
  Inst_P *OSM_P::get_instr_mullw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_signedMulDiv<mullw_signedMulDiv_Src, mullw_signedMulDiv_Src1, mullw_signedMulDiv_Src2, mullw_signedMulDiv_Trg> *ic = (Inst_signedMulDiv<mullw_signedMulDiv_Src, mullw_signedMulDiv_Src1, mullw_signedMulDiv_Src2, mullw_signedMulDiv_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::signedMulDivClass,p);
    return ic;
  }
  
  // Factory function for instruction "mullw.".
  Inst_P *OSM_P::get_instr_mullw_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_signedMulCr<mullw__signedMulCr_Src, mullw__signedMulCr_Src1, mullw__signedMulCr_Src2, mullw__signedMulCr_Trg> *ic = (Inst_signedMulCr<mullw__signedMulCr_Src, mullw__signedMulCr_Src1, mullw__signedMulCr_Src2, mullw__signedMulCr_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::signedMulCrClass,p);
    ic->_masks[0] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "mullwo".
  Inst_P *OSM_P::get_instr_mullwo(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_signedMulCr<mullwo_signedMulCr_Src, mullwo_signedMulCr_Src1, mullwo_signedMulCr_Src2, mullwo_signedMulCr_Trg> *ic = (Inst_signedMulCr<mullwo_signedMulCr_Src, mullwo_signedMulCr_Src1, mullwo_signedMulCr_Src2, mullwo_signedMulCr_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::signedMulCrClass,p);
    ic->_masks[0] = 0xc0000000;
    ic->_masks[1] = 0xc0000000;
    return ic;
  }
  
  // Factory function for instruction "mullwo.".
  Inst_P *OSM_P::get_instr_mullwo_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_signedMulCr<mullwo__signedMulCr_Src, mullwo__signedMulCr_Src1, mullwo__signedMulCr_Src2, mullwo__signedMulCr_Trg> *ic = (Inst_signedMulCr<mullwo__signedMulCr_Src, mullwo__signedMulCr_Src1, mullwo__signedMulCr_Src2, mullwo__signedMulCr_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::signedMulCrClass,p);
    ic->_masks[0] = 0xc0000000;
    ic->_masks[1] = 0xc0000000;
    return ic;
  }
  
  // Factory function for instruction "nand".
  Inst_P *OSM_P::get_instr_nand(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<nand_sfx_Src, nand_sfx_Trg> *ic = (Inst_sfx<nand_sfx_Src, nand_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "nand.".
  Inst_P *OSM_P::get_instr_nand_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<nand__sfx_Src, nand__sfx_Trg> *ic = (Inst_sfx<nand__sfx_Src, nand__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "neg".
  Inst_P *OSM_P::get_instr_neg(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<neg_sfx_Src, neg_sfx_Trg> *ic = (Inst_sfx<neg_sfx_Src, neg_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "neg.".
  Inst_P *OSM_P::get_instr_neg_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<neg__sfx_Src, neg__sfx_Trg> *ic = (Inst_sfx<neg__sfx_Src, neg__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "nego".
  Inst_P *OSM_P::get_instr_nego(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<nego_sfx_Src, nego_sfx_Trg> *ic = (Inst_sfx<nego_sfx_Src, nego_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0xc0000000;
    ic->_masks[0] = 0xc0000000;
    return ic;
  }
  
  // Factory function for instruction "nego.".
  Inst_P *OSM_P::get_instr_nego_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<nego__sfx_Src, nego__sfx_Trg> *ic = (Inst_sfx<nego__sfx_Src, nego__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0xc0000000;
    ic->_masks[1] = 0xc0000000;
    return ic;
  }
  
  // Factory function for instruction "nor".
  Inst_P *OSM_P::get_instr_nor(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<nor_sfx_Src, nor_sfx_Trg> *ic = (Inst_sfx<nor_sfx_Src, nor_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "nor.".
  Inst_P *OSM_P::get_instr_nor_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<nor__sfx_Src, nor__sfx_Trg> *ic = (Inst_sfx<nor__sfx_Src, nor__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
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
  
  // Factory function for instruction "or.".
  Inst_P *OSM_P::get_instr_or_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<or__sfx_Src, or__sfx_Trg> *ic = (Inst_sfx<or__sfx_Src, or__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "orc".
  Inst_P *OSM_P::get_instr_orc(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<orc_sfx_Src, orc_sfx_Trg> *ic = (Inst_sfx<orc_sfx_Src, orc_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "orc.".
  Inst_P *OSM_P::get_instr_orc_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<orc__sfx_Src, orc__sfx_Trg> *ic = (Inst_sfx<orc__sfx_Src, orc__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
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
    Inst_sfx<oris_sfx_Src, oris_sfx_Trg> *ic = (Inst_sfx<oris_sfx_Src, oris_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "rfci".
  Inst_P *OSM_P::get_instr_rfci(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_jump<rfci_jump___other__> *ic = (Inst_jump<rfci_jump___other__>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::jumpClass,p);
    return ic;
  }
  
  // Factory function for instruction "rfi".
  Inst_P *OSM_P::get_instr_rfi(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_jump<rfi_jump___other__> *ic = (Inst_jump<rfi_jump___other__>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::jumpClass,p);
    return ic;
  }
  
  // Factory function for instruction "rldcl".
  Inst_P *OSM_P::get_instr_rldcl(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<rldcl_sfx_Src, rldcl_sfx_Trg> *ic = (Inst_sfx<rldcl_sfx_Src, rldcl_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[3] = 0x3f;
    return ic;
  }
  
  // Factory function for instruction "rldcl.".
  Inst_P *OSM_P::get_instr_rldcl_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<rldcl__sfx_Src, rldcl__sfx_Trg> *ic = (Inst_sfx<rldcl__sfx_Src, rldcl__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[9] = 0x3f;
    ic->_masks[10] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "rldcr".
  Inst_P *OSM_P::get_instr_rldcr(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<rldcr_sfx_Src, rldcr_sfx_Trg> *ic = (Inst_sfx<rldcr_sfx_Src, rldcr_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[3] = 0x3f;
    return ic;
  }
  
  // Factory function for instruction "rldcr.".
  Inst_P *OSM_P::get_instr_rldcr_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<rldcr__sfx_Src, rldcr__sfx_Trg> *ic = (Inst_sfx<rldcr__sfx_Src, rldcr__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[9] = 0x3f;
    ic->_masks[10] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "rldic".
  Inst_P *OSM_P::get_instr_rldic(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<rldic_sfx_Src, rldic_sfx_Trg> *ic = (Inst_sfx<rldic_sfx_Src, rldic_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "rldic.".
  Inst_P *OSM_P::get_instr_rldic_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<rldic__sfx_Src, rldic__sfx_Trg> *ic = (Inst_sfx<rldic__sfx_Src, rldic__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "rldicl".
  Inst_P *OSM_P::get_instr_rldicl(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<rldicl_sfx_Src, rldicl_sfx_Trg> *ic = (Inst_sfx<rldicl_sfx_Src, rldicl_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "rldicl.".
  Inst_P *OSM_P::get_instr_rldicl_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<rldicl__sfx_Src, rldicl__sfx_Trg> *ic = (Inst_sfx<rldicl__sfx_Src, rldicl__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "rldicr".
  Inst_P *OSM_P::get_instr_rldicr(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<rldicr_sfx_Src, rldicr_sfx_Trg> *ic = (Inst_sfx<rldicr_sfx_Src, rldicr_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "rldicr.".
  Inst_P *OSM_P::get_instr_rldicr_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<rldicr__sfx_Src, rldicr__sfx_Trg> *ic = (Inst_sfx<rldicr__sfx_Src, rldicr__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "rldimi".
  Inst_P *OSM_P::get_instr_rldimi(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<rldimi_sfx_Src, rldimi_sfx_Trg> *ic = (Inst_sfx<rldimi_sfx_Src, rldimi_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "rldimi.".
  Inst_P *OSM_P::get_instr_rldimi_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<rldimi__sfx_Src, rldimi__sfx_Trg> *ic = (Inst_sfx<rldimi__sfx_Src, rldimi__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "rlwimi".
  Inst_P *OSM_P::get_instr_rlwimi(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<rlwimi_sfx_Src, rlwimi_sfx_Trg> *ic = (Inst_sfx<rlwimi_sfx_Src, rlwimi_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "rlwimi.".
  Inst_P *OSM_P::get_instr_rlwimi_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<rlwimi__sfx_Src, rlwimi__sfx_Trg> *ic = (Inst_sfx<rlwimi__sfx_Src, rlwimi__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "rlwinm".
  Inst_P *OSM_P::get_instr_rlwinm(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<rlwinm_sfx_Src, rlwinm_sfx_Trg> *ic = (Inst_sfx<rlwinm_sfx_Src, rlwinm_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "rlwinm.".
  Inst_P *OSM_P::get_instr_rlwinm_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<rlwinm__sfx_Src, rlwinm__sfx_Trg> *ic = (Inst_sfx<rlwinm__sfx_Src, rlwinm__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "rlwnm".
  Inst_P *OSM_P::get_instr_rlwnm(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<rlwnm_sfx_Src, rlwnm_sfx_Trg> *ic = (Inst_sfx<rlwnm_sfx_Src, rlwnm_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[2] = 0x1f;
    return ic;
  }
  
  // Factory function for instruction "rlwnm.".
  Inst_P *OSM_P::get_instr_rlwnm_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<rlwnm__sfx_Src, rlwnm__sfx_Trg> *ic = (Inst_sfx<rlwnm__sfx_Src, rlwnm__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[7] = 0x1f;
    ic->_masks[8] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "sc".
  Inst_P *OSM_P::get_instr_sc(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<sc_sfx_Src, sc_sfx_Trg> *ic = (Inst_sfx<sc_sfx_Src, sc_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "sld".
  Inst_P *OSM_P::get_instr_sld(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<sld_sfx_Src, sld_sfx_Trg> *ic = (Inst_sfx<sld_sfx_Src, sld_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[5] = 0x7f;
    return ic;
  }
  
  // Factory function for instruction "sld.".
  Inst_P *OSM_P::get_instr_sld_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<sld__sfx_Src, sld__sfx_Trg> *ic = (Inst_sfx<sld__sfx_Src, sld__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[12] = 0x7f;
    ic->_masks[13] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "slw".
  Inst_P *OSM_P::get_instr_slw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<slw_sfx_Src, slw_sfx_Trg> *ic = (Inst_sfx<slw_sfx_Src, slw_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[3] = 0x3f;
    return ic;
  }
  
  // Factory function for instruction "slw.".
  Inst_P *OSM_P::get_instr_slw_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<slw__sfx_Src, slw__sfx_Trg> *ic = (Inst_sfx<slw__sfx_Src, slw__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[9] = 0x3f;
    ic->_masks[10] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "srad".
  Inst_P *OSM_P::get_instr_srad(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<srad_sfx_Src, srad_sfx_Trg> *ic = (Inst_sfx<srad_sfx_Src, srad_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[5] = 0x7f;
    ic->_masks[4] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "srad.".
  Inst_P *OSM_P::get_instr_srad_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<srad__sfx_Src, srad__sfx_Trg> *ic = (Inst_sfx<srad__sfx_Src, srad__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[12] = 0x7f;
    ic->_masks[13] = 0x80000000;
    ic->_masks[11] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "sradi".
  Inst_P *OSM_P::get_instr_sradi(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<sradi_sfx_Src, sradi_sfx_Trg> *ic = (Inst_sfx<sradi_sfx_Src, sradi_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "sradi.".
  Inst_P *OSM_P::get_instr_sradi_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<sradi__sfx_Src, sradi__sfx_Trg> *ic = (Inst_sfx<sradi__sfx_Src, sradi__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0x80000000;
    ic->_masks[3] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "sraw".
  Inst_P *OSM_P::get_instr_sraw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<sraw_sfx_Src, sraw_sfx_Trg> *ic = (Inst_sfx<sraw_sfx_Src, sraw_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[3] = 0x3f;
    ic->_masks[4] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "sraw.".
  Inst_P *OSM_P::get_instr_sraw_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<sraw__sfx_Src, sraw__sfx_Trg> *ic = (Inst_sfx<sraw__sfx_Src, sraw__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[9] = 0x3f;
    ic->_masks[10] = 0x80000000;
    ic->_masks[11] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "srawi".
  Inst_P *OSM_P::get_instr_srawi(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<srawi_sfx_Src, srawi_sfx_Trg> *ic = (Inst_sfx<srawi_sfx_Src, srawi_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "srawi.".
  Inst_P *OSM_P::get_instr_srawi_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<srawi__sfx_Src, srawi__sfx_Trg> *ic = (Inst_sfx<srawi__sfx_Src, srawi__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0x80000000;
    ic->_masks[3] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "srd".
  Inst_P *OSM_P::get_instr_srd(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<srd_sfx_Src, srd_sfx_Trg> *ic = (Inst_sfx<srd_sfx_Src, srd_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[5] = 0x7f;
    return ic;
  }
  
  // Factory function for instruction "srd.".
  Inst_P *OSM_P::get_instr_srd_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<srd__sfx_Src, srd__sfx_Trg> *ic = (Inst_sfx<srd__sfx_Src, srd__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[12] = 0x7f;
    ic->_masks[13] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "srw".
  Inst_P *OSM_P::get_instr_srw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<srw_sfx_Src, srw_sfx_Trg> *ic = (Inst_sfx<srw_sfx_Src, srw_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[3] = 0x3f;
    return ic;
  }
  
  // Factory function for instruction "srw.".
  Inst_P *OSM_P::get_instr_srw_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<srw__sfx_Src, srw__sfx_Trg> *ic = (Inst_sfx<srw__sfx_Src, srw__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[9] = 0x3f;
    ic->_masks[10] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "stb".
  Inst_P *OSM_P::get_instr_stb(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<stb_store_Src, stb_store_Trg, DummyOp> *ic = (Inst_store<stb_store_Src, stb_store_Trg, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::storeClass,p);
    ic->_masks[0] = 0x800000;
    return ic;
  }
  
  // Factory function for instruction "stbu".
  Inst_P *OSM_P::get_instr_stbu(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<stbu_store_Src, stbu_store_Trg, stbu_store_TrgEA> *ic = (Inst_store<stbu_store_Src, stbu_store_Trg, stbu_store_TrgEA>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::storeClass,p);
    ic->_masks[1] = 0x800000;
    return ic;
  }
  
  // Factory function for instruction "stbux".
  Inst_P *OSM_P::get_instr_stbux(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<stbux_store_Src, stbux_store_Trg, stbux_store_TrgEA> *ic = (Inst_store<stbux_store_Src, stbux_store_Trg, stbux_store_TrgEA>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::storeClass,p);
    ic->_masks[2] = 0x800000;
    return ic;
  }
  
  // Factory function for instruction "stbx".
  Inst_P *OSM_P::get_instr_stbx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<stbx_store_Src, stbx_store_Trg, DummyOp> *ic = (Inst_store<stbx_store_Src, stbx_store_Trg, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::storeClass,p);
    ic->_masks[3] = 0x800000;
    return ic;
  }
  
  // Factory function for instruction "std".
  Inst_P *OSM_P::get_instr_std(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<std_sfx_Src, std_sfx_Trg> *ic = (Inst_sfx<std_sfx_Src, std_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x800000;
    return ic;
  }
  
  // Factory function for instruction "stdu".
  Inst_P *OSM_P::get_instr_stdu(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<stdu_sfx_Src, stdu_sfx_Trg> *ic = (Inst_sfx<stdu_sfx_Src, stdu_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0x800000;
    return ic;
  }
  
  // Factory function for instruction "stdux".
  Inst_P *OSM_P::get_instr_stdux(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<stdux_sfx_Src, stdux_sfx_Trg> *ic = (Inst_sfx<stdux_sfx_Src, stdux_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x800000;
    return ic;
  }
  
  // Factory function for instruction "stdx".
  Inst_P *OSM_P::get_instr_stdx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<stdx_sfx_Src, stdx_sfx_Trg> *ic = (Inst_sfx<stdx_sfx_Src, stdx_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0x800000;
    return ic;
  }
  
  // Factory function for instruction "sth".
  Inst_P *OSM_P::get_instr_sth(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<sth_store_Src, sth_store_Trg, DummyOp> *ic = (Inst_store<sth_store_Src, sth_store_Trg, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::storeClass,p);
    ic->_masks[0] = 0x800000;
    return ic;
  }
  
  // Factory function for instruction "sthbrx".
  Inst_P *OSM_P::get_instr_sthbrx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<sthbrx_sfx_Src, sthbrx_sfx_Trg> *ic = (Inst_sfx<sthbrx_sfx_Src, sthbrx_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[2] = 0xffff;
    ic->_masks[1] = 0x800000;
    return ic;
  }
  
  // Factory function for instruction "sthu".
  Inst_P *OSM_P::get_instr_sthu(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<sthu_store_Src, sthu_store_Trg, sthu_store_TrgEA> *ic = (Inst_store<sthu_store_Src, sthu_store_Trg, sthu_store_TrgEA>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::storeClass,p);
    ic->_masks[1] = 0x800000;
    return ic;
  }
  
  // Factory function for instruction "sthux".
  Inst_P *OSM_P::get_instr_sthux(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<sthux_store_Src, sthux_store_Trg, sthux_store_TrgEA> *ic = (Inst_store<sthux_store_Src, sthux_store_Trg, sthux_store_TrgEA>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::storeClass,p);
    ic->_masks[2] = 0x800000;
    return ic;
  }
  
  // Factory function for instruction "sthx".
  Inst_P *OSM_P::get_instr_sthx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<sthx_store_Src, sthx_store_Trg, DummyOp> *ic = (Inst_store<sthx_store_Src, sthx_store_Trg, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::storeClass,p);
    ic->_masks[3] = 0x800000;
    return ic;
  }
  
  // Factory function for instruction "stmw".
  Inst_P *OSM_P::get_instr_stmw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_stmw<stmw_stmw_Src, stmw_stmw_Trg> *ic = (Inst_stmw<stmw_stmw_Src, stmw_stmw_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::stmwClass,p);
    ic->_masks[0] = 0x800000;
    return ic;
  }
  
  // Factory function for instruction "stw".
  Inst_P *OSM_P::get_instr_stw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<stw_store_Src, stw_store_Trg, DummyOp> *ic = (Inst_store<stw_store_Src, stw_store_Trg, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::storeClass,p);
    ic->_masks[0] = 0x800000;
    return ic;
  }
  
  // Factory function for instruction "stwbrx".
  Inst_P *OSM_P::get_instr_stwbrx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<stwbrx_sfx_Src, stwbrx_sfx_Trg> *ic = (Inst_sfx<stwbrx_sfx_Src, stwbrx_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0x800000;
    return ic;
  }
  
  // Factory function for instruction "stwcx.".
  Inst_P *OSM_P::get_instr_stwcx_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<stwcx__sfx_Src, stwcx__sfx_Trg> *ic = (Inst_sfx<stwcx__sfx_Src, stwcx__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    ic->_masks[1] = 0x0000000000000001ull;
    ic->_masks[2] = 0x800000;
    return ic;
  }
  
  // Factory function for instruction "stwu".
  Inst_P *OSM_P::get_instr_stwu(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<stwu_store_Src, stwu_store_Trg, stwu_store_TrgEA> *ic = (Inst_store<stwu_store_Src, stwu_store_Trg, stwu_store_TrgEA>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::storeClass,p);
    ic->_masks[1] = 0x800000;
    return ic;
  }
  
  // Factory function for instruction "stwux".
  Inst_P *OSM_P::get_instr_stwux(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<stwux_store_Src, stwux_store_Trg, stwux_store_TrgEA> *ic = (Inst_store<stwux_store_Src, stwux_store_Trg, stwux_store_TrgEA>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::storeClass,p);
    ic->_masks[2] = 0x800000;
    return ic;
  }
  
  // Factory function for instruction "stwx".
  Inst_P *OSM_P::get_instr_stwx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<stwx_store_Src, stwx_store_Trg, DummyOp> *ic = (Inst_store<stwx_store_Src, stwx_store_Trg, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::storeClass,p);
    ic->_masks[3] = 0x800000;
    return ic;
  }
  
  // Factory function for instruction "subf".
  Inst_P *OSM_P::get_instr_subf(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<subf_sfx_Src, subf_sfx_Trg> *ic = (Inst_sfx<subf_sfx_Src, subf_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "subf.".
  Inst_P *OSM_P::get_instr_subf_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<subf__sfx_Src, subf__sfx_Trg> *ic = (Inst_sfx<subf__sfx_Src, subf__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "subfc".
  Inst_P *OSM_P::get_instr_subfc(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<subfc_sfx_Src, subfc_sfx_Trg> *ic = (Inst_sfx<subfc_sfx_Src, subfc_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "subfc.".
  Inst_P *OSM_P::get_instr_subfc_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<subfc__sfx_Src, subfc__sfx_Trg> *ic = (Inst_sfx<subfc__sfx_Src, subfc__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    ic->_masks[1] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "subfco".
  Inst_P *OSM_P::get_instr_subfco(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<subfco_sfx_Src, subfco_sfx_Trg> *ic = (Inst_sfx<subfco_sfx_Src, subfco_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0xc0000000;
    ic->_masks[0] = 0xe0000000;
    return ic;
  }
  
  // Factory function for instruction "subfco.".
  Inst_P *OSM_P::get_instr_subfco_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<subfco__sfx_Src, subfco__sfx_Trg> *ic = (Inst_sfx<subfco__sfx_Src, subfco__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0xc0000000;
    ic->_masks[1] = 0xe0000000;
    return ic;
  }
  
  // Factory function for instruction "subfe".
  Inst_P *OSM_P::get_instr_subfe(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<subfe_sfx_Src, subfe_sfx_Trg> *ic = (Inst_sfx<subfe_sfx_Src, subfe_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0x20000000;
    ic->_masks[0] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "subfe.".
  Inst_P *OSM_P::get_instr_subfe_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<subfe__sfx_Src, subfe__sfx_Trg> *ic = (Inst_sfx<subfe__sfx_Src, subfe__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0xa0000000;
    ic->_masks[1] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "subfeo".
  Inst_P *OSM_P::get_instr_subfeo(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<subfeo_sfx_Src, subfeo_sfx_Trg> *ic = (Inst_sfx<subfeo_sfx_Src, subfeo_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0xe0000000;
    ic->_masks[0] = 0xe0000000;
    return ic;
  }
  
  // Factory function for instruction "subfeo.".
  Inst_P *OSM_P::get_instr_subfeo_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<subfeo__sfx_Src, subfeo__sfx_Trg> *ic = (Inst_sfx<subfeo__sfx_Src, subfeo__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0xe0000000;
    ic->_masks[1] = 0xe0000000;
    return ic;
  }
  
  // Factory function for instruction "subfic".
  Inst_P *OSM_P::get_instr_subfic(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<subfic_sfx_Src, subfic_sfx_Trg> *ic = (Inst_sfx<subfic_sfx_Src, subfic_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "subfme".
  Inst_P *OSM_P::get_instr_subfme(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<subfme_sfx_Src, subfme_sfx_Trg> *ic = (Inst_sfx<subfme_sfx_Src, subfme_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0x20000000;
    ic->_masks[0] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "subfme.".
  Inst_P *OSM_P::get_instr_subfme_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<subfme__sfx_Src, subfme__sfx_Trg> *ic = (Inst_sfx<subfme__sfx_Src, subfme__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0xa0000000;
    ic->_masks[1] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "subfmeo".
  Inst_P *OSM_P::get_instr_subfmeo(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<subfmeo_sfx_Src, subfmeo_sfx_Trg> *ic = (Inst_sfx<subfmeo_sfx_Src, subfmeo_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0xe0000000;
    ic->_masks[0] = 0xe0000000;
    return ic;
  }
  
  // Factory function for instruction "subfmeo.".
  Inst_P *OSM_P::get_instr_subfmeo_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<subfmeo__sfx_Src, subfmeo__sfx_Trg> *ic = (Inst_sfx<subfmeo__sfx_Src, subfmeo__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0xe0000000;
    ic->_masks[1] = 0xe0000000;
    return ic;
  }
  
  // Factory function for instruction "subfo".
  Inst_P *OSM_P::get_instr_subfo(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<subfo_sfx_Src, subfo_sfx_Trg> *ic = (Inst_sfx<subfo_sfx_Src, subfo_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0xc0000000;
    ic->_masks[0] = 0xc0000000;
    return ic;
  }
  
  // Factory function for instruction "subfo.".
  Inst_P *OSM_P::get_instr_subfo_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<subfo__sfx_Src, subfo__sfx_Trg> *ic = (Inst_sfx<subfo__sfx_Src, subfo__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0xc0000000;
    ic->_masks[1] = 0xc0000000;
    return ic;
  }
  
  // Factory function for instruction "subfze".
  Inst_P *OSM_P::get_instr_subfze(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<subfze_sfx_Src, subfze_sfx_Trg> *ic = (Inst_sfx<subfze_sfx_Src, subfze_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0x20000000;
    ic->_masks[0] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "subfze.".
  Inst_P *OSM_P::get_instr_subfze_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<subfze__sfx_Src, subfze__sfx_Trg> *ic = (Inst_sfx<subfze__sfx_Src, subfze__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0xa0000000;
    ic->_masks[1] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "subfzeo".
  Inst_P *OSM_P::get_instr_subfzeo(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<subfzeo_sfx_Src, subfzeo_sfx_Trg> *ic = (Inst_sfx<subfzeo_sfx_Src, subfzeo_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[1] = 0xe0000000;
    ic->_masks[0] = 0xe0000000;
    return ic;
  }
  
  // Factory function for instruction "subfzeo.".
  Inst_P *OSM_P::get_instr_subfzeo_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<subfzeo__sfx_Src, subfzeo__sfx_Trg> *ic = (Inst_sfx<subfzeo__sfx_Src, subfzeo__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0xe0000000;
    ic->_masks[1] = 0xe0000000;
    return ic;
  }
  
  // Factory function for instruction "tlbivax".
  Inst_P *OSM_P::get_instr_tlbivax(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<DummyOp, DummyOp> *ic = (Inst_sfx<DummyOp, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "tlbre".
  Inst_P *OSM_P::get_instr_tlbre(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<DummyOp, DummyOp> *ic = (Inst_sfx<DummyOp, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "tlbsx".
  Inst_P *OSM_P::get_instr_tlbsx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<DummyOp, DummyOp> *ic = (Inst_sfx<DummyOp, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "tlbsync".
  Inst_P *OSM_P::get_instr_tlbsync(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<DummyOp, DummyOp> *ic = (Inst_sfx<DummyOp, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "tlbwe".
  Inst_P *OSM_P::get_instr_tlbwe(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<DummyOp, DummyOp> *ic = (Inst_sfx<DummyOp, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "tw".
  Inst_P *OSM_P::get_instr_tw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<tw_sfx_Src, DummyOp> *ic = (Inst_sfx<tw_sfx_Src, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x41000000;
    ic->_masks[1] = 0x200;
    return ic;
  }
  
  // Factory function for instruction "twi".
  Inst_P *OSM_P::get_instr_twi(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<twi_sfx_Src, DummyOp> *ic = (Inst_sfx<twi_sfx_Src, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x41000000;
    ic->_masks[1] = 0x200;
    return ic;
  }
  
  // Factory function for instruction "wrtee".
  Inst_P *OSM_P::get_instr_wrtee(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<wrtee_sfx_Src, wrtee_sfx_Trg> *ic = (Inst_sfx<wrtee_sfx_Src, wrtee_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x8000;
    ic->_masks[1] = 0x8000;
    return ic;
  }
  
  // Factory function for instruction "wrteei".
  Inst_P *OSM_P::get_instr_wrteei(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<DummyOp, wrteei_sfx_Trg> *ic = (Inst_sfx<DummyOp, wrteei_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x8000;
    return ic;
  }
  
  // Factory function for instruction "xor".
  Inst_P *OSM_P::get_instr_xor(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<xor_sfx_Src, xor_sfx_Trg> *ic = (Inst_sfx<xor_sfx_Src, xor_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "xor.".
  Inst_P *OSM_P::get_instr_xor_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<xor__sfx_Src, xor__sfx_Trg> *ic = (Inst_sfx<xor__sfx_Src, xor__sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "xori".
  Inst_P *OSM_P::get_instr_xori(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<xori_sfx_Src, xori_sfx_Trg> *ic = (Inst_sfx<xori_sfx_Src, xori_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "xoris".
  Inst_P *OSM_P::get_instr_xoris(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<xoris_sfx_Src, xoris_sfx_Trg> *ic = (Inst_sfx<xoris_sfx_Src, xoris_sfx_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "decode_miss".
  Inst_P *OSM_P::get_instr_decode_miss(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_decode_miss' is not implemented in this model.");
  }
  
  // Factory function for instruction "fetch_exception".
  Inst_P *OSM_P::get_instr_fetch_exception(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_fetch_exception *ic = (Inst_fetch_exception*)_instr_pool.alloc();
    ic->reset(_issueId,_commits,adliss::P::fetch_exceptionClass,p);
    return ic;
  }
  
  void OSM_P::set_branch_target(addr_t addr) {
    _branch_target = addr;
    _cur_instr->set_branch_target(addr);
  }
  
  void OSM_P::signalException(Inst_P *instr) {
    if (Fetcher.logger()) Fetcher.logger()->logMessage("commiting exception");
    if (_speculative || !instr) return;
    instr->set_branch_target(get_pc());
    _speculative = true;
    _spec_tag = instr->iid();
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
    if (_speculative) return;
    if (logger) logger->logMessage("commiting exception");
    if (instr.is_fetch_exception()) {
      commit_fetch_exception(instr);
    } else {
      instr._instr->commit_exceptions(*_iss);
    }
    instr.set_branch_target(get_pc());
  };
  
  void OSM_P::cancel_all_mem(Inst_P &instr)
  {
    UadlMemAccess *ma = instr.mem();
    while (ma) {
      if (ma->valid_request_id()) {
        L1d_obj.cancel(ma->request_id(),true);
      }
      ma = ma->_next;
    }
  }
  
  void OSM_P::commit_fetch_exception(Inst_P&instr)
  {
    _fetchExceptionInProgress = false;
  }
  
  void OSM_P::exec_mEX(Inst_P&instr,Logger *logger,unsigned position)
  {
    exec(instr,logger);
  }
  
  void OSM_P::exec_mem_mEX(Inst_P&instr,Logger *logger,unsigned position)
  {
    exec_mem(instr,logger);
  }
  
  void OSM_P::exec_mID(Inst_P&instr,Logger *logger,unsigned position)
  {
    exec(instr,logger);
  }
  
  void OSM_P::exec_mem_mID(Inst_P&instr,Logger *logger,unsigned position)
  {
    exec_mem(instr,logger);
  }
  
  void OSM_P::exec_mMM(Inst_P&instr,Logger *logger,unsigned position)
  {
    exec(instr,logger);
  }
  
  void OSM_P::exec_mem_mMM(Inst_P&instr,Logger *logger,unsigned position)
  {
    exec_mem(instr,logger);
  }
  
  void OSM_P::exec_mWB(Inst_P&instr,Logger *logger,unsigned position)
  {
    exec(instr,logger);
  }
  
  void OSM_P::exec_mem_mWB(Inst_P&instr,Logger *logger,unsigned position)
  {
    exec_mem(instr,logger);
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
    _speculative = false;
    if (ic.iid() >= _resetFetchId) {
    }
    taken_flush_mWB(ic,logger,position);
  }
  
  void OSM_P::taken_flush_mWB(Inst_P&ic,Logger *logger,unsigned position,bool given_addr,addr_t addr)
  {
    _fetchException = 0;
    if (ic.iid() >= _resetFetchId) {
      if (!given_addr && ic._instr) {
      }
    }
    _speculative = false;
    mWB.update_tail();
    for (unsigned i = 0; i != 1; ++i) {
      if (Inst_P *instr = mMM[i]) {
        (instr->_flush)(instr,*this,logger);
        discard(instr);
      }
    }
    mMM.reset();
    for (unsigned i = 0; i != 1; ++i) {
      if (Inst_P *instr = mEX[i]) {
        (instr->_flush)(instr,*this,logger);
        discard(instr);
      }
    }
    mEX.reset();
    for (unsigned i = 0; i != 1; ++i) {
      if (Inst_P *instr = mID[i]) {
        (instr->_flush)(instr,*this,logger);
        discard(instr);
      }
    }
    mID.reset();
    flush_issue_stage(logger,position,given_addr,addr);
  }
  
  void OSM_P::flush_mMM(Inst_P&ic,Logger *logger,unsigned position)
  {
    _speculative = false;
    if (ic.iid() >= _resetFetchId) {
    }
    taken_flush_mMM(ic,logger,position);
  }
  
  void OSM_P::taken_flush_mMM(Inst_P&ic,Logger *logger,unsigned position,bool given_addr,addr_t addr)
  {
    _fetchException = 0;
    if (ic.iid() >= _resetFetchId) {
      if (!given_addr && ic._instr) {
      }
    }
    _speculative = false;
    mMM.update_tail();
    for (unsigned i = 0; i != 1; ++i) {
      if (Inst_P *instr = mEX[i]) {
        (instr->_flush)(instr,*this,logger);
        discard(instr);
      }
    }
    mEX.reset();
    for (unsigned i = 0; i != 1; ++i) {
      if (Inst_P *instr = mID[i]) {
        (instr->_flush)(instr,*this,logger);
        discard(instr);
      }
    }
    mID.reset();
    flush_issue_stage(logger,position,given_addr,addr);
  }
  
  void OSM_P::flush_mEX(Inst_P&ic,Logger *logger,unsigned position)
  {
    _speculative = false;
    if (ic.iid() >= _resetFetchId) {
    }
    taken_flush_mEX(ic,logger,position);
  }
  
  void OSM_P::taken_flush_mEX(Inst_P&ic,Logger *logger,unsigned position,bool given_addr,addr_t addr)
  {
    _fetchException = 0;
    if (ic.iid() >= _resetFetchId) {
      if (!given_addr && ic._instr) {
      }
    }
    _speculative = false;
    mEX.update_tail();
    for (unsigned i = 0; i != 1; ++i) {
      if (Inst_P *instr = mID[i]) {
        (instr->_flush)(instr,*this,logger);
        discard(instr);
      }
    }
    mID.reset();
    flush_issue_stage(logger,position,given_addr,addr);
  }
  
  void OSM_P::flush_mID(Inst_P&ic,Logger *logger,unsigned position)
  {
    _speculative = false;
    if (ic.iid() >= _resetFetchId) {
    }
    taken_flush_mID(ic,logger,position);
  }
  
  void OSM_P::taken_flush_mID(Inst_P&ic,Logger *logger,unsigned position,bool given_addr,addr_t addr)
  {
    _fetchException = 0;
    if (ic.iid() >= _resetFetchId) {
      if (!given_addr && ic._instr) {
      }
    }
    _speculative = false;
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
      Inst_P *fe = get_instr_fetch_exception(0);
      _fetchLogData = _iss->log_get_pending_queue();
      fe->set_fetch_exception(_fetchException,_fetchExceptionHandle,Fetcher.nextFetchAddr(),_fetchLogData);
      _fetchLogData = 0;
      _fetchException = 0;
      return fe;
    }
    
    return instr;
  }
  
  void OSM_P::issue(InstrPacket *instr)
  {
    Fetcher.removeInstr(instr->size());
  }
  
  void OSM_P::commit(Inst_P*instr)
  {
    _iss->free_packet(instr->_instr);
    free_instr(instr);
  }
  
  void OSM_P::commit_checked(Inst_P*instr)
  {
    if (instr->is_fetch_exception()) {
      _iss->log_commit(instr->_fedata);
      instr->discard_fetch_exception();
    } else {
      _iss->free_packet(instr->_instr);
    }
    free_instr(instr);
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
    catch (adliss::P::Exception e) {
      _fetchException = oldExceptionFlag ^ (unsigned)e;
      _iss->cancelExceptions(_fetchException);
      throw 0;
    }
    
    return true;
  }
  
  
  void OSM_P::updateInstrCacheStatus(CacheStatusHint &hint,addr_t addr)
  {
    _iss->update_instr_cache_status(hint._cacheStatus,CacheIFetch,addr);
  }
  
  bool OSM_P::fetchCacheEnabled(addr_t addr)
  {
    return _iss->L1i_cache_enabled(adl::CacheIFetch,addr);
  }
  
  bool OSM_P::issueInstructions()
  {
    if (currTime() < _startTime) return false;
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
      mMM.createLogger(_logclient,tflags);
      _stage_loggers.set(1,mMM.logger(0));
      mEX.createLogger(_logclient,tflags);
      _stage_loggers.set(2,mEX.logger(0));
      mID.createLogger(_logclient,tflags);
      _stage_loggers.set(3,mID.logger(0));
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
      L2_obj.createLogger(_logclient,tflags);
    } else {
      L2_obj.removeLogger();
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
    if (_speculative) {
      *_out << pfx << "[" << _iss->name() << " speculative on iid=" << dec << _spec_tag << "]\n";
    }
    if (_iss->hasPendingExceptions()) {
      *_out << pfx << "[external exception]\n";
    }
    Fetcher.report_and_clear(o,pfx);
    Issue.report_and_clear(o,pfx);
  }
  
  void OSM_P::report_and_clear_bottom(ostream &o,const string &pfx)
  {
    L1d_obj.report_and_clear(o,pfx);
    *_out << '\n';
    L1i_obj.report_and_clear(o,pfx);
    *_out << '\n';
    L2_obj.report_and_clear(o,pfx);
    *_out << '\n';
    Mem_obj.report_and_clear(o,pfx);
    *_out << '\n';
  }
  
  adl::IssNode& OSM_P::iss() { return *_iss; }
  const adl::IssNode& OSM_P::iss() const { return *_iss; }
  ISS_P& OSM_P::get_iss() { return *_iss; }
  
  void OSM_P::preCycle()
  {
    Mem_obj.preCycle();
    L2_obj.preCycle();
    L1d_obj.preCycle();
    L1i_obj.preCycle();
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
    L1i_obj.postCycle();
    L1d_obj.postCycle();
    L2_obj.postCycle();
    FwdEX.postCycle();
    FwdMM.postCycle();
    if (logMemory()) Mem_obj.logAllRequests();
    if (logMemory()) L1i_obj.logAllRequests();
    if (logMemory()) L1d_obj.logAllRequests();
    if (logMemory()) L2_obj.logAllRequests();
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
        if (!mWB.empty()) {
          if (mWB.head()->action(*this,logger0,0)) {
          } else {
            _stall = true;
          }
        }
      }
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(1+mMM.pos(0)) : 0);
        if (!mMM.empty()) {
          if (mMM.head()->action(*this,logger0,0)) {
          } else {
            _stall = true;
          }
        }
      }
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(2+mEX.pos(0)) : 0);
        if (!mEX.empty()) {
          if (mEX.head()->action(*this,logger0,0)) {
          } else {
            _stall = true;
          }
        }
      }
      {
        Logger *logger0 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(3+mID.pos(0)) : 0);
        if (!mID.empty()) {
          if (mID.head()->action(*this,logger0,0)) {
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
  mEX(getPath(_iss,".") + "mEX"), 
  mID(getPath(_iss,".") + "mID"), 
  mMM(getPath(_iss,".") + "mMM"), 
  mWB(getPath(_iss,".") + "mWB"), 
  Mem_obj(*this, logWarnings()), 
  Mem(Mem_obj), 
  _log_mem(true), 
  L2_obj(*this,Mem_obj), 
  L2(L2_obj), 
  L1d_obj(*this,L2_obj), 
  L1d(L1d_obj), 
  L1i_obj(*this,L2_obj), 
  L1i(L1i_obj), 
  ATBL("ATBL"), 
  ATBU("ATBU"), 
  CIA("CIA"), 
  CR("CR"), 
  CSRR0("CSRR0"), 
  CSRR1("CSRR1"), 
  CTR("CTR"), 
  DAC1("DAC1"), 
  DAC2("DAC2"), 
  DBCR0("DBCR0"), 
  DBCR1("DBCR1"), 
  DBCR2("DBCR2"), 
  DBSR("DBSR"), 
  DEAR("DEAR"), 
  DEC("DEC"), 
  DECAR("DECAR"), 
  DVC1("DVC1"), 
  DVC2("DVC2"), 
  ESR("ESR"), 
  GPR("GPR"), 
  IAC1("IAC1"), 
  IAC2("IAC2"), 
  IAC3("IAC3"), 
  IAC4("IAC4"), 
  IAR("IAR"), 
  IVOR0("IVOR0"), 
  IVOR1("IVOR1"), 
  IVOR10("IVOR10"), 
  IVOR11("IVOR11"), 
  IVOR12("IVOR12"), 
  IVOR13("IVOR13"), 
  IVOR14("IVOR14"), 
  IVOR15("IVOR15"), 
  IVOR2("IVOR2"), 
  IVOR3("IVOR3"), 
  IVOR4("IVOR4"), 
  IVOR5("IVOR5"), 
  IVOR6("IVOR6"), 
  IVOR7("IVOR7"), 
  IVOR8("IVOR8"), 
  IVOR9("IVOR9"), 
  IVPR("IVPR"), 
  LR("LR"), 
  MSR("MSR"), 
  NIA("NIA"), 
  PID("PID"), 
  PIR("PIR"), 
  PVR("PVR"), 
  QEXIT("QEXIT"), 
  RESRV("RESRV"), 
  SPRG0("SPRG0"), 
  SPRG1("SPRG1"), 
  SPRG2("SPRG2"), 
  SPRG3("SPRG3"), 
  SPRG4("SPRG4"), 
  SPRG5("SPRG5"), 
  SPRG6("SPRG6"), 
  SPRG7("SPRG7"), 
  SRR0("SRR0"), 
  SRR1("SRR1"), 
  TBL("TBL"), 
  TBU("TBU"), 
  TCR("TCR"), 
  TSR("TSR"), 
  USPRG0("USPRG0"), 
  XER("XER"), 
  dsESR("dsESR"), 
  lsamd_stall("lsamd_stall",1), 
  msync_stall("msync_stall",1), 
  FwdEX("FwdEX"), 
  FwdMM("FwdMM"), 
  Fetcher(*this,getPath(_iss,".")+"Fetcher",18,L1i_obj,0,_fetchSizes,&Mem_obj), 
  _cur_instr(0), 
  _last_instr(0), 
  _speculative(false), 
  _branch_target(0), 
  _spec_tag(0), 
  Issue("         "), 
  _fetchException(0), 
  _fetchExceptionInProgress(false), 
  _fetchLogData(0), 
  _stall(false), 
  _stage_loggers(4), 
  top(*this)
  {
    alloc_mem_access_data();
    _iss->_uarch = this;
    L1d_obj.set_level(0);
    L1i_obj.set_level(0);
  };
  
  void OSM_P::reset()
  {
    UadlArch::reset();
    _breakpoint = false;
    mEX.reset();
    mID.reset();
    mMM.reset();
    mWB.reset();
    Mem_obj.reset();
    _mem_access_pool.reset();
    _log_mem = true;
    L2_obj.reset();
    L1d_obj.reset();
    L1i_obj.reset();
    ATBL.reset();
    ATBU.reset();
    CIA.reset();
    CR.reset();
    CSRR0.reset();
    CSRR1.reset();
    CTR.reset();
    DAC1.reset();
    DAC2.reset();
    DBCR0.reset();
    DBCR1.reset();
    DBCR2.reset();
    DBSR.reset();
    DEAR.reset();
    DEC.reset();
    DECAR.reset();
    DVC1.reset();
    DVC2.reset();
    ESR.reset();
    GPR.reset();
    IAC1.reset();
    IAC2.reset();
    IAC3.reset();
    IAC4.reset();
    IAR.reset();
    IVOR0.reset();
    IVOR1.reset();
    IVOR10.reset();
    IVOR11.reset();
    IVOR12.reset();
    IVOR13.reset();
    IVOR14.reset();
    IVOR15.reset();
    IVOR2.reset();
    IVOR3.reset();
    IVOR4.reset();
    IVOR5.reset();
    IVOR6.reset();
    IVOR7.reset();
    IVOR8.reset();
    IVOR9.reset();
    IVPR.reset();
    LR.reset();
    MSR.reset();
    NIA.reset();
    PID.reset();
    PIR.reset();
    PVR.reset();
    QEXIT.reset();
    RESRV.reset();
    SPRG0.reset();
    SPRG1.reset();
    SPRG2.reset();
    SPRG3.reset();
    SPRG4.reset();
    SPRG5.reset();
    SPRG6.reset();
    SPRG7.reset();
    SRR0.reset();
    SRR1.reset();
    TBL.reset();
    TBU.reset();
    TCR.reset();
    TSR.reset();
    USPRG0.reset();
    XER.reset();
    dsESR.reset();
    lsamd_stall.reset();
    msync_stall.reset();
    FwdEX.reset();
    FwdMM.reset();
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
    _stall = false;
  }
  
  bool OSM_P::active() const
  {
    if (breakpoint()) return false;
    return (_iss->activeCount() || instr_alloc_count() || Mem_obj.isActive() || L1d_obj.isActive() || L1i_obj.isActive() || L2_obj.isActive());
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
    L2_obj.reset_has_data();
    L1d_obj.reset_has_data();
    L1i_obj.reset_has_data();
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
    struct PostSim {
      PostSim(ISS_P *core) : _core(core) {};
      ~PostSim() { _core->postSim(); }
      ISS_P *_core;
    } __post_sim__(_iss);
    
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
    struct PostSim {
      PostSim(ISS_P *core) : _core(core) {};
      ~PostSim() { _core->postSim(); }
      ISS_P *_core;
    } __post_sim__(_iss);
    
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
    L1i_obj.postSim();
    L1d_obj.postSim();
    L2_obj.postSim();
  }
  
  unsigned OSM_P::fetchStalls() const { return Fetcher.fetchStalls(); }
  
  unsigned OSM_P::dmemStalls() const { return L1d_obj.memStalls(0); }
  
  unsigned OSM_P::imemStalls() const { return L1i_obj.memStalls(0); }
  
  UadlArch *createArch(const string &name, unsigned &id,UadlMemoryIf *memory = NULL,ttime_t startTime = 0)
  {
    if (memory) {
      cerr << "Warning:  An external memory was specified but the model was created to use internal memory.\n";}
    
    unsigned thread_id = 0;
    return new OSM_P(id,thread_id,std::cout,startTime);
  }
  
  
  
} // namespace uadl

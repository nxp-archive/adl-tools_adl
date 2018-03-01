//
// ADL/Uadl version 3.17.2
//
// Cycle-accurate/approximate model generated for P
//

#ifdef UADL_SEPARATE_COMPILE
#  define ADL_HEADER
#endif
#include "dlx2-ese-safe-iss.cc"
#define _OSM_P_IMPLEMENTATION_H
#include "dlx2-ese-safe.h"

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
      if ( uarch.mMM.has_space(uarch,*this,logger, uarch.mEX.capacity() ) ) {
            uarch.exec_branch(*this,logger) ;
            write_ops(uarch,*this,logger) ;
            if ( branch_taken(uarch) ) {
                  uarch.taken_flush_mEX(*this,logger,position  ) ;
              }
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
      if ( can_read_ops(uarch,*this,logger) && can_write_ops(uarch,*this,logger) && uarch.mEX.has_space(uarch,*this,logger) ) {
            read_ops(uarch,*this,logger) ;
            allocate_ops(uarch,*this,logger) ;
            return change_state_S_ID__S_EX(uarch,logger,position) ;
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
  // Operands for instruction class decode_miss
  //
  
  // Operands:
  //   (src: ESR)
  struct decode_miss_decode_miss___other__ {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.ESR.can_alloc(uarch,ic,logger,"read")) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).ESR.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).ESR.name(),-1);
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
  template <class __other___t>
  struct Inst_decode_miss : public Inst_P {
    
    typedef struct Inst_decode_miss<__other___t> ThisClass;
    
    Inst_decode_miss &thisInstr() { return *this; };
    
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
    
    void deallocate_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      __other__.deallocate(uarch,ic,logger);
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
    
    bool change_state_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
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
      if ( can_read_ops(uarch,*this,logger) && can_write_ops(uarch,*this,logger) && uarch.mEX.has_space(uarch,*this,logger) ) {
            {uarch.exec(*this,logger) ; if (has_exceptions()) goto HandleException; } ;
            return change_state_S_ID__S_IDe(uarch,logger,position) ;
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
    }
    static void flush_S_EXe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXe(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
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
    
    // Flush action S_MMe
    void flush_S_MMe(OSM_P &uarch,Logger *logger) {
    }
    static void flush_S_MMe_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_MMe(uarch,logger);
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
    
    bool change_state_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit_checked(this);
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
    
    bool change_state_S_WB__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WB -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
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
      if ( uarch.mEX.empty(uarch,*this,logger) && uarch.mMM.empty(uarch,*this,logger) && uarch.mWB.empty(uarch,*this,logger) && ! uarch.Issue.has_attr(uarch,*this,logger, 0 ) ) {
            uarch.exec(*this,logger) ;
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
  // Operands for instruction class load
  //
  
  // Operands:
  //   (imm: D)
  struct lbz_load_Imm {
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
      return uarch._iss->get_D(ic.opc()[0]).uint32();
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
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).FOO.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsESR.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsFoo.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).FOO.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsESR.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsFoo.name(),-1);
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
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).FOO.name(),-1);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsESR.name(),-1);
        break;
        case 3: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsFoo.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).FOO.name(),-1);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsESR.name(),-1);
        break;
        case 3: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsFoo.name(),-1);
        break;
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
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(4)._index;
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
  
  // Operand Alias:  lbz_load_Imm -> lhz_load_Imm
  //   (imm: D)
  typedef lbz_load_Imm lhz_load_Imm;
  
  // Operand Alias:  lbz_load_Src -> lhz_load_Src
  //   (src: FOO[mask 0x00100000]/p)
  //   (src: GPR(RA))
  //   (src: dsESR?)
  //   (src: dsFoo[mask 0xff000000]/p?)
  typedef lbz_load_Src lhz_load_Src;
  
  // Operand Alias:  lbz_load_Trg -> lhz_load_Trg
  //   (trg: GPR(RT))
  typedef lbz_load_Trg lhz_load_Trg;
  
  // Operand Alias:  lbz_load_Imm -> lwz_load_Imm
  //   (imm: D)
  typedef lbz_load_Imm lwz_load_Imm;
  
  // Operand Alias:  lbz_load_Src -> lwz_load_Src
  //   (src: FOO[mask 0x00100000]/p)
  //   (src: GPR(RA))
  //   (src: dsESR?)
  //   (src: dsFoo[mask 0xff000000]/p?)
  typedef lbz_load_Src lwz_load_Src;
  
  // Operand Alias:  lbz_load_Trg -> lwz_load_Trg
  //   (trg: GPR(RT))
  typedef lbz_load_Trg lwz_load_Trg;
  
  // Operand Alias:  lbz_load_Imm -> lwzu_load_Imm
  //   (imm: D)
  typedef lbz_load_Imm lwzu_load_Imm;
  
  // Operands:
  //   (src: FOO[mask 0x00100000]/p)
  //   (src: GPR(RA . uint32 (  )))
  //   (src: dsESR?)
  //   (src: dsFoo[mask 0xff000000]/p?)
  struct lwzu_load_Src {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.FOO.can_alloc(uarch,ic,logger,"read", ic._masks[2])) return false;
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
      if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read")) return false;
      if (!uarch.dsFoo.can_alloc(uarch,ic,logger,"read", ic._masks[3])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).FOO.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsESR.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsFoo.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).FOO.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsESR.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsFoo.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op,const bits<32> &mask = bits<32>()) {
      switch (sub_op) {
        case 0: if (!uarch.FOO.can_alloc(uarch,ic,logger,"read", (ic._masks[2] & ~mask).uint32())) return false;
          return true;
        case 1: if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
          return true;
        case 2: if (!uarch.dsESR.can_alloc(uarch,ic,logger,"read")) return false;
          return true;
        case 3: if (!uarch.dsFoo.can_alloc(uarch,ic,logger,"read", (ic._masks[3] & ~mask).uint32())) return false;
          return true;
      }
      assert(0);
      return 0;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger,unsigned sub_op) {
      switch (sub_op) {
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).FOO.name(),-1);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsESR.name(),-1);
        break;
        case 3: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsFoo.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).FOO.name(),-1);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsESR.name(),-1);
        break;
        case 3: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsFoo.name(),-1);
        break;
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
        case 0: return ic._masks[2];
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
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(3)._index;
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
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).FOO.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsESR.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsFoo.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).FOO.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsESR.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsFoo.name(),-1);
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
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).FOO.name(),-1);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index);
        break;
        case 3: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsESR.name(),-1);
        break;
        case 4: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsFoo.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).FOO.name(),-1);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index);
        break;
        case 3: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsESR.name(),-1);
        break;
        case 4: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsFoo.name(),-1);
        break;
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
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(5)._index);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(5)._index;
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
  template <class Imm_t, class Src_t, class Trg_t>
  struct Inst_load : public Inst_P {
    
    typedef struct Inst_load<Imm_t, Src_t, Trg_t> ThisClass;
    
    Inst_load &thisInstr() { return *this; };
    
    Imm_t Imm;
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
      Imm.deallocate(uarch,ic,logger);
      Src.deallocate(uarch,ic,logger);
      Trg.deallocate(uarch,ic,logger);
    }
    
    void write_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      Imm.write(uarch,ic,logger);
      Src.write(uarch,ic,logger);
      Trg.write(uarch,ic,logger);
    }
    
    bool exec_done() const { return mem() == 0; }
    
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
    
    bool change_state_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      uarch.commit(this);
      return true;
    }
    
    bool change_state_S_WBp__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBp -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      uarch.commit(this);
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, uarch.mEX.capacity() ) && uarch.Mem.can_request_read(uarch,*this,logger) ) {
            {uarch.exec_mem(*this,logger) ; if (has_exceptions()) goto HandleException; } ;
            uarch.Mem.send_read_request(uarch,*this,logger) ;
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
      if ( Src.can_read(uarch,*this,logger) && Trg.can_write(uarch,*this,logger) && uarch.mEX.has_space(uarch,*this,logger) && uarch.hid_stall.can_allocate(uarch,*this,logger) ) {
            Src.read(uarch,*this,logger) ;
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
      if ( uarch.mID.has_space(uarch,*this,logger) && uarch.mID.instr_class(uarch,*this,logger, 0 ) != adliss::P::loadClass && uarch.mID.instr_class(uarch,*this,logger, 0 ) != adliss::P::storeClass ) {
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
            { uarch.exec_and_read(*this,logger, uarch.Mem )  ; if (has_exceptions()) goto HandleException; } ;
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
    
    // Flush action S_WBp
    void flush_S_WBp(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
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
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(0)._index);
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
    
    bool change_state_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    bool change_state_S_WBp__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBp -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
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
      {
            if ( Src.can_read(uarch,*this,logger) && uarch.all_stall.can_allocate(uarch,*this,logger) && uarch.hid_stall.can_allocate(uarch,*this,logger) && uarch.mEX.empty(uarch,*this,logger) && uarch.mMM.empty(uarch,*this,logger) && uarch.mWB.empty(uarch,*this,logger) ) {
                  Src.read(uarch,*this,logger) ;
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
      uarch.all_stall.deallocate(uarch,*this,logger);
      uarch.hid_stall.deallocate(uarch,*this,logger);
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
      uarch.all_stall.deallocate(uarch,*this,logger);
      uarch.hid_stall.deallocate(uarch,*this,logger);
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
      uarch.all_stall.deallocate(uarch,*this,logger);
      uarch.hid_stall.deallocate(uarch,*this,logger);
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
    
    // Flush action S_WBp
    void flush_S_WBp(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      uarch.all_stall.deallocate(uarch,*this,logger);
      uarch.hid_stall.deallocate(uarch,*this,logger);
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
  struct mtspr_mtspr_hid_Src {
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
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).SPR.name(),ic.op(2)._index);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(2)._index;
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
  
  // Operands:
  //   (src: MSR[mask 0x00400000]/p)
  struct mtspr_mtspr_hid___other__ {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.MSR.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).MSR.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).MSR.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,const bits<32> &mask) {
      if (!uarch.MSR.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
      return true;
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
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return ic._masks[0]; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(1).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(1) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 1; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  //
  // Instruction class mtspr_hid
  //
  template <class Src_t, class Trg_t, class __other___t>
  struct Inst_mtspr_hid : public Inst_P {
    
    typedef struct Inst_mtspr_hid<Src_t, Trg_t, __other___t> ThisClass;
    
    Inst_mtspr_hid &thisInstr() { return *this; };
    
    Src_t Src;
    Trg_t Trg;
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
      return (Src.can_read(uarch,ic,logger) && Trg.can_read(uarch,ic,logger) && __other__.can_read(uarch,ic,logger));
    }
    
    void read_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      Src.read(uarch,ic,logger);
      Trg.read(uarch,ic,logger);
      __other__.read(uarch,ic,logger);
    }
    
    void deallocate_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      Src.deallocate(uarch,ic,logger);
      Trg.deallocate(uarch,ic,logger);
      __other__.deallocate(uarch,ic,logger);
    }
    
    void write_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      Src.write(uarch,ic,logger);
      Trg.write(uarch,ic,logger);
      __other__.write(uarch,ic,logger);
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
    
    bool change_state_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    bool change_state_S_WBp__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBp -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.Mem.can_force_lock(uarch,*this,logger) ) {
            uarch.Mem.allocate_force_lock(uarch,*this,logger) ;
            uarch.Fetcher.set_cancel(uarch,*this,logger, true ) ;
            uarch.Fetcher.cancel(uarch,*this,logger) ;
            {uarch.exec(*this,logger) ; if (has_exceptions()) goto HandleException; } ;
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
      {
            if ( can_read_ops(uarch,*this,logger) && uarch.mEX.has_space(uarch,*this,logger) ) {
                  read_ops(uarch,*this,logger) ;
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
      uarch.Mem.deallocate_force_lock(uarch,*this,logger);
      Trg.deallocate(uarch,*this,logger);
      uarch.hid_stall.deallocate(uarch,*this,logger);
    }
    static void flush_S_EXp_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_EXp(uarch,logger);
    }
    
    // Flush action S_ID
    void flush_S_ID(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      uarch.hid_stall.deallocate(uarch,*this,logger);
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
      uarch.Mem.deallocate_force_lock(uarch,*this,logger);
      Trg.deallocate(uarch,*this,logger);
      uarch.hid_stall.deallocate(uarch,*this,logger);
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
      uarch.Mem.deallocate_force_lock(uarch,*this,logger);
      Trg.deallocate(uarch,*this,logger);
      uarch.hid_stall.deallocate(uarch,*this,logger);
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
    
    // Flush action S_WBp
    void flush_S_WBp(OSM_P &uarch,Logger *logger) {
      uarch.Mem.deallocate_force_lock(uarch,*this,logger);
      Trg.deallocate(uarch,*this,logger);
      uarch.hid_stall.deallocate(uarch,*this,logger);
    }
    static void flush_S_WBp_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBp(uarch,logger);
    }
    
  };
  
  //
  // Operands for instruction class rfi
  //
  
  // Operands:
  //   (trg: MSR)
  //   (trg: NIA)
  //   (src: SRR0)
  //   (src: SRR1)
  struct rfi_rfi___other__ {
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
  
  //
  // Instruction class rfi
  //
  template <class __other___t>
  struct Inst_rfi : public Inst_P {
    
    typedef struct Inst_rfi<__other___t> ThisClass;
    
    Inst_rfi &thisInstr() { return *this; };
    
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
      if ( uarch.mMM.has_space(uarch,*this,logger, uarch.mEX.capacity() ) ) {
            uarch.exec_branch(*this,logger) ;
            write_ops(uarch,*this,logger) ;
            uarch.taken_flush_mEX(*this,logger,position  ) ;
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
      if ( can_read_ops(uarch,*this,logger) && can_write_ops(uarch,*this,logger) && uarch.mEX.has_space(uarch,*this,logger) ) {
            read_ops(uarch,*this,logger) ;
            allocate_ops(uarch,*this,logger) ;
            return change_state_S_ID__S_EX(uarch,logger,position) ;
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
  // Operands for instruction class sfx
  //
  
  // Operands:
  //   (src: GPR(RA))
  struct addic_sfx_Src1 {
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
  //   (trg: GPR(RT))
  struct addic_sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
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
  
  // Operands:
  //   (trg: XER[mask 0x20000000]/p)
  struct addic_sfx_TrgFlags {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,const bits<32> &mask) {
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
      return true;
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.XER.allocate(uarch,ic,logger, ic._masks[0]);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.XER.deallocate(uarch,ic,logger, ic._masks[0]);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).XER.name(),-1);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(2)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).XER.name(static_cast<OSM_P&>(uarch),0); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return ic._masks[0]; }
    
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
  struct addic__sfx_Src1 {
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
  
  // Operands:
  //   (src: XER[mask 0x80000000]/p)
  struct addic__sfx_SrcFlags {
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
      return ic.op(3)._index;
    }
    
    bool is_real(OSM_P &uarch,Inst_P &ic,Logger *logger) const { return true; }
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).XER.name(static_cast<OSM_P&>(uarch),0); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return ic._masks[0]; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(3).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(3) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 3; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operands:
  //   (trg: GPR(RT))
  struct addic__sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(2)._index,"read")) return false;
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
  
  // Operands:
  //   (trg: CR[mask 0xf0000000]/p)
  //   (trg: XER[mask 0x20000000]/p)
  struct addic__sfx_TrgFlags {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read", ic._masks[1])) return false;
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
        case 1: if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[2] & ~mask).uint32())) return false;
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
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CR.allocate(uarch,ic,logger, ic._masks[1]);
      uarch.XER.allocate(uarch,ic,logger, ic._masks[2]);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CR.deallocate(uarch,ic,logger, ic._masks[1]);
      uarch.XER.deallocate(uarch,ic,logger, ic._masks[2]);
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
  
  // Operand Alias:  addic_sfx_Src1 -> addi_sfx_Src1
  //   (src: GPR(RA)?)
  typedef addic_sfx_Src1 addi_sfx_Src1;
  
  // Operand Alias:  addic_sfx_Trg -> addi_sfx_Trg
  //   (trg: GPR(RT))
  typedef addic_sfx_Trg addi_sfx_Trg;
  
  // Operand Alias:  addic_sfx_Src1 -> addis_sfx_Src1
  //   (src: GPR(RA)?)
  typedef addic_sfx_Src1 addis_sfx_Src1;
  
  // Operand Alias:  addic_sfx_Trg -> addis_sfx_Trg
  //   (trg: GPR(RT))
  typedef addic_sfx_Trg addis_sfx_Trg;
  
  // Operands:
  //   (src: GPR(RS))
  struct ori_sfx_Src1 {
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
  //   (src: GPR(RS))
  struct andi__sfx_Src1 {
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
  
  // Operand Alias:  addic__sfx_SrcFlags -> andi__sfx_SrcFlags
  //   (src: XER[mask 0x80000000]/p)
  typedef addic__sfx_SrcFlags andi__sfx_SrcFlags;
  
  // Operands:
  //   (trg: GPR(RA))
  struct andi__sfx_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read")) return false;
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
  
  // Operands:
  //   (trg: CR[mask 0xf0000000]/p)
  struct andi__sfx_TrgFlags {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read", ic._masks[3])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,const bits<32> &mask) {
      if (!uarch.CR.can_alloc(uarch,ic,logger,"read", (ic._masks[3] & ~mask).uint32())) return false;
      return true;
    }
    
    bool can_write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      return true;
    }
    
    void allocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CR.allocate(uarch,ic,logger, ic._masks[3]);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.CR.deallocate(uarch,ic,logger, ic._masks[3]);
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
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return ic._masks[3]; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(0).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(0) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 0; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operand Alias:  addic_sfx_Src1 -> add_sfx_Src1
  //   (src: GPR(RA))
  typedef addic_sfx_Src1 add_sfx_Src1;
  
  // Operands:
  //   (src: GPR(RB))
  struct add_sfx_Src2 {
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
  
  // Operand Alias:  addic__sfx_Trg -> add_sfx_Trg
  //   (trg: GPR(RT))
  typedef addic__sfx_Trg add_sfx_Trg;
  
  // Operand Alias:  andi__sfx_Src1 -> or_sfx_Src1
  //   (src: GPR(RS))
  typedef andi__sfx_Src1 or_sfx_Src1;
  
  // Operand Alias:  add_sfx_Src2 -> or_sfx_Src2
  //   (src: GPR(RB))
  typedef add_sfx_Src2 or_sfx_Src2;
  
  // Operand Alias:  ori_sfx_Trg -> or_sfx_Trg
  //   (trg: GPR(RA))
  typedef ori_sfx_Trg or_sfx_Trg;
  
  // Operand Alias:  ori_sfx_Src1 -> rlwinm_sfx_Src1
  //   (src: GPR(RS))
  typedef ori_sfx_Src1 rlwinm_sfx_Src1;
  
  // Operand Alias:  ori_sfx_Trg -> rlwinm_sfx_Trg
  //   (trg: GPR(RA))
  typedef ori_sfx_Trg rlwinm_sfx_Trg;
  
  // Operand Alias:  addic__sfx_Src1 -> cmpi_sfx_Src1
  //   (src: GPR(RA))
  typedef addic__sfx_Src1 cmpi_sfx_Src1;
  
  // Operands:
  //   (src: XER[mask 0x80000000]/p)
  struct cmpi_sfx_SrcFlags {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", ic._masks[3])) return false;
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
      if (!uarch.XER.can_alloc(uarch,ic,logger,"read", (ic._masks[3] & ~mask).uint32())) return false;
      return true;
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
    
    std::string name(UadlArch &uarch,Inst_P &ic,unsigned sub_op = 0,unsigned op_off = 0) const{ return static_cast<OSM_P&>(uarch).XER.name(static_cast<OSM_P&>(uarch),0); }
    
    RegId id(UadlArch &uarch,const Inst_P &ic,unsigned = 0,unsigned op_off = 0) const { return static_cast<OSM_P&>(uarch).XER.id(static_cast<OSM_P&>(uarch),0); }
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return ic._masks[3]; }
    
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
  
  // Operand Alias:  addic__sfx_Src1 -> cmp_sfx_Src1
  //   (src: GPR(RA))
  typedef addic__sfx_Src1 cmp_sfx_Src1;
  
  // Operands:
  //   (src: GPR(RB))
  struct cmp_sfx_Src2 {
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
  
  // Operand Alias:  addic__sfx_SrcFlags -> cmp_sfx_SrcFlags
  //   (src: XER[mask 0x80000000]/p)
  typedef addic__sfx_SrcFlags cmp_sfx_SrcFlags;
  
  // Operand Alias:  cmpi_sfx_Trg -> cmp_sfx_Trg
  //   (trg: CR/p)
  typedef cmpi_sfx_Trg cmp_sfx_Trg;
  
  // Operands:
  //   (src: SPR(SPRN))
  struct mfspr_sfx_Src1 {
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
  //   (src: GPR(RS))
  struct mtspr_sfx_Src1 {
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
  //   (src: MSR[mask 0x00400000]/p)
  struct mtspr_sfx_SrcFlags {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.MSR.can_alloc(uarch,ic,logger,"read", ic._masks[0])) return false;
      return true;
    }
    
    bool read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).MSR.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).MSR.name(),-1);
    }
    
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger,const bits<32> &mask) {
      if (!uarch.MSR.can_alloc(uarch,ic,logger,"read", (ic._masks[0] & ~mask).uint32())) return false;
      return true;
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
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).SPR.name(),ic.op(2)._index);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(2)._index;
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
  
  // Operand Alias:  andi__sfx_Src1 -> srw_sfx_Src1
  //   (src: GPR(RS))
  typedef andi__sfx_Src1 srw_sfx_Src1;
  
  // Operands:
  //   (src: GPR(RB)[mask 0x0000001f]/p)
  struct srw_sfx_Src2 {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read", ic._masks[4])) return false;
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
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(1)._index,"read", (ic._masks[4] & ~mask).uint32())) return false;
      return true;
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
    
    bits<32> mask(Inst_P&ic,unsigned = 0) const { return ic._masks[4]; }
    
    tag_t get_wm_tag(Inst_P &ic,unsigned = 0) const { return ic._instr->write_mask(1).uint32(); }
    
    void set_wm_tag(tag_t tag,Inst_P &ic,unsigned = 0) const { ic._instr->write_mask(1) = tag; }
    
    unsigned slot(unsigned sub_op = 0,unsigned op_off = 0) const { return 1; }
    
    bool is_op_read(Inst_P&ic,unsigned = 0) const {
      return (!ic._instr || ic._instr->is_op_read(slot()));
    }
    unsigned num_sub_ops() const{ return 1; };
    
  };
  
  // Operand Alias:  ori_sfx_Trg -> srw_sfx_Trg
  //   (trg: GPR(RA))
  typedef ori_sfx_Trg srw_sfx_Trg;
  
  // Operand Alias:  addic_sfx_Src1 -> mullw_sfx_Src1
  //   (src: GPR(RA))
  typedef addic_sfx_Src1 mullw_sfx_Src1;
  
  // Operand Alias:  add_sfx_Src2 -> mullw_sfx_Src2
  //   (src: GPR(RB))
  typedef add_sfx_Src2 mullw_sfx_Src2;
  
  // Operand Alias:  addic__sfx_Trg -> mullw_sfx_Trg
  //   (trg: GPR(RT))
  typedef addic__sfx_Trg mullw_sfx_Trg;
  
  // Operand Alias:  addic_sfx_Src1 -> tw_sfx_Src1
  //   (src: GPR(RA))
  typedef addic_sfx_Src1 tw_sfx_Src1;
  
  // Operand Alias:  add_sfx_Src2 -> tw_sfx_Src2
  //   (src: GPR(RB))
  typedef add_sfx_Src2 tw_sfx_Src2;
  
  //
  // Instruction class sfx
  //
  template <class Src1_t, class Src2_t, class SrcFlags_t, class Trg_t, class TrgFlags_t>
  struct Inst_sfx : public Inst_P {
    
    typedef struct Inst_sfx<Src1_t, Src2_t, SrcFlags_t, Trg_t, TrgFlags_t> ThisClass;
    
    Inst_sfx &thisInstr() { return *this; };
    
    Src1_t Src1;
    Src2_t Src2;
    SrcFlags_t SrcFlags;
    Trg_t Trg;
    TrgFlags_t TrgFlags;
    void reset(unsigned iid,unsigned cid,unsigned ic,InstrPacket *instr) {
      Inst_P::reset(iid,cid,ic);
      _instr = instr;
      _move = 0;
      _action = (StateFunc)action_S_INIT_p;
      _flush = (FlushFunc)flush_S_INIT_p;
    }
    
    void deallocate_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      Src1.deallocate(uarch,ic,logger);
      Src2.deallocate(uarch,ic,logger);
      SrcFlags.deallocate(uarch,ic,logger);
      Trg.deallocate(uarch,ic,logger);
      TrgFlags.deallocate(uarch,ic,logger);
    }
    
    void write_ops(OSM_P &uarch,Inst_P &ic,Logger *logger)
    {
      Src1.write(uarch,ic,logger);
      Src2.write(uarch,ic,logger);
      SrcFlags.write(uarch,ic,logger);
      Trg.write(uarch,ic,logger);
      TrgFlags.write(uarch,ic,logger);
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
    
    bool change_state_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    bool change_state_S_WBp__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBp -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.commit(this);
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, 2 ) ) {
            {uarch.exec_mEX(*this,logger,position) ; if (has_exceptions()) goto HandleException; } ;
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
      if ( Src1.can_read(uarch,*this,logger) && Src2.can_read(uarch,*this,logger) && SrcFlags.can_read(uarch,*this,logger) && Trg.can_write(uarch,*this,logger) && TrgFlags.can_write(uarch,*this,logger) && uarch.mEX.has_space(uarch,*this,logger) && uarch.all_stall.can_allocate(uarch,*this,logger) ) {
            Src1.read(uarch,*this,logger) ;
            Src2.read(uarch,*this,logger) ;
            SrcFlags.read(uarch,*this,logger) ;
            Trg.allocate(uarch,*this,logger) ;
            TrgFlags.allocate(uarch,*this,logger) ;
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
      TrgFlags.deallocate(uarch,*this,logger);
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
      TrgFlags.deallocate(uarch,*this,logger);
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
      TrgFlags.deallocate(uarch,*this,logger);
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
      TrgFlags.deallocate(uarch,*this,logger);
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
    
    // Flush action S_WBp
    void flush_S_WBp(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
      TrgFlags.deallocate(uarch,*this,logger);
    }
    static void flush_S_WBp_p(Inst_P *x,OSM_P &uarch,Logger *logger) {
      return static_cast<ThisClass*>(x)->flush_S_WBp(uarch,logger);
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
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).FOO.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsESR.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsFoo.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).FOO.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsESR.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsFoo.name(),-1);
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
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).FOO.name(),-1);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index);
        break;
        case 3: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsESR.name(),-1);
        break;
        case 4: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsFoo.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).FOO.name(),-1);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index);
        break;
        case 3: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsESR.name(),-1);
        break;
        case 4: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsFoo.name(),-1);
        break;
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
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).FOO.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(5)._index);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsESR.name(),-1);
      if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsFoo.name(),-1);
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg) {
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).FOO.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(5)._index);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsESR.name(),-1);
      logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsFoo.name(),-1);
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
        case 0: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).FOO.name(),-1);
        break;
        case 1: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
        break;
        case 2: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index);
        break;
        case 3: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(5)._index);
        break;
        case 4: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsESR.name(),-1);
        break;
        case 5: if (logger)logger->logOpAction("read",static_cast<OSM_P&>(uarch).dsFoo.name(),-1);
        break;
      }
      return true;
    }
    
    void log_read(UadlArch &uarch,Inst_P &ic,Logger *logger,const string &msg,unsigned sub_op) {
      switch (sub_op) {
        case 0: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).FOO.name(),-1);
        break;
        case 1: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
        break;
        case 2: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(4)._index);
        break;
        case 3: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).GPR.name(),ic.op(5)._index);
        break;
        case 4: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsESR.name(),-1);
        break;
        case 5: logger->logOpAction(msg,static_cast<OSM_P&>(uarch).dsFoo.name(),-1);
        break;
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
  struct stwu_store_Trg {
    bool can_read(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      if (!uarch.GPR.can_alloc(uarch,ic,logger,ic.op(3)._index,"read")) return false;
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
      uarch.GPR.allocate(uarch,ic,logger,ic.op(3)._index);
    }
    
    void deallocate(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      uarch.GPR.deallocate(uarch,ic,logger,ic.op(3)._index);
    }
    
    void write(OSM_P &uarch,Inst_P &ic,Logger *logger) {
      deallocate(uarch,ic,logger);
      if (logger)logger->logOpAction("write",static_cast<OSM_P&>(uarch).GPR.name(),ic.op(3)._index);
    }
    
    unsigned field_value(OSM_P &uarch,Inst_P &ic,Logger *logger) const {
      return ic.op(3)._index;
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
    
    bool change_state_S_WBe__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBe -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      uarch.commit(this);
      return true;
    }
    
    bool change_state_S_WBp__S_INIT(OSM_P &uarch,Logger *logger,unsigned position) {
      if (logger && uarch.logOsm()) { logger->logMessage("S_WBp -> S_INIT"); }
      uarch.mWB.remove_instr();
      uarch.free_all_mem(*this);
      uarch.commit(this);
      return true;
    }
    
    // State action S_EX
    bool action_S_EX(OSM_P &uarch,Logger *logger,unsigned position) {
      if ( uarch.mMM.has_space(uarch,*this,logger, uarch.mEX.capacity() ) && uarch.Mem.can_request_write(uarch,*this,logger) ) {
            {uarch.exec_mem(*this,logger) ; if (has_exceptions()) goto HandleException; } ;
            uarch.Mem.send_write_request(uarch,*this,logger) ;
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
      if ( Src.can_read(uarch,*this,logger) && Trg.can_write(uarch,*this,logger) && uarch.mEX.has_space(uarch,*this,logger) && uarch.hid_stall.can_allocate(uarch,*this,logger) ) {
            Src.read(uarch,*this,logger) ;
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
      if ( uarch.mID.has_space(uarch,*this,logger) && ! uarch.mID.has_attr(uarch,*this,logger, 0 , adliss::P::single_issue ) ) {
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
            { uarch.exec_and_write(*this,logger, uarch.Mem )  ; if (has_exceptions()) goto HandleException; } ;
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
    
    // Flush action S_WBp
    void flush_S_WBp(OSM_P &uarch,Logger *logger) {
      Trg.deallocate(uarch,*this,logger);
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
  
  inline const adl::OpInfo &Inst_P::op(unsigned index) const { return _instr->op(index); };
  
  inline const uint32_t *Inst_P::opc() const { return _instr->opc(); };
  
  unsigned OSM_P::_ic_attrs[] = {
    0, // unknownClass
    0x0, // branchClass
    0x0, // decode_missClass
    0x0, // fetch_exceptionClass
    0x1, // isyncClass
    0x2, // loadClass
    0x0, // mtmsrClass
    0x0, // mtspr_hidClass
    0x0, // rfiClass
    0x0, // sfxClass
    0x2, // storeClass
    0, // maxClass
  };
  
  unsigned OSM_P::getInstrClassAttrByName(const std::string &attr)
  {
    if (attr == "serialized") return 0x1;
    if (attr == "single_issue") return 0x2;
    return 0;
  }
  
  void OSM_P::alloc_mem_access_data()
  {
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
    ma->reset(_memId++,m);
    _cur_instr->add_mem(ma);
  }
  
  static unsigned initFetchSizes[] = { 8,  };
  FetchSizes OSM_P::_fetchSizes(&initFetchSizes[0],&initFetchSizes[1]);
  
  // Factory function for instruction "add".
  Inst_P *OSM_P::get_instr_add(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<add_sfx_Src1, add_sfx_Src2, DummyOp, add_sfx_Trg, DummyOp> *ic = (Inst_sfx<add_sfx_Src1, add_sfx_Src2, DummyOp, add_sfx_Trg, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "addi".
  Inst_P *OSM_P::get_instr_addi(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addi_sfx_Src1, DummyOp, DummyOp, addi_sfx_Trg, DummyOp> *ic = (Inst_sfx<addi_sfx_Src1, DummyOp, DummyOp, addi_sfx_Trg, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "addic".
  Inst_P *OSM_P::get_instr_addic(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addic_sfx_Src1, DummyOp, DummyOp, addic_sfx_Trg, addic_sfx_TrgFlags> *ic = (Inst_sfx<addic_sfx_Src1, DummyOp, DummyOp, addic_sfx_Trg, addic_sfx_TrgFlags>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x20000000;
    return ic;
  }
  
  // Factory function for instruction "addic.".
  Inst_P *OSM_P::get_instr_addic_(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<addic__sfx_Src1, DummyOp, addic__sfx_SrcFlags, addic__sfx_Trg, addic__sfx_TrgFlags> *ic = (Inst_sfx<addic__sfx_Src1, DummyOp, addic__sfx_SrcFlags, addic__sfx_Trg, addic__sfx_TrgFlags>*)_instr_pool.alloc();
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
    Inst_sfx<addis_sfx_Src1, DummyOp, DummyOp, addis_sfx_Trg, DummyOp> *ic = (Inst_sfx<addis_sfx_Src1, DummyOp, DummyOp, addis_sfx_Trg, DummyOp>*)_instr_pool.alloc();
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
    Inst_sfx<andi__sfx_Src1, DummyOp, andi__sfx_SrcFlags, andi__sfx_Trg, andi__sfx_TrgFlags> *ic = (Inst_sfx<andi__sfx_Src1, DummyOp, andi__sfx_SrcFlags, andi__sfx_Trg, andi__sfx_TrgFlags>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    ic->_masks[3] = 0xf0000000;
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
  
  // Factory function for instruction "bc".
  Inst_P *OSM_P::get_instr_bc(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_branch<bc_branch___other__> *ic = (Inst_branch<bc_branch___other__>*)_instr_pool.alloc();
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
    Inst_branch<bclr_branch___other__> *ic = (Inst_branch<bclr_branch___other__>*)_instr_pool.alloc();
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
    Inst_branch<bl_branch___other__> *ic = (Inst_branch<bl_branch___other__>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::branchClass,p);
    return ic;
  }
  
  // Factory function for instruction "cmp".
  Inst_P *OSM_P::get_instr_cmp(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<cmp_sfx_Src1, cmp_sfx_Src2, cmp_sfx_SrcFlags, cmp_sfx_Trg, DummyOp> *ic = (Inst_sfx<cmp_sfx_Src1, cmp_sfx_Src2, cmp_sfx_SrcFlags, cmp_sfx_Trg, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x80000000;
    return ic;
  }
  
  // Factory function for instruction "cmpi".
  Inst_P *OSM_P::get_instr_cmpi(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<cmpi_sfx_Src1, DummyOp, cmpi_sfx_SrcFlags, cmpi_sfx_Trg, DummyOp> *ic = (Inst_sfx<cmpi_sfx_Src1, DummyOp, cmpi_sfx_SrcFlags, cmpi_sfx_Trg, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[3] = 0x80000000;
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
    Inst_sfx<DummyOp, DummyOp, DummyOp, DummyOp, DummyOp> *ic = (Inst_sfx<DummyOp, DummyOp, DummyOp, DummyOp, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "isync".
  Inst_P *OSM_P::get_instr_isync(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_isync *ic = (Inst_isync*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::isyncClass,p);
    return ic;
  }
  
  // Factory function for instruction "lbz".
  Inst_P *OSM_P::get_instr_lbz(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lbz_load_Imm, lbz_load_Src, lbz_load_Trg> *ic = (Inst_load<lbz_load_Imm, lbz_load_Src, lbz_load_Trg>*)_instr_pool.alloc();
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
    Inst_load<lhz_load_Imm, lhz_load_Src, lhz_load_Trg> *ic = (Inst_load<lhz_load_Imm, lhz_load_Src, lhz_load_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::loadClass,p);
    ic->_masks[0] = 0x100000;
    ic->_masks[1] = 0xff000000;
    return ic;
  }
  
  // Factory function for instruction "lmw".
  Inst_P *OSM_P::get_instr_lmw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_lmw' is not implemented in this model.");
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
    Inst_load<lwz_load_Imm, lwz_load_Src, lwz_load_Trg> *ic = (Inst_load<lwz_load_Imm, lwz_load_Src, lwz_load_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::loadClass,p);
    ic->_masks[0] = 0x100000;
    ic->_masks[1] = 0xff000000;
    return ic;
  }
  
  // Factory function for instruction "lwzu".
  Inst_P *OSM_P::get_instr_lwzu(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<lwzu_load_Imm, lwzu_load_Src, lwzu_load_Trg> *ic = (Inst_load<lwzu_load_Imm, lwzu_load_Src, lwzu_load_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::loadClass,p);
    ic->_masks[2] = 0x100000;
    ic->_masks[3] = 0xff000000;
    return ic;
  }
  
  // Factory function for instruction "lwzx".
  Inst_P *OSM_P::get_instr_lwzx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_load<DummyOp, lwzx_load_Src, lwzx_load_Trg> *ic = (Inst_load<DummyOp, lwzx_load_Src, lwzx_load_Trg>*)_instr_pool.alloc();
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
    Inst_sfx<mfspr_sfx_Src1, DummyOp, DummyOp, mfspr_sfx_Trg, DummyOp> *ic = (Inst_sfx<mfspr_sfx_Src1, DummyOp, DummyOp, mfspr_sfx_Trg, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "mtmsr".
  Inst_P *OSM_P::get_instr_mtmsr(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_mtmsr<mtmsr_mtmsr_Src, mtmsr_mtmsr_Trg> *ic = (Inst_mtmsr<mtmsr_mtmsr_Src, mtmsr_mtmsr_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::mtmsrClass,p);
    return ic;
  }
  
  // Factory function for instruction "mtspr".
  Inst_P *OSM_P::get_instr_mtspr(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    if ((_iss->get_SPRN(p->_opc[0]) == 51)) {
      Inst_mtspr_hid<mtspr_mtspr_hid_Src, mtspr_mtspr_hid_Trg, mtspr_mtspr_hid___other__> *ic = (Inst_mtspr_hid<mtspr_mtspr_hid_Src, mtspr_mtspr_hid_Trg, mtspr_mtspr_hid___other__>*)_instr_pool.alloc();
      ic->reset(_issueId++,_commits,adliss::P::mtspr_hidClass,p);
      return ic;
    }
    if ((_iss->get_SPRN(p->_opc[0]) == 50)) {
      Inst_mtspr_hid<mtspr_mtspr_hid_Src, mtspr_mtspr_hid_Trg, mtspr_mtspr_hid___other__> *ic = (Inst_mtspr_hid<mtspr_mtspr_hid_Src, mtspr_mtspr_hid_Trg, mtspr_mtspr_hid___other__>*)_instr_pool.alloc();
      ic->reset(_issueId++,_commits,adliss::P::mtspr_hidClass,p);
      ic->_masks[0] = 0x400000;
      return ic;
    }
    Inst_sfx<mtspr_sfx_Src1, DummyOp, mtspr_sfx_SrcFlags, mtspr_sfx_Trg, DummyOp> *ic = (Inst_sfx<mtspr_sfx_Src1, DummyOp, mtspr_sfx_SrcFlags, mtspr_sfx_Trg, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[0] = 0x400000;
    return ic;
  }
  
  // Factory function for instruction "mulli".
  Inst_P *OSM_P::get_instr_mulli(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_mulli' is not implemented in this model.");
  }
  
  // Factory function for instruction "mullw".
  Inst_P *OSM_P::get_instr_mullw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<mullw_sfx_Src1, mullw_sfx_Src2, DummyOp, mullw_sfx_Trg, DummyOp> *ic = (Inst_sfx<mullw_sfx_Src1, mullw_sfx_Src2, DummyOp, mullw_sfx_Trg, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "or".
  Inst_P *OSM_P::get_instr_or(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<or_sfx_Src1, or_sfx_Src2, DummyOp, or_sfx_Trg, DummyOp> *ic = (Inst_sfx<or_sfx_Src1, or_sfx_Src2, DummyOp, or_sfx_Trg, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    return ic;
  }
  
  // Factory function for instruction "ori".
  Inst_P *OSM_P::get_instr_ori(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<ori_sfx_Src1, DummyOp, DummyOp, ori_sfx_Trg, DummyOp> *ic = (Inst_sfx<ori_sfx_Src1, DummyOp, DummyOp, ori_sfx_Trg, DummyOp>*)_instr_pool.alloc();
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
    Inst_rfi<rfi_rfi___other__> *ic = (Inst_rfi<rfi_rfi___other__>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::rfiClass,p);
    return ic;
  }
  
  // Factory function for instruction "rlwinm".
  Inst_P *OSM_P::get_instr_rlwinm(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_sfx<rlwinm_sfx_Src1, DummyOp, DummyOp, rlwinm_sfx_Trg, DummyOp> *ic = (Inst_sfx<rlwinm_sfx_Src1, DummyOp, DummyOp, rlwinm_sfx_Trg, DummyOp>*)_instr_pool.alloc();
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
    Inst_sfx<srw_sfx_Src1, srw_sfx_Src2, DummyOp, srw_sfx_Trg, DummyOp> *ic = (Inst_sfx<srw_sfx_Src1, srw_sfx_Src2, DummyOp, srw_sfx_Trg, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::sfxClass,p);
    ic->_masks[4] = 0x1f;
    return ic;
  }
  
  // Factory function for instruction "stb".
  Inst_P *OSM_P::get_instr_stb(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<stb_store_Src, DummyOp> *ic = (Inst_store<stb_store_Src, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::storeClass,p);
    ic->_masks[0] = 0x100000;
    ic->_masks[1] = 0xff000000;
    return ic;
  }
  
  // Factory function for instruction "sth".
  Inst_P *OSM_P::get_instr_sth(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<sth_store_Src, DummyOp> *ic = (Inst_store<sth_store_Src, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::storeClass,p);
    ic->_masks[0] = 0x100000;
    ic->_masks[1] = 0xff000000;
    return ic;
  }
  
  // Factory function for instruction "sthx".
  Inst_P *OSM_P::get_instr_sthx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<sthx_store_Src, DummyOp> *ic = (Inst_store<sthx_store_Src, DummyOp>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::storeClass,p);
    ic->_masks[0] = 0x100000;
    ic->_masks[1] = 0xff000000;
    return ic;
  }
  
  // Factory function for instruction "stmw".
  Inst_P *OSM_P::get_instr_stmw(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    throw unimpl_instr_exception("Instruction 'get_instr_stmw' is not implemented in this model.");
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
    Inst_store<stw_store_Src, DummyOp> *ic = (Inst_store<stw_store_Src, DummyOp>*)_instr_pool.alloc();
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
    Inst_store<stwu_store_Src, stwu_store_Trg> *ic = (Inst_store<stwu_store_Src, stwu_store_Trg>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::storeClass,p);
    ic->_masks[0] = 0x100000;
    ic->_masks[1] = 0xff000000;
    return ic;
  }
  
  // Factory function for instruction "stwx".
  Inst_P *OSM_P::get_instr_stwx(void *x)
  {
    InstrPacket *p ATTRIBUTE_UNUSED = (InstrPacket*)(x);
    Inst_store<stwx_store_Src, DummyOp> *ic = (Inst_store<stwx_store_Src, DummyOp>*)_instr_pool.alloc();
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
    Inst_sfx<tw_sfx_Src1, tw_sfx_Src2, DummyOp, DummyOp, DummyOp> *ic = (Inst_sfx<tw_sfx_Src1, tw_sfx_Src2, DummyOp, DummyOp, DummyOp>*)_instr_pool.alloc();
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
    Inst_decode_miss<decode_miss_decode_miss___other__> *ic = (Inst_decode_miss<decode_miss_decode_miss___other__>*)_instr_pool.alloc();
    ic->reset(_issueId++,_commits,adliss::P::decode_missClass,p);
    return ic;
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
        Mem_obj.cancel(ma->request_id(),true);
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
    position = 0;
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
    position = 0;
    for (int i = position+1; i < 2; ++i) {
      if (Inst_P *instr = mID.remove_instr(i)) {
        instr->flush(*this,logger);
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
  
  
  bool OSM_P::fetchCacheEnabled(addr_t addr)
  {
    return false;
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
        Logger *logger1 ATTRIBUTE_UNUSED = ((Tracing && logPipeline()) ? _stage_loggers.get(0+mWB.pos(1)) : 0);
        if (!mWB.empty()) {
          if (mWB.head()->action(*this,logger0,0)) {
            if (!mWB.empty()) {
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
  Fetcher(*this,getPath(_iss,".")+"Fetcher",6,Mem_obj,0,_fetchSizes,&Mem_obj), 
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

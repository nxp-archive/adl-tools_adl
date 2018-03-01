//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// General helper code/data structures for creating the transactional ISS.  This
// mainly does work on creating the instruction packet.
//

#ifndef _GENINSTRPACKET_H_
#define _GENINSTRPACKET_H_

#include "helpers/gc_hash_map.h"

#include "fe/Types.h"

namespace adl {

  class opfxstream;
  class Core;
  class Instr;
  class InstrWalker;
  class ConfigDB;
  class CoreConfig;
  class GenInstrPacketBase;
  class GenInstrPacket;
  class ClassData;

  struct SlotInfo {
    Opencxx::Ptree *_sname;  // Assigned a symbol if we are in a helper and
    // indices are passed in from outside.
    int  _start;             // Fixed slot allocation- starting slot.
    int  _stop;              // Fixed slot allocation- ending slot.
    bool _trg;
    bool _src;

    SlotInfo() : _sname(0), _start(0), _stop(0), _trg(false), _src(false) {};

    bool empty() const { return !_sname && _start == 0 && _stop == 0; };

    void update(const SlotInfo &x) {
      // We purposefully don't copy the symbol name b/c we want to create a
      // unique one.
      if (empty()) {
        // If empty, then copy slot information.
        _start = x._start;
        _stop  = x._stop;
      }
      // Only ever update this information b/c we don't want to lose info, e.g.
      // if this isa target but a prior usage of this register was only as a
      // source.
      if (x._src) {
        _src = true;
      }
      if (x._trg) {
        _trg = true;
      }
    }
  };

  inline bool operator<(const SlotInfo &x,const SlotInfo &y)
  {
    return (x._start < y._start)
      || (!(y._start < x._start) && x._stop < y._stop);
  }

  typedef gc_map<UsedReg,SlotInfo> IssUsedRegs;

  typedef IssUsedRegs::value_type SlotPair;
  typedef std::pair<const SlotInfo *,const UsedReg *> SlotPtrPair;

  std::ostream &operator<<(std::ostream &o,const SlotInfo &si);
  std::ostream &operator<<(std::ostream &o,const IssUsedRegs &sa);

  struct SlotAlloc {

    IssUsedRegs _items;

    bool      _trg_fixed; // True => All trg register accesses are fixed.
    bool      _pw;        // True => A trg register is only partially written.
    bool      _cond;      // True => A trg register is conditionally updated.
    bool      _excepts;   // True => An exception is generated.
    bool      _mem;       // True => Accesses a memory.
    bool      _cache;     // True => Accesses a cache.
    bool      _delayed;   // True => Delayed writes exist for this instruction.
    bool      _c_mem;     // True => Memory accessed during commit (write hooks).
    bool      _c_cache;   // True => Memory accessed during commit (write hooks).
    SlotInfo *_branch;    // If a branch, then ptr to branch slot.
    unsigned  _num_slots;
    unsigned  _slot_width;

    SlotAlloc() : _trg_fixed(false), _pw(false), _cond(false), 
                  _excepts(false), _mem(false), _cache(false), 
                  _delayed(false), _c_mem(false), _c_cache(false), 
                  _branch(0), _num_slots(0), _slot_width(0) {};

    void update(const SlotAlloc &x) {
      _trg_fixed = x._trg_fixed;
      _pw        = x._pw;
      _cond      = x._cond;
      _excepts   = x._excepts;
      _mem       = x._mem;
      _cache     = x._cache;
      _delayed   = x._delayed;
      _c_mem     = x._c_mem;
      _c_cache   = x._c_cache;
      _num_slots = x._num_slots;
      _slot_width= x._slot_width;
    }

    SlotPtrPair getSlot(const gc_string &name,Opencxx::Ptree *index = 0) const;
    SlotPtrPair getSlot(const gc_string &reg,const gc_string &index) const;
    SlotPtrPair getSlot(const UsedReg &) const;
    SlotPtrPair getSlot(const SlotInfo *si) const;

    bool has_delays() const { return _delayed; };
    bool is_branch() const { return _branch; };
    SlotInfo *branch_slot() const { return _branch; };

    bool memory() const { return _mem; };
    bool cache()  const { return _cache; };
  };

  std::ostream &operator<<(std::ostream &o,const SlotAlloc &sa);

  struct InstrSlotAlloc : public SlotAlloc {
    Opencxx::Ptree  *_ro_name;
    Opencxx::Ptree  *_bi_name;
    Opencxx::Ptree  *_di_name;
    Opencxx::Ptree  *_ex_name;
    Opencxx::Ptree  *_co_name;
    Opencxx::Ptree  *_cm_name;
    Opencxx::Ptree  *_qr_name;
    Opencxx::Ptree  *_cc_name;
    Opencxx::Ptree  *_cd_name;
    Opencxx::Ptree  *_at_name;
    Opencxx::Ptree  *_fi_name;

    InstrSlotAlloc();
  };

  typedef gc_hash_map<gc_string,SlotAlloc> SlotAllocs;
  typedef gc_hash_map<gc_string,InstrSlotAlloc> InstrSlotAllocs;

  struct InstrPacketMap : public gc_map<const Core *,GenInstrPacketBase *> {};

  gc_string makeSlotRef(int slot,const UsedReg *ur,bool is_assign,bool is_ref,
                        bool is_partial,unsigned packet_width,
                        const char *index = 0,const char *delay = 0);

  gc_string makeSlotRef(Opencxx::Ptree *slot,const UsedReg *ur,bool is_assign,bool is_ref,
                        bool is_partial,unsigned packet_width,
                        const char *index = 0,const char *delay = 0);

  bool encap_instruction(const Instr &instr);
  bool skip_instruction(const Instr &instr);

  // Returns true if an instruction accesses memory.
  bool is_mem_instr(const Instr &instr);

  gc_string getCoreTypeName(const Core &core);

  struct GenInstrPacketBaseIF {
    GenInstrPacketBaseIF(const Core &core,const ConfigDB &config);
    GenInstrPacketBaseIF(GenInstrPacketBase *impl);

    void doSlotAllocation();

    unsigned max_slots() const;

    const InstrSlotAlloc &decode_miss_slots() const;

    const InstrSlotAllocs *instr_slots() const;

    GenInstrPacketBase *get_impl() { return _impl; };

  protected:
  
    GenInstrPacketBase *_impl;
  };

  struct GenInstrPacketIF {

    GenInstrPacketIF(opfxstream &o,InstrPacketMap &packet_map,const Core &core,
                     InstrWalker &walker,ClassData &cd,const ConfigDB &config,
                     const CoreConfig &coreconfig);

    void generateInstrPacket();

    unsigned instrPacketWidth() const;

    unsigned max_slots() const;

    const InstrSlotAlloc &decode_miss_slots() const;

    const InstrSlotAllocs *instr_slots() const;

    const SlotAlloc &branch_taken_slots() const;

    const SlotAlloc &post_exec_slots() const;

    const SlotAlloc &mem_comm_slots() const;

    const SlotAllocs &helper_slots() const;

  protected:
  
    GenInstrPacket *_impl;
  };

}

#endif


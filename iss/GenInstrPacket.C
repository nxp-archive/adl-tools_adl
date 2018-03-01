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

#include <iostream>
#include <sstream>
#include <stdexcept>

#include "helpers/Macros.h"
#include "helpers/pfxstream.h"
#include "helpers/BasicTypes.h"

#include "fe/Data.h"
#include "fe/Helpers.h"

#include "ConfigDB.h"
#include "MiscTypes.h"
#include "GenInstrPacket.h"
#include "GenExceptions.h"
#include "InstrWalker.h"

namespace adl {

using namespace std;
using namespace Opencxx;
using namespace PtreeUtil;

// Define this to get extra debugging information when generating a transactional ISS. 
//#define TRANS_DEBUG

#ifdef TRANS_DEBUG
#define TDBG(x) { cout << x << endl; }
#else
#  define TDBG(x)
#endif

// Counts the number of slots.  For a register-file without a specified element, we include
// the entire size of the register file.
pair<unsigned,bool> slotCount(const UsedRegs &used_regs)
{
  bool fixed = true;
  int count = 0;
  ForEach(used_regs,i) {
    const UsedReg &ur = *i;

    bool f;
    count += ur.next_slot(f);
    if (!f) {
      fixed = false;
    }
  }
  return make_pair(count,fixed);
}


// Returns the slot for a given register or register-file/index pair.
// Returns (slot-number/UsedReg ptr) if element found.
// Returns (0,0) if element not found.
SlotPtrPair SlotAlloc::getSlot(const gc_string &reg,Ptree *index) const
{
  // For the first pass, look for fixed items.
  ForEach(_items,i) {

    const UsedReg  &ur = i->first;
    const SlotInfo &si = i->second;

    // If we match by register/register-file name...
    if ( reg == ur.regb()->name() ) {
      if ( ur.reg() && !index) {
        // It's a register, so fixed/not-fixed is not relevant.
        return make_pair(&si,&ur);
      } else if ( ur._begin && index_compare(index,ur._begin ) ) {
        // We match an instruction field.
        return make_pair(&si,&ur);
      }

    }
  }

  // If we didn't find it on the first pass, then look for non-fixed items.
  ForEach(_items,i) {
    const UsedReg  &ur = i->first;
    const SlotInfo &si = i->second;

    // If we match by register/register-file name...
    if ( reg == ur.regb()->name() ) {
      if (!ur.is_fixed()) {
        // No match on the field, so we're not fixed.
        return make_pair(&si,&ur);
      }
    }
  }

  //    RError("Could not find data about register-file access:  " << reg << " " << index);
  return make_pair((const SlotInfo*)0,(const UsedReg *)0);
}

SlotPtrPair SlotAlloc::getSlot(const gc_string &reg,const gc_string &index) const
{
  return getSlot(reg,Ptree::Make(index.c_str()));
}

SlotPtrPair SlotAlloc::getSlot(const UsedReg &x) const
{
  IfFind(iter,_items,x) {
    return make_pair(&(iter->second),&(iter->first));
  } else {
    return make_pair((const SlotInfo*)0,(const UsedReg *)0);
  }
}

SlotPtrPair SlotAlloc::getSlot(const SlotInfo *x) const
{
  ForEach(_items,iter) {
    if (&(iter->second) == x) {
      return make_pair(&(iter->second),&(iter->first));
    }
  }
  return make_pair((const SlotInfo*)0,(const UsedReg *)0);
}

ostream &operator<<(ostream &o,const SlotInfo &si)
{
  if (si._sname) {
    o << si._sname;
  } else {
    o << si._start << "-" << si._stop;
  }
  if (si._src) {
    o << ",s";
  }
  if (si._trg) {
    o << ",t";
  }
  return o;
}

ostream &operator<<(ostream &o,const IssUsedRegs &si)
{
  ForEach(si,i) {
    o << i->first << ": " << i->second << ' ';
  }
  return o;
}

ostream &operator<<(ostream &o,const SlotAlloc &sa)
{
  o << "[" << sa._items << "<" << sa._num_slots << " slots";
  if (sa._trg_fixed) {
    o << " (fixed trg)";
  }
  o << ">]";
  return o;
}

InstrSlotAlloc::InstrSlotAlloc() : 
  _ro_name(AdlGenSym()), 
  _bi_name(0),
  _di_name(0),
  _ex_name(AdlGenSym()), 
  _co_name(AdlGenSym()),
  _cm_name(AdlGenSym()),
  _qr_name(AdlGenSym()),
  _cc_name(AdlGenSym()),
  _cd_name(AdlGenSym()),
  _at_name(0),
  _fi_name(0)
{};

gc_string makeSlotRef_internal(int slot,Ptree *slot_name,const UsedReg *ur,bool is_assign,bool is_ref,bool is_partial,unsigned packet_width,
                            const char *index,const char *delay)
{
  gc_ostringstream ss;
  bool sa = false;
  // If we're reading from the slot, then we adjust it to the size of the register so
  // that any slices will work as expected.
  if (ur && ur->regb()->width() != packet_width && is_ref && !is_assign) {
    ss << "bits<" << ur->regb()->width() << ">(";
    sa = true;
  }
  ss << PktName << "->slot";
  if (ur) {
    if (is_assign) {
      if (is_partial) {
        // For a partial-write, we have to return the value/mask pair so that
        // the mask can be updated.
        ss << "_pair";
      } else {
        ss << "_used";
        if (delay) {
          ss << "_delayed";
        }
      }
    } else {
      if (is_partial) {
        ss << "_pair";
      }
    }
    ss << "(";
    if (slot_name) {
      ss << slot_name;
    } else {
      ss << slot;
    }
    if (index && !ur->is_fixed()) {
      ss << "+(" << index << ")";
    }
  } else {
    ss << "(";
    if (slot_name) {
      ss << slot_name;
    } else {
      ss << slot;
    }
  }
  if (ur && is_assign && !is_partial) {      
    // If in an assignment and we want to track usage, then this must be
    // treated as a function call, rather than something which returns a
    // reference which is then assigned.
    ss << ",";
    // If delayed, then add on the delay argument.
    if (delay) {
      ss << delay << ",";
    }
  } else {
    ss << ")";
  }
  if (sa) {
    ss << ")";
  }
  return ss.str();
}

gc_string makeSlotRef(int slot,const UsedReg *ur,bool is_assign,bool is_ref,bool is_partial,unsigned packet_width,
                   const char *index,const char *delay)
{
  return makeSlotRef_internal(slot,0,ur,is_assign,is_ref,is_partial,packet_width,index,delay);
}

gc_string makeSlotRef(Ptree *slot,const UsedReg *ur,bool is_assign,bool is_ref,bool is_partial,unsigned packet_width,
                   const char *index,const char *delay)
{
  return makeSlotRef_internal(-1,slot,ur,is_assign,is_ref,is_partial,packet_width,index,delay);
}


// Code for such instructions is generated but they do not have disassembler
bool encap_instruction(const Instr &instr) 
{
  return (!instr.get_nested().empty() && instr.getOpcs().empty());
}
  
bool skip_instruction(const Instr &instr) 
{
  // Skip instructions which are just assembler shorthands.
  // Instruction with nested instruction even if not used, it is called by its
  // nested instructions.
  if ((!instr.is_used() || instr.shorthand()) &&  !encap_instruction(instr)) {
    return true;
  }
  return false;
}

gc_string getCoreTypeName(const Core &core)
{
  return core.name();
}

bool is_mem_instr(const Instr &instr)
{
  return (!instr.usageinfo()._src_mems.empty() || !instr.usageinfo()._trg_mems.empty());
}


class GenInstrPacketBase : public gc {
public:
  GenInstrPacketBase(const Core &core,const ConfigDB &config) :
    _core(core), 
    _config(config),
    _instr_count(0),
    _instr_slots(new InstrSlotAllocs),
    _cia(0),
    _nia(0),
    _max_slots(0),
    _needs_commit_coro(false),
    _allocated(false)
  {
    _cia = _core.cia();
    _nia = _core.nia();
    assert(_cia && _nia);
  };

protected:
  const Core       &_core;

  const ConfigDB   &_config;

  uint64_t          _instr_count;

  InstrSlotAllocs  *_instr_slots;

  const Reg        *_cia;
  const Reg        *_nia;

  SlotAllocs        _helper_slots;
  SlotAlloc         _post_exec_slots;
  SlotAlloc         _branch_taken_slots;
  InstrSlotAlloc    _decode_miss_slots;

  UsageInfo         _mem_comm_regs;
  SlotAlloc         _mem_comm_slots;

  unsigned          _max_slots;

  bool              _needs_commit_coro;

  bool              _allocated;

public:

  bool trans_rethrow() const
  {
    return _config.trans_rethrow();
  }

  bool transactional_iss() const
  {
    return _config.iss_type() != tNormal;
  }

  bool hybrid_iss() const
  {
    return _config.iss_type() == tHybrid;
  }

  bool cached_packets() const
  {
    return _config.trans_pool_size() == 0;
  }

  bool track_reads() const
  {
    return _config.track_reads();
  }

  bool gen_instr_attrs() const
  {
    return _config.gen_instr_attr();
  }

  unsigned max_slots() const
  {
    return _max_slots;
  }

  const InstrSlotAlloc &decode_miss_slots() const
  {
    return _decode_miss_slots;
  }

  const InstrSlotAllocs *instr_slots() const
  {
    return _instr_slots;
  }

  bool haveExceptions() const
  {
    return !_core.exceptions().empty();
  }

  bool have_instr_attrs() const
  {
    return !_core.instr_attrs().empty();
  }

  const SlotAllocs &helper_slots(const gc_string &name) const
  {
    return _helper_slots;
  }

  SlotInfo &addSlot(SlotAlloc &trg,const SlotPair &src)
  {
    auto ip = trg._items.insert(make_pair(src.first,SlotInfo()));
    ip.first->second.update(src.second);
    ip.first->first.update_mutables(src.first);
    return ip.first->second;
  }

  // This function is used for adding in slots for resources used by the
  // memory hierarchy.  If the instruction makes use of these resources, then
  // we treat them as just targets.  If they're not used, then they're treated
  // as sources, so that the memory hierarchy will see the correct value.
  void addMemCommSlots(SlotAlloc &trg,const UsageInfo &ui,bool is_mem_instr)
  {
    // Add in memory communication slots.
    if (is_mem_instr && !_mem_comm_regs._trg_regs.empty()) {
      ForEach(_mem_comm_slots._items,i) {
        SlotInfo &s = addSlot(trg,*i);
        // If not a target, then just make it a source.
        if (!ui._trg_regs.count(i->first)) {
          s._trg = false;
          s._src = true;
        }
        // If a partial target, then we need to make it a source, in order to
        // get the complete value for the memory hierarchy to use.
        if (i->first.partial()) {
          s._src = true;
        }
      }
    }
  }

  // Add all elements from src to trg using the update method.  This is very
  // important- if an element already exists, for example, that's marked
  // only as a source, and another helper function treats it as a source and
  // target, then we want to make sure that we update it appropriately.
  void addSlots(SlotAlloc &trg,const SlotAlloc &src,bool set_srcs,bool set_trgs)
  {
    ForEach(src._items,i) {
      // If sources or targets are set externally, then do not add any items.
      if (set_srcs && i->second._src) {
        break;
      }
      if (set_trgs && i->second._trg) {
        break;
      }
      // If this is a parm, then don't add it.  We'll process it later as a
      // needed resource, but we don't want to add it to our list, since it
      // doesn't have any relevancy outside of the original function.
      if (i->first.is_parm()) {
        break;
      }
      // Finally, add the item.
      addSlot(trg,*i);
    }
    if (src._excepts) {
      // If any helper can issue exceptions, then we record this fact.
      trg._excepts = true;
    }
    if (src._mem) {
      // Same for memory.
      trg._mem = true;
    }
    if (src._cache) {
      // Same for memory.
      trg._cache = true;
    }
  }

  void checkBranch(SlotAlloc &slots,const UsedReg &ur,SlotInfo &si)
  {
    // If we have the NIA as a target, then mark this as a branch
    // and record the slot index of the NIA.
    if (!slots._branch &&  si._trg && ur.reg() == _nia ) {
      slots._branch = &si;
    }
      
    static int bt_attr = Data::get_attr(gc_string("branch_target"));
    if (ur.regb()->has_attr(bt_attr)) {
      slots._branch = &si;
    }

  }

  // Allocates instruction packet slots, given usage information.
  void allocateSlots(SlotAlloc &slots,const UsageInfo &ui,bool is_func,bool is_mem_instr)
  {
    slots._trg_fixed = true;
    slots._cond      = false;
    slots._pw        = false;
    slots._delayed   = false;
    slots._excepts   = ui._exceptions;
    slots._mem       = (!ui._src_mems.empty() || !ui._trg_mems.empty());
    slots._cache     = (!ui._src_caches.empty() || !ui._trg_caches.empty());

    addMemCommSlots(slots,ui,is_mem_instr);

    if (_core.post_exec_handler()) {
      addSlots(slots,_post_exec_slots,ui._explicit_srcs,ui._explicit_trgs);
    }

    // If this instruction targets the NIA and we have a branch-target handler,
    // then include its slots as well.
    bool has_nia_trg = false;
    ForEach(ui._trg_regs,i) {
      if (i->reg() == _core.nia()) {
        has_nia_trg = true;
        break;
      }
    }
    if (_core.branch_taken_handler() && has_nia_trg) {
      addSlots(slots,_branch_taken_slots,ui._explicit_srcs,ui._explicit_trgs);
    }

    int max_slot = 0;

    ForEach(slots._items,i) {
      max_slot = max(max_slot,i->second._stop);
      if ( !i->first.is_fixed() ) {
        slots._trg_fixed = false;
      }
      if ( i->first.conditional() ) {
        slots._cond = true;
      }
      if ( i->first.delayed() ) {
        slots._delayed = true;
      }
    }

    // Add all of the items for this element.  The fact that the underlying data
    // structure is a set means that duplicates will be removed.  We mark the
    // elements as source and/or target, as appropriate.
    ForEach(ui._src_regs,i) {
      if (i->direct()) {
        break;
      }
      slots._items.insert(make_pair(*i,SlotInfo())).first->second._src = true;
    }
    ForEach(ui._trg_regs,i) {
      if (i->direct()) {
        break;
      }
      auto iter = slots._items.insert(make_pair(*i,SlotInfo())).first;
      iter->second._trg = true;
      // We need to propagate these values in cases where we have both sources
      // and targets (and thus the target value was not inserted.
      iter->first.update_mutables(*i);

      if ( ! i->is_fixed() ) {
        slots._trg_fixed = false;
      }
      if ( i->conditional() ) {
        slots._cond = true;
      }
      if ( i->delayed() ) {
        slots._delayed = true;
      }
      if ( i->partial() ) {
        slots._pw = true;
      }
    }

    TDBG("Resulting slots:  " << slots);

    // Now we go through and allocate slots in a sequential manner.
    int index = 0;
    int count = max_slot;
    ForEach(slots._items,i) {
      const UsedReg &ur = i->first;
      SlotInfo &si = i->second;
      bool allocated = false;

      checkBranch(slots,ur,si);
	
      ++index;

      // Skip over elements we've gotten from helpers, since they've already
      // been allocated.
      if (!si.empty()) {
        allocated = true;
      }

      // Skip this if we've already allocated our register.
      if (allocated) {
        continue;
      }

      if (is_func) {
        // If we're in a function, then slot allocations are passed in as
        // parameters.
        si._sname = AdlGenSym();
      } else {
        // Not in a function- slot allocations are absolute elements within
        // the packet.
        bool f;
        si._start = count;
        count += ur.next_slot(f);
        si._stop = count;
        if (!f) {
          slots._trg_fixed = false;
        }
      }

      // If we have exceptions, then we mark all slots as conditional, since
      // in that case, we'll have to track what's been modified and what
      // hasn't when we do a commit.  If we access memory and the core has
      // exceptions, then we assume that there's a good chance that exceptions
      // might be thrown by the memory access, and thus we must track the
      // dirty state.
      //
      // We can modify this member b/c it's marked as mutable.
      if (slots._excepts || (slots._mem && haveExceptions())) {
        ur._cond = true;
      }

    }

    TDBG("Final slots:  " << slots);

    slots._num_slots = count;
  }

  // Allocates slots for a given function and places the results into the
  // _helper_slots data structure using the specified name.
  void allocateFuncSlots(const UsageInfo &func_ui,const gc_string &name)
  {
    // For each helper, register a slot allocation object.
    auto ip = _helper_slots.insert(make_pair(name,SlotAlloc()));

    if (ip.second) {
      // We put stuff into a temporary object so that if we encounter recursion,
      // the element that's in the map is empty until we're done.
      SlotAlloc tmp_sa;

      TDBG("Helper:  " << name);
        
      // Now do our own slot allocations.
      allocateSlots(tmp_sa,func_ui,true,false);

      ip.first->second = tmp_sa;
      
    } else {
      // It's already been processed, so skip it.
    }
  }

  // Allocates slots for helper functions.
  void allocateHelperSlots(const UsageInfo &ui)
  {
    ForEach(ui._helpers,iter) {
      TDBG("Child helper:  " << *iter);
      allocateFuncSlots(_core.helpers().find(*iter)->second,*iter);
    }
  }

  // In order to figure out if there are registers which communicate between
  // instructions and memory, we first look at all registers which are sourced
  // by the memory hierarchy.  We then look to see if any instructions which
  // reference memory target any of these registers
  void findMemCommRegs()
  {
    _mem_comm_regs.clear();

    UsageInfo mem_regs;

    // First, search through the MMU and all lookups, creating a single data set.
    if (auto mmu = _core.mmu()) {
        
      // MMU functions.
      mem_regs.update(mmu->instr_enable());
      mem_regs.update(mmu->data_enable());
      mem_regs.update(mmu->instr_miss());
      mem_regs.update(mmu->data_miss());
      mem_regs.update(mmu->multi_hit());
      mem_regs.update(mmu->aligned_write());
      mem_regs.update(mmu->misaligned_write());
      mem_regs.update(mmu->misaligned_read());
      mem_regs.update(mmu->multi_hit());

      // Loop over each lookup, adding in their registers.
      ForEach(mmu->all_lookups(),iter) {
        const MmuLookup &lookup = *iter->second;

        mem_regs.update(lookup.exec_perm());
        mem_regs.update(lookup.load_perm());
        mem_regs.update(lookup.store_perm());
        mem_regs.update(lookup.valid_func());
        mem_regs.update(lookup.hit_func());
        mem_regs.update(lookup.miss_func());
        mem_regs.update(lookup.reset());
        mem_regs.update(lookup.multi_hit());
      }
    }

    // If we have source registers, then we want to search the memory
    // instructions for target registers.
    if (!mem_regs._src_regs.empty()) {
      TDBG("Memory usage info:" << mem_regs);

      UsageInfo mem_instrs;

      ForEach(_core.instrs(),iter) {
        const Instr &instr = *(iter->second);
        const UsageInfo &instr_ui = instr.usageinfo();

        if (is_mem_instr(instr)) {
          mem_instrs.update(instr_ui);
        }
      }

      // Now, do we have an intersection between the memory sources and the
      // instruction targets?
      set_intersection(mem_regs._src_regs.begin(),mem_regs._src_regs.end(),
                       mem_instrs._trg_regs.begin(),mem_instrs._trg_regs.end(),
                       inserter(_mem_comm_regs._trg_regs,_mem_comm_regs._trg_regs.end()));

      if (!_mem_comm_regs._trg_regs.empty()) {
        TDBG("Memory communication registers:  " << _mem_comm_regs);
      }
    }
  }

  void checkUiCommitResources(InstrSlotAlloc &isa,const UsageInfo &ui)
  {
    if (!ui._src_mems.empty() || !ui._trg_mems.empty()) {
      isa._c_mem = true;
    }
    if (!ui._src_caches.empty() || !ui._trg_caches.empty()) {
      isa._c_cache = true;      
    }
  }

  void checkRegCommitResources(InstrSlotAlloc &isa,const Reg *reg)
  {
    if (const RegHook * rh = reg->writehook()) {
      const UsageInfo &ui = rh->usageinfo();
      checkUiCommitResources(isa,ui);
    }
  }

  void checkRegfileCommitResources(InstrSlotAlloc &isa,const RegFile *regfile)
  {
    // We only care about sparse register files for right now, b/c we're not
    // doing usage tracking on the other type of register-file.
    if (regfile->sparse()) {
      ForEach((regfile->entries()),e_iter) {
        const RegEntry &re = *e_iter->second;
        if (const Reg *r = re.reg().reg()) {
          checkRegCommitResources(isa,r);
        } else if (const RegFile *rf = re.reg().regfile()) {
          checkRegfileCommitResources(isa,rf);
        }
      }
      // Check the invalid-entry hooks as well.
      if (regfile->invalid_entry_read()) {
        checkUiCommitResources(isa,regfile->invalid_entry_read());
      } else if (_core.invalid_entry_read()) {
        checkUiCommitResources(isa,_core.invalid_entry_read());
      }

      if (regfile->invalid_entry_write()) {
        checkUiCommitResources(isa,regfile->invalid_entry_write());
      } else if (_core.invalid_entry_write()) {
        checkUiCommitResources(isa,_core.invalid_entry_write());
      }
    }
  }

  // This checks to see if any targets are associated with write-hooks which
  // access memory or cache.
  void checkCommitResources(InstrSlotAlloc &isa,const UsageInfo &ui)
  {
    ForEach(ui._trg_regs,i) {
      const UsedReg &ur = *i;

      if (const Reg *r = ur.reg()) {
        checkRegCommitResources(isa,r);
      } else if (const RegFile *rf = ur.regfile()) {
        checkRegfileCommitResources(isa,rf);
      }
    }
  }

  void handleAliasUsedReg(InstrSlotAlloc &isa,const UsedRegs &urs,const InstrSlotAlloc &trg_isa,const PPairList &subst)
  {
    ForEach(urs,iter) {
      UsedReg ur = *iter;
      UsedReg a_ur = ur;
      a_ur._begin = replaceAllEqual(ur._begin,subst);
      a_ur._end = replaceAllEqual(ur._end,subst);

      IfFind(x,trg_isa._items,a_ur) {
        // Found the item, so insert using our translated item.
        isa._items.insert(make_pair(ur,x->second));
      }
    }
  }

  // Allocates slots for an alias.
  void allocateAliasSlots(const Instr &instr,InstrSlotAlloc &isa,const InstrSlotAlloc &trg_isa)
  {
    // For each alias source usageinfo item, if it's directly in the target,
    // then use it.
    const UsageInfo &instr_ui = instr.usageinfo();

    isa.update(trg_isa);

    PPairList subst;
    IntMap parms;
    assert(!instr.multitarget_shorthand());
    getRevAliasSubstitutions(subst,parms,instr.get_alias());    

    handleAliasUsedReg(isa,instr_ui._src_regs,trg_isa,subst);
    handleAliasUsedReg(isa,instr_ui._trg_regs,trg_isa,subst);

    ForEach(isa._items,i) {
      const UsedReg &ur = i->first;
      SlotInfo &si = i->second;

      checkBranch(isa,ur,si);
    }
  }

  // Allocates slots for a single instruction.
  void allocateInstrSlots(const Instr &instr,size_t &max_srcs,size_t &max_trgs)
  {
    if (skip_instruction(instr)) {
      return;
    }

    ++_instr_count;

    const UsageInfo &instr_ui = instr.usageinfo();

    // These will store all source and target info from the instruction and
    // all helpers.
    InstrSlotAlloc &isa = _instr_slots->insert(make_pair(instr.name(),InstrSlotAlloc())).first->second;
    
    TDBG("Instr " << instr.name());

    // If this is an alias, then we must use the parent's slot allocations.
    // It's just a matter of mapping them to what we have an ignoring extras
    // that the alias doesn't use.
    if (instr.is_alias()) {
      auto trg = instr.get_alias()->_target;
      
      // If the target hasn't been evaluated yet, then evaluate it now.
      IfFind(t_iter,(*_instr_slots),trg->name()) {
        allocateAliasSlots(instr,isa,t_iter->second);
      } else {
        allocateInstrSlots(*trg,max_srcs,max_trgs);
        IfFind(t_iter,(*_instr_slots),trg->name()) {
          allocateAliasSlots(instr,isa,t_iter->second);
        } else {
          RError("Target of alias slot allocations not found.");
        }
      }
    } else {
      allocateSlots(isa,instr_ui,false,is_mem_instr(instr));
    }

    // If this is a branch, then we will generate a branch-info function
    // for it.
    if (isa.is_branch()) {
      isa._bi_name = AdlGenSym();
    }

    // If this instruction has delayed-writes, then we need an info function
    // for it.
    if (isa.has_delays()) {
      isa._di_name = AdlGenSym();
    }

    // If attributes are exported, we need a name for the accessor.
    if (gen_instr_attrs()) {
      isa._at_name = AdlGenSym();
      isa._fi_name = AdlGenSym();
    }
	
    _max_slots = max(_max_slots,isa._num_slots);
    max_srcs = max(max_srcs,instr_ui._src_regs.size());
    max_trgs = max(max_trgs,instr_ui._trg_regs.size());

    checkCommitResources(isa,instr_ui);

    if (isa._c_mem || isa._c_cache) {
      _needs_commit_coro = true;
    }
  }


  // Allocates slots for all instructions.
  void doSlotAllocation()
  {
    _max_slots = 0;

    size_t max_srcs = 0;
    size_t max_trgs = 0;

    _instr_count = 0;

    if (transactional_iss()) {
      // We need to figure out if we have any registers which are used to
      // communicate between instructions and the memory hiearchy.
      findMemCommRegs();
        
      // We work bottom-up with the helper functions, allocating slots for them so that
      // slots will be allocated for the same registers in a consistent manner.
      // We iterate over the instruction list so that we only work with helpers that are used by
      // instructions.
      ForEach(_core.instrs(),iter) {
        const Instr &instr = *(iter->second);
        const UsageInfo &instr_ui = instr.usageinfo();
        allocateHelperSlots(instr_ui);
      }
        
      allocateHelperSlots(_core.post_exec_handler());

      // Handle the odd-ball extras, such as the decode_miss, post_exec and post_packet
      // function.  We don't mess with the pre/post-fetch functions b/c they
      // operate immediately on the architected state.
      {
        SlotAlloc tmp_slots;
        allocateSlots(tmp_slots,_core.post_exec_handler(),false,false);
        _post_exec_slots = tmp_slots;
      }
      {
        SlotAlloc tmp_slots;
        allocateSlots(tmp_slots,_core.branch_taken_handler(),false,false);
        _branch_taken_slots = tmp_slots;
      }
      {
        InstrSlotAlloc tmp_slots;
        allocateSlots(tmp_slots,_core.decode_miss_handler(),false,false);
        _decode_miss_slots = tmp_slots;
      }
      {
        InstrSlotAlloc tmp_slots;
        allocateSlots(tmp_slots,_mem_comm_regs,false,false);
        _mem_comm_slots = tmp_slots;
      }
    }

    // Now allocate slots for the instructions themselves.
    ForEach(_core.instrs(),iter) {
      allocateInstrSlots( *(iter->second),max_srcs,max_trgs );
    }

    if ((_config.verbose_mode() > 1) && transactional_iss()) {
      cout << "Slot Allocations:\n"
           << "-----------------\n\n";
        
      cout << "Instructions:\n";
      ForEach ((*_instr_slots),i) {
        cout << i->first << ":  " << i->second << "\n";
      }
        
      cout << "Helpers:\n";
      ForEach (_helper_slots,i) {
        cout << i->first << ":  " << i->second << "\n";
      }

      cout << "\nMemory communication registers:\n";
      printUsageInfo(cout,_mem_comm_regs,"  ");
      cout << "\n"
           << "Max number of slots      :  " << _max_slots << "\n"
           << "Max number of src entries:  " << max_srcs << "\n"
           << "Max number of trg entries:  " << max_trgs << "\n"
           << "Max number of packets    :  " << _config.trans_pool_size() << "\n\n";
    }

    _allocated = true;
  }

};

class GenInstrPacket : public GenInstrPacketBase {
public:
  GenInstrPacket(opfxstream &o,
                 const Core &core,
                 InstrWalker &walker,
                 ClassData &cd,
                 const ConfigDB &config,
                 const CoreConfig &coreconfig) :
    GenInstrPacketBase(core,config),
    _o(o), 
    _cd(cd),
    _walker(walker),
    _coreconfig(coreconfig),
    _packet_width(0)
  {
  };

  GenInstrPacket(opfxstream &o,
                 GenInstrPacketBase &pbase,
                 InstrWalker &walker,
                 ClassData &cd,
                 const CoreConfig &coreconfig) :
    GenInstrPacketBase(pbase),
    _o(o), 
    _cd(cd),
    _walker(walker),
    _coreconfig(coreconfig),
    _packet_width(0)
  {
  };

private:

  opfxstream     &_o;

  ClassData      &_cd;

  InstrWalker      &_walker;
  const CoreConfig &_coreconfig;

  unsigned          _packet_width;

public:

  unsigned instrPacketWidth() const
  {
    return _packet_width;
  }

  bool need_values() const
  {
    return !hybrid_iss() || (_coreconfig.need_packet_values());
  }

  const SlotAlloc &post_exec_slots() const
  {
    return _post_exec_slots;
  }

  const SlotAlloc &branch_taken_slots() const
  {
    return _branch_taken_slots;
  }

  const SlotAlloc &mem_comm_slots() const
  {
    return _mem_comm_slots;
  }

  const SlotAllocs &helper_slots() const
  {
    return _helper_slots;
  }

  // If we have a transactional ISS, then here's where we do the allocation of
  // slots for the instruction packet.
  void generateInstrPacket()
  {
    if (!_allocated) {
      doSlotAllocation();
    }

    // The width of an element of the packet is going to be the width of the widest register.
    _packet_width = 0;
    ForEach(_core.regs(),i) {
      _packet_width = max(_packet_width,i->second->width());
    }
    ForEach(_core.regfiles(),i) {
      _packet_width = max(_packet_width,i->second->width());
    }
      
    _walker.set_packet_width(_packet_width);

    if (transactional_iss()) {
      writeInstrPacket();

      // Finally, set up the walker so that any helper calls are properly modified.
      _walker.add_slots_func("setException",new SlotAlloc);
      // If we're a transactional ISS then we populate the instruction
      // walker's function map to that it knows which functions should have an
      // instruction packet added to any call.
      ForEach(_helper_slots,i) {
        _walker.add_slots_func(i->first,&(i->second));
      }
    }
  }

  // Writes the actual instruction packet data structure.
  void writeInstrPacket()
  {
    gc_string coretype = getCoreTypeName(_core);

    // If we have any delayed writes, then we add on a delay field into each slot.
    bool have_delays = _core.has_delayed_writes();

    // This will store the instruction packet when executing an instruction's
    // action code.
    _cd._member_vars.push_back (gc_string(PktType) + " *" + PktName);
    _cd._constr_inits.push_back(gc_string(PktName) + "(0)");  
    _cd._reset_stmts.push_back (gc_string(PktName) + " = 0");        

    unsigned max_width = 0;
    ForEach(_core.regs(),i) {
      max_width = max(max_width,i->second->width());
    }
    ForEach(_core.regfiles(),i) {
      max_width = max(max_width,i->second->width());
    }

    _o << "enum { MaxRegisterWidth = " << dec << max_width << "};\n"
       << "typedef OpInfoM<MaxRegisterWidth>  CoreOpInfoM;\n\n"
       << "typedef OpInfoMs<MaxRegisterWidth> CoreOpInfoMs;\n\n"


    // Now generate the code for the packet and pool of packets.  For dirty
    // bits, we use a bits object, since it's equivalent in functionality to a
    // bitset, for this purpose.
       << "enum { StackSize = 8192 };\n\n"
       << "struct StackObj { StackObj *_next; char _data[StackSize]; char *data() { return &_data[0]; } };\n\n"
       << "struct " << PktType << ";\n";
    if (_config.mem_if() == mNonBlocking) {
      _o << "typedef Coroutine<" << coretype << "," << PktType << " *> Coro;\n\n";
      // We need a pointer to store the current coroutine in order to be able to
      // resume w/o having to pass it as an argument everywhere.
      _cd._member_vars.push_back("Coro *_co");
      _cd._constr_inits.push_back("_co(0)");
      _cd._reset_stmts.push_back("_co = 0");
      // This is used for storing the yield context (context of the main driver
      // context).
      _cd._member_vars.push_back("Coro::context_t _main_context");
    }
    if (!hybrid_iss()) {
      _o << "typedef void (" << coretype << "::*ReadOperand)(" << PktType << "*,uint32_t *,unsigned);\n"
         << "typedef MemAccess::OpType (" << coretype << "::*Commit)(" << PktType << "*,uint32_t *,unsigned);\n"
         << "typedef MemAccess::OpType (" << coretype << "::*Exec)(uint32_t *);\n";
    }
    if (have_delays) {
      _o << "typedef int (" << coretype << "::*DelayInfo)(" << PktType << "*,unsigned);\n";
    }
    if (gen_instr_attrs()) {
      _o << "typedef const Attrs::AttrList& (*AttrInfo)();\n";
      _o << "typedef int (*FieldInfo)(unsigned);\n";
    }
    _o << "typedef BranchInfoType (" << coretype << "::*BranchInfo)(" << PktType << "*);\n"
       << "typedef void (" << coretype << "::*QuickReset)(" << PktType << "*);\n"
       << "\n"
       << "template <size_t _Nb>\n"
       << "struct bits_pair {\n";
    if (need_values()) {
      _o << "bits<_Nb> _v;\n";
    }
    if (have_delays) {
      _o << "unsigned _d;\n";
    }
    _o << "bits<_Nb> _m;\n";
    if (track_reads()) {
      _o << "bits<_Nb> _r;\n";
    }
    _o << "void mask_reset() {\n"
       << "_m.reset();\n";
    if (track_reads()) {
      _o << "_r.reset();\n";
    }
    _o << "}\n"
       << "void reset() {\n";
    if (need_values()) {
      _o << "_v.reset();\n";
    }
    _o << "mask_reset();\n"
       << "}\n"
       << "};\n"
       << "typedef bits_pair<" << _packet_width <<"> InstrSlot;\n"
       << "\n"
       << "struct " << PktType << " : InstrPacketOpBase<" << _max_slots << "> {\n"
       << "InstrSlot _slots[" << _max_slots << "];\n";
    if (!track_reads()) {
      // If we're tracking reads, thene this data will be recorded in the _r
      // element of each slot.
      _o << "bits<" << _max_slots << "> _op_read;\n";
    }
    if (!hybrid_iss()) {
      _o << "ReadOperand  _read_operand;\n"
         << "Commit       _commit;\n"
         << "Exec         _exec;\n";
    }
    if (have_delays) {
      _o << "DelayInfo    _delay_info;\n";
    }
    if (gen_instr_attrs()) {
      _o << "AttrInfo     _attr_info;\n"
         << "FieldInfo    _field_info;\n";
    }
    _o << "BranchInfo   _branch_info;\n"
       << "int          _branch_target;\n";

    if (cached_packets()) {
      _o << "QuickReset  _quick_reset;\n";
    }

    if (have_instr_attrs()) {
      _o << getInstrAttrType(_core) << ' ' << getInstrAttrVar() << ";\n";
    }

    if (_config.mem_if() == mNonBlocking) {
      _o << "Coro _co;\n";

      if (_needs_commit_coro) {
        _o << "Coro                _cco;\n"
           << "unsigned            _commit_index;\n"
           << "StackObj           *_cstack;\n";
      }
    }
    _o << "union {\n"
       << "InstrPacket *_next;\n"
       << "StackObj    *_stack;\n\n"
       << "};\n\n"
       << "void reset(int id,int size,addr_t addr,uint32_t *opc,unsigned numops,Disassembler disasm) {\n";
    if (!track_reads()) {
      _o << "_op_read.reset();\n";
    }
    if (have_instr_attrs()) {
      _o << getInstrAttrVar() << " = 0;\n";
    }
    if (_config.mem_if() == mNonBlocking) {
      if (_needs_commit_coro) {
        _o << "_cstack = 0;\n";
      }
    }
    _o << "_stack = 0; InstrPacketOpBase<" << _max_slots << ">::reset(id,size,addr,opc,numops,disasm);\n"
       << "};\n";

    if (cached_packets()) {
      _o << "void quick_reset(" << coretype << " &core) {\n"
         << "_op_read.reset();\n"
         << "_stack = 0; InstrPacketOpBase<" << _max_slots << ">::quick_reset();\n"
         << "(core.*_quick_reset)(this);\n"        
         << "}\n";
    }

    _o << "void clear_mask(unsigned i) { _slots[i].mask_reset(); };\n"
       << "void clear(unsigned i) { _slots[i].reset(); };\n";
    if (have_delays) {
      _o << "void clear_delay(unsigned i) { _slots[i]._d = 0; };\n"
         << "unsigned get_delay(unsigned index) const { return _slots[index]._d; };\n";
    }
    _o << "const bits<" << _packet_width << "> &write_mask(unsigned i) const { return _slots[i]._m; }\n"
       << "bits<" << _packet_width << "> &write_mask(unsigned i) { return _slots[i]._m; }\n";
    if (track_reads()) {
      _o << "const bits<" << _packet_width << "> &read_mask(unsigned i) const { return _slots[i]._r; }\n";
    } else if (hybrid_iss()) {
      // Hybrid, but we're not tracking reads, so we don't have this information.  Just return a 0.
      _o << "const bits<" << _packet_width << "> read_mask(unsigned i) const { return 0; }\n";
    } else {
      // Not tracking reads, then just always return a full mask if the operand is read.
      _o << "const bits<" << _packet_width << "> read_mask(unsigned i) const {\n"
         << "if (is_op_read(i)) {\n"
         << "bits<" << _packet_width << "> x; x.set(); return x;\n"
         << "} else {"
         << "return 0;\n"
         << "}\n"
         << "}\n";
    }
    if (hybrid_iss()) {
      _o << "void slot_used(unsigned i) { _slots[i]._m.set(); }\n"
         << "template <size_t Nb>\n"
         << "void slot_used(const bits<Nb> &i) { _slots[i.uint32()]._m.set(); }\n";
      if (need_values()) {
        _o << "void set_slot(unsigned i,const bits<" << _packet_width << "> &x) { _op_read.set(i,true); _slots[i]._v = x;}\n"
           << "bits<" << _packet_width << "> &slot(unsigned i) { return _slots[i]._v; }\n"
           << "const bits<" << _packet_width << "> &slot(unsigned i) const { return _slots[i]._v; }\n";
      }
      if (have_delays) {
        _o << "void slot_used(unsigned i, unsigned d) { _slots[i]._m.set(); _slots[i]._d = d; }\n";
      }
      if (track_reads()) {
        _o << "void slot_read(unsigned i) { _slots[i]._r.set(); }\n"
           << "template <size_t Nb>\n"
           << "void slot_read(const bits<Nb> &i) { _slots[i.uint32()]._r.set();}\n"
           << "void slot_read(unsigned i,unsigned l) { _slots[i]._r.set(l,1); }\n"
           << "void slot_read(unsigned i,unsigned l,unsigned r) { _slots[i]._r.mask(l,r); }\n"
           << "bool is_op_read(unsigned i) const { return _slots[i]._r.any(); }\n\n";
      } else {
        _o << "bool is_op_read(unsigned i) const { return true; }\n\n";
      }
    } else {
      _o << "void set_slot(unsigned i,const bits<" << _packet_width << "> &x) { _op_read.set(i,true); _slots[i]._v = x;}\n"
         << "bits<" << _packet_width << "> &slot(unsigned i) { return _slots[i]._v; }\n"
         << "const bits<" << _packet_width << "> &slot(unsigned i) const { return _slots[i]._v; }\n"
         << "template <size_t Nb>\n"
         << "bits<" << _packet_width << "> &slot(const bits<Nb> &i) { return _slots[i.uint32()]._v; }\n"
         << "void slot_used(unsigned i, const bits<" << _packet_width << "> &v) { _slots[i]._m.set(); _slots[i]._v = v; }\n"
         << "template <size_t Nb>\n"
         << "void slot_used(const bits<Nb> &i,const bits<" << _packet_width << "> &v) { _slots[i.uint32()]._m.set(); _slots[i.uint32()]._v = v; }\n"
         << "bool is_op_read(unsigned i) const { return _op_read.get(i); }\n\n";
      if (have_delays) {
        _o << "void slot_used_delayed(unsigned i, unsigned d,const bits<" << _packet_width << "> &v) { _slots[i]._m.set(); _slots[i]._d = d; _slots[i]._v = v;  }\n"
           << "template <size_t Nb>\n"
           << "void slot_used_delayed(const bits<Nb> &i,unsigned d,const bits<" << _packet_width << "> &v) { _slots[i.uint32()]._m.set(); _slots[i.uint32()]._d = d; _slots[i.uint32()]._v = v; }\n";
      }
    }
    _o << "bool is_dirty(unsigned i) const { return _slots[i]._m.any(); }\n\n"
       << "bits_pair<" << _packet_width << "> &slot_pair(unsigned i) { return _slots[i]; }\n"
       << "template <size_t Nb>\n"
       << "bits_pair<" << _packet_width << "> &slot_pair(const bits<Nb> &i) { return _slots[i.uint32()]; }\n"

    // The resolved-op function exists so that you can resolve aliases and
    // indirection through sparse register-files.
       << "void getResolvedOp(" << coretype << "::CoreOpInfoMs &ops,unsigned index) const {\n"
       << "const OpInfo &x = getOp(index);\n"
       << "if (x._type & OpInfo::opSrc) {\n"
       << coretype << "::reg_read_alias_resolver(ops,x);\n"
       << "} else {\n"
       << coretype << "::reg_write_alias_resolver(ops,x);\n"
       << "}\n"
       << "};\n";
    
    // The read-operand and commit functions are only needed for true transactional ISSs.
    if (!hybrid_iss()) {
      _o << "void read_operand(" << coretype << "& core,unsigned index) {\n";
      if (haveExceptions()) {
        _o << "try {\n";
      }
      _o << "if (Tracing) core.logger().log_current_instr(this);\n"
         << "(core.*_read_operand)(this,_opc,index);\n";
      if (haveExceptions()) {
        _o << "}\n"
           << "catch (" << coretype << "::Exception exdata) {\n"
           << "_exdata |= exdata;\n"
           << "_exitems = core." << getExceptionSetupFunc() << "(exdata,_exitems);\n";
        if (trans_rethrow()) {
          _o << "throw;\n";
        }
        _o << "}\n";
      }
      _o << "}\n"
         << "MemAccess::OpType commit(" << coretype << "& core,unsigned index) {\n";
      if (_config.mem_if() == mNonBlocking && _needs_commit_coro) {
        _o << "core._co = &_cco;\n";
      }
      _o << "if (Tracing) core.logger().log_current_instr(this);\n";
      if (haveExceptions()) {
        _o << "try {\n";
      }
      _o << "return (core.*_commit)(this,_opc,index);\n";
      if (haveExceptions()) {
        _o << "}\n"
           << "catch (" << coretype << "::Exception exdata) {\n"
           << "if (Tracing) core.logger().log_current_instr(this);\n"
           << "_exdata |= exdata;\n"
           << "_exitems = core." << getExceptionSetupFunc() << "(exdata,_exitems);\n";
        if (trans_rethrow()) {
          _o << "throw;\n";
        } else {
          _o << "return MemAccess::opNone;\n";
        }
        _o << "}\n";
      }
      _o << "}\n";
    }
    _o << "// Returns (<branch-type>,<branch-taken>).\n"
       << "BranchInfoType branch_info(" << coretype << " &core) {\n"
       << "return (core.*_branch_info)(this);\n"
       << "}\n";

    if (!hybrid_iss()) {
      _o << "addr_t branch_target() {\n"
         << "return _slots[_branch_target]._v.uint64();\n"
         << "}\n";

      _o << "MemAccess::OpType exec(" << coretype << "& core) {\n"
         << "core." << PktName << " = this;\n";
      if (_config.mem_if() == mNonBlocking) {
        _o << "core._co = &_co;\n";
      }
      _o << "if (Tracing) core.logger().log_current_instr(this);\n";
      if (haveExceptions()) {
        _o << "try {\n";
      }
      _o << "return (core.*_exec)(_opc);\n";
      if (haveExceptions()) {
        _o << "}\n"
           << "catch (" << coretype << "::Exception exdata) {\n"
           << "_exdata |= exdata;\n"
           << "_exitems = core." << getExceptionSetupFunc() << "(exdata,_exitems);\n";
        if (trans_rethrow()) {
          _o << "throw;\n";
        } else {
          _o << "return MemAccess::opNone;\n";
        }
        _o << "}\n";
      }
      _o << "}\n";
    }

    if (gen_instr_attrs()) {
      _o << "const Attrs::AttrList &attr_info() {\n"
         << "return (_attr_info)();\n"
         << "}\n"

         << "int field_info(unsigned index) {\n"
         << "return (_field_info)(index);\n"
         << "}\n";
    }

    if (have_delays) {
      _o << "int delay_info(" << coretype << " &core,unsigned index) {\n"
         << "return (core.*_delay_info)(this,index);\n"
         << "}\n";
    }

    _o << "bool has_exceptions() const {\n";
    if (haveExceptions()) {
      _o << "return _exdata;\n";          
    } else {
      _o << "return false;\n";
    }
    _o << "}\n"

       << "void set_exceptions(unsigned e) {\n";
    if (haveExceptions()) {
      _o << "_exdata |= e;\n";
    }
    _o << "}\n"

       << "void commit_exceptions(" << coretype << "&core) {\n";
    if (haveExceptions()) {
      _o << "if (_exdata) {\n"
         << "if (Tracing) core.logger().log_current_instr(this);\n"
         << "core." << PktName << " = this;\n"
         << "core.genExceptionsInternal(_exdata,_exitems);\n"
         << "}\n";
    }
    _o << "}\n\n"

       << "void clear_exceptions() {\n";
    if (haveExceptions()) {
      _o << "_exdata = 0;\n";
    }
    _o << "}\n\n";

    gc_string po = (hybrid_iss()) ? "printOperandH" : "printOperand";
    _o << "virtual bool printOp(std::ostream &o,unsigned index) const {\n"
       << "return " << po << "<" << PktType << "," << coretype << ">(o,index,*this,false);\n"
       << "}\n"
       << "virtual bool printOpResolved(std::ostream &o,unsigned index) const {\n"
       << "return " << po << "<" << PktType << "," << coretype << ">(o,index,*this,true);\n"
       << "}\n"
       << "};\n\n";
    if (!cached_packets()) {
      _o << "PacketPool<" << PktType << "," << _config.trans_pool_size() << "> _packets;\n\n";
    } else {
      _o << "PacketAlloc<" << PktType << "> _packets;\n\n";
    }
    if (_config.mem_if() == mNonBlocking) {
      _o << "PacketPool<StackObj," << _config.coro_pool_size() << "> _coro_stacks;\n\n";
    }
  }

};

GenInstrPacketBaseIF::GenInstrPacketBaseIF(const Core &core,const ConfigDB &config) :
  _impl(new GenInstrPacketBase(core,config))
{
}

GenInstrPacketBaseIF::GenInstrPacketBaseIF(GenInstrPacketBase *x) :
  _impl(x)
{
}

ImplCall(GenInstrPacketBaseIF,doSlotAllocation);

unsigned GenInstrPacketBaseIF::max_slots() const
{
  return _impl->max_slots();
}

const InstrSlotAllocs *GenInstrPacketBaseIF::instr_slots() const
{
  return _impl->instr_slots();
}

const InstrSlotAlloc &GenInstrPacketBaseIF::decode_miss_slots() const
{
  return _impl->decode_miss_slots();
}


GenInstrPacketIF::GenInstrPacketIF(opfxstream &o,InstrPacketMap &packet_map,
                                   const Core &core,InstrWalker &walker,
                                   ClassData &cd,const ConfigDB &config,
                                   const CoreConfig &coreconfig) :
  _impl(0)
{
  IfFind(iter,packet_map,&core) {
    _impl = new GenInstrPacket(o,*(iter->second),walker,cd,coreconfig);
  } else {
    _impl = new GenInstrPacket(o,core,walker,cd,config,coreconfig);
  }
};

ImplCall(GenInstrPacketIF,generateInstrPacket);

unsigned GenInstrPacketIF::instrPacketWidth() const
{
  return _impl->instrPacketWidth();
}

const InstrSlotAllocs *GenInstrPacketIF::instr_slots() const
{
  return _impl->instr_slots();
}

const InstrSlotAlloc &GenInstrPacketIF::decode_miss_slots() const
{
  return _impl->decode_miss_slots();
}

unsigned GenInstrPacketIF::max_slots() const
{
  return _impl->max_slots();
}  

const SlotAlloc &GenInstrPacketIF::branch_taken_slots() const
{
  return _impl->branch_taken_slots();
}

const SlotAlloc &GenInstrPacketIF::post_exec_slots() const
{
  return _impl->post_exec_slots();
}

const SlotAlloc &GenInstrPacketIF::mem_comm_slots() const
{
  return _impl->mem_comm_slots();
}

const SlotAllocs &GenInstrPacketIF::helper_slots() const
{
  return _impl->helper_slots();
}

}

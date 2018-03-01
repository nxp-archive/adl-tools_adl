//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Code transformer for instruction action code.
//

#include <stdexcept>
#include <sstream>
#include <iostream>
#include <cassert>

#include "opencxx/parser/Ptree.h"
#include "opencxx/parser/PtreeUtil.h"
#include "opencxx/parser/PtreeBlock.h"
#include "opencxx/parser/ptree-generated.h"
#include "opencxx/Environment.h"
#include "opencxx/TypeInfo.h"
#include "opencxx/Bind.h"

#include "helpers/Macros.h"
#include "helpers/CodeGen.h"

#include "fe/Data.h"
#include "fe/Helpers.h"
#include "fe/FieldSupport.h"
#include "fe/ResWalker.h"
#include "fe/InstrTable.h" //

#include "MiscTypes.h"
#include "InstrWalker.h"
#include "GenInstrPacket.h"
#include "GenMems.h"

using namespace std;
using namespace Opencxx;
using namespace PtreeUtil;

namespace adl {

  void InstrWalker::reset() 
  { 
    _cur_reg.clear(); 
    _ctx_index = 0;
    _alt_thisreg_index = 0;
    _cur_cache.clear(); 
    _reg_sfx.clear();
    reset_regs(); 
    _has_halt = false; 
    _record_mem_accesses = false;
    _hybrid_iss = false;
    _mem_accesses.clear();
    set_memory_suffix_default();
    _trans_conds = 0;
    _parm_trans = 0;
    _parm_assigns.clear();
  }

  void InstrWalker::set_class_name(const gc_string &cn,const gc_string &cnp)
  {
    _class_name = cn;
    _class_name_pfx = cnp;
  }

  ParmTransType InstrWalker::set_trans_parms(ParmTransType p) 
  { 
    ParmTransType o = _translate_parms; 
    _translate_parms = p; 
    if (p == ptNone) {
      _parm_trans = 0;
    }
    return o; 
  };

  void InstrWalker::set_cur_reg (const gc_string &x,bool is_ctx_res) 
  { 
    _cur_reg = x; 
    if (is_ctx_res) {
      _ctx_index = List(AdlGenSym(),Ptree::Make(","));
    } else {
      _ctx_index = 0;
    }
  }

  Ptree *InstrWalker::get_ctx_index() const 
  { 
    return First(_ctx_index);
  }

  Ptree *InstrWalker::ctx_arg() const
  {
    return _ctx_index;
  }

  Ptree *InstrWalker::translateCode(Environment *e,Ptree *p)
  {
    _has_skip = false;
    _memory_accessed = false;
    _regs.clear();
    return RtlWalker::translateCode(e,p);
  }

  Ptree *InstrWalker::translateCode(Environment *e,Ptree *p,bool reset)
  {
    if (reset) {
      _regs.clear();
    }
    _has_skip = false;
    _memory_accessed = false;
    return RtlWalker::translateCode(e,p);
  }

  bool InstrWalker::is_zero(Ptree *x) const
  {
    uint32_t value;
    if (get_uint(value,x,env)) {
      if (value == 0) {
        return true;
      }
    }
    return false;
  }

  const Parm *InstrWalker::is_parm(Ptree *x) const
  {
    return (_core) ? _core->get_parm(x->ToString()) : 0;
  }

  bool InstrWalker::is_halt_oper(Ptree *x) const
  {
    return Eq(x,"halt");
  }

  bool InstrWalker::is_cur_reg(const RegBase *x) const
  {
    return (x->name() == _cur_reg) ;
  }

  bool InstrWalker::is_cur_cache(Opencxx::Ptree *x) const
  {
    return Eq(x,CurrentCacheName);
  }

  bool InstrWalker::is_parent_method(Ptree *x) const
  {
    if (Eq(x,"raiseException")) {
      return true;
    } else if (Eq(x,"setCurrentInstrTable")) {
      return true;
    } else {
      return false;
    }
  }

  const MmuLookup *InstrWalker::is_mmulookup(Ptree *x)
  {
    if (_core) {
      if (const MmuLookup *lookup = _core->get_mmulookup(x->ToString())) {
        return lookup;
      }
    }
    return 0;
  }

  bool InstrWalker::is_context(Ptree *x) const
  {
    if (_core) {
      if ( _core->get_ctx(x->ToString())) {
        return true;
      }
    }
    return false;
  }
  
  bool InstrWalker::is_prefix_info(Ptree *x) const 
  {
    return _core ? _core->is_prefix_info(x->ToString()) : false;
  }

  bool InstrWalker::is_cur_reg_enumerated(Ptree *x) 
  {
    if (_core && !_cur_reg.empty()) {
      const RegBase *rb = 0;
      if ((rb =_core->get_reg(_cur_reg)) || (rb = _core->get_regfile(_cur_reg))) {
        return rb->is_enumerated(x->ToString());
      }
    }
    return false;
  }

  bool InstrWalker::is_enumerated(Ptree *r, Ptree *x) 
  {
    const RegBase *rb = 0;
    if ((rb = is_register(r)) || (rb = is_regfile(r))) {
      return rb->is_enumerated(x->ToString());
    }
    return false;
  }

  const Reg *InstrWalker::is_register(Ptree *x)
  {
    // Otherwise, examine the core.
    if (_core) {
      // If we have a current-register set and we come across the
      // current-register indicator name, then we regard this as
      // a register.
      if (!_cur_reg.empty() && Eq(x,CurrentRegName)) {
        return _core->get_reg(_cur_reg);
      }
      if (const Reg *reg = _core->get_reg(x->ToString())) {
        if (_reg_record) {
          _regs.push_back(reg);
        }
        return reg;
      }
    }
    return 0;
  }

  const RegFile *InstrWalker::is_regfile(Ptree *x)
  {
    if (_core) {
      if (!_cur_reg.empty() && Eq(x,CurrentRegName)) {
        return _core->get_regfile(_cur_reg);
      }
      if (const RegFile *rf = _core->get_regfile(x->ToString())) {
        if (_reg_record) {
          _regs.push_back(rf);
        }
        return rf;
      }
    }
    return 0;
  }

  const RegField *InstrWalker::is_field(Ptree *x,const RegBase &reg) const
  {
    RegFieldHash::const_iterator i = reg.fields().find(x->ToString());
    if (i != reg.fields().end()) {
      return i->second;
    }
    return 0;
  }

  const InstrField *InstrWalker::is_instrfield(Ptree *x)  const
  {
    return _core ? _core->get_instrfield(x->ToString()) : 0;
  }

  const SubInstr *InstrWalker::is_subinstr(Ptree *x) const
  {
    return _core ? _core->get_subinstr(x->ToString()) : 0;
  }

  const SlotAlloc *InstrWalker::is_slot_func(const gc_string &fn) const
  {
    IfFind(x,_slot_funcs,fn) {
      return x->second;
    } else {
      return 0;
    }
  }


  Ptree *InstrWalker::genArgsTree(const SubInstr &subinstr,const SubInstrGrp &subinstrgrp) 
  {
    gc_ostringstream o;
    // List all arguments- the instruction fields.
    int numfields = subinstr.sizeFields();
    // Grab all instruction field values.
    for (int i = 0; i != numfields; ++i) {
      Instr::Field fp = subinstr.getField(i);
      const InstrField &field = *(fp._field);
      if (field.is_overlay()) {
        continue;
      }
      if (i) {
        o << ",";
      }
      gc_string suffix;
      if (is_special(field)) {
        suffix += "_p";
      }
      o << field.name() << "_" << subinstrgrp.name() << suffix;
    }
  
    return Ptree::Make("%s",o.str().c_str());
  }

  bool InstrWalker::isFunction(Ptree *exp)
  {
    TypeInfo t;
    exp->Typeof(this,t);
    return t.IsFunction();
  }

  // Grab a slice of an array from src and put into trg.  You can specify slice
  // indices or a special symbol that will end the slice (end_sym).
  int arraySlice(PtreeArray &trg,PtreeArray &src,int start = 0,int end = -1,const gc_string &end_sym = gc_string())
  {
    if (end < 0 || end > (int)src.Number()) {
      end = src.Number();
    }
    if (start >= (int)src.Number()) {
      start = 0;
    }
    trg.Clear();
    for (int i = start; i != end; ++i) {
      if (!end_sym.empty() && Eq(src[i],end_sym.c_str())) {
        return i;
      }
      trg.Append(src[i]);
    }
    return -1;
  }

  // This returns a slot index for use by a hybrid ISS.
  Ptree *InstrWalker::getSlotRefIndex(const RegBase *reg,Ptree *index,bool direct)
  {
    if (!_slots || !_hybrid_iss || direct) {
      return 0;
    }
    gc_ostringstream ss;
    ss << "(";
    auto x = _slots->getSlot(reg->name(),index);
    if (x.first) {
      if (x.first->_sname) {
        ss << x.first->_sname;
      } else {
        ss << x.first->_start;
      }
      if (index && !x.second->is_fixed()) {
        ss << "+(" << Translate(index) << ")";
      }
      ss << " )";
      return Ptree::Make("%s",ss.str().c_str());
    } else {
      return 0;
    }
  }

  // This creates code which marks a register as used.
  Ptree *InstrWalker::getSlotUsed(const RegBase *reg,Ptree *index,Ptree *delay,bool direct,Ptree *expr)
  {
    if (!_slots || !_hybrid_iss) {
      return 0;
    }
    auto hs = getSlotRefIndex(reg,index,direct);
    if (hs) {
      gc_ostringstream ss;
      ss << PktName << "->slot_used(" << hs;
      if (delay) {
        ss << ", " << delay;
      }
      // Note: We join this with a semi-colon to avoid issues with trailing
      // if/else expressions which aren't surrounded by braces.  Use of the comma
      // makes this a single expression.
      ss << ") , ";
      return Ptree::Make("%s",ss.str().c_str());
    } else {
      return 0;
    }
  }

  // This creates a call to the core's target update function on an NIA update,
  // for hybrid ISSs.
  Ptree *InstrWalker::getTargetUpdate(const RegBase *reg,Ptree *expr)
  {
    if (_slots && _hybrid_iss && _core && _instr_item && _core->nia() == reg) {
      return Ptree::Make(", %s(to_addr(%p),%d)",getBrTrgUpdateName().c_str(),expr,_instr_item->fetch_width()/8);
    } else {
      return 0;
    }
  }

  // This creates code which marks a register as read.
  Ptree *InstrWalker::getSlotRead(const RegBase *reg,Ptree *index,Ptree *left,Ptree *right,bool direct)
  {
    if (!_slots || !_hybrid_iss || !_track_reads) {
      return 0;
    }
    auto hs = getSlotRefIndex(reg,index,direct);
    if (hs) {
      gc_ostringstream ss;
      ss << PktName << "->slot_read(" << hs;
      if (left) {
        ss << ", to_uint32(" << left << ")";
      }
      if (right) {
        ss << ", to_uint32(" << right << ")";
      }
      ss << ")";
      return Ptree::Make("%s",ss.str().c_str());
    } else {
      return 0;
    }
  }

  // This returns a slot access for use by a transactional ISS.
  Ptree *InstrWalker::genSlotAccess(const RegBase *reg,Ptree *index,bool is_assign,bool is_ref,bool is_partial,bool allow_hybrids,
                                    bool direct,Ptree *delay)
  {
    // Only proceed if we have slots and we're not a hybrid ISS or we're
    // allowing hybrid ISSs explicitly.
    if (!_slots || (_hybrid_iss && !allow_hybrids) || direct) {
      return 0;
    }
    const char *delay_str = 0;
    if (delay) {
      delay_str = Ptree::Make("to_uint32(%p)",delay)->ToString();
    }
    auto x = _slots->getSlot(reg->name(),index);
    if (x.first) {
      Ptree *index2 = Translate(index);
      if (x.first->_sname) {
        return Ptree::Make("%s",makeSlotRef(x.first->_sname,x.second,is_assign,is_ref,is_partial,_packet_width,index2->ToString(),delay_str).c_str());
      } else {
        return Ptree::Make("%s",makeSlotRef(x.first->_start,x.second,is_assign,is_ref,is_partial,_packet_width,index2->ToString(),delay_str).c_str());
      }
    } else {
      return 0;
    }
  }

  // Transforms a slice, which could consist of either a pair of expressions or just a single expression,
  // by surrounding each argument with a call to to_uint32().  Returns the transformed item and true if
  // a pair, false if single.
  pair<Ptree *,bool> InstrWalker::transformSlice(Ptree *slice_left,Ptree *slice_right)
  {
    if ( slice_right ) {
      return make_pair(Ptree::Make("to_uint32(%p),to_uint32(%p)",rename(slice_left),rename(slice_right)),true);
    } else {
      return make_pair(Ptree::Make("to_uint32(%p)",rename(slice_left)),false);
    }
  }

  // If the register is the NIA, then check to see if we should be using the
  // "simple" form, which doesn't log branch-taken activity.
  Ptree *InstrWalker::get_nia_suffix(const Reg *reg)
  {
    if (_core && _core->nia() == reg && _nia_simple_mode) {
      return Ptree::Make(getNiaSfx().c_str());
    }
    return 0;
  }

  // If a delayed write, this returns the suffix used to call the correct
  // function.
  Ptree *get_delay_suffix(Ptree* delayed)
  {
    return (delayed) ? Ptree::Make("_delayed") : 0;
  }

  Ptree *get_delay_arg(Ptree *delayed)
  {
    return (delayed) ? Ptree::Make(",to_uint32(%p)",delayed) : 0;
  }

  Ptree *make_slice_args(Ptree *slice_left,Ptree *slice_right)
  {
    Ptree *args;
    if (slice_right) {
      args = List(slice_left,Ptree::Make(","),slice_right);
    } else {
      args = List(slice_left);
    }
    return List(Ptree::Make("("),args,Ptree::Make(")"));
  }

  // Generate code for a slice read.  This is a little convoluted: We create a
  // function call object from what we have (the read expression and the slice
  // parameters) so that the RtlWalker's TranslateFuncall will properly handle
  // sizing the resulting slice.
  Ptree *InstrWalker::genSliceRead(Ptree *read,Ptree *slice_left,Ptree *slice_right)
  {
    auto fc = new PtreeFuncallExpr(read,make_slice_args(slice_left,slice_right));
    return Translate(fc);
  }

  // Generates code for a write to a slice of a register file.  We don't have to
  // handle reads in a special manner, because the read of the slice is simply a
  // slice access to a normal read of a register or register file.
  Ptree *InstrWalker::genRegSliceWrite(const RegBase *reg,Ptree *slice_left,Ptree *slice_right,Ptree *expr,Ptree *ctx_elm,Ptree* delayed,bool direct)
  {
    Ptree *slot = genSlotAccess(reg,0,expr,false,true,true,direct,0);
    Ptree *slot1 = (slot) ? Ptree::Make("%p, ",slot) : 0;

    auto s2 = transformSlice(slice_left,slice_right);

    if (ctx_elm) {
      return Ptree::Make("%s_ctx_slice_set( to_uint32(%p),%p,( %p ) )",reg->name().c_str(),ctx_elm,s2.first,expr);
    } else {
      if (is_cur_reg(reg)) {
        return Ptree::Make("%s_slice_set_inhook%s( %p , ( %p ) )",reg->name().c_str(),_reg_sfx.c_str(),s2.first,expr);
      } else {
        auto d = get_delay_suffix(delayed);
        auto d1 = get_delay_arg(delayed);
        return Ptree::Make("%s_slice_set%p(%p %p,( %p ) %p )",reg->name().c_str(),d,slot1,s2.first,expr,d1);
      }
    }
  }

  // Generates code for a write to a slice of a register file.  We don't have to
  // handle reads in a special manner, because the read of the slice is simply a
  // slice access to a normal read of a register or register file.
  Ptree *InstrWalker::genRegFileSliceWrite(const RegBase *reg,Ptree *index,Ptree *rindex,Ptree *slice_left,Ptree *slice_right,Ptree *expr,Ptree *ctx_elm,Ptree *delayed,bool direct)
  {
    Ptree *slot = genSlotAccess(reg,rindex,expr,false,true,true,direct,0);
    Ptree *slot1 = (slot) ? Ptree::Make("%p, ",slot) : 0;
    
    auto s2 = transformSlice(slice_left,slice_right);

    auto d = get_delay_suffix(delayed);
    auto d1 = get_delay_arg(delayed);

    if ( s2.second ) {
      if (ctx_elm) {
        return Ptree::Make("%s_ctx_slice_set(to_uint32(%p) , to_uint32(%p), %p , ( %p ) );",reg->name().c_str(),ctx_elm,index,s2.first,expr);
      } else {
        if (is_cur_reg(reg)) {
          return Ptree::Make("%s_slice_set_inhook%s( %p , %p , ( %p ) );",reg->name().c_str(),_reg_sfx.c_str(),index,s2.first,expr);
        } else {
          return Ptree::Make("%s_slice_set%p(%p to_uint32(%p), %p , ( %p ) %p );",reg->name().c_str(),d,slot1,index,s2.first,expr,d1);
        }
      }
    } else {
      if (ctx_elm) {
        return Ptree::Make("%s_ctx_slice_set(to_uint32(%p) , to_uint32(%p), %p , %p , ( %p ) );",reg->name().c_str(),ctx_elm,index,s2.first,s2.first,expr);
      } else {
        if (is_cur_reg(reg)) {
          return Ptree::Make("%s()(%p).set_data().set( to_uint32(%p) , %p, ( %p ) );",reg->name().c_str(),index,s2.first,s2.first,expr);
        } else {
          return Ptree::Make("%s_slice_set%p(%p to_uint32(%p), %p , %p , ( %p ) %p );",reg->name().c_str(),d,slot1,index,s2.first,s2.first,expr,d1);
        }
      }
    }
  }

  // In a transactional ISS, the branch-taken handler must be called
  // immediately, so that its effects will be seen.
  Ptree *InstrWalker::genBranchTakenCall(const Reg *reg)
  {
    if (_core && (reg == _core->nia())) {
      if (_core->branch_taken_handler()) {
        return Ptree::Make(" %s(); ",getBrTakenHandlerName().c_str());
      }
    }
    return 0;
  }

  // Generates code for accessing a register.
  Ptree *InstrWalker::genRegAccess(const Reg *reg,Ptree *expr,Ptree *ctx_elm,Ptree *delayed,bool direct,Ptree *l,Ptree *r)
  {
    if (is_cur_reg(reg)) {
      if (expr) {
        return Ptree::Make("%s_write_inhook%s ( %p %p )",_cur_reg.c_str(),_reg_sfx.c_str(),ctx_arg(),expr);
      } else {
        // For the current register (register variables we don't want to treat
        // in the normal fashion because we're creating a hook, and thus
        // bypassing the usual referencing logic) we have to modify it so that
        // we access the _data member.
        return make_bitslice(Ptree::Make("%s_read_inhook(%p)",_cur_reg.c_str(),get_ctx_index()),reg->width(),0);
      }
    } else {
      auto var = Ptree::Make("%s",reg->name().c_str());
      if (expr) {
        // We have a write.
        if (ctx_elm) {
          // Write to a specific context.
          return Ptree::Make("%p_ctx_write( to_uint32(%p) , %p )",var,ctx_elm,expr);
        } else {
          Ptree *slot = genSlotAccess(reg,0,true,true,false,false,direct,delayed);
          Ptree *bt_call = genBranchTakenCall(reg);
          auto nia_sfx = get_nia_suffix(reg);
          auto d = get_delay_suffix(delayed);
          auto d1 = get_delay_arg(delayed);
          if (slot) {
            if (_conditional_slot_access) {
              return make_bitslice(Ptree::Make("{ if (%s) %p(%p)); else %p_write%p(%p %p); %p }",PktName,slot,expr,var,d,expr,d1,bt_call),reg->width(),0);
            } else {
              return Ptree::Make("{ %p ( %p )); %p }",slot,expr,bt_call);
            }
          } else {
            Ptree *hs = getSlotUsed(reg,0,delayed,direct,expr);
            Ptree *tu = getTargetUpdate(reg,expr);
            if (_conditional_slot_access && hs) {
              hs = Ptree::Make("if (%s) %p",PktName,hs);
            }
            Ptree *tmp = Ptree::Make("%p %p_write%p%p( %p %p) %p",hs,var,nia_sfx,d,expr,d1,tu);
            if (reg == _suffix_reg) {
              // If we match the suffix register then add on the specified code.
              return Ptree::Make("{ %p ; %s ; }",tmp,_suffix_stmt.c_str());
            } else {
              return tmp;
            }
          }
        }
      } else {
        // We have a read.
        if (ctx_elm) {
          // Read from a specific context.
          return make_bitslice(Ptree::Make("%p_ctx_read(to_uint32(%p))",var,ctx_elm),reg->width(),0);
        } else {
          Ptree *slot = genSlotAccess(reg,0,false,true,false,false,direct,0);
          if (slot) {
            if (_conditional_slot_access) {
              return make_bitslice(Ptree::Make("((%s) ? %p : %p_read())",PktName,slot,var),reg->width(),0);
            } else {
              return make_bitslice(slot,reg->width(),0);
            }
          } else {
            Ptree *hs = getSlotRead(reg,0,l,r,direct);
            if (hs) {
              return make_bitslice(Ptree::Make("( %p , %p_read())",hs,var),reg->width(),0);
            } else {
              return make_bitslice(Ptree::Make("%p_read()",var),reg->width(),0);
            }
          }
        }
      }
    }
  }

  // Generates code for accessing a register file.
  Ptree *InstrWalker::genRegFileAccess(const RegFile *rf,Ptree *index,Ptree *rindex,Ptree *expr,Ptree *ctx_elm,Ptree *delayed,
                                       bool direct,Ptree *left, Ptree *right)
  {
    auto var = Ptree::Make("%s",rf->name().c_str());
    if (is_cur_reg(rf)) {
      if (expr) {
        // Assignment to a register file.

        // We don't allow a write to a register-file with two arguments- this is
        // probably a typo, where the user thought they were writing to a
        // register.
        if (Match(index,"[ %* , %* ]")) {
          RError("Invalid write to a register file (did you forget an index, e.g. REGFILE(index)(x,y))?:  " << var);
        }
      
        return Ptree::Make("%s_write_inhook%s( %p %p , ( %p ) )",_cur_reg.c_str(),_reg_sfx.c_str(),ctx_arg(),index,expr);
      } else {
        // Read from a register file.
        return make_bitslice(Ptree::Make("%p_read_inhook(%p to_uint32(%p))",var,ctx_arg(),index),rf->width(),0);
      }
    } else {
      if (expr) {
        if (Match(index,"[ %* , %* ]")) {
          RError("Invalid write to a register file (did you forget an index, e.g. REGFILE(index)(x,y))?:  " << var);
        }
        if (ctx_elm) {
          // Write to a specific thread context.
          return Ptree::Make("%p_ctx_write( to_uint32(%p) , to_uint32(%p) , ( %p ) )",var,ctx_elm,index,expr);
        } else {
          Ptree *slot = genSlotAccess(rf,rindex,true,true,false,false,direct,delayed);
          if ( slot ) {
            if (_conditional_slot_access) {
              return make_bitslice(Ptree::Make("{ if (%s) %p(%p); else %p_write(to_uint32(%p) , (%p)); }",PktName,slot,expr,var,index,expr),rf->width(),0);
            } else {
              return Ptree::Make("%p ( %p ))",slot,expr);
            }
            return Ptree::Make("%p ( %p ) )",slot,expr);
          } else {
            Ptree *hs = getSlotUsed(rf,rindex,delayed,direct,expr);
            if (_conditional_slot_access && hs) {
              hs = Ptree::Make("if (%s) %p",PktName,hs);
            }
            auto d = get_delay_suffix(delayed);
            auto d1 = get_delay_arg(delayed);
            return Ptree::Make("%p %p_write%p( to_uint32(%p) , ( %p ) %p)",hs,var,d,index,expr,d1);
          }
        }
      } else {
        // Read from a register file.
        if (ctx_elm) {
          // Read from a specific thread context.
          return make_bitslice(Ptree::Make("%p_ctx_read( to_uint32(%p) , to_uint32(%p) )",var,ctx_elm,index),rf->width(),0);
        } else {
          Ptree *slot = genSlotAccess(rf,rindex,false,true,false,false,direct,0);
          if ( slot ) {
            // We're transactional, so the read should read from the instruction
            // packet.
            if (_conditional_slot_access) {
              return make_bitslice(Ptree::Make("((%s) ? %p : %p_read( to_uint32(%p) ) )",PktName,slot,var,index),rf->width(),0);
            } else {
              return make_bitslice(slot,rf->width(),0);
            }
          } else {
            Ptree *hs = getSlotRead(rf,rindex,left,right,direct);
            if (hs) {
              return make_bitslice(Ptree::Make("( %p , %p_read( to_uint32(%p) ))",hs, var,index),rf->width(),0);
            } else {
              return make_bitslice(Ptree::Make("%p_read( to_uint32(%p) )",var,index),rf->width(),0);
            }

          }
        }
      }
    }
  }

  // Generates code for accessing a field of a register.
  Ptree *InstrWalker::genRegFieldAccess(const RegBase *reg,const RegField *field,Ptree *field_index,
                                        Ptree *expr,Ptree *ctx_elm,Ptree *delayed,bool direct)
  {
    bool is_cr = is_cur_reg(reg);

    if (field_index && !field->indexed()) {
      RError("Field " << field->name() << " is not an indexed field.");
    }

    Ptree *slot = genSlotAccess(reg,0,expr,false,true,true,direct,0), *slot1 = 0;
    if (slot) {
      slot1 = Ptree::Make("%p, ",slot);
    }

    if (field_index) {
      // We have an indexed field.
      if (expr) {
        // We have an assignment
        if (ctx_elm) {
          // We're accessing a field of a register in a specific context.
          return Ptree::Make("%s_%s_ctx_write(to_uint32(%p) , to_uint32(%p),(%p))",reg->name().c_str(),field->name().c_str(),ctx_elm,field_index,expr);
        } else {
          if (is_cr) {
            return Ptree::Make("%s_%s_write_inhook%s(%p to_uint32(%p),(%p))",reg->name().c_str(),field->name().c_str(),_reg_sfx.c_str(),ctx_arg(),field_index,expr);
          } else {
            auto d = get_delay_suffix(delayed);
            auto d1 = get_delay_arg(delayed);
            if (_conditional_slot_access && slot) {            
              return Ptree::Make("((%s) ? %s_%s_write%p(%p to_uint32(%p),(%p) %p) : %s_%s_write%p(to_uint32(%p),(%p) %p))",
                                 PktName,reg->name().c_str(),field->name().c_str(),d,slot1,field_index,expr,d1,
                                 reg->name().c_str(),field->name().c_str(),d,field_index,expr,d1);
            } else {
              return Ptree::Make("%s_%s_write%p(%p to_uint32(%p),(%p) %p)",reg->name().c_str(),field->name().c_str(),d,slot1,field_index,expr,d1);
            }
          }
        }
      } else {
        // We have a read-access.
        if (ctx_elm) {
          // We're accessing a field of a register in a specific context.
          return make_bitslice(Ptree::Make("%s_%s_ctx_read(to_uint32(%p) , to_uint32(%p))",reg->name().c_str(),field->name().c_str(),ctx_elm,field_index),field->width());
        } else {
          if (_conditional_slot_access && slot) {
            return make_bitslice(Ptree::Make("((%s) ? %s_%s_read(%p to_uint32(%p)) : %s_%s_read(to_uint32(%p))",PktName,reg->name().c_str(),
                                             field->name().c_str(),slot1,field_index,reg->name().c_str(),field->name().c_str(),field_index),field->width());
          } else {
            return make_bitslice(Ptree::Make("%s_%s_read(%p to_uint32(%p))",reg->name().c_str(),field->name().c_str(),slot1,field_index),field->width());
          }
        }
      }
    } else {
      // We have a non-indexed field.
      if (expr) {
        // We have an assignment.
        if (ctx_elm) {
          return Ptree::Make("%s_%s_ctx_write(to_uint32(%p) , %p)",reg->name().c_str(),field->name().c_str(),ctx_elm,expr);
        } else {
          if (is_cr) {
            return Ptree::Make("%s_%s_write_inhook%s(%p %p)",reg->name().c_str(),field->name().c_str(),_reg_sfx.c_str(),ctx_arg(),expr);
          } else {
            auto d = get_delay_suffix(delayed);
            auto d1 = get_delay_arg(delayed);
            if (_conditional_slot_access && slot) {
              return Ptree::Make("((%s) ? %s_%s_write%p(%p %p %p) : %s_%s_write%p(%p %p))",
                                 PktName,reg->name().c_str(),field->name().c_str(),d,slot1,expr,d1,
                                 reg->name().c_str(),field->name().c_str(),d,expr,d1);
            } else {
              return Ptree::Make("%s_%s_write%p(%p %p %p)",reg->name().c_str(),field->name().c_str(),d,slot1,expr,d1);
            }
          }
        }
      } else {
        // We have a read-access.
        if (ctx_elm) {
          return make_bitslice(Ptree::Make("%s_%s_ctx_read(to_uint32(%p))",reg->name().c_str(),field->name().c_str(),ctx_elm),field->width());
        } else {
          if (_conditional_slot_access && slot) {
            return make_bitslice(Ptree::Make("((%s) ? %s_%s_read(%p) : %s_%s_read())",PktName,
                                             reg->name().c_str(),field->name().c_str(),slot,reg->name().c_str(),field->name().c_str()),field->width());
          } else {
            return make_bitslice(Ptree::Make("%s_%s_read(%p)",reg->name().c_str(),field->name().c_str(),slot),field->width());
          }
        }
      }
    }
  }

  // Generates code for accessing a field of a register file.
  Ptree *InstrWalker::genRegFileFieldAccess(const RegBase *reg,const RegField *field,Ptree *index,Ptree *rindex,
                                            Ptree *field_index,Ptree *expr,Ptree *ctx_elm,Ptree *delayed,bool direct)
  {
    bool is_cr = is_cur_reg(reg);

    if (field_index && !field->indexed()) {
      RError("Field " << field->name() << " is not an indexed field.");
    }

    Ptree *slot = genSlotAccess(reg,rindex,expr,false,true,true,direct,0);
    Ptree *slot1 = (slot) ? Ptree::Make("%p, ",slot) : 0;

    // We have a register file.
    if (field_index) {
      // We have an indexed field.
      if (expr) {
        // This is an assignment.
        if (ctx_elm) {
          // Write to a specific context.
          return Ptree::Make("%s_%s_ctx_write(to_uint32(%p),to_uint32(%p),to_uint32(%p),%p)",reg->name().c_str(),field->name().c_str(),ctx_elm,index,field_index,expr);          
        } else {
          if (is_cr) {
            return Ptree::Make("%s_%s_write_inhook%s(%p to_uint32(%p),to_uint32(%p),%p)",reg->name().c_str(),field->name().c_str(),_reg_sfx.c_str(),ctx_arg(),index,field_index,expr);
          } else {
            auto d = get_delay_suffix(delayed);
            auto d1 = get_delay_arg(delayed);
            if (_conditional_slot_access && slot) {
              return Ptree::Make("((%s) ? %s_%s_write%p(%p to_uint32(%p),to_uint32(%p),%p %p) : %s_%s_write%p(to_uint32(%p),to_uint32(%p),%p %p))",
                                 PktName,reg->name().c_str(),field->name().c_str(),d,slot1,index,field_index,expr,d1,
                                 reg->name().c_str(),field->name().c_str(),d,index,field_index,expr,d1);
            } else {
              return Ptree::Make("%s_%s_write%p(%p to_uint32(%p),to_uint32(%p),%p %p)",reg->name().c_str(),field->name().c_str(),d,slot1,index,field_index,expr,d1);
            }
          }
        }
      } else {
        // This is a read-access.
        if (ctx_elm) {
          // Read from a specific context.
          return make_bitslice(Ptree::Make("%s_%s_ctx_read(to_uint32(%p),to_uint32(%p),to_uint32(%p))",reg->name().c_str(),field->name().c_str(),ctx_elm,index,field_index),field->width());
        } else {
          if (_conditional_slot_access && slot) {
            return make_bitslice(Ptree::Make("((%s) ? %s_%s_read(%p to_uint32(%p),to_uint32(%p)) : %s_%s_read(to_uint32(%p),to_uint32(%p)))",
                                             PktName,
                                             reg->name().c_str(),field->name().c_str(),slot1,index,field_index,
                                             reg->name().c_str(),field->name().c_str(),index,field_index),field->width());
          } else {
            return make_bitslice(Ptree::Make("%s_%s_read(%p to_uint32(%p),to_uint32(%p))",reg->name().c_str(),field->name().c_str(),slot1,index,field_index),field->width());
          }
        }
      }
    } else {
      // We have a non-indexed field.
      if (expr) {
        // This is an assignment.
        if (ctx_elm) {
          return Ptree::Make("%s_%s_ctx_write(to_uint32(%p),to_uint32(%p),%p)",reg->name().c_str(),field->name().c_str(),ctx_elm,index,expr);
        } else {
          if (is_cr) {
            return Ptree::Make("%s_%s_write_inhook%s(%p to_uint32(%p),%p)",reg->name().c_str(),field->name().c_str(),_reg_sfx.c_str(),ctx_arg(),index,expr);
          } else {
            auto d = get_delay_suffix(delayed);
            auto d1 = get_delay_arg(delayed);
            if (_conditional_slot_access && slot) {
              return Ptree::Make("((%s) ? %s_%s_write%p(%p to_uint32(%p),%p %p) : %s_%s_write%p(to_uint32(%p),%p %p))",
                                 PktName,
                                 reg->name().c_str(),field->name().c_str(),d,slot1,index,expr,d1,
                                 reg->name().c_str(),field->name().c_str(),d,index,expr,d1);
            } else {
              return Ptree::Make("%s_%s_write%p(%p to_uint32(%p),%p %p)",reg->name().c_str(),field->name().c_str(),d,slot1,index,expr,d1);
            }
          }
        }
      } else {
        // This is a read-access.
        if (ctx_elm) {
          return make_bitslice(Ptree::Make("%s_%s_ctx_read(to_uint32(%p),to_uint32(%p))",reg->name().c_str(),field->name().c_str(),ctx_elm,index),field->width());
        } else {
          if (_conditional_slot_access && slot) {
            return make_bitslice(Ptree::Make("((%s) ? %s_%s_read(%p to_uint32(%p)) : %s_%s_read(%p to_uint32(%p)))",
                                             PktName,
                                             reg->name().c_str(),field->name().c_str(),slot1,index,
                                             reg->name().c_str(),field->name().c_str(),index),field->width());
          } else {
            return make_bitslice(Ptree::Make("%s_%s_read(%p to_uint32(%p))",reg->name().c_str(),field->name().c_str(),slot1,index),field->width());
          }
        }
      }
    }
  }

  // Generate code for writing to a TLB lookup.
  Ptree *InstrWalker::genMmuLookupWrite(const MmuLookup *lookup,Ptree *args,Ptree *expr)
  {
    return Ptree::Make("%s.write( %p , ( %p ))",lookup->name().c_str(),args,expr);
  }

  // Generate code for accessing a memory.
  Ptree *InstrWalker::genMemoryAccess(PtreeMemRead *pm,Ptree *expr) 
  {
    _memory_accessed = true;

    gc_string mem_name = getMemoryName((pm->_memaccess.is_global()) ? MemoryName : pm->_memaccess._mem->name());

    // We're not translating a memory hook.
    Ptree *addr   = Translate(pm->_memaccess._addr);
    unsigned size = pm->_memaccess._size;
    Ptree *tval   = Translate(pm->_memaccess._type);     // Optional type specifier.
    Ptree *cwaddr = Translate(pm->_memaccess._cw_addr);  // Optional critical-word address.
    Ptree *delay  = Translate(pm->_memaccess._delay);

    Ptree *type     = (tval)   ? Ptree::Make(",%p",tval) : 0;
    Ptree *cwflag_a = (cwaddr) ? Ptree::Make(",true") : 0;
    Ptree *cwaddr_a = (cwaddr) ? Ptree::Make(",%p",cwaddr) : 0;
    
    if (expr) {
      Ptree *expr2 = Translate(expr);

      // We only care about delay for writes.
      Ptree *delay_a         = (delay)  ? Ptree::Make(",%p",delay) : 0;
      const char  *delay_sfx = (delay)  ? "_delayed" : "";

      unsigned sz;
      unsigned of;
      bool is;
      if (get_bits_size(expr2,sz,of,is) && _record_mem_accesses) {
        // Record the access for later use.
        _mem_accesses.push_back(MemAccess(mem_name+"_"+_memory_write_suffix,size,sz));
      }
      // This is a write to memory.
      return Ptree::Make("%s_%s%s<%d>(%p,to_bits<ns_bits>(%p)%p%p%p%p)",
                         mem_name.c_str(),_memory_write_suffix.c_str(),delay_sfx,size,
                         addr,expr2,delay_a,type,cwflag_a,cwaddr_a);
    } else {
      if (_record_mem_accesses) {
        _mem_accesses.push_back(MemAccess(mem_name+"_"+_memory_read_suffix,size));
      }
      // This is a read from memory.
      return make_bitslice(Ptree::Make("%s_%s<%d>(%p%p%p%p)",mem_name.c_str(),_memory_read_suffix.c_str(),size,addr,type,cwflag_a,cwaddr_a),size*8);
    }
  }



  // Produces code for handling the info/warning/error directives.
  Ptree *InstrWalker::genMsgCode(MsgType type,Ptree *exp)
  {
    // Convert the info function into a block which gc_stringifies its arguments using
    // a gc_string-stream, then sends the result to the logging manager.
    PtreeArray arg_array;
    bool changed;
    populateArgs(arg_array,this,exp,0,changed);

    gc_string tstr;
    switch (type) {
    case Info:
      tstr = "mInfo";
      break;
    case Warning:
      tstr = "mWarn";
      break;
    case Error:
      tstr = "mError";
      break;
    }

    // We have arguments, so construct the block.
    if ((numArgs(arg_array) > 1)) {
      PtreeArray info_args;
      // Do we have a data-start symbol?  If so, only copy up to that item, then
      // put the rest into a data array.
      //  Copy 2nd argument on.  We start at
      // element 2 b/c this array contains the argument separators (the commas).
      int rc = arraySlice(info_args,arg_array,2,-1,AnnotationDataStart);
      shallowReplace(info_args,",",Ptree::Make(" << "));
      Ptree *body = 0;
      if (rc >= 0) {
        gc_vector<std::pair<Ptree *,Ptree *> > data_args;
        // Remove trailing << in info arg array.
        info_args[info_args.Number()-1] = 0;
        // Get the data arguments and put them into a vector of pairs.
        for (unsigned i = rc+2; i < arg_array.Number(); i = i+2) {
          Ptree *key,*value;
          if (Match(arg_array[i],"[ ( [ %? , %? ] ) ]",&key,&value)) {
            data_args.push_back(make_pair(key,value));
          } else {
            RError("Bad annotation argument:  " << exp);
          }
        }
        gc_ostringstream ss;
        
        bool first = true;
        ForEach(data_args,i) {
          list_sep(ss,first);
          ss << "LogBase::Data(" << i->first << ",(" << i->second << "))";
        }
        body = Ptree::Make("using namespace std;\n"
                           "ostringstream ss;\nss << %p;\n"
                           "LogBase::Data data[] = { %s };\n"
                           "_logger->log_annotation(%s,%p,ss.str(),%d,data);\n",
                           info_args.All(),ss.str().c_str(),tstr.c_str(),
                           arg_array[0],data_args.size());
      } else {
        body = Ptree::Make("using namespace std;\n"
                           "ostringstream ss;\nss << %p;\n"
                           "_logger->log_annotation(%s,%p,ss.str());\n",
                           info_args.All(),tstr.c_str(),arg_array[0]);
      }
      if (type == Info) {
        // For info messsages, we only produce them if tracing is on.
        return Ptree::Make("if (Tracing && (_trace_mode & ttAnnotation)) {\n%p}\n",body);
      } else {
        // For other kinds of messages, we always produce them.
        return Ptree::Make("{\n%p}\n",body);
      }
    } else {
      // Empty info function- return a null block.
      return Ptree::Make("{};");
    }
  }

  // Create the extra arguments used by a helper function for a transactional ISS.
  Ptree *InstrWalker::genHelperArgs()
  {
    PtreeArray args;

    bool first = true;
    ForEach(_slots->_items,iter) {
      const char *c = (first) ? "" : ",";
      first = false;
      args.Append(Ptree::Make("%s int %p",c,iter->second._sname));
    }

    return args.All();
  }
  
  Ptree *InstrWalker::rename(Ptree *var) {
    gc_string str = var->ToString();
    auto ir = _renames.find(str);    
    if (ir != _renames.end()) {
      str = ir->second;
      return Ptree::Make("%s",ir->second.c_str());
    } else {
      return var;
    }
  }

  Ptree *InstrWalker::getHelperDefaultParameter(const gc_string &fname,unsigned pindex)
  {
    Ptree *dv = 0;
    if (_core) {
      if (const HelperInfo *hi = _core->get_helper(fname)) {
        Ptree *args = Third(get_func_args(hi->_func));

        unsigned index = 0;
        while (args != 0) {
          Ptree *p = args->Car();

          if (index == pindex) {
            dv =  Nth(p,3);
            break;
          }
          args = args->Cdr();
          if (args != 0) {
            args = args->Cdr();
          }

          ++index;
        }        
      }
    }

    // Worst case- punt with 0.
    return (dv) ? dv : Ptree::Make("0");
  }

  // Adds on the extra arguments required by the helper to handle dynamic slot allocation.
  Ptree *InstrWalker::genHelperCall(const gc_string &fname,Ptree *fun,const SlotAlloc *sa,
                                    Ptree *orig_args,Ptree *final_args)
  {
    // First, collect up all sources and targets into a single list so that we
    // can iterate over a single thing.
    gc_list<const UsedReg *> res;
    ForEach(sa->_items,iter) {
      res.push_back(&iter->first);
    }

    // Now we go through and create the extra arguments.
    PtreeArray slot_args;

    int num_args = (Length(orig_args)+1)/2;

    SlotInfo dummy;
    dummy._start = -1;

    bool first = true;
    ForEach(res,iter) {
      const SlotInfo *r = 0;
      if (!first) {
        slot_args.Append(Ptree::Make(","));
      }
      first = false;
      if ((*iter)->is_parm()) {
        // Is it a parameter?  Then we use the parameter expression to find the resource, e.g. if
        // parameter 2 is RT+2 and is used within that function, then we need to find out what slot we're
        // using for RT+2.
        int p = (*iter)->_parm;
        Ptree *arg;
        if (p >= num_args) {
          // We assume that this is a default-argument case.  So, we have to get
          // the default argument from the function declaration.
          arg = getHelperDefaultParameter(fname,p);
        } else {
          arg = Nth(orig_args,p*2);
        }
        auto x = _slots->getSlot((*iter)->regb()->name(),arg);
        r = x.first;
      } else {
        // Not a parameter, so we use the slot value instead.  This might be
        // either a parameter that's been passed in or a fixed value, if we're
        // at the top-level (instruction action code).
        auto x = _slots->getSlot(**iter);
        r = x.first;
      }
      if (!r) {
        // If not found, then this could be due to the fact that an instruction
        // calls a helper in such a way that this resource is not used,
        // e.g. we've propagated constants and evaluated conditionals and the
        // result is that we never use the item.  In that case, substitute in a
        // dummy value of -1.
        r = &dummy;
      }
      if (r->_sname) {
        slot_args.Append(r->_sname);
      } else {
        slot_args.Append(Ptree::Make("%d",r->_start));
      }
    }

    return Ptree::Make("%p ( %p %s %p )",fun,slot_args.All(),
                       ((num_args && slot_args.Number()) ? "," : ""),final_args);
  }

	
  // **************************************************************************
  Ptree *InstrWalker::genMicroInstrsRun(Opencxx::Ptree *exp) 
  {
    bool changed = false;
    PtreeArray array;
    populateArgs(array,this,exp,0,changed);
    
    gc_string instr_table_name;
    gc_string instr_field_name;
    
    if (!_core) { RError("The core is invalid."); }
    if (!_instr_item) { RError("The instruction is invalid."); }
    
    Ptree *p0 = Ptree::Make("\n{");
    
    for (unsigned i = 0; i < numArgs(array); ++i) {  
        Ptree *arg = getArg(array,i);

        const InstrField *instrfield = is_instrfield(arg);
        instr_field_name = instrfield->name();
        instr_table_name = instrfield->instrtablename();
        const InstrTable *instr_table = _core->get_instrtable(instr_table_name);
        unsigned instr_table_width = instr_table->getWidth();
        unsigned num = instr_table_width/32;  // The table width is round to 32-bits words, safe to /32.
        unsigned max_num = _core->instr_width()/32; // The max table width is round to 32-bits words, safe to /32.

        Ptree *p1, *p2;
        if (_core->instrEndianness() == Core::Little) 
        { //Little endianess for instruction
            p1 = Ptree::Make("%p\n{  fill_in_instr_buf(%s, %s, %d, %d, true);", 
                               p0, instr_field_name.c_str(), _iname.c_str(), num, max_num);
        } 
        else 
        { //Big endianess for instruction
            p1 = Ptree::Make("%p\n{  fill_in_instr_buf(%s, %s, %d, %d, false);", 
                               p0, instr_field_name.c_str(), _iname.c_str(), num, max_num);
        }

				p2 = Ptree::Make("%p\n micro_instr_run(%s, false); }", 
												   p1, instr_table_name.c_str());
                                 
        p0 = p2;
    } 
    
    return Ptree::Make("%p\n}", p0);
  }
  // ##########################################################################	
	
  
  // **************************************************************************
  Ptree *InstrWalker::genMicroInstrsRunCommit(Opencxx::Ptree *exp) 
  {
    bool changed = false;
    PtreeArray array;
    populateArgs(array,this,exp,0,changed);
    
    gc_string instr_table_name;
    gc_string instr_field_name;
    
    if (!_core) { RError("The core is invalid."); }
    if (!_instr_item) { RError("The instruction is invalid."); }
    
    Ptree *p0 = Ptree::Make("\n{");
    
    for (unsigned i = 0; i < numArgs(array); ++i) {  
        Ptree *arg = getArg(array,i);

        const InstrField *instrfield = is_instrfield(arg);
        instr_field_name = instrfield->name();
        instr_table_name = instrfield->instrtablename();
        const InstrTable *instr_table = _core->get_instrtable(instr_table_name);
        unsigned instr_table_width = instr_table->getWidth();
        unsigned num = instr_table_width/32;  // The table width is round to 32-bits words, safe to /32.
        unsigned max_num = _core->instr_width()/32; // The max table width is round to 32-bits words, safe to /32.

        Ptree *p1, *p2;
        if (_core->instrEndianness() == Core::Little) 
        { //Little endianess for instruction
            p1 = Ptree::Make("%p\n{  fill_in_instr_buf(%s, %s, %d, %d, true);", 
                               p0, instr_field_name.c_str(), _iname.c_str(), num, max_num);
        } 
        else 
        { //Big endianess for instruction
            p1 = Ptree::Make("%p\n{  fill_in_instr_buf(%s, %s, %d, %d, false);", 
                               p0, instr_field_name.c_str(), _iname.c_str(), num, max_num);
        }

        if (i < numArgs(array) - 1) 
        { // Not the last micro instruction
					p2 = Ptree::Make("%p\n micro_instr_run(%s, false); }", 
														 p1, instr_table_name.c_str());
        } 
        else 
        { // The last micro instruction
					p2 = Ptree::Make("%p\n micro_instr_run(%s, true); }", 
														 p1, instr_table_name.c_str());
        }				
                                 
        p0 = p2;
    } 
    
    return Ptree::Make("%p\n}", p0);
  }
  // ##########################################################################


  
  Ptree *InstrWalker::handleVariable(Ptree *var,TypeInfo *type,Ptree *ctx_el)
  {
    if (!type) {
      if (is_cur_cache(var)) {
        return Ptree::Make("%s",_cur_cache.c_str());
      } else if (is_prefix_info(var)) {
        return Ptree::Make("read_%p()",var);
      } else if (const InstrField *f = is_instrfield(var)) {
        // If we have an instruction field, then make it a bit-slice object
        // so that we can take sized-slices.
        // Also- re-name it if it's owned by a sub-instruction.
        gc_string sfx;
        int f_ix;
        if (_instr_item && ((f_ix = _instr_item->getField(var->ToString())) >= 0)) {
          if (_instr_item->getField(f_ix)._subinstr) {
            sfx = "_" + _instr_item->getField(f_ix)._subinstr->name();
          }
        }
        return make_bitslice(Ptree::Make("%p%s",rename(var),sfx.c_str()),f->width());
      } else if (is_cur_reg_enumerated(var)) {
        return Ptree::Make("%s_t::%p",_cur_reg.c_str(),var);
      } else if (const Parm *parm = is_parm(var)) {
        if (_translate_parms != ptNone) {
          // We're in parm translation mode, so translate to the appropriate
          // symbol, creating a new one if necessary.
          if (!_parm_trans) {
            _parm_trans = new ParmTrans;
          }
          Ptree *sym = ((*_parm_trans)[parm->name()] = PItem(AdlGenSym(),parm))._sym;
          gc_string ptype = getParmType(*parm);
          // We insert a cast so that all parm parameters can just have a type
          // of 'int'.  If we're just recording, then the symbol is a dummy- we
          // just return the original variable.
          return (_translate_parms == ptTrans) ? Ptree::Make("((%s)%p)",ptype.c_str(),sym) : var;
        }
      }
    }
    return var;
  }

  Ptree *InstrWalker::TranslateVariable(Ptree *exp)
  {
    using namespace PtreeUtil;

    //cerr << "Examining variable:  " << exp << endl;

    Environment *e = env;
    Bind *b = 0;

    // Handle namespace functions and methods here.
    if (isFunction(exp)) {
      return exp;
    }

    // This could be a template- check for that.
    if (!exp->IsLeaf()) {
      Ptree *x = 0;
      if (Match(exp,"[%? [< %* >]]",&x)) {
        return exp;
      }
      Ptree *r = 0;
      if (Match(exp,"[%? :: %?]",&r,&x)) {
        if (is_enumerated(r,x)) {
          return Ptree::Make("%p_t::%p",r,x);
        }
      }
    }
   

    // Search through all scopes for the variable.
    while (e) {
      if (!e->LookupTop(exp,b)) {
        e = e->GetOuterEnvironment();
      } else {
        break;
      }
    }
            
    if (!b) {
      return handleVariable(exp,0,0);
    } else {
      TypeInfo type;
      b->GetType(type,env);
      return handleVariable(exp,&type,0);
    }
  }

  // Assignments need to be processed because a lot of them need to be transformed
  // into function calls to specific write functions.  This is the case with
  // memory and registers.  We convert Mem() to mem_write and move the rhs into
  // being the second argument of the function.  For registers, writes are converted
  // to calls to <reg name>_write().
  Ptree *InstrWalker::TranslateAssign(Ptree *exp)
  {
    
    Ptree *left = First(exp);

    Ptree* right = Third(exp);
    Ptree* right2 = Translate(right);

    // Before we translate, catch any assignments to registers or register files.
    // Otherwise, those references will be translated as reads, which is something
    // that we don't want.
    Ptree *func;

    if (Match(left," [ %? ( %* ) ] ",&func)) {
      Ptree *args2 = Second(TranslateArguments(left->Cdr()));
      if ( const MmuLookup *lookup = is_mmulookup(func)) {
        return genMmuLookupWrite(lookup,args2,right2);
      }
    }
    else if (const Parm *parm = is_parm(left)) {
      if (_parm_assigns.count(parm->name())) {
        // If we're replacing parms with assignment functions, do the translation
        // here.
        gc_string f = getParmAssign(*parm);
        return Ptree::Make("%s(%p)",f.c_str(),right2);
      }
    }

    return RtlWalker::TranslateAssign(exp);

  }

  // Here we handle fields, register files, and memory, Zero, and SignExtend.
  Ptree* InstrWalker::TranslateFuncall(Ptree* exp)
  {
    Ptree* fun = exp->Car();
    Ptree *reg, *field, *index, *method;

    gc_string fname = extractBaseFuncName(fun);
    Ptree *args  = exp->Cdr();

    if (fname == "getCurrentInstr") {
      _curr_instr_accessed = true;
    }

    if (const SlotAlloc *sa = is_slot_func(fname)) {
      if (_slots ) {        
        // If we have a transactional ISS, then we need to add the instruction
        // packet to the call and any additional parameters used for handling
        // dynamic slots.
        //
        // Note: Very important!  We're purposely passing in args, rather than
        // args2, because we're looking things up in a SlotAlloc object, based
        // upon the original value.  We don't want to confuse things, e.g. if
        // you have a slice access, the SlotAccess is keyed to the original
        // slice access syntax, not the transformed text.
        //       
        return genHelperCall(fname,fun,sa,Second(args),translated_arglist(args));
      }
    }
    else if (Eq(fun,Cia)) {
      // This only occurs in alias expressions, but we just handle it here
      // regardless, since misuse should have been caught by the front-end.
      // Basically, this just translates into a read of the core's cia register.
      if (_core) {
        return Ptree::Make("%s_read()",_core->cia()->name().c_str());
      }
    }
    else if (Eq(fun,UpdateReg)) {
      // Only found within concurrent register/registe-file hooks.
      Ptree *args2 = TranslateArguments(args);
      return Ptree::Make("%s%p",getUpdateRegFunc(_cur_reg).c_str(),args2);
    } else if (Eq(fun,"concat")) {
      // We track the resulting bit-width of the result and we split up this
      // operation, since we only support a finite number of arguments in intbv.
      return genConcat(exp,TranslateArguments(args),args);
    } else if (Eq(fun,"zero")) {
      // Convert to the templated function form.
      Ptree *arg1 = First(translated_arglist(args));
      return make_bitslice(Ptree::Make("zero<%p>()",arg1),get_uint(arg1,"zero-function return size",env));
    } else if (Eq(fun,"instrHasAttr")) {
      // Convert argument to form used for instruction attributes.  We prepend some stuff to avoid name collisions.
      Ptree *arg1 = First(translated_arglist(args));
      Ptree *arg2 = Third(translated_arglist(args));
      if (arg2) {
        return Ptree::Make("instrHasAttr(%p,%s)",arg1,getInstrAttrName(get_str(arg2,"instrHasAttr argument.")).c_str());
      } else {
        return Ptree::Make("instrHasAttr(%s)",getInstrAttrName(get_str(arg1,"instrHasAttr argument.")).c_str());
      }
    }
    else if (Match(fun,"[ %? . %? ]",&reg,&method)) {
      if (Eq(method,"hasAttr")) {
        // Convert argument to form used for register-file attributes.
        Ptree *args2 = translated_arglist(args);
        Ptree *arg1 = First(args2);
        Ptree *arg2 = Third(args2);
        return Ptree::Make("%p() . hasAttr(%p,%s)",reg,arg1,getRegFileAttrName(get_str(arg2,"hasAttr argument.")).c_str());        
      }
    }
    else if (Eq(fun,"skip_instruction")) {
      _has_skip = true;
      if (_skip_commit) { // skip_commit -  when skip_instr is in post_fetch hook
        return Ptree::Make("{ _skip_instruction_commit = true; }");
      } else {
        return Ptree::Make("{ goto skip_instruction_target; }");
      }
    }  
    else if (Eq(fun,"info")) {
      return genMsgCode(Info,exp);
    } 
    else if (Eq(fun,"warning")) {
      return genMsgCode(Warning,exp);
    } 
    else if (Eq(fun,"error")) {
      return genMsgCode(Error,exp);
    } 
    else if (Match(fun," [ [ %? ( %? ) ] . %? ] ",&reg,&index,&field)) {
      if (const InstrField *encap = is_instrfield(reg)) {
        if (const InstrField *nested = encap->getField(field->ToString())) {
          if (nested->is_constrained()) {
            gc_string tbl = makeInstrFldTblAccessName(*nested,encap);
            return Ptree::Make("%s(get_%p__%p(%p),%p)",tbl.c_str(),reg,field,reg,translated_arglist(args));
          }
        }
      }
    }
    else if (const InstrField *inf = is_instrfield(fun)) { 
      if(inf->is_constrained()) {
        Ptree *arg1 = First(translated_arglist(args));
        return Ptree::Make("%s(%p.uint32(),%p)",makeInstrFldTblAccessName(*inf).c_str(),fun,arg1);
      } else if (inf->is_prefix() && inf->index()) {
        // Must be 1-arity function
        if (get_func_call_numargs(exp) != 1) {
          RError("Indexed prefix field must take one integer parameter.");
        } else {
          return Ptree::Make("get_%p(%p)",fun,translated_arglist(args));
        }
      }
    }
    else if (Eq(fun,"run")) {  // Expand run() function
        return genMicroInstrsRun(exp);
    }   
    else if (Eq(fun,"run_commit")) {  // Expand run_commit() function
        return genMicroInstrsRunCommit(exp);
    } 		
    else {
      if(is_halt_oper(fun)) {
        _has_halt = true;
      }
    }

    // Nothing special, so go to default action.
    return RtlWalker::TranslateFuncall(exp);
  }

  // If the lhs is a register or register file and the rhs is a field, then we
  // convert to a call to the appropriate field-read function.  If the lhs is a
  // context object with an element access, then the right must be an element of
  // the context (a register or register-file access).
  Ptree* InstrWalker::TranslateDotMember(Ptree* exp)
  {
    Ptree* left = exp->Car();
    Ptree* member = Third(exp);
    
    if (const InstrField *field = is_instrfield(left)) {
      // Check member for a nested field.
      if (field->getField(member->ToString())) {
        return Ptree::Make("get_%p__%p(%p)",left,member,left);
      }
    }

    return RtlWalker::TranslateDotMember(exp);
  }

  RegAccess InstrWalker::translateRegAccess(const RegAccess &ra)
  {
    RegAccess ra2;

    ra2 = ra;

    // Use the alternate index if we have one and this is a this-reg reference.
    ra2._index   = (ra.is_thisreg() && _alt_thisreg_index) ? _alt_thisreg_index :  Translate(ra._index);
    ra2._findex  = Translate(ra._findex);
    ra2._left    = Translate(ra._left);
    ra2._right   = Translate(ra._right);
    ra2._context = Translate(ra._context);
    ra2._delay   = Translate(ra._delay);

    return ra2;
  }

  Ptree *InstrWalker::TranslateSubInstrCall(PtreeSubInstrCall *exp)
  {
    Ptree *funcall = exp->Car();
    Ptree *args    = (funcall && funcall->IsA(ntFuncallExpr)) ? funcall->Cdr() : 0;
    Ptree *fields  = genArgsTree(exp->subinstr(),exp->subinstrgrp());

    return Ptree::Make("%s_action(%p %s %p)",exp->subinstr().name().c_str(),
                       (args) ? translated_arglist(args) : 0,
                       (fields) ? "," : "",
                       fields);
  }

  // The data model has already discovered all register writes can put them into
  // easy-to-access containers, so we just directly call our generation routines
  // here.
  Ptree *InstrWalker::TranslateRegWrite(PtreeRegWrite *x)
  {
    RegAccess ra = translateRegAccess(x->_regaccess);

    Ptree *expr = Translate(x->get_expr());

    if (_reg_record) {
      _regs.push_back(ra.rb());
    }

    Ptree *result = 0;
    if (ra.is_reg()) {
      if (ra.is_slice()) {
        result = genRegSliceWrite(ra._reg,ra._left,ra._right,expr,ra._context,ra._delay,ra._direct);
      } else if (ra.has_field()) {
        result = genRegFieldAccess(ra._reg,ra._field,ra._findex,expr,ra._context,ra._delay,ra._direct);
      } else {
        result = genRegAccess(ra._reg,expr,ra._context,ra._delay,ra._direct);
      }
    } else if (ra.is_regfile()) {
      if (ra.is_slice()) {
        result = genRegFileSliceWrite(ra._regfile,ra._index,ra._rindex,ra._left,ra._right,expr,ra._context,ra._delay,ra._direct);
      } else if (ra.has_field()) {
        result = genRegFileFieldAccess(ra._regfile,ra._field,ra._index,ra._rindex,ra._findex,expr,ra._context,ra._delay,ra._direct);
      } else {
        result = genRegFileAccess(ra._regfile,ra._index,ra._rindex,expr,ra._context,ra._delay,ra._direct);
      }
    } else {
      RError("Unknown register access:  " << x);
    }
    return result;
  }

  // Same as above, for register reads.
  Ptree *InstrWalker::TranslateRegRead(PtreeRegRead *x)
  {
    RegAccess ra = translateRegAccess(x->_regaccess);

    if (_reg_record) {
      _regs.push_back(ra.rb());
    }

    Ptree *result = 0;
    if (ra.is_reg()) {
      if (ra.is_slice()) {
        result = genSliceRead(genRegAccess(ra._reg,0,ra._context,ra._delay,ra._direct,ra._left,ra._right),ra._left,ra._right);
      } else if (ra.has_field()) {
        result = genRegFieldAccess(ra._reg,ra._field,ra._findex,0,ra._context,ra._delay,ra._direct);
      } else {
        result = genRegAccess(ra._reg,0,ra._context,ra._delay,ra._direct);
      }
    } else if (ra.is_regfile()) {
      if (ra.is_slice()) {
        result = genSliceRead(genRegFileAccess(ra._regfile,ra._index,ra._rindex,0,ra._context,ra._delay,ra._direct,ra._left,ra._right),ra._left,ra._right);
      } else if (ra.has_field()) {
        result = genRegFileFieldAccess(ra._regfile,ra._field,ra._index,ra._rindex,ra._findex,0,ra._context,ra._delay,ra._direct);
      } else {
        if (!ra._index) {
          // If there's no index, then we just return a simple reference to the register-file object.
          result = Ptree::Make("%s()",ra._regfile->name().c_str());
        } else {
          result = genRegFileAccess(ra._regfile,ra._index,ra._rindex,0,ra._context,ra._delay,ra._direct);
        }
      }
    } else {
      RError("Unknown register access:  " << x);
    }
    return result;
  }

  // The data model has already discovered all memory writes can put them into
  // easy-to-access containers, so we just directly call our generation routines
  // here.
  Ptree *InstrWalker::TranslateMemWrite(PtreeMemWrite *x)
  {
    return genMemoryAccess(x,x->_expr);
  }

  // Same as above, for register reads.
  Ptree *InstrWalker::TranslateMemRead(PtreeMemRead *x)
  {
    return genMemoryAccess(x,0);
  }

  // This is here just so that we can signal that we're translating a function's argument declaration.
  Ptree* InstrWalker::TranslateFunctionImplementation(Ptree *exp)
  {
    _translating_func_args = true;
    Ptree *r = RtlWalker::TranslateFunctionImplementation(exp);
    return r;
  }

  Ptree* InstrWalker::TranslateDeclarator(bool record, PtreeDeclarator *decl)
  {
    Ptree *decl2 = RtlWalker::TranslateDeclarator(record,decl);
    if (_translating_func_args) {
      if ( _slots ) {
        Ptree *func = First((Ptree*)decl);
        Ptree *args = Third((Ptree*)decl);

        // If the user has used the old C-style approach of using "void" to mean
        // no arguments then simply eliminate the argument here.
        if (Eq(Ca_ar(args),"void")) {
          args = 0;
        }
        
        // Generate the parameters for storing source/target resources.
        Ptree *extra_args = genHelperArgs();
        Ptree *tmp = Ptree::Make("%p ( %p %s %p )",func,extra_args,((Length(args) && extra_args) ? "," : ""),args);
        decl2 = tmp;
      }
      _translating_func_args = false;
    }
    return decl2;
  }

  // This adds on a new translation-time conditional element.  If the conditional
  // already exists, then we return a pointer to the existing item.
  TransCond *InstrWalker::add_trans_cond(Ptree *cond)
  {
    // First, search to see if this conditional has already been used.
    if (_trans_conds) {
      ForEach((*_trans_conds),i) {
        if (Equal((*i)->_cond,cond)) {
          return *i;
        }
      }
    }

    // Didn't find it already, so add on a new item.
    auto tc = new TransCond;
    tc->_sym = AdlGenSym();

    if (!_trans_conds) {
      _trans_conds = new TransConds;
    }

    _trans_conds->push_back(tc);

    tc->_cond = cond;

    return tc;
  }

  const InstrBase* InstrWalker::handle_trans_cond(Ptree *cond,Ptree* &cond2)
  {
    auto old_tcm = _trans_cond_mode;
    // Note: We're comparing against the unprocessed conditional, so we don't
    // have to deal with complications like our substitutions of 0 with
    // ConstantZero, etc.
    TransCond *tc = 0;
    ResWalker w(_trans_cond_mode);
    w.Translate(cond);
    if (w.is_fixed()) {
      // This is dependent only upon resources which are known at
      // translation-time, so we can break this up into two instructions, based
      // upon whether the expression is true or false.
      tc = add_trans_cond(cond2);

      // Substitute the actual expression with the flag variable.
      cond2 = tc->_sym;
    } else {
      // Not fixed: We cannot handle fixed conditionals below this level, so turn 
      // off the flag for now.
      _trans_cond_mode = 0;
    }
    return old_tcm;
  }

  Ptree* InstrWalker::TranslateIf(Ptree *s)
  {
    if (_trans_cond_mode) {
      Ptree* cond = PtreeUtil::Third(s);
      Ptree* cond2 = Translate(cond);

      auto old_tcm = handle_trans_cond(cond,cond2);
  
      Ptree* then_part = PtreeUtil::Nth(s,4);
      Ptree* then_part2 = Translate(then_part);

      Ptree* else_part = PtreeUtil::Nth(s,6);
      Ptree* else_part2 = Translate(else_part);

      // Restore the flag.
      _trans_cond_mode = old_tcm;

      if(cond == cond2 && then_part == then_part2 && else_part == else_part2)
        return s;
      else{
        Ptree* rest = PtreeUtil::ShallowSubst(cond2, cond, then_part2, then_part,
                                              else_part2, else_part, s->Cdr());
        return new PtreeIfStatement(s->Car(), rest);
      }

    } else {
      return RtlWalker::TranslateIf(s);
    }
  }

  Ptree* InstrWalker::TranslateCond(Ptree* exp)
  {
    // Note: We don't do this processing if we're handling a var statement, where
    // this is going to be the new lhs of the var decl.
    if (_trans_cond_mode && !_translating_var_decl) {
      Ptree* c = PtreeUtil::First(exp);
      Ptree* c2 = Translate(c);

      auto old_tcm = handle_trans_cond(c,c2);

      Ptree *exp2 = translate_ternary(exp,c,c2);

      // Restore the flag.
      _trans_cond_mode = old_tcm;

      return exp2;
    } else {
      return RtlWalker::TranslateCond(exp);
    }
  }


  // These are here just to turn off translation-time conditional processing for
  // anything that they contain.

  Ptree* InstrWalker::TranslateFor(Ptree *exp)
  {
    auto old_tcm = _trans_cond_mode;
    _trans_cond_mode = 0;
    Ptree *r = RtlWalker::TranslateFor(exp);
    _trans_cond_mode = old_tcm;
    return r;
  }

  Ptree* InstrWalker::TranslateSwitch(Ptree *exp)
  {
    auto old_tcm = _trans_cond_mode;
    _trans_cond_mode = 0;
    Ptree *r = RtlWalker::TranslateSwitch(exp);
    _trans_cond_mode = old_tcm;
    return r;
  }

  Ptree* InstrWalker::TranslateWhile(Ptree *exp)
  {
    auto old_tcm = _trans_cond_mode;
    _trans_cond_mode = 0;
    Ptree *r = RtlWalker::TranslateWhile(exp);
    _trans_cond_mode = old_tcm;
    return r;
  }

  Ptree* InstrWalker::TranslateDo(Ptree *exp)
  {
    auto old_tcm = _trans_cond_mode;
    _trans_cond_mode = 0;
    Ptree *r = RtlWalker::TranslateDo(exp);
    _trans_cond_mode = old_tcm;
    return r;
  }

  Ptree* InstrWalker::TranslateStmts(Ptree* block)
  {
    Ptree *block2 = AdlTranslatingWalker::TranslateBrace(block);
    if (block != block2) {
      return new PtreeStmts(block2->Car(),block2->Cdr());
    } else {
      return block2;
    }
  }

}

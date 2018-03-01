//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Decode table classes.
//

#include <iostream>
#include <iomanip>
#include <sstream>
#include <cassert>
#include <limits>

#include "helpers/fe_exceptions.h"
#include "helpers/Macros.h"
#include "helpers/BitTwiddles.h"
#include "Data.h"
#include "InstrTable.h"
#include "MsgLog.h"
#include "Helpers.h"

using namespace std;
using namespace rnumber;

namespace adl {

#define DError(x) { ostringstream ss; ss << x; throw decode_error(ss.str()); }

  //#define DStrictErrorr(x) { ostringstream ss; ss << x; throw decode_error(ss.str()); }
#define DStrictError(x)

  //#undef Verbose
#define Verbose 2

#ifdef Verbose
#  define V1(x) { if (Verbose >= 1) { cerr << x; } }
#else
#  define V1(x)
#endif

  typedef vector<RNumber> RNumberVect;
  struct OpcField {
    RNumber _mask; 
    int   _shift;
    RNumber _value;
    unsigned _width;

    OpcField(const RNumber& m,int s,const RNumber& v, unsigned width) : _mask(m), _shift(s), _value(v),_width(width) {
    };
    
    OpcField(unsigned start,unsigned end,const RNumber& v, unsigned max_width) {
      RNumber ones(0xFFFFFFFFU,max_width);
      unsigned width = end - start + 1;
      unsigned shift =  max_width - end - 1;
      RNumber  mask  = (ones&(ones << (32 - width))) >> (32 - width) << shift;
      _mask.resize(mask.size());
      _mask = mask;
      _shift = shift;
      _value = v;
      _width = max_width;
    };
    
    RNumber make_key() const {
      RNumber value(0,_width);
      value = (_value << _shift);
      return value;
      
    };

  };
  // Forward declaration of helpers
  unsigned get_shift(const RNumber& mask);
  unsigned get_norm(const RNumber& mask);
  RNumberVect get_ones_groups(const RNumber& mask);

  // Used for storing a possible encoding conflict, for error reporting purposes.
  static const Instr *conflict = 0;

  string conflict_msg()
  {
    string s;
    if (conflict) {
      MkStr(s,"A possible conflict is with instruction " << conflict->name() << ".");
    }
    return s;
  }

  ostream &operator<<(ostream &o,const OpcField &x)
  {
    //o << hex << x._value << "(" << dec << x._shift << "," << x._width << hex << ")" <<" (0x" << x.make_key() << ")";
    o << hex << x._mask << "(" << x.make_key() << ")";
    return o;
  }
  
  // true if mask is nonzero in exactly one word
  bool legal_mask(const RNumber& mask) {
    return !((mask >> ((32*(get_shift(mask)/32)) + 32)) > 0);
  }

  struct OpcFields : public gc_list<OpcField> {};

  OpcFields InstrTable::get_opcfields(const Instr &instr)
  {
    OpcFields fields;
    // Add on opcodes.
    for (unsigned i = 0; i != instr.sizeOpcs(); ++i) {
      Instr::Field ifp = instr.getOpc(i);
      V1("Got field: (" << ifp._name << ", 0x" << hex << ifp._value << ", " << ifp._field->mask() << ")" << endl);
      
      // Need to translate to big endian, currently we work with highbit, because some
      // prefix instructions are 20 bit , so it get rounded to 24 and leads to errors in the
      // instruction table. We have to do something with this later.
      unsigned shift = instr.is_mixed_endian() ? 0 : _width - instr.width();
      auto flds = split_field(ifp,shift);
      // reverse - want to put leftmost first
      RevForEach(flds,j) {
        fields.push_back(*j);      
      }
    }
    return fields;
  }  

  // Checks that field entirely fits into one word, if no
  // returns cutted fields
  OpcFields InstrTable::split_field (const Instr::Field& ifp, unsigned shift) 
  {
    OpcFields fields;

    const InstrField& field =  (*ifp._field);
    RNumber ones(0xFFFFFFFFU,_width);
    uint64_t value = ifp._value;

    RevForEach(field.bits(),i) {
      unsigned start = i->lo(_width) - shift;
      unsigned end   = i->hi(_width) - shift;
      // If they are not on the same word then we have to cut
      unsigned tmp_width = 0;
      if ((start/32 + 1) == end/32) {
        // right part
        tmp_width = end - (start/32+32) + 1;
        RNumber tmp_val   = (value & (ones >> (32 - tmp_width))) ; 		
        value >>= tmp_width;	
        fields.push_back(OpcField(start/32+32,end,tmp_val,_width)); 

        // left part
        tmp_width = (start/32 + 31) - start + 1;
        tmp_val   = (value & (ones >> (32 - tmp_width))) ; 		
        value >>= tmp_width;	
        fields.push_back(OpcField(start,start/32+31,tmp_val,_width));
  
      } else if ((start/32 + 2) == end/32) {
 	
        //right part
        tmp_width = end - (start/32+64) + 1;
        RNumber   tmp_val   = (value & (ones >> (32 - tmp_width))) ; 		
        value >>= tmp_width;	
        fields.push_back(OpcField(start/32+64,end,tmp_val,_width));      
        
        // middle part
        tmp_width = 32;
        tmp_val   = (value & (ones >> (32 - tmp_width))) ; 		
        value >>= tmp_width;	
        fields.push_back(OpcField(start/32+32,start/32+64,tmp_val,_width));         
		
        // left part
        tmp_width = (start/32 + 31) - start + 1;
        tmp_val   = (value & (ones >> (32 - tmp_width))) ; 		
        value >>= tmp_width;	
        fields.push_back(OpcField(start,start/32+31,tmp_val,_width));      
      
      } else {
        RNumber  tmp_val   = (value & (ones >> (32 - i->width()))) ; 		
        value >>= i->width();
        fields.push_back(OpcField(start,end,tmp_val,_width));       
      }         
    }
    return fields;
  } 

  std::ostream &operator<< (std::ostream &o,const InstrTable &t)
  {
    t.dump(o,"  ");
    return o;
  }

  InstrTable::InstrTable() : 
    _width(0), 
    _word(0),
    _mask(0),    
    _default_table(0),
    _default_conflict(false),
    _instr(0),
    _embedded(false), // By default, the instruction table is not for micro instructions
    _max_instr_width(0) // 
  {}

  InstrTable::InstrTable(unsigned width) : 
    _width(width),
    _mask(0,width), 
    _default_table(0),
    _default_conflict(false),
    _instr(0),
    _embedded(false), // By default, the instruction table is not for micro instructions
    _max_instr_width(width) //
  {}

  void InstrTable::set_embedded() { // Set the instruction table for micro instructions RECURSIVELY
    if (empty()) {
      _embedded = true;
    } else {
      ForEach(_table,i) {
        i->second->set_embedded();
      }
    }
  }

  void InstrTable::clear()
  {
    _mask = 0;
    _word = 0;
    _shift = 0;
    _default_table = 0;
    _default_conflict = false;
    _instr = 0;
    _table.clear();
  }

  void add_to_mask(RNumber &ret,const InstrField &field,unsigned width)
  {
    ForEach(field.bits(),b) {
      RNumber ones(0,width);
      ones.invert();
      unsigned l = b->lo(width);
      unsigned r = width - b->hi(width) - 1;
      ones = (ones << l) >> l;
      ones = (ones >> r) << r;
      ret |= ones;       
    }
  }

  // Return instruction mask, if opc_only - mask of opcodes, otherwise all defined bits.
  RNumber get_instr_mask(const Instr *instr, unsigned width, bool opc_only) 
  {
    RNumber ret(0,width);
    if (instr->shorthand()) {
      auto alias = instr->get_alias();
      ret = get_instr_mask(alias->_target,width,opc_only);
      ForEach(alias->_fields,i) {
        if (i->_type == Instr::Field::Value) {
          RNumber tmp(i->_field->setValue((uint64_t)-1,width),width);
          if (instr->is_mixed_endian()) {
            tmp <<= (width - instr->width());
          }
          ret |= instr->adjustForLittleEndian(tmp,width,true);
        }
      }
      return ret;
    } else {
      unsigned sz = opc_only ? instr->sizeOpcs() : instr->sizeAll();
      for (unsigned i = 0; i != sz; ++i) {
        Instr::Field ifp = opc_only ? instr->getOpc(i) : instr->getAll(i);
        add_to_mask(ret,*(ifp._field),width);
      }
      return instr->adjustForLittleEndian(ret,width,true);
    }
  }
  
  // Return mask of undefined bits.
  RNumber get_undefined_mask(const Instr *instr, unsigned width) 
  {
    return ~(get_instr_mask(instr,width,false));
  }   

  // Return opcode mask.
  RNumber get_opc_mask(const Instr *instr, unsigned width) 
  {
    return get_instr_mask(instr,width,true);
  }

  // Return a mask representing any fields marked as 'allow_conflict=true'.  If
  // a shorthand, the user may have set a value.  We still want to treat it as a
  // conflict, though, so that as we insert these into the tree, we follow the
  // correct route.  Thus, for conflicts, we look at the target's fields.
  RNumber get_conflict_mask(const Instr *instr, unsigned width) 
  {
    RNumber ret(0,width);
    if (instr->shorthand()) {
      instr = instr->get_alias()->_target;
    }
    unsigned sz = instr->sizeFields();
    for (unsigned i = 0; i != sz; ++i) {
      Instr::Field ifp = instr->getField(i);
      if (ifp._field->allow_conflict()) {
        add_to_mask(ret,*(ifp._field),width);
      }
    }
    return instr->adjustForLittleEndian(ret,width,true);
  }

  // Returns a mask of any conflict fields.
  RNumber get_conflict_mask(Instrs &instrs, unsigned enc_width) 
  {
    // First, calculate the common mask.  Also, store the size of the smallest
    // instruction.
    auto sw = numeric_limits<unsigned>::max();
    RNumber mask(0,enc_width);
    ForEach(instrs,i) {
      mask |= get_conflict_mask(*i,enc_width);
      sw = min(sw,(*i)->width());
    }    
    // Now clear all bits after the smallest instruction, since they cannot be
    // common to all instructions.
    if (sw < mask.size()) {
      mask.setField(sw,mask.size()-1,RNumber(0,enc_width));
    }
    return (mask);    
  }

  // For a list of instructions return common opcode bits mask.
  RNumber get_common_opc(Instrs &instrs, unsigned enc_width) 
  {
    RNumber mask(0,enc_width);
    mask.invert();
    ForEach(instrs,i) {
      mask &= get_opc_mask(*i,enc_width);
    }    
    return (mask);    
  }
  
  // Return a list instruction that have opcode equal to value on a given mask.
  Instrs get_common_instrs(RNumber val,RNumber mask,Instrs &instrs) 
  {
    Instrs ret;
    ForEach(instrs,i) {
      RNumber opc = (*i)->getOpcode(mask.size(),true);
      if ((opc & mask) == val && !(*i)->shorthand()) {
        ret.push_back(*i);
      } 
    }
    return ret;
  }
  
  // Return all possible values of opcodes on a given mask
  gc_set<RNumber> get_values(RNumber mask,Instrs &instrs) 
  {
    gc_set<RNumber> ret;
    ForEach(instrs,i) {
      ret.insert(mask&(*i)->getOpcode(mask.size(),true));
    }
    return ret;
  }
  
  // Set mask.
  void InstrTable::set_mask(RNumber mask) 
  {
    if (_mask > 0) {
      DError("Trying to set mask to already set node");
    }
    if (!legal_mask(mask)) {
      DError("Trying to set big mask.");
    }
    if (mask != 0) {
      _mask.resize(_width);
      _mask = mask;
      _shift = get_shift(_mask);
    } 
  }
  
  // Add branch.  
  void InstrTable::add_node(RNumber value) 
  {
    _table.insert(make_pair(_mask&value,new InstrTable(_width)));
  }

  // Report encoding conflict.
  void report_conflict(Instrs &instrs, MsgLog &ml, bool warning = false) 
  {
    ostringstream os;
    // True if found a pair of conflicting instructions.
    bool found_pair = false;
    ForEach(instrs,i) {
      auto j = i;
      ++j;
      while ( j != instrs.end()) {
        int width = max((*i)->width(),(*j)->width());
        auto mi = get_opc_mask(*i,width);
        auto mj = get_opc_mask(*j,width);
        auto opci = (*i)->getOpcode(width,true);
        auto opcj = (*j)->getOpcode(width,true);
        auto m = mi&mj;
        if ( m == 0 || (((opci&m) == (opcj&m))  && ((m != mi) || (m !=mj)))) {
          // No common opcode or 
          // the common opcode is the same but one mask is not inside another:
          // it is violation of our rules.
          os << "'" << (*i)->name() << "','" << (*j)->name() << "'.\n";
          found_pair = true;
          break;
        }        
        ++j;
      };
      if (found_pair) {
        break;
      }
    }
    // At this point, if we didn't find a conflict, and we're just in warning
    // mode, then we don't issue an error.  This is because we might be trying to
    // use too small of a mask, so there really isn't a problem and a warning
    // would just confuse the user.
    if (warning && !found_pair) {
      return;
    }
    if (!found_pair) {
      os << "Could not build decision tree based on these instructions:";
      ForEach(instrs,i) {
        os << " '" << (*i)->name() << "'"; 
      }
      os << ".\n";
    }
    if (warning) {
      if (Data::strictMode()) {
        AddWarning(ml,(*instrs.begin())->loc(),
                   "Possible conflict between instructions:  " + os.str());
      }    
    } else {	
      DError("Conflict between instructions:  " + os.str());
    }
  }
  
  // Check encoding overlap:
  // Large instruction is allowed to define values of fields in small instruction, or
  // use its dont-care bits. 
  bool encoding_overlap(Instr *large, Instr *small) 
  {
    if (large == small) {
      return false;
    }
    unsigned width = max(large->width(),small->width());
    ForEach(large->getAllFields(),i) {
      if (i->_type != Instr::Field::Value) {
        continue;
      }
      RNumber field_mask =  (i->_field->setValue((uint64_t)-1,width)) ;
      RNumber undefined = get_undefined_mask(small,width);
      bool same_field = false;
      ForEach(small->getAllFields(),ii) {
        RNumber msk =  (ii->_field->setValue((uint64_t)-1,width)) ;
        if (msk == field_mask) {
          same_field = true;
        }
      }
      bool dont_care  = (field_mask&undefined) == field_mask;
   
      if (!same_field && !dont_care) {
        return true;
      }
    }
    return false;
  }

  // Return maximal opcode mask.
  Instr *max_mask(Instrs &instrs, unsigned width) 
  {
    Instr *ret = NULL;
    RNumber mask(0,width);
    ForEach(instrs,i) {
      RNumber m = get_opc_mask(*i,width);
      if (get_norm(m) >= get_norm(mask)) {
        mask = m;
        ret = (*i);
      }      
    }
    return ret;
  }
  
  // Sort instruction by mask size.
  // It is not fast, usually there are two-three instructions in a group.
  Instrs sort_by_mask(Instrs &instrs,unsigned width) 
  {
    Instrs tail;
    if (instrs.size() <= 1) {
      tail = instrs;
    } else {
      Instr *head = max_mask(instrs,width); 
      ForEach(instrs,i) {
        if (*i != head) {
          tail.push_back(*i);
        }	
      }
      tail = sort_by_mask(tail,width);
      tail.push_front(head);
    }
    return tail;
  }
   
  bool check_overlap(Instrs &instrs,unsigned width,MsgLog &ml,bool signal_error) 
  {
    if (instrs.size() == 1) {
      return true;
    }
    
    ForEach(instrs,i) {
      if (*i != instrs.back()) {
        auto j = i;
        ++j;
        // i should have bigger mask or different common opcode
        auto mi = get_opc_mask(*i,width);
        auto mj = get_opc_mask(*j,width);
        if (((mi|mj) != mi) || (mi == mj)) {
          auto m = mi&mj;

          if (m != mj) {
            // Sort was not good enough (bad instruction group), order is not correct.
            if (signal_error) {
              report_conflict(instrs,ml);
              // Won't get here due to exception...
            }
            return false;
          }

          auto opci = (*i)->getOpcode(width,true);
          auto opcj = (*j)->getOpcode(width,true);
          // Last check: opcode should differ
          if ((m&opci) == (m&opcj)) {
            if (signal_error) {
              Instrs conf;
              conf.push_back(*i);
              conf.push_back(*j);
              report_conflict(conf,ml);
              // Won't get here due to exception...
            }
            return false;
          }
        }
      }
    }
    report_conflict(instrs,ml,true);
    return true;
  }

  // Return nth (from the left) non zero submask that is does not cross word boundary.
  RNumber split_mask(RNumber mask,unsigned word = 0) 
  {
    while (word < mask.size()/32) {   
      int l = 32*word;
      int r = mask.size()-32*(word+1);
      RNumber sub_mask = (((mask << l ) >> l) >> r) << r;
      if (sub_mask > 0) {
        // Found nonzero submask.
        return sub_mask;
      }      
      ++word;
    }
    return mask;
  }

  // For a given set of values and instructions, add the items to the tree and
  // continue building.
  bool InstrTable::add_to_tree(const gc_set<RNumber> &values,Instrs &instrs,const RNumber &mask,
                               const RNumber &used_mask,const RNumber &used_conflict_mask,MsgLog &ml)
  {
    set_mask(mask);        
    ForEach(values,v) {
      add_node(*v);
      auto tmp = get_common_instrs(*v,_mask,instrs);
      if (!_table[*v]->build_tree(tmp,_mask|used_mask,used_conflict_mask,ml)) {
        return false;
      }
    }
    return true;
  }

  void print_stuff(const Instrs &instrs,const RNumber &omask)
  {
    cout << "Opcode mask 0x" << hex << omask << "for:\n";
    ForEach(instrs,i) {
      cout << "  " << (*i)->name() << '\n';
    }
    cout << "\n";
  }

  bool InstrTable::add_conflict_sets(Instrs &normal,Instrs &conflict,bool default_conflict,
                                     const RNumber &cmask,const RNumber &used_mask,
                                     const RNumber &used_conflict_mask,MsgLog &ml)
  {
    // Create a node for this split case.  Conflicts go into the default
    // table.  Everything else is handled as normal.
    auto nvalues = get_values(cmask,normal);
    if (!add_to_tree(nvalues,normal,cmask,used_mask,used_conflict_mask,ml)) {
      return false;
    }
    _default_table = new InstrTable(_width);
    _default_conflict = default_conflict;
    // Note: For the default table, we do not or-in the conflict mask
    // b/c it's not part of the opcode.  This would confuse the next
    // call of build_tree.  However, we *do* record that we've used the
    // conflict mask, so that we don't get caught in an infinite loop if
    // we need to progress to the next word for encoding.
    if (!_default_table->build_tree(conflict,used_mask,used_conflict_mask | cmask,ml)) {
      return false;
    } else {
      return true;
    }
  }
  
  // Try building this node with conflicts.
  bool InstrTable::try_with_conflicts(Instrs &instrs,const RNumber &cmask,const RNumber &used_mask,
                                      const RNumber &used_conflict_mask,MsgLog &ml)
  {
    // There are conflict fields, so use them.  Where the conflict mask
    // corresponds to opcode values, build normal trees.  Put everything else
    // into the default tree and continue to build.  The conflict mask has to
    // cover all bits of the opcode, not just partially overlap, or else we can
    // end up with a situation where we over-consume upcode bits and can't
    // finish creating the tree.
    Instrs normal, conflict;
    ForEach(instrs,i) {
      RNumber opc = get_opc_mask(*i,_width);
      if ( (cmask & opc) == cmask ) {
        normal.push_back(*i);
      } else {
        conflict.push_back(*i);
      }
    }

    return this->add_conflict_sets(normal,conflict,true,cmask,used_mask,used_conflict_mask,ml);
  }

  bool InstrTable::construct_tree_from_instrs(Instrs &instrs,RNumber &mask,const RNumber &used_mask,
                                              const RNumber &used_conflict_mask,bool at_end,MsgLog &ml)
  {
    Instrs tmp = sort_by_mask(instrs,_width);   
    if (!check_overlap(tmp,_width,ml,at_end)) {
      // If we fail here, then try again using the next word.
      ++_word;
      return false;
    }

    // It is OK (otherwise exception is thrown or we have skipped to next
    // word) We start from the smallest mask.
    mask = used_mask^get_opc_mask(tmp.back(),_width);
    mask = split_mask(mask);
    if (mask != 0) {
      RNumber val = mask&tmp.back()->getOpcode(_width,true);
      set_mask(mask);
      add_node(mask&val);
      // Build the new node.  Note: We don't modify the instruction list
      // b/c we might need more mask information from all of these
      // instructions if they span a word.
      _table[mask&val]->build_tree(tmp,_mask|used_mask,used_conflict_mask,ml);
    } else {
      tmp.pop_back();
      build_tree(tmp,used_mask,used_conflict_mask,ml);
    }

    return true;
  }

  // Try to break the opcodes up into two groups, based around a common opcode
  // portion.  So, we'll have a node for the common opcode portion, then we'll
  // split into a fixed portion and a default case.
  bool InstrTable::try_conflict_partitioning(Instrs &instrs,const RNumber &used_mask,const RNumber &used_conflict_mask,MsgLog &ml)
  {
    // Should be the case, b/c otherwise we'd have created a leaf node by this
    // point (check_overlap would never have returned false).
    assert(instrs.size() > 1);
    
    Instrs tmp = sort_by_mask(instrs,_width);

    // Scan through all instructions and find a common mask, plus an instruction which doesn't overlap with the others.
    RNumber common_mask(0,_width), conflict(0,_width);
    common_mask.setAll();
    bool conflicts_allowed = false;
    
    ForEach(instrs,i) {
      if (*i != instrs.back()) {
        auto j = i;
        ++j;

        conflicts_allowed = conflicts_allowed || (*i)->allow_conflict();
        
        auto mi = get_opc_mask(*i,_width);
        auto mj = get_opc_mask(*j,_width);

        common_mask &= mi;
        common_mask &= mj;

        // If we haven't found a conflict yet, check for it.  Keep going in
        // order to generate a complete common-mask.
        if (conflict.iszero()) {
          if (((mi|mj) != mi)) {
            // Found a case where there isn't a perfect overlap, so find the larger.
            if ((mi & ~common_mask) != 0) {
              conflict = mi & ~common_mask;
            } else {
              conflict = mj & ~common_mask;
            }
          }
        }
      }
    }
    
    // If no conflict mask was found, then we probably just have the case of
    // exact-overlapping instructions with no difference in opcode.
    if (!conflicts_allowed || conflict.iszero()) {
      report_conflict(instrs,ml,false);
      return false;
    }

    if (common_mask.iszero()) {
      auto i = instrs.begin();
      auto j = i; ++j;
      DError("No common opcodes between instruction " << (*i)->name() << " and instruction " << (*j)->name() << ".");
      return false;
    }

    // Divide up the instructions between those that match the conflict mask and those that don't.
    Instrs grp1;
    Instrs grp2;

    ForEach(instrs,i) {
      auto mi = get_opc_mask(*i,_width);
      if (!(mi & conflict).iszero()) {
        grp1.push_back(*i);
      } else {
        grp2.push_back(*i);
      }
    }

    // Sanity check- we should have instructions in both groups.
    assert(!grp1.empty() && !grp2.empty());
    
    // We don't want the bits for this node in the next node's mask.
    conflict = conflict & ~common_mask;
    
    // First, set this node to the common bits of the set of instructions.
    RNumber val = common_mask & grp1.back()->getOpcode(_width,true);
    common_mask = split_mask(common_mask,_word);    
    set_mask(common_mask);
    add_node(common_mask&val);
    InstrTable *new_node = _table[common_mask&val];

    // Try across all possible words.
    unsigned w = 0;
    while (w < conflict.size()/32) {

      RNumber cmask = split_mask(conflict,w);
      new_node->_word = w;

      // Add in the new instructions.  The node gets the first set and the
      // node's default-table gets the second set.
      if (new_node->add_conflict_sets(grp1,grp2,false,cmask,used_mask | common_mask,used_conflict_mask | common_mask,ml)) {
        return true;
      }

      ++w;
    }
    return false;
  }
  
  // Build decoding tree. We dont use shorthands for that.
  bool InstrTable::build_tree(Instrs &instrs,const RNumber &used_mask,const RNumber &used_conflict_mask,MsgLog &ml) 
  {
    const RNumber omask = used_mask^get_common_opc(instrs,_width);
    for (int i = 0; i != 2; ++i) {
      // We start with not allowing the use of conflict masks.  If that fails,
      // then we start over, allowing their use.
      bool allow_conflict = i;
      // Cut at word borders.  We first try to encode the first 32-bits.  If that fails,
      // we skip to the next word and try again.
      _word = 0;
      while (_word < omask.size()/32) {
        bool last_word = (_word+1 >= omask.size()/32);
        // We're at the very end (and thus must report an error) if we're at the
        // last encoding word and we're allowing conflict masks.
        bool at_end = last_word && allow_conflict;
        RNumber mask = split_mask(omask,_word);
        if (instrs.size() == 1) {
          // We have only one instruction so just set it as a last node.
          Instr *instr = instrs.front();
          if (mask != 0) {
            RNumber opc = instr->getOpcode(_width,true);
            set_mask(mask);
            add_node(mask&opc);
            auto both_masks = _mask | used_mask;
            auto common = get_common_opc(instrs,_width);
            auto result = both_masks ^ common;
            // Check if we need more than one junction 
            if (result > 0) {
              // Check for a runaway case: We've somehow used more bits than are
              // in the lone instruction's opcode.  Thus, we'll loop infinitely
              // b/c we can never seemingly make progress, since we've already
              // overconsumed.
              assert( (common & result) != 0 );
              _table[mask&opc]->build_tree(instrs,_mask|used_mask,used_conflict_mask,ml);
            }
          }
          return true;
        } else {
          auto values = get_values(mask,instrs);
          if (values.size() == 1) {
            // We have many instructions with one common opcode value.
            // So we need to check for legality of such a situation. 

            if (allow_conflict) {
              // Do we have conflict fields?  This is where the user knows that a
              // conflict will arise, but allows a default case, e.g. one
              // instruction will have a fixed 0 value, another has a field which
              // will have a 1 in conflict situations.
              RNumber cmask = get_conflict_mask(instrs,_width) & ~used_conflict_mask;
              if (!cmask.iszero()) {
                // Manually specified conflicts.
                return try_with_conflicts(instrs,cmask,used_mask,used_conflict_mask,ml);
              }

              // No conflict mask, so attempt to partition into two groups.
              // This assumes that we have masks which don't perfectly overlap.
              if (try_conflict_partitioning(instrs,used_mask,used_conflict_mask,ml)) {
                return true;
              }
            }

            if (!construct_tree_from_instrs(instrs,mask,used_mask,used_conflict_mask,at_end,ml)) {
              continue;
            }
            
          } else {
            return add_to_tree(values,instrs,mask,used_mask,used_conflict_mask,ml);
          }
          return true;      
        }
        // We are done.
        return true;
      }
    }
    return false;
  }

  // Add a list of instructions
  void InstrTable::add_instrs_internal(Instrs &instrs) 
  {
    ForEach(instrs,i) {
      RNumber m(0,_width);
      add_instr_internal(**i,m); 
    } 
  }

  // Add a single instruction, used_bits keeps bits that were already checked by previous nodes.
  void InstrTable::add_instr_internal(Instr &instr, RNumber used_bits) 
  {
    RNumber opc  = instr.getOpcode(_width,true);
    RNumber mask = get_opc_mask(&instr,_width);
    if (mask == 0) {
      DError("Instruction '" << instr.name() << "' has no opcode fields.");
      return;
    }

    if (_table.empty()) {
      if (_instr) {
        // This must be shorhand.  We add if it has a unique mapping, i.e. it
        // has a 1-1 mapping of alias to target and isn't a macro (shorthand
        // maps to a sequence of instructions).
        if (instr.shorthand()) {
          if (instr.get_aliases().size() == 1) {
            _shorthands.push_back(&instr);
          }
        } else {
          DError(instr.name() << "Encountered a leaf-node in table already occupied by an instruction: " << _instr->name() << " (existing item).");
        }
      } else {
        // Usual instructions
        _instr = &instr;
        _instr->_shorthands = &_shorthands;
      }
    } else {
      if (_instr && instr.shorthand()) {
        if (instr.get_alias()->_target == _instr) {
          _shorthands.push_back(&instr);       
        }
      } else {
        if ((_mask&mask) != 0) {
          if (_default_table && ((_mask & get_conflict_mask(&instr,_width)) != 0)) {
            // This is the special conflict-case where we use the default table.
            // This situation arises when shorthands, which have a conflict
            // value set, are inserted.  We want to insert it with the real
            // instruction and not get confused due to a hardcoding of the
            // field.
            _default_table->add_instr_internal(instr,used_bits);
          } else if (_table.find(_mask&opc) != _table.end()) {
            _table[_mask&opc]->add_instr_internal(instr, used_bits|_mask);       
          } else {
            // TBD: this should be error in the strict mode, but currently we allow this.
            DStrictError("BAD NODE: " << instr.name() << endl);
            add_node(_mask&opc);
            _table[_mask&opc]->add_instr_internal(instr, used_bits|_mask);
          }
        } else if (_default_table && _default_conflict && ((_mask & get_conflict_mask(&instr,_width)) != 0)) {
          // This is the special conflict-case where we use the default table
          // and the conflict is allowed due to a user-defined conflict mask
          // fromt the instruction fields.
          _default_table->add_instr_internal(instr,used_bits);
        } else if (_default_table && !_default_conflict &&
                   (((_default_table->_mask & mask) != 0) ||  // This term checks to see if the instruction's opcode matches with the default table.
                    ((mask & ~used_bits) == 0))) {            // If we've used up all bits, then place it in the default table as the default option.
          // This is the special conflict-case where we use the default table.
          // It arises when there is no common super-set opcode and arises
          // naturally, so we have to check the instruction's opcode mask.
          _default_table->add_instr_internal(instr,used_bits);
        } else {
          _instr = &instr;   
          _instr->_shorthands = &_shorthands;
        }
      }
    }
  } 

  void InstrTable::add_instrs(Instrs &instrs, MsgLog &ml) 
  {

    Instrs instructions;
    Instrs shorthands;

    ForEach(instrs,i) {
      if ( !(*i)->shorthand()) {
        instructions.push_back(*i);
      } else if ( (*i)->get_aliases().size() == 1) {
        // Ignore multi-target shorthands.
        shorthands.push_back(*i);
      }
    }

    if (build_tree(instructions,RNumber(0,_width),RNumber(0,_width),ml)) {
      add_instrs_internal(instructions);
      add_instrs_internal(shorthands);
    }
  }

  InstrTable *InstrTable::getTable(const RNumber& mvalue)
  {
    TableHash::iterator iter = _table.find(mvalue);
    if (iter == _table.end()) {
      // Not found- create a new table.
      auto ip = _table.insert(make_pair(mvalue,new InstrTable(_width)));
      iter = ip.first;
    }  
    return iter->second;
  }

  void InstrTable::get_instrs(InstrList &instrs)
  {
    if (_instr) {
      instrs.push_back(_instr);
    } else {
      ForEach(_table,i) {
        i->second->get_instrs(instrs);
      }
    }
  }

  // Get a mask for the specified bits (inclusive).
  RNumber get_bits(int hi,int lo, unsigned width)
  {
    RNumber mask(0,width);
    mask.invert(); //all ones

    mask = (mask >> lo) << lo;
    mask = (mask << (width - hi - 1)) >> (width - hi - 1);
    return mask;
  }

  RNumberVect get_ones_groups(const RNumber& mask)
  {
    unsigned width = mask.size();
    RNumberVect ones;

    RNumber bit(1,width);     

    bool group = false;
    uint start = 0;
    uint index = 0;

    for ( ; bit != 0; ++index) {
      if ((bit & mask) != 0) {
        // Entering, or in, a group.
        if (!group) {
          group = true;
          start = index;
        }
      } else {
        // Not in, or leaving, a group.
        if (group) {
          // Leaving a group.
          ones.push_back(get_bits(index-1,start,width));
          group = false;
        };
      }
      bit <<= 1;
    }
    // Handle a group that's still in progress.
    if (group) {
      ones.push_back(get_bits(index-1,start,width));
    }
    return ones;
  }

  void report_conflict(Instr &instr,InstrTable &table)
  {
    if (const Instr *ci = table.instr()) {
      DError("Conflict between instruction '" << instr.name() << " and instruction '"
             << ci->name() << "'.");
    } else {
      DError("Conflict between instruction '" << instr.name() << "' and field mask 0x"
             << hex << table.mask() << ".");
    }
  }  

  unsigned get_shift(const RNumber& mask)
  {  
    for (unsigned i = 0 ; i != mask.size() ; ++i){
      if (mask.getBit(mask.size()-i-1)) {
        return i;
      } 
    }
    return 0;
  } 

  unsigned get_norm(const RNumber& mask)
  {
    unsigned ret = 0;
    for (unsigned i = 0 ; i != mask.size() ; ++i){
      if (mask.getBit(mask.size()-i-1)) {
        ret++;
      } 
    }
    return ret;
  } 

  bool InstrTable::is_prefix() const 
  {
    if (_instr) {
      return _instr->is_prefix();
    }

    if (!_table.empty()) {
      return _table.begin()->second->is_prefix();      
    }

    return false;
  }

  void InstrTable::merge() 
  {

    InstrTable* next =_table.begin()->second;
    // Recompute new mask.
    _mask  = _mask | next->_mask;
    // Recompute shift.
    _shift = get_shift(_mask);   

    RNumber cur_value = _table.begin()->first;
    TableHash new_hash;
    // merge all itables
    ForEach(_table,t) {
      ForEach(t->second->_table,i) {
        new_hash.insert(make_pair(i->first | t->first,i->second));	
      }      
    }
    _table.clear();      
    _table.insert(new_hash.begin(),new_hash.end());       
  }

  // True if able to merge the current node with the next one,
  // conditions are: it does not have any instruction, 
  // and children have the same mask that is nonzero in the same word where the
  // parent's mask is.  We also don't merge anything with a default table, since 
  // this is a special conflict situation.
  bool InstrTable::redundant() const 
  {
    if (!_instr && !_default_table && (_table.size() >= 1)) { 
      RNumber m = _table.begin()->second->_mask; 
      if (!m || !legal_mask(_mask|m)) {
        return false;
      }
      ForEach(_table,i) {
        const InstrTable &it = *(i->second);
        if (it._mask != m || it._instr || it._default_table) {
          return false;	  
        }	
      }
      return true;
    } else {  	
      return false;
    }
  }

  void InstrTable::finalize() 
  {
    if (!_mask) {
      return;
    }
    
    // TBD: check if we need it now.
    while (redundant()) {
      merge();
    }

    if (!legal_mask(_mask)) {
      DError("Mask 0x" << hex << _mask << dec << " is not confined to one word." );
    }

    ForEach(_table,t) {
      t->second->finalize();
    }
    return;
  }

  void InstrTable::dump(ostream &o,const gc_string &pfx) const
  {

    if (_instr) {
      o << _instr->name() << "\n";

      if (!_shorthands.empty()) {
        gc_string newpfx = indent(pfx);
        ForEach(_shorthands,i) {
          o << newpfx << "Shorthand:  " << (*i)->name() << ": ";
          (*i)->get_alias()->print(o,**i);
          o << "\n";
        }
      }
    }

    if (!_table.empty()) {
      o << pfx << hex << "Mask:  0x" << _mask << dec;

      if (_word) {
        o << " (word " << _word << ")";
      }
      o << "\n";

      gc_string newpfx = indent(pfx);
      ForEach(_table,i) {
        o << newpfx << (i->first >> _shift) << "(" << hex << i->first << dec << "):  ";
        i->second->dump(o,newpfx);
      }
      if (_default_table) {
        o << newpfx << "default:  ";
        _default_table->dump(o,newpfx);
      }
    }
  }

}

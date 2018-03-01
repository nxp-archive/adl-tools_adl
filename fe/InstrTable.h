//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Decode table classes.
//

#ifndef _INSTRTABLE_H_
#define _INSTRTABLE_H_

#include <iosfwd>
#include <string>

#include "gc/gc_cpp.h"

#include "helpers/gc_hash_map.h"
#include "helpers/gc_list.h"
#include "helpers/BasicTypes.h"
#include "rnumber/RNumber.h"


namespace adl {

  class Instr;
  class InstrTable;
  class Field;
  class OpcFields;
  typedef gc_map<rnumber::RNumber,InstrTable *> TableHash;

  struct InstrList : public gc_list<Instr *> {};

  // The decode table is stored as an m-ary tree.  Inner nodes are InstrTable
  // objects, which store a mask and then a hash, where keys are opcodes and
  // values are child nodes.  Leaf nodes are Instr objects.
  struct InstrTable : public gc {
    InstrTable();
    InstrTable(unsigned width); 
    // Add an instruction- throws a decode_error if there's a problem.
    bool add_inst(Instr &instr, uint64_t good_mask);
    void add_instrs(Instrs &instrs, MsgLog &ml);
    
    const rnumber::RNumber& mask() const { return _mask; };
    unsigned shift() const { return _shift; };
    const Instr *instr() const { return _instr; };
    const InstrList &shorthands() const { return _shorthands; };

    typedef TableHash::const_iterator const_iterator;

    bool empty() const { return _table.empty(); };
    const_iterator begin() const { return _table.begin(); };
    const_iterator end()   const { return _table.end(); };

    const InstrTable *default_table() const { return _default_table; };
    
    bool is_prefix() const;

    void setInstrWidth(unsigned width) { _max_instr_width = width; } //
    unsigned getInstrWidth() const { return _max_instr_width; } //
    unsigned getWidth() const { return _width; } // 
    void setWidth(unsigned width) { _width = width;};

    void finalize();
    // Dump to a stream.
    void dump(std::ostream &,const gc_string &pfx) const;
    void clear();
    
    bool is_embedded() const { return _embedded; } // Indicate whether the instruction table is used for micro instructions or not
    void set_embedded(); // Set the instruction table for micro instructions RECURSIVELY
   
  private:
 
    // Try do build decoding tree, 
    bool build_tree(Instrs &instrs, const rnumber::RNumber &used_mask, const rnumber::RNumber &used_conflict_mask, MsgLog &ml);
    bool add_to_tree(const gc_set<rnumber::RNumber> &values,Instrs &instrs,const rnumber::RNumber &mask,
                     const rnumber::RNumber &used_mask,const rnumber::RNumber &used_conflict_mask,MsgLog &ml);
    // Add instructions to the already built tree
    void add_instrs_internal(Instrs &instrs);
    void add_instr_internal(Instr &instr, rnumber::RNumber used_bits);
    void set_mask(rnumber::RNumber mask);
    void add_node(rnumber::RNumber value);
    void resolve_conflict(Instrs &instrs, rnumber::RNumber used_mask);

    bool try_with_conflicts(Instrs &instrs,const rnumber::RNumber &cmask,const rnumber::RNumber &used_mask,
                            const rnumber::RNumber &used_conflict_mask,MsgLog &ml);
    bool construct_tree_from_instrs(Instrs &instrs,rnumber::RNumber &mask,const rnumber::RNumber &used_mask,
                                    const rnumber::RNumber &used_conflict_mask,bool at_end,MsgLog &ml);
    bool try_conflict_partitioning(Instrs &instrs,const rnumber::RNumber &used_mask,
                                   const rnumber::RNumber &used_conflic_mask,MsgLog &ml);
    bool add_conflict_sets(Instrs &normal,Instrs &conflict,bool default_conflict,
                           const rnumber::RNumber &cmask,const rnumber::RNumber &used_mask,
                           const rnumber::RNumber &used_conflict_mask,MsgLog &ml);
    
    OpcFields get_opcfields(const Instr &instr);
    // If the field crosses words boundary we cut it
    OpcFields split_field(const adl::Instr::Field& ifp,unsigned shift);
    
    void get_instrs(InstrList &instrs); 
 
    InstrTable *getTable(const rnumber::RNumber& mvalue);

    // True if we can remove this node by mergine with the next node
    bool redundant() const;
    // Merge with the child node.
    void merge();
    
    // Width of encoding (maximum width of instruction) 
    unsigned _width;
    // Which word to get.  By default, it's set to 0, which means "the next word".
    // If greater than zero, then we have to skip ahead and read that many more words
    // to find a portion of the instruction that can be used for differentiating the 
    // instructions.
    unsigned _word;
    // The mask for indentifying the opcode.
    rnumber::RNumber _mask;
    // Shift amount to right-justify mask/value.
    int   _shift;
    // Child nodes, hashed by opcode.  These are not justified- they
    // correspond directly to what is obtained by applying the mask.
    TableHash _table;
    // In the weird case where the user has specified a conflict field, we first check all known cases 
    // in the _table member, then have a default value here, for anything else.  This is rare.
    InstrTable *_default_table;
    // If we have a default table, then it can either be due to an explicit
    // user-defined conflict mask (via allow_conflict on instruction fields) or
    // because we had to manually split a mask.  This is true if the conflict
    // arose from a manually specified mask, in which case we check the conflict
    // mask when inserting instructions.  Otherwise, we use the instruction's
    // opcode mask.
    bool _default_conflict;
    // If a leaf node, the instruction is stored here.
    Instr     *_instr;
    // If a leaf node, then this stores any short hand instructions that
    // may exist.
    InstrList  _shorthands;
    bool _embedded;  // Indicate whether the instruction table is used for micro instructions or not
    unsigned _max_instr_width; // Different from _width, which is round to 32-bit word
  };

  std::ostream &operator<< (std::ostream &,const InstrTable &);

}

#endif

// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Various functions for dealing with instruction decoder generation.
//

#include <iostream>

#include "rnumber/RNumber.h"

#include "helpers/Macros.h"
#include "helpers/CodeGen.h"

#include "fe/Data.h"

#include "DecodeSupport.h"
#include "FieldSupport.h"

using namespace std;
using namespace rnumber;

namespace adl {

  // Returns amount by which instruction should be shifted in order to make place for
  // prefix information.
  // For big endian the shift is the highest bit of prefix fields plus one
  // For little endian we need to know the highest bit of usual fields and subtract it from highbit of instruction
  static unsigned encoding_shift(const Instr& instr,bool little_endian) 
  {
    unsigned ret = 0;
    ForEach(instr.getAllFields(),k) {
      if (!k->_field) {
        continue;
      }
      const InstrField &f = *(k->_field->interface() ? k->_field->interface() : k->_field);
      if (f.is_prefix() && !little_endian) {
        ret = max(ret,k->_field->highbit());	
      } 
      else if (!f.is_prefix() && little_endian) {
        ret = max(ret,k->_field->highbit());	
      }
    }
      
    if (little_endian) {
      ret = instr.width() - ret - 1;
    } else{
      ret = ret + 1;
    }
      
    return ret;
  }

  // Tries to find an appropriate prefix counter, else uses the default that's
  // supplied.  We also search an instruction's parent type for prefix counters.
  gc_string getPrefixIndex(const InstrField &field,const Instr &instr,const gc_string &default_index)
  {
    if (instr.getField(field.name()) >= 0) {
      if (auto pc = instr.get_prefix_counter(field.name())) {
        return *pc;
      }
    } else if (auto tinstr = instr.get_type()) {
      if (tinstr->getField(field.name()) >= 0) {
        if (auto pc = tinstr->get_prefix_counter(field.name())) {
          return *pc;
        }
      }
    }
    return default_index;
  }


  // write fetch code for prefix architecture
  void writePrefixArchFetchCode(ostream &o,const Core &core,const gc_string &iname,const gc_string &fname,bool prefix_table)
  {
    if (prefix_table) {
      // use fetched value as is.
      o << iname << "[0] = " << fname << ";\n" ;
    } else {
      // Fetch the indexed prefix bits which are not related to any block.
      // We assume that all instructions have the same fixed prefix bits 
      // therefore, it suffices to look at a single pseudo instruction
      const InstrHash &instrs = core.instrs();
      bool little_endian = (core.bitEndianness() == Core::Little);
      unsigned enc_shift = 32;
      ForEach(instrs,i) { 
        const Instr &instr = *(i->second);
        if (!instr.is_pseudo()) {
          continue;
        }
        bool first = true;
        ForEach(instr.getFields(),k) {
          if (!k->_field) {
            continue;
          }	    
          const InstrField &f = *(k->_field->interface() ? k->_field->interface() : k->_field);
          if (!f.is_prefix()) {
            continue;
          }
	    
          enc_shift = encoding_shift(instr,little_endian); 
	    
          if (!f.index() || !f.blks().empty()) {
            continue;
          }
          if (first) {
            o << iname << "[0] = (";
            first = false;
          } else {
            o << iname << "[0] |= (";
          }
          getPrefixField(o,f,getPrefixIndex(f,instr,"_packet_position"), "");
          unsigned shift = little_endian ? 32 - (instr.width() - k->_field->lowbit()): 31 - k->_field->highbit();
          o <<  " << " << shift << ");\n";
        }
        break;
      }
      o << iname << "[0] |= (" << fname << " >> " << enc_shift << ");\n"
        << iname << "[1] = ("  << fname << " << " << 32-enc_shift << ");\n\n";
    }
  }

  void genPrefixFieldSet(ostream &o,const Core &core,const InstrField &field, const InstrField &ifc, const gc_string &ifc_name)
  {
    gc_string value = ifc.name();
    if (field.width() < ifc.width()) {
      if (core.is_little_endian()) {
        o << ifc_name << ".set(" << ifc.width() - 1 << "," << 0 << "," << value << ");\n";
      } else {
        o << ifc_name << ".set(0," <<  ifc.width()-1 << "," << value << ");\n";
      }

    } else {
      o << ifc_name << " = " << value << ";\n";
    }
  }

  static void genPrefixFieldGetter(ostream &o,const InstrField &field, const gc_string &ifc_name, const gc_string &pfx, gc_string blk="")
  {
    o << ifc_name << "_getter = &" <<  pfx << "get_" << field.name() << blk << ";\n";
  }

  // This writes out the logic which sets the special prefix variables from the
  // data in a prefix instruction.  This data is then used during subsequent
  // decoding of instructions to extend the actual size of the instruction.
  void genPrefixFieldSetup(ostream &o,const Core &core,const Instr &instr,const gc_string &pfx)
  {
    int numfields = instr.sizeFields();
    for(int i = 0; i != numfields; ++i) {
      Instr::Field fp = instr.getField(i);
      if (fp._type == Instr::Field::Resrvd) {
        continue;
      }

      const InstrField &field = *(fp._field);
      const InstrField &ifc   = field.interface() ? *(field.interface()) : field;
      const InstrField *ftype = ifc.field_type();
      bool  gen_getter = (ftype && ftype->blks().size() > 1);
      if (ifc.blks().size() > 1) {
        ForEach(field.blks(),b) {
          gc_string sfx = "__" + *b;
          gc_string trgt = "_" + ifc.name() + sfx;
          genPrefixFieldSet(o,core,field,ifc,trgt);
          if (gen_getter) {
            trgt = "_" + ftype->name() + sfx;
            genPrefixFieldGetter(o,ifc,trgt,pfx,sfx);
          }
        }
      } else {
        gc_string trgt = "_" + ifc.name();
        genPrefixFieldSet(o,core,field,ifc,trgt);
        if (gen_getter && !ifc.blks().empty()) {
          trgt = "_" + ftype->name() + "__" + ifc.blks().front();;
          genPrefixFieldGetter(o,ifc,trgt,pfx);
        }
      }
    }
  }

  void packetPosIncr(ostream &o,const Core &core,const gc_string &pkt_pos,const Instr &instr)
  {
    unsigned incr = (core.packet_pos_incr() == 0) ? 1 : (instr.fetch_width() / core.packet_pos_incr()); 
    if (core.parallel_execution()) {
      o << pkt_pos << " += " << dec << incr << ";\n";

      // Increment any prefix counters we have, as well.
      ForEach(instr.prefix_counter_mods(),i) {
        o << i->first << ((i->second) ? "++" : "--") << ";\n";
      }
    }
  }

  // Returns number of nonzero words counting from the left.
  unsigned required_fetch(const RNumber& mask) 
  {
    ForRange(mask.size()/32,i) {
      uint32_t tmp = (uint32_t)(mask >> (i*32)).uint32();
      if(tmp != 0) {
        return mask.size()/32 - i;
      }
    }
    return 0;   
  } 

  unsigned num_words(const Core &core,unsigned num_bits) 
  {
    return (num_bits-core.prefix_bits()+31)/32;  
  } 

  // Generate the prefix counters, which can be used to index into indexed
  // prefix fields.
  void genPrefixCounters(ostream &o,const Core &core,ClassData &cd)
  {
    const PrefixCounterHash &pfxs = core.prefix_counters();

    ForEach(pfxs,i) {
      const PrefixCounter &pfx = *i->second;

      cd._member_vars.push_back("unsigned " + pfx.name());
      gc_string ci;
      MkGStr(ci,pfx.name() << "(" << pfx.reset_value() << ")");
      cd._constr_inits.push_back(ci);
    }

    o << "void prefix_counter_reset()\n"
      << "{\n";
    ForEach(pfxs,i) {
      const PrefixCounter &pfx = *i->second;
        
      o << pfx.name() << " = " << dec << pfx.reset_value() << ";\n";
    }
    o << "}\n\n";

    cd._reset_stmts.push_back("prefix_counter_reset()");
  }


}

// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Various functions for dealing with instruction fields.
//

#include <iostream>

#include <tuple>

#include "helpers/Macros.h"
#include "helpers/CodeGen.h"
#include "helpers/gc_vector.h"
#include "helpers/gc_string.h"
#include "helpers/AdlTranslatingWalker.h"

#include "fe/Helpers.h"

#include "opencxx/parser/Ptree.h"
#include "opencxx/parser/PtreeUtil.h"

#include "FieldSupport.h" // required for dumping instrfield action code
#include "IFieldWalker.h"
#include "Data.h"

using namespace std;
using namespace rnumber;
using namespace Opencxx;

#undef Verbose 
//#define Verbose 1

#ifdef Verbose
#  define V1(x) { if (Verbose >= 1) { cerr << x; } }
#else
#  define V1(x)
#endif

namespace adl {


  struct Term {
    int  width;
    int  start,end; // term is from left to right bit inclusively
    int  shift;
    int  jshift;    // Right-justification shift for 64bit fields.

    Term (int w,int s,int e,int sh,int jsh) : width(w), start(s), end(e), shift(sh), jshift(jsh) {};

    // If a negative index, then is indicates that it's not part of the actual
    // instruction encoding itself.
    bool is_prefix() const { return start < 0; };
  };


  struct FieldInstrs {
    gc_string   _fname;
    mutable IntVect  _ids;

    
    FieldInstrs(const gc_string &fname) : _fname(fname) {};
    FieldInstrs(const gc_string &fname, int id) : _fname(fname) {  add(id); };
    void add(int id) const { _ids.push_back(id); };
    bool operator<(const FieldInstrs &o) const  { return _fname < o._fname; };

  };


  typedef gc_set<FieldInstrs>               MappedFields;
  typedef gc_map<const InstrField*, MappedFields> FieldMappedFields;

  
  typedef gc_vector<Term> Terms;
  

  // Writes shift and mask operations.
  // A positive shift is a right shift, negative is left.
  // Non zero width modification means that instead of (var) (Var>> width_modifiction) is 
  // used. This is needed by setters.
  // Add one more argument for field width
  void writeFieldTerm(ostream &o,const gc_string &var,int start,int end,int shift,int jshift,int max_width,
                      int instr_width, unsigned field_width, bool is_little_endian, unsigned width_modification) //

  {
    unsigned num_words = (max_width + 31)/32;
 
    // Use bits<> as the type string when the field width is over 64 bits
    gc_string type_str;
    if (field_width > 64) {
      gc_ostringstream ss;   
      ss << "bits" << "<" << field_width << ">";
      type_str = ss.str();
    } else type_str = "uint64_t";
  
    gc_vector<gc_string> vars;
    ForRange(num_words,i) {
      int index = (is_little_endian) ? (num_words-i-1) : i;
      vars.push_back(var + uitogs(index));
    }     
    // It must be a setter, so only two inputs are expected (max instructoin field width is 64 bits)
    if (width_modification) {    
      if (num_words == 1) {
        vars[0] = "(" + var + "0 >> " + uitogs(width_modification) + ")";
      } else if (num_words ==2) {
        vars[0] = "(((" + var + "0 >> " + uitogs(width_modification) + ") << 32)";
        vars[1] = "(" + var + "1 >> " + uitogs(width_modification) + ")|(" +
          var + "0 << (32 - (" + uitogs(width_modification) + "))))"; 
      }
    }

    unsigned src = (start)/32;	
    unsigned s = start % 32;
    unsigned e = end   % 32;
    uint32_t mask = (uint32_t)-1;
    mask = ((((mask << s) >> s) >> (32-1-e)) << (32-1-e)); 

    // Sanity check- skip if, for some reason, the field's word exceeds the
    // maximum number of words we could ever have.
    if (src >= vars.size()) {
      o << "0";
      return;
    }
  
    if (field_width > 32) { //
      // if shift left
      if (shift < 0) {
        o << "(((" << type_str << ")(" << vars[src] << "&0x" << hex << mask << dec << ")"; //
        if (src == num_words-2) {
          o << " << 32";
        }  	
        o << ") << " << -shift << ")";	
      } else {
        // For other over 32-bit cases, we right-justify the word value, then shift it
        // into the appropriate location in the final bits vector.
        o << "(((" << type_str << ")((" << vars[src] << "&0x" << hex << mask << dec << ") >> " << jshift << ") << " << shift << "))"; //
      }
    } else {
      o << "((" << vars[src] << "&0x" << hex << mask << dec << ")";
      // Right shift
      if (shift > 0) {
        if (src == num_words-1) {
          o << ">>" << shift;
        } else {  
          if (shift > (int)(32*(num_words-src-1))) {
            o << ">>" << shift%32;
          } else if (shift < (int)(32*(num_words-src-1))){
            o << "<<" << 32 - shift%32;
          }
        }	
        // left shift : just shift the rightmost word
      } else if (shift < 0) {
        if (src == num_words-1) {
          o << "<<" << -shift;
        } 	
      }
      o << ")";	              
    }   
  }

  // Writes shift and mask operations.  Same as writeFieldTerm, except that the input is only ever
  // a single item- either 32-bit or 64-bit.  We just create a single operation for it, rather than
  // taking 64-bit data from multiple input words.
  //
  // A positive shift is a right shift, negative is left.
  // Non zero width modification means that instead of (var) (Var>> width_modifiction) is 
  // used. This is needed by setters.
  void writeFieldTermSingle(ostream &o,const gc_string &var,int start,int end,int shift,int width,bool use_bits)
  {
    if (use_bits) {
      o << var << "(" << start << "," << end << ")";
    } else {
      // Just work with simple unsigned values.
      int base = (width > 32) ? 64 : 32;

      gc_string sfx;
      uint64_t mask = (uint64_t)-1;
      int off = 0;
      if (base == 64) {
        sfx = "ull";
      } else {
        // Since our mask is 64-bit, and we use a left-shift to clear the upper
        // bits, we have to take into account the extra shifting required for when
        // we only care about a 32-bit value.
        off = 32;
      }

      unsigned s = start % base + off;
      unsigned e = end   % base;
      mask = ((((mask << s) >> s) >> (base-1-e)) << (base-1-e)); 

      o << "((" << var << " & 0x" << hex << mask << dec << "ull)";
      // Right shift
      if (shift > 0) {
        o << ">>" << shift;
      }
      o << ")";
    }
  }

  // Return a mask of the bits specified (inclusive).
  RNumber makemask(unsigned start,unsigned end,unsigned width)
  {
    RNumber mask(0,width);
    mask.invert();    
    return ((((mask << start) >> start) >> (width-1-end)) << (width-1-end));
  }

  uint32_t make_submask(unsigned start) {
    return makemask(32-start%32,32,32).uint32();
  }
  
  // Break interval at word border
  gc_vector<pair<int,int> > break_interval(int start,int end) {
    gc_vector<pair<int,int> > ret;
    // Start from the left
    while (end >= start) {
      if (start < 0 && end >= 0) {
        // Crossing from the prefix into normal encoding, so split.
        ret.push_back(make_pair(start,-1));
        start = end;
        //      } else if (e/32 == s/32) {
      } else if (end/32 == start/32) {        
        // In the same word -> we are done
        ret.push_back(make_pair(start,end));
        return ret;
      } else {
        // Go one word up
        int next = (start/32 + 1)*32;
        ret.push_back(make_pair(start,next-1));
        start = next;
      }
    }
    return ret;
  }

  // Create shift and mask operations for getting values.
  Terms makeGetPairs(const BitPairVect &bits, const IntVect *gaps, bool is_little_endian,
                     unsigned encoding_width, unsigned instr_width, unsigned field_width,
                     bool check_break)
  {
    Terms gps;
    // For each range item, we construct a mask and shift value.
    // The mask is based upon the bits in the range.  The shift
    // value is based upon the position of the range in the list.
    // For example, bits of {[16:20],[11:15]} for a 10 bit field 
    // results in the first element having a mask of 0x1f0000 and a 
    // right shift of 16.  The second element is a mask of 0xf800
    // and a right shift of 6.

    // We only support up to 64-bit destinations.  For 32-bit fields or less,
    // though, everything works out if we just use the instruction's width.
    int destwidth = (field_width > 32) ? 64 : encoding_width;

    bool is_64bit = (field_width > 32);
    bool has_gaps = (gaps && !gaps->empty());
    int  deststart = destwidth - field_width;
    if (has_gaps) {
      deststart -= gaps->back(); 
    }
    int ix = 0;
    int instr_shift = (is_little_endian) ? 0 : (encoding_width - instr_width);
    ForEach(bits,i) {
      // For little-endian, everything is already right-justified, so we don't
      // want to shift.  For big-endian, it's in the upper bits, so we do have
      // to shift.
      int s = i->lo(encoding_width) - instr_shift;
      int e = i->hi(encoding_width) - instr_shift;

      if (has_gaps) {
        deststart += (*gaps)[ix];
        ++ix;
      }

      // Check if we need to break up the intervals.  If check_break is false,
      // then skip.  If this is for handling nested fields, then check_break will
      // be false, as we know that this comes from a single contiguous value.
      gc_vector<pair<int,int> > pairs;
      if (check_break) {
        pairs = break_interval(s,e);
      } else {
        pairs.push_back(make_pair(s,e));
      }
      ForEach(pairs,p) {
        int l = p->first;
        int r = p->second;
        // Width of field.
        int swidth = r - l + 1;      
        // Shift value to get from source to destination.
        int jshift = (32 - (r%32+1));
        int shift;
        if (is_64bit) {
          // For larger fields, we have to map from the instruction's location to
          // the final 64-bit location.  Basically, we're taking a right-justified
          // value (the right-justification comes from jshift) and then moving it
          // as far left as we need it to go to get to deststart.
          shift = (64 - swidth - deststart);
        } else {
          // For 32-bit, everything is simpler b/c it's all within a single word.
          shift = (deststart - l);
        }
        // Update destination start bit for next range.
        deststart += swidth;
        gps.push_back(Term(swidth,l,r,shift,jshift));
      }
    }
    return gps;
  }

  // Create shift and mask operations for setting values.
  Terms makeSetPairs(const BitPairVect &bits,int encoding_width)
  {
    Terms gps;
    // For each range item, we construct a mask and shift value.

    // Accesses are done in reverse order so that we can
    // consume bits from the right.
    RevForEach(bits,i) {
      int s = i->lo(encoding_width);
      int e = i->hi(encoding_width);
      auto pairs = break_interval(s,e);
      RevForEach(pairs,p) {
        int l = p->first;
        int r = p->second;
        gps.push_back(Term(r-l+1,l,r,0,0));
      }
    }
    return gps;
  }

  gc_string bits_type(const InstrField &f,bool use_bits = true)
  {
    gc_ostringstream ss;
    ss << ((use_bits) ? "bits" : "intbv") << "<" << f.size() << ">";
    return ss.str();
  }

  gc_string data_type(const InstrField &f) 
  {
    return (f.size() > 32 ? "uint64_t" : "unsigned");      
  }

  gc_string to_data_type(const InstrField &f) 
  {
    return (f.size() > 32 ? ".uint64()" : ".uint32()");
  }

  // If instr_width is zero we dont add width postfix
  gc_string genNestedFieldGetterName(const InstrField &f, const InstrField &encap, const Core &core, unsigned instr_width)
  {
    gc_ostringstream os;
    gc_string encap_name = encap.interface() ? encap.interface()->name() : encap.name();
    os << "get_" << encap_name <<  "__" << f.name();
    if (core.bitEndianness() == Core::Little && instr_width != 0) {
      os << "_width_" << instr_width;
    }
    
    return os.str();
  }

  gc_string genIntrnNestedFieldGetterName(const InstrField &f, const InstrField &encap)
  {
    gc_string encap_name = encap.interface() ? encap.interface()->name() : encap.name();
    return gc_string("get_") + encap_name + "__" + f.name();
  }
  
  bool ignore_field(const InstrField &f,bool table = false) 
  {
    if ((f.is_pseudo() && !table) || f.is_prefix() || f.encap_field() || f.is_variable() || !f.is_used()) {
      return true;
    }

    if (auto ifc = f.interface()) {
      return ifc->is_variable();
    }

    return false;
  }

  gc_string makeInstrFldTblAccessName(const InstrField &f, const InstrField *encap)
  {
    if (encap) {
      return "get_" + encap->name() + "___" + f.name() + "_Table";
    } else {
      return "get_" + f.name() + "_Table";
    }
  }

  void genConstrEntries(UIntVect &constr, const InstrFieldTable::Entries &entries) 
  {
    int i=0;
    ForEach(entries,e) {
      if (!InstrFieldTable::is_reserved(*e)) {
        constr.push_back(i);
      }
      ++i;
    }
  }


  void genInstrFieldTableAccess(std::ostream &o, const InstrField &field) 
  {
    const InstrFieldTable          &table   = field.table();
    const InstrFieldTable::Entries &entries = table.entries();
    UIntVect constr;

    genConstrEntries(constr,entries); 

    o << "switch(idx) {\n";
    ForEach(constr, i) {
      o << "case " <<  *i << ":\n" 
        << "switch(e) {\n";
      int ci=0;
      ForEach(entries[*i],v) {
        o << "case " << ci++ << ":\n" 
          << "return " << *v << ";\n";
      }
      //FIXME: o << "default: RError(\"Incorrect InstrField " << field.name() << " table entry:   \" << e);\n"
      o << "}\n";
    }
    //FIXME: o << "default: RError(\"Incorrect InstrField " << field.name() << " table entry:   \" << idx);\n"
    o << "}\n"
      << "return 0;\n"
      << "}\n\n";
  }


  void writeFieldValue(ostream &o, const BitPairVect &bits, const IntVect *gaps, const gc_string& base,
                       bool is_little_endian,unsigned encoding_width, unsigned instr_width) 
  {
    unsigned field_width = bits.width();
    Terms ifield_gps = makeGetPairs(bits,gaps,is_little_endian,encoding_width,instr_width,field_width,true);
    ForEach (ifield_gps,gi) {
      writeFieldTerm(o,base,gi->start,gi->end,gi->shift,gi->jshift,encoding_width,instr_width,
                     field_width, is_little_endian,0);  // 
      if (gi+1 != ifield_gps.end()) {
        o << " | ";		
      }
    }
    o << ";\n";
  }


  void writeFieldValue(ostream &o, const InstrField &ifield,gc_string sym,bool is_little_endian,
                       unsigned encoding_width, unsigned instr_width) 
  {
    gc_string base = (ifield.is_inverted()) ? ("~" + sym) : sym;
    writeFieldValue(o,ifield.bits(),ifield.gaps(),base,is_little_endian,encoding_width,instr_width);
  }


  void writeFieldValueSingle(ostream &o, const BitPairVect &bits, const IntVect *gaps, 
                             const gc_string& var, bool is_little_endian, unsigned width,bool use_bits) 
  {
    unsigned field_width = bits.width();
    Terms ifield_gps = makeGetPairs(bits,gaps,is_little_endian,width,width,field_width,false);
    ForEach (ifield_gps,gi) {
      writeFieldTermSingle(o,var,gi->start,gi->end,gi->shift,width,use_bits);
      if (gi+1 != ifield_gps.end()) {
        o << " | ";		
      }
    }
    if (use_bits) {
      o << ".uint32()";
    }
  }


  void writeFieldValueSingle(ostream &o, const InstrField &ifield,gc_string sym,
                             bool is_little_endian, unsigned width,bool use_bits) 
  {
    gc_string var = (ifield.is_inverted()) ? ("~" + sym) : sym;
    writeFieldValueSingle(o,ifield.bits(),ifield.gaps(),var,is_little_endian,width,use_bits);
  }


  void genFuncArgs(ostream &o, unsigned num_words,bool add_brace = true) 
  {
    o << "(";
    ForRange(num_words,j) {
      o << "unsigned x" << j <<  " ATTRIBUTE_UNUSED";
      if (j == num_words-1) {
        o << ")";
        if (add_brace) {
          o << " {\n";
        }
      } else {
        o << ", ";
      }
    }
  }

  void genCallArgs(ostream &o, unsigned num_words, gc_string arg = "x") 
  {
    o << "(";
    ForRange(num_words,j) {
      o << arg << j <<  (j == num_words-1 ? ")" : ", ");
    }
  }

  void genReturnFields(ostream &o, const Core &core, const InstrField &ifield,unsigned instr_width)
  {
    gc_map<const InstrField *, Terms> ref_gps;
    gc_map<const InstrField *, BitPairVect> ref_bits;
    gc_multimap<const InstrField *, pair<InstrField::Field::Type,unsigned> > ref_access;
    const InstrField::Fields &ifields = ifield.fields();

    bool tbl_access = false;
    ForEach(ifields,f) {
      if (f->_field == &ifield) {
        continue;
      }
      pair<InstrField::Field::Type,unsigned> typeIdx;
      if (f->_type == InstrField::Field::TblIdx) {  // Need to distinguish only  between these two access types
        typeIdx = make_pair(f->_type, f->_value);
        tbl_access = true;
      } else {
        typeIdx = make_pair(InstrField::Field::Ref,0);
	
      }
      ref_access.insert(make_pair(f->_field,typeIdx));
    } 

    ForEach(ifields,f) {
      ref_bits.insert(make_pair(f->_field,BitPairVect()));
    }

    ForEach(ifields,f) {
      auto it = ref_bits.find(f->_field);
      if (f->_type == InstrField::Field::Range) {
        it->second.add_bits(f->_range);
      } else if (f->_type == InstrField::Field::Ref) { // Full range of nested field
        it->second.add_bits(BitPair(0,f->_field->size()));
      }
    }

    bool is_le = (core.instrEndianness() == Core::Little);
    
    ForEach(ref_bits,rb) {
      Terms terms;
      ForEach(rb->second,bp) {
        BitPairVect tmp;
        tmp.add_bits(*bp);
        Terms term = makeGetPairs(tmp,ifield.gaps(),is_le,32,32,bp->width(),true);
        terms.push_back(term.front());
      }
      ref_gps.insert(make_pair(rb->first,terms));
    }
    if (tbl_access || ref_bits.find(&ifield) != ref_bits.end()) {
      o << data_type(ifield) << " " << ifield.name() << "_acc0 = ";
      writeFieldValue(o,ifield,"x",is_le,core.instr_width(),instr_width); 
      o << ";\n";     
    }

    unsigned num_words = core.instr_width()/32;
    ForEach(ref_access,ra) {
      const InstrField &nested = *(ra->first);
      pair<InstrField::Field::Type, unsigned>    typeIdx = ra->second;
      if (typeIdx.first  == InstrField::Field::Ref) {
        o << data_type(ifield) << " " << nested.name() << "_acc0 = " << genNestedFieldGetterName(nested,ifield,core,instr_width);
        genCallArgs(o, num_words, (ifield.name() + "_acc"));
        o << ";\n";
      } else { // It is a table access
        o  << "unsigned " << nested.name() << "_tbl" << typeIdx.second << " = " ;
        o  << makeInstrFldTblAccessName(nested,&ifield) 
           << "(" <<genIntrnNestedFieldGetterName(nested,ifield) << "("
           << ifield.name() << "_acc0)," << typeIdx.second << ");\n";
      }
    }

    unsigned shift = ifield.size();
    gc_list<tuple<gc_string,unsigned,unsigned> > terms_shifts;
    RevForEach(ifields,k) {
      const InstrField::Field &f = *k;
      const InstrField *fld = f._field;
      if (fld) {
        if (f._type == InstrField::Field::TblIdx) {
          gc_ostringstream os;
          os  << fld->name() << "_tbl" << f._value; 
          terms_shifts.push_back(make_tuple(os.str(),shift-fld->size(),shift-1));
          shift -= fld->size();
        } else {
          gc_string ret_term = AdlGenSym()->ToString();  
          gc_string base  = (fld->is_inverted()) ? "~" : "";
          base += fld->name() + "_acc";
          auto igp = ref_gps.find(fld);
          Term &gi = igp->second.back();
          o << data_type(ifield) << " " << ret_term << " = ";
          writeFieldTerm(o,base,gi.start,gi.end,gi.shift,gi.jshift,ifield.width(),ifield.width(),
                         fld->width(),(core.instrEndianness() == Core::Little),0);  //                     
          igp->second.pop_back();
          o << ";\n";
          terms_shifts.push_back(make_tuple(ret_term,shift-f._range.width(),shift-1));
          shift -= f._range.width();
        }
      }
    }

    // We're using an intbv type here so that we're always big-endian for bit-numbering purposes.
    gc_string res = "res";
    o << bits_type(ifield,false) << " " << res << ";\n";

    RevForEach(terms_shifts,i) {
      o << res << ".set(" << get<1>(*i) << "," << get<2>(*i) << "," << get<0>(*i) << ");\n";
    }

    o << "return " << res << ";\n";
  }

  // For a nested instruction field, this writes out the data described in the
  // 'fields' array.  This basically consists of two options: A series of
  // extraction terms from the parent, or a table reference to the parent.
  void writeNestedInstrFieldFields(ostream &o,const InstrField &f,const InstrField &encap,
                                   unsigned width,const gc_string &base,bool is_little_endian)
  {
    // If we have a single item and it's the encapsulating field, and it's
    // constrained, then we reference that guy's table.  We know 'fields' isn't
    // empty because the caller checks.
    const InstrField::Fields &fields = f.fields();
    assert(!fields.empty());
    o << "return ";
    if ((fields.front()._field == &encap || fields.front()._field == encap.interface()) && encap.is_constrained()) {
      // Yes- table reference.
      o << makeInstrFldTblAccessName(encap,0) << "(" << base << ".uint32()," << fields.front()._value << ")";
    } else {
      // No- writes out extraction terms.
      BitPairVect  ranges;
      ForEach(fields,i) { // nested instruction field may have only Range type fields.
        ranges.add_bits(i->_range);
      }	
      bool uses_table = false;
      if (f.is_constrained()) {
        // If this accesses a table, then we also call the table accessor.
        o << makeInstrFldTblAccessName(f,&encap) << "(";
        uses_table = true;
      }
      writeFieldValueSingle(o,ranges,0,base,is_little_endian,width,true);
      if (uses_table) {
        o << ",0)";
      }
    }
  }

  void genIntrnNestedInstrFieldGetter(ostream &o, const Core &core, const InstrField &f, const InstrField &encap,
                                      const gc_string &pfx,bool is_little_endian)
  {
    const InstrField::Fields &fields = f.fields();
    gc_string base = (f.is_inverted() ? "~x0" : "x0");
    unsigned width = encap.size();
    if (f.bits().empty()) {
      gc_string argtype = "const intbv<" + uitogs(encap.size()) + "> &";
      o << pfx << " "  << data_type(f) << " " << genNestedFieldGetterName(f,encap,core,0)
        << "(" << argtype << " x0)\n{\n";
      if (!fields.empty()) { // fields type nested instruction
        writeNestedInstrFieldFields(o,f,encap,width,base,is_little_endian);
      }
      else if (f.action()) {
        o << "bits<" << encap.width() << ">  &z0 = x0";
        o << f.action();
      }
      o << ";\n}\n\n";      
    } // fields empty  
    else { // bits not empty
      o << pfx << " "  << data_type(f) << " " << genNestedFieldGetterName(f,encap,core,0)
        << "(const intbv<" << encap.size() << "> &x0)\n{\n";
      o << "return ";
      writeFieldValueSingle(o,f.bits(),0,"x0",is_little_endian,width,true);
      o << ";\n}\n\n";
      // Generate version for internal use non-bits based
      o << pfx << " "  << data_type(f) << " " << genIntrnNestedFieldGetterName(f,encap)
        << "(" << data_type(encap) << "  x0) {\n return ";
      // Adjust the width to the size of integer we're using for this version of the function.
      writeFieldValueSingle(o,f.bits(),0,"x0",is_little_endian,(width > 32) ? 64 : 32,false);
      o << ";\n}\n\n";
    }
  }

  void genNestedInstrFieldGetter(ostream &o, const Core &core, const InstrField &f, const InstrField &encap, const gc_string &pfx, unsigned instr_width)
  {
    const InstrField::Fields &fields = f.fields();
    bool empty = false;
    unsigned num_words = core.instr_width()/32;
    bool is_little_endian = (core.instrEndianness() == Core::Little);
    if (f.bits().empty()) {
      o << pfx << " "  << data_type(f) << " " << genNestedFieldGetterName(f,encap,core,instr_width);
      genFuncArgs(o, num_words);	
      gc_string base = (f.is_inverted() ? "~z0" : "z0");
      o << data_type(encap) << " z0 = get_" << encap.name();
      if (core.is_little_endian() && instr_width) {
        o << "_width_" << instr_width;
      }
      genCallArgs(o,num_words);
      o << ";\n";
      if (!fields.empty()) { // fields type nested instruction
        BitPairVect  ranges;
        ForEach(fields,i) { // nested instruction field may have only Range type fields.
          ranges.add_bits(i->_range);
        }
        o << "return ";
        unsigned width = (encap.size() > 32) ? 64 : 32;
        writeFieldValueSingle(o,ranges,0,base,is_little_endian,width,false);
      }
      else if (f.action()) {
        o << f.action();
      }
    }
    else { // bits not empty
      if (!f.is_constrained()) {
        o << pfx << " "  << data_type(f) << " " << genNestedFieldGetterName(f,encap,core,0);
        genFuncArgs(o, num_words);	
        gc_string  fname = encap.name() + "_acc";
        o << data_type(encap) << " " << fname << "0 = ";
        writeFieldValue(o,encap,"x",(core.instrEndianness() == Core::Little),core.instr_width(),instr_width);
        o << "return ";
        writeFieldValue(o,f,fname,(core.instrEndianness() == Core::Little),core.instr_width(),instr_width);
      } else {  
        empty = true;
      }
    }    
    if (!empty) {
      o << ";\n}\n\n";
    }
  }


  gc_set<unsigned> instr_widths(const InstrField& f, const Core& core) {
    gc_set<unsigned> ret;
    if (core.is_little_endian()) {
      ForEach(core.instrs(),i) {
        const Instr& instr = *(i->second);
        if (instr.getField(f.name()) != -1) {
          ret.insert(instr.width());
        }   
      }    
    } else {
      ret.insert(core.instr_width());
    }
    return ret;
  }

  gc_string nested_data_type(const InstrField &f) 
  {
    ForEach(f.instrfields(),i) {
      const InstrField &n = *(i->second);
      if (n.bits().empty()) {
        return data_type(n);
      }
    }
    return data_type(f);
  }

  void  getReturnFields(const InstrField &f, StrSet &returnFields) 
  {
    ForEach(f.instrfields(),i) {
      const InstrField &n = *(i->second);
      if (n.bits().empty()) {
        returnFields.insert(n.name());
      }
    }
  }

  void genGenericInstrFieldGetters(ostream &o, const Core &core, const gc_string &pfx)
  {
    FieldMappedFields fieldMappedFields;
    StrStrMap         retTypes;

    ForEach(core.instrs(),i) {
      Instr &instr = *(i->second);
      int    id = instr.id();
      gc_string suffix = (core.is_little_endian() ? gc_string("_width_") + uitogs(instr.width()) : "");
      ForEach(instr.getFields(),j) {
        const InstrField *f = j->_field;
        const InstrField *inface = (f ? f->interface() : 0);
        if (inface) {
          gc_string fname = f->name() + suffix;
          retTypes.insert(make_pair(inface->name(),nested_data_type(*f)));
          auto mappedFieldIt =  fieldMappedFields.find(inface);
          if (mappedFieldIt == fieldMappedFields.end()) {
            mappedFieldIt = fieldMappedFields.insert(make_pair(inface,MappedFields())).first;
          } 
          MappedFields &mappedFields = mappedFieldIt->second; 	  
          MappedFields::iterator  fieldInstrsIt = mappedFields.find(fname);
          if (fieldInstrsIt == mappedFields.end()) {
            fieldInstrsIt = mappedFields.insert(FieldInstrs(fname)).first;
          }
          fieldInstrsIt->add(id);
        }
      }
    }

    o << "//\n // Instruction field generic getters. \n// \n\n";
    ForEach(fieldMappedFields,f) {
      StrSet nested;
      gc_string ifname     =  f->first->name(); // interface field name
      bool      is_prefix  =  f->first->is_prefix();
      gc_string retType    = retTypes.find(ifname)->second;
      gc_string sRetType   = retType;
      unsigned  nRetFields = core.num_ret_fields();
      if (nRetFields) {
        retType = "const " + retType + "*";
      }
	
      getReturnFields(*(f->first), nested);

      o << pfx << " " << retType << " get_" << ifname
        << "(const uint32_t *x ATTRIBUTE_UNUSED, int id ATTRIBUTE_UNUSED) {\n";
      if (nRetFields) {
        o << "static " << sRetType << " vals[] = {";
        ForRange(nRetFields,k) {
          o << "(" << sRetType << ") -1" << (k == nRetFields-1 ? "};\n" : ",");
        }
      }
      o << "switch(id) {\n";
      const MappedFields &mfs = f->second;
      ForEach(mfs, mf) {
        ForEach(mf->_ids,i) {
          o << "case " << *i << ":\n";
        }
        ostringstream os;
        os << "get_" << mf->_fname << "(";
        ForRange(core.instr_width()/32,k) {
          os << "x[" << k << "]" << (k == core.instr_width()/32 - 1 ? ")" : ",");
        }
        if (!is_prefix) {
          os << ".uint32()";
        }

        if (nRetFields) {
          if (nested.empty()) {
            o << "vals[0] = " << os.str() << ";\n";
          } else {
            int nset = 0;
            ForEach(nested,k) {
              o << "vals[" << nset++ << "] = get_" << ifname << "__" << *k 
                << "(bits<" << f->first->size() <<  ">(" << os.str() << "));\n";
            }
          }
          o << "return &vals[0];\n";
        } else {
          o << "return " << os.str() << ";\n";
        }
        o << "break;\n";
      }
      o << "default: RError(\"Illegal instruction id for field: " << ifname << "\");\n"
        << "}\n"
        << "}\n\n";
    }
  }


  // Create instruction field accessors.  These are functions
  // which take a uint and return the field value (right justified).
  void genInstrFieldGetters(ostream &o,const Core &core,const gc_string &pfx, bool gen_proto, bool micro_op_only)
  {

    const InstrFieldHash &instrfields = core.instrfields();
    o << "//\n// Instruction field getter functions.\n//\n\n";

    bool is_le = (core.instrEndianness() == Core::Little);
    
    StrSet written; // To keep track of already written nested fields, only one is needed for every field.
    ForEach(instrfields,i) { // gen nested instruction fields getters
      const InstrField &encap = *(i->second);
      if (encap.is_pseudo() || encap.is_prefix() || !encap.is_used()) {
        continue;
      }
      ForEach(encap.instrfields(),j) {
        InstrField &f = *(j->second);
        gc_string encap_name = encap.interface() ? encap.interface()->name() : encap.name();
        gc_string f_name = f.interface() ? f.interface()->name() : f.name();
        gc_string name = encap_name + "." + f_name;
        if (written.find(name) == written.end()) {
          genIntrnNestedInstrFieldGetter(o,core,f,encap,pfx,is_le);
          written.insert(name);
        }
      }
    }

    ForEach(instrfields,i) {
      const InstrField &f = *(i->second);
      gc_set<unsigned> widths = instr_widths(f,core);
      if (ignore_field(f)) {
        continue;
      }

      if (micro_op_only && f.type() != InstrField::tInstr) {
        continue;
      }
    
      unsigned width = core.instr_width();  //

      ForEach(widths,w) {
        gc_string width_str = core.is_little_endian() ? gc_string("_width_") + uitogs(*w) : "";

        if (gen_proto) {
          o << pfx << " " << bits_type(f) << " get_" << f.name() << width_str ;
          genFuncArgs(o, width/32,false); //
          o << " ATTRIBUTE_UNUSED;\n";
        }
	
        o << pfx << " " << bits_type(f) << " get_" << f.name() << width_str ;
        genFuncArgs(o, width/32); //
        if (f.action()) {
          o <<  "bits<" + uitogs(f.size()) + "> z0 = " ;
          writeFieldValue(o,f,"x",(core.instrEndianness() == Core::Little),width,*w); //
          o << "\n" <<  f.action() ;	
        } else if (!f.fields().empty()) {
          genReturnFields(o,core,f,*w);
        } else {
          o << "return ";
          writeFieldValue(o,f,"x",(core.instrEndianness() == Core::Little),width,*w); //
        }
        o << "\n}\n\n";
      }
    }
    
   
    o << "\n";
  }

  // Create instruction field setters.  These are functions
  // which place a uint into the bits defined by the field, returning
  // that value.
  // Note: Split fields handling assumes:
  // 1. Prefix bits are the higher bits of the field
  // 2. Prefix bits are continguous

  bool ignore_setfield(const InstrField& f) {
    return f.is_pseudo() || !f.is_used();
  }

  // Note: le_width is non-zero only if we're writing out little-endian terms.
  void writeSetterTerms(std::ostream &o,const Terms &sps,const gc_string &base,gc_string v,unsigned le_width)
  {
    int shift = 0;
    ForEach (sps,si) {
      if ( si->is_prefix()) {
        continue;
      }

      if (shift) {
        o << base << " >>=   0x" << std::hex << shift << ";\n";
      }	  

      shift = si->width;

      // Index of the target word.
      unsigned ind = (si->end)/32;

      // Hack: Little-endian is a hack due to all of the conversions of bit indices.
      unsigned v_ind = (le_width) ? (((le_width-1)/32) - ind) : ind;  //
      int pshift = (ind*32 - si->end + 31)%32;

      uint32_t msk = (uint32_t)-1 >> (32 - si->width);
      o <<  v << "[" << v_ind << "] |= ((" << base << "&0x" << hex << msk << ")";
      if (pshift > 0) {
        o << " << " << dec << pshift;
      } else if (pshift < 0) {
        o << " >> " << dec << -pshift;	
      }
      o << ");\n";
    }
  }

  void writePrefixBitsTerm(std::ostream &o,const BitPairVect &bits,int fetch_width,int prefix_width,
                           const gc_string &pfield,int pfield_width,gc_string v)
  {
    int encoding_width = fetch_width + prefix_width;
    int s = 0,e = 0,p_offset = 0;
    ForEach(bits,b) {
      if ((int) b->max() >= fetch_width) {
        s = b->lo(encoding_width);
        // For field which consists of both prefix and fetch bits
        e = min(b->hi(encoding_width),prefix_width-1);
        // The prefix offset is the offset of this field within the portion of the
        // prefix for this instruction.  For example, if each instruction gets two
        // bits in the prefix, and those two bits each represent one bit of a
        // field, e.g. RA and RT, then this is the offset from the right of that
        // field within those bits.
        p_offset = prefix_width - e - 1;
        break;
      }
    }
    int ind = e/32;
    int  rshift = (ind*32 - e +31)%32;
    uint32_t mask = (((uint32_t)-1) >> rshift) << rshift;
    mask = (mask << s%32) >> s%32;

    if (pfield_width) { // it is little endian
      o << "int shift = " << pfield_width << " - (index+1)*" << prefix_width << ";\n";
    } else {
      o << "int shift = index*" << prefix_width << ";\n";
    }
    // Note that the final shift is minus the prefix-offset so that the bits line
    // up correctly within the prefix.
    o << "unsigned v0 =  ((v"<< "[" << e/32 <<  "] & 0x" << hex << mask << ") >> " << dec << (rshift-p_offset) << ");\n"
      << pfield << "[current_position].value |= (v0 << shift);\n";
    //    << "adl_set_prefix_field_slice(&" << pfield << "[current_position],group_index,v0);\n";
  }

  void genInstrFieldSetters(std::ostream &o,const Core &core,bool gen_prefix_setters,const gc_string &pfx)
  {
    const InstrFieldHash &instrfields = core.instrfields();
    o << "//\n// Instruction field inserter functions.\n//\n\n";
    bool le = (core.instrEndianness() == Core::Little);

    ForEach(instrfields,i) {
      const InstrField &f = *(i->second);
      // we still need interfaces
      if (ignore_setfield(f))  {
        continue;    
      }            
      
      gc_set<unsigned> widths = instr_widths(f,core);
      
      ForEach(widths,w) {
        gc_string width_str = core.is_little_endian() ? gc_string("_width_") + uitogs(*w) : "";
	
        BitPairVect bits;
        ForEach(f.bits(),b) {
          bits.add_bits(BitPair(b->l(),b->r()));
        }


        gc_string base = (f.is_inverted()) ? "~val" : "val";
        unsigned disp = f.is_prefix() ? 0 : core.prefix_bits();
        Terms sps = makeSetPairs(bits,*w-disp);
        o << pfx << " void set_" << f.name() << width_str <<  "(unsigned *x ATTRIBUTE_UNUSED, bfd_uint64_t val ATTRIBUTE_UNUSED) {\n";
        writeSetterTerms(o,sps,base,"x",(le ? *w : 0));
        o << "}\n\n"; 
        const InstrField::SplitInfo *si = f.splitInfo();
        if (gen_prefix_setters && si && si->has_width(*w)) {
          o << pfx << " void set_" << f.name() << width_str <<  "_prefix(bfd_uint64_t val,int index,int current_position ATTRIBUTE_UNUSED) {\n";
          Terms pfx_sps = makeSetPairs(bits,*w);
          o << "\nunsigned v[] = {";
          ForRange(core.instr_width()/32,k) {
            o << "0, ";
          }
          o << "};\n\n";
          writeSetterTerms(o,pfx_sps,base,"v",(le ? *w : 0));
          int pfield_width = (core.is_little_endian() ? core.get_instrfield(si->_name)->width() : 0);
          writePrefixBitsTerm(o,bits,*w-disp,disp,si->_name,pfield_width,"v");
          o << "}\n\n"; 
        }
      }
    }
    o << "\n";
  }

  // Create instruction field clearers.  These are functions which, given an
  // instruction value, clear the specified field.
  void genInstrFieldClearers(std::ostream &o,const Core &core,const gc_string &pfx)
  {
    const InstrFieldHash &instrfields = core.instrfields();

    bool le = (core.instrEndianness() == Core::Little);
    o << "//\n// Instruction field clearer functions.\n//\n\n";
    ForEach(instrfields,i) {
      const InstrField &f = *(i->second);

      if (ignore_field(f)) {
        continue;
      }

      gc_set<unsigned> widths = instr_widths(f,core);

      ForEach(widths,w) {
        // Round up to nearest word for little-endian.
        //        unsigned width = (core.is_little_endian()) ? (32*((*w - 1) / 32 + 1)) : *w;
        unsigned width = *w;
        
        gc_string width_str = core.is_little_endian() ? gc_string("_width_") + uitogs(width) : "";
    
        RNumber mask = f.mask();
        gc_vector<uint32_t> masks;
        gc_vector<gc_string> vars;      
      
        o << pfx << " void clear_" << f.name() << width_str << " (unsigned *x) {\n";

        gc_string vbase = "x";
        ForRevRange(core.instr_width()/32,i) {
          // Left justify for little-endian, to be consistent with the setters.
          RNumber justified_mask = (le && (width % 32)) ? (mask << ((32 - width)%32)) : mask;
          uint32_t tmp = (uint32_t)(justified_mask >> i*32).uint32();
          masks.push_back(tmp);
          int v_ind = (le) ? (core.instr_width()/32 - i - 1): i;
          vars.insert(vars.begin(),vbase + "[" + uitogs(v_ind) + "]");
        }

        gc_string p = "";
	   
        ForRange (masks.size(),i) {
          // Clear only where needed
          if (masks[i]) {
            o << "  " << p << vars[i] << " = " << p << vars[i] << hex << " & (~0x" << masks[i] << dec << ");\n";
          } 
        }
        o << "\n}\n\n";
      }
    }
    o << "\n";
  }

  // Create instruction field clearers.  This generates a more comprehensive list,
  // equivalent to all setters that are created.
  void genInstrFieldClearersAll(std::ostream &o,const Core &core,const gc_string &pfx)
  {
    const InstrFieldHash &instrfields = core.instrfields();
    bool le = (core.instrEndianness() == Core::Little);	

    o << "//\n// Instruction field clearer functions.\n//\n\n";
    ForEach(instrfields,i) {
      const InstrField &f = *(i->second);

      if (ignore_setfield(f)) {
        continue;
      }

      gc_set<unsigned> widths = instr_widths(f,core);
      
      ForEach(widths,w) {
        // Round up to nearest word for little-endian.
        //        unsigned width = (core.is_little_endian()) ? (32*((*w - 1) / 32 + 1)) : *w;
        unsigned width = *w;
        
        gc_string width_str = core.is_little_endian() ? gc_string("_width_") + uitogs(width) : "";

        RNumber mask = f.mask();
        gc_vector<uint32_t> masks;
        gc_vector<gc_string> vars;
      
        o << pfx << " void clear_" << f.name() << width_str << "(unsigned *x ATTRIBUTE_UNUSED) {\n";

        gc_string vbase = "x"; 
        ForRevRange(core.instr_width()/32,i) {
          // Left justify for little-endian, to be consistent with the setters.
          RNumber justified_mask = (le && (width % 32)) ? (mask << (32 - width%32)) : mask;
          uint32_t tmp = (uint32_t)(justified_mask >> i*32).uint32();
          //uint32_t tmp = (uint32_t)(mask >> i*32).uint32();

          masks.push_back(tmp);
          int v_ind = (le) ? (core.instr_width()/32 - i - 1): i;
          vars.insert(vars.begin(),vbase + "[" + uitogs(v_ind) + "]");				
        }
  
        ForRange (masks.size(),i) {
          // Clear only where needed
          if (masks[i]) {
            o << "  " << vars[i] << " = " << vars[i] << hex << " & (~0x" << masks[i] << ");\n";
          } 
        }
        o << "\n}\n\n";
      }
    }
    o << "\n";
  }

  void genInstrFieldTableAccess(std::ostream &o, const InstrField& field, const gc_string &pfx, const InstrField *encap=0)
  {
    o << pfx << " unsigned "   << makeInstrFldTblAccessName(field,encap) << "(unsigned idx, unsigned e){\n" ;
    genInstrFieldTableAccess(o,field);
  }


  void genInstrFieldTableGetters(std::ostream &o, const Core &core,const gc_string &pfx)
  {
    const InstrFieldHash &instrfields = core.instrfields();
  
    ForEach(instrfields,i) {
      const InstrField &f = *(i->second);
      if (ignore_field(f,true)) {
        continue;
      }
      if (f.is_constrained()) {
        genInstrFieldTableAccess(o,f,pfx);
      }
      ForEach(f.instrfields(),k) {
        const InstrField &n = *(k->second);
        if (n.is_constrained()) {
          genInstrFieldTableAccess(o,n,pfx,&f);
        }
      }
    }
  }

  void genInstrFieldCheckers(std::ostream &o, const Core &core,const gc_string &pfx)
  {
    const InstrFieldHash &instrfields = core.instrfields();  
    ForEach(instrfields,i) {
      const InstrField &f = *(i->second);    
      // generate checker only for interface fields.
      if (f.has_checks() && !f.interface()) {
        o << pfx << "bfd_uint64_t " << f.name() << "_checker" << "(bfd_uint64_t n, bool check_only){\n";     
        o << "bfd_boolean result1 = FALSE;\n";
        o << "bfd_boolean result2 = FALSE;\n";
        if (!f.ranges().empty()) {
          ForEach(f.ranges(),i) {
            if (i->first == 0) {
              o << "if (n <= " << dec << i->second << "UL) {\n";	  
            }
            else {
              o << "if (" << dec << i->first << "ULL <= n && n <= " << i->second << "ULL) {\n";
            }
            o << "  result1 = TRUE;\n";
            o << "} ";
            if (!(*i == f.ranges().back())){
              o << "else ";
            }	  
          }      
        }
        else {
          o << "result1 = TRUE;\n";
        }

        o << "\n";

        if (!f.masks().empty()) {
          ForEach(f.masks(),i) {
            o << "if ((" << dec << i->first << "ULL & n)  ==  " << (i->first & i->second) << "ULL) {\n";
            o << "  result2 = TRUE;\n";
            o << "} ";
            if (!(*i == f.masks().back())){
              o << "else ";
            }	  
          }      
        }
        else {
          o << "result2 = TRUE;\n";
        }

        o << "\n"
          << "if (check_only) {\n" 
          << "   return (result1 && result2) ? 1 : 0;"
          << "}\n";
        o << '\n';      
        o << "if (!(result1 && result2)) {\n";
        o << "  as_fatal(_(\"Operand " << f.name() << " is out of bounds\"));\n";
        o << "  return -1;\n";
        o << "}\n";
        o << "return n;";    
        o << '\n' << "};\n";    
      }
    }
  }

  gc_string genFieldName(const InstrField &f, gc_string blk="")
  {
    return (blk == "" ? f.name() : (f.name() + "__" + blk));
  }

  bool is_multiblock(const InstrField &f) 
  {
    return (f.blks().size() > 1);
  }

  InstrField::IndexEndianness index_field_endianness(const Core &core,const InstrField &f)
  {
    switch (f.index_endianness()) {
    case InstrField::Big:
    case InstrField::Little:
      return f.index_endianness();
    default:
      return (core.is_little_endian()) ? InstrField::Little : InstrField::Big;
    }
  }
  
  void genPrefixFieldDeclaration(ostream &o, const Core &core,const InstrField &f, const gc_string &pfx, gc_string fname)
  {
    string type = (index_field_endianness(core,f) == InstrField::Big) ? "intbv" : "le_intbv";

    if (f.field_type()) {
      o << pfx << type << "<" << f.width() << "> &_" << fname << ";\n";
      
    } else {
      o << pfx << type << "<" << f.width() << "> _" << fname << ";\n";
    }
  }



  void genPrefixFieldGetterDeclaration(ostream &o, const InstrField &f, const gc_string &pfx, gc_string fname)
  {
    o << pfx << "pFieldGetter  _" << fname << "_getter;\n";
  }


  // Create declarations of core data member corresponding to prefix fields. 
  // Types and other declarations go to 'o', member-variables are sent to 'mo.
  void genPrefixFieldDeclarations(ostream &o, ostream &mo,const Core &core, 
                                  const gc_string &pfx,const gc_string &cname) 
  {
    const InstrFieldHash &instrfields = core.instrfields();
    o << "//\n// Prefix field declarations. \n//\n\n";
    
    
    ForEach(instrfields,i) {
      const InstrField &f = *(i->second);
      if (f.is_prefix() && f.is_pseudo() && is_multiblock(f)) {
        o << "typedef unsigned (" << cname << "::*pFieldGetter)(unsigned Index);\n\n";
        break;
      }
    }

    ForEach(instrfields,i) {
      const InstrField &f = *(i->second);
      if (!f.is_prefix() || !f.is_pseudo()) {
        continue;
      }
      if (is_multiblock(f)) {
        ForEach(f.blks(),b) { // A field associated with several block - duplicate data
          genPrefixFieldDeclaration(mo,core,f,pfx,genFieldName(f,*b));
          genPrefixFieldGetterDeclaration(mo,f,pfx,genFieldName(f,*b));
        }
      } else {
        genPrefixFieldDeclaration(mo,core,f,pfx,genFieldName(f));
      }
    }
  }

  void genPrefixFieldGetterCtor(const InstrField &f, StrList &constr_inits, const gc_string &blk)
  {
    gc_ostringstream  os;
    gc_string fname = genFieldName(f,blk);
    os << "_" << fname << "_getter(0)";
    constr_inits.push_back(os.str());
  }

  void genPrefixFieldCtor(const InstrField &f, StrList &constr_inits, gc_string blk="")
  {
    gc_ostringstream  os;
    gc_string fname = genFieldName(f,blk);
    if (auto tf = f.field_type()) {
      gc_string tblk;
      if (!blk.empty() || !is_multiblock(*tf)) {
        tblk = blk;
      } else {
        tblk = f.blks().front();
      }
      gc_string  tname = genFieldName(*tf,tblk);
      os << "_" << fname << "(_" << tname << ")";
    } else {
      os << "_" << fname  << "(" << (f.has_default_value() ? uitogs(f.default_value()) : "0") << ")";
    }
    constr_inits.push_back(os.str());
  }

  void genPrefixFieldCtors(const Core &core, StrList &constr_inits) 
  {
    const InstrFieldHash &instrfields = core.instrfields();
    ForEach(instrfields,i) {
      const InstrField &f = *(i->second);
      if (!f.is_prefix() || !f.is_pseudo()) {
        continue;
      }
      if (is_multiblock(f)) {
        ForEach(f.blks(),b) {
          genPrefixFieldCtor(f,constr_inits,*b);
          genPrefixFieldGetterCtor(f,constr_inits,*b);
        }
      } else {
        genPrefixFieldCtor(f,constr_inits);
      }
    }
  } 
  

  void genPrefixFieldSetter(ostream &o, gc_string fname, const gc_string &pfx, gc_string member_name)
  {
    // the implementation field sets the pseudo field it implements 
    o << pfx << " void set_" << fname  << "(unsigned x0) {\n";
    o << "_" << member_name << " = x0;\n";
    o << "}\n\n";
  }
  void genPrefixFieldSetters(ostream &o, const Core &core, const gc_string &pfx)
  {
    const InstrFieldHash &instrfields = core.instrfields();
    o << "//\n// Prefix field setter functions. \n//\n\n";
    ForEach(instrfields,i) {
      const InstrField &f = *(i->second);
      // If not a prefix field or is prefix opcode skip
      if (!f.is_prefix() || (!f.is_pseudo() && !f.interface())) {
        continue;
      }
      const InstrField &tf = (f.is_pseudo() ? f : *(f.interface()));
      const InstrField &pf = (tf.field_type() ? *(tf.field_type()) : tf);
      if (is_multiblock(f)) {
        ForEach(f.blks(),b) {
          genPrefixFieldSetter(o,genFieldName(f,*b),pfx,genFieldName(pf,*b));
        }
      } else {
        if (is_multiblock(pf)) {
          genPrefixFieldSetter(o,genFieldName(f),pfx,genFieldName(pf,f.blks().front()));
        } else {
          genPrefixFieldSetter(o,genFieldName(f),pfx,genFieldName(pf));
        }
      }
    }
  }

  void genPrefixFieldGetterClearer(ostream &o, const gc_string &pfx, gc_string fname)
  {
    gc_string gname = fname + "_getter";
    o << pfx <<  " void reset_" << gname <<  "() {\n";
    o <<  "_" << gname << " =  0;\n";
    o << "}\n\n";
  }

  void genPrefixFieldClearer(ostream &o, const InstrField &f, const gc_string &pfx, gc_string fname)
  {
    o << pfx <<  " void reset_" << fname <<  "() {\n";
    o <<  "_" << fname << " = " << (f.has_default_value() ? uitogs(f.default_value()) : "0") << ";\n";
    o << "}\n\n";
    
  }
  void genPrefixFieldClearers(ostream &o, const Core &core, const gc_string &pfx) 
  {
    const InstrFieldHash &instrfields = core.instrfields();
    o << "//\n// Prefix field reset functions. \n//\n\n";
    ForEach(instrfields,i) {
      const InstrField &f = *(i->second);
      if (!f.is_prefix() || !f.is_pseudo() || f.is_variable() || f.field_type()) { // aliases are not cleared
        continue;
      }
      if (is_multiblock(f)) {
        ForEach(f.blks(),b) {
          genPrefixFieldClearer(o,f,pfx,genFieldName(f,*b));
          genPrefixFieldGetterClearer(o,pfx,genFieldName(f,*b));
        }

      } else {
        genPrefixFieldClearer(o,f,pfx,genFieldName(f));
      }
    }

    o << "//\n// Reset All prefix fields functions.\n//\n\n";
    o << pfx << " void  reset_prefix() {\n";
    ForEach(instrfields,i) {
      const InstrField &f = *(i->second);
      if (!f.is_prefix() || !f.is_pseudo() || f.is_variable() || f.field_type()) {
        continue;
      }
      if (is_multiblock(f)) {
        ForEach(f.blks(),b) {
          o << " reset_" << genFieldName(f,*b) << "();\n";
          o << " reset_" << genFieldName(f,*b) << "_getter();\n";
        }
      } else {
        o << " reset_" << genFieldName(f) << "();\n";
      }
    }    
    o << "}\n\n";
  }

  void getPrefixField(ostream &o, const InstrField &f, const gc_string &args, const gc_string &blk) 
  {
    const InstrField &tf = (f.interface() ? *f.interface() : f);
    o << "get_" << genFieldName(tf, blk) << "(" << args << ")";
  }

  void genPrefixFieldGetter(ostream &o, const Core& core, const InstrField &f, const gc_string &pfx,
                            gc_string fname, unsigned instr_width, AdlTranslatingWalker *walker,
                            bool c_plus_plus,bool action_ok)
  {
    // This getters are to read the data form the field
    if (f.is_pseudo()) {
      if (!c_plus_plus) {
        return;
      }
      string ix = uitoa(f.index());
      ostringstream os;
      
      if (index_field_endianness(core,f) == InstrField::Little) {
        os << "(Index+1)*" << ix << "-1, Index*" << ix;
      } else {
        os << "Index*" << ix << ", (Index+1)*" << ix << "-1";
      }

      if (f.index() != 0) {	
        unsigned size = (f.size() <= 32 ? 32 : 64);
        if (f.action()) {
          if (action_ok) {
            IFieldWalker iw(f.env(),f,size);
            // First, if we have an additional walker, translate using it first.
            Ptree *action = get_func_body(f.action());
            if (walker) {
              action = walker->Translate(action);
            }
            o << "unsigned get_" << fname << "(" << get_func_args(f.action()) <<") {\n";
            o << "bits<" << f.width() << ">  z0 = _" << fname << ";\n";
            o << iw.Translate(action) << "\n";
          } else {
            // If action-code isn't OK, then we just return 0 by default.
            o << "unsigned get_" << fname << "(unsigned Index) {\n";
            o << "return 0;\n";
          }
        } else if (f.field_type() || f.blks().size() <= 1) {
          o << "unsigned get_" << fname << "(unsigned Index) {\n";
          o << "return _" << fname << ".get(" << os.str() << ").uint32();\n";
        } else {
          o << "unsigned get_" << fname << "(unsigned Index) {\n";
          o << "return (_" << fname << "_getter ? (this->*_" << fname << "_getter)(Index) :"
            << (f.has_default_value() ? uitogs(f.default_value()) : "0") << ");\n";
        }
        o << "}\n\n";
      } else {
        o << "unsigned read_" << fname << "() {\n";
        o << "return _" << fname << ".uint32();\n";
        o << "}\n\n";
      }
      
    } else { // This is the  standard read_field 
      o << pfx << " " << data_type(f) << " get_" << f.name();
      if (core.is_little_endian()) {
        o << "_width_" << instr_width;
      }
      
      genFuncArgs(o,core.instr_width()/32);
      o << "return ";
      writeFieldValue(o,f,"x",(core.instrEndianness() == Core::Little),core.instr_width(),instr_width);
      o << "\n}\n\n";
    }
  }

  void genPrefixFieldGetters(ostream &o, const Core &core, const gc_string &pfx, 
                             AdlTranslatingWalker *walker, bool c_plus_plus, bool action_ok) 
  {
    const InstrFieldHash &instrfields = core.instrfields();
    o << "//\n// Prefix field getter functions. \n//\n\n";
    ForEach(instrfields,i) {
      const InstrField &f = *(i->second);
      if (!f.is_prefix()) {
        continue;
      }
      
      gc_set<unsigned> widths = instr_widths(f,core);
      ForEach(widths,w) {
        if (is_multiblock(f) && f.is_pseudo()) {
          ForEach(f.blks(),b) {
            genPrefixFieldGetter(o,core,f,pfx,genFieldName(f,*b),*w,walker,c_plus_plus,action_ok);
          }
        } else {
          genPrefixFieldGetter(o,core,f,pfx,genFieldName(f),*w,walker,c_plus_plus,action_ok);
        }
        if (f.is_pseudo()) {
          break;
        }
      }	
    }
  }
  
  void genEntryName(ostream &o,const RegEntry& entry,const RegFile &rf) {
    if (!entry.syntax().empty()) {
      // If entry has syntax just print it.
      o << entry.syntax();
    } else {
      // or print register name or register file name with index
      const RegName& rn = entry.reg();
      if (rn.regfile()) {
        // If it is a regfile, we also print out the index
        if (rn.regfile()->prefix().empty()) {
          o << rn.regfile()->log_name();
        } else {
          o << rn.regfile()->prefix();
        }
        o << rn.index();               
      } else if (rn.reg()) {
        // Register -> just its name
        o << rn.reg()->name();
      } else {
        // Not a clear reference, action defined by hooks, we cannot decode it so just print some gc_string.
        o << rf.name() << entry.id();
      }
    }
  }
  
  void genDisasmHelpers(ostream &o,const Core &core,const gc_string &pfx) {
    o << "//\n// Disassembler helpers\n//\n\n";
    ForEach(core.regfiles(),i) {
      const RegFile &rf = *(i->second);
      if (rf.sparse()) {
        o << pfx << " const char* " << rf.name()  << "_disasm(unsigned x) {\n";
        o << "switch (x) {\n";
        ForEach(rf.entries(),e) {
          o << "case " << e->first << ": return \"";
          genEntryName(o,*(e->second),rf);
          o << "\";\n";	   
        }
        // In fact that cannot happen, just for debug.
        o << "default: return \"reserved\";\n}\n}\n\n";	
      }
    }
  } 

  bool is_special(const InstrField &field)
  {
    return (field.is_signed() || field.offset() || field.is_inverted() || field.shift());
  }

  int getInstrFieldWidth(const InstrField &field,unsigned nia_width,bool orig) 
  {
    int type = 0;
    if (is_special(field) && !orig) {
      type = field.width();
      if (field.shift()) {
        type += field.shift();
      }
      // We could tighten up the width for the offset by figuring out the true
      // maximum value, but that seems like more trouble than it's worth.
      if (field.is_signed() || field.offset()) {
        type = nia_width;
      }
      if (field.addr() == InstrField::aPcRel || field.addr() == InstrField::aAbs) {
        type = nia_width;
      }
    } else {
      const InstrField *ifc   = field.interface() ? field.interface() : &field;
      type = (ifc->index() ? ifc->width() : ifc->size());
    }
    return type;
  }

  gc_string getInstrFieldType(const InstrField &field,unsigned nia_width,bool orig,bool use_signed) 
  {
    gc_ostringstream ss;            

    if (use_signed && field.is_signed()) {
      ss << "s";
    }
    ss << "bits<" << getInstrFieldWidth(field,nia_width,orig) << ">";
    return ss.str();
  }


}   

  

 

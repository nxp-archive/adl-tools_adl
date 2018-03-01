// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Entry point for the ISS generation client.
//

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>

#include "opencxx/parser/Ptree.h"
#include "opencxx/parser/PtreeUtil.h"
#include "opencxx/parser/ptree-generated.h"
#include "opencxx/TypeInfo.h"
#include "opencxx/Environment.h"

#include "helpers/pfxstream.h"
#include "helpers/FilenameUtils.h"
#include "helpers/AnyOption.h"
#include "helpers/Macros.h"
#include "helpers/CodeGen.h"
#include "helpers/RtlWalker.h"

#include "fe/Helpers.h"
#include "fe/FieldSupport.h"
#include "fe/DecodeSupport.h"
#include "fe/AdlParser.h"
#include "fe/Data.h"
#include "fe/InstrTable.h"

#include "GenAsm.h"

using namespace std;
using namespace Opencxx;
using namespace PtreeUtil;
using namespace rnumber;

// A helper used in InstrInfo() construction, below.
static bool fieldIsEnumerated (const adl::Instr::Field &ifield)
{
  // Ignore assembler fields, since they're not involved in the direct assembly
  // process.
  return !ifield._field->is_assembler() && ifield._field->is_enumerated();
}

namespace adl {

  // Used for identifiying expression modifiers and the indices that they used
  // (expressions used within a shorthand instruction).
  struct ModItem {
    Ptree *_name;
    IntSet _indices;

    ModItem() : _name(0) {};
    ModItem(Ptree *n,const IntSet &s) : _name(n), _indices(s) {};
  };

  typedef gc_vector<ModItem> ModItemVect;
  
  // Width of field above which we'll switch to hex radix by default.
  const unsigned DecCutoff = 5;
  // The field is not consrainted, i.e, empty InstFieldTable
  const int      SimplFld  = -1;

  static const gc_string DefaultTableName = "default";
  static const gc_string CiaVar           = "__cia";

  extern const char *DefaultInstrTable;
  extern const char *PrefixInstrTable;

  static Ptree *Zero = Ptree::Make("0");

  static const gc_string ws = " *";

  typedef gc_map<const Instr *,gc_string> InstrStrMap;


  // Gets the function's body after translating it.  Always use this function,
  // rather than calling the walker directly, because there's a subtlety of
  // usage here: You must translate the entire function so that the functions
  // arguments are properly recorded.  This is required in order to translate
  // bits objects correctly when slices are taken: If the arguments are not
  // recorded, then we don't know their types.
  Ptree *funcBody(Walker &walker,Ptree *f)
  {
    return get_func_body(walker.Translate(f));
  }
  
  gc_string getEnumName(const Instr &instr)
  {
    return "instr_" + makeFriendlyName(instr.name());
  }

  gc_string getInstrTableType(const gc_string &pfx)
  {
    return pfx + "_InstrTables";
  }

  gc_string getInstrType(const gc_string &pfx)
  {
    return pfx + "_InstrType";
  }

  gc_string getInstrInfoType(const gc_string &pfx)
  {
    return pfx + "_InstrInfo";
  }

  gc_string getDbgRegsName(const gc_string &pfx)
  {
    return pfx + "_regs";
  }

  // An instruction's name, from the assembler's point of view, comes from the
  // syntax string, if one is given, otherwise we use the declared name.
  gc_string getInstrName(const Instr &instr)
  {
    if (const Instr::Syntax *syntax = instr.syntax()) {
      return lowercase(syntax->getName());
    } else {
      return lowercase(instr.name());
    }
  }

  gc_string genArgs(const gc_string& iname, const char* sep = ", ",unsigned num_words = 1,bool pointer_arg = false)  
  {
    gc_ostringstream o;
    ForRange(num_words,i) {
      o << iname;
      if (pointer_arg) {
        o << '[' << i << ']';
      } else {
        o << i;
      }
      if ( i != num_words - 1) {
        o << sep;
      }
    }
    return o.str();
  }

  gc_string genFieldName(const InstrField &field, const gc_string &sfx,int idx=SimplFld) 
  {
    gc_string fname;
    if (auto ifc = field.interface()) {
      fname = ifc->name();
    } else {
      fname = field.name();
    }
    if (idx > SimplFld) {
      gc_ostringstream ss;
      ss << fname << "__" << idx;
      return ss.str();
    }
    if (auto encp = field.encap_field()) {
      fname = encp->name() + "__" + fname;
    }
    fname += sfx;
    return fname;
  }

  gc_string genGetField(const Core &core,const InstrField &field,const gc_string &sfx,unsigned instr_width,const gc_string &iname = "insn",
                        int idx=SimplFld,bool pointer_arg = false)
  {

    unsigned num_words = core.instr_width()/32;
    gc_ostringstream ss;

    if (idx == SimplFld) {
      ss << "get_";
      if (field.encap_field()) {
        ss << field.encap_field()->name() << "__";
      }
      ss << field.name();
      if (instr_width) {
        ss << "_width_" << instr_width;
      }
      ss << "(" << genArgs(iname,", ",num_words,pointer_arg) << ")";
      return ss.str();
    }
    gc_string  fname = field.interface() ? field.interface()->name() : field.name();
    ss << makeInstrFldTblAccessName(field) << "(" << fname << sfx << ".uint32()," << idx << ")";
    return ss.str();
  }

  gc_string genGetModifiedField(const Core &core,const InstrField &field,const gc_string &sfx,unsigned instr_width,bool library_mode,
                                const gc_string &iname = "insn", int idx=SimplFld,bool pointer_arg = false)
  {
    gc_string value = genGetField(core,field,sfx,instr_width,iname,idx,pointer_arg);

    unsigned width = field.width();

    if (field.is_signed()) {
      value = value + "." + ((library_mode) ? "template " : "") + "signExtend<" + uitogs(core.nia()->width()) + ">()";
      width = core.nia()->width();
    }

    // For efficiency, we resize via signExtend before shifting.  However, if we didn't
    // have to sign-extend, then we have to expand the value for the shift.
    if (field.shift()) {
      if (!field.is_signed()) {
        width += field.shift();
        value = "bits<" + uitogs(width) + ">(" + value + ")";
      }
      value = "(" + value + " << " + uitogs(field.shift()) + ")";
    }

    // Same for the offset- expand the value if necessary, resize and add on the
    // offset.
    if (field.offset()) {
      if (!field.is_signed()) {
        width = core.nia()->width();
        value = "bits<" + uitogs(width) + ">(" + value + ")";
      }
      value = "(" + value + " + " + uitogs(field.offset()) + ")";
    }

    return value;
  }

  bool hasMultipleTables(const Core &core)
  {
    return (core.instrtables().size() > 1) && core.asm_config().disasm_instrtables().empty();
  }

  gc_string getTableSelect() {
    return "internal_current_table";
  }

  // If the instruction has preceding fields, then record the possible
  // enumeration values.
  void getInstrPfxFields(StrSet &instr_pfx_fields, const Instr &instr)
  {
    if (const Instr::Syntax *syntax = instr.syntax()) {
      ForEach(syntax->_fmt,i) {
        if (isspace(*i)) {
          continue;
        }
        else if (syntax->isPfield(*i)) {
          const Instr::Field &fp = syntax->get(*i);
          const InstrField &field = *(fp._field);

          // Insert all possible values.
          ForEach(field.enums(),i) {
            const gc_string &v = i->first;
            if (v.empty()) continue;
            instr_pfx_fields.insert(v);
          }
        }
        else {
          break;
        }
      }
    }
  }

  unsigned getFmtSize(const Instr &instr) {
    unsigned ret(0);
    if (const Instr::Syntax *syntax = instr.syntax()) {
      for (gc_string::const_iterator i = syntax->_fmt.begin()+syntax->_in_end; i != syntax->_fmt.end(); ++i) {
        if(!syntax->isfield(*i) && !isspace(*i)) {
          ++ret;
        }
      }
    } else {
      return getInstrName(instr).size();
    }
    return ret;
  }


  int refFieldCount(const Instr::Alias &a)
  {
    int count = 0;
    ForEach(a._fields,i) {
      if ( i->_type == Instr::Field::Ref) {
        ++count;
      }
    }
    return count;
  }

  int exprFieldCount(const Instr::Alias &a)
  {
    int count = 0;
    ForEach(a._fields,i) {
      if ( i->_type == Instr::Field::Expr ) {
        ++count;
      }
    }
    return count;
  }

 gc_string addSignExt(const gc_string &value,int width,bool use_cpp)
  {
    gc_ostringstream ss;
    gc_string mask;
    gc_string inv_value;
    gc_string pfx("");
    gc_string sgn,unsgn;
    if (!use_cpp) {
      sgn = "long long";
      unsgn = "unsigned long long";
    } else {
      sgn = "int64_t";
      unsgn = "uint64_t";
    }
    if (width < 32) {
      mask = "(1 << " + uitogs(width-1) + ")";
      inv_value  = "(" + value + "- (1 << " + uitogs(width) +"))";
    } else if (width < 64) {
      mask = "((" + unsgn + ")1 << " + uitogs(width-1) + ")";
      inv_value  = "(" + value + "- ((" + unsgn + ")1 << " + uitogs(width) +"))";
    } else {
      mask = "((" + unsgn + ")1 << 63))";
      inv_value = "((" + sgn + ")" +  value + ")";
    }
    ss << "((" << value << " & " << mask << ") ? " << inv_value << ":" << value << ")";

    return ss.str();
  }

 
  uint32_t get_submask(RNumber mask) {
    uint32_t ret = 0;
    while(mask > 0) {
      if ((ret = mask.uint32()) > 0) {
        return ret;
      }
      mask >>= 32;
    }
    return ret;
  }

  unsigned get_index(RNumber mask) {
    unsigned ret = 0;
    unsigned sz  = mask.size()/32  - 1;
    while(mask > 0) {
      if (mask.uint32() > 0) {
        return (sz - ret);
      }
      mask >>= 32;
      ret++;
    }
    return ret; 

  };

  unsigned get_subval(int ind,const RNumber& mask) {
    assert((unsigned) (ind+1)*32 <= mask.size());
    return mask.getUIntField(ind*32,ind*32 + 31);    
  }

  // x < y if x has fewer reference fields than y.  We want those to be first,
  // since they're more specific.  We also give preference to items with more
  // expressions, so those are also more specific.
  struct SortBySpecificity {
    bool operator()(const Instr *x,const Instr *y)
    {
      // This is only valid for aliases (shorthands);
      const Instr::Alias *x_a = x->get_alias();
      const Instr::Alias *y_a = y->get_alias();
      assert(x_a && y_a);

      unsigned xr = refFieldCount(*x_a);
      unsigned yr = refFieldCount(*y_a);

      if (xr != yr) {
        return (xr < yr);
      }
      
      // If same number of reference fields, then count number of expressions.
      // We want more expressions first, since that implies higher specificity.
      return ( exprFieldCount(*x_a) > exprFieldCount(*y_a) );
    }
  };

  string getAttrName(const string &n) 
  {
    return n;
  }
  
  string getAttrName(unsigned id) 
  {
    return getAttrName(Data::get_attr_name(id));
  }

  typedef pair<const RegBase*,unsigned> DbgRegItem;
  typedef gc_vector<DbgRegItem> DbgRegItems;

  void recordDbgRegItem(int dbgattr,DbgRegItems &reg_items,const RegBase *rb)
  {
    if (rb->has_attr(dbgattr)) {
      auto iter = rb->attrs().find(dbgattr);
      assert(iter != rb->attrs().end());
      auto &ap = iter->second;
          
      unsigned order = (ap._type == AttrParams::Int) ? ap._int_list.front() : 0;
          
      reg_items.push_back(make_pair(rb,order));
    }
  }
  
  void checkForDebugRegs(DbgRegList &debug_regs,const Core &core)
  {
    int dbgattr = Data::get_attr("debug");
    if (dbgattr >= 0) {
      // Scan registers to see if we've listed any with the attribute 'debug'.  If
      // so, then we create a list of pointers to these elements.  If the
      // attribute has an ordering value, record that as well.  We'll sort this
      // list, then add in the items in sorted order.
      DbgRegItems reg_items;

      for (auto &i : core.regfiles()) {
        const RegFile *rf = i.second;

        recordDbgRegItem(dbgattr,reg_items,rf);
      }

      for (auto &i : core.regs()) {
        const Reg *r = i.second;

        recordDbgRegItem(dbgattr,reg_items,r);
      }

      sort(reg_items.begin(),reg_items.end(),[](const DbgRegItem &x,const DbgRegItem &y) {
          return x.second < y.second;
        });
    
      for (auto &i : reg_items) {
        if (const RegFile *rf = dynamic_cast<const RegFile*>(i.first)) {
          // For register files, list out the names of all entries by index.
          // If sparse, then list out the names of the constituent registers.
          if (rf->sparse()) {

            ForEach(rf->entries(),j) {
              auto &rn = j->second->reg();

              if (const Reg *r = rn.reg()) {
                debug_regs.push_back(DbgRegElm(r->name(),r->width()));
              } else if (const RegFile *r = rn.regfile()) {
                if (rn.has_index()) {
                  debug_regs.push_back(DbgRegElm(r->name(),r->prefix(),rn.index(),r->width()));
                }
              }
            }

          } else {
            for (unsigned c = 0; c != rf->size(); ++c) {
              debug_regs.push_back(DbgRegElm(rf->name(),rf->prefix(),c,rf->width()));
            }
          }
        } else if (const Reg *r = dynamic_cast<const Reg*>(i.first)) {
          debug_regs.push_back(DbgRegElm(r->name(),r->width()));
        }
      }
     
    }
  }

  struct DisExprWalker : public Walker {
    DisExprWalker(Environment *env,const Core &core) : Walker(env), _core(core) {}

    virtual Ptree *TranslateVariable(Ptree *exp)
    {
      auto f = exp->ToString();
      if (_core.get_instrfield(f)) {
        return Ptree::Make("%p.uint32()",exp);
      } else {
        return exp;
      }
    }

    Ptree* TranslatePtree(Opencxx::Ptree *var)
    {
      if (var->IsLeaf()) {
        auto f = var->ToString();
        if (_core.get_instrfield(f)) {
          return Ptree::Make("%p.uint32()",var);
        } else {
          return var;
        }
      } else {
        Ptree *car = Translate(var->Car());
        Ptree *cdr = Translate(var->Cdr());
        if (car != var->Car() || cdr != var->Cdr()) {
          return PtreeUtil::Cons(car,cdr);
        }
      }
      return Walker::TranslatePtree(var);
    }
  protected:
    const Core &_core;
  };

  // Common code for handling hooks.
  class AsmActionWalker : public RtlWalker {
  public:
    AsmActionWalker(Environment *env) : RtlWalker(env,false,false) {}

    // These should probably never be called, bu they're pure virtual, so we
    // have to define them anyway.
    virtual Ptree *TranslateRegWrite(PtreeRegWrite *exp)
    {
      return Translate(exp->Car());
    }
    
    virtual Ptree *TranslateRegRead(PtreeRegRead *exp)
    {
      return Translate(exp->Car());
    }

    virtual Ptree *TranslateMemWrite(PtreeMemWrite *exp)
    {
      return Translate(exp->Car());
    }
    
    virtual Ptree *TranslateMemRead(PtreeMemRead *exp)
    {
      return Translate(exp->Car());
    }

    virtual Ptree *TranslateSubInstrCall(PtreeSubInstrCall *exp)
    {
      return Translate(exp->Car());
    }

    virtual Ptree* TranslateStmts(Ptree* block)
    {
      Ptree *block2 = AdlTranslatingWalker::TranslateBrace(block);
      if (block != block2) {
        return new PtreeStmts(block2->Car(),block2->Cdr());
      } else {
        return block2;
      }
    }  

  };

  struct GenBase : public gc {

    const Core &_core;

    gc_string   _archpfx;       // Architecture prefix.

    gc_string   _arch_name;
    gc_string   _arch_print_name;

    bool        _keep_ws;       // True => Keep all whitespace in syntax strings.
    bool        _use_cpp;
    bool        _library_mode;
    gc_string   _filename;      // For logging purposes.
    gc_string   _log_usage;     // Usage logging data.  
                                //   Empty or "false" or "no":  No logging.
                                //   "true":         Log using default info.
                                //   <ident>:<ver>:  Log using that.
                                //   <ident>:        Log using the program name and a default version.

    GenBase(const Core &core,bool use_cpp,bool library_mode,const gc_string &fname,const gc_string &log_usage) : 
      _core(core),
      _keep_ws(false),
      _use_cpp(use_cpp),
      _library_mode(library_mode),
      _filename(fname),
      _log_usage(log_usage)
    {};
   
    // Generate a usage-logging call, if applicable.
    gc_string genUsageLogging(ostream &o,const char *type,const gc_string &default_ver = gc_string())
    {
      gc_string progname, version;
      createModelVersion(progname,version,_filename,_log_usage);
      if (!default_ver.empty()) {
        version = default_ver;
      }

      if (_log_usage != "false" && _log_usage != "no") { 
        // Note: We're not currently logging at exit b/c there seems to be some
        // weirdness going on when we do.  We seem to get strange crashes in
        // exit.  So, we'll just log immediately with a fake exit code, or now.
        o << "static bool dummy ATTRIBUTE_UNUSED = adl::log_usage(\"" << progname << "\",\"" << version << "\",0,\"" << type << "\",\"make-asm\");\n";
      }
      return version;
    }

    unsigned get_reloc_size_code(unsigned width_in_bytes)
    {
      // The following reverses the mapping found in binutils bfd_get_reloc_size.
      switch (width_in_bytes) {
      case 1: return 0;
      case 2: return 1;
      case 3: return 3;
      case 4: return 2;
      case 0: return 3;
      case 8: return 4;
      case 16: return 8;
      default: return 0;
      }
    }

    typedef vector<const Relocation *> Relocs;

    static bool sort_relocations_by_name(const Relocation *x,const Relocation *y)
    {
      return x->asmstr() < y->asmstr();
    }

    unsigned reloc_instr_width(const InstrField &field,const Instr *instr,unsigned width)
    {
      if (_core.is_big_endian()) {
        // For big-endian, it's easy- just add on the lowbit of the field to the
        // width of the relocation itself and we're good.
        return width + field.lowbit();
      } else {
        // For little-endian, we need the instruction with which this is associated.
        assert(instr);
        return instr->width();
      }
      return width;
    }

    // Generates an array of howto structs for relocations.  We need this to be
    // a sequence sorted by the name used, which can be either the real name or
    // the abbreviation.
    void genRelocHowtos(ostream &o, const Relocs &relocs)
    {
      o << "\nstatic reloc_howto_type " << _archpfx << "_elf_howto_table[] = {\n";
      ForEach(relocs,i) {
        const Relocation &reloc = **i;
        bool pcrel = reloc.pcrel();
        unsigned width = reloc.field_width();

        unsigned size = get_reloc_size_code((width-1) / 8 + 1);         
        unsigned rs = reloc.right_shift();
        unsigned bitpos = reloc.bitpos();
        o << "HOWTO(" << reloc.value() << "," << rs << "," << size << "," << width << "," << pcrel << "," << bitpos << ",";
        if (reloc.check_overflow()) {
          // If the relocation is associated with a field, is signed, and
          // doesn't contain a right shift, then consider it to be a bitfield
          // for overflow checking.
          if (!reloc.field() && !reloc.right_shift() && reloc.is_signed()) {
            o << "complain_overflow_bitfield";
          } else if (reloc.is_signed()) {
            o << "complain_overflow_signed";
          } else {
            o << "complain_overflow_unsigned";
          }
        } else {
          o << "complain_overflow_dont";
        }
        // Specifies the special function for reloc_howto_type
        o << ",bfd_elf_generic_reloc,\"" << reloc.name() << "\",0,0x" << hex << reloc.src_mask() 
          << ",0x" << reloc.dst_mask() << dec << "," << pcrel << "), // relocation " << reloc.name() << " (" << reloc.asmstr() << ")\n";
      }
      o << "EMPTY_HOWTO(-1)\n"
        << "};\n\n"
        << "static unsigned num_" << _archpfx << "_elf_howtos ATTRIBUTE_UNUSED = " << _core.relocs().size()+1 << ";\n";
    }

    // Generates the relocation "howto" array, which just includes enough info
    // in the howto structs to be able to print proper disassembly info.  It
    // then creates a function for returning the relevant howto pointer given a
    // relocation value.
    void genRelocInfo(ostream &o)
    {
      Relocs relocs;
      ForEach(_core.relocs(),i) {
        relocs.push_back(i->second);
      }
      
      genRelocHowtos(o, relocs);

      o << "\nextern \"C\" reloc_howto_type *adl_elf_info_to_howto(unsigned r)\n"
        << "{\n"
        << "switch (r) {\n";

      int count = 0;
      ForEach(_core.relocs(),i) {
        const Relocation &reloc = *i->second;
        o << "case " << reloc.value() << ":\n"
          << "return &" << _archpfx << "_elf_howto_table[" << count << "];\n";
        ++count;
      }

      o << "}\n"
        << "return 0;\n"
        << "}\n\n";
    }

    // This is ugly- if we're little-endian, then we need to get a width, but we
    // don't know the instruction used.  So, this only works if the field is
    // used with just one instruction-width.  Otherwise, we'll generate an
    // error.  Maybe, in the future, we could generate multiple relocation
    // sub-names or something.
    enum AccessorType { rGet, rSet, rClear };
    
    gc_string getRelocAccessorName(const InstrField &field,const Instr *instr,AccessorType type)
    {
      gc_string pfx;
      switch (type) {
      case rGet:
        pfx = "get_";
        break;
      case rSet:
        pfx = "set_";
        break;
      case rClear:
        pfx = "clear_";
        break;
      }
      
      gc_string base = pfx + field.name();      

      if (_core.is_little_endian()) {
        assert(instr);
        return base + "_width_" + uitogs(instr->width());
      } else {
        return base;
      }
    }

    gc_string getRelocActionName(const Relocation &reloc)
    {
      return reloc.name() + "_action";
    }

    // Generate relocation action functions, if any exist.
    void genRelocActions(ostream &o)
    {
      AsmActionWalker walker(_core.env());

      ForEach(_core.relocs(),i) {
        const Relocation &reloc = *i->second;

        if (reloc.action()) {

          Ptree *func = walker.Translate(reloc.action());

          o << "addr_t " << getRelocActionName(reloc) << "(" << get_func_args(reloc.action()) << ")"
            << get_func_body(func) << "\n\n";
        }
      }
    }

    unsigned reloc_get_offset(const Relocation &reloc)
    {
      if (reloc.offset() >= 0) {
        return reloc.offset();
      } else if (reloc.field()) {
        const InstrField &field = *reloc.field();
        unsigned bitpos = 0;
        if (_core.is_little_endian()) {
          assert(reloc.instr());
          return reloc.instr()->width() - field.highbit() - 1;
        } else {
          bitpos = field.lowbit();
        }
        return bitpos;
      } else {
        return 0;
      }
    }

    gc_string getRelocSetterName(const Relocation &reloc)
    {
      return reloc.name() + "_set";
    }

    void genRelocSetterFuncs(ostream &o,const gc_string &pfx)
    {
      // First, generate the set functions.  This clears a field, then inserts
      // the appropriate value.
      ForEach(_core.relocs(),i) {
        const Relocation &reloc = *i->second;
        gc_string data_array = "data";
        if (reloc.field()) {
          o << "static void " << getRelocSetterName(reloc) << " (unsigned *data,unsigned size,int big_endian,bfd_uint64_t addr)\n"
            << "{\n";

          unsigned shift = 0;
          if (reloc.macro_op_field()) {
            assert(!reloc.macro_op_field()->is_pseudo());
            if (_core.is_little_endian()) {
              if (reloc.instr()->width() > 32) {
                shift = (64 - reloc.instr()->width());
              } else {
                shift = (32 - reloc.instr()->width());                
              }
            }
            
            data_array = "micro_data";
            // If we have a macro-op field, then we have to extract the inner
            // instruction first, then insert the relocation, then place the
            // updated instruction back into the macro-op field.  This gets
            // ugly- we have to right justify the result for everything to now
            // work properly.
            o << "bfd_vma x = " << pfx << getRelocAccessorName(*reloc.macro_op_field(),reloc.macro_op_instr(),rGet) << "(data[0],data[1]).uint64() << " << shift << ";\n"
              << "unsigned " << data_array << "[4];\n"
              << "bfd_vma y = 0;\n"
              << "populate_data_array(" << data_array << ",x,y,size,big_endian);\n";
          }
          
          o << pfx << getRelocAccessorName(*reloc.field(),reloc.instr(),rClear) << "(" << data_array << ");\n"
            << pfx << getRelocAccessorName(*reloc.field(),reloc.instr(),rSet) << "(" << data_array << ",addr);\n";

          if (reloc.macro_op_field()) {
            o << "depopulate_data_array(&x,&y," << data_array << ",size,big_endian);\n";
            if (shift) {
              o << "x >>= " << shift << ";\n";
            }
            // Clear the macro instruction first, since we are updating the
            // field and don't want any residual bits from the addend.
            o << pfx << getRelocAccessorName(*reloc.macro_op_field(),reloc.macro_op_instr(),rClear) << "(data);\n"
              << pfx << getRelocAccessorName(*reloc.macro_op_field(),reloc.macro_op_instr(),rSet) << "(data,x);\n";
          }
          
          o << "}\n\n";
        }
      }
    }

    // Simple helper for integer division with rounding-up behavior.
    unsigned div_rnd_up(unsigned dividend,unsigned divisor)
    {
      return (dividend + divisor - 1) / divisor;
    }
    
    // Generates the relocation -> setter mapping function.  This maps
    // relocation types to setter functions.
    void genRelocSetters(ostream &o,const gc_string &pfx)
    {
      o << "typedef void (*reloc_setter)(unsigned *,unsigned,int,bfd_uint64_t);\n"
        << "typedef bfd_uint64_t (*reloc_action)(bfd_uint64_t,int);\n\n";

      genRelocSetterFuncs(o,pfx);
            
      o << "extern \"C\" void adl_get_reloc_funcs(unsigned r,reloc_setter *rs,reloc_action *ra,int *size,int *inner_size,int *offset)\n"
        << "{\n"
        << "switch (r) {\n";

      ForEach(_core.relocs(),i) {
        const Relocation &reloc = *i->second;

        o << "case " << reloc.value() << ":\n";
        if (reloc.field()) {
          // We shouldn't have a pseudo-field here- the front-end should have
          // taken care of that.
          assert(!reloc.field()->is_pseudo());
          
          o << "*rs = " << getRelocSetterName(reloc) << ";\n";
          // If this relocation is associated with a field, then we're going to
          // use its setter, so we have to add on any extra width associated
          // with where the field is in the instruction (for big-endian) or
          // actually find the relevant instruction for little-endian.
          unsigned width = reloc_instr_width(*reloc.field(),reloc.instr(),reloc.field_width());
          if (reloc.macro_op_field()) {
            unsigned macro_op_width = reloc_instr_width(*reloc.macro_op_field(),reloc.macro_op_instr(),0);
            // Make sure to round up, so that, e.g. a 36-bit field's inner-size is recorded as 5 rather than 4.
            o << "*size = " << div_rnd_up(macro_op_width,8) << ";\n"
              << "*inner_size = " << div_rnd_up(width,8) << ";\n";            
          } else {
            o << "*size = " << div_rnd_up(width,8) << ";\n"
              << "*inner_size = 0;\n";
          }
          // Then we have to record an offset for recovering the original
          // instruction location, so that we can use our setter function.
          unsigned offset = reloc_get_offset(reloc);
          o << "*offset = " << offset/8 << ";\n";
        }
        if (reloc.action()) {
          o << "*ra = " << getRelocActionName(reloc) << ";\n";
        }
        
        o << "break;\n";
      }
      
      o << "}\n"
        << "}\n\n";
    }

    // Generate an architecture block, used by the BFD library for low-level
    // functionality such as handling relocations.
    //
    // Note: Still a bit of a hack, since we identify this as a PowerPC.  We
    // probably need to fix this at some point!
    void genArchInfo(ostream &o) 
    {
      // We derive the width of the machine from the size of the program
      // counter: It's either 32-bit or 64-bit.
      unsigned width = (_core.cia()->width() > 32) ? 64 : 32;

      if (_arch_name.empty()) _arch_name = _archpfx;
      if (_arch_print_name.empty()) _arch_print_name = _arch_name;

      o << "const char *adl_arch_name = \"" << _arch_print_name << "\";\n\n"

        << "extern \"C\" const bfd_arch_info_type bfd_adl_arch =\n"
        << "{\n"
        << width << ", /* number of bits in a word. */\n"
        << width << ", /* number of bits in an address. */\n"
        << "8, /* number of bits in a byte. */\n"
        << "bfd_arch_powerpc,\n"
        << ((width == 32) ? "bfd_mach_ppc" : "bfd_mach_ppc64") << ",\n"
        << "\"" << _arch_name << "\",\n"
        << "\"" << _arch_print_name << "\",\n"
        << "3,\n"
        << "TRUE,\n"
        << "bfd_default_compatible,\n"
        << "bfd_default_scan,\n"
        << "bfd_arch_default_fill,\n"
        << "0\n"
        << "};\n\n";
    }

  };

  
  class DefaultDisassemblerCall : public DisassemblerCallIF
  {
  public:
    DefaultDisassemblerCall(bool dis_micro_ops) : _dis_micro_ops(dis_micro_ops) {};

    bool disassembleMicroOps() const override { return _dis_micro_ops; };
    
    void operator()(ostream &o,const gc_string &arg,const gc_string &table_type,bool has_multiple_tables) override
    {
      if (!has_multiple_tables || table_type.empty()) {
        o << "disassemble_insn(memaddr," << arg << ",info);\n";
      } else {
        o << "{\n"
          << "TableType orig_table = getCurrentInstrTable();\n"
          << "setCurrentInstrTable(" << table_type << ");\n"
          << "disassemble_insn(memaddr," << arg << ",info);\n"
          << "setCurrentInstrTable(orig_table);\n"
          << "}\n";
      }
    }

    bool _dis_micro_ops;
  };
  
  // Class for generating the guts of the disassembler.
  struct DisassemblerImpl {

    const Core &_core;

    bool        _use_cpp;
    bool        _library_mode;
    bool        _gen_pfx_logic;

    size_t      _namesize;

    const gc_string &_iname;
    const gc_string &_fname;
    const gc_string &_pkt_pos;

    DisassemblerCallIF &_discall;


    DisassemblerImpl(const Core &core,bool use_cpp,bool library_mode,bool gen_pfx_logic,
                     size_t namesize,const gc_string &iname,const gc_string &fname,
                     const gc_string &pkt_pos,DisassemblerCallIF &discall) :
      _core(core),
      _use_cpp(use_cpp),
      _library_mode(library_mode),
      _gen_pfx_logic(gen_pfx_logic),
      _namesize(namesize),
      _iname(iname),
      _fname(fname),
      _pkt_pos(pkt_pos),
      _discall(discall)
    {}
    
    virtual ~DisassemblerImpl() {};

    // The default recursive call maps to gnu binutil's format.
    void writeDisasmCall(ostream &o,const gc_string &arg,const gc_string &table_type)
    {
      _discall(o,arg,table_type,hasMultipleTables(_core));
    }
    
    virtual void writeField(ostream &o,const Instr::Field &fp, int idx=SimplFld, const gc_string &p = gc_string())
    {
      gc_string sfx;
      if (fp._subinstr) {
        sfx = "_" + fp._subinstr->name();
      }
      writeField(o,*fp._field,sfx,idx,p);
    }

    // Write out a 'name' field.  This might be a field which references a register file or an instruction table.
    void writeNameField(ostream &o,const InstrField &field,gc_string value,gc_string conv)
    {
      gc_string pfx;         // possible prefix
      gc_string fmt("%d");   // format

      // Instruction tables are the most complex, because we have to make a
      // recursive call to the disassembler.  So, handle that first.  We don't
      // do that for C++-mode b/c this implies that we're creating standalone
      // functions for the ISS.  So, we can only print them as hex fields.
      if (field.instrtable()) {
        // We have to left align the values for the disassembler to work.
        if (!_discall.disassembleMicroOps()) {
          o << "os << std::hex << std::showbase << " << value << " << std::noshowbase;\n";
        } else {
          o << "{\n" << dec;
          // Left-align for big-endian, keep right-aligned for little-endian.
          if (field.width() > 32) {
            if (_core.is_little_endian()) {
              o << "uint64_t val = " << value << conv << ";\n"
                << "unsigned data[2] = { ((unsigned)val & 0xffffffff), (unsigned)(val >> 32) };\n";
            } else {
              unsigned shift = field.width();
              o << "uint64_t val = " << value << conv << " << " << (64 - shift) << ";\n"
                << "unsigned data[2] = { (unsigned)(val >> 32), ((unsigned)val & 0xffffffff) };\n";
            }
          } else {
            unsigned shift = (_core.is_little_endian()) ? 32 : field.width();
            o << "unsigned data[1] = { ((unsigned)" << value << conv << ") << " << (32 - shift) << " };\n";
          }
          writeDisasmCall(o,"data",field.instrtablename());
          o << "}\n";
        }
      } else {
        if (const RegFile *rf = field.regfile()) {
          if (rf->sparse()) {
            // Sparse regfiles print containing reg name.
            value = rf->name() + "_disasm(" + value + conv + ")";
            fmt = "%s";
            conv.clear();
          } else if (!rf->prefix().empty()) {
            pfx = rf->prefix();
          } 
        }
        // Register-file prefix (if it exists, otherwise decimal).
        if (_use_cpp) {
          o << "os << std::dec << \"" << pfx << "\" << " << value << ";\n";
        } else {	     
          o << "(*info->fprintf_func) (info->stream,\"" << pfx << fmt << "\"," << value << conv << ");\n";            
        }
      }
    }
    
    // Given an instruction field, writes out code for displaying an instruction field.
    virtual void writeField(ostream &o,const InstrField &field,const gc_string &sfx,int idx=SimplFld, const gc_string &p = gc_string())
    {
      //      const InstrField &field = (*fp._field);
      gc_string addr_conv = gc_string(".") + ((!field.is_signed()) ? "u" : "") + ((_core.nia()->width() > 32) ? "int64()" : "int32()");

      const InstrField::Syntax *syntax = field.syntax();
      if (syntax) {  
        auto siter =  syntax->_fmt.begin();
        auto send =   syntax->_fmt.end();
        // Advance past whitespace to start of field information.
        while (siter != send && isspace(*siter)) {
          ++siter;
        }

        // We break the syntax string into elements- either calls to fprintf
        // which print constants, or calls to the appropriate function to print a field.
        while (siter != send) {
          if (syntax->isfield(*siter)) {
            auto fid = syntax->getfiledid(*siter);
            // Handle sign-extension and shifting.
            const InstrField::Field &fp = fid.first;
            writeField(o,*fp._field,sfx,fid.second);	    
            ++siter;
          } else {
            // Non-control code- collect string up to the next control code and
            // display that.
            if (_use_cpp) {
              o << "os << \"";
            } else {
              o << "(*info->fprintf_func) (info->stream, \"";
            }
            while (siter != send && !syntax->isfield(*siter)) {
              o << *siter;
              ++siter;
            }
            if (_use_cpp) {
              o << "\";\n";
            } else {
              o << "\");\n";
            }
          }
        }
        return;
      }


      gc_string value;
      gc_string tconv = (field.width() >= 32) ? "64" : "32";
      gc_string conv = (field.is_signed()) ? (".int"+tconv+"()") : (".uint"+tconv+"()");
      if (!p.empty()) {
        value = "(" + p + ")";
      }
      else {
        value = genFieldName(field,sfx,idx);
        if (field.encap_field()) {
          gc_string oname = genFieldName(*(field.encap_field()),sfx,idx) ;
          value = "get_" + value +  "(" + oname + ")";
          conv.clear();
        }
      }

      if (field.is_enumerated()) {
        // Field is enumerated- print the appropriate string based upon the value we have.
        o << "switch (" << value << ".uint32()) {\n";
        auto enums = field.enums();
        auto last = enums.end();
        ForEach(enums,cur) {
          if ( cur->first == "" || cur->first == "reserved") {
            // Do nothing.
          } else if (last == enums.end() || (cur->second != last->second)) {
            // If the value is the same as the prior (assuming that there is a
            // prior), then skip.  We only disassemble to the first option when
            // multiple options for the same value are present.
            o << "case " << dec << cur->second << ":\n";
            if (_use_cpp) {
              o << "os << \"" << cur->first << "\";\n";
            } else {
              o << "(*info->fprintf_func) (info->stream, \"" << cur->first << "\");\n";
            }
            o << "break;\n";
          }
          last = cur;
        }
        o << "}\n";
      } else {
        // We have a control code.  Output the appropriate display routine.
        switch (field.addr()) {
        case InstrField::aPcRel:
          // Relative address.
          if (_use_cpp) {
            o << "os << \"0x\" << std::setw(16) << std::setfill('0')  << std::hex << ((memaddr + " << value << addr_conv << ")";
            if (_core.nia()->width() < 64) {
              o << " & ((1ull << " << _core.nia()->width() << ")-1)";
            }
            o << ");\n";
          } else {
            o << "(*info->print_address_func) (((memaddr + " << value << addr_conv << ")";
            if (_core.nia()->width() < 64) {
              o << " & ((1ull << " << _core.nia()->width() << ")-1)";
            }
            o << "), info);\n";
          }
          break;
        case InstrField::aAbs:
          // Absolute address.
          if (_use_cpp) {
            o << "os << \"0x\" << std::setw(16) << std::setfill('0')  << std::hex << (" << value << ");\n";
          } else {
            o << "(*info->print_address_func) (" << value << addr_conv << ", info);\n";
          }
          break;
        case InstrField::aNone:
          // Not an address- use appropriate display information.
          InstrField::Display d = field.display();
          if (d == InstrField::Default) {
            // If we're to use the default display, then if the field has a
            // reference to a register file, memory, or instruction table, then
            // use the register's prefix (or relevant named syntax), otherwise
            // use decimal if smaller than 6 bits.
            if (field.regfile() || field.memory() || field.instrtable()) {
              d = InstrField::Name;
            } else if (field.memory() || field.width() > 5) {
              d = InstrField::Hex;
            } else {
              d = InstrField::Dec;
            }
          }
          switch (d) {
          case InstrField::Default:
          case InstrField::Dec:
          case InstrField::SignedDec:            
            // Decimal or decimal with an explicit sign value.
            if (_use_cpp) {
              if (d == InstrField::SignedDec) {
                o << "os << std::dec << std::showpos << " << value << " << std::noshowpos;\n";                
              } else {
                o << "os << std::dec << " << value << ";\n";                
              }
            } else {
              string signfmt = (d == InstrField::SignedDec) ? "+" : "";
              string c = (field.size() > 32) ? "ll" : "";
              c += (field.is_signed()) ? "d" : "u";
              o << "(*info->fprintf_func) (info->stream, \"%" << signfmt << c << "\"," << value << conv << ");\n";
            }
            break;
          case InstrField::Hex:
            // Hexadecimal.
            if (_use_cpp) {
              o << "os << std::hex << std::showbase << " << value << " << std::noshowbase;\n";
            } else {
              string c = (field.width() > 32) ? "llx" : "x";
              if (field.is_signed()) {	
                o << "if (" << value << " < 0) \n{"
                  << "(*info->fprintf_func) (info->stream, \"-0x%" << c << "\"," 
                  << "((~" << value << conv << ")+1)" << ");\n } else {\n"
                  << "(*info->fprintf_func) (info->stream, \"0x%" << c << "\"," << value << conv << ");\n}\n";

              } else {
                gc_string fmt;
                if (field.width() >= 32) {
                  fmt = "llx\",(long long unsigned)";
                } else {
                  fmt = "x\",";
                }
                o << "(*info->fprintf_func) (info->stream, \"0x%" << fmt << value << conv << ");\n";
              }
            }
            break;
          case InstrField::Name:
            writeNameField(o,field,value,conv);
            break;        
          }
        }
      }
    }

    void writeNumber(ostream &o,const Instr::Field& fp)
    {
      if (_use_cpp) {
        o << "os << std::dec << (" << fp._expr << ");\n";
      } else {
        o << "(*info->fprintf_func) (info->stream, \"%d\",to_uint32(" << fp._expr << "));\n";
      }
    }        

    gc_string getExprName(const gc_string &s)
    {
      return s + "_expr";
    }

    typedef gc_set<const InstrField *> InstrFieldPSet;

    // Fields marked as such are written implicitly in front of the instruction.
    // They're then excluded from the default assembler output.  The syntax
    // string can override, however.
    void writeImplicitPrefixFields(ostream &o,const Instr &instr,InstrFieldPSet &skip)
    {
      int numfields = instr.sizeFields();
      for (int i = 0; i != numfields; ++i) {
        Instr::Field fp = instr.getField(i);
        if (fp._type == Instr::Field::Resrvd) {
          continue;
        }
        const InstrField &field = *(fp._field);
        switch (field.disassemble()) {
        case InstrField::dsTrue:
          // Normal field to disassemble.
          break;
        case InstrField::dsPrefix:
          // Disassemble before the instruction's name, then skip by default.
          writeField(o,fp);
          if (_use_cpp) {
            o << "os << ' ';\n";
          } else {
            o << "(*info->fprintf_func) (info->stream, \" \");\n";
          }
        case InstrField::dsFalse:
          // Skip this field.
          skip.insert(&field);
          break;
        }
      }
    }

    // Writes out the lines which will print the fields.  We use the syntax string
    // if present, otherwise we use the order of the fields as they appear in the
    // instruction.  If the field has a register file associated with it, then we
    // use that register's prefix, otherwise we print in decimal.  Otherwise, we
    // print in decimal if the field's width is <= 5, otherwise we use hex with a
    // 0x prefix.
    virtual void writeFields(ostream &o,const Instr &instr)
    {
      // Do we have a syntax line?  If so, use it.
      const Instr::Syntax *syntax = 0;
      gc_string::const_iterator siter;
      gc_string::const_iterator send;
      bool use_syntax = false;
      if (instr.dsyntax()) {
        syntax = instr.dsyntax();
        siter = syntax->_fmt.begin();
        send = syntax->_fmt.end();
        use_syntax = true;
        syntax = instr.dsyntax();
      } else if (instr.syntax() && !instr.syntax()->have_to_invert()) {      
        syntax = instr.syntax(); 
        siter = syntax->_fmt.begin();
        send = syntax->_fmt.end();
        use_syntax = true;	    
      }

      // Write out any implicit prefix fields.
      InstrFieldPSet skip;
      writeImplicitPrefixFields(o,instr,skip);

      if (use_syntax) {
        // Evaluate any expressions here.
        DisExprWalker dewalker(_core.env(),_core);
        ForEach(syntax->_items,iter) {
          const Instr::Field &fp = iter->first;
          if (fp._type == Instr::Field::Expr && fp._name && strlen(fp._name)) {
            o << "bits<32> " << getExprName(fp._name) << " = " << dewalker.Translate(fp._expr) << ";\n";
          }
        }

        // If there is whitespace immediately after the instruction name and no
        // prefix fields, then attempt to left-justify the name.  Otherwise,
        // don't bother- it'll just be printed as part of the syntax string.
        if (syntax->hasName() && isspace(syntax->_fmt[syntax->_in_end]) && !syntax->pfx_end()) {
          gc_string nm = syntax->getName();
          siter +=  syntax->_in_end;

          if (_use_cpp) {
            if (_namesize) {
              o << "os << setfill(' ') << setw(" << _namesize << ") << left << \"" << nm << "\" << right;\n";
            } else {
              o << "os << \"" << nm << " \";\n";
            }
          } else {
            if (_namesize ){
              o << "(*info->fprintf_func) (info->stream, \"%-" << dec << _namesize << "s\",\"" << nm << "\");\n";
            } else {
              o << "(*info->fprintf_func) (info->stream, \"" << nm << " \");\n";
            }
          }
        }

        // Advance past whitespace to start of field information.
        while (siter != send && isspace(*siter)) {
          ++siter;
        }

        // We break the syntax string into elements- either calls to fprintf
        // which print constants, or calls to the appropriate function to print a field.
        while (siter != send) {
          if (syntax->isfield(*siter)) {
            auto fid = syntax->getfiledid(*siter);
            // Handle sign-extension and shifting.
            const Instr::Field &fp = fid.first;
            if (fp._type == Instr::Field::Expr) {
              if (fp._field) {
                writeField(o,fp,fid.second,getExprName(fp._name)); 
              } else{ 
                writeNumber(o,fp);  
              }
            } else {
              writeField(o,fp,fid.second);	    
            }	    	    
            ++siter;
          } else {
            // Non-control code- collect string up to the next control code and
            // display that.
            if (_use_cpp) {
              o << "os << \"";
            } else {
              o << "(*info->fprintf_func) (info->stream, \"";
            }
            while (siter != send && !syntax->isfield(*siter)) {
              o << *siter;
              ++siter;
            }
            if (_use_cpp) {
              o << "\";\n";
            } else {
              o << "\");\n";
            }
          }
        }
      } else {
        // The name of the instruction object is used as the instruction name.
        if (_use_cpp) {
          if (_namesize) {
            o << "os << setfill(' ') << setw(" << _namesize << ") << left << \"" << instr.name() << "\" << right;\n";
          } else {
            o << "os << \"" << instr.name() << " \";\n";
          }
        } else {
          if (_namesize ) {
            o << "(*info->fprintf_func) (info->stream, \"%-" << dec << _namesize << "s\",\"" << instr.name() << "\");\n";
          } else {
            o << "(*info->fprintf_func) (info->stream, \"" << instr.name() << " \");\n";
          }
        }
        // Write out each of the operands using the default rules.
        int numfields = instr.sizeFields();
        bool first = true;
        for (int i = 0; i != numfields; ++i) {
          Instr::Field fp = instr.getField(i);
          if (fp._type == Instr::Field::Resrvd || skip.count(fp._field)) {
            continue;
          }
          if (!first) {
            if (_use_cpp) {
              o << "os << ',';\n";
            } else {
              o << "(*info->fprintf_func) (info->stream, \",\");\n";
            }
          }
          first = false;
          writeField(o,fp);
        }
      }
    }

    // Return the number of bytes used (width of the instruction).
    void writeReturn(ostream &o,const Instr &instr)
    {
      o << "return " << instr.width()/8 << ";\n"; //
      // o << "return " << (instr.width()-1)/8+1 << ";\n";    //
    }

    // Write the name, fields, and a return statement for an instruction.
    void writeInstrInfo(ostream &o,const Instr &instr)
    {
      o << "// Instruction " << instr.name() << "\n";
      writeFields(o,instr);
      writeReturn(o,instr);
    }

    void writeGetField(ostream &o,const InstrField &field,const SubInstrGrp *subinstr,unsigned instr_width,const gc_string &iname, int idx, bool pointer_arg)
    {
      o << getInstrFieldType(field,_core.nia()->width(),false,true);

      gc_string sfx;
      if (subinstr) {
        sfx += "_" + subinstr->name();
      }

      // We set the unused attribute so that if the variable is not used in
      // an expression, a warning is not issued.  We could just be smarter
      // about what we emit, this is easier for now.
      o << genFieldName(field,sfx,idx) << " ATTRIBUTE_UNUSED = " << genGetModifiedField(_core,field,sfx,instr_width,_library_mode,iname,idx,pointer_arg) << ";\n";
    }

    void fieldSetup(ostream &o,const Instr::Fields &fields,unsigned instr_width,unsigned num_words)
    {
      gc_string iname = "tmpinsn";

      o << "unsigned " << iname << "[" << num_words << "] ATTRIBUTE_UNUSED = {";
      bool first = true;
      ForRange(num_words,i) {
        list_sep(o,first);
        o << _iname << "[" << i << "]";
      }
      o << "};\n\n";

      // First, process the overlay fields.  After each extraction, set its value to 0 so that
      // the underlying field will be extracted sans overlay value.
      ForEach(fields,i) {
        const InstrField &field = *(i->_field);
        if (!field.is_overlay() || i->_type == Instr::Field::Resrvd) {
          continue;
        }
        writeGetField(o,field,i->_subinstr,instr_width,iname,SimplFld,true);
        // It's an overlay, so remove its value.
        o << "clear_" << field.name() << "(" << iname << ");\n";
      }
      ForEach(fields,i) {
        const InstrField &field = *(i->_field);
        if (field.is_overlay() || i->_type == Instr::Field::Resrvd) {
          continue;
        }
        writeGetField(o,field,i->_subinstr,instr_width,iname,SimplFld,true);
      }

      ForEach(fields,i) {
        const InstrField &field = *(i->_field);
        if (!field.is_constrained()) {
          continue;
        }

        ForRange(field.table().entry_size(),k) {
          writeGetField(o,field,i->_subinstr,instr_width,iname,k,true);
        }
      }
    }

    void shorthandSetup(ostream &o,const Instr::Fields &fields,bool &emit_if,bool &first)
    {
      if (emit_if) {
        o << "if (";
        emit_if = false;
      }
      pfx_sep(o," && ",first);
    }

    // Write a shorthand, which contains various tests to see if it's applicable, followed by the instruction
    // disassembly code.
    virtual void writeShorthand(ostream &o,const Instr &sh)
    {
      if ( !sh.disassemble()) {
        return;
      }
      // For each shorthand, construct a conditional that checks if it applies.
      // For each field, it must either be equal to the corresponding field, or equal
      // to the constant value specified.
      auto emit_if = true;
      auto first = true;
      auto fields = sh.get_alias()->_fields;
      // Skip if there are no entries specializing this shorthand.
      ForEach(fields,field) {
        const gc_string &fname = field->_field->interface() ? field->_field->interface()->name() : field->_field->name();
        switch (field->_type) {
        case Instr::Field::Resrvd:
        case Instr::Field::RValue:
          std::cerr << "\n Reserved and RValue fields not supported.";
          exit(0);
          break;
        case Instr::Field::Expr:
          shorthandSetup(o,fields,emit_if,first);
          // Field has an expression which must hold.
          o << "(" << fname << " == (" << field->_expr << "))";
          break;
        case Instr::Field::Value:
          shorthandSetup(o,fields,emit_if,first);
          // Field has a constant value that must be maintained.
          o << "(" << fname << " == 0x" << hex << field->_value << dec << ")";
          break;
        case Instr::Field::Ref:
          // Field must be the same as another field.  Ignore if we're dealing with the
          // same field.
          if (fname != sh.getField(field->_value)._field->name()) {
            shorthandSetup(o,fields,emit_if,first);
            o << "(" << fname << " == " << sh.getField(field->_value)._field->name() << ")";
          }
          break;
        case Instr::Field::Alias:
          break;
        }
      }
      if (!emit_if) {
        o << ") {\n";
        writeInstrInfo(o,sh);
        o << "}\n";
      }
    }

    // Write a "leaf node" entry in the disassembly table.
    virtual void writeDisTableEntry(ostream &o,const Instr &instr,const InstrList &shorthands,bool next_table_func)
    {
      // If we have shorthands, we try to match against those first.  Failing that,
      // we use the generic instruction.  We sort the shorthands in order of 
      // First, we sort the shorthands in order of specificity (number of reference fields)
      // so that we'll match against a more constrained shorthand before we match against
      // a less general shorthand.

      unsigned num_words = _core.instr_width()/32;
      unsigned instr_width = _core.is_little_endian() ? instr.width() : 0;
      fieldSetup(o,instr.getFields(),instr_width,num_words);

      if (_gen_pfx_logic) {
        if (instr.is_prefix()) {
          // Generate any setup of prefix variables.
          genPrefixFieldSetup(o,_core,instr,"");
        } else {
          packetPosIncr(o,_core,_pkt_pos,instr);
        }

        // If the instruction has an automatic next-table selector, then apply it.
        if (next_table_func && instr.next_table()) {
         DisExprWalker dewalker(_core.env(),_core);
          o << dewalker.Translate(instr.next_table()) << "\n";
        }
      }

      vector<const Instr *> sorted_shorthands(shorthands.begin(),shorthands.end());
      sort(sorted_shorthands.begin(),sorted_shorthands.end(),SortBySpecificity());
      ForEach(sorted_shorthands,i) {
        writeShorthand(o,**i);
      }
      writeInstrInfo(o,instr);
    }

    // write the accessors of a "leaf node" entry in the disassembly table. 
    virtual void writeDisTableEntryAccessors(ostream &o,const Instr &instr, const InstrList &shorthands) 
    {
      unsigned num_words = _core.instr_width()/32;
      unsigned instr_width = _core.is_little_endian() ? instr.width() : 0;
      fieldSetup(o,instr.getFields(),instr_width,num_words);
    } 
    
    // We only need additional fetch logic if we have prefix bits, since otherwise we just read directly
    // from the input source.
    virtual bool write_additional_fetch(ostream &o,unsigned start, unsigned end, bool prefix, unsigned &fetch_index)
    {
      if (start >= end ) {
        return false;
      }

      unsigned pb = _core.prefix_bits();

      if (!prefix && (pb > 0)) {
        for(unsigned i = start; i < end; ++i) {
          o << _iname << "[" << i << "] |= (" << _fname << "[" << fetch_index << "] >> " << dec << pb << ");\n"
            << _iname << "[" << i+1<< "] = (" << _fname << "[" << fetch_index << "] << " << 32-pb << ");\n";
          ++fetch_index;
        }

      }

      return true;
    }

    virtual void writeDecodeMiss(ostream &o,unsigned fetch_index)
    {
      o << "(*info->fprintf_func) (info->stream, \".long 0x%x" << "\"," << _iname << "[0]);\n";
      o << "return " << (fetch_index+1)*4 << ";\n";
    }

    // Write the actual disassembly table.
    void writeDisTableInternal(ostream &o,const InstrTable &table, unsigned fetched,unsigned fetch_index,bool prefix,bool write_decode_miss = true)
    {
      if (!table.empty()) {
        // Not a leaf node:  Create a switch statement that operates
        // on the relevant portion of the current instruction.
        RNumber omask = table.mask();

        unsigned to_fetch = required_fetch(table.mask());
        if (write_additional_fetch(o,fetched,to_fetch,prefix,fetch_index)) {
          fetched = to_fetch;
        }

        unsigned mask  = get_submask(omask);
        unsigned index = get_index(omask);

        o << "switch (" << _iname << "[" << dec << index << "] & 0x" << hex << mask << ") {\n";
        ForEach(table,i) {
          unsigned subval = get_subval(index,i->first);
          o << "case 0x" << hex << subval << "ul: {\n";
          writeDisTableInternal(o,*(i->second),fetched,fetch_index,prefix,write_decode_miss);
          o << "}\n"
            << "break;\n";
        }

        if (table.default_table()) {
          // Default-table case.  Only arises when we have a field tagged as
          // allow_conflict=true.
          o << "default:{\n";
          writeDisTableInternal(o,*table.default_table(),fetched,fetch_index,prefix,write_decode_miss);
          o << "} }\n" << dec;          
        } else if (const Instr *instr = table.instr()) {
          // Situation where we have specializations for specific instructions
          // (above), then a non-specialized case, e.g. the user has specified
          // fixed values for a field that's not usually an opcode, then has a
          // case where the field is just a pure operand.
          o << "default:{\n";
          to_fetch = num_words(_core,instr->fetch_width());
          write_additional_fetch(o,fetched,to_fetch,prefix,fetch_index);
          writeDisTableEntry(o,*table.instr(),table.shorthands(),true);
          o << "} }\n" << dec;
        } else {
          // Default case- write out the hex value and return.
          o << "default:;\n";
          if (write_decode_miss) {
            writeDecodeMiss(o,fetch_index);
          }
          o << "}\n" << dec;
        }
      } else if (const Instr *instr = table.instr()) {
        unsigned to_fetch = num_words(_core,instr->fetch_width());
        write_additional_fetch(o,fetched,to_fetch,prefix,fetch_index);
        writeDisTableEntry(o,*table.instr(),table.shorthands(),true);
      }
    }

    // Start for writing the disassembly table.
    virtual void writeDisTableStart(ostream &o,const InstrTable &table, bool write_decode_miss = true)
    {
      bool prefix_tbl = (_core.prefix_bits() && table.is_prefix());
      if (_core.prefix_bits()) {
        writePrefixArchFetchCode(o,_core,_iname,_fname+"[0]",prefix_tbl);
      }
      writeDisTableInternal(o,table,1,1,prefix_tbl,write_decode_miss);
    }

  };

  class DebugDisCall : public DisassemblerCallIF
  {
    bool disassembleMicroOps() const { return true; };
    
    void operator()(ostream &o,const gc_string &arg,const gc_string &table_type,bool has_multiple_tables) override
    {
      o << "disassemble_to_struct(info," << arg;
      if (!table_type.empty()) {
        o << ',' << table_type;
      }
      o << ");\n";
    }
    
  };
  
  // This generates a cut-down disassembler which just disassembles an
  // instruction into a structure.
  struct DisassemblerDebugImpl : public DisassemblerImpl
  {
    InstrStrMap  _inames;
    unsigned     _max_args;
    gc_string    _archpfx;
    DebugDisCall _discall;

    DisassemblerDebugImpl(const Core &core,const gc_string &iname,const gc_string &fname,
                          const InstrStrMap &inames,unsigned max_args) :
      DisassemblerImpl(core,false,false,false,0,iname,fname,gc_string(),_discall),
      _inames(inames),
      _max_args(max_args)
    {
    }


    bool skip_field(const Instr::Field fp) 
    {
      const InstrField &field = *(fp._field);
      return (field.is_overlay() || field.is_variable() || fp._type == Instr::Field::Resrvd);  
    }

    gc_string getInstrFieldName(const InstrField &field,const SubInstrGrp *subinstr,bool orig)
    {
      const InstrField* f = field.interface() ? field.interface() : &field;
      gc_string n = f->name();
      if (subinstr) {
        n += "_" + subinstr->name();
      }
      if (is_special(field) && orig) {
        return f->name() + "_p";
      }
      return n;
    }

   
    virtual void writeDecodeMiss(ostream &o,unsigned fetch_index)
    {
      o << "return instr_invalid;\n";
    }


    // Write a "leaf node" entry in the disassembly table.
    virtual void writeDisTableEntry(ostream &o,const Instr &instr,const InstrList &shorthands,bool next_table_func)
    {
      unsigned num_words = _core.instr_width()/32;
      unsigned instr_width = _core.is_little_endian() ? instr.width() : 0;
      fieldSetup(o,instr.getFields(),instr_width,num_words);

      gc_string type;
      IfFind(iter,_inames,&instr) {
        type = iter->second;
      } else {
        RError("Instruction " << instr.name() << " not found in instruction name map.");
      }

      o << "info._type = " << type << ";\n"
        << "info._size = " << dec << instr.width() << ";\n"
        << "info._name = \"" << instr.name() << "\";\n";
      
      // List all arguments- the instruction fields.
      int numfields = instr.sizeFields();
      // Grab all instruction field values.
      unsigned argCount=0;
      for (int i = 0; i != numfields; ++i) {
        Instr::Field fp = instr.getField(i);
        const InstrField &field = *(fp._field);
        // variable fields use for opcode
        if (skip_field(fp)) {  
          continue;
        }
        o << "info._args[" << argCount++ << "] = "
          << getInstrFieldName(field,fp._subinstr,false);
        if (field.is_signed()) {
          o << ".int32();\n";
        } else {
          o << ".uint32();\n";
        }
      }

      o << "info._numargs = " << argCount << ";\n"
        << "return " << type << ";\n";

      assert(argCount <= _max_args);
    }

  };

  // Class for generating the disassembler.
  struct GenerateDisassemblerInt : public GenBase {

    unsigned  _machine_code;
    size_t    _namesize;
    bool      _gen_pfx_logic; // Whether to add prefix/packet update logic.
    gc_string _cname;         // Disassembler class name.
    gc_string _iname;         // Instruction data name.
    gc_string _fname;         // Fetch data name.
    gc_string _pkt_pos;       // Packet-position variable name.

    DefaultDisassemblerCall _defaultDisassemblerCall;
    DisassemblerCallIF     &_discall; // For recursive disassembler calls.

    ClassData _cd;

    GenerateDisassemblerInt(const Core &core,bool use_cpp,bool name_ws,bool gen_pfx_logic,bool library_mode,
                            const gc_string &fname,const gc_string &log_usage,DisassemblerCallIF *dc) :
      GenBase(core,use_cpp,library_mode,fname,log_usage),
      _machine_code(0),
      _namesize(0),
      _gen_pfx_logic(gen_pfx_logic),
      _cname("Disassemble"),
      _iname("insn"),
      _fname("input"),
      _pkt_pos("_packet_position"),
      _defaultDisassemblerCall(!use_cpp),
      _discall( (dc) ? *dc : _defaultDisassemblerCall)
    {
      findMaxInstrNameSize(name_ws);
    }

    void writeDisTableEntry(ostream &o,const Instr &instr,const InstrList &shorthands)
    {
      DisassemblerImpl disasm(_core,_use_cpp,_library_mode,_gen_pfx_logic,_namesize,_iname,_fname,_pkt_pos,_discall);

      disasm.writeDisTableEntry(o,instr,shorthands,false);
    }

    void writeDisTableEntryAccessors(ostream &o,const Instr &instr, const InstrList &shorthands) 
    {
      DisassemblerImpl disasm(_core,_use_cpp,_library_mode,_gen_pfx_logic,_namesize,_iname,_fname,_pkt_pos,_discall);

      disasm.writeDisTableEntryAccessors(o,instr,shorthands);
    }

    // Main function for writing the disassembly tables.
    void writeDisTables(ostream &o)
    {
      DisassemblerImpl disasm(_core,_use_cpp,_library_mode,_gen_pfx_logic,_namesize,_iname,_fname,_pkt_pos,_discall);

      bool mt = hasMultipleTables(_core);

      if (mt) {
        o << "enum TableType {\n";
        gc_string first_table;
        ForEach(_core.instrtables(),i) {
          gc_string n = Data::get_attr_name(i->first);
          if (i->second._pos == 0) {
            first_table = n;
          }
          o << n << ",\n";
        }
        o << "};\n\n"
          << "void setCurrentInstrTable(TableType t) {\n"
          << getTableSelect() << " = t;\n"
          << "}\n\n"
          << "TableType getCurrentInstrTable() {\n"
          << "return " << getTableSelect() << ";\n"
          << "}\n\n";

        _cd._member_vars.push_back("TableType " + getTableSelect());
        _cd._constr_inits.push_back(getTableSelect() + "(" + first_table + ")");
      }

      gc_string input = (_core.prefix_bits()) ? _fname : _iname;
      o << "int disassemble_insn(bfd_vma memaddr ATTRIBUTE_UNUSED, unsigned *" << input << ",struct disassemble_info *info)\n"
        << "{\n";

      if (_core.prefix_bits()) {
        unsigned n = _core.instr_width()/32 + 1;
        o << "uint32_t " << _iname << "[" << n << "] = { ";
        bool first = true;
        ForRange(n,i) {
          list_sep(o,first);
          o << "0";
        }
        o << "};\n";
      }
      if (mt) {
        // If we have more than one table, then we look for an instruction-table
        // selection symbol.  This is something of the form of "$$x", where x
        // specifies the instruction table to be used.
        o << "if (info->symbols != NULL && info->num_symbols > 0) {\n"
          << "const char *name = info->symbols[0]->name;\n";
        bool first = true;

        ForEach(_core.instrtables(),i) {
          gc_string n = Data::get_attr_name(i->first);
          gc_string t = n;
          if (n == DefaultInstrTable) {
            n = DefaultTableName;
          }
          pfx_sep(o,"else ",first);
          o << "if (strcmp(name,\"$$" << n << "\") == 0) {\n"
            << getTableSelect() << " = " << t << ";\n"
            << "}\n";

        }
        o << "}\n\n"
          << "switch (" << getTableSelect() << ") {\n";

        ForEach(_core.instrtables(),i) {
          gc_string n = Data::get_attr_name(i->first);
          o << "case " << n << ": {\n";
          const InstrTable &table = *(i->second._table);
          disasm.writeDisTableStart(o,table);
          o << "} break;\n";
        }
        o << "}\n";
      } else if (!_core.asm_config().disasm_instrtables().empty()) {
        // We have priority list for instruction tables.
        ForEach(_core.asm_config().disasm_instrtables(),i) {
          bool last = (i == _core.asm_config().disasm_instrtables().end());
          int id = Data::get_attr(*i);
          const InstrTable &table = *(_core.instrtables().find(id)->second._table);
          disasm.writeDisTableStart(o,table,last);
        }	
      } else {
        // Only a single table exists, so there's no need for any selection logic.
        const InstrTable &table = *(_core.instrtables().begin()->second._table);
        disasm.writeDisTableStart(o,table);
      }
      // For stupid compilers, add a final handler here.
      o << "(*info->fprintf_func) (info->stream, \".long 0x%x\"," << input << "[0]);\n"
        << "info->bytes_per_chunk = 4;\n"
        << "return 4;\n"    
        << "}\n\n";
    }

    void writeDisassembleToStructDecl(ostream &o,bool is_decl)
    {
      bool mt = hasMultipleTables(_core);

      o << getInstrType(_archpfx) << " " << _archpfx << "_disassemble(" << getInstrInfoType(_archpfx) << " *info,unsigned *input";
      if (mt) {
        o << "," << getInstrTableType(_archpfx) << " table_type";
      }
      o << ")";

      if (is_decl) o << ";";

      o << "\n";
    }

    // Write various enum types to the generated header file for debugger assist.
    void writeDbgHeader(ostream &o,InstrStrMap &inames,DbgRegList &debug_regs,unsigned &max_args)
    {

      o << "//\n"
        << "// Header instruction information for " << _arch_print_name << "\n"
        << "//\n\n"

        << "#ifndef _" << _archpfx << "_HEADER\n"
        << "#define _" << _archpfx << "_HEADER\n\n";

      // Instruction table types.

      bool mt = hasMultipleTables(_core);

      if (mt) {

        o << "enum {\n";

        bool got_default = false;
        ForEach(_core.instrtables(),i) {
          gc_string n = Data::get_attr_name(i->first);
          
          if (n == DefaultInstrTable) got_default = true;
          
          o << n << ",\n";
        }
        if (!got_default) {
          o << DefaultInstrTable << ",\n";
        }

        o << "} " << getInstrTableType(_archpfx) << ";\n\n";

      }

      // Enums for the instructions themselves.

      inames.clear();

      o << "typedef enum {\n"
        << "instr_invalid,\n";

      ForEach(_core.instrs(),i) {
        const Instr &instr = *(i->second);

        if (!instr.is_used() || instr.is_pseudo() || instr.is_alias()) continue;

        gc_string enum_name = getEnumName(instr);
        inames.insert(make_pair(&instr,enum_name));

        o << enum_name << ",\n";

        max_args = max(max_args,instr.sizeFields());
      }

      o << "} " << getInstrType(_archpfx) << ";\n\n"

        << "enum {\n"
        << _archpfx << "_MaxInstrOps = " << max_args << "\n"
        << "};\n\n"

        // A structure to hold disassembly info about instructions.
        << "typedef struct {\n"
        << getInstrType(_archpfx) << " _type;\n"
        << "unsigned    _size; // Instruction width in bits.\n"
        << "const char *_name;\n"
        << "unsigned    _numargs;\n"
        << "int         _args[" << _archpfx << "_MaxInstrOps];\n"
        << "} " << getInstrInfoType(_archpfx) << ";\n\n";

      checkForDebugRegs(debug_regs,_core);
      if (!debug_regs.empty()) {
        o << "enum {\n"
          << _archpfx << "_NumRegs = " << debug_regs.size() << "\n"
          << "};\n\n"

          << "// List of debug registers.\n"
          << "extern const char *" << getDbgRegsName(_archpfx) << "[];\n\n";
      }

      o << "#ifdef __cplusplus\n"
        << "extern \"C\"\n"
        << "#endif\n";
      writeDisassembleToStructDecl(o,true);

      o << "\n\n"
        << "#endif\n\n";
    }

    void writeDbgRegs(ostream &o,const DbgRegList &debug_regs)
    {
      if (!debug_regs.empty()) {
        o << "const char *" << getDbgRegsName(_archpfx) << "[" << debug_regs.size() << "] = {\n";
        ForEach(debug_regs,i) {
          const DbgRegElm &dr = *i;

          const gc_string pfx = lowercase((!dr._pfx.empty()) ? dr._pfx : dr._name);

          o << "\"" << pfx;
          if (dr.has_index()) {
            o << dr._index;
          }
          o << "\",\n";
        }
        o << "};\n\n";
      }

    }

    // The disassembler for debug assistance.  Returns false on a decode miss.
    void writeDbgDisasm(ostream &o,const InstrStrMap &inames,unsigned max_args)
    {      
      DisassemblerDebugImpl disasm(_core,_iname,_fname,inames,max_args);

      bool mt = hasMultipleTables(_core);

      o << getInstrType(_archpfx) << " disassemble_to_struct(" << getInstrInfoType(_archpfx) << " &info,unsigned *" << _iname;
      if (mt) {
        o << "," << getInstrTableType(_archpfx) << " table_type";
      }
      o << ")\n"
        << "{\n";

      if (mt) {
        o << "switch (table_type) {\n";

        ForEach(_core.instrtables(),i) {
          gc_string n = Data::get_attr_name(i->first);
          o << "case " << n << ": {\n";
          const InstrTable &table = *(i->second._table);
          disasm.writeDisTableStart(o,table);
          o << "} break;\n";
        }
        o << "}\n";
      } else if (!_core.asm_config().disasm_instrtables().empty()) {
        // We have a priority list for instruction tables.
        ForEach(_core.asm_config().disasm_instrtables(),i) {
          bool last = (i == _core.asm_config().disasm_instrtables().end());
          int id = Data::get_attr(*i);
          const InstrTable &table = *(_core.instrtables().find(id)->second._table);
          disasm.writeDisTableStart(o,table,last);
        }	
      } else {
        // Only a single table exists, so there's no need for any selection logic.
        const InstrTable &table = *(_core.instrtables().begin()->second._table);
        disasm.writeDisTableStart(o,table);
      }

      o << "return instr_invalid;\n"
        << "}\n\n";
    }


    // Generate code used for assisting with debugger implementations.  This
    // includes a special decode function for disassembling instructions into a
    // data structure, plus register info and such.
    void writeDebuggerHelpers(ostream &o,ostream &h,DbgRegList &debug_regs)
    {
      InstrStrMap inames;
      unsigned    max_args = 0;

      writeDbgHeader(h,inames,debug_regs,max_args);

      // Generate the debugger disassembler.  This is used to decode an
      // instruction into its type and operand values, for use by, e.g. the
      // function preamble examining code in gdb.
      writeDbgDisasm(o,inames,max_args);


    }

    void genPacketMods(ostream &o)
    {
      _cd._member_vars.push_back("unsigned _packet_position");
      _cd._constr_inits.push_back("_packet_position(0)");
      _cd._reset_stmts.push_back("_packet_position = 0");
      _cd._member_vars.push_back("bool _packet_position_reset");
      _cd._constr_inits.push_back("_packet_position_reset(false)");
      _cd._reset_stmts.push_back("_packet_position_reset = false");
	
      o << "//\n" 
        << "//  Packet position accessors.\n"
        << "//\n\n"
        << "unsigned getPacketPosition() {\n"
        << "return " << _pkt_pos << ";\n"
        << "}\n\n"
        << "void setPacketPosition(unsigned pos) {\n"
        << _pkt_pos << " = pos;\n"
        << "_packet_position_reset = (" << _pkt_pos << " == 0);\n"
        << "}\n\n"
        << "//\n"
        << "//\n"
        << "void packetPositionReset() {\n"
        << "if (_packet_position_reset) {\n"
        << "_packet_position_reset = false;\n";

      if (!_core.prefix_counters().empty()) {
        o << "prefix_counter_reset();\n";
      }
      
      if (_core.has_prefix_instruction()) {	
        o << "reset_prefix();\n";
      }
      o << "}\n"
        << "}\n\n";
    }

    void genPrefixSupport(ostream &o)
    {
      if (_core.parallel_execution()) {
        genPrefixFieldCtors(_core,_cd._constr_inits);
        gc_ostringstream ss;
        genPrefixFieldDeclarations(o,ss,_core,"",_cname);
        _cd._member_vars.push_back(ss.str());

        genPrefixFieldGetters(o,_core,"inline static",0,true,false);
        genPrefixFieldClearers(o,_core,"inline");
        genPrefixCounters(o,_core,_cd);
        genPacketMods(o);
      }
    }

    // Store the length of the longest instruction name.
    void findMaxInstrNameSize(bool name_ws)
    {
      const InstrHash &instrs = _core.instrs();
      _namesize = 0;
      if (name_ws) {
        ForEach(instrs,i) {
          const Instr &instr = *(i->second);
          if (instr.shorthand() || !instr.is_abstract() || instr.syntax()) {
            gc_string nm = getInstrName(instr);
            _namesize = max(_namesize,nm.size());
          }
        }
        ++_namesize;
      }
    }

    void genHelpers(ostream &o)
    {
      AsmActionWalker walker(_core.env());

      const HelperMap &helpers = _core.helpers();
      ForEach (helpers,i) {
        const gc_string &fname = i->first;
        
        // Skip if it's not an assembly helper.
        if (!_core.disasm_helpers().count(fname)) continue;

        Ptree *func = walker.Translate(i->second._func);

        o << func << "\n\n";
      }
    }

    // FIXME: This is a hack.  This should be replaced by properly generated
    // target vector items that aren't linked to PPC.
    void genMachineCodeSetup(ostream &o)
    {
      if (_machine_code) {
        o << "static bool adl_set_machine_code() {\n"
          << "extern bfd_target powerpc_elf64_vec;\n"
          << "extern bfd_target powerpc_elf64_le_vec;\n"
          << "extern bfd_target powerpc_elf32_vec;\n"
          << "extern bfd_target powerpc_elf32_le_vec;\n"          

          << "((struct elf_backend_data*)(powerpc_elf64_vec.backend_data))->elf_machine_code = " << _machine_code << ";\n"
          << "((struct elf_backend_data*)(powerpc_elf64_le_vec.backend_data))->elf_machine_code = " << _machine_code << ";\n"
          << "((struct elf_backend_data*)(powerpc_elf32_vec.backend_data))->elf_machine_code = " << _machine_code << ";\n"
          << "((struct elf_backend_data*)(powerpc_elf32_le_vec.backend_data))->elf_machine_code = " << _machine_code << ";\n"

          << "return true;\n"
          << "}\n\n"
        
          << "static bool dummy_mc_set = adl_set_machine_code();\n\n";        
      }
    }
    
    // We only need setters if we have relocations associated with instruction
    // fields.
    bool needSettersForRelocs()
    {
      ForEach(_core.relocs(),i) {
        if (i->second->field()) return true;
      }
      return false;
    }

    // Generate a disassembler.
    void exec(ostream &o,ostream *h,const gc_string &header_name)
    {
      o << "/*\n"
        << "\n"
        << "  Disassembler code generated by ADL.\n"
        << "\n"
        << "*/\n\n"
        << "#include <inttypes.h>\n"
        << "#include <stdio.h>\n"
        << "#include <iostream>\n"
        << "#include \"sysdep.h\"\n"
        << "#include \"dis-asm.h\"\n"
        << "extern \"C\" {\n"
        << "#include \"elf-bfd.h\"\n"
        << "#include \"bfd.h\"\n"
        << "#include \"libbfd.h\"\n"
        << "#include \"bfdlink.h\"\n"
        << "}\n"
        << "#include \"helpers/LogUsage.h\"\n"
        << "#include \"helpers/Macros.h\"\n"
        << "\n";

      if (h) {
        o << "#include \"" << header_name << "\"\n"
          << "\n";
      }

      gc_string lepfx;
      if (_core.is_little_endian()) {
        lepfx = "le_";
      }

      o << "#define __NO_RNUMBER__\n"
        << "#include \"helpers/" << lepfx << "sintbv.h\"\n"
        << "#define bits " << lepfx << "intbv\n"
        << "#define sbits " << lepfx << "sintbv\n\n"
        << "#define ns_bits " << lepfx << "intbv\n"
        << "#define ns_sbits " << lepfx << "sintbv\n\n"
        << "#include \"helpers/TypeConv.h\"\n\n"
        << "using namespace adl;\n\n";

      // Write the machine-code setup logic, if applicable.
      genMachineCodeSetup(o);
      
      o << "struct " << _cname << " {\n";
     
      // Write helpers for disassembler.
      genDisasmHelpers(o,_core,"inline static");

      // OK, write the real helpers here.  I think the above is a bit misnamed.
      genHelpers(o);

      genInstrFieldTableGetters(o,_core,"static inline");

      // Write out all instruction field accessors.
      genInstrFieldGetters(o,_core,"static inline",false,false);

      // We include the setter functions in case there are relocations, for use
      // by the linker.
      bool need_setters = needSettersForRelocs();
      if (need_setters) {
        genInstrFieldSetters(o,_core,false,"static ");
      }
      // We include clear functions for handling instruction shorthands with
      // overlay fields and for linking.
      if (need_setters) {
        genInstrFieldClearers(o,_core,"static");
      } else {
        genInstrFieldClearers(o,_core,"static inline");
      }

      genPrefixSupport(o);

      writeDisTables(o);

      DbgRegList  debug_regs;
      if (h) {
        writeDebuggerHelpers(o,*h,debug_regs);
      }

      int bits = _core.instr_width();
      int bytes = bits/8;
      int words = bytes/4;
      bool par_exec = _core.parallel_execution();

      _cd.writeMemberVars(o);

      o << _cname << " ()";
      _cd.writeConstrInits(o);
      o << "\n"
        << "{\n"
        << "}\n\n"

        << "};\n\n"
        << "static " << _cname << " disassemble_obj;\n";

      genRelocInfo(o);

      genRelocActions(o);

      genRelocSetters(o,_cname + "::");

      // Note: Only written once, in the disassembler, since the assembler
      // includes the disassembler.
      genArchInfo(o);

      genUsageLogging(o,"disasm");

      o << "extern \"C\" int " << _archpfx << "_print_insn (bfd_vma memaddr, struct disassemble_info *info,int bigendian);\n\n" 
        << "extern \"C\" int " << _archpfx << "_print_insn (bfd_vma memaddr, struct disassemble_info *info,int bigendian)\n"
        << "{\n"
        << "bfd_byte buffer[" << bytes << "];\n"
        << "unsigned  insn[" << words << "];\n";
      if (par_exec) {
        o << "char *application_data = (char *) info->application_data;\n";
      }
      o << "int status;\n\n"
        << "int size = " << bytes << ";\n"

        // This code is here so that we don't read past the supplied buffer.
        // For example, with variable-sized instructions, we might attempt to
        // read 4 bytes but only have a 2 byte instruction remaining in the
        // buffer.  This would cause an error message in the output.  So, we
        // adjust our buffer size.  If no buffer is supplied, then we ignore
        // this, e.g. gdb connected to a remote target.
        << "if (info->buffer_length) {\n"
        << "if (memaddr+size > (info->buffer_vma+info->buffer_length)) {\n"
        << "size = (info->buffer_vma+info->buffer_length)-memaddr;\n"
        << "memset(buffer,0," << bytes << ");\n"
        << "}\n"
        << "}\n"
        << "status = (*info->read_memory_func) (memaddr, buffer, size, info);\n"
        << "if (status != 0) {\n"
        << "(*info->memory_error_func) (status, memaddr, info);\n"
        << "return -1;\n"
        << "}\n"
        << "if (bigendian) {\n";
      for(int i = 0 ; i != words ; ++i) {
        o << "insn[" << i << "] = bfd_getb32 (buffer +" << i << "*4);\n";
      }    
      o  << "} else {\n";
      for(int i = 0 ; i != words ; ++i) {
        o << "insn[" << i << "] = bfd_getl32 (buffer +" << i << "*4);\n";
      }    
      o << "}\n"
        << "int sz = disassemble_obj.disassemble_insn(memaddr,insn,info);\n";
      if (par_exec) {
        o << "disassemble_obj.packetPositionReset();\n"
          << "if (application_data != info->application_data) {\n"
          << "info->application_data = (void*)((char*)info->application_data - (size - sz));\n"
          << "}";
      }
      o << "return sz;\n"
        << "}\n\n";

      if (h) {
        writeDbgRegs(o,debug_regs);

        o << "extern \"C\" ";
        writeDisassembleToStructDecl(o,false);

        bool mt = hasMultipleTables(_core);

        o << "{\n"
          << "return disassemble_obj.disassemble_to_struct(*info,input";
        if (mt) {
          o << ",table_type";
        }
        o << ");\n"
          << "}\n\n";
      }

    }

  };

  // Class for generating the assembler.
  struct GenerateAssemblerInt : public GenBase {

    StrSet      _instr_names;      // Instruction names, derived from syntax strings.
    StrSet      _instr_pfx_fields; // List of enumerated field values that may precede instruction names.

    unsigned    _min_instr_width;   // Minimium instruction size, in bytes.

    unsigned    _num_asm_instrs;   // The number of assembly-only instructions.

    gc_string   _terminating_chars;     // Set of characters we use as an
                                        // instruction-name delimiter.
    gc_string   _min_terminating_chars; // Minimal set of terminating chars-
                                        // takes into account chars in
                                        // instruction names.
    bool	      _bigMem; // Use longest immediate opcode for unresolved symbols
    gc_string   _asm_ver; // Assembler version string.

    gc_map<const InstrField *,gc_string> _enum_arrays; // Maps enumerated fields to names of their corresponding enum-option arrays.

    typedef pair<const InstrField *, unsigned>  TransField;
    gc_map<TransField, gc_string> _trans_field_array;

    StrSet _reloc_extras;

    GenerateAssemblerInt (const Core &core, bool bigMem,const gc_string &fname,
                          const gc_string &log_usage,const gc_string asm_ver,
                          const StrSet &reloc_extras) : 
      GenBase(core,false,false,fname,log_usage), 
      _min_instr_width(0),
      _num_asm_instrs(0),
      _bigMem(bigMem),
      _asm_ver(asm_ver),
      _reloc_extras(reloc_extras)
    {
    };

    // Generates an alignment mask, given a size in bytes.
    uint64_t genmask(unsigned sz)
    {
      return ~( (uint64_t)(-1) << (uint64_t)(log2(sz)));
    }

    gc_string quoteReChar(char c)
    {
      gc_string r;
      switch (c) {
      case '.': case '[': case '\\': 
      case '*': case '^': case '$': 
      case '?': case '{': case '}': 
      case '(': case ')': case '"':
      case '|': case '+': case ']': case '-':
        // Special character for a regular expression- we'll need to quote it.
        r += "\\\\";
      default:
        // Normal character- add it.
        r += c;
      }
      return r;
    }

    gc_string quoteReStr(const gc_string &s)
    {
      gc_string r;
      ForEach(s,i) {
        r += quoteReChar(*i);
      }
      return r;
    }

    // Quoting a character class is weird, because \ isn't a normal quote character.
    gc_string quoteReCharClass(gc_string s)
    {
      gc_string r;
      // First, if we have a ] then that must go first.
      ForEach(s,i) {
        if (*i == ']') {
          r += *i;
          s.erase(i);
          i = s.begin();
        }
      }
      // If we have ] or ^, then those must follow.
      ForEach(s,i) {
        if (*i == '[' || *i == '^') {
          r += *i;
          s.erase(i);
          i = s.begin();
        }
      }
      // Anything else is OK.
      r += s;
      return r;
    }

    // Return a string of packet chars.  These are not escaped for a regex!
    gc_string get_re_packet_chars()
    {
      auto const & assm = _core.asm_config();
      gc_string t = get_packet_char(assm.group_seps(),assm.line_seps(),true,true) + 
        get_packet_char(assm.group_seps(),assm.line_seps(),false,true);
      return t;
    }

    // Generate a regular expression used for parsing the operands
    // of an instruction.
    gc_string genRegEx(const Instr &instr)
    {
      auto const & assm = _core.asm_config();
      gc_string pkt_end = get_packet_char(assm.group_seps(),assm.line_seps(),false,true);

      // We make sure to anchor the regex to the front for efficiency and to
      // avoid erroneous matches.  Leading whitespace should be skipped over by
      // the assembler code, but might still occur if we have permutable fields
      // where there's an option for no value.  In that case, this will allow us
      // to match because the leading whitespace will match to the separator
      // between the instruction and the non-permutable fields.
      gc_string re = "^" + ws;

      // If we have a syntax string, we use it.
      // Otherwise, we expect a comma-delimited expression for
      // each of the opcodes in the instruction.
      if (const Instr::Syntax *syntax = instr.syntax()) {
        // Syntax string:  Replace each field with (.*) to capture the value.
        // We must quote any special characters.  For enumerated fields, we generated
        // a sequence of choices of the possible values.
        const gc_string &fmt = syntax->_fmt;

        gc_string::const_iterator iter = fmt.begin();
        gc_string::const_iterator end = fmt.end();

        // First, skip leading characters, which we'll assume to be the
        // instruction name.  Skip any leading permutable fields which might be
        // before the instruction name.

        // Get to start of instruction name.
        while (iter != end && (isspace(*iter) || syntax->isPfield(*iter))) {
          ++iter;
        }

        // Then skip the instruction name.
        while (iter != end && !isspace(*iter) && !syntax->isfield(*iter)) {
          ++iter;
        }

        // Now process the string.  The gas lexer will remove whitespace unless
        // it's a separator between fields, so we can do the same to simplify
        // regex parsing.  In that case, it will collapse the whitespace to a
        // single space, further simplifying parsing.
        bool add_ws = false;
        // Tracks if we're still part of the name, i.e. fields which adjoin the
        // instruction name.
        bool was_pn = true;
        // Tracks if we had a field- for whitespace compression.
        bool was_f  = false;

        // Advance past whitespace to start of field information.
        while (iter != end && isspace(*iter)) {
          ++iter;
          // Whitespace means we're done with the name.  If nothing's here (we
          // have abutting fields, then we'll consider ourselves still in the
          // name portion.
          was_pn = false;
        }
        gc_string::const_iterator arg_start = iter;

        // Scan for non-field characters.
        gc_string::const_iterator iter2 = iter;
        // Comma is a delimiter and should be excluded
        // Add operand symbols that should be excluded; we'd better quote them if ']' is involved (to-do).
        gc_string fd = "[^" + pkt_end + "," + get_single_char(assm.operand_symbols_excluded());
        gc_string inner;
        for  ( ; iter2 != end ; ++iter2) {
          // regex does not like ']' and '-' inside [], we also remove all characters as separators.
          // '+' can be added in front of a positive integer for an operand, thus should be taken into account
          if ( !syntax->isfield(*iter2) && *iter2 != ']' && *iter2 != '-' && *iter2 != '+' && !isalnum(*iter2)) {
            inner += *iter2;
          }
        }

        fd += quoteReCharClass(inner);
        // The signed-decimal regex requires a leading +/-.
        gc_string signed_dec_fd = "([+-]" + fd + " ]+)";
        fd = "(" + fd;
        gc_string regfd = fd +" +]+)";
        fd += " ]+)";
        
        StrIntList pFieldChoices;
        bool p_has_default = false;
        while (iter != end) {
          if (syntax->isPfield(*iter)) {
            const Instr::Field &fp = syntax->get(*iter);
            const InstrField &field = *(fp._field);

            if (field.has_default_value()) {
              p_has_default = true;
            }	
        
            if (instr.getOpc(field.name()) > 0) {
              ForEach(field.enums(),i){	      
                if ((uint32_t)i->second == fp._value) {
                  pFieldChoices.push_back(*i);
                }  
              }      
            } else {
              pFieldChoices.insert(pFieldChoices.end(), field.enums().begin(), field.enums().end());
            }  	  
          }      
          ++iter;
        }

        //remove empty strings	
        if (count_if(pFieldChoices.begin(), pFieldChoices.end(), 
                     equal_1st_in_pair<gc_string,int> (EMPTY_STRING) ))
          {
            p_has_default = true;
            auto empty_strings = remove_if(pFieldChoices.begin(), pFieldChoices.end(), 
                                           equal_1st_in_pair<gc_string,int> (EMPTY_STRING) );

            pFieldChoices.erase(empty_strings, pFieldChoices.end());
          }      

        iter = arg_start;

        while (iter != end) {
          if (syntax->isfield(*iter)) {
            // Adding whitespace because it's a needed separator.
            if (add_ws) {
              re += ' ';
              add_ws = false;
            }
            was_f = true;
            const Instr::Field &fp = syntax->get(*iter);
            const InstrField &field =  *(fp._field);
            int f_ix = instr.getOpc(field.name());
            if (field.is_enumerated()) {
              // Is enumerated: Accept only the allowed strings.  If we have a
              // default value, then we allow the option to be omitted.
              re += "(";
              bool first = true;
              const StrIntList* choices; 
              bool has_empty = false;

              if (syntax->isPfield(*iter)) {
                choices = &pFieldChoices;
                has_empty = p_has_default;
              } else {
                choices = &field.enums();	
                has_empty = field.has_default_value();	   
              }

              // Note: It's not standard to have an empty sub-expression, so we
              // have to use the '?' 0 or 1 selector to denote this.
              ForEach( (*choices) ,i) {		
                if ( i->first.empty()) {
                  has_empty = true;
                }
                else if (i->first != "reserved") {
                  pfx_sep(re,"|",first);
                  re += quoteReStr(i->first);
                }
              }

              // Use a alternation with an empty option, rather than an optional
              // sub-group ('?' operator) so that the resulting args array will
              // have an empty string.
              if (has_empty) {
                re += "|";
              }
              re += ")";

              //if it is with a predefined value
            } else if (fp._type == Instr::Field::Value && f_ix > 0) {
              gc_ostringstream ss; ss << fp._value; 
              gc_string tmpstr = "(";
              if (field.regfile()) {
                const RegFile &rf = *(field.regfile());
                tmpstr += rf.prefix() + ss.str() + "|" + ss.str(); 
                ForEach(rf.entries(),ii) {
                  if (ii->first == fp._value) {
                    const RegEntry &entry = *(ii->second);          
                    tmpstr += "|" + entry.reg()()->name();
                  }
                }
              }

              tmpstr += ")";	      
              re += tmpstr;	    
            } else if (field.syntax()) { 
              // nested field with syntax
              const InstrField::Syntax *fsyntax = field.syntax();
              gc_string::const_iterator fiter = fsyntax->_fmt.begin();
              gc_string::const_iterator fend  = fsyntax->_fmt.end();

              // skip whitespaces
              while( fiter != fend && isspace(*fiter)) {
                ++fiter;
              }
              re += "(";
              while (fiter != fend) {
                if (fsyntax->isfield(*fiter)) {
                  gc_string nfd = "[^";
                  gc_string::const_iterator fiter2 = fiter;
                  gc_string inner = pkt_end;
                  for ( ; fiter2 != fend; ++fiter2) {
                    if (!syntax->isfield(*fiter2) && *fiter2 != ']' && *fiter2 != '-') {
                      inner += *fiter2;
                    }
                  }
                  nfd += quoteReCharClass(inner) + ", \\t]+";
                  const InstrField::Field &nfp = fsyntax->get(*fiter);
                  const InstrField &nfield = *(nfp._field);
                  if (nfield.is_enumerated()) {
                    re += "(";
                    bool first = true;
                    ForEach(nfield.enums(),ii) {
                      if (ii->first != "reserved") {
                        pfx_sep(re,"|",first);
                        re += quoteReStr(ii->first);
                      }
                    }
                    if (nfield.has_default_value()) {
                      pfx_sep(re,"\"\"",first);
                    }
                    re += ")";
                  } else { // not enumerated
                    re += nfd ;
                  }
                } else {
                  re += quoteReChar(*fiter);
                }
                ++fiter;
              }
              re += ")";
            } else if (field.regfile()) {
              re += regfd;
            } else if (field.display() == InstrField::SignedDec) {
              // Create a tighter regex for signed decimals in order to make
              // sure that we grab only what we need.
              re += signed_dec_fd;
              if (field.has_default_value()) {
                // Make it optional if it has a default value.
                re += "?";
              }
            } else {
              // Not enumerated:  We'll grab a sequence of non-whitespace characters.
              re += fd;
            }
          } else {
            if (isspace(*iter)) {
              if (_keep_ws) {
                // User says to keep whitespace, so we keep it.
                re += ' ';
              } else {
                if (was_f || was_pn) {
                  // We only want this white space if it is explicitly in the
                  // syntax string and not part of the operand list (follows a
                  // comma).
                  add_ws = true;
                }
              }
            } else { 
              // No need to add whitespace, as we encountered some other type of
              // separator and thus the gas lexer will remove the whitespace.
              // However, if we were part of an initial sequence, where fields
              // were part of the instruction name, then whitespace is needed,
              // as the assembler leaves it in as a separator between the
              // instruction name and the arguments.
              if (add_ws && was_pn) {
                re += ' ';
                was_pn = false;
              }
              // We're no longer following a field.
              was_f  = false;
              add_ws = false;
              re += quoteReChar(*iter);
            } 
          }
          ++iter;
        }
      } else {
        // Default:  Create a comma-delimited regular expression.
        bool first = true;
        gc_string inner = pkt_end + " ,\\t";
        // Comma is a delimiter and should be excluded
        gc_string fd = "([^" + quoteReCharClass(inner) + ",]+)";
        ForRange(instr.sizeFields(),i) {
          auto field = instr.getField(i)._field;
          if (!field->is_assembler() && !field->is_reserved()) {
            pfx_sep(re,",",first);
            re += fd;
          }
        }
      }

      // Remove a trailing whitespace regex so that it doesn't interfere with figuring
      // out if we matched against all operands.
      if (re.size() > ws.size() && re.substr(re.size()-ws.size()) == ws) {
        re.erase(re.size()-ws.size());
      }
      
      if (assm.explicit_operand_regex_end()) {
        if (re == "^") return "$";   // Replace "^" with "$"
        else return re+'$';
      }
      
      return re;
    }

    typedef gc_map<gc_string,int> StrIntMap;

    // Stores information about a field.
    struct FInfo {
      gc_string _name;         // Name of the field, (use field_indices map to look up field).
      gc_string _ifc_name;     // Name of the interface of the field, if exists, otherwise field's name
      int       _index;        // Index of field in the assembler-line list of operands.
      gc_string _flags;        // Flags used by the assembler.
      bool      _is_signed;
			bool 			_is_unsigned_upper_bound;
      bool      _is_reg;
      gc_string _regfile;      // If _is_reg is true, then this is the name of the register file.
      bool      _is_immed;
      bool      _is_asm;       // If it is an assembler field.
      bool      _is_enum;      // If the field is enumerated.
      bool      _is_modified;  // The field has a modifier - used for determining longest immediate 
      unsigned  _width;        // used to sort immeds by their width
      int       _lshift;       // The left-shift value, if any.
      unsigned  _offset;       // The field offset, if any.
      uint64_t  _lower;        // For bounds checking.
      uint64_t  _upper;     
      uint64_t  _mask;         // Low bit mask shifted operands.

      static bool _explicit_regs; // explicit_regs flag was set in ADL file

      FInfo() : _index(-1), _is_signed(false), _is_unsigned_upper_bound(false), _is_reg(false), _is_immed(false), _is_asm(false), _is_enum(false), _is_modified(false), _width(0), _lshift(0), _offset(0), _lower(0), _upper(0), _mask(0) {};

      bool operator!=(const FInfo &x) const { return !(*this == x); };

      bool operator==(const FInfo &x) const {
        return ( (_is_reg == x._is_reg) && (_regfile == x._regfile) && (_lshift == x._lshift) && (_offset == x._offset) );
      };

      // For sorting, immediates are considered 'less' than registers.
      // If explicit_regs is set in ADL then we are guaranteed that registers
      // are preceded by their prefix, hence we reverse the order.
      // Small fiedls (by upper)
      // We then sort by shift size and then offset.
      bool operator<(const FInfo &x) const {
        if ( !_is_reg && x._is_reg) {
          return _explicit_regs ? false : true;
        } else if ( _is_reg && !x._is_reg) {
          return _explicit_regs ? true : false;
        } else if ( _lshift < x._lshift) {
          return true;
        } else {
          return _offset < x._offset;
        }
      };

    };

    
    // This is a mapping from field indices, as known by the instruction, to
    // operand information, as seen by the assembler line.  For example, add
    // RT,RA,RB would have entries of 0 -> RT information, 1 -> RA information, 2
    // -> RB information, while "mr RA,RS" would map 0 -> RS, 1 -> RA, since the
    // syntax string reverses the order of RA and RS.
    typedef gc_map<unsigned,FInfo> FInfos;

    // Grab information about a single field.
    void getFieldData(FInfos &fmap,unsigned index,int op_index,const InstrField &field)
    {
      FInfo fi;

      bool first = true;
      fi._width = field.width();
      if (field.addr() == InstrField::aPcRel) {
        pfx_add(fi._flags,"ADL_RELATIVE"," | ",first);
      } else if (field.addr() == InstrField::aAbs) {
        pfx_add(fi._flags,"ADL_ABSOLUTE"," | ",first);
      }
      if (field.is_signed()) {
        if (field.is_unsigned_upper_bound()) {
          pfx_add(fi._flags,"ADL_EXT_SIGNED"," | ",first);
        } else {
          pfx_add(fi._flags,"ADL_SIGNED"," | ",first);
        }
      }
      if (field.regfile()) {
        pfx_add(fi._flags,"ADL_REGISTER"," | ",first);
        fi._is_reg = true;
        fi._regfile = field.regfile()->name();
      } else if (field.type()  == InstrField::tImm) {
        fi._is_immed = true;
      }
      if (field.is_assembler()) {
        fi._is_asm = true;
      }
      if (field.is_enumerated()) {
        fi._is_enum = true;
      }
      if (fi._flags.empty()) {
        fi._flags = "0";
      }
      
      fi._name      = (field.encap_field() ?  field.encap_field()->name() + "." + field.name() : field.name());
      fi._index     = op_index;
      fi._lshift    = field.shift();
      fi._offset    = field.offset();
      fi._is_signed = field.is_signed();
			fi._is_unsigned_upper_bound = field.is_unsigned_upper_bound();

      // We do the arithmetic in 64-bits, then assign to the values, which are
      // of type long, which should be compatible with the assembler we're
      // generating.  We may have to switch these over to always being 64-bit,
      // but we'll see.
      uint64_t lower_v,upper_v;

      // For signed fields, our value is +/- max-signed value.  If unsigned, then
      // it's 0..max-unsigned value.  Take into account the offset as well.
      if (field.is_signed()) {
        uint64_t  upper = ( 1ull << (field.size() + field.shift() - 1) ) - 1;
        if (field.is_unsigned_upper_bound()) {
          upper_v = ( 1ull << (field.size() + field.shift()) ) - 1;
        } else {
          upper_v = upper;
        }
        lower_v = ~upper;
      } else {
        lower_v = 0 + field.offset();
        upper_v = (( 1ull << (field.size() + field.shift()) ) - 1) + field.offset();
      }
      fi._lower = lower_v;
      fi._upper = upper_v;
      // Then we also set the low-bit mask if we have a shift value.
      if (field.shift()) {
        fi._mask = ( (uint64_t)1 << field.shift() ) - 1;
      }

      fmap[index] = fi;
    }


    RNumber getFieldBits(const BitPair &bp, const RNumber &v, const RNumber &res, bool ignore_endianess) 
    {
      unsigned start = bp.lo(v.size());
      unsigned end   = bp.hi(v.size());
      if (ignore_endianess) {
        if (end < start) {
          swap(start,end);
        }
        return  (res << bp.width()) | v.getUIntField(start,end);
      } else {
        if (end < start) {
          return (res << bp.width()) | v.getUIntField(res.size()-start-1,res.size()-end-1);
        } else {
          return (res << bp.width()) | v.getUIntField(start,end);
        }
      }
    }

    RNumber getFieldBits(const BitPairVect &bits, const RNumber &v, bool ignore_endianess) 
    {
      RNumber res(0,v.size());
      ForEach(bits,b) {
        res = getFieldBits(*b,v,res,ignore_endianess);
      }
      return res;
    }

    // This method uses field's fields to create a value.  If skip is set to true, then a reserved
    // value has been crossed, so the results should be skipped.
    RNumber getFieldsVal(bool &skip,const InstrField &field, const RNumber &v, bool ignore_endianess=false)
    {
      RNumber res(0,v.size());
      const InstrField::Fields &fields = 
        (field.fields().empty() && field.interface()) ? field.interface()->fields() : field.fields(); 
      ForEach(fields,f) {
        if (f->_type == InstrField::Field::Range) {
          res = getFieldBits(f->_range,v,res,ignore_endianess);
        }
        else {
          const InstrField *nfield = (field.getField(f->_name));
          assert(nfield);
          res <<= nfield->size();
          if (f->_type == InstrField::Field::TblIdx) {
            const InstrFieldTable      &t = nfield->table();
            unsigned idx =  getFieldBits(nfield->bits(),v,true).uint32();
            if (idx < t.entries().size() ) {
              const InstrFieldTable::InstrFieldTableEntry &e = (t.entries())[idx];
              if (!t.is_reserved(e) && (f->_value < e.size())) {
                res |= e[f->_value]; 
              } else {
                skip = true;
              }
            } else {
              skip = true;
            }
          } else { // it is a Ref
            res = getFieldsVal(skip,*nfield,v);
          }
        }
      }
      return res;
    }

    typedef list<RNumber>   RNumList;

    template<typename SIter, typename VIter>
    gc_string genSyntaxItems(const InstrField::Syntax *syntax,const  gc_string &pfx, SIter si, 
                          SIter se, VIter vi,unsigned k, const InstrField *f ) 
    {
      gc_ostringstream ss;

      for( ; si != se ; ++si) {
        if (syntax->isfield(*si)) {
          if (f->regfile() && f->regfile()->sparse()) {  
            const RegFile *rf = f->regfile();
            auto entry = rf->entries().find(vi->uint32());
            if (entry != rf->entries().end()) {
              genEntryName(ss,*(entry->second),*rf);
            }
          } else {
            ss << pfx << *vi;
          }
          ++vi;
        } else {
          ss << *si;
        }
      }
      return ss.str();
    }

    static bool sortEnumItems(const pair<gc_string,int> &x,const pair<gc_string,int> &y)
    {
      return x.first < y.first;
    }

    gc_string procInstrFieldSyntax(int &count,const InstrField &ifield, unsigned t)
    {
      const InstrField::Syntax *syntax = ifield.syntax();
      const gc_string &fmt = syntax->_fmt;
      const InstrField *f = ifield.is_alias() ? ifield.get_alias()->_target : &ifield;
      unsigned vmax = (1 << f->width());
      gc_vector<gc_string>  pfx;
      if (!f->regfile()) {      // TBD - handle
        pfx.push_back("");
      } else if (!f->regfile()->prefix().empty()) {
        pfx.push_back(f->regfile()->prefix());
      } else {
        pfx.push_back(f->regfile()->name());
      }

      gc_vector<pair<gc_string,int> > enums;

      StrSet        sym_vals;
      ForRange(vmax,k) { // iterate on all possible field values
        bool skip = false;
        RNumber fval = f->fields().empty() ? RNumber(k,f->size()) : getFieldsVal(skip,*f,RNumber(k,f->size()),true); 
        RNumList vals;
        ForRange(syntax->_items.size(),i) {
          const InstrField &n = *(syntax->_items[i].first._field);
          vals.push_back(getFieldsVal(skip,n,fval));
        }
        if (skip) {
          continue;
        }
        ForEach(pfx,p) {
          gc_string si;
          switch(t) {
          case Instr::Field::None:
            enums.push_back(make_pair(genSyntaxItems(syntax,*p,fmt.begin(),fmt.end(),vals.begin(),k,f),k));
            break;
          case Instr::Field::Reverse:
            enums.push_back(make_pair(genSyntaxItems(syntax,*p,fmt.begin(),fmt.end(),vals.rbegin(),k,f),k));
            break;
          case Instr::Field::Symmetric:
            // Add reverse element, exclude identical tuples
            si = genSyntaxItems(syntax,*p,fmt.begin(),fmt.end(),vals.begin(),k,f);
            if (sym_vals.find(si) == sym_vals.end()) {
              sym_vals.insert(si);
              enums.push_back(make_pair(si,k));
            }
            si = genSyntaxItems(syntax,*p,fmt.begin(),fmt.end(),vals.rbegin(),k,f);
            if (sym_vals.find(si) == sym_vals.end()) {
              sym_vals.insert(si);
              enums.push_back(make_pair(si,k));
            }
            break;
          case Instr::Field::TransCnt:
            break;
          }
        }
      }

      sort(enums.begin(),enums.end(),sortEnumItems);

      gc_ostringstream ss;
      ForRange(enums.size(),k) {
        ss << "{ \"" << enums[k].first << "\", " << enums[k].second << " },";
        if (!(k%5)) {
          ss << "\n";
        }
      }

      count = enums.size();
      return ss.str();
    }

    // This grabs information about each field, including the index as entered
    // by the user in the assembly line.
    void getInstrFieldData(FInfos &fmap,const Instr &instr)
    {
      // First, figure out the mapping between operands as seen by the user (either
      // a syntax string or the default order) and their order in the instruction.
      if (const Instr::Syntax *syntax = instr.syntax()) {
        // Has syntax string:  Use order specified there for fields.
        ForRange(syntax->_items.size(),i) {
          if (syntax->_items[i].first._type == Instr::Field::Alias) {
            continue;
          }
          const InstrField *field = (syntax->_items[i].first._field);
          if (field->is_alias()) {
            field = field->get_alias()->_target;
          }
          getFieldData(fmap,instr.getField(field->name()),i,*field);
          ForEach(field->instrfields(),n) {
            const InstrField &nfield = *(n->second);
            gc_string name = field->name() + "." + nfield.name();
            getFieldData(fmap,instr.getField(name),i,nfield);
          }
        }
        int op_index = syntax->_items.size();
        ForRange(instr.sizeFields(),i) {
          const InstrField &field = *(instr.getField(i)._field);
          if (field.is_assembler()) {
            getFieldData(fmap,i,op_index++,field);
          }
        }
      } else {
        // Default case:  List fields in the order in which they appear, skipping
        // the optional overlay fields.
        int op_index = 0;
        ForRange(instr.sizeFields(),i) {
          const InstrBase::Field &f = instr.getField(i);
          const InstrField &field = *(f._field);
          if (!field.is_overlay() &&  
              (f._type != Instr::Field::Resrvd || field.is_assembler())) {
            getFieldData(fmap,i,op_index++,field);
          }
        }
      }
    }


    // Data structure used to maintain data on prefix variables
    struct PrefixVarInfo {
      int      _default;
      int      _width;
      unsigned _field_width;
      unsigned _mask;

      PrefixVarInfo(int def,int width,int field_width,unsigned mask) : 
        _default(def),_width(width),_field_width(field_width),_mask(mask) {};
    };

    typedef gc_map<gc_string, PrefixVarInfo> PrefixVarInfos;

    struct ShortcutHelper : public Walker {

      ShortcutHelper(Environment *env) :
        Walker(env)
      {}

      const IntSet &indices() const
      {
        return _indices;
      }
      
      int get_index_from_syntax(const Instr &instr,const gc_string &var)
      {
        if (instr.syntax()) {
          ForRange(instr.syntax()->_items.size(),i) {
            if (instr.syntax()->_items[i].first._field->name() == var) {
              return i;
            }
          }      
        }
        return -1;
      }

      Ptree *shortcut_field_helper(const InstrField &field,unsigned index)
      {
        // Record the index that's used in a set, so that we hae a unique set of used items.
        _indices.insert(index);
        
        gc_string expr = "(operands[" + uitogs(index) + "].X_add_number)";
        if (field.has_checks()){
          expr = "(" + field.name() + "_checker(" + expr + ",FALSE))";
        }
        if (field.shift()) {
          expr = "("+expr+" >> " + uitogs(field.shift()) + ")";
        }
        if (field.offset()) {
          expr = "("+expr+" - " + uitogs(field.offset()) + ")";
        }
        if (field.is_signed()) {
          expr = "((int)" + addSignExt(expr,field.width(),false) + ")";
        }
        return Ptree::Make(expr.c_str());
      }

      Ptree *shortcut_cia_helper()
      {
        gc_string expr = "(int)(" + CiaVar + ")";
        return Ptree::Make(expr.c_str());
      }

      virtual Ptree* TranslateFuncall(Ptree* exp)
      {
        Ptree *fun = exp->Car();
        gc_string fname = fun->ToString();

        // If this is a bit-mask manipulation function, then translate the
        // arguments and pass on the function call.
        if (fname == Cia) {
          // Special case for the cia (program counter): Translate that as a
          // reference to the line-number variable.
          return shortcut_cia_helper();
        } else if (validAliasEscape(fname)) {
          Ptree* fun = exp->Car();
          Ptree* args = exp->Cdr();
          Ptree* args2 = TranslateArguments(args);
          if(args == args2)
            return exp;
          else
            return new PtreeFuncallExpr(fun, args2);
        } else {
          return Walker::TranslateFuncall(exp);
        }
      }

    protected:
      IntSet         _indices;
    };

    // This walker merely registers instruction attributes queried in assembler handlers.
    class InstrAttrWalker : public AsmActionWalker {
    public:
      InstrAttrWalker(const Core &core, IntSet &instr_attrs) : 
        AsmActionWalker(core.env()), _core(core), _instr_attrs(instr_attrs), _msgs_used(false) {}
      
      virtual Ptree *TranslateFuncall(Ptree *exp) 
      {
        Ptree *fun = exp->Car();
        Ptree *r;
        if (Eq(fun,"instrHasAttr") || 
            (Match(fun,"[ %* . %? ]",&r) && Eq(r,"instrHasAttr")) ) {
          if(get_func_call_numargs(exp) != 1) {
            RError("instrHasAttr takes a single argument.");
          }
          Ptree *arg = get_func_call_arg(exp,0);
          gc_string attr = get_str(arg,"instrHasAttr argument.");
          int a_id = Data::get_attr(attr);
          if (a_id < 0) {
            RError("Unknown attribute in instrHasAttr call " << attr);
          }
          _instr_attrs.insert(a_id);
        } 
        else if (Eq(fun,"instrHasAttrVal") || 
            (Match(fun,"[ %* . %? ]",&r) && Eq(r,"instrHasAttrVal")) ) {
          if(get_func_call_numargs(exp) != 2) {
            RError("instrHasAttrVal takes two arguments.");
          }
          Ptree *arg = get_func_call_arg(exp,0);
          gc_string attr = get_str(arg,"instrHasAttrVal argument.");
          int a_id = Data::get_attr(attr);
          if (a_id < 0) {
            RError("Unknown attribute in instrHasAttr call " << attr);
          }
          _instr_attrs.insert(a_id);
        } 
        else if (Eq(fun,"info") || Eq(fun,"error")) {
          _msgs_used = true;
        }
        return Walker::TranslateFuncall(exp);
      }
      
      bool msgs_used() const { return _msgs_used; } 
      
    private:
      const Core  &_core;
      IntSet      &_instr_attrs;
      bool         _msgs_used;
    };
    
    // Simple walker transforms an expression using instruction fields into an
    // expression referencing operand array elements.
    class ShExprWalker : public ShortcutHelper {
    public:
      ShExprWalker(const Instr &instr,GenBase &base) : 
        ShortcutHelper(instr.env()), 
        _instr(instr), 
        _base(base) {};

      virtual Ptree* TranslateVariable(Ptree *exp)
      {
        int index;
        auto var =  exp->ToString();
        if ((index = _instr.getField(var)) >= 0) {
          const InstrField &field = *(_instr.getField(index)._field);
         
          // If we have a syntax string, then we must look up the item relative to
          // its position in the syntax string.
          int s_index = get_index_from_syntax(_instr,var);
          if (s_index >= 0) {
            index = s_index;
          }
         
          return shortcut_field_helper(field,index);
        } else {
          RError ("Unknown variable found in shortcut expression:  " << var );
        }
      
      }

    protected:
      gc_string getField(const InstrField &field)
      {
        return genGetField(_base._core,field,"",_instr.width(),"insn",SimplFld,true);
      }

    private:
      const Instr   &_instr;
      GenBase       &_base;
    };

    class SyntaxExprWalker : public ShortcutHelper {
    public:
      SyntaxExprWalker(const Instr &instr) : 
        ShortcutHelper(instr.env()), 
        _instr(instr) {};

      virtual Ptree* TranslateVariable(Ptree *exp)
      {
        gc_string var = exp->ToString();
        int index = get_index_from_syntax(_instr,var);
        if (index  >= 0) {
          const InstrField &field = *(_instr.syntax()->_items[index].first._field);
          return shortcut_field_helper(field,index);
        } else {
          RError ("Unknown variable found in syntax expression:  " << var );
        }	
      }

    private:
      const Instr &_instr;
    };

    class ValidityWalker : public ShortcutHelper {
    public:
      ValidityWalker(const Instr &instr) : 
        ShortcutHelper(instr.env()), 
        _instr(instr), 
        _checked(false) {};

      bool checked()  const { return  _checked; };
      
      virtual Ptree* TranslateInfix(Ptree *exp)
      {
        Ptree* left = PtreeUtil::First(exp);
        Ptree* left2 = Translate(left);
        Ptree* right = PtreeUtil::Third(exp);
        Ptree* right2 = Translate(right);
        int    val;
        if (get_int(val,left2,env)) {
          if (get_int(val,right2,env)) {
            return Ptree::Make(";");
          }
          return right2;
        }
        if (get_int(val,right2,env)) {
          return left2;
        }
        return Ptree::Make("%p%p",left2,right2);
      }

      virtual Ptree *TranslateCond(Ptree *exp) 
      {
        Ptree* c = PtreeUtil::First(exp);
        Ptree* c2 = Translate(c);

        Ptree* t = PtreeUtil::Third(exp);
        Ptree* t2 = Translate(t);
        Ptree* e = PtreeUtil::Nth(exp,4);
        Ptree* e2 = Translate(e);
	
        int val;
        if (get_int(val,c2,env)) {
          if (get_int(val,t2,env)) {
            return get_int(val,e2,env) ? Ptree::Make(";")  : e2;
          } else {
            return get_int(val,e2,env) ? t2 : Ptree::Make("%p%p",t2,e2);
          }
        } else {
          if (get_int(val,t2,env)) {
            return get_int(val,e2,env) ? c2 : Ptree::Make("%p%p",c2,e2);
          } else {
            return get_int(val,e2,env) ? Ptree::Make("%p%p",c2,t2) : Ptree::Make("%p%p%p",c2,t2,e2);
          }
        }
      }

      virtual Ptree* TranslateParen(Ptree *exp) 
      {
        Ptree *e = PtreeUtil::Second(exp);
        return Translate(e);
      }

      virtual Ptree *TranslateUnary(Ptree *exp) 
      {
        Ptree *e = PtreeUtil::Second(exp);
        return Translate(e);
      }

      virtual Ptree* TranslateVariable(Ptree *exp)
      {
        gc_string var = exp->ToString();
        int index = get_index_from_syntax(_instr,var);
        if (index  >= 0) {
          const InstrField &field = *(_instr.syntax()->_items[index].first._field);
          if (field.has_checks() && _valids.find(&field) == _valids.end()) {
            _valids.insert(&field);
            _checked = true;
            gc_string expr = "if (!" + field.name() + "_checker(" 
              + "operands[" + uitogs(index) + "].X_add_number,TRUE)) return FALSE;\n";
            return Ptree::Make(expr.c_str());
          } else {
            return Ptree::Make(";"); // NULL instruction;
          }
        } else {
          RError ("Unknown variable found in syntax expression:  " << var );
        }	
      }

    private:
      set<const InstrField*> _valids;
      const Instr &_instr;
      bool  _checked;
    };

    // Walker for post-asm hook,
    // Does the following : all prefix field accesses are translated to ".value".
    class PostAsmWalker : public AsmActionWalker {
    public:
      PostAsmWalker(const Core &core, const PrefixVarInfos &pfx_vars,
                    const StrIntMap &field_indices,
                    const AsmParmHash &asm_parms) : 
        AsmActionWalker(core.env()), _core(core), _pfx_vars(pfx_vars),
        _field_indices(field_indices), _asm_parms(asm_parms),
        _queue_size(core.asm_config().queue_size()),_mode(NoMode),
        _in_assign(false), _funcall(false) {};

      enum Mode { NoMode, PreAsmMode, PostAsmMode, InstrRuleMode, PacketRuleMode };

      virtual Ptree* TranslateVariable(Ptree *exp)
      {
        int id = -1;
        if (is_prefix_var(exp)) {
          if (_queue_size > 1) {
            RError("Queue index is missing for variable:  " << exp );
          }
          if (_in_assign && _funcall) {
            return Ptree::Make("%p[current_position]",exp);
          } else {
            return Ptree::Make("%p[current_position].value",exp);
          }
        } else if ((id = get_field_id(exp)) != -1) {
          int ifc_id = get_field_iface_id(exp);
          Ptree *pos = current_group_index();
          return Ptree::Make("adl_get_field(%d,%d,%p,current_position)",id,ifc_id,pos);
        } else if ((id = get_reg_id(exp)) != -1) {
          return Ptree::Make("%d",id);
        }
        return AsmActionWalker::TranslateVariable(exp); 
      }

      Ptree *TranslateDeclaration(Ptree *def) 
      {
        Ptree *tspec = PtreeUtil::Second(def);
        if (Eq(tspec,"RegWrites")) {
          Ptree *decls =  PtreeUtil::Third(def);
          Ptree *var,*rhs;
          if (Match(decls,"[[%? = %?]]",&var,&rhs)) {
            RError("RegWrites can not be initialized: " << def);
          } else if (Match(decls,"[ %? ]",&var)) {
            gc_string name = get_str(var,"RegWrite variable.");
            _regwrite_vars.insert(name);
            return Ptree::Make("struct reg_writes %s;\n%s.size=0;",name.c_str(),name.c_str());
          } else {
            RError("Incorrect declaration: " << def);
          }
        }  
        return AsmActionWalker::TranslateDeclaration(def);
      }

      Ptree *TranslateArray(Ptree *exp) 
      {
        Ptree *array = exp->Car();
        Ptree* index = PtreeUtil::Third(exp);
        Ptree* index2 = Translate(index);
        if (is_prefix_var(array)) {
          uint32_t idx = get_uint(index2,"queue index.",env);
          if (idx > _queue_size -1) {
            RError("Out of range queue index : " <<  exp);
          }
          if (_in_assign && _funcall) {
            return Ptree::Make("%p[(current_position + %d - (%p)) %%  %d]",array,_queue_size,index2,_queue_size);
	  
          } else {
            return Ptree::Make("%p[(current_position + %d - (%p)) %%  %d].value",array,_queue_size,index2,_queue_size);
          }
        } else if (is_regwrite_var(array))  {
          return index2;
        }
        return AsmActionWalker::TranslateArray(exp);
      }

      Ptree *TranslateAssign(Ptree *exp) 
      {
        Ptree *left = PtreeUtil::First(exp);
        Ptree *right = PtreeUtil::Third(exp);
        gc_string rhs = right->ToString();
        gc_string lhs = left->ToString();
        Ptree  *a,*m;
        int id = -1;
        if (rhs == "'*'") {
          if (is_label_parm(lhs)) {
            return Ptree::Make("%p = curr_label()",left);
          } else {
            RError("Illegel expression:  " << exp);
          }
        } else if ((id=get_field_id(left)) != -1) { // assginment to InstrField
          int ifc_id = get_field_iface_id(left);
          Ptree *right2 = Translate(right);
          bool string_val = is_string_val(right2);
          if (_mode == PreAsmMode) {
            if (string_val) {
              return Ptree::Make("adl_pre_set_enum_field(%d,%d,%p)",id,ifc_id,right2);
            } else {
              return Ptree::Make("adl_pre_set_field(%d,%d,%p)",id,ifc_id,right2);
            }
          } else if (_mode == PostAsmMode) {
            Ptree *pos = current_group_index();
            if (string_val) {
              return Ptree::Make("adl_set_enum_field(%d,%d,%p,%p,current_position)",id,ifc_id,right2,pos);
            } else {
              return Ptree::Make("adl_set_field(%d,%d,%p,%p,current_position)",id,ifc_id,right2,pos);
            }
          } else { // Rules  cannot modify fields
            RError("Illegel expression:  " << exp);
          }
        } else if (Match(left,"[ %? . %?]",&a,&m) && ((id = get_field_id(m)) != -1)) {
          int ifc_id = get_field_iface_id(m);
          if (is_packet_arg(a)) {
            Ptree *right2 = Translate(right);
            if (is_string_val(right2)) {
              return Ptree::Make("adl_set_enum_field(%d,%d,%p,%p)",id,ifc_id,right2,a);
            } else {
              return Ptree::Make("adl_set_field(%d,%d,%p,%p)",id,ifc_id,right2,a);
            }
          }
        } else {
          _in_assign = true;
          Ptree *left2 = Translate(left);
          Ptree *right2 = Translate(right);
          if (_funcall) {
            _funcall = _in_assign = false;
            return Ptree::Make("adl_set_prefix_field_slice(%p,%p)",left2,right2);
          }
          _in_assign = false;
        }
        return AsmActionWalker::TranslateAssign(exp);
      }

      Ptree *TranslateIf(Ptree *exp)
      {
        Ptree *f,*g;
        Ptree *cond  = PtreeUtil::Third(exp);
        Ptree *cond2 = cond;
        Ptree* then_part = PtreeUtil::Nth(exp,4);
        Ptree* then_part2 = Translate(then_part);
        Ptree* else_part = PtreeUtil::Nth(exp,6);
        Ptree* else_part2 = Translate(else_part);
	
        if (PtreeUtil::Match(cond2,"[ %? == '*']",&f) || 
            PtreeUtil::Match(cond2,"[ '*' == %?]",&f))   {
          Ptree *cond3;
          gc_string nm = f->ToString();
          if (is_label_parm(nm)) {
            cond3 = Ptree::Make("%p == curr_label()",f);
          } else {
            RError("Illegal condition:  " << cond2);
          }

          Ptree* rest = PtreeUtil::ShallowSubst(cond3, cond, then_part2, then_part,
                                                else_part2, else_part, exp->Cdr());
          return new PtreeIfStatement(exp->Car(), rest);
        } else if (PtreeUtil::Match(cond2,"[ %? ==  %? ]",&f, &g)) {
          Ptree   *rb,*rfield,*r = 0,*cond3 = 0;
          const RegBase *regb = 0;
          gc_string  mask;

          if (Match(f,"[ %? . %?]",&rb,&rfield) && isRegBaseField(rb,rfield,mask)) {
            r = Translate(g);
          } else if (Match(g,"[ %? . %?]",&rb,&rfield) && isRegBaseField(rb,rfield,mask)) {
            r = Translate(f);
          } else {
            gc_string fname = f->ToString();
            gc_string gname = g->ToString();
            if ((regb = _core.get_reg(fname)) || (regb = _core.get_regfile(fname))) {
              r = Translate(g);
              cond3 = Ptree::Make("%p == %d",r,regb->id());
            } else if ((regb = _core.get_reg(gname)) || (regb = _core.get_regfile(gname))) {
              r = Translate(f);
              cond3 = Ptree::Make("%p == %d",r,regb->id());
            }
          }

          if (!mask.empty()) {
            RNumber rmask(mask.c_str());
            gc_ostringstream  ss;
            ss << "0x" << hex << rmask.uint64() << "ULL" << ends;
	  
            cond3 = Ptree::Make("%p == %s",r,ss.str().c_str());
          } 
          if (cond3) {
            Ptree* rest = PtreeUtil::ShallowSubst(cond3, cond, then_part2, then_part,
                                                  else_part2, else_part, exp->Cdr());
            return new PtreeIfStatement(exp->Car(), rest);
          }
        } 

        cond2 = Translate(cond);
        if (cond2 != cond) {
          Ptree* rest = PtreeUtil::ShallowSubst(cond2, cond, then_part2, then_part,
                                                else_part2, else_part, exp->Cdr());
          return new PtreeIfStatement(exp->Car(), rest);
        }
        return AsmActionWalker::TranslateIf(exp);
      }
      
      Ptree *TranslateDotMember(Ptree *exp) 
      {
        Ptree *left  = exp->Car();
        Ptree *member = Third (exp);
        gc_string mask;

        int    id  = get_field_id(member);
        if (id != -1 && is_packet_arg(left)) {
          int ifc_id = get_field_iface_id(member);
          return Ptree::Make("adl_get_field(%d,%d,%p)",id,ifc_id,left);
        } else if (isRegBaseField(left,member,mask)) {
          RNumber rmask(mask.c_str());
          gc_ostringstream  ss;
          ss << "0x" << hex << rmask.uint64() << "ULL" << ends;

          return Ptree::Make("%s",ss.str().c_str());
        }
        return AsmActionWalker::TranslateDotMember(exp);
      }


      Ptree *handleFuncall(Ptree *fun,Ptree *exp,Ptree *pos) 
      {
        int numargs = get_func_call_numargs(exp);

        if (Eq(fun,"get_dest")) {
          if (numargs != 1) {
            RError("get_dest function takes one argument.");
          }
          Ptree  *arg = get_func_call_arg(exp,0);
          if (!is_regwrite_var(arg)) {
            RError("get_dest argument must be of type RegWrites.");
          }
          return Ptree::Make("get_dest(&%p,adl_get_instr_position(%p,current_position),current_position)",arg,pos);
        } else if (Eq(fun,"intersect_dest")) {
          if (numargs != 3) {
            RError("intersect_dest function takes three arguments.");
          }
          Ptree *arg1 = get_func_call_arg(exp,0);
          Ptree *arg2 = get_func_call_arg(exp,1);
          Ptree *arg3 = get_func_call_arg(exp,2);
          if (!is_regwrite_var(arg1) || !is_regwrite_var(arg2) || !is_regwrite_var(arg3)) {
            RError("intersect_dest arguments must be of type RegWrites.");
          }
          return Ptree::Make("intersect_dest(&%p,&%p,&%p)",arg1,arg2,arg3);
        }
        return exp;
      }
    
      Ptree *TranslateFuncall(Ptree *exp) 
      {
        Ptree *fun = exp->Car();
        Ptree *l,*r;

        if (Eq(fun,"intersect_dest")) {
          return handleFuncall(fun,exp,current_group_index());
        } 
        else if (Eq(fun,"combineInstr")) {   // Combine micro instructions into a macro one
          // Add in the argument count parameter to the call.
          Ptree *exp2 = AsmActionWalker::TranslateFuncall(exp);
          int num_args = get_func_call_numargs(exp)-1;
          if (num_args < 1) {
            RError("Too few arguments for combineInstr()");
          }
          Ptree *args = Second(exp->Cdr());
          Ptree *args2 = PtreeUtil::Cons(Ptree::Make(","),args);
          args2 = PtreeUtil::Cons(Ptree::Make("%d",num_args),args2);
          return new PtreeFuncallExpr(exp2->Car(),PtreeUtil::ShallowSubst(args2,args,exp2->Cdr()));
        } 
        else if (Eq(fun,"patchInstr")) {   // Patch a macro instruction at the 32 LSBs with a micro one  
          // Add in the argument count parameter to the call.
          int num_args = get_func_call_numargs(exp);
          if (num_args != 3) {
            RError("Invalid number of arguments for patchInstr()");
          }
          return AsmActionWalker::TranslateFuncall(exp);
        }
        else if (Eq(fun,"error") || Eq(fun,"info")) {
          // Convert arguments to insert into a stringstream, then call the appropriate messaging function.
          PtreeArray args;
          bool changed = false;
          populateArgs(args,this,exp,0,changed);
          Ptree *pos;
          switch((int) _mode) {
          case PacketRuleMode: 
            pos = Zero;
            break;
          default:
            pos = current_group_index();
            break;
          }
          shallowReplace(args,",",Ptree::Make(" << "));
          // Ignore the first parameter.
          args[0] = 0;
          return Ptree::Make("{ std::ostringstream ss; ss %p; %p(ss.str().c_str(),%p,adl_get_current_group()); }",args.All(),fun,pos);
        } else if (Match(fun,"[ %? . %? ]",&l,&r)) {
          if (Eq(r,"mask")) {
            Ptree *pos = Translate(l);
            Ptree *var = l->Car();
            if (get_func_call_numargs(exp) != 1) {
              RError("mask function takes one argument.");
            }
            Ptree *idx = get_func_call_arg(exp,0);
            unsigned i;
            if (get_uint(i,idx,env) && i > 1) {
              RError("mask argument can be 0 or 1.");
            }
            return Ptree::Make("(%p.rwrites[%p].mask[%p])",var,pos,idx);
          } else if (Eq(r,"reg")) {
            Ptree *pos = Translate(l);
            Ptree *var = l->Car();
            if (get_func_call_numargs(exp) != 0) {
              RError("reg function takes zero arguments.");
            }
            return Ptree::Make("(%p.rwrites[%p].id)",var,pos);
          } else if (Eq(r,"get_dest")) {
            Ptree *pos = Translate(l);
            return handleFuncall(r,exp,pos);
          } else if (Eq(r,"instrHasField")) {
            if (get_func_call_numargs(exp) != 1) {
              RError("instrHasField function expects 1 argument.");
            }
            Ptree *a = get_func_call_arg(exp,0);
            int id = get_field_id(a);
            if (id < 0) {
              RError("instrHasField function expects a valid instruction field name.");
            }
            int ifc_id = get_field_iface_id(a);
            return Ptree::Make("adl_query_field(%d,%d,%p)",id,ifc_id,l);
          }
        } else if (_in_assign) {
          if (is_gen_prefix_var(fun)) {
            _funcall = true;
            Ptree *args  = exp->Cdr();
            Ptree *args2 = Second(TranslateArguments(args));
            Ptree *fun2 = Translate(fun);
            return Ptree::Make("&%p,%p",fun2,args2);
          }
        }
        return AsmActionWalker::TranslateFuncall(exp);
      }
    

      void set_mode(Mode m)      { _mode = m; };
      void clear_mode() 	       { _mode = NoMode; };
      void set_packet_arg_name(Ptree *p) { _packet_arg_name = p->ToString(); };
      void clear_packet_vars() 	       
      { 
        _packet_arg_name.clear(); 
        _regwrite_vars.clear(); 
        set_mode(NoMode);
      }

      int get_field_iface_id(Ptree *exp) {
        const InstrField *f = _core.get_instrfield(exp->ToString());
        if (f && !f->is_prefix()) {
          const InstrField *ifc_field = f->interface() ? f->interface() : f;
          while (ifc_field->type() == InstrField::tRefField) {
            ifc_field = ifc_field->field_type();
          }
          gc_string ifc_name = ifc_field->name();
          auto it = _field_indices.find(ifc_field->name());
          if (it == _field_indices.end()) {
            RError("Unknown instruction field: " << exp->ToString());
          }
          return it->second;
        }
        return -1;
      }

      int get_field_id(Ptree *exp) {
        const InstrField *f = _core.get_instrfield(exp->ToString());
        if (f && !f->is_prefix()) {
          auto it = _field_indices.find(f->name());
          if (it == _field_indices.end()) {
            RError("Unknown instruction field: " << exp->ToString());
          }
          return it->second;
        }
        return -1;
      }

    protected:

      bool is_gen_prefix_var(Ptree *exp) 
      {
        return (is_prefix_var(exp) || (!exp->IsLeaf() && is_prefix_var(exp->Car())));
      }

      bool is_prefix_var(Ptree *v)
      {
        return (_pfx_vars.find(v->ToString()) != _pfx_vars.end());
      }

      // save it for setters 
      bool is_string_val(Ptree *exp) 
      {
        const char *n = exp->ToString();
        return (n[0] == '"');
      }

      bool is_label_parm(gc_string &n) 
      {
        auto it = _asm_parms.find(n);
        return (it != _asm_parms.end() && it->second->type() == AsmParm::Label);
      }

      // Try to figure out if p is an expression of type InstrBundle.  This can be complex b/c p can be either an array
      // expression or a dot-form method call.
      bool is_packet_arg(Ptree *p) 
      {
        bool is_type_name;
        TypeInfo t;
        // Should be either an identifier, array or a method call.
        if (!p->IsLeaf()) {
          p = p->Car();
          // Do we now have a dot-form?
          if (p->IsA(ntDotMemberExpr)) {
            p = p->Car();
          }
        }
        env->Lookup(p,is_type_name,t);
        if (t.IsReferenceType()) {
          t.Dereference();
        }
        Ptree *type = t.MakePtree();
        if (!type->IsLeaf()) {
          type = type->Car();
        }
        if (!type->IsLeaf() && type->Car()->IsA(CONST)) {
          type = Second(type);
        }
        return (!is_type_name && (Eq(type,"InstrInfo") || Eq(type,"InstrBundle")));
      }
   
      bool is_regwrite_var(Ptree *p) 
      {
        return (_regwrite_vars.find(p->ToString()) != _regwrite_vars.end());
      }

      Ptree *current_group_index() 
      {
        return Ptree::Make("adl_get_current_pos()");
      }

      int get_reg_id(Ptree *exp) 
      {
        const RegBase *rb;
        if ((rb = _core.get_regfile(exp->ToString())) || (rb = _core.get_reg(exp->ToString()))) {
          return rb->id();
        }
        return -1;
      }

      bool isRegBaseField(Ptree *r,Ptree *f,gc_string &mask) 
      {
        Ptree *reg;
        const RegBase *rb = 0;
        if (Match(r,"[ %? ( %* ) ]",&reg)) {
          rb = _core.get_regfile(reg->ToString());
        } else {
          rb = _core.get_reg(r->ToString());
        }
        if (rb) {
          const RegFieldHash &fields = rb->fields();

          auto it = fields.find(f->ToString());
          if (it != fields.end()) {
            mask = it->second->write_mask().str();
            return true;
          }
        }
        return false;
      }

    private:
      const Core        &_core;
      const PrefixVarInfos &_pfx_vars;
      const StrIntMap   &_field_indices;
      const AsmParmHash &_asm_parms;
      gc_string       _packet_arg_name;
      StrSet	 _regwrite_vars;
      unsigned _queue_size;
      Mode  _mode;
      bool  _in_assign;
      bool  _funcall;
    };	      
  
    // Walker for assember instruction action
    // Translates accesses to instructions fields to accesses to corresponding 
    // values in a values array
    class AssemblerWalker : public Walker {
    public:
      AssemblerWalker(const Core &core, const StrIntMap &field_indices, const PrefixVarInfos &pfx_vars, const AsmParmHash &asm_parms) : 
        Walker(core.env()), _field_indices(field_indices), _pfx_vars(pfx_vars), _asm_parms(asm_parms)
        ,_queue_size(core.asm_config().queue_size()),_uses_operands(false){};

      bool uses_operands() { return _uses_operands; };
    
      virtual Ptree *TranslateVariable(Ptree *exp) {
        gc_string var = exp->ToString();
        auto it = _field_indices.find(var);
        if (it != _field_indices.end()) {
          _uses_operands = true;
          return Ptree::Make("operands[%d]",it->second);
        }
        if (is_prefix_var(exp)) {
          if (_queue_size > 1) {
            RError("Queue index is missing for variable:  " << exp );
          }
          return Ptree::Make("%p[current_position].value",var.c_str());
        }

        return Walker::TranslateVariable(exp);
      }


      Ptree *TranslateAssign(Ptree *exp) 
      {
        Ptree *left = PtreeUtil::First(exp);
        Ptree *right = PtreeUtil::Third(exp);
        gc_string rhs = right->ToString();
        gc_string lhs = left->ToString();
        if (rhs == "'*'") {
          if (is_label_parm(lhs)) {
            return Ptree::Make("%p = curr_label()",left);
          } else {
            RError("Illegel expression:  " << exp);
          }
        }
        auto it = _field_indices.find(rhs);
        if (it != _field_indices.end()) {
          _uses_operands = true;    
          if (is_label_parm(lhs)) {
            return Ptree::Make("%p = operands[%d].symbol",left,it->second);
          } else {
            return Ptree::Make("%p = operands[%d].val",left,it->second);
          }
        }
        return Walker::TranslateAssign(exp);
      }

      Ptree *TranslateArray(Ptree *exp) 
      {
        Ptree *array = exp->Car();
        Ptree* index = PtreeUtil::Third(exp);
        Ptree* index2 = Translate(index);

        if (is_prefix_var(array)) {
          uint32_t idx = get_uint(index2,"queue index.",env);
          if (idx > _queue_size -1) {
            RError("Out of range queue index : " <<  exp);
          }
          return Ptree::Make("%p[(current_position + %d - (%p)) %% %d].value",array,_queue_size,index2,_queue_size);
        }
        return Walker::TranslateArray(exp);
      }

      Ptree *TranslateIf(Ptree *exp)
      {
        Ptree *l,*f;
        Ptree *cond  = PtreeUtil::Third(exp);
        Ptree *cond2 = cond;
        Ptree* then_part = PtreeUtil::Nth(exp,4);
        Ptree* then_part2 = Translate(then_part);
        Ptree* else_part = PtreeUtil::Nth(exp,6);
        Ptree* else_part2 = Translate(else_part);
	
        if (PtreeUtil::Match(cond2,"[ %? == '*']",&f) || 
            PtreeUtil::Match(cond2,"[ '*' == %?]",&f))   {
          gc_string nm = f->ToString();
          auto it = _field_indices.find(nm);
          Ptree *cond3;
          if (it != _field_indices.end()) {
            _uses_operands = true;
            cond3 = Ptree::Make("operands[%d].symbol == curr_label()",it->second);
          } else if (is_label_parm(nm)) {
            cond3 = Ptree::Make("%p == curr_label()",f);
          } else {
            RError("Illegal condition:  " << cond2);
          }

          Ptree* rest = PtreeUtil::ShallowSubst(cond3, cond, then_part2, then_part,
                                                else_part2, else_part, exp->Cdr());
          return new PtreeIfStatement(exp->Car(), rest);
        } else if (PtreeUtil::Match(cond2,"[ %? == %?]",&l,&f)) {
          auto itl = _field_indices.find(l->ToString());
          auto itf = _field_indices.find(f->ToString());
          bool  f_isop = (itf != _field_indices.end());
          bool  l_isop = (itl != _field_indices.end());
          Ptree *cond3;
          if (f_isop) {
            _uses_operands = true;
            if (l_isop) {
              cond3 = Ptree::Make("(operands[%d].symbol == operands[%d].symbol) && (operands[%d].val == operands[%d].val))",itl->second,itf->second,itl->second,itf->second);
            } else {
              cond3 = Ptree::Make("%p == operands[%d].val",l,itf->second);
            }
          } else {
            if (l_isop) {
              _uses_operands = true;
              cond3 = Ptree::Make("operands[%d].val == %p",itl->second,f);
            } else {
              return Walker::TranslateIf(exp);
            }
          }
          Ptree* rest = PtreeUtil::ShallowSubst(cond3, cond, then_part2, then_part,
                                                else_part2, else_part, exp->Cdr());
          return new PtreeIfStatement(exp->Car(), rest);
        }
        return Walker::TranslateIf(exp);
      }

    protected:
      bool is_prefix_var(Ptree *v) 
      {
        return (_pfx_vars.find(v->ToString()) != _pfx_vars.end());
      }

      bool is_label_parm(gc_string &n) 
      {
        auto it = _asm_parms.find(n);
        return (it != _asm_parms.end() && it->second->type() == AsmParm::Label);
      }

      
    private:
      const StrIntMap   &_field_indices;
      const PrefixVarInfos &_pfx_vars;
      const AsmParmHash &_asm_parms;
      unsigned  _queue_size;
      bool _uses_operands;
    };
  
    static bool sortEnums(const pair<gc_string,int> *x, const pair<gc_string,int> *y)
    {
      return x->first < y->first;
    }

    int writeEnums(ostream &o,const InstrField &field)
    {
      int count = 0;
      gc_vector<const pair<gc_string,int> *> enums;
      ForEach(field.enums(),j) {
        if ( j->first != ReservedEnum ) { 
          enums.push_back( & (*j) );
          ++count;
        }
      }

      sort(enums.begin(),enums.end(),sortEnums);

      ForEach(enums,j) {
        o << "{ \"" << (*j)->first << "\", " << (*j)->second << " }, ";
      }
      return count;
    }

    int writeSparseRegEnums(ostream &o,const RegFile &rf)
    {
      gc_vector<pair<gc_string,int> > enums;
      int count = 0;
      gc_ostringstream ss;
      ForEach(rf.entries(),i) {
        genEntryName(ss,*(i->second),rf);
        enums.push_back(make_pair(ss.str(),i->first));
        ss.str("");
        ++count;
      }

      sort(enums.begin(),enums.end(),sortEnumItems);

      ForEach(enums,i) {
        o << "{ \"" << i->first << "\", " << i->second << " }, ";
      }

      return count;
    }

    int writeRegEnums(ostream &o,const RegFile &rf)
    {
      gc_vector<pair<gc_string,int> > enums;

      gc_ostringstream ss;
      gc_string pfx = rf.prefix().empty() ? rf.name() : rf.prefix();
      int count = 0;
      for(int i = 0; i != (int)rf.size(); ++i) {
        ss << pfx << i;
        enums.push_back(make_pair(ss.str(),i));
        ss.str("");
        ++count;
      }

      sort(enums.begin(),enums.end(),sortEnumItems);

      ForEach(enums,i) {
        o << "{ \"" << i->first << "\", " << i->second << " }, ";
      }

      return count;
    }
  
    // Generates arrays which store enumerated field information.  The names of these arrays
    // are then indexed by the instruction field's pointer.
    void genEnumFieldData(ostream &o) {
          
      _enum_arrays.clear();

      o << dec;
      ForEach(_core.instrfields(),i) {
        const InstrField &field = *(i->second);
        if (field.is_enumerated()) {
          Ptree *s = AdlGenSym();
          Ptree *s2 = AdlGenSym();
          o << "// For field " << field.name() << "\n"
            << "enum_field " << s << "[] = { ";
          int count = writeEnums(o,field);
          o << "};\n"
            << "enum_fields " << s2 << " = { " << s << " , " << count << " };\n\n"; 
          _enum_arrays[&field] = s2->ToString();
        } 
        else if (field.syntax()) {
          ForRange(Instr::Field::TransCnt,t) {
            Ptree *s = AdlGenSym();
            Ptree *s2 = AdlGenSym();
            switch (t) {
            case Instr::Field::None:
              o << "// For plain field " << field.name() << "\n";
              break;
            case Instr::Field::Reverse:
              o << "// For reverse field " << field.name() << "\n";
              break;
            case Instr::Field::Symmetric:
              o << "// For symmetric " << field.name() << "\n";
              break;
            }
            int count = 0;
            gc_string enums = procInstrFieldSyntax(count,field,t);
            o << "struct enum_field " << s << "[] = { "
              << enums
              << " };\n"
              << "enum_fields " << s2 << " = { " << s << " , " << count << " };\n\n"; 
            _trans_field_array[make_pair(&field, t)] = s2->ToString();
          }
        } 
        else if (field.regfile() && field.regfile()->sparse()) {
          Ptree *s = AdlGenSym();
          Ptree *s2 = AdlGenSym();
          const RegFile *rf = field.regfile();
          o << "// For field (sparse regfile referencing) " << field.name() << "\n"
            << "struct enum_field " << s << "[] = { ";
          int count = writeSparseRegEnums(o,*rf);
          o << " };\n"
            << "enum_fields " << s2 << " = { " << s << " , " << count << " };\n\n"; 
          _enum_arrays[&field] = s2->ToString();	
        } 
        else if (field.regfile() && _core.asm_config().explicit_regs()) {
          Ptree *s = AdlGenSym();
          Ptree *s2 = AdlGenSym();
          const RegFile *rf = field.regfile();
          o << "// For field (Usual regfile referencing, no regnames) " << field.name() << "\n"
            << "struct enum_field " << s << "[] = { ";
          int count = writeRegEnums(o,*rf);
          o << " };\n"
            << "enum_fields " << s2 << " = { " << s << " , " << count << " };\n\n"; 
          _enum_arrays[&field] = s2->ToString();	
        } 	    
      }
    }

    gc_string getTransFieldArrayName(const InstrField *field,Instr::Field::Transform t) 
    {
      auto i = _trans_field_array.find(make_pair(field,t));
      if (i != _trans_field_array.end()) {
        return "&" + i->second;
      } else {
        return "";
      }	  
    }

    // Returns the name of an enumeration array, if one exists for this field, otherwise
    // returns a string representing a null-pointer.
    gc_string getEnumArrayName(const InstrField *field)
    {
      auto i = _enum_arrays.find(field);
      if (i != _enum_arrays.end()) {
        return "&" + i->second;
      } else {
        return "0";
      }
    }

    // Generate an array storing enumerated-field data for an instruction.
    Ptree *genEnumArray(ostream &o,const Instr &instr)
    {
      Ptree *name = AdlGenSym();

      o << "struct enum_fields *" << name << "[] = { ";

      if (const Instr::Syntax *syntax = instr.syntax()) {
        // find start of intermediate pseudo suffix.
        // i.e, if syntax has has (0,k) fields and 
        // all fields in (j,k) are intermediate pseudo fields find j

        // We have a syntax string- iterate over the fields as listed there.
        ForEach(syntax->_items,i) {
          const InstrField *field = i->first._field;

          // getTransFieldArrayName returns "" if not found
          gc_string field_enum = field->syntax() ? getTransFieldArrayName(field,i->first._trans) : "";
          if (field_enum == "") {
            field_enum = getEnumArrayName(field);
          }
          o << field_enum << ", ";
        }
      } else {
        // Default case- no syntax string.
        ForRange(instr.sizeFields(),i) {
          const InstrField *field = instr.getField(i)._field;
          o << getEnumArrayName(field) << ", ";
        }
      }
      o << " (struct enum_fields *) -1,};\n";
      return name;
    }
    
    // This generates the assembler action code, only if regular action exits. Otherwise, the
    // instruction is treated as an assembler directive
    Ptree *genAssemblerCode(ostream &o,const Instr &instr, const Instr::Alias *alias, const StrIntMap &field_order, 
                            const PrefixVarInfos &pfx_vars, const AsmParmHash &asm_parms)
    {
      Ptree* assembler = alias ? alias->_target->assembler() : instr.assembler();

      if (!assembler || (!instr.action() && !alias)) {
        return 0;
      }

      AssemblerWalker w(_core,field_order,pfx_vars,asm_parms);
      Ptree *name = AdlGenSym();
      Ptree *translated = funcBody(w,assembler);
      if (instr.is_alias() && w.uses_operands()) {
        RError("Alias to instruction that uses operands values in the assembler action is not supported");
      }
      else {
        o << "\n\n// Assembler for instruction '" << instr.name() << "'.\n"
          << "static void " << name << "_assembler(struct adl_operand_val *operands ATTRIBUTE_UNUSED,unsigned current_position ATTRIBUTE_UNUSED)"
          << translated << "\n";
      }
      return name;
    }

    // Generate an alias action function.
    Ptree *genAliasActionCode(ostream &o,const Instr &instr,const StrIntMap &field_order,
                              const PrefixVarInfos &pfx_vars,const AsmParmHash &asm_parms)
    {
      if (!instr.is_complex_alias()) {
        return 0;
      }

      Ptree *name = AdlGenSym();
      PostAsmWalker w(_core,pfx_vars,field_order,asm_parms);
      Ptree *translated = funcBody(w,instr.alias_action());

      o << "\n\n// Assembler for instruction '" << instr.name() << "'.\n"
        << "static InstrBundle " << name << "(" << get_func_args(instr.alias_action()) << ")"
        << get_func_body(translated) << "\n";
      
      return name;
    }

    // If the field is a prefix field it return index of appropriate 
    // prefix variable, otherwise return -1.
    int prefix_id(const InstrField &field, const PrefixVarInfos &prefix_vars) {
      if (field.is_prefix()) {
        int id = 0;
        ForEach(prefix_vars,i) {
          const InstrField *ifc = field.interface() ? field.interface() : &field;
          if (ifc->name() == i->first) {
            return id;
          }
          ++id;
        }	
      } 
      return -1;      
    };

    // If the field is a prefix field and it has a"type" it return index of appropriate 
    // "type" prefix variable, otherwise return -1.
    int prefix_type_id(const InstrField &field, const PrefixVarInfos &prefix_vars) {
      if (field.is_prefix() && field.type() == InstrField::tRefField) {
        return prefix_id(*(field.field_type()),prefix_vars);
      }
      return -1;
    }

    void writeInstrFieldContents(ostream &o,int index,const FInfo &finfo,
                                 const Instr &instr,const InstrField &field,const PrefixVarInfos &prefix_vars)
    {
      // The index is always -1 for pseudo fields.
      //index = (field.is_pseudo()) ? -1 : index;

      o << dec << "{" << index << ", "
        << finfo._index  << ", " 
        << finfo._flags  << ", " 
        << finfo._lshift << ", "
        << finfo._offset << ", ";

      unsigned bitpos;
      if (field.bits().empty()) {
        bitpos = 0;
      } else {
        bitpos = (_core.is_little_endian()) ? (instr.width() - field.highbit() - 1) : field.lowbit();
      }

      o << bitpos << ", ";

      if (finfo._is_signed) {
        o << "((bfd_int64_t)" << (finfo._lower+1) << "ull-1), "
          << finfo._upper  << "ull, ";
      } else {
        o << std::hex << std::showbase
          << finfo._lower  << "ull, "
          << finfo._upper  << "ull, "
          << std::dec << std::noshowbase;
      }
      o << std::hex << std::showbase 
        << finfo._mask   << "ull, "
        << std::dec << std::noshowbase
        << "0, 0, 0,";
      if (field.has_checks()) {
        o << field.name() <<"_checker,";
      } else {
        o << " 0,";
      }    
      o << " 0,";
      o << prefix_id(field,prefix_vars) << "," << prefix_type_id(field,prefix_vars) << ",0},";
      o << dec << std::noshowbase;
    }

    void writeSimpleInstrField(ostream &o,unsigned int i,FInfos &fmap,const Instr &instr, 
                               StrIntMap &field_indices,const PrefixVarInfos &prefix_vars, 
                               StrIntMap &field_order, const gc_string &width_str, int &numops)
    {
      auto field = instr.getField(i)._field;
      auto fi = fmap.find(i);
      if (fi != fmap.end()) {
        int idx = field_indices[field->name() + width_str];
        writeInstrFieldContents(o,idx,fi->second,instr,*field,prefix_vars);
        gc_string fname = (field->interface() ? field->interface()->name() : field->name());
        field_order.insert(make_pair(fname,numops));
        ++numops;
      }
    }

    void writeGeneralInstrField(ostream &o,unsigned index,const Instr::Field &fp,const InstrField &field,
                                const gc_string &fname,StrIntMap &field_indices,const gc_string &width_str,
                                ModItemVect &helper_names,PVect &validator_names,const PrefixVarInfos &prefix_vars,
                                StrIntMap &field_order,int &numops)
    {
      switch (fp._type) {
      case Instr::Field::Resrvd:
      case Instr::Field::RValue:
        RError("Reserved and RValue fields not supported.");
        break;
      case Instr::Field::Value:
        // Skip b/c this will be handled by the opcode setup code.
        return;
      case Instr::Field::Expr:
        // Expressions cause us to create a helper function which
        // sets up the code.
        o << "{" << field_indices[fname + width_str] << ", -1, 0, 0, 0, 0, 0, 0, 0, " 
          << helper_names[index]._name << "_modifier, " << helper_names[index]._name << "_mod_indices, 0,";
        if (field.has_checks()) {
          o << field.name() <<"_checker,";
        } else {
          o << " 0,";
        }  
        o << (validator_names[index] ? (gc_string(validator_names[index]->ToString()) + "_validator, "):"0, ");
        o << prefix_id(field,prefix_vars) << "," << prefix_type_id(field,prefix_vars) << ",0},";
        field_order.insert(make_pair(fp._name,numops));
        ++numops;
        break;        
      case Instr::Field::Ref: {

      }
      case Instr::Field::Alias:
        break;
      }
    }

    void writeIndexArray(ostream &o,const ModItem &mi)
    {
      o << "static int " << mi._name << "_mod_indices[] = { ";
      ForEach(mi._indices,i) {
        o << *i << ", ";
      }
      o << " -1 };\n";
    }
    
    // Write field entries for an instruction.  We use the order specified by
    // a syntax line, if it exists, otherwise we list them in the order that they
    // appear within the instruction.
    // Returns number of operands for the instruction.
    int writeInstrFields(ostream &o,FInfos &fmap,Ptree *name,const Instr &instr, const Instr::Alias *alias, 
                         const gc_string &width_str, StrIntMap &field_indices, 
                         const PrefixVarInfos &prefix_vars, StrIntMap &field_order, int &num_terms,
                         int &num_pfx_ops,int &num_prm_ops,int &num_asm_ops)

    {
      ModItemVect helper_names;
      PVect validator_names;

      const Instr::Syntax *syntax = instr.syntax();
      
      num_asm_ops = 0;
      
      // If we have any shorthand expressions, create helper functions.
      if (alias) {
        ForRange(alias->_fields.size(),i) {
          const Instr::Field &fp = alias->_fields[i];
          if (fp._type == Instr::Field::Expr) {
            // We have an expression.  We have to create the helper function
            // and then store its name so that it can be instantiated in the
            // array entry.
            validator_names.push_back(0);

            // Figure out what fields are being used, then return the result
            // by evaluating the expression.
            ShExprWalker walker(instr,*this);
            Ptree *expr2 = walker.Translate(fp._expr);

            // Record the name of the modifier function and what indices are used by the modifier.
            Ptree *name = AdlGenSym();
            helper_names.push_back(ModItem(name,walker.indices()));            
            
            o << "\nstatic bfd_uint64_t " << name << "_modifier (const expressionS *operands, unsigned " << CiaVar << " ATTRIBUTE_UNUSED) { return " << expr2 << "; }\n\n";

            writeIndexArray(o,helper_names.back());
          } else {
            helper_names.push_back(ModItem());
            validator_names.push_back(0);
          }
        }
      } else if (syntax) {
        // Ignore syntax if we're processing an alias.
        // If we have syntax check for possible expressions in it.
        ForRange(syntax->_items.size(),i) {
          const Instr::Field &fp =  syntax->_items[i].first;
          if (fp._type == Instr::Field::Expr) {
            Ptree *name = AdlGenSym();
            
            SyntaxExprWalker walker(instr);
            o << "\nstatic bfd_uint64_t " << name << "_modifier (const expressionS *operands, unsigned " << CiaVar << " ATTRIBUTE_UNUSED) { return " << walker.Translate(fp._expr) << "; }\n";

            helper_names.push_back(ModItem(name,walker.indices()));

            writeIndexArray(o,helper_names.back());
            
            ValidityWalker  vwalker(instr);
            Ptree   *expr = vwalker.Translate(fp._expr);
            if (vwalker.checked()) { // a validator is required
              validator_names.push_back(name);
              o << "\n static bool " << name << "_validator(const expressionS *operands) {\n"
                <<  expr << ";\n"
                << " return TRUE;\n}\n";
            } else {
              validator_names.push_back(0);
            }
          } else {
            helper_names.push_back(ModItem());
            validator_names.push_back(0);
          }
        }      
      }
      
      int numops = 0;
      num_terms = 0;
      IntVect   dumped;

      ostringstream ss;
      
      if (alias) {
        gc_map<gc_string,gc_string>  fields_impls;
	
        // Construct a mapping between fields' interfaces occuring in alias' list
        // and their implmentation in this instruction. Extend to nested fields
        ForEach(instr.getFields(),i) {
          const InstrField *f = i->_field;
          gc_string  f_name = f->name();
          gc_string  ifc_name = (f->interface() ? f->interface()->name() : f_name);
          fields_impls.insert(make_pair(ifc_name,f_name));
          ForEach(f->instrfields(),k) {
            gc_string nf_name   = f_name + "." + k->first;
            gc_string nifc_name = ifc_name + "." + k->first;
            fields_impls.insert(make_pair(nifc_name,nf_name));
          }
        }

        ForRange(alias->_fields.size(),i) {
          const Instr::Field &fp = alias->_fields[i];
          const InstrField &field = *(fp._field);

          if (!field.is_assembler()) {
            if (fp._type == Instr::Field::Ref) {
              auto fi = fmap.find(fp._value);
              if (fi != fmap.end()) {
                int idx = 0;
                if (field.encap_field()) {
                  gc_string impl_name = fields_impls[fp._sname];
                  idx = field_indices[impl_name + width_str];
                  // Ensure for nested fields that the encapsulating field is dumped only once
                  if (find(dumped.begin(),dumped.end(),idx) != dumped.end()) { 
                    continue;
                    ++num_terms;
                  }
                  dumped.push_back(idx);
                } else {
                  idx = field_indices[field.name() + width_str];
                }
                writeInstrFieldContents(ss,idx,fi->second,instr,field,prefix_vars);
                ++numops;
              }
            } else {
              writeGeneralInstrField(ss,i,fp,field,field.name(),field_indices,
                                     width_str,helper_names,validator_names,
                                     prefix_vars,field_order,numops);
            }
          }
        }

        // Now handle assembler fields.
        int op_index = numops;
        int index = 0;
        ForRange(alias->_fields.size(),i) {
          const Instr::Field &fp = alias->_fields[i];
          const InstrField &field = *(fp._field);

          if (field.is_assembler()) {
            writeSimpleInstrField(ss,index,fmap,instr,field_indices,prefix_vars,field_order,
                                  width_str,op_index);
            ++num_asm_ops;
            ++index;
          }
        }
        
      } else if (syntax) {
        string type;
        ForRange(syntax->_items.size(),i) {
          auto fp = syntax->_items[i].first;
          const InstrField *field = syntax->_items[i].first._field;
          if (field->is_alias()) {
            field = field->get_alias()->_target;
          }
          const InstrField* interface = 0;
          if (field->is_pseudo()) {
            ForEach(instr.getFields(),f) {
              if (f->_field) {
                auto ifc = f->_field->interface();
                if (ifc && ifc == field) {
                  interface = f->_field;
                  break;
                }
              }
            }
            // If we're not a shorthand, then we want to skip pseudo fields
            // unless they're interfaces.  This is because, at this point,
            // psueod fields are just found in syntax strings in order to be
            // used by expressions.
            if (!instr.shorthand() && !interface) {
              continue;
            }
          }
          bool expr_flag = false;
          for (unsigned ii = i+1; (ii != syntax->_items.size()) ; ++ii) {
            auto ffp = syntax->_items[ii].first;
            auto ffield = syntax->_items[ii].first._field;	    
            // if the current field will be handled by an expression in the future
            if (ffp._type == Instr::Field::Expr && ffield->name() == field->name()) {
              expr_flag = true;
              break;
            }	    
          }
          if (expr_flag) {
            continue;
          }
          uint32_t ii = instr.getField(interface ? interface->name() : field->name());

          gc_string fname = interface ? interface->name() : field->name();
          auto fi = fmap.find(ii);

          switch (fp._type) {
          case Instr::Field::Ref:
            if (fi != fmap.end()) {
              auto fsyntax = field->syntax();
              if (fsyntax) {
                // This term will be expanded into _items.size() terms.
                num_terms += fsyntax->_items.size() - 1;
              }

              int idx = field_indices[fname + width_str];
              writeInstrFieldContents(ss,idx,fi->second,instr,((interface) ? *interface : *field),prefix_vars);
              field_order.insert(make_pair(fp._name,numops));
              ++numops;
            }
            break;
          default:
            writeGeneralInstrField(ss,i,fp,*field,fname,field_indices,width_str,
                                 helper_names,validator_names,prefix_vars,
                                 field_order,numops);
          }
        }

        // Add assembler fields
        ForRange(instr.sizeFields(),i) {
          auto field = instr.getField(i)._field;
          if (field->is_assembler()) {
            writeSimpleInstrField(ss,i,fmap,instr,field_indices,prefix_vars,field_order,
                                  width_str,numops);
            ++num_asm_ops;
          }
        }
      } else {
        // Default case: List fields in the order in which they appear.
        ForRange(instr.sizeFields(),i) {
          auto field = instr.getField(i)._field;
          if (!field->is_assembler()) {
            writeSimpleInstrField(ss,i,fmap,instr,field_indices,prefix_vars,
                                  field_order,width_str,numops);
            ++num_asm_ops;
          }
        }
        num_terms += numops;

        ForRange(instr.sizeFields(),i) {
          auto field = instr.getField(i)._field;
          if (field->is_assembler()) {
            writeSimpleInstrField(ss,i,fmap,instr,field_indices,prefix_vars,
                                  field_order,width_str,numops);
            ++num_asm_ops;
          }
        }
      }

      if (numops != 0) {
        o << "struct adl_operand " << name << "_operands[] = { " << ss.str() << " };\n";
      }
      
      num_terms += numops;

      // Categorize the operand based upon where it falls in the list,
      // i.e. if it's a prefix op (before instruction name), perm op (any
      // order- only allowed directly after instruction name), or a normal
      // fixed field.
      if (instr.syntax()) {
        const InstrBase::Syntax &syntax = *(instr.syntax());
        num_pfx_ops = syntax.pfx_end();
        num_prm_ops = syntax.perm_end() - syntax.pfx_end();
      } else {
        num_pfx_ops = 0;
        num_prm_ops = 0;
      }

      return numops;
    }


    struct InstrInfo {
      Ptree       *_sym;
      const Instr *_instr;
      const Instr::Alias *_alias;
      int          _numops;
      int 	       _numterms;
      int          _numenumfields;
      int          _num_pfx_ops;
      int          _num_prm_ops;
      int          _num_asm_ops;
      Ptree       *_array_name;
      Ptree       *_sfield_array;
      int          _num_sfields;
      gc_string    _re;
      Ptree       *_enums_name;
      FInfos       _fmap;
      unsigned     _n_fixed;
      Ptree       *_assembler;
      Ptree       *_alias_action;
      Ptree       *_instr_attrs;
      UIntVect     _blks;
      Ptree       *_pfx_ctr;
      Ptree       *_pfx_ctr_mod;
      Ptree       *_argchecker;
      Ptree       *_rules;
      Ptree	      *_dests;
      int	         _longest;

      InstrInfo (const Instr *instr,const Instr::Alias *alias,int n,int nt, int npfx, int nprm, int nasms,Ptree *a,Ptree *sa,int n_sa,const gc_string &r,
                 Ptree *e,const FInfos &fmap, unsigned n_fixed, Ptree *assembler, Ptree *alias_action,
                 Ptree *instr_attrs, const UIntVect &blks, Ptree *pfx_ctr, Ptree *pfx_ctr_mod, Ptree *argchecker, Ptree *rules,Ptree *dests) : 
        _sym(0),
        _instr(instr), 
        _alias(alias),
        _numops(n), 
        _numterms(nt),
        _numenumfields(instr->sizeFieldsWhose(std::ptr_fun(fieldIsEnumerated))), 
        _num_pfx_ops(npfx),
        _num_prm_ops(nprm),
        _num_asm_ops(nasms),
        _array_name(a), 
        _sfield_array(sa),
        _num_sfields(n_sa),
        _re(r), 
        _enums_name(e),
        _fmap(fmap),
        _n_fixed(n_fixed),
        _assembler(assembler),
        _alias_action(alias_action),
        _instr_attrs(instr_attrs),
        _blks(blks),
        _pfx_ctr(pfx_ctr),
        _pfx_ctr_mod(pfx_ctr_mod),
        _argchecker(argchecker),
        _rules(rules),
        _dests(dests),
        _longest(-1)
      {};
    };

    void writeInstrInfo(ostream &o,const InstrInfo &info,uint64_t mask)
    {
      const Instr &instr = *(info._instr);

      // Size of instruction in bytes.
      int wb = 0;

      // Create the opcode mask.
      RNumber opc;
      gc_ostringstream os;

      if (!instr.shorthand()) {
        wb = max(1u,instr.width()/8);        // Minimum of 1 byte even if the
                                            // instruction is just a few bits.
        opc = instr.getOpcode(_core.instr_width(),info._alias,false);

        // If reserved bits aren't 0, then get and set the appropriate mask.
        if (instr.reserved_bits_value()) {
          RNumber rm = instr.getReservedMask(_core.instr_width(),info._alias);
          opc = opc | rm;
        }
        
        unsigned prefix_bits = (_core.prefix_bits() && !instr.is_prefix() ? _core.prefix_bits() : 0);
        if (prefix_bits){
          if (_core.is_little_endian()) {
            opc <<= prefix_bits;
          } else {
            opc >>= prefix_bits;
          }
        } 

        // Left justify the result for little-endian.  Then we have to swap the words.  Ugh.  FIXME!!
        if (instr.is_little_endian()) {
          if ((instr.width() % 32)) {
            unsigned shift = 32 - (instr.width() % 32);
            opc <<= shift;
          }
          opc = swap_words(opc);
        }

        os << "{ ";
        while (opc != 0) {
          os << "0x" << hex << opc.getUIntField(0,31) << ",";
          opc <<= 32;
        }
        
        os << "},";
      } else {
        os << "{ 0 },";
      }

      int p_end   = 0;
      if (instr.syntax()) {
        p_end   = instr.syntax()->perm_end();
      }

      // Write the entry. 
      o << "{ \"" << instr.name() << "\", " << instr.assemble() << ", " << wb << ", " 
        << instr.width() << ", " << _core.instr_width() << ", "  // Add instruction width, and max_width to the adl_opcode structure
        <<" 0x" << hex << mask
        << ", " << os.str() <<  dec << info._enums_name << ", \"" << info._re << "\", 0, " 
        << info._numops << ", "  << info._numterms << ", " << info._num_pfx_ops << ", "
        << info._num_prm_ops << ", " << info._num_asm_ops << ", " << info._array_name << "," << info._sfield_array << "," << info._num_sfields << ","
        << p_end << ", ";
      if (info._alias_action) {
        o << info._alias_action << ",";
      } else {
        o << "0,";
      }
      if (info._assembler) {
        o << info._assembler << "_assembler,";
      } else {
        o << "0,";
      }
      if (info._instr_attrs) {
        o << "&" << info._instr_attrs << ",";
      } else {
        o << "0,";
      }
      o << dec << info._blks.size() << ",{";
      bool first = true;
      ForEach(info._blks,i) {
        list_sep(o,first);
        o << *i;
      }
      o << "}, " << info._pfx_ctr << "," << info._pfx_ctr_mod << "," << info._argchecker << "," 
        << info._rules << "," << info._dests << "," << info._longest	<< ", },\n";
    }

    typedef gc_vector<InstrInfo> InstrInfoList;
   
    // We sort the instruction info objects by looking at their operands.
    // a fewer number of operands is considered 'less' than a larger number.
    // For equal numbers, immediates/enums come before registers and we sort by
    // shift size for immediates.  The idea is that we want to match an immediate
    // first, since the user may write 'op 2'.  This should match an immediate
    // operand, if one exists, else it will match a register op, since in PowerPC,
    // it's valid to write either '2' or 'r2'.
    // New addidtion: Now instructions are sorted according to width first, to allow
    // the best code density.
    struct SortInstrInfo {
      bool operator()(const InstrInfo &x,const InstrInfo &y) {

        // First off, we sort by explicit rank.
        if (x._instr->asm_rank() < y._instr->asm_rank()) {
          return true;
        } else if (x._instr->asm_rank() > y._instr->asm_rank()) {
          return false;
        }
        
        // Instructions that use enumerated fields take precedence,
        // because their regular expressions will be more specific.
        // In addition, the more enumerated fields used, the higher
        // the precedence.
        if (x._numenumfields > y._numenumfields) {	  
          return true;
        } else if (x._numenumfields < y._numenumfields) {	  
          return false;
        }
	
        unsigned xfmt = getFmtSize(*(x._instr));
        unsigned yfmt = getFmtSize(*(y._instr));

        if (xfmt > yfmt) {
          // First check instruction with larger format.
          return true;
        } else if (xfmt < yfmt) {
          return false;
        }


        int xw = x._instr->width();
        int yw = y._instr->width();
        bool smaller = (xw < yw);
    
        // Next, sort by specificity, in order to get the expected operand
        // matching behavior, e.g. foo r1,1 matches against a register-immediate
        // form, if it exists, whereas foo r1,r1 matches a register-register
        // form.

        int sx = x._fmap.size();
        int sy = y._fmap.size();
        if (sx < sy) {
          return true;
        } else if (sx > sy) {
          return false;
        } else {
          // For equal # of operands, consider on an operand-by-operand basis.
          if (x._n_fixed == y._n_fixed) {	  
            auto xb = x._fmap.begin();
            auto xe = x._fmap.end();
            auto yb = y._fmap.begin();
            auto ye = y._fmap.end();
            for ( ; xb != xe; ++xb, ++yb) {
              if ( xb->second < yb->second ) {
                return true;
              } else if ( xb->second != yb->second ) {
                return false;
              } 
            }
            // Otherwise, pick the smaller instruction for better code density.
            return smaller;
          }  
          else {
            return x._n_fixed > y._n_fixed;	  
          }  
        }
      };
    };


    struct  SortFInfo {
      bool operator()(const FInfo *x,const FInfo *y) {
        if (!x->_ifc_name.empty() && !y->_ifc_name.empty()) {
          return x->_ifc_name < y->_ifc_name;
        } else {
          return x->_name < y->_name;
        }
      }
    };

    // Stores instruction fields for comparison and sorting purposes.  Assembler
    // fields are ignored at this point because they're "out-of-band", i.e. they
    // don't play into the direct use of instructions, but are instead handled
    // by separate setup functions in the assembler.
    struct FInfoSet : public gc_set<const FInfo*, SortFInfo> {
    public:
      typedef gc_set<const FInfo*, SortFInfo> Base;
      FInfoSet(const FInfos &finfos)  {
        ForEach(finfos,iter) {
          const FInfo *f = &(iter->second);
          if (f->_is_asm) continue;
          if (f->_is_immed) {
            _immeds.insert(f);
          } else {
            insert(f);
          }
        }
      }

      bool all_empty() const { return Base::empty() && _immeds.empty(); };

      gc_set<const FInfo*,SortFInfo>  _immeds;
    };

    // Figures out if two instructions have equivalent immediate info.  Also
    // returns false if there is a mismatch in instruction fields, e.g. fields
    // point to different register files.
    bool equivImmedInfo(const InstrInfo *x,const InstrInfo *y)
    {
      // First, they must have the same regular expression.  This prevents us
      // from choosing two different types of instructions which have similar
      // fields, e.g.  'foo (sp+%f),%f' won't be considered equivalent to 'foo
      // %f,%f' because we assume that a difference in syntax implies a
      // difference in functionality, even though the two may have similar types
      // of fields.
      if (x->_re != y->_re) return false;

      FInfoSet xfs(x->_fmap);
      FInfoSet yfs(y->_fmap);

      if (xfs.all_empty()) {
        return yfs.all_empty();
      }

      if (xfs.size() != yfs.size()) {
        return false;
      }

      if (xfs._immeds.size() != yfs._immeds.size()) {
        return false;
      }

      // For non immediates, must have the same interface and, if a
      // register-file is referenced, must have the same type.
      auto i = xfs.begin();
      ForEach(yfs,j) {
        const FInfo &x = **i;
        const FInfo &y = **j;
        if (x._ifc_name != y._ifc_name ||
            x._regfile != y._regfile) return false;
        ++i;
      }
      auto ii = xfs._immeds.begin();
      ForEach(yfs._immeds,jj) {
        const FInfo &x = **ii;
        const FInfo &y = **jj;
        if (x._lshift != y._lshift ||
            x._offset != y._offset ||
            x._is_modified != y._is_modified ||
						// If one has unsigned_upper_bound, then it can be treated as both signed and unsigned, so no comparison in this case
            ((x._is_signed != y._is_signed) && (!x._is_unsigned_upper_bound) && (!y._is_unsigned_upper_bound)) || 
            x._is_enum != y._is_enum) return false; 
        ++ii;
      }

      return true;
    }
    
    struct SortImmedInfo {
      bool operator()(const InstrInfo *x,const InstrInfo *y) {
        FInfoSet xfs(x->_fmap);
        FInfoSet yfs(y->_fmap);
        if (yfs._immeds.empty()) return true;
        if (xfs._immeds.empty()) return false;
        if (xfs.size() < yfs.size()) return true;
        if (xfs.size() > yfs.size()) return false;
	
        auto i = xfs.begin();
        ForEach(yfs,j) {
          if ((*i)->_ifc_name <  (*j)->_ifc_name) return true;
          if ((*i)->_ifc_name >  (*j)->_ifc_name) return false;
          ++i;
        }

        // Instructions have the same non-immed fields, sort by immed size
        if (xfs._immeds.size() < yfs._immeds.size()) return true;
        if (xfs._immeds.size() > yfs._immeds.size()) return false;

        auto k = xfs._immeds.begin();
        ForEach(yfs._immeds,j) {
          if ((*k)->_width < (*j)->_width) return true;
          ++k;
        }
        return false;
      }
    };


    void markModifiedField(InstrInfo *instr_info,const Instr::Field &fp) 
    {
      if (fp._type != Instr::Field::Expr ||  fp._field == NULL) {
        return;
      }
      const InstrField *fld = (fp._field->interface() ? fp._field->interface() : fp._field);
      const gc_string &ifc_name = fld->name();
      ForEach(instr_info->_fmap,i) {
        FInfo &fi = i->second;
        if (fi._ifc_name == ifc_name) {
          fi._is_modified = true;
        }
      }
    }

    void genBigMemInfo(InstrInfoList &instr_infos)
    {
      // Stores all immediate instrutions, sorted by size of immediate.  Some
      // instructions will be equivalent, but we want to store everything
      // because we need to set their _longest field.
      gc_multiset<InstrInfo*,SortImmedInfo>  immed_instr;

      int cnt = -1;
      ForEach(instr_infos,i) {
        ++cnt;
        const Instr *instr = i->_instr;
        ForEach(instr->getFields(),f) {
          const InstrField *fld = f->_field;
          if (fld && fld->type() == InstrField::tImm && 
              !fld->is_assembler() && !fld->is_reserved() && !fld->is_parm()) {
            i->_longest = cnt;
            immed_instr.insert(&(*i));
            break;
          }
        }
      }

      // Set modified field info for immediate fields.
      ForEach(immed_instr,i) {
        const Instr *instr = (*i)->_instr;
        if (const Instr::Syntax *syntax = instr->syntax()) {
          ForRange(syntax->_items.size(),k) {
            markModifiedField(*i,syntax->_items[k].first);
          }
        }
        const Instr::Alias *alias = instr->get_alias();
        if (alias && !instr->is_nested()) {
          ForRange(alias->_fields.size(),k) {
            markModifiedField(*i,alias->_fields[k]);
          }
        }
      }

      // Now we go through and figure out our equivalence classes, i.e. what
      // instructions are equivalent to each other and can have the same
      // "longest" value.  The issue is that equivalent instructions need not be
      // consecutive in immed_instr.  However, they are sorted, so we just need
      // to iterate forward in the set, knowing that any item later in the set
      // is longer than a previous item.
      while (!immed_instr.empty()) {
        // This will store the items considered to be part of a single
        // equivalence class.  Start with the first item and put it in the list.
        // We can use a list b/c our input is an already-sorted set.
        gc_list<InstrInfo*> cur_class;
        cur_class.push_back(*(immed_instr.begin()));
        immed_instr.erase(immed_instr.begin());
        // cout << "Starting class:  " << (*cur_class.begin())->_instr->name() << "\n";
        // Now iterate over the remaining items, putting anything equivalent
        // into the list.  Since the set is sorted, we know that we're
        // increasing in terms of "longest".
        for ( auto iter = immed_instr.begin(); iter != immed_instr.end(); ) {
          // cout << "  Checking:  " << (*iter)->_instr->name() << "\n";
          if ( equivImmedInfo(*iter,cur_class.back()) ) {
            // Part of the same equivalence class.
            auto tmp = iter++;
            cur_class.push_back(*tmp);
            immed_instr.erase(tmp);
            // cout << "added to class " << cur_class.front()->_instr->name() << '\n';
          } else {
            // Not part of the equivalence class, so skip it.
            ++iter;
          }
        }
        // Now we have an equivalence class in cur_class.  So mark all items'
        // _longest with the last item in the list, which will be the "longest".
        int longest = cur_class.back()->_longest;
        ForEach(cur_class,iter) {
          (*iter)->_longest = longest;
        }
      }
    }

    // Generate a function to map field IDs to prefix counters and to return the
    // prefix counter value.
    Ptree *genPfxCtrFunc(ostream &o,const Instr &instr)
    {
      const InstrField *f = 0;
      const InstrField::SplitInfo *si = 0;
      // If neither this instruction nor its parent-type (if it has one) has any
      // prefix counters, then don't bother creating a function.
      if (instr.prefix_counters().empty()) {
        if (!instr.get_type() || instr.get_type()->prefix_counters().empty()) {
          return Zero;
        }
      }

      Ptree *fname = Ptree::Make("%s_prefix_counter",makeFriendlyName(instr.name()).c_str());
      o << "int " << fname << "(int id,int index)\n"
        << "{\n"
        << "switch (id) {\n";
      ForEach(instr.getFields(),i) {
        f = i->_field;

        // The user will specify this using the interface name, not the expanded
        // name, so if there's an interface, use its name.
        auto fi = (f->interface()) ? f->interface() : f;

        // First, try querying the instruction using this field.
        gc_string pc = getPrefixIndex(*fi,instr,gc_string());
        if (pc.empty()) {
          // If that didn't work, then try again using the split-info field.
          // This will always refer to the parent instruction type, which will
          // actually have this field.
          auto tinstr = instr.get_type();
          if ( (si = f->splitInfo()) && tinstr ) {
            if (tinstr->getField(si->_name) >= 0) {
              if (auto pfx_ctr = tinstr->get_prefix_counter(si->_name)) {
                pc = *pfx_ctr;
              }
            }
          }
        }
        if (!pc.empty()) {
          o << "case " << f->id() << ":\n"
            << "return " << pc << ";\n";
        }
      }
      o << "default:\n"
        << "return index;\n"
        << "}\n"
        << "}\n\n";
      return fname;
    }

    // Generate a function to modify any relevant prefix counters for the function.
    Ptree *genPfxCtrModFunc(ostream &o,const Instr &instr)
    {
      if (!instr.prefix_counter_mods().empty()) {
        Ptree *fname = Ptree::Make("%s_prefix_counter_mods",makeFriendlyName(instr.name()).c_str());
        o << "void " << fname << "()\n"
          << "{\n";          
        ForEach(instr.prefix_counter_mods(),i) {
          o << i->first << ( (i->second) ? "++" : "--") << ";\n";
        }
        o << "}\n\n";
        return fname;
      } else {
        return Zero;
      }
    }

    unsigned writeInstrInfos(ostream &o,Ptree *name,const InstrInfoList &infos,uint64_t mask)
    {
      o << "static struct adl_opcode " << name << "[] = {\n";
      unsigned count = 0;
      ForEach(infos,i) {
        const InstrInfo &info = *i;
        o << "// " << info._instr->name() << "    (" << count << ")\n";
        writeInstrInfo(o,info,mask);
        ++count;
      }
      o << "};\n\n";
      return count;
    }

    pair<Ptree *,unsigned> writeShorthands(ostream &o,const InstrInfoList &shorthands,uint64_t mask)
    {
      if (shorthands.empty()) {
        return make_pair(Zero,0);
      }
      Ptree *nm = AdlGenSym();
      unsigned count = writeInstrInfos(o,nm,shorthands,mask);
      return make_pair(nm,count);
    }

    // This only returns true if we're dealing with a shorthand or an alias
    // which maps to a real instruction.  If this maps to a function consisting
    // of only pseudo fields, then we just treat this as a real instruction.
    //
    // Right now, all we do is check for nesting, but that's not really good
    // enough- we could target a pure alias w/o nesting, theoretically.
    bool is_real_alias(const Instr &instr)
    {
      if (instr.is_alias()) {
        if (instr.is_nested()) {
          return false;
        } else if (instr.shorthand()) {
          // Shorthands are always real aliases.
          return true;
        } else {
          const Instr::Alias &alias = *instr.get_alias();
          ForEach(alias._fields,i) {
            if ( !i->_field->is_pseudo() ) {
              return true;
            }
          }
          return false;
        }
      } else {
        return false;
      }
    }
    
    unsigned genInstrTable(ostream &o,const gc_string &tname,const Instrs &instrs,
                           StrIntMap &field_indices, StrIntMap &blk_indices, const AsmRuleHash &asm_rules,
                           const PrefixVarInfos &prefix_vars, const AsmParmHash &asm_parms,const IntSet &instr_attrs,bool dest)
    {
      // This maintains a set of all instruction names that we come across.
      _instr_names.clear();

      uint64_t mask = (uint64_t)-1;

      unsigned maxfields = 0;

      // Next, generate arrays for storing operands.
      gc_map<gc_string,InstrInfoList > instr_info;
      ForEach(instrs,iter) {
        const Instr &instr = **iter;
        
        gc_string nm = getInstrName(instr);
        _instr_names.insert(nm);

        // Write instruction field information.  For a shorthand, write entries
        // for each target mapping.
        FInfos fmap;
        // Gather information from the syntax string, or else use defaults.
        getInstrFieldData(fmap,instr);
        gc_string width_str("");
        if (_core.is_little_endian()) {
          width_str += "_width_" + uitogs(instr.width());
        }
        
        InstrInfoList shorthands;
        
        o << "\n// Instruction " << instr.name() << "\n";
        auto attrs = genInstrAttrs(o,instr,instr_attrs);

        uint64_t sa_mask = (uint64_t)-1;
        if (is_real_alias(instr)) {
          ForEach(instr.get_aliases(),a_iter) {
            StrIntMap  field_order;
            int num_terms = 0, num_pfx_ops = 0, num_prm_ops = 0, num_asm_ops = 0;
            const Instr::Alias *alias = *a_iter;
            Ptree *array_name = AdlGenSym();
            
            o << "\n// Shorthand:  " << instr.name() << " -> " << alias->_target->name() << ";\n";
            unsigned numops = writeInstrFields(o,fmap,array_name,instr,alias,width_str,field_indices,prefix_vars,field_order,num_terms,num_pfx_ops,num_prm_ops,num_asm_ops);
            maxfields = max(maxfields,numops);

            array_name = (numops == 0) ? Ptree::Make("0") : Ptree::Make("%p_operands",array_name);
            
            // Add on an entry to the shorthand map.  This is a little strange
            // here: If it's a shorthand, then we pass on the alias target plus
            // the alias information, since the shorthand has no encoding of its
            // own.  However, if it's an alias with encoding, then we can't do
            // that, since we have our own unique encoding.
            const Instr *a_instr = 0;
            if (!instr.shorthand()) {
              a_instr = &instr;
              alias = 0;
            } else {
              a_instr = alias->_target;
            }
            shorthands.push_back(InstrInfo(a_instr,alias,numops,num_terms,num_pfx_ops,num_prm_ops,num_asm_ops,array_name,Zero,0,"",Zero,fmap,
                                           instr.getNFixed(),
                                           genAssemblerCode(o,**iter,alias,field_order,prefix_vars,asm_parms),0,
                                           attrs, // Note:  Used parent's attributes here.
                                           UIntVect(),
                                           Zero,
                                           Zero,
                                           Zero,
                                           Zero,
                                           Zero));
            sa_mask = min(sa_mask,genmask(a_instr->width()/8));
          }
        }
        o << "\n// Instruction " << instr.name() << "\n";
        StrIntMap  field_order;
        int num_terms = 0, num_pfx_ops = 0, num_prm_ops = 0, num_asm_ops = 0;
        // Note: For shorthands, don't use a width string because we don't want
        // to specialize field names at this point.
        Ptree *array_name = Ptree::Make("%p_operands",AdlGenSym());
        unsigned numops = writeInstrFields(o,fmap,array_name,instr,0,(instr.shorthand() ? gc_string() : width_str),field_indices,prefix_vars,field_order,num_terms,num_pfx_ops,num_prm_ops,num_asm_ops);
        maxfields = max(maxfields,numops);
        array_name = (numops == 0) ? Ptree::Make("0") : Ptree::Make("%p_operands",array_name);
        
        // Generate the regular expression used to parse the operands.
        auto sa_p = writeShorthands(o,shorthands,sa_mask);
        auto ip = instr_info.insert(make_pair(nm,InstrInfoList()));
        ip.first->second.push_back(InstrInfo(&instr,0,numops,num_terms,num_pfx_ops,num_prm_ops,num_asm_ops,array_name,sa_p.first,sa_p.second,
                                             genRegEx(**iter),
                                             genEnumArray(o,**iter),
                                             fmap,
                                             instr.getNFixed(),
                                             genAssemblerCode(o,**iter,0,field_order,prefix_vars,asm_parms),
                                             genAliasActionCode(o,**iter,field_order,prefix_vars,asm_parms),
                                             attrs,
                                             getInstrBlks(instr,blk_indices),
                                             genPfxCtrFunc(o,instr),
                                             genPfxCtrModFunc(o,instr),
                                             genInstrArgChecker(o,instr),
                                             genInstrRules(o,instr),
                                             genInstrDests(o,instr,field_indices,dest)));
        // Calculate and store the mask.  The mask for this table will be the minimum of all masks.
        mask = min(mask,genmask(instr.width()/8)); //
        // mask = min(mask,genmask((instr.width()-1)/8+1)); //       
      }
      o << "\n";

      // List out all opcode entries.
      ForEach(instr_info,i) {
        const gc_string &name = i->first;
        sort(i->second.begin(),i->second.end(),SortInstrInfo());
        genBigMemInfo(i->second);
        Ptree *sym = i->second.front()._sym = AdlGenSym();
        o << "// Instructions named '" << name << "'.\n";
        writeInstrInfos(o,sym,i->second,mask);
      }

      // Now generate the instruction table.
      o << "// Instruction table.\n"
        << "static struct adl_instr " << tname << "_instructions [] = {\n";
      int numinsts = 0;
      ForEach(instr_info,i) {
        const gc_string    name = lowercase(i->first);
        o << "{ \"" << name << "\", " << i->second.size() << ", " << i->second.front()._sym << " },\n";
        ++numinsts;
      }
      if (numinsts == 0) {
        o << " { \"\",0,0 },\n";
      }
      o << "};\n\n"
        << "static const int num_" << tname << "_instructions = " << numinsts << ";\n\n";      

      return maxfields;
    }

    // Generated the static variables
    void genAsmParms(ostream &o,const AsmParmHash &parms)
    {
      ForEach(parms,i) {
        AsmParm &parm = *(i->second);
        if (parm.type() == AsmParm::Label) {
          o << "static symbolS *" <<  parm.name() << " = 0;\n";
        } else { // integer
          gc_string is_const = (parm.is_const() ? "const " : " ");
          o << "static " << is_const << "bfd_uint64_t " << parm.name() << " = " << parm.value() << ";\n";
        }
      }
      o << "\n\n";
    }

    int getMaxTrgs() 
    {
      size_t max_trgs = 1;
      gc_string prefix_itable = (_core.asm_config().asm_instrtables().empty() ?
                              "" :  PrefixInstrTable);

      ForEach(_core.instrtables(),it) {
        const Instrs &instrs = it->second._instrs;
        gc_string name = Data::get_attr_name(it->first);
        if (name != prefix_itable) {
          ForEach(instrs,i) {
            const UsageInfo &instr_ui = (*i)->usageinfo();
            max_trgs = max(max_trgs,instr_ui._trg_regs.size());
          }
        }
      }

      int max_entry_trg = 1;
      ForEach(_core.regfiles(),i) {
        if  (i->second->sparse()) {
          ForEach(i->second->entries(),e) {
            const RegEntry &re = *(e->second);
            if (re.write()) {
              const UsageInfo &ui = re.write()->usageinfo();
              const UsedRegs  &trg_regs = ui._trg_regs;
              max_entry_trg = max(max_entry_trg,(int)trg_regs.size());
            }
          }
        }
      }

      return max_trgs*max_entry_trg;
    }


    void genRegIdIdx(ostream &o, const UsedReg &ur) 
    {
      o << "*res_regid  = " << ur.regb()->id() << ";\n"
        << "*res_regidx = " << (ur._begin ? max(0,get_int(ur._begin,"reg index",ur.regb()->env())) : 0) << ";\n";
    }
 

    void genResolvedRegs(ostream &o) 
    {
      o << "// resolve sparse registers \n"
        << "static bool resolve_regid(int *res_regid ATTRIBUTE_UNUSED, int *res_regidx ATTRIBUTE_UNUSED,\n"
        << "int regid ATTRIBUTE_UNUSED,int regidx ATTRIBUTE_UNUSED,int entry_idx ATTRIBUTE_UNUSED)\n"
        << "{\n"
        << "bool multiple_regs = false;\n"
        << "switch(regid)\n"
        << "{\n";
      ForEach(_core.regfiles(),i) {
        auto rf = *(i->second);
        if (!rf.sparse()) {
          continue;
        }
        o << "case " << rf.id() << ":\n"
          << "switch(regidx)\n"
          << "{\n";
        ForEach(rf.entries(),e) {
          const RegEntry &re = *(e->second);
          if (!re.read() && !re.write()) {
            const RegName &rn = re.reg();
            o << "case " << dec << re.id() << ":\n"
              << "*res_regid = " << rn()->id() << ";\n"
              << "*res_regidx = " << max(rn.index(),0) << ";\n"
              << "break;\n";
          } else if (re.write()) {
            const UsageInfo &ui = re.write()->usageinfo();
            const UsedRegs  &trg_regs = ui._trg_regs;
            if (!trg_regs.empty()) {
              o << "case " << dec << re.id() << ":\n";
              if (trg_regs.size() == 1) {
                genRegIdIdx(o,*(trg_regs.begin()));
              } else {
                unsigned entry = 0;
                o << "switch(entry_idx)\n"
                  << "{\n";
                ForEach(trg_regs,t) {
                  auto ur = *t;
                  o << "case " << dec << entry++ << ":\n";
                  genRegIdIdx(o,ur);
                  if (entry < trg_regs.size()) {
                    o << "multiple_regs = TRUE;\n";
                  }
                  o << "break;\n";
                }
                o << "default:\n"
                  << "break;\n"
                  << "}\n";
              }
              o << "break;\n";
            }
          }
        }
        o << "default:\n"
          << "*res_regid = regid;\n"
          << "*res_regidx = regidx;\n"
          << "}\n"
          << "break;\n";
      }
      o << "default:\n"
        << "*res_regid = regid;\n"
        << "*res_regidx = regidx;\n"
        << "}\n" 
        << "return multiple_regs;\n"
        << "}\n\n";
    }

    void genAsmRulesHelpers(ostream &o,const AsmRuleHash &rules,bool &dest,bool msgs_used) 
    {
      dest = false;

      if (msgs_used) {
        o << "static void error(const char *msg,int pos ATTRIBUTE_UNUSED,int current_position ATTRIBUTE_UNUSED) ATTRIBUTE_UNUSED;\n"
          << "static void error(const char *msg,int pos ATTRIBUTE_UNUSED,int current_position ATTRIBUTE_UNUSED)\n"
          << "{\n" 
          << "adl_error(msg,pos,current_position);\n"
          << "}\n\n"
        
          << "static void info(const char *msg,int pos ATTRIBUTE_UNUSED,int current_position ATTRIBUTE_UNUSED) ATTRIBUTE_UNUSED;\n"
          << "static void info(const char *msg,int pos ATTRIBUTE_UNUSED,int current_position ATTRIBUTE_UNUSED)\n"
          << "{\n" 
          << "adl_info(msg,pos,current_position);\n"
          << "}\n\n";
      }

      if (rules.empty()) {
        return;
      }

      ForEach(rules,r) {
        if (r->second->dest()) {
          dest = true;
        }
      }
      if (dest) {
        genResolvedRegs(o);
        o << "// RegWrite data structures and helper functions\n\n"
          << "struct reg_write {\n"
          << "int id;\n"
          << "int start;\n"
          << "int end;\n"
          << "unsigned long long mask[2];\n"
          << "};\n\n"
          << "struct reg_writes {\n"
          << "struct reg_write rwrites[" << getMaxTrgs() <<  "];\n"
          << "int size;\n"
          << "};\n\n"
          << "static void get_dest(struct reg_writes *regwrites ATTRIBUTE_UNUSED,int pos ATTRIBUTE_UNUSED,int current_position ATTRIBUTE_UNUSED)\n" 
          << "{\n"
          << "struct reg_writes *rw = (struct reg_writes *) adl_get_dest(pos,current_position);\n"
          << "if (!rw) {\n"
          << "regwrites->size = 0;\n"
          << "return;\n"
          << "}\n"
          << "memcpy((void*)regwrites,(void*)rw,sizeof(struct reg_writes));\n"
          << "int i;\n"
          << "int ns = regwrites->size;\n"
          << "for(i=0; i < regwrites->size; ++i) {\n"
          << "bool same = (regwrites->rwrites[i].start == regwrites->rwrites[i].end);\n" 
          << "if (regwrites->rwrites[i].start < 0) {\n" 
          << "int field_id = - regwrites->rwrites[i].start - 1;\n"
          << "regwrites->rwrites[i].start = adl_get_field(field_id,0,pos,current_position);\n"
          << "}\n"
          << "if (regwrites->rwrites[i].end < 0) {\n" 
          << "if (same) {\n"
          << "regwrites->rwrites[i].end = regwrites->rwrites[i].start;\n"
          << "} else {\n"
          << "int field_id = - regwrites->rwrites[i].end - 1;\n"
          << "regwrites->rwrites[i].end = adl_get_field(field_id,0,pos,current_position);\n"
          << "}\n"
          << "}\n"
          << "if(regwrites->rwrites[i].id < 0) {\n"
          << "int id = -regwrites->rwrites[i].id;\n" 
          << "int start = regwrites->rwrites[i].start;\n"
          << "int end = regwrites->rwrites[i].end;\n"
          << "if (resolve_regid(&regwrites->rwrites[i].id,&regwrites->rwrites[i].start,id,start,0))\n"
          << "{\n"
          << "if (same) {\n"
          << "regwrites->rwrites[i].end = regwrites->rwrites[i].start;\n"
          << "}\n"
          << "int k=1;\n"
          << "while(resolve_regid(&regwrites->rwrites[ns].id,&regwrites->rwrites[ns].start,id,start,k)){\n" 
          << "regwrites->rwrites[ns].mask[0] = regwrites->rwrites[i].mask[0];\n"
          << "if (same) {\n"
          << "regwrites->rwrites[ns].end = regwrites->rwrites[ns].start;\n"
          << "}\n"
          << "ns++;\n"
          << "k++;\n"
          << "}\n"
          << "regwrites->rwrites[ns].mask[0] = regwrites->rwrites[i].mask[0];\n"
          << "if (same) {\n"
          << "regwrites->rwrites[ns].end = regwrites->rwrites[ns].start;\n"
          << "}\n"
          << "ns++;\n"
          << "}\n"
          << "if(!same) {\n"
          << "if (resolve_regid(&regwrites->rwrites[i].id,&regwrites->rwrites[i].end,id,end,0))\n"
          << "{\n"
          << "int k=1;\n"
          << "while(resolve_regid(&regwrites->rwrites[ns].id,&regwrites->rwrites[ns].end,id,end,k)){\n" 
          << "regwrites->rwrites[ns].mask[0] = regwrites->rwrites[i].mask[0];\n"
          << "ns++;\n"
          << "k++;\n"
          << "}\n"
          << "regwrites->rwrites[ns].mask[0] = regwrites->rwrites[i].mask[0];\n"
          << "ns++;\n"
          << "}\n"
          << "}\n"
          << "}\n"
          << "}\n"
          << "regwrites->size = ns;\n"
          << "}\n\n" 
          << "static void intersect_dest(struct reg_writes *in1 ATTRIBUTE_UNUSED, struct reg_writes *in2 ATTRIBUTE_UNUSED, struct  reg_writes *out ATTRIBUTE_UNUSED)\n"
          << "{\n"
          << "out->size = 0;\n"
          << "int nw1 = in1->size;\n"
          << "int nw2 = in2->size;\n"
          << "if (nw1 == 0 || nw2 == 0) {\n"
          << "return;\n"
          << "};\n"
          << "int i2 = 0;\n"
          << "int i1;\n" 
          << "struct reg_write *rw1 =0;\n"
          << "struct reg_write *rw2 = &(in2->rwrites[i2]);\n"
          << "for(i1 = 0; i1 < nw1; ++i1) {\n" 
          << "rw1 = &(in1->rwrites[i1]);\n"
          << "if (rw1->id < rw2->id) {\n"
          << "continue;\n"
          << "}\n"
          << "while(rw2->id < rw1->id || (rw2->id == rw1->id && rw2->end < rw1->start)) {\n"
          << "if (++i2 > nw2) {\n"
          << "break;\n"
          << "}\n"
          << "rw2 = &(in2->rwrites[i2]);\n"
          << "}\n"
          << "if (rw1->id == rw2->id && (rw2->start <= rw1->end || rw2->end <= rw1->end)) {\n"
          << "out->rwrites[out->size].id    = rw1->id;\n"
          << "out->rwrites[out->size].start = (rw1->start < rw2->start ? rw2->start : rw1->start);\n"
          << "out->rwrites[out->size].end   = (rw1->end   < rw2->end   ? rw1->end   : rw2->start);\n"
          << "out->rwrites[out->size].mask[0] = rw1->mask[0];\n"
          << "out->rwrites[out->size].mask[1] = rw2->mask[0];\n"
          << "out->size++;\n"
          << "if (++i2 > nw2) {\n"
          << "break;\n"
          << "}\n"
          << "rw2 = &(in2->rwrites[i2]);\n"
          << "}\n"
          << "}\n"
          << "}\n\n";
      }
    }

    gc_string getRuleCheckerName(const gc_string &name) 
    {
      return  name + "_rule_checker";
    }
    gc_string getRuleCheckerName(const AsmRule &rule) {
      return  getRuleCheckerName(rule.name());
    }

  
  
    gc_string genAsmRuleCheckers(ostream &o, const AsmRuleHash &asm_rules,const PrefixVarInfos &pfx_vars, 
                              StrIntMap &field_indices, const AsmParmHash &asm_parm)
    {
      StrSet  used_rules;
      if (asm_rules.empty()) {
        return "0";
      }

      ForEach(_core.instrtables(),t) {
        const Instrs &instrs = t->second._instrs;
        ForEach(instrs,i) {
          ForEach((*i)->asmrules(),r) {
            used_rules.insert(*r);
          }
        }
      }
    
      PostAsmWalker w(_core,pfx_vars,field_indices,asm_parm);
      if (!asm_rules.empty()) {
        o << "// Assembler rule checkers.\n\n";
      }
      bool packet_rule = false;
      ForEach(asm_rules,r) {
        AsmRule &rule = *(r->second);

        // We want to convert the argument into a reference for either type.
        StrList rule_arg;

        if (rule.type() == AsmRule::Instr) {
          if (used_rules.find(rule.name()) == used_rules.end()) {
            continue;
          }
          w.set_mode(PostAsmWalker::InstrRuleMode);
          rule_arg.push_back("InstrInfo");
        } else {
          Ptree *arg = get_func_call_arg(rule.action(),0);
          w.set_mode(PostAsmWalker::PacketRuleMode);
          w.set_packet_arg_name(Second(arg));
          packet_rule = true;
          rule_arg.push_back("InstrBundle");
        } 

        o << "// Checker for rule: " <<  rule.name() << "\n"
          << "static void " << getRuleCheckerName(rule) << "(" << referize_args(rule.action(),rule_arg) << ",int current_position ATTRIBUTE_UNUSED)\n"
          << funcBody(w,rule.action()) << "\n";

        w.clear_packet_vars();
      }

      if (packet_rule) {
        o << "// Post packet rule checker\n"
          << "static void  post_packet_rule_checker(InstrBundle &b ATTRIBUTE_UNUSED,int current_position ATTRIBUTE_UNUSED)\n"
          << "{\n";
        ForEach(asm_rules,r) {
          AsmRule &rule = *(r->second);
          if (rule.type() == AsmRule::Packet) {
            o << getRuleCheckerName(rule) << "(b,current_position);\n";
          }
        }
        o << "}\n\n";
      }
      return (packet_rule ? "&(post_packet_rule_checker)" : "0");
    }

    gc_string get_width_sfx(unsigned width)
    {
      return _core.is_little_endian() ? gc_string("_width_") + uitogs(width) : "";
    }

    bool has_width_sfx() const
    {
      return _core.is_little_endian();
    }

    // When to add a no-width field entry.  We only do this if we have
    // width-suffixes and the field isn't an implementation or reserved field.
    bool add_nowidth_field(const InstrField &field)
    {
      return (has_width_sfx() && !field.interface() && !field.is_reserved());
    }

    void writeFieldArrayElement(ostream &o,const gc_string &fname,const InstrField &field,
                                StrIntMap &field_indices,bool is_real_entry,
                                const gc_string &pfx_setter,const gc_string &getter)
    {
      int    reloc_type = -1;
      bool   reloc_extra = false;
      int    reloc_width = -1;
      if (!field.relocs().empty()) {            
        auto reloc = field.relocs().front();
        reloc_type = reloc->value();
        reloc_width = reloc->width();
        reloc_extra = _reloc_extras.count(reloc->name());
      }
      //cout << "Looking up field " << fname << endl;
      int cnt = field_indices.find(fname)->second;
      const InstrField *ifc_field = field.interface() ? field.interface() : &field;
      while (ifc_field->type() == InstrField::tRefField) {
        ifc_field = ifc_field->field_type();
      }
      gc_string ifc_name = (ifc_field == &field ? fname : ifc_field->name());
      int ifc_id = field_indices.find(ifc_name)->second;
      int assm = field.is_assembler() ? 1 : 0;
      gc_string setter, clearer;
      if (is_real_entry) {
        setter  = (field.is_pseudo() || !field.is_used()) ? "0" : ("set_" + fname);
        clearer = (field.is_pseudo() || !field.is_used()) ? "0" : ("clear_" + fname);
      } else {
        setter = "0";
        clearer = "0";
      }
      o << "{\"" << fname << "\"," << dec
        << field.id() << ","
        << ifc_id << "," 
        << field.width() << ","
        << setter << ","
        << clearer << ","
        << getter << ","
        << field.default_value() << "," 
        << reloc_type << ","
        << reloc_extra << ","
        << reloc_width << ","
        << assm <<  ","
        << getEnumArrayName(&field) << ","
        << pfx_setter << "},\t// " << cnt << "\n";      
    }
    
    void genFieldArray(ostream &o,StrIntMap &field_indices,int count)
    {
      const InstrFieldHash &fields = _core.instrfields();
      gc_set<gc_string> unique_fields;

      o << "// Instruction operands.\n";
      o << "static struct adl_field " << _archpfx << "_operands [] = {\n";
      ForEach(fields,i) {
        const InstrField &field = *(i->second);
        if (!field.is_pseudo()) {
          auto si = field.splitInfo();
          gc_set<unsigned> widths = instr_widths(field,_core);
          unique_fields.insert((field.interface()) ? field.interface()->name() : field.name());
          ForEach(widths,w) {
            gc_string fname = i->first + get_width_sfx(*w);
            gc_string pfx_setter = (si && si->has_width(*w) ? "set_" + fname + "_prefix" : "0");
            gc_string getter = (field.type() == InstrField::tInstr) ? getMicroOpGetterName(field,*w) : "0";
            writeFieldArrayElement(o,fname,field,field_indices,true,pfx_setter,getter);
          }
        }
        else {
          // We add in pseudo-fields in case we need default values.
          writeFieldArrayElement(o,i->first,field,field_indices,false,"0","0");
        }                 
      }
      if (fields.empty()) {
        o << "{ \"\",0,0,0,0,0,0,0,0,0,0,0 }\n";
      }
      o << "};\n\n"
        << "static const int num_" << _archpfx << "_operands = " << count << ";\n\n";

      // Now create a list of "simple" names useful for error messages.  This
      // will be searchable by id, so it's sorted so that we can use a binary
      // search on it.  First, create a unique list of names.
      gc_map<unsigned,gc_string> fields_by_index;
      ForEach(unique_fields,i) {
        IfFind(x,field_indices,*i) {
          fields_by_index.insert(make_pair(x->second,x->first));
        }
      }
      o << "static struct adl_name_pair " << _archpfx << "_operands_by_index [] = {\n" << dec;
      if (fields_by_index.empty()) {
        o << " { \"\", 0 }\n";
      } else {
        ForEach (fields_by_index,i) {
          o << "{ \"" << i->second << "\", " << i->first << " },\n";
        }
      }
      o << "};\n\n"
        << "static const int num_" << _archpfx << "_operands_by_index = " << fields_by_index.size() << ";\n\n";

    }

    gc_string findRelocBySize(const Relocs &relocs,unsigned size,const gc_string &default_value)
    {
      ForEach(relocs,i) {
        const Relocation &reloc = **i;

        if (!reloc.pcrel() && !reloc.right_shift() && !reloc.action() && !reloc.field() && reloc.field_width() == size) {
          return to_string(reloc.value()).c_str();
        }
      }
      
      return default_value;
    }

    void genDefaultRelocs(ostream &o,const Relocs &relocs)
    {
      // Scan relocs to find a simple relocation (no actions) that matches the various sizes.
      // If we don't find something, then use the default.
      StrVect relocs_by_size(9);

      relocs_by_size[1] = findRelocBySize(relocs,8,"BFD_RELOC_8");
      relocs_by_size[2] = findRelocBySize(relocs,16,"BFD_RELOC_16");
      relocs_by_size[3] = findRelocBySize(relocs,24,"BFD_RELOC_24");
      relocs_by_size[4] = findRelocBySize(relocs,32,"BFD_RELOC_32");
      relocs_by_size[8] = findRelocBySize(relocs,64,"BFD_RELOC_64");
      
      // Then generate the size -> reloc mapping function.
      o << "extern \"C\" int adl_size_lookup(unsigned size)\n"
        << "{\n"
        << "switch (size) {\n"
        << "case 1:\n"
        << "return " << relocs_by_size[1] << ";\n"
        << "break;\n"
        << "case 2:\n"
        << "return " << relocs_by_size[2] << ";\n"
        << "break;\n"
        << "case 3:\n"
        << "return " << relocs_by_size[3] << ";\n"
        << "break;\n"
        << "case 4:\n"
        << "return " << relocs_by_size[4] << ";\n"
        << "break;\n"
        << "case 8:\n"
        << "return " << relocs_by_size[8] << ";\n"
        << "break;\n"
        << "default:\n"
        << "return -1;\n"
        << "}\n"
        << "}\n\n";
    }
    
    // Generates a table of relocations, if they exist.  Returns true if
    // anything was generated, false otherwise.
    bool genRelocArray(ostream &o,const StrIntMap &field_indices)
    {
      Relocs relocs;
      ForEach(_core.relocs(),i) {
        relocs.push_back(i->second);
      }
      sort(relocs.begin(),relocs.end(),sort_relocations_by_name);
      
      // We always have to at least generate a stub, for linking purposes on
      // Windows.
      genRelocHowtos(o, relocs);

      genDefaultRelocs(o,relocs);
      
      if (!relocs.empty()) {       
        o << "\n// Relocations ordered by name, for fast name -> type searches.\n"
          << "static struct adl_reloc_name " << _archpfx << "_relocs_by_index [] = {\n" << dec;
        unsigned count = 0;
        ForEach (relocs,i) {
          const Relocation &r = **i;
          o << " { \"" << r.asmstr() << "\", " << count++ << ", ";
          if (r.field()) {
            gc_set<unsigned> widths = instr_widths(*r.field(),_core);
            // If this relocation is associated with a field, then we need to
            // match on that as well.  For example, a name of 'lo' might be
            // associated with a store's immediate field and a load's immediate
            // field, which might differ in their bit positions.
            ForEach(widths,w) {
              gc_string fname = r.field()->name() + get_width_sfx(*w);
              auto iter = field_indices.find(fname);
              if (iter != field_indices.end()) {
                o << iter->second;
              } else {
                o << "-1";
              }
            }
          } else {
            o << "-1";
          }
          o << ", " << ( (_reloc_extras.count(r.name())) ? "true" : "false" )
            << " }, // " << r.name() << "\n";
        }
        o << "};\n\n"
          << "static const int num_" << _archpfx << "_relocs_by_index = " << _core.relocs().size() << ";\n\n";

        // Then list out the offset table.  These are the explicit offsets set
        // directly from ADL.
        o << "\n// Relocation offsets.\n"
          << "static struct adl_int_pair " << _archpfx << "_reloc_offsets [] = {\n" << dec;
        ForEach(relocs,i) {
          const Relocation &r = **i;
          o << " { " << r.value() << "," << r.offset() << " }, // " << r.name() << "\n";
        }
        o << "};\n\n"
          << "static const int num_" << _archpfx << "_reloc_offsets = " << _core.relocs().size() << ";\n\n";

        return true;
      }

      return false;
    }

    gc_string getMicroOpGetterName(const InstrField &f,unsigned w)
    {
      return f.name() + get_width_sfx(w) + "_std_getter";
    }
    
    // Generate getter functions used by macro-ops to extract micro-op fields.
    void genMicroOpGetters(ostream &o)
    {
      if (!_core.has_micro_ops()) return;

      ForEach(_core.instrfields(),i) {
        const InstrField &f = *(i->second);

        if (f.is_pseudo() || f.type() != InstrField::tInstr) continue;

        gc_set<unsigned> widths = instr_widths(f,_core);
        ForEach(widths,w) {
          o << "static bfd_uint64_t " << getMicroOpGetterName(f,*w) << "(unsigned *data) {\n"
            << "return get_" << f.name() << get_width_sfx(*w) << "(data[0],data[1]).uint64();\n"
            << "}\n\n";
        }
      }
    }
    
    // Generates the instruction table.
    // Returns the maximum field count encountered.
    unsigned genInstrTables(ostream &o, const PrefixVarInfos &prefix_vars,
                            StrIntMap &field_indices, StrIntMap &blk_indices,
                            const AsmRuleHash &asm_rules, const AsmParmHash &asm_parms, 
                            const IntSet &instr_attrs, gc_string &post_pkt_rule, bool dest)
    {
      // First, generate the instruction field table and populate a
      // lookup table of name->index fields.

      const InstrFieldHash &fields = _core.instrfields();
      int id = 0;

      ForEach(fields,i) {
        const InstrField &f = *(i->second);
        int first_id = -1;
        if (!f.is_pseudo()) {
          gc_set<unsigned> widths = instr_widths(f,_core);
          ForEach(widths,w) {
            gc_string sfx = get_width_sfx(*w);
            gc_string fname = i->first + sfx;
            field_indices.insert(make_pair(fname,id));
            if (first_id < 0) first_id = id;
            ++id;
          }
          // Insert the original name, pointing to the first item, so that an
          // alias can find a version for querying things like default values.
          if (add_nowidth_field(f)) {
            field_indices.insert(make_pair(i->first,first_id));
          }
        }
        else {
          // We insert a pseudo-field, since we might need its default value.
          field_indices.insert(make_pair(i->first,id++));            
        }
      }
      int count = id; 

      // cout << "Field indices:  ";
      // printAssoc(cout,field_indices,false);
      // cout << endl;

      // We only need getters if we have micro-ops.
      if (_core.has_micro_ops()) {
        genInstrFieldGetters(o,_core,"static inline",false,true);
        genMicroOpGetters(o);
      }

      // Generate field setters and clearers.
      genInstrFieldSetters(o,_core,true,"static ");
      genInstrFieldClearersAll(o,_core,"static ");

      genInstrFieldCheckers(o,_core,"static ");

      // Create any enumerated-field arrays.
      genEnumFieldData(o);

      genFieldArray(o,field_indices,count);

      // Enum type for block types.
      o << "enum InstrBlocks {\n";
      ForEach(blk_indices,i) {
        o << i->first << " = " << i->second << ",\n";
      }
      o << "};\n\n";

      post_pkt_rule = genAsmRuleCheckers(o,asm_rules,prefix_vars,field_indices,asm_parms);

      unsigned maxfields = 0;

      FInfo::_explicit_regs = _core.asm_config().explicit_regs();

      // Iterate over all instruction tables, generating code for them.
      if (_core.asm_config().asm_instrtables().empty()) {
        ForEach(_core.instrtables(),i) {
          const Instrs &instrs = i->second._instrs;
          gc_string name = Data::get_attr_name(i->first);
          maxfields = max(maxfields,genInstrTable(o,name,instrs,field_indices,blk_indices,asm_rules,prefix_vars,asm_parms,instr_attrs,dest));
        }
      } else {
        Instrs all_instrs;
        ForEach(_core.instrtables(),i) {
          const Instrs &instrs = i->second._instrs; 
          gc_string name = Data::get_attr_name(i->first);
          if (name != PrefixInstrTable) {
            all_instrs.insert(all_instrs.end(),instrs.begin(),instrs.end());
          } else {
            genInstrTable(o,PrefixInstrTable,instrs,field_indices,blk_indices,asm_rules,prefix_vars,asm_parms,instr_attrs,dest);
          }
        }
        maxfields = max(maxfields,genInstrTable(o,DefaultInstrTable,all_instrs,field_indices,blk_indices,asm_rules,prefix_vars,asm_parms,instr_attrs,dest));
      }

      return maxfields;
    }



    bool genAsmInstrAttributes(ostream &o,const AsmRuleHash &rules,IntSet &instr_attrs) 
    {
      InstrAttrWalker w(_core,instr_attrs);
    
      if (_core.pre_asm_handler()) {
        funcBody(w,_core.pre_asm_handler()._func);
      }
      if (_core.post_asm_handler()) {
        funcBody(w,_core.post_asm_handler()._func);
      }
      if (_core.post_packet_asm_handler()) {
        funcBody(w,_core.post_packet_asm_handler()._func);
      }
    
      ForEach(rules,r) {
        AsmRule &rule = *(r->second);
        funcBody(w,rule.action());
      }

      ForEach(_core.helpers(),i) {
        const gc_string &fname = i->first;

        if (!_core.asm_helpers().count(fname)) continue;

        funcBody(w,i->second._func);
      }
    
      if (!instr_attrs.empty()) {
        uint32_t flag = 0;
        o << "enum InstrAttrs {\n" << hex;
        ForEach(instr_attrs,iter) {
          o << getAttrName(Data::get_attr_name(*iter)) << " = 0x" << flag << ",\n";
          flag += 1;
          if (flag > 63) {           
            std::cerr << "\nThe number of attributes is over the limit.\n";
            exit(0);
          }
        }
        o << "};\n\n" << dec;
      }
      return w.msgs_used();
    }

    int  getFieldId(Ptree *exp,const Instr &instr,const StrIntMap &field_indices) 
    {
      if (exp) {
        const InstrField *f = 0;
        if (auto alias = instr.get_alias()) {
          const char *nm = exp->ToString(); 
          ForEach(alias->_fields,i) {
            Instr::Field fld = *i;
            if (!strcmp(fld._name,nm) && fld._sname)
              f = _core.get_instrfield(fld._sname); 
          }
        } else {
          f = _core.get_instrfield(exp->ToString());
        }
        if (f && !f->is_prefix()) {
          gc_string enc_name = (f->encap_field() ? f->encap_field()->name() + "." : ""); 
          gc_string name = enc_name + f->name();
          auto it = field_indices.find(name);
          if (it == field_indices.end()) {
            RError("Unknown instruction field: " << exp->ToString());
          }
          return it->second;
        }
      }
      return -1;
    }
    

    int getRegIndex(Ptree *exp,const Instr &instr,const StrIntMap &field_indices,int defVal)
    {
      int val = getFieldId(exp,instr,field_indices);
      if (val < 0) { // not a field name
        if (!exp || !get_int(val,exp,instr.env())) {
          val = defVal;
        }
      } else {
        val = -val-1;
      }
      return val;
    }


    Ptree *genInstrDests(ostream &o, const Instr &instr,const StrIntMap &field_indices,bool dest)
    {
      const UsageInfo &ui = instr.usageinfo();
      const UsedRegs  &trg_regs = ui._trg_regs;
      
      if (!dest || trg_regs.empty()) {
        return Zero;
      }
      gc_set<UsedReg>  used_regs(trg_regs.begin(),trg_regs.end());
      
      Ptree* sym = AdlGenSym();
      int cnt = 0;
      o << "// reg_writes getter\n"
        << "static void* " << sym << "(void)\n"
        << "{\n"
        << "static struct reg_writes rwrites = {{\n";
      ForEach(used_regs,i) {
	
        if (i->regb()) {
          const RegFile *rf = i->regfile();
          int  id = ((rf && rf->sparse()) ? -rf->id() : i->regb()->id());
          int  begin = getRegIndex(i->_begin,instr,field_indices,0);
          int  end   = getRegIndex(i->_end,instr,field_indices,begin);
          RNumber mask = i->_mask;
          if (mask.iszero()) {
            mask.setAll();
          }
          o << "{" << id << "/* " << i->regb()->name() << " */,"
            << begin << "," << end << ","
            << hex << "{0x" << mask.uint64() << "ULL,0}" << dec << "},\n";
          ++cnt;
        }
      }
      o << "}," << cnt << "};\n"
        << "return (void *) &(rwrites);\n"
        << "}\n";
      
      return sym;
    }

    // This creates a function to check arguments across an instruction.
    // Currently, we just check to see that if a field is repeated, then its
    // value must be the same.
    Ptree *genInstrArgChecker(ostream &o,const Instr& instr)
    {
      if (const Instr::Syntax *syntax = instr.syntax()) {
        gc_map<gc_string,gc_vector<int> > index_map;
        // Build up a map of name -> indices.  We only care about direct fields,
        // e.g. no expressions, aliases, etc.
        ForRange(syntax->_items.size(),i) {
          const Instr::Field &fp =  syntax->_items[i].first;
          if (fp._type == InstrBase::Field::Ref) {
            index_map[fp._field->name()].push_back(i);
          }
        }
        // Now see if we have any duplicates.
        bool have_dupl = false;
        ForEach(index_map,i) {
          if (i->second.size() > 1) {
            have_dupl = true;
          }
        }
        if (have_dupl) {
          // We do have duplicates, so we need a checker.
          Ptree *name = Ptree::Make("%p_argchecker",AdlGenSym());
          o << "static bool " << name << " (const expressionS *operands) {\n";
          ForEach(index_map,i) {
            if (i->second.size() > 1) {
              o << "if (!(";
              bool first = true;
              for (unsigned cur = 1 ; cur != i->second.size(); ++cur) {
                pfx_sep(o," && ",first);
                o << "(operands[" << i->second[cur-1] << "].X_add_number == operands[" << i->second[cur] << "].X_add_number)";
              }
              o << ")) return false;\n";
            }
          }
          o << "return true;\n"
            << "}\n\n";
          return name;
        }
      }
      return Zero;
    }

    Ptree *genInstrRules(ostream &o, const Instr& instr)
    {

      auto rules = instr.asmrules();
      if (rules.empty()) {
        return Zero;
      }
    
      // No need to generate a checker
      if (rules.size() == 1) {
        gc_string checker = getRuleCheckerName(rules.front());
        return Ptree::Make("&(%s)",checker.c_str());
      }

      Ptree* sym = AdlGenSym();

      o << "// Assembler rule checker\n"  << dec
        << "static void " << sym << "(InstrInfo &ii ATTRIBUTE_UNUSED,int current_position ATTRIBUTE_UNUSED)\n"
        << "{\n";
      ForEach(rules,r) {
        o << getRuleCheckerName(*r) << "(ii,current_position);\n"; 
      }
      o << "}\n\n";

      return Ptree::Make("&(%p)",sym);
    }

    Ptree *genInstrAttrs(ostream &o,const Instr &instr,const IntSet &instr_attrs) 
    {
      if (instr_attrs.empty()) {
        return 0;
      }

      const AttrList &attrs = instr.attrs();

      // First, do we have any values?  If so, then create an array of them,
      // otherwise, we don't.  For now, we only support simple ints and strings.
      bool has_values = false;
      ForEach(attrs,a) {
        if (instr_attrs.count(a->first) && a->second._type != AttrParams::NoParam) {
          has_values = true;
          break;
        }
      }
      Ptree *values = 0;
      if (has_values) {
        values = AdlGenSym();
        o << "adl_instr_attr_val " << values << "[] = { ";
        ForEach(attrs,a) {
          if (instr_attrs.count(a->first)) {
            o << "{ ((uint64_t)(1ULL << " << getAttrName(a->first) << ")), ";
            switch (a->second._type) {
            case AttrParams::Int:
            case AttrParams::IntList:
              o << " " << a->second._int_list.front() << ", 0 ";
              break;
            case AttrParams::Str:
            case AttrParams::StrList:
              o << " 0, \"" << a->second._str_list.front() << "\" ";
              break;
            default:
              o << " 0, 0 ";
              break;
            }
            o << "}, ";
          }
        }
        o << "{ 0, 0, 0 } };\n";
      }

      Ptree *name = AdlGenSym();
      o << "adl_instr_attrs " << name << " = { ";

      bool first = true;
      ForEach(attrs,a) {
        if (instr_attrs.count(a->first)) {
          pfx_sep(o," | ",first);
          o << "((uint64_t)(1ULL << " << getAttrName(a->first) << "))";
        }
      }
      if (first) {
        o << 0;
      }
      o << " , ";
      if (values) {
        o << values;
      } else {
        o << "0";
      }
      o << " };\n";
        
      return name;
    }


    // Generates a table of register names.
    void genRegNames(ostream &o)
    {
      // For each register file, if it has a prefix, then insert its elements.
      // For all sparse register files, insert the name and its register index.
      o << "static const struct adl_name_pair " << _archpfx << "_regnames[] = {\n" << dec;

      int count = 0;
      StrSet used_names;
      const RegFileHash &regfiles = _core.regfiles();
      ForEach (regfiles,i) {
        const RegFile &rf = *(i->second);
        if (!rf.prefix().empty() && !rf.sparse()) {
          // Non-sparse register file with a prefix.  Install prefix<num> entries.
          ForRange(rf.size(),j) {
            gc_ostringstream os;
            os << rf.prefix() << j;
            gc_string name = lowercase(os.str());
            if (used_names.count(name) == 0) {
              o << "{\"" << name << "\"," << j << "},\n";
              ++count;
              used_names.insert(name);
            }
          }
        } else if (!rf.prefix_for_regname().empty() && !rf.sparse() && rf.prefix().empty()) {
          // Non-sparse register file with a prefix for regname only.  Install prefix<num> entries.
          ForRange(rf.size(),j) {
            gc_ostringstream os;
            os << rf.prefix_for_regname() << j;
            gc_string name = os.str();
            if (used_names.count(name) == 0) {
              o << "{\"" << name << "\"," << j << "},\n";
              ++count;
              used_names.insert(name);
            }
          }
        } else if (rf.sparse()) {
          // Sparse register file- install constituent registers' names with their indices.
          ForEach(rf.entries(),j) {
            const RegEntry &entry = *(j->second);
            gc_string name("");
            if (!entry.syntax().empty()) {
              name = lowercase(entry.syntax());
            } else if (entry.reg().reg()) {
              name =  lowercase(entry.reg()()->name());
            }
            if (!name.empty() && used_names.count(name) == 0) {
              o << "{\"" << name << "\"," << entry.id() << "},\n";
              ++count;
              used_names.insert(name);
            }
          }
		}
      }
      if (count == 0) {
        o << "{ \"\",0 }\n";
      }
      o << "};\n\n"
        << "static const int num_" << _archpfx << "_regnames = " << count << ";\n\n";
    }

    
    // Generate a table of instruction mnemonics from all instruction tables.
    // This also records any pre-instruction name fields that exist.
    void genInstrNames(ostream &o) 
    {
      _min_instr_width = numeric_limits<unsigned>::max();
      set<unsigned> instr_widths;

      _instr_pfx_fields.clear();
      _instr_names.clear();
      ForEach(_core.instrtables(),t) {
        const Instrs &instrs = t->second._instrs;
        ForEach(instrs,i) {
          const Instr &instr = **i;
          
          unsigned instr_width = instr.width()/8;
          if (instr_width == 0) instr_width = 1;
          _min_instr_width = min(_min_instr_width,instr_width);
          instr_widths.insert(instr_width);

          // Record the name.
          gc_string nm = getInstrName(instr);
          _instr_names.insert(nm);
          
          // Do we have preceding fields?
          getInstrPfxFields(_instr_pfx_fields,instr);
        }
      };

      // If we have any instruction widths which are not even multiples of each
      // other, then we set our minimum width to 1 to avoid alignment issues.
      assert(_min_instr_width);
      ForEach(instr_widths,i) {
        if ( (*i % _min_instr_width) != 0 ) {
          _min_instr_width = 1;
          break;
        }
      }

      o << "static const char *instr_names[] = {\n";
      if (_instr_names.empty()) {
        o << " 0 ";
      } else {
        ForEach(_instr_names,i) {
          o << "\"" << *i << "\",\n";
        }
      }
      o << "};\n\n"
        << "static int num_instr_names = " << _instr_names.size() << ";\n\n";

      o << "static const char *instr_pfx_fields[] ATTRIBUTE_UNUSED = {\n";
      if (_instr_pfx_fields.empty()) {
        o << " 0 ";
      } else {
        ForEach(_instr_pfx_fields,i) {
          o << "\"" << *i << "\",\n";
        }
      }
      o << "};\n\n"
        << "static int num_instr_pfx_fields ATTRIBUTE_UNUSED = " << _instr_pfx_fields.size() << ";\n\n";
    }

    // Generate a table of instruction-table section names.
    void genITableNames(ostream &o)
    {
      o << "static const char *" << _archpfx << "_itnames[] = {\n" << dec;

      if (!_core.asm_config().asm_instrtables().empty()) {
        o << "\"default\",\n";
      } else {
        ForEach(_core.instrtables(),i) {
          const gc_string &n = Data::get_attr_name(i->first);
          if (n == DefaultInstrTable) {
            o << "\"default\",\n";
          } else {
            o << "\"" << n << "\",\n";
          }
        }
      }
      o << "};\n\n";
    }

    // Returns prefix variables along with default value
    void  get_prefix_vars(PrefixVarInfos &prefix_vars) {
      ForEach(_core.instrfields(),i) {
        const InstrField &f = *(i->second);
        if (f.is_prefix() && f.is_pseudo()) {
          unsigned mask  = -1;
          if (f.index()) {
            int shift = sizeof(int)*8 - f.index();
            if (_core.is_little_endian()) {
              mask <<= shift;
            } else {
              mask >>= shift;
            }
          } 
          // HACK: We're just assuming 0 as a decent default.
          int def_val = f.has_default_value() ? f.default_value() : 0;
          prefix_vars.insert(make_pair(f.name(),PrefixVarInfo(def_val,f.width(),f.index(),
                                                              mask)));
        }
      }
    }

    void calcBlkIndices(StrIntMap &blk_indices) 
    {
      const Blocks &blocks = _core.blocks();
      int  id = 1;
      ForEach(blocks,b) {
        blk_indices.insert(make_pair(*b,id++));
      }
    }

    UIntVect getInstrBlks(const Instr &instr, const StrIntMap &blk_indices)
    {
      UIntVect blks;

      const StrList &bnames = instr.blks();
      ForEach(bnames,bn) {
        auto it = blk_indices.find(*bn);
        if (it != blk_indices.end()) {
          blks.push_back(it->second);
        }
      }
      return blks;
    }

    void genPrefixCounters(ostream &o)
    {
      o << "// Prefix counters.\n";

      ForEach(_core.prefix_counters(),i) {
        const PrefixCounter &pc = *i->second;

        o << "static unsigned " << pc.name() << ";\n";
      }
      o << "\n"
        << "static void reset_prefix_counters()\n"
        << "{\n";
      ForEach(_core.prefix_counters(),i) {
        const PrefixCounter &pc = *i->second;

        o << pc.name() << " = " << pc.reset_value() << ";\n";
      }      
      o << "}\n\n";
    }

    void genPrefixVars(ostream &o,const PrefixVarInfos &pfx_vars,unsigned queue_size) 
    {
      o << "// Prefix variables.\n";

      ForEach(pfx_vars,i) {  
        const PrefixVarInfo &pinfo = i->second;
        o << "static struct adl_prefix_field " << i->first <<  "[] = {";
        ForRange(queue_size,k) {
          o << "{" <<  std::dec << pinfo._default << ", " << pinfo._default << ", "
            <<  pinfo._width << ", " << pinfo._field_width << ", 0x" << std::hex << pinfo._mask << "},"; // TBD check for field width
        }
        o << " };\n";
      }

      int n=0;
      ForRange(queue_size,k) {
        o << "struct adl_prefix_field *pfx_fields" << n << "[] = {";
        if (pfx_vars.empty()) {
          o << " 0 ";
        } else {
          ForEach(pfx_vars,i) {
            o << " &" << i->first << "[" << n  << "],";
          }
        }
        ++n;
        o << "};\n";
      }
      o << "struct adl_prefix_fields pfx_queue[] = {";
      n=0;
      ForRange(queue_size,k) {
        o << "{pfx_fields"  << n++ << "},";
      }
      o << "};\n";
    }

    void genPostAsmHandlers(ostream &o,const PrefixVarInfos &pfx_vars, StrIntMap &field_indices,const AsmParmHash &asm_parm)
    {
      PostAsmWalker w(_core,pfx_vars,field_indices,asm_parm);

      if (_core.pre_asm_handler()) {
        w.set_mode(PostAsmWalker::PreAsmMode);
        o << "static void pre_asm(int current_position ATTRIBUTE_UNUSED)\n"
          << funcBody(w,_core.pre_asm_handler()._func) << "\n";
      }
      w.set_mode(PostAsmWalker::PostAsmMode);
      if (_core.post_asm_handler()) {
        o << "static void post_asm(" << get_func_args(_core.post_asm_handler()._func) 
          << " ATTRIBUTE_UNUSED, int current_position ATTRIBUTE_UNUSED)\n"
          << funcBody(w,_core.post_asm_handler()._func) << "\n";
      }

      // If we have a post-packet handler, then we'll use it.  Otherwise, we
      // create one which just sets prefix info (if we have prefix bits).
      if (_core.post_packet_asm_handler()) {
        StrList pp_arg;
        pp_arg.push_back("InstrBundle");

        Ptree *arg = get_func_call_arg(_core.post_packet_asm_handler()._func,0);
        w.set_packet_arg_name(Second(arg));
        o << "static void post_packet_asm(" << referize_args(_core.post_packet_asm_handler()._func,pp_arg) << ",int current_position ATTRIBUTE_UNUSED)\n"
          << funcBody(w,_core.post_packet_asm_handler()._func) << "\n";
      } else if (_core.prefix_bits()) {
        o << "static void post_packet_asm(const InstrBundle &b)\n"
          << "{\n"
          << "b.set_prefix_fields();\n"
          << "}\n\n";
      }
    }


    void genAsmInstrs(ostream &o,const PrefixVarInfos &pfx_vars, 
                      const StrIntMap &field_indices, 
                      const AsmParmHash &asm_parms)
    {
      StrStrMap instrs;
      PostAsmWalker w(_core,pfx_vars,field_indices,asm_parms);
      ForEach(_core.instrs(),i) {
        const Instr &instr = *(i->second);
        if (instr.assembler() && !instr.action()) { // Instruction has only assembler
          Ptree *s = AdlGenSym();
          o << "static void " << s << "(int current_position ATTRIBUTE_UNUSED) {\n"
            << funcBody(w,instr.assembler())  << "\n}\n";
          if (!instr.action()) {
            instrs.insert(make_pair(getInstrName(instr),s->ToString()));
          } 
        }
      }
      o << "static struct adl_asm_instr asm_instrs[] ATTRIBUTE_UNUSED = {\n";
      if (instrs.empty() ) {
        o << " {\"\",0 } ";
      } else {
        ForEach(instrs,i) {
          o << "{\"" << i->first << "\", " << i->second << "},";
        }
      }
      o << "\n};\n";
      _num_asm_instrs = instrs.size();
      o << "static int num_asm_instrs ATTRIBUTE_UNUSED = " << _num_asm_instrs << ";\n";
    }

    gc_string setup_pfx_vars(const PrefixVarInfos &pfx_vars,unsigned queue_size) {
      return "";
      gc_ostringstream os;
      ForRange(queue_size,k) {
        int n = 0;
        ForEach(pfx_vars,i) {
          os << "pfx_fields[" << k << "][" << (n++) << "]" << " = &" << i->first << "[" << k << "];\n";
        }
      }
      return os.str();
    }

    // This figures out how we delineate the instruction's name when parsing an
    // assembler line.  We do this by always considering whitespace to be
    // terminating, but then we look at the leading characters for all possible
    // values of any enumerated fields, for fields which immediately follow the
    // instruction name.  We then look at the ending characters of any
    // instructions.  If there's an overlap, then we generate an error.  We may
    // have to do something more sophisticated than this later, but I think this
    // will work for now.
    void calcTerminatingChars()
    {
      set<char> terms;

      // First, generate character set of enumerated fields.  We only look at
      // enumerated fields which immediately follow the instruction name.
      ForEach(_core.instrs(),i) {
        if (auto syntax = i->second->syntax()) {
          // We only care if we have an instruction string, because otherwise
          // the default syntax always includes spaces between the name and the
          // first field.  We also only care about abutting fields, so we just
          // need to check the char immediately after the instruction name.
          gc_string::const_iterator iter = syntax->_fmt.begin()+syntax->_in_end;
          if (iter != syntax->_fmt.end() && syntax->isfield(*iter)) {
            const Instr::Field &fp = syntax->get(*iter);
            const InstrField &field = *(fp._field);
            ForEach(field.enums(),j) {
              if (!j->first.empty() && j->first != ReservedEnum && !_instr_pfx_fields.count(j->first)) {
                terms.insert( j->first.at(0) );
              }                
            }
          }
        }
      }

      _terminating_chars = "\\t ";
      _min_terminating_chars = _terminating_chars;
      ForEach(terms,i) {
        _terminating_chars.push_back(*i);
      }

      // Now remove instruction name chars in order to avoid conflicts.
      ForEach (_core.instrs(),i) {
        gc_string nm = getInstrName(*(i->second));
        ForRange (nm.size(),ii) {
          char c = nm.at(ii);
          IfFind (x,terms,c) {
            terms.erase(x);
          }
        }
      }

      // Then remove any chars from the possible prefix field values.
      ForEach(_instr_pfx_fields,i) {
        ForRange(i->size(),ii) {
          char c = i->at(ii);
          IfFind (x,terms,c) {
            terms.erase(x);
          }
        }
      }

      ForEach(terms,i) {
        _min_terminating_chars.push_back(*i);
      }
    }

    bool single_char(const gc_string &str) {
      return str.length() == 1 || (str.length() == 2 && str[0] == '\\');
    }

    // Return single char strings in the form of "char *"
    // If line_separator -> we do not print out '\0' and '\n' because they are defaults and
    // when printed confuse assembler.
    gc_string get_single_char(const StrList &strs, bool no_white = false) {
      gc_string ret;
      ForEach(strs,i) {
        if (single_char(*i)) {
          if (no_white && (*i == "\\0" || *i == "\\n" || *i == " ")) {
            continue;
          } else {
            ret += *i;
          }
        }
      }
      return ret;
    }

    // Return multi char strings in the form "char **"
    gc_string get_multi_char(const StrList &strs) {
      gc_ostringstream os;
      int num = 0;
      ForEach(strs,i) {
        if (!single_char(*i)) {
          os << "\"" << *i << "\",";
          ++num;
        }
      }
      return (num  == 0) ? " 0 " : os.str();
    }
    
    // Return number of multi char strings in the form "char **"
    int get_num_multi_char(const StrList &strs) { 
      int num = 0;      
      ForEach(strs,i) {
        if (!single_char(*i)) {
          ++num;
        }
      }
      return num;
    }

    // Packet characters are get from grouping characters and we assume that line separator is
    // also packet separator with the lowest priority. '\0' is always packet separator.
    gc_string get_packet_char(const AsmConfig::StrPairList &glist, const StrList &seplist, bool begin,bool ignore_eol = false) {
      gc_ostringstream os;
      ForEach(glist,i) {
        if (begin) {
          if ( ignore_eol && i->first == "\\n") continue;
          os << i->first;
        } else {
          if ( ignore_eol && i->second == "\\n") continue; //
          os << i->second;
        }
      }
      
      return os.str();
    };

    int num_tables() {
      if (_core.asm_config().asm_instrtables().empty()) {
        return _core.instrtables().size();
      } else {
        if (_core.has_prefix_instruction()) {
          return 2;
        } else {
          return 1;
        }
      }
    };

    void genHelperDecls(ostream &o)
    {
      const HelperMap &helpers = _core.helpers();
      ForEach (helpers,i) {
        const gc_string &fname = i->first;
        
        // Skip if it's not an assembly helper.
        if (!_core.asm_helpers().count(fname)) continue;

        Ptree *func = i->second._func;

        o << First(func) << ' ' << Second(func) << ' ' << Third(func) << ";\n";
      }
    }

    void genHelpers(ostream &o,const PrefixVarInfos &pfx_vars, 
                    const StrIntMap &field_indices, 
                    const AsmParmHash &asm_parms)
    {
      // Spit out helper code here.
      o << "//\n"
        << "// Helper code.\n"
        << "//\n\n";
      
      PostAsmWalker w(_core,pfx_vars,field_indices,asm_parms);

      const HelperMap &helpers = _core.helpers();

      ForEach (helpers,i) {
        const gc_string &fname = i->first;

        // Skip if it's not an assembly helper.
        if (!_core.asm_helpers().count(fname)) continue;
        
        o << w.Translate(i->second._func) << "\n\n";
      }

      o << "\n";
    }

    // Main entry point for assembler generation.
    void exec(ostream &o)
    {
      if (_archpfx.empty()) {
        RError("An architecture prefix (--arch-pfx or --ap) must be specified for generating an assembler.");
      }
      o << "/*\n\
	\n\
	Assembler code implementation file for " << _archpfx << " generated by ADL.\n\
	\n\
	*/\n\
\n\
#ifndef _MSC_VER\n\
extern \"C\" {\n\
#  include \"as.h\"\n\
}\n\
#endif\n\
\n\
#include <stdio.h>\n\
#include <sstream>\n\
#include <string>\n\
\n\
extern \"C\" {\n\
#  include \"as.h\"\n\
#  include \"bfd/elf-bfd.h\"\n\
#  include \"bfd/bfd.h\"\n\
#  include \"bfd/libbfd.h\"\n\
}\n\
\n\
#define __NO_RNUMBER__\n\n";

      gc_string lepfx;
      if (_core.is_little_endian()) {
        lepfx = "le_";
      }

      o << "#include \"adl-asm-impl.h\"\n"
        << "#include \"adl-asm-info.h\"\n\n"

        << "#include \"helpers/Macros.h\"\n"
        << "#include \"helpers/" << lepfx << "sintbv.h\"\n"
        << "#define bits " << lepfx << "intbv\n"
        << "#define sbits " << lepfx << "sintbv\n\n"
        << "#define ns_bits " << lepfx << "intbv\n"
        << "#define ns_sbits " << lepfx << "sintbv\n\n"
        << "#include \"helpers/LogUsage.h\"\n"
        << "#include \"helpers/TypeConv.h\"\n"
        << "extern \"C\" {\n"
        << "#include \"tc-" << _archpfx << ".h\"\n"
        << "}\n"
        << "\n"
        << "using namespace std;\n\n"
        << "using namespace adl;\n"
        << "\n\n";

      // Needed functions for specifying architecture and machine.
      auto const & assm = _core.asm_config();
      o << 
        "\n"
        "/* Figure out the BFD architecture to use.  These functions \n"
        "   are called well before md_begin, when the output file is opened.  */ \n"
        "\n"
        "enum bfd_architecture " << _archpfx << "_arch () \n"
        "{\n"
        "return ADL_TARGET_ARCH;\n"
        "}\n"
        "\n"
        "const char adl_parallel_separator_chars[] = \"" << get_single_char(assm.all_group_seps(),true) << "\";\n"
        "const char adl_symbol_chars[] = \"" << get_single_char(assm.symbol_chars()) << "\";\n"
        "const char comment_chars[] = \""        << get_single_char(assm.comments())           << "\";\n"
        "const char line_comment_chars[] = \""   << get_single_char(assm.line_comments())      << "\";\n"
        "const char line_separator_chars[] = \"" << get_single_char(assm.line_seps(),true)     << "\";\n"
        "const char packet_begin_chars[] = \""   
        << get_packet_char(assm.group_seps(),assm.line_seps(),true)    << "\";\n"
        "const char packet_end_chars[] = \""   
        << get_packet_char(assm.group_seps(),assm.line_seps(),false)   << "\";\n"
        "static const char *init_comment_strs[] = {" << get_multi_char(assm.comments())            << "};\n"
        "static int init_num_comment_strs = "         << get_num_multi_char(assm.comments())        << ";\n" 
        "static const char *init_line_comment_strs[] = {" << get_multi_char(assm.line_comments())  << "};\n"
        "static int init_num_line_comment_strs = "    << get_num_multi_char(assm.line_comments())   << ";\n"
        "static int init_queue_size = " << assm.queue_size() << ";\n"
        "static int init_queue_offset = " << assm.queue_offset() << ";\n"

        "unsigned long " << _archpfx << "_mach ()\n"
        "{\n"
        "return default_adl_mach();\n"
        "}\n"
        "\n"
        "extern char*\n"
        << _archpfx << "_target_format ()\n"
        "{\n"
        "return default_adl_target_format();\n"
        "}\n\n";

      if (!assm.instr_seps().empty()) {
        o << "static const char instr_separator_chars[] = \"" << get_single_char(assm.instr_seps(),true)   << "\";\n";
      }

      // Generate the assembler global varibales
      genAsmParms(o,assm.parms());

      
      IntSet  instr_attrs;
      bool msgs_used = genAsmInstrAttributes(o,assm.rules(),instr_attrs);

      bool dest;
      // Generate the assembler error function
      genAsmRulesHelpers(o,assm.rules(),dest,msgs_used);

      // Filter out prefix fields
      PrefixVarInfos pfx_vars;
      get_prefix_vars(pfx_vars);

      // Generate variables that will hold prefix information
      genPrefixVars(o,pfx_vars,assm.queue_size());

      // Generate prefix counters, used to specify prefix slices.
      genPrefixCounters(o);

      // Associate an integer with blocks
      StrIntMap blk_indices;

      calcBlkIndices(blk_indices);

      genHelperDecls(o);

      StrIntMap field_indices;

      gc_string post_pkt_rule;
      // Generate the instruction table.
      unsigned maxfields = genInstrTables(o,pfx_vars,field_indices,blk_indices,assm.rules(),assm.parms(),instr_attrs,post_pkt_rule,dest);

      // Generate the list of register aliases.
      genRegNames(o);

      // Generate section names for instruction tables.
      genITableNames(o);

      // Generate relocation data, if relevant.
      bool relocs = genRelocArray(o,field_indices);

      // Generate a list of all instructions name
      genInstrNames(o);

      // Figure out how we terminate the search of the instruction name.
      calcTerminatingChars();

      // Generate any helper functions.
      genHelpers(o,pfx_vars,field_indices,assm.parms());

      // Generate post-asm and post-packet-asm hooks
      genPostAsmHandlers(o,pfx_vars,field_indices,assm.parms());

      //  Generate assembler instructions;
      genAsmInstrs(o,pfx_vars,field_indices,assm.parms());

      gc_string packetized_encoding =  (_core.packetized_encoding()) ? "1" : "0";
      gc_string pre_asm   = _core.pre_asm_handler()  ? "pre_asm" : "0"; 
      gc_string post_asm  = _core.post_asm_handler() ? "post_asm" : "0";
      gc_string post_pkt_asm  = (_core.post_packet_asm_handler() || _core.prefix_bits()) ? "post_packet_asm" : "0";


      o << "static int " << getTableSelect() << " = 0;\n"

        << "static bool show_warnings = false;\n"

        << "static struct hash_control *instr_hash[" << num_tables() << "];\n\n"

        // The register name hash.
        << "static struct hash_control *reg_hash;\n\n"

        // The assembler instruction hash
        << "static struct hash_control *asm_instr_hash;\n\n"

        // The instruction-prefix fields hash
        << "static struct hash_control *instr_pfx_fields_hash;\n\n"

        // Terminatin chars for the instruction name.
        << "static const char *terminating_chars     = \"" << _terminating_chars << get_single_char(assm.all_group_seps()) << "\";\n\n"
        << "static const char *min_terminating_chars = \"" << _min_terminating_chars << get_single_char(assm.all_group_seps()) << "\";\n\n"

        // Initialize endianness before main() so that it is set before we open
        // the output file.  This occurs before md_begin is called.
        << "static bool setup_endianness()\n"
        << "{\n"
        << "adl_setup_endianness(" << ((_core.instrEndianness() == Core::Big) ? "true" : "false") << "); return true;\n"
        << "}\n\n"
        << "static bool setup_endianness_var ATTRIBUTE_UNUSED = setup_endianness();\n\n"

        // Create the md_begin function, which handles initialization.
        << 
        "void md_begin()\n"
        "{\n";
      gc_string version = genUsageLogging(o,"asm",_asm_ver);
      o << setup_pfx_vars(pfx_vars,assm.queue_size()) 
        << "adl_setup_general(min_terminating_chars,terminating_chars," << packetized_encoding << "," << pre_asm 
        << "," << post_asm << "," <<  post_pkt_asm << "," << post_pkt_rule << ",init_queue_size, init_queue_offset, show_warnings, " 
        << ((_bigMem) ? "true" : "false") << "," << _min_instr_width << ");\n"
        << "adl_setup_comments(init_comment_strs, init_num_comment_strs, init_line_comment_strs, init_num_line_comment_strs);\n"
        << "adl_setup_instrfields(" << _archpfx << "_operands, num_" << _archpfx << "_operands," << _archpfx << "_operands_by_index, num_" << _archpfx << "_operands_by_index);\n"
        << "adl_setup_grouping(packet_begin_chars, packet_end_chars);\n"
        << "adl_setup_prefix(pfx_queue, " << pfx_vars.size();
      if (_core.has_prefix_instruction()) {
        o << ", prefix_instructions,num_prefix_instructions";
      } else {
        o << ",0,0";
      }
      o << ",reset_prefix_counters);\n";

      if (!assm.instr_seps().empty()) {
        o << "adl_setup_instr_separators(instr_separator_chars);\n";
      }

      if (relocs) {
        o << "adl_setup_relocations(" << _archpfx << "_elf_howto_table,num_" << _archpfx << "_elf_howtos,"
          << _archpfx << "_relocs_by_index,num_" << _archpfx << "_relocs_by_index,"
          << _archpfx << "_reloc_offsets,num_" << _archpfx << "_reloc_offsets);\n";
      }

      int count = 0;
      if (assm.asm_instrtables().empty()) {
        ForEach(_core.instrtables(),i) {
          const gc_string &n = Data::get_attr_name(i->first);
          o << "instr_hash[" << count << "] = adl_setup_instructions(" << n << "_instructions,num_" << n << "_instructions," << _archpfx << "_operands);\n";
          ++count;
        }
      } else {
        o << "instr_hash[0] = adl_setup_instructions(" << DefaultInstrTable << "_instructions,num_" << DefaultInstrTable << "_instructions," << _archpfx << "_operands);\n";
        if (_core.has_prefix_instruction()) {
          o << "instr_hash[1] = adl_setup_instructions(" << PrefixInstrTable << "_instructions,num_" << PrefixInstrTable << "_instructions," << _archpfx << "_operands);\n";
        }
      }
      o << "reg_hash = adl_setup_name_hash(" << _archpfx << "_regnames,num_" << _archpfx << "_regnames,\"register\");\n";

      if (_num_asm_instrs) {
        o << "asm_instr_hash = adl_setup_asm_instructions(asm_instrs,num_asm_instrs);\n";
      } else {
        o << "asm_instr_hash = 0;\n";
      }

      if (!_instr_pfx_fields.empty()) {
        o << "instr_pfx_fields_hash = adl_setup_instr_pfx_fields(instr_pfx_fields,num_instr_pfx_fields);\n";
      } else {
        o << "instr_pfx_fields_hash = 0;\n";
      }

      // Mainly provides the ability for users to define a final setup hook.
      o << "adl_setup_finish();\n";
      
      o << "}\n\n"

        // The verion string.
        << "std::string adl_asm_version = \"" << version << "\";\n\n"

        // The assemble function is called for each instruction encountered.
        <<
        "extern \"C\" void md_assemble(char *str)\n"
        "{\n"
        "adl_assemble(str,instr_hash[" << getTableSelect() << "],asm_instr_hash,reg_hash,instr_pfx_fields_hash," << maxfields << "," << getTableSelect() << "," << _archpfx << "_itnames[" << getTableSelect() << "]);\n"
        "}\n\n"

        // Fixup function handles fixups.
        <<
        "extern \"C\" void md_apply_fix (fixS *fixP ,valueT *valP ,segT seg)\n"
        "{\n"
        "if (!acb_apply_fix(fixP,valP,seg)) {\n"
        // Final user-supplied callback to do anything extra with relocations.
        "adl_apply_fix(fixP,valP,seg);\n"
        "}\n"
        "}\n\n"

        // Parsing of options.  We then call a user supplied function so that extra options can be handled.
        <<
        "int adl_parse_option(int,char*);\n\n"
        "extern \"C\" int md_parse_option (int c,char *arg)\n"
        "{\n"
        "if (c == 'd') {\n"
        "show_warnings = TRUE;\n"
        "return 1;\n"
        << "} else if (c == 'm') {\n";
      bool first = true;
      count = 0;
      ForEach(_core.instrtables(),i) {
        gc_string n = Data::get_attr_name(i->first);
        if (n == DefaultInstrTable) {
          n = DefaultTableName;
        }
        pfx_sep(o," else ",first);
        o << "if (!strcmp(arg,\"" << n << "\")) {\n"
          << getTableSelect() << " = " << count << ";\n"
          << "return 1;\n"
          << "}\n";
        ++count;
      }
      o << "}\n"
        << "return adl_parse_option(c,arg);\n"
        "}\n\n"

        // Displaying of options.  A user supplied function is also called to handle any hand-written user options.
        <<
        "void adl_show_usage(FILE*);\n\n"
        "extern \"C\" void md_show_usage (FILE *stream ATTRIBUTE_UNUSED)\n"
        "{\n"
        "fprintf (stream,\n"
        "\"\\nInstruction-selection options:\\n\\n\"\n";
      // List instructiont-able flags, which are -m options.  For the standard table (other), use -mdefault.
      ForEach(_core.instrtables(),i) {
        const gc_string &n = Data::get_attr_name(i->first);
        if (n == DefaultInstrTable) {
          o << "\"-m" << DefaultTableName << "\t\tSelect the default instruction table.\\n\"\n";
        } else {
          o << "\"-m" << n << "\t\tSelect the '" << n << "' instruction table.\\n\"\n";
        }
      }
      o << "\"-d\t\t\tShow debugging warnings about undefined symbols.\\n\"\n";
      o << ");\n"
        << "adl_show_usage(stream);\n"
        <<  "}\n\n"
        // Generate adl_get_instr_names method used by devtech integration
        << "extern \"C\" void adl_get_instr_names(const char ***names, int *num_names)\n"
        << "{\n"
        << "*names = instr_names;\n"
        << "*num_names = num_instr_names;\n"
        << "};\n\n"
        // Generate adl_get_instr_ops method used by devtech integration
        << "extern \"C\" void adl_get_instr_ops(const struct adl_name_pair **names, int* num_names)\n"
        << "{\n"
        << "*names = " << _archpfx << "_regnames;\n"
        << "*num_names = num_" << _archpfx << "_regnames;\n"
        << "}\n\n"
        << "extern \"C\" bfd_boolean  dt_debug(void);\n\n"
        << "extern \"C\" bfd_boolean  dt_debug(void)\n"
        << "{\n"
        << "#ifdef _DEVTECH_DEBUG_\n"
        << "return TRUE;\n"
        << "#else\n"
        << "return FALSE;\n"
        << "#endif\n"
        << "}\n\n";
    }
  };

  // We dive down to the first leaf node to retrieve a core.
  const Core *getFirstCore(const SysItem *si)
  {
    if (const Core *c = dynamic_cast<const Core *>(si)) {
      // Found a core- we're done.
      return c;
    } else if (const System *s = dynamic_cast<const System *>(si)) {
      // We have a system, so use its first child.
      return getFirstCore(s->items().front().second);
    } else {
      // Shouldn't ever get here.
      RError("Found a SysItem object that I don't know how to handle:  " << si->name());
    }
  }

  // Opens the output file and initiates the model creation process.
  void genAsmDisasm(const Data &data,const string &archpfx,const string &archname,const string &archpname,
                    const string &input,const AnyOption &options)
  {
    if (options.getFlag("print-data")) {
      data.dump(cout,"");
      return;
    }

    // If the user has specified a core, use it, otherwise if only one exists,
    // then use it, otherwise pick the first.  If the first element is a system,
    // then dive down and find the first leaf node.
    const Core *core = 0;
    gc_string cname = options.getOption("core").c_str();
    if (cname.empty()) {
      if (!data.systems().empty()) {
        // If no cores, then is this a system?  If so, grab the system's first leaf node.
        core = getFirstCore(data.systems().begin()->second);
      } 
      else if (!data.cores().empty()) {
        // Default- take first core.
        core = data.cores().begin()->second;
      }
    } else {
      // Try to find it in the database.  Systems take priority.
      const System *s = data.get_sys(cname);
      if (s) {
        core = getFirstCore(s);
      } else if (!(core = data.get_core(cname))) {
        RError ("Unknown system or core specified:  " << cname);
      }
    }

    if (!core) {
      RError("No core was specified and one could not be found automatically.");
    }

    cout << "Core:  " << core->name() << "\n";

    bool create_dis = options.getFlag("dis",true);
    bool create_asm = options.getFlag("asm",true);
    bool create_dbg = options.getFlag("dbg",false);

    bool big_mem = options.getFlag("big-mem",false);

    gc_string log_usage = options.getOption("log-usage").c_str();

    string basename = gen_default_name(input,"").c_str();

    unsigned machine_code = options.getOption("emc",0);
    
    if (create_dis) {
      string disname = options.getOption("do","objdump-"+basename+".cc");
      ofstream dis(disname.c_str());
      if (!dis) {
        RError("Unable to open output file '" << disname << "'.");
      }

      // The line-count stream enables us to create line directives by
      // tracking our current line number.
      opfxstream disln(dis.rdbuf(),2,'{','}');

      string header_name = options.getOption("ho","instr-info-"+basename+".h");
      ofstream hdr;      
      unique_ptr<opfxstream> hdrln;
      if (create_dbg) {
        hdr.open(header_name.c_str());
        if (!hdr) {
          RError("Unable to open output header file '" << header_name << "'.");
        }
        hdrln.reset(new opfxstream(hdr.rdbuf(),2,'{','}'));
      }

      GenerateDisassemblerInt gendis(*core,false,true,true,false,disname.c_str(),log_usage,0);
      gendis._archpfx         = archpfx.c_str();
      gendis._arch_name       = archname.c_str();
      gendis._arch_print_name = archpname.c_str();
      gendis._machine_code    = machine_code;

      // Create the disassembler.  Note: We strip directory information from the
      // header, since we create those files in the same output directory.
      gendis.exec(disln,hdrln.get(),base_name(header_name).c_str());
    }

    if (create_asm) {
      string asmname = options.getOption("ao","as-"+basename+".cc");
      string asm_ver = options.getOption("asm-ver");
      ofstream asml(asmname.c_str());

      if (!asml) {
        RError("Unable to open output file '" << asmname << "'.");
      }

      StrSet reloc_extras;
      auto &re = options.getValues("reloc-extra");
      for (auto &i : re) {
        reloc_extras.insert(i.c_str());
      }      
      
      // The line-count stream enables us to create line directives by
      // tracking our current line number.
      opfxstream asmln(asml.rdbuf(),2,'{','}');
      
      GenerateAssemblerInt genasm(*core,big_mem,asmname.c_str(),log_usage,asm_ver.c_str(),reloc_extras);
      genasm._archpfx = archpfx.c_str();
      genasm._keep_ws = options.getFlag("keep-ws");
        
      genasm.exec(asmln);
    }

  }

  GenDisassembler::GenDisassembler(const Core &core,bool use_cpp,bool name_ws,bool gen_pfx_logic,bool library_mode,
                                   const gc_string &fname,const gc_string &log_usage,DisassemblerCallIF *dc) :
    _dis(new GenerateDisassemblerInt(core,use_cpp,name_ws,gen_pfx_logic,library_mode,fname,log_usage,dc))
  {
  }

  void GenDisassembler::writeDisTableEntry(ostream &o,const Instr &instr,const InstrList &shorthands)
  {
    _dis->writeDisTableEntry(o,instr,shorthands);
  }

  void GenDisassembler::writeDisTableEntryAccessors(ostream &o,const Instr &instr,const InstrList &shorthands)
  {
    _dis->writeDisTableEntryAccessors(o,instr,shorthands);
  }

  void GenDisassembler::exec(ostream &o,std::ostream *h,const gc_string &header_name)
  {
    _dis->exec(o,h,header_name);
  }

  GenAssembler::GenAssembler(const Core &core,const gc_string &fname,const gc_string &log_usage) :
    _asm(new GenerateAssemblerInt(core,false,fname,log_usage,gc_string(),StrSet()))
  {
  }

  void GenAssembler::exec(ostream &o)
  {
    _asm->exec(o);
  }
  bool GenerateAssemblerInt::FInfo::_explicit_regs = false;

}

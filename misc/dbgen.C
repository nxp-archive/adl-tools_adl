//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

// Entry-point for the db generation client.  In general, the database is some
// sort of a textual representation of static data structures- either a list or
// a key/value associative structure, along with scalar elements (integers,
// strings, and booleans).
// 
// IMPORTANT: tag_start with map_start or list_start does not requier tag_end, 
// i.e.these are all valid:
// tag_start(..), ... . tag_end() 
// tag_start(..),map_start(),  ... . map_end() 
// tag_start(..),list_start(),  ... , list_end() 
//
// All this does not affect perl but XML is broken when one does not follow those rules.

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <ctime>

#include "opencxx/parser/Ptree.h"

#include "helpers/Macros.h"
#include "helpers/CodeGen.h"
#include "helpers/AnyOption.h"
#include "helpers/LogUsage.h"
#include "helpers/StringUtils.h"

#include "fe/AdlParser.h"
#include "fe/Data.h"
#include "fe/Helpers.h"
#include "fe/FieldSupport.h"

#include "DocHelpers.h"
#include "DocCfgParser.h"

using namespace std;
using namespace Opencxx;
using namespace PtreeUtil;

//#define DEBUG
#ifdef DEBUG
#define DBG(x) _o << "\n<!-- " << x << " -->\n";
#else
#define DBG(x)
#endif

#include "dbgen.h"


namespace adl {

  //
  // Configuration Globals.
  //
  bool PrintDate       = false;
  bool RegUsageFields  = false;
  bool ExpandExprs     = false;
  bool UsedOnly        = false;
  bool OrigAliasFormat = false;

# define AddString(x) { gc_ostringstream ss; ss << x; string_add (ss.str ()); }
 
# define AddTaggedInt(name,val)  { tag_start((name)); int_add((val));    tag_end(); };

# define AddTaggedStr(name,val)  { tag_start((name)); string_add((val)); tag_end(); };

# define AddTaggedBool(name,val) { tag_start((name)); bool_add((val));   tag_end(); };

# define AddTaggedFunc(name,val) { tag_start((name)); addfunc((val));   tag_end(); };

# define TagEnd(n) { while(n-- >0) tag_end(); };

  Opencxx::Ptree * GenerateCoreDB::get_func_contents (Opencxx::Ptree * value) {
    return get_func_body (value);
  }

  void GenerateCoreDB::genDocString (const gc_string & doc) {
    if (!doc.empty()) {
      tag_start ("doc");
      bigstring_add (doc);
      tag_end();
    }
  }

  void GenerateCoreDB::addexpr (Ptree * p) {
    gc_ostringstream ss;
    ss << p;
    bigstring_add (ss.str());
  }

  void GenerateCoreDB::addfunc (Ptree * p) {
    gc_ostringstream ss;
    ss << get_func_contents(p);
    bigstring_add (ss.str());
  }

  void GenerateCoreDB::addfunc (const HelperInfo &hi) {
    addfunc(hi._func);
  }
    
  void GenerateCoreDB::genBits(const BitPairVect &bits) {
    if (!bits.empty()) {
      tag_start("bits");
      list_start(); 
      ForEach(bits,b) {
        sublist_start("range");
        int_add(b->l());
        int_add(b->r());
        list_end();
      }
      list_end();
    }
  }

  void GenerateCoreDB::genEnums(const StrIntList &enums) {

    tag_start("enumerated");
    map_start();

    ForEach(enums,i) {
      tag_start("option",uitogs(i->second));
      string_add(i->first);
      tag_end();
    }
    map_end();
  }

  void GenerateCoreDB::genTable(const InstrField *f) {
    if (f->table().empty()) {
      return;
    }
    tag_start("table");
    map_start();
    ForRange(f->table().entries().size(),i) {
      auto entry = f->table().entries().at(i);
      if ((int)entry.at(0) == -1) {
        tag_start("entry",uitogs(i));
        string_add("reserved");
        tag_end();
      } else {
        tag_start("entry",uitogs(i));
        list_start();
        ForRange(entry.size(),ii) {
          int_add(entry.at(ii));
        }
        list_end();
      }
    }
    map_end();
  }

  void GenerateCoreDB::genRanges(const InstrField *f) {
    if (f->ranges().empty()) {
      return;
    }
    tag_start("valid_ranges");
    list_start();
    ForEach(f->ranges(),i) {
      sublist_start("range");
      int_add(i->first);
      int_add(i->second);
      list_end();
    }  
    list_end();
  }

  void GenerateCoreDB::genMasks(const InstrField *f) {
    if (f->masks().empty()) {
      return;
    }
    tag_start("valid_masks");
    list_start();
    ForEach(f->masks(),i) {
      sublist_start("mask");
      int_add(i->first);
      int_add(i->second);
      list_end();
    }  
    list_end();

  }

  void GenerateCoreDB::genRef(const InstrField *f) {

    switch (f->type()) {
    case InstrField::tRegFile:
      if (f->regfile()) {
        AddTaggedStr("ref",f->regfile()->name());
      }
      break;
    case InstrField::tMem:
      if (f->memory()) {
        AddTaggedStr("ref",f->memory()->name());
      }
      break;
    case InstrField::tInstr:    //
      if (f->instrtable()) {
        AddTaggedStr("ref",f->instrtablename());
      }
      break;
    case InstrField::tImm:
    case InstrField::tRefField:
    default:
      ;
    }

  }


  // Generate instruction field documentation.
  void GenerateCoreDB::genInstrFields () 
  {
    if (_core.instrfields().empty()) {
      return;
    }

    if (UsedOnly && _used_instrfields.empty()) {
      return;
    }

    auto ifields = _core.instrfields();

    tag_start("instrfields");
    map_start();
    ForEach(ifields,i) {
      auto field = (i->second);
      if (field->encap_field()) {
        continue;
      }
      if (UsedOnly && !_used_instrfields.count(field)) {
        continue;
      }

      tag_start ("instrfield", field->name());
      map_start(); 

      genDocString(field->doc());

      genInstrFieldAsm(field);

      genAttrs(*field);

      if (field->is_alias()) {

        AddTaggedStr("alias",field->get_alias()->_target_name);

      } else {

        genBits(field->bits());

        if (field->is_pseudo()) {
          AddTaggedBool("pseudo",true);
        }

        if (field->is_prefix()) {
          AddTaggedBool("prefix",true);
        }

        AddTaggedInt("width",field->width());

        AddTaggedInt("size",field->size());

        AddTaggedInt("shift",field->shift());

        AddTaggedInt("offset",field->offset());

        if (field->index()) {
          AddTaggedInt("indexed",field->index());
        }

        tag_start("mask");
        AddString("0x" << std::hex << field->mask());
        tag_end();

        if (!field->blks().empty()) {
          tag_start("blocks");
          list_start(); {
            ForEach(field->blks(),b) {
              string_add(*b);
            }
          }
          list_end();
        } 

        genTable(field);

        if (field->is_enumerated()) {
          genEnums(field->enums());
        }

        genAddr(field);

        genDisplay(field);

        genType(field);

        if (field->is_overlay()) {
          AddTaggedBool("overlay",true);
        }

        if (field->is_signed()) {
          AddTaggedBool("signed",true);
        }

        if (field->is_inverted()) {
          AddTaggedBool("inverted",true);
        }

        if (field->is_assembler()) {
          AddTaggedBool("assembler",true);
        }
        
        if (field->is_unsigned_upper_bound()) {       //
          AddTaggedBool("unsigned_upper_bound",true);
        }
	
        if (field->action()) {
          tag_start("action");
          addfunc(field->action());
          tag_end(); 
        }

        if (field->has_default_value()) {
          AddTaggedInt("value",field->default_value());
        }

        if (field->interface()) {
          AddTaggedStr("implements",field->interface()->name());
        } 	

        if (!field->relocs().empty()) {
          AddTaggedStr("reloc",field->relocs().front()->name());
        }

        genRanges(field);

        genMasks(field);

        genRef(field);

        if(field->instrtable()) {   //
            AddTaggedStr("instrtable",field->instrtablename());
        }
        
        genFieldsDescription(field);

        genInstrFieldsDescription(field);
      }

      map_end();
    }
    map_end();
  }

  void GenerateCoreDB::genInstrFieldsDescription(const InstrField *field) {

    if (field->instrfields().empty()) {
      return;
    }

    tag_start("instrfields");
    map_start();

    ForEach(field->instrfields(),i) {
      const InstrField *f = i->second;
      tag_start("instrfield",(f->name()));
      map_start();
      AddTaggedInt("size",f->size());
      genFieldsDescription(f);
      genBits(f->bits());
      genTable(f);
      map_end();
    }
    map_end();
  }

  void GenerateCoreDB::genType(const InstrField *f) {
    switch (f->type()) {
    case InstrField::tRegFile:
      AddTaggedStr("type","regfile");
      break;
    case InstrField::tMem:
      AddTaggedStr("type","mem");
      break;
    case InstrField::tImm:
      AddTaggedStr("type","imm");
      break;
    case InstrField::tInstr:          //
      AddTaggedStr("type","instr");
      break;      
    case InstrField::tRefField:
      AddTaggedStr("type","regfield");
      break;
    default:
      ;
    }
  }

  void GenerateCoreDB::genDisplay(const InstrField *f) {
    switch (f->display()) {
    case InstrField::Default:
      break;
    case InstrField::Hex:
      AddTaggedStr("display","hex");
      break;
    case InstrField::Dec:
      AddTaggedStr("display","dec");
      break;
    case InstrField::Name:
      AddTaggedStr("display","name");
      break;
    default:
      ;
    }
  }

  void GenerateCoreDB::genAddr(const InstrField *f) {
    switch (f->addr()) {
    case InstrField::aNone:
      break;
    case InstrField::aAbs:
      AddTaggedStr("addr","abs");
      break;
    case InstrField::aPcRel:
      AddTaggedStr("addr","pc");
      break;
    default:
      ;
    }
  }

  void GenerateCoreDB::genFieldsDescription(const InstrField *f) {
    if (f->fields().size() == 0) {
      return;
    }
    auto fields = f->fields();
    tag_start("fields");
    list_start();
    ForRange(fields.size(),i) {
      InstrField::Field f = fields[i];
      switch (f._type) {
      case InstrField::Field::Ref: 
        string_add(f._field->name());
        break;
      case InstrField::Field::Range:
        list_start();
        int_add(f._range.l());
        int_add(f._range.r());
        list_end();
        break;
      case InstrField::Field::TblIdx:
      case InstrField::Field::RefIdx:
        map_start();
        tag_start("field",f._field->name()); 
        int_add(f._value);
        tag_end();
        map_end();
        break;
      }
    }
    list_end();
  }

  bool GenerateCoreDB::hasPrintFields(const Instr *instr) const 
  {
    if (instr->is_alias()) {
      // We need to check that alias does not change opcode, only extends it.
      ForEach(instr->get_aliases(),a_iter) {
        const Instr::Alias &alias = **a_iter;
        const Instr *target = alias._target;
        ForRange(instr->sizeOpcs(),i) {
          Instr::Field f = instr->getOpc(i);
          if (f._type == Instr::Field::Value || f._type == Instr::Field::RValue) {
            // Let's check that the values is the same.
            // We check only opcs, becaus alias can set some values to non-opcode fields
            ForRange(target->sizeOpcs(),j) {
              Instr::Field tf = target->getOpc(j);
              if (!strcmp(f._name,tf._name)) {
                if (f._value != tf._value) {
                  return true;
                }
              }
            }
          }
        }
      }
      return false;
    } else {
      return true;
    }
  }

  void GenerateCoreDB::genInstrFieldDescription (const InstrBase *instr) {
    tag_start("fields");
    map_start();
    ForRange(instr->sizeAll(),i) {
      Instr::Field f = instr->getAll(i);

      switch (f._type) {
      case Instr::Field::Ref:
        tag_start ("field", f._name);
        string_add ("");
        tag_end ();
        break;
      case Instr::Field::RValue:
        tag_start ("field", f._name);
        int_add (f._value);
        tag_end ();
        break;
      case Instr::Field::Value:
        tag_start ("field", f._name);
        int_add(f._value);
        tag_end ();
        break;
      case Instr::Field::Expr:
        tag_start ("field", f._name);
        AddString(f._expr);
        tag_end ();
        break; 
      case Instr::Field::Resrvd:
        tag_start ("field", f._name);
        string_add("reserved");
        tag_end ();
        break;
      default:;
      }
    }
    DBG (__LINE__);
    map_end();
  }

  void GenerateCoreDB::genRegUsage(const gc_string &name,const UsedRegs &ur)
  {
    if (RegUsageFields) {
      if (!ur.empty()) {
        tag_start(name);
        list_start();
        ForEach(ur,i) {
          const UsedReg &r = *i;
          gc_ostringstream ss;
          StrVect fieldNames;
          if (!getUsedFields(fieldNames,r,_core.is_little_endian(),0)) {
            // No set of fields covered the mask completely, so just print the
            // mask.
            ss << r;
            string_add(ss.str());
          } else {
            // We have fields, so add each element as a separate entry.
            ForEach(fieldNames,rf) {
              ss.str("");
              ss << r.regb()->name();
              if ( r._begin ) {
                ss << "(" << r._begin;
                if ( r._end) {
                  ss << "," << r._end;
                }
                ss << ")";
              }
              ss << "." << *rf;
              if (r.partial()) {
                ss << "/p";
              }
              if (r.delayed()) {
                ss << "/d";
              }
              if (r.direct()) {
                ss << "/D";
              }
              if (r.conditional()) {
                ss << '?';
              }
              string_add(ss.str());
            }
          }
        }
        list_end();
      }
    } else {
      // Don't bother trying to find field names, so just print the data
      // directly.
      genStrList(name,ur);
    }
  }
 
  void GenerateCoreDB::getHookInfoTopLevel(UsageInfo &ui,const UsageInfo &src)
  {
    RegSet handled_src,handled_trg;
    getHookInfo(ui,src._src_regs,handled_src,handled_trg,true,true);
    getHookInfo(ui,src._trg_regs,handled_src,handled_trg,false,true);
  }

  void GenerateCoreDB::getHookInfo(UsageInfo &ui,const UsageInfo &src,
                                   RegSet &handled_src,RegSet &handled_trg)
  {
    getHookInfo(ui,src._src_regs,handled_src,handled_trg,true,false);
    getHookInfo(ui,src._trg_regs,handled_src,handled_trg,false,false);
  }

  void update_ui(UsageInfo &trg_ui,const UsageInfo &src_ui,bool is_src,bool top_level,
                        const RegBase *orig)
  {
    if (top_level && orig) {
      // At the top-level, so we want to remove a reference to the original item
      // so that it doesn't interfere.  For example, if we're dealing with
      // register X and that has a write-hook which writes all of X, then we
      // remove X so that we don't lose the mask information from the original
      // write to X.
      UsageInfo tmp_ui(src_ui);
      UsedRegs *tmp_ur = (is_src) ? &tmp_ui._src_regs : &tmp_ui._trg_regs;
      IfFind(x,(*tmp_ur),UsedReg(orig)) {
        tmp_ur->erase(x);
      }
      trg_ui.update(tmp_ui);
    } else {
      trg_ui.update(src_ui);
    }
  }

  void GenerateCoreDB::getHookInfo(UsageInfo &ui,const UsedRegs &src,
                                   RegSet &handled_src,RegSet &handled_trg,
                                   bool is_src,bool top_level)
  {
    ForEach(src,s_iter) {
      const UsedReg &ur = *s_iter;

      if (!ur.regb() || ( (is_src) ? handled_src.count(ur.regb()) : handled_trg.count(ur.regb())) ) continue;

      // To prevent a recursion issue, we mark any registers as handled, then
      // skip if it's already been processed.
      if (is_src) {
        handled_src.insert(ur.regb());
      } else {
        handled_trg.insert(ur.regb());
      }

      if (auto r = ur.reg()) {
        if (is_src && r->readhook()) {
          update_ui(ui,r->readhook()->usageinfo(),is_src,top_level,ur.regb());
          getHookInfo(ui,r->readhook()->usageinfo(),handled_src,handled_trg);
        }
        if (!is_src && r->writehook()) {
          update_ui(ui,r->writehook()->usageinfo(),is_src,top_level,ur.regb());
          getHookInfo(ui,r->writehook()->usageinfo(),handled_src,handled_trg);
        }
      } else if (auto r = ur.regfile()) {
        if (is_src && r->readhook()) {
          update_ui(ui,r->readhook()->usageinfo(),is_src,top_level,ur.regb());
          getHookInfo(ui,r->readhook()->usageinfo(),handled_src,handled_trg);
        }
        if (!is_src && r->writehook()) {
          update_ui(ui,r->writehook()->usageinfo(),is_src,top_level,ur.regb());
          getHookInfo(ui,r->writehook()->usageinfo(),handled_src,handled_trg);
        }
      }
    }
  }
   
  void GenerateCoreDB::genUsageInfo(const UsageInfo &ui) 
  {
    // For the database, we want to include any resources accessed via register
    // hooks.  Thus, we have to merge in any usage information from the src and
    // target registers before we report.
    UsageInfo trg_ui = ui;
    getHookInfoTopLevel(trg_ui,ui);

    genRegUsage("inputs",trg_ui._src_regs);
    genRegUsage("outputs",trg_ui._trg_regs);
    genStrList("input_mems",trg_ui._src_mems);
    genStrList("output_mems",trg_ui._trg_mems);
    genStrList("input_caches",trg_ui._src_caches);
    genStrList("output_cashes",trg_ui._trg_caches);
    genStrList("helpers",trg_ui._helpers);
    if (trg_ui._exceptions) {
      AddTaggedBool("raises_exception",true);
    }
  }

  // Generates inputs and outputs of instruction ( sources and targets)
  void GenerateCoreDB::genUsageInfo (const InstrBase & instr) 
  {
    auto const &ui = instr.usageinfo();
    genUsageInfo(ui);
  }

  // Sanity check: If the item is a field and isn't a field which exists in the
  // instruction, then return true.
  bool GenerateCoreDB::unknown_field(const Instr &instr,Ptree *x)
  {
    if (!x) return false;

    gc_string s = x->ToString();
    
    if (_core.get_instrfield(s)) {
      if ( instr.getField(s) < 0) {
        return true;
      }
    }
    return false;
  }

  // Generate inputs and outputs of instructions.  If for an alias, then
  // substitute with the alias's input fields.
  void GenerateCoreDB::genUsageInfo (const Instr &instr,bool is_alias)
  {
    if (is_alias) {

      const UsageInfo &orig_ui = instr.usageinfo();
      UsageInfo final_ui(orig_ui);
      final_ui._src_regs.clear();
      final_ui._trg_regs.clear();

      bool mts = instr.multitarget_shorthand();

      // For each alias, substitute its values into the instruction's usage,
      // then merge with the overall usageinfo item.
      ForEach(instr.get_aliases(),a_iter) {
        const Instr::Alias *alias = *a_iter;

        UsageInfo tmp_ui;
        PPairList subst;
        IntMap parms;

        getAliasSubstitutions(subst,parms,alias);
        
        ForEach(orig_ui._src_regs,i) {
          UsedReg ur = *i;
          ur._begin = replaceAllEqual(ur._begin,subst);
          ur._end = replaceAllEqual(ur._end,subst);
          if (!mts || !unknown_field(instr,ur._begin)) {
            tmp_ui._src_regs.insert(ur);
          }
        }
        ForEach(orig_ui._trg_regs,i) {
          UsedReg ur = *i;
          ur._begin = replaceAllEqual(ur._begin,subst);
          ur._end = replaceAllEqual(ur._end,subst);
          if (!mts || !unknown_field(instr,ur._begin)) {
            tmp_ui._trg_regs.insert(ur);
          }
        }
        final_ui.update(tmp_ui);
      }
      genUsageInfo(final_ui);
    } else {
      genUsageInfo(instr);
    }
  }


  // Generates attributes
  void GenerateCoreDB::genAttrs (const Attrs & attrs) {

    if (!attrs.attrs().empty()) {
      tag_start("attributes");
      map_start();
      ForEach(attrs.attrs(),i) {
        const gc_string &name = Data::get_attr_name(i->first);
        const AttrParams &params = (i->second);
        tag_start("attribute",name);
        switch (params._type) { 
        case AttrParams::NoParam:
          string_add("");
          break;
        case AttrParams::Int:
          int_add(*(params._int_list.begin()));
          break;
        case AttrParams::Str:
          string_add(*(params._str_list.begin()));
          break;
        case AttrParams::IntList:
          sublist_start("intlist");        
          ForEach(params._int_list,ii) {
            int_add(*ii);
          }
          list_end();
          break;
        case AttrParams::StrList:
          sublist_start("strlist");        
          ForEach(params._str_list,ii) {
            string_add(*ii);
          }
          list_end();
          break;
        default:
          ;
        }
        tag_end();
      }
      map_end();
    }
  }

  void GenerateCoreDB::genRelocations() 
  {
    if (_core.relocs().empty()) {
      return;
    }
    if (UsedOnly && _used_relocs.empty()) {
      return;
    }

    tag_start("relocations");
    map_start();
    ForEach(_core.relocs(),i) {
      const Relocation &reloc = *(i->second);

      if (UsedOnly && !_used_relocs.count(&reloc)) {
        continue;
      }

      tag_start("reloc",reloc.name());
      map_start();

      genAttrs(reloc);

      genDocString(reloc.doc());
      
      AddTaggedStr("abbrev",reloc.abbrev());
      AddTaggedInt("field_width",reloc.field_width());
      AddTaggedBool("pcrel",reloc.pcrel());

      if (reloc.value() != -1) {
        AddTaggedInt("value",reloc.value());
        if (reloc.right_shift() != -1) {        
          AddTaggedInt("right_shift",reloc.right_shift()); // Specifies the number of bits the relocation value is right-shifted before it is encoded
        }
        if (!reloc.right_shift_exp().empty()) {        
          AddTaggedStr("right_shift_exp",reloc.right_shift_exp()); // Specifies the expression for the number of bits the relocation value is right-shifted before it is encoded
        }        
      }
      if (reloc.width() !=  -1) {
        AddTaggedInt("width",reloc.width());
      }     

      if (reloc.width() !=  -1 && reloc.offset() != -1) {
        AddTaggedInt("offset",reloc.offset());
      }       
      
      map_end();
    }
    map_end();
  }

  void GenerateCoreDB::genGroups () {
    if (!_core.groups().empty()) {
      tag_start("groups");
      list_start();
      ForEach(_core.groups(),i) {
        const Group &grp = *i->second;
        tag_start("group", i->first);
        map_start();
        genDocString(grp.doc());
        tag_start("type");
        string_add(grp.type());
        tag_end();
        if (!grp.items().empty()) {
          tag_start("items");
          list_start();
          ForEach(grp.items(),ii) {
            string_add(*ii);       
          }       
          list_end();
        }
        genAttrs(grp);
        map_end();      
      }      
      list_end();      
    }    
  }

  void GenerateCoreDB::genInstrFieldAsm (const InstrField *f) {
    if (const InstrField::Syntax *syntax = f->syntax()) {
      gc_ostringstream ss;
      ForEach(syntax->_fmt,i) {
        if (syntax->isfield(*i)) {
          if (syntax->get(*i)._name) {
            ss << syntax->get(*i)._name;
          }
        } else {
          ss << *i;
        }
      }
      AddTaggedStr("syntax",ss.str());
    }
  }

  // Output an assembler format for an instruction.
  void GenerateCoreDB::genInstrAsm (const InstrBase & instr)
  {
    gc_ostringstream ss, dss;

    // Generate a dsyntax string as well, then override it if the user supplied
    // an explicit one.  For the dsyntax string, we explicitly denote fields
    // using ${} syntax.
    
    if (const Instr::Syntax *syntax = instr.syntax()) {
      ForEach(syntax->_fmt,i) {
        if (syntax->isfield(*i)) {
          ss << syntax->get(*i);
          dss << "${" << syntax->get(*i) << "}";
        } else {
          ss << *i;
          dss << *i;
        }
      }
    } 
		else {
      ss << instr.name();
      dss << instr.name();
      if (instr.sizeFields()) {
        ss << ' ';
        dss << ' ';
      }
      bool first1 = true, first2 = true;
      ForRange(instr.sizeFields(),i) {
        const InstrField *f = instr.getField(i)._field;
        if (f->is_reserved()) continue;
        if (f->interface()) f = f->interface();
        
        list_sep(ss,first1);
        list_sep(dss,first2);
        ss << f->name();
        dss << "${" << f->name() << "}";
      }
    }
    tag_start("syntax");
    string_add(ss.str());
    tag_end();
		
		if (const Instr::Syntax *syntax = instr.dsyntax()) {	// If there is dsyntax, show it.
      dss.str(gc_string());
      ForEach(syntax->_fmt,i) {
        if (syntax->isfield(*i)) {
          dss << "${" << syntax->get(*i) << "}";
        } else {
          dss << *i;
        }
      }		
		}

    tag_start("dsyntax");
    string_add(dss.str());
    tag_end();
  }

  void GenerateCoreDB::genInstrBlock(const InstrBase *instr)
  {
    if (!instr->get_blk().empty()) {
      tag_start("block");
      string_add(instr->get_blk());
      tag_end();
    }
  }

  void GenerateCoreDB::genInstrBase(const InstrBase *instr) 
  {
    genDocString(instr->doc());
    genInstrAsm(*instr);
    genInstrBlock(instr);
    genAttrs(*instr);
  }

  bool GenerateCoreDB::expandExprs(const Instr *instr) const
  {
    return (ExpandExprs && !(instr && instr->has_attr(_no_expand_attr)));
  }

  pair<Ptree *,bool> GenerateCoreDB::genInstrAction(const Instr *instr,Ptree *action,const gc_string &action_name)
  {
    if (action) {
      Ptree *action2 = _walker.Translate(action);
      tag_start (action_name); 
      addfunc(action2);
      tag_end ();
      Ptree *op = _walker.doc_op();
      if (!op && expandExprs(instr)) {
        op = action2;
      }
      return make_pair(op,!_walker.has_ctrl_stmts());
    }
    return make_pair((Ptree*)0,false);
  }
   
  pair<Ptree *,bool> GenerateCoreDB::genInstrActions(const Instr *instr) 
  {
    if (!instr->is_pseudo()) {
      auto p = genInstrAction(instr,instr->action(),"action");
      if (instr->assembler()) {
        tag_start ("assembler"); 
        addfunc(instr->assembler());
        tag_end ();
      }
      if (instr->alias_action()) {
        tag_start ("alias_action"); 
        addfunc(instr->alias_action());
        tag_end ();
      }      
      return p;
    } else {
      tag_start("pseudo"); 
      bool_add(true);
      tag_end();
      return make_pair((Ptree*)0,false);
    }
  }

  pair<Ptree *,bool> GenerateCoreDB::genInstrParentAction(const Instr *instr,const Instr::Alias *alias)
  {
    assert(alias);
    auto trg = alias->_target;
    assert(trg);

    Ptree *paction = replaceAliasParms(trg->action(),_walker,alias);

    return genInstrAction(instr,paction,"parent_action");
  }

  void GenerateCoreDB::genInstrWidth(const Instr *instr) {
    // An outer instruction does not have width, nor does a shorthand with multiple targets.
    if (instr->get_nested().empty() && !instr->multitarget_shorthand()) {
      tag_start("width");
      int_add(instr->width());
      tag_end();

      if (instr->width() != instr->fetch_width()) {
        tag_start("fetch_width");
        int_add(instr->fetch_width());
        tag_end();
      }
    }
  }

  // This tries to find a given field within the specified usage information,
  // and if found, starts off the printing of a source or destination map.
  bool GenerateCoreDB::genFieldSrcTgt(const char *fname,const UsedRegs &used_regs,gc_string type)
  {
    bool found = false;
    ForEach(used_regs,i) {
      const UsedReg  &ur = *i;
      if (ur._begin && Eq(ur._begin,fname)) {
        tag_start(type);  
        map_start();
        found = true;
        if (ur._cond) {
          AddTaggedBool("conditional",true);
        }
        break;
      }
    }
    return found;
  }
  

  void GenerateCoreDB::addAliasFieldTarget(const Instr::Field &f)
  {
    switch(f._type) {
    case InstrBase::Field::Ref:
      AddTaggedStr("value",f._sname);
      break;
    case InstrBase::Field::RValue:
      AddTaggedStr("value",f._field->name());
      break;
    case InstrBase::Field::Value:
      AddTaggedInt("value",f._value);
      break;
    case InstrBase::Field::Expr:
      AddTaggedStr("value",f._expr->ToString());
      break;
    default:
      AddTaggedStr("field",f._field->name());
    }
  }

  // Anything for which we couldn't determine a source or target is listed here.
  void GenerateCoreDB::genAliasMiscFields(const Instr::Fields &fields, const StrSet &dumped)
  {
    bool first = true;
    ForEach(fields,f) {
      if (dumped.find(f->_name) == dumped.end()) {
        if (first) {
          tag_start("miscs");
          list_start();
          first = false;
        }
        tag_start("misc");
        map_start();
        AddTaggedStr("field",f->_name);
        addAliasFieldTarget(*f);
        map_end();
      }
    }
    if (!first) {
      list_end();
    }
  }

  // For an alias, lists sources or destinations.  We look at the supplied
  // UsedRegs data to try and find the relevant field in order to know if it's a
  // source or target.  Thus, this usage information should be from the alias's
  // target, since the alias's own usage information might not contain these
  // fields, such as if the field is mapped to a constant.
  void GenerateCoreDB::genAliasSrcTgtFields(const Instr::Fields &fields, const UsedRegs &used_regs, gc_string type, StrSet &dumped)
  {
    gc_string types = type + "s";
    tag_start(types);
    list_start();
    ForEach(fields,f) {
      if (!genFieldSrcTgt(f->_name,used_regs,type)) {
        continue;
      }
      AddTaggedStr("field",f->_name);
      dumped.insert(f->_name);
      addAliasFieldTarget(*f);
      map_end();
    }
    list_end();
  }


  void GenerateCoreDB::genInstrAlias(const Instr *instr,bool orig_format) 
  {
    const UsageInfo &ui = instr->usageinfo();
    ForEach(instr->get_aliases(),a_iter) {
      const Instr::Alias &alias = **a_iter;

      if (orig_format) {
        tag_start("alias");
        map_start();
        AddTaggedStr("target",alias._target->name());
      } else {
        tag_start("alias",alias._target->name());
        map_start();
      }

      StrSet dumped;
      genAliasSrcTgtFields(alias._fields,ui._src_regs,"source",dumped);
      genAliasSrcTgtFields(alias._fields,ui._trg_regs,"destination",dumped);
      genAliasMiscFields(alias._fields,dumped);
      
      if (!orig_format) {
        auto op = genInstrParentAction(instr,&alias);
        genExtractedOp(instr,op.first,op.second);
      }
      
      map_end();
    }
  }

   
  void GenerateCoreDB::genInstrAsmRules(const Instr *instr) {
    if (!instr->asmrules().empty()) {
      tag_start("asmrules");
      list_start();
      ForEach(instr->asmrules(),r){
        string_add(*r);
      }
      list_end();
    }
  }


  void GenerateCoreDB::genExtractedOp(const Instr *instr,Ptree *op,bool expand_exprs)
  {
    if (op) {
      tag_start("extracted-doc-op");
      Ptree *op2 = extractBlockBody(doOpTransforms(op,expandExprs(instr) && expand_exprs,&_walker,_doccfg));
      addexpr(op2);
      tag_end();
    }
  }

  void GenerateCoreDB::genInstr(const Instr *instr, const gc_string &parent) 
  {
    _walker.reset();

    tag_start("instruction",instr->name());
    map_start();
    genInstrWidth(instr);
    genInstrBase(instr);
    if (hasPrintFields(instr) || !parent.empty()) {  
      genInstrFieldDescription(instr);
    }

    pair<Ptree *,bool> op(reinterpret_cast<Ptree*>(0),false);
    if (instr->is_alias()) {
      if (instr->is_complex_alias()) {
        tag_start("alias_action");
        addfunc(instr->alias_action());
        tag_end();
      }
      else if (OrigAliasFormat && (instr->get_aliases().size() == 1)) {
        // Yuck- StarCore wants the original format for single-target aliases
        // (the format that we had before adding support for multiple targets
        // for shorthands).  I'm making it a flag so that we can just remove it
        // altogether when they don't need it anymore.
        genInstrAlias(instr,true);
        op = genInstrParentAction(instr,instr->get_alias());
      } else {
        tag_start("aliases");
        map_start();
        genInstrAlias(instr,false);
        map_end();
      }
    } else {
      op = genInstrActions(instr);
    }

    if (!parent.empty()) {
      tag_start("parent");
      string_add(parent);
      tag_end();
    }

    if (instr->is_prefix()) {
      tag_start("prefix");
      bool_add(true);
      tag_end();
    }

    AddTaggedBool("disassemble",instr->disassemble());

    if (op.first) {
      genExtractedOp(instr,op.first,op.second);
    }

    if (!instr->doc_op().empty()) {
      tag_start ("doc-op");
      bigstring_add (instr->doc_op());
      tag_end();
    }
     
    if (const Instr *type = instr->get_type()) {
      tag_start("type");
      string_add(type->name());
      tag_end();
    }

    genInstrAsmRules(instr);

    genUsageInfo(*instr,instr->is_alias());
    map_end();
  }

# if 0
  // Not used at the moment, may be will need later
  void GenerateCoreDB::GenNestedInstr(const Instr *outer,const Instr *inner) {
    tag_start("instruction",inner->name());
    map_start();
    genInstrWidth(inner);
    genInstrBase(inner);
    genInstrFieldDescription(inner);
    genInstrAction(outer);
    genInstr(outer);
    map_end();
  }
# endif

  bool GenerateCoreDB::skip_instr(const Instr *instr) {
    if (instr->is_simple_alias()) {
      auto tgt = instr->get_alias()->_target;
      return !tgt->get_nested().empty();
    } else {
      return false;
    }
  }
   
  // Generate instruction documentation.
  void GenerateCoreDB::genInstrs () {
    if (_core.instrs().empty()) {
      return;
    }

    tag_start("instrs");
    map_start();

    ForEach(_core.instrs(),i) {
      auto instr = (i->second);
      if (skip_instr(instr)) {
        continue;
      } 
      genInstr(instr,gc_string());
      ForEach(instr->get_nested(),i) {
        genInstr(i->second,instr->name());
      }
    }
    map_end();
  }
   
  
  // Generate subinstruction documentation.
  void GenerateCoreDB::genSubInstrs () {
    if (_core.subinstrs().empty()) {
      return;
    }
    tag_start("subinstrs");
    map_start();
    ForEach(_core.subinstrs(),i) {
      auto instr = (i->second);
      tag_start("subinstruction", instr->name());
      map_start();
      genInstrBase(instr);
      genInstrFieldDescription(instr);
      genUsageInfo(*instr);
      map_end();
    }
    map_end();
  }

  void GenerateCoreDB::genRegField (const RegField & rf) {
    tag_start("field", rf.name());
    map_start();

    genDocString(rf.doc());

    if (rf.indexed()) {
      tag_start("indexed");
      map_start();
      tag_start("width");
      int_add(rf.width());
      tag_end();
      tag_start("count");
      int_add(rf.index_count());
      tag_end();
      map_end();
    } else {
      genBits(rf.bits());
    }

    if (rf.reserved()) {
      tag_start("reserved");
      bool_add(true);
      tag_end();
    }
    if (rf.readonly()) {
      tag_start("readonly");
      bool_add(true);
      tag_end();
    }

    genAttrs(rf);

    map_end();
  }

  void GenerateCoreDB::genRegFields (const RegBase & reg) {
    if (reg.has_fields()) {
      tag_start("fields");
      map_start();
      ForEach(reg.fields(),i) {
        genRegField(*(i->second));
      }
      DBG (__LINE__);
      map_end();
    }
  }

  void GenerateCoreDB::genRegReset (const RegBase & reg) {
    switch (reg.resettype()) {
    case RegBase::None:
      break;
    case RegBase::Value:
      tag_start("reset");
      AddString(reg.resetvalue());
      tag_end();
      break;
    case RegBase::Action:
      tag_start("reset");
      addfunc(reg.resetaction());
      tag_end();
    case RegBase::Enum:
      break;
    }
  }

  // Write stuff common to both registers and register files.
  void GenerateCoreDB::genRegBaseStuff (const RegBase & reg) {

    genDocString(reg.doc());

    AddTaggedInt("width",reg.width());

    if (reg.offset()) {
      AddTaggedInt("offset",reg.offset());
    }

    if (reg.pseudo()) {
      AddTaggedBool("pseudo",true);
    }

    if (reg.is_serial()) {
      AddTaggedBool("serial",true);
    }

    // List all fields here.
    genRegFields(reg);

    if (reg.has_reserved_bits()) {
      AddTaggedStr("reserved_mask",reg.reserved_mask().str());
    }

    if (reg.has_readonly_bits()) {
      AddTaggedStr("readonly_mask",reg.readonly_mask().str());
    }

    genAttrs(reg);

    if (reg.is_enumerated()) {
      genEnums(reg.enums());
    }

    // Print reset information.
    genRegReset(reg);


  }

  // Write register read/write hook information.
  void GenerateCoreDB::genRegHook (const gc_string & type, const RegHook & hook) {
    tag_start(type);      // two types here are "read" and "write"
    map_start();
    if (hook.ignore()) {
      tag_start("ignored");
      bool_add(true);
      tag_end ();
    } else if (const RegBase *ra = hook.alias()()) {
      tag_start("alias");
      string_add(ra->name());
      tag_end ();
      if (hook.alias().has_index()) {
        tag_start("alias_index");
        int_add(hook.alias().index());
        tag_end ();
      }
    } else {
      addfunc(hook.action());
    }
    map_end();
  }

  // Generate register documentation
  void GenerateCoreDB::genRegs () {
    if (_core.regs().empty()) {
      return;
    }

    tag_start("regs");
    map_start();

    ForEach(_core.regs(),i) {
      auto reg = *(i->second);

      if (UsedOnly && !_core.used_regs().count(i->second)) {
        continue;
      }

      tag_start("register", reg.name ());
      map_start();

      // Write common stuff.
      genRegBaseStuff(reg);

      // Test for being shared.
      genShared(reg);

      // Write read/write action information.
      if (const RegHook *hook = reg.readhook()) {
        genRegHook("read",*hook);
      }
      if (const RegHook *hook = reg.writehook()) {
        genRegHook("write",*hook);
      }

      DBG (__LINE__ << ":" << __FUNCTION__);
      map_end();
    }

    map_end();
  }

  void GenerateCoreDB::genEntryHook (const gc_string & type, const RegEntryHook & rh) {
    tag_start(type);      // two types here are "read" and "write"
    if (rh.ignore()) {
      tag_start("ignored");
      bool_add(true);
      tag_end ();
    } else {
      addfunc(rh.action());
    }
    tag_end();
  }

  // If we have a sparse register file, then create a table displaying indices vs. register names.
  void GenerateCoreDB::genSparseFile (const RegFile & rf) {
    if (rf.sparse()) {
      tag_start("entries");
      map_start();
      ForEach(rf.entries(),i) {
        const RegEntry &entry = *(i->second);
        tag_start("entry", uitogs(entry.id()));
        map_start();

        if (entry.syntax().empty()) {
          gc_ostringstream ss;
          genEntryName(ss,entry,rf);
          AddTaggedStr("syntax",ss.str());	    
        } else {
          AddTaggedStr("syntax",entry.syntax()); 
        }

        if (const RegName &rn = entry.reg()) {
          tag_start("reg");	
          gc_ostringstream ss;
          if (rn.reg()) {
            ss << rn.reg()->name();
          } else if (rn.regfile()) {
            ss << rn.regfile()->name();
          }
          if (rn.has_index()) {
            ss << "(" << rn.index() << ")";
          }
          string_add(ss.str());
          tag_end();
        }

        if (const RegEntryHook *hook = entry.read()) {
          genEntryHook("read",*hook);
        }
        if (const RegEntryHook *hook = entry.write()) {
          genEntryHook("write",*hook);
        }

        map_end();
      }
      map_end();
    }
  }

  // Write register read/write hook information.
  void GenerateCoreDB::genRegFileHook (const gc_string & type, const RegFileHook & hook) {
    tag_start(type);      // two types here are "read" and "write"
    map_start();
    if (hook.ignore()) {
      tag_start("ignored");
      bool_add(true);
      tag_end ();
    } else if (const RegFile *rfa = hook.alias().regfile()) {
      tag_start("alias");
      map_start();
      tag_start("reg");
      string_add(rfa->name());
      tag_end ();

      if (!hook.aliasindices().empty()) {
        tag_start("map");
        map_start();
        ForRange(hook.aliasindices().size(),i) {
          tag_start("alias_index", uitogs(i));
          int_add(hook.aliasindices()[i]);
          tag_end ();
        }
        map_end();
      }
      map_end();
    } else {
      tag_start("action");
      addfunc(hook.action());
      tag_end ();
    }
    map_end();
  }

  // Generate register-file documentation
  void GenerateCoreDB::genRegFiles () {
    if (_core.regfiles().empty()) {
      return;
    }

    tag_start("regfiles");

    map_start();
    ForEach(_core.regfiles(),i) {
      auto rf = *(i->second);

      if (UsedOnly && !_core.used_regs().count(i->second)) {
        continue;
      }

      tag_start("regfile", rf.name());
      map_start();

      // Write common stuff.
      genRegBaseStuff(rf);

      AddTaggedInt("size",rf.size());

      if (!rf.prefix().empty()) {
        AddTaggedStr("prefix",rf.prefix());
      }

      // Test for being shared.
      genShared(rf);

      // If sparse, then list containing registers.
      genSparseFile(rf);

      // Write read/write action information.
      if (const RegFileHook *hook = rf.readhook()) {
        genRegFileHook("read",*hook);
      }
      if (const RegFileHook *hook = rf.writehook()) {
        genRegFileHook("write",*hook);
      }

      map_end();
    }

    map_end();
  }

  // Generate exception information.
  void GenerateCoreDB::genExceptions () {
    if (_core.exceptions().empty()) {
      return;
    }

    tag_start("exceptions");
    map_start();

    ForEach(_core.exceptions(),i) {
      const Exception &ex = *(i->second);

      tag_start("exception", ex.name());
      map_start();

      genDocString(ex.doc());

      genFields(ex.fields());

      tag_start("priority");
      int_add(ex.priority());
      tag_end ();

      tag_start("action");
      addfunc(ex.action());
      tag_end ();

      map_end();
    }

    map_end();
  };

  void GenerateCoreDB::genResources(const Context &ctx) {
    genKeysList("regs",ctx._regs);
    genKeysList("regfiles",ctx._regfiles);
    genKeysList("mmu",ctx._mmu);
    genKeysList("parms",ctx._parms);
    genKeysList("caches",ctx._caches);
    genKeysList("mems",ctx._memories);
    genKeysList("eventbuses",ctx._eventbuses);
  }


  // Generate context information.
  void GenerateCoreDB::genContexts () {
    if (_core.contexts().empty()) {
      return;
    }

    tag_start("contexts");
    map_start();

    ForEach(_core.contexts(),i) {
      const Context &ctx = *(i->second);

      tag_start("context", ctx.name());
      map_start();

      genDocString(ctx.doc());

      if (ctx.is_thread()) {
        AddTaggedBool("thread",true);
      }

      tag_start("active");
      addfunc(ctx.active());
      tag_end ();

      tag_start("num_contexts");
      int_add(ctx.num_contexts());
      tag_end ();

      genResources(ctx);

      map_end();
    }

    map_end();
  }

  // Generate cache information.
  void GenerateCoreDB::genCaches () {
    if (_core.caches().empty()) {
      return;
    }

    tag_start("caches");
    map_start();

    ForEach(_core.caches(),i) {
      const Cache &cache = *(i->second);

      tag_start("cache", cache.name());
      map_start();

      genDocString(cache.doc());

      AddTaggedInt("level",cache.level());

      tag_start("type");
      switch (cache.type()) {
      case InstrCache:
        string_add("instr");
        break;
      case DataCache:
        string_add("data");
        break;
      case UnifiedCache:
        string_add("unified");
        break;
      }
      tag_end ();

      tag_start("size");
      int_add(cache.size());
      tag_end ();

      tag_start("line_size");
      int_add(cache.linesize());
      tag_end ();

      if (cache.fully_assoc()) {
        tag_start("fully_assoc");
        bool_add(true);
        tag_end ();
      } else {
        tag_start("set_assoc");
        int_add(cache.ways());
        tag_end ();
      }

      genFields(cache.set_fields(),"setfields");

      genFields(cache.way_fields(),"wayfields");

      tag_start("enable-predicate");
      addfunc(cache.enable());
      tag_end ();

      tag_start("hit");
      addfunc(cache.hit());
      tag_end ();

      tag_start("miss");
      addfunc(cache.miss());
      tag_end ();

      tag_start("invalidate-line");
      addfunc(cache.invalidate_line());
      tag_end ();

      tag_start("read-line");
      addfunc(cache.read_line());
      tag_end ();

      tag_start("replace");
      addfunc(cache.replace());
      tag_end ();

      tag_start("line-access");
      addfunc(cache.line_access());
      tag_end ();

      tag_start("writethrough-predicate");
      addfunc(cache.write_through());
      tag_end ();

      tag_start("number-of-sets");
      addfunc(cache.sets_func());
      tag_end ();

      tag_start("number-of-ways");
      addfunc(cache.ways_func());
      tag_end ();

      map_end();
    }
    map_end();
  }

  // Generate eventbus information.
  void GenerateCoreDB::genEventBuses () {
    if (_core.eventbuses().empty()) {
      return;
    }

    tag_start("eventbuses");
    map_start();

    ForEach(_core.eventbuses(),i) {
      const EventBus &eb = *(i->second);

      tag_start("eventbus", eb.name());
      map_start();

      genDocString(eb.doc());

      genFields(eb.fields());

      tag_start("action");
      addfunc(eb.action());
      tag_end ();

      map_end();
    }

    map_end();
  }

  // Generate external resource information.
  void GenerateCoreDB::genExtResources () {
    if (_core.extresources().empty()) {
      return;
    }

    tag_start("ext_resources");
    map_start();

    ForEach(_core.extresources(),i) {
      const ExtResource &er = *(i->second);

      tag_start("ext_resource", er.name());
      map_start();

      genDocString(er.doc());

      AddTaggedStr("objtype",er.objtype());

      AddTaggedBool("external",er.external());

      tag_start("constr_args");
      list_start();
      ForEach(er.constr_args(),i) {
        AddTaggedStr("constr_arg",(*i)->ToString());
      }
      
      list_end();

      map_end();
    }

    map_end();
  }


  // Generate memory information.
  void GenerateCoreDB::genMemories () 
  {
    if (_core.memories().empty()) {
      return;
    }

    tag_start("memories");
    map_start();

    ForEach(_core.memories(),i) {
      const Memory &mem = *(i->second);

      tag_start ("memory", mem.name());
      map_start();

      genDocString(mem.doc());

      tag_start("instr_mem");
      bool_add(mem.instr_mem());
      tag_end ();

      if (mem.prefetch_mem()) {
        AddTaggedBool("prefetch_mem",true);
      }

      tag_start("size");
      int_add(mem.size());
      tag_end ();

      tag_start("addr_unit");
      int_add(mem.addr_unit());
      tag_end ();

      if (mem.parent()) {
        tag_start("parent");
        list_start();
        string_add(mem.parent()->name());
        int_add(mem.offset());
        list_end();
      }

      if (const MemoryHook *mh = mem.read_hook()) {
        tag_start("read");
        addfunc(mh->action());
        tag_end ();
      }
      if (const MemoryHook *mh = mem.write_hook()) {
        tag_start("write");
        addfunc(mh->action());
        tag_end ();
      }

      map_end();
    }

    map_end();
  }

  void GenerateCoreDB::genMmuLookups (const MmuLookupHash & lookups, const gc_string & name) {
    if (lookups.empty()) {
      return;
    }

    tag_start("mmulookups", name);
    map_start();

    ForEach(lookups,l) {
      const MmuLookup &lookup = *(l->second);

      if (lookup.is_interface()) {
        continue;
      }

      tag_start("mmulookup", lookup.name ());
      map_start();

      genDocString(lookup.doc());

      switch (lookup.type()) {
      case MmuLookup::Instr:
        tag_start("instr-only");
        bool_add(true);
        tag_end ();
        break;
      case MmuLookup::Data:
        tag_start("data-only");
        bool_add(true);
        tag_end ();
        break;
      default:
        ;
      }

      tag_start("priority");
      int_add(lookup.priority());
      tag_end ();

      tag_start("page-size");      
      map_start();
      switch (lookup.sizetype()) {
      case MmuLookup::None:
        break;
      case MmuLookup::Const:
        tag_start("const");
        bool_add(true);
        tag_end ();

        tag_start("size");
        int_add(lookup.pagesize());
        tag_end ();

        break;
      case MmuLookup::BitSize:
        tag_start("bitsize"); bool_add(true);                         tag_end ();
        tag_start("scale");   int_add(lookup.sizescale());            tag_end ();
        tag_start("field");   string_add(lookup.sizefield()->name()); tag_end ();
        tag_start("offset");  int_add(lookup.sizeoffset());           tag_end ();
        tag_start("shift");   int_add(lookup.sizeshift());            tag_end ();
        break;
      case MmuLookup::LeftMask:
        tag_start("leftmask"); bool_add(true);                         tag_end ();
        tag_start("field");    string_add(lookup.sizefield()->name()); tag_end ();
        tag_start("shift");    int_add(lookup.sizeshift());            tag_end ();
        break;
      }
      map_end();

      if (lookup.has_array()) {
        tag_start("entries");
        int_add(lookup.entries());
        tag_end ();

        if (lookup.fully_assoc()) {
          tag_start("fully_assoc");
          bool_add(true);
          tag_end ();
        } else {
          tag_start("set_assoc");
          int_add(lookup.assoc());
          tag_end ();
        }
      }

      genFields(lookup.set_fields(),"setfields");
      genFields(lookup.way_fields(),"wayfields");

      if (!lookup.tests().empty()) {
        if (lookup.tests().size() == 1) {
          tag_start("tests");
          list_start();
          ForEach(lookup.tests().begin()->second->tests(),t) {
            addexpr(*t);
          }
          list_end();
        } else {
          tag_start("testsets");
          map_start();
          ForEach(lookup.test_order(),t) {
            const MmuTestSet &testset = **t;
            tag_start("testset",testset.name());
            map_start();
            if (testset.enable()) {
              tag_start("enable");
              addfunc(testset.enable());
              tag_end();
            }
            tag_start("tests");
            list_start();
            ForEach(testset.tests(),iter) {
              addexpr(*iter);
            }
            list_end();
            map_end();
          }
          map_end();
        }
      }

      tag_start("exec-perm");
      addfunc(lookup.exec_perm());
      tag_end ();

      tag_start("load-perm");
      addfunc(lookup.load_perm());
      tag_end ();

      tag_start("store-perm");
      addfunc(lookup.store_perm());
      tag_end ();

      tag_start("valid");
      addfunc(lookup.valid_func());
      tag_end ();

      tag_start("hit");
      addfunc(lookup.hit_func());
      tag_end ();

      tag_start("miss");
      addfunc(lookup.miss_func());
      tag_end ();

      tag_start("reset");
      addfunc(lookup.reset());
      tag_end ();

      tag_start("mem_read");
      addfunc(lookup.mem_read());
      tag_end();

      tag_start("mem_write");
      addfunc(lookup.mem_write());
      tag_end();

      tag_start("mem_size");
      int_add(lookup.mem_size());
      tag_end ();

      if (!lookup.lookups().empty()) {
        genMmuLookups(lookup.lookups(),"child-lookups");
      }

      map_end();
    }

    map_end();
  }

  void GenerateCoreDB::genMmu () 
  {
    if (const MMU *mmu = _core.mmu()) {

      tag_start("mmu");
      map_start();

      genDocString(mmu->doc());

      AddTaggedBool("interleaved-fetch",mmu->interleaved_fetch());
      AddTaggedBool("interleaved-reads",mmu->interleaved_reads());
      AddTaggedBool("interleaved-writes",mmu->interleaved_writes());

      genMmuLookups(mmu->top_lookups(),"lookups");

      tag_start("instr-enable");
      addfunc(mmu->instr_enable());
      tag_end ();

      tag_start("data-enable");
      addfunc(mmu->data_enable());
      tag_end ();

      tag_start("instr-miss");
      addfunc(mmu->instr_miss());
      tag_end ();

      tag_start("data-miss");
      addfunc(mmu->data_miss());
      tag_end ();

      tag_start("load-miss");
      addfunc(mmu->load_miss());
      tag_end ();

      tag_start("store-miss");
      addfunc(mmu->store_miss());
      tag_end ();

      tag_start("multi-hit");
      addfunc(mmu->multi_hit());
      tag_end ();

      tag_start("aligned-write");
      addfunc(mmu->aligned_write());
      tag_end ();

      tag_start("misaligned-write");
      addfunc(mmu->misaligned_write());
      tag_end ();

      tag_start("misaligned-read");
      addfunc(mmu->misaligned_read());
      tag_end ();

      tag_start("post-write");
      addfunc(mmu->post_write());
      tag_end ();

      tag_start("post-read");
      addfunc(mmu->post_read());
      tag_end ();

      map_end();
    }
  }

  void GenerateCoreDB::genCoreLevelHooks()
  {
    tag_start("core-level-hooks");
    map_start();

    tag_start("decode-miss");
    addfunc(_core.decode_miss_handler());
    tag_end();

    tag_start("pre-cycle");
    addfunc(_core.pre_cycle_handler());
    tag_end();

    tag_start("post-cycle");
    addfunc(_core.post_cycle_handler());
    tag_end();

    tag_start("pre-fetch");
    addfunc(_core.pre_fetch_handler());
    tag_end();

    tag_start("post-fetch");
    addfunc(_core.post_fetch_handler());
    tag_end();

    tag_start("post-exec");
    addfunc(_core.post_exec_handler());
    tag_end();

    tag_start("post-asm");
    addfunc(_core.post_asm_handler());
    tag_end();

    tag_start("post-packet-asm");
    addfunc(_core.post_packet_asm_handler());
    tag_end();

    tag_start("post-packet");
    addfunc(_core.post_packet_handler());
    tag_end();

    tag_start("active-watch");
    addfunc(_core.active_watch());
    tag_end();

    tag_start("instr-table-watch");
    addfunc(_core.itable_watch());
    tag_end();

    map_end();
  }

  void GenerateCoreDB::genParms() {
    if (_core.parms().empty()) {
      return;
    }

    tag_start("parms");
    map_start();
    ForEach(_core.parms(),i) {
      const Parm &parm = *(i->second);
      tag_start("parm",parm.name());
      map_start();

      if (parm.has_value()) {
        AddTaggedStr("value",parm.value());
      }

      tag_start("options");
      list_start();
      ForEach(parm.options(),opt) {
        string_add(*opt);
      }
      list_end();

      if (parm.watch()) {
        tag_start("watch");
        addfunc(parm.watch());
        tag_end();	     
      }

      if (parm.is_const()) {
        AddTaggedBool("constant",true);
      }

      map_end();       
    }
    map_end();
  }

  void GenerateCoreDB::genMask(const AddrMask &m) {
    tag_start(m.name());
    map_start();
    tag_start("initial");
    AddString("0x" << std::hex << m.initial_value());
    tag_end();
    AddTaggedBool("constant",m.is_const());
    if (m.watch()) {
      AddTaggedFunc("watch",m.watch());
    }
    map_end();
  }

  void GenerateCoreDB::genMasks() {
    genMask(_core.ra_mask());
    genMask(_core.ea_mask());
  }

  void GenerateCoreDB::genHelpers() {
    if (!_core.helpers().empty()) {
      tag_start("helpers");
      map_start();
      ForEach(_core.helpers(),i) {
        if (UsedOnly && !_core.used_helpers().count(i->first)) {
          continue;
        }
        tag_start("helper",i->first);
        map_start();
        tag_start("action");
        gc_ostringstream ss;
        ss << i->second._func;
        bigstring_add(ss.str());
        tag_end();
        genUsageInfo(i->second);
        map_end();
      }
      map_end();
    }
  }

  void GenerateCoreDB::genAsmConfig() {
    tag_start("asm_config");
    map_start();
    auto asmcfg = _core.asm_config();
    genStrList("comments",asmcfg.comments());
    genStrList("line_comments",asmcfg.line_comments());
    genStrList("line_seps",asmcfg.line_seps());
    genStrList("instr_seps",asmcfg.line_seps());

    const AsmConfig::StrPairList &gseps = asmcfg.group_seps();

    if (!gseps.empty()) { 
      tag_start("group_seps");
      list_start();
      ForEach(gseps,i) {
        sublist_start("group_sep");
        string_add(i->first);
        string_add(i->second);
        list_end();
      }
      list_end();
    }   

    genStrList("asm_instrtables",asmcfg.asm_instrtables());
    genStrList("disasm_instrtables",asmcfg.disasm_instrtables());

    if (!asmcfg.parms().empty()) {
      tag_start("parms");
      map_start();
      ForEach(asmcfg.parms(),i) {
        const AsmParm &parm = *(i->second);
        tag_start("parm",parm.name());
        map_start();
        genDocString(parm.doc());
        if (parm.value() != (uint64_t)-1) {
          AddTaggedInt("value",parm.value());
        }
        if (parm.is_const()) {
          AddTaggedBool("constant",true);
        }
        switch (parm.type()) {
        case AsmParm::Integer:
          AddTaggedStr("type","int");
          break;
        case AsmParm::Label:
          AddTaggedStr("type","label");
          break;
        default:
          break;
        }

        map_end();
      }

      map_end();
    }    

    if (!asmcfg.rules().empty()) {
      tag_start("rules");
      map_start();
      ForEach(asmcfg.rules(),i) {
	const AsmRule &rule = *(i->second);
	tag_start("rule",rule.name());
	map_start();
	genDocString(rule.doc());
	switch(rule.type()) {
	case AsmRule::Instr:
	  AddTaggedStr("type","Instr");
	  break;
	case AsmRule::Packet:
	  AddTaggedStr("type","Packet");
	  break;
	default:
	  break;
	}
	AddTaggedFunc("action",rule.action());
	map_end();
      }
      map_end();
    }
    map_end();
  }

  // If we care about only generating used data, then calculate what's used
  // here.
  void GenerateCoreDB::calcUsedData()
  {
    if (UsedOnly) {
      // First, list only instruction fields used by instructions.
      ForEach(_core.instrs(),i) {
        const Instr &instr = *i->second;
        ForEach(instr.getAllFields(),j) {
          if (j->_field) {
            const InstrField *f = j->_field;
            _used_instrfields.insert(j->_field);
            // If this is just an implementation of a real instruction field,
            // then we consider that item used as well.
            if (f->interface()) {
              _used_instrfields.insert(f->interface());
            }
          }
        }

      }

      // Then look at used relocations based upon the instruction fields.
      ForEach(_used_instrfields,i) {
        const InstrField &field = **i;
        if (!field.relocs().empty()) {
          _used_relocs.insert(field.relocs().front());
        }
      }
    }

    // ForEach(_core.used_regs(),i) {
    //   cout << "Used reg:  " << i->regb()->name() << "\n";
    // }
  }

  GenerateCoreDB::GenerateCoreDB (const Core & c, ostream & o, const DocCfg &doccfg) :
    _core (c),
    _o (o), 
    _walker(c,c.env()),
    _doccfg(doccfg),
    _no_expand_attr(Data::get_attr(DocNoExpandExprs))
  {
  }

  GenerateCoreDB::~GenerateCoreDB () {
  }

  void GenerateCoreDB::exec () {

    tag_start ("core", _core.name());
    map_start();

    genDocString(_core.doc());

    if (!_core.type().empty()) {
      AddTaggedStr("type",_core.type());
    }

    if (_core.is_little_endian()) {
      AddTaggedStr("bit_endianness","little");
    }

    genStrList("blocks",_core.blocks());
    genStrList("type_declarations",_core.type_decls());

    if (_core.parallel_execution()) {
      AddTaggedInt("parallel_execution",_core.parallel_execution());
    }

    calcUsedData();

    genMasks();
    genRegs();
    genRegFiles();
    genRelocations();
    genInstrFields();
    genInstrs(); 
    genSubInstrs();
    genExceptions();
    genContexts();
    genCaches();
    genEventBuses();
    genExtResources();
    genMemories();
    genMmu();
    genCoreLevelHooks();
    genGroups();
    genParms();

    genAsmConfig();
    genHelpers();

    map_end();
  };


  //   class GenerateSysDB : public GenerateDBiface {

  //  public:
  GenerateSysDB::GenerateSysDB (const Data & data, ostream & o, const DocCfg &doccfg) : 
    _data(data), _o(o), _doccfg(doccfg) {
  }

  GenerateSysDB::~GenerateSysDB() {
  }

  void GenerateSysDB::exec () {

    time_t tt = time(0);
    tm ttm = *localtime(&tt);

    gc_ostringstream ss;
    ss << ttm.tm_mday << "." << ttm.tm_mon + 1 << "." << ttm.tm_year + 1900 
       << " at " << ttm.tm_hour << ":" << ttm.tm_min << ":" << ttm.tm_sec;

    data_start();
     
    if (PrintDate) {
      tag_start("date");
      string_add(ss.str());
      tag_end();
    }

    tag_start ("cores");
    map_start ();
    ForEach(_data.cores(),i) {
      genItem(i->second,Resources());
    }
    map_end ();
    tag_start ("systems");
    map_start ();
    ForEach(_data.systems(),i) {
      genItem(i->second,Resources());
    }
    map_end ();
    data_end();
  }

  void GenerateSysDB::genItem (const SysItem * item, const Resources & pshared) {
    if (const Core *core = dynamic_cast<const Core *>(item)) {

      GenerateCoreDB *gencore = make_core_db(*core,_o,_doccfg);
      gencore->exec();      
    } else if (const System *sys = dynamic_cast<const System *>(item)) {
      genSystem(*sys,pshared);
    }
  }

  void GenerateSysDB::listSharedItems (const gc_string & key, const ResItems & items) {
    assert (key == "regs" || key == "regfiles" || key == "caches" ||
            key == "mmulookups" || key == "memories" || key == "eventbuses"); 
    tag_start(key); // string " checked
    list_start();
    ForEach(items,i) {
      string_add(i->first);
    }
    list_end();
  }

  void GenerateSysDB::genSharedResources (const System & sys, const Resources & pshared) {
    if (!sys.doc().empty()) {
      tag_start("overview");
      string_add(sys.doc());
      tag_end ();
    }

    const Resources &shared = sys.shared();
    listSharedItems("regs",shared._regs);
    listSharedItems("regfiles",shared._regfiles);
    listSharedItems("caches",shared._caches);
    listSharedItems("mmulookups",shared._mmu);
    listSharedItems("memories",shared._memories);
    listSharedItems("eventbuses",shared._eventbuses);

  }

  void GenerateSysDB::genSystem (const System & sys, const Resources & pshared) {
    tag_start("sys", sys.name ());
    map_start ();

    genSharedResources(sys,pshared);

    ForEach(sys.items(),i) {
      genItem(i->second,sys.shared());
    }    
    map_end ();
  }

  // The Perl writer class.
  //  class GeneratePerlCoreDB : public GenerateCoreDB {

  //  public:

  GeneratePerlCoreDB::GeneratePerlCoreDB (const Core & c, ostream & o, const DocCfg &doccfg) : 
    GenerateCoreDB (c, o, doccfg) {
  }

  // The overall data structure is called $data and is a hash reference.
  void GeneratePerlCoreDB::data_start () {
    _o << "$data = {\n";
  }

  void GeneratePerlCoreDB::data_end () {
    _o << "};\n\n";
  }

  void GeneratePerlCoreDB::list_start () {
    _o << "[\n";
  }

  void GeneratePerlCoreDB::list_end () {
    _o << "],\n";
  }

  void GeneratePerlCoreDB::sublist_start (const gc_string &sep) {
    list_start();
  }

  void GeneratePerlCoreDB::map_start () {
    _o << "{\n";
  }

  void GeneratePerlCoreDB::submap_start (const gc_string &sep) {
    map_start();
  }

  void GeneratePerlCoreDB::map_end () {
    _o << "},\n";
  }

  void GeneratePerlCoreDB::tag_start (const gc_string & key, const gc_string & name) {
    if (name == "") {
      _o << "\"" << key << "\" => ";
    } else {
      _o << "\"" << name << "\" => ";
    }
  }

  void GeneratePerlCoreDB::int_add (uint64_t x) {
    _o << x << ",\n";
  }

  void GeneratePerlCoreDB::bool_add (bool x) {
    _o << x << ",\n";
  }

  void GeneratePerlCoreDB::string_add (const gc_string & x) {
    _o << "\"" << x << "\",\n";
  }

  void GeneratePerlCoreDB::bigstring_add (const gc_string & x) {
    _o << "<<DATA\n" << x << "\nDATA\n,\n";
  }


  GeneratePerlSysDB::GeneratePerlSysDB (const Data & d, ostream &o, const DocCfg &doccfg) :
    GenerateSysDB (d, o, doccfg) {
  }

  GeneratePerlCoreDB * GeneratePerlSysDB::make_core_db (const Core & c, ostream & o, const DocCfg &doccfg) {
    return new GeneratePerlCoreDB (c, o, doccfg);
  }

  // The overall data structure is called $data and is a hash reference.
  void GeneratePerlSysDB::data_start () {
    _o << "$data = {\n";
  }

  void GeneratePerlSysDB::data_end () {
    _o << "};\n\n";
  }

  void GeneratePerlSysDB::list_start () {
    _o << "[\n";
  }

  void GeneratePerlSysDB::list_end () {
    _o << "],\n";
  }

  void GeneratePerlSysDB::sublist_start (const gc_string &sep) {
    list_start();
  }

  void GeneratePerlSysDB::map_start () {
    _o << "{\n";
  }

  void GeneratePerlSysDB::submap_start (const gc_string &sep) {
    map_start();
  }

  void GeneratePerlSysDB::map_end () {
    _o << "},\n";
  }

  void GeneratePerlSysDB::tag_start (const gc_string & key, const gc_string & name) {
    if (name == "") {
      _o << "\"" << key << "\" => ";
    } else {
      _o << "\"" << key << ":" << name << "\" => ";
    }
  }

  void GeneratePerlSysDB::int_add (uint64_t x) {
    _o << x << ",\n";
  }

  void GeneratePerlSysDB::bool_add (bool x) {
    _o << x << ",\n";
  }

  void GeneratePerlSysDB::string_add (const gc_string & x) {
    _o << "\"" << x << "\",\n";
  }

  void GeneratePerlSysDB::bigstring_add (const gc_string & x) {
    _o << "<<DATA\n" << x << "\nDATA\n,\n";
  }

  GenerateXmlCoreDB::GenerateXmlCoreDB (const Core & c, ostream & o, const DocCfg &doccfg)
    : GenerateCoreDB (c, o, doccfg) {
  }

  // The database contains a single item named <data>.  We also have the usual XML header stuff.
  void GenerateXmlCoreDB::data_start () {
    _o << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n\n"
       << "<data>\n"; }

  void GenerateXmlCoreDB::data_end () { _o << "</data>\n"; }
  void GenerateXmlCoreDB::list_start () { 
    DBG (__FUNCTION__);
    no_calls_stack.back () = false; 
    key_count_stack.back ()++;
  }

  void GenerateXmlCoreDB::list_end () {
    DBG (__FUNCTION__ << " size " << key_count_stack.size ());
    if (key_count_stack.size ()) {
      DBG(" back " << key_names.back() << ":" << key_count_stack.back ());
    }
    if (no_calls_stack.back () ||
        (--(key_count_stack.back ()) <= 0)) {
      tag_end ();
    }
  }

  void GenerateXmlCoreDB::sublist_start (const gc_string &sep) { 
    tag_start(sep);
    list_start();
  }


  void GenerateXmlCoreDB::map_start () { 
    DBG (__FUNCTION__);
    no_calls_stack.back () = false; 
    key_count_stack.back ()++; 
  }

  void GenerateXmlCoreDB::submap_start (const gc_string &sep) { 
    tag_start(sep);
    map_start();
  }

  void GenerateXmlCoreDB::map_end () { 
    DBG (__FUNCTION__ << " no_calls " << no_calls_stack.back ()
         << " back () " << key_names.back () 
         << ":" << key_count_stack.back () 
         << " size () " << key_count_stack.size ());
    if (no_calls_stack.back () ||
        (--key_count_stack.back () <= 0)) {
      tag_end ();
    }
  }
  void GenerateXmlCoreDB::tag_start (const gc_string & key, const gc_string & name) {
    if (name == "") {
      _o << "<" << key << ">\n"; 
    } else {
      _o << "<" << key << " name=\"" << name << "\">\n";
    }
    DBG (__FUNCTION__ << ": keynames push " << key);
    key_names.push_back (key);
    key_count_stack.push_back(0);
    no_calls_stack.push_back (true);
  }
  void GenerateXmlCoreDB::tag_end () {
    DBG (__FUNCTION__ << ": back '" << key_names.back () << "'");
    if (!key_names.empty()) {
      _o << "</" << key_names.back () << ">\n";
      key_names.pop_back ();
      key_count_stack.pop_back();
      no_calls_stack.pop_back();
    } else {
      _o << "\n***************************************************************"
         << "\n***************************************************************"
         << " Error : key_names was empty" 
         << "\n***************************************************************"
         << "\n***************************************************************" << endl;;
    }
  }
  void GenerateXmlCoreDB::int_add (uint64_t x) {
    _o << "<int>" << x << "</int>\n";
  }
  void GenerateXmlCoreDB::bool_add (bool x) {
    if (x) {
      _o << "<true />\n";
    } else {
      _o << "<false />\n";
    }
  }

  struct XmlReplacement {
    XmlReplacement (char k, gc_string r) : key (k), replacement (r) {}
    char key;
    gc_string replacement;
  };

  // entity replacement for standard strings.
  gc_string xml_replace (const gc_string & in) {
    /*
      5 standard entities

      <   &lt;
      >   &gt;
      &   &amp;
      '   &apos;
      "   &quote;
    */
    list <XmlReplacement> rt;
    rt.push_back (XmlReplacement ('<', "&lt;"));
    rt.push_back (XmlReplacement ('>', "&gt;"));
    rt.push_back (XmlReplacement ('&', "&amp;"));
    rt.push_back (XmlReplacement ('\'', "&apos;"));
    rt.push_back (XmlReplacement ('\"', "&quote"));

    gc_ostringstream outstr;
    ForEach (in, i) {
      bool replaced = false;
      ForEach (rt, q) {
        if (*i == q->key) {
          outstr << q->replacement;
          replaced = true; 
        }
      }
      if (!replaced) {
        outstr << *i;
      }
    }
    return outstr.str ();
  }

  void GenerateXmlCoreDB::string_add (const gc_string & x) {
    gc_string xml_string = xml_replace (x);
    _o << "<str>" << xml_string << "</str>\n";
  }
  void GenerateXmlCoreDB::bigstring_add (const gc_string & x) {
    _o << "<str><![CDATA[\n" << x << "\n]]></str>\n";
  }

  GenerateXmlSysDB::GenerateXmlSysDB (const Data &d, ostream &o, const DocCfg &doccfg) : GenerateSysDB (d, o, doccfg) { }

  GenerateXmlCoreDB *  GenerateXmlSysDB::make_core_db (const Core &c, ostream &o, const DocCfg &doccfg) {
    return new GenerateXmlCoreDB (c, o, doccfg); }

  void  GenerateXmlSysDB::data_start() {
    _o << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n\n"
       << "<data>\n"; }

  void  GenerateXmlSysDB::data_end () {
    _o << "</data>\n";
  }
  void  GenerateXmlSysDB::list_start () { 
    DBG (__FUNCTION__);
    no_calls_stack.back () = false; 
    key_count_stack.back ()++;
  }

  void  GenerateXmlSysDB::list_end () {
    DBG (__FUNCTION__ << ": size " << key_count_stack.size ()
         << " back " << key_names.back () << ":"
         << key_count_stack.back ());
    if (no_calls_stack.back () ||
        (--(key_count_stack.back ()) <= 0)) {
      tag_end ();
    }
  }

  void GenerateXmlSysDB::sublist_start (const gc_string &sep) { 
    tag_start(sep);
    list_start();
  }

  void GenerateXmlSysDB::map_start () { 
    DBG (__FUNCTION__);
    no_calls_stack.back () = false; 
    key_count_stack.back ()++; 
  }
   
  void GenerateXmlSysDB::submap_start (const gc_string &sep) { 
    tag_start(sep);
    map_start();
  }
   
  void  GenerateXmlSysDB::map_end () { 
    DBG (__FUNCTION__ << ": no_calls " << no_calls_stack.back ()
         << " back () " << key_names.back () << ":" << key_count_stack.back () )
      if (no_calls_stack.back () ||
          (--key_count_stack.back () <= 0)) {
        tag_end ();
      }
  }
  void  GenerateXmlSysDB::tag_start (const gc_string & key, const gc_string & name) {
    DBG (__FUNCTION__);
    if (name == "") {
      _o << "<" << key << ">\n"; 
    } else {
      _o << "<" << key << " name=\"" << name << "\">\n";
    }
    DBG (__FUNCTION__ << ": push " << key);
    key_names.push_back (key);
    key_count_stack.push_back(0);
    no_calls_stack.push_back (true);
  }
  void  GenerateXmlSysDB::tag_end () {
    DBG (__FUNCTION__ << ": no_calls " << no_calls_stack.back ()
         << " back () " << key_names.back () << ":" << key_count_stack.back () )

      if (!key_names.empty()) {
        _o << "</" << key_names.back () << ">\n";
        key_names.pop_back ();
        key_count_stack.pop_back();
        no_calls_stack.pop_back();
      } else {
        _o << "\n***************************************************************"
           << "\n***************************************************************"
           << " Error : key_names was empty" 
           << "\n***************************************************************"
           << "\n***************************************************************" << endl;;
      }
  }
  void  GenerateXmlSysDB::int_add (uint64_t x) {
    _o << "<int>" << x << "</int>\n";
  }
  void  GenerateXmlSysDB::bool_add (bool x) {
    if (x) {
      _o << "<true />\n";
    } else {
      _o << "<false />\n";
    }
  }

  void  GenerateXmlSysDB::string_add (const gc_string & x) {
    gc_string xml_string = xml_replace (x);
    _o << "<str>" << xml_string << "</str>\n";
  }

  void  GenerateXmlSysDB::bigstring_add (const gc_string &x) {
    _o << "<str><![CDATA[\n" << x << "\n]]></str>\n";
  }
  //   };

  // Create a generate class of the appropriate type.
  GenerateSysDB * make_sys_db (const gc_string & type, const Data & data, ostream & out, const DocCfg &doccfg) {
    if (type == "perl") {
      return new GeneratePerlSysDB(data,out,doccfg);
    } else if (type == "xml") {
      return new GenerateXmlSysDB (data, out,doccfg);
    } else {
      RError("Unrecognized database type:  " << type);
    }
  }

  // Opens the output file and initiates the model creation process.
  void process (const Data & data, const gc_string & type, const AnyOption & options,const DocCfg &doccfg) 
  {
    GenerateSysDB * gendb = make_sys_db (type, data, cout, doccfg);

    gendb->exec ();
  }
}

using namespace adl;

DocCfg *create_configdb(const AnyOption &options,bool fullpatherrors)
{
  string conf_name = options.getOption("config");
  
  DocCfg *configdb = new DocCfg;
  configdb->parse(conf_name,fullpatherrors);
  
  return configdb;
}

int main (int argc, const char ** argv) 
{
  log_usage_at_exit(argc,argv,"tool");

  AnyOption options (argc,argv);
  standardParserOptionSetup(options);

  options.addUsage  (":  [options] <source>");
  options.setFlag   ("help","h",         "Display help.");
  options.setFlag   ("print-date", "pd", "Print date of file creation.  Default is "+btos(PrintDate)+".");
  options.setFlag   ("used-reg-fields",  "Display register fields in usage information.  Default is "+btos(RegUsageFields)+".");
  options.setFlag   ("expand-exprs",     "Expand variables in-place in instruction action code.  Default is "+btos(ExpandExprs)+".");
  options.setFlag   ("used-only",        "Remove unused elements such as instruction fields and relocations.  Default is "+btos(UsedOnly)+".");
  options.setFlag   ("orig-alias-fmt",   "Use the original alias format for instructions with single aliases.  Default is "+btos(OrigAliasFormat)+".");
  options.setOption ("type","t",         "Specify a database type.  Currently supported types are 'perl', 'xml', 'full-xml'.");
  options.setOption ("config","cf",      "Specify a documentation-generation configuration file.");

  if (!options.processCommandLine ()) {
    return 1;
  }
  if (options.getFlag ("h")) {
    options.printUsage ();
    return 0;    
  }

  if (!options.getArgc ()) {
    cerr << "No source file specified.\n";
    options.printUsage ();
    return 1;
  }

  AdlParser parser;

  string input = options.getArgv (0);
  gc_string type = options.getOption ("type").c_str();

  bool fullpatherrors = standardParserSetup(parser,options,"dummy");

  if (type.empty ()) {
    cerr << "No database type was specified.\n";
    options.printUsage ();
    return 1;
  }

  PrintDate       = options.getFlag("print-date",PrintDate);
  RegUsageFields  = options.getFlag("used-reg-fields",RegUsageFields);
  ExpandExprs     = options.getFlag("expand-exprs",ExpandExprs);
  UsedOnly        = options.getFlag("used-only",UsedOnly);
  OrigAliasFormat = options.getFlag("orig-alias-fmt",OrigAliasFormat);

  try {

    DocCfg *doccfg = create_configdb(options,fullpatherrors);
    
    // This parses the ADL file and creates the data model.
    if (!parser.parse (input)) {
      return 1;
    }
    
    process(parser.data(),type,options,*doccfg);
  } catch (exception & err) {
    cerr << "Error:  " << err.what () << "\n";
    return 1;
  }

  return 0;
}


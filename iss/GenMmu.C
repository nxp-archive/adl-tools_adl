//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Main implementation file for generating an MMU.
// The generation code is contained within the GenMmu class, which
// is private.  This lets me have the advantages of using a class 
// (no need for forward declarations, member variables, etc.) without
// having to have a separate h/C file, which is a pain to maintain.
//

#include <sstream>
#include <stdexcept>
#include <cassert>

#include "opencxx/parser/Ptree.h"
#include "opencxx/parser/PtreeUtil.h"

#include "fe/Data.h"
#include "fe/Helpers.h"

#include "helpers/pfxstream.h"
#include "helpers/Macros.h"
#include "helpers/CodeGen.h"

#include "AttrsHelper.h"
#include "GenMmu.h"
#include "GenInstrPacket.h"
#include "InstrWalker.h"
#include "ConfigDB.h"
#include "GlobalMemoryName.h"
#include "MiscTypes.h"

using namespace Opencxx;
using namespace PtreeUtil;
using namespace std;

namespace adl {

  static gc_string NoMmuControlAttr = "no_mmu_control";

  static const char *attr_used = " ATTRIBUTE_USED ";

  // Translation class used for converting MMU fields into the proper
  // expression for accessing a field.
  class MmuWalker : public RtlWalker {
  public:
    MmuWalker(Opencxx::Environment *e,const MmuLookup &lookup,const gc_string &entry,bool lib_mode) :
      RtlWalker(e,false,lib_mode),
      _lookup(lookup),
      _entry(entry)
    {
    }

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

    virtual Opencxx::Ptree* TranslateVariable(Opencxx::Ptree*exp)
    {
      if (_lookup.find_field(exp,0)) {
        return Ptree::Make("%s.%p",_entry.c_str(),exp);
      } else {
        return RtlWalker::TranslateVariable(exp);
      }
    }
  
  private:
    const MmuLookup &_lookup;
    gc_string           _entry;
  };

  gc_string getWayUserTypeName(const MmuLookup &l)
  {
    return l.type_name();
  }

  gc_string getWayTypeName(const MmuLookup &l)
  {
    return l.name() + "_way_t";
  }

  gc_string getDefaultSetTypeName(const MmuLookup &l)
  {
    return (l.name() + "_set_t");
  }

  bool has_set_type(const MmuLookup &l)
  {
    return (l.ways() > 1) || (l.is_interface());
  }

  gc_string getSetTypeName(const MmuLookup &l)
  {
    return (has_set_type(l)) ? getDefaultSetTypeName(l) : getWayTypeName(l);
  }

  gc_string getBaseWayTypeName(const MmuLookup &l)
  {
    if (auto p = l.iparent()) {
      return getWayTypeName(*p);
    } else {
      return "MmuBase";
    }
  }  

  gc_string getBaseSetTypeName(const MmuLookup &l)
  {
    if (auto p = l.iparent()) {
      return getDefaultSetTypeName(*p);
    } else {
      return "";
    }  
  }

  gc_string getMultihitName(const MmuLookup &l)
  {
    if (l.multi_hit()) {
      return l.name() + "_multi_hit_handler";
    } else {
      return gc_string();
    }
  }

  gc_string getFinalhitName(const MmuLookup &l)
  {
    if (l.final_hit()) {
      return l.name() + "_final_hit_handler";
    } else {
      return gc_string();
    }
  }

  gc_string getArrayTypeName(const MmuLookup &l)
  {
    return l.name() + "_array";
  }

  gc_string getArrayMember()
  {
    return "_entries";
  }

  gc_string getAccessor(const MmuLookup &l,const Context *ctx,bool use_active = true)
  {
    gc_string a = getArrayName(l);

    if (ctx) {
      if (use_active) {
        a = ctx->name() + "()." + a;
      } else {
        a = ctx->name() + "(" + ctx->name() + ".debug_context())." + a;
      }
    }

    return a;
  }

  gc_string getAccessor(const LookupEl &le,bool use_active)
  {
    return getAccessor(*le._lookup,le._ctx,use_active);
  }

  gc_string getFullArrayMember(const MmuLookup &l,const Context *ctx)
  {
    return getAccessor(l,ctx) + "." + getArrayMember();
  }

  gc_string getArrayName(const MmuLookup &l)
  {
    return "_" + l.name();
  }

  gc_string getArrayParmName(const MmuLookup &l)
  {
    return l.name() + "_arg";
  }

  gc_string getProxyObject(const MmuLookup &l)
  {
    return l.name() + "_proxy";
  }

  gc_string getProxyName(const MmuLookup &l)
  {
    return l.name();
  }

  gc_string getTestSetEnableName(const MmuLookup &l,const MmuTestSet &t)
  {
    return l.name() + "_" + t.name() + "_enable";
  }

  gc_string getLookupGetSets(const MmuLookup &l,const Context *ctx)
  {
    return getAccessor(l,ctx) + ".num_sets()";
  }

  gc_string getLookupGetWays(const MmuLookup &l,const Context *ctx)
  {
    return getAccessor(l,ctx) + ".num_ways()";
  }

  gc_string getLookupNumSets(const MmuLookup &l)
  {
    return l.name() + "_num_sets";
  }

  gc_string getLookupSetMask(const MmuLookup &l)
  {
    return l.name() + "_set_mask";
  }

  gc_string getLookupSetShift(const MmuLookup &l)
  {
    return l.name() + "_set_shift";
  }

  gc_string getLookupNumWays(const MmuLookup &l)
  {
    return l.name() + "_num_ways";
  }

  gc_string retryLabel()
  {
    return "RetryTranslation";
  }

  bool has_realpage(const MmuLookup &lookup) 
  {
    if (lookup.realpage()) {
      return true;
    } else {
      ForEach(lookup.lookups(),i) {
        if (has_realpage(*i->second)) {
          return true;
        }
      }
      return false;
    }
  }

  bool hasRealPages(const Core &core)
  {
    if (const MMU *mmu = core.mmu()) {
      const MmuLookupHash &lookups = mmu->all_lookups();
      ForEach(lookups,l) {
        if (has_realpage(*l->second)) return true;
      }
      return false;
    } else {
      return false;
    }
  }

  class GenMmuBase {
  public:
    GenMmuBase (opfxstream &o,ClassData &cd,const ConfigDB &config,const CoreConfig *coreconfig) :
      _o(o), 
      _cd(cd),
      _config(config),
      _coreconfig(coreconfig)
    {};

  protected:
    opfxstream       &_o;

    ClassData        &_cd;
    const ConfigDB   &_config;
    const CoreConfig *_coreconfig;

  public:

    bool transactional_iss() const
    {
      return _config.iss_type() != tNormal;
    }

    bool have_dynamic_tlbs() const
    {
      return _coreconfig && !_coreconfig->dyn_tlbs().empty();
    }

    bool is_dynamic(const MmuLookup &l) const
    {
      return _coreconfig && _coreconfig->dyn_tlbs().count(l.name());
    }

    bool log_unarch() const
    {
      return _config.log_unarch();
    }

    bool parent_has_field(const MmuLookup *parent,const MmuField &field,bool is_way)
    {
      if (parent) {
        if ((is_way) ? 
            (parent->way_fields().find(field.name()) != parent->way_fields().end()) : 
            (parent->set_fields().find(field.name()) != parent->set_fields().end())) {
          return true;
        } else if (parent->iparent()) {
          return parent_has_field(parent->iparent(),field,is_way);
        }
      }
      return false;
    }

    unsigned parent_field_count(const MmuLookup *parent,bool is_way)
    {
      if (parent) {
        if (is_way) {
          return parent->way_fields().size() + parent_field_count(parent->iparent(),is_way);
        } else {
          return parent->set_fields().size() + parent_field_count(parent->iparent(),is_way);
        }
      } else {
        return 0;
      }
    }

    void genFieldData(const MmuLookup &lookup,const MmuFieldHash &fields,const gc_string &basetype)
    {
      static int unarch_attr  = Data::get_attr(Unarchitected);

      const MmuLookup *parent = lookup.iparent();
      
      bool has_base = (parent && !basetype.empty());
      bool is_way   = (&lookup.way_fields() == &fields);

      // Define the way fields.
      ForEach(fields,iter) {
        const MmuField &field = *(iter->second);
        if (!parent || !parent_has_field(parent,field,is_way)) {
          _o << genFieldType(field) << " " << field.name() << ";\n";
        }
      }

      // Define the init/report functions.
      _o << "void init(const FieldData &ld) {\n";
      ForEach(fields,iter) {
        const MmuField &field = *(iter->second);
        if (!parent || !parent_has_field(parent,field,is_way)) {
          _o << field.name() << " = getLookupField(ld,\"" << field.name() << "\"," << genFieldInit(field) << ");\n";
        }
      }
      if (has_base) {
        _o << basetype << "::init(ld);\n";
      }
      _o << "};\n"
         << "void report(FieldData &fd) const {\n";
      if (has_base) {
        _o << basetype << "::report(fd);\n";
      }
      ForEach(fields,iter) {
        const MmuField &field = *(iter->second);
        // Ignore if unarchitected and we're not logging unarchitected items.
        if (!log_unarch() && field.has_attr(unarch_attr)) continue;
        if (!parent || !parent_has_field(parent,field,is_way)) {
          _o << "fd.push_back(FieldPair(\"" << field.name() 
             << "\"," << genFieldIntGet(field) << "));\n";
        }
      }
      _o << "};\n"
         << "static void info(MmuLookupInfo &info) {\n";
      if (has_base) {
        _o << basetype <<"::info(info);\n";
      }
      ForEach(fields,iter) {
        const MmuField &field = *(iter->second);
        if (!parent || !parent_has_field(parent,field,is_way)) {
          const MmuField &field = *(iter->second);
          if (!log_unarch() && field.has_attr(unarch_attr)) continue;
          _o << "info._fields.push_back(MmuFieldInfo());\n"
             << "info._fields.back()._name     = \"" << field.name() << "\";\n"
             << "info._fields.back()._setfield = " << ((is_way) ? "false" : "true") << ";\n"
             << "info._fields.back()._width    = " << field.bits() << ";\n";
          const AttrList &attrs = field.attrs();
          ForEach(attrs,i) {
            AttrsHelper ah(&i->second);
            ah.gen_var(_o);	    
            _o << "info._fields.back()._attrs.push_back(std::make_pair(" 
               << i->first << "," << ah.gen_constr() << "));\n";
          }
          _o << "info._fields.back().finalize();\n";          
        }
      }
      _o <<"}\n";
    }

    // A bit of a hack here- we're assuming that the address-comparison test
    // will be the same for all, so we just use the first.  This might not be
    // valid- we might need to extend this a bit.
    gc_string genEPNcreate (const MmuLookup &lookup)
    { 
      gc_ostringstream epn;
      if (!lookup.tests().empty()) {
        ForEach((lookup.tests().begin()->second->tests()),i) {
          Ptree *test = *i;
          Ptree *testname = get_func_call_name(test);
          if (Eq(testname,"AddrComp")) {
            auto field = lookup.find_field(get_func_call_arg(test,0),0);
            epn << field->name() << ".uint64() << ("<< lookup.sizeshift() << " + " << lookup.pageshift() << ")";
          }
        }
      }
      return epn.str();
    }

    bool hasEPN (const MmuLookup &lookup)
    {
      if (!lookup.tests().empty()) {
        ForEach((lookup.tests().begin()->second->tests()),i) {
          Ptree *test = *i;
          Ptree *testname = get_func_call_name(test);
          if (Eq(testname,"AddrComp")) {
            return true;
          }
        }
      }
      return false;
    }

    gc_string genRPNcreate (const MmuLookup &lookup)
    {
      gc_ostringstream rpn;
      rpn << lookup.realpage()->name() << ".uint64() << (" << lookup.sizeshift() << " + " << lookup.pageshift() << ")";
      return rpn.str();
    }

    bool hasRPN (const MmuLookup &lookup)
    {
      return (lookup.realpage());
    }

    // Write the field structure.  Assumes that the object has an array.
    void genWayStruct(const MmuLookup &lookup)
    {
      const MmuLookup *parent = lookup.iparent();

      gc_string waytype      = getWayTypeName(lookup);
      gc_string wayusertype  = getWayUserTypeName(lookup);
      gc_string basetype     = getBaseWayTypeName(lookup);
      gc_string settype      = getSetTypeName(lookup);

      // Necessary fwd decl.
      if (!lookup.set_fields().empty()) {
        _o << "struct " << settype << ";\n\n";
      }

      // Define the field object 
      _o << "struct " << wayusertype << " : public " << basetype << " {\n";

      genFieldData(lookup,lookup.way_fields(),basetype);

      // Retrieval of specific fields by index.
      _o << "virtual uint64_t getField(unsigned index) const {\n"
         << "switch (index) {\n";
      int count = parent_field_count(parent,true);
      ForEach(lookup.way_fields(),iter) {
        const MmuField &field = *(iter->second);
        if (!parent_has_field(parent,field,true)) {
          _o << "case " << dec << count++ << ": return " << genFieldIntGet(field) << ";\n";
        }
      }
      _o << "default:\n";
      if (parent) {
        _o << "return " << basetype << "::getField(index);\n";
      } else {
        _o << "throw std::runtime_error(\"Bad index for getField for lookup " << lookup.name() << ".\");\n";
      }
      _o << "break;\n"
         << "}\n"
         << "};\n\n"
         << "void reset() {\n";
      ForEach(lookup.way_fields(),iter) {
        const MmuField &field = *(iter->second);
        if (!parent_has_field(parent,field,true)) {
          _o << field.name() << " = " << genFieldInit(field) << ";\n";
        }
      }
      if (parent) {
        _o << basetype << "::reset();\n";
      }
      _o << "};\n";

      // Define the id and name functions.
      _o << "virtual unsigned id() const { return " << lookup.id() << ";};\n"
         << "virtual const char *name() const { return \"" << lookup.name() << "\"; }\n"
         << "virtual bool user_entry() const { return true; };\n"
         << "virtual " << wayusertype << " *clone() const { return new " << wayusertype << "(*this); };\n";

      if (!lookup.is_interface()) {
        _o << "bool valid() const ";
        // If we don't have a valid field, then we consider the object to always be valid.
        // Since this routine is primarily used by the testwriter, it just means that the entire
        // lookup will be dumped unless the user describes how to know if something is valid.
        if (Ptree *valid = lookup.valid_func()._func) {
          // Note:  No translation done here b/c fields are referenced directly and we don't allow
          // register references.
          _o << get_func_body(valid) << "\n";
        } else {
          _o << "{ return true; }\n";
        }
      }

      if (_config.trace_mode()) {
        // This version is called when a user explicitly calls logMmuTranslation.
        _o << "void logMmuTranslation(const IssCore &core,TransType tt,int seq,addr_t ea,addr_t ra) const {\n"
           << "core.logger().log_mmu_translation(tt,seq,-1,-1,ea,ra,this);\n"
           << "};\n";
      }

      if (hasEPN(lookup)) {
        _o << "addr_t epn() const { return " << genEPNcreate(lookup) << "; };\n"
           << "bool   has_epn() const { return true; };\n";
      } else {
        _o << "addr_t epn() const { return 0; };\n"
           << "bool   has_epn() const { return false; };\n";
      }
      if (hasRPN(lookup)) {
        _o << "addr_t rpn() const { return " << genRPNcreate(lookup) << "; };\n"
           << "bool   has_rpn() const { return true; };\n";
      } else {
        _o << "addr_t rpn() const { return 0; };\n"
           << "bool   has_rpn() const { return false; };\n";
      }

      _o << "};\n\n";

      StrList constr_args;
      // This is a wrapper object which is what's actually instantiated.
      _o << "struct " << waytype << " : public " << wayusertype << " {\n"
         << waytype << "(const " << wayusertype << " &x) : " << wayusertype << "(x)";
      if (_config.trace_mode() && !lookup.is_interface()) {
        _o << ", _set(-1), _way(-1)";
      }
      _o << "{};\n";
      if (!lookup.is_interface()) {
        _o << "virtual bool user_entry() const { return false; };\n"
           << "virtual " << waytype << " *clone() const { return new " << waytype << "(*this); };\n";
      }
      // If we're in tracing mode and we have child nodes, we add a child-node
      // pointer to tracking hits.
      if (_config.trace_mode() && !lookup.is_interface()) {
        constr_args.push_back("_set(0)");
        constr_args.push_back("_way(0)");
        _o << "int16_t _set, _way;\n"        
           << "int set() const { return _set; };\n"
           << "int way() const { return _way; };\n"
           << "void logMmuTranslation(const IssCore &core,TransType tt,int seq,addr_t ea,addr_t ra) const {\n"
           << "core.logger().log_mmu_translation(tt,seq,set(),way(),ea,ra,this);\n";
        if (!lookup.lookups().empty()) {
          _o << "if (_last_hit) {\n"
             << "_last_hit->logMmuTranslation(core,tt,seq,ea,ra);\n"
             << "}\n";
        }
        _o << "};\n";
        if (!lookup.lookups().empty()) {
          _o << "const MmuBase *_last_hit;\n";
          constr_args.push_back("_last_hit(0)");
        }
      }

      if (!lookup.set_fields().empty()) {
        constr_args.push_back("_parent_set(0)");
      }

      _o << waytype << "()";
      if (!constr_args.empty()) {
        _o << " : ";
        bool first = true;
        ForEach(constr_args,i) {
          list_sep(_o,first);
          _o << *i;
        }
      }
      _o << " {};\n";

      // We need a pointer to the parent set if we have set fields, in order to
      // do reporting properly.
      if (!lookup.set_fields().empty()) {
        _o << settype << " *_parent_set;\n";
      }

      _o << "void report(FieldData &fd) const {\n";
      // If we're a way and we have set-fields, then we call the parent's
      // report routine, in order to get values for those fields.
      if (!lookup.set_fields().empty()) {
        _o << "if (_parent_set) {\n"
           << "_parent_set->report(fd);\n"
           << "}\n";
      }
      _o << wayusertype << "::report(fd);\n"
         << "}\n"
         << "};\n\n";
    }

    void genSetStruct(const MmuLookup &lookup)
    {
      // We only create a set object if we have more than one way per set or
      // this is an interface object, in which case we create it for
      // consistency.
      if (has_set_type(lookup)) {
        const MmuLookup *parent = lookup.iparent();

        bool is_dyn = is_dynamic(lookup);
        bool is_interface = lookup.is_interface();

        gc_string waytype  = getWayTypeName(lookup);
        gc_string settype  = getSetTypeName(lookup);
        gc_string basetype = getBaseSetTypeName(lookup);
        _o << "struct " << settype << ((!basetype.empty()) ? " : " : "") << basetype << " {\n";
        genFieldData(lookup,lookup.set_fields(),basetype);

        if (!is_interface) {
          // We need a constructor that pushes a pointer of the set to each way,
          // so that the way can retrieve set fields (if necessary).
          if (is_dyn) {
            _o << settype << "() : _ways(0) {\n";
          } else {
            _o << settype << "() : _ways(new " << waytype << "[" << lookup.ways() << "]) {\n";
          }
          if (!lookup.set_fields().empty()) {
            _o << "for (int i = 0; i != " << lookup.ways() << "; ++i) {\n"
               << "_ways[i]._parent_set = this;\n"
               << "}\n";
          }
          _o << "}\n"
             << "~" << settype << "() { delete [] _ways; };\n";
        }

        _o << "void reset(bool realloc,unsigned numways,uint16_t set) {\n";
        if (is_dyn && !is_interface) {
          _o << "if (realloc) {\n"
             << "delete [] _ways;\n"
             << "_ways = new " << waytype << "[numways];\n"
             << "}\n";
        }
        ForEach(lookup.set_fields(),iter) {
          const MmuField &field = *(iter->second);
          if (!parent_has_field(parent,field,false)) {
            _o << field.name() << " = " << genFieldInit(field) << ";\n";
          }
        }
        if (parent) {
          _o << basetype << "::reset(realloc,numways,set);\n";
        }
        if (lookup.ways() && !is_interface) {
          _o << dec << "for (unsigned way = 0; way != numways; ++way) {\n"
             << "_ways[way].reset();\n";
          if (_config.trace_mode()) {
            _o << "_ways[way]._set = set;\n"
               << "_ways[way]._way = way;\n";
          }
          _o << "}\n";
        }
        _o << "};\n";
      
        if (!is_interface) {
          _o << waytype << " &operator[](unsigned i) { return _ways[i]; }\n"
             << "const " << waytype << " &operator[](unsigned i) const { return _ways[i]; }\n\n"
             << waytype << " *_ways;\n";
        }

        _o << "};\n\n";
      }
    }

    bool oneDimArray(const MmuLookup &lookup) 
    {
      return (lookup.fully_assoc() || lookup.ways() == 1);
    }

    void genReportCall(const MmuLookup &lookup,const gc_string &set_entry,const gc_string &way_entry,const gc_string &indices)
    {
      _o << "fd.clear();\n";
      if (!way_entry.empty()) {
        _o << way_entry << ".report(fd);\n";
      }
      if (hasRPN(lookup)) {
        _o << "const addr_t rpn =" << way_entry << ".rpn();\n"
           << "const bool  hrpn =" << way_entry << ".has_rpn();\n";
      } else {
        _o << "const addr_t rpn = 0;\n"
           << "const bool  hrpn = false;\n";
      }
      if (hasEPN(lookup)) {
        _o << "const addr_t epn =" << way_entry << ".epn();\n"
           << "const bool  hepn =" << way_entry << ".has_epn();\n";
      } else {
        _o << "const addr_t epn = 0;\n"
           << "const bool  hepn = false;\n";
      }

      gc_string entry = ((way_entry.empty() ? set_entry : way_entry));
      _o << "rb.report_mmu(n," << entry << "," << indices << ",fd," << entry << ".valid(),shared,epn, hepn, rpn, hrpn);\n";
    }

    void genLookupEnumId(const MmuLookup &lookup,StrSet &processed)
    {
      if (processed.count(lookup.name())) {
        return;
      }
      if (lookup.iparent()) {
        genLookupEnumId(*lookup.iparent(),processed);
      }
      processed.insert(lookup.name());
      _o << lookup.enum_name() << " = " << lookup.id() << ",\n";
    }

    void genLookupArrayInfo(const MmuLookup &lookup)
    {
      _o << "static void info(MmuLookupInfo &info,bool shared) {\n"
         << "info._name = \"" << lookup.name() << "\";\n"
         << "info._id   = " << lookup.id() << ";\n"
         << "info._sets = " << lookup.sets() << ";\n";
      if (lookup.sizetype() == MmuLookup::Const) {
        _o << "info._pagesize = " << lookup.pagesize() << ";\n";
      } else {
        _o << "info._pagesize = 0;\n";
      }
      _o << "info._pageshift = " << lookup.pageshift() << ";\n";
      if (has_set_type(lookup)) {
        _o << getSetTypeName(lookup) << "::info(info);\n";
      }
      _o << getWayTypeName(lookup) << "::info(info);\n";
      _o << "info._ways = " << lookup.ways() << ";\n"
         << "info._shared = shared;\n";
      const AttrList &attrs = lookup.attrs();
      ForEach(attrs,i) {
        AttrsHelper ah(&i->second);
        ah.gen_var(_o);	    
        _o << "info._attrs.push_back(std::make_pair(" 
           << i->first << "," << ah.gen_constr() << "));\n";
      }
      _o << "};\n\n";
    }

    void genLookupDataStructure(const MmuLookup &lookup,const Resources &shared,StrSet &processed,bool in_ctx)
    {
      // Don't write it out if we've already handled it.
      if (processed.count(lookup.name())) {
        return;
      }
      if (lookup.iparent()) {
        genLookupDataStructure(*lookup.iparent(),shared,processed,in_ctx);
      }
      processed.insert(lookup.name());
      if (lookup.is_interface()) {
        // For interfaces, create the base class type only.
        genWayStruct(lookup);
        genSetStruct(lookup);
      } else if (lookup.has_array()) {

        gc_string settype = getSetTypeName(lookup);
        gc_string fobj    = getArrayTypeName(lookup);
        gc_string farray  = getArrayName(lookup);
      
        gc_string array_name = getArrayMember();
        bool fully_assoc = lookup.fully_assoc();
        int num_sets = lookup.sets();
        int num_ways = lookup.ways();

        bool is_dyn = is_dynamic(lookup);

        bool is_shared;

        gc_string num_elem = (num_ways == 1 && !is_dyn) ? "num_sets()" : "num_ways()";

        if (shared.contains(lookup)) {
          is_shared = true;
          // It is shared.  Create a pointer to the data object.
          gc_string fparm = getArrayParmName(lookup);

          gc_string carg = fobj + " &" + fparm;

          _cd._member_vars.push_back(fobj + " &" + farray);
          _cd._constr_args.push_back( carg );
          _cd._constr_inits.push_back(farray + "(" + fparm + ")");
        } else {
          is_shared = false;
          // Not shared at this level in the hierarchy- create the actual data
          // structure.

          genWayStruct(lookup);
          genSetStruct(lookup);

          _o << "struct " << fobj << " {\n";

          if (is_dyn) {
            if (!fully_assoc) {
              _o << "unsigned _num_sets;\n";
            }
            _o << "unsigned _num_ways;\n";
          }
          if (fully_assoc) {
            // For fully-associative or arrays with a single array, we just need a
            // single array of objects.  Storage is in a simple C array of field
            // objects.  Note: This implementation is fairly simplistic: We'll be
            // doing linear searches on this array.  In the future, we'll probably
            // want something more sophisticated in the future, such as a trie.
            _o << settype << " " << array_name << ";\n";
          } else {
            // If not fully associative, then we have a two-dimensional array.
            // The outer dimension represents sets, the inner dimension represents ways.
            _o << settype << " *" << array_name << ";\n";
          }

          if (fully_assoc) {
            if (is_dyn) {
              _o << fobj << "() : _num_ways(0) {}\n";
            }
          } else {
            if (is_dyn) {
              _o << fobj << "() : _num_ways(0), _num_sets(0), " << array_name << "(0) {}\n";
            } else {
              _o << fobj << "() : " << array_name << "(new " << settype << "[" << num_sets << "]) {}\n";
            }
            _o << "~" << fobj << "() { delete [] " << array_name << "; }\n";
          }

          // Now generate the set/get functions for this array.  We do this for non-shared and
          // shared resources so that any visible lookup can be accessed at any point in the
          // hierarchy.

          genLookupArrayInfo(lookup);

          gc_string set_entry = array_name + ( (fully_assoc) ? "" : "[set]");
          gc_string way_entry;
          gc_string reset_arg;
          if (num_ways > 1) {
            way_entry = set_entry + "[way]";
          }

          if (has_set_type(lookup)) {
            reset_arg = "realloc,numways,set";
          }

          if (is_dyn) {
            if (!fully_assoc) {
              _o << "unsigned num_sets() const { return _num_sets; }\n";
            } else {
              _o << "unsigned num_sets() const { return " << num_sets << "; }\n";
            }
            _o << "unsigned num_ways() const { return _num_ways; }\n";
          } else {
            _o << "unsigned num_sets() const { return " << num_sets << "; }\n"
               << "unsigned num_ways() const { return " << num_ways << "; }\n";
          }

          _o << "void set (unsigned set,unsigned way,const FieldData &ld) {\n"
             << "if (set >= num_sets()) {\n"
             << "throw std::runtime_error(\"Bad set specified.\");\n"
             << "}\n"
             << "if (way >= num_ways()) {\n"
             << "throw std::runtime_error(\"Bad way specified.\");\n"
             << "}\n"
             <<  set_entry << ".init(ld);\n";
          if (!way_entry.empty()) {
            _o <<  way_entry << ".init(ld);\n";
          }
          _o << "};\n\n"

             << "void get (unsigned set,unsigned way,FieldData &fd) const {\n"
             << "if (set >= num_sets()) {\n"
             << "throw std::runtime_error(\"Bad set specified.\");\n"
             << "}\n"
             << "if (way >= num_ways()) {\n"
             << "throw std::runtime_error(\"Bad way specified.\");\n"
             << "}\n"
             << "fd.clear();\n"
             << set_entry << ".report(fd);\n";
          if (!way_entry.empty()) {
            _o << way_entry << ".report(fd);\n";
          }
          _o << "};\n\n"

             << "void reset(bool realloc,unsigned numsets,unsigned numways) {\n";
          if (is_dyn) {
            _o << "if (realloc) {\n"
               << "_num_ways = numways;\n";
            if (!fully_assoc) {
              _o << "_num_sets = numsets;\n"
                 << "delete " << array_name << ";\n"
                 << array_name << " = new " << settype << "[numsets];\n";
            }
            _o << "}\n";
          }
          if (fully_assoc) {
            if (has_set_type(lookup)) {
              _o << array_name << ".reset(realloc,numways,0);\n";
            } else {
              _o << array_name << ".reset();\n";
            }
          } else {
            _o << "for (unsigned set = 0; set != num_sets(); ++set) {\n"
               << array_name << "[set].reset(" << reset_arg << ");\n"
               << "}\n";
          }
          _o << "}\n\n"

             << "void show (const IssNode *n,ReportBase &rb,bool shared) const {\n"
             << "FieldData fd;\n";
          if (fully_assoc) {
            _o << "for (unsigned way = 0; way != " << num_elem << "; ++way) {\n";
            genReportCall(lookup,set_entry,way_entry,"0,way");
            _o << "}\n";
          } else if (num_ways == 1 && !is_dyn) {
            _o << "for (unsigned set = 0; set != " << num_elem << "; ++set) {\n";
            genReportCall(lookup,set_entry,way_entry,"0,set");
            _o << "}\n";
          } else {
            _o << "for (unsigned set = 0; set != num_sets(); ++set) {\n"
               << "for (unsigned way = 0; way != num_ways(); ++way) {\n";
            genReportCall(lookup,set_entry,way_entry,"set,way");
            _o << "}\n"
               << "}\n";
          }
          _o << "};\n"
             << "};\n\n";

          if (!in_ctx) {
            _cd._member_vars.push_back(fobj + " " + farray);
          }
        }

      } else {
        // If we don't have an array, but we do have way-fields, then we define
        // the data structure anyway.  It may be used by the miss function.
        // Always define the basic structure, so that we have a type associated
        // with this tlb which can be used by the miss function.
        genWayStruct(lookup); 
        if (!lookup.way_fields().empty()) {
          // For introspection purposes, declare the array type w/just the info function.
          gc_string fobj    = getArrayTypeName(lookup);        
          _o << "struct " << fobj << " {\n";
          genLookupArrayInfo(lookup);
          _o << "};\n\n";
        }
      }
    }

    void genLookupDataStructures(const LookupList &lookups,const Resources &shared)
    {
      _o << "//\n"
         << "// MMU data structures.\n"
         << "//\n\n";
      // For each lookup, create an enumeration entry of its id, so that their
      // IDs can be referred to symbolically.
      _o << "enum TlbIds {\n";
      StrSet processed;
      ForEach(lookups,i) {
        const MmuLookup &lookup = *i->_lookup;
        genLookupEnumId(lookup,processed);
      }
      _o << "};\n\n";

      // We iterate over all lookups, but we write them out in a depth-first
      // manner, in order to ensure that all base lookups are written out first,
      // before things which inherit from them.
      processed.clear();
      ForEach(lookups,i) {
        const MmuLookup &lookup = *i->_lookup;
        bool in_ctx = i->_ctx;
        genLookupDataStructure(lookup,shared,processed,in_ctx);
      }
    }

    // A lookup is included in the various helper functions if it isn't an
    // interface and has an array.
    bool includeInHelpers(const MmuLookup &l)
    {
      return !l.is_interface() && l.has_array();
    }

    void genGetSetFuncs(const LookupList &lookups,const Resources &shared,bool is_core)
    {
      // Function for setting field information.
      _o << "void setMmuLookup(const std::string &lookup,unsigned set,unsigned way,const FieldData &fd) {\n";
      ForEach (lookups,i) {
        const LookupEl &le = *i;
        const MmuLookup &l = *le._lookup;
        if (!includeInHelpers(l)) continue;
        gc_string fname = getAccessor(le,false);
        _o << "if (lookup == \"" << l.name() << "\") {\n"
           << fname << ".set(set,way,fd);\n"
           << "} else ";
      }
      _o << " {\n"
         << "throw std::runtime_error(\"Bad MMU lookup item:  \"+lookup);\n"
         << "}\n"
         << "};\n\n";

      // Function for getting field information.
      _o << "void getMmuLookup(const std::string &lookup,unsigned set,unsigned way,FieldData &fd) const {\n";
      ForEach (lookups,i) {
        const LookupEl &le = *i;
        const MmuLookup &l = *le._lookup;
        if (!includeInHelpers(l)) continue;
        gc_string fname = getAccessor(le,false);
        _o << "if (lookup == \"" << l.name() << "\") {\n"
           << fname << ".get(set,way,fd);\n"
           << "} else ";
      }
      _o << " {\n"
         << "throw std::runtime_error(\"Bad MMU lookup item:  \"+lookup);\n"
         << "}\n"
         << "};\n\n";
    }

    void genInfoFunc(const LookupList &lookups,const Resources &shared,bool is_core)
    {
      // Function for getting information about the MMU.
      int num_lookups = 0;
      ForEach(lookups,l_iter) {
        const MmuLookup &l = *l_iter->_lookup;
        if (!l.is_interface()) {
          ++num_lookups;
        }
      }
      _o << "static MmuInfo initMmuInfo() {\n"
         << "MmuInfo info(" << num_lookups << ");\n";
      int c = 0;
      ForEach (lookups,i) {
        const MmuLookup &l = *i->_lookup;
        bool sh = shared.contains(l);
        if (l.is_interface()) continue;
        if (l.set_fields().empty() && l.way_fields().empty()) continue;
        gc_string ftype = getArrayTypeName(l);
        _o << ftype << "::info(info[" << c++ << "]," << sh << ");\n";
      }
      _o << "return info;\n"
         << "};\n\n"
         << "static const MmuInfo &baseMmuInfo() {\n"
         << "static MmuInfo info = initMmuInfo();\n"
         << "return info;\n"
         << "};\n\n"
         << "const MmuInfo &getMmuInfo() const {\n"
         << "return baseMmuInfo();\n"
         << "};\n\n";
    }

    void genShowFunc(const LookupList &lookups,const Resources &shared,bool is_core)
    {
      // Function for listing all MMU information.
      _o << "void showMmu(ReportBase &rb) const {\n";
      // First list all non-context lookups.
      ForEach (lookups,i) {
        const LookupEl &le = *i;
        const MmuLookup &l = *i->_lookup;
        if (le._ctx) continue;
        if (!includeInHelpers(l)) continue;
        bool sh = shared.contains(l);
        gc_string fname = getArrayName(l);
        _o << fname << ".show(this,rb," << sh << ");\n";
      }    
      bool had_ctx = false;
      ForEach (lookups,i) {
        const LookupEl &le = *i;
        const MmuLookup &l = *le._lookup;
        const Context *ctx = le._ctx;
        if (!ctx) continue;
        if (!includeInHelpers(l)) continue;
        bool sh = shared.contains(l);
        gc_string fname = getArrayName(l);

        had_ctx = true;
        gc_string index = "ctx_index";
        _o << "for (unsigned " << index << " = 0; " << index << " != " << ctx->num_contexts() << "; ++" << index << ") {\n"
           << "rb.report_ctx_change(this," << ctx->id() << ",\"" << ctx->name() << "\"," << index << ");\n"
           << ctx->name() << "(" << index << ")." << fname << ".show(this,rb," << sh << ");\n"
           << "}\n";
      }
      if (had_ctx) {
        _o << "rb.report_ctx_off(this);\n";
      }

      _o << "};\n\n";
    }

    void genReset(const LookupList &lookups,const Resources &shared,bool is_core)
    {
      // Skip if we have nothing to reset.
      if (!lookups.empty()) {
        _o << "struct ResetMmuLogger : public ReportBase {\n"
           << "virtual void report_mmu (const IssNode *n,const MmuBase &mb,unsigned set, unsigned way,\n"
           << "const FieldData &fd,bool valid,bool shared,\n"
           << "addr_t epn,bool valid_epn,addr_t rpn, bool valid_rpn) {\n"
           << "n->logger().log_mmu_translation(WriteTrans,0,set,way,0,0,&mb);\n"      
           << "}\n"
           << "};\n\n";

        gc_list<pair<gc_string,const Context *> > resets;

        // First, generate per-lookup reset functions.  For context-specific
        // lookups, the function invalidates the specified context.
        ForEach (lookups,i) {
          const MmuLookup &l = *i->_lookup;
          const Context *ctx = i->_ctx;
          if (!includeInHelpers(l)) continue;
          gc_string fname = getArrayName(l);
          bool is_dyn = is_dynamic(l);

          if (shared.contains(l) && is_core) continue;

          gc_string num_sets,num_ways;

          gc_string name = l.name() + "_reset";
          resets.push_back(make_pair(name,ctx));

          _o << "void " << name << " (bool realloc";
          if (ctx) {
            _o << ",unsigned ctx_index";
          }
          _o << ") {\n";
          if (!l.fully_assoc()) {
            if (is_dyn) {
              num_sets = getLookupNumSets(l);
              gc_string num_sets_v = uitogs(l.sets());
              _cd._member_vars.push_back("unsigned "+num_sets);
              _cd._constr_inits.push_back(num_sets+"(" + num_sets_v + ")");

              gc_string set_mask = getLookupSetMask(l);
              gc_string set_mask_v = uitogs(l.sets()-1);
              _cd._member_vars.push_back("unsigned "+set_mask);
              _cd._constr_inits.push_back(set_mask+"(" + set_mask_v + ")");

              gc_string set_shift = getLookupSetShift(l);
              gc_string set_shift_v = uitogs(trailing_zeros(l.sets()));
              _cd._member_vars.push_back("unsigned "+set_shift);
              _cd._constr_inits.push_back(set_shift+"(" + set_shift_v + ")");
            } else {
              num_sets = uitogs(l.sets());
            }
          } else {
            num_sets = uitogs(l.sets());
          }

          if (is_dyn) {
            num_ways = getLookupNumWays(l);
            gc_string num_ways_v = uitogs(l.ways());
            _cd._member_vars.push_back("unsigned "+num_ways);
            _cd._constr_inits.push_back(num_ways+"(" + num_ways_v + ")");
          } else {
            num_ways = uitogs(l.ways());
          }

          gc_string cn;
          if (ctx) {
            cn = ctx->name() + "(ctx_index).";
          }

          _o << cn << fname << ".reset(realloc," << num_sets << "," << num_ways << ");\n"
             << "}\n\n";
        }

        bool did_reset = false;
        // Function for reseting an MMU.  This does just the basic reset and
        // does not call the reset hook, if one exists.  That's called on a
        // per-core basis, so that the core's registers are accessible.  We do
        // not reset shared tlbs in the core itself, since the system will do
        // the reset.
        _o << "void resetMmu(bool realloc,bool log) {\n";
        ForEach (resets,r) {
          const gc_string &name = r->first;
          const Context *ctx = r->second;

          did_reset = true;

          if (ctx) {
            _o << "for (unsigned ctx_index = 0; ctx_index != " << ctx->num_contexts() << "; ++ctx_index) {\n"
               << name << "(realloc,ctx_index);\n"
               << "}\n";
          } else {
            _o << name << "(realloc);\n";
          }
        }

        if (did_reset) {
          _o << "if (Tracing && log && (tracing_on(ttMmuTranslation))) {\n"
             << "ResetMmuLogger rl;\n"
             << "showMmu(rl);\n"
             << "}\n";
        }
        _o << "};\n\n";
        
        _cd._reset_stmts.push_back("resetMmu(init_reset,log)");
      }
    }

    // Generate helper functions.
    void genHelpers(const LookupList &lookups,const Resources &shared,bool is_core)
    {
      genGetSetFuncs(lookups,shared,is_core);
      genInfoFunc(lookups,shared,is_core);
      genShowFunc(lookups,shared,is_core);
      genReset(lookups,shared,is_core);
    }

  };

  class GenMmu : public GenMmuBase {
  public:
    GenMmu(opfxstream &o,const MMU &mmu,
           const Core &core,InstrWalker &walker,
           const gc_string &filename,const SlotAlloc &mem_comm_slots,
           ClassData &cd,const ConfigDB &config,const CoreConfig &coreconfig) :
      GenMmuBase(o,cd,config,&coreconfig), 
      _mmu(mmu),
      _core(core),
      _walker(walker),
      _mem_comm_slots(mem_comm_slots),
      _filename(filename)
    {};

  private:

    const MMU        &_mmu;
    const Core       &_core;
    InstrWalker      &_walker;
    const SlotAlloc  &_mem_comm_slots;
    const gc_string  &_filename;

  public:

    // Translation cache type.  Can be either none, a page-based approach, or a
    // bounds check.
    enum TransCacheType { None, Page, Bounded };

    bool dmi_cache_support() const
    {
      return _coreconfig->dmi_cache_support();
    }

    bool jit_support() const
    {
      return _config.jit_support();
    }

    bool dc_support() const
    {
      return _config.dc_support();
    }

    void genFunc(const gc_string &fname,Environment *env,Ptree *func)
    {
      adl::genFunc(_o,_config,_walker,_filename,fname,env,func);
    }

    bool haveExceptions() const
    {
      return !_core.exceptions().empty();
    }

    gc_string accessPfx(AccessType type) 
    {
      switch (type) {
      case aInstr:
        return "_instr";
      case aLoad:
        return "_load";
      case aStore:
        return "_store";
      }
      assert(0);
    }

    // Generate a mask for extracting the effective-page from an address.
    void genAddrMask(const MmuLookup &lookup,const gc_string &entry)
    {
      if (lookup.sizetype() == MmuLookup::None) {
        return;
      }

      _o << "const addr_t mask = ";
      switch (lookup.sizetype()) {
      case MmuLookup::Const:
        _o << "~(" << lookup.pagesize() << " - 1);\n";
        break;
      case MmuLookup::BitSize:
        _o << "(~((1ULL << (" << lookup.sizescale() << " * (" << entry << "." << lookup.sizefield()->name() << ".uint64() + "
           << lookup.sizeoffset() << "))) - 1) << " << lookup.sizeshift() << ");\n";
        break;
      case MmuLookup::LeftMask:
        // Left mask- invert the mask, mask the address, then compare to the
        // relevant effective-page field.
        _o << "(~" << entry << "." << lookup.sizefield() << ".uint64() << " << lookup.sizeshift() << ");\n";
        break;
      default:
        // For type none, don't create anything.
        break;
      } 
    }

    // Generates the actual tests, for a given test set.
    void genTests(const MmuLookup &lookup,const MmuTestSet &testset,const gc_string &entry)
    {
      gc_string miss_action;
      if (lookup.has_array()) {
        if (lookup.ways() == 1 && !is_dynamic(lookup)) {
          miss_action = "break";
        } else {
          miss_action = "continue";
        }
      } else {
        miss_action = "return false";
      }

      ForEach(testset.tests(),i) {
        Ptree *test = *i;
        Ptree *testname = get_func_call_name(test);
        if (Eq(testname,"Bounded")) {
          // Test against lower/upper-bound expressions.
          MmuWalker walker(_core.env(),lookup,entry,_config.library_mode());
          Ptree *lb_exp = walker.Translate(_walker.translateCode(lookup.env(),get_func_call_arg(test,0)));
          Ptree *ub_exp = walker.Translate(_walker.translateCode(lookup.env(),get_func_call_arg(test,1)));
          Ptree *bm_exp = walker.Translate(_walker.translateCode(lookup.env(),get_func_call_arg(test,2)));
          Ptree *amod  = (bm_exp) ? Ptree::Make("(ea & (%p))",bm_exp) : Ptree::Make("ea");

          _o << "if ( ((" << lb_exp << ") > " << amod << ") || ((" << ub_exp << ") < " << amod << ")) " << miss_action << ";\n\n";
        } else if (Eq(testname,"AddrComp")) {
          // Address comparison- we mask the relevant portion of the address and
          // compare it to the specified TLB entry's field.
          auto field = lookup.find_field(get_func_call_arg(test,0),0);

          _o << "if ( (ea & mask) != ((" << entry << "." << field->name() << ".uint64() << ("
             << lookup.sizeshift() << " + " << lookup.pageshift() << ")) & mask))  " << miss_action << ";\n\n";
        } else if (Eq(testname,"AddrIndex")) {
          // This is skipped- it's only allowed for single-way arrays and will be
          // used to directly index into the element.
          continue;
        } else if (Eq(testname,"Compare")) {
          // Comparison- we compare the field to each of its arguments.  If any
          // match, then the comparison succeeds.
          auto field = lookup.find_field(get_func_call_arg(test,0),0);
          gc_string fn = entry + "." + field->name();
          _o << "if (! (";
          int numargs = get_func_call_numargs(test);
          bool first = true;
          for (int a = 1; a != numargs; ++a) {
            pfx_sep(_o," || ",first);
            _o << "(" << fn << " == (" << _walker.translateCode(lookup.env(),get_func_call_arg(test,a),false) << "))";
          }
          _o << "))  " << miss_action << ";\n\n";
        } else if (Eq(testname,"Check")) {
          // Check- we just execute the expression and match only if the
          // expression evaluates to true.
          // The expression must be modified so that all fields reference the actual entry.
          MmuWalker walker(_core.env(),lookup,entry,_config.library_mode());
          Ptree *exp = walker.Translate(_walker.translateCode(lookup.env(),get_func_call_arg(test,0)));
          _o << "if (!(" << exp << ")) " << miss_action << ";\n\n";
        }
      }
    }

    // Generate the matching tests for a lookup object.
    void genTests(const MmuLookup &lookup,const gc_string &entry)
    {
      // If we have only a single test-set, then we can generate this w/o any
      // predicate tests.  Otherwise, we have to test each predicate, then
      // generate the corresponding test set.
      if (lookup.tests().size() == 1) {
        genTests(lookup,*(lookup.tests().begin()->second),entry);
      } else {
        // Everything but the last element in the test-order should have a
        // predicate.  This should have already been checked by the front-end.
        auto last_testset = lookup.test_order().end();
        --last_testset;
        
        ForEach(lookup.test_order(),iter) {
          const MmuTestSet &testset = **iter;

          if (iter != last_testset) {
            _o << "if (" << getTestSetEnableName(lookup,testset) << "(tt)) {\n";
            genTests(lookup,testset,entry);
            _o << "}\n";
          } else {
            _o << "else {\n";
            genTests(lookup,testset,entry);
            _o << "}\n";
          }
        }
      }
    }
  
    // Writes the code for creating an ra from an ea and a lookup field entry.
    void genAddrConversion(const MmuLookup &lookup,const gc_string &entry,bool ns,bool write_epn = true)
    {
      if (lookup.sizetype() != MmuLookup::None) {
        _o << "am  = mask;\n"
           << "rpn = ( "<< entry << "." << genRPNcreate(lookup) << ") & mask;\n";
        if (write_epn) {
          _o << "epn = (ea & mask);\n";
        }
      } else if (lookup.bounded() && !ns) {
        MmuWalker walker(_core.env(),lookup,entry,_config.library_mode());
        Ptree *lb_exp = walker.Translate(_walker.translateCode(lookup.env(),lookup.lower_bound()));
        Ptree *ub_exp = walker.Translate(_walker.translateCode(lookup.env(),lookup.upper_bound()));
        Ptree *bm_exp = walker.Translate(_walker.translateCode(lookup.env(),lookup.bound_mask()));

        _o << "lb = std::max(to_uint64(" << lb_exp << "),lb);\n"
           << "ub = std::min(to_uint64(" << ub_exp << "),ub);\n";
        if (bm_exp) {
          _o << "bm = std::max(to_uint64(" << bm_exp << "),bm);\n";
        } else {
          _o << "bm = ((uint64_t)-1);\n";
        }
      }
    }

    // Writes code to log a translation.
    void genLogMmuCall(const MmuLookup &lookup,const StrList &parents,const gc_string &entry)
    {
      if (lookup.lookups().empty() && parents.empty()) {
        gc_string ra;
        if (lookup.bounded()) {
          ra = "ea";
        } else {
          ra = "make_addr(rpn,am,ea)";
        }
        _o << "logMmuTranslation(&" << entry << ",tt,seq,false,ea," << ra << ");\n";
      }
    }

    enum PermType { ExecPerm, LoadPerm, StorePerm };

    gc_string permSuffix(PermType type)
    {
      switch (type) {
      case ExecPerm:
        return "execute";
      case LoadPerm:
        return "load";
      case StorePerm:
        return "store";
      }
      assert(0);
    }

    gc_string permName(const MmuLookup &lookup,PermType type)
    {
      return lookup.name() + "_check_" + permSuffix(type);
    }

    // A permission check function consists of three actual functions:
    // 1.  The requirement checker.
    // 2.  The fail function.
    // 3.  The main entry point which calls the requirement function and then
    //     calls the fail function if there's a problem.
    void genPermFunc(Ptree *perm,const MmuLookup &lookup,const StrList &parents,PermType type)
    {
      if (perm) {
        _o << "void " << permName(lookup,type) << "(" << constify_args(perm,parents) << ")\n"
           << _walker.translateCode(lookup.env(),get_func_body(perm),false) << "\n\n";
      }
    }

    gc_string hitFuncName(const MmuLookup &lookup) 
    {
      return lookup.name() + "_hit_func";
    }

    gc_string missFuncName(const MmuLookup &lookup) 
    {
      return lookup.name() + "_miss_func";
    }

    gc_string missObjName(const MmuLookup &lookup)
    {
      return "__" + lookup.name() + "_miss_obj";
    }

    // Generate the various permission checking functions.
    void genTransFuncs(const MmuLookup &lookup,const StrList &parents)
    {
      StrList newparents = parents;
      gc_string misstype = getWayTypeName(lookup);
      gc_string ftype = getWayUserTypeName(lookup);
      newparents.push_front(ftype);

      // Generate permission checking functions, if they exist.
      genPermFunc(lookup.exec_perm()._func,lookup,newparents,ExecPerm);
      genPermFunc(lookup.load_perm()._func,lookup,newparents,LoadPerm);
      genPermFunc(lookup.store_perm()._func,lookup,newparents,StorePerm);

      // We don't care about recording referenced registers for these functions
      // because they don't affect MMU operation wrt. caching.
      bool orr = _walker.setRegRecord(false);

      // Generate hit function, if it exists.
      if (auto hf = lookup.hit_func()) {
        _o << "void " << hitFuncName(lookup) << "(" << constify_args(hf._func,newparents,1) << ")\n"
           << _walker.translateCode(lookup.env(),get_func_body(hf._func),false) << "\n\n";
      }

      // If we have a miss function, we also create a temporary object for storing
      // its results.
      if (auto mf = lookup.miss_func()) {
        _o << misstype << " " << missObjName(lookup) << ";\n\n"
           << ftype << " " << missFuncName(lookup) << "(" << get_func_args(mf._func) << ")\n"
           << _walker.translateCode(lookup.env(),get_func_body(mf._func),false) << "\n\n";
      }

      _walker.setRegRecord(orr);
    }

    void genPermCall(const MmuLookup &lookup,PermType type,const StrList &parents)
    {
      Ptree *perm = 
        (type == ExecPerm) ? lookup.exec_perm()._func :
        (type == LoadPerm) ? lookup.load_perm()._func :
        lookup.store_perm()._func;

      if (perm) {
        _o << permName(lookup,type) << "(";
        ForEach(parents,i) {
          _o << *i << ",";
        }
        _o << "ea,seq);\n";
      }
    }

    void genPermChecks(const MmuLookup &lookup,const StrList &parents)
    {
      if (lookup.type() == MmuLookup::Instr) {
        // Lookup is only an instruction lookup.
        genPermCall(lookup,ExecPerm,parents);
      } else {
        _o << "switch (tt) {\n";
        if ( lookup.type() == MmuLookup::Both ) {
          _o << "case InstrTrans:\n";
          genPermCall(lookup,ExecPerm,parents);
          _o << "break;\n";
        }
        _o << "case LoadTrans:\n";
        genPermCall(lookup,LoadPerm,parents);
        _o << "break;\n"
           << "case StoreTrans:\n";
        genPermCall(lookup,StorePerm,parents);
        _o << "break;\n"
           << "default:\n"
           << "break;\n"
           << "}\n";
      }
    }

    unsigned startOfMask(addr_t x)
    {
      return __builtin_clzll(x);
    }

    // Generates set selection logic if we have an address-index test.
    void genAddrIndex(const MmuLookup &lookup)
    {
      gc_string pfx;
      if (lookup.tests().size() > 1) {
        _o << "int set = 0;\n";
        pfx = "";
      } else {
        pfx = "int ";
      }

      auto last_testset = lookup.test_order().end();
      --last_testset;
      bool conditional_tests = (lookup.test_order().size() > 1);

      ForEach(lookup.test_order(),l_iter) {
        const MmuTestSet &testset = **l_iter;

        ForEach(testset.tests(),i) {
          Ptree *test = *i;
          Ptree *testname = get_func_call_name(test);
          if (Eq(testname,"AddrIndex")) {
            // Only item that we care about.
            int sb = get_uint(get_func_call_arg(test,0),"AddrIndex start bit",lookup.env());
            int eb = get_uint(get_func_call_arg(test,1),"AddrIndex stop bit",lookup.env());
            
            // Get the ea_mask from the core.
            const AddrMask &eamask = _core.ea_mask();
            assert(eamask.is_const());
            
            int som = startOfMask(eamask.initial_value());
            // Get position of start of mask, then adjust starting indices accordingly.
            sb = som + sb;
            eb = som + eb;
            
            unsigned mask = ( 1 << (eb - sb + 1) ) - 1;

            bool h_e = false;
            if (conditional_tests) {
              if (l_iter != last_testset) {
                _o << "if (" << getTestSetEnableName(lookup,testset) << "(tt)) {\n";
              } else {
                _o << "else {\n";
              }
              h_e = true;
            }
            _o << pfx << " set = (ea >> ((sizeof(addr_t)*8) - " << eb+1 << ")) & " << mask << ";\n";
            if (h_e) {
              _o << "}\n";
            }
          }
        }
      }
    }

    // Writes a call to a lookup's child lookup.  This includes not only its parms
    // but also the list of translations made so far.
    void writeChildTransHeaderCall(const MmuLookup &lookup,const StrList &parents)
    {
      _o << "if (" << lookup.name() << "_translate(ea,";
      if (lookup.bounded()) {
        _o << "lb,ub";
        if (lookup.bound_mask()) {
          _o << ",bm";
        }
      } else {
        _o << "rpn,epn,am";
      }
      _o << ",tt,seq";
      ForEach(parents,i) {
        _o << "," << *i;
      }
      _o << "))";
    }

    gc_string getLastHitName(const MmuLookup &lookup)
    {
      return "_" + lookup.name() + "_last_hit";
    }

    typedef vector<MmuLookup *> Lookups;

    // Generates the sequence of calls to child lookup objects in
    // the proper order, according to priority.
    void genChildTransHeaderCalls(const MmuLookupHash &children,const StrList &parents)
    {
      Lookups cl;
      ForEach(children,i) {
        cl.push_back(i->second);
      }
      sort(cl.begin(),cl.end(),SortPriority());
      ForEach(cl,i) {
        const MmuLookup &l = **i;
        writeChildTransHeaderCall(l,parents);
        _o << " {\n";
        if (_config.trace_mode()) {
          _o << parents.front() << "._last_hit = " << getLastHitName(l) << ";\n";
        }
        _o << "return true;\n"
           << "}\n";
      }
      // Didn't get a hit.
      _o << parents.front() << "._last_hit = 0;\n";
    }

    gc_string genLookupEntry(const MmuLookup &lookup,const gc_string &index,const Context *ctx)
    {
      bool fully_assoc = lookup.fully_assoc();
      gc_string farray = getFullArrayMember(lookup,ctx);
      bool oneDim = oneDimArray(lookup);

      gc_string entry = farray;
      if (!oneDim) {
        entry += "[set][" + index + "]";
      } else {
        if (lookup.ways() > 1 || lookup.sets() > 1) {
          if (fully_assoc) {
            entry += "[" + index + "]";
          } else {
            entry += "[set]";
          }
        }
      }
      return entry;
    }

    // This creates the guts of a translation or search operation.
    void genTransBody(gc_string &index,gc_string &entry,const MmuLookup &lookup,const Context *ctx)
    {
      bool fully_assoc = lookup.fully_assoc();
      gc_string num_ways = getLookupGetWays(lookup,ctx);
    
      if (lookup.ways() == 1 && !is_dynamic(lookup)) { 
        // If we have only one way, then check to see if we have an address-index test. If so,
        // place that select logic here.
        genAddrIndex(lookup);
      } else if (!fully_assoc) {
        // Not fully associative, so we need the lookup logic for the set.
        gc_string set_mask, set_shift;
        if (is_dynamic(lookup)) {
          set_mask = getLookupSetMask(lookup);
          set_shift = getLookupSetShift(lookup);
        } else {
          set_mask = uitogs(lookup.sets() - 1);
          set_shift = uitogs(trailing_zeros(lookup.pagesize()));
        }
        _o << "int set = ((ea >> " << set_shift << ") & " << set_mask << ");\n";
      }

      index = "i";

      entry = genLookupEntry(lookup,index,ctx);

      // Loop over all entries, performing the specified tests.
      if (lookup.ways() == 1 && !is_dynamic(lookup)) {
        _o << "do {\n";
      } else {
        _o << "for (unsigned " << index << " = 0; " << index << " != " << num_ways << "; ++" << index << ") {\n";
      }
      genAddrMask(lookup,entry);
      genTests(lookup,entry);
    }

    void genTransLoopEnd(const MmuLookup &lookup)
    {
      if (lookup.ways() == 1) {
        _o << "} while (0);\n";
      } else {
        _o << "}\n";
      }
    }

    // Setup translation-type predicates which can be used by the various tests.
    void createTransTypePreds(const MmuLookup &lookup)
    {
      // We then have to setup the predicates, if relevant.
      switch (lookup.type()) {
      case MmuLookup::Instr:
        _o << "const bool ATTRIBUTE_UNUSED Instr = true, Data = false, Load = false, Store = false;\n";
        break;
      case MmuLookup::Both:
        _o << "const bool ATTRIBUTE_UNUSED Instr = (tt == InstrTrans), Load = (tt == LoadTrans), Store = (tt == StoreTrans), Data = (Load || Store);\n";
        break;
      case MmuLookup::Data:
        _o << "const bool ATTRIBUTE_UNUSED Instr = false, Load = (tt == LoadTrans), Store = (tt == StoreTrans), Data = true;\n";
        break;
      }
    }

    // Create a lookup class.
    void genLookupFunc(const MmuLookup &lookup,const Context *ctx,const StrList &parents)
    {
      gc_string lname = lookup.name();

      gc_string transname  = lname + "_translate";
      gc_string searchname = lname + "_search";

      gc_string ftype  = getWayUserTypeName(lookup);
      gc_string farray = getFullArrayMember(lookup,ctx);

      gc_string lasthit = getLastHitName(lookup);

      bool is_dyn = is_dynamic(lookup);

      // If in trace-mode, add last-hit pointer.
      if (_config.trace_mode()) {
        _cd._member_vars.push_back("mutable const " + ftype + "* " + lasthit);
        _cd._constr_inits.push_back(lasthit + "(0)");
        _cd._reset_stmts.push_back(lasthit + " = 0;");
      }

      if (lookup.has_array()) {
        bool fully_assoc = lookup.fully_assoc();

        // We have an array, so generate lookup logic.

        // Generate all of the enable functions for the test-sets, if any exist.
        ForEach(lookup.test_order(),iter) {
          const MmuTestSet &testset = **iter;
          if (testset.enable()) {
            genFunc("bool " + getTestSetEnableName(lookup,testset),testset.env(),testset.enable()._func);
          }
        }

        // Generate the multi-hit and final-hit handlers, if applicable.
        if (lookup.multi_hit()) {
          genFunc("void " + getMultihitName(lookup),lookup.env(),lookup.multi_hit()._func);
        }
        if (lookup.final_hit()) {
          genFunc("void " + getFinalhitName(lookup),lookup.env(),lookup.final_hit()._func);
        }

        // The search function does the lookup and sets the set and way indices if found.
        _o << "bool " << searchname << "(unsigned &set_result,unsigned &way_result,addr_t ea,TransType tt) {\n";

        createTransTypePreds(lookup);

        gc_string index,entry;
        genTransBody(index,entry,lookup,ctx);
        if (fully_assoc) {
          _o << "set_result = 0;\n";
        } else {
          _o << "set_result = set;\n";
        }
        _o << "way_result = " << ((lookup.ways() == 1 && !is_dyn) ? "0" : index) << ";\n"
           << "return true;\n";
        genTransLoopEnd(lookup);
        _o << "return false;\n"
           << "}\n\n";

        // Only generate the translation functions if the lookup has a valid
        // priority.
        if (lookup.valid_priority()) {
          // The no-side-effects translate function does a search, then calculates
          // a real-address, but doesn't do any checking of the address.  This is
          // a bit of a hack: We just do a cast to call the search function.
          // Theoretically, there could be side effects there, because of register
          // accesses.  If there are, we'll blame it on the modelers. :)
          _o << "// No-side-effects version of the translate routine.\n"
             << "bool " << transname << "_ns (addr_t ea," << transParms(lookup,true) << "TransType tt,bool log) const {\n"
             << "unsigned set,i;\n"
             << "if (const_cast<" << _core.name() << "&>(*this)." << searchname << "(set,i,ea,tt)) {\n";
          genAddrMask(lookup,entry);
          genAddrConversion(lookup,entry,true,false);
          if (_config.trace_mode()) {
            _o << lasthit << " = &" << entry << ";\n";
          }
          _o << "return true;\n"
             << "} else {\n";
          if (_config.trace_mode()) {
            _o << lasthit << " = 0;\n";
          }
          _o << "return false;\n"
             << "}\n"
             << "}\n\n"

            // The translate function does the lookup and returns the modified address
            // if there is a hit.
             << "// Standard translation version:  Do a search and check to make sure it's valid.\n"
             << "bool " << transname << "(addr_t ea," << transParms(lookup,false) << "TransType tt,int seq";
          ForEach(parents,i) {
            _o << ",const " << *i << " &" << *i;
          }
          _o << ")\n"
             << "{\n";
          if (lookup.multi_hit_search()) {
            _o << "unsigned found = 0;\n";
          }

          createTransTypePreds(lookup);

          StrList newparents = parents;
          newparents.push_front(entry);

          genTransBody(index,entry,lookup,ctx);
        
          // In multi-hit mode, see if we've already got a hit.  If so, then
          // execute the relevant code and then return, since we've already got a
          // match.  We also log the hit for debug purposes.
          if (lookup.multi_hit()) {
            _o << "if (found) {\n";
            if (_config.trace_mode()) {
              // If we don't have children, then we can log here.  Otherwise, we
              // must wait until the children are called, in order to make sure
              // that an address exists.
              genLogMmuCall(lookup,parents,entry);
            }
            // If we have a final hit handler, then we want to continue the
            // search.
            _o << getMultihitName(lookup) << "(tt,ea,seq);\n"
               << "return true;\n"
               << "}\n";
          }

          genAddrConversion(lookup,entry,false);

          const MmuLookupHash &children = lookup.lookups();

          if (_config.trace_mode()) {
            _o << lasthit << " = &" << entry << ";\n";
            // If we don't have children, then we can log here.  Otherwise, we
            // must wait until the children are called, in order to make sure
            // that an address exists.
            genLogMmuCall(lookup,parents,entry);
          }

          // If we find a match, then execute permission checks.
          genPermChecks(lookup,newparents);

          // Then call the hit function, if appropriate.
          if (lookup.hit_func()) {
            _o << hitFuncName(lookup) << "(tt," << entry << ",ea,seq);\n";
          }

          // If we have children, place calls here.  Otherwise we're done.
          if (children.empty()) {
            // In a multi-hit situation, record that we've got a hit and then
            // proceed with searching in order to see if anything else hits.
            // Otherwise, we're done.
            if (lookup.multi_hit_search()) {
              _o << "++found;\n";
            } else {
              _o << "return true;\n";
            }
          } else {
            genChildTransHeaderCalls(children,newparents);
          }

          genTransLoopEnd(lookup);

          if (lookup.multi_hit_search()) {
            _o << "if (found) {\n";
            if (lookup.final_hit()) {
              _o << getFinalhitName(lookup) << "(tt,ea,seq,found);\n";
            }    
            _o << "return true;\n";
            _o << "}\n";
          }

          // Miss code goes here.
          if (lookup.miss_func()) {
            // We missed, so execute a miss function.
            gc_string missobj = missObjName(lookup);
            _o << missobj << " = " << missFuncName(lookup) << "(tt,ea,seq);\n";
            genAddrMask(lookup,missobj);
            genAddrConversion(lookup,missobj,false);
            if (_config.trace_mode()) {
              _o << lasthit << " = &" << missobj << ";\n";
              genLogMmuCall(lookup,parents,missobj);
            }
            _o << "return true;\n";
          } else {
            // No miss function, so we return false.
            if (_config.trace_mode()) {
              _o << lasthit << " = 0;\n";
            }
            _o << "return false;\n";
          }

          _o << "}\n\n";

        }
         
      } else if (lookup.valid_priority() && lookup.valid_translating_lookup()) {
        // No array exists, so we just execute the miss function.
        // Only generate the translation functions if the lookup has a valid
        // priority.

        _o << "// Standard translation version:  Do a search and check to make sure it's valid.\n"
           << "bool " << transname << "(addr_t ea," << transParms(lookup,false) << "TransType tt,int seq";
        ForEach(parents,i) {
          _o << ",const " << *i << " &" << *i;
        }
        _o << ")\n"
           << "{\n";
        createTransTypePreds(lookup);

        genTests(lookup,gc_string());
        gc_string missobj = missObjName(lookup);
        _o << missobj << " = " << missFuncName(lookup) << "(tt,ea,seq);\n";
        genAddrMask(lookup,missobj);
        genAddrConversion(lookup,missobj,false);

        // Since we don't really have an array, we just use an empty object so
        // that we have something for the functions.
        StrList newparents = parents;
        newparents.push_front(missobj);

        if (_config.trace_mode()) {
          // If we don't have children, then we can log here.  Otherwise, we
          // must wait until the children are called, in order to make sure
          // that an address exists.
          _o << lasthit << " = &" << missobj << ";\n";
          genLogMmuCall(lookup,parents,missobj);
        }
        
        // If we find a match, then execute permission checks.
        genPermChecks(lookup,newparents);

        _o << "return true;\n"
           << "}\n\n"
           << "// No-side-effects version of the translate routine.\n"
           << "bool " << transname << "_ns (addr_t ea," << transParms(lookup,true) << "TransType tt,bool log) const {\n"
           << transTemps(lookup) << ";\n"
           << "return const_cast<" << _core.name() << "&>(*this)." << transname << "(ea," << transArgs(lookup,false) << "tt,0);\n"
           << "}\n\n";
      }

    }

    // If we're supporting dynamic lookups, then add in the support functions for
    // setting various parameters.
    void genLookupDynSupport(const MmuLookup &lookup)
    {
      gc_string pfx = lookup.name() + "-";
      gc_string lname = getArrayName(lookup);
      
      if (is_dynamic(lookup)) {
        
        gc_string set = getSetParmName(lookup.name());
        gc_string get = getGetParmName(lookup.name());
        gc_string lst = getListParmName(lookup.name());

        bool fully_assoc = lookup.fully_assoc();

        _o << "bool " << set << "(const std::string &parm,unsigned value)\n"
           << "{\n"
           << "if (set_model_lookup_parm(";
        if (fully_assoc) {
          _o << "0,0,0,";
        } else {
          _o << "&" << getLookupNumSets(lookup) << ","
             << "&" << getLookupSetMask(lookup) << ","
             << "&" << getLookupSetShift(lookup) << ",";
        }
        _o << getLookupNumWays(lookup) << ","
           << "\"" << pfx << "\",parm,value)) {\n"
           << lname << ".reset(true," << ((fully_assoc) ? "1" : getLookupNumSets(lookup)) 
           << "," << getLookupNumWays(lookup) << ");\n";
        if (lookup.reset()._func) {
          _o << lookup.name() + "_init_reset();\n";
        }
        _o << "return true;\n"
           << "} else {\n"
           << "return false;\n"
           << "}\n"
           << "}\n\n"

           << "bool " << get << "(unsigned &value,const std::string &parm) const\n"
           << "{\n"
           << "return get_model_lookup_parm(value,\"" << pfx << "\",parm,";
        if (fully_assoc) {
          _o << "0,";
        } else {
          _o << "&" << getLookupNumSets(lookup) << ",";
        }
        _o << getLookupNumWays(lookup) 
           << ");\n"
           << "}\n\n"

           << "void " << lst << "(StrPairs &parms) const\n"
           << "{\n"
           << "return list_model_lookup_parm(\"" << pfx << "\",\"" << lookup.name() << "\",parms," << fully_assoc << ");\n"
           << "}\n\n";

        _cd._dyn_parms.push_back(lookup.name());
      }
    }


    // Create the attrs which implement the lookup objects.
    void genLookupFuncs(const MmuLookupHash &lookups,const StrList &parents)
    {
      ForEach(lookups,i) {
        const MmuLookup &lookup = *(i->second);
        const Context *ctx = _core.is_ctx_resource(lookup);

        if (!lookup.is_interface()) {
          genTransFuncs(lookup,parents);
          genLookupFunc(lookup,ctx,parents);
          StrList newparents = parents;
          newparents.push_front(lookup.type_name());
          genLookupFuncs(lookup.lookups(),newparents);
          genLookupDynSupport(lookup);
        }
      }
    }
  
    Lookups getLookups(AccessType type)
    {
      Lookups lookups;
      ForEach(_mmu.top_lookups(),i) {
        if (!i->second->is_interface() && i->second->valid_priority()) {
          if (type == aInstr) {
            if (i->second->type() == MmuLookup::Instr || i->second->type() == MmuLookup::Both) {
              lookups.push_back(i->second);
            }
          } else {
            if (i->second->type() == MmuLookup::Data || i->second->type() == MmuLookup::Both) {
              lookups.push_back(i->second);
            }
          }
        } 
      }
      sort(lookups.begin(),lookups.end(),SortPriority());
      return lookups;
    }

    // This fetches from the all-lookups list and does not sort by priority.
    Lookups getAllLookups(AccessType type)
    {
      Lookups lookups;
      ForEach(_mmu.all_lookups(),i) {
        if (!i->second->is_interface() && i->second->valid_priority()) {
          if (type == aInstr) {
            if (i->second->type() == MmuLookup::Instr || i->second->type() == MmuLookup::Both) {
              lookups.push_back(i->second);
            }
          } else {
            if (i->second->type() == MmuLookup::Data || i->second->type() == MmuLookup::Both) {
              lookups.push_back(i->second);
            }
          }
        } 
      }
      return lookups;
    }

    // Return true if we have multiple test sets within a set of lookups.
    bool check_has_multi_sets(const Lookups &lookups)
    {
      bool has_multi_sets = false;
      ForEach(lookups,i) {
        if ( (*i)->tests().size() > 1) {
          has_multi_sets = true;
          break;
        }
      }
      return has_multi_sets;
    }

    gc_string transArgs(const MmuLookup &lookup,bool ns,const char *pfx = "")
    {
      if (ns) {
        if (lookup.bounded()) {
          return gc_string();
        } else {
          return "rpn,mask,";
        }
      } else {        
        if (lookup.bounded()) {
          gc_string args = gc_string(pfx)+"lb,"+pfx+"ub,"+pfx+"bm,";
          return args;
        } else {
          return gc_string(pfx)+"rpn,"+pfx+"epn,"+pfx+"mask,";
        }
      }
    }

    gc_string transParms(const MmuLookup &lookup,bool ns)
    {
      if (ns) {
        if (lookup.bounded()) {
          return gc_string();
        } else {
          return "addr_t &rpn,addr_t &am,";
        }
      } else {        
        if (lookup.bounded()) {
          gc_string parms = "addr_t &lb,addr_t &ub,addr_t &bm,";
          return parms;
        } else {
          return "addr_t &rpn,addr_t &epn,addr_t &am,";
        }
      }
    }

    gc_string transTemps(const MmuLookup &lookup)
    {
      if (lookup.bounded()) {
        return "addr_t lb, ub, bm";
      } else {
        return "addr_t epn, mask";
      }
    }

    void writeTransHeaderCall(const MmuLookup &lookup,AccessType type)
    {
      _o << "if (" << lookup.name() << "_translate(ea,";
      switch (lookup.type()) {
      case MmuLookup::Both:
        switch (type) {
        case aInstr:
          _o << transArgs(lookup,false,"_instr_") << "InstrTrans";
          break;
        case aLoad:
          _o << transArgs(lookup,false,"_load_") << "LoadTrans";
          break;
        case aStore:
          _o << transArgs(lookup,false,"_store_") << "StoreTrans";
        }
        break;
      case MmuLookup::Instr:
        _o << transArgs(lookup,false,"_instr_") << "InstrTrans";
        break;
      case MmuLookup::Data:
        switch (type) {
        case aInstr:
          // Should never get here.
          assert(0);
          break;
        case aLoad:
          _o << transArgs(lookup,false,"_load_") << "LoadTrans";
          break;
        case aStore:
          _o << transArgs(lookup,false,"_store_") << "StoreTrans";
        }
        break;      
        break;
      }
      _o << ",seq))";
    }

    void writeTransHeaderCallNs(const MmuLookup &lookup,AccessType type)
    {
      _o << "if (" << lookup.name() << "_translate_ns(ea,";
      switch (lookup.type()) {
      case MmuLookup::Both:
        switch (type) {
        case aInstr:
          _o << transArgs(lookup,true) << "InstrTrans";
          break;
        case aLoad:
          _o << transArgs(lookup,true) << "LoadTrans";
          break;
        case aStore:
          _o << transArgs(lookup,true) << "StoreTrans";
        }
        break;
      case MmuLookup::Instr:
        _o << transArgs(lookup,true) << "InstrTrans";
        break;
      case MmuLookup::Data:
        switch (type) {
        case aInstr:
          // Should never get here.
          assert(0);
          break;
        case aLoad:
          _o << transArgs(lookup,true) << "LoadTrans";
          break;
        case aStore:
          _o << transArgs(lookup,true) << "StoreTrans";
        }
        break;      
        break;
      }
      _o << ",log))";
    }

    bool hasMissHandler(AccessType type)
    {
      switch (type) {
      case aInstr:
        if (_mmu.instr_miss()) {
          return true;
        }
        break;
      case aLoad:
        if (_mmu.load_miss() || _mmu.data_miss()) {
          return true;
        }
        break;
      case aStore:
        if (_mmu.store_miss() || _mmu.data_miss()) {
          return true;
        }
        break;
      }
      return false;
    }

    void writeMissHandlerCall(AccessType type)
    {
      switch (type) {
      case aInstr:
        if (_mmu.instr_miss()) {
          _o << "if (instruction_miss_handler(ea,seq)) goto " << retryLabel() << ";\n";
        }
        break;
      case aLoad:
        if (_mmu.load_miss()) {
          _o << "if (load_miss_handler(ea,seq)) goto " << retryLabel() << ";\n";
        }
        if (_mmu.data_miss()) {
          _o << "if (data_miss_handler(ea,seq)) goto " << retryLabel() << ";\n";
        }
        break;
      case aStore:
        if (_mmu.store_miss()) {
          _o << "if (store_miss_handler(ea,seq)) goto " << retryLabel() << ";\n";
        }
        if (_mmu.data_miss()) {
          _o << "if (data_miss_handler(ea,seq)) goto " << retryLabel() << ";\n";
        }
        break;
      }
    }
                        
    gc_string getCacheValidVar(AccessType type,TransCacheType cttype)
    {
      gc_string pfx = accessPfx(type);

      assert(cttype != None);
      if (cttype == Bounded) {
        return pfx + "_mmu_bounds_cache_valid";
      } else {
        return pfx + "_mmu_page_cache_valid";
      }

    }

    void writeCacheValidSet(AccessType type,TransCacheType cttype,bool value)
    {
      if (cttype == None) return;

      _o << getCacheValidVar(type,cttype) << " = " << value << ";\n";
    }

    void writeCacheClear(AccessType type,TransCacheType cttype)
    {
      if (cttype == None) return;

      gc_string pfx = accessPfx(type);

      if (cttype == Bounded) {
        gc_string lb    = pfx + "_lb";
        gc_string ub    = pfx + "_ub";
        gc_string bm    = pfx + "_bm";

        _o << lb << " = 0;\n"
           << ub << " = (addr_t)-1;\n"
           << bm << " = (addr_t)-1;\n";
      } else {
        gc_string mask  = pfx + "_mask";
        gc_string epn   = pfx + "_epn";

        _o << epn << " = 0;\n"
           << mask << " = (addr_t)-1;\n";
      }
    }

    void writeAddrCreate(TransCacheType cttype,const MmuLookup &lookup,AccessType type,
                         bool has_multi_sets,bool ns,bool rc)
    {
      if (ns) {
        if (has_realpage(lookup)) {
          _o << "ra = make_addr(rpn,mask,ea);\n";
        } else {
          _o << "ra = ea;\n";
        }
        if (_config.trace_mode()) {
          _o << "if (log) {\n"
             << "logMmuTranslation(" << getLastHitName(lookup) << "," << getTransType(type) << ",0,log,ea,ra);\n"
             << "}\n";
        }
        _o << "return true;\n";
      } else {
        gc_string pfx = accessPfx(type);
        if (type != aInstr) {
          _o << pfx << "_last_translation_type = " << lookup.id() << ";\n";
        }
        if (_config.trace_mode()) {
          _o << pfx << "_last_hit = " << getLastHitName(lookup) << ";\n";
        }
        if (has_multi_sets) {
          // If we have multiple test sets, then we OR in the current
          // instruction attributes, so that it becomes part of the cache-hit
          // criteria.
          gc_string var = accessPfx(type) + "_epn";
          _o << var << " |= " << getMmuInstrAttrGetter() << "();\n";
        }
        writeCacheValidSet(type,cttype,true);
        if (has_realpage(lookup)) {
          _o << "ra = make_addr(" << pfx << "_rpn," << pfx << "_mask,ea);\n";
        } else {
          _o << "ra = ea;\n";
        }
        // Only supply the TLB name to the address check function if it actually
        // has an array.
        _o << getAddrCheckCall("ea","ra",((lookup.has_array()) ?  "(*"+pfx+"_last_hit)" : ""),"",type);
        if (!lookup.lookups().empty() && _config.trace_mode()) {
          // If we have nested lookups, then we just log at the top-level.
          // Otherwise, we log within the translate function so that we can log
          // on translation hits which aren't valid due to, e.g. permissions,
          // etc.
          _o << "logMmuTranslation(" << pfx << "_last_hit," << getTransType(type) << ",seq,false,ea,ra);\n";
        }        
        if (rc) {
          _o << "return ra;\n";
        }
      }
    }

    // This writes the code that checks whether we can use a cached translation,
    // or whether we must do another translation.
    // ns:  No side-effects should be generated.
    void writeCacheCheck(TransCacheType cttype, AccessType type,bool has_multi_sets,
                         bool has_bound_mask,bool ns)
    {
      if (cttype == None) return;

      gc_string pfx = accessPfx(type);

      gc_string valid = getCacheValidVar(type,cttype);
      gc_string mask  = pfx + "_mask";
      gc_string rpn   = pfx + "_rpn";
      gc_string epn   = pfx + "_epn";
      gc_string lb    = pfx + "_lb";
      gc_string ub    = pfx + "_ub";
      gc_string bm    = pfx + "_bm";

      auto lookups = getLookups(type);

      gc_string trans_type = getTransType(type);

      // Do we have multiple test sets?  If so, we OR in the instruction
      // attributes to try and catch situations where those differentiate
      // between sets.  If only registers select different sets, then they'll be
      // regarded as MMU config registes and they'll end up clearing the cache,
      // so we won't have to worry about them.
      
      if (cttype == Page) {
        // Page-based check w/translation.
        gc_string opt_attr;
        if (has_multi_sets) {
          opt_attr = " | " + getMmuInstrAttrGetter() + "()";
        }
        
        _o << "if (" << valid << " && (((ea & " << mask << ")" << opt_attr << ") == " << epn << ")) {\n";
      } else {
        // Bounded check w/no translation.
        gc_string ea = (has_bound_mask) ? ("(ea & " + bm + ")") : "ea";
        _o << "if (" << valid << " && ((" << ea << " >= " << lb << ") && (" << ea << " <= " << ub << "))) {\n";
      }

      // If we hit in the cache, then if we're a data translation, we must check to see if
      // the last translation was of the same type (load or store).  If not, then we execute
      // the hit handler so that the model can update any necessary state.
      //
      // First- do we have any hit handlers?
      if ((type != aInstr) && !ns) {
        bool have_hit_handlers = false;
        ForEach(lookups,i) {
          if ( (*i)->hit_func() ) {
            have_hit_handlers = true;
            break;
          }
        }
        if (have_hit_handlers) {
          // We do have hit handlers, so we must write the appropriate checking logic.
          _o << "if (_last_data_access_type != " << trans_type << ") {\n";
          if (lookups.size() > 1) {
            _o << "switch (" << pfx << "_last_translation_type) {\n";
            ForEach(lookups,i) {
              const MmuLookup &lookup = **i;
              if (lookup.hit_func()) {
                gc_string hf = hitFuncName(lookup);
                _o << "case " << dec << lookup.id() << ":\n"
                   << hf << "(" << trans_type << ",*((" << getWayTypeName(lookup) << "*)" << pfx << "_last_hit),ea,seq);\n"
                   << "break;\n";
              }
            }
            _o << "}\n";
          } else {
            // If we only have a single lookup, then we dispense with the switch
            // statement.
            const MmuLookup &lookup = *(lookups.front());
            _o << hitFuncName(lookup) << "(" << trans_type << ",*((" << getWayTypeName(lookup) << "*)" << pfx << "_last_hit),ea,seq);\n";
          }
          _o << "}\n";
        }
      }
      if (ns) {
        if (cttype == Page) {
          _o << "ra = make_addr(" << rpn << "," << mask << ",ea);\n";
        } else {
          _o << "ra = ea;\n";
        }
      } else {
        if (type != aInstr) {
          _o << "_last_data_access_type = " << trans_type << ";\n";
        }
        if (cttype == Page) {
          _o << "addr_t ra = make_addr(" << rpn << "," << mask << ",ea);\n";
        } else {
          _o << "addr_t ra = ea;\n";
        }
        // Do any of the lookups have empty arrays?  Then we have to make the
        // query conditional.
        gc_string cond_tlb;
        ForEach(lookups,i) {
          const MmuLookup &lookup = **i;
          if (!lookup.has_array()) {
            cond_tlb = pfx + "_last_hit";
            break;
          }
        }
        _o << getAddrCheckCall("ea","ra","(*"+pfx+"_last_hit)",cond_tlb,type);
      }
      if (_config.trace_mode()) {
        if (ns) {
          _o << "if (log) {\n"
             << "logMmuTranslation(" << pfx << "_last_hit," << getTransType(type) << ",0,true,ea,ra);\n"
             << "}\n";
        } else {
          _o << "logMmuTranslation(" << pfx << "_last_hit," << getTransType(type) << ",seq,false,ea,ra);\n";
        }
      }
      _o << "return ra;\n"
         << "}\n";
    }

    gc_string enableName(AccessType type)
    {
      if (type == aInstr) {
        return "instr_translation_enable";
      } else {
        return "data_translation_enable";
      }
    }

    // Checks to see if any lookups can be cached using the page-based technique.
    bool check_has_page_cache(AccessType type)
    {
      auto lookups = getAllLookups(type);
      ForEach(lookups,i) {
        auto x = get_cache_type(**i);
        if (x.first == Page) return true;
      }
      return false;
    }

    // Checks to see if any lookups can be cached using the bounded-range technique.
    // The second element of the pair is true if a bounds mask exists.
    pair<bool,bool> check_has_bounds_cache(AccessType type)
    {
      auto lookups = getAllLookups(type);
      ForEach(lookups,i) {
        if ( (*i)->bounded() ) {
          auto x = get_cache_type(**i);
          if (x.first == Bounded) return make_pair(true,x.second);
        }
      }
      return make_pair(false,false);
    }
    
    pair<TransCacheType,bool> get_cache_type(const MmuLookup &l,bool examine_children = false)
    {
      if ( l.sizetype() != MmuLookup::None ) {
        return make_pair(Page,false);
      } else if ( l.bounded() ) {
        return make_pair(Bounded,l.bound_mask());
      }
      if (examine_children) {
        ForEach(l.lookups(),i) {
          auto r = get_cache_type(*i->second,examine_children);
          if (r.first != None) return r;
        }
      }
      return make_pair(None,false);
    }

    // Generates calls to the various top-level lookups and to the translation
    // cache.
    void genLookupCalls(AccessType type,bool ns)
    {
      bool p;
      Ptree *enable;

      if (type == aInstr) {
        p = _mmu.instr_prioritized();
        enable = _mmu.instr_enable()._func;
      } else {
        p = _mmu.data_prioritized();
        enable = _mmu.data_enable()._func;
      }

      if (!ns && _mmu.final_hit()) {
        // If we have a final-hit handler, then we search across all lookups,
        // even if priotizied.  But we only do this for the no-side-effect case,
        // since this situation is generally only needed for permission
        // checking.
        p = false;
      }

      auto lookups = getLookups(type);
      bool has_multi_sets   = check_has_multi_sets(lookups);
      bool has_page_cache   = check_has_page_cache(type);
      auto bounds_cache = check_has_bounds_cache(type);
      bool has_bmask = bounds_cache.second;
      const IntSet &instr_attrs = _core.instr_attrs();
      if (instr_attrs.empty() || type == aInstr) {
        // If we don't have any instruction attributes, then we don't care about
        // this extra checking, because we assume that the usual
        // register-modification cache invalidation process will take care of
        // things.
        //
        // We also ignore instruction accesses for now, since we assume that the
        // instruction fetching stream will not be discontiguous based upon
        // instruction attributes.
        has_multi_sets = false;
      }

      gc_string enable_pfx;
      if (ns) {
        _o << "addr_t ATTRIBUTE_UNUSED rpn,mask;\n";
        enable_pfx = "const_cast<" + _core.name() + "&>(*this).";
      }

      if (has_page_cache) {
        writeCacheCheck(Page,type,has_multi_sets,has_bmask,ns);
      }
      if (bounds_cache.first) {
        writeCacheCheck(Bounded,type,has_multi_sets,has_bmask,ns);
      }

      if ((type != aInstr) && !ns) {
        _o << "_last_data_access_type = " << getTransType(type) << ";\n";
      }

      // Write the enable logic, if present.
      if (enable) {
        _o << "if (!" << enable_pfx << enableName(type) << "()) {\n";
        if (ns) {
          _o << "ra = ea;\n"
             << "return true;\n";
        } else {
          _o << "return ea;\n";
        }
        _o << "}\n";
      }

      if (!ns) {
        // At this point, invalidate the cache to make sure that a translation
        // with an exception doesn't create an invalid state.
        if (has_page_cache) {
          writeCacheValidSet(type,Page,false);
        }
        if (bounds_cache.first) {
          // We invalidate the page cache and clear out the bounds, since we set
          // the page cache incrementally to be a subset of what matches.
          writeCacheValidSet(type,Bounded,false);
          writeCacheClear(type,Bounded);
        }
        _o << "addr_t ra;\n";

        // We'll catch any exceptions generated in this function in order to
        // invalidate the cache.
        if (haveExceptions()) {
          _o << "try {\n";
        }

        // This label is used if a restart is required for the translation.
        if (hasMissHandler(type)) {
          _o << retryLabel() << ":\n";
        }
      }

      if (!p) {
        // For tracking multiple hits.
        _o << "int count = 0;\n";
      }

      // Create prioritized lookup code or multi-hit lookup code.
      ForEach(lookups,i) {
        const MmuLookup &l = *(*i);
        auto ctpair = get_cache_type(l,true);
        if (ns) {
          writeTransHeaderCallNs(l,type);
        } else {
          writeTransHeaderCall(l,type);
        }
        _o << " {\n";
        // Return immediately if prioritized.
        if (p) {
          writeAddrCreate(ctpair.first,l,type,has_multi_sets,ns,true);
        } else {
          writeAddrCreate(ctpair.first,l,type,has_multi_sets,ns,false);
          _o << "++count;\n";
        }
        _o << "}\n";
      }
      if (!ns && p) {
        // Only needed here if prioritized; otherwise it's called in the 0 case
        // in the switch.
        writeMissHandlerCall(type);
      }
      if (!p) {
        if (ns) {
          _o << "if (count == 0) {\n"
             << "return false;\n"
             << "} else {\n"
             << "return ra;\n"
             << "}\n";
        } else {
          _o << "switch (count) {\n"
             << "case 1:\n";
          if (_mmu.final_hit()) {
            _o << "final_hit_handler(" << getTransType(type) << ",ea,seq,count);\n";
          }
          _o << "return ra;\n"
             << "case 0:\n";
          writeMissHandlerCall(type);
          _o << "break;\n"
             << "default:\n";
          if (_mmu.multi_hit()) {
            _o << "multi_hit_handler(" << getTransType(type) << ",ea,seq);\n";
          }
          if (_mmu.final_hit()) {
            _o << "final_hit_handler(" << getTransType(type) << ",ea,seq,count);\n";
          }
          _o << "break;\n"
             << "}\n";
        }
      }
      
      if (ns) {
        if (bounds_cache.first) {
          // Hmm... I'm not sure if this is general enough.  The idea is that,
          // for the no-side-effect case, if we have a bounds cache, then this
          // implies no translation, so even if we don't match, our real address
          // is the same as the effective address, so we should return true.  We
          // might need to generalize this by adding in a hook or something.
          _o << "ra = ea;\n"
             << "return true;\n";
        } else {
          _o << "return false;\n";
        }
      } else {
        if (_config.trace_mode()) {
          _o << accessPfx(type) << "_last_hit = 0;\n";
        }

        // If we have exceptions, then make sure that the cache is invalidated
        // if an exception occurs here.
        if (haveExceptions()) {
          _o << "}\n"
             << "catch (...) {\n";
          if (has_page_cache) {
            writeCacheValidSet(type,Page,false);
          }
          if (bounds_cache.first) {
            writeCacheValidSet(type,Bounded,false);
          }            
          _o << "throw;\n"
             << "}\n";
        }

        if (has_page_cache) {
          writeCacheValidSet(type,Page,false);
        }
        if (bounds_cache.first) {
          writeCacheValidSet(type,Bounded,false);
        }
        _o << "return ea;\n";
      }
    }

    void genHandlers()
    {
      // The handlers.
      if (Ptree *p = _mmu.instr_miss()._func) {
        _o << "\nbool instruction_miss_handler(" << get_func_args(p) << ")\n"
           << _walker.translateCode(_mmu.env(),get_func_body(p),false);
      }
      if (Ptree *p = _mmu.data_miss()._func) {
        _o << "\nbool data_miss_handler(" << get_func_args(p) << ")\n"
           << _walker.translateCode(_mmu.env(),get_func_body(p),false);
      }
      if (Ptree *p = _mmu.load_miss()._func) {
        _o << "\nbool load_miss_handler(" << get_func_args(p) << ")\n"
           << _walker.translateCode(_mmu.env(),get_func_body(p),false);
      }
      if (Ptree *p = _mmu.store_miss()._func) {
        _o << "\nbool store_miss_handler(" << get_func_args(p) << ")\n"
           << _walker.translateCode(_mmu.env(),get_func_body(p),false);
      }
      if (Ptree *p = _mmu.multi_hit()._func) {
        _o << "\nvoid multi_hit_handler(" << get_func_args(p) << ")\n"
           << _walker.translateCode(_mmu.env(),get_func_body(p),false);
      }
      if (Ptree *p = _mmu.final_hit()._func) {
        _o << "\nvoid final_hit_handler(" << get_func_args(p) << ")\n"
           << _walker.translateCode(_mmu.env(),get_func_body(p),false);
      }
      if (Ptree *p = _mmu.aligned_write()._func) {
        _o << "\nvoid aligned_write_handler(" << get_func_args(p) << ")\n"
           << _walker.translateCode(_mmu.env(),get_func_body(p),false);
      } else {
        _o << "\nvoid aligned_write_handler(addr_t,addr_t) {}\n\n";
      }
      if (Ptree *p = _mmu.misaligned_write()._func) {
        _o << "\nvoid misaligned_write_handler(" << get_func_args(p) << ")\n"
           << _walker.translateCode(_mmu.env(),get_func_body(p),false);
      } else {
        _o << "\nvoid misaligned_write_handler(addr_t,addr_t) {}\n\n";
      }
      if (Ptree *p = _mmu.misaligned_read()._func) {
        _o << "\nvoid misaligned_read_handler(" << get_func_args(p) << ")\n"
           << _walker.translateCode(_mmu.env(),get_func_body(p),false);
      } else {
        _o << "\nvoid misaligned_read_handler(addr_t,addr_t) {}\n\n";
      }
      if (Ptree *p = _mmu.pre_read()._func) {
        _o << "\nvoid pre_read_handler(" << get_func_args(p) << ")\n"
           << _walker.translateCode(_mmu.env(),get_func_body(p),false);
      } else {
        _o << "\nvoid pre_read_handler(addr_t,addr_t) {}\n\n";
      }
      if (Ptree *p = _mmu.post_read()._func) {
        _o << "\nvoid post_read_handler(" << get_func_args(p) << ")\n"
           << _walker.translateCode(_mmu.env(),get_func_body(p),false);
      } else {
        _o << "\nvoid post_read_handler(addr_t,addr_t) {}\n\n";
      }
      if (Ptree *p = _mmu.pre_write()._func) {
        _o << "\nvoid pre_write_handler(" << get_func_args(p) << ")\n"
           << _walker.translateCode(_mmu.env(),get_func_body(p),false);
      } else {
        _o << "\nvoid pre_write_handler(addr_t,addr_t) {}\n\n";
      }
      if (Ptree *p = _mmu.post_write()._func) {
        _o << "\nvoid post_write_handler(" << get_func_args(p) << ")\n"
           << _walker.translateCode(_mmu.env(),get_func_body(p),false);
      } else {
        _o << "\nvoid post_write_handler(addr_t,addr_t) {}\n\n";
      }
    }

    void genEnableFuncs()
    {
      if (Ptree *ie = _mmu.instr_enable()._func) {
        _o << "bool " << enableName(aInstr) << "()\n"
           << _walker.translateCode(_mmu.env(),get_func_body(ie),false)
           << "\n\n";
      }
      if (Ptree *de = _mmu.data_enable()._func) {
        _o << "bool " << enableName(aLoad) << "()\n"
           << _walker.translateCode(_mmu.env(),get_func_body(de),false)
           << "\n\n";
      }
    }


    void genAddrCheck()
    {
      if (_core.addr_check_handler()) {
        genFunc("void " + getAddrCheckHandler(),_core.env(),_core.addr_check_handler()._func);
      }
    }

    gc_string getAddrCheckCall(const gc_string &ea,const gc_string &ra,const gc_string &tlb,
                               const gc_string &cond_tlb,AccessType type)
    {
      return adl::getAddrCheckCall(_core,ea,ra,"",tlb,cond_tlb,type);
    }

    // Create the translation function.
    void genTranslate()
    {
      genHandlers();

      bool has_instr_page_cache   = check_has_page_cache(aInstr);
      auto has_instr_bounds_cache = check_has_bounds_cache(aInstr);

      if (has_instr_page_cache) {
        _cd._member_vars.push_back("addr_t _instr_rpn, _instr_epn, _instr_mask");
      } 
      if (has_instr_bounds_cache.first == true) {
        _cd._member_vars.push_back("addr_t _instr_lb, _instr_ub");
        if (has_instr_bounds_cache.second) {
          _cd._member_vars.push_back("addr_t _instr_bm");
        }
      }

      bool has_data_page_cache   = check_has_page_cache(aLoad);
      auto has_data_bounds_cache = check_has_bounds_cache(aLoad);

      if (has_data_page_cache) {
        _cd._member_vars.push_back("addr_t _load_rpn,  _load_epn,  _load_mask");
        _cd._member_vars.push_back("addr_t _store_rpn, _store_epn, _store_mask");
      } 
      if (has_data_bounds_cache.first) {
        _cd._member_vars.push_back("addr_t _load_lb,  _load_ub");
        _cd._member_vars.push_back("addr_t _store_lb, _store_ub");
        if (has_data_bounds_cache.second) {
          _cd._member_vars.push_back("addr_t _load_bm, _store_bm");
        }
      }

      if (has_instr_page_cache || has_data_page_cache) {
        _cd._member_vars.push_back("bool   _instr_mmu_page_cache_valid, _load_mmu_page_cache_valid, _store_mmu_page_cache_valid");
        _cd._constr_inits.push_back("_instr_mmu_page_cache_valid(false)");
        _cd._constr_inits.push_back("_load_mmu_page_cache_valid(false)");
        _cd._constr_inits.push_back("_store_mmu_page_cache_valid(false)");
        _cd._reset_stmts.push_back("_instr_mmu_page_cache_valid = false;");
        _cd._reset_stmts.push_back("_load_mmu_page_cache_valid = false;");
        _cd._reset_stmts.push_back("_store_mmu_page_cache_valid = false;");
      }
      if (has_instr_bounds_cache.first || has_data_bounds_cache.first) {
        _cd._member_vars.push_back("bool   _instr_mmu_bounds_cache_valid, _load_mmu_bounds_cache_valid, _store_mmu_bounds_cache_valid");
        _cd._constr_inits.push_back("_instr_mmu_bounds_cache_valid(false)");
        _cd._constr_inits.push_back("_load_mmu_bounds_cache_valid(false)");
        _cd._constr_inits.push_back("_store_mmu_bounds_cache_valid(false)");
        _cd._reset_stmts.push_back("_instr_mmu_bounds_cache_valid = false;");
        _cd._reset_stmts.push_back("_load_mmu_bounds_cache_valid = false;");
        _cd._reset_stmts.push_back("_store_mmu_bounds_cache_valid = false;");
      }
      _cd._member_vars.push_back("TransType _last_data_access_type");
      _cd._member_vars.push_back("unsigned  _load_last_translation_type, _store_last_translation_type");
      _cd._member_vars.push_back("const MmuBase *_instr_last_hit, *_load_last_hit, *_store_last_hit");

      _cd._constr_inits.push_back("_instr_last_hit(0)");
      _cd._constr_inits.push_back("_load_last_hit(0)");
      _cd._constr_inits.push_back("_store_last_hit(0)");
      _cd._reset_stmts.push_back("_instr_last_hit = 0;");
      _cd._reset_stmts.push_back("_load_last_hit = 0;");
      _cd._reset_stmts.push_back("_store_last_hit = 0;");

      _o << "\nvoid clear_mmu_cache() {\n";
      if (has_instr_page_cache) {
        _o << getCacheValidVar(aInstr,Page) << " = false; ";
      }
      if (has_data_page_cache) {
        _o << getCacheValidVar(aLoad,Page) << " = false; " 
           << getCacheValidVar(aStore,Page) << " = false;\n";
      }
      if (has_instr_bounds_cache.first) {
        _o << getCacheValidVar(aInstr,Bounded) << " = false; ";
      }
      if (has_data_bounds_cache.first) {
        _o << getCacheValidVar(aLoad,Bounded) << " = false; " 
           << getCacheValidVar(aStore,Bounded) << " = false;\n";
      }
      if (jit_support()) {
        _o << "_jit_bb_hash.flush_ea_hash();\n";
      }
      if (dc_support()) {
        _o << "_dc_bb_hash.flush_ea_hash();\n";
      }
      if (dmi_cache_support()) {
        gc_string drn = getDmiCacheName(GlobalMemoryName,"read");
        gc_string dwn = getDmiCacheName(GlobalMemoryName,"write");
        _o << drn << ".reset();\n"
           << dwn << ".reset();\n";
      }
      _o << "};\n\n"
         << "void checkInstrPerms() {\n";
      if (has_instr_page_cache) {
        _o << getCacheValidVar(aInstr,Page) << " = false;\n";
      } 
      if (has_instr_bounds_cache.first) {
        _o << getCacheValidVar(aInstr,Bounded) << " = false;\n" ;
      }
      _o << "};\n\n"
         << "void checkDataPerms()  {\n";
      if (has_data_page_cache) {
        _o << getCacheValidVar(aLoad,Page) << " = false;\n" 
           << getCacheValidVar(aStore,Page) << " = false;\n";
      } 
      if (has_data_bounds_cache.first) {
        _o << getCacheValidVar(aLoad,Bounded) << " = false;\n" 
           << getCacheValidVar(aStore,Bounded) << " = false;\n";
      }
      _o << "};\n\n"
         << "void checkLoadPerms()  {\n";
      if (has_data_page_cache) {
        _o << getCacheValidVar(aLoad,Page) << " = false;\n";
      } 
      if (has_data_bounds_cache.first) {
        _o << getCacheValidVar(aLoad,Bounded) << " = false;\n";
      }
      _o << "}\n\n"
         << "void checkStorePerms() {\n";
      if (has_data_page_cache) {
        _o << getCacheValidVar(aStore,Page) << " = false;\n";
      } 
      if (has_data_bounds_cache.first) {
        _o << getCacheValidVar(aStore,Bounded) << " = false;\n";
      }

      _o << "}\n\n";

      if (_config.trace_mode()) {
        _o << "void logMmuTranslation(const MmuBase *t,TransType tt,int seq,bool always_log,addr_t ea,addr_t ra) const {\n"
           << "if (Tracing && (always_log || adl_unlikely(_trace_mode & ttMmuTranslation))) {\n"
           << "if (t) {\n"
           << "t->logMmuTranslation(*this,tt,seq,ea,ra);\n"
           << "}\n"
           << "}\n"
           << "}\n\n";
      }

      // This is the user-facing variant, allowing users to log their own
      // translations, if they're doing something unusual like hardware
      // tablewalk.
      _o << "void logMmuTranslation(const MmuBase &t,TransType tt,int seq,addr_t ea,addr_t ra) const {\n";
      if (_config.trace_mode()) {
        _o << "logMmuTranslation(&t,tt,seq,false,ea,ra);\n";
      }
      _o << "}\n\n";      

      genEnableFuncs();

      _o << "addr_t instrReadTranslateInternal(addr_t ea,int seq) " << attr_used << "{\n";
      genLookupCalls(aInstr,false);
      _o << "};\n\n";

      _o << "addr_t dataReadTranslateInternal(addr_t ea,int seq) " << attr_used << "{\n";
      genLookupCalls(aLoad,false);
      _o << "};\n\n";

      _o << "addr_t dataWriteTranslateInternal(addr_t ea,int seq) " << attr_used << "{\n";
      genLookupCalls(aStore,false);
      _o << "};\n\n";

      _o << "bool instrReadTranslateInternalNs(addr_t &ra,addr_t ea,bool log) const " << attr_used << "{\n";
      genLookupCalls(aInstr,true);
      _o << "};\n\n";

      _o << "bool dataReadTranslateInternalNs(addr_t &ra,addr_t ea,bool log) const " << attr_used << "{\n";
      genLookupCalls(aLoad,true);
      _o << "};\n\n";

      _o << "bool dataWriteTranslateInternalNs(addr_t &ra,addr_t ea,bool log) const " << attr_used << "{\n";
      genLookupCalls(aStore,true);
      _o << "};\n\n";

    }

    // The proxy object is the object that user interacts with when directly 
    // accessing a lookup.
    void genProxyObject(const MmuLookup &lookup)
    {
      if (lookup.has_array()) {

        gc_string way_type  = getWayUserTypeName(lookup);
        gc_string set_type  = getSetTypeName(lookup);

        gc_string pname  = getProxyName(lookup);
        gc_string ptype  = getProxyObject(lookup);
        gc_string farray = getArrayName(lookup);
        gc_string array_name = getArrayMember();

        bool fully_assoc = lookup.fully_assoc();

        gc_string cn;
        const Context *ctx = _core.is_ctx_resource(lookup);
        if (ctx) {
          cn = ctx->name() + "().";
        }

        gc_string num_sets, num_ways;
        if (is_dynamic(lookup)) {
          num_sets = "_core." + getLookupGetSets(lookup,ctx);
          num_ways = "_core." + getLookupGetWays(lookup,ctx);
        } else {
          num_sets = uitogs(lookup.sets());
          num_ways = uitogs(lookup.ways());
        }

        gc_string set_entry = "_core."+cn+farray + "." + array_name + ( (fully_assoc) ? "" : "[set]");
        gc_string way_entry = set_entry + ( (lookup.ways() == 1) ? "" : "[way]");

        _o << "struct " << ptype << " {\n"
           << _core.name() << " &_core;\n"
           << ptype << "(" << _core.name() << " &core) : _core(core) {};\n\n"

           << way_type << " &operator()(unsigned set,unsigned way) {\n"
           << "if (set >= " << num_sets << ") {\n"
           << "throw std::runtime_error(\"Bad set specified.\");\n"
           << "}\n"
           << "if (way >= " << num_ways << ") {\n"
           << "throw std::runtime_error(\"Bad way specified.\");\n"
           << "}\n"
           << "return " << way_entry << ";\n"
           << "};\n\n"
           << "template <size_t N1,size_t N2>\n"
           << way_type << " &operator()(const bits<N1> &set,const bits<N2> &way) {\n"
           << "return operator()(set.uint32(),way.uint32());\n"
           << "}\n\n"

          // Note that we have to clear the MMU cache on a write in case the
          // current translation is being written to.
           << "void write(unsigned set,unsigned way,const " << way_type << " &v) " << attr_used << "{\n"
           << way_type << " &x = (*this)(set,way);\n"
           << "x = v;\n"
           << "_core.clear_mmu_cache();\n"
           << "if (Tracing && (_core.trace_mode() & ttMmuTranslation)) {\n"
           << "  x.logMmuTranslation(_core,WriteTrans,0,0,0);\n"
           << "}\n"
           << "};\n\n"
           << "template <size_t N1,size_t N2>\n"
           << "void write(const bits<N1> &set,const bits<N2> &way,const " << way_type << " &v) {\n"
           << "write(set.uint32(),way.uint32(),v);\n"
           << "}\n\n"

           << set_type << " &operator()(unsigned set) " << attr_used << "{\n"
           << "if (set >= " << num_sets << ") {\n"
           << "throw std::runtime_error(\"Bad set specified.\");\n"
           << "}\n"
           << "return " << set_entry << ";\n"
           << "};\n\n"
           << "template <size_t N1>\n"
           << set_type << " &operator()(const bits<N1> &set) {\n"
           << "return operator()(set.uint32());\n"
           << "}\n\n"
      
           << "bool search(unsigned &set,unsigned &way,addr_t ea,TransType t) " << attr_used << "{\n"
           << "return _core." << lookup.name() << "_search(set,way,ea,t);\n"
           << "}\n\n"

           << "template <size_t N1>\n"
           << "bool search(unsigned &set,unsigned &way,const bits<N1> &ea,TransType tt) {\n"
           << "return search(set,way,ea.uint64(),tt);\n"
           << "}\n\n"

           << "unsigned num_sets() const { return " << num_sets << "; };\n\n"

           << "unsigned num_ways() const { return " << num_ways << "; };\n\n"
      
           << "};\n\n";

        _cd._member_vars.push_back(ptype + " " + pname);

        _cd._constr_inits.push_back(pname+"(*this)");
      }
    }

    void genProxyObjects(const LookupList &lookups)
    {
      ForEach(lookups,i) {
        const MmuLookup &lookup = *i->_lookup;
        if (!lookup.is_interface()) {
          genProxyObject(lookup);
        }
      }
    }

    // Write routines for reading/writing a lookup to memory.
    void genSerialization(const MmuLookup &lookup,const Context *ctx)
    {
      if (lookup.has_array()) {
        gc_string num_sets, num_ways, num_sets_inv, num_ways_inv;
        if (is_dynamic(lookup)) {
          num_sets = "_core." + getLookupGetSets(lookup,ctx);
          num_ways = "_core." + getLookupGetWays(lookup,ctx);
        } else {
          num_sets_inv = num_sets = uitogs(lookup.sets());
          num_ways_inv = num_ways = uitogs(lookup.ways());
        }

        // For each read and write, we put in an assertion which checks the
        // amount written (by checking the address) vs. the size specified by
        // the user.  This acts as a double-check, to avoid hard-to-debug memory
        // corruption caused by overwrites.

        if (lookup.mem_read()) {

          genFunc("addr_t " + lookup.name() + "_read_entry",_core.env(),lookup.mem_read()._func);

          _o << "addr_t " << lookup.name() << "_read(addr_t addr) {\n"
             << "addr_t oaddr = addr;\n"
             << "for (unsigned set = 0; set != " << num_sets << "; ++set) {\n"
             << "for (unsigned way = 0; way != " << num_ways << "; ++way) {\n"
             << "addr = " << lookup.name() << "_read_entry(set,way,addr);\n"
             << "}\n"
             << "}\n"
             << "assert(addr == (oaddr+" << lookup.mem_size() << "));\n"
             << "return addr;\n"
             << "}\n\n";
        }

        if (lookup.mem_write()) {

          genFunc("addr_t " + lookup.name() + "_write_entry",_core.env(),lookup.mem_write()._func);

          _o << "addr_t " << lookup.name() << "_write(addr_t addr) {\n"
             << "addr_t oaddr = addr;\n"
             << "for (unsigned set = 0; set != " << num_sets << "; ++set) {\n"
             << "for (unsigned way = 0; way != " << num_ways << "; ++way) {\n"
             << "addr = " << lookup.name() << "_write_entry(set,way,addr);\n"
             << "}\n"
             << "}\n"
             << "assert(addr == (oaddr+" << lookup.mem_size() << "));\n"
             << "return addr;\n"
             << "}\n\n";
        }
      }
    }

    void genSerialization(const LookupList &lookups)
    {
      ForEach(lookups,i) {
        const MmuLookup &lookup = *i->_lookup;
        const Context *ctx = i->_ctx;
        if (!lookup.is_interface()) {
          genSerialization(lookup,ctx);
        }
      }
    }

    // Generates the reset function for the MMU.  This is only
    // needed if a reset handler is present.  Otherwise, the basic resetMmu()
    // call will suffice, which sets all fields to their default POR values.
    void genMmuReset(const LookupList &lookups)
    {
      ForEach(lookups,i) {
        const MmuLookup &lookup = *i->_lookup;
        if (!lookup.is_interface()) {
          if (Ptree *reset = lookup.reset()._func) {
            gc_string name = lookup.name() + "_init_reset";
            genFunc("void " + name,lookup.env(),reset);
            _cd._reset_stmts.push_back(name+"()");
          }
        }
      }
    }

    // Main generation entry point.
    void generate(RegBaseSet &reg_refs)
    {
      _o << "\n"
         << "//\n"
         << "// MMU routines.\n"
         << "//\n\n";

      genAddrCheck();

      _walker.reset_regs();

      LookupList all_lookups;
      ForEach(_mmu.all_lookups(),i) {
        const MmuLookup *l = i->second;
        if (const Context *ctx = _core.is_ctx_resource(*l)) {
          all_lookups.push_back( LookupEl(l,ctx) );
        } else {
          all_lookups.push_back( l );
        }
      }

      // Data structures are generated separately.  We helpers on all lookups,
      // i.e. top-level and nested.
      genProxyObjects(all_lookups);
      genHelpers(all_lookups,_core.shared(),true);
      genSerialization(all_lookups);
      genMmuReset(all_lookups);

      // Set slots for the mmu communication registers.
      if (transactional_iss()) {
        _walker.set_slots(&_mem_comm_slots);
        _walker.set_conditional_slot_access(true);
      }

      // The lookup translation stuff only starts at the top.
      genLookupFuncs(_mmu.top_lookups(),StrList());

      genTranslate();

      _walker.set_slots(0);
      _walker.set_conditional_slot_access(false);

      reg_refs.clear();
      reg_refs.insert(_walker.regs_referenced().begin(),_walker.regs_referenced().end());

      // Finally, remove any registers referenced which have the no-mmu-control attribute.
      int nmc = Data::get_attr(NoMmuControlAttr);
      if (nmc >= 1) {
        for (auto iter = reg_refs.begin(); iter != reg_refs.end(); ) {
          if ((*iter)->has_attr(nmc)) {
            auto tmp = iter++;
            reg_refs.erase(tmp);
          } else {
            ++iter;
          }
        }
      }
    }

    // Entry point for generating stubs for designs that don't have MMUs.
    void generateMMUstubs(opfxstream &o)
    {
      genAddrCheck();

      o << "//\n"
        << "//Stubs for the core services.\n"
        << "//\n"
        << "void setMmuLookup(const std::string &lookup,unsigned set,unsigned way,const FieldData &ld) { throw std::runtime_error(\"No MMU exists for this design.\"); };\n"
        << "void getMmuLookup(const std::string &lookup,unsigned set,unsigned way,FieldData &ld) const { throw std::runtime_error(\"No MMU exists for this design.\"); };\n"
        << "void showMmu(ReportBase &) const {};\n"
        << "\n"
        << "void clear_mmu_cache() {};\n"
        << "void checkInstrPerms() {};\n"
        << "void checkDataPerms() {};\n"
        << "void checkLoadPerms() {};\n"
        << "void checkStorePerms() {};\n"
        << "\n"
        << "//\n"
        << "// Stubs for the translation functions.\n"
        << "//\n"
        << "addr_t instrReadTranslateInternal(addr_t a,int seq) { " << getAddrCheckCall("a","a","","",aInstr) << " return a; };\n"
        << "addr_t dataReadTranslateInternal (addr_t a,int seq) { " << getAddrCheckCall("a","a","","",aLoad) << " return a; };\n"
        << "addr_t dataWriteTranslateInternal(addr_t a,int seq) { " << getAddrCheckCall("a","a","","",aStore) << " return a; };\n"
        << "\n"
        << "bool instrReadTranslateInternalNs(addr_t &ra,addr_t a,bool log = false) const { ra = a; return true; };\n"
        << "bool dataReadTranslateInternalNs (addr_t &ra,addr_t a,bool log = false) const { ra = a; return true; };\n"
        << "bool dataWriteTranslateInternalNs(addr_t &ra,addr_t a,bool log = false) const { ra = a; return true; };\n"
        << "\n"
        << "void aligned_write_handler(addr_t,addr_t) { };\n"
        << "void misaligned_write_handler(addr_t,addr_t) { };\n"
        << "void misaligned_read_handler(addr_t,addr_t) { };\n"
        << "void pre_read_handler(addr_t,addr_t) { };\n"
        << "void post_read_handler(addr_t,addr_t) { };\n"
        << "void pre_write_handler(addr_t,addr_t) { };\n"
        << "void post_write_handler(addr_t,addr_t) { };\n"
        << "\n";
    }


  };

  // Generate helper functions only.
  void generateMmuHelpers(opfxstream &o,const LookupList &lookups,
                          ClassData &cd,const Resources &shared,
                          const ConfigDB &config,bool is_core)
  {
    GenMmuBase gm(o,cd,config,0);

    gm.genHelpers(lookups,shared,is_core);
  }

  // Just generates MMU lookup data structures.
  void generateLookupDS(opfxstream &o,const LookupList &lookups,
                        ClassData &cd,const Resources &shared,
                        const ConfigDB &config)
  {
    GenMmuBase gm(o,cd,config,0);
  
    gm.genLookupDataStructures(lookups,shared);
  }

  void generateLookupDS(opfxstream &o,const Core &core,ClassData &cd,
                        const ConfigDB &config,const CoreConfig &coreconfig)
  {
    if (const MMU *mmu = core.mmu()) {
      GenMmuBase gm(o,cd,config,&coreconfig);
      
      LookupList all_lookups;
      ForEach(mmu->all_lookups(),i) {
        const MmuLookup *l = i->second;
        if (const Context *ctx = core.is_ctx_resource(*l)) {
          all_lookups.push_back( LookupEl(l,ctx) );
        } else {
          all_lookups.push_back( l );
        }
      }
      
      gm.genLookupDataStructures(all_lookups,core.shared());
    }
  }

  // Entry point for generating the MMU.
  void generateMMU(opfxstream &o,const MMU *mmu,const Core &core,InstrWalker &walker,
                   const SlotAlloc &mem_comm_slots,const gc_string &filename,ClassData &cd,
                   RegBaseSet &reg_refs,const ConfigDB &config,const CoreConfig &coreconfig )
  {
    GenMmu gm(o,*mmu,core,walker,filename,mem_comm_slots,cd,config,coreconfig);
    if (mmu) {
      gm.generate(reg_refs);
    } else {
      gm.generateMMUstubs(o);
    }
  }

}

//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Main generation code for all register stuff.
//


#include <iostream>
#include <sstream>
#include <stdexcept>
#include <assert.h>

#include "opencxx/parser/Ptree.h"
#include "opencxx/parser/PtreeUtil.h"

#include "fe/Data.h"
#include "fe/Helpers.h"

#include "helpers/gc_list.h"
#include "helpers/pfxstream.h"
#include "helpers/Macros.h"
#include "helpers/BitTwiddles.h"
#include "helpers/CodeGen.h"

#include "asm/GenAsm.h"

#include "MiscTypes.h"
#include "AttrsHelper.h"

#include "InstrWalker.h"
#include "ConfigDB.h"
#include "GenRegs.h"

using namespace std;
using namespace Opencxx;
using namespace PtreeUtil;

static const char *LogAsRegAttr = "log_as_reg";

static const char *attr_used = " ATTRIBUTE_USED ";

namespace adl {

gc_string getRegDataType(const RegBase &r)
{
  gc_ostringstream ss;
  ss << "bits<" << r.width() << ">";
  return ss.str();
}

gc_string getRegTypeName(const RegBase &r)
{
  return r.name() + "_t";
}

gc_string getRegObjName(const RegBase &r)
{
  return "_" + r.name() + "_Reg";
}

gc_string getRegParmName(const RegBase &r)
{
  return "__" + r.name() + "_Reg";
}

gc_string getRegDbgAccess(const RegBase &r)
{
  return "Dbg_" + r.name();
}

gc_string getContextTypeName(const Context &ctx)
{
  return ctx.name() + "_context_t";
}

gc_string getContextObjName(const Context &ctx)
{
  return ctx.name();
}

gc_string getContextObjType(const Context &ctx)
{
  return ctx.name() + "_obj_t";
}

gc_string getContextControl(const Context &ctx,bool full_qual)
{
  gc_string r;
  if (full_qual) {
    r = getContextObjName(ctx) + ".";
  }
  return r + "_" + ctx.name() + "_active";
}

gc_string getContextDbgControl(const Context &ctx,bool full_qual)
{
  gc_string r;
  if (full_qual) {
    r = getContextObjName(ctx) + ".";
  }
  return r + "_" + ctx.name() + "_debug";
}

gc_string getContextAccess(const Context &ctx,const gc_string &var = "")
{
  if (var.empty()) {
    return getContextObjName(ctx) + "(" + getContextControl(ctx) + ").";
  } else {
    return getContextObjName(ctx) + "(" + var + ").";
  }
}

gc_string getCtxChangeFunc(const Context &ctx)
{
  return ctx.name() + "_check_change";
}

gc_string getCtxChangeBackFunc(const Context &ctx)
{
  return ctx.name() + "_change_back";
}

gc_string getRegFieldGetter(const RegBase &r,const RegField &rf)
{
  return "getfield_" +r.name() + "_" + rf.name();
}

gc_string getRegFieldSetter(const RegBase &r,const RegField &rf)
{
  return "setfield_" +r.name() + "_" + rf.name();
}

gc_string getCallbackName(bool read)
{
  return (read) ? "_reg_read_callbacks" : "_reg_callbacks";
}

gc_string getDefaultCallback()
{
  return "default_reg_callback_action";
}

gc_string getGlobalInvalidEntryReadName()
{
  return "global_invalid_read_hook";
}

gc_string getGlobalInvalidEntryWriteName()
{
  return "global_invalid_write_hook";
}

gc_string getInvalidEntryReadName(const RegBase &r)
{
  return r.name() + "_invalid_read_hook";
}

gc_string getInvalidEntryWriteName(const RegBase &r)
{
  return r.name() + "_invalid_write_hook";
}

gc_string getRegReadAliasResolverName()
{
  return "reg_read_alias_resolver";
}

gc_string getRegWriteAliasResolverName()
{
  return "reg_write_alias_resolver";
}

gc_string getExternalRegRead(const RegBase &r)
{
  return "IssBaseClass::" + r.name() + "_external_read";
}

gc_string getExternalRegWrite(const RegBase &r)
{
  return "IssBaseClass::" + r.name() + "_external_write";
}

gc_string getRegExtWriteFunc(const RegBase &r)
{
  return r.name() + "_external_write";
}


// Create a map of register and register file lists.  Each primary key in this map
// represents a context, or 'default' for any non-context resource.
void makeRegLists(RegCtxMap &regdata,const Core &core)
{
  ForEach(core.regs(),i) {
    const Reg *reg = i->second;
      
    if (const Context *ctx = core.is_ctx_resource(*reg)) {
      RegCtxData &rc = regdata[ctx->name()];
      rc._regs.push_back(RegEl<Reg>(reg,core.is_shared(*reg),ctx));
      rc._ctx = ctx;
    } else {
      regdata[DefaultCtxName]._regs.push_back(RegEl<Reg>(reg,core.is_shared(*reg)));
    }

  }

  ForEach(core.regfiles(),i) {
    const RegFile *rf = i->second;

    if (const Context *ctx = core.is_ctx_resource(*rf)) {
      RegCtxData &rc = regdata[ctx->name()];
      rc._regfiles.push_back(RegEl<RegFile>(rf,core.is_shared(*rf),ctx));
      rc._ctx = ctx;
    } else {
      regdata[DefaultCtxName]._regfiles.push_back(RegEl<RegFile>(rf,core.is_shared(*rf)));
    }

  }
}


// If the hook is an alias, then add it to the list.
void check_for_alias(InstrWalker::RegBaseList &regs,bool &is_alias,const RegHookBase *hook)
{
  if (hook && hook->aliased()) {
    const RegName &rn = hook->alias();
    is_alias = true;
    if (rn()) {
      regs.push_back(rn());
    }
  }
}

// Given a hook, look for registers it references and record those registers,
// along with the hook.  When those registers are modified, this watch will be
// called, in order to update appropriate state.
void genWatch(opfxstream &o,Watches &watches,InstrWalker &walker,
              Environment *env,Ptree *watch) 
{
  Ptree *newwatch = funcBody(walker,env,watch);
  Ptree *sym = AdlGenSym();
  o << "void " << sym << "() " << newwatch << "\n";

  // Get the registers referenced.
  InstrWalker::RegBaseList regs = walker.regs_referenced();

  // Then iterate over them, looking for aliases.  The targets of the alias will
  // be recorded in this watch list as well, since a modification of the
  // underlying item should trigger a state change as well or an aliased
  // register.  We also remove the alias itself, since it's redundant.
  InstrWalker::RegBaseList extras;
  for (auto i = regs.begin(); i != regs.end(); ) {
    const RegBase *rb = *i;
    bool is_alias = false;
    if (const RegFile *rf = dynamic_cast<const RegFile *>(rb)) {
      check_for_alias(extras,is_alias,rf->readhook());
      check_for_alias(extras,is_alias,rf->writehook());
    } else if (const Reg *reg = dynamic_cast<const Reg *>(rb)) {
      check_for_alias(extras,is_alias,reg->readhook());
      check_for_alias(extras,is_alias,reg->writehook());
    }
    if (is_alias) {
      auto tmp = i++;
      regs.erase(tmp);
    } else {
      ++i;
    }
  }

  // Then add everything in extras to the set of registers.
  regs.insert(regs.end(),extras.begin(),extras.end());

  watches.push_back(Watch(regs,sym));
}

void gen_regfield_get(opfxstream &o, const gc_string& get,const gc_string& pktoff, 
                      const RegField& rf, unsigned offset,const gc_string &dotTemplate) 
{
  auto bits = rf.bits();
  if (rf.is_split()) {
    unsigned shift = 0;   
    o << "bits<" << rf.width() << "> tmp;\n";
    ForEach(bits,i) {
      unsigned l = i->l() - offset; 
      unsigned r = i->r() - offset;       
        
      unsigned lx = shift;
      unsigned rx = shift + i->width() - 1;
        
      if (BitPair::is_little_endian()) {
        lx = bits.width() - lx - 1;
        rx = bits.width() - rx - 1;
      }	     
      gc_string func = "." + dotTemplate + " get<" + uitogs(i->width()) + ">";
      o << "tmp.set(" << lx << "," << rx << ",";       
      o << get << func << "(" << pktoff << l << "," << pktoff << r << "));\n";
      shift += i->width();
    }
    o << "return tmp;\n";
  } else {    
    unsigned l = bits.front().l() - offset; 
    unsigned r = bits.front().r() - offset;
 
    gc_string func = "." + dotTemplate + " get<" + uitogs(bits.front().width()) + ">";
    o << "return " << get << func << "(" << pktoff << l << "," << pktoff << r << ");\n";    
  }  
}

void gen_indexed_regfield_exprs(const RegBase &r,const RegField &rf,gc_string &left,gc_string &right)
{
  gc_string rf_width(uitogs(rf.width()));
  gc_string r_width(uitogs(r.width()));
  
  left  =  "((i%"+r_width+")*" + rf_width + ")";
  right = "(((i%"+r_width+")*" + rf_width + ")+" + rf_width + "-1)";
}

void gen_regfield_mask(opfxstream &o, const gc_string& get,const gc_string& pktoff, 
                       const RegBase &r,const RegField& rf, const gc_string &member) 
{
  auto bits = rf.bits();

  gc_string f = get;
  if (!member.empty()) {
    f += "." + member;
  }
  if (rf.indexed()) {
    gc_string left,right;
    gen_indexed_regfield_exprs(r,rf,left,right);
    o << f << ".mask(" << pktoff << left << "," << pktoff << right << ");\n";
  } else {
    ForEach(bits,i) {
      unsigned left = i->l() - r.offset(); 
      unsigned right = i->r() - r.offset();       
      
      o << f << ".mask(" << pktoff << left << "," << pktoff << right << ");\n";
    }
  }
}

void  gen_regfield_set(opfxstream& o, const gc_string& var, const gc_string& pktoff, 
                       const RegField& rf,unsigned offset, const gc_string& x)
{
  unsigned width = 0;
  auto bits = rf.bits();
  if (rf.is_split()) {
    ForEach(bits,i) {
      unsigned l = i->l() - offset;
      unsigned r = i->r() - offset;
        
      unsigned lx = width;
      unsigned rx = width + i->width() - 1;
        
      if (BitPair::is_little_endian()) {
        lx = bits.width() - lx - 1;
        rx = bits.width() - rx - 1;
      }	  
        
      o << var << ".set(" << pktoff << l << "," << pktoff << r << "," 
        << x << "(" << lx  << "," << rx << "));\n";
      width += i->width();    
    }
  } else {
    unsigned l = bits.back().l() - offset;
    unsigned r = bits.back().r() - offset;     
    o << var << ".set(" << pktoff << l << "," << pktoff << r << "," << x << ");\n";
  }     
}

class GenRegsBase {
public:
  GenRegsBase (opfxstream &o,ClassData &cd,const ConfigDB &config,RegBaseSet *mmu_regs = 0) :
    _o(o), 
    _cd(cd),
    _config(config),
    _mmu_regs(mmu_regs),
    _rf_log_as_reg_attr(Data::get_attr(gc_string(LogAsRegAttr)))
  {};

protected:
  opfxstream     &_o;

  ClassData      &_cd;
  const ConfigDB &_config;

  RegBaseSet     *_mmu_regs;
  int             _rf_log_as_reg_attr;

public:

  bool multi_compile() const
  {
    return _config.num_compile_jobs() > 0;
  }

  gc_string headerOnly() const
  {
    return adl::headerOnly(multi_compile());
  }

  bool transactional_iss() const
  {
    return _config.iss_type() == tTransactional;
  }

  bool hybrid_iss() const
  {
    return _config.iss_type() == tHybrid;
  }

  bool normal_iss() const
  {
    return _config.iss_type() == tNormal;
  }

  bool track_reads() const
  {
    return _config.track_reads();
  }

  bool log_reads() const
  {
    return _config.log_reg_reads();
  }

  bool log_unarch() const
  {
    return _config.log_unarch();
  }

  bool log_partial_masks() const
  {
    return _config.log_reg_masks();
  }

  bool time_tagged() const
  {
    return _config.time_tagged();
  }

  // Return a parameter for register/register-file accesses when we have a
  // time-tagged ISS.  Returns an empty string otherwise, so that it can always
  // be called.
  gc_string ttp() const
  {
    return (time_tagged()) ? "TimeTaggedBase &ttb," : "";
  }

  // Returns an argument for register/register-file accesses when we have a
  // time-tagged ISSs.  Returns an empty string otherwise, so that it can always
  // be called.
  gc_string tta(bool comma = false) const
  {
    if (time_tagged()) {
      return (comma) ? "(*this)," : "*this";
    } else {
      return gc_string();
    }
  }

  // Similar to above, but passes on the parameter used by ttp(), rather than
  // the this pointer of the ISS.
  gc_string tta2(bool comma = false) const
  {  
    if (time_tagged()) {
      return (comma) ? "ttb," : "ttb";
    } else {
      return gc_string();
    }
  }

  bool jit_support() const
  {
    return _config.jit_support();
  }

  bool extern_regs() const
  {
    return _config.library_mode();
  }

  bool regGenOverride(const RegBase &reg)
  {
    if (_config.gen_all_reg_code()) return true;

    if (_config.gen_full_regs().count(reg.name())) return true;

    return false;
  }

  bool isRegWritten(const RegBase &reg)
  {
    if (regGenOverride(reg)) return true;

    return reg.is_reg_written();
  }

  bool isRegRead(const RegBase &reg)
  {
    if (regGenOverride(reg)) return true;

    return reg.is_reg_read();
  }

  bool isRegSliceWritten(const RegBase &reg)
  {
    if (regGenOverride(reg)) return true;

    return reg.is_reg_slice_written();
  }

  bool isRegSliceRead(const RegBase &reg)
  {
    if (regGenOverride(reg)) return true;

    return reg.is_reg_slice_read();
  }

  bool isFieldUsed(const RegBase &reg,const RegField &rf)
  {
    if (regGenOverride(reg)) return true;
    
    return rf.is_field_used();
  }

  bool isFieldRead(const RegBase &reg,const RegField &rf)
  {
    if (regGenOverride(reg)) return true;
    
    return rf.is_field_read();
  }

  bool isFieldWritten(const RegBase &reg,const RegField &rf)
  {
    if (regGenOverride(reg)) return true;
    
    return rf.is_field_written();
  }

  bool isFieldHelperUsed(const RegBase &reg,const RegField &rf)
  {
    if (regGenOverride(reg)) return true;
    
    return rf.is_field_helper_used();
  }

  // Ugh- major C++ hack here.  If we're generating in library-mode, then the
  // ISS is a template and we must use .template to disambiguate template
  // references such as .get<1>.  If we're not in library-mode, then the class
  // isn't a template, and so we can't use this syntax.  Who's the idiot who
  // came up with this crap?
  gc_string dotTemplate()
  {
    return (_config.library_mode()) ? "template " : "";
  }

  gc_string getRegDataObj(const RegBase &r)
  {
    gc_ostringstream ss;
    if (time_tagged()) {
      ss << "Tag<" << getRegDataType(r) << " >";
    } else {
      ss << "NoTag<" << getRegDataType(r) << " >";
    }
    return ss.str();
  }

  gc_string getRegFileBaseType(const RegFile &regfile,const gc_string &rdata)
  {
    gc_ostringstream ss;
    ss << "RegFile<" << rdata << "," << regfile.size() << "," << next_higher_p2(regfile.size()) << ">";
    return ss.str();
  }

  void genCtxSwap(const Context *ctx,int index,bool do_swap = true)
  {
    if (ctx) {
      gc_string ctxctrl = getContextControl(*ctx);
      _o << "unsigned __old__ = " << ctxctrl << "; ";
      if (do_swap) {
        _o << ctxctrl << " = " << index << ";";
      } 
      _o << "\n";
    }
  }

  void genCtxSwap(const CtxInstance &ctxi)
  {
    genCtxSwap(ctxi.ctx(),ctxi.index());
  }

  void genCtxSwapBack(const Context *ctx)
  {
    if (ctx) {
      gc_string ctxctrl = getContextControl(*ctx);
      _o << ctxctrl << " = __old__;\n";
    }
  }

  void genCtxSwapBack(const CtxInstance &ctxi)
  {
    genCtxSwapBack(ctxi.ctx());
  }

  // This is the accessor function, which might hide a context reference.  If
  // we're in a context, then we just generate the functions w/o any context
  // addressing logic, of course.
  void genRegObjAccessor(const RegBase &reg,bool is_ref,const Context *ctx)
  {
    if (is_ref || !reg.pseudo()) {
      gc_string name  = reg.name();
      gc_string obj   = getRegObjName(reg);
      gc_string type  = getRegTypeName(reg);
      gc_string dtype = getRegDataType(reg);
      gc_string dname = getRegDbgAccess(reg);

      // Is this item part of a context?  If so, then we have to modify how
      // we're accessing the item.
      gc_string pfx1, pfx2;

      if (ctx) {
        pfx1 = getContextAccess(*ctx);
        pfx2 = getContextAccess(*ctx,getContextDbgControl(*ctx));
      }

      _o << type << " &" << name << "() { return " << pfx1 << obj << "; };\n"
         << "const " << type << " &" << name << "() const { return " << pfx1 << obj << "; };\n"
          
         << type << " &" << dname << "() { return " << pfx2 << obj << "; };\n"
         << "const " << type << " &" << dname << "() const { return " << pfx2 << obj << "; };\n";
        
      if (ctx) {
        // If this is a context resource, then we also need a function for being able to access a
        // particular element.
        gc_string pfx = getContextAccess(*ctx,"ctx_index");
        _o << type << " &" << name << "(unsigned ctx_index) { return " << pfx << obj << "; };\n";
        _o << "const " << type << " &" << name << "(unsigned ctx_index) const { return " << pfx << obj << "; };\n";
      }

      _o << "\n";
    }
  }


  void genConcurHookReturn(const RegBase &rb, bool concurrent)
  {
    if (concurrent) {
      _o << "if (" << getRegBundleName(rb) << "[0].empty()) {\n"
	 << "return;\n"
	 << "}\n";
    }
  }

  gc_string getRegBundleName(const RegBase &rb) 
  {
    return rb.name() + "()._value_bundles";
  }

  void genBundleClear(const RegBase &rb, bool concurrent,const Core &core) 
  {
    if (concurrent) {
      _o << getRegBundleName(rb) << ".clear(0);\n";
      bool delayed = core.delayed_writes().count(rb.name());
      if (delayed) {
        _o << getRegBundleName(rb) << ".next();\n";
      }
    }
  }

  void genValueBundle(const RegBase &rb,const Core *core) 
  {
    if (core) {
      gc_string name = rb.name();
      if (core->concurrent_writes().count(name)) {
        // commits for the current cycle.
        unsigned d = (core->has_delayed_writes()) ? _config.max_delay_slots() : 5;
        _o << "Bundles<" << dec << rb.width() << "," << d << ">  _value_bundles;\n"; 
      }
    }
  }

  void genRegDataStructure(const Reg &reg,bool shared,const Context *ctx, const Core *core)
  {
    gc_string type  = getRegTypeName(reg);
    gc_string rtype = getRegDataType(reg);
    gc_string rdata = getRegDataObj(reg);

    if (!shared) {
      if (!reg.pseudo()) {
        _o << "struct " << type << " : public " << rdata << " {\n";
  
        if (reg.has_reserved_bits()) {
          _o << rtype << " _reserved_mask;\n";
        }
        if (reg.has_readonly_bits()) {
          _o << rtype << " _readonly_mask;\n";
        }

        genValueBundle(reg,core);

        // Generate the constructor.
        _o << type << "()";
        bool first = true;
        if (reg.has_reserved_bits()) {
          constr_init_sep(_o,first);
          _o << "_reserved_mask(\"" << reg.reserved_mask().str() << "\")";
        }
        if (reg.has_readonly_bits()) {
          constr_init_sep(_o,first);
          _o << "_readonly_mask(\"" << reg.readonly_mask().str() << "\")";
        }

        _o << "\n{};\n";

        genEnumType(reg);
        // Done with class.
        _o << "};\n\n";
      }
    }

    genRegObjAccessor(reg,false,ctx);

  }

  void generateRegDS( const RegList &regs, const Core *core)
  {
    // We don't need register data structures if we're in the external-regs
    // mode, since all data will come from an external source.
    if (extern_regs()) {
      return;
    }

    _o << "//\n"
       << "// Register Data Structures\n"
       << "//\n\n";
    ForEach(regs,i) {
      genRegDataStructure(*(i->_reg),i->_shared,i->_ctx,core);
    }
  }

  void genRegVariable(ClassData &cd,const RegBase &reg,bool is_rf,bool shared)
  {
    gc_string name  = getRegObjName(reg);
    gc_string parm  = getRegParmName(reg);
    gc_string type  = getRegTypeName(reg);

    if (!shared) {
      if (is_rf || !reg.pseudo()) {
        // Now instantiate it.  We instantiate all register-files, even aliases,
        // b/c of the need to have accessor functions, such as validIndex().
        //_o << type << " " << name << ";\n\n";    
        cd._member_vars.push_back(type + " " + name);
      }
    } else {
      // It is shared- declare reference to it.
      cd._member_vars.push_back(type + " &" + name);
      cd._constr_inits.push_back(name+"("+parm+")");
      cd._constr_args.push_back(type+" &"+parm);
    }
  }

  void genEnumType(const RegBase &r) 
  {
    if (r.is_enumerated()) {
      _o << "enum Enum" << getRegTypeName(r) << "  {";
      bool first = true;
      ForEach(r.enums(),i) {
        list_sep(_o,first);
        if (i->first != ReservedEnum) {
          _o << i->first << " = " << i->second;
        }
      }
      _o << "};\n";
    }
  }

  void generateRegVars(ClassData &cd,const RegList &regs)
  {
    // We don't need register data structures if we're in the external-regs
    // mode, since all data will come from an external source.
    if (extern_regs()) {
      return;
    }

    _o << "//\n"
       << "// Register Variables\n"
       << "//\n\n";
    ForEach(regs,i) {
      genRegVariable(cd,*(i->_reg),false,i->_shared);
    }
  }

  enum ssType { ssRead, ssWrite, ssValid, ssHasAttr, ssDbgSet, ssDbgGet };

  gc_string sparse_hook_name(const RegFile &regfile,unsigned index,ssType type,const RegFile *rf_trg = 0)
  {
    gc_ostringstream ss;
    switch (type) {
    case ssWrite:
      if (rf_trg) {
        ss << regfile.name() << "_write_" << rf_trg->name();
      } else {
        ss << regfile.name() << "_write_" << index;
      }
      break;
    case ssRead:
      if (rf_trg) {
        ss << regfile.name() << "_read_" << rf_trg->name();
      } else {
        ss << regfile.name() << "_read_" << index;
      }
      break;
    case ssHasAttr:
    case ssValid:
    case ssDbgSet:
    case ssDbgGet:
      assert(0);
    }
    return ss.str();
  }

  gc_string regAccessor(const RegName &reg,const gc_string &sfx,const gc_string &parms = "",const gc_string &ctx = "")
  {
    bool first = true;
    assert(reg);
    gc_ostringstream ss;
    ss << reg()->name() << sfx << "(" << ctx;
    if (!ctx.empty() && (reg.has_child_id() || reg.has_index() || !parms.empty())) {
      ss << ",";
    }
    if (reg.has_child_id()) {
      list_sep(ss,first);
      ss << "_child_id";
    }
    else if (reg.has_index()) {
      list_sep(ss,first);
      ss << reg.index();
    }
    if (!parms.empty()) {
      list_sep(ss,first);
      ss << parms;
    }
    ss << ")";
    return ss.str();
  }

  gc_string getRegFileAttrExpr(const RegBase &r,const Core &core)
  {
    const IntSet &regfile_attrs = core.regfile_attrs();
    const AttrList &attrs = r.attrs();
    bool first = true;
    gc_ostringstream o;
    o << "(";
    ForEach(attrs,a) {
      if (regfile_attrs.count(a->first)) {
        pfx_sep(o," | ",first);
        o << "(" << getRegFileAttrType(core) << ")" << core.name() << "::" << getRegFileAttrName(a->first);
      }
    }
    if (first) {
      o << "0";
    }
    o << ")";
    return o.str();
  }

  // Write out a switch statement for accessing registers in a sparse array.
  // For read and write operations, a hook, if present, overrides the default
  // behavior or directly accessing the register.
  void genSparseSwitch(const RegFile &regfile,ssType type,const Core *core)
  {
    const RegEntryHash &entries = regfile.entries();

    _o << "switch (index) {\n";
    ForEach(entries,i) {
      const RegEntry &entry = *(i->second);
      const RegName &rn = entry.reg();
      gc_string ctx, c_index;
      if (rn.ctx()) {
        ctx = "_ctx";
        MkGStr(c_index,rn.ctx().index());
      }
      _o << "case " << dec << entry.id() << ":\n";
      switch (type) {
      case ssWrite:
        if (const RegEntryHook *hook = entry.write()) {
          if (hook->ignore()) {
            _o << "// Write is ignored.\n";
          } else if (hook->action()) {
            auto rf = entry.reg().regfile();
            _o << sparse_hook_name(regfile,entry.id(),type,rf) << "(x";
            if (rf) _o << "," << entry.reg().index();
            _o << ");\n";
          }
        } else  {
          _o << regAccessor(rn,ctx+"_write","x",c_index) << ";\n";
        }
        _o << "break;\n";
        break;
      case ssRead:
        if (const RegEntryHook *hook = entry.read()) {
          if (hook->action()) {
            auto rf = entry.reg().regfile();
            _o << "return " << sparse_hook_name(regfile,entry.id(),type,rf) << "(";
            if (rf) _o << entry.reg().index();
            _o << ");\n";
          }
          break;
        } else {
          _o << "return " << regAccessor(rn,ctx+"_read",c_index) << ";\n";
        }
        break;
      case ssDbgSet: {
        if (rn.reg()) {
          // For a register, we set the value using its debug interface.
          const Reg &reg = *(rn.reg());
          gc_string dname = regDbgAccessor(reg,false,c_index);
          if (!dname.empty()) {
            _o << dname << ".set_value(value);\n"
               << "return true;\n";
          } else {
            _o << "return false;\n";
          }
        } else if (rn.regfile()) {
          // For a register file, we call the corresponding debug function with this new index.
          _o << "return " << rn()->name() << "_debug_set(" << rn.index() << ",value);\n";
        } else {
          // For a pseudo-entry, we simply return false.
          _o << "return false;\n";
        }
      } break;
      case ssDbgGet: {
        if (rn.reg()) {
          const Reg &reg = *(rn.reg());
          gc_string dname = regDbgAccessor(reg,true,c_index);
          if (!dname.empty()) {
            _o << "return " << dname << ".get_value();\n";
          } else {
            _o << "rc = 0;\n"
               << "return 0;\n";
          }
        } else if (rn.regfile()) {
          _o << "return " << rn()->name() << "_debug_get(" << rn.index() << ",rc);\n";
        } else {
          _o << "rc = 0;\n"
             << "return 0;\n";
        }
      } break;      
      case ssHasAttr: {
        if (core) {
          if (rn.reg()) {
            // Just a register, so attributes will be constant.
            const Reg &reg = *(rn.reg());
            _o << "return " << getRegFileAttrExpr(reg,*core) << " & attr;\n";
          } else if (rn.regfile()) {
            // If a register file, then pass on the call.
            _o << "return " << getRegTypeName(*rn.regfile()) << "::hasAttr(" << rn.index() << ",attr);\n";
          } else {
            // Nothing specified, so it can't have any attributes.
            _o << "return false;\n";
          }
        }
      } break;        
      case ssValid:
        _o << "return true;\n";
        break;
      }
    }
    _o << "default:\n";
    if (type == ssValid || type == ssDbgSet || type == ssHasAttr) {
      _o << "return false;\n";
    } else if (type == ssDbgGet) {
      _o << "rc = false;\n"
         << "return 0;\n";
    } else if (type == ssRead) {
      if (regfile.invalid_entry_read()) {
        _o << getInvalidEntryReadName(regfile) << "(index);\n";
      }
      else if (core && core->invalid_entry_read()) {
        _o << getGlobalInvalidEntryReadName() << "(" << regfile.id() << ",index);\n";
      }
      // In order to be consistent between transactional ISSs and simple ISSs,
      // we always return 0 on a bad read.  It's up to the user to check,
      // using validIndex, if an operand is invalid.  If we simply aborted,
      // then on a transactional ISS, we would abort during read_operand
      // before the exec function was able to perform the check.
      _o << "return 0;\n";
    } else {
      if (regfile.invalid_entry_write()) {
        _o << getInvalidEntryWriteName(regfile) << "(index,x);\n";
      } else if (core && core->invalid_entry_write()) {
        _o << getGlobalInvalidEntryWriteName() << "(" << regfile.id() << ",index,x);\n";
      } else {
        _o << "std::cerr << \"Attempt to access " << regfile.name() << "(\" << index << \"), which is undefined.\\n\\n\";\n"
           << "abort();\n";
      }
    }
    _o << "}\n";
  }

  bool hasStorage(const RegFile &rf)
  {
    return (!rf.pseudo() && !rf.sparse());
  }

  void writeIndexMapping(const RegFileHook *hook,const gc_string &name)
  {
    bool handled_mapping = false;
    if (hook) {
      if (!hook->aliasindices().empty()) {
        // Our alias maps to different registers, so we have to create a switch
        // statement to handle this.
        const UIntVect &indices = hook->aliasindices();
        _o << "switch (index) {\n";
        for (unsigned i = 0; i != indices.size(); ++i) {
          _o << "case " << i << ":\n"
             << "return " << indices[i] << ";\n"
             << "break;\n";
        }
        _o << "default:\n"
           << "std::cerr << \"Bad register access " << name << "(\" << index << \").\\n\\n\";\n"
           << "abort();\n"
           << "};\n";
        handled_mapping = true;
      }
    }
    if (!handled_mapping) {
      _o << "return index;\n";
    }
  }

  // Generate debug set function for a register file.
  void genRegFileDebugSet(const RegFile &regfile)
  {
    gc_string name    = regfile.name();
    gc_string type    = getRegTypeName(regfile);
    unsigned width = regfile.width();

    _o << "bool " << name << "_debug_set(unsigned index,const bits<" << dec << width << "> &value) {\n";

    gc_string dname = regFileDbgAccessor(regfile,false);
    
    if (!dname.empty()) {
      _o << "if (index >= " << dname << ".size()) { return false; };\n"
         << dname << ".set_value(" << type << "::getWriteIndex(index),value);\n"
         << "return true;\n";
    } else if (regfile.sparse()) {
      // For a sparse register file, we must create switch statements to handle
      // get/set operations to each individual element.
      genSparseSwitch(regfile,ssDbgSet,0);
    } else {
      // We don't allow gets/sets on pseudo registers- just return false.
      _o << "return false;\n";
    }
    _o << "}\n\n";
  }

  // Generate debug get function for a register file.
  void genRegFileDebugGet(const RegFile &regfile)
  {
    gc_string name    = regfile.name();
    gc_string type    = getRegTypeName(regfile);
    unsigned width = regfile.width();

    _o << "bits<" << width << "> " << name << "_debug_get(unsigned index,bool &rc) const {\n";

    gc_string dname = regFileDbgAccessor(regfile,false);

    if (!dname.empty()) {
      _o << "if (index >= " << dname << ".size()) { rc = false; return 0; };\n"
         << "rc = true;\n"
         << "return " << dname << ".get_value(" << type << "::getReadIndex(index));\n";
    } else if (regfile.sparse()) {
      // For a sparse register file, we must create switch statements to handle
      // get/set operations to each individual element.
      genSparseSwitch(regfile,ssDbgGet,0);
    } else {
      // We don't allow gets/sets on pseudo registers- just return false.
      _o << "rc = false;\n"
         << "return 0;\n";
    }
    _o << "}\n\n";
  }

  void genRegFileDebugFuncs(const RegFile &regfile)
  {
    if (!extern_regs()) {
      genRegFileDebugSet(regfile);
      genRegFileDebugGet(regfile);
    }
  }


  // Generate the data structure used for a register file.
  void genRegFileDataStructure(const RegFile &regfile,bool shared,const Context *ctx, const Core *core)
  {
    gc_string name     = regfile.name();
    gc_string type     = getRegTypeName(regfile);
    gc_string rtype    = getRegDataType(regfile);
    gc_string rdata    = getRegDataObj(regfile);
    gc_string basetype = getRegFileBaseType(regfile,rdata);

    if (!shared) {

      bool has_storage = hasStorage(regfile) && !extern_regs();

      // The data structure itself is pretty simple:  Storage, if necessary, and any helper
      // functions, such as validIndex.
      _o << "struct " << type;
      if (has_storage) {
        // Only create a storage element if the register is not a pseudo-register.
        _o << " : public " << basetype;

      }
      _o << " {\n";

      if (has_storage) {
        BitVect mask;

        if (regfile.has_reserved_bits()) {
          _o << rtype << " _reserved_mask;\n";
        }
        if (regfile.has_readonly_bits()) {
          _o << rtype << " _readonly_mask;\n";
        }

        if (core && core->parallel_execution() && !regfile.is_serial()) {
          genValueBundle(regfile,core);
	 
          mask.resize(regfile.width());
          mask.set();

          if (core && core->concurrent_writes().count(name)) {
            // Insert a setter
            _o << "void set_value(unsigned index,const " << rtype << " &x) { operator()(index).set_value(x); }\n";
          } else {
            // Insert a setter.
            _o << "void set_value(unsigned index,const " << rtype << " &x) {\n"
               << "operator()(index).set_value(x);\n"
               << "}\n";
          }
        } else {
          // Insert a setter.
          _o << "void set_value(unsigned index,const " << rtype << " &x) { operator()(index).set_value(x); }\n";

        }

        // Generate the constructor.
        _o << type << "()";
        bool first = true;
        if (regfile.has_reserved_bits()) {
          constr_init_sep(_o,first);
          _o << "_reserved_mask(\"" << regfile.reserved_mask().str() << "\")";
        }
        if (regfile.has_readonly_bits()) {
          constr_init_sep(_o,first);
          _o << "_readonly_mask(\"" << regfile.readonly_mask().str() << "\")";
        }

        _o << "\n{};\n";

        // Insert a value accessor.
        _o << rtype << " get_value(unsigned index) const { return operator()(index).get_data(); };\n";

        genEnumType(regfile);
      }

      // Valid index function:  If sparse, then we create a switch statement, else we just test
      // the index.
      _o << "static bool validIndex(unsigned index) ATTRIBUTE_USED {\n";
      if (regfile.sparse()) {
        genSparseSwitch(regfile,ssValid,0);
      } else {
        _o << "return (index < " << dec << regfile.size() << ");\n";
      }
      _o << "};\n";

      // Write the attribute-access method, if register attributes are ever used.
      if (core && !core->regfile_attrs().empty()) {
        _o << "static bool hasAttr(unsigned index," << getRegFileAttrType(*core) << " attr) ATTRIBUTE_USED {\n";
        if (regfile.sparse()) {
          genSparseSwitch(regfile,ssHasAttr,core);
        } else {
          _o << "return " << getRegFileAttrExpr(regfile,*core) << " & attr;\n";
        }
        _o << "};\n\n"
           << "template <size_t Nb> static bool hasAttr(bits<Nb> index," << getRegFileAttrType(*core) << " attr) { return hasAttr(index.uint32(),attr); };\n";
      }
      
      // We create a mapping from an input index to an aliased index.  This is the unit function unless we're
      // an alias.
      _o << "static unsigned getReadIndex(unsigned index) ATTRIBUTE_INLINE {\n";
      writeIndexMapping(regfile.readhook(),name);
      _o << "}\n";

      _o << "static unsigned getWriteIndex(unsigned index) ATTRIBUTE_INLINE {\n";
      writeIndexMapping(regfile.writehook(),name);
      _o << "}\n"

         << "template <size_t Nb> static bool validIndex(bits<Nb> x) { return validIndex(x.uint32()); };\n"

         << "};\n\n";

    }

    genRegObjAccessor(regfile,true,ctx);
    genRegFileDebugFuncs(regfile);
  }

  void generateRegFileDS(const RegFileList &rf, const Core *core)
  {
    _o << "//\n"
       << "// Register File Data Structures\n"
       << "//\n\n";
    ForEach(rf,i) {
      genRegFileDataStructure(*(i->_reg),i->_shared,i->_ctx,core);
    }
  }

  void generateRegFileVars(ClassData &cd,const RegFileList &rf)
  {
    _o << "//\n"
       << "// Register File Variables\n"
       << "//\n\n";
    ForEach(rf,i) {
      genRegVariable(cd,*(i->_reg),true,i->_shared);
    }
  }

  gc_string rnIndex(const RegName &rn)
  {
    gc_ostringstream ss;

    if (rn.has_child_id()) {
      ss << "_child_id";
    } else if (rn.has_index()) {
      // If an index exists, e.g. the register is mapped to an element of a register-file, we add on
      // that index here.
      ss << rn.index();
    }
    return ss.str();
  }

  // Generate a register access for a debug access.  Aliases are converted to refer
  // to their target.
  gc_string regDbgAccessor(const Reg &reg,bool is_read,const gc_string &c_index = "")
  {
    gc_ostringstream ss;
    // If we have an alias, then use that.
    if (const RegHook *hook = (is_read) ? reg.readhook() : reg.writehook()) {
      if (hook->alias()()) {
        const RegName &rn = hook->alias();
        if (rn.ctx()) {
          // If we refer to another context, then we must specify it here.
          ss << rn()->name() << "(";
          if (!c_index.empty()) {
            ss << c_index;
          } else {
            ss << rn.ctx().index();
          }
        } else if (rn.reg() && rn.reg()->has_alias()) {
          return regDbgAccessor(*rn.reg(),is_read,c_index);
        } else {
          // Otherwise, we use the debug form of the accessor so that, if this is a context register,
          // we'll get the relevant value for the current debug context.
          ss << getRegDbgAccess(*(rn())) << "(";
        }
        ss << ")";
        if (rn.regfile()) {
          ss << "(" << rnIndex(rn) << ")";
        }
        return ss.str();
      }
    }
    // No alias, so as long as we have storage for this register,
    // use it.
    if (!reg.pseudo()) {
      if (!c_index.empty()) {
        ss << reg.name() << "(" << c_index << ")";
      } else {
        ss << getRegDbgAccess(reg) << "()";
      }
    }
    return ss.str();
  }

  gc_string regFileDbgAccessor(const RegFile &rf,bool is_read,const gc_string &c_index = "")
  {
    if (rf.sparse()) {
      return gc_string();
    }
    gc_ostringstream ss;
    // If we have an alias, then use that.
    if (const RegFileHook *hook = (is_read) ? rf.readhook() : rf.writehook()) {
      if (hook->alias()()) {
        const RegName &rn = hook->alias();
        // We don't support aliases to sparse register files either.
        if (rn.regfile()->sparse()) {
          return gc_string();
        }
        // If we refer to another context, then we must specify it here.
        if (rn.ctx()) {
          ss << rn()->name() << "(";
          if (!c_index.empty()) {
            ss << c_index;
          } else {
            ss << rn.ctx().index();
          }
        } else {
          ss << getRegDbgAccess(*rn()) << "(";
        }
        ss << ")";
        return ss.str();
      }
    }
    // No alias, so as long as we have storage for this register,
    // use it.
    if (!rf.pseudo()) {
      if (!c_index.empty()) {
        ss << rf.name() << "(" << c_index << ")";
      } else {
        ss << getRegDbgAccess(rf) << "()";
      }
    }
    return ss.str();
  }

  unsigned regFileDbgAccessIndex(const RegFile &rf,unsigned index,bool is_read)
  {
    if (const RegFileHook *hook = (is_read) ? rf.readhook() : rf.writehook()) {
      return hook->getMappedIndex(index);
    }
    return index;
  }

  // Generate the function which maps a register name to an index.
  void genGetRegIndex(const RegList &regs,const RegFileList &regfiles)
  {
    _o << "bool getRegIndex(int &rindex,int &rfindex,const std::string &name) const\n"
       << headerOnly()
       << "{\n"
       << "rindex = rfindex = -1;\n";

    if (!regs.empty() || !regfiles.empty()) {
      // First, construct code for a table lookup of the exact name.
      _o << "typedef _STDEXT hash_map<std::string,unsigned> RegNames;\n"
         << "static RegNames::value_type inits[] = {\n";
      unsigned count = 0;
      ForEach(regs,i) {
        const Reg &reg = *(i->_reg);
        gc_string dname = regDbgAccessor(reg,false);
        if (!dname.empty()) {
          _o << "RegNames::value_type(\"" << reg.log_name() << "\"," << reg.id() << "),\n";
          ++count;
        }
      }
      ForEach(regfiles,i) {
        const RegFile &rf = *(i->_reg);
        _o << "RegNames::value_type(\"" << rf.log_name() << "\"," << rf.id() << "),\n";
        ++count;
      }
      _o << "};\n"
         << "static RegNames regnames(&inits[0],&inits[" << count << "]);\n"
         << "RegNames::const_iterator iter = regnames.find(name);\n"
         << "if (iter != regnames.end()) {\n"
         << "rindex = iter->second;\n"
         << "return true;\n"
         << "}\n";
      // If that fails, look for register-file items which concatenate the index
      // to the register-file name.
      ForEach(regfiles,i) {
        const RegFile &rf = *(i->_reg);
        _o << "if ( (rfindex = checkRegFileName(name," << rf.log_name().size() << ",\"" << rf.log_name() << "\")) >= 0 ) {\n"
           << "rindex = " << rf.id() << ";\n"
           << "return true;\n"
           << "}\n";
      }
    }

    _o << "return false;\n"
       << "}\n"
       << endHeaderOnly();
  }

  // Generate the default register map.
  void genDefaultRegMap(const DbgRegList &debug_regs)
  {
    _o << "const DbgRegItem *getDefaultRegMap()\n"
       << "{\n"
       << "static DbgRegItem dbg_reg_items[" << debug_regs.size()+1 << "] = {\n";
      
    ForEach(debug_regs,i) {
      const DbgRegElm &dr = *i;

      _o << "{ \"" << dr._name;
      if (dr.has_index()) {
        _o << dr._index;
      }
      _o << "\", " << dr._width << " },\n";
    }

    _o << "{ 0, 0 }\n"
       << "};\n"
       << "return dbg_reg_items;\n"
       << "}\n";
  }

  void genIntSetRegElm(const Reg &reg)
  {
    gc_string dname = regDbgAccessor(reg,false);

    // If we have a slice alias, then we need extra logic to set up the slice.
    gc_string setup,value = "value",finish;
    auto hook = reg.writehook();
    if (hook && hook->alias()) {
      if (hook->slice()) {
        value = "value2";
        MkGStr(setup,"{\n" << "bits<" << reg.width() <<"> " << value << " = value >> " << hook->slice_left_shift() << ";\n" 
               << value << " = " << value <<"(" << hook->slice_low() << "," << hook->slice_hi() << ");\n");
        finish = "} ";
      }
    }

    if (!dname.empty()) {
      _o << "case " << reg.id() << ":\n"
         << setup
         << dname << ".set_value(bits<" << reg.width() << ">(" << value << "));\n";
      if (_mmu_regs && _mmu_regs->count(&reg)) {
        _o << "clear_mmu_cache();\n";
      }
      _o << finish << "break;\n";
    }
  }

  void genIntSetRegFileElm(const RegFile &rf)
  {
    _o << "case " << rf.id() << ":\n"
       << "return " << rf.name() << "_debug_set(index,value);\n"
       << "break;\n";
  }

  // Generate the body of the setReg function that takes an integer as a
  // register identifier.
  void genIntSetRegBody(const RegList &regs,const RegFileList &regfiles)
  {
    if (!regs.empty() || !regfiles.empty()) {
      _o << "switch (rindex) {\n";
      ForEach(regs,i) {
        genIntSetRegElm(*(i->_reg));
      }
      ForEach(regfiles,i) {
        genIntSetRegFileElm(*(i->_reg));
      }

      _o << "default:\n"
         << "return false;\n"
         << "}\n"
         << "checkWatches();\n"
         << "return true;\n";
    } else {
      _o << "return false;\n";
    }
  }

  // Generate the body of the setRegExt function that takes an integer as a
  // register identifier.
  void genIntSetRegExtBody(const RegList &regs,const RegFileList &regfiles)
  {
    if (!regs.empty() || !regfiles.empty()) {
      _o << "switch (rindex) {\n";
      ForEach(regs,i) {
        const Reg &reg = *(i->_reg);
        if (reg.external_write_hook()) {
          _o << "case " << reg.id() << ":\n"
             << getRegExtWriteFunc(reg) << "(value);\n";
          if (_mmu_regs && _mmu_regs->count(&reg)) {
            _o << "clear_mmu_cache();\n";
          }
          _o << "break;\n";
        } else {
          genIntSetRegElm(reg);
        }
      }
      ForEach(regfiles,i) {
        const RegFile &rf = *(i->_reg);
        if (rf.external_write_hook()) {
          _o << "case " << rf.id() << ":\n"
             << getRegExtWriteFunc(rf) << "(index,value);\n"
             << "break;\n";
        } else {
          genIntSetRegFileElm(rf);
        }
      }

      _o << "default:\n"
         << "return false;\n"
         << "}\n"
         << "checkWatches();\n"
         << "return true;\n";
    } else {
      _o << "return false;\n";
    }
  }

  // Generate the body of the set-reg function which takes a gc_string name.
  void genStringSetRegBody(const gc_string &pred)
  {
    _o << "int rindex,rfindex;\n"
       << "if (!getRegIndex(rindex,rfindex,name)) {\n"
       << "return false;\n"
       << "}\n"
       << "return " << pred << "(rindex,((rfindex >= 0) ? rfindex : index),value);\n";
  }

  // Generate the body of the writeReg function that takes an integer as a
  // register identifier.
  void genIntWriteRegBody(const RegList &regs,const RegFileList &regfiles)
  {
    if (!regs.empty() || !regfiles.empty()) {
      if (!_config.log_writereg()) {
        _o << "uint64_t of = clear_tracing();\n";
      }
      _o << "bool rc = true;\n"
         << "try {\n"
         << "switch (rindex) {\n";
      ForEach(regs,i) {
        const Reg &reg = *(i->_reg);
        if (isRegWritten(reg)) {
          _o << "case " << reg.id() << ":\n";
          if (_config.log_writereg()) {
            _o << "if (Tracing && (_trace_mode & (ttRegWrite))) {\n"
               << "_logger->log_reg_ext_write(" << dec << reg.id() << ",\"" << reg.name() << "\",value);\n"
               << "}\n";
          }
          _o << reg.name() << "_write(value);\n"
             << "break;\n";
        }
      }
      ForEach(regfiles,i) {
        const RegFile &rf = *(i->_reg);
        if (isRegWritten(rf)) {
          _o << "case " << rf.id() << ":\n"
             << "if (" << getRegTypeName(rf) << "::validIndex(index)) {\n";
          if (_config.log_writereg()) {
            _o << "if (Tracing && (_trace_mode & (ttRegWrite))) {\n"
               << "_logger->log_regfile_ext_write(" << dec << rf.id() << ",\"" << rf.name() << "\",index,value);\n"
               << "}\n";
          }
          _o << rf.name() << "_write(index,value);\n"
             << "break;\n"
             << "}\n"
             << "rc = false;\n"
             << "break;\n";
        }
      }
      _o << "default:\n"
         << "rc = false;\n"
         << "}\n"
         << "}\n"
         << "catch (...) {\n";
      if (!_config.log_writereg()) {
        _o << "set_tracing(of);\n";
      }
      _o << "throw;\n"
         << "}\n";
      if (!_config.log_writereg()) {
        _o << "set_tracing(of);\n";
      }
      // Check for any context changes.
      _o << getCtxUpdateName() << "();\n"
         << "return rc;\n";
    } else {
      _o << "return false;\n";
    }
  }
  
  // Generate the body of the getReg function which takes an integer as a
  // register identifier.
  void genIntGetRegBody(const RegList &regs,const RegFileList &regfiles,const gc_string &sfx)
  {
    if (!regs.empty() || !regfiles.empty()) {
      _o << "bool rc = true;\n"
         << "switch (rindex) {\n";
      ForEach(regs,i) {
        const Reg &reg = *(i->_reg);
        gc_string dname = regDbgAccessor(reg,false);
        if (!dname.empty()) {
          // If we have a slice-alias, then we have to add on extra logic.
          auto hook = reg.writehook();
          gc_string pfx,nsfx = sfx;
          if (hook && hook->slice()) {
            pfx = "(";
            MkGStr(nsfx,"(" << hook->slice_low() << "," << hook->slice_hi() << ") << " << hook->slice_left_shift() << ")" << sfx);
          }
          _o << "case " << reg.id() << ":\n"
             << "value = " << pfx << dname << ".get_value()" << nsfx << ";\n"
             << "break;\n";
        }
      }
      ForEach(regfiles,i) {
        const RegFile &rf = *(i->_reg);
        _o << "case " << rf.id() << ":\n"
           << "value = " << rf.name() << "_debug_get(index,rc)" << sfx << ";\n"
           << "break;\n";
      }

      _o << "default:\n"
         << "return false;\n"
         << "}\n"
         << "return rc;\n";
    } else {
      _o << "return false;\n";
    }
  }

  // Generate the body of the get-reg function which takes a string name.
  void genStringGetRegBody()
  {
    _o << "int rindex,rfindex;\n"
       << "if (!getRegIndex(rindex,rfindex,name)) {\n"
       << "return false;\n"
       << "}\n"
       << "return getReg(rindex,((rfindex >= 0) ? rfindex : index),value);\n";
  }

  // Generate the body of the getReg function which takes an integer as a
  // register identifier.
  void genIntReadRegBody(const RegList &regs,const RegFileList &regfiles,const gc_string &sfx)
  {
    if (!regs.empty() || !regfiles.empty()) {
      _o << "uint64_t of = clear_tracing();\n"
         << "bool rc = true;\n"
         << "try {\n"
         << "switch (rindex) {\n";
      ForEach(regs,i) {
        const Reg &reg = *(i->_reg);
        if (isRegRead(reg)) {
          _o << "case " << reg.id() << ":\n"
             << "value = " << reg.name() << "_read()" << sfx << ";\n"
             << "break;\n";
        }
      }
      ForEach(regfiles,i) {
        const RegFile &rf = *(i->_reg);
        if (isRegRead(rf)) {
          _o << "case " << rf.id() << ":\n"
             << "if (" << getRegTypeName(rf) << "::validIndex(index)) {\n"
             << "value = " << rf.name() << "_read(index)" << sfx << ";\n"
             << "break;\n"
             << "}\n"
             << "rc = false;\n"
             << "break;\n";
        }
      }
      _o << "default:\n"
         << "rc = false;\n"
         << "}\n"
         << "}\n"
         << "catch (...) {\n"
         << "set_tracing(of);\n"
         << "throw;\n"
         << "}\n"
         << "set_tracing(of);\n"
         << "return rc;\n";
    } else {
      _o << "return false;\n";
    }
  }

  // Generate the body of the get-reg function which takes a string name.
  void genStringReadRegBody()
  {
    _o << "int rindex,rfindex;\n"
       << "if (!getRegIndex(rindex,rfindex,name)) {\n"
       << "return false;\n"
       << "}\n"
       << "return readReg(rindex,((rfindex >= 0) ? rfindex : index),value);\n";
  }

  // Returns true if we should show the register, false otherwise.
  template <class R>
  bool showRegPred(const R &reg,bool is_regfile,int *ignore_class = 0)
  {
    // Do not print if it's marked as a register to be ignored.
    for ( ; (ignore_class && *ignore_class >= 0); ++ignore_class) {
      if (reg.has_attr(*ignore_class)) {
        return false;
      }
    }

    // Do not print if this is an alias, unless it's an alias to a register in
    // another context.  This avoids extraneous alias printing, while still
    // handling the case that most people care about.  Eventually, we'll want
    // this to be controlled by a user-flag, but this should be a good default
    // for now.  Also, if we're an alias to a child-indexed register-file
    // element, then print that too.  This way, we'll see a register in the
    // child (probably a thread) as well as the parent, which is what most
    // people want.
    auto hook = reg.writehook();
    if (hook && hook->alias()) {
      if ( hook->alias().ctx() || (!is_regfile && hook->alias().regfile()) ) {
        return true;
      } else {
        return false;
      }
    }

    // Otherwise, show the register.
    return true;
  }

  gc_string intbvAccessor()
  {
    return (_config.rnumber_support()) ? "rnum()" : "uint64()";
  }

  void genShowRegBodyInternal(const RegCtxData &regdata,int *ignore_class,const gc_string &index)
  {
    ForEach(regdata._regs,i) {
      const Reg &reg = *(i->_reg);
      bool is_shared = i->_shared;
      if (showRegPred(reg,false,ignore_class)) {
        gc_string dname = regDbgAccessor(reg,true,index);
        if (!dname.empty()) {
          auto hook = reg.writehook();
          gc_string pfx,sfx;
          if (hook && hook->slice()) {
            pfx = "(";
            MkGStr(sfx,"(" << hook->slice_low() << "," << hook->slice_hi() << ") << " << hook->slice_left_shift() << ")" << sfx);
          }
          _o << "rb.report_reg(this, " << reg.id() << ",\"" << reg.name() << "\"," << pfx << dname << ".get_value()" << sfx << "." << intbvAccessor() << "," 
             << ((is_shared) ? "true" : "false") << ");\n";
        }
      }
    }
    ForEach(regdata._regfiles,i) {
      const RegFile &rf = *(i->_reg);
      gc_string type = getRegTypeName(rf);
      bool is_shared = i->_shared;
      bool log_as_reg = rf.has_attr(_rf_log_as_reg_attr);
      if (showRegPred(rf,true)) {
        for (unsigned i = 0; i != rf.size(); ++i) {
          gc_string dname = regFileDbgAccessor(rf,true,index);
          if (!dname.empty()) {
            if (log_as_reg) {
              // It's a register-file, but we're told to log it as individual registers.
              _o << "rb.report_reg(this," << rf.id() << ",\"" << rf.log_name() << i << "\"," 
                 << dname << ".get_value(" << type << "::getReadIndex( " << i << "))." << intbvAccessor() << ","
                 << ((is_shared) ? "true" : "false") << ");\n"; 
            } else {
              // Standard register-file logging.
              gc_string dname = regFileDbgAccessor(rf,true,index);
              if (!dname.empty()) {
                _o << "rb.report_regfile(this," << rf.id() << ",\"" << rf.log_name() << "\"," << i << "," 
                   << dname << ".get_value(" << type << "::getReadIndex( " << i << "))." << intbvAccessor() << ","
                   << ((is_shared) ? "true" : "false") << ");\n"; 
              }
            }
          }
        }
      }
    }
  }

  // Generate the body of the showReg function.
  void genShowRegBody(const RegCtxMap &regdata,int *ignore_class)
  {
    if (regdata.size() > 1) {
      // If we have contexts, then start off w/o context printing.
      _o << "rb.report_ctx_off(this);\n";
    }
    gc_string index;
    // Always print non-context data first.
    genShowRegBodyInternal(regdata.find(DefaultCtxName)->second,ignore_class,index);
    // Then print context data.
    ForEach(regdata,iter) {
      if (iter->first == DefaultCtxName) {
        continue;
      }

      const Context *ctx = iter->second._ctx;
      index = "ctx_index";
      _o << "for (unsigned " << index << " = 0; " << index << " != " << ctx->num_contexts() << "; ++" << index << ") {\n"
         << "rb.report_ctx_change(this," << ctx->id() << ",\"" << ctx->name() << "\"," << index << ");\n";
      
      genShowRegBodyInternal(iter->second,ignore_class,index);

      _o << "}\n";
    }

    if (regdata.size() > 1) {
      _o << "rb.report_ctx_off(this);\n";
    }
    _o << "}\n\n";
  }

  void writeRegInfo(const RegBase &r,bool shared,bool sparse,int size)
  {
    _o << "tmp.push_back(RegInfo());\n"
       << "tmp.back()._id = " << r.id() << ";\n"
       << "tmp.back()._name = \"" << r.name() << "\";\n"
       << "tmp.back()._shared = " << shared << ";\n"
       << "tmp.back()._pseudo = " << r.pseudo() << ";\n"
       << "tmp.back()._sparse = " << sparse << ";\n"
       << "tmp.back()._width = " << r.width() << ";\n"
       << "tmp.back()._size = " << size << ";\n";
    const StrIntList enums = r.enums();
    ForEach(enums,i) {
      _o << "tmp.back()._enums.push_back(EnumValueInfo("
         << i->second << ",\"" << i->first << "\"));\n";
    }
    const AttrList &attrs = r.attrs();
    ForEach(attrs,i) {
      AttrsHelper ah(&i->second);
      ah.gen_var(_o);
      _o << "tmp.back()._attrs.push_back(std::make_pair("
         << i->first << "," << ah.gen_constr() << "));\n";
    }
    _o << "tmp.back().finalize();\n";
    ForEach(r.fields(),i) {
      const RegField &rf = *(i->second);
      gc_string entry = "tmp.back()._fields.back().";
      _o << "tmp.back()._fields.push_back(RegFieldInfo());\n"
         << entry << "_name = \"" << rf.name() << "\";\n";
      ForEach(rf.bits(),biter) {
        _o << entry << "_bits.push_back(std::make_pair(" << biter->l() << "," << biter->r() << "));\n";
      }
      const AttrList &attrs = rf.attrs();
      ForEach(attrs,i) {
        AttrsHelper ah(&i->second);
        ah.gen_var(_o);
        _o << entry << "_attrs.push_back(std::make_pair("
           << i->first << "," << ah.gen_constr() << "));\n";
      }
      _o << entry << "finalize();\n";
    }
  }

  // Checks to see if any external write hooks exist for the core.
  bool has_ext_write_hooks(const RegList &regs,const RegFileList &regfiles)
  {
    ForEach(regs,i) {
      if (i->_reg->external_write_hook()) {
        return true;
      }
    }
    ForEach(regfiles,i) {
      if (i->_reg->external_write_hook()) {
        return true;
      }
    }
    return false;
  }

  // Generate a setRegs function.
  void generateRegAssistFuncs(const RegCtxMap &regdata,const RegList &regs,const RegFileList &regfiles,
                              int *ignore_class,bool is_system)
  {
    // First, generate the name => index mapping function.
    genGetRegIndex(regs,regfiles);

    if (!extern_regs()) {
      if (_config.rnumber_support()) {
        // Create the register setting function.
        _o << "virtual bool setReg(const std::string &name,unsigned index,const rnumber::RNumber &value)\n"
           << headerOnly()
           << "{\n";
        genStringSetRegBody("setReg");
        _o << "}\n"
           << endHeaderOnly()
           << "virtual bool setReg(unsigned rindex,unsigned index,const rnumber::RNumber &value)\n"
           << headerOnly()
           << "{\n";
        genIntSetRegBody(regs,regfiles);
        _o << "}\n"
           << endHeaderOnly();
      }

      _o << "virtual bool setReg(const std::string &name,unsigned index,uint64_t value)\n"
         << headerOnly() 
         << "{\n";
      genStringSetRegBody("setReg");
      _o << "}\n"
         << endHeaderOnly()
         << "virtual bool setReg(unsigned rindex,unsigned index,uint64_t value)\n"
         << headerOnly() 
         << "{\n";
      genIntSetRegBody(regs,regfiles);
      _o << "}\n"
         << endHeaderOnly();

      if (!is_system && has_ext_write_hooks(regs,regfiles)) {
        if (_config.rnumber_support()) {
          // Create the register setting function.
          _o << "virtual bool setRegExt(const std::string &name,unsigned index,const rnumber::RNumber &value)\n"
             << headerOnly()
             << "{\n";
          genStringSetRegBody("setRegExt");
          _o << "}\n"
             << endHeaderOnly()
             << "virtual bool setRegExt(unsigned rindex,unsigned index,const rnumber::RNumber &value)\n"
             << headerOnly()
             << "{\n";
          genIntSetRegExtBody(regs,regfiles);
          _o << "}\n"
             << endHeaderOnly();
        }

        _o << "virtual bool setRegExt(const std::string &name,unsigned index,uint64_t value)\n"
           << headerOnly()
           << "{\n";
        genStringSetRegBody("setRegExt");
        _o << "}\n"
           << endHeaderOnly()
           << "virtual bool setRegExt(unsigned rindex,unsigned index,uint64_t value)\n"
           << headerOnly()
           << "{\n";
        genIntSetRegExtBody(regs,regfiles);
        _o << "}\n"
           << endHeaderOnly();
      }

      if (!is_system) {
        if (_config.rnumber_support()) {
          // Create the register writing function.
          _o << "virtual bool writeReg(const std::string &name,unsigned index,const rnumber::RNumber &value)\n"
             << headerOnly()
             << "{\n";
          genStringSetRegBody("writeReg");
          _o << "}\n"
             << endHeaderOnly()
             << "virtual bool writeReg(unsigned rindex,unsigned index,const rnumber::RNumber &value)\n"
             << headerOnly()
             << "{\n";
          genIntWriteRegBody(regs,regfiles);
          _o << "}\n"
             << endHeaderOnly();
        }
        
        _o << "virtual bool writeReg(const std::string &name,unsigned index,uint64_t value)\n"
           << headerOnly()
           << "{\n";
        genStringSetRegBody("writeReg");
        _o << "}\n"
           << endHeaderOnly()
           << "virtual bool writeReg(unsigned rindex,unsigned index,uint64_t value)\n"
           << headerOnly()
           << "{\n";
        genIntWriteRegBody(regs,regfiles);
        _o << "}\n"
           << endHeaderOnly();
      }

      if (_config.rnumber_support()) {
        // Create the register getting function.
        _o << "virtual bool getReg(const std::string &name,unsigned index,rnumber::RNumber &value) const\n"
           << headerOnly()
           << "{\n";
        genStringGetRegBody();
        _o << "}\n"
           << endHeaderOnly()          
           << "virtual bool getReg(unsigned rindex,unsigned index,rnumber::RNumber &value) const\n"
           << headerOnly()
           << "{\n";
        genIntGetRegBody(regs,regfiles,".rnum()");
        _o << "}\n"
           << endHeaderOnly();
      }

      _o << "virtual bool getReg(const std::string &name,unsigned index,uint64_t &value) const\n"
         << headerOnly()
         << "{\n";
      genStringGetRegBody();
      _o << "}\n"
         << endHeaderOnly()
         << "virtual bool getReg(unsigned rindex,unsigned index,uint64_t &value) const\n"
         << headerOnly()
         << "{\n";
      genIntGetRegBody(regs,regfiles,".uint64()");
      _o << "}\n"
         << endHeaderOnly();

      if (!is_system) {
        if (_config.rnumber_support()) {
          // Create the register getting function.
          _o << "virtual bool readReg(const std::string &name,unsigned index,rnumber::RNumber &value)\n"
             << headerOnly()
             << "{\n";
          genStringReadRegBody();
          _o << "}\n"
             << endHeaderOnly()
             << "virtual bool readReg(unsigned rindex,unsigned index,rnumber::RNumber &value)\n"
             << headerOnly()
             << "{\n";
          genIntReadRegBody(regs,regfiles,".rnum()");
          _o << "}\n"
             << endHeaderOnly();
        }

        _o << "virtual bool readReg(const std::string &name,unsigned index,uint64_t &value)\n"
           << headerOnly()
           << "{\n";
        genStringReadRegBody();
        _o << "}\n"
           << endHeaderOnly()
           << "virtual bool readReg(unsigned rindex,unsigned index,uint64_t &value)\n"
           << headerOnly()
           << "{\n";
        genIntReadRegBody(regs,regfiles,".uint64()");
        _o << "}\n"
           << endHeaderOnly();
      }

      _o << "virtual void showRegs(ReportBase &rb) const \n"
         << headerOnly()
         << "{\n";
      genShowRegBody(regdata,ignore_class);
      _o << endHeaderOnly();
    }

    _o << "static RegInfos initRegInfo()\n"
       << headerOnly()
       << "{\n"
       << "RegInfos tmp;\n";
    ForEach(regs,i) {
      const Reg &reg = *(i->_reg);
      bool is_shared = i->_shared;
      writeRegInfo(reg,is_shared,false,0);
    }
    ForEach(regfiles,i) {
      const RegFile &rf = *(i->_reg);
      bool is_shared = i->_shared;
      writeRegInfo(rf,is_shared,rf.sparse(),rf.size());
    }
    _o << "sort_info(tmp);\n"
       << "return tmp;\n"
       << "}\n"
       << endHeaderOnly()
       << "static const RegInfos &baseRegInfo() {\n"
       << "static RegInfos info = initRegInfo();\n"
       << "return info;\n"
       << "}\n\n"
       << "virtual const RegInfos &getRegInfo() const {\n"
       << "return baseRegInfo();\n"
       << "}\n\n";
  }

};

class GenRegs : public GenRegsBase {
public:
  GenRegs(opfxstream &o,
          const Core &core,
          InstrWalker &walker,
          const gc_string &filename,
          RegBaseSet &mmu_regs,
          Parents &parents,
          Watches &watches,
          ClassData &cd,
          const ConfigDB &config,
          const CoreConfig &coreconfig) :
    GenRegsBase(o,cd,config,&mmu_regs),
    _core(core), 
    _walker(walker),
    _filename(filename),
    _parents(parents),
    _watches(watches),
    _coreconfig(coreconfig),
    _packet_width(0)
  {};

private:

  const Core       &_core;
  InstrWalker      &_walker;
  const gc_string     &_filename;
  Parents          &_parents;
  Watches          &_watches;
  const CoreConfig &_coreconfig;

  unsigned          _packet_width;

public:

  bool parallel_execution() const
  {
    return _core.parallel_execution();
  }

  bool rollback() const
  {
    return _config.rollback_mode();
  }

  bool ignore_mode() const
  {
    return _config.ignore_mode();
  }

  bool commit_writes() const 
  {
    return _core.parallel_execution() && !transactional_iss();
  }

  unsigned packetPoolSize() const
  {
    return _config.packet_pool_size();
  }

  // Returns true if a register or register-file requires a commit-write
  // (delayed execution update).  If immediate is false, then only returns true
  // if the write-hook is *not* immediate.  If immediate is true, then returns
  // true if the write-hook *is* immediate.
  bool is_commit_write(const RegBase &r,bool want_immediate = false) const
  {
    bool immediate = false;
    if (r.has_immediate_writehook()) {
      immediate = true;
    }
    return commit_writes() && !r.is_serial() && ((want_immediate && immediate) || (!want_immediate && !immediate));
  }

  bool hasPartialAccess(const RegBase &reg)
  {
    return (reg.is_reg_slice_written() || reg.is_reg_field_written() ||
            reg.is_reg_slice_read() || reg.is_reg_field_read());
  }

  bool needRegPartialAccess(const RegBase &reg)
  {
    if (regGenOverride(reg)) return true;

    return hasPartialAccess(reg);
  }

  bool needRegWrite(const RegBase &reg)
  {
    if (regGenOverride(reg)) return true;

    if (reg.is_reg_written()) return true;

    // If it's a context resource, we need a read and a write routine for being
    // able to load/store the context to memory.
    if (_core.is_ctx_resource(reg)) return true;

    // Need this, since the register-read callback actually writes a value to
    // the register.
    if (has_reg_read_callback(reg.name())) return true;

    // Need a write to handle the read/modify/write behavior for slices and/or fields.
    if (transactional_iss() && hasPartialAccess(reg)) return true;

    if ((is_commit_write(reg) || extern_regs()) && 
        (reg.is_reg_slice_written() || reg.is_reg_field_written())) return true;

    // Same thing- need a write function for the RMW behavior due to a hook.
    if (reg.has_writehook() && (reg.is_reg_slice_written() || reg.is_reg_field_written())) return true;

    return false;
  }

  bool needRegRead(const RegBase &reg)
  {
    if (regGenOverride(reg)) return true;

    // If it's a context resource, we need a read and a write routine for being
    // able to load/store the context to memory.
    if (_core.is_ctx_resource(reg)) return true;

    return reg.is_reg_read() || reg.is_reg_slice_read() || reg.is_reg_slice_written() || 
      reg.is_reg_field_read() || reg.is_reg_field_written(); 
  }

  bool isRegUsed(const RegBase &reg)
  {
    if (regGenOverride(reg)) return true;

    // If we have callbacks, geneate the code, since client code will expect the
    // register to exist.  If it's a context register, then we also consider it
    // used, since we allow for reading/writing of contexts.
    return reg.is_reg_used() || has_reg_read_callback(reg.name()) || has_reg_callback(reg.name())
      || _core.is_ctx_resource(reg);
  }
    
  bool has_reg_callbacks() const
  {
    return _coreconfig.has_reg_callbacks();
  }

  bool has_reg_read_callbacks() const
  {
    return _coreconfig.has_reg_read_callbacks();
  }

  bool has_reg_callback(const gc_string &r) const
  {
    return _coreconfig.reg_callbacks().count(r);
  }

  bool has_reg_read_callback(const gc_string &r) const
  {
    return _coreconfig.reg_read_callbacks().count(r);
  }

  bool have_dt() const
  {
    return _config.dep_tracking();
  }

  const StrSet &reg_callbacks() const
  {
    return _coreconfig.reg_callbacks();
  }

  const StrSet &reg_read_callbacks() const
  {
    return _coreconfig.reg_read_callbacks();
  }

  void setPacketWidth(unsigned w)
  {
    _packet_width = w;
  }

  void writeCurLocation()
  {
    adl::writeCurLocation(_o,_config,_filename);
  }

  void writeLineDirective(Environment *env,Ptree *expr)
  {
    adl::writeLineDirective(_o,_config,env,expr);
  }

  Ptree *funcBody(Environment *e,Ptree *f)
  {
    return adl::funcBody(_walker,e,f);
  }

  void genFunc(const gc_string &fname,Environment *env,Ptree *func)
  {
    adl::genFunc(_o,_config,_walker,_filename,fname,env,func);
  }

  // Given a node, returns the member variable's name that's a reference to this parent.
  Parent &getParent(const SysItem &si)
  {
    ForEach(_parents,i) {
      if (i->_si == &si) {
        return *i;
      }
    }
    RError("Node " << si.name() << " is not a registered parent of " << _core.name());
  }

  // Creates a watch expression for a parent and returns the full name of the function.
  gc_string getParentWatch(const SysItem &si,const RegBase &reg,Ptree *child_watch)
  {
    Parent &pn = getParent(si);

    auto ip = pn._watches.insert(make_pair(reg.name(),AdlGenSym()));

    ip.first->second._child_watches.insert(make_pair(child_watch,&_core));

    return ( pn._ref + "." + ip.first->second._watchfunc->ToString() );
  }

  void addRegTraceCall(const RegBase &reg,Ptree *ctx_index = 0,const gc_string &sfx = "")
  {
    if (has_reg_callback(reg.name())) {
      addRegCallback(reg);
    }
    if (_config.trace_mode()) {
      // If we're tracing the NIA, we make sure that the trace function is
      // called if branch-taken information is desired, as well as register
      // tracing.
      gc_string condtrace = (&reg == _core.nia()) ? "CondRegBranchTrace" : "CondRegTrace";
      _o << condtrace << "(" << reg.name() << "_trace" << sfx << "(" << ctx_index << "));\n";
    }
  }

  void addRegTraceCall(const RegBase &reg,const RegField &rf,Ptree *ctx_arg = 0)
  {
    if (has_reg_callback(reg.name())) {
      addRegCallback(reg);
    }
    if (_config.trace_mode()) {
      if (log_partial_masks()) {
        _o << "CondRegTrace(\n"
           << "{\n"
           << "bits<" << reg.width() << "> mask;\n";
        gen_regfield_mask(_o,"mask","",reg,rf,"");
        _o << reg.name() << "_trace(mask);\n"
           << "}\n"
           << ");\n";
      } else {
        addRegTraceCall(reg,ctx_arg);
      }
    }
  }

  void addRegCallback(const RegBase &reg,const gc_string &arg = gc_string())
  {
    _o << reg.name() << "_callback(" << arg << ");\n";
  }

  void addRegReadCallback(const Reg &reg)
  {
    if (has_reg_read_callback(reg.name())) {
      _o << reg.name() << "_read_callback();\n";
    }
  }

  void addRegReadCallback(const RegFile &rf)
  {
    if (has_reg_read_callback(rf.name())) {
      _o << rf.name() << "_read_callback(index);\n";
    }
  }

  void addRegTraceCall(const RegBase &reg,const gc_string &slice)
  {
    if (has_reg_callback(reg.name())) {
      addRegCallback(reg);
    }
    if (_config.trace_mode()) {
      if (log_partial_masks()) {
        _o << "CondRegTrace(\n"
           << "{\n"
           << "bits<" << reg.width() << "> mask;\n"
           << "mask.mask(" << slice << ");\n"
           << reg.name() << "_trace(mask);\n"
           << "}\n"
           << ");\n";
      } else {
        addRegTraceCall(reg);
      }
    }
  }

  void addRegReadTraceCall(const RegBase &reg,Ptree *ctx_index = 0,const gc_string &sfx = "")
  {
    if (_config.trace_mode() && log_reads()) {
      _o << "CondRegTrace(" << reg.name() << "_read_trace" << sfx << "(" << ctx_index << "));\n";
    }
  }

  void addRegReadTraceCall(const RegBase &reg,const RegField &rf,Ptree *ctx_arg = 0)
  {
    if (_config.trace_mode() && log_reads()) {
      if (log_partial_masks()) {
        _o << "CondRegTrace(\n"
           << "{\n"
           << "bits<" << reg.width() << "> mask;\n";
        gen_regfield_mask(_o,"mask","",reg,rf,"");
        _o << reg.name() << "_read_trace(" << ctx_arg << "mask);\n"
           << "}\n"
           << ");\n";
      } else {
        addRegReadTraceCall(reg,ctx_arg);
      }
    }
  }

  // In order to hide the implementation of how a modeler chooses to model
  // something (as a set of registers or as a register-file), we allow the use
  // of an attribute to change the logging so that a register-file update can be
  // logged as a register with the index appended to the name.
  void genRegFileLogCall(const RegFile &rf,bool is_read,const gc_string &mask_sfx,const gc_string &accessor)
  {
    const char *type  = (is_read) ? "read" : "write";
    const char *ltype = (is_read) ? "Read" : "Write";
    _o << dec;
    if (_config.trace_mode()) {
      if (rf.has_attr(_rf_log_as_reg_attr)) {
        // Log as a register.
        _o << "if (Tracing && (_trace_mode & ttReg" << ltype << ")) {\n";
        if (!is_read) genRegDtTrgCheck(rf,"index");
        _o << "char buf[" << (rf.log_name().size() + 10) << "];\n"
           << "sprintf(buf,\"" << rf.log_name() << "%d\",index);\n"
           << "_logger->log_reg_" << type << mask_sfx << "(" << rf.id() << ",buf," << accessor << ");\n"
           << "}\n";
      } else {
        // Standard:  Log as a register-file update.
        _o << "if (Tracing && (_trace_mode & ttRegfile" << ltype << ")) {\n";
        if (!is_read) genRegDtTrgCheck(rf,"index");
        _o << "_logger->log_regfile_" << type << mask_sfx << "(" << rf.id() << ",\"" << rf.log_name() << "\",index," << accessor << ");\n"
           << "}\n";
      }
    }
  }

  void addRegFileTraceCall(const RegFile &rf,Ptree *ctx_arg = 0)
  {
    if (has_reg_callback(rf.name())) {
      addRegCallback(rf,"index");
    }
    if (hasStorage(rf) && _config.trace_mode()) {
      _o << "CondRegTrace(" << rf.name() << "_trace(" << ctx_arg << "index));\n";
    }
  }

  void addRegFileTraceCall(const RegFile &regfile,const RegField &rf,Ptree *ctx_arg = 0)
  {
    if (has_reg_callback(rf.name())) {
      addRegCallback(regfile,"index");
    }
    if (hasStorage(regfile) && _config.trace_mode()) {
      if (log_partial_masks()) {
        _o << "CondRegTrace(\n"
           << "{\n"
           << "bits<" << regfile.width() << "> mask;\n";
        gen_regfield_mask(_o,"mask","",regfile,rf,"");
        _o << regfile.name() << "_trace(" << ctx_arg << "index,mask);\n"
           << "}\n"
           << ");\n";
      } else {
        addRegFileTraceCall(regfile,ctx_arg);
      }
    }
  }

  void addRegFileTraceCall(const RegFile &regfile,const gc_string &slice)
  {
    if (has_reg_callback(regfile.name())) {
      addRegCallback(regfile,"index");
    }
    if (hasStorage(regfile) && _config.trace_mode()) {
      if (log_partial_masks()) {
        _o << "CondRegTrace(\n"
           << "{\n"
           << "bits<" << regfile.width() << "> mask;\n"
           << "mask.mask(" << slice << ");\n"
           << regfile.name() << "_trace(index,mask);\n"
           << "}\n"
           << ");\n";
      } else {
        addRegFileTraceCall(regfile);
      }
    }
  }

  void addRegFileReadTraceCall(const RegFile &rf,Ptree *ctx_arg = 0)
  {
    if (hasStorage(rf) && _config.trace_mode() && log_reads()) {
      _o << "CondRegTrace(" << rf.name() << "_read_trace(" << ctx_arg << "index));\n";
    }
  }

  void addRegFileReadTraceCall(const RegFile &regfile,const RegField &rf,Ptree *ctx_arg = 0)
  {
    if (hasStorage(regfile) && _config.trace_mode() && log_reads()) {
      if (log_partial_masks()) {
        _o << "CondRegTrace(\n"
           << "{\n"
           << "bits<" << regfile.width() << "> mask;\n";
        gen_regfield_mask(_o,"mask","",regfile,rf,"");
        _o << regfile.name() << "_read_trace(" << ctx_arg << "index,mask);\n"
           << "}\n"
           << ");\n";
      } else {
        addRegFileReadTraceCall(regfile,ctx_arg);
      }
    }
  }

  void addRegBaseTraceCall(const RegBase &rb,Ptree *ctx_arg = 0)
  {
    if (const Reg *r = dynamic_cast<const Reg*>(&rb)) {
      addRegTraceCall(*r,ctx_arg);
    }
    else if (const RegFile *rf = dynamic_cast<const RegFile*>(&rb)) {
      addRegFileTraceCall(*rf,ctx_arg);
    } else {
      assert(0);
    }
  }

  void addRegBaseTraceCall(const RegBase &rb,const RegField &rf)
  {
    if (const Reg *r = dynamic_cast<const Reg*>(&rb)) {
      addRegTraceCall(*r,rf);
    }
    else if (const RegFile *regfile = dynamic_cast<const RegFile*>(&rb)) {
      addRegFileTraceCall(*regfile,rf);
    } else {
      assert(0);
    }
  }

  void addRegBaseTraceCall(const RegBase &rb,const gc_string &slice)
  {
    if (const Reg *r = dynamic_cast<const Reg*>(&rb)) {
      addRegTraceCall(*r,slice);
    }
    else if (const RegFile *regfile = dynamic_cast<const RegFile*>(&rb)) {
      addRegFileTraceCall(*regfile,slice);
    } else {
      assert(0);
    }
  }

  void addRegBaseReadTraceCall(const RegBase &rb,Ptree *ctx_arg = 0)
  {
    if (const Reg *r = dynamic_cast<const Reg*>(&rb)) {
      addRegReadTraceCall(*r,ctx_arg);
    }
    else if (const RegFile *rf = dynamic_cast<const RegFile*>(&rb)) {
      addRegFileReadTraceCall(*rf,ctx_arg);
    } else {
      assert(0);
    }
  }

  void addRegCtxTraceCall(const RegBase &reg,const Context &ctx,Ptree *ctx_index)
  {
    if (_config.trace_mode()) {
      _o << "{\n"
         << "bool change = " << getCtxChangeFunc(ctx) << "(" << ctx_index << ");\n";
      addRegTraceCall(reg,ctx_index);
      _o << getCtxChangeBackFunc(ctx) << "(change);\n"
         << "}\n";
    }
  }

  // Writes any watch expressions required, such as for parameters
  // or the MMU.
  void writeWatches(const RegBase &reg,const SysItem *parent)
  {
    bool ob = false;
    ForEach(_watches,pw) {
      if (pw->watches_reg(&reg)) {
        if (!ob) {
          _o << "{\n";
          ob = true;
        }
        if (parent) {
          // If the register is shared, then we call the actual owner's watch expression, which will
          // propagate the event to all relevant children.
          _o << getParentWatch(*parent,reg,pw->_watchfunc) << "();\n";
        } else {
          _o << pw->_writefunc << "();\n";
        }
      }
    }

    if (ob) {
      _o << "}\n";
    }

    if (_mmu_regs && _mmu_regs->count(&reg)) {
      _o << "clear_mmu_cache();\n";
    }
  }
  
  void genRegDtSrcCheck(const RegBase &reg,const gc_string &index = "-1")
  {
    if (have_dt()) {
      _o << getDepVar() << ".check_src(DepItem::dpReg," << reg.id() << "," << index << ",++" << getRegSeqVar() << ");\n";
    }
  }

  void genRegDtTrgCheck(const RegBase &reg,const gc_string &index = "-1")
  {
    if (have_dt()) {
      _o << getDepVar() << ".check_trg(DepItem::dpReg," << reg.id() << "," << index << ");\n";
    }
  }

  void genRegTraceLogic(const Reg &reg,bool add_branch_taken = false,const gc_string &sfx = gc_string())
  {
    genRegTraceLogicInternal(reg,false,add_branch_taken,sfx);
    if (log_partial_masks()) {
      genRegTraceLogicInternal(reg,true,add_branch_taken,sfx);
    }
  }

  void genRegTraceLogicInternal(const Reg &reg,bool mask,bool add_branch_taken = false,const gc_string &sfx = gc_string())
  {     
    static int cia_attr     = Data::get_attr(Cia);
    static int unarch_attr  = Data::get_attr(Unarchitected);

    if (!isRegUsed(reg)) return;

    // Ignore if the register is the current-instruction-address register or
    // unarchitected register logging is off and the register has the
    // unarchitected flag set.
    bool ignore = reg.has_attr(cia_attr) || (!log_unarch() && reg.has_attr(unarch_attr));

    gc_string data_accessor, addr_accessor;

    gc_string mask_p;
    gc_string mask_sfx;
    gc_string mask_parm;
    gc_string mask_arg;
    if (mask) {
      mask_p = "mask";
      mask_arg = ",mask." + intbvAccessor();
      mask_sfx = "_mask";
      MkGStr(mask_parm,"const bits<" << reg.width() << "> &mask");
    }

    _o << "void " << reg.name() << "_trace" << sfx << "(" << mask_parm << ") " << attr_used << "{\n";
    if (!ignore) {
      _o << "if (Tracing && (_trace_mode & ttRegWrite)) {\n";
      genRegDtTrgCheck(reg);
      if (extern_regs()) {
        MkGStr(data_accessor,getExternalRegRead(reg) << "()." << intbvAccessor());
        MkGStr(addr_accessor,getExternalRegRead(reg) << "().uint64()");
        _o << "_logger->log_reg_write" << mask_sfx << "(" << reg.id() << ",\"" << reg.log_name() << "\"," << data_accessor << mask_arg << ");\n";
      }
      else if (const RegHook *hook = reg.writehook()) {
        if (const Reg *alias = hook->alias().reg()) {
          // If we're dealing with an alias, then we call the target's trace
          // function if it's not mapped to a context register.  If it is,
          // then we log the access using this name, since that way we don't
          // have to log the fact that we're handling something in a different
          // context.
          if (hook->alias().ctx()) {
            int index = hook->alias().ctx().index();
            MkGStr(data_accessor,alias->name() << "(" << index << ").get_data()." << intbvAccessor());
            MkGStr(addr_accessor,alias->name() << "(" << index << ").get_data().uint64()");
            _o << "_logger->log_reg_write" << mask_sfx << "(" << reg.id() <<  ",\"" << reg.log_name() << "\"," << data_accessor << mask_arg << ");\n";          
          } else {
            _o << alias->name() << "_trace(" << mask_p << ");\n";
            MkGStr(addr_accessor,alias->name() << "_read_internal().uint64()");
          }
        } else if (const RegFile *alias = hook->alias().regfile()) {
          // In the case where the destination is a register file, we do want
          // tracing, as we target the internal write function which doesn't do
          // tracing itself.
          gc_string v;
          MkGStr(v,alias->name() << "_read_internal(" << rnIndex(hook->alias()) << ")");
          if (hook->slice()) {
            MkGStr(v,v << "(" << hook->slice_low() << "," << hook->slice_hi() << ")");
            if (hook->slice_left_shift()) {
              MkGStr(v,"(" << v << " << " << hook->slice_left_shift() << ")");
            }
          }
          _o << "_logger->log_reg_write" << mask_sfx << "(" << reg.id() <<  ",\"" << reg.log_name() << "\"," << v << "." << intbvAccessor() << mask_arg << ");\n";
          MkGStr(addr_accessor,alias->name() << "_read_internal(" << rnIndex(hook->alias()) << ").uint64()");
        } else if (hook->ignore()) {
          // Ignore:  No tracing.
        } else {
          // We have an action function, so as long as
          // we're not a pseudo register, use the normal
          // tracing logic.
          if (!reg.pseudo()) {
            MkGStr(data_accessor,reg.name() << "().get_data()." << intbvAccessor());
            MkGStr(addr_accessor,reg.name() << "().get_data().uint64()");
            _o << "_logger->log_reg_write" << mask_sfx << "(" << reg.id() <<  ",\"" << reg.log_name() << "\"," << data_accessor << mask_arg << ");\n";          
          }
        }
      } 
      else {
        MkGStr(data_accessor,reg.name() << "().get_data()." << intbvAccessor());
        MkGStr(addr_accessor,reg.name() << "().get_data().uint64()");
        _o << "_logger->log_reg_write" << mask_sfx << "(" << reg.id() << ",\"" << reg.log_name() << "\"," << data_accessor << mask_arg << ");\n";
      }
      _o << "}\n";

      if (add_branch_taken && (&reg == _core.nia())) {
        // If true- we should consider a write to this register to indicate a taken branch.
        _o << "if (Tracing && (_trace_mode && ttBranchTaken)) {\n"
           << "_logger->log_branch_taken(" << addr_accessor << ");\n"
           << "}\n";

        // The hook always gets called, if it's defined.
        if (_core.branch_taken_handler() && !transactional_iss()) {
          _o << getBrTakenHandlerName() << "();\n";
        }
      }
    }
    _o << "}\n\n";

    // Add callback code, if appropriate.
    if (has_reg_callback(reg.name())) {
      _o << "void " << reg.name() << "_callback()" << attr_used << "\n"
         << "{\n";
      int index = 0;
      // Yes, this is an associative data structure, but our IDs are indices
      // into an array, so we need to calculate that by counting
      // sequentially through the list.
      ForEach(reg_callbacks(),i) {
        if (*i == reg.name()) {
          if (data_accessor.empty()) {
            RError("A register callback is not supported for register " << reg.name() << " because we do not support this type of aliased/pseudo register.");
          }
          _o << "(*" << getCallbackName(false) << "[" << index << "])(0," << data_accessor << ");\n";
          break;
        }
        index++;
      }
      _o << "}\n\n";
    }

    // Add on a context-specific trace function if we're a context register.  We
    // don't do this for the mask stuff, since we're not tracking partial ops on
    // context-specific stuff.
    if (_config.trace_mode() && sfx.empty() && !mask) {
      auto ctx = _core.is_ctx_resource(reg);
      if (!extern_regs() && !reg.pseudo() && ctx) {
        _o << "void " << reg.name() << "_trace(unsigned ctx_index) " << attr_used << "{\n"
           << "if (Tracing && (_trace_mode & ttRegWrite)) {\n"
           << "_logger->log_reg_write(" << reg.id() <<  ",\"" << reg.name() << "\"," << reg.log_name() << "(ctx_index).get_data()." << intbvAccessor() << mask_arg << ");\n"
           << "}\n"
           << "}\n\n";
      }
    }

    if (log_reads()) {
      gc_string data_accessor;
      _o << "void " << reg.name() << "_read_trace" << sfx << "(" << mask_parm << ") " << attr_used << "{\n";
      if (!ignore) {
        _o << "if (Tracing && (_trace_mode & ttRegRead)) {\n";
        genRegDtSrcCheck(reg);
        if (extern_regs()) {
          MkGStr(data_accessor,getExternalRegRead(reg) << "()." << intbvAccessor());
          _o << "_logger->log_reg_read" << mask_sfx << "(" << reg.id() << ",\"" << reg.log_name() << "\"," << data_accessor << mask_arg << ");\n";
        }
        else if (const RegHook *hook = reg.readhook()) {
          if (const Reg *alias = hook->alias().reg()) {
            // If we're dealing with an alias, then we call the target's trace
            // function if it's not mapped to a context register.  If it is,
            // then we log the access using this name, since that way we don't
            // have to log the fact that we're handling something in a different
            // context.
            if (hook->alias().ctx()) {
              int index = hook->alias().ctx().index();
              MkGStr(data_accessor,alias->name() << "(" << index << ").get_data()." << intbvAccessor());
              _o << "_logger->log_reg_read" << mask_sfx << "(" << reg.id() <<  ",\"" << reg.log_name() << "\"," << data_accessor << mask_arg << ");\n";          
            } else {
              _o << alias->name() << "_read_trace(" << mask_p << ");\n";
            }
          } else if (hook->alias().regfile()) {
            // Ignore:  No tracing.  We use the register-file's tracing
            // logic instead.
          } else if (hook->ignore()) {
            // Ignore:  No tracing.
          } else {
            // We have an action function, so as long as
            // we're not a pseudo register, use the normal
            // tracing logic.
            if (!reg.pseudo()) {
              MkGStr(data_accessor,reg.name() << "().get_data()." << intbvAccessor());
              _o << "_logger->log_reg_read" << mask_sfx << "(" << reg.id() <<  ",\"" << reg.log_name() << "\"," << data_accessor << mask_arg << ");\n";          
            }
          }
        } 
        else {
          MkGStr(data_accessor,reg.name() << "().get_data()." << intbvAccessor());
          _o << "_logger->log_reg_read" << mask_sfx << "(" << reg.id() << ",\"" << reg.log_name() << "\"," << data_accessor << mask_arg << ");\n";
        }
        _o << "}\n";
      }
      _o << "}\n\n";
    }
  }

  // If true, returns a 3-element tuple representing a context-index parameter.
  // Accessor functions may be used to extract specific portions for use as a
  // parameter in a function call.
  Ptree *getCtxParm(bool has_ctx)
  {
    if (has_ctx) {
      return List(Ptree::Make("uint32_t"),Ptree::Make("ctx_index"),Ptree::Make(","));
    } else {
      return 0;
    }
  }

  // Returns the argument w/a trailing comma.  Null-safe.
  Ptree *ctx_arg(Ptree *ctx_parm)
  {
    if (ctx_parm) {
      return ctx_parm->Cdr();
    } else {
      return 0;
    }
  }

  // Returns the index only.  Null-safe.
  Ptree *ctx_index(Ptree *ctx_parm)
  {
    if (ctx_parm) {
      return ctx_parm->Cdr()->Car();
    } else {
      return 0;
    }
  }

  // Returns the parm w/o a trailing comma.  Null-safe.
  Ptree *ctx_parm_nc(Ptree *ctx_parm)
  {
    if (ctx_parm) {
      return List(First(ctx_parm),Second(ctx_parm));
    } else {
      return 0;
    }
  }

  // This adds on an index parameter if we have a context.
  gc_string genRegHookArgs(bool has_ctx,Ptree *hookaction,const RegBase *r=0)
  {
    Ptree *fa = funcArgs(hookaction);
    gc_ostringstream ss;
    ss << '(';
    if (has_ctx) {
      ss << "uint32_t " << _walker.get_ctx_index();
      if (Length(fa)) {
        ss << ", ";
      }
    }
    if (r && _core.concurrent_writes().count(r->name())) {
      int bid = get_arity(hookaction) - 1;
      Ptree *arg;
      if (bid == 1) { // regfile
        int i=0;
        arg = NthDeclarator(hookaction,i); 
        ss << First(arg) << " " << Second(arg) << ",";
      }
      arg = PtreeUtil::NthDeclarator(hookaction,bid);
      ss << "const " << First(arg) << "<" << r->width() << ">& " << Second(arg) << ")" ;
    } else {
      ss << fa << ')';
    }
    return ss.str();
  }
    
  gc_string genSliceArgs(const gc_string &left, const gc_string &right, const Core &core, const gc_string &offset = "") {
    gc_ostringstream os;
    if (core.is_big_endian()) {
      os << offset << left << "," << offset << right;
    } else {
      os << offset << right << "," << offset << left;
    }
    return os.str();
  }
    
  void writeIgnoreCheck()
  {
    if (ignore_mode()) {
      _o << "if (" << getIgnoreEnableName() << ") return;\n";
    }
  }

  void writeRegDelayBody(const RegBase &reg,const gc_string &ctype,
                         const gc_string &fargs,const gc_string &field,
                         const gc_string &index,const gc_string &index1,
                         const gc_string &index2,const gc_string &delay,
                         const gc_string &queue = "",const gc_string &sfx = "",
			 const gc_string &btype = "",const gc_string &value="",
			 const gc_string &mask="")
  {
    gc_string name = reg.name();

    gc_string sfx_p = (sfx.empty()) ? "_commit_write" : sfx;
    gc_string queue_p = (queue.empty()) ? getCommitQueueName() : queue;
    gc_string delay_p = (delay.empty()) ? "" : ("," + delay);
    bool   concurrent = (!btype.empty() && !value.empty() && !mask.empty());

    gc_string commitWrite = "&" + _core.name() + "::" + name + field + sfx_p;
    if (fargs.empty()) {
      _o << "decltype(" << commitWrite << ") pCommit = " << commitWrite << ";\n";
    } else {
      _o << "typedef void (" << _core.name() << "::*Functor)(" << fargs << ");\n"
         << "Functor pCommit = " << commitWrite << ";\n";
    }
    _o << "static PacketPool<" << ctype << "<decltype(this),decltype(pCommit)," << reg.width() 
       << "," << packetPoolSize() << ">, " << packetPoolSize() << ">   pool;\n"
       << ctype << "<decltype(this),decltype(pCommit)," << reg.width() << "," << packetPoolSize() << "> *cmt = pool.alloc();\n"
       << "cmt->reset(this,pCommit," << index << index1 << index2 << "x,pool);\n"
       << queue_p << ".add(cmt" << delay_p << ");\n";
    if (concurrent) {
      _o << "static PacketPool<" << btype << ", " << packetPoolSize() << "> bundlePool;\n"
         <<  btype <<  " *be = bundlePool.alloc();\n"
         << "bits<" << reg.width() << ">  value, mask;\n"
         << "value" << value << ";\n"
         << "mask"  << mask  << ";\n";

      _o << "be->reset(cmt," << _core.cia()->name() << "_read().uint64()," << index << "value,mask,bundlePool);\n"
         << getRegBundleName(reg) << ".add(be" << delay_p << ");\n";
    }
  }

  void writeRegSaveCall(const RegBase &reg,const gc_string &index)
  {
    if (rollback()) {
      _o << reg.name() << "_save(";
      if (!index.empty()) {
        _o << index;
      }
      _o << ");\n";
    }
  }

  void writeRegOuterWriteBody(const Reg &reg,const Context *ctx,bool has_write_hook,const gc_string &sfx = "")
  {
    gc_string name = reg.name();

    gc_string ctx_ctrl = (ctx) ? getContextControl(*ctx)+"," : "";
    _o << name << "_write_internal" << sfx << "(" << ctx_ctrl << "x);\n";
    if (!has_write_hook) {
      // No need for this trace call if we have a write-hook, since the logging
      // is done by the hook.
      addRegTraceCall(reg,0,sfx);
    }
  }

  // Add in code, if appropriate, for calling a read-callback.
  void writeRegReadCallback(const RegBase &reg,const Context *ctx,gc_string rindex,const gc_string &sfx = "")
  {
    if (has_reg_read_callback(reg.name())) {
      const gc_string &name = reg.name();
      gc_string ctx_ctrl = (ctx) ? getContextControl(*ctx)+"," : "";

      _o << "void " << name << "_read_callback(";
      if (!rindex.empty()) {
        _o << "unsigned " << rindex;
      }
      _o << ")" << attr_used << "\n"
         << "{\n";
      int index = 0;

      gc_string rpindex;
      if (rindex.empty()) {
        rindex = "0";
      } else {
        rpindex = rindex + ",";
      }
      // Yes, this is an associative data structure, but our IDs are indices
      // into an array, so we need to calculate that by counting
      // sequentially through the list.
      ForEach(reg_read_callbacks(),i) {
        if (*i == reg.name()) {
          _o << "RegReadCallback *cb = " << getCallbackName(true) << "[" << index << "];\n"
             << "if (cb) {\n"
             << name << "_write_internal(" << ctx_ctrl << rpindex << "(*cb)(" << rindex << "));\n"
             << "}\n";
          break;
        }
        index++;
      }
      _o << "}\n\n";
    }
  }

  void genRegOuterWrite(const Reg &reg,bool commit_write,bool needs_hook_write,bool concurrent,
                        const Context *ctx,const gc_string &rtype,bool always_inline, const gc_string &sfx = "")
  {
    gc_string name = reg.name();

    bool delayed = _core.delayed_writes().count(name) && sfx.empty();

    gc_string attr_inline = (always_inline) ? " ATTRIBUTE_INLINE " : "";

    gc_string rval,rmask,btype;
    if (concurrent) {
      rval = " = x";
      rmask = ".set()";
      MkGStr(btype,"RegBundleElem<" << reg.width() << '>');
    }
    if (delayed) {
      _o << "\n";
      _o << "void " << name << "_write_delayed" << sfx << "(const " << rtype << " &x,unsigned d) {\n";
      writeIgnoreCheck();
      writeRegDelayBody(reg,"CommitRegWrite","","","","","","d","","",btype,rval,rmask);
      _o << "}\n\n";
    }

    _o << "void " << name << "_write" << sfx << "(const " << rtype << " &x) " << attr_inline << "{\n";
    
    writeIgnoreCheck();
    if(commit_write) {
      writeRegDelayBody(reg,"CommitRegWrite","","","","","","0","","",btype,rval,rmask);
      _o << "}\n";
      _o << "void " << name << "_commit_write" << sfx << "(const " << rtype << " &x) {\n"; // Add the suffix
      
      if ((reg.name() == _core.cia()->name()) && sfx.empty()) {
        const_cast<Core&>(_core).set_CIA_commit_write(); // Set the flag to show CIA_commit_write() is available in the ISS model
      }
    }

    writeRegOuterWriteBody(reg,ctx,needs_hook_write,sfx);

    _o << "};\n";

    // I'm generating the read-callback code here, because I have access to the
    // necessary parms for generating the call.
    writeRegReadCallback(reg,ctx,gc_string(),sfx);

    // If we have a delayed write, then we also need a commit_write, in addition to a normal write.
    if (delayed && !commit_write) {
      _o << "void " << name << "_commit_write(const " << rtype << " &x) {\n";        
      writeRegOuterWriteBody(reg,ctx,needs_hook_write);
      _o << "};\n";
    }
  }

  gc_string genWriteExpr(const RegBase &r,const gc_string &name,bool is_regfile)
  {
    gc_string expr;

    if (r.has_reserved_bits()) {
      expr = "(x & ~" + name + "()._reserved_mask)";
    } else {
      expr = "x";
    }
    gc_string index = (is_regfile) ? "(index)" : "";
    if (r.has_readonly_bits()) {
      expr = "(" + expr + " & ~" + name + "()._readonly_mask) | (" + name + "()" + index + ".get_data(" + tta() + ") & " + name + "()._readonly_mask)";
    }

    return expr;
  }

  gc_string hookSliceArgs(const RegBase &reg)
  {
    gc_ostringstream ss;
    if (reg.has_start_stop()) {
      ss << ",unsigned start = 0,unsigned stop = " << reg.width()-1;
    }
    return ss.str();
  }

  gc_string hookSliceParms(const RegBase &reg)
  {
    gc_string p;
    if (reg.has_start_stop()) {
      p = ",start,stop";
    }
    return p;
  }

  void writeAliasMaskingExpr(const RegBase &reg,const RegName &alias,const gc_string &rtype)
  {
    bool is_regfile = alias.regfile();

    bool has_rb = false;
    if (reg.has_reserved_bits()) {
      _o << "static const " << rtype << " __reserved_mask(\"" << reg.reserved_mask().str() << "\");\n";
      has_rb = true;
    }
    if (reg.has_readonly_bits()) {
      _o << "static const " << rtype << " __readonly_mask(\"" << reg.readonly_mask().str() << "\");\n";
      has_rb = true;
    }

    if (!has_rb) return;

    gc_string expr;
    if (reg.has_reserved_bits()) {
      expr = "(x & ~__reserved_mask)";
    } else {
      expr = "x";
    }
    gc_string index;
    if (is_regfile) {
      if (alias.has_child_id()) {
        index = "(_child_id)";
      } else if (alias.has_index()) {
        gc_ostringstream ss;
        ss << "(" << alias.index() << ")";
        index = ss.str();
      } else {
        index = "(" + getRegTypeName(reg) + "::getWriteIndex(index))";
      }
    }
    const RegBase &trg = *alias();
    if (reg.has_readonly_bits() && !trg.pseudo()) {
      expr = "(" + expr + " & ~" + "__readonly_mask) | (" + trg.name() + "()" + index + ".get_data(" + tta() + ") & " + "__readonly_mask)";
    }
    
    _o << "x = " << expr << ";\n";
  }

  // Generate an in-hook write, if necessary.  This is the function called to
  // perform a write from within a write-hook.
  void genRegInHookWrite(const Reg &reg,const gc_string &name,Ptree *cp,const gc_string &rtype,const gc_string &sfx)
  {
    // Function for writing directly to a register- used within a hook or
    // whereever direct access is required.
    _o << "void " << name << "_write_inhook" << sfx << "(" << cp << "const " << rtype << " &x) " << attr_used << "{\n";
    
    if (reg.pseudo()) {
      // If we're here and we're a pseudo register, then we need this function
      // for a reset action.  Thus, we just call our own internal write routine.
      _o << name << "_write_internal(x);\n"
         << "}\n";
      return;
    }

    if (is_commit_write(reg,true)) {
      // The more complicated case: We have an immediate write-hook, which means
      // that the write-hook is called during normal execution, but all of the
      // updates are delayed.  This is as opposed to a non-immediate write-hook
      // (for a parallel architecture), in which the write-hook is called during
      // the commit phase.
      writeIgnoreCheck();
      writeRegDelayBody(reg,"CommitRegWrite","","","","","","0","","","","","");
      _o << "}\n\n"
         << "void " << name << "_commit_write(const " << rtype << " &x) {\n";
      // Then fall through and create the simple write function, which in this
      // case will be executed at commit-time.
    }

    gc_string expr = genWriteExpr(reg,name,false);
    auto ctx = _core.is_ctx_resource(reg);

    // Standard situation- a simple function for updating the register's
    // value, to be called within a write-hook.
    if (extern_regs()) {
      _o << getExternalRegWrite(reg) << "(x);\n";
    } else {
      _o << name << "(" << ctx_index(cp) << ").set_data(" << tta() << ") = " << expr << ";\n";
      addRegTraceCall(reg,0,sfx);
    }
    _o << "}\n";
    if (sfx.empty()) {
      if (ctx) {
        _o << "void " << name << "_ctx_write(" << cp << "const " << rtype << " &x) " << attr_used << "{\n"
           << name << "_write_internal(" << ctx_index(cp) << ",x);\n";
        addRegCtxTraceCall(reg,*ctx,ctx_index(cp));
        _o << "}\n";
      }
    }
  }

  // Generate a register write hook.  //
  // sfx: Suffix to add to the register-write functions, for creating
  //      special-purpose writes for different types of registers.
  void genRegWriteHook(bool &needs_write_hook,bool &needs_save,bool &concurrent,bool &has_start_stop,
                       const Reg &reg,const gc_string &rtype,bool always_inline,const gc_string &sfx = "")
  {
    // Insert a default writehook if one doesn't exist, otherwise use
    // the actual writehook.
    // This code handles the write logic but *doesn't* do the tracing- that's
    // handled by the outer-most function.  Aliases call this function so that
    // they can have their own tracing logic, e.g. a write to A, which is an alias
    // to B, should report that A was written.
    gc_string name = reg.name();

    if (!needRegWrite(reg)) return;

    needs_save = false;
    needs_write_hook = false;
      
    _o << "void " << name << "_write_internal" << sfx << "(";

    // If we have a context resource then the internal write hook takes a context index.
    auto ctx = _core.is_ctx_resource(reg);
    auto cp = getCtxParm(ctx);

    // Only does anything if we have a context.
    _o << cp;

    bool write_alias = false;
    bool handled = false;
    bool immediate = false;
    concurrent = _core.concurrent_writes().count(name);
    if (const RegHook *hook = reg.writehook()) {
      if (hook->immediate()) {
        // If the hook is marked as an immediate write, then we generate the
        // delayed-commits in the in-hook functions.
        immediate = true;
      }
      if (Ptree *hookaction = hook->action()) {
        _o << "const " << rtype << " &x" << hookSliceArgs(reg) << ") " << attr_used << "{\n";
        genConcurHookReturn(reg,concurrent);
        writeRegSaveCall(reg,"");
        needs_save = true;
        gc_string hook_arg = concurrent ? (getRegBundleName(reg) + "[0]") : "x";
        has_start_stop = hook->has_start_stop();
        _o << name << "_write_hook" << sfx << "(" << ctx_arg(cp) << " " << hook_arg << hookSliceParms(reg) << ");\n";
        genBundleClear(reg,concurrent,_core);
        _o << "};\n";
        // Action code:  Process and write it out.
        _walker.set_reg_sfx(sfx);
        _o << "void " << name << "_write_hook" << sfx << genRegHookArgs(ctx,hookaction,&reg) << " " << attr_used << "{\n";
        writeLineDirective(hook->env(),hookaction);
        _o << funcBody(hook->env(),hookaction) << "\n";
        writeCurLocation();
        handled = true;
        needs_write_hook = true;
        _walker.clear_reg_sfx();
      } else if (hook->ignore()) {
        // Ignore:  Silently ignore the write.
        _o << rtype << ") {\n";
        handled = true;
      } else if (hook->alias()()) {
        // Alias:  Modify the target of the alias instead of this register.
        if (reg.has_reserved_bits() || reg.has_readonly_bits()) {
          _o << rtype << " x) " << attr_used << "{\n";
        } else {
          _o << "const " << rtype << " &x) {\n";
        }
        writeAliasMaskingExpr(reg,hook->alias(),rtype);
        gc_string ctx_parm;
        if (hook->alias().ctx()) {
          MkGStr(ctx_parm,hook->alias().ctx().index());
        }
        else if (const Context *a_ctx = _core.is_ctx_resource(*hook->alias()())) {          
          MkGStr(ctx_parm,getContextControl(*a_ctx,true));
        }
        if (hook->slice()) {
          gc_string parm;
          if (hook->slice_left_shift()) {
            MkGStr(parm,hook->slice_low() << "," << hook->slice_hi() << ",(x >> " << hook->slice_left_shift() << ")");
          } else {
            MkGStr(parm,hook->slice_low() << "," << hook->slice_hi() << ",x");
          }
          _o << regAccessor(hook->alias(),((hook->alias().regfile()) ? "_slice_set_internal" : "_slice_set"),parm,ctx_parm) << ";\n";
        } else {
          _o << regAccessor(hook->alias(),"_write_internal","x",ctx_parm) << ";\n";
        }
        handled = true;
        write_alias = true;
      }
    }

    bool commit_write = is_commit_write(reg);
     		       		      
    gc_string expr = genWriteExpr(reg,name,false);

    // Default:  Set the register to the specified value.  Take into account
    // reserved and read-only masks, if necessary.
    if (!handled) {
      // The write must take into account reserved and/or read-only masks.
      handled = true;
      gc_string ctx_parm = (ctx) ? "ctx_index" : "";
      _o << "const " << rtype << " &x) {\n";
      writeRegSaveCall(reg,"");
      needs_save = true;
      if (extern_regs()) {
        _o << getExternalRegWrite(reg) << "(x);\n";
      } else {
        _o << name << "(" << ctx_parm << ").set_data(" << tta() << ") = " << expr << ";\n";
      }
    }
    writeWatches(reg,_core.is_shared(reg));
    _o << "};\n";

    // The outer function.  We only turn it into a commit-write if we're not an
    // alias.  Otherwise, the target of this alias will handle the commit.
    genRegOuterWrite(reg,commit_write && !write_alias,needs_write_hook,concurrent,ctx,rtype,always_inline,sfx);

    // If we have a reset action, then we do need this, but we don't want
    // tracing to be altered.  If we have a read hook, then it's possible that
    // the register is modified by the read, so we want a in-register write
    // function as well.
    if (reg.resettype() == RegBase::Action || reg.readhook()) {
      needs_write_hook = true;
    }

    if (needs_write_hook || ctx || reg.external_write_hook()) {
      genRegInHookWrite(reg,name,cp,rtype,sfx);
    }
  }

  // If relevant, generate code for an external-write function.  This is a
  // function which is called by an external user of the model, such as a
  // systems model.  This lets various actions occur when the register is set
  // externally, such as write-1-to-clear, etc.
  void genRegExtWriteHook(const RegBase &reg)
  {
    if (Ptree *ew = reg.external_write_hook()) {
      _o << "void " << getRegExtWriteFunc(reg) << "(" << funcArgs(ew) << ") {\n";
      writeLineDirective(reg.env(),ew);
      _o << funcBody(reg.env(),ew) << "\n";
      writeCurLocation();
      _o << "}\n\n";
    }
  }

  // Generate a register read hook.
  void genRegReadHook(const Reg &reg,const gc_string &base_rtype,bool has_write_hook)
  {
    auto ctx = _core.is_ctx_resource(reg);
    auto cp = getCtxParm(ctx);

    if (!needRegRead(reg)) return;

    // Insert a default readhook if one doesn't exist, otherwise use
    // the actual readhook.
    bool handled = false;
    gc_string name = reg.name();
    gc_string rtype = base_rtype;

    // We need the inhook read function if we have a write hook or if we have a read hook.
    bool needs_read_hook = has_write_hook || reg.external_write_hook();
    bool simple_reg = false;

    if (const RegHook *hook = reg.readhook()) {
      if (Ptree *hookaction = reg.readhook()->action()) {
        _o << rtype << " " << name << "_read_internal" << genRegHookArgs(ctx,hookaction) << " " << attr_used << "{\n";
        // Action:  Transform and write out the action body.
        writeLineDirective(hook->env(),hookaction);
        _o << funcBody(hook->env(),hookaction) << "\n";
        writeCurLocation();
        handled = true;
        needs_read_hook = true;
      } else if (hook->alias()()) {
        gc_string slice;
        if (hook->slice()) {
          if (hook->slice_left_shift()) {
            MkGStr(slice,"(" << hook->slice_low() << "," << hook->slice_hi() << ") << " << hook->slice_left_shift());
          } else {
            MkGStr(slice,"(" << hook->slice_low() << "," << hook->slice_hi() << ")");
          }
        }
        // Alias:  Return the alias's value instead.
        // If we map to a context resource, then we need to temporarily swap that context.
        const CtxInstance &ctxi = hook->alias().ctx();
        const Context *ctx = ctxi.ctx();
        if (ctx) {
          gc_string parm;
          MkGStr(parm,ctxi.index());
          _o << rtype << " " << name << "_read_internal() " << attr_used << "{\n";
          _o << rtype << " tmp = " << regAccessor(hook->alias(),"_read_internal","",parm) << ";\n";
          _o << "return tmp" << slice << ";\n";
        } else {
          // If the target of the alias is special at all (has a readhook), then
          // we can't return a reference to the value- we have to return a copy.
          // This is also the case if the sizes are different.
          if ( !hook->alias()()->has_readhook() && (reg.width() == hook->alias()()->width()) && !hook->slice() ) {
            rtype = "const " + rtype + " &";
          }
          gc_string ctx_parm;
          if (const Context *a_ctx = _core.is_ctx_resource(*hook->alias()())) {          
            ctx_parm = getContextControl(*a_ctx,true);
          }
          _o << rtype << " " << name << "_read_internal() " << attr_used << "{\n"
             << "return " << regAccessor(hook->alias(),"_read_internal","",ctx_parm) << slice << ";\n";
        }
        handled = true;
      }
    }
    if (!handled) {
      if (extern_regs()) {
        _o << rtype << " " << name << "_read_internal(" << ctx_parm_nc(cp) << ") " << attr_used << "{\n"
           << "return " << getExternalRegRead(reg) << "();\n";
      } else {
        rtype = base_rtype + " &";
        _o << rtype << " " << name << "_read_internal(" << ctx_parm_nc(cp) << ") {\n"
         << "return " << name << "(" << ctx_index(cp) << ").get_data(" << tta() << ");\n";
        simple_reg = true;
      }
    }
    _o << "};\n\n";

    if (needs_read_hook && !reg.pseudo()) {
      // Needs a read-hook read function.
      if (extern_regs()) {
        _o << base_rtype << " " << name << "_read_inhook(" << ctx_parm_nc(cp) << ") " << attr_used << "{\n"
           << "return " << getExternalRegRead(reg) << "();\n";
      } else {
        _o << base_rtype << " &" << name << "_read_inhook(" << ctx_parm_nc(cp) << ") " << attr_used << "{\n"
           << "return " << name << "(" << ctx_index(cp) << ").get_data(" << tta() << ");\n";
      }
      _o << "}\n\n";
    }

    gc_string ctx_ctrl = (ctx) ? getContextControl(*ctx) : "";

    _o << rtype << " " << name << "_read() " << attr_used << "{\n";
    addRegReadCallback(reg);
    addRegBaseReadTraceCall(reg);
    _o << "return " << name << "_read_internal(" << ctx_ctrl << ");\n"
       << "}\n";

    if (log_reads()) {
      // This is the form used by field/slice accessors when register-read logging is on, in order
      // to avoid an erroneous read log message.
      _o << rtype << " " << name << "_read_notrace() {\n";
      addRegReadCallback(reg);
      _o << "return " << name << "_read_internal(" << ctx_ctrl << ");\n"
         << "}\n";
    }

    if (time_tagged() && !extern_regs() && simple_reg) {
      // For time-tagged ISSs, add a form which returns the value, but logs this
      // as a target.  Used by field and slice accessors to correctly log a
      // read/modify/write operation. Only needed for "simple" registers, since
      // more complex ones require an explicit read/modify/write operation.
      _o << rtype << " " << name << "_read_modify() {\n";
      _o << rtype << "tmp = " << name << "_read_internal(" << ctx_ctrl << ");\n"
         << name << "(" << ctx_ctrl << ").add_target(*this);\n"
         << "return tmp;\n"
         << "}\n";      
    }

    // This is a simple read function for non-pseudo registers which allows access to a particular
    // thread context.  This is only created if we are a context resource.
    if (!reg.pseudo() && ctx) {
      _o << rtype << " " << name << "_ctx_read(" << ctx_parm_nc(cp) << ") {\n"
         << "return " << name << "_read_internal(" << ctx_index(cp) << ");\n"
         << "}\n";
    }
  }

  // If we're supporting rollback, then generate the save and restore
  // functions.  These should be relatively simple, because we're avoiding all
  // of the complexities of hooks, aliases, etc.  We assume that this function
  // is only called for real registers, and not for pseudo/aliases.
  void genRegSaveLogic(const RegBase &reg,const gc_string &rtype,
                       const gc_string &ctype,const gc_string &index)
  {
    if (rollback()) {

      gc_string ni;

      gc_string name = reg.name();

      auto ctx = _core.is_ctx_resource(reg);
      gc_string ctx_ctrl;
      if (ctx) {
        ctx_ctrl = "(" + getContextControl(*ctx) + ")";
      } else {
        ctx_ctrl = "()";
      }
      gc_string index_p,index_p1,index_arg,index1;
      if (!index.empty()) {
        index_p   = "unsigned " + index;
        index_p1  = index_p + ",";
        index1    = index + ',';
        index_arg = "(" + index + ")";
      }

      // This creates the save function, which pushes values onto a restore queue.
        _o << ni << "void " << name << "_save(" << index_p << ") " << attr_used << "{\n";
      if (!reg.pseudo()) {
        _o << "if (" << getRollbackEnableName() << ") {\n"
           << rtype << " x = " << name << ctx_ctrl << index_arg << ".get_data(" << tta() << ");\n";
        writeRegDelayBody(reg,ctype,"","",index1,"","","",getRestoreStackName(),"_restore");
        _o << "}\n";
      }
      _o << "}\n\n";

      // This is the restore function, which is called by an element in the
      // restore-queue.
      _o << ni << "void " << name << "_restore(" << index_p1 << " const " << rtype << " &x) " << attr_used << "{\n";
      if (!reg.pseudo()) {
        _o << name << ctx_ctrl << index_arg << ".set_data(" << tta() << ") = x;\n";
        writeWatches(reg,_core.is_shared(reg));
      }
      _o << "}\n";
    }
  }

  // Returns true if we have a simple alias, e.g. not a complex alias involving read/write hooks.
  bool reg_has_simple_alias(const Reg &reg)
  {
    if (const RegHook *hook = reg.writehook()) {
      if (hook->alias()) {
        return true;
      }
    }
    return false;
  }

  // Generate register reset code.
  void genRegReset(const Reg &reg,const gc_string &rtype)
  {
    gc_string name = reg.name();

    // Insert tag reset, if necessary.
    if (time_tagged() && !reg.pseudo()) {
      _o << "void " << name << "_reset_tag() { " << name << "().reset_tag(); }\n";
    }

    auto ctx = _core.is_ctx_resource(reg);
    auto cp = getCtxParm(ctx);

    // Insert reset function.
    _o << "void " << name << "_reset_value(" << ctx_parm_nc(cp) << ") {\n";
    if (!extern_regs() && (!reg.pseudo() || reg_has_simple_alias(reg))) {
      // We create a reset function if we're either an alias or not a pseudo
      // register.  The ability to reset an alias is useful so that you can
      // easily reset, to different values, registers which alias
      // context-based registers.
      switch (reg.resettype()) {
      case RegBase::None:
      case RegBase::Value: {
        bool handled = false;
        _o << rtype << " x(\"" << reg.resetvalue() << "\");\n";
        if (const RegHook *hook = reg.writehook()) {
          if (hook->alias()()) {
            gc_string ctx_parm;
            if (hook->alias().ctx()) {
              MkGStr(ctx_parm,hook->alias().ctx().index());
            }
            else if (const Context *a_ctx = _core.is_ctx_resource(*hook->alias()())) {          
              MkGStr(ctx_parm,getContextControl(*a_ctx,true));
            }
            _o << regAccessor(hook->alias(),"_write_internal","x",ctx_parm) << ";\n";
            handled = true;
          }
        }
        if (!handled) {
          if (extern_regs()) {
            _o << getExternalRegWrite(reg) << "(x);\n";
          } else {
            _o << name << "().set_value(x);\n";
          }
        }
        if (!reg.pseudo() && !extern_regs()) {
          _o << name << "().reset_tag();\n";
        }
        _o << "};\n";
      } break;      
      case RegBase::Enum: 
        break;
      case RegBase::Action: {
        Ptree *action = reg.resetaction();
        _o << name << "_reset_action(" << ctx_arg(cp) << ctx_index(cp) << ");\n";
        if (!reg.pseudo() && !extern_regs()) {
          _o << name << "(" << ctx_index(cp) << ").reset_tag();\n";
        }
        _o << "};\n"
           << "void " << name << "_reset_action" << genRegHookArgs(ctx,action) << " {\n";
        writeLineDirective(reg.env(),action);
        _o << funcBody(reg.env(),action) << "\n";
        writeCurLocation();
        _o << "}\n";
      }
      default:
        ;
      }
    } else {
      _o << "}\n";
    }
  }

  bool has_hooks(const Reg &reg) const
  {
    return (reg.readhook() || reg.writehook());
  }

  bool has_hooks(const RegFile &reg) const
  {
    return (reg.readhook() || reg.writehook() || reg.sparse());
  }

  bool has_hooks(const RegBase &rb) const
  {
    if (const Reg *r = dynamic_cast<const Reg*>(&rb)) {
      return has_hooks(*r);
    }
    else if (const RegFile *rf = dynamic_cast<const RegFile*>(&rb)) {
      return has_hooks(*rf);
    } else {
      assert(0);
    }
  }


  void writeIndexedFieldHelpers(const RegBase &reg,const RegField &rf,const gc_string &rtype,const gc_string &rftype,
                                const gc_string &left, const gc_string &right)
  {
    // Register creation for transactional ISSs is done twice and we only want
    // these functions to be done once, so we place the guard here.
    _o << rftype << " " << getRegFieldGetter(reg,rf) << " (const " << rtype << " &x,unsigned i) {\n"
       << "return x." << dotTemplate() << " get<" << rf.width() << ">(" << genSliceArgs(left,right,_core,"") << ");\n"
       << "}\n\n"
       << rtype << " " << getRegFieldSetter(reg,rf) << " (" << rtype << " x,unsigned i,const " << rftype << " &v) {\n"
       << "x." << dotTemplate() << " set<" << rf.width() << ">(" << genSliceArgs(left,right,_core,"") << ",v);\n"
       << "return x;\n"
       <<"}\n\n";
  }

  void writeFieldHelpers(const RegBase &reg,const RegField &rf,const gc_string &rtype,const gc_string &rftype)
  {
    // Register creation for transactional ISSs is done twice and we only want
    // these functions to be done once, so we place the guard here.
    _o << rftype << " " << getRegFieldGetter(reg,rf) << "(const " << rtype << " &x) {\n";
    gen_regfield_get(_o, "x", "", rf, reg.offset(),dotTemplate());
    _o << "}\n\n"
       << rtype << " " << getRegFieldSetter(reg,rf) << "(" << rtype << " x,const " << rftype << " &v) {\n";
    gen_regfield_set(_o, "x", "", rf, reg.offset(), "v");
    _o << "return x;\n"
       <<"}\n\n";
  }

  void writeRegFieldHybridMaskSet(const RegBase &reg,const RegField &rf,
                                  bool is_hybrid,const gc_string &left,
                                  const gc_string &right,const gc_string &pktparm,
                                  const gc_string &pktoff_m)
  {
    if (is_hybrid) {
      if (rf.indexed()) {
        _o << pktparm << "._m.mask(" << genSliceArgs(left,right,_core,pktoff_m) << ");\n";
      } else {
        gen_regfield_mask(_o,pktparm,pktoff_m,reg,rf,"_m");
      }
    }
  }

  void writeRegFieldReadMaskSet(const RegBase &reg,const RegField &rf,
                                bool is_hybrid,const gc_string &left,
                                const gc_string &right,const gc_string &pktparm,
                                const gc_string &pktoff_m)
  {
    if (is_hybrid && track_reads()) {
      if (rf.indexed()) {
        _o << pktparm << "._r.mask(" << genSliceArgs(left,right,_core,pktoff_m) << ");\n";
      } else {
        gen_regfield_mask(_o,pktparm,pktoff_m,reg,rf,"_r");
      }
    }
  }

  void genRegFieldOuterWrite(const RegBase &reg,const RegField &rf,
                             bool is_transactional,bool is_hybrid,
                             bool commit_write,bool write_mask,
                             bool has_save,bool has_start_stop,
                             const gc_string &ctype,const gc_string &index,
                             const gc_string &pktarg1,const gc_string &rtype,
                             const gc_string &rftype,const gc_string &left,
                             const gc_string &right,const gc_string &get,
                             const gc_string &pktparm,const gc_string &pktoff_v,
                             const gc_string &pktoff_m,const gc_string &btype,
                             const gc_string &rval,const gc_string &rmask)
  {
    gc_string rname = reg.name();

    bool delayed = _core.delayed_writes().count(rname);

    gc_string commit = (write_mask ? "_commit" : ""); 

    gc_string index_arg, index1;
    if (!index.empty()) {
      index_arg = "unsigned " + index + ", ";
      index1 = index + ", ";
    }

    gc_string pktparm1;
    if (!pktparm.empty()) {
      pktparm1 = pktparm + ",";
    }
      
    gc_string findex_arg, findex;
    if (rf.indexed()) {
      findex_arg = "unsigned i,";
      findex = "i,";
    }

    if (delayed) {
      _o << "\n";
      _o << "void " << rname << "_" << rf.name() << "_write_delayed(" << pktarg1 << index_arg 
         << findex_arg << "const " << rftype << " &x,unsigned d) " << attr_used << "{\n";
      if (rf.write_ok()) {
        writeIgnoreCheck();
        // If the field is reserved or read-only, then we can't write to it.
        if (is_transactional) {
          // For a transactional ISS, just record the delay in the packet; the
          // commit queue will get its value during the call to the relevant
          // commit function.
          _o << rname << "_" << rf.name() << "_write(" << pktparm1 << index1 << findex << "x);\n"
             << pktparm << "._d = d;\n";
        } else {
          if (is_hybrid) {
            _o << pktparm << "._d = d;\n";
          }
          writeRegDelayBody(reg,ctype,"","_"+rf.name(),index1,findex,"","d","","",btype,rval,rmask);
        }
      }
      _o << "}\n\n";
    }

    _o << "void " << rname << "_" << rf.name() << "_write(" << pktarg1 << index_arg 
       << findex_arg << "const " << rftype << " &x) " << attr_used << "{\n";

    // If it's reserved or read-only, then we can't write to it.
    if (!rf.write_ok()) {
      _o << "}\n\n";
      return;
    }

    bool write_mask_set = true;
    writeIgnoreCheck();
    if (!is_transactional && delayed && !commit_write) {
      // If we just have a delayed write, and not a parallel commit-write,
      // then the normal write just calls the commit_write directly.
      _o << rname << "_" << rf.name() << "_commit_write(" << index1 << findex << "x);\n";
      writeRegFieldHybridMaskSet(reg,rf,is_hybrid,left,right,pktparm,pktoff_m);
      write_mask_set = false;
      _o << "}\n\n";
    }

    if (write_mask_set) {
      writeRegFieldHybridMaskSet(reg,rf,is_hybrid,left,right,pktparm,pktoff_m);
    }

    // Add to commit queue
    if ((commit_write || delayed) && !is_transactional) {
      if (commit_write) {
        writeRegDelayBody(reg,ctype,"","_"+rf.name(),index1,findex,"","0","","",btype,rval,rmask);
        _o << "}\n";
      }

      // Done for this case, because the non-transactional version will write
      // the identical code.
      if ((commit_write || delayed) && is_hybrid) {
        return;
      }

      _o << "void " << rname << "_" << rf.name() << "_commit_write("  
         << index_arg << findex_arg << "const " << rftype << " &x) " << attr_used << "{\n";
    }
    if (has_save) {
      writeRegSaveCall(reg,index);
    }
    if (has_hooks(reg) && !is_transactional) {
      // If it's a context resource or commit-write, then ignore the slice args-
      // just too complicated to handle all permutations.
      if (has_start_stop && !(_core.is_ctx_resource(reg) || commit_write || delayed)) {
        gc_string hooksliceargs = hookFieldSliceArgs(rf);
        _o << rname << commit << "_write_internal(" << index1 << "x" << hooksliceargs << ");\n";
      } else {
        // If we have hooks, then we have to do the read/modify/write operation.
        _o << rtype << "tmp = " << rname << "_read(" << index << ");\n";
        if (rf.indexed()) {
          _o << "tmp.set<" << rf.width() << ">(" << genSliceArgs(left,right,_core) << ",x);\n";
        } else {
          gen_regfield_set(_o,"tmp","",rf,reg.offset(),gc_string("x"));
        }
        _o << rname << commit << "_write(" << index1 << "tmp);\n";
      }
    } else {
      // Otherwise, we just modify the value directly.
      if (rf.indexed()) {
        _o << get << ".set<" << rf.width() << ">(" << genSliceArgs(left,right,_core,pktoff_v) << ",x);\n";
        if (is_transactional) {
          _o << pktparm << "._m.mask(" << genSliceArgs(left,right,_core,pktoff_m) << ");\n";
        } else {
          addRegBaseTraceCall(reg,rf);
          writeWatches(reg,_core.is_shared(reg));
        }
      } else {
        gen_regfield_set(_o,get,pktoff_v,rf,reg.offset(),gc_string("x"));
        if (is_transactional) {
          gen_regfield_mask(_o,pktparm,pktoff_v,reg,rf,"_m");
        } else {
          addRegBaseTraceCall(reg,rf);
          writeWatches(reg,_core.is_shared(reg));
        }
      }
    }
    _o << "};\n";
  }

  void writeRegSliceHybridMaskSet(const RegBase &reg,
                                  bool is_hybrid,const gc_string &pktparm,
                                  const gc_string &pktoff_m)
  {
    if (is_hybrid) {
      _o << pktparm << "._m.mask(" << pktoff_m << "(l % " << reg.width() << ")," << pktoff_m << "(r % " << reg.width() << "));\n";
    }
  }

  void genRegInHookSlice(const RegBase &reg,const gc_string &rtype,const gc_string &ctype,
                         const gc_string &index_arg,const gc_string &index1,const gc_string &hindex,
                         const gc_string &fargs)
  {
    gc_string rname = reg.name();

    // In-hook variant.
    gc_string set;
    if (reg.has_reserved_bits()) {
      set = "(x & ~" + rname + "()._reserved_mask(l,r))";
    } else {
      set = "x"; 
    }

    _o << "template <size_t Nb>\n"
       << "void " << rname << "_slice_set_inhook(" << index_arg << "unsigned l,unsigned r,const bits<Nb> &x) {\n";

    if (is_commit_write(reg,true)) {
        writeRegDelayBody(reg,ctype,fargs,"_slice",index1,"l,","r,","0","","","","","");
        _o << "}\n\n"
           << "template <size_t Nb>\n"
           << "void " << rname << "_slice_commit_write(" << index_arg << "unsigned l,unsigned r,const bits<Nb> &x) {\n";
    }

    _o << reg.name() << "()" << hindex << ".set_data(" << tta() << ").set(l,r," << set << ");\n";
    addRegBaseTraceCall(reg,"l,r");
    _o << "}\n"
       << "void " << rname <<  "_slice_set_inhook(" << index_arg << "unsigned l,unsigned r,uint64_t x) {\n"
       << rname << "_slice_set_inhook(" << index1 << "l,r," << rtype << "(x));\n"
       << "};\n";
  }

  // We also generate a slice-set function, which handles explicit writes to
  // slices of a register.  This follows the same paradigm of using a
  // read/modify/write operation if we have hooks, otherwise we simply modify
  // the value directly.
  void genRegSliceWrite(const RegBase &reg,bool is_transactional,
                        bool is_hybrid,bool commit_write,
                        bool write_mask,bool has_save,bool has_start_stop,
                        const gc_string &ctype,const gc_string &index,const gc_string &hindex,
                        const gc_string &pktarg1,const gc_string &rtype,
                        const gc_string &get,const gc_string &pktparm,
                        const gc_string &pktoff_v,const gc_string &pktoff_m,
                        const gc_string &btype)
  {
    if (!isRegSliceWritten(reg)) return;

    gc_string rname = reg.name();

    bool delayed = _core.delayed_writes().count(rname);

    gc_string commit = (write_mask ? "_commit" : "");

    gc_string index_arg, index1;
    if (!index.empty()) {
      index_arg = "unsigned " + index + ", ";
      index1 = index + ", ";
    }

    gc_string fargs = index_arg + "unsigned,unsigned,const bits<Nb>&";

    gc_string pktparm1;
    if (!pktparm.empty()) {
      pktparm1 = pktparm + ",";
    }

    bool concurrent = _core.concurrent_writes().count(rname);
    gc_string rval,rmask;
    if (concurrent) {
      rval  = ".set(l,r,x)";
      rmask = ".mask(l,r)";
    }

    if (delayed) {
      _o << "\n"
         << "template <size_t Nb>\n";
      _o << "void " << rname << "_slice_set_delayed(" << pktarg1 << index_arg << "unsigned l,unsigned r,const bits<Nb> &x,unsigned d) {\n";
      writeIgnoreCheck();
      if (is_transactional) {
        // For a transactional ISS, just record the delay in the packet; the
        // commit queue will get its value during the call to the relevant
        // commit function.
        _o << rname << "_slice_set(" << pktparm1  << index1 << "l,r,x);\n"
           << pktparm << "._d = d;\n";
      } else {
        writeRegDelayBody(reg,ctype,fargs,"_slice",index1,"l,","r,","d","","",btype,rval,rmask);
      }
      _o << "}\n\n"
         << "void " << rname << "_slice_set_delayed(" << pktarg1 << index_arg << "unsigned l,unsigned r,uint64_t x,unsigned d) " << attr_used << "{\n"
         << rname << "_slice_set_delayed(" << pktparm1 << index1 << "l,r," << rtype << "(x),d);\n"
         << "}\n";
    }

    _o << "template <size_t Nb>\n"
       << "void " << rname << "_slice_set_internal(" << pktarg1 << index_arg << "unsigned l,unsigned r,const bits<Nb> &x) {\n";

    writeIgnoreCheck();
    bool write_mask_set = true;
    if (!is_transactional && delayed && !commit_write) {
      // For non-parallel architectures with delays, a normal write just calls
      // the commit-write function to cause the update to happen immediately.
      _o << rname << "_slice_commit_write(" << index1 << "l,r,x);\n";
      writeRegSliceHybridMaskSet(reg,is_hybrid,pktparm,pktoff_m);
      write_mask_set = false;
      _o << "}\n\n";
    }
      
    // add to commit queue
    if ((commit_write || delayed) && !is_transactional) {
      if (commit_write) {
        writeRegDelayBody(reg,ctype,fargs,"_slice",index1,"l,","r,","0","","",btype,rval,rmask);
        _o << "}\n";
      }
      _o << "\n"
         << "template <size_t Nb>\n"
         << "void " << rname << "_slice_commit_write(" << pktarg1 << index_arg << "unsigned l,unsigned r,const bits<Nb> &x) {\n";
    }

    if (has_save) {
      writeRegSaveCall(reg,index);
    }

    bool need_inhook = false;
    if ((has_hooks(reg) || extern_regs() || reg.has_readonly_bits()) && !is_transactional) {
      need_inhook = reg.has_hook_action();
      // If we have hooks or read-only masks, then we have to do the read/modify/write operation.
      // If the user's hook takes indexes, however, then we can just call that
      // function.
      if (need_inhook && has_start_stop && !_core.is_ctx_resource(reg)) {
        _o << rname << commit << "_write_internal(" << index1 << "x,l,r);\n";
      } else {
        _o << rtype << " tmp = " << rname << "_read(" << index << ");\n"
           << "tmp.set(l,r,x);\n"
           << rname << commit << "_write(" << index1 << "tmp);\n";
      }
    } else {
      if (is_transactional) {
        // For transactional, in order to make sure that the indices are
        // correct, we do a mod operation on the indices.
        _o << pktparm << "._m.mask(" << pktoff_v << "(l % " << reg.width() << ")," << pktoff_v << "(r % " << reg.width() << "));\n"
           << get << ".set(" << pktoff_v << "(l % " << reg.width() << ")," << pktoff_v << "(r % " << reg.width() << "),x);\n";
      } else {
        // Otherwise, we just modify the value directly.  If we have a reserved
        // mask then we need to do the masking.  We could just do this as a
        // read-modify-write operation, but this should be a tad faster.
        _o << get << ".set(l,r,";
        if (reg.has_reserved_bits()) {
          _o << "(x & ~" + rname + "()._reserved_mask(l,r))";
        } else {
          _o << "x";
        }
        _o << ");\n";
      }
      if (!is_transactional) {
        if (commit_write || delayed) {
          addRegBaseTraceCall(reg,"l,r");
        }
        writeWatches(reg,_core.is_shared(reg));
      }
    }
    if (!write_mask_set) {
      writeRegSliceHybridMaskSet(reg,is_hybrid,pktparm,pktoff_m);
    }

    _o << "};\n";

    // Various other forms to handle different combinations of intbv/int arguments.
    _o << "template <size_t Nb>\n"
       << "void " << rname << "_slice_set(" << pktarg1 << index_arg << "unsigned l,unsigned r,const bits<Nb> &x) {\n"
       << rname << "_slice_set_internal(" << pktparm1 << index1 << "l,r,x);\n";
    if (!is_transactional && !need_inhook) {
      addRegBaseTraceCall(reg,"l,r");
    }
    _o << "}\n\n"

       << "void " << rname <<  "_slice_set(" << pktarg1 << index_arg << "unsigned l,unsigned r,uint64_t x) {\n"
       << rname << "_slice_set(" << pktparm1 << index1 << "l,r," << rtype << "(x));\n"
       << "};\n";

    // Create in-hook variants if needed.  Don't do this for transactional or
    // hybrid, since this will cause a duplication of the same functions.
    if (need_inhook && !(is_transactional || is_hybrid) && !reg.pseudo()) {
      genRegInHookSlice(reg,rtype,ctype,index_arg,index1,hindex,fargs);
    }
  }

  // Generates a mask-set function, used by a transactional ISS to update a
  // portion of a register on a partial write.
  void genRegMaskSet(const RegBase &reg,const gc_string &ctype,const gc_string &index,
                     const gc_string &pktarg,const gc_string &pktparm,const gc_string &commit)
  {
    gc_string rname = reg.name();

    bool delayed = _core.delayed_writes().count(reg.name());

    gc_string index_arg;
    gc_string index1;
    if (!index.empty()) {
      index_arg = "unsigned " + index + ",";
      index1 = index + ",";
    }

    _o << "void " << rname << "_mask_set(" << index_arg << pktarg << ") " << attr_used << "{\n"
       << "bits<" << reg.width() << "> tmp = " << rname << "_read(" << index << ");\n"
       << "tmp = (tmp & ~" << pktparm << "._m) | (" << pktparm << "._v & " << pktparm <<"._m);\n"
       << rname << commit <<"_write(" << index1 << "tmp);\n"
       <<"}\n";
    // If delayed, then we need another version for handling the delayed
    // write- both a setup function to add to the commit queue and an update
    // functor to commit the value.
    if (delayed) {
      _o << "void " << rname << "_mask_set(" << index_arg << pktarg << ",unsigned d) {\n"
         << "bits<" << reg.width() << "> x = " << pktparm << "._v;\n"
         << "bits<" << reg.width() << "> m = " << pktparm << "._m;\n";
      writeRegDelayBody(reg,ctype,"","_mask_set",index1,"m,","","d");
      _o <<"}\n\n"
         << "void " << rname << "_mask_set_commit_write(" << index_arg << "const bits<" << reg.width() << "> &v,"
         << "const bits<" << reg.width() << "> &m) {\n"
         << "bits<" << reg.width() << "> tmp = " << rname << "_read(" << index << ");\n"
         << "tmp = (tmp & ~m) | (v & m);\n"
         << rname << commit <<"_write(" << index1 << "tmp);\n"
         <<"}\n";
    }
  }

  gc_string hookFieldSliceArgs(const RegField &rf)
  {
    gc_ostringstream ss;
    if (rf.indexed()) {
      ss << ",i*" << rf.width() << ",(i*" << rf.width() << ")+" << rf.width() << "-1";
    } else {
      // Since we can't fully support non-contiguous fields, just return the low
      // and the high range.
      ss << "," << rf.bits().lowbit() << "," << rf.bits().highbit();
    }
    return ss.str();
  }

  void genRegFieldIndexedInHookWrite(const Reg &reg,const RegField &rf,const gc_string &rtype,
                                     const gc_string &rftype,Ptree *cp,const gc_string &left,const gc_string &right)
  {
    gc_string rname = reg.name();
    if (is_commit_write(reg,true)) {
      // The in-hook write is delayed b/c this is a register with an immediate write-hook.
      gc_string findex_arg = "unsigned i,";
      gc_string findex = "i,";
      writeRegDelayBody(reg,"CommitRegIndexedFieldWrite","","_"+rf.name(),"",findex,"","0","","","","","");

      _o << "}\n\n"
         << "void " << rname << "_" << rf.name() << "_commit_write(" << findex_arg << "const " << rftype << " &x) " << attr_used << "{\n";
    }

    // Standard in-hook write for fields.
    if (extern_regs()) {
      _o << rtype << "tmp = " << getExternalRegRead(reg) << "();\n"
         << "tmp.set<" << rf.width() << ">(i*" << rf.width() << ",(i*" << rf.width() << ")+" << rf.width() << "-1,x);\n"
         << getExternalRegWrite(reg) << "(tmp);\n";
    } else {
      _o << rname << "(" << ctx_index(cp) << ").set_data(" << tta() << ").set<" << rf.width() << ">(" << genSliceArgs(left,right,_core) << ",x);\n";
    }
    addRegBaseTraceCall(reg,rf);
  }

  void genRegFieldInHookWrite(const Reg &reg,const RegField &rf,const gc_string &rtype,const gc_string &rftype,Ptree *cp)
  {
    gc_string rname = reg.name();
    if (is_commit_write(reg,true)) {
      // The in-hook write is delayed b/c this is a register with an immediate write-hook.
      writeRegDelayBody(reg,"CommitRegFieldWrite","","_"+rf.name(),"","","","0","","","","","");

      _o << "}\n\n"
         << "void " << rname << "_" << rf.name() << "_commit_write(const " << rftype << " &x) " << attr_used << "{\n";
    }

    // Standard in-hook write for fields.
    if (extern_regs()) {
      _o << rtype << " tmp = " << getExternalRegRead(reg) << "();\n";		
      gen_regfield_set(_o,"tmp","",rf,reg.offset(),gc_string("x"));
      _o << getExternalRegWrite(reg) << "(tmp);\n";
    } else {
      gen_regfield_set(_o,rname + "(" + ptos(ctx_index(cp)) + ").set_data(" + tta() + ")","",rf,reg.offset(),gc_string("x"));
    }
    addRegBaseTraceCall(reg,rf);
  }

  // Generates members storing field information.
  // The general idea with accessing fields is that we follow a read/modify/write paradigm if we
  // have read or write hooks, so that those side-effects are maintained.  If not, then we can just
  // do a simple access to the underlying element.
  void genRegFields(const Reg &reg,bool is_transactional,bool is_hybrid,
                    bool has_write_hook,bool has_save,bool concurrent,bool has_start_stop)
  {
    gc_string rname = reg.name();
    gc_string rtype = "bits<"+uitogs(reg.width())+">";

    gc_string pktarg,pktarg1;
    gc_string pktparg,pktparg1;
    if (is_transactional || is_hybrid) {
      MkGStr(pktarg,"bits_pair<" << _packet_width << "> &slot");
      pktarg1 = pktarg + ",";
    }
    
    gc_string pktparm;
    if (is_transactional || is_hybrid) {
      pktparm = "slot";
    }

    // For hybrid ISSs, we always deal directly with the register, so this is
    // only applicable for transactional ISSs.
    gc_string get, getm, ctx_get;
    if (is_transactional) {
      get = getm = "slot._v";
    } else if (log_reads()) {
      get = getm = rname + "_read_notrace()";
    } else if (time_tagged()) {
      get = rname + "_read()";
      getm = rname + "_read_modify()";
    } else {
      get = getm = rname + "_read()";
    }

    // Packet offset
    gc_string pktoff_v, pktoff_m;
    if (is_transactional || is_hybrid) {
      if (BitPair::is_little_endian()) {
        pktoff_m = "";
      } else {
        MkGStr(pktoff_m,( _packet_width - reg.width() ) << " + ");
      }
      if (!is_hybrid) {
        pktoff_v = pktoff_m;
      }
    }

    auto ctx = _core.is_ctx_resource(reg);
    auto cp = getCtxParm(ctx);
      
    bool commit_write = is_commit_write(reg);
    bool write_mask = (commit_write && !reg.pseudo() && !_core.is_shared(reg));
    gc_string commit = (write_mask ? "_commit" : ""); 
    gc_string rmask,rval;
    gc_string btype = (concurrent ? "RegBundleElem<"+uitogs(reg.width())+">" : "");

    // If we have fields, then they become member variables if non-indexed
    // or member functions if they are indexed.
    if (reg.has_fields()) {

      ForEach(reg.fields(),i) {
        const RegField &rf = *(i->second);

        if (!isFieldUsed(reg,rf)) continue;

        gc_string rftype = "bits<"+uitogs(rf.width())+">";

        if (rf.indexed()) {
          gc_string left,right;
          gen_indexed_regfield_exprs(reg,rf,left,right);
	    
          // Create the helper get/set functions.
          if (isFieldHelperUsed(reg,rf)) {
            if (!is_transactional && !is_hybrid) {
              writeIndexedFieldHelpers(reg,rf,rtype,rftype,left,right);
            }
          }

          if (isFieldRead(reg,rf)) {
            // Write the read/write functions used for normal accesses.
            _o << rftype << " " << rname << "_" << rf.name() << "_read(" << pktarg1 << "unsigned i) " << attr_used << "{\n";
            addRegReadCallback(reg);
            writeRegFieldReadMaskSet(reg,rf,is_hybrid,left,right,pktparm,pktoff_m);
            addRegReadTraceCall(reg, rf);
            _o << "return " << get << "." << dotTemplate() << " get<" << rf.width() << ">(" << genSliceArgs(left,right,_core,pktoff_v) << ");\n"
               << "};\n";
          }

          if (concurrent) {
            // TBD 
            MkGStr(rval,".set<" << rf.width() << ">(" << genSliceArgs(left,right,_core) << ",x)");
            MkGStr(rmask,".mask(" << genSliceArgs(left,right,_core) << ")"); 
          }

          if (isFieldWritten(reg,rf)) {
            genRegFieldOuterWrite(reg,rf,is_transactional,is_hybrid,commit_write,write_mask,has_save,has_start_stop,
                                  "CommitRegIndexedFieldWrite","",pktarg1,rtype,rftype,left,right,getm,
                                  pktparm,pktoff_v,pktoff_m,btype,rval,rmask);
          }

          // Handled by the non-transactional call.
          if (!is_transactional && !is_hybrid && !reg.pseudo()) {
            // For accessing register fields in specific thread contexts.
            if (ctx) {
              has_write_hook = true;
              if (isFieldRead(reg,rf)) {
                _o << rftype << " " << rname << "_" << rf.name() << "_ctx_read(unsigned ctx_index,unsigned i) {\n"
                   << "return " << rname << "_read_internal(ctx_index)." << dotTemplate() << " get<" << rf.width() << ">(" <<genSliceArgs(left,right,_core) << ");\n"
                   << "};\n";
              }

              if (isFieldWritten(reg,rf)) {
                _o << "void " << rname << "_" << rf.name() << "_ctx_write(unsigned ctx_index,unsigned i,const " << rftype << " &x) {\n";
                if ((extern_regs() || has_hooks(reg)) && !is_transactional) {
                  // If we have hooks, then we have to do the read/modify/write operation.
                  if (has_start_stop) {
                    _o << rname << "_write_internal(ctx_index,x" << hookFieldSliceArgs(rf) << ");\n";
                  } else {
                    _o << rtype << "tmp = " << rname << "_read_internal(ctx_index);\n"
                       << "tmp.set<" << rf.width() << ">(i*" << rf.width() << ",(i*" << rf.width() << ")+" << rf.width() << "-1,x);\n"
                       << rname << "_write_internal(ctx_index,tmp);\n";
                  }
                } else {
                  // Otherwise, we just modify the value directly.
                  _o << rname << "_" << rf.name() << "_write_inhook(ctx_index,i,x);\n";
                }
                addRegCtxTraceCall(reg,*ctx,ctx_index(cp));
                _o << "}\n";
              }
            }
            if (has_write_hook && isFieldWritten(reg,rf)) {
              _o << "void " << rname << "_" << rf.name() << "_write_inhook(" << cp << "unsigned i,const " << rftype << " &x) " << attr_used << "{\n";
              genRegFieldIndexedInHookWrite(reg,rf,rtype,rftype,cp,left,right);
              _o << "};\n";
            }
          }
        } else {
          // Non-indexed field- no arguments are required.

          // Create the helper get/set functions.
          if (isFieldHelperUsed(reg,rf) || concurrent) {
            if (!is_transactional && !is_hybrid) {
              writeFieldHelpers(reg,rf,rtype,rftype);
            }
          }

          // Create the read/write functions.
          if (isFieldRead(reg,rf)) {
            _o << rftype << " " << rname << "_" << rf.name() << "_read(" << pktarg << ") " << attr_used << "{\n";
            addRegReadCallback(reg);
            writeRegFieldReadMaskSet(reg,rf,is_hybrid,"","",pktparm,pktoff_m);
            addRegReadTraceCall(reg, rf);
            gen_regfield_get(_o, get, pktoff_v, rf, reg.offset(),dotTemplate());
            _o << "};\n\n";
          }

          if (concurrent) {
            MkGStr(rmask, " = \"" << rf.write_mask().str() << "\"");
            MkGStr(rval," = setfield_" << rname << "_" << rf.name() << "(value,x)");
          }

          if (isFieldWritten(reg,rf)) {
            genRegFieldOuterWrite(reg,rf,is_transactional,is_hybrid,commit_write,write_mask,has_save,has_start_stop,
                                  "CommitRegFieldWrite","",pktarg1,rtype,rftype,"","",getm,pktparm,pktoff_v,pktoff_m,btype,rval,rmask);
          }

          if (!is_transactional && !is_hybrid && !reg.pseudo()) {
            if (ctx) {
              has_write_hook = true;
              // For accessing register fields in specific thread contexts.
              if (isFieldRead(reg,rf)) {
                _o << rftype << " " << rname << "_" << rf.name() << "_ctx_read(unsigned ctx_index) {\n";
                gen_regfield_get(_o, rname + "_read_internal(" + ptos(ctx_index(cp)) + ")", "", rf, reg.offset(),dotTemplate());
                _o << "};\n";
              }

              if (isFieldWritten(reg,rf)) {
                _o << "void " << rname << "_" << rf.name() << "_ctx_write(unsigned ctx_index,const " << rftype << " &x) {\n";
                if ((extern_regs() || has_hooks(reg)) && !is_transactional) {
                  if (has_start_stop) {
                    _o << rname << "_write_internal(ctx_index,x" << hookFieldSliceArgs(rf) << ");\n";
                  } else {
                    _o << rtype << " tmp = " << rname << "_read_internal(ctx_index);\n";		
                    gen_regfield_set(_o,"tmp","",rf,reg.offset(),gc_string("x"));
                    _o << rname << "_write_internal(ctx_index,tmp);\n";
                  }
                } else {
                  _o << rname << "_" << rf.name() << "_write_inhook(ctx_index,x);\n";
                }
                addRegCtxTraceCall(reg,*ctx,ctx_index(cp));
                _o << "}\n";
              }
            }
            if (has_write_hook && isFieldWritten(reg,rf)) {
              _o << "void " << rname << "_" << rf.name() << "_write_inhook(" << cp << "const " << rftype << " &x) " << attr_used << "{\n";
              genRegFieldInHookWrite(reg,rf,rtype,rftype,cp);
              _o << "};\n";
            }
          }
        }
      }
    }

    // For a transactional ISS, we need a mask-write function which can update
    // a portion of a register.
    if (is_transactional && needRegPartialAccess(reg)) {
      genRegMaskSet(reg,"CommitRegMaskSet","",pktarg,pktparm,commit);
    }

    // Then generate a function for updating a portion of a register (a slice).
    genRegSliceWrite(reg,is_transactional,is_hybrid,commit_write,write_mask,has_save,has_start_stop,
                     "CommitRegSliceSet","","",pktarg1,rtype,getm,pktparm,pktoff_v,pktoff_m,btype);

    // This is a simple slice-write function for non-pseudo registers which allows access to a particular
    // thread context.  This is only created if we are a context resource.
    if (!reg.pseudo() && ctx && !is_transactional && !is_hybrid && isRegSliceWritten(reg)) {
      _o << "template <size_t Nb>\n"
         << "void " << rname << "_ctx_slice_set(" << cp << "unsigned l,unsigned r,const bits<Nb> &x) {\n";
      if (has_hooks(reg) && !is_transactional) {
        _o << rtype << " tmp = " << rname << "_read_internal(ctx_index);\n"
           << "tmp.set(l,r,x);\n"
           << rname << "_write_internal(ctx_index,tmp);\n";
      } else {
        _o << rname << "(" << ctx_index(cp) << ").set_data().set(l,r,x);\n";
      }
      addRegCtxTraceCall(reg,*ctx,ctx_index(cp));
      _o << "}\n"
         << "void " << rname << "_ctx_slice_set(" << cp << "unsigned l,unsigned r,uint64_t x) {\n"
         << rname << "_ctx_slice_set(" << ctx_arg(cp) << "l,r," << rtype << "(x));\n"
         << "}\n";
    }

  }

  void genRegUpdateFunc(const RegBase &reg,bool has_index)
  {
    gc_string i_arg,index;
    if (has_index) {
      i_arg = "unsigned index, ";
      index = "(index)";
    }
    _o << dec
       << "void " << getUpdateRegFunc(reg.name()) << "(" << i_arg << "const ValueBundle<" << reg.width() <<"> &v) {\n"
       << "for (unsigned i = 0; i != v.size(); ++i) {\n"
       << reg.name() << "()" << index << ".set_data(" << tta() << ") = (v[i].value() & v[i].mask()) | (" << reg.name() << "()" << index << ".get_data() & ~v[i].mask());\n"
       << "}\n";
    if (has_index) {
      addRegFileTraceCall(dynamic_cast<const RegFile&>(reg),0);
    } else {
      addRegTraceCall(reg);
    }
    _o << "}\n\n";
  }

  // For each register, we make it a class.  We then create accessor methods
  // in the core for dealing with the register.  The InstrWalker class is responsible
  // for transformin code to use these functions, e.g. XER = 5 becomes XER_write(5).
  void genRegObjects(const Reg &reg)
  {
    gc_string name  = reg.name();
    gc_string type  = getRegTypeName(reg);
    gc_string rtype = getRegDataType(reg);
    gc_string rdata = getRegDataObj(reg);

    // For any of our code, we want to ignore the name of our
    // own register when doing code re-writing.
    _walker.set_cur_reg(name,_core.is_ctx_resource(reg));

    _o << "// Register " << name << "\n";

    // Generate read/write interface.  For this logic, if we're writing
    // functions for the NIA, we do want to track take branches, since this is
    // the version used by instruction action code.
    genRegTraceLogic(reg,true);
    bool has_write_hook = false, has_save = false, concurrent = false, has_start_stop = false;
    genRegWriteHook(has_write_hook,has_save,concurrent,has_start_stop,reg,rtype,false);

    // Generate the external write hook, if relevant.
    genRegExtWriteHook(reg);

    // Concurrent write hooks (ones which take a ValueBundle argument), may use
    // the updateReg helper function to update the register all at once.
    if (concurrent && has_write_hook && !reg.pseudo()) {
      genRegUpdateFunc(reg,false);
    }

    // Special case- for the NIA, generate a special function which doesn't
    // track branches.  This needs to be fast, so we have it always inline.
    if (&reg == _core.nia()) {
      genRegTraceLogic(reg,false,getNiaSfx());
      bool has_write_hook = false, has_save = false;
      genRegWriteHook(has_write_hook,has_save,concurrent,has_start_stop,reg,rtype,true,getNiaSfx());
    }

    genRegReadHook(reg,rtype,has_write_hook);
    genRegReset(reg,rtype);
      
    if (has_save) {
      genRegSaveLogic(reg,rtype,"CommitRegWrite","");
    }

    // For fields and slice operators, if we're transactional, we need both the
    // original and instruction packet-aware versions, since there are some
    // functions which don't use the packet and some that do.
    genRegFields(reg,false,false,has_write_hook,has_save,concurrent,has_start_stop);
    if (transactional_iss() || hybrid_iss()) {
      genRegFields(reg,transactional_iss(),hybrid_iss(),has_write_hook,has_save,concurrent,has_start_stop);
    }

    _o << "\n";

    _walker.reset();
  }

  // Write out all registers.
  void generateRegs()
  {
    const RegHash &regs = _core.regs();

    _o << "//\n// Register Handling Code.\n//\n\n";
    ForEach(regs,i) {
      const Reg &reg = *(i->second);
      genRegObjects(reg);
    }
    _o << "\n";
  }

  void addRegFileCtxTraceCall(const RegFile &rf,const Context &ctx,Ptree *cp)
  {
    if (_config.trace_mode() && hasStorage(rf)) {
      _o << "{\n"
         << "bool change = " << getCtxChangeFunc(ctx) << "(" << ctx_index(cp) << ");\n";
      addRegFileTraceCall(rf,ctx_arg(cp));
      _o << getCtxChangeBackFunc(ctx) << "(change);\n"
         << "}\n";
    }
  }

  void genRegFileTraceLogic(const RegFile &regfile)
  {
    genRegFileTraceLogicInternal(regfile,false);
    if (log_partial_masks()) {
      genRegFileTraceLogicInternal(regfile,true);
    }
  }
    
  // Used for complex register files.
  void genRegFileTraceLogicInternal(const RegFile &regfile,bool mask)
  {
    if (!isRegUsed(regfile)) return;

    if (!regfile.pseudo() && !regfile.sparse()) {

      gc_string mask_sfx;
      gc_string mask_parm;
      gc_string mask_arg;
      if (mask) {
        mask_arg = ",mask." + intbvAccessor();
        mask_sfx = "_mask";
        MkGStr(mask_parm,",const bits<" << regfile.width() << "> &mask");
      }
      
      _o << "void " << regfile.name() << "_trace(unsigned index" << mask_parm << ") " << attr_used << "{\n";
      gc_string data_accessor;
      if (extern_regs()) {
        MkGStr(data_accessor,getExternalRegRead(regfile) << "(index)." << intbvAccessor());
      } else {
        MkGStr(data_accessor,regfile.name() << "().operator()(index).get_data()." << intbvAccessor());
      }
      genRegFileLogCall(regfile,false,mask_sfx,data_accessor+mask_arg);
      _o << "}\n\n";
      
      if (has_reg_callback(regfile.name())) {
        _o << "void " << regfile.name() << "_callback(unsigned index)" << attr_used << "\n"
           << "{\n";
        int index = 0;
        // Yes, this is an associative data structure, but our IDs are indices
        // into an array, so we need to calculate that by counting
        // sequentially through the list.
        ForEach(reg_callbacks(),i) {
          if (*i == regfile.name()) {
            _o << "(*" << getCallbackName(false) << "[" << index << "])(index," << data_accessor << ");\n";
            break;
          }
          index++;
        }
        _o << "};\n";
      }

      // Add on a context-specific trace function if we're a context register.
      // We're not handling this with masks yet.
      if (!mask) {
        auto ctx = _core.is_ctx_resource(regfile);
        if (ctx) {
          _o << "void " << regfile.name() << "_trace(unsigned ctx_index,unsigned index) {\n";
          genRegFileLogCall(regfile,false,mask_sfx,regfile.name() + "(ctx_index).operator()(index).get_data()." + intbvAccessor() + mask_arg);
          _o << "}\n\n";
        }
      }

      if (log_reads()) {
        _o << "void " << regfile.name() << "_read_trace(unsigned index" << mask_parm << ") {\n";
        gc_string data_accessor;
        if (extern_regs()) {
          MkGStr(data_accessor,getExternalRegRead(regfile) << "(index)." << intbvAccessor());
        } else {
          MkGStr(data_accessor,regfile.name() << "().operator()(index).get_data()." << intbvAccessor());
        }
        genRegDtSrcCheck(regfile,"index");
        genRegFileLogCall(regfile,true,mask_sfx,data_accessor+mask_arg);
        _o << "};\n";
      }
    }
  }
    
  void writeRegFileOuterWriteBody(const RegFile &regfile,const Context *ctx)
  {
    gc_string name = regfile.name();

    gc_string ctx_ctrl = (ctx) ? getContextControl(*ctx)+"," : "";
    _o << name << "_write_internal_traced(" << ctx_ctrl << "index,x);\n";
  }

  void genRegFileReadRangeCheck(const RegFile &regfile)
  {
    if (!is_power_2(regfile.size())) {
      // We only need to do a range-check if we don't have a power-of-2 size.
      _o << "if (index >= " << regfile.size() << ") {\n";
      if (regfile.invalid_entry_read()) {
        // We have an invalid-handler to call.
        _o << getInvalidEntryReadName(regfile) << "(index);\n";
      } else if (_core.invalid_entry_read()) {
        // Global handler, if applicable.
        _o << getGlobalInvalidEntryReadName() << "(" << regfile.id() << ",index);\n";
      } else {
        // Default action- just exit the simulation.
        _o << "std::cerr << \"Attempt to access " << regfile.name() << "(\" << index << \"), which is undefined.\\n\\n\";\n"
           << "abort();\n";
      }
      _o << "}\n";
    }
  }

  void genRegFileWriteRangeCheck(const RegFile &regfile)
  {
    if (!is_power_2(regfile.size())) {
      // We only need to do a range-check if we don't have a power-of-2 size.
      _o << "if (index >= " << regfile.size() << ") {\n";
      if (regfile.invalid_entry_write()) {
        // We have an invalid-handler to call.
        _o << getInvalidEntryWriteName(regfile) << "(index,x);\n";
      } else if (_core.invalid_entry_write()) {
        // Global handler, if applicable.
        _o << getGlobalInvalidEntryWriteName() << "(" << regfile.id() << ",index,x);\n";
      } else {
        // Default action- just exit the simulation.
        _o << "std::cerr << \"Attempt to access " << regfile.name() << "(\" << index << \"), which is undefined.\\n\\n\";\n"
           << "abort();\n";
      }
      _o << "}\n";
    }
  }

  void genRegFileOuterWrite(const RegFile &regfile,bool commit_write,
                            const Context *ctx,const gc_string &rtype)
  {
    gc_string name = regfile.name();

    bool delayed = _core.delayed_writes().count(name);

    bool concurrent = _core.concurrent_writes().count(name);
    gc_string btype,rvalue,rmask;
    if (concurrent) {
      MkGStr(btype,"RegfileBundleElem<" << regfile.width() << '>');
      rvalue = " = x";
      rmask = ".set()";
    }

    if (delayed) {
      _o << "\n";
      _o << "void " << name << "_write_delayed(unsigned index,const " << rtype << " &x,unsigned d) " << attr_used << "{\n";
      writeIgnoreCheck();
      writeRegDelayBody(regfile,"CommitRegfileWrite","","","index,","","","d","","",btype,rvalue,rmask);
      _o << "}\n\n";
    }

    _o << "void " << name << "_write(unsigned index,const " << rtype << " &x) {\n";
    writeIgnoreCheck();
    if(commit_write) {
      writeRegDelayBody(regfile,"CommitRegfileWrite","","","index,","","","0","","",btype,rvalue,rmask);
      _o << "}\n";
      _o << "void " << name << "_commit_write(unsigned index,const " << rtype << " &x) " << attr_used << "{\n";
    }

    writeRegFileOuterWriteBody(regfile,ctx);

    _o << "};\n";

    writeRegReadCallback(regfile,ctx,"index");

    // If we have a delayed write, then we also need a commit_write, in addition to a normal write.
    if (delayed && !commit_write) {
      _o << "void " << name << "_commit_write(unsigned index,const " << rtype << " &x) " << attr_used << "{\n";        
      writeRegFileOuterWriteBody(regfile,ctx);
      _o << "};\n";
    }
  }

  void genRegFileInHookWrite(const RegFile &regfile,const gc_string &name,Ptree *cp,const gc_string &rtype)
  {
    _o << "void " << name << "_write_inhook(" << cp << "unsigned index,const " << rtype << " &x) " << attr_used << "{\n";
    if (is_commit_write(regfile,true)) {
      writeRegDelayBody(regfile,"CommitRegfileWrite","","","index,","","","0","","","","","");
      _o << "}\n\n"
         << "void " << name << "_commit_write(unsigned index,const " << rtype << " &x) {\n";
    }

    gc_string expr = genWriteExpr(regfile,name,true);
    auto ctx = _core.is_ctx_resource(regfile);

    if (extern_regs()) {
      _o << getExternalRegWrite(regfile) << "(index,x);\n";
    } else {
      genRegFileWriteRangeCheck(regfile);
      _o << name << "(" << ctx_index(cp) << ")(index).set_data(" << tta() << ") = " << expr << ";\n";
    }
    _o << "}\n";
    if (ctx) {
      _o << "void " << name << "_ctx_write(" << cp << "unsigned index,const " << rtype << " &x) {\n"
         << name << "_write_internal(" << ctx_index(cp) << ",index,x);\n";
      addRegFileCtxTraceCall(regfile,*ctx,cp);
      _o << "}\n";
    }
  }

  void genRegFileWriteHook(bool &has_write_hook,bool &concurrent,bool &has_save,bool &has_start_stop,const RegFile &regfile,const gc_string &rtype)
  {
    if (!needRegWrite(regfile)) return;

    // Insert a default writehook if one doesn't exist, otherwise use
    // the actual writehook.  If sparse, then we generate the lookup switch statement.
    bool handled = false;
    bool needs_hook_write = (regfile.resettype() == RegBase::Action);
    bool commits = (commit_writes() && !regfile.is_serial());
    gc_string name = regfile.name();
    gc_string type = getRegTypeName(regfile);
    auto ctx = _core.is_ctx_resource(regfile);
    auto cp = getCtxParm(ctx);

    if (regfile.invalid_entry_write()) {
      genFunc("void " + getInvalidEntryWriteName(regfile),regfile.env(),regfile.invalid_entry_write()._func);
    }

    concurrent = _core.concurrent_writes().count(name);
    bool write_alias = false;
    bool immediate = false;
    if (const RegFileHook *hook = regfile.writehook()) {
      if (hook->immediate()) {
        // If the hook is marked as an immediate write, then we generate the
        // delayed-commits in the in-hook functions.
        immediate = true;
      }
      if (Ptree *hookaction = hook->action()) {
        // Action code:  Process and write it out.  The code itself it put within a
        // hook function and the write function calls this hook function.  This allows us
        // to put tracing logic, if applicable, in the write function and have it always called,
        // even if the hook function has return statements through-out it.
        _o << "void " << name << "_write_hook" << genRegHookArgs(ctx,hookaction,&regfile) << " " << attr_used << "{\n";
        writeLineDirective(hook->env(),hookaction);
        _o << funcBody(hook->env(),hookaction) << "\n";
        writeCurLocation();
        has_start_stop = hook->has_start_stop();
        _o << "}\n";
        _o << "void " << name << "_write_internal(" << cp << "unsigned index,const " << rtype << " &x" << hookSliceArgs(regfile) << ") " << attr_used << "{\n";
        genConcurHookReturn(regfile,concurrent);
        writeRegSaveCall(regfile,"index");
        has_save = true;
        gc_string hook_arg = concurrent ? (getRegBundleName(regfile) + "[0]") : "x";
        _o << name << "_write_hook(" << ctx_arg(cp) << "index," << hook_arg << hookSliceParms(regfile) << ");\n";
        genBundleClear(regfile,concurrent,_core);
        handled = true;
        needs_hook_write = true;
        has_write_hook = true;
      } else if (hook->ignore()) {
        // Ignore:  Silently ignore the write.
        _o << "void " << name << "_write_internal(" << cp << "unsigned index,const &" << rtype << ") " << attr_used << "{\n";
        handled = true;
      } else if (const RegBase *alias = hook->alias()()) {
	
        // Alias:  Modify the target of the alias instead of this register.
        _o << "void " << name << "_write_internal(" << cp << "unsigned index,";
        if (regfile.has_reserved_bits() || regfile.has_readonly_bits()) {
          _o << rtype << " x) {\n";
        } else {
          _o << "const " << rtype << " &x) " << attr_used << "{\n";
        }
        writeAliasMaskingExpr(regfile,hook->alias(),rtype);
        _o << alias->name();
        if (commits) {
          // use the target register commit
          _o << "_write(";
        } else {
          _o << "_write_internal_traced(";
        }
        _o << ctx_arg(cp) << type << "::getWriteIndex(index),x);\n";
        handled = true;
        write_alias = true;
      }
    }

    gc_string expr = genWriteExpr(regfile,name,true);

    // Default:  Set the register to the specified value.
    if (!handled) {
      if (regfile.sparse()) {
        // Sparse:  Generate the switch statement to select the proper register.
        // No tracing done here- the individual registers will log the access.
        _o << "void " << name << "_write_internal(unsigned index,const " << rtype << " &x) " << attr_used << "{\n";
        genSparseSwitch(regfile,ssWrite,&_core);
      } else {
        // Normal, default write case:  Simply modify the register.
        _o << "void " << name << "_write_internal(" << cp << "unsigned index,const " << rtype << " &x) " << attr_used << "{\n";
        writeRegSaveCall(regfile,"index");
        has_save = true;
        if (extern_regs()) {
          _o << getExternalRegWrite(regfile) << "(index,x);\n";
        } else {
          genRegFileWriteRangeCheck(regfile);
          _o << name << "(" << ctx_index(cp) << ")(index).set_data(" << tta() << ") = " << expr << ";\n";
        }
      }
    }
    writeWatches(regfile,_core.is_shared(regfile));
    _o << "};\n";

    if (!regfile.pseudo()) {
      // This is a simple read function for non-pseudo registers which allows access to a particular
      // thread context.  This is only created if we are a context resource.
      if (ctx || needs_hook_write) {
        genRegFileInHookWrite(regfile,name,cp,rtype);
      }
    }

    // This does a write and adds tracing.  May be called by aliases in order
    // to execute an aliased action.
    _o << "void " << name << "_write_internal_traced(" << cp << "unsigned index,const " << rtype << " &x) " << attr_used << "{\n"
       << name << "_write_internal(" << ctx_arg(cp) << "index,x);\n";
    if (!regfile.pseudo()) {
      addRegFileTraceCall(regfile);
    }
    _o << "}\n";

    // Wrapper and version using intbv for the index.  For consistancy with
    // registers, a _write_internal function exists with an equivalent _write
    // function.  We only do commit-writes for simple register files.  If we're
    // dealing with an alias or a pseudo-file, then the commits will be handled
    // by the eventual targets.
    bool commit_write = commit_writes() && 
      (!regfile.has_writehook() || regfile.writehook()->action()) &&
      !regfile.is_serial() && 
      !write_alias && 
      !regfile.sparse() && 
      !immediate;

    genRegFileOuterWrite(regfile,commit_write,ctx,rtype);
  }

  void genRegFileReadHook(const RegFile &regfile,const gc_string &base_rtype,bool needs_write_hook)
  {
    if (!needRegRead(regfile)) return;

    // Insert a default readhook if one doesn't exist, otherwise use
    // the actual readhook.
    bool handled = false;

    gc_string name = regfile.name();
    gc_string type  = getRegTypeName(regfile);

    auto ctx = _core.is_ctx_resource(regfile);
    auto cp = getCtxParm(ctx);

    if (regfile.invalid_entry_read()) {
      gc_string fn;
      MkGStr(fn,"bits<" << regfile.width() << "> " << getInvalidEntryReadName(regfile));
      genFunc(fn,regfile.env(),regfile.invalid_entry_read()._func);
    }

    // We need an in-hook read if we have a read or a write hook.
    bool needs_read_hook = needs_write_hook;
    bool simple_regfile = false;

    gc_string rtype = base_rtype;
    if (const RegFileHook *hook = regfile.readhook()) {
      if (Ptree *hookaction = regfile.readhook()->action()) {
        // Action:  Transform and write out the action body.
        _o << rtype << " " << name << "_read_internal" << genRegHookArgs(ctx,hookaction) << " " << attr_used << "{\n";
        writeLineDirective(hook->env(),hookaction);
        _o << funcBody(hook->env(),hookaction) << "\n";
        writeCurLocation();
        _o << "};\n";
        handled = true;
        needs_read_hook = true;
      } else if (const RegBase *alias = hook->alias()()) {
        // Alias:  Return the alias's value instead.
        _o << rtype << " " << name << "_read_internal(" << cp << "unsigned index) " << attr_used << "{\n";
        const RegFile *rf = static_cast<const RegFile*>(alias);
        if (rf->sparse()) {
          _o << "return " << alias->name() << "_read(" << ctx_index(cp) << type << "::getReadIndex(index));\n";
        } else {
          _o << "return " << alias->name() << "(" << ctx_index(cp) << ")(" << type << "::getReadIndex(index)).get_data();\n";
        }
        _o << "};\n";
        handled = true;
      }
    }
    if (!handled) {
      if (regfile.sparse()) {
        // Generate switch statement.
        _o << rtype << " " << name << "_read_internal(unsigned index)" << attr_used << " {\n";
        genSparseSwitch(regfile,ssRead,&_core);
      } else {
        // Generic access.
        if (extern_regs()) {
          _o << rtype << name << "_read_internal(" << cp << "unsigned index) " << attr_used << "{\n"
             << "return " << getExternalRegRead(regfile) << "(index);\n";
        } else {
          rtype = base_rtype + " &";
          _o << rtype << name << "_read_internal(" << cp << "unsigned index) " << attr_used << "{\n";
          genRegFileReadRangeCheck(regfile);
          _o << "return " << regfile.name() << "(" << ctx_index(cp) << ")(index).get_data(" << tta() << ");\n";
          simple_regfile = true;
        }
      }
      _o << "};\n";
    }

    if (needs_read_hook && !regfile.pseudo()) {
      if (extern_regs()) {
        _o << base_rtype << " " << name << "_read_inhook(" << cp << "unsigned index) " << attr_used << "{\n"
           << "return " << getExternalRegRead(regfile) << "();\n";
      } else {
        _o << base_rtype << " &" << name << "_read_inhook(" << cp << "unsigned index) " << attr_used << "{\n";
        genRegFileReadRangeCheck(regfile);
        _o << "return " << regfile.name() << "(" << ctx_index(cp) << ")(index).get_data(" << tta() << ");\n";
      }
      _o << "}\n\n";
    }

    gc_string ctx_ctrl = (ctx) ? getContextControl(*ctx)+"," : "";
    _o << rtype << " " << name << "_read(unsigned index) " << attr_used << "{\n";
    addRegReadCallback(regfile);
    addRegBaseReadTraceCall(regfile);
    _o << "return " << name << "_read_internal(" << ctx_ctrl << "index);\n"
       << "}\n\n";

    if (log_reads()) {
      // This is the form used by field/slice accessors when register-read logging is on, in order
      // to avoid an erroneous read log message.
      _o << rtype << " " << name << "_read_notrace(unsigned index) " << attr_used << "{\n";
      addRegReadCallback(regfile);
      _o << "return " << name << "_read_internal(" << ctx_ctrl << "index);\n"
         << "}\n";
    }

    if (time_tagged() && !extern_regs() && simple_regfile) {
      // For time-tagged ISSs, add a form which returns the value, but logs this
      // as a target.  Used by field and slice accessors to correctly log a
      // read/modify/write operation. Only needed for "simple" registers, since
      // more complex ones require an explicit read/modify/write operation.
      _o << rtype << " " << name << "_read_modify(unsigned index) " << attr_used << "{\n";
      _o << rtype << "tmp = " << name << "_read_internal(" << ctx_ctrl << "index);\n"
         << name << "(" << ctx_ctrl << ")(index).add_target(*this);\n"
         << "return tmp;\n"
         << "}\n";      
    }


    // This is a simple read function for non-pseudo registers which allows
    // access to a particular thread context.  This is only created if we are a
    // context resource.  We also create an internal read function which uses
    // the current context, for use by any aliases.
    if (!regfile.pseudo() && ctx) {
      _o << rtype << " " << name << "_ctx_read(unsigned ctx_index,unsigned index) {\n"
         << "return " << regfile.name() << "_read_internal(ctx_index,index);\n"
         << "}\n"

         << rtype << " " << name << "_read_internal(unsigned index) " << attr_used << "{ return " << name << "_read_internal(" << ctx_ctrl << "index); }\n\n";      
    }

  }

  void genRegFileFieldIndexedInHookWrite(const RegFile &regfile,const RegField &rf,const gc_string &rtype,
                                         const gc_string &rftype,Ptree *cp,const gc_string &left,const gc_string &right)
  {
    gc_string rname = regfile.name();
    if (is_commit_write(regfile,true)) {
      // The in-hook write is delayed b/c this is a register with an immediate write-hook.
      gc_string findex_arg = "unsigned i,";
      gc_string findex = "i,";
      writeRegDelayBody(regfile,"CommitRegfileIndexedFieldWrite","","_"+rf.name(),"index,",findex,"","0","","","","","");

      _o << "}\n\n"
         << "void " << rname << "_" << rf.name() << "_commit_write(unsigned index," << findex_arg << "const " << rftype << " &x) {\n";
    }

    // Standard in-hook write for fields.
    if (extern_regs()) {
      _o << rtype << " tmp = " << getExternalRegRead(regfile) << "(index);\n"
         << "tmp.set<" << rf.width() << ">(i*" << rf.width() << ",(i*" << rf.width() << ")+" << rf.width() << "-1,x);\n"
         << getExternalRegWrite(regfile) << "(index,tmp);\n";
    } else {
      _o << rname << "(" << ctx_index(cp) << ")(index).set_data(" << tta() << ").set<" << rf.width() << ">(" << genSliceArgs(left,right,_core) << ",x);\n";
    }
    addRegBaseTraceCall(regfile,rf);
  }

  void genRegFileFieldInHookWrite(const RegFile &regfile,const RegField &rf,const gc_string &rtype,
                                         const gc_string &rftype,Ptree *cp)
  {
    gc_string rname = regfile.name();
    if (is_commit_write(regfile,true)) {
      // The in-hook write is delayed b/c this is a register with an immediate write-hook.
      writeRegDelayBody(regfile,"CommitRegfileFieldWrite","","_"+rf.name(),"index,","","","0","","","","","");

      _o << "}\n\n"
         << "void " << rname << "_" << rf.name() << "_commit_write(unsigned index,const " << rftype << " &x) " << attr_used << "{\n";
    }

    if (extern_regs()) {
      _o << rtype << " tmp = " << getExternalRegRead(regfile) << "(index);\n";
      gen_regfield_set(_o,"tmp","",rf,regfile.offset(),gc_string("x"));
      _o << getExternalRegWrite(regfile) << "(index,tmp);\n";		 
    } else {
      gen_regfield_set(_o,rname + "(" + ptos(ctx_index(cp)) + ")(index).set_data(" + tta() + ")", "", rf,regfile.offset(),gc_string("x"));
    }
    addRegBaseTraceCall(regfile,rf);
  }

  // Generates members storing field information.
  void genRegFileFields(const RegFile &regfile,bool is_transactional,bool is_hybrid,bool has_write_hook,
                        bool has_save,bool has_start_stop)
  {
    gc_string rname = regfile.name();
    gc_string rtype = "bits<"+uitogs(regfile.width())+">";

    gc_string pktarg,pktarg1;
    if (is_transactional || is_hybrid) {
      MkGStr(pktarg,"bits_pair<" << _packet_width << "> &slot");
      pktarg1 = pktarg + ",";
    }
    
    gc_string pktparm,pktparm1,pktparmv;
    if (is_transactional || is_hybrid) {
      pktparm = "slot";
      pktparm1 = pktparm + ",";
    }

    gc_string get, getm;
    if (is_transactional) {
      get = getm = "slot._v";
    } else if (log_reads()) {
      get = getm = rname + "_read_notrace(index)";
    } else if (time_tagged()) {
      get = rname + "_read(index)";
      getm = rname + "_read_modify(index)";
    } else {
      get = getm = rname + "_read(index)";
    }
 
    gc_string pktoff_v, pktoff_m;
    if (is_transactional || is_hybrid) {
      if (BitPair::is_little_endian()) {
        pktoff_m = "";
      } else {
        MkGStr(pktoff_m,( _packet_width - regfile.width() ) << " + ");
      }
      if (!is_hybrid) {
        pktoff_v = pktoff_m;
      }
    }
      
    auto ctx = _core.is_ctx_resource(regfile);
    auto cp = getCtxParm(ctx);

    bool commit_write = is_commit_write(regfile);
    bool write_mask = (commit_write && hasStorage(regfile) && !_core.is_shared(regfile));
    bool concurrent = _core.concurrent_writes().count(rname);
		       
    gc_string rval,rmask;
    gc_string btype = (concurrent ? "RegfileBundleElem<"+uitogs(regfile.width())+">" : "");

    gc_string commit = (write_mask ? "_commit" : "");
    // If we have fields, then they become member variables if non-indexed
    // or member functions if they are indexed.
    if (regfile.has_fields()) {
      ForEach(regfile.fields(),i) {
        const RegField &rf = *(i->second);

        if (!isFieldUsed(regfile,rf)) continue;

        gc_string rftype = "bits<"+uitogs(rf.width())+">";
        if (rf.indexed()) {
          gc_string rf_width(uitogs(rf.width()));
          gc_string left,right;
          gen_indexed_regfield_exprs(regfile,rf,left,right);
	    
          // Create the helper get/set functions.
          if (isFieldHelperUsed(regfile,rf)) {
            if (!is_transactional && !is_hybrid) {
              writeIndexedFieldHelpers(regfile,rf,rtype,rftype,left,right);
            }
          }

          if (isFieldRead(regfile,rf)) {
            _o << rftype << " " << rname << "_" << rf.name() << "_read(" << pktarg1 << "unsigned index,unsigned i) " << attr_used << "{\n";
            addRegReadCallback(regfile);
            addRegFileReadTraceCall(regfile,rf);
            writeRegFieldReadMaskSet(regfile,rf,is_hybrid,left,right,pktparm,pktoff_m);          
            _o << "return " << get << "." << dotTemplate() << "get<" << rf.width() << ">(" << genSliceArgs(left,right,_core,pktoff_v) << ");\n"    
               << "};\n\n";
          }

          if (concurrent) {
            MkGStr(rval,".set<" << rf_width << ">(" << genSliceArgs(left,right,_core) << ",x)");
            MkGStr(rmask,".mask(" << genSliceArgs(left,right,_core) << ")"); 
          }

          if (isFieldWritten(regfile,rf)) {
            genRegFieldOuterWrite(regfile,rf,is_transactional,is_hybrid,
                                  commit_write,write_mask,has_save,has_start_stop,
                                  "CommitRegfileIndexedFieldWrite","index",
                                  pktarg1,rtype,rftype,left,right,getm,
                                  pktparm,pktoff_v,pktoff_m,btype,rval,rmask);
          }

          if (!is_transactional && !is_hybrid && !regfile.pseudo()) {
            if (ctx) {
              has_write_hook = true;
              if (isFieldRead(regfile,rf)) {
                _o << rftype << " " << rname << "_" << rf.name() << "_ctx_read(" << cp << "unsigned index,unsigned i) {\n"
                   << "return " << rname << "_read_internal(" << ctx_index(cp) << ",index)." << dotTemplate() << "get<" << rf.width() << ">(" << genSliceArgs(left,right,_core) << ");\n"
                   << "};\n";
              }

              if (isFieldWritten(regfile,rf)) {
                _o << "void " << rname << "_" << rf.name() << "_ctx_write(" << cp << "unsigned index,unsigned i,const " << rftype << " &x) {\n";
                if ((extern_regs() || has_hooks(regfile)) && !is_transactional) {
                  if (has_start_stop) {
                    _o << rname << "_write_internal(ctx_index,index,x" << hookFieldSliceArgs(rf) << ");\n";
                  } else {
                    _o << rtype << " tmp = " << rname << "_read_internal(ctx_index,index);\n"
                       << "tmp.set<" << rf.width() << ">(i*" << rf.width() << ",(i*" << rf.width() << ")+" << rf.width() << "-1,x);\n"
                       << rname << "_write_internal(ctx_index,index,tmp);\n";
                  }
                } else {
                  _o << rname << "_" << rf.name() << "_write_inhook(" << ctx_index(cp) << ",index,i,x);\n";
                }
                addRegFileCtxTraceCall(regfile,*ctx,cp);
                _o << "}\n";
              }
            }
            if (has_write_hook) {
              _o << "void " << rname << "_" << rf.name() << "_write_inhook(" << cp << "unsigned index,unsigned i,const " << rftype << " &x) " << attr_used << "{\n";
              genRegFileFieldIndexedInHookWrite(regfile,rf,rtype,rftype,cp,left,right);
              _o << "};\n";
            }
          }

        } else {
          // Non-indexed field- no arguments are required.

          // Create the helper get/set functions.
          if (isFieldHelperUsed(regfile,rf) || concurrent) {
            if (!is_transactional && !is_hybrid) {
              writeFieldHelpers(regfile,rf,rtype,rftype);
            }
          }

          if (isFieldRead(regfile,rf)) {
            _o << rftype << " " << rname << "_" << rf.name() << "_read(" << pktarg1 << "unsigned index) " << attr_used << "{\n";
            addRegReadCallback(regfile);
            writeRegFieldReadMaskSet(regfile,rf,is_hybrid,"","",pktparm,pktoff_m);
            addRegFileReadTraceCall(regfile,rf);
            gen_regfield_get(_o, get, pktoff_v, rf, regfile.offset(),dotTemplate());
            _o << "}\n\n";
          }

          // TBD
          if (concurrent) {
            MkGStr(rmask, " = \"" << rf.write_mask().str() << "\"");
            MkGStr(rval," = setfield_" << rname << "_" << rf.name() << "(value,x)");
          }

          if (isFieldWritten(regfile,rf)) {
            genRegFieldOuterWrite(regfile,rf,is_transactional,is_hybrid,
                                  commit_write,write_mask,has_save,has_start_stop,
                                  "CommitRegfileFieldWrite","index",
                                  pktarg1,rtype,rftype,"","",getm,pktparm,pktoff_v,pktoff_m,btype,rval,rmask);
          }

          if (!is_transactional && !is_hybrid && !regfile.pseudo()) {
            if (ctx) {
              has_write_hook = true;
              // For accessing register fields in specific thread contexts.

              if (isFieldRead(regfile,rf)) {
                _o << rftype << " " << rname << "_" << rf.name() << "_ctx_read(unsigned ctx_index,unsigned index) {\n";
                gen_regfield_get(_o, rname + "_read_internal(" + ptos(ctx_index(cp)) + ",index)", "", rf, regfile.offset(),dotTemplate());
                _o << "};\n";
              }

              if (isFieldWritten(regfile,rf)) {
                _o << "void " << rname << "_" << rf.name() << "_ctx_write(" << cp << "unsigned index,const " << rftype << " &x) {\n";
                if ((extern_regs() || has_hooks(regfile)) && !is_transactional) {
                  if (has_start_stop) {
                    _o << rname << "_write_internal(" << ctx_arg(cp) << "index,x" << hookFieldSliceArgs(rf) << ");\n";		 
                  } else {
                    _o << rtype << " tmp = " << rname << "_read_internal(" << ctx_arg(cp) << "index);\n";
                    gen_regfield_set(_o,"tmp","",rf,regfile.offset(),gc_string("x"));
                    _o << rname << "_write_internal(" << ctx_arg(cp) << "index,tmp);\n";		 
                  }
                } else {
                  _o << rname << "_" << rf.name() << "_write_internal(" << ctx_index(cp) << ",index,x);\n";
                }
                addRegFileCtxTraceCall(regfile,*ctx,cp);
                _o << "}\n";
              }
            }
            if (has_write_hook) {
              _o << "void " << rname << "_" << rf.name() << "_write_inhook(" << cp << "unsigned index,const " << rftype << " &x) " << attr_used << "" << attr_used << "{\n";
              genRegFileFieldInHookWrite(regfile,rf,rtype,rftype,cp);
              _o << "};\n";
            }
          }
        }
      }
    }

    // For a transactional ISS, we need a mask-write function which can update
    // a portion of a register.
    if (is_transactional && needRegPartialAccess(regfile)) {
      genRegMaskSet(regfile,"CommitRegFileMaskSet","index",pktarg,pktparm,commit);
    }

    // Then generate a function for updating a portion of a register-file (a slice).
    genRegSliceWrite(regfile,is_transactional,is_hybrid,commit_write,write_mask,has_save,has_start_stop,
                     "CommitRegfileSliceSet","index","(index)",pktarg1,rtype,getm,pktparm,pktoff_v,pktoff_m,btype);

    // This is a simple slice-write function for non-pseudo registers which allows access to a particular
    // thread context.  This is only created if we are a context resource.
    if (!regfile.pseudo() && _core.is_ctx_resource(regfile) && !is_transactional && !is_hybrid && isRegSliceWritten(regfile)) {
      _o << "template <size_t Nb>\n"
         << "void " << rname << "_ctx_slice_set(" << cp << "unsigned index,unsigned x,unsigned y,const bits<Nb> &v) {\n";
      if (has_hooks(regfile) && !is_transactional) {
        _o << rtype << " tmp = " << rname << "_read_internal(" << ctx_arg(cp) << "index);\n"
           << "tmp.set(x,y,v);\n"
           << rname << "_write_internal(" << ctx_arg(cp) << "index,tmp);\n";		 
      } else {
        _o << rname << "(" << ctx_index(cp) << ")(index).set_data(" << tta() << ").set(x,y,v);\n";
      }
      addRegFileCtxTraceCall(regfile,*ctx,cp);
      _o << "}\n"
         << "void " << rname << "_ctx_slice_set(" << cp << "unsigned index,unsigned x,unsigned y,uint64_t v) {\n"
         << rname << "_ctx_slice_set(" << ctx_arg(cp) << "index,x,y," << rtype << "(v));\n"
         << "}\n";
    }
  }

  typedef gc_set<const RegFile *> RegFileSet;

  void writeRegFileEntryHook(const RegFile &regfile,const RegEntry &entry,ssType type,
                             RegFileSet &rf_set,const gc_string &rtype,Environment *env,Ptree *func)
  {
    Ptree *args = 0,*index = 0;
    gc_string name;
    if (entry.reg().regfile()) {
      // Skip if we've already generated this function.
      if (rf_set.count(entry.reg().regfile())) return;
      rf_set.insert(entry.reg().regfile());
      index = PtreeUtil::GenSym();
      _walker.set_alt_thisreg_index(index);
      args = add_arg(func,Ptree::Make("unsigned %p",index));
      name = sparse_hook_name(regfile,entry.id(),type,entry.reg().regfile());      
    } else {
      name = sparse_hook_name(regfile,entry.id(),type);
      args = get_func_args(func);
    }
    _o << rtype << ' ' << name << "(" << _walker.translateCode(env,args) << ")\n"
       << _walker.translateCode(env,get_func_body(func)) << "\n\n";
      _walker.set_alt_thisreg_index(0);
  }

  // If we have a sparse register file, generate any entry hooks.
  void genRegFileEntryHooks(const RegFile &regfile)
  {
    RegFileSet rf_reads,rf_writes;
    ForEach(regfile.entries(),i) {
      const RegEntry &entry = *(i->second);
      if (const RegEntryHook *read = entry.read()) {
        if (!read->ignore()) {
          writeRegFileEntryHook(regfile,entry,ssRead,rf_reads,getRegDataType(regfile),read->env(),read->action());
        }
      }
      if (const RegEntryHook *write = entry.write()) {
        if (!write->ignore()) {
          writeRegFileEntryHook(regfile,entry,ssWrite,rf_writes,"void",write->env(),write->action());
        }
      }
    }
  }

  // Generate register-file reset code.
  void genRegFileReset(const RegFile &rf)
  {
    gc_string name  = rf.name();
    gc_string rtype = getRegDataType(rf);

    // The rest of the interface is only needed for real registers.
    if (hasStorage(rf) && !extern_regs()) {

      auto ctx = _core.is_ctx_resource(rf);
      auto cp = getCtxParm(ctx);

      _o << "void " << name << "_reset_value(" << ctx_parm_nc(cp) << ") {\n";
      // Insert reset function.
      switch (rf.resettype()) {
      case RegBase::None:
      case RegBase::Value:
        _o << name << "().reset(" << rtype << "(\"" << rf.resetvalue() << "\"));\n";
        break;      
      case RegBase::Enum: 
        break;
      case RegBase::Action: {
        _o << name << "_reset_action(" << ctx_arg(cp) << ctx_index(cp) << ");\n";
      }
      default:
        ;
      }
      if (!extern_regs()) {
        _o << name << "(" << ctx_index(cp) << ").reset_tags();\n";
      }
      _o << "};\n";
      if (rf.resettype() == RegBase::Action) {
        Ptree *action = rf.resetaction();
        _o << "void " << name << "_reset_action" << genRegHookArgs(ctx,action) << " {\n";
        writeLineDirective(rf.env(),action);
        _o << funcBody(rf.env(),action) << "\n";
        writeCurLocation();
        _o << "}\n";
      }
    }
  }

  // Writes out a register file.  The register file is a simple data structure-
  // any logic is contained in the accessor methods, which are methods of the 
  // core itself.  This requires a code transformation to, for example, convert
  // GPR(RT) into GPR_read(RT).
  void genRegFileObjects(const RegFile &regfile)
  {
    gc_string name     = regfile.name();
    gc_string rdata    = getRegDataObj(regfile);
    gc_string rtype    = getRegDataType(regfile);
    gc_string type     = getRegTypeName(regfile);
    gc_string basetype = getRegFileBaseType(regfile,rdata);

    _walker.set_cur_reg(name,_core.is_ctx_resource(regfile));

    _o << "// Register File " << name << "\n";

    genRegFileTraceLogic(regfile);
    // Generate the write hook.
    bool has_save = false;
    bool has_write_hook = false;
    bool has_start_stop = false;
    bool concurrent = false;
    genRegFileWriteHook(has_write_hook,concurrent,has_save,has_start_stop,regfile,rtype);

    // Generate the external write hook, if relevant.
    genRegExtWriteHook(regfile);

    // Concurrent write hooks (ones which take a ValueBundle argument), may use
    // the updateReg helper function to update the register all at once.
    if (concurrent && has_write_hook && !regfile.pseudo()) {
      genRegUpdateFunc(regfile,true);
    }

    // Generate the read hook.
    genRegFileReadHook(regfile,rtype,has_write_hook);

    if (has_save) {
      genRegSaveLogic(regfile,rtype,"CommitRegfileWrite","index");
    }

    // For fields and slice operators, if we're transactional, we need both the
    // original and instruction packet-aware versions, since there are some
    // functions which don't use the packet and some that do.
    genRegFileFields(regfile,false,false,has_write_hook,has_save,has_start_stop);
    if (transactional_iss() || hybrid_iss()) {
      genRegFileFields(regfile,transactional_iss(),hybrid_iss(),has_write_hook,has_save,has_start_stop);
    }

    genRegFileEntryHooks(regfile);
    genRegFileReset(regfile);

    _o << "\n";

    _walker.reset();
  }

  // Write out all register files.
  void generateRegFiles()
  {
    const RegFileHash &regfiles = _core.regfiles();

    _o << "//\n// Register File Handling Code.\n//\n\n";
    ForEach(regfiles,i) {
      const RegFile &regfile = *(i->second);
      genRegFileObjects(regfile);
    }
    _o << "\n";
  }

  void writeResetCalls(const StrList &por_regs,const StrList &reset_regs,const gc_string &ctxctrl)
  {
    ForEach(reset_regs,i) {
      _o << *i << "_reset_value(" << ctxctrl << ");\n";
    }
    if (!por_regs.empty()) {
      _o << "if (por_reset) {\n";
      ForEach(por_regs,i) {
        _o << *i << "_reset_value(" << ctxctrl << ");\n";
      }
      _o << "}\n";
    }
  }

  // When creating the register reset logic, we create two logical groupings:
  // Those registers with a reset value and those without.  Those without are
  // only reset if the por_reset flag is true.
  void genRegResetInternal(const RegCtxData &regdata,const gc_string &ctxctrl)
  {
    StrList por_regs,reset_regs;
    ForEach(regdata._regs,i) {
      const Reg &reg = *(i->_reg);
      if (reg.resettype() != RegBase::None) {
        reset_regs.push_back(reg.name());
      } else if (!reg.pseudo()) {
        por_regs.push_back(reg.name());
      }
    }
    writeResetCalls(por_regs,reset_regs,ctxctrl);

    StrList por_rfs,reset_rfs;
    ForEach(regdata._regfiles,i) {
      const RegFile &rf = *(i->_reg);
      if (rf.resettype() != RegBase::None) {
        reset_rfs.push_back(rf.name());
      } else if (!rf.pseudo() && !rf.sparse()) {
        por_rfs.push_back(rf.name());
      }
    }
    writeResetCalls(por_rfs,reset_rfs,ctxctrl);
  }

  void genRegCallbacksInternal(const gc_string &type,const StrSet &callbacks,bool read)
  {
    _cd._member_vars.push_back("std::vector<" + type + "*> " + getCallbackName(read));
    
    gc_string defval = (read) ? ("(("+type+"*)0)") : ("&"+getDefaultCallback());
    gc_string init;
    MkGStr(init,getCallbackName(read)+"(" << callbacks.size() << "," << defval << ")");
    _cd._constr_inits.push_back(init);
    _o << "virtual bool set" << type << "(const std::string &name," << type << " *cb) {\n"
       << "int rindex, rfindex;\n"
       << "if (!getRegIndex(rindex,rfindex,name)) {\n"
       << "return false;\n"
       << "}\n"
       << "return set" << type << "(rindex,cb);\n"
       << "}\n\n"
       << "virtual bool set" << type << "(unsigned rindex," << type << " *cb) {\n"
       << "switch (rindex) {\n";
    int count = 0;
    ForEach(callbacks,i) {
      const RegBase *reg = _core.get_regitem(*i);
      // Should have already been checked during config parsing.
      assert(reg);
      _o << "case " << reg->id() << ":\n"
         << getCallbackName(read) << "[" << count++ << "] = (cb) ? cb : " << defval << ";\n"
         << "return true;\n";
    }
    _o << "default:\n"
       << "return false;\n"
       << "}\n"
       << "}\n\n";
  }

  // Generate register callback support, if necessary.
  void genRegCallbacks()
  {
    // If we have callbacks, then generate the appropriate setup function.
    if (has_reg_callbacks()) {
      genRegCallbacksInternal("RegCallback",reg_callbacks(),false);
    }

    // Same for read callbacks.
    if (has_reg_read_callbacks()) {
      genRegCallbacksInternal("RegReadCallback",reg_read_callbacks(),true);
    }
  }

  void genRegAliasResolverItem(const RegName &rn,const RegBase *e_reg,const gc_string &f_name)
  {
    // To avoid an infinite loop, we do not recurse if we're dealing with the
    // current item.
    if (rn()) {
      _o << dec;
      if (rn() == e_reg) {
        _o << "ops.push_back(CoreOpInfoM(" << rn()->id() << ",";
      } else {
        _o << f_name << "(ops," << rn()->id() << ",";
      }
      if (rn.has_child_id()) {
        // This is wrong (should be _child_id), but this is a static function
        // and we don't have access to that data.  We should fix this
        // eventually.
        _o << "0";
      }
      else if (rn.has_index()) {
        _o << rn.index();
      } 
      else {
        _o << "-1"; 
      }
      _o << ",type";
      if (rn() == e_reg) {
        _o << ",true";
      }
      _o << ");\n";
    }
  }

  // If we have an index, then I don't know how we can really resolve that,
  // since the index might be based upon a variable set at runtime.  For now,
  // I'll just skip it and deal with it later.
  void genRegAliasResolverItem(const UsedReg &ur,const RegBase *e_reg,const gc_string &f_name,
                               const gc_string &type,bool add_this)
  {
    // Skip the register that's the entry.  We want the entry to always be
    // listed first, so it's done one level up.
    _o << dec;
    int index;
    if (!ur._begin || !get_int(index,ur._begin,ur.regb()->env())) {
      index = -1;
    }
    
    if (ur.regb() != e_reg) {    
      if (!ur._mask.iszero()) {
        if (ur._mask.size() > 64) {
          _o << "{\n"
             << "static intbv<MaxRegisterWidth> big_mask(\"0x" << hex << ur._mask << "\");\n" << dec
             << "ops.push_back(CoreOpInfoM(" << ur.regb()->id() << "," << index << "," << type << ",false,big_mask));\n"
             << "}\n";
        } else {
          gc_string sfx = (ur._mask > 32) ? "ull" : "";
          _o << "ops.push_back(CoreOpInfoM(" << ur.regb()->id() << "," << index << "," << type << ",false,0x" << hex << ur._mask << sfx << "));\n" << dec;
        }
      } else {
        _o << "ops.push_back(CoreOpInfoM(" << ur.regb()->id() << "," << index << "," << type << ",false));\n";
      }
    } else if (add_this) {
      _o << "ops.push_back(CoreOpInfoM(" << ur.regb()->id() << ",index,type,true));\n";      
    }
  }

  void genRegAliasResolverItem(const UsageInfo &ui,const RegBase *e_reg,const gc_string &f_name,bool add_this)
  {
    ForEach(ui._src_regs,s_iter) {
      genRegAliasResolverItem(*s_iter,e_reg,f_name,"OpInfo::opSrc",add_this);
    }
    ForEach(ui._trg_regs,t_iter) {
      genRegAliasResolverItem(*t_iter,e_reg,f_name,"OpInfo::opTrg",add_this);
    }    
  }

  void genRegAliasResolverInternal(const RegList &regs,const RegFileList &regfiles,
                                   bool is_read,const gc_string &f_name)
  {
    // For each register or register-file in the system, if it's not an alias or
    // sparse, then return original data.  Otherwise, call the function again
    // with the target information.  If sparse or has a hook, then push on usage
    // info.
    //
    // We only evaluate hooks if _all_reg_aliases is true.  It's on by default,
    // but uADL turns it off, since it's only interested in virtualized
    // registers, and hooks work on non-virtualized items, i.e. the hook is
    // called on the initial operand read or commit, not during execution.
    _o << "switch (id) {\n";
    if (_config.all_reg_aliases()) {
      ForEach(regs,r_iter) {
        const Reg &reg = *r_iter->_reg;
        if (const RegHook *rh = ((is_read) ? reg.readhook() : reg.writehook())) {
          if (rh->aliased()) {
            _o << "case " << dec << reg.id() << ":\n";
            genRegAliasResolverItem(rh->alias(),&reg,f_name);
            _o << "return;\n";
          } else if (!rh->usageinfo().empty()) {
            _o << "case " << dec << reg.id() << ":\n";
            genRegAliasResolverItem(rh->usageinfo(),&reg,f_name,true);
            _o << "return;\n";
          }
        }
      }
    }
    ForEach(regfiles,r_iter) {
      const RegFile &rf = *r_iter->_reg;
      if (rf.sparse()) {
        // Sparse register file, so we have to list each element and its
        // corresponding entry.  If it's anything other than a simple entry,
        // then we skip it.
        const RegEntryHash &entries = rf.entries();
        _o << "case " << dec << rf.id() << ":\n";
        _o << "switch (index) {\n";
        ForEach(entries,e_iter) {
          const RegEntry &entry = *e_iter->second;
          if (entry.read() || entry.write() || entry.reg()) {
            // No read or write hook, so just deal with a simple entry.
            _o << "case " << dec << entry.id() << ":\n";
            genRegAliasResolverItem(entry.reg(),&rf,f_name);
            if (const RegEntryHook *reh = (is_read) ? entry.read() : entry.write()) {
              genRegAliasResolverItem(reh->usageinfo(),entry.reg()(),f_name,false);
            }
            _o << "return;\n";
          }
        }
        _o << "}\n"
           << "return;\n";
      } else if (_config.all_reg_aliases()) {
        if (const RegFileHook *rh = ((is_read) ? rf.readhook() : rf.writehook())) {
          if (rh->aliased()) {
            _o << "case " << dec << rf.id() << ":\n";
            if (!rh->aliasindices().empty()) {
              const RegName &rn = rh->alias();
              unsigned t_index = rn()->id();
              // Has an index mapping, so we list each element.
              _o << "switch (index) {\n";
              ForRange((rh->aliasindices().size()),count) {
                _o << "case " << dec << count << ":\n"
                   << f_name << "(ops," << t_index << "," << rh->aliasindices()[count] << ",type);\n"
                   << "return;\n";
              }
              _o << "}\n"
                 << "return;\n";
            } else {
              _o << "case " << dec << rf.id() << ":\n";
              genRegAliasResolverItem(rh->alias(),&rf,f_name);
              _o << "return;\n";
            }
          } else if (!rh->usageinfo().empty()) {
            _o << "case " << dec << rf.id() << ":\n";
            genRegAliasResolverItem(rh->usageinfo(),&rf,f_name,true);
            _o << "return;\n";
          }
        }
      }
    }
    _o << "}\n";
  }

  // For use by performance models: Provide support for disambiguating
  // register aliases.
  void genRegAliasResolver(const RegList &regs,const RegFileList &regfiles)
  {
    _o << "static void " << getRegReadAliasResolverName() << "(CoreOpInfoMs &ops,unsigned id,int index,int type) {\n";
    genRegAliasResolverInternal(regs,regfiles,true,getRegReadAliasResolverName());
    _o << "ops.push_back(CoreOpInfoM(id,index,type,true));\n"
       << "}\n\n"
       << "static void " << getRegWriteAliasResolverName() << "(CoreOpInfoMs &ops,unsigned id,int index,int type) {\n";
    genRegAliasResolverInternal(regs,regfiles,false,getRegWriteAliasResolverName());
    _o << "ops.push_back(CoreOpInfoM(id,index,type,true));\n"
       << "}\n\n"
       << "static void " << getRegReadAliasResolverName() << "(CoreOpInfoMs &ops,const OpInfo &opinfo) {\n"
       << getRegReadAliasResolverName() << "(ops,opinfo._id,opinfo._index,opinfo._type);\n"
       << "}\n\n"
       << "static void " << getRegWriteAliasResolverName() << "(CoreOpInfoMs &ops,const OpInfo &opinfo) {\n"
       << getRegWriteAliasResolverName() << "(ops,opinfo._id,opinfo._index,opinfo._type);\n"
       << "}\n\n";
  }

  // If we have global invalid read/write hooks, then generate them here.
  void genGlobalInvalidHooks(const RegFileList &regfiles)
  {
    // Find the size of the wideset sparse register file and use that as the return-value size.
    unsigned width = 0;
    ForEach(regfiles,i) {
      const RegFile &regfile = *(i->_reg);
      if (regfile.sparse()) {
        width = max(width,regfile.width());
      }
    }
    
    // Sanity check- if we don't have any sparse registers, then we don't need
    // to generate this code.
    if (width) {
      if (_core.invalid_entry_write()) {
        genFunc("void " + getGlobalInvalidEntryWriteName(),_core.env(),_core.invalid_entry_write()._func);
      }
      
      if (_core.invalid_entry_read()) {
        gc_string fn;
        MkGStr(fn,"bits<" << width << "> " << getGlobalInvalidEntryReadName());
        genFunc(fn,_core.env(),_core.invalid_entry_read()._func);
      }
    }
  }

  // Generate support types for register-file attributes.
  void generateRegFileAttrs()
  {
    const IntSet &regfile_attrs = _core.regfile_attrs();
    if (!regfile_attrs.empty()) {
      uint64_t flag = 1;

      _o << "enum RegFileAttrs {\n" << hex;
      gc_string sfx = (regfile_attrs.size() > 32) ? "ull" : "";
      ForEach(regfile_attrs,iter) {
        _o << getRegFileAttrName(Data::get_attr_name(*iter)) << " = 0x" << flag << sfx << ",\n";
        flag <<= 1;
      }
      _o << "};\n\n" << dec;
    }
  }

  void genPcSetFunction(const Reg *reg,const char *name)
  {
    _o << "virtual void " << name << "(addr_t a)\n{\n";
    if (extern_regs()) {
      _o << getExternalRegWrite(*reg) << "(a);\n";
    } else {
      gc_string dname = regDbgAccessor(*reg,false);
      if (dname.empty()) {
        // Some strange case- we'll have to fall back to using the non-debug
        // access method.  Shouldn't normally occur.
        _o << reg->name() << "_write(a);\n";
      } else {
        // Normally, we'll access the NIA using the usual debug method.
        _o << dname << ".set_value(a);\n";
      }
    }
    _o << "}\n\n";
  }

  void genPcGetFunction(const Reg *reg,const char *name)
  {
    _o << "virtual addr_t " << name << "() const \n{\n";
    if (extern_regs()) {
      _o << "return " << getExternalRegRead(*reg) << "().uint64();\n";
    } else {
      gc_string dname = regDbgAccessor(*reg,true);
      if (dname.empty()) {
        // Some strange case- we'll have to fall back to using the non-debug
        // access method.  Shouldn't normally occur.
        _o << "return " << reg->name() << "_read().uint64();\n";
      } else {
        // Normally, we'll access the NIA using the usual debug method.
        _o << "return " << dname << ".get_value().uint64();\n";
      }
    }
    _o << "}\n\n";
  }

  void generateRegSupport()
  {
    RegCtxMap regdata;
    makeRegLists(regdata,_core);

    RegList regs;
    RegFileList regfiles;

    ForEach(regdata,i) {
      regs.insert(regs.end(),i->second._regs.begin(),i->second._regs.end());
      regfiles.insert(regfiles.end(),i->second._regfiles.begin(),i->second._regfiles.end());
    }

    // Create the register reporting function.  Ignore the CIA and ignore
    // unarchitected registers if the log-reg-unarch flag is set to false.
    int ignores[] = { Data::get_attr(Cia), (log_unarch() ? -1 : Data::get_attr(Unarchitected)), -1 };

    generateRegAssistFuncs(regdata,regs,regfiles,ignores,false);

    DbgRegList debug_regs;
    checkForDebugRegs(debug_regs,_core);
    if (!debug_regs.empty()) {
      genDefaultRegMap(debug_regs);
    }

    // We implement the logReg function here so that we can take into accont
    // whether we're using RNumber or not.
    _o << "bool logReg(unsigned rindex,unsigned index) const\n"
       << "{\n"
       << "REGTYPE_t value;\n"
       << "if (!getReg(rindex,index,value)) return false;\n"
       << "const RegInfo &ri = getRegInfo()[rindex];\n"
       << "if (ri.is_regfile()) {\n"
       << "_logger->log_regfile_ext_write(rindex,ri._name.c_str(),index,value);\n"
       << "_logger->log_regfile_write(rindex,ri._name.c_str(),index,value);\n"
       << "} else {\n"
       << "_logger->log_reg_ext_write(rindex,ri._name.c_str(),value);\n"
       << "_logger->log_reg_write(rindex,ri._name.c_str(),value);\n"
       << "}\n"
       << "return true;\n"
       << "}\n\n";

    genGlobalInvalidHooks(regfiles);

    genRegCallbacks();

    if (!extern_regs()) {
      // Create the register reset function.  If we have contexts, then we have to cycle
      // through all context in order to reset all registers.

      _o << "struct ResetRegLogger : public ReportBase {\n"
         << "virtual void report_reg (const IssNode *n,unsigned id,const char *name,REGTYPE value,bool shared) {\n"
         << "n->logger().log_reg_write(id,name,value);\n"
         << "};\n"
         << "virtual void report_regfile (const IssNode *n,unsigned id,const char *name,int index,REGTYPE value,bool shared) {\n"
         << "n->logger().log_regfile_write(id,name,index,value);\n"
         << "};\n"
         << "};\n\n"

         << "virtual void resetRegs(bool por_reset = true,bool log = false)\n{\n";
      ForEach(regdata,iter) {
        if (iter->first == DefaultCtxName) {
          continue;
        }
        const Context *ctx = iter->second._ctx;

        genCtxSwap(ctx,0,false);
        gc_string ctxctrl = getContextControl(*ctx);
        _o << "for (" << ctxctrl << " = 0; " << ctxctrl << " != " << ctx->num_contexts() << "; ++" << ctxctrl << ") {\n";

        genRegResetInternal(iter->second,ctxctrl);

        _o << "}\n";
        genCtxSwapBack(ctx);
      }

      // Generate the non-context based registers last, in case we have aliases to
      // context registers that are irregular in their reset values, e.g. an alias
      // that resets one instance of a context register.
      genRegResetInternal(regdata.find(DefaultCtxName)->second,"");
      _o << "if (Tracing && log && (_trace_mode & ttRegWrite)) {\n"
         << "ResetRegLogger rl;\n"
         << "showRegs(rl);\n"
         << "}\n"
         << "}\n\n";

      _cd._reset_stmts.push_back("resetRegs(por_reset,log)");
    }

    // For use by performance models: Provide support for disambiguating
    // register aliases.
    if (!normal_iss()) {
      genRegAliasResolver(regs,regfiles);
    }

    gc_string dname;

    // Create the program-counter set function.
    const Reg *nia = _core.nia();
    assert(nia);

    const Reg *cia = _core.cia();
    assert(cia);

    genPcSetFunction(nia,"setProgramCounter");
    genPcGetFunction(nia,"getProgramCounter");
    genPcGetFunction(cia,"getCurrentInstrAddr");
  }

  // Writes out all registers and register-files in the design.
  void generateRegResources()
  {
    // Write out the register resources.
    generateRegs();
    generateRegFiles();
    generateRegSupport();
  }

};

GenRegsBaseIF::GenRegsBaseIF(opfxstream &o,ClassData &cd,const ConfigDB &config) :
  _impl(new GenRegsBase(o,cd,config))
{
}

GenRegsBaseIF::GenRegsBaseIF(GenRegsBase *impl) : _impl(impl) {};

ImplCallArgs(GenRegsBaseIF,generateRegDS,(const RegList &regs, const Core *core),(regs,core));

ImplCallArgs(GenRegsBaseIF,generateRegFileDS,(const RegFileList &rf,const Core *core),(rf,core));

ImplCallArgs(GenRegsBaseIF,generateRegVars,(ClassData &cd,const RegList &regs),(cd,regs));

ImplCallArgs(GenRegsBaseIF,generateRegFileVars,(ClassData &cd,const RegFileList &rf),(cd,rf));

ImplCallArgs(GenRegsBaseIF,generateRegAssistFuncs,(const RegCtxMap &regdata,const RegList &regs,const RegFileList &regfiles,
                                                   int *ignore_class,bool is_system),
             (regdata,regs,regfiles,ignore_class,is_system));

GenRegsIF::GenRegsIF(opfxstream &o,const Core &core,InstrWalker &walker,
                     const gc_string &filename,RegBaseSet &mmu_regs,
                     Parents &parents,Watches &watches,ClassData &cd,
                     const ConfigDB &config, const CoreConfig &coreconfig) :
  GenRegsBaseIF(new GenRegs(o,core,walker,filename,mmu_regs,parents,watches,cd,config,coreconfig)),
  _impl(static_cast<GenRegs*>(GenRegsBaseIF::_impl))
{
}

ImplCallArgs(GenRegsIF,setPacketWidth,(unsigned w),(w));

ImplCall(GenRegsIF,generateRegFileAttrs);

ImplCall(GenRegsIF,generateRegResources);

}

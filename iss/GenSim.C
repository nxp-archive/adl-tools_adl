//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// The main code for generating the simulator.
//

#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <stdexcept>
#include <assert.h>

#include <boost/algorithm/string/case_conv.hpp>

#include "opencxx/parser/Ptree.h"
#include "opencxx/parser/PtreeUtil.h"
#include "opencxx/Environment.h"

#include "helpers/pfxstream.h"
#include "helpers/AnyOption.h"
#include "helpers/Macros.h"
#include "helpers/CodeGen.h"
#include "helpers/FilenameUtils.h"

#include "fe/FieldSupport.h"
#include "fe/DecodeSupport.h"
#include "fe/Data.h"
#include "fe/Helpers.h"
#include "AttrsHelper.h"

#include "InstrWalker.h"
#include "ConfigDB.h"
#include "GenSim.h"
#include "GenRegs.h"
#include "GenMems.h"
#include "GenMmu.h"
#include "GenCache.h"
#include "GenInstrPacket.h"
#include "GenInstrs.h"
#include "GenExceptions.h"
#include "MiscTypes.h"
#include "PfxFileArray.h"
#include "adl_config.h"

using namespace std;
using namespace Opencxx;
using namespace PtreeUtil;

namespace adl {

   
   bool is_little_endian(const System &system)  {
     if (system.local_cores().size()>0) {
       return (system.local_cores().begin()->second->bitEndianness() == Core::Little);
     }
     bool ret = false;
     ForEach(system.local_systems(),i) {
       if(is_little_endian(*i->second)) {
	 ret = true;
       }
     }
     return ret;        
   }

   bool is_little_endian(const Data &data) {
    if (data.cores().size()>0) {
      return (data.cores().begin()->second->bitEndianness() == Core::Little);      
    }
    bool ret = false;
    ForEach(data.systems(),i) {
      if(is_little_endian(*i->second)) {
	ret = true;
      }
    }
    return ret;
  } 

  const Core *isCore(const SysItem *n)
  {
    return dynamic_cast<const Core *>(n);
  }
  
  const System *isSystem(const SysItem *n)
  {
    return dynamic_cast<const System *>(n);
  }

  gc_string getEbHandler()
  {
    return "_ebhandler";
  }

 
  struct GenBase : public gc {
    GenBase(opfxstream &o,PfxFileArray *impls,opfxstream *jo,opfxstream *gho,
            const ConfigDB &config, const BaseConfig *baseconfig, 
            const gc_string &filename, StrSet *externs) :
      _o(o),
      _impls(impls),
      _jo(jo),
      _gho(gho),
      _config(config),
      _filename(filename)
    {
      if(externs) {
        set_extern_mem(*externs);
      }
      set_extern_mem(baseconfig);
    }

  protected:
    opfxstream       &_o;
    PfxFileArray     *_impls;
    opfxstream       *_jo;
    opfxstream       *_gho;
    const ConfigDB   &_config;
    gc_string            _filename;
    StrSet	          _externs;

  public:

    bool time_tagged() const
    {
      return _config.time_tagged();
    }

    bool td_api() const
    {
      return _config.td_api();
    }

    bool tags_only_caches() const
    {
      return _config.tags_only_caches();
    }

    bool extern_mem() 
    {
      return _config.extern_mem();
    }

    bool extern_dmi()
    {
      return _config.extern_dmi();
    }

    bool extern_mem(const gc_string &name) 
    {
      return (_externs.find(name) != _externs.end());
    }

    void set_extern_mem(const StrSet &externs) 
    {
      _externs.insert(externs.begin(),externs.end());
    }

    bool graphite_support() const
    {
      return _config.graphite_support();
    }

    void set_extern_mem(const BaseConfig *baseconfig) 
    {
      if(baseconfig) {
        const BaseConfig::Memories &memories = baseconfig->memories();
        ForEach(memories,i) {
          if(i->second) {
            _externs.insert(i->first);
          }
        }
      }
    }

    void writeCurLocation()
    {
      adl::writeCurLocation(_o,_config,_filename);
    }

    void writeLineDirective(Environment *env,Ptree *expr)
    {
      adl::writeLineDirective(_o,_config,env,expr);
    }

    void genParentRefs(ClassData &cd,Parents::const_iterator pb,Parents::const_iterator pe)
    {
      for ( ; pb != pe; ++pb) {
        cd._constr_args.push_back(pb->_type + " &" + pb->_arg);
        cd._constr_inits.push_back(pb->_ref + "(" + pb->_arg + ")");
        cd._member_vars.push_back(pb->_type + " &" + pb->_ref);
      }
    }
  
    gc_string getSysItemType(const SysItem &s)
    {
      if (s.type().empty()) {
        return s.name();
      } else {
        return s.type();
      }
    }

    gc_string getParmArg(const Parm &p)
    {
      return p.name() + "_init";
    }

    gc_string getEventBusIndexName(const EventBus &eb)
    {
      return eb.name() + "_handle";
    }

    gc_string getEventBusIdName(const EventBus &eb)
    {
      return eb.name() + "_id";
    }

    gc_string getEventBusDataType(const EventBus &eb)
    {
      return eb.name() + "_t";
    }

    void genEventBusDS(const EventBus &eb,const Resources &shared,ClassData &cd,bool atleaf)
    {
      gc_string ebname = getEventBusIndexName(eb);
      gc_string ebid   = getEventBusIdName(eb);

      if (!shared.contains(eb)) {
        // First, generate the data packet type.
        gc_string ebt = getEventBusDataType(eb);

        _o << "struct " << ebt << "{\n";
        ForEach(eb.fields(),i) {
          const EventBusField &ebf = *(i->second);
          _o << genFieldType(ebf) << " " << ebf.name() << ";\n";
        }
        _o << ebt << "()\n";
        bool first = true;
        ForEach(eb.fields(),i) {
          const EventBusField &ebf = *(i->second);
          constr_init_sep(_o,first);
          _o << ebf.name() << "(" << genFieldInit(ebf) << ")";
        }
        _o << "{}\n"
           << "};\n\n";
        
        // Finally, add code to store the index for this bus.  This is not needed
        // at the leaf (core) level.
        if (!atleaf) {
          cd._member_vars.push_back("int " + ebname);
          cd._member_vars.push_back("int " + ebid);
          cd._constr_inits.push_back(ebname + "(0)");
          cd._constr_inits.push_back(ebid + "(0)");
        }
      } else {
        // It is shared- pass it to child objects.
        cd._constr_args.push_back("int &" + ebname);
        cd._constr_args.push_back("int &" + ebid);
      }

    }

    void genParmType(const Parm &parm,const gc_string &type)
    {
      _o << "enum " << type << " {";
      bool first = true;
      ForEach(parm.options(),i) {
        list_sep(_o,first);
        _o << *i;
      }
      _o << "};\n";
    }

    // Generate data and variable info for a parameter.
    void generateParmDS(const Parm &parm,ClassData &cd,const Resources &shared,bool is_tconst)
    {
      bool is_bool = parm.is_bool();

      gc_string type = getParmType(parm);
      gc_string name  = getParmName(parm);

      if (!shared.contains(parm)) {
        // Generate an enum and a variable.  If the parameter is constant,
        // we make the variable constant.  That should allow the compiler to
        // fold out conditional statements that use the constant parameters.
        // As a special case, if the only options are "true" and "false", then we make
        // this a boolean.
        if (!is_bool && !is_tconst) {
          genParmType(parm,type);
        }

        gc_string ppfx,psfx;
        if (is_tconst) {
          // If we're generating a JIT and this is a translation-time constant
          // parm, then wrap with the appropriate macro.
          ppfx = "Dummify(";
          psfx = ")";
        }          
        if (parm.is_const()) {
          cd._member_vars.push_back( "static const " + type + " " + name + " = " + parm.value() );
        } else {
          cd._member_vars.push_back( type + " " + ppfx + name + psfx );
          if (!is_tconst) {
            cd._constr_inits.push_back(parm.name()+"("+parm.value()+")");
          }
          cd._reset_stmts.push_back(parm.name()+" = "+parm.value());
        }
      } else {
        // Shared:  Pass in a reference to the variable.
        gc_string carg = getParmArg(parm);
        cd._constr_inits.push_back(name + "(" + carg + ")" );
        if (parm.is_const()) {
          cd._member_vars.push_back( "const " + type + " &" + name );
          cd._constr_args.push_back("const " + type + " &" + carg);
        } else {
          cd._member_vars.push_back(type + " &" + name );
          cd._constr_args.push_back(type + " &" + carg);
        }
      }
    }

    // Generate data and variable info for an external resource.
    void generateExtResDS(const ExtResource &er,ClassData &cd,const Resources &shared)
    {
      const gc_string &type = er.objtype();
      const gc_string &name = er.name();

      if (!shared.contains(er)) {
        // If external, then we're just dealing with a pointer, so we just init it to 0 and create
        // a method for setting it, plus create a pointer member variable.  Otherwise, we instantiate
        // it with the listed constructor args
        if (er.external()) {
          cd._member_vars.push_back(type + " *" + name);
          cd._constr_inits.push_back(name + "(0)");
          if (er.reset()) {
            cd._reset_stmts.push_back("if (" + name + ") " + name + "->reset(init_reset,por_reset)");
          }
        } else {
          cd._member_vars.push_back(type + " " + name);
          gc_string ca = name + "(";
          bool first = true;
          ForEach(er.constr_args(),i) {
            list_sep(ca,first);
            ca += (*i)->ToString();
          }
          ca += ")";
          cd._constr_inits.push_back(ca);
          if (er.reset()) {
            cd._reset_stmts.push_back(name + ".reset(init_reset,por_reset)");
          }
        }
      } else {
        // Shared:  Pass in a reference to the variable.
        gc_string carg = name + "_p";
        cd._constr_inits.push_back(name + "(" + carg + ")" );
        if (er.external()) {
          cd._member_vars.push_back( type + " *" + name );
          cd._constr_args.push_back( type + " *" + carg);
        } else {
          cd._member_vars.push_back( type + " &" + name );
          cd._constr_args.push_back( type + " &" + carg);
        }
      }

      // Regardless of whether it's shared or not, we generate the accessors.
      if (er.external()) {
        _o << "void set_" << name << "(" << type << " *x) { " << name << " = x; };\n\n"
           << type << " *get_" << name << "() { return " << name << "; };\n\n";
      } else {
        _o << type << " &get_" << name << "() { return " << name << "; };\n\n";
      }
    }

    // Write out a setter function for setting an external resource by name.
    void generateExtResSetter(const ExtResourceHash &erh)
    {
      _o << "bool setExtResource(const std::string &name,void *res)\n"
         << "{\n";

      bool first = true;
      ForEach(erh,i) {
        const ExtResource &er = *i->second;

        if (!er.external()) continue;

        const gc_string &type = er.objtype();
        const gc_string &name = er.name();

        if (!first) {
          _o << "else ";
        }

        _o << "if (name == \"" << name << "\") {\n"
           << "set_" << name << "(reinterpret_cast<" << type << "*>(res));\n"
           << "return true;\n"
           << "}\n";

        first = false;
      }

      _o << "return false;\n"
         << "}\n\n";
    }

    // Write the virtual functions which allow an IssNode to access its
    // respective global functions.
    void writeGlobalAccessFuncs()
    {
      gc_string ns = getIssNamespace(_config);

      _o << "virtual const char *getCoreName() const { return " << ns << "::getCoreName(); }\n\n"
         << "virtual const char *getModelFileName() const { return " << ns << "::getModelFileName(); }\n\n"
         << "virtual const adl::AttrData &getAttrData() const { return " << ns << "::getAttrData(); }\n\n"
         << "virtual const char *getModelPreamble() const { return " << ns << "::getModelPreamble(); }\n\n"
         << "virtual void resetGlobal() { " << ns << "::resetGlobal(); }\n\n"
         << "virtual void setMemorySize(unsigned size) { " << ns << "::setMemorySize(size); }\n\n";
    }

  };

  gc_string getPath(const Parents &parents,const gc_string &name)
  {
    bool first = true;
    gc_string path;
    ForEach(parents,i) {
      pfx_sep(path,"_",first);
      path += i->_si->name();
    }
    if (!path.empty()) {
      path += "_";
    }
    path += name;
    return path;
  }

  // This is the main generation class, implementing most of the model
  // writing.
  class GenCore : public GenBase {

  public:
    GenCore(opfxstream &o,PfxFileArray *impls,opfxstream *jo,opfxstream *gho,const Core &core,CoreConfig *coreconfig,
            const ConfigDB &config,const gc_string &fname,StrSet *externs,StrList &end_code,
            Path &path,Parents &parents,InstrSyms &instr_syms,InstrPacketMap &packet_map) :
      GenBase(o,impls,jo,gho,config,coreconfig,fname,externs),
      _core(core),
      _coreconfig(*((coreconfig) ? coreconfig : new CoreConfig(&_core))),
      _end_code(end_code),
      _parents(parents),
      _path(path),
      _instr_syms(instr_syms),
      _walker(_core.env(),config.ld_okay(),config.library_mode(),&_core),
      _miss_handler(AdlGenSym()),
      _genip(_o,packet_map,_core,_walker,_cd,_config,_coreconfig),
      _geninstrs(_o,_impls,jo,_core,_walker,_filename,_watches,_cd,_genip,_end_code,instr_syms,
                 _path,_config,_coreconfig),
      _genexceptions(_o,_core,_walker,_filename,_watches,_cd,_config,_coreconfig),
      _genregs(_o,_core,_walker,_filename,_mmu_regs,_parents,_watches,_cd,_config,_coreconfig)
    {
    };

  private:
    const Core       &_core;
    CoreConfig       &_coreconfig;
    StrList          &_end_code;
    Parents          &_parents;
    const Path       &_path;
    InstrSyms        &_instr_syms;    

    InstrWalker       _walker;

    ClassData         _cd;

    RegBaseSet        _mmu_regs;       // Registers referenced by the MMU (if one exists).

    Ptree            *_miss_handler;   // Handler for decode misses.

    GenInstrPacketIF  _genip;          // For generating the instruction packet of a transactional ISS.

    GenInstrsIF       _geninstrs;      // For generating the instructions.

    GenExceptionsIF   _genexceptions;  // For generating exceptions.

    GenRegsIF         _genregs;        // For generating registers.

    // Stores information about parameters or other resources that have watch
    // expressions.  Contains a hash of registers/register-files that it
    // references.  If these change, then the watch expression should be called to
    // evaluate the new state of the parameter.
    Watches          _watches;

    unsigned         _max_slots;       // Maximum number of slots used in a transactional ISS.

  public:

    bool mt_support() const
    {
      return _config.mt_support();
    }

    bool library_mode() const
    {
      return _config.library_mode();
    }

    bool normal_iss() const
    {
      return _config.iss_type() == tNormal;
    }

    bool transactional_iss() const
    {
      return _config.iss_type() == tTransactional;
    }

    bool hybrid_iss() const
    {
      return _config.iss_type() == tHybrid;
    }

    bool instr_attrs() const
    {
      return _config.gen_instr_attr();
    }

    bool rollback() const
    {
      return _config.rollback_mode() || _core.post_fetch_skip();
    }

    bool ignore_mode() const
    {
      return _config.ignore_mode();
    }
 
    bool jit_support() const
    {
      return _config.jit_support();
    }

    bool dc_support() const
    {
      return _config.dc_support();
    }

    bool multi_issue() const
    {
      return (_coreconfig.issue_width() > 1);
    }

    bool logging_mem() const
    {
      return _config.mem_if() == mLogging;
    }

    int issue_width() const
    {
      return _coreconfig.issue_width();
    }

    Ptree *tracing_on() const
    {
      return _coreconfig.tracing_on();
    }

    Ptree *tracing_off() const
    {
      return _coreconfig.tracing_off();
    }

    Ptree *post_sim() const
    {
      return _coreconfig.post_sim();
    }

    Ptree *compl_time() const
    {
      return _coreconfig.compl_time();
    }

    bool have_dt() const
    {
      return _config.dep_tracking();
    }

    unsigned max_slots() const
    {
      return _max_slots;
    }

    bool multi_compile() const
    {
      return _config.num_compile_jobs() > 0;
    }

    const MMU *get_mmu() const
    {
      bool genmmu = _coreconfig.gen_mmu();
      if (genmmu) {
        return _core.mmu();
      } else {
        return 0;
      }
    }

    // Return true if this is a translation-time constant parm.  We return false
    // if the parameter is constant, because if it is already const, we'll make
    // it a static const and thus simplify the whole process.
    bool is_tconst_parm(const Parm &parm) const
    {
      return (jit_support() && !parm.is_const() && _coreconfig.trans_const_parms().count(parm.name()));
    }

    bool gen_caches() const
    {
      return _coreconfig.gen_caches();
    }

    bool dmi_cache_support() const
    {
      return _coreconfig.dmi_cache_support();
    }

    bool event_bus_send() const
    {
      return _config.event_bus_send();
    }

    bool extern_event_bus() const
    {
      return _config.extern_event_bus();
    }

    Ptree *funcBody(Environment *e,Ptree *f)
    {
      return adl::funcBody(_walker,e,f);
    }

    void genFunc(const gc_string &fname,Environment *env,Ptree *func)
    {
      adl::genFunc(_o,_config,_walker,_filename,fname,env,func);
    }

    void genWatch(Environment *env,Ptree *watch)
    {
      adl::genWatch(_o,_watches,_walker,env,watch);
    }

    gc_string getMemPointsIterator(bool is_const)
    {
      return adl::getMemPointsIterator(library_mode(),is_const);
    }

    Ptree *makeConstArgs(Ptree *func)
    {
      int ac = 0;
      Ptree *newargs = get_func_args(func);
      unsigned numargs = get_func_call_numargs(func);
      ForRange(numargs,arg) {
        Ptree *oldarg = get_func_call_arg(func,ac)->Car();
        newargs = PtreeUtil::Subst(Ptree::Make("const %p &",oldarg),
                                   oldarg,newargs);
        ++ac;
      }
      return newargs;
    }

    // Writes out a function with a given name, but modifies all non-builtin-type arguments to
    // be constant references.
    void genFuncConstArgs(const gc_string &fname,Environment *env,Ptree *func)
    {
      if (func) {
        _o << fname << "(" << makeConstArgs(func) << ")\n";
        writeLineDirective(env,func);
        _o << funcBody(env,func) << "\n";
        writeCurLocation();
        _o << "\n";
      }
    }

    // Generate an address mask watch expression.
    void genAddrMask(const AddrMask &am) 
    {
      if (Ptree *watch = am.watch()) {
        genWatch(am.env(),watch);
      }

      // Only create a modification function if the mask is non-constant.
      if (!am.is_const()) {
        gc_string mn = am.name();
        _cd._member_vars.push_back("addr_t " + mn );
        {
          gc_ostringstream ss;
          ss << mn << "(0x" << hex << am.initial_value() << "ULL)";
          _cd._constr_inits.push_back(ss.str());
        }
        {
          gc_ostringstream ss;
          ss << mn << " = 0x" << hex << am.initial_value() << "ULL";
          _cd._reset_stmts.push_back(ss.str());
        }                           
      }
    }

    // Generate a checker for core activity.
    void genActivityCheck()
    {
      if (Ptree *activity = _core.active_watch()) {
        Ptree *newa = funcBody(_core.env(),activity);
        Ptree *sym1 = AdlGenSym();
        Ptree *sym2 = AdlGenSym();
        _o << "bool " << sym1 << "() " << newa << "\n\n"
           << "void " << sym2 << "() {\n"
           << "if (" << sym1 << "()) {\n"
           << "activate();"
           << "\n} else {\n"
           << "halt();\n"
           << "}\n"
           << "}\n\n";
        _watches.push_back(Watch(_walker.regs_referenced(),sym2));
      }
    }

    // Generate architectural parameters.
    void generateParms()
    {
      const ParmHash &parms = _core.parms();

      if (!parms.empty()) {
        _o << "//\n"
           << "// Architectural parameters.\n"
           << "//\n";
        // Generate an enum and a variable.  If the parameter is constant,
        // we make the variable constant.  That should allow the compiler to
        // fold out conditional statements that use the constant parameters.
        ForEach(parms,i) {
          const Parm &parm = *(i->second);

          bool is_tconst = is_tconst_parm(parm);
          generateParmDS(parm,_cd,_core.shared(),is_tconst);

          // If this is a translation-time constant, then we need to update its
          // value with the JIT engine.  We do that by using a special update
          // function for the assignment.
          if (is_tconst) {
            _walker.add_parm_assign(parm.name());

            gc_string type  = getParmType(parm);
            gc_string name  = getParmName(parm);
            gc_string hname = getParmHandleName(parm);

            _o << "void " << getParmAssign(parm) << "(" << type << " x) {\n"
               << name << " = x;\n"
               << "_jit.set_parm_value(" << hname << ",x);\n"
               << "}\n\n";
          }

          // If we have a watch expression, we transform it here and also record
          // all register/register-files that are referenced so we know when to update
          // the parameters.
          if (Ptree *watch = parm.watch()) {
            genWatch(parm.env(),watch);
          }
        }
        _o << "\n\n";
      }

      // Handle any watch expressions here.
      genAddrMask(_core.ra_mask());
      genAddrMask(_core.ea_mask());

      // Generate the activity checker.
      genActivityCheck();
      
    }

    // Generate external resources.
    void generateExtResources()
    {
      const ExtResourceHash &extresources = _core.extresources();

      if (!extresources.empty()) {
        _o << "//\n"
           << "// External Resources.\n"
           << "//\n";
        bool has_ext = false;
        ForEach(extresources,i) {
          const ExtResource &er = *(i->second);

          generateExtResDS(er,_cd,_core.shared());

          if (er.external()) has_ext = true;
        }
        // If we have true external resources, then create a by-name setter
        // function.
        if (has_ext) {
          generateExtResSetter(extresources);
        }
        _o << "\n\n";
      }
    }

    // Generate the code which updates state due to set-register operations.
    void generateCheckWatches()
    {
      _o << "void checkWatches() {\n";
      if (!_watches.empty()) {
        _o << "// We have watch-functions.\n";
      }
      // Insert any parameter watch calls here so that the model
      // will be consistant.
      ForEach(_watches,i) {
        _o << i->_watchfunc << "();\n";
      }
      _o << "}\n\n";
    }

    // Generate the declarations, accessors of prefix fields
    void generatePrefixDS()
    {
      if (_core.parallel_execution()) { // Prefixes are supported only in parallel architectures
        genPrefixFieldCtors(_core,_cd._constr_inits);
	
        gc_ostringstream ss;
        genPrefixFieldDeclarations(_o,ss,_core,"",_core.name());
        _cd._member_vars.push_back(ss.str());
        genPrefixFieldGetters(_o,_core,"inline static",&_walker,true,true);
        genPrefixFieldSetters(_o,_core,"inline");
        genPrefixFieldClearers(_o,_core,"inline");
        genPrefixCounters(_o,_core,_cd);
        _cd._reset_stmts.push_back("reset_prefix();");
      }
    }

    // Generate an event bus.
    void genEventBus(const EventBus &eb,PMap &eb_handlers)
    {
      gc_string ebtype = eb.name() + "Handler";
      gc_string ebname = eb.name();
      gc_string ebt    = getEventBusDataType(eb);
      gc_string ebparm = getEventBusIndexName(eb);
      gc_string ebid   = getEventBusIdName(eb);

      Ptree *sym = AdlGenSym();
      eb_handlers[eb.name()] = sym;
      gc_string fn = sym->ToString();

      gc_string ctype = _core.name();

      genEventBusDS(eb,_core.shared(),_cd,true);

      bool shared = _core.is_shared(eb);

      // Then generate the handler function.  We want to convert the argument
      // to a constant reference for performance reasons.
      genFuncConstArgs("void " + fn,_core.env(),eb.action()._func);

      gc_string r_parm, r_arg;
      if (extern_event_bus()) {
        r_parm = "EventBusExtHandler *eh";
        r_arg = ",sizeof(" + ebt + ")";
      }

      // Now we create the functor object which is
      // registered with the bus.  This calls the handler function.
      _o << "struct " << ebtype << " : public EventBusHandler {\n"
         << ctype << " &_core;\n"
         << "int " << ( (shared) ? "&" : "") << "_handle;\n\n"
         << "int " << ( (shared) ? "&" : "") << "_id;\n\n"
         << ebtype << "(" << ctype << " &c" << ((shared) ? ",int &handle,int &id" : "") << ") : _core(c), "
         << "_handle(" << ((shared) ? "handle" : "0") << "), _id(" << ((shared) ? "id" : "0") << ") {}\n"
         << "void registerbus(" << r_parm << ") {\n";
      if (extern_event_bus()) {
        _o << "if (eh) {\n"
           << "eh->";
      }
      _o << "register_eventbus(_handle,_id,\"" << eb.name() << "\",\"" << eb.signature() << "\",*this);\n";
      if (extern_event_bus()) {
        _o << "}\n";
      }
      _o << "}\n"
         << "void operator()(const void *data,int id) {\n"
         << "if (id != _id) {\n";
      if (mt_support()) {
        _o << "boost::lock_guard<" << getEventBusMutexType() << "> lock(_core." << getEventBusMutexName() << ");\n";
      }
      _o << "if (Tracing && _core.trace_mode()) {\n"
         << "_core.logger().log_core_change();\n"
         << "}\n"
         << "_core." << fn << "(*((const " << ebt << " *)data));\n";
      if (!_core.contexts().empty()) {
        _o << "_core." << getCtxUpdateName() << "();\n";
      }
      _o << "}\n"
         << "};\n"
         << "void send(const " << ebt << "&data,bool signal_self = false) {\n";
      if (event_bus_send()) {
        // Normal operation: Data is broadcast to all other cores automatically.
        if (extern_event_bus()) {
          _o << "_core." << getEbHandler() << "->";
        }
        _o << "signal_eventbus(_handle,(signal_self) ? -1 : _id,&data" << r_arg << ");\n";
      } else {
        // Otherwise- send to a registered functor.
        _o << "if (signal_self) { operator()(&data,_id); }\n"
           << "_core." + getEventBusRecvName() + "->operator()(_core,data);\n";
      }
      _o << "if (Tracing && _core.trace_mode()) {\n"
         << "_core.logger().log_core_change();\n"
         << "}\n"
         << "};\n"
         << "};\n\n";

      _cd._member_vars.push_back(ebtype + " " + eb.name());
   
      if (shared) {
        _cd._constr_inits.push_back(eb.name()+"(*this,"+ebparm+","+ebid+")");
      } else {
        // Finally, add code to register this bus.
        _cd._constr_inits.push_back(eb.name() + "(*this)");
        if (!extern_event_bus()) {
          _cd._constr_stmts.push_back(eb.name()+".registerbus()");
        }
      }

    }

    // Generate external-user event bus send/receive functions.  This is only
    // done if the --no-event-bus-send option is set during generation.
    void generateEventBusHelpers(PMap &eb_handlers)
    {
      if (!event_bus_send()) {
        // First, create a wrapper class that will do the sending.  Each event bus
        // will have a specialization for the event bus type.  Note: Because we're
        // in a class scope, we can't do a full specialization (reason: C++ is a
        // giant, complicated piece of junk).  So, we have to stick in a dummy
        // template parameter that won't be specialized.
        _o << "template <size_t Dummy,class T>\n"
           << "struct EventBusSender {};\n\n";

        ForEach(_core.eventbuses(),i) {
          const EventBus &eb = *i->second;

          gc_string ebt = getEventBusDataType(eb);
          Ptree *ebh = eb_handlers[eb.name()];

          _o << "template <size_t Dummy>\n"
             << "struct EventBusSender<Dummy," << ebt << "> {\n"
             << "EventBusSender(P& core) : _core(core) {};\n"
             << "void operator()(const " << ebt << " &data) { _core." << ebh << "(data); }\n"
             << "P &_core;\n"
             << "};\n\n";
        }

        // Then create the member method which will call this stuff.
        _o << "// To be called by an external user to send event-bus data to the core.\n"
           << "template <class T>\n"
           << "void event_bus_send(const T &t)\n"
           << "{\n"
           << "EventBusSender<0,T> sender(*this);\n"
           << "if (Tracing && trace_mode()) {\n"
           << "logger().log_core_change();\n"
           << "}\n"
           << "sender(t);\n"
           << "check_context_updates();\n"
           << "}\n\n";

        // Then install the logic to send the event to a registered functor.
        _cd._member_vars.push_back(getEventBusRecvType() + " *" + getEventBusRecvName());
        _cd._constr_inits.push_back(getEventBusRecvName() + "(0)");

        _o << "struct EventBusReceiver {\n"
           << "virtual ~EventBusReceiver() {};\n";
        ForEach(_core.eventbuses(),i) {
          const EventBus &eb = *i->second;
          gc_string ebt = getEventBusDataType(eb);

          _o << "virtual void operator()(" << _core.name() << " &core,const " << ebt << " &data) {};\n";
        }
        _o << "};\n\n"
           << "void setEventBusReceiver(" << getEventBusRecvType() << "* x)\n"
           << "{\n"
           << getEventBusRecvName() << " = x;\n"
           << "}\n\n";
      } else if (extern_event_bus()) {
        // If we have an external handler, generate the appropriate support
        // functions.
        gc_string ebt = "EventBusExtHandler";
        gc_string ebh = getEbHandler();

        _cd._member_vars.push_back(ebt + " *" + ebh);
        _cd._constr_inits.push_back(ebh + "(0)");

        _o << "bool hasExternalEventBuses() const { return true; }\n\n"

           << "void setEventBusHandler(" << ebt << " *e) {\n" 
           << ebh << " = e;\n";
        ForEach(_core.eventbuses(),i) {
          const EventBus &eb = *i->second;
          _o << eb.name() << ".registerbus(e);\n";
        }
        _o << "}\n\n"

           << ebt << " *getEventBusHandler() const { return " << ebh << "; }\n\n";
      }
    }

    // Mark as atomic any instruction which accesses a register resource also
    // used by an event bus.
    void findAtomicInstrs()
    {
      UsedRegs eb_regs;

      // Get all register resources used by the event buses.
      ForEach(_core.eventbuses(),i) {
        eb_regs.add(i->second->action()._src_regs);
        eb_regs.add(i->second->action()._trg_regs);
      }

      ForEach(_core.instrs(),i) {
        const Instr &instr = *i->second;
        bool uses_eb_res = false;
        ForEach (instr.usageinfo()._src_regs,j) {
          if (eb_regs.count(*j)) uses_eb_res = true;
        }
        ForEach (instr.usageinfo()._trg_regs,j) {
          if (eb_regs.count(*j)) uses_eb_res = true;
        }
        if (uses_eb_res) {
          InstrSym &sym  = _instr_syms.find(instr.name())->second;
          sym._uses_eb_res = true;
        }
      }
    }

    // Generate any event buses.
    void generateEventBuses()
    {
      PMap eb_handlers;

      // If we're adding MT support, then add on the mutex used to guard
      // accesses across cores.
      if (mt_support()) {
        _cd._member_vars.push_back(getEventBusMutexType() + " " + getEventBusMutexName());
      }

      // We also need to figure out the resources used by the event buses.  Any
      // instrutions which also use those resources are added to a list of
      // instructions which must be atomic for MT support.
      findAtomicInstrs();

      ForEach(_core.eventbuses(),i) {
        // Generate the data structures.
        genEventBus(*(i->second),eb_handlers);      
      }

      // This generates the support logic, which allows extern users to
      // send/receive eventbus data.
      generateEventBusHelpers(eb_handlers);
    }

    // Writes the DMI reserve functions for reads or writes.
    void genDmiReservedFuncs(const gc_string &type)
    {
      _o << "void clear_dmi_" << type << "_reserved() {\n"
         << "_dmi_" << type << "_reserves.clear();\n"
         << "}\n\n"

         << "bool " << getDmiReservedCheck(type) << "(addr_t ea) {\n"
         << "return _dmi_" << type << "_reserves.count(ea & DmiCache::CompMask);\n"
         << "}\n\n"

         << "void add_dmi_" << type << "_reserved(addr_t ea) {\n"
         << "_dmi_" << type << "_reserves[ea & DmiCache::CompMask]++;\n"
         << "}\n\n"

         << "void clear_dmi_" << type << "_reserved(addr_t ea) {\n"
         << "Reserves::iterator i = _dmi_" << type << "_reserves.find(ea & DmiCache::CompMask);\n"
         << "if (i != _dmi_" << type << "_reserves.end()) {\n"
         << "i->second--;\n"
         << "if (i->second == 0) {\n"
         << "_dmi_" << type << "_reserves.erase(i);\n"
         << "}\n"
         << "}\n"
         << "}\n\n";
    }

    // Generate the data structures and support functions for checking whether
    // memory is reserved wrt. the dmi cache.  We reserve memory for reads and
    // writes separately.  Each entry's key is an address aligned with the DMI
    // cache page size and the value is a count.  We remove entries when the
    // count decrements to 0 and we increment the count each time we add an
    // address.  This way, we can track when there are multiple watchpoints for
    // the same entry.
    void genDmiReservedSupport()
    {
      if (!dmi_cache_support()) {
        return;
      }

      _o << "typedef _STDEXT hash_map<addr_t,unsigned> Reserves;\n\n";

      _cd._member_vars.push_back("Reserves _dmi_read_reserves");
      _cd._member_vars.push_back("Reserves _dmi_write_reserves");

      genDmiReservedFuncs("read");
      genDmiReservedFuncs("write");
    }

    // Miscellaneous code to check when removing a watchpoint or breakpoint.
    void genClearPointChecks()
    {
      if (jit_support()) {
        _o << "if (is_bp) {\n"
           << "removeJitBreakpoint(iter->first);\n"
           << "}\n";
      }
      if (dc_support()) {
        _o << "if (is_bp) {\n"
           << "removeDcBreakpoint(iter->first);\n"
           << "}\n";
      }
      if (dmi_cache_support()) {
        _o << "if (!is_bp) {\n"
           << "if (is_read) {\n"
           << "clear_dmi_read_reserved(iter->first);\n"
           << "} else {\n"
           << "clear_dmi_write_reserved(iter->first);\n"
           << "}\n"
           << "}\n";
      }
    }

    // Generate debug data structures and support functions, if the mode is enabled.
    void generateDebugSupport()
    {
      if (_config.debug_mode()) {
        _o << "struct MemPoint {\n"
           << "unsigned _handle;\n"
           << "bool     _is_tmp;\n"
           << "PtBase  *_payload;\n"
           << "MemPoint() : _handle(0), _is_tmp(false), _payload(0) {};\n"
           << "MemPoint(unsigned h,bool t,PtBase *p) : _handle(h), _is_tmp(t), _payload(p) {}\n"
           << "};\n\n"
           << "typedef _STDEXT hash_map<addr_t,MemPoint> MemPoints;\n\n";

        _cd._member_vars.push_back("MemPoints _breakpoints");
        _cd._member_vars.push_back("MemPoints _read_watchpoints");
        _cd._member_vars.push_back("MemPoints _write_watchpoints");

        _cd._member_vars.push_back("unsigned _bp_counter");
        _cd._member_vars.push_back("unsigned _wp_counter");

        _cd._constr_inits.push_back("_bp_counter(0)");
        _cd._constr_inits.push_back("_wp_counter(0)");

        genDmiReservedSupport();

        gc_string mt = getMemPointsIterator(false);

        _o << "void issueBreakpoint(addr_t ea," << mt <<" b_iter) {\n"
           << "MemPoint tmp = b_iter->second;\n"
           << "if (tmp._is_tmp) { _breakpoints.erase(b_iter); }\n"
           << "_logger->log_breakpoint(ea,tmp._handle,tmp._payload);\n"
           << "}\n\n"

           << "#ifndef ADL_HEADER\n\n"

           << "void clearBreakpoints() {\n"
           << "_breakpoints.clear();\n"
           << "_bp_counter = 0;\n"
           << "};\n\n";

        _o << "void clearWatchpoints() {\n"
           << "_read_watchpoints.clear();\n"
           << "_write_watchpoints.clear();\n";
        if (dmi_cache_support()) {
          _o << "clear_dmi_read_reserved();\n"
             << "clear_dmi_write_reserved();\n";
        }
        _o << "_wp_counter = 0;\n"
           << "};\n\n";

        _o << "unsigned setBreakpointInternal(addr_t a,bool is_tmp,PtBase *payload) {\n"
           << "unsigned index = _bp_counter++;\n"
           << "_breakpoints[a] = MemPoint(index,is_tmp,payload);\n";
        // In JIT mode, we invalidate any BBs containing the new breakpoint
        // address, so that we'll re-translate with a breakpoint inserted.
        if (jit_support()) {
          _o << "addJitBreakpoint(a);\n";
        }
        if (dc_support()) {
          _o << "addDcBreakpoint(a);\n";
        }
        _o << "return index;\n"
           << "}\n\n"

           << "bool clearPointsByAddr(addr_t addr, MemPoints &points,bool is_bp,bool is_read) {\n"
           << mt << " iter;\n"
           << "if ((iter = points.find(addr)) != points.end()) {\n";
        genClearPointChecks();
        _o << "points.erase(iter);\n"
           << "return true;\n"
           << "}\n"
           << "return false;\n"
           << "}\n\n"

           << "bool clearPoint(MemPoints &points,unsigned h,bool is_bp,bool is_read) {\n"
           << "for (" << mt << " iter = points.begin(); iter != points.end(); ) {\n"
           << "if (iter->second._handle == h) {\n";
        genClearPointChecks();
        _o << "points.erase(iter);\n"
           << "return true;\n"
           << "} else {\n"
           << "++iter;\n"
           << "}\n"
           << "}\n"
           << "return false;\n"
           << "}\n\n"

           << "#endif // ADL_HEADER\n\n"

           << "unsigned setBreakpoint(addr_t a,PtBase *payload = 0)"
           << headerOnly2()
           << "{\n"
           << "return setBreakpointInternal(a,false,payload);\n"
           << "}\n"
           << endHeaderOnly()

           << "unsigned setTmpBreakpoint(addr_t a,PtBase *payload = 0)"
           << headerOnly2()
           << "{\n"
           << "return setBreakpointInternal(a,true,payload);\n"
           << "}\n"
           << endHeaderOnly()

           << "bool clearBreakpointByAddr(addr_t addr)"
           << headerOnly2()
           << "{\n"
           << "return clearPointsByAddr(addr, _breakpoints,true,false);\n"
           << "}\n"
           << endHeaderOnly()

           << "bool clearWatchpointByAddr(addr_t addr,WatchType type)"
           << headerOnly2()
           << "{\n"
           << "switch (type) {\n"
           << "case wpRead:\n"
           << "return clearPointsByAddr(addr,_read_watchpoints,false,true);\n"
           << "case wpWrite:\n"
           << "return clearPointsByAddr(addr,_write_watchpoints,false,false);\n"
           << "case wpAccess: {\n"
           << "bool r = clearPointsByAddr(addr,_read_watchpoints,false,true);\n"
           << "bool w = clearPointsByAddr(addr,_write_watchpoints,false,false);\n"
           << "return r || w;\n"
           << "}\n"
           << "default:\n"
           << "return false;\n"
           << "}\n"
           << "}\n"
           << endHeaderOnly()

           << "unsigned setWatchpoint(addr_t a,WatchType type,PtBase *payload = 0)"
           << headerOnly2()
           << "{\n"
           << "WatchpointsEnabled = true;\n"
           << "unsigned index = _wp_counter++;\n"
           << "if (type & wpRead) {\n"
           << "_read_watchpoints[a & ~WordMask] = MemPoint(index,false,payload);\n";
        if (dmi_cache_support()) {
          _o << "add_dmi_read_reserved(a);\n";
        }
        _o << "}\n"
           << "if (type & wpWrite) {\n"
           << "_write_watchpoints[a & ~WordMask] = MemPoint(index,false,payload);\n";
        if (dmi_cache_support()) {
          _o << "add_dmi_write_reserved(a);\n";
        }
        _o << "}\n"
           << "return index;\n"
           << "}\n"
           << endHeaderOnly()

           << "bool clearBreakpoint(unsigned h)"
           << headerOnly2()
           << "{\n"
           << "return clearPoint(_breakpoints,h,true,false);\n"
           << "}\n"
           << endHeaderOnly()

           << "bool clearWatchpoint(unsigned h)"
           << headerOnly2()
           << "{\n"
           << "bool rw = clearPoint(_read_watchpoints,h,false,true);\n"
           << "bool ww = clearPoint(_write_watchpoints,h,false,false);\n"
           << "return (rw || ww);\n"
           << "}\n"
           << endHeaderOnly();
      }
    }

    // Generate dependency-tracking support code.
    void genDepTracking()
    {
      if (have_dt()) {
        _o << "bool has_dependency_tracking() const { return true; }\n\n"
           << "void " << getDepReset() << "()\n"
           << "{\n"
           << getDepVar() << ".clear();\n"
           << getRegSeqVar() << " = 0;\n"
           << getMemSeqVar() << " = 0;\n"          
           << "}\n\n"

           << "const DepItems *get_last_dependencies() const { return &" << getDepVar() << ".last_hit(); };\n\n"

           << "unsigned get_reg_seq_num() const { return " << getRegSeqVar() << "; };\n\n"
           << "unsigned get_mem_seq_num() const { return " << getMemSeqVar() << "; };\n\n";
        
        _cd._member_vars.push_back("DepTracking " + getDepVar());

        _cd._member_vars.push_back("unsigned " + getRegSeqVar());
        _cd._constr_inits.push_back(getRegSeqVar() + "(0)");

        _cd._member_vars.push_back("unsigned " + getMemSeqVar());
        _cd._constr_inits.push_back(getMemSeqVar() + "(0)");

        _cd._reset_stmts.push_back("resetDepTracking()");
      }
    }

    // Create the accessor/setup functions here.
    void generateSetupFuncs(bool top)
    {
      // For communicating with an outer performance model.
      if (normal_iss()) {
        // Just a stub, since this is a normal ISS.
        _o << "void setExtInstrFlag(unsigned) {};\n\n"
           << "unsigned getExtInstrFlag() { return 0; };\n\n";
      } else {
        // Create a virtual function, since an application may derive from this
        // ISS.
        _o << "virtual void setExtInstrFlag(unsigned) {};\n\n"
           << "virtual unsigned getExtInstrFlag() { return 0; };\n\n";
      }

      // This is here so that a performance model's fetch unit can be
      // synchronized upon activation of a core or thread.
      if (!normal_iss()) {
        // Just a stub for the default.
        _o << "// This must be implemented by the performance model for handling fetch-unit synchronization.\n"
           << "void synchronizeActivation();\n\n";
      }

      _o << "inline void halt()     {\n";
      _geninstrs.generateHalt();
      _o << "set_active_count(0);\n"
         << "};\n\n"
         << "inline void activate() { if (!_active) {\n"
         << "_active = true;\n";
      if (!normal_iss()) {
        _o << "synchronizeActivation();\n";
      }
      _o << "}\n"
         << "};\n\n"
         << "inline bool active() const { return _active; };\n\n";
      _cd._reset_stmts.push_back("_active = true");

      _cd._reset_stmts.push_back(_coreconfig.core_base() + "::reset()");

      // Defined in the core's additional base class.
      _o << "virtual uint64_t instr_count() const { return _instr_count; };\n\n"
         << "virtual void decr_instr_count(uint64_t d) { _instr_count -= d; };\n\n"

         << "virtual bool isBigEndian() const { return " << (_core.dataEndianness() == Core::Big) << "; };\n\n";
      
      // Write the reset method.
      _o << "void reset_internal(bool init_reset,bool por_reset,bool log)\n"
         << "{\n";
      _cd.writeResetStmts(_o);
      _o << "checkWatches();\n"
         << "};\n\n"
         << "void reset () {\n"
         << "uint64_t of = clear_tracing();\n"
         << "reset_internal(false,true,false);\n"
         << "set_tracing(of);\n"
         << "}\n\n"
         << "void resetCore() {\n"
         << "reset_internal(false,false,true);\n"
         << "}\n\n"
         << "void resetInit() {\n"
         << "reset_internal(true,true,false);\n"
         << "}\n\n";

      // If we have any dynamic parameters, then create the top-level get/set/list routines here.
      if (!_cd._dyn_parms.empty()) {

        _o << "void set_dyn_parm(const std::string &parm,unsigned value)\n"
           << "{\n";
        bool first = true;
        ForEach(_cd._dyn_parms,i) {
          gc_string set = getSetParmName(*i);
          cond_sep(_o,first);
          _o << "(" << set << "(parm,value)) {\n"
             << ";\n"
             << "}\n";
        }
        _o << "else {\n"
           << "IssCore::set_dyn_parm(parm,value);\n"
           << "}\n"
           << "}\n\n"

           << "unsigned get_dyn_parm(const std::string &parm) const\n"
           << "{\n"
           << "unsigned value;\n";
        first = true;
        ForEach(_cd._dyn_parms,i) {
          gc_string get = getGetParmName(*i);
          cond_sep(_o,first);
          _o << "(" << get << "(value,parm)) {\n"
             << "return value;\n"
             << "}\n";
        }
        _o << "else {\n"
           << "return IssCore::get_dyn_parm(parm);\n"
           << "}\n"
           << "}\n\n"

           << "void list_dyn_parm(StrPairs &parms) const\n"
           << "{\n";
        ForEach(_cd._dyn_parms,i) {
          gc_string lst = getListParmName(*i);
          _o << lst << "(parms);\n";
        }
        _o << "}\n\n";

      }

      genDepTracking();

      // Add on any extra arguments from an external client.
      _cd._member_vars.insert(_cd._member_vars.end(),_coreconfig.extra_members().begin(),_coreconfig.extra_members().end());
      _cd._constr_inits.insert(_cd._constr_inits.end(),_coreconfig.extra_inits().begin(),_coreconfig.extra_inits().end());

      // List member variables.
      _cd.writeMemberVars(_o);
      _o << "\n";

      bool extra_args = !_cd._constr_args.empty();

      // Write the constructor for the class.  If we have no extra args, then
      // child_id can be optional.
      _cd._constr_args.push_front(gc_string("unsigned child_id") + ((extra_args) ? "" : " = 0"));
      _cd._constr_args.push_front("unsigned &id");
      _cd._constr_args.push_front("const IssNode *parent");
      _cd._constr_args.push_front("const std::string &__n");
      _cd._constr_inits.push_front("IssCore(__n,\""+getSysItemType(_core)+"\",parent,id,child_id)");

      _o << _core.name();
      _cd.writeConstrArgs(_o);
      _cd.writeConstrInits(_o);
      _o << "\n{\n";
      if (top) {
        // Only call reset if this is the top-level item, in order to ensure
        // that all references to everything in the hierarchy is setup first.
        _o << "resetInit();\n";
      }
      _cd.writeConstrStmts(_o);
      _o << "};\n\n";

      _o << '~' << _core.name() << "()\n"
         << "{\n";
      _cd.writeDestrStmts(_o);
      _o << "}\n\n";
    }

    // Returns true if any of the MMU registers are contained within
    // the specified context.
    bool mmuHasContextRegs(const Context &ctx)
    {
      ForEach(_mmu_regs,iter) {
        if (ctx.contains(**iter)) {
          return true;
        }
      }
      return false;
    }

    void genContextSupportFuncs(const ContextHash &contexts,IntMap &ctx_sizes)
    {
      gc_string ctxupdate = getContextUpdateVar();

      // This is the main signal for whether a context-change event has occurred.
      _cd._member_vars.push_back("bool "+ctxupdate);
      _cd._constr_inits.push_back(ctxupdate+"(false)");

      _o << "\n"
         << "CtxInfos initCtxInfo() const {\n"
         << "CtxInfos tmp;\n";
      ForEach(contexts,citer) {
        const Context &ctx = *(citer->second);
        _o << "tmp.push_back(CtxInfo());\n"
           << "tmp.back()._id = " << ctx.id() << ";\n"
           << "tmp.back()._name = \"" << ctx.name() << "\";\n"
           << "tmp.back()._num_contexts = " << ctx.num_contexts() << ";\n"
           << "tmp.back()._is_thread = " << ctx.is_thread() << ";\n"
           << "tmp.back()._size = " << ctx_sizes[ctx.name()] << ";\n";
        const AttrList &attrs = ctx.attrs();
        ForEach(attrs,i) {
          AttrsHelper ah(&i->second);
          ah.gen_var(_o);
          _o << "tmp.back()._attrs.push_back(std::make_pair(" 
             << i->first << "," << ah.gen_constr() << "));\n";
        }
        _o << "tmp.back().finalize();\n";
      }
      _o << "sort_info(tmp);\n"
         << "return tmp;\n"
         << "}\n\n"
         << "const CtxInfos &getContextInfo() const {\n"
         << "static CtxInfos info = initCtxInfo();\n"
         << "return info;\n"
         << "};\n\n";

      bool first = true;
      _o << "unsigned getCtxId(const std::string &name) const {\n";
      ForEach(contexts,citer) {
        const Context &ctx = *(citer->second);
        gc_string cdc = getContextDbgControl(ctx);
        cond_sep(_o,first);
        _o << "(name == \"" << ctx.name() << "\") {\n"
           << "return " << ctx.id() << ";\n"
           << "}\n";
      }
      _o << "else {\n"
         << "return IssNode::getCtxId(name);\n"
         << "}\n"
         << "}\n\n"

         << "unsigned setDbgCtx(unsigned id,unsigned cn) {\n";
      first = true;
      ForEach(contexts,citer) {
        const Context &ctx = *(citer->second);
        gc_string cdc = getContextDbgControl(ctx);
        cond_sep(_o,first);
        _o << "(id == " << ctx.id() << ") {\n"
           << "unsigned old = " << cdc << ";\n"
           << cdc << " = cn;\n"
           << "return old;\n"
           << "}\n";
      }
      _o << "else {\n"
         << "return IssNode::setDbgCtx(id,cn);\n"
         << "}\n"
         << "}\n\n"

         << "void setActiveCtxToDbg(unsigned id) {\n";
      first = true;
      ForEach(contexts,citer) {
        const Context &ctx = *(citer->second);
        gc_string cd  = getContextControl(ctx);
        gc_string cdc = getContextDbgControl(ctx);
        cond_sep(_o,first);
        _o << "(id == " << ctx.id() << ") {\n"
           << cd << " = " << cdc << ";\n";
        if (mmuHasContextRegs(ctx)) {
          _o << "clear_mmu_cache();\n";
        }
        _o << "}\n";
      }
      _o << "else {\n"
         << "return IssNode::setActiveCtxToDbg(id);\n"
         << "}\n"
         << "}\n\n"

         << "void restoreActiveCtx(unsigned id) {\n";
      first = true;
      ForEach(contexts,citer) {
        const Context &ctx = *(citer->second);
        cond_sep(_o,first);
        _o << "(id == " << ctx.id() << ") {\n"
           << getCtxControlFunc(ctx) << "();\n";
        if (mmuHasContextRegs(ctx)) {
          _o << "clear_mmu_cache();\n";
        }
        _o << "}\n";
      }
      _o << "else {\n"
         << "return IssNode::restoreActiveCtx(id);\n"
         << "}\n"
         << "}\n\n";
      
      // This is the non-virtual version for use within the model.
      first = true;
      _o << "unsigned activeContext(unsigned id) const {\n";
      ForEach(contexts,citer) {
        const Context &ctx = *(citer->second);
        cond_sep(_o,first);
        _o << "(id == " << ctx.id() << ") {\n"
           << "return " << getContextControl(ctx) << ";\n"
           << "}\n";
      }
      _o << "else {\n"
         << "RError(\"Unknown context ID \" << id << \".\");\n"
         << "}\n"
         << "}\n\n"

        // This is a virtual version for use by external users.
         << "unsigned getActiveCtx(unsigned id) const {\n"
         << "return activeContext(id);\n"
         << "}\n\n";

      first = true;
      _o << "void writeContext(unsigned id,unsigned index,addr_t addr) {\n";
      ForEach(contexts,citer) {
        const Context &ctx = *(citer->second);
        cond_sep(_o,first);
        _o << "(id == " << ctx.id() << ") {\n"
           << getContextObjName(ctx) << ".write(index,addr);\n"
           << "}\n";
      }
      _o << "else {\n"
         << "RError(\"Unknown context ID \" << id << \".\");\n"
         << "}\n"
         << "}\n\n";

      first = true;
      _o << "void readContext(unsigned id,unsigned index,addr_t addr) {\n";
      ForEach(contexts,citer) {
        const Context &ctx = *(citer->second);
        cond_sep(_o,first);
        _o << "(id == " << ctx.id() << ") {\n"
           << getContextObjName(ctx) << ".read(index,addr);\n"
           << "}\n";
      }
      _o << "else {\n"
         << "RError(\"Unknown context ID \" << id << \".\");\n"
         << "}\n"
         << "}\n\n";

      first = true;
      _o << "void clearContext(unsigned id,unsigned index) {\n";
      ForEach(contexts,citer) {
        const Context &ctx = *(citer->second);
        cond_sep(_o,first);
        _o << "(id == " << ctx.id() << ") {\n"
           << getContextObjName(ctx) << ".clear(index);\n"
           << "}\n";
      }
      _o << "else {\n"
         << "RError(\"Unknown context ID \" << id << \".\");\n"
         << "}\n"
         << "}\n\n";     
    }

    // Writes functions for writing/reading a context from memory using the
    // core's memory read/write functions.  We're only supporting the global
    // memory for now.
    void writeCtxLoadStoreClear(const Context &ctx,const RegList &regs,const RegFileList &regfiles,IntMap &ctx_sizes)
    {       
      gc_string ctx_ctrl = getContextControl(ctx);

      _o << "void read(unsigned index,addr_t addr) {\n"
         << "assert(index < num_contexts());\n";

      if (logging_mem()) {
        _o << "_core.enable_mem_logging(false);\n"
           << "try {\n";
      }

      StrList res;
      if (!ctx.mem_layout().empty()) {
        res = ctx.mem_layout();
      } else {
        ForEach(regs,i)     res.push_back(i->_reg->name());
        ForEach(regfiles,i) res.push_back(i->_reg->name());
        ForEach(ctx._mmu,i) res.push_back(i->first);
      }

      // Remove any items that were omitted, if they're in the list.
      auto riter = res.begin();
      while (riter != res.end()) {
        if (ctx.mem_omit().count(*riter)) {
          auto tmp = riter++;
          res.erase(tmp);
          continue;
        }
        ++riter;
      }

      unsigned size = 0;

      ForEach(res,i) {
        if (ctx._regfiles.count(*i)) {
          const RegFile &rf = *_core.get_regfile(*i);
          if (rf.pseudo()) continue;
          _o << "for (unsigned i = 0; i != " << rf.size() << "; ++i) {\n"
             << "_core." << rf.name() << "_ctx_write(index,i,_core." << getMemoryName(MemoryName) << "_read<" << rf.width()/8 << ">(addr));\n"
             << "addr += " << rf.width()/8 << ";\n"
             << "}\n";
          size += (rf.width()/8) * rf.size();
        } else if (ctx._regs.count(*i)) {
          const Reg &reg = *_core.get_reg(*i);
          if (reg.pseudo()) continue;
          _o << "_core." << reg.name() << "_ctx_write(index,_core." << getMemoryName(MemoryName) << "_read<" << reg.width()/8 << ">(addr));\n"
             << "addr += " << reg.width()/8 << ";\n";
          size += reg.width()/8;
        } else {
          const MmuLookup &l = *_core.get_mmulookup(*i);
          _o << "unsigned octx = _core." << ctx_ctrl << ";\n"
             << "_core." << ctx_ctrl << " = index;\n"
             << "addr = _core." << l.name() << "_read(addr);\n"
             << "_core." << ctx_ctrl << " = octx;\n";
          size += l.mem_size();
        }
      }

      ctx_sizes[ctx.name()] = size;

      if (logging_mem()) {
        _o << "}\n"
           << "catch (...) {\n"
           << "_core.enable_mem_logging(true);\n"
           << "throw;\n"
           << "}\n"
           << "_core.enable_mem_logging(true);\n";
      }

      _o << "}\n\n"

         << "void write(unsigned index,addr_t addr) {\n"
         << "assert(index < num_contexts());\n";

      if (logging_mem()) {
        _o << "_core.enable_mem_logging(false);\n"
           << "try {\n";
      }

      ForEach(res,i) {
        if (ctx._regfiles.count(*i)) {
          const RegFile &rf = *_core.get_regfile(*i);
          if (rf.pseudo()) continue;
          _o << "for (unsigned i = 0; i != " << rf.size() << "; ++i) {\n"
             << "_core." << getMemoryName(MemoryName) << "_write<" << rf.width()/8 << ">(addr,_core." << rf.name() << "_ctx_read(index,i));\n"
             << "addr += " << rf.width()/8 << ";\n"
             << "}\n";
        } else if (ctx._regs.count(*i)) {
          const Reg &reg = *_core.get_reg(*i);
          if (reg.pseudo()) continue;
          _o << "_core." << getMemoryName(MemoryName) << "_write<" << reg.width()/8 << ">(addr,_core." << reg.name() << "_ctx_read(index));\n"
             << "addr += " << reg.width()/8 << ";\n";
        } else {
          const MmuLookup &l = *_core.get_mmulookup(*i);
          _o << "unsigned octx = _core." << ctx_ctrl << ";\n"
             << "_core." << ctx_ctrl << " = index;\n"
             << "addr = _core." << l.name() << "_write(addr);\n"
             << "_core." << ctx_ctrl << " = octx;\n";
        }
      }

      if (logging_mem()) {
        _o << "}\n"
           << "catch (...) {\n"
           << "_core.enable_mem_logging(true);\n"
           << "throw;\n"
           << "}\n"
           << "_core.enable_mem_logging(true);\n";
      }

      _o << "}\n\n"

         << "void clear(unsigned index) {\n"
         << "assert(index < num_contexts());\n";

      if (logging_mem()) {
        _o << "_core.enable_mem_logging(false);\n"
           << "try {\n";
      }

      ForEach(res,i) {
        if (ctx._regfiles.count(*i)) {
          const RegFile &rf = *_core.get_regfile(*i);
          if (rf.pseudo()) continue;
          _o << "for (unsigned i = 0; i != " << rf.size() << "; ++i) {\n"
             << "_core." << rf.name() << "_ctx_write(index,i,0);\n"
             << "}\n";
        } else if (ctx._regs.count(*i)) {
          const Reg &reg = *_core.get_reg(*i);
          if (reg.pseudo()) continue;
          _o << "_core." << reg.name() << "_ctx_write(index,0);\n";
        } else {
          const MmuLookup &l = *_core.get_mmulookup(*i);
          _o << "_core." << l.name() << "_reset(false,index);\n";
        }
      }

      if (logging_mem()) {
        _o << "}\n"
           << "catch (...) {\n"
           << "_core.enable_mem_logging(true);\n"
           << "throw;\n"
           << "}\n"
           << "_core.enable_mem_logging(true);\n";
      }

      _o << "}\n\n";
    }

    // Generates the contexts data structures and auxiliary functions.
    void genContexts(RegCtxMap &regdata)
    {
      auto contexts = _core.contexts();

      gc_string ctxupdate = getContextUpdateVar();

      gc_string coretype = getCoreTypeName(_core);

      IntMap ctx_sizes;

      ForEach (contexts,citer) {
        const Context &ctx = *(citer->second);

        gc_string cname   = citer->first;
        gc_string ctype   = getContextTypeName(ctx);
        gc_string coname  = getContextObjName(ctx);
        gc_string cotype  = getContextObjType(ctx);
        gc_string ctxctrl = getContextControl(ctx,false); 
        gc_string ctxdbg  = getContextDbgControl(ctx,false); 
      
        const RegList     &regs     = regdata[cname]._regs;
        const RegFileList &regfiles = regdata[cname]._regfiles;

        _o << "struct " << ctype << " {\n\n";

        // Note: We don't currently support shared resources in contexts.
        ClassData ctxdata;
        _genregs.generateRegVars(ctxdata,regs);
        _genregs.generateRegFileVars(ctxdata,regfiles);
        ctxdata.writeMemberVars(_o);

        ForEach(ctx._mmu,i) {
          const gc_string &ln = i->first;
          const MmuLookup &l = *_core.get_mmulookup(ln);
          _o << getArrayTypeName(l) << ' ' << getArrayName(l) << ";\n";
        }

        _o << "\n};\n\n"

           << "struct " << cotype << " {\n\n"
           << cotype << "(" << coretype << " &core) : _core(core) {\n"
           << "reset();\n"
           << "};\n"
           << "void reset() {" << ctxctrl << " = " << ctxdbg << " = 0;}\n"
           << "unsigned num_contexts() const { return " << ctx.num_contexts() << "; };\n"
           << "unsigned active_context() const { return " << ctxctrl << "; };\n\n"
           << "unsigned debug_context() const { return " << ctxdbg << "; };\n\n"

           << ctype << " &operator()(unsigned index) { return _ctx[index]; };\n"
           << "const " << ctype << " &operator()(unsigned index) const { return _ctx[index]; }\n\n"

           << ctype << " &operator()() { return _ctx[" << ctxctrl << "]; };\n"
           << "const " << ctype << " &operator()() const { return _ctx[" << ctxctrl << "]; };\n\n";

        writeCtxLoadStoreClear(ctx,regs,regfiles,ctx_sizes);

        _o << coretype << " &_core;\n"
           << ctype << " _ctx[" << ctx.num_contexts() << "];\n"
           << "unsigned " << ctxctrl << ";\n"
           << "unsigned " << ctxdbg << ";\n\n"
           << "};\n\n";

        // Then instantiate the context object and the control variable.
        // The control variable will be init'd and reset to 0.
        _cd._member_vars.push_back(cotype + " " + coname);
        _cd._constr_inits.push_back(coname+"(*this)");
        _cd._reset_stmts.push_back(coname + ".reset();");

        // External access to the context-control variables needs to have the object name added.
        ctxctrl = getContextControl(ctx);
        ctxdbg  = getContextDbgControl(ctx);

        // This is the predicate which controls what thread is active.
        gc_string pname  = ctx.name() + "_control_predicate";
        gc_string fname  = getCtxControlFunc(ctx);
        gc_string uname  = getCtxUpdateFunc(ctx);
        gc_string ccname = getCtxChangeFunc(ctx);
        gc_string cbname = getCtxChangeBackFunc(ctx);
        Ptree *sym = AdlGenSym();
        genFunc("unsigned "+pname,_core.env(),ctx.active());
        _o << "void " << fname << "() {\n"
           << ctxctrl << " = " << pname << "();\n"
           << "assert(" << ctxctrl << " < " << ctx.num_contexts() << ");\n";
        // If any of the MMU registers are contained within this context, then we must
        // invalidate the MMU cache on a context change.
        if (mmuHasContextRegs(ctx)) {
          _o << "clear_mmu_cache();\n";
        }
        _o << "}\n\n"
           << "void " << sym << "() {\n"
           << ctxupdate << " = true;\n"
           << "}\n\n"
           << "void " << uname << "() {\n"
           << fname << "();\n"
           << "if (Tracing && trace_mode()) {\n"
           << "_logger->log_ctx_change(" << ctx.id() << ",\"" << ctx.name() << "\"," << ctxctrl << ",ctxSwitch);\n"
           << "}\n"
           << "}\n\n"
           << "bool " << ccname << "(unsigned index) {\n"
           << "if (Tracing && trace_mode()) {\n"
           << "if (index != " << ctxctrl << ") {\n"
           << "_logger->log_ctx_change(" << ctx.id() << ",\"" << ctx.name() << "\",index,ctxUpdate);\n"
           << "return true;\n"
           << "}\n"
           << "}\n"
           << "return false;\n"
           << "}\n\n"
           << "void " << cbname << "(bool log) {\n"
           << "if (Tracing && trace_mode() && log) {\n"
           << "_logger->log_ctx_change(" << ctx.id() << ",\"" << ctx.name() << "\"," << ctxctrl << ",ctxDone);\n"
           << "}\n"
           << "}\n\n";
          
        _watches.push_back(Watch(_walker.regs_referenced(),Ptree::Make("%s",uname.c_str()),sym));
      }

      // Then generate introspection information and support functions.
      if (!contexts.empty()) {
        genContextSupportFuncs(contexts,ctx_sizes);
      }
    }

    void genNextConcurRegs()
    {
      if (_core.has_concurrent_writes()) {
        const StrSet &concr_regs = _core.concurrent_writes();
        ForEach(concr_regs,r) {
          _o << *r << "()._value_bundles.next();\n";
        }
      }
    }

    void generateParallelDS()
    {
      gc_string queueType;
	
      if ((_core.parallel_execution() || _core.has_delayed_writes()) && 
          ((_config.mem_if() != mNonBlocking) || normal_iss())) {
        // If we have delay slots, then use a value large enough to handle the
        // highest.  Otherwise, we just need a single queue to handle parallel
        // commits for the current cycle.
        unsigned d = (_core.has_delayed_writes()) ? max(_core.max_write_delay()+1,(unsigned)_config.max_delay_slots()) : 5;
        gc_string cq;
        MkGStr(cq,"CommitQueues<" << dec << d << ">  "+getCommitQueueName());
        _cd._member_vars.push_back(cq);
      }
      // Generate the pakcet_position 
      if (_core.parallel_execution()) {
        if (_core.post_fetch_skip()) {
          _cd._member_vars.push_back("bool _skip_instruction_commit");
          _cd._constr_inits.push_back("_skip_instruction_commit(false)");
          _cd._reset_stmts.push_back("_skip_instruction_commit = false");
        }

        _cd._member_vars.push_back("unsigned _packet_position");
        _cd._constr_inits.push_back("_packet_position(0)");
        _cd._reset_stmts.push_back("_packet_position = 0");
        _cd._member_vars.push_back("bool _packet_position_reset");
        _cd._constr_inits.push_back("_packet_position_reset(false)");
        _cd._reset_stmts.push_back("_packet_position_reset = false");
        _cd._member_vars.push_back("bool _empty_commit_queue");      // A flag to indicate whether the commit queue should be 
        _cd._constr_inits.push_back("_empty_commit_queue(true)");    // emptied after the instruction execution, it is true
        _cd._reset_stmts.push_back("_empty_commit_queue = true");    // by default, but it is false for micro instructions
                                                                     // The flag is not used for all architectures, and may be
                                                                     // redundant for some architectures
        
        
	
	
        _o << "//\n" 
           << "//  Packet position accessors.\n"
           << "//\n\n"
           << "unsigned getPacketPosition() const  {\n"
           << "return _packet_position;\n"
           << "}\n\n"
           << "void setPacketPosition(unsigned pos) {\n"
           << "_packet_position = pos;\n"
           << "_packet_position_reset = (_packet_position == 0);\n"
           << "}\n\n"
           << "bool end_of_packet() const {\n"
           << "return (_packet_position == 0);\n"
           << "}\n\n"
           << "//\n"
           << "// called after post_exec() "
           << "//\n";
        if (_core.has_prefix_instruction() || !transactional_iss()) {
          _o << "void packetPositionReset() {\n"
             << "if (_packet_position_reset) {\n"
             << "_packet_position_reset = false;\n"
             << "prefix_counter_reset();\n";
          if (_core.post_packet_handler()) {
            _o << "post_packet_handler();\n";
          }
	  
          if (_core.has_prefix_instruction()) {	
            _o << "reset_prefix();\n";
          }
          if (normal_iss() || _config.mem_if() != mNonBlocking) {
            _o << getCommitQueueName() + ".commit();\n";
          }
          if (_core.has_delayed_writes()) {
            _o << getCommitQueueName() << ".next();\n";
            genNextConcurRegs();
          }
          if (_core.post_packet_commit_handler()) {
            _o << "post_packet_commit_handler();\n";
            // Add in another commit to make sure that any resources modified by
            // the final handler get flushed out.
            if (normal_iss() || _config.mem_if() != mNonBlocking) {
              _o << getCommitQueueName() + ".commit();\n";
            }
          }

          _o << "}\n"
             << "}\n\n";
        }
      }

      // If we have delayed writes, then we need to create a function to allow
      // the performance model to drain the commit queue.  Otherwise, create a
      // dummy function in order to create a consistent interface.
      _o << "// Commit any pending updates.\n"
         << "void empty_commit_queue() {\n";
      if (_core.parallel_execution() || _core.has_delayed_writes()) {
        if (_core.parallel_execution()) {
          if (!_core.has_prefix_instruction() && !transactional_iss()) {
            _o << "if ((_instr_count % " << _core.parallel_execution() << " == 0) && (_empty_commit_queue)) {\n" //
               << "_packet_position_reset = false;\n"
               << getCommitQueueName() << ".commit();\n";
            if (_core.has_delayed_writes()) {
              _o << getCommitQueueName() << ".next();\n";
              genNextConcurRegs();
            }
            _o << "setPacketPosition(0);\n"
               << "}\n";  //
          }
        } else if (_core.has_delayed_writes()) {
          // For the situation where we don't have a parallel architecture, but we
          // do have delay slots.
          _o << getCommitQueueName() << ".commit();\n"
             << getCommitQueueName() << ".next();\n";
        }
      }
      _o << "}\n\n";

    }

    // This generates any data structures/support stuff required if we are
    // generating a model with rollback capabilities.
    void generateRollbackDS()
    {
      if (rollback()) {
        gc_string ni;
        gc_string rs = getRestoreStackName();
        _cd._member_vars.push_back("RestoreStack "+rs);
        _cd._reset_stmts.push_back(rs+".reset()");

        gc_string re = getRollbackEnableName();
        _cd._member_vars.push_back("bool "+re);
        _cd._constr_inits.push_back(re+"(false)");
        _cd._reset_stmts.push_back(re+" = false");

        _o << ni << "void enable_rollback(bool e) { " << re << " = e; }\n\n"
           << ni << "void commit_rollback() { " << rs << ".commit(); }\n\n"
           << ni << "void flush_rollback() { " << rs << ".reset(); }\n\n"
           << ni << "RestoreStack get_rollback_data() {\n"
           << "RestoreStack tmp;\n"
           << "tmp.set(" << rs << ");\n"
           << "return tmp;\n"
           << "}\n\n";
      } else {
        // Stubbs for the above rollback functions, so that we can always
        // compile, even if rollback isn't enabled.
        _o << "void enable_rollback(bool e) { }\n\n"
           << "void commit_rollback() { }\n\n"
           << "void flush_rollback() { }\n\n"
           << "RestoreStack get_rollback_data() { return RestoreStack(); }\n\n";
      }
    }

    // This generates support for ignore mode.
    void generateIgnoreModeDS()
    {
      if (ignore_mode()) {

        gc_string re = getIgnoreEnableName();
        _cd._member_vars.push_back("bool "+re);
        _cd._constr_inits.push_back(re+"(false)");
        _cd._reset_stmts.push_back(re+" = false");

        _o << "void set_ignore_mode(bool i) { " << re << " = i; }\n\n"
           << "bool ignore_enabled() { return " << re << "; }\n\n";
      } else {
        // Stubbs for the above rollback functions, so that we can always
        // compile, even if rollback isn't enabled.
        _o << "void set_ignore_mode(bool) { }\n\n"
           << "bool ignore_enabled() { return false; }\n\n";
      }
    }

    bool skip_for_getifielddata(const InstrField &ifield)
    {
      return (ifield.interface() || ifield.encap_field() || ifield.is_generated());
    }

    void generateInstrFieldDS() 
    {
      if (instr_attrs()) {
        auto ifields = _core.instrfields();
        int cnt = 0;
        ForEach(ifields,i) {
          const InstrField &ifield = *i->second;
          if (skip_for_getifielddata(ifield)) continue;
          cnt++;
        }
        _o << "const IndexMap &getInstrFieldData() {\n"
           << "static std::pair<const std::string,int> instrfield_inits[" << cnt <<"] = {\n";
        ForEach(ifields,i) {
          const InstrField &ifield = *i->second;
          if (skip_for_getifielddata(ifield)) continue;
          _o << "std::make_pair(\"" << ifield.name() << "\"," << ifield.id() << "),\n";
        }
        _o << "};\n"
           << "static IndexMap instrfield_hash(&instrfield_inits[0],&instrfield_inits["
           << cnt << "]);\n"
           << "return instrfield_hash;\n"
           << "};\n";
      }
    }

    // If we have JIT support, then we generally want to generate a DMI
    // (direct-memory interface) cache, which maps effective addresses to host
    // memory, as a way to speed up performance.  However, for simple models
    // (those with no MMU or hardware caches) and no debug-mode, we don't
    // bother, because it just adds extra overhead.
    void checkDmiCache()
    {
      if (_config.dmi_cache_support()) {
        // The global flag is true if we haven't been told to not generate the
        // cache and we have JIT support enabled.
        if (!_core.mmu() && !_core.get_first_cache(DataCache) && !_config.debug_mode()) {
          _coreconfig.set_dmi_cache_support(false);
        } else {
          _coreconfig.set_dmi_cache_support(true);
        }
      } else {
        _coreconfig.set_dmi_cache_support(false);
      }
    }

    void genForwardDecls(const gc_string &coretype)
    {
      if (multi_compile()) {
        _geninstrs.generateJitFwdDecls();
      }
    }

    gc_string getGlobalParmName(const gc_string &pfx,const Parm &parm)
    {
      return "Global_" + pfx + "_" + getParmName(parm);
    }

    // We also list constant parms here.  These are parms which can be
    // considered to be constant at the time of translation.  We declare them as
    // globals so that LLVM can then fold them out as constants.  We also set up
    // a macro so that, during JIT compile, their decls in the class will use a
    // different name, so that the member variable isn't used.
    void genGlobalJitDecls()
    {
      if (jit_support()) {
        gc_string pfx = getJitCoreGlobal(_path);
        ForEach(_coreconfig.trans_const_parms(),i) {
          const Parm &parm = *_core.get_parm(*i);
          gc_string name  = getParmName(parm);
          gc_string gname = getGlobalParmName(pfx,parm);
          gc_string hname = getParmHandleName(parm);
          gc_string type  = getParmType(parm);       

          genParmType(parm,type);

          _o << "extern \"C\" " << type << " " << gname << " = " << parm.value() << ";\n"
             << "#define " << name << " " << gname << "\n\n";

          _cd._member_vars.push_back("JitSupport::ParmId "+hname);
          _cd._constr_inits.push_back(hname+"(_jit.get_parm_handle(\""+gname+"\",&"+gname+","+parm.value()+"))");
        }
      }
    }

    // This generates the graphite header, which contains an enum for the
    // various instruction classes, plus an array of name strings.
    void genGraphiteEnum()
    {
      if (!graphite_support()) return;

      auto enum_name   = getPathStr(_path,"_") + "_Types";
      auto string_name = getPathStr(_path,"_") + "_Names";
      auto is_impl     = _core.name() + "_graphite_impl";
      auto guard       = _core.name() + "_define_h";

      vector<const CoreConfig::GraphiteGroup*> grps;
      for (auto &g : _coreconfig.graphite_groups()) {
        grps.push_back(&g.second);
      }
      assert(!grps.empty());
      sort(grps.begin(),grps.end(),[](const CoreConfig::GraphiteGroup *x,const CoreConfig::GraphiteGroup *y) {
          return x->id() < y->id();
        });

      *_gho << "#ifndef " << guard << "\n"
            << "#define " << guard << "\n\n"

            << "enum " << enum_name << " {\n";

      for (auto &g : grps) {
        *_gho << g->name() << " = " << g->id() << ",\n";
      }

      *_gho << "num_instr_classes = " << grps.back()->id()+1 << ",\n"
            << "};\n\n"

            << "extern const char *" << string_name << "[];\n\n"

            << "#endif\n\n"

            << "#ifdef " << is_impl << "\n\n"
            << "const char *" << string_name << "[] = {\n";

      for (auto &g : grps) {
        *_gho << "\"" << g->name() << "\",\n";
      }

      *_gho << "};\n\n"
            << "#endif\n\n";
    }


    void generate()
    {
      gc_string coretype = getCoreTypeName(_core);

      // Setup of various symbol names and stuff.
      _geninstrs.handlerSetup();

      // Any global JIT decls or defines for this core.
      genGlobalJitDecls();

      // A graphite enum, if relevant.
      genGraphiteEnum();

      if (library_mode()) {
        _o << "\ntemplate <class IssBaseClass>";
      }
      _o << "\nstruct " << coretype << " : ";
      if (library_mode()) {
        _o << "public IssBaseClass, ";
      }
      _o << "public IssCore";

      // Add on additional base classes as necessary.  At the very least, we
      // want the instruction count.  If we're time-tagged and/or have the td
      // API, then we need the appropriate base class.
      string base_class;
      if (time_tagged()) {
        base_class = "TimeTaggedBase";
      } else if (td_api()) {
        base_class = "TemporalDecouplingBase";
      } else {
        base_class = "InstrCountBase";
      }
      _coreconfig.set_core_base(base_class);

      _o << ", public " << base_class << " {\n\n";

      // Check whether this core should have a DMI cache.
      checkDmiCache();

      // If we have a transactional ISS, generate the instruction packet.
      _genip.generateInstrPacket();
      _max_slots = _genip.max_slots();

      // Generate the register-file-attribute enum, if applicable.
      _genregs.generateRegFileAttrs();

      // Generate the instruction-attribute enum, if applicable.
      _geninstrs.generateInstrAttrs();

      // Generate the MMU data structures first, if applicable, so that helper
      // functions can use the MMU lookup types.
      generateLookupDS(_o,_core,_cd,_config,_coreconfig);

      // Generate exception declarations.
      _genexceptions.generateExceptions();

      // Generate helper code.
      _geninstrs.generateHelpers();

      // Generate architectural parameters
      generateParms();

      // Generate external resources
      generateExtResources();

      // Generate instruction table information.
      _geninstrs.generateInstrTables();

      // Generate event buses.
      generateEventBuses();

      // Generate the MMU, if one is defined and we're configured
      // to do so.
      generateMMU(_o,get_mmu(),_core,_walker,_genip.mem_comm_slots(),_filename,_cd,_mmu_regs,_config,_coreconfig);

      // Generate caches, if they exist.
      if (gen_caches()) {
        generateCaches(_o,_core,_walker,_watches,_filename,_cd,_config,_coreconfig);
      }

      // Generate commit queue if parallel execution mode
      generateParallelDS();

      // Generate any rollback/restore logic, if necessary.
      generateRollbackDS();

      // Same for ignore-mode.
      generateIgnoreModeDS();

      // Generate instruction field ids
      generateInstrFieldDS();

      // Generate memories, if they exist
      generateMemories(_o,_core,_walker,_filename,_externs,_end_code,_cd,_config,_coreconfig);

      RegCtxMap regdata;
      makeRegLists(regdata,_core);

      // Generate the data structures for all registers and register files.
      // This includes the creation of all contexts.
      ForEach(regdata,i) {
        _genregs.generateRegDS(i->second._regs,&_core);
        _genregs.generateRegFileDS(i->second._regfiles,&_core);
      }

      _genregs.generateRegVars(_cd,regdata[DefaultCtxName]._regs);
      _genregs.generateRegFileVars(_cd,regdata[DefaultCtxName]._regfiles);

      genContexts(regdata);

      // Create the registers.
      _genregs.setPacketWidth(_genip.instrPacketWidth());
      _genregs.generateRegResources();

      generateCheckWatches();
      
      // Create disassembler helpers
      genDisasmHelpers(_o,_core,"static");
      
      // Create instruction field accessors.
      genInstrFieldGetters(_o,_core,"inline static");
      genInstrFieldClearers(_o,_core,"inline static");
      genInstrFieldTableGetters(_o,_core,"inline static");
      if (!normal_iss() && _core.parallel_execution()) {
        genGenericInstrFieldGetters(_o,_core,"inline static");
      }

      // create prefix fields accessors
      generatePrefixDS();

      // Create the subinstruction fucntions
      _geninstrs.generateSubInstrs();

      // Create instruction handler functions.
      _geninstrs.generateInstrHandlers();

      // Create miscellaneous simulation support functions.
      _geninstrs.generateSimSupportFuncs();

      // Add system call support, if relevant.
      _geninstrs.generateSysCallSupport();

      // Create the instruction handling code.
      _geninstrs.generateInstrs();

      // Miscellaneous simulation control functions.
      writeSupportFuncs(_o,_cd,_config,true,post_sim(),coretype);

      // Create debug support stuff, if mode is enabled.
      generateDebugSupport();

      // Handle parent references.
      genParentRefs(_cd,_parents.begin(),_parents.end());

      // Create the accessor/setup functions here.
      generateSetupFuncs(_parents.empty());

      // Write out the global function accessors.
      writeGlobalAccessFuncs();

      _o << "};\n\n";

      // If necessary, write out any plain-function declarations here (necessary
      // for some situations in which we use plain functions, rather than
      // methods, to avoid MSVC bullshit bugs).
      genForwardDecls(coretype);

    }

  };

  // This writes out all systems and cores in the database.
  class GenSystem : public GenBase {
  public:
    GenSystem(opfxstream &o,PfxFileArray *impls,opfxstream *jo,opfxstream *gho,
              ConfigDB &config,const gc_string &filename, 
              StrList &end_code,const SysConfig *sysconfig, StrSet *externs,
              InstrCoreSyms &instr_core_syms,InstrPacketMap &packet_map) :
      GenBase(o,impls,jo,gho,config, sysconfig,filename,externs),
      _config(config),
      _end_code(end_code),
      _instr_core_syms(instr_core_syms),
      _packet_map(packet_map),
      _max_slots(0)
    {
    };

    ConfigDB           &_config;
    StrList            &_end_code;
    InstrCoreSyms      &_instr_core_syms;
    InstrPacketMap     &_packet_map;
    unsigned            _max_slots;

    bool normal_iss() const
    {
      return _config.iss_type() == tNormal;
    }

    bool jit_support() const
    {
      return _config.jit_support();
    }

    bool extern_event_bus() const
    {
      return _config.extern_event_bus();
    }

    void generate(const Data &data)
    {
      Path       path;
      Parents    parents;

      // First, generate all cores in the system.
      genCores(path,parents,data.cores());

      // Next, create all systems.
      Resources shared;
      const SysHash &systems = data.systems();
      ForEach(systems,i) {
        genSystem(path,*(i->second),parents,shared,_externs);
      }
    };
  
  protected:

    // Lookup timing information for a path.  Returns 0 if configuration information
    // does not exist.  Throws an error if configuration information does exist but
    // we can't find a path for this core.
    CoreConfig *getCoreConfig(Path &path)
    {
      return _config.getCoreConfig(path);
    }

    const SysConfig *getSysConfig(Path &path) 
    {
      return _config.getSysConfig(path);
    }

    // Generate cores.
    void genCores(Path &path,Parents &parents,const CoreHash &cores)
    {
      ForEach(cores,i) {
        const Core &core = *(i->second);
        path.push_back(core.name());
        if (_config.verbose_mode()) {
          cout << "\n<CORE name=\"" << path << "\">\n";
        }
        InstrSyms &is = _instr_core_syms[&core];
        GenCore gencore(_o,_impls,_jo,_gho,core,getCoreConfig(path),_config,_filename,&_externs,_end_code,
                        path,parents,is,_packet_map);
        gencore.generate();
        path.pop_back();
        if (_config.verbose_mode()) {
          cout << "</CORE>\n";
        }
        _max_slots = max(_max_slots,gencore.max_slots());
      }
    }

    struct ItemData {
      gc_string _type_path;
      gc_string _inst_path;
      const Core *_core;
    };

    // Does a depth-first traversal to find the first core.
    // Returns a pair:  (full-path,const-ptr-to-core).
    ItemData getCore(const SysItem *sys)
    {
      if (const System *s = isSystem(sys)) {
        assert(!s->items().empty());
        auto cp = getCore(s->items().begin()->second);
        ItemData tmp;
        tmp._type_path = s->name()+"::" + cp._type_path;
        tmp._inst_path = s->items().begin()->first + ((cp._inst_path.empty()) ? "" : ("." + cp._inst_path));
        tmp._core      = cp._core;
        return tmp;
      } else if (const Core *c = isCore(sys)) {
        ItemData tmp;
        tmp._type_path = c->name();
        tmp._core      = c;
        return tmp;
      } else {
        assert(0);
      }
    }

    template <class T> struct NameOrder {
      bool operator()(T x,T y) const {
        return (x->name() < y->name());
      };
    };

    void genSharedRes(const System &sys,const Resources &shared,ClassData &cd,StrList &child_args)
    {
      // Instantiate any shared resources that haven't already been declared.
      // We do this in alphabetical order by name so that we'll match the generation
      // order so that our constructor arguments match up.

      auto cp = getCore(&sys);

     // Generate any shared MMU data structures.
      LookupList lookups;
      ForEach(sys.shared_mmulookups(),i) {
        const MmuLookup &lookup = *(sys.get_mmulookup(i->first));
        lookups.push_back(&lookup);
        child_args.push_back(getArrayName(lookup));
      }

      generateLookupDS(_o,lookups,cd,shared,_config);
      generateMmuHelpers(_o,lookups,cd,shared,_config,false);

      // Generate shared parameters.
      ForEach(sys.shared_parms(),i) {
        const Parm &parm = *(sys.get_parm(i->first));
        child_args.push_back(getParmName(parm));
        generateParmDS(parm,cd,shared,false);
      }

      // Generate any shared eventbuses.
      ForEach(sys.shared_eventbuses(),i) {
        const EventBus &eb = *(sys.get_eventbus(i->first));
        child_args.push_back(getEventBusIndexName(eb));
        child_args.push_back(getEventBusIdName(eb));
        genEventBusDS(eb,shared,cd,false);
        // Now add code to register the bus.  We only do this at the top-most level
        // so that we only ever get a single instance of the bus registered.
        if (!shared.contains(eb)) {
          if (!extern_event_bus()) {
            cd._constr_stmts.push_back(cp._inst_path+"->"+eb.name()+".registerbus()");
          }
        }
      }

      // Generate any shared external resources.
      ForEach(sys.shared_extresources(),i) {
        const ExtResource &er = *(sys.get_extresource(i->first));
        child_args.push_back(er.name());
        generateExtResDS(er,cd,shared);
      }      

      generateCacheDS(_o,sys,shared,cd,_config,&child_args);

      MemoryList memories;
      ForEach(sys.shared_memories(),i) {
        const Memory *memory = sys.get_memory(i->first);
        memories.push_back(MemoryEl(memory,shared._memories.count(i->first)));
        child_args.push_back(getMemoryName(*memory));
      }
      generateMemDS(_o,sys.name(),memories,_externs,cd,_config,true);
      generateMemHandler(_o,sys,&sys.items(),memories,_externs,cd,_config,true);

      RegList regs;
      RegFileList regfiles;
      GenRegsBaseIF genregs(_o,cd,_config);

      ForEach(sys.shared_regs(),i) {
        const Reg *reg = sys.get_reg(i->first);
        assert(reg);

        child_args.push_back(reg->name()+"()");

        regs.push_back(RegEl<Reg>(sys.get_reg(i->first),
                                  shared._regs.count(i->first)));
      }
      genregs.generateRegDS(regs,cp._core);  
      genregs.generateRegVars(cd,regs);

      ForEach(sys.shared_regfiles(),i) {
        const RegFile *rf = sys.get_regfile(i->first);
        assert(rf);

        child_args.push_back(rf->name()+"()");
      
        regfiles.push_back(RegEl<RegFile>(sys.get_regfile(i->first),
                                          shared._regfiles.count(i->first)));
      }
      genregs.generateRegFileDS(regfiles,cp._core);
      genregs.generateRegFileVars(cd,regfiles);

      _o << "\n";

      RegCtxMap regdata;
      regdata[DefaultCtxName]._regs = regs;
      regdata[DefaultCtxName]._regfiles = regfiles;

      // Now generate the show-registers and setRegs functions for this level in
      // the hiearchy.
      genregs.generateRegAssistFuncs(regdata,regs,regfiles,0,true);

    }

    typedef gc_list<Path> Paths;

    // Returns a path of member variable(s)' names to a leaf element.  This
    // returns a list of all matching elements of the specified type.
    bool pathToMember(Paths &paths,const System *cur,const SysItem *trg)
    {
      auto items = cur->items();
      ForEach(items,i) {
        if ( i->second == trg ) {
          paths.push_back(Path());
          paths.back().push_front(i->first);
        } else if (const System *n = isSystem(i->second)) {
          Paths tmp_paths;
          if (pathToMember(tmp_paths,n,trg)) {
            ForEach (tmp_paths,j) {
              paths.push_back(*j);
              paths.back().push_front(i->first);
            }
          }
        }
      }
      return !paths.empty();
    }

    Paths pathToMember(const System &cur,const SysItem *trg)
    {
      Paths paths;
      if (pathToMember(paths,&cur,trg)) {
        return paths;
      } else {
        RError("Could not find a member variable of type " << trg->name());
      }
    }

    void genSharedWatches(const System &cur,const Parents &parents)
    {
      // Generate any watch functions.
      auto me = parents.back();
      ForEach(me._watches,i) {
        const ParentWatch &pw = i->second;
        _o << "void " << pw._watchfunc << "() {\n";
        ForEach(pw._child_watches,j) {
          Paths paths = pathToMember(cur,j->second);
          ForEach(paths,p) {
            _o << getPathStr(*p,"->") << "->" << j->first << "();\n";
          }
        }
        _o << "}\n\n";
      }
    }

    void writeChildCalls(const System &sys,const gc_string &fname, const gc_string &cname,bool close_func = true, 
                         const char* fargs = "", const char* child_args = "", const char *pre = "")
    {
      _o << "void " << fname << "( " <<  fargs << " )\n"
         << "{\n"
         << pre;
      ForEach(sys.items(),i) {
        _o << i->first << "->" << cname << "(" << child_args << ");\n";
      }
      if (close_func) {
        _o << "}\n\n";
      }
    }

    void genSystem(Path &path,const System &sys,Parents &parents,const Resources &shared,
                   StrSet &externs)
    {
      _o << "struct " << sys.name() << " : IssSystem {\n\n";

      // First, we put any shared resources of our own into the shared-set
      // so that children won't declare resources that we will declare at
      // this level.
      Resources ushared = sys.shared();

      path.push_back(sys.name());
      set_extern_mem(getSysConfig(path));
      set_extern_mem(externs);

      ClassData  cd;
      StrList    child_args;

      bool top = (parents.empty());

      parents.push_back(Parent(&sys,sys.name(),AdlGenSym()));

      // Top-level node starts the child-id numbering scheme.  Child systems
      // just pass the reference down to the cores.
      if (!top) {
        cd._constr_args.push_back("unsigned &child_id");
      } else {
        cd._member_vars.push_back("unsigned _dummy_child_id");
        cd._constr_inits.push_back("_dummy_child_id(0)");
      }

      genSharedRes(sys,shared,cd,child_args);

      if (!normal_iss()) {
        cd._constr_args.push_back("const ttime_t *time");
      }

      // Handle parent references here.  Skip the last item, since that
      // represents outself.
      genParentRefs(cd,parents.begin(),parents.end()-1);

      // Generate any locally-defined cores.
      genCores(path,parents,sys.local_cores());

      // Generate any locally-defined systems.
      ForEach(sys.local_systems(),i) {
        genSystem(path,*(i->second),parents,ushared,_externs);
      }

      // Generate watches needed by shared resources.
      genSharedWatches(sys,parents);


      // Merge the shared information from this level with
      // what's already been shared.
      ushared.update(shared);

      path.pop_back();
      parents.pop_back();

      // We use a dummy variable to allow us to initialize system resources
      // before the children in the initializer list.
      cd._member_vars.push_back("bool __dummy_reset_var");
      cd._constr_inits.push_back("__dummy_reset_var(reset_sys_resources(true))");

      // Now instantiate all constituent items.
      cd.writeMemberVars(_o);
      ForEach(sys.items(),i) {
        _o << i->second->name() << " *" << i->first << ";\n";
      }
      _o << "\n";

      // Now write out the various accessor functions needed for simulation.
      _o << sys.name() << "(const std::string &__n,const IssNode *parent,unsigned &id";
      ForEach(cd._constr_args,i) {
        _o << "," << *i;
      }
      _o << ") :\n"
         << "IssSystem(__n,\"" << getSysItemType(sys) << "\",parent)";
      bool first = false;
      ForEach(cd._constr_inits,i) {
        constr_init_sep(_o,first);
        _o << *i;
      }
      ForEach(sys.items(),i) {
        constr_init_sep(_o,first);
        _o << i->first << "(new " << i->second->name() << "(\"" << i->first << "\",this,id";
        // At the top, send in the dummy child id variable, else just pass on
        // the reference item.
        if (top) {
          _o << ",_dummy_child_id";
        } else {
          _o << ",child_id";
        }
        // If the child is a core, then increment the value once it's passed
        // in.
        if (isCore(i->second)) {
          _o << "++";
        }
        ForEach(child_args,j) {
          _o << "," << *j;
        }
        if (!normal_iss()) {
          _o << ",time";
        }
        // If locally defined, add on parent references.
        if ( sys.check_is_local(*(i->second)) ) {
          ForEach(parents,i) {
            _o << "," << i->_ref;
          }
          _o << ",*this";
        }
        _o << "))";
      }
      _o << "\n{\n";
      ForEach(sys.items(),i) {
        _o << "_children.push_back(" << i->first << ");\n";
      }
      if (parents.empty()) {
        _o << "resetInit();\n";
      }
      cd.writeConstrStmts(_o);
      _o << "};\n\n"

         << "unsigned activeCount() const\n"
         << "{\n"
         << "return (!_active) ? 0 : (";
      first = true;
      ForEach(sys.items(),i) {
        pfx_sep(_o," + ",first);
        _o << i->first << "->activeCount()";
      }
      _o << ");\n"
         << "}\n\n"
         << "virtual bool is_active() const {\n"
         << "return activeCount();\n"
         << "}\n\n";

      if (normal_iss()) {
        if (jit_support()) {
          writeChildCalls(sys,"simulate", "simulate", true, "", "");
        } else {
          writeChildCalls(sys,"simulate","simulate",true,"");
        }
      }

      _o << "bool reset_sys_resources(bool init_reset)\n"
         << "{\n"
      // Dummy statement in case we have reset statements which rely upon the
      // "log" parameter that's present for core reset functions.
         << "const bool ATTRIBUTE_UNUSED log = false;\n";
      cd.writeResetStmts(_o);
      _o << "_active = true;\n"
         << "return true;\n"
         << "}\n";

      writeChildCalls(sys,"reset", "reset",true,"","","reset_sys_resources(false);\n");

      writeChildCalls(sys,"resetInit", "resetInit",true,"","","");

      writeChildCalls(sys,"checkWatches", "checkWatches");

      writeChildCalls(sys,"tracingOn", "tracingOn");

      writeChildCalls(sys,"tracingOff", "tracingOff");

      writeSupportFuncs(_o,cd,_config,false,false,gc_string());

      // Write out the global function accessors.
      writeGlobalAccessFuncs();

      _o << "};\n\n";

    };

  };

  // Writes various outer-level information.
  class GenOuter : public GenBase {
  public:
    GenOuter(opfxstream &o,const Data &data,const ConfigDB &config,const gc_string &filename) : 
      GenBase(o,0,0,0,config,0,filename,0), _data(data) 
    {};

  protected:
    const Data &_data;

  public:
    
    void genHelperDecls() 
    {
      // Spit out helper declarations here.
      _o << "//\n// Helper decls.\n//\n\n";
      const PList &decls = _data.decls();
      ForEach (decls,i) {
        _o << *i << "\n";
      }
    }
    
    void genMemDataStructure()
    {
      _o << "// The global memory object.\n\n\n"  
         << "// Use the mem_read and mem_write routines to access this object from \n"
         << "// other translation units, since those functions\n"
         << "// are not templated.\n\n"
         << "// Include the default implementation\n\n";
      if(extern_mem()) {
        _o <<"#include \"iss/DefaultExternMem.h\"\n\n" ;
      } else {
        _o <<"#include \"iss/DefaultMem.h\"\n\n" ;
      }

      if (extern_dmi()) {
        _o << "#include \"iss/DmiMemHandler.h\"\n\n";
      }
    }

  };

  class GenSim {
  public:
    GenSim(opfxstream &o,PfxFileArray *impls,opfxstream *jo,opfxstream *gho,
           const Data &data,const gc_string &tlname,
           const gc_string &fname,const string &ghname,
           InstrCoreSyms &instr_core_syms,InstrPacketMap &packet_map,
           ConfigDB &config) :
      _o(o),
      _impls(impls),
      _jo(jo),
      _gho(gho),
      _data(data),
      _tlname(tlname),
      _fname(fname),
      _ghname(ghname),
      _instr_core_syms(instr_core_syms),
      _packet_map(packet_map),
      _config(config),
      _max_slots(0)
    {
    };

  protected:
    opfxstream         &_o;
    PfxFileArray       *_impls;
    opfxstream         *_jo;
    opfxstream         *_gho;
    const Data         &_data;
    gc_string           _tlname;
    gc_string           _fname;
    string              _ghname;
    InstrCoreSyms      &_instr_core_syms;
    InstrPacketMap     &_packet_map;
    ConfigDB           &_config;

    unsigned            _max_slots;

  public:

    bool mt_support() const
    {
      return _config.mt_support();
    }

    bool library_mode() const
    {
      return _config.library_mode();
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

    bool logging_mem() const
    {
      return _config.mem_if() == mLogging;
    }

    bool extern_mem() const
    {
      return _config.extern_mem();
    }

    bool extern_event_bus() const
    {
      return _config.extern_event_bus();
    }

    unsigned max_slots() const
    {
      return _max_slots;
    }

    bool jit_support() const
    {
      return _jo;
    }

    bool graphite_support() const
    {
      return _config.graphite_support();
    }

    bool dc_support() const
    {
      return _config.dc_support();
    }

    bool multi_compile() const
    {
      return _config.num_compile_jobs() > 0;
    }

    void writeStdHeader(ostream &o,const gc_string &tname)
    {
      o << "//\n"
         << "// ADL Version " << ADL_PACKAGE_VERSION << "\n"
         << "//\n"
         << "// ISS model generated for " << tname << ".\n";
      if (!_config._preamble.empty()) {
        o << "// " << filter_string(_config._preamble,"\n// ") << "\n";
      }
      o << "//\n\n";
    }

    // Write preamble code to implementation files, if we're doing a multi-file compile.
    void startImplFiles(const gc_string &ns,const gc_string &tname)
    {
      if (_impls) {
        ForRange(_impls->size(),i) {
          ostream &o = (*_impls)[i];

          writeStdHeader(o,tname);
          o << "#define ADL_HEADER\n"
            << "#include \"" << _fname << "\"\n"
            << "\n"
            << "namespace " << ns << " {\n\n";
        }
      }
    }

    // Finish up implementation files, if doing a multi-file compile.
    void finishImplFiles()
    {
      if (_impls) {
        ForRange(_impls->size(),i) {
          ostream &o = (*_impls)[i];

          o << "}\n\n";
        }
      }
    }

    // Generate a usage-logging call, if applicable.
    void genUsageLogging(const gc_string &fname)
    {
      if (_config.log_usage() != "false" && _config.log_usage() != "no") {

        gc_string ul = _config.log_usage();
        gc_string progname, version;        
        createModelVersion(progname,version,fname,ul);

        // Note: We're not currently logging at exit b/c there seems to be some
        // weirdness going on when we do.  We seem to get strange crashes in
        // exit.  So, we'll just log immediately with a fake exit code, or now.
        _o << "#ifndef ADL_HEADER\n"
           << "static bool dummy ATTRIBUTE_UNUSED = log_usage(\"" << progname << "\",\"" << version << "\",0,\"model\",\"make-iss\");\n"
           << "#endif\n\n";
      }
    }

    void genJitHandlersStart(const SysItem *toplevel)
    {
      if (!jit_support()) return;

      writeStdHeader(*_jo,toplevel->name());
      *_jo << "//\n"
           << "// JIT instruction handler implementation file.\n"
           << "//\n"
           << "\n"
           << "#define ADL_HEADER\n"
           << "#define REGFILE_NO_BOUNDS_CHECK\n"
           << "#define NDEBUG\n"
           << "#define ADL_DEBUG_CONTROL\n"
           << "#define ADL_JIT_COMPILE\n"
           << "\n"
           << "\n"
           << "#include \"" << _fname << "\"\n"
           << "\n"
           << "using namespace adl;\n"
           << "\n"
           << "extern \"C\" {\n\n";
    }

    void genJitHandlersEnd()
    {
      if (!jit_support()) return;

      *_jo << "}\n\n";
    }

    void genGraphiteHeaderStart(const SysItem *toplevel)
    {
      if (!graphite_support()) return;

      writeStdHeader(*_gho,toplevel->name());

      *_gho << "//\n"
            << "// Graphite instruction class header.\n"
            << "//\n\n"
        
            << "namespace " << getIssNamespace(_config) << " {\n\n";
    }

    void genGraphiteHeaderEnd()
    {
      if (!graphite_support()) return;

      *_gho << "}\n\n";
    }

    // Various header stuff for decode cache support.
    void genDcDefines()
    {
      if (dc_support()) {
        _o << "#include \"iss/DecodeCacheSupport.h\"\n\n";
      }
    }

    void genNoJitDefines()
    {
      // We have a separate register tracing surround so that register tracing
      // prediction can be separated from other tracing events, such as MMU
      // tracing, which might be enabled by users for other purposes.
      _o << "# define CondTrace(x)          { if (adl_unlikely(_trace_mode)) x; }\n"
         << "# define CondRegTrace(x)       { if (adl_unlikely(_trace_mode & (ttRegWrite | ttRegRead))) x; }\n"
         << "# define CondRegBranchTrace(x) { if (adl_unlikely(_trace_mode & (ttRegWrite | ttRegRead | ttBranchTaken))) x; }\n"
         << "# define ExtCondTrace(x)       { if (adl_unlikely(core->trace_mode())) x; }\n"
         << "# define Dummify(x) x\n";
    }

    // Various header defines and decls for JIT models.
    //
    // The CondTrace macro is used to guard tracing code, in order to improve
    // inlining performance for the JIT.  The problem is that the eventual
    // register tracing functions, due to their use of RNumber, for whatever
    // reason, mean that the register write functions won't get inlined.  By
    // guarding with CondTrace, we're able to cause the inlining to occur when
    // tracing is disabled due to LLVM's constant folding, which allows it to
    // immediately remove the register tracing code, and thus inline the much
    // simpler write function.
    void genJitDefines()
    {
      if (jit_support()) {
        _o << "#include \"iss/JitSupport.h\"\n\n"
           << "#ifdef ADL_JIT_COMPILE\n"
           << "# define CondTrace(x)          { if (" << getJitTraceVar() << ") x; }\n"
           << "# define CondRegTrace(x)       { if (" << getJitTraceVar() << ") x; }\n"
           << "# define CondRegBranchTrace(x) { if (" << getJitTraceVar() << ") x; }\n"
           << "# define ExtCondTrace(x)\n"
           << "# define Dummify(x) _dummy ## x\n"
           << "#else\n";
        genNoJitDefines();
        _o << "#endif\n\n";
      } else {
        genNoJitDefines();
      }
      _o << "\n";
    }

    // Any includes/defines for Graphite integration support.
    void genGraphiteDefines()
    {
      if (graphite_support()) {
        _o << "#include \"graphite/GraphiteSupport.h\"\n\n";
        _o << "#include \"" << base_name(_ghname) << "\"\n\n";
      }
    }

    // Main generation routine.
    void generate()
    {    
      gc_string iss_namespace = getIssNamespace(_config);

      // If the user has specified a top-level item, then try to find it amongst
      // the systems and cores present in the database.  If not found, then we pick
      // a system, if only one is present, else a core if only one is present, else we
      // produce an error.
      const SysItem *toplevel = 0;
      if (_tlname.empty()) {
        if (!_data.systems().empty()) {
          if (_data.systems().size() == 1) {
            toplevel = _data.systems().begin()->second;
          } else {
            RError("No explicit top-level item was specified and multiple systems were found in the database.\n"
                   << "Please specify a top-level item using the --top-level command-line option.");
          }
        } else if (!_data.cores().empty()) {
          if (_data.cores().size() == 1) {
            toplevel = _data.cores().begin()->second;
          } else {
            RError("No explicit top-level item was specified and no systems were found and multiple cores were found in the database.\n"
                   << "Please specify a top-level item using the --top-level command-line option.");
          }
        } else {
          RError("No cores or systems found in the database.");
        }
      } else {
        // Try to find it in the database.  Systems take priority.
        if (!(toplevel = _data.get_sys(_tlname))) {
          if (!(toplevel = _data.get_core(_tlname))) {
            RError ("Unknown top-level item specified:  " << _tlname);
          }
        }
      }

      if (!toplevel) {
        RError("No top-level element was found.");
      }

      genJitHandlersStart(toplevel);
      genGraphiteHeaderStart(toplevel);

      writeStdHeader(_o,toplevel->name());
      
      _o << "#include <iostream>\n"
         << "#include <iomanip>\n"
         << "#include <stdexcept>\n"
         << "#include <sstream>\n"
         << "#include <fstream>\n"
         << "#include <cmath>\n";

      // Are we doing anything with RNumber?
      if (!_config.rnumber_support()) {
        _o << "#define __NO_RNUMBER__\n\n";
      }

      // Include any extra include files.
      ForEach(_config.cpp_cmds(),i) {
        const gc_string &cmd = *i;
        if (cmd.find("define:") == 0) {
          _o << "\n#define " << cmd.substr(7) << "\n";
        } else if (cmd.find("include:") == 0) {
          _o << "\n#include " << cmd.substr(8) << "\n";
        } else {
          RError("Unknown C preprocessor directive command: " << *i);
        }
      }

      _o << "\n"
         << "namespace adl {\n"
      // Specify global constant for tracing.  This is a constant global so that
      // the compiler can fold it out.  Note: This block should *only* contain
      // constants because otherwise you'll get multiple-definition link errors
      // trying to link together multiple ISSs.  If they're constants, then the
      // compiler will fold them out.
      //
         << "static const bool Tracing = " << _config.trace_mode() << ";\n"
         << "}\n\n";

      // If we're supporting multi-threaded simulation, then make sure that the
      // global memory (if we're using it) will be thread-safe.  We also create
      // a global mutex.  We may need to expand upon this in the future, in case
      // we want to support heterogenous simulations.
      if (mt_support()) {
        _o << "#define THREAD_SAFE_MEMORY\n"
           << "#include \"iss/MemoryLocks.h\"\n"
           << "#include \"iss/ThreadedSimLock.h\"\n\n";
      }
      
      // Specify whether this is a transactional ISS or not.
      if (transactional_iss()) {
        if (!logging_mem()) {
          _o << "#define TRANSACTIONAL_ISS\n\n";
        }
        // We only need the coroutine interface if we're usinga non-blocking
        // memory interface.
        if (_config.mem_if() == mNonBlocking) {
          _o << "#include \"iss/Coroutine.h\"\n\n";
        }
      }
      if (logging_mem()) {
        _o << "#define SAFE_MODE_ISS\n\n";
      }

      if (_config.tlm2_endianness()) {
        _o << "#define DATA_INVARIANT_MEMORY\n\n";
      }
                 
      genBitsDecls(_o,is_little_endian(_data));
 
      _o << "#include \"iss/ModelSupport.h\"\n\n";

      if (!_config.syscall_enabled()) {
        _o << "#define ADL_SYSCALL_DISABLED\n";
      }
      _o << "#include \"iss/SysCallMgr.h\"\n\n"
         << "#include \"iss/SysCallExtHandler.h\"\n\n";

       if (extern_event_bus()) {
        _o << "#include \"iss/EventBusExtHandler.h\"\n\n";
      }

      genDcDefines();
      genJitDefines();
      genGraphiteDefines();

      _o << "using namespace adl;\n"
         << "\n\n"
      
         << "#include \"helpers/stringhash.h\"\n"
         << "#include \"helpers/LogUsage.h\"\n"
         << "\n";

      // This will store code that's batched up to be emitted at the end of the
      // file.
      StrList end_code;

      GenOuter genouter(_o,_data,_config,_fname);
      GenSystem gensys(_o,_impls,_jo,_gho,_config,_fname,end_code,_config.getSysConfig(),0,_instr_core_syms,_packet_map);
      
      // We do not put the helper declarations inside of the ADL namespace because
      // this could cause conflicts with using them as declarations for external
      // libraries.
      genouter.genHelperDecls();

      genouter.genMemDataStructure();

      startImplFiles(iss_namespace,toplevel->name());

      if (jit_support()) {
        // The idea behind this macro substitution is that Clang will see all
        // references to the _trace_mode flag as references to a global, so that
        // during code generation, we can fold out the tracing-control flag
        // because it will be set as to be constant.
        _o << "#ifdef ADL_JIT_COMPILE\n"
           << "# define _trace_mode " << getJitTraceVar() << "\n"
           << "uint64_t " << getJitTraceVar() << "  = 0;\n\n"
           << "#endif\n\n";
      }

      _o << "namespace " << iss_namespace << " {\n\n";

      if (jit_support()) {
          _o << "#ifdef JIT_DEBUG\n"
             << "# define JDBG(x) x\n"
             << "#else\n"
             << "# define JDBG(x)\n"
             << "#endif\n\n";
      }

      if (dc_support()) {
          _o << "#ifdef DC_DEBUG\n"
             << "# define DCDBG(x) x\n"
             << "#else\n"
             << "# define DCDBG(x)\n"
             << "#endif\n\n";
      }

      // Declarations for our global functions in this namespace.
      _o << "const char *getCoreName();\n"
         << "const char *getModelFileName();\n"
         << "const adl::AttrData &getAttrData();\n"
         << "const char *getModelPreamble();\n"
         << "void resetGlobal();\n"
         << "void setMemorySize(unsigned size);\n\n";

      // Weirdness here for gcc 5 optimized: _fname gets corrupted for some
      // reason, so maybe the compiler removes a root that the gc was depending
      // upon?  Well, creating this temporary fixes it.
      gc_string tfname = _fname.c_str();
      
      gensys.generate(_data);
      gc_string tname = toplevel->name();
      bool is_core = isCore(toplevel);

      _max_slots = gensys._max_slots;
      
      auto attrs = Data::get_attrs();
      int t = tfname.find_last_of('/');
      _o << "#ifndef ADL_HEADER\n"
         << "const char* getModelFileName() {\n"
         << "static const char* fname = " << "\"" <<  tfname.substr(t+1,tfname.size()-(t+1)-3) << "\";\n"
         << "return fname;\n"
         << "}\n\n";

      // Spit out global access/setup functions here.
      _o << "const AttrData &getAttrData() {\n"
         << "static std::pair<const std::string,int> attr_inits[" << attrs.size() << "] = {\n";
      ForEach(attrs,i) {
        _o << "std::make_pair(\"" << i->first << "\"," << i->second._id << "),\n";
      }
      _o << "};\n"
         << "static AttrData attr_hash(&attr_inits[0],&attr_inits[" << attrs.size() << "]);\n"
         << "return attr_hash;\n"
         << "}\n\n";

      genUsageLogging(tfname);

      _o << "IssNode *createTopLevelNode(unsigned &id)\n{\n";
      if (normal_iss() && !library_mode()) {
        _o << "return new " << tname << "(\"" << tname << "\",0,id";
        if (is_core) {
          _o << ",0";
        }
        _o << ");\n";
      } else {
        _o << "// Dummy function to avoid linking issues.\n"
           << "return 0;\n";
      }
      _o << "}\n\n"
        
         << "const char *getCoreName()\n{\n"
         << "static const char* tmpName = \"" << tname << "\";\n"
         << "return tmpName;\n" 
         << "}\n\n";

      _o << "void resetGlobal()\n"
         << "{\n"
         << "WatchpointsEnabled = false;\n";
      // If we have external memory then it's up to the external driver to reset memory.
      if (!extern_mem()) {
        _o << "resetMemory();\n";
      }
      _o << "}\n\n"
         << "void setMemorySize(unsigned size)\n"
         << "{\n";
      if (!extern_mem()) {
        _o << "GlobalMemory.set_mem_size(size);\n";          
      }
      _o << "}\n\n";
      
      // Model preamble data is simply gc_string information supplied by the user.  The
      // standalone executable will display this when the model starts.  It can be
      // used for such things as specifying versioning information, licensing, etc.
      _o << "static const char *ModelPreambleData = \"" << filter_string(_config._preamble,"\\n") << "\";\n\n"
         << "const char *getModelPreamble()\n"
         << "{\n"
         << "return ModelPreambleData;\n"
         << "}\n\n";
     

      // Emit any end-of-file code that we need to.
      ForEach(end_code,i) {
        _o << *i << '\n';
      }

      _o << "#endif // ADL_HEADER\n\n"
         << "}\n";

      finishImplFiles();

      genJitHandlersEnd();
      genGraphiteHeaderEnd();
    }

  };

  GenSimIF::GenSimIF(opfxstream &o,PfxFileArray *impl,opfxstream *jo,opfxstream *gho,const Data &data,const gc_string &tlname,
                     const gc_string &fname,const string &ghname,InstrCoreSyms &instr_core_syms,InstrPacketMap &packet_map,
                     ConfigDB &config) :
    _impl(new GenSim(o,impl,jo,gho,data,tlname,fname,ghname,instr_core_syms,packet_map,config))
  {
  }

  ImplCall(GenSimIF,generate);

}

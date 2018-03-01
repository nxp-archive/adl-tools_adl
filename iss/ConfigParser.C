//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Parser for time-tag configuration information.
//

#include <sstream>
#include <iostream>

#include "fe/Helpers.h"
#include "fe/ParsingHelpers.h"
#include "fe/Data.h"
#include "fe/Handler.h"

#include "helpers/Macros.h"
#include "helpers/fe_exceptions.h"
#include "helpers/gc_string.h"

#include "ConfigParser.h"
#include "ConfigDB.h"

#include "opencxx/parser/Ptree.h"
#include "opencxx/parser/PtreeArray.h"
#include "opencxx/PtreeIter.h"

using namespace std;
using namespace std::placeholders;
using namespace Opencxx;
using namespace PtreeUtil;

namespace adl {

  class ConfigDB;
  class CoreConfig;

  void unknown_key(const char *key,Ptree *value)
  {
    PError(value,"Unrecognized key '" << key << "'.");
  }

  CoreConfig &make_core_node(Ptree *loc,ConfigDB::Node &node,
                             const char *name,const Core &core)
  {
    if (!name) {
      PError(loc,"Core block defined without a name.");
    }
    auto ip = node._children.insert(make_pair(name,nullptr));
    if (ip.second) {
      ip.first->second = new ConfigDB::Node(new CoreConfig(&core));
    }
    return *(ip.first->second->_coreConfig);
  }

  ConfigDB::Node  *make_sys_node(Ptree *loc,ConfigDB::Node &node,const char *name, const System &sys)
  {
    if (!name) {
      PError(loc,"System block defined without a name.");
    }
    auto ip = node._children.insert(make_pair(name,nullptr));
    if (ip.second) {
      ip.first->second = new ConfigDB::Node(new SysConfig(&sys));
    }
    return ip.first->second;
  }

  enum MemberType { None, Instr, IClass, Memory };

  // Returns the type of a name (instruction, instruction class, or group).
  MemberType get_name_type(const gc_string &name,const Core &core)
  {
    if (!core.get_instr(name)) {
      if(core.get_memory(name)) { 
        return Memory;
      } else if (Data::get_attr(name) < 0) {
        return None;
      } else {
        return IClass;
      }
    } else {
      return Instr;
    }
  }

  // If supplied with a core, make sure that this is a valid instruction or instruction class.
  MemberType check_valid(Ptree *loc,const gc_string &name,const Core &core)
  {
    auto type = get_name_type(name,core);
    if (type == None) {
      PError(loc,"'" << name 
             << "' is not a valid instruction or instruction class defined within core " 
             << core.name() << ".");
    }
    return type;
  }

  // Base class for configuration handlers.
  class BaseCfgHandler : public Handler {
  public:
    BaseCfgHandler(Environment *env,Ptree *loc,
                   CoreConfig &config,const Core &core,bool modify) :
      Handler(env,loc),
      _config(config),
      _core(core)
    {
      _modify = modify;
    };

  protected:
    CoreConfig &_config;
    const Core &_core;
  };

  // Base class for configuration handlers.
  class BaseRemoveCfgHandler : public RemoveHandler {
  public:
    BaseRemoveCfgHandler(Environment *env,Ptree *loc,
                         CoreConfig &config,const Core &core,
                         const gc_string &name,bool modify) :
      RemoveHandler(env,loc,name,modify),
      _config(config),
      _core(core)
    {
    };

  protected:
    CoreConfig &_config;
    const Core &_core;
  };

  // Processes core configuration groups.
  class ConfigCfgHandler : public BaseCfgHandler {
  public:
    ConfigCfgHandler(Environment *env,Ptree *loc,
                     CoreConfig &config,const Core &core,bool modify) : 
      BaseCfgHandler(env,loc,config,core,modify)
    {};

    virtual void add_key(const char *key,Ptree *value)
    {
      if (!strcmp(key,"issue_width")) {
        _config._issue_width = get_uint(value,"core issue-width.",env());
      } else if (!strcmp(key,"mem_split_size")) {
        _config._mem_split_size = get_uint(value,"core memory split-size.",env());
      } else if (!strcmp(key,"generate_mmu")) {
        _config._gen_mmu = get_bool(value,"mmu generation flag.",env());
      } else if (!strcmp(key,"generate_caches")) {
        _config._gen_caches = get_bool(value,"cache generation flag.",env());
      } else if (!strcmp(key,"dmi_cache")) {
        _config._dmi_cache_support = get_bool(value,"DMI (direct-memory interface) cache support flag.",env());
      } else if (!strcmp(key,"log_original_mem_access")) {
        _config._log_orig_mem_access = get_bool(value,"log-original-memory-access flag.",env());
      } else if (!strcmp(key,"tracing_on")) {
        _config._tracing_on = get_func(value,0,"tracing-on function.");
      } else if (!strcmp(key,"tracing_off")) {
        _config._tracing_off = get_func(value,0,"tracing-off function.");
      } else if (!strcmp(key,"clock_cycle_ratio")) {
        _config._clock_cycle_ratio = get_dbl(value,"clock-cycle-ratio value.");
      } else if (!strcmp(key,"reg_callbacks")) {
        handle_list_or_item(value,bind(&ConfigCfgHandler::add_reg_callback,this,_1));
      } else if (!strcmp(key,"reg_read_callbacks")) {
        handle_list_or_item(value,bind(&ConfigCfgHandler::add_reg_read_callback,this,_1));
      } else if (!strcmp(key,"bb_end_instrs")) {
        handle_list_or_item(value,bind(&ConfigCfgHandler::add_bb_end_instr,this,_1));
      } else if (!strcmp(key,"opt_bb_end_instrs")) {
        handle_list_or_item(value,bind(&ConfigCfgHandler::add_opt_bb_end_instr,this,_1));
      } else if (!strcmp(key,"split_branch_begin")) {
        handle_list_or_item(value,bind(&ConfigCfgHandler::add_split_branch_begin,this,_1));
      } else if (!strcmp(key,"split_branch_end")) {
        handle_list_or_item(value,bind(&ConfigCfgHandler::add_split_branch_end,this,_1));
      } else if (!strcmp(key,"split_branch_cancel")) {
        handle_list_or_item(value,bind(&ConfigCfgHandler::add_split_branch_cancel,this,_1));
      } else if (!strcmp(key,"exception_callbacks")) {
        handle_list_or_item(value,bind(&ConfigCfgHandler::add_except_callback,this,_1));
      } else if (!strcmp(key,"dyn_caches")) {
        handle_list_or_item(value,bind(&ConfigCfgHandler::add_dyn_caches,this,_1));
      } else if (!strcmp(key,"dyn_tlbs")) {
        handle_list_or_item(value,bind(&ConfigCfgHandler::add_dyn_tlbs,this,_1));
      } else if (!strcmp(key,"trans_const_parms")) {
        handle_list_or_item(value,bind(&ConfigCfgHandler::add_trans_const_parms,this,_1));
      } else if (!strcmp(key,"cache_miss_latency")) {
        handle_list_or_item(value,bind(&ConfigCfgHandler::add_cache_miss_latency,this,_1));
      } else if (!strcmp(key,"external_dmi_write_update")) {
        _config._external_dmi_write_update = get_bool(value,"external-dmi-update flag.",env());
      } else if (!strcmp(key,"post_sim")) {
        _config._post_sim = get_func(value,0,"post-simulation hook.");
      } else if (!strcmp(key,"compl_time")) {
        _config._compl_time = get_func(value,1,"completion-time hook.");
      } else if (!strcmp(key,"cache_line_size")) {
        _config._cache_line_size = get_uint(value,"cache line size",env());
      } else {
        unknown_key(key,value);
      }
    }

    void add_cache_miss_latency(Ptree *v)
    {
      Ptree *cn, *l;
      if (Match(v,"[ %? = %? ]",&cn,&l)) {      
        gc_string name = get_str(cn,"cache name");
        unsigned latency;
        if (get_uint(latency,l,env())) {
          _config._cache_miss_latency[name] = latency;
        } else if (get_func(l,2,0)) {
          _config._cache_miss_latency[name] = l;
        } else {
          PError(v,"Expected an integer or function for latency for cache " << name << ".");
        }
      } else {
        PError(v,"Expected a list of pairs of form (cache-name,latency) for the cache-miss-latency parameter.");
      }
    }

    void add_bb_end_instr(Ptree *v)
    {
      gc_string n = get_str(v,"instruction for a forced basic-block end.");
      if (!_core.get_instr(n) && (Data::get_attr(n) < 0)) {
        PError(v,"Unknown instruction or attribute:  " << n);
      }
      _config._bb_end_instrs.insert(n);
    }

    void add_opt_bb_end_instr(Ptree *v)
    {
      gc_string n = get_str(v,"instruction for an optional basic-block end.");
      if (!_core.get_instr(n) && (Data::get_attr(n) < 0)) {
        PError(v,"Unknown instruction or attribute:  " << n);
      }
      _config._opt_bb_end_instrs.insert(n);
    }

    void add_split_branch_begin(Ptree *v)
    {
      gc_string n = get_str(v,"instruction for the first half of a split-branch pair.");
      if (!_core.get_instr(n) && (Data::get_attr(n) < 0)) {
        PError(v,"Unknown instruction or attribute:  " << n);
      }
      _config._split_branch_begin.insert(n);
    }

    void add_split_branch_end(Ptree *v)
    {
      gc_string n = get_str(v,"instruction for the second half of a split-branch pair.");
      if (!_core.get_instr(n) && (Data::get_attr(n) < 0)) {
        PError(v,"Unknown instruction or attribute:  " << n);
      }
      _config._split_branch_end.insert(n);
    }

    void add_split_branch_cancel(Ptree *v)
    {
      gc_string n = get_str(v,"instruction to cancel a split-branch pair.");
      if (!_core.get_instr(n) && (Data::get_attr(n) < 0)) {
        PError(v,"Unknown instruction or attribute:  " << n);
      }
      _config._split_branch_cancel.insert(n);
    }

    void add_reg_callback(Ptree *rcb)
    {
      gc_string r = get_ident(rcb,"register-name for a callback.");
      if (!_core.get_regitem(r)) {
        PError(rcb,"Unknown identifier specified for a register callback:  " << r);
      }
      _config._reg_callbacks.insert(r);
    }

    void add_reg_read_callback(Ptree *rcb)
    {
      gc_string r = get_ident(rcb,"register-name for a read callback.");
      if (!_core.get_regitem(r)) {
        PError(rcb,"Unknown identifier specified for a register read callback:  " << r);
      }
      _config._reg_read_callbacks.insert(r);
    }

    void add_except_callback(Ptree *rcb)
    {
      gc_string r = get_ident(rcb,"exception-name for a callback.");
      if (!_core.get_exception(r)) {
        PError(rcb,"Unknown identifier specified for an exception callback:  " << r);
      }
      _config._except_callbacks.insert(r);
    }

    void add_dyn_caches(Ptree *rcb)
    {
      gc_string c = get_ident(rcb,"cache name.");
      if (!_core.get_cache(c)) {
        PError(rcb,"Unknown identifier specified for a cache name:  " << c);
      }
      _config._dyn_caches.insert(c);
    }

    void add_dyn_tlbs(Ptree *rcb)
    {
      gc_string c = get_ident(rcb,"mmu tlb name.");
      if (!_core.get_mmulookup(c)) {
        PError(rcb,"Unknown identifier specified for an mmu tlb:  " << c);
      }
      _config._dyn_tlbs.insert(c);
    }

    void add_trans_const_parms(Ptree *v)
    {
      gc_string c = get_ident(v,"architectural-parameter name.");
      if (!_core.get_parm(c)) {
        PError(v,"Unknown identifier specified for an architectural parameter:  " << c);
      }
      _config._trans_const_parms.insert(c);
    }

  };

  class MemoryCfgHandler : public RemoveHandler {
  public:
    MemoryCfgHandler(Environment *env  ,Ptree *loc, 
                     const gc_string &name,BaseConfig& config,bool modify) :
      RemoveHandler(env, loc, name, modify),
      _config(config)
    {};
    
    virtual void add_key(const char *key, Ptree *value) {
      if (!strcmp(key,"extern_mem")) {
        _config.set_extern_mem(_name, get_bool(value,"extern-mem flag.",env()));
      } else {
        unknown_key(key,value);
      }
    }

  private:
    BaseConfig    &_config;
  };

  // Processes instruction groups.
  // Note:  Currently implemented as just a one-pass process.
  class GroupCfgHandler : public BaseRemoveCfgHandler {
  public:
    GroupCfgHandler(Environment *env,Ptree *loc,
                    const gc_string &name,CoreConfig &config,const Core &core,
                    CoreConfig::Group &group,bool modify) : 
      BaseRemoveCfgHandler(env,loc,config,core,name,modify), 
      _group(group)
    {};

    virtual void add_key(const char *key,Ptree *value)
    {
      if (!strcmp(key,"items")) {
        handle_list_or_item(value,bind(&GroupCfgHandler::add_group_item,this,_1));
      } else if (!strcmp(key,"stages")) {
        _group._stages.clear();
        handle_list_or_item(value,bind(&GroupCfgHandler::add_stage,this,_1));
        check_stages(value);
      } else if (!strcmp(key,"allow_issue")) {
        _group._allow_issue._p = get_func(value,1,"allow-issue hook.");
      } else if (!strcmp(key,"latency")) {
        _group._latency = get_uint(value,"latency flag",env());
      } else {
        unknown_key(key,value);
      }
    }
  private:

    // This is called for each item in the list given to the "items" key.
    void add_group_item(Ptree *item)
    {
      gc_string mname = get_ident(item,"group member name");

      if (mname == "default_instruction") {
        _config._default_grp = &_group;
      } else {

        // Make sure it's a recognized name.
        check_valid(item,mname,_core);

        // Add to the temporary list of items.
        _group._items.push_back(mname);
      }
    }

    // Do any checks after processing the stage list.
    void check_stages(Ptree *loc)
    {
      int mem_count = 0;
      ForEach(_group._stages,i) {
        const CoreConfig::Stage &stage = *i;
        if (!stage.tag().empty()) {
          const gc_string &tag = stage.tag();
          if ( tag == "exec") {
            ++mem_count;
          } else if ( tag == "issue" || tag == "compl" ) {
            ;
          } else {
            PError(loc,"Unknown stage tag encountered:  " << tag);
          }
        }
      }

      if (mem_count > 1) {
        PError(loc,"Found more than one memory-access tag in the stage list.");
      }
    }

    // This is called for each item in a stage list.
    void add_stage(Ptree *p)
    {
      // A stage can just be a simple name, or stage=X, where X is a special
      // tag.
      Ptree *s, *l;
      CoreConfig::Stage stage;
      if (Match(p,"[ %? = %? ]",&s,&l)) {
        stage._name = get_str(s,"stage name");
        stage._tag = get_str(l,"stage tag");
      } else {
        stage._name = get_str(p,"stage name");
      }

      _group._stages.push_back(stage);
    }

    CoreConfig::Group &_group;
  };

  // Processes Graphite instruction groups.
  class GraphiteGroupCfgHandler : public BaseRemoveCfgHandler {
  public:
    GraphiteGroupCfgHandler(Environment *env,Ptree *loc,
                            const gc_string &name,CoreConfig &config,const Core &core,
                            CoreConfig::GraphiteGroup &group,bool modify) : 
      BaseRemoveCfgHandler(env,loc,config,core,name,modify), 
      _group(group)
    {};

    virtual void add_key(const char *key,Ptree *value)
    {
      if (!strcmp(key,"items")) {
        handle_list_or_item(value,bind(&GraphiteGroupCfgHandler::add_group_item,this,_1));
      } else {
        unknown_key(key,value);
      }
    }
  private:

    // This is called for each item in the list given to the "items" key.
    void add_group_item(Ptree *item)
    {
      gc_string mname = get_ident(item,"group member name");

      if (mname == "default_instruction") {
        _config._default_graphite_grp = &_group;
      } else {

        // Make sure it's a recognized name.
        check_valid(item,mname,_core);

        // Add to the temporary list of items.
        _group._items.push_back(mname);
      }
    }

    CoreConfig::GraphiteGroup &_group;
  };

  // A core node contains information configuring a core.
  class CoreCfgHandler : public BaseRemoveCfgHandler {
  public:
    CoreCfgHandler(Environment *env,Ptree *loc,
                   CoreConfig &config,const Core &core,bool modify) : 
      BaseRemoveCfgHandler(env,loc,config,core,core.name(),modify) 
    {};

    virtual Handler *add_define(MsgLog &ml,Environment *e,Ptree *p,
                                const char *type,const char *name,bool modify)
    {
      if (!strcmp(type,"config")) {
        return new ConfigCfgHandler(e,p,_config,_core,modify);
      } else if (!strcmp(type,"group")) {
        if (!name) {
          PError(p,"Groups require a name.");
        }
        // If we're modifying a group, then we insert it only if it doesn't exist.
        // If we've got a define, then we overwrite it.
        CoreConfig::Group &group = (modify) ? 
          _config._groups.insert(make_pair(name,CoreConfig::Group())).first->second :
          (_config._groups[name] = CoreConfig::Group());
        group._name = name;
        return new GroupCfgHandler(e,p,name,_config,_core,group,modify);
      } else if (!strcmp(type,"graphite_group")) {
        if (!name) {
          PError(p,"Graphite groups require a name.");
        }
        // If we're modifying a group, then we insert it only if it doesn't exist.
        // If we've got a define, then we overwrite it.
        CoreConfig::GraphiteGroup &group = (modify) ? 
          _config._graphite_groups.insert(make_pair(name,CoreConfig::GraphiteGroup())).first->second :
          (_config._graphite_groups[name] = CoreConfig::GraphiteGroup());
        group._name = name;
        return new GraphiteGroupCfgHandler(e,p,name,_config,_core,group,modify);
      } else if (!strcmp(type,"mem")) {
        // Check that the memory exists within the core, then proceed
        if (_core.get_memory(name)) {
          return new MemoryCfgHandler(e,p,name,_config,modify);
        } else {
          PError(p,"Memory '" << name << "' does not exist in the ADL description within core " << _core.name() << ".");	      }    
      } else {
        PError(p,"A define-block of type '" << type << "' is not allowed within this block.");
      }
    }
    
  };

  // A system node simply handles hierarchy- it can contain cores or other
  // system nodes.
  class SysCfgHandler : public RemoveHandler {
  public:
    SysCfgHandler(Environment *env,Ptree *loc,
                  ConfigDB::Node &n,const System &sys,bool modify) : 
      RemoveHandler(env,loc,sys.name(),modify), 
      _node(n), 
      _sys(sys)
    {};

    virtual Handler *add_define(MsgLog &ml,Environment *e,Ptree *p,
                                const char *type,const char *name,bool modify)
    {
      if (!strcmp(type,"core")) {
        // Core node- check that this exists in the hierarchy and then proceed.
        if (const Core *core = _sys.get_core(name)) {
          return new CoreCfgHandler(e,p,make_core_node(p,_node,name,*core),*core,modify);
        } else {
          PError(p,"Core '" << name << "' does not exist in the ADL description within system " << _sys.name() << ".");
        }      
      } else if (!strcmp(type,"sys")) {
        // System node- check that this exists in the hierarchy and then proceed.
        if (const System *sys = _sys.get_sys(name)) {
          return new SysCfgHandler(e,p,*make_sys_node(p,_node,name,*sys),*sys,modify);
        } else {
          PError(p,"System '" << name << "' does not exist in the ADL description within system " << _sys.name() << ".");
        }
      } else if (!strcmp(type,"mem")) {
        // Core node - check that is exists in hierarchi and then proceed
        if(_sys.get_memory(name)) {
          return new MemoryCfgHandler(e,p,name,*_node._sysConfig,modify);
        } else {
          PError(p,"Memory '" << name << "' does not exist in the ADL description within system " << _sys.name() << ".");
        }
      } else {
        PError(p,"A define-block of type '" << type << "' is not allowed within this block.");
      }
    }

    virtual void add_key(const char *key,Ptree *value)
    {
      unknown_key(key,value);
    }

  private:
    ConfigDB::Node &_node;
    const System   &_sys;
  };

  // Processes core configuration groups.
  class OuterConfigCfgHandler : public Handler {
  public:
    OuterConfigCfgHandler(Environment *env,Ptree *loc,
                          ConfigDB &config,bool modify) : 
      Handler(env,loc),
      _config(config)
    {
      _modify = modify;
    };

    virtual void add_key(const char *key,Ptree *value)
    {
      if (!strcmp(key,"trace_mode")) {
        _config._trace_mode = get_bool(value,"trace-mode flag.",env());
      } else if (!strcmp(key,"debug_mode")) {
        _config._debug_mode = get_bool(value,"debug-mode flag.",env());
      } else if (!strcmp(key,"verbose_mode")) {
        _config._verbose_mode = get_bool(value,"verbose-mode flag.",env());
      } else if (!strcmp(key,"line_directives")) {
        _config._ld_okay = get_bool(value,"line-directive insertion flag.",env());
      } else if (!strcmp(key,"time_tagged")) {
        _config._time_tagged = get_bool(value,"time-tagged flag.",env());
      } else if (!strcmp(key,"preamble")) {
        handle_list_or_item(value,bind(&OuterConfigCfgHandler::add_preamble_gc_string,this,_1));
      } else if (!strcmp(key,"extern_mem")) {
        _config._extern_mem=get_bool(value,"extern-mem flag.",env());
      } else if (!strcmp(key,"extern_dmi")) {
        _config._extern_dmi=get_bool(value,"extern-dmi flag.",env());
      } else if (!strcmp(key,"extern_dmi_entry_size")) {
        _config._extern_dmi_entry_size=get_uint(value,"extern-dmi entry-size value.",env());
      } else if (!strcmp(key,"extern_dmi_size")) {
        _config._extern_dmi_size=get_uint(value,"extern-dmi size value.",env());
      } else if (!strcmp(key,"rnumber")) {
        _config._rnumber_support=get_bool(value,"rnumber support flag.",env());
      } else if (!strcmp(key,"dmi_cache")) {
        _config._dmi_cache_support=get_bool(value,"DMI (direct-memory interface) cache support flag.",env());
      } else if (!strcmp(key,"dmi_cache_size")) {
        _config._dmi_cache_size=get_uint(value,"DMI (direct-memory interface) cache size.",env());
      } else if (!strcmp(key,"icache_touch")) {
        _config._icache_touch= (get_bool(value,"Instruction-cache-touch flag.",env()) ? tTrue : tFalse);
      } else if (!strcmp(key,"jit")) {
        _config._jit_support=get_bool(value,"JIT support flag.",env());
      } else if (!strcmp(key,"jit_ea_hash_size")) {
        _config._jit_ea_hash_size = get_uint(value,"JIT EA hash size.",env());
      } else if (!strcmp(key,"track_reads")) {
        _config._track_reads = get_bool(value,"read-tracking flag.",env());
      } else if (!strcmp(key,"log_reg_reads")) {
        _config._log_reg_reads = get_bool(value,"register-read-logging flag.",env());
      } else if (!strcmp(key,"log_unarch")) {
        _config._log_unarch = get_bool(value,"unarchitected-resource logging flag.",env());
      } else if (!strcmp(key,"log_reg_masks")) {
        _config._log_reg_masks = get_bool(value,"register read/write partial masks flag.",env());
      } else if (!strcmp(key,"syscall_enabled")) {
        _config._syscall_enabled = get_bool(value,"system-call enable flag.",env());
      } else if (!strcmp(key,"iss_type")) {
        gc_string n = get_simple_ident(value,"ISS type.");
        if (n == "normal") {
          _config._iss_type = tNormal;
        }
        else if (n == "hybrid") {
          _config._iss_type = tHybrid;
        }
        else if (n == "transactional") {
          _config._iss_type = tTransactional;
        }
        else {
          PError(value,"Unknown ISS type:  " << n);
        }
      } else if (!strcmp(key,"trans_pool_size")) {
        _config._trans_pool_size=get_uint(value,"transactional ISS instruction packet pool size.",env());
      } else if (!strcmp(key,"mem_if")) {
        gc_string n = get_simple_ident(value,"memory-interface type.");
        if (n == "nonblocking") {
          _config._mem_if = mNonBlocking;
        }
        else if (n == "blocking") {
          _config._mem_if = mBlocking;
        }
        else if (n == "logging") {
          _config._mem_if = mLogging;
        }
        else {
          PError(value,"Unknown memory interface type:  " << n);
        }
      } else if (!strcmp(key,"coro_pool_size")) {
        _config._coro_pool_size=get_uint(value,"transactional ISS coroutine pool size.",env());
      } else if (!strcmp(key,"iss_namespace")) {
        _config._iss_namespace = get_str(value,"iss namespace.");
      } else if (!strcmp(key,"num_compile_jobs")) {
        _config._num_compile_jobs = get_uint(value,"number of compile jobs.",env());
      } else if (!strcmp(key,"max_delay_slots")) {
        _config._max_delay_slots = get_uint(value,"maximum number of delay slots.",env());
      } else if (!strcmp(key,"packet_pool_size")) {
        _config._packet_pool_size = get_uint(value,"maximum number of elements in the delayed-register-write pool objects.",env());
      } else if (!strcmp(key,"rollback_mode")) {
        _config._rollback_mode = get_bool(value,"rollback-mode flag.",env());
      } else if (!strcmp(key,"ignore_mode")) {
        _config._ignore_mode = get_bool(value,"ignore-mode flag.",env());
      } else if (!strcmp(key,"gen_instr_info")) {
        _config._gen_instr_attr = get_bool(value,"the generate-instructions-info flag.",env());
      } else if (!strcmp(key,"disassembler")) {
        _config._disassembler = get_bool(value,"the disassembler flag.",env());
      } else if (!strcmp(key,"lib_mode")) {
        _config._library_mode = get_bool(value,"the lib-mode flag.",env());
      } else if (!strcmp(key,"event_bus_send")) {
        _config._event_bus_send = get_bool(value,"event bus send/transmit flag.",env());
      } else if (!strcmp(key,"external_event_bus")) {
        _config._extern_event_bus = get_bool(value,"external event bus flag.",env());
      } else if (!strcmp(key,"td_api")) {
        _config._td_api = get_bool(value,"temporal-decoupling flag.",env());
      } else if (!strcmp(key,"dep_tracking")) {
        _config._dep_tracking = get_bool(value,"data-dependency tracking flag.",env());
      } else if (!strcmp(key,"tlm2_endianness")) {
        _config._tlm2_endianness = get_bool(value,"TLM2 endianness flag.",env());
      } else if (!strcmp(key,"per_instr_td_incr")) {
        _config._per_instr_td_incr = get_bool(value,"Per-instruction temporal-decoupling counter increment flag.",env());
      } else if (!strcmp(key,"mt_support")) {
        _config._mt_support = get_bool(value,"Multi-threaded support flag.",env());
      } else if (!strcmp(key,"mt_rw_locked_mem")) {
        _config._mt_rw_locked_mem = get_bool(value,"Locked memory read/write flag.",env());
      } else if (!strcmp(key,"mt_locked_mem")) {
        _config._mt_locked_mem = get_bool(value,"Locked memory access flag.",env());
      } else if (!strcmp(key,"gen_full_regs")) {
        handle_list_or_item(value,bind(&OuterConfigCfgHandler::add_gen_full_regs,this,_1));
      } else if (!strcmp(key,"gen_all_reg_code")) {
        _config._gen_all_reg_code = get_bool(value,"generate-full-regster-code flag.",env());
      } else if (!strcmp(key,"log_td_commit")) {
        _config._log_td_commit = get_bool(value,"log completion using temporal-decoupling counter flag.",env());
      } else if (!strcmp(key,"tags_only_caches")) {
        _config._tags_only_caches = get_bool(value,"tag-only caches flag.",env());
      } else if (!strcmp(key,"cpp_cmds")) {
        handle_list_or_item(value,bind(&OuterConfigCfgHandler::add_cpp_cmd,this,_1));
      } else {
        unknown_key(key,value);
      }
    }

  private:

    void add_cpp_cmd(Ptree *value)
    {
      gc_string tmp = get_str(value,"C preprocessor command");
      _config._cpp_cmds.push_back(tmp);
    }
    
    void add_preamble_gc_string(Ptree *value)
    {
      gc_string tmp = get_str(value,"preamble data.");
      _config._preamble += tmp + '\n';
    }

    void add_gen_full_regs(Ptree *v)
    {
      gc_string r = get_ident(v,"register name.");
      _config._gen_full_regs.insert(r);
    }

    ConfigDB &_config;
  };

  // Outer handler class.
  class OuterCfgHandler : public Handler {
  public:
    OuterCfgHandler(Environment *env,Ptree *loc,
                    ConfigDB &config,const Data &data) : 
      Handler(env,loc), 
      _config(config), 
      _data(data) 
    {};

    virtual Handler *add_define(MsgLog &ml,Environment *e,Ptree *p,
                                const char *type,const char *name,bool modify)
    {
      if (!strcmp(type,"config")) {
        return new OuterConfigCfgHandler(e,p,_config,modify);
      }
      else if (!strcmp(type,"core")) {
        // Core node- check that this exists in the hierarchy and then proceed.
        if (const Core *core = _data.get_core(name)) {
          return new CoreCfgHandler(e,p,make_core_node(p,_config._top,name,*core),*core,modify);
        } else {
          PError(p,"Core '" << name << "' does not exist in the ADL description.");
        }      
      } 
      else if (!strcmp(type,"sys")) {
        // System node- check that this exists in the hierarchy and then proceed.
        if (const System *sys = _data.get_sys(name)) {
          return new SysCfgHandler(e,p,*make_sys_node(p,_config._top,name,*sys),*sys,modify);
        } else {
          PError(p,"System '" << name << "' does not exist in the ADL description.");
        }
      } else {
        PError(p,"A define-block of type '" << type << "' is not allowed within this block.");
      }
    }

  private:
    ConfigDB   &_config;
    const Data &_data;
  };


  ConfigParser::ConfigParser(ConfigDB &config,const Data &data,bool check_aux_funcs) :
    _config(config),
    _data(data),
    _check_aux_funcs(check_aux_funcs)
  {
  }

  Handler *ConfigParser::make_initial_handler(Environment *env)
  {
    return new OuterCfgHandler(env,0,_config,_data);
  }

  // If desired, check any auxiliary functions added from the config file.
  void ConfigParser::second_pass(MsgLog &msglog)
  {
    _config.finalize(msglog,_check_aux_funcs);
  }

}

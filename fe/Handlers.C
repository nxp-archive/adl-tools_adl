//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Various Handler class implementations.
//

#include <iostream>
#include <cstring>
#include <cassert>

#include "opencxx/parser/token-names.h"
#include "opencxx/parser/Ptree.h"
#include "opencxx/Environment.h"
#include "opencxx/parser/PtreeArray.h"
#include "opencxx/PtreeIter.h"

#include "helpers/Macros.h"
#include "helpers/fe_exceptions.h"
#include "helpers/BasicTypes.h"

#include "Helpers.h"
#include "ParsingHelpers.h"
#include "Data.h"
#include "Handlers.h"
#include "MsgLog.h"

using namespace std;
using namespace std::placeholders;
using namespace Opencxx;
using namespace PtreeUtil;

namespace adl {

  const char *AllItems = "*";

  // Special override for instructions only.  In addition to not adding a
  // warning on strict mode, we don't remove dummy instructions, b/c of the
  // weird behavior where dummies are needed for later, for proper removal
  // purposes.
  template<>  
  bool check_remove <InstrHandler,InstrHash>(MsgLog &ml,InstrHandler &h,InstrHash &c)
  {
    if (h.remove()) {
      // Remove the item if the remove flag was set.
      InstrHash::iterator iter;
      if ( (iter = c.find(h.name())) != c.end() && !iter->second->is_dummy()) {
        c.erase(iter);
        return true;
      } 
    }
    return false;
  }


  
  template <class T,class H,class C,class Outer>
  void process_instrs(MsgLog &log,H &handler,C &container,Outer &outer,void (Outer::*setup)(H&,T&),int num)
  {
    try {
      if (!check_remove(log,handler,container)) {
        // Remove not set, so add the item if not a modify, otherwise, require
        // that it exist.  Then process all keys.
        T *item = 0;
        if (handler.modify()) {
          typename C::iterator iter;
          if ( (iter = container.find(handler.name())) == container.end()) {
            item = new T(handler.env(),handler.loc(),handler.name(),num,handler.remove());
            container[handler.name()] = item;
          } else {
            item = iter->second;
          }
          report_modify(log,handler,"Instruction");
        } else {
          // This is a define, so we replace the object if it already exists.
          item = new T(handler.env(),handler.loc(),handler.name());
          // Warn if we're replacing anything, since this sometimes indicates a
          // subtle error, though it's perfectly legal, and sometimes very
          // useful, to do this.
          auto ip = container.insert(make_pair(handler.name(),item));
          if (!ip.second) {
            if (DataWarnRedefineMode()) {
              AddWarning(log,handler.loc(),"Item '" << handler.name() << "' was redefined.");
              AddWarning(log,ip.first->second->loc(),"    Previous definition was here.");
            }
            ip.first->second = item;
          }
        }
        (outer.*setup)(handler,*item);
        handler.process_keys(log);    
      } 
    }
    catch (parse_error &err) {
      log.add(err);
    }
  }


  // If not only instructions will have dummy objects its worth to merge this two fuctions.
  template <class HC,class DC,class H,class T,class Outer>
  void process_instr_handler(MsgLog &log,HC &handler_container,
                             DC &dest_container,Outer &outer,
                             void (Outer::*setup)(H&,T&))
  {
    int num = 0;
    check_for_remove_keys(handler_container);
    for (typename HC::iterator i = handler_container.begin(); i != handler_container.end(); ) {
      if ( (*i)->has_expansion() ) {
        i = process_expansion(*(*i),handler_container,i);
      } else {
        process_instrs(log,*(*i),dest_container,outer,setup,num++);
        ++i;
      }
    }
  }  

  int get_attr_checked(Ptree *loc,const gc_string &n,AttrParams::Type ptype = AttrParams::NoParam,
                       bool require_valid_item = true)
  {
    try {
      return Data::get_attr(n,ptype,require_valid_item);
    }
    catch (runtime_error &err) {
      PError(loc,err.what());
    }
  }

  AttrItem parse_attr_item(Ptree *p,bool require_valid_attr,Environment *env)
  {
    AttrItem aitem;
    if(const char *n = get_ident(p,0)) {   
      // Unparametrized attribute
      aitem._attr = get_attr_checked(p,n,AttrParams::NoParam,require_valid_attr);
    } else { 
      // Must be functional call, otherwise.
      Ptree *name, *params;           
      if (Match(p,"[ %? ( [%?] ) ]",&name,&params)) {
        Opencxx::Ptree *l;
        if ( (l = get_flat_list(params)) ) {
          if(is_int(PtreeUtil::First(l),env)) {
            aitem._params._type = AttrParams::IntList;	    
            handle_list_or_item(params, bind(&AttrParams::record_int_param,&aitem._params,_1,env));  
          } else {
            aitem._params._type = AttrParams::StrList;	    
            handle_list_or_item(params, bind(&AttrParams::record_str_param,&aitem._params,_1));	    
          }
        } else {
          if(is_int(params,env)) {	
            aitem._params._type = AttrParams::Int;
            aitem._params = AttrParams(get_uint(params,"integer attribute parameter.",env));
          } else {
            aitem._params._type = AttrParams::Str;
            aitem._params = AttrParams(get_str(params,"string attribute parameter."));
          }
        }  
        aitem._attr = get_attr_checked(name,get_ident(name,"attribute name."),aitem._params._type,
                                       require_valid_attr);
      } else {
        PError(p,"Error in parameters of class." );
      }  
    }
    return aitem;
  }

  template <typename A>
  void add_an_attr(Ptree *p,A &arg,Environment *env)
  {
    if (!arg.add_attr(parse_attr_item(p,true,env))) {
      PError(p,"Error adding attribute " << p);
    }
  }

  template <typename A>
  void remove_an_attr(Ptree *p,A &arg)
  {
    if (!arg.add_attr(AttrItem(get_attr_checked(p,get_ident(p,"attribute to remove."))),true,true)) {
      PError(p,"Error removing attribute " << p);
    }
  }

  // No checking in this version.
  template <typename A>
  void remove_an_attr_nc(Ptree *p,A &arg)
  {
    int rc = Data::get_attr(get_ident(p,"attribute to remove."));
    if (rc >= 0) {
      arg.add_attr(AttrItem(rc),true,false);
    }
  }

  // Handles the "attrs" key:  Calls add_class on the argument and returns true,
  // else returns false if the key doesn't match.  Throws a parse_error on bad data.
  // Also handles the doc_title key if the supplied T is a Named object.
  template <class T>
  bool handle_attrs(const char *key,Ptree *value,T &h,Environment *env)
  {
    if (!strcmp(key,"inherit_attrs")) {
      h.set_inherit_attrs(get_bool(value,"attribute-inherit flag.",env));
      return true;
    }
    else if (!strcmp(key,"attrs") || !strcmp(key,"add_attrs")) {
      // Handle attributes.
      handle_list_or_item(value,bind(add_an_attr<T>,_1,ref(h),env));
      return true;
    } 
    else if (!strcmp(key,"remove_attrs")) {
      handle_list_or_item(value,bind(remove_an_attr<T>,_1,ref(h)));
      return true;
    }  
    else if (!strcmp(key,"remove_attrs_nc")) {
      handle_list_or_item(value,bind(remove_an_attr_nc<T>,_1,ref(h)));
      return true;
    }  
    else if (!strcmp(key,"doc_title")) {
      if (auto n = dynamic_cast<Named*>(&h)) {
        n->setDocName(get_str(value,"custom title for documentation."));
        return true;
      } else {
        PError(value,"This object does not accept the doc_title key.");
      }
    }
    else if (!strcmp(key,"doc_title_add")) {
      if (auto n = dynamic_cast<Named*>(&h)) {
        n->setDocNameAdd(get_str(value,"additional title for documentation."));
        return true;
      } else {
        PError(value,"This object does not accept the doc_title_add key.");
      }
    }
    else {
      return false;
    }
  }

  //
  // OuterHandler:  Responsible for accepting defines at the outer-most scope.
  //

  OuterHandler::OuterHandler(Opencxx::Environment *e,Opencxx::Ptree *p) : 
    Handler(e,p), 
    _data(0),
    _systems(new SysHandlerHash),
    _cores(new CoreHandlerHash),
    _archs(new ArchHandlerHash)
  {
  }

  ArchHandler *make_arch(MsgLog &ml,ArchHandlerHash *archs,Environment *e,Ptree *p,const char *name,bool modify)
  {
    if (!name) {
      PError(p,"Architecture defined without a name.");
    }
    // Do the usual checks- defmod requires that it exist, defines will
    // overwrite.
    ArchHandler *a = 0;
    if (modify) {
      // If we're modifying, then see if the object already exists.
      auto iter = archs->find(name);
      if (iter == archs->end()) {
        PError(p,"Attempt to modify architecture '" << name << "' which has not yet been defined.");
      } else {      
        a = iter->second;
      }
      report_modify(ml,*a,"Architecture");
      a->_dm_envs.push_back(e);
    } else {
      // Not modifying, so we're going to overwrite.
      (*archs)[name] = (a = new ArchHandler(e,p,name,modify));
    }
    return a;
  }

  CoreHandler *make_core(MsgLog &ml,CoreHandlerHash *cores,Environment *e,Ptree *p,const char *name,bool modify)
  {
    check_ident(name,p,"core");
    CoreHandler *c = 0;
    if (modify) {
      // If we're modifying, then see if the object already exists.
      auto iter = cores->find_local(name);
      if (iter == cores->end()) {
        PError(p,"Attempt to modify core '" << name << "' which has not yet been defined.");
      } else {      
        c = iter->second;
      }
      report_modify(ml,*c,"Core");
      // Add on this environment so that we can later add a parent reference in
      // it.
      c->_dm_envs.push_back(e);
    } else {
      // Not modifying, so we're going to overwrite.
      (*cores)[name] = (c = new CoreHandler(e,p,name,modify));
    }
    return c;
  }

  SysHandler *make_system(MsgLog &ml,SysHandlerHash *systems,CoreHandlerHash *cores,
                          Environment *e,Ptree *p,const char *name,bool modify)
  {
    check_ident(name,p,"system");
    // Does the system already exist?  If so, it's an error.  Otherwise,
    // add the system.
    SysHandler *s;
    if (modify) {
      auto iter = systems->find_local(name);
      if (iter == systems->end()) {
        PError(p,"Attempt to modify system '" << name << "' which has not yet been defined.");
      } else {
        s = iter->second;
        report_modify(ml,*s,"system");
      }
    } else {
      (*systems)[name] = (s = new SysHandler(e,p,systems,cores,name,modify));
    }
    return s;
  }

  void process_cores(MsgLog &log,ArchHandlerHash *archs,CoreHandlerHash &chandlers,
                     CoreHash &cores)
  {
    // In the first phase, instantiate or remove all cores.
    auto i = chandlers.begin();
    while (i != chandlers.end()) {
      CoreHandler &ch = *(i->second);
      if (!check_remove(log,ch,cores)) {
        Core *c = new Core(ch.env(),ch.loc(),i->first);
        auto ip = cores.insert(make_pair(i->first,c));
        assert(ip.second);
        ch.setCore(c);
        ch._archs = archs;
        // So that we can see helper functions across all architectures, add on
        // all architecture environments to the core's environment.  Likewise,
        // each architecture's environment then gets a parent-object pointer to
        // the core.  We have to do this for the main architecture object, as
        // well as any defmod environments that came along.
        ForEach((*archs),i) {
          ArchHandler &a = *i->second;
          c->env()->AddBaseclassEnv(a.env());
          addParentObj(a.env(),c);
          ForEach(a._dm_envs,j) {
            c->env()->AddBaseclassEnv(*j);
            addParentObj(*j,c);
          }
        }
        // Then add on parent references for any core-handler defmod
        // environments.
        ForEach(ch._dm_envs,i) {
          addParentObj(*i,c);
        }
      } else {
        // If we're removing, then also remove from the handlerhash so that we don't
        // find it if we look up a core in the hash later.
        if (Data::strictMode()){
          AddWarning(log,ch.loc(), "Core '" << ch.name() << "' is removed");
        }
        chandlers.erase(i++);
        continue;
      }
      ++i;
    }

    // Now we process the keys. 
    ForEach(chandlers,i) {
      CoreHandler &ch = *(i->second);
      ch.process_keys(log);
    }
  }

  void process_systems(MsgLog &log,ArchHandlerHash *archs,
                       SysHandlerHash &shandlers,SysHash &systems)
  {
    // In the first phase, instantiate or remove all systems
    auto i = shandlers.begin();
    while (i != shandlers.end()) {
      SysHandler &sh = *(i->second);
      if (!check_remove(log,sh,systems)) {
        System *s = new System(sh.env(),sh.loc(),i->first);
        auto ip = systems.insert(make_pair(i->first,s));
        assert(ip.second);
        sh._archs = archs;
        sh._sys = s;
      } else {
        // If we're removing, then also remove from the handlerhash so that we don't
        // find it if we look up a core in the hash later.
        if (Data::strictMode()){
          AddWarning(log,sh.loc(), "System '" << sh.name() << "' is removed");
        }
        shandlers.erase(i++);
        continue;
      }
      ++i;
    }

    // Now process the keys.  The two-phase process means that we won't have an ordering
    // issue when handling declarations in the systems.
    ForEach(shandlers,i) {
      SysHandler &sh = *(i->second);
      sh.process_keys(log);
    }
  }

  Handler *OuterHandler::add_define(MsgLog &ml,Environment *e,Ptree *p,const char *type,
                                    const char *name,bool modify)
  {
    if (!strcmp(type,"arch")) {
      return make_arch(ml,_archs,e,p,name,modify);
    }
    else if (!strcmp(type,"core")) {
      return make_core(ml,_cores,e,p,name,modify);
    }
    else if (!strcmp(type,"sys")) {
      return make_system(ml,_systems,_cores,e,p,name,modify);
    }
    else {
      unknown_define(p,type,name);
    }
    abort();
  }

  // This scans through all cores and systems, instantiating them in the data
  // model.
  void OuterHandler::process_keys(MsgLog &log)
  {
    // This processes the keys for all cores and systems.
    process_cores(log,_archs,*_cores,_data->_cores);
    process_systems(log,_archs,*_systems,_data->_systems);
  }


  //
  // AspectHandler: Responsible for processing aspect parameters.
  //

  void AspectHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"labels")) {
      handle_list_or_item(value,bind(&AspectHandler::add_label,this,_1));
    }
    else if (!strcmp(key,"attrs")) {
      handle_list_or_item(value,bind(&AspectHandler::add_attr,this,_1));
    }
    else if (!strcmp(key,"instr_pre_exec")) {
      _aspect->_instr_pre_exec = get_bool(value,"instruction pre-exec flag.",env());
    }
    else if (!strcmp(key,"instr_post_exec")) {
      _aspect->_instr_post_exec = get_bool(value,"instruction post-exec flag.",env());
    }
    else if (!strcmp(key,"exclusive")) {
      _aspect->_exclusive = get_bool(value,"exclusive flag",env());
    }
    else if (!strcmp(key,"priority")) {
      _aspect->_priority = get_uint(value,"priority value",env());
    }
    else if (!strcmp(key,"if_defined")) {
      handle_list_or_item(value,bind(&AspectHandler::add_if_def,this,_1));
    }
    else if (!strcmp(key,"action")) {
      // action:  Should be a 0-ary function.
      _aspect->_action = get_func(value,0,"aspect action.");
    } 
    else {
      unknown_key(value,key);
    }
  }

  void AspectHandler::add_attr(Ptree *p)
  {
    _aspect->_attrs.push_back(parse_attr_item(p,false,env()));
  }

  void AspectHandler::add_if_def(Ptree *p)
  {
    _aspect->_def_checks.push_back(p);
  }

  void AspectHandler::add_label(Ptree *p)
  {
    _aspect->_labels.insert(get_ident(p,"aspect label"));
  }

  //
  // AspectGrpHandler:  Subclass for a define block that can take aspects.
  //
  Handler *AspectGrpHandler::add_define(MsgLog &ml,Environment *e,Ptree *p,const char *type,const char *name,bool modify)
  {
    if (!strcmp(type,"aspect")) {
      check_ident(name,p,"aspect");
      auto ah = new AspectHandler(e,p,name,modify);
      _aspects.push_back(ah);
      return ah;
    } 
    else {
      return 0;
    }
  }

  void AspectGrpHandler::process_keys(MsgLog &log)
  {
    process_named_handler(log,_aspects,_a->_aspects,*this,&AspectGrpHandler::setup_aspect_handler);
  }

  void AspectGrpHandler::setup_aspect_handler(AspectHandler &ah,Aspect &a)
  {
    a._doc += ah.doc();
    ah._aspect = &a;
  }

  Handler *AspectRemoveHandler::add_define(MsgLog &ml,Environment *e,Ptree *p,const char *type,const char *name,bool modify)
  {
    if (Handler *h = AspectGrpHandler::add_define(ml,e,p,type,name,modify)) {
      return h;
    } else {
      unknown_define(p,type,name);
    }
    abort();
  }

  void AspectRemoveHandler::process_keys(MsgLog &log)
  {
    RemoveHandler::process_keys(log);
    AspectGrpHandler::process_keys(log);
  }

  void AspectRemoveHandler::get_expansion(StrSet &items)
  {
    // Should have already checked for this.
    assert(_gh);
    
    // Resolve the group at this point.
    _gh->resolve_items(&items);
  }

  Handler *AspectIntRemoveHandler::add_define(MsgLog &ml,Environment *e,Ptree *p,const char *type,const char *name,bool modify)
  {
    if (Handler *h = AspectGrpHandler::add_define(ml,e,p,type,name,modify)) {
      return h;
    } else {
      unknown_define(p,type,name);
    }
    abort();
  }

  void AspectIntRemoveHandler::process_keys(MsgLog &log)
  {
    IntRemoveHandler::process_keys(log);
    AspectGrpHandler::process_keys(log);
  }


  //
  // SysHandler:  Responsible for system blocks.
  //

  // The define operation generally replaces an existing item.
  Handler *SysHandler::add_define(MsgLog &ml,Environment *e,Ptree *p,const char *type,const char *name,bool modify)
  {
    if (!strcmp(type,"core")) {
      return make_core(ml,_cores,e,p,name,modify);
    }
    else if (!strcmp(type,"sys")) {
      return make_system(ml,_systems,_cores,e,p,name,modify);
    }
    else if (!strcmp(type,"shared")) {
      auto sh = new ShareHandler(e,p);
      _shares.push_back(sh);
      return sh;
    } 
    else {
      return AspectRemoveHandler::add_define(ml,e,p,type,name,modify);
    }
    assert(0);
  }

  void SysHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"type")) {
      _sys->_type = get_str(value,"type name.");
    } 
    else {
      RemoveHandler::handle_key(key,value);
    }
  }

  void SysHandler::handle_decl(const char *type,Opencxx::Ptree *name)
  {
    // Each declaration may refer to either a system or a core.
    // First, look for a system of the specified name.
    SysHandlerHash::iterator si;
    const char *n = get_ident(name,"system component declaration name");
    if ( (si = _systems->find(type)) != _systems->end()) {
      _sys->_items.push_back(make_pair(n,si->second->_sys));
      return;
    }
    CoreHandlerHash::iterator ci;
    if ( (ci = _cores->find(type)) != _cores->end()) {
      _sys->_items.push_back(make_pair(n,ci->second->core()));
      return;
    }
    PError(name,"Could not find a system or core of name '" << type << "'.");

  }

  void SysHandler::process_keys(MsgLog &log)
  {
    process_cores(log,_archs,*_cores,_sys->_local_cores);
    process_systems(log,_archs,*_systems,_sys->_local_systems);

    Handler::process_keys(log);

    process_unnamed_handler(log,_shares,*this,&SysHandler::setup_share_handler);
  }

  void SysHandler::setup_share_handler(ShareHandler &sh)
  {
    _sys->_share_loc = sh.loc();
    sh._sys = _sys;
  }

  // 
  // ContextHandler:  Responsible for a core's contexts, if applicable.
  //

  void ContextHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"regs")) {
      handle_list_or_item(value,bind(&ContextHandler::record_reg,this,_1));
    } 
    else if (!strcmp(key,"regfiles")) {
      handle_list_or_item(value,bind(&ContextHandler::record_regfile,this,_1));
    } 
    else if (!strcmp(key,"mmulookups")) {
      handle_list_or_item(value,bind(&ContextHandler::record_mmulookup,this,_1));
    } 
    else if (!strcmp(key,"num_contexts")) {
      _context->_num_contexts = get_uint(value,"number of contexts",env());
    } 
    else if (!strcmp(key,"mem_layout")) {
      handle_list_or_item(value,bind(&ContextHandler::record_mem_layout,this,_1));
    } 
    else if (!strcmp(key,"mem_omit")) {
      handle_list_or_item(value,bind(&ContextHandler::record_mem_omit,this,_1));
    } 
    else if (!strcmp(key,"active")) {
      _context->_active = get_func(value,0,"context-activity function");
    }
    else if (handle_attrs(key,value,*_context,env())) {
      ;
    }
    else {
      unknown_key(value,key);
    }
  }

  void ContextHandler::record_mem_layout(Ptree *p)
  {
    _context->add_mem_layout_item(get_ident(p,"context resource item"));
  }

  void ContextHandler::record_mem_omit(Ptree *p)
  {
    _context->add_mem_omit_item(get_ident(p,"context resource item"));
  }

  void ContextHandler::record_reg(Ptree *p)
  {
    gc_string name = get_ident(p,"register");
    _context->add_reg(name,_core);
  }

  void ContextHandler::record_regfile(Ptree *p)
  {
    gc_string name = get_ident(p,"register file");
    _context->add_regfile(name,_core);
  }

  void ContextHandler::record_mmulookup(Ptree *p)
  {
    gc_string name = get_ident(p,"MMU lookup");
    _context->add_mmulookup(name,_core);
  }

  bool ContextHandler::has_attr(const AttrItem &ai) const
  {
    return (_context) ? (bool)_context->has_attr(ai) : false;
  }

  //
  // ShareHandler:  Responsible for resource-sharing information
  // 

  void ShareHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"regs")) {
      handle_list_or_item(value,bind(&ShareHandler::record_shared_reg,this,_1));
    } else if (!strcmp(key,"regfiles")) {
      handle_list_or_item(value,bind(&ShareHandler::record_shared_regfile,this,_1));
    } else if (!strcmp(key,"mmulookups")) {
      handle_list_or_item(value,bind(&ShareHandler::record_shared_mmu,this,_1));
    } else if (!strcmp(key,"parms")) {
      handle_list_or_item(value,bind(&ShareHandler::record_shared_parm,this,_1));
    } else if (!strcmp(key,"caches")) {
      handle_list_or_item(value,bind(&ShareHandler::record_shared_cache,this,_1));
    } else if (!strcmp(key,"mems")) {
      handle_list_or_item(value,bind(&ShareHandler::record_shared_memory,this,_1));
    } else if (!strcmp(key,"eventbuses")) {
      handle_list_or_item(value,bind(&ShareHandler::record_shared_eventbus,this,_1));
    } else if (!strcmp(key,"ext_resources")) {
      handle_list_or_item(value,bind(&ShareHandler::record_shared_extresource,this,_1));
    } else {
      unknown_key(value,key);
    }
  }

  void ShareHandler::record_shared_cache(Ptree *p)
  {
    gc_string name = get_ident(p,"shared cache");
    _sys->add_shared_cache(name);  
  }

  void ShareHandler::record_shared_memory(Ptree *p)
  {
    gc_string name = get_ident(p,"shared memory");
    _sys->add_shared_memory(name);  
  }


  void ShareHandler::record_shared_mmu(Ptree *p)
  {
    gc_string name = get_ident(p,"shared mmu lookup");
    _sys->add_shared_mmu(name);
  }

  void ShareHandler::record_shared_reg(Ptree *p)
  {
    gc_string name = get_ident(p,"shared register");
    _sys->add_shared_reg(name);
  }

  void ShareHandler::record_shared_regfile(Ptree *p)
  {
    gc_string name = get_ident(p,"shared register file");
    _sys->add_shared_regfile(name);
  }

  void ShareHandler::record_shared_parm(Ptree *p)
  {
    gc_string name = get_ident(p,"shared parameter");
    _sys->add_shared_parm(name);
  }

  void ShareHandler::record_shared_eventbus(Ptree *p)
  {
    gc_string name = get_ident(p,"shared event bus");
    _sys->add_shared_eventbus(name);
  }

  void ShareHandler::record_shared_extresource(Ptree *p)
  {
    gc_string name = get_ident(p,"shared external resource");
    _sys->add_shared_extresource(name);
  }

  //
  // CoreHandler:  Responsible for core blocks.
  //

  // In the second pass, we figure out what architectures our design
  // consists of, then we iterate through that list, building up a single
  // data model.  Thus, architecture blocks act as "mix-ins"- they're not
  // complete by themselves, but must be taken together as a whole.
  void CoreHandler::process_keys(MsgLog &log)
  {
    _core->_doc = _doc;

    gc_list<Handler *> archs;

    // Duplicate the remove list, because we don't know if a remove command is
    // for a key in the core or in the core's architecture.
    _corearch._removes = _removes;

    bool dupl = !_removes.empty();
    
    // Handle any architectures first, before we process our keys, so that the
    // keys within the core will override anything in the architecture.  Here,
    // we just build up a list of all appropriate architectures.  
    //
    // Note that we duplicate the architectures if we have a remove list for the
    // core.  This is so that one core doesn't influence the architectures used
    // by another.  If no remove lists exist, or exist only in the
    // architectures, then we're OK.
    ForEach(_archlist,n) {
      ArchHandlerHash::iterator iter = _archs->find(get_str(*n,"an architecture name."));
      if (iter != _archs->end()) {
        ArchHandler &arch = *(iter->second);
        arch.setCore(_core);
        archs.push_back((dupl) ? new ArchHandler(arch) : &arch);
      }
      else {
        PError(*n,"Unknown architecture '" << *n << "'");
      }
    }    

    // This is the architecture within the core which handles
    // architecture-specific keys that the user can put directly into the core
    // define.  This is handled last, since it contains the final say on
    // overrides.  No need to copy it, since it's unique to the core.
    archs.push_back(&_corearch);

    // Now check for any removes that should be performed.
    check_for_remove_keys(archs);

    // Process all architectures.
    ForEach(archs,iter) {
      (*iter)->process_keys(log);
    }

    // Process any keys specific to the core.
    set_aspects(_core);
    AspectRemoveHandler::process_keys(log);

    // Then add an instruction table for each name identified.
    int count = 0;
    ForEach(_itablenames,i) {
      if (!_core->add_instr_table(*i,count++,0)) { // Add one more parameter 0 due to refactoring
        PError(loc(),"Unknown instruction class '" << *i << "'.");
      }
    }
  }

  Handler *CoreHandler::add_define(MsgLog &ml,Environment *e,Ptree *p,
                                   const char *type,const char *name,bool modify)
  {
    return _corearch.add_define(ml,e,p,type,name,modify);
  }

  void CoreHandler::add_func(const char *name,Opencxx::Ptree *p)
  {
    _corearch.add_func(name,p);
  }

  void CoreHandler::add_type_decl(Opencxx::Ptree *p)
  {
    _corearch.add_type_decl(p);
  }

  // We intercept architectures here so that we can process them before we
  // process the keys in a core.  Also, due to how we handle cores and the fact
  // that the cores can be architecture stuff in them as well, we end up with an
  // ordering problem related to handling attributes.  To solve this, we
  // intercept attributes here that are declared in the core and add them
  // immediately to the Data pool.
  void CoreHandler::add_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"archs")) {
      _archlist.clear();
      handle_list_or_item(value,bind(&CoreHandler::record_arch,this,_1));
    } else if (!strcmp(key,"attrs")) {
      // Handle attributes.
      handle_list_or_item(value,bind(&CoreHandler::record_attr,this,_1));      
    } else {
      RemoveHandler::add_key(key,value);
    }
  }

  void CoreHandler::record_attr(Ptree *p)
  {
    Data::add_attr(get_str(p,"attribute name."));
  }

  void CoreHandler::record_arch(Ptree *p)
  {
    _archlist.push_back(p);
  }

  void CoreHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"instrtables")) {
      // Create an instruction table for each item in the list.
      _itablenames.clear();
      handle_list_or_item(value,bind(&CoreHandler::record_itable,this,_1));
    }
    else if (!strcmp(key,"type")) {
      _core->_type = get_str(value,"type name.");
    }
    else if (!strcmp(key,"remove")) {
      _remove = get_bool(value,"remove flag",env());
    }
    else {
      // Anything else is shunted to the architecture object for this core, which
      // allows users to declare arch stuff directly within the core define.
      _corearch.handle_key(key,value);
    }
  }

  void CoreHandler::record_itable(Ptree *p)
  {
    const char *n = get_ident(p,"an instruction class.");
    _itablenames.push_back(n);
  }

  //
  // ArchHandler:  Responsible for processing architecture define blocks.
  //
  
  ArchHandler::ArchHandler(Opencxx::Environment *e,Opencxx::Ptree *p,const gc_string &name,bool modify) : 
    RemoveHandler(e,p,name,modify), 
    _core(0),
    _tmp_groups(new GroupHash),
    _ra_mask(0),
    _ea_mask(0)
      
  {};

  void ArchHandler::add_func(const char *name,Ptree *p)
  {
    // We use the [] operator b/c we want subsequent functions to override
    // prior functions.
    _helpers[name] = p;
  }

  void ArchHandler::add_type_decl(Ptree *p)
  {
    // We allow any type here so that it can be used as a helper type.
    _type_decls.push_back(p);
  }

  Handler *ArchHandler::add_defines(MsgLog &ml,Opencxx::Environment *e,Opencxx::Ptree *p,
                                    const char *type,const char *name) 
  {
    check_ident(name,p,"Group");
    GroupHandler* gh = 0;
    ForEach(_groups,i) {
      if ((*i)->name() == gc_string(name)) {
        gh = *i;
        break;	
      }
    }
    if (!gh) {
      PError(p,"Attempt to use group '" << name << "' which has not yet been defined.");
    }
 
    Handler *h = add_define(ml,e,p,type,"group_defmod",true);

    // Store group modifier, if possible.
    if (AspectRemoveHandler *result = dynamic_cast<AspectRemoveHandler*>(h)) {
      result->set_group(gh);
      return result;
    } else {
      PError(p,"This type does not allow groups to be used for selection purposes.");
    }
  }
    
  // The define operation generally replaces an existing item.
  Handler *ArchHandler::add_define(MsgLog &ml,Environment *e,Ptree *p,const char *type,const char *name,bool modify)
  {
    if (!strcmp(type,"reg")) {
      check_ident(name,p,"Register");
      auto rh = new RegHandler(e,p,name,modify);
      _regs.push_back(rh);
      return rh;
    } 
    else if (!strcmp(type,"parm")) {
      check_ident(name,p,"Parm");
      auto ph = new ParmHandler(e,p,name,modify);
      _parms.push_back(ph);
      return ph;
    } 
    else if (!strcmp(type,"context")) {
      check_ident(name,p,"Context");
      auto ch = new ContextHandler(e,p,name,modify);
      _contexts.push_back(ch);
      return ch;
    } 
    else if (!strcmp(type,"ra_mask")) {
      if (name) {
        PError(p,"Type 'ra_mask' does not take a name.");
      }
      if (!(modify && _ra_mask)) {
        _ra_mask = new AddrMaskHandler(e,p);
      }
      return _ra_mask;
    }
    else if (!strcmp(type,"ea_mask")) {
      if (name) {
        PError(p,"Type 'ea_mask' does not take a name.");
      }
      if (!(modify && _ea_mask)) {
        _ea_mask = new AddrMaskHandler(e,p);
      }
      return _ea_mask;
    }
    else if (!strcmp(type,"regfile")) {
      check_ident(name,p,"Register-file");
      auto rfh = new RegFileHandler(e,p,name,modify);
      _regfiles.push_back(rfh);
      return rfh;
    } 
    else if (!strcmp(type,"instrfield")) {
      check_ident(name,p,"Instruction field");
      auto ifh = new InstrFieldHandler(e,p,name,modify);
      _instrfields.push_back(ifh);
      return ifh;
    }
    else if (!strcmp(type,"prefixcounter")) {
      check_ident(name,p,"Prefix Counter");
      auto pch = new PrefixCounterHandler(e,p,name,modify);
      _prefix_counters.push_back(pch);
      return pch;
    }
    else if (!strcmp(type,"instr")) {
      check_ident(name,p,"Instruction",false);
      auto ih = new InstrHandler(e,p,name,modify);
      _instrs.push_back(ih);
      return ih;
    }
    else if (!strcmp(type,"subinstr")) {
      check_ident(name,p,"Sub-instruction",false);
      auto sih = new SubInstrHandler(e,p,name,modify);
      _subinstrs.push_back(sih);
      return sih;
    }
    else if (!strcmp(type,"exception")) {
      check_ident(name,p,"Exception");
      auto eh = new ExceptionHandler(e,p,name,modify);
      _exceptions.push_back(eh);
      return eh;
    }
    else if (!strcmp(type,"mmu")) {
      if (name) {
        PError(p,"Type 'mmu' does not take a name.");
      }
      auto mh = new MmuHandler(e,p,"mmu",modify);
      _mmus.push_back(mh);
      return mh;
    }
    else if (!strcmp(type,"cache")) {
      check_ident(name,p,"Cache");
      auto ch = new CacheHandler(e,p,name,modify);
      _caches.push_back(ch);
      return ch;
    }
    else if (!strcmp(type,"eventbus")) {
      check_ident(name,p,"Event Bus");
      auto ch = new EventBusHandler(e,p,name,modify);
      _eventbuses.push_back(ch);
      return ch;
    }
    else if (!strcmp(type,"ext_resource")) {
      check_ident(name,p,"External Resource");
      auto ch = new ExtResourceHandler(e,p,name,modify);
      _extresources.push_back(ch);
      return ch;
    }
    else if (!strcmp(type,"mem")) {
      check_ident(name,p,"Mem");
      auto  mh =  new MemoryHandler(e,p,name,modify);
      _memories.push_back(mh);
      return mh;
    }
    else if (!strcmp(type,"group")) {
      check_ident(name,p,"Group");
      auto  gh =  new GroupHandler(e,p,name,modify,*this);
      // Have to do it . This call only register new group in _tmp_groups 
      process_items(*gh,*_tmp_groups,*this,&ArchHandler::setup_group_handler);      
      _groups.push_back(gh);
      return gh;
    } else if (!strcmp(type,"assembler")) {
      if (name) {
        PError(p,"Assembler config cannot have name.");
      }
      if (_asmcfg) {
        PError(p,"Assembler config already exists.");
      } else {
        _asmcfg = new AsmConfigHandler(e,p);
      }
      return _asmcfg;
    }
    else if (!strcmp(type,"reloc")) {
      check_ident(name,p,"relocation");
      auto rh =  new RelocationHandler(e,p,name,modify);
      _relocs.push_back(rh);
      return rh;
    }
    else if (Handler *h = AspectGrpHandler::add_define(ml,e,p,type,name,modify)) {
      return h;
    }
    else {
      unknown_define(p,type,name);
    }
    abort();
  }

  // In the second pass, we iterate over each constituent list.
  void ArchHandler::process_keys(MsgLog &log)
  {
    DocHandler::process_keys(log);

    // Initial handling- just sets up keys, but doesn't resolve anything.
    preprocess_group_handlers(log);  

    set_aspects(_core);
    AspectGrpHandler::process_keys(log);

    // We concatenate together the documentation gc_strings, for now.
    _core->_doc += _doc;

    // Add on any helpers that we get here.
    // We use operator[] because we want later versions of a function
    // to override prior versions.
    ForEach(_helpers,i) {
      _core->_helpers[i->first] = i->second;
    }
   
    // Add on any type declarations from this architecture to the core.
    _core->_type_decls.insert(_core->_type_decls.end(),_type_decls.begin(),_type_decls.end());

    // Handle parms.
    process_named_handler(log,_parms,_core->_parms,*this,&ArchHandler::setup_parm_handler);

    // Handle contexts.
    process_named_handler(log,_contexts,_core->_contexts,*this,&ArchHandler::setup_context_handler);

    // Handle register files.
    process_named_handler(log,_regfiles,_core->_regfiles,*this,&ArchHandler::setup_regfile_handler);

    // Handle registers.
    process_named_handler(log,_regs,_core->_regs,*this,&ArchHandler::setup_reg_handler);

    // Handle exceptions.
    process_named_handler(log,_exceptions,_core->_exceptions,*this,&ArchHandler::setup_exception_handler);

    // Handle relocations
    process_named_handler(log,_relocs,_core->_relocs,*this,&ArchHandler::setup_relocation_handler);

    // Handle prefix counters.
    process_named_handler(log,_prefix_counters,_core->_prefix_counters,*this,&ArchHandler::setup_pfxctr_handler);

    // Handle instruction fields.
    process_named_handler(log,_instrfields,_core->_instrfields,*this,&ArchHandler::setup_instrfield_handler);

    // Handle subinstructions.
    process_named_handler(log,_subinstrs,_core->_subinstrs,*this,&ArchHandler::setup_subinstr_handler);

    // Handle instructions.
    process_instr_handler(log,_instrs,_core->_instrs,*this,&ArchHandler::setup_instr_handler);

    // Handle the caches.
    process_named_handler(log,_caches,_core->_caches,*this,&ArchHandler::setup_cache_handler);

    // Handle the event buses.
    process_named_handler(log,_eventbuses,_core->_eventbuses,*this,&ArchHandler::setup_eventbus_handler);

    // Handle any opaque resources.
    process_named_handler(log,_extresources,_core->_extresources,*this,&ArchHandler::setup_extresource_handler);

    //Handle the local memories
    process_named_handler(log,_memories,_core->_memories,*this,&ArchHandler::setup_memory_handler);
    
    // Handle the MMU.
    process_mmu_handler(log);

    // Handle the address masks.
    if (_ra_mask) {
      process_addr_mask(*_ra_mask,_core->_ra_mask,log);
    }

    if (_ea_mask) {
      process_addr_mask(*_ea_mask,_core->_ea_mask,log);
    }

    //Handle the assembler configuration
    if (_asmcfg) {
      process_asm_config(*_asmcfg,_core->_asmcfg,log);
    }

    // Handle groups.  This is really just to handle any groups which aren't
    // used any where else.
    process_group_handlers(log);  
  }

  void ArchHandler::process_asm_config(AsmConfigHandler &asmh, AsmConfig &asmcfg,MsgLog &log) {
    
    asmh._asmcfg = &asmcfg;
    asmh.process_keys(log);
  }
  
  void ArchHandler::process_addr_mask(AddrMaskHandler &amh,AddrMask &am,MsgLog &log)
  {
    if (amh.remove()) {
      am.clear();
    } else {
      amh._mask = &am;
      am._doc += amh.doc();
      amh.set_aspects(&am);
      amh.process_keys(log);
    }
  }
  
  void ArchHandler::setup_group_handler(GroupHandler &gh,Group &group)
  {   
    gh._group = &group;
    group._doc += gh.doc();
  }

  void ArchHandler::setup_regfile_handler(RegFileHandler &rfh,RegFile &rf)
  {
    rf._doc += rfh.doc();
    rfh._core = _core;
    rfh.set_regbase(&rf);
  }

  void ArchHandler::setup_reg_handler(RegHandler &rh,Reg &reg)
  {
    reg._doc += rh.doc();
    rh._core = _core;
    rh.set_regbase(&reg);
  }

  void ArchHandler::setup_parm_handler(ParmHandler &ph,Parm &p)
  {
    p._doc += ph.doc();
    ph._parm = &p;
    ph.set_aspects(&p);
  }

  void ArchHandler::setup_context_handler(ContextHandler &ch,Context &c)
  {
    c._doc += ch.doc();
    ch._context = &c;
    ch._core = _core;
    ch.set_aspects(&c);
  }

  void ArchHandler::setup_exception_handler(ExceptionHandler &eh,Exception &ex)
  {
    ex._doc += eh.doc();
    eh._exception = &ex;
    eh.set_aspects(&ex);
  }


  void ArchHandler::setup_relocation_handler(RelocationHandler &rh, Relocation &reloc)
  {
    reloc._doc += rh.doc();
    rh._reloc = &reloc;
  }

  void ArchHandler::setup_pfxctr_handler(PrefixCounterHandler &pch,PrefixCounter &pc)
  {
    pc._doc += pch.doc();
    pch._pc = &pc;
  }

  void ArchHandler::setup_instrfield_handler(InstrFieldHandler &ifh,InstrField &instrfield)
  {
    instrfield._doc += ifh.doc();
    ifh._core = _core;
    ifh._instrfield = &instrfield;
  }

  void ArchHandler::setup_subinstr_handler(SubInstrHandler &sih, SubInstr &subinstr)
  {
    subinstr._doc += sih.doc();
    sih._core = _core;
  
    sih.set_instrbase(&subinstr);
  }


  void ArchHandler::setup_instr_handler(InstrHandler &ih,Instr &instr)
  {
    instr._doc += ih.doc();
    ih._core = _core;  
    ih.set_instrbase(&instr);
    // In case this is a dummy removal item.  Only needed for instructions b/c
    // of the weird handling we have to do for nested instructions.  Have I
    // mentioned how much I hate the nested instruction concept??
    instr._remove = instr._remove || ih.remove();
  }

  void ArchHandler::setup_memory_handler(MemoryHandler &mh, Memory &memory) 
  {
    memory._doc += mh.doc();
    mh._core = _core;
    mh._memory = &memory;
  }

  // Creates any caches and places them into the correct levels in the
  // core object.
  void ArchHandler::setup_cache_handler(CacheHandler &ch,Cache &cache)
  {
    cache._doc += ch.doc();
    ch._cache = &cache;
  }

  void ArchHandler::setup_eventbus_handler(EventBusHandler &ebh,EventBus &bus)
  {
    bus._doc += ebh.doc();
    ebh._bus = &bus;
  }

  void ArchHandler::setup_extresource_handler(ExtResourceHandler &rh,ExtResource &res)
  {
    res._doc += rh.doc();
    rh._res = &res;
  }

  Core::Endianness get_endianness(Ptree *value,const char *msg)
  {
    if (!strcmp(get_ident(value,msg),"little")) {
      return Core::Little; 
    } else if (!strcmp(get_ident(value,msg),"big")) {
      return Core::Big; 
    } else {
      PError(value,"Unknown endianness for " << msg);
    }
  }

  void ArchHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"attrs")) {
      // Handle attributes.
      handle_list_or_item(value,bind(&ArchHandler::add_attr,this,_1));
    }
    else if (!strcmp(key,"branch_taken")) {
      // action:  Should be a 0-ary function. 
      _core->_branch_taken_handler._func = get_func(value,0,"branch-taken handler."); 
    }
    else if (!strcmp(key,"addr_check")) {
      // action: Should be a 3-ary or 6-ary function, but this will be checked
      // during finalization.
      _core->_addr_check_handler._func = get_func(value,-1,"address-check handler."); 
    }
    else if (!strcmp(key,"pre_exception")) {
      // action:  Should be a 1-ary function. 
      _core->_pre_exception_handler._func = get_func(value,1,"pre-exception handler."); 
    }
    else if (!strcmp(key,"post_exception")) {
      // action:  Should be a 1-ary function. 
      _core->_post_exception_handler._func = get_func(value,1,"post-exception handler."); 
    }
    else if (!strcmp(key,"invalid_entry_read")) {
      // action:  Should be a 2-ary function. 
      _core->_invalid_entry_read._func = get_func(value,2,"global invalid-entry-read handler."); 
    }
    else if (!strcmp(key,"invalid_entry_write")) {
      // action:  Should be a 3-ary function. 
      _core->_invalid_entry_write._func = get_func(value,3,"global invalid-entry-write handler."); 
    }
    else if (!strcmp(key,"decode_miss")) {
      // action:  Should be  2-ary function.
      _core->_decode_miss_handler._func = get_func(value,2,"decode-miss handler.");
      
    } 
    else if (!strcmp(key,"decode_retry")) {
      // action: Should be 2-ary function.
      _core->_decode_retry_handler._func = get_func(value,2,"decode-retry handler.");
    }
    else if (!strcmp(key,"post_fetch")) { 
      // action:  Should be a unary function. 
      _core->_post_fetch_handler._func = get_func(value,1,"post-fetch handler."); 
    }
    else if (!strcmp(key,"pre_cycle")) {
      // action:  Should be a 0-ary function. 
      _core->_pre_cycle_handler._func = get_func(value,0,"pre-cycle handler."); 
    } 
    else if (!strcmp(key,"post_cycle")) {
      // action:  Should be a 0-ary function. 
      _core->_post_cycle_handler._func = get_func(value,0,"post-cycle handler."); 
    } 
    else if (!strcmp(key,"pre_fetch")) {
      // action:  Should be a 0-ary function. 
      _core->_pre_fetch_handler._func = get_func(value,0,"pre-fetch handler."); 
    }
    else if (!strcmp(key,"post_exec")) { 
      // action:  Should be a 0-ary function. 
      _core->_post_exec_handler._func = get_func(value,0,"post-exec handler.");
    } else if (!strcmp(key,"pre_asm")) {
      _core->_pre_asm_handler._func = get_func(value,0,"pre-asm handler.");
    } else if (!strcmp(key,"post_asm")) { 
      // action:  Should be a 1-ary function. 
      _core->_post_asm_handler._func = get_func(value,1,"post-asm handler.");
    }
    else if (!strcmp(key,"post_packet_asm")) {
      // action:  Should be a 0-ary function. 
      _core->_post_packet_asm_handler._func = get_func(value,1,"post-packet-asm handler.");
    }
    else if (!strcmp(key,"post_packet")) {
      // action: Should be 0-ary function.
      _core->_post_packet_handler._func = get_func(value,0,"post-packet handler.");
    }
    else if (!strcmp(key,"post_packet_commit")) {
      // action: Should be 0-ary function.
      _core->_post_packet_commit_handler._func = get_func(value,0,"post-packet-commit handler.");
    }
    else if (!strcmp(key,"active")) {
      // action:  Should be a unary function.
      _core->_active_watch = get_func(value,0,"activity watch function.");
    } 
    else if (!strcmp(key,"itable_watch")) {
      _core->_itable_watch = get_func(value,0,"instruction-table watch function");
    }
    else if (!strcmp(key,"parallel_execution")) {
      _core->_parallel_execution = get_uint(value,"parallel execution set size.",env());
    } 
    else if (!strcmp(key,"packet_pos_incr")) {
      _core->_packet_pos_incr = get_uint(value,"packet-position increment size.",env());
    } 
    else if (!strcmp(key,"blk")) {
      handle_list_or_item(value,bind(&ArchHandler::add_blk,this,_1));
    }
    else if (!strcmp(key,"bit_endianness")) {
      _core->_bitEndianness = get_endianness(value,"bit-endianness");
    }
    else if (!strcmp(key,"instr_endianness")) {
      _core->_instrEndianness = get_endianness(value,"instr-endianness");
    }
    else if (!strcmp(key,"data_endianness")) {
      _core->_dataEndianness = get_endianness(value,"data-endianness");
    }
    else if (!strcmp(key,"reserved_bits_value")) {
      _core->_reserved_bits_value = get_uint(value,"reserved-bits value.",env());
    } 
    else if (!strcmp(key,"immediate_writehooks")) {
      _core->_immediate_writehooks = (get_bool(value,"immediate-write-hook flag.",env()) ? ImmTrue : ImmFalse);
    }
    else if (!strcmp(key,"exception_is_branch")) {
      _core->_exception_is_branch = get_bool(value,"exception-is-branch flag.",env());
    }
    else {
      unknown_key(value,key);
    }
  }

  void ArchHandler::add_attr(Ptree *p)
  {
    Data::add_attr(get_str(p,"class name."));
  }

  void ArchHandler::add_blk(Ptree *p) 
  {
    auto it = (_core->_blocks).insert(get_ident(p,"block name."));
    if (!it.second) {
      PError(p,"Block has already been defined.");
    }
  }

  void ArchHandler::process_mmu_handler(MsgLog &log)
  {
    ForEach(_mmus,i) {
      MmuHandler &mh = *(*i);
      try {
        if (mh.remove()) {
          _core->_mmu = 0;
        } else {
          // Remove not set, so add the item if it doesn't already exist.
          if (!_core->_mmu) {
            _core->_mmu = new MMU(mh.env(),mh.loc());
          }
          _core->_mmu->_doc += mh.doc();
          mh._core = _core;
          mh._mmu = _core->_mmu;
          mh.process_keys(log);
        }
      }
      catch (parse_error &err) {
        log.add(err);
      }
    }
  }

  void ArchHandler::preprocess_group_handlers(MsgLog &log)
  {
    // Transfer and check information
    process_named_handler(log,_groups,_core->_groups,*this,&ArchHandler::setup_group_handler);    
  }

  void ArchHandler::process_group_handlers(MsgLog &log) 
  {  
    // Resolve what was not already resolved
    ForEach(_groups,i) {
      (*i)->resolve_items(0);
    }
  }
  
  //
  // AddrMaskHandler:  Process address masks.
  // 

  void AddrMaskHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"value")) {
      _mask->_initial = get_addr(value,"initial value",env());
    } else if (!strcmp(key,"watch")) {
      _mask->_watch = get_func(value,0,"watch function.");
    } else {
      unknown_key(value,key);
    }
  }

  //
  // ParmHandler:  Processes architectural parameters.
  //

  void ParmHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"options")) {
      handle_list_or_item(value,bind(&ParmHandler::add_option,this,_1));
    } else if (!strcmp(key,"value")) {
      _parm->_value = get_ident(value,"parameter default value");
    } else if (!strcmp(key,"constant")) {
      _parm->_constant = get_bool(value,"parameter constant flag",env());
    } else if (!strcmp(key,"watch")) {
      _parm->_watch = get_func(value,0,"watch function.");
    } else if (handle_attrs(key,value,*_parm,env())) {
      ;
    } else {
      unknown_key(value,key);
    }
  }

  void ParmHandler::add_option(Ptree *p)
  {
    _parm->_options.insert(get_ident(p,"parameter option"));
  }

  bool ParmHandler::has_attr(const AttrItem &ai) const
  {
    return (_parm) ? (bool)_parm->has_attr(ai) : false;
  }

  //
  // RegBaseHandler: Responsible for processing data common between
  //                 registers and register files.
  //

  Handler *RegBaseHandler::add_define(MsgLog &ml,Environment *e,Ptree *p,const char *type,const char *name,bool modify)
  {
    if (!strcmp(type,"field")) {
      if (!name) {
        PError(p,"Register field defined without a name.");
      }
      auto rfh = new RegFieldHandler(e,p,name,modify);
      _regfields.push_back(rfh);
      return rfh;
    }
    else {
      return AspectRemoveHandler::add_define(ml,e,p,type,name,modify);
    }
  }

  void RegBaseHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"log_name")) {
      _regbase->_log_name = get_str(value,"register logging name.");
    }
    else if (!strcmp(key,"width")) {
      // Bit width specification.
      _regbase->_width = get_uint(value,"register width.",env());
    }
    else if (!strcmp(key,"offset")) {
      // Bit offset specification.
      _regbase->_offset = get_uint(value,"register offset.",env());
    }
    else if (!strcmp(key,"pseudo")) {
      // Psuedo-register(file) specifier.
      _regbase->_pseudo = get_bool(value,"pseudo specifier.",env());
    }
    else if (!strcmp(key,"reset")) {
      if (get_uintarb(value,0)) {
        _regbase->_resettype = RegBase::Value;
        _regbase->_resetvalue = value;
      } else if (get_func(value,-1,0)) {
        _regbase->_resettype = RegBase::Action;
        _regbase->_resetaction = value;
      } else if (get_str(value,"reset value.")){
        _regbase->_resettype = RegBase::Enum;
        _regbase->_resetvalue = value;
      }
    }
    else if (!strcmp(key,"external_write")) {
      _regbase->_external_write = get_func(value,-1,"external-write hook.");
    }
    else if (!strcmp(key,"serial")) {
      _regbase->_serial = get_bool(value,"serial register.",env());
    } else if (!strcmp(key,"enumerated")) {
      handle_list_or_item(value,bind(&RegBaseHandler::record_enums_adapter,this,_1));
    }
    else if (handle_attrs(key,value,*_regbase,env())) {
      ;
    }
    else {
      unknown_key(value,key);
    }
  }

  void RegBaseHandler::record_enums_adapter(Ptree *p) 
  {
    int value;
    if (_regbase->_enums.empty()) {
      value = 0;
    } else {
      value = _regbase->_enums.back().second + 1;
    }
    handle_list_or_item(p, bind(&RegBaseHandler::record_enums, this, _1, ref(value)));
  }

  void RegBaseHandler::record_enums(Ptree *p, int &value)
  {
    _regbase->_enums.push_back(make_pair(get_ident(p,"enumarated-field value."),value));
  }

  // This processes its keys, then handles all of the register files.
  void RegBaseHandler::process_keys(MsgLog &log)
  {
    AspectRemoveHandler::process_keys(log);

    process_named_handler(log,_regfields,_regbase->_fields,
                          *this,&RegBaseHandler::setup_regfield_handler);

  }

  void RegBaseHandler::setup_regfield_handler(RegFieldHandler &rfh,RegField &rf)
  {
    rf._doc = rfh.doc();
    rfh._regbase = _regbase;
    rfh._field = &rf;
  }

  template<class Base,class Hook> void RegBaseHandler::process_hook(MsgLog &log,Base *base,Hook *hook,bool read)
  {
    if (hook) {
      if (hook->remove()) {
        if (read) {
          base->_read = 0;
        } else {
          base->_write = 0;
        }
      } else {
        if (read) {
          base->_read = hook->create_data_object();
        } else {
          base->_write = hook->create_data_object();
        }
        hook->_core = _core;
        hook->process_keys(log);
      }
    }
  }

  bool RegBaseHandler::has_attr(const AttrItem &ai) const
  {
    return (_regbase) ? (bool)_regbase->has_attr(ai) : false;
  }

  bool RegBaseHandler::has_field(const gc_string &field) const
  {
    return (_regbase) ? (bool)_regbase->get_field(field) : false;
  }

  //
  // RegHandler: Resposible for processing register define blocks.
  //

  Handler *RegFileHandler::add_define(MsgLog &ml,Environment *e,Ptree *p,const char *type,const char *name,bool modify)
  {
    if (!strcmp(type,"read")) {
      if (name) {
        PError(p,"A read-hook definition does not take a name.");
      }
      _read = new RegFileHookHandler(e,p,true,modify);
      return _read;
    }
    else if (!strcmp(type,"write")) {
      if (name) {
        PError(p,"A write-hook definition does not take a name.");
      }
      _write = new RegFileHookHandler(e,p,false,modify);
      return _write;
    }
    else {
      return RegBaseHandler::add_define(ml,e,p,type,name,modify);
    }
  }

  Handler *RegFileHandler::add_define(MsgLog &ml,Environment *e,Ptree *p,const char *type,unsigned id,bool modify)
  {
    if (!strcmp(type,"entry")) {
      auto reh = new RegEntryHandler(e,p,id,modify);
      _regentries.push_back(reh);
      return reh;
    }
    else {
      unknown_define(p,type,id);
    }
    abort();
  }

  void RegFileHandler::optional_new_hook(Ptree *loc)
  {
    RegFileHook *rh = new RegFileHook(_regfile->env(),loc);
    if (!_regfile->_read) {
      _regfile->_read = rh;
    }
    if (!_regfile->_write) {
      _regfile->_write = rh;
    }
  }

  void RegFileHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"size")) {
      _regfile->_size = get_uint(value,"register-file size.",env());
    }
    else if (!strcmp(key,"prefix")) {
      _regfile->_prefix = get_ident(value,"register-file prefix.");
    }
    else if (!strcmp(key,"prefix_for_regname")) {
      _regfile->_prefix_for_regname = get_ident(value,"register-file prefix for regname.");
    }	
    else if (!strcmp(key,"alias")) {
      optional_new_hook(value);
      _regfile->_read->_alias = _regfile->_write->_alias = value;
    }
    else if (!strcmp(key,"alias_slice")) {
      optional_new_hook(value);
      _regfile->_read->_slice_value = _regfile->_write->_slice_value = value;
    }
    else if (!strcmp(key,"invalid_entry_read")) {
      _regfile->_invalid_entry_read = get_func(value,1,"register-file invalid-entry read hook.");
    }
    else if (!strcmp(key,"invalid_entry_write")) {
      _regfile->_invalid_entry_write = get_func(value,2,"register-file invalid-entry write hook.");
    } else {
      RegBaseHandler::handle_key(key,value);
    }
  }

  void RegFileHandler::process_keys(MsgLog &log)
  {
    RegBaseHandler::process_keys(log);

    process_named_handler(log,_regentries,_regfile->_entries,
                          *this,&RegFileHandler::setup_regentry_handler);  

    process_hook(log,_regfile,_read,true);
    process_hook(log,_regfile,_write,false);
  }

  void RegFileHandler::setup_regentry_handler(RegEntryHandler &rfeh,RegEntry &rfe)
  {
    rfe._doc = rfeh.doc();
    rfeh._regentry = &rfe;
    rfeh._regfile = _regfile;
    rfeh.set_aspects(&rfe);
  }

  void RegFileHandler::set_regbase(RegFile *rf) 
  { 
    _regbase = _regfile = rf; 
    set_aspects(rf);
  }

  //
  // RegEntryHandler:  Responsible for processing sparse regfile entries.
  //

  void RegEntryHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"reg")) {
      _regentry->_reg = value;
    } else if (!strcmp(key,"syntax")) {
      _regentry->_syntax = get_str(value,"syntax.");
    }
    else {
      unknown_key(value,key);
    }
  }

  Handler *RegEntryHandler::add_define(MsgLog &ml,Environment *e,Ptree *p,const char *type,const char *name,bool modify)
  {
    if (!strcmp(type,"read")) {
      if (name) {
        PError(p,"A read-hook definition does not take a name.");
      }
      _read = new RegEntryHookHandler(e,p,true,modify);
      return _read;
    }
    else if (!strcmp(type,"write")) {
      if (name) {
        PError(p,"A write-hook definition does not take a name.");
      }
      _write = new RegEntryHookHandler(e,p,false,modify);
      return _write;
    }
    else {
      return AspectIntRemoveHandler::add_define(ml,e,p,type,name,modify);
    }
    abort();
  }

  void RegEntryHandler::process_hook(MsgLog &log,RegEntryHookHandler *hook,bool read)
  {
    if (hook) {
      if (hook->remove()) {
        if (read) {
          _regentry->_read = 0;
        } else {
          _regentry->_write = 0;
        }
      } else {
        if (read) {
          _regentry->_read = hook->create_data_object();
        } else {
          _regentry->_write = hook->create_data_object();
        }
        hook->process_keys(log);
      }
    }
  }

  void RegEntryHandler::process_keys(MsgLog &log)
  {
    AspectIntRemoveHandler::process_keys(log);

    process_hook(log,_read,true);
    process_hook(log,_write,false);
  }


  void RegEntryHookHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"ignore")) {
      _hook->_ignore = get_bool(value,"hook ignore field.",env());
    } else if (!strcmp(key,"action")) {
      // action:  Should be a function.
      if (_read) {
        _hook->_action = get_func(value,0,"register-file-entry read-hook action");
      } else {
        _hook->_action = get_func(value,1,"register-file-entry write-hook action");
      }
    } else {
      unknown_key(value,key);
    }
  }

  RegEntryHook *RegEntryHookHandler::create_data_object()
  {
    _hook = new RegEntryHook(env(),loc());
    set_aspects(_hook);
    return _hook;
  }

  //
  // RegHandler:: Resposible for processing register define blocks.
  //

  Handler *RegHandler::add_define(MsgLog &ml,Environment *e,Ptree *p,const char *type,const char *name,bool modify)
  {
    if (!strcmp(type,"read")) {
      if (name) {
        PError(p,"A read-hook definition does not take a name.");
      }
      _read = new RegHookHandler(e,p,true,modify);
      return _read;
    }
    else if (!strcmp(type,"write")) {
      if (name) {
        PError(p,"A write-hook definition does not take a name.");
      }
      _write = new RegHookHandler(e,p,false,modify);
      return _write;
    }
    else {
      return RegBaseHandler::add_define(ml,e,p,type,name,modify);
    }
  }

  void RegHandler::optional_new_hook(Ptree *loc)
  {
    RegHook *rh = new RegHook(_reg->env(),loc);
    if (!_reg->_read) {
      _reg->_read = rh;
    }
    if (!_reg->_write) {
      _reg->_write = rh;
    }
  }

  void RegHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"alias")) {
      optional_new_hook(value);
      _reg->_read->_alias = _reg->_write->_alias = value;
    }
    else if (!strcmp(key,"alias_slice")) {
      optional_new_hook(value);
      _reg->_read->_slice_value = _reg->_write->_slice_value = value;
    }
    else {
      RegBaseHandler::handle_key(key,value);
    }
  }

  void RegHandler::process_keys(MsgLog &log)
  {
    RegBaseHandler::process_keys(log);
  
    process_hook(log,_reg,_read,true);
    process_hook(log,_reg,_write,false);
  }

  void RegHandler::set_regbase(Reg *r) 
  { 
    _regbase = _reg = r; 
    set_aspects(r);
  }

  //
  // RegFieldHandler:  Handles register fields.
  //

  void RegFieldHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"bits")) {
      // Try for a single integer or a pair.
      if (!_field->_bits.record_bits(value,false,env())) {
        // Didn't work, so try to handle a list of pairs.
        handle_list_or_item(value,bind(&BitPairVect::record_bits_adapter,&_field->_bits,_1,env()));
      }
    }
    else if (!strcmp(key,"indexed")) {
      unsigned w = get_uint(value,"register-field index width.",env());
      _field->set_indexed(w);
    } 
    else if (!strcmp(key,"reserved")) {
      _field->_reserved = get_bool(value,"register-field reserved flag.",env());
    }
    else if (!strcmp(key,"readonly")) {
      _field->_readonly = get_bool(value,"register-field read-only flag.",env());
    } 
    else if (!strcmp(key,"writable")) {
      _field->_writable = get_bool(value,"register-field writable flag.",env());
    } 
    else if (handle_attrs(key,value,*_field,env())) {
      ;
    } 
    else {
      unknown_key(value,key);
    }
  }

  //
  // HookHandler:  Handles register read/write hook fields.
  //

  void HookHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"alias")) {
      _hookbase->_alias = value;
    }
    else if (!strcmp(key,"slice")) {
      _hookbase->_slice_value = value;
    } else if (!strcmp(key,"ignore")) {
      _hookbase->_ignore = get_bool(value,"alias/hook ignore field.",env());
    } else if (!strcmp(key,"immediate")) {
      _hookbase->_immediate = (get_bool(value,"immediate flag.",env()) ? ImmTrue : ImmFalse);
    } else {
      unknown_key(value,key);
    }
  }

  RegFileHook *RegFileHookHandler::create_data_object()
  {
    _hookbase = _hook = new RegFileHook(env(),loc());
    return _hook;
  }

  void RegFileHookHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"regs")) {
      // offset:  should be a pair of ints or a list of pairs.
      handle_list_or_item(value,bind(&RegFileHookHandler::record_reg_alias,this,_1));
    }
    else if (!strcmp(key,"action")) {
      // action:  Should be a function.
      if (_read) {
        _hook->_action = get_func(value,1,"register-file read-hook action");
      } else {
        _hook->_action = get_func(value,-1,"register-file write-hook action");
      }
    }
    else {
      HookHandler::handle_key(key,value);
    }
  }

  // We expect this to receive a pair of integers denoting register indices.
  void RegFileHookHandler::record_reg_alias(Ptree *p)
  {
    Ptree *l,*h;
    if (Match(p,"[ ( [ %? , %? ] ) ]",&l,&h)) {
      unsigned lo = get_uint(l,"lower register alias index.",env());
      unsigned hi = get_uint(h,"upper register alias index.",env());
      if (lo > hi) {
        PError(p,"Lower register alias index must be <= to upper.");
      }
      for (unsigned i = lo; i <= hi; ++i) {
        _hook->_aliasregs.push_back(i);
      }
    } else {
      PError(p,"Each element of the register alias list should be a pair of register indices.");
    }
  }

  RegHook *RegHookHandler::create_data_object()
  {
    _hookbase = _hook = new RegHook(env(),loc());
    return _hook;
  }

  void RegHookHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"action")) {
      // action:  Should be a function.
      if (_read) {
        _hook->_action = get_func(value,0,"register read-hook action");
      } else {
        _hook->_action = get_func(value,-1,"register write-hook action");
      }
    }
    else {
      HookHandler::handle_key(key,value);
    }
  }

  //
  // Prefix counter handler
  //

  void PrefixCounterHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"reset")) {
      _pc->_reset_value = get_uint(value,"reset value.",env());
    } else if (handle_attrs(key,value,*_pc,env())) {
      ;
    } else {
      unknown_key(value,key);
    }
  }

  //
  // Instruction field handler.
  //

  void InstrFieldHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"bits")) {
      // Try for a single integer or a pair.
      if (!_instrfield->_bits.record_bits(value,false,env())) {
        // Didn't work, so try to handle a list of pairs.
        handle_list_or_item(value,bind(&BitPairVect::record_bits_adapter,&_instrfield->_bits,_1,env()));
      }
    } else if (!strcmp(key,"type")) {
      const char *t = get_ident(value,"field type.");
      if (!strcmp(t,"regfile")) {
        _instrfield->_type = InstrField::tRegFile;
      } else if (!strcmp(t,"memory")) {
        _instrfield->_type = InstrField::tMem;
      } else if (!strcmp(t,"immed")) {
        _instrfield->_type = InstrField::tImm;
      } else if (!strcmp(t,"instr")) {      // New key for micro-instructions
        _instrfield->_type = InstrField::tInstr;		
      } else {
        _instrfield->_type = InstrField::tRefField;
        _instrfield->_pfield_name = t;
      }
    } else if (!strcmp(key,"ref")) {
      _instrfield->_ref = value;
    } else if (!strcmp(key,"enumerated")) {
      handle_list_or_item(value,bind(&InstrFieldHandler::record_enums_adapter,this,_1));
    } else if (!strcmp(key,"is_signed")) {
      _instrfield->_signed = get_bool(value,"signed flag.",env());
    } else if (!strcmp(key,"unsigned_upper_bound")) {
      _instrfield->_unsigned_ub = get_bool(value,"unsigned upper-bound flag.",env());
    } else if (!strcmp(key,"overlay")) {
      _instrfield->_overlay = get_bool(value,"overlay flag.",env());
    } else if (!strcmp(key,"is_inverted")) {
      _instrfield->_inverted = get_bool(value,"inverted flag.",env());
    } else if (!strcmp(key,"pseudo")) {
      _instrfield->_pseudo = get_bool(value,"pseudo flag.",env());
    } else if (!strcmp(key,"valid_ranges")) {
      InstrFieldHandler::record_range_adapter(value); 
    } else if (!strcmp(key,"valid_masks")) {
      InstrFieldHandler::record_masks_adapter(value); 
    } else if (!strcmp(key,"addr")) {
      const char *t = get_ident(value,"address type.");
      if (!strcmp(t,"pc")) {
        _instrfield->_addr_type = InstrField::aPcRel;
      } else if (!strcmp(t,"abs")) {
        _instrfield->_addr_type = InstrField::aAbs;
      } else if (!strcmp(t,"none")) {
        _instrfield->_addr_type = InstrField::aNone;
      } else {
        PError(value,"Unknown instruction address type:  " << value);
      }
    } else if (!strcmp(key,"shift")) {
      _instrfield->_shift = get_uint(value,"field shift value.",env());
    } else if (!strcmp(key,"offset")) {
      _instrfield->_offset = get_uint(value,"field offset value.",env());      
    } else if (!strcmp(key,"display")) {
      const char *t = get_ident(value,"display type.");
      if (!strcmp(t,"hex")) {
        _instrfield->_display = InstrField::Hex;
      } else if (!strcmp(t,"dec")) {
        _instrfield->_display = InstrField::Dec;
      } else if (!strcmp(t,"signed_dec")) {
        _instrfield->_display = InstrField::SignedDec;
      } else if (!strcmp(t,"name")) {
        _instrfield->_display = InstrField::Name;
      } else if (!strcmp(t,"def")) {
        _instrfield->_display = InstrField::Default;
      } else {
        PError(value,"Unknown instruction display type:  " << value);
      }
    } else if (!strcmp(key,"table")) {
      handle_list_or_item(value,bind(&InstrFieldHandler::record_table_adapter,this,_1));
    } else if (!strcmp(key,"value")) { 
      _instrfield->_default = value;
    } else if (!strcmp(key,"prefix")) {
      _instrfield->_prefix = get_bool(value,"prefix flag.",env());
    } else if (!strcmp(key,"indexed")) {
      uint32_t w = get_uint(value,"prefix field index width.",env());
      if (w != 0) {
        _instrfield->_indexed = w;
      } else {
        PError(value,"Prefix field index must be positive.");
      }
    } else if (!strcmp(key,"index_endianness")) {
      if (!strcmp(get_ident(value,"Bit endianness"),"little")) {
        _instrfield->_index_endianness = InstrField::Little; 
      } else if (!strcmp(get_ident(value,"Bit endianness"),"big")) {
        _instrfield->_index_endianness = InstrField::Big; 
      } else {
        PError(loc(),"Unknown endianness");
      }
    } else if (!strcmp(key,"width")) {
      if (!strcmp(value->ToString(),"variable")) {
        _instrfield->_complement = true;
      } else {
        _instrfield->_width = get_uint(value,"field width.",env());
      }
    } else if (!strcmp(key,"size")) {
      _instrfield->_size = get_uint(value,"field size.",env());
    } else if (!strcmp(key,"blk")) {
      handle_list_or_item(value,bind(&InstrFieldHandler::add_blk,this,_1));
    } else if (!strcmp(key,"action")) {
      _instrfield->_action = value;
    } else if (!strcmp(key,"alias")) {
      handle_alias(value);
    } else if (!strcmp(key,"fields")) {
      // Try for a signle integer or pair
      if (!record_fields(value,false)) {
        handle_list_or_item(value,bind(&InstrFieldHandler::record_fields_adapter,this,_1));
      }
    } else if (!strcmp(key,"syntax")) {
      handle_syntax(value);
    } else if (!strcmp(key,"reserved")) {
      _instrfield->_reserved = get_bool(value,"reserved flag.",env());
    } else if (!strcmp(key,"assembler")) {
      _instrfield->_assembler = get_bool(value,"assembler flag.",env());
    } else if (!strcmp(key,"disassemble")) {
      gc_string t = get_ident(value,"disassemble type.");
      if (t == "true") {
        _instrfield->_disassemble = InstrField::dsTrue;
      }
      else if (t == "false") {
        _instrfield->_disassemble = InstrField::dsFalse;
      }
      else if (t == "prefix") {
        _instrfield->_disassemble = InstrField::dsPrefix;
      }
      else {
        PError(value,"Unknown disassemble-type value;  " << t);
      }
    } else if (!strcmp(key,"parm")) {
      _instrfield->_parm = get_bool(value,"parm flag.",env());
    } else if (!strcmp(key,"allow_conflict")) {
      _instrfield->_allow_conflict = get_bool(value,"allow-conflict flag.",env());
    } else if (handle_attrs(key,value,*_instrfield,env())) {
      ;
    } else {
      unknown_key(value,key);
    }
  }

  void InstrFieldHandler::handle_alias(Ptree *value) {
    if (_instrfield->_alias) {
      PError(value,"Instruction field may not be aliased to more than one field)");
    } else {
      _instrfield->_alias = new InstrField::Alias;
      _instrfield->_alias->_target_name = get_str(value,"instruction field alias name.");
    }
  }
  
  // This just collects the items- we process them later.
  void add_syntax_item(Ptree *p,PtreeArray &items)
  {
    items.Append(p);
  }



  void InstrFieldHandler::handle_syntax(Ptree *value) 
  {
    auto items = new PtreeArray;
    handle_list_or_item(value,bind(add_syntax_item,_1,ref(*items)));

    if(items->Number() == 0) {
      PError(value,"Bad syntax specified: Expected a list with at lest one element.");
    }
    auto syntax =  new InstrField::Syntax;

    // First 
    if (const char *tmp = get_str((*items)[0],0)) {
      syntax->_fmt = tmp;
    }
    syntax->_orig = items;
    _instrfield->_syntax = syntax;
  }

  Handler *InstrFieldHandler::add_define(MsgLog &ml,Environment *e,Ptree *p,const char *type, 
                                         const char *name,bool modify)
  {
    if (!strcmp(type,"instrfield")) {
      check_ident(name,p,"Instrfields");
      InstrFieldHandler *ifh = new InstrFieldHandler(e,p,name,modify);
      _instrfields.push_back(ifh);
      return ifh;
    } else {
      unknown_define (p,type,name);
    }
    abort();
  }

  void InstrFieldHandler::record_enums_adapter(Ptree *p)
  {
    int value;
    if (_instrfield->_enums.empty()) {
      value = 0;
    } else {
      value = _instrfield->_enums.back().second + 1 ;
    } 
    handle_list_or_item(p, bind(&InstrFieldHandler::record_enums,this,_1,ref(value)));
  }

  void InstrFieldHandler::record_enums(Ptree *p, int &value)
  {
    _instrfield->_enums.push_back(make_pair(get_str(p,"enumerated-field value."),value));
  }
  
  void InstrFieldHandler::record_range_adapter(Opencxx::Ptree *p)  
  {
    Ptree *l,*h;
    unsigned int ll,hh;
    if (Match(p,"[ ( [ %? , %? ] ) ]",&l,&h)) {
      if (!get_uint(ll,l,env())){
        handle_list_or_item(p,bind(&InstrFieldHandler::record_range_adapter,this,_1));   
      }
      else {
        ll = get_uint(l,"lower range limit.",env());
        hh = get_uint(h,"upper range limit.",env());
        _instrfield->_ranges.push_back(make_pair(ll,hh));
      }
    }
    else {
      PError(p, "Parsing error in valid_ranges.");
    }  
  }
  
  


  void InstrFieldHandler::record_masks_adapter(Opencxx::Ptree *p)  
  {
    Ptree *l,*h;
    unsigned int ll,hh;
    if (Match(p,"[ ( [ %? , %? ] ) ]",&l,&h)) {
      if (!get_uint(ll,l,env())) {
        handle_list_or_item(p,bind(&InstrFieldHandler::record_masks_adapter,this,_1));   
      }
      else {
        ll = get_uint(l,"validity mask pattern.",env());
        hh = get_uint(h,"validity mask value.",env());
        _instrfield->_masks.push_back(make_pair(ll,hh));
      }
    }
    else {
      PError(p, "Parsing error in valid_masksf.");
    }  
  }  
  

  void InstrFieldHandler::add_blk(Ptree *p) 
  {
    _instrfield->_blks.push_back(get_ident(p,"block name."));
  }


  void InstrFieldHandler::record_fields_adapter(Ptree *p) 
  {
    record_fields(p,true);
  }

  bool InstrFieldHandler::record_fields(Ptree *p, bool error)
  {
    unsigned hi,lo;
    Ptree *n,*a,*b;
    if (get_uint(lo,p,env())) {
      _instrfield->_fields.push_back(InstrField::Field(lo,lo));
    } else if (Match(p,"[ ( [ %? , %? ] ) ]",&a,&b)) {
      if (get_uint(lo,a,env()) && get_uint(hi,b,env())) {
        _instrfield->_fields.push_back(InstrField::Field(lo,hi));
      } else {
        if (error) {
          PError(p,"Expected an integer for field index.");
        } else {
          return false;
        }
      }

    } else if (Match(p,"[ %? ( [ %?, %? ] ) ]",&n,&a,&b)) {
      const char *name = get_simple_ident(n,"instruction field name.");
      if (get_uint(lo,a,env()) && get_uint(hi,b,env())) {
        _instrfield->_fields.push_back(InstrField::Field(name,lo,hi));
      } else {
        if (error) {
          PError(p,"Expected an integer for field index.");
        } else {
          return false;
        }
      }
    } else if (Match(p,"[ %? ( [ %? ] ) ]",&n,&a)) {
      const char *name = get_simple_ident(n,"instruction field name.");
      if (get_uint(lo,a,env())) {
        _instrfield->_fields.push_back(InstrField::Field(name,lo));
      } else {
        if (error) {
          PError(p,"Expected an integer for field index.");
        } else {
          return false;
        }
      }
    } else {
      const char *name = p->ToString();
      if (is_ident(name)) {
        _instrfield->_fields.push_back(InstrField::Field(name));
      } else {
        if (error) {
          PError(p,"Expected idnetifier for instruction field name.");
        } else {
          return false;
        }
      }
    }
    return true;
  }

  void InstrFieldHandler::record_table_adapter(Ptree *p) 
  {
    record_table(p,true);
  }

  bool InstrFieldHandler::record_table(Ptree *p,bool error) 
  {
    gc_string n = get_str_name(p) ;
    if (n == "reserved") {
      _instrfield->_table.add_reserved();
    } else {
      _instrfield->_table.add_entry();
      handle_list_or_item(p,bind(&InstrFieldHandler::add_table_entry,this,_1));
    }
    return true;
  }

  void InstrFieldHandler::add_table_entry(Ptree *e)
  {
    _instrfield->_table.add_to_entry(get_uint(e,"table entry field.",env()));
  }

  void InstrFieldHandler::process_keys(MsgLog &log)
  {
    RemoveHandler::process_keys(log);

    process_named_handler(log,_instrfields,_instrfield->_instrfields,*this,&InstrFieldHandler::setup_instrfield_handler);
  }

  void InstrFieldHandler::setup_instrfield_handler(InstrFieldHandler &ifh, InstrField &ifield) 
  {
    ifield._doc += ifh.doc();
    ifh._core = _core;
    ifh._instrfield = &ifield;
  }

  bool InstrFieldHandler::has_attr(const AttrItem &ai) const
  {
    return (_instrfield) ? (bool)_instrfield->has_attr(ai) : false;
  }

  // 
  // Relocation handler.
  //
  void RelocationHandler::handle_key(const char *key, Opencxx::Ptree *value)
  {
    if (!strcmp(key,"abbrev")) {
      _reloc->_abbrev = get_ident(value,"relocation abbreviation.");
    } else if (!strcmp(key,"value")) {
      _reloc->_value = get_uint(value,"relocation value.",env());
    } else if (!strcmp(key,"width")) {
      _reloc->_width = get_uint(value,"instruction width.",env());
    } else if (!strcmp(key,"field_width")) {
      _reloc->_field_width = get_uint(value,"field width.",env());
    } else if (!strcmp(key,"pcrel")) {
      _reloc->_pcrel = get_bool(value,"pc-relative flag.",env());
    } else if (!strcmp(key,"right_shift")) {
      _reloc->_right_shift = get_uint(value,"relocation value right shift.",env()); // Specifies the number of bits the relocation value is right-shifted before it is encoded
    } else if (!strcmp(key,"right_shift_exp")) {
      _reloc->_right_shift_exp = get_str(value,"relocation value right shift expression."); // Specifies the expression for the number of bits the relocation value is right-shifted before it is encoded
    } else if (!strcmp(key,"offset")) {
      _reloc->_offset = get_uint(value,"relocation offset.",env()); // Specifies the offset relative to the width of the instruction
    } else if (!strcmp(key,"bitpos")) {
      _reloc->_bitpos = get_uint(value,"bit position.",env());
    } else if (!strcmp(key,"is_signed")) {
      _reloc->_is_signed = get_bool(value,"signed flag.",env());
    } else if (!strcmp(key,"check_overflow")) {
      _reloc->_check_overflow = get_bool(value,"check-overflow flag.",env());
    } else if (!strcmp(key,"src_mask")) {
      _reloc->_src_mask = get_uint64(value,"relocation source mask.",env());
    } else if (!strcmp(key,"dst_mask")) {
      _reloc->_dst_mask = get_uint64(value,"relocation destination mask.",env());
    } else if (!strcmp(key,"action")) {
      _reloc->_action = get_func(value,2,"action.");
    } else if (!strcmp(key,"instrfield")) {
      // We'll process this later- this could be a simple identifier or a pair
      // of identifiers to identify a micro-op field and a macro-op field.
      _reloc->_field._p = value;
    } else if (handle_attrs(key,value,*_reloc,env())) {
      ;    
    } else {
      unknown_key(value,key);
    }
  }

  bool RelocationHandler::has_attr(const AttrItem &ai) const
  {
    return (_reloc) ? (bool)_reloc->has_attr(ai) : false;
  }


  //
  // InstrBase handler.
  //

  void InstrBaseHandler::add_blk(Ptree *p) 
  {
    _instrbase->_blks.push_back(get_ident(p,"block name."));
  }

  void InstrBaseHandler::add_pfx_ctr(Ptree *p)
  {
    Ptree *pc, *f;
    if ( Match(p,"[ ( [ %? , %? ] ) ]",&pc,&f) ) {
      gc_string counter = get_ident(pc,"prefix counter.");
      gc_string field = get_ident(f,"prefix counter field binding.");
      _instrbase->_prefix_counters.push_back(make_pair(counter,field));
    } else {
      PError(p,"Expected a '( prefix-counter , instruction-field )' for a prefixcounter entry.");
    }
  }

  void InstrBaseHandler::add_pfx_ctr_incr(Ptree *p) 
  {
    _instrbase->_prefix_counter_mods.push_back(make_pair(get_ident(p,"prefix counter."),true));
  }

  void InstrBaseHandler::add_pfx_ctr_decr(Ptree *p) 
  {
    _instrbase->_prefix_counter_mods.push_back(make_pair(get_ident(p,"prefix counter."),false));
  }

  void InstrBaseHandler::handle_key(const char *key,Ptree *value,bool instr)
  {
    if (!strcmp(key,"fields")) {
      handle_list_or_item(value,bind(&InstrBaseHandler::add_field,this,_1));
    }
    else if (!strcmp(key,"prefix_counters")) {
      handle_list_or_item(value,bind(&InstrBaseHandler::add_pfx_ctr,this,_1));
    }
    else if (!strcmp(key,"prefix_counter_incr")) {
      handle_list_or_item(value,bind(&InstrBaseHandler::add_pfx_ctr_incr,this,_1));
    }
    else if (!strcmp(key,"prefix_counter_decr")) {
      handle_list_or_item(value,bind(&InstrBaseHandler::add_pfx_ctr_decr,this,_1));
    }
    else if (!strcmp(key,"syntax")) {
      handle_syntax(value,instr,false);
    }
    else if (!strcmp(key,"dsyntax")) {
      handle_syntax(value,instr,true);
    }
    else if (!strcmp(key,"blk")) {
      handle_list_or_item(value,bind(&InstrBaseHandler::add_blk,this,_1));
    }
    else {
      unknown_key(value,key);
    }
  }

  unsigned binGc_StringToInt(const char* bit) 
  {
    unsigned val = 0;
    while(*bit != '\0') {
      val = (val << 1) + (*bit - '0');
      ++bit;
    }
    return val;
  }

  bool  isBinary(const char* s) 
  {
    if (*s != 'b' || *(++s) == '\0') {
      return false;
    }
    return (strspn(s,"01") == strlen(s));
  }


  enum ArgType { atField, atOpc, atTmpField };

  pair<ArgType,InstrBase::FieldBase *> add_field_slice(Ptree *b,Ptree *f,Ptree *v,bool allow_inits,Environment *env)
  {
    Ptree *lb, *hb, *n;
    unsigned lo,hi;
    unsigned value;
    if (Match(b,"[bits ( [ %? , %? ] ) ]",&lb,&hb)) {
      // Multi-bits specification, e.g, (bits(0,3),D(0,1)) or (bits(4,5),b01) or (bits(5,5),reserved)
      if (!get_uint(lo,lb,env) || !get_uint(hi,hb,env)) {
        PError(b,"Expected integers for bit field index.");   
      }
    }
    else if (Match(b,"[bits ( [ %? ] ) ]",&lb)) {
      // Single bit specification.
      if (get_uint(lo,lb,env)) {
        hi = lo;
      } else {
        PError(b,"Expected integers for bit field index.");   
      }
    } else {
      PError(b,"Illegal fields syntax.");
    }

    // Integer value   (bits(1,2),3))
    unsigned lf,hf;
    InstrBase::TmpField *field = 0;
    bool val_ok = true;
    char *bit = 0;
    if (get_uint(value,f,env)) { 
      field = new InstrBase::TmpField(value,lo,hi);
    } // 
    else if (Match(f,"[ %? ( [ %? , %? ] ) ]",&n,&lb,&hb)) {
      const char *name = get_simple_ident(n,"instruction field name");
      if (!get_uint(lf,lb,env) || !get_uint(hf,hb,env)) {
        PError(f,"Expected integers for bit field index.");   
      }
      field = new InstrBase::TmpField(name,lo,hi,lf,hf);
    }
    else if (Match(f,"[ %? ( [ %? ] ) ]",&n,&lb)) {
      const char *name = get_simple_ident(n,"instruction field name");
      if (!get_uint(lf,lb,env)) {
        PError(f,"Expected integers for bit field index.");   
      }
      field = new InstrBase::TmpField(name,lo,hi,lf,lf);
    }
    else if (f->ToString() == gc_string("reserved")) {
      field = new InstrBase::TmpField(lo,hi);
      val_ok = false;
    }
    else if ((bit = f->ToString()) && isBinary(bit)) {
      value = binGc_StringToInt(++bit);
      field = new InstrBase::TmpField(value,lo,hi);
      val_ok = false;
    }
    else {
      const char *name = get_simple_ident(f,"instruction field name.");
      field = new InstrBase::TmpField(name,lo,hi);
    }

    if (v) {
      if (!allow_inits || !val_ok) {
        PError(v,"A value may not be specified at this point.");
      }
      if (!get_uint(value,v,env)) {
        if ((bit = f->ToString()) && isBinary(bit)) {
          value = binGc_StringToInt(++bit);
        } else {
          PError(v,"Unknown value specified for field.");
        }
      }
      field->set_value(value);
    }

    return make_pair(atTmpField,field);
  }

  // Tries to convert a Ptree to an instruction field.  If the value is
  // a number, then we push that back, otherwise we push back the name.
  // For instructions, all Fields are stored in the same vector, 
  // will be splitted to fields and opcs at finalize. (i.e.fields == opcs == all)
  // Thus,  the original order is maintained. This is is required for handling subinstr with opcs
  // For aliases fields are splitted.
  pair<ArgType,InstrBase::FieldBase *> add_to_fields(Ptree *p,bool allow_inits,Environment *env)
  {
    Ptree *b, *f,*a, *v;
    unsigned value;
    if (get_uint(value,p,env)) {
      // Handle integer values.
      return make_pair(atField,new InstrBase::Field(0,value));
    } else if (const char *name = get_simple_ident(p,0)) {
      // Simple identifier.
      return make_pair(atField,new InstrBase::Field(name));
    } else if (allow_inits && Match(p,"[%? ( [ %? ] )]",&f,&a)) {
      // An initialization form, e.g. X(10).
      const char *name = get_simple_ident(f,"instruction field name.");
      unsigned arg = get_uint(a,"instruction field argument.",env);
      return make_pair(atOpc,new InstrBase::Field(name,arg));
    } else if (Match(p,"[ ( [ [ %? , %? ] , %? ] ) ]",&b, &f, &v)) { 
      // A per-bit field encoding with a value, e.g. (bits(0,1),Foo(3,4),2)
      return add_field_slice(b,f,v,allow_inits,env);
    } else if (Match(p,"[ ( [ %? , %? ] ) ]",&b, &f)) { 
      // A per-bit field encoding, e.g. (bits(0,1),Foo(3,4))
      return add_field_slice(b,f,0,allow_inits,env);
    } else if (Match(p,"[ ( %? ) ]",&f)) {
      // Remove parentheses.
      return add_to_fields(f,allow_inits,env);
    } else {
      // Might be an expression- store it and process it later.
      return make_pair(atField,new InstrBase::Field(0,p));
    }
  }


  void InstrBaseHandler::add_field(Ptree *p) 
  {
    _instrbase->_init_all.push_back(add_to_fields(p,true,env()).second);
  }

  // This should be a list, where the first element is a gc_string
  // and the remaining elements are identifiers which name instruction fields.
  // Here, we just do basic syntax ng.
  // write or to assempbler syntax (default) or to disassembler syntax
  void InstrBaseHandler::handle_syntax(Ptree *value,bool instr,bool is_dsyntax) 
  {
    auto items = new PtreeArray;
    handle_list_or_item(value,bind(add_syntax_item,_1,ref(*items)));
  
    auto syntax = new InstrBase::Syntax;
    if (items->Number() == 0) {
      PError(value,"Bad syntax specifier:  Expected a list with at least one element.");
    }

    // First item should be a gc_string.
    const char *tmp;
    if ((tmp = get_str((*items)[0],0))) {
      syntax->_fmt = tmp;
    }

    // Each subsequent item should identify a field.  This will
    // be figured out during finalization.
    syntax->_orig = items;
    
    if (is_dsyntax) {
      _instrbase->_dsyntax = syntax;
    }
    else {
      _instrbase->_syntax = syntax;
    }
  }

  bool InstrBaseHandler::has_attr(const AttrItem &ai) const
  {
    return (_instrbase) ? (bool)_instrbase->has_attr(ai) : false;
  }

  bool InstrBaseHandler::has_field(const gc_string &field) const
  {
    return (_instrbase) ? (bool)_instrbase->find_bfield(field) : false;
  }

  //
  // SubInstruction handler.
  //


  void SubInstrHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"action")) {
      _instrbase->_action = get_func(value,-1,"sub-instruction action.");
    } else if (handle_attrs(key,value,*_subinstr,env())) {
      ;
    } else {
      InstrBaseHandler::handle_key(key,value,false);
    }
  }

  void SubInstrHandler::set_instrbase(SubInstr *subinstr)
  {
    _instrbase = _subinstr = subinstr;
    set_aspects(subinstr);
  }

  //
  // SubInstructionGroup handler
  //


  void SubInstrGrpHandler::handle_key(const char* key,Ptree *value) 
  {
    if (!strcmp(key,"subs")) {
      handle_list_or_item(value,bind(&SubInstrGrpHandler::record_subinstr_names,this,_1));
    } else {
      RemoveHandler::handle_key(key,value);
    }
  }

  void  SubInstrGrpHandler::record_subinstr_names(Ptree *p)
  {
    auto name = get_ident(p,"subinstruction name.");
    _subinstrgrp->_names.push_back(name);
  }

  //
  // Instruction handler.
  //

  void InstrHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"width")) {
      if (!strcmp(value->ToString(),"variable")) {
        _instr->_need_encoding = true;
      } else {
        _instr->_width = get_uint(value,"instruction width value.",env());
      }
      if (_instr->_width == 0 && !_instr->_need_encoding) {
        PError(value,"Width must be a non-zero bit count.");
      }
    }
    else if (!strcmp(key,"doc_op")) {
      _instr->_doc_op = get_str(value,"documentation operation description.");
    }
    else if (!strcmp(key,"action")) {
      // action : should be a 0-ary function
      _instrbase->_action = get_func(value,0,"instruction action.");
    }
    else if (!strcmp(key,"assemble")) {
      _instr->_assemble = get_bool(value,"assemble flag.",env());
    }
    else if (!strcmp(key,"asm_rank")) {
      _instr->_asm_rank = get_uint(value,"assembly rank.",env());
    }         
    else if (!strcmp(key,"disassemble")) {
      _instr->_disassemble = get_bool(value,"disassemble flag.",env());
    }
    else if (!strcmp(key,"allow_conflict")) {
      _instr->_allow_conflict = get_bool(value,"allow-conflict flag",env());
    }
    else if (!strcmp(key,"assembler")) {
      _instr->_assembler = get_func(value,0,"instruction assembler");
    }
    else if (!strcmp(key,"alias_action")) {
      _instr->_alias_action = get_func(value,1,"instruction assembler");
    }    
    else if (!strcmp(key,"names")) {
      _instr->_names.clear();
      handle_list_or_item(value,bind(&InstrHandler::record_instr_names,this,_1));
    }
    else if (!strcmp(key,"next_table")) {
      _instr->_next_table = get_func(value,0,"next-table function.");
    }
    else if (!strcmp(key,"alias")) {
      // alias: should be a function call
      handle_list_or_item(value,bind(&InstrHandler::handle_alias,this,_1));
    } 
    else if (!strcmp(key,"prefix")) {
      _instr->_prefix = get_bool(value,"prefix flag.",env());
    } 
    else if (!strcmp(key,"pseudo")) {
      _instr->_pseudo = get_bool(value,"pseudo flag.",env());
    }
    else if (!strcmp(key,"type")) {
      _instr->_type_name = get_ident(value,"instruction type.");
    }
    else if (!strcmp(key,"exclude_sources")) {
      handle_list_or_item(value,bind(&InstrHandler::record_res_used,this,_1,ref(_instr->_orig_source_excludes)));
    }
    else if (!strcmp(key,"exclude_targets")) {
      handle_list_or_item(value,bind(&InstrHandler::record_res_used,this,_1,ref(_instr->_orig_target_excludes)));
    }
    else if (!strcmp(key,"sources")) {
      handle_list_or_item(value,bind(&InstrHandler::record_res_used,this,_1,ref(_instr->_orig_sources)));
    }
    else if (!strcmp(key,"targets")) {
      handle_list_or_item(value,bind(&InstrHandler::record_res_used,this,_1,ref(_instr->_orig_targets)));
    } 
    else if (!strcmp(key,"asm_rules")) {
      handle_list_or_item(value,bind(&InstrHandler::record_asm_rules,this,_1));
    }
    else if (!strcmp(key,"dependencies")) {
      _instr->_dependencies = new Instr::Dependencies(value);
    }
    else if (!strcmp(key,"reserved_bits_value")) {
      _instr->_reserved_bits_value = get_uint(value,"reserved-bits value.",env());
    }
    else if (!strcmp(key,"enable_post_fetch")) {
      bool v;
      if (get_func(value,0,0)) {
        _instr->_enable_post_fetch = Instr::PostFetchFunc;
        _instr->_enable_post_fetch_func = value;
      } else if (get_bool(v,value,env())) {
        _instr->_enable_post_fetch = (v) ? Instr::PostFetchTrue : Instr::PostFetchFalse;
        _instr->_enable_post_fetch_func = 0;
      } else {
        PError(value,"Bad enable-post-fetch value; expected bool or enable function.");
      }
    }
    else if (handle_attrs(key,value,*_instr,env())) {
      ;
    } 
    else {
      InstrBaseHandler::handle_key(key,value,true);
    }
  }

  Handler *InstrHandler::add_define(MsgLog &ml,Environment *e,Ptree *p,const char *type, 
                                    const char *name,bool modify)
  {
    if (!strcmp(type,"subinstrs")) {
      if (!name) {
        PError(p,"Subinstrs defined without a name.");
      }
      SubInstrGrpHandler *sigh = new SubInstrGrpHandler(e,p,name,modify);
      _subinstrgrps.push_back(sigh);
      return sigh;
    } else if (!strcmp(type,"instr")) {
      check_ident(name,p,"Instrs",false);
      auto ih =  new InstrHandler(e,p,name,modify);
      _instrs.push_back(ih);
      return ih;
    } else {
      return InstrBaseHandler::add_define(ml,e,p,type,name,modify);
    }
    abort();
  }

  void InstrHandler::record_res_used(Ptree *p,PList &res)
  {
    res.push_back(p);
  }

  void InstrHandler::record_instr_names(Ptree *p)
  {
    auto name = get_ident(p,"instruction name.");
    _instr->_names.push_back(name);
  }

  void InstrHandler::record_asm_rules(Ptree *p) 
  {
    auto name = get_ident(p,"assembler rule name.");
    _instr->_asmrules.push_back(name);
  }

  void InstrHandler::handle_alias(Ptree *p)
  {
    Ptree *instr, *args;
    // We expect it to be a function call.
    if (Match(p,"[%? ( %? )]",&instr,&args)) {  
      auto alias = new Instr::TmpAlias;
      alias->_name = get_str(instr,"alias name.");
      // Iterate over the arguments.
      while (args) {
        Ptree *arg = args->Car();
        args = args->Cdr();
        Ptree *name,*value;
        if (Match(arg,"[ %? ( [ %? ] ) ]",&name,&value)) {
          const char *nm = get_ident(name,"alias target's field name.");
          auto r = add_to_fields(value,false,env());
          switch(r.first) {
          case atField:
            alias->_fields.push_back(*InstrBase::get_field(r.second));
            alias->_field_names.push_back(nm);
            break;
          case atOpc:
            // Ignored b/c we don't allow this.  We do add it to the opcode array,
            // though, so that we'll detect this as an error later.
            alias->_opcs.push_back(*InstrBase::get_field(r.second));
            break;
          case atTmpField:          
            PError(arg,"Expected a field mapping of the form 'target-field(alias-value)'.");
            break;
          }
        } else {
          PError(arg,"Expected a field mapping of the form 'target-field(alias-value)'.");
        }
        // Skip the ",".
        if (args) {
          args = args->Cdr();
        }
      }
      _instr->_tmpalias.push_back(alias);
    } else {
      PError(loc(),"Expected a function-call form for instruction alias.");
    }
  }


  // This scans through all subinstruction groups
  void  InstrHandler::process_keys(MsgLog &log)
  {
    InstrBaseHandler::process_keys(log);
  
    // Handle subinstruction groups
    process_named_handler(log,_subinstrgrps,_instr->_subinstrgrps,*this,&InstrHandler::setup_subinstrgrp_handler);
    // Handle nested instructions
    process_named_handler(log,_instrs,_instr->_instrs,*this,&InstrHandler::setup_instr_handler);
  }

  void  InstrHandler::setup_subinstrgrp_handler(SubInstrGrpHandler& sigh, SubInstrGrp &sig)
  {
    sig._doc  += sigh.doc();
    sigh._subinstrgrp = &sig;
  }

  void InstrHandler::setup_instr_handler(InstrHandler &ih, Instr  &instr) 
  {
    instr._doc += ih.doc();
    ih._core = _core;
    ih.set_instrbase(&instr);
  }

  void  InstrHandler::set_instrbase(Instr *instr)
  {
    _instrbase = _instr = instr;
    set_aspects(instr);
  }


  //
  // Exception handler.
  //

  Handler *ExceptionHandler::add_define(MsgLog &ml,Environment *e,Ptree *p,const char *type,const char *name,bool modify)
  {
    if (!strcmp(type,"field")) {
      check_ident(name,p,"Exception field object");
      auto cfh = new ExceptionFieldHandler(e,p,name,modify);
      _fields.push_back(cfh);
      return cfh;
    }
    else {
      return AspectRemoveHandler::add_define(ml,e,p,type,name,modify);
    }
  }

  void ExceptionHandler::process_keys(MsgLog &log)
  {
    AspectRemoveHandler::process_keys(log);

    process_named_handler(log,_fields,_exception->_fields,
                          *this,&ExceptionHandler::setup_field_handler);
  }

  void ExceptionHandler::setup_field_handler(ExceptionFieldHandler &efh,ExceptionField &ef)
  {
    ef._doc     = efh.doc();
    efh._field = &ef;  
  }

  void ExceptionHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"priority")) {
      _exception->_priority = get_uint(value,"exception priority.",env());
    } 
    else if (!strcmp(key,"sensitivity")) {
      const char *s = get_ident(value,"exception sensitivity type");
      if (!strcmp(s,"edge")) {
        _exception->_sensitivity = EdgeSensitive;
      } 
      else if (!strcmp(s,"level")) {
        _exception->_sensitivity = LevelSensitive;
      } else {
        PError(value,"Unknown sensitivity type '" << value << "'.  Expected 'edge' or 'level'.");
      }
    } 
    else if (!strcmp(key,"enable")) {
      // enable:  Should be a 0-ary function.
      _exception->_enable = get_func(value,0,"exception enable predicate.");
    } 
    else if (!strcmp(key,"action")) {
      // action: Arity depends upon whether we have fields, so checking is
      // delayed until the finalize routine.
      _exception->_action = get_func(value,-1,"exception action.");
    } 
    else if (handle_attrs(key,value,*_exception,env())) {
      ;
    }
    else {
      unknown_key(value,key);
    }
  }

  bool ExceptionHandler::has_attr(const AttrItem &ai) const
  {
    return (_exception) ? (bool)_exception->has_attr(ai) : false;
  }

  //
  // MMU handler.
  // 

  Handler *MmuHandler::add_define(MsgLog &ml,Environment *e,Ptree *p,const char *type,const char *name,bool modify)
  {
    if (!strcmp(type,"lookup")) {
      check_ident(name,p,"MMU lookup object");
      auto mlh = new MmuLookupHandler(e,p,name,modify);
      _lookups.push_back(mlh);
      return mlh;
    }
    else if (Handler *h = AspectGrpHandler::add_define(ml,e,p,type,name,modify)) {
      return h;
    }
    else {
      return DocHandler::add_define(ml,e,p,type,name,modify);
    }
  }

  void MmuHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"both_enable")) {
      _mmu->_instr_enable._func = _mmu->_data_enable._func = get_func(value,0,"enable test.");
    }
    else if (!strcmp(key,"instr_enable")) {
      _mmu->_instr_enable._func = get_func(value,0,"instruction enable test.");
    }
    else if (!strcmp(key,"data_enable")) {
      _mmu->_data_enable._func = get_func(value,0,"data enable test.");
    }
    else if (!strcmp(key,"multi_hit")) {
      _mmu->_multi_hit._func = get_func(value,3,"multiple-hit action.");
    } 
    else if (!strcmp(key,"final_hit")) {
      _mmu->_final_hit._func = get_func(value,4,"final-hit action.");
    } 
    else if (!strcmp(key,"instr_miss")) {
      _mmu->_instr_miss._func = get_func(value,2,"instruction-translation miss action.");
    } 
    else if (!strcmp(key,"data_miss")) {
      _mmu->_data_miss._func = get_func(value,2,"data-translation miss action.");
    } 
    else if (!strcmp(key,"load_miss")) {
      _mmu->_load_miss._func = get_func(value,2,"data-translation load miss action.");
    } 
    else if (!strcmp(key,"store_miss")) {
      _mmu->_store_miss._func = get_func(value,2,"data-translation store miss action.");
    } 
    else if (!strcmp(key,"aligned_write")) {
      _mmu->_aligned_write._func = get_func(value,2,"aligned-write handler.");
    }
    else if (!strcmp(key,"misaligned_write")) {
      _mmu->_misaligned_write._func = get_func(value,2,"misaligned-write handler.");
    }
    else if (!strcmp(key,"misaligned_read")) {
      _mmu->_misaligned_read._func = get_func(value,2,"misaligned-read handler.");
    }
    else if (!strcmp(key,"pre_read")) {
      _mmu->_pre_read._func = get_func(value,2,"pre-read handler.");
    }
    else if (!strcmp(key,"post_read")) {
      _mmu->_post_read._func = get_func(value,2,"post-read handler.");
    }
    else if (!strcmp(key,"interleaved_fetch")) {
      _mmu->_interleaved_fetch = get_bool(value,"interleaved-fetch flag.",env());
    }
    else if (!strcmp(key,"interleaved_reads")) {
      _mmu->_interleaved_reads = get_bool(value,"interleaved-reads flag.",env());
    }
    else if (!strcmp(key,"interleaved_writes")) {
      _mmu->_interleaved_writes = get_bool(value,"interleaved-writes flag.",env());
    }
    else if (!strcmp(key,"pre_write")) {
      _mmu->_pre_write._func = get_func(value,2,"pre-write handler.");
    }
    else if (!strcmp(key,"post_write")) {
      _mmu->_post_write._func = get_func(value,2,"post-write handler.");
    }
    else if (handle_attrs(key,value,*_mmu,env())) {
      ;
    }
    else {
      unknown_key(value,key);
    }
  }

  void MmuHandler::process_keys(MsgLog &log)
  {
    DocHandler::process_keys(log);
    AspectGrpHandler::process_keys(log);

    process_named_handler(log,_lookups,_mmu->_top_lookups,
                          *this,&MmuHandler::setup_lookup_handler);
  }

  void handle_lookup_inheritance(MmuLookupHandler &mlh,MmuLookup &ml,MmuLookupHash &lookups)
  {
    // We check to see if this item inherits from another.  If it does, then we
    // copy over that item's stuff.
    if (Opencxx::Ptree *parent = mlh.inherit()) {
      const char *pname = get_ident(parent," parent lookup object name.");
      // Try to find this object- it's an error if we can't.
      auto piter = lookups.find(pname);
      if (piter == lookups.end()) {
        PError(parent,"Unknown parent lookup object.");
      }
      // Now copy the object over this one.
      ml.copy(*(piter->second));
    }

  }

  void MmuHandler::setup_lookup_handler(MmuLookupHandler &mlh,MmuLookup &ml)
  {
    handle_lookup_inheritance(mlh,ml,_mmu->_top_lookups);

    ml._doc     = mlh.doc();
    mlh._lookup = &ml;
    mlh.set_aspects(&ml);
  }

  //
  // MMU test-set handler.
  //
  void MmuTestSetHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"enable")) {
      _test_set->_enable._func = get_func(value,1,"test-set enable predicate.");
    }
    else if (!strcmp(key,"test")) {
      _test_set->_tests.push_back(get_func_call(value,"matching-test."));
    }
    else if (handle_attrs(key,value,*_test_set,env())) {
      ;
    }
    else {
      unknown_key(value,key);
    }
  }


  //
  // MMU lookup handler.
  // 

  Handler *MmuLookupHandler::add_define(MsgLog &ml,Environment *e,Ptree *p,const char *type,const char *name,bool modify)
  {
    if (!strcmp(type,"array")) {
      if (name) {
        PError(p,"Type 'array' does not take a name.");
      }
      auto x = new MmuArrayHandler(e,p);
      _arrays.push_back(x);
      return x;
    }
    else if (!strcmp(type,"setfield")) {
      check_ident(name,p,"MMU set field object");
      auto mfh = new MmuFieldHandler(e,p,name,modify);
      _set_fields.push_back(mfh);
      return mfh;
    }
    else if (!strcmp(type,"wayfield")) {
      check_ident(name,p,"MMU way field object");
      auto mfh = new MmuFieldHandler(e,p,name,modify);
      _way_fields.push_back(mfh);
      return mfh;
    }
    else if (!strcmp(type,"lookup")) {
      check_ident(name,p,"MMU lookup object");
      auto mlh = new MmuLookupHandler(e,p,name,modify);
      _lookups.push_back(mlh);
      return mlh;
    }
    else if (!strcmp(type,"tests")) {
      check_ident(name,p,"MMU test-set object");
      auto mtsh = new MmuTestSetHandler(e,p,name,modify);
      _test_sets.push_back(mtsh);
      return mtsh;
    }
    else {
      return AspectRemoveHandler::add_define(ml,e,p,type,name,modify);
    }
  }

  void MmuLookupHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"type")) {
      const char *t = get_simple_ident(value,"type identifier.");
      if (!strcmp(t,"Both")) {
        _lookup->_type = MmuLookup::Both;
      } 
      else if (!strcmp(t,"Instr")) {
        _lookup->_type = MmuLookup::Instr;
      } 
      else if (!strcmp(t,"Data")) {
        _lookup->_type = MmuLookup::Data;
      }
      else {
        PError(value,"Unknown lookup type.");
      }
    }
    else if (!strcmp(key,"priority")) {
      uint32_t pr;
      if (get_uint(pr,value,env())) {
        _lookup->_priority = pr;
      } else if (const char *pi = get_ident(value,0)) {
        if (strcmp(pi,"ignore") == 0) {
          _lookup->_priority = -1;
        } else {
          PError(value,"Unknown identifier '" << value << "' for MMU lookup priority.");
        }
      } else{
        PError(value,"Expected an integer or 'ignore' for MMU lookup priority.");
      }
    }
    else if (!strcmp(key,"interface")) {
      _lookup->_interface = get_bool(value,"interface.",env());
    }
    else if (!strcmp(key,"inherit")) {
      ; // Skip this key- it's handled by handle_lookup_inheritance.
    }
    else if (!strcmp(key,"size")) {
      uint32_t ps;
      if (get_uint(ps,value,env())) {
        // Page size is a constant value.
        _lookup->_size = ps;
        _lookup->_sizetype = MmuLookup::Const;
      } else {
        _lookup->_sizefield._p = value;
        _lookup->_sizetype = MmuLookup::BitSize;
      }
    }
    else if (!strcmp(key,"sizetype")) {
      const char *type = get_ident(value,"pagesize type identifier");
      if (!strcmp(type,"BitSize")) {
        _lookup->_sizetype = MmuLookup::BitSize;
      }
      else if (!strcmp(type,"LeftMask")) {
        _lookup->_sizetype = MmuLookup::LeftMask;
      }
      else {
        PError(value,"Unknown page-size type '" << type << "'.");
      }
    }
    else if (!strcmp(key,"sizescale")) {
      _lookup->_sizescale = get_uint(value," page-size scale value.",env());
    }
    else if (!strcmp(key,"sizeoffset")) {
      _lookup->_sizeoffset = get_uint(value," page-size base value.",env());
    }
    else if (!strcmp(key,"sizeshift")) {
      _lookup->_sizeshift = get_uint(value," page-size shift value.",env());
    }
    else if (!strcmp(key,"pageshift")) {
      _lookup->_pageshift = get_uint(value," page-shift value.",env());
    }
    else if (!strcmp(key,"realpage")) {
      _lookup->_realpage._p = value;
    } 
    else if (!strcmp(key,"valid")) {
      _lookup->_valid._func = get_func(value,0,"translation-valid predicate");;
    } 
    else if (!strcmp(key,"test")) {
      // For compatibility, if we have test elements directly within the lookup,
      // these are added to a specially named default object.
      auto iter = _lookup->_tests.find(DefaultTestSet);
      if (iter == _lookup->_tests.end()) {
        iter = _lookup->_tests.insert(make_pair(DefaultTestSet,new MmuTestSet(_env,value,DefaultTestSet))).first;
      }
      iter->second->_tests.push_back(get_func_call(value," matching-test."));
    }
    else if (!strcmp(key,"test_order")) {
      if (!_lookup->_tmp_test_order) {
        _lookup->_tmp_test_order = new StrList;
      }
      _lookup->_tmp_test_order->clear();
      handle_list_or_item(value,bind(&MmuLookupHandler::record_test_order,this,_1));
    }
    else if (!strcmp(key,"multi_hit")) {
      _lookup->_multi_hit._func = get_func(value,3,"multiple-hit action.");
    }
    else if (!strcmp(key,"final_hit")) {
      _lookup->_final_hit._func = get_func(value,4,"final-hit action.");
    }
    else if (!strcmp(key,"hit")) {
      _lookup->_hit._func = get_func(value,-1,"lookup hit-handler.");
    }
    else if (!strcmp(key,"miss")) {
      _lookup->_miss._func = get_func(value,3,"lookup miss-handler.");
    }
    else if (!strcmp(key,"reset")) {
      _lookup->_reset._func = get_func(value,0,"lookup reset-handler.");
    }
    else if (!strcmp(key,"exec_perm")) {
      _lookup->_exec_perm._func = get_func(value,-1,"execution permission function.");
    }
    else if (!strcmp(key,"data_perm")) {
      _lookup->_load_perm._func = _lookup->_store_perm._func = get_func(value,-1,"data permission function.");
    }
    else if (!strcmp(key,"load_perm")) {
      _lookup->_load_perm._func = get_func(value,-1,"data load permission function.");
    }
    else if (!strcmp(key,"store_perm")) {
      _lookup->_store_perm._func = get_func(value,-1,"data storage permission function.");
    }
    else if (!strcmp(key,"entry_mem_size")) {
      _lookup->_entry_mem_size = get_uint(value,"memory serialization size",env());
    } 
    else if (!strcmp(key,"mem_read")) {
      _lookup->_mem_read = get_func(value,3,"memory-read function");
    }
    else if (!strcmp(key,"mem_write")) {
      _lookup->_mem_write = get_func(value,3,"memory-write function");
    }
    else if (handle_attrs(key,value,*_lookup,env())) {
      ;
    }
    else {
      unknown_key(value,key);
    }
  }

  void MmuLookupHandler::record_test_order(Ptree *p) 
  {
    gc_string name = get_ident(p,"test-set name");
    _lookup->_tmp_test_order->push_back(name);
  }

  Opencxx::Ptree *MmuLookupHandler::inherit() const
  {
    ForEach(_keypairs,i) {
      if (!strcmp(i->first,"inherit")) {
        return i->second;
      }
    }
    return 0;
  }

  void MmuLookupHandler::process_keys(MsgLog &log)
  {
    AspectRemoveHandler::process_keys(log);

    process_named_handler(log,_test_sets,_lookup->_tests,
                          *this,&MmuLookupHandler::setup_test_set_handler);

    process_named_handler(log,_lookups,_lookup->_lookups,
                          *this,&MmuLookupHandler::setup_lookup_handler);

    process_named_handler(log,_set_fields,_lookup->_set_fields,
                          *this,&MmuLookupHandler::setup_field_handler);

    process_named_handler(log,_way_fields,_lookup->_way_fields,
                          *this,&MmuLookupHandler::setup_field_handler);

    ForEach(_arrays,i) {
      (*i)->_lookup = _lookup;
      LogPError(log,{
          (*i)->process_keys(log);
        });
    }

  }

  void MmuLookupHandler::setup_lookup_handler(MmuLookupHandler &mlh,MmuLookup &ml)
  {
    handle_lookup_inheritance(mlh,ml,_lookup->_lookups);

    ml._doc     = mlh.doc();
    mlh._lookup = &ml;  
    mlh.set_aspects(&ml);
  }

  void MmuLookupHandler::setup_field_handler(MmuFieldHandler &mfh,MmuField &mf)
  {
    mf._doc     = mfh.doc();
    mfh._field = &mf;  
  }

  void MmuLookupHandler::setup_test_set_handler(MmuTestSetHandler &mth,MmuTestSet &mt)
  {
    mt._doc       = mth.doc();
    mth._test_set = &mt;  
  }

  //
  // Array stuff within an MMU lookup object.
  //

  void MmuArrayHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"entries")) {
      _lookup->_array_entries = get_uint(value,"number of entries in array.",env());
    }
    else if (!strcmp(key,"set_assoc")) {
      _lookup->_array_assoc = get_uint(value,"array associativity.",env());
    } 
    else {
      unknown_key(value,key);
    }
  }

  //
  // Generic field handler.
  // 

  void GenericFieldHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"bits")) {
      _field->_bits = get_uint(value,"field bits specifier.",env());
    } 
    else if (!strcmp(key,"reset")) {
      // We allow either integers or identifiers (which represent enums).
      if (get_uintarb(value,0)) {
        _field->_resetvalue = value;
        _field->_int_reset = true;
      } else if ( get_ident(value,0)) {
        _field->_resetvalue = value;
        _field->_int_reset = false;
      } else {
        PError(value,"Expected an integer value or identifier for field reset-value.");
      }
    }
    else if (handle_attrs(key,value,*_field,env())) {
      ;
    }
    else {
      unknown_key(value,key);
    }
  }

  //
  // Cache handler.
  //

  Handler *CacheHandler::add_define(MsgLog &ml,Environment *e,Ptree *p,const char *type,const char *name,bool modify)
  {
    if (!strcmp(type,"setfield")) {
      check_ident(name,p,"Cache field object");
      auto cfh = new CacheFieldHandler(e,p,name,modify);
      _set_fields.push_back(cfh);
      return cfh;
    }
    else if (!strcmp(type,"wayfield")) {
      check_ident(name,p,"Cache field object");
      auto cfh = new CacheFieldHandler(e,p,name,modify);
      _way_fields.push_back(cfh);
      return cfh;
    }
    else {
      return AspectRemoveHandler::add_define(ml,e,p,type,name,modify);
    }
  }

  void CacheHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"level")) {
      _cache->_level = get_uint(value,"cache level.",env());
    } 
    else if (!strcmp(key,"type")) {
      _cache->_type = Cache::get_type(get_ident(value,"cache type"));
    }
    else if (!strcmp(key,"size")) {
      _cache->_size = get_uint(value,"cache size.",env());
    }
    else if (!strcmp(key,"linesize")) {
      _cache->_linesize = get_uint(value,"cacheline size.",env());
    }
    else if (!strcmp(key,"set_assoc")) {
      handle_list_or_item(value,bind(&CacheHandler::record_ways,this,_1));
    }
    else if (!strcmp(key,"sets")) {
      handle_list_or_item(value,bind(&CacheHandler::record_sets,this,_1));
    }
    else if (!strcmp(key,"sets_func")) {
      _cache->_sets_func = get_func(value,0,"number-of-sets specification function.");
    }
    else if (!strcmp(key,"assoc_func")) {
      _cache->_ways_func = get_func(value,0,"number-of-ways specification function.");
    }
    else if (!strcmp(key,"enable")) {
      _cache->_enable = get_func(value,-1,"enable function.");
    }
    else if (!strcmp(key,"hit")) {
      _cache->_hit = get_func(value,2,"cache-hit function.");
    }
    else if (!strcmp(key,"hit_pred")) {
      _cache->_hit_pred = get_func(value,3,"cache-hit predicate function.");
    }
    else if (!strcmp(key,"miss")) {
      _cache->_miss = get_func(value,2,"cache-miss function.");
    }
    else if (!strcmp(key,"invalidate_line")) {
      _cache->_invalidate_line = get_func(value,3,"invalidate-line function.");
    }
    else if (!strcmp(key,"read_line")) {
      _cache->_read_line = get_func(value,4,"read-line function.");
    }
    else if (!strcmp(key,"replace")) {
      _cache->_replace = get_func(value,2,"replacement-algorithm function.");
    }
    else if (!strcmp(key,"line_access")) {
      _cache->_line_access = get_func(value,4,"line-access function.");
    }
    else if (!strcmp(key,"write_through")) {
      _cache->_write_through = get_func(value,2,"write-through predicate.");
    }
    else if (!strcmp(key,"miss_enable")) {
      _cache->_miss_enable = get_func(value,3,"miss-enable predicate.");
    }
    else if (handle_attrs(key,value,*_cache,env())) {
      ;
    }
    else {
      unknown_key(value,key);
    }
  }

  void CacheHandler::record_ways(Ptree *p)
  {
    auto ways = get_uint(p,"cache set-associativity (way count).",env());
    _cache->_ways_list.push_back(ways);
  }

  void CacheHandler::record_sets(Ptree *p)
  {
    auto sets = get_uint(p,"cache set count.",env());
    _cache->_sets_list.push_back(sets);
  }

  void CacheHandler::process_keys(MsgLog &log)
  {
    AspectRemoveHandler::process_keys(log);

    process_named_handler(log,_set_fields,_cache->_set_fields,
                          *this,&CacheHandler::setup_field_handler);
    process_named_handler(log,_way_fields,_cache->_way_fields,
                          *this,&CacheHandler::setup_field_handler);
  }

  void CacheHandler::setup_field_handler(CacheFieldHandler &cfh,CacheField &cf)
  {
    cf._doc     = cfh.doc();
    cfh._field = &cf;  
  }

  bool CacheHandler::has_attr(const AttrItem &ai) const
  {
    return (_cache) ? (bool)_cache->has_attr(ai) : false;
  }

  //
  // Memory handler 
  //

  void  MemoryHandler::handle_key(const char* key, Opencxx::Ptree* value)
  {
    if(!strcmp(key,"instr_mem")) {
      _memory->_instr_mem = get_bool(value,"instruction memory.",env());
    }  else if(!strcmp(key,"size")) {
      _memory->_size = get_uint(value,"memory size.",env()) ;
    }  else if(!strcmp(key,"addr_unit")) {
      _memory->_addr_unit = get_uint(value,"memory address unit.",env()) ;
    }  else if(!strcmp(key,"parent")) {
      Ptree *m, *o;
      if(Match(value,"[ ( [ %? , %? ] ) ]",&m,&o)) {
        _memory->_parent_name = get_ident(m,"parent memory name.");
        _memory->_offset = get_uint(o,"parent memory offset.",env());
      } else {
        PError(value,"Bad parent value:  " << value << " (expected (<mem name>, <offset>) pair).")
          }
    } else if(!strcmp(key,"read")) {
      _memory->_read = new MemoryHook(_env,_loc,get_func(value,2,"read hook function."));
    } else if(!strcmp(key,"instr_read")) {
      _memory->_instr_read = new MemoryHook(_env,_loc,get_func(value,2,"read hook function."));
    } else if(!strcmp(key,"write")) {
      _memory->_write = new MemoryHook(_env,_loc,get_func(value,2,"write hook function."));
    } else if (!strcmp(key,"prefetch_mem")) {
      _memory->_prefetch_mem = get_bool(value,"prefetch memory.",env());
    } else {
      unknown_key(value,key) ;
    }
  }

  //
  // Event bus handler.
  //

  Handler *EventBusHandler::add_define(MsgLog &ml,Environment *e,Ptree *p,const char *type,const char *name,bool modify)
  {
    if (!strcmp(type,"field")) {
      check_ident(name,p,"event bus field object");
      auto efh = new EventBusFieldHandler(e,p,name,modify);
      _fields.push_back(efh);
      return efh;
    }
    else {
      return AspectRemoveHandler::add_define(ml,e,p,type,name,modify);
    }
  }

  void EventBusHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"action")) {
      _bus->_action = get_func(value,1,"action function.");
    }
    else if (handle_attrs(key,value,*_bus,env())) {
      ;
    }
    else {
      unknown_key(value,key);
    }
  }

  void EventBusHandler::process_keys(MsgLog &log)
  {
    AspectRemoveHandler::process_keys(log);

    process_named_handler(log,_fields,_bus->_fields,
                          *this,&EventBusHandler::setup_field_handler);
  }

  void EventBusHandler::setup_field_handler(EventBusFieldHandler &efh,EventBusField &field)
  {
    field._doc = efh.doc();
    efh._field = &field;  
  }
 
  bool EventBusHandler::has_attr(const AttrItem &ai) const
  {
    return (_bus) ? (bool)_bus->has_attr(ai) : false;
  }

  //
  // ExtResource handler.
  //

  void ExtResourceHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"objtype")) {
      _res->_objtype = get_ident(value,"external resource object type.");
    }
    else if (!strcmp(key,"external")) {
      _res->_external = get_bool(value,"external resource extern flag.",env());
    }
    else if (!strcmp(key,"reset")) {
      _res->_reset = get_bool(value,"external resource reset flag.",env());
    }
    else if (!strcmp(key,"constr_args")) {
      handle_list_or_item(value,bind(&ExtResourceHandler::record_constr_arg,this,_1));
    }
    else if (handle_attrs(key,value,*_res,env())) {
      ;
    }
    else {
      unknown_key(value,key);
    }
  }

  void ExtResourceHandler::record_constr_arg(Ptree *a)
  {
    _res->_constr_args.push_back(a);
  }
 
  bool ExtResourceHandler::has_attr(const AttrItem &ai) const
  {
    return (_res) ? (bool)_res->has_attr(ai) : false;
  }

 
  //
  // Group handler.
  //
 
  void GroupHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"items")) {
      handle_list_or_item(value,bind(&GroupHandler::record_items,this,_1));
    }
    else if (!strcmp(key,"has_attrs")) {
      handle_list_or_item(value,bind(&GroupHandler::record_attr_filter,this,_1,ref(_group->_attr_filter)));
    }
    else if (!strcmp(key,"has_fields")) {
      handle_list_or_item(value,bind(&GroupHandler::record_filter,this,_1,ref(_group->_field_filter)));
    }
    else if (!strcmp(key,"check_parent")) {
      _group->_check_parent = get_bool(value,"check-parent flag.",env());
    }
    else if (!strcmp(key,"type")) {
      _group->_type = get_str(value,"group type");    
    }
    else if (handle_attrs(key,value,*_group,env())) {
      ;
    } else {
      unknown_key(value,key);
    }
  }

  void GroupHandler::record_attr_filter(Ptree *p,AttrItems &filters)
  {
    filters.push_back(parse_attr_item(p,false,env()));
  }

  void GroupHandler::record_filter(Ptree *p,StrSet &filters)
  {
    filters.insert(get_str(p,"filter item."));
  }
  
  void GroupHandler::record_items(Ptree *p)
  {     
    _group->add_item(p);   
  }

# define breakto(x) { goto x; }

  // Check for required attributes and fields.  Any attribute will suffice, whereas for fields,
  // all are required.
  template <typename H>
  bool check_filters(const AttrItems &attr_filter,const StrSet *field_filter,const H &h)
  {
    // Ignore anything that's already an expansion.
    if (h.has_expansion()) return false;

    // Then check attributes.  We look for any match.
    if (!attr_filter.empty()) {
      ForEach(attr_filter,i) {
        if (h.has_attr(*i)) {
          breakto(PASS1);
        }
      }
      return false;
    }
  PASS1:
    // Then look for fields.  We require all to be defined.
    if (field_filter && !field_filter->empty()) {
      ForEach((*field_filter),i) {
        if (!h.has_field(*i)) {
          return false;
        }
      }      
    }
    return true;
  }
 
  template <typename HC,typename S>
  void fill_with_names(const AttrItems &attr_filter,const StrSet *field_filter,const HC& hc, S& s,bool check_parent) 
  {
    ForEach(hc,i) {
      if ( check_filters(attr_filter,field_filter,**i) ) {
        s.insert((*i)->name());
      }
    }
  }

  template <typename S>
  void fill_with_names(const AttrItems &attr_filter,const StrSet *field_filter,const InstrHandlers& hc, S& s,bool check_parent) 
  {
    ForEach(hc,i) {
      if (!(*i)->instrs().empty()) {
        fill_with_names(attr_filter,field_filter,(*i)->instrs(),s,check_parent);
      }
      // Check this instruction if it is not a parent, or else if the
      // check-parent flag is set.
      if ((*i)->instrs().empty() || check_parent) {
        if (check_filters(attr_filter,field_filter,**i) ) {
          s.insert((*i)->name());
        }
      }
    }
  }
  
  template <typename HC>
  bool lookup_handlers(const HC& hc, const gc_string& s) 
  {
    ForEach(hc,i) {
      if ((*i)->name() == s) {
        return true;
      }
    }
    return false;
  }

  void GroupHandler::fill_group_with_all_items(Ptree *loc,const ArchHandler& ah, const Group &group,StrSet &result) 
  {
    if (group._type.empty()) {
      PError(loc,"Group '" << group._name << "' does not have type which is required for 'all items' group.");
      return;
    }
    bool cp = group.check_parent();
    if (group._type == "instr") { 
      fill_with_names(group._attr_filter,&group._field_filter,ah._instrs,result,cp);
    } else if (group._type == "reg") {
      fill_with_names(group._attr_filter,&group._field_filter,ah._regs,result,cp);
    } else if (group._type == "regfile") {
      fill_with_names(group._attr_filter,&group._field_filter,ah._regfiles,result,cp);
    } else if (group._type == "parm") {
      fill_with_names(group._attr_filter,0,ah._parms,result,cp);
    } else if (group._type == "instrfield") {
      fill_with_names(group._attr_filter,0,ah._instrfields,result,cp);
    } else if (group._type == "reloc") {
      fill_with_names(group._attr_filter,0,ah._relocs,result,cp);
    } else if (group._type == "resource") {
      fill_with_names(group._attr_filter,0,ah._extresources,result,cp);
    } else {
      PError(loc,"The type '" << group._type << "' is not supported for 'all items' group.");
    }
  }
  
  Group* GroupHandler::find_group(Ptree* loc,GroupHash &groups,Ptree *sym) {
    auto group = groups.find(sym->ToString())->second;
    if (groups.find(sym->ToString()) == groups.end()) {
      PError(loc,"Group '" << sym << "' has not yet been defined.");
    }
    return group;
  }

  void GroupHandler::resolve_items(StrSet *result)
  {
    resolve_group_items(result,_ah,*_group,_ah._core->_groups,0);
  }

  void GroupHandler::resolve_group_items(StrSet *result,const ArchHandler& ah,Group &group,GroupHash& groups, unsigned depth) 
  {
    if (!group._raw_items.empty() || !group._attr_filter.empty() || !group._field_filter.empty()) {
      if (!group._raw_items.empty()) {
        ForEach(group._raw_items,i) {
          resolve_group_operations(*i,group._items,ah,group,groups,depth);
        }
      } else if (!group._attr_filter.empty() || !group._field_filter.empty()) {
        fill_group_with_all_items(group.loc(),ah,group,group._items);
      }
      GroupHandler::check_group(group.loc(),ah,group);
      
      // Done with this stuff after processing.
      group._raw_items.clear();  
      group._attr_filter.clear();
      group._field_filter.clear();
    }

    if (result) {
      result->insert(group._items.begin(),group._items.end());
    }
  }

  // This processes _raw_items that are Ptree pointers and could
  // be referece to another group or set operations. 
  void GroupHandler::resolve_group_operations(Ptree* value,StrSet &result,const ArchHandler& ah,Group &group,GroupHash& groups, unsigned depth) 
  { 
    if (depth++ > 100) {
      PError(group.loc(),"Links are too deep.");
    }    

    enum OpType { None, Diff, Intersect, Union };
    OpType set_op = None;
    Ptree *l = 0, *r = 0;

    // Any unary operations are taken care of here and we return immediately.
    if (PtreeUtil::Match(value,"[ %? & %? ]",&l,&r)) {
      // Set intersection operation.
      set_op = Intersect;
    } else if (PtreeUtil::Match(value,"[ %? - %? ]",&l,&r)){
      // Set difference operation.
      set_op = Diff;
    } else if (PtreeUtil::Match(value,"[ %? | %? ]",&l,&r)){
      // Set union operation.
      set_op = Union;
    } else if (PtreeUtil::Match(value,"[ %? , %? ]",&l,&r)) {
      // Sequencing operation- implicit union.
      set_op = Union;
    } else if (PtreeUtil::Match(value,"[* %?]",&l)) {
      // Group reference operation.
      auto group1 = find_group(group.loc(),groups,l);
      resolve_group_items(&result,ah,*group1,groups,depth+1);
      return;
    } else if (PtreeUtil::Match(value,"[ ( %? ) ]",&l)) {
      // Sub-expression.
      resolve_group_operations(l,result,ah,group,groups,depth);
      return;
    } else {
      // A leaf-element- refers to a resource in the architecture.
      gc_string item = get_str(value,"group item");
      
      if (item == AllItems) {
        fill_group_with_all_items(value,ah,group,result);
      } else {
        result.insert(item);
      }
      return;
    }

    // If we get here, we should have a binary operation to do.  Otherwise, we
    // should have returned earlier.
    assert(set_op != None);

    StrSet items1, items2;
    insert_iterator<StrSet> r_iter(result,result.begin());

    // Figure out what the lhs and rhs are.
    resolve_group_operations(l,items1,ah,group,groups,depth+1);
    resolve_group_operations(r,items2,ah,group,groups,depth+1);
      
    // Do the operation.  Results are placed into result.
    switch (set_op) {
    case None:
      break;
    case Diff:
      set_difference  (items1.begin(),items1.end(),items2.begin(),items2.end(),r_iter);
      break;
    case Intersect:
      set_intersection(items1.begin(),items1.end(),items2.begin(),items2.end(),r_iter);
      break;
    case Union:
      set_union(items1.begin(),items1.end(),items2.begin(),items2.end(),r_iter);
      break;
    }    
  }


  void GroupHandler::check_group(Ptree* loc,const ArchHandler& ah, Group& group) {
    if (group._checked) {
      // We dont check twice because we resolve all groups every time a defmod is 
      // met and in case of remove we can think that group item is invalid.
      return;
    }
    group._checked = true;
    
    if (group._items.empty()) {
      return;
    }
    
    if (group._type.empty()) {
      group._type = get_type(loc,ah,*(group._items.begin()));
    } 
    
    gc_set<gc_string> handlers = GroupHandler::get_names(loc,ah,group._type);
 
 
     
    ForEach(group._items,i) {
      if (handlers.count(*i) == 0) {
        PError(loc,"Nonhomogeneous group '" << group._name << "': The type of group is '" << group._type << "' but '" << *i << "' of this type does not exist." );
      }
    }
  }
  
  
  gc_set<gc_string> make_name_set(const InstrHandlers& hc1, const SubInstrHandlers& hc2) {
    gc_set<gc_string> ret;
    ForEach(hc1,i) {
      ret.insert(((*i)->name()));
      ForEach((*i)->instrs(),ii) {
        ret.insert(((*ii)->name()));
      }
	
    }
    ForEach(hc2,i) {
      ret.insert(((*i)->name()));
    }
    return ret;
  }
  
  template <typename HC>
  gc_set<gc_string> make_name_set(const HC& hc) {
    gc_set<gc_string> ret;
    ForEach(hc,i) {
      ret.insert(((*i)->name()));
    }
    return ret;
  }

  gc_set<gc_string> GroupHandler::get_names(Ptree* loc,const ArchHandler& ah,const gc_string& str) {
    if (str == "instr") {
      return make_name_set(ah._instrs,ah._subinstrs);
    } else if (str =="reg") {
      return make_name_set(ah._regs);
    } else if (str =="regfile") {
      return make_name_set(ah._regfiles);
    } else if (str =="parm") {
      return make_name_set(ah._parms);
    } else if (str =="context") {
      return make_name_set(ah._contexts);
    } else if (str =="instrfield") {
      return make_name_set(ah._instrfields);
    } else if (str =="exception") {
      return make_name_set(ah._exceptions);
    } else if (str =="cache") {
      return make_name_set(ah._caches);
    } else if (str =="mem") {
      return make_name_set(ah._memories);
    } else if (str =="eventbus") {
      return make_name_set(ah._eventbuses);
    } else if (str =="reloc") {
      return make_name_set(ah._relocs);
    } else if (str =="resource") {
      return make_name_set(ah._extresources);
    } else {
      PError(loc,"Illegal item '" << str << "' in group.");
    }
  }

  
  gc_string GroupHandler::get_type(Ptree* loc,const ArchHandler& ah,const gc_string& str) {
    if (lookup_handlers(ah._instrs,str) || lookup_handlers(ah._subinstrs,str)) {
      return "instr";
    } else if (lookup_handlers(ah._regs,str)) {
      return "reg";
    } else if (lookup_handlers(ah._regfiles,str)) {
      return "regfile";
    } else if (lookup_handlers(ah._parms,str)) {
      return "parm";
    } else if (lookup_handlers(ah._contexts,str)) {
      return "context";
    } else if (lookup_handlers(ah._instrfields,str)) {
      return "instrfield";
    } else if (lookup_handlers(ah._exceptions,str)) {
      return "exception";
    } else if (lookup_handlers(ah._caches,str)) {
      return "cache";
    } else if (lookup_handlers(ah._memories,str)) {
      return "mem";
    } else if (lookup_handlers(ah._eventbuses,str)) {
      return "eventbus";
    } else if (lookup_handlers(ah._relocs,str)) {
      return "relocs";
    } else if (lookup_handlers(ah._extresources,str)) {
      return "resource";
    } else {
      PError(loc,"Illegal item '" << str << "' in group.");
    }  
  }


  //
  // Assembler config handler
  //


  Handler *AsmConfigHandler::add_define(MsgLog &ml,Environment *e,Ptree *p,const char *type,const char* name, bool modify) 
  {
    if (!strcmp(type,"parm")) {
      check_ident(name,p,"Parm");
      auto ph = new AsmParmHandler(e,p,name,modify);
      _parms.push_back(ph);
      return ph;
    } else if (!strcmp(type,"rule")) {
      check_ident(name,p,"Rule");
      auto rh = new AsmRuleHandler(e,p,name,modify);
      _rules.push_back(rh);
      return rh;
    } else {
      unknown_define(p,type,name);
    }
    abort();
  }

  void AsmConfigHandler::setup_parm_handler(AsmParmHandler &ph, AsmParm &ap) 
  {
    ap._doc += ph.doc();
    ph._asmparm = &ap;
  }

  void AsmConfigHandler::setup_rule_handler(AsmRuleHandler &rh, AsmRule &ar)
  {
    ar._doc += rh.doc();
    rh._asmrule = &ar;
  }

  void AsmConfigHandler::process_keys(MsgLog &log) 
  {
    Handler::process_keys(log);

    process_named_handler(log,_parms,_asmcfg->_parms,
                          *this,&AsmConfigHandler::setup_parm_handler);

    process_named_handler(log,_rules,_asmcfg->_rules,
                          *this,&AsmConfigHandler::setup_rule_handler);
  }


  void AsmConfigHandler::handle_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"comments")) {
      handle_list_or_item(value,bind(&AsmConfigHandler::record_comment_adapter,this,_1));
    } else if (!strcmp(key,"line_comments")) {
      handle_list_or_item(value,bind(&AsmConfigHandler::record_line_comment_adapter,this,_1));
    } else if (!strcmp(key,"line_separators")) {
      handle_list_or_item(value,bind(&AsmConfigHandler::record_line_separator_adapter,this,_1));
    } else if (!strcmp(key,"instr_separators")) {
      handle_list_or_item(value,bind(&AsmConfigHandler::record_instr_separator_adapter,this,_1));
    } else if (!strcmp(key,"packet_grouping")) {
      handle_list_or_item(value,bind(&AsmConfigHandler::record_group_separator_adapter,this,_1));
    } else if (!strcmp(key,"asm_instrtables")) {
      handle_list_or_item(value,bind(&AsmConfigHandler::record_asm_instrtables_adapter,this,_1));
    } else if (!strcmp(key,"disasm_instrtables")) {
      handle_list_or_item(value,bind(&AsmConfigHandler::record_disasm_instrtables_adapter,this,_1));      
    } else if (!strcmp(key,"explicit_regs")) {
      _asmcfg->_explicit_regs = get_bool(value,"explicit_regs.",env());
    } else if (!strcmp(key,"explicit_operand_regex_end")) {
      _asmcfg->_explicit_operand_regex_end = get_bool(value,"explicit_operand_regex_end.",env());      
    } else if (!strcmp(key,"symbol_chars")) {
      handle_list_or_item(value,bind(&AsmConfigHandler::record_symbol_chars_adapter,this,_1));
    } else if (!strcmp(key,"operand_symbols_excluded")) {
      handle_list_or_item(value,bind(&AsmConfigHandler::record_operand_symbols_excluded_adapter,this,_1));      
    } else if (!strcmp(key,"queue_size")) {
      _asmcfg->_queue_size = get_uint(value,"queue size.",env());
    } else if (!strcmp(key,"queue_offset")) {
      _asmcfg->_queue_offset = get_uint(value,"queue offset.",env());
    } else {
      unknown_key(value,key);
    }
  }

  bool valid_sep(const gc_string &str) 
  {
    switch (str.length()) {
    case 1: 
      return true;
    case 2: 
      if (str[0] == '\\') {
        return (str[1] == 't' || str[1] == 'n' || str[1] == '\\' || str[1] == '0');
      } 
      return false;
    default:
      return false;     
    }     
  }
 
  void AsmConfigHandler::record_comment_adapter(Ptree *p)
  {     
    _asmcfg->_comments.push_back(get_str(p,"comment character"));   
  }
 
  void AsmConfigHandler::record_line_comment_adapter(Ptree *p)
  {     
    _asmcfg->_line_comments.push_back(get_str(p,"line comment character"));   
  }
  
  void AsmConfigHandler::record_line_separator_adapter(Ptree *p)
  {     
    const char *sep = get_str(p,"line comment character");
    if (valid_sep(sep)) {
      _asmcfg->_line_seps.push_back(sep); 
    } else {
      PError(p, "Invalid line separator '" << sep << "'.");
    }
  }

  void AsmConfigHandler::record_instr_separator_adapter(Ptree *p)
  {     
    const char *sep = get_str(p,"instruction comment character");

    if (valid_sep(sep)) {
      _asmcfg->_instr_seps.push_back(sep); 
    } else {
      PError(p, "Invalid instruction separator '" << sep << "'.");
    }
  }
  
  void AsmConfigHandler::record_symbol_chars_adapter(Ptree *p) {
    const char *sym = get_str(p,"symbol character");
    if (valid_sep(sym)) {
      _asmcfg->_symbol_chars.push_back(sym);
    } else {
      PError(p, "Invalid symbol character '" << sym << "'.");
    }
  }

  void AsmConfigHandler::record_operand_symbols_excluded_adapter(Ptree *p) {
    const char *sym = get_str(p,"operand symbol excluded");
    if (valid_sep(sym)) {
      _asmcfg->_operand_symbols_excluded.push_back(sym);
    } else {
      PError(p, "Invalid operand symbols excluded '" << sym << "'.");
    }
  }  
  
  void AsmConfigHandler::record_group_separator_adapter(Ptree *p)
  {     
    Ptree *l,*r;
    if (Match(p,"[ ( [ %? , %? ] ) ]",&l,&r)) {
      if (!get_str(l,0) || !get_str(r,0)) {
        PError(p, "Parsing error in group separator characters in the assembler configuration.");
      } else {
        const char *ll = get_str(l,"left packet group separator.");
        if (!valid_sep(ll)) {
          PError(p, "Invalid left packet group separator '" << ll << "'.");
        }
        const char *rr = get_str(r,"right packet group separator.");
        if (!valid_sep(rr)) {
          PError(p, "Invalid right packet group separator '" << rr << "'.");
        }
        _asmcfg->_group_seps.push_back(make_pair(ll,rr));
      }
    } else if (get_str(p,0)) {
      const char *sep = get_str(p,"packet group separator.");
      if (!valid_sep(sep)) {
        PError(p, "Invalid packet group separator '" << sep << "'.");
      } else {
        _asmcfg->_group_seps.push_back(make_pair(sep,sep));
      }
    } else {
      PError(p, "Parsing error in group separator characters in the assembler configuration.");
    }  
  } 

  void AsmConfigHandler::record_asm_instrtables_adapter(Ptree *p)
  {     
    _asmcfg->_asm_instrtables.push_back(get_str(p,"Assembler instruction table."));   
  }

  void AsmConfigHandler::record_disasm_instrtables_adapter(Ptree *p)
  {     
    _asmcfg->_disasm_instrtables.push_back(get_str(p,"Disassembler instruction table."));   
  }  


  void AsmParmHandler::handle_key(const char* key,Opencxx::Ptree *value) 
  {
    if (!strcmp(key,"constant")) {
      _asmparm->_constant = get_bool(value,"parameter constant flag.",env());
    } else if (!strcmp(key,"value")) {
      _asmparm->_value = get_uint(value,"parameter default value.",env());
    } else if (!strcmp(key,"type")) {
      const char *t = get_ident(value,"parm type.");
      if (!strcmp(t,"integer")) {
        _asmparm->_type = AsmParm::Integer;
      } else if (!strcmp(t,"label")) {
        _asmparm->_type = AsmParm::Label;
      } else {
        PError(value,"Unknown parm type:  " << value);
      }
    }
    else {
      unknown_key(value,key);
    }
  }

  void AsmRuleHandler::handle_key(const char* key,Opencxx::Ptree *value) 
  {
    if (!strcmp(key,"action")) {
      _asmrule->_action = get_func(value,1,"assembler rule.");
    } else {
      unknown_key(value,key);
    }
  }

}

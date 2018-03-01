//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Various Handler class implementations.
//

#ifndef _HANDLERIMPL_H_
#define _HANDLERIMPL_H_

#include "helpers/BasicTypes.h"
#include "Handler.h"
#include "Types.h"

namespace adl {

  
  class Data;
  class Core;
  class RegBase;
  class Reg;
  class RegFile;
  class RegEntry;
  class RegField;
  class InstrBase;
  class SubInstr;
  class SubInstrGrp;
  class Instr;
  class StrHash;
  class PrefixCounter;
  class InstrField;
  class Relocation;
  class System;
  class Parm;
  class AddrMask;
  class Aspect;
  class Exception;
  class ExceptionField;
  class RegHookBase;
  class RegFileHook;
  class RegEntryHook;
  class RegHook;
  class MMU;
  class MmuTestSet;
  class MmuLookup;
  class GenericField;
  class MmuField;
  class CacheField;
  class Cache;
  class Memory;
  class EventBus;
  class EventBusField;
  class Context;
  class Group;
  class GroupHash;
  class AsmConfig;
  class AsmParm;
  class AsmRule;
  class SysHandler;
  class CoreHandler;
  class ShareHandler;
  class RegHookHandler;
  class RegFileHookHandler;
  class RegEntryHookHandler;
  class SysHandlerHash;
  class CoreHandlerHash;
  class MmuArrayHandler;
  class AddrMaskHandler;
  class GroupHandler;
  class AsmConfigHandler;
  class AsmParmHandler;
  class AsmRuleHandler;
  class AttrItem;
  class AttrItems;
  class ExtResource;
  
  HandlerList(ArchHandler);
  HandlerList(ClassHandler);
  HandlerList(ParmHandler);
  HandlerList(RegFileHandler);
  HandlerList(RegHandler);
  HandlerList(ShareHandler);
  HandlerList(RegFieldHandler);
  HandlerList(RegEntryHandler);
  HandlerList(PrefixCounterHandler);
  HandlerList(InstrFieldHandler);
  HandlerList(SubInstrHandler);
  HandlerList(SubInstrGrpHandler);
  HandlerList(InstrHandler);
  HandlerList(RelocationHandler);
  HandlerList(ExceptionHandler);
  HandlerList(ExceptionFieldHandler);
  HandlerList(AspectHandler);
  HandlerList(MmuHandler);
  HandlerList(MmuLookupHandler);
  HandlerList(MmuTestSetHandler);
  HandlerList(MmuArrayHandler);
  HandlerList(MmuFieldHandler);
  HandlerList(CacheFieldHandler);
  HandlerList(CacheHandler);
  HandlerList(MemoryHandler);
  HandlerList(EventBusHandler);
  HandlerList(EventBusFieldHandler);
  HandlerList(ContextHandler);
  HandlerList(GroupHandler);
  HandlerList(AsmParmHandler);
  HandlerList(AsmRuleHandler);
  HandlerList(ExtResourceHandler);

  typedef gc_list<Opencxx::Environment *> EnvList;

  struct ArchHandlerHash : public gc_hash_map<gc_string,ArchHandler *> {};

  // Outer-most handler class.
  class OuterHandler : public Handler {
  public:
    OuterHandler(Opencxx::Environment *e,Opencxx::Ptree *p);

    virtual Handler *add_define(MsgLog &ml,Opencxx::Environment *e,Opencxx::Ptree *p,const char *type,const char *name,bool modify);

    virtual void process_keys(MsgLog &);

    Data        *_data;
  protected:
    SysHandlerHash   *_systems;
    CoreHandlerHash  *_cores;
    ArchHandlerHash  *_archs;
  };

  // This is simply a hash with a parent pointer- lookups first examine the
  // current hash, then climb up the parents, doing searches.  It's useful for
  // when nested, local instances are permitted.
  template <class DH>
  struct NestedHandlers : gc_hash_map<gc_string,DH *> {
    typedef gc_hash_map<gc_string,DH *> Base;

    typedef typename Base::key_type key_type;
    typedef typename Base::iterator iterator;
    typedef typename Base::const_iterator const_iterator;

    NestedHandlers() : _parent(0) {};
    NestedHandlers(NestedHandlers *p) : _parent(p) {};

    using Base::insert;
    using Base::erase;
    using Base::clear;

    // Returns 1 if found; if not found in this map, will return the
    // parent's value.
    int count(const key_type &key) const
    {
      if (!Base::count(key)) {
        if (_parent) {
          return _parent->count(key);
        } else {
          return 0;
        }
      } else {
        return 1;
      }
    }

    // Only searches the current scope.
    int count_local(const key_type &key) const
    {
      return Base::count(key);
    }

    // Searchs its map, then return's parent value if not found.
    const_iterator find(const key_type &key) const { return const_cast<NestedHandlers&>(*this).find(key); };
    iterator find(const key_type &key)
    {
      iterator i = Base::find(key);
      if (i != Base::end()) {
        return i;
      } else {
        if (_parent) {
          return _parent->find(key);
        } else {
          return Base::end();
        }
      }
    }

    // Does only a find on this hash's contents- does not search parents.
    const_iterator find_local(const key_type &key) const { return const_cast<NestedHandlers&>(*this).find_local(key); };
    iterator find_local(const key_type &key)
    {
      return Base::find(key);
    }

  private:
    NestedHandlers *_parent;
  };

  struct CoreHandlerHash : public NestedHandlers<CoreHandler> {
    CoreHandlerHash() {};
    CoreHandlerHash(CoreHandlerHash *c) : NestedHandlers<CoreHandler>(c) {};
  };
  struct SysHandlerHash : public NestedHandlers<SysHandler> {
    SysHandlerHash() {};
    SysHandlerHash(SysHandlerHash *s) : NestedHandlers<SysHandler>(s) {};
  };

  // Stores information about an aspect.
  class AspectHandler : public RemoveHandler {
  public:
    AspectHandler(Opencxx::Environment *e,Opencxx::Ptree *p,const char *name,bool modify) :
      RemoveHandler(e,p,name,modify), _aspect(0) {};

    virtual void handle_key(const char *key,Opencxx::Ptree *value);

    Aspect *_aspect;
  private:
    friend class ArchHandler;

    void add_label(Opencxx::Ptree *p);
    void add_if_def(Opencxx::Ptree *p);
    void add_attr(Opencxx::Ptree *p);
  };

  // Stores a set of aspects.  This is to be used with multiple inheritence- you must
  // also inherit from an object which derives from Handler, e.g. RemoveHandler.
  class AspectGrpHandler {
  public:
    AspectGrpHandler() : _a(0) {};

    Handler *add_define(MsgLog &ml,Opencxx::Environment *e,Opencxx::Ptree *p,
                        const char *type,const char *name,bool modify);

    void process_keys(MsgLog &);

    void set_aspects(Aspects *a) { _a = a; };
    Aspects *_a;
  protected:
    void setup_aspect_handler(AspectHandler &ah,Aspect &a);

    AspectHandlers _aspects;
  };

  // A remove handler which can handle aspects.  Use with named objects.  Also
  // can have a group, though not all items which use this class support groups.
  class AspectRemoveHandler : public RemoveHandler, public AspectGrpHandler {
  public:
    AspectRemoveHandler(Opencxx::Environment *e,Opencxx::Ptree *p,const gc_string &n,bool modify) :
      RemoveHandler(e,p,n,modify),
      _gh(0)
    {};

    Handler *add_define(MsgLog &ml,Opencxx::Environment *e,Opencxx::Ptree *p,
                        const char *type,const char *name,bool modify);

    void process_keys(MsgLog &);

    void set_name(const gc_string &n) { _name = n; _gh = 0; };
    void set_group(GroupHandler *gh) { _gh = gh; };
    bool has_expansion() const { return _gh; };
    void get_expansion(StrSet &);

  protected:
    GroupHandler *_gh;
  };

  // A remove handler which can handle aspects.  Use with named objects.
  class AspectIntRemoveHandler : public IntRemoveHandler, public AspectGrpHandler {
  public:
    AspectIntRemoveHandler(Opencxx::Environment *e,Opencxx::Ptree *p,unsigned id,bool modify) :
      IntRemoveHandler(e,p,id,modify)
    {};

    Handler *add_define(MsgLog &ml,Opencxx::Environment *e,Opencxx::Ptree *p,
                        const char *type,const char *name,bool modify);

    void process_keys(MsgLog &);
  };

  // A handler that can be used for "field" type objects.
  class GenericFieldHandler : public RemoveHandler {
  public:
    GenericFieldHandler(Opencxx::Environment *e,Opencxx::Ptree *p,
                        const gc_string &name,bool m) :
      RemoveHandler(e,p,name,m), _field(0) {};

    virtual void handle_key(const char *key,Opencxx::Ptree *value);

    GenericField *_field;

  };

  // System handler class.
  class SysHandler : public AspectRemoveHandler {
  public:
    SysHandler(Opencxx::Environment *e,Opencxx::Ptree *p,
               SysHandlerHash *s,CoreHandlerHash *c,const gc_string &n,bool m) : 
      AspectRemoveHandler(e,p,n,m), _archs(0), _sys(0), _systems(new SysHandlerHash(s)), 
      _cores(new CoreHandlerHash(c)) {};

    virtual Handler *add_define(MsgLog &ml,Opencxx::Environment *e,Opencxx::Ptree *p,const char *type,const char *name,bool modify);
    virtual void handle_key(const char *key,Opencxx::Ptree *value);
    virtual void process_keys(MsgLog &log);
    virtual void handle_decl(const char *type,Opencxx::Ptree *name);

    ArchHandlerHash *_archs;
    System          *_sys;
  protected:
    friend class OuterHandler;

    void record_shared(Opencxx::Ptree *p);
    void setup_share_handler(ShareHandler &sh);

    SysHandlerHash  *_systems;
    CoreHandlerHash *_cores;

    ShareHandlers    _shares;
  };

  class ShareHandler : public Handler {
  public:
    ShareHandler(Opencxx::Environment *e,Opencxx::Ptree *p) :
      Handler(e,p), _sys(0)
    {};

    virtual void handle_key(const char *key,Opencxx::Ptree *value);

    System          *_sys;

  protected:
    void record_shared_reg(Opencxx::Ptree *p);
    void record_shared_regfile(Opencxx::Ptree *p);
    void record_shared_mmu(Opencxx::Ptree *p);
    void record_shared_parm(Opencxx::Ptree *p);
    void record_shared_cache(Opencxx::Ptree *p);
    void record_shared_memory(Opencxx::Ptree *p);
    void record_shared_eventbus(Opencxx::Ptree *p);
    void record_shared_extresource(Opencxx::Ptree *p);
  };

  // Architecture handler class.
  class ArchHandler : public RemoveHandler, public AspectGrpHandler {
  public:
    ArchHandler(Opencxx::Environment *e,Opencxx::Ptree *p,const gc_string &name,bool modify);

    virtual Handler *add_define(MsgLog &ml,Opencxx::Environment *e,Opencxx::Ptree *p,
                                const char *type,const char *name,bool modify);

    virtual Handler *add_defines(MsgLog &ml,Opencxx::Environment *e,Opencxx::Ptree *p,
                                 const char *type,const char *name);

    virtual void add_func(const char *name,Opencxx::Ptree *p);

    virtual void add_type_decl(Opencxx::Ptree *p);

    virtual void process_keys(MsgLog &);

    virtual void handle_key(const char *key,Opencxx::Ptree *value);

    void setCore(Core *c) { _core = c; };
    
    EnvList          _dm_envs;  // List of environments from defmods of this architecture.
  protected:
    friend class CoreHandler;
    friend class GroupHandler;

    void add_attr(Opencxx::Ptree *p);
    void add_blk(Opencxx::Ptree *p);

    void setup_parm_handler(ParmHandler &ph,Parm &p);
    void setup_context_handler(ContextHandler &ch,Context &c);
    void setup_regfile_handler(RegFileHandler &rfh,RegFile &rf);
    void setup_reg_handler(RegHandler &rh,Reg &reg);
    void setup_relocation_handler(RelocationHandler &rh, Relocation &reloc);
    void setup_pfxctr_handler(PrefixCounterHandler &pch,PrefixCounter &pc);
    void setup_instrfield_handler(InstrFieldHandler &ifh,InstrField &instrfield);
    void setup_subinstr_handler(SubInstrHandler &sih,SubInstr &subinstr);
    void setup_instr_handler(InstrHandler &ih,Instr &instr);
    void setup_exception_handler(ExceptionHandler &eh,Exception &ex);
    void setup_cache_handler(CacheHandler &ch,Cache &c);
    void setup_eventbus_handler(EventBusHandler &ebh,EventBus &eb);
    void setup_extresource_handler(ExtResourceHandler &ebh,ExtResource &er);
    void setup_memory_handler(MemoryHandler &mh, Memory &mem);
    void setup_group_handler(GroupHandler &gh,Group &group);
    void process_mmu_handler(MsgLog &log);
    void preprocess_group_handlers(MsgLog &log);
    void process_group_handlers(MsgLog &log);
    void process_addr_mask(AddrMaskHandler &amh,AddrMask &am,MsgLog &log);
    void process_asm_config(AsmConfigHandler &asmh, AsmConfig &asmcfg,MsgLog &log);
    
    
    Core                 *_core;

    HelperMap             _helpers;

    PList                 _type_decls;

    ContextHandlers       _contexts;
    ParmHandlers          _parms;
    RegFileHandlers       _regfiles;
    RegHandlers           _regs;
    PrefixCounterHandlers _prefix_counters;
    InstrFieldHandlers    _instrfields;
    SubInstrHandlers      _subinstrs;
    RelocationHandlers    _relocs;
    InstrHandlers         _instrs;
    ExceptionHandlers     _exceptions;
    MmuHandlers           _mmus;
    CacheHandlers         _caches;
    MemoryHandlers        _memories;
    EventBusHandlers      _eventbuses;
    ExtResourceHandlers   _extresources;
    GroupHandlers         _groups;
    GroupHash            *_tmp_groups;

    AddrMaskHandler      *_ra_mask;
    AddrMaskHandler      *_ea_mask;
    AsmConfigHandler     *_asmcfg;
    
  };

  // Core handler class.
  class CoreHandler : public AspectRemoveHandler {
  public:
    CoreHandler(Opencxx::Environment *e,Opencxx::Ptree *p,const gc_string &n,bool m) : 
      AspectRemoveHandler(e,p,n,m), _archs(0), _core(0), _corearch(e,p,n,m) {};

    virtual void process_keys(MsgLog &);

    virtual Handler *add_define(MsgLog &ml,Opencxx::Environment *e,Opencxx::Ptree *p,
                                const char *type,const char *name,bool modify);

    virtual void add_func(const char *name,Opencxx::Ptree *p);

    virtual void add_type_decl(Opencxx::Ptree *p);

    virtual void add_key(const char *key,Opencxx::Ptree *value);
    virtual void handle_key(const char *key,Opencxx::Ptree *value);

    void setCore(Core *c) { _core = c; _corearch.setCore(c); };
    Core *core() const { return _core; };

    ArchHandlerHash *_archs;

    EnvList          _dm_envs;  // List of environments from defmods of this core.
  protected:
    friend class OuterHandler;

    void record_arch(Opencxx::Ptree *p);
    void record_itable(Opencxx::Ptree *p);
    void record_attr(Opencxx::Ptree *p);

    Core                          *_core;

    PList                          _archlist;
    ArchHandler                    _corearch;

    StrList                        _itablenames;

  };

  // Context handler class.
  class ContextHandler : public AspectRemoveHandler {
  public:
    ContextHandler(Opencxx::Environment *e,Opencxx::Ptree *p,const gc_string &name,bool m) :
      AspectRemoveHandler(e,p,name,m) {};

    virtual void handle_key(const char *key,Opencxx::Ptree *value);

    Core    *_core;
    Context *_context;

    // Used for group selection.
    bool has_attr(const AttrItem &ai) const;

  private:
    void record_reg(Opencxx::Ptree *p);
    void record_regfile(Opencxx::Ptree *p);
    void record_mmulookup(Opencxx::Ptree *p);
    void record_mem_layout(Opencxx::Ptree *p);
    void record_mem_omit(Opencxx::Ptree *p);
  };

  // Address mask handler class.
  class AddrMaskHandler : public AspectRemoveHandler {
  public:
    AddrMaskHandler(Opencxx::Environment *e,Opencxx::Ptree *p) :
      AspectRemoveHandler(e,p,"",false), _mask(0) {};

    virtual void handle_key(const char *key,Opencxx::Ptree *value);

    AddrMask   *_mask;
  };

  // Parameter handler class.
  class ParmHandler : public AspectRemoveHandler {
  public:
    ParmHandler(Opencxx::Environment *e,Opencxx::Ptree *p,const gc_string &name,bool m) :
      AspectRemoveHandler(e,p,name,m), _parm(0) {};

    virtual void handle_key(const char *key,Opencxx::Ptree *value);

    Parm *_parm;

    // Used for group selection.
    bool has_attr(const AttrItem &ai) const;
    bool has_field(const gc_string &field) const { return false; };

  private:
    void add_option(Opencxx::Ptree *value);
  };

  // Handler for information common between registers and register files.
  class RegBaseHandler : public AspectRemoveHandler {
  public:
    RegBaseHandler(Opencxx::Environment *e,Opencxx::Ptree *p,const gc_string &name,bool m) : 
      AspectRemoveHandler(e,p,name,m), _regbase(0) {};

    virtual Handler *add_define(MsgLog &ml,Opencxx::Environment *e,Opencxx::Ptree *p,
                                const char *type,const char *name,bool modify);
    virtual void handle_key(const char *key,Opencxx::Ptree *value);

    virtual void process_keys(MsgLog &);

    // Used for group selection.
    bool has_attr(const AttrItem &ai) const;
    bool has_field(const gc_string &field) const;

    Core *_core;
  protected:
    void setup_regfield_handler(RegFieldHandler &rfh,RegField &rf);

    template<class Base,class Hook> void process_hook(MsgLog &,Base *base,Hook *hook,bool read);
    void record_enums_adapter(Opencxx::Ptree *p);
    void record_enums(Opencxx::Ptree *p, int &value);



    // Stores all register fields.
    RegFieldHandlers _regfields;

    RegBase     *_regbase;
  };

  // Register file handler class.
  class RegFileHandler : public RegBaseHandler {
  public:
    RegFileHandler(Opencxx::Environment *e,Opencxx::Ptree *p,const gc_string &name, bool m) : 
      RegBaseHandler(e,p,name,m), _regfile(0), _read(0), _write(0)  {};

    virtual Handler *add_define(MsgLog &ml,Opencxx::Environment *e,Opencxx::Ptree *p,
                                const char *type,const char *name,bool modify);
    virtual Handler *add_define(MsgLog &ml,Opencxx::Environment *e,Opencxx::Ptree *p,
                                const char *type,unsigned name,bool modify);
    virtual void handle_key(const char *key,Opencxx::Ptree *value);
    virtual void process_keys(MsgLog &);

    void set_regbase(RegFile *rf);

  protected:
    friend class RegBaseHandler;

    void setup_regentry_handler(RegEntryHandler &rfeh,RegEntry &rfe);
    void optional_new_hook(Opencxx::Ptree *loc);

    RegFile    *_regfile;

    RegEntryHandlers _regentries;

    // Store read and write alias information.
    RegFileHookHandler *_read;
    RegFileHookHandler *_write;

  };

  // Register-file entry elements.
  class RegEntryHandler : public AspectIntRemoveHandler {
  public:
    RegEntryHandler(Opencxx::Environment *e,Opencxx::Ptree *p,unsigned id,bool m) :
      AspectIntRemoveHandler(e,p,id,m) {};

    virtual void handle_key(const char *key,Opencxx::Ptree *value);

    Handler *add_define(MsgLog &ml,Opencxx::Environment *e,Opencxx::Ptree *p,const char *type,const char *name,bool modify);

    void process_keys(MsgLog &log);
  protected:
    friend class RegFileHandler;

    void process_hook(MsgLog &log,RegEntryHookHandler *hook,bool read);

    RegEntry *_regentry;
    RegFile  *_regfile;

    RegEntryHookHandler *_read;
    RegEntryHookHandler *_write;
  };

  class RegEntryHookHandler : public AspectRemoveHandler {
  public:
    RegEntryHookHandler(Opencxx::Environment *e,Opencxx::Ptree *p,bool read,bool m) : 
      AspectRemoveHandler(e,p,"",m), _hook(0), _read(read) {};

    virtual void handle_key(const char *key,Opencxx::Ptree *value);

    RegEntryHook *create_data_object();
  protected:

    RegEntryHook *_hook;
    bool          _read;
  };


  // Register handler class.
  class RegHandler : public RegBaseHandler {
  public:
    RegHandler(Opencxx::Environment *e,Opencxx::Ptree *p,const gc_string &name,bool m) : 
      RegBaseHandler(e,p,name,m), _reg(0), _read(0), _write(0) {};

    virtual Handler *add_define(MsgLog &ml,Opencxx::Environment *e,Opencxx::Ptree *p,
                                const char *type,const char *name,bool modify);
    void handle_key(const char *key,Opencxx::Ptree *value);
    virtual void process_keys(MsgLog &);

    void set_regbase(Reg *r);

  protected:
    friend class RegBaseHandler;

    void optional_new_hook(Opencxx::Ptree *);

    Reg       *_reg;

    // Store read and write alias information.
    RegHookHandler *_read;
    RegHookHandler *_write;

  };

  // Stores information about a register field.
  class RegFieldHandler : public AspectRemoveHandler {
  public:
    RegFieldHandler(Opencxx::Environment *e,Opencxx::Ptree *p,const gc_string &name,bool m) : 
      AspectRemoveHandler(e,p,name,m), _regbase(0), _field(0) {};

    virtual void handle_key(const char *key,Opencxx::Ptree *value);

    RegBase    *_regbase;
    RegField   *_field;

  };

  // Stores information about a read or write alias for a register
  // or register file.
  class HookHandler : public AspectRemoveHandler {
  public:
    HookHandler(Opencxx::Environment *e,Opencxx::Ptree *p,bool read,bool m) : 
      AspectRemoveHandler(e,p,"",m), _core(0), _hookbase(0), _read(read) {};

    virtual void handle_key(const char *key,Opencxx::Ptree *value);

    Core *_core;
  protected:

    RegHookBase *_hookbase;
    bool         _read;
  };


  // Hook handler for register files.
  class RegFileHookHandler : public HookHandler {
  public:
    RegFileHookHandler(Opencxx::Environment *e,Opencxx::Ptree *p,bool read,bool m) :
      HookHandler(e,p,read,m), _hook(0) {};

    virtual void handle_key(const char *key,Opencxx::Ptree *value);

    RegFileHook *create_data_object();

  protected:  
    void record_reg_alias(Opencxx::Ptree *p);

    RegFileHook *_hook;
  };

  // Hook handler for registers.
  class RegHookHandler : public HookHandler {
  public:
    RegHookHandler(Opencxx::Environment *e,Opencxx::Ptree *p,bool read,bool m) :
      HookHandler(e,p,read,m), _hook(0) {};

    virtual void handle_key(const char *key,Opencxx::Ptree *value);

    RegHook *create_data_object();

  protected:
    RegHook *_hook;
  };

  // Stores information about a prefix counter.
  class PrefixCounterHandler : public AspectRemoveHandler {
  public:
    PrefixCounterHandler(Opencxx::Environment *e,Opencxx::Ptree *p,const gc_string &name,bool m) : 
      AspectRemoveHandler(e,p,name,m), _pc(0) {};

    virtual void handle_key(const char *key,Opencxx::Ptree *value);

    PrefixCounter *_pc;
  };

  // Handler for instruction fields.
  class InstrFieldHandler : public AspectRemoveHandler {
  public:
    InstrFieldHandler(Opencxx::Environment *e,Opencxx::Ptree *p,const gc_string &name,bool m) :
      AspectRemoveHandler(e,p,name,m), _core(0), _instrfield(0) {};

    Handler *add_define(MsgLog &ml,Opencxx::Environment *e,Opencxx::Ptree *p,const char *type,const char *name,bool modify);
    virtual void process_keys(MsgLog &log);
    virtual void handle_key(const char *key,Opencxx::Ptree *value);

    // Used for group selection.
    bool has_attr(const AttrItem &ai) const;
    bool has_field(const gc_string &field) const { return false; };

    const Core *_core;
    InstrField *_instrfield;
    InstrFieldHandlers _instrfields;
  protected:
    void record_range_adapter(Opencxx::Ptree *value);
    void record_masks_adapter(Opencxx::Ptree *value);
    void record_fields_adapter(Opencxx::Ptree *value);
    void record_enums_adapter(Opencxx::Ptree *value);
    void record_relocs_adapatr(Opencxx::Ptree *value);
    void record_enums(Opencxx::Ptree *p, int &value);
    bool record_bits(Opencxx::Ptree *p, bool error);
    void record_bits_adapter(Opencxx::Ptree *p);
    void add_bits(unsigned l,unsigned h);
    void record_table_adapter(Opencxx::Ptree *p);
    bool record_table(Opencxx::Ptree *p,bool error);
    bool record_fields(Opencxx::Ptree *p,bool error);
    void add_table_entry(Opencxx::Ptree *e);
    void add_blk(Opencxx::Ptree *p);
    void handle_syntax(Opencxx::Ptree *p);
    void handle_alias(Opencxx::Ptree *p);
    void setup_instrfield_handler(InstrFieldHandler &ifh, InstrField &ifield) ;
  };

  // Handler for relocations - no keys/define allowed
  class RelocationHandler : public AspectRemoveHandler {
  public:
    RelocationHandler(Opencxx::Environment *e,Opencxx::Ptree *p,const gc_string &name,bool m) :
      AspectRemoveHandler(e,p,name,m), _reloc(0) {};

    virtual void handle_key(const char* key,Opencxx::Ptree *value);

    bool has_attr(const AttrItem &ai) const;
    bool has_field(const gc_string &field) const { return false; };

  protected:
    friend class ArchHandler;

    Relocation *_reloc;
  };

  // Handler for the information common between instrction and subinstruction
  class InstrBaseHandler : public AspectRemoveHandler {
  public:
    InstrBaseHandler(Opencxx::Environment *e,Opencxx::Ptree *p,const gc_string &name,bool m) :
      AspectRemoveHandler(e,p,name,m), _core(0), _instrbase(0) {} ;

    void handle_key(const char *key,Opencxx::Ptree *value, bool instr);  
 
    // Used for group selection.
    bool has_attr(const AttrItem &ai) const;
    bool has_field(const gc_string &field) const;

    const Core *_core;
    InstrBase  *_instrbase;
  protected:
    friend class ArchHandler;

    virtual void add_blk(Opencxx::Ptree *p);
    virtual void add_field(Opencxx::Ptree *p);
    virtual void add_pfx_ctr(Opencxx::Ptree *p);
    virtual void add_pfx_ctr_incr(Opencxx::Ptree *p);
    virtual void add_pfx_ctr_decr(Opencxx::Ptree *p);
    virtual void handle_syntax(Opencxx::Ptree *value,bool instr,bool handle_as_dsyntax=false);
  };


  // Handler for SubInstructions
  class SubInstrHandler : public InstrBaseHandler {
  public:
    SubInstrHandler(Opencxx::Environment *e,Opencxx::Ptree *p,const gc_string &name,bool m) :
      InstrBaseHandler(e,p,name,m), _subinstr(0) {};
    
    virtual void handle_key(const char *key,Opencxx::Ptree *value);
    void set_instrbase(SubInstr *subinstr);
  
  protected:
    void setup_subinstrgrp_handler(SubInstrGrpHandler& sigh, SubInstrGrp &sig);

    friend class ArchHandler;
    SubInstr *_subinstr;
  };

  // Handler of SubInstructions group
  class SubInstrGrpHandler : public RemoveHandler {
  public:
    SubInstrGrpHandler(Opencxx::Environment *e,Opencxx::Ptree *p,const gc_string &name,bool m) :
      RemoveHandler(e,p,name,m) {} ;  
    
    void handle_key(const char *key,Opencxx::Ptree *value) ;
  protected:
    void record_subinstr_names(Opencxx::Ptree *p);

    friend class InstrHandler;
    SubInstrGrp  *_subinstrgrp;
  };




  // Handler for instructions.
  class InstrHandler : public InstrBaseHandler {
  public:
    InstrHandler(Opencxx::Environment *e,Opencxx::Ptree *p,const gc_string &name,bool m) :
      InstrBaseHandler(e,p,name,m), _instr(0){};

    Handler *add_define(MsgLog &ml,Opencxx::Environment *e,Opencxx::Ptree *p,const char *type,const char *name,bool modify);
    virtual void process_keys(MsgLog &log);
    virtual void handle_key(const char *key,Opencxx::Ptree *value);
    void  set_instrbase(Instr* instr);
    const InstrHandlers& instrs() const { return _instrs;};

  protected:
    void handle_alias(Opencxx::Ptree *p);
    void record_instr_names(Opencxx::Ptree *p);
    void record_asm_rules(Opencxx::Ptree *p);
    void record_res_used(Opencxx::Ptree *p,PList &excludes);
    void setup_subinstrgrp_handler(SubInstrGrpHandler &sigh, SubInstrGrp &sig);
    void setup_instr_handler(InstrHandler &ih, Instr &instr);

    friend class ArchHandler;
  
    SubInstrGrpHandlers  _subinstrgrps;
    InstrHandlers	 _instrs;
    Instr        	*_instr;
  };

  // Handler for exceptions.
  class ExceptionHandler : public AspectRemoveHandler {
  public:
    ExceptionHandler(Opencxx::Environment *e,Opencxx::Ptree *p,const gc_string &name,bool m) :
      AspectRemoveHandler(e,p,name,m), _exception(0) {};

    Handler *add_define(MsgLog &ml,Opencxx::Environment *e,Opencxx::Ptree *p,const char *type,const char *name,bool modify);
    virtual void handle_key(const char *key,Opencxx::Ptree *value);
    void process_keys(MsgLog &log);

    Exception *_exception;

    // Used for group selection.
    bool has_attr(const AttrItem &ai) const;

  protected:
    void setup_field_handler(ExceptionFieldHandler &,ExceptionField &);

    ExceptionFieldHandlers _fields;

    friend class ArchHandler;

  };

  // Handler for exception fields.
  class ExceptionFieldHandler : public GenericFieldHandler {
  public:
    ExceptionFieldHandler(Opencxx::Environment *e,Opencxx::Ptree *p,
                          const gc_string &name,bool m) :
      GenericFieldHandler(e,p,name,m) {}

  protected:
    friend class ExceptionHandler;
  };


  // Handler for the MMU.
  class MmuHandler : public RemoveHandler, public AspectGrpHandler {
  public:
    MmuHandler(Opencxx::Environment *e,Opencxx::Ptree *p,const gc_string &name,bool m) :
      RemoveHandler(e,p,name,m), 
      _mmu(0), _core(0)
    {
      _modify = m;
    };

    virtual Handler *add_define(MsgLog &ml,Opencxx::Environment *e,Opencxx::Ptree *p,
                                const char *type,const char *name,bool modify);
    virtual void handle_key(const char *key,Opencxx::Ptree *value);
    virtual void process_keys(MsgLog &);

    MMU  *_mmu;
    Core *_core;
  protected:
    friend class ArchHandler;

    void setup_lookup_handler(MmuLookupHandler &mh,MmuLookup &ml);

    MmuLookupHandlers _lookups;

  };

  // Handler for sets of tests within an MMU lookup.
  class MmuTestSetHandler : public RemoveHandler {
  public:
    MmuTestSetHandler(Opencxx::Environment *e,Opencxx::Ptree *p,const gc_string &name,bool m) :
      RemoveHandler(e,p,name,m), _test_set(0) {};

    virtual void handle_key(const char *key,Opencxx::Ptree *value);

    MmuTestSet *_test_set;

    // Returns the name of the parent object if this
    // inherits, 0 otherwise.
    Opencxx::Ptree *inherit() const;

  protected:
    friend class MmuLookupHandler;

    void setup_test_set_handler(MmuTestSetHandler &,MmuTestSet &);

    MmuLookupHandlers _test_sets;
  }; 

  // Handler for the MMU lookup sub-object.
  class MmuLookupHandler : public AspectRemoveHandler {
  public:
    MmuLookupHandler(Opencxx::Environment *e,Opencxx::Ptree *p,const gc_string &name,bool m) :
      AspectRemoveHandler(e,p,name,m), _lookup(0) {};

    virtual Handler *add_define(MsgLog &ml,Opencxx::Environment *e,Opencxx::Ptree *p,
                                const char *type,const char *name,bool modify);
    virtual void handle_key(const char *key,Opencxx::Ptree *value);
    virtual void process_keys(MsgLog &);

    MmuLookup *_lookup;

    // Returns the name of the parent object if this
    // inherits, 0 otherwise.
    Opencxx::Ptree *inherit() const;

  protected:
    friend class MmuHandler;

    void setup_lookup_handler(MmuLookupHandler &,MmuLookup &);
    void setup_field_handler(MmuFieldHandler &,MmuField &);
    void setup_test_set_handler(MmuTestSetHandler &,MmuTestSet &);
    void record_test_order(Opencxx::Ptree *);

    MmuTestSetHandlers _test_sets;
    MmuLookupHandlers  _lookups;
    MmuArrayHandlers   _arrays;
    MmuFieldHandlers   _set_fields;
    MmuFieldHandlers   _way_fields;
  };

  // Handler for MMU fields.
  class MmuFieldHandler : public GenericFieldHandler {
  public:
    MmuFieldHandler(Opencxx::Environment *e,Opencxx::Ptree *p,
                    const gc_string &name,bool m) :
      GenericFieldHandler(e,p,name,m) {}


  protected:
    friend class MmuLookupHandler;

  };

  // Handler for the array sub-object of MMU lookup.
  class MmuArrayHandler : public Handler {
  public:
    MmuArrayHandler(Opencxx::Environment *e,Opencxx::Ptree *p) :
      Handler(e,p), _lookup(0) {};

    virtual void handle_key(const char *key,Opencxx::Ptree *value);

    MmuLookup *_lookup;
  protected:
    friend class MmuLookupHandler;
  };

  // Handler for the cache object.
  class CacheHandler : public AspectRemoveHandler {
  public:
    CacheHandler(Opencxx::Environment *e,Opencxx::Ptree *p,
                 const gc_string &name,bool m) :
      AspectRemoveHandler(e,p,name,m),
      _cache(0)
    {};

    Handler *add_define(MsgLog &ml,Opencxx::Environment *e,Opencxx::Ptree *p,const char *type,const char *name,bool modify);
    virtual void handle_key(const char *key,Opencxx::Ptree *value);
    void process_keys(MsgLog &log);

    Cache    *_cache;

    // Used for group selection.
    bool has_attr(const AttrItem &ai) const;
    bool has_field(const gc_string &field) const { return false; };

  protected:
    void record_sets(Opencxx::Ptree *p);
    void record_ways(Opencxx::Ptree *p);
    void setup_field_handler(CacheFieldHandler &,CacheField &);

    CacheFieldHandlers  _set_fields;
    CacheFieldHandlers  _way_fields;
  };

  // Handler for cache fields.
  class CacheFieldHandler : public GenericFieldHandler {
  public:
    CacheFieldHandler(Opencxx::Environment *e,Opencxx::Ptree *p,
                      const gc_string &name,bool m) :
      GenericFieldHandler(e,p,name,m) {}

  protected:
    friend class CacheHandler;
  };

  // Handler for the event bus object.
  class EventBusHandler : public AspectRemoveHandler {
  public:
    EventBusHandler(Opencxx::Environment *e,Opencxx::Ptree *p,
                    const gc_string &name,bool m) :
      AspectRemoveHandler(e,p,name,m),
      _bus(0)
    {};

    Handler *add_define(MsgLog &ml,Opencxx::Environment *e,Opencxx::Ptree *p,const char *type,const char *name,bool modify);
    virtual void handle_key(const char *key,Opencxx::Ptree *value);
    void process_keys(MsgLog &log);

    // Used for group selection.
    bool has_attr(const AttrItem &ai) const;

    EventBus    *_bus;

  protected:
    friend class ArchHandler;

    void setup_field_handler(EventBusFieldHandler &,EventBusField &);

    EventBusFieldHandlers  _fields;
  };

  // Handler for event bus fields.
  class EventBusFieldHandler : public GenericFieldHandler {
  public:
    EventBusFieldHandler(Opencxx::Environment *e,Opencxx::Ptree *p,
                         const gc_string &name,bool m) :
      GenericFieldHandler(e,p,name,m) {}

  protected:
    friend class EventBusHandler;
  };

  // Handler for opaque external resources.
  class ExtResourceHandler : public AspectRemoveHandler {
  public:
    ExtResourceHandler(Opencxx::Environment *e,Opencxx::Ptree *p,
                    const gc_string &name,bool m) :
      AspectRemoveHandler(e,p,name,m),
      _res(0)
    {};

    virtual void handle_key(const char *key,Opencxx::Ptree *value);

    // Used for group selection.
    bool has_attr(const AttrItem &ai) const;
    bool has_field(const gc_string &field) const { return false; };

    ExtResource    *_res;

  protected:
    friend class ArchHandler;

    void record_constr_arg(Opencxx::Ptree *);
  };

  // Handler for Memory objects
  class MemoryHandler : public AspectRemoveHandler {
  public:
    MemoryHandler(Opencxx::Environment *e, Opencxx::Ptree *p,
                  const gc_string &name,bool m) :
      AspectRemoveHandler(e,p,name,m),
      _memory(0)
    {};
  
    virtual void handle_key(const char* key, Opencxx::Ptree *value);

    // Used for group selection.
    bool has_attr(const AttrItem &ai) const;

    Core	  *_core;
    Memory  *_memory;    
  protected:
    friend class ArchHandler;
  } ;
  
  // Handler for group bjects
  class GroupHandler : public RemoveHandler {
  public:
    GroupHandler(Opencxx::Environment *e, Opencxx::Ptree *p,
                 const gc_string &name,bool m,ArchHandler &ah) :
      RemoveHandler(e,p,name,m), _group(0), _ah(ah)
    {};
  
    virtual void handle_key(const char* key, Opencxx::Ptree *value);
    void record_items(Opencxx::Ptree *p);
    void record_filter(Opencxx::Ptree *p,StrSet &filters);
    void record_attr_filter(Opencxx::Ptree *p,AttrItems &filters);

    void resolve_items(StrSet *result);

    Group       *_group; 
    ArchHandler &_ah;
  protected:
    // This resolves a single group.
    static void resolve_group_items(StrSet *result,const ArchHandler& ah,Group &group,GroupHash& groups, unsigned depth);
    // This resolves set operations
    static void resolve_group_operations(Opencxx::Ptree* value,StrSet &result,const ArchHandler& ah,
                                         Group &group,GroupHash& groups, unsigned depth);
    // Get all items of specified before type.
    static void fill_group_with_all_items(Opencxx::Ptree* loc,const ArchHandler& ah,const Group& group,StrSet &result);
    // Lookup for group in a hash.
    static Group* find_group(Opencxx::Ptree* loc,GroupHash &groups,Opencxx::Ptree *sym);
    // Checks validity of group items
    static void check_group(Opencxx::Ptree* loc, const ArchHandler& ah, Group& group);
    // Returns type if string.
    static gc_string get_type(Opencxx::Ptree* loc,const ArchHandler& ah,const gc_string& str);
    // Returns set of names for the given typ of defines.
    static gc_set<gc_string> get_names(Opencxx::Ptree* loc,const ArchHandler& ah,const gc_string& str);

    friend class ArchHandler;
  } ;
 
  // Handler for assembler params
  class AsmParmHandler : public RemoveHandler {
  public:
    AsmParmHandler(Opencxx::Environment *e, Opencxx::Ptree *p,const gc_string &name,bool m) :
    RemoveHandler(e,p,name,m),_asmparm(0) {};
    
    virtual void handle_key(const char* key,Opencxx::Ptree *value);

  protected:
    friend class AsmConfigHandler;
    
    AsmParm  *_asmparm;
  };

  // Handler for assembler rules
  class AsmRuleHandler : public RemoveHandler {
  public:
    AsmRuleHandler(Opencxx::Environment *e,Opencxx::Ptree *p,const gc_string &name,bool m) :
    RemoveHandler(e,p,name,m),_asmrule(0) {};
    
    virtual void handle_key(const char *key,Opencxx::Ptree *value);
    
  protected:
    friend class AsmConfigHandler;

    AsmRule *_asmrule;
  };

  // Assembler setupe handler class.
  class AsmConfigHandler : public Handler {
  public:
    AsmConfigHandler(Opencxx::Environment *e,Opencxx::Ptree *p) :
      Handler(e,p),_asmcfg(0){};

      virtual Handler *add_define(MsgLog &ml,Opencxx::Environment *e,Opencxx::Ptree *p,const char *type,const char *name,bool modify);
      virtual void handle_key(const char *key,Opencxx::Ptree *value);
      void process_keys(MsgLog &log);

      void record_comment_adapter(Opencxx::Ptree *p);
      void record_line_comment_adapter(Opencxx::Ptree *p);
      void record_line_separator_adapter(Opencxx::Ptree *p);
      void record_instr_separator_adapter(Opencxx::Ptree *p);
      void record_symbol_chars_adapter(Opencxx::Ptree *p);
      void record_operand_symbols_excluded_adapter(Opencxx::Ptree *p);
      void record_group_separator_adapter(Opencxx::Ptree *p);
      void record_asm_instrtables_adapter(Opencxx::Ptree *p);
      void record_disasm_instrtables_adapter(Opencxx::Ptree *p);
    
  protected:
      void setup_parm_handler(AsmParmHandler &ph, AsmParm &ap);
      void setup_rule_handler(AsmRuleHandler &rh, AsmRule &ar);

      friend class ArchHandler;
      AsmParmHandlers _parms;
      AsmRuleHandlers _rules;			   

      AsmConfig *_asmcfg;
  };


}

#endif

//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This class stores configuration information for an ISS.
//

#ifndef _CONFIG_DB_H_
#define _CONFIG_DB_H_

#include <iosfwd>
#include <vector>

#include "gc/gc_cpp.h"

#include "helpers/gc_string.h"

#include "fe/Types.h"

namespace adl {

  class System;
  class Core;
  class Data;
  class Instr;
  class Reg;
  class AnyOption;
  class ConfigDB;

  // This macro adds a member of type t named _x.  It also adds an accessor named
  // x() and a set function named set_x().  You still need to add a default value
  // by adding to the initializer list of the parent class.
#define AddProperty(t,x) public:                \
  t x() const { return _##x; };                 \
    void set_##x(t v) { _##x = v; };            \
private:                                        \
t _##x

  // Mutable variant.
#define AddMProperty(t,x) public:               \
  t x() const { return _##x; };                 \
    void set_##x(t v) const { _##x = v; };      \
private:                                        \
mutable t _##x

  // Just provides a simple reference accessor for the item.
#define AddRProperty(t,x) public:                \
  const t &x() const { return _##x; };                 \
private:                                        \
t _##x

  // Same as above, but designed for aggregate objects.  The accessor returns a reference
  // and a has_ method is created to see if any elements exist in the object.
#define AddRefProperty(t,x) public:                 \
    const t &x() const { return _##x; };            \
    t &get_##x() { return _##x; };            \
    bool has_##x() const { return !_##x.empty(); }; \
    void set_##x(const t &v) { _##x = v; };         \
private:                                            \
t _##x



  // Specifically for strings.
#define AddStrProperty(x) public:                 \
  const gc_string &x() const { return _##x; };        \
  const std::string x##_str() const { return _##x.c_str(); };  \
    gc_string &get_##x() { return _##x; };            \
    bool has_##x() const { return !_##x.empty(); }; \
    void set_##x(const gc_string &v) { _##x = v; };         \
    void set_##x(const std::string &v) { _##x = v.c_str(); }; \
private:                                            \
gc_string _##x

  // This class stores data shared by Systems and Cores. 
  // Currently, it handles the 
  class BaseConfig : public gc {
  public:
    typedef gc_map<const gc_string, bool> Memories;

  protected:
    BaseConfig() {}
    void  set_extern_mem(const gc_string &n, bool ext);

  public:

    bool	extern_mem(const gc_string &n) const; 
    const Memories &memories() const 	{ return _memories; }
    const StrSet   &extern_mems() const   { return _extern_mems; } 
    void	dump(std::ostream &, const gc_string &pfx) const;
  
    friend class MemoryCfgHandler;

  private:
    Memories  _memories;
    StrSet    _extern_mems;
  };


  // Store infomration about configuration for a system.
  class SysConfig : public BaseConfig {
  public:
    SysConfig(const System *sys) : 
      _sys(sys) 
    {}

    void	dump(std::ostream &o, const gc_string &pfx) const { BaseConfig::dump(o,pfx); }

    friend class SysConfigHandler;

  private:
    const System*  _sys;		    
  };

  // Stores information about configuration information for a single core.
  class CoreConfig : public BaseConfig {
  public:
    CoreConfig(const Core *core) :
      _gen_mmu(true),
      _gen_caches(true),
      _dmi_cache_support(false),
      _log_orig_mem_access(false),
      _issue_width(1),
      _mem_split_size(0),
      _tracing_on(0),
      _tracing_off(0),
      _post_sim(0),
      _compl_time(0),
      _clock_cycle_ratio(0),
      _need_packet_values(false),
      _virtual_mem_interface(true),
      _ignore_latency(false),
      _external_dmi_write_update(true),
      _cache_line_size(64), // Arbitrary default- use a 64-byte cache line
                            // unless otherwise specified.
      _default_grp(0),
      _default_graphite_grp(0),
      _last_graphite_id(0),
      _core(core)
    {}

    typedef gc_map<int,int> Attrs;

    // Various configuration properties.
    AddStrProperty(core_base);                 // The core's additional base class.
    AddProperty(bool,gen_mmu);                 // True => generate the MMU if it exists.
    AddProperty(bool,gen_caches);              // True => generate the caches if they exist.
    AddProperty(bool,dmi_cache_support);       // True => Create the internal DMI cache.
    AddProperty(bool,log_orig_mem_access);     // True => Store top-level memory
                                               // access info for hybrid or transactional ISSs.
    AddProperty(int, issue_width);             // Issue-width of processor, if time-tagged.
    AddProperty(int, mem_split_size);          // Memory access split size, in
                                               // bytes.  4 = split accesses
                                               // misaligned on 32-bits, 8 =
                                               // split on double-word. 0 = use
                                               // default.

    // These are mutable so that we can update them with the new, translated
    // function later on.
    AddMProperty(Opencxx::Ptree *,tracing_on);  // Tracing-turning-on hook.
    AddMProperty(Opencxx::Ptree *,tracing_off); // Tracing-turning-off hook.
    AddMProperty(Opencxx::Ptree *,post_sim);    // Post-simulation hook.
    AddMProperty(Opencxx::Ptree *,compl_time);  // Completion-time hook.

    AddProperty(double, clock_cycle_ratio);    // Cycle ratio used for
                                               // triggering pre/post-cycle
                                               // hooks.
    AddRefProperty(StrSet,reg_callbacks);      // Registers which should be
                                               // configured to have callbacks.
    AddRefProperty(StrSet,reg_read_callbacks); // Registers which should be
                                               // configured to have callbacks.
    AddRefProperty(StrSet,except_callbacks);   // Exceptions which should be
                                               // configured to have callbacks.
    AddRefProperty(StrSet,bb_end_instrs);      // Explicitly force these instructions to end a basic block.
    AddRefProperty(StrSet,opt_bb_end_instrs);  // Optionally allow these instructions to end a basic block.
    AddRefProperty(StrSet,split_branch_begin); // Indicates an instruction which is the first half of a split branch pair.
    AddRefProperty(StrSet,split_branch_end);   // Indicates an instruction which is the second half of a split branch pair.
    AddRefProperty(StrSet,split_branch_cancel);// Indicates an instruction which cancels a fixed-target split branch pair.
    AddRefProperty(StrSet,dyn_caches);         // Caches which should be configured to be run-time dynamic.
    AddRefProperty(StrSet,dyn_tlbs);           // MMU lookups which should be configured to be run-time dynamic.
    AddProperty(bool,need_packet_values);      // True => We need values in the
                                               // instruction packet, even if a
                                               // hyrid ISS.  Only relevant for
                                               // hybrid ISSs, as otherwise it's
                                               // defined.
    AddProperty(bool,virtual_mem_interface);   // For transactional ISSs, use a
                                               // virtual interface for various
                                               // memory interface functions.
                                               // Otherwise, no implementation
                                               // (just an interface) is created
                                               // and another code generator has
                                               // to create the functions.
    AddProperty(bool,ignore_latency);          // If true, then do not generate
                                               // the handle_latency function
                                               // for transactional ISSs.
    AddRefProperty(StrList,extra_members);     // Extra member variables.
    AddRefProperty(StrList,extra_inits);       // Inits for the extra variables.
    AddRefProperty(StrSet,trans_const_parms);  // Arch parms the JIT can consider to be constants at translation time.
    AddRefProperty(IntOrFuncMap,cache_miss_latency); // Latencies for caches on
                                                     // a miss, in a time-tagged
                                                     // ISS.  Can be either a
                                                     // fixed constant or a
                                                     // function evaluated at
                                                     // time of miss.
    AddProperty(bool,external_dmi_write_update); // True => External DMI should
                                                 // update internal DMI.  Defaults
                                                 // to true, but can be disabled
                                                 // for certain situations,
                                                 // e.g. write-back L1 caches.

    AddProperty(unsigned,cache_line_size);     // Only used when interfacing
                                               // with external memory and we
                                               // want icache touch logic.

  public:

    // Stores information about a pipeline stage for a time-tagged ISS.
    class Stage {
      AddStrProperty(name);            // Stage's name.
      AddStrProperty(tag);             // A tag associated with the stage.

      friend class GroupCfgHandler;
    public:
      Stage() {};
    };

    typedef gc_vector<Stage> Stages;

    // Stores information about a group.
    class Group {
      AddStrProperty(name);                    // Group name.
      AddRefProperty(Stages, stages);          // For time-tagged ISSs: Stages
                                               // associated with this group.
      AddRProperty(PAuxFunc,allow_issue);      // Predicate function to allow
                                               // next instruction in a
                                               // multi-issue core to issue.
      AddProperty   (unsigned,latency);        // Latency of instructions
                                               // in this group.

      StrVect _items;                           // Temporary list of items, used during parsing.

      friend class GroupCfgHandler;
      friend class CoreCfgHandler;
    public:
      Group() :
        _latency(1)
      {};

      void finalize(Core &core,MsgLog &ml,CoreConfig &cfg,bool check_aux_funcs);

    };
    typedef gc_map<gc_string,Group> Groups;

    // Stores information about a group for mapping instructions to Graphite instruction classes.
    class GraphiteGroup {
      AddStrProperty(name);                     // Group name
      AddProperty(unsigned,id);                 // Unique ID for creating the enum.
      
      StrVect _items;                           // Temporary list of items, used during parsing.

      friend class GraphiteGroupCfgHandler;
      friend class CoreCfgHandler;
    public:
      GraphiteGroup() : _id(0) {};

      void finalize(MsgLog &ml,CoreConfig &cfg,unsigned id);
    };
    typedef gc_map<gc_string,GraphiteGroup> GraphiteGroups;

    const Core *core() const { return _core; };

    const Groups &groups() const { return _groups; };

    const GraphiteGroups &graphite_groups();

    // Get a group by instruction/instruction class.
    // Returns 0 if nothing is associated.
    const Group *get_group(const Instr &instr) const;

    // Get a Graphite group by instruction name.
    const GraphiteGroup &get_graphite_group(const Instr &instr) const;

    // Access the database of instructions which need to have operands read (for
    // a hybrid ISS).
    StrSet &readOpInstrs() { return _readOpInstrs; };
    const StrSet &readOpInstrs() const { return _readOpInstrs; };

    // Lookup a latency for an instruction (by name).
    // Returns the value specified by 'def' if not found.
    int get_latency(const Instr &,int def = 1) const;

    // Returns true if the group name exists.
    bool validGroupName(const gc_string &n) const { return _groups.count(n); };

    void finalize(MsgLog &ml,bool check_aux_funcs);

    // Dump database information to the specified stream.
    void dump(std::ostream &,const gc_string &pfx) const;

  private:
    friend class GroupCfgHandler;
    friend class GraphiteGroupCfgHandler;
    friend class ConfigCfgHandler;
    friend class CoreCfgHandler;

    typedef gc_map<gc_string,gc_string> InstrMap;

    template <typename G,typename C>
    const G &get_group(const C &c,const gc_string &n) const;

    template <typename G,typename C>
    const G *get_group_from_instr(const C &c,const G *default_grp,const InstrMap &instr_mapping,const Instr &instr) const;

    void check_for_graphite_default() const;

    // Groups in this core.
    Groups      _groups;  

    // Maps instructions or instruction classes to groups.
    InstrMap    _instr_mapping;

    // Instructions which need operands to be read, if a hybrid ISS is being
    // generated.
    StrSet      _readOpInstrs;

    Group      *_default_grp;
  
    // For the Graphite perf model integration, this is used for mapping
    // instructions to instruction classes.
    mutable GraphiteGroups _graphite_groups;
    InstrMap               _graphite_instr_mapping;
    mutable GraphiteGroup *_default_graphite_grp;
    mutable unsigned       _last_graphite_id;

    // The core associated with this configuration.
    const Core *_core;
  };

  // The types of ISS.  Options are:
  //
  // 1.  tNormal: No tracking of operands, execution is immediate.
  //
  // 2.  tHybrid: Operands are tracked in an instruction packet, execution is
  //     immediate.
  //
  // 3.  tTransactional: Operands are tracked in an instruciton packet, execution
  //     occurs within the packet.
  //
  enum IssType { tNormal, tHybrid, tTransactional };

  // The types of memory interface for ISSs.  These are only relevant for
  // non-normal ISSs.  Options are:
  //
  // 1.  mNonBlocking:  Only relevant for transactional ISSs.  All memory is handled by the external
  //     model via a coroutine interface.
  //
  // 2.  mBlocking: Generally only used for hybrid models, though it could be
  //     used for a transactional ISS.  A memory access is handled via a virtual
  //     function, no coroutines are used.
  //
  // 3.  mLogging: Only relevant for hybrid models, e.g. used by uADL for
  //     iss-mem-mode.  In this mode, memory is handled directly by the ISS, but
  //     the accesses are also logged so that the external model can track it.
  //
  enum MemType { mNonBlocking, mBlocking, mLogging };

  // Generic tri-value non-set/true/false type.
  enum TriBool { tFalse, tTrue, tNotSet };

  // Outer database storing a hierarchy of core configuration information.
  class ConfigDB : public gc {
  public:
    ConfigDB() :
      _iss_type(tNormal),
      _track_reads(false),
      _log_reg_reads(false),
      _log_reg_masks(false),
      _log_unarch(true),
      _trans_pool_size(2),
      _coro_pool_size(2),
      _dmi_cache_support(false),
      _dmi_cache_size(2048),
      _dc_support(false),
      _jit_support(false),
      _graphite_support(false),
      _icache_touch(tNotSet),
      _jit_ea_hash_size(10),
      _rnumber_support(true),
      _trace_mode(true),
      _log_instr_args_mode(false),
      _debug_mode(true),
      _verbose_mode(0),
      _ld_okay(false),
      _time_tagged(false),
      _syscall_enabled(true),
      _extern_mem(false),
      _extern_dmi(false),
      _extern_dmi_entry_size(10),
      _extern_dmi_size(2048),
      _num_compile_jobs(0),
      _mem_if(mNonBlocking),
      _trans_ifetch_accesses(2),
      _trans_split_fetch(true),
      _skip_caches(false),
      _log_fetches(true),
      _tags_only_caches(false),
      _low_mem(false),
      _log_instr_reads(true),
      _max_delay_slots(3),
      _packet_pool_size(1000),
      _rollback_mode(false),
      _ignore_mode(false),
      _disassembler(false),
      _library_mode(false),
      _event_bus_send(true),
      _extern_event_bus(false),
      _td_api(false),
      _dep_tracking(false),
      _add_level_sensitive_support(false),
      _clear_pending_excepts(true),
      _trans_rethrow(false),
      _all_reg_aliases(true),
      _prefetch_cia_incr(true),
      _instr_size_check(true),
      _add_branch_hook(false),
      _log_writereg(true),
      _incl_unused_helpers(false),
      _tlm2_endianness(false),
      _per_instr_td_incr(true),
      _mt_support(false),
      _mt_rw_locked_mem(true),
      _mt_locked_mem(false),
      _gen_all_reg_code(false),
      _log_td_commit(false)
    {
    }

    AddProperty(IssType,iss_type);           // The type of ISS to generate.  Refer to IssType.
    AddProperty(bool,track_reads);           // for hybrid or transactional ISSs, track operands read.
    AddProperty(bool,log_reg_reads);         // If true, we log register reads.
    AddProperty(bool,log_reg_masks);         // If true, we log register read/write partial masks.
    AddProperty(bool,log_unarch);            // If true, we log unarchitected/indeterminate registers, exceptions, etc.
    AddProperty(int, trans_pool_size);       // Max number of packets for a
                                             // transactional ISS.  If -1, then
                                             // the non-pool based allocator is
                                             // used, which only allocated.
                                             // This way, an external
                                             // instruction cache can be used.
    AddProperty(int, coro_pool_size);        // Max number of outstanding memory coroutines for a transactional ISS.
    AddProperty(bool,dmi_cache_support);     // True => Generate a DMI
                                             // (direct-memory interface) cache
                                             // for the model.  Note that this
                                             // is the global override flag.  If
                                             // false, we don't generate any
                                             // support, but if true, then it's
                                             // on a per-core basis.
    AddProperty(int,dmi_cache_size);         // Size of the DMI cache.
    AddProperty(bool,dc_support);            // Flag to indicate whether to generate a decode-cache model.
    AddProperty(bool,jit_support);           // Flag to indicate whether to generate a JIT model.
    AddStrProperty(jit_ihname);              // Name of the instruction handler source file to generate.
    AddStrProperty(jit_bcname);              // Bitcode file name to be read by final JIT model.
    AddProperty(bool,graphite_support);      // Enable Graphite integration support.
    AddProperty(TriBool,icache_touch);       // True => JIT and trace-cache models should touch the icache.
    AddProperty(int,jit_ea_hash_size);       // Size (in bits, i.e. size is 2^n) of fast ea hash for JIT.
    AddProperty(bool,rnumber_support);       // True => Include RNumber support.
    AddProperty(bool,trace_mode);            // True => Generate tracing support.
    AddProperty(bool,log_instr_args_mode);   // True => Generate arguments logging support.
    AddProperty(bool,debug_mode);            // True => Generate brkpt/wtchpt support.
    AddProperty(int,verbose_mode);           // Non-zero => Verbose model generation.
    AddProperty(bool,ld_okay);               // True => Insert line directives.
    AddProperty(bool,time_tagged);           // True => Generate time-tagged ISS.
    AddProperty(bool,syscall_enabled);       // True => System call support enabled.
    AddProperty(bool,extern_mem);            // True => Mem is external.
    AddProperty(bool,extern_dmi);            // True => Create an external DMI interface.
    AddProperty(int,extern_dmi_entry_size);  // Entry size of the external DMI in bits, e.g. 7 = 128 byte entry.
    AddProperty(int,extern_dmi_size);        // Number of entries in the external DMI.
    AddStrProperty(iss_namespace);           // Namespace to use to wrap ISS.
    AddProperty(int,num_compile_jobs);       // Non=zero => Split ISS into multiple instruction groups for separate compilation.
    AddProperty(MemType,mem_if);             // Memory interface type. Refer to declaration of MemType for more information.
    AddProperty(unsigned,trans_ifetch_accesses); // For a non-blocking
                                                 // transactional ISS, number of
                                                 // possible fetch accesses.  We
                                                 // default to 2 to support a
                                                 // misaligned fetch, but uADL
                                                 // generally sets this to 1,
                                                 // since the fetch unit handles
                                                 // this itself.
    AddProperty(bool,trans_split_fetch);     // For transactional ISSs, true => fetch may be split due to a misaligned read.
    AddProperty(bool,skip_caches);           // True => The memory interface should skip any caches and deal directly with memory.
    AddProperty(bool,log_fetches);           // True => Logging-memory should log fetches.
    AddProperty(bool,tags_only_caches);      // True => Caches track only tags and store no data.
    AddProperty(bool,low_mem);               // True => generate leaf wrappers for instruction actions    
    AddProperty(bool,log_instr_reads);       // True => ISS should log
                                             // instruction reads.  False =>
                                             // Performance model must do it.
    AddProperty(int, max_delay_slots);       // Max number of delay slots
                                             // (length of a delayed write).
    AddProperty(int, packet_pool_size);      // Number of elements in the delayed register write pool objects.
    AddProperty(bool,rollback_mode);         // True => Enable rollback.
    AddProperty(bool,ignore_mode);           // True => Enable ignore-mode (can ignore register and stores).
    AddProperty(bool,gen_instr_attr);        // True => Generate instructions attributes to tansactional-ISS
    AddProperty(bool,disassembler);          // True => Generate a disassembler function in the model.
    AddProperty(bool,library_mode);          // True => Generate the model for instruction functionality only.
    AddProperty(bool,event_bus_send);        // True => Event buses should transmit data.
    AddProperty(bool,extern_event_bus);      // True => Handle all event buses opaquely using an installed handler.
    AddProperty(bool,td_api);                // True => Add support for the temporal-decoupling API.
    AddProperty(bool,dep_tracking);          // True => Add in data-dependency tracking.
    AddProperty(bool,add_level_sensitive_support); // True => Always add level-sensitive exception support.
    AddProperty(bool,clear_pending_excepts); // True => If an exception moves from pending to active, clear in pending.
    AddProperty(bool,trans_rethrow);         // True => In a transactional ISS,
                                             // rethrow exceptions out of the
                                             // instruction packet.
    AddProperty(bool,all_reg_aliases);       // True => Track all register aliases in trans-ISS register alias resolver.
    AddProperty(bool,prefetch_cia_incr);     // True => Prefetch shouldw increment cia for a transactional iss.
    AddProperty(bool,instr_size_check);      // True => Add in checks for
                                             // transactional ISS to make sure
                                             // that instruction size matches
                                             // fetched size.
    AddProperty(bool,add_branch_hook);       // True => Add a call to the branch hook for a hybrid-iss, at end of branch handler code.
    AddProperty(bool,log_writereg);          // True => writeReg calls are logged, bracketed by log_ext_reg_start/log_ext_reg_stop.
                                             // program-name and version.
    AddProperty(bool,incl_unused_helpers);   // True => Include unused helper functions, false => omit from SS.
    AddProperty(bool,tlm2_endianness);       // True => Use address swizzling vs. byte swapping for endianness conversion.
    AddProperty(bool,per_instr_td_incr);     // True => Increment temporal-decoupling counter per instruction.
                                             // False => Group increments at end of basic blocks.
    AddProperty(bool,mt_support);            // True => Generate necessary code to support multithreaded simulation.
    AddProperty(bool,mt_rw_locked_mem);      // True => Lock in-core read/write memory accesses during multithreaded simulation.
    AddProperty(bool,mt_locked_mem);         // True => Lock all memory accesses during multithreaded simulation.
    AddStrProperty(log_usage);               // Usage logging string.  If default, then use default values for 
    AddStrProperty(decode_rtype);            // For a transactional ISS, specify an alternate return type for decode functions.
    AddStrProperty(decode_arg);              // As above- extra argument for decode functions.
    AddStrProperty(decode_argtype);          // As above- extra argument's type for decode functions.
    AddStrProperty(decode_rpfx);             // As above- prefix for function call on return.
    AddRefProperty(StrList,cpp_cmds);        // Extra include files or defines to include at the top of the ISS.
                                             // If the string starts with "define:", it's considered a
                                             // define symbol.  If it starts with "include:" it's considered an
                                             // include directive.
    AddRefProperty(StrSet,gen_full_regs);    // Generate all relevant code for the specified registers.
    AddProperty(bool,gen_all_reg_code);      // Generate all code for all registers.
    AddProperty(bool,log_td_commit);         // Log instruction completion times using the temporal-decoupling counter value.

  public:
    gc_string _preamble;                   // Preamble string printed by the ISS.

    // fn:    Configuration file name.  If empty, then no action taken.
    // data:  ADL data model, for cross-referencing and error checking purposes.
    // options: Options database for getting configuration info such as defines and includes.
    // check_aux_funcs:  At the end of parsing, check any auxiliary functions added to the data model.
    // Throws a runtime_error if there is a problem parsing the file.
    void parse(const std::string &fn,const Data &data,const AnyOption &options,bool check_aux_funcs = false);

    // Return a configuration item for a specified core, returning
    // 0 if the object does not exist.
    const CoreConfig *getCoreConfig(const Path &path) const;
    CoreConfig *getCoreConfig(const Path &path);

    // Returns a configuration item for the first leaf node (core).
    const CoreConfig *getFirstCoreConfig() const;

    // Create a core configuration object if a top-level object does not already
    // exist.  If it does exist, then it returns a reference to that object.
    // Throws a runtime_error if a CoreConfig already exists but has a different
    // core pointer.
    CoreConfig &makeCoreConfig(const Core &core);
    // Similar to the above, but inserts the hierarchy specified by path first,
    // then inserts the core.  The path should not contain the core's name.
    CoreConfig &makeCoreConfig(const Path &path,const Core &core);

    // Return a configuration item for a specified system, returning
    // 0 if the object does not exist.
    const SysConfig *getSysConfig(const Path &path) const;
    SysConfig *getSysConfig(const Path &path);


    const SysConfig *getSysConfig() const { return _top._sysConfig; } 

    void finalize(MsgLog &ml,bool check_aux_funcs);

    // Dump database information to the specified stream.
    void dump(std::ostream &) const;

    // Return all memories external in design
    void extern_mems(StrSet& externs) const;

    friend class OuterCfgHandler;
    friend class OuterConfigCfgHandler;

    struct Node : public gc {
      typedef gc_map<gc_string,Node *> Nodes;

      Nodes       _children;
      CoreConfig *_coreConfig;
      SysConfig  *_sysConfig;

      Node() : _coreConfig(0), _sysConfig(0) {};
      Node(CoreConfig *c) : _coreConfig(c), _sysConfig(0) {};
      Node(SysConfig *c)  : _coreConfig(0), _sysConfig(c) {};
      void finalize(MsgLog &ml,bool check_aux_funcs);
      void dump(std::ostream &,const gc_string &) const;
    };

  private:
    const Node *walk(Path::const_iterator p,Path::const_iterator e,const Node &) const;
    Node *walk_insert(Path::const_iterator p,Path::const_iterator e,Node &cur);
    const CoreConfig *getFirstCoreConfigInternal(const Node *cur) const;

    void  extern_mems(const Node &cur, StrSet &externs) const;

    Node         _top;
  };

}

#endif

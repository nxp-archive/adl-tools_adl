//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Main data structure for storing ADL information.
//

#ifndef _DATA_H_
#define _DATA_H_

#include <iosfwd>
#include <functional>
#include <cassert>
#include <limits>

#include "gc/gc_cpp.h"

#include "helpers/BasicTypes.h"
#include "Types.h"

#define const_core const_cast<const Core*>(_core)

namespace Opencxx {
  class Ptree;
  class PtreeArray;
  class Environment;
}

namespace adl {
  
  class System;
  class Core;
  class Parm;
  class RegBase;
  class RegFile;
  class Reg;
  class RegField;
  class RegFileHook;
  class RegHook;
  class RegEntry;
  class InstrField;
  class PrefixCounter;
  class Relocation;
  class SubInstr;
  class SubInstrGrp;
  class Instr;
  class InstrTable;
  class ExceptionField;
  class Exception;
  class MsgLog;
  class Aspect;
  class MMU;
  class MmuTestSet;
  class MmuLookup;
  class MmuField;
  class AddrMask;
  class Cache;
  class CacheField;
  class Memory;
  class EventBusField;
  class EventBus;
  class ExtResource;
  class Context;
  class Group;
  class BitPair;
  class AsmConfig;
  class AsmParm;
  class AsmRule;
  class InstrBase;
  class CheckCode;

  typedef gc_list<const char*> CharPtrList;
  typedef  gc_list<std::pair<gc_string,int> >   StrIntList;

  enum WriteHookImm { ImmNotSet, ImmFalse, ImmTrue };

  class InstrList;

  struct StrHash : public gc_map<gc_string,int> {};
  

  // I use maps here so that items are ordered in a deterministic fashion.
  typedef gc_map<gc_string,Core *> CoreHash;
  typedef gc_map<gc_string,System *> SysHash;
  typedef gc_map<gc_string,RegFile *> RegFileHash;
  typedef gc_map<gc_string,Reg *> RegHash;
  typedef gc_map<unsigned,Reg *> RegIndexHash;
  typedef gc_map<gc_string,RegField *> RegFieldHash;
  typedef gc_map<unsigned,RegEntry *> RegEntryHash;
  typedef gc_map<gc_string,InstrField *> InstrFieldHash;
  typedef gc_map<gc_string,InstrTable *> InstrTableHash;  // Instruction tables for micro-instructions
  typedef gc_map<gc_string,Relocation *> RelocationHash;
  typedef gc_map<gc_string,SubInstr *> SubInstrHash;
  typedef gc_map<gc_string,SubInstrGrp *> SubInstrGrpHash;
  typedef gc_map<gc_string,Instr *> InstrHash;
  typedef gc_map<gc_string,Exception *> ExceptionHash;
  typedef gc_map<gc_string,Parm *> ParmHash;
  typedef gc_map<gc_string,MmuLookup *> MmuLookupHash;
  typedef gc_map<gc_string,MmuTestSet *> MmuTestSetHash;
  typedef gc_map<gc_string,MmuField *> MmuFieldHash;
  typedef gc_map<gc_string,CacheField *> CacheFieldHash;
  typedef gc_map<gc_string,Cache *> CacheHash;
  typedef gc_map<gc_string,Memory *> MemoryHash;
  typedef gc_map<gc_string,EventBusField *> EventBusFieldHash;
  typedef gc_map<gc_string,ExceptionField *> ExceptionFieldHash;
  typedef gc_map<gc_string,EventBus *> EventBusHash;
  typedef gc_map<gc_string,ExtResource *> ExtResourceHash;
  typedef gc_map<gc_string,Context *> ContextHash;
  typedef gc_map<gc_string,AsmParm *> AsmParmHash;
  typedef gc_map<gc_string,AsmRule *> AsmRuleHash;
  typedef gc_map<gc_string,Aspect *> AspectHash;
  typedef gc_map<gc_string,PrefixCounter *> PrefixCounterHash;

  typedef gc_list<const Relocation *> RelocList;

  struct GroupHash : public gc_map<gc_string, Group *> {};
  typedef gc_vector<Cache *> Caches;
  typedef StrSet	     Blocks;

  typedef gc_list<MmuTestSet *> MmuTestSets;
  typedef gc_list<SubInstr *> SubInstrs;
  typedef gc_list<Instr *> Instrs;
  typedef gc_list<const Aspect *> AspectList;
  
  struct ITableEntry {
    unsigned    _id;
    Instrs      _instrs;  // List of the instructions used in this table.
    InstrTable *_table;   // The instruction table itself.
    unsigned    _pos;     // Position within the original list- 0 is first.

    ITableEntry(unsigned id,InstrTable *t,unsigned pos) : _id(id), _table(t), _pos(pos) {};
  };
  typedef gc_map<unsigned,ITableEntry> InstrTables;

  //
  // Predefined attributes.
  //

  // Current-instruction-address register.
  extern const char *Cia;
  // Next-instruction-address register.
  extern const char *Nia;
  // Unarchitected registers.
  extern const char *Unarchitected;
  // Indeterminate intermediate results.
  extern const char *Indeterminate;
  // Suppress all documentation for this object.
  extern const char *DocHidden;
  // Suppress implementations in documentation.
  extern const char *DocNoCode;
  // Suppress expression expansion in database and documentation.
  extern const char *DocNoExpandExprs;
  // The default instruction table's class.
  extern const char *DefaultInstrTable;
  // The prefix instruction table.
  extern const char *PrefixInstrTable;
  // Default name for MMU tests at the lookup level.
  extern const char *DefaultTestSet;


  // Name of memory object recognized for action code.
  extern const char *MemoryName;
  // Name of the MMU object recognized for action code.
  extern const char *MmuName;
  // Standard name for referring to target/source register
  // in a read/write hook.
  extern const char *CurrentRegName;
  // Standard name for referring to the current cache within
  // a cache hook.
  extern const char *CurrentCacheName;

  // Standard name for start of data pairs in annotation messages (info, warning, error).
  extern const char *AnnotationDataStart;

  // Names for mask types.
  extern const char *EaMaskName;
  extern const char *RaMaskName;

  // Name of the special "reserved" enumerated-field value.
  extern const char *ReservedEnum;

  // Name for the update function for register/register-file concurrent hooks.
  extern const char *UpdateReg;

  // Names for the special getter/setter functions provided for manipulating
  // bits which correspond to register fields in the design.  This is just the
  // leading prefix.
  extern const char *RegFieldGetterPfx;
  extern const char *RegFieldSetterPfx;

  // Used to indicate that a reserved-bit value was not specified.
  enum { RsrvNotSet = 0xff };

  void addGlobalResource(const gc_string &name);
  bool validAliasEscape(const gc_string &name);

  // Stuff common to all data model objects.
  class Base : public gc {
  public:
    Base(Opencxx::Environment *env,Opencxx::Ptree *loc);
    virtual ~Base();

    // By default, it can be shared.  Override this to add checks
    // if this isn't always true.  Should throw an exception if
    // a problem is found.
    virtual void check_sharing_okay() const { };

    // Dump data for debugging purposes.
    virtual void dump(std::ostream &,const gc_string &pfx) const = 0;
    // This dumps to cerr.
    virtual void dump() const;

    Opencxx::Ptree       *loc() const { return _loc; };
    Opencxx::Environment *env() const { return _env; };

  protected:
    // Used for locating the object in the source file when generating an error
    // message.
    Opencxx::Ptree       *_loc;
    // Used for checking code for escaping variables and stuff like that.
    Opencxx::Environment *_env;
  };

  // Subclass for something with a document string.
  struct Doc {
    virtual ~Doc() {};

    const gc_string &doc() const { return _doc; };

    virtual void dump_doc(std::ostream &,const gc_string &pfx,bool header) const; 
  protected:
    gc_string _doc;
  };

  // Subclass for something that has a name and a doc string.
  struct Named : public Doc {
    Named () {};
    Named (const gc_string &n) : _name(n) {};

    const gc_string &name() const { return _name; };
    const gc_string &docName() const { return (has_doc_name()) ? _doc_name : _name; };
    const gc_string &docNameAdd() const { return _doc_name_add; };

    bool has_doc_name() const { return !_doc_name.empty(); };
    bool has_doc_name_add() const { return !_doc_name_add.empty(); };

    void setDocName(const gc_string &n) { _doc_name = n; };
    void setDocNameAdd(const gc_string &n) { _doc_name_add = n; };
    void setName(const gc_string &n) { _name = n; };

    virtual void dump_doc(std::ostream &,const gc_string &pfx,bool header) const; 
  protected:
    gc_string _name;
    gc_string _doc_name;     // name that is customized for display, e.g. in documentation
    gc_string _doc_name_add; // additional info to add to the title
  };

  // Functor useful with STL algorithms.
  struct GetNamed {
    gc_string operator()(const Named *x) const { return x->name(); };
  };

  // An item for storing an attribute parameter.
  struct AttrParams {  
    enum Type {NoParam, Int, IntList, Str, StrList};    

    Type         _type;
    adl::IntList _int_list;
    adl::StrList _str_list; 
    bool         _remove;
     
    AttrParams():_type(NoParam),_remove(false){};      
    AttrParams(Type t):_type(t),_remove(false) {};
    AttrParams(gc_string s):_type(Str),_remove(false) { _str_list.push_back(s);};      
    AttrParams(int i):_type(Int),_remove(false) { _int_list.push_back(i);};       
    AttrParams(adl::StrList &sv):_type(StrList),_str_list(sv),_remove(false){};      
    AttrParams(adl::IntList &iv):_type(IntList),_int_list(iv),_remove(false){};   
      
    bool operator==(const AttrParams &x) const;

    void record_int_param(Opencxx::Ptree* p,Opencxx::Environment *env); 
    void record_str_param(Opencxx::Ptree* p);
  };

  // id of attribute and list of parameters
  typedef gc_map<unsigned,AttrParams> AttrList;

  // Stores an attribute using its integer handle, plus a parameter value, which
  // might be empty.
  struct AttrItem : public gc {
    AttrItem() : _attr(-1) {};
    AttrItem(int attr) : _attr(attr) {};
    AttrItem(int attr,AttrParams::Type type) : _attr(attr), _params(type) {};

    bool valid() const { return _attr >= 0; };

    int attr() const { return _attr; };
    const AttrParams &params() const { return _params; };
    
  protected:
    friend AttrItem parse_attr_item(Opencxx::Ptree *,bool,Opencxx::Environment *);
    friend class Attrs;

    int        _attr;
    AttrParams _params;
  };

  struct AttrItems : public gc_list<AttrItem> {};

  std::ostream &operator<<(std::ostream &,const AttrItem &);
  std::ostream &operator<<(std::ostream &os,AttrParams::Type t);
  std::ostream &operator<<(std::ostream &os,const AttrParams &);
  std::ostream &operator<<(std::ostream &,const AttrItems &);

  // This stores information about an aspect.
  class Aspect : public Base, public Named {
  public:
    Aspect (Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);

    const StrSet &labels() const { return _labels; };
    const PVect &def_checks() const { return _def_checks; };
    const AttrItems &attrs() const { return _attrs; };
    Opencxx::Ptree *action() const { return _action; };
		bool instr_pre_exec() const { return _instr_pre_exec; };
		bool instr_post_exec() const { return _instr_post_exec; };
    bool exclusive() const { return _exclusive; };
    
    bool has_priority() const { return _priority != std::numeric_limits<unsigned>::max(); };
    unsigned priority() const { return _priority; };

    void dump(std::ostream &o,const gc_string &pfx) const;
  protected:
    friend class AspectHandler;
    friend class AspectGrpHandler;

    StrSet          _labels;          // C labels that trigger the aspect.
    PVect           _def_checks;      // Only expand if these symbols are defined.
    AttrItems       _attrs;           // Attributes to check in parent.
    bool            _instr_pre_exec;  // Trigger on start of instruction-action code.
    bool            _instr_post_exec; // Trigger on end of instruction-action code.
    bool            _exclusive;       // Do not continue on up the hierarchy,
                                      // looking for other aspects.
    unsigned        _priority;        // Priority value, if relevant.  maxint indicates no priority.
    Opencxx::Ptree *_action;          // Action code to be inserted.
  };

  // Subclass for something that can have aspects attached.
  class Aspects : public Base {
  public:
    Aspects (Opencxx::Environment *env,Opencxx::Ptree *loc);

    const AspectHash &aspects() const { return _aspects; };

    // Returns true if we have the specified attribute.  A bit of a hack having
    // it here, but this is because Attrs derives from Aspects and we want an
    // aspect to be able to query to see if its parent object contains an
    // attribute.
    virtual const AttrParams *has_attr(unsigned c) const { return 0; }

    // Same as above, but uses an AttrItem so that parameters are matched as well.
    virtual const AttrParams *has_attr(const AttrItem &ai) const { return 0; }    

    // Updates the list with all aspects that have a label matching 'label'.
    // Returns true if any are found to be exclusive.  This is virtual, so
    // derived objects can override it to fetch aspects in different ways, such
    // as by querying child objects.
    // Note: The AspectList argument is not cleared.  Items are simply added.
    virtual bool getAspects(AspectList &,const gc_string &label) const;

    void dump(std::ostream &o,const gc_string &pfx) const;
  protected:
    // For merging purposes.
    void add_aspects(const Aspects &src);

    friend class AspectGrpHandler;

    AspectHash _aspects;
  };

  // Subclass for something which has attributes associated with it.  These are
  // stored as a list of integers and correspond to items stored in another class,
  // such as Core.
  struct Attrs : public Aspects {
    Attrs (Opencxx::Environment *env,Opencxx::Ptree *loc) : 
      Aspects(env,loc),
      _inherit(true)
    {};

    // Whether this object should inherit attributes (if applicable).
    bool will_inherit_attrs() const { return _inherit; };
   
    // Returns true if any attributes exist.
    bool has_attrs() const { return !_attrs.empty(); };

    void set_inherit_attrs(bool x) { _inherit = x; };
    // Manipulations with attributes. Returns false if the specified attribute does not exist.
    bool add_attr(AttrItem aitem, bool remove = false,bool check = true);
    // Add list of attributes, overwrites parameters.  Overwrites only occur if
    // the types match.
    void add_attrs(const AttrList& attrs,bool use_new_values);
    
    const AttrList &attrs() const { return _attrs; };

    // Returns true if we have the specified attribute.
    virtual const AttrParams *has_attr(unsigned c) const;

    // Same as above, with parameter checking.
    virtual const AttrParams *has_attr(const AttrItem &ai) const;

    // This must be called b/c we expect the items to be sorted
    // for searching.
    void finalize();

    void dump(std::ostream &o) const;
    void dump(std::ostream &o,const gc_string &pfx) const;
  protected:

    AttrList _attrs;       // Any attributes: Set of integers corresponding to
                           // integers stored in Data.
    bool     _inherit;     // Whether this object will inherit attributes from a
                           // parent object (if applicable).
  };

  // A generic field:  Has a size in bits and a reset value.
  class GenericField : public Attrs, public Named {
  public:
    GenericField(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);

    unsigned bits() const { return _bits; };

    Opencxx::Ptree *resetvalue() const { return _resetvalue; };

    // True if reset is a numeric value.
    bool int_reset() const { return _int_reset; };

    virtual void finalize(MsgLog &);

    virtual void dump(std::ostream &,const gc_string &pfx) const;

  protected:
    friend bool operator==(const GenericField &,const GenericField &);

    friend class GenericFieldHandler;

    unsigned        _bits;
    Opencxx::Ptree *_resetvalue;
    bool            _int_reset;
  };


  // This stores the entire design database.  Note that architectures
  // do not exist in the data model- they are merged together to form
  // a single core.  These cores are then stored in _cores.
  class Data : public Base {
  public:
    struct Attr {      
      int              _id;
      AttrParams::Type _type; 
      Attr(int id, AttrParams::Type type):_id(id), _type(type){};   
    };
    typedef gc_map<gc_string,Attr> AttrHash;
    Data(Opencxx::Environment *env);

    // Lookup systems or cores- returns 0 if not found.
    const System *get_sys(const gc_string &name) const;
    const Core *get_core(const gc_string &name) const;
    Core *get_core(const gc_string &name);

    // Access the hash of cores.
    const CoreHash &cores() const { return _cores; };

    // Access the hash of systems.
    const SysHash &systems() const { return _systems; };

    // Access to helper C++ code.
    const PList &decls() const { return _decls; };

    // Add to the list of C++ helper code.
    void add_decl(Opencxx::Ptree *);

    // Note:  Classes are static so that all data base elements
    // may see a consistent mapping of class name -> integer handle.

    // Gain access to all classes.
    static const AttrHash &get_attrs();

    // Register a new attribute.
    static int add_attr(const gc_string &n,AttrParams::Type = AttrParams::NoParam);
  
    // Returns an attribute index.  This sets the type to the type specified and
    // detects type violations.  Throws a runtime_error if the attribute does
    // not exist if or is of the wrong type if require_valid_attr is true.
    static int get_attr(const gc_string &name, AttrParams::Type type,bool require_valid_attr = true);

    // Returns the id of an attribute with no type checking.  Returns -1 if
    // attribute does not exist.
    static int get_attr(const gc_string &name);

    // Linear search of attributes by integer value.
    static const char *get_attr_name(int s);

    // List attributes to the stream.
    static void dump_attrs(std::ostream &);
  
    // True if we are in strict mode.
    static bool strictMode() { return _strict; }
    // True if we should warn about redefinitions.
    static bool warnRedefineMode() { return _warn_redefine; }
    // True if we don't want to expand aspects.
    static bool origAction() { return _orig_action; }

    static void setStrict(bool s) { _strict = s; } 
    static void setWarnRedefine(bool w) { _warn_redefine = w; } 
    static void setOrigAction(bool o) { _orig_action = o; }

    // Final construction pass- does various checks and populates
    // various data structures.
    void finalize(MsgLog &);

    void dump(std::ostream &,const gc_string &pfx) const;
  protected:
    friend class AdlParser;
    friend class OuterHandler;

    // This stores helper C++ code at the outer-scope.
    PList _decls;

    // This stores information about all of the cores.
    // It hashes the objects by name.
    CoreHash _cores;

    // This stores information about all top-level systems,
    // hashed by name.
    SysHash _systems;

    // Specifies strict mode.
    static bool _strict;
    static bool _warn_redefine;
    static bool _orig_action;
  };

  class SysItem;

  // This map is used to store information about a type of shared resource.
  // The keys are the names of the shared items, e.g. register names, and the 
  // values the actual owners of the resource.
  typedef gc_map<gc_string,const SysItem *> ResItems;

  // Stores a collection of core resources.
  struct Resources {
    ResItems _regs;
    ResItems _regfiles;
    ResItems _mmu;
    ResItems _parms;
    ResItems _caches;
    ResItems _memories;
    ResItems _eventbuses;
    ResItems _extresources;

    // For this version, we check both _regs and _regfiles for a match.
    const SysItem *contains(const RegBase &) const;
    const SysItem *contains(const Reg &) const;
    const SysItem *contains(const RegFile &) const;
    const SysItem *contains(const MmuLookup &) const;
    const SysItem *contains(const Parm &) const;
    const SysItem *contains(const Cache &) const;
    const SysItem *contains(const Memory &) const;
    const SysItem *contains(const EventBus &) const;
    const SysItem *contains(const ExtResource &) const;

    // Updates 'this' with share information from x, such that if x contains a shared
    // item, it replaces the information in 'this'.  So, think of x as being a parent's
    // info and the parent info takes precedence.
    void update(const Resources &x);

    bool empty() const;
    void dump(std::ostream &,const gc_string &pfx) const;
  };

  // Base class for items that may appear in systems.
  class SysItem : public Aspects, public Named {
  public:
    SysItem (Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);

    // A type value (user-defined use).
    const gc_string &type() const { return _type; };

    // Returns a pointer to the register if it is contained within this item.
    virtual const Reg *get_reg(const gc_string &name) const = 0;
    // Returns a pointer to the register-file if it is contained within this item.
    virtual const RegFile *get_regfile(const gc_string &name) const = 0;
    // Returns a pointer to an MMU lookup object, if contained within this item.
    virtual const MmuLookup *get_mmulookup(const gc_string &name) const = 0;
    // Returns a pointer to a parameter, if contained within this item.
    virtual const Parm *get_parm(const gc_string &name) const = 0;
    // Returns a pointer to a cache, if contained within this item.
    virtual const Cache *get_cache(const gc_string &name) const = 0;
    // Returns a pointer to a memory, if contained within this item.
    virtual const Memory *get_memory(const gc_string &name) const = 0;
    // Returns a pointer to an eventbus, if contained within this item.
    virtual const EventBus *get_eventbus(const gc_string &name) const = 0;
    // Returns a pointer to an external resource, if contained within this item.
    virtual const ExtResource *get_extresource(const gc_string &name) const = 0;

    // Returns the number of constituent cores.
    virtual int numCores() const = 0;

    void add_shared_reg(const gc_string &name);
    void add_shared_regfile(const gc_string &name);
    void add_shared_mmu(const gc_string &name);
    void add_shared_parm(const gc_string &name);
    void add_shared_cache(const gc_string &name);
    void add_shared_memory(const gc_string &name);
    void add_shared_eventbus(const gc_string &name);
    void add_shared_extresource(const gc_string &name);

    virtual void add_shared_reg(const gc_string &name,const SysItem *owner);
    virtual void add_shared_regfile(const gc_string &name,const SysItem *owner);
    virtual void add_shared_mmu(const gc_string &name,const SysItem *owner);
    virtual void add_shared_parm(const gc_string &name,const SysItem *owner);  
    virtual void add_shared_cache(const gc_string &name,const SysItem *owner);
    virtual void add_shared_memory(const gc_string &name,const SysItem *owner);
    virtual void add_shared_eventbus(const gc_string &name,const SysItem *owner);
    virtual void add_shared_extresource(const gc_string &name,const SysItem *owner);

    // Access to shared resources.
    const Resources &shared() const { return _shared; };
    const ResItems &shared_regs() const { return _shared._regs; };
    const ResItems &shared_regfiles() const { return _shared._regfiles; };
    const ResItems &shared_mmulookups() const { return _shared._mmu; };
    const ResItems &shared_parms() const { return _shared._parms; };
    const ResItems &shared_caches() const { return _shared._caches; };
    const ResItems &shared_memories() const { return _shared._memories; };
    const ResItems &shared_eventbuses() const { return _shared._eventbuses; };
    const ResItems &shared_extresources() const { return _shared._extresources; };

    // Returns true if shared.
    const SysItem * is_shared(const Reg &) const;
    const SysItem * is_shared(const RegFile &) const;
    const SysItem * is_shared(const RegBase &) const;
    const SysItem * is_shared(const MmuLookup &) const;
    const SysItem * is_shared(const Parm &) const;
    const SysItem * is_shared(const Cache &) const;
    const SysItem * is_shared(const Memory &) const;
    const SysItem * is_shared(const EventBus &) const;
    const SysItem * is_shared(const ExtResource &) const;

    virtual void check_child_indexing(MsgLog &ml,unsigned child_id) = 0;
  protected:

    gc_string     _type;

    // Shared resources.
    Resources       _shared;
  };

  // This stores information about a collection of cores or systems.
  class System : public SysItem {
  public:
    System (Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);

    virtual const Reg *get_reg(const gc_string &name) const;
    virtual const RegFile *get_regfile(const gc_string &name) const;
    virtual const MmuLookup *get_mmulookup(const gc_string &name) const;
    virtual const Parm *get_parm(const gc_string &name) const;
    virtual const Cache *get_cache(const gc_string &name) const;
    virtual const Memory* get_memory(const gc_string &name) const;
    virtual const EventBus* get_eventbus(const gc_string &name) const;
    virtual const ExtResource* get_extresource(const gc_string &name) const;

    virtual int numCores() const;

    using SysItem::add_shared_reg;
    using SysItem::add_shared_regfile;
    using SysItem::add_shared_mmu;
    using SysItem::add_shared_parm;
    using SysItem::add_shared_cache;
    using SysItem::add_shared_memory;
    using SysItem::add_shared_eventbus;
    using SysItem::add_shared_extresource;

    void add_shared_reg(const gc_string &name,const SysItem *owner);
    void add_shared_regfile(const gc_string &name,const SysItem *owner);
    void add_shared_mmu(const gc_string &name,const SysItem *owner);
    void add_shared_parm(const gc_string &name,const SysItem *owner);
    void add_shared_cache(const gc_string &name,const SysItem *owner);
    void add_shared_memory(const gc_string &name,const SysItem *owner);
    void add_shared_eventbus(const gc_string &name,const SysItem *owner);
    void add_shared_extresource(const gc_string &name,const SysItem *owner);

    // Access to locally-defined systems/cores
    const SysHash &local_systems() const { return _local_systems; };
    const CoreHash &local_cores() const { return _local_cores; };

    // Find a system or core.  These look up items by their type.
    const System *get_sys(const gc_string &name) const;
    const Core   *get_core(const gc_string &name) const;

    // Access to constituent items.
    typedef gc_list<std::pair<gc_string,SysItem *> > Items;
    const Items &items() const { return _items; };

    // Looks up an item by its declaration name.
    const SysItem *get_item(const gc_string &name) const;
    // These are variants of the above- they do a dynamic cast to make sure that
    // the type is what you expect.
    const System  *get_sys_item(const gc_string &name) const;
    const Core    *get_core_item(const gc_string &name) const;

    // Returns true if item is locally defined.
    bool check_is_local(SysItem &si) const;

    void finalize(MsgLog &ml);
    void dump(std::ostream &,const gc_string &pfx) const;
  protected:
    friend class SysHandler;

    virtual void check_child_indexing(MsgLog &ml,unsigned child_id);

    template <class T>
    void check_shared_object(const gc_string &name,const gc_string &descr,
                             const T * (SysItem::*get)(const gc_string &) const) const;

    // Locally defined systems and cores
    SysHash   _local_systems;
    CoreHash  _local_cores;
    // Items in the system.
    Items      _items;

    Opencxx::Ptree *_share_loc;
  };

  // Stores information about an address mask.
  class AddrMask : public Attrs, public Named {
  public:
    AddrMask (Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);

    bool   is_const() const { return _constant; };
    addr_t initial_value() const { return _initial; };
    Opencxx::Ptree *watch() const { return _watch; };

    void clear();

    void finalize(Core &core);
    void dump(std::ostream &o,const gc_string &pfx) const;

  protected:
    friend class AddrMaskHandler;
    friend class ArchHandler;

    bool            _constant;
    addr_t          _initial;
    Opencxx::Ptree *_watch;
  };
       
  // Stores information about assembler configuration
  class AsmConfig : public Base {
  public:
    typedef gc_list< std::pair<gc_string,gc_string> > StrPairList;
    AsmConfig(Opencxx::Environment *env,Opencxx::Ptree *loc);    
    
    void finalize(Core &core, MsgLog &);   
    void dump(std::ostream &o,const gc_string &pfx) const;
    
    unsigned queue_size() const { return _queue_size; };
    unsigned queue_offset() const { return _queue_offset; };
    
    const StrList &comments() const  { return _comments; };
    const StrList &line_comments() const { return _line_comments; };
    const StrList &line_seps() const { return _line_seps; };
    const StrList &instr_seps() const { return _instr_seps; };
    const StrPairList &group_seps() const { return _group_seps; };
    const StrList all_group_seps() const;
    const StrList  &asm_instrtables() const { return _asm_instrtables; };
    const StrList  &disasm_instrtables() const { return _disasm_instrtables; };
    const AsmParmHash &parms() const { return _parms; };
    const AsmRuleHash &rules() const { return _rules; };
    bool  explicit_regs() const { return _explicit_regs; };
    bool  explicit_operand_regex_end() const { return _explicit_operand_regex_end; };
    const StrList  symbol_chars() const { return _symbol_chars; };
    const StrList  operand_symbols_excluded() const { return _operand_symbols_excluded; };

  protected:
    friend class AsmConfigHandler;

    void checkInstrTable(Core &core,MsgLog &ml,const StrList &instrtable,const char *msg);
    
    bool        _default;   
    unsigned    _queue_size;
    unsigned    _queue_offset;
    StrList     _comments;
    StrList     _line_comments;
    StrList     _line_seps;
    StrList     _instr_seps;
    StrPairList _group_seps;
    StrList     _asm_instrtables;
    StrList     _disasm_instrtables;
    AsmParmHash _parms;
    AsmRuleHash _rules;
    bool        _explicit_regs;
    bool        _explicit_operand_regex_end;
    StrList     _symbol_chars;
    StrList     _operand_symbols_excluded;
  };

  // Store assembler specific parameters
  class AsmParm : public Base, public Named {
  public:

    enum Type { NoType, Integer, Label };


    AsmParm(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);
    void finalize(const Core &core, MsgLog &);
    void dump (std::ostream &o, const gc_string &pfx) const;

    Type    type() const { return _type; };
    uint64_t value() const { return _value; };
    bool     is_const() const { return _constant; };
    
  protected:
    friend class AsmParmHandler;
    friend class AsmConfigHandler;

    uint64_t _value;
    bool     _constant;
    Type     _type;
  };

  // Store assembler rules that need to be checked.
  class AsmRule : public Base, public Named {
  public:
    enum Type { NoType, Instr, Packet };

    AsmRule(Opencxx::Environment *env,Opencxx::Ptree *loc, const gc_string &name); 

    void finalize(Core &core, MsgLog &);
    void dump(std::ostream &o,const gc_string &pfx) const;
    
    Type type() const { return _type; };
    bool dest() const { return _dest; };
    Opencxx::Ptree *action() const { return _action; }
    
  protected:
    friend class AsmRuleHandler;
    friend class AsmConfigHandler;

    Type _type;
    bool _dest; // does rule access UsageInfo
    Opencxx::Ptree *_action;
  };


  // This stores information on a single core.
  class Core : public SysItem {
  public:
    
    enum  Endianness {Big, Little} ;
    
    Core (Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);

    // Access to helper functions.
    const HelperMap &helpers() const { return _helpers; };

    // Access to other type declarations.
    const PList &type_decls() const { return _type_decls; };

    // Access to auxiliary functions.
    const AuxFuncList &aux_funcs() const { return _aux_funcs; };

    // These functions allow an external user to add helpers and type
    // declarations.  These should only be called after the initial parsing of
    // the front-end.  Once all new helpers have been added, call
    // check_helpers() to make sure that they're properly translated and
    // checked.
    void add_helper(const gc_string &name,Opencxx::Ptree *func);
    void add_type_decl(Opencxx::Ptree *decl);

    // Add an auxiliary function, which allows external users to insert code
    // into the main class.
    AuxFunc *add_aux_func(Opencxx::Ptree *func,const gc_string &rtype,
                          const gc_string &comment,bool isConst = false);

    // Delete an existing auxiliary function.  Returns true if deleted, else false.
    bool del_aux_func(const AuxFunc *);

    // These return null if the object is not found.
    const RegBase *get_regitem(const gc_string &name) const;
    const RegFile *get_regfile(const gc_string &name) const;
    const Reg *get_reg(const gc_string &name) const;
    const Parm *get_parm(const gc_string &name) const;
    const Context *get_ctx(const gc_string &name) const;
    const Relocation *get_reloc(const gc_string &name) const;
    const PrefixCounter *get_pfx_ctr(const gc_string &name) const;
    const InstrField *get_instrfield(const gc_string &name) const;
    const SubInstr *get_subinstr(const gc_string &name) const;
    const Instr *get_instr(const gc_string &name) const;
    const Exception *get_exception(const gc_string &name) const;
    const MmuLookup *get_mmulookup(const gc_string &name) const;
    const Cache *get_cache(const gc_string &name) const;
    const Memory* get_memory(const gc_string &name) const;
    const InstrTable* get_instrtable(const gc_string & name) const; //
    const EventBus* get_eventbus(const gc_string &name) const;
    const ExtResource* get_extresource(const gc_string &name) const;
    HelperInfo *get_helper(const gc_string &name) const;

    bool  is_block(const gc_string &name) const;
    bool  is_prefix_info(const gc_string &name) const;

    bool immediate_writehooks() const { return _immediate_writehooks == ImmTrue; };

    // Access to the various data structures.
    const ContextHash &contexts() const { return _contexts; };
    const ParmHash &parms() const { return _parms; };
    const RegHash &regs() const { return _regs; };
    const RegFileHash &regfiles() const { return _regfiles; };
    const InstrFieldHash &instrfields() const { return _instrfields; };
    const PrefixCounterHash &prefix_counters() const { return _prefix_counters; };
    const SubInstrHash &subinstrs() const { return _subinstrs; };
    const InstrHash &instrs() const { return _instrs; };
    const InstrTables &instrtables() const { return _instrtables; };
    const ExceptionHash &exceptions() const { return _exceptions; };
    const CacheHash &caches() const { return _caches; };
    const MMU *mmu() const { return _mmu; };
    const MemoryHash &memories() const { return _memories; };
    const EventBusHash &eventbuses() const { return _eventbuses; };
    const ExtResourceHash &extresources() const { return _extresources; };
    const GroupHash &groups() const { return _groups; };
    const RelocationHash &relocs() const { return _relocs; } 
    const Blocks &blocks() const { return _blocks; };
    const InstrTableHash &instr_tables() const { return _instr_tables; }; //

    // Returns the relevant context object if the resource is part of a context,
    // or 0 if not a context resource.
    const Context *is_ctx_resource(const Reg &reg) const;
    const Context *is_ctx_resource(const RegFile &rf) const;
    const Context *is_ctx_resource(const RegBase &rf) const;
    const Context *is_ctx_resource(const MmuLookup &ml) const;

    // Various cache info methods.
    bool has_caches() const;
    bool has_instr_caches() const;
    bool has_data_caches() const;
    // For this method, UnifiedCache is not a valid value.
    bool has_caches(CacheType) const;

    unsigned max_cache_level() const;
    const Cache *get_cache(CacheType type,unsigned level) const;
    const Cache *get_first_cache(CacheType type) const;
    const Cache *get_last_cache(CacheType type) const;
    const Caches &instr_caches() const { return _instr_caches; };
    const Caches &data_caches() const { return _data_caches; };

    // If true, we consider exceptions to be branches, in terms of logging and
    // the branch-taken hook.
    bool exception_is_branch() const { return _exception_is_branch; };

    // True if we have a local memory for fetching instructions.
    bool has_instr_memory() const;
   
    // True if we have a local prefetch memory for pre-fetching instructions
    bool has_prefetch_buffer() const;

    // True if we call setMicroOpMask().
    bool has_micro_op_offset() const { return _has_micro_op_offset; };

    // True if we have any instruction fields which map to instruction tables
    // (are micro operations).
    bool has_micro_ops() const { return _max_micro_ops; };
    unsigned max_micro_ops() const { return _max_micro_ops; };

    // Maximum number of instruction's bits originating from prefix. Default 0
    unsigned prefix_bits() const { return _max_prefix_bits; };

    const Reg *cia() const { return _cia; };
    const Reg *nia() const { return _nia; };

    // Any registers used with delayed_write().
    const StrSet &delayed_writes() const { return _delayed_writes; };
    bool has_delayed_writes() const { return !_delayed_writes.empty(); };
    // Note: Only correct if all delayed writes can be statically determined.
    // The actual value used when generating a simulator may have to be set by
    // using a config variable.
    unsigned max_write_delay() const { return _max_write_delay; };

    void add_delayed(const gc_string &d,Opencxx::Ptree *dexpr);


    // Any concurrent registers, using ValueBundle in write hook
    const StrSet &concurrent_writes() const { return _concurrent_writes; };
    bool has_concurrent_writes() const { return !_concurrent_writes.empty(); };
    
    void add_concurrent(const gc_string &c);
    
    // Access to helper sets.
    const StrSet &instr_helpers() const { return _instr_helpers; };
    const StrSet &non_instr_helpers() const { return _non_instr_helpers; };
    const StrSet &asm_helpers() const { return _asm_helpers; };
    const StrSet &disasm_helpers() const { return _disasm_helpers; };
    const StrSet &used_helpers() const { return _used_helpers;};
   
    // Contains any used registers or register files.
    const UsedRegs &used_regs() const { return _used_regs; };

    // Access to a list (begin, end pointers) to the miscellaneous core hooks.
    std::pair<const HelperInfo *,const HelperInfo *> misc_funcs() const { 
      return std::make_pair(_misc_funcs_begin,_misc_funcs_end); 
    };

    // Address-check handler.
    const HelperInfo &addr_check_handler() const { return _addr_check_handler; };

    // Pre-exception handler.
    const HelperInfo &pre_exception_handler() const { return _pre_exception_handler; };

    // Post-exception handler.
    const HelperInfo &post_exception_handler() const { return _post_exception_handler; };

    // Branch-taken (change-in-control) handler.
    const HelperInfo &branch_taken_handler() const { return _branch_taken_handler; };

    // Decode miss handler.
    const HelperInfo &decode_miss_handler() const { return _decode_miss_handler; };

    // Decode retry handler.
    const HelperInfo &decode_retry_handler() const { return _decode_retry_handler; };

    // Pre-cycle handler.
    const HelperInfo &pre_cycle_handler() const { return _pre_cycle_handler; };

    // Post-cycle handler.
    const HelperInfo &post_cycle_handler() const { return _post_cycle_handler; };

    // Pre-fetch handler.
    const HelperInfo &pre_fetch_handler() const { return _pre_fetch_handler; };

    // Post-fetch handler.
    const HelperInfo &post_fetch_handler() const { return _post_fetch_handler; };

    // Post-exec handler.
    const HelperInfo &post_exec_handler() const { return _post_exec_handler; };
    
    // Pre-asm handler.
    const HelperInfo &pre_asm_handler() const { return _pre_asm_handler; };

    // Post-asm handler.
    const HelperInfo &post_asm_handler() const { return _post_asm_handler; };

    // Post-asm handler.
    const HelperInfo &post_packet_asm_handler() const { return _post_packet_asm_handler; };

    // Post-packet handler.
    const HelperInfo &post_packet_handler() const { return _post_packet_handler; };

    // Post-packet-commit handler (called after packet is committed).
    const HelperInfo &post_packet_commit_handler() const { return _post_packet_commit_handler; };

    // Only valid for sparse registers: Reads/writes of invalid indices.  These
    // are global defaults that are used if no per-register-file function
    // exists.
    const HelperInfo &invalid_entry_read() const { return _invalid_entry_read; };
    const HelperInfo &invalid_entry_write() const { return _invalid_entry_write; };

    // Active/halted watch function.
    Opencxx::Ptree *active_watch() const { return _active_watch; };

    // Current-instruction-table watch function.
    Opencxx::Ptree *itable_watch() const { return _itable_watch; };

    // Address mask objects.
    const AddrMask &ra_mask() const { return _ra_mask; };
    const AddrMask &ea_mask() const { return _ea_mask; };

    const AsmConfig &asm_config() const { return _asmcfg;};
    
    // Parallel execution - if larger than 0, packet size
    uint parallel_execution() const { return _parallel_execution;}

    // Packetized encodings- multiple instructions are assembled in parallel
    // into a larger instruction or packet.
    bool packetized_encoding() const { return _packetized_encoding; };

    // Packet position increment amont.  0 means to advance by 1 per instruction, otherwise
    // value is fetch_width / _packet_pos_incr  (all values in bits).
    unsigned packet_pos_incr() const { return _packet_pos_incr; };
    
    // True if has prefix instructions. 
    bool has_prefix_instruction() const;

    // True if skip_instr is defined in the post_fetch hook
    bool post_fetch_skip() const { return _post_fetch_skip; };

    // Used for width of instruction table decoding masks
    unsigned instr_width() const { return _max_instr_width;};

    // Maximum of return-type nested fields - used by transactional ISS
    unsigned num_ret_fields() const { return _num_ret_fields; };

    // Returns true if it is an  encoding-less (ASCII)  flow
    bool  need_encoding() const;

    // Call this to add a local resource- a name that's allowed in functions
    // that doesn't correspond to other types of known resources.  This is often
    // called so that aux funcs can know about special types that are added
    // externally.
    void add_local_resource(const gc_string &name);

    // Returns true if the parameter is a recognized resource
    // in the design.
    bool is_resource(const gc_string &name) const;

    // Returns true and set val, if it is a register/register fiel enumerated value
    // return false, otherwise
    bool get_regbase_enum(const gc_string &name, int &val) const;

    // For index numbering.
    Endianness bitEndianness() const { return _bitEndianness;};
    bool is_little_endian() const { return _bitEndianness == Core::Little;};
    bool is_big_endian() const { return _bitEndianness == Core::Big;};
    bool is_mixed_endian() const { return instrEndianness() == Core::Big && is_little_endian(); };

    // For instruction fetches.
    Endianness instrEndianness() const { return _instrEndianness;};

    // For data accesses.
    Endianness dataEndianness() const { return _dataEndianness;};

    virtual int numCores() const { return 1; };

    // Call this function if extra helpers have been added and need to be
    // checked.
    void check_helpers(MsgLog &ml);

    // Call this function is auxiliary functions were added.  This finalizes and
    // checks all such functions.
    void check_aux_funcs(MsgLog &ml);

    // Call this to check an arbitrary function supplied by the user.  Errors
    // are reported by throwing a parse_error exception.  Helper usage is not
    // recorded, but the usage info is returned for the function in 'ui'.  The
    // translated function is returned.
    Opencxx::Ptree *check_func(UsageInfo &ui,Opencxx::Ptree *func) const;

    // For internal use and use by other front-end functions.  External users
    // shouldn't call this directly.
    void process_helper(HelperInfo *result,HelperInfo &src,const gc_string &name,
                        Opencxx::PtreeArray *args,const InstrBase *instr,const StrSet &excepts,
                        bool top_level);

    // Used internally- toggle on or off whether we're processing MMU facilities which could
    // affect address translation.  Mainly used for attribute recording.
    bool set_mmu_handling(bool h);

    // Query for the set of attributes used by instruction-attribute queries (in
    // general and specifically within the MMU, where the attributes could
    // affect translations).
    const IntSet &instr_attrs() const { return _instr_attrs; };
    const IntSet &mmu_instr_attrs() const { return _mmu_instr_attrs; };
    const IntSet &regfile_attrs() const { return _regfile_attrs; };

    // Default reserved bits value for the core.
    uint8_t reserved_bits_value() const { return _reserved_bits_value; };

    // For internal use- we record what attributes are used by instruction-attribute queries.
    void add_instr_attr(const gc_string &attr);
    void add_instr_attr(unsigned attr);

    void set_has_micro_op_offset();

    void add_used_reg(const RegBase *r);

    // Same for register-file attributes.
    void add_regfile_attr(const gc_string &attr);
    void add_regfile_attr(unsigned attr);

    void add_asm_helpers(const StrSet &helpers);
    void add_disasm_helper(const gc_string &helper);

    // For internal use- toggle whether we need typed and/or critical word memory.
    enum MemCallTypes { BasicMem = 1, TypedMem = 2, CritWordMem = 4 };
    void set_typed_mem(MemCallTypes t) { _mem_types = (_mem_types | t); };
    int mem_types() const { return _mem_types; };
    bool mem_type_check(MemCallTypes t) const { return _mem_types & t; };
    bool typed_mem() const { return _mem_types & TypedMem; };
    bool crit_word_mem() const { return _mem_types & CritWordMem; };
    void updateInstrFieldReloc(const gc_string &ifname,const Relocation *reloc);
    void set_used_field(const InstrField &f);

    // For internal use- used by VarWalker for setting up a chain of
    // register/register-file read/write accesses.  This way, we can then go
    // back after finalization and understand usage throughout the core.
    void pushRegAccess(PtreeRegRead *p);
   
    const RegField *is_regfield_helper(const gc_string &name) const;

    // Final construction pass- does various checks and populates
    // various data structures.
    void finalize(MsgLog &ml);

    void dump(std::ostream &,const gc_string &pfx) const;
    
    // Check and set the flag to show whether CIA_commit_write() is available in the ISS model or not
    bool is_CIA_commit_write() const { return _is_CIA_commit_write; }
    void set_CIA_commit_write() { _is_CIA_commit_write = true; }

    struct MicroOpInfo {
      gc_string _table_name;
      int       _id;
    };
    typedef gc_map<gc_string,MicroOpInfo> FieldMicroOpMap;

    // Fetch info about all micro-op fields in the core.
    FieldMicroOpMap getMicroOpFields() const;
    
  protected:
    friend class CoreHandler;
    friend class ArchHandler;
    friend class ClassHandler;
    friend class GroupHandler;
    friend class RegHandler;
    friend class RegFileHandler;
    friend class RegHookHandler;
    friend class RegFileHookHandler;
    friend class Memory;
    friend class CheckCode;

    // These return null if the object is not found
    Context *get_ctx(const gc_string &name);
    Parm *get_parm(const gc_string &name);
    RegBase *get_regitem(const gc_string &name);
    RegFile *get_regfile(const gc_string &name);
    Reg *get_reg(const gc_string &name);
    InstrField *get_instrfield(const gc_string &name);
    Relocation *get_reloc(const gc_string &name);
    PrefixCounter *get_pfx_ctr(const gc_string &name);
    SubInstr *get_subinstr(const gc_string &name);
    Instr *get_instr(const gc_string &name);
    Exception *get_exception(const gc_string &name);
    Cache *get_cache(const gc_string &name);
    Memory *get_memory(const gc_string &name);
    InstrTable * get_instrtable(const gc_string & name); //
    EventBus *get_eventbus(const gc_string &name);
    ExtResource *get_extresource(const gc_string &name);

    void  encode_instrs(MsgLog &ml);
    void updateRegUsage();
    void updateRegUsage(RegAccess &);
    void checkRelocationUniqueness(MsgLog &ml);
    
    // Return true if we have any instruction fields which are tied to
    // instruction tables (what we call micro-instructions).
    bool hasMicroInstrs() const;
    
    // Add an instruction table, given a class name and a position within
    // the list of instruction tables (0 is first).
    // Returns false if there's a problem, such as that the
    // name doesn't exist.  If the table already exists, no
    // insertion is done, but true is still returned.
    bool add_instr_table(const gc_string &name, unsigned pos, unsigned width); // Add one parameter as width

    virtual void check_child_indexing(MsgLog &ml,unsigned child_id);

    void pc_setup();

    Cache *get_cache(Caches &caches,unsigned level);
    Cache *get_first_cache(Caches &caches);
    Cache *get_last_cache(Caches &caches);

    Cache *get_cache(CacheType type,unsigned level);
    Cache *get_first_cache(CacheType t);
    Cache *get_last_cache(CacheType t);

    void add_cache(Caches &caches,Cache *c);
    void add_cache(Cache *c);
    void finalize_instrs(MsgLog &);
    void finalize_caches(MsgLog &);
    void postprocess_instrfields();
    void check_cache_hierarchy(MsgLog &ml,Caches &caches);
    void dump_caches(std::ostream &o,gc_string pfx) const;

    void finalize_tmpfields(MsgLog &,InstrHash *generated,bool pseudo);
    void check_prefix();
    void check_assembler_fields();
    void calc_prefix_bits();
    void calc_ret_fields();
    void build_instrtables(MsgLog &);

    void create_asm_excepts(StrSet &excepts,bool pre,bool post);

    void tagMicroOps();

    // Storage of helper functions.  This is only mutable because we need to
    // be able to recursively update usage info.
    mutable HelperMap     _helpers;

    // This maps helper+args -> usage-info,translated-code.  This avoids
    // repeated translation of the same function if it has the same arguments.
    mutable HelperArgsMap _translated_helpers;

    // This is a set of local resources- names which are allowed in helper/aux
    // funcs that are explicitly added by an external user.
    StrSet _local_resources;

    // Storage of other user type declarations.
    PList _type_decls;

    // Auxiliary functions- used by external users to add code to the main class.
    AuxFuncList _aux_funcs;

    // Stores any contexts.
    ContextHash _contexts;

    // Stores architectural parameters.
    ParmHash _parms;

    // Stores all parameter options by name, indexed
    // to parent parameter.
    gc_map<gc_string,Parm*> _alloptions;

    // Pointer to the start of a list of miscellaneous helper info objects.
    HelperInfo *_misc_funcs_begin;
    HelperInfo *_misc_funcs_end;

    // Address-check handler.
    HelperInfo _addr_check_handler;

    // Pre-exception handler.
    HelperInfo _pre_exception_handler;

    // Post-exception handler.
    HelperInfo _post_exception_handler;

    // Branch-taken (change-in-control) handler.
    HelperInfo _branch_taken_handler;

    // Decode-miss handler code.
    HelperInfo _decode_miss_handler;

    // Decode-retry handler
    HelperInfo _decode_retry_handler;

    // Called after before fetch.
    // Called each cycle for a performance model, or before fetch for an ISS.
    HelperInfo _pre_cycle_handler;

    // Called after post_exec for a performance model, or after post_exec for an ISS.
    HelperInfo _post_cycle_handler;

    // Called before fetch.
    HelperInfo _pre_fetch_handler;

    // Called after fetch, but before instruction execution.
    HelperInfo _post_fetch_handler;

    // Called after execution.
    HelperInfo _post_exec_handler;

    // Called after a packet execution. In parallel execution
    HelperInfo _post_packet_handler;

    // Called after a packet has executed and been committed, so that the
    // architectural state has been updated.
    HelperInfo _post_packet_commit_handler;

    // Assembler hooks
    // Called before assembling every instruction. 
    HelperInfo _pre_asm_handler;

    // Called after assembling every instruction. 
    HelperInfo _post_asm_handler;

    // Called after assembling a packet of instructions - in parallel architecture
    HelperInfo _post_packet_asm_handler;

    // Dummy helper (always empty), used to define the list of miscellaneous
    // helper objects.
    HelperInfo _end_handler;

    // Global default invalid handlers for sparse register-file read/write
    // operations.
    HelperInfo _invalid_entry_read;
    HelperInfo _invalid_entry_write;

    // Watch expression for defining core's active state.
    Opencxx::Ptree *_active_watch;

    Opencxx::Ptree *_itable_watch;

    // Address mask objects.
    AddrMask _ea_mask;
    AddrMask _ra_mask;

    WriteHookImm _immediate_writehooks;

    // All register files.
    RegFileHash _regfiles;

    // All of the registers in the architecture.
    RegHash _regs;

    // Current and next instruction address registers.
    const Reg *_cia;
    const Reg *_nia;

    // Prefix-counters are used for selecting bits from prefix instructions to
    // extend an instruction field.  They're used with indexed instruction
    // fields.
    PrefixCounterHash _prefix_counters;

    // The instruction fields.
    InstrFieldHash _instrfields;
	
    // The instrucion tables
    InstrTableHash _instr_tables;

    // The relocations
    RelocationHash _relocs;

    // Ths subinstructions
    SubInstrHash _subinstrs;

    // The instructions.
    InstrHash _instrs;

    // The exceptions.
    ExceptionHash _exceptions;

    // The various instruction tables.
    InstrTables _instrtables;

    // The MMU object, if one is defined.
    MMU *_mmu;

    // All caches, indexed by name.
    CacheHash _caches;

    // Memories 
    MemoryHash _memories;

    // If true, we consider exceptions to be branches, in terms of logging and
    // the branch-taken hook.
    bool      _exception_is_branch;
    
    bool      _has_instr_memory;
    
    // A prefecth memory is defined.
    bool      _has_prefetch_memory;

    bool      _has_micro_op_offset;
    unsigned  _max_micro_ops;

    
    // Parallel execution. if nonzero, value indicates packet size
    uint    _parallel_execution;

    // True if we encode multiple instructions into a packet or larger
    // instruction.
    bool    _packetized_encoding;

    // Amount by which to advance packet position.  0 => 1 for each instruction, else
    // fetch_width / _packet_pos_incr.
    uint    _packet_pos_incr;
  
    // Caches.  Indices correspond to cache levels, where L1 is stored in
    // index 0.  Note that these arrays can have holes in them, e.g. an L2
    // may exist but no L1 exists.
    Caches _instr_caches;
    Caches _data_caches;

    // Event buses.
    EventBusHash _eventbuses;

    // Opaque resources
    ExtResourceHash _extresources;
    
    // Groups
    GroupHash _groups;
    
    // Needed to build instruction table
    unsigned _max_instr_width;

    // Max number of instruction bits originating from prefix
    unsigned _max_prefix_bits;

    // max return-type nested fields 
    int  _num_ret_fields;

    // Whether the user ever uses typed memory accesses (a type specifier, in
    // addition to address and size information).
    int _mem_types;

    StrSet _prefix_excepts;

    // system blocks
    Blocks _blocks;
    
    Endianness _bitEndianness;
    Endianness _instrEndianness;
    Endianness _dataEndianness;

    AsmConfig _asmcfg;

    // Lists helpers used by non-instructions and by instructions and by the
    // assembler and disassembler, and any helpers which have been found to be
    // used by the system at all.
    StrSet _instr_helpers, _non_instr_helpers, _asm_helpers, _disasm_helpers, _used_helpers;
   
    // Any register/register-files used by the system.
    UsedRegs _used_regs;

    // Any registers which are used with delayed_write().
    StrSet _delayed_writes;

    // Max delayed-write value that we can statically determine.
    unsigned _max_write_delay;

    // Any register which has concurrent_write, i.e., ValueBundle in write hook
    StrSet _concurrent_writes;

    // True if skip_instruction is used in post_fetch_hook
    bool  _post_fetch_skip;

    // Toggles whether we're processing MMU action code which can affect address
    // translations.
    bool _mmu_handling;

    // Records what attributes are used by instructions (in general and
    // specifically for the MMU).
    IntSet _instr_attrs;
    IntSet _mmu_instr_attrs;  

    // And what attributes are used by register-files.
    IntSet _regfile_attrs;

    // Core-level preferred reserved-bits value.
    uint8_t _reserved_bits_value;
    
    // Flag to show whether _commit_write() is available in the ISS model or not
    bool _is_CIA_commit_write;

    // Used during finalization to build a list of all register accesses.  We
    // then scan this to understand what registers were used and how.
    PtreeRegRead *_reg_accesses;
  };

  // This stores information about a context, which allows resources to be organized into
  // banks of instances.
  class Context : public Attrs, public Resources, public Named {
  public:
    Context (Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);

    unsigned id() const { return _id; };

    void add_reg(const gc_string &,const Core *c);
    void add_regfile(const gc_string &,const Core *c);
    void add_mmulookup(const gc_string &,const Core *c);
    void add_mem_layout_item(const gc_string &);
    void add_mem_omit_item(const gc_string &);

    // We consider a context to be a thread if it contains the program counter
    // (which we refer to in ADL as the CIA (current instruction address)
    // register.
    bool is_thread() const { return _is_thread; };

    int num_contexts() const { return _num_contexts; };

    const StrList &mem_layout() const { return _mem_layout; };
    const StrSet  &mem_omit() const { return _mem_omit; };

    Opencxx::Ptree *active() const { return _active; };

    void finalize(Core &core,MsgLog &ml,unsigned id);
    void dump(std::ostream &o,const gc_string &pfx) const;

  private:
    friend class ArchHandler;
    friend class ContextHandler;
  
    template <class T>
    void check_items(MsgLog &ml,const ResItems &items,const gc_string &descr,const Core &core,
                     const T * (Core::*get)(const gc_string &) const) const;

    unsigned        _id;
    int             _num_contexts;
    bool            _is_thread;
    StrList         _mem_layout;
    StrSet          _mem_omit;

    Opencxx::Ptree *_active;
  };

  // Used to identify a context instance.
  struct CtxInstance {
    CtxInstance() : _context(0), _index(-1) {};
    CtxInstance(const Context *c,int i) : _context(c), _index(i) {};

    operator bool() const { return _context; };

    const Context *ctx() const { return _context; };
    int index() const { return _index; };

    const Context *_context;
    int            _index;
  };

  // This stores information about an architectural parameter.
  class Parm : public Attrs, public Named {
  public:
    Parm (Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);

    // Possible options.
    const StrSet &options() const { return _options; };
    // Default value.
    const gc_string &value() const { return _value; };
    // Whether it has a value or not.
    bool has_value() const { return !_value.empty(); };
    // Whether it's constant.
    bool is_const() const { return _constant; };
    // The (optional) watch expression.
    Opencxx::Ptree *watch() const { return _watch; };

    // Returns true if this is a boolean parameter, i.e. it has two options of "true" and "false".
    bool is_bool() const;

    // This equivalence is defined in terms of what's needed to be able to share a
    // parameter.  We require that the options and const-ness be the same.
    bool operator==(const Parm &) const ;

    void finalize(Core &core,MsgLog &ml);
    void dump(std::ostream &o,const gc_string &pfx) const;

  protected:
    friend class ParmHandler;
    friend class ArchHandler;

    StrSet          _options;  // Valid options for the parm.
    gc_string     _value;    // Current value.
    bool            _constant; // Whether it's constant.
    Opencxx::Ptree *_watch;    // Monitor/update function.
  };

  // This stores information common to both registers and register files.
  class RegBase : public Attrs, public Named {
  public:
    RegBase (Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);

    unsigned id() const { return _id; };
    unsigned width() const { return _width; };
    unsigned offset() const { return _offset; };
    bool pseudo() const { return _pseudo; };

    const gc_string &log_name() const { return _log_name; };

    const RegFieldHash &fields() const { return _fields; };

    bool has_fields() const { return !_fields.empty(); };
    const RegField *get_field(const gc_string &) const;

    const BitVect &reserved_mask() const { return _reserved; };
    const BitVect &readonly_mask() const { return _readonly; };

    bool has_reserved_bits() const { return !_reserved.is_reset(); };
    bool has_readonly_bits() const { return !_readonly.is_reset(); };

    enum ResetType { None, Value, Action, Enum };
    ResetType resettype() const { return _resettype; };
    Opencxx::Ptree *resetvalue() const { return _resetvalue; };
    Opencxx::Ptree *resetaction() const { return _resetaction; };

    Opencxx::Ptree *external_write_hook() const { return _external_write; };

    // Hooks are unique to derived classes, but it's useful to have a generic way
    // to find out if the register is "special".
    virtual bool has_readhook() const = 0;
    virtual bool has_writehook() const = 0;
    virtual bool has_immediate_writehook() const = 0;

    // Returns true if the item has a write-hook which takes the extra
    // start/stop parameters.
    virtual bool has_start_stop() const = 0;

    // Returns true if a write-hook has action code.
    virtual bool has_hook_action() const = 0;

    // Serial registers are update immediately even during  parallel execution.
    bool is_serial() const { return _serial; }; 

    bool get_enumerated(const gc_string &s, int &v) const;
    bool is_enumerated() const { return !_enums.empty(); }
    bool is_enumerated(const gc_string &s) const;
    const StrIntList &enums() const { return _enums; };
   
    // True if it is a branch target
    bool branch_target(const Core& core) const; 

    // Various checks about whether a register may be shared.
    virtual void check_sharing_okay() const;

    // This equivalence is defined in terms of what's needed to be able to
    // share a register.
    bool operator==(const RegBase &) const ;

    // Query routines about register usage.
    bool is_reg_used() const;
    bool is_reg_read() const;
    bool is_reg_written() const;
    bool is_reg_slice_read() const;
    bool is_reg_slice_written() const;
    bool is_reg_field_read() const;
    bool is_reg_field_written() const;

    // Used internally.
    void set_reg_usage(unsigned f) const { _usage_flags |= f; };

    virtual void finalize(Core &core,unsigned id,bool has_writehook);

    void dump(std::ostream &,const gc_string &pfx) const;

  protected:
    friend class ArchHandler;
    friend class RegBaseHandler;
    friend class RegFileHandler;
    friend class RegHandler;
    friend class HookHandler;

    void set_default_width();

    unsigned    _id;

    gc_string _log_name;

    // Bit width.
    unsigned _width;

    // Bit offset.
    unsigned _offset;

    // Hash of register fields, indexed by name.
    RegFieldHash _fields;

    // Read/write hook/alias information.
    bool     _pseudo;

    // Serial register, updated immediately even during parallel execution
    bool  _serial;

    // If enumerated, list of enumeration values.
    StrIntList     _enums;      

    // Reset value, if applicable.  If it's a value, then we store the
    // character string to the integer, so that we can handle arbitrary-sized
    // data.
    ResetType       _resettype;
    Opencxx::Ptree *_resetvalue;
    // Reset action code.  If set, this takes priority over the
    // reset value.
    Opencxx::Ptree *_resetaction;

    // External-write action.  Arguments are different between register and
    // register-file (the registe-file has an extra index parm).  This is used
    // to model extra functionality on a write from an external source, e.g. a
    // systems model.
    Opencxx::Ptree *_external_write;

    // Read-only and reserved masks are generated from field data.
    BitVect _reserved;
    BitVect _readonly;

    mutable unsigned _usage_flags;
  };

  // Helper class for objects which refer to registers.  Stores a
  // string name which will be looked up during finalization.
  struct RegName {
    enum { 
      InvalidIndex = -1,   // Index not used for this name.
      ChildIdIndex = -2,   // Indexed to the child-id of this core object.
    };

    RegName();
    RegName(Opencxx::Ptree *n);

    const Reg     *reg() const;
    const RegFile *regfile() const;
    const RegBase *operator()() const { return _reg; };
    int index() const { return _index; };

    const CtxInstance &ctx() const { return _ctx; };

    operator bool() const { return _reg; };
  
    bool has_index() const { return _index >= 0; };
    bool has_child_id() const { return _index == ChildIdIndex; };
    bool has_reg()     const { return reg(); };
    bool has_regfile() const { return regfile(); };

    void finalize(const Core &core);

    void set_index(unsigned i) { _index = i; };
    friend std::ostream &operator<<(std::ostream &,const RegName &);

  protected:
    Opencxx::Ptree *_name;
    const RegBase  *_reg;
    int             _index;

    // If this refers to a register in a particular context, that
    // information is stored here.
    CtxInstance     _ctx;
  };

  // Information about a regfile-entry's action code, if there is any.
  class RegEntryHook : public Attrs {
  public:
    RegEntryHook(Opencxx::Environment *env,Opencxx::Ptree *loc);

    bool ignore() const { return _ignore; };

    Opencxx::Ptree *action() const { return _action; };
    const UsageInfo &usageinfo() const { return _usageinfo; };

    void finalize(Core &core,bool is_write,const RegName &);
    void dump(std::ostream &o,const gc_string &pfx) const;

  protected:
    friend class RegEntryHookHandler;

    bool  _ignore;
    UsageInfo    _usageinfo;
    Opencxx::Ptree *_action;  
  };

  // Stores information about a register entry in a sparse register file.
  class RegEntry : public Attrs, public Doc {
  public:
    RegEntry(Opencxx::Environment *env,Opencxx::Ptree *loc,unsigned id);

    unsigned id() const{ return _id; };

    const RegName &reg() const { return _reg; };

    const RegEntryHook *read() const { return _read; };
    const RegEntryHook *write() const { return _write; };
    
    const gc_string syntax() const { return _syntax;};
    
    bool finalize(Core &core,MsgLog &ml,unsigned max_size);

    void set_alias_index(unsigned i) { _reg.set_index(i); };
    void set_id(unsigned i) { _id = i; };

    void dump(std::ostream &,const gc_string &pfx) const;
  protected:
    friend class RegEntryHandler;
    friend class RegFileHandler;

    RegName    _reg;

    unsigned   _id;

    RegEntryHook *_read;
    RegEntryHook *_write;
    gc_string    _syntax; // syntax string as be printed by disassembler.
  };

  // This stores information about a register file.
  class RegFile : public RegBase {
  public:
    RegFile (Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);

    bool sparse() const { return !_entries.empty(); };
    unsigned size() const { return _size; };
    const gc_string &prefix() const { return _prefix; };
	const gc_string &prefix_for_regname() const { return _prefix_for_regname; };
	
    const RegEntryHash &entries() const { return _entries; };

    const RegFileHook *readhook() const { return _read; };
    const RegFileHook *writehook() const { return _write; };

    virtual bool has_readhook() const { return _read; };
    virtual bool has_writehook() const { return _write; };
    virtual bool has_immediate_writehook() const;

    virtual bool has_start_stop() const;
    virtual bool has_hook_action() const;

    bool has_alias() const;

    // Only valid for sparse registers:  Reads/writes of invalid indices.
    const HelperInfo &invalid_entry_read() const { return _invalid_entry_read; };
    const HelperInfo &invalid_entry_write() const { return _invalid_entry_write; };

    // Various checks about whether a register may be shared.
    virtual void check_sharing_okay() const;

    // This equivalence is defined in terms of what's needed to be able to
    // share a register.
    bool operator==(const RegFile &) const ;

    // We finalize the alias info first, to avoid ordering issues when sparse
    // register-files refer to other register-files.
    void finalize_alias(Core &core,MsgLog &ml);

    // Final construction pass- does various checks and populates
    // various data structures.
    virtual void finalize(Core &core,MsgLog &ml,unsigned id);

    void dump(std::ostream &,const gc_string &pfx) const;
  private:
    friend class RegBaseHandler;
    friend class RegFileHandler;
    friend class Reg;

    // If sparse, then register entries are stored here.
    RegEntryHash _entries;

    // Number of registers in the file.
    unsigned _size;

    // Prefix used for assembly/disassembly.
    gc_string _prefix;
	
	// Prefix used for regnames only.
	gc_string _prefix_for_regname;

    // For sparse register-files, these are hooks for reads/writes of invalid
    // indices.
    HelperInfo _invalid_entry_read;
    HelperInfo _invalid_entry_write;

    // Alias information, if applicable.
    RegFileHook *_read;
    RegFileHook *_write;
  };

  // This stores information about a register.
  class Reg : public RegBase {
  public:
    Reg (Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);

    const RegHook *readhook() const { return _read; };
    const RegHook *writehook() const { return _write; };

    virtual bool has_readhook() const { return _read; };
    virtual bool has_writehook() const { return _write; };
    virtual bool has_immediate_writehook() const;

    virtual bool has_start_stop() const;
    virtual bool has_hook_action() const;

    bool has_alias(bool check_hooks = true) const;

    // This equivalence is defined in terms of what's needed to be able to
    // share a register.
    bool operator==(const Reg &) const ;

    // Final construction pass- does various checks and populates
    // various data structures.
    virtual void finalize(Core &core,MsgLog &ml,unsigned id);

    void dump(std::ostream &,const gc_string &pfx) const;
  private:
    friend class RegHandler;
    friend class RegBaseHandler;

    // Alias information, if applicable.
    RegHook *_read;
    RegHook *_write;
  };

  class RegHookBase : public Base {
  public:
    RegHookBase(Opencxx::Environment *env,Opencxx::Ptree *loc);

    bool aliased() const { return _alias; };
    const RegName &alias() const { return _alias; };

    bool slice() const { return _slice; };
    unsigned slice_low() const { return _low; };
    unsigned slice_hi() const { return _hi; };
    unsigned slice_left_shift() const { return _lshift; };
    unsigned slice_width() const { return _hi - _low + 1; };

    bool immediate() const { return _immediate == ImmTrue; };

    bool ignore() const { return _ignore; };

    bool has_start_stop() const { return _has_start_stop; };

    const UsageInfo &usageinfo() const { return RegHookBase::_usageinfo; };

    Opencxx::Ptree *action() const { return _action; };

  protected:
    friend class HookHandler;
    friend bool operator==(const RegHookBase &x,const RegHookBase &y);

    void finalize(const RegBase *parent,Core &core,
                  bool is_write,bool &has_parms,
                  const std::pair<Opencxx::Ptree*,Opencxx::Ptree*> &slice_parms);
    void dump(std::ostream &o,const gc_string &pfx) const;
    bool setImmediate(const Core &core,bool is_write);

    RegName      _alias;
    bool         _ignore;
    bool         _slice;
    WriteHookImm _immediate;          // True => For a concurrent write, evaluate
                                      // the write-hook immediately, then delay
                                      // updates until the final commit process.  If
                                      // false, then the write-hook is evaluated at
                                      // commit time.
    bool         _has_start_stop;
    unsigned     _low, _hi, _lshift;
    UsageInfo    _usageinfo;
    Opencxx::Ptree *_action;  

    Opencxx::Ptree *_slice_value; // Temporary- just stores the original value
                                  // until finalization.
  };

  // Stores information about a register hook/alias.
  class RegHook : public RegHookBase {
  public:
    RegHook (Opencxx::Environment *env,Opencxx::Ptree *loc);

    void finalize(Reg *parent,Core &core,bool is_write,bool &has_parms);

    void dump(std::ostream &,const gc_string &pfx) const;
  protected:
    friend class HookHandler;
    friend class RegHookHandler;
    friend class RegHandler;
  };

  // Specializes RegHook for register files.
  class RegFileHook : public RegHookBase {
  public:
    RegFileHook(Opencxx::Environment *env,Opencxx::Ptree *loc);

    const UIntVect &aliasindices() const { return _aliasregs; };

    // Returns the aliased index, given an initial index.  If no mappings are defined
    // just returns index.
    unsigned getMappedIndex(unsigned index) const;

    void finalize(RegFile *parent,Core &core,bool is_write,bool &has_parms);

    void dump(std::ostream &,const gc_string &pfx) const;
  protected:
    friend class RegFileHookHandler;
    friend class RegFileHandler;
    friend bool operator==(const RegFileHook &x,const RegFileHook &y);

    // Registers in the alias that we use.
    UIntVect _aliasregs;
  };
  
  // left index may > right index, indicating big endian bit order,
  // lo(max_instr_width) and  hi(max_instr_width) will always return big endian indices, 
  // Note that negative values can be returned.
  class BitPair {
  public:
    BitPair(unsigned l,unsigned r): _l(l), _r(r) {}; 
    int lo(unsigned w) const { return (is_big_endian()) ? _l : ((int)w-(int)_l-1); };   
    int hi(unsigned w) const { return (is_big_endian()) ? _r : ((int)w-(int)_r-1); };   
    unsigned max() const { return (_l < _r) ? _r : _l ; };
    unsigned l() const { return _l;};
    unsigned r() const { return _r;};
    unsigned width() const { return (_l < _r) ? _r-_l+1 : _l-_r+1;};
    // Returns true if the argument intersects with this object.
    bool intersect(const BitPair &) const;
    // Returns true if the argument is a subset of this object.
    bool subset(const BitPair &) const;
    void check_order(Opencxx::Ptree *loc) const;
    bool operator<(const BitPair& rhs) const { return lo(0) < rhs.lo(0);}
    bool operator>(const BitPair& rhs) const { return lo(0) > rhs.lo(0);}
    
    static bool is_big_endian() { return endianness;};
    static bool is_little_endian() { return !endianness;};
    static void set_endianness(bool b) { endianness = b;};
  private:
    friend class BitPairVect;
    unsigned _l,_r; // left and right index
    static bool endianness; // true if big endian bit order;
  };
  
  struct BitPairVect : public gc_vector<BitPair> {    
    void record_bits_adapter(Opencxx::Ptree *p,Opencxx::Environment *env);
    bool record_bits(Opencxx::Ptree *p,bool error,Opencxx::Environment *env);
    void add_bits(const BitPair &bp) { this->push_back(bp); };
    void add_bits(unsigned l,unsigned h) { this->push_back(BitPair(l,h)); };
    void swap();
    unsigned highbit() const;
    unsigned lowbit() const;
    unsigned width() const;
    bool intersect(const BitPair &bp) const;
    void finalize(Opencxx::Ptree *loc);
  };
     
  std::ostream &operator<<(std::ostream &,BitPair);
  
  
  // Stores information about register fields.
  class RegField : public Attrs, public Named {
  public:
    RegField (Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);

    const BitPairVect& bits() const { return _bits;}
     
    unsigned width() const { return _width; };

    bool indexed() const { return _indexed; };
    bool is_split() const { return _bits.size() > 1; };
    unsigned index_count() const { return _indexcount; };

    bool reserved() const { return _reserved; };
    bool readonly() const { return _readonly; };
    bool writable() const { return _writable; };

    // A write to a field is OK if the field is not reserved and, if read-only,
    // then writable is set.
    bool write_ok() const { return !_reserved && (!_readonly || _writable); };

    const BitVect &write_mask() const { return _write; };

    // Query routines about register usage.
    bool is_field_used() const;
    bool is_field_read() const;
    bool is_field_written() const;
    bool is_field_helper_used() const;

    // Used internally.
    void set_field_usage(RegUsedFlags f) const { _usage_flags |= f; };

    void finalize(Core& core,unsigned offset,unsigned regwidth,bool has_writehook);

    void dump(std::ostream &,const gc_string &pfx) const;
  private:
    void set_indexed(unsigned w);

    friend class RegFieldHandler;
    friend class RegBaseHandler;
   
    BitPairVect _bits;
    
    unsigned _width;
    
    bool     _indexed;
    unsigned _indexcount;

    // If true, we're reserved.
    bool _reserved;

    // if true, we're readonly.
    bool _readonly;

    // If true, directly writable even if readonly.
    bool _writable;

    // mask for parallel execution
    BitVect _write;

    mutable unsigned _usage_flags;
  };
  
  // Stores information about a counter used to select bits from a prefix
  // instruction, which is then combined with an instruction's bits to an extend
  // a field's size.
  class PrefixCounter : public Attrs, public Named {
  public:
    PrefixCounter (Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);

    unsigned reset_value() const { return _reset_value; };

    void dump(std::ostream &,const gc_string &pfx) const;
  private:
    friend class ArchHandler;
    friend class PrefixCounterHandler;

    unsigned _reset_value;   // Reset value.
  };

  // Store constraints information for the instruction field, in form 
  // of a table of allowable value.
  class InstrFieldTable : public Base {
  public:
    typedef gc_vector<unsigned> InstrFieldTableEntry;
    typedef gc_vector<InstrFieldTableEntry> Entries;
    InstrFieldTable(Opencxx::Environment *env, Opencxx::Ptree *loc);
  
    void add_reserved();
    void add_entry() ;
    void add_to_entry(unsigned v);

    uint size() const { return _size; };
    uint entry_size() const      { return _entry_size; }
    const Entries &entries() const { return _entries; } ;

    static bool is_reserved(const InstrFieldTableEntry &e);
    bool   empty() const { return _entries.empty();}


    void finalize(const Core &core, MsgLog &ml);
    void dump(std::ostream &,const gc_string &pfx) const;



  private:
    Entries  _entries;
    uint     _entry_size;
    uint     _size; // bit size of returned value
  };

  // A class used to map src/target bit ranges or src/value 
  struct MappedBits {
    MappedBits(BitPair &src, BitPair &tgt,unsigned val=0)   : _src(src.l(),src.r()), _tgt(tgt.l(),tgt.r()), _value(val) {};
    MappedBits(BitPair &src, unsigned val=0) : _src(src.l(),src.r()), _tgt(0,0), _value(val) {};

    const BitPair &src() const { return _src; };
    const BitPair &trgt() const { return _tgt; };
    unsigned value() const { return _value; };

    BitPair  _src;
    BitPair  _tgt;
    unsigned _value;
  };

  typedef gc_multimap< std::pair<InstrField*,const SubInstrGrp*>, MappedBits> FieldBits;
  typedef FieldBits::iterator FieldBitsIter;


  // Stores information about an instruction field.
  class InstrField : public Attrs, public Named {
  public:
    enum Display { Default, Hex, Dec, SignedDec, Name };
    enum Type { tRegFile, tMem, tImm, tRefField, tInstr};  // Add a new type for micro-instructions
    enum Addr { aNone, aAbs, aPcRel };
    enum Disassemble { dsFalse, dsTrue, dsPrefix };
    enum IndexEndianness {UseParent, Big, Little} ;

    typedef  gc_list<std::pair<uint64_t ,uint64_t> >   Ranges; 
    typedef  gc_list<std::pair<uint64_t ,uint64_t> >   Masks;  
    typedef  gc_multimap<gc_string,InstrField*>      InstrFieldMMap;

    struct Field {
      enum Type { Ref, Range, RefIdx, TblIdx };
      const char* _name;
      const InstrField *_field; 
      BitPair     _range; 
      uint32_t    _value;
      Type        _type;

      Field(const char *n,unsigned l,unsigned h) : _name(n),_field(0),_range(l,h),_value(0),_type(Range) {};
      Field(const char *n,unsigned v) : _name(n),_field(0),_range(0,0),_value(v),           _type(RefIdx) {};
      Field(const char *n) : _name(n),_field(0),_range(0,0),_value(0),                      _type(Ref) {};
      Field(unsigned l, unsigned h) : _name(0),_field(0),_range(l,h),_value(0),             _type(Range) {};
    };
    typedef gc_vector<Field> Fields;


    struct Syntax : public gc {
      enum { Offset = 128 };
      
      typedef std::pair<Field, int>  FieldId;
      typedef gc_vector<FieldId>    FieldIds;

      gc_string          _fmt;    // Format string.
      FieldIds             _items;  // Indices of fields in the instruction.
      Opencxx::PtreeArray *_orig;   // Original field information.
      
      Syntax() : _orig(0) {};
      Syntax(const Syntax &o) ;
    
      bool isfield(unsigned char c) const { return (c >= Offset); };

      unsigned getfield(unsigned char c) const { return (c - Offset); };
      const FieldId&  getfiledid(unsigned char c) const { return _items.at(getfield(c)); };
      Field get(unsigned char c) const { return _items.at(getfield(c)).first; };
    };

    
    struct Alias : public gc {
      const InstrField   *_target;
      gc_string         _name;
      gc_string         _target_name;
      InstrField::Syntax *_syntax;
    };


    struct SplitInfo : public gc {
      // For assembler fields which some of their bits are prefix bits
      SplitInfo(const gc_string &name) : _name(name) {};
    
      bool  has_width(int w) const  { return (_widths.find(w) != _widths.end()); };

      gc_string _name;   // name of target prefix field interface
      IntSet      _widths; // instruction widths
    };

    InstrField(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);

    // Return the highest bit of the field.
    virtual unsigned highbit() const { return _highbit; };
    virtual unsigned lowbit()  const { return _bits.lowbit(); };

    virtual bool is_split() const { return (_bits.size() > 1); };
    virtual unsigned num_splits() const { return _bits.size(); };

    // Total width of the field, in bits.
    virtual unsigned width() const { return _width; };

    // If field is indexed - size of each slice
    virtual unsigned index() const { return _indexed; };

    // If field is indexed - endianness for retrieving slice.
    virtual IndexEndianness index_endianness() const { return _index_endianness; };

    // Total size of retruned field, in bits
    virtual unsigned size() const  { return _size; } ;

    // The mask has bits set for every bit in the field.
    const rnumber::RNumber& mask() const { return _mask; };

    // The block the field is associated with
    virtual const adl::StrList &blks() const { return _blks; };

    // Return the action code of the field
    virtual Opencxx::Ptree *action() const { return _action; };

    virtual const BitPairVect &bits() const { return _bits; };

    virtual const Fields &fields() const { return _fields; };

    // Instruction field relocation.  This is usually going to be unique, unless
    // this instruction field is used by an instruction which can map to
    // different macro-ops.
    virtual const RelocList &relocs() const { return (_alias && _alias->_target) ? _alias->_target->_relocs : _relocs; };
    
    virtual Type    type()    const { return _type; };
    virtual Display display() const { return _display; };
    virtual Addr    addr()    const { return _addr_type; };
    virtual bool    is_enumerated() const { return !_enums.empty(); };
    virtual bool    is_overlay() const { return _overlay; };
    virtual bool    is_signed()  const { return _signed; };
    virtual bool    is_unsigned_upper_bound() const { return _unsigned_ub; };
    virtual bool    is_inverted() const { return _inverted; };
    virtual bool    is_pseudo() const { return _pseudo; };
    virtual bool    is_alias() const { return (_alias != NULL); };
    virtual Alias  *get_alias() const { return _alias; };
    virtual bool    is_constrained() const { return !_table.empty(); }
    virtual bool    is_prefix() const { return _prefix; }
    virtual bool    is_variable() const { return _complement; };
    virtual bool    is_reserved() const { return _reserved; };
    virtual bool    is_assembler() const { return _assembler; };
    virtual Disassemble disassemble() const { return _disassemble; };
    virtual bool    is_parm() const { return _parm; };
    virtual bool    is_used() const { return _used; };
    virtual bool    allow_conflict() const { return _allow_conflict; };
    virtual unsigned shift() const { return _shift; };
    virtual unsigned offset() const { return _offset; };

    bool    is_generated() const { return _generated; };
    void    clear_variable() { _complement = false; };

    virtual const InstrField *field_type() const { return _pfield; };
    virtual const char *field_type_name()  const { return _pfield_name; };
    virtual const InstrField *encap_field() const { return _encap_field; };
    virtual const StrIntList &enums() const { return _enums; };
    virtual const Ranges &ranges() const { return _ranges; };
    virtual const Masks &masks() const { return _masks; };
    virtual const RegFile *regfile() const { return _regfile; };
    virtual const Memory  *memory() const { return _memory; };
    virtual const InstrTable *instrtable() const { return _instrtable; }; //
    virtual const gc_string instrtablename() const { return _instrtablename; }; //
    virtual const InstrFieldTable &table() const { return _table; };
    virtual const InstrFieldHash  &instrfields() const { return _instrfields; };
    virtual const Syntax *syntax() const { return _syntax; };
    static InstrField *build_impl(Opencxx::Environment *env,Opencxx::Ptree *loc, const gc_string &name, 
                                  InstrField *ref,bool is_reserved);
    virtual void  add_bits(const BitPair &bp) { _bits.add_bits(bp);};

    // get a nested field
    virtual const InstrField *getField(const gc_string &name) const;

    // Given a value, this writes the value into the bits specified
    // by the instruction field and returns that value, with 0s in all
    // other positions.Second parameter specifies width of return number (width of instruction)
    virtual rnumber::RNumber setValue(uint64_t val, unsigned width) const;
    virtual int32_t default_value() const;
    virtual bool has_default_value() const;
    //if it has validity checks
    virtual bool has_checks() const { return (!_ranges.empty() || !_masks.empty());}; 

    virtual void compute_mask(unsigned max_instr_width); 

    virtual void setPrefix() { _prefix = true ; };

    // return the field this implements - used by InstrFieldImpl 
    virtual const InstrField *interface() const { return 0 ;};

    // In prefix indexed field implementation: gaps-bits unmapped by the implementation
    virtual IntVect *gaps() { return 0; };
    virtual const IntVect *gaps() const { return 0; };

    void finalize(const Core &);  // For finalizing instruction-table type      
    void finalize(Core &,MsgLog &ml,InstrFieldMMap &fields);
    virtual void finalize(const Core &,MsgLog &ml,InstrFieldHash &fields,const gc_string &outer_name);

    void finalize_action(Core &);
    void finalize_alias(Core &,MsgLog &ml);
    void set_id(int id) { _id = id; };

    virtual void print_alias(std::ostream &,const gc_string &pfx) const;
    virtual void dump(std::ostream &,const gc_string &pfx) const;

    virtual unsigned id() const  { return _id; };

    virtual void setSplitInfo(SplitInfo *si) { _splitInfo = si; };
    virtual SplitInfo *splitInfo() { return _splitInfo; };
    virtual const SplitInfo *splitInfo() const { return _splitInfo; };
   
    // Works on a mutable element, so this can be const.  Register this field as
    // being used.
    void set_used() const { _used = true; };

  protected:
    friend class Core;
    friend class ArchHandler;
    friend class InstrFieldHandler;

    void handle_default_value();
    void check_bits(unsigned &width, unsigned maxWidth=0);
    void finalize_nested(const Core &,MsgLog &ml,InstrField *out);
    void finalize_fields(const Core &, MsgLog &ml);
    void finalize(const Core &,MsgLog &ml,unsigned maxWidth=0);
    void check_dims(const Core &,MsgLog &ml);
    bool legal_var(Opencxx::Ptree *p, const gc_string &name) const;
    void process_syntax(Core &core);
    void addImplicitEnums(const UIntVect &tlb_indices);

    BitPairVect     _bits;        // Vector of pairs of bit indices.

    unsigned        _highbit;
    unsigned        _width;
    unsigned 	    _size;
    rnumber::RNumber _mask;
 

    Type	    _type; // Field Type

    Display        _display;     // How the field should be displayed.
    bool           _overlay;     // If this field may overlap another field.
    bool           _signed;      // If we have a signed value (implies sign extension for encoding).
    bool           _unsigned_ub; // True => if signed, then treat upper bound as unsigned.
    bool           _inverted;    // If value should be inverted prior to encoding.
    bool           _pseudo;      // If it is a pseudo field: not encoded directly by assembler
    bool	   _prefix;      // If is a prefix field, can be used only in prefix instructions
    bool	   _complement;  // If is a complementary field, captures all fields not explicitly associated
    bool     _allow_conflict;    // During encoding, treat as an opcode bit, even though not a field.
                                 // Any other instructions which do have a fixed value will be selected first,
                                 // with instructions having this field being selected by default.
    Addr           _addr_type;   // Address type:  n/a, absolute. pc-relative.
    unsigned       _shift;       // If there's an implied shift value.
    unsigned       _offset;      // Implicit offset added to the field's value.
    StrIntList     _enums;       // If enumerated, list of enumeration values.
    unsigned	   _indexed;	 // Prefix field index width
    IndexEndianness _index_endianness; // Endianness for retrieving slices for indexed fields.
    InstrField    *_encap_field; // For nested instruction field, the encapsulating field


    Opencxx::Ptree*_ref;         // Temporary- what we're referencing.
    const InstrTable *_instrtable;  // The instruction table that the field is associated with, if any. Maybe modify some internal flag in the pointed instruction table by const_cast
    gc_string _instrtablename; // The instruction table name that the field is associated with, if any.
    const RegFile *_regfile;     // Register file instruction field is associated with, if any.
    const Memory  *_memory;      // Memory field is associated with, if any.
    InstrFieldTable _table;      // List of legal tuples, which can be used in this field.
    InstrFieldHash  _instrfields; // Nested instruction fields

    union {
      uint32_t  _default_num_value;// Default value for non-enumerated instruction field
      const char *_default_value;  // Default value for enumerated instruction field
      Opencxx::Ptree *_default; 
    };
    Ranges	        _ranges;      //  Valid ranges for the field   
    Masks	          _masks;       //  Valid masks for the field   
    adl::StrList    _blks;	      //  Block the field is associated with.
    RelocList       _relocs;      //  Relocations for this field
    Opencxx::Ptree *_action;      //  The action that defines the tree
    Fields          _fields;      //  The fields our the instruction field's output
    union {                       //  If prefix, name and type of prefix field implemented
      const char*    _pfield_name;
      const InstrField* _pfield;
    };
    Syntax 	     *_syntax;
    Alias            *_alias;
    unsigned	     _id;
    bool 	_reserved; // Reserved fields are ignored in decoding and need not be aliased
    bool	_assembler; // Field will not occur in syntax string but will be set by the assembler code
    Disassemble _disassemble; // Default disassembler behavior.
    bool	_parm; // Field is parm, shouldnt be mapped to bits
    bool	_generated; // Field implementation was created by gen_impl().
    SplitInfo  *_splitInfo; // For assembler fields, info on any prefix bits, if exist
    mutable bool _used;  // True if actually used by an instruction in a core that
                         // we're processing.
  };


  // Stores infromation about an instruction field implementation
  class InstrFieldImpl : public InstrField {
  public:
    //
    // The InstrFieldImpl class uses only a small subset of the base class member data
    // These are : loc,env, name, _bits, _mask, _highbit
    // Therefore, the following functions are not overloaded:
    // highbit(), is_split(), num_splits(), mask(), bits(), add_bits(), setValue().
    // 

    InstrFieldImpl(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name, const InstrField &trgt);
    // Total width of the field, in bits.
    virtual unsigned width() const { return _width; };

    // If field is indexed - size of each slice
    virtual unsigned index() const { return _impl.index(); };

    virtual IndexEndianness index_endianness() const { return _impl.index_endianness(); };

    // Total size of retruned field, in bits
    virtual unsigned size() const  { return _impl.size(); } ;

    // The block the field is associated with
    virtual const adl::StrList &blks() const { return _impl.blks(); };

    // Return the action code of the field
    virtual Opencxx::Ptree *action() const { return _impl.action(); };

    virtual const Fields &fields() const { return _fields; };



    virtual Type    type()    const { return _impl.type(); };
    virtual Display display() const { return _impl.display(); };
    virtual Addr    addr()    const { return _impl.addr(); };
    virtual bool    is_enumerated() const { return _impl.is_enumerated(); };
    virtual bool    is_overlay() const { return _impl.is_overlay(); };
    virtual bool    is_signed()  const { return _impl.is_signed(); };
    virtual bool    is_unsigned_upper_bound()  const { return _impl.is_unsigned_upper_bound(); };
    virtual bool    is_inverted() const { return _impl.is_inverted(); };
    virtual bool    is_pseudo() const { return false; };
    virtual bool    is_constrained() const { return _impl.is_constrained(); }
    virtual bool    is_prefix() const { return _impl.is_prefix(); }
    virtual bool    is_variable() const { return _impl.is_variable(); };
    virtual bool    is_reserved() const { return _impl.is_reserved(); };
    virtual bool    is_assembler() const { return _impl.is_assembler(); };
    virtual Disassemble disassemble() const { return _impl.disassemble(); };
    virtual bool    is_parm() const { return false; };
    virtual bool    is_used() const { return _impl.is_used(); };
    virtual bool    allow_conflict() const { return _impl.allow_conflict(); };
    virtual unsigned shift() const { return _impl.shift(); };
    virtual unsigned offset() const { return _impl.offset(); };
    virtual const InstrField *encap_field() const { return _impl.encap_field(); };
    virtual const InstrField *field_type() const { return _impl.field_type(); };
    virtual const char *field_type_name() const { return _impl.field_type_name(); };
    virtual const StrIntList& enums() const { return _impl.enums(); };
    virtual const Ranges&    ranges() const { return _impl.ranges(); };
    virtual const Masks&    masks() const { return _impl.masks(); };
    virtual const RegFile *regfile() const { return _impl.regfile(); };
    virtual const Memory  *memory() const { return _impl.memory(); };
    virtual const InstrFieldTable &table() const { return _impl.table(); };
    virtual const InstrFieldHash  &instrfields() const { return _impl.instrfields(); };
    virtual const Syntax *syntax() const { return _impl.syntax(); };
    // get a nested field
    virtual const InstrField *getField(const gc_string &name) const { return _impl.getField(name); };

    // Given a value, this writes the value into the bits specified
    // by the instruction field and returns that value, with 0s in all
    // other positions.
    virtual int32_t default_value() const  { return _impl.default_value(); };
    virtual bool has_default_value() const { return _impl.has_default_value(); };
    //if it has validity checks
    virtual bool has_checks() const { return _impl.has_checks(); }; 

    // return the field this implements - used by InstrFieldImpl 
    virtual const InstrField *interface() const { return &_impl ;};

    virtual IntVect *gaps() { return &_gaps; };
    virtual const IntVect *gaps() const  { return &_gaps; };

    virtual void setSplitInfo(SplitInfo *si) { _splitInfo = si; };
    virtual SplitInfo *splitInfo() { return _splitInfo; };
    virtual const SplitInfo *splitInfo() const { return _splitInfo; };

    virtual void finalize(const Core &,MsgLog &ml,InstrFieldHash &fields,const gc_string &outer_name);
    virtual void dump(std::ostream &,const gc_string &pfx) const;
    virtual unsigned id() const  { return _impl.id(); };
  private:
    friend class InstrField;

    const InstrField  &_impl;
    IntVect           _gaps;
    SplitInfo  *_splitInfo; // For assembler fields, info on any prefix bits, if exist
  };


  // Store information about relocation
  class Relocation : public Attrs, public Named {
  public:
    Relocation(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);

    // The name used within the assembly is either the relocation's name, or the
    // string specified by asmstr.
    const gc_string &asmstr() const { return (_abbrev.empty()) ? name() : _abbrev; };
    const gc_string &abbrev() const { return _abbrev; };

    int value() const { return _value; };
    int right_shift() const { return _right_shift; }; // Specifies the number of bits the relocation value is right-shifted before it is encoded
    const gc_string &right_shift_exp() const { return _right_shift_exp; }; // Specifies the expression for the number of bits the relocation value is right-shifted before it is encoded
    int width() const { return _width; };
    unsigned field_width() const { return _field_width; };
    bool     pcrel() const { return _pcrel > 0; };
    Opencxx::Ptree *action() const { return _action; };
    int offset() const { return _offset; };
    int bitpos() const { return _bitpos; };
    bool is_signed() const { return _is_signed; };
    bool check_overflow() const { return _check_overflow; };
    const InstrField *field() const { return _field._d; };
    // Note: Only set for little-endian architectures so that we can get
    // position information of the field.
    const Instr *instr() const { return _instr; };

    const InstrField *macro_op_field() const { return _macro_op_field; };    
    const Instr *macro_op_instr() const { return _macro_op_instr; };
    
    uint64_t src_mask() const { return _src_mask; };
    uint64_t dst_mask() const { return _dst_mask; };

    void     finalize(Core &,MsgLog &);
    void     dump(std::ostream &,const gc_string &pfx) const; 

    enum { Undef = -1 };
  private:
    const Instr *find_instr(Core &core_nc,MsgLog &ml,const InstrField &field);
    void updateFromInstrField(const InstrField &field,MsgLog &ml);
    void handle_instr_field(Core &core_nc,MsgLog &ml);
    const InstrField *findFieldImpl(const Core &core,MsgLog &ml,const InstrField &field);
    
    friend class RelocationHandler;
    friend class ArchHandler;
    friend class Core;

    gc_string _abbrev;
    int       _value;
    int       _width;
    int       _field_width;           // Width of field (in bits) which uses this relocation.
    int       _pcrel;                 // True => Consider this field to be pc relative.
    int       _right_shift;           // Specifies the number of bits the relocation value is right-shifted before it is encoded
    gc_string _right_shift_exp;       // Specifies the expression for the number of bits the relocation value is right-shifted before it is encoded
    int       _offset;                // Specifies the offset relative to the width of the instruction, 
                                      // which could be a macro instruction, so the relocation is not necessarily 
                                      // aligned to the end of the instruction.
    int       _bitpos;                // Left shift for positioning relocation within the instruction.
    int       _is_signed;             // True => Value should be considered signed for overflowchecking.
    bool      _check_overflow;        // True => Check overflow when performing relocation.
    Opencxx::Ptree *_action;          // Any special action to perform upon the relocation value.

    uint64_t _src_mask;               // Any portion of the instruction to add to the relocation value.
    uint64_t _dst_mask;               // Portion of the instruction in which to place the value.

    Ptr<InstrField>   _field;         // Associated field.
    const InstrField *_macro_op_field;// If this relocation is associated with a
                                      // micro-op, then this is the macro-op
                                      // field which encompasses the micro
                                      // operation.
    const Instr      *_instr;         // For little-endian architectures, we
                                      // have to know the size of the
                                      // instruction which uses the field, in
                                      // order to understand its position.  So,
                                      // for little-endian architectures, we get
                                      // that instruction here.  If we find any
                                      // other instructions which also use that
                                      // field, then they must be of the same
                                      // size.  TODO: Remove this when we get
                                      // rid of little-endian bit notation with
                                      // big-endian ordering, since that's what
                                      // causes the real problem.
    const Instr      *_macro_op_instr;// Same thing- this time for the macro-op field.
  };

  // Store infromation common to instructions and subinstructions

  class InstrBase : public Attrs, public Named {
  public:
    struct FieldBase : public gc {
     
      FieldBase(const char *name) : _name(name), _subinstr(0) {};
      virtual ~FieldBase() {};

      virtual FieldBase *clone() const = 0;
      virtual bool is_tmp() const = 0;

      const char         *_name;
      const SubInstrGrp  *_subinstr;
    };

    typedef gc_vector<FieldBase *> BFields;

    struct Field : public FieldBase {
      const InstrField *_field;
      const InstrField *_outer;
      const char       *_sname;
      enum ValType { Ref, Value, Expr, RValue, Resrvd, Alias}; 
      enum Transform   { None, Reverse, Symmetric, TransCnt};
      ValType           _type;
      union {
        uint64_t        _value;
        Opencxx::Ptree *_expr;
      };  
      Transform         _trans;

      Field(const char *n) : FieldBase(n), _field(0), _outer(0), _sname(0), _type(Ref), _value(0), _trans(None) {};
      Field(const char *n,const InstrField *field) : FieldBase(n), _field(field), _outer(0), _sname(0), _type(Alias), _value(0), _trans(None) {};
      Field(const char *n,uint64_t v) : FieldBase(n), _outer(0), _sname(0), _type(Value), _value(v),_trans(None) {};
      Field(const char *n,Opencxx::Ptree *e) : FieldBase(n), _field(0),_outer(0),_sname(0), _type(Expr),_expr(e),_trans(None) {};
      Field(const char *n, Transform t) : FieldBase(n), _field(0), _outer(0), _sname(0), _type(Ref), _value(0), _trans(t) {};

      Field *clone() const { return new Field(*this); };
      bool isOpcode() const { return _type == Value; };
      bool isOpcodeImp() const { return _type == RValue; };
      bool is_tmp() const { return false; };

      void dump(std::ostream &,bool use_doc_name) const;
      gc_string get_name(bool use_doc_name) const;
    };

    typedef gc_vector<Field> Fields;

    struct TmpField : public FieldBase {
      enum ValType { Ref, Value, Expr, RValue, Resrvd};
      enum Stat {Mapped, Sliced};
      ValType   _type;
      Stat 	_stat;
      BitPair   _src;
      BitPair   _tgt;
      union {
        uint32_t        _value;
        Opencxx::Ptree *_expr;
      };  
      TmpField(uint32_t l,uint32_t h) 
        : FieldBase(0), _type(Resrvd), _stat(Sliced), _src(l,h), _tgt(0,0), _value(0) {};
      TmpField(uint32_t v, uint32_t l,uint32_t h) 
        : FieldBase(0), _type(Value), _stat(Sliced), _src(l,h),_tgt(0,0), _value(v) {};
      TmpField(const char *n, uint32_t l, uint32_t h) 
        : FieldBase(n), _type(Ref), _stat(Mapped), _src(l,h), _tgt(0,0), _value(0) {};
      TmpField(const char *n, uint32_t ls, uint32_t hs, uint32_t lt, uint32_t ht) 
        : FieldBase(n), _type(Ref), _stat(Sliced), _src(ls,hs), _tgt(lt,ht), _value(0)  {};

      TmpField *clone() const { return new TmpField(*this); };
      bool is_tmp() const { return true; };
      void set_value(uint32_t v) { _type = RValue; _value = v; };
    };

    
    struct TmpFields : public gc {
      const char*  get_name(const char *n) const;
      const char*  get_inv_name(const char *n) const;
					   
      void  push_back(TmpField tf) { _fields.push_back(tf);};

      TmpField &back() { return _fields.back(); };
      const TmpField &back() const { return _fields.back(); };

      void  add_impl(const gc_string &s, const gc_string &imp); 
      void  finalize(const Core &core);
      StrStrMap  _impls;
      StrStrMap  _ifcs;
      gc_vector<TmpField> _fields;
    };

    // Information for instruction assembly, disassembly.
    struct Syntax : public gc {
      enum { Offset = 128 };
      typedef std::pair<Field, int>  FieldId;
      typedef gc_vector<FieldId>    FieldIds;

      gc_string          _fmt;    // Format string.
      FieldIds             _items;  // Indices of fields in the instruction.
      Opencxx::PtreeArray *_orig;   // Original field information.
      unsigned _pfx_end;            // End of prefix fields.
      unsigned _perm_end;           // End of permutable fields
      unsigned _in_begin,_in_end;   // Instruction name start/end offsets.
      unsigned _read_count;         // number of %f/%p terms 
      
      Syntax() : _orig(0), _pfx_end(Offset), _perm_end(Offset), _in_begin(0), _in_end(0), _read_count(0) {};
      Syntax(const Syntax &o) ;
    
      bool isfield(unsigned char c) const { return (c >= Offset); };
      bool isPfxfield(unsigned char c) const { return (c >= Offset && c < _pfx_end); };
      bool isPfield(unsigned char c) const { return (c >= Offset && c  < _perm_end); };
      bool isFfield(unsigned char c) const { return ( isfield(c) && !isPfield(c)); };    

      unsigned pfx_end() const { return (_pfx_end-Offset); };
      unsigned perm_end() const { return (_perm_end-Offset);};
      unsigned getfield(unsigned char c) const { return (c - Offset); };
      const FieldId&  getfiledid(unsigned char c) const { return _items.at(getfield(c)); };
      Field get(unsigned char c) const { return _items.at(getfield(c)).first; };

      bool hasName() const { return _in_end > 0; };
      gc_string getName() const { assert (hasName()); return _fmt.substr(_in_begin,(_in_end-_in_begin)); };

      bool have_to_invert() const;  // True if syntax has to be inverted for correct dsyntax                 
    };

    // Helper functions for casting from the base to one of the derived forms.
    static InstrBase::TmpField *get_tmpfield(InstrBase::FieldBase *f);    
    static InstrBase::Field *get_field(InstrBase::FieldBase *f);


    const Syntax *syntax() const { return _syntax; };
    const Syntax *dsyntax() const { return _dsyntax; };
    
    InstrBase(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);

    // Stores prefix counters as a list of pairs.  The first element is the
    // counter name, the second is the name of the field to which it is bound.
    const StrPairList &prefix_counters() const { return _prefix_counters; };

    // Lists prefix counters modified by this instruction.  First element is the
    // counter name, second element is whether to increment (true) or decrement
    // (false).
    const StrBoolList &prefix_counter_mods() const { return _prefix_counter_mods; };

    // Given a field name, searches for an associated prefix counter.  Returns 0 if not found.
    const gc_string *get_prefix_counter(const gc_string &field_name) const;

    const Fields &getFields() const { return _fields; };
    unsigned sizeFields() const { return _fields.size(); };
    template <typename pred> unsigned sizeFieldsWhose(const pred &p) const { return std::count_if(_fields.begin(), _fields.end(), p); };
    Field getField(unsigned x) const;
    // Returns index of field, or -1 if not found.
    int getField(const gc_string &nm) const { return findField(_fieldhash,nm); };

    // Same as getField, but will also return a field if its parent (if the field is an interface) matches.
    int getFieldOrParent(const gc_string &nm) const;
    
    int findField(const StrHash &,const gc_string &nm) const;

    virtual const UsageInfo &usageinfo() const { return _usageinfo; };

    const gc_string get_blk() const { return (_blks.empty()) ? gc_string() : _blks.front(); } // TODO: it may be necessary to remove this API and update callers to be list-aware
    const adl::StrList &blks () const { return _blks; }; // parents of nested instructions will have more than one possible block

    const Fields &getOpcs() const { return _opcs; };
    unsigned sizeOpcs() const { return _opcs.size(); };
    Field getOpc(unsigned x) const;
    // Returns index of field, or -1 if not found.
    int getOpc(const gc_string &nm) const { return findField(_opchash,nm); };

    const Fields &getAllFields() const { return _all; };
    unsigned sizeAll() const { return _all.size(); };
    Field getAll(unsigned x) const { return _all.at(x); };
    // Returns index of field, or -1 if not found.
    int getAll(const gc_string &nm) const { return findField(_allhash,nm); };

    // True if this is a micro operation (the instruction is encoded into an
    // instruction field of a larger instruction).
    bool is_micro_op() const { return _is_micro_op; };
    bool has_micro_ops() const { return _mo_count; };
    unsigned micro_op_count() const { return _mo_count; };
    bool has_micro_op_offset() const { return !_mo_offset_field.empty(); };
    const gc_string &micro_op_offset_field() const { return _mo_offset_field; };
    
    // This describes whether an instruction is a branch, and if so, if it's
    // conditional or unconditional.  This is determined simply by seeing if the
    // next-instruction-address register is updated, and if so, if the update is
    // conditional.
    BranchType is_branch() const { return _branch_type; };

    Opencxx::Ptree *action() const { return _action; };
    virtual void dump(std::ostream &,const gc_string &pfx) const;
    virtual void finalize_tmpfields(const Core &, InstrFieldHash &, bool prefix,bool check_for_pseudo_fields);

  protected:
    friend class ArchHandler;
    friend class InstrBaseHandler;
    friend class InstrHandler;
    friend class SubInstrHandler;
    friend class InstrTable;

    InstrBase(const InstrBase &o,const gc_string &name);

    virtual void finalize(MsgLog &,Core &, bool actionless=false, bool alias = false, bool assembler=false,
                          ResExcludes *src_excludes = 0,ResExcludes *trg_excludes = 0);
    virtual void process_fields(const Core &,Fields &,StrHash &, Fields &);
    virtual void process_pfx_ctrs(MsgLog &ml,Core &core);
    virtual void ptree_to_syntax(const Core &,Syntax* syntax);
    virtual void process_syntax(Core &);
    virtual void process_dsyntax(MsgLog &ml,Core &);
    virtual void process_shorthand(Core &) {};
    virtual void process_width(unsigned prefix_bits, bool need_encodng) {};
    const char*  gen_impl(const Core &core,FieldBitsIter iter,FieldBitsIter last,InstrField *target,InstrFieldHash &,
                          const char *n,bool is_reserved);
    const gc_string field_name(const gc_string &nm) const { return _tmpfields ? _tmpfields->get_name(nm.c_str()) : nm; };
    void check_for_pseudo_fields(const Core &core,bool check_pseudo_fields);
    bool add_tmp_field(const Core &core,unsigned &new_start,FieldBase *src,unsigned start,
                       bool le,bool check_pseudo_fields);
    void distr_fields(Fields &fields,Fields &opcs,TmpFields *tmpfields,BFields &src,
                      bool allow_opc_impl,bool drop_tmpfields);
    FieldBase *find_bfield(const gc_string &);

    void set_is_micro_op() { _is_micro_op = true; };

    // TBD - check
    // This pair of dummy functions is required since subinstr do no alias.
    virtual void print_intro(std::ostream &o,const gc_string &pfx) const;
    virtual void print_alias(std::ostream &l,const gc_string &pfx) const;

    // For initial processing- stores all fields, in the beginning.
    BFields _init_all;
    // Stores prefix counters as a list of pairs.  The first element is the
    // counter name, the second is the name of the field to which it is bound.
    StrPairList _prefix_counters;
    // Which prefix counters to increment (true) or decrement (false).
    StrBoolList _prefix_counter_mods;
    // Stores fields- items w/o a value.
    Fields _fields;
    // Stores opcodes- items w/ a value.
    Fields _opcs;
    // Stores all fields/opcodes.
    Fields _all;  
    // Stores all nested fields of _fields;
    Fields  _nestedfields;
    // Stores all nested fields of _opcs
    Fields _nestedopcs;
    // Stores fields by name and index.
    StrHash _fieldhash;
    // Stores opcodes by name and index.
    StrHash _opchash;  
    // Stores all opcodes and fields.
    StrHash _allhash;  
    // Fields on-the-fly
    TmpFields *_tmpfields;
    // The semantics of the instruction.
    Opencxx::Ptree *_action;
    // Syntax information for assembly.
    Syntax   *_syntax;
    // Syntax information for disassembly.
    Syntax   *_dsyntax;
    // Information about source and target registers and helpers used.
    UsageInfo _usageinfo;
    // The first element is the block the instruction belongs to.
    // For parents of nested instructions, this is a list of all possible blocks.
    adl::StrList _blks;
    // True if this instruction maps to an instruction table used as micro-ops.
    bool _is_micro_op;
    // Number of micro ops in the instruction.
    unsigned _mo_count;
    // If a call is made to setMicroOpOffset, then we record that info here so
    // that we can use it for later model generation.
    gc_string _mo_offset_field;
    // Whether this is a branch.
    BranchType  _branch_type;
   
  };

  class SubInstr : public InstrBase {
  public:
    SubInstr(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);
  
    virtual void finalize(MsgLog &ml,Core &);
    virtual void dump(std::ostream &o,const gc_string &pfx) const;
    Opencxx::Ptree *body() const { return _body; };
    Opencxx::Ptree *args() const { return _args; };

  private:
    friend class Instr;
    friend class SubInstrHandler;
    virtual void process_fields(const Core &,Fields &,StrHash &,Fields &);
    virtual void process_syntax(Core &);
    virtual void process_dsyntax(MsgLog &ml,Core &);
    Opencxx::Ptree *_body;
    Opencxx::Ptree *_args;
  };

  // Class for describing an alias in the finished data model.
  // The _fields array maps to fields in the target.  If the item
  // has a value, then that value should be used for that field of
  // the target.  If it doesn't have a value, then the _value field
  // holds the index of the field of this instruction that maps to it.
  struct InstrAlias : public gc {
    const Instr       *_target;
    InstrBase::Fields  _fields;
    
    InstrAlias() : _target(0) {};
    void print(std::ostream &,const Instr &) const;
  };
 
  // Stores information about an instruction.
  class Instr : public InstrBase {
  public:
    Instr(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name, int dummy = -1, bool remove = false);

    // The default rank of an instruction, in terms of ordering instructions
    // with the same name in the assembler.  We set this to a reasonably large
    // value so that someone can easily set a higher priority by putting in a
    // lower number.
    enum { DefaultAsmRank = 100 };
    
    // Stores information used for data-dependency tracking.
    class Dependencies : public gc {
    public:
      Dependencies(Opencxx::Ptree *o) : _orig(o) {};

      struct DepItem : public gc {
        DepItem () : _gmem(false), _mem(0), _reg(0), _index(0), _seq(0) {};

        const RegBase *regb() const { return _reg; };
        const Reg     *reg() const;
        const RegFile *regfile() const;
        const Memory  *mem() const { return _mem; };
        bool          gmem() const { return _gmem; };

        bool has_seq() const { return _seq > 0; };

        bool            _gmem;  // Item refers to global memory.
        const Memory   *_mem;   // Pointer to memory object, if relevant.
        const RegBase  *_reg;   // Pointer to register object, if relevant.
        Opencxx::Ptree *_index; // Index expression, for register-file accesses.
        unsigned        _seq;   // Access sequence number, if relevant.  0 => not used, 1 is first access.
      };
      
      typedef gc_list<DepItem> DepItems;
      struct DepExpr {
        DepItem  _trg;
        DepItems _srcs;
      };

      typedef gc_list<DepExpr> DepExprs;

      const DepExprs &exprs() const { return _exprs; };

      void finalize(MsgLog &ml,const Core &core,const Instr *instr);
      void dump(std::ostream &o,const gc_string &pfx) const;
    protected:
      void parseDepExpr(Opencxx::Ptree *p,MsgLog &ml,const Core &core,const Instr *instr);
      DepItem parseDepItem(Opencxx::Ptree *p,MsgLog &ml,const Core &core,const Instr *instr);
      void getDepSeq(DepItem &di,MsgLog &ml,Opencxx::Ptree *p);
      bool getDepRes(DepItem &di,MsgLog &ml,Opencxx::Ptree *p,const Core &core);

      DepExprs        _exprs; // The dependency expressions.
      Opencxx::Ptree *_orig;  // Original ptree block grabbed from parsing.
    };

    typedef InstrAlias Alias;
    typedef gc_vector<Alias *> Aliases;

    // Class for describing an alias during processing.
    struct TmpAlias : public gc {
      gc_string    _name;
      StrVect      _field_names;
      Fields       _fields;
      Fields       _opcs;
    };
    typedef gc_vector<TmpAlias *> TmpAliases;
    
    // Auxilary class for inverting syntax.  
    class ExtSyntaxItem {
    public: 
      enum Type {    Constants,   // mapping to a list of constans          e.g. BS(P>0 ? 0 : 1) 
                     Conditional,   // mapping to a list of functions   e.g. IMM(P>0 ? P : P>>8) 
                     Unconditional} ; // mapping to single function          e.g. SPRF(SPRF>>1)

      gc_set<gc_string>  _inputs;    // name of input fields
      gc_string          _output;    // name of output field    
      Opencxx::Ptree*      _orig_expr; // original expression
      Opencxx::PtreeArray* _inv_exprs; // inverted expressions      	 
      Type _type;     
    };
    typedef gc_map<gc_string,ExtSyntaxItem> ExtSyntaxItems;
    typedef gc_vector< std::pair<uint64_t,gc_string> > UIntStrVect;
    
    typedef InstrBase::Syntax Syntax;
    typedef InstrBase::Field  Field;
    typedef InstrBase::Fields Fields;

    unsigned id() const { return _id; };
    // Note: This returns true if the parent core was marked as
    // "instr_endianness = little".
    bool is_little_endian() const { return _little_endian; };

    // TODO: Remove support for mixed-endian (which is where bit-pairs use
    // little-endian numbering, but the memory organization is big-endian.  This
    // leads to all kinds of extra complexity that we can eliminate.
    bool is_mixed_endian() const;   
    
    bool is_simple_alias() const { return !_alias.empty() || !_tmpalias.empty(); };
    bool is_complex_alias() const { return _alias_action; };
    bool is_alias() const { return is_simple_alias() || is_complex_alias(); };
    // Returns the first alias item.  Only a single alias item is allowed,
    // unless we're a shorthand, in which case we allow multilple aliases
    // (basically acting as a macro).
    const Alias *get_alias() const { return (!_alias.empty()) ? _alias.front() : 0; };
    // Return the list of aliases (may be empty).
    const Aliases &get_aliases() const { return _alias; };
    // True if this is a shorthand (an assembler construct for mapping one form
    // to a real instruction or instruction(s)).
    bool shorthand() const { return _shorthand; };
    bool multitarget_shorthand() const { return shorthand() && (_alias.size() > 1); };

    const gc_string &doc_op() const { return _doc_op; };

    // This returns true if the instruction is included in any instruction table
    // of its parent core.
    bool is_used() const { return _shorthands; };

    // This returns true if the instruction is encoding of multiple instructions
    // using subinstrgrps.
    bool is_succint() const { return !_subinstrgrps.empty(); };

    bool has_subinstrs() const { return _has_subinstrs; };

    // True if it is a placeholder for defmod operation
    bool is_dummy() const { return _dummy > 0;};
    // True if it a dummy remve.
    bool is_remove() const { return _remove;};
        
    // Pseudo instruction defines an instruction template - how it is 
    // composed from prefix
    bool is_pseudo() const { return _pseudo; };

    // Is this a prefix instruction
    bool is_prefix() const { return _prefix; };


    // This returns true if all fields are pseudo - used as a target for
    // multiple aliases with encoding 
    bool is_abstract() const;

    // True, if instrcution was defined without encoding
    bool need_encoding() const { return _need_encoding; };

    // This allows an instruction to be encoded when its opcode overlaps with
    // non-opcode bits of other instructions, and there is no single instruction
    // which has a set of opcode bits which is a superset of all other bits.
    // This should only be set in the case of complex encodings, e.g. "if these
    // bits are set to 1, then we interpret this as one kind of instruction,
    // otherwise...".  This is analogous to the allow_conflict flag in
    // instruction fields, but this is useful for when the instruction field
    // mapping is very complex, so that it's easier to specify this in the
    // instruction.
    bool allow_conflict() const { return _allow_conflict; };

    // Width in bits, including prefix information.
    unsigned width() const { return _width; };
    
    // Width in bits, without prefix bits.
    unsigned fetch_width() const { return _fetch_width; };

    // Next instruction table (if relevant).
    Opencxx::Ptree *next_table() const { return _next_table; };

    // This returns the usage info of the target of an alias, if this is an
    // alias.  Otherwise, returns the instruction's own usage info.
    virtual const UsageInfo &usageinfo() const;

    const InstrList &shorthands() const;

    const InstrHash &get_nested() const { return _instrs; };

    bool is_nested() const { return _nested;};
    
    bool has_variable_field() const;
    
    const Instr *get_type() const { return _type; };

    bool assemble() const { return _assemble; };

    unsigned asm_rank() const { return _asm_rank; };
    
    bool disassemble() const { return _disassemble; };

    Opencxx::Ptree *alias_action() const { return _alias_action; };
    
    Opencxx::Ptree *assembler() const { return _assembler; };

    const StrVect &asmrules() const { return _asmrules; };

    const Dependencies *dependencies() const { return _dependencies; };

    uint8_t reserved_bits_value() const { return _reserved_bits_value; };

    enum PostFetchEnum {
      PostFetchFalse,
      PostFetchTrue,
      PostFetchFunc,
    };

    PostFetchEnum enable_post_fetch() const { return _enable_post_fetch; };
    const HelperInfo &enable_post_fetch_func() const { return _enable_post_fetch_func; };

    // Returns an RNumber with all opcode values set.  If alias is set, then get
    // opcode data taking alias into account.  Generally, alias will just come
    // from the same instruction, but this allows for supporting shorthands
    // which map to multiple target instructions.  For the single parameter form,
    // if an alias is present, the first alias is used.
    rnumber::RNumber getOpcode(unsigned width,bool swap_for_le) const;
    rnumber::RNumber getOpcode(unsigned width,const Instr::Alias *alias,bool swap_for_le) const;

    // Returns an RNumber with 1s for all reserved bits (reserved fields and
    // missing fields).
    rnumber::RNumber getReservedMask(unsigned width) const;
    rnumber::RNumber getReservedMask(unsigned width,const Instr::Alias *alias) const;

    //Returns number of opcode-like fields (in the case of subinstructions with the same name);
    unsigned getNFixed() const;

    // For instructions, we fetch aspects of child objects as well, if they have
    // no action code.  Yes, this is kind of a hack for StarCore.
    virtual bool getAspects(AspectList &,const gc_string &label) const;

    virtual void finalize(Core &core, MsgLog &msglog, InstrHash &generated, InstrHash &dummies);
    virtual void finalize_tmpfields(const Core &, InstrFieldHash &);
    virtual void finalize_tmpfields_alias(const Core &, InstrFieldHash &);
    virtual void finalize_split_fields();
    void expand_subinstructions(Core &core,MsgLog &ml,InstrHash &generated, InstrHash &dummies);
    void transfer_fields(Core &core,MsgLog &ml);

    virtual void dump(std::ostream &,const gc_string &pfx) const;

    rnumber::RNumber adjustForLittleEndian(rnumber::RNumber x,unsigned width,bool swap_for_le) const;    
  private:
    friend class ArchHandler;
    friend class InstrHandler;
    friend class InstrTable;
    friend class Core;    
    Instr(const Instr& o, const gc_string &name);

    bool initialMicroOpCheck(const Core &core);

    virtual void process_fields(const Core &,Fields &,StrHash &,Fields &);
    virtual void process_syntax(Core &);
    void process_syntax_list(Syntax* syntax, InstrHash& generated, bool dsyntax = false);
    ExtSyntaxItems build_ext_items(Opencxx::Environment* env,Core& core);
    UIntStrVect    search_fixed_vals(Opencxx::Ptree* loc);
    void process_fixed_vals(Syntax* ds, UIntStrVect& fixed_vals);
    Opencxx::Ptree *build_inv_expr(gc_vector<Opencxx::Ptree *> &values,
                               gc_vector<std::pair<Opencxx::Ptree*, gc_string> > &checks,
                               gc_string target,
                               Opencxx::Environment* env);
    void gen_val_checks(gc_vector<Opencxx::Ptree *> &values,
                        gc_vector<std::pair<Opencxx::Ptree*, gc_string> > &checks,
                        gc_string &target,
                        ExtSyntaxItems &ext_items);
    bool gen_val_checks_for_pseudo(gc_vector<Opencxx::Ptree *> &values,
                                   gc_vector<std::pair<Opencxx::Ptree*, gc_string> > &checks,
                                   Syntax::FieldId& fp,
                                   gc_string &target,
                                   ExtSyntaxItems &ext_items,
                                   const Core &core,
                                   Opencxx::Environment* env);
    
    bool pc_found(Opencxx::Ptree* expr);
    unsigned int process_format(Syntax* synntax, gc_string name,bool require_name_first);

    void process_subsyntax(Syntax* syntax, const Core& core, bool dsyntax = false);
    virtual void process_dsyntax(MsgLog &,Core &);
    bool is_syntax_list(const Syntax*);
    void invert_syntax(MsgLog &ml,Core&);
    virtual void process_width(unsigned prefix_bits, bool need_encoding);
    virtual void process_shorthand(Core &);
    void process_var_opcode(TmpAlias *,const Instr *);
    void check_blk(const Core &);
    void process_reserved_bits_value(Core &core, MsgLog &ml );

    void encode(const Core &, unsigned &opcd, unsigned opcd_size, unsigned width);

    int  add_alias_field(const char *name,bool add_fields, bool &is_opc);
    Instr* gen_instruction(const gc_string &name, SubInstrHash &subinstrs, Core &core);
    void finalize_nested(const Core &core);
    void finalize(Core &core, MsgLog &ml);

    virtual void print_intro(std::ostream &,const gc_string &pfx) const;
    virtual void print_alias(std::ostream &,const gc_string &pfx) const;
    
    // Apply modification from defmod for instruction not know at parse-time
    // (that built from subinstructions)
    void apply_modifications(Instr &source);
   
    // Add encap instruction attributes and blk information to nested instruction
    void add_encap_data(MsgLog &ml, const Instr &instr);
  
    // Gets the total fields width (excluding opcode), to determine encoding width
    unsigned fields_width(const Core &core) const;

    void process_user_src_trg_info(const Core &core,MsgLog &ml);
    void process_exclude(ResExcludes &excludes,const Core &core,Opencxx::Ptree *src);
    void process_user_resource(UsedRegs &usedregs,const Core &core,Opencxx::Ptree *src);

    // Unique ID of this instruction.
    unsigned    _id;
    // Width of encoding in bits.
    unsigned    _width; 
    // Width in bits without prefix bits.
    unsigned    _fetch_width; 
    // True -> Little-endian instruction, thus do word-swap.
    bool        _little_endian;
    // True -> Instruction should be considered for assembly.
    bool        _assemble;
    // Specify an ordering rank for instructions of the same name in the
    // assembler.  Lower is higher-priority.  Instructions of the same rank are
    // sorted according to the assembler-generator's heuristics.
    unsigned    _asm_rank;
    // True -> Instruction should be considered for disassembly.
    bool        _disassemble;
    // The assembler function of the instruction
    Opencxx::Ptree *_assembler;
    // Action function for complex aliases.
    Opencxx::Ptree *_alias_action;
    // An alias- used during processing.
    TmpAliases _tmpalias;
    // The alias, after processing.
    Aliases   _alias;
    // This stores any instructions which are shorthands for this instructin.
    InstrList  *_shorthands;
    // True -> Used for assembly only- this is a shorthand for another instruction.
    bool      _shorthand;
    // This stores the groups of subinstrs
    SubInstrGrpHash _subinstrgrps; 
    // This stores the instructions names, required for subinstr usage
    StrVect     _names;
    // True:  This is a generated instruction with sub-instructions.
    bool _has_subinstrs;
    // Value greater then zero indicates that instruction is used as a placeholeder 
    // for defmod information. The number used
    // indicates order they appear in .adl file (important for defmod).
    int _dummy;
    // In case of dummy function indicates that there was a defmod with "remove = 1".
    bool _remove;

    // Optional operation documentation.
    gc_string _doc_op;
    
    // This is a prefix instruction
    bool	_prefix;
    // This is a pseudo (template) instruction
    bool	_pseudo; 
    // Instruction type name - must be a pseudo instruction 
    gc_string _type_name;
    // Next instruction table to be used for the next decode after this instruction.
    Opencxx::Ptree *_next_table;
    // Instruction type - the instruction refered by _type_name,  if applicable
    const Instr *_type;
    // Nested instructions 
    InstrHash  	_instrs;
    // Is a nested instruction
    bool	_nested;
    // Is this an instruction without encoding    
    bool        _need_encoding;
    // Allow conflicts in encoding.  See comments above in allow_conflict().
    bool        _allow_conflict;
    // Preferred value to use for reserved bits (1 or 0).
    uint8_t     _reserved_bits_value;
    // Whether to add the post-fetch logic (which generally bumps the program counter).
    PostFetchEnum _enable_post_fetch;
    HelperInfo    _enable_post_fetch_func;

    // Temporary- stores info about items which should be excluded from the 
    // final source list.
    PList       _orig_source_excludes;   
    ResExcludes _source_excludes;

    PList       _orig_target_excludes;   
    ResExcludes _target_excludes;

    // These temporarily store explicit user-specified resources and target.
    PList       _orig_sources;
    PList       _orig_targets;

    // Internal use:  Control over syntax/dsyntax processing with sub-instructions and such.
    bool _process_sub_syntax;
    bool _process_sub_dsyntax;

    // This stores the names of the assembler rules associated with the instruction
    StrVect 	_asmrules;

    // Stores information for data-dependency tracking.
    Dependencies *_dependencies;
    
  };

  std::ostream &operator<<(std::ostream &,const Instr::Field &);

  std::ostream &operator<<(std::ostream &,const Instr::Dependencies::DepItem &);


  class SubInstrGrp : public Base, public Named
  {
  public:
    SubInstrGrp(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);

    typedef SubInstrs::const_iterator	     SubInstrIterator;
    SubInstrIterator  begin() const { return _subinstrs.begin(); };
    SubInstrIterator  end()   const { return _subinstrs.end(); };
    unsigned  size() const { return _subinstrs.size(); }

    const StrList &names()       const { return _names; };
    const SubInstrs &subinstrs() const { return _subinstrs; };

    void finalize(const Core &);
    void dump(std::ostream &, const gc_string &pfx) const;

  private:
    friend class SubInstrGrpHandler;
    friend class InstrHandler;

    StrList    _names; 	// subinstrcutions names 
    SubInstrs  _subinstrs;
  };

  // Stores info about a single field within an exception.
  class ExceptionField : public GenericField {
  public:
    ExceptionField(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);

    friend class ExceptionHandler;
  };

  // Stores information about an exception.
  class Exception : public Attrs, public Named {
  public:
    Exception(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);

    unsigned priority() const { return _priority; };

    Opencxx::Ptree *action() const { return _action; };

    Opencxx::Ptree *enable() const { return _enable; };

    bool has_fields() const { return !_fields.empty(); };

    const ExceptionFieldHash &fields() const { return _fields; };

    SensitivityType sensitivity() const { return _sensitivity; };

    void finalize(Core &,MsgLog &ml);

    void dump(std::ostream &,const gc_string &pfx) const;

    gc_string get_exception_type() const;
  private:
    friend class ArchHandler;
    friend class ExceptionHandler;

    unsigned            _priority;
    Opencxx::Ptree     *_action;
    Opencxx::Ptree     *_enable;
    ExceptionFieldHash  _fields;
    SensitivityType     _sensitivity;
  };

  // Stores information about the MMU.
  class MMU : public Attrs {
  public:
    MMU(Opencxx::Environment *env,Opencxx::Ptree *loc);

    // This returns top-level lookups.
    const MmuLookupHash &top_lookups() const { return _top_lookups; };
    // All lookups, even nested ones.
    const MmuLookupHash &all_lookups() const { return _all_lookups; };

    // Only searches top-level lookups.
    const MmuLookup *get_top_mmulookup(const gc_string &) const;
    // Searches through the complete set.
    const MmuLookup *get_any_mmulookup(const gc_string &) const;

    bool is_lookup_enum(const gc_string &) const;

    const gc_string &doc() const { return _doc; };

    // True if the lookups are prioritized.  If false, then the multi-hit
    // function should be called if multiple hits occur.
    bool instr_prioritized() const { return _instr_prioritized; };
    bool data_prioritized() const { return _data_prioritized; };

    // True if a multi-hit search should be used.
    bool multi_hit_search() const { return _multi_hit || _final_hit; };

    bool interleaved_fetch() const { return _interleaved_fetch; };
    bool interleaved_reads() const { return _interleaved_reads; };
    bool interleaved_writes() const { return _interleaved_writes; };

    const HelperInfo &instr_enable() const { return _instr_enable; };
    const HelperInfo &data_enable()  const { return _data_enable; };

    const HelperInfo &instr_miss() const { return _instr_miss; };
    const HelperInfo &data_miss() const { return _data_miss; };
    const HelperInfo &load_miss() const { return _load_miss; };
    const HelperInfo &store_miss() const { return _store_miss; };
    const HelperInfo &multi_hit() const { return _multi_hit; };
    const HelperInfo &final_hit() const { return _final_hit; };
    const HelperInfo &misaligned_read() const { return _misaligned_read; };
    const HelperInfo &aligned_write() const { return _aligned_write; };
    const HelperInfo &misaligned_write() const { return _misaligned_write; };
    const HelperInfo &pre_read() const { return _pre_read; };
    const HelperInfo &post_read() const { return _post_read; };
    const HelperInfo &pre_write() const { return _pre_write; };
    const HelperInfo &post_write() const { return _post_write; };

    void populate_global_lookups(MsgLog &ml);
    void finalize(MsgLog &,Core &);

    void dump(std::ostream &,const gc_string &pfx) const;

  protected:
    MmuLookup *get_top_mmulookup(const gc_string &);
    MmuLookup *get_any_mmulookup(const gc_string &);
    void check_miss_handler(MsgLog &ml,Core &core,HelperInfo &miss_func,const char *msg);

  private:
    friend class ArchHandler;
    friend class MmuHandler;
    friend class MmuLookup;

    MmuLookupHash _top_lookups;
    MmuLookupHash _all_lookups;

    bool _data_prioritized;
    bool _instr_prioritized;

    bool _interleaved_fetch;
    bool _interleaved_reads;
    bool _interleaved_writes;

    HelperInfo _instr_enable;
    HelperInfo _data_enable;

    HelperInfo _instr_miss;
    HelperInfo _data_miss;
    HelperInfo _load_miss;
    HelperInfo _store_miss;

    HelperInfo _multi_hit;  
    HelperInfo _final_hit;

    HelperInfo _aligned_write;
    HelperInfo _misaligned_write;

    HelperInfo _misaligned_read;

    HelperInfo _pre_read;
    HelperInfo _post_read;

    HelperInfo _pre_write;
    HelperInfo _post_write;

    gc_string _doc;
  };

  // Stores info about a single field within a lookup object.
  class MmuField : public GenericField {
  public:
    MmuField(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);

    friend class MmuLookupHandler;
  };

  // Stores tests used by an mmu lookup.  These tests are grouped into sets so
  // that different criteria can be used, depending upon the state of the model.
  class MmuTestSet : public Attrs, public Named {
  public:
    MmuTestSet(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);

    MmuTestSet *clone() const;

    const HelperInfo &enable() const { return _enable; };
    const PVect &tests() const { return _tests; };

    void finalize(MsgLog &,MmuLookup &,Core &,const StrVecList &);
    void dump(std::ostream &,const gc_string &pfx,bool print_name = true) const;
  private:
    friend class MmuTestSetHandler;
    friend class MmuLookupHandler;

    HelperInfo        _enable;
    PVect             _tests;
  };

  // Stores information about a MMU lookup type.
  class MmuLookup : public Attrs, public Named {
  public:
    enum Type { Instr, Data, Both };
    enum PSType { None, Const, BitSize, LeftMask };

    MmuLookup(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);

    unsigned id() const { return _id; };

    // Copy data from the source, but not name, location, or environment.
    void copy(const MmuLookup &);

    Type type() const { return _type; };
    int priority() const { return _priority; };

    // If not valid, then lookup is not used as part of the overall address
    // translation scheme.  It only exists as a disconnected entity.
    bool valid_priority() const { return _priority >= 0; };

    // A translating lookup is something which is not an interface and either
    // has an array or a miss handler.  If it doesn't have either of those, then
    // it might be used for explicit logging purposes, such as for a
    // page-table-entry, so we'll need the data structure but no translation
    // functions.
    bool valid_translating_lookup() const { return !is_interface() && (has_array() || miss_func()); };

    // Return the type-name associated with this lookup.
    gc_string type_name() const;
    // The enum name used for dealing with TLB ids in user code.
    gc_string enum_name() const;
    // Return all possible, valid typenames.  This includes inherited items.
    StrVect all_type_names() const;

    // True:  This is an interface object and should thus not be instantiated.
    bool is_interface() const { return _interface; };

    // The interface parent, if applicable.
    const MmuLookup *iparent() const { return _iparent; };

    bool has_array() const { return _array_entries != 0; };
    bool fully_assoc() const { return _array_assoc == 0; };

    // True if a multi-hit search should be used.
    bool multi_hit_search() const { return _multi_hit || _final_hit; };

    // True if tlb contains a Bounded test.
    bool bounded() const { return _lower_bound; };
    Opencxx::Ptree *lower_bound() const { return _lower_bound; };
    Opencxx::Ptree *upper_bound() const { return _upper_bound; };
    Opencxx::Ptree *bound_mask() const { return _bound_mask; };

    unsigned entries() const { return _array_entries; };
    unsigned assoc() const { return _array_assoc; };
    unsigned sets() const { return (fully_assoc()) ? 1 : (_array_entries / _array_assoc); };
    unsigned ways() const { return (fully_assoc()) ? _array_entries : _array_assoc; };

    PSType sizetype() const { return _sizetype; };
    const MmuField *sizefield() const { return _sizefield._d; };
    int             pagesize() const { return _size; };
    unsigned        sizescale() const { return _sizescale; };
    unsigned        sizeoffset() const { return _sizeoffset; };
    unsigned        sizeshift() const { return _sizeshift; };

    int             pageshift() const { return _pageshift; };

    // This is the total size, in bytes, if the lookup is serialized to memory.
    unsigned        mem_size() const { return _entry_mem_size * sets() * ways(); };

    // Return the smallest possible page size (in bytes).
    unsigned        min_page_size() const;

    const MmuField *realpage() const { return _realpage._d; };

    // These search across both set and way fields.
    // Find a field; returns -1 if not found.
    const MmuField *find_field(const gc_string &name) const;
    // Find a field; throw an error if not found and msg != 0;
    const MmuField *find_field(Opencxx::Ptree *,const char *msg) const;

    const MmuFieldHash &set_fields() const { return _set_fields; };
    const MmuFieldHash &way_fields() const { return _way_fields; };

    const MmuLookupHash &lookups() const { return _lookups; };

    const MmuTestSetHash &tests() const { return _tests; };
    const MmuTestSets    &test_order() const { return _test_order; };

    const HelperInfo &exec_perm() const  { return _exec_perm; };
    const HelperInfo &load_perm() const  { return _load_perm; };
    const HelperInfo &store_perm() const { return _store_perm; };

    const HelperInfo &multi_hit() const { return _multi_hit; };
    const HelperInfo &final_hit() const { return _final_hit; };

    const HelperInfo &valid_func() const { return _valid; };
    const HelperInfo &hit_func() const   { return _hit; };
    const HelperInfo &miss_func() const  { return _miss; };
    const HelperInfo &reset() const      { return _reset; };

    const HelperInfo &mem_read() const { return _mem_read; };
    const HelperInfo &mem_write() const { return _mem_write; };

    // This equivalence is defined in terms of what's needed to be able to
    // share a lookup across cores.
    bool operator==(const MmuLookup &) const;

    void check_sharing_okay() const;
    void populate_global_lookups(MsgLog &ml,MMU &mmu);
    void finalize(MsgLog &,Core &,const StrVecList &,unsigned id);

    void dump(std::ostream &,const gc_string &pfx) const;
  private:
    friend class MmuHandler;
    friend class MmuLookupHandler;
    friend class MmuArrayHandler;
    friend class MmuPermHandler;
    friend class MmuTestSet;

    int  check_lookup_args(MsgLog &ml,Opencxx::Ptree *func,const StrVecList &lnames,
                           int extra_args,const char *msg);
    void check_lookup_func(MsgLog &ml,HelperInfo &info,Core &core,const StrVecList &lnames,const char *msg);
    bool check_code(Opencxx::Ptree *func,CheckCode &cc,bool preds_okay,const StrVecList *lnames);
    bool check_code(Opencxx::Ptree* &result,Opencxx::Ptree *func,Core &core,bool return_okay,bool reg_targets_okay,
                    bool preds_okay,const StrVecList *lnames = 0);
    bool check_code(HelperInfo &info,Core &core,bool return_okay,
                    bool reg_targets_okay = true,bool preds_okay = true,const StrVecList * = 0);
    Opencxx::Ptree *check_test(Opencxx::Ptree *test,Core &core,const StrVecList &lnames);
    void copy_tests(const MmuLookup &x);

    MmuLookupHash    _lookups;       // Child lookup objects.

    unsigned         _id;

    Type             _type;

    int              _priority;

    bool             _interface;     // True:  This is just an interface lookup (should not be instantiated).
    const MmuLookup *_iparent;       // Non-zero:  Interface parent object.
  
    unsigned         _array_entries;
    unsigned         _array_assoc;

    MmuFieldHash     _set_fields;
    MmuFieldHash     _way_fields;

    Ptr<MmuField>    _sizefield;
    int              _size;          // Either the value, or ptr to field storing size.
    unsigned         _sizescale;
    unsigned         _sizeoffset;
    unsigned         _sizeshift;             
    PSType           _sizetype; 

    int              _pageshift;

    Ptr<MmuField>    _realpage;      // Ptr to field specifying real-page.

    Opencxx::Ptree  *_lower_bound;   // If applicable, lower-bound field.
    Opencxx::Ptree  *_upper_bound;   // If applicable, upper-bound field.
    Opencxx::Ptree  *_bound_mask;    // If applicable, expression for masking bondary comparison address.

    HelperInfo       _valid;         // Translation valid predicate.

    HelperInfo       _multi_hit;  
    HelperInfo       _final_hit;  

    HelperInfo       _miss;
    HelperInfo       _hit;
    HelperInfo       _reset;

    MmuTestSetHash   _tests;

    StrList         *_tmp_test_order;
    MmuTestSets      _test_order;

    HelperInfo        _exec_perm;
    HelperInfo        _load_perm;
    HelperInfo        _store_perm;

    unsigned          _entry_mem_size;
    HelperInfo        _mem_read;
    HelperInfo        _mem_write;
  };

  struct SortPriority {
    bool operator()(const MmuLookup *x,const MmuLookup *y) {
      return x->priority() < y->priority();
    };
  };

  struct CompPriority {
    bool operator()(const MmuLookup *x,const MmuLookup *y) {
      return x->priority() == y->priority();
    };
  };

  // Stores info about a single field within a cache set or way.
  class CacheField : public GenericField {
  public:
    CacheField(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);

    friend class CacheHandler;
  };


  // Stores information about a cache
  class Cache : public Attrs, public Named {
  public:
    Cache(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);

    unsigned id() const { return _id; };
    CacheType type() const { return _type; };
    bool      unified() const { return _type == UnifiedCache; };

    const gc_string &orig_name() const { return _orig_name; };

    unsigned  level() const { return _level; };
    unsigned  size() const { return _size; };
    unsigned  linesize() const { return _linesize; };

    bool fully_assoc() const { return (_ways == 0); };

    // Represents maximum values, if multiple values are present.
    unsigned sets() const { return _sets; };
    unsigned ways() const { return _ways; };

    // Returns the list of possible choices for sets/ways.
    const UIntVect &sets_list() const { return _sets_list; };
    const UIntVect &ways_list() const { return _ways_list; };

    bool multi_sets() const { return _sets_list.size() > 1; };
    bool multi_ways() const { return _ways_list.size() > 1; };

    Opencxx::Ptree *enable() const { return _enable; };
    Opencxx::Ptree *hit() const    { return _hit; };
    Opencxx::Ptree *hit_pred() const { return _hit_pred; };
    Opencxx::Ptree *miss() const   { return _miss; };
    Opencxx::Ptree *invalidate_line() const   { return _invalidate_line; };
    Opencxx::Ptree *read_line() const   { return _read_line; };
    Opencxx::Ptree *replace() const   { return _replace; };
    Opencxx::Ptree *line_access() const   { return _line_access; };
    Opencxx::Ptree *write_through() const   { return _write_through; };
    Opencxx::Ptree *miss_enable() const   { return _miss_enable; };
    Opencxx::Ptree *sets_func() const { return _sets_func; };
    Opencxx::Ptree *ways_func() const { return _ways_func; };

    const Cache *next() const { return _next; };

    gc_string typestr() const;
    gc_string type_enum_str() const;

    // This equivalence is defined in terms of what's needed to be able to
    // share a cache across cores.
    bool operator==(const Cache &) const;

    void finalize(Core &,MsgLog &,unsigned id);
    void dump(std::ostream &,const gc_string &pfx) const;

    static gc_string typestr(CacheType type);
    static gc_string type_enum_str(CacheType type);
    static CacheType get_type(const gc_string &n);

    // User-defined fields for each set and way in the cache.
    const CacheFieldHash &set_fields() const { return _set_fields; };
    const CacheFieldHash &way_fields() const { return _way_fields; };

  private:
    friend class CacheHandler;
    friend class ArchHandler;
    friend class Core;

    unsigned  _id;
    gc_string _orig_name; // For when performance caches are used, name of original cache.
    CacheType _type;    // Type of cache.
    unsigned _level;    // 1 == L1, etc.
    unsigned _size;     // Size of cache in bytes.
    unsigned _linesize; // Size of cache line in bytes.
    unsigned _sets;     // Max number of sets.
    unsigned _ways;     // Max set associativity of cache.

    UIntVect _sets_list,_ways_list;   // Lists of possible sets/ways.
    
    CacheFieldHash  _set_fields;      // User-defined fields for each set.
    CacheFieldHash  _way_fields;      // User-defined fields for each way.
  
    Opencxx::Ptree *_enable;          // Enable function- must evaluate to true for cache
                                      // to be on.
    Opencxx::Ptree *_hit;             // Action on a hit, if any.
    Opencxx::Ptree *_hit_pred;        // If specified, check to see if a way is a valid hit.
    Opencxx::Ptree *_miss;            // Action on a miss, if any.

    Opencxx::Ptree *_invalidate_line; // Action on a line invalidate.
    Opencxx::Ptree *_read_line;       // Action to take when a new line is needed.
    Opencxx::Ptree *_replace;         // Custom replacement algorithm function.
    Opencxx::Ptree *_line_access;     // Called for each hit/load of a line.
    Opencxx::Ptree *_sets_func;       // Function for specifying number of sets,
                                      // if multiple values are allowed.
    Opencxx::Ptree *_ways_func;       // Function for specifying number of ways,
                                      // if multiple values are allowed.
    Opencxx::Ptree *_write_through;   // Write through/back predicate (true => through).
    Opencxx::Ptree *_miss_enable;     // Miss-enable predicate (true => allow
                                      // allocation, false => skip and go to
                                      // next level of mem hierarchy).

    Cache          *_next;   // Next cache in the hierarchy.
  };

  class MemoryHook : public Base {
  public:
    MemoryHook(Opencxx::Environment *env, Opencxx::Ptree *loc, Opencxx::Ptree *hook);
  
    Opencxx::Ptree  *action() const   { return _action; }

    void finalize(Core &core, MsgLog &ml, bool is_write);
    void dump(std::ostream &,const gc_string &pfx) const;

  private:
    Opencxx::Ptree 	*_action; 
  };

  // Stores information about a local memory.
  class Memory : public Attrs, public Named {
  public:
    Memory(Opencxx::Environment *env, Opencxx::Ptree *loc, const gc_string& name);

    unsigned id() const { return _id; };

    // Is it an instruction memory 
    bool     instr_mem() const  { return _instr_mem; } 
    // Is it a prefetch instruction buffer
    bool     prefetch_mem() const { return _prefetch_mem; }
    // Memory size in bytes
    unsigned size() const  { return _size; } 
    // Addressing nit in bytes
    unsigned addr_unit() const { return _addr_unit; }
    // Parent memory - if it has one
    const Memory *parent() const { return _parent; }
    // Offset in parent memory - default 0
    unsigned offset() const { return _offset; } 
    // Called for each data read.
    MemoryHook *read_hook()  const { return _read; }
    // Called for each instruction read.
    MemoryHook *instr_read_hook()  const { return _instr_read; }
    // Called for each data write.
    MemoryHook *write_hook() const { return _write; }

    // This equivalence is defined in terms of what's needed to be able to share a
    // memory.  We require that they point to the same physical memory. Either one
    // is a parent of the other or they a common parent
    bool  operator==(const Memory &) const;

    void finalize(Core &core, MsgLog &ml,unsigned id);
    void dump(std::ostream &,const gc_string &pfx) const;
  
  protected:
    friend class MemoryHandler;
    friend class ArchHandler;
    friend class Core;

    unsigned    _id;
    bool        _instr_mem;    // Can it store insturctions.
    bool	_prefetch_mem; // Does it serve as instruction prefetch buffer 
    unsigned    _size; 	       // Size in bytes 
    unsigned    _addr_unit;    // Unit of addressing 
    gc_string _parent_name;  // Name of the parent memory
    Memory     *_parent; 	   // Pointer to parent memory, if specified
    unsigned    _offset;	   // Offset within parent memory, if specified
    MemoryHook *_read;         // Read hook, if specified
    MemoryHook *_instr_read;   // Instruction read hook, if specified
    MemoryHook *_write;        // Write hook if specified
  };

  // Stores info about a single field within an event bus.
  class EventBusField : public GenericField {
  public:
    EventBusField(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);

    gc_string signature() const;

  protected:
    friend class EventBusHandler;
    friend class EventBusFieldHandler;
  };

  // Stores information about a broadcast-bus.  This is an IPC device which allows
  // a core to transmit data to all other cores which are attached to this bus.
  class EventBus : public Attrs, public Named {
  public:
    EventBus(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);

    // This contains an encoding of the fields as a string, i.e. it's the bus's type.
    const gc_string &signature() const { return _signature; };
    const EventBusFieldHash &fields() const { return _fields; };

    const HelperInfo &action() const { return _action; };

    // This equivalence is defined in terms of what's needed to be able to
    // share a bus across cores.
    bool operator==(const EventBus &) const;

    void finalize(Core &,MsgLog &);
    void dump(std::ostream &,const gc_string &pfx) const;

  protected:
    friend class ArchHandler;
    friend class EventBusHandler;

    EventBusFieldHash _fields;

    gc_string         _signature;

    HelperInfo        _action;
  };

  // Stores information about an opaque external resource.  Basically, ADL knows
  // nothing about this resource other than that the name is legal.  This allows
  // the object to be used within action code, etc. and to be instantiated as a
  // member variable in the simulator.
  class ExtResource : public Attrs, public Named {
  public:
    ExtResource(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);

    const gc_string &objtype() const { return _objtype; };
    bool external() const { return _external; };
    bool reset() const { return _reset; };
    const PList &constr_args() const { return _constr_args; };

    bool operator==(const ExtResource &x) const;

    void finalize(Core &,MsgLog &);
    void dump(std::ostream &,const gc_string &pfx) const;

  protected:
    friend class ArchHandler;
    friend class ExtResourceHandler;

    gc_string _objtype;     // The resource's type.
    bool      _external;    // Whether to instantiate as a member variable or just a pointer.
    bool      _reset;       // Whether to reset the object or not at core reset.
    PList     _constr_args; // If not external, then constructor arguments.
  };
  
  class ArchHandler;
  // This stores information about groups.
  class Group : public Attrs, public Named {
  public:
    typedef gc_set<Opencxx::Ptree*,PtreeCmp> RawItems;
    
    Group(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);
    
    void finalize(const Core &,MsgLog &);    
    void dump(std::ostream &o,const gc_string &pfx) const;
    
    void add_item(Opencxx::Ptree* p);    
    bool has_item(const gc_string& str) const { return _items.count(str) >0 ;};
    
    bool check_parent() const { return _check_parent; };

    const StrSet& items() const { return _items;};
    unsigned size() const { return _items.size();};
    gc_string type() const { return _type;};
    
  protected: 
    friend class ArchHandler;
    friend class GroupHandler;
    friend class Core;;
    
    // Returns type of object according to its name
    gc_string get_type(const Core& core,const gc_string& str);
    // Processed items
    StrSet    _items;
    // Type of group
    gc_string _type;
    // Items to be processed
    RawItems  _raw_items;   
    // Attributes to use as a filter.
    AttrItems _attr_filter;
    StrSet    _field_filter;
    // True if we should check the parent as well, in a nested situation.
    bool      _check_parent;
    // True if its itesm were checked for validity
    bool      _checked;
  };

 
  //
  // Implementation
  //

  inline const Cache *Core::get_cache(CacheType t,unsigned level) const
  {
    return const_cast<Core*>(this)->get_cache(t,level);
  }

  inline const Cache *Core::get_first_cache(CacheType t) const
  {
    return const_cast<Core*>(this)->get_first_cache(t);
  }

  inline const Cache *Core::get_last_cache(CacheType t) const
  {
    return const_cast<Core*>(this)->get_last_cache(t);
  }

  inline const Parm *Core::get_parm(const gc_string &name) const 
  { 
    return const_cast<Core*>(this)->get_parm(name); 
  }

  inline const Context *Core::get_ctx(const gc_string &name) const 
  { 
    return const_cast<Core*>(this)->get_ctx(name); 
  }

  inline const RegBase *Core::get_regitem(const gc_string &name) const 
  { 
    return const_cast<Core*>(this)->get_regitem(name); 
  }

  inline const RegFile *Core::get_regfile(const gc_string &name) const 
  { 
    return const_cast<Core*>(this)->get_regfile(name); 
  }

  inline const Reg *Core::get_reg(const gc_string &name) const 
  { 
    return const_cast<Core*>(this)->get_reg(name); 
  }

  inline const Relocation *Core::get_reloc(const gc_string &name) const
  {
    return const_cast<Core*>(this)->get_reloc(name);
  }

  inline const PrefixCounter *Core::get_pfx_ctr(const gc_string &name) const
  {
    return const_cast<Core*>(this)->get_pfx_ctr(name);
  }

  inline const InstrField *Core::get_instrfield(const gc_string &name) const 
  { 
    return const_cast<Core*>(this)->get_instrfield(name); 
  }

  inline const SubInstr *Core::get_subinstr(const gc_string &name) const
  {
    return const_cast<Core*>(this)->get_subinstr(name);
  }

  inline const Instr *Core::get_instr(const gc_string &name) const 
  { 
    return const_cast<Core*>(this)->get_instr(name); 
  }

  inline const Exception *Core::get_exception(const gc_string &name) const 
  { 
    return const_cast<Core*>(this)->get_exception(name); 
  }

  inline const Cache *Core::get_cache(const gc_string &name) const 
  { 
    return const_cast<Core*>(this)->get_cache(name); 
  }

  inline const Memory *Core::get_memory(const gc_string &name) const 
  {
    return const_cast<Core*>(this)->get_memory(name);
  }

  inline const InstrTable *Core::get_instrtable(const gc_string &name) const //
  {
    return const_cast<Core*>(this)->get_instrtable(name);
  }
  
  
  inline const EventBus *Core::get_eventbus(const gc_string &name) const 
  {
    return const_cast<Core*>(this)->get_eventbus(name);
  }

  inline const ExtResource *Core::get_extresource(const gc_string &name) const 
  {
    return const_cast<Core*>(this)->get_extresource(name);
  }

  inline const MmuLookup *MMU::get_top_mmulookup(const gc_string &name) const 
  { 
    return const_cast<MMU*>(this)->get_top_mmulookup(name); 
  }

  inline const MmuLookup *MMU::get_any_mmulookup(const gc_string &name) const 
  { 
    return const_cast<MMU*>(this)->get_any_mmulookup(name); 
  }

}


#endif


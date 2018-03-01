//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef _UADL_PARSE_H
#define _UADL_PARSE_H

#include <map>
#include <set>
#include <vector>
#include <cassert>

#include "helpers/BasicTypes.h"
#include "helpers/gc_string.h"

#include "fe/Data.h"
#include "fe/Types.h"
#include "fe/AdlParser.h"

#include "iss/GenDriver.h"
#include "iss/GenInstrPacket.h"

#include "UadlCommon.h"
#include "ResChecker.h"

namespace uadl {

  extern const gc_string OtherOperandStr;

  extern const gc_string FetchExceptionStr;

  extern const gc_string DecodeMissStr;

  enum { FetchExceptionId = -1, DecodeMissId = -2, };

  typedef adl::Data ADL_IR;

  class UadlIr;
  class MemoryBase;
  class PrntStrSet;
  class Transaction;
  class Thread;
  class Resources;

  using adl::CacheType;

  typedef gc_map<gc_string,int> StrIntMap;

  // Stuff common to all data model objects.
  class Base : public gc {
  public:
    Base(Opencxx::Environment *env,Opencxx::Ptree *loc) : _loc(loc), _env(env) {};
    virtual ~Base() {};

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


  // A base-class for named objects.
  class NamedBase : public Base {
  public:
    NamedBase(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name) :
      Base(env,loc),
      _name(name)
    {}

    const gc_string &name() const { return _name; };

    gc_string _name;       // manager name
  };

  // Base class for all manager objects.
  class Resource : public NamedBase {
  public:
    Resource(Opencxx::Environment *env, Opencxx::Ptree *loc, const gc_string &name) :
      NamedBase(env, loc, name)
    {}

    virtual Resource *clone() const = 0;
    virtual bool isMemory() const { return false; }

  };

  // Fetch unit.
  class FetchUnit : public Resource {
  public:
    FetchUnit(Opencxx::Environment *env, Opencxx::Ptree *loc,
              const gc_string &name) :
      Resource(env, loc, name), _nEntries(0), _entrySize(0),
      _minFetchEntries(0), _maxFetchCount(std::numeric_limits<unsigned>::max()), 
      _lazyFetch(true), _flushFetch(true), _canCancel(true),
      _reuseData(false) {}

    virtual FetchUnit *clone() const;

    // access/query methods
    unsigned nEntries() const { return _nEntries; }
    unsigned entrySize() const { return _entrySize; }
    unsigned minFetchEntries() const { return _minFetchEntries; }
    unsigned maxFetchCount() const { return _maxFetchCount; };
    const std::vector<unsigned> &fetchSizes() const { return _fetchSizes; }
    bool lazyFetch() const { return _lazyFetch; }
    bool flushFetch() const { return _flushFetch; }
    bool canCancel() const { return _canCancel; }
    bool reuseData() const { return _reuseData; }
    bool interleaved() const { return _interleaved; };
    const gc_string &fetchMemory() const { return _fetchMemory; }
    const gc_string &fetchMemoryPort() const { return _fetchMemoryPort; }
    const adl::StrVect &branchPredictors() const { return _branchPredictors; }

    // Final setup/checking.
    void finalize(const adl::Core &core,const Resources &res,adl::MsgLog &ml);
    // Debugging dump.
    virtual void dump(std::ostream &,const gc_string &pfx) const;

  private:
    friend class FetchUnitHandler;

    // data members
    unsigned _nEntries;                // number of fetch entries
    unsigned _entrySize;               // fetch entry size (in bytes)
    unsigned _minFetchEntries;         // minimal number of available entries
    unsigned _maxFetchCount;           // maximum number of fetches to do in a cycle.
    // required for initiating a new fetch
    std::vector<unsigned> _fetchSizes; // descendingly sorted array of fetch request
    // size (in bytes)
    bool _lazyFetch;                // make fetch decision before issuing
    bool _flushFetch;               // flush always causes a new fetch
    bool _canCancel;                // whether can cancel pending fetch requests
    bool _reuseData;                // whether can reuse instruction buffer data upon flushing
    bool _interleaved;              // whether to interleave threads' fetches or not.
    gc_string _fetchMemory;         // instruction source memory
    gc_string _fetchMemoryPort;     // port of instruction source memory
    adl::StrVect _branchPredictors; // name of branch predictor(s) - if defined
  };

  // Scheduling types for pipeline stages:
  enum SchedulingType { 
    sDependent,          // Only executes the next instruction if the current's
                         // action returns true.
    sLockStep,           // Execute actions for all instructions first, then
                         // execute moves if all actions return true.
    sIndependent,        // Execute all instructions completely independently.
  };

  std::ostream &operator<< (std::ostream &,SchedulingType);

  class PipelineStage : public Resource {
  public:
    PipelineStage(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name) :
      Resource(env, loc, name), 
      _size(1), 
      _bandwidth(0),
      _scheduler(sDependent),
      _rigid(false),
      _issue(false),
      _shared(false),
      _index(0)
    { 
    }

    virtual PipelineStage *clone() const;

    // access/query methods
    const adl::StrVect &path() const { return _path; };
    gc_string full_name(const gc_string &sep = gc_string(".")) const;
    gc_string path_name(const gc_string &sep = gc_string(".")) const;
    unsigned size() const { return _size; }
    unsigned bandwidth() const { return _bandwidth; }
    unsigned index() const { return _index; };
    SchedulingType scheduler() const { return _scheduler; };
    bool rigid() const { return _rigid; };
    bool issue() const { return _issue; };
    bool shared() const { return _shared; };
    
    // Final setup/checking.
    void finalize(const adl::StrVect &path);
    // Debugging dump.
    virtual void dump(std::ostream &,const gc_string &pfx) const;

  private:
    friend class StageHandler;
    friend class MngrsHandler;
    friend class Resources;

    bool set_scheduler(const char *str);

    // data members
    adl::StrVect       _path;           // Full path, which includes thread hierarchy.
    unsigned           _size;           // number of pipeline stage tokens
    unsigned           _bandwidth;      // allocation bandwidth
    SchedulingType     _scheduler;      // Type of scheduling used for this stage.
    bool               _rigid;          // True => stage should stall previous stages.
    bool               _issue;          // True => this is the issue stage.
    bool               _shared;         // True => an instruction may be in
                                        // this stage and another stage at the
                                        // same time.
    unsigned           _index;          // used for report ordering.
  };

  typedef gc_map<gc_string, PipelineStage*> PipelineStages;

	typedef gc_vector<const PipelineStage *> PipeVect;

  struct StageIndexSorter {
    bool operator()(const PipelineStage *x,const PipelineStage *y) {
      return x->index() < y->index();
    }
  };


  class Semaphore : public Resource {
  public:
    Semaphore(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name) :
      Resource(env, loc, name),
      _count(1)
    {
    }

    virtual Semaphore *clone() const;
    
    unsigned count() const { return _count; };

    // Debugging dump.
    virtual void dump(std::ostream &,const gc_string &pfx) const;

  private:
    friend class SemaphoreHandler;
    
    unsigned _count;
  };

  typedef gc_map<gc_string, Semaphore*> Semaphores;

  class Flag : public Resource {
  public:
    Flag(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name) :
      Resource(env, loc, name)
    {
    }

    virtual Flag *clone() const;
    
    // Debugging dump.
    virtual void dump(std::ostream &,const gc_string &pfx) const;

  private:
    friend class FlagHandler;
  };

  typedef gc_map<gc_string, Flag*> Flags;

  // This class handles both register and register files.
  class RegisterFile : public Resource {
  public:
    RegisterFile(Opencxx::Environment *env,Opencxx::Ptree *loc, const gc_string &name,
                 bool is_reg = true) :
      Resource(env, loc, name), 
      _id(-1),
      _size(0),
      _width(0),
      _latency(0),
      _hasAlias(false),
      _is_reg(is_reg)
    {
    }

    RegisterFile(const gc_string &name,bool is_reg) :
      Resource(0, 0, name), 
      _id(-1),
      _size(0),
      _width(0),
      _latency(0),
      _hasAlias(false),
      _is_reg(is_reg)
    {
    }

    virtual RegisterFile *clone() const;

    // access/query methods
    int id() const { return _id; }
    unsigned size() const { return _size; }
    unsigned width() const { return _width; }
    unsigned latency() const { return _latency; }
    bool hasAlias() const { return _hasAlias; }
    bool is_reg() const { return _is_reg; };

    // Final setup/checking.
    void finalize(const adl::Core &core,UadlIr &ir,adl::MsgLog &ml);
    // Debugging dump.
    virtual void dump(std::ostream &,const gc_string &pfx) const;

  private:
    friend class RegFileHandler;
    friend class MngrsHandler;

    // data members
    int   _id;       // register file id (from ADL IR)
    unsigned _size;     // number of registers
    unsigned _width;    // register bit width
    unsigned _latency;  // read after write latency
    bool  _hasAlias;
    bool  _is_reg;   // True => register, False => register-file.
  };

  typedef gc_map<gc_string, RegisterFile*> RegisterFiles;

  class ForwardingPath : public Resource {
  public:
    ForwardingPath(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name) :
      Resource(env, loc, name),
      _broadcast(false),
      _size(1),
      _width(0)
    {
    }

    virtual ForwardingPath *clone() const;

    // access/query methods
    bool broadcast() const { return _broadcast; };
    unsigned size() const { return _size; }
    unsigned width() const { return _width; }

    // Final setup/checking.
    void finalize(const adl::Core &core,adl::MsgLog &ml);
    // Debugging dump.
    virtual void dump(std::ostream &,const gc_string &pfx) const;
  private:
    friend class FwdPathHandler;

    bool     _broadcast;
    unsigned _size;
    unsigned _width;    
  };

  typedef gc_map<gc_string,ForwardingPath *> ForwardingPaths;

  // Base class used for items which can have custom algorithms. 
  struct CustomItem : public Resource {
    CustomItem(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name) :
      Resource(env, loc, name) 
    {
    }

    const gc_string &header() const { return _header; };
    const gc_string &path() const { return _path; };
    const adl::PMap &parms() const { return _parms; };
    const ResInfo   &args() const { return _args; };

    bool hasPreCycle()  const { return _args._hasPreCycle; };
    bool hasPostCycle() const { return _args._hasPostCycle; };
    bool hasPostSim()   const { return _args._hasPostSim; };

    bool is_template() const { return !_args._tmpl_args.empty(); };

    void dump(std::ostream &o,const gc_string &pfx) const;
  protected:
    friend class CustomArgHandler;

    void handleArgChecking(UadlIr &ir,adl::MsgLog &ml,const gc_string &type,
                           const adl::StrSet &default_tmpls,const adl::StrSet &default_constrs,
                           bool full_path_errors);
    void checkArgs(ArgItems::iterator iter,ArgItems::iterator end,const adl::StrSet &defaults,adl::MsgLog &ml);

    gc_string   _header;   // Stores the name of the header.
    gc_string   _path;     // Path to header.  If empty, implies current directory.
    adl::PMap   _parms;    // All parms for this resource, stored in a raw format.
    ResInfo     _args;     // Information about the class's argument (if checked) and other stuff.
  };

  // A custom resource is a user-defined resource.  In the front-end, we just
  // collect up parameters and let the back-end figure out if they're
  // constructor or template parameters.
  class CustomResource : public CustomItem {
  public:
    CustomResource(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name) :
      CustomItem(env, loc, name) 
    {
    }

    virtual CustomResource *clone() const;

    const gc_string &type() const { return _type; };

    // Final setup/checking.
    void finalize(const adl::Core &core,UadlIr &ir,adl::MsgLog &ml,
                  bool check_args,bool full_path_errors);
    // Debugging dump.
    virtual void dump(std::ostream &,const gc_string &pfx) const;

  private:
    friend class CustomHandler;
    friend class CustomArgHandler;

    gc_string   _type;     // Stores the type name.
  };

  typedef gc_map<gc_string, CustomResource*> CustomResources;

  // TODO: use an enum algorithm type, add an enable predicate, add support for
  // user-defined algorithms.
  class BranchPredictor : public CustomItem {
  public:
    BranchPredictor(Opencxx::Environment *env, Opencxx::Ptree *loc,
                    const gc_string &name) :
      CustomItem(env, loc, name), 
      _custom(false),
      _size(0), 
      _counterWidth(0) {}

    virtual BranchPredictor *clone() const;

    // access/query methods
    bool custom() const { return _custom; };
    const gc_string &algorithm() const { return _algorithm; }
    unsigned size() const { return _size; }
    unsigned counterWidth() const { return _counterWidth; }
    const adl::PAuxFunc &enable() const { return _enable; }

    // For extra comparison tags.
    struct RegCompare {
      RegCompare(const gc_string &r, const gc_string &f = "") :
        _reg(r), _field(f), _rb(0), _rf(0) {}

      gc_string          _reg;
      gc_string          _field;
      const adl::RegBase  *_rb;
      const adl::RegField *_rf;
    };

    typedef gc_vector<RegCompare> RegCompares;

    bool has_reg_compares() const { return !_reg_compares.empty(); }
    const RegCompares &reg_compares() const { return _reg_compares; }

    // Final setup/checking.
    void finalize(adl::Core &,UadlIr &ir,adl::MsgLog &ml,bool check_args,bool full_path_errors);
    // Debugging dump.
    virtual void dump(std::ostream &,const gc_string &pfx) const;

  private:
    friend class BranchPredHandler;

    bool              _custom;
    gc_string         _algorithm;
    unsigned          _size;
    unsigned          _counterWidth;
    adl::PAuxFunc     _enable;

    RegCompares       _reg_compares;
  };

  std::ostream &operator<<(std::ostream &o, const BranchPredictor::RegCompare &r);

  typedef gc_map<gc_string, BranchPredictor*> BranchPredictors;



  // Memory port used by MemoryBase class and its derived classes
  class MemoryPort : public Resource {
  public:
    MemoryPort(Opencxx::Environment *env, Opencxx::Ptree *loc,
               const gc_string &name) :
      Resource(env, loc, name), 
      _type(adl::UnifiedCache), 
      _commonBandwidth(false),
      _addrBandwidth(1), 
      _dataBandwidth(1),
      _readLatency(0),
      _writeLatency(0),
      _maxRequest(0),
      _allowBusWait(true),
      _id(0) 
    {}

    MemoryPort(const gc_string &name, bool commonBandwidth, unsigned addrBandwidth, 
               unsigned dataBandwidth, unsigned readLatency,unsigned writeLatency) : 
      Resource(0, 0, name), 
      _type(adl::UnifiedCache), 
      _commonBandwidth(commonBandwidth),
      _addrBandwidth(addrBandwidth),
      _dataBandwidth(dataBandwidth), 
      _readLatency(readLatency),
      _writeLatency(writeLatency),
      _maxRequest(0),
      _allowBusWait(true),
      _allowBusWaitSpecified(false),
      _id(0) 
    {}

    virtual MemoryPort *clone() const;

    CacheType type() const { return _type; };
    bool     commonBandwidth() const { return _commonBandwidth; };
    unsigned addrBandwidth() const { return _addrBandwidth; }
    unsigned dataBandwidth() const { return _dataBandwidth; }
    unsigned readLatency() const { return _readLatency; };
    unsigned writeLatency() const { return _writeLatency; };
    unsigned maxRequest() const { return _maxRequest; };
    bool     allowBusWait() const { return _allowBusWait; };
    unsigned id() const { return _id; }

    void finalize(unsigned id,unsigned maxRequest,bool pAllowBusWait);

    // Debugging dump
    virtual void dump(std::ostream &, const gc_string &pfx) const;

  private:
    friend class MemPortHandler;
    friend class MemoryBase;

    CacheType _type;           // We use 'cache' type here for a port type (whether
                               // the port is for instructions or data), but this
                               // doesn't mean that it requires the use of a cache.
                               // It's just an existing enum that we re-use.
    bool     _commonBandwidth; // If true, then the same bandwidth is consumed by
                               // both address and data.
    unsigned _addrBandwidth;
    unsigned _dataBandwidth;
    unsigned _readLatency;
    unsigned _writeLatency;
    unsigned _maxRequest;
    bool     _allowBusWait;
    bool     _allowBusWaitSpecified;
    unsigned _id;
  };

  std::ostream &operator<< (std::ostream &o,const MemoryPort *mp);

  typedef gc_map<gc_string, MemoryPort*> MemoryPorts;



  /*******************************************************************************
   * common base class for Memory and Cache
   *
   * TODO:
   *   - change _allowBusWait default to false when zen650 model adapts it
   *   - should _preemptive be a port attribute?
   *   - change parameter "max_request" to "queue_size"
   ******************************************************************************/
  class MemoryBase : public Resource {
  public:
    MemoryBase(Opencxx::Environment *env, Opencxx::Ptree *loc,
               const gc_string &name) :
      Resource(env, loc, name), _addrBandwidth(1), _dataBandwidth(1),
      _dataWidth(32), _maxRequest(1), _allowBusWait(true), _preemptive(false),
      _defaultPort(NULL) {}

    ~MemoryBase() { delete _defaultPort; }

    // access/query methods
    virtual bool isMemory() const { return true; }
    bool isCache() const;

    static const gc_string &defaultPortName();

    unsigned addrBandwidth() const { return _addrBandwidth; }
    unsigned dataBandwidth() const { return _dataBandwidth; }
    unsigned dataWidth() const { return _dataWidth; }
    unsigned maxRequest() const { return _maxRequest; }
    bool allowBusWait() const { return _allowBusWait; }
    bool preemptive() const { return _preemptive; }
    void disablePreemptive() { _preemptive = false; }

    const MemoryPort *findPortByName(const gc_string &name) const;
    const MemoryPorts &ports() const { return _ports; }
    MemoryPort *defaultPort();
    const MemoryPort *defaultPort() const;

    // Final setup/checking.
    void finalize(adl::MsgLog &ml);
    // Debugging dump.
    virtual void dump(std::ostream &,const gc_string &pfx) const;

  protected:
    friend class MemBaseHandler;

    // data members
    unsigned _addrBandwidth;
    unsigned _dataBandwidth;
    unsigned _dataWidth;   // data bus bit width
    unsigned _maxRequest;
    bool _allowBusWait; // allow a request to wait on the bus if queue is full
    bool _preemptive;   // can load/store preempt bus-waiting fetch?

    MemoryPorts _ports;

  private:
    MemoryPort *_defaultPort;
  };

  std::ostream &operator<< (std::ostream &,const MemoryBase *);

  // inline methods
  inline const gc_string &MemoryBase::defaultPortName() 
  { static gc_string name = "__default_Port_Name__"; return name; }



  class Memory : public MemoryBase {
  public:
    Memory(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name)
      : MemoryBase(env, loc, name), _readLatency(1), _writeLatency(1), 
        _readOnly(false) {}

    virtual Memory *clone() const;

    // access/query methods
    unsigned readLatency() const { return _readLatency; }
    unsigned writeLatency() const { return _writeLatency; }
    bool readOnly() const { return _readOnly; }

    // Final setup/checking.
    void finalize(const adl::Core &core, adl::MsgLog &ml);
    // Debugging dump.
    virtual void dump(std::ostream &,const gc_string &pfx) const;

  private:
    friend class MemHandler;

    // data members
    unsigned _readLatency;
    unsigned _writeLatency;
    bool _readOnly;
  };

  typedef gc_map<gc_string, Memory *> Memories;



  class Cache : public MemoryBase {
  public:
    Cache(Opencxx::Environment *env, Opencxx::Ptree *loc, const gc_string &name,
          const adl::Cache *IssCache = NULL) :
      MemoryBase(env, loc, name),
      _IssCache(IssCache) {}

    virtual Cache *clone() const;

    // access/query methods
    const gc_string &nextLevelMemory() const { return _memory; }
    const gc_string &nextLevelMemoryPort() const { return _memoryPortName; }

    bool custom_model() const { return !_custom_class.empty(); };
    const gc_string &custom_class() const { return _custom_class; };
    const gc_string &custom_header() const { return _custom_header; };

    const ConfigMap &config() const { return _config; };

    const adl::PAuxFunc &linefillWidth() const { return _linefillWidth; }

    const adl::PAuxFunc &storeBufEnable() const { return _storeBufEnable; }

    const adl::Cache *IssCache() const { return _IssCache; }

    // Final setup/checking.
    void finalize(adl::Core &core,const Resources &res,const UadlIr &ir,adl::MsgLog &ml);
    // Debugging dump.
    virtual void dump(std::ostream &,const gc_string &pfx) const;

  private:
    friend class CacheHandler;
    friend class Resources;

    // data members
    gc_string _memory;           // next-level memory
    gc_string _memoryPortName;   // next-level memory port name

    gc_string _custom_class;     // Name of the custom class, if applicable.
    gc_string _custom_header;    // Name of the custom header, if applicable.
    
    ConfigMap _config;           // Stores configuration key/value pairs.

    adl::PAuxFunc _linefillWidth;
    
    adl::PAuxFunc _storeBufEnable;

    const adl::Cache *_IssCache;
  };

  typedef gc_map<gc_string, Cache*> Caches;


  // This item binds a state in an instruction class to a pipeline stage.  The
  // exact pipeline stage is thread-specific, hence the need to supply a Thread
  // object when retrieving a stage object.
  class StateBinding : public gc {
  public:
    StateBinding(const gc_string &state, const gc_string &stagen) :
      _state(state), 
      _stagen(stagen) 
    {}

    const gc_string   &state() const { return _state; };
    const gc_string   &stagen() const { return _stagen; };
    const PipelineStage *stage(const Thread &t) const;

  private:
    friend class InstClass;

    gc_string   _state;
    gc_string   _stagen;
  };

  class Machine;

  class StateBindings : public Base 
  {
  public:
    StateBindings(Opencxx::Environment *env,Opencxx::Ptree *loc) :
      Base(env,loc)
    {}

    const gc_string *getStage(const gc_string &state) const;
    const gc_string *getStage(const gc_string *state) const;

    const PipelineStage *getStage(const Thread &thread,const gc_string &state) const;
    const PipelineStage *getStage(const Thread &thread,const gc_string *state) const;

    // Final setup/checking.
    void finalize(UadlIr &ir,adl::MsgLog &ml,const Machine &machine);
    // Debugging dump.
    virtual void dump(std::ostream &,const gc_string &pfx) const;

  private:
    friend class MappingHandler;

    typedef gc_vector<StateBinding *> Bindings;

    Bindings _bindings;

    // Temporary storage during parsing.
    gc_vector<std::pair<gc_string,gc_string> > _tmp;
  };

  class Machine : public NamedBase {
  public:
    Machine(Opencxx::Environment *env,Opencxx::Ptree *loc, const gc_string &name) :
      NamedBase(env,loc,name),
      _stateBindings(0)
    {}

    // query methods
    unsigned nStates() const { return _states.size() + 1; }

    const adl::StrSet &states() const { return _states; };

    const gc_string &initState() const { return _initState; };

    // Has a state- initial or other.
    bool hasState(const gc_string &stateName) const { 
      return hasRealState(stateName) || stateName == _initState; 
    }

    // Has state and not the initial state.
    bool hasRealState(const gc_string &stateName) const { return _states.count(stateName); };

    // Default state bindings.  May be overridden by an instruction class.
    const StateBindings *stateBindings() const { return _stateBindings; };

    // Return the corresponding exception state for the supplied state.  Returns
    // an empty string if not found.
    gc_string getExceptionState(const gc_string &s) const;

    // Returns true if a state is an exception state.
    bool isExceptionState(const gc_string &s) const;

    // Final setup/checking.
    void finalize(UadlIr &ir,adl::MsgLog &ml);
    // Debugging dump.
    virtual void dump(std::ostream &,const gc_string &pfx) const;

    // For use creating machines by back-ends which need special purpose machines.
    void setInitState(const gc_string &i);
    void addState(const gc_string &s);

  private:
    friend class MachineHandler;
    friend class ExceptionMappingHandler;
    friend class InstClass;

    StateBindings *stateBindings() { return _stateBindings; };

    gc_string      _initState;
    adl::StrSet    _states;
    adl::StrStrMap _emapping;    // Mapping of states to corresponding exception
                                 // states.

    // For the default mapping, if used.
    StateBindings  *_stateBindings;
  };

  typedef gc_map<gc_string,Machine *> Machines;

  // Resource arguments for transactions are stored in vector.  If the first
  // element is null, then it's considered to mean that all registers should be
  // handled before the other managers are processed.
  struct MgrVect : public gc_vector<const Resource *>
  {
    typedef gc_vector<const Resource *> Base;

    MgrVect(size_type n,const Base::value_type &v) : Base(n,v) {};
    MgrVect() {};

    bool is_allreg() const { return !empty() && front() == 0; };
  };

  class InstClass;

  typedef std::set<int> IntSet;
  typedef gc_set<const PipelineStage *> StageSet;
  typedef gc_set<const Semaphore *> SemaphoreSet;
  typedef gc_set<const MemoryBase *> MemSet;

  typedef gc_map<gc_string,unsigned> OprndValues;

  enum OperandType { opConst, opImm, opReg };

  // Stores operands indexed to instruction fields.
  struct Operand : public gc {
    Operand() : _type(opReg), _s_u(0), _t_u(0) {};
    Operand(const adl::UsedReg *s_u,const adl::UsedReg *t_u,const rnumber::RNumber &mask) : _type(opReg), _s_u(s_u), _t_u(t_u), _mask(mask) {};
    Operand(const gc_string &f,const rnumber::RNumber &mask) : _type(opImm), _s_u(0), _t_u(0), _f(f), _mask(mask) {};
    Operand(const rnumber::RNumber &v) : _type(opConst), _s_u(0), _t_u(0), _mask(v) {};

    bool is_imm() const { return _type == opImm; };
    bool is_const() const { return _type == opConst; };

    bool is_src() const { return _s_u || !_f.empty(); };
    bool is_trg() const { return _t_u; };

    unsigned value() const { return _mask.uint32(); };

    // Returns source or target.  If we just want access to the resource, then
    // this is fine, since this will only vary in terms of mask.
    const adl::UsedReg &ur() const { return (_s_u) ? *_s_u : *_t_u; };

    bool eq_no_mask(const Operand &x) const {
      return _s_u == x._s_u &&
        _t_u == x._t_u &&
        _f == _f;
    }

    // For our purposes here, we only care if masks exist, not what they are.
    bool operator< (const Operand &x) const {
      if (_s_u && !x._s_u) return true;
      if (!_s_u && x._s_u) return false;
      if (_t_u && !x._t_u) return true;
      if (!_t_u && x._t_u) return false;
      if (is_const() && !x.is_const()) {
        return true;
      }
      if (is_const() && x.is_const() && _mask < x._mask) {
        return true;
      }
      if (is_imm() && x.is_imm() && _f < x._f) {
        return true;
      }
      if (_s_u && x._s_u && compare_lt_if_mask(*_s_u,_mask,*x._s_u,x._mask)) {
        return true;
      } 
      else {
        return (_t_u && x._t_u && compare_lt_if_mask(*_t_u,_mask,*x._t_u,x._mask));
      }
    }

    OperandType         _type;   // Operand type, e.g. constant-value, immediate, or register/register-file.
    const adl::UsedReg *_s_u;    // The source resource in question, if a source.
    const adl::UsedReg *_t_u;    // The target resource in question, if a target.
    gc_string           _f;      // If this is just for an immediate, this is
                                 // the field's name.
    rnumber::RNumber    _mask;   // A source-mask, if relevant.

  };

  std::ostream &operator<< (std::ostream &,const Operand &);

  typedef gc_vector<Operand> OpVect;
  typedef gc_map<gc_string,OpVect> Operands;
  typedef adl::StrStrMap Functions;

  struct InstItem : public gc {
    gc_string         _name;     // The name of the instruction.
    const adl::Instr *_instr;    // Pointer to the actual instruction object, if valid. 
    OprndValues       _opvalues; // List of operand/value constraints, if any.
    Operands          _operands; // Mapping of operands to used-register
                                 // elements in the instruction.  At the end of
                                 // processing, if an operand is not in this
                                 // map, then it means that the instruction
                                 // doesn't have that operand.
    Functions         _funcs;    // Mapping of interface function names to
                                 // actual helper functions.

    InstItem(const gc_string &name) : _name(name), _instr(0) {};

    const OpVect* getOperand(const gc_string &oname) const;

    const gc_string *getFunc(const gc_string &fname) const;

    bool has_opvs() const { return !_opvalues.empty(); };

    void dump(std::ostream &o,const gc_string &pfx) const;
  };

  typedef gc_list<InstItem> InstList;

  std::ostream &operator<<(std::ostream &,const InstItem &);
  std::ostream &operator<<(std::ostream &o,const InstList &sl);

  gc_string getInstrClassEnumTypeName();
  gc_string getInstrClassAttrTypeName();

  // Stores information about action code associated with a particular state.
  class Action : public gc {
	public:
    Action(const gc_string &state) : 
      _state(state), _has_possible_excpt(false), 
      _has_exec(false), _has_flush(false), _action(0), _loc(0) {};

    Opencxx::Ptree *loc() const { return _loc; };

    const gc_string &state() const { return _state; };
    const adl::StrSet &to_states() const { return _to_states; };
    const adl::StrSet &from_states() const { return _fr_states; };
    const adl::PVect &repeats() const { return _repeats; };
    bool has_possible_excpt() const { return _has_possible_excpt; };
    bool has_exec() const { return _has_exec; };
    bool has_flush() const { return _has_flush; };
    bool has_repeats() const { return !_repeats.empty(); };
    Opencxx::Ptree *action() const { return _action; };    

    void finalize(const UadlIr &ir,const InstClass &ic,const adl::Core &core,adl::MsgLog &ml);
    void dump(std::ostream &o,const gc_string &pfx) const;
    void display() const;

  protected:
    friend class ActionWalker;
    friend class InstClass;

    void set_action(Opencxx::Ptree *a) { _loc = _action = a; };

    const gc_string     _state;               // Action holder (state).
    adl::StrSet         _to_states;           // Set of states this action can transition to.
    adl::StrSet         _fr_states;           // Cumulative total states which transition to this one.
		adl::StrSet         _sc_checks;           // Set of stages checked for capacity.
    bool                _has_possible_excpt;  // True => action code calls exec(), reads operands, etc.
    bool                _has_exec;            // True => action code calls exec().
    bool                _has_flush;           // True => action code calls a flush function.
    adl::PVect          _repeats;             // Statements to repeat for
                                              // lock-step actions.
    Opencxx::Ptree     *_action;              // Action code.
    Opencxx::Ptree     *_loc;                 // Location in orginal file- used for error handling.
  };

  class Actions : public gc_map<gc_string,Action *> {
	public:
    void finalize(const UadlIr &ir,const InstClass &ic,const adl::Core &core,adl::MsgLog &ml);
    void dump(std::ostream &o,const gc_string &pfx) const;
	};

  // For storing information about what method calls are made in an instruction
  // class.  Maps method -> num-args.
  typedef gc_map<gc_string,gc_set<int> > MethodCalls;

  // TODO: see if IR parameter can be removed
  class InstClass : public NamedBase {
  public:
    InstClass(Opencxx::Environment *env, Opencxx::Ptree *loc,
              const gc_string &name) :
      NamedBase(env, loc, name), _temp_machine(0), _machine(0),
      _stateBindings(0), _action_block(0), 
      _allow_missing_operands(false), _allow_extra_operands(true),
      _has_opvs(false), _uses_mem(false), _has_exceptions(false), 
      _has_branch(false), _all_branches(false), _has_default_instr(false),
      _commit_on_exception(tNotSet)
    {}

    // Type name to be used in generation.
    gc_string getTypeName() const;

    // access/query methods
		bool isInitState(const gc_string &state) const;
    const gc_string *findStageBindingName(const gc_string &state) const;
    const PipelineStage *findStageBinding(const Thread &thread,const gc_string &state) const;

    const Machine &machine() const { return *_machine; };

    const StateBindings &stateBindings() const { assert(_stateBindings); return *_stateBindings; };

    const InstList &instrs() const { return _instrs; };
    const InstItem *getInstr(const gc_string &name) const;
    const IntSet &instIdSet() const { return _instIds; }
    bool has_opvs() const { return _has_opvs; }

    // True if this class contains the fetch exception pseudo-instruction.
    bool hasFetchException() const { return _instIds.count(FetchExceptionId); };
    // True if this class only represents the fetch exceptin.
    bool onlyFetchException() const { return hasFetchException() && _instIds.size() == 1; };

    // True if this class contains the decode-miss pseudo-instruction.
    bool hasDecodeMiss() const { return _instIds.count(DecodeMissId); };
    // True if this class only represents the decode-miss pseudo-instruction..
    bool onlyDecodeMiss() const { return hasDecodeMiss() && _instIds.size() == 1; };

    // True if this class is the default for handling any instructions not
    // listed anywhere else.
    bool hasDefaultInstr() const { return _has_default_instr; };

    const adl::StrSet &operands() const { return _operands; };
    bool is_operand(const gc_string &o) const { return _operands.count(o); };
    bool is_operand(Opencxx::Ptree *o) const;

    const adl::StrSet &funcs() const { return _funcs; };
    bool is_func(const gc_string &o) const { return _funcs.count(o); };
    bool is_func(Opencxx::Ptree *o) const;

    bool commit_on_exception() const { return (_commit_on_exception == tTrue) ? true : false; };

    // Returns true if any instructions access memory or caches.
    bool uses_mem() const { return _uses_mem; };

    // True if any instructions explicitly invoke exceptions.
    bool has_instr_exceptions() const { return _has_exceptions; };

    // True if any instructions access memory or caches or explicitly invokes
    // exceptions.
    bool has_exceptions() const { return _uses_mem || _has_exceptions; };

    // True if any instructions can branch.
    bool has_branch() const{ return _has_branch; };

    // True if all instructions can branch.
    bool all_branches() const{ return _all_branches; };

    // Checks to see if a method call with the specified number of arguments
    // exists in the instruction class's action code
    bool check_func_call(const gc_string &func,int num_args) const;
    // Checks to see if the function call exists, for any given number of
    // arguments.  Returns the argument set, or 0 if not found.
    const gc_set<int> *check_func_call(const gc_string &func) const;

    const Actions &actions() const { return _actions; };

    const Action *getAction(const gc_string &name) const { return const_cast<InstClass&>(*this).getAction(name); };

    // Get all attributes.
    const adl::StrSet &get_attrs() const { return _attrs; };

    // Get an attribute's integer value.  Returns -1 if the attribute is not valud.
    static int get_attr_value(const gc_string &a);

    // Access all registered attributes.
    static const adl::IntMap &get_ic_attrs();

    // Checks to make sure that a specified instruction field exists for all
    // instructions in this class.  If an override is specified, then this only
    // checks that the field is valid, not whether it exists for all
    // instructions.  If an error occurs, and a message log is supplied, an error
    // is added to the log.
    // Returns true on success,false on error.
    bool check_instr_field(const adl::Core &core,adl::MsgLog *ml,Opencxx::Ptree *loc,
                           const gc_string &field,bool missing_override = false,
                           PrntStrSet *allows = 0) const;

    // Processing of instructions contained by the class.
    void process_instr_membership(UadlIr &ir,adl::MsgLog &ml);
    // Final setup/checking.
    void finalize(UadlIr &ir,adl::MsgLog &ml);
    // Debugging dump.
    virtual void dump(std::ostream &,const gc_string &pfx) const;

  private:
    friend class InstClassHandler;
    friend class OperandHandler;
    friend class FuncHandler;
    friend class ActionWalker;
    friend class UadlIr;

    Action *addAction(Opencxx::Ptree *id);
    Action *getAction(const gc_string &name);

    void add_attr(Opencxx::Ptree *loc,const gc_string &attr);
    static void finalize_attrs();

    void add_func_call(const gc_string &func,int num_args);

    void parseActionBlock(UadlIr &ir,adl::Core &core,adl::MsgLog &ml);
    void processOperands(UadlIr &ir,const adl::Core &core,adl::MsgLog &ml);
    void processFuncs(UadlIr &ir,const adl::Core &core,adl::MsgLog &ml);
    void processFromStates(const gc_string &state,StrIntMap from_states,int depth);
    void checkToStates(adl::MsgLog &ml);

    Opencxx::Ptree *_temp_machine;
    const Machine  *_machine;

    InstList        _instrs;      // List of instructions in this class.  May list
                                  // just an instruction, or additional operand
                                  // value constraints.

    adl::StrSet    _instr_attrs;  // Attributes defining class' instructions
    adl::StrSet    _attrs;        // Attributes associated with this class. 

    StateBindings  *_stateBindings;

    adl::PVect      _action_block; // Stores initial action block(s) before
                                   // processing.  It's a vector so that defmods
                                   // can override on a per-state basis.
    Actions         _actions;

    IntSet         _instIds;      // Set of valid instruction (ISA) id's.  -1
                                  // represents the fetch-exception
                                  // pseudo-instruction.
    adl::StrSet     _operands;    // Lists valid operand names.
    adl::StrSet     _funcs;       // Lists valid interface functions.

    // Just used during initial parsing.  Outer hash indexes operand groups by
    // instruction name.  The empty string stores the default operand group.
    typedef gc_map<gc_string,gc_vector<std::pair<gc_string,Opencxx::Ptree *> > > TmpOperands;
    TmpOperands     _tmp_ops, _tmp_funcs;

    bool _allow_missing_operands; // If true, we allow instructions in instruction classes
                                  // to not have certain operands.  A dummy, null-operand is
                                  // used instead.

    bool _allow_extra_operands;   // If true, then a listed operand need not be
                                  // used by all instructions in the class.

    bool _has_opvs;               // Has operand value constraints?

    bool _uses_mem;               // Any instruction touch caches or memory?

    bool _has_exceptions;         // Any instructions explicitly raise exceptions?

    bool _has_branch;             // Any instructions branch?

    bool _all_branches;           // All instructions branches?

    bool _has_default_instr;      // True if this contains the special
                                  // "default-instruction" pseudo-instruction.

    TriBool _commit_on_exception; // Commit exception and flush when it occurs.

    MethodCalls _func_calls;      // Stores information on what method calls are
                                  // made in the instruction class.

  };

  typedef gc_map<gc_string,InstClass *> InstClasses;

  class Resources : public gc {
  public:
    Resources();

    // Perform a deep copy on all resources.
    void copy(Resources &new_res) const;

    const Resource *findResourceByName(const gc_string &name) const;
    const PipelineStage *findStageByName(const gc_string &name) const;
    const Semaphore *findSemaphoreByName(const gc_string &name) const;
    const Flag *findFlagByName(const gc_string &name) const;
    const RegisterFile *findRegFileByName(const gc_string &name) const;
    const ForwardingPath *findFwdPathByName(const gc_string &name) const;
    const BranchPredictor *findBranchPredictorByName(const gc_string &name) const;
    const CustomResource *findCustomResourceByName(const gc_string &name) const;
    const Cache *findCacheByName(const gc_string &name) const;
    const Memory *findMemoryByName(const gc_string &name) const;

    const FetchUnit *fetcher() const { return _fetcher; }
    const PipelineStage *issueStage() const { return _issueStage; }
    const PipelineStages &pipelineStages() const { return _pipelineStages; };
    const Semaphores &semaphores() const { return _semaphores; }
    const Flags &flags() const { return _flags; }
    const RegisterFiles &registerFiles() const { return _registerFiles; }
    const ForwardingPaths &forwardingPaths() const { return _forwardingPaths; };
    const BranchPredictors &branchPredictors() const { return _branchPredictors; };
    const CustomResources &customResources() const{ return _customResources; };
    const gc_vector<const Cache*> &sortedCaches() const { return _sortedCaches; }
    const Caches &caches() const { return _caches; }
    const Memories &memories() const { return _memories; }

    void finalize(UadlIr &ir,Thread &thread,const adl::System *sys,adl::Core *core,const Resources *parent_res,
                  adl::MsgLog &ml,const adl::StrVect &path);
    void import_managers(const adl::System *sys, adl::Core *core, const Resources *parent_res, adl::MsgLog &ml);
    void finalize_managers(UadlIr &ir,Thread &thread,adl::Core *core,adl::MsgLog &ml);
    void dump(std::ostream &o,const gc_string &pfx) const;

  protected:
    friend class UadlIr;
    friend class MngrsHandler;

    void add_imported_reg(const gc_string &name,bool is_reg);
    void add_imported_cache(const gc_string &name,const adl::Cache *adl_cache);
    void add_imported_memory(const gc_string &name);

    FetchUnit            *_fetcher;           // instruction fetch unit
    const PipelineStage  *_issueStage;        // the issue stage for this pipeline
    PipelineStages        _pipelineStages;
    Semaphores            _semaphores;
    Flags                 _flags;
    RegisterFiles         _registerFiles;
    ForwardingPaths       _forwardingPaths;
    BranchPredictors      _branchPredictors;
    CustomResources       _customResources;
    Caches                 _caches;
    gc_vector<const Cache*> _sortedCaches;
    Memories               _memories;
  };

  typedef gc_map<gc_string,Thread *> Threads;

  // A thread simply stores resources known by an ADL core, plus a pointer to
  // the ADL core.  It can also contain sub-threads.
  class Thread : public NamedBase {
  public:
    Thread(Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);

    // This clones all resources but does not copy over any child threads.
    Thread *clone(const gc_string &name) const;

    const gc_string &type() const { return _type; };

    const Thread *parent() const { return _parent; };
    const Resources &res() const { return _res; };
    const Threads &threads() const { return _threads; };

    bool is_group() const { return _system; };

    const adl::SysItem &item() const;
    adl::Core          *core() const { return _core; };
    const adl::System  *sys() const { return _system; };

    const MemoryBase *firstLevelDataMem() const;
    const MemoryBase *firstLevelInstMem() const;
    const MemoryPort *firstLevelInstMemPort() const;
    const Memory *lastLevelInstrMem() const;

    // This just returns a core from a consistuent thread.  Currently, we assume
    // a symmetric multi-threading model, from a functionality point of view, so
    // any core will do.
    adl::Core *get_core() const;

    void finalize(UadlIr &ir,const Resources *parent_res,adl::MsgLog &ml,adl::StrVect path);
    void import_managers(const Resources *parent_res,adl::MsgLog &ml);
    void finalize_managers(UadlIr &ir,adl::MsgLog &ml);
    virtual void dump(std::ostream &,const gc_string &pfx) const;
  protected:
    friend class UadlIr;
    friend class ThreadHandler;
    friend class CoreHandler;

    gc_string             _type;
    const Thread         *_parent;
    adl::Core            *_core;    // If a leaf-node, this is the pointer to the ADL core.
    const adl::System    *_system;  // If a thread-group, a pointer to the system object.
    Resources             _res;     // Resources in the thread.
    Threads               _threads; // Any sub-threads (for thread groups).
  };

  // The uADL data model.
  class UadlIr : public Base {
  public:
    UadlIr(ADL_IR &iss_ir,adl::GenDriverIF *iss_driver,
           const gc_string &iss_hfilename, 
           const gc_string &uArchHFilenamem,
           bool check_cr_args,bool full_paths);

    virtual ~UadlIr() {};

    const ADL_IR &archIR() const { return _iss_ir; }
    const adl::InstrSlotAlloc &get_slots(const gc_string &instrname) const;
    bool hasExceptions() const;
    bool allow_unimpl_instrs() const { return _allow_unimpl_instrs; };
    const adl::PAuxFunc &mem_access() const { return _mem_access; };
    Opencxx::Ptree *branch_hook() const { return _branch_hook; };

    unsigned maxRegFileId() const { return _maxRegFileId; }
    unsigned maxRegWidth() const { return _maxRegWidth; }
    unsigned maxOperands() const { return _maxOperands; }
    unsigned maxInstrSize() const { return _maxInstrSize; }
    unsigned minInstrSize() const { return _minInstrSize; }
    unsigned instrSizeGCD() const { return _instrSizeGCD; }

    const gc_string &CIAName() const;
    const gc_string &NIAName() const;

    static bool full_paths() { return _full_paths; };

    // uarch access/query methods
    const gc_string &archName() const { return _archName; }
    const gc_string &uArchName() const { return _uArchName; }

    unsigned deadLockTimeOut() const { return _timeOut; }
    void setDeadLockTimeOut(unsigned timeOut) { _timeOut = timeOut; }

    // This returns a reference to the first core of the first thread.
    const adl::Core &core() const;
    adl::Core &core();

    // This returns the resources for a thread (leaf-node), which will consist
    // of an instance of everything visible in the system.
    const Resources &res() const { return _thread->_res; };
    // Just the top-level resources (those shared amongst all threads).
    const Resources &top_res() const { return _top->_res; };
    // Reference to the first child node (thread).
    const Thread &thread() const { return *_thread; };
    // Reference to the top-level thread or thread-group.
    const Thread &top() const { return *_top; };

		const PipeVect &sortedStages() const { return _sortedStages; };

    unsigned maxMemId() const;

    const Machines &machines() const { return _machines; }
    const InstClasses &instClasses() const { return _instClasses; }

    const InstClass &first_class() const { assert(!_instClasses.empty()); return *(_instClasses.begin()->second); };

    // Access to helper functions.
    const adl::HelperMap &helpers() const { return _helpers; };

    const adl::HelperInfo *get_helper(const gc_string &name) const;

    // Access to other type declarations.
    const adl::PList &type_decls() const { return _type_decls; };

    bool isHelper(const gc_string &name) const;
    bool isCoreHelper(const gc_string &name) const;

    void check_operand_access(adl::MsgLog&,Opencxx::Ptree*);

    // If any instruction classes query operand values.
    bool has_value_queries() const;

    const Thread* findThreadByName(const gc_string &name) const;
    const Machine *findMachineByName(const gc_string &machineName) const;
    const InstClass *findInstClassByName(const gc_string &instClassName) const;

    // Assuming symmetrical threads, these reference the relevant items of the
    // first thread in the system.
    const MemoryBase *firstLevelDataMem() const { return _firstLevelDataMem; };
    const MemoryBase *firstLevelInstMem() const { return _firstLevelInstMem; };
    const MemoryPort *firstLevelInstMemPort() const { return _firstLevelInstMemPort; };
    const Memory *lastLevelInstrMem() const { return _lastLevelInstrMem; };

    Opencxx::Ptree *findMemAlias(const gc_string &name) const;

    const gc_string &ISS_HFilename() const { return _iss_hfilename; }
    const gc_string &uArchHFilename() const { return _uArchHFilename; }
    const adl::StrList &incl_paths() const { return _incl_paths; };

    // Final setup/checking.
    void finalize(adl::MsgLog &ml);
    // Debugging dump.
    virtual void dump(std::ostream &,const gc_string &pfx) const;

  protected:
    adl::Core *findCore(adl::MsgLog &ml,Opencxx::Ptree *loc,const gc_string &name);
    const adl::System *findSys(const gc_string &name);
    void setupCore(adl::Core *core,Opencxx::Environment *env);
    void declareTop(adl::MsgLog &ml,Opencxx::Environment *env,Opencxx::Ptree *loc,const gc_string &name);
    void declareThread(Thread &parent,Thread &thread,bool is_group);

    void addCache();
    void createThreadHierarchy();
    Thread *replicateThreads (const gc_string &name,Thread &oh);

    void setupLastLevelInstrMem();
    void pushSharedResources(Thread &thread,adl::MsgLog &ml);

    void finalize_managers(adl::MsgLog &ml);
    void process_unimpl_instrs(adl::Core &core, adl::MsgLog &ml);
    void process_mem_aliases(adl::Core &core, adl::MsgLog &ml);
    void process_helpers(adl::Core &core,adl::MsgLog &ml);

    // Get the next pipestage index.
    unsigned next_ps_index() { return _pipeindex++; };

    void addInstClassType();

		void processScheduling(adl::MsgLog &ml);

    void add_mem_alias(const gc_string &src,Opencxx::Ptree *trg);

    

  private:
    Thread &top() { return *_top; };

    friend class OuterHandler;
    friend class CoreHandler;
    friend class MngrsHandler;
    friend class RegisterFile;
    friend class ActionWalker;
    friend class UadlParser;
    friend class UadlDriver;
    friend class InstClass;
    friend class MemAliasHandler;
    friend class ThreadHandler;
    friend class Resources;

    gc_string           _archName;
    gc_string           _uArchName;

    typedef gc_list<Opencxx::Environment *> EnvList;

    EnvList               _all_envs;

    unsigned _timeOut; // dead/live-lock timeout cycle count for safe mode

    Thread               *_top;               // Top-level resources.
    Thread               *_thread;            // Pointer to first child node
                                              // (should be same as all others).

		PipeVect              _sortedStages;      // Stages in sorted (scheduled) order.
    Machines              _machines;
    InstClasses           _instClasses;       // instruction class definitions
    adl::PMap             _mem_aliases;       // store memory aliases we know about.

    adl::HelperMap        _helpers;           // Stores uADL helper functions.
    adl::PList            _type_decls;        // Stores uADL user type decls.

    // ISS-related data members
    ADL_IR           &_iss_ir;            // architecture IR created by ADL
    const gc_string _iss_hfilename;       // transactional ISS header filename
    const gc_string _uArchHFilename;      // microarchitecture model header filename
    
    const adl::InstrPacketMap  &_packet_map;  // Hash of cores pointers -> allocation objects.
    const adl::InstrSlotAllocs *_instr_slots; // Slot allocations for this core.
    const adl::InstrSlotAlloc  *_dm_slots;    // Slots for the decode-miss handler.

    unsigned _maxRegFileId;               // maximum register (file) id
    unsigned _maxRegWidth;                // maximum register bit width
    unsigned _maxOperands;                // maximum number of operands
    unsigned _maxInstrSize;               // maximum instruction size (in bytes)
    unsigned _minInstrSize;               // minimum instruction size (in bytes)
    unsigned _instrSizeGCD;               // greatest common divisor (GCD) of instruction sizes
    bool     _commit_on_exception;        // Commit exception when it occurs and flush pipe.
    bool     _allow_unimpl_instrs;        // True => allow instructions to not
                                          // be covered by any instruction
                                          // class.
    bool         _check_cr_args;          // Parse custom resource headers and check/store arguments.
    static bool  _full_paths;             // Include full-paths for errors and other messages.
    adl::StrList _incl_paths;             // Search paths besides current directory. 

    const MemoryBase *_firstLevelDataMem;     // first-level data memory/cache
    const MemoryBase *_firstLevelInstMem;     // first-level instruction memory/cache
    const Memory     *_lastLevelInstrMem;
    const MemoryPort *_firstLevelInstMemPort; // first-level instruction memory/cache port
    const MemoryPort *_firstLevelDataMemPort; // first-level data memory/cache port    

    Opencxx::Ptree *_branch_hook;                // Hook function on branch execution.
    adl::PAuxFunc   _mem_access;                 // Memory access hook.

    unsigned        _pipeindex;
  };

  //
  // Top-level uADL (and ADL) parser class
  //
  class UadlParser : public adl::BaseParser {
  public:
    UadlParser(UadlIr *ir);

  protected:
    // Creates the initial outer-handler.
    virtual adl::Handler *make_initial_handler(Opencxx::Environment *);

    virtual void setup(Opencxx::Environment *env);

    // For adding top-level decls to the ADL data model.
    void add_function(Opencxx::Ptree *exp);
    void add_declaration(Opencxx::Ptree *exp);    

    // Creates the data model, handles the second pass.
    virtual void second_pass(adl::MsgLog &);
    
    // The uADL data model.
    UadlIr *_ir;
  };

}

#endif // _UADL_PARSE_H

//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// The API exported by the model.  This file may be included multiple times if you
// have multiple core models.  In such a situation, you should define ISS_NAMESPACE to
// be the name of the namespace for each core model you generated, e.g. if you ran adl2iss
// twice, once with --iss-namespace=foo and once with --iss-namespace=bar, then you would
// include this file twice in your wrapper/driver code:
//
// #define ISS_NAMESPACE foo
// #include ModelInterface.h
//
// #define ISS_NAMESPACE bar
// #include ModelInterface.h
//
// This then creates declarations for the basic entry points to obtain an instance of the model:
//
// IssNode *foo_model = foo::getTopLevelNode(); 
// IssNode *bar_model = bar::getTopLevelNode(); 
//

#ifndef _MODEL_INTERFACE_H_
#define _MODEL_INTERFACE_H_

#include <string>
#include <vector>
#include <list>
#include <map>

#include "helpers/BasicTypes.h"

#include "Introspection.h"

#ifndef __NO_RNUMBER__
namespace rnumber {
  class RNumber;
}
# define REGTYPE_t rnumber::RNumber
# define REGTYPE const REGTYPE_t &
#else

// If RNumber is not defined, then use a dummy class.
#ifndef RNUMBER_H
namespace rnumber {
  
  struct RNumber { };

}
#endif
# define REGTYPE_t uint64_t
# define REGTYPE REGTYPE_t
#endif

#define DEFAULT_ISS_NAMESPACE adliss

namespace adl {

  struct MemHandler;
  struct SysCallExtHandler;
  struct EventBusExtHandler;

  typedef std::map<std::string,int> IndexMap;

  struct StrPairs : public std::vector<std::pair<std::string,std::string> > {};

  // An eventbus handler object.  These items are registered with the event-bus
  // container.  It's up to the handler to do the casting to get the correct
  // data type.  For standalone models, or ones in a single process, the
  // internal implementation works just fine, but externally implemented
  // broadcasts are possible when this is not the case.
  struct EventBusHandler {
    virtual ~EventBusHandler() {};
    virtual void operator()(const void *,int id) = 0;
  };

  typedef std::list<EventBusHandler*> HandlerList;

  // Used internally for storing information about event buses, but declared
  // here so that it may be used by integration code, when external event buses
  // are implemented.
  struct EventBus {
    std::string            _name;
    std::string            _sig;
    int                    _count;
    HandlerList            _handlers;

    EventBus(const std::string &n,const std::string &s) : _name(n), _sig(s), _count(0) {};
  };

  typedef std::vector<EventBus> EventBuses;

  //
  // Used to query about attributes associated with ADL resources.
  // The attribute strings are interned and referred to using integer
  // handles.  This class allows you to go from string -> int in O(1)
  // time and int -> string in O(n) time.
  //
  struct AttrData : public IndexMap
  {
    typedef std::map<std::string,int> Base;
    typedef Base::value_type value_type;

    AttrData();
    AttrData(const value_type *b,const value_type *e);

    // Returns -1 if an attribute does not exist.
    int                get_attr(const std::string &n) const;
    // Returns an empty string if the attribute does not exist.
    const std::string &get_attr(int x) const;
  };

  //
  // The name and ID of the global memory.
  //
  extern const char *GlobalMemName;
  extern const int GlobalMemId;

  //
  // MMU interface data structures and functions.
  //

  // This is used for initializing an MMU lookup field.
  // The first value of each pair is the item's name, the second is the item's value.
  struct FieldPair {
    std::string _name;
    uint64_t      _value;

    FieldPair() : _value(0) {};
    FieldPair(const std::string &n,uint64_t v) : _name(n), _value(v) {};
  };

  struct FieldData : public std::vector<FieldPair> {};

  // C-like interface for FieldData, to make it easier to interface with some other languages.
  size_t getFieldDataSize(const FieldData *);
  void getFieldPair(const FieldData *,unsigned index,std::string &name,uint64_t &value);
  void addFieldPair(FieldData *,const std::string &name,uint64_t value);
  void setFieldPair(FieldData *,unsigned index,const std::string &name,uint64_t value);

  std::ostream &operator<<(std::ostream &,const FieldData &);

  struct IssCore;

  // Base class for all MMU lookup objects.
  struct MmuBase {
    virtual ~MmuBase() {};

    // For the logging interface, if you want to keep a persistent pointer to an
    // MmuBase object, then you *must* query user_entry and clone this element
    // if the return is true.  If the return is false, then it indicates that
    // the entry is in the TLB and will have a relatively long lifespan, though
    // cloning it is always the safest option, just in case the entry gets
    // overwritten in the TLB.
    virtual bool user_entry() const = 0;
    virtual MmuBase *clone() const = 0;

    virtual unsigned id() const = 0;
    virtual const char *name() const = 0;

    virtual uint64_t getField(unsigned index) const = 0;
    virtual void init(const FieldData &ld) = 0;
    virtual void report(FieldData &ld) const = 0;

    virtual addr_t epn() const { return 0; };
    virtual addr_t rpn() const { return 0; };

    virtual bool has_epn() const { return false; };
    virtual bool has_rpn() const { return false; };

    virtual int set() const { return -1; };
    virtual int way() const { return -1; };

    virtual void logMmuTranslation(const IssCore &core,TransType,int seq,addr_t ea,addr_t ra) const {};
  };

  struct CacheLineData : public std::vector<uint32_t> {};

  struct IssNode;
  typedef std::vector<IssNode *> IssChildren;

  // Callback triggered by calls to set_active_count by cores.
  struct ActiveCallback {
    virtual ~ActiveCallback() {};

    // When the active count is modified, this functor is called (if one is
    // registered), with a pointer to the core and a reference to the count
    // value.  Generally, the callback should modify active_count based upon
    // new_count and then return the old value.
    virtual unsigned set_active_count(IssCore *c,bool &active_count,unsigned new_count) = 0;
    virtual void trigger_exception(unsigned eflag) = 0;
  };

  // Call-back functor for register callbacks.  Register callbacks must be setup
  // during model-build via the config file.  The callbacks will then only occur
  // for the specified registers.  The default callback is a dummy which does
  // nothing.  The user may then register a new callback by calling
  // setRegCallback() on an IssNode.
  struct RegCallback {
    virtual ~RegCallback() {};

    // On an event change, this is the function which is called for a registered
    // callback.  The default is to do nothing.  For a register-file, index
    // contains the index of the changed element.  For a plain register, index
    // is 0.
    virtual void operator()(unsigned index,REGTYPE value) {};
  };

  // Same as above, for register-read callbacks.  The functor should return a
  // value, which is then stored into the register during the read process.  The
  // default (no callback installed) is to skip this step, treating the register
  // as a normal register.
  struct RegReadCallback {
    virtual ~RegReadCallback() {};

    // For a register-file, 'index' contains the index of the element to modify.
    // For a plain register, index is 0.
    virtual REGTYPE_t operator()(unsigned index) = 0;
  };


  // Same as above, for exceptions.
  struct ExceptCallback {
    virtual ~ExceptCallback() {};

    // When an exception is raised, this method is called.
    virtual void operator()() {};
  };

  // Interface for "show" routines- used to dump register, MMU, and other information.
  struct ReportBase {
    virtual ~ReportBase() {};

    // Called whenever a context change occurs.
    virtual void report_ctx_change(const IssNode *,unsigned id,const char *name,unsigned context_num) {};
    // Called when the reporting of context data is finished.
    virtual void report_ctx_off(const IssNode *) {};

    // Called on every register by the model's showRegs function.  Shared means
    // that the register is defined at a higher level in the hierarchy.
    virtual  void report_reg (const IssNode *,unsigned id,const char *name,REGTYPE value,bool shared) {};
    virtual  void report_regfile (const IssNode *,unsigned id,const char *name,int index,REGTYPE value,bool shared) {};
    // Called for every TLB entry by the model's showMmu function.
    virtual void report_mmu (const IssNode *,const MmuBase &mb,unsigned set, unsigned way,
                             const FieldData &fd,bool valid,bool shared,addr_t epn,bool valid_epn,addr_t rpn, bool valid_rpn) {};  
    // Called for every cache way by the model's showCaches function.  Note:
    // data may be 0 if the cache does not implement data.
    virtual void report_cache(const IssNode *,const char *name,CacheType type,unsigned level,addr_t addr,unsigned set,unsigned way,
                              bool valid,const FieldData &fd,bool shared,const byte_t *data,unsigned n) {}; 
    virtual void report_memory(IssNode *,unsigned id, const std::string &name, bool shared) {};

  };

  struct LogBase;

  struct PtBase;

  struct DbgRegItem {
    const char *_name;
    unsigned    _width;

    bool valid() const { return _name; };
  };

  // A dependency item used by log_dependencies.
  struct DepItem {
    enum Type { dpReg, dpMem };
    
    DepItem(Type t,unsigned id,unsigned snum) : _type(t), _id(id), _snum(snum) {};

    Type     _type;  // Type for this item:  register or memory.
    unsigned _id;    // Id for item.
    unsigned _snum;  // Sequence number for this access.  Only valid for sources.
  };

  class LockedMutexHash;

  class CoreModel;

  // Items in the vector are the sources.  Target information is stored in _trg.
  struct DepItems : public std::list<DepItem> {};

  // Exception classes for debug support.  The idea is that a handler for 
  // a debug event should throw this if it wants simulation to be interruped.
  // For example, on a breakpoint, a call to log_breakpoint is made.  If the 
  // handler for this breakpoint would like simulation to be interrupted, such as
  // to return control to a debugger, it should throw this exception- the simulation
  // loop will then immediately exit and return control to the caller.
  struct SimInterrupt {};

  // Base class for systems or cores.
  struct IssNode {
    IssNode(const std::string &name,const std::string &type,const IssNode *parent);
    virtual ~IssNode();

    // Node name access.
    const std::string &name() const { return _name; };

    // Type access.
    const std::string &type() const { return _type; };

    // Pointer to parent node (null if at root).
    const IssNode *parent() const { return _parent; };
   
    // Name, type, and parent can be modified.
    void setName(const std::string &n) { _name = n; };
    void setType(const std::string &t) { _type = t; };
    void setParent(const IssNode *p) { _parent = p; };

    // These are only really defined for cores, but they're defined for all
    // objects just to make the API simpler.
    virtual LogBase &logger() const;

    // Call this to install a logging object.
    virtual void setLogger(LogBase *);

    // True if the model was configured with multi-threading support, and is
    // thus safe to use with the multi-threaded simulation kernel.
    virtual bool hasMultiThreadedSupport() const { return false; };

    // Call this to provide the locking object used for locking atomic
    // instructions and memory accesses.
    virtual void setMemoryLocks(LockedMutexHash *) {};

    // Call this to cause external exceptions to be delayed.  Used by MT
    // simulations to avoid collisions between exception side-effects and normal
    // execution.
    virtual void setDelayedExternalExceptions(bool) {};

    // True if this model uses an external memory (requires setMemHandler to be
    // called to install a memory object) for the specified memory.
    virtual bool hasExternalMem(unsigned index = 0) const { return false; };

    // If external memory is enabled, then this lets the user set
    // the memory-handler object used to access the external memory.
    virtual void setMemHandler(MemHandler *,unsigned index = 0) {};
    virtual MemHandler *getMemHandler(unsigned index = 0) const { return 0; };

    // Set and get the system-call handler object.
    virtual void setSysCallHandler(SysCallExtHandler *) {};
    virtual SysCallExtHandler *getSysCallHandler() const { return 0; };

    // True if this model uses an external event-bus system (requires
    // setEventBusHandler to be called to install a handler object).
    virtual bool hasExternalEventBuses() const { return false; };

    // If external event bus broadcasts are enabled, then this lets the user set
    // the object used to broadcast event bus information.  Any existing event
    // buses are registered with this object when this method is called.
    virtual void setEventBusHandler(EventBusExtHandler *) {};
    virtual EventBusExtHandler *getEventBusHandler() const { return 0; };

    // Set an external resource.  We have to use a void* here, in order to make
    // this generic for any kind of external resource.  Internally, this will be
    // cast as necessary.  Returns false if no resource matches the specified
    // name.
    virtual bool setExtResource(const std::string &name,void *res) { return false; };

    // Sets a Graphite performance model.  This only does something if we
    // created the model with this feature.
    virtual void setGraphitePerfModel(CoreModel *) {};
    virtual CoreModel *getGraphitePerfModel() const { return 0; };
    virtual bool hasGraphitePerfSupport() const { return false; };
    virtual unsigned numGraphiteRegs() const { return 0; };

    // True if a leaf node (core).
    virtual bool is_core() const = 0;

    // Returns the natural endianness of the data layout of the target.
    virtual bool isBigEndian() const = 0;
    
    // True if this is a core with data-dependency tracking information.
    virtual bool has_dependency_tracking() const { return false; };

    // Access to register and memory sequence nnumbers.
    virtual unsigned get_reg_seq_num() const { return 0; };
    virtual unsigned get_mem_seq_num() const { return 0; };

    // Access to the last dependency-list hit.
    virtual const DepItems *get_last_dependencies() const { return 0; };

    // This reads memory through the hierarchy w/o modifying the caches.
    // Returns true on success, false on failure, where a failure might be
    // caused by a translation exception.
    // id:     Id of the memory.  0 is the global memory.
    // trans:  If true, address is considered an ea and will be translated.
    // addr:   Address of access.
    // size:   Access size:  8, 16, 32.
    virtual bool debug_instr_read(uint32_t &result,bool trans,addr_t addr,int size) const = 0;
    virtual bool debug_instr_write(bool trans,addr_t addr,uint32_t value,int size) = 0;
    virtual bool debug_data_read(uint32_t &result,unsigned id,bool trans,addr_t addr,int size) const = 0;
    virtual bool debug_data_write(unsigned id,bool trans,addr_t addr,uint32_t value,int size) = 0;
  
    // True if the node is active.
    virtual bool is_active() const { return _active; };

    // Instruction count for this node.
    virtual uint64_t instr_count() const = 0;
    // Allows the instruction count to be decremented by the simulation
    // environment.
    virtual void decr_instr_count(uint64_t d) {};

    // If this is a time-tagged ISS, then true.
    virtual bool isTimeTagged() const { return false; };

    // When using a time-tagged ISS with external memory, call this function to
    // add latency during an external memory operation.
    virtual void addLatency(unsigned) {};

    // For when we have an internal DMI, these values are used for read and
    // write latencies on an internal DMI hit.
    virtual void setDmiLatencies(unsigned read_latency,unsigned write_latency) {};

    // For a time-tagged ISS, return the final completion time in the
    // simulation.  Note: Current time is returned via the td counter.
    virtual ttime_t finalTime() const { return 0; };

    // Access the program counter.
    virtual void setProgramCounter(addr_t) = 0;
    virtual addr_t getProgramCounter() const = 0;

    // For architectures which have a separate current-instruction-address and
    // next-instruction-address concept and pre-instruction increment semantics,
    // e.g. CIA and NIA in POWER, this returns the current instruction address.
    // For other architectures, this will be the same as the program counter.
    virtual addr_t getCurrentInstrAddr() const = 0;

    // Traversal of children.
    const IssChildren &children() const { return _children; };

    // Get a context's ID from its name.
    virtual unsigned getCtxId(const std::string &name) const;

    // Modify the debug-control value for a given context.  The context may be specified using
    // either its numerical id or its name.
    virtual unsigned setDbgCtx(unsigned id,unsigned context_num);
    unsigned setDbgCtx(const std::string &name,unsigned context_num);

    // This sets the active context to the value of the debug context.
    virtual void setActiveCtxToDbg(unsigned id);
    void setActiveCtxToDbg(const std::string &name);

    // This restores the active context to what it should be, based upon the state of the system.
    virtual void restoreActiveCtx(unsigned id);
    void restoreActiveCtx(const std::string &name);

    // Get the currently active index for a given context.
    virtual unsigned getActiveCtx(unsigned id) const;
    unsigned getActiveCtx(const std::string &name) const;

    // Read/write contexts to memory at the specified address.
    virtual void readContext(unsigned id,unsigned index,addr_t addr) {};
    virtual void writeContext(unsigned id,unsigned index,addr_t addr) {};
    
    // Clear a specified element of a context.
    virtual void clearContext(unsigned id,unsigned index) {};

    // Get an index from a register name.  If true, the name was found, false if not found.
    // Updates index for the register's index and updates rfindex if the name refers to an element
    // of a register file.  Sets values to -1 if not found/not set.
    virtual bool getRegIndex(int &rindex,int &rfindex,const std::string &name) const { rindex = -1; rfindex = -1; return false; };

    // Force a value into a register.  In the case of the version taking a
    // string, 'index' is ignored if the name contains a register-file index,
    // e.g. FOO3, where FOO is the name of a register file.  In such a case, 3
    // is used as the index and 'index' is ignored.
    virtual bool setReg(unsigned rindex,unsigned index,const rnumber::RNumber &value) { return false; };
    virtual bool setReg(unsigned rindex,unsigned index,uint64_t value) { return false; };
    virtual bool setReg(const std::string &name,unsigned index,const rnumber::RNumber &value) { return false; };
    virtual bool setReg(const std::string &name,unsigned index,uint64_t value) { return false; };

    // Same as setReg, unless the user has defined an external-write hook, in
    // which case that is called instead.
    virtual bool setRegExt(unsigned rindex,unsigned index,const rnumber::RNumber &value)         { return setReg(rindex,index,value); };
    virtual bool setRegExt(unsigned rindex,unsigned index,uint64_t value)                        { return setReg(rindex,index,value); };
    virtual bool setRegExt(const std::string &name,unsigned index,const rnumber::RNumber &value) { return setReg(name,index,value); };
    virtual bool setRegExt(const std::string &name,unsigned index,uint64_t value)                { return setReg(name,index,value); };;

    // This logs the value of a register to any installed loggers.  It calls
    // getReg internally to retrieve the value.  This can be useful if you've
    // called setReg, since that doesn't log any modifications.
    // Returns false if the specified register does not exist.
    virtual bool logReg(unsigned rindex,unsigned index) const { return false; };
    virtual bool logReg(const std::string &name,unsigned index) const { return false; };

    // Same as setReg, except that any associated write hooks are called.
    virtual bool writeReg(unsigned rindex,unsigned index,const rnumber::RNumber &value) { return false; };
    virtual bool writeReg(unsigned rindex,unsigned index,uint64_t value) { return false; };
    virtual bool writeReg(const std::string &name,unsigned index,const rnumber::RNumber &value) { return false; };
    virtual bool writeReg(const std::string &name,unsigned index,uint64_t value) { return false; };

    // Get a value for a register.  Returns false if the register does not exist
    // or the index is bad; true otherwise.  The register's value is placed into
    // the value parameter.
    // KEY_FUNC: getReg
    virtual bool getReg(unsigned rindex,unsigned index,rnumber::RNumber &value) const { return false; };
    virtual bool getReg(unsigned rindex,unsigned index,uint64_t &value) const { return false; };
    virtual bool getReg(const std::string &name,unsigned index,rnumber::RNumber &value) const { return false; };
    virtual bool getReg(const std::string &name,unsigned index,uint64_t &value) const { return false; };

    // Same as getReg, except that any associated read hooks are called.
    virtual bool readReg(unsigned rindex,unsigned index,rnumber::RNumber &value) { return false; };
    virtual bool readReg(unsigned rindex,unsigned index,uint64_t &value) { return false; };
    virtual bool readReg(const std::string &name,unsigned index,rnumber::RNumber &value) { return false; };
    virtual bool readReg(const std::string &name,unsigned index,uint64_t &value) { return false; };
  
    // Display registers by calling reg() or regfile() for each resource.  This
    // shows all registers visible at this level.  Registers which are shared but
    // defined at a level of hierarchy above this one have the shared flag set on
    // the call to reg().
    virtual void showRegs(ReportBase &) const {};

    // Register a callback.  If the specified register does not support
    // callbacks, returns false, else returns true.  A value of 0 sets this
    // to use the default callback, which does nothing.
    virtual bool setRegCallback(unsigned rindex,RegCallback *cb) { return false; };
    virtual bool setRegCallback(const std::string &name,RegCallback *cb) { return false; };

    // Same for register-read callbacks.
    virtual bool setRegReadCallback(unsigned rindex,RegReadCallback *cb) { return false; };
    virtual bool setRegReadCallback(const std::string &name,RegReadCallback *cb) { return false; };

    // Register an exception callback.
    virtual bool setExceptCallback(unsigned index,ExceptCallback *cb) { return false; };
    virtual bool setExceptCallback(const std::string &name,ExceptCallback *cb) { return false; };

    // Debug support.  These routines will only do something if debug-mode was
    // turned on during generation.
    virtual void clearBreakpoints() {};
    virtual void clearWatchpoints() {};

    virtual unsigned setBreakpoint(addr_t,PtBase *payload = 0) { return 0; };
    virtual unsigned setTmpBreakpoint(addr_t,PtBase *payload = 0) { return 0; };
    virtual bool clearBreakpointByAddr(addr_t addr) { return false; };
    virtual unsigned setWatchpoint(addr_t,WatchType type,PtBase *payload = 0) { return 0; };
    virtual bool clearWatchpointByAddr(addr_t addr,WatchType type) { return false; };
    
    // NOTE: have to add it because _cur_core in TarmacReader.C is using it
    virtual void checkForBreakpoint(addr_t addr);

    // MMU get/set routines.
    virtual void setMmuLookup(const std::string &lookup,unsigned set,unsigned way,const FieldData &ld) {};
    virtual void getMmuLookup(const std::string &lookup,unsigned set,unsigned way,FieldData &ld) const {};
    virtual void showMmu(ReportBase &) const {};

    // Cache get/set routines.
    virtual void setCaches(const std::string &name,unsigned set,unsigned way,const FieldData &fd,const CacheLineData &cd) {};
    virtual void showCaches(ReportBase &) const {};

    // Memory set/get routines
    virtual unsigned getMemId(const std::string &name) const = 0;

    virtual void   setMem(unsigned id, addr_t addr, uint64_t data, unsigned size) = 0;
    void   setMem(const std::string &name, addr_t addr, uint64_t data, unsigned size);

    virtual uint64_t getMem(unsigned id, addr_t addr,unsigned size) const = 0;
    uint64_t getMem(const std::string &name, addr_t addr,unsigned size) const;

    virtual void showMem(ReportBase &) {};				     

    // These are sized memory accesses for directly accessing the main memory.
    virtual uint64_t mem_read64  (addr_t addr) = 0;
    virtual uint32_t mem_read32  (addr_t addr) = 0;
    virtual uint16_t mem_read16(addr_t addr) = 0;
    virtual uint8_t  mem_read8 (addr_t addr) = 0;
    virtual void mem_write64  (addr_t addr,uint64_t value,unsigned nb = BytesPerDW) = 0;
    virtual void mem_write32  (addr_t addr,uint32_t value,unsigned nb = BytesPerWord) = 0;
    virtual void mem_write16(addr_t addr,uint16_t value,unsigned nb = BytesPerHW) = 0;
    virtual void mem_write8 (addr_t addr,uint8_t value) = 0;

    // 32-bit read/write from a specified array.  This virtualizes whether we
    // should use byte-swapping or data-invariant address swizzling, as used in
    // TLM2 DMI.
    virtual uint32_t mem_read32(const byte_t *data) const = 0;
    virtual void mem_write32(byte_t *data,uint32_t v,unsigned nb = BytesPerWord) = 0;

    // Access to instruction memory. 
    virtual std::string getInstrMemName() const { return GlobalMemName; }
    // Access to data memory.
    virtual std::string getDataMemName() const { return GlobalMemName; }

    // Generate an exception.  The 'flags' value is a set of bit flags
    // corresponds to the exception flags obtainable from getExceptionInfo.
    // This immediately causes the side effects associated with an exception, if
    // setDelayedExternalExceptions(true) was not called.  However, it does not
    // abort current execution, if called during the execution of an
    // instruction.
    //
    // Otherwise, exceptions are stored and then only executed when
    // applyExceptions() is called.  This is really only applicable when working
    // with a multi-threaded simulation.
    virtual void genExceptions(unsigned flags) {} ;

    // Do the actual exception generation, for a multi-threaded model with
    // delayed-exceptions enabled.
    virtual void applyExceptions() {};

    // This will cancel a pending exception.  Only relevant for level-sensitive
    // exceptions.  Returns true if the exception was pending, false otherwise.
    virtual bool cancelExceptions(unsigned flags) { return false; } ;

    // This should only be called during the execution of an exception; it
    // immediately raises an exception and aborts the current instruction that
    // is being executed.
    virtual void genExceptionImm(unsigned id) {};

    // Introspection mechanisms.
    virtual const MmuInfo        &getMmuInfo() const;
    virtual const RegInfos       &getRegInfo() const;
    virtual const CacheInfos     &getCacheInfo() const;
    virtual const MemInfos       &getMemInfo() const;
    virtual const ExceptionInfos &getExceptionInfo() const;
    virtual const CtxInfos       &getContextInfo() const;

    virtual unsigned get_active_count() const;
    virtual unsigned set_active_count(unsigned n);

    // The active callback is called by cores whenever their activity count is
    // modified via set_active_count.
    virtual void setActiveCallback(ActiveCallback *ac) {};

    // General model reset function.
    virtual void reset() = 0;

    // Should step the simulator by one instruction.  Does not catch SimInterrupt calls.
    virtual void simple_step() {};

    // Runs the simulator for a time quanta, does not catch SimInterrupt calls.
    virtual ProgramStatus simple_run()     { return HALT; };
    virtual ProgramStatus simple_run_jit() { return HALT; };
    virtual ProgramStatus simple_run_dc()  { return HALT; };

    // Returns true if the simulator has the specified type of simulation mode.
    virtual bool has_sim_type(SimType type) const { return false; };

    // Main entry point into the simulator.  Runs until there are no more
    // active cores, or a SimInterrupt exception is thrown.
    virtual ProgramStatus run(SimType type) { return HALT; };
    virtual ProgramStatus run() { return HALT; };
    virtual ProgramStatus run_jit();
    virtual ProgramStatus run_dc();

    // Single step of the system.
    virtual ProgramStatus step() { return HALT; };

    // Multi-step of the system.
    virtual ProgramStatus stepn(unsigned n) { return HALT; };

    // Execute a single instruction from the specified buffer, bypassing the
    // normal fetch logic.
    // buf:  Buffer of words from which to read.
    // n:    Number of elements in buf.  The min. of n and the number of words
    //       in the largest instruction in the core are read.  Extra words in
    //       the internal buffer are 0'd.
    virtual ProgramStatus exec_from_buffer(uint32_t *buf,unsigned n) { return HALT; };

    // Tracing control:  The following apply only to the current node.

    // Toggle on tracing.  The value is or'd with the current value and the previous value
    // is returned.  The TraceType enum describes the different types of tracing events.  The
    // default is to set all values.
    virtual uint64_t set_tracing(uint64_t flags = (uint64_t)-1) = 0;
  
    // Toggle off tracing.  The bits set in the argument turn off the relevant tracing
    // activities.  Default is to turn off all events.  Returns the prior value
    virtual uint64_t clear_tracing(uint64_t flags = (uint64_t)-1) = 0;
  
    // Set the tracing flags directly.  Returns the prior value.
    virtual uint64_t set_tracing_flags(uint64_t flags) = 0;
  
    // Returns true if any tracing is activated.
    virtual bool tracing_on() const = 0;
  
    // Returns true if the specified tracing events are all on.
    virtual bool tracing_on(uint64_t flags) const = 0;
  
    // Returns the tracing flags.
    virtual uint64_t tracing_flags() const = 0;

    // Tracing control: The following apply recursively to all nodes.  They
    // return the flags for the first leaf-node encountered.
    virtual uint64_t set_tracing_r(uint64_t flags = (uint64_t)-1) = 0;
    virtual uint64_t clear_tracing_r(uint64_t flags = (uint64_t)-1) = 0;
    virtual uint64_t set_tracing_flags_r(uint64_t flags) = 0;
    virtual bool tracing_on_r() const = 0;
    virtual bool tracing_on_r(uint64_t flags) const = 0;

    // Only relevant for JIT models:  Dump any generated code (recursive).
    virtual void dump_jit_code() {};

    // Tracing on/off hooks.
    virtual void tracingOn() {};
    virtual void tracingOff() {};

    // Access to the disassembler (if enabled).  This allows the user to
    // disassemble arbitrary memory.  Throws a std::exception if there is a
    // problem, such as due to a translation problem.  Returns the size of the
    // instructions in bytes.
    //
    // o:      Disassembly output stream
    // addr:   Source address.  This is an ea if tran=true, else an ra.
    // tran:   Whether to translate.
    // tindex: Index of instruction table to use, or -1 to use the current.
    virtual unsigned disassemble(std::ostream &o,addr_t addr,bool tran,int tindex) const;
  
    // Get/Set/List dynamic parameters within the model.  Throws a run-time error if a parameter
    // is invalid or the data is invalid.
    virtual void     set_dyn_parm(const std::string &parm,unsigned value);
    virtual unsigned get_dyn_parm(const std::string &parm) const;
    virtual void     list_dyn_parm(StrPairs &parms) const;

    // Returns various simulation statistics, identified by name.  Throws a
    // runtime_error if the statistic doesn't exist.
    virtual unsigned get_sim_stat(const std::string &parm) const;

    // For temporal decoupling.  Only implemented if the model is built with this
    // feature.  The idea is that there is a 64-bit counter which is incremented
    // by a specified value for each instruction that's executed.  When this
    // counter exceeds the threshold, execution stops.

    // Returns true if the model supports the temporal decoupling API.
    virtual bool has_td() const { return false; };
    // Set the counter to a specified value.  Returns the old value.
    virtual uint64_t set_td_counter(uint64_t) { return 0; };
    // Set the increment value.  Returns the old value.
    virtual unsigned set_td_increment(unsigned) { return 0; };
    // Set the threshold value.  Returns the old value.  By default, it's set to a max value.
    virtual uint64_t set_td_threshold(uint64_t = (uint64_t)-1) { return 0; };
    // Set the threshold value to the new time if the current time is <= the
    // current time.
    virtual void cond_set_td_threshold(uint64_t cur_time,uint64_t new_time) {};

    // Accessors for the various values.
    virtual uint64_t  get_td_counter()     const { return 0; };
    virtual uint64_t *get_td_counter_ptr()       { return 0; };
    virtual unsigned  get_td_increment()   const { return 0; };
    virtual uint64_t  get_td_threshold()   const { return 0; };

    // For controlling the external-memory direct-memory-interface cache.
    // Only implemented if DMI is enabled.
    //
    // ca:    Specifies instruction, load, or store cache.
    // addr:  Target address to update.
    // mem:   Pointer to hos4t memory location.  This must point to the exact location
    //        in memory containing 'addr' and the memory block must be as large as the
    //        DMI-cache entry-size contained in the model.
    //        Note:  Memory must be in the host endian format.
    //
    // Returns true if we have a DMI cache for this model.
    virtual bool has_dmi_cache() const { return false; };
    virtual unsigned dmi_page_size() const { return 0; };
    // If the model has been configured with an internal dmi cache, and it talks
    // directly with the external dmi cache (i.e. there is no data cache), then
    // any update must only be done during an external memory access, so that the
    // internal cache will know the correct effective (virtual) address to use.
    virtual void update_dmi_cache_entry(CacheAccess ca,addr_t addr,byte_t *mem) {};
    // If the model has been configured with an internal dmi cache, and it talks
    // directly with the external dmi cache (i.e. there is no data cache), then an
    // invalidate will reset the internal dmi caches, since we don't know the
    // effective address of the entry to invalidate.
    virtual void invalidate_dmi_cache_entry(CacheAccess ca,addr_t addr) {};
    // This will invalidate all DMI caches (internal and external).
    virtual void invalidate_dmi_cache() {};

    //
    // The following functions return the results of the equivalent global
    // functions and allow common code to work with multiple ISSs which have
    // different namespaces.
    //

    // Returns the name of the top-level core
    virtual const char *getCoreName() const = 0;

    // Returns the top-level file name of the model used to generate the core
    virtual const char *getModelFileName() const = 0;

    // Get access to the attributes database for this model.
    virtual const adl::AttrData &getAttrData() const = 0;

    // Get access to any user-specified preamble data.
    virtual const char *getModelPreamble() const = 0;

    // Resets memory and other global state.
    virtual void resetGlobal() = 0;

    // Sets the size of memory, if using a block (non-sparse) memory model.
    virtual void setMemorySize(unsigned size) = 0;

    // A default register map for debugger integration, if one is present.
    virtual const DbgRegItem *getDefaultRegMap() { return 0; };

  protected:
    std::string    _name;
    std::string    _type;
    bool           _active;
    IssChildren    _children;
    const IssNode *_parent;
  };

  // Base class for ISS systems.
  struct IssSystem : public IssNode {
    IssSystem (const std::string &name,const std::string &type,const IssNode *parent,bool child_owner = true) : 
      IssNode(name,type,parent),
      _child_owner(child_owner)
    {};
    
    ~IssSystem();

    virtual bool is_core() const { return false; };

    // Returns the value of the first child.
    virtual bool isBigEndian() const;
    
    // Returns true if all constituent items have Graphite core model support.
    virtual bool hasGraphitePerfSupport() const;

    // Returns true if all constituent items are MT-safe.
    virtual bool hasMultiThreadedSupport() const;

    // System objects propagate their handlers to child nodes.
    virtual void setMemHandler(MemHandler *);
    virtual void setSysCallHandler(SysCallExtHandler *);
    virtual void setEventBusHandler(EventBusExtHandler *);

    // Returns true if first child has an external memory.
    virtual bool hasExternalMem() const;

    virtual void setDelayedExternalExceptions(bool);

    // Instruction count for this node.  This returns the sum of all child nodes.
    virtual uint64_t instr_count() const;

    // This recursively sets the pc of all children.
    virtual void setProgramCounter(addr_t);

    // This returns the program counter of the first core.
    virtual addr_t getProgramCounter() const;
    virtual addr_t getCurrentInstrAddr() const;

    // This distributes the active callback to all constituent cores.
    virtual void setActiveCallback(ActiveCallback *ac);

    // Reset all children, by default.
    virtual void reset();

    // The default version of this calls simulate on all children.
    virtual void simple_step();

    // Returns true if any children are active, false if our own active flag is
    // false or no children are active.
    virtual bool is_active() const;

    // Returns true if the simulator has the specified type of simulation mode.
    virtual bool has_sim_type(SimType type) const;

    // Run all children, by default.
    virtual ProgramStatus run();
    virtual ProgramStatus run(SimType type);
    
    // Executes run_jit for each children.
    virtual ProgramStatus run_jit();

    // Executes run_dc for each children.
    virtual ProgramStatus run_dc();

    // Step all children, by default.
    virtual ProgramStatus step();

    // Step all chilren, by default.
    virtual ProgramStatus stepn(unsigned n);

    // Turn tracing on/off for all children, by default.
    virtual void tracingOn();
    virtual void tracingOff();

    // Add a child item.  This should really only be used by the support
    // framework and only at the beginning of simulation.
    void add_child(IssNode *);

    // By default, the memory routines just route their info to the first child.
    virtual bool debug_instr_read(uint32_t &result,bool trans,addr_t addr,int size) const;
    virtual bool debug_instr_write(bool trans,addr_t addr,uint32_t value,int size);
    virtual bool debug_data_read(uint32_t &result,unsigned id,bool trans,addr_t addr,int size) const;
    virtual bool debug_data_write(unsigned id,bool trans,addr_t addr,uint32_t value,int size);

    virtual unsigned getMemId(const std::string &name) const;
    virtual void     setMem(unsigned id, addr_t addr, uint64_t data, unsigned size);
    virtual uint64_t getMem(unsigned id, addr_t addr,unsigned size) const;

    virtual uint64_t mem_read64  (addr_t addr);
    virtual uint32_t mem_read32  (addr_t addr);
    virtual uint16_t mem_read16(addr_t addr);
    virtual uint8_t  mem_read8 (addr_t addr);
    virtual void mem_write64  (addr_t addr,uint64_t value,unsigned nb = BytesPerDW);
    virtual void mem_write32  (addr_t addr,uint32_t value,unsigned nb = BytesPerWord);
    virtual void mem_write16(addr_t addr,uint16_t value,unsigned nb = BytesPerHW);
    virtual void mem_write8 (addr_t addr,uint8_t value);

    virtual uint32_t mem_read32(const byte_t *data) const;
    virtual void mem_write32(byte_t *data,uint32_t v,unsigned nb = BytesPerWord);

    // Tracing control.
    virtual uint64_t set_tracing(uint64_t flags = (uint64_t)-1);
    virtual uint64_t clear_tracing(uint64_t flags = (uint64_t)-1);
    virtual uint64_t set_tracing_flags(uint64_t flags);
    virtual bool tracing_on() const;
    virtual bool tracing_on(uint64_t flags) const;
    virtual uint64_t tracing_flags() const;

    virtual uint64_t set_tracing_r(uint64_t flags = (uint64_t)-1);
    virtual uint64_t clear_tracing_r(uint64_t flags = (uint64_t)-1);
    virtual uint64_t set_tracing_flags_r(uint64_t flags);
    virtual bool tracing_on_r() const;
    virtual bool tracing_on_r(uint64_t flags) const;
    virtual void dump_jit_code();

    // This just propagates the calls to the system's children.
    virtual void     set_dyn_parm(const std::string &parm,unsigned value);
    virtual unsigned get_dyn_parm(const std::string &parm) const;
    virtual void     list_dyn_parm(StrPairs &parms) const;

    // Systems simply replicate these values to all children.  However, they just return
    // the value from the last child.
    virtual uint64_t set_td_counter(uint64_t);
    virtual unsigned set_td_increment(unsigned);
    virtual uint64_t set_td_threshold(uint64_t = (uint64_t)-1);
    virtual void cond_set_td_threshold(uint64_t cur_time,uint64_t new_time);

    // These return value from the first child.
    virtual uint64_t  get_td_counter() const;
    virtual uint64_t *get_td_counter_ptr();
    virtual unsigned  get_td_increment() const;
    virtual uint64_t  get_td_threshold() const;

    // These replicate to all children, returning false if any child returns false.
    virtual bool setRegCallback(unsigned rindex,RegCallback *cb);
    virtual bool setRegCallback(const std::string &name,RegCallback *cb);

    // Simply replicates this to all children.  Returns false if any child returns false.  In the case of
    // functions returning a handle, just returns the first handle it getes.
    virtual bool setExceptCallback(unsigned index,ExceptCallback *cb);
    virtual bool setExceptCallback(const std::string &name,ExceptCallback *cb);

    // Debug support.  These routines will only do something if debug-mode was
    // turned on during generation.
    virtual void clearBreakpoints();
    virtual void clearWatchpoints();

    virtual unsigned setBreakpoint(addr_t,PtBase *payload = 0);
    virtual unsigned setTmpBreakpoint(addr_t,PtBase *payload = 0);
    virtual bool clearBreakpointByAddr(addr_t addr);
    virtual unsigned setWatchpoint(addr_t,WatchType type,PtBase *payload = 0);
    virtual bool clearWatchpointByAddr(addr_t addr,WatchType type);
    
    // virtual void checkForBreakpoint(addr_t addr);

    //
    // We have versions here so that a standalone system can be allocated as a
    // container for other IssNode objects.  These delegate to the children.
    //
    virtual const char *getCoreName() const;
    virtual const char *getModelFileName() const;
    virtual const adl::AttrData &getAttrData() const;
    virtual const char *getModelPreamble() const;
    virtual void resetGlobal();
    virtual void setMemorySize(unsigned size);

  private:
    bool _child_owner;   // True => Child nodes will be deleted.
  };

  // Base class for cores- the "leaf nodes" of the design.
  // This allows us to register these items so that they can
  // have their state set.
  struct IssCore : public IssNode {
    IssCore (const std::string &name,const std::string &type,const IssNode *parent,
             unsigned &id,unsigned child_id = 0);

    virtual bool is_core() const { return true; };

    unsigned getCoreId() const{ return _id; };

    unsigned getChildId() const { return _child_id; };

    // These are only really defined for cores, but they're defined for all
    // objects just to make the API simpler.
    virtual LogBase &logger() const { return *_logger; };

    // For internal use.
    LogBase *get_logger() { return _logger; };

    // Call this to install a logging object.
    virtual void setLogger(LogBase *);

    // External access to the MMU.  This will perform a translation using the
    // core's MMU, returning true on a successful translation, false otherwise.
    // This performs a translation w/o side-effects: Hit-handlers, permission
    // checks, etc. are not called.  If 'log' is true, then a call to
    // log_mmu_translation is made, irregardless of the current tracing flags,
    // as long as logging support has been enabled for the model.
    virtual bool extInstrReadTranslate(addr_t &ra,addr_t ea,bool log = false) const = 0;
    virtual bool extDataReadTranslate(addr_t &ra,addr_t ea,bool log = false) const = 0;
    virtual bool extDataWriteTranslate(addr_t &ra,addr_t ea,bool log = false) const = 0;

    // Debug support.  These routines will only do something if debug-mode was
    // turned on during generation.
    virtual void clearBreakpoints() {};
    virtual void clearWatchpoints() {};

    // Sets breakpoints and watchpoints.  Returns an integer handle for the point.
    // This handle is used to identify the relevant watchpoint/breakpoint when it
    // occurs (when log_breakpoint or log_watchpoint is called).  A temporary
    // breakpoint is one which is removed after it is encountered.  The payload
    // parameter is associated with the breakpoint and is passed to the
    // log_breakpoint logging routine.  This can be used by an integrator to
    // associate various behavior with a breakpoint.  For example, it can store a
    // watchpoint handle within a temporary breakpoint, for implementing
    // watchpoint behavior with a debugger.
    virtual unsigned setBreakpoint(addr_t,PtBase *payload = 0);
    virtual unsigned setTmpBreakpoint(addr_t,PtBase *payload = 0);
    virtual unsigned setWatchpoint(addr_t,WatchType type,PtBase *payload = 0);

    // Remove a point using its handle.  Returns true on success, false on failure. e.g.
    // if the relevant index doesn't exist.
    virtual bool clearBreakpoint(unsigned h) { return false; };
    virtual bool clearWatchpoint(unsigned h) { return false; };
    virtual bool clearBreakpointByAddr(addr_t addr) { return false; };
    virtual bool clearWatchpointByAddr(addr_t addr,WatchType type) { return false; };

    virtual void checkForBreakpoint(addr_t addr);
    
    using IssNode::logReg;
    virtual bool logReg(const std::string &name,unsigned index) const;

    // Tracing control.
    virtual uint64_t set_tracing(uint64_t flags = (uint64_t)-1);
    virtual uint64_t clear_tracing(uint64_t flags = (uint64_t)-1);
    virtual uint64_t set_tracing_flags(uint64_t flags);
    virtual bool tracing_on() const;
    virtual bool tracing_on(uint64_t flags) const;
    virtual uint64_t tracing_flags() const;

    virtual uint64_t set_tracing_r(uint64_t flags = (uint64_t)-1);
    virtual uint64_t clear_tracing_r(uint64_t flags = (uint64_t)-1);
    virtual uint64_t set_tracing_flags_r(uint64_t flags);
    virtual bool tracing_on_r() const;
    virtual bool tracing_on_r(uint64_t flags) const;

    // Only used by internal objects.
    uint64_t trace_mode() const { return _trace_mode; };
    void set_trace_mode(uint64_t tm) { _trace_mode = tm; };

  protected:
    LogBase *_logger;
    uint64_t _trace_mode;
    unsigned _id;
    unsigned _child_id;
  };

  //
  // Memory operations.  These modify global memory.
  //
  uint64_t mem_read64  (addr_t addr);
  uint32_t mem_read32  (addr_t addr);
  uint32_t mem_log_read32  (addr_t addr);
  uint16_t mem_read16(addr_t addr);
  uint8_t  mem_read8 (addr_t addr);
  void mem_write64  (addr_t addr,uint64_t value,unsigned nb=BytesPerDW);
  void mem_write32  (addr_t addr,uint32_t value,unsigned nb=BytesPerWord);
  void mem_write16  (addr_t addr,uint16_t value,unsigned nb=BytesPerHW);
  void mem_write8   (addr_t addr,uint8_t value);
  void initialize_mem (IssNode &n, const std::string &name, addr_t addr, uint32_t data, unsigned size,bool log_only = false);
  void resetMemory();

  // Block accesss to memory.  Address must be aligned with the block size.
  // These only exist if external memory is not being used.
  void mem_readpage(byte_t *t,size_t n,addr_t addr);
  void mem_writepage(byte_t *s,size_t n,addr_t addr);

  // Used by transactional ISSs for tracking memory usage.
  struct MemAccess {

    enum OpType { 
      opNone,        // No operation to perform.
      opCache,       // A cache operation is to be performed.
      opMem,         // A memory access is to be performed.
    };

    union {
      addr_t    _ea;     // Target effective address. 
      struct {
        int     _set, _way; // Only used for direct set/way cache operations.
      } _c;
    };
    addr_t    _ra;     // Target real address.
    union {
      int       _id;     // If a cache action, then this is the ID of the cache.
      unsigned  _size;   // Size in bytes of the original transaction.
    };
    unsigned    _nb;     // For writes, actual number of bytes being written.
    CacheAccess _type;   // Type of access.
    OpType      _op;     // Operation classification.

    MemAccess() { reset(); };
    MemAccess(CacheAccess type,int id,addr_t ra) : 
      _ra(ra), _id(id), _type(type), _op(MemAccess::opCache) {};
    MemAccess(CacheAccess type,int id,addr_t ra,int set,int way) : 
      _ra(ra), _id(id), _type(type), _op(MemAccess::opCache) { _c._set = set; _c._way = way; };
    MemAccess(addr_t ea,addr_t ra,unsigned size,unsigned nb,CacheAccess type) : 
      _ea(ea), _ra(ra), _size(size), _nb(nb), _type(type), _op(opMem) {};

    OpType op() const { return _op; };
    void set(addr_t ea,addr_t ra,unsigned s,unsigned nb,CacheAccess t) { _ea = ea; _ra = ra; _size = s; _nb = nb; _type = t; _op = opMem; };
    void reset() { _op = opNone; };
    addr_t ea() const { return _ea; };
    addr_t ra() const { return _ra; };

    // This will return true if there is an outstanding operation.
    operator bool() const { return _op != opNone; };
  };

  // Used by transactional ISSs to track cache status.  This is a bit of a hack
  // because its size is fixed, but this is necessary right now because uADL
  // needs to queue up these data structures within fixed code.
  struct CacheStatus {
    bool _valid;
    bool _enabled[MaxCacheStatusLevels];
    bool _miss_allocate[MaxCacheStatusLevels];

    CacheStatus() : _valid(false) {};

    bool valid() const { return _valid; };

    // Note that these indices refer to the cache level, which starts at 1.
    bool enabled(unsigned level) const { return _enabled[level-1]; }
    bool miss_allocate(unsigned level) const { return _miss_allocate[level-1]; }
  };

  std::ostream &operator<<(std::ostream &o,const MemAccess &ma);

  //
  // The logging interface.  Each of these calls in the model is gated by a
  // TraceType value, so logging can be controlled at a fine-level of granularity.
  //
  // With a bare model, these calls are expected to be provided by an external
  // library.  The standalone-executable support library implements these
  // functions such that each call is broadcast to all registered logging objects
  // (objects derived from the LoggingIface object).
  //

  // Describes types of instructions.
  enum InstrFlags {
    ifBrUncond = 0x1, // Unconditional branch.
    ifBrCond   = 0x2, // Conditional branch.
    ifHalts    = 0x4, // Instruction may halt simulation.
  };

  // This enumeration is used for toggling the filtering of various logging
  // functions.  If a new logging function is added, an entry should be added to
  // this enum.
  enum TraceType {
    ttNone           = 0x000000,
    ttInstrPrefetch  = 0x000001,
    ttInstrRead      = 0x000002,
    ttInstr          = 0x000004,
    ttInstrTimes     = 0x000008,
    ttInstrArgs      = 0x000010,
    ttMmuTranslation = 0x000020,
    ttCacheAction    = 0x000040,
    ttRegWrite       = 0x000080,
    ttRegRead        = 0x000100,
    ttRegfileWrite   = 0x000200,
    ttRegfileRead    = 0x000400,
    ttCoreMemWrite   = 0x000800,
    ttCoreMemRead    = 0x001000,
    ttMemWrite       = 0x002000,
    ttMemRead        = 0x004000,
    ttException      = 0x008000,
    ttBreakpoint     = 0x010000,
    ttWatchpoint     = 0x020000,
    ttAnnotation     = 0x040000,
    ttBranchTaken    = 0x080000,
    ttBasicBlockEnd  = 0x100000,
  };

  struct PacketLogBase;
  typedef void *LogType;

  // Base class for payload data for watchpoints and breakpoints.
  struct PtBase {
    virtual ~PtBase() {};
  };

  // Base class for all logging operations.  Derive from this class and then
  // install it into an IssCore object.
  struct LogBase {

    // This object is used by log_annotation to report key-value pairs after the
    // AdlDataStart keyword from the Info, Warning, or Error commands.  Note
    // that if string data is used, the lifetime of the string is just for that
    // of the log_annotation call.  If you plan to use the data later, you must
    // store it, such as by assigning it to a std::string.
    //
    // A default operator<< exists for Data (declared below).
    struct Data {
      enum Type { NumType, StrType };

      std::string _key;
      Type        _type;
      union {
        uint64_t    _value;
        const char *_str;
      };

      Data() : _value(0) {};
      Data(const std::string &k,uint64_t v) : _key(k), _type(NumType), _value(v) {};
      Data(const std::string &k,const char *s) : _key(k), _type(StrType), _str(s) {};

      bool is_string() const { return _type == StrType; };
      bool is_num() const { return _type == NumType; };
    }; 

    virtual ~LogBase() {};

    // Called by the core object when the logger is installed into a new core.
    virtual void init(IssCore *) {};

    // Logs the fact that an instruction is going to be fetched.
    // ea:  The effective address of the impending fetch.
    virtual void log_instr_prefetch(addr_t ea) {};

    // Indicates that an instruction is going to be executed via
    // exec_from_buffer().
    virtual void log_instr_from_buffer() {};

    // Logs the instruction fetch action itself.
    // id:     Numerical identifier for the memory.
    // name:   Memory from which the instruction is being read.
    // ea/ra:  Effective and real address of the read.
    // value:  The memory value that was read.
    virtual void log_instr_read(unsigned id,const char *name,addr_t ea,addr_t ra,uint32_t value) {};

    // Transactional-ISS only: If an external model is calling log_instr_read,
    // then this allows any remaining instruction-fetch objects to be cleared from
    // the instruction-fetch list.
    virtual void log_clear_instr_reads() {};

    // Logs the end of a basic block.  This is called immediately before
    // log_instr for a given instruction.  The purpose of this logging call is
    // to allow for faster instruction logging by allowing the user to disable
    // log_instr calls after the start of a basic block; this call then notifies
    // the logger when the basic block is complete.
    // ea:  Effective address of the instruction at the end of the basic block.
    virtual void log_basic_block_end(addr_t ea) {};

    // Logs the instruction usage.
    // opc:        The opcode of the instruction.
    // num_bytes:  Length of instruction in bytes.
    // name:       The name of the instruction.
    // dis:        A disassembler function, which will write a complete disassembly line
    //             to a supplied stream.
    // flags:      Instruction flags.  Uses the bit flags defined by InstrFlags.
    virtual void log_instr(const uint32_t *opc,int num_half_bytes,const char *name,Disassembler dis,uint32_t flags) {};

    // Logs the instruction uasge and input/output registers
    // opc:        The opcode of the instruction.
    // num_bytes:  Length of instruction in bytes.
    // name:       The name of the instruction.
    // dis:        A disassembler function, which will write a line of input/output registers
    virtual void log_instr_args(const uint32_t *opc,int num_half_bytes,const char *name,Disassembler dis) {};

    // For a time-tagged ISS, this logs the issue time and completion time of
    // the instruction.
    virtual void log_instr_issue_time(ttime_t) {};
    virtual void log_instr_completion_time(ttime_t) {};

    // Logs a register write operation.
    // id:     Numerical identifier for the register.
    // name:   Name of the register.
    // value:  The new value of the register.
    virtual void log_reg_write(unsigned id,const char *name,REGTYPE value) {};

    // Logs a register read operation.  This is only called if the model is compiled with
    // the --log-reg-reads option.
    // id:     Numerical identifier for the register.
    // name:   Name of the register.
    // value:  The value of the register.
    virtual void log_reg_read(unsigned id,const char *name,REGTYPE value) {};

    // Logs a register-file write operation.
    // id:     Numerical identifier for the register file.
    // name:   Name of the register.
    // index:  Index of the element being written.
    // value:  The new value of the register.
    virtual void log_regfile_write(unsigned id,const char *name,uint32_t index,REGTYPE value) {};

    // Logs a register-file read operation.  This is only called if the model is compiled with
    // the --log-reg-reads option.
    // id:     Numerical identifier for the register file.
    // name:   Name of the register.
    // index:  Index of the element being written.
    // value:  The new value of the register.
    virtual void log_regfile_read(unsigned id,const char *name,uint32_t index,REGTYPE value) {};

    // The following are all versions of the above, but with an extra mask parameter.  If the model is compiled
    // with the --log-reg-masks option, then partial register read/write operations are logged via these functions,
    // which contain a mask describing what portion of the register was read or written.
    virtual void log_reg_write_mask    (unsigned id,const char *name,REGTYPE value,REGTYPE mask) {};
    virtual void log_reg_read_mask     (unsigned id,const char *name,REGTYPE value,REGTYPE mask) {};
    virtual void log_regfile_write_mask(unsigned id,const char *name,uint32_t index,REGTYPE value,REGTYPE mask) {};
    virtual void log_regfile_read_mask (unsigned id,const char *name,uint32_t index,REGTYPE value,REGTYPE mask) {};

    // Indicates an external write to a register.  The argument value represents
    // the data being written.  Intermediate results (normal log_reg_write,
    // etc. calls) log the actual changes to the system.
    virtual void log_reg_ext_write(unsigned id,const char *name,REGTYPE value) {};
    virtual void log_regfile_ext_write(unsigned id,const char *name,uint32_t index,REGTYPE value) {};

    // These reflect a memory access from the core's point of view.
    // id:        Numerical identifier for the memory.
    // name:      Name of the memory.
    // ea:        Effective address of the operation.
    // num_bytes: Number of bytes of the operation.
    virtual void log_core_mem_write(unsigned id,const char *name,addr_t ea,int num_bytes) {};
    virtual void log_core_mem_read(unsigned id,const char *name,addr_t ea,int num_bytes) {};

    // Same as above, but called when "typed" memory access are used.
    virtual void log_core_mem_write_typed(unsigned id,const char *name,addr_t ea,int num_bytes,CacheAccess type) {};
    virtual void log_core_mem_read_typed(unsigned id,const char *name,addr_t ea,int num_bytes,CacheAccess type) {};

    // These reflect a memory access from the point of view of the memory's new state.
    // id:     Numerical identifier for the memory.
    // name:   Name of the memory.
    // pre:    True => Call is before action is taken; False => Action has occurred.
    // seq:    Sequence number for misaligned accesses.  Starts with 0.  Will have a 1 for next access, if misaligned.
    // ea/ra:  Effective and real address of the operation.  This is always a 32-bit word-aligned address
    // value:  The new value of memory.
    virtual void log_mem_write(unsigned id,const char *name,bool pre,int seq,addr_t ea,addr_t ra,uint32_t value) {};
    virtual void log_mem_read(unsigned id,const char *name,bool pre,int seq,addr_t ea,addr_t ra,uint32_t value) {};

    // Logs a memory-translation operation.
    // tt:  Type of translation being performed.
    // seq: Sequence number for misaligned accesses.
    // set: Set affected, -1 if not valid.
    // way: Way affected, -1 if not valid.
    // ea:  The effective-address initiating the translation.  Note:  Not relevant (will be 0) for tt=WriteTrans.
    // ra:  The resulting real-address.    Note:  Not relevant (will be 0) for tt=WriteTrans.
    // mb:  A pointer to the translation object that was picked.
    virtual void log_mmu_translation(TransType tt,int seq,int set,int way,addr_t ea,addr_t ra,const MmuBase *mb) {};

    // Logs a cache access/operation.
    // name:      The name of the cache.
    // action:    The action type, e.g. hit, miss, evict, etc.
    // access:    The type of access generating the action, e.g. instruction-fetch, data-read, etc.
    // level:     Level in the hierarchy of caches.
    // set/way:   The set/way of the access.  -1 for set or way values indicates invalid values 
    //            that should be ignored.
    // linemask:  line-mask value for this cache.
    // ra:        Real address of the cache access.
    virtual void log_cache_action(const char *name,CacheAction action,CacheAccess access,unsigned level,
                                  int set,int way,unsigned linemask,addr_t ra) {};

    // These log the ocurrance of a breakpoint or watchpoint.
    // ea:      Effective address of the access generating the break/watchpoint.
    // pre:     True if the watchpoint is occurring before the load/store, false if after.
    // handle:  Integer handle of the break/watchpoint.
    // type:    Type of watchpoint (load, store, etc.).
    virtual void log_breakpoint(addr_t ea,unsigned handle,PtBase *payload) {};
    virtual void log_watchpoint(addr_t ea,bool pre,WatchType type,int num_bytes,uint64_t value,
                                unsigned handle,PtBase *payload) {};

    // Logs the occurrance of an exception.
    // handle:  Integer handle of the exception.  This can be obtained by calling getExceptionInfo()
    //          on a core.
    // pre:     True => Call is before action is taken; False => Exception side-effects have occurred.
    // name:    The exception's name.
    virtual void log_exception(unsigned handle,bool pre,const char *name) {};

    // Logs the occurrance of an annotation message.  Note that warnings and
    // errors are always logged, even if tracing is turned off.  In the second version, extra
    // numeric data may be sent as well.
    // level: User-defined annotation level.  msg: The annotation message.
    // data:  Arbitrary numeric data sent by the user.
    virtual void log_annotation(MsgType type,unsigned level,const std::string &msg,
                                unsigned ndata = 0,Data data[] = 0) {};

    // Logs the fact that a branch was taken, i.e. that the
    // next-instruction-address register was updated by action code.
    // ea:  Target of the branch.
    virtual void log_branch_taken(addr_t ea) {};

    // Logs the fact that a branch has completed.  If it was taken, then there
    // was a previous call to log_branch_taken.
    // ea:  Address of the branch.
    virtual void log_branch_end(addr_t ea) {};

    // Called when a core change is occurring.  Since log_instr_prefetch includes an IssCore pointer,
    // this isn't called for when there's a core change due to a new instruction.  Instead, this is
    // called when a core change occurs in the middle of an instruction's intermediate result list, e.g.
    // one core modifies another core's resources directly.
    // newcore:  The new core for which resources are being modified.
    virtual void log_core_change() {};

    // Called whenever a context change occurs.
    virtual void log_ctx_change(unsigned id,const char *name,unsigned context_num,CtxType update) {};

    // This is only required for transactional ISSs and is called to register an
    // instruction packet for storing logging information.  Call with 0 to specify
    // that no instruction exists and this should be added to the pending queue.
    // The pending queue will be added to the next fetched instruction's list of
    // IRs.
    //
    // Returns the prior value.
    virtual PacketLogBase *log_current_instr(PacketLogBase *x) { return 0; };

    // This is only required for transactional ISSs and is called to signal that
    // any pending logging data should be flushed.
    virtual void log_flush_pending() {};

    // Discard all logging events.  Only relevant for transactional ISss.
    virtual void log_discard(PacketLogBase *x) {};

    // This is only required for transactional ISSs and is called to signal the
    // end of an instruction.  For multi-threaded applications, it is used to
    // signal the end of a complete instruction, in order to produce a total
    // instruction order across multiple threads.  In that case, the argument
    // 'x' is always 0.
    virtual void log_commit(PacketLogBase *x) {};

    // For compatibility between transactional and non-transactional ISSs, this
    // must be called if code is expecting to directly receive a log event, such
    // as after performing an external translation.  Returns the prior value.
    virtual bool log_direct(bool d) { return false; };

    // Fetch the pending queue of logging items.  This clears the queue as well.
    virtual LogType log_get_pending_queue() { return 0; };

    // Append to the pending queue.
    virtual void log_append_pending_queue(LogType) {};

  };

  std::ostream &operator<<(std::ostream &,const LogBase::Data &);
  

  //
  // Various global flags.
  //

  // Mode flags.
  extern bool   VerboseMode;
  extern bool   QuietMode;
  extern bool   DebugMode;
  extern bool   SysCalls_enabled;
  extern bool   LaxMode;

  // If non-zero, then we halt the simulation when we match this count (across all
  // processors).
  extern uint64_t InstrMax;

  // Heartbeat:  Every n instructions, print out info.
  extern uint64_t Heartbeat;

  //
  // Transactional-ISS relevant stuff.
  // 

  // Information about an operand.
  struct OpInfo {

    enum OpType { opNone = 0x0, opSrc = 0x1, opTrg = 0x2, opCond = 0x4, opPartial = 0x8 };

    unsigned _id;     // ID of the register.
    int      _index;  // Index, if relevant (is a single element of a register file).
    unsigned _type;   // Operand type information (source, target, etc.).

    bool valid() const { return _type != opNone; };
  bool isSrc() const { return _type & opSrc; };
  bool isTrg() const { return _type & opTrg; };

    OpInfo() {};
    OpInfo(unsigned id,int index,unsigned t) : _id(id), _index(index), _type(t) {};
    OpInfo(const std::pair<unsigned,int> &x,unsigned t) : _id(x.first), _index(x.second), _type(t) {};
  };

  struct ExItems;

  // Return type returned by branch-info functions.
  typedef std::pair<BranchType,bool> BranchInfoType;

  // This is an opaque class which wraps information about exceptions.  You get
  // this by calling getExceptionData on a transactional ISS object.  It's reference
  // counted to make memory allocation easier.
  struct ExHandle {
    ExItems *_exitems;

    ExHandle() : _exitems(0) {};
    ExHandle(const ExHandle &);
    ~ExHandle();

    void reset();

    const ExHandle &operator=(const ExHandle &);

    void alloc(unsigned n);
    ExItems &operator()();
  };

  // Return type returned by branch-info functions.
  typedef std::pair<BranchType,bool> BranchInfoType;

  // Base class for any kind of instruction-packet-like thing.
  struct PacketLogBase {
    unsigned     _exdata;        // If an exception occurs, this is the index of
                                 // the exception.
    ExHandle     _exitems;       // If there are exceptions, then this stores
                                 // the relevant data structures.
    LogType      _log;           // Extra pointer for use with logging systems.

    PacketLogBase() : _exdata(0), _log(0) {};
  };

  // Base class for instruction packets.
  struct InstrPacketBase : public PacketLogBase {
    int          _id;            // Unique instruction ID.  -1 for a decode-miss.
    int          _size;          // Size of instruction in bytes.
    addr_t       _addr;          // Address of instruction.
    uint32_t     _opc[2];           // The instruction's opcode.
    unsigned     _numops;        // Number of operands.
    
    Disassembler _disasm;        // Disassembler function for the instruction.

    virtual ~InstrPacketBase() {};

    void reset (int id,int size,addr_t addr,uint32_t opc[2],unsigned numops,
                Disassembler disasm)
    {
      _id = id; 
      _size = size;
      _addr = addr; 
      _opc[0] = opc[0]; 
      _opc[1] = opc[1]; 
      _numops = numops;
      _exdata = 0;
      _exitems.reset();
      _disasm = disasm;
      _log = 0;
    }

    void quick_reset()
    {
      _log = 0;
      _exdata = 0;
    }

    int id() const { return _id; };

    unsigned size() const { return _size; };

    addr_t addr() const { return _addr; };

    const uint32_t *opc() const { return &_opc[0]; };

    unsigned getNumOps() const {
      return _numops;
    }

    void setExceptionState(unsigned e) { _exdata = e; };

    virtual const OpInfo &getOp(unsigned index) const = 0;

    virtual bool printOp(std::ostream &o,unsigned index) const = 0;
    virtual bool printOpResolved(std::ostream &o,unsigned index) const = 0;
  };

  // Base class for instruction packets w/operand information.  The size of the
  // operand information is templated.
  template <size_t MaxSlots>
  struct InstrPacketOpBase : InstrPacketBase {

    OpInfo       _ops[MaxSlots]; // Operand information about this instruction.

  const OpInfo &op(unsigned index) const { return _ops[index]; };

  virtual const OpInfo &getOp(unsigned index) const {
    return op(index);
  }
};

  // Printing routines for InstrPacketBase.  operator<< is equivalent to printInstrPacket.
  // printInstrPacketResolved prints operands w/aliases and sparse register elements resolved to
  // their real elements.
  void printInstrPacket(std::ostream &,const InstrPacketBase &);
  void printInstrPacketResolved(std::ostream &,const InstrPacketBase &);
  std::ostream &operator<<(std::ostream &,const InstrPacketBase &);

  // Jit related parameters.  These are common to all cores (obviously), since
  // they're static.
  class TranslationParms {
  public:
    // If non-zero, we compile JIT basic blocks in background thread(s).
    static void set_mt_jit_compile(unsigned x) { _mt_jit_compile = x; };

    // Max number of instructions in a basic block.  When exceeded, we insert a
    // branch to another block.  Should rarely be used, since you don't normally
    // have tons of instructions without any intervening change-in-flow
    // instructions.
    static void set_max_bb_instr_count(unsigned x) { _max_bb_instr_count = x; }

    // Max number of BBs allowed at a time.  Basically a proxy for memory usage.
    static void set_max_bb_count(unsigned x) { _max_bb_count = x; }

    // Set the threshold at which we consider a block to be hot (should be JIT'd).
    static void set_jit_bb_hot_threshold(unsigned x) { _jit_bb_hot_threshold = x; };
    
    // Same for decode-cache.
    static void set_dc_bb_hot_threshold(unsigned x) { _dc_bb_hot_threshold = x; };

    // Set the size of the decode-cache buffer.
    static void set_dc_buffer_size(unsigned x) { _dc_buffer_size = x; };

    static unsigned mt_jit_compile() { return _mt_jit_compile; };
    static unsigned max_bb_instr_count() { return _max_bb_instr_count; };
    static unsigned max_bb_count() { return _max_bb_count; };
    static unsigned dc_buffer_size() { return _dc_buffer_size; };
    static unsigned jit_bb_hot_threshold() { return _jit_bb_hot_threshold; };
    static unsigned dc_bb_hot_threshold()  { return _dc_bb_hot_threshold; };
    
  private:
    static unsigned _mt_jit_compile;
    static unsigned _max_bb_instr_count;
    static unsigned _max_bb_count;
    static unsigned _dc_buffer_size;
    static unsigned _jit_bb_hot_threshold;
    static unsigned _dc_bb_hot_threshold;
  };

}

#endif

// These declarations are purposefully outside of the guards so that they can be
// included multiple times.  See the notes at the top of the file for more
// information.
#if !defined(ISS_NAMESPACE)
#  define ISS_NAMESPACE DEFAULT_ISS_NAMESPACE
#endif

//
// Note that the following functions are exported with C linkage if the model is
// generated with --c-interface, so that they can be used for a plugin-style
// architecture via dlsym.  You'll need to define ADL_C_INTERFACE before including
// ModelInterface.h so that the declarations will match up.
//
#if defined (ADL_C_INTERFACE)
#define AdlInterfaceBegin extern "C"
#else
#define AdlInterfaceBegin namespace ISS_NAMESPACE
#endif

AdlInterfaceBegin {

  // Get access to the top-level item.  This will be a system in an MP/MT ISS or a
  // core for a uni ISS.  Each call to this function allocates a new object, which
  // is not gc'd.  The id may be used to assign a unique id to each core.  Each
  // core which is created uses the id, then post-increments it.
  adl::IssNode *createTopLevelNode(unsigned &id);

  // Returns the name of the top-level core
  const char *getCoreName();

  // Returns the top-level file name of the model used to generate the core
  const char *getModelFileName();

  // Get access to the attributes database for this model.
  const adl::AttrData &getAttrData();

  // Get access to any user-specified preamble data.
  const char *getModelPreamble();

  // Resets memory and other global state.
  void resetGlobal();

  // Sets the size of memory, if using a block (non-sparse) memory model.
  void setMemorySize(unsigned size);

}


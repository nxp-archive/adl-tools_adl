//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Routines used by the generated ISS model.
//

#include <iostream>
#include <vector>
#include <list>
#include <sstream>
#include <stdexcept>
#include <stdarg.h>
#include <iomanip>
#include <iterator>
#include <utility>

// We set this here so that the library will not have any RNumber dependencies.
// In reality, we may still be using RNumber, but this lets us get away with having
// only a single library.
#define __NO_RNUMBER__
#define __IN_IMPLEMENTATION__

#include "helpers/Macros.h"
#include "helpers/StringUtils.h"

#include "ModelSupport.h"

#include "TypeIO.h"

using namespace std;

namespace adl {

  // Use the defines MEM_BIT_WIDTH and MEM_LOOKUP_LEVELS to define the memory
  // characteristics.

  // Number of bits that we care about.  The address type is defined in ModuleInterface.h.
#ifndef MEM_BIT_WIDTH
# define MEM_BIT_WIDTH 64
#endif

  // Number of lookup levels.  For example, 32/2 means that we care about 32 bits of the
  // address and have 2 levels of lookup- one tree node and then the lookup within the
  // page itself.  A level of 1 thus implies a single array of bytes.
#ifndef MEM_LOOKUP_LEVELS
# define MEM_LOOKUP_LEVELS 4
#endif

  // Run-time control of the system-call facility.
  bool SysCalls_enabled = false;

  // Various flags set by command-line parameters.
  bool     VerboseMode = false;
  bool     QuietMode   = false;
  bool     DebugMode   = false;
  bool     TimeTagged  = false;
  bool     LaxMode     = false;

  // Set to true if multiple threads will be used in the simulation.
  bool ThreadedSim = false;

  // Set to true once any watchpoints are set.  This is used to cut down on
  // overhead when watchpoints are not being used.
  bool WatchpointsEnabled = false;

  // Global memory name.
  const char *GlobalMemName = "Mem";
  const int   GlobalMemId   = 0;

  // Max-instruction count (considered as a per-core value).
  uint64_t InstrMax   = 0;

  // Heartbeat:  Every n instructions, print out info.
  uint64_t Heartbeat  = 0;

  bool		Enable_JIT;



  // Acts as a guaranteed source of 0s as a default for memory requests.
  const byte_t DummyBuffer[8] = {0,0,0,0,0,0,0,0};

  // Default handler for unknown instructions.
  void unknownInstruction(addr_t cia,uint32_t instr)
  {
    RError("Unknown instruction at address 0x" << hex << setw(8) << setfill('0') << cia 
           << ":  0x" << setw(8) << setfill ('0') << instr);
  }

  // Dummy register-callback used for a default action.
  RegCallback default_reg_callback_action;

  // Dummy for exceptions.
  ExceptCallback default_except_callback_action;

  AttrData::AttrData()
  {}

  AttrData::AttrData(const value_type *b,const value_type *e) : Base(b,e) 
  {}

  int AttrData::get_attr(const std::string &n) const 
  {
    const_iterator i = find(n);
    return (i == end()) ? -1 : i->second;
  }

  const std::string &AttrData::get_attr(int x) const
  {
    static string dummy;

    const AttrData &me = *this;
    for (AttrData::const_iterator i = me.begin(); i != me.end(); ++i) {
      if (i->second == x) {
        return i->first;
      }
    }
    return dummy;
  }
  
  struct AttrOrder {
    bool operator()(const Attrs::Attr &x,const  Attrs::Attr &y) const {  
      return (x.first < y.first);
    }

    bool operator()(const unsigned &x,const  Attrs::Attr &y) const {  
      return (x < y.first);
    }
   
    bool operator()(const  Attrs::Attr &x, const unsigned &y) const {  
      return (x.first < y);
    }  
    
  };
  

  
  void Attrs::finalize()
  {
    sort(_attrs.begin(),_attrs.end(),AttrOrder());
  }

  bool Attrs::has_attr(unsigned c) const
  {
    return binary_search(_attrs.begin(),_attrs.end(),c,AttrOrder());
  }

  std::pair<bool,Attrs::Params> Attrs::get_attr_data(unsigned c) const
  {
    auto p = equal_range(_attrs.begin(),_attrs.end(),c,AttrOrder());
    if (p.first == p.second) {
      // Not found.
      return make_pair(false,Params());
    } else {
      // Was found.
      return make_pair(true,p.first->second);
    }
  }

  static const AttrData *attrData = 0;

  void setAttrData(const AttrData &ad)
  {
    attrData = &ad;
  }

  ostream &operator<<(ostream &o,const Attrs &x)
  {
    assert(attrData);
    auto const &cd = *attrData;

    if (!x._attrs.empty()) {
      o << "<attrs> ";
      ForEach(x._attrs,i) {
        o << cd.get_attr(i->first);
        i->second.dump(o);
      }
      o << "</attrs>\n";
    }
    return o;
  }

  void Attrs::Params::dump(ostream &o) const {
    switch (_type) {      
    case Int:
      o << "(" << itos(_int_list.at(0)) << ") ";      
      break;
    case IntList:
      o << "(";	
      ForRange(_int_list.size(),i) {
        o << itos(_int_list.at(i));
        if (i != _int_list.size() - 1) {
          o << ",";
        }
      }
      o << ") ";	
      break;
    case StrList:
      o << "(";	
      ForRange(_str_list.size(),i) {
        o << "'" << _str_list.at(i) << "'";
        if (i != _str_list.size() - 1) {
          o << ",";
        }
      }
      o << ") ";	
      break;
    case Str:
      o << "('" << _str_list.at(0) << "') ";  
      break;     
    default:
      o << " "; 	  
    }
  }
  
  ostream &operator<<(ostream &o,const IntroBase &x)
  {
    o << "<name> " << x._name << " </name>\n"
      << static_cast<const Attrs&>(x);
    return o;
  }

  ostream &operator<<(ostream &o,const SharedIntroBase &x)
  {
    o << static_cast<const IntroBase&>(x)
      << "<shared> " << x._shared << " </shared>\n";
    return o;
  }

  template <class C>
  void displaySequence(ostream &o,const char *tag,const C &x)
  {
    if (!x.empty()) {
      o << "<" << tag << ">\n";
      ostream_iterator<typename C::value_type> oiter(o,"\n");
      copy(x.begin(),x.end(),oiter);
      o << "</" << tag << ">\n";
    }
  }

  ostream &operator<<(ostream &o,const MmuFieldInfo &x)
  {
    o << "<mmufield>\n"
      << static_cast<const IntroBase&>(x)
      << "<width> " << x._width << " </width>\n"
      << "</mmufield>\n";
    return o;
  }

  ostream &operator<<(ostream &o,const MmuFieldInfos &x)
  {
    displaySequence(o,"fields",x);
    return o;
  }

  ostream &operator<<(ostream &o,const MmuLookupInfo &x)
  {
    o << "<mmulookup>\n"
      << static_cast<const SharedIntroBase&>(x)
      << "<id> " << x._id << " </id>\n"
      << "<sets> " << x._sets << " </sets>\n"
      << "<ways> " << x._ways << " </ways>\n"
      << x._fields
      << "</mmulookup>\n";
    return o;
  }

  ostream &operator<<(ostream &o,const MmuInfo &x)
  {
    displaySequence(o,"mmu",x);
    return o;
  }

  ostream &operator<<(ostream &o,const RegFieldInfo &x)
  {
    o << "<regfield>\n"
      << static_cast<const IntroBase&>(x);
    ForEach(x._bits,i) {
      o << "<start> " << i->first  << " </start>\n"
        << "<end> "   << i->second << " </end>\n";
    }
    o << "</regfield>\n";
    return o;
  }

  ostream &operator<<(ostream &o,const RegFieldInfos &x)
  {
    displaySequence(o,"fields",x);
    return o;
  }

  ostream &operator<<(ostream &o, const EnumValueInfo &x)
  {
    o << "<val> "  << x.first << " </val>\n"
      << "<name> " << x.second << " </name>\n";
    return o;
  }

  ostream &operator<<(ostream &o, const EnumValueInfos &x)
  {
    displaySequence(o,"enums",x);
    return o;
  }


  ostream &operator<<(ostream &o,const RegInfo &x)
  {
    o << "<reg>\n"    
      << static_cast<const SharedIntroBase&>(x)
      << "<id> " << x._id << " </id>\n"
      << "<pseudo> " << x._pseudo << " </pseudo>\n"
      << "<width> " << x._width << " </width>\n"
      << "<size> " << x._size << "</size>\n"
      << x._fields
      << x._enums
      << "</reg>\n";
    return o;
  }

  ostream &operator<<(ostream &o,const RegInfos &x)
  {
    displaySequence(o,"regs",x);
    return o;
  }

  ostream &operator<<(ostream &o,const CacheInfo &x)
  {
    o << "<cache>\n"
      << static_cast<const SharedIntroBase&>(x)
      << "<type> ";
    switch (x._type) {
    case InstrCache:
      o << "instr";
      break;
    case DataCache:
      o << "data";
      break;
    default:
      o << "unified";
      break;
    }
    o << " </type>\n"
      << "<level> " << x._level << " </level>\n"
      << "<size> " << x._size << " </size>\n"
      << "<linesz> " << x._linesize << " </linesz>\n"
      << "<sets>\n";
    ForEach(x._sets,i) {
      o << "<set>" << *i << "</set>\n";
    }
    o << "</sets>\n"
      << "<ways>\n";
    ForEach(x._ways,i) {
      o << "<way>" << *i << "</way>\n";
    }
    o << "</ways>\n"
      << "</cache>\n";
    return o;
  }

  ostream &operator<<(ostream &o,const CacheInfos &x)
  {
    displaySequence(o,"caches",x);
    return o;
  }

  ostream &operator<<(ostream &o,const ExceptionInfo &x)
  {
    o << "<exception>\n"
      << static_cast<const IntroBase&>(x)
      << "<id> " << x._id << "</id>\n"
      << "<flag> " << x._flag << "</flag>\n"
      << "<priority> " << x._priority << "</priority>\n"
      << "</exception>\n";
    return o;
  }

  ostream &operator<<(ostream &o,const ExceptionInfos &x)
  {
    displaySequence(o,"exceptions",x);
    return o;
  }

  ostream &operator<<(ostream &o,const MemInfo &x) 
  {
    o << "<memory>\n"
      << static_cast<const SharedIntroBase&>(x)
      << "<id> " << x._id << " </id>\n"
      << "<instr_mem> " << x._instr_mem << " </instr_mem>\n"
      << "<size> " << x._size << " </size>\n"
      << "<addr_unit> " << x._addr_unit << " </addr_unit>\n"
      << "<parent> " << x._parent << " </parent>\n"
      << "<offset> " << x._offset << " </offset>\n"
      << "</memory>\n";
    return o;
  }

  ostream &operator<<(ostream &o,const MemInfos &x) 
  {
    displaySequence(o,"memories",x);
    return o;
  }

  ostream &operator<<(ostream &o,const CtxInfo &x) 
  {
    o << "<context>\n"
      << static_cast<const SharedIntroBase&>(x)
      << "<id> " << x._id << " </id>\n"
      << "<num> " << x._num_contexts << " </num>\n"
      << "</context>\n";
    return o;
  }

  ostream &operator<<(ostream &o,const CtxInfos &x) 
  {
    displaySequence(o,"contexts",x);
    return o;
  }

  void BlockMemory::set_mem_size(unsigned size)
  {
    if (count_ones(size) != 1) {
      RError("Memory size must be a non-zero power of 2.");
    }
    _mem_size = size;
    _mem_mask = _mem_size - 1;

    delete _mem;    
    _mem = new byte_t[_mem_size];
    memset(_mem,0,_mem_size);
  }

  static LogBase dummy_logger;

  ostream &operator<<(ostream &o,const LogBase::Data &d)
  {
    if (d.is_string()) {
      o << d._key << "=\"" << d._str << '"';
    } else {
      o << d._key << '=' << d._value;
    }
    return o;
  }

  IssNode::IssNode(const string &name,const string &type,const IssNode *parent) :
    _name(name), 
    _type(type),
    _active(true),
    _parent(parent)
  {};

  IssNode::~IssNode() 
  {};

  LogBase &IssNode::logger() const
  {
    return dummy_logger;
  }

  void IssNode::setLogger(LogBase *l)
  {
  }

  unsigned IssNode::get_active_count() const 
  { 
    return _active; 
  }

  unsigned IssNode::set_active_count(unsigned n) 
  { 
    unsigned o = _active;
    _active = n;
    return o;
  }

  const MmuInfo &IssNode::getMmuInfo() const
  {
    static MmuInfo dummy;

    return dummy;
  }

  const RegInfos &IssNode::getRegInfo() const
  {
    static RegInfos dummy;

    return dummy;
  }

  const CacheInfos &IssNode::getCacheInfo() const
  {
    static CacheInfos dummy;

    return dummy;
  }

  const ExceptionInfos &IssNode::getExceptionInfo() const
  {
    static ExceptionInfos dummy;

    return dummy;
  }

  const MemInfos &IssNode::getMemInfo() const
  {
    static MemInfos dummy;

    return dummy;
  }

  const CtxInfos &IssNode::getContextInfo() const
  {
    static CtxInfos dummy;

    return dummy;
  }

  void IssNode::setMem(const std::string &name, addr_t addr,uint64_t data,unsigned size)
  {
    setMem(getMemId(name),addr,data,size);
  }

  uint64_t IssNode::getMem(const std::string &name, addr_t addr,unsigned size) const
  {
    return getMem(getMemId(name),addr,size);
  }

  unsigned IssNode::getCtxId(const std::string &name) const
  {
    RError("Unknown context name " << name);
  }

  unsigned IssNode::setDbgCtx(unsigned id,unsigned context_num)
  {
    RError("Unknown context ID " << id);
  }

  unsigned IssNode::setDbgCtx(const std::string &name,unsigned context_num)
  {
    return setDbgCtx(getCtxId(name),context_num);
  }

  void IssNode::setActiveCtxToDbg(unsigned id)
  {
    RError("Unknown context ID " << id);
  }

  void IssNode::setActiveCtxToDbg(const std::string &name)
  {
    return setActiveCtxToDbg(getCtxId(name));
  }

  void IssNode::restoreActiveCtx(unsigned id)
  {
    RError("Unknown context ID " << id);
  }

  void IssNode::restoreActiveCtx(const std::string &name)
  {
    return restoreActiveCtx(getCtxId(name));
  }

  unsigned IssNode::getActiveCtx(unsigned id) const
  {
    RError("Unknown context ID " << id);
  }

  unsigned IssNode::getActiveCtx(const std::string &name) const
  {
    return getActiveCtx(getCtxId(name));
  }

  unsigned IssNode::disassemble(std::ostream &o,addr_t addr,bool tran,int tindex) const
  {
    RError("The disassembler is not supported by this model.");
  }  

  void IssNode::set_dyn_parm(const std::string &parm,unsigned value)
  {
    RError("Unsupported dynamic parameter:  " << parm);
  }

  unsigned IssNode::get_dyn_parm(const std::string &parm) const
  {
    RError("Unsupported dynamic parameter:  " << parm);
  }

  unsigned IssNode::get_sim_stat(const std::string &parm) const
  {
    RError("Unknown simulation statistic:  " << parm);
  }

  ProgramStatus IssNode::run_jit()
  {
    RError("JIT support not enabled for this model.");
  }

  ProgramStatus IssNode::run_dc()
  {
    RError("Decode-cache support not enabled for this model.");
  }

  void IssNode::list_dyn_parm(StrPairs &parms) const
  {
  }
  
  void IssNode::checkForBreakpoint(addr_t addr)
  {
    RError("Check for breakpoint not implemented.");
  }

  IssSystem::~IssSystem()
  {
    if (_child_owner) {
      ForEach(_children,i) {
        delete *i;
      }
    }
  }

  bool IssSystem::isBigEndian() const
  {
    if (!_children.empty()) {
      return _children.front()->isBigEndian();
    } else {
      // Kind of an arbitrary default here...
      return true;
    }
  }
  
  bool IssSystem::hasGraphitePerfSupport() const
  {
    ForEach(_children,i) {
      if (! (*i)->hasGraphitePerfSupport() ) {
        return false;
      }
    }
    return true;
  }

  bool IssSystem::hasMultiThreadedSupport() const
  {
    ForEach(_children,i) {
      if (! (*i)->hasMultiThreadedSupport()) {
        return false;
      }
    }
    return true;
  }

  bool IssSystem::hasExternalMem() const
  {
    return _children.front()->hasExternalMem();
  }

  void IssSystem::setMemHandler(MemHandler *mh)
  {
    ForEach(_children,i) {
      (**i).setMemHandler(mh);
    }
  }

  void IssSystem::setSysCallHandler(SysCallExtHandler *sch)
  {
    ForEach(_children,i) {
      (**i).setSysCallHandler(sch);
    }
  }

  void IssSystem::setEventBusHandler(EventBusExtHandler *ebh)
  {
    ForEach(_children,i) {
      (**i).setEventBusHandler(ebh);
    }
  }

  void IssSystem::setDelayedExternalExceptions(bool x)
  {
    ForEach(_children,i) {
      (**i).setDelayedExternalExceptions(x);
    }
  }

  void IssSystem::setActiveCallback(ActiveCallback *ac)
  {
    ForEach(_children,i) {
      (**i).setActiveCallback(ac);
    }
  }

  uint64_t IssSystem::instr_count() const
  {
    uint64_t count = 0;
    ForEach(_children,i) {
      count += (**i).instr_count();
    }
    return count;
  }

  // This recursively sets the pc of all children.
  void IssSystem::setProgramCounter(addr_t a)
  {
    ForEach(_children,i) {
      (**i).setProgramCounter(a);
    }
  }

  // This returns the program counter of the first core.
  addr_t IssSystem::getProgramCounter() const
  {
    return _children.front()->getProgramCounter();
  }

  addr_t IssSystem::getCurrentInstrAddr() const
  {
    return _children.front()->getCurrentInstrAddr();
  }

  void IssSystem::add_child(IssNode *n)
  {
    _children.push_back(n);
  }

  bool IssSystem::is_active() const
  {
    if (!_active) {
      return false;
    }
    bool a = false;
    ForEach(_children,i) {
      a = a || (**i).is_active();
    }
    return a;
  }
    
  // Reset all children, by default.
  void IssSystem::reset()
  {
    ForEach(_children,i) {
      (**i).reset();
    }
  }
    
  void IssSystem::tracingOn(  )
  {
    ForEach(_children,i) {
      (**i).tracingOn();
    }
  }
    
  void IssSystem::tracingOff(  )
  {
    ForEach(_children,i) {
      (**i).tracingOff();
    }
  }

  bool IssSystem::debug_instr_read(uint32_t &result,bool trans,addr_t addr,int size) const
  {
    return _children.front()->debug_instr_read(result,trans,addr,size);
  }

  bool IssSystem::debug_instr_write(bool trans,addr_t addr,uint32_t value,int size)
  {
    return _children.front()->debug_instr_write(trans,addr,value,size);
  }

  bool IssSystem::debug_data_read(uint32_t &result,unsigned id,bool trans,addr_t addr,int size) const
  {
    return _children.front()->debug_data_read(result,id,trans,addr,size);
  }

  bool IssSystem::debug_data_write(unsigned id,bool trans,addr_t addr,uint32_t value,int size)
  {
    return _children.front()->debug_data_write(id,trans,addr,value,size);
  }

  unsigned IssSystem::getMemId(const std::string &name) const
  {
    return _children.front()->getMemId(name);
  }

  void IssSystem::setMem(unsigned id, addr_t addr, uint64_t data, unsigned size)
  {
    _children.front()->setMem(id,addr,data,size);
  }

  uint64_t IssSystem::getMem(unsigned id, addr_t addr,unsigned size) const
  {
    return _children.front()->getMem(id,addr,size);
  }

  uint64_t IssSystem::mem_read64  (addr_t addr)
  {
    return _children.front()->mem_read64(addr);
  }

  uint32_t IssSystem::mem_read32  (addr_t addr)
  {
    return _children.front()->mem_read32(addr);
  }

  uint16_t IssSystem::mem_read16(addr_t addr)
  {
    return _children.front()->mem_read16(addr);
  }

  uint8_t  IssSystem::mem_read8 (addr_t addr)
  {
    return _children.front()->mem_read8(addr);
  }

  void IssSystem::mem_write64  (addr_t addr,uint64_t value,unsigned nb)
  {
    _children.front()->mem_write64(addr,value,nb);
  }

  void IssSystem::mem_write32  (addr_t addr,uint32_t value,unsigned nb)
  {
    _children.front()->mem_write32(addr,value,nb);
  }

  void IssSystem::mem_write16(addr_t addr,uint16_t value,unsigned nb)
  {
    _children.front()->mem_write16(addr,value,nb);
  }

  void IssSystem::mem_write8 (addr_t addr,uint8_t value)
  {
    _children.front()->mem_write8(addr,value);
  }

  uint32_t IssSystem::mem_read32  (const byte_t *data) const
  {
    return _children.front()->mem_read32(data);
  }

  void IssSystem::mem_write32(byte_t *data,uint32_t v,unsigned nb)
  {
    _children.front()->mem_write32(data,v,nb);    
  }


  void IssSystem::simple_step()
  {
    ForEach(_children,i) {
      (**i).simple_step();
    }
  }

  // Returns true if the simulator has the specified type of simulation mode.
  bool IssSystem::has_sim_type(SimType type) const
  {
    return _children.front()->has_sim_type(type);
  }

  // Run all children, by default.
  ProgramStatus IssSystem::run()
  {
    try {
      while (is_active()) {
        simple_step();
      }
    }
    catch (SimInterrupt) {
      return BREAK;
    }
    return HALT;    
  }

  // This only works if the children have the temporal-decoupling API set up, in
  // which case we stop executing when that count expires, and return control to
  // this outer loop.
  ProgramStatus IssSystem::run_jit()
  {
    try {
      while (is_active()) {
        // Clear the temporal-decoupling counter for this iteration.  Each child
        // will return from run_jit() when its threshold is exceeded, so the
        // clear matters on the second and subsequent iteration.
        set_td_counter(0);
        ForEach(_children,i) {
          (**i).simple_run_jit();
        }
      }
    }
    catch (SimInterrupt) {
      return BREAK;
    }
    return HALT;    
  }

  // This only works if the children have the temporal-decoupling API set up, in
  // which case we stop executing when that count expires, and return control to
  // this outer loop.
  ProgramStatus IssSystem::run_dc()
  {
    try {
      while (is_active()) {
        // Clear the temporal-decoupling counter for this iteration.  Each child
        // will return from run_jit() when its threshold is exceeded, so the
        // clear matters on the second and subsequent iteration.
        set_td_counter(0);
        ForEach(_children,i) {
          (**i).simple_run_dc();
        }
      }
    }
    catch (SimInterrupt) {
      return BREAK;
    }
    return HALT;    
  }

  // Run all children, by default.
  ProgramStatus IssSystem::run(SimType type)
  {
    // FIXME:  Extend to handle JIT.
    switch (type) {
    case SimDc:
      return run_dc();
    case SimJit:
      return run_jit();
    default:
      return run();
    }
  }
  
  // Step all children, by default.
  ProgramStatus IssSystem::step()
  {
    try {
      simple_step();
      return (is_active()) ? ACTIVE : HALT;
    }
    catch (SimInterrupt) {
      return BREAK;
    }
  }

  // Step all chilren, by default.
  ProgramStatus IssSystem::stepn(unsigned n)
  {
    try {
      for (unsigned i = 0; i != n; ++i) {
        simple_step();
      }
      return (is_active()) ? ACTIVE : HALT;
    }
    catch (SimInterrupt) {
      return BREAK;
    }
  }

  // Tracing control.
  uint64_t IssSystem::set_tracing(uint64_t flags)
  {
    return 0;
  }

  uint64_t IssSystem::clear_tracing(uint64_t flags)
  {
    return 0;
  }

  uint64_t IssSystem::set_tracing_flags(uint64_t flags)
  {
    return 0;
  }

  bool IssSystem::tracing_on() const
  {
    return false;
  }

  bool IssSystem::tracing_on(uint64_t flags) const
  {
    return false;
  }

  uint64_t IssSystem::tracing_flags() const
  {
    return 0;
  }

  void IssSystem::dump_jit_code()
  {
    ForEach(_children,i) {
      (*i)->dump_jit_code();
    }
  }

  uint64_t IssSystem::set_tracing_r(uint64_t flags)
  {
    bool first = true;
    uint64_t rc = 0;
    ForEach(_children,i) {
      uint64_t trc = (*i)->set_tracing_r(flags);
      rc = (first) ? trc : rc;
      first = false;
    }
    return rc;
  }

  uint64_t IssSystem::clear_tracing_r(uint64_t flags)
  {
    bool first = true;
    uint64_t rc = 0;
    ForEach(_children,i) {
      uint64_t trc = (*i)->clear_tracing_r(flags);
      rc = (first) ? trc : rc;
      first = false;
    }
    return rc;
  }

  uint64_t IssSystem::set_tracing_flags_r(uint64_t flags)
  {
    bool first = true;
    uint64_t rc = 0;
    ForEach(_children,i) {
      uint64_t trc = (*i)->set_tracing_flags_r(flags);
      rc = (first) ? trc : rc;
      first = false;
    }
    return rc;
  }

  bool IssSystem::tracing_on_r() const
  {
    bool is_on = false;
    ForEach(_children,i) {
      is_on = is_on || (*i)->tracing_on_r();
    }
    return is_on;
  }

  bool IssSystem::tracing_on_r(uint64_t flags) const
  {
    bool is_on = false;
    ForEach(_children,i) {
      is_on = is_on || (*i)->tracing_on_r(flags);
    }
    return is_on;
  }

  // We set the parms on all children so that if they're not shared, it works,
  // but if they are shared, then it still works (the operation is just
  // duplicated).
  void IssSystem::set_dyn_parm(const std::string &parm,unsigned value)
  {
    ForEach(_children,i) {
      (*i)->set_dyn_parm(parm,value);
    }
  }

  // For now, just request from the first child.  We assume that we have a
  // homogenous system.
  unsigned IssSystem::get_dyn_parm(const std::string &parm) const
  {
    if (!_children.empty()) {
      return _children.front()->get_dyn_parm(parm);
    } else {
      return 0;
    }
  }

  // For now, just list the first child's parms.  We assume that these refer to
  // shared resources.
  void IssSystem::list_dyn_parm(StrPairs &parms) const
  {
    if (!_children.empty()) {
      _children.front()->list_dyn_parm(parms);
    }
  }

  uint64_t IssSystem::set_td_counter(uint64_t x)
  {
    uint64_t v = 0;
    ForEach(_children,i) {
      v = (*i)->set_td_counter(x);
    }
    return v;
  }

  unsigned IssSystem::set_td_increment(unsigned x)
  {
    unsigned v = 0;
    ForEach(_children,i) {
      v = (*i)->set_td_increment(x);
    }
    return v;
  }

  uint64_t IssSystem::set_td_threshold(uint64_t x)
  {
    uint64_t v = 0;
    ForEach(_children,i) {
      v = (*i)->set_td_threshold(x);
    }
    return v;
  }

  void IssSystem::cond_set_td_threshold(uint64_t cur_time,uint64_t new_time)
  {
    ForEach(_children,i) {
      (*i)->cond_set_td_threshold(cur_time,new_time);
    }
  }

  uint64_t IssSystem::get_td_counter() const
  {
    assert(!_children.empty());
    return _children.front()->get_td_counter();
  }

  uint64_t *IssSystem::get_td_counter_ptr()
  {
    assert(!_children.empty());
    return _children.front()->get_td_counter_ptr();
  }

  uint64_t IssSystem::get_td_threshold() const
  {
    assert(!_children.empty());
    return _children.front()->get_td_threshold();
  }

  unsigned IssSystem::get_td_increment() const
  {
    assert(!_children.empty());
    return _children.front()->get_td_increment();
  }

  bool IssSystem::setRegCallback(unsigned rindex,RegCallback *cb)
  {
    ForEach(_children,i) {
      if (!(*i)->setRegCallback(rindex,cb)) return false;
    }
    return true;
  }

  bool IssSystem::setRegCallback(const std::string &name,RegCallback *cb)
  {
    ForEach(_children,i) {
      if (!(*i)->setRegCallback(name,cb)) return false;
    }
    return true;
  }

  bool IssSystem::setExceptCallback(unsigned index,ExceptCallback *cb)
  {
    ForEach(_children,i) {
      if (!(*i)->setExceptCallback(index,cb)) return false;
    }
    return true;
  }

  bool IssSystem::setExceptCallback(const std::string &name,ExceptCallback *cb)
  {
    ForEach(_children,i) {
      if (!(*i)->setExceptCallback(name,cb)) return false;
    }
    return true;
  }

  void IssSystem::clearBreakpoints()
  {
    ForEach(_children,i) {
      (*i)->clearBreakpoints();
    }
  }

  void IssSystem::clearWatchpoints()
  {
    ForEach(_children,i) {
      (*i)->clearWatchpoints();
    }
  }

  unsigned IssSystem::setBreakpoint(addr_t addr,PtBase *payload)
  {
    unsigned h = 0;
    ForEach(_children,i) {
      h = (*i)->setBreakpoint(addr,payload);
    }
    return h;
  }

  unsigned IssSystem::setTmpBreakpoint(addr_t addr,PtBase *payload)
  {
    unsigned h = 0;
    ForEach(_children,i) {
      h = (*i)->setTmpBreakpoint(addr,payload);
    }
    return h;
  }

  bool IssSystem::clearBreakpointByAddr(addr_t addr)
  {
    ForEach(_children,i) {
      if (!(*i)->clearBreakpointByAddr(addr)) return false;
    }
    return true;
  }

  unsigned IssSystem::setWatchpoint(addr_t addr,WatchType type,PtBase *payload)
  {
    unsigned h = 0;
    ForEach(_children,i) {
      h = (*i)->setWatchpoint(addr,type,payload);
    }
    return h;
  }

  bool IssSystem::clearWatchpointByAddr(addr_t addr,WatchType type)
  {
    ForEach(_children,i) {
      if (!(*i)->clearWatchpointByAddr(addr,type)) return false;
    }
    return true;
  }
  
  // void IssSystem::checkForBreakpoint(addr_t addr)
  // {
    // ForEach(_children,i) {
      // (*i)->checkForBreakpoint(addr);
    // }
  // }

  const char *IssSystem::getCoreName() const 
  { 
    if (!_children.empty()) {
      return _children.front()->getCoreName();
    } else {
      return 0; 
    }
  }

  const char *IssSystem::getModelFileName() const 
  { 
    if (!_children.empty()) {
      return _children.front()->getModelFileName();
    } else {
      return 0; 
    }
  }
  
  const adl::AttrData &IssSystem::getAttrData() const 
  { 
    if (!_children.empty()) {
      return _children.front()->getAttrData();
    } else {
      static AttrData dummy; 
      return dummy; 
    }
  }

  const char *IssSystem::getModelPreamble() const 
  { 
    if (!_children.empty()) {
      return _children.front()->getModelPreamble();
    } else {
      return 0; 
    }
  }

  void IssSystem::resetGlobal() 
  {
    if (!_children.empty()) {
      _children.front()->resetGlobal();
    }
  }

  void IssSystem::setMemorySize(unsigned size) 
  {
    if (!_children.empty()) {
      _children.front()->setMemorySize(size);
    }
  }

  IssCore::IssCore (const std::string &name,const std::string &type,const IssNode *parent,
                    unsigned &id,unsigned child_id) : 
    IssNode(name,type,parent), 
    _logger(&dummy_logger),
    _trace_mode(0),
    _id(id++),
    _child_id(child_id)
  {};


  void IssCore::setLogger(LogBase *l)
  {
    if (l) {
      _logger = l;
    } else {
      _logger = &dummy_logger;
    }
  }

  unsigned IssCore::setBreakpoint(addr_t,PtBase *)
  {
    RError("Breakpoint support not enabled.");
  }

  unsigned IssCore::setTmpBreakpoint(addr_t,PtBase *)
  {
    RError("Temporary breakpoint support not enabled.");
  }

  unsigned IssCore::setWatchpoint(addr_t,WatchType type,PtBase *)
  {
    RError("Watchpoint support not enabled.");
  }
  
  void IssCore::checkForBreakpoint(addr_t addr) 
  {
    RError("Check For Breakpoint not enabled.");
  }

  // Note: The main version is implemented per core so that we can take into
  // account whether RNumber is being used or not.
  bool IssCore::logReg(const std::string &name,unsigned index) const
  {
    int rindex, rfindex;
    if (!getRegIndex(rindex,rfindex,name)) return false;
    if (rfindex < 0) rfindex = index;
    
    return logReg(rindex,rfindex);
  }

  // Tracing control.
  uint64_t IssCore::set_tracing(uint64_t flags)
  {
    uint64_t p = _trace_mode;
    _trace_mode |= flags;
    return p;  
  }

  uint64_t IssCore::clear_tracing(uint64_t flags)
  {
    uint64_t p = _trace_mode;
    _trace_mode &= ~flags;
    return p;
  }

  uint64_t IssCore::set_tracing_flags(uint64_t flags)
  {
    uint64_t p = _trace_mode;
    _trace_mode = flags;
    return p;
  }

  bool IssCore::tracing_on() const
  {
    return _trace_mode;
  }

  bool IssCore::tracing_on(uint64_t flags) const
  {
    return (_trace_mode & flags) == flags;
  }

  uint64_t IssCore::tracing_flags() const
  {
    return _trace_mode;
  }

  uint64_t IssCore::set_tracing_r(uint64_t flags)
  {
    return set_tracing(flags);
  }

  uint64_t IssCore::clear_tracing_r(uint64_t flags)
  {
    return clear_tracing(flags);
  }

  uint64_t IssCore::set_tracing_flags_r(uint64_t flags)
  {
    return set_tracing_flags(flags);
  }

  bool IssCore::tracing_on_r() const
  {
    return tracing_on();
  }

  bool IssCore::tracing_on_r(uint64_t flags) const
  {
    return tracing_on(flags);
  }


  ExHandle::ExHandle(const ExHandle &x) : _exitems(x._exitems)
  {
    if (_exitems) {
      _exitems->_ref++;
    }
  }

  ExHandle::~ExHandle()
  {
    reset();
  }

  void ExHandle::reset()
  {
    if (_exitems) {
      if (--_exitems->_ref == 0) {
        delete _exitems;
      }
    }
    _exitems = 0;
  }

  const ExHandle &ExHandle::operator=(const ExHandle &x)
  {
    reset();
    _exitems = x._exitems;
    if (_exitems) {
      _exitems->_ref++;
    }
    return *this;
  }

  ExItems &ExHandle::operator()()
  {
    return *_exitems;
  }

  void ExHandle::alloc(unsigned n)
  {
    if (!_exitems) {
      _exitems = new ExItems(n);
    }
  }

  // The global registry of event buses in the system.
  static EventBuses &eventBuses()
  {
    static EventBuses x;
    return x;
  }

  // Maps names to indices in eventBuses.
  static IndexMap &busMap()
  {
    static IndexMap x;
    return x;
  }

  void register_eventbus(int &handle,int &id,const string &name,const string &sig,EventBusHandler &handler)
  {
    pair<IndexMap::iterator,bool> ip = busMap().insert(make_pair(name,eventBuses().size()));

    int index;
    if (ip.second) {
      // New bus- add the item,
      index = eventBuses().size();
      eventBuses().push_back(EventBus(name,sig));
    } else {
      // Bus already exists.  Verify that the signatures match.
      index = ip.first->second;
      EventBus &eb = eventBuses()[index];
      if (sig != eb._sig) {
        RError("Signature mismatch when installing to bus " << name);
      }
    }

    EventBus &eb = eventBuses()[index];
    eb._handlers.push_back(&handler);
    handle = index;
    id = eb._count++;
  }

  void signal_eventbus(int handle,int id,const void *data)
  {
    EventBus &eb = eventBuses()[handle];

    for(HandlerList::iterator i = eb._handlers.begin(); i != eb._handlers.end(); ++i) {
      (**i)(data,id);
    }
  }

  ostream &operator<<(ostream &o,const MemAccess &ma)
  {
    switch (ma._op) {
    case MemAccess::opNone:
      o << "<mem nop>";
      break;
    case MemAccess::opCache:
      switch (ma._type) {
      case CacheInvalidate:
      case CacheFlush:
      case CacheStore:
        o << ma._type << dec << ": cache " << ma._id << ", set:  " << ma._c._set << ", way: " << ma._c._way;
        break;
      default:
        o << ma._type << hex << ": cache " << ma._id << ", ea: 0x" << ma._ea << ", ra: 0x" << ma._ra;
        break;
      }
      break;
    case MemAccess::opMem:
      o << ma._type << hex << ": ea: 0x" << ma._ea << ", ra: 0x" << ma._ra << dec << " (" << ma._size << " bytes)";
      break;
    }
    return o;
  }

  ostream &operator<<(ostream &o,const FieldData &fd)
  {
    o << "{ " << hex;
    ForEach(fd,i) {
      o << i->_name << " => 0x" << i->_value << ", ";
    }
    o << dec << "}";
    return o;
  }

  // C-like interface for FieldData, to make it easier to interface with some other languages.

  size_t getFieldDataSize(const FieldData *fd)
  {
    return fd->size();
  }

  void addFieldPair(FieldData *fd,const std::string &name,uint64_t value)
  {
    fd->push_back(FieldPair(name,value));
  }

  void setFieldPair(FieldData *fd,unsigned index,const std::string &name,uint64_t value)
  {
    fd->at(index) = FieldPair(name,value);
  }

  void getFieldPair(const FieldData *fd,unsigned index,std::string &name,uint64_t &value)
  {
    const FieldPair &fp = fd->at(index);
    name = fp._name;
    value = fp._value;
  }

  void printInstrPacket(std::ostream &o,const InstrPacketBase &x)
  {
    if (x._disasm) {
      x._disasm(o,x._addr,&x._opc[0]);
    } else {
      o << ".long 0x" << hex << &x._opc << dec;
    }
    if (x.getNumOps() > 0) {
      o << " [ ";
      ForRange(x.getNumOps(),i) {
        if (x.printOp(o,i)) {
          o << ' ';
        }
      }
      o << "]";
    }
  }

  void printInstrPacketResolved(std::ostream &o,const InstrPacketBase &x)
  {
    if (x._disasm) {
      x._disasm(o,x._addr,&x._opc[0]);
    } else {
      o << ".long 0x" << hex << &x._opc << dec;
    }
    if (x.getNumOps() > 0) {
      o << " [ ";
      ForRange(x.getNumOps(),i) {
        if (x.printOpResolved(o,i)) {
          o << ' ';
        }
      }
      o << "]";
    }
  }

  ostream &operator<<(ostream &o,const InstrPacketBase &x)
  {
    printInstrPacket(o,x);
    return o;
  }

  int checkRegFileName(const string &name,unsigned rflen,const char *rfname)
  {
    if (name.compare(0,rflen,rfname) == 0) {
      char *end = 0;
      int index = strtoul(&name[rflen],&end,10);
      if (end == 0 || *end == 0) {
        return index;
      }
    }
    return -1;
  }

  //
  // These are globals which can be adjusted by the startup code and modify
  // Fast-ISS behavior.  Refer to ModelInterface.h for what they mean.
  //

  unsigned TranslationParms::_mt_jit_compile         = 1;
  unsigned TranslationParms::_max_bb_instr_count     = 50;
  unsigned TranslationParms::_max_bb_count           = 5000;
  unsigned TranslationParms::_jit_bb_hot_threshold   = 100;
  unsigned TranslationParms::_dc_bb_hot_threshold    = 2;
  unsigned TranslationParms::_dc_buffer_size         = 20000;
  
  //
  // Routines for modifying dynamic parameters.
  //
  
  // Cache parameters.
  bool set_model_cache_parm (unsigned &linesize,unsigned &linemask,unsigned &lineshift,
                             unsigned &sets,unsigned &ways,const std::string &pfx,const std::string &parm,unsigned value)
  {
    if (parm == pfx+"cache-line-size") {
      if (value == 0) {
        RError("Cache line size must be non-zero.");
      }
      if (!is_power_2(value)) {
        RError("Cache line size must be a power of 2.");
      }
      if (value % 4) {
        RError("Cache linesize must be an even number of 32-bit words.");
      }
      linesize  = value;
      linemask  = value-1;
      lineshift = count_trailing_zeros(linesize);
      return true;
    }
    else if (parm == pfx+"cache-size") {
      if (value == 0) {
        RError("Cache suize must be non-zero.");
      }
      if (!is_power_2(value)) {
        RError("Cache size must be a power of 2.");
      }
      sets = value / (ways * linesize);
      return true;
    }
    else if (parm == pfx+"cache-num-sets") {
      if (value == 0) {
        RError("Cache set size must be non-zero.");
      }
      if (!is_power_2(value)) {
        RError("Cache set size must be a power of 2.");
      }
      sets = value;
      return true;
    }
    else if (parm == pfx+"cache-num-ways") {
      if (value == 0) {
        RError("Cache way size must be non-zero.");
      }
      ways = value;
      return true;
    }
    else {
      return false;
    }
  }

  bool get_model_cache_parm (unsigned &value,const string &pfx,const std::string &parm,unsigned linesize,unsigned sets,unsigned ways)
  {
    if (parm == pfx+"cache-line-size") {
      value = linesize;
      return true;
    }
    if (parm == pfx+"cache-size") {
      value = sets * ways * linesize;
      return true;
    }
    else if (parm == pfx+"cache-num-sets") {
      value = sets;
      return true;
    }
    else if (parm == pfx+"cache-num-ways") {
      value = ways;
      return true;
    }
    else {
      return false;
    }
  }

  void list_model_cache_parm(const string &pfx,const string &name,StrPairs &parms)
  {
    parms.push_back(make_pair(pfx+"cache-size", "Specify the "+name+" cache's size in bytes, using the existing line-size and number of ways."));
    parms.push_back(make_pair(pfx+"cache-line-size","Specify the "+name+" cache's line size (in bytes)."));
    parms.push_back(make_pair(pfx+"cache-num-sets", "Specify the number of sets in the "+name+" cache."));
    parms.push_back(make_pair(pfx+"cache-num-ways", "Specify the number of ways per set in the "+name+" cache."));
  }

  // MMU lookup parameters.
  bool set_model_lookup_parm (unsigned *sets,unsigned *set_mask,unsigned *set_shift,
                              unsigned &ways,const std::string &pfx,const std::string &parm,unsigned value)
  {
    if (sets && parm == pfx+"tlb-num-sets") {
      if (value == 0) {
        RError("TLB set size size must be non-zero.");
      }
      if (!is_power_2(value)) {
        RError("TLB set size size must be a power of 2.");
      }
      *sets      = value;
      *set_mask  = value-1;
      *set_shift = count_trailing_zeros(*sets);
      return true;
    }
    else if (parm == pfx+"tlb-num-ways") {
      if (value == 0) {
        RError("TLB way size must be non-zero.");
      }
      ways = value;
      return true;
    }
    else {
      return false;
    }
  }

  bool get_model_lookup_parm (unsigned &value,const std::string &pfx,const std::string &parm,
                              unsigned sets,unsigned ways)
  {
    if (sets && parm == pfx+"tlb-num-sets") {
      value = sets;
      return true;
    }
    else if (parm == pfx+"tlb-num-ways") {
      value = ways;
      return true;
    }
    else {
      return false;
    }
  }

  void list_model_lookup_parm(const std::string &pfx,const std::string &name,StrPairs &parms,bool fully_assoc)
  {
    if (!fully_assoc) {
      parms.push_back(make_pair(pfx+"tlb-num-sets", "Specify the number of sets in the "+name+" TLB."));
    }
    parms.push_back(make_pair(pfx+"tlb-num-ways", "Specify the number of ways per set in the "+name+" TLB."));
  }

  // Copy data from an external buffer to the internal instruction buffer for
  // direct execution of instruction data, bypassing fetch.
  void copy_to_instr_buf(uint32_t *trg,uint32_t *src,unsigned t_n,unsigned s_n,bool byte_swap)
  {
    unsigned s = min(t_n,s_n);
    unsigned i = 0;
    if (byte_swap) {
      for (; i != s; ++i) {
        trg[i] = swap_uint32(src[i]);
      }
    } else {
      for (; i != s; ++i) {
        trg[i] = src[i];
      }
    }
    for (; i != t_n; ++i) {
      trg[i] = 0;
    } 
  }

    
  // Display 32-bit number with text
  void disp(const char * text, uint32_t i, bool in_hex) {
      if (in_hex) {
        std::cout << text << std::hex << std::setfill('0') << std::setw(8) << i << std::dec;
      } else {
        std::cout << text << i;
      }
      
  }    

  // Display text info
  void disp(const char * text) {
      std::cout << text;
  }   

//
// Implementation for the aligned accesses.
//

# undef  INLINE_ALIGNED_ACCESSES
# undef  _ALIGNED_ACCESS_H_
# define IMPL_ALIGNED_ACCESSES

namespace noinline_accesses {
# include "iss/AlignedAccess.h"  
}
  
}


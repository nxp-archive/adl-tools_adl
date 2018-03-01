//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Implementation file for the scripting-interface callback logger.  This stores
// functions which will be invoked when various tracing events occur.
//

#include <vector>
#include <sstream>
#include <stdexcept>

#include "rnumber/RNumber.h"

#include "helpers/Macros.h"

#include "ModelInterface.h"
#include "SimInterface.h"
#include "ScriptingIface.h"
#include "ScriptLogger.h"
#include "ScriptLoggerImpl.h"
#include "TargetHash.h"
#include "TypeIO.h"

using namespace std;
using namespace __gnu_cxx;

namespace adl {

  struct MyFieldData : public FieldData {};

  ostream &operator<<(ostream &o,const MyFieldData &fd)
  {
    ForEach(fd,i) {
      o << " {" << i->_name << " " <<hex<< i->_value << "}";
    }
    return o;
  }


  ScriptLoggerImpl::ScriptLoggerImpl() : _scripter(scripter()) 
  {
  };

  ScriptLoggerImpl::~ScriptLoggerImpl() 
  {
  }

  void ScriptLoggerImpl::reset()
  {
  }

  void ScriptLoggerImpl::registerOurself()
  {
    registerLogger(this);
  }

  void ScriptLoggerImpl::unregisterOurself()
  {
    unregisterLogger(this);
  }


  // Given an event name and an optional filter value (empty indicates
  // nonexistant), returns a map pointer/id pair.
  pair<FuncMap *,int> ScriptLoggerImpl::get_event_map(const Target &target,const string &event,const string &filter)
  {
    int id = -1;

    if (event == "instr") {
      return make_pair(&_log_instr_map,-1);
    } 
    else if (event == "instr_args") {
      return make_pair(&_log_instr_args_map,-1);
    }
    else if (event == "basic_block_end") {
      return make_pair(&_log_basic_block_end_map,-1);
    } 
    else if (event == "reg_write") {
      id = get_filter_id<RegInfos>(target,filter,&IssNode::getRegInfo,"registers");
      return make_pair(&_log_reg_write_map,id);
    }
    else if (event == "reg_read") {
      id = get_filter_id<RegInfos>(target,filter,&IssNode::getRegInfo,"registers");
      return make_pair(&_log_reg_read_map,id);
    }
    else if (event == "regfile_write") {
      id = get_filter_id<RegInfos>(target,filter,&IssNode::getRegInfo,"register files");
      return make_pair(&_log_regfile_write_map,id);
    }
    else if (event == "regfile_read") {
      id = get_filter_id<RegInfos>(target,filter,&IssNode::getRegInfo,"register files");
      return make_pair(&_log_regfile_read_map,id);
    }
    else if (event == "mmu_translation") {
      return make_pair(&_log_mmu_map,-1);
    }
    else if (event == "cache_action") {
      return make_pair(&_log_cache_map,-1);
    }
    else if (event == "mem_write") {
      if (filter == GlobalMemName) {
        id = GlobalMemId;
      } else {
        id = get_filter_id<MemInfos>(target,filter,&IssNode::getMemInfo,"memories");
      }
      return make_pair(&_log_memwrite_map,id);
    }
    else if (event == "mem_read") {
      if (filter == GlobalMemName) {
        id = GlobalMemId;
      } else {
        id = get_filter_id<MemInfos>(target,filter,&IssNode::getMemInfo,"memories");
      }
      return make_pair(&_log_memread_map,id);
    }
    else if (event == "exception") {
      id = get_filter_id<ExceptionInfos>(target,filter,&IssNode::getExceptionInfo,"exceptions");
      return make_pair(&_log_exception_map,id);
    }
    else if (event == "break") {
      return make_pair(&_log_breakpoint_map,-1);
    }
    else if (event == "watch") {
      return make_pair(&_log_watchpoint_map,-1);
    }
    else {
      RError("Unknown logging event " << event);
    }
  }

  // Add a function to the specified event, optionally filtering by the value
  // specified by filter.  Additional filter data is added, if supplied.
  void ScriptLoggerImpl::add_event(const Target &target,const string &event,const string &func,const string &filter,
                                   unsigned filtercount,addr_t filter1,addr_t filter2)
  {
    bool was_empty = empty();
    
    auto fmap = get_event_map(target,event,filter);
    
    FuncItem &fi = fmap.first->add(target,fmap.second,func);
    
    fi.set_filter_data(filtercount,filter1,filter2);
    
    // If we were empty, then we add ourselves to the set of loggers.
    if (was_empty) {
      registerOurself();
    }
  }

  // Add a function to the specified event, optionally filtering using an index.
  void ScriptLoggerImpl::add_event(const Target &target,const string &event,const string &func,int id)
  {
    bool was_empty = empty();

    auto fmap = get_event_map(target,event,"");

    fmap.first->add(target,id,func);

    // If we were empty, then we add ourselves to the set of loggers.
    if (was_empty) {
      registerOurself();
    }
  }

  // Removes a function of the specified name from the specified event.
  // Returns the total number of functions removed.
  int ScriptLoggerImpl::del_event(const Target &target,const string &event,const string &func)
  {
    
    auto fmap = get_event_map(target,event,"");

    int count = fmap.first->del(target,func);

    // If we're now empty, then we remove ourselves from the set of loggers.
    // There's no need to have the overhead of an extra writer when we won't
    // be called.
    if (empty()) {
      unregisterOurself();
    }

    return count;
  }

  bool ScriptLoggerImpl::empty() const
  {
    return 
      _log_instr_map.empty() &&
      _log_instr_args_map.empty() &&
      _log_basic_block_end_map.empty() &&
      _log_reg_write_map.empty() &&
      _log_reg_read_map.empty() &&
      _log_regfile_write_map.empty() &&
      _log_regfile_read_map.empty() &&
      _log_mmu_map.empty() &&
      _log_cache_map.empty() &&
      _log_memread_map.empty() &&
      _log_memwrite_map.empty() &&
      _log_exception_map.empty() &&
      _log_breakpoint_map.empty() &&
      _log_watchpoint_map.empty();
  }

  struct ScriptLoggerObj;

  struct ScriptLoggerObj : public LoggingIface {

    TFuncs            _funcs;

    ScriptLoggerImpl &_parent;
    ScriptingIface   &_scripter;
    Target            _core;

    string            _result;
    ostringstream     _ss;
    addr_t            _instr_addr;

    ScriptLoggerObj(ScriptLoggerImpl &parent,IssCore &c) :
      _parent(parent),
      _scripter(parent._scripter),
      _core(&c),
      _instr_addr(0)
    {};

    // Simple RAII class for temporarily changing the scripting target.
    struct TargetState {

      TargetState(ScriptLoggerObj &sl) :
        _scripter(sl._scripter),
        _orig(_scripter.setPath(sl._core))
      {};

      ~TargetState()
      {
        _scripter.setPath(_orig);
      }

      ScriptingIface &_scripter;
      Target          _orig;
    };

    // Invokes all functions for function map 'fmap' listed in _funcs with arguments specified by x.
# define Invoke(x) {                                                \
      if (!_funcs.empty()) {                                        \
        _ss.str(""); _ss << dec << ' ' << x;                        \
        TargetState ts(*this);                                      \
        ForEach(_funcs,i) {                                         \
          if (*i) { /* Check for a null (was filtered). */          \
            if (!_scripter.eval(_result,(*i)->_func + _ss.str())) { \
              RError((*i)->_func << ":  " << _result);              \
            }                                                       \
          }                                                         \
        }                                                           \
      }                                                             \
    }

    void logInstrPrefetch(addr_t ea)
    {
      _instr_addr = ea;
    }

    void logInstr(const uint32_t *opc,int num_half_bytes,const char* name, Disassembler dis,uint32_t flags)
    {
      get_matching_events(_funcs,_core,_parent._log_instr_map,-1,NullFilter());
      unsigned w = (num_half_bytes+7)/8;
      rnumber::RNumber opcode = rnumber::RNumber(opc,w,32*w);
      unsigned shift = (32*opcode.wordCount() - 4*num_half_bytes);
      opcode >>= shift;
      if (num_half_bytes) {
        opcode.resize(4*num_half_bytes);
      }
      Invoke(_instr_addr << " " << opcode << " " << name);
    }    

    void logInstrArgs(const uint32_t *opc,int num_half_bytes,const char* name, Disassembler dis)
    {
      get_matching_events(_funcs,_core,_parent._log_instr_args_map,-1,NullFilter());
      unsigned w = (num_half_bytes+7)/8;
      rnumber::RNumber opcode = rnumber::RNumber(opc,w,32*w);
      unsigned shift = (32*opcode.wordCount() - 4*num_half_bytes);
      opcode >>= shift;
      if (num_half_bytes) {
        opcode.resize(4*num_half_bytes);
      }
      ostringstream ss;
      
      dis(ss,0,opc);  // inputs
      string s1 = ss.str();
      ss.str("");
      dis(ss,1,opc);  // outputs
      string s2 = ss.str();
      Invoke(opcode << " " << name << " {" << s1 << "} {" << s2 << "}");
    }    

    void logBasicBlockEnd(addr_t ea)
    {
      get_matching_events(_funcs,_core,_parent._log_basic_block_end_map,-1,NullFilter());
      Invoke(ea);
    }    


    void logRegWrite(unsigned id,const char *name,const rnumber::RNumber &value)
    {
      get_matching_events(_funcs,_core,_parent._log_reg_write_map,id,NullFilter());

      Invoke(id << " " << name << " " << value);
    }

    void logRegRead(unsigned id,const char *name,const rnumber::RNumber &value)
    {
      get_matching_events(_funcs,_core,_parent._log_reg_read_map,id,NullFilter());

      Invoke(id << " " << name << " " << value);
    }

    struct FilterIndex {
      bool operator()(const FuncItem &fi) const {
        // We allow it if we don't have a filter or else the first filter
        // element matches the index.
        return (!fi._filtercount || (fi._filter1 == _index));
      };
      FilterIndex(uint32_t index) : _index(index) {};
      uint32_t _index;
    };

    void logRegfileWrite(unsigned id,const char *name,uint32_t index,const rnumber::RNumber &value)
    {
      get_matching_events(_funcs,_core,_parent._log_regfile_write_map,id,FilterIndex(index));

      Invoke(id << " " << name << " " << index << " " << value);
    }

    void logRegfileRead(unsigned id,const char *name,uint32_t index,const rnumber::RNumber &value)
    {
      get_matching_events(_funcs,_core,_parent._log_regfile_read_map,id,FilterIndex(index));

      Invoke(id << " " << name << " " << index << " " << value);
    }

    void logRegWriteMask(unsigned id,const char *name,const rnumber::RNumber &value,const rnumber::RNumber &mask)
    {
      get_matching_events(_funcs,_core,_parent._log_reg_write_map,id,NullFilter());

      Invoke(id << " " << name << " " << value);
    }

    void logRegReadMask(unsigned id,const char *name,const rnumber::RNumber &value,const rnumber::RNumber &mask)
    {
      get_matching_events(_funcs,_core,_parent._log_reg_read_map,id,NullFilter());

      Invoke(id << " " << name << " " << value);
    }

    void logRegfileWriteMask(unsigned id,const char *name,uint32_t index,const rnumber::RNumber &value,const rnumber::RNumber &mask)
    {
      get_matching_events(_funcs,_core,_parent._log_regfile_write_map,id,FilterIndex(index));

      Invoke(id << " " << name << " " << index << " " << value);
    }

    void logRegfileReadMask(unsigned id,const char *name,uint32_t index,const rnumber::RNumber &value,const rnumber::RNumber &mask)
    {
      get_matching_events(_funcs,_core,_parent._log_regfile_read_map,id,FilterIndex(index));

      Invoke(id << " " << name << " " << index << " " << value);
    }

    void logMmuTranslation(TransType tt, int seq, int set, int way, addr_t ea, addr_t ra, const MmuBase* trans) 
    {
      get_matching_events(_funcs,_core,_parent._log_mmu_map,-1,NullFilter());

      MyFieldData fd;
      trans->report(fd);
      Invoke(trans->name() << " " << tt << " " << set << " " << way << " " << ea << " " << ra << " " << fd);
    }

    void logCacheAction(const char *name,CacheAction ca,CacheAccess cac,unsigned level,int set,int way,unsigned linemask,addr_t ra)
    {
      get_matching_events(_funcs,_core,_parent._log_cache_map,-1,NullFilter());

      Invoke(name << " " << ca << " " << cac << " " << level << " " << set << " " << way << " " << linemask << " " << ra);
    }

    struct FilterMemRange {
      bool operator()(const FuncItem &fi) const {
        // We allow it if we don't have a filter or else the first filter
        // element matches the index.
        switch (fi._filtercount) {
        case 1:
          // One filter: Address must match exactly.
          return (_addr == fi._filter1);
        case 2:
          // Two filter values: Address must fall into the specified range
          // (inclusive).
          return (_addr >= fi._filter1 && _addr <= fi._filter2);
        }
        // Default case- no filter, so allow.
        return true;
      };
      FilterMemRange(addr_t addr) : _addr(addr) {};
      addr_t _addr;
    };

    void logMemWrite(unsigned id,const char *name,bool pre,int seq,addr_t ea, addr_t ra, uint32_t value)
    {
      get_matching_events(_funcs,_core,_parent._log_memwrite_map,id,FilterMemRange(ra));

      Invoke(id << " " << name << " " << pre << " " << seq << " " << ea << " " << ra << " " << value);
    }

    void logMemRead(unsigned id,const char *name,bool pre,int seq,addr_t ea, addr_t ra, uint32_t value)
    {
      get_matching_events(_funcs,_core,_parent._log_memread_map,id,FilterMemRange(ra));

      Invoke(id << " " << name << " " << pre << " " << seq << " " << ea << " " << ra << " " << value);
    }

    void logException(unsigned id,bool pre,const char *name) 
    {
      get_matching_events(_funcs,_core,_parent._log_exception_map,id,NullFilter());

      Invoke(id << " " << pre << " " << name);
    }

    void logBreakpoint(addr_t ea,unsigned id,PtBase *) 
    {
      get_matching_events(_funcs,_core,_parent._log_breakpoint_map,id,NullFilter());

      Invoke(id << " " << ea);

      // If the return value of the breakpoint callback is "simstop", then we
      // throw a stop-now exception in order to halt execution.
      if (_result == "simstop") {
        throw SimInterrupt();
      }
    }

    void logWatchpoint(addr_t ea,bool pre,WatchType w,int num_bytes,uint64_t value,unsigned id,PtBase *payload) 
    {
      get_matching_events(_funcs,_core,_parent._log_watchpoint_map,id,NullFilter());

      Invoke(id << " " << w << " " << ea << " " << pre << " " << num_bytes << " " << value);

      // If the return value of the breakpoint callback is "simstop", then we
      // throw a stop-now exception in order to halt execution.
      if (_result == "simstop") {
        throw SimInterrupt();
      }
    }
  
  };

  LoggingIface *ScriptLoggerImpl::register_core(IssCore &core)
  {
    return new ScriptLoggerObj(*this,core);
  }


  ScriptLogger *makeScriptLogger() 
  {
    return new ScriptLoggerImpl; 
  }

}

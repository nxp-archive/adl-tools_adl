//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Handler classes for the uADL parser.
//

#include <iostream>
#include <cassert>

#include "opencxx/Environment.h"

#include "helpers/Macros.h"

#include "fe/Helpers.h"
#include "fe/ParsingHelpers.h"

#include "UadlParser.h"
#include "UadlHandlers.h"

using namespace std;
using namespace std::placeholders;
using namespace adl;
using namespace Opencxx;
using namespace PtreeUtil;


namespace uadl {

  class FetchUnit;
  class PipelineStage;
  class Semaphore;
  class Flag;
  class RegisterFile;
  class ForwardingPath;
  class BranchPredictor;
  class MemoryBase;
  class Memory;
  class Cache;
  class MemBaseHandler;
  class MemAliasHandler;
  class InstClass;


  HandlerList(MngrsHandler);
  HandlerList(ThreadHandler);
  HandlerList(FetchUnitHandler);
  HandlerList(StageHandler);
  HandlerList(SemaphoreHandler);
  HandlerList(FlagHandler);
  HandlerList(RegFileHandler);
  HandlerList(FwdPathHandler);
  HandlerList(BranchPredHandler);
  HandlerList(MemHandler);
  HandlerList(CacheHandler);
  HandlerList(MachineHandler);
  HandlerList(InstClassHandler);
  HandlerList(MemPortHandler);
  HandlerList(MappingHandler);
  HandlerList(ExceptionMappingHandler);
  HandlerList(OperandHandler);
  HandlerList(FuncHandler);
  HandlerList(InstrOpHandler);
  HandlerList(CustomHandler);
  HandlerList(CustomArgHandler);
  HandlerList(MemAliasHandler);

  // Definition of an individual manager block.

  class SemaphoreHandler : public IrHandler {
  protected:
    friend class MngrsHandler;

    Semaphore *_semaphore;

  public:
    SemaphoreHandler(Environment *e,Ptree *p,const gc_string &n,bool modify,UadlIr *ir) : 
      IrHandler(e,p,n,modify,ir),
      _semaphore(0)
    {
    }
    
    void handle_key(const char *key,Ptree *value)
    {
      if (!strcmp(key,"count")) {    
        _semaphore->_count = get_uint(value,"count value",env());
      } else {
        unknown_key(value,key);
      }
    }
  };

  class FlagHandler : public IrHandler {
  protected:
    friend class MngrsHandler;

    Flag *_flag;

  public:
    FlagHandler(Environment *e,Ptree *p,const gc_string &n,bool modify,UadlIr *ir) : 
      IrHandler(e,p,n,modify,ir),
      _flag(0)
    {
    }
  };

  // For handling arguments in a custom resource.
  class CustomArgHandler : public IrHandler {
  public:

    CustomItem    *_c;

    CustomArgHandler(Environment *e,Ptree *p,bool modify,UadlIr *ir) : 
      IrHandler(e,p,gc_string(),modify,ir),
      _c(0)
    {
    }

    void handle_key(const char *key,Ptree *value)
    {
      // All keys are taken to be arguments.
      _c->_parms[key] = value;
    }
  };

  // Definition of an individual manager block.
  class CustomHandler : public IrHandler {
  protected:
    friend class MngrsHandler;

    CustomArgHandlers  _args;
    CustomResource    *_c;

  public:
    CustomHandler(Environment *e,Ptree *p,const gc_string &n,bool modify,UadlIr *ir) : 
      IrHandler(e,p,n,modify,ir),
      _c(0)
    {
    }

    Handler *add_define(MsgLog &ml,Environment *e,Ptree *p,const char *type,const char *name,bool modify)
    {
      if (!strcmp(type,"args")) {
        if (name) {
          PError(p,"Type 'args' does not take a name.");
        }
        auto cah = new CustomArgHandler(e,p,modify,_ir);
        _args.push_back(cah);
        return cah;
      }
      else {
        unknown_define(p,type,name);
      }
      assert(0);
    }

    void handle_key(const char *key,Ptree *value)
    {
      if (!strcmp(key,"header")) {    
        _c->_header = get_str(value,"header filename");
      }
      else if (!strcmp(key,"type")) {    
        _c->_type = get_str(value,"type name");
      } 
      else {
        unknown_key(value,key);
      }
    }

    void process_keys(MsgLog &ml)
    {
      IrHandler::process_keys(ml);

      // Process the ports
      process_unnamed_handler(ml,_args,*this,&CustomHandler::setup_arg_handler);
    }

    void setup_arg_handler(CustomArgHandler &cah)
    {
      cah._c = _c;
    }

  };

  // Definition of an individual manager block.
  class RegFileHandler : public IrHandler {
  protected:
    friend class MngrsHandler;

    bool          _is_reg;
    RegisterFile *_regfile;

  public:
    RegFileHandler(Environment *e,Ptree *p,const gc_string &n,bool modify,bool is_reg,UadlIr *ir) : 
      IrHandler(e,p,n,modify,ir),
      _is_reg(is_reg),
      _regfile(0)
    {
    }

    void handle_key(const char *key,Ptree *value)
    {
      if (!strcmp(key,"latency")) {    
        _regfile->_latency = get_uint(value,"register latency",env());
      } else {
        unknown_key(value,key);
      }
    }
  };



  // Definition of an individual manager block.
  class FwdPathHandler : public IrHandler {
  protected:
    friend class MngrsHandler;

    ForwardingPath *_fp;

  public:
    FwdPathHandler(Environment *e,Ptree *p,const gc_string &n,bool modify,UadlIr *ir) : 
      IrHandler(e,p,n,modify,ir),
      _fp(0)
    {
    }

    void handle_key(const char *key,Ptree *value)
    {
      if (!strcmp(key,"broadcast")) {
        _fp->_broadcast = get_bool(value,"broadcasting flag.",env());
      } else if (!strcmp(key,"size")) {
        _fp->_size = get_uint(value,"forwarding path size.",env());
      } else if (!strcmp(key,"width")) {
        _fp->_width = get_uint(value,"forwarding path width.",env());
      } else {
        unknown_key(value,key);
      }
    }
  };

  // Definition of an individual manager block.
  class BranchPredHandler : public IrHandler {
  protected:
    friend class MngrsHandler;

    CustomArgHandlers  _args;
    BranchPredictor   *_bp;
  public:
    BranchPredHandler(Environment *e,Ptree *p,const gc_string &n,bool modify,UadlIr *ir) : 
      IrHandler(e,p,n,modify,ir),
      _bp(0)
    {
    }

    Handler *add_define(MsgLog &ml,Environment *e,Ptree *p,const char *type,const char *name,bool modify)
    {
      if (!strcmp(type,"args")) {
        if (name) {
          PError(p,"Type 'args' does not take a name.");
        }
        auto bpah = new CustomArgHandler(e,p,modify,_ir);
        _args.push_back(bpah);
        return bpah;
      }
      else {
        unknown_define(p,type,name);
      }
      assert(0);
    }

    void handle_key(const char *key,Ptree *value)
    {
      if (!strcmp(key,"algorithm")) {
        _bp->_algorithm = get_ident(value,"branch predictor algorithm.");
      } else if (!strcmp(key,"header")) {
        _bp->_header = get_str(value,"branch predictor header.");
      } else if (!strcmp(key,"size")) {
        _bp->_size = get_uint(value,"branch predictor size.",env());
      } else if (!strcmp(key,"counter_width")) {
        _bp->_counterWidth = get_uint(value,"counter width.",env());
      } else if (!strcmp(key,"enable")) {
        _bp->_enable._p = get_func(value,0,"branch-predictor enable predicate.");
      } else if (!strcmp(key,"reg_compares")) {
        handle_list_or_item(value,bind(&BranchPredHandler::add_reg_compare,this,_1));
      } else {
        unknown_key(value,key);
      }
    }

    void add_reg_compare(Ptree *p)
    {
      Ptree *r,*f;
      const char *r_s = 0, *f_s = 0;
      if (Match(p,"[ %? . %? ]",&r,&f)) {
        r_s = get_simple_ident(r," register-compare register name.");
        f_s = get_simple_ident(f," register-compare register-field name.");
        _bp->_reg_compares.push_back(BranchPredictor::RegCompare(r_s,f_s));
      } else if ((r_s = get_simple_ident(p,0))) {
        _bp->_reg_compares.push_back(BranchPredictor::RegCompare(r_s));
      } else {
        PError(p,"Expected a register name or register.field item for predictor register-compare entry, but got '" << p << "'.");
      }
    }

    void process_keys(MsgLog &ml)
    {
      IrHandler::process_keys(ml);

      // Process the ports
      process_unnamed_handler(ml,_args,*this,&BranchPredHandler::setup_arg_handler);
    }

    void setup_arg_handler(CustomArgHandler &bpah)
    {
      bpah._c = _bp;
    }

  };

  class MemPortHandler : public IrHandler {
  protected:
    friend class MemBaseHandler;
    
    MemoryPort *_p;

  public:
    MemPortHandler(Environment *e,Ptree *p,const gc_string &n,bool modify,UadlIr *ir) : 
      IrHandler(e,p,n,modify,ir),
      _p(0)
    {
    }


    void handle_key(const char *key, Ptree *value) 
    {
      if (!strcmp(key,"addr_bandwidth")) {
        _p->_addrBandwidth = get_uint(value,"port address bandwidth.",env());
        _p->_commonBandwidth = false;
      } else if (!strcmp(key,"data_bandwidth")) {
        _p->_dataBandwidth = get_uint(value,"port data bandwidth.",env());
        _p->_commonBandwidth = false;
      } else if (!strcmp(key,"bandwidth")) {
        _p->_addrBandwidth = _p->_dataBandwidth = get_uint(value,"port bandwidth.",env());
        _p->_commonBandwidth = true;
      } else if (!strcmp(key,"allow_bus_wait")) {
        _p->_allowBusWait = get_bool(value,"allow waiting request on bus.",env());
        _p->_allowBusWaitSpecified = true;
      } else if (!strcmp(key,"read_latency")) {
        _p->_readLatency = get_uint(value,"port read latency.",env());
      } else if (!strcmp(key,"write_latency")) {
        _p->_writeLatency = get_uint(value,"port write latency.",env());
      } else if (!strcmp(key,"max_requests")) {
        _p->_maxRequest = get_uint(value,"port max requests.",env());
      } else if (!strcmp(key,"type")) {
        _p->_type = get_cache_type(get_ident(value,"memory port type."));
      } else {
        unknown_key(value,key);
      }
    }


  };  




  // Definition of an individual manager block.
  class MemBaseHandler : public IrHandler {
  protected:
    friend class MngrsHandler;

    MemPortHandlers _ports;

    MemoryBase *_mb;

  public:
    MemBaseHandler(Environment *e,Ptree *p,const gc_string &n,bool modify,UadlIr *ir) : 
      IrHandler(e,p,n,modify,ir),
      _mb(0)
    {
    }



    bool handle_key_internal(const char *key,Ptree *value)
    {
      if (!strcmp(key,"addr_bandwidth")) {
        _mb->_addrBandwidth = get_uint(value,"address bandwidth.",env());
      } else if (!strcmp(key,"data_bandwidth")) {
        _mb->_dataBandwidth = get_uint(value,"data bandwidth.",env());
      } else if (!strcmp(key,"data_width")) {
        _mb->_dataWidth = get_uint(value,"data width.",env());
      } else if (!strcmp(key,"max_requests")) {
        _mb->_maxRequest = get_uint(value,"max requests.",env());
      } else if (!strcmp(key,"allow_bus_wait")) {
        _mb->_allowBusWait = get_bool(value,"allow waiting request on bus.",env());
      } else if (!strcmp(key,"preemptive")) {
        _mb->_preemptive = get_bool(
                                    value,
                                    "allow load/store to preempt a bus-waiting fetch.",
                                    env()
                                    );
      } else {
        return false;
      }
      return true;
    }

    void handle_key(const char *key,Ptree *value)
    {
      if (!handle_key_internal(key,value)) {
        unknown_key(value,key);
      }
    }

    Handler *add_define(MsgLog &ml,Environment *e,Ptree *p,const char *type,const char *name,bool modify)
    {
      if (!strcmp(type,"port")) {
        check_ident(name,p,"Port");
        auto ph = new MemPortHandler(e,p,name,modify,_ir);
        ph->_ir = _ir;
        _ports.push_back(ph);
        return ph;
      }
      else {
        unknown_define(p,type,name);
      }
      assert(0);
    }
  
  
    void process_keys(MsgLog &ml)
    {
      IrHandler::process_keys(ml);

      // Process the ports
      process_named_handler(ml,_ports,_mb->_ports,*this,&MemBaseHandler::setup_memport_handler);
    }

    void setup_memport_handler(MemPortHandler &ph , MemoryPort &p)
    {
      ph._p = &p;
    }

  };


  // Definition of an individual manager block.
  class MemHandler : public MemBaseHandler {
  protected:
    friend class MngrsHandler;
    Memory *_m;

  public:
    MemHandler(Environment *e,Ptree *p,const gc_string &n,bool modify,UadlIr *ir) : 
      MemBaseHandler(e,p,n,modify,ir),
      _m(0)
    {
    }

    void handle_key(const char *key,Ptree *value)
    {
      if (!strcmp(key,"read_latency")) {
        _m->_readLatency = get_uint(value,"read latency.",env());
      } else if (!strcmp(key,"write_latency")) {
        _m->_writeLatency = get_uint(value,"write latency.",env());
      } else if (!strcmp(key,"read_only")) {
        _m->_readOnly = get_bool(value,"read-only flag.",env());
      } else {
        MemBaseHandler::handle_key(key,value);
      }
    }


  };



  // Definition of an individual manager block.
  class CacheHandler : public MemBaseHandler {
  protected:
    friend class MngrsHandler;

    Cache *_c;

  public:
    CacheHandler(Environment *e,Ptree *p,const gc_string &n,bool modify,UadlIr *ir) : 
      MemBaseHandler(e,p,n,modify,ir),
      _c(0)
    {
    }

    void handle_key(const char *key,Ptree *value)
    {
      if (!strcmp(key,"next_level_memory")) {
        Ptree *m,*p;
        if (Match(value,"[ %? . %? ]",&m,&p)) {
          _c->_memory = get_ident(m,"next-level memory.");
          _c->_memoryPortName = get_ident(p,"next-level memory port.");
        } else {
          _c->_memory = get_ident(value,"next-level memory.");
        }
      } else if (!strcmp(key,"custom")) {
        Ptree *c,*h;
        if (Match(value,"[ ( [ %? , %? ] ) ]",&c,&h)) {
          _c->_custom_class = get_ident(c,"custom-model class name.");
          _c->_custom_header = get_str(h,"custom-model header file.");
        } else {
          PError(value,"Incorrect custom-class specifier.  Expected ( <class-name> , <header-file> ).\n");
        }
      } else if (!strcmp(key,"write_mode")) {
        _c->_config.add(key,get_ident(value,"write mode."));
      } else if (!strcmp(key,"read_hit_latency")) {
        _c->_config.add(key,get_uint(value,"read-hit latancy.",env()));
      } else if (!strcmp(key,"write_hit_latency")) {
        _c->_config.add(key,get_uint(value,"write-hit latency.",env()));
      } else if (!strcmp(key,"linefill_read_latency")) {
        _c->_config.add(key,get_uint(value,"linefill read latency.",env()));
      } else if (!strcmp(key,"linefill_write_latency")) {
        _c->_config.add(key,get_uint(value,"linefill write latency.",env()));
      } else if (!strcmp(key,"linefill_writeback_latency")) {
        _c->_config.add(key,get_uint(value,"linefill writeback latency.",env()));
      } else if (!strcmp(key,"linefill_access_width")) {
        _c->_config.add(key,get_uint(value,"linefill access width.",env()));
      } else if (!strcmp(key, "linefill_current_access_width")) {
        _c->_config.add("dynamic_entry_size",true);
        _c->_linefillWidth._p = get_func(value, 2,"linefill access width hook.");
      } else if (!strcmp(key,"linefill_lock_cycle")) {
        _c->_config.add(key,get_uint(value, "lookup lock cycle of linefill.",env()));
      } else if (!strcmp(key,"linefill_lock_holdoff_cycle")) {
        _c->_config.add(key,get_uint(value, "lookup hold-off lock cycles of linefill.",env()));
      } else if (!strcmp(key, "linefill_request_block_cycle")) {
        _c->_config.add(key,get_uint(value,"linefill request-block cycle.",env()));
      } else if (!strcmp(key,"consecutive_linefills")) {
        _c->_config.add(key,get_bool(value, "consecutive linefill flag.",env()));
      } else if (!strcmp(key,"block_on_critical_word")) {
        _c->_config.add(key,get_ident(value, "block-on-critical-word option."));
      } else if (!strcmp(key,"evict_lock_cycle")) {
        _c->_config.add(key,get_uint(value, "lookup lock cycle of eviction.",env()));
      } else if (!strcmp(key,"taken_to_write_latency")) {
        _c->_config.add(key,get_uint(value, "taken-to-write latency.",env()));
      } else if (!strcmp(key,"write_to_next_level_request_latency")) {
        _c->_config.add(key,get_uint(value,"write to next-level-request latency.",env()));
      } else if (!strcmp(key,"invalidate_latency")) {
        _c->_config.add(key,get_uint(value,"invalidate latency.",env()));
      } else if (!strcmp(key,"touch_latency")) {
        _c->_config.add(key,get_uint(value,"touch latency.",env()));
      } else if (!strcmp(key,"lock_latency")) {
        _c->_config.add(key,get_uint(value,"lock latency.",env()));
      } else if (!strcmp(key,"unlock_latency")) {
        _c->_config.add(key,get_uint(value,"unlock latency.",env()));
      } else if (!strcmp(key,"zero_latency")) {
        _c->_config.add(key,get_uint(value,"zero latency.",env()));
      } else if (!strcmp(key,"critical_word_first")) {
        _c->_config.add(key,get_bool(value,"critical-word-first flag.",env()));
      } else if (!strcmp(key,"streaming_linefill")) {
        _c->_config.add(key,get_bool(value,"streaming-linefill flag.",env()));
      } else if (!strcmp(key,"linefill_lazy_writeback")) {
        _c->_config.add(key,get_bool(value,"linefill lazy writeback flag.",env()));
      } else if (!strcmp(key,"linefill_lock_on_valid")) {
        _c->_config.add(key,get_bool(value,"linefill lock-on-valid flag.",env()));
      } else if (!strcmp(key, "store_buffer_size")) {
        _c->_config.add(key,get_uint(value,"store buffer max-size value.",env()));
      } else if (!strcmp(key, "store_buffer_enable")) {
        _c->_storeBufEnable._p = get_func(value, 0,"store buffer enable predicate.");
      } else if (!MemBaseHandler::handle_key_internal(key,value)) {
        // If we don't recognize the key, then add as a potential extra key for a
        // custom cache/memory-object model.
        unsigned val;
        if (const char *str = get_ident(value,0)) {
          _c->_config.add(key,str,false);
        } else if (get_uint(val,value,env())) {
          _c->_config.add(key,val,false);
        } else {
          PError(value,"Cache parameter " << key << " (not a built-in parameter) has a value that is not an identifier or an integer.");
        }
      }
    }

  };


  class MappingHandler : public IrHandler {
  public:
    StateBindings *_sb;

    MappingHandler(Environment *e,Ptree *p,bool modify,UadlIr *ir) :
            IrHandler(e,p,"",modify,ir),
      _sb(0)
    {
    }

    void handle_key(const char *key,Ptree *value)
    {
      // Each key is taken to be a stage name.  The value is then a state or
      // lists of states.
      gc_string k = key;
      handle_list_or_item(value,bind(&MappingHandler::add_stage,this,_1,ref(k)));
    }

    void add_stage(Opencxx::Ptree *p,gc_string &stage)
    {
      _sb->_tmp.push_back(make_pair(stage,get_ident(p,"state name")));
    }
  };

  class ExceptionMappingHandler : public IrHandler {
  public:
    Machine *_m;

    ExceptionMappingHandler(Environment *e,Ptree *p,bool modify,UadlIr *ir) :
      IrHandler(e,p,"",modify,ir),
      _m(0)
    {
    }

    void handle_key(const char *key,Ptree *value)
    {
      // Each key is taken to be an exception state name.  The value is the name
      // of a non-exception state.
      gc_string k = key;
      handle_list_or_item(value,bind(&ExceptionMappingHandler::add_state,this,_1,ref(k)));
    }

    void add_state(Opencxx::Ptree *p,gc_string &state)
    {
      _m->_emapping.insert(make_pair(get_ident(p,"state name"),state));
    }
  };


  template <class H,class C>
  void process_mappings(MsgLog &log,H &h,C &c)
  {
    ForEach(h._mappings,i) {
      MappingHandler &mh = *(*i);
      try {
        if (mh.remove()) {
          h.setStateBindings(0);
        } else {
          // Remove not set, so add the item if it doesn't already exist.
          if (!h.stateBindings()) {
            h.setStateBindings(new StateBindings(mh.env(),mh.loc()));
          }
          mh._sb = h.stateBindings();
          mh.process_keys(log);
        }
      }
      catch (parse_error &err) {
        log.add(err);
      }
    }      
  }

  class MachineHandler : public IrHandler {
  public:
    friend class CoreHandler;

    MappingHandlers          _mappings;
    ExceptionMappingHandlers _emappings;

    Machine *_m;

    MachineHandler(Environment *e,Ptree *p,const gc_string &n,bool modify,UadlIr *ir) : 
      IrHandler(e,p,n,modify,ir),
      _m(0)
    {
    }

    StateBindings *stateBindings()
    {
      return _m->_stateBindings;
    }

    void setStateBindings(StateBindings *sb)
    {
      _m->_stateBindings = sb;
    }

    Handler *add_define(MsgLog &ml,Environment *e,Ptree *p,const char *type,const char *name,bool modify)
    {
      if (!strcmp(type,"mapping")) {
        if (name) {
          PError(p,"Type 'mapping' does not take a name.");
        }
        auto mh = new MappingHandler(e,p,modify,_ir);
        mh->_ir = _ir;
        _mappings.push_back(mh);
        return mh;      
      }
      else if (!strcmp(type,"exception_mapping")) {
        if (name) {
          PError(p,"Type 'exception_mapping' does not take a name.");
        }
        auto eh = new ExceptionMappingHandler(e,p,modify,_ir);
        eh->_ir = _ir;
        _emappings.push_back(eh);
        return eh;
      }
      else {
        unknown_define(p,type,name);
      }
      assert(0);
    }


    void handle_key(const char *key,Ptree *value)
    {
      if (!strcmp(key,"init_state")) {
        _m->_initState = get_ident(value,"machine's initial state.");
      } else if (!strcmp(key,"states")) {
        handle_list_or_item(value,bind(&MachineHandler::add_state,this,_1));      
      } else 
        unknown_key(value,key); 
    }

    void add_state(Ptree *p)
    {
      _m->_states.insert(get_ident(p,"machine state."));
    }

    // Setup all resources, then finalize them.
    void process_keys(MsgLog &log)
    {
      IrHandler::process_keys(log);

      process_mappings(log,*this,*_m);
      process_unnamed_handler(log,_emappings,*this,&MachineHandler::add_emapping);
    }

    void add_emapping(ExceptionMappingHandler &emh)
    {
      emh._m = _m;
    }
  };

  class OperandHandler : public IrHandler {
  public:
    InstClass *_ic;
    gc_string  _iname;

    OperandHandler(Environment *e,Ptree *p,bool modify,const gc_string &iname,UadlIr *ir) :
            IrHandler(e,p,"",modify,ir),
            _ic(0),
            _iname(iname)
    {
    }

    void handle_key(const char *key,Ptree *value)
    {
      // Each key is taken to be an operand name.  The value is a register,
      // register-file, register-file element indexed by an instruction field,
      // field(instruction-field), or a special keyword "sources" or "targets".
      // The whole thing can be a pair, where the second element is a mask.  The
      // error checking is done in UadlParser.C.
      _ic->_tmp_ops[_iname].push_back(make_pair(gc_string(key),value));
    }

  };

  class FuncHandler : public IrHandler {
  public:
    InstClass *_ic;
    gc_string  _iname;

    FuncHandler(Environment *e,Ptree *p,bool modify,const gc_string &iname,UadlIr *ir) :
      IrHandler(e,p,"",modify,ir),
      _ic(0),
      _iname(iname)
    {
    }

    void handle_key(const char *key,Ptree *value)
    {
      // Each key is a function interface name (what's called in the action
      // code).  The value is the name of a helper to be called.
      _ic->_tmp_funcs[_iname].push_back(make_pair(gc_string(key),value));
    }

  };

  template <class T>
  void process_items(MsgLog &log,InstClass *ic,T &items)
  {
    ForEach(items,i) {
      typename T::value_type &item = *i;
      try {
        item->_ic = ic;
        item->process_keys(log);
      }
      catch (parse_error &err) {
        log.add(err);
      }
    }      
  }    
  
  class InstrOpHandler : public IrHandler {
  public:
    OperandHandlers _operands;
    FuncHandlers    _funcs;
    InstClass      *_ic;
    gc_string       _iname;

    InstrOpHandler(Environment *e,Ptree *p,bool modify,const gc_string &iname,UadlIr *ir) :
      IrHandler(e,p,"",modify,ir),
      _ic(0),
      _iname(iname)
    {
    }

    Handler *add_define(MsgLog &ml,Environment *e,Ptree *p,const char *type,const char *name,bool modify)
    {
      if (!strcmp(type,"operands")) {
        if (name) {
          PError(p,"Type 'operands' does not take a name.");
        }
        auto oh = new OperandHandler(e,p,modify,_iname,_ir);
        oh->_ir = _ir;
        _operands.push_back(oh);
        return oh;      
      }
      else if (!strcmp(type,"functions")) {
        if (name) {
          PError(p,"Type 'functions' does not take a name.");
        }
        auto fh = new FuncHandler(e,p,modify,_iname,_ir);
        fh->_ir = _ir;
        _funcs.push_back(fh);
        return fh;      
      }
      else {
        unknown_define(p,type,name);
      }
      assert(0);
    }

    // Setup all resources, then finalize them.
    void process_keys(MsgLog &log)
    {
      IrHandler::process_keys(log);

      process_items(log,_ic,_operands);
      process_items(log,_ic,_funcs);
    }

  };

  class MemAliasHandler : public IrHandler {
  public:
    friend class CoreHandler;

    MemAliasHandler(Environment *e,Ptree *p,bool modify,UadlIr *ir) : 
      IrHandler(e,p,"",modify,ir)
    {
    }

    void handle_key(const char *key,Ptree *value)
    {
      _ir->add_mem_alias(key,value);
    }

  };

  class InstClassHandler : public IrHandler {
  public:
    friend class CoreHandler;

    MappingHandlers          _mappings;
    OperandHandlers          _operands;
    FuncHandlers             _funcs;
    InstrOpHandlers          _instrs;
    InstClass               *_ic;

    InstClassHandler(Environment *e,Ptree *p,const gc_string &n,bool modify,UadlIr *ir) : 
      IrHandler(e,p,n,modify,ir),
      _ic(0)
    {
    }

    StateBindings *stateBindings()
    {
      return _ic->_stateBindings;
    }

    void setStateBindings(StateBindings *sb)
    {
      _ic->_stateBindings = sb;
    }

    Handler *add_define(MsgLog &ml,Environment *e,Ptree *p,const char *type,const char *name,bool modify)
    {
      if (!strcmp(type,"mapping")) {
        if (name) {
          PError(p,"Type 'mapping' does not take a name.");
        }
        auto mh = new MappingHandler(e,p,modify,_ir);
        mh->_ir = _ir;
        _mappings.push_back(mh);
        return mh;      
      }
      else if (!strcmp(type,"operands")) {
        if (name) {
          PError(p,"Type 'operands' does not take a name.");
        }
        auto oh = new OperandHandler(e,p,modify,gc_string(),_ir);
        oh->_ir = _ir;
        _operands.push_back(oh);
        return oh;      
      }
      else if (!strcmp(type,"functions")) {
        if (name) {
          PError(p,"Type 'functions' does not take a name.");
        }
        auto fh = new FuncHandler(e,p,modify,gc_string(),_ir);
        fh->_ir = _ir;
        _funcs.push_back(fh);
        return fh;
      }
      else if (!strcmp(type,"instr")) {
        check_ident(name,p,"Instruction-operand block",false);
        auto ih = new InstrOpHandler(e,p,modify,name,_ir);
        ih->_ir = _ir;
        _instrs.push_back(ih);
        return ih;
      }
      else {
        unknown_define(p,type,name);
      }
      assert(0);
    }

    void handle_key(const char *key,Ptree *value)
    {
      if (!strcmp(key,"instructions")) {
        handle_list_or_item(value,bind(&InstClassHandler::add_instr,this,_1));
      } else if (!strcmp(key,"instr_attrs")) {
        handle_list_or_item(value,bind(&InstClassHandler::add_instr_attr,this,_1));
      } else if (!strcmp(key,"attrs")) {
        handle_list_or_item(value,bind(&InstClassHandler::add_attr,this,_1));
      } else if (!strcmp(key,"machine")) {
        // We'll do error checking during finalization, when we try and find the
        // machine.
        _ic->_temp_machine = value;
      } else if (!strcmp(key,"action")) {
        _ic->_action_block.push_back(get_func(value,0,"action block."));
      } else if (!strcmp(key,"allow_missing_operands")) {
        _ic->_allow_missing_operands = get_bool(value,"allow-missing-operands flag.",env());
      } else if (!strcmp(key,"allow_extra_operands")) {
        _ic->_allow_extra_operands = get_bool(value,"allow-extra-operands flag.",env());
      } else if (!strcmp(key,"commit_on_exception")) {
        _ic->_commit_on_exception = (get_bool(value,"commit-on-exception flag.",env()) ? tTrue : tFalse);
      } else {
        unknown_key(value,key); 
      }
    }

    void add_instr_attr(Ptree *value)
    {
      _ic->_instr_attrs.insert(get_ident(value,"instruction attribute.")); 
    }

    void add_attr(Ptree *value)
    {
      _ic->add_attr(value,get_ident(value,"instruction class attribute.")); 
    }

    // We support either strings/identifiers (plain instruction names), or else
    // functions, which allow the user to specify operand-value constraints, e.g. mtspr(SPRN(20))
    void add_instr(Ptree *p)
    {
      if (get_func_call(p,0)) {
        InstItem ii(get_str(get_func_call_name(p),"instruction name."));
        unsigned numargs = get_func_call_numargs(p);
        ForRange(numargs,a) {
          auto arg = get_func_call_arg(p,a);
          // We expect this to be of the form <field>(<value>);
          Ptree *f,*v;
          if ( Match(arg,"[ %? ( [ %? ] ) ]",&f,&v)) {
            gc_string field   = get_str(f,"operand-field name.");
            unsigned value = get_uint(v,"operand value constraint.",env());
            ii._opvalues.insert(make_pair(field,value));
          } else {
            PError(p,"Expected <field>(<value>) for argument " << a << ", but found " << arg);
          }
        }
        _ic->_instrs.push_back(ii);
      } else if (const char *n = get_str(p,"instruction name.")) {
        _ic->_instrs.push_back(InstItem(n));
      } else {
        PError(p,"Expected a string, identifier, or <instr>(<field>(<value>)[,...]) for instruction-class item.");
      }
    }

    // Setup all resources, then finalize them.
    void process_keys(MsgLog &log)
    {
      IrHandler::process_keys(log);

      process_mappings(log,*this,*_ic);

      process_instrs(log);

      process_items(log,_ic,_operands);
      process_items(log,_ic,_funcs);
    }

    void process_instrs(MsgLog &log)
    {
      ForEach(_instrs,i) {
        InstrOpHandler &ih = *(*i);
        try {
          ih._ic = _ic;
          ih.process_keys(log);
        }
        catch (parse_error &err) {
          log.add(err);
        }
      }      
    }    

  };


  // Definition of an individual manager block.
  class StageHandler : public IrHandler {
  protected:
    friend class MngrsHandler;

    PipelineStage *_stage;
  public:
    StageHandler(Environment *e,Ptree *p,const gc_string &n,bool modify,UadlIr *ir) : 
      IrHandler(e,p,n,modify,ir),
      _stage(0)
    {
    }

    void handle_key(const char *key,Ptree *value)
    {
      if (!strcmp(key,"size")) {    
        _stage->_size = get_uint(value,"pipeline stage size.",env());
      } else if (!strcmp(key,"bandwidth")) {
        _stage->_bandwidth = get_uint(value,"pipeline stage bandwidth.",env());
      } else if (!strcmp(key,"rigid")) {
        _stage->_rigid = get_bool(value,"rigid-pipeline flag.",env());
      } else if (!strcmp(key,"scheduler")) {
        if (!_stage->set_scheduler(get_ident(value,"scheduler type."))) {
          PError(value,"Unknown scheduling type '" << value << "'.");
        }
      } else if (!strcmp(key,"issue")) {
        _stage->_issue = get_bool(value,"issue flag.",env());
      } else if (!strcmp(key,"shared")) {
        _stage->_shared = get_bool(value,"shared flag.",env());
      } else {
        unknown_key(value,key);
      }
    }

  };

  // Definition of an individual manager block.
  class FetchUnitHandler : public IrHandler {
  protected:
    friend class MngrsHandler;
    FetchUnit *_fetcher;

  public:
    FetchUnitHandler(Environment *e,Ptree *p,const gc_string &n,bool modify,UadlIr *ir) : 
      IrHandler(e,p,n,modify,ir),
      _fetcher(0)
    {
    }


    void handle_key(const char *key,Ptree *value)
    {
      if (!strcmp(key,"fetch_memory")) {    
        Ptree *m, *p;
        if (Match(value,"[ %? . %? ]",&m,&p)) {
          _fetcher->_fetchMemory = get_ident(m,"fetch-memory name.");
          _fetcher->_fetchMemoryPort = get_ident(p,"fetch-memory port name.");
        } else {
          _fetcher->_fetchMemory = get_ident(value,"fetch-memory name.");
        }
      } else if (!strcmp(key,"entries")) {
        _fetcher->_nEntries = get_uint(value,"number of fetch entries.",env());
      } else if (!strcmp(key,"entry_size")) {
        _fetcher->_entrySize = get_uint(value,"fetch entry size.",env());
      } else if (!strcmp(key,"min_fetch_entries")) {
        _fetcher->_minFetchEntries = get_uint(value,"minimal entries for a new fetch.",env());
      } else if (!strcmp(key,"max_fetch_count")) {
        _fetcher->_maxFetchCount = get_uint(value,"maximum fetch count.",env());
      } else if (!strcmp(key,"fetch_size")) {
        handle_list_or_item(value,bind(&FetchUnitHandler::add_fetch_size,this,_1));
      } else if (!strcmp(key,"lazy_fetch")) {
        _fetcher->_lazyFetch = get_bool(value,"make fetch decision before issuing",env());
      } else if (!strcmp(key,"flush_fetch")) {
        _fetcher->_flushFetch = get_bool(value,"flush always causes a new fetch",env());
      } else if (!strcmp(key,"can_cancel")) {
        _fetcher->_canCancel = get_bool(value,"can cancel pending fetch requests",env());
      } else if (!strcmp(key,"reuse_data")) {
        _fetcher->_reuseData = get_bool(value,"can reuse instruction buffer data",env());
      } else if (!strcmp(key,"branch_predictor")) {
        handle_list_or_item(value,bind(&FetchUnitHandler::add_branch_predictor,this,_1));
      } else if (!strcmp(key,"interleaved")) {
        _fetcher->_interleaved = get_bool(value,"interleaved flag",env());
      } else {
        unknown_key(value,key);
      }
    }

    void add_branch_predictor(Ptree *p)
    {
      _fetcher->_branchPredictors.push_back(get_ident(p,"branch predictor name."));
    }

    void add_fetch_size(Ptree *p)
    {
      unsigned fetchSize = get_uint(p,"fetch size",env());
      _fetcher->_fetchSizes.push_back(fetchSize);
    }
  };


 
  // Definition of a manager block.
  class MngrsHandler : public IrHandler {
  protected:
    FetchUnitHandlers   _fetchers;
    StageHandlers       _stages;
    SemaphoreHandlers   _semaphores;
    FlagHandlers        _flags;
    RegFileHandlers     _regfiles;
    FwdPathHandlers     _fwdpaths;
    BranchPredHandlers  _branchpreds;
    MemHandlers         _mems;
    CacheHandlers       _caches;
    CustomHandlers      _customs;

  public:
    Resources          *_res;

    MngrsHandler(Environment *e,Ptree *p,const gc_string &n,bool modify,UadlIr *ir) : 
      IrHandler(e,p,n,modify,ir),
      _res(0)
    {
    }

    Handler *add_define(MsgLog &ml,Environment *e,Ptree *p,const char *type,const char *name,bool modify)
    {
      check_ident(name,p,type);

      if (!strcmp(type,"fetchunit")) {
        auto mh = new FetchUnitHandler(e,p,name,modify,_ir);
        _fetchers.push_back(mh);
        return mh;
      } else if (!strcmp(type,"pipestage")) {
        auto ph = new StageHandler(e,p,name,modify,_ir);
        _stages.push_back(ph);
        return ph;
      } else if (!strcmp(type,"semaphore")) {
        auto fh = new SemaphoreHandler(e,p,name,modify,_ir);
        _semaphores.push_back(fh);
        return fh;
      } else if (!strcmp(type,"flag")) {
        auto fh = new FlagHandler(e,p,name,modify,_ir);
        _flags.push_back(fh);
        return fh;
      } else if (!strcmp(type,"regfile")) {
        auto rh = new RegFileHandler(e,p,name,modify,false,_ir);
        _regfiles.push_back(rh);
        return rh;
      } else if (!strcmp(type,"reg")) {
        auto rh = new RegFileHandler(e,p,name,modify,true,_ir);
        _regfiles.push_back(rh);
        return rh;
      } else if (!strcmp(type,"forwarding_path")) {
        auto fh = new FwdPathHandler(e,p,name,modify,_ir);
        _fwdpaths.push_back(fh);
        return fh;
      } else if (!strcmp(type,"branch_predictor")) {
        auto bh = new BranchPredHandler(e,p,name,modify,_ir);
        _branchpreds.push_back(bh);
        return bh;
      } else if (!strcmp(type,"memory")) {
        auto mh = new MemHandler(e,p,name,modify,_ir);
        _mems.push_back(mh);
        return mh;
      } else if (!strcmp(type,"cache")) {
        auto ch = new CacheHandler(e,p,name,modify,_ir);
        _caches.push_back(ch);
        return ch;
      } else if (!strcmp(type,"custom")) {
        auto ch = new CustomHandler(e,p,name,modify,_ir);
        _customs.push_back(ch);
        return ch;
      } else {
        unknown_define (p,type,name);
      }
      assert(0);
    }

    // Setup all resources, then finalize them.
    void process_keys(MsgLog &log)
    {
      IrHandler::process_keys(log);

      // Process the fetch unit.
      process_fetch_unit(log);

      // Process the pipeline stages.
      process_named_handler(log,_stages,_res->_pipelineStages,*this,&MngrsHandler::setup_stage_handler);    

      // Process the semaphores.
      process_named_handler(log,_semaphores,_res->_semaphores,*this,&MngrsHandler::setup_semaphore_handler);    

      // Process the flags.
      process_named_handler(log,_flags,_res->_flags,*this,&MngrsHandler::setup_flag_handler);    

      // Process the register files.
      process_named_handler(log,_regfiles,_res->_registerFiles,*this,&MngrsHandler::setup_regfile_handler);    

      // Process the forwarding paths.
      process_named_handler(log,_fwdpaths,_res->_forwardingPaths,*this,&MngrsHandler::setup_fwdpath_handler);    

      // Process the branch predictors.
      process_named_handler(log,_branchpreds,_res->_branchPredictors,*this,&MngrsHandler::setup_branchpred_handler);    

      // Process the memories.
      process_named_handler(log,_mems,_res->_memories,*this,&MngrsHandler::setup_mem_handler);    

      // Process the caches.
      process_named_handler(log,_caches,_res->_caches,*this,&MngrsHandler::setup_cache_handler);    

      // Process custom resources.
      process_named_handler(log,_customs,_res->_customResources,*this,&MngrsHandler::setup_custom_handler);    
    }

    void setup_stage_handler(StageHandler &ph,PipelineStage &p)
    {
      ph._stage = &p;
      // In order to attempt to maintain the same ordering set by the user, we
      // make use of an index here.  We only update the index for non-modify
      // operations, in order to keep the original order the same.
      if (!ph.modify()) {
        p._index = _ir->next_ps_index();
      }
    }

    void setup_semaphore_handler(SemaphoreHandler &fh,Semaphore &f)
    {
      fh._semaphore = &f;
    }

    void setup_flag_handler(FlagHandler &fh,Flag &f)
    {
      fh._flag = &f;
    }

    void setup_regfile_handler(RegFileHandler &rh,RegisterFile &r)
    {
      rh._regfile = &r;
      r._is_reg = rh._is_reg;
    }

    void setup_fwdpath_handler(FwdPathHandler &fh,ForwardingPath &f)
    {
      fh._fp = &f;
    }

    void setup_branchpred_handler(BranchPredHandler &bh,BranchPredictor &b)
    {
      bh._bp = &b;
    }


    void setup_mem_handler(MemHandler &mh, Memory &m)
    {
      mh._mb = &m;
      mh._m = &m;
    }


    void setup_cache_handler(CacheHandler &ch, Cache &c)
    {
      ch._mb = &c;
      ch._c = &c;
    }

    void setup_custom_handler(CustomHandler &ch, CustomResource &c)
    {
      ch._c = &c;
    }


    void process_fetch_unit(MsgLog &log)
    {
      ForEach(_fetchers,i) {
        FetchUnitHandler &fh = *(*i);
        try {
          if (fh.remove()) {
            _res->_fetcher = 0;
            if (adl::DataStrictMode()) {
              AddWarning(log,fh.loc(),"Item '" << fh.name() << "' was removed.");
            }
          } else {
            // Remove not set, so add the item if it doesn't already exist.
            if (!_res->_fetcher || !fh.modify()) {
              if (_res->_fetcher) {
                if (DataWarnRedefineMode()) {
                  AddWarning(log,fh.loc(),"Item '" << fh.name() << "' was redefined.");
                  AddWarning(log,_res->_fetcher->loc(),"    Previous definition was here.");
                }
              }
              _res->_fetcher = new FetchUnit(fh.env(),fh.loc(),fh.name());
            } else if (adl::DataStrictMode()) {
              AddWarning(log,fh.loc(), "Item '" << fh.name() << "' was modified"); 
            }
            fh._fetcher = _res->_fetcher;
            fh.process_keys(log);
          }
        }
        catch (parse_error &err) {
          log.add(err);
        }
      }
    }
  };

  // Definition of a thread.
  class ThreadHandler : public IrHandler {
  protected:
    MngrsHandlers      _mngrs;
    bool               _is_group;
    Thread            *_thread;
    ThreadHandlers     _threads;

  public:
    ThreadHandler(Environment *e,Ptree *p,const gc_string &n,bool modify,UadlIr *ir,bool is_group) : 
      IrHandler(e,p,n,modify,ir),
      _is_group(is_group),
      _thread(0)
    {
    }

    Handler *add_define(adl::MsgLog &ml,Environment *e,Ptree *p,const char *type,const char *name,bool modify)
    {
      if (!strcmp(type,"resources")) {
        if (name) {
          PError(p,"Type 'resources' does not take a name.");
        }
        auto mh = new MngrsHandler(e,p,"resources",modify,_ir);
        mh->_ir = _ir;
        _mngrs.push_back(mh);
        return mh;
      } else if (!strcmp(type,"thread")) {
        auto th = new ThreadHandler(e,p,name,modify,_ir,false);
        _threads.push_back(th);
        return th;      
      } else if (!strcmp(type,"thread_group")) {
        auto th = new ThreadHandler(e,p,name,modify,_ir,true);
        _threads.push_back(th);
        return th;      
      } else {
        unknown_define(p,type,name);
      }
      assert(0);
    }
   
    void process_keys(adl::MsgLog &log)
    {
      IrHandler::process_keys(log);

      // Process the threads.
      process_named_handler(log,_threads,_thread->_threads,*this,&ThreadHandler::setup_thread_handler);

      // Process all resources.
      ForEach(_mngrs,i) {
        (*i)->_res = &_thread->_res;
        (*i)->process_keys(log);
      }
    }

    void setup_thread_handler(ThreadHandler &th,Thread &t)
    {
      _ir->declareThread(*_thread,t,th._is_group);
      th._thread = &t;
    }


    friend class CoreHandler;
  };

  // Definition of a core.
  class CoreHandler : public IrHandler {
  protected:
    adl::PMap          _helpers;
    adl::PList         _type_decls;
    MngrsHandlers      _mngrs;
    ThreadHandlers     _threads;
    MachineHandlers    _machines;
    InstClassHandlers  _classes;
    MemAliasHandlers   _memaliases;
    UadlIr            *_core;

  public:
    CoreHandler(Environment *e,Ptree *p,const gc_string &n,bool modify,UadlIr *ir) : 
      IrHandler(e,p,n,modify,ir), _core(ir)
    {
    }

    Handler *add_define(adl::MsgLog &ml,Environment *e,Ptree *p,const char *type,const char *name,bool modify)
    {
      if (!strcmp(type,"resources")) {
        if (name) {
          PError(p,"Type 'resources' does not take a name.");
        }
        auto mh = new MngrsHandler(e,p,"resources",modify,_ir);
        mh->_ir = _ir;
        _mngrs.push_back(mh);
        return mh;
      } else if (!strcmp(type,"thread")) {
        auto th = new ThreadHandler(e,p,name,modify,_ir,false);
        _threads.push_back(th);
        return th;      
      } else if (!strcmp(type,"thread_group")) {
        auto th = new ThreadHandler(e,p,name,modify,_ir,true);
        _threads.push_back(th);
        return th;      
      } else if (!strcmp(type,"machine")) {
        auto mh = new MachineHandler(e,p,name,modify,_ir);
        _machines.push_back(mh);
        return mh;
      } else if (!strcmp(type,"instr_class")) {
        auto ch = new InstClassHandler(e,p,name,modify,_ir);
        _classes.push_back(ch);
        return ch;
      } else if (!strcmp(type,"mem_alias")) {
        if (name) {
          PError(p,"Type 'mem_alias' does not take a name.");
        }
        auto mh = new MemAliasHandler(e,p,modify,_ir);
        _memaliases.push_back(mh);
        return mh;
      } else {
        unknown_define(p,type,name);
      }
      assert(0);
    }

    void add_func(const char *name,Opencxx::Ptree *p)
    {
      // We use the [] operator b/c we want subsequent functions to override
      // prior functions.
      _helpers[name] = p;
    }

    void add_type_decl(Opencxx::Ptree *p)
    {
      // We allow any type here so that it can be used as a helper type.
      _type_decls.push_back(p);
    }
   

    void process_keys(adl::MsgLog &log)
    {
      IrHandler::process_keys(log);

      // Add on any helpers that we get here.
      // We use operator[] because we want later versions of a function
      // to override prior versions.
      ForEach(_helpers,i) {
        _ir->_helpers[i->first] = i->second;
      }

      // Add anny type declarations.
      ForEach(_type_decls,i) {
        _ir->_type_decls.insert(_ir->_type_decls.end(),_type_decls.begin(),_type_decls.end());
      }

      // Process all resources.
      ForEach(_mngrs,i) {
        (*i)->_res = &_ir->_top->_res;
        (*i)->process_keys(log);
      }

      // Process the threads.
      process_named_handler(log,_threads,_ir->_top->_threads,*this,&CoreHandler::setup_thread_handler);

      // Process the machines.
      process_named_handler(log,_machines,_ir->_machines,*this,&CoreHandler::setup_machine_handler);

      // Process instruction classes.
      process_named_handler(log,_classes,_ir->_instClasses,*this,&CoreHandler::setup_ic_handler);

      // Process memory aliases.
      process_unnamed_handler(log,_memaliases,*this,&CoreHandler::setup_memalias_handler);
    }

    void handle_key(const char *key, Ptree *value)
    {
      if (!strcmp(key, "dead_lock_timeout")) {
        _core->setDeadLockTimeOut(get_uint(
                                           value, "dead/live-lock timeout cycle count for safe mode.",env())
                                  );
      }
      else if (!strcmp(key,"commit_on_exception")) {
        _core->_commit_on_exception = get_bool(value,"commit-on-exception flag.",env());
      }
      else if (!strcmp(key,"allow_unimpl_instrs")) {
        _core->_allow_unimpl_instrs = get_bool(value,"allow-unimplemented-instructions flag.",env());
      }
      else if (!strcmp(key,"mem_access")) {
        _core->_mem_access._p = get_func(value,1,"memory-access hook.");
      }
      else if (!strcmp(key,"branch_hook")) {
        _core->_branch_hook = get_func(value,1,"branch-hook function.");
      }
      else {
        unknown_key(value, key);
      }
    }

    friend class OuterHandler;

    void setup_thread_handler(ThreadHandler &th,Thread &t)
    {
      _ir->declareThread(*_ir->_top,t,th._is_group);
      th._thread = &t;
    }

    void setup_machine_handler(MachineHandler &mh,Machine &m)
    {
      mh._m = &m;
    }
    
    void setup_ic_handler(InstClassHandler &ch,InstClass &c)
    {
      ch._ic = &c;
    }

    void setup_memalias_handler(MemAliasHandler &mh)
    {
    }

  };


  IrHandler::IrHandler(Environment *e,Ptree *p,const gc_string &n,bool modify,UadlIr *ir) :
    RemoveHandler(e,p,n,modify),
    _ir(ir)
  {
  }



  OuterHandler::OuterHandler(Environment *e,Ptree *p,const gc_string &n,bool modify,UadlIr *ir) : 
    IrHandler(e,p,n,modify,ir)
  {
  }

  Handler *OuterHandler::add_define(MsgLog &ml,Environment *e,Ptree *p,const char *type,const char *name,bool modify)
  {
    if (!strcmp(type,"core")) {
      check_ident(name,p,"Core");
      auto ch = new CoreHandler(e,p,name,modify,_ir);
      ch->_ir = _ir;
      // We switch to the current environment, but we have to store all
      // environments, since we want to see everything in all of these blocks,
      // e.g. helper functions.
      _ir->_env = e;      
      _ir->_all_envs.push_back(e);
      _cores.push_back(ch);
      return ch;      
    }
    else {
      unknown_define(p,type,name);
    }
    assert(0);
  }  

  // In the second pass, we iterate over each constituent list.
  void OuterHandler::process_keys(MsgLog &ml)
  {
    IrHandler::process_keys(ml);

    // Handle cores.
    ForEach(_cores,i) {
      auto ch = *i;
      _ir->declareTop(ml,env(),ch->loc(),ch->name());
      ch->process_keys(ml);
    }
  }





}




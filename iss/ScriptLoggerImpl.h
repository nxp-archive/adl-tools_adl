//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This is the implementation class for the ADL script logger.  It can be used
// as a base class by, for example, uADL, which can add on more events for
// logging.
//

#ifndef _SCRIPT_LOGGER_IMPL_H_
#define _SCRIPT_LOGGER_IMPL_H_

#include <vector>
#include <stdexcept>

#include "helpers/Macros.h"
#include "helpers/stringhash.h"

#include "NodeHelpers.h"
#include "ScriptLogger.h"

namespace adl {

  // A single function invocation item: A function name, plus optional filtering
  // information (two numerical elements, plus a count of how many are relevant
  // (0, 1, or 2).
  struct FuncItem {
    std::string  _func;
    unsigned     _filtercount;
    addr_t       _filter1;
    addr_t       _filter2;

    FuncItem() : _filtercount(0), _filter1(0), _filter2(0) {};
    FuncItem(const std::string &f) : _func(f), _filtercount(0), _filter1(0), _filter2(0) {};

    bool operator==(const std::string &f) const { return _func == f; };

    void set_filter_data(unsigned count,addr_t f1,addr_t f2) {
      _filtercount = count;
      _filter1 = f1;
      _filter2 = f2;
    }
  };

  struct MatchFuncByName {
    bool operator()(const FuncItem &x,const std::string &y) {
      return x._func == y;
    }
  };

  // Intermediate data structure- returns a list of functions to be invoked.
  typedef std::vector<const FuncItem *> TFuncs;

  // This stores a list of functions to be invoked.  Note: Deletion is done only
  // on name.
  struct Funcs : public std::vector<FuncItem> 
  {

    FuncItem &add(const std::string &func) 
    {
      auto iter = find(begin(),end(),func);
      if (iter == end()) {
        push_back(func);
        return back();
      }
      return *iter;
    }

    bool del(const std::string &func)
    {
      auto iter = find(begin(),end(),func);
      if (iter != end()) {
        erase(iter);
        return true;
      }
      return false;
    }

  };

  // For when functions can be filtered by an ID, this stores functions indexed by
  // id.
  struct FuncsById : public std::vector<Funcs> 
  {

    FuncItem &add(unsigned id,const std::string &func)
    {
      if (id >= size()) {
        resize(id+1);
      }
      return at(id).add(func);
    }

    bool del(int &id,const std::string &func)
    {
      bool found = false;
      int count = 0;
      ForEach((*this),iter) {
        if (iter->del(func)) {
          found = true;
          id = count;
        }
        ++count;
      }
      return found;
    }

    bool exists(unsigned id) const
    {
      return (id < size()) && !at(id).empty();
    }

  
  };

  // A collection of functions.  Functions may be either unfiltered or filtered by
  // an integer ID.  For some events, no filtering is supported, so that data
  // structure will remain empty.
  struct EventFuncs {
    Funcs     _all;   // Events to always be invoked.
    FuncsById _filt;  // Only invoked if an id matches.

    FuncItem &add(int id,const std::string &func)
    {
      if (id < 0) {
        return _all.add(func);
      } else {
        return _filt.add(id,func);
      }
    }

    int del(const std::string &func)
    {
      int id = -1;
      bool found = false;

      if (_all.del(func)) { found = true; };
      if (_filt.del(id,func)) { found = true; };

      if (!found) {
        RError("Attempt to remove logging function " << func << " which does not exist."); 
      }

      return id;
    }

  };

  // Stores a collection of EventFuncs.  The outer EventFuncs specifies
  // functions to be invoked regardless of context.  The two dimensional array
  // is used to specify events triggered to specific contexts.  The first
  // dimension specifies a context ID and the second dimension specifies context
  // instances.
  struct EventsByCtx {
    EventFuncs _all;                                // Events to be invoked regardless of context.
    std::vector<std::vector<EventFuncs > > _by_ctx; // Context-specific events.

    FuncItem &add(const Target &target,int id,const std::string &func) 
    {
      if (target.has_ctx()) {
        // If we have context information, then add to the context-specific 2d array.
        unsigned cid  = target.ctx_id();
        unsigned cnum = target.ctx_num();
        if (_by_ctx.size() <= cid) {
          _by_ctx.resize(cid+1);
        }
        if (_by_ctx[cid].size() <= cnum) {
          _by_ctx[cid].resize(cnum+1);
        }
        return _by_ctx[cid][cnum].add(id,func);
      } else {
        // Otherwise, just add to the 'all' map.
        return _all.add(id,func);
      }
    }

    int del(const Target &target,const std::string &func)
    {
      if (target.has_ctx()) {
        unsigned cid  = target.ctx_id();
        unsigned cnum = target.ctx_num();
        if (cid < _by_ctx.size()) {
          if (cnum < _by_ctx[cid].size()) {
            return _by_ctx[cid][cnum].del(func);
          }
        }
        return 0;
      } else {
        // No ctx info- just remove from the all map.
        return _all.del(func);
      }
    }
  };


  // This indexes functions with a specific target node.
  struct FuncMap : public std::tr1::unordered_map<IssNode *,EventsByCtx>
  {
    FuncItem &add(const Target &target,int id,const std::string &func) 
    {
      return (*this)[target.node()].add(target,id,func);
    }

    int del(const Target &target,const std::string &func)
    {
      return (*this)[target.node()].del(target,func);
    }
  };

  // A null filter object (the default).  Other filter objects can be used to
  // filter upon the filter values that can exist for a function item.
  struct NullFilter {
    bool operator()(const FuncItem &f) const { return true; };
  };

  template <class T>
  void get_matching_events(TFuncs &funcs,const EventFuncs &ef,int id,const T &filter)
  {
    ForEach(ef._all,i) {
      funcs.push_back(&(*i));
    }
    if (id >= 0 && ef._filt.exists(id)) {
      ForEach(ef._filt[id],i) {
        funcs.push_back(&(*i));
      } 
    }
    ForEach(funcs,i) {
      if ( !filter(**i) ) {
        // If it's to be filtered out, then simply assign a null to the
        // pointer value.  Invoke will skip null values.
        *i = 0;
      }
    }
  }

  template <class T>
  void get_matching_events(TFuncs &funcs,Target &core,const EventsByCtx &ebc,int id,const T &filter)
  {
    // This matches non-context specific stuff.
    get_matching_events(funcs,ebc._all,id,filter);

    // This matches the context-specific stuff.
    ForRange(ebc._by_ctx.size(),cid) {
      if (!ebc._by_ctx[cid].empty()) {
        unsigned cnum = core.node()->getActiveCtx(cid);
        if (cnum < ebc._by_ctx[cid].size()) {
          get_matching_events(funcs,ebc._by_ctx[cid][cnum],id,filter);
        }            
      }
    }
  }

  // Update funcs with a list of functions to be invoked.  If id >= 0, then we
  // use it as a matching criteria.  We always invoke the always-invoke functions, then anything
  // that matches the filtering value.
  template <class T>
  void get_matching_events(TFuncs &funcs,Target &core,const FuncMap &fmap,int id,const T &filter)
  {
    funcs.clear();
    auto iter = fmap.find(core.node());
    if (iter != fmap.end()) {
      get_matching_events(funcs,core,iter->second,id,filter);
    }
  }

  struct ScriptLoggerImpl : public adl::ScriptLogger 
  {

    FuncMap         _log_instr_map;
    FuncMap         _log_instr_args_map;
    FuncMap         _log_basic_block_end_map;
    FuncMap         _log_reg_write_map;
    FuncMap         _log_reg_read_map;
    FuncMap         _log_regfile_write_map;
    FuncMap         _log_regfile_read_map;
    FuncMap         _log_mmu_map;
    FuncMap         _log_cache_map;
    FuncMap         _log_memread_map;
    FuncMap         _log_memwrite_map;
    FuncMap         _log_exception_map;
    FuncMap         _log_breakpoint_map;
    FuncMap         _log_watchpoint_map;

    ScriptingIface &_scripter;

    ScriptLoggerImpl();

    virtual ~ScriptLoggerImpl();

    virtual LoggingIface *register_core(IssCore &core);  

    virtual void reset();

    // These are called whenever events are added or if all events are removed.
    // It should do whatever is necessary to register the object with the
    // simulated cores, in order to receive appropriate logging events.
    virtual void registerOurself();
    virtual void unregisterOurself();

    // This looks up an id of a named item by querying the supplied IssNode
    // using the supplied method pointer.  If found, it returns the id.  If not
    // found, it throws a runtime error using msg.
    template <class T>
    int get_filter_id(const Target &target,const std::string &filter,const T& (IssNode::*info)() const,const char *msg)
    {
      if (filter.empty()) {
        return -1;
      }

      int rc = -1;
      const T &data = (target.node()->*info)();
      ForEach(data,iter) {
        if (filter == iter->_name) {
          return iter->_id;
        }
      }
      if (rc < 0) {
        RError("No filter item '" << filter << "' found for " << msg << ".");
      }
      return rc;
    }

    // Given an event name and an optional filter value (empty indicates
    // nonexistant), returns a map pointer/id pair.
    virtual std::pair<FuncMap *,int> get_event_map(const Target &target,const std::string &event,const std::string &filter);

    // Add a function to the specified event, optionally filtering by the value
    // specified by filter.  Additional filter data is added, if supplied.
    virtual void add_event(const Target &target,const std::string &event,const std::string &func,const std::string &filter,
                           unsigned filtercount,addr_t filter1,addr_t filter2);

    // Add a function to the specified event, optionally filtering using an index.
    virtual void add_event(const Target &target,const std::string &event,const std::string &func,int id);

    // Removes a function of the specified name from the specified event.
    // Returns the total number of functions removed.
    virtual int del_event(const Target &target,const std::string &event,const std::string &func);

    virtual bool empty() const;

  };


}

#endif


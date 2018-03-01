//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This file is generated automatically by the script iss/trans_logging_events.
// DO NOT EDIT MANUALLY.
//


#include <assert.h>
#include <vector>

#include "rnumber/RNumber.h"

#include "helpers/Macros.h"

#include "iss/LoggingMgr.h"
#include "iss/ModelInterface.h"

#ifndef _MSC_VER
# include "iss/ThreadedLogger.h"
#endif

using namespace std;
using namespace rnumber;

namespace adl {


  struct LoggingObj : public LoggingObjBase {

  LoggerClients _clients;

  void registerLogger(LogParentIface *parent,LoggingIface* iface)
  {
    _clients.push_back(make_pair(parent,iface));
  }
 
  void unregisterLogger(LogParentIface *parent)
  {
    auto iter = _clients.begin();
    while (iter != _clients.end()) {
      if ((*iter).first == parent) {
        _clients.erase(iter);
        break;
      }
      iter++;
    }
  }


  // Base class for all logging.  Note:  W do *not* use the GC because it
  // can get confused with our use of coroutines.  Since these objects may
  // be allocated while we're in a coroutine, we just use the standard allocator.
  struct TransLogBase {

    TransLogBase() : _next(0), _valid(true) {};
    virtual ~TransLogBase() { assert(_valid); _valid = false; };

    // This should perform the actual logging operation.
    virtual void commit() = 0;

    TransLogBase *_next;  // Next pointer in chain.
    bool          _valid; // Double-deletion checker.
  };

  // The core associated with this object.
  IssCore              &_core;

  // If true, directly broadcast logging events.
  bool                  _direct_logging;

  // Stores the current instruction packet.
  PacketLogBase      *_current_instr;

  // Stores pending events which might occur before an instruction
  // has been allocated.
  TransLogBase         *_pending_log_head, *_pending_log_tail;

  // Stores instruction fetch information.  Each decode will then remove
  // as many items as necessary for the relevant instruction.  This operates as a queue,
  // where we add to the back and take from the front.
  struct logInstrReadObj;
  logInstrReadObj      *_instr_fetch_head, *_instr_fetch_tail;
  addr_t                _last_instr_fetch;

  // When logging an instruction, we store the instruction translation here.
  struct logMmuTranslationObj;
  logMmuTranslationObj *_instr_trans;
  bool                  _add_to_trans_list;

  LoggingObj(IssCore &core) :
    _core(core),
    _direct_logging(false),
    _current_instr(0),
    _pending_log_head(0),
    _pending_log_tail(0),
    _instr_fetch_head(0),
    _instr_fetch_tail(0),
    _last_instr_fetch(0),
    _instr_trans(0),
    _add_to_trans_list(false)
  {
  }

  PacketLogBase *log_current_instr (PacketLogBase *instr)
  {
    PacketLogBase *o = _current_instr;
    _current_instr = instr;
    return o;
  }

  // Internally used helper function for cdr'ing down the log list so that
  // we can print the elements in the correct order.
  void issue_log_event (TransLogBase *x)
  {
    if (x) {
      // We recurse to the end, then issue the events.
      issue_log_event(x->_next);
      x->commit();
      // Note:  Explicit memory management here!  See comment in TransLogBase about
      // why we're not using the GC.
      delete x;
    }
  }

  // This commits all pending log information to the registered clients.
  // If no instruction packet is supplied, then it commits the pending list.
  void log_commit (PacketLogBase *instr)
  {
    if (instr) {
      // Iterate over any objects in the list, calling their commit functions.    
      TransLogBase *x = (TransLogBase *)instr->_log;
      issue_log_event(x);
      instr->_log = 0;
      // If this is the current instruction, then clear the current_instr pointer- we assume
      // that the act of committing is done when the instruction is finished.
      if (instr == _current_instr) {
        _current_instr = 0;
      }
    } else if (_pending_log_head) {
      issue_log_event(_pending_log_head);
      _pending_log_head = _pending_log_tail = 0;
    }
  }

  // Discards all log data.
  void log_discard (PacketLogBase *instr)
  {
    TransLogBase *head = (TransLogBase *)instr->_log;
    while (head) {
      TransLogBase *tmp = head;
      head = head->_next;
      delete tmp;
    }    
    instr->_log = 0;
  }

  // Switches between direct and queued logging.
  bool log_direct(bool d)
  {
    bool tmp = _direct_logging;
    _direct_logging = d;
    return tmp;
  }

  // Fetches and clears the pending queue.
  LogType log_get_pending_queue()
  {
    LogType p = _pending_log_head;
    _pending_log_head = _pending_log_tail = 0;
    return p;
  }

  // Adds an item to the pending queue.
  void add_to_pending_queue(TransLogBase *obj)
  {
    obj->_next = _pending_log_head;
    _pending_log_head = obj;
    if (!_pending_log_tail) {
      _pending_log_tail = obj;
    }
  }

  // Appends to the pending queue.
  void log_append_pending_queue(LogType p)
  {
    TransLogBase *obj = (TransLogBase*)p;
    add_to_pending_queue_tail(obj);
    // Chase the tail until we get to the end.
    if (_pending_log_tail) {
      while (_pending_log_tail->_next) {
        _pending_log_tail = _pending_log_tail->_next;
      }
    }
  }

  void add_event (TransLogBase *obj)
  {
    if (_direct_logging) {
      issue_log_event(obj);
    } else if (_current_instr) {
      obj->_next = (TransLogBase*)(_current_instr->_log);
      _current_instr->_log = obj;
    } else {
      add_to_pending_queue(obj);
    }
  }

  // Adds an item to the back of the pending queue.
  void add_to_pending_queue_tail(TransLogBase *obj)
  {
    if (!_pending_log_tail) {
      _pending_log_tail = obj;
    } else {
      _pending_log_tail->_next = obj;
    }
    if (!_pending_log_head) {
       _pending_log_head = _pending_log_tail;
    }
  }

  struct logCtxChangeObj : public TransLogBase {
    logCtxChangeObj(LoggerClients &clients, unsigned id, const char * name, unsigned context_num, CtxType update) :
       _clients(clients), _id(id), _name(name), _context_num(context_num), _update(update) {};

    logCtxChangeObj *next() { return static_cast<logCtxChangeObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logCtxChange(_id, _name, _context_num, _update);
      }
    };

    LoggerClients &_clients;
    unsigned _id;
    const char * _name;
    unsigned _context_num;
    CtxType _update;

  };


  void log_ctx_change(unsigned id, const char * name, unsigned context_num, CtxType update)
  {
    TransLogBase *obj = new logCtxChangeObj(_clients, id, name, context_num, update);
    add_event(obj);
  }


  struct logInstrReadObj : public TransLogBase {
    logInstrReadObj(LoggerClients &clients, unsigned id, const char * name, addr_t ea, addr_t ra, uint32_t value) :
       _clients(clients), _id(id), _name(name), _ea(ea), _ra(ra), _value(value) {};

    logInstrReadObj *next() { return static_cast<logInstrReadObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logInstrRead(_id, _name, _ea, _ra, _value);
      }
    };

    LoggerClients &_clients;
    unsigned _id;
    const char * _name;
    addr_t _ea;
    addr_t _ra;
    uint32_t _value;

  };


  void log_instr_read(unsigned id, const char * name, addr_t ea, addr_t ra, uint32_t value)
  {
    TransLogBase *obj = new logInstrReadObj(_clients, id, name, ea, ra, value);
    add_event(obj);
  }


  struct logBranchEndObj : public TransLogBase {
    logBranchEndObj(LoggerClients &clients, addr_t ea) :
       _clients(clients), _ea(ea) {};

    logBranchEndObj *next() { return static_cast<logBranchEndObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logBranchEnd(_ea);
      }
    };

    LoggerClients &_clients;
    addr_t _ea;

  };


  void log_branch_end(addr_t ea)
  {
    TransLogBase *obj = new logBranchEndObj(_clients, ea);
    add_event(obj);
  }


  struct logRegWriteObj : public TransLogBase {
    logRegWriteObj(LoggerClients &clients, unsigned id, const char * name, const rnumber::RNumber & value) :
       _clients(clients), _id(id), _name(name), _value(value) {};

    logRegWriteObj *next() { return static_cast<logRegWriteObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logRegWrite(_id, _name.c_str(), _value);
      }
    };

    LoggerClients &_clients;
    unsigned _id;
    std::string _name;
    RNumber _value;

  };


  void log_reg_write(unsigned id, const char * name, const rnumber::RNumber & value)
  {
    TransLogBase *obj = new logRegWriteObj(_clients, id, name, value);
    add_event(obj);
  }


  struct logWatchpointObj : public TransLogBase {
    logWatchpointObj(LoggerClients &clients, addr_t ea, bool pre, WatchType type, int num_bytes, uint64_t value, unsigned h, PtBase * payload) :
       _clients(clients), _ea(ea), _pre(pre), _type(type), _num_bytes(num_bytes), _value(value), _h(h), _payload(payload) {};

    logWatchpointObj *next() { return static_cast<logWatchpointObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logWatchpoint(_ea, _pre, _type, _num_bytes, _value, _h, _payload);
      }
    };

    LoggerClients &_clients;
    addr_t _ea;
    bool _pre;
    WatchType _type;
    int _num_bytes;
    uint64_t _value;
    unsigned _h;
    PtBase * _payload;

  };


  void log_watchpoint(addr_t ea, bool pre, WatchType type, int num_bytes, uint64_t value, unsigned h, PtBase * payload)
  {
    TransLogBase *obj = new logWatchpointObj(_clients, ea, pre, type, num_bytes, value, h, payload);
    add_event(obj);
  }


  struct logAnnotationObj : public TransLogBase {
    logAnnotationObj(LoggerClients &clients, MsgType type, unsigned level, const string & msg, unsigned ndata, LogBase::Data * data) :
       _clients(clients), _type(type), _level(level), _msg(msg), _ndata(ndata) {_data.insert(_data.begin(),&data[0],&data[ndata]);};

    logAnnotationObj *next() { return static_cast<logAnnotationObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logAnnotation(_type, _level, _msg, _ndata, ((_data.empty()) ? 0 : &_data[0]));
      }
    };

    LoggerClients &_clients;
    MsgType _type;
    unsigned _level;
    string _msg;
    unsigned _ndata;
    std::vector<LogBase::Data> _data;

  };


  void log_annotation(MsgType type, unsigned level, const string & msg, unsigned ndata, LogBase::Data * data)
  {
    TransLogBase *obj = new logAnnotationObj(_clients, type, level, msg, ndata, data);
    add_event(obj);
  }


  struct logRegfileReadMaskObj : public TransLogBase {
    logRegfileReadMaskObj(LoggerClients &clients, unsigned id, const char * name, uint32_t index, const rnumber::RNumber & value, const rnumber::RNumber & mask) :
       _clients(clients), _id(id), _name(name), _index(index), _value(value), _mask(mask) {};

    logRegfileReadMaskObj *next() { return static_cast<logRegfileReadMaskObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logRegfileReadMask(_id, _name, _index, _value, _mask);
      }
    };

    LoggerClients &_clients;
    unsigned _id;
    const char * _name;
    uint32_t _index;
    RNumber _value;
    RNumber _mask;

  };


  void log_regfile_read_mask(unsigned id, const char * name, uint32_t index, const rnumber::RNumber & value, const rnumber::RNumber & mask)
  {
    TransLogBase *obj = new logRegfileReadMaskObj(_clients, id, name, index, value, mask);
    add_event(obj);
  }


  struct logRegfileReadObj : public TransLogBase {
    logRegfileReadObj(LoggerClients &clients, unsigned id, const char * name, uint32_t index, const rnumber::RNumber & value) :
       _clients(clients), _id(id), _name(name), _index(index), _value(value) {};

    logRegfileReadObj *next() { return static_cast<logRegfileReadObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logRegfileRead(_id, _name, _index, _value);
      }
    };

    LoggerClients &_clients;
    unsigned _id;
    const char * _name;
    uint32_t _index;
    RNumber _value;

  };


  void log_regfile_read(unsigned id, const char * name, uint32_t index, const rnumber::RNumber & value)
  {
    TransLogBase *obj = new logRegfileReadObj(_clients, id, name, index, value);
    add_event(obj);
  }


  struct logRegfileExtWriteObj : public TransLogBase {
    logRegfileExtWriteObj(LoggerClients &clients, unsigned id, const char * name, uint32_t index, const rnumber::RNumber & value) :
       _clients(clients), _id(id), _name(name), _index(index), _value(value) {};

    logRegfileExtWriteObj *next() { return static_cast<logRegfileExtWriteObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logRegfileExtWrite(_id, _name, _index, _value);
      }
    };

    LoggerClients &_clients;
    unsigned _id;
    const char * _name;
    uint32_t _index;
    RNumber _value;

  };


  void log_regfile_ext_write(unsigned id, const char * name, uint32_t index, const rnumber::RNumber & value)
  {
    TransLogBase *obj = new logRegfileExtWriteObj(_clients, id, name, index, value);
    add_event(obj);
  }


  struct logInstrObj : public TransLogBase {
    logInstrObj(LoggerClients &clients, const uint32_t* opc, int num_half_bytes, const char * name, Disassembler dis, uint32_t flags) :
       _clients(clients), _num_half_bytes(num_half_bytes), _name(name), _dis(dis), _flags(flags) { _opc = new uint32_t[2]; _opc[0] = opc[0]; if ((num_half_bytes/8) > 1) { _opc[1] = opc[1];} };
    ~logInstrObj() {  delete [] _opc; ;};


    logInstrObj *next() { return static_cast<logInstrObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logInstr(_opc, _num_half_bytes, _name, _dis, _flags);
      }
    };

    LoggerClients &_clients;
    uint32_t* _opc;
    int _num_half_bytes;
    const char * _name;
    Disassembler _dis;
    uint32_t _flags;

  };

void log_instr(const uint32_t* opc, int num_half_bytes, const char * name, Disassembler dis, uint32_t flags)  {
    TransLogBase *obj = new logInstrObj(_clients, opc, num_half_bytes, name, dis, flags);
    obj->_next = _pending_log_head;
    _current_instr->_log = obj;
    _pending_log_head = _pending_log_tail = 0;
  }

  struct logMemReadObj : public TransLogBase {
    logMemReadObj(LoggerClients &clients, unsigned id, const char * name, bool pre, int seq, addr_t ea, addr_t ra, uint32_t value) :
       _clients(clients), _id(id), _name(name), _pre(pre), _seq(seq), _ea(ea), _ra(ra), _value(value) {};

    logMemReadObj *next() { return static_cast<logMemReadObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logMemRead(_id, _name, _pre, _seq, _ea, _ra, _value);
      }
    };

    LoggerClients &_clients;
    unsigned _id;
    const char * _name;
    bool _pre;
    int _seq;
    addr_t _ea;
    addr_t _ra;
    uint32_t _value;

  };


  void log_mem_read(unsigned id, const char * name, bool pre, int seq, addr_t ea, addr_t ra, uint32_t value)
  {
    TransLogBase *obj = new logMemReadObj(_clients, id, name, pre, seq, ea, ra, value);
    add_event(obj);
  }


  struct logRegExtWriteObj : public TransLogBase {
    logRegExtWriteObj(LoggerClients &clients, unsigned id, const char * name, const rnumber::RNumber & value) :
       _clients(clients), _id(id), _name(name), _value(value) {};

    logRegExtWriteObj *next() { return static_cast<logRegExtWriteObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logRegExtWrite(_id, _name, _value);
      }
    };

    LoggerClients &_clients;
    unsigned _id;
    const char * _name;
    RNumber _value;

  };


  void log_reg_ext_write(unsigned id, const char * name, const rnumber::RNumber & value)
  {
    TransLogBase *obj = new logRegExtWriteObj(_clients, id, name, value);
    add_event(obj);
  }


  struct logInstrCompletionTimeObj : public TransLogBase {
    logInstrCompletionTimeObj(LoggerClients &clients, ttime_t completion_time) :
       _clients(clients), _completion_time(completion_time) {};

    logInstrCompletionTimeObj *next() { return static_cast<logInstrCompletionTimeObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logInstrCompletionTime(_completion_time);
      }
    };

    LoggerClients &_clients;
    ttime_t _completion_time;

  };


  void log_instr_completion_time(ttime_t completion_time)
  {
    TransLogBase *obj = new logInstrCompletionTimeObj(_clients, completion_time);
    add_event(obj);
  }


  struct logMmuTranslationObj : public TransLogBase {
    logMmuTranslationObj(LoggerClients &clients, TransType tt, int seq, int set, int way, addr_t ea, addr_t ra, const MmuBase * trans) :
       _clients(clients), _tt(tt), _seq(seq), _set(set), _way(way), _ea(ea), _ra(ra) {_trans = (trans->user_entry()) ? trans->clone() : trans;};
    ~logMmuTranslationObj() { if (_trans->user_entry()) delete _trans; ;};


    logMmuTranslationObj *next() { return static_cast<logMmuTranslationObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logMmuTranslation(_tt, _seq, _set, _way, _ea, _ra, _trans);
      }
    };

    LoggerClients &_clients;
    TransType _tt;
    int _seq;
    int _set;
    int _way;
    addr_t _ea;
    addr_t _ra;
    const MmuBase* _trans;

  };

void log_mmu_translation(TransType tt, int seq, int set, int way, addr_t ea, addr_t ra, const MmuBase * trans)  {
    if (_direct_logging) {
      logMmuTranslationObj *obj = new logMmuTranslationObj(_clients, tt, seq, set, way, ea, ra, trans);     
      issue_log_event(obj);
    } else {
      logMmuTranslationObj *obj = new logMmuTranslationObj(_clients, tt, seq, set, way, ea, ra, trans);     
      add_event(obj);
    }
  }

  struct logInstrIssueTimeObj : public TransLogBase {
    logInstrIssueTimeObj(LoggerClients &clients, ttime_t issue_time) :
       _clients(clients), _issue_time(issue_time) {};

    logInstrIssueTimeObj *next() { return static_cast<logInstrIssueTimeObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logInstrIssueTime(_issue_time);
      }
    };

    LoggerClients &_clients;
    ttime_t _issue_time;

  };


  void log_instr_issue_time(ttime_t issue_time)
  {
    TransLogBase *obj = new logInstrIssueTimeObj(_clients, issue_time);
    add_event(obj);
  }


  struct logRegfileWriteObj : public TransLogBase {
    logRegfileWriteObj(LoggerClients &clients, unsigned id, const char * name, uint32_t index, const rnumber::RNumber & value) :
       _clients(clients), _id(id), _name(name), _index(index), _value(value) {};

    logRegfileWriteObj *next() { return static_cast<logRegfileWriteObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logRegfileWrite(_id, _name, _index, _value);
      }
    };

    LoggerClients &_clients;
    unsigned _id;
    const char * _name;
    uint32_t _index;
    RNumber _value;

  };


  void log_regfile_write(unsigned id, const char * name, uint32_t index, const rnumber::RNumber & value)
  {
    TransLogBase *obj = new logRegfileWriteObj(_clients, id, name, index, value);
    add_event(obj);
  }


  struct logMemWriteObj : public TransLogBase {
    logMemWriteObj(LoggerClients &clients, unsigned id, const char * name, bool pre, int seq, addr_t ea, addr_t ra, uint32_t value) :
       _clients(clients), _id(id), _name(name), _pre(pre), _seq(seq), _ea(ea), _ra(ra), _value(value) {};

    logMemWriteObj *next() { return static_cast<logMemWriteObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logMemWrite(_id, _name, _pre, _seq, _ea, _ra, _value);
      }
    };

    LoggerClients &_clients;
    unsigned _id;
    const char * _name;
    bool _pre;
    int _seq;
    addr_t _ea;
    addr_t _ra;
    uint32_t _value;

  };


  void log_mem_write(unsigned id, const char * name, bool pre, int seq, addr_t ea, addr_t ra, uint32_t value)
  {
    TransLogBase *obj = new logMemWriteObj(_clients, id, name, pre, seq, ea, ra, value);
    add_event(obj);
  }


  struct logCoreChangeObj : public TransLogBase {
    logCoreChangeObj(LoggerClients &clients) :
       _clients(clients) {};

    logCoreChangeObj *next() { return static_cast<logCoreChangeObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logCoreChange();
      }
    };

    LoggerClients &_clients;

  };


  void log_core_change()
  {
    TransLogBase *obj = new logCoreChangeObj(_clients);
    add_event(obj);
  }


  struct logInstrFromBufferObj : public TransLogBase {
    logInstrFromBufferObj(LoggerClients &clients) :
       _clients(clients) {};

    logInstrFromBufferObj *next() { return static_cast<logInstrFromBufferObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logInstrFromBuffer();
      }
    };

    LoggerClients &_clients;

  };


  void log_instr_from_buffer()
  {
    TransLogBase *obj = new logInstrFromBufferObj(_clients);
    add_event(obj);
  }


  struct logRegWriteMaskObj : public TransLogBase {
    logRegWriteMaskObj(LoggerClients &clients, unsigned id, const char * name, const rnumber::RNumber & value, const rnumber::RNumber & mask) :
       _clients(clients), _id(id), _name(name), _value(value), _mask(mask) {};

    logRegWriteMaskObj *next() { return static_cast<logRegWriteMaskObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logRegWriteMask(_id, _name.c_str(), _value, _mask);
      }
    };

    LoggerClients &_clients;
    unsigned _id;
    std::string _name;
    RNumber _value;
    RNumber _mask;

  };


  void log_reg_write_mask(unsigned id, const char * name, const rnumber::RNumber & value, const rnumber::RNumber & mask)
  {
    TransLogBase *obj = new logRegWriteMaskObj(_clients, id, name, value, mask);
    add_event(obj);
  }


  struct logInstrArgsObj : public TransLogBase {
    logInstrArgsObj(LoggerClients &clients, const uint32_t* opc, int num_half_bytes, const char * name, Disassembler dis) :
       _clients(clients), _num_half_bytes(num_half_bytes), _name(name), _dis(dis) { _opc = new uint32_t[2]; _opc[0] = opc[0]; if ((num_half_bytes/8) > 1) { _opc[1] = opc[1];} };
    ~logInstrArgsObj() {  delete [] _opc; ;};


    logInstrArgsObj *next() { return static_cast<logInstrArgsObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logInstrArgs(_opc, _num_half_bytes, _name, _dis);
      }
    };

    LoggerClients &_clients;
    uint32_t* _opc;
    int _num_half_bytes;
    const char * _name;
    Disassembler _dis;

  };

void log_instr_args(const uint32_t* opc, int num_half_bytes, const char * name, Disassembler dis)  {
    TransLogBase *obj = new logInstrArgsObj(_clients, opc, num_half_bytes, name, dis);
		obj->_next = _pending_log_head;
    _current_instr->_log = obj;
    _pending_log_head = _pending_log_tail = 0;
  }

  struct logRegReadMaskObj : public TransLogBase {
    logRegReadMaskObj(LoggerClients &clients, unsigned id, const char * name, const rnumber::RNumber & value, const rnumber::RNumber & mask) :
       _clients(clients), _id(id), _name(name), _value(value), _mask(mask) {};

    logRegReadMaskObj *next() { return static_cast<logRegReadMaskObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logRegReadMask(_id, _name.c_str(), _value, _mask);
      }
    };

    LoggerClients &_clients;
    unsigned _id;
    std::string _name;
    RNumber _value;
    RNumber _mask;

  };


  void log_reg_read_mask(unsigned id, const char * name, const rnumber::RNumber & value, const rnumber::RNumber & mask)
  {
    TransLogBase *obj = new logRegReadMaskObj(_clients, id, name, value, mask);
    add_event(obj);
  }


  struct logRegReadObj : public TransLogBase {
    logRegReadObj(LoggerClients &clients, unsigned id, const char * name, const rnumber::RNumber & value) :
       _clients(clients), _id(id), _name(name), _value(value) {};

    logRegReadObj *next() { return static_cast<logRegReadObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logRegRead(_id, _name.c_str(), _value);
      }
    };

    LoggerClients &_clients;
    unsigned _id;
    std::string _name;
    RNumber _value;

  };


  void log_reg_read(unsigned id, const char * name, const rnumber::RNumber & value)
  {
    TransLogBase *obj = new logRegReadObj(_clients, id, name, value);
    add_event(obj);
  }


  struct logCacheActionObj : public TransLogBase {
    logCacheActionObj(LoggerClients &clients, const char * name, CacheAction ca, CacheAccess cac, unsigned level, int set, int way, unsigned linemask, addr_t ra) :
       _clients(clients), _name(name), _ca(ca), _cac(cac), _level(level), _set(set), _way(way), _linemask(linemask), _ra(ra) {};

    logCacheActionObj *next() { return static_cast<logCacheActionObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logCacheAction(_name, _ca, _cac, _level, _set, _way, _linemask, _ra);
      }
    };

    LoggerClients &_clients;
    const char * _name;
    CacheAction _ca;
    CacheAccess _cac;
    unsigned _level;
    int _set;
    int _way;
    unsigned _linemask;
    addr_t _ra;

  };


  void log_cache_action(const char * name, CacheAction ca, CacheAccess cac, unsigned level, int set, int way, unsigned linemask, addr_t ra)
  {
    TransLogBase *obj = new logCacheActionObj(_clients, name, ca, cac, level, set, way, linemask, ra);
    add_event(obj);
  }


  struct logCoreMemReadTypedObj : public TransLogBase {
    logCoreMemReadTypedObj(LoggerClients &clients, unsigned id, const char * name, addr_t ea, int nb, CacheAccess type) :
       _clients(clients), _id(id), _name(name), _ea(ea), _nb(nb), _type(type) {};

    logCoreMemReadTypedObj *next() { return static_cast<logCoreMemReadTypedObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logCoreMemReadTyped(_id, _name, _ea, _nb, _type);
      }
    };

    LoggerClients &_clients;
    unsigned _id;
    const char * _name;
    addr_t _ea;
    int _nb;
    CacheAccess _type;

  };


  void log_core_mem_read_typed(unsigned id, const char * name, addr_t ea, int nb, CacheAccess type)
  {
    TransLogBase *obj = new logCoreMemReadTypedObj(_clients, id, name, ea, nb, type);
    add_event(obj);
  }


  struct logCoreMemReadObj : public TransLogBase {
    logCoreMemReadObj(LoggerClients &clients, unsigned id, const char * name, addr_t ea, int nb) :
       _clients(clients), _id(id), _name(name), _ea(ea), _nb(nb) {};

    logCoreMemReadObj *next() { return static_cast<logCoreMemReadObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logCoreMemRead(_id, _name, _ea, _nb);
      }
    };

    LoggerClients &_clients;
    unsigned _id;
    const char * _name;
    addr_t _ea;
    int _nb;

  };


  void log_core_mem_read(unsigned id, const char * name, addr_t ea, int nb)
  {
    TransLogBase *obj = new logCoreMemReadObj(_clients, id, name, ea, nb);
    add_event(obj);
  }


  struct logBreakpointObj : public TransLogBase {
    logBreakpointObj(LoggerClients &clients, addr_t ea, unsigned h, PtBase * payload) :
       _clients(clients), _ea(ea), _h(h), _payload(payload) {};

    logBreakpointObj *next() { return static_cast<logBreakpointObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logBreakpoint(_ea, _h, _payload);
      }
    };

    LoggerClients &_clients;
    addr_t _ea;
    unsigned _h;
    PtBase * _payload;

  };

void log_breakpoint(addr_t ea, unsigned h, PtBase * payload)  {
     // Breakpoints are directly logged b/c they are generated at free-packet time.
     ForEach(_clients,i) {
      i->second->logBreakpoint(ea, h, payload);
    }
  }

  struct logCoreMemWriteTypedObj : public TransLogBase {
    logCoreMemWriteTypedObj(LoggerClients &clients, unsigned id, const char * name, addr_t ea, int nb, CacheAccess type) :
       _clients(clients), _id(id), _name(name), _ea(ea), _nb(nb), _type(type) {};

    logCoreMemWriteTypedObj *next() { return static_cast<logCoreMemWriteTypedObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logCoreMemWriteTyped(_id, _name, _ea, _nb, _type);
      }
    };

    LoggerClients &_clients;
    unsigned _id;
    const char * _name;
    addr_t _ea;
    int _nb;
    CacheAccess _type;

  };


  void log_core_mem_write_typed(unsigned id, const char * name, addr_t ea, int nb, CacheAccess type)
  {
    TransLogBase *obj = new logCoreMemWriteTypedObj(_clients, id, name, ea, nb, type);
    add_event(obj);
  }


  struct logCoreMemWriteObj : public TransLogBase {
    logCoreMemWriteObj(LoggerClients &clients, unsigned id, const char * name, addr_t ea, int nb) :
       _clients(clients), _id(id), _name(name), _ea(ea), _nb(nb) {};

    logCoreMemWriteObj *next() { return static_cast<logCoreMemWriteObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logCoreMemWrite(_id, _name, _ea, _nb);
      }
    };

    LoggerClients &_clients;
    unsigned _id;
    const char * _name;
    addr_t _ea;
    int _nb;

  };


  void log_core_mem_write(unsigned id, const char * name, addr_t ea, int nb)
  {
    TransLogBase *obj = new logCoreMemWriteObj(_clients, id, name, ea, nb);
    add_event(obj);
  }


  struct logInstrPrefetchObj : public TransLogBase {
    logInstrPrefetchObj(LoggerClients &clients, addr_t ea) :
       _clients(clients), _ea(ea) {};

    logInstrPrefetchObj *next() { return static_cast<logInstrPrefetchObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logInstrPrefetch(_ea);
      }
    };

    LoggerClients &_clients;
    addr_t _ea;

  };

void log_instr_prefetch(addr_t ea)  {
    // This item is added to the back of the queue because it represents the first item in
    // an instruction grouping and we iterate over the list in reverse when committing log messages.
    TransLogBase *obj = new logInstrPrefetchObj(_clients,ea);
    add_to_pending_queue_tail(obj);
    _last_instr_fetch = ea;
  }

  struct logBranchTakenObj : public TransLogBase {
    logBranchTakenObj(LoggerClients &clients, addr_t ea) :
       _clients(clients), _ea(ea) {};

    logBranchTakenObj *next() { return static_cast<logBranchTakenObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logBranchTaken(_ea);
      }
    };

    LoggerClients &_clients;
    addr_t _ea;

  };


  void log_branch_taken(addr_t ea)
  {
    TransLogBase *obj = new logBranchTakenObj(_clients, ea);
    add_event(obj);
  }


  struct logExceptionObj : public TransLogBase {
    logExceptionObj(LoggerClients &clients, unsigned handle, bool pre, const char * name) :
       _clients(clients), _handle(handle), _pre(pre), _name(name) {};

    logExceptionObj *next() { return static_cast<logExceptionObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logException(_handle, _pre, _name);
      }
    };

    LoggerClients &_clients;
    unsigned _handle;
    bool _pre;
    const char * _name;

  };


  void log_exception(unsigned handle, bool pre, const char * name)
  {
    TransLogBase *obj = new logExceptionObj(_clients, handle, pre, name);
    add_event(obj);
  }


  struct logRegfileWriteMaskObj : public TransLogBase {
    logRegfileWriteMaskObj(LoggerClients &clients, unsigned id, const char * name, uint32_t index, const rnumber::RNumber & value, const rnumber::RNumber & mask) :
       _clients(clients), _id(id), _name(name), _index(index), _value(value), _mask(mask) {};

    logRegfileWriteMaskObj *next() { return static_cast<logRegfileWriteMaskObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logRegfileWriteMask(_id, _name, _index, _value, _mask);
      }
    };

    LoggerClients &_clients;
    unsigned _id;
    const char * _name;
    uint32_t _index;
    RNumber _value;
    RNumber _mask;

  };


  void log_regfile_write_mask(unsigned id, const char * name, uint32_t index, const rnumber::RNumber & value, const rnumber::RNumber & mask)
  {
    TransLogBase *obj = new logRegfileWriteMaskObj(_clients, id, name, index, value, mask);
    add_event(obj);
  }


  struct logBasicBlockEndObj : public TransLogBase {
    logBasicBlockEndObj(LoggerClients &clients, addr_t ea) :
       _clients(clients), _ea(ea) {};

    logBasicBlockEndObj *next() { return static_cast<logBasicBlockEndObj *>(_next); };

    void commit() {
      ForEach(_clients,i) {
        i->second->logBasicBlockEnd(_ea);
      }
    };

    LoggerClients &_clients;
    addr_t _ea;

  };


  void log_basic_block_end(addr_t ea)
  {
    TransLogBase *obj = new logBasicBlockEndObj(_clients, ea);
    add_event(obj);
  }


  // Flush anything in the pending queue.
  void log_flush_pending()
  {
    while (_pending_log_head) {
      TransLogBase *tmp = _pending_log_head;
      _pending_log_head = _pending_log_head->_next;
      delete tmp;
    }
    _pending_log_head = _pending_log_tail = 0;
    log_clear_instr_reads();
  }

  };

  // MT safety not really supported at this point...
  LoggingObjBase *createLoggingObj(IssCore &core,bool mtsafe)
  {
    return new LoggingObj(core);
  }

}



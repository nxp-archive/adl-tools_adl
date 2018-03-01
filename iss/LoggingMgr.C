//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This file is generated automatically by the script iss/simple_logging_events.
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


    // Dummy function for compatibility with the transactional-ISS logging features.
    // Direct logging is always on.
    bool log_direct(bool d)
    {
      return true;
    }

    LogType log_get_pending_queue()
    {
      return 0;
    }

    void log_append_pending_queue(LogType p)
    {
    }

    void log_clear_instr_reads()
    {
    }

    // Dummy function for flushing.
    void log_flush_pending()
    {
    }

    void log_discard(InstrPacketBase *)
    {
    }

    // Dummuy function for committing.
    void log_commit(InstrPacketBase *)
    {
    }


    void log_ctx_change(unsigned id, const char * name, unsigned context_num, CtxType update)
    {
  		
      ForEach(_clients,i) {
        i->second->logCtxChange(id, name, context_num, update);
      }
    }

    void log_instr_read(unsigned id, const char * name, addr_t ea, addr_t ra, uint32_t value)
    {
  		
      ForEach(_clients,i) {
        i->second->logInstrRead(id, name, ea, ra, value);
      }
    }

    void log_branch_end(addr_t ea)
    {
  		
      ForEach(_clients,i) {
        i->second->logBranchEnd(ea);
      }
    }

    void log_reg_write(unsigned id, const char * name, const rnumber::RNumber & value)
    {
  		
      ForEach(_clients,i) {
        i->second->logRegWrite(id, name, value);
      }
    }

    void log_watchpoint(addr_t ea, bool pre, WatchType type, int num_bytes, uint64_t value, unsigned h, PtBase * payload)
    {
  		
      ForEach(_clients,i) {
        i->second->logWatchpoint(ea, pre, type, num_bytes, value, h, payload);
      }
    }

    void log_annotation(MsgType type, unsigned level, const string & msg, unsigned ndata, LogBase::Data * data)
    {
  		
      ForEach(_clients,i) {
        i->second->logAnnotation(type, level, msg, ndata, data);
      }
    }

    void log_regfile_read_mask(unsigned id, const char * name, uint32_t index, const rnumber::RNumber & value, const rnumber::RNumber & mask)
    {
  		
      ForEach(_clients,i) {
        i->second->logRegfileReadMask(id, name, index, value, mask);
      }
    }

    void log_regfile_read(unsigned id, const char * name, uint32_t index, const rnumber::RNumber & value)
    {
  		
      ForEach(_clients,i) {
        i->second->logRegfileRead(id, name, index, value);
      }
    }

    void log_regfile_ext_write(unsigned id, const char * name, uint32_t index, const rnumber::RNumber & value)
    {
  		
      ForEach(_clients,i) {
        i->second->logRegfileExtWrite(id, name, index, value);
      }
    }

    void log_instr(const uint32_t* opc, int num_half_bytes, const char * name, Disassembler dis, uint32_t flags)
    {
  		
      ForEach(_clients,i) {
        i->second->logInstr(opc, num_half_bytes, name, dis, flags);
      }
    }

    void log_mem_read(unsigned id, const char * name, bool pre, int seq, addr_t ea, addr_t ra, uint32_t value)
    {
  		
      ForEach(_clients,i) {
        i->second->logMemRead(id, name, pre, seq, ea, ra, value);
      }
    }

    void log_reg_ext_write(unsigned id, const char * name, const rnumber::RNumber & value)
    {
  		
      ForEach(_clients,i) {
        i->second->logRegExtWrite(id, name, value);
      }
    }

    void log_instr_completion_time(ttime_t completion_time)
    {
  		
      ForEach(_clients,i) {
        i->second->logInstrCompletionTime(completion_time);
      }
    }

    void log_mmu_translation(TransType tt, int seq, int set, int way, addr_t ea, addr_t ra, const MmuBase * trans)
    {
  		
      ForEach(_clients,i) {
        i->second->logMmuTranslation(tt, seq, set, way, ea, ra, trans);
      }
    }

    void log_instr_issue_time(ttime_t issue_time)
    {
  		
      ForEach(_clients,i) {
        i->second->logInstrIssueTime(issue_time);
      }
    }

    void log_regfile_write(unsigned id, const char * name, uint32_t index, const rnumber::RNumber & value)
    {
  		
      ForEach(_clients,i) {
        i->second->logRegfileWrite(id, name, index, value);
      }
    }

    void log_mem_write(unsigned id, const char * name, bool pre, int seq, addr_t ea, addr_t ra, uint32_t value)
    {
  		
      ForEach(_clients,i) {
        i->second->logMemWrite(id, name, pre, seq, ea, ra, value);
      }
    }

    void log_core_change()
    {
  		
      ForEach(_clients,i) {
        i->second->logCoreChange();
      }
    }

    void log_instr_from_buffer()
    {
  		
      ForEach(_clients,i) {
        i->second->logInstrFromBuffer();
      }
    }

    void log_reg_write_mask(unsigned id, const char * name, const rnumber::RNumber & value, const rnumber::RNumber & mask)
    {
  		
      ForEach(_clients,i) {
        i->second->logRegWriteMask(id, name, value, mask);
      }
    }

    void log_instr_args(const uint32_t* opc, int num_half_bytes, const char * name, Disassembler dis)
    {
  		
      ForEach(_clients,i) {
        i->second->logInstrArgs(opc, num_half_bytes, name, dis);
      }
    }

    void log_reg_read_mask(unsigned id, const char * name, const rnumber::RNumber & value, const rnumber::RNumber & mask)
    {
  		
      ForEach(_clients,i) {
        i->second->logRegReadMask(id, name, value, mask);
      }
    }

    void log_reg_read(unsigned id, const char * name, const rnumber::RNumber & value)
    {
  		
      ForEach(_clients,i) {
        i->second->logRegRead(id, name, value);
      }
    }

    void log_cache_action(const char * name, CacheAction ca, CacheAccess cac, unsigned level, int set, int way, unsigned linemask, addr_t ra)
    {
  		
      ForEach(_clients,i) {
        i->second->logCacheAction(name, ca, cac, level, set, way, linemask, ra);
      }
    }

    void log_core_mem_read_typed(unsigned id, const char * name, addr_t ea, int nb, CacheAccess type)
    {
  		
      ForEach(_clients,i) {
        i->second->logCoreMemReadTyped(id, name, ea, nb, type);
      }
    }

    void log_core_mem_read(unsigned id, const char * name, addr_t ea, int nb)
    {
  		
      ForEach(_clients,i) {
        i->second->logCoreMemRead(id, name, ea, nb);
      }
    }

    void log_breakpoint(addr_t ea, unsigned h, PtBase * payload)
    {
  		
      ForEach(_clients,i) {
        i->second->logBreakpoint(ea, h, payload);
      }
    }

    void log_core_mem_write_typed(unsigned id, const char * name, addr_t ea, int nb, CacheAccess type)
    {
  		
      ForEach(_clients,i) {
        i->second->logCoreMemWriteTyped(id, name, ea, nb, type);
      }
    }

    void log_core_mem_write(unsigned id, const char * name, addr_t ea, int nb)
    {
  		
      ForEach(_clients,i) {
        i->second->logCoreMemWrite(id, name, ea, nb);
      }
    }

    void log_instr_prefetch(addr_t ea)
    {
  		
      ForEach(_clients,i) {
        i->second->logInstrPrefetch(ea);
      }
    }

    void log_branch_taken(addr_t ea)
    {
  		
      ForEach(_clients,i) {
        i->second->logBranchTaken(ea);
      }
    }

    void log_exception(unsigned handle, bool pre, const char * name)
    {
  		
      ForEach(_clients,i) {
        i->second->logException(handle, pre, name);
      }
    }

    void log_regfile_write_mask(unsigned id, const char * name, uint32_t index, const rnumber::RNumber & value, const rnumber::RNumber & mask)
    {
  		
      ForEach(_clients,i) {
        i->second->logRegfileWriteMask(id, name, index, value, mask);
      }
    }

    void log_basic_block_end(addr_t ea)
    {
  		
      ForEach(_clients,i) {
        i->second->logBasicBlockEnd(ea);
      }
    }
  };


# ifndef _MSC_VER

  struct LoggingMtObj : public ThreadedLogger {


  struct logCtxChangeMtObj : public MtLogBase {
    logCtxChangeMtObj(unsigned id, const char * name, unsigned context_num, CtxType update)
        :
_id(id), _name(name), _context_num(context_num), _update(update) {};

    void doLog(LoggingIface *iface) {
			iface->logCtxChange(_id, _name, _context_num, _update);
    };

    
    unsigned _id;
    const char * _name;
    unsigned _context_num;
    CtxType _update;

  };

  void log_ctx_change(unsigned id, const char * name, unsigned context_num, CtxType update)
  {
    MtLogBase *obj = new logCtxChangeMtObj(id, name, context_num, update);
		while (!_tqueue.push(obj));
  }


  struct logInstrReadMtObj : public MtLogBase {
    logInstrReadMtObj(unsigned id, const char * name, addr_t ea, addr_t ra, uint32_t value)
        :
_id(id), _name(name), _ea(ea), _ra(ra), _value(value) {};

    void doLog(LoggingIface *iface) {
			iface->logInstrRead(_id, _name, _ea, _ra, _value);
    };

    
    unsigned _id;
    const char * _name;
    addr_t _ea;
    addr_t _ra;
    uint32_t _value;

  };

  void log_instr_read(unsigned id, const char * name, addr_t ea, addr_t ra, uint32_t value)
  {
    MtLogBase *obj = new logInstrReadMtObj(id, name, ea, ra, value);
		while (!_tqueue.push(obj));
  }


  struct logBranchEndMtObj : public MtLogBase {
    logBranchEndMtObj(addr_t ea)
        :
_ea(ea) {};

    void doLog(LoggingIface *iface) {
			iface->logBranchEnd(_ea);
    };

    
    addr_t _ea;

  };

  void log_branch_end(addr_t ea)
  {
    MtLogBase *obj = new logBranchEndMtObj(ea);
		while (!_tqueue.push(obj));
  }


  struct logRegWriteMtObj : public MtLogBase {
    logRegWriteMtObj(unsigned id, const char * name, const rnumber::RNumber & value)
        :
_id(id), _name(name), _value(value) {};

    void doLog(LoggingIface *iface) {
			iface->logRegWrite(_id, _name, _value);
    };

    
    unsigned _id;
    const char * _name;
    RNumber _value;

  };

  void log_reg_write(unsigned id, const char * name, const rnumber::RNumber & value)
  {
    MtLogBase *obj = new logRegWriteMtObj(id, name, value);
		while (!_tqueue.push(obj));
  }


  struct logWatchpointMtObj : public MtLogBase {
    logWatchpointMtObj(addr_t ea, bool pre, WatchType type, int num_bytes, uint64_t value, unsigned h, PtBase * payload)
        :
_ea(ea), _pre(pre), _type(type), _num_bytes(num_bytes), _value(value), _h(h), _payload(payload) {};

    void doLog(LoggingIface *iface) {
			iface->logWatchpoint(_ea, _pre, _type, _num_bytes, _value, _h, _payload);
    };

    
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
    MtLogBase *obj = new logWatchpointMtObj(ea, pre, type, num_bytes, value, h, payload);
		while (!_tqueue.push(obj));
  }


  struct logAnnotationMtObj : public MtLogBase {
    logAnnotationMtObj(MsgType type, unsigned level, const string & msg, unsigned ndata, LogBase::Data * data)
        :
_type(type), _level(level), _msg(msg), _ndata(ndata) {_data.insert(_data.begin(),&data[0],&data[ndata]);};

    void doLog(LoggingIface *iface) {
			iface->logAnnotation(_type, _level, _msg, _ndata, ((_data.empty()) ? 0 : &_data[0]));
    };

    
    MsgType _type;
    unsigned _level;
    string _msg;
    unsigned _ndata;
    std::vector<LogBase::Data> _data;

  };

  void log_annotation(MsgType type, unsigned level, const string & msg, unsigned ndata, LogBase::Data * data)
  {
    MtLogBase *obj = new logAnnotationMtObj(type, level, msg, ndata, data);
		while (!_tqueue.push(obj));
  }


  struct logRegfileReadMaskMtObj : public MtLogBase {
    logRegfileReadMaskMtObj(unsigned id, const char * name, uint32_t index, const rnumber::RNumber & value, const rnumber::RNumber & mask)
        :
_id(id), _name(name), _index(index), _value(value), _mask(mask) {};

    void doLog(LoggingIface *iface) {
			iface->logRegfileReadMask(_id, _name, _index, _value, _mask);
    };

    
    unsigned _id;
    const char * _name;
    uint32_t _index;
    RNumber _value;
    RNumber _mask;

  };

  void log_regfile_read_mask(unsigned id, const char * name, uint32_t index, const rnumber::RNumber & value, const rnumber::RNumber & mask)
  {
    MtLogBase *obj = new logRegfileReadMaskMtObj(id, name, index, value, mask);
		while (!_tqueue.push(obj));
  }


  struct logRegfileReadMtObj : public MtLogBase {
    logRegfileReadMtObj(unsigned id, const char * name, uint32_t index, const rnumber::RNumber & value)
        :
_id(id), _name(name), _index(index), _value(value) {};

    void doLog(LoggingIface *iface) {
			iface->logRegfileRead(_id, _name, _index, _value);
    };

    
    unsigned _id;
    const char * _name;
    uint32_t _index;
    RNumber _value;

  };

  void log_regfile_read(unsigned id, const char * name, uint32_t index, const rnumber::RNumber & value)
  {
    MtLogBase *obj = new logRegfileReadMtObj(id, name, index, value);
		while (!_tqueue.push(obj));
  }


  struct logRegfileExtWriteMtObj : public MtLogBase {
    logRegfileExtWriteMtObj(unsigned id, const char * name, uint32_t index, const rnumber::RNumber & value)
        :
_id(id), _name(name), _index(index), _value(value) {};

    void doLog(LoggingIface *iface) {
			iface->logRegfileExtWrite(_id, _name, _index, _value);
    };

    
    unsigned _id;
    const char * _name;
    uint32_t _index;
    RNumber _value;

  };

  void log_regfile_ext_write(unsigned id, const char * name, uint32_t index, const rnumber::RNumber & value)
  {
    MtLogBase *obj = new logRegfileExtWriteMtObj(id, name, index, value);
		while (!_tqueue.push(obj));
  }


  struct logInstrMtObj : public MtLogBase {
    logInstrMtObj(const uint32_t* opc, int num_half_bytes, const char * name, Disassembler dis, uint32_t flags)
        :
_num_half_bytes(num_half_bytes), _name(name), _dis(dis), _flags(flags) { _opc = new uint32_t[2]; _opc[0] = opc[0]; if ((num_half_bytes/8) > 1) { _opc[1] = opc[1];} };
    ~logInstrMtObj() {  delete [] _opc; ;};


    void doLog(LoggingIface *iface) {
			iface->logInstr(_opc, _num_half_bytes, _name, _dis, _flags);
    };

    
    uint32_t* _opc;
    int _num_half_bytes;
    const char * _name;
    Disassembler _dis;
    uint32_t _flags;

  };

  void log_instr(const uint32_t* opc, int num_half_bytes, const char * name, Disassembler dis, uint32_t flags)
  {
    MtLogBase *obj = new logInstrMtObj(opc, num_half_bytes, name, dis, flags);
		while (!_tqueue.push(obj));
  }


  struct logMemReadMtObj : public MtLogBase {
    logMemReadMtObj(unsigned id, const char * name, bool pre, int seq, addr_t ea, addr_t ra, uint32_t value)
        :
_id(id), _name(name), _pre(pre), _seq(seq), _ea(ea), _ra(ra), _value(value) {};

    void doLog(LoggingIface *iface) {
			iface->logMemRead(_id, _name, _pre, _seq, _ea, _ra, _value);
    };

    
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
    MtLogBase *obj = new logMemReadMtObj(id, name, pre, seq, ea, ra, value);
		while (!_tqueue.push(obj));
  }


  struct logRegExtWriteMtObj : public MtLogBase {
    logRegExtWriteMtObj(unsigned id, const char * name, const rnumber::RNumber & value)
        :
_id(id), _name(name), _value(value) {};

    void doLog(LoggingIface *iface) {
			iface->logRegExtWrite(_id, _name, _value);
    };

    
    unsigned _id;
    const char * _name;
    RNumber _value;

  };

  void log_reg_ext_write(unsigned id, const char * name, const rnumber::RNumber & value)
  {
    MtLogBase *obj = new logRegExtWriteMtObj(id, name, value);
		while (!_tqueue.push(obj));
  }


  struct logInstrCompletionTimeMtObj : public MtLogBase {
    logInstrCompletionTimeMtObj(ttime_t completion_time)
        :
_completion_time(completion_time) {};

    void doLog(LoggingIface *iface) {
			iface->logInstrCompletionTime(_completion_time);
    };

    
    ttime_t _completion_time;

  };

  void log_instr_completion_time(ttime_t completion_time)
  {
    MtLogBase *obj = new logInstrCompletionTimeMtObj(completion_time);
		while (!_tqueue.push(obj));
  }


  struct logMmuTranslationMtObj : public MtLogBase {
    logMmuTranslationMtObj(TransType tt, int seq, int set, int way, addr_t ea, addr_t ra, const MmuBase * trans)
        :
_tt(tt), _seq(seq), _set(set), _way(way), _ea(ea), _ra(ra) {_trans = (trans->user_entry()) ? trans->clone() : trans;};
    ~logMmuTranslationMtObj() { if (_trans->user_entry()) delete _trans; ;};


    void doLog(LoggingIface *iface) {
			iface->logMmuTranslation(_tt, _seq, _set, _way, _ea, _ra, _trans);
    };

    
    TransType _tt;
    int _seq;
    int _set;
    int _way;
    addr_t _ea;
    addr_t _ra;
    const MmuBase* _trans;

  };

  void log_mmu_translation(TransType tt, int seq, int set, int way, addr_t ea, addr_t ra, const MmuBase * trans)
  {
    MtLogBase *obj = new logMmuTranslationMtObj(tt, seq, set, way, ea, ra, trans);
		while (!_tqueue.push(obj));
  }


  struct logInstrIssueTimeMtObj : public MtLogBase {
    logInstrIssueTimeMtObj(ttime_t issue_time)
        :
_issue_time(issue_time) {};

    void doLog(LoggingIface *iface) {
			iface->logInstrIssueTime(_issue_time);
    };

    
    ttime_t _issue_time;

  };

  void log_instr_issue_time(ttime_t issue_time)
  {
    MtLogBase *obj = new logInstrIssueTimeMtObj(issue_time);
		while (!_tqueue.push(obj));
  }


  struct logRegfileWriteMtObj : public MtLogBase {
    logRegfileWriteMtObj(unsigned id, const char * name, uint32_t index, const rnumber::RNumber & value)
        :
_id(id), _name(name), _index(index), _value(value) {};

    void doLog(LoggingIface *iface) {
			iface->logRegfileWrite(_id, _name, _index, _value);
    };

    
    unsigned _id;
    const char * _name;
    uint32_t _index;
    RNumber _value;

  };

  void log_regfile_write(unsigned id, const char * name, uint32_t index, const rnumber::RNumber & value)
  {
    MtLogBase *obj = new logRegfileWriteMtObj(id, name, index, value);
		while (!_tqueue.push(obj));
  }


  struct logMemWriteMtObj : public MtLogBase {
    logMemWriteMtObj(unsigned id, const char * name, bool pre, int seq, addr_t ea, addr_t ra, uint32_t value)
        :
_id(id), _name(name), _pre(pre), _seq(seq), _ea(ea), _ra(ra), _value(value) {};

    void doLog(LoggingIface *iface) {
			iface->logMemWrite(_id, _name, _pre, _seq, _ea, _ra, _value);
    };

    
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
    MtLogBase *obj = new logMemWriteMtObj(id, name, pre, seq, ea, ra, value);
		while (!_tqueue.push(obj));
  }


  struct logCoreChangeMtObj : public MtLogBase {
    logCoreChangeMtObj()
        {};

    void doLog(LoggingIface *iface) {
			iface->logCoreChange();
    };

    

  };

  void log_core_change()
  {
    MtLogBase *obj = new logCoreChangeMtObj();
		while (!_tqueue.push(obj));
  }


  struct logInstrFromBufferMtObj : public MtLogBase {
    logInstrFromBufferMtObj()
        {};

    void doLog(LoggingIface *iface) {
			iface->logInstrFromBuffer();
    };

    

  };

  void log_instr_from_buffer()
  {
    MtLogBase *obj = new logInstrFromBufferMtObj();
		while (!_tqueue.push(obj));
  }


  struct logRegWriteMaskMtObj : public MtLogBase {
    logRegWriteMaskMtObj(unsigned id, const char * name, const rnumber::RNumber & value, const rnumber::RNumber & mask)
        :
_id(id), _name(name), _value(value), _mask(mask) {};

    void doLog(LoggingIface *iface) {
			iface->logRegWriteMask(_id, _name, _value, _mask);
    };

    
    unsigned _id;
    const char * _name;
    RNumber _value;
    RNumber _mask;

  };

  void log_reg_write_mask(unsigned id, const char * name, const rnumber::RNumber & value, const rnumber::RNumber & mask)
  {
    MtLogBase *obj = new logRegWriteMaskMtObj(id, name, value, mask);
		while (!_tqueue.push(obj));
  }


  struct logInstrArgsMtObj : public MtLogBase {
    logInstrArgsMtObj(const uint32_t* opc, int num_half_bytes, const char * name, Disassembler dis)
        :
_num_half_bytes(num_half_bytes), _name(name), _dis(dis) { _opc = new uint32_t[2]; _opc[0] = opc[0]; if ((num_half_bytes/8) > 1) { _opc[1] = opc[1];} };
    ~logInstrArgsMtObj() {  delete [] _opc; ;};


    void doLog(LoggingIface *iface) {
			iface->logInstrArgs(_opc, _num_half_bytes, _name, _dis);
    };

    
    uint32_t* _opc;
    int _num_half_bytes;
    const char * _name;
    Disassembler _dis;

  };

  void log_instr_args(const uint32_t* opc, int num_half_bytes, const char * name, Disassembler dis)
  {
    MtLogBase *obj = new logInstrArgsMtObj(opc, num_half_bytes, name, dis);
		while (!_tqueue.push(obj));
  }


  struct logRegReadMaskMtObj : public MtLogBase {
    logRegReadMaskMtObj(unsigned id, const char * name, const rnumber::RNumber & value, const rnumber::RNumber & mask)
        :
_id(id), _name(name), _value(value), _mask(mask) {};

    void doLog(LoggingIface *iface) {
			iface->logRegReadMask(_id, _name, _value, _mask);
    };

    
    unsigned _id;
    const char * _name;
    RNumber _value;
    RNumber _mask;

  };

  void log_reg_read_mask(unsigned id, const char * name, const rnumber::RNumber & value, const rnumber::RNumber & mask)
  {
    MtLogBase *obj = new logRegReadMaskMtObj(id, name, value, mask);
		while (!_tqueue.push(obj));
  }


  struct logRegReadMtObj : public MtLogBase {
    logRegReadMtObj(unsigned id, const char * name, const rnumber::RNumber & value)
        :
_id(id), _name(name), _value(value) {};

    void doLog(LoggingIface *iface) {
			iface->logRegRead(_id, _name, _value);
    };

    
    unsigned _id;
    const char * _name;
    RNumber _value;

  };

  void log_reg_read(unsigned id, const char * name, const rnumber::RNumber & value)
  {
    MtLogBase *obj = new logRegReadMtObj(id, name, value);
		while (!_tqueue.push(obj));
  }


  struct logCacheActionMtObj : public MtLogBase {
    logCacheActionMtObj(const char * name, CacheAction ca, CacheAccess cac, unsigned level, int set, int way, unsigned linemask, addr_t ra)
        :
_name(name), _ca(ca), _cac(cac), _level(level), _set(set), _way(way), _linemask(linemask), _ra(ra) {};

    void doLog(LoggingIface *iface) {
			iface->logCacheAction(_name, _ca, _cac, _level, _set, _way, _linemask, _ra);
    };

    
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
    MtLogBase *obj = new logCacheActionMtObj(name, ca, cac, level, set, way, linemask, ra);
		while (!_tqueue.push(obj));
  }


  struct logCoreMemReadTypedMtObj : public MtLogBase {
    logCoreMemReadTypedMtObj(unsigned id, const char * name, addr_t ea, int nb, CacheAccess type)
        :
_id(id), _name(name), _ea(ea), _nb(nb), _type(type) {};

    void doLog(LoggingIface *iface) {
			iface->logCoreMemReadTyped(_id, _name, _ea, _nb, _type);
    };

    
    unsigned _id;
    const char * _name;
    addr_t _ea;
    int _nb;
    CacheAccess _type;

  };

  void log_core_mem_read_typed(unsigned id, const char * name, addr_t ea, int nb, CacheAccess type)
  {
    MtLogBase *obj = new logCoreMemReadTypedMtObj(id, name, ea, nb, type);
		while (!_tqueue.push(obj));
  }


  struct logCoreMemReadMtObj : public MtLogBase {
    logCoreMemReadMtObj(unsigned id, const char * name, addr_t ea, int nb)
        :
_id(id), _name(name), _ea(ea), _nb(nb) {};

    void doLog(LoggingIface *iface) {
			iface->logCoreMemRead(_id, _name, _ea, _nb);
    };

    
    unsigned _id;
    const char * _name;
    addr_t _ea;
    int _nb;

  };

  void log_core_mem_read(unsigned id, const char * name, addr_t ea, int nb)
  {
    MtLogBase *obj = new logCoreMemReadMtObj(id, name, ea, nb);
		while (!_tqueue.push(obj));
  }


  struct logBreakpointMtObj : public MtLogBase {
    logBreakpointMtObj(addr_t ea, unsigned h, PtBase * payload)
        :
_ea(ea), _h(h), _payload(payload) {};

    void doLog(LoggingIface *iface) {
			iface->logBreakpoint(_ea, _h, _payload);
    };

    
    addr_t _ea;
    unsigned _h;
    PtBase * _payload;

  };

  void log_breakpoint(addr_t ea, unsigned h, PtBase * payload)
  {
    MtLogBase *obj = new logBreakpointMtObj(ea, h, payload);
		while (!_tqueue.push(obj));
  }


  struct logCoreMemWriteTypedMtObj : public MtLogBase {
    logCoreMemWriteTypedMtObj(unsigned id, const char * name, addr_t ea, int nb, CacheAccess type)
        :
_id(id), _name(name), _ea(ea), _nb(nb), _type(type) {};

    void doLog(LoggingIface *iface) {
			iface->logCoreMemWriteTyped(_id, _name, _ea, _nb, _type);
    };

    
    unsigned _id;
    const char * _name;
    addr_t _ea;
    int _nb;
    CacheAccess _type;

  };

  void log_core_mem_write_typed(unsigned id, const char * name, addr_t ea, int nb, CacheAccess type)
  {
    MtLogBase *obj = new logCoreMemWriteTypedMtObj(id, name, ea, nb, type);
		while (!_tqueue.push(obj));
  }


  struct logCoreMemWriteMtObj : public MtLogBase {
    logCoreMemWriteMtObj(unsigned id, const char * name, addr_t ea, int nb)
        :
_id(id), _name(name), _ea(ea), _nb(nb) {};

    void doLog(LoggingIface *iface) {
			iface->logCoreMemWrite(_id, _name, _ea, _nb);
    };

    
    unsigned _id;
    const char * _name;
    addr_t _ea;
    int _nb;

  };

  void log_core_mem_write(unsigned id, const char * name, addr_t ea, int nb)
  {
    MtLogBase *obj = new logCoreMemWriteMtObj(id, name, ea, nb);
		while (!_tqueue.push(obj));
  }


  struct logInstrPrefetchMtObj : public MtLogBase {
    logInstrPrefetchMtObj(addr_t ea)
        :
_ea(ea) {};

    void doLog(LoggingIface *iface) {
			iface->logInstrPrefetch(_ea);
    };

    
    addr_t _ea;

  };

  void log_instr_prefetch(addr_t ea)
  {
    MtLogBase *obj = new logInstrPrefetchMtObj(ea);
		while (!_tqueue.push(obj));
  }


  struct logBranchTakenMtObj : public MtLogBase {
    logBranchTakenMtObj(addr_t ea)
        :
_ea(ea) {};

    void doLog(LoggingIface *iface) {
			iface->logBranchTaken(_ea);
    };

    
    addr_t _ea;

  };

  void log_branch_taken(addr_t ea)
  {
    MtLogBase *obj = new logBranchTakenMtObj(ea);
		while (!_tqueue.push(obj));
  }


  struct logExceptionMtObj : public MtLogBase {
    logExceptionMtObj(unsigned handle, bool pre, const char * name)
        :
_handle(handle), _pre(pre), _name(name) {};

    void doLog(LoggingIface *iface) {
			iface->logException(_handle, _pre, _name);
    };

    
    unsigned _handle;
    bool _pre;
    const char * _name;

  };

  void log_exception(unsigned handle, bool pre, const char * name)
  {
    MtLogBase *obj = new logExceptionMtObj(handle, pre, name);
		while (!_tqueue.push(obj));
  }


  struct logRegfileWriteMaskMtObj : public MtLogBase {
    logRegfileWriteMaskMtObj(unsigned id, const char * name, uint32_t index, const rnumber::RNumber & value, const rnumber::RNumber & mask)
        :
_id(id), _name(name), _index(index), _value(value), _mask(mask) {};

    void doLog(LoggingIface *iface) {
			iface->logRegfileWriteMask(_id, _name, _index, _value, _mask);
    };

    
    unsigned _id;
    const char * _name;
    uint32_t _index;
    RNumber _value;
    RNumber _mask;

  };

  void log_regfile_write_mask(unsigned id, const char * name, uint32_t index, const rnumber::RNumber & value, const rnumber::RNumber & mask)
  {
    MtLogBase *obj = new logRegfileWriteMaskMtObj(id, name, index, value, mask);
		while (!_tqueue.push(obj));
  }


  struct logBasicBlockEndMtObj : public MtLogBase {
    logBasicBlockEndMtObj(addr_t ea)
        :
_ea(ea) {};

    void doLog(LoggingIface *iface) {
			iface->logBasicBlockEnd(_ea);
    };

    
    addr_t _ea;

  };

  void log_basic_block_end(addr_t ea)
  {
    MtLogBase *obj = new logBasicBlockEndMtObj(ea);
		while (!_tqueue.push(obj));
  }


  struct logCommitMtObj : public MtLogBase {
    logCommitMtObj() : _ts(get_nstime()) {};

		uint64_t get_time_stamp() { return _ts; };

		// Uncomment for debug (uncomment DatWriterLogger::logCommit and ThreadedLogger as well).
		//void doLog(LoggingIface *iface) {
		//  iface->logCommit(_ts);
		//}

    uint64_t _ts;
  };

  // Record timestamp for ordering purposes.
  void log_commit (PacketLogBase *)
  {
		MtLogBase *obj = new logCommitMtObj;
		while (!_tqueue.push(obj));
	}


  };

# endif // _MSC_VER


  LoggingObjBase *createLoggingObj(IssCore &core,bool use_mt)
  {
#   ifdef _MSC_VER
    return static_cast<LoggingObjBase*>(new LoggingObj);
#   else
    return (use_mt) ? static_cast<LoggingObjBase*>(new LoggingMtObj) : static_cast<LoggingObjBase*>(new LoggingObj);
#   endif
  }

}



//

//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#include <assert.h>
#include <iomanip>
#include <sstream>

#include "helpers/Macros.h"

#include "iss/TypeIO.h"

#include "Logger.h"
#include "MemoryRequest.h"

using namespace std;

namespace uadl {

  void Logger::requestReportInner(ostream &o, const ReqInfo &rinfo, unsigned id, 
                                  addr_t ea, addr_t ra, unsigned size, unsigned serialized,
                                  const byte_t *data)
  {
    if (rinfo.valid_iid()) o << "iid=" << rinfo._instId << ", ";
    if (rinfo.valid_tid()) o << "tid=" << rinfo._threadId << ", ";

    o << "mid=" << id;

    o << ", ea=0x" << std::hex << ea << ", ra=0x" << ra << std::dec;

    if (size) {
      o << ", size=" << size;
    }

    if (serialized) {
      o << ", serialized=0x" << std::hex << serialized;
    }

    // A bit of a hack here: It's possible that data will be non-null but
    // invalid due to an offset being applied.  However, the offsets are always
    // small, so we just make sure that this is larger than an impossibly small
    // pointer.
    if (data > (const byte_t *)16) {
      o << ", data=0x" << std::hex << std::setfill('0');

      for (unsigned i = 0; i < size; i++) o << std::setw(2) << (unsigned)data[i];
    }
  }

  void Logger::requestReport(ostream &o, const ReqInfo &rinfo, unsigned id,
                            addr_t ea, addr_t ra, unsigned size, unsigned serialized,
                            const byte_t *data)
  {
    o << "(" << dec;
    requestReportInner(o,rinfo,id,ea,ra,size,serialized,data);
    o << ")";
  }

  void Logger::requestReport(ostream &o, const MemoryRequest &request,
                             const byte_t *data)
  {
    o << "(" << dec;
    requestReportInner(o, request.rinfo, request.id, request.ea,
                       request.addr, request.dataWidth, request.serialized, data);
    if (request.expired) {
      o << ", expired";
    }
    if (request.cancelled) {
      o << ", cancelled";
    }
    if (request.isHit()) {
      o << ", is-hit";
    }
    if (request.needLinefill()) {
      o << ", need-line-fill";
    }
    if (request.needEviction()) {
      o << ", need-eviction";
    }
    if (request.replaceLine) {
      o << ", replaced-line";
    }
    o << ")";
  }

  ostream &operator << (ostream &o,const MemoryRequest &request)
  {
    Logger::requestReport(o,request,0);
    return o;
  }

  string Logger::requestReport(const MemoryRequest &request,const byte_t *data)
  {
    ostringstream ss;
    requestReport(ss, request, data);
    return ss.str();
  }

  string Logger::requestReport(const ReqInfo &rinfo, uint64_t id, addr_t ea,
                               addr_t ra, unsigned size, unsigned serialized, const byte_t *data)
  {
    ostringstream ss;
    requestReport(ss, rinfo, id, ea, ra, size, serialized, data);
    return ss.str();
  }

  string Logger::getIndent(const string &pfx) const
  {
    return string(pfx.size() + (_name.empty() ? 0 : _name.length() + 2), ' ');
  }

  // MemoryLogger
  bool Logger::report(ostream &o,const string &pfx) const
  {
    if (hasLog()) {
      const string indent = getIndent(pfx);

      auto iter = _log.begin();
      o << pfx << (_name.empty() ? "" : _name + ": ") << *iter++ << "\n";

      for( ; iter != _log.end(); ++iter) {
        o << indent << *iter << "\n";
      }
      return true;
    } else {
      return false;
    }
  }

  bool Logger::report_and_clear(ostream &o,const string &pfx)
  {
    bool r = report(o,pfx);
    clear();
    return r;
  }

  void Logger::logMessage(const string &msg)
  {
    if (logAll()) {
      _log.push_back(msg);
    }
  }

  void Logger::logMessage(const char *msg)
  {
    if (logAll()) {
      _log.push_back(msg);
    }
  }

  void Logger::logStall(const string &msg)
  {
    if (_logclient) _logclient->logStall(*this,msg);
    if (logAll()) {
      _log.push_back("stall: "+msg);
    }
  }

  void Logger::logFlush(const string &msg)
  {
    if (_logclient) _logclient->logFlush(*this,msg);
    if (logAll()) {
      _log.push_back("flush: "+msg);
    }
  }

  void Logger::logOpAction(const char *action,const string &name,int index)
  {
    if (logAll()) {
      ostringstream ss;
      ss << action << " " << name;
      if (index >= 0) {
        ss << "[" << dec << index << "]";
      }
      logMessage(ss.str());
    }
  }

  void Logger::logOpAction(const char *action,const string &name,int index,uint64_t v)
  {
    if (logAll()) {
      ostringstream ss;
      ss << action << " " << name;
      if (index >= 0) {
        ss << "[" << dec << index << "]";
      }
      ss << "(0x" << hex << v << ")";
      logMessage(ss.str());
    }
  }

  void Logger::logOpAction(const string &action,const string &name,int index)
  {
    logOpAction(action.c_str(),name,index);
  }

  void Logger::logOpAction(const string &action,const string &name,int index,uint64_t v)
  {
    logOpAction(action.c_str(),name,index,v);
  }

  void Logger::logReadAvailFail(const string &name,int index,tag_t tag)
  {
    ostringstream ss;
    ss << "failed to read " << name;
    if (index >= 0) {
      ss << "[" << dec << index << "]";
    }
    ss << " (holder:  id=" << tag << ")";
    logStall(ss.str());
  }

  void Logger::logBranch(bool taken,addr_t target)
  {
    if (logAll()) {
      ostringstream ss;
      ss << "branch ";
      if (taken) {
        ss << "taken, target: 0x" << hex << target;
      } else {
        ss << "not taken";
      }
      logMessage(ss.str());
    }
  }

  void Logger::logReadNextRequest(uint64_t id, addr_t ea, addr_t ra,
                                  unsigned size, unsigned serialized,bool fetch,
                                  const ReqInfo &rinfo)
  {
    if (logAll()) {
      logMessage(string("send ") + (fetch ? "fetch" : "read") + " request " +
                 requestReport(rinfo, id, ea, ra, size, serialized));
    }
  }


  void Logger::logReadNextAction(uint64_t id, addr_t ea, addr_t ra,
                                 unsigned size, unsigned serialized, bool fetch,
                                 const byte_t *data, const ReqInfo &rinfo)
  {
    if (logAll()) {
      logMessage(string("receive ") + (fetch ? "fetch" : "read") + " data " +
                 requestReport(rinfo, id, ea, ra, size, serialized, data));
    }
  }


  void Logger::logWriteNextRequest(uint64_t id, addr_t ea, addr_t ra,
                                   unsigned size, unsigned serialized, const ReqInfo &rinfo)
  {
    if (logAll()) {
      logMessage("send write request " +
                 requestReport(rinfo, id, ea, ra, size, serialized));
    }
  }


  void Logger::logWriteNextAction(uint64_t id, addr_t ea, addr_t ra,
                                  unsigned size, unsigned serialized, const byte_t *data,
                                  const ReqInfo &rinfo)
  {
    if (logAll()) {
      logMessage("send write data " +
                 requestReport(rinfo, id, ea, ra, size, serialized, data));
    }
  }

  void Logger::logSendCacheCommand(MemRequestType type, addr_t ra)
  {
    if (logAll()) {
      ostringstream s;
      
      s << "send cache command " << type << " (ra=0x" << std::hex << ra
        << ")";
      
      logMessage(s.str());
    }
  }





  void MemoryLogger::logReadRequest(const MemoryRequest &request)
  {
    if (logAll()) {
      logMessage(string() + "receive " +
                 (request.isDataAccess() ? "read" : "fetch") + " request " +
                 requestReport(request));
    }
  }


  void MemoryLogger::logReadAction(const MemoryRequest &request,const byte_t *data)
  {
    if (logAll()) {
      logMessage(string() + "reply " +
                 (request.isDataAccess() ? "read" : "fetch") + " data " +
                 requestReport(request, data));
    }
  }


  void MemoryLogger::logWriteRequest(const MemoryRequest &request)
  { 
    if (logAll()) {
      logMessage("receive write request " + requestReport(request)); 
    }
  }


  void MemoryLogger::logWriteAction(const MemoryRequest &request,
                                    const byte_t *data)
  { 
    if (logAll()) {
      logMessage("receive write data " + requestReport(request, data)); 
    }
  }


  void MemoryLogger::logPreempt(const MemoryRequest &request)
  {
    if (logAll()) {
      logMessage("fetch request " + requestReport(request) +
                 " preempted and removed");
    }
  }

  void MemoryLogger::logReadNextRequest(uint64_t id, addr_t ea, addr_t ra,
                                        unsigned size, unsigned serialized, bool fetch,
                                        const ReqInfo &rinfo)
  {
    if (logAll()) {
      logMessage(string("send ") + (fetch ? "fetch" : "read") + " request " +
                 requestReport(rinfo, id, ea, ra, size, serialized) +
                 (_next.empty() ? "" : " to " + _next));
    }
  }


  void MemoryLogger::logReadNextAction(uint64_t id, addr_t ea, addr_t ra,
                                       unsigned size, unsigned serialized, bool fetch,
                                       const byte_t *data, const ReqInfo &rinfo)
  {
    if (logAll()) {
      logMessage(string("receive ") + (fetch ? "fetch" : "read") + " data " +
                 requestReport(rinfo, id, ea, ra, size, serialized, data) +
                 (_next.empty() ? "" : " from " + _next));
    }
  }


  void MemoryLogger::logWriteNextRequest(uint64_t id, addr_t ea, addr_t ra,
                                         unsigned size, unsigned serialized, const ReqInfo &rinfo)
  {
    if (logAll()) {
      logMessage("send write request " +
                 requestReport(rinfo, id, ea, ra, size, serialized) +
                 (_next.empty() ? "" : " to " + _next));
    }
  }


  void MemoryLogger::logWriteNextAction(uint64_t id, addr_t ea, addr_t ra,
                                        unsigned size, unsigned serialized, const byte_t *data,
                                        const ReqInfo &rinfo)
  {
    if (logAll()) {
      logMessage("send write data " +
                 requestReport(rinfo, id, ea, ra, size, serialized, data) +
                 (_next.empty() ? "" : " to " + _next));
    }
  }

  void MemoryLogger::logCancel(const MemoryRequest &request, bool deep)
  { 
    if (logAll()) {
      logMessage("request " + requestReport(request) + " cancelled " +
                 (deep ? "and removed" : "but not removed"));
    }
  }


  void MemoryLogger::logCancelNext(uint64_t id, addr_t ea, addr_t ra,
                                   unsigned size, const ReqInfo &rinfo)
  { 
    if (logAll()) {
      logMessage("cancel request " + requestReport(rinfo, id, ea, ra, size, 0) +
                 (_next.empty() ? "" : " to " + _next));
    }
  }


  void MemoryLogger::logDiscardRequest(const MemoryRequest &request)
  {
    if (logAll()) {
      logMessage("request " + requestReport(request) + " discarded"); 
    }
  }

  void MemoryLogger::logCacheCommandBegin(const MemoryRequest &request)
  {
    if (logAll()) {
      ostringstream s;
      
      s << "receive " << request.type << " command "
        << requestReport(request);
      
      logMessage(s.str());
    }
  }


  void MemoryLogger::logCacheCommandEnd(const MemoryRequest &request)
  {
    if (logAll()) {
      ostringstream s;
      
      s << "command " << request.type << " " << requestReport(request)
        << " completed";
      
      logMessage(s.str());
    }
  }


  void cachePosition(ostream &o,const MemoryRequest &request)
  {
    o << "(set=" << request.set << ", way=" << request.way << ")";
  }

  void MemoryLogger::logCacheLookup(const MemoryRequest &request)
  {
    if (logAll() && isCache()) {
      ostringstream ss;

      ss << "request " << request.type << " ";
      requestReport(ss,request);
      ss << " lookup: ";
      if (request.isHit()) {
        ss << "hit ";
        cachePosition(ss,request);
      } else {
        ss << "miss, ";
        if (request.needLinefill()) {
          ss << "allocate ";
          cachePosition(ss,request);
        } else {
          ss << "non-allocate";
        }
      
      }
      logMessage(ss.str());
    }
  }

  void MemoryLogger::logRequest(const string &pfx,int time_to_expire,const MemoryRequest &request)
  {
    ostringstream ss;

    ss << pfx << ((request.isRead()) ? "R: " : "W: ");
    if (!request.expired) {
      ss << "expires in " << time_to_expire << "c ";
    }
    ss << request;
    logMessage(ss.str());
  }

  void MemoryLogger::logPortRequest(const string &portname,int index,int time_to_expire,const MemoryRequest &request)
  {
    ostringstream ss;
    ss << "request: " << portname << "[";
    if (index < 0) {
      ss << "w";
    } else {
      ss << index;
    }
    ss << "]:  ";
    logRequest(ss.str(),time_to_expire,request);
  }

}

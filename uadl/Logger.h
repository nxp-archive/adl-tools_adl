//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef _LOGGER_H
#define _LOGGER_H

#include <list>
#include <string>

#include "UadlTypes.h"

namespace adl {
  struct InstrPacketBase;
}

namespace uadl {

  typedef adl::CacheAccess MemRequestType;

  class InstrBase;
  class MemoryRequest;
  class Logger;

  //
  // Optional client class which can be installed into the loggers.  This is
  // called for various events, such as stalls and flushes and can be used to
  // trigger events in applications and models which embed uADL cores.
  //
  struct LogClient {
    virtual ~LogClient() {};

    // Called on a stall.
    virtual void logStall(Logger &,const std::string &msg) = 0;

    // Called on a flush.
    virtual void logFlush(Logger &,const std::string &msg) = 0;
  };

  enum TFlags { LogNone = 0, LogAll = 1 };

  //
  // Base logger class.
  //
  class Logger {
  public:
    typedef std::list<std::string> Msgs;

    Logger(const std::string &name,LogClient *lc,uint32_t tflags) : 
      _name(name), _lc_name(name), _logclient(lc), _tflags(tflags) {}

    Logger(const std::string &name,const std::string &lc_name,LogClient *lc,uint32_t tflags) : 
      _name(name), _lc_name(lc_name), _logclient(lc), _tflags(tflags) {}

    virtual ~Logger() {};

    static void requestReportInner(std::ostream &o, const ReqInfo &rinfo, unsigned id, 
                                   addr_t ea, addr_t ra, unsigned size, unsigned serialized,
                                   const byte_t *data = 0);
    static void requestReport(std::ostream &o, const MemoryRequest &request,const byte_t *data = 0);

    const std::string &name() const { return _name; };
    const std::string &client_name() const { return _lc_name; };

    void setClient(LogClient *lc,uint32_t tflags) { _logclient = lc; _tflags = tflags; };

    bool logAll() const { return _tflags | LogAll; };

    // access/query methods
    bool hasLog() const { return !_log.empty(); }

    virtual bool report(std::ostream &o,const std::string &pfx) const;
    // This both generates a report and then clears the logger, so it's ready
    // for new logging.
    bool report_and_clear(std::ostream &,const std::string &pfx);

    virtual void clear() { _log.clear();  }

    virtual void logMessage(const std::string &msg);
    virtual void logMessage(const char *);

    virtual void logOpAction(const char *action,const std::string &name,int index);
    virtual void logOpAction(const char *action,const std::string &name,int index,uint64_t v);

    virtual void logOpAction(const std::string &action,const std::string &name,int index);
    virtual void logOpAction(const std::string &action,const std::string &name,int index,uint64_t v);

    virtual void logReadAvailFail(const std::string &name,int index,tag_t tag);

    virtual void logBranch(bool taken,addr_t target);

    // Primarily used for instructions and their interaction with memory
    // elements.
    virtual void logReadNextRequest(uint64_t id, adl::addr_t ea, adl::addr_t ra, unsigned size,
                                    unsigned serialized, bool fetch, const ReqInfo &rinfo);
    virtual void logReadNextAction(uint64_t id, adl::addr_t ea, adl::addr_t ra, unsigned size,
                                   unsigned serialized, bool fetch, const adl::byte_t *data, const ReqInfo &rinfo);
    virtual void logWriteNextRequest(uint64_t id, adl::addr_t ea, adl::addr_t ra, unsigned size,
                                     unsigned serialized,const ReqInfo &rinfo);
    virtual void logWriteNextAction(uint64_t id, adl::addr_t ea, adl::addr_t ra, unsigned size,
                                    unsigned serialized, const adl::byte_t *data,const ReqInfo &rinfo);

    virtual void logSendCacheCommand(MemRequestType type, adl::addr_t ra);

    virtual void logStall(const std::string &msg);

    virtual void logFlush(const std::string &msg);

  protected:
    static void requestReport(std::ostream &o, const ReqInfo &rinfo, unsigned id,
                         addr_t ea, addr_t ra, unsigned size, unsigned serialized,
                         const byte_t *data = 0);
    static std::string requestReport(const MemoryRequest &request,const byte_t *data = 0);
    static std::string requestReport(const ReqInfo &rinfo, uint64_t id, addr_t ea,
                                     addr_t ra, unsigned size, unsigned serialized, const byte_t *data = 0);

    std::string getIndent(const std::string &pfx) const;

    std::string  _name;
    std::string  _lc_name;
    Msgs         _log;
    LogClient   *_logclient;
    uint32_t     _tflags;
  };

  //
  // Logger for a memory element in the hierarchy (cache, memory, etc.).
  //
  class MemoryLogger : public Logger {
  public:
    MemoryLogger(const std::string &name, const std::string &next, LogClient *lc,unsigned tflags) :
      Logger(name,lc,tflags), _next(next) {}

    // logging interface
    void logReadRequest(const MemoryRequest &request);
    void logReadAction(const MemoryRequest &request,const adl::byte_t *data);
    void logWriteRequest(const MemoryRequest &request);
    void logWriteAction(const MemoryRequest &request, const adl::byte_t *data);
    void logPreempt(const MemoryRequest &request);

    void logCacheCommandBegin(const MemoryRequest &request);
    void logCacheCommandEnd(const MemoryRequest &request);

    void logCacheLookup(const MemoryRequest &request);

    void logReadNextRequest(uint64_t id, addr_t ea, addr_t ra, unsigned size, unsigned serialized,
                            bool fetch, const ReqInfo &rinfo = ReqInfo()) override;
    void logReadNextAction(uint64_t id, addr_t ea, addr_t ra, unsigned size, unsigned serialized,
                           bool fetch, const byte_t *data, const ReqInfo &rinfo = ReqInfo()) override;
    void logWriteNextRequest(uint64_t id, addr_t ea, addr_t ra, unsigned size, unsigned serialized,
                             const ReqInfo &rinfo = ReqInfo()) override;
    void logWriteNextAction(uint64_t id, addr_t ea, addr_t ra, unsigned size, unsigned serialized,
                            const byte_t *data, const ReqInfo &rinfo = ReqInfo()) override;

    void logCancel(const MemoryRequest &request, bool deep);
    void logCancelNext(uint64_t id, adl::addr_t ea, adl::addr_t ra, unsigned size,
                       const ReqInfo &rinfo = ReqInfo());
    void logDiscardRequest(const MemoryRequest &request);

    // Called when we're logging all requests.  index < 0 means a waiting request.
    void logRequest(const std::string &pfx,int time_to_expire,const MemoryRequest &request);
    void logPortRequest(const std::string &portname,int index,int time_to_expire,const MemoryRequest &request);

  private:
    // query/access methods
    bool isCache() const { return !_next.empty(); }

    // data members
    std::string _next; // name of next-level memory
  };

}

#endif // _LOGGER_H

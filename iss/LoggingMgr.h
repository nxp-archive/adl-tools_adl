//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This file describes the interface for the logging manager.  The logging
// manager is present with standalone executables, or with base libraries if a
// logging library is linked in as well.
//
// The basic purpose of the logging manager is to broadcast logging events to
// all registered clients.
//
// LoggingIface: The interface for registered clients.  Logging data for each
// core is sent to the object of this type that is registered for each core.
// This is equivalent to the LogBase object.
//
// LogParentIface: The user calls registerLogger to register a new logging
// client.  The object provided to registerLogger, however, is a parent object.
// Its register_core() method is called repeatedly for each core encountered.
// The parent object should return a LoggingIface object for each core that is
// encountered.  It is legal for all such returned values to be just a single
// object, or they can all be different objects.
//
// The API:
//
// registerLogger: Call this to register a new set of loggers.  The object
// passed in is the parent object, as described above.
//
// unregisterLogger: Call this to remove an object.  All LoggingIface objects
// associated with the parent object provided to unregisterLogger are removed.
//
// installLogger: This must be called on the root object before any loggers are
// registered, in order to populate the system/core with the broadcasting
// logger.  This is done automatically for a standalone executable, or if the
// "thick" libraries are used. i.e. if SimInterface.h:init is called.
//

#ifndef _H_LOGGINGMGR__
#define _H_LOGGINGMGR__

#include "ModelInterface.h"

namespace rnumber {
  class RNumber;
}

namespace adl {

  struct IssCore;
  struct MmuBase;

  //
  // The LoggingMgr class exports an interface for logging and allows subclasses
  // of the interface to register for logging callbacks.  This corresponds to a LogBase object
  // that's installed on a per-core basis.
  //
  struct LoggingIface {
    virtual ~LoggingIface() {};
    virtual void logInstrPrefetch(addr_t ea) {}
    virtual void logInstrFromBuffer() {}
    virtual void logInstrRead(unsigned id,const char *name,addr_t ea, addr_t ra, uint32_t value) {}
    virtual void logInstr(const uint32_t *opc,int num_bytes,const char* name, Disassembler dis,uint32_t flags) {}
    virtual void logInstrArgs(const uint32_t *opc, int num_bytes, const char *name, Disassembler dis) {}
    virtual void logInstrIssueTime(ttime_t issue_time) {}
    virtual void logInstrCompletionTime(ttime_t completion_time) {}
    virtual void logBasicBlockEnd(addr_t ea) {}
    virtual void logMmuTranslation(TransType tt, int seq, int set, int way, addr_t ea, addr_t ra, const MmuBase* trans) {}
    virtual void logCacheAction(const char *name,CacheAction,CacheAccess,unsigned level,int set,int way,unsigned linemask,addr_t ra) {};
    virtual void logRegExtWrite(unsigned id,const char* name, const rnumber::RNumber& value) {}
    virtual void logRegfileExtWrite(unsigned id,const char* name, uint32_t index, const rnumber::RNumber& value) {}
    virtual void logRegWrite(unsigned id,const char* name, const rnumber::RNumber& value) {}
    virtual void logRegRead(unsigned id,const char* name, const rnumber::RNumber& value) {}
    virtual void logRegfileWrite(unsigned id,const char* name, uint32_t index, const rnumber::RNumber& value) {}
    virtual void logRegfileRead(unsigned id,const char* name, uint32_t index, const rnumber::RNumber& value) {}
    virtual void logRegWriteMask(unsigned id,const char* name, const rnumber::RNumber& value, const rnumber::RNumber& mask) {}
    virtual void logRegReadMask(unsigned id,const char* name, const rnumber::RNumber& value, const rnumber::RNumber& mask) {}
    virtual void logRegfileWriteMask(unsigned id,const char* name, uint32_t index, const rnumber::RNumber& value, const rnumber::RNumber& mask) {}
    virtual void logRegfileReadMask(unsigned id,const char* name, uint32_t index, const rnumber::RNumber& value, const rnumber::RNumber& mask) {}
    virtual void logCoreMemWrite(unsigned id,const char *name,addr_t ea, int num_bytes) {}
    virtual void logCoreMemRead(unsigned id,const char *name,addr_t ea, int num_bytes) {}
    virtual void logCoreMemWriteTyped(unsigned id,const char *name,addr_t ea, int num_bytes,CacheAccess type) {}
    virtual void logCoreMemReadTyped(unsigned id,const char *name,addr_t ea, int num_bytes,CacheAccess type) {}
    virtual void logMemWrite(unsigned id,const char *name,bool pre,int seq,addr_t ea, addr_t ra, uint32_t value) {}
    virtual void logMemRead(unsigned id,const char *name,bool pre,int seq,addr_t ea, addr_t ra, uint32_t value) {}
    virtual void logBreakpoint(addr_t ea,unsigned handle,PtBase *payload) {}
    virtual void logWatchpoint(addr_t ea,bool pre,WatchType,int num_bytes,uint64_t value,unsigned handle,PtBase *payload) {}
    virtual void logException(unsigned handle,bool pre,const char *name) {}
    virtual void logAnnotation(MsgType type,unsigned level,const std::string &msg,unsigned ndata,LogBase::Data data[]) {}
    virtual void logBranchTaken(addr_t ea) {};
    virtual void logBranchEnd(addr_t ea) {};
    virtual void logDependencies(const DepItems &) {};
    virtual void logCoreChange() {};
    virtual void logCtxChange(unsigned id,const char *name,unsigned context_num,CtxType update) {};
    virtual void logCommit(uint64_t) {};
  };

  //
  // Derive from this class in order to install LoggingIface objects into all
  // cores in a system.
  //
  struct LogParentIface {

    virtual ~LogParentIface() {};

    // Called for each core in a system.  This should return a new LoggingIface
    // object, which will be installed as a logger object for that core.
    virtual LoggingIface *register_core(IssCore &core) = 0;

  };

  //
  // Standard object used for broadcasting logging data to registered LoggingIface
  // objects.
  //
  struct LoggingObjBase : public LogBase {
    virtual void registerLogger(LogParentIface *parent,LoggingIface* iface) = 0;
    virtual void unregisterLogger(LogParentIface *parent) = 0;

    // Only used by multi-threaded loggers.  Returns true if any items
    // processed.
    virtual bool handleLogging() { return false; };
    virtual uint64_t handleMergedLogging() { return 0; };
    virtual void commitMergedLogging() {};
  };

  // Interface object for installing loggers.  Call createLoggingMgr() to create
  // an object, then call these routines to register individual loggers.
  struct LoggingMgr {
    virtual ~LoggingMgr() {};

    virtual void installLoggers(IssNode *n) = 0;
    virtual void registerLogger(LogParentIface *p) = 0;
    virtual void unregisterLogger(LogParentIface* p) = 0;

    // Lets the logging manager know that logging is about to start.
    virtual void startLogging() {};

    // Lets the logging manager know that logging has ended.
    virtual void doneLogging() {};

  protected:
    virtual bool use_mt() const = 0;
  };

  typedef std::vector<std::pair<LogParentIface *,LoggingIface*> > LoggerClients;

  //
  // The interface for dealing with loggers.
  //

  //
  // Interface functions with a user-supplied logging manager object.
  //

  // Create a logging manager object.
  LoggingMgr *createLoggingMgr(bool use_mt);

}

#endif


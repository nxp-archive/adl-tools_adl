//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef _UADL_MEMORY_H
#define _UADL_MEMORY_H

#include <deque>
#include <vector>
#include <string>
#include <iostream>
#include <assert.h>

#include "helpers/BasicTypes.h"

#include "Timer.h"
#include "Logger.h"
#include "UadlMemoryIf.h"
#include "MemoryRequest.h"
#include "BandwidthControl.h"


namespace uadl {

  using std::deque;
  using std::string;
  using adl::byte_t;
  using adl::addr_t;
  using adl::CacheAction;

  class UadlArch;

  /*******************************************************************************
   * memory interface delegate
   *
   * NOTE:
   *   - This class delegates accesses to an external memory so that the interface
   *     is consistent with an internal memory.
   *   - The parent object is responsible for properly setting the external memory
   *     handler, so no valid() checking is done in referencing _mem.
   ******************************************************************************/
  class MemoryDelegate : public UadlMemoryIf {
  public:
    MemoryDelegate(UadlMemoryIf *mem = 0) : UadlMemoryIf(""), _mem(mem), _level(0) {}
    virtual ~MemoryDelegate() {}

    bool valid() const { return _mem; }
    void setMemory(UadlMemoryIf &mem) { _mem = &mem; }
    const UadlMemoryIf *getMemory() const { return _mem; };

    virtual void reset() { _mem->reset(); }

    virtual void setMemParms(unsigned addrBandwidth, unsigned dataBandwidth, unsigned readLatency,
                             unsigned writeLatency, unsigned queueSize, bool allowBusWait, bool preemptive,
                             bool readOnly)
    {
      assert(_mem);
      _mem->setMemParms(addrBandwidth,dataBandwidth,readLatency,writeLatency,queueSize,
                        allowBusWait,preemptive,readOnly);
    }

    virtual MemoryPort *addPort(const string &name, bool commonBandwidth,
                                unsigned addrBandwidth, unsigned dataBandwidth,
                                unsigned readLatency, unsigned writeLatency,
                                unsigned maxRequest,bool allowBusWait)
    {
      assert(_mem);
      return _mem->addPort(name,commonBandwidth,addrBandwidth,dataBandwidth,readLatency,
                           writeLatency,maxRequest,allowBusWait);
    }

    unsigned level() const { return _level; };
    unsigned level_mask() const { return 1 << _level; };
    virtual unsigned get_level_mask() const { return level_mask(); };
    virtual void set_level(unsigned l) { _level = l; };

    virtual bool getMemoryData(adl::CacheAccess type,addr_t addr, unsigned size,
                               byte_t *data);
    virtual bool setMemoryData(addr_t addr, unsigned size, const byte_t *data);

    virtual bool fetchMemoryData(addr_t addr, unsigned size, byte_t *data,
                                 uint64_t &exception, bool cacheEnabled = false);
    virtual bool readMemoryData(addr_t addr, unsigned size, byte_t *data,
                                uint64_t &exception, bool cacheEnabled = false);
    virtual bool writeMemoryData(addr_t addr, unsigned size, const byte_t *data,
                                 uint64_t &exception, bool cacheEnabled = false);

    virtual bool emptyInquire(Logger *logger) const;

    virtual bool inactiveInquire(Logger *logger) const;

    virtual bool serializedInquire(Logger *logger, unsigned serialized, unsigned portId) const;

    virtual bool sendRequestInquire(InstrBase *instr,Logger *logger, MemRequestType type,
                                    unsigned serialized,unsigned portId = 0) const;
    virtual unsigned sendFetchRequest(addr_t ea, addr_t ra, unsigned size,
                                      unsigned serialized,uint64_t &exception, const CacheStatusHint *hint,
                                      const ReqInfo &rinfo,unsigned portId = 0);
    virtual unsigned sendReadRequest(addr_t ea, addr_t ra, unsigned size,
                                     unsigned serialized,uint64_t &exception, const ReqInfo &rinfo,
                                     unsigned portId = 0);
    virtual unsigned sendWriteRequest(addr_t ea, addr_t ra, unsigned size,
                                      unsigned serialized,uint64_t &exception, const byte_t *data = NULL,
                                      bool writeThrough = false, const ReqInfo &rinfo = ReqInfo(),
                                      unsigned portId = 0, bool isEvict = false);

    virtual unsigned sendReadRequest(addr_t ea, addr_t ra, unsigned size,
                                     unsigned serialized,uint64_t &exception,
                                     const CacheStatusHint *hint,
                                     const ReqInfo &rinfo, unsigned portId = 0);
    virtual unsigned sendWriteRequest(addr_t ea, addr_t ra, unsigned size,
                                      unsigned serialized,uint64_t &exception,
                                      const CacheStatusHint *hint,
                                      const byte_t *data = NULL,
                                      bool writeThrough = false, const ReqInfo &rinfo = ReqInfo(),
                                      unsigned portId = 0, bool isEvict = false);

    virtual bool readInquire(InstrBase *instr,Logger *logger, unsigned portId, unsigned requestId) const;
    virtual void read(unsigned portId,unsigned requestId, byte_t *data);

    virtual bool writeInquire(InstrBase *instr,Logger *logger, unsigned portId, unsigned requestId) const;
    virtual void write(unsigned portId,unsigned requestId, const byte_t *data);

    virtual void setRequestSize(unsigned portId,unsigned requestId,unsigned dataWidth);

    virtual bool lockInquire(InstrBase *instr,Logger *logger) const;
    virtual bool forceLockInquire(InstrBase *instr,Logger *logger) const;
    virtual void lockAction(InstrBase *instr) { _mem->lockAction(instr); }
    virtual bool unlockInquire(InstrBase *instr,Logger *logger) const;
    virtual void unlockAction() { _mem->unlockAction(); }
    virtual void unlockAction(unsigned delay) { _mem->unlockAction(delay); }

    virtual void cancel(unsigned portId, unsigned requestId, bool deep);
    virtual void cancel(unsigned requestId, bool deep);

    // Note: These are normally not called by an external-memory model unless
    // the --extern-cycle-mem option is set at generation time.
    virtual void preCycle();
    virtual void postCycle();
    virtual void logAllRequests() const;

    virtual bool isActive() const { return _mem->isActive(); }

    virtual unsigned nAvailableRequestSlots() const;
    virtual unsigned nAvailableRequestSlots(unsigned portId) const;

  private:
    UadlMemoryIf *_mem;
    unsigned      _level;
  };


  // inline methods
  inline bool MemoryDelegate::getMemoryData(adl::CacheAccess type,addr_t addr,
                                            unsigned size, byte_t *data)
  { 
    return _mem->getMemoryData(type, addr, size, data); 
  }

  inline bool MemoryDelegate::setMemoryData(addr_t addr, unsigned size,
                                            const byte_t *data)
  { 
    return _mem->setMemoryData(addr, size, data); 
  }

  inline bool MemoryDelegate::fetchMemoryData(addr_t addr, unsigned size,
                                              byte_t *data, uint64_t &exception,
                                              bool cacheEnabled)
  { 
    return _mem->fetchMemoryData(addr, size, data, exception, cacheEnabled); 
  }

  inline bool MemoryDelegate::readMemoryData(addr_t addr, unsigned size,
                                             byte_t *data, uint64_t &exception,
                                             bool cacheEnabled)
  { 
    return _mem->readMemoryData(addr, size, data, exception, cacheEnabled); 
  }

  inline bool MemoryDelegate::writeMemoryData(addr_t addr, unsigned size,
                                              const byte_t *data,
                                              uint64_t &exception,
                                              bool cacheEnabled)
  { 
    return _mem->writeMemoryData(addr, size, data, exception, cacheEnabled); 
  }

  inline bool MemoryDelegate::emptyInquire(Logger *logger) const
  { 
    return _mem->emptyInquire(logger); 
  }

  inline bool MemoryDelegate::inactiveInquire(Logger *logger) const
  { 
    return _mem->inactiveInquire(logger); 
  }

  inline bool MemoryDelegate::serializedInquire(Logger *logger, unsigned serialized, unsigned portId) const
  { 
    return _mem->serializedInquire(logger, serialized, portId); 
  }

  inline bool MemoryDelegate::sendRequestInquire(InstrBase *instr,Logger *logger,
                                                 MemRequestType type,unsigned serialized,
                                                 unsigned portId) const
  { 
    return _mem->sendRequestInquire(instr, logger, type, serialized, portId); 
  }


  inline unsigned MemoryDelegate::sendFetchRequest(addr_t ea, addr_t ra, unsigned size,
                                                   unsigned serialized,uint64_t &exception, const CacheStatusHint *hint, 
                                                   const ReqInfo &rinfo,unsigned portId)
  { 
    return _mem->sendFetchRequest(ea, ra, size, serialized, exception, hint, rinfo, portId); 
  }

  inline unsigned MemoryDelegate::sendReadRequest(addr_t ea, addr_t ra, unsigned size,
                                                  unsigned serialized,uint64_t &exception,
                                                  const ReqInfo &rinfo, unsigned portId)
  { 
    return _mem->sendReadRequest(ea, ra, size, serialized, exception, rinfo, portId); 
  }

  inline unsigned MemoryDelegate::sendWriteRequest(addr_t ea, addr_t ra, unsigned size,
                                                   unsigned serialized,uint64_t &exception,
                                                   const byte_t *data,
                                                   bool writeThrough,
                                                   const ReqInfo &rinfo, unsigned portId,
                                                   bool isEvict)
  {
    return _mem->sendWriteRequest(ea, ra, size, serialized, exception, data, writeThrough,
                                  rinfo, portId, isEvict);
  }


  inline unsigned MemoryDelegate::sendReadRequest(addr_t ea, addr_t ra, unsigned size,
                                                  unsigned serialized,uint64_t &exception,
                                                  const CacheStatusHint *hint,
                                                  const ReqInfo &rinfo, unsigned portId)
  { 
    return _mem->sendReadRequest(ea, ra, size, serialized, exception, hint, rinfo, portId); 
  }

  inline unsigned MemoryDelegate::sendWriteRequest(addr_t ea, addr_t ra, unsigned size,
                                                   unsigned serialized,uint64_t &exception,
                                                   const CacheStatusHint *hint,
                                                   const byte_t *data,
                                                   bool writeThrough,
                                                   const ReqInfo &rinfo, unsigned portId,
                                                   bool isEvict)
  {
    return _mem->sendWriteRequest(ea, ra, size, serialized, exception, hint, data,
                                  writeThrough, rinfo, portId, isEvict);
  }


  inline bool MemoryDelegate::readInquire(InstrBase *instr,Logger *logger, unsigned portId,unsigned requestId) const
  { 
    return _mem->readInquire(instr,logger, portId, requestId); 
  }

  inline void MemoryDelegate::read(unsigned portId, unsigned requestId, byte_t *data)
  { 
    _mem->read(portId, requestId, data); 
  }

  inline bool MemoryDelegate::writeInquire(InstrBase *instr, Logger *logger, unsigned portId, unsigned requestId) const
  { 
    return _mem->writeInquire(instr, logger, portId, requestId); 
  }

  inline void MemoryDelegate::write(unsigned portId, unsigned requestId, const byte_t *data)
  { 
    _mem->write(portId, requestId, data); 
  }

  inline void MemoryDelegate::setRequestSize(unsigned portId,unsigned requestId,unsigned dataWidth)
  {  
    _mem->setRequestSize(portId,requestId,dataWidth); 
  }

  inline bool MemoryDelegate::lockInquire(InstrBase *instr,Logger *logger) const
  { 
    return _mem->lockInquire(logger); 
  }

  inline bool MemoryDelegate::forceLockInquire(InstrBase *instr,Logger *logger) const
  { 
    return _mem->lockInquire(logger); 
  }

  inline bool MemoryDelegate::unlockInquire(InstrBase *instr,Logger *logger) const
  { 
    return _mem->unlockInquire(logger); 
  }

  inline void MemoryDelegate::cancel(unsigned portId, unsigned requestId, bool deep)
  { 
    _mem->cancel(portId, requestId, deep); 
  }

  inline void MemoryDelegate::cancel(unsigned requestId, bool deep)
  { 
    _mem->cancel(requestId, deep); 
  }

  inline void MemoryDelegate::preCycle()
  {
    _mem->preCycle();
  }

  inline void MemoryDelegate::postCycle()
  {
    _mem->postCycle();
  }

  inline void MemoryDelegate::logAllRequests() const
  {
    _mem->logAllRequests();
  }

  inline unsigned MemoryDelegate::nAvailableRequestSlots() const
  {
    return _mem->nAvailableRequestSlots();
  }

  inline unsigned MemoryDelegate::nAvailableRequestSlots(unsigned portId) const
  {
    return _mem->nAvailableRequestSlots(portId);
  }


  /*******************************************************************************
   * memory token manager
   *
   * NOTE:
   *   - see TransactionCodeGen.C::MemoryCodeGen::classDef() for default
   *     constructor parameters
   *   - assume requests are processed in first-come first-serve order
   *   - If a transaction has both an OSM and request id as arguments, the memory
   *     model always assumes that the OSM does own that request.
   *
   * TODO:
   *   - check parameters (in code generation): positive latency, etc.
   ******************************************************************************/
  class Memory : public UadlMemoryIf, public Resource {
  public:
    Memory(const Timer &timer,const string &name, unsigned addrBandwidth,
           unsigned dataBandwidth, unsigned readLatency, unsigned writeLatency,
           unsigned queueSize, bool allowBusWait, bool preemptive, bool readOnly,
           bool withData = true) :
      UadlMemoryIf(name), Resource(timer), _withData(withData),
      _logger(NULL), _level(0), _requestId(1),
      _queueWasFull(false), _queueWasActive(false),
      _preemptive(allowBusWait && preemptive), _hasPreemptionId(false),
      _locked(false), _unlock_delay(-1)
    { 
      setMemParms(addrBandwidth,dataBandwidth,readLatency,writeLatency,queueSize,
                  allowBusWait,preemptive,readOnly);
      addPort(MemoryPort::defaultPort(), false, addrBandwidth, dataBandwidth, 0, 0, queueSize, allowBusWait); 
    }

    virtual void setMemParms(unsigned addrBandwidth, unsigned dataBandwidth, unsigned readLatency,
                             unsigned writeLatency, unsigned queueSize, bool allowBusWait, bool preemptive,
                             bool readOnly)
    {
      _addrBandwidth.setBandwidth(addrBandwidth);
      _dataBandwidth.setBandwidth(dataBandwidth);
      _readLatency = readLatency;
      _writeLatency = writeLatency;
      _queueSize = queueSize;
      _preemptive = (allowBusWait && preemptive);
      _readOnly = readOnly;
    }

    // TODO: should reach this state automatically
    virtual void reset();

    virtual unsigned memStalls(unsigned portId) const { return getPort(portId).memStalls(); };

    // Untimed debug access interface
    // TODO: should be const
    virtual bool getMemoryData(adl::CacheAccess type,addr_t addr, unsigned size,
                               byte_t *data);
    virtual bool setMemoryData(addr_t addr, unsigned size, const byte_t *data);

    // Note: For this interface, we use the default implementation of the untimed
    // execution access functions, which just calls the debug routines.

    // Transaction interface
    virtual bool emptyInquire(Logger *logger) const;

    virtual bool inactiveInquire(Logger *logger) const;

    virtual bool serializedInquire(Logger *logger, unsigned serialized, unsigned portId) const;

    virtual bool sendRequestInquire(InstrBase *instr,Logger *logger, MemRequestType type, 
                                    unsigned serialized, unsigned portId = 0)
      const;
    virtual unsigned sendFetchRequest(addr_t ea, addr_t ra, unsigned size,
                                      unsigned serialized, uint64_t &exception, const CacheStatusHint *hint, 
                                      const ReqInfo &rinfo, unsigned portId = 0);
    virtual unsigned sendReadRequest(addr_t ea, addr_t ra, unsigned size,
                                     unsigned serialized, uint64_t &exception, const ReqInfo &rinfo,
                                     unsigned portId = 0);
    virtual unsigned sendWriteRequest(addr_t ea, addr_t ra, unsigned size,
                                      unsigned serialized, uint64_t &exception, const byte_t *data = NULL,
                                      bool writeThrough = false, const ReqInfo &rinfo = ReqInfo(),
                                      unsigned portId = 0, bool isEvict = false);

    // This just allows a memory to be treated by a cache by redirecting these
    // calls to the normal calls that are used.
    virtual unsigned sendReadRequest(addr_t ea, addr_t ra, unsigned size,
                                     unsigned serialized, uint64_t &exception,
                                     const CacheStatusHint *hint,
                                     const ReqInfo &rinfo, unsigned portId = 0);
    virtual unsigned sendWriteRequest(addr_t ea, addr_t ra, unsigned size,
                                      unsigned serialized, uint64_t &exception,
                                      const CacheStatusHint *hint,
                                      const byte_t *data = NULL,
                                      bool writeThrough = false, const ReqInfo &rinfo = ReqInfo(),
                                      unsigned portId = 0, bool isEvict = false);

    virtual bool readInquire(InstrBase *instr,Logger *logger, unsigned portId, unsigned requestId) const;
    virtual void read(unsigned portId, unsigned requestId, byte_t *data);
    virtual bool writeInquire(InstrBase *instr,Logger *logger, unsigned portId, unsigned requestId) const;
    virtual void write(unsigned portId, unsigned requestId, const byte_t *data);

    void setRequestSize(unsigned portId,unsigned requestId,unsigned dataWidth);

    virtual bool lockInquire(InstrBase *instr,Logger *logger) const;
    virtual bool forceLockInquire(InstrBase *instr,Logger *logger) const;
    virtual void lockAction(InstrBase *instr);
    virtual bool unlockInquire(InstrBase *instr,Logger *logger) const;
    virtual void unlockAction() { _locked = false; }
    virtual void unlockAction(unsigned delay);

    virtual bool requestDoneInquire(Logger *logger, unsigned portId, unsigned requestId) const;

    // non-transaction interface
    virtual void cancel(unsigned portId, unsigned requestId, bool deep);
    virtual void cancel(unsigned requestId, bool deep);

    // per time step methods
    virtual void preCycle();
    virtual void postCycle() {}

    // Write logging information about all requests in all ports and any waiting requests.
    virtual void logAllRequests() const;
    
    // any cleanup to perform at end of simulation.
    virtual void afterSim() {};

    // access/query methods
    // TODO: fix isActive() implementation
    virtual bool isActive() const;
    const MemoryRequest *requestInfo(unsigned portId,unsigned requestId) const;
    virtual unsigned nAvailableRequestSlots() const;
    virtual unsigned nAvailableRequestSlots(unsigned portId) const;

    bool emptyRequests(unsigned portId) const;
    bool activePort(unsigned portId) const;

    unsigned level() const { return _level; };
    unsigned level_mask() const { return 1 << _level; };
    virtual unsigned get_level_mask() const { return level_mask(); };
    virtual void set_level(unsigned l) { _level = l; };

    // logging methods
    bool logEnabled() const { return _logger; }
    virtual void createLogger(LogClient *lc,unsigned tflags);
    virtual void removeLogger();
    bool report_and_clear(std::ostream &o,const std::string &pfx);

    virtual MemoryPort *addPort(const string &name, bool commonBandwidth,
                                unsigned addrBandwidth, unsigned dataBandwidth,
                                unsigned readLatency, unsigned writeLatency,
                                unsigned maxRequest,bool allowBusWait);

  protected:
    bool hasAddrBandwidth(unsigned portId) const;
    bool hasDataBandwidth(unsigned portId) const;
    bool consumeAddrBandwidth(unsigned portId);
    bool consumeDataBandwidth(unsigned portId);

    bool requestInquire(InstrBase *instr,Logger *logger, unsigned portId, unsigned requestId) const;

    bool cancel_internal(MemoryPort &port,unsigned requestId,bool deep);
    void check_waiting_for_cancel(unsigned requestId,bool deep);

    MemoryRequest &sendRequest(MemRequestType type, addr_t ea, addr_t ra,
                                     unsigned size, unsigned serialized, const CacheStatusHint *hint,
                                     bool writeThrough = false,
                                     const ReqInfo &rinfo = ReqInfo(), int portId = 0,
                                     bool isEvict = false);
    bool memLockInquire(InstrBase *instr,Logger *logger) const;

    // Note: This only removes the front request, since requestInquire already
    // verified that the item was at the front.
    void removeRequest(MemoryPort &port);
    void addWaitingRequest(MemoryPort &port);
    virtual void preemptFetch() {}
    virtual void cancelNextLevelRequest(const MemoryRequest &request) {}
    virtual void postCancelRequest(MemoryPort &port,unsigned requestId) {};
    void saveQueueStatus();

    void checkDelayedUnlock();

    byte_t *hasData(addr_t addr) const;
    virtual byte_t *hasData(addr_t addr, unsigned &size) const { return NULL; }

    virtual unsigned readLatency(MemoryRequest &request);
    virtual unsigned writeLatency(MemoryRequest &request);

    MemoryRequest *getWaitingRequest(unsigned requestId);
    const MemoryRequest *getWaitingRequest(unsigned requestId) const { 
      return const_cast<Memory*>(this)->getWaitingRequest(requestId); 
    };
    std::pair<MemoryRequest *,MemoryPort*> getWaitingRequestAndPort(unsigned requestId);

    // data members
    BandwidthControl _addrBandwidth; // address bandwidth
    BandwidthControl _dataBandwidth; // data bandwidth

    // NOTE: only use addRequest() to add requests to _requests
    std::vector<MemoryPort>   _ports;     // ports

    unsigned _readLatency;        // read latency (in cycle)
    unsigned _writeLatency;       // write latency (in cycle)
    bool _readOnly;
    const bool _withData;            // if the memory operates data

    MemoryLogger *_logger;

    uint64_t _exception_flag;             // exception flag place holder (not used)

    unsigned _level;            // Level of this memory/cache in the hierarchy.

  protected:
    const MemoryPort &getPort(unsigned portId) const { return _ports[portId]; };
    MemoryPort &getPort(unsigned portId) { return _ports[portId]; };
    const MemoryPort &getPort(const MemoryRequest &r) const { return _ports[r.portId]; };
    MemoryPort &getPort(const MemoryRequest &r) { return _ports[r.portId]; };

  private:

    void logRequestInternal(const std::string &portname,unsigned index,const MemoryRequest &request,bool &first) const;

    // This adds the request to the port specified by the request.
    void addRequest(const MemoryRequest &request);

    // This allocates a new request; it then needs to be setup by the caller.
    MemoryRequest *addRequest(unsigned portId);

    // TODO: revisit logic, should be + only
    void updateQueueStatus() { 
      _queueWasFull = !nAvailableRequestSlots(); 
      _queueWasActive = isActive();
    }
    void updatePortStatus();
    const MemoryRequest *canPreempt(MemRequestType type,unsigned portId) const;
    // MemoryRequest *canPreempt(MemRequestType type);

    unsigned _queueSize;  // request queue size
    uint64_t _requestId;     // request id counter
    bool _queueWasFull;      // True if, at the start of the cycle, the queue was full.
    bool _queueWasActive;    // True if, at the start of the cycle, there were any requests.

    bool _preemptive;          // can load/store preempt bus-waiting fetch?
    bool _hasPreemptionId;
    uint64_t _preemptionId;

    bool _locked;
    int _unlock_delay;          // Used for implementing delayed unlock.
    uint64_t _locker;
  };


  //
  // Templates wrapper for memories.  This is used directly by the actual model and
  // uses a template parameter for the model/ISS.
  //
  template <class MemType,class ModelType,class InstrType,bool safeMode,unsigned portId = 0>
  class MemoryT {
  public:
    MemoryT (MemType &mem) : _mem(mem) {};

    // Inactive inquire.
    bool is_inactive(UadlArch &uarch,InstrType &instr,Logger *logger) {
      return _mem.inactiveInquire(logger);
    }

    // Read request inquire.
    bool can_request_read(UadlArch &uarch,InstrType &instr,Logger *logger,bool serialized = false) {
      return _mem.sendRequestInquire(&instr,logger,adl::CacheRead,serialized,portId);
    }

    // Write request inquire.
    bool can_request_write(UadlArch &uarch,InstrType &instr,Logger *logger,bool serialized = false) {
      return _mem.sendRequestInquire(&instr,logger,adl::CacheWrite,serialized,portId);
    }

    // Can accept an additional read request.
    bool can_accept_read_requests(UadlArch &uarch,InstrType &instr,Logger *logger) {
      if (!_mem.nAvailableRequestSlots()) {
        if (logger) logger->logStall(_mem.name() + ": request queue full.");
        return false;
      }
      return true;
    }

    // Can accept an additional write request.
    bool can_accept_write_requests(UadlArch &uarch,InstrType &instr,Logger *logger) {
      if (!_mem.nAvailableRequestSlots()) {
        if (logger) logger->logStall(_mem.name() + ": request queue full.");
        return false;
      }
      return true;
    }

    // Read inquire.
    bool can_read(UadlArch &uarch,InstrType &instr,Logger *logger) {   
      if (!instr.mem()) return true;
      return _mem.readInquire(&instr,logger,portId,instr.mem()->request_id());
    }

    // Write inquire.
    bool can_write(UadlArch &uarch,InstrType &instr,Logger *logger) {   
      if (!instr.mem()) return true;
      return _mem.writeInquire(&instr,logger,portId,instr.mem()->request_id());
    }

    void send_read_request(ModelType &uarch,InstrType &instr,Logger *logger) {
      if (!instr.rq_mem()) return;
      send_read_request_base(uarch,instr,logger,*instr.rq_mem());
      instr.next_req_mem();
    };

    void send_read_request(ModelType &uarch,InstrType &instr,Logger *logger, unsigned nb) {
      if (!instr.rq_mem()) return;
      send_read_request_base(uarch,instr,logger,nb,*instr.rq_mem());
      instr.next_req_mem();
    };

    // Called before the ISS exec() which actually does the read.
    void read(ModelType &uarch,InstrType &instr,Logger *logger) {
      if (!instr.mem()) return;
      UadlMemAccess &ma = *(instr.mem());
      _mem.read(portId,ma.request_id(),(safeMode) ? 0 : ma.data()+ma.offset());
      ma.set_valid_data();
      uarch.set_memory_result(instr);
      if (logger && uarch.logMemory()) {
        logger->logReadNextAction(ma.request_id(), ma._ea, ma._ra, ma._nb, ma._serialized, false, ma.data()+ma.offset(), instr.rinfo() );
      }
    };

    // Call this between multiple execs that consume memory, if a single read
    // request consumes more than what one exec() uses.
    bool next(ModelType &uarch,InstrType &instr,Logger *logger) {
      if (safeMode) {
        // Safe mode- all transactions have been created apriori, so use them.
        if (instr.multiple_mem()) {
          unsigned id = instr.mem()->request_id();
          uarch.free_mem(instr);
          instr.mem()->set_request_id(id);
          return true;
        }
        return false;
      } else {
        // Not safe-mode:  A single transaction will be used multiple times.
        UadlMemAccess &ma = *(instr.mem());
        ma._offset += ma._size;
        uarch.set_memory_result(instr);
        return true;
      }
    }
    
    // *Must* be called after the exec() which follows the memory read.
    void post_read(ModelType &uarch,InstrType &instr,Logger *logger) {
      uarch.free_mem(instr);
    }

    void send_write_request(ModelType &uarch,InstrType &instr,Logger *logger) {
      if (!instr.rq_mem()) return;
      send_write_request_base(uarch,instr,logger,*instr.rq_mem());
      instr.next_req_mem();
    };

    void send_write_request(ModelType &uarch,InstrType &instr,Logger *logger,unsigned nb) {
      if (!instr.rq_mem()) return;
      send_write_request_base(uarch,instr,logger,nb,*instr.rq_mem());
      instr.next_req_mem();
    };

    // *Must* be called before the exec() which precedes a write so that the ISS
    // will be able to access the data.
    void pre_write(ModelType &uarch,InstrType &instr,Logger *logger) {
      if (!instr.mem()) return;
      uarch.set_memory_result(instr);
      instr.mem()->set_valid_data();
    }

    // Performs the actual write.  This is called after the ISS exec() which
    // does the write.
    void write(ModelType &uarch,InstrType &instr,Logger *logger) {
      if (!instr.mem()) return;
      UadlMemAccess &ma = *(instr.mem());
      _mem.write(portId,ma.request_id(),(safeMode) ? 0 : ma.data());
      if (logger && uarch.logMemory()) {
        logger->logWriteNextAction(ma.request_id(), ma._ea, ma._ra, ma._nb, ma._serialized, ma.data(), instr.rinfo() );
      }
      uarch.free_mem(instr);
    };

    // Override the number of bytes to write for the current transaction.
    void set_size(ModelType &uarch,InstrType &instr,Logger *logger,unsigned nb) {    
      UadlMemAccess &ma = *(instr.mem());
      ma._nb = nb;
      _mem.setRequestSize(portId,ma.request_id(),nb);
    }

    bool request_queue_empty(UadlArch &uarch,InstrType &instr,Logger *logger) {
      return _mem.emptyInquire(logger);
    }

    bool request_queue_inactive(UadlArch &uarch,InstrType &instr,Logger *logger) {
      return _mem.inactiveInquire(logger);
    }

    // Returns true if a force-lock operation can be performed.
    bool can_force_lock(ModelType &uarch,InstrType &instr,Logger *logger) {
      return _mem.forceLockInquire(&instr,logger);
    }

    // Perform a force-lock operation.
    void allocate_force_lock(ModelType &uarch,InstrType &instr,Logger *logger) {
      _mem.lockAction(&instr);
    }

    // Returns true if a lock operation can be performed.
    bool can_lock(ModelType &uarch,InstrType &instr,Logger *logger) {
      return _mem.lockInquire(&instr,logger);
    }

    // Perform a lock operation.
    void allocate_lock(ModelType &uarch,InstrType &instr,Logger *logger) {
      _mem.lockAction(&instr);
    }

    // Returns true if an unlock operation can be performed.  Not required if
    // the instruction was the locker.
    bool can_unlock(ModelType &uarch,InstrType &instr,Logger *logger) {
      return _mem.unlockInquire(&instr,logger);
    }

    // Perform an unlock operation.
    void deallocate_lock(ModelType &uarch,InstrType &instr,Logger *logger) {
      _mem.unlockAction();
    }

    // Same as above- just for completeness.
    void deallocate_force_lock(ModelType &uarch,InstrType &instr,Logger *logger) {
      _mem.unlockAction();
    }

    // Perform an unlock operation which will occur after a specified delay.
    void deallocate_lock(ModelType &uarch,InstrType &instr,Logger *logger,unsigned delay) {
      _mem.unlockAction(delay);
    }    

    // Same as above- just for completeness.
    void deallocate_force_lock(ModelType &uarch,InstrType &instr,Logger *logger,unsigned delay) {
      _mem.unlockAction(delay);
    }

  protected:
    void send_read_request_base(ModelType &uarch,InstrType &instr,Logger *logger,UadlMemAccess &ma) {
      uint64_t tmp;
      ma.set_request_id( _mem.sendReadRequest( ma._ea, ma._ra, ma._nb, ma._serialized, tmp, ma.hint(), instr.rinfo(), portId ) );
      if (logger && uarch.logMemory()) {
        logger->logReadNextRequest(ma.request_id(), ma._ea, ma._ra, ma._nb, ma._serialized, false, instr.rinfo() );
      }
    };

    void send_read_request_base(ModelType &uarch,InstrType &instr,Logger *logger, unsigned nb,UadlMemAccess &ma) {
      ma._nb = nb;
      uint64_t tmp;
      ma.set_request_id( _mem.sendReadRequest( ma._ea, ma._ra, ma._nb, ma._serialized, tmp, ma.hint(), instr.rinfo(), portId ) );
      if (logger && uarch.logMemory()) {
        logger->logReadNextRequest(ma.request_id(), ma._ea, ma._ra, ma._nb, ma._serialized, false, instr.rinfo() );
      }
    };

    void send_write_request_base(ModelType &uarch,InstrType &instr,Logger *logger,UadlMemAccess &ma) {
      uint64_t tmp;
      ma.set_request_id( _mem.sendWriteRequest( ma._ea, ma._ra, ma._nb, ma._serialized, tmp, ma.hint(), 
                                                          (const byte_t *)0, false, instr.rinfo(), portId, false ) );
      if (logger && uarch.logMemory()) {
        logger->logWriteNextRequest(ma.request_id(), ma._ea, ma._ra, ma._nb, ma._serialized, instr.rinfo() );
      }
    };

    void send_write_request_base(ModelType &uarch,InstrType &instr,Logger *logger,unsigned nb,UadlMemAccess &ma) {
      ma._nb = nb;
      uint64_t tmp;
      ma.set_request_id( _mem.sendWriteRequest( ma._ea, ma._ra, ma._nb, ma._serialized, tmp, ma.hint(), 
                                                          (const byte_t *)0, false, instr.rinfo(), portId, false ) );
      if (logger && uarch.logMemory()) {
        logger->logWriteNextRequest(ma.request_id(), ma._ea, ma._ra, ma._nb, ma._serialized, instr.rinfo() );
      }
    };

    MemType &_mem;
  };


  inline unsigned Memory::sendReadRequest(addr_t ea, addr_t ra, unsigned size,
                                          unsigned serialized, uint64_t &exception,
                                          const CacheStatusHint *hint,
                                          const ReqInfo &rinfo, unsigned portId)
  { return sendReadRequest(ea, ra, size, serialized, exception, rinfo, portId); }


  inline unsigned Memory::sendWriteRequest(addr_t ea, addr_t ra, unsigned size,
                                           unsigned serialized, uint64_t &exception,
                                           const CacheStatusHint *hint,
                                           const byte_t *data, bool writeThrough,
                                           const ReqInfo &rinfo, unsigned portId,
                                           bool isEvict)
  {
    return sendWriteRequest(ea, ra, size, serialized, exception, data, writeThrough, rinfo,
                            portId, isEvict);
  }


  inline bool Memory::readInquire(InstrBase *instr,Logger *logger, unsigned portId, unsigned requestId) const
  { return requestInquire(instr, logger, portId, requestId); }


  inline bool Memory::writeInquire(InstrBase *instr,Logger *logger, unsigned portId, unsigned requestId) const
  { return requestInquire(instr, logger, portId, requestId); }


  inline bool Memory::lockInquire(InstrBase *instr,Logger *logger) const
  {
    return memLockInquire(instr, logger) &&
      (_locked || emptyInquire(logger));
  }

  inline bool Memory::forceLockInquire(InstrBase *instr,Logger *logger) const
  {
    return memLockInquire(instr, logger);
  }


  inline void Memory::lockAction(InstrBase *instr)
  { _locked = true; _locker = instr->iid(); }


  // TODO: warn if unlocking an unlocked memory
  inline bool Memory::unlockInquire(InstrBase *instr,Logger *logger) const
  { return memLockInquire(instr, logger); }

  // No port specified, so look at total number of requests.
  inline unsigned Memory::nAvailableRequestSlots() const
  {
    return nAvailableRequestSlots(0);
  }

  // Port was specified, so look at requests just for this port.
  inline unsigned Memory::nAvailableRequestSlots(unsigned portId) const
  {    
    return getPort(portId).nAvailableRequestSlots();
  }

  inline bool Memory::emptyRequests(unsigned portId) const
  {    
    return getPort(portId).emptyRequests();
  }

  inline bool Memory::activePort(unsigned portId) const
  {    
    return getPort(portId).active();
  }

  inline void Memory::checkDelayedUnlock()
  {
    // Handle delayed unlocking.
    if (_locked && _unlock_delay >= 0) {
      if ( --_unlock_delay <= 0 ) {
        _locked = false;
        _unlock_delay = -1;
      }
    }
  }

  inline bool Memory::hasAddrBandwidth(unsigned portId) const
  { return getPort(portId).hasAddrBandwidth(currTime()); }


  inline bool Memory::hasDataBandwidth(unsigned portId) const
  { return getPort(portId).hasDataBandwidth(currTime()); }


  inline bool Memory::consumeAddrBandwidth(unsigned portId) 
  { return getPort(portId).consumeAddrBandwidth(currTime()); } 


  inline bool Memory::consumeDataBandwidth(unsigned portId) 
  { return getPort(portId).consumeDataBandwidth(currTime()); } 


  inline void Memory::addRequest(const MemoryRequest &request)
  { 
    MemoryRequest &nr = getPort(request).addRequest(request);
    nr.takenTime = currTime(); 
  }

  inline MemoryRequest *Memory::addRequest(unsigned portId)
  { 
    return getPort(portId).addRequest();
  }

  /*******************************************************************************
   * memory that uses external latency information
   *
   * TODO:
   *   - design a more accurate mapping algorithm to handle linefill/eviction
   ******************************************************************************/
  class ExtLatencyMem : public Memory {
  public:
    ExtLatencyMem(const Timer &timer, const string &name, unsigned addrBandwidth,
                  unsigned dataBandwidth, unsigned readLatency, unsigned writeLatency,
                  unsigned queueSize, bool allowBusWait, bool preemptive,
                  bool readOnly, bool withData = true, bool debug = false);

    virtual ~ExtLatencyMem() {}

    void addLatency(MemRequestType type, addr_t addr, unsigned size,
                    const std::vector<unsigned> &latencies);

  protected:
    virtual unsigned readLatency(MemoryRequest &request);
    virtual unsigned writeLatency(MemoryRequest &request);

  private:
    struct MemLatency {
      MemLatency(addr_t _addr, unsigned _size, const std::vector<unsigned> &_latencies)
        : addr(_addr), size(_size), latencies(_latencies) {}

      // data members
      addr_t addr;
      unsigned size;
      std::vector<unsigned> latencies;
    };

    bool findLatency(std::deque<MemLatency> &latencies, MemoryRequest &request,
                     bool inOrder);

    // data members
    std::deque<MemLatency> _fetchLatencies; // instruction fetch latencies
    std::deque<MemLatency> _readLatencies;  // data read latencies
    std::deque<MemLatency> _writeLatencies; // data write latencies
    std::deque<MemLatency> _evictLatencies; // eviction write latencies

    std::deque<MemLatency>* _latencies[adl::MaxCacheAccess];

    const bool _debug;
    uint64_t _nLookup; // total lookup count
    uint64_t _nMiss;   // total miss count
  };

} // namespace uadl

#endif // _UADL_MEMORY_H

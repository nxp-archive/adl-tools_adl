//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#include <assert.h>
#include <sstream>
#include <iostream>
#include <sstream>
#include <string.h>
#ifndef _MSC_VER
# include <strings.h>
#endif

#include "helpers/BasicTypes.h"
#include "helpers/Macros.h"

#include "iss/ModelInterface.h"

#include "UadlMemory.h"

using namespace std;

namespace uadl {

  UadlMemAccess::~UadlMemAccess() 
  {
    delete _hint;
    delete _data;
  }


  void Memory::reset() 
  { 
    ForEach(_ports,i) {
      i->reset();
    }
  }

  bool Memory::isActive() const 
  { 
    ForEach(_ports,i) {
      if (i->hasRequests()) return true;
    }
    return false;
  }

  void Memory::createLogger(LogClient *lc,unsigned tflags) 
  {     
    if (!_logger) {
      _logger = new MemoryLogger(name(),string(),lc,tflags); 
    } else {
      _logger->setClient(lc,tflags);
    }
  }

  void Memory::removeLogger() { 
    delete _logger;
    _logger = 0; 
  }

  bool Memory::report_and_clear(std::ostream &o,const std::string &pfx)
  {
    if (_logger) {
      return _logger->report_and_clear(o,pfx);
    } else {
      return false;
    }
  }

  // Memory
  // TODO: check page boundary
  bool Memory::getMemoryData(adl::CacheAccess type,addr_t addr, unsigned size,
                             byte_t *data)
  {
    if (_withData) memcpy(data, hasData(addr), size);

    return _withData;
  }


  // TODO: check page boundary
  bool Memory::setMemoryData(addr_t addr, unsigned size, const byte_t *data)
  {
    if (_withData) memcpy(hasData(addr), data, size);

    return _withData;
  }


  // This returns true if the memory is inactive at this point, e.g. there are no
  // current requests.
  bool Memory::emptyInquire(Logger *logger) const
  {
    if (!getPort(0).emptyRequests()) {
      if (logger) logger->logStall(name() + ": request queue not empty");
      
      return false;
    }
    
    return true;
  }

  // This returns true if the memory was inactive for this entire cycle,
  // e.g. there were no before-time-step events such as fetches.
  bool Memory::inactiveInquire(Logger *logger) const
  {
    if (_queueWasActive) {
      if (logger) logger->logStall(name() + ": request queue not inactive for this cycle");

      return false;
    }

    return true;
  }


  bool Memory::serializedInquire(Logger *logger, unsigned serialized, unsigned portId) const
  {
    if ((serialized & level_mask()) && activePort(portId)) {
      getPort(portId).incrMemStalls();
      if (logger) {
        logger->logStall(name() + ": serialized transaction with existing requests.");
      }
      return false;
    }
    
    return true;
  }

  unsigned Memory::readLatency(MemoryRequest &request)
  { 
    if (request.hasPort() && getPort(request).hasReadLatency()) {
      return getPort(request).readLatency();
    }
    return _readLatency; 
  }

  unsigned Memory::writeLatency(MemoryRequest &request)
  { 
    if (request.hasPort() && getPort(request).hasWriteLatency()) {
      return getPort(request).writeLatency();
    }
    return _writeLatency; 
  }

  bool Memory::sendRequestInquire(InstrBase *instr,Logger *logger, MemRequestType type,
                                  unsigned serialized, unsigned portId) const
  {
    if (!memLockInquire(instr, logger)) return false;

    if (!hasAddrBandwidth(portId)) {
      if (type != adl::CacheIFetch) getPort(portId).incrMemStalls();;
      if (logger)
        logger->logStall(name() + ": no address bandwidth");

      return false;
    }

    if (!serializedInquire(logger,serialized,portId)) {
      return false;
    }

    if ((_queueWasFull || !nAvailableRequestSlots(portId)) && !canPreempt(type,portId)) {
      getPort(portId).incrMemStalls();
      if (logger)
        logger->logStall(name() + ": request queue full");

      return false;
    }

    return true;
  }


  unsigned Memory::sendFetchRequest(addr_t ea, addr_t ra, unsigned size,
                                    unsigned serialized, uint64_t &exception, 
                                    const CacheStatusHint *hint, const ReqInfo &rinfo,
                                    unsigned portId)
  {
    MemoryRequest &request = sendRequest(adl::CacheIFetch, ea, ra, size,
                                         serialized, hint, false, rinfo, portId);

    if (logEnabled()) _logger->logReadRequest(request);

    return request.id;
  }


  unsigned Memory::sendReadRequest(addr_t ea, addr_t ra, unsigned size,
                                   unsigned serialized, uint64_t &exception, 
                                   const ReqInfo &rinfo, unsigned portId)
  {
    const MemoryRequest &request = sendRequest(adl::CacheRead, ea, ra, size, serialized,
                                               NULL, false, rinfo, portId);

    if (logEnabled()) _logger->logReadRequest(request);

    return request.id;
  }


  unsigned Memory::sendWriteRequest(addr_t ea, addr_t ra, unsigned size,
                                    unsigned serialized, uint64_t &exception, const byte_t *data,
                                    bool writeThrough, const ReqInfo &rinfo, unsigned portId,
                                    bool isEvict)
  {
    const MemoryRequest &request = sendRequest(adl::CacheWrite, ea, ra, size,
                                               serialized, NULL, writeThrough, rinfo, portId,
                                               isEvict);

    if (logEnabled()) _logger->logWriteRequest(request);

    return request.id;
  }



  void Memory::read(unsigned portId,unsigned requestId, byte_t *data)
  {
    MemoryPort &port = getPort(portId);
    MemoryRequest &request = port.frontRequest();
    assert(request.id == requestId);

    consumeDataBandwidth(portId);

    if (_withData && data) {
      memcpy(data, request.data, request.dataWidth);
    }

    if (logEnabled()) {
      _logger->logReadAction(request,(_withData) ? request.data : 0);
    }

    removeRequest(port);
  }

  // TODO: correctly handle readOnly
  void Memory::write(unsigned portId,unsigned requestId, const byte_t *data)
  {
    MemoryPort &port = getPort(portId);
    MemoryRequest &request = port.frontRequest();
    assert(request.id == requestId);

    consumeDataBandwidth(request.portId);

    // update memory content
    if (_withData && data && !_readOnly) {
      writeMemoryData(request.addr, request.dataWidth, data, _exception_flag);
    }

    if (logEnabled()) {
      _logger->logWriteAction(request, data);
    }

    removeRequest(port);
  } 

  void Memory::setRequestSize(unsigned portId,unsigned requestId,unsigned dataWidth)
  {
    MemoryRequest &request = *getPort(portId).findRequest(requestId);
    request.dataWidth = dataWidth;
  }

  MemoryRequest *Memory::getWaitingRequest(unsigned requestId)
  {
    MemoryRequest *wr;
    ForEach(_ports,i) {
      if ((wr = i->waitingRequest()) && (wr->id == requestId)) {
        return wr;
      }
    }
    return 0;
  }
  
  std::pair<MemoryRequest *,MemoryPort*> Memory::getWaitingRequestAndPort(unsigned requestId)
  {
    MemoryRequest *wr;
    ForEach(_ports,i) {
      if ((wr = i->waitingRequest()) && (wr->id == requestId)) {
        return make_pair(wr,&*i);
      }
    }
    return pair<MemoryRequest *,MemoryPort*>((MemoryRequest*)0,(MemoryPort*)0);
  }

  bool Memory::requestDoneInquire(Logger *logger, unsigned portId, unsigned requestId) const
  {
    bool done = true;

    // check active/pending requests
    if (getPort(portId).findRequest(requestId)) {
      done = false;
    }

    // check waiting request
    if (done) {
      if (getWaitingRequest(requestId)) done = false;
    }

    if (!done) {
      getPort(portId).incrMemStalls();
      if (logger) {
        std::ostringstream s;
        
        s << name() << ": request (id=" << requestId << ") is not completed yet";
        
        logger->logStall(s.str());
      }
    }

    return done;
  }

  bool Memory::cancel_internal(MemoryPort &port,unsigned requestId,bool deep)
  {
    bool requestFound = false;

    find_pair fp;
    if ( valid_find_pair(fp = port.findRequestPair(requestId)) ) {
      MemoryRequest &request = *fp.first;
      requestFound = true;
      
      if (logEnabled()) _logger->logCancel(request, deep);

      // A write must cancel its next-level actions
      if (deep || request.isWrite()) cancelNextLevelRequest(request);

      if (deep) {
        port.removeRequest(fp.second);
        // Perform any fixup opereations after doing removal.
        postCancelRequest(port,requestId);
        addWaitingRequest(port);
      } else {
        request.cancelled = true;
      }
    }

    return requestFound;
  }

  void Memory::check_waiting_for_cancel(unsigned requestId,bool deep)
  {
    pair<MemoryRequest*,MemoryPort*> p = getWaitingRequestAndPort(requestId);
    if (p.first) {
      
      if (logEnabled()) _logger->logCancel(*p.first, deep);
      
      if (deep) p.second->cancelWaitingRequest();
      else p.first->cancelled = true;
    } else {
      cerr << "time " << currTime() << ": " << name()
           << ":  Warning: to-be-cancelled memory request " << requestId << " not found\n";
    }
  }


  /*******************************************************************************
   * NOTE: - The circular queue is not efficient for internal deletions, but
   * request cancellation SHOULD not occur frequently.
   ******************************************************************************/
  void Memory::cancel(unsigned portId, unsigned requestId, bool deep)
  {
    // Search request in request queue
    MemoryPort &port = getPort(portId);

    if (!cancel_internal(port,requestId,deep)) {
      check_waiting_for_cancel(requestId,deep);
    }
  }

  // This searches across all ports looking for the specified request.
  void Memory::cancel(unsigned requestId,bool deep)
  {
    bool requestFound = false;

    // Search request in request queue
    ForEach(_ports,piter) {
      if (cancel_internal(*piter,requestId,deep)) {
        requestFound = true;
        break;
      }
    }
    
    if (!requestFound) {
      check_waiting_for_cancel(requestId,deep);
    }
  }

  void Memory::unlockAction(unsigned delay) 
  { 
    if (delay && _locked) {
      _unlock_delay = delay; 
    } else {
      unlockAction();
    }
  }


  void Memory::preCycle()
  {
    // save request queue full or not-full status
    saveQueueStatus();

    checkDelayedUnlock();

    ForEach(_ports,piter) {
      MemoryPort &port = *piter;
      for (unsigned i = 0; i != port.sizeRequests(); ++i) {
        MemoryRequest &request = port.getRequest(i);
        
        if (!request.expired) {
          if (request.isRead()) {
            request.latency = readLatency(request);
            if (timer().timeAtOrPassed(request.time + request.latency)) {
              request.expired = true;
              request.data = hasData(request.addr);
            }
          } else { // request.isWrite()
            request.latency = writeLatency(request);
            request.expired = timer().timeAtOrPassed(request.time + request.latency);
          }
        }
      }

      // remove expired-and-cancelled requests from the header of the queue
      while (port.hasRequests() && port.frontRequest().expired && port.frontRequest().cancelled) {
        removeRequest(port);
      }
    }

  }

  void Memory::logRequestInternal(const string &portname,unsigned index,const MemoryRequest &request,bool &first) const
  {
    int expires = ( request.time + request.latency - timer().currTime() );
    if (first) {
      _logger->logMessage("----");
      first = false;
    }
    _logger->logPortRequest(portname,index,expires,request);
  }

  void Memory::logAllRequests() const
  {
    if (_logger) {
      static string empty_name;

      ForEach(_ports,piter) {
        const MemoryPort &port = *piter;
        bool first = true;
        const string &portname = (port.name() == port.defaultPort()) ? empty_name : port.name();
        for (unsigned i = 0; i != port.sizeRequests(); ++i) {
          const MemoryRequest &request = port.getRequest(i);
          logRequestInternal(portname,i,request,first);
        }
        if (const MemoryRequest *wr = port.waitingRequest()) {
          logRequestInternal(portname,-1,*wr,first);
        }
      }
    }
  }


  const MemoryRequest* Memory::requestInfo(unsigned portId,unsigned requestId) const
  {
    if (const MemoryRequest *request = getPort(portId).findRequest(requestId)) {
      return request;
    }
    
    return getWaitingRequest(requestId);
  }

  bool Memory::requestInquire(InstrBase *instr,Logger *logger, unsigned portId, unsigned requestId) const
  {
    if (!memLockInquire(instr, logger)) return false;

    // find the request
    const MemoryPort &port = getPort(portId);
    const MemoryRequest *request = port.findRequest(requestId);

    if (!request) {
      getPort(portId).incrMemStalls();
      if (logger)
        logger->logStall(name() + ": request does not exist");

      return false;
    }

    // check: request is at port header
    if (request != &port.frontRequest()) {
      getPort(portId).incrMemStalls();
      if (logger)
        logger->logStall(name() + ": request pending");

      return false;
    }

    if (!request->expired) {
      getPort(portId).incrMemStalls();
      if (logger) {
        if (request->isRead())
          logger->logStall(name() + ": data not ready");
        else if (request->isWrite())
          logger->logStall(name() + ": write in progress");
        else
          logger->logStall(name() + ": request processing in progress");
      }

      return false;
    }

    if (!hasDataBandwidth(request->portId)) {
      getPort(portId).incrMemStalls();
      if (logger)
        logger->logStall(name() + ": no data bandwidth");

      return false;
    }

    return true;
  }


  MemoryRequest& Memory::sendRequest(MemRequestType type, addr_t ea,
                                           addr_t ra, unsigned size,unsigned serialized,
                                           const CacheStatusHint *hint,
                                           bool writeThrough, const ReqInfo &rinfo,
                                           int portId, bool isEvict)
  {
    consumeAddrBandwidth(portId);
  
    MemoryRequest *nr = 0;

    MemoryPort &port = getPort(portId);

    // accept request
    if (!port.fullRequests()) {
      nr = port.addRequest();
      // In-place new means that this is not allocating memory, just writing
      // over the location given.
      new (nr) MemoryRequest(_requestId++, currTime(), type, isEvict, ea, ra, size, serialized, hint, writeThrough,
                             rinfo, portId);
    }
    else if (const MemoryRequest *request = canPreempt(type,portId)) {
      if (!request->cancelled) {
        preemptFetch();

        if (logEnabled()) _logger->logPreempt(*request);
      }

      nr = const_cast<MemoryRequest*>(request);
      new (nr) MemoryRequest(_requestId++, currTime(), type, isEvict, ea, ra, size, serialized, hint, writeThrough,
                             rinfo, portId);
    }
    else { // _allowWaitingRequest && !_hasWaitingRequest
      MemoryRequest newRequest (_requestId++, currTime(), type, isEvict, ea, ra, size, serialized, hint, writeThrough,
                                rinfo, portId);
      nr = &(getPort(portId).addWaitingRequest(newRequest));
    }

    updateQueueStatus();

    return *nr;
  }


  bool Memory::memLockInquire(InstrBase *instr,Logger *logger) const
  {
    if (_locked && (!instr || instr->iid() != _locker)) {
      if (logger) {
        ostringstream ss;
        ss << name() << " locked by instruction [iid=" << _locker << "]";
        logger->logStall(ss.str());
      }

      return false;
    }

    return true;
  }

  void Memory::addWaitingRequest(MemoryPort &port)
  {
    if (const MemoryRequest *wr = port.waitingRequest()) {
      addRequest(*wr);
      port.cancelWaitingRequest();
    }
  }

  void Memory::removeRequest(MemoryPort &port)
  {
    port.popFrontRequest();
    addWaitingRequest(port);
  }

  void Memory::saveQueueStatus()
  {
    updateQueueStatus();
    updatePortStatus();
    if (_preemptive) {
      _hasPreemptionId = false;
      ForEach(_ports,i) {
        const MemoryRequest *wr = i->waitingRequest();
        if (wr && wr->type == adl::CacheIFetch) {
          _hasPreemptionId = true;
          _preemptionId = wr->id;
          break;
        }
      }
    }
  }

  void Memory::updatePortStatus()
  {
    ForEach(_ports,i) {
      i->updateStatus();
    }
  }

  MemoryPort *Memory::addPort(const string &name, bool commonBandwidth, 
                              unsigned addrBandwidth, unsigned dataBandwidth, 
                              unsigned readLatency, unsigned writeLatency,
                              unsigned maxRequests,bool allowBusWait)
  {
    if (_ports.size() == 1 && _ports[0].name() == MemoryPort::defaultPort()) {
      _ports.pop_back();
    }

    _ports.push_back(MemoryPort(name, commonBandwidth, addrBandwidth, dataBandwidth, 
                                readLatency, writeLatency, maxRequests, allowBusWait));
    return &_ports.back();
  }

  byte_t* Memory::hasData(addr_t addr) const
  {
    if (_withData) {
      unsigned tmp;

      return hasData(addr, tmp);
    }
    else {
      return NULL;
    }
  }

  const MemoryRequest* Memory::canPreempt(MemRequestType type,unsigned portId) const
  {
    if (_hasPreemptionId) {
      const MemoryRequest *request = getWaitingRequest(_preemptionId);

      const MemoryPort &port = getPort(portId);
      if (!request && port.backRequest().id == _preemptionId)
        request = &port.backRequest();

      if (request && (type == adl::CacheRead || type == adl::CacheWrite || request->cancelled)) { 
        return request; 
      }
    }

    return 0;
  }

  // ExtLatencyMem
  ExtLatencyMem::ExtLatencyMem(const Timer &timer, const string &name,
                               unsigned addrBandwidth, unsigned dataBandwidth,
                               unsigned readLatency, unsigned writeLatency,
                               unsigned queueSize, bool allowBusWait,
                               bool preemptive, bool readOnly, bool withData,
                               bool debug) :
    Memory(timer, name, addrBandwidth, dataBandwidth, readLatency, writeLatency,
           queueSize, allowBusWait, preemptive, readOnly, withData),
    _debug(debug), _nLookup(0), _nMiss(0)
  {
    for (unsigned i = 0; i < adl::MaxCacheAccess; i++) _latencies[i] = NULL;

    _latencies[adl::CacheIFetch] = &_fetchLatencies;
    _latencies[adl::CacheRead] = &_readLatencies;
    _latencies[adl::CacheWrite] = &_writeLatencies;
    _latencies[adl::CacheFlush] = &_evictLatencies;
  }


  void ExtLatencyMem::addLatency(MemRequestType type, addr_t addr, unsigned size,
                                 const vector<unsigned> &latencies)
  {
    if (_latencies[type]) {
      _latencies[type]->push_back(MemLatency(addr, size, latencies));
    }

    /* NOTE:
     *   - ADL uses CacheFlush to indicate eviction write
     *   - remove oldest eviction latency if size threshold exceeded */
    // TODO: 5 is an experimental magic number
    if (type == adl::CacheFlush && _evictLatencies.size() > 5)
      _evictLatencies.pop_front();
  }


  unsigned ExtLatencyMem::readLatency(MemoryRequest &request)
  {
    if (!request.latency) {
      request.latency = Memory::readLatency(request);

      findLatency(*_latencies[request.type], request, true);
    }

    return request.latency;
  }


  unsigned ExtLatencyMem::writeLatency(MemoryRequest &request)
  {
    if (!request.latency) {
      request.latency = Memory::writeLatency(request);

      // eviction latency record is out-of-order
      findLatency(request.isEvict ? _evictLatencies : _writeLatencies, request,
                  !request.isEvict);
    }

    return request.latency;
  }


  bool ExtLatencyMem::findLatency(deque<MemLatency> &latencies,
                                  MemoryRequest &request, bool inOrder)
  {
    if (_debug) _nLookup++;

    for (deque<MemLatency>::iterator i = latencies.begin(); i != latencies.end();
         i++)
      {
        if (request.addr >= i->addr && request.addr < i->addr + i->size) {
          // TODO: replace 8 with a parameter
          const unsigned index = (request.addr - i->addr) >> 3;

          // latency found
          request.latency = i->latencies.size() > index ? i->latencies[index]
            : i->latencies[0];

          // if in-order access, remove all past entries
          if (inOrder && i != latencies.begin())
            latencies.erase(latencies.begin(), i);

          return true;
        }
      }

    // latency not found
    if (_debug) {
      _nMiss++;

      const double missRate = _nLookup ? (double)1.0 * _nMiss / _nLookup : 0;
      const string type = request.type == adl::CacheIFetch ? "fetch" :
        request.type == adl::CacheRead   ? "read"  :
        request.isEvict                  ? "evict" : "write";

      cerr << "Warning: " << name() << " latency lookup miss, time: "
           << currTime() << ", type: " << type << ", ra: 0x" << std::hex
           << request.addr << std::dec << "\n"
           << "         total lookup count: " << _nLookup << "\n"
           << "         total miss count:   " << _nMiss << "\n"
           << "         miss rate:          " << missRate << "\n";

    }

    return false;
  }

}

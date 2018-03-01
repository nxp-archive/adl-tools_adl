//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef _MEMORY_REQUEST_H
#define _MEMORY_REQUEST_H

#include <string>

#include "iss/ModelInterface.h"
#include "iss/CircularQueue.h"

#include "Timer.h"
#include "BandwidthControl.h"
#include "UadlTypes.h"

namespace uadl {

  using adl::byte_t;
  using adl::addr_t;
  using adl::CacheAction;


  // TODO: support multiple levels of caches on store buffer enable status
  class CacheStatusHint {
  public:
    CacheStatusHint() : _storeBufEnabled(false), _next(0) {};

    bool valid() const { return _cacheStatus.valid(); }

    // TODO: should be const
    adl::CacheStatus  _cacheStatus;
    bool              _storeBufEnabled;
    CacheStatusHint  *_next;
  };

  typedef adl::CacheAccess MemRequestType;

  /*******************************************************************************
   * memory request
   *
   * NOTE:
   *   - server side fills: id, time, expired, cancelled, data (read)
   *   - client side fills: type, ea, ra, size, writeThrough
   *
   * TODO:
   *   - change type of request id to uint64_t
   *   - use access methods to access data members
   *   - put cache-only members into a derived class
   ******************************************************************************/
  class MemoryRequest {
  public:
    MemoryRequest() :
      portId(0), expired(false), cancelled(false),
      type(adl::CacheRead), isEvict(false), ea(0), addr(0), dataWidth(0),
      serialized(0), data(NULL), latency(0), hint(CacheStatusHint()),
      writeThrough(false), lookupDone(false), replaceLine(false), set(-1), way(-1), 
      action(adl::CacheNone), hitLinefillBuf(false), useStoreBuf(false)
    {}

    MemoryRequest(MemRequestType _type, bool _isEvict, addr_t _ea, addr_t _ra,
                  unsigned _size, unsigned _serialized, const CacheStatusHint *_hint = NULL,
                  bool _writeThrough = false, const ReqInfo &_rinfo = ReqInfo(),
                  int _portId = 0) :
      rinfo(_rinfo), portId(_portId), expired(false), cancelled(false),
      type(_type), isEvict(_isEvict), ea(_ea), addr(_ra), dataWidth(_size),
      serialized(_serialized), data(NULL), latency(0), hint(_hint ? *_hint : CacheStatusHint()),
      writeThrough(_writeThrough), lookupDone(false),  replaceLine(false), set(-1), way(-1), 
      action(adl::CacheNone), hitLinefillBuf(false), useStoreBuf(false)
    {}

    MemoryRequest(unsigned _id,ttime_t _time,MemRequestType _type, bool _isEvict, addr_t _ea, addr_t _ra,
                  unsigned _size, unsigned _serialized, const CacheStatusHint *_hint = NULL,
                  bool _writeThrough = false, const ReqInfo &_rinfo = ReqInfo(),
                  int _portId = 0) :
      id(_id), rinfo(_rinfo), portId(_portId), time(_time), takenTime(_time), expired(false), cancelled(false),
      type(_type), isEvict(_isEvict), ea(_ea), addr(_ra), dataWidth(_size),
      serialized(_serialized), data(NULL), latency(0), hint(_hint ? *_hint : CacheStatusHint()),
      writeThrough(_writeThrough), lookupDone(false),  replaceLine(false), set(-1), way(-1),
      action(adl::CacheNone), hitLinefillBuf(false), useStoreBuf(false)
    {}

    // access/query methods
    bool hasPort() const { return portId >= 0; };
    bool isRead() const;
    bool isWrite() const { return type == adl::CacheWrite; }
    bool isDataAccess() const { return !isInstAccess(); }
    bool isInstAccess() const { return type == adl::CacheIFetch; }

    // data members
    unsigned id;         // request id
    ReqInfo  rinfo;      // info of instruction generating request.

    int portId;          // request port (-1 for cache commands)
    ttime_t time;        // request time
    ttime_t takenTime;   // taken time
    ttime_t lookupTime;  // lookup time (cache only)
    bool expired;        // whether latency has expired
    bool cancelled;      // whether request has been cancelled

    MemRequestType type; // request type
    bool isEvict;        // sub write type: eviction or not
    addr_t ea;           // effective address
    addr_t addr;         // real address
    unsigned dataWidth;  // data width
    unsigned serialized; // Serialized-transaction mask.
    byte_t *data;        // data (for read only)
    unsigned latency;

    // cache-only section
    bool isHit() const;
    bool needEviction() const { return lookupDone && action == adl::CacheEvict; }
    bool needLinefill() const;

    // data members
    CacheStatusHint hint;         // cache status hint
    bool writeThrough;            // write through flag

    bool lookupDone;
    bool replaceLine;             // whether cause a line to be replaced
    int set;                      // signed to be compatible with ADL
    int way;                      // signed to be compatible with ADL
    unsigned offset;
    CacheAction action;           // ADL cache action code
    bool hitLinefillBuf;
    bool useStoreBuf;             // whether write goes through store buffer

    byte_t buf[sizeof(uint64_t)]; // buffer to hold non-allocate read miss data
  };


  inline bool MemoryRequest::isRead() const
  { return type == adl::CacheRead || type == adl::CacheIFetch; }


  inline bool MemoryRequest::isHit() const
  { return lookupDone && (action == adl::CacheHit || hitLinefillBuf); }


  // only for CacheRead, CacheWrite and CacheTouch
  inline bool MemoryRequest::needLinefill() const
  {
    return lookupDone && (action == adl::CacheMiss || action == adl::CacheEvict);
  }

  typedef std::pair<MemoryRequest *,int> find_pair;
  typedef std::pair<const MemoryRequest *,int> const_find_pair;

  inline bool valid_find_pair(const find_pair &p) { return p.first; };
  inline bool valid_find_pair(const const_find_pair &p) { return p.first; };

  //
  // uADL memory port class.  Each port allows the model to constrain behavior
  // based upon bandwidth concerns or by maximum-outstanding requests.  The port
  // can be referenced explicitly in the model's statemachines, or implicitly,
  // such as via a cache's next-level or memory.  Bandwidth can be separate for
  // address and data, or linked together.
  //
  class MemoryPort {
    typedef adl::DCircularQueue<MemoryRequest> MemoryRequests;

  public:
    MemoryPort(const std::string &name, bool commonBandwidth,unsigned addrBandwidth, unsigned dataBandwidth,
               unsigned readLatency, unsigned writeLatency, unsigned maxRequests, bool allowBusWait) : 
      _name(name), 
      _commonBandwidth(commonBandwidth),
      _addrBandwidth(addrBandwidth), 
      _dataBandwidthInternal(dataBandwidth),
      _dataBandwidth( (commonBandwidth) ? &_addrBandwidth : &_dataBandwidthInternal),
      _readLatency(readLatency),
      _writeLatency(writeLatency),
      _requests(maxRequests),
      _active(false),
      _allowWaitingRequest(allowBusWait),
      _hasWaitingRequest(false),
      _memStalls(0)
    {}

    MemoryPort(const MemoryPort &x) :
      _name(x._name), 
      _commonBandwidth(x._commonBandwidth),
      _addrBandwidth(x._addrBandwidth), 
      _dataBandwidthInternal(x._dataBandwidthInternal),
      _dataBandwidth( (_commonBandwidth) ? &_addrBandwidth : &_dataBandwidthInternal),
      _readLatency(x._readLatency),
      _writeLatency(x._writeLatency),
      _requests(x._requests.capacity()),
      _active(false),
      _allowWaitingRequest(x._allowWaitingRequest),
      _hasWaitingRequest(x._hasWaitingRequest),
      _memStalls(0)
    {
    }
    
    const std::string &name() const { return _name; }

    void reset();

    MemoryPort &operator= (const MemoryPort &x);

    bool hasReadLatency() const { return _readLatency > 0; };
    bool hasWriteLatency() const { return _writeLatency > 0; };

    unsigned readLatency() const { return _readLatency; };
    unsigned writeLatency() const { return _writeLatency; };

    unsigned nAvailableRequestSlots() const;

    // bandwidth control/query methods
    bool hasAddrBandwidth(ttime_t currTime) const;
    bool hasDataBandwidth(ttime_t currTime) const;
    bool consumeAddrBandwidth(ttime_t currTime);
    bool consumeDataBandwidth(ttime_t currTime);

    unsigned sizeRequests() const { return _requests.size(); };

    MemoryRequest &addRequest(const MemoryRequest &request);
    MemoryRequest *addRequest();

    bool hasRequests() const { return !_requests.empty(); };
    bool fullRequests() const { return _requests.full(); };
    bool emptyRequests() const { return (_requests.empty() && !hasWaitingRequest()); };
    bool hasWaitingRequest() const { return _hasWaitingRequest; };

    MemoryRequest &addWaitingRequest(const MemoryRequest &r);
    void cancelWaitingRequest() { _hasWaitingRequest = false; };

    // Remove the first request.
    void popFrontRequest() { _requests.pop_front(); };

    // Remove a specified request, given an index.
    void removeRequest(unsigned index) { _requests.erase(index); };

    // Access to the first request.
    const MemoryRequest &frontRequest() const { return _requests.front(); };
    MemoryRequest &frontRequest() { return _requests.front(); };

    // Access to the last request.
    const MemoryRequest &backRequest() const { return _requests.back(); };
    MemoryRequest &backRequest() { return _requests.back(); };

    // Get a request by index in the queue.
    const MemoryRequest &getRequest(unsigned index) const { return _requests[index]; };
    MemoryRequest &getRequest(unsigned index) { return _requests[index]; };

    // Find a request by id, return 0 if not found.
    const MemoryRequest *findRequest(unsigned requestId) const;
    MemoryRequest *findRequest(unsigned requestId);

    // Find a request by id, returns a pair:  Pointer to request, and index of request.
    // The returned object can be tested for a successful find by calling valid_find().
    find_pair findRequestPair(unsigned requestId);
    const_find_pair findRequestPair(unsigned requestId) const;

    const MemoryRequest *waitingRequest() const;
    MemoryRequest *waitingRequest();

    bool active() const { return _active; };

    void updateStatus() { _active = !emptyRequests(); };

    unsigned memStalls() const { return _memStalls; };
    void incrMemStalls() const { ++_memStalls; };

    // Ensure that all default ports have the same name
    static const std::string &defaultPort();

  private:
    std::string       _name;
    bool              _commonBandwidth;
    BandwidthControl  _addrBandwidth;
    BandwidthControl  _dataBandwidthInternal;
    BandwidthControl *_dataBandwidth;

    unsigned          _readLatency, 
                      _writeLatency;        // Latency (in cycles).  Valid if not zero.

    MemoryRequests    _requests;            // Stores the actual requests.

    bool              _active;              // Active during this cycle.
    bool              _allowWaitingRequest; // If we allow a request to wait on the bus.
    bool              _hasWaitingRequest;   // If a waiting request exists.
    MemoryRequest     _waitingRequest;      // Request waiting on the bus

    mutable unsigned  _memStalls;
  };

  typedef std::vector<MemoryPort> MemoryPorts;

  inline MemoryPort &MemoryPort::operator= (const MemoryPort &x)
  {
    _name = x._name;
    _commonBandwidth = x._commonBandwidth;
    _addrBandwidth = x._addrBandwidth;
    _dataBandwidthInternal = x._dataBandwidthInternal;
    _dataBandwidth = (_commonBandwidth) ? &_addrBandwidth : &_dataBandwidthInternal;

    _readLatency = x._readLatency;
    _writeLatency = x._writeLatency;
    _requests.clear();

    _allowWaitingRequest = x._allowWaitingRequest;
    _hasWaitingRequest = x._hasWaitingRequest;
    return *this;
  }

  inline void MemoryPort::reset()
  {
    _requests.clear();
    _hasWaitingRequest = false;
    _memStalls = 0;
  }

  inline bool MemoryPort::hasAddrBandwidth(ttime_t currTime) const
  { return _addrBandwidth.hasBandwidth(currTime); }


  inline bool MemoryPort::hasDataBandwidth(ttime_t currTime) const
  { return _dataBandwidth->hasBandwidth(currTime); }


  inline bool MemoryPort::consumeAddrBandwidth(ttime_t currTime)
  { return _addrBandwidth.consumeBandwidth(currTime); }


  inline bool MemoryPort::consumeDataBandwidth(ttime_t currTime)
  { return _dataBandwidth->consumeBandwidth(currTime); }


  inline const std::string &MemoryPort::defaultPort() 
  { static std::string name = "__default_Port_Name__"; return name; }


  inline unsigned MemoryPort::nAvailableRequestSlots() const 
  { 
    return (_requests.capacity() - _requests.size()) + ( (_allowWaitingRequest && !_hasWaitingRequest) ? 1 : 0);
  }

  inline MemoryRequest &MemoryPort::addRequest(const MemoryRequest &request)
  {
    MemoryRequest *r = _requests.push_back();
    *r = request;
    return *r;
  }

  inline MemoryRequest *MemoryPort::addRequest()
  {
    return _requests.push_back();
  }

  inline MemoryRequest &MemoryPort::addWaitingRequest(const MemoryRequest &r)
  {
    _hasWaitingRequest = true; 
    _waitingRequest = r;
    return _waitingRequest;
  }

  inline const MemoryRequest *MemoryPort::findRequest(unsigned requestId) const
  {
    return const_cast<MemoryPort*>(this)->findRequest(requestId);
  }

  // This is an in-order search, but since we generally care about what's at the
  // front, it should normally be fast.
  inline MemoryRequest *MemoryPort::findRequest(unsigned requestId)
  {
    for (unsigned i = 0; i != _requests.size(); ++i) {
      if (_requests[i].id == requestId) {
        return &_requests[i];
      }
    }
    return 0;
  }

  inline const_find_pair MemoryPort::findRequestPair(unsigned requestId) const
  {
    return const_cast<MemoryPort*>(this)->findRequestPair(requestId);
  }

  inline find_pair MemoryPort::findRequestPair(unsigned requestId)
  {
    for (unsigned i = 0; i != _requests.size(); ++i) {
      if (_requests[i].id == requestId) {
        return find_pair(&_requests[i],i);
      }
    }
    return find_pair(reinterpret_cast<MemoryRequest*>(0),-1);
  }

  inline const MemoryRequest *MemoryPort::waitingRequest() const
  {
    return const_cast<MemoryPort*>(this)->waitingRequest();
  }

  inline MemoryRequest *MemoryPort::waitingRequest()
  {
    return (_hasWaitingRequest) ? &_waitingRequest : 0;
  }

} // namespace uadl

#endif // _MEMORY_REQUEST_H

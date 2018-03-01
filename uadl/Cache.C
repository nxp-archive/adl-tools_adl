//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

// TODO: assert(request does not cross line boundary)

#include <assert.h>
#include <string.h>
#include <sstream>
#include <iomanip>
#include <iostream>
#include <stdexcept>

#include "helpers/BasicTypes.h"
#include "helpers/Macros.h"

#include "iss/ModelInterface.h"
#include "iss/TypeIO.h"

#include "Cache.h"

using namespace std;

using adl::byte_t;

namespace uadl {

  bool CacheConfigMap::get_bool(const string &key,bool dval) const
  {
    const_iterator i = find(key);
    if (i != end() && i->second.is_int()) {
      return i->second.val();
    } else {
      return dval;
    }
  }

  int CacheConfigMap::get_int(const string &key,int dval) const
  {
    const_iterator i = find(key);
    if (i != end() && i->second.is_int()) {
      return i->second.val();
    } else {
      return dval;
    }
  }

  string CacheConfigMap::get_str(const string &key,const string &dval) const
  {
    const_iterator i = find(key);
    if (i != end() && i->second.is_str()) {
      return i->second.str();
    } else {
      return dval;
    }
  }

  // LinefillBuf
  LinefillBuf::LinefillBuf(Cache &cache, UadlMemoryIf &memory,
                           unsigned memoryPort, unsigned lineWidth, unsigned entrySize,
                           bool criticalWordFirst, bool streaming, 
                           bool withData, bool lazyWriteback, bool dynEntrySize)
    : _withData(withData),
      _cache(cache), _memory(memory),
      _memoryPort(memoryPort), _logger(0), _criticalWordFirst(criticalWordFirst),
      _streaming(criticalWordFirst && streaming), _lazyWriteback(lazyWriteback), 
      _dynEntrySize(dynEntrySize), _empty(true), _dirty(false),
      _locked(false), _loaded(false), _started(false), _allsent(false), 
      _criticalWordReceived(false), _requestsRemaining(0),
      _data(0), _hasExtra(false)
  {
    setEntrySize(lineWidth,entrySize);
  }

  void LinefillBuf::setEntrySize(unsigned lineWidth,unsigned p_entrySize)
  {
    _lineWidth = lineWidth;
    _entrySize = p_entrySize;
    _nEntries  = lineWidth / entrySize();

    unsigned emask = entrySize()-1;

    _lineAddrMask = ~((addr_t)lineWidth-1);
    _entryOffsetMask = (~_lineAddrMask & ~(addr_t)emask);
    _entryShift = count_trailing_zeros(entrySize());
    _byteOffsetMask = (~_lineAddrMask);

    _entries.resize(_nEntries);

    if (_withData) {
      delete _data;
      byte_t *data = _data = new byte_t[_nEntries * entrySize()];

      for (unsigned i = 0; i < _nEntries; i++, data += entrySize())
        _entries[i].data = data;
    }
  }

  void LinefillBuf::init(const ReqInfo &rinfo, uint64_t requestId, addr_t ea,
                         addr_t ra, unsigned set, unsigned way, bool readData,
                         ttime_t requestTime, bool replaceLine)
  {
    _rinfo = rinfo;
    _requestId = requestId;
    _set = set;
    _way = way;
    _readData = readData;

    addr_t entry_ea = ea & _lineAddrMask;
    addr_t entry_ra = ra & _lineAddrMask;

    _criticalWordReceived = false;

    if (_dynEntrySize) {
      unsigned curEntrySize = _cache.linefillWidth(ea,ra);
      if (curEntrySize != _entrySize) {
        setEntrySize( _lineWidth, curEntrySize );
      }
    }

    _requestsRemaining = _nEntries;
    // As we initialize the linefill buffer, if we have a valid, extra outstanding
    // entry, then record that and toggle the other off.
    for (unsigned i = 0; i < _nEntries; entry_ea += entrySize(), entry_ra += entrySize(), i++) { 
      _entries[i].init(entry_ea, entry_ra); 
      if (_hasExtra && entry_ra == _extraEntry.addr && _extraEntry.requestSent) {
        _entries[i].requestSent = true;
        _entries[i].requestId = _extraEntry.requestId;
        _hasExtra = false;
        --_requestsRemaining;
      }
    }
    _hasExtra = false;

    _empty   = false;
    _loaded  = false;
    _dirty   = false;
    _locked  = false;
    _started = false;
    _allsent = false;

    if (criticalWordFirst()) {
      _criticalIds.clear();
      addCriticalAddr(ra);
    }

    if (_streaming) {
      _requestTime = requestTime;
      _replaceLine = replaceLine;
      _streamed = false;
    }
  }

  bool LinefillBuf::setupExtra(addr_t ea,addr_t ra)
  {
    if (!_hasExtra) {
      _hasExtra = true;
      if (criticalWordFirst()) {
        ea &= ~adl::DWMask;
        ra &= ~adl::DWMask;
      } else {
        ea &= _lineAddrMask;
        ra &= _lineAddrMask;
      }
      _extraEntry.init(ea,ra);
      return true;
    }
    return false;
  }

  // TODO: should write set hasData?  Will double-word write cancel read?
  bool LinefillBuf::loaded() const
  {
    if (empty()) return false;
    
    if (_loaded) return true;

    for (unsigned i = 0; i < _nEntries; i++) if (!_entries[i].hasData) return false;

    _loaded = true;

    return true;
  }


  const vector<addr_t> LinefillBuf::criticalAddrs() const
  {
    if (empty() || !criticalWordFirst() || _criticalIds.empty())
      return vector<addr_t>();

    vector<addr_t> addrs(_criticalIds.size(), 0);

    for (unsigned i = 0; i < _criticalIds.size(); i++)
      addrs[i] = _entries[_criticalIds[i]].addr;

    return addrs;
  }


  bool LinefillBuf::loadData()
  {
    bool dataRead = false;
    _criticalWordReceived = false;

    if (!empty()) {
      bool continueRead = true;

      // load critical entries
      if (_criticalWordFirst) {
        for (unsigned i = 0; continueRead && i < _criticalIds.size(); i++) {
          if ( (continueRead = loadEntryData(_entries[_criticalIds[i]], dataRead)) ) {
            if (_criticalIds[i] == _critical_entry) {
              _criticalWordReceived = true;
            }
          }
        }

        bool hasData = true;

        while (!_criticalIds.empty() && hasData) {
          // remove entry id if data loaded
          if ((hasData = _entries[_criticalIds.front()].hasData))
            _criticalIds.pop_front();
        }
      }

      // load non-critical entries
      for (unsigned i = 0; continueRead && i < _nEntries; i++)
        continueRead = loadEntryData(_entries[i], dataRead);
    }

    return dataRead;
  }


  bool LinefillBuf::sendRequest()
  {
    bool continueSend = true;

    _allsent = true;

    if (!empty()) {
      // load critical entries
      if (_criticalWordFirst) {
        for (unsigned i = 0; continueSend && i < _criticalIds.size(); i++) {
          LinefillBufEntry &entry = _entries[_criticalIds[i]];
          continueSend = sendEntryRequest(entry);
          _allsent = _allsent && entry.requestSent;
        }
      }

      // load non-critical entries
      for (unsigned i = 0; continueSend && i < _nEntries; i++) {
        LinefillBufEntry &entry = _entries[i];
        continueSend = sendEntryRequest(entry);
        _allsent = _allsent && entry.requestSent;
      }
    }

    return continueSend;
  }

  // Send an extra request, if it exists.
  bool LinefillBuf::sendExtraRequest()
  {
    if (_hasExtra && !_extraEntry.requestSent) {
      return sendEntryRequest(_extraEntry);
    } else{
      return true;
    }
  }

  // NOTE: assume the request hits the linefill buffer
  bool LinefillBuf::readStreamPredicate(const MemoryRequest &request)
  {
    _streamed = _streaming &&
      /* streaming conditions:
       *   - the request that immediately follows in time
       *   - some uncomprehensible mystery :) */
      (request.time == _requestTime + 1 ||
       (!_streamed && !_replaceLine && _started &&
        Timer::isOldOrSameTime(request.time, _startTime)));

    return _streamed;
  }


  bool LinefillBuf::loadEntryData(LinefillBufEntry &entry, bool &dataRead)
  {
    if (entry.hasData)      return true;  // continue loading
    if (!entry.requestSent) return false; // stop loading

    bool canRead = _memory.readInquire(0, 0, _memoryPort, entry.requestId);

    if (canRead) {
      _memory.read(_memoryPort, entry.requestId, entry.data);
      dataRead = true;
      entry.hasData = true;

      if (_logger) {
        _logger->logReadNextAction(entry.requestId, entry.ea, entry.addr,
                                   entrySize(), 0, !_readData, entry.data, _rinfo);
      }
    }

    return canRead;
  }


  bool LinefillBuf::sendEntryRequest(LinefillBufEntry &entry)
  {
    if (entry.hasData || entry.requestSent) return true; // continue sending

    const bool canSendRequest = _memory.sendRequestInquire(0,0,
                                                           _readData ? adl::CacheRead : adl::CacheIFetch,
                                                           0, _memoryPort
                                                           );

    if (canSendRequest) {
      uint64_t exception = 0; // exception flag place holder

      if (_readData) { // read data
        entry.requestId = _memory.sendReadRequest(entry.ea, entry.addr,
                                                  entrySize(), 0, exception,
                                                  _rinfo, _memoryPort);
      }
      else {           // read instructions
        entry.requestId = _memory.sendFetchRequest(entry.ea, entry.addr,
                                                   entrySize(), 0, exception,
                                                   0, _rinfo, _memoryPort);
      }

      entry.requestSent = true;
      --_requestsRemaining;

      if (_logger) {
        _logger->logReadNextRequest(entry.requestId, entry.ea, entry.addr,
                                    entrySize(), 0, !_readData, _rinfo);
      }

      if (!_started) {
        _started = true;
        _startTime = _cache.currTime();
      }
    }

    return canSendRequest;
  }

  //
  // EvictBuf
  //

  void EvictBuf::setEntrySize(unsigned lineWidth, unsigned entrySize)
  {
    _nEntries = (lineWidth / entrySize);
    _entrySize = entrySize;
    
    _entries.resize(_nEntries);
    
    delete _data;
    if (_withData) _data = new byte_t[_nEntries * entrySize]; 
  }

  void EvictBuf::init(const ReqInfo &rinfo, uint64_t requestId, addr_t ea, addr_t ra,
                      const byte_t *data)
  {
    if (_dynEntrySize) {
      unsigned curEntrySize = _cache.linefillWidth(ea,ra);
      if (curEntrySize != _entrySize) {
        setEntrySize( _lineWidth, curEntrySize );
      }
    }

    for (unsigned i = 0; i < _nEntries; ea += _entrySize, ra += _entrySize, i++)
      _entries[i].reset(ea, ra);

    if (_data) memcpy(_data, data, _nEntries * _entrySize);

    _hasData = true;
    _rinfo = rinfo;
    _requestId = requestId;
  }


  bool EvictBuf::evicted() const
  {
    if (empty()) return false;

    for (unsigned i = 0; i < _nEntries; i++) if (!_entries[i].dataSent) return false;

    return true;
  }


  bool EvictBuf::pushData()
  {
    bool dataWritten = false;

    if (!empty()) {
      bool continueWrite = true;
      const byte_t *data = _data;

      for (unsigned i = 0; continueWrite && i < _nEntries; i++) {
        EvictBufEntry &entry = _entries[i];

        continueWrite = entry.requestSent && (entry.dataSent ||
                                              _memory.writeInquire(0, 0, _memoryPort, entry.requestId));

        if (!entry.dataSent && continueWrite) {
          _memory.write(_memoryPort, entry.requestId, data);
          dataWritten = true;
          entry.dataSent = true;

          if (_logger) {
            _logger->logWriteNextAction(entry.requestId, entry.ea, entry.addr,
                                        _entrySize, 0, data, _rinfo);
          }
        }

        // advance data position if the eviction buffer stores data
        if (data) data+= _entrySize;
      }
    }

    return dataWritten;
  }


  bool EvictBuf::sendRequest()
  {
    bool continueSend = true;
    const byte_t *data = _data;

    if (!empty()) {
      for (unsigned i = 0; continueSend && i < _nEntries; i++) {
        EvictBufEntry &entry = _entries[i];
        // continueSend = entry.requestSent || 
        //                _memory.sendRequestInquire(0, adl::CacheWrite, false,
        //                                           _memoryPort);

        // TODO: temporary ugly transformation
        if (!(continueSend = entry.requestSent)) {
          if ((continueSend = _memory.sendRequestInquire(0, 0, adl::CacheWrite, 0, _memoryPort))) {
            uint64_t exception = 0; // exception flag place holder

            entry.requestId = _memory.sendWriteRequest(entry.ea, entry.addr,
                                                       _entrySize, 0, exception,
                                                       data, false, _rinfo,
                                                       _memoryPort, true);
            entry.requestSent = true;

            if (_logger) {
              _logger->logWriteNextRequest(entry.requestId, entry.ea, entry.addr, _entrySize, 0, _rinfo);
            }
          }
        }

        // advance data position if the eviction buffer stores data
        if (data) data+= _entrySize;
      }
    }

    return continueSend;
  }

  BlockCriticalType get_block_critical_type(const CacheConfigMap &cm)
  {
    string bct = cm.get_str("block_on_critical_word");
    if (bct.empty()) {
      return bcwFalse;
    } else if (bct == "true") {
      return bcwTrue;
    } else if (bct == "false") {
      return bcwFalse;
    } else if (bct == "serialized") {
      return bcwSerialized;
    } else {
      RError("Unknown block-on-critical-word value:  " << bct);
    }
  }

  // The user specifies this as bits, but the cache model needs it in terms of
  // bytes, so we translate it here.
  unsigned getLinefillWidth(const CacheConfigMap &cm)
  {
    return cm.get_int("linefill_access_width") / 8;
  }

  // Cache
  Cache::Cache(Timer &timer, 
               const string &name, 
               UadlMemoryIf &memory,
               unsigned memoryPort, 
               unsigned lineWidth, 
               unsigned addrBandwidth,
               unsigned dataBandwidth, 
               unsigned queueSize, 
               bool allowBusWait,
               bool preemptive, 
               const CacheConfigMap &cm,
               bool withData) :
    Memory(timer, name, addrBandwidth, dataBandwidth, 
           cm.get_int("read_hit_latency"), cm.get_int("write_hit_latency"),
           queueSize, allowBusWait, preemptive, false, withData),
    _memory(memory), _memoryPort(memoryPort), _lineWidth(lineWidth),
    _writeThrough(cm.get_str("write_mode") == "WriteThrough"), 
    _fillReadLatency(cm.get_int("linefill_read_latency")),
    _fillWriteLatency(cm.get_int("linefill_write_latency")), 
    _zeroLatency(cm.get_int("zero_latency")),
    _taken2writeLatency(cm.get_int("taken_to_write_latency")),
    _write2nextLatency(cm.get_int("write_to_next_level_request_latency")),
    _linefillWritebackLatency(cm.get_int("linefill_writeback_latency")),
    _linefillLockCycle(cm.get_int("linefill_lock_cycle")), 
    _linefillLockHoldoffCycle(cm.get_int("linefill_lock_holdoff_cycle")),
    _linefillLockOnValid(cm.get_bool("linefill_lock_on_valid")),
    _consecutiveLinefills(cm.get_bool("consecutive_linefills")),
    _blockOnCriticalWord(get_block_critical_type(cm)),
    _evictLockCycle(cm.get_int("evict_lock_cycle")),
    _storeBufSize(cm.get_int("store_buffer_size")),
    _linefill(*this, memory, memoryPort, lineWidth, getLinefillWidth(cm), 
              cm.get_bool("critical_word_first"), cm.get_bool("streaming_linefill"), withData, 
              cm.get_bool("linefill_lazy_writeback"), cm.get_bool("dynamic_entry_size")),
    _evictBuf(*this, lineWidth, getLinefillWidth(cm), memory, memoryPort, withData, cm.get_bool("dynamic_entry_size")),
    _cacheStalls(0),
    _command(adl::CacheNoAccess), 
    _linefillRequestBlock(0),
    _canLookup(true), 
    _memDataBusBusy(false), 
    _serializedValid(true), 
    _lastMemTime(0), 
    _lastCritWordTime(0)
  {
    timer.register_dyn_parms(*this);

    // TODO: check: read latency no shorter than linefill read latency
    _commandLatency[adl::CacheFlush] = cm.get_int("invalidate_latency");
    _commandLatency[adl::CacheStore] = 0;
    _commandLatency[adl::CacheTouch] = cm.get_int("touch_latency");
    _commandLatency[adl::CacheAlloc] = 0;
    _commandLatency[adl::CacheInvalidate] = cm.get_int("invalidate_latency");
    _commandLatency[adl::CacheLock] = cm.get_int("lock_latency");
    _commandLatency[adl::CacheUnlock] = cm.get_int("unlock_latency");
  }

  bool Cache::set_dyn_parm(const std::string &parm,unsigned value)
  {
    if (parm == name()+"-cache-linefill-access-width") {
      if ( value <= 8 || !is_power_2(value)) {
        RError(parm << " must be larger than 8 and a power of 2.");
      }
      _linefill.setEntrySize(_lineWidth,value/8);
      _evictBuf.setEntrySize(_lineWidth,value/8);
      return true;
    } else {
      return false;
    }
  }
  
  bool Cache::get_dyn_parm(unsigned &value,const std::string &parm) const
  {
    if (parm == name()+"-cache-linefill-access-width") {
      value = _linefill.entrySize()*8;
      return true;
    } else {
      return false;
    }
  }
  
  void Cache::list_dyn_parm(adl::StrPairs &parms) const
  {
    parms.push_back(make_pair(name()+std::string("-cache-linefill-access-width"),name() + " width of access for each linefill operation."));
  }


  void Cache::createLogger(LogClient *lc,unsigned tflags) 
  {     
    if (!_logger) {
      _logger = new MemoryLogger(name(),_memory.name(),lc,tflags); 
      _linefill.attachLogger(*_logger);
      _evictBuf.attachLogger(*_logger);
    } else {
      _logger->setClient(lc,tflags);
    }
  }

  void Cache::removeLogger() { 
    _linefill.detachLogger();
    _evictBuf.detachLogger();
    delete _logger;
    _logger = 0; 
  }

  void Cache::set_level(unsigned level)
  {
    _level = level;
    _memory.set_level(level+1);
  }

  void Cache::reset()
  {
    // TODO: should cancel next-level transactions
    _linefill.clear();
    _evictBuf.clear();
    _storeBuf.clear();
    _linefillIds.clear();
    _actions.clear();

    _command = adl::CacheNoAccess;

    _canLookup = true;
    _lookupDisableTimes.clear();
    _lookupEnableTimes.clear();

    _cacheStalls = 0;

    Memory::reset();
  }

  bool Cache::isActive() const 
  { 
    return Memory::isActive() || !_linefill.quiet() || !_storeBuf.empty(); 
  }

  bool Cache::_getMemoryData(adl::CacheAccess type,addr_t addr, unsigned size,
                             byte_t *data, bool debug, bool cacheEnabled)
  {
    if (!_withData) return false;

    unsigned i = 0, dataSize;
    const byte_t *src;

    while (i < size) {
      if ((src = hasData(addr + i, dataSize))) {
        if (dataSize > size - i) dataSize = size - i;

        memcpy(data + i, src, dataSize);
        i += dataSize;
      }
      else if (debug) {                    // debug access
        if (_memory.getMemoryData(type, addr + i, 1, data + i)) i++;
        else return false;
      }
      else if (type == adl::CacheIFetch) { // non-debug fetch access
        if (_memory.fetchMemoryData(addr + i, 1, data + i, _exception_flag,
                                    cacheEnabled))
          { i++; }
        else
          { return false; }
      }
      else { // type == adl::CacheRead     // non-debug read access
        if (_memory.readMemoryData(addr + i, 1, data + i, _exception_flag,
                                   cacheEnabled))
          { i++; }
        else
          { return false; }
      }
    }

    return true;
  }


  bool Cache::_setMemoryData(addr_t addr, unsigned size, const byte_t *data,
                             bool debug, bool cacheEnabled)
  {
    if (!_withData) return false;

    unsigned i = 0, dataSize;
    byte_t *dest;

    while (i < size) {
      if ((dest = hasData(addr + i, dataSize))) {
        if (dataSize > size - i) dataSize = size - i;

        memcpy(dest, data + i, dataSize);
        i += dataSize;
      }
      else if (debug) { // debug access
        if (_memory.setMemoryData(addr + i, 1, data + i)) i++;
        else return false;
      }
      else {            // non-debug write access
        if (_memory.writeMemoryData(addr + i, 1, data + i, _exception_flag,
                                    cacheEnabled))
          { i++; }
        else
          { return false; }
      }
    }

    return true;
  }

  bool Cache::linefillBlockInquire(Logger *logger, unsigned serialized) const
  {
    switch (_blockOnCriticalWord) {
    case bcwTrue:
      if ( _linefill.criticalWordReceived()) {
        ++_cacheStalls;
        if (logger) {
          logger->logStall(name() + ": critical-word receive in progress.");
        }
        return false;
      }
      break;
    case bcwSerialized:
      if (serialized && (serialized & _memory.get_level_mask()) && _linefill.criticalWordReceived()) {
        ++_cacheStalls;
        if (logger) {
          logger->logStall(name() + ": critical-word receive in progress.");
        }
        return false;
      }
      break;
    default:
      break;
    }
    return true;
  }

  bool Cache::sendRequestInquire(InstrBase *instr,Logger *logger, MemRequestType type,
                                 unsigned serialized,unsigned portId) const
  {
    if (hasCommand()) {
      ++_cacheStalls;
      if (logger) {
        ostringstream ss;
        ss << name() << ": cache management command " << _command << " in process.";
        logger->logStall(ss.str());
      }

      return false;
    }
    else {
      if (!linefillBlockInquire(logger,serialized)) {
        return false;
      }
      return Memory::sendRequestInquire(instr, logger, type, serialized, portId);
    }
  }

  unsigned Cache::sendReadRequest(addr_t ea, addr_t ra, unsigned size,
                                  unsigned serialized, uint64_t &exception, const CacheStatusHint *hint,
                                  const ReqInfo &rinfo, unsigned portId)
  {
    const MemoryRequest &request = sendRequest(adl::CacheRead, ea, ra, size, serialized, 
                                               hint, false, rinfo, portId);

    if (logEnabled()) _logger->logReadRequest(request);
    
    return request.id;
  }

  unsigned Cache::sendWriteRequest(addr_t ea, addr_t ra, unsigned size, unsigned serialized,
                                   uint64_t &exception, const CacheStatusHint *hint,
                                   const byte_t *data, bool writeThrough,
                                   const ReqInfo &rinfo, unsigned portId, bool isEvict)
  {
    const MemoryRequest &request = sendRequest(adl::CacheWrite, ea, ra, size,
                                               serialized, hint, writeThrough, rinfo,
                                               portId, isEvict);

    if (logEnabled()) _logger->logWriteRequest(request);
    
    return request.id;
  }


  // TODO: reuse search information from requestInquire
  void Cache::read(unsigned portId, unsigned requestId, byte_t *data)
  {
    MemoryPort &port = getPort(portId);
    MemoryRequest &request = port.frontRequest();

    consumeDataBandwidth(portId);

    if (_withData && data) memcpy(data, request.data, request.dataWidth);

    operationEnd(request);

    if (logEnabled()) _logger->logReadAction(request,(_withData) ? request.data : 0);

    removeRequest(port);
  }


  bool Cache::writeInquire(InstrBase *instr,Logger *logger, unsigned portId, unsigned requestId) const
  {
    if (!Memory::writeInquire(instr, logger, portId, requestId)) return false;

    // At this point, we've already verified that the request is at the front,
    // so we can just grab it.
    const MemoryRequest &request = getPort(portId).frontRequest();

    if (request.useStoreBuf) {
      if (_storeBufSize && _storeBuf.size() > _storeBufSize) {
        ++_cacheStalls;
        if (logger) {
          logger->logStall(name() + ":  store buffer full.");
        }
        return false;
      }
    }

    // write miss, no allocation, not use store buffer
    if (request.action == adl::CacheNone && !request.useStoreBuf) {
      const NextLevelAction &action = _actions.front();

      // is this the next pending action?
      if (!(action.started && action.id == requestId &&
            action.type == STORE_NEXT)) {
        ++_cacheStalls;
        if (logger) {
          logger->logStall(name() + ": request pending");
        }

        return false;
      }

      // is the next-level memory ready for write?
      if (!_memory.writeInquire(instr, logger, _memoryPort, action.requestId)) {
        ++_cacheStalls;
        if (logger) {
          logger->logStall(name() + ": next-level memory not ready for write");
        }

        return false;
      }
    }

    return true;
  }

  Cache::NextLevelAction *Cache::findAction(uint64_t id)
  {
    for (unsigned i = 0; i != _actions.size(); ++i) {
      if (id == _actions[i].id) {
        return &(_actions[i]);
      }
    }
    return 0;
  }

  void Cache::write(unsigned portId,unsigned requestId, const byte_t *data)
  { 
    MemoryPort &port = getPort(portId);
    MemoryRequest &request = port.frontRequest();

    consumeDataBandwidth(portId);

    // update cache content
    if (data && request.data) memcpy(request.data, data, request.dataWidth);

    operationEnd(request);

    // set linefill buffer dirty flag
    if (_linefill.hasEntry(request.addr)) _linefill.setDirty();

    if (logEnabled()) _logger->logWriteAction(request, data);

    if (request.useStoreBuf) {
      // write to store buffer
      StoreBufEntry *s = _storeBuf.push_back();
      new (s) StoreBufEntry(request.ea, request.addr,
                            request.dataWidth, request.serialized, currTime(),
                            request.hint, _withData ? data : NULL,
                            request.writeThrough, request.rinfo);
      // At this point, the request is going away, so clear it from the
      // corresponding action.
      NextLevelAction *action = findAction(request.id);
      if (action) {
        action->request = 0;
      }
    }
    else if (request.action == adl::CacheNone) {
      // write miss, no allocation, not use store buffer
      const NextLevelAction &action = _actions.front();

      _memory.write(_memoryPort, action.requestId, data);
      _memDataBusBusy = true;
      _lastMemTime = currTime();

      if (logEnabled()) {
        _logger->logWriteNextAction(action.requestId, request.ea, request.addr,
                                    request.dataWidth, request.serialized, data, request.rinfo);
      }
      _lastMemTime = currTime();

      _actions.pop_front();
      _nActions--;
      // TODO: continue action data phase
    }
    removeRequest(port);
  }


  bool Cache::requestDoneInquire(Logger *logger, unsigned portId,unsigned requestId) const
  {
    bool done = Memory::requestDoneInquire(logger, portId, requestId);

    // check active linefill
    if (done && !_linefill.quiet() && requestId == _linefill.requestId())
      done = false;

    // check active eviction
    if (done && !_evictBuf.empty() && requestId == _evictBuf.requestId())
      done = false;

    // check other actions
    for (unsigned i = 0; done && i < _actions.size(); i++)
      if (requestId == _actions[i].id) done = false;

    if (!done) {
      ++_cacheStalls;
      if (logger) {
        std::ostringstream s;
        
        s << name() << ": request (id=" << requestId << ") is not completed yet";
        
        logger->logStall(s.str());
      }
    }

    return done;
  }

  bool Cache::nextLevelActionsEmptyInquire(Logger *logger) const
  {
    if (!_actions.empty()) {
      ++_cacheStalls;
      if (logger) {
        logger->logStall(name() + ": next-level-actions not empty");
      }
      return false;
    }
    return true;
  }

  // Returns false if there is a store buf entry with a matching instruction id.
  bool Cache::storeBufDoneInquire(Logger *logger,uint64_t instId) const
  {
    for (unsigned i = 0; i != _storeBuf.size(); ++i) {
      const StoreBufEntry &s = _storeBuf[i];
      if (s.rinfo._instId == (int64_t)instId) {
        ++_cacheStalls;
        if (logger) logger->logStall(name() + ": store-buffer entry not done");
        return false;
      }
    }
    return true;
  }
  
  bool Cache::commandInquire(Logger *logger) const
  {
    if (!_linefill.quiet()) {
      ++_cacheStalls;
      if (logger) logger->logStall(name() + ": linefill buffer not empty");
      
      return false;
    }
    else {
      return emptyInquire(logger);
    }
  }


  uint64_t Cache::command(MemRequestType type, addr_t ea, addr_t ra, int set,
                          int way, bool writeThrough, const ReqInfo &rinfo,
                          const CacheStatusHint *hint)
  {
    sendRequest(type, ea, ra, _lineWidth, 0, hint, writeThrough, rinfo);
    _command = type;

    MemoryRequest &request = defPort().backRequest();

    if (request.set < 0 && request.way < 0) {
      request.set = set;
      request.way = way;
    }

    if (logEnabled()) _logger->logCacheCommandBegin(request);

    return request.id;
  }


  void Cache::preCycle()
  {
    // save request queue full or not-full status
    saveQueueStatus();

    checkDelayedUnlock();

    if (!_lookupDisableTimes.empty() && currTime() == _lookupDisableTimes[0]) {
      _canLookup = false;
      if (logEnabled()) _logger->logMessage("Line-fill buffer lock cycle");
      _lookupDisableTimes.pop_front();
    }

    // Note: Enable comes after disable in case the times are the same; we don't
    // want a deadlock due to never re-enabling.
    if (!_lookupEnableTimes.empty() && currTime() == _lookupEnableTimes[0]) {
      _canLookup = true;
      _lookupEnableTimes.pop_front();
    }

    // next-level memory action: data phase
    bool continueAct = true;

    bool clearLinefill = false;

    _memDataBusBusy = false;
    _serializedValid = true;

    while (continueAct && !_actions.empty()) {
      NextLevelAction &action = _actions.front();

      if (action.type == LINEFILL) {
        if (action.started) {
          // A serialized access can occur if all requests have been sent and the
          // critical word has been loaded, since serialization is only relevant
          // from the core's perspective, i.e. non-critical words don't matter.
          _serializedValid = (_linefill.allRequestsSent() && _linefill.criticalWordsLoaded());

          if (_linefill.loadData()) {
            _memDataBusBusy = true;
          }

          if (_linefill.loaded()) {
            if (!action.completed) {
              action.completed = true;
              action.completeTime = currTime();
            }

            // Write linefill buffer to data array if we are in
            // immediate-writeback-mode.  If this LINEFILL has a parallel EVICT,
            // hold write-back until the EVICT starts.  Delay the clearing of the
            // linefill until after requests have been processed so that we
            // properly detect if the last request hit in the line-fill buffer.
            if (!_linefill.lazyWriteback()) {
              if (!action.hasParallelEvict || _actions[1].started) {
                clearLinefill = writeBackLinefill();
              }
            }
          }
        }

        // Continue if the linefill has completed (it's empty, will be cleared, or
        // we're in lazy-writeback-mode and the linefill has completed).
        continueAct = action.started && (clearLinefill || _linefill.quiet());
      }
      else if (action.type == EVICT) {
        if (action.started) {
          if (_evictBuf.pushData()) _memDataBusBusy = true;
          if (_evictBuf.evicted()) _evictBuf.clear();
        }

        continueAct = action.started && _evictBuf.empty();
      }
      else if (action.type == READ_NEXT) {
        continueAct = action.started && _memory.readInquire(0, 0, _memoryPort, action.requestId);

        if (continueAct) {
          MemoryRequest &request = *action.request;

          _memory.read(_memoryPort, action.requestId, request.buf);
          _memDataBusBusy = true;
          _lastMemTime = currTime();
          request.data = request.buf;
          request.expired = true;
          
          if (logEnabled()) {
            _logger->logReadNextAction(action.requestId, request.ea, request.addr,
                                       request.dataWidth, request.serialized, request.isInstAccess(),
                                       ((_withData) ? request.buf : 0), request.rinfo);
          }
        }
        if (action.started) {
          _serializedValid = false;
        }
      }
      else if (action.type == STORE_NEXT) {
        continueAct = action.useStoreBuf && action.started && _memory.writeInquire(0, 0, _memoryPort, action.requestId);

        if (continueAct) {
          const StoreBufEntry &entry = _storeBuf.front();

          _memory.write(_memoryPort, action.requestId, entry.data());
          _memDataBusBusy = true;
          _lastMemTime = currTime();

          if (logEnabled()) {
            _logger->logWriteNextAction(action.requestId, entry.ea, entry.addr,
                                        entry.dataWidth, 0, entry.data(),
                                        entry.rinfo);
          }

          assert(entry.ea == action.ea);
          _storeBuf.pop_front();
        }
        if (action.started) {
          _serializedValid = false;
        }
      }

      if (continueAct) {
        _actions.pop_front();
      }
    }

    // remaining current-cycle actions
    _nActions = _actions.size();
    if (_linefillRequestBlock) --_linefillRequestBlock;

    // process requests
    MemoryPort &port = defPort();
    for (unsigned i = 0; i < port.sizeRequests(); i++) {
      MemoryRequest &request = port.getRequest(i);

      if (request.expired) continue;

      const bool isNewRequest = !request.lookupDone;

      if (isNewRequest) {
        // do cache lookup for a new request
        // if (_canLookup || atLinefillBuf &&
        //     _linefill.readStreamPredicate(request))
        if (_canLookup) operationBegin(request);
        else break;
      }

      // check if the request hits the linefill buffer
      const bool atLinefillBuf = hitLinefillBuf(request) && !clearLinefill;

      /* TODO: Change the transactional ISS so that it knows "in-progress" states
       *       and does not report a miss while hitting on the linefill buffer.
       *       After that is done, this extra copy is no longer necessary */
      if (atLinefillBuf) {
        request.hitLinefillBuf = true;

        if (request.set != (int)_linefill.set() ||
            request.way != (int)_linefill.way())
          {
            /* Recover the ADL cache from a false lookup miss.  A complete recovery
             * would require to restore the old tag address.  This is a temporary
             * solution, so we just invalidate that cache line. */
            if (request.action == adl::CacheMiss ||
                request.action == adl::CacheEvict)
              { clearValid(request.set, request.way); }

            request.set = _linefill.set();
            request.way = _linefill.way();
          }

        // TODO: this shall not happen once the ISS knows "in-progress" states
        if (request.action == adl::CacheNone)
          request.offset = _linefill.addr2Offset(request.addr);
      }

      if (isNewRequest && (request.isRead() || request.isWrite())) {
        if (logEnabled()) {
          _logger->logCacheLookup(request);
        }
      }

      if (request.isRead()) {
        if (atLinefillBuf) {        // at linefill buffer
          if (_linefill.hasData(request.addr) &&
              timer().timeAtOrPassed(request.lookupTime + _fillReadLatency))
            {
              request.data = _linefill.data(request.addr);
              request.expired = true;
            }
        }
        else if (request.isHit()) { // read hit
          if (cacheLineHasData(request.set, request.way) &&
              timer().timeAtOrPassed(request.lookupTime + _readLatency))
            {
              request.data = cachePosition(request.set, request.way,
                                           request.offset);
              request.expired = true;
            }
        }
        else if (isNewRequest) {    // new read miss
          if (request.needLinefill()) {
            addLinefillAction(request);
          } else {
            NextLevelAction *na = _actions.push_back();
            new (na) NextLevelAction(READ_NEXT, request);
          }
        }
      }
      else if (request.isWrite() && !hasCommand()) {
        /* TODO: If a write miss with no allocation later hits a linefill, the
         *       write-through action will still be done.  We need better
         *       consistency between the ISS cache and Uadl cache. */
        /* write-through conditions:
         *   - write-miss non-allocation: request.action == adl::CacheNone
         *   - cache is write-through
         *   - request is write-through */
        const bool isWriteThrough = request.action == adl::CacheNone ||
          _writeThrough || request.writeThrough;
        const bool takenExpired = timer().timeAtOrPassed(request.takenTime +
                                                         _taken2writeLatency);

        if (atLinefillBuf) {               // at linefill buffer
          if (_linefill.hasData(request.addr) && takenExpired &&
              timer().timeAtOrPassed(request.time + _fillWriteLatency))
            {
              request.data = _linefill.data(request.addr);
              request.expired = true;
            }
        }
        else if (request.isHit()) {        // write hit
          if (cacheLineHasData(request.set, request.way) && takenExpired &&
              timer().timeAtOrPassed(request.time + _writeLatency))
            {
              request.data = cachePosition(request.set, request.way,
                                           request.offset);
              request.expired = true;
            }
        }
        else if (request.needLinefill()) { // write miss with allocation
          if (isNewRequest) addLinefillAction(request);
        }
        else {                             // write miss with no allocation
          if (takenExpired) {
            request.data = NULL;
            request.expired = true;
          }
        }

        if (isWriteThrough) {
          if (isNewRequest) {
            /* HACK: 'isHit() == true' means that the write data is first stored
             *       in the cache, then written to the next-level memory.
             *       Currently we always use _storeBuf for this because the
             *       implementation is simple.  Therefore, _storeBuf is not
             *       exactly the physical store buffer. */
            request.useStoreBuf = (request.hint.valid()
                                   ? request.hint._storeBufEnabled
                                   : storeBufEnabled()) || request.isHit();

            // register a STORE_NEXT action (after a potential LINEFILL action)
            if (!request.cancelled) {
              NextLevelAction *na = _actions.push_back();
              new (na) NextLevelAction(STORE_NEXT, request);
            }
          }
          else if (request.isHit() && !request.useStoreBuf) {
            // HACK: override 'useStoreBuf' in both request and action
            for (unsigned i = 0; !request.useStoreBuf && i < _actions.size(); i++) {
              if (_actions[i].id == request.id && _actions[i].type == STORE_NEXT)
                request.useStoreBuf = _actions[i].useStoreBuf = true;
            }
          }
        }
      }
      else if (request.type == adl::CacheWrite) { // CacheZero
        if (isNewRequest && request.needEviction()) {
          NextLevelAction *na = _actions.push_back();
          new (na) NextLevelAction(EVICT, request);
        }

        if (request.action != adl::CacheNone) { // zero cache
          // if need EVICT, do not zero until the EVICT has started
          if (timer().timeAtOrPassed(request.time + _zeroLatency) &&
              (!request.needEviction() || _actions.empty() ||
               _actions.back().started)) {
            if (_withData) memset(cachePosition(request.set, request.way, 0), 0, _lineWidth);
            endCommand();
          }
        }
        else {                                  // zero next-level memory
          addr_t ea = request.ea - _linefill.addr2Offset(request.addr);
          addr_t ra = request.addr - _linefill.addr2Offset(request.addr);;
          uint64_t data = 0;

          request.useStoreBuf = true;

          // NOTE: Do not use "ea < ea + _lineWidth" as loop termination
          //       condition because it may wrap-around
          //
          // NOTE: We use the linefill-access width for the store buffer size.
          //       Is this correct, or should we be using the datawidth of the
          //       next-level memory?  I think this is more intuitive, as you
          //       set the linefill-access-width once and it applies to loads
          //       (linefill operations) and stores.
          for (unsigned i = 0; i < _lineWidth / _linefill.entrySize();
               i++, ea += _linefill.entrySize(), ra += _linefill.entrySize())
            {
              NextLevelAction *na = _actions.push_back();
              new (na) NextLevelAction(STORE_NEXT, request, ea, ra);
              StoreBufEntry *s = _storeBuf.push_back();
              new (s) StoreBufEntry(ea, ra, _linefill.entrySize(),
                                    request.serialized, currTime(), request.hint,
                                    _withData ? (byte_t*)&data : NULL,
                                    request.writeThrough,
                                    request.rinfo);
            }

          endCommand();
        }
      }
      else if (request.type == adl::CacheTouch) { // CacheTouch
        if (isNewRequest && request.needLinefill()) addLinefillAction(request);

        if ((request.isHit() || !request.needLinefill()) &&
            timer().timeAtOrPassed(request.time + _commandLatency[request.type]))
          { endCommand(); }
      }
      else {                                      // all other cache commands
        if (isNewRequest && request.needEviction()) {
          NextLevelAction *na = _actions.push_back();
          new (na) NextLevelAction(EVICT, request);
        }

        if (timer().timeAtOrPassed(request.time + _commandLatency[request.type]))
          {
            if (request.type == adl::CacheAlloc && request.action != adl::CacheNone)
              enableCacheLineData(request.set, request.way);

            endCommand();
          }
      }

      // one lookup per time step
      if (isNewRequest) break;
    }

    if (clearLinefill) {
      _linefill.clear();
    }

    // remove expired-and-cancelled requests from the header of the queue
    while (port.hasRequests() && port.frontRequest().expired && port.frontRequest().cancelled) {
      operationEnd(port.frontRequest());
      removeRequest(port);
    }
  }

  // Returns true if we're serialized and can proceed.  This checks linefills and
  // their critical-word status.
  bool Cache::check_serialized(unsigned serialized)
  {
    if (serialized & _memory.get_level_mask()) {
      // Yes, we're serialized.  If a non-serialized action occurred, then it will have set the
      // _serialized_valid flag to false.
      return _serializedValid;
    }
    return true;
  }
  

  void Cache::postCycle()
  {
    // next-level memory action: request phase
    bool continueAct = true;
    unsigned storeBufId = 0;

    //   if (logEnabled()) {
    //     ostringstream s;
    //     s << name() << ":  Request slots available:  " << nAvailableRequestSlots();
    //     _logger->logMessage(s.str());
    //   }
    for (unsigned i = 0; i < _nActions && continueAct; i++) {
      NextLevelAction &action = _actions[i];

      if (action.type == LINEFILL) {
        bool was_valid = !_linefill.empty();
        if (!action.started) {
          // If we do lazy writebacks, then do the writeback now if necessary.
          if (_linefill.lazyWriteback() && !_linefill.empty() && _linefill.loaded()) {
            if (writeBackLinefill()) {
              _linefill.clear();
            }
          }
          if (_linefill.empty()) {
            _linefill.init(action.rinfo, action.id, action.ea, action.addr,
                           action.set, action.way,
                           action.requestType != adl::CacheIFetch,
                           action.requestTime, action.replaceLine);
            _linefillIds.pop_front();
            action.started = true;
          } else if (_consecutiveLinefills && _linefill.allRequestsSent() && !_linefill.hasExtraEntry()) {
            // If possible, start up the next linefill request before everything
            // is done with the first one.
            if (!_linefillRequestBlock) {
              _linefill.setupExtra(action.ea,action.addr);
            }
          }
        }

        const bool freshLinefill = action.started && !_linefill.started();

        continueAct = action.started && _linefill.sendRequest();
        if (_linefill.hasExtraEntry()) {
          continueAct = _linefill.sendExtraRequest();
        }

        if (freshLinefill && _linefill.started()) {
          // set up lookup lock time
          ttime_t lookupLockTime = _linefillLockCycle +
            (action.hasParallelEvict
             ? _evictLockCycle : 0);

          // If this cache experiences locks only when the linefill was valid, and
          // we're not in that situation, then ignore the lock-time.
          if (_linefillLockOnValid && !was_valid) {
            lookupLockTime = 0;
          }

          if (lookupLockTime) {
            // TODO: move lookup to taken time, remove "+ 1", restore const
            ttime_t lookupDisableTime = currTime() + _linefillLockHoldoffCycle;
            const ttime_t lookupEnableTime = lookupDisableTime + lookupLockTime;

            // HACK: zen650-specific rule
            if (_linefillLockHoldoffCycle == 1 && !action.replaceLine && !_memDataBusBusy) {
              lookupDisableTime++;
            }

            if (_lookupEnableTimes.empty() ||
                lookupDisableTime > _lookupEnableTimes.back())
              {      // no overlap
                _lookupDisableTimes.push_back(lookupDisableTime);
                _lookupEnableTimes.push_back(lookupEnableTime);
              }
            else { // overlap
              if (lookupEnableTime > _lookupEnableTimes.back())
                _lookupEnableTimes.back() = lookupEnableTime;
            }
          }
        }
      }
      else if (action.type == EVICT) {
        if (!action.started && _evictBuf.empty()) {
          // FIXME: use real ea
          _evictBuf.init(action.rinfo, action.id, 0,
                         getTagAddr(action.set, action.way),
                         cachePosition(action.set, action.way, 0));

          action.started = true;
        }

        continueAct = action.started && _evictBuf.sendRequest();
      }
      else if (action.type == READ_NEXT) {
        const MemoryRequest *request = action.request;

        // Interesting point here: We can't just use sendRequestInquire to check
        // serialization b/c of the usual behavior with linefills.  If a linefill
        // is in progress, but the critical words have been fetched, then we can
        // proceed.  Thus, we need to do our own checking.
        if (!action.started && 
            _memory.sendRequestInquire(0, 0, request->type, 0, _memoryPort) && 
            check_serialized(request->serialized))
          {
            if (request->type == adl::CacheRead) {
              action.requestId = _memory.sendReadRequest(request->ea, request->addr,
                                                         request->dataWidth, request->serialized,
                                                         _exception_flag,
                                                         request->rinfo,
                                                         _memoryPort);
            }
            else { // request->type == adl::CacheIFetch
              action.requestId = _memory.sendFetchRequest(request->ea,
                                                          request->addr,
                                                          request->dataWidth, request->serialized,
                                                          _exception_flag, 0, request->rinfo, _memoryPort);
            }

            action.started = true;

            if (logEnabled()) {
              _logger->logReadNextRequest(action.requestId, request->ea,
                                          request->addr, request->dataWidth, request->serialized,
                                          request->isInstAccess(), request->rinfo);
            }
          }

        continueAct = action.started;
      }
      else if (action.type == STORE_NEXT) {

        const unsigned serialized = (action.useStoreBuf && storeBufId < _storeBuf.size()) ? _storeBuf[storeBufId].serialized : action.request->serialized;
        if (!action.started &&
            (!action.useStoreBuf || (_storeBuf.size() > storeBufId &&
             timer().timeAtOrPassed(_storeBuf[storeBufId].time +
                                    _write2nextLatency))) &&
            _memory.sendRequestInquire(0, 0, adl::CacheWrite, serialized, _memoryPort))
          {
            const addr_t ea = action.useStoreBuf ? _storeBuf[storeBufId].ea
              : action.request->ea;
            const addr_t ra = action.useStoreBuf ? _storeBuf[storeBufId].addr
              : action.request->addr;
            const unsigned size = action.useStoreBuf ? _storeBuf[storeBufId].dataWidth
              : action.request->dataWidth;
            const CacheStatusHint &hint = action.useStoreBuf
              ? _storeBuf[storeBufId].hint
              : action.request->hint;
            const byte_t* const data = action.useStoreBuf
              ? _storeBuf[storeBufId].data() : NULL;
            const bool writeThrough = action.useStoreBuf
              ? _storeBuf[storeBufId].writeThrough
              : action.request->writeThrough;
            const ReqInfo &rinfo = action.useStoreBuf
              ? _storeBuf[storeBufId].rinfo
              : action.request->rinfo;

            action.requestId = _memory.sendWriteRequest(ea, ra, size, serialized, _exception_flag,
                                                        &hint, data, writeThrough,
                                                        rinfo, _memoryPort);
            action.started = true;

            if (logEnabled())
              _logger->logWriteNextRequest(action.requestId, ea, ra, size, serialized, rinfo);
          }

        continueAct = action.started;

        if (action.useStoreBuf) storeBufId++;
      }
    }
  }

  // If a cache has a lazy-writeback linefill buffer, then flush it if loaded, if
  // we're at the end of simulation, in order to make sure that our cache lines
  // are up to date for final reporting.
  void Cache::postSim()
  {
    if (_linefill.lazyWriteback() && !_linefill.empty() && _linefill.loaded()) {
      dumpLinefill();
    }
  }

  void Cache::logAction(const NextLevelAction &action) const
  {
    ostringstream ss;
    ss << "action: ";
    switch (action.type) {
    case LINEFILL:
      ss << "linefill";
      break;
    case EVICT:
      ss << "evict";
      break;
    case READ_NEXT:
      ss << "read-next";
      break;
    case STORE_NEXT:
      ss << "store-next";
      break;
    default:
      break;
    }
    if (action.started) {
      ss << ", started";
    }
    if (action.completed) {
      ss << ", completed";
    }
    if (MemoryRequest *request = action.request) {
      ss << ":  ";
      int expires = ( request->time + request->latency - timer().currTime() );
      _logger->logRequest(ss.str(),expires,*request);
    } else {
      ss << '(';
      Logger::requestReportInner(ss,action.rinfo,action.id,action.ea,action.addr,0,false);
      ss << ')';
      _logger->logMessage(ss.str());
    }
  }

  // In addition to logging all requests, we also log all actions.
  void Cache::logAllRequests() const
  {
    Memory::logAllRequests();
    if (_logger) {
      bool first = true;
      for (unsigned i = 0; i < _actions.size(); ++i) {
        if (first) {
          _logger->logMessage("----");
          first = false;
        }
        logAction(_actions[i]);
      }
    }
  }

  /*******************************************************************************
   * NOTE:
   *   - A started LINEFILL or EVICT cannot be cancelled because the cache status
   *     has been altered.
   *   - An EVICT caused by a LINEFILL can only be cancelled together with the
   *     LINEFILL.  If the LINEFILL has been started, the EVICT cannot be
   *     cancelled even if it is not started, because the cache line will be
   *     overridden.
   *   - For this reason, an EVICT caused by a LINEFILL always has request = NULL.
   ******************************************************************************/
  void Cache::cancelNextLevelRequest(const MemoryRequest &request)
  {
    unsigned nActions = _nActions;

    // this is a slow algorithm, but cancel() should not occur often
    for (unsigned count = 0; count != _actions.size(); ++count) {
      /* We must compare request id rather than address of the request object
       * because a stale request pointer might point to a new request object. */
      const NextLevelAction &a = _actions[count];
      if (a.id == request.id) {
        // cancel next-level memory request
        if ((a.type == READ_NEXT || a.type == STORE_NEXT) && a.started) {
          if (logEnabled()) {
            _logger->logCancelNext(a.requestId, request.ea, request.addr,
                                   request.dataWidth, request.rinfo);
          }

          /* TODO: Whether capable of doing deep cancellation should be a model
           *       parameter.  For now, we always assume deep cancellation */
          _memory.cancel(_memoryPort,a.requestId,true);
        }

        // cancel the action
        // if (!i->started || (i->type == READ_NEXT || i->type == STORE_NEXT)) {
        if (a.type == READ_NEXT || a.type == STORE_NEXT) {
          _actions.erase(count);

          if (count < nActions) _nActions--;

          // Since we just erased count, _actions[count] now refers to the new
          // item, so we don't want to increment past it for the next loop
          // iteration.
          --count;

        }
      }
    }
  }

  // After cancelling a request in the cache, we have to go back and fixup our
  // action request pointers, since the requests themselves might have moved in
  // the circular queue.
  void Cache::postCancelRequest(MemoryPort &port,unsigned requestId)
  {
    unsigned nActions = _nActions;
    // For each action, find the corresponding id in the request array.
    for (unsigned i = 0; i != _actions.size(); ++i) {
      NextLevelAction &action = _actions[i];
      // Note: Result might be 0 because we don't remove linefill actions, so
      // it's possible for their requests to be deleted but the action remains.
      action.request = port.findRequest(action.id);
      if (action.type != LINEFILL && action.type != STORE_NEXT && action.request == 0) {
        _actions.erase(i);
        if (i < nActions) _nActions--;
        --i;
      }
    }
  }

  byte_t* Cache::hasData(addr_t addr, unsigned &size) const
  {
    if (!_withData) return NULL;

    // check linefill buffer
    if (_linefill.hasData(addr)) {
      size = _linefill.entrySize() - addr % _linefill.entrySize();

      return _linefill.data(addr);
    }

    // check data array
    unsigned set, way, offset;

    if (lookup(addr, set, way, offset) && cacheLineHasData(set, way)) {
      size = _lineWidth - offset;

      return cachePosition(set, way, offset);
    }

    // check store buffer
    for (int i = _storeBuf.size() - 1; i >= 0; i--) {
      const StoreBufEntry &entry = _storeBuf[i];

      if (addr >= entry.addr && addr < entry.addr + entry.dataWidth) {
        size = entry.dataWidth - (addr - entry.addr);

        return const_cast<byte_t*>(entry.data() + (addr - entry.addr));
      }
    }

    // check push buffer
    if (!_evictBuf.empty() && addr >= _evictBuf.addr() &&
        addr < _evictBuf.addr() + _lineWidth)
      {
        size = _lineWidth - (addr - _evictBuf.addr());

        return const_cast<byte_t*>(_evictBuf.data() + (addr - _evictBuf.addr()));
      }

    // data not in the cache
    return NULL;
  }


  void Cache::addLinefillAction(MemoryRequest &request)
  {
    disableCacheLineData(request.set, request.way);
    NextLevelAction *na = _actions.push_back();
    new (na) NextLevelAction(LINEFILL, request, request.needEviction());
    _linefillIds.push_back(request.id);

    if (request.needEviction()) {
      NextLevelAction *na = _actions.push_back();
      new (na) NextLevelAction(EVICT, request);
    }
  }


  // Returns true if we should clear the line-fill buffer.
  bool Cache::writeBackLinefill()
  {
    unsigned set = _linefill.set();
    unsigned way = _linefill.way();
    bool overloaded = false;
    bool requestsPending = false;

    MemoryPort &port = defPort();

    // check if overloaded: pending LINEFILLs fall into the same set/way
    for (unsigned i = 1; i < _actions.size() && !overloaded; i++) {
      overloaded = _actions[i].type == LINEFILL && _actions[i].set == set && _actions[i].way == way;
    }

    if (overloaded) {
      // check if pending requests fall into the linefill buffer
      for (unsigned i = 0; i < port.sizeRequests() && !requestsPending; i++)
        requestsPending = hitLinefillBuf(port.getRequest(i));
    }
    else {
      // copy linefill buffer to data array
      setCacheData(set, way, 0, _lineWidth, _linefill.data(0));
      enableCacheLineData(set, way);

      // adjust requests that hit linefill buffer
      for (unsigned i = 0; i < port.sizeRequests(); i++) {
        MemoryRequest &request = port.getRequest(i);

        if (hitLinefillBuf(request)) {
          if (request.expired) {
            /* calculate offset rather than use request.offset because the request
             * may have CacheNone action code and has no offset calculated */
            request.data = cachePosition(set, way,
                                         _linefill.addr2Offset(request.addr));
          }
        }
      }
    }

    if (logEnabled()) _logger->logMessage("Line-fill buffer writeback");

    if (!requestsPending) {
      return true;
    } else {
      _linefill.setLocked();
      return false;
    }
  }

  void Cache::endCommand()
  {
    MemoryPort &port = defPort();
    const MemoryRequest &request = port.frontRequest();

    if (logEnabled()) _logger->logCacheCommandEnd(request);

    // i->expired = true;

    operationEnd(request);
    removeRequest(port);
    _command = adl::CacheNoAccess;
  }


  // For end-of-simulation purposes, dump the linefill back to the cache.
  void Cache::dumpLinefill()
  {
    unsigned set = _linefill.set();
    unsigned way = _linefill.way();
    
    setCacheData(set, way, 0, _lineWidth, _linefill.data(0));
    enableCacheLineData(set, way);
  }
  
  bool Cache::hitLinefillBuf(const MemoryRequest &request) const
  {
    return _linefill.hasEntry(request.addr) && (_linefillIds.empty() ||
                                                Counter::isLesser(request.id, _linefillIds.front()));
  }


  Cache::NextLevelAction::NextLevelAction(NextLevelActionType _type,
                                          MemoryRequest &_request,
                                          bool _hasParallelEvict) :
    type(_type), rinfo(_request.rinfo), id(_request.id), request(&_request),
    hasParallelEvict(_hasParallelEvict), useStoreBuf(_request.useStoreBuf),
    started(false), completed(false)
  {
    /* copy necessary request fields for LINEFILL, EVICT and STORE_NEXT because the request
     * may have been cancelled by the time the action is taken */
    if (type == LINEFILL || type == EVICT) {
      requestType = request->type;
      ea = request->ea;
      addr = request->addr;
      set = request->set;
      way = request->way;

      if (type == LINEFILL) {
        requestTime = request->time;
        replaceLine = request->replaceLine;
      }

      // For safety- don't reference the request, since it may no longer be
      // valid.
      request = 0;
    } else if (type == STORE_NEXT) {
      // Note: Keep the request pointer for now; we'll clear it on the write,
      // when the request is de-allocated.  We do have to update address
      // information so that we can check it later, via an assertion, to make
      // sure we're not out of sync.
      ea = request->ea;
      addr = request->addr;
    }
  }

  // This form is only used for when we create multiple entries, such as for a
  // cache-zeroing operation.  This allows us to explicitly set the effective
  // address, in order to keep things consistent.  This should only be used for
  // STORE_NEXT operations and will assert otherwise.
  Cache::NextLevelAction::NextLevelAction(NextLevelActionType _type,
                                          MemoryRequest &_request,
                                          addr_t ea_p,addr_t ra_p,
                                          bool _hasParallelEvict) :
    type(_type), rinfo(_request.rinfo), id(_request.id), request(&_request),
    hasParallelEvict(_hasParallelEvict), useStoreBuf(_request.useStoreBuf),
    started(false), completed(false)
  {
    assert (type == STORE_NEXT);

    // Note: Keep the request pointer for now; we'll clear it on the write,
    // when the request is de-allocated.  We do have to update address
    // information so that we can check it later, via an assertion, to make
    // sure we're not out of sync.
    ea = ea_p;
    addr = ra_p;
  }

} // namespace uadl

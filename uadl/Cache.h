//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
#ifndef _CACHE_H
#define _CACHE_H

#include <assert.h>
#include <string.h>
#include <vector>
#include <string>
#include <helpers/BasicTypes.h>
#include <helpers/BitTwiddles.h>

#include "DynParms.h"
#include "UadlMemory.h"

using std::vector;
using std::string;
using adl::byte_t;
using adl::CacheRead;
using adl::CacheWrite;
using adl::CacheFlush;
using adl::CacheInvalidate;

// Set to true to have all circular queues do range-checking.
#define CacheQueueDebugCheck false

namespace uadl {

  // This class is used for configuring the cache model, or a user's custom
  // cache model.
  struct CacheConfigItem {
    std::string _str;
    int         _val;

    CacheConfigItem(const std::string &str) : _str(str), _val(-1) {};
    CacheConfigItem(int val) : _val(val) {};

    const std::string &str() const { return _str; };
    int val() const { return _val; };

    bool is_str() const { return !_str.empty(); };
    bool is_int() const { return _str.empty(); };
  };

  struct CacheConfigMap : public std::map<std::string,CacheConfigItem>
  {
    typedef std::map<std::string,CacheConfigItem> Base;

    CacheConfigMap(const Base::value_type *b,const Base::value_type *e) : Base(b,e) {};

    bool get_bool(const std::string &key,bool dval = false) const;
    int get_int(const std::string &key,int dval = 0) const;
    std::string get_str(const std::string &key,const std::string &dval = std::string()) const;
  };

  class Cache;

  // TODO: write overwrite/cancel a pending read
  class LinefillBuf {
  public:
    LinefillBuf(Cache &cache, UadlMemoryIf &memory, unsigned memoryPort,
                unsigned nEntries, unsigned entrySize, bool criticalWordFirst,
                bool streaming, bool withData, bool lazyWriteback, bool dynEntrySize);

    ~LinefillBuf() { delete _data; }

    void setEntrySize(unsigned lineWidth, unsigned entrySize);

    void init(const ReqInfo &rinfo, uint64_t requestId, addr_t ea, addr_t ra,
              unsigned set, unsigned way, bool readData, ttime_t requestTime,
              bool replaceLine);
    bool setupExtra(addr_t ea,addr_t ra);
    void clear() { _empty = true; }

    // access/query methods
    bool criticalWordFirst() const { return _criticalWordFirst; }
    bool lazyWriteback() const { return _lazyWriteback; };
    unsigned set() const { return _set; }
    unsigned way() const { return _way; }
    addr_t lineAddr() const { return _entries[0].addr; }
    unsigned entrySize() const { return _entrySize; }

    byte_t *data(addr_t addr) const;
    unsigned addr2Offset(addr_t addr) const { return addr & _byteOffsetMask; }
    ttime_t startTime() const { return _startTime; };
    unsigned requestsRemaining() const { return _requestsRemaining; };

    bool empty() const { return _empty; }
    bool quiet() const { return (empty() || (_lazyWriteback && loaded())); }
    bool started() const { return !empty() && _started; }
    bool loaded() const;
    uint64_t requestId() const { return _requestId; }

    bool criticalWordsLoaded() const { return loaded() || (criticalWordFirst() && _entries[_critical_entry].hasData); };
    bool criticalWordReceived() const { return _criticalWordReceived; };
    void clearCriticalWordReceived() { _criticalWordReceived = false; };

    void setLocked() { _locked = true; }

    bool hasEntry(addr_t addr) const;
    bool hasData(addr_t addr) const;
    const vector<addr_t> criticalAddrs() const;

    bool dirty() const { return _dirty; }
    void setDirty() { _dirty = true; }

    bool loadData();
    bool sendRequest();
    bool sendExtraRequest();

    bool allRequestsSent() const { return _allsent; };

    bool hasExtraEntry() const { return _hasExtra; };
    
    void addCriticalAddr(addr_t addr);

    // logging methods
    void attachLogger(MemoryLogger &logger) { _logger = &logger; }
    void detachLogger() { _logger = 0; }

    bool readStreamPredicate(const MemoryRequest &request);

  private:
    const bool     _withData;        // If buffer stores actual data (non-safe-mode).

    Cache        &_cache;
    UadlMemoryIf &_memory;           // next-level memory
    unsigned _memoryPort;            // next-level memory port
    MemoryLogger *_logger;           // memory logger

    const bool _criticalWordFirst;
    adl::DSCircularQueue<unsigned,CacheQueueDebugCheck> _criticalIds;     // critical entry id's

    const bool _streaming;
    const bool _lazyWriteback;        // Whether we writeback when loaded or what for next request.
    const bool _dynEntrySize;         // True => entry size can change dynamically during execution.

    unsigned _lineWidth;              // Cached here in case we need it.
    unsigned _nEntries;               // Maximum number of entries.  Not constant b/c can be modified by dynamic parms.
    unsigned _entrySize;              // Size of each access.  Not constant b/c
                                      // can be modified by dynamic parms.
    addr_t   _lineAddrMask;           // line address mask
    addr_t   _entryOffsetMask;        // mask to retrieve line-fill entry from an address.
    unsigned _entryShift;             // shift required to retrieve index of line-fill entry from address.
    addr_t   _byteOffsetMask;         // byte offset mask

    ReqInfo  _rinfo;                 // Instruction info which generated the linefill request.
    uint64_t _requestId;
    unsigned _set;
    unsigned _way;
    bool _readData;   // true if reading data, false if reading instructions
    bool _empty;
    bool _dirty;
    bool _locked;     // True if the linefill buffer cannot be written back to
    // data array due to overloaded line allocation and pending
    // requests that hit the linefill buffer.
    unsigned _critical_entry;         // index of true critical entry.

    mutable bool _loaded;     // caches whether the line-fill is loaded.

    bool _started;
    bool _allsent;
    ttime_t _startTime; // time when reading from next-level memory starts
    
    bool     _criticalWordReceived;
    unsigned _requestsRemaining;

    struct LinefillBufEntry {
      LinefillBufEntry() : data(NULL), hasData(false), requestSent(false) {}

      void init(addr_t _ea, addr_t _ra);

      addr_t ea;
      addr_t addr;
      byte_t *data;

      bool hasData;
      bool requestSent;
      uint64_t requestId;
    };

    unsigned addr2EntryId(addr_t addr) const;
    bool loadEntryData(LinefillBufEntry &entry, bool &dataRead);
    bool sendEntryRequest(LinefillBufEntry &entry);

    vector<LinefillBufEntry> _entries;
    byte_t *_data;

    // For supporting consecutive linefills.  This is an extra request that can
    // exist before all data for the existing linefill comes back.  
    // TODO: Generalize to n entries, or perhaps duplicate the linefill guts.
    bool             _hasExtra;
    LinefillBufEntry _extraEntry;
    
    // for zen650 linefill streaming hack
    ttime_t _requestTime;
    bool _replaceLine;
    bool _streamed;
  };


  inline byte_t* LinefillBuf::data(addr_t addr) const
  { return _data ? &_data[addr2Offset(addr)] : NULL; }


  inline bool LinefillBuf::hasEntry(addr_t addr) const
  { return !empty() && (addr & _lineAddrMask) == lineAddr(); }


  inline bool LinefillBuf::hasData(addr_t addr) const
  { return hasEntry(addr) && _entries[addr2EntryId(addr)].hasData; }


  inline void LinefillBuf::addCriticalAddr(addr_t addr)
  // TODO: test hardware's critical_word_first behavior
  // { _criticalIds.push_back(addr2EntryId(addr)); }
  {
    const unsigned ci = addr2EntryId(addr);

    // Record true critical entry.
    _critical_entry = ci;

    // add critical word id
    _criticalIds.push_back(ci);
    // add "after" ids
    for (unsigned i = ci + 1; i < _nEntries; i++) _criticalIds.push_back(i);
    // add "before" ids
    for (unsigned i = 0; i < ci; i++) _criticalIds.push_back(i);
  }


  inline unsigned LinefillBuf::addr2EntryId(addr_t addr) const
  { return (addr & _entryOffsetMask) >> _entryShift; }


  inline void LinefillBuf::LinefillBufEntry::init(addr_t _ea, addr_t _ra)
  { ea = _ea; addr = _ra; hasData = requestSent = false; }



  class EvictBuf {
  public:
    EvictBuf(Cache &cache, unsigned lineWidth, unsigned entrySize, UadlMemoryIf &memory,
             unsigned memoryPort, bool withData, bool dynEntrySize) :
      _cache(cache), _withData(withData), _dynEntrySize(dynEntrySize), 
      _lineWidth(lineWidth), _memory(memory), _memoryPort(memoryPort), 
      _logger(0), _data(0), _hasData(false)
    { 
      setEntrySize(lineWidth,entrySize);
    }

    ~EvictBuf() { delete _data; }

    void setEntrySize(unsigned lineWidth, unsigned entrySize);

    void init(const ReqInfo &rinfo, uint64_t requestId, addr_t ea, addr_t ra,
              const byte_t *data);
    void clear() { _hasData = false; }

    // access/query methods
    bool empty() const { return !_hasData; }
    bool evicted() const;
    uint64_t requestId() const { return _requestId; }
    addr_t addr() const { return _entries[0].addr; }
    const byte_t *data() const { return _data; }

    bool pushData();
    bool sendRequest();

    // logging methods
    void attachLogger(MemoryLogger &logger) { _logger = &logger; }
    void detachLogger() { _logger = 0; }

  private:
    struct EvictBufEntry {
      void reset(addr_t _ea, addr_t _ra);

      addr_t ea;
      addr_t addr;

      bool requestSent;
      bool dataSent;
      uint64_t requestId;
    };

    Cache        &_cache;

    const bool     _withData;
    const bool     _dynEntrySize; // True => Entry size can change dynamically
                                  // during execution.

    unsigned _lineWidth;         // Cached for use if we support dynamic entry resizing.
    unsigned _nEntries;          // Not constant b/c can be modified by dynamic parameters.
    unsigned _entrySize;         // Not constant b/c can be modified by dynamic parameters.

    UadlMemoryIf &_memory;       // next-level memory
    unsigned      _memoryPort;   // next-level memory port
    MemoryLogger *_logger;       // memory logger

    vector<EvictBufEntry> _entries;
    byte_t *_data;
    bool _hasData;
    ReqInfo  _rinfo;             // Info from instruction which generate the eviction.
    uint64_t _requestId;
  };


  inline void EvictBuf::EvictBufEntry::reset(addr_t _ea, addr_t _ra)
  { ea = _ea, addr = _ra; requestSent = dataSent = false; }

  enum BlockCriticalType { bcwFalse, bcwSerialized, bcwTrue };

  /*******************************************************************************
   * cache (with multi-cycle read/write latency)
   *
   * NOTE:
   *   - see TransactionCodeGen.C::CacheCodeGen::classDef() for default
   *     constructor parameters
   *   - assume no read/write request spans over cache line boundary
   *   - lineWidth must be a power of 2
   *
   * TODO:
   *   - protect an evicted line from being written before it is copied to
   *     eviction buffer
   *   - check: _lineWidth | 8
   *   - remove flush latency
   ******************************************************************************/
  class Cache : public Memory, public DynParms {
  public:
    Cache(Timer &timer, const string &name, UadlMemoryIf &memory,
          unsigned memoryPort, unsigned lineWidth, unsigned addrBandwidth,
          unsigned dataBandwidth, unsigned queueSize,
          bool allowBusWait, bool preemptive, const CacheConfigMap &configmap,
          bool withData = true);

    virtual ~Cache() {}
    
    virtual void reset();

    // untimed debug access interface
    virtual bool getMemoryData(adl::CacheAccess type,addr_t addr, unsigned size,
                               byte_t *data);
    virtual bool setMemoryData(addr_t addr, unsigned size, const byte_t *data);

    // untimed access interface
    virtual bool fetchMemoryData(addr_t addr, unsigned size, byte_t *data,
                                 uint64_t &exception, bool cacheEnabled = false);
    virtual bool readMemoryData(addr_t addr, unsigned size, byte_t *data,
                                uint64_t &exception, bool cacheEnabled = false);
    virtual bool writeMemoryData(addr_t addr, unsigned size, const byte_t *data,
                                 uint64_t &exception, bool cacheEnabled = false);

    virtual bool sendRequestInquire(InstrBase *instr, Logger *logger, MemRequestType type,
                                    unsigned serialized,unsigned portId = 0)
      const;

    // These exist primarily for safe-mode: We track a pointer back to the OSM so
    // that cache status can be retrieved.
    unsigned sendReadRequest(addr_t ea, addr_t ra, unsigned size,
                             unsigned serialized, uint64_t &exception,
                             const CacheStatusHint *hint,
                             const ReqInfo &rinfo, unsigned portId = 0) override;
    unsigned sendWriteRequest(addr_t ea, addr_t ra, unsigned size,
                              unsigned serialized, uint64_t &exception,
                              const CacheStatusHint *hint,
                              const byte_t *data = NULL,
                              bool writeThrough = false, const ReqInfo &rinfo = ReqInfo(),
                              unsigned portId = 0, bool isEvict = false) override;
    
    // transaction interface
    virtual void read(unsigned portId, unsigned requestId, byte_t *data);
    virtual bool writeInquire(InstrBase *instr,Logger *logger, unsigned portId, unsigned requestId) const;
    virtual void write(unsigned portId, unsigned requestId, const byte_t *data);

    virtual bool requestDoneInquire(Logger *logger, unsigned portId, unsigned requestId) const;

    // The stalls are the sum off cache-specific stalls plus general memory
    // object related stalls.
    unsigned memStalls(unsigned portId) const { return _cacheStalls + Memory::memStalls(0); };

    bool linefillBlockInquire(Logger *logger, unsigned serialized) const;

    bool storeBufDoneInquire(Logger *logger,uint64_t instId) const;

    bool nextLevelActionsEmptyInquire(Logger *logger) const;

    bool commandInquire(Logger *logger) const;

    uint64_t command(MemRequestType type, addr_t ea, addr_t ra, int set, int way,
                     bool writeThrough, const ReqInfo &rinfo,const CacheStatusHint *hint = NULL);

    // Overloaded, since the memory logger needs to have a next-level name in
    // order to know that it's a cache.
    virtual void createLogger(LogClient *,unsigned);
    virtual void removeLogger();

    // per time step methods
    virtual void preCycle();
    virtual void postCycle();
    virtual void postSim();
    virtual void logAllRequests() const;

    // access/query methods
    virtual bool isActive() const;
    virtual bool storeBufEnabled() { return true; }
    // This is only used if a cache is configured to have dynamic entry sizes,
    // in which case it is queried during linefill init.
    virtual unsigned linefillWidth(addr_t ea,addr_t ra) { return 0; }

    virtual void set_level(unsigned);

    // for handling dynamic parameters.
    virtual bool set_dyn_parm(const std::string &parm,unsigned value);
    virtual bool get_dyn_parm(unsigned &value,const std::string &parm) const;
    virtual void list_dyn_parm(adl::StrPairs &parms) const;

  protected:
    // Currently, we only support having a single port in a cache.  I just don't
    // think it really makes a lot of sense to have multiple ports in a cache,
    // so that's why we always just use the first one.
    MemoryPort &defPort() { return _ports[0]; };
    const MemoryPort &defPort() const { return _ports[0]; };

    virtual void cancelNextLevelRequest(const MemoryRequest &request);
    virtual void postCancelRequest(MemoryPort &port,unsigned requestId);

    virtual bool lookup(addr_t addr, unsigned &set, unsigned &way, unsigned &offset) const = 0;
    virtual byte_t *cachePosition(unsigned set, unsigned way, unsigned offset) const;
    virtual addr_t getTagAddr(unsigned set, unsigned way) = 0;
 
    virtual void enableCacheLineData(unsigned set, unsigned way) = 0;
    virtual void disableCacheLineData(unsigned set, unsigned way) = 0;
    virtual bool cacheLineHasData(unsigned set, unsigned way) const = 0;

    // ADL cache interface
    virtual void operationBegin(MemoryRequest &request);
    virtual void operationEnd(const MemoryRequest &request) const = 0;
    // temporary HACK
    virtual void clearValid(unsigned set, unsigned way) = 0;

    // access/query methods
    virtual byte_t *hasData(addr_t addr, unsigned &size) const;
    bool hasCommand() const { return _command != adl::CacheNoAccess; }

  protected:
    bool _getMemoryData(adl::CacheAccess type,addr_t addr, unsigned size,
                        byte_t *data, bool debug, bool cacheEnabled = false);
    bool _setMemoryData(addr_t addr, unsigned size, const byte_t *data, bool debug,
                        bool cacheEnabled = false);

    void addLinefillAction(MemoryRequest &request);
    bool writeBackLinefill();
    void dumpLinefill();
    void endCommand();
    bool check_serialized(unsigned serialized);

    // access/query methods
    void getCacheData(unsigned set, unsigned way, unsigned offset, unsigned size,
                      byte_t *data) const;
    void setCacheData(unsigned set, unsigned way, unsigned offset, unsigned size,
                      const byte_t *data);

    bool hitLinefillBuf(const MemoryRequest &request) const;

    // data members
    UadlMemoryIf &_memory;    // next-level memory  
    unsigned        _memoryPort; // next-level memory port

    const unsigned _lineWidth;   // number of bytes per line
    const bool _writeThrough; // write mode (T: write through, F: write back)

    const unsigned _fillReadLatency;          // linefill read latency on hit (in cycle) 
    const unsigned _fillWriteLatency;         // linefill write latency on hit (in cycle)
    const unsigned _zeroLatency;              // zero latency (in cycle)
    const unsigned _taken2writeLatency;       // taken-to-write latency (in cycle)
    const unsigned _write2nextLatency;        // write-to-(next-level-request) latency
    const unsigned _linefillWritebackLatency; // latency (in cycle) from linefill is
                                              // done to data is in data array
    const unsigned _linefillLockCycle;        // lookup idle cycle of linefill
    const unsigned _linefillLockHoldoffCycle; // Hold off time on a line-fill idle cycle(s).
    const bool     _linefillLockOnValid;      // Linefill lock only applies when prior data was valid.
    const bool     _consecutiveLinefills;     // True => start a next linefill before the existing one ends, if
                                              // the first has made all of its requests.
    BlockCriticalType _blockOnCriticalWord;   // Block a request on receiving a critical word.  Maybe set to always block,
                                              // or only block for serialized, or never block.
    const unsigned _evictLockCycle;           // lookup idle cycle of eviction
    const unsigned _storeBufSize;             // If non-zero:  Max size of store buffer.

    LinefillBuf _linefill;                    // linefill buffer
    EvictBuf _evictBuf;                       // push buffer

    // TODO: support fixed-width/fixed-size store buffer
    struct StoreBufEntry {
      StoreBufEntry() {};
      StoreBufEntry(addr_t _ea, addr_t _ra, unsigned _dataWidth, unsigned _serialized,
                    ttime_t _time,const CacheStatusHint &_hint, const byte_t *data,
                    bool _writeThrough, const ReqInfo &_rinfo) :
        ea(_ea), addr(_ra), dataWidth(_dataWidth), serialized(_serialized), time(_time), hint(_hint),
        writeThrough(_writeThrough), rinfo(_rinfo), _hasData(data)
      { if (data) memcpy(_data, data, dataWidth); }
      
      addr_t ea;
      addr_t addr;
      unsigned dataWidth;
      unsigned serialized;
      ttime_t time;    // data write time
      
      CacheStatusHint hint;
      bool writeThrough;
      ReqInfo rinfo;   // instruction info generating the store.

      const byte_t *data() const { return _hasData ? _data : NULL; }

    private:
      bool _hasData;
      byte_t _data[sizeof(uint64_t)];
    };

    adl::DSCircularQueue<StoreBufEntry,CacheQueueDebugCheck> _storeBuf; // store buffer

    // next-level memory action type
    enum NextLevelActionType {
      LINEFILL,       // load linefill buffer
      EVICT,          // evict a cache line
      READ_NEXT,      // serve a non-allocate read miss
      STORE_NEXT,     // write a store buffer entry
      MAX_TYPE
    };

    // data structure to control next-level memory action ordering
    struct NextLevelAction {
      NextLevelAction() {};
      NextLevelAction(NextLevelActionType _type, MemoryRequest &_request,
                      bool _hasParallelEvict = false);
      NextLevelAction(NextLevelActionType _type, MemoryRequest &_request,
                      addr_t ea,addr_t ra,bool _hasParallelEvict = false);

      NextLevelActionType type;
      ReqInfo  rinfo;         // for LINEFILL and EVICT
      uint64_t id;            // this-level memory request id
      MemoryRequest *request; // (used by READ_NEXT and STORE_NEXT that does not
                              //  use store buffer)
      bool hasParallelEvict;  // (only used by LINEFILL)

      // request data members (only copied for LINEFILL, EVICT, STORE_Next)
      MemRequestType requestType;
      addr_t ea;
      addr_t addr;
      unsigned set;
      unsigned way;

      // request data members (only copied for LINEFILL)
      bool requestTime;
      bool replaceLine;

      // request data members (only used by STORE_NEXT)
      bool useStoreBuf;

      // request control data members
      bool started;
      unsigned requestId;     // next-level memory request id
      bool completed;         // (only used by LINEFILL)
      ttime_t completeTime;   // completion time (only used by LINEFILL)
    };

    void logAction(const NextLevelAction &action) const;
    NextLevelAction *findAction(uint64_t id);

    adl::DSCircularQueue<NextLevelAction,CacheQueueDebugCheck> _actions;
    unsigned _nActions; // number of current-cycle actions (excluding new actions)
    // redundant information for fast access to linefill request id's
    adl::DSCircularQueue<uint64_t,CacheQueueDebugCheck> _linefillIds;

    mutable unsigned _cacheStalls;

    MemRequestType _command;
    unsigned _commandLatency[adl::MaxCacheAccess]; // cache command latencies

    unsigned _linefillRequestBlock;

    // lookup control data members
    bool _canLookup;
    bool _memDataBusBusy;
    bool _serializedValid;
    ttime_t _lastMemTime, _lastCritWordTime;
    adl::DSCircularQueue<ttime_t,CacheQueueDebugCheck> _lookupDisableTimes;
    adl::DSCircularQueue<ttime_t,CacheQueueDebugCheck> _lookupEnableTimes;
  };


  // Templates wrapper class for caches.  This contains the methods actually used
  // by instruction-class code.
  template <class CacheType,class ModelType,class InstrType,bool safeMode>
  class CacheT : public MemoryT<CacheType,ModelType,InstrType,safeMode> {
    typedef MemoryT<CacheType,ModelType,InstrType,safeMode> Base;
  public:
    CacheT (CacheType &cache) : Base(cache), _cache(cache) {};

    bool next_level_actions_empty(UadlArch &uarch,InstrType &instr,Logger *logger) {
      return _cache.nextLevelActionsEmptyInquire(logger);
    }

    bool linefill_not_blocked(UadlArch &uarch,InstrType &instr,Logger *logger) {
      UadlMemAccess *ma = instr.mem();
      return _cache.linefillBlockInquire(logger,(ma) ? ma->_serialized : 0);
    }

    bool store_buf_done(UadlArch &uarch,InstrType &instr,Logger *logger) {
      return _cache.storeBufDoneInquire(logger,instr.iid());
    }

    bool can_request_cmd(UadlArch &uarch,InstrType &instr,Logger *logger) {   
      return _cache.commandInquire(logger);
    }
    
    void send_cmd(ModelType &uarch,InstrType &instr,Logger *logger) {
      if (!instr.mem()) return;
      
      UadlMemAccess &ma = *instr.mem();

      bool writeThrough = _cache.is_write_through(ma._ra);

      _cache.command(ma._type,ma._ea,ma._ra,ma._c._set,ma._c._way,writeThrough,instr.rinfo(),ma.hint());

      if (logger && uarch.logMemory()) {
        logger->logSendCacheCommand(ma._type,ma._ra);
      }

      uarch.free_mem(instr);
    };
    
    CacheType &_cache;
  };

  inline bool Cache::getMemoryData(adl::CacheAccess type,addr_t addr, unsigned size,
                                   byte_t *data)
  { return _getMemoryData(type, addr, size, data, true); }


  inline bool Cache::setMemoryData(addr_t addr, unsigned size, const byte_t *data)
  { return _setMemoryData(addr, size, data, true); }


  inline bool Cache::fetchMemoryData(addr_t addr, unsigned size, byte_t *data,
                                     uint64_t &exception, bool cacheEnabled)
  {
    return _getMemoryData(adl::CacheIFetch, addr, size, data, false,
                          cacheEnabled);
  }


  inline bool Cache::readMemoryData(addr_t addr, unsigned size, byte_t *data,
                                    uint64_t &exception, bool cacheEnabled)
  {
    return _getMemoryData(adl::CacheRead, addr, size, data, false, cacheEnabled);
  }


  inline bool Cache::writeMemoryData(addr_t addr, unsigned size, const byte_t *data,
                                     uint64_t &exception, bool cacheEnabled)
  { return _setMemoryData(addr, size, data, false, cacheEnabled); }


  inline byte_t* Cache::cachePosition(unsigned set, unsigned way, unsigned offset) const
  { return NULL; }


  inline void Cache::operationBegin(MemoryRequest &request)
  { request.lookupTime = currTime(); }


  // TODO: check cache-line boundary
  inline void Cache::getCacheData(unsigned set, unsigned way, unsigned offset, unsigned size,
                                  byte_t *data) const
  { if (_withData) memcpy(data, cachePosition(set, way, offset), size); }


  // TODO: check cache-line boundary
  inline void Cache::setCacheData(unsigned set, unsigned way, unsigned offset, unsigned size,
                                  const byte_t *data)
  { if (_withData) memcpy(cachePosition(set, way, offset), data, size); }

} // namespace uadl

#endif // _CACHE_H

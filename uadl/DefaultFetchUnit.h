//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
#ifndef _DEFAULT_FETCH_UNIT_H
#define _DEFAULT_FETCH_UNIT_H

// TODO: remove assert() and assert.h
#include <assert.h>
#include <vector>

#include "helpers/BasicTypes.h"

#include "iss/CircularQueue.h"

#include "UadlArch.h"
#include "FetchLogger.h"
#include "UadlMemoryIf.h"
#include "DynParms.h"

namespace uadl {

  // TODO: remove lazyFetch, flushFetch from all others
  // TODO: should canCancel be a memory parameter?
  // TODO: switch logger type
#define TEMPLATE template<class ArchType, unsigned entrySize, unsigned numFetchSizes,  \
                          int minFetchEntries, unsigned minInstrSize, \
                          unsigned maxInstrSize, unsigned maxFetchCount, \
                          unsigned BPStep, bool hasPredictor, bool lazyFetch, \
                          bool flushFetch, bool canCancel, bool reuseData, \
                          bool safeMode, bool issMemMode>

  struct FetchData {
    addr_t        _ea;
    addr_t        _ra;
    unsigned      _size;
    const byte_t *_data;

    FetchData() : _data(0) {};
    FetchData(addr_t ea,addr_t ra,unsigned size,const byte_t *data) : _ea(ea), _ra(ra), _size(size), _data(data) {};

    operator bool () { return _data; };
  };

  /* NOTE: _alignedFetch is true iff WE KNOW that the next fetch is aligned wrt
   *       the largest fetch size.  It could be false even if the next fetch IS
   *       aligned wrt the largest fetch size. */
  TEMPLATE class DefaultFetchUnit : public DynParms {
  public:
    // Note: Slight speed improvement if we use a power of 2, since modulo
    // operations are then faster.
    // These should probably be made configurable at model creation time.
    enum { 
      NumInstrBufEntries = 64,
      NumPendingRequests = 64,
      NumPredTakenInfo   = 64,
    };

    DefaultFetchUnit(ArchType &arch, const std::string &name,unsigned nEntries,
                     UadlMemoryIf &mem, unsigned memPort, const FetchSizes &fetchSizes,
                     UadlMemoryIf *lastMem = 0);

    ~DefaultFetchUnit() 
    {
      removeLogger();
    }

    // Call this for a complete re-start.
    void full_reset();

    // This simiply updates the fetch unit so that it will start at a new
    // address.
    void reset(addr_t newFetchAddr) { 
      flush(newFetchAddr); 
    }

    unsigned fetchStalls() const { return _fetchStalls; };

    void createLogger(LogClient *lc,unsigned tflags) { 
      if (!_logger) { 
        _logger = new FetchLogger(_name,lc,tflags); 
      } else {
        _logger->setClient(lc,tflags);
      }
    };

    void removeLogger() { delete _logger; _logger = 0; };
    bool report_and_clear(std::ostream &o,const std::string &pfx) { 
      if (_logger) 
        return _logger->report_and_clear(o,pfx); 
      else
        return false;
    };

    FetchLogger *logger() { return _logger; };
    const FetchLogger *logger() const { return _logger; };

    // preCycle() and postCycle()
    void receiveInstrData();
    void sendFetchRequests();

    FetchData instrData(bool trace_on);
    void decodeUpdate(bool success, unsigned size);
    void removeInstr(unsigned size);
    bool flush(addr_t newFetchAddr);

    bool predTaken() const { return _predTaken; };
    addr_t predTrg() const { return _predTrg; };

    // Temporarily overrides the default can-cancel value with a new one.  A
    // subsequent flush, cancel, or reset restores the default.
    void set_cancel(ArchType &uarch,InstrBase &instr,Logger *logger,bool can_cancel) { _can_cancel = can_cancel; };

    void flush(ArchType &uarch,InstrBase &instr,Logger *logger);

    void cancel(ArchType &uarch,InstrBase &instr,Logger *logger);

    void pause(ArchType &uarch,InstrBase &instr,Logger *logger);

    void force(ArchType &uarch,InstrBase &instr,Logger *logger);

    void forceFetch() { _tryNewFetch = true; }
    void pauseFetch(uint64_t instrId);
    void preemptFetch();

    // access/query methods
    unsigned instrBufEntrySize() const { return entrySize; }

    bool empty() const;
    addr_t nextFetchAddr() const { return _nextFetchAddr; }
    addr_t nextInstrAddr() const;

    addr_t nextIssueAddr() const;

    // logging methods
    void instrBufReport() const;

    // for handling dynamic parameters.
    virtual bool set_dyn_parm(const std::string &parm,unsigned value);
    virtual bool get_dyn_parm(unsigned &value,const std::string &parm) const;
    virtual void list_dyn_parm(adl::StrPairs &parms) const;

  private:
    // Instruction buffer entry object.
    class InstrBufEntry {
    public:
      InstrBufEntry() : _ea(0), _ra(0), _predTaken(false) {}

      InstrBufEntry(addr_t ea, addr_t ra, const byte_t *data, 
                    bool predTaken = false, addr_t predTrg = 0, unsigned instrSize = 0) :
        _ea(ea), _ra(ra), _predTaken(predTaken), _predTrg(predTrg), _instrSize(instrSize)
      {
        memcpy(_data, data, entrySize);
      }

      addr_t ea() const { return _ea; }
      addr_t ra() const { return _ra; }
      const byte_t *data() const { return _data; }
      bool predTaken() const { return _predTaken; }
      addr_t predTrg() const{ return _predTrg; };
      unsigned instrSize() const { return _instrSize; }

      void clearTakenPred() { _predTaken = false; }

    private:
      addr_t _ea;       // virtual address
      addr_t _ra;       // physical address
      byte_t _data[entrySize];
      bool _predTaken;  // if predicted taken
      addr_t _predTrg;  // target, if predicted taken
      unsigned _instrSize; // size (in byte) of a predicted-taken instruction
    };

    void setup();
    bool logEnabled() const { return _logger; };
    bool hasInstrSpace();
    void flushInstrBuf(unsigned remain, addr_t newFetchAddr, bool flushCanCancel = canCancel);
    void logInstrRead(addr_t ea,addr_t ra);

    std::vector<mask_t> createAddrMasks() const;
    std::vector<mask_t> createAddrFetchSizes() const;

    // configuration data members
    ArchType &_arch;
    std::string _name;
    UadlMemoryIf &_mem;             // first-level instruction memory
    const unsigned _memPort;        // first-level instruction memory port
    UadlMemoryIf* const _lastMem;   // last-level instruction memory

    const ReqInfo _rinfo;

    int _nEntries;

    const std::vector<unsigned> _fetchSizes;
    const std::vector<mask_t>   _addrMasks;
    const std::vector<mask_t>   _addrFetchSizes;
    addr_t                      _fetchMask; 

    FetchLogger *_logger;

    // run-time data members
    bool _can_cancel;               // Current value for if flush/cancel can
                                    // cancel outstanding stuff.
    bool _tryNewFetch;
    bool _alignedFetch;
    addr_t _nextFetchAddr;          // next fetch address (ea)
    uint64_t _pauser;
    bool     _predTaken;            // last prediction.
    addr_t   _predTrg;              // last prediction target.

    byte_t _data[sizeof(uint64_t)]; // issMemMode only

    // The vector is treated as a circular queue in order to decrease the
    // overhead of managing the entries.
    adl::CircularQueue<InstrBufEntry,NumInstrBufEntries> _instrBuf;

    unsigned _prevInstrBufSpace; // Previous-cycle available instruction buffer space
                                 // (minFetchEntries ? in entry : in byte)
    unsigned _currInstrBufSpace; // Current-cycle available instruction buffer space
                                 // (in byte)
    unsigned _nextDecodeEntry;   // Next instruction buffer entry to be decoded
    unsigned _lastDecodeEntries; // Number of last decoded instruction buffer entries

    byte_t _decodeBuf[maxInstrSize]; // Contiguous fetch data, read by ISS for decode.

    unsigned _fetchStalls;       // Tracks the number of stalls caused by fetch.

    // Pending fetch request object.
    class FetchRequest {
    public:
      FetchRequest() : _id(0), _ea(0), _ra(0) {};
      FetchRequest(uint64_t id, addr_t ea, addr_t ra, unsigned fetchSize,
                   unsigned frontPadSize, unsigned rearPadSize,
                   bool cacheEnabled = false, const byte_t *data = 0) : 
      _id(id), _ea(ea), _ra(ra), _fetchSize(fetchSize),
      _frontPadSize(frontPadSize), _rearPadSize(rearPadSize), _preempted(false),
      _cacheEnabled(cacheEnabled)
          // TODO: use the largest fetch size
      { if (data) memcpy(_data, data, sizeof(uint64_t)); }

      unsigned size() const { return _fetchSize - _frontPadSize - _rearPadSize; }

      uint64_t _id;           // request id
      addr_t   _ea;           // request ea
      addr_t   _ra;           // request ra
      unsigned _fetchSize;    // total fetch size (in byte)
                              // TODO: replace this with a method
      unsigned _frontPadSize; // extra fetch size at front due to alignment constraint
      unsigned _rearPadSize;  // extra fetch size at rear due to taken prediction

      bool _preempted;

      CacheStatusHint _hint;
      bool _cacheEnabled;

      // TODO: use the largest fetch size
      byte_t _data[sizeof(uint64_t)];
    };

    adl::CircularQueue<FetchRequest,NumPendingRequests> _pendingRequests;

    // predicted-taken branch info type
    class PredTakenInfo {
    public:
      PredTakenInfo() : _addr(0), _target(0), _size(0) {};
      PredTakenInfo(addr_t addr, addr_t target, unsigned size) :
        _addr(addr), _target(target), _size(size) {}

      addr_t   _addr;    // instruction (virtual) address
      addr_t   _target;  // target address
      unsigned _size;    // instruction size
    };

    adl::CircularQueue<PredTakenInfo,NumPredTakenInfo> _predTakenInstrs;

    bool _incompletePredTakenInstr;
  };


#define CLASS DefaultFetchUnit<ArchType, entrySize, numFetchSizes, minFetchEntries, \
                               minInstrSize, maxInstrSize, maxFetchCount, BPStep, hasPredictor, lazyFetch, \
                               flushFetch, canCancel, reuseData, safeMode, \
                               issMemMode>

  TEMPLATE CLASS::DefaultFetchUnit(ArchType &arch,const std::string &name,
                                   unsigned nEntries, UadlMemoryIf &mem, unsigned memPort,
                                   const std::vector<unsigned> &fetchSizes,
                                   UadlMemoryIf *lastMem) :
    _arch(arch), _name(name), _mem(mem), _memPort(memPort),
    _lastMem(lastMem), _rinfo(-1,arch.thread_id()), _nEntries(nEntries), _fetchSizes(fetchSizes),
    _addrMasks(createAddrMasks()), _addrFetchSizes(createAddrFetchSizes()),
    _fetchMask(_addrMasks.front()), _logger(0), _alignedFetch(false),
    _predTaken(false), _predTrg(0)
  {
    // TODO:  Make the queue size configurable.
    assert(nEntries < NumInstrBufEntries);

    arch.register_dyn_parms(*this);
    // TODO: add more parameters sanity check

    if (safeMode && !issMemMode) assert(lastMem);

    full_reset();
  }

  // After changing number of entries or entry-size, call this.  This isn't
  // meant to change things once simulation starts.
  TEMPLATE void CLASS::full_reset()
  {
    _can_cancel = canCancel;
    _nextDecodeEntry   = 0;
    _lastDecodeEntries = 0;
    _incompletePredTakenInstr = false;
    _pendingRequests.clear();
    _predTakenInstrs.clear();
    _instrBuf.clear();

    _prevInstrBufSpace = (minFetchEntries ? _nEntries : _nEntries * entrySize);
    _currInstrBufSpace = (_nEntries * entrySize);

    _fetchStalls = 0;
  }

  TEMPLATE void CLASS::receiveInstrData()
  {
    _prevInstrBufSpace = minFetchEntries ? _nEntries - _instrBuf.size() : _currInstrBufSpace;
    _tryNewFetch = lazyFetch ? hasInstrSpace() : true;

    // TODO: support fetch logging and dynamic fetch size
    while (!_pendingRequests.empty() && _mem.readInquire(0, _logger, _memPort, _pendingRequests.front()._id)) {
      FetchRequest &request = _pendingRequests.front();
      addr_t ea = request._ea;
      addr_t ra = request._ra;

      // uarch read to complete memory read transaction
      _mem.read(_memPort, request._id, safeMode ? 0 : request._data);

      // ISS read to retrieve the right data
      if (safeMode && !issMemMode) {
        uint64_t fetchException = 0;

        _lastMem->fetchMemoryData(ra, request._fetchSize, request._data,
                                  fetchException, request._cacheEnabled);
      }

      if (logEnabled()) {
        _logger->logReceiveData(request._id, ea, ra, request._fetchSize, request._data);
      }

      // store fetched data into instruction buffer
      ea += request._frontPadSize;
      ra += request._frontPadSize;

      for (const byte_t *position = request._data + request._frontPadSize;
           position < request._data + request._frontPadSize + request.size();
           position += entrySize, ea += entrySize, ra += entrySize)
        {
          if (hasPredictor) {
            bool predTaken = false;
            addr_t predTarget = 0;
            unsigned instrSize = 0;

            // check if data come from a predicted-taken instruction
            if (!_predTakenInstrs.empty()) {
              const PredTakenInfo &predInfo = _predTakenInstrs.front();

              // instruction datum belongs to a predicted-taken branch
              if (ea == predInfo._addr) {
                predTaken = true;
                predTarget = predInfo._target;
                instrSize = predInfo._size;
              }

              // instruction data fully fetched
              if (ea + entrySize == predInfo._addr + predInfo._size) {
                _predTakenInstrs.pop_front();
              }
            }

            *(_instrBuf.push_back()) = InstrBufEntry(ea, ra, position, predTaken, predTarget, instrSize);
          } else {
            *(_instrBuf.push_back()) = InstrBufEntry(ea, ra, position);
          }
        }

      _pendingRequests.pop_front();
    }
  }


  TEMPLATE void CLASS::sendFetchRequests()
  {
    unsigned fetchCount = 0;
    bool canSend = true;
    PredTakenInfo *takenPred = 0;
    CacheStatusHint hint;

    if (_tryNewFetch) {
      uint64_t fetchException = 0;

      // re-send last fetch request if preempted
      if (!_arch.hasFetchException() && !_pendingRequests.empty() &&
          _pendingRequests.back()._preempted &&
          (canSend = _mem.sendRequestInquire(0, _logger, adl::CacheIFetch, 0, _memPort)))
        {
          FetchRequest &request = _pendingRequests.back();

          CacheStatusHint *fhint = (safeMode) ? &request._hint : 0;

          // TODO: re-do TLB lookup?
          request._id = _mem.sendFetchRequest(request._ea, request._ra,
                                              request._fetchSize, 0, fetchException,
                                              fhint, _rinfo, _memPort);
          request._preempted = false;
          request._cacheEnabled = _arch.fetchCacheEnabled(request._ra);

          _currInstrBufSpace -= request.size();
          fetchCount++;

          // TODO: re-do branch prediction?
          if (hasPredictor && !_predTakenInstrs.empty()) {
            PredTakenInfo &predInfo = _predTakenInstrs.back();
            const addr_t fetchAddr = request._ea + request._frontPadSize;

            // cover last predicted-taken address?
            if (predInfo._addr >= fetchAddr &&
                predInfo._addr < fetchAddr + request.size()) { 
              takenPred = &predInfo; 
            }
          }

          if (logEnabled()) {
            if (hasPredictor) {
              _logger->logSendRequest(request._id, request._ea, request._ra,
                                      request._fetchSize,
                                      takenPred || _arch.branchPredictEnabled(),
                                      takenPred ? &takenPred->_addr : 0,
                                      takenPred ? &takenPred->_target : 0,
                                      true);
            }
            else {
              _logger->logSendRequest(request._id, request._ea, request._ra,
                                      request._fetchSize, true);
            }
          }
        }

      while ((!_arch.hasFetchException()) &&
             ((hasInstrSpace()) || (minFetchEntries && !fetchCount)) &&
             (fetchCount < maxFetchCount) && 
             ((canSend = _mem.sendRequestInquire(0, _logger, adl::CacheIFetch, 0, _memPort))))
        {
          unsigned fetchSize = _fetchSizes[0];
          unsigned frontPadSize = 0;
          unsigned rearPadSize = 0;
          addr_t fetchEA, fetchRA;

          if (!_alignedFetch) {
            if (numFetchSizes > 1) { 
              // Multiple fetch sizes, so try to find the largest aligned fetch
              // size.
              unsigned fetchIndex = _nextFetchAddr & _fetchMask;
              fetchSize = _addrFetchSizes[fetchIndex];
              _alignedFetch = (fetchIndex == 0);

              if (!fetchSize) {
                // over-fetch from a past address
                const addr_t nextAddr = (_nextFetchAddr & ~(addr_t)_addrMasks[0]) +
                  _fetchSizes[0];
                const unsigned distance = nextAddr - _nextFetchAddr;

                for (int i = numFetchSizes - 1; !fetchSize && i >= 0; i--) {
                  if (_fetchSizes[i] >= distance) {
                    fetchSize = _fetchSizes[i];
                  }
                }

                frontPadSize = fetchSize - distance;
                _nextFetchAddr = _arch.ea(nextAddr - fetchSize);
                _alignedFetch = true;
              }
            }
            else {                        // single fetch size
              if (_nextFetchAddr & _addrMasks[0]) {
                // over-fetch from a past address
                const addr_t nextAddr = _nextFetchAddr & ~(addr_t)_addrMasks[0];

                frontPadSize = _nextFetchAddr - nextAddr;
                _nextFetchAddr = nextAddr;
              }

              _alignedFetch = true;
            }
          }

          try {
            _arch.fetchTranslate(_nextFetchAddr, fetchRA, frontPadSize,
                                 fetchSize, safeMode ? _data : 0);
          }
          catch (int) {
            // We signal via an int b/c we don't have access to the ISSs
            // exception type in the header file.  We don't care what the
            // exception data is- we're just using this as a way to avoid a
            // conditional in the common path.
            if (logEnabled()) _logger->logFetchException(_nextFetchAddr, fetchSize);

            // restore ISS fetch size
            _nextFetchAddr += frontPadSize;
            
            break;
          }

          fetchEA = _nextFetchAddr;

          if (safeMode) {
            _arch.updateInstrCacheStatus(hint,fetchRA);
          }

          const uint64_t requestId = _mem.sendFetchRequest(fetchEA, fetchRA,
                                                           fetchSize, 0,
                                                           fetchException,
                                                           &hint,_rinfo,_memPort);
          fetchCount++;

          // do branch prediction at fetch time
          if (hasPredictor) {
            takenPred = 0;

            // step 1: if the last predicted-taken instruction is completely
            //         fetched, do new branch predictions
            if (!_incompletePredTakenInstr) {
              for (addr_t instrAddr = _nextFetchAddr + frontPadSize;
                   !takenPred && instrAddr < _nextFetchAddr + fetchSize;
                   instrAddr += BPStep)
                {
                  addr_t BP_target;
                  unsigned BP_size;

                  Prediction prediction = _arch.branchPredict(instrAddr,BP_target, BP_size);
                  if (prediction == Taken) {
                    takenPred = _predTakenInstrs.push_back();
                    *takenPred = PredTakenInfo(instrAddr, BP_target, BP_size);
                  }
                }
            }

            // step 2: determine next fetch address
            if (takenPred || _incompletePredTakenInstr) {
              // takenPred                 => a new taken prediction
              // _incompletePredTakenInstr => an old incomplete taken prediction
              const PredTakenInfo &predInfo = _predTakenInstrs.back();
              _incompletePredTakenInstr = predInfo._addr + predInfo._size > _nextFetchAddr + fetchSize;

              if (_incompletePredTakenInstr) {
                _nextFetchAddr = _arch.ea(_nextFetchAddr + fetchSize);
              }
              else {
                // exclude spaces after the predicted-taken branch
                rearPadSize = (_nextFetchAddr + fetchSize) - (predInfo._addr + predInfo._size);
                _nextFetchAddr = predInfo._target;
                _alignedFetch = false;
              }
            }
            else {
              _nextFetchAddr = _arch.ea(_nextFetchAddr + fetchSize);
            }
          }
          else { // no branch prediction at fetch time
            _nextFetchAddr = _arch.ea(_nextFetchAddr + fetchSize);
          }

          FetchRequest *nextFetch = _pendingRequests.push_back();
          *nextFetch = FetchRequest(requestId, fetchEA, fetchRA,
                                    fetchSize, frontPadSize,
                                    rearPadSize,
                                    _arch.fetchCacheEnabled(fetchRA),
                                    issMemMode ? _data : 0);
          if (safeMode) {
            _arch.updateInstrCacheStatus(nextFetch->_hint,fetchRA);
          }

          // reserve instruction buffer space
          _currInstrBufSpace -= nextFetch->size();

          // logging fetch request
          if (logEnabled()) {
            if (hasPredictor) {
              _logger->logSendRequest(requestId, fetchEA, fetchRA, fetchSize,
                                      _arch.branchPredictEnabled(),
                                      takenPred ? &takenPred->_addr : 0,
                                      takenPred ? &takenPred->_target : 0);
            }
            else {
              _logger->logSendRequest(requestId, fetchEA, fetchRA, fetchSize);
            }
          }
        }
    }

    if (!fetchCount) {
      _fetchStalls++;
      if (logEnabled()) { // log fetch stall
        if (_arch.hasFetchException()) {
          // stall: pending fetch exception
          _logger->logStallException();
        }
        else if (_prevInstrBufSpace < minFetchEntries) {
          // stall: insufficient instruction buffer entry
          _logger->logStallNoEntry(minFetchEntries, _prevInstrBufSpace);
        }
        else if (!minFetchEntries && _prevInstrBufSpace < _fetchSizes[0]) {
          // stall: insufficient instruction buffer space
          _logger->logStallNoSpace(_fetchSizes[0], _prevInstrBufSpace);
        }
        else if (!canSend) {
          // stall: instruction memory unavailable
          _logger->logStallCannotSend(_mem.name());
        }
        else if (!_tryNewFetch) {
          // stall: paused by instruction
          _logger->logStallPaused(_pauser);
        }
        else {
          // stall: program terminated
          _logger->logStallIssDown();
        }
      }
    }
  }

  // There's no need to log for safe-mode, since the instruction reads are
  // directly connected to the instruction execution.  Here, we're only fetching
  // for time-accounting purposes, so we don't need to log anything.
  TEMPLATE void CLASS::logInstrRead(addr_t ea,addr_t ra)
  {
    if (!safeMode) {
      _arch.log_instr_read(ea,ra);
        
      if (_instrBuf.size() == 8 && (ea & 0x7) < 4) {
        _arch.log_instr_read(ea + 4, ra + 4);
      }
    }
  }

  TEMPLATE FetchData CLASS::instrData(bool trace_on)
  {
    if (_instrBuf.size() > _nextDecodeEntry) {
      _predTaken = false;
      _predTrg = 0;
      InstrBufEntry &firstEntry = _instrBuf[_nextDecodeEntry];

      if (entrySize == maxInstrSize) {
        _predTaken = firstEntry.predTaken();
        _predTrg = firstEntry.predTrg();
        //if (trace_on) logInstrRead(firstEntry.ea(),firstEntry.ra());
        return FetchData(firstEntry.ea(),firstEntry.ra(),entrySize,firstEntry.data());
      } else {
        const unsigned instrSize = (_instrBuf.size() - _nextDecodeEntry) * entrySize;

        // check: * has at least minimum data
        //        * a predicted-taken instruction has enough data
        if ((instrSize < minInstrSize) || (firstEntry.predTaken() && instrSize < firstEntry.instrSize())) { 
          return FetchData(); 
        }

        //        if (trace_on) logInstrRead(firstEntry.ea(),firstEntry.ra());        

        unsigned size = firstEntry.predTaken() ? firstEntry.instrSize() : maxInstrSize > instrSize ? instrSize : maxInstrSize;
        _predTaken = firstEntry.predTaken();
        _predTrg = firstEntry.predTrg();
        _lastDecodeEntries = size / entrySize;

        byte_t *data = _decodeBuf;

        memset(data, 0, maxInstrSize);
        memcpy(data, _instrBuf[_nextDecodeEntry].data(), entrySize);
        data += entrySize;

        // copy instruction buffer data
        for (unsigned i = _nextDecodeEntry + 1; i < _nextDecodeEntry + _lastDecodeEntries; i++, data += entrySize) {
          //if (trace_on) logInstrRead(_instrBuf[i].ea(),_instrBuf[i].ra());
          memcpy(data, _instrBuf[i].data(), entrySize);
        }

        return FetchData(firstEntry.ea(),firstEntry.ra(),size,_decodeBuf);
      }
    } else {
      return FetchData();
    }
  }

  // TODO: clear _hasFetchException when calling flushInstrBuf()
  TEMPLATE void CLASS::decodeUpdate(bool success, unsigned size)
  {
    if (entrySize == maxInstrSize) {
      _nextDecodeEntry++;
    }
    else {
      if (success) {
        if (size) { // valid instruction
          _lastDecodeEntries = size / entrySize;
          if (hasPredictor && safeMode) { // need bad prediction check
            for (unsigned i = _nextDecodeEntry + 1;
                 i < _nextDecodeEntry + _lastDecodeEntries; i++)
              {
                if (_instrBuf[i].predTaken()) {
                  // bad prediction found
                  flushInstrBuf(_lastDecodeEntries, 0);

                  if (logEnabled()) _logger->logBadPrediction(_instrBuf[i].ea());
                }
              }
          }
        }

        _nextDecodeEntry += _lastDecodeEntries;
      }
      else if (safeMode && _instrBuf[_nextDecodeEntry].predTaken()) {
        // insufficient instruction data, need to recover from a bad branch
        // prediction
        _instrBuf[_nextDecodeEntry].clearTakenPred();
        flushInstrBuf(_lastDecodeEntries, 0);

        if (logEnabled()) _logger->logBadPrediction(_instrBuf[_nextDecodeEntry].ea());
      }
    }
  }


  TEMPLATE void CLASS::removeInstr(unsigned size)
  {
    // assert(!(size % entrySize));
    if (size == 0) return;

    unsigned entries;

    if (minInstrSize == maxInstrSize) {
      // assert(size == minInstrSize);
      entries = minInstrSize / entrySize;
      size = entries * entrySize; // make size compile-time constant
    }
    else {
      entries = size / entrySize;
    }

    _instrBuf.pop_front(entries);
    _currInstrBufSpace += size;
    _nextDecodeEntry -= entries;
  }


  TEMPLATE bool CLASS::flush(addr_t newFetchAddr)
  {
    bool hasTarget = false; // target not found yet

    if (reuseData) {
      addr_t fetchRA;

      // check if the target address is already in the instruction buffer
      unsigned sizeInstrBuf = _instrBuf.size();
      for (unsigned i = _nextDecodeEntry; !hasTarget && i < sizeInstrBuf; i++) {
        if (_instrBuf[i].ea() == newFetchAddr) {
          // TODO: check exception status
          _arch.fetchTranslate(newFetchAddr, fetchRA);

          if ((hasTarget = _instrBuf[i].ra() == fetchRA) && i != _nextDecodeEntry) {
              _instrBuf.erase_middle(_nextDecodeEntry,i);
              // reclaim instruction buffer space
              _currInstrBufSpace += (i - _nextDecodeEntry) * entrySize;
            }
        }
      }

      // check if the target address is already pending
      unsigned sizePendingRequests = _pendingRequests.size();
      for (unsigned i = 0; !hasTarget && i < sizePendingRequests; i++) {
        if (_pendingRequests[i]._ea == newFetchAddr) {
          // TODO: check exception status
          _arch.fetchTranslate(newFetchAddr, fetchRA);

          if ((hasTarget = (_pendingRequests[i]._ra == fetchRA))) {
            if (_instrBuf.size() > _nextDecodeEntry) {
              // reclaim instruction buffer space
              _currInstrBufSpace += (_instrBuf.size() - _nextDecodeEntry) * entrySize;
              _instrBuf.pop_back(_instrBuf.size()-_nextDecodeEntry);
            }

            if (i) {
              // reclaim instruction buffer space
              for (unsigned j = 0; j < i; j++) {
                _mem.cancel(_memPort,_pendingRequests[j]._id, canCancel);
                _currInstrBufSpace += _pendingRequests[j].size();
              }

              _pendingRequests.pop_front(i);
            }
          }
        }
      }
    }

    if (!hasTarget) flushInstrBuf(0, newFetchAddr, _can_cancel);
    _can_cancel = canCancel;

    return !hasTarget;
  }

  TEMPLATE void CLASS::flush(ArchType &uarch,InstrBase &instr,Logger *logger)
  {
    addr_t newFetchAddr = 0;
    if (!_instrBuf.empty()) {
      // If we have entries in the buffer, then the next fetch address will be to
      // this first entry, since it's being flushed.
      newFetchAddr = _instrBuf.front().ea();
    } else if (!_pendingRequests.empty()) {
      // Otherwise- do we have anything that's pending?  If so, that'll be our new
      // next-fetch-address, since we're canceling this one.
      newFetchAddr = _pendingRequests.front().ea;
    } else {
      // Otherwise, we have nothing to cancel, so continue as usual.
      return;
    }

    flushInstrBuf(0, newFetchAddr, _can_cancel);
    _can_cancel = canCancel;
  }

  TEMPLATE void CLASS::cancel(ArchType &uarch,InstrBase &instr,Logger *logger)
  {
    addr_t newFetchAddr = 0;
    if (!_pendingRequests.empty()) {
      // Do we have anything that's pending?  If so, that'll be our new
      // next-fetch-address, since we're canceling this one.
      newFetchAddr = _pendingRequests.front()._ea;
    } else {
      // Otherwise, we have nothing to cancel, so continue as usual.
      return;
    }

    flushInstrBuf(_instrBuf.size(), newFetchAddr, _can_cancel);
    _can_cancel = canCancel;
  }

  TEMPLATE void CLASS::pause(ArchType &uarch,InstrBase &instr,Logger *logger)
  {
    pauseFetch(instr.iid());
  }

  TEMPLATE void CLASS::force(ArchType &uarch,InstrBase &instr,Logger *logger)
  {
    forceFetch();
  }

  TEMPLATE void CLASS::pauseFetch(uint64_t instrId)
  { _tryNewFetch = false; _pauser = instrId; }


  TEMPLATE void CLASS::preemptFetch()
  {
    FetchRequest &request = _pendingRequests.back();

    request._preempted = true;
    _currInstrBufSpace += request.size();
  }


  TEMPLATE bool CLASS::empty() const
  { return _instrBuf.size() <= _nextDecodeEntry && _pendingRequests.empty(); }

  // Generally just called by safe-mode ISSs checking for the next instruction
  // to issue.  First, stages behind the querying instruction are checked, then
  // this is eventually called if no other instructions exist.
  TEMPLATE addr_t CLASS::nextIssueAddr() const
  {
    if (!_instrBuf.empty()) return _instrBuf.front().ea();

    if (!_pendingRequests.empty()) return _pendingRequests.front()._ea;

    return _nextFetchAddr;
  }

  TEMPLATE addr_t CLASS::nextInstrAddr() const
  {
    if (_instrBuf.size() > _nextDecodeEntry)
      return _instrBuf[_nextDecodeEntry].ea();
    else if (!_pendingRequests.empty())
      return _pendingRequests.front()._ea + _pendingRequests.front()._frontPadSize;
    else
      return _nextFetchAddr;
  }


  TEMPLATE void CLASS::instrBufReport() const
  {
    if (logEnabled()) {
      // log instruction buffer data
      for (unsigned i = 0; i < _instrBuf.size(); i++) {
        _logger->logInstrBufEntry(_instrBuf[i].ea(), _instrBuf[i].ra(), entrySize,
                                  _instrBuf[i].data(), _instrBuf[i].predTaken(), _instrBuf[i].predTrg());
      }

      // log pending fetch requests
      for (unsigned i = 0; i < _pendingRequests.size() && !_pendingRequests[i]._preempted; i++) {
          const FetchRequest &request = _pendingRequests[i];

          _logger->logPendingRequest(request._id, request._ea, request._ra, request._fetchSize);
        }
    }
  }


  TEMPLATE bool CLASS::hasInstrSpace()
  {
    if (minFetchEntries) {
      // Make sure we're using signed arithmetic!!!
      return (_nEntries - (int)_instrBuf.size()) >= minFetchEntries;
    }
    else {
      // debug code to check instruction buffer space consistency
      int instrBufSpace = (_nEntries - _instrBuf.size()) * entrySize;

      for (unsigned i = 0; i < _pendingRequests.size(); i++) {
        if (!_pendingRequests[i]._preempted)
          instrBufSpace -= _pendingRequests[i].size();
      }

      // use type conversion to avoid annoying warnings
      if (instrBufSpace < 0 || (unsigned)instrBufSpace != _currInstrBufSpace) {
        assert(false);
      }

      return _currInstrBufSpace >= _fetchSizes[0];
    }
  }


  TEMPLATE void CLASS::flushInstrBuf(unsigned remain, addr_t newFetchAddr, bool flushCanCancel)
  {
    const unsigned bound = _nextDecodeEntry + remain;

    if (_instrBuf.size() > bound) {
      // reclaim instruction buffer space
      _currInstrBufSpace += (_instrBuf.size() - bound) * entrySize;
      // remove instructions
      _instrBuf.pop_back(_instrBuf.size()-bound);
    }
    else if (_instrBuf.size() > _nextDecodeEntry) { // all remain
      remain = _instrBuf.size() - _nextDecodeEntry;
    }
    else { // no data to flush
      remain = 0;
    }

    // TODO: log request cancellation
    // cancel old fetch requests, which may not be preempted
    if (_pendingRequests.size() > 1) {
      for (unsigned i = 0; i < _pendingRequests.size() - 1; i++) {
        _mem.cancel(_memPort,_pendingRequests[i]._id, canCancel);
        _currInstrBufSpace += _pendingRequests[i].size();
      }
    }

    // cancel the most recent fetch request, which may be preempted
    if (!_pendingRequests.empty() && !_pendingRequests.back()._preempted) {
      _mem.cancel(_memPort,_pendingRequests.back()._id, flushCanCancel);
      _currInstrBufSpace += _pendingRequests.back().size();
    }

    _pendingRequests.clear();

    // reset next fetch address
    _nextFetchAddr = !remain ? newFetchAddr
      : _arch.ea(_instrBuf[_nextDecodeEntry + remain - 1].ea() + entrySize);
    _alignedFetch = false;

    if (hasPredictor) {
      // clear branch prediction states
      _predTakenInstrs.clear();
      _incompletePredTakenInstr = false;
    }

    if (flushFetch) _tryNewFetch = true;
  }


  TEMPLATE std::vector<mask_t> CLASS::createAddrMasks() const
  {
    std::vector<mask_t> masks;

    for (unsigned i = 0; i != numFetchSizes; ++i) {
      masks.push_back(_fetchSizes[i] - 1);
    }

    return masks;
  }

  // The idea here is that we create a vector of values to handle the possible
  // values we might get from taking an address and masking it with the largest
  // possible fetch-size mask.  This creates an index we use to index into this
  // list, which then stores the corresponding fetch size, or 0 if not aligned.
  TEMPLATE std::vector<mask_t> CLASS::createAddrFetchSizes() const
  {
    std::vector<mask_t> masks(_fetchSizes.front(),0);

    // Element 0 always represents an aligned fetch with the largest (first)
    // fetch size.
    masks[0] = _fetchSizes[0];
    
    // For each subsequent item, in decreasing size, the element will be even
    // values, starting at its value.  Skip if there's already an entry, since
    // that represents a larger value.  So, for example, if we have fetch sizes
    // of 8, 4, and 2 then element 0 represents a fetch size of 8, since we've
    // masked the address with (8-1) = 7 and received 0.  A value of 4 means a
    // fetch size of 4, and a value of 2 or 6 means a fetch size of 2.
    for (unsigned i = 1; i < numFetchSizes; ++i) {
      unsigned fs = _fetchSizes[i];
      for ( unsigned j = fs; j < masks.size(); j += fs) {
        if (!masks[j]) {
          masks[j] = fs;
        }
      }
    }
    
    return masks;
  }

  TEMPLATE bool CLASS::set_dyn_parm(const std::string &parm,unsigned value)
  {
    if (parm == "fetch-num-entries") {
      _nEntries = value;
      full_reset();
      return true;
    } else {
      return false;
    }
  }
  
  TEMPLATE bool CLASS::get_dyn_parm(unsigned &value,const std::string &parm) const
  {
    if (parm == "fetch-num-entries") {
      value = _nEntries;
      return true;
    } else {
      return false;
    }
  }
  
  TEMPLATE void CLASS::list_dyn_parm(adl::StrPairs &parms) const
  {
    parms.push_back(make_pair(std::string("fetch-num-entries"),std::string("Number of entries in the fetch unit.")));
  }

} // namespace uadl

#undef TEMPLATE
#undef CLASS

#endif // _DEFAULT_FETCH_UNIT_H

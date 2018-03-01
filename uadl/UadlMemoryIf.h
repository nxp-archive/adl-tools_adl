//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
#ifndef _UADL_MEMORY_IF_H
#define _UADL_MEMORY_IF_H

#include <string>

#include "UadlTypes.h"
#include "Named.h"

namespace uadl {

  using adl::byte_t;
  using adl::addr_t;


  class Logger;
  class CacheStatusHint;
  class MemoryPort;
  class MemoryRequest;

  typedef adl::CacheAccess MemRequestType;

  /*******************************************************************************
   * Uadl memory interface
   *
   * TODO:
   *   - interface for nAvailableRequestSlots()
   *   - update WriteQueue to inherit from Memory (empty, lock, etc.)
   ******************************************************************************/
  class UadlMemoryIf : public Named {
  public:
    UadlMemoryIf(const std::string &name = "") : Named(name) {}

    virtual ~UadlMemoryIf() {}

    virtual void setMemParms(unsigned addrBandwidth, unsigned dataBandwidth, unsigned readLatency,
                             unsigned writeLatency, unsigned queueSize, bool allowBusWait, bool preemptive,
                             bool readOnly) = 0;
    
    virtual MemoryPort *addPort(const std::string &name, bool commonBandwidth,
                                unsigned addrBandwidth, unsigned dataBandwidth,
                                unsigned readLatency, unsigned writeLatency,
                                unsigned maxRequest,bool allowBusWait) = 0;

    virtual void reset() {}

    virtual void set_level(unsigned) = 0;
    virtual unsigned get_level_mask() const = 0;

    virtual unsigned memStalls(unsigned portId) const { return 0; };

    // Untimed access interface for debug purposes.  The type parameter for the
    // get routine allows you to distinguish between instruction and data debug
    // reads.
    virtual bool getMemoryData(adl::CacheAccess type, addr_t addr, unsigned size,
                               byte_t *data) { return false; }
    virtual bool setMemoryData(addr_t addr, unsigned size, const byte_t *data);

    // Untimed access interface for execution purposes.  This defaults to using
    // the debug API, but may be changed for when a platform model must
    // distinguish between the two.
    virtual bool fetchMemoryData(addr_t addr, unsigned size, byte_t *data,
                                 uint64_t &exception, bool cacheEnabled = false);
    virtual bool readMemoryData(addr_t addr, unsigned size, byte_t *data,
                                uint64_t &exception, bool cacheEnabled = false);
    virtual bool writeMemoryData(addr_t addr, unsigned size, const byte_t *data,
                                 uint64_t &exception, bool cacheEnabled = false);

    // Transaction interface.  For safe-mode models, this should just consume
    // time/bandwidth.  The memory contents are not actually used.
    virtual bool emptyInquire(Logger *logger) const = 0;
    virtual bool inactiveInquire(Logger *logger) const = 0;
    virtual bool serializedInquire(Logger *machine, unsigned serialized, unsigned portId) const = 0;

    virtual bool sendRequestInquire(InstrBase *instr,Logger *machine, MemRequestType type,unsigned serialized,
                                    unsigned portId = 0) const = 0;
    virtual unsigned sendFetchRequest(addr_t ea, addr_t ra, unsigned size,
                                      unsigned serialized, uint64_t &exception, const CacheStatusHint *hint, 
                                      const ReqInfo &rinfo, unsigned portId = 0) = 0;
    virtual unsigned sendReadRequest(addr_t ea, addr_t ra, unsigned size,
                                     unsigned serialized, uint64_t &exception, const ReqInfo &rinfo,
                                     unsigned portId = 0) = 0;
    virtual unsigned sendWriteRequest(addr_t ea, addr_t ra, unsigned size,
                                      unsigned serialized, uint64_t &exception, const byte_t *data = NULL,
                                      bool writeThrough = false, const ReqInfo &rinfo = ReqInfo(),
                                      unsigned portId = 0, bool isEvict = false) = 0;

    // These variants are used only by the cache class, in order to pass more
    // state information.
    virtual unsigned sendReadRequest(addr_t ea, addr_t ra, unsigned size,
                                     unsigned serialized, uint64_t &exception,
                                     const CacheStatusHint *hint,
                                     const ReqInfo &rinfo, unsigned portId = 0);
    virtual unsigned sendWriteRequest(addr_t ea, addr_t ra, unsigned size,
                                      unsigned serialized,uint64_t &exception,
                                      const CacheStatusHint *hint,
                                      const byte_t *data = NULL,
                                      bool writeThrough = false, const ReqInfo &rinfo = ReqInfo(),
                                      unsigned portId = 0, bool isEvict = false);

    virtual bool readInquire(InstrBase *instr,Logger *logger, unsigned portId, unsigned requestId) const = 0;
    virtual void read(unsigned portId, unsigned requestId, byte_t *data) = 0;

    virtual bool writeInquire(InstrBase *instr,Logger *logger, unsigned portId, unsigned requestId) const = 0;
    virtual void write(unsigned portId, unsigned requestId, const byte_t *data) = 0;

    virtual void setRequestSize(unsigned portId,unsigned requestId,unsigned dataWidth) = 0;

    virtual bool lockInquire(Logger *logger) const;
    virtual void lockAction(InstrBase *instr) {}
    virtual bool unlockInquire(Logger *logger) const;
    virtual void unlockAction() {}
    virtual void unlockAction(unsigned delay) {}

    // Non-transaction interface
    virtual void cancel(unsigned portId, unsigned requestId, bool deep) = 0;
    virtual void cancel(unsigned requestId, bool deep) = 0;

    // per time step methods
    virtual void preCycle() {};
    virtual void postCycle() {};
    virtual void logAllRequests() {};

    // Access/query methods
    virtual bool isActive() const = 0;

    virtual unsigned nAvailableRequestSlots() const = 0;
    virtual unsigned nAvailableRequestSlots(unsigned portId) const = 0;
  };


  // inline methods
  inline bool UadlMemoryIf::setMemoryData(addr_t addr, unsigned size,
                                          const byte_t *data)
  { return false; }


  inline bool UadlMemoryIf::fetchMemoryData(addr_t addr, unsigned size, byte_t *data,
                                            uint64_t &exception,
                                            bool cacheEnabled)
  { return getMemoryData(adl::CacheIFetch, addr, size, data); }


  inline bool UadlMemoryIf::readMemoryData(addr_t addr, unsigned size, byte_t *data,
                                           uint64_t &exception, bool cacheEnabled)
  { return getMemoryData(adl::CacheRead, addr, size, data); }


  inline bool UadlMemoryIf::writeMemoryData(addr_t addr, unsigned size,
                                            const byte_t *data,
                                            uint64_t &exception,
                                            bool cacheEnabled)
  { return setMemoryData(addr, size, data); }


  inline unsigned UadlMemoryIf::sendReadRequest(addr_t ea, addr_t ra, unsigned size,
                                                unsigned serialized,uint64_t &exception,
                                                const CacheStatusHint *hint,
                                                const ReqInfo &rinfo, unsigned portId)
  { return sendReadRequest(ea, ra, size, serialized, exception, rinfo, portId); }


  inline unsigned UadlMemoryIf::sendWriteRequest(addr_t ea, addr_t ra, unsigned size,
                                                 unsigned serialized,uint64_t &exception,
                                                 const CacheStatusHint *hint,
                                                 const byte_t *data,
                                                 bool writeThrough, const ReqInfo &rinfo,
                                                 unsigned portId, bool isEvict)
  {
    return sendWriteRequest(ea, ra, size, serialized, exception, data, writeThrough, rinfo,
                            portId, isEvict);
  }


  inline bool UadlMemoryIf::lockInquire(Logger *logger) const
  { return true; }


  inline bool UadlMemoryIf::unlockInquire(Logger *logger) const
  { return true; }

} // namespace uadl

#endif // _UADL_MEMORY_IF_H

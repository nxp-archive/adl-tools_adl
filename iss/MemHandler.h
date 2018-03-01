//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// If a model is generated with the --extern-mem option, then this interface
// class is used to access memory.  Derive from this class and then supply a
// reference to each relevant IssNode object.
//
// Where relevant, an external memory can set a latency value by calling
// set_latency().  This will set a latency value which will then be used by, for
// example, a performance model.
//

#ifndef _MEMHANDLER_H_
#define _MEMHANDLER_H_

#include <vector>

#include "helpers/BasicTypes.h"

namespace adl {

  struct MemHandler {
    typedef std::vector<unsigned> Latencies;

    MemHandler() {};
    virtual ~MemHandler() {};

    virtual uint64_t read64(CacheAccess ca,addr_t ra) = 0;
    virtual uint32_t read32(CacheAccess ca,addr_t ra) = 0;
    virtual uint16_t read16(CacheAccess ca,addr_t ra) = 0;
    virtual uint8_t  read8(CacheAccess ca,addr_t ra) = 0;
    virtual void     write64(addr_t ra, uint64_t v, unsigned nb) = 0;
    virtual void     write32(addr_t ra, uint32_t v, unsigned nb) = 0;
    virtual void     write16(addr_t ra, uint16_t v, unsigned nb) = 0;
    virtual void     write8 (addr_t ra, uint8_t  v) = 0;
    virtual void     readpage(CacheAccess ca,byte_t *t,size_t n,addr_t addr,addr_t critical_addr) = 0;
    virtual void     writepage(byte_t *s,size_t n,addr_t addr,addr_t critical_addr) = 0;
    virtual void     fillpage(unsigned char c,size_t n,addr_t addr) {};
    virtual void     reset() = 0;

    // Only used for safe-mode uADL models.  For time-tagged ISSs, use
    // IssNode::addLatency() to add latency on an external memory operation.
    void set_latency(unsigned l)         { clear_latency(); add_latency(l); };
    void add_latency(unsigned l)         { _latencies.push_back(l); };
    void clear_latency()                 { _latencies.clear(); };
    const Latencies &get_latency() const { return _latencies; };

    // For use with external libraries requiring locking for atomic operations.
    virtual void lock() {};
    virtual void release() {};

  private:
    Latencies _latencies;
  };

}

#endif

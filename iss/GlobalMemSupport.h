//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//


// This file contains the global Mem interface
#ifndef _GLOBAL_MEM_SUPPORT_H_
#define _GLOBAL_MEM_SUPPORT_H_

#include "GlobalMemoryName.h"

namespace adl {

# ifdef ADL_HEADER

  void mem_readpage(byte_t *t,size_t n,addr_t addr,addr_t critical_addr);
  uint64_t mem_read64(addr_t addr);
  uint32_t mem_read32(addr_t addr);
  uint32_t Mem_log_read32(addr_t addr);
  uint16_t mem_read16(addr_t addr);
  uint8_t mem_read8(addr_t addr);
  void mem_writepage(byte_t *s,size_t n,addr_t addr,addr_t critical_addr);
  void mem_write64(addr_t addr,uint64_t value,unsigned nb);
  void mem_write32(addr_t addr,uint32_t value,unsigned nb);
  void mem_write16(addr_t addr,uint16_t value,unsigned nb);
  void mem_write8(addr_t addr,uint8_t value);
  void resetMemory();

# else

  void mem_readpage(byte_t *t,size_t n,addr_t addr,addr_t crit_addr)
  {
    GlobalMemory.readpage(t,n,addr,crit_addr);
  }

  uint64_t mem_log_read64(addr_t addr)
  {
    return GlobalMemory.read64(addr);
  }

  uint32_t mem_log_read32(addr_t addr)
  {
    return GlobalMemory.read32(addr);
  }

  uint16_t mem_log_read16(addr_t addr)
  {
    return GlobalMemory.read16(addr);
  }

  uint8_t mem_log_read8(addr_t addr)
  {
    return GlobalMemory.read8(addr);
  }

  uint64_t mem_read64(addr_t addr)
  {
    return GlobalMemory.read64(addr);
  }

  uint32_t mem_read32(addr_t addr)
  {
    return GlobalMemory.read32(addr);
  }

  uint16_t mem_read16(addr_t addr)
  {
    return GlobalMemory.read16(addr);
  }

  uint8_t mem_read8(addr_t addr)
  {
    return GlobalMemory.read8(addr);
  }

  void mem_writepage(byte_t *s,size_t n,addr_t addr,addr_t crit_addr)
  {
    GlobalMemory.writepage(s,n,addr,crit_addr);
  }

  void mem_write64(addr_t addr,uint64_t value,unsigned nb)
  {
    GlobalMemory.write64(addr,value,nb);
  }

  void mem_write32(addr_t addr,uint32_t value,unsigned nb)
  {
    GlobalMemory.write32(addr,value,nb);
  }

  void mem_write16(addr_t addr,uint16_t value,unsigned nb)
  {
    GlobalMemory.write16(addr,value,nb);
  }

  void mem_write8(addr_t addr,uint8_t value)
  {
    GlobalMemory.write8(addr,value);
  }

  void resetMemory()
  {
    GlobalMemory.reset();
  }

# endif

}

#endif

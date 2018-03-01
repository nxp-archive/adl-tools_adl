//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This is a simple driver program to test a model's ability to disassemble
// a chunk of memory.
//

#include <assert.h>
#include <iostream>
#include <iomanip>
#include <stdarg.h>
#include <sstream>
#include <stdexcept>

//#define VLE

#ifdef USE_LOGGING_MGR
# include "rnumber/RNumber.h"
#else
# define __NO_RNUMBER__
#endif

#include "helpers/Macros.h"

#ifndef IssNamespace
# define IssNamespace DEFAULT_ISS_NAMESPACE
#endif

#define ISS_NAMESPACE IssNamespace
#include "iss/ModelInterface.h"
#include "iss/Memory.h"
#include "iss/MemHandler.h"

#ifdef UADL
# include "uadl/UadlArchIf.h"
#endif

using namespace std;
using namespace adl;

//
// For this example, the memory is implemented as a 1 Mb array.  The byte_*
// routines are helper functions declared in Memory.h.
//

// External memory.
enum { MemSize = 0x100000, MemMask = (MemSize-1) };

bool MemLogging = true;

byte_t external_mem[MemSize];

uint32_t extern_mem_read32(addr_t ra)
{
  return byte_read32(external_mem,(ra & MemMask));
}

void extern_mem_write32(addr_t ra, uint32_t v, unsigned nb = 4)
{
  byte_write32(external_mem,(ra & MemMask),v,nb);
}

struct MyMem : public MemHandler {

  MyMem(IssNode *node,unsigned excpt,addr_t exception_addr = 0) : _node(node), _excpt(excpt), _exception_addr(exception_addr) {};

  uint64_t read64(CacheAccess ca,addr_t ra)
  {
    log_access(ca,ra,64);
    return byte_read64(external_mem,(ra & MemMask));
  }

  uint32_t read32(CacheAccess ca,addr_t ra)
  {
    log_access(ca,ra,32);
    if (_excpt && ra == _exception_addr) {
      _node->genExceptionImm(_excpt);
    }
    return byte_read32(external_mem,(ra & MemMask));
  }

  uint16_t read16(CacheAccess ca,addr_t ra)
  {
    log_access(ca,ra,16);
    return byte_read16(external_mem,(ra & MemMask));
  }

  uint8_t  read8(CacheAccess ca,addr_t ra)
  {
    log_access(ca,ra,8);
    return byte_read8(external_mem,(ra & MemMask));
  }

  void write64(addr_t ra, uint64_t v, unsigned nb)
  {
    log_access(CacheWrite,ra,64);
    byte_write64(external_mem,(ra & MemMask),v,nb);
  }

  void write32(addr_t ra, uint32_t v, unsigned nb)
  {
    log_access(CacheWrite,ra,32);
    byte_write32(external_mem,(ra & MemMask),v,nb);
  }

  void write16(addr_t ra, uint16_t v, unsigned nb)
  {
    log_access(CacheWrite,ra,16);
    byte_write16(external_mem,(ra & MemMask),v,nb);
  }

  void write8 (addr_t ra, uint8_t  v)
  {
    log_access(CacheWrite,ra,8);
    byte_write8(external_mem,(ra & MemMask),v);
  }

  void readpage(CacheAccess ca,byte_t *t,size_t n,addr_t addr,addr_t crit_addr)
  {
    log_access(ca,addr,n);
    byte_pagecopy(t,0,external_mem,(addr & MemMask),n);
  }

  void writepage(byte_t *s,size_t n,addr_t addr,addr_t crit_addr)
  {
    log_access(CacheWrite,addr,n);
    byte_pagecopy(external_mem,(addr & MemMask),s,0,n);
  }

  void reset()
  {
    memset(external_mem,0,MemSize);
  }

  void log_access(CacheAccess ca,addr_t ra,int size)
  {
    if (MemLogging) {
      switch (ca) {
      case CacheIFetch:
        cout << "# Instruction read:  0x" << hex << ra << "\n";
        break;
      case CacheWrite:
        cout << "# Data write (" << dec << size << " bits):  0x" << hex << ra << "\n";
        break;
      case CacheRead:
        cout << "# Data read (" << dec << size << " bits):  0x" << hex << ra << "\n";
        break;
      case CacheLogRead:
        cout << "# Data log read (" << dec << size << " bits):  0x" << hex << ra << "\n";
        break;
      case CacheILogRead:
        cout << "# Instruction log read:  0x" << hex << ra << "\n";
        break;
      default:
        ;
      }
    }
  }

private:
  IssNode *_node;
  unsigned _excpt;
  addr_t   _exception_addr;

};

//
// These are some helper functions for displaying model state.
//

void display_memory(int cnt, ... )
{
  va_list ap;
  va_start(ap,cnt);
  for (int i = 0; i != cnt; ++i) {
    unsigned addr = va_arg(ap,unsigned);
    cout << "MD n=Mem ra=0x" << hex << addr << " d=0x" << hex 
         << extern_mem_read32(addr) << "\n";
  }
  va_end(ap);
}


int main(int argc,const char *argv[])
{
  addr_t start = 0, end = 0;
  string table;

  for (int i = 1; i != argc; ++i) {
    if (!strcmp(argv[i],"-table")) {
      table = argv[++i];
    }
    else if (!strcmp(argv[i],"-start")) {
      start = strtoul(argv[++i],0,0);
    }
    else if (!strcmp(argv[i],"-end")) {
      end = strtoul(argv[++i],0,0);
    }
  }

  if (start == 0 && end == 0) {
    cerr << "No start/end disassembly addresses specified.\n";
    return 1;
  }
 
  MemLogging = false;

  unsigned id = 0;
  IssNode *root = IssNamespace::createTopLevelNode(id);

  int tindex = -1;
  if (!table.empty()) {
    tindex = IssNamespace::getAttrData().get_attr(table);
    if (tindex < 0) {
      cerr << "The specified instruction table does not exist:  " << table << "\n";
      return 1;
    }
  }

  MyMem mem(root,0,0);

  root->setMemHandler(&mem);

  // This initializes memory to the program to be run.
  extern_mem_write32(0x00000200,0x04897f5b);
  extern_mem_write32(0x10000204,0xd051e2fd);

  extern_mem_write32(0x00000100,0x80401000);  // lwz r2,0x1000(0)
  extern_mem_write32(0x00000104,0x394A0100);  // addi r10,r10,0x100
  extern_mem_write32(0x00000108,0x7C620A14);  // add r3,r2,r1
  extern_mem_write32(0x0000010c,0x34A5FFFE);  // subic. r5,r5,2
  extern_mem_write32(0x00000110,0x90640000);  // stw r3,0(r4)
  extern_mem_write32(0x00000114,0x90660000);  // stw r3,0(r6)
  extern_mem_write32(0x00000118,0xA0E01004);  // lhz  r7,0x1004(r0)
  extern_mem_write32(0x0000011c,0x89001005);  // lbz  r8,0x1005(r0)
  extern_mem_write32(0x00000120,0x99002004);  // stb  r8,0x2004(r0)
  extern_mem_write32(0x00000124,0xB0E02008);  // sth  r7,0x2008(r0)

  cout << "Disassembly:\n";

  while (start < end) {
    cout << '\n' << hex << start << ":\t" << dec;
    start += root->disassemble(cout,start,true,tindex);
  }

  cout << "\n\n";

}



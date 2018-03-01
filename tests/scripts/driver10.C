//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// A simple driver program to test that we can use an ELF and DAT reader with a standalone ISS.
//

#include <assert.h>
#include <iostream>
#include <iomanip>
#include <stdarg.h>
#include <sstream>
#include <stdexcept>

#include "rnumber/RNumber.h"

#include "helpers/Macros.h"

#ifndef IssNamespace
# define IssNamespace DEFAULT_ISS_NAMESPACE
#endif

#define ISS_NAMESPACE IssNamespace
#include "iss/ModelInterface.h"
#include "iss/Memory.h"
#include "iss/MemHandler.h"
#include "iss/StringHelpers.h"

#ifdef UADL
# include "uadl/UadlArchIf.h"
#endif

#include "iss/ElfReader.h"
#include "iss/DatReader.h"
#include "iss/TypeIO.h"

using namespace std;
using namespace adl;

//
// For this example, the memory is implemented as a 1 Mb array.  The byte_*
// routines are helper functions declared in Memory.h.
//

// External memory.
enum { MemSize = 0x100000, MemMask = (MemSize-1) };

byte_t external_mem[MemSize];

uint32_t extern_mem_read32(addr_t ra)
{
  return byte_read32(external_mem,(ra & MemMask));
}

void extern_mem_write32(addr_t ra, uint32_t v, uint32_t m)
{
  byte_write32(external_mem,(ra & MemMask),v,m);
}

struct MyMem : public MemHandler {

  MyMem(IssNode *node,unsigned excpt,addr_t exception_addr = 0) : _node(node), _excpt(excpt), _exception_addr(exception_addr) {};

  uint64_t read64(CacheAccess ca,addr_t ra)
  {
    return byte_read64(external_mem,(ra & MemMask));
  }

  uint32_t read32(CacheAccess ca,addr_t ra)
  {
    if (_excpt && ra == _exception_addr) {
      _node->genExceptionImm(_excpt);
    }
    return byte_read32(external_mem,(ra & MemMask));
  }

  uint16_t read16(CacheAccess ca,addr_t ra)
  {
    return byte_read16(external_mem,(ra & MemMask));
  }

  uint8_t  read8(CacheAccess ca,addr_t ra)
  {
    return byte_read8(external_mem,(ra & MemMask));
  }

  void write64(addr_t ra, uint64_t v, unsigned nb)
  {
    byte_write64(external_mem,(ra & MemMask),v,nb);
  }

  void write32(addr_t ra, uint32_t v, unsigned nb)
  {
    byte_write32(external_mem,(ra & MemMask),v,nb);
  }

  void write16(addr_t ra, uint16_t v, unsigned nb)
  {
    byte_write16(external_mem,(ra & MemMask),v,nb);
  }

  void write8 (addr_t ra, uint8_t  v)
  {
    byte_write8(external_mem,(ra & MemMask),v);
  }

  void readpage(CacheAccess ca,byte_t *t,size_t n,addr_t addr,addr_t crit_addr)
  {
    byte_pagecopy(t,0,external_mem,(addr & MemMask),n);
  }

  void writepage(byte_t *s,size_t n,addr_t addr,addr_t crit_addr)
  {
    byte_pagecopy(external_mem,(addr & MemMask),s,0,n);
  }

  void reset()
  {
    memset(external_mem,0,MemSize);
  }

private:
  IssNode *_node;
  unsigned _excpt;
  addr_t   _exception_addr;

};

//
// These are the logging functions.  For this example, they simply display their
// results in a format similar to the DAT trace format used by the standalone ADL
// ISS executables.
//

static addr_t instr_ea, instr_ra;

namespace adl {

struct MyLogger : public LogBase {

  void log_instr_prefetch(addr_t ea)
  {
    cout << "\nI ea=0x" << hex << ea << "\n";
  }

  void log_instr_read(unsigned id,const char *name,addr_t ea,
                      addr_t ra,uint32_t data)
  {
    instr_ea = ea;
    instr_ra = ra;
    cout << "M n=Mem t=ifetch ea=0x" << hex << ea << " ra=0x" 
         << ra << " d=0x" << data << "\n";
  }

  void log_instr(const uint32_t* opc,int num_half_bytes,const char *name,Disassembler dis,uint32_t flags)
  {
    cout << "INSTR op=0x" << hex << setfill('0') << setw(num_half_bytes) << (*opc >> ((8-num_half_bytes)*4)) << " asm=\"";
    dis(cout,instr_ea,opc);
    cout << "\"\n";
  }

  void log_reg_write(unsigned id,const char *name,REGTYPE value)
  {
    cout << "R n=" << name << " d=0x" << hex << value << "\n";
  }

  void log_regfile_write(unsigned id,const char *name,uint32_t index,
                         REGTYPE value)
  {
    cout << "R n=" << name << " i=" << dec << index 
         << " d=0x" << hex << value << "\n";
  }

  void log_core_mem_write(unsigned id,const char *name,
                          addr_t ea,int nb)
  {
    cout << "D n=Mem t=write ea=0x" << hex << ea 
         << " nb=" << dec << nb << "\n";
  }

  void log_core_mem_read(unsigned id,const char *name,
                         addr_t ea,int nb)
  {
    cout << "D n=Mem t=read ea=0x" << hex << ea 
         << " nb=" << dec << nb << "\n";
  }

  void log_core_mem_write_typed(unsigned id,const char *name,
                                addr_t ea,int nb,CacheAccess type)
  {
    cout << "D n=Mem t=" << type << " ea=0x" << hex << ea 
         << " nb=" << dec << nb << " typed=true\n";
  }

  void log_core_mem_read_typed(unsigned id,const char *name,
                               addr_t ea,int nb,CacheAccess type)
  {
    cout << "D n=Mem t=" << type << " ea=0x" << hex << ea 
         << " nb=" << dec << nb << " typed=true\n";
  }

  void log_mem_write(unsigned id,const char *name,bool pre,int seq,addr_t ea,
                     addr_t ra,uint32_t value)
  {
    if (!pre) {
      cout << "M n=Mem t=write ea=0x" << hex << ea 
           << " ra=0x" << ra << " d=0x" << value << "\n";
    }
  }

  void log_mem_read(unsigned id,const char *name,bool pre,int seq,addr_t ea,
                    addr_t ra,uint32_t value)
  {
    if (!pre) {
      cout << "M n=Mem t=read ea=0x" << hex << ea 
           << " ra=0x" << ra << " d=0x" << value << "\n";
    }
  }

  void log_annotation(MsgType type,unsigned level,const std::string &msg)
  {
    cout << "A l=" << level << " m=\"" << msg << "\"\n";
  }

  void log_cache_action(const char *name,CacheAction action,CacheAccess access,unsigned level,
                        int set,int way,unsigned linemask,addr_t ra)
  {
    cout << "C n=" << name;
    switch (action) {
    case CacheLoad:
      cout << " a=load";
      break;
    case CacheHit:
      cout << " a=hit";
      break;
    case CacheMiss:
      cout << " a=miss";
      break;
    case CacheEvict:
      cout << " a=evict";
      break;
    case CacheNone:
      break;
    }

    switch (access) {
    case CacheNoAccess:
      cout << " t=none";
      break;
    case CacheIFetch:
      cout << " t=ifetch";
      break;
    case CacheRead:
      cout << " t=read";
      break;
    case CacheStore:
    case CacheWrite:
      cout << " t=write";
      break;
    case CacheFlush:
      cout << " t=flush";
      break;
    case CacheTouch:
      cout << " t=touch";
      break;
    case CacheAlloc:
      cout << " t=alloc";
      break;
    case CacheInvalidate:
      cout << " t=invalidate";
      break;
    case CacheLock:
      cout << " t=lock";
      break;
    case CacheUnlock:
      cout << " t=unlock";
      break;
    default:
      break;
    }

    cout << dec;

    if (set >= 0) {
      cout << " set=" << set;
    }
    if (way >= 0) { 
      cout << " way=" << way;
    }

    cout << " lm=0x" << hex << linemask << " ra=0x" << (ra & ~((addr_t)linemask)) << dec << "\n";    
  }

};

}

MyLogger mylogger;

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

struct Reporter : ReportBase
{
    virtual  void report_reg (const IssNode *,unsigned id,const char *name,REGTYPE value,bool shared) {
      cout << "RD n=" << name << " d=0x" << hex << setfill('0') << setw(8) << value << "\n";
    }

    virtual  void report_regfile (const IssNode *,unsigned id,const char *name,int index,REGTYPE value,bool shared) {
      cout << "RD n=" << name << " i=" << dec << index << " d=0x" << hex << setfill('0') << setw(8) << value << "\n";
    }

    virtual void report_cache(const IssNode *,const char *name,CacheType type,unsigned level,addr_t addr,unsigned set,unsigned way,
                              bool valid,const FieldData &fd,bool shared,const byte_t *data,unsigned n) {
      if (valid) {
        cout << "CD n=" << name << " set=" << dec << set << " way=" << way << hex 
             << " ra=0x" << setfill('0') << setw(sizeof(addr_t)*2) << setfill('0') << addr << dec;
        ForEach(fd,i) {
          cout << " " << i->_name << "=" << i->_value;
        }
        cout << " d=" << hex;
        bool first = true;
        for (unsigned i = 0; i < n; i += 4) {
          if (!first) cout << ",";
          first = false;
          cout << "0x" << setw(8) << setfill('0') << byte_read32(data,i);
        }
        cout << dec << "\n";
      }
    }
};

unsigned get_exception_id(IssNode *node,const string &name)
{
  const ExceptionInfos &einfo = node->getExceptionInfo();
  ForEach(einfo,i) {
    if (i->_name == name) {
      return i->_id;
    }
  }
  RError("Unknown exception name " << name);
}

int main(int argc,const char *argv[])
{
  try {
    bool trace = true;
    unsigned id = 0;

    string elf_fn, dat_fn;

    for (int i = 1; i != argc; ++i) {
      if (!strcmp(argv[i],"-trace")) {
        trace = true;
      }
      else if (!strcmp(argv[i],"-no-trace")) {
        trace = false;
      }
      else if (!strcmp(argv[i],"-elf")) {
        elf_fn = argv[++i];
      }
      else if (!strcmp(argv[i],"-dat")) {
        dat_fn = argv[++i];
      }
    }

    // This instantiates the model.

    IssNode *root = IssNamespace::createTopLevelNode(id);

    MyMem mem(root,0,0);

    root->setLogger(&mylogger);
    root->setMemHandler(&mem);

    // If we have an ELF file, then read it here.
    if (!elf_fn.empty()) {
      ElfReaderFactory erf;
      Args args;
      args.push_back("no-elf-pa");
      Reader *er = erf.create(elf_fn,root,&args);
      er->useFileEntryPoint(true);
      if (!er->readFile()) {
        RError("Could not read ELF file '" << elf_fn << "'.");
      }
    }

    // If we have a DAT file, then read it here.
    if (!dat_fn.empty()) {
      Reader *dr = new DatReader(dat_fn,root);
      if (!dr->readFile()) {
        RError("Could not read DAT file '" << dat_fn << "'.");
      }
    }

    if (trace) {
      root->set_tracing_r();
    }

    cout << "CORE n=:P\n";


    cout << "TRACE\n";

    root->run();

    cout << "\nRESULTS\n";

    // Final results display.
    Reporter reporter;

    display_memory(4,0x1000,0x1004,0x2000,0x2004);
    root->showRegs(reporter);
    root->showCaches(reporter);
  }
  catch (exception &err) {
    cerr << "Error:  " << err.what() << "\n\n";
    return 1;
  }
}


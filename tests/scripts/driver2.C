//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This is a simple driver program that can link against a bare-bones ISS
// that does not have the standalone infrastructure.
//
// You may define IssNamespace to specify an alternate namespace to be included.
//

#include <assert.h>
#include <iostream>
#include <iomanip>
#include <stdarg.h>
#include <sstream>
#include <stdexcept>
#include <memory>

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
    log_access(ca,ra & ~DWMask,64);
    return byte_read64(external_mem,(ra & MemMask));
  }

  uint32_t read32(CacheAccess ca,addr_t ra)
  {
    log_access(ca,ra & ~WordMask,32);
    if (_excpt && ra == _exception_addr) {
      _node->genExceptionImm(_excpt);
    }
    return byte_read32(external_mem,(ra & MemMask));
  }

  uint16_t read16(CacheAccess ca,addr_t ra)
  {
    log_access(ca,ra & ~HWMask,16);
    return byte_read16(external_mem,(ra & MemMask));
  }

  uint8_t  read8(CacheAccess ca,addr_t ra)
  {
    log_access(ca,ra,8);
    return byte_read8(external_mem,(ra & MemMask));
  }

  void write64(addr_t ra, uint64_t v, unsigned nb)
  {
    log_access(CacheWrite,ra & ~DWMask,64);
    byte_write64(external_mem,(ra & MemMask),v,nb);
  }

  void write32(addr_t ra, uint32_t v, unsigned nb)
  {
    log_access(CacheWrite,ra & ~WordMask,32);
    byte_write32(external_mem,(ra & MemMask),v,nb);
  }

  void write16(addr_t ra, uint16_t v, unsigned nb)
  {
    log_access(CacheWrite,ra & ~HWMask,16);
    byte_write16(external_mem,(ra & MemMask),v,nb);
  }

  void write8 (addr_t ra, uint8_t  v)
  {
    log_access(CacheWrite,ra,8);
    byte_write8(external_mem,(ra & MemMask),v);
  }

  void readpage(CacheAccess ca,byte_t *t,size_t n,addr_t addr,addr_t crit_addr)
  {
    log_access(ca,addr,crit_addr,n);
    byte_pagecopy(t,0,external_mem,(addr & MemMask),n);
  }

  void writepage(byte_t *s,size_t n,addr_t addr,addr_t crit_addr)
  {
    log_access(CacheWrite,addr,crit_addr,n);
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

  void log_access(CacheAccess ca,addr_t ra,addr_t crit_ra,int size)
  {
    if (MemLogging) {
      switch (ca) {
      case CacheIFetch:
        cout << "# Instruction read:  0x" << hex << ra << ", critical:  0x" << crit_ra << "\n";
        break;
      case CacheWrite:
        cout << "# Data write (" << dec << size << " bits):  0x" << hex << ra << ", critical:  0x" << crit_ra << "\n";
        break;
      case CacheRead:
        cout << "# Data read (" << dec << size << " bits):  0x" << hex << ra << ", critical:  0x" << crit_ra << "\n";
        break;
      case CacheLogRead:
        cout << "# Data log read (" << dec << size << " bits):  0x" << hex << ra << ", critical:  0x" << crit_ra << "\n";
        break;
      case CacheILogRead:
        cout << "# Instruction log read:  0x" << hex << ra << ", critical:  0x" << crit_ra << "\n";
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

#ifdef USE_LOGGING_MGR

#include "iss/LoggingMgr.h"

struct MyLogger : public LogParentIface, public LoggingIface {

  addr_t _instr_ea;

  LoggingIface *register_core(IssCore &core)
  {
    return this;
  }

  void logInstrPrefetch(addr_t ea)
  {
    cout << "\nI ea=0x" << hex << ea << "\n";
  }

  void logInstrRead(unsigned id,const char *name,addr_t ea,
                    addr_t ra,uint32_t data)
  {
    _instr_ea = ea;
  }

  void logInstr(const uint32_t* opc,int num_half_bytes,const char *name,Disassembler dis)
  {
    cout << "INSTR op=0x" << hex << setfill('0') << setw(num_half_bytes) << (*opc >> ((8-num_half_bytes)*4)) << " asm=\"";
    dis(cout,_instr_ea,opc);
    cout << "\"\n";
  }  

};

#else

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

  void log_reg_write(unsigned id,const char *name,uint64_t value)
  {
    cout << "R n=" << name << " d=0x" << hex << value << "\n";
  }

  void log_regfile_write(unsigned id,const char *name,uint32_t index,
                         uint64_t value)
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

#endif

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
  int prog = 0;
  unsigned id = 0;

  for (int i = 1; i != argc; ++i) {
    if (!strcmp(argv[i],"-trace")) {
      trace = true;
    }
    else if (!strcmp(argv[i],"-no-trace")) {
      trace = false;
    }
    if (!strcmp(argv[i],"-mem-logging")) {
      MemLogging = true;
    }
    else if (!strcmp(argv[i],"-no-mem-logging")) {
      MemLogging = false;
    }
    else if (!strcmp(argv[i],"-prog")) {
      prog = atoi(argv[++i]);
    }
  }

  // This instantiates the model.
# ifdef UADL
  uadl::UadlArch *model = uadl::createArch("P");
  IssNode *root = &model->ISS();
# else
  IssNode *root = IssNamespace::createTopLevelNode(id);
# endif


# ifdef TrapAddr
  MyMem mem(root,get_exception_id(root,"DataStorage"),TrapAddr);
# else
  MyMem mem(root,0,0);
# endif  

# ifdef USE_LOGGING_MGR
  auto_ptr<LoggingMgr> mgr(createLoggingMgr(false));
  mgr->installLoggers(root);
# else
  root->setLogger(&mylogger);
# endif

  root->setMemHandler(&mem);

  if (trace) {
    root->set_tracing_r();
  }

  cout << "CORE n=:P\n";

  // This initializes memory to the program to be run.
# ifdef VLE
  extern_mem_write32(0x00000100,0x04897f5b);
  extern_mem_write32(0x10000104,0xd051e2fd);

  root->setReg("NIA",0,0x100);
  root->setReg("GPR24",0,30);
  root->setReg("GPR25",0,100);
  root->setReg("GPR26",0,10);
  root->setReg("GPR27",0,1);  
# else

  switch (prog) {
  case 0:
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
    // Some register initializations.
    root->setReg("NIA",0,0x100);
    root->setReg("GPR1",0,0x1fff);
    root->setReg("GPR4",0,0x2000);
    root->setReg("GPR6",0,0xfffffffe);
    // If there's a cache, enable it.
    root->setReg("CCR",0,0xc0000000);
    break;
  case 1:
    extern_mem_write32(0x00000100,0x80201000);  // lwz r1,0x1000(r0)
    extern_mem_write32(0x00000104,0x80403000);  // lwz r2,0x3000(r0)
    extern_mem_write32(0x00000108,0x80601004);  // lwz r3,0x1004(r0)
    extern_mem_write32(0x0000010c,0x00000000);  // .long 0
    extern_mem_write32(0x00000110,0x00000000);  // .long 0
    extern_mem_write32(0x00000114,0x7D5A02A6);  // mfspr r10,SRR0
    extern_mem_write32(0x00000118,0x394A0004);  // addi r10,r10,4
    extern_mem_write32(0x0000011c,0x3A801234);  // li r20,0x1234
    extern_mem_write32(0x00000120,0x7D5A03A6);  // mtspr SRR0,r10
    extern_mem_write32(0x00000124,0x4C000064);  // rfi
    // Some register initializations.
    root->setReg("NIA",0,0x100);
    root->setReg("IVOR2",0,0x114);
    break;
  case 2:
    extern_mem_write32(0x00000010,0x8040100C);	//	lwz r2,0x100c(r0)
    extern_mem_write32(0x00000014,0x8040200C);  //  lwz r2,0x200c(r0)
    extern_mem_write32(0x00000018,0x90403008);	//	stw r2,0x3008(r0)
    root->setReg("NIA",0,0x10);
    root->setReg("CCR",0,0xc0000000);
    break;
  default:
    RError("No program " << prog << " defined.");
  }

# endif

  // Data memory initializations.
  extern_mem_write32(0x1000,0xdeadbeef);
  extern_mem_write32(0x1004,0x12345678);
  extern_mem_write32(0x2000,0x00000000);
  extern_mem_write32(0x3000,0x87654321);
  extern_mem_write32(0xfffffffc,0);
  extern_mem_write32(0x00000000,0);

  cout << "TRACE\n";

# ifdef USE_LOGGING_MGR
  // If using the logging manager, then install our logger.
  mgr->registerLogger(&mylogger);
# endif

  // This runs the simulation until it halts.  For this model, the core will
  // halt when it encounters an opcode of 0x00000000.
# ifdef UADL
  while (model->isActive()) {
    model->proceed();
  }
# else
  root->run();
# endif

  cout << "\nRESULTS\n";

# ifdef UADL
  cout << "\n# time " << model->currTime() << ": simulation terminated.  "
       << model->issueCount() << " instructions issued.\n\n";
# endif

  // Final results display.
  Reporter reporter;

  display_memory(6,0x1000,0x2000,0x2004,0x2008,0xfffffffc,0x00000000);
  root->showRegs(reporter);
  root->showCaches(reporter);
  }
  catch (exception &err) {
    cerr << "Error:  " << err.what() << "\n\n";
    return 1;
  }
}


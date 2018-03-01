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

//#define VLE

#ifdef USE_LOGGING_MGR
# include "rnumber/RNumber.h"
#else
# define __NO_RNUMBER__
#endif

#include "helpers/Macros.h"

#ifndef UadlNamespace
#  define UadlNamespace DEFAULT_UADL_MODEL_NAMESPACE
#endif

#ifndef IssNamespace
# define IssNamespace DEFAULT_ISS_NAMESPACE
#endif

#define UADL_MODEL_NAMESPACE UadlNamespace
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

#ifdef USE_LOGGING_MGR

#include "iss/LoggingMgr.h"

struct MyLogger : public LoggingIface {

  addr_t _instr_ea;

  void logInstrPrefetch(const IssCore *c,addr_t ea)
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

MyLogger mylogger;

#else

//
// These are the logging functions.  For this example, they simply display their
// results in a format similar to the DAT trace format used by the standalone ADL
// ISS executables.
//

static addr_t instr_ea, instr_ra;

struct MyLogger : public adl::LogBase {

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
    case CacheLogRead:
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

  void log_exception(unsigned handle,const char *name)
  {
  }

  void log_instr_issue_time(ttime_t)
  {
  }

  void log_instr_completion_time(ttime_t)
  {
  }

  InstrPacketBase *log_current_instr(InstrPacketBase *x)
  {
    return 0;
  }

  void log_flush_pending()
  {
  }

  void log_commit(InstrPacketBase *x)
  {
  }

};

MyLogger mylogger;

#endif


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
         << mem_read32(addr) << "\n";
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

struct MyCallback : public RegCallback {
  MyCallback(const string &n) : _name(n) {};

  virtual void operator()(unsigned index,REGTYPE value) {
    cout << "# Callback:  " << _name << ":  New value is 0x" << value << "\n";
  };

  string _name;
};

void install_callback(IssNode *root,MyCallback *b)
{
  if (!root->setRegCallback(b->_name,b)) {
    cout << "Unable to install callback on register " << b->_name << " (ignored).\n";
  }
}

int main(int argc,const char *argv[])
{
  try {
    bool trace = true;
    bool trace_all = false;
    bool pctest = false;
    bool reset_test = false;
    int prog = 0;
    unsigned id = 0;

    for (int i = 1; i != argc; ++i) {
      if (!strcmp(argv[i],"-trace")) {
        trace = true;
      }
      else if (!strcmp(argv[i],"-trace-all")) {
        trace_all = true;
      }
      else if (!strcmp(argv[i],"-no-trace")) {
        trace = false;
      }
      else if (!strcmp(argv[i],"-pctest")) {
        pctest = true;
      }
      else if (!strcmp(argv[i],"-reset-test")) {
        reset_test = true;
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
    uadl::UadlArch *model = UadlNamespace::createArch("P",id);
    IssNode *root = &model->iss();
    if (trace_all) {
      model->set_tracing(-1);
    }
# else
    IssNode *root = IssNamespace::createTopLevelNode(id);
# endif


# ifdef TrapAddr
    MyMem mem(root,get_exception_id(root,"DataStorage"),TrapAddr);
# else
    MyMem mem(root,0,0);
# endif  

    // root->setMemHandler(&mem);

    if (trace) {
      root->set_tracing();
    }

    cout << "CORE n=:P\n";

    // This initializes memory to the program to be run.
# ifdef VLE
    mem_write32(0x00000100,0x04897f5b);
    mem_write32(0x10000104,0xd051e2fd);

    root->setReg("NIA",0,0x100);
    root->setReg("GPR24",0,30);
    root->setReg("GPR25",0,100);
    root->setReg("GPR26",0,10);
    root->setReg("GPR27",0,1);  
# else

    switch (prog) {
    case 0:
      mem_write32(0x00000100,0x80401000);  // lwz r2,0x1000(0)
      mem_write32(0x00000104,0x394A0100);  // addi r10,r10,0x100
      mem_write32(0x00000108,0x7C620A14);  // add r3,r2,r1
      mem_write32(0x0000010c,0x34A5FFFE);  // subic. r5,r5,2
      mem_write32(0x00000110,0x90640000);  // stw r3,0(r4)
      mem_write32(0x00000114,0x90660000);  // stw r3,0(r6)
      mem_write32(0x00000118,0xA0E01004);  // lhz  r7,0x1004(r0)
      mem_write32(0x0000011c,0x89001005);  // lbz  r8,0x1005(r0)
      mem_write32(0x00000120,0x99002004);  // stb  r8,0x2004(r0)
      mem_write32(0x00000124,0xB0E02008);  // sth  r7,0x2008(r0)
      // Some register initializations.
      root->setReg("NIA",0,0x100);
      root->setReg("GPR1",0,0x1fff);
      root->setReg("GPR4",0,0x2000);
      root->setReg("GPR6",0,0xfffffffe);
      // If there's a cache, enable it.
      root->setReg("CCR",0,0xc0000000);
      break;
    case 1:
      mem_write32(0x00000100,0x80201000);  // lwz r1,0x1000(r0)
      mem_write32(0x00000104,0x80403000);  // lwz r2,0x3000(r0)
      mem_write32(0x00000108,0x80601004);  // lwz r3,0x1004(r0)
      mem_write32(0x0000010c,0x00000000);  // .long 0
      mem_write32(0x00000110,0x00000000);  // .long 0
      mem_write32(0x00000114,0x7D5A02A6);  // mfspr r10,SRR0
      mem_write32(0x00000118,0x394A0004);  // addi r10,r10,4
      mem_write32(0x0000011c,0x3A801234);  // li r20,0x1234
      mem_write32(0x00000120,0x7D5A03A6);  // mtspr SRR0,r10
      mem_write32(0x00000124,0x4C000064);  // rfi
      // Some register initializations.
      root->setReg("NIA",0,0x100);
      root->setReg("IVOR2",0,0x114);
      break;
    case 2:
      mem_write32(0x00000100,0x38210001);  // addi r1
      mem_write32(0x00000104,0x38210001);  // addi r1    
      mem_write32(0x00000108,0x38210001);  // addi r1
      mem_write32(0x0000010c,0x7C320BA6);  // mtspr 5
      mem_write32(0x00000110,0x38210001);  // addi r1
      mem_write32(0x00000114,0x7C330BA6);  // mtspr 5
      mem_write32(0x00000118,0x00000000);  // .long 0
      root->setReg("NIA",0,0x100);
      break;
    case 3:
      mem_write32(0x00000100,0x38210001);  // addi r1,r1,1
      mem_write32(0x00000104,0x38210001);  // addi r1,r1,1
      mem_write32(0x00000108,0x80401000);  // lwz r2,0x1000(0)
      mem_write32(0x0000010c,0x38210001);  // addi r1,r1,1
      mem_write32(0x00000110,0x00000000);  // .long 0x0
      mem_write32(0x00000114,0x00000000);  // .long 0x0
      mem_write32(0x00000118,0x00000000);  // .long 0x0
      mem_write32(0x0000011c,0x80601003);  // lwz r3,0x1003(0)
      mem_write32(0x00000120,0x38210001);  // addi r1,r1,1
      mem_write32(0x00000124,0x38210001);  // addi r1,r1,1
      mem_write32(0x00000128,0x00000000);  // .long 0x0
      root->setProgramCounter(0x100);
      break;
    default:
      RError("No program " << prog << " defined.");
    }

# endif

    // Data memory initializations.
    mem_write32(0x1000,0xdeadbeef);
    mem_write32(0x1004,0x12345678);
    mem_write32(0x2000,0x00000000);
    mem_write32(0x3000,0x87654321);
    mem_write32(0xfffffffc,0);
    mem_write32(0x00000000,0);

    cout << "TRACE\n";

# ifdef USE_LOGGING_MGR
    // If using the logging manager, then install our logger.
    registerLogger(&mylogger);
# else
    root->setLogger(&mylogger);
# endif

# ifdef USE_CALLBACKS
    // We're creating some callbacks here to test this feature.
    MyCallback hid0("HID0"), hid1("HID1");
    install_callback(root,&hid0);
    install_callback(root,&hid1);
# endif

    // This runs the simulation until it halts.  For this model, the core will
    // halt when it encounters an opcode of 0x00000000.
# ifdef UADL
    model->preRun();
    if (pctest) {
      // Special operation: Cycle for a while, then change the program counter
      // and then cycle until done.
      for (int i = 0; i != 8; ++i) {
        model->proceed();
      }
      model->setProgramCounter(0x11c);
      while (model->is_active()) {
        model->proceed();
      }
    } else if (reset_test) {
      // Special operation:  Cycle for a while, reset, then continue.  This makes
      // sure that reset will work.
      for (int i = 0; i != 10; ++i) {
        model->proceed();
      }
      model->reset();
      model->setProgramCounter(0x100);
      root->setReg("GPR1",0,0x2fff);
      root->setReg("GPR4",0,0x2000);
      root->setReg("GPR6",0,0xfffffffe);
      while (model->is_active()) {
        model->proceed();
      }
    } else {
      // Normal operation:  Cycle until done.
      while (model->is_active()) {
        model->proceed();
      }
    }
# else
    root->run();
# endif

    cout << "\nRESULTS\n";

# ifdef UADL
    cout << "\n# time " << std::dec << model->currTime()
         << ": simulation terminated.  " << model->issueCount()
         << " instructions issued.\n\n";
# endif

    // Final results display.
    Reporter reporter;

    display_memory(6,0x1000,0x2000,0x2004,0x2008,0xfffffffc,0x00000000);
    root->showRegs(reporter);
    root->showCaches(reporter);
  }
  catch (exception &e) {
    cout << "Error:  " << e.what() << endl;
    return 1;
  }
}


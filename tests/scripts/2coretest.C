//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Tests that we can instantiate two different cores.
//

#include <assert.h>
#include <iostream>
#include <iomanip>
#include <stdarg.h>

#define __NO_RNUMBER__

#include "helpers/Macros.h"

#define ISS_NAMESPACE mod1
#include "iss/ModelInterface.h"

#undef ISS_NAMESPACE
#define ISS_NAMESPACE mod18
#include "iss/ModelInterface.h"

using namespace std;
using namespace adl;

int main()
{
  // Instantiate two models and print their cache data.

  unsigned id = 0;
  IssNode *m1 = mod1::createTopLevelNode(id);
  IssNode *m2 = mod18::createTopLevelNode(id);

  cout << "Model 1:  " << m1->name() << "\n"
       << "Cache Data:\n";

  setAttrData(mod1::getAttrData());
  cout << m1->getCacheInfo();

  cout << "\nModel 2:  " << m2->name() << "\n"
       << "Cache Data:\n";

  setAttrData(mod18::getAttrData());
  cout << m2->getCacheInfo();
}

namespace adl {

  void log_instr_prefetch(const IssCore *c,addr_t ea)
  {
  }

  void log_instr_read(unsigned id,const char *name,addr_t ea,
                      addr_t ra,uint32_t data)
  {
  }

  void log_instr(const uint32_t* opc,int num_bytes,const char *name,Disassembler dis)
  {
  }

  void log_instr_args(const uint32_t* opc,int num_bytes,const char *name,Disassembler dis)
  {
  }

  void log_reg_write(unsigned id,const char *name,uint64_t value)
  {
  }

  void log_regfile_write(unsigned id,const char *name,uint32_t index,
                         uint64_t value)
  {
  }

  void log_core_mem_write(unsigned id,const char *name,
                          addr_t ea,int nb)
  {
  }

  void log_core_mem_read(unsigned id,const char *name,
                         addr_t ea,int nb)
  {
  }

  void log_mem_write(unsigned id,const char *name,bool pre,int seq,addr_t ea,
                     addr_t ra,uint32_t value)
  {
  }

  void log_mem_read(unsigned id,const char *name,bool pre,int seq,addr_t ea,
                    addr_t ra,uint32_t value)
  {
  }

  void log_annotation(const IssCore *core,MsgType type,unsigned level,const std::string &msg)
  {
  }

  void log_cache_action(const char *name,CacheAction action,CacheAccess access,unsigned level,
                        int set,int way,unsigned linemask,addr_t ra)
  {
  }

  void log_exception(unsigned handle,const char *name)
  {
  }

}

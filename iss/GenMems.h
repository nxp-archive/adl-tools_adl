//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Interface to memory generation code.
//

#ifndef _GEN_MEMS_H_
#define _GEN_MEMS_H_

#include "MiscTypes.h"

namespace adl {

  class opfxstream;
  class InstrWalker;
  class ClassData;
  class ConfigDB;
  class Memory;
  class StrList;
  class SysItem;

  struct MemoryEl {
    const Memory *_memory;
    bool		_shared;

    MemoryEl(const Memory *memory, bool shared) : _memory(memory), _shared(shared){}
    MemoryEl() : _memory(0), _shared(false) {}
  };

  typedef gc_list<MemoryEl> MemoryList;

  gc_string getMemoryName(const gc_string &memory);
  gc_string getMemoryName(const Memory &memory);
  gc_string getExternMemoryType(const gc_string &name);
  gc_string getExtDmiType();
  gc_string getExtDmiName();

  gc_string getGlobalMemName(bool is_extern);
  gc_string getGlobalMemRef();
  gc_string getMemAccessor(bool is_data,bool is_extern,bool extern_dmi,const gc_string &name);

  // Just generate memory data structures.
  void generateMemDS(opfxstream &o,const gc_string &name,const MemoryList &mems,
                     const StrSet &externs,ClassData &cd,const ConfigDB &config,
                     bool is_system);

  // Generate external memory handler.
  void generateMemHandler(opfxstream &o,const SysItem &item,const System::Items *children,
                          const MemoryList &mems,const StrSet &externs,ClassData &cd,
                          const ConfigDB &config,bool is_system);

  // Entry point for generating the memory data structures.
  void generateMemories(opfxstream &o,const Core &core,InstrWalker &walker,
                        const gc_string &filename,const StrSet &externs,
                        StrList &end_code,ClassData &cd,const ConfigDB &config,
                        const CoreConfig &coreconfig);

}

#endif

//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Interface to MMU generation code.
//

#ifndef _GENMMU_H_
#define _GENMMU_H_

#include "MiscTypes.h"

namespace adl {

  class opfxstream;
  class InstrWalker;
  class ClassData;
  class ConfigDB;
  class CoreConfig;
  class SlotAlloc;
  class Context;

  // A list of MMU lookups.  The second element specifies the context, If a lookup is part of a context.
  struct LookupEl {
    const MmuLookup *_lookup;
    const Context   *_ctx;

    LookupEl(const MmuLookup *l,const Context *c) : _lookup(l), _ctx(c) {};
    LookupEl(const MmuLookup *l) : _lookup(l), _ctx(0) {};
  };
  typedef gc_list<LookupEl> LookupList;

  gc_string getWayTypeName(const MmuLookup &l);
  gc_string getSetTypeName(const MmuLookup &l);
  gc_string getArrayTypeName(const MmuLookup &l);
  gc_string getArrayName(const MmuLookup &l);
  gc_string getArrayParmName(const MmuLookup &l);

  // Returns true if any lookups have any real pages, i.e. do actual translation.
  bool hasRealPages(const Core &core);

  // Just generate MMU data structures.
  void generateLookupDS(opfxstream &o,const LookupList &lookups,
                        ClassData &cd,const Resources &shared,const ConfigDB &config);

  void generateLookupDS(opfxstream &o,const Core &core,ClassData &cd,
                        const ConfigDB &config,const CoreConfig &coreconfig);

  // Generate helper functions only.
  void generateMmuHelpers(opfxstream &o,const LookupList &lookups,
                          ClassData &cd,const Resources &shared,
                          const ConfigDB &config,bool is_core);

  // Entry point for generating the MMU.
  void generateMMU(opfxstream &,const MMU *mmu,const Core &,InstrWalker &walker,const SlotAlloc &mem_comm_slots,
                   const gc_string &filename,ClassData &,RegBaseSet &,const ConfigDB &config,const CoreConfig &coreconfig);

}

#endif

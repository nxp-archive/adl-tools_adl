//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Interface to cache generation code.
//

#ifndef _GENCACHE_H_
#define _GENCACHE_H_

namespace adl {

  class opfxstream;
  class InstrWalker;
  class ClassData;
  class ConfigDB;
  class Cache;
  class Watches;

  struct CacheGen {
    const Cache *_cache;
    bool         _shared;
    bool         _perf;
    bool         _dyn;
    bool         _data;

    CacheGen(const Cache *c,bool s,bool p,bool d,bool da) : 
      _cache(c), 
      _shared(s), 
      _perf(p), 
      _dyn(d),
      _data(da)
    {};
  };
  typedef gc_list<CacheGen> CacheList;

  gc_string getCacheType(const Cache &cache);
  gc_string getCacheName(const Cache &cache);

  // Just generate cache data structures for a system.
  void generateCacheDS(opfxstream &o,const System &system,const Resources &shared,
                       ClassData &cd,const ConfigDB &config,StrList *child_args);

  // Entry point for generating the caches.
  void generateCaches(opfxstream &o,const Core &core,InstrWalker &walker,Watches &watches,
                      const gc_string &filename,ClassData &cd,const ConfigDB &config,const CoreConfig &coreconfig);

}

#endif

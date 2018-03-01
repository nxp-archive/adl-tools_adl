//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// An example of a custom cache.  All this does is hardcode the
// block-on-critical-word option to "serialized".
//

#ifndef _MY_CACHE_H_
#define _MY_CACHE_H_

#include "uadl/Cache.h"

class MyCache : public uadl::Cache {
public:
  MyCache(uadl::Timer &timer, const std::string &name, uadl::UadlMemoryIf &memory,
          unsigned memoryPort, unsigned lineWidth, unsigned addrBandwidth,
          unsigned dataBandwidth, unsigned queueSize, bool allowBusWait, 
          bool preemptive, const uadl::CacheConfigMap &configmap,
          bool withData = true) :
    uadl::Cache(timer,name,memory,memoryPort,lineWidth,addrBandwidth,
                dataBandwidth,queueSize,allowBusWait,preemptive,configmap,withData)
  {
    _blockOnCriticalWord = uadl::bcwSerialized;
  }

};

template <class CacheType,class ModelType,class InstrType,bool safeMode>
struct MyCacheT : public uadl::CacheT<CacheType,ModelType,InstrType,safeMode> {
  typedef uadl::CacheT<CacheType,ModelType,InstrType,safeMode> Base;

  MyCacheT (CacheType &cache) : Base(cache), _cache(cache) {};

  bool example_new_transaction(ModelType &uarch,InstrType &instr,uadl::Logger *logger,unsigned foo) {   
    return true;
  }

  CacheType &_cache;
};

#endif

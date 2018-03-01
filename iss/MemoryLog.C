//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#include "helpers/Macros.h"

#include "MemoryLog.h"
#include "ModelInterface.h"

using namespace std;

namespace adl {

  // This initializes the main hash with IssNode* -> MemData objects.  Each
  // MemData object has a hash that maps local memory names to integer
  // identifiers.  These int IDs are the indices into the address array.
  void MemoryLog::add_node(const IssNode &n)
  {
	  pair<NodeMap::iterator,bool> ip = _local_inits.insert(make_pair(&n,MemData()));
	  MemData &md = ip.first->second;
  
    const MemInfos &minfo = n.getMemInfo();
  
    md._addrs.resize(minfo.size());
    ForEach(minfo,i) {
      md._mem_ids.insert(make_pair(i->_name,i->_id));
    }
  }

  void MemoryLog::add_nodes(const IssNode &node)
  {
    add_node(node);
    ForEach(node.children(),i) {
      add_nodes(**i);
    }
  }

  unsigned MemoryLog::get_mem_id(const IssNode &n,const string &name)
  {
    return _local_inits[&n]._mem_ids[name];
  }

  bool MemoryLog::set(const IssNode &n,const std::string mname,addr_t addr)
  {
    return set(n,get_mem_id(n,mname),addr);
  }

  void MemoryLog::clear()
  {
    _global_inits.clear();
    ForEach(_local_inits,i) {
      i->second.clear();
    }
  }

  // Gain access to the global memory log.
  MemoryLog &memoryLog()
  {
    static MemoryLog log;
    return log;
  }

}

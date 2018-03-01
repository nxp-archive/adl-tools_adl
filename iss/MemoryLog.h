//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Memory logging support for standalone ISSs.  This is used to record memory
// activity- generally just activity from the readers.  A writer should add its
// own tracking if addresses need to be logged during a simulation.
//

#ifndef _MEMORYLOG_H_
#define _MEMORYLOG_H_

#include <iostream>
#include <map>
#include <vector>
#include <stdexcept>

#include "helpers/BasicTypes.h"

namespace adl {

  struct IssNode;

  // This class is used to track what addresses are used by a memory.  Currently, it's just
  // a simple STL map of 4k blocks, which are each represented by a vector of bools.  If an
  // address is touched, we set to true that element.  We may need to tune this structure over time
  // after we profile it a bit.
  typedef std::vector<bool> MemInitArray;
  typedef std::map<addr_t,MemInitArray> MemInitsBase;

  struct MemInits : private MemInitsBase 
  {

    enum { WordShift = 2, WordMask = ~(-1 << WordShift), WordMax = (WordMask+1),
           AddrShift = 12, AddrMask = ~(-1 << AddrShift), AddrMax = (AddrMask+1),
           BlockSize = (AddrMax >> WordShift)
    };

    MemInits() : _last_addr(0), _last(0)
    {
    }

    // Important point: The copy constructor must blank out the cached block
    // pointer, or else we'll actually be updating the map that was the source for
    // this copy, and not our own map.
    MemInits(const MemInits &x) : MemInitsBase(x), _last_addr(0), _last(0)
    {
    }

    MemInits &operator=(const MemInits &x)
    {
      _last = 0;
      _last_addr = 0;
      MemInitsBase::operator=(x);
      return *this;
    }

    void clear() 
    {
      MemInitsBase::clear();
      _last = 0;
      _last_addr = 0;
    }

    void add(addr_t blk_addr)
    {
      MemInitsBase::iterator iter = find(blk_addr);
      if (iter == MemInitsBase::end()) {
        iter = insert(make_pair(blk_addr,std::vector<bool>())).first;
        iter->second.resize(BlockSize,false);
        //std::cerr << "Adding block for 0x" << hex << blk_addr << dec << ":  Blocksize:  " << BlockSize << endl;
      }
      _last_addr = blk_addr;
      _last = &(iter->second);
      //std::cerr << "Adding:  Addr:  " << std::hex << iter->first << ":  page:  " << _last << "\n";
    }

    // Adds address 'a' to the container.  Returns true if the insertion was made,
    // false if the address was already set.
    bool set(addr_t a) 
    {
      addr_t blk_addr = a >> AddrShift;
      unsigned blk_offset = (a & AddrMask) >> WordShift;
      if (!_last || blk_addr != _last_addr) {
        add(blk_addr);
      }
      bool old = (*_last)[blk_offset];
      (*_last)[blk_offset] = true;
      //std::cerr << "Setting:  Addr:  " << std::hex << a << ":  page:  " << _last << "\n";
      return !old;
    }

    struct const_iterator {
      MemInitsBase::const_iterator _di;
      unsigned                     _index;

      const_iterator(const MemInitsBase::const_iterator &di,unsigned i) : _di(di), _index(i) {};

      const_iterator(const MemInitsBase::const_iterator &di) : _di(di), _index(0) {};

      const_iterator &operator++() { 
        _index += WordMax;
        if (_index >= AddrMax) { 
          ++_di; 
          _index = 0; 
        } 
        return *this; 
      };

      addr_t operator*() const { return (_di->first << AddrShift) | _index; };

      bool is_init() const { 
        const MemInitArray *x = &(_di->second);
        //std::cerr << "Iterator:  Addr:  " << std::hex << _di->first << ":  page:  " << x << "\n";
        return (*x)[_index >> WordShift]; 
      };

      bool operator==(const const_iterator &x) const { return (_di == x._di && _index == x._index); };

      bool operator!=(const const_iterator &x) const { return !operator==(x); };
    };

    const_iterator begin() const 
    { 
      return const_iterator(MemInitsBase::begin(),0); 
    }

    const_iterator end() const 
    { 
      return const_iterator(MemInitsBase::end());
    }

  private:

    addr_t        _last_addr;
    MemInitArray *_last;

  };

  // Stores memory initialization for all memories in all cores.
  struct MemoryLog 
  {
    typedef std::vector<MemInits> MemInitsVect;
    typedef std::map<std::string,unsigned> MemIds;

    class MemData {
      MemInitsVect _addrs;
      MemIds       _mem_ids;

      friend struct MemoryLog;

    public:
      // Return a specified inits array.  We adjust the ID to take into account that
      // there is never a local memory with an ID of 0.
      const MemInits &get_addrs(int id) const { return _addrs[id-1]; };
      MemInits &get_addrs(int id) { return _addrs[id-1]; };

      void clear() {
        _addrs.clear();
      }
    };

    typedef std::map<const IssNode *,MemData> NodeMap;


    const MemInits &global_inits() const { return _global_inits; }
    const NodeMap  &local_inits() const { return _local_inits; }

    const MemData &local_inits(const IssNode &n) const 
    {
      NodeMap::const_iterator i = _local_inits.find(&n);
      if (i == _local_inits.end()) {
        throw std::runtime_error("MemoryLog::_local_inits:  unknown node.");
      } else {
        return i->second;
      }
    }


    void add_nodes(const IssNode &root);

    unsigned get_mem_id(const IssNode &n,const std::string &name);

    bool set(const IssNode &n,unsigned id,addr_t addr)
    {
      if (id == 0) {
        return _global_inits.set(addr);
      } else {
        return _local_inits[&n].get_addrs(id).set(addr);
      }
    }

    // Returns false if the specified address was already initialized.
    bool set(const IssNode &n,const std::string mname,addr_t addr);

    void clear();

  private:
    NodeMap  _local_inits;
    MemInits _global_inits;

    void add_node(const IssNode &root);
  };



  // Gain access to the global memory log.
  MemoryLog &memoryLog();

}

#endif

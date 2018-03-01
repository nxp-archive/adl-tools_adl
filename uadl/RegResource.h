//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef _REGISTERS_H_
#define _REGISTERS_H_

#include <iosfwd>

#include "iss/CircularQueue.h"

#include "UadlTypes.h"
#include "Logger.h"

namespace uadl {

  class RegResourceBase {
  public:
    RegResourceBase(const std::string &name) : _name(name) {};

    const std::string &name() const { return _name; };
    const std::string &name(UadlArch &,unsigned = 0) const { return _name; };

    void logStall(UadlArch &uarch,Logger *logger,int index,const char *msg,uint64_t mask) const;
    void logStall(UadlArch &uarch,Logger *logger,int index,const char *msg) const;

    // Used for allocations and deallocations.
    void logAlloc(UadlArch &uarch,Logger *logger,uint64_t id,int index,bool alloc) const;
    void logAlloc(UadlArch &uarch,Logger *logger,uint64_t id,int index,bool alloc,uint64_t mask) const;

  protected:
    void fullName(std::ostream &,int index) const;
    void findOwner(std::ostream &o,UadlArch &uarch,int index,uint64_t mask) const;

    const std::string _name;
  };

  // Registers are identified using a unique, opaque type (basically, a pointer).
  typedef const void * RegId;
# define RegInvalidId ((RegId)0)
  
  // We track register allocations using two methods: For full registers, we
  // employ a simple counter.  A non-zero value means that somebody is currently
  // targeting it.  For partial allocations, we have a mask which stores the
  // current portion being targeted, along with a deque storing
  // partial-allocation masks.  The idea is that we can quickly check against
  // this current mask and allocations are fairly cheap as well.  Deallocations
  // are a little more expensive, since we have to find a matching mask in the
  // deque, but the number of items in a deque should be relatively small.
  template <typename mask_t>
  struct RegAlloc {
    typedef adl::DSCircularQueue<std::pair<tag_t,mask_t> > PQueue;

    unsigned            _full_alloc_count;  // Tracks whole-register allocations.
    mask_t              _partial_mask;      // Current partially-allocated portion of the register.
    tag_t               _last_allocator;    // Tag for last instruction to allocate register.
    PQueue              _partial_allocs;    // Tracks partial-register allocations.

    RegAlloc() : _full_alloc_count(0) {};

    RegId id(UadlArch &,unsigned) const { return ((RegId)this); };

    void reset() {
      _full_alloc_count = 0;
      _partial_mask = 0;
      _partial_allocs.clear();
    }

    // Check if we can do a full-register allocation.
    bool can_alloc(tag_t id) const { return (_partial_mask == 0 && (_full_alloc_count == 0 || _last_allocator == id) ); };

    // Check if we can do a partial allocation.
    bool can_alloc_partial(mask_t mask) const { return (!mask || (_full_alloc_count == 0 && (mask & _partial_mask) == 0)); };

    // Allocate the entire register.
    void allocate(tag_t id) { ++_full_alloc_count; _last_allocator = id; };

    // Allocate part of the register.
    void allocate(tag_t id,mask_t mask) { _partial_allocs.push_back(std::make_pair(id,mask)); _partial_mask |= mask; _last_allocator = id; };

    // Get tag of last allocator.
    tag_t last_allocator(UadlArch &,unsigned) const { return _last_allocator; };

    // Given a tag, search later allocations, removing anything from the
    // mask,returning the new mask.  In other words, let's say a forwarding path
    // contains the first half-word of the register with a given tag, but then
    // subsequently, the first nibble was allocated to a different instruction.
    // This would take the mask (containing the first half-word set) and remove
    // the first nibble, since that was allocated by a different instruction.
    mask_t rm_later_partial_alloc(UadlArch &,unsigned,mask_t mask,tag_t tag) const {
      if (_full_alloc_count) {
        // Full allocation- either the tag matches (OK), or it doesn't (fail).
        return (tag == _last_allocator) ? mask : 0;
      } else {
        // Partial allocation- starting at the back, remove any allocations not
        // matching our tag.
        for (int i = _partial_allocs.size()-1; i >= 0; --i) {
          if (tag == _partial_allocs[i].first) {
            // Found our instruction, so we're done.
            break;
          } else {
            // A different instruction, so remove its mask from our mask.
            mask &= ~_partial_allocs[i].second;
          }
        }
        return mask;
      }
    }

    // Deallocate the entire register.
    void deallocate() { if (_full_alloc_count) --_full_alloc_count; };

    // Deallocate part of the register.
    void deallocate(mask_t mask) {
      _partial_mask = 0;
      // Find a matching mask and remove it from the list of masks.  As we do this, calculate
      // a new mask.
      int tmp = -1;
      int n = _partial_allocs.size();
      for (int i = 0; i != n; ++i) {
        if ( tmp == -1 &&  _partial_allocs[i].second == mask ) {
          tmp = i;
        } else {
          _partial_mask |= _partial_allocs[i].second;
        }
      }
      // Remove the mask if found (should always be found!).
      if (tmp != -1) {
        _partial_allocs.erase(tmp);
      }      
    }
  };

  // Right now, inquiring for read or write is basically the same operation: We
  // can do it if the alloc mask allows us to.
  template <typename mask_t>
  class RegResource : public RegResourceBase, private RegAlloc<mask_t> {
    typedef RegAlloc<mask_t> Base;

  public:
    RegResource(const char *name) :
      RegResourceBase(name)
    {
      reset();
    }

    using RegAlloc<mask_t>::id;
    using RegAlloc<mask_t>::reset;
    using RegAlloc<mask_t>::last_allocator;
    using RegAlloc<mask_t>::rm_later_partial_alloc;

    bool can_alloc(UadlArch &uarch,InstrBase &,Logger *logger,const char *msg,mask_t mask) const {
      if (!Base::can_alloc_partial(mask)) {
        uarch.incrRegStalls();
        if (logger) logStall(uarch,logger,-1,msg,mask);
        return false;
      } else {
        return true;
      }
    }

    bool can_alloc(UadlArch &uarch,InstrBase &ic,Logger *logger,const char *msg) const {
      if (!Base::can_alloc(ic.iid())) {
        uarch.incrRegStalls();
        if (logger) logStall(uarch,logger,-1,msg);
        return false;
      } else {
        return true;
      }
    }

    bool can_read(UadlArch &uarch,InstrBase &ic,Logger *logger) const {
      return can_alloc(uarch,ic,logger,"read");
    }

    bool can_read(UadlArch &uarch,InstrBase &ic,Logger *logger,mask_t mask) const {
      return can_alloc(uarch,ic,logger,"read",mask);
    }

    void allocate(UadlArch &uarch,InstrBase &ic,Logger *logger) { 
      Base::allocate(ic.iid());
      if (logger) logAlloc(uarch,logger,ic.iid(),-1,true); 
    }

    void allocate(UadlArch &uarch,InstrBase &ic,Logger *logger,mask_t mask) { 
      Base::allocate(ic.iid(),mask);
      if (logger) logAlloc(uarch,logger,ic.iid(),-1,true,mask); 
    }

    void deallocate(UadlArch &uarch,InstrBase &ic,Logger *logger) { 
      Base::deallocate();
      if (logger) logAlloc(uarch,logger,ic.iid(),-1,false);
    }

    void deallocate(UadlArch &uarch,InstrBase &ic,Logger *logger,mask_t mask) { 
      Base::deallocate(mask);
      if (logger) logAlloc(uarch,logger,ic.iid(),-1,false,mask);
    }
    
    static unsigned getIndex(unsigned index) { return index; };

  };

  std::string getRegName(const std::string &name,unsigned index);

  // A register-file behaves like a register, except that there is an array of
  // masks.
  template <typename mask_t,size_t N>
  class RegFileResource : public RegResourceBase {
  public:
    RegFileResource(const char *name) :
      RegResourceBase(name)
    {
      reset();
    }

    using RegResourceBase::name;
    std::string name(UadlArch &,unsigned index = 0) const { return getRegName(_name,index); };

    RegId id(UadlArch &uarch,unsigned index) const { return _allocs[index].id(uarch,index); };

    void reset()
    {
      for (int i = 0; i != N; ++i) {
        _allocs[i].reset();
      }
    }

    bool can_alloc(UadlArch &uarch,InstrBase &ic,Logger *logger,unsigned index,const char *msg,mask_t mask) const {
      if (!_allocs[index].can_alloc_partial(mask)) {
        uarch.incrRegStalls();
        if (logger) logStall(uarch,logger,index,msg,mask);
        return false;
      } else {
        return true;
      }
    }

    bool can_alloc(UadlArch &uarch,InstrBase &ic,Logger *logger,unsigned index,const char *msg) const {
      if (!_allocs[index].can_alloc(ic.iid())) {
        uarch.incrRegStalls();
        if (logger) logStall(uarch,logger,index,msg);
        return false;
      } else {
        return true;
      }
    }

    bool can_read(UadlArch &uarch,InstrBase &ic,Logger *logger,unsigned index) const {
      return can_alloc(uarch,ic,logger,index,"read");
    }

    bool can_read(UadlArch &uarch,InstrBase &ic,Logger *logger,unsigned index,mask_t mask) const {
      return can_alloc_partial(uarch,ic,logger,index,"read",mask);
    }

    void allocate(UadlArch &uarch,InstrBase &ic,Logger *logger,unsigned index) { 
      _allocs[index].allocate(ic.iid());
      if (logger) logAlloc(uarch,logger,ic.iid(),index,true); 
    }

    void allocate(UadlArch &uarch,InstrBase &ic,Logger *logger,unsigned index,mask_t mask) { 
      _allocs[index].allocate(ic.iid(),mask);
      if (logger) logAlloc(uarch,logger,ic.iid(),index,true,mask); 
    }

    tag_t last_allocator(UadlArch &uarch,unsigned index) const { return _allocs[index].last_allocator(uarch,index); };

    mask_t rm_later_partial_alloc(UadlArch &uarch,unsigned index,mask_t mask,tag_t tag) const { return _allocs[index].rm_later_partial_alloc(uarch,index,mask,tag); };

    void deallocate(UadlArch &uarch,InstrBase &ic,Logger *logger,unsigned index) { 
      _allocs[index].deallocate();
      if (logger) logAlloc(uarch,logger,ic.iid(),index,false);
    }

    void deallocate(UadlArch &uarch,InstrBase &ic,Logger *logger,unsigned index,mask_t mask) { 
      _allocs[index].deallocate(mask);
      if (logger) logAlloc(uarch,logger,ic.iid(),index,false,mask);
    }
    
    static unsigned getIndex (unsigned index) { return index; };
    
  protected:
    RegAlloc<mask_t> _allocs[N];
  };

  // An alias to another register.
  template <typename mask_t,typename trg_mask_t>
  class RegAlias {
    typedef RegResource<trg_mask_t> Target;
  public:
    RegAlias(Target &trg) :
      _trg(trg)
    {
    }

    const std::string &name(UadlArch &uarch,unsigned index = 0) const { return _trg.name(uarch,index); };

    RegId id() const { return _trg.id(); };

    bool can_alloc(UadlArch &uarch,Logger *logger,const char *msg,mask_t mask) const {
      return _trg.can_alloc(uarch,logger,msg,mask);
    }

    bool can_alloc(UadlArch &uarch,Logger *logger,const char *msg) const {
      return _trg.can_alloc(uarch,logger,msg);
    }

    void allocate(UadlArch &uarch,InstrBase &ic,Logger *logger) { 
      _trg.allocate(uarch,ic,logger);
    }

    void allocate(UadlArch &uarch,InstrBase &ic,Logger *logger,mask_t mask) { 
      _trg.allocate(uarch,ic,logger,mask);
    }

    tag_t last_allocator(UadlArch &uarch,unsigned index) const { return _trg.last_allocator(uarch,index); };

    mask_t rm_later_partial_alloc(UadlArch &uarch,unsigned index,mask_t mask,tag_t tag) const { return _trg.rm_later_partial_alloc(uarch,index,mask,tag); };

    void deallocate(UadlArch &uarch,InstrBase &ic,Logger *logger) { 
      _trg.deallocate(uarch,ic,logger);
    }

    void deallocate(UadlArch &uarch,InstrBase &ic,Logger *logger,mask_t mask) { 
      _trg.deallocate(uarch,ic,logger,mask);
    }
    
  protected:
    Target &_trg;
  };

  // An alias to an element of a register-file.
  template <typename mask_t,typename trg_mask_t,size_t N>
  class RegFileElmAlias {
    typedef RegFileResource<trg_mask_t,N> Target;
  public:
    RegFileElmAlias(Target &trg,unsigned index) :
      _trg(trg),
      _index(index)
    {
    }

    const std::string &name(UadlArch &uarch,unsigned index = 0) const { return _trg.name(uarch,_index); };

    RegId id() const { return _trg.id(_index); };

    bool can_alloc(UadlArch &uarch,Logger *logger,const char *msg,mask_t mask) const {
      return _trg.can_alloc(uarch,logger,_index,msg,mask);
    }

    bool can_alloc(UadlArch &uarch,Logger *logger,const char *msg) const {
      return _trg.can_alloc(uarch,logger,_index,msg);
    }

    void allocate(UadlArch &uarch,InstrBase &ic,Logger *logger) { 
      _trg.allocate(uarch,ic,logger,_index);
    }

    void allocate(UadlArch &uarch,InstrBase &ic,Logger *logger,mask_t mask) { 
      _trg.allocate(uarch,ic,logger,_index,mask);
    }

    tag_t last_allocator(UadlArch &uarch,unsigned) const { return _trg.last_allocator(uarch,_index); };

    mask_t rm_later_partial_alloc(UadlArch &uarch,unsigned index,mask_t mask,tag_t tag) const { return _trg.rm_later_partial_alloc(uarch,_index,mask,tag); };

    void deallocate(UadlArch &uarch,InstrBase &ic,Logger *logger) { 
      _trg.deallocate(uarch,ic,logger,_index);
    }

    void deallocate(UadlArch &uarch,InstrBase &ic,Logger *logger,mask_t mask) { 
      _trg.deallocate(uarch,ic,logger,_index,mask);
    }
    
  protected:
    Target   &_trg;
    unsigned  _index;
  };

}

#endif

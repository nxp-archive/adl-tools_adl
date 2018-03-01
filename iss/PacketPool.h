//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
//
// This implements a simple fixed-size allocation pool.  The template type must
// have a member named _next to store the next available element.  Elements are
// allocated off o the heap at startup; after that, no heap allocations are
// made.
//

#ifndef _PACKET_POOL_H_
#define _PACKET_POOL_H_

#include <cassert>

namespace adl {

  // The basic pool allocator.
  template <class T,size_t N>
  struct PacketPool {

    typedef T             value_type;
    typedef value_type ** iterator;

    PacketPool()
    {
      for (int i = 0; i != N; ++i) {
        _data[i] = new value_type;
      }
      reset();
    };

    ~PacketPool()
    {
      for (int i = 0; i != N; ++i) {
        delete _data[i];
      }
    }

    void reset()
    {
      for (int i = 0; i != N-1; ++i) {
        _data[i]->_next = _data[i+1];
      }
      _data[N-1]->_next = 0;
      _head = _data[0];
    }

    unsigned num_free() const
    {
      unsigned count = 0;
      value_type *h = _head;
      while (h) {
        h = h->_next;
        ++count;
      }      
      return count;
    }

    bool none_allocated() const
    {
      return (num_free() == N);
    }

    value_type *alloc() {
      assert(_head);
      value_type *x = _head;
      _head = _head->_next;
      return x;
    };

    void free(value_type *x) {
      assert(x != _head);
      x->_next = _head;
      _head = x;
    };

    void free_all(value_type *h,value_type *t) {
      t->_next = _head;
      _head = h;
    }

    void sanity_check(value_type *h) {
      while (h) {
        value_type *fh = _head;
        while (fh) {
          assert(fh != h);
          fh = fh->_next;
        }
        h = h->_next;
      }
    }

    // For iterating over all items in the pool, such as for doing initial
    // setup.
    iterator begin() { return &_data[0]; };
    iterator end()   { return &_data[N]; };

    value_type *_data[N];
    value_type *_head;

  };

  // This one keeps a count of outstanding objects.
  template <class T,size_t N>
  struct PacketPoolCounted : private PacketPool<T,N> {
    typedef PacketPool<T,N> Base;
    typedef typename Base::value_type value_type;

    PacketPoolCounted() : _count(0) {}

    void reset()
    {
      Base::reset();
      _count = 0;
    }

    value_type *alloc() {
      ++_count;
      return Base::alloc();
    }

    void free(value_type *x) {
      --_count;
      Base::free(x);
    }

    unsigned num_alloced() const { return _count; };
    
    unsigned _count;
  };

  // This is used when we *don't* want a pool-based allocator, but we want the
  // same interface.  It's for use when you have an external cache and thus
  // don't want to ever free an object- this just allocated using new and never
  // frees the item.
  template <class T>
  struct PacketAlloc {
    typedef T             value_type;

    value_type *alloc() {
      return new T;
    };

    void free(value_type *) {};

  };

}

#endif


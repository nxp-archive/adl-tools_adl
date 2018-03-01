//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
// This implements a simple circular queue of objects.  Memory allocation is
// only done at construction time, after which the queue is fixed in size.
//
// Note:  Default operation is on the dangerous side, as no size checks are performed.  
// These checks can be enabled via the template parm CheckSize.
//

#ifndef _CIRCULAR_QUEUE_H_
#define _CIRCULAR_QUEUE_H_

#include <cstddef>
#include <assert.h>

namespace adl {

  // The base class for circular queues.  Not used directly by end-user code.
  template <class T,class S,bool CheckSize>
  struct CircularQueueBase {
    S        _s;     // Support calculation class.
    T       *_buf;   // Buffer data
    unsigned _head;  // Index begin data
    unsigned _tail;  // Index stop data
    
    void push_back(const T &p)
    {
      if (CheckSize) {
        assert(!full());
      }

      _buf[_tail] = p;
      _tail = _s.next_tail(_tail);
    }

    // Similar to the other push_back, except that this just returns a pointer
    // to the element just added.  This allows the user to then do an in-place
    // new or call a reset routine to set data, reducing the overhead of an
    // extra copy.
    T *push_back()
    {
      if (CheckSize) {
        assert(!full());
      }
 
      T *t = &_buf[_tail];
      _tail = _s.next_tail(_tail);
      return t;
    }

    void pop_front(unsigned n)
    {
      if (CheckSize) {  
        assert(size() >= n);
      }

      _head = _s.next_head(_head,n);
    }

    void pop_front()
    {
      pop_front(1);
    }

    void pop_back(unsigned n)
    {
      if (CheckSize) {  
        assert(size() >= n);
      }

      _tail = _s.prev_tail(_tail,n);
    }

    void pop_back()
    {
      pop_back(1);
    }

    // Erases from start to (stop-1), moving elements down.
    void erase_middle(unsigned start,unsigned stop)
    {
      if (CheckSize) {
        assert( start < size() && stop <= size() );
      }

      for (unsigned i = start, t = stop, s = size(); t != s; ++i, ++t ) {
        (*this)[i] = (*this)[t];
      }

      pop_back(stop-start);
    }

    // Erases an arbitrary element specified by index.
    void erase(unsigned index)
    {
      erase_middle(index,index+1);
    }

    T &front() const
    {
      if (CheckSize) {
        assert (!empty());
      }
      return _buf[_head];
    }

    T &back() const
    {
      if (CheckSize) {
        assert (!empty());
      }
      return _buf[_s.prev_tail(_tail,1)];
    }

    T &operator[] (unsigned i) const
    {
      if (CheckSize) {
        assert (i < size());
      }
      i = _s.next_head(_head,i);
      return _buf[i];
    }

    // Returns -1 if not found, otherwise the index of the element where the
    // item was found.
    int find(const T &x) const
    {
      for (unsigned i = _head; i != _tail; i = _s.next_tail(i)) {
        if (_buf[i] == x) {
          return _s.conv_abs_index(_head,i);
        }
      }
      return -1;
    }

    void clear() 
    { 
      _head = _tail = 0; 
    }

    unsigned size() const
    { 
      return ((_head > _tail) ? _s.max() : 0) + _tail - _head; 
    }

    bool empty() const 
    { 
      return (_head == _tail); 
    }

    bool full() const  
    { 
      return ( _s.next_tail(_tail) == _head );
    }

    unsigned capacity() const
    {
      return _s.max() - 1;
    }

  };

  // Internal class used by a fixed-sized circular queue for calculating the next tail value.
  template <size_t N>
  struct CircularQueueSupport {
    unsigned max() const { return N; };

    unsigned next_tail(unsigned tail) const {
      return (tail + 1) & (N - 1);
    }

    unsigned next_head(unsigned head,unsigned n) const {
      return (head + n) & (N - 1);
    };

    unsigned prev_tail(unsigned tail,unsigned n) const {
      return (tail - n) & (N-1);
    }

    unsigned conv_abs_index(unsigned head,unsigned n) const {
      return (n - head + N) & (N-1);
    }

  };

  //
  // Fixed-size circular queue, where the size is a template parameter.  Note
  // that the size (N) must be a power of 2 in order to use fast modular
  // arithmetic.
  //
  template <class T,size_t N,bool CheckSize = false> 
  class CircularQueue : private CircularQueueBase<T,CircularQueueSupport<N>,CheckSize> {
    typedef CircularQueueBase<T,CircularQueueSupport<N>,CheckSize> Base;
  public:
    CircularQueue()
    {
      // The size must be a power of 2 in order to use some nice tricks for
      // modular operations.
      assert ( (N & (N-1)) == 0 );
      Base::_buf= new T[N];
      Base::_head = Base::_tail = 0;
    }
  
    ~CircularQueue()
    {
      delete [] Base::_buf;
    }

    unsigned capacity() const
    {
      return N-1;
    }
 
    using Base::push_back;

    using Base::pop_front;
    
    using Base::pop_back;

    using Base::erase_middle;

    using Base::erase;

    using Base::find;

    using Base::front;

    using Base::back;

    using Base::operator[];

    using Base::clear;
  
    using Base::size;

    using Base::empty;

    using Base::full;

    using Base::capacity;
  };

  // Internal class used by DCircularQueue (construction-time-sized queue) for
  // calculating the next tail value.
  struct DCircularQueueSupport {
    unsigned max() const { return _maxsize; };

    unsigned next_tail(unsigned tail) const {
      return (tail + 1) % (_maxsize);      
    }

    unsigned next_head(unsigned head,unsigned n) const {
      return (head + n) % (_maxsize);
    }

    unsigned prev_tail(unsigned tail,unsigned n) const {
      return (tail - n + _maxsize) % (_maxsize);
    }

    unsigned conv_abs_index(unsigned head,unsigned n) const {
      return (n - head + _maxsize) % (_maxsize);
    }

    unsigned _maxsize;
  };
  
  //
  // Fixed-size circular queue, where the size is a constructor parameter.  For
  // this version, the size need not be a power of 2.  We also allocate
  // maxsize+1 objects, since we always have one unused slot for the
  // full-condition.
  //
  template <class T,bool CheckSize = false> 
  class DCircularQueue : private CircularQueueBase<T,DCircularQueueSupport,CheckSize> {
    typedef CircularQueueBase<T,DCircularQueueSupport,CheckSize> Base;
  public:
    DCircularQueue(unsigned maxsize)
    {      
      Base::_buf = 0;
      resize(maxsize);
    }
  
    ~DCircularQueue()
    {
      delete [] Base::_buf;
    }

    void resize(unsigned maxsize)
    {
      assert(maxsize > 0);
      Base::_s._maxsize = maxsize+1;
      delete Base::_buf;
      Base::_buf= new T[Base::_s._maxsize];
      Base::_head = Base::_tail = 0;
    }

    using Base::push_back;

    using Base::pop_front;

    using Base::pop_back;
    
    using Base::erase_middle;

    using Base::erase;

    using Base::find;

    using Base::front;

    using Base::back;

    using Base::operator[];
  
    using Base::clear;
 
    using Base::size;

    using Base::empty;

    using Base::full;
 
    using Base::capacity;
  };

  // Internal class used by DSCircularQueue (dynamically-sized queue) for
  // calculating the next tail value.
  struct DSCircularQueueSupport {
    DSCircularQueueSupport() : _maxsize(0) {};

    unsigned max() const { return _maxsize; };

    unsigned next_tail(unsigned tail) const {
      return (tail + 1) & (_maxsize-1);      
    }

    unsigned next_head(unsigned head,unsigned n) const {
      return (head + n) & (_maxsize-1);
    }

    unsigned prev_tail(unsigned tail,unsigned n) const {
      return (tail - n) & (_maxsize-1);
    }

    unsigned conv_abs_index(unsigned head,unsigned n) const {
      return (n - head + _maxsize) & (_maxsize-1);
    }

    unsigned _maxsize;
  };

  //
  // This is a dynamically-sized circular queue, which grows as necessary.  Note
  // that growth is not very efficient, since we have to copy all items, but
  // growth is exponential in order to limit the number of resizes.  Plus, we
  // can then use the faster form of arithmetic (bitwise ops rather than
  // modulo), though that's not likely to be very significant.  When we do grow,
  // we copy all items from the current head through the tail to the new block,
  // where the head starts at position 0.
  //
  template <class T,bool CheckSize = false> 
  class DSCircularQueue : private CircularQueueBase<T,DSCircularQueueSupport,CheckSize> {
    typedef CircularQueueBase<T,DSCircularQueueSupport,CheckSize> Base;
  public:
    DSCircularQueue()
    {      
      Base::_buf = 0;
      Base::_head = Base::_tail = 0;
    }
  
    ~DSCircularQueue()
    {
      delete [] Base::_buf;
    }

    void push_back(const T &p)
    {
      if (needs_resize()) {
        resize();
      }

      Base::push_back(p);
    }

    // Similar to the other push_back, except that this just returns a pointer
    // to the element just added.  This allows the user to then do an in-place
    // new or call a reset routine to set data, reducing the overhead of an
    // extra copy.
    T *push_back()
    {
      if (needs_resize()) {
        resize();
      }

      return Base::push_back();      
    }

    using Base::pop_front;

    using Base::pop_back;
    
    using Base::erase_middle;

    using Base::erase;

    using Base::find;

    using Base::front;

    using Base::back;

    using Base::operator[];
  
    using Base::clear;
 
    using Base::size;

    using Base::empty;

    // Since it's dynamically sized, it's never "full".
    bool full() const  
    { 
      return false;
    }
 
    using Base::capacity;

  private:
    bool needs_resize() const
    {
      return (!Base::_buf || Base::full());
    }

    void resize()
    {
      // Increase the size exponentially, or if this is the first allocation,
      // set it to 4 to begin with, which means that 3 items can be added before
      // the next allocation (1 item of space is always wasted).
      if (Base::_s._maxsize == 0) {
        // Starting from scratch, so we just need to allocate some space.
        Base::_s._maxsize = 4;
        Base::_buf = new T[Base::_s._maxsize];
        Base::_head = Base::_tail = 0;
      } else {
        // Space has already been allocated, so we must allocate new space and
        // copy the data over.
        unsigned n  = size();
        unsigned nn = Base::_s._maxsize << 1;
        T *newmem = new T[nn];
        // Now copy everything over.
        for (unsigned i = 0; i != n; ++i) {
          newmem[i] = operator[](i);
        }
        // Have to update these after the copy, since they're used by operator[].
        Base::_s._maxsize = nn;
        Base::_head = 0;
        Base::_tail = n;
        delete [] Base::_buf;
        Base::_buf = newmem;
      }
    }
  };

}

#endif

//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// A garbage-collected STL vector.
//

#ifndef _GC_VECTOR_H_
#define _GC_VECTOR_H_

#include <vector>

#include "gc/gc_cpp.h"
#include "gc/gc_allocator.h"

template <typename _Tp>
class gc_vector : public gc, public std::vector<_Tp,gc_allocator<_Tp> >
{  
public:
  typedef typename std::vector<_Tp,gc_allocator<_Tp> > VBase;
  typedef typename VBase::allocator_type               allocator_type;
  typedef typename VBase::size_type                    size_type;
  typedef typename VBase::value_type                   value_type;

  // [23.2.2.1] construct/copy/destroy
  // (assign() and get_allocator() are also listed in this section)
  /**
   *  @brief  Default constructor creates no elements.
   */
  explicit
  gc_vector(const allocator_type& __a = allocator_type())
    : VBase(__a) 
  {}

  /**
   *  @brief  Create a %vector with copies of an exemplar element.
   *  @param  n  The number of elements to initially create.
   *  @param  value  An element to copy.
   *
   *  This constructor fills the %vector with @a n copies of @a value.
   */
  gc_vector(size_type _n, const value_type& _value,
          const allocator_type& _a = allocator_type())
    : VBase(_n,_value,_a)
  {}

  /**
   *  @brief  Create a %vector with default elements.
   *  @param  n  The number of elements to initially create.
   *
   *  This constructor fills the %vector with @a n copies of a
   *  default-constructed element.
   */
  explicit
  gc_vector(size_type __n)
    : VBase(__n)
  {}

  /**
   *  @brief  Builds a %vector from a range.
   *  @param  first  An input iterator.
   *  @param  last  An input iterator.
   *
   *  Create a %vector consisting of copies of the elements from
   *  [@a first,@a last).  This is linear in N (where N is
   *  distance(@a first,@a last)).
   *
   *  @if maint
   *  We don't need any dispatching tricks here, because insert does all of
   *  that anyway.
   *  @endif
   */
  template<typename _InputIterator>
  gc_vector(_InputIterator __first, _InputIterator __last,
       const allocator_type& __a = allocator_type())
    : VBase(__first,__last,__a)
  {};

};

// This one is explicitly managed, but uses the traceable allocator, so it's
// OK to put gc'd objects in it.
template <typename _Tp>
class tr_vector : public std::vector<_Tp,traceable_allocator<_Tp> >
{  
public:
  typedef typename std::vector<_Tp,traceable_allocator<_Tp> > VBase;
  typedef typename VBase::allocator_type               allocator_type;
  typedef typename VBase::size_type                    size_type;
  typedef typename VBase::value_type                   value_type;

  // [23.2.2.1] construct/copy/destroy
  // (assign() and get_allocator() are also listed in this section)
  /**
   *  @brief  Default constructor creates no elements.
   */
  explicit
  tr_vector(const allocator_type& __a = allocator_type())
    : VBase(__a) 
  {}

  /**
   *  @brief  Create a %vector with copies of an exemplar element.
   *  @param  n  The number of elements to initially create.
   *  @param  value  An element to copy.
   *
   *  This constructor fills the %vector with @a n copies of @a value.
   */
  tr_vector(size_type _n, const value_type& _value,
          const allocator_type& _a = allocator_type())
    : VBase(_n,_value,_a)
  {}

  /**
   *  @brief  Create a %vector with default elements.
   *  @param  n  The number of elements to initially create.
   *
   *  This constructor fills the %vector with @a n copies of a
   *  default-constructed element.
   */
  explicit
  tr_vector(size_type __n)
    : VBase(__n)
  {}

  /**
   *  @brief  Builds a %vector from a range.
   *  @param  first  An input iterator.
   *  @param  last  An input iterator.
   *
   *  Create a %vector consisting of copies of the elements from
   *  [@a first,@a last).  This is linear in N (where N is
   *  distance(@a first,@a last)).
   *
   *  @if maint
   *  We don't need any dispatching tricks here, because insert does all of
   *  that anyway.
   *  @endif
   */
  template<typename _InputIterator>
  tr_vector(_InputIterator __first, _InputIterator __last,
       const allocator_type& __a = allocator_type())
    : VBase(__first,__last,__a)
  {};

};

#endif

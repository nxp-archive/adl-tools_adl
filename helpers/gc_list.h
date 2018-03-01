//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// A garbage-collected STL list.
//

#ifndef _GC_LIST_H_
#define _GC_LIST_H_

#include <list>

#include "gc/gc_cpp.h"
#include "gc/gc_allocator.h"

template <typename _Tp>
class gc_list : public gc, public std::list<_Tp,gc_allocator<_Tp> >
{  
public:
  typedef typename std::list<_Tp,gc_allocator<_Tp> > LBase;
  typedef typename LBase::allocator_type             allocator_type;
  typedef typename LBase::size_type                  size_type;
  typedef typename LBase::value_type                 value_type;

  // [23.2.2.1] construct/copy/destroy
  // (assign() and get_allocator() are also listed in this section)
  /**
   *  @brief  Default constructor creates no elements.
   */
  explicit
  gc_list(const allocator_type& __a = allocator_type())
    : LBase(__a) 
  {}

  /**
   *  @brief  Create a %list with copies of an exemplar element.
   *  @param  n  The number of elements to initially create.
   *  @param  value  An element to copy.
   *
   *  This constructor fills the %list with @a n copies of @a value.
   */
  gc_list(size_type _n, const value_type& _value,
          const allocator_type& _a = allocator_type())
    : LBase(_n,_value,_a)
  {}

  /**
   *  @brief  Create a %list with default elements.
   *  @param  n  The number of elements to initially create.
   *
   *  This constructor fills the %list with @a n copies of a
   *  default-constructed element.
   */
  explicit
  gc_list(size_type __n)
    : LBase(__n)
  {}

  /**
   *  @brief  Builds a %list from a range.
   *  @param  first  An input iterator.
   *  @param  last  An input iterator.
   *
   *  Create a %list consisting of copies of the elements from
   *  [@a first,@a last).  This is linear in N (where N is
   *  distance(@a first,@a last)).
   *
   *  @if maint
   *  We don't need any dispatching tricks here, because insert does all of
   *  that anyway.
   *  @endif
   */
  template<typename _InputIterator>
  gc_list(_InputIterator __first, _InputIterator __last,
          const allocator_type& __a = allocator_type())
    : LBase(__first,__last,__a)
  {};

};

template <typename _Tp>
class tr_list : public std::list<_Tp,traceable_allocator<_Tp> >
{  
public:
  typedef typename std::list<_Tp,traceable_allocator<_Tp> > LBase;
  typedef typename LBase::allocator_type             allocator_type;
  typedef typename LBase::size_type                  size_type;
  typedef typename LBase::value_type                 value_type;

  // [23.2.2.1] construct/copy/destroy
  // (assign() and get_allocator() are also listed in this section)
  /**
   *  @brief  Default constructor creates no elements.
   */
  explicit
  tr_list(const allocator_type& __a = allocator_type())
    : LBase(__a) 
  {}

  /**
   *  @brief  Create a %list with copies of an exemplar element.
   *  @param  n  The number of elements to initially create.
   *  @param  value  An element to copy.
   *
   *  This constructor fills the %list with @a n copies of @a value.
   */
  tr_list(size_type _n, const value_type& _value,
          const allocator_type& _a = allocator_type())
    : LBase(_n,_value,_a)
  {}

  /**
   *  @brief  Create a %list with default elements.
   *  @param  n  The number of elements to initially create.
   *
   *  This constructor fills the %list with @a n copies of a
   *  default-constructed element.
   */
  explicit
  tr_list(size_type __n)
    : LBase(__n)
  {}

  /**
   *  @brief  Builds a %list from a range.
   *  @param  first  An input iterator.
   *  @param  last  An input iterator.
   *
   *  Create a %list consisting of copies of the elements from
   *  [@a first,@a last).  This is linear in N (where N is
   *  distance(@a first,@a last)).
   *
   *  @if maint
   *  We don't need any dispatching tricks here, because insert does all of
   *  that anyway.
   *  @endif
   */
  template<typename _InputIterator>
  tr_list(_InputIterator __first, _InputIterator __last,
          const allocator_type& __a = allocator_type())
    : LBase(__first,__last,__a)
  {};
};

#endif

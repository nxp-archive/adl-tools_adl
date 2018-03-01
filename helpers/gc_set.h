//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// A garbage-collected STL set.
//

#ifndef _GC_SET_H_
#define _GC_SET_H_

#include <set>

#include "gc/gc_cpp.h"
#include "gc/gc_allocator.h"

template <typename _Key, typename _Compare = std::less<_Key> >
class gc_set : public gc, public std::set<_Key,_Compare,gc_allocator<_Key> >
{  
public:
  typedef typename std::set<_Key,_Compare,gc_allocator<_Key> > Base;
  typedef typename Base::key_type                            key_type;
  typedef typename Base::key_compare                         key_compare;
  typedef typename Base::allocator_type                      allocator_type;

  gc_set() {};
  explicit gc_set(const _Compare& __comp,const allocator_type &__a = allocator_type()) : Base(__comp,__a) {};
  template <class _InputIterator>
  gc_set(_InputIterator __f, _InputIterator __l) : Base(__f,__l) {};
  template <class _InputIterator>
  gc_set(_InputIterator __f, _InputIterator __l,
         const _Compare &__comp,const allocator_type &__a) : Base(__f,__l,__comp,__a) {};
};

template <class _Key, class _Compare =std::less<_Key> >
class gc_multiset : public gc, public std::multiset<_Key,_Compare,gc_allocator<_Key> >
{
public:
  typedef typename std::multiset<_Key,_Compare,gc_allocator<_Key> > Base;
  typedef typename Base::key_type                             key_type;
  typedef typename Base::key_compare                          key_compare;
  typedef typename Base::allocator_type                       allocator_type;

  gc_multiset() {};
  explicit gc_multiset(const _Compare& __comp,const allocator_type &__a = allocator_type()) : Base(__comp,__a) {};
  template <class _InputIterator>
  gc_multiset(_InputIterator __f, _InputIterator __l) : Base(__f,__l) {};
  template <class _InputIterator>
  gc_multiset(_InputIterator __f, _InputIterator __l,
         const _Compare &__comp,const allocator_type &__a) : Base(__f,__l,__comp,__a) {};
};

#endif


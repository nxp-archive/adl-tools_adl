//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// A garbage-collected STL map.
//

#ifndef _GC_MAP_H_
#define _GC_MAP_H_

#include <map>
#include <utility>

#include "gc/gc_cpp.h"
#include "gc/gc_allocator.h"

template <typename Key, typename _Tp, typename _Compare = std::less<Key> >
class gc_map : public gc, public std::map<Key,_Tp,_Compare,gc_allocator<std::pair<const Key, _Tp> > >
{  
public:
  typedef typename std::map<Key,_Tp,_Compare,gc_allocator<std::pair<const Key, _Tp> > > Base;
  typedef typename Base::key_type                            key_type;
  typedef typename Base::value_type                          value_type;
  typedef typename Base::mapped_type                         mapped_type;
  typedef typename Base::key_compare                         key_compare;
  typedef typename Base::allocator_type                      allocator_type;

  gc_map() {};
  explicit gc_map(const _Compare& __comp,const allocator_type &__a = allocator_type()) : Base(__comp,__a) {};
  template <class _InputIterator>
  gc_map(_InputIterator __f, _InputIterator __l) : Base(__f,__l) {};
  template <class _InputIterator>
  gc_map(_InputIterator __f, _InputIterator __l,
         const _Compare &__comp,const allocator_type &__a) : Base(__f,__l,__comp,__a) {};
};

template <class Key, class _Tp, class _Compare =std::less<Key> >
class gc_multimap : public gc, public std::multimap<Key,_Tp,_Compare,gc_allocator<std::pair<const Key, _Tp> > >
{
public:
  typedef typename std::multimap<Key,_Tp,_Compare,gc_allocator<std::pair<const Key, _Tp> > > Base;
  typedef typename Base::key_type                             key_type;
  typedef typename Base::value_type                           value_type;
  typedef typename Base::mapped_type                          mapped_type;
  typedef typename Base::key_compare                          key_compare;
  typedef typename Base::allocator_type                       allocator_type;

  gc_multimap() {};
  explicit gc_multimap(const _Compare& __comp,const allocator_type &__a = allocator_type()) : Base(__comp,__a) {};
  template <class _InputIterator>
  gc_multimap(_InputIterator __f, _InputIterator __l) : Base(__f,__l) {};
  template <class _InputIterator>
  gc_multimap(_InputIterator __f, _InputIterator __l,
         const _Compare &__comp,const allocator_type &__a) : Base(__f,__l,__comp,__a) {};
};

template <typename Key, typename _Tp, typename _Compare = std::less<Key> >
class tr_map : public gc, public std::map<Key,_Tp,_Compare,traceable_allocator<std::pair<const Key, _Tp> > >
{  
public:
  typedef typename std::map<Key,_Tp,_Compare,traceable_allocator<std::pair<const Key, _Tp> > > Base;
  typedef typename Base::key_type                            key_type;
  typedef typename Base::value_type                          value_type;
  typedef typename Base::mapped_type                         mapped_type;
  typedef typename Base::key_compare                         key_compare;
  typedef typename Base::allocator_type                      allocator_type;

  tr_map() {};
  explicit tr_map(const _Compare& __comp,const allocator_type &__a = allocator_type()) : Base(__comp,__a) {};
  template <class _InputIterator>
  tr_map(_InputIterator __f, _InputIterator __l) : Base(__f,__l) {};
  template <class _InputIterator>
  tr_map(_InputIterator __f, _InputIterator __l,
         const _Compare &__comp,const allocator_type &__a) : Base(__f,__l,__comp,__a) {};
};


#endif


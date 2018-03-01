//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// A garbage-collected STL hash set.
//

#ifndef _GC_HASH_SET_H_
#define _GC_HASH_SET_H_

#ifdef _MSC_VER
# include <hash_set>
#else
# include <tr1/unordered_set>
#endif

#include <string>

#include "gc/gc_cpp.h"
#include "gc/gc_allocator.h"

#include "stringhash.h"

#ifdef _MSC_VER

template<class _Kty,class _Tr = _STDEXT hash_compare<_Kty, _STD less<_Kty> > >
class gc_hash_set : public gc, public _STDEXT hash_set<_Kty,_Tr,gc_allocator< _Kty> >
{
public:
	typedef typename _STDEXT hash_set<_Kty,_Tr,gc_allocator< _Kty> > Base;
};

#else


template <class _Value, class _HashFcn = std::tr1::hash<_Value>, 
          class _EqualKey = std::equal_to<_Value> >
class gc_hash_set : public gc, public std::tr1::unordered_set<_Value,_HashFcn,_EqualKey,gc_allocator<_Value> >
{  
public:
  typedef typename std::tr1::unordered_set<_Value,_HashFcn,_EqualKey,gc_allocator<_Value> > Base;

  typedef typename Base::key_equal key_equal;
  typedef typename Base::hasher hasher;
  typedef typename Base::size_type size_type;
  typedef typename Base::allocator_type allocator_type;

  gc_hash_set() {};
  explicit gc_hash_set(size_type __n) : Base(__n) {};
  gc_hash_set(size_type __n, const hasher& __hf) : Base(__n,__hf) {};
  gc_hash_set(size_type __n, const hasher& __hf, const key_equal& __eql,
           const allocator_type& __a = allocator_type()) : Base(__n,__hf,__eql,__a) {};

  template <class _InputIterator>
  gc_hash_set(_InputIterator __f, _InputIterator __l) : Base(__f,__l) {};
  template <class _InputIterator>
  gc_hash_set(_InputIterator __f, _InputIterator __l, size_type __n) : Base(__f,__l,__n) {};
  template <class _InputIterator>
  gc_hash_set(_InputIterator __f, _InputIterator __l, size_type __n,
           const hasher& __hf) : Base(__f,__l,__n,__hf) {};
  template <class _InputIterator>
  gc_hash_set(_InputIterator __f, _InputIterator __l, size_type __n,
              const hasher& __hf, const key_equal& __eql,
              const allocator_type& __a = allocator_type()) : Base(__f,__l,__n,__hf,__eql,__a) {};
};

//
// Versions which use the traceable_allocator.
//

template <class _Value, class _HashFcn = std::tr1::hash<_Value>, 
          class _EqualKey = std::equal_to<_Value> >
class tr_hash_set : public gc, public std::tr1::unordered_set<_Value,_HashFcn,_EqualKey,traceable_allocator<_Value> >
{  
public:
  typedef typename std::tr1::unordered_set<_Value,_HashFcn,_EqualKey,traceable_allocator<_Value> > Base;

  typedef typename Base::key_equal key_equal;
  typedef typename Base::hasher hasher;
  typedef typename Base::size_type size_type;
  typedef typename Base::allocator_type allocator_type;

  tr_hash_set() {};
  explicit tr_hash_set(size_type __n) : Base(__n) {};
  tr_hash_set(size_type __n, const hasher& __hf) : Base(__n,__hf) {};
  tr_hash_set(size_type __n, const hasher& __hf, const key_equal& __eql,
           const allocator_type& __a = allocator_type()) : Base(__n,__hf,__eql,__a) {};

  template <class _InputIterator>
  tr_hash_set(_InputIterator __f, _InputIterator __l) : Base(__f,__l) {};
  template <class _InputIterator>
  tr_hash_set(_InputIterator __f, _InputIterator __l, size_type __n) : Base(__f,__l,__n) {};
  template <class _InputIterator>
  tr_hash_set(_InputIterator __f, _InputIterator __l, size_type __n,
           const hasher& __hf) : Base(__f,__l,__n,__hf) {};
  template <class _InputIterator>
  tr_hash_set(_InputIterator __f, _InputIterator __l, size_type __n,
              const hasher& __hf, const key_equal& __eql,
              const allocator_type& __a = allocator_type()) : Base(__f,__l,__n,__hf,__eql,__a) {};
};

#endif

#endif

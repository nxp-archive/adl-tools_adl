//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// A garbage-collected STL hash.
//

#ifndef _GC_HASH_MAP_H_
#define _GC_HASH_MAP_H_

#include <string>

#include "gc/gc_cpp.h"
#include "gc/gc_allocator.h"

#include "stringhash.h"

#ifdef _MSC_VER

template<class _Kty,class _Ty,class _Tr = _STDEXT hash_compare<_Kty, _STD less<_Kty> > >
class gc_hash_map : public gc, public _STDEXT hash_map<_Kty,_Ty,_Tr,gc_allocator< _STD pair<const _Kty,_Ty> > >
{
public:
	typedef typename _STDEXT hash_map<_Kty,_Ty,_Tr,gc_allocator< _STD pair<const _Kty,_Ty> > > Base;
};

#else

template <class _Key, class _Tp, class _HashFcn = std::tr1::hash<_Key>, 
          class _EqualKey = std::equal_to<_Key> >
class gc_hash_map : public gc, public std::tr1::unordered_map<_Key,_Tp,_HashFcn,_EqualKey,gc_allocator<_Tp> >
{  
public:
  typedef typename std::tr1::unordered_map<_Tp,gc_allocator<_Tp> > Base;
  typedef typename Base::size_type                             size_type;
  typedef typename std::pair<const _Key,_Tp>                   value_type;
  typedef typename Base::hasher                                hasher;
  typedef typename Base::key_equal                             key_equal;
  typedef typename Base::allocator_type                        allocator_type;

  gc_hash_map() {};
  explicit gc_hash_map(size_type __n) : Base(__n) {};
  gc_hash_map(size_type __n, const hasher& __hf) : Base(__n,__hf) {};
  gc_hash_map(size_type __n, const hasher& __hf, const key_equal& __eql,
           const allocator_type& __a = allocator_type()) : Base(__n, __hf, __eql, __a) {}
  template <class _InputIterator>
  gc_hash_map(_InputIterator __f, _InputIterator __l) : Base(__f,__l) {};
  template <class _InputIterator>
  gc_hash_map(_InputIterator __f, _InputIterator __l, size_type __n) : Base(__f,__l,__n) {};
  template <class _InputIterator>
  gc_hash_map(_InputIterator __f, _InputIterator __l, size_type __n,
              const hasher& __hf) : Base(__f,__l,__n,__hf) {};
  template <class _InputIterator>
  gc_hash_map(_InputIterator __f, _InputIterator __l, size_type __n,
              const hasher& __hf, const key_equal& __eql,
              const allocator_type& __a = allocator_type()) : Base(__f,__l,__n,__hf,__eql,__a) {};
};

template <class _Key, class _Tp, class _HashFcn = std::tr1::hash<_Key>, 
          class _EqualKey = std::equal_to<_Key> >
class gc_hash_multimap : public gc, public std::tr1::unordered_multimap<_Key,_Tp,_HashFcn,_EqualKey,gc_allocator<_Tp> >
{
public:
  typedef typename std::tr1::unordered_multimap<_Tp,gc_allocator<_Tp> > Base;
  typedef typename Base::size_type                             size_type;
  typedef typename std::pair<const _Key,_Tp>                   value_type;
  typedef typename Base::hasher                                hasher;
  typedef typename Base::key_equal                             key_equal;
  typedef typename Base::allocator_type                        allocator_type;

  gc_hash_multimap() {};
  explicit gc_hash_multimap(size_type __n) : Base(__n) {};
  gc_hash_multimap(size_type __n, const hasher& __hf) : Base(__n,__hf) {};
  gc_hash_multimap(size_type __n, const hasher& __hf, const key_equal& __eql,
                   const allocator_type& __a = allocator_type()) : Base(__n, __hf, __eql, __a) {}
  template <class _InputIterator>
  gc_hash_multimap(_InputIterator __f, _InputIterator __l) : Base(__f,__l) {};
  template <class _InputIterator>
  gc_hash_multimap(_InputIterator __f, _InputIterator __l, size_type __n) : Base(__f,__l,__n) {};
  template <class _InputIterator>
  gc_hash_multimap(_InputIterator __f, _InputIterator __l, size_type __n,
                   const hasher& __hf) : Base(__f,__l,__n,__hf) {};
  template <class _InputIterator>
  gc_hash_multimap(_InputIterator __f, _InputIterator __l, size_type __n,
                   const hasher& __hf, const key_equal& __eql,
                   const allocator_type& __a = allocator_type()) : Base(__f,__l,__n,__hf,__eql,__a) {};  
};

//
// Versions which use the traceable_allocator.
//

template <class _Key, class _Tp, class _HashFcn = std::tr1::hash<_Key>, 
          class _EqualKey = std::equal_to<_Key> >
class tr_hash_map : public gc, public std::tr1::unordered_map<_Key,_Tp,_HashFcn,_EqualKey,traceable_allocator<_Tp> >
{  
public:
  typedef typename std::tr1::unordered_map<_Tp,traceable_allocator<_Tp> > Base;
  typedef typename Base::size_type                             size_type;
  typedef typename std::pair<const _Key,_Tp>                   value_type;
  typedef typename Base::hasher                                hasher;
  typedef typename Base::key_equal                             key_equal;
  typedef typename Base::allocator_type                        allocator_type;

  tr_hash_map() {};
  explicit tr_hash_map(size_type __n) : Base(__n) {};
  tr_hash_map(size_type __n, const hasher& __hf) : Base(__n,__hf) {};
  tr_hash_map(size_type __n, const hasher& __hf, const key_equal& __eql,
           const allocator_type& __a = allocator_type()) : Base(__n, __hf, __eql, __a) {}
  template <class _InputIterator>
  tr_hash_map(_InputIterator __f, _InputIterator __l) : Base(__f,__l) {};
  template <class _InputIterator>
  tr_hash_map(_InputIterator __f, _InputIterator __l, size_type __n) : Base(__f,__l,__n) {};
  template <class _InputIterator>
  tr_hash_map(_InputIterator __f, _InputIterator __l, size_type __n,
              const hasher& __hf) : Base(__f,__l,__n,__hf) {};
  template <class _InputIterator>
  tr_hash_map(_InputIterator __f, _InputIterator __l, size_type __n,
              const hasher& __hf, const key_equal& __eql,
              const allocator_type& __a = allocator_type()) : Base(__f,__l,__n,__hf,__eql,__a) {};
};

template <class _Key, class _Tp, class _HashFcn = std::tr1::hash<_Key>, 
          class _EqualKey = std::equal_to<_Key> >
class tr_hash_multimap : public gc, public std::tr1::unordered_multimap<_Key,_Tp,_HashFcn,_EqualKey,traceable_allocator<_Tp> >
{
public:
  typedef typename std::tr1::unordered_multimap<_Tp,traceable_allocator<_Tp> > Base;
  typedef typename Base::size_type                             size_type;
  typedef typename std::pair<const _Key,_Tp>                   value_type;
  typedef typename Base::hasher                                hasher;
  typedef typename Base::key_equal                             key_equal;
  typedef typename Base::allocator_type                        allocator_type;

  tr_hash_multimap() {};
  explicit tr_hash_multimap(size_type __n) : Base(__n) {};
  tr_hash_multimap(size_type __n, const hasher& __hf) : Base(__n,__hf) {};
  tr_hash_multimap(size_type __n, const hasher& __hf, const key_equal& __eql,
                   const allocator_type& __a = allocator_type()) : Base(__n, __hf, __eql, __a) {}
  template <class _InputIterator>
  tr_hash_multimap(_InputIterator __f, _InputIterator __l) : Base(__f,__l) {};
  template <class _InputIterator>
  tr_hash_multimap(_InputIterator __f, _InputIterator __l, size_type __n) : Base(__f,__l,__n) {};
  template <class _InputIterator>
  tr_hash_multimap(_InputIterator __f, _InputIterator __l, size_type __n,
                   const hasher& __hf) : Base(__f,__l,__n,__hf) {};
  template <class _InputIterator>
  tr_hash_multimap(_InputIterator __f, _InputIterator __l, size_type __n,
                   const hasher& __hf, const key_equal& __eql,
                   const allocator_type& __a = allocator_type()) : Base(__f,__l,__n,__hf,__eql,__a) {};  
};

#endif

#endif


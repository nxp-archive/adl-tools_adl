//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Various classes used for querying a model about its resources.
// This is for querying *about* resources and not about their current
// state during simulation.
//

#ifndef _INTROSPECTION_H_
#define _INTROSPECTION_H_

#include <algorithm>

#include "helpers/BasicTypes.h"

namespace adl {

typedef std::vector<int> IntVect;
typedef std::vector<std::string> StrVect;

// Base class used by all instrospection data structure for storing flag
// attribute information.  The idea is that we store attributes as integers and
// it's fast (O(log n)) to query to see if something has an attribute.  To get
// attribute handles from a string name, call getAttrData() and then you can call
// the get_attr() method on that data structure.
class Attrs {
public:
  struct Params {  
    enum Type {NoParam, Int, IntList, Str, StrList};    
    Type _type;
    IntVect _int_list;
    StrVect _str_list;
    Params():_type(NoParam){};
    Params(std::string s)      : _type(Str) { _str_list.push_back(s);};
    Params(int i)              : _type(Int) { _int_list.push_back(i);}; 
    Params(StrVect &sv)   : _type(StrList){
      _str_list.insert(_str_list.end(), sv.begin(), sv.end());
    };
    Params(IntVect &iv)   : _type(IntList){
      _int_list.insert(_int_list.end(), iv.begin(), iv.end());
    };      

    bool has_data() const { return _type != NoParam; };

    void dump(std::ostream &o) const;
  };  
    
  typedef std::pair<unsigned , Params> Attr;
  typedef std::vector<Attr> AttrList;
  AttrList _attrs;
    
  virtual ~Attrs() {};
  virtual void finalize();

  // Returns true if we have the specified attribute.
  bool has_attr(unsigned c) const;  

  // Returns data associated with an attribute.  Returns a pair, where the first element
  // is true if the attribute exists, the second parm is the data.
  std::pair<bool,Params> get_attr_data(unsigned c) const;
}; 

struct AttrData;

// This function must be called before the printing of any introspection data,
// in order to register the relevant attribute database. 
void setAttrData(const AttrData &);


// Common fields used by various introspection objects.
struct IntroBase : Attrs {
  std::string _name;
};

struct SharedIntroBase : IntroBase {
  bool _shared;
};

std::ostream &operator<<(std::ostream &o,const Attrs &);
std::ostream &operator<<(std::ostream &o,const IntroBase &);
std::ostream &operator<<(std::ostream &o,const SharedIntroBase &);

// Information about an MMU field.
struct MmuFieldInfo : public IntroBase {
  bool        _setfield;
  unsigned    _width;
};
typedef std::vector<MmuFieldInfo> MmuFieldInfos;

std::ostream &operator<<(std::ostream &o,const MmuFieldInfo &);
std::ostream &operator<<(std::ostream &o,const MmuFieldInfos &);

// For getting information about an mmu lookup.  Note: The pagesize is set to
// the page size in bytes if the lookup has fixed-size pages.  Otherwise, the
// size is set to 0.
struct MmuLookupInfo : public SharedIntroBase {
  unsigned       _id;
  unsigned       _sets;
  unsigned       _ways;
  unsigned       _pagesize;
  unsigned       _pageshift;
  MmuFieldInfos  _fields;
};
// Information about all lookups in the mmu.
typedef std::vector<MmuLookupInfo> MmuInfo;

std::ostream &operator<<(std::ostream &o,const MmuLookupInfo &);
std::ostream &operator<<(std::ostream &o,const MmuInfo &);

struct RegFieldInfo : public IntroBase {
  typedef std::pair<unsigned, unsigned> BitPair;    
  typedef std::vector<BitPair> Bits;
  Bits _bits; 
};
typedef std::vector<RegFieldInfo> RegFieldInfos;

std::ostream &operator<<(std::ostream &o,const RegFieldInfo &);
std::ostream &operator<<(std::ostream &o,const RegFieldInfos &);

struct EnumValueInfo : public std::pair<int,std::string> { 
  EnumValueInfo(int i,std::string s) : std::pair<int,std::string>(i,s) {};
};
typedef std::vector<EnumValueInfo>     EnumValueInfos;
  
std::ostream &operator<<(std::ostream &o, const EnumValueInfo &);
std::ostream &operator<<(std::ostream &o, const EnumValueInfos &);

struct IdBase {
  unsigned _id;

  IdBase() {};
  IdBase(unsigned id) : _id(id) {};
};

// For getting information about a register or register file.
struct RegInfo : public SharedIntroBase, public IdBase {
  unsigned          _width;    // Register width in bits.
  unsigned          _size;     // If non-zero, then size of register file.
  bool              _pseudo;
  bool              _sparse;
  RegFieldInfos     _fields;
  EnumValueInfos    _enums;

  bool is_regfile() const { return _size; };
};
typedef std::vector<RegInfo> RegInfos;

std::ostream &operator<<(std::ostream &o,const RegInfo &);
std::ostream &operator<<(std::ostream &o,const RegInfos &);


// For getting information about contexts.
struct CtxInfo : public SharedIntroBase, public IdBase {
  unsigned          _num_contexts;
  bool              _is_thread;
  unsigned          _size;          // Size in bytes for all elements in the
                                    // context, except those excluded by the
                                    // omit list.
};
typedef std::vector<CtxInfo> CtxInfos;

std::ostream &operator<<(std::ostream &o,const CtxInfo &);
std::ostream &operator<<(std::ostream &o,const CtxInfos &);

// For getting information about caches.
struct CacheInfo : public SharedIntroBase {
  CacheType _type;
  unsigned  _level;
  unsigned  _size;
  unsigned  _linesize;
  IntVect   _sets;
  IntVect   _ways;
};
typedef std::vector<CacheInfo> CacheInfos;

std::ostream &operator<<(std::ostream &o,const CacheInfo &);
std::ostream &operator<<(std::ostream &o,const CacheInfos &);

// For getting information about exceptions.
struct ExceptionInfo : public IntroBase, public IdBase {
  unsigned        _flag;
  unsigned        _priority;
  SensitivityType _sensitivity;
};
typedef std::vector<ExceptionInfo> ExceptionInfos;

std::ostream &operator<<(std::ostream &o,const ExceptionInfo &);
std::ostream &operator<<(std::ostream &o,const ExceptionInfos &);

// For getting information about memories
struct MemInfo : public SharedIntroBase, public IdBase {
  bool 	      _instr_mem;
  bool        _extern;
  unsigned    _size;
  unsigned    _addr_unit;
  std::string _parent;
  unsigned    _offset;
};
typedef std::vector<MemInfo> MemInfos;

std::ostream &operator<<(std::ostream &o,const MemInfo &);
std::ostream &operator<<(std::ostream &o,const MemInfos &);

template<class T>
struct SortById {
  bool operator()(const T &x,const T &y) const {
    return x._id < y._id;
  }
};

template<class C>
void sort_info(C &container)
{
  std::sort(container.begin(),container.end(),SortById<typename C::value_type>());
}

struct FindById {
  bool operator()(const IdBase &x,const IdBase &y) {
    return x._id < y._id;
  }
};

// This will search within a sorted introspection array for the item specified
// by a given identifier.
template<class C>
typename C::const_iterator find_info(const C& container,unsigned id)
{
  return std::lower_bound(container.begin(),container.end(),id,FindById());
}


}

#endif


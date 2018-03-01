//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef _TOKEN_H_
#define _TOKEN_H_

#include <iosfwd>
#include <string>
#include <cstring>
#include <vector>
#include <algorithm>
#include <unordered_map>

#include "helpers/BasicTypes.h"
#include "helpers/Macros.h"

using namespace std;

namespace adl {

// A simple read-only token/string class.
class Token  {
public:
  Token() : _ptr(0), _len(0),_alloc(false){};
    
    
  // If alloc is true then memory will be allocated and content will be copied
  // else these constructors create Token which only point to the string- they do not
  // copy it.
  Token(const char *p,int n, bool alloc = true);
  Token(const char *p,       bool alloc = true);
    
    
  Token(const Token &x);   
  Token(Token &&x);
    
  // This will always allocate a new string using the gc and copy
  // the contents over.
  Token(const std::string &s);
  // This will always allocate a new string of the given size
  // using the garbage collector.
  Token(int n);  
    
  virtual ~Token();

  bool operator==(const Token &x) const;
  bool operator==(const char *x) const;
  bool operator==(const std::string &x) const;

  bool operator<(const Token &x) const;
  bool operator!=(const Token &x) const;

  // Returns the strcmp value:  < 0 if this is less than x, > 0 if this is > x, else 0.
  int cmp(const Token &x) const;

  friend bool operator<(const Token &x,const char *n);
  friend bool operator<(const char *n,const Token &x);

  friend bool operator<(const Token &x,const std::string &n);
  friend bool operator<(const std::string &n,const Token &x);

  virtual Token& operator=(const Token &x);
  Token& operator=(Token &&x);   

  // This will allocate a new string, containing contents
  // + the argument string.  The original contents are
  // not modified.
  Token append(const Token &x) const; 

  bool empty() const { return !_len; };
  unsigned size() const { return _len; };
  char operator[](unsigned n) { return _ptr[n]; };
  // resize does not allocate new space, simply extend the length of the current one
  void resize(unsigned new_len) {_len = new_len; };

  // Direct access to the underyling data.  Careful: This is not
  // null-terminated!
  const char *data() const { return _ptr; };
  // This increments the data pointer and decrements the length.
  const char *incr_data() { if (_len) { --_len; ++_ptr; } return _ptr; };

  std::string str() const { return std::string(_ptr,_len); };
  friend std::ostream &operator<< (std::ostream &,const Token &);
protected:
  friend struct TokenHash;

  const char *_ptr;
  unsigned _len;
  bool _alloc;
  //indicator that memory was allocated for data, not just pointer
    
};

inline bool Token::operator==(const Token &x) const
{
  if (_len != x._len) {
    return false;
  } else {
    return !strncmp(_ptr,x._ptr,_len);
  }
}

inline bool Token::operator==(const char *x) const
{
  size_t s = strlen(x);
  if (_len != s) {
    return false;
  }
  return !strncmp(_ptr,x,s);
}

inline bool operator==(const char *str,const Token &x)
{
  return x == str;
}

inline bool Token::operator==(const std::string &x) const
{
  return x.compare(0,size(),data(),size()) == 0;
}

inline bool operator==(const std::string &str,const Token &x)
{
  return x == str;
}

// If the strings are of the same size, returns the value based upon the
// lexographic ordering of the two strings, else < 0 if this is shorter than x,
// else > 0 if longer than x.
inline int Token::cmp(const Token &x) const
{
  const size_t len = min(_len,x._len);
  int r = strncmp(_ptr,x._ptr,len);
  if (!r) {
    r = _len - x._len;
  }
  return r;
}

inline bool Token::operator!=(const Token &x) const
{
  return !(*this == x);
}

inline bool Token::operator<(const Token &x) const
{
  return (cmp(x) < 0);
}

inline bool operator<(const Token &x,const char *n)
{
  int ret = 0 ;

  unsigned char *src = (unsigned char*)x._ptr;
  unsigned char *end = src + x._len;
  unsigned char *dst = (unsigned char*)n;
  while ((src < end) && ((ret = *src - *dst) == 0)) {
    ++src, ++dst;
  }

  if (ret == 0) {
    // If we've gotten to the end of the token and n still has a non-null
    // character, then it's longer and thus is > than x.
    return *dst;
  }
  return ret < 0;
}

inline bool operator<(const char *n,const Token &x)
{
  int ret = 0 ;

  unsigned char *src = (unsigned char*)n;
  unsigned char *dst = (unsigned char*)x._ptr;
  unsigned char *end = dst + x._len;

  while ((dst < end) && ((ret = *src - *dst) == 0)) {
    ++src, ++dst;
  }

  if (ret == 0) {
    // If we've gotten to the end of the token and n still has a non-null
    // character, then it's longer, and thus n is not less than x.
    return false;
  }
  return ret < 0;
}

inline bool operator<(const Token &x,const std::string &n)
{
  return operator<(x,n.c_str());
}

inline bool operator<(const std::string &n,const Token &x)
{
  return operator<(n.c_str(),x);
}

// Useful for using Tokens, or something like that, in a sorted
// vector and then using equal_range to find the elements.
template <class T>
struct ComparePair {
  bool operator()(const T &x,const T &y) {
    return x.first < y.first;
  }

  bool operator()(const T &x,const char *n) {
    return x.first < n;
  }

  bool operator()(const char *n,const T &x) {
    return n < x.first;
  }

  bool operator()(const T &x,const Token &y) {
    return x.first < y;
  }

  bool operator()(const Token &x,const T &y) {
    return x < y.first;
  }
};

// This is a simple associative container based upon a sorted sequence.  You
// first add items, then finalize it, then can search it.  Its memory
// performance is better than a map, assuming that you can divide your usage
// into this phased approach.
template <class T>
struct TokLookup : std::vector<std::pair<Token,T> > {

  typedef std::vector<std::pair<Token,T> >   base_type;
  typedef std::pair<Token,T>                 pair_type;
  typedef ComparePair<pair_type>             compare_type;
  typedef typename base_type::iterator       iterator;
  typedef typename base_type::const_iterator const_iterator;

  void finalize() {
    std::sort(this->begin(),this->end(),compare_type());
  }

  iterator find_key(const char *n) {
    std::pair<iterator,iterator> range = std::equal_range(this->begin(),this->end(),n,compare_type());
    if ( range.first != range.second ) {
      return range.first;
    } else {
      return this->end();
    }
  }   
   
  const_iterator find_key(const char *n) const {
    return const_cast<TokLookup<T> &>(*this).find_key(n);
  }

  iterator find_key(const Token &t) {
    std::pair<iterator,iterator> range = std::equal_range(this->begin(),this->end(),t,compare_type());
    if ( range.first != range.second ) {
      return range.first;
    } else {
      return this->end();
    }
  }
   
  bool has_key(const char *n) const {
    return was_found(find_key(n));
  }
    
  const_iterator find_key(const Token &t) const {
    return const_cast<TokLookup<T> &>(*this).find_key(t);
  }

  bool not_found(const iterator &x) const {
    return (x == this->end());
  }

  bool not_found(const const_iterator &x) const {
    return (x == this->end());
  }

  bool was_found(const iterator &x) const {
    return (x != this->end());
  }

  bool was_found(const const_iterator &x) const {
    return (x != this->end());
  }
};

  uint64_t atoi64(const char *s,const char *msg);
  uint64_t atoi64(const Token &t,const char *msg);

  uint64_t atoi64(const char *s,unsigned len,const char *msg);
  bool atoi64_exact(const Token &t,uint64_t &v);

  bool atoi64(const char *s,uint64_t &v);
  bool atoi64(const Token &t,uint64_t &v);

  uint32_t atoi32(const char *s,const char *msg);
  uint32_t atoi32(const Token &t,const char *msg);

}


#ifdef _MSC_VER

namespace stdext {

	inline size_t hash_value(adl::Token x)
	{
		unsigned long __h = 0;
		for (unsigned i = 0; i != x.size(); ++i)
			__h = 5*__h + x[i];
		return size_t(__h);
	}

}

#else

namespace std {

  // Hash/set support for Token.
  template<> struct hash<adl::Token>
  {
    size_t operator()(adl::Token x) const { 
      unsigned long __h = 0;
      for (unsigned i = 0; i != x.size(); ++i)
        __h = 5*__h + x[i];
      return size_t(__h);
    }
  };
}

#endif



#endif

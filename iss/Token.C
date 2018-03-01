//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#include <iostream>
#include <sstream>
#include <stdexcept>
#include <errno.h>
#include <assert.h>

#include "helpers/Macros.h"

#include "Token.h"
extern int errno;

using namespace std;

#ifdef _MSC_VER
# define strtoll _strotoi64
# define strtoull _strtoui64
#endif


namespace adl {

  //
  // Token methods.
  //
  // No need to clear it b/c the garbage collector does that for us.

  Token::Token(int n) :
    _len(n),
    _alloc(true)
  {
    _ptr = new char[n];
  }

  Token::Token(const string &s) :   
    _len(s.size()),
    _alloc(true)
  {
    _ptr = new char[_len+1];
    strncpy((char*)_ptr,s.c_str(),_len+1);
    (char&)_ptr[_len] = 0;
  }

  Token::Token(const char *p,int n,bool alloc) :
    _len(n),
    _alloc(alloc)
  {    
    if (alloc) {
     _ptr = new char[n+1];
     strncpy((char*)_ptr,p,_len);
     (char&)_ptr[_len] = 0;
    } else {
      _ptr = p;
    }
  }  
  
  Token::Token(const Token &x) :
    _len(x._len),
    _alloc(x._alloc)
  {
    if (_alloc) {
      _ptr = new char[_len+1];
      strncpy((char*)_ptr,x._ptr,_len);
      (char&)_ptr[_len] = 0;
    } else {
      _ptr = x._ptr;
    }
  }  

  // Move-construction semantics.
  Token::Token(Token &&x) :
    _ptr(x._ptr),
    _len(x._len),
    _alloc(x._alloc)
  {
    // Since we're moving it, we want to make sure that the other guy doesn't
    // de-alloc the object if it was allocated.
    x._alloc = false;
  }

  // Move-assigment semantics.
  Token& Token::operator=(Token &&x) 
  {
    // Don't want the other guy to delete his item.
    if (_alloc) { 
      delete [] _ptr; 
    };
    _ptr = x._ptr;
    _len = x._len;
    _alloc = x._alloc;
    x._alloc = false;
    return *this;
  }
  
  Token& Token::operator=(const Token &x) 
  {
    // If we're about to overwrite something we alloc'd, then delete what we
    // had.
    if (_alloc) { 
      delete [] _ptr; 
    };
    _len = x._len;
    _alloc = x._alloc;
    if (_alloc) {
      _ptr = new char[_len+1];
      strncpy((char*)_ptr,x._ptr,_len);  
      (char&)_ptr[_len] = 0;
    } else {
      _ptr = x._ptr;
    }
    return *this;
  }

  Token::Token(const char *p, bool alloc) : 
    _len(strlen(p)), 
    _alloc(alloc) 
  {
    if (_alloc) {
      _ptr = new char[_len+1]; 
      strncpy((char*)_ptr,p,_len);  
      (char&)_ptr[_len] = 0;
    } else {
      _ptr = p;
    } 
  };


  Token::~Token()
  {
    if (_alloc) {
      delete [] _ptr;
    }
  };


  ostream &operator<< (ostream &o,const Token &s)
  {
    o.write(s._ptr,s._len);
    return o;
  }

  Token Token::append(const Token &x) const
  {
	DeclArray(char,s,(_len+x._len+1));
    strncpy(s,_ptr,_len);
    strncpy(s+_len,x._ptr,x._len);
    return Token(s,_len+x._len,true);
  }
  
  uint64_t atoi64(const char *s,const char *msg)
  {
    char *error;
    uint64_t data = strtoull(s,&error,0);
    if (s == error) {
      if (msg) {
        RError("Expected a numerical value for " << msg);
      } else {
        return data;
      }
    }
    return data;
  }

  uint64_t atoi64(const Token &t,const char *msg)
  {
    return atoi64(t.data(),msg);
  }

  bool atoi64(const char *s,uint64_t &v)
  {
    char *error;
    v = strtoull(s,&error,0);
      
    if (s == error || errno == ERANGE) {
      return false;
    }
    return true;
  }

  bool atoi64(const Token &t,uint64_t &v)
  {
    return atoi64(t.data(),v);
  }

  bool atoi64(const char *s,unsigned len,uint64_t &v)
  {
    char *error;
    v = strtoull(s,&error,0);
      
    if (s == error || ((error-s) < (int)len) || errno == ERANGE) {
      return false;
    }
    return true;
  }

  bool atoi64_exact(const Token &t,uint64_t &v)
  {
    return atoi64(t.data(),t.size(),v);
  }

  uint32_t atoi32(const char *s,const char *msg)
  {
    char *error;
    uint32_t data = strtoul(s,&error,0);
    if (s == error) {
      if (msg) {
        RError("Expected a numerical value for " << msg);
      } else {
        return 0;
      }
    }
    return data;
  }

  uint32_t atoi32(const Token &t,const char *msg)
  {
    return atoi32(t.data(),msg);
  }

  int32_t atoint32(const char *s,const char *msg)
  {
    char *error;
    int32_t data = strtol(s,&error,0);
    if (s == error) {
      if (msg) {
        RError("Expected a numerical value for " << msg);
      } else {
        return -1;
      }
    }
    return data;
  }

  int32_t atoint32(const Token &t,const char *msg)
  {
    return atoint32(t.data(),msg);
  }

}

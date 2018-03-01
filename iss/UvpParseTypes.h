//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
//
// Various types used for parsing UVP files.
//

#ifndef _UVP_PARSE_TYPES_H_
#define _UVP_PARSE_TYPES_H_

#include <list>
#include <memory>

#include "Token.h"

namespace adl {

  // Base object for value objects in a UVP.
  struct UvpValue {
    virtual ~UvpValue() {};

    virtual bool operator==(const UvpValue *uv) const = 0;
    virtual void print(std::ostream &o) const = 0;

  };

  template <class T>
  bool compare_uvp_types(const T &x,const UvpValue *y) {
    if (const T *t = dynamic_cast<const T*>(y)) {
      return x == *t;
    } else {
      return false;
    }
  }

  // A value object which stores a list.
  struct UvpList : public UvpValue, public std::list<std::unique_ptr<UvpValue> > {
    typedef std::list<std::unique_ptr<UvpValue> > base_type;
    virtual bool operator==(const UvpValue *uv) const {
      return compare_uvp_types(*this,uv);
    }
    virtual void print(std::ostream &o) const;
  };

  // A value object which stores a map.
  struct UvpMap : public UvpValue, public TokLookup<std::unique_ptr<UvpValue> > {

    typedef TokLookup<std::unique_ptr<UvpValue> > base_type;

    virtual bool operator==(const UvpValue *uv) const {
      return compare_uvp_types(*this,uv);
    }
    virtual void print(std::ostream &o) const;
  };

  // Tokens are all derived from this class, which is just a simple read-only
  // string class.  The type of the object is used for type information of the
  // token itself.  Everything is gc'd so that pointers can be passed around
  // without worrying about cleanup.
  struct UvpToken : public UvpValue, public Token {     
    typedef Token base_type;

    UvpToken() {};
    UvpToken(const char *p,int n,bool alloc = true) : Token(p,n,alloc) {};
    UvpToken(const Token &x) : Token(x){};
    virtual ~UvpToken() {};

    using Token::operator==;

    virtual bool operator==(const UvpValue *uv) const {
      return compare_uvp_types(*this,uv);
    }

    virtual void print(std::ostream &o) const;
  };

  inline std::ostream &operator<< (std::ostream &o,const UvpValue *v) {
    if (v) {
      v->print(o);
    } else {
      o << "<null>";
    }
    return o;
  }

  inline std::ostream &operator<< (std::ostream &o,const UvpValue &v) {
    v.print(o);
    return o;
    
  }

  inline std::ostream &operator<< (std::ostream &o,const std::unique_ptr<UvpToken> &ut) {
    return operator<<(o,static_cast<const UvpValue*>(ut.get()));
  }

  inline std::ostream &operator<< (std::ostream &o,const std::unique_ptr<UvpValue> &uv) {
    return operator<<(o,uv.get());
  }

#if defined(_MSC_VER) && (_MSC_VER < 1400)

#define TokenType(x, arg1)                               \
  struct x##Token : public UvpToken {                   \
    x##Token() {}; \
    x##Token(const char *p,int n, bool alloc=true) : UvpToken(p,n,alloc) {};   \
      arg1                                       \
        }

#else

#define TokenType(x, ...)                               \
  struct x##Token : public UvpToken {                   \
    x##Token() {}; \
    x##Token(const char *p,int n, bool alloc=true) : UvpToken(p,n,alloc) {};   \
      __VA_ARGS__                                       \
        }

#endif

  TokenType (Ident, void dummy() { }; );
  TokenType (Num, virtual bool operator==(const UvpValue *uv) const; );
  TokenType (Str, StrToken(const Token &x) : UvpToken(x) {}; \
             virtual void print(std::ostream &o) const;  );

  // Stores information about a single UVP command, which consists of a port,
  // command, and map of parms, indexed by the parameter name.
  struct UvpCmd {

    std::unique_ptr<UvpToken> _port;
    std::unique_ptr<UvpToken> _cmd;
    std::unique_ptr<UvpToken> _instance;

    UvpMap    _parms;

    UvpCmd() : _port(nullptr), _cmd(nullptr), _instance(nullptr) {};
  };

  std::ostream &operator<< (std::ostream &o,const UvpCmd &v);

}

#endif

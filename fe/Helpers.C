//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Miscellaneous helper typedefs, macros, etc.
//

#include <stdio.h>
#include <iostream>
#include <cassert>
#include <sstream>
#include <unistd.h>

#include "opencxx/parser/LeafName.h"
#include "opencxx/parser/DupLeaf.h"
#include "opencxx/parser/PtreeUtil.h"
#include "opencxx/parser/PtreeArray.h"
#include "opencxx/parser/Encoding.h"
#include "opencxx/parser/ptree-generated.h"
#include "opencxx/parser/AbstractTranslatingWalker.h"
#include "opencxx/PtreeIter.h"
#include "opencxx/Walker.h"

#include "helpers/fe_exceptions.h"

#include "Types.h"
#include "Helpers.h"
#include "ExprWalker.h"

using namespace std;
using namespace Opencxx;
using namespace PtreeUtil;

namespace adl {

  const char *Indent = "  ";

  Ptree *strdup(const gc_string &s)
  {
    return new DupLeaf((char*)s.c_str(),s.size());
  }

  Ptree *strconst(const char *s)
  {
    return new Leaf((char*)s,strlen(s));
  }

  gc_string indent(const gc_string &pfx)
  {
    return pfx + Indent;
  }

  const char *ptos(Opencxx::Ptree *p)
  {
    if (!p) {
      return "";
    } else {
      return p->ToString();
    }
  }

  // Returns a Ptree object storing a string version of the given number.
  Ptree *uint2p(unsigned v)
  {
    const int bufsize = 20;
    char buf[bufsize];
    int l = snprintf(buf,bufsize,"%d",v);
    return new DupLeaf(buf,l);
  }

  const char *get_str_name(Leaf &n)
  {
    char *str = new (GC) char[n.GetLength()+1];
    strncpy(str,n.GetPosition(),n.GetLength());
    str[n.GetLength()] = 0;
    return str;
  }

  // Returns a string of any type of node.
  gc_string get_str_name(Opencxx::Ptree *p)
  {
    ostringstream ss;
    ss << p;
    return gc_string(ss.str().c_str());
  }

  // For a given type, try to get that kind of value.  Return true if found,
  // false otherwise.  We always have to evaluate for an expression, in case the
  // user has specified an enum.
  template <typename N>
  bool get_type(N &v,Ptree *value,Environment *env = 0)
  {
    ExprWalker<N> ew(env);
    return reifyPtree(v,ew.Translate(value));
  }
  
  // Tries to get a boolean- returns true if it got it, false otherwise.
  bool get_bool(bool &v,Ptree *value,Environment *env)
  {
    return get_type(v,value,env);
  }
  
  // Tries to get a boolean- throws an exception if it can't using the message
  // provided.  This will accept a numeric value as well, converting zero to
  // false and non-zero to true.
  bool get_bool(Ptree *value,const char *msg,Environment *env)
  {
    bool val;
    if (get_bool(val,value,env)) {
      return val;
    } else {
      PError(value,"Expected a boolean value for " << msg);
    }
  }

  bool get_uint(unsigned &v,Ptree *value,Opencxx::Environment *env)
  {
    return get_type(v,value,env);
  }

  // Tries to get an unsigned int- throws an exception if it can't using
  // the message provided.
  uint32_t get_uint(Ptree *value,const char *msg,Opencxx::Environment *env)
  {
    unsigned val;
    if (get_uint(val,value,env)) {
      return val;
    } else {
      PError(value,"Expected an unsigned integer value for " << msg);
    }
  }

  bool get_int(int &v,Ptree *value,Opencxx::Environment *env)
  {
    return get_type(v,value,env);
  }
  
  // Tries to get a signed int- throws an exception if it can't using
  // the message provided.
  int32_t get_int(Ptree *value,const char *msg,Opencxx::Environment *env)
  {
    int val;
    if (get_int(val,value,env)) {
      return val;
    } else {
      PError(value,"Expected an integer value for " << msg);
    }
  }

  bool get_uint64(uint64_t &v,Ptree *value,Opencxx::Environment *env)
  {
    return get_type(v,value,env);
  }
    
  // Tries to get a 64-bit unsigned integer.
  uint64_t get_uint64(Ptree *value,const char *msg,Opencxx::Environment *env)
  {
    uint64_t val;
    if (get_uint64(val,value,env)) {
      return val;
    } else {
      PError(value,"Expected an integer value for " << msg);
    }
  }

  addr_t get_addr(Ptree *value,const char *msg,Opencxx::Environment *env)
  {
    return get_uint64(value,msg,env);
  }

  // Returns true if the item is an integer.  No conversion is done so that
  // integers of arbitrary size can be handled.
  bool get_uintarb(Ptree *value,const char *msg)
  {
    unsigned val;
    if (value->Reify(val)) {
      return true;
    } else if (msg) {
      PError(value,"Expected an integer value for " << msg);
    } else {
      return false;
    }
  }

  bool get_dbl(double &v,Ptree *value)
  {
    return get_type(v,value);
  }
  
  double get_dbl(Opencxx::Ptree *value,const char *msg)
  {
    double val;
    if (get_dbl(val,value)) {
      return val;
    } else {
      PError(value,"Expected a floating point value for " << msg);
    }
  }

  // Tries to get an identifier- throws an exception if it can't.
  // This will accept either leafname objects or quoted strings.
  const char *get_str(Ptree *p,const char *msg)
  {
    char *s;
    if (LeafName *n = dynamic_cast<LeafName*>(p)) {
      return get_str_name(*n);
    } else if (p->Reify(s)) {
      return s;
    } else {
      const char *n = p->ToString();
      if (is_ident(n)) {
        return n;
      }
    }
    if (msg) {
      PError(p,"Expected an identifier or string for " << msg);
    } else {
      return 0;
    }
  }

  bool is_ident(const char *str)
  {
    if (str) {
      if (isalpha(*str) || (*str == '_')) {
        for ( ++str; *str; ++str) {
          if (!(isalnum(*str) || (*str == '_'))) {
            return false;
          }
        }
        return true;
      }
    }
    return false;
  }

  bool is_int(Ptree *value,Environment *env)
  {
    unsigned dummy;
    return get_uint(dummy,value,env);
  }

  const char *get_ident(Ptree *p,const char *msg)
  {
    const char *ident = get_str(p,msg);
    if (!is_ident(ident)) {
      if (msg) {
        PError(p,"Expected a valid C++ identifier for " << msg);
      } else {
        return 0;
      }
    }
    return ident;
  }

  // This version will not reify strings.
  const char *get_simple_ident(Opencxx::Ptree *p,const char *msg)
  {
    if (Leaf *n = dynamic_cast<Leaf*>(p)) {
      return get_str_name(*n);
    } else if (msg) {
      PError(p,"Expected a simple identifier for " << msg);
    } else {
      return 0;
    }
  }

  // Tries to extract the arity of the given function- assumes that this
  // is a function.
  unsigned get_arity(Ptree *value)
  {
    if (Eq(First(value),"{")) {
      // Just a block- arity is 0.
      return 0;
    } else {
      Ptree *args;
      if (Match(value,"[func ( %? ) %_ ]",&args)) {
        if (!args) {
          // Null found for expression in parentheses, so arity is 0.
          return 0;
        }
        PtreeIter iter(args);
        Ptree *p;
        unsigned arity = 0;
        while ( (p = iter()) ) {
          if ( !Eq(p,",")) {
            ++arity;
          }
        }
        return arity;
      } else {
        abort();
      }
    }
  }

  // Tries to get a function-call- throws an exception if it can't.  If msg is 0,
  // returns 0 on failure.
  Opencxx::Ptree *get_func_call(Opencxx::Ptree *value,const char *msg)
  {
    if (dynamic_cast<PtreeFuncallExpr*>(value)) {
      return value;
    } else if (msg) {
      PError(value,"Expected a function call expression for " << msg);
    } else {
      return 0;
    }
  }

  // Assuming that this is a function call, return the name of the function.
  Ptree *get_func_call_name(Ptree *value)
  {
    return First(value);
  }

  // Number of arguments in the function.
  int get_func_call_numargs(Ptree *value)
  {
    Ptree *args = Third(value);

    if (args) {
      int numargs = 0;
      while (args) {
        if (!Eq(args->Car(),",")) ++numargs;
        args = args->Cdr();
      }
      return numargs;
    } else {
      return 0;
    }
  }

  // Assuming that this is a function call, return the nth argument (starting at 0).
  Ptree *get_func_call_arg(Ptree *value,int arg)
  {
    return NthDeclarator(value,arg);
  }


  // Tries to get a lambda function with a specified arity- throws an
  // exception if it can't.
  Ptree *get_func(Ptree *value,int arity,const char *msg)
  {
    if (!value || value->IsLeaf()) {

      // Do we have a constant of 0 (interpreted as a null function)?
      if (value && value->IsLeaf()) {
        unsigned val;
        if (get_uint(val,value,0) && (val == 0)) {
          // Yes- we have the special null-function identifier.
          return 0;
        }
      }

      if (msg) {
        PError(value,"Expected a function of arity " << arity << " for " << msg);
      } else {
        return 0;
      }
    }

    Ptree *header = First(value);
    if (header && (Eq(First(value),"{") || Eq(First(value),"func"))) {
      if (arity >= 0) {
        int f_arity = get_arity(value);
        if (arity != f_arity) {
          if (msg) {
            PError(value,"Expected a function of arity " << arity << " but found arity of " << f_arity
                   << " for " << msg);
          } else {
            return 0;
          }
        }
      }
      return value;
    } else {
      if (msg) {
        PError(value,"Expected a function of arity " << arity << " for " << msg);
      } else {
        return 0;
      }
    }
  }

  Ptree *get_func_args(Ptree *value)
  {
    if (!value) { return 0; };
    if (Eq(First(value),"{")) {
      return 0;
    } else {
      return Nth(value,2);
    }
  }

  Ptree *get_func_body(Ptree *value)
  {
    if (!value) { return 0; };
    if (Eq(First(value),"{")) {
      return value;
    } else {
      return Nth(value,4);
    }
  }

  // Returns a pair, where the first element is a Ptree object representing the
  // name portion of a register-file reference and the second element is an
  // index.
  pair<Ptree *,int> get_reg_refp(Ptree *value,const char *msg,bool allow_child_id,
                                 Environment *env)
  {
    Ptree *name, *index;
    int i;
    if (Match(value,"[ %? ( [ %? ] )]",&name,&index)) {
      if (!get_int(i,index,env)) {
        if (!allow_child_id) {
          PError(value,"Expected an integer for " << msg);
        }
        const char *ci = 0;
        if (!(ci = get_ident(index,0))) {
          PError(value,"Expected an integer or 'child_id' for " << msg);
        }
        if (strcmp(ci,"child_id") == 0) {
          i = -2;
        } else {
          PError(value,"Expected an integer or 'child_id' for " << msg);
        }
      }
      return make_pair(name,i);
    } else {
      return make_pair(value,-1);
    }
  }

  // Returns a pair, where the first element is an identifier and the second
  // element is an index.  This matches against a simple identifier (in which case
  // the index is -1) or a function call, e.g. foo(1).
  pair<const char *,int> get_reg_ref(Ptree *value,const char *msg,bool allow_child_id,Environment *env)
  {
    auto rp = get_reg_refp(value,msg,allow_child_id,env);
    const char *n;

    if (rp.second >= 0) {
      n = get_ident(rp.first,msg);
      return make_pair(n,rp.second);
    } else {
      if ( (n = get_ident(rp.first,0)) ) {
        return make_pair(n,rp.second);
      } else if (msg) {
        PError(value,"Expected an identifier or '<identifier>(<index>)' for " << msg);
      } else {
        return make_pair((const char *)0,rp.second);
      }
    }
  }

  bool arg_is_bits(Ptree *func,int argn,int size)
  {
    if (func) {
      // Make sure that the argument's type is 'bits' of the right width.
      Ptree *arg = NthDeclarator(func,argn);
      if (arg) {
        Ptree *decl = Second(arg);
        const char *type = decl->GetEncodedType();

        Encoding encode_arg;
        encode_arg.ValueParam(size);
        Encoding encode;
        encode.Template(Ptree::Make("bits"),encode_arg);

        return (strcmp(type,encode.Get()) == 0);

      } else {
        return false;
      }
    } else {
      return true;
    }
  }

  bool arg_is_bits(Ptree *func,int argn,const char *size)
  {
    if (func) {
      // Make sure that the argument's type is 'bits' of the right width.
      Ptree *arg = NthDeclarator(func,argn);
      if (arg) {
        Ptree *decl = Second(arg);
        const char *type = decl->GetEncodedType();

        Encoding encode_arg;
        encode_arg.SimpleName(Ptree::Make("%s",size));
        Encoding encode;
        encode.Template(Ptree::Make("bits"),encode_arg);

        return (strcmp(type,encode.Get()) == 0);

      } else {
        return false;
      }
    } else {
      return true;
    }
  }

  bool arg_is_uint(Ptree *func,int argn)
  {
    if (func) {
      // Make sure that the argument's type is an unsigned int.
      Ptree *arg = NthDeclarator(func,argn);
      if (arg) {
        Ptree *decl = Second(arg);
        const char *type = decl->GetEncodedType();

        Encoding encode;
        encode.Append("Ui",2);

        return (strcmp(type,encode.Get()) == 0);

      } else {
        return false;
      }
    } else {
      return true;
    }
  }

  bool arg_is_int(Ptree *func,int argn)
  {
    if (func) {
      // Make sure that the argument's type is an int.
      Ptree *arg = NthDeclarator(func,argn);
      if (arg) {
        Ptree *decl = Second(arg);
        const char *type = decl->GetEncodedType();

        Encoding encode;
        encode.Append("i",2);

        return (strcmp(type,encode.Get()) == 0);

      } else {
        return false;
      }
    } else {
      return true;
    }
  }

  bool arg_is_bool(Ptree *func,int argn)
  {
    if (func) {
      // Make sure that the argument's type is an unsigned int.
      Ptree *arg = NthDeclarator(func,argn);
      if (arg) {
        Ptree *decl = Second(arg);
        const char *type = decl->GetEncodedType();

        Encoding encode;
        encode.Append("b",2);

        return (strcmp(type,encode.Get()) == 0);

      } else {
        return false;
      }
    } else {
      return true;
    }
  }

  // Returns true if the specified argument is the specified name.  The Ptree argument
  // should be the entire function declaration.  The first argument is 0.
  bool arg_is_type(Opencxx::Ptree *func,const char *rtype,int argn)
  {
    if (func) {
      // Make sure that the argument's type matches rtype.
      Ptree *arg = NthDeclarator(func,argn);
      if (arg) {
        Ptree *decl = Second(arg);
        const char *type = decl->GetEncodedType();

        Encoding encode;
        encode.SimpleName(Ptree::Make(rtype));

        return (strcmp(type,encode.Get()) == 0);

      } else {
        return false;
      }
    } else {
      return true;
    }
  }

  bool arg_is_type(Opencxx::Ptree *func,const gc_string &rtype,int argn)
  {
    return arg_is_type(func,rtype.c_str(),argn);
  }

  bool arg_is_type(Opencxx::Ptree *func,const StrVect &rtypes,int argn)
  {
    if (func) {
      // Make sure that the argument's type is one of the elements of rtypes.
      Ptree *arg = NthDeclarator(func,argn);
      if (arg) {
        Ptree *decl = Second(arg);
        const char *type = decl->GetEncodedType();

        ForEach(rtypes,r_iter) {
          Encoding encode;
          encode.SimpleName(Ptree::Make(r_iter->c_str()));

          if (strcmp(type,encode.Get()) == 0) {
            return true;
          }
        }
        return false;
      } else {
        return false;
      }
    } else {
      return true;
    }
  }

  bool arg_is_addr(Ptree *func,int argn)
  {
    return arg_is_type(func,"addr_t",argn);
  }

  // For a specified argumeent, returns a pair consisting of the type and the argument name.
  pair<gc_string,gc_string> get_func_arg(Ptree *func,int argn)
  {
    if (func) {
      Ptree *arg = NthDeclarator(func,argn);
      if (arg) {
        return make_pair(gc_string(First(arg)->ToString()),gc_string(Second(arg)->ToString()));
      }
    }
    return make_pair(gc_string(),gc_string());
  }

  // Return the name of the nth parameter.
  Ptree *get_func_arg_name(Ptree *func,int argn)
  {
    if (func) {
      Ptree *arg = NthDeclarator(func,argn);
      if (arg) {
        return Second(arg)->Car();
      }
    }
    return 0;
  }


  // Given a comma-delimited list, this returns a flattened list of items.
  Ptree *flatten_clist(Ptree *p)
  {
    if (!p) return List();
    if (p->IsLeaf()) return List(p);
    Ptree *l,*r;
    if (Match(p,"[ %? , %? ]",&l,&r)) {
      Ptree *lresult = flatten_clist(l);
      Ptree *rresult = flatten_clist(r);

      if (!lresult) {
        return rresult;
      }
      if (!rresult) {
        return lresult;
      }
      return Append(lresult,rresult);
    } else {
      return List(p);
    }
  }

  // Returns the flattened list if this is a parenthese-delimited list, or 0
  // otherwise.
  Ptree *get_flat_list(Ptree *v)
  {
    Ptree *l;
    if (Match(v," [ ( %? ) ] ",&l)) {
      return flatten_clist(l);
    }
    return 0;
  }

  void shallowReplace(PtreeArray &array,const char *orig,Ptree *replace) 
  {
    ForRange(array.Number(),i) {
      if (Eq(array[i],orig)) {
        array[i] = replace;
      }
    }
  }

  // Populate an array with arguments from a function call.
  void populateArgs(PtreeArray &array,Walker *w,Ptree *exp,CurArgHandler *curarg,bool &changed)
  {
    Ptree *args = Third(exp);

    unsigned index = 0;
    while (args != 0) {
      Ptree *p = args->Car();
      if (curarg) {
        curarg->setCurArg(index);
      }
      Ptree *q = w->Translate(p);
      if (curarg) {
        curarg->doneWithArg();
      }
      ++index;
      if (p != q) {
        changed = true;
      }
      array.Append(q);
      args = args->Cdr();
      if (args != 0) {
        array.Append(args->Car());
        args = args->Cdr();
      }
    }
  }

  // Retrieve an argument from a populated array.
  // Some adjustments are necessary to the index due to the inclusion of
  // commas in the array.
  Ptree* getArg(PtreeArray &array,unsigned n)
  {
    if (2*n >= array.Number()) {
      return 0;
    } else {
      return array[2*n];
    }
  }

  void setArg(PtreeArray &array,unsigned n,Ptree *x)
  {
    array[2*n] = x;
  }

  unsigned numArgs(PtreeArray &array)
  {
    return (array.Number()+1) / 2;
  }

  unsigned log2(unsigned v)
  {
    static const char LogTable256[] = {
      0, 0, 1, 1, 2, 2, 2, 2, 3, 3, 3, 3, 3, 3, 3, 3,
      4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4, 4,
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
      5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7,
      7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7, 7
    };

    unsigned r;     // r will be lg(v)
    register unsigned int t, tt; // temporaries
  
    if ( (tt = v >> 16) ) {
      r = (t = tt >> 8) ? 24 + LogTable256[t] : 16 + LogTable256[tt];
    } else {
      r = (t = v >> 8) ? 8 + LogTable256[t] : LogTable256[v];
    }
    return r;
  }

  // Returns true if the argument string represents an intbv type conversion method.
  bool isTypeConversion(Ptree *x)
  {
    if ( Eq(x,"uint32") ) {
      return true;
    }
    if ( Eq(x,"int32") ) {
      return true;
    }
    if ( Eq(x,"uint64") ) {
      return true;
    }
    if ( Eq(x,"int64") ) {
      return true;
    }

    return false;
  }

  CacheType get_cache_type(const gc_string &n)
  {
    if (n == "instr") {
      return InstrCache;
    } else if (n == "data") {
      return DataCache;
    } else if (n == "unified") {
      return UnifiedCache;
    } else {
      RError("Unknown cache type " << n);
    }
  }

  gc_string get_cache_type_str(CacheType type)
  {
    switch (type) {
    case InstrCache:
      return "instr";
    case DataCache:
      return "data";
    default:
      return "unified";
    }
  }

  // Given an argument list, removes the nth argument and returns the remaining arguments.
  Ptree *removeArgN(Ptree *args,unsigned n)
  {
    unsigned i = 0;

    Ptree *new_args = PtreeUtil::List();

    if (args) {
      PtreeIter iter(args);
      Ptree *p;

      while ( (p = iter()) ) {
        if (i != n) {
          new_args = PtreeUtil::Append(new_args,p);
        } else {
          ++i;
          p = iter();
        }
        if ( !Eq(p,",")) {
          ++i;
        }
      }      
    }
    return new_args;
  }

  // Converts the specified arguments into constant references.
  Ptree *constify_args(Ptree *func,const StrList &const_args,int offset)
  {
    int ac = offset;
    Ptree *newargs = get_func_args(func);
    ForEach(const_args,arg) {
      newargs = PtreeUtil::Subst(Ptree::Make("const %s &",arg->c_str()),
                                 get_func_call_arg(func,ac)->Car(),newargs);
      ++ac;
    }
    return newargs;
  }

  // Converts the specified arguments into non-const references.
  Ptree *referize_args(Ptree *func,const StrList &const_args,int offset)
  {
    int ac = offset;
    Ptree *newargs = get_func_args(func);
    ForEach(const_args,arg) {
      newargs = PtreeUtil::Subst(Ptree::Make("%s &",arg->c_str()),
                                 get_func_call_arg(func,ac)->Car(),newargs);
      ++ac;
    }
    return newargs;
  }

  // Converts the types of the specified arguments from the original to the new.
  Ptree *transform_args(Ptree *func,const StrList &arg_mods,int offset)
  {
    int ac = offset;
    Ptree *newargs = get_func_args(func);
    ForEach(arg_mods,arg) {
      newargs = PtreeUtil::Subst(Ptree::Make(arg->c_str()),
                                 get_func_call_arg(func,ac)->Car(),newargs);
      ++ac;
    }
    return newargs;    
  }

  // Add on an argument to the argument list of a function and return the new argument list.
  Ptree *add_arg(Ptree *func,Ptree *arg) 
  {
    Ptree *origargs = get_func_args(func);
    Ptree *newargs = 0;
    if (!origargs) {
      newargs = arg;
    } else {
      newargs = PtreeUtil::Append(origargs,PtreeUtil::List(Ptree::Make(","),arg));
    }
    return newargs;
  }

  // This attempts to do an intelligent comparison between an index and a
  // slot-index, ignoring type conversion operators, e.g. X.uint32() will match
  // against X.
  bool index_compare(Ptree *index,Ptree *slot_index)
  {
    // First, try a normal comparison.
    if (compare(index,slot_index) == 0) {
      return true;
    }
    // Else, try seeing if this is a dot expression with a type conversion.
    Ptree *f, *tc;
    if (Match(slot_index,"[[%? . %?] ( %* )]",&f,&tc) && isTypeConversion(tc)) {
      if (compare(index,f) == 0) {
        return true;
      }
    }
    return false;
  }

  void print_mem_usage(const char *pfx)
  {
    char buf[2048];

    sprintf(buf,"echo -n '%s: ' ; cat /proc/%d/status | grep VmSize",pfx,getpid());
    system(buf);
  }

  gc_string extractBaseFuncName(Ptree *fun)
  {
    if (!fun->IsLeaf()) {
      // In case this is a template-function call.
      fun = First(fun);
    }
    return fun->ToString();
  }

 
}

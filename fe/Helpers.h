//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Miscellaneous helper typedefs, macros, etc.
//

#ifndef _HELPERS_H_
#define _HELPERS_H_

#include <sstream>
#include <iterator>
#include <limits.h>

#include "opencxx/PtreeIter.h"

#include "helpers/Macros.h"
#include "helpers/BasicTypes.h"

#include "Types.h"

namespace Opencxx {
  class Leaf;
  class Walker;
  class PtreeArray;
  class Ptree;
}

namespace adl {

  // Returns the number of trailing 0s in a word.
  inline int trailing_zeros(unsigned x)
  {
    return (x) ? __builtin_ctz(x) : 1;
  }
  
  inline int trailing_zeros_64(uint64_t x)
  {
    return (x) ? __builtin_ctzll(x) : 1;
  }

  // Finds the next highest power of 2 for a given value.
  template <class T>
  T next_higher_p2(T k) {
    if (k == 0)
      return 1;
    k--;
    for (unsigned i=1; i<sizeof(T)*CHAR_BIT; i<<=1)
      k = k | k >> i;
    return k+1;
  }
  
  // Returns a string version of a Ptree object.  This is null-safe-
  // an empty string will be returned in such a case.
  const char *ptos(Opencxx::Ptree *p);
  
  // Returns a Ptree object storing a string version of the given number.
  Opencxx::Ptree *uint2p(unsigned v);

  // Returns the log base 2 of the value.
  unsigned log2(unsigned x);

  // Duplicates a string, returning a DupLeaf of the same contents.
  Opencxx::Ptree *strdup(const gc_string &);

  // Creates a leaf object which points to a constant string.
  Opencxx::Ptree *strconst(const char *s);

  // Useful for dumping hierarchies- returns a new string with space
  // appended.
  gc_string indent(const gc_string &pfx);

  // Given a leaf, returns a string of its contents.
  const char *get_str_name(Opencxx::Leaf &n);

  // Returns a string of any type of node.
  gc_string get_str_name(Opencxx::Ptree *);

  // Tries to get the specified type; throws an exception if it can't using the
  // message provided.  An Environment may be supplied in order to resolve enum
  // values; if a 0 is used for environment, then no enum expansion will occur.
  bool get_bool(Opencxx::Ptree *value,const char *msg,Opencxx::Environment *env);
  unsigned get_uint(Opencxx::Ptree *value,const char *msg,Opencxx::Environment *env);
  int get_int(Opencxx::Ptree *value,const char *msg,Opencxx::Environment *env);
  uint64_t get_uint64(Opencxx::Ptree *value,const char *msg,Opencxx::Environment *env);
  addr_t get_addr(Opencxx::Ptree *value,const char *msg,Opencxx::Environment *env);
  double get_dbl(Opencxx::Ptree *value,const char *msg);

  // Alternate version- returns false if it can't get an integer.
  bool get_bool(bool &v,Opencxx::Ptree *value,Opencxx::Environment *env);
  bool get_uint(unsigned &v,Opencxx::Ptree *value,Opencxx::Environment *env);
  bool get_int(int &v,Opencxx::Ptree *value,Opencxx::Environment *env);
  bool get_uint64(uint64_t &v,Opencxx::Ptree *value,Opencxx::Environment *env);
  bool get_dbl(double &v,Opencxx::Ptree *value);

  // Returns true if the item is an integer literal, but doesn't do any
  // conversion.  Useful for handling integers of arbitrary size.  This doesn't
  // do any expression evaluation.
  bool get_uintarb(Opencxx::Ptree *value,const char *msg);

  // Tries to get an identifier- throws an exception if it can't.
  // This will accept either leaf objects or quoted strings.  If the message
  // is 0, will return 0 on failure.
  const char *get_str(Opencxx::Ptree *p,const char *msg);

  // Returns true if the supplied string is an identifier.
  bool is_ident(const char *str);

  // Returns true if the supplied item is an integer.  An environment may be
  // specified for enum expansion or may be set to 0 for no expansion.
  bool is_int(Opencxx::Ptree *value,Opencxx::Environment *env);

  // Same as above, but the result must be a valid C identifier.  Throws a
  // parse_error on failure is msg is non-null, otherwise returns 0.
  const char *get_ident(Opencxx::Ptree *p,const char *msg);

  // This version will not reify strings.  If the message is 0, will return
  // 0 on failure.
  const char *get_simple_ident(Opencxx::Ptree *p,const char *msg);

  // Tries to get a lambda function with a specified arity- throws an exception
  // if it can't.  As a special exception, the constant 0 is allowed, which will
  // return a null function.  If msg is 0, returns 0 on failure.  If arity is
  // -1, the arity is not checked.
  Opencxx::Ptree *get_func(Opencxx::Ptree *value,int arity,const char *msg);

  // Tries to extract the arity of the given function- assumes that this
  // is a function.
  unsigned get_arity(Opencxx::Ptree *value);

  // Returns the argument body of a lambda function, or 0 if one is not supplied.
  Opencxx::Ptree *get_func_args(Opencxx::Ptree *value);

  // Returns the body of a lambda function.
  Opencxx::Ptree *get_func_body(Opencxx::Ptree *value);

  // Tries to get a function-call- throws an exception if it can't.  If msg is 0,
  // returns 0 on failure.
  Opencxx::Ptree *get_func_call(Opencxx::Ptree *value,const char *msg);

  // Assuming that this is a function call, return the name of the function.
  Opencxx::Ptree *get_func_call_name(Opencxx::Ptree *value);

  // Number of arguments in the function call.
  int get_func_call_numargs(Opencxx::Ptree *value);

  // Assuming that this is a function call, return the nth argument (starting at 0).
  Opencxx::Ptree *get_func_call_arg(Opencxx::Ptree *value,int arg);

  // Returns a pair, where the first element is a Ptree and the second element
  // is an index.  This matches against a simple identifier (in which case the
  // index is -1) or a function call, e.g. foo(1).
  // On failure, is msg is non-null, throws a parse_error, else returns (0,-1).
  // if allow_child_id is true, then we allow the special child_id identifier to be used.
  std::pair<Opencxx::Ptree *,int> get_reg_refp(Opencxx::Ptree *value,const char *msg,bool allow_child_id,Opencxx::Environment *env);

  // Returns a pair, where the first element is an identifier and the second
  // element is an index.  This matches against a simple identifier (in which case
  // the index is -1) or a function call, e.g. foo(1).
  // On failure, is msg is non-null, throws a parse_error, else returns (0,-1).
  // if allow_child_id is true, then we allow the special child_id identifier to be used.
  std::pair<const char *,int> get_reg_ref(Opencxx::Ptree *value,const char *msg,bool allow_child_id,Opencxx::Environment *env);

  // Returns true if the specified argument declaration is for a `bits` type of the
  // given size.  The Ptree argument should be the entire function declaration.
  // The first argument is 0.
  bool arg_is_bits(Opencxx::Ptree *func,int arg,int size);

  // Same as above, except that the size requirement is specified as a string.
  bool arg_is_bits(Opencxx::Ptree *func,int arg,const char *size);

  // Returns true if the specified argument is an unsigned integer.  The Ptree
  // argument should be the entire function declaration.
  // The first argument is 0.
  bool arg_is_uint(Opencxx::Ptree *func,int arg);

  // Returns true if the specified argument is an integer.  The Ptree
  // argument should be the entire function declaration.
  // The first argument is 0.
  bool arg_is_int(Opencxx::Ptree *func,int arg);

  // Returns true if the specified argument is a bool.  The Ptree argument
  // should be the entire function declaration.  The first argument is 0.
  bool arg_is_bool(Opencxx::Ptree *func,int arg);

  // Returns true if the specified argument is an addr_t.  The Ptree argument
  // should be the entire function declaration.  The first argument is 0.
  bool arg_is_addr(Opencxx::Ptree *func,int arg);

  // Returns true if the specified argument is the specified name.  The Ptree argument
  // should be the entire function declaration.  The first argument is 0.
  bool arg_is_type(Opencxx::Ptree *func,const char *type,int arg);
  bool arg_is_type(Opencxx::Ptree *func,const gc_string &type,int arg);
  bool arg_is_type(Opencxx::Ptree *func,const StrVect &types,int arg);

  // For a specified argumeent, returns a pair consisting of the type and the argument name.
  std::pair<gc_string,gc_string> get_func_arg(Opencxx::Ptree *func,int arg);

  // Return the name of the nth parameter.
  Opencxx::Ptree *get_func_arg_name(Opencxx::Ptree *func,int argn);

  // Given a comma-delimited list, this returns a flattened list of items.
  Opencxx::Ptree *flatten_clist(Opencxx::Ptree *p);

  // Returns the flattened list if this is a parenthese-delimited list, or 0
  // otherwise.
  Opencxx::Ptree *get_flat_list(Opencxx::Ptree *v);

  // Returns true if the argument string represents an intbv type conversion method.
  bool isTypeConversion(Opencxx::Ptree *x);

  // Returns a CacheType enum, given a string.  Throws a runtime_error if the input string is not recognized.
  CacheType get_cache_type(const gc_string &n);
  gc_string get_cache_type_str(CacheType);

  //
  // Argument processing helper functions.
  //

  // Base class used by populateArgs() to convey what parameter is currently being handled.  This can be
  // left null if you don't care.
  struct CurArgHandler {
    virtual ~CurArgHandler() {};

    virtual void setCurArg(unsigned index) = 0;
    virtual void doneWithArg() = 0;
  };

  // Populate array with arguments.
  void shallowReplace(Opencxx::PtreeArray &array,const char *orig,Opencxx::Ptree *replace);
  void populateArgs(Opencxx::PtreeArray &array,Opencxx::Walker *w,Opencxx::Ptree *args,CurArgHandler *,bool &changed);
  Opencxx::Ptree* getArg(Opencxx::PtreeArray &array,unsigned n);
  void setArg(Opencxx::PtreeArray &array,unsigned n,Opencxx::Ptree *x);
  unsigned numArgs(Opencxx::PtreeArray &array);

  //templatea for processing containers of pairs
  template<typename T1, typename T2>
  class equal_1st_in_pair {
  public:
    equal_1st_in_pair(T1 const & value) : value_(value) {}

    typedef std::pair<T1, T2> pair_type;
    bool operator() (pair_type p) const {
        return p.first == value_;
    }
  private:
    T1 const & value_;
  };

  template<typename T1, typename T2>
  class equal_2nd_in_pair {
  public:
    equal_2nd_in_pair(T2 const & value) : value_(value) {}

    typedef std::pair<T1, T2> pair_type;
    bool operator() (pair_type p) const {
        return p.second == value_;
    }
  private:
    T2 const & value_;
  };


  template<typename T1, typename T2>
  class less_1st_in_pair {
  public:
    less_1st_in_pair(T1 const & value) : value_(value) {}

    typedef std::pair<T1, T2> pair_type;
    bool operator() (pair_type p) const {
        return p.first < value_;
    }
  private:
    T1 const & value_;
  };

  template<typename T1, typename T2>
  class less_2nd_in_pair {
  public:
    less_2nd_in_pair(T2 const & value) : value_(value) {}

    typedef std::pair<T1, T2> pair_type;
    bool operator() (pair_type p) const {
        return p.second < value_;
    }
  private:
    T2 const & value_;
  };


  //
  // Various generic printing routines.
  //

  template <class C>
  void printSequence(std::ostream &o,const C &c,bool newline)
  {
    std::ostream_iterator<typename C::value_type> oiter(o," ");

    std::copy(c.begin(),c.end(),oiter);

    if (newline) {
      o << '\n';
    }
  }

  template <class C>
  void printSequence(std::ostream &o,const C &c,const char *pfx, bool newline)
  {
    std::ostream_iterator<typename C::value_type> oiter(o,pfx);

    std::copy(c.begin(),c.end(),oiter);

    if (newline) {
      o << '\n';
    }
  }

  template <class C>
  gc_string getSequence(const C &c)
  {
    gc_ostringstream o;
    printSequence(o,c,false);
    return o.str();
  }

  template <class C>
  void printAssoc(std::ostream &o,const C &c,bool newline)
  {
    std::ostream_iterator<typename C::value_type> oiter(o," ");

    ForEach(c,i) {
      o << i->first << "(" << i->second << ") ";
    }

    if (newline) {
      o << '\n';
    }
  }

  template <class C>
  gc_string getAssoc(const C &c)
  {
    std::ostringstream o;
    printAssoc(o,c,false);
    return o.str();
  }

  template <class C>
  void printSequenceKeys(std::ostream &o,const C &c,bool newline)
  {
    std::ostream_iterator<typename C::key_type> oiter(o," ");

    std::transform(c.begin(),c.end(),oiter,std::_Select1st<typename C::value_type>());

    if (newline) {
      o << '\n';
    }
  }

  template <class C>
  void printSequenceOfPairs(std::ostream &o,const C &c,bool newline)
  {
    ForEach(c,i){
      o << " (" << i->first << ", " << i->second << ")  ";
    }
    if (newline) {
      o << '\n';
    }
  }

  template <class C>
  void printSequenceValues(std::ostream &o,const C &c,bool newline)
  {
    std::ostream_iterator<typename C::mapped_type> oiter(o," ");

    transform(c.begin(),c.end(),oiter,std::_Select2nd<typename C::value_type>());

    if (newline) {
      o << '\n';
    }
  }

  template <class T>
  struct NameOrder {
    bool operator()(T x,T y) const {
      return (x->name() < y->name());
    };
  };

  template <class Hash>
  void printHashOrdered(std::ostream &o,const gc_string &pfx,const Hash &h)
  {
    ForEach (h,i) {
      i->second->dump(o,pfx);
    }
  }

  // Given an argument list, removes the nth argument and returns the remaining arguments.
  Opencxx::Ptree *removeArgN(Opencxx::Ptree *args,unsigned n);

  // Gets the function arguments of a lambda function.
  Opencxx::Ptree *funcArgs(Opencxx::Ptree *f);

  // Converts the specified arguments into constant references.
  Opencxx::Ptree *constify_args(Opencxx::Ptree *func,const StrList &const_args,int offset = 0);

  // Converts the specified arguments into non-const references.
  Opencxx::Ptree *referize_args(Opencxx::Ptree *func,const StrList &const_args,int offset = 0);

  // Converts the types of the specified arguments from the original to the new.
  Opencxx::Ptree *transform_args(Opencxx::Ptree *func,const StrList &arg_mods,int offset = 0);  

  // Add on an argument to the argument list of a function and return the new
  // function.  The 'func' parm should represent the complete parameter (type
  // and name).
  Opencxx::Ptree *add_arg(Opencxx::Ptree *func,Opencxx::Ptree *arg);

  // This attempts to do an intelligent comparison between an index and a
  // slot-index, ignoring type conversion operators, e.g. X.uint32() will match
  // against X.
  bool index_compare(Opencxx::Ptree *index,Opencxx::Ptree *slot_index);  


  void print_mem_usage(const char *pfx);

  // Extracts out just the function name, even when this is a template-function
  // call.
  gc_string extractBaseFuncName(Opencxx::Ptree *fun);

}

#endif

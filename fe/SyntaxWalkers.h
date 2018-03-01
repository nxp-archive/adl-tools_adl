//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Classes that allow to invert syntax into dsyntax
//

#ifndef _SYNTAX_INVERTER_H_
#define _SYNTAX_INVERTER_H_

#include <iosfwd>
#include <string>
#include <functional>

#include "gc/gc_cpp.h"

#include "helpers/BasicTypes.h"

#include "Types.h"

namespace Opencxx {
  class Environment;
}

namespace adl {
  
// Class walks only thru conditions of expression and stores all possible
// outputs of expression.
class CondWalker: public Opencxx::Walker {
public:
  CondWalker(Opencxx::Environment *env) : 
    Opencxx::Walker(env),
    _num_cond(0),
    _exprs(new Opencxx::PtreeArray)
  {
  }
      
  virtual Opencxx::Ptree* TranslateInfix(Opencxx::Ptree* exp) { return exp;};
  virtual Opencxx::Ptree* TranslateUnary(Opencxx::Ptree* exp) { return exp;};
    
  Opencxx::PtreeArray* get_exprs() { return _exprs;};
  uint32_t get_cond() { return _num_cond;};
  virtual Opencxx::Ptree* TranslateCond(Opencxx::Ptree* exp);
private: 
  uint32_t _num_cond; //number of conditional expressions met
  Opencxx::PtreeArray* _exprs;//stores all possible output expressions   
}; 
  
 
// Class for substiting given variable by any expression.
class SubstWalker : public Opencxx::Walker {
public:
  SubstWalker(Opencxx::Environment* env,const PMap &renames);

  // Alternate constructor:  Directly take a from -> to pair.
  SubstWalker(Opencxx::Environment* env,const gc_string &from,Opencxx::Ptree *to);
  SubstWalker(Opencxx::Environment* env,const gc_string &from,const gc_string &to);

  void add_rename(const gc_string &from,Opencxx::Ptree *to);
  void add_rename(const gc_string &from,const gc_string &to);

  virtual Opencxx::Ptree* TranslateVariable(Opencxx::Ptree *var);
  virtual Opencxx::Ptree* TranslateFuncall(Opencxx::Ptree *var);
  virtual Opencxx::Ptree* TranslatePtree(Opencxx::Ptree *var);
private:
  Opencxx::Ptree *get_rename(const gc_string &n);

  PMap _renames; // From -> To pairs for substitution.
}; 


// Walker for inverting simple expressions: only one variable, operations =
// ">>","<<","~","+", "-" unary and binary, "+" - binary
class InvWalker: public Opencxx::Walker {
public:
  InvWalker(Opencxx::Environment* env) : Opencxx::Walker(env){};
  virtual Opencxx::Ptree* TranslateInfix(Opencxx::Ptree* exp);
  virtual Opencxx::Ptree* TranslateUnary(Opencxx::Ptree* exp); 
};

}

#endif

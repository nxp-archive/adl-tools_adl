//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
//
// A simple walker for examinin whether code contains only instruction fields,
// other other kinds of external resources.
//

#include <string>

#include "opencxx/parser/Ptree.h"

#include "Data.h"
#include "ResWalker.h"
#include "Helpers.h"

using namespace std;
using namespace Opencxx;

namespace adl {

ResWalker::ResWalker(const InstrBase *instr) : 
  Walker((Environment*)0), 
  _fixed(true), 
  _instr(instr) 
{};

bool ResWalker::is_fixed() const
{
  return _fixed;
}

bool ResWalker::is_instrfield(Ptree *x)
{
  gc_string name = x->ToString();
  return _instr && _instr->getAll(name) >= 0;
}

Ptree *ResWalker::handleVariable(Ptree *exp)
{
  if ( !is_instrfield(exp)) {
    _fixed = false;
  }
  return exp;
}

Ptree *ResWalker::TranslateVariable(Ptree *exp)
{
  return handleVariable(exp);
}

Ptree *ResWalker::TranslatePtree(Ptree *exp)
{
  bool       b;
  unsigned   i;
  char      *s;
  if (get_bool(b,exp,env) || get_uint(i,exp,env) || exp->Reify(s)) {
    return exp;
  }
  return handleVariable(exp);
}

}

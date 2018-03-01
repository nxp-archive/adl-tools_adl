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

#ifndef _RESWALKER_H_
#define _RESWALKER_H_

#include "opencxx/Walker.h"

namespace adl {

class InstrBase;

// This is an auxiliary walker used just for parsing expressions.  After
// traversing an expression, it records whether any other variables besides
// instruction fields were seen.
class ResWalker : public Opencxx::Walker {
public:
  ResWalker(const InstrBase *instr);

  bool is_fixed() const;

  bool is_instrfield(Opencxx::Ptree *x);

  Opencxx::Ptree *handleVariable(Opencxx::Ptree *exp);

  Opencxx::Ptree *TranslateVariable(Opencxx::Ptree *exp);

  Opencxx::Ptree *TranslatePtree(Opencxx::Ptree *exp);

private:
  bool             _fixed;  // True => fixed resource (only instrfields and constants).
  const InstrBase *_instr;  // Instruction to search for fields.
};   

}

#endif

//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Base class for a walker which translates the ADL data model.  This exists due
// to limitations in how C++ can easily handle a visitor pattern, or at least
// how OpenC++ implements a visitor pattern.  The problem is that multiple
// dispatch doesn't really work how you'd like it to work using the
// implementation here, so we have to create this extra class and manually test
// for it in certain situations, rather than just letting the virtual functions
// implement it implicitly.
//

#ifndef _ADLTRANSLATINGWALKER_h_
#define _ADLTRANSLATINGWALKER_h_

#include "opencxx/Walker.h"

namespace adl {

class PtreeRegWrite;
class PtreeRegRead;
class PtreeMemWrite;
class PtreeMemRead;
class PtreeSubInstrCall;

struct AdlTranslatingWalker : public Opencxx::Walker {
  AdlTranslatingWalker(Opencxx::Parser *p) : Opencxx::Walker(p) {};
  AdlTranslatingWalker(Opencxx::Parser *p, Opencxx::Environment *e) : Opencxx::Walker(p,e) {};
  AdlTranslatingWalker(Opencxx::Environment *e) : Opencxx::Walker(e) {};
  AdlTranslatingWalker(Opencxx::Walker *w) : Opencxx::Walker(w) {};

  virtual Opencxx::Ptree *TranslateRegWrite(PtreeRegWrite *) = 0;
  virtual Opencxx::Ptree *TranslateRegRead(PtreeRegRead *) = 0;
  virtual Opencxx::Ptree *TranslateMemWrite(PtreeMemWrite *) = 0;
  virtual Opencxx::Ptree *TranslateMemRead(PtreeMemRead *) = 0;
  virtual Opencxx::Ptree *TranslateSubInstrCall(PtreeSubInstrCall *) = 0;
  virtual Opencxx::Ptree* TranslateStmts(Opencxx::Ptree*) = 0;
};

}

// Execute block if 'walker' is an AdlTranslatingWalker.  'w' is then a pointer
// to this object.
#define IfAdlTranslator(walker,w) \
  if (AdlTranslatingWalker *w = dynamic_cast<AdlTranslatingWalker*>(walker))


#endif

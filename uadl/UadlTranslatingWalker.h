//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Base class for a walker which translates uADL related code.  This exists due
// to limitations in how C++ can easily handle a visitor pattern, or at least
// how OpenC++ implements a visitor pattern.  The problem is that multiple
// dispatch doesn't really work how you'd like it to work using the
// implementation here, so we have to create this extra class and manually test
// for it in certain situations, rather than just letting the virtual functions
// implement it implicitly.
//

#ifndef _UADLTRANSLATINGWALKER_h_
#define _UADLTRANSLATINGWALKER_h_

#include "opencxx/Walker.h"

namespace uadl {

  class PtreeTransaction;

  struct UadlTranslatingWalker : public Opencxx::Walker {
    UadlTranslatingWalker(Opencxx::Parser *p) : Opencxx::Walker(p) {};
    UadlTranslatingWalker(Opencxx::Parser *p, Opencxx::Environment *e) : Opencxx::Walker(p,e) {};
    UadlTranslatingWalker(Opencxx::Environment *e) : Opencxx::Walker(e) {};
    UadlTranslatingWalker(Opencxx::Walker *w) : Opencxx::Walker(w) {};

    virtual Opencxx::Ptree *TranslateTransaction(PtreeTransaction *) = 0;

  };

}

// Execute block if 'walker' is a uAdlTranslatingWalker.  'w' is then a pointer
// to this object.
#define IfuAdlTranslator(walker,w) \
  if (UadlTranslatingWalker *w = dynamic_cast<UadlTranslatingWalker*>(walker))


#endif

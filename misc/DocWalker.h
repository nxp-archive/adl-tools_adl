//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// A documentation code transformer.
//

#ifndef _DOCWALKER_H_
#define _DOCWALKER_H_

#include "helpers/AdlTranslatingWalker.h"
#include "helpers/BasicTypes.h"
#include "helpers/gc_map.h"

#include "fe/Types.h"

#include "DocCfgParser.h"

namespace Opencxx {
  class Environment;
}

namespace adl {

  class DocCfg;
  class Core;

  // Class for making transformations on action code for documentation purposes.
  // If we encounter the special label "doc_op", then we store the label's body.
  // You can also do substitutions on variables by calling set_subst with an
  // array of (orig,subst) items.
  class DocWalker : public AdlTranslatingWalker {
  public:
    DocWalker(const Core &core,Opencxx::Environment *env) : 
      AdlTranslatingWalker(env),
      _core(core),
      _doc_op(0),
      _subst(0),
      _parms(0),
      _eval_expr(false),
      _has_ctrl(false)
    {
    }

    Opencxx::Environment *env() { return AdlTranslatingWalker::env; };
    
    void reset() { _doc_op = 0; _has_ctrl = false; };

    Opencxx::Ptree *doc_op() const { return _doc_op; };

    bool has_ctrl_stmts() const { return _has_ctrl; };

    void set_subst(PPairList *subst = 0) { _subst = subst; }; 

    void set_parms(IntMap *parms = 0) { _parms = parms; }; 

    virtual Opencxx::Ptree *TranslateLabel(Opencxx::Ptree* exp);
    virtual Opencxx::Ptree *TranslateRegWrite(PtreeRegWrite *exp);
    virtual Opencxx::Ptree *TranslateRegRead(PtreeRegRead *exp);
    virtual Opencxx::Ptree *TranslateMemWrite(PtreeMemWrite *exp);
    virtual Opencxx::Ptree *TranslateMemRead(PtreeMemRead *exp);
    virtual Opencxx::Ptree *TranslateSubInstrCall(PtreeSubInstrCall *exp);
    virtual Opencxx::Ptree *TranslateStmts(Opencxx::Ptree *exp);
    virtual Opencxx::Ptree *TranslateVariable(Opencxx::Ptree *exp);
    virtual Opencxx::Ptree *TranslateIf(Opencxx::Ptree* s);
    virtual Opencxx::Ptree* TranslateCond(Opencxx::Ptree*);
    virtual Opencxx::Ptree *TranslateParen(Opencxx::Ptree* exp);
    virtual Opencxx::Ptree *TranslateInfix(Opencxx::Ptree* exp);
    virtual Opencxx::Ptree *TranslateUnary(Opencxx::Ptree* exp);
    virtual Opencxx::Ptree *TranslateSwitch(Opencxx::Ptree* s);
    virtual Opencxx::Ptree* TranslateFor(Opencxx::Ptree* s);
    virtual Opencxx::Ptree* TranslateWhile(Opencxx::Ptree* s);
    virtual Opencxx::Ptree* TranslateDo(Opencxx::Ptree* s);
    virtual Opencxx::Ptree* TranslateBlock(Opencxx::Ptree* block);

    bool translate_const_expr(unsigned &res,Opencxx::Ptree *expr);

  protected:
    Opencxx::Ptree *attemptExprEval(Opencxx::Ptree *left,Opencxx::Ptree *op,Opencxx::Ptree *right);
    Opencxx::Ptree *attemptExprEval(Opencxx::Ptree *left,Opencxx::Ptree *op);

  private:
    const Core      &_core;
    Opencxx::Ptree  *_doc_op;
    const PPairList *_subst;
    const IntMap    *_parms;
    bool             _eval_expr;
    bool             _has_ctrl;
  }; 

}

#endif


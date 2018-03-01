//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// A documentation code transformer.
//

#include "opencxx/parser/PtreeDeclaration.h"
#include "opencxx/parser/PtreeBlock.h"
#include "opencxx/Environment.h"

#include "helpers/Macros.h"

#include "fe/Data.h"
#include "fe/Helpers.h"

#include "DocCfgParser.h"
#include "DocHelpers.h"
#include "DocWalker.h"

using namespace std;
using namespace Opencxx;
using namespace PtreeUtil;

namespace adl {

  // If we come across the special label "doc_op", then we store this block for
  // later use.  Otherwise, we ignore and continue.
  Ptree* DocWalker::TranslateLabel(Ptree* s)
  {
    Ptree* l   = First(s);
   
    Ptree* s2  = Walker::TranslateLabel(s);
    
    // If it's a block, then return what's inside of the block for expansion,
    // but store the entire block.
    Ptree *body = Third(s2);
    Ptree *body2 = body;
    if (dynamic_cast<PtreeBlock*>(body)) {
      body2 = Second(body);
    }

    // Special label, so store.
    if ( Eq(l,"doc_op") ) {
      _doc_op = body;
      return body2;
    }
    return s2;
  }

  Ptree *DocWalker::TranslateRegWrite(PtreeRegWrite *exp)
  {
    return Translate(exp->Car());
  }

  Ptree *DocWalker::TranslateRegRead(PtreeRegRead *exp)
  {
    return Translate(exp->Car());
  }

  Ptree *DocWalker::TranslateMemWrite(PtreeMemWrite *exp)
  {
    return Translate(exp->Car());
  }

  Ptree *DocWalker::TranslateMemRead(PtreeMemRead *exp)
  {
    return Translate(exp->Car());
  }

  Ptree *DocWalker::TranslateSubInstrCall(PtreeSubInstrCall *exp)
  {
    return Translate(exp->Car());
  }
  
  Ptree *DocWalker::TranslateStmts(Ptree* block)
  {
    Ptree *block2 = AdlTranslatingWalker::TranslateBrace(block);
    if (block != block2) {
      return new PtreeStmts(block2->Car(),block2->Cdr());
    } else {
      return block2;
    }
  }  

  bool dontNeedParens(Ptree *e)
  {
    return (e->IsA(ntParenExpr) || e->IsA(ntFuncallExpr) || e->IsLeaf());
  }

  Ptree *DocWalker::TranslateVariable(Ptree *exp)
  {
    if (_subst) {
      ForEach ( (*_subst), i ) {
        if ( Equal(exp,i->first)) {
          return i->second;
        }
      }
    }

    if (_parms) {
      IfFind(i,(*_parms),exp->ToString()) {
        return make_leaf<LeafUNSIGNED>(i->second);
      }
    }
    
    return exp;
  }


  // Attempt to evaluate a binary op.  If right is 0, then we assume a
  // unary op, with left holding the expression.
  Ptree *DocWalker::attemptExprEval(Ptree *left,Ptree *op,Ptree *right)
  {
    if (_eval_expr) {
      AttemptExprEval<unsigned> ae;

      return ae.eval(left,op,right);
    }
    return 0;
  }

  // Attempt to evaluate a unary op.
  Ptree *DocWalker::attemptExprEval(Ptree *left,Ptree *op)
  {
    if (_eval_expr) {
      AttemptExprEval<unsigned> ae;

      return ae.eval(left,op);
    }
    return 0;
  }

  // This will evaluate constant expressions if we are in that mode.  If so,
  // then we handle basic expressions.  If either leaf is an unsigned, we use
  // unsigned arithmetic, otherwise we use signed arithmetic.
  Ptree *DocWalker::TranslateInfix(Ptree* exp)
  {
    Ptree* left = PtreeUtil::First(exp);
    Ptree* left2 = Translate(left);
    Ptree* right = PtreeUtil::Third(exp);
    Ptree* right2 = Translate(right);
    Ptree* op = PtreeUtil::Second(exp);

    if (Ptree *const_result = attemptExprEval(left2,op,right2)) {
      return const_result;
    }

    return Walker::TranslateInfix(exp);
  }

  Ptree *DocWalker::TranslateUnary(Ptree* exp)
  {
    Ptree* oprnd = PtreeUtil::Second(exp);
    Ptree* oprnd2 = Translate(oprnd);
    Ptree* op = PtreeUtil::First(exp);

    if (Ptree *const_result = attemptExprEval(oprnd2,op)) {
      return const_result;
    }

    return Walker::TranslateUnary(exp);
  }

  // If we're evaluated expressions, then we strip the parentheses so that when
  // we process the values, they don't confuse us.  Otherwise, we pass on as-is.
  Ptree* DocWalker::TranslateParen(Ptree* exp)
  {
    if (_eval_expr) {
      Ptree* e = PtreeUtil::Second(exp);
      Ptree* e2 = Translate(e);
      return e2;
    } else {
      return AdlTranslatingWalker::TranslateParen(exp);
    }
  }

  bool DocWalker::translate_const_expr(unsigned &res,Ptree *expr)
  {
    // Try to see if we can statically evaluate the conditional.
    _eval_expr = true;
    Ptree *cond_result = Translate(expr);
    bool have_result = get_uint(res,cond_result,env());
    _eval_expr = false;
    return have_result;
  }

  Ptree* DocWalker::TranslateCond(Ptree* exp)
  {
    Ptree* cond = PtreeUtil::First(exp);

    unsigned res = 0;
    bool have_result = translate_const_expr(res,cond);

    bool discard_then = (have_result && !res);
    bool discard_else = (have_result && res);

    if (discard_else) {
      Ptree* then_part = PtreeUtil::Third(exp);
      Ptree* then_part2 = Translate(then_part);
      return then_part2;
    } else if (discard_then) {
      Ptree* else_part = PtreeUtil::Nth(exp,4);
      Ptree* else_part2 = Translate(else_part);
      return else_part2;
    } else {
      return Walker::TranslateCond(exp);
    }
  }

  // If we can evaluate the conditional, then we do and we only return the relevant portion
  // of the if (or else).
  Ptree* DocWalker::TranslateIf(Ptree* s)
  {
    Ptree* cond = PtreeUtil::Third(s);

    unsigned res = 0;
    bool have_result = translate_const_expr(res,cond);

    bool discard_then = (have_result && !res);
    bool discard_else = (have_result && res);

    if (discard_else) {
      Ptree* then_part = PtreeUtil::Nth(s,4);
      Ptree* then_part2 = Translate(then_part);
      return then_part2;
    } else if (discard_then) {
      Ptree* else_part = PtreeUtil::Nth(s,6);
      Ptree* else_part2 = Translate(else_part);
      return else_part2;
    } else {
      _has_ctrl = true;
      return Walker::TranslateIf(s);
    }
  }

  // If we can evaluate the expression to a constant, then we only return the
  // case value that matches.
  Ptree* DocWalker::TranslateSwitch(Ptree* s)
  {
    Ptree* cond = PtreeUtil::Third(s);

    unsigned res = 0;
    bool have_result = translate_const_expr(res,cond);

    Ptree *s2 = Walker::TranslateSwitch(s);

    if (have_result) {

      Ptree* body = PtreeUtil::Nth(s2,4);
      if (PtreeBlock *block = dynamic_cast<PtreeBlock*>(body)) {
        PtreeArray array;
        Ptree *b = PtreeUtil::Second(block);
        Ptree *r = b;
        bool add = false;
        while(r != 0){
          Ptree* p = r->Car();
          if (Ptree *cs = dynamic_cast<PtreeCaseStatement*>(p)) {
            Ptree *c = Second(cs);
            unsigned cv;
            if (get_uint(cv,c,env())) {
              if (cv == res) {
                add = true;
                Ptree* st = PtreeUtil::Nth(cs,3);
                array.Append(st);
              }
            }
          } else if (dynamic_cast<PtreeBreakStatement*>(p)) {
            add = false;
          } else if (add) {
            array.Append(p);
          }
          r = r->Cdr();
        }
        if (array.Number()) {
          return new PtreeBlock(First(block),array.All(),Third(block));
        }
      }
    }
    _has_ctrl = true;

    return s2;
  }

  Ptree* DocWalker::TranslateFor(Ptree* s)
  {
    _has_ctrl = true;
    return AdlTranslatingWalker::TranslateFor(s);
  }

  Ptree* DocWalker::TranslateWhile(Ptree* s)
  {
    _has_ctrl = true;
    return AdlTranslatingWalker::TranslateWhile(s);
  }

  Ptree* DocWalker::TranslateDo(Ptree* s)
  {
    _has_ctrl = true;
    return AdlTranslatingWalker::TranslateDo(s);
  }

  // When translating with expression translation, we want to make sure that we
  // squash empty statements, since those represent assignments and declarations
  // that have been merely stored away for future use.
  Ptree* DocWalker::TranslateBlock(Ptree* block)
  {
    Ptree* block2;

    NewScope();

    PtreeArray array;
    bool changed = false;
    Ptree* body = PtreeUtil::Second(block);
    Ptree* rest = body;
    while(rest != 0){
      Ptree* p = rest->Car();
      Ptree* q = Translate(p);
      // Ignore if the statement is empty or consists of just ";";
      if (! (!q || (!First(q) && Eq(Second(q),";")))) {
        array.Append(q);
      }
      if(p != q)
        changed = true;

      rest = rest->Cdr();
    }
    
    if(changed)
      block2 = new PtreeBlock(PtreeUtil::First(block), array.All(),
                              PtreeUtil::Third(block));
    else
      block2 = block;

    ExitScope();
    return block2;
  }

}

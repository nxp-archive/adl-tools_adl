//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Classes that allow to invert syntax into dsyntaxs
//

#include <iostream>
#include <iomanip>

#include "opencxx/parser/Leaf.h"
#include "opencxx/parser/DupLeaf.h"
#include "opencxx/parser/token-names.h"
#include "opencxx/parser/ptree-generated.h"
#include "opencxx/parser/Ptree.h"
#include "opencxx/parser/PtreeUtil.h"
#include "opencxx/parser/PtreeArray.h"
#include "opencxx/PtreeTypeUtil.h"
#include "helpers/Macros.h"
#include "VarWalker.h"
#include "Helpers.h"
#include "Data.h"
#include "SyntaxWalkers.h"

using namespace std;
using namespace Opencxx;
using namespace PtreeUtil;


namespace adl {
	
  Ptree* CondWalker::TranslateCond(Ptree* exp){
    _num_cond++;
    Ptree* c = PtreeUtil::First(exp);
    Ptree* c2 = Translate(c);
    Ptree* t = PtreeUtil::Third(exp);
    Ptree* t2 = Translate(t);
    Ptree* e = PtreeUtil::Nth(exp,4);
    Ptree* e2 = Translate(e);
    if (t == t2) {
      _exprs->Append(t);
    }
    if (e == e2) {
      _exprs->Append(e);
    }
    return new PtreeCondExpr(c2, PtreeUtil::List(PtreeUtil::Second(exp), t2,
                                                 PtreeUtil::Nth(exp,3), e2));				     
  }

  SubstWalker::SubstWalker(Opencxx::Environment* env,const PMap &renames) : 
    Opencxx::Walker(env), 
    _renames(renames) 
  {}

  SubstWalker::SubstWalker(Opencxx::Environment* env,const gc_string &from,Opencxx::Ptree *to) : 
    Opencxx::Walker(env)
  {
    add_rename(from,to);
  };

  SubstWalker::SubstWalker(Opencxx::Environment* env,const gc_string &from,const gc_string &to) : 
    Opencxx::Walker(env)
  {
    add_rename(from,to);
  };

  void SubstWalker::add_rename(const gc_string &from,Opencxx::Ptree *to)
  {
    _renames.insert(make_pair(from,to));
  }

  void SubstWalker::add_rename(const gc_string &from,const gc_string &to)
  {
    _renames.insert(make_pair(from,Ptree::Make(to.c_str())));
  }


  Ptree *SubstWalker::get_rename(const gc_string &n)
  {
    IfFind(ir,_renames,n) {
      return ir->second;
    }
    return 0;
  }

  Ptree* SubstWalker::TranslateVariable(Ptree *exp)
  {
    // Rename variable, if needed
    if (Ptree *p = get_rename(exp->ToString())) {
      return p;
    }

    return Walker::TranslateVariable(exp);
  }

  Ptree* SubstWalker::TranslatePtree(Opencxx::Ptree *var)
  {
    if (var->IsLeaf()) {
      if (Ptree *p = get_rename(var->ToString())) {
        return p;
      }
    } else {
      Ptree *car = Translate(var->Car());
      Ptree *cdr = Translate(var->Cdr());
      if (car != var->Car() || cdr != var->Cdr()) {
        return PtreeUtil::Cons(car,cdr);
      }
    }
    return Walker::TranslatePtree(var);
  }

  Ptree* SubstWalker::TranslateFuncall(Ptree *exp)
  {
    Ptree* fun = exp->Car();

    Ptree *replace = get_rename(fun->ToString());
    Ptree *fun2 = 0;
    if (replace) {
      fun2 = fun;
    } else {
      // We don't want to translate this if we have a rename, because then we'll
      // just get another renaming situation.
      fun2 = Translate(fun);    
    }

    Ptree* args = exp->Cdr();
    Ptree* args2 = TranslateArguments(args);

    Ptree *exp2 = 0;
    if(fun == fun2 && args == args2)
      exp2 = exp;
    else
      exp2 = new PtreeFuncallExpr(fun2, args2);

    if (replace) {
      replace->SetCar(exp2);
      return replace;
    } else {
      return exp2;
    }
  }
 
  Ptree* InvWalker::TranslateInfix(Ptree* exp) 
  {
    Ptree* left = PtreeUtil::First(exp);
    Ptree* left2 = Translate(left);
    Ptree* right = PtreeUtil::Third(exp);
    Ptree* right2 = Translate(right);
    Ptree* op = PtreeUtil::Second(exp);
    unsigned l;
    unsigned r;
    if ( Eq(op,"<<") ) {
      return new  PtreeInfixExpr(left2, PtreeUtil::List(strconst(">>"), right2));
    } 
    else if ( Eq(op,">>") ) {
      return new  PtreeInfixExpr(left2, PtreeUtil::List(strconst("<<"), right2));
    } 
    else if ( Eq(op,"+") ) {
      //check if both are constanst
      if (left2->Reify(l) && right2->Reify(r)) {	  
        return Ptree::Make("%d",l+r);
      }
      // 1+x => x-1 
      else if (left2->Reify(l)) {
        return new  PtreeInfixExpr(right2, PtreeUtil::List(strconst("-"), left2));
      }
      // x+1 => x-1 
      else if (right2->Reify(r)) {
        return new  PtreeInfixExpr(left2, PtreeUtil::List(strconst("-"), right2));
      }	
      else {
        return new  PtreeInfixExpr(left2, PtreeUtil::List(strconst("+"), right2));
      }
    }      
    else if ( Eq(op,"-") ) {
      //check if both are constant
      if (left2->Reify(l) && right2->Reify(r)) {	  
        return Ptree::Make("%d",l-r);
      }
      // 1-x => 1-x 
      else if (left2->Reify(l)) {
        return new  PtreeInfixExpr(left2, PtreeUtil::List(strconst("-"), right2));
      }
      // x-1 => x+1 
      else if (right2->Reify(r)) {
        return new  PtreeInfixExpr(left2, PtreeUtil::List(strconst("+"), right2));
      }	
      else {
        return new  PtreeInfixExpr(left2, PtreeUtil::List(strconst("-"), right2));
      }
    } 
    else {
      return new  PtreeInfixExpr(left2, PtreeUtil::List(PtreeUtil::Second(exp), right2));
    }
  }; 
  
  Ptree* InvWalker::TranslateUnary(Ptree* exp) 
  {
    //TODO check width of the field
    Ptree* op = PtreeUtil::First(exp);
    Ptree* oprnd = PtreeUtil::Second(exp);
    Ptree* oprnd2 = Translate(oprnd);
    if (Eq(op,"~")) {
      return new  PtreeInfixExpr(strconst("0xFFFFFFFF"),PtreeUtil::List(strconst("^"),oprnd2));
    } else {
      if(oprnd == oprnd2) {
        return exp;
      } else {
        return new PtreeUnaryExpr(PtreeUtil::First(exp), PtreeUtil::List(oprnd2));
      }
    }
  };

}

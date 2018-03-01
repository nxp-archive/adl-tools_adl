//
// A simple expression walker for evaluating numeric expressions.  This is
// templated on a numeric type, so that it supports type-appropriate arithmetic.
//

#ifndef _EXPR_WALKER_H_
#define _EXPR_WALKER_H_

#include "opencxx/parser/PtreeUtil.h"
#include "opencxx/Environment.h"
#include "opencxx/BindEnumName.h"
#include "opencxx/Walker.h"

#include "Types.h"

namespace adl {

  // Parse an expression and return a leaf object with the result, if possible.
  // Anything that can't be evaluated is just passed on.  By default, variables
  // are not handled, but this may be modified by subclassing.
  template <typename N>
  class ExprWalker : public Opencxx::Walker {
  public:
    ExprWalker(Opencxx::Environment *e) : Opencxx::Walker(e) {}

    // Strip the parentheses so as to not confuse the final reify step.
    Opencxx::Ptree* TranslateParen(Opencxx::Ptree* exp)
    {
      using namespace Opencxx;
      using namespace PtreeUtil;
      
      Ptree* e  = Second(exp);
      Ptree* e2 = Translate(e);
      return e2;
    }

    Opencxx::Ptree *TranslateUnary(Opencxx::Ptree *exp)
    {
      using namespace Opencxx;
      using namespace PtreeUtil;
      
      auto exp2 = Walker::TranslateUnary(exp);

      Ptree *op2 = First(exp2);
      Ptree *left2 = Second(exp2);

      AttemptExprEval<N> ae;
      if (Ptree *const_result = ae.eval(left2,op2)) {
        return const_result;
      }

      return exp2;
    }
  
    // This will evaluate constant expressions if we are in that mode (determined by the top value
    // in the constant-eval stack.  If so, then we handle basic expressions.  If either leaf is an
    // unsigned, we use unsigned arithmetic, otherwise we use signed arithmetic.
    Opencxx::Ptree *TranslateInfix(Opencxx::Ptree* exp)
    {
      using namespace Opencxx;
      using namespace PtreeUtil;
      
      Ptree* left   = First(exp);
      Ptree* left2  = Translate(left);
      Ptree* right  = Third(exp);
      Ptree* right2 = Translate(right);
      Ptree* op     = Second(exp);

      AttemptExprEval<N> ae;
      if (Ptree *const_result = ae.eval(left2,op,right2)) {
        return const_result;
      }

      Ptree *exp2;
      // This occurs if we did not successfully evaluate a constant expression.
      if(left == left2 && right == right2)
        exp2 = exp;
      else
        exp2 = new PtreeInfixExpr(left2,PtreeUtil::List(PtreeUtil::Second(exp), right2));
    
      return exp2;
    }

    // If we encounter a variable, then the only thing we can handle are enums.
    Opencxx::Ptree *TranslateVariable(Opencxx::Ptree *exp)
    {
      using namespace Opencxx;
      using namespace PtreeUtil;

      Bind *b = 0;
      if (env->Lookup(exp,b)) {
        if (auto be = dynamic_cast<BindEnumName*>(b)) {
          return Translate(be->GetSpecification());
        }
      }
      return exp;
    }
    
  };
  
}

#endif

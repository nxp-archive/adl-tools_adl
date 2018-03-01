//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//


#include <assert.h>
#include <iostream>
#include <vector>
#include <sstream>
#include <string.h>

#include "opencxx/parser/Leaf.h"
#include "opencxx/parser/token-names.h"
#include "opencxx/parser/PtreeDeclaration.h"
#include "opencxx/parser/PtreeTemplateDecl.h"
#include "opencxx/parser/ptree-generated.h"
#include "opencxx/parser/Encoding.h"
#include "opencxx/Environment.h"
#include "opencxx/Bind.h"
#include "opencxx/TypeInfo.h"
#include "opencxx/BindVarName.h"
#include "opencxx/PtreeIter.h"

#include "fe/Helpers.h"

#include "BasicTypes.h"
#include "RtlWalker.h"
#include "Macros.h"
#include "fe_exceptions.h"

using namespace std;
using namespace Opencxx;
using namespace PtreeUtil;

namespace adl {

  // This enum should specify the maximum number of arguments we support for
  // intbv concat operations.
  enum { MaxIntbvConcatArgs = 6 };

  void PtreeBitSlice::Typeof(AbstractTypingWalker *w, TypeInfo &ti)
  {
    ti.Set(_type,_env);
  }

  // Tries to find a leaf in the given tree, returns 0 if not found.
  Ptree *find_leaf(Ptree *p)
  {
    if (!p) {
      return 0;
    } else if (p->IsLeaf()) {
      return p;
    } else if (Ptree *l = find_leaf(p->Car())) {
      return l;
    } else {
      return find_leaf(p->Cdr());
    }
  }

  // Given an environment and a ptree item, returns a filename and line number.
  // This will only work correctly if you give it a chunk of code from the original
  // program- it dives down until it finds the first leaf, then it uses that to get
  // the information.
  Ptree *getLineNumber(Environment *env,Ptree *expr,int &line)
  {
    Ptree *e = find_leaf(expr);
    if (e) {
      return env->GetLineNumber(e,line);
    } else {
      line = 0;
      return Ptree::Make("unknown");
    }
  }

  // Writes a line directive with the program location pointed to by expr.
  // Note:  Expr must come from the original program for this to work correctly.
  void lineDirective(ostream &o,Environment *env,Ptree *expr)
  {
    int line;
    Ptree *fn = getLineNumber(env,expr,line);
    o << "# " << line << " \"" << fn << "\"\n";
  }

  // Returns a line directive with the program location pointed to by expr.
  // Note:  Expr must come from the original program for this to work correctly.
  Ptree *lineDirective(Environment *env,Ptree *expr)
  {
    int line;
    Ptree *fn = getLineNumber(env,expr,line);
    Ptree *p = Ptree::Make("# %d \"%p\"\n",line,fn);
    char *str = p->ToString();
    return new Leaf(str,strlen(str));
  }

  // Given a list and an object, appends the object to the list and returns
  // a pointer to the new end-of-list object.
  inline Ptree *lappend(Ptree *l,Ptree *x)
  {
    return PtreeUtil::Last(PtreeUtil::Snoc(l,x));
  }

  RtlWalker::RtlWalker(Environment *e,bool ld_okay,bool lib_mode) : 
    AdlTranslatingWalker(e),
    _ld_okay(ld_okay),
    _lib_mode(lib_mode),
    _translating_var_decl(false)
  {
  }

  Ptree *RtlWalker::translateCode(Environment *e,Ptree *p)
  {
    NewScope(e);
    Ptree *result = Translate(p);
    ExitScope();
    return result;
  }

  // Translate a "block" statement.  We basically just convert it to 
  // a brace-delimited block.
  Ptree *RtlWalker::translateBlockStmt(Ptree *exp)
  {
    return Nth(exp,3);
  }

  // We just return w/o translating, but we do process
  // the parameters in order to register them with the environment.
  Ptree *RtlWalker::translateFunc(Ptree *exp)
  {
    Ptree *args = Second(exp);
    TranslateArgDeclList2(true,env,false,false,0,args);
    Ptree *body = Nth(exp,3);
    return Translate(body);
  }

  // The base class is broken here:  It doesn't translate the body of the
  // function.  So, this version does.
  Ptree* RtlWalker::TranslateTemplateFunction(Ptree* temp_def, Ptree* fun)
  {
    Walker::TranslateTemplateFunction(temp_def,fun);
    Ptree *result = TranslateDeclaration(fun);
    if (result != fun) {
      return new PtreeTemplateDecl(temp_def->Car(),ShallowSubst(result,fun,temp_def->Cdr()));
    } else {
      return temp_def;
    }
  }

  // This has to understand func blocks b/c their form
  // is not standard C++.  Otherwise, we ignore the
  // code.
  Ptree* RtlWalker::TranslateUserPlain(Ptree*exp)
  {
    TypeInfo type;
    Ptree* keyword = First(exp);
    Ptree* rest = ListTail(exp, 1);

    Ptree *nexp = 0;

    NewScope();
    if (Eq(keyword,"func")) {
      nexp = translateFunc(rest);
    } else if (Eq(keyword,"block")) {
      nexp = translateBlockStmt(rest);
    }
    ExitScope();

    return nexp;
  }

  // We intercept variable declarations and transform "var" statements
  // into "typeof".  We add the type of the new variable to the environment
  // so that typing will work for this variable.
  Ptree* RtlWalker::TranslateDeclaration(Ptree* def)
  {
    Ptree* tspec = PtreeUtil::Second(def);

    if (Eq(tspec,"var")) {
      Ptree* decls = PtreeUtil::Third(def);
      Ptree *var,*rhs;
      Ptree *tspec2 = 0;
      if (Match(decls,"[[%? = %?]]",&var,&rhs)) {
        _translating_var_decl = true;
        Ptree *new_rhs = Translate(rhs);
        _translating_var_decl = false;
        const char *n = var->ToString();
        TypeInfo ti;
        Typeof(new_rhs,ti);
        unsigned s,o;
        const char *t;
        bool dummy;
        if (get_bits_size(ti,s,o,dummy)) {
          // For the bits case, we remove the offset, since that's not something that
          // makes sense to propagate.
          TypeInfo nti;
          generateBitsEncoding(nti,s,0);
          t = nti.Encoding();
        } else {
          // Normal case- propagate the type w/o modification.
          t = ti.Encoding();
        }
        env->AddDupEntry((char*)n,strlen(n),new BindVarName((char*)t));
        tspec2 = Ptree::Make("AdlTypeof(%p)",new_rhs);      
      } else {
        PError(def,"Bad var statement:  An initialization value must be present.");
      }
      Ptree *newdecl = new PtreeDeclaration(First(def),
                                            List(tspec2,decls,new Leaf(";", 1)));
      return Walker::TranslateDeclaration(newdecl);
    } else {
      return Walker::TranslateDeclaration(def);
    }
  }

  void RtlWalker::TypeofVariable(Ptree* exp, TypeInfo& t)
  {
    // If this is a bits object, return its type directly, else continue the query.
    if (!exp->IsLeaf() && (Eq(First(exp),"bits") || Eq(First(exp),"sbits"))) {
      t.Set(exp->GetEncodedName(),env);
      t.Reference(); // see TypeofFuncall
      t.Reference();
      return;
    }

    bool is_type_name;
  
    if(env->Lookup(exp, is_type_name, t))
      if(is_type_name)	// if exp is a class name
        t.Reference();	// see TypeofFuncall
  }


  // If both sides are bits objects of different sizes, then we manipulate the
  // smaller of the two sides so that both sides are the same.  This is required
  // by C++, which demands that both sides of a ternary operator return the same
  // type.  This function exists so that a parent class can manipulate the
  // conditional but still get the expected processing on the then and else
  // clauses.
  Ptree* RtlWalker::translate_ternary(Ptree *exp,Ptree *c,Ptree *c2)
  {
    Ptree* t  = Third(exp);
    Ptree* t2 = Translate(t);
    Ptree* e  = Nth(exp,4);
    Ptree* e2 = Translate(e);

    unsigned s1 = 0,s2 = 0,o1 = 0,o2 = 0;
    bool b1,b2; // true if signed
    if (get_bits_size(t2,s1,o2,b1) && get_bits_size(e2,s2,o2,b2)) {
      char type[2] = "";
      if (s1 < s2) {
        if(b1) {
          strcpy(type,"s");
        }
        Ptree *tmp = make_bitslice(Ptree::Make("%sbits<%d>(%p)",type,s2,t2),s2,o1);
        t2 = tmp;
      } else if (s1 > s2) {
        if(b2) {
          strcpy(type,"s");
        }      
        Ptree *tmp = make_bitslice(Ptree::Make("%sbits<%d>(%p)",type,s1,e2),s1,o2);
        e2 = tmp;
      }
    }

    if(c == c2 && t == t2 && e == e2)
      return exp;
    else
      return new PtreeCondExpr(c2, PtreeUtil::List(PtreeUtil::Second(exp), t2,
                                                   PtreeUtil::Nth(exp,3), e2));
  }


  Ptree* RtlWalker::TranslateCond(Ptree *exp)
  {
    Ptree* c  = First(exp);
    Ptree* c2 = Translate(c);

    return translate_ternary(exp,c,c2);
  }

  // This translates assignments to intbv objects into intbv::set() calls.
  Ptree *RtlWalker::TranslateAssign(Ptree *exp)
  {
    
    Ptree *left = First(exp);
    Ptree *left2 = 0;
    Ptree* right = Third(exp);
    Ptree* right2 = Translate(right);

    Ptree *func,*args;
    if (Match(left," [ %? ( %? ) ] ",&func,&args)) {
      unsigned size,offset;
      bool dummy;
      if (get_bits_size(func,size,offset,dummy)) {
        TypeInfo t;
        Typeof(func,t);
        // FIXME:  Should we sorry about offsets anymore?
        Ptree *args2 = Translate(args);
        return make_bitslice(Ptree::Make("%p.set( %p , ( %p ) )",func,args2,right2),t);
      }
    }

    if (!left2) {
      left2 = Translate(left);
    }
    if(left == left2 && right == right2)
      return exp;
    else
      return new PtreeAssignExpr(left2, PtreeUtil::List(PtreeUtil::Second(exp), right2));
  }

  // Helper function used for breaking up intbv argument lists which are too long.
  Ptree *breakup_concat_args(Ptree *args,unsigned argcount)
  {
    PtreeArray array;

    // If we have only n arguments left, then we can exit.  Otherwise, take n-1
    // arguments, where n is the max number of arguments we allow to concat.
    // These, plus the separating commas, are added to the array.  We then append
    // the final argument, which is a call to concat for everything else.
    if (argcount == adl::MaxIntbvConcatArgs) {
      return args;
    }

    Ptree *cur = args;
    unsigned i = 0;
    while(i < (adl::MaxIntbvConcatArgs-1) && cur != 0){
      Ptree* p = cur->Car();
      array.Append(p);    
      cur = cur->Cdr();
      ++i;
      if(cur != 0){
        array.Append(cur->Car());
        cur = cur->Cdr();
      }
    }
    if (cur) {
      array.Append(new PtreeFuncallExpr(Ptree::Make("concat"),
                                        List(Ptree::Make("("),
                                             breakup_concat_args(cur,argcount-i),
                                             Ptree::Make(")"))));
    }
    return array.All();
  }

  // Process a concat call- we just want to propagate the width of the result.  If
  // the number of arguments is greater than the maximum supported by intbv, then
  // we have to break up the argument list into multiple concat calls.
  Ptree *RtlWalker::genConcat(Ptree *exp,Ptree *arglist,Ptree *orig_arglist)
  {
    int width = 0;
    bool all_handled = true;
    Ptree *args = Second(arglist);
    unsigned argcount = 0;
    while (args != 0) {
      Ptree *p = args->Car();
      ++argcount;

      unsigned sz;
      unsigned of;
      bool is;
      if (get_bits_size(p,sz,of,is) && all_handled) {
        width += sz;
      } else {
        all_handled = false;
      }
      args = args->Cdr();
      if (args != 0) {
        args = args->Cdr();
      }
    }

    // Is the argument count greater than what we support?  If so, we must break
    // up the function into multiple calls.
    if (argcount > adl::MaxIntbvConcatArgs) {
      arglist = List(First(arglist),breakup_concat_args(Second(arglist),argcount),Third(arglist));
    }

    Ptree* fun = exp->Car();
    Ptree* exp2;
    if(arglist == orig_arglist) {
      exp2 = exp;
    } else {
      exp2 = new PtreeFuncallExpr(fun, arglist);
    }

    if (all_handled) {
      return make_bitslice(exp2,width);
    } else {
      // Couldn't figure out argument size.
      return exp2;
    }
  }

  Ptree *RtlWalker::translated_arglist(Ptree *args)
  {
    return Second(TranslateArguments(args));
  }

  // This translates function calls on bits objects to templated gets so that the
  // return type is sized to the slice being made.
  Ptree *RtlWalker::TranslateFuncall(Ptree *exp)
  {
    Ptree* fun = exp->Car();

    Ptree* fun2 = Translate(fun);

    Ptree* args = exp->Cdr();

    if (Eq(fun,"concat")) {
      // We track the resulting bit-width of the result and we split up this
      // operation, since we only support a finite number of arguments in intbv.
      return genConcat(exp,TranslateArguments(args),args);
    } else if (Eq(fun,"zero")) {
      // Convert to the templated function form.
      Ptree *arg1 = First(translated_arglist(args));
      return make_bitslice(Ptree::Make("zero<%p>()",arg1),get_uint(arg1,"zero-function return size",env));
    }
    else if (Eq(fun,"signExtend")) {
      // Convert to the templated member form.
      Ptree *args2 = translated_arglist(args);
      Ptree *arg1 = First(args2);
      Ptree *arg2 = Third(args2);
      return make_bitslice(Ptree::Make("%p.signExtend<%p>()",arg1,arg2),get_uint(arg2,"sign-extension return size",env));
    }
    else if (Eq(fun,"zeroExtend")) {
      // Convert to a simple bits constructor call.
      Ptree *args2 = translated_arglist(args);
      Ptree *arg1 = First(args2);
      Ptree *arg2 = Third(args2);
      return make_bitslice(Ptree::Make("bits<%p>(%p)",arg2,arg1),get_uint(arg2,"zero-extension return size",env));
    }

    Ptree* args2 = TranslateArguments(args);

    // Do we have a bits object?  If so, convert to a templated slice.
    // Note that we create a PtreeBitsSlice object so that typing will work even with
    // chained calls.
    unsigned size,offset;
    bool dummy;
    if (get_bits_size(fun2,size,offset,dummy)) {
      Ptree *args = Third(exp);
      Ptree *oarg1 = First(args);
      Ptree *oarg2 = Third(args);
      Ptree *arg1 = Translate(oarg1);
      Ptree *arg2 = Translate(oarg2);
      unsigned s,e;
      TypeInfo t;
      Typeof(fun2,t);
      string off_str;
      // If this object has an offset, then it indicates that we have something
      // like a register, where its leading index can be non-zero.  We must adjust
      // our slice indices here accordingly b/c we want the user to be able to
      // index into the register using indices relative to the offset.
      if (offset) {
        MkStr(off_str,"-" << offset);
      }
      if (arg1) {
        if (get_uint(s,arg1,env) && arg2 && get_uint(e,arg2,env)) {
          // Arguments are constant, so we want to return a properly sized slize.
          unsigned w = e > s?(e-s+1):(s-e+1);
          // If in library mode, use the .template disambiguator in order to
          // avoid weird compiler errors due to the ISS being a template itself.
          return make_bitslice(Ptree::Make("%p.%sget<%d>(%d%s,%d%s)",fun2,((_lib_mode) ? "template " : ""),w,s,
                                           off_str.c_str(),e,off_str.c_str()),w);
        } else if (offset) {
          // Arguments are not constant.  If we have an offset, then we adjust the arguments by that,
          // but we can't modify the return type.
          return Subst(fun2,fun,Ptree::Make("((%p)%s)",arg1,off_str.c_str()),oarg1,
                       Ptree::Make("((%p)%s)",arg2,off_str.c_str()),oarg2,exp);
        }
      }
    }

    Ptree *exp2;
    if(fun == fun2 && args == args2) {
      exp2 = exp;
    } else {
      exp2 = new PtreeFuncallExpr(fun2, args2);
    }
    return exp2;
  }

  // Returns the size of a bits object, or false if it's not a bits object.
  bool RtlWalker::get_bits_size(Opencxx::Ptree *p,unsigned &size,unsigned &offset,bool &is_signed)
  {
    size = 0;
    offset = 0;
    TypeInfo t;
    Typeof(p,t);
    return get_bits_size(t,size,offset,is_signed);
  }

  bool RtlWalker::get_bits_size(TypeInfo &t,unsigned &size,unsigned &offset,bool &is_signed)
  {
    while (t.IsReferenceType()) {
      t.Dereference();
    }
    try {
      // There are some types that OpenC++ can't handle right now, such as
      // template expressions.  In that case, it throws a fatal error.  So, we
      // just catch it and report that we can't figure out the size (which is
      // true anyway).
      if (t.IsTemplateClass() && (Eq(t.FullTypeName(),"bits") || Eq(t.FullTypeName(),"sbits"))) {
        if (Eq(t.FullTypeName(),"sbits")) {
          is_signed = true;
        } else {
          is_signed = false;
        }
	
        TypeInfo at0,at1;
        t.NthTemplateArgument(0,at0);
        t.NthTemplateArgument(1,at1);
        if (Ptree *p = at1.MakePtree()) {
          get_uint(offset,p->Car(),env);
        }
        if (Ptree *p = at0.MakePtree()) {
          get_uint(size,p->Car(),env);
        }
        return true;
      }
      return false;
    }
    catch (...) {
      return false;
    }
  }

  PtreeBitSlice *RtlWalker::make_bitslice(Ptree *x,const TypeInfo &t)
  {
    return new PtreeBitSlice((char*)t.Encoding(),x,env);
  }

  PtreeBitSlice *RtlWalker::make_bitslice(Ptree *x,unsigned size,unsigned offset)
  {
    TypeInfo t;
    generateBitsEncoding(t,size,offset);
    return new PtreeBitSlice((char*)t.Encoding(),x,env);
  }

  void RtlWalker::generateBitsEncoding(TypeInfo &t,unsigned size,unsigned offset)
  {
    Encoding args;
    args.ValueParam(size);
    args.ValueParam(offset);
    Encoding bits;
    bits.Template(Ptree::Make("bits"),args);
    t.Set(bits.Get(),env);
  }

}

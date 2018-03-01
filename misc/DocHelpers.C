//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Various helper functions used by the database and documentation generator.
//

#include <stdexcept>

#include "opencxx/parser/PtreeArray.h"
#include "opencxx/parser/LeafName.h"
#include "opencxx/parser/MopMsg.h"
#include "opencxx/parser/TheErrorLog.h"
#include "opencxx/parser/ErrorLog.h"
#include "opencxx/parser/DupLeaf.h"

#include "rnumber/RNumber.h"

#include "helpers/Macros.h"

#include "fe/Helpers.h"
#include "fe/Data.h"

#include "DocHelpers.h"
#include "DocWalker.h"

using namespace std;
using namespace Opencxx;
using namespace rnumber;

namespace adl {

  // Just to maintain consistant typing.
  Ptree* PtreeBlockNB::Translate(AbstractTranslatingWalker* w)
  {
    Ptree *p2 = w->TranslateBlock(this);
    if (this != p2) {
      return new PtreeBlockNB(dynamic_cast<PtreeBlock*>(p2));
    } else {
      return p2;
    }
  }
  

  // Same printing routine as for PtreeBrace, except doesn't bring the actual braces.
  int PtreeBlockNB::Write(std::ostream& out, int indent)
  {
    int n = 0;

    Ptree* p = this->Cdr()->Car();
    while(p != 0){
      if(p->IsLeaf()){
        TheErrorLog().Report(
                             MopMsg(Msg::Fatal, "PtreeBlockNB::Write()", "non list"));
        break;
      }
      else{
        PrintIndent(out, indent);
        ++n;
        Ptree* q = p->Car();
        p = p->Cdr();
        if(q != 0)
          n += q->Write(out, indent + 1);
      }
    }
    ++n;
    return n;
  }


  void subtractBitVect(IntSet &r,const BitVect &bv,int offset,bool le)
  {
    int count = (le) ? offset+bv.size()-1 : offset;
    ForEach(bv,i) {
      if (*i) {
        r.erase(count);
      }
      count = (le) ? (count-1) : (count+1);
    }
  }

  IntSet mask2set(const RNumber &mask,int offset,bool le)
  {
    IntSet result;
    for (unsigned b = 0; b < mask.size(); ++b) {
      unsigned const bitValue = (le) ? mask.getBitLSB(b) : mask.getBit(b);
      if (bitValue) {
        result.insert(offset+b);
      }
    }
    return result;
  }

  // Return the difference between the specified set of inclusive
  // bit ranges and the given bit mask, assuming that the bits in
  // the ranges would all be set to 1.  If "le" is true, the mask
  // is considered little-endian.
  // difference:  Bits of the field not covered by the mask.  If empty,
  //              then the mask completely coveres the field.
  // remainder:   Bits of the mask not used by the field.
  static void subtractMaskFromBitRanges(IntSet &difference,IntSet &remainder,unsigned int offset,const RNumber &mask,
                                        const BitPairVect &bpv,bool le)
  {
    IntSet vectorBitNumbers;
    IntSet maskBitNumbers;
    ForEach(bpv,pair) {
      unsigned const l = std::min(pair->l(), pair->r());
      unsigned const r = std::max(pair->l(), pair->r());
      for (unsigned i = l; i <= r; ++i) {
        vectorBitNumbers.insert(i);
      }
    }
    maskBitNumbers = mask2set(mask,offset,le);
#   if 0
    // perhaps useful for debugging...
    std::cerr << "[for mask " << mask << " (size " << mask.size() << ")]\n";
    std::cerr << " vector bits: ";
    std::copy(vectorBitNumbers.begin(),vectorBitNumbers.end(),std::ostream_iterator<unsigned int>(std::cerr," "));
    std::cerr << "\n mask bits: ";
    std::copy(maskBitNumbers.begin(),maskBitNumbers.end(),std::ostream_iterator<unsigned int>(std::cerr," "));
    std::cerr << "\n";
#   endif
    // This makes sure that all bits of the field are covered by the mask.
    set_difference(vectorBitNumbers.begin(),vectorBitNumbers.end(),
                   maskBitNumbers.begin(),maskBitNumbers.end(),
                   inserter(difference,difference.begin()));
    // This tracks which bits of the mask were used by the field.
    if (difference.empty()) {
      IntSet tmp;
      set_difference(remainder.begin(),remainder.end(),
                     vectorBitNumbers.begin(),vectorBitNumbers.end(),
                     inserter(tmp,tmp.begin()));
      remainder = tmp;
    }
  }
  
  // Return true only if the specified set of bit ranges is equal to
  // the given bit mask, assuming that the ranges would all be set to 1.
  // FIXME - move this, it's generic and might be more useful elsewhere.
  static bool bitRangesMatchMask(IntSet &remainder,const BitPairVect &bpv,const rnumber::RNumber &mask,unsigned int offset,bool le)
  {
    IntSet difference;
    subtractMaskFromBitRanges(difference,remainder,offset,mask,bpv,le);
    bool result = difference.empty();
    return result;
  }

# if 0
  // Test of bitRangesMatchMask().  DEBUG ONLY
  void _testBitRangesMatchMask()
  {
    BitPairVect testRanges;
    // test basic mask equivalence
    testRanges.push_back(BitPair(0, 1));
    testRanges.push_back(BitPair(5, 7));
    testRanges.push_back(BitPair(10, 10));
    rnumber::RNumber maskLE(0,64);
    rnumber::RNumber mask(0,64);
    maskLE.setBitLSB(0,1);
    maskLE.setBitLSB(1,1);
    maskLE.setBitLSB(5,1);
    maskLE.setBitLSB(6,1);
    maskLE.setBitLSB(7,1);
    maskLE.setBitLSB(10,1);
    mask.setBit(0,1);
    mask.setBit(1,1);
    mask.setBit(5,1);
    mask.setBit(6,1);
    mask.setBit(7,1);
    mask.setBit(10,1);
    assert(bitRangesMatchMask(testRanges,maskLE,0,true));
    assert(false == bitRangesMatchMask(testRanges,maskLE,0,false));
    assert(bitRangesMatchMask(testRanges,mask,0,false));
    // test of wider fields
    testRanges.push_back(BitPair(60, 59));
    maskLE.setBitLSB(60,1);
    maskLE.setBitLSB(59,1);
    mask.setBit(60,1);
    mask.setBit(59,1);
    assert(bitRangesMatchMask(testRanges,maskLE,0,true));
    assert(false == bitRangesMatchMask(testRanges,maskLE,0,false));
    assert(bitRangesMatchMask(testRanges,mask,0,false));
  }
# endif

  bool is_hidden(const gc_hash_set<const Named*> *hidden_items,const Named &item)
  {
    return (hidden_items && (hidden_items->find(&item) != hidden_items->end()));
  }

  bool getUsedFields(StrVect &fieldNames,const UsedReg &ur,bool little_endian,
                     const gc_hash_set<const Named*> *hidden_items)
  {
    auto reg = ur.regb();

    if (!reg || ur._mask.iszero() || reg->fields().empty()) return false;

    // Create a set representing all bits of the mask.  We must completely cover
    // this in order to print fields.
    IntSet remainder = mask2set(ur._mask,reg->offset(),little_endian);
    // Then remove the reserved mask, as these count as don't-cares.
    subtractBitVect(remainder,reg->reserved_mask(),reg->offset(),little_endian);

    // Then we sort by size of field so that we'll encounter larger fields
    // first.  This way, we should cover the used bits by using the least number
    // of fields.
    gc_vector< RegField* > usedFields;
    gc_vector< RegField* > biggestFields;
    ForEach(reg->fields(),iter) {
      biggestFields.push_back(iter->second);
    }
    std::sort(biggestFields.begin(),biggestFields.end(),BitsSizeSort<RegField>());
    ForEach(biggestFields,iter) {
      auto field = *iter;
      if (bitRangesMatchMask(remainder,field->bits(),ur._mask,
                             reg->offset(),little_endian) &&
          !is_hidden(hidden_items,*field)) {
        usedFields.push_back(*iter);
      }
      if (remainder.empty()) {
        break;
      }
    }

    if (!remainder.empty() || usedFields.empty()) {
      // We get to here if no fields match (an explicit slice was accessed on a
      // register w/no fields, or else we couldn't get a complete covering field
      // (a slice was accessed that didn't match some set of fields).
      return false;
    } else {
      // We found fields, so add them.
      ForEach(usedFields,toField) {
        fieldNames.push_back((*toField)->name());
      }
      return true;
    }
  }

  // Transform an expression based upon fields and parameters from an alias.
  Ptree* replaceAliasParms(Ptree *exp,DocWalker &walker,const Instr::Alias *alias)
  {
    if (!alias) return exp;

    PPairList subst;
    IntMap parms;

    getAliasSubstitutions(subst,parms,alias);

    walker.set_parms(&parms);    
    walker.set_subst(&subst);
    Ptree *exp2 = walker.Translate(exp);
    walker.set_subst();
    walker.set_parms();    

    return exp2;
  }

  // If given a block, then this returns a non-brace-printing block.
  Ptree* extractBlockBody(Ptree *ex)
  {
    if (auto pb = dynamic_cast<PtreeBlock*>(ex)) {
      return new PtreeBlockNB(pb);
    } else {
      return ex;
    }
  }

  // This class implements substAllEqual.
  struct SubstObj {
    typedef gc_map<Ptree *,Ptree *,PtreeCmp> TokenMap;
    typedef vector<TokenMap> TokenStack;

    SubstObj(bool expand_expr,const PPairList &subst,DocWalker *walker) : 
      _expand_expr(expand_expr),
      _subst_ocrd(false),
      _subst(subst), 
      _walker(walker),
      _vars(1)
    {};

    bool is_capture(Ptree *p) {
      return p && p->IsLeaf() && (*p->GetPosition() == '$');
    }

    // Same as PtreeUtil::Equal, except that $ tokens found in q imply that the
    // equivalent portion of p is captured into _captures.
    bool equal(Ptree* p, Ptree* q)
    {
      if(p == q) {
        return true;
      } else if (p == 0 || q == 0) {
        return false;
      } else if (is_capture(q)) {
        // Found a capture, so store it and return.  This is always considered a
        // match, no matter what p is.
        _captures[q] = p;
        return true;
      } else if (!q->IsLeaf() && is_capture(q->Car()) && PtreeUtil::Second(q)->IsA(ntDeclarator)) {
        // Weired parsing case- sometimes a simple $x is stored as [$x [0]],
        // such as for templated casts, so we treat that as if it were just $x.
        _captures[q->Car()] = p;
        return true;
      } else if (p->IsLeaf() && p->GetPosition()[0] == '0' && p->GetPosition()[1] == 'x') {
        // p is a number, so reify both and compare values.
        unsigned x,y;
        return (get_uint(x,p,_walker->env()) && get_uint(y,q,_walker->env()) && (x == y));
      } else if(p->IsLeaf() || q->IsLeaf()) {
        return PtreeUtil::Eq(p, q);
      } else {
        return equal(p->Car(), q->Car()) && equal(p->Cdr(), q->Cdr());
      }
    }

    Ptree *get_capture(Ptree *p)
    {
      Ptree *q = 0;
      IfFind(x,_captures,p) {
        q = translate(x->second);
      }
      return q;
    }

    Ptree *get_value(PtreeArray &args,unsigned index)
    {
      Ptree *p = 0;
      if (index < args.Number()) {
        p = args[index];
      }
      return subst(p);
    }

    // To find a variable value, we crawl up the stack, looking for items in the
    // maps.  Note that this returns a pointer to the pair in the map, so that
    // the variable's value can be updated, as well as retrieved.
    TokenMap::value_type *find_var(Ptree *v)
    {
      for (auto iter = _vars.rbegin(); iter != _vars.rend(); ++iter) {
        IfFind(x,(*iter),v) {
          return &*x;
        }
      }
      return 0;
    }

    // Try to evaluate as a numerical expression, return the original expression
    // if that doesn't work.  This just evaluates opereators and numeric
    // literals.
    Ptree *evalExpr(Ptree *exp)
    {
      Ptree *e = PtreeUtil::First(PtreeUtil::Third(exp));

      if (!e) {
        RError("The _eval_ function takes a single argument:  The expression to evaluate.");
      }

      if (_walker) {
        unsigned val;
        if (_walker->translate_const_expr(val,subst(e))) {
          return Ptree::Make("%d",val);
        }
      }

      return e;
    }

    // Performs string creation.  Takes the same basic format as Ptree::Make but
    // currently just supports %p as a format code.
    Ptree *createStr(Ptree *exp)
    {
      const unsigned N = 4096;
      char buf[N];

      Ptree *a = PtreeUtil::Third(exp);
      PtreeArray args;

      while (a != 0) {
        Ptree *p = a->Car();
        args.Append(p);
        a = a->Cdr();
        if (a != 0) {
          a = a->Cdr();
        }
      }      
      
      // The create-string function, _str_, returns a new token (a string
      // w/no quotes).  It expects its first argument to be a format string and subsequent
      // arguments to be expressions for insertion.  The formats it can handle are:
      //
      // %%:  Print a %
      // %p:  Expand an expression, keeping it as an expression.
      // %s:  Expand an expression as a string, inlining the string.
      if (args.Number() == 0 || !args[0]->IsLeaf()) {
        RError("Expected a string constant as the first argument for the _str_ function.");
      }

      // Note:  Iteration skips the quotes in the fmt string.
      const char *fmt = args[0]->GetPosition() + 1;
      const char *end = fmt + args[0]->GetLength() - 2;
      unsigned j = 0, a_iter = 1;
      Ptree *result = 0;

      while(fmt != end) {
        char c = *fmt++;
        if(c == '%') {
          c = *fmt++;
          if (c == '%') {
            buf[j++] = c;
          } else if (c == 's') {
            // Insert as a string.
            Ptree *p = get_value(args,a_iter++);
            if (p) {
              if (p->IsLeaf()) {
                memmove(&buf[j], p->GetPosition(), p->GetLength());
                j += p->GetLength();
              } else {
                ostringstream ss;
                p->PrintOn(ss);
                // Hmm...  Looks like maybe the string is a temporary, so
                // grabbing a pointer to it can result in bad data in newer
                // versions of gcc.
                string str = ss.str();
                const char *cstr = str.c_str();
                unsigned l = str.size();
                memmove(&buf[j],cstr,l);
                j += l;
              }
            }
          } else if (c == 'p') {
            // Insert, keeping it as a ptree object.
            Ptree *p = get_value(args,a_iter++);
            if (p) {
              if (p->IsLeaf()) {
                memmove(&buf[j], p->GetPosition(), p->GetLength());
                j += p->GetLength();
              } else {
                Ptree *q = 0;
                if(j > 0)
                  q = PtreeUtil::List(new DupLeaf(buf, j), p);
                else
                  q = PtreeUtil::List(p);
                
                j = 0;
                result = PtreeUtil::Nconc(result, q);                
              }
            }
          } else {
            RError ("Unknown format code %" << c);
          }         
        } else {
          buf[j++] = c;
        }
      }

      assert(j < N);

      if(j > 0) {
        if(result == 0) {
          result = new DupLeaf(buf, j);
        } else {
          result = PtreeUtil::Snoc(result, new DupLeaf(buf, j));
        }
      }

      return result;
    }

    // This walks the argument tree, looking for substitution tokens and
    // substituting them from _captures if found.  This is the internal function
    // and should not be called explicitly- call subst() instead.
    Ptree *subst(Ptree *p)
    {
      if (!p) {
        return 0;
      } else if (p->IsLeaf()) {
        if (is_capture(p)) {
          return get_capture(p);
        }
        return p;
      } else if (p->IsA(ntFuncallExpr)) {
        // Certain function calls are special.  They take the form of _<name>_.
        Ptree *fn = p->Car();
        if (PtreeUtil::Eq(fn,"_str_")) {
          // _str_: Create a string from the format specified in the first
          //         argument.  Subsequent arguments supply values to the fields
          //         specified in the format string.
          return createStr(p);
        } else if (PtreeUtil::Eq(fn,"_eval_")) {
          // _eval_: Evaluate the argument as a numerical expression.  Perform
          //         all substitutions first.  This only handles basic arithmetic
          //         binary and unary ops and literal values.
          return evalExpr(p);
        } else if (PtreeUtil::Eq(fn,"_null_")) {
          // _null_: Return a null value.
          return 0;
        }
      } else if (p->IsA(ntAssignExpr)) {
        // Assignments can be special, if the initial transformation is to a
        // local variable, because of expression expansion.  We turn off
        // expansion on the left and proceed.
        bool ee = _expand_expr;
        _expand_expr = false;
        Ptree *car = p->Car();
        Ptree *car2 = subst(car);
        _expand_expr = ee;
        Ptree *cdr = p->Cdr();
        Ptree *cdr2 = subst(cdr);
        if (car == car2 && cdr == cdr2) {
          return p;
        } else {
          return new PtreeAssignExpr(car2,cdr2);
        }
      }

      // Default- substitute the left and right, then create a new node if
      // necessary.
      Ptree *car = p->Car();
      Ptree *car2 = subst(car);
      Ptree *cdr = p->Cdr();
      Ptree *cdr2 = subst(cdr);
      if (car == car2 && cdr == cdr2) {
        return p;
      } else {
        return clonePtree(p,car2,cdr2);
      }
    }

    bool dontNeedParens(Ptree *e)
    {
      return (e->IsA(ntParenExpr) || e->IsA(ntFuncallExpr) || e->IsA(ntDotMemberExpr) || e->IsLeaf());
    }   

    Ptree *translate(Ptree *list)
    {
      ForEach(_subst,i) {
        if (equal(list,i->first)) {
          _subst_ocrd = true;
          return translate(subst(i->second));
        }
      }
      if(list == 0) {
        return list;
      } else if (dynamic_cast<LeafName*>(list)) {
        // Variable: If we're expanding expressions, then capture its rhs as
        // long as it's a local variable.
        if (_expand_expr) {
          if (auto val = find_var(list)) {
            if (Ptree *new_exp = translate(val->second)) {
              _subst_ocrd = true;
              if (dontNeedParens(new_exp)) {
                return new_exp;
              } else {
                return new PtreeParenExpr(Ptree::Make("("), PtreeUtil::List(val->second, Ptree::Make(")")));
              }
            }
          }
        }
      } 
      if (list->IsLeaf()) {
        return list;
      }
      if (dynamic_cast<PtreeBlock*>(list)) {
        // We have a brace-delimited block of some kind, meaning that we have a
        // new scope.  Add a new scope block and then translate the body.
        _vars.push_back(TokenMap());
        Ptree *b = PtreeUtil::Second(list);
        Ptree *b2 = translate(b);
        if (!b2) {
          return 0;
        } else if (b == b2) {
          return list;
        } else {
          return new PtreeBlock(list->Car(),b2,PtreeUtil::Third(list));
        }
        _vars.pop_back();
      }
      if (list->IsA(ntParenExpr) && _expand_expr) {
        // Parenthetical expression:  Get rid of excess parentheses, if they exist.
        Ptree *p = PtreeUtil::Second(list);
        Ptree *p2 = translate(p);
        if (dontNeedParens(p2)) {
          return p2;
        }
        if(p == p2) {
          return list;
        } else {
          return new PtreeParenExpr(list->Car(), PtreeUtil::List(p2, PtreeUtil::Third(list)));
        }
      }
      if (list->IsA(ntDeclaration) && _expand_expr) {
        // Declaration: Store the item for later expansion and return 0.
        Ptree* decls = PtreeUtil::Third(list);
        do {
          Ptree *decl = decls->Car();
          Ptree *v = PtreeUtil::First(decl);
          Ptree *e = translate(PtreeUtil::Third(decl));
          _vars.back()[v] = e;
          // Skip the comma and go to the next declaration, if one exists.
          decls = decls->Cdr();
          if (decls) {
            decls = decls->Cdr();
          }
        } while (decls);
        return 0;
      }
      if (list->IsA(ntAssignExpr) && _expand_expr) {
        // Assignment:  Store the item for later if it's a local variable.
        Ptree* left = PtreeUtil::First(list);
        if (left->IsLeaf()) {
          if (auto val = find_var(left)) {
            Ptree* right = PtreeUtil::Third(list);
            Ptree* right2 = translate(right);
            // Is this a modifying assignment, e.g. +=?  If so, then we need to
            // store it as the appropriate binary operation.
            Ptree *op = PtreeUtil::Second(list);
            if (!PtreeUtil::Eq(op,"=")) {
              Ptree *op2 = new DupLeaf(op->GetPosition(),op->GetLength()-1);
              Ptree* left2 = translate(left);
              right2 = new PtreeInfixExpr(left2,PtreeUtil::List(op2,right2));
              right2 = new PtreeParenExpr(Ptree::Make("("), PtreeUtil::List(right2, Ptree::Make(")")));
            }
            val->second = right2;
            return 0;
          }
        }
      }
      {
        PtreeArray newlist;
        bool changed = false;
        Ptree* rest = list;
        while(rest != 0) {
          Ptree* p = rest->Car();
          Ptree* q = translate(p);
          if (q || !p) {
            // The !p clause is for preserving empty blocks written by the user,
            // e.g. an empty if-clause.
            newlist.Append(q);
          }
          if(p != q)
            changed = true;

          rest = rest->Cdr();
        }

        // Squash empty statements.
        if (newlist.Number() == 0 || (newlist.Number() == 1 && PtreeUtil::Eq(newlist[0],";"))) {
          return 0;
        }
        // Squash empty blocks.
        if (dynamic_cast<PtreeBlock*>(list) && newlist.Number() == 2) {
          return 0;
        }

        Ptree *r = (changed) ? newlist.All() : list;
        NonLeaf *nl = dynamic_cast<NonLeaf*>(r);
        if (changed && nl) {
          if (dynamic_cast<PtreeBlock*>(list)) {
            r = new PtreeBlock(nl->Car(),nl->Cdr());
          } else if (list->IsA(ntParenExpr)) {
            r = new PtreeParenExpr(nl->Car(),nl->Cdr());
          } else if (list->IsA(ntFuncallExpr)) {
            r = new PtreeFuncallExpr(nl->Car(),nl->Cdr());
          }
        }
        return r;
      }
    }

    bool             _expand_expr;
    bool             _subst_ocrd;
    const PPairList &_subst;
    DocWalker       *_walker;
    TokenMap         _captures; // For captures.
    TokenStack       _vars;     // For expression expansion.
  };

  // Same as above, except that when a token starting with $ is found,
  // e.g. $foo, that token is stored and re-used by a substitution which
  // references it.
  Ptree* substAllEqual(bool &subst_ocrd,Ptree *list, bool expand_exprs,const PPairList &subst,DocWalker *walker)
  {
    SubstObj s(expand_exprs,subst,walker);

    Ptree *p = s.translate(list);
    subst_ocrd = s._subst_ocrd;
    return p;
  }

  // Perform operation transformations and expression expansion.
  Opencxx::Ptree* doOpTransforms(Opencxx::Ptree *op,bool expand_exprs,DocWalker *walker,const DocCfg &doccfg)
  { 
      //      if (expand_exprs && walker) {
      //   walker->set_expand_expr(true);
      //   Ptree *op2 = walker->Translate(op);
      //   if (walker->has_ctrl_stmts()) {
      //     // We can't do statement expansion if there are control statements, so
      //     // in that case, forget about it.
      //     expand_exprs = false;
      //   } else {
      //     op = op2;
      //   }
      //   walker->set_expand_expr(false);
      // }

    bool subst_ocrd = false;
    do {
      op = (!expand_exprs && doccfg.op_transforms().empty()) ? op : substAllEqual(subst_ocrd,op,expand_exprs,doccfg.op_transforms(),walker);
    } while (subst_ocrd);

    return op;
  }

}

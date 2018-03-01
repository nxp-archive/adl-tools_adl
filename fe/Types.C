//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Miscellaneous helper types.
//

#include <sstream>
#include <iterator>
#include <cassert>
#include <iostream>
#include <iomanip>
#include <stdexcept>
#include <cstdio>

#include "opencxx/parser/PtreeBlock.h"
#include "opencxx/parser/TheErrorLog.h"
#include "opencxx/parser/ErrorLog.h"
#include "opencxx/parser/PtreeUtil.h"
#include "opencxx/parser/DupLeaf.h"
#include "opencxx/parser/MopMsg.h"
#include "opencxx/Environment.h"
#include "opencxx/TypeInfo.h"

#include "helpers/CodeGen.h"
#include "helpers/Macros.h"
#include "helpers/AdlTranslatingWalker.h"

#include "Helpers.h"
#include "Types.h"
#include "Data.h"

using namespace std;
using namespace rnumber;
using namespace Opencxx;

namespace adl {

  Ptree *parentObj = Ptree::Make("$parent$");

  // If set to true, then extra information is printed when Display() is called
  // on a Ptree object.
  static bool DataModelDebug = false;

  bool dataModelDebug()
  {
    return DataModelDebug;
  }

  void setDataModelDebug(bool d)
  {
    DataModelDebug = d;
  }

  // This is a more deterministic symbol generator than the one in Plasma.  Use
  // this one so that files will generate the same way, so that comparisons are
  // easier.
  Ptree* AdlGenSym()
  {
    static int count = 1;

    char buf[20];
    int len = sprintf (buf,"_sym%d",count++);

    // We don't technically need the null byte, but it might make debuggin
    // easier.
    char* name = new (GC) char[len+1];
    strcpy(name,buf);
    return new Leaf(name,len);
  }

  int compare (Opencxx::Ptree *x,Ptree *y)
  {
    // Check pointers- if the same or one is null, we can handle it here.
    if (x == y) {
      return 0;
    }
    if (!x && y) {
      return -1;
    }
    if (x && !y) {
      return 1;
    }
    // Otherwise, compare by stringification.
    const char *xs = x->ToString();
    const char *ys = y->ToString();
    return strcmp(xs,ys);
  }

  bool reifyPtree(unsigned &res,Ptree *p)
  {
    return p->Reify(res);
  }

  bool reifyPtree(int &res,Ptree *p)
  {
    unsigned r;
    bool result = p->Reify(r);
    res = (unsigned)r;
    return result;
  }

  bool reifyPtree(uint64_t &res,Ptree *p)
  {
    unsigned v;
    if (p->IsLeaf() && p->Reify(v)) {
      res = strtoull(p->GetPosition(),0,0);
      return true;
    }
    return false;
  }

  bool reifyPtree(double &res,Ptree *p)
  {
    if (p->IsLeaf()) {
      const char* s = p->GetPosition();
      int len = p->GetLength();
      char *ep;
      res = strtod(s,&ep);
      // We fail if the conversion didn't stop at the end of the token.
      return (ep == (s+len));
    } else {
      return false;
    }
  }

  bool reifyPtree(bool &res,Ptree *p)
  {
    return p->Reify(res);
  }


  // Create leaf objects based upon a specific type.  In this case, we derive
  // the token enum from the actual type.

  Ptree *make_leaf(bool n)
  {
    return make_leaf<LeafUNSIGNED>((unsigned)n);
  }  
  
  Ptree *make_leaf(unsigned n)
  {
    return make_leaf<LeafUNSIGNED>(n);
  }

  Ptree *make_leaf(int n)
  {
    return make_leaf<LeafSIGNED>(n);
  }

  Ptree *make_leaf(uint64_t n)
  {
    return make_leaf<LeafUNSIGNED>(n);
  }    

  Ptree *make_leaf(double n)
  {
    return make_leaf<LeafDOUBLE>(n);
  }  
  

  
  bool BitVect::is_set() const
  {
    return ( find(begin(),end(),false) == end() ); 
  }

  bool BitVect::is_set(unsigned start,unsigned stop) const
  {
    assert (start < size() && stop < size());  
    return ( find(begin()+start,begin()+stop+1,false) == (begin()+stop+1) ); 
  }

  bool BitVect::is_reset() const
  {
    return ( find(begin(),end(),true) == end() ); 
  }

  bool BitVect::is_reset(unsigned start,unsigned stop) const
  {
    assert (start < size() && stop < size());  
    return ( find(begin()+start,begin()+stop+1,true) == (begin()+stop+1) ); 
  }

  void BitVect::set()
  {
    fill (begin(),end(),true);
  }

  void BitVect::set(unsigned start,unsigned stop)
  {
    assert (start < size() && stop < size());
    fill (begin()+start,begin()+stop+1,true);
  }

  void BitVect::reset()
  {
    fill (begin(),end(),false);
  }

  void BitVect::reset(unsigned start,unsigned stop)
  {
    assert (start < size() && stop < size());
    fill (begin()+start,begin()+stop+1,false);
  }

  gc_string BitVect::str() const
  {
    gc_ostringstream ss;

    if (empty()) {
      return "";
    }

    ss << "0b";
    ostream_iterator<bool> iter(ss);
    copy(begin(),end(),iter);
    return ss.str();
  }

  ostream &operator<<(ostream &o,const StrVect &sv)
  {
    printSequence(o,sv,false);
    return o;
  }

  ostream &operator<<(ostream &o,const StrList &sl)
  {
    printSequence(o,sl,false);
    return o;
  }

  ostream &operator<<(ostream &o,const IntOrFunc &i)
  {
    if (i.is_int()) {
      o << i._val;
    } else {
      o << i._func;
    }
    return o;
  }

  ostream &operator<<(ostream &o,const StrSet &ss)
  {
    printSequence(o,ss,false);
    return o;
  }

  ostream &operator<< (ostream &o,const Path &path)
  {
    bool first = true;
    ForEach(path,i) {
      if (!first) {
        o << path.get_sep();
      }
      first = false;
      o << *i;
    }
    return o;
  }

  gc_string getPathStr(const Path &path,const char *osep)
  {
    gc_string p;
    const char *sep = (osep) ? osep : path.get_sep();
    bool first = true;
    ForEach(path,i) {
      if (!first) {
        p += sep;
      }
      first = false;
      p += *i;
    }
    return p;
  }

  const char *BindParent::_type = "P$parentObj$";

  Bind::Kind BindParent::What()
  {
    return isVarName;
  }

  void BindParent::GetType(TypeInfo& t, Environment* e)
  {
    t.Set((char*)_type, e);
  }
  
  const char* BindParent::GetEncodedType()
  {
    return (char*)_type;
  }

  bool BindParent::IsType()
  {
    return false;
  }

  // Add a parent object pointer to the given environment.
  void addParentObj(Opencxx::Environment *env,Aspects *parent)
  {
    env->AddEntry((char*)parentObj->GetPosition(),parentObj->GetLength(),new BindParent(parent));  
  }

  UsedReg::UsedReg() : 
    _reg(0), 
    _begin(0), 
    _end(0), 
    _parm(-1), 
    _partial(false), 
    _delay(0),
    _direct(false),
    _cond(false)
  {}

  UsedReg::UsedReg(const RegBase *r) : 
    _reg(r), 
    _begin(0), 
    _end(0), 
    _parm(-1), 
    _mask(0,r->width()),
    _partial(false), 
    _delay(0),
    _direct(false),
    _cond(false) 
  {}

  UsedReg::UsedReg(const RegBase *r,Opencxx::Ptree *b) : 
    _reg(r), 
    _begin(b), 
    _end(0), 
    _parm(-1), 
    _mask(0,r->width()),
    _partial(false),
    _delay(0), 
    _direct(false),
    _cond(false) 
  {}

  UsedReg::UsedReg(const RegBase *r,Opencxx::Ptree *v,int p) : 
    _reg(r), 
    _begin(v), 
    _end(0), 
    _parm(p), 
    _mask(0,r->width()),
    _partial(false),
    _delay(0), 
    _direct(false),
    _cond(false) 
  {}

  UsedReg::UsedReg(const RegBase *r,Opencxx::Ptree *b,Opencxx::Ptree *e) : 
    _reg(r), 
    _begin(b), 
    _end(e), 
    _parm(-1), 
    _mask(0,r->width()),
    _partial(false), 
    _delay(0),
    _direct(false),
    _cond(false) 
  {}

  void UsedReg::setreg(const RegBase *r)
  {
    _reg = r;
    _mask.resize(r->width());
  }

  const Reg *UsedReg::reg() const
  {
    return dynamic_cast<const Reg *>(_reg);
  }

  const RegFile *UsedReg::regfile() const
  {
    return dynamic_cast<const RegFile *>(_reg);
  }

  bool UsedReg::is_fixed() const 
  { 
    return reg() || (_begin && !is_range());
  }

  bool UsedReg::is_range() const
  {
    return regfile() && _begin && _end;
  }

  int UsedReg::next_slot(bool &fixed) const
  {
    if ( reg() ) {
      fixed = true;
      return 1;
    } else {
      if (is_fixed()) {
        fixed = true;
        return 1;
      } else {
        fixed = false;
        return regfile()->size();
      }
    }
  }

  // This clears the partial-access status if a mask is fully set.
  void UsedReg::check_if_still_partial() const
  {
    RNumber tmp(_mask);
    tmp.invert();
    if (tmp.iszero()) {
      _mask.clearAll();
      _partial = false;
    }
  }

  // If the incoming mask is zero, then this means we're not using masks
  // (meaning that we can't staticaly determine partial usage), so we just clear
  // the mask.  The partial flag is still set, so we know that this isn't a full
  // usage of the register.  If the mask is completely set, then we can switch
  // off the partial flag, since we know that the entire register is used.  This
  // is mostly important when a series of partial updates completely overwrites
  // a register and the register is then later referenced- we don't want it to
  // be considered a source in such a situation.
  void UsedReg::update_mask(const UsedReg &ur) const
  {
    if (ur._mask.iszero()) {
      _mask.clearAll();
    } else if (!_mask.iszero()) {
      _mask |= ur._mask;
    }
    check_if_still_partial();
  }

  void UsedReg::update_mutables(const UsedReg &ur) const 
  {
    if (ur.partial()) {
      _partial = true;
    }
    if (ur.conditional()) {
      _cond = true;
    }
    if (ur.delayed()) {
      _delay = ur.delay();
    }
    if (ur.direct()) {
      _direct = true;
    }
    update_mask(ur);
  }

  bool operator==(const UsedReg &x,const UsedReg &y)
  {
    return x.regb() == y.regb() &&
      PtreeUtil::Equal(x._begin,y._begin) &&
      PtreeUtil::Equal(x._end,y._end) &&
      PtreeUtil::Equal(x._delay,y._delay) &&
      x._parm == y._parm &&
      x._mask == y._mask &&
      x._partial == y._partial &&
      x._direct == y._direct &&
      x._cond == y._cond;
  }

  // We compare by name first, then we consider a fixed-index less than a field,
  // which is less than not having an element.
  bool operator<(const UsedReg &x,const UsedReg &y)
  {
    PtreeCmp pc;
    if (x.regb() && y.regb()) {
      // If they're both initialized...
      if (x.regb()->name() == y.regb()->name()) {
        // Names are the same, so compare expressions.
        if (x._begin && y._begin) {
          // Sort first by begin.  If those are equal, then sort by end.
          int c = compare(x._begin,y._begin);
          if (c == 0) {
            return pc(x._end,y._end);
          }
          return c < 0;
        } else {
          return (!x._begin && y._begin);
        }
      } else {
        // Names not the same- return whose is less.
        return x.regb()->name() < y.regb()->name();
      }
    } else {
      // Not both initialized, so uninit'd is less than init'd.
      return (!x.regb() && y.regb());
    }  
    return false;
  }

  // A less-than comparison that does take into account the mask value.
  bool compare_lt_w_mask(const UsedReg &x,const UsedReg &y)
  {
    if (x.regb() && y.regb()) {
      // If they're both initialized...
      if (x.regb()->name() == y.regb()->name()) {
        // Names are the same, so compare expressions.
        if (x._begin && y._begin) {
          // Sort first by begin.  If those are equal, then sort by end.
          int c = compare(x._begin,y._begin);
          if (c == 0) {
            int c2 = compare(x._end,y._end);
            if (c2 == 0) {
              return x._mask < y._mask;
            } else {
              return c2 < 0;
            }
          }
          return c < 0;
        } else if (!x._begin && y._begin) {
          return true;
        } else if (x._begin && !y._begin) {
          return false;
        } else {
          // Neither has a begin expression.
          return x._mask < y._mask;
        }
      } else {
        // Names not the same- return whose is less.
        return x.regb()->name() < y.regb()->name();
      }
    } else {
      // Not both initialized, so uninit'd is less than init'd.
      return (!x.regb() && y.regb());
    }  
    return false;    
  }

  // A less-than comparison that just detects if a mask exists.
  bool compare_lt_if_mask(const UsedReg &x,const RNumber &x_mask,const UsedReg &y,const RNumber &y_mask)
  {
    if (x.regb() && y.regb()) {
      // If they're both initialized...
      if (x.regb()->name() == y.regb()->name()) {
        // Names are the same, so compare expressions.
        if (x._begin && y._begin) {
          // Sort first by begin.  If those are equal, then sort by end.
          int c = compare(x._begin,y._begin);
          if (c == 0) {
            int c2 = compare(x._end,y._end);
            if (c2 == 0) {
              return x._mask < y._mask;
            } else {
              return c2 < 0;
            }
          }
          return c < 0;
        } else if (!x._begin && y._begin) {
          return true;
        } else if (x._begin && !y._begin) {
          return false;
        } else { 
          // Neither has a begin expression, so compare based on whether a mask
          // exists or not.
          RNumber x_mask_tmp ( (!x_mask.iszero()) ? x_mask : x._mask);
          RNumber y_mask_tmp ( (!y_mask.iszero()) ? y_mask : y._mask);
          return (x_mask_tmp.iszero() && !y_mask_tmp.iszero());
        }
      } else {
        // Names not the same- return whose is less.
        return x.regb()->name() < y.regb()->name();
      }
    } else {
      // Not both initialized, so uninit'd is less than init'd.
      return (!x.regb() && y.regb());
    }  
    return false;    
  }

  ostream &operator<<(ostream &o,const UsedReg &ur)
  {
    if (!ur._reg) {
      o << "<empty>";
    } else {
      o << ur._reg->name();
      if ( ur._begin ) {
        o << "(" << ur._begin;
        if ( ur._end) {
          o << "," << ur._end;
        }
        if ( ur.is_parm() ) {
          o << " [parm #" << ur._parm << "]";
        }
        o << ")";
      }
      
    }
    if (!ur._mask.iszero()) {
      o << "[mask 0x" << hex << setfill('0') << setw(ur._reg->width()/4) << ur._mask << "]" << dec;
    }
    if (ur._partial) {
      o << "/p";
    }
    if (ur.delayed()) {
      o << "/d[" << ur.delay() << "]";
    }
    if (ur._direct) {
      o << "/D";
    }
    if (ur._cond) {
      o << '?';
    }
    return o;
  }

  void UsedReg::display() const
  {
    cerr << *this;
  }

  void UsedRegs::display() const
  {
    cerr << *this;
  }

  ostream &operator<<(ostream &o,const UsedRegs &ur)
  {
    printSequence(o,ur,false);
    return o;
  }

  void UsedRegs::update_mutables(const UsedRegs &x)
  {
    ForEach(x,i) {
      auto f = find(*i);
      if (f != this->end()) {
        f->update_mutables(*i);
      }
    }
  }

  ostream &operator<<(ostream &o,const UsedRegList &urs)
  {
    ostream_iterator<UsedRegList::value_type> iter(o,", ");
    copy(urs.begin(),urs.end(),iter);
    return o;
  }

  void merge_conds(UsedRegs &trg,const UsedRegs &src)
  {
    gc_set<const UsedReg*> src_set;
    // Insert items from source list into target list.
    ForEach(src,s) {
      auto ip = trg.insert(*s);
      if (ip.second) {
        // Item was inserted, so mark as conditional, since it doesn't exist in
        // the source list.
        ip.first->_cond = true;
      }
      // Item was either a target or common, so record this fact.
      src_set.insert(&*ip.first);
    }
    // For every item not in the list of items already covered, mark as
    // conditional, since it means they're unique to the target list.
    ForEach(trg,t) {
      if (!src_set.count(&*t)) {
        t->_cond = true;
      }
    }
  }

  void merge_conds(UsedRegs &trg,const UsedRegs &src,bool merge_cond)
  {
    if (merge_cond) {
      // Merge the conditionals.
      merge_conds(trg,src);
    } else {
      // Not merging conditionals, so just add all new items.
      trg.add(src);
    }
  }


  UsedMem::UsedMem(const gc_string &name,Ptree *delay) : 
    _name(name),
    _delay(delay)
  {}

  ostream &operator<<(ostream &o,const UsedMem &um)
  {
    o << um.name();
    if (um.delayed()) {
      o << "/d[" << um.delay() << "]";
    }

    return o;
  }

  bool operator==(const UsedMem &x,const UsedMem &y)
  {
    return x.name() == y.name() && PtreeUtil::Equal(x._delay,y._delay);
  }

  bool operator<(const UsedMem &x,const UsedMem &y)
  {
    if (x.name() != y.name()) {
      return x.name() < y.name();
    } else {
      return compare(x._delay,y._delay) < 0;
    }
  }

  void UsedMems::display() const
  {
    cerr << *this;
  }

  ostream &operator<<(ostream &o,const UsedMems &um)
  {
    printSequence(o,um,false);
    return o;
  }


  
  void UsageInfo::update(const UsageInfo &x,bool merge_cond) 
  {
    merge_conds(_src_regs,x._src_regs,merge_cond);
    _src_regs.update_mutables(x._src_regs);

    merge_conds(_trg_regs,x._trg_regs,merge_cond);
    _trg_regs.update_mutables(x._trg_regs);

    update_non_regs(x);
  }

  void UsageInfo::update_non_regs(const UsageInfo &x) 
  {
    _src_mems.insert(x._src_mems.begin(),x._src_mems.end());
    _trg_mems.insert(x._trg_mems.begin(),x._trg_mems.end());

    _src_caches.insert(x._src_caches.begin(),x._src_caches.end());
    _trg_caches.insert(x._trg_caches.begin(),x._trg_caches.end());

    _helpers.insert(x._helpers.begin(),x._helpers.end());

    _exceptions = _exceptions || x._exceptions;
    _halts = _halts || x._halts;
  }

bool UsageInfo::empty() const
{
  if (_src_regs.empty() &&
      _trg_regs.empty() &&
      _src_mems.empty() &&
      _trg_mems.empty() &&
      _src_caches.empty() &&
      _trg_caches.empty() &&
      _helpers.empty()) {
    return true;
  } else {
    return false;
  }
}

  bool UsageInfo::operator==(const UsageInfo &x) const
  {
    return _src_regs == x._src_regs &&
      _trg_regs == x._trg_regs &&
      _src_mems == x._src_mems &&
      _trg_mems == x._trg_mems &&
      _src_caches == x._src_caches &&
      _trg_caches == x._trg_caches &&
      _helpers == x._helpers;
  }

  void UsageInfo::mark_as_cond()
  {
    ForEach(_src_regs,i) {
      i->_cond = true;
    }
    ForEach(_trg_regs,i) {
      i->_cond = true;
    }
  }

  void UsageInfo::clear() 
  {
    _src_regs.clear();
    _trg_regs.clear();
    _src_mems.clear();
    _trg_mems.clear();
    _src_caches.clear();
    _trg_caches.clear();
    _helpers.clear();
    _exceptions = false;
    _halts = false;
  }

  void printUsageInfo(ostream &o,const UsageInfo &ui,const gc_string &pfx)
  {
    if (!ui._src_regs.empty()) {
      o << pfx << "Source Registers:  " << ui._src_regs << "\n";
    }
    if (!ui._trg_regs.empty()) {
      o << pfx << "Target Registers:  " << ui._trg_regs << "\n";
    }
    if (!ui._src_mems.empty()) {
      o << pfx << "Source Memories:  " << ui._src_mems << "\n";
    }
    if (!ui._trg_mems.empty()) {
      o << pfx << "Target Memories:  " << ui._trg_mems << "\n";
    }
    if (!ui._src_caches.empty()) {
      o << pfx << "Source Caches:  " << ui._src_caches << "\n";
    }
    if (!ui._trg_caches.empty()) {
      o << pfx << "Target Caches:  " << ui._trg_caches << "\n";
    }
    if (!ui._helpers.empty()) {
      o << pfx << "Helper Functions:  " << ui._helpers << "\n";
    }
    if (ui._exceptions) {
      o << pfx << "Does raise exceptions.\n";
    }
  }

  std::ostream &operator<<(std::ostream &o,const UsageInfo &ui)
  {
    printUsageInfo(o,ui,"");
    return o;
  }

  void UsageInfo::display()
  {
    cerr << *this;
  }

  PtreeHelperCall::PtreeHelperCall(UsedRegList &parms)
  {
    _parms.swap(parms);
  };

  PtreeHelperCallLeaf::PtreeHelperCallLeaf(Ptree *p,UsedRegList &parms) : 
    Leaf(p->GetPosition(),p->GetLength()),
    PtreeHelperCall(parms)
  {
  };

  int PtreeHelperCallLeaf::Write(ostream &o, int x)
  {
    int n = Leaf::Write(o,x);
    if (DataModelDebug) {
      o << "[helper: " << _parms << "]";
    }
    return n;
  }

  PtreeHelperCallNonLeaf::PtreeHelperCallNonLeaf(Ptree *p,UsedRegList &parms) : 
    NonLeaf(p->Car(),p->Cdr()),
    PtreeHelperCall(parms)
  {
  };

  int PtreeHelperCallNonLeaf::Write(ostream &o, int x)
  {
    int n = NonLeaf::Write(o,x);
    if (DataModelDebug) {
      o << "[helper: " << _parms << "]";
    }
    return n;
  }

  void PtreeStmts::Print(std::ostream& s, int indent, int depth)
  {
    if(TooDeep(s, depth))
      return;

    int indent2 = indent + 1;
    s << "[ 0 ";
    Ptree* body = PtreeUtil::Rest(this);
    if(body == 0){
      PrintIndent(s, indent2);
      s << "0";
    }
    else
      while(body != 0){
	    PrintIndent(s, indent2);
	    if(body->IsLeaf()){
          s << "@ ";
          body->Print(s, indent + 1, depth + 1);
	    }
	    else{
          Ptree* head = body->Car();
          if(head == 0)
		    s << "0";
          else
		    head->Print(s, indent + 1, depth + 1);
	    }

	    body = body->Cdr();
      }

    PrintIndent(s, indent);
    s << " 0 ]";
  }

  int PtreeStmts::Write(std::ostream& out, int indent)
  {
    int n = 0;

    Ptree* p = this->Cdr()->Car();
    while(p != 0){
      if(p->IsLeaf()){
	    TheErrorLog().Report(
                             MopMsg(Msg::Fatal, "PtreeBrace::Write()", "non list"));
	    break;
      }
      else{
	    ++n;
	    Ptree* q = p->Car();
	    p = p->Cdr();
	    if(q != 0)
          n += q->Write(out, indent + 1);
      }
    }    
    PrintIndent(out, indent);
    return n;
  }

  // If we're translating with an AdlTranslator, then translate using the
  // appropriate function.
  Ptree* PtreeStmts::Translate(AbstractTranslatingWalker* walker)
  {
    IfAdlTranslator(walker,w) {
      return w->TranslateStmts(this);
    } else {
      return PtreeBrace::Translate(walker);
    }    
  }

  PtreeRegWrite::PtreeRegWrite(Ptree *p,bool thisreg) : 
    PtreeRegRead(p,thisreg)
  {
    _regaccess._write = true;
  };

  int PtreeRegWrite::Write(ostream &o, int x)
  {
    int n = NonLeaf::Write(o,x);
    if (DataModelDebug) {
      o << "[" << _regaccess << " := " << get_expr() << "]";
    }
    return n;
  }

  Ptree *PtreeRegWrite::Translate(AbstractTranslatingWalker* walker)
  {
    IfAdlTranslator(walker,w) {
      return w->TranslateRegWrite(this);
    } else {
      return NonLeaf::Translate(walker);
    }
  }

  PtreeRegRead::PtreeRegRead(Ptree *p,bool thisreg) : 
    NonLeaf(p,0),
    _next(0)
  {
    _regaccess._write = false;
    _regaccess._thisreg = thisreg;
  };

  int PtreeRegRead::Write(ostream &o, int x)
  {
    int n = NonLeaf::Write(o,x);
    if (DataModelDebug) {
      o << "[" << _regaccess << "]";
    }
    return n;
  }

  Ptree *PtreeRegRead::Translate(AbstractTranslatingWalker* walker)
  {
    IfAdlTranslator(walker,w) {
      return w->TranslateRegRead(this);
    } else {
      return NonLeaf::Translate(walker);
    }
  }



  PtreeMemWrite::PtreeMemWrite(Ptree *p) : 
    PtreeMemRead(p)
  {
    _memaccess._write = true;
  };

  int PtreeMemWrite::Write(ostream &o, int x)
  {
    int n = NonLeaf::Write(o,x);
    if (DataModelDebug) {
      o << "[" << _memaccess << " := " << get_expr() << "]";
    }
    return n;
  }

  Ptree *PtreeMemWrite::Translate(AbstractTranslatingWalker* walker)
  {
    IfAdlTranslator(walker,w) {
      return w->TranslateMemWrite(this);
    } else {
      return NonLeaf::Translate(walker);
    }
  }

  PtreeMemRead::PtreeMemRead(Ptree *p) : 
    NonLeaf(p,0)
  {
    _memaccess._write = false;
  };

  int PtreeMemRead::Write(ostream &o, int x)
  {
    int n = NonLeaf::Write(o,x);
    if (DataModelDebug) {
      o << "[" << _memaccess << "]";
    }
    return n;
  }

  Ptree *PtreeMemRead::Translate(AbstractTranslatingWalker* walker)
  {
    IfAdlTranslator(walker,w) {
      return w->TranslateMemRead(this);
    } else {
      return NonLeaf::Translate(walker);
    }
  }

  PtreeSubInstrCall::PtreeSubInstrCall(Ptree *p,const SubInstr &subinstr,
                                       const SubInstrGrp &subinstrgrp,
                                       const UsageInfo &uinfo) :
    NonLeaf(p,0),
    _subinstr(subinstr),
    _subinstrgrp(subinstrgrp),
    _uinfo(uinfo)
  {
  }

  int PtreeSubInstrCall::Write(ostream &o, int x)
  {
    size_t n = o.tellp();
    o << _subinstr.name();
    Ptree *c = Car();
    if (c && c->IsA(ntFuncallExpr)) {
      o << Car()->Cdr();
    }
    if (DataModelDebug) {
      o << "[" << _subinstrgrp.name() << ": " << _subinstr.name() << "]";
    }
    n -= o.tellp();
    return n;
  }

  Ptree* PtreeSubInstrCall::Translate(AbstractTranslatingWalker* walker)
  {
    IfAdlTranslator(walker,w) {
      return w->TranslateSubInstrCall(this);
    } else {
      return walker->TranslatePtree(this);
    }
  }

  const RegBase *RegAccess::rb() const 
  {
    if (_reg) { return _reg; } else { return _regfile; }
  }

  std::ostream &operator<< (std::ostream &o,const RegAccess &ra)
  {
    if (ra._direct) {
      o << "direct ";
    }
    if (ra._write) {
      if (ra._delay) {
        o << "delayed (by " << ra._delay << ") ";
      }
      o << "write";
    } else {
      o << "read";
    }
    if (ra._thisreg) {
      o << " thisreg";
    }
    if (ra._context) {
      o << "(ctx " << ra._context << ")";
    }
    o << ": ";
    if (ra.rb()) {
      o << ra.rb()->name();
      if (ra.is_regfile() && ra._index) {
        o << "(" << ra._index;
        if (!PtreeUtil::Equal(ra._index,ra._rindex)) {
          o << " (" << ra._rindex << ")";
        }
        o << ")";
      }
      if (ra._field) {
        o << "." << ra._field->name();
        if (ra._findex) {
          o << "(" << ra._findex << ")";
        }
      } else if (ra.is_slice()) {
        if (ra._right) {
          o << "(" << ra._left << "," << ra._right << ")";
        } else {
          o << "(" << ra._left << ")";
        }
      }
    } else {
      o << "<empty>";
    }
    return o;
  }

  std::ostream &operator<< (std::ostream &o,const MemAccess &ma)
  {
    if (ma._write) {
      if (ma._delay) {
        o << "delayed (by " << ma._delay << ") ";
      }
      o << "write";
    } else {
      o << "read";
    }
    o << ": ";
    if (ma.is_global()) {
      o << MemoryName;
    } else {
      o << ma.mem()->name();
    }
    o << dec << " @ " << ma._addr << " of " << ma._size << " bytes";
    if (ma._type) {
      o << ", type=" << ma._type;
    }
    if (ma._cw_addr) {
      o << ", critical-addr=" << ma._cw_addr;
    }
    return o;
  }

  void HelperInfo::print(ostream &o,const gc_string &pfx) const
  {
    o << _func << "\n";
    printUsageInfo(o,*this,pfx);
  }

  ostream &operator<< (ostream &o,const HelperInfo &h)
  {
    h.print(o,gc_string());
    return o;
  }

  AuxFunc::AuxFunc(Ptree *func,const gc_string &rtype,const gc_string &comment,
                   bool isConst) :
    _name(AdlGenSym()),
    _func(func),
    _isConst(isConst),
    _rtype(rtype),
    _comment(comment)
  {
  }

  void AuxFunc::print(std::ostream &o,const gc_string &pfx) const
  {
    if (!_comment.empty()) {
      o << pfx << "// " << filter_string(_comment,"\n"+pfx+"// ") << "\n";
    }
    o << pfx << _rtype << " " << _func << "\n";
  }

  void null_ptr_exception()
  {
    throw runtime_error("Null-pointer exception");
  }


  void ClassData::writeMemberVars(std::ostream &o) const
  {
    ForEach(_member_vars,i) {
      o << *i << ";\n";
    }
  }

  void ClassData::writeConstrArgs(std::ostream &o) const
  {
    o << "(";
    bool first = true;
    ForEach(_constr_args,i) {
      list_sep(o,first);
      o << *i;
    }
    o << ")";
  }

  void ClassData::writeConstrInits(std::ostream &o) const
  {
    bool first = true;
    ForEach(_constr_inits,i) {
      constr_init_sep(o,first);
      o << *i;
    }
  }

  void ClassData::writeConstrStmts(std::ostream &o) const
  {
    ForEach(_constr_stmts,i) {
      o << *i << ";\n";
    }
  }

  void ClassData::writeResetStmts(std::ostream &o) const
  {
    ForEach(_reset_stmts,i) {
      o << *i << ";\n";
    }
  }

  void ClassData::writeDestrStmts(std::ostream &o) const
  {
    ForEach(_dstr_stmts,i) {
      o << *i << ";\n";
    }
  }


  // replaceAllEqual() substitutes SUBST[i].second for all occurences of ORIG[i].first in
  // LIST.  It recursively searches LIST for ORIG.
  Ptree* replaceAllEqual(Ptree* list, const PPairList &subst)
  {
    ForEach(subst,i) {
      if (PtreeUtil::Equal(list,i->first)) {
        return i->second;
      }
    }
    if(list == 0 || list->IsLeaf())
      return list;
    else{
      PtreeArray newlist;
      bool changed = false;
      Ptree* rest = list;
      while(rest != 0){
        Ptree* p = rest->Car();
        Ptree* q = replaceAllEqual(p, subst);
        newlist.Append(q);
        if(p != q)
          changed = true;

        rest = rest->Cdr();
      }

      Ptree *r = (changed) ? newlist.All() : list;
      NonLeaf *nl = dynamic_cast<NonLeaf*>(r);
      if (changed && dynamic_cast<PtreeBlock*>(list) && nl) {
        r = new PtreeBlock(nl->Car(),nl->Cdr());
      }
      return r;
    }
  }

  void getAliasSubstitutions(PPairList &subst,IntMap &parms,const Instr::Alias *alias)
  {
    if (!alias) return;

    auto trg = alias->_target;
    assert(trg);

    // Now do substitutions so that fields use the alias's names.  First,
    // construct a replacement mapping.
    // We also want to do basic expression evaluation of parm fields in order to
    // eliminate them if possible.
    ForEach (alias->_fields,i) {
      Ptree *s = 0;
      if ( i->_type == InstrBase::Field::Ref ) {
        s = Ptree::Make("%s",i->_sname);
      } else if ( i->_type == InstrBase::Field::Value ) {
        s = Ptree::Make("%d",(unsigned)i->_value);
        if (i->_field->is_parm()) {
          parms.insert(make_pair(i->_field->name(),i->_value));
        }
      } else if ( i->_type == InstrBase::Field::Expr ) {
        s = i->_expr;
      }
      if (s) {
        subst.push_back(make_pair( Ptree::Make("%s",i->_field->name().c_str() ) , s ));
      }
    }

  }

  void getRevAliasSubstitutions(PPairList &subst,IntMap &parms,const Instr::Alias *alias)
  {
    if (!alias) return;

    auto trg = alias->_target;
    assert(trg);

    // Now do substitutions so that fields use the alias's names.  First,
    // construct a replacement mapping.
    // We also want to do basic expression evaluation of parm fields in order to
    // eliminate them if possible.
    ForEach (alias->_fields,i) {
      Ptree *s = 0;
      if ( i->_type == InstrBase::Field::Ref ) {
        s = Ptree::Make("%s",i->_sname);
      } else if ( i->_type == InstrBase::Field::Value ) {
        s = Ptree::Make("%d",(unsigned)i->_value);
        if (i->_field->is_parm()) {
          parms.insert(make_pair(i->_field->name(),i->_value));
        }
      } else if ( i->_type == InstrBase::Field::Expr ) {
        s = i->_expr;
      }
      if (s) {
        subst.push_back(make_pair( s , Ptree::Make("%s",i->_field->name().c_str() ) ));
      }
    }

  }

  Ptree *clonePtree(Ptree *p,Ptree *car,Ptree *cdr)
  {
    Ptree *p2 = new NonLeaf(0,0);
    memcpy(p2,p,sizeof(Ptree));
    p2->SetCar(car);
    p2->SetCdr(cdr);
    return p2;
  }

  // We copy stuff to a vector so that we can make comparisons using standard
  // algorithms and we preserve the array itself (PtreeArray has only a shallow
  // copy).
  HelperArgs::HelperArgs(const gc_string &n,PtreeArray *args) :
    _name(n),
    _has_args(args)
  {
    if (args) {
      _args.resize(args->Number());
      ForRange(args->Number(),i) {
        _args[i] = (*args)[i];
      }
    }
  }

  // If the names compare, then we do a lexicographical comparison on the
  // vector, comparing using PtreeCmp, which compares the Ptree structure,
  // rather than just compares pointers.
  bool HelperArgs::operator<(const HelperArgs &x) const
  {
    int c = _name.compare(x._name);
    if (c == 0) {
      if (_has_args != x._has_args) {
        return !_has_args;
      }
      return lexicographical_compare(_args.begin(),_args.end(),x._args.begin(),x._args.end(),PtreeCmp());
    } else {
      return c < 0;
    }
  }

  // Swap words in an RNumber.
  rnumber::RNumber swap_words(const rnumber::RNumber &x)
  {
    if (x.wordCount() > 1) {
      // Bit of a hack here, to get to the underlying buffer so that we can
      // change it.
      RNumber res(x);
      size_t wc = res.wordCount();
      uint32_t *buf = (uint32_t*)res.buffer();
      for (size_t i=0; i < wc; ++i) {
        buf[i] = x.buffer()[wc-i-1];
      }
      if (x.size() % 32) {
        int adjust = (32 * wc) -  res.size();
        buf[wc-1] <<= adjust;
        res >>= adjust;
      }
      return res;
    } else {
      return x;
    }
  }
  
}

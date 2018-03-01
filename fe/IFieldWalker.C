#include "opencxx/parser/Leaf.h"
#include "opencxx/parser/token-names.h"
#include "opencxx/parser/ptree-generated.h"
#include "opencxx/Environment.h"
#include "opencxx/Bind.h"
#include "opencxx/TypeInfo.h"
#include "opencxx/PtreeIter.h"
#include "opencxx/Class.h"
#include "opencxx/BindEnumName.h"

#include "helpers/Macros.h"

#include "Data.h"
#include "Helpers.h"
#include "IFieldWalker.h"
#include "FieldSupport.h"

using namespace std;
using namespace Opencxx;
using namespace PtreeUtil;

namespace adl {

  const InstrField *IFieldWalker::is_instrfield(Ptree *p) const 
  {
    gc_string name = p->ToString();
    auto i = _field.instrfields().find(name);
    if (i != _field.instrfields().end()) {
      return i->second;
    }
    return 0;
  }

  IFieldWalker::IFieldWalker(Environment *e,const InstrField &f,unsigned size) : Walker(e), _field(f), _size(size) {}
  
  Ptree *IFieldWalker::TranslateAssign(Opencxx::Ptree *p) 
  {  
    Ptree *left = First(p);
    Ptree *right = Translate(Third(p));
    
    if (Match(left,"ThisField")) {
      unsigned  v;
      if (get_uint(v,right,env)) {
        return new PtreeReturnStatement(Ptree::Make("return"),Rest(Rest(p)));
      } else {
        return Ptree::Make("return %p.uint32();",right);
        //return new PtreeReturnStatement(Ptree::Make("return"),Ptree::Make("(%p).uint%d()",right,_size));
      }
    }
    return Walker::TranslateReturn(p);
  }
  
  Ptree *IFieldWalker::TranslateFuncall(Ptree *p) {
    Ptree *fun = p->Car();
    if (Eq(fun,"bits")) {
      Ptree *args  = p->Cdr();
      Ptree *args2 = TranslateArguments(args);
      args2 = Second(args2);
      Ptree *arg1 = First(args2);
      Ptree *arg2 = Third(args2);
      arg2 = (!arg2 ? arg1 : arg2);
      return Ptree::Make("z0.get(%p,%p)",arg1,arg2);
    } 
    const InstrField *f = is_instrfield(fun);
    //    if (const InstrField *f = is_instrfield(fun)) {
    if (f) {
      Ptree *args  = p->Cdr();
      Ptree *args2 = TranslateArguments(args);
      return Ptree::Make("%s(z0,%p)",makeInstrFldTblAccessName(*f,&_field).c_str(),args2);
    }
    
    return Walker::TranslateFuncall(p);
  }
}

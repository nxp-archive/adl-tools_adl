
#ifndef _IFIELDWALKER_H_
#define _IFIELDWALKER_H_

#include "opencxx/Walker.h"


namespace adl {

  class InstrField;

  // This walks over an instruction fields action code and handle its return value and
  // 

class IFieldWalker : public Opencxx::Walker {
  public :
    IFieldWalker(Opencxx::Environment *e, const InstrField &f, unsigned size);
    virtual ~IFieldWalker() {};

    virtual Opencxx::Ptree *TranslateAssign(Opencxx::Ptree *p);
    virtual Opencxx::Ptree *TranslateFuncall(Opencxx::Ptree *p);

 private:
    const InstrField *is_instrfield(Opencxx::Ptree *p) const;

    const InstrField &_field;
    unsigned         _size;
  };


}



#endif

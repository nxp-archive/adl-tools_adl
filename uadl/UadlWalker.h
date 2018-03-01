//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Code transformer for uadl instruction-class code.
//

#ifndef _UADLWALKER_H_
#define _UADLWALKER_H_

#include <string>

#include "helpers/gc_hash_map.h"
#include "helpers/RtlWalker.h"

#include "fe/Types.h"

using namespace std;
using namespace adl;

namespace uadl {

  class UadlIr;
  class InstClass;

  gc_string getStateChangeName(const gc_string &from,const gc_string &to);

  class UadlWalker : public RtlWalker {
  public:
    UadlWalker(Opencxx::Environment *e,bool ld_okay,bool safeMode,const UadlIr &ir,const gc_string &iss_type);

    void set_cur_state(const gc_string &cur_state) { _cur_state = cur_state; };
    void set_class(const InstClass &ic);
    void set_can_halt(bool h) { _can_halt = h; };
    void set_dealloc(PList *a) { _deallocs = a; };
    
    virtual Opencxx::Ptree* TranslateFuncall(Opencxx::Ptree* exp);
    virtual Opencxx::Ptree *TranslateRegWrite(PtreeRegWrite *);
    virtual Opencxx::Ptree *TranslateRegRead(PtreeRegRead *);
    virtual Opencxx::Ptree *TranslateMemWrite(PtreeMemWrite *);
    virtual Opencxx::Ptree *TranslateMemRead(PtreeMemRead *);
    virtual Opencxx::Ptree *TranslateSubInstrCall(PtreeSubInstrCall *);
    virtual Opencxx::Ptree* TranslateStmts(Opencxx::Ptree*);
    virtual Opencxx::Ptree* TranslateVariable(Opencxx::Ptree*);
    virtual Opencxx::Ptree* TranslatePtree(Opencxx::Ptree*);

  protected:
    bool is_mgr(Opencxx::Ptree *x);
    Opencxx::Ptree *checkForInstrClass(Opencxx::Ptree *v);
    Opencxx::Ptree *checkForInstrClassAttr(Opencxx::Ptree *v);
    Opencxx::Ptree *checkForResource(Opencxx::Ptree *v);
    Opencxx::Ptree *addExceptionCheck(Opencxx::Ptree *r);
    gc_string getStageName();
    Opencxx::Ptree *optionalArgs(Opencxx::PtreeArray &args);

    const UadlIr         &_ir;
    const InstClass      *_ic;
    Opencxx::Environment *_core_env;
    PList                *_deallocs;
    gc_string             _iss_type;
    gc_string             _cur_state;
    bool                  _can_halt;
    bool                  _safeMode;
    StrHashSet            _instr_class_enums;
  };

}

#endif

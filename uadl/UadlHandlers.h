//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Handler classes for the uADL parser.
//

#ifndef _UADLHANDLERS_H_
#define _UADLHANDLERS_H_

#include "fe/Handler.h"

namespace adl {
  class MsgLog;
}

namespace uadl {
  
  class UadlIr;

  HandlerList(CoreHandler);

  struct IrHandler : public adl::RemoveHandler {
    IrHandler(Opencxx::Environment *e,Opencxx::Ptree *p,const gc_string &n,bool modify,UadlIr *ir);

    UadlIr *_ir;
  };

  // Outer-most handler class.
  class OuterHandler : public IrHandler {
  public:
    OuterHandler(Opencxx::Environment *e,Opencxx::Ptree *p,const gc_string &n,bool modify,UadlIr *ir);

    virtual adl::Handler *add_define(adl::MsgLog &ml,Opencxx::Environment *e,Opencxx::Ptree *p,const char *type,const char *name,bool modify);

    virtual void process_keys(adl::MsgLog &);

  protected:
    CoreHandlers  _cores;
  };

}

#endif

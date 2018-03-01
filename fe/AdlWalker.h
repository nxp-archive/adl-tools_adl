//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This is the visitor which walks the initial input.
//

#ifndef _ADLWALKER_H_
#define _ADLWALKER_H_

#include "opencxx/ClassWalker.h"

#include "helpers/gc_list.h"

#include "Types.h"

namespace adl {

class Handler;
class BaseParser;
class MsgLog;

// The walker class performs the first pass, creating the tree of Handler
// objects.  Note:  It's okay to call get_outer(), then destroy this object
// because the Handler objects are garbage collected.
class AdlWalker : public Opencxx::Walker {
public:
  AdlWalker(Opencxx::Parser *,BaseParser &,MsgLog &ml);

  void setFullPathErrors(bool fp) { _fullpath = fp; };

  virtual Opencxx::Ptree* TranslateUserPlain(Opencxx::Ptree*);

  virtual Opencxx::Ptree* TranslateEnumSpec(Opencxx::Ptree*);

  // Return a pointer to the outer-most scope handler object.
  Handler *get_outer() { return _outer; };

  // Access the walker's environment.
  Opencxx::Environment *get_env() { return env; };

protected:
  void fatal(const char *msg,Opencxx::Ptree *item,Opencxx::Ptree *where);
  void error(const char *msg,Opencxx::Ptree *item,Opencxx::Ptree *where);
  void warning(const char *msg,Opencxx::Ptree *item,Opencxx::Ptree *where);

  void translateDefine(Opencxx::Ptree* keyword,Opencxx::Ptree* rest);

  void handleDefineBlock(Opencxx::Ptree *name,Opencxx::Ptree *type,const char *type_str,
                         bool is_modify,Opencxx::Ptree *body);
  void handleDefine(Opencxx::Ptree *loc,const char *type,const char *name,
                    bool is_modify,Opencxx::Ptree *body);
  void handleDefine(Opencxx::Ptree *loc,const char *type,unsigned name,
                    bool is_modify,Opencxx::Ptree *body);
    
  void handleGroupDefine(Opencxx::Ptree *loc,const char *type,const char *name,
                         Opencxx::Ptree *body);

    
    
  void walk_body(Opencxx::Ptree *body);

  void add_decl(Opencxx::Ptree *elem,Opencxx::Ptree *body,bool no_decls);

  Handler &handler() { assert(!_handlers.empty()); return *(_handlers.back()); };

  typedef gc_list<Handler *> Handlers;
  
  Handlers      _handlers;
  Handler      *_outer;
  bool          _fullpath;
  MsgLog       &_ml;
};

}

#endif

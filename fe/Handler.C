//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Handler classes encapsulate the logic about what can be nested within what
// when parsing a description.  This defines the interface.  The basic idea is
// that the handlers are added to a stack and the top item's various methods are
// called as the parser encounters various items.  Errors are handled via
// exception throws.  Each handler has a reference to the AdlData object and
// should add data to that object as appropriate.
//

#include <iostream>

#include "opencxx/parser/Ptree.h"

#include "helpers/fe_exceptions.h"
#include "helpers/Macros.h"
#include "helpers/StringUtils.h"

#include "Helpers.h"
#include "Handler.h"
#include "MsgLog.h"

using namespace std;
using namespace Opencxx;

namespace adl {

  // Called when a new define block is encountered.  This should return
  // a new handler object to process this block, or throw an error if the
  // new block is illegal.  The object will be popped automatically when
  // the block is exited.  It is illegal to return a null value.
  Handler *Handler::add_define(MsgLog &ml,Environment *e,Ptree *p,const char *type,const char *name,bool modify)
  {
    if (modify) {
      throw parse_error(p,string("A modify-block of type '")+type+"' (named "+name+") is not allowed within this block.");
    } else {
      throw parse_error(p,string("A define-block of type '")+type+"' (named "+name+") is not allowed within this block.");
    }
  }

  Handler *Handler::add_define(MsgLog &ml,Environment *e,Ptree *p,const char *type,unsigned id,bool modify)
  {
    if (modify) {
      throw parse_error(p,string("A modify-block of type '")+type+"' (id "+itos(id)+") is not allowed within this block.");
    } else {
      throw parse_error(p,string("A define-block of type '")+type+"' (id "+itos(id)+") is not allowed within this block.");
    }
  }

  
  Handler *Handler::add_defines(MsgLog &ml,Opencxx::Environment *e,Opencxx::Ptree *p,const char *type,const char *name) {
    throw parse_error(p,string("Groups are not allowed within this block."));
  }


  // This is called when a documentation string is encountered.
  void Handler::add_doc_string(Ptree *p)
  {
    throw parse_error(p,"A documentation string is not allowed in this object.");
  }

  // This is called when a new key/value pair is encountered.  
  void Handler::add_key(const char *key,Opencxx::Ptree *value)
  {
    _keypairs.push_back(make_pair(key,value));
  }

  // This is called when a new type/name declaration is encountered.  
  void Handler::add_decl(const char *type,Opencxx::Ptree *name)
  {
    _declpairs.push_back(make_pair(type,name));
  }

  // This is called when a nested function is encountered.
  void Handler::add_func(const char *name,Opencxx::Ptree *func)
  {
    throw parse_error(func,"A nested function is not allowed here.");
  }

  // Called when a type declaration is encountered.
  void Handler::add_type_decl(Opencxx::Ptree *tdecl)
  {
    throw parse_error(tdecl,"A nested type-declaration is not allowed here.");
  }

  // Called when a remvoe command is encountered.  This stores the name of the item to remove.
  void Handler::add_remove(const char *name)
  {
    _removes.insert(name);
  }

  // Called for each stored key/value pair.
  void Handler::handle_key(const char *key,Opencxx::Ptree *value)
  {
    throw parse_error(value,string("The key '")+key+"' is not allowed here.");
  }

  // Called for each stored type/name decl.
  void Handler::handle_decl(const char *type,Opencxx::Ptree *name)
  {
    throw parse_error(name,string("The declaration '")+type+"' is not allowed here.");
  }

  bool Handler::remove_key(KeyPairs &keys,const RemoveSet &removes)
  {
    bool found = false;
    auto iter = keys.begin();
    while ( iter != keys.end() ) {
      if (removes.count(iter->first)) {
        auto tmp = iter++;
        keys.erase(tmp);
        found = true;
      } else {
        ++iter;
      }
    }
    return found;
  }

  // If this is a declaration that's a remove command, then we process it and return true,
  // else we return false.
  void Handler::handle_removes(const RemoveSet &removes)
  {
    // First, see if it's a key and remove it if it is.
    if (!remove_key(_keypairs,removes)) {
      remove_key(_declpairs,removes);
    }

  }

  void Handler::process_keys(MsgLog &log)
  {
    ForEach(_keypairs,i) {
      try {
        handle_key(i->first,i->second);
      }
      catch (parse_error &err) {
        if (log.pass_exceptions()) {
          throw;
        } else {
          log.add(err);
        }
      }
    }
    ForEach(_declpairs,i) {
      try {
        handle_decl(i->first,i->second);
      }
      catch (parse_error &err) {
        if (log.pass_exceptions()) {
          throw;
        } else {
          log.add(err);
        }
      }
    }
  }


  //
  // DocHandler: Adds doc-string support to a handler.
  //

  void DocHandler::add_doc_string(Ptree *p)
  {
    const char *str;
    if (!(str = get_str(p,0))) {
      PError(p,"Expected a string.");
    }
    // Eat leading newlines.
    int count = 0;
    while (str[count] == '\n') ++count;
    _doc += &str[count];
  }

  //
  // RemoveHandler:  Handles remove operations.
  //

  RemoveHandler::RemoveHandler(Environment *e,Ptree *p,const gc_string &n,bool modify) : 
    DocHandler(e,p), 
    _name(n) 
  {
    _modify = modify;
  }

  void RemoveHandler::add_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"remove")) {
      _remove = get_bool(value,"remove flag",env());
    } else {
      DocHandler::add_key(key,value);
    }
  }

  IntRemoveHandler::IntRemoveHandler(Environment *e,Ptree *p,unsigned id,bool modify) : 
    DocHandler(e,p), 
    _id(id) 
  {
    _modify = modify;
  }

  void IntRemoveHandler::add_key(const char *key,Ptree *value)
  {
    if (!strcmp(key,"remove")) {
      _remove = get_bool(value,"remove flag",env());
    } else {
      DocHandler::add_key(key,value);
    }
  }


  void check_ident(const char *ident,Ptree *loc,const char *msg,bool valid_ident)
  {
    if (!ident) {
      PError(loc,msg << " defined without a name.");
    }
    if (valid_ident && !is_ident(ident)) {
      PError(loc,msg << " name must be a valid C identifier.");
    }
  }

  unsigned check_int(const char *ident,Ptree *loc,const char *msg)
  {
    if (!ident) {
      PError(loc,msg << " defined without a name.");
    }
    char *endptr;
    unsigned v = strtol(ident,&endptr,0);
  
    if (*endptr != 0) {
      PError(loc,msg << " name must be an integer value.");
    }
    return v;
  }

  // Eventually, this should be hooked up so that we can turn on
  // or off warnings.
  static bool UnknownOkay = false;
  
  void unknown_define(Ptree *p,const char *type,const char *name)
  {
    if (name) {
      PError(p,"Unknown type encountered:  " << type << "(" << name << ")");
    } else {
      PError(p,"Unknown type encountered:  " << type);
    }
  }

  void unknown_define(Ptree *p,const char *type,unsigned id)
  {
    PError(p,"Unknown type encountered:  " << type << "(" << id << ")");
  }

  void unknown_key(Ptree *p,const char *key)
  {
    if (UnknownOkay) {
      PWarn(p,"Unknown key '" << key << "'");
    } else {
      PError(p,"Unknown key '" << key << "'");
    }
  }


}

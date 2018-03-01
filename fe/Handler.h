//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Handler classes encapsulate the logic about what can be nested within what
// when parsing a description.  This defines the interface.  The basic idea is
// that a tree of Handler objects is created, with basic error checking on what
// can be nested within what.  Then, once all architectures have been instantiated
// within a core, we do the final parse.
//

#ifndef _HANDLER_H_
#define _HANDLER_H_

#include "gc/gc_cpp.h"

#include "helpers/gc_string.h"
#include "helpers/gc_list.h"
#include "helpers/gc_hash_map.h"
#include "helpers/gc_hash_set.h"
#include "helpers/stringhash.h"

namespace Opencxx {
  class Ptree;
  class Environment;
}

// Use this to easily define lists of pointers to objects.
#define HandlerList(x) class x;                 \
    typedef gc_list<x *> x##s

namespace adl {

  class Data;
  class MsgLog;
  class Handler;
  class StrSet;
  typedef  gc_list<Handler *> Handlers;

  //
  // Helper functions.
  //

  // Check that a token is a valid identifier.
  void check_ident(const char *ident,Opencxx::Ptree *loc,const char *msg,bool valid_ident = true);
  // Check that a token is a valid integer.
  unsigned check_int(const char *ident,Opencxx::Ptree *loc,const char *msg);  

  // Reports a standard error for an unknown define/defmod block.
  void unknown_define(Opencxx::Ptree *p,const char *type,const char *name);
  void unknown_define(Opencxx::Ptree *p,const char *type,unsigned id);  

  // Standard error for an unkonwn key within a define/defmod block.
  void unknown_key(Opencxx::Ptree *p,const char *key);

  // The base class functionality for most functions is to throw an error, so a
  // derived class can simply not override a function if something is illegal.
  // This class throws parse_errror objects, which are derived from
  // std::exception.  In general, const char * are used for strings- these will be
  // 0 if empty.

  class Handler : public gc {
  public:
    
    Handler(Opencxx::Environment *env,Opencxx::Ptree *loc) :  _env(env), _loc(loc), _remove(false), _modify(false) {};
    virtual ~Handler() {};

    // Called when a new define block is encountered.  This should return
    // a new handler object to process this block, or throw an error if the
    // new block is illegal.  The object will be popped automatically when
    // the block is exited.  It is illegal to return a null value.
    // Two forms exist:  A name (rhs item in the define) may be either a string 
    // or an integer.
    
    virtual Handler *add_define(MsgLog &ml,Opencxx::Environment *e,Opencxx::Ptree *p,const char *type,const char *name,bool modify);
    virtual Handler *add_define(MsgLog &ml,Opencxx::Environment *e,Opencxx::Ptree *p,const char *type,unsigned id,bool modify);

    // This is called then group name is met in defmod block.
    virtual Handler *add_defines(MsgLog &ml,Opencxx::Environment *e,Opencxx::Ptree *p,const char *type,const char *name);

    // This is called when a documentation string is encountered.
    virtual void add_doc_string(Opencxx::Ptree *p);

    // Called for each key/value pair in this class during the second phase 
    // of parsing.
    virtual void handle_key(const char *key,Opencxx::Ptree *value);

    // Called for each type/name decl in this class during the second phase 
    // of parsing.
    virtual void handle_decl(const char *type,Opencxx::Ptree *name);

    // This is called when a new key/value pair is encountered- the data is
    // stored in the class.
    virtual void add_key(const char *key,Opencxx::Ptree *value);

    // This is called when a new declaration is encountered- the data is
    // stored in the class.
    virtual void add_decl(const char *type,Opencxx::Ptree *name);

    // This is called when a nested function is encountered.
    virtual void add_func(const char *name,Opencxx::Ptree *func);

    // Called when a type declaration is encountered.
    virtual void add_type_decl(Opencxx::Ptree *);

    // Called when a remvoe command is encountered.  This stores the name of the item to remove.
    virtual void add_remove(const char *name);

    typedef gc_hash_set<const char *,HashConstChar,CmpConstChar> RemoveSet;

    void handle_removes(const RemoveSet &);

    const RemoveSet &removes() const { return _removes; };

    // For handling group expansion.  By default, no support.
    virtual bool has_expansion() const { return false; };
    virtual void get_expansion(StrSet &) {};

    // This can be called to iterate through all key/value pairs, calling
    // handle_key for each item.
    virtual void process_keys(MsgLog &);

    bool remove() const { return _remove; };
    bool modify() const { return _modify; };

    Opencxx::Ptree *loc() const { return _loc; };
    Opencxx::Environment *env() const { return _env; };
  protected:

    Opencxx::Environment *_env;

    // Ptree object used for obtaining source location for errors.
    Opencxx::Ptree *_loc;

    // Storage of key/value pairs.
    typedef gc_list<std::pair<const char *,Opencxx::Ptree*> > KeyPairs;
    KeyPairs _keypairs;

    // Storage of declaration type/name pairs.
    KeyPairs _declpairs;  

    RemoveSet _removes;

    // True if we encounter a key of "remove".
    bool _remove;

    // True if this is a modify, false is a define.
    bool _modify;

  private:
    bool remove_key(KeyPairs &keys,const RemoveSet &removes);
  };

  // This is a handler that supports a doc string.
  class DocHandler : public Handler {
  public:
    DocHandler(Opencxx::Environment *e,Opencxx::Ptree *p) : Handler(e,p) {};

    const gc_string &doc() const { return _doc; };

    virtual void add_doc_string(Opencxx::Ptree *p);
  protected:
    gc_string _doc;
  };

  // This sets the _remove flag if it catches a key of "remove"
  // with a true value.
  class RemoveHandler : public DocHandler {
  public:
    RemoveHandler(Opencxx::Environment *e,Opencxx::Ptree *p,const gc_string &n,bool modify);

    const gc_string &name() const { return _name; };
    virtual void set_name(const gc_string &n) { _name = n; };

    virtual void add_key(const char *key,Opencxx::Ptree *value);  

  protected:
    gc_string  _name;
  };

  // A remove handler which contains an integer id.
  class IntRemoveHandler : public DocHandler {
  public:
    IntRemoveHandler(Opencxx::Environment *e,Opencxx::Ptree *p,unsigned id,bool modify);

    const unsigned name() const { return _id; };
    const unsigned id() const { return _id; };
    
    // Dummy to avoid compilation issues.
    virtual void set_name(const gc_string &n) { };

    virtual void add_key(const char *key,Opencxx::Ptree *value);  

  protected:
    unsigned  _id;
  };


}

#endif

//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// A simple, generic handler.  Each handler stores its own
// information (key/value pairs and declarations) plus a map
// of child nodes.  The child nodes are stored in a multi-map,
// ordered by type.
//

#include <iostream>
#include <sstream>

#include "opencxx/parser/Ptree.h"

#include "helpers/Macros.h"

#include "SimpleHandler.h"
#include "Helpers.h"

using namespace std;
using namespace Opencxx;

namespace adl {

  SimpleHandler::SimpleHandler(Environment *env,Ptree *loc,
                               const gc_string &type,const gc_string &name,
                               bool modify) :
    Handler(env,loc),
    _type(type),
    _name(name)
  {
    _modify = modify;
  }

  Handler *SimpleHandler::add_define(MsgLog &ml,Environment *e,Ptree *p,const char *type,
                                     const char *name,bool modify)
  {
    SimpleHandler *sh = new SimpleHandler(e,p,(type) ? type : "",(name) ? name : "",modify);
    _children.insert(sh);
    return sh;
  }

  void SimpleHandler::add_doc_string(Ptree *p)
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

  // Do nothing.
  void SimpleHandler::process_keys(MsgLog &)
  {
  }

  void SimpleHandler::dump(ostream &o,const gc_string &pfx) const
  {
    o << pfx << "Item:  " << _type << " " << _name << "\n";
    gc_string pfx2 = indent(pfx);
    gc_string pfx3 = indent(pfx2);
  
    if (!_keypairs.empty()) {
      o << pfx2 << "Keys:\n";
      ForEach(_keypairs,i) {
        o << pfx3 << i->first << " = " << i->second << "\n";
      }
    }

    if (!_declpairs.empty()) {
      o << pfx2 << "Declarations:\n";
      ForEach(_declpairs,i) {
        o << pfx3 << i->first << " " << i->second << "\n";
      }
    }

    if (!_children.empty()) {
      o << pfx2 << "Children:\n";
      ForEach(_children,i) {
        (*i)->dump(o,pfx3);
      }
    }

  }

  void SimpleHandler::dump() const
  {
    dump(cerr,"");
  }

  ostream &operator<< (ostream &o,const SimpleHandler *sh)
  {
    sh->dump(o,"");
    return o;
  }

}

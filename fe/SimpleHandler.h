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

#ifndef _SIMPLE_HANDLER_H_
#define _SIMPLE_HANDLER_H_

#include <iosfwd>

#include "helpers/gc_string.h"
#include "helpers/gc_set.h"
#include "helpers/fe_exceptions.h"

#include "Handler.h"

namespace adl {

  class SimpleHandler : public Handler {
  public:
    SimpleHandler(Opencxx::Environment *env,Opencxx::Ptree *loc,
                  const gc_string &type,const gc_string &name,
                  bool modify);

    virtual Handler *add_define(MsgLog &ml,Opencxx::Environment *e,Opencxx::Ptree *p,const char *type,const char *name,bool modify);
    virtual void add_doc_string(Opencxx::Ptree *p);
    virtual void process_keys(MsgLog &);

    const gc_string &name() const { return _name; };
    const gc_string &type() const { return _type; };
    const gc_string &doc() const { return _doc; };

    virtual void dump(std::ostream &,const gc_string &pfx) const;
    // This dumps to cerr.
    void dump() const;

  private:
    struct SHNameSort {
      bool operator()(const SimpleHandler *,const SimpleHandler *);
    };

    typedef gc_multiset<SimpleHandler *,SHNameSort> Children;

    gc_string _type;
    gc_string _name;
    gc_string _doc;

    Children    _children;
  };

  inline bool SimpleHandler::SHNameSort::operator()(const SimpleHandler *x,const SimpleHandler *y)
  {
    return (x->type() < y->type());
  }

  std::ostream &operator<< (std::ostream &,const SimpleHandler *);

}

#endif

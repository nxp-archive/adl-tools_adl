//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Miscellaneous helpers for parsing purposes.
//

#ifndef _PARSING_HELPERS_H_
#define _PARSING_HELPERS_H_

#include <sstream>
#include <iterator>
#include <utility>

#include "helpers/Macros.h"
#include "helpers/BasicTypes.h"

#include "MsgLog.h"

namespace Opencxx {
  class Leaf;
  class Walker;
  class PtreeArray;
  class Ptree;
}

namespace adl {

  bool DataStrictMode();
  bool DataWarnRedefineMode();
 
  // If the remove flag is set for h, remove from container c.
  // Returns true if the remove flag was set, false otherwise.
  template <class RH,class C>
  bool check_remove(MsgLog &log,RH &h,C &c)
  {
    if (h.remove()) {
      // Remove the item if the remove flag was set.
      typename C::iterator iter;
      if ( (iter = c.find(h.name())) != c.end()) {
        if (adl::DataStrictMode()) {
          AddWarning(log,h.loc(),"Item '" << h.name() << "' was removed.");
        }
        c.erase(iter);
      }
      return true;
    }
    return false;
  }

  // This searches back through a handler container, removing any keys which are
  // designated as to be removed (the remove list).  We iterate backwards so
  // that it it only affects things earlier on.  This allows the user to use a
  // defmod to remove a key, then later add another defmod which sets that key
  // again.  The result is that they key is removed, as expected, then added
  // back in again, as you would want.
  template <class HC>
  void check_for_remove_keys(HC &handler_container)
  {
    RevForEach(handler_container,iter) {
      // For each remove list, remove all items from this container and any other containers.
      auto const & removes = (*iter)->removes();
      if (!removes.empty()) {
        auto r_iter = iter;
        for ( ; r_iter != handler_container.rend(); ++r_iter) {
          (*r_iter)->handle_removes(removes);
        }
      }
    }
  }

  // Reports that an item was modified by defmod if '-strict' option was specified.
  template <class H> 
  void report_modify(MsgLog &log,const H &h,const char *type = 0) {
    if (!DataStrictMode()) {
      return;
    }
    if (type) {
      AddWarning(log,h.loc(), type << " '" << h.name() << "' was modified."); 
    } else {
      AddWarning(log,h.loc(), "Item '" << h.name() << "' was modified"); 
    }
  }
  
  // Generic processing routine for items.  Handles the remove flag,
  // removing the item from the container if it's set.  If this is a modify,
  // we grab the item and produce an error if it doesn't exist.  if it's a define,
  // we insert/replace it in the container.
  // The type must have a constructor that takes a location and name.
  // The container must be a hash that takes a string key and has a ptr to T as the value.
  // "outer" is the parent class- a method of its is called to handle extra setup.
  template <class T,class H,class C,class Outer>
  void process_items(MsgLog &log,H &handler,C &container,Outer &outer,void (Outer::*setup)(H&,T&))
  {
    try {
      if (!check_remove(log,handler,container)) {
        // Remove not set, so add the item if not a modify, otherwise, require
        // that it exist.  Then process all keys.
        T *item = 0;
        if (handler.modify()) {
          typename C::iterator iter;
          if ( (iter = container.find(handler.name())) == container.end()) {
            PError(handler.loc(),"Attempt to modify '" << handler.name() << "' which has not yet been defined.");
          } else {
            item = iter->second; 
            report_modify(log,handler);
          }
        } else {
          // This is a define, so we replace the object if it already exists.
          item = new T(handler.env(),handler.loc(),handler.name());
          // Warn if we're replacing anything, since this sometimes indicates a
          // subtle error, though it's perfectly legal, and sometimes very
          // useful, to do this.
          auto ip = container.insert(std::make_pair(handler.name(),item));
          if (!ip.second) {
            if (DataWarnRedefineMode()) {
              AddWarning(log,handler.loc(),"Item '" << handler.name() << "' was redefined.");
              AddWarning(log,ip.first->second->loc(),"    Previous definition was here.");
            }
            ip.first->second = item;
          }
        }
        (outer.*setup)(handler,*item);
        handler.process_keys(log);
      }
    }
    catch (parse_error &err) {
      if (log.pass_exceptions()) {
        throw;
      } else {
        log.add(err);
      }
    }
  }

  // Same as above, but any parse errors are passed on, rather than being stored
  // into a message log.
  template <class T,class H,class C,class Outer>
  void process_items(H &handler,C &container,Outer &outer,void (Outer::*setup)(H&,T&))
  {
    MsgLog tmplog;
    tmplog.set_pass(true);
    process_items(tmplog,handler,container,outer,setup);
  }

  template <class H,class HC, class HI>
  HI process_expansion(H &handler,HC &handler_container,HI iter)
  {
    StrSet new_items;
    handler.get_expansion(new_items);
    // We want to insert after this item, so that we can skip this item.
    HI orig = iter;
    ++iter;
    ForEach(new_items,i) {
      H *new_handler = new H(handler);
      new_handler->set_name(*i);
      handler_container.insert(iter,new_handler);
    }
    // Then remove the expansion item.
    HI tmp = orig++;
    handler_container.erase(tmp);
    return orig;
  }
    
  // Processes items which have names and are inserted into hashes
  // in the destination core object.
  template <class HC,class DC,class H,class T,class Outer>
  void process_named_handler(MsgLog &log,HC &handler_container,
                             DC &dest_container,Outer &outer,
                             void (Outer::*setup)(H&,T&))
  {    
    check_for_remove_keys(handler_container);
    for (typename HC::iterator i = handler_container.begin(); i != handler_container.end(); ) {
      if ( (*i)->has_expansion()) {
        i = process_expansion(*(*i),handler_container,i);
      } else { 
        process_items(log,*(*i),dest_container,outer,setup);
        ++i;
      }
    }
  }
  
  // Processes unnamed items which therefore do not accept a remove key.
  // The setup function is responsible for creating the required data object
  // and adding it to the relevant data structure in the parent data object.
  template <class HC,class H,class Outer>
  void process_unnamed_handler(MsgLog &log,HC &handler_container,
                               Outer &outer,void (Outer::*setup)(H&))
  {
    check_for_remove_keys(handler_container);
    ForEach(handler_container,i) {
      H &handler = *(*i);
      if ( handler.remove() ) { 
        PError(handler.loc(),"This type does not accept a remove parameter.");
      }
      (outer.*setup)(handler);
      handler.process_keys(log);
    }
  }

  // Calls a callable type for each item if it gets a flat list, or else calls
  // the member function on the value.
  template <typename C>
  void handle_list_or_item(Opencxx::Ptree *v,C c)
  {
    Opencxx::Ptree *l;
    if ( (l = get_flat_list(v)) ) {
      Opencxx::PtreeIter iter(l);
      Opencxx::Ptree *p;
      while ( (p = iter())) {
        c(p);
      }
    } else {
      c(v);
    }
  }

}

#endif

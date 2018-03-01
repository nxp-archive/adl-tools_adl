//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
//
// Various helper functions for IssNode manipulation.
//

#include <string>
#include <sstream>
#include <stdexcept>
#include <assert.h>

#include "helpers/Macros.h"

#include "ModelInterface.h"
#include "StringHelpers.h"
#include "NodeHelpers.h"

using namespace std;

namespace adl {

  // Examines a node's context introspection data to see if a thread context
  // exists.  This is identified by the Data model as a context which contains
  // the cia.
  const CtxInfo *getThreadContext(const IssNode &node)
  {
    const CtxInfos &info = node.getContextInfo();

    ForRange(info.size(),l_iter) {
      const CtxInfo &ci = info[l_iter];
      if (ci._is_thread) {
        return &ci;
      }
    }
    return 0;
  }

  string add_to_path(const string &parent_path,const string &name,const string &delim)
  {
    string path = parent_path;
    if (!path.empty()) {
      path += delim;
    }
    path += name;
    return path;
  }

  string add_to_path(const string &parent_path,const IssNode &node,const string &delim)
  {
    return add_to_path(parent_path,node.name(),delim);
  }

  void get_path_internal(string &path,const IssNode *node,const string &delim,int size)
  {
    if (!node) {
      path.reserve(size);
    } else {
      get_path_internal(path,node->parent(),delim,size+delim.size()+node->name().size());
      if (node->parent()) {
        path += delim;
      }
      path += node->name();
    }
  }

  string getPathName(const IssNode *node,const string &delim)
  {
    int size = 0;
    string result;
    get_path_internal(result,node,delim,size);
    return result;
  }

  // Return a target of the parent.
  Target Target::parent() const
  {
    Target p = *this;
    if (p.has_ctx()) {
      // If in a context, then exit the context by removing context information.
      p.remove_ctx();
    } else {
      if (p._node) {
        // Otherwise, if we have a node, go to the parent.
        p._node = const_cast<IssNode*>(p._node->parent());
      }
    }
    return p;
  }


  // Sets the specified node's debug context to the value stored here, if there is
  // a valid debug context.
  void Target::setDbgCtx()
  {
    if (_ctx_id >= 0 && _ctx_num >= 0) {
      _node->setDbgCtx(_ctx_id,_ctx_num);
    }
  }

  CtxState::CtxState(const Target &t,bool modify_active) : 
    _trg(t),
    _old_value(-1),
    _old_active_value(-1),
    _modify_active(modify_active)
  {
    assert(_trg.node());
    if (_trg.ctx_id() >= 0) {
      _old_value = _trg.node()->setDbgCtx(_trg.ctx_id(),_trg.ctx_num());
      if (_modify_active) {
        _old_active_value = _trg.node()->getActiveCtx(_trg.ctx_id());
        _trg.node()->setActiveCtxToDbg(_trg.ctx_id());
      }
    }
  }

  CtxState::~CtxState() 
  {
    if (_old_value >= 0) {
      if (_modify_active) {
        _trg.node()->setDbgCtx(_trg.ctx_id(),_old_active_value);
        _trg.node()->setActiveCtxToDbg(_trg.ctx_id());
      }
      _trg.node()->setDbgCtx(_trg.ctx_id(),_old_value);
    }
  }

  // Parses a path and does error checking to ensure it's correct.  Returns a Target
  // object containing the target node and any context info.
  // root:          Root from which to start traversing.
  // path:          Path to parse
  // cur:           If nothing present except a final data item, then this is the node selected.
  // last_is_data:  If true, last element is considered a name value for use by a command.
  //                False:  Parse the entire path.
  pair<Target,string> parsePath(IssNode *root,const string &pathstr,const Target &cur,bool last_is_data)
  {
    Args path = splitPath(pathstr);

    pair<Target,string> retvalue;

    if (path.empty()) {
      retvalue.first = cur;
      return retvalue;
    }

    if (last_is_data) {
      retvalue.second = path.back();
      path.pop_back();
    }

    // Do we have anything left?  If not, then we don't have a real path, so return the current target.
    if (path.empty()) {
      retvalue.first = cur;
      return retvalue;
    }

    IssNode *node = root;

    auto iter = path.begin();
    if (*iter != node->name()) {
      RError("Root name '" << node->name() << "' does not match first path element of '" << *iter << "'.");
    }

    for ( ++iter ; iter != path.end(); ++iter) {
      if ( node->is_core()  ) {
        // If we're at a core level and we have items left, then the last element must be a context.
        // We have to remove the trailing context number identifier.
        size_t n = iter->find_last_not_of("0123456789");
        if (n == string::npos) {
          RError("Invalid context-identifier at end of path.  Must be of the form <context-name><integer id>, e.g. thread0");
        }
        // If this throws an exception, then we know that we have an invalid
        // context name.  Otherwise, it won't cause any side-effects and we'll
        // know we have a valid name.
        try {
          retvalue.first.set_ctx_id( node->getCtxId(iter->substr(0,n+1)) );
          retvalue.first.set_ctx_num( strtoul(iter->substr(n+1).c_str(),0,0) );
        }
        catch (const exception &) {
          RError("Unknown path element:  '" << *iter << "'.");
        }
      } else {
        // We have a system, so scan the children for the next path element.
        const IssChildren &children = node->children();
        ForEach(children,citer) {
          if ( (*citer)->name() == *iter) {
            node = (*citer);
            goto End;
          }
        }
        // Didn't match a child node.
        RError("Unknown path element:  '" << *iter << "'.");
      End:
        // Multi-level break for finding a child.
        ;
      }
    }

    retvalue.first.set_node( node );
    return retvalue;
  }

}

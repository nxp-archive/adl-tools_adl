//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
//
// Various helper functions for IssNode manipulation.
//

#ifndef _NODE_HELPERS_
#define _NODE_HELPERS_

#include <utility>

#include "helpers/stringfwd.h"

namespace adl {

  struct IssNode;
  struct CtxInfo;

  const std::string PathDelim = ":";

  // Describes the target of a path, which can consist of a node as well as a context.
  class Target {
  public:
    Target() : _node(0), _ctx_id(-1), _ctx_num(-1) {};
    Target(IssNode *n) : _node(n), _ctx_id(-1), _ctx_num(-1) {};
    Target(IssNode *n,int id,int num) : _node(n), _ctx_id(id), _ctx_num(num) {};

    IssNode *node() const { return _node; };
    void set_node(IssNode *n) { _node = n; };

    int      ctx_id() const { return _ctx_id; };
    void set_ctx_id(int id) { _ctx_id = id; };

    int      ctx_num() const { return _ctx_num; };
    void set_ctx_num(int num) { _ctx_num = num; };

    bool has_ctx() const { return _ctx_id >= 0; };
    void set_ctx(int id,int num) { _ctx_id = id; _ctx_num = num; };
    void remove_ctx() { _ctx_id = _ctx_num = -1; };

    bool operator==(const Target &x) const {
      return _node == x._node && _ctx_id == x._ctx_id && _ctx_num == x._ctx_num;
    }
    bool operator!=(const Target &x) const {
      return !operator==(x);
    }

    // Return a target of the parent.
    Target parent() const;

    void reset(IssNode *n = 0) {
      _node = n;
      _ctx_id = -1;
      _ctx_num = -1;
    }

    // Sets the specified node's debug context to the value stored here.
    void setDbgCtx();

  protected:
    IssNode     *_node;
    int          _ctx_id;
    int          _ctx_num;
  };

  struct CTarget : private Target {
    CTarget() : Target() {};
    CTarget(const IssNode *n) : Target(const_cast<IssNode*>(n)) {};
    CTarget(IssNode *n,int id,int num) : Target(n,id,num) {};

    const IssNode *node() const { return _node; };
    void set_node(const IssNode *n) { _node = const_cast<IssNode*>(n); };

    using Target::has_ctx;
    using Target::set_ctx;
    using Target::set_ctx_id;
    using Target::set_ctx_num;
    using Target::remove_ctx;
    using Target::parent;
    using Target::reset;
    using Target::ctx_id;
    using Target::ctx_num;
    bool operator==(const CTarget &x) { return Target::operator==(x); };
    bool operator!=(const CTarget &x) { return Target::operator!=(x); };
  };

  // This is a 'Resource Acquisition is Initialization' idiom class
  // for setting a context state on an item.  It sets the state, then the destructor
  // restores that state, thus making the use of the object exception safe.
  struct CtxState {
    CtxState(const Target &t,bool modify_active = false);
    ~CtxState();

  private:
    Target _trg;
    int    _old_value;
    int    _old_active_value;
    bool   _modify_active;
  };

  // Parses a path and does error checking to ensure it's correct.  Returns a Target
  // object containing the target node and any context info.
  // root:          Root form which to start traversing.
  // path:          Path to parse
  // cur:           If nothing present except a final data item, then this is the node selected.
  //                Thus, supplying a value for cur is really only required if last_is_data is true.
  // last_is_data:  If true, last element is considered a name value for use by a command.
  //                False:  Parse the entire path.
  std::pair<Target,std::string> parsePath(IssNode *root,const std::string &pathstr,const Target &cur = Target(),bool last_is_data = false);

  // This is a simple routine which returns a string representing a path,
  // given a node pointer.  It walks back up the hierarchy, calculating the path.
  std::string getPathName(const IssNode *node,const std::string &delim);

  // Examines a node's context introspection data to see if a thread context
  // exists.  This is identified, currently, by the data model as a context
  // which contains the cia.
  const CtxInfo *getThreadContext(const IssNode &node);

  // Helper routine for building node paths.
  std::string add_to_path(const std::string &parent_path,const IssNode &node,const std::string &delim);
  std::string add_to_path(const std::string &parent_path,const std::string &name,const std::string &delim);

}

#endif

//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#include "helpers/CodeGen.h"
#include "helpers/Macros.h"

#include "fe/Data.h"

#include "UadlCommon.h"
#include "UadlParser.h"

using namespace std;
using namespace adl;

namespace uadl {

  const char *PreCycleName = "preCycle";

  const char *PostCycleName = "postCycle";

  const char *PostSimName = "postSim";

  ostream &operator<< (ostream &o,const ConfigItem &ci)
  {
    if (ci.is_str()) {
      o << ci._str;
    } else {
      o << ci._val;
    }
    return o;
  }

  void ConfigMap::add(const gc_string &key,const char *val,bool builtin)
  {
    insert(make_pair(key,ConfigItem(gc_string(val),builtin)));
  }

  void ConfigMap::add(const gc_string &key,const gc_string &val,bool builtin)
  {
    insert(make_pair(key,ConfigItem(val,builtin)));
  }

  void ConfigMap::add(const gc_string &key,uint32_t val,bool builtin)
  {
    insert(make_pair(key,ConfigItem(val,builtin)));
  }

  void ConfigMap::add(const gc_string &key,bool val,bool builtin)
  {
    insert(make_pair(key,ConfigItem(val,builtin)));
  }

  const ConfigItem *ConfigMap::find(const gc_string &key) const
  {
    auto x = Base::find(key);
    if (x != end()) {
      return &(x->second);
    } else {
      return 0;
    }
  }

  const gc_string &getUadlNamespace()
  {
    static gc_string n = "uadl";
    return n;
  }

  gc_string getIssNamespace(const gc_string &nameSpace)
  { 
    static gc_string n = "adliss"; 
    return (nameSpace.empty() || nameSpace == getUadlNamespace()) ? n : (nameSpace + "_iss");
  }

  static gc_string getIssClassNameInternal(const Thread *thread)
  {
    return (!thread) ? "ISS" : (getIssClassNameInternal(thread->parent()) + "_" + thread->item().name());
  }

  gc_string getIssClassName(const Thread &thread)
  {
    return getIssClassNameInternal(&thread);
  }

  gc_string getuArchClassName(const Thread &t)
  {
    return "OSM_" + t.item().name();
  }

  gc_string getuArchClassName(const UadlIr &ir)
  {
    return getuArchClassName(ir.thread());
  }

  gc_string getInstBaseName(const UadlIr &ir)
  {
    return "Inst_" + ir.uArchName();
  }

  gc_string getInstAllocName(const UadlIr &ir)
  {
    return "Inst_" + ir.uArchName() + "_Alloc";
  }

  gc_string getInstClassName(const InstClass &ic)
  {
    return "Inst_" + ic.name();
  }

  gc_string getFetchName(const FetchUnit &fetch)
  {
    return fetch.name();
  }

  gc_string getInstrOpName(const gc_string &iname,const gc_string &icname,const gc_string &oname)
  {
    return makeFriendlyName(iname) + "_" + icname + "_" + oname;
  }

  gc_string getInstrOpBaseName(const gc_string &icname,const gc_string &oname)
  {
    return icname + "_" + oname;
  }

  const gc_string &getDummyOpName()
  {
    static gc_string n = "DummyOp";
    return n;
  }

  gc_string getRegName(const RegBase &rb)
  {
    return rb.name();
  }

  gc_string getRegRead(const adl::RegBase &rb,const adl::RegField *rf)
  {
    return "_iss->" + rb.name() + ((rf) ? ("_" + rf->name()) : "") + "_read()";
  }

  gc_string getSparseType(const RegFile &rf)
  {
    return rf.name() + "_t";
  }

  gc_string getMaskType(int width)
  {
    return ((width > 32) ? "uint64_t" : "uint32_t");
  }

  gc_string getMaskType(const RegBase &rb)
  {
    return ((rb.width() > 32) ? "uint64_t" : "uint32_t");
  }

  gc_string getOpType(const gc_string &op)
  {
    return op + "_t";
  }

  gc_string getOpParm(const gc_string &op)
  {
    return op + "_p";
  }

  gc_string getOpVar(const gc_string &op)
  {
    return op + "_v";
  }

  gc_string getForwardingPathType(const UadlIr &ir,bool safeMode,const ForwardingPath &fp)
  {
    gc_ostringstream ss;
    
    ss << "ForwardingPath<" << getInstBaseName(ir) << "," << safeMode << ","
       << fp.broadcast() << "," << fp.size() << "," << fp.width() << ">";
    return ss.str();
  }

  // Returns true if any pipeline stages have the shared attribute.
  bool has_shared_stages(const UadlIr &ir)
  {
    ForEach(ir.sortedStages(),i) {
      if ( (*i)->shared() ) {
        return true;
      }
    }
    return false;
  }

  // Returns true if an instruction class contains a value-query method call.
  bool has_value_query(const InstClass &ic)
  {
    return ic.check_func_call("value");
  }

  const gc_string &getIssueName()
  {
    static gc_string n = "Issue";
    return n;
  }

}

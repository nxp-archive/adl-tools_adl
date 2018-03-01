//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef _UADL_COMMON_H
#define _UADL_COMMON_H

#include <ctype.h>
#include <sstream>

#include "helpers/BasicTypes.h"
#include "helpers/gc_string.h"
#include "helpers/gc_map.h"

namespace adl {
  class RegBase;
  class RegField;
  class RegFile;
};

namespace uadl {

  class UadlIr;
  class Thread;
  class InstClass;
  class FetchUnit;
  class ForwardingPath;

  extern const char *PreCycleName;
  extern const char *PostCycleName;
  extern const char *PostSimName;

  enum TriBool { tFalse, tTrue, tNotSet };

  inline TriBool bool2tb(bool b) { return (b) ? tTrue : tFalse; };

  // This class is used for configuring the cache model, or a user's custom
  // cache model.
  struct ConfigItem {
    gc_string _str;
    int       _val;
    bool      _builtin;

    ConfigItem(const gc_string &str,bool b) : _str(str), _val(-1), _builtin(b) {};
    ConfigItem(int val,bool b) : _val(val), _builtin(b) {};

    const gc_string &str() const { return _str; };
    int val() const { return _val; };

    bool is_str() const { return !_str.empty(); };
    bool is_int() const { return _str.empty(); };
  };

  std::ostream &operator<< (std::ostream &,const ConfigItem &);

  struct ConfigMap : public gc_map<gc_string,ConfigItem>
  {
    typedef gc_map<gc_string,ConfigItem> Base;

    void add(const gc_string &key,const char *val,bool builtin = true);
    void add(const gc_string &key,const gc_string &val,bool builtin = true);
    void add(const gc_string &key,uint32_t val,bool builtin = true);
    void add(const gc_string &key,bool val,bool builtin = true);

    const ConfigItem *find(const gc_string &key) const;
  };

  const gc_string &getUadlNamespace();

  gc_string getIssNamespace(const gc_string &nameSpace);

  gc_string getIssClassName(const Thread &ir);

  gc_string getuArchClassName(const UadlIr &ir);
  gc_string getuArchClassName(const Thread &thread);

  gc_string getInstBaseName(const UadlIr &ir);

  gc_string getInstAllocName(const UadlIr &ir);

  gc_string getInstClassName(const InstClass &ic);

  gc_string getFetchName(const FetchUnit &);

  gc_string getInstrOpBaseName(const gc_string &icname,const gc_string &oname);

  gc_string getInstrOpName(const gc_string &iname,const gc_string &icname,const gc_string &oname);

  const gc_string &getDummyOpName();

  gc_string getRegName(const adl::RegBase &rb);

  gc_string getRegRead(const adl::RegBase &rb,const adl::RegField *rf = 0);

  gc_string getSparseType(const adl::RegFile &rf);

  gc_string getMaskType(int width);
  gc_string getMaskType(const adl::RegBase &rb);

  gc_string getOpType(const gc_string &op);
  gc_string getOpParm(const gc_string &op);
  gc_string getOpVar(const gc_string &op);

  gc_string getForwardingPathType(const UadlIr &ir,bool safeMode,const ForwardingPath &fp);

  // Returns true if any pipeline stages have the shared attribute.
  bool has_shared_stages(const UadlIr &ir);

  // Returns true if an instruction class contains a value-query method call.
  bool has_value_query(const InstClass &ic);

  const gc_string &getIssueName();

} // namespace uadl

#endif // _UADL_COMMON_H

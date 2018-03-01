//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Some basic tracing flag setup for uADL models.
//

#include "helpers/AnyOption.h"

#include "UadlArch.h"
#include "UadlTrace.h"

using namespace std;
using namespace adl;

namespace uadl {

  bool setupTraceOptions(AnyOption &options)
  {
    options.setFlag("trace-memory",     "tm", "Enable memory activity tracing.");
    options.setFlag("trace-fetch",      "tf", "Enable fetch-unit tracing.");
    options.setFlag("trace-pipeline",   "tp", "Enable pipeline tracing.");
    options.setFlag("trace-stall",      "ts", "Enable pipeline stall tracing.");
    options.setFlag("trace-operand",    "to", "Enable instruction operand tracing.");
    options.setFlag("trace-target",     "tt", "Enable instruction target tracing.");
    options.setFlag("trace-warnings",   "tw", "Enable pipeline warning tracing.");
    options.setFlag("trace-osm",              "Enable OSM state transition tracing.");
    options.setFlag("trace-all",        "ta", "Enable all tracing information.");

    return true;
  }

  // The idea here is that we enable certain type of tracing if the event is
  // turned on, e.g. --trace-foo, or if the user has set the --trace-all flag.
  // However, if the user has specifically turned off a tracing event, then this
  // overrides the --trace-all flag.  In other words, something like this:
  // --trace-all --no-trace-osm, would trace everything but OSM events.
  void check_trace_type(AnyOption &options,uint32_t &trace_flags,const string &type)
  {
    if (options.haveOption(type)) {
      // The flag was set, so use its value.    
      uint32_t flag = get_tracing_event_by_name(type);
      if (options.getFlag(type)) {
        trace_flags |= flag;
      } else {
        trace_flags &= ~flag;
      }
    }
  }

  uint32_t getTraceFlags(AnyOption &options)
  {
    uint32_t trace_flags = 0;

    // If the "all" flag is set, then start with all bits set.
    if (options.getFlag("trace-all", false)) {
      trace_flags = (uint32_t)-1;
    }

    // Then set or clear flags based upon individual options.
    check_trace_type(options,trace_flags,"trace-memory");
    check_trace_type(options,trace_flags,"trace-pipeline");
    check_trace_type(options,trace_flags,"trace-fetch");
    check_trace_type(options,trace_flags,"trace-stall");
    check_trace_type(options,trace_flags,"trace-operand");
    check_trace_type(options,trace_flags,"trace-target");
    check_trace_type(options,trace_flags,"trace-branch");
    check_trace_type(options,trace_flags,"trace-warnings");
    check_trace_type(options,trace_flags,"trace-osm");

    return trace_flags;
  }

}

//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#include <assert.h>
#include <map>
#include <stdexcept>
#include <sstream>

#include "helpers/BasicTypes.h"
#include "helpers/Macros.h"

#include "LogControl.h"

namespace uadl {

  using namespace std;


  // Add new tracing event names here!!
  // This returns a trace flag given a string name.
  uint32_t get_tracing_event_by_name(const string &trace_event)
  {
    enum { NumTraceEvents = 8 };
    static pair<string,uint32_t> trace_init[NumTraceEvents] = {
      make_pair("trace-memory",LOG_MEMORY),
      make_pair("trace-pipeline",LOG_PIPELINE),
      make_pair("trace-operand",LOG_OPERAND),
      make_pair("trace-target",LOG_TARGET),
      make_pair("trace-branch",LOG_BRANCH),
      make_pair("trace-osm",LOG_OSM),
    };
    static map<string,uint32_t> trace_map(&trace_init[0],&trace_init[NumTraceEvents]);

    map<string,uint32_t>::const_iterator iter = trace_map.find(trace_event);
    if (iter == trace_map.end()) {
      RError("Unknown trace event:  " << trace_event);
    } else {
      return iter->second;
    }
  }

  uint32_t LogControl::set_tracing(uint32_t flags) 
  { 
    uint32_t of = _flags; 
    _flags |= flags; 
    update_tracing();
    return of; 
  }
  
  uint32_t LogControl::clear_tracing(uint32_t flags) 
  { 
    uint32_t of = _flags; 
    _flags &= ~flags; 
    update_tracing();
    return of; 
  }
  
  uint32_t LogControl::set_tracing_flags(uint32_t flags) 
  { 
    uint32_t of = _flags; 
    _flags = flags; 
    update_tracing();
    return of; 
  }

  // Toggle on tracing using a string name.
  uint32_t LogControl::set_tracing(const string &tracing_event)
  {
    uint32_t flag = get_tracing_event_by_name(tracing_event);
    return set_tracing(flag);
  }

  // Toggle off tracing using a string name.
  uint32_t LogControl::clear_tracing(const string &tracing_event)
  {
    uint32_t flag = get_tracing_event_by_name(tracing_event);
    return clear_tracing(flag);
  }

}

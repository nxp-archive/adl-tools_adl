//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef _LOG_CONTROL_H_
#define _LOG_CONTROL_H_

namespace uadl {

  // The various logging control flags.
  enum LogItem {
    LOG_WARNINGS = 0x1, 
    LOG_MEMORY   = 0x2, 
    LOG_FETCH    = 0x4,
    LOG_PIPELINE = 0x8, 
    LOG_OPERAND  = 0x10, 
    LOG_TARGET   = 0x20, 
    LOG_BRANCH   = 0x40,
    LOG_OSM      = 0x80,
    LOG_OTHER    = 0x100,
  };

  uint32_t get_tracing_event_by_name(const std::string &trace_event);

  // This controls logging across an entire core.
  class LogControl {
  public:
    LogControl() : 
      _flags(0)
    {}

    virtual ~LogControl() {};

    // Toggle on tracing.  The value is or'd with the current value and the previous value
    // is returned.  The TraceType enum describes the different types of tracing events.  The
    // default is to set all values.
    uint32_t set_tracing(uint32_t flags = (uint32_t)-1);
  
    // Toggle off tracing.  The bits set in the argument turn off the relevant tracing
    // activities.  Default is to turn off all events.  Returns the prior value
    uint32_t clear_tracing(uint32_t flags = (uint32_t)-1);
  
    // These set and clear individual events by name.
    uint32_t set_tracing(const std::string &tracing_event);
    uint32_t clear_tracing(const std::string &tracing_event);

    // Set the tracing flags directly.  Returns the prior value.
    uint32_t set_tracing_flags(uint32_t flags);
  
    // Returns true if any tracing is activated.
    bool tracing_on() const { return _flags; };
  
    // Returns true if the specified tracing events are all on.
    bool tracing_on(uint32_t flags) const { return (flags & _flags) == flags; };
  
    // Returns the tracing flags.
    uint32_t tracing_flags() const { return _flags; };

  protected:
    // Quick single-event tracing check.
    bool trace_flag_on(uint32_t flag) const { return _flags & flag; };

    // Called whenever tracing events change.
    virtual void update_tracing() {};

    uint32_t      _flags;
  };


}

#endif

//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// If a model is generated with the --extern-eventbuses option, then this
// interface class is used to communicate event bus information.  Derive from
// this class and then supply a reference to each relevant IssNode object.
// Install by calling IssNode::setEventBusHandler.
//
// Note that this system is slightly different than what you get when you
// generate with --no-event-bus-send.  In that case, the system is templated and
// requires that you include the specific model so that you have access to the
// event bus data types.
//
// In this case, all data is completely opaque- you can just do memcpys of the
// data to transport it.  However, it's generic and does not require any model
// subclassing.
//

#ifndef _EVENTBUS_EXT_HANDLER_
#define _EVENTBUS_EXT_HANDLER_

#include "helpers/BasicTypes.h"

namespace adl {

  //
  // This is the external event bus interface.  Similar to external memory, when
  // used, all event bus sends go through this interface class, rather than to
  // the global system, as below.  The user must register this class with the
  // relevant cores at the start of simulation.
  //
  struct EventBusExtHandler {
    virtual ~EventBusExtHandler() {};

    // Called to register a new event bus (happens during node construction).
    // The signature (sig) should be checked against any other registered event
    // bus of the same name to make sure that the event bus is the same as
    // what's already registered.
    virtual void register_eventbus(int &handle,int &id,const std::string &name,const std::string &sig,EventBusHandler &handler) = 0;

    // Called on an event bus send operation.  The data packet is opaque, so you
    // can just copy len bytes of it to somewhere, then invoke any other
    // registed objects (casting is done by the generated code).
    virtual void signal_eventbus(int handle,int id,const void *data,int len) = 0;
  };

}

#endif


//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#include "BandwidthControl.h"


namespace uadl {

// BandwidthControl
bool BandwidthControl::consumeBandwidth(ttime_t currTime)
{
  // infinite bandwidth
  if (!_bandwidth) return true;

  // different time, first consume
  if (currTime != _lastConsumeTime) {
    _lastConsumeTime = currTime;
    _consumed = 1;

    return true;
  }

  // same time, bandwidth available
  if (_consumed < _bandwidth) {
    _consumed ++;

    return true;
  }

  return false;
}

} // namespace uadl

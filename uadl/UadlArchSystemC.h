//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Adds on very basic SystemC support to a uADL model.  This just basically
// defines a clock and a way to bind an external clock signal to it.  The
// generated class contains the SC_METHOD information.
//

#ifndef _UADL_ARCH_SYSTEMC_H
#define _UADL_ARCH_SYSTEMC_H

#include <systemc.h>

#include "UadlArch.h"

namespace uadl {

  // NOTE: see UadlArchSystemCIf.h for default constructor parameters
  class UadlArchSystemC : public UadlArch {
  public:
    UadlArchSystemC(ttime_t startTime, std::ostream &out) :
      UadlArch(startTime, out)
    {
    }

    void bindClock(sc_signal<bool> &clock) { _clock(clock); }

    sc_in<bool> _clock;
  };

} // namespace uadl

#endif // _UADL_ARCH_SYSTEMC_H

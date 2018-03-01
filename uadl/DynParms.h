//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef _DYN_PARM_H_
#define _DYN_PARM_H_

namespace adl {
  struct StrPairs;
}

namespace uadl {

  // Defines a dynamic parameter manipulator which can be modified via the main
  // architecture at simulation start-time.  An object which has dynamic
  // parameters should derive from this class and register itself into the Timer
  // class.  Objects may throw runtime_error if an error is encountered during
  // data validation, but shouldn't throw due to an unknown parm name, since
  // these will be chained together.
  struct DynParms {
    virtual ~DynParms() {};
    
    // Should return false if a dynamic parameter is not recognized, so that the
    // main object will pass it to the next item to try.
    virtual bool set_dyn_parm(const std::string &parm,unsigned value) { return false; }

    // Should reurn false if a dynamic parameter is not recognized.  If it is recognized,
    // return true and update 'value'.
    virtual bool get_dyn_parm(unsigned &value,const std::string &parm) const { return false; }

    // Should append relevant parms to the list.  First element is the parm
    // name, second element is a description string.
    virtual void list_dyn_parm(adl::StrPairs &parms) const {}
  };

}

#endif

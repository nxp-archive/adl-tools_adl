//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef _RUNCONTROLIFACE_H_
#define _RUNCONTROLIFACE_H_

namespace adl {

  class IssNode;

  //
  // Interface class for interfacing with the debugger.
  //
  struct RunControlIface {

    virtual ~RunControlIface() {};

    // Perform any additional/specialized reset actions needed.
    virtual void reset(IssNode &node) {};

    // Reverse execution support
    virtual bool hasReverseExecSupport(IssNode &node) { return false; };
    
    // Should execute the model until some sort of stopping condition occurs.
    virtual ProgramStatus run(IssNode &node,SimType stype) = 0;
    
    // KEY_FUNC: Reverse execution
    virtual ProgramStatus rRun(IssNode &node,SimType stype) { return ACTIVE; };

    // Should execute the model n times, whatever that means for this particular
    // model.
    virtual ProgramStatus stepn(unsigned n,IssNode &node) = 0;
    
    // KEY_FUNC: Reverse stepn
    virtual ProgramStatus rStepn(unsigned n,IssNode &node) { return ACTIVE; };
    
    // Waveform support, run until timestamp
    virtual ProgramStatus runUntilTime(IssNode &node, uint64_t time) { return ACTIVE; };
    
    // Waveform support, reverse run until timestamp
    virtual ProgramStatus rRunUntilTime(IssNode &node, uint64_t time) { return ACTIVE; };

  };


}

#endif

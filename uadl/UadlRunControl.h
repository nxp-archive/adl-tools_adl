//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// The run-control interface is used for alternate interfaces, such as by the
// debugger and the scripting interface.
//

#ifndef _UADL_RUN_CONTROL_H_
#define _UADL_RUN_CONTROL_H_

#include "helpers/BasicTypes.h"

#include "iss/ScriptingIface.h"

namespace adl {
  class IssNode;
}

namespace uadl {

  class UadlArch;
  class UadlScriptLogger;

  // A simple run-control class to interface uADL models with the scripting interface.
  struct UadlRunControl : public adl::ScriptingControlIface {

    UadlRunControl();

    virtual void reset(adl::IssNode &node);

    void setArch(UadlArch *arch) { _arch = arch; };

    void setup(adl::ttime_t endTime,uint32_t trace_flags);

    virtual uint64_t instr_count(const adl::IssNode &) const;

    virtual uint64_t get_cycle_count(adl::IssNode &node);

    virtual adl::ProgramStatus run(adl::IssNode &node,adl::SimType stype);

    virtual adl::ProgramStatus stepn(unsigned n,adl::IssNode &node);

    virtual void cyclen(unsigned n);

    virtual void genExceptions(unsigned flags,adl::IssNode &node);

    virtual bool cancelExceptions(unsigned flags,adl::IssNode &node);

    virtual void     set_dyn_parm(adl::IssNode &node,const std::string &parm,unsigned value);
  
    virtual unsigned get_dyn_parm(const adl::IssNode &node,const std::string &parm) const;
  
    virtual void     list_dyn_parm(const adl::IssNode &node,adl::StrPairs &parms) const;

    virtual adl::ScriptLogger *createScriptLogger();

  protected:
    adl::ProgramStatus status() const;

    UadlArch         *_arch;
    UadlScriptLogger *_logger;
    adl::ttime_t      _endTime;
  
    void step_model();

    void check_breakpoint();

  };

}

#endif

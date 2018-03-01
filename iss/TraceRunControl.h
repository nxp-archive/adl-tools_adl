#include <iostream>

#include "Reader.h"

#include "ScriptingIface.h"
#include "ScriptLogger.h"

using namespace std;

namespace adl {

  // This is a run-control object for trace reader.
  
  struct TraceRunControl : public ScriptingControlIface {
    
    unique_ptr<Reader> _reader;  // done - TODO: should be a unique_pointer
    // done - TODO: should add an init() to init Reader
    // void reader_init(unique_ptr<Reader> reader);

    virtual void reset(IssNode &node);
    
    virtual bool hasReverseExecSupport(IssNode &node);
    
    virtual uint64_t instr_count(const IssNode &node) const;
    
    virtual ProgramStatus run(IssNode &node,SimType stype);
    
    virtual ProgramStatus rRun(IssNode &node,SimType stype);
    
    virtual ProgramStatus runUntilTime(IssNode &node, uint64_t time);
    
    virtual ProgramStatus rRunUntilTime(IssNode &node, uint64_t time);

    virtual ProgramStatus stepn(unsigned n,IssNode &node);
    
    virtual ProgramStatus rStepn(unsigned n,IssNode &node);

    // NOTE: have to add those because they are pure virtual in ScriptingIface.h
    virtual void genExceptions(unsigned flags,IssNode &node) {};   

    virtual bool cancelExceptions(unsigned flags,IssNode &node) { return false; };

    virtual void set_dyn_parm(IssNode &node,const std::string &parm,unsigned value) {};

    virtual unsigned get_dyn_parm(const IssNode &node,const std::string &parm) const { return node.get_dyn_parm(parm); };

    virtual void list_dyn_parm(const IssNode &node,StrPairs &parms) const {};
    
    virtual ScriptLogger *createScriptLogger();

  };
  
}


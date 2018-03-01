#include <iostream>

#include "TarmacReader.h"
#include "DatReader.h"
#include "ExecSupport.h"

#include "ScriptingIface.h"
#include "ScriptLogger.h"

#include "TraceRunControl.h"

using namespace std;

namespace adl {

  // This is a run-control object for trace reader.
    
  // void TraceRunControl::reader_init(unique_ptr<Reader> reader)
  // {
  // _reader = move(reader);
  // }

  void TraceRunControl::reset(IssNode &node)
  {
    node.reset();
  }
  
  // KEY_FUNC: hasReverseExecSupport
  bool TraceRunControl::hasReverseExecSupport(IssNode &node)
  {
    return true;
  }
    
  uint64_t TraceRunControl::instr_count(const IssNode &node) const
  {
    return _reader->instrCount();
  }
    
  ProgramStatus TraceRunControl::run(IssNode &node,SimType stype)
  {
    try {
      while (1) {
        _reader->applyNext(false);
      }
        
      return HALT; // should never reach here
    }
    catch (SimInterrupt) {
      return BREAK;
    }
  }

  // KEY_FUNC: rRun
  ProgramStatus TraceRunControl::rRun(IssNode &node,SimType stype)
  {
    try {
      while (1) {
        _reader->applyPrev(false);
      }
        
      return HALT; // should never reach here
    }
    catch (SimInterrupt) {
      return BREAK;
    }
  }
  
  // KEY_FUNC: runUntilTime
  ProgramStatus TraceRunControl::runUntilTime(IssNode &node, uint64_t time)
  {
    try {
      while (node.get_td_counter() < time) { // reaches the time
        _reader->applyNext(true);
      }    
      // we do not catch breakpoint in this case
        
      return HALT; // should never reach here
    }
    catch (SimInterrupt) {
      return BREAK;
    }
  }
  // KEY_FUNC: rRunUntilTime
  ProgramStatus TraceRunControl::rRunUntilTime(IssNode &node, uint64_t time)
  {
    try {
      while (node.get_td_counter() > time) { // reaches the beginning or reaches the time
        _reader->applyPrev(true);
      }
        
      return HALT; // should never reach here
    }
    catch (SimInterrupt) {
      return BREAK;
    }
  }
  
  ProgramStatus TraceRunControl::stepn(unsigned n,IssNode &node)
  {
    try {
      unsigned i = 0;
      bool res;
      while (i < n) {        
        res = _reader->applyNext(false);
        if (res) {
          i++;
        }
        else { // reach the end
          return HALT; 
        }
      }    
      return ACTIVE;
    }
    catch (SimInterrupt) {
      return BREAK;
    }
  }

  // KEY_FUNC: rStepn
  ProgramStatus TraceRunControl::rStepn(unsigned n,IssNode &node)
  {
    try {
      unsigned i = 0;
      bool res;
      while (i < n) {        
        res = _reader->applyPrev(false);
        if (res) {
          i++;
        }
        else { // reach the end or breakpoint
          return HALT; 
        }
      }    
      return ACTIVE;
    }
    catch (SimInterrupt) {
      return BREAK;
    }
  }

  ScriptLogger *TraceRunControl::createScriptLogger()
  {
#     if CLI_SUPPORT != 0
    return makeScriptLogger();
#     else
		return 0;
#     endif
  }

  
}


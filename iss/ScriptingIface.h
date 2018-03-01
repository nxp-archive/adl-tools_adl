//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef _H_SCRIPTINGIFACE__
#define _H_SCRIPTINGIFACE__

#include "helpers/stringfwd.h"

#include "iss/DbgInterface.h"
#include "iss/RunControlIface.h"

namespace adl {

  class Target;
  struct StrPairs;
  class ScriptLogger;
  class Args;
  struct tCmd;

  //
  // The RunControl object abstracts out how to cycle and execute the model, so
  // that it can be used with more than just a basic ISS.
  //
  struct ScriptingControlIface : public RunControlIface {

    // Should return the current instruction count for the object.
    virtual uint64_t instr_count(const IssNode &node) const = 0;

    // Should trigger the exceptions specified by the mask.
    virtual void genExceptions(unsigned flags,IssNode &node) = 0;

    // Should cancel an exception.
    virtual bool cancelExceptions(unsigned flags,IssNode &node) = 0;    

    // For dynamic parameter manipulation.
    virtual void     set_dyn_parm(IssNode &node,const std::string &parm,unsigned value) = 0;
    virtual unsigned get_dyn_parm(const IssNode &node,const std::string &parm) const = 0;
    virtual void     list_dyn_parm(const IssNode &node,StrPairs &parms) const = 0;

    // For event logging.  Instantiate a class for handling event callbacks.
    virtual ScriptLogger *createScriptLogger() = 0;
  };

  // A command handler's prototype.  The return value is the value to be
  // returned to the interpeter.  You should avoid writing to cerr or cout,
  // since the function may be invoked by a remote protocol, e.g. gdb, and
  // thus the output would not be returned to the user in an obvious location.
  // On error you should return a runtime_error.
  typedef std::string (*Command) (void *obj,const tCmd *cmd,const Args &args);

  // A structure which contains information on the commands this program
  // can understand.
  struct tCmd {    
    const char *name;		// User printable name of the function.
    void       *obj;    // Object handler for the command to use.
    Command     func;		// Method to call to do the job.
    const char *doc;		// Documentation for this function.
  };

  //
  // The ScriptingIFace class provides a scripting interface for an ISS model.
  // This is the interface that the outside world sees.
  //
  class ScriptingIface : public DbgInterface {

  public:

    // Main entry point for the scripting interface.  Note: This does not
    // return.  Cleanup is performed by invoking adl::finalize().
    virtual void exec(void) = 0;

    // Execute a TCL command.  The result of the command is placed into
    // 'result'.  Returns false if there's an error, true otherwise.  Note that
    // the interpreter is persistent across calls to eval().
    virtual bool eval(std::string &result,const std::string &line) = 0;

    // Execute a file.  On error, throws a runtime_error.
    virtual void evalFile(const std::string &fn) = 0;

    virtual void setRoot(IssNode* n) = 0;

    virtual void setRunCtrl(ScriptingControlIface *runctrl) = 0;

    virtual void addCommand(tCmd &cmd) = 0;

    virtual const char *docString(const std::string &name) const = 0;

    // Sets the current node path used by the scripting engine.  Returns the old
    // value.
    virtual Target setPath(const Target &trg) = 0;

    virtual void print(std::ostream &o) const = 0;
  };

  inline std::ostream& operator<<(std::ostream& target, const ScriptingIface& iface)
  {
    iface.print(target);
    return target;
  }

  // Grab a reference to the singleton object.
  ScriptingIface &scripter();

  // Call this to check to see if a config script should be run.  It then
  // executes it if there is one.
  void checkForCliConfig(IssNode &root,ScriptingControlIface &runctrl);

  // Call this to check to see if CLI mode is requested.
  // Returns a debugger-interface object if true, 0 otherwise.
  DbgInterface *checkForCliMode(IssNode &root,ScriptingControlIface &runctrl);

  // Start the scripting interface using the specified run-control object and root
  // node.  If a script is specified, then that file is opened and executed.
  // Otherwise, the scripting interface will start in an interactive mode.
  DbgInterface *startCliMode(IssNode &root,ScriptingControlIface &runctrl,const std::string &script = std::string(),
                             const std::vector<std::string> &args = std::vector<std::string>());

}
#endif


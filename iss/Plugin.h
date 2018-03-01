//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Support types for the plug-in interface.
//

#ifndef _PLUGIN_H_
#define _PLUGIN_H_

#include <iosfwd>

#include "helpers/stringfwd.h"

namespace adl {
  
  struct IssNode;
  class Writer;
  class Reader;
  struct SysCallHandler;
  struct MemoryLog;
  class AnyOption;
  struct Args;

# define AdlPluginEntry "adl_plugin_entry"

  // This base class is the basic interface to a plugin.  A plugin shared object
  // must return a pointer to a class derived from this interface.  For each plugin,
  // the framework looks for a function called "adl_plugin_entry", which is expected to have
  // a signature of:
  //
  // Plugin *adl_plugin_entry();
  //
  // The function returns a pointer to the plugin interface, which then has the job of
  // handling queries for handling different custom reader, writer, or system-call handler
  // creation requests.
  class Plugin {
  public:
    virtual ~Plugin() {};

    // Generalized initialization routine.  This is called when the plugin is
    // loaded and allows the plugin to perform custom actions, such as
    // registering custom loggers, etc.  The method has access to the options
    // database and may install new command-line options at this point.
    virtual void init(AnyOption &options) {};

    // Called after final option processing.  If the plugin should access the
    // command-line database at this point to read any needed option values.
    virtual void setup(AnyOption &options,IssNode &root) {};

    // If 'type' is recognized (this will be the extension of an output file), then construct
    // the relevant writer type.  If not recognized, return a 0.
    virtual Writer *createWriter(IssNode &root,const std::string &type,const std::string &fn,const MemoryLog &memlog) { return 0; };

    // Same as above, except this is for constructing a stream writer, which
    // writes data to an output stream.
    virtual Writer *createStreamWriter(IssNode &root,const std::string &type,std::ostream &os,const MemoryLog &memlog) { return 0; };

    // If 'type' is recognized (this will be the extension of an input file), then construct
    // the relevant reader type.  If not recognized, return a 0.
    virtual Reader *createReader(IssNode *root,const std::string &type,
                                 const std::string &filename,const Args *args) { return 0; };

    // If this plugin has a system-call handler, then create such an object.  If
    // not, return 0.  Note: This is for using the global system-call emulator
    // interface which is part of the standalone runtime.  For installing an
    // external system call handler, just call IssNode::setSysCallHandler in
    // createMemHandler.
    virtual SysCallHandler *createSysCallHandler() { return 0; };

    // If this plugin defines an external memory handler, then create such an
    // object.  This method should call setMemHandler on the root argument (or
    // its children if it's a system).
    virtual void createMemHandler(IssNode &root) {};

    // Called at the end of a simulation run, after logging has been finalized.
    // This may be called multiple times if there are multiple tests in any
    // input files.
    virtual void finalize(IssNode &root) {};
  };

}

#endif

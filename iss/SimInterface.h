//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This file contains the API available when using a standalone ISS.
//

#ifndef _SIMINTERFACE_H_
#define _SIMINTERFACE_H_

#include <string>
#include <vector>
#include <list>
#include <memory>

#include "helpers/BasicTypes.h"

namespace adl {

  class Reader;
  class Writer;
  class AnyOption;
  class DbgInterface;
  class Plugin;
  class LoggingMgr;
  class LogParentIface;

  struct RunControlIface;
  struct IssNode;
  struct Args;

  //
  // Various mode flags.
  //

  // Global value for whether we should use a file's entry point to set the program
  // counter when reading files.  Set via a command-line parameter and applies to
  // files specified on the command-line.
  extern bool UseFileEntryPoint;
  // Global value for entry point, if empty - use file entry point else use the point defined by user.
  extern std::string EntryPoint;


  // This list stores all plugin objects, plus the default services object.
  typedef std::list<Plugin *> PluginList;

  typedef std::shared_ptr<Writer> WriterPtr;
  typedef std::list<WriterPtr> WriterList;

  // Functor used for installing external memories.  If not supplied to init(),
  // then this default version is used.
  struct ExtMemHelper {
    virtual ~ExtMemHelper() {};

    virtual void operator() (IssNode &node,const PluginList &plugins) const;
  };

  // Writer/logger context, used when you don't want to use the internal static
  // objects.  Creates its own logging mgr object.
  struct WriterContext {

    WriterContext();
	WriterContext(WriterContext &&x);

    IssNode *node() const { return _node; };

    void installLoggers(IssNode *n);
    void registerLogger(LogParentIface *p);
    void unregisterLogger(LogParentIface* p);

    void startLogging();
    void doneLogging();

    void addWriter (WriterPtr);
    void removeWriter(WriterPtr writer);
    void removeAllWriters();

    WriterList const &writers() const { return _writers; };

  protected:
    WriterList                  _writers;
    std::unique_ptr<LoggingMgr> _mgr;
    IssNode                    *_node;
  };

  // Gain access to the global command-line options object.
  // You can add options to this during startup, then query for the options later.
  AnyOption &options();

  // Factory function for allocating the core object.
  typedef IssNode *(*RootCreator)(unsigned &id);

  // Create the top-level item.  This may be a system of items if n > 1.
  IssNode *createRoot(unsigned n,unsigned &coreid,RootCreator creator);

  // Does all of the model setup.  Returns 0 if main should exit.  Throws
  // runtime_error for problems.  May be used as an entry point by external
  // application.
  IssNode *init(int argc,const char *argv[],RootCreator creator,bool error_unknown_options = true,
                const ExtMemHelper &ext_mem_helper = ExtMemHelper());

  // Same functionality as above, but does not create the object if a non-null
  // object is supplied (in which case, creator may be 0 as well).  The object
  // may be passed in for initialization.  Returns 0 if we should exit
  // immediately w/o error, true to proceed.
  IssNode *init(IssNode *root,int argc,const char *argv[],RootCreator creator = 0,
                bool error_unknown_options = true,const ExtMemHelper &ext_mem_helper = ExtMemHelper());

  // This performs any necessary cleanup code after the execution of the
  // simulation or a debugger/alternate execution loop.  If an alternate loop
  // doesn't return normally, it should install an exit handler which invokes
  // this function.
  void finalize(WriterContext &wc);
  void finalize();

  // Simplfied interface for starting up the scripting interface, if so desired.
  // Uses the default run-control object, which allows for flexibility with
  // different kinds of models, e.g. ADL or uADL.  The run control object can be
  // set or retrieved via routines declared in ExecSupport.h
  //
  // If 'script' is specified, then that file will be opened and executed.
  // Otherwise, the scripting interface will start in an interactive mode.  If
  // 'args' is specified, then these arguments will be used to initialize TCL's
  // command-line argument variables.
  DbgInterface *startCliMode(IssNode &root,const std::string &script = std::string(),
                             const std::vector<std::string> &args = std::vector<std::string>());

  // This will execute a command via the cli mode.  It assumes that any
  // initialization was performed by startCliMode, if applicable.
  std::string evalCliCommand(const std::string &cmd);

  // Create a reader and use it to initialize the model.  If the extension is an
  // empty string, then the type is derived from the filename, otherwise it is used to
  // specify the file type.
  //
  // If use_file_entry_point is set, then we use the entry point specified by the file,
  // otherwise we do not.  This is only obeyed if the reader supports this option.
  // Additional user flags may be passed in 'args'.
  //
  // If laod_file is true, then the file is actually read.  Otherwise, it is not
  // read; the user must call Reader::readFile() to actually load the file.
  //
  // If init_memory is true, then we initialize memory.  If false, we do
  // everything else, e.g. set registers, initialize the starting address, but
  // we don't init memory.
  Reader *readInput(IssNode *root,const std::string &fn,const std::string &ext = "",
                    bool use_file_entry_point = true, bool load_file = true, bool init_memory = true, 
                    const std::string &entry_point = "", const Args *args = 0);

  struct ReaderInfo {
    std::string _filename;  // Name of the file that was read.
    std::string _fmt;       // Format type used when reading this file.
    ReaderInfo(const std::string &fn,const std::string &fmt) : _filename(fn), _fmt(fmt) {};
  };
  typedef std::list<ReaderInfo> ReaderInfoList;

  // This routine returns information about files which have been read.  This list
  // is reset by resetSimulation.
  const ReaderInfoList &readerInfo();

  // This reads in all input files.  If called repeatedly, it repeats the
  // same inputs, i.e. you can reset the model, then call this again to
  // repeatedly run the same stuff.
  void processAllInput(IssNode &root);

  // This processes command line options.
  bool processOptions(int argc,const char *argv[],bool error_unknown_options = true);
  bool processRoot(const IssNode* r);

  // Add on a plugin to the list.  This is equivalent to specifying a shared
  // object on the command-line, via the plugin command-line parameter, then
  // having that shared object supply a plugin, except that this is generally
  // used when an alternative main() is being supplied.
  void addPlugin(Plugin *plugin);

  //
  // In the routines below, the versions which do not take an explicit
  // WriterContext use an internal WriterContext object.
  //

  // Creates and installs the appropriate writer for the specified output file
  // name.  If the name is empty, creates a default name based upon the input
  // file name's type or the specified format type.
  WriterPtr processOutputFile(WriterContext &wc,IssNode *root,std::string outputFileName,
                            std::string outputFileFormat,const std::string &defaultDir,
                            const std::string &defaultInput,const std::string &defaultSfx,std::ostream *log);
  WriterPtr processOutputFile(IssNode *root,std::string outputFileName,std::string outputFileFormat,
                            const std::string &defaultDir,const std::string &defaultInput,
                            const std::string &defaultSfx,std::ostream *log);

  // If we're using multi-file, then given the root that's not a core, create
  // writer contexts for each child item.  Otherwise, create the writer for the
  // top-level item.
  WriterList processMultiOutputFile(IssNode *root,std::string outputFileName,std::string outputFileFormat,
                                    const std::string &defaultDir,const std::string &defaultInput,
                                    const std::string &defaultSfx,std::ostream *log);

  // Clear all loggers from the internal, static list.
  void clearLoggers();
  
  // Once a root node is created, call installLoggers to install the broadcast
  // loggers into each core.  This must be called before any writers are
  // created.  This is for when you want to use the internal, static
  // WriterContext, rather than supplying your own.
  void installLoggers(IssNode *n);

  // These are used for registering additional logger interface objects with the built-in static writer context.
  void registerLogger(LogParentIface *p);
  void unregisterLogger(LogParentIface* p);

  // Creates a new writer and installs it for use with tracing.  If the extension
  // is an empty string, then the type is derived from the filename, otherwise it
  // is used to specify the file type.
  // The resulting writer must be added to a WriterContext via WriterContext::addWriter.
  WriterPtr createWriter(IssNode &root,const std::string &filename,const std::string &ext = "");

  // Creates a new writer for writing to a stream.  The 'type' parameter specifies what kind
  // of writer to create.
  WriterPtr createStreamWriter(WriterContext &wc,IssNode &root,std::ostream &os,const std::string &type);
  WriterPtr createStreamWriter(IssNode &root,std::ostream &os,const std::string &type);

  // Registers the specified writer with the static writer context.  The writer
  // is then owned by the system.
  void addWriter (WriterPtr);

  // De-registers the specified writer from the static writer context, deleting
  // the writer.
  void removeWriter(WriterPtr);

  // Remove all registered writers from the static writer context, deleting all
  // writers.
  void removeAllWriters();

  // This gracefully stops logging and dumps state if so requested.  Call this
  // before finalize in order to properly stop all logging.
  void handleFinalState(bool dumpstate,IssNode &root);

  // Dumps the current state of the architecture as an "initial state". If a writer
  // is passed, then only applies to that writer, else applies to all writers. 
  void dumpInitialState(Writer* w);
  void dumpInitialState();
  void dumpInitialState(WriterContext &wc);

  // Dumps the final state of the architecture as a "final state".  If a writer
  // is passed, then only applies to that writer, else applies to all writers.
  void dumpFinalState(Writer* w);
  void dumpFinalState();
  void dumpFinalState(WriterContext &wc);

  // Turns on tracing.  This causes the writer to record the current state of
  // the model as an "initial state".  May be called repeatedly, in which case
  // a new sequence is started.  For example, for UVPs, this means a new test.
  // If tracing is currently on, has no effect.
  // The root node is required b/c the tracing-on event will be generated.
  void startLogging(WriterContext &wc,IssNode &root);
  void startLogging(IssNode &root);

  // Turns off tracing.  This causes the writer to record the current state as
  // a "result state".  Has no effect if tracing is currently off.
  // The root node is required b/c the tracing-off event will be generated.
  void stopLogging(WriterContext &wc,IssNode &root);
  void stopLogging(IssNode &root);

  // Turn on the logging of events for a writer.  This does not modify what
  // tracing events are active and it does not execute the trace-on hook, if one
  // exists.
  void startSlicing(IssNode &root,Writer* w);
  void startSlicing(WriterContext &wc,IssNode &root);
  void startSlicing(IssNode &root);

  // Turn off the logging of events for a writer.  This does not modify the active
  // tracing events and does not execute the trace-off hook.
  void stopSlicing(IssNode &root,Writer* w);
  void stopSlicing(WriterContext &wc,IssNode &root);
  void stopSlicing(IssNode &root);

  // A general reset function for reseting the simulation and any other portions
  // of the standalone framework that need to be reset.  If a run-control object
  // is provided, then its reset routine is used to reset the model.
  void resetSimulation(IssNode &root,RunControlIface *runctrl = 0);

  // Returns the type of simulator to be used, as specified by the command-line
  // options.
  SimType simMode();

  // Returns true if we should simulate using the multi-threaded kernel.
  bool threadedSim();

  // Run the simulation using the MT simulator is threadedSim() returns true,
  // else in single-threaded mode, using the simulation type specified.  Uses
  // the internal thread pool.  Call threadedSim() directly if you want to use
  // your own thread pool.
  //
  // Returns BREAK if a breakpoint occurred, otherwise ACTIVE, though if root is
  // a single core, will return HALT if the core is halted.
  ProgramStatus runSim(IssNode &root,SimType smode);

}

#endif

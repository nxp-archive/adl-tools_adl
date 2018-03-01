//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This contains the code used to turn a generated ISS into an
// executable.  This should contain anything that's only needed
// if we're generating an executable, rather than just a library,
// e.g. file readers, etc.
//

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <stdexcept>
#include <algorithm>
#include <list>
#include <assert.h>

#ifdef _MSC_VER
# include <sys/timeb.h>
# include <sstream>
#else
# include <sys/time.h>
# if !defined(__MINGW32__)
#   include <monetary.h>
# endif
#endif

#include <stdarg.h>
#include <fcntl.h>

#include "helpers/Macros.h"

#include "helpers/AnyOption.h"
#include "helpers/FilenameUtils.h"
#include "helpers/StringUtils.h"

#include "ModelInterface.h"
#include "Memory.h"
#include "Writer.h"
#include "Reader.h"
#include "DynLoad.h"
#include "Plugin.h"
#include "SysCallHandler.h"
#include "SysCallExtHandler.h"
#include "SysCallMgr.h"
#include "StringHelpers.h"
#include "DbgRegMap.h"
#include "ThreadedKernel.h"
//Trace reader run-control
#include "TraceRunControl.h"

#if !defined(DAT_SUPPORT) || (DAT_SUPPORT != 0)
# define DAT_SUPPORT
# include "DatWriter.h"
# include "FastDatWriter.h"
# include "DatReader.h"
#else
# undef DAT_SUPPORT
#endif

#if !defined(TARMAC_SUPPORT) || (TARMAC_SUPPORT != 0)
# define TARMAC_SUPPORT
# include "TarmacReader.h"
#else
# undef TARMAC_SUPPORT
#endif

#if !defined(ELF_SUPPORT) || (ELF_SUPPORT != 0)
# define ELF_SUPPORT
# include "ElfReader.h"
#else
# undef ELF_SUPPORT
#endif

#if !defined(IHEX_SUPPORT) || (IHEX_SUPPORT != 0)
# define IHEX_SUPPORT
# include "IHexReader.h"
#else
# undef IHEX_SUPPORT
#endif

#if !defined(UVP_SUPPORT) || (UVP_SUPPORT != 0)
# define UVP_SUPPORT
# include "UvpReader.h"
# include "UvpWriter.h"
#else
# undef UVP_SUPPORT
#endif

// No CLI support for MINGW for now.
#ifdef __MINGW32__
#  define CLI_SUPPORT 0
#  define GDB_SUPPORT 0
#endif

#if !defined(GDB_SUPPORT) || (GDB_SUPPORT != 0)
# define GDB_SUPPORT
# include "RemoteGdbServer.h"
#else
# ifndef _MSC_VER
#   warning "GDB Support disabled"
# endif
# undef GDB_SUPPORT
#endif

#if !defined(CLI_SUPPORT) || (CLI_SUPPORT != 0)
# define CLI_SUPPORT
#else
# ifndef _MSC_VER
#   warning "CLI Support disabled"
# endif
# undef CLI_SUPPORT
#endif

#include "ScriptingIface.h"
#include "ScriptLogger.h"

#include "LoggingMgr.h"
#include "SimInterface.h"
#include "DbgInterface.h"
#include "MemoryLog.h"

using namespace std;

namespace adl {

  // This stores registered writers.
  typedef std::list<WriterContext> WriterContexts;
  static WriterContexts writers;

  // Stores system-call emulation handlers.
  typedef std::list<unique_ptr<SysCallExtHandler> > SysCallHandlers;
  static SysCallHandlers syscallHandlers;

  // This list stores readers which aren't finished yet.  That is, they support multiple
  // tests and the file they're reading contains multiple tests.
  typedef std::list<unique_ptr<Reader> > ReaderList;
  static ReaderList persistentReaders;

  // This stores information about files which have been read for this
  // simulation.  It is cleared by the resetSimulation() routine.
  static ReaderInfoList readerInfoList;
  
  // By default, we want to use the interpreter.
  static SimType SimMode = SimNormal;

  static unsigned TestCount    = 0;
  static unsigned MaxTestCount = 0;
  static int      NumCores     = 1;

  static const int DefaultMaxInstrCount = 1000000;

  static bool DumpJitCode = false;
  static bool MultiFileTrace = false;
  static bool FastTrace = false;
  static bool ThreadedTrace = false;

  // Global value for whether we should use a file's entry point to set the program
  // counter when reading files.  Set via a command-line parameter and applies to
  // files specified on the command-line.
  bool UseFileEntryPoint = false;
  // Global value for entry point, if empty - use file entry point else use the point defined by user.
  string EntryPoint = "";

  static unsigned DefaultThreshold = 100;

  static string FastTraceExt = "fdat";

  extern  const char* MemoryName;

  extern bool ThreadedSim;
  
  WriterContext::WriterContext() :
    _mgr(createLoggingMgr(ThreadedTrace)),
    _node(0)
  {}

  WriterContext::WriterContext(WriterContext &&x) :
    _writers(x._writers),
    _mgr(move(x._mgr)),
    _node(x._node)
  {}

  void WriterContext::installLoggers(IssNode *n)
  {
    _node = n;
    _mgr->installLoggers(n);
  }

  void WriterContext::registerLogger(LogParentIface *p)
  {
    _mgr->registerLogger(p);
  }

  void WriterContext::unregisterLogger(LogParentIface* p)
  {
    _mgr->unregisterLogger(p);
  }

  void WriterContext::addWriter (WriterPtr w)
  {
    _mgr->registerLogger(w.get());
    _writers.push_back(w);
  }

  void WriterContext::removeWriter(WriterPtr writer)
  {
    // If a writer exists, shut it down and deregister it.
    if (writer) {
      _mgr->unregisterLogger(writer.get());
      auto wi = find(_writers.begin(),_writers.end(),writer);
      if (wi != _writers.end()) {
        _writers.erase(wi);
      }
    }
  }

  void WriterContext::removeAllWriters()
  {
    while (!_writers.empty()) {
      removeWriter(_writers.front());
    }    
  }

  void WriterContext::startLogging()
  {
    _mgr->startLogging();
  }

  void WriterContext::doneLogging()
  {
    _mgr->doneLogging();
  }

  // This is a default run-control object for ADL ISS scripting.  You can
  // override this by creating another class derived from ScriptingControlIface and
  // creating a factory to produce it.  Then call setRunControlFactory(),
  // declared in ExecSupport.h, to have it be used.
  struct IssRunControl : public ScriptingControlIface {

    virtual void reset(IssNode &node)
    {
      node.reset();
    }
    
    virtual bool hasReverseExecSupport(IssNode &node)
    {
      return false;
    }

    virtual uint64_t instr_count(const IssNode &node) const
    {
      return node.instr_count();
    }
    
    virtual ProgramStatus run(IssNode &node,SimType stype)
    {
      return runSim(node,stype);
    }

    virtual ProgramStatus stepn(unsigned n,IssNode &node)
    {
      return node.stepn(n);
    }

    virtual void genExceptions(unsigned flags,IssNode &node)
    {
      node.genExceptions(flags);
    }    

    virtual bool cancelExceptions(unsigned flags,IssNode &node)
    {
      return node.cancelExceptions(flags);
    }

    virtual void     set_dyn_parm(IssNode &node,const std::string &parm,unsigned value)
    {
      node.set_dyn_parm(parm,value);
    }

    virtual unsigned get_dyn_parm(const IssNode &node,const std::string &parm) const
    {
      return node.get_dyn_parm(parm);
    }

    virtual void     list_dyn_parm(const IssNode &node,StrPairs &parms) const
    {
      node.list_dyn_parm(parms);
    }

    virtual ScriptLogger *createScriptLogger()
    {
#     ifdef CLI_SUPPORT
      return makeScriptLogger();
#     else
		return 0;
#     endif
    }

  };
  
  static IssRunControl default_runctrl;
  static TraceRunControl tr_runctrl;  //done - TODO: put static outside function. along with default runcontrol
  static ScriptingControlIface *runctrl = &default_runctrl;
  static ThreadPool *threadpool = 0;

  void setRunControl(ScriptingControlIface *rc)
  {
    runctrl = rc;
  }

  ScriptingControlIface *getRunControl()
  {
    return runctrl;
  }

  void checkForConfig(IssNode &root)
  {
#   ifdef CLI_SUPPORT
    checkForCliConfig(root,*runctrl);
#   endif    
  }

  void checkForRegMap(IssNode &node)
  {
    if (!node.is_core()) {
      ForEach(node.children(),i) {
        checkForRegMap(**i);
      }
    } else {
      if (auto d = node.getDefaultRegMap()) {
        DbgRegMap &drm = dbgRegMap();
        DbgRegInfos &dri = drm[&node];
        dri.clear();
        dri.setBigEndian(node.isBigEndian());
                         
        int id,index;
        for ( ; d->valid(); ++d) {
          if (!node.getRegIndex(id,index,d->_name)) {
            RError("Unknown register in default register map:  " << d->_name);
          }

          dri.push_back(DbgRegInfo(id,index,d->_width));
        }
      }
    }
  }

#define CheckDebugger(x) { if (DbgInterface *y = (x)) { return y; } }

  // Add support to debugger interface factories here.
  DbgInterface *createDebugger(IssNode &root)
  {
#   ifdef CLI_SUPPORT
    // This should be first so that it will execute a config script before it
    // runs the debugger, if so requested.
    CheckDebugger(checkForCliMode(root,*runctrl));
#   endif

#   ifdef GDB_SUPPORT
    CheckDebugger(checkForGdbMode(root,*runctrl));
#   endif

    return 0;
  }

  // Start up the CLI interface, if support for this feature is included.  Use the
  // default run-control object.
  DbgInterface *startCliMode(IssNode &root,const string &script,const vector<string> &args)
  {
# ifdef CLI_SUPPORT
    return startCliMode(root,*runctrl,script,args);
# else
    return 0;
# endif
  }

  static string getFileType(const string &fn,const string &ext)
  { 
    if (!ext.empty()) {
      // We're given a file type, so use it explicitly.
      return ext;
    } else {
      // No file type given, so try to get it from the file name.
      return getExtension(fn);
    }
  }

  void clearLoggers()
  {
    writers.clear();
  }

  // We have to create multiple context objects if we're using multi-file-trace,
  // and attach those to the object's children.  Otherwise, we just use a single
  // object and attach it to the specified item.
  void installLoggers(IssNode *n)
  {
    if (MultiFileTrace) {
      if (!n->is_core()) {
        ForEach(n->children(),i) {
          IssNode *node = *i;
          writers.push_back(WriterContext());
          writers.back().installLoggers(node);
        }
      } else {
        // Disable because we just have a single item.
        MultiFileTrace = false;
        writers.push_back(WriterContext());
        writers.back().installLoggers(n);      
      }
    } else {
      writers.push_back(WriterContext());
      writers.back().installLoggers(n);
    }
  }

  void registerLogger(LogParentIface *p)
  {
    ForEach(writers,i) {
      i->registerLogger(p);
    }
  }

  void unregisterLogger(LogParentIface* p)
  {
    ForEach(writers,i) {
      i->unregisterLogger(p);
    }
  }

  void addWriter (WriterPtr w)
  {
    ForEach(writers,i) {
      i->addWriter(w);
    }
  }
 
  void removeWriter(WriterPtr writer)
  {
    ForEach(writers,i) {
      i->removeWriter(writer);
    }
  }

  void removeAllWriters()
  {
    ForEach(writers,i) {
      i->removeAllWriters();
    }
  }

  // This is the default-services class, which is tried last, after trying any plugins.
  struct DefaultServices : public Plugin {

    Writer *createWriter(IssNode &root,const string &type,const string &fn,const MemoryLog &memlog)
    {
      // Instantiates the correct writer for the formats and file names specified
      if (type == "uvp") {
#       ifdef UVP_SUPPORT
        return new UvpWriter (fn,root,memlog); 
#       else
        RError("UVP output file format not supported."); 
#       endif      
      }
      else if (type == "dlz4") {
#       ifdef DAT_SUPPORT
        return new DatWriter(fn,root,memlog,true);
#       else
        RError("DAT output file format not supported."); 
#       endif     
      }
      else if (type == "elf") {        
        cerr << "Warning:  ELF output file format not supported.  Using DAT instead.\n\n"; 
        return createWriter(root,"dat",gen_default_name(fn,".dat"),memlog);
      } 
      else if (type == "hex" || type == "ihx") {        
        cerr << "Warning:  Intel Hex output file format not supported.  Using DAT instead.\n\n"; 
        return createWriter(root,"dat",gen_default_name(fn,".dat"),memlog);
      } 
      else if (type == "dat") {
#       ifdef DAT_SUPPORT
        return new DatWriter (fn,root,memlog);
#       else
        RError("DAT output file format not supported."); 
#       endif      
      }
      else if (type == FastTraceExt) {
#       ifdef DAT_SUPPORT
        return new FastDatWriter (fn,root,memlog);
#       else
        RError("Fast DAt output file format not supported."); 
#       endif      
      }
      else {
        return 0;
      }
    }

    Writer *createStreamWriter(IssNode &root,const string &type,ostream &os,const MemoryLog &memlog)
    {
      // Instantiates the correct writer for the formats and file names specified
      if (type == "uvp") {
#       ifdef UVP_SUPPORT
        return new UvpWriter (os,root,memoryLog()); 
#       else
        RError("UVP output file format not supported."); 
#       endif      
      }
      else if (type == "dat") {
#       ifdef DAT_SUPPORT
        return new DatWriter (os,root,memoryLog());
#       else
        RError("DAT stream output not supported."); 
#      endif      
      }
      else {
        return 0;
      }
    }

    Reader *createReader(IssNode *root,const string &type,const string &fn,const Args *args)
    {
      // Instantiates the correct reader for the formats and file names specified
      if (type == "uvp") {
#       ifdef UVP_SUPPORT      
        return new UvpReader(fn,root);
#       else
        RError("UVP input file format not supported."); 
#       endif
      }
      else if (type == "dlz4") {
#       ifdef DAT_SUPPORT      
        return new DatReader(fn,root,true);
#       else
        RError("DAT input file format not supported."); 
#       endif
      }
      else if (type == "tarmac") {
#       ifdef TARMAC_SUPPORT      
        return new TarmacReader(fn,root,false);
#       else
        RError("TARMAC input file format not supported."); 
#       endif     
      }
      else if (type == "hex" || type == "ihx") {
#       ifdef IHEX_SUPPORT      
        return new IHexReader(fn,root);
#       else
        RError("Intel Hex input file format not supported."); 
#       endif
      }
      
#     ifdef ELF_SUPPORT      
      ElfReaderFactory f;
      if (type == "elf" || f.isElfFile(fn)) {
        return f.create(fn,root,args); 
      }
#     else
      if (type == "elf") {
        RError("ELF input file format not supported."); 
      }
#     endif

#     ifdef DAT_SUPPORT      
      return new DatReader (fn,root);
#     else
      RError("DAT input file format not supported."); 
#     endif
    }

  };

  // The default handler for readers and writers.
  static DefaultServices defaultServices;
  static PluginList plugins(1,&defaultServices);  // This list stores all plugin objects, plus the default services object.

  // Returns true if 'root' has an installed mem handler.  If the item is a
  // system, then all of its children have to have installed memory handlers.
  bool checkMemHandlerInstalled(const IssNode &root)
  {
    if (root.is_core()) {
      // Iterate over all external memories.
      const MemInfos &mi = root.getMemInfo();

      // Handle main memory case.
      bool mh_installed = root.getMemHandler(0);

      // Handle any local memories.
      ForEach(mi,i) {
        if (i->_extern) {
          mh_installed = mh_installed || root.getMemHandler(i->_id);
        }
      }
      return mh_installed;
    } else {
      ForEach(root.children(),i) {
        if (!checkMemHandlerInstalled(**i)) {
          return false;
        }
      }
      return true;
    }
  }

  void ExtMemHelper::operator()(IssNode &root,const PluginList &plugins) const
  {
    if (root.hasExternalMem()) {
        
      // Iterate over all plugins, seeing if there's a memory handler that can
      // be installed.
      ForEach (plugins,piter) {
        (*piter)->createMemHandler(root);
      }
        
      if (!checkMemHandlerInstalled(root)) {
        RError("Model is configured to use an external memory, but none were installed via plugins.");
      }
    }
  }

  // This is the factory that creates new writers.
  WriterPtr createWriter(IssNode &root,const string &fn,const string &ext)
  {
    string type = getFileType(fn,ext);

    WriterPtr writer;

    // Iterate over all service providers, looking for something which can
    // handle this file type.
    ForEach (plugins,piter) {
      if ((writer = WriterPtr((*piter)->createWriter(root,type,removeFormatSpecifier(fn),memoryLog()))) != 0) {
        break;
      }
    }

    if (!writer) {
      RError("Unknown/unsupported file output type:  '" << type << "'.");
    }

    return writer;
  }

  // This is the factory that creates new writers which will target a specified stream.
  WriterPtr createStreamWriter(IssNode &root,ostream &os,const string &type)
  {
    WriterPtr writer;

    // Iterate over all service providers, looking for something which can
    // handle this file type.
    ForEach (plugins,piter) {
      if ((writer = WriterPtr((*piter)->createStreamWriter(root,type,os,memoryLog()))) != 0) {
        addWriter(writer);
        break;
      }
    }

    if (!writer) {
      RError("Output type '" << type << "' not supported for stream output."); 
    }

    return writer;
  }

  static Reader *createReader(IssNode *root,const string &fn,const string &ext,const Args *args)
  {  
    string type = getFileType(fn,ext);

    Reader *reader;
    
    ForEach(plugins,piter) {
      if ((reader = (*piter)->createReader(root,type,removeFormatSpecifier(fn),args)) != 0) {
        return reader;
      }
    }

    RError("Unknown/unsupported file input type:  '" << type << "'.");
  }

  // Install the system-call handler default object.  If we're doing a
  // multi-threaded sim, then we have to install one for each top-level object.
  void installSysCalls(IssNode *root)
  {
    // Handle the single-core case, just in case.
    if (ThreadedSim && !root->children().empty()) {
      ForEach(root->children(),i) {
        IssNode *node = *i;

        syscallHandlers.push_back(unique_ptr<SysCallExtHandler>(createDefaultSysCallMgr()));
        node->setSysCallHandler(syscallHandlers.back().get());
      }
    } else {
      syscallHandlers.push_back(unique_ptr<SysCallExtHandler>(createDefaultSysCallMgr()));
      root->setSysCallHandler(syscallHandlers.back().get());
    }
  }

  // Add on any plugins supplied by client code.
  void addPlugin(Plugin *pl)
  {
    plugins.push_front(pl);
  }

  // Load any plugin objects then install any relevant services that they may provide.
  void loadPlugins(IssNode *root)
  {
    // Then load any library specified on the command-line.
    const AnyOption::Values &plugs = options().getValues("plugin");
    
    ForEach(plugs,piter) {
      // Try to open the library.
      lt_dlhandle handle = loadLib(piter->c_str());

      // If that worked, try to get the main entry point and use that to get the
      // provider function.
      typedef Plugin *(*AdlPlugin)();
      AdlPlugin ap = (AdlPlugin)lookupSymbol(handle,AdlPluginEntry);
      Plugin *pl = ap();

      plugins.push_front(pl);
    }
  }

  // Once we've loaded everything, we go through and call the init code,
  // then re-process command-line parms, then do setup and install system-call
  // handlers.
  void handlePlugins(IssNode *root)
  {
    // Execute any initialization code. 
    ForEach(plugins,piter) {
      Plugin &pl = **piter;
      pl.init(options());
    }

    // Re-process the command-line, issuing errors for any unknown command-line
    // options.  This will take into account any options which were added by
    // plugins, so at this point, anything unknown is an error.
    options().reset();
    if (!options().processCommandLine()) {
      RError("Bad command-line.");
    }

    ForEach(plugins,piter) {
      Plugin &pl = **piter;

      // Execute any setup code.
      pl.setup(options(),*root);

      // Load any system call handlers supplied by plugins.  These should just
      // be the default system call objects, which also inherit form SysCallMgr,
      // so we should be able to dynamic-cast safely.
      if (SysCalls_enabled) {
        ForEach(syscallHandlers,i) {
          SysCallMgr &mgr = dynamic_cast<SysCallMgr&>( **i );
          mgr.addHandler(pl.createSysCallHandler());
        }
      }
    }
  }

  Reader *readInput(IssNode *root,const string &fn,const string &ext,
                    bool use_file_entry_point,bool load_file,bool init_memory,
                    const string &entry_point,const Args *args)
  {
    Reader *reader = createReader(root,fn,ext,args);
    readerInfoList.push_back(ReaderInfo(fn,reader->format_str()));
    reader->initMemory(init_memory);
    reader->useFileEntryPoint(use_file_entry_point);
    reader->setEntryPoint(entry_point);
    if (load_file) {
      reader->readFile();
    }
    return reader;
  }

  const ReaderInfoList &readerInfo()
  {
    return readerInfoList;
  }

  void dumpInitialState(Writer* w)
  {
    assert(w);
    w->post_initialize();
  }

  void dumpInitialState(WriterContext &wc)
  {
    ForEach(wc.writers(),i) {
      (*i)->post_initialize();
    }
  }

  void dumpInitialState()
  {
    ForEach(writers,i) {
      dumpInitialState(*i);
    }
  }


  void dumpFinalState(Writer* w)
  {
    assert(w);
    w->pre_finalize();
    w->writeResults();
    w->post_finalize();
  }

  void dumpFinalState(WriterContext &wc)
  {
    ForEach(wc.writers(),i) {
      (*i)->pre_finalize();
      (*i)->writeResults();
      (*i)->post_finalize();
    }
  }

  void dumpFinalState()
  {
    ForEach(writers,i) {
      dumpFinalState(*i);
    }
  }

  // If fast tracing is enabled, then we only care about tracing instructions.
  // This speeds things up a tad.  Otherwise, trace everything.
  uint64_t defaultTraceFlags()
  {
    return (FastTrace) ? (ttInstrPrefetch | ttInstr) : (uint64_t)(-1);
  }

  void startLogging(WriterContext &wc,IssNode &root)
  {
    if (!root.tracing_on_r()) {
      root.set_tracing_r(defaultTraceFlags());
      dumpInitialState(wc);
      wc.startLogging();
      root.tracingOn();
    }
  }

  void startLogging(IssNode &root)
  {
    ForEach(writers,i) {
      startLogging(*i,*i->node());
    }
  }

  void stopLogging(WriterContext &wc,IssNode &root)
  {
    if (root.tracing_on_r()) {
      root.tracingOff();
      root.clear_tracing_r();
    }
    dumpFinalState(wc);
  }

  void stopLogging(IssNode &root)
  {
    ForEach(writers,i) {
      stopLogging(*i,*i->node());
    }
  }

  void finalizeLogging(WriterContext &wc)
  {
    wc.doneLogging();
  }

  // This really only matters for MT logging: Shut down the logging threads so
  // that we can safely dump final state.
  void finalizeLogging()
  {
    ForEach(writers,i) {
      finalizeLogging(*i);
    }
  }

  void finalizePlugins(IssNode &root)
  {
    ForEach(plugins,piter) {
      Plugin &pl = **piter;

      pl.finalize(root);
    }
  }
                                                                                                                       
  void startSlicing(IssNode &root,Writer* w)
  {
    dumpInitialState(w);
    root.tracingOn();
  }

  void startSlicing(WriterContext &wc,IssNode &root)
  {
    dumpInitialState(wc);
    root.tracingOn();
  }

  void startSlicing(IssNode &root)
  {
    ForEach(writers,i) {
      startSlicing(*i,*i->node());
    }
  }

                                                                                                                         
  void stopSlicing(IssNode &root,Writer* w)
  {
    root.tracingOff();
    dumpFinalState(w);
  }

  void stopSlicing(WriterContext &wc,IssNode &root)
  {
    root.tracingOff();
    dumpFinalState(wc);
  }

  void stopSlicing(IssNode &root)
  {
    ForEach(writers,i) {
      stopSlicing(*i,*i->node());
    }
  }


  static void printHierarchy(ostream &o,const string &pfx,const IssNode &node)
  {
    o << pfx << node.name() << "\n";
    ForEach(node.children(),i) {
      printHierarchy(o,pfx + "  ",*(*i));
    }
  }

  static bool originalTraceFlag;

  void handleInitialState(bool dumpstate,IssNode &root)
  {
    root.clear_tracing_r();
    if (originalTraceFlag) {
      startLogging(root);
    } else if (dumpstate) {
      dumpInitialState();
    }
  }

  void handleFinalState(bool dumpstate,IssNode &root)
  {
    if (DumpJitCode) {
      root.dump_jit_code();
    }
    finalizeLogging();
    finalizePlugins(root);
    if (root.tracing_on_r()) {
      stopLogging(root);
    } else if (dumpstate) {
      dumpFinalState();
    }
  }

  bool haveMoreInput()
  {
    return !persistentReaders.empty();
  }

  void walk_hierarchy_internal(ostream &o,const IssNode &node)
  {
    setAttrData(node.getAttrData());
    o << "\n<node>\n"
      << "<name> " << node.name() << " </name>\n"
      << node.getRegInfo()
      << node.getMmuInfo()
      << node.getExceptionInfo()
      << node.getCacheInfo();
    if (!node.children().empty()) {
      o << "<children>\n";
      ForEach(node.children(),i) {
        walk_hierarchy_internal(o,**i);
      }
      o << "</children>\n";
    }
    o << "</node>\n";
  }

  void walk_hierarchy(ostream &o,const IssNode &node)
  {
    o << "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>\n\n";
    walk_hierarchy_internal(o,node);
  }

  // Handles all of the default naming stuff.  Returns a filename and a file
  // type, given inputs which may be empty, plus the defaults.
  pair<string,string> getFinalFileNameAndType(string outputFileName,string outputFileFormat,
                                              const string &defaultDir,const string &defaultInput,
                                              const string &defaultSfx)
  {
    if (outputFileName.empty()) {      
      if (outputFileFormat.empty()) {
        outputFileFormat = getExtension(defaultInput);
      }
      outputFileName = gen_default_name(defaultInput,defaultSfx+".out." + outputFileFormat);
      if (!defaultDir.empty()) {
        outputFileName = defaultDir + '/' + outputFileName;
      }
    } else if (outputFileFormat.empty()) {
      outputFileFormat = getExtension(outputFileName);
    }    

    return make_pair(outputFileName,outputFileFormat);
  }

  // Creates an appropriate writer for a single output file.
  WriterPtr processOutputFile(WriterContext &wc,IssNode *root,string outputFileName,string outputFileFormat,
                            const string &defaultDir,const string &defaultInput,
                            const string &defaultSfx,ostream *log)
  {
    auto out = getFinalFileNameAndType(outputFileName,outputFileFormat,defaultDir,defaultInput,defaultSfx);

    auto w = createWriter(*root,out.first,out.second);
    wc.addWriter(w);
    if (log && !QuietMode) {
      *log << "Opening output filename:  " << w->filename() << " (" << w->format_str() << ")\n";
    }
    return w;
  }

  WriterPtr processOutputFile(IssNode *root,string outputFileName,string outputFileFormat,
                            const string &defaultDir,const string &defaultInput,
                            const string &defaultSfx,ostream *log)
  {
    return processOutputFile(writers.back(),root,outputFileName,outputFileFormat,
                      defaultDir,defaultInput,defaultSfx,log);
  }

  // If we're using multi-file, then given the root that's not a core, create
  // writer contexts for each child item.  Otherwise, create the writer for the
  // top-level item.
  WriterList processMultiOutputFile(IssNode *root,string outputFileName,string outputFileFormat,
                                    const string &defaultDir,const string &defaultInput,
                                    const string &defaultSfx,ostream *log)
  {
    WriterList wl;
    if (MultiFileTrace) {
      // installLoggers has already checked that we have a system, so no need to
      // check that here.

      auto out = getFinalFileNameAndType(outputFileName,outputFileFormat,defaultDir,defaultInput,defaultSfx);

      // Divide up the name into the extension and the base portion of the name,
      // so that we can then append the core-id easily to the base portion.  We search for the base_name 
      // in the overall path, and find the extension relative to that, in order to not have any issues with
      // "." in directories.
      auto fp = split_file_name(out.first);

      int count = 0;
      assert(writers.size() == root->children().size());

      auto witer = writers.begin();
      ForEach(root->children(),i) {
        IssNode *node = *i;

        ostringstream ss;
        ss << fp.first << "_" << count++ << fp.second;

        auto w = shared_ptr<Writer>(createWriter(*node,ss.str(),out.second));
        witer->addWriter(w);
        wl.push_back(w);
        if (log && !QuietMode) {
          *log << "Opening output filename:  " << w->filename() << " (" << w->format_str() << " for " << getPathName(node,":") << ")\n";
        }
        ++witer;
      }
    } else {
      if (threadedSim() && root->tracing_on_r()) {
        RError("Multi-threaded tracing is only supported if multi-file-tracing is enabled.");
      }

      // Just create a standard single output file.
      wl.push_back(processOutputFile(root,outputFileName,outputFileFormat,defaultDir,defaultInput,defaultSfx,log));
    }
    return wl;
  }

  // Creates an appropriate writer for each specified output file.
  void processAllOutput(IssNode *root)
  {
    string default_outputFileFormat = options().getOption("of");

    const AnyOption::Values &outputs = options().getValues("o");
    if (outputs.empty()) {
      processMultiOutputFile(root,string(),default_outputFileFormat,string(),
                             options().getArgv(options().getArgc()-1),string(),&cerr);
    } else {
      ForEach(outputs,oiter) {    
        // Skip any --no-output options which might be scattered in the list, if
        // the user override the flag.
        if (*oiter == options().flagUnset()) continue;
        processMultiOutputFile(root,*oiter,default_outputFileFormat,string(),options().getArgv(options().getArgc()-1),string(),&cerr);
      }
    }
  }

  // This reads inputs from the persistent readers, removing any from the list
  // that have finished.  Returns false when finished.
  bool readNextInput(IssNode &root)
  {
    ++TestCount;
    if (MaxTestCount && (MaxTestCount >= TestCount)) {
      return false;
    }
    bool did_something = false;
    auto iter = persistentReaders.begin();
    for ( ; iter != persistentReaders.end(); ) {
      bool activity = (*iter)->readFile();
      if ( !activity || (*iter)->finished() ) {
        ReaderList::iterator tmp = iter++;
        persistentReaders.erase(tmp);
      } else {
        iter++;
      }
      did_something = did_something || activity;
    }
    if (did_something) {
      handleInitialState(true,root);
    }
    return did_something;
  }

  // This reads in all input files.  If called repeatedly, it repeats the
  // same inputs, i.e. you can reset the model, then call this again to
  // repeatedly run the same stuff.
  void processAllInput(IssNode &root)
  {
    int num_input_files = options().getArgc();
    persistentReaders.clear();
    // For each input file given, instantiate the correct reader and read it in.
    string inputFileFormat = options().getOption("if");
    for (int i=0; i<num_input_files; i++) {
      string inputFileName = options().getArgv(i);
      size_t found = inputFileName.find(inputFileFormat);
      // Trace run control
      if (options().getFlag("trace-run")) { // TODO: would better use boolean variables instead of getFlag()
        if (found != string::npos) {
          unique_ptr<Reader> reader(createReader(&root,inputFileName,inputFileFormat,NULL));
          if (!reader->hasTraceSupport()) {
            cerr << "trace reading run-control is currently under construction.\n";
          }
          else {
            reader->initTraceReading();
            tr_runctrl._reader=move(reader);
            // tr_runctrl.reader_init(reader);
            // runctrl = &tr_runctrl;
            // set run-control
            setRunControl(&tr_runctrl);
          }
        }
        else {
          cout << "warning: ignore none trace-run input.\n";
        }
      } // end if (options().getFlag("trace-run")) 
      else {
        unique_ptr<Reader> reader(readInput(&root,inputFileName,inputFileFormat,UseFileEntryPoint,true,true,EntryPoint));
        QPrint("Opening input filename:  " << reader->filename() << " (" << reader->format_str() << ")");
        if (!reader->finished()) {
          persistentReaders.push_back(move(reader));
        }
      }      
    }
  }

  void displayPreamble(const IssNode &node)
  {
    cerr << node.getModelPreamble() << "\n";
  }

  SimType simMode()
  {
    return SimMode;
  }

  bool threadedSim()
  {
    return ThreadedSim;
  }

  // Does all of the model setup.  Returns false if main
  // should exit.  Throws runtime_error for problems.
  bool processOptions(int argc,const char *argv[],bool error_unknown_options)
  {
    unsigned dms = BlockMemory::DefaultMemorySize;

    // Standard options.
    // KEY_FUNC: setFlags
    options().addUsage(":  [options] <dat file>");
    options().setFlag  ("help",                "h",       "Display help.");
    options().setFlag  ("verbose",             "v",       "Verbose mode.  Default = "+btos(VerboseMode)+".");
    options().setFlag  ("quiet",               "q",       "Quiet mode.  Default = "+btos(QuietMode)+".");
    options().setFlag  ("debug",               "d",       "Debug mode.  Default = "+btos(DebugMode)+".");

    options().setFlag  ("lax",                 "l",       "Lax mode.  If true, then an attempt to specify a value for an unknown resource,\n"
                        "such as a register, will result in a warning, rather than an error.  Default = "+btos(LaxMode)+".");
    options().setFlag  ("warn-mem-overwrite",             "Issue a warning if a memory address is initialized more than once.  This option may be negated.\nThe default is true.");
    options().setFlag  ("trace",               "ti",      "Trace mode.  Intermediate results will be sent to any output files.  Default = false.");
    options().setFlag  ("mt-trace",            "mti",     "Multi-threaded trace mode.  Tracing actions are performed using a background thread.  Default = false.");
    options().setFlag  ("multi-file-trace",    "mft",     "Enable multi-file trace.  Each core (with -n > 1) creates a separate trace file.  Default = " + btos(MultiFileTrace) + ".");
    options().setFlag  ("use-file-entry-point","fep",     "Use the entry point specified in input files for setting the program counter of cores.\n"
                        "Only valid for readers which support this feature, which is currently just the ELF format.\n"
                        "Default = "+btos(UseFileEntryPoint)+".");
    options().setOption("entry-point",	       "ep",      "Use user-defined entry point for setting the program counter of cores.\n"
                        "Only valid for readers which support this feature, which is currently just the ELF format.\n"
                        "Default = none.");
    options().setFlag  ("dump-options",                   "Dump command-line options to any output files.  Default = true.\n");
    options().setFlag  ("syscall-enabled",     "sce",     "Enable system call support.");
    options().setFlag  ("model-info",          "mi",      "Display model information and exit.");

    options().setOption("num-top-items",       "n",       "Replicate the top-level item n times, creating a system of items."); 
    options().setOption("input-format",        "if",      "Input file format.  Input is expected in the specified file format.");  
    options().setOption("output-format",       "of",      "Output file format.  Output is produced in the specified file format. Default = dat.");
    options().setOption("output",              "o",       "Output file name.  Output is directed to the given file name.\n"
                        "Negate with 'no-output' to turn off output.");
    options().setOptionalValue("output");

    options().setFlag  ("multi-threaded-sim",  "mt",      "Simulate using the multi-threaded kernel.  Default is "+btos(ThreadedSim)+".");

    options().setFlag  ("jit",                 "j",       "Enable JIT mode of execution. Default = no-jit.");
    options().setFlag  ("jit-dump",                       "Dump generated code at end of simulation.");
    options().setOption("jit-mt",                         "Enable/disable multi-threaded JIT compilation (compilation of basic blocks in a background thread.\n"
                                                          "Default = "+itos(TranslationParms::mt_jit_compile())+".  Used as a flag, e.g. --jit-mt, implies one\n"
                                                          "worker thread. Provide an integer to specify the number of compiler threads to use, e.g. --jit-mt=2\n"
                                                          "creates 2 compiler threads.  Negate with --no-jit-mt or use a value of 0 to turn of multi-threading.");
    options().setOptionalValue("jit-mt");
    options().setOption("jit-max-bb-size",                "Specify maximum number of target instructions allowed\n"
                        "in a single JIT basic block.  Default = "+itos(TranslationParms::max_bb_instr_count())+".");
    options().setOption("jit-max-bb-count",               "Specify max number of basic blocks allowed before a reset.\n"
                        "Default = "+itos(TranslationParms::max_bb_count())+".");
    options().setOption("jit-hot-count",                  "Specify threshold at which point we consider a block to be 'hot' and JIT it.\n"
                        "Default = "+itos(TranslationParms::jit_bb_hot_threshold())+".");
    options().setOption("dc-hot-count",                   "Specify threshold at which point we consider a block to be 'hot'\nand byte-compile it for the decode-cache."
                        "Default = "+itos(TranslationParms::dc_bb_hot_threshold())+".");

    options().setFlag  ("decode-cache",        "dc",      "Enable the decode-cache mode of execution. Default = no-decode-cache.");
    options().setOption("dc-buf-size",                    "Specify the size of the decode-cache buffer (in instructions).");

    options().setOption("td-count",                       "Set the temporal-decoupling threshold count to the specified value.");
    options().setOption("td-incr",                        "Set the temporal-decoupling increment to the specified value.");
    options().setOption("max-instr-count",     "mic",     "Max number of instructions to execute (0 is off).  Default = "+itos(DefaultMaxInstrCount)+".");
    options().setOption("max-tests",                      "Max number of tests to execute, if input files have multiple tests.  Default = 0 (no max specified.");
    options().setOption("memory-size",         "ms",      "Memory size, if the model was compiled with a block (non-sparse) memory.  Default = "+itos(dms)+".");
    options().setFlag  ("print-count",         "pt",      "Print total count of instructions including speed of simulation.  Default = on.");
    options().setOption("heartbeat",                      "Specify a heartbeat value.  Every n instructions (per core), information will be displayed.\n"
                        "0 = off.  Default = 0.");
    options().setOption("eval",                           "Evaluate the argument within the scripting interpreter.  This option may be repeated.");
    options().setOption("plugin",                         "Register a plugin (shared object).");
    options().setFlag  ("trace-run",                      "Trace reading run-control (for HWSW debug)");  //change to flag
    options().useCommandArgs(argc,argv);

    // Allow unknown options for now- they'll be checked again in loadPlugins.
    options().errorOnUnknowns(false);

    if (!options().processCommandLine()) {
      RError("Bad command-line.");
    }

    // Turn error checking back on.
    options().errorOnUnknowns(error_unknown_options);

    if (options().getFlag("h")) {
      options().printUsage();
      return false;
    };

    VerboseMode = options().getFlag("v",VerboseMode);

    QuietMode = options().getFlag("q",QuietMode);
  
    DebugMode = options().getFlag("d",DebugMode);

    LaxMode = options().getFlag("l",LaxMode);

    UseFileEntryPoint = options().getFlag("use-file-entry-point",UseFileEntryPoint);
    
    EntryPoint = options().getOption("entry-point",EntryPoint);
 
    InstrMax = options().getOption("mic",DefaultMaxInstrCount);

    Heartbeat = options().getOption("heartbeat",Heartbeat);

    MultiFileTrace = options().getFlag("mft",MultiFileTrace);

    NumCores = options().getOption("n",NumCores);

    ThreadedSim = options().getFlag("mt",ThreadedSim);

    // Decide on the type of simulator to use.  JIT takes priority (arbitrary
    // decision).
    if (options().getFlag("jit",false)) {
      SimMode = SimJit;
    } 
    else if (options().getFlag("decode-cache",false)) {
      SimMode = SimDc;
    }

    DumpJitCode = options().getFlag("jit-dump",DumpJitCode);

    if (options().haveOption("jit-mt")) {
      if (!options().getFlag("jit-mt")) {
        TranslationParms::set_mt_jit_compile(0);
      } else {
        TranslationParms::set_mt_jit_compile(options().getOption("jit-mt",(int)TranslationParms::mt_jit_compile()));
      }
    }
    TranslationParms::set_max_bb_instr_count(options().getOption("jit-max-bb-size",(int)TranslationParms::max_bb_instr_count()));
    TranslationParms::set_max_bb_count(options().getOption("jit-max-bb-count",(int)TranslationParms::max_bb_count()));
    TranslationParms::set_jit_bb_hot_threshold(options().getOption("jit-hot-count",(int)TranslationParms::jit_bb_hot_threshold()));
    TranslationParms::set_dc_bb_hot_threshold(options().getOption("dc-hot-count",(int)TranslationParms::dc_bb_hot_threshold()));
    TranslationParms::set_dc_buffer_size(options().getOption("dc-buf-size",(int)TranslationParms::dc_buffer_size()));

    MaxTestCount = options().getOption("max-tests",(int)MaxTestCount);

    // Set system-call enabled flag (it's disabled unless the user explicitly enables it) 
    SysCalls_enabled = options().getFlag("sce",SysCalls_enabled);
	
    originalTraceFlag = options().getFlag("trace",false);

    // Using this option is the same as -trace, in terms of turning on tracing.
    ThreadedTrace = options().getFlag("mt-trace",false);
    if (ThreadedTrace) {
      originalTraceFlag = true;
    }

    // If we're using multi-threading and we want tracing, then we need
    // multi-threaded tracing.  This can produce either separate files or a
    // single file.  In the case of a single file, instructions are merged
    // according to time stamps in order to create a correct total-order trace.
    if (ThreadedSim && originalTraceFlag) {
      ThreadedTrace = true;
    }

    return true;
  }

  bool processRoot(const IssNode* root)
  {
    // This should be near the top b/c we don't want any other output.
    // Otherwise, we'll get stuff that no longer make this a valid XML file.
    if (options().getFlag("mi")) {
      if (root) {
        walk_hierarchy(cout,*root);
      }
      return false;
    }

    // Configure the memory log hierarchy.
    memoryLog().add_nodes(*root);

    if (VerboseMode) {
      cout << "System hierarchy:\n";
      printHierarchy(cout,"  ",*root);
      cout << "-----------------\n";
    }

    return true;
  }

  // Create the top-level item.  This may be a system of items if n > 1.
  IssNode *createRoot(unsigned n,unsigned &coreid,RootCreator creator)
  {
    if (n == 1) {
      // User didn't specify a replication value, so just directly return the
      // node.
      return (creator)(coreid);
    } else {
      // User did specify a replication value, so we create a system to hold n
      // top-level elements.
      IssSystem *sys = new IssSystem("procs","sys",0);
      ForRange(n,i) {
        IssNode *child = (creator)(coreid);
        std::ostringstream ss;
        ss << child->name() << i;
        child->setName(ss.str());
        child->setParent(sys);
        sys->add_child(child);
      }
      return sys;
    }
  }

  // Create the top-level item.  This may be a system of items if the user
  // requested that the top-level item should be replicated.
  static IssNode *createRoot(RootCreator creator)
  {
    unsigned coreid = 0;
    
    return createRoot(NumCores,coreid,creator);
  }

  // We want to use the MT safe logger if threaded simulation has been
  // requested, but *not* if the only output file is the special fast-trace type
  // that we support.  In that case, we use the non-mt safe logging manager,
  // since the whole point is to reduce the overhead in order to try and catch
  // behavior that occurs when normal tracing is off but does not occur when
  // tracing is enabled.
  bool useMtSafeLogger()
  {
    // No threaded sim, then don't bother.
    if (!threadedSim()) {
      return false;
    }

    const AnyOption::Values &outputs = options().getValues("o");

    // If we have a single output file with an extension of ".fdat", then we
    // don't bother either, since this indicates that we want a fast trace.
    if (outputs.size() == 1 && getExtension(outputs.front()) == FastTraceExt) {
      FastTrace = true;
      return false;
    }

    // Otherwise, we want mt safe if we're running with threading enabled.
    return threadedSim();
  }

  // Main initialization entry point for when you don't want to create a node.
  IssNode *init(IssNode *root,int argc,const char *argv[],RootCreator creator,
                bool error_unknown_options,const ExtMemHelper &ext_mem_helper) 
  {
    if (!processOptions(argc,argv,error_unknown_options)) {
      return 0;
    }

    if (argc < 2) {
      options().printUsage();
      RError("No options specified.");
    }

    // Instantiate a model object if one isn't supplied.
    if (!root) {
      assert(creator);
      root = createRoot(creator);
    }
    assert(root);

    // This only applies to the block-memory, since the sparse memory always
    // supports a full 64-bit address.
    if (options().haveOption("ms")) {
      root->setMemorySize(options().getOption("ms",BlockMemory::DefaultMemorySize));
    }

    // Create the main logging object, into which all individual logging events
    // will be installed.
    installLoggers(root);
    
#   if !defined(_MSC_VER) && !defined(__MINGW32__)
    scripter().setRoot(root);
    scripter().setRunCtrl(runctrl);
#   endif

    // If so instructed, print the hierarchy and exit.
    if (!processRoot(root)) {
      return 0;
    }

    // Evaluate any 'eval' arguments.
#   if !defined(_MSC_VER) && !defined(__MINGW32__)
    try {
      auto evals = options().getValues("eval");
      ForEach(evals,i) {
        string result;
        if (!scripter().eval(result,*i)) {
          RError(result);
        }
      }
    }
    catch (runtime_error &err) { ; }
#   endif

    initDynLoader();

    // Install the system-call emulation handler(s).
    installSysCalls(root);

    // Load any plugins that were specified.  
    loadPlugins(root);

    // Process the plugins.  Note that this also, at this point, finalizes the
    // option database, attempting to process any unknown options (in case a
    // plugin added options).  At this point, unknown options trigger an error,
    // since we've already processed the original options and anything added by
    // plugins.
    handlePlugins(root);

    // Set the temporal-decoupling threshold to the max-instruction-count value
    // in case we're using the JIT or trace-cache.  This way, we'll exit at that
    // point and then check our max-instruction-count and then halt.
    if (InstrMax) {
      root->set_td_threshold(InstrMax);
      root->set_td_increment(1);
    }

    // Set any temporal-decoupling values, if specified.  Really only useful for
    // MP simulations with the JIT.
    bool td_set = false;
    if (options().haveOption("td-count")) {
      root->set_td_threshold(options().getOption("td-count",0));
      td_set = true;
    }
    if (options().haveOption("td-incr")) {
      root->set_td_increment(options().getOption("td-incr",0));
    }

    // If we're simulating with the JIT and we have an MP system, then set the
    // td threshold to a default value if not set, since otherwise we won't
    // switch between cores.  If the threshold remains 0, then the model doesn't
    // support temporal decoupling and we issue an error.  We disable this
    // check if we're using the multi-threaded kernel, since we don't have to
    // do any round-robin scheduling, in that case.
    if (!ThreadedSim) {
      // If not multi-threaded, then if we're using a translating ISS with
      // multiple cores, we use the TD API to perform the context switches.
      if ((SimMode == SimJit || SimMode == SimDc) && !root->is_core()) {
        if (!td_set) {
          root->set_td_threshold(DefaultThreshold);
        }
        if (!root->get_td_threshold()) {
          RError("Model does not support temporal decoupling.  MP JIT and MT simulations are thus not supported.");
        }
      }
    } else {
      // For MT simulation, we use the TD API to enforce progress: We don't want
      // one thread executing a million instruction ahead of the others.
      // However, we don't require it- we just set a default if we can.
      if (!root->hasMultiThreadedSupport()) {
        RError("Model does not support multi-threaded simulation.");
      }
      if (!td_set) {
        root->set_td_threshold(DefaultThreshold);
      }
    }

    // If the model has an external memory, then we have to get it from a
    // plugin, else we cannot continue.
    ext_mem_helper(*root,plugins);

    // Check to see if any node has a built-in register map.  This is generated
    // by assigning the attribute "debug" to a register or register-file and is
    // used when interfacing with a debugger.  This acts as a default- it can be
    // overridden by the setregmap function in a config file, hence why we do
    // this first.
    checkForRegMap(*root);

    // Check to see if a config script exists for configuring the design.
    checkForConfig(*root);
        
    // Only do this if we have an input file.  If we don't have any inputs, then
    // we don't create any writers.  This might be done, for example, if working
    // with a debugger; all input will come from the debugger and not from a file.
    int num_input_files = options().getArgc();
    if (num_input_files) {
      bool do_output = true;
      if (options().haveOption("o")) {  
        if (!options().getFlag("o")) {
          do_output = false;
        }
      }
      if (do_output) {
        processAllOutput(root);
      } else {
        QPrint("Test output has been disabled.");
      }

      // Now handle all input files.
      processAllInput(*root);
    }

    // If tracing is requested, start it.  Otherwise, just dump the initial state
    // If we have any input files.
    handleInitialState(num_input_files,*root);
    
    return root;
  }

  // Main init entry point which creates a node.
  IssNode* init (int argc,const char *argv[],RootCreator creator,
                 bool error_unknown_options,const ExtMemHelper &ext_mem_helper)
  {
    IssNode *root = 0;
    // Then initialize it.
    if (!(root = init(0,argc,argv,creator,error_unknown_options,ext_mem_helper))) {
      return 0;
    }

    return root;
  }

  // Run the simulation using the MT simulator if threadedSim() returns true,
  // else in single-threaded mode, using the simulation type specified.
  ProgramStatus runSim(IssNode &root,SimType smode)
  {
#   ifdef _MSC_VER
    return root.run(smode);
#   else
    if (threadedSim()) {
      if (!threadpool) {
        threadpool = createThreadPool(root);
      }
      return threadedSimRun(root,smode,threadpool);
    } else {
      return root.run(smode);
    }   
#   endif 
  }
  
  void finalize(WriterContext &wc)
  {
    // Note:  This is necessary to ensure that the writer's file is closed, since
    // it's garbage collected.
    removeAllWriters();
    exitDynLoader();
  }

  void finalize()
  {
    ForEach (writers,i) {
      finalize(*i);
    }
#   ifndef _MSC_VER
    delete threadpool;
    threadpool = 0;
#   endif
  }

  void resetSimulation(IssNode &root,RunControlIface *runctrl)
  {
    if (runctrl) {
      runctrl->reset(root);
    } else {
      root.reset();
    }
    root.resetGlobal();
    readerInfoList.clear();
  }

  // Any necessary init stuff for getting speed statistics to print correctly.  Call
  // this before any other I/O.
  void init_speed_display()
  {
# ifndef _MSC_VER
    // Use the current language locale for monetary conversions so that our speed
    // value will use a grouping characeter, e.g. in the US, a comma will be used.
    setlocale(LC_MONETARY,"");
# endif
  }

  // Returns a string representing the instructions/sec value.
  string get_speed(uint64_t instr_count,double secs)
  {
# if defined (_MSC_VER) || defined(__MINGW32__)
    // Microsoft doesn't implement strfmon.
    ostringstream ss;
    ss << (instr_count/secs);
    return ss.str();
# else
    // Otherwise, use currency format in order to get separators.
    char buf[100];
    strfmon(buf,sizeof(buf),"%!-n",(instr_count/secs));
    return buf;
# endif
  }
 
  // Returns the current time in microseconds.
  uint64_t getusec()
  {
#   ifdef _MSC_VER
    // It seems like Windows just tracks milliseconds, so we convert that
    // to microseconds in order to be consistent.
    struct _timeb t;
#   if _MSC_VER >= 1400
    _ftime_s (&t);
#   else
    _ftime (&t);
#   endif
    uint64_t s = t.time;
    s = s*1000 + t.millitm;
    return s*1000;
#   else
    struct timeval t;
    gettimeofday( &t, NULL );
    uint64_t s = t.tv_sec;
    return s*1000000 + t.tv_usec;
#   endif
  }


}

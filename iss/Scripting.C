//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
// This is the main implementation class for the scripting interface, which uses TCL as the 
// scripting language.
//
// To register a new command that will be useable by TCL, there are two steps:
//
// 1.  Define a new method in the Scripting class.  Use the Handler() macro to declare the method in
//     order to ensure consistency of arguments with the expected handler type.  For example, a new command
//     named "foo" would be declared as:
//
//       Handler(foo) { ... }
//
//     The name of the command should be the same as the name of the method.
//
// 2.  Add a new entry to the _commands array, located at the bottom of this file.  Use the HandlerDecl
//     macro to make this easier.  From the example above, you would add a line such as:
//
//       HandlerDecl(foo, "help string for foo"),
//

#include <cstring>
#include <iostream>
#include <iomanip>
#include <string>
#include <map>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <vector>
#include <assert.h>
#include <memory>

#include <tcl.h>

#include "rnumber/RNumber.h"

#include "helpers/Macros.h"
#include "helpers/AnyOption.h" 

#include "LoggingMgr.h"
#include "StringHelpers.h"
#include "NodeHelpers.h"
#include "Introspection.h"
#include "ModelInterface.h"
#include "SimInterface.h"
#include "Reader.h"
#include "Writer.h"
#include "ScriptingIface.h"
#include "DbgRegMap.h"
#include "ScriptLogger.h"
#include "MemoryLog.h"

using namespace std;
using namespace rnumber;

namespace adl {

  typedef std::list<std::string> StrList;

  bool evalTcl(std::string &result,const std::string& line, const vector<string> *args = 0);
  void interactiveTcl(const vector<string> *args = 0);
  void quitTcl();

  // Add command-line options to the option database.  Note the use of logical &&
  // to ensure that we evaluate all of the additions.
  static bool dummy ATTRIBUTE_UNUSED = 
    options().setFlag  ("cli",           "Command-line interface mode.") &&
    options().setOption("script", "sf",  "Execute commands from this file and then exit.") &&
    options().setOption("script-arg",    "Add the option as a command-line parameter to the script interpreter.") &&
    options().setOption("config", "cfg", "Execute the specified configuration script then proceed with normal operation.");

  // This reporter is used to stringify register state for display purposes.
  struct RegReporter : public ReportBase {

    RegReporter() : _first(true) {};

    void report_ctx_change(const IssNode *n,unsigned id,const char *name,unsigned context_num)
    {
      _ss << "\nContext=" << name << context_num << "\n";
      _first = false;
    }

    void report_ctx_off(const IssNode *n)
    {
      _ss << "\n";
      if (!_first) {
        _ss << "EndContext\n";
      }
    }

    void report_reg(const IssNode* node, unsigned id,const char* name, const rnumber::RNumber& value, bool shared)
    {
      _ss << name << "=0x" << hex << value << "\n";
    }

    void report_regfile(const IssNode* node, unsigned id,const char* name, int index, const rnumber::RNumber& value, bool shared)
    {
      _ss << name << dec << index << "=0x" << hex << value << "\n";
    }

    string str() const { return _ss.str(); };

    void reset()
    {
      _ss.str("");
      _ss.clear();
      _first = true;
    }

  protected:
    bool _first;
    ostringstream _ss;
  };

  static string uppercase(const string &s)
  {
    string n = s;
    transform (n.begin(), n.end(),   // source
               n.begin(),                    // destination
               (int(*)(int))toupper);                // operation
    return n;
  }

  // Returns true if this is a valid numeric format, false otherwise.
  static bool is_number(const string &str)
  {
    if (str.empty()) {
      return false;
    }
    if (str[0] == '0' && str.size() > 2) {
      if (str[1] == 'x' || str[1] == 'X') {
        return (str.find_first_not_of("0123456789abcdefABCDEF",2) == string::npos);
      } else if (str[1] == 'b' || str[1] == 'B') {
        return (str.find_first_not_of("01",2) == string::npos);
      }
    }
    // Default case- only numbers allowed.
    return (str.find_first_not_of("0123456789") == string::npos);
  }

# define Handler(X) static std::string X(void *obj,const tCmd *cmd,const Args &args)  
# define Self Scripting *self = (Scripting*)obj;

  //
  // The ScriptingIface class provides a TCL scripting interface for an ISS model.
  //
  class Scripting : public ScriptingIface {

  public:

    Scripting() :
      _logger(*this),
      _runctrl(0),
      _sim_mode(SimNormal),
      _root(0),
      _reader(nullptr),
      _script_logger(nullptr),
      _sttype(stNone)
    {
      initialize();
    }

    typedef std::map<std::string, tCmd*> CommandMap;

  protected:

    struct CliLogger : public LoggingIface, public LogParentIface {

      CliLogger(Scripting &cli) : _cli(cli) {};

      virtual void logBreakpoint(addr_t ea,unsigned handle,PtBase *);
      virtual void logException(unsigned handle,bool pre,const char *name);

      virtual LoggingIface *register_core(IssCore &core)
      {
        return this;
      }

      Scripting &_cli;

    };

    // Structure for managing actions tied to instruction addresses.
    struct IAddrSpec {
      enum Action { eBreak, eSliceOn, eSliceOff, eAnnotate };
      
      IAddrSpec(uint64_t address, Action action, unsigned long count, 
                const Args& args) :
        _address(address), _action(action), _count(count), _counter(0), _args(args) {}
      
      std::string translateAction(void) const;
         
      addr_t      _address;
      Action      _action;
      unsigned    _count;
      unsigned    _counter;
      Args        _args;
    };

    typedef std::vector<IAddrSpec> AddrSpecs;
    typedef std::map<addr_t, AddrSpecs> Actions;

    static tCmd                  _commands[];

    string                       _script;

    vector<string>               _args;

    CliLogger                    _logger;
    
    ScriptingControlIface       *_runctrl;

    SimType                      _sim_mode;

    IssNode                     *_root;

    Target                       _cur;

    std::string                  _path;

    map<std::string,WriterList>  _writers;
  
    unique_ptr<Reader>           _reader;

    WriterPtr                    _swriter;

    unique_ptr<ScriptLogger>     _script_logger;
    
    enum STraceType { stNone, stBuf, stStdout, stStderr };
    STraceType                   _sttype;
    ostringstream                _strace;

    Actions                      _actions;    

    AddrSpecs                    _pendingActions;

  public:
  
    ~Scripting(void) 
    {
    }

    static CommandMap &commands(void)
    {
      static CommandMap theCmds;

      return theCmds;
    }

    void set_script(const string &script)
    {
      _script = script;
    }

    void set_args(const vector<string> &args)
    {
      _args = args;
    }

    // The number of writers currently active.
    unsigned num_writers() const 
    { 
      return _writers.size(); 
    }

    const Actions& getActions(void) const 
    { 
      return _actions; 
    }

    const AddrSpecs& getPendingActions(void) const 
    { 
      return _pendingActions; 
    }

    Target setPath(const Target &trg)
    {
      Target old = _cur;
      _cur = trg;
      _path = getPathName(_cur.node(),":");
      return old;
    }

    tCmd* findCommand(const string&  name)
    {
      tCmd* retVal = 0;
      CommandMap::iterator iter = commands().find(name);
      if (iter != commands().end()) {
        retVal = (*iter).second;
      }

      return (retVal);
    }

    const tCmd *findCommand(const std::string& name) const
    {
      return const_cast<Scripting*>(this)->findCommand(name);
    }

    const char *docString(const string &name) const
    {
      if (const tCmd *cmd = findCommand(name)) {
        return cmd->doc;
      } else {
        return "<unknown>";
      }
    }

    bool isComment(const string &s) 
    {
      return (s.find_first_of("#",0) == 0);
    }

    // Use this macro for when errors are detected during argument processing.
    // It prints out the usage string for the command.
# define UsageError(cmd,msg) {                            \
      std::ostringstream ss;                              \
      ss << cmd << ": " << msg << "  " << self->docString(cmd); \
      throw std::runtime_error(ss.str());                 \
    }

    // Same as above- use this if we're in a member function of Scripting.
# define UsageErrorM(cmd,msg) {                                 \
      std::ostringstream ss;                                    \
      ss << cmd << ": " << msg << "  " << docString(cmd); \
      throw std::runtime_error(ss.str());                       \
    }

    // Use this macro for when usage information is not appropriate.
# define ExecError(cmd,msg) {                   \
      std::ostringstream ss;                    \
      ss << cmd << ": " << msg;                 \
      throw std::runtime_error(ss.str());       \
    }

    //---------------------------------------------------------------------------
    // findActiveNode -- return the active node in the model.  If except is true,
    //                   then a runtime_error will be thrown if no active node
    //                   is found.
    //---------------------------------------------------------------------------
    IssCore* findActiveNode(IssNode* current,bool except = false)
    {
      IssCore *returnValue = 0, *c = 0;
      if ((c = dynamic_cast<IssCore*>(current)) && current->is_active()) {
        returnValue = c;
      } else {
        const IssChildren& children = current->children();
        IssChildren::const_iterator iter = children.begin();
        while (iter != children.end() && returnValue == NULL) {
          IssNode* child = const_cast<IssNode*>(*iter);
          returnValue = findActiveNode(child,except);
          iter++;
        }    
      }
      if (except && !returnValue) {
        RError("No active node found.");
      }
      return returnValue;
    }

    IssNode *findActiveNode(bool except = false)
    {
      return findActiveNode(_root,except);
    }

    void addCommand(tCmd &cmd)
    {
      commands().insert(make_pair(cmd.name,&cmd));
    }

    void initialize()
    {
      for (int i = 0; _commands[i].name; i++) {
        _commands[i].obj = this;
        addCommand(_commands[i]);
      }
    }

    // Main entry point for when the scripting interface is invoked.  Either
    // executes a script non-interactively or else enters the interactive loop.
    void exec(void)
    {
      if ( !_script.empty() ) {
        evalFile( _script );
      } else {
        cout << "Entering interactive interpreter." << endl;
        interactiveTcl( &_args );
      }
    }

    bool eval(string &result,const string &cmd)
    {
      return evalTcl(result,cmd,&_args);
    }

    void evalFile(const string &fn)
    {
      // To execute the file, we just source it.
      string result, evalScript = "source " + fn;
      if (!evalTcl(result,evalScript,&_args)) {
        // If the interpreter returns an error, propagate this up the stack.
        RError(result);
      }
    }

    // This initialization routine is only used when we enter scripting mode.
    void scripting_initialize(IssNode &node,ScriptingControlIface &runctrl)
    {
      setRoot(&node);

      setRunCtrl(&runctrl);

      // Create the appropriate event-callback handler object.
      _script_logger.reset(runctrl.createScriptLogger());

      // Cor the CLI SliceMode is always on (so that updates go to the logger)
      node.set_tracing_r();
   
      // Register our logger.
      registerLogger(&_logger);
    }

    IssNode *root()
    {
      return _root;
    }

    void setRoot(IssNode* n) 
    { 
      _root = n;
      _cur.reset(n); 
      if (_root) {
        _path = _root->name();
      } else {
        _path.clear();
      }
    }

    void setRunCtrl(ScriptingControlIface *runctrl)
    {
      _runctrl = runctrl;
    }

    // Translate a symbol to an address; if already an address, just return it
    uint64_t lookup_symbol(const string& arg)
    {
      addr_t address = 0;
      if (!getAddr(address,arg)) {
        if (_reader) {
          address = _reader->symbol(arg.c_str());
        } else {
          RError("Invalid symbol or address specified.  Try calling load to load in the elf file that contains that symbol.");
        }
      }
      return address;
    }

    void shutdown()
    {
      stopLogging(*(_cur.node()));
      removeAllWriters();
      _writers.clear();
    }

    // Call this before exiting to make sure that the root node's destructor
    // (and childrens') get called.  Since Tcl_Exit actually exits, we can't
    // rely upon the normal delete occurring in main.  Proper shutdown is
    // important when using the JIT, since the compiler threads need to exit
    // gracefully.
    void deleteRoot()
    {
      delete _root;
      _root = 0;
    }

    bool active() const
    {
      return _root;
    }

    //---------------------------------------------------------------------------
    // addAction -- add an action to occur on an address or symbol
    //---------------------------------------------------------------------------
    void addAction(uint64_t address, 
                   IAddrSpec::Action action,
                   unsigned long count,
                   const Args& args)
    {
      // if the Action List is not there it will be created by this call
      // otherwise we will get the existing one
      Actions::iterator actionsIter = _actions.insert(make_pair(address,AddrSpecs())).first;
  
      // add the Action spec to the list
      (*actionsIter).second.push_back(IAddrSpec(address,action,count,args));
  
      if (VerboseMode) {
        cout << scripter() << endl;
      }
    }

    void addAction(uint64_t address, 
                   IAddrSpec::Action action,
                   unsigned long count,
                   const string& cmd,
                   const string& arg)
    {
      Args tmp_args;
      tmp_args.push_back(cmd);
      tmp_args.push_back(arg);
      addAction(address,action,count,tmp_args);
    }

    //---------------------------------------------------------------------------
    // removeAction -- remove an action set to occur on an address or symbol
    //---------------------------------------------------------------------------
    void removeAction(uint64_t address, 
                      IAddrSpec::Action action)
    {
      // are there any actions for the specified address/symbol ?
      map<uint64_t,vector<IAddrSpec> >::iterator entry = _actions.find(address);
      if (entry == _actions.end()) {
        RError("No entry found for " << hex << address);
      } else {
        // yes!  is there oneof the specified action type ?
        vector<IAddrSpec>& specs = (*entry).second;
        vector<IAddrSpec>::iterator iter = specs.begin();
        bool found = false;
        while (!found && iter != specs.end()) {
          if ((*iter)._action == action) {
            // yes, we found it - erase it
            found = true;
            specs.erase(iter);
          } else {
            iter++;
          }
        }
        if (!found) {
          RError("No action found for " << hex << address);
        } else if ((*entry).second.size() == 0) {
          // nothing left for this address/symbol
          _actions.erase(entry);
        }
      }

      if (VerboseMode) {
        cout << scripter() << endl;
      }
 
    }

    //---------------------------------------------------------------------------
    // findNodeByPath
    //---------------------------------------------------------------------------
    IssNode* findNodeByPath(const string& path)
    {
      auto p = splitPath(path);
      unsigned int level = 0;
      return _findNodeByPath(_root,p,level);
    }


    //---------------------------------------------------------------------------
    // _findNodeByPath
    //---------------------------------------------------------------------------
    IssNode* _findNodeByPath(IssNode* curr,const Args& path,unsigned level)
    {
      if (level < path.size()) {
        if (curr->name().compare(path[level]) == 0 && level == path.size()-1) {
          return curr;
        } else {
          IssNode* retVal = NULL;
          const vector<IssNode*>& children = curr->children();
          unsigned int idx = 0;
          while (!retVal && idx < children.size()) {
            retVal = _findNodeByPath(children[idx],path,level+1);
            idx++;
          }
          return retVal;
        }
      } else {
        return NULL;
      }
    }


    //---------------------------------------------------------------------------
    // formatAnnotation
    //---------------------------------------------------------------------------
    string formatAnnotation(const string& msg)
    {
      // %reg:regname
      // %reg:regname[index]
      // %mem:address
      // %node

      string retVal = msg;

      // first find and replace all the '%reg' tokens
      string::size_type i = retVal.find("%reg:");

      while (i != string::npos) {
        string::size_type next = retVal.find_first_of(" %\t",i+5);
        string regSpec = retVal.substr(i+5,next-(i+5));
        string orgVal = "%reg:";
        string reg = regSpec;
        string idxStr = "0";
        unsigned long idx = 0;
        uint64_t value = 0;
        stringstream valueStr;
        i = regSpec.find("[");
        next = regSpec.find("]");
        if (i != string::npos && next != string::npos) {
          // have an indexed register
          reg = regSpec.substr(0,i);
          idxStr = regSpec.substr(i+1,next-i-1);
          orgVal = orgVal + reg + "[" + idxStr + "]";
          char* endPtr = NULL;
          idx = strtol(idxStr.c_str(),&endPtr,0);
          if (strlen(endPtr) > 0) { 
            throw runtime_error("Invalid index specified");
          }
        } else if ((i == string::npos && next != string::npos) ||
                   (i != string::npos && next == string::npos)) {
          throw runtime_error("Invalid register index specified");
        } else {
          // just a simple register
          reg = regSpec;
          orgVal = orgVal + reg;
        }
        if (IssNode* active = findActiveNode()) {
          active->getReg(reg,0,value);
          valueStr << "0x" << hex << value;
          retVal.replace(retVal.find(orgVal),orgVal.length(),valueStr.str());
          i = retVal.find("%reg");
        }
      }

      // now all the %mem tokens
      i = retVal.find("%mem:");
      while (i != string::npos) {
        string::size_type next = retVal.find_first_of(" %\t",i+5);
        string addressStr = retVal.substr(i+5,next-(i+5));
        char* endPtr = NULL;
        unsigned long address = strtol(addressStr.c_str(),&endPtr,0);
        if (strlen(endPtr) > 0) { 
          throw runtime_error("Invalid address specified");
        }
        uint32_t value = _root->getMem(0, address,32);
        stringstream valueStr;
        valueStr << "0x" << hex << value;
        retVal.replace(i,5+addressStr.length(),valueStr.str());
        i = retVal.find("%mem:");
      }

      // then all the %node tokens
      i = retVal.find("%node");
      while (i != string::npos) {
        retVal.replace(i,5,findActiveNode()->name());
        i = retVal.find("%node");
      }

      return retVal;
    }

    pair<Target,string> parsePathInternal(IssNode *root,const string &pathstr,bool last_is_data = false) 
    {
      // cur does not get updated on a thread switch, it's up to us to update it
      // before calling parsePath so that if a full path is not specified, cur
      // will point to the currently active node.
      Target cur(_cur);
      try {
        assert(root);
        if (const CtxInfo *ctx = getThreadContext(*root)) {
          cur.set_ctx_id(ctx->_id);
        }
        cur.set_ctx(0,findActiveNode(_root,true)->getActiveCtx(cur.ctx_id()));
      }
      catch (runtime_error &) {
        // No contexts exist.
        cur.remove_ctx();
      }
      return parsePath(root,pathstr,cur,last_is_data);
    }

    string setBreak(const string &arg)
    {
      addr_t address;
      setBreakInt(address,arg,_cur);
      string rc;
      MkStr(rc,"Breakpoint added on node " + _cur.node()->name() << " at 0x" << hex << address);
      return rc;
    }

    int setBreakH(const string &arg)
    {
      addr_t address;
      return setBreakInt(address,arg,_cur);
    }

    int setBreakInt(addr_t &address,const string &arg, const Target &target)
    {
      if (target.node()->is_core()) {
        address = lookup_symbol(arg);
        return target.node()->setBreakpoint(address);
      } else {
        RError("setbrk:  Bad node.  A breakpoint may only be applied to a core.");
      }
    }

    int setWatchH(const string &arg,WatchType w)
    {
      addr_t address;
      return setWatchInt(address,w,arg,_cur);
    }

    int setWatchInt(addr_t &address,WatchType type,const string &arg, const Target &target)
    {
      if (target.node()->is_core()) {
        address = lookup_symbol(arg);
        return target.node()->setWatchpoint(address,type);
      } else {
        RError("setbrk:  Bad node.  A breakpoint may only be applied to a core.");
      }
    }

    string clearBreak(const string &path)
    {
      return clearBreakInt(path,_cur);
    }

    string clearBreakInt(const string& arg, const Target &target)
    {
      string returnValue;
      if (target.node()->is_core()) {
        uint64_t address = lookup_symbol(arg);
        target.node()->clearBreakpointByAddr(address);
        return "Breakpoint removed on node: " + target.node()->name(); 
      } else {
        RError("clrbrk:  Bad node.  A breakpoint may only be cleared from a core.");
      }
    }


    //---------------------------------------------------------------------------
    // simload:  Load a file into the model.
    // arg1:  filename.
    // arg2:  Optional:  Specify a format type to override the default obtained from the filename.
    // arg3:  Optional:  If 'setpc', then use file's entry-point.  Allowed as argument 2 if no format specified.
    //                   If '--', then switch to parsing extra arguments (flags for the reader).
    // argN:  Optional:  Additional flags to send to the reader for configuring it.
    //---------------------------------------------------------------------------
    Handler(simload)
    {
      Self;

      if (args.size() < 2) {
        UsageError(args[0],"No filename supplied.");
      }

      // Parsing is a bit more complicated than I'd like.
      unsigned arg = 1;
      string fn = getArg(args,arg);
      string fmt;
      bool usepc = false, eargs = false;
      Args extras;

      while (arg < args.size()) {
        string a = getArg(args,arg);
        if (usepc || eargs) {
          // It's an extra argument if we've already gotten 'setpc' or '--'.
          extras.push_back(a);
        } else if (a == "setpc") {
          usepc = true;
        } else if (a == "--") {
          eargs = true;
        } else if (fmt.empty()) {
          // If we haven't gotten a format yet and it's not one of the other
          // special flags, then we regard this as the format.
          fmt = a;
        } else {
          // Otherwise we'll just treat it as an extra argument.
          extras.push_back(a);
        }
      }

      // fix me: If we load multiple elf files then symbol lookups need to refer
      // to the correct elf file.
      self->_reader = unique_ptr<Reader>(readInput((self->_cur.node()),fn,fmt,usepc,true,true,string(),&extras));
      if (self->_reader) {
        return "File loaded:  "+args[1];
      } else {
        RError("Failed to load file:  " << fn);
      }
    }

    //---------------------------------------------------------------------------
    // setmem:  Set a memory value.  This is limited to a maximum of a 32-bit write.
    // arg1:  address.
    // arg2:  value
    // arg3:  Optional:  length, in bytes (1-4).
    //---------------------------------------------------------------------------
    Handler(setmem)
    {
      Self;

      if (args.size() < 3) {
        UsageError(args[0],"Bad arguments.");
      }

      unsigned arg = 1;
      uint64_t addr;
      if (!getAddr(addr,args,arg)) {
        UsageError(args[0],"Invalid address.");
      }

      uint32_t data;
      if (!getUInt(data,args,arg)) {
        UsageError(args[0],"Invalid memory value.");
      }

      uint32_t size;
      if (!getUInt(size,args,arg)) {
        size = 4;
      }
      size *= 8; //convert bytes to bits 

      self->_root->setMem(GlobalMemName,addr,data,size);

      string r;
      MkStr(r,"Wrote 0x" << hex << data << dec << " (" << size / 8 << " bytes) to 0x" << hex << addr);
      return r;
    }

    void get_mem_args(addr_t &base,unsigned &length,unsigned &size,const Args &args)
    {
      if (args.size() < 3) {
        UsageErrorM(args[0],"Bad arguments.");
      }

      unsigned arg = 1;

      if (!getAddr(base,args,arg)) {
        UsageErrorM(args[0],"Invalid base address.");
      }

      if (!getUInt(length,args,arg)) {
        UsageErrorM(args[0],"Invalid length.");
      }

      size = 4;
      string size_arg = getArg(args,arg);
      if (!size_arg.empty()) {
        if (!getUInt(size,size_arg)) {
          UsageErrorM(args[3],"Invalid read-size specifier.  Expected read-size in bytes:  1, 2, 4, or 8.");
        }
      }
    }

    //---------------------------------------------------------------------------
    // mem:  Show the memory contents of a model.
    // arg1:  Base address.
    // arg2:  Length in bytes.
    // arg3:  Read size (1, 2, 4, 8);
    // arg4:  Words per line.  Default is 4
    //---------------------------------------------------------------------------
    Handler(mem)
    {
      Self;

      addr_t   base;      
      unsigned length;
      unsigned size;

      self->get_mem_args(base,length,size,args);

      unsigned arg = 4;
      string wpl_arg = getArg(args,arg);

      // Words-per-line argument.
      unsigned wpl = 4;
      if (!wpl_arg.empty()) {
        if (!getUInt(wpl,wpl_arg)) {
          UsageError(args[4],"Invalid words-per-line argument.");
        }
      }

      string pfx(2,' ');
      uint64_t offset = base % size;

      switch (size) {
      case 1:
      case 2:
      case 4:
      case 8:
        break;
      default:
        UsageError(args[3],"Bad read-size argument.  Expected read size in bytes:  1, 2, 4, or 8.");
      }

      // Adjust the addresses to be word aligned for display purposes
      ostringstream result;
      unsigned totalBytes = 0;
      unsigned line_incr = (wpl * size);
      for (unsigned line = 0; totalBytes < length; line += line_incr) {
        // beginning of a line
        result << '\n' << setfill(' ') << setw(16) << hex << showbase << (base - (base % size) + line) << ": ";   
    
        for (unsigned int i = 0; i < offset; ++i) {
          result << pfx;
        }
    
        for (unsigned word = 0; word < wpl && totalBytes < length; word++) {
          // four bytes per word
          for (unsigned byte = offset; byte < size && totalBytes < length; byte++) {
            result << setfill('0') << setw(2) << noshowbase << hex << (unsigned int) self->_root->mem_read8(base + totalBytes);
            totalBytes++;
          }
          if (word < wpl-1) {
            result << ' ';
          }
          offset = 0;
        }
    
      }
  
      return result.str();
    }

    //---------------------------------------------------------------------------
    // memlist:  Return a list of memory values.
    // arg1:  base address.
    // arg2:  length in bytes.
    // arg3:  Optional:  Read size (1, 2, 4, 8).
    // arg4:  Optional:  Radix (char, dec, hex).
    //---------------------------------------------------------------------------
    Handler(memlist)
    {
      Self;

      addr_t   base;      
      unsigned length;
      unsigned size;

      self->get_mem_args(base,length,size,args);

      unsigned arg     = 4;
      string radix_arg = getArg(args,arg);

      enum Radix { rChar, rDec, rHex };
      Radix radix = rHex;
      if (!radix_arg.empty()) {
        if (radix_arg == "char") {
          if (size != 1) {
            UsageError(args[4],"A radix of 'char' is only valid with a read-size of 1.");
          }
          radix = rChar;        
        } else if (radix_arg == "dec") {
          radix = rDec;
        } else if (radix_arg == "hex") {
          radix = rHex;
        } else {
          UsageError(args[4],"Invalid radix specifier.  Expected 'char', 'dec', or 'hex'.");
        }
      }

      ostringstream result;
      string pfx;
      result.fill('0');
      if (radix == rHex) {
        result << hex;
        pfx = "0x";
      }

      switch (size) {
      case 1:
        if (radix == rChar) {
          for (unsigned addr = base; addr < (base+length); addr += 1) {
            result << (char)((unsigned int) self->_root->mem_read8(addr));
          }
        } else {
          for (unsigned addr = base; addr < (base+length); addr += 1) {
            result << pfx << setw(2) << (unsigned int) self->_root->mem_read8(addr) << ' ';
          }
        }
        break;
      case 2:
        for (unsigned addr = base; addr < (base+length); addr += 2) {
          result << pfx << setw(4) << (unsigned int) self->_root->mem_read16(addr) << ' ';
        }
        break;
      case 4:
        for (unsigned addr = base; addr < (base+length); addr += 4) {
          result << pfx << setw(8) << (unsigned int) self->_root->mem_read32(addr) << ' ';
        }
        break;
      case 8:
        for (unsigned addr = base; addr < (base+length); addr += 8) {
          result << pfx << setw(16) << (unsigned int) self->_root->mem_read64(addr) << ' ';
        }
        break;
      default:
        UsageError(args[3],"Invalid read-size specifier.  Expected 1, 2, 4, or 8.");        
      }
      
      return result.str();
    }

    //---------------------------------------------------------------------------
    // traceon::  Dump initial state and start logging.
    // arg1:  Filename
    // arg2:  Optional:  Format to use.
    //---------------------------------------------------------------------------
    Handler(traceon)
    {
      Self;

      unsigned arg = 1;

      string fn = getArg(args,arg);
      string fmt = getArg(args,arg);
  
      if (fn.empty()) {
        UsageError(args[0],"No file name specified.");
      }

      if (self->_writers.count(fn)) {
        ExecError(args[0],"Slicing is already enabled for file '"+fn+"'.");
      }

      // If no tracing is enabled, then enable everything.
      if (!self->_root->tracing_on_r()) {
        self->_root->set_tracing_r();
      }

      // The appropriate writer will be created by the factory and registered
      // internally.  Start from the root, in order to handle MP situations
      // properly.
      WriterList wl = processMultiOutputFile(self->_root,fn,fmt,string(),string(),string(),0);
      // We register the writer(s) here so that we can track it by filename and remove it
      // as requested.
      self->_writers.insert(make_pair(fn,wl));

      // This dumps the initial state.
      ForEach(wl,iter) {
        startSlicing(*(self->_cur.node()),iter->get());
      }

      return "Tracing to file: " + fn;
    }

    //---------------------------------------------------------------------------
    // traceoff:  Dump final state and stop logging.
    // arg1:  Optional:  Filename to stop slicing.  If none specified,
    //                   then slicing for all writers is disabled.
    //---------------------------------------------------------------------------
    Handler(traceoff)
    {
      Self;

      unsigned arg = 1;

      string fn = getArg(args,arg);

      WriterList wl;
      if (!fn.empty()) {
        auto iter = self->_writers.find(fn);
        if (iter == self->_writers.end()) {
          ExecError(args[0],"No slicing is active for file '" << fn << "'.");
        }
        wl.swap(iter->second);
        self->_writers.erase(iter);
      }

      // Remove one or all writers, depending upon whether a filename was specified.
      if (!wl.empty()) {
        // Dump the final state.
        ForEach(wl,witer) {
          stopSlicing(*(self->_cur.node()),witer->get());
          removeWriter(*witer);
        }
      } else {
        stopSlicing(*(self->_cur.node()));
        removeAllWriters();
        self->_writers.clear();
      }

      return "Tracing stopped.";
    }

    //---------------------------------------------------------------------------
    // straceon::  Turn on stream tracing.
    // arg1:  Writer type
    //---------------------------------------------------------------------------
    Handler(straceon)
    {
      Self;

      unsigned arg = 1;

      string fmt = getArg(args,arg);

      if (fmt.empty()) {
        UsageError(args[0],"No writer type specified.");
      }

      // Optional destination- right now, only stdout is supported.
      ostream *strace = &self->_strace;
      self->_sttype = stBuf;

      string dest;
      if (args.size() == 3) {
        dest = getArg(args,arg);
        if (dest == "stdout") {
          strace = &cout;
          self->_sttype = stStdout;
        }
        else if (dest == "stderr") {
          strace = &cerr;
          self->_sttype = stStderr;
        } else {
          RError("Invalid stream destination.  Currently, 'stdout' and 'stderr' are supported. ");
        }
      }

      // The appropriate writer will be created by the factory and registered internally.
      self->_swriter = createStreamWriter(*(self->_cur.node()),*strace,fmt);

      // If no tracing is enabled, then enable everything.
      if (!self->_root->tracing_on_r()) {
        self->_root->set_tracing_r();
      }

      return "1";
    }

    //---------------------------------------------------------------------------
    // straceget::  Get the tracing buffer (if used).  If stdout is being used, then
    //              flush stdout.
    // arg1:  Writer type
    //---------------------------------------------------------------------------
    Handler(straceget)
    {
      Self;

      if (self->_swriter) {
        self->_swriter->stream_done();
        switch (self->_sttype) {
        case stBuf: {
          string r = self->_strace.str();
          self->_strace.str(string());
          return r;
        }
        case stStdout:
          cout << flush;
        default:
          return "";
        }
      } else {
        RError("Stream tracing not currently enabled.");
      }
    }

    //---------------------------------------------------------------------------
    // straceoff:  Removes the stream writer.
    //---------------------------------------------------------------------------
    Handler(straceoff)
    {
      Self;

      if (self->_swriter) {
        removeWriter(self->_swriter);
        self->_swriter = 0;
        self->_strace.str(string());
        return "1";
      } else {
        return "0";
      }
    }

    //---------------------------------------------------------------------------
    // tracelist:  List all files currently being traced.
    //---------------------------------------------------------------------------
    Handler(tracelist)
    {
      Self;

      ostringstream o;
      StrList col1,col2;
      ForEach(self->_writers,i) {
        col1.push_back(i->first);
        col2.push_back(i->second.front()->format_str());
      }
      self->formatColumns(o,col1,col2);
      return o.str();
    }

    //---------------------------------------------------------------------------
    // inputlist:  List all files that have been read during this simulation.
    //---------------------------------------------------------------------------
    Handler(inputlist)
    {
      Self;

      ostringstream o;
      StrList col1,col2;
      ForEach(readerInfo(),i) {
        col1.push_back(i->_filename);
        col2.push_back(i->_fmt);
      }
      self->formatColumns(o,col1,col2);
      return o.str();
    }

    //---------------------------------------------------------------------------
    // setbrk:  Set a breakpoint.
    // arg1:  Symbol or address.
    //---------------------------------------------------------------------------
    Handler(setbrk)
    {
      Self;

      // first we will try to add the action
      if (args.size() < 2) {
        UsageError(args[0],"No symbol or address specified.");
      }

      // the occurrence count is optional.  If one is not specified, we'll break
      // every time we hit it, otherwise we'll only break when the occurrence 
      // count matches the counter.
      int count = 0;
      if (args.size() == 3) {
        char* endPtr = NULL;
        count = strtol(args[2].c_str(),&endPtr,0);
      }

      uint64_t address = self->lookup_symbol(args[1]);
      self->addAction(address, IAddrSpec::eBreak, count, args[0], args[1]);
  
      return self->setBreak(args[1]);
    }

    //---------------------------------------------------------------------------
    // clrbrk:  Clear a breakpoint.
    // arg1:  Symbol or address.
    //---------------------------------------------------------------------------
    Handler(clrbrk)
    {
      Self;

      // first we will try to remove the action
      if (args.size() < 2) {
        UsageError(args[0],"No symbol or address specified.");
      }

      uint64_t address = self->lookup_symbol(args[1]);
      self->removeAction(address, IAddrSpec::eBreak);
  
      return self->clearBreak(args[1]);  
    }

    //---------------------------------------------------------------------------
    // triggerexcpt:  Trigger the named exception.
    //---------------------------------------------------------------------------
    Handler(triggerexcpt)
    {
      Self;

      if (args.size() < 2) {
        UsageError(args[0],"No exception name specified.");
      }
      auto data = self->parsePathInternal(self->_root,args[1],true);
      return self->triggerExcptInt(data.second,data.first);
    }

    unsigned getExceptionFlag(const string &arg,const Target &target,const char *name)
    {
      const ExceptionInfos &infos = dynamic_cast<IssCore *>(target.node())->getExceptionInfo();
      auto ei = infos.begin();
      while (ei != infos.end()) {
        if (ei->_name == arg) {
          return ei->_flag;
        }
        ++ei;
      }
      RError(name << ": Invalid exception name '" << arg << "'.");
    }

    string triggerExcptInt(const string &arg,const Target &target)
    {
      if (target.node()->is_core()) {
        unsigned flag = getExceptionFlag(arg,target,"triggerexcpt");
        CtxState restore(target,true);
        _runctrl->genExceptions(flag,*target.node());
        return "Exception triggered on node: " + target.node()->name();
      } else {
        RError("triggerexcpt:  Bad node.  An exception may only be triggered on a core.");
      }
    }

    //---------------------------------------------------------------------------
    // cancelexcpt:  Cancel the named exception.
    //---------------------------------------------------------------------------
    Handler(cancelexcpt)
    {
      Self;

      if (args.size() < 2) {
        UsageError(args[0],"No exception name specified.");
      }
      auto data = self->parsePathInternal(self->_root,args[1],true);
      return self->cancelExcptInt(data.second,data.first);
    }

    string cancelExcptInt(const string &arg,const Target &target)
    {
      if (target.node()->is_core()) {
        unsigned flag = getExceptionFlag(arg,target,"cancelexcpt");
        CtxState restore(target,true);
        if (_runctrl->cancelExceptions(flag,*target.node())) {
          return "Exception canceled on node: " + target.node()->name();
        } else {
          RError("No such pending exception(s) to cancel on node: " + target.node()->name());          
        }
      } else {
        RError("cancelerexcpt:  Bad node.  An exception may only be triggered on a core.");
      }
    }

    //---------------------------------------------------------------------------
    // exceptlist:  List all valid exceptions.
    //---------------------------------------------------------------------------
    Handler(excptlist)
    {
      Self;

      if (args.size() > 1) {
        auto data = self->parsePathInternal(self->_root,args[1]);
        return self->exceptListInt(data.first);
      } else {
        return self->exceptListInt(self->_cur);
      }
    }

    string exceptListInt(const Target &target)
    {
      if (target.node()->is_core()) {
        const ExceptionInfos &infos = dynamic_cast<IssCore *>(target.node())->getExceptionInfo();
        string r;
        bool first = true;
        ForEach(infos,i) {
          if (!first) {
            r += " ";
          } else {
            first = false;
          }
          r += i->_name;
        }
        return r;
      } else {
        RError("excptlist:  Bad node.  Exceptions are only defined for cores.");
      }
    }

    //---------------------------------------------------------------------------
    // icount:  Return the instruction count of the current or named node.
    //---------------------------------------------------------------------------
    Handler(icount)
    {
      Self;

      if (args.size() > 1) {
        auto data = self->parsePathInternal(self->_root,args[1]);
        return self->instrCountInt(data.first);
      } else {
        return self->instrCountInt(self->_cur);
      }
    }

    string instrCountInt(const Target &target)
    {
      ostringstream ss;
      ss << target.node()->instr_count();
      return ss.str();
    }

    //---------------------------------------------------------------------------
    // symbol:  Return the address of a symbol.
    //---------------------------------------------------------------------------
    Handler(symbol)
    {
      Self;

      if (args.size() < 2) {
        UsageError(args[0],"No symbol specified.");
      }
      addr_t sym = self->lookup_symbol(args[1]);
      string s;
      MkStr(s,"0x" << hex << sym);
      return s;
    }

    // Parses a register name for an index.  Returns a pair of <name,index>.
    // If no index is specified, then default_index is used.
    pair<string,unsigned> parseRegName(const std::string &name,unsigned default_index)
    {
      pair<string,unsigned> ndata(name,default_index);
      string::size_type b = name.find("[");
      string::size_type e = name.find("]");
      if (b != string::npos && e != string::npos) {
        // have an indexed register
        ndata.first = name.substr(0,b);
        string idxStr = name.substr(b+1,e-b-1);
        char* endPtr = NULL;
        unsigned idx = strtol(idxStr.c_str(),&endPtr,0);
        if (strlen(endPtr) > 0) { 
          RError("Invalid register index specified.");
        } else {
          ndata.second = idx;
        }
      } else if ((b == string::npos && e != string::npos) ||
                 (b != string::npos && e == string::npos)) {
        RError("Invalid register index specified.");
      }  
      return ndata;
    }

    typedef bool (IssNode::*Setter)(const std::string &,unsigned,const RNumber &);

    //---------------------------------------------------------------------------
    // pc:    Get the program counter
    // arg0:  optional:  Path of core.
    //---------------------------------------------------------------------------
    Handler(pc)
    {
      Self;

      auto trg = self->parsePathInternal(self->_root,(args.size() > 1) ? args[1] : string(),false);

      ostringstream ss;
      ss << "0x" << hex << trg.first.node()->getProgramCounter();
      return ss.str();
    }

    //---------------------------------------------------------------------------
    // cia:   Get the current instruction address.
    // arg0:  optional:  Path of core.
    //---------------------------------------------------------------------------
    Handler(cia)
    {
      Self;

      auto trg = self->parsePathInternal(self->_root,(args.size() > 1) ? args[1] : string(),false);

      ostringstream ss;
      ss << "0x" << hex << trg.first.node()->getCurrentInstrAddr();
      return ss.str();
    }

    //---------------------------------------------------------------------------
    // setpc: Set the program counter.
    // arg0:  optional:  Path of core.
    // arg1:  new program counter value 
    //---------------------------------------------------------------------------
    Handler(setpc)
    {
      Self;

      if (args.size() < 1) {
        UsageError(args[0],"Bad args.");
      }

      auto trg = self->parsePathInternal(self->_root,(args.size() > 2) ? args[1] : string(),false);

      const string &valueStr = (args.size() > 2) ? args[2] : args[1];
      addr_t newpc;
      if (!getAddr(newpc,valueStr)) {
        RError("setpc:  Invalid numeric format specified.");
      }
      trg.first.node()->setProgramCounter(newpc);

      return valueStr;
    }

    //---------------------------------------------------------------------------
    // setreg:  Set a register value.
    // arg0:  name.  Name may include an index, e.g. foo[1] and may contain a path.
    // arg1:  value or index
    // arg2:  optional:  value, if arg1 is index
    //---------------------------------------------------------------------------
    Handler(setreg)
    {
      Self;

      if (args.size() < 3) {
        UsageError(args[0],"Bad args.");
      }
      auto data = self->parsePathInternal(self->_root,args[1],true);
      return self->setRegInt(data.second,args,data.first,&IssNode::setReg);
    }

    //---------------------------------------------------------------------------
    // setregext:  Set a register value, using the external-write hook if present.
    // arg0:  name.  Name may include an index, e.g. foo[1] and may contain a path.
    // arg1:  value or index
    // arg2:  optional:  value, if arg1 is index
    //---------------------------------------------------------------------------
    Handler(setregext)
    {
      Self;

      if (args.size() < 3) {
        UsageError(args[0],"Bad args.");
      }
      auto data = self->parsePathInternal(self->_root,args[1],true);
      return self->setRegInt(data.second,args,data.first,&IssNode::setRegExt);
    }

    //---------------------------------------------------------------------------
    // writereg:  Write a register value.  This will trigger a write hook,
    //            if associated with a register.
    // arg0:  name.  Name may include an index, e.g. foo[1]
    // arg1:  value or index
    // arg2:  optional:  value, if arg1 is index
    //---------------------------------------------------------------------------
    Handler(writereg)
    {
      Self;

      if (args.size() < 3) {
        UsageError(args[0],"Bad args.");
      }
      auto data = self->parsePathInternal(self->_root,args[1],true);
      return self->setRegInt(data.second,args,data.first,&IssNode::writeReg);
    }

    string setRegInt(const string &name,const Args& args, const Target &target, Setter setter)
    {
      unsigned index = (args.size() == 3) ? 0 : strtoul(args[2].c_str(),0,0);

      // Check for an index on the name.
      auto ndata = parseRegName(name,index);
    
      const string &valueStr = (args.size() == 3) ? args[2] : args[3];

      // Check the value because the model seems to take gibberish without
      // complaining.
      if (!is_number(valueStr)) {
        RError("setReg:  Invalid numeric format specified.");
      }

      RNumber value(valueStr);

      // Now set the register
      CtxState restore(target);
      if (!(target.node()->*setter)(ndata.first,ndata.second,value)) {
        // let's try it caps
        string upperName = uppercase(ndata.first);

        if (!(target.node()->*setter)(upperName,ndata.second,value)) {
          RError("setReg:  Invalid register name '" << upperName << "'.");
        }
      }
      return "Register updated on node: " + target.node()->name(); 

    }

    //---------------------------------------------------------------------------
    // regs:  Display all registers in the machine.
    // arg0:   Optional: path of node to display.
    //---------------------------------------------------------------------------
    Handler(regs)
    {
      Self;

      RegReporter reporter;

      unsigned arg = 1;
      string path = getArg(args,arg);

      auto data = self->parsePathInternal(self->_root,path,false);
      CtxState restore(data.first);
      data.first.node()->showRegs(reporter);
      return reporter.str();
    }

    //---------------------------------------------------------------------------
    // reg:  Get a register value.
    // arg0:  name.  Name may include an index, e.g. foo[1]
    // arg1:  value or index
    //---------------------------------------------------------------------------
    Handler(reg)
    {
      Self;

      if (args.size() < 2) {
        UsageError(args[0],"Bad arguments.");
      }
      auto data = self->parsePathInternal(self->_root,args[1],true);
      return self->getRegInt(data.second,args,data.first,true);
    }

     //---------------------------------------------------------------------------
    // readreg:  Get a register value using the in-model access method.  This will
    //           call a read-hook, if present.
    // arg0:  name.  Name may include an index, e.g. foo[1]
    // arg1:  value or index
    //---------------------------------------------------------------------------
    Handler(readreg)
    {
      Self;

      if (args.size() < 2) {
        UsageError(args[0],"Bad arguments.");
      }
      auto data = self->parsePathInternal(self->_root,args[1],true);
      return self->getRegInt(data.second,args,data.first,false);
    }

    // Unfortunately, I can't use a method pointer, as is done for setRegInt,
    // b/c the signatures of getReg and readReg differ (getReg is const).
    string getRegInt(const string &name,const Args &args,const Target &target,bool use_get)
    {
      unsigned index = 0;
      string displayName (name);
      if (args.size() == 3) {
        index = strtoul(args[2].c_str(),0,0);
        displayName += args[2];
      }

      // Check for an index on the name.
      auto ndata = parseRegName(name,index);
        
      // Now set the register
      CtxState restore(target);
      RNumber value;
      if (use_get) {
        if (!target.node()->getReg(ndata.first,ndata.second,value)) {
          // let's try it caps
          string upperName = uppercase(ndata.first);
          
          if (!target.node()->getReg(upperName,ndata.second,value)) {
            RError("reg:  Invalid register name '" << upperName << "'.");
          }
        }
      } else {
        if (!target.node()->readReg(ndata.first,ndata.second,value)) {
          // let's try it caps
          string upperName = uppercase(ndata.first);
          
          if (!target.node()->readReg(upperName,ndata.second,value)) {
            RError("readreg:  Invalid register name '" << upperName << "'.");
          }
        }
      }
      return "0x" + value.str(RNumber::rhex);
    }

    //---------------------------------------------------------------------------
    // logreg:  Log a register's value.
    // arg0:    name.  Name may include an index, e.g. foo[1]
    // arg1:    optional index
    //---------------------------------------------------------------------------
    Handler(logreg)
    {
      Self;

      if (args.size() < 2) {
        UsageError(args[0],"Bad arguments.");
      }
      auto data = self->parsePathInternal(self->_root,args[1],true);
      return self->logRegInt(data.second,args,data.first);
    }

    string logRegInt(const string &name,const Args &args,const Target &target)
    {
      unsigned index = 0;
      if (args.size() == 3) {
        index = strtoul(args[2].c_str(),0,0);
      }

      // Check for an index on the name.
      auto ndata = parseRegName(name,index);
        
      // Now set the register
      CtxState restore(target);
      if (!target.node()->logReg(ndata.first,ndata.second)) {
        // let's try it caps
        string upperName = uppercase(ndata.first);
        
        if (!target.node()->logReg(upperName,ndata.second)) {
          RError("logreg:  Invalid register name '" << upperName << "'.");
        }
      }
      return "logreg OK";
    }

    //---------------------------------------------------------------------------
    // path:  Return the current hierarchy path.
    //---------------------------------------------------------------------------
    Handler(path)
    {
      Self;

      return self->_path;
    }

    enum PathType {ptNone=0,ptSys=1,ptCore=2,ptThread=4};

    // Walks the hierarchy in a depth-first manner, appending paths to the paths
    // argument.
    void walkHierarchy(Args &paths,PathType type,const string &parent_path,IssNode &node) 
    {
      string path = add_to_path(parent_path,node,PathDelim);

      if (node.is_core()) {
        if (type & ptCore) {
          paths.push_back(path);
        }
        // If any contexts exist with the "thread" attribute, then list those as
        // paths as well.
        auto ctx = getThreadContext(node);
        if (ctx && (type & ptThread)) { 
          ForRange(ctx->_num_contexts,i) {
            ostringstream ss;
            ss << ctx->_name << i;
            string tpath = add_to_path(path,ss.str(),PathDelim);
            paths.push_back(tpath);
          }
        }
      } else {
        if (type & ptSys) {
          paths.push_back(path);
        }
        ForEach(node.children(),iter) {
          walkHierarchy( paths, type, path, **iter );
        }
      }
    }

    //---------------------------------------------------------------------------
    // pathlist:  Return a list of all paths in the system.
    //---------------------------------------------------------------------------
    Handler(pathlist)
    {
      Self;

      Args paths;
      int type = ptNone;
      for (unsigned i = 1; i < args.size(); ++i) {
        if ( args[i] == "cores" ) {
          type |= ptCore;
        } else if (args[i] == "sys") {
          type |= ptSys;
        } else if (args[i] == "threads") {
          type |= ptThread;
        } else {
          RError("Unknown flag for pathlist:  " << args[i]);
        }
      }
      if (!type) {
        // Set all bits so that all flags are seen as set.
        type = -1;
      }
      self->walkHierarchy(paths,(PathType)type,string(),*self->_root);
      string result;
      ForEach(paths,p) {
        result += *p + ' ';
      }
      return result;
    }

    //---------------------------------------------------------------------------
    // setpath:  Set a new hierarchy path.
    // arg1:  The new path.
    //---------------------------------------------------------------------------
    Handler(setpath)
    {
      Self;

      if (args.size() < 2) {
        UsageError(args[0],"Bad arguments.");
      }
      auto data = self->parsePathInternal(self->_root,args[1]);
      self->_cur = data.first;
      self->_path = args[1];
      return "Path set to:  " + args[1];
    }

    struct TraceEventsMap : public map<string,TraceType>
    {
      TraceEventsMap()
      {
        static pair<string,TraceType> traceEventsInit[] = {
          pair<string,TraceType>("instr_prefetch",ttInstrPrefetch),
          pair<string,TraceType>("instr_read",ttInstrRead),
          pair<string,TraceType>("instr",ttInstr),
          pair<string,TraceType>("instr_args",ttInstrArgs),
          pair<string,TraceType>("instr_times",ttInstrTimes),
          pair<string,TraceType>("mmu_trans",ttMmuTranslation),
          pair<string,TraceType>("cache_action",ttCacheAction),
          pair<string,TraceType>("reg_write",ttRegWrite),
          pair<string,TraceType>("reg_read",ttRegRead),
          pair<string,TraceType>("regfile_write",ttRegfileWrite),
          pair<string,TraceType>("regfile_read",ttRegfileRead),
          pair<string,TraceType>("core_mem_write",ttCoreMemWrite),
          pair<string,TraceType>("core_mem_read",ttCoreMemRead),
          pair<string,TraceType>("mem_write",ttMemWrite),
          pair<string,TraceType>("mem_read",ttMemRead),
          pair<string,TraceType>("exception",ttException),
          pair<string,TraceType>("breakpoint",ttBreakpoint),
          pair<string,TraceType>("watchpoint",ttWatchpoint),
          pair<string,TraceType>("annotation",ttAnnotation),
          pair<string,TraceType>("branchtaken",ttBranchTaken),
          pair<string,TraceType>("basic_block_end",ttBasicBlockEnd),
          pair<string,TraceType>("",ttNone),
        };

        for (int i = 0; !traceEventsInit[i].first.empty(); ++i) {
          insert(traceEventsInit[i]);
        }
      };
    };

    static TraceEventsMap _traceEvents;

    string traceEventsHelp()
    {
      ostringstream ss;
      ss << "Legal trace events: ";
      ForEach(_traceEvents,i) {
        ss << ' ' << i->first;
      }
      ss << '.';
      return ss.str();
    }

    uint64_t getTraceEvents(const Args& args)
    {
      uint64_t r = 0;
      for (unsigned i = 1; i != args.size(); ++i) {
        auto iter = _traceEvents.find(args[i]);
        if (iter != _traceEvents.end()) {
          r |= iter->second;
        } else {
          RError("Unknown trace event '" << args[i] << "'.  " << traceEventsHelp());
        }
      }
      return r;
    }

    //---------------------------------------------------------------------------
    // setfilter:  Set new filtering flags.
    // arg1..n:  A tracing flag to enable.
    //---------------------------------------------------------------------------
    Handler(setfilter)
    {
      Self;

      if (args.size() < 2) {
        self->_root->set_tracing_r();
      } else {
        uint64_t flags = self->getTraceEvents(args);
        self->_root->set_tracing_r(flags);
      }
      return "setfilter OK";
    }

    //---------------------------------------------------------------------------
    // clrfilter:  Clear filtering flags.
    // arg1..n:  A tracing flag to disable.
    //---------------------------------------------------------------------------
    Handler(clrfilter)
    {
      Self;

      if (args.size() < 2) {
        self->_root->clear_tracing_r();
      } else {
        uint64_t flags = self->getTraceEvents(args);
        self->_root->clear_tracing_r(flags);
      }
      return "clrfilter OK";
    }

    //---------------------------------------------------------------------------
    // filterlist:  Return a list of all active filtering events.
    //---------------------------------------------------------------------------
    Handler(filterlist)
    {
      Self;

      unsigned flags = self->_cur.node()->tracing_flags();
      
      string events;
      bool first = true;
      ForEach(self->_traceEvents,iter) {
        if (iter->second & flags) {
          if (!first) {
            events += " ";
          } else {
            first = false;
          }
          events += iter->first;
        }
      }

      return events;
    }
   
    //---------------------------------------------------------------------------
    // help:  Print ADL-specific commands and their usage information.
    //
    //   arg1:  Optional.  List information about a specific command.
    //---------------------------------------------------------------------------
    Handler(help)
    {
      Self;

      const CommandMap &theCmds = commands();
      auto iter = theCmds.begin();
      int printed = 0;

      ostringstream ss;
      StrList col1,col2;
      while (iter != theCmds.end()) {
        if ( args.size() < 2 || ((*iter).first.find(args[1]) != string::npos) ) {
          col1.push_back((*iter).first);
          col2.push_back((*iter).second->doc);
          printed++;
        }
        iter++;
      }

      if (printed) {
        self->formatColumns(ss,col1,col2);
      } else {
        iter = theCmds.begin();
        ss << "No commands match '" << args[1] << "'.  Possibilities are: " << endl;
    
        while (iter != theCmds.end()) {
          if (printed == 6) {
            printed = 0;
            ss << '\n';
          }
          ss << "  " << (*iter).first;
          printed++;
          iter++;
        }
    
        if (printed) {
          ss << '\n';
        }
      }
      return ss.str();
    }

    void getPathAndCount(const Args &args,pair<Target,string> &data,uint32_t &count,const char *msg)
    {
      unsigned arg = 1;

      if (args.size() < 1) {
        UsageErrorM(args[0],"Bad arguments.");
      } else if (args.size() > 2) {
        data = parsePathInternal(_root,getArg(args,arg));
      } else {
        data.first = Target(_cur);
      }
      if (args.size() > 1) {
        if (!getUInt(count,args,arg)) {
          UsageErrorM(args[0],"Bad " << msg << " count.");
        }
      }      
    }

    //---------------------------------------------------------------------------
    // step:  Single-step, or step by N, the model (active path) or the path specified.
    //
    //   arg1: cycle count.
    //
    // or
    //
    //   arg1: path
    //   arg2: cycle count.
    //---------------------------------------------------------------------------
    Handler(step)
    {
      Self;
      pair<Target,string> data;
      uint32_t count = 1;      
      self->getPathAndCount(args,data,count,"step");

      string result = "Stepping on node: " + data.first.node()->name();
      self->_runctrl->stepn(count,*data.first.node());
      
      ostringstream ss;
      ss << "\n Current instruction address: 0x" << hex << data.first.node()->getCurrentInstrAddr();
      result += ss.str();
      
      return result;
    }

    void clearSliceBreakpoint (addr_t address,IAddrSpec::Action a) 
    {
      Actions::iterator iter = _actions.find(address);
      int s = 0;
      for (unsigned int i = 0; i < (*iter).second.size(); i++) {
        switch ((*iter).second[i]._action) {
        case IAddrSpec::eSliceOn: 
        case IAddrSpec::eSliceOff: 
          s++; 
          break;
        default: 
          break;
        }
      }
  
      // We only clear the breakpoint in the model if we are the last watcher of this 
      // address and we were using it for slicing.    
      if (s == 1) {
        // remove the breakpoint in the model
        stringstream o;
        o << address;
        clearBreak(o.str());
      }
    }

    // This function handles queued up events.  Returns true if the simulator
    // should return to the interactive loop.
    bool processActions(const tCmd *cmd,AddrSpecs &actions)
    {
      IssNode* activeNode = findActiveNode();
      uint64_t instr_count = _root->instr_count() + 1;
      bool done = false;
      ForEach(actions,iter) {
        const IAddrSpec& curr = (*iter);
        
        switch (curr._action) {
        case IAddrSpec::eBreak:
          if (activeNode) {
            cout << "Breakpoint hit on node: " << activeNode->name() << "  at address: 0x";
            cout.fill('0');
            cout.width(16);
            cout << hex << (*iter)._address << " count: "<<dec<<curr._count << "  InstrCount: "<<dec<< instr_count << endl;
          }
          done = true;
          break;
        case IAddrSpec::eSliceOn:
          {
            cout << "Slicing has begun at address: 0x" << 
              hex << curr._address << 
              dec << "  count: " << curr._count << "  InstrCount: "<<  instr_count + 1
                 <<"  to file: " << curr._args << 
              endl;              
            traceon(this,cmd,curr._args); 
          } break;
        case IAddrSpec::eSliceOff: 
          {
            cout << "Slicing has ended at address: 0x" << 
              hex << curr._address << 
              dec << "  count: " << curr._count << "  InstrCount: "<<  instr_count + 1
                 << endl;              
            traceoff(this,cmd,curr._args); 
          }
          break;
        case IAddrSpec::eAnnotate:         
          if (num_writers() == 0) {
            string r = formatAnnotation(curr._args[0]);
            if (VerboseMode) {
              cout << "NOTE: Annotation: " << r << endl;
            }
            activeNode->logger().log_annotation(mInfo,0,r);
          } else if (VerboseMode) {
            cout << "NOTE: annotation point hit, but slicing is disabled." << endl;
          }
          break;
        default:
          RError("Unsupported breakpoint action: " << curr.translateAction());
        }
      }
      return done;
    }

    //---------------------------------------------------------------------------
    // go:  Continue with the simulation.
    //
    //      This function does write to cout in order to display actions as they
    //      occur.  Since it's unlikely that this routine would be used by a
    //      remote interface, this is probably OK, at least for now.
    //      
    //---------------------------------------------------------------------------
    Handler(go)
    {
      Self;

      bool done = false;
    
      while (!done) {
      
        self->_runctrl->run(*self->_root,self->_sim_mode);
        if (!self->_pendingActions.empty() > 0) {
          done = self->processActions(cmd,self->_pendingActions);
          self->_pendingActions.clear();
          if (VerboseMode) {
            cout << scripter() << endl;
          }
        } else {
          cout << "Simulation halted at address: 0x";
          cout.fill('0');
          cout.width(16);
          cout << hex << self->_root->getProgramCounter() << endl;
          done = true;
        }
      }
  
      return string();
    }

    //---------------------------------------------------------------------------
    // ijam:  Directly execute the specified words
    //
    //
    //   arg1: path (optional- treated as a path if not parseable as an integer).
    //   argn: instruction word
    //
    //---------------------------------------------------------------------------
    Handler(ijam)
    {
      Self;

      if (args.size() < 2) {
        UsageError(args[0],"Bad arguments.");
      }

      unsigned argc = args.size()-1;
      string path;
      unsigned arg = 1;
      unsigned dummy;
      if (!getUInt(dummy,args[arg])) {
        // Not an integer, so treat as a path.
        path = args[arg++];
        argc -= 1;
      }

      Target trg;
      if (path.empty()) {
        trg = Target(self->_cur);
      } else {
        trg = self->parsePathInternal(self->_root,path).first;
      }

      uint32_t instrbuf[argc];      
      ForRange(argc,i)  {
        if (!getUInt(instrbuf[i],args,arg)) {
          UsageError(args[0],"Invalid instruction word argument.");
        }
      }

      trg.node()->exec_from_buffer(instrbuf,argc);

      return string();
    }

    //---------------------------------------------------------------------------
    // active:  Query an object to get its active count.  If this is a core, it will
    //          be a boolean.  For a system, it's a sum of the active counts.
    //
    //    arg1: Optional:  path
    //      
    //---------------------------------------------------------------------------
    Handler(active)
    {
      Self;

      pair<Target,string> data;

      if (args.size() < 2) {
        UsageError(args[0],"Bad arguments.");
      } else if (args.size() > 2) {
        data = self->parsePathInternal(self->_root,args[1]);
      } else {
        data.first = Target(self->_cur);
      }
      
      unsigned acount = data.first.node()->get_active_count();
      
      ostringstream result;
      result << acount;
      return result.str();
    }
    
    //---------------------------------------------------------------------------
    // setactive:  Specify an active count for the current or specified path.
    //             This lets the user halt or re-active a core.
    //             Returns the prior value.
    //
    //    arg1: value
    //    
    // or
    //
    //    arg1: path
    //    arg2: value
    //---------------------------------------------------------------------------
    Handler(setactive)
    {
      Self;

      unsigned arg = 1;
      pair<Target,string> data;
      uint32_t count = 0;      
      if (args.size() < 2) {
        UsageError(args[0],"Bad arguments.");
      } else if (args.size() > 2) {
        data = self->parsePathInternal(self->_root,getArg(args,arg));
      } else {
        data.first = Target(self->_cur);
      }
      if (!getUInt(count,args,arg)) {
        UsageError(args[0],"Bad active-count.");
      }      

      unsigned prior_count = data.first.node()->set_active_count(count);
      
      ostringstream result;
      result << prior_count;
      return result.str();
    }

    //---------------------------------------------------------------------------
    // sliceon:  Turn slicing on.
    //
    //   arg1:  symbol or address
    //   arg2:  filename
    //
    // or:
    //
    //   arg1:  symbol or address
    //   arg2:  count
    //   arg3:  filename
    //---------------------------------------------------------------------------
    Handler(sliceon)
    {
      Self;

      if (args.size() < 3) {
        UsageError(args[0],"Too few arguments");
      }

      unsigned arg = 1;
      string target = getArg(args,arg);

      uint32_t sliceOnCount = 1;
      string sliceFilename;
      if (args.size() == 3) {
        sliceFilename = getArg(args,arg);
      } else {
        if (!getUInt(sliceOnCount,args,arg)) {
          UsageError(args[0],"Bad slice-count.");
        }
        sliceFilename = getArg(args,arg);
      }
    
      if (sliceFilename.empty()) {
        UsageError(args[0],"Invalid filename.");
      }

      uint64_t address = self->lookup_symbol(target);

      // First we will try to add the action.
      self->addAction(address, IAddrSpec::eSliceOn, sliceOnCount, args[0], sliceFilename);

      self->setBreak(target);

      ostringstream ss;
      // We won't get here if the slice point was invalid, so our state will remain consistent.
      ss << "Slicing will begin at address: 0x" << 
        hex << address << 
        dec << "  count: " << sliceOnCount << 
        "  to file: " << sliceFilename;
      return ss.str();
    }

    //---------------------------------------------------------------------------
    // sliceoff:  Turn slicing off.
    //
    //   arg1:  symbol or address
    //   arg2:  Optional:  count
    //   arg3:  Optional:  filename
    //---------------------------------------------------------------------------
    Handler(sliceoff)
    {
      Self;

      if (args.size() < 2) {
        UsageError(args[0],"Too few arguments");
      }

      unsigned arg = 1;
      string target = getArg(args,arg);

      uint32_t sliceOffCount = 1;
      if (args.size() > 2) {
        if (!getUInt(sliceOffCount,args,arg)) {
          UsageError(args[0],"Bad slice-count.");
        }
      }

      string sliceFilename = getArg(args,arg);
    
      uint64_t address = self->lookup_symbol(target);

      // first we will try to add the action
      self->addAction(address, IAddrSpec::eSliceOff, sliceOffCount, args[0], sliceFilename);
    
      self->setBreak(target);

      ostringstream ss;
      // we won't get here if the slice point was invalid, so our state will remain consistent
      ss <<"Slicing will end at address: 0x" << 
        hex << address << 
        dec << "  count: " << sliceOffCount;

      return ss.str();
    }

    //---------------------------------------------------------------------------
    // annotate:  Insert a message into the output trace when an address is reached.
    //---------------------------------------------------------------------------
    Handler(annotate)
    {
      Self;

      if (args.size() < 3) {
        UsageError(args[0],"Too few arguments");
      }
  
      unsigned arg = 1;
      string target = getArg(args,arg);

      // if we got here, the addr or symbol was OK, now check the 2nd arg to the command
      string msg = getArg(args,arg);
      string::size_type q1 = msg.find("\"");
      string::size_type q2 = msg.rfind("\"");
    
      if (msg.empty() || q1 == string::npos || q2 == string::npos || q1+1 == q2) {
        UsageError(args[0],"Invalid message specified.");
      }
    
      msg = msg.substr(q1+1,q2-q1-1);    

      // Sanity check the format string.  An exception will be thrown if there is
      // a problem.
      self->formatAnnotation(msg);
    
      // Now we will try to add the action.
      uint64_t address = self->lookup_symbol(target);
      self->addAction(address, IAddrSpec::eAnnotate, 0, args[0], msg);

      self->setBreak(target);

      // We won't get here if the slice point was invalid, so our state will
      // remain consistent.
      ostringstream ss;
      ss << "Annotations will occur at address: 0x" << 
        hex << address << "  with msg: " << msg;

      return ss.str();
    }

    //---------------------------------------------------------------------------
    // tracemsg: Send a message to the logger of the current, or a specific, core.
    //---------------------------------------------------------------------------
    Handler(tracemsg)
    {
      Self;

      if (args.size() < 3) {
        UsageError(args[0],"Too few arguments");
      }

      pair<Target,string> data;
      unsigned arg = 1, level = 0;
      if (args.size() > 3) {
        data = self->parsePathInternal(self->_root,getArg(args,arg));
      } else {
        data.first = Target(self->_cur);
      }
      if (!getUInt(level,args,arg)) {
        RError("Expected argument " << arg << " to be an integer.");
      }

      // Only do this if annotation tracing is on for this core.    
      if (data.first.node()->tracing_on(ttAnnotation)) {
        data.second = getArg(args,arg);

        // Format the annotation string.
        data.second = self->formatAnnotation(data.second);
      
        data.first.node()->logger().log_annotation(mInfo,level,data.second);
      
        return "1";
      } else {
        return "0";
      }
    }

    // Gets a register width from introspection data.
    unsigned getRegWidth(const IssNode *node,unsigned id)
    {
      const RegInfos &ri = node->getRegInfo();

      ForEach(ri,i) {
        if (i->_id == id) {
          return i->_width;
        }
      }
      return 0;
    }

    // Sets up a debugger register mapping element and adds it to the
    // specified map.
    void setDbgRegData(DbgRegInfos &dri,const IssNode *node,const string &name,int width)
    {
      int id,index;

      if (!node->getRegIndex(id,index,name)) {
        RError("Unknown register:  " << name);
      }
      
      // No user specified width, so get it from the introspection data.
      if (width < 0) {
        width = getRegWidth(node,id);
      }

      dri.push_back(DbgRegInfo(id,index,width));
    }

    //---------------------------------------------------------------------------
    // setregmap:  Set a register mapping for use by the debugger interface.
    //---------------------------------------------------------------------------
    Handler(setregmap)
    {
      Self;

      if (args.size() < 2) {
        UsageError(args[0],"No path specified.");
      }

      Target trg;
      if (args[1] == "root") {
        trg = self->_root;
      } else {
        // First, get the path for the item whose register mapping we're setting.
        trg = parsePath(self->_root,args[1]).first;
      }

      // Then recursively set cores, if given a system, else just set the system.
      self->setRegMapInt(trg.node(),args);
      
      return string();
    }

    void setRegMapInt(IssNode *n,const Args &args)
    {
      if (n->is_core()) {
        // Add a new entry for this node, or clear it if it already exists.
        DbgRegMap &drm = dbgRegMap();
        DbgRegInfos &dri = drm[n];
        dri.clear();

        // Now collect up register information.
        for (unsigned arg = 2; arg != args.size(); ++arg) {
          string item = args[arg];

          uint32_t width;

          // If the string contains a ':' then it's a register:width item.
          size_t c;
          if ( (c = item.find(':')) != string::npos) {
            string name = item.substr(0,c);
            string w = item.substr(c+1);
            if (!getUInt(width,w)) {
              RError("Bad debug-register-mapping element:  " << item << ".  Expected <register-name>:<width>.");
            }
            setDbgRegData(dri,n,name,width);
          } else if ( item.find_first_not_of("0123456789") != string::npos ) {
            // It's not just a number, so we treat it as a register name.
            setDbgRegData(dri,n,item,-1);
          } else {
            // It's just a number, so it's treated as a placeholder.
            if (!getUInt(width,item)) {
              RError("Bad debug-register mapping element- thought it was just a width, but it didn't parse as a number:  " << item);
            }
            dri.push_back(DbgRegInfo(-1,-1,width));
          }
        }
      } else {
        ForEach(n->children(),c_iter) {
          setRegMapInt(*c_iter,args);
        }
      }
    }

 
    //---------------------------------------------------------------------------
    // setregendianness:  Set register endianness for debugger communication.
    //---------------------------------------------------------------------------
    Handler(setregendianness)
    {
      Self;

      if (args.size() < 3) {
        UsageError(args[0],"No path specified or endianness value not specified.");
      }

      Target trg;
      if (args[1] == "root") {
        trg = self->_root;
      } else {
        // First, get the path for the item whose register mapping we're setting.
        trg = parsePath(self->_root,args[1]).first;
      }

      // Then recursively set cores, if given a system, else just set the system.
      self->setRegEndiannessInt(trg.node(),args);
      
      return string();
    }

    void setRegEndiannessInt(IssNode *n,const Args &args)
    {
      if (n->is_core()) {
        // Add a new entry for this node or get existing item.
        DbgRegMap &drm = dbgRegMap();
        DbgRegInfos &dri = drm[n];

        string e = args[2];
        if (e == "little") {
          dri.setBigEndian(false);
        } else if (e == "big") {
          dri.setBigEndian(true);
        } else {
          RError("Bad endianness value specified: " << e << ".  Expected 'big' or 'little'.");
        }
      } else {
        ForEach(n->children(),c_iter) {
          setRegMapInt(*c_iter,args);
        }
      }
    }


    
    //---------------------------------------------------------------------------
    // setlogcmd:  Add a logging callback function.
    //
    //   arg1:  Name of event with which to register the command.  May be a path to
    //          a specific hierarchy node.
    //   arg2:  Name of function
    //   arg3:  Filter name (optional)
    //   arg4:  Filter value 1 (optional).
    //   arg5:  Filter value 2 (optional).
    //---------------------------------------------------------------------------
    Handler(setlogcmd)
    {
      Self;

      if (args.size() < 3) {
        UsageError(args[0],"Too few arguments");
      }

      unsigned arg = 1;
      string event  = getArg(args,arg);
      string func   = getArg(args,arg);
      string filter = getArg(args,arg);


      unsigned fcount = 0;
      string f1 = getArg(args,arg);
      string f2 = getArg(args,arg);
      if (!f1.empty()) ++fcount;
      if (!f2.empty()) ++fcount;

      uint64_t filter1 = 0, filter2 = 0;
    
      if (!f1.empty()) {
        if (!getAddr(filter1,f1)) {
          UsageError(args[0],"Invalid first filter value (expected a numeric value).");
        }
      }

      if (!f2.empty()) {
        if (!getAddr(filter2,f2)) {
          UsageError(args[0],"Invalid second filter value (expected a numeric value).");
        }
      }

      // Note that we're *not* calling parse-path-internal here because we don't
      // want an implicit context to be used.  If the user doesn't explicitly
      // specify a context, either via a path in the event specifier or by
      // calling setpath, then this event is tied to the core's activity and
      // will fire on any context.
      auto data = parsePath(self->_root,event,self->_cur,true);
      self->_script_logger->add_event(data.first,data.second,func,filter,fcount,filter1,filter2);

      return "1";
    }

    //---------------------------------------------------------------------------
    // clrlogcmd:  Remove a logging callback function.
    //
    //   arg1:  Name of event with which to remove.
    //   arg2:  Name of function to remove.
    //---------------------------------------------------------------------------
    Handler(clrlogcmd)
    {
      Self;

      if (args.size() < 3) {
        UsageError(args[0],"Too few arguments");
      }

      unsigned arg = 1;
      string event = getArg(args,arg);
      string func  = getArg(args,arg);

      self->_script_logger->del_event(self->_cur,event,func);

      return "1";
    }

    //---------------------------------------------------------------------------
    // setbreakcmd:  Add a function to be called on a breakpoint.
    //
    //   arg1:  Address or symbol on which to break.
    //   arg2:  Name of function to call.
    //---------------------------------------------------------------------------
    Handler(setbreakcmd)
    {
      Self;

      if (args.size() < 3) {
        UsageError(args[0],"Too few arguments");
      }

      unsigned arg = 1;
      string target  = getArg(args,arg);
      string func   = getArg(args,arg);

      int handle = self->setBreakH(target);
      self->_script_logger->add_event(self->_cur,"break",func,handle);

      return "1";
    }

    //---------------------------------------------------------------------------
    // clrbreakcmd:  Remove a breakpoint function.
    //
    //   arg1:  Name of function to remove.
    //---------------------------------------------------------------------------
    Handler(clrbreakcmd)
    {
      Self;

      if (args.size() < 2) {
        UsageError(args[0],"Too few arguments");
      }

      unsigned arg = 1;
      string func  = getArg(args,arg);

      int handle = self->_script_logger->del_event(self->_cur,"break",func);

      if (self->_cur.node()->is_core()) {
        dynamic_cast<IssCore *>(self->_cur.node())->clearBreakpoint(handle);
      } else {
        RError("clrbreakcmd:  Bad node.  A breakpoint may only be cleared from a core.");
      }      

      return "1";
    }

    //---------------------------------------------------------------------------
    // setwatchcmd:  Add a function to be called on a watchpoint.
    //
    //   arg1:  Address or symbol on which to break.
    //   arg2:  Type of watchpoint:  read, write, or access (either read or write).
    //   arg3:  Name of function to call.
    //---------------------------------------------------------------------------
    Handler(setwatchcmd)
    {
      Self;

      if (args.size() < 4) {
        UsageError(args[0],"Too few arguments");
      }

      unsigned arg = 1;
      string target = getArg(args,arg);
      string type   = getArg(args,arg);
      string func   = getArg(args,arg);

      WatchType w;
      if (type == "read") {
        w = wpRead;
      }
      else if (type == "write") {
        w = wpWrite;
      }
      else if (type == "access") {
        w = wpAccess;
      }
      else {
        RError("Unknown watchpoint type '" << type << "'.  Expected 'read', 'write', or 'access'.");
      }

      int handle = self->setWatchH(target,w);
      self->_script_logger->add_event(self->_cur,"watch",func,handle);

      return "1";
    }

    //---------------------------------------------------------------------------
    // clrwatchcmd:  Remove a watchpoint function.
    //
    //   arg1:  Name of function to remove.
    //---------------------------------------------------------------------------
    Handler(clrwatchcmd)
    {
      Self;

      if (args.size() < 2) {
        UsageError(args[0],"Too few arguments");
      }

      unsigned arg = 1;
      string func  = getArg(args,arg);

      int handle = self->_script_logger->del_event(self->_cur,"watch",func);

      if (self->_cur.node()->is_core()) {
        dynamic_cast<IssCore *>(self->_cur.node())->clearWatchpoint(handle);
      } else {
        RError("clrwatchcmd:  Bad node.  A watchpoint may only be cleared from a core.");
      }      

      return "1";
    }

    //---------------------------------------------------------------------------
    // setdynparm:  Set a dynamic model parameter.  Returns 1 on success.
    //
    //   arg1:  Parameter name
    //   arg2:  Parameter value (integer).
    //---------------------------------------------------------------------------
    Handler(setdynparm)
    {
      Self;

      if (args.size() != 3) {
        UsageError(args[0],"Wrong number of arguments.");
      }

      unsigned arg = 1;
      string parm    = getArg(args,arg);
    
      uint32_t value;
      if (!getUInt(value,args,arg)) {
        UsageError(args[0],"Invalid parameter value.");
      }

      self->_runctrl->set_dyn_parm(*self->_cur.node(),parm,value);

      return "1";
    }

    //---------------------------------------------------------------------------
    // getdynparm:  Get a dynamic model parameter.  Returns the parameter value 
    //              on success.
    //
    //   arg1:  Parameter name
    //---------------------------------------------------------------------------
    Handler(getdynparm)
    {
      Self;
      if (args.size() != 2) {
        UsageError(args[0],"Wrong number of arguments.");
      }

      unsigned arg = 1;
      string parm    = getArg(args,arg);

      ostringstream ss;
      ss << self->_runctrl->get_dyn_parm(*self->_cur.node(),parm);
      return ss.str();
    }

    //---------------------------------------------------------------------------
    // listdynparm:  List available model parameters.
    //---------------------------------------------------------------------------
    Handler(listdynparm)
    {
      Self;

      StrPairs parms;
      self->_runctrl->list_dyn_parm(*self->_cur.node(),parms);

      if (parms.empty()) {
        return string();
      }

      int printed = 0;

      ostringstream ss;
      StrList col1,col2;
      ForEach(parms,iter) {
        if ( args.size() < 2 || ((*iter).first.find(args[1]) != string::npos) ) {
          col1.push_back((*iter).first);
          col2.push_back((*iter).second);
          printed++;
        }
      }

      if (printed) {
        self->formatColumns(ss,col1,col2);
      } else {
        ss << "No commands match '" << args[1] << "'.  Possibilities are: " << endl;

        ForEach(parms,iter) {
          if (printed == 6) {
            printed = 0;
            ss << '\n';
          }
          ss << "  " << (*iter).first;
          printed++;
          iter++;
        }
    
        if (printed) {
          ss << '\n';
        }
      }
      return ss.str();
    }

    //---------------------------------------------------------------------------
    // settd:  Set various temporal decoupling parameters.
    //
    //  arg1:  Parameter name (with optional path specifier)
    //  arg2:  Value
    //---------------------------------------------------------------------------
    Handler(settd)
    {
      Self;
      if (args.size() < 3) {
        UsageError(args[0],"Wrong number of arguments.");
      }

      unsigned arg = 1;
      string path = getArg(args,arg);
      auto data = self->parsePathInternal(self->_root,path,true);

      uint64_t val;
      if (!getAddr(val,args,arg)) {
        RError("Expected argument " << arg <<" to be an integer.");
      }
      
      const string &parm = data.second;
      
      uint64_t r = 0;
      if (parm == "threshold") {
        r = data.first.node()->set_td_threshold(val);
      }
      else if (parm == "count") {
        r = data.first.node()->set_td_counter(val);
      }
      else if (parm == "incr") {
        r = data.first.node()->set_td_increment(val);
      }
      else {
        RError(args[0] << ":  Unknown parameter specified:  " << parm);
      }

      ostringstream ss;
      ss << r;
      return ss.str();
    }

    //---------------------------------------------------------------------------
    // gettd:  Get various temporal decoupling parameters.
    //
    //  arg1:  Parameter name (with optional path specifier)
    //---------------------------------------------------------------------------
    Handler(gettd)
    {
      Self;
      if (args.size() < 2) {
        UsageError(args[0],"Wrong number of arguments.");
      }

      unsigned arg = 1;
      string path = getArg(args,arg);
      auto data = self->parsePathInternal(self->_root,path,true);
      
      const string &parm = data.second;
      
      uint64_t r = 0;
      if (parm == "threshold") {
        r = data.first.node()->get_td_threshold();
      }
      else if (parm == "count") {
        r = data.first.node()->get_td_counter();
      }
      else if (parm == "incr") {
        r = data.first.node()->get_td_increment();
      }
      else {
        RError(args[0] << ":  Unknown parameter specified:  " << parm);
      }

      ostringstream ss;
      ss << r;
      return ss.str();
    }
    
    //---------------------------------------------------------------------------
    // simstat:  Get information about a particular simulation statistic.  Returns the
    //           statistic value, or an error if it does not exist.
    //
    //   arg1:  Parameter name
    //---------------------------------------------------------------------------
    Handler(simstat)
    {
      Self;
      if (args.size() != 2) {
        UsageError(args[0],"Wrong number of arguments.");
      }

      unsigned arg = 1;
      string parm    = getArg(args,arg);

      ostringstream ss;

      ss << self->_cur.node()->get_sim_stat(parm);
      return ss.str();
    }

    //---------------------------------------------------------------------------
    // simreset:  Reset the entire simulation.
    //---------------------------------------------------------------------------
    Handler(simreset)
    {
      Self;

      memoryLog().clear();
      resetSimulation(*self->_root,self->_runctrl);
      self->_script_logger->reset();
      return "1";
    }

    //---------------------------------------------------------------------------
    // modelreset:  Reset the models but not memory.
    //---------------------------------------------------------------------------
    Handler(modelreset)
    {
      Self;

      self->_runctrl->reset(*self->_root);
      self->_script_logger->reset();
      return "1";
    }

    //---------------------------------------------------------------------------
    // quit:  Halt the simulation.  Same as exit- this is just provided for 
    //        compatibility.
    //---------------------------------------------------------------------------
    Handler(quit)
    {
      Self;

      cout << "\nInstructions executed: "<<dec<< self->_runctrl->instr_count(*self->_root)<<endl;
      quitTcl();
      return string();
    }

    //---------------------------------------------------------------------------
    // simmode:  Specify the type of simulator to use.  Returns the current type
    //           of simulator being used.  If no argument is supplied, just returns 
    //           the current status.
    //
    //           Argument:  normal, jit
    //---------------------------------------------------------------------------
    Handler(simmode)
    {
      Self;

      if (args.size() > 1) {
        unsigned arg = 1;
        string a = getArg(args,arg);
        if (a == "jit") {
          self->_sim_mode = SimJit;
        } else if (a == "dc") {
          self->_sim_mode = SimDc;
        } else {
          self->_sim_mode = SimNormal;
        }
      }
      switch (self->_sim_mode) {
      case SimJit:
        return "jit";
      case SimDc:
        return "dc";
      default:
        return "normal";
      }
    }

    void print(ostream &target) const
    {
      target << "--- BEGIN Scripting ---" << endl;

      auto actionIter = getActions().begin();
      auto end = getActions().end();

      target << " >> Action Specifications <<" << endl;    
      if (actionIter == end) {
        target << "EMPTY" << endl;
      } else while (actionIter != end) {
        target << "  ** Address: 0x" << hex << (*actionIter).first << " ";
        const vector<IAddrSpec>& specs = (*actionIter).second;
        if (specs.size() == 0) {
          target << "EMPTY" << endl;
        } else {
          target << endl;
          for (unsigned int i = 0; i != specs.size(); ++i) {
            target << "    -> Action: " << specs[i].translateAction() << 
              "  count: " << dec << specs[i]._count << "  args: \"" << specs[i]._args << "\"" << endl;
          }
        }
        actionIter++;
      }
  
      const AddrSpecs &pending = getPendingActions();
      target << " >> Pending Actions <<" << endl;
      if (pending.size() == 0) {
        target << "EMPTY" << endl;
      } else {
        for (unsigned int i = 0; i != pending.size(); ++i) {
          target << "    -> Action: " << pending[i].translateAction() << 
            "  count: " << dec << pending[i]._count << "  args: \"" << 
            pending[i]._args << "\"" << endl;
        }
      }
  
      target << "--- END Scripting ---" << endl;
    }

    // This forms two columns of textual data.
    void formatColumns(ostream &o,const StrList &col1,const StrList &col2)
    {   
      size_t width1 = 0;
      ForEach(col1,i) {
        width1 = max(width1,i->size());
      }

      auto iter2 = col2.begin();
      ForEach(col1,iter1) {
        o << setw(width1) << *iter1;
        if (iter2 != col2.end()) {
          o << "    " << *iter2++;
        }
        o << "\n";
      }
    }

  };

  // The singleton instance of the scripting object.
  static Scripting theInstance;

  //-------------------------------------------------------------------//
  //-------  IAddrSpec Implementation  ------------------//
  //-------------------------------------------------------------------//

  string Scripting::IAddrSpec::translateAction(void) const
  {
    string retVal = "UNKNOWN";

    switch (_action) {
    case eBreak:
      retVal = "break  ";
      break;
    case eSliceOn:
      retVal = "sliceon";
      break;
    case eSliceOff: 
      retVal = "sliceoff";
      break;
    case eAnnotate:
      retVal = "annotate";
      break;
    default:
      retVal = "ERROR   ";
    }

    return retVal;
  }

  //-------------------------------------------------------------------//
  //-----------------  CliLogger Implementation  ----------------------//
  //-------------------------------------------------------------------//
  void Scripting::CliLogger::logException(unsigned,bool pre,const char *name)
  {
    if (pre) {
      if (VerboseMode) {
        cout << "NOTE: Exception triggered: " << name << endl;
      }
    }
  }

  void Scripting::CliLogger::logBreakpoint(addr_t address, unsigned,PtBase *)
  {
    auto entry = _cli._actions.find(address);
    vector<IAddrSpec>& specs = (*entry).second;
    bool erase = false;

    bool sliceOnHit = false;
    bool sliceOffHit = false;
    bool bkptHit = false;

    // If found, process it.  If not found, then we assume it's a breakpoint set
    // by somebody else, so we ignore it.
    if (entry != _cli._actions.end()) {

      uint64_t instr_count = theInstance._root->instr_count() + 1;

      // Add to the pending actions vector if the counts match.  Note: We only
      // return to the interpreter if we have an actual breakpoint.  Slice and
      // annotation operations do not stop.
      ForEach(specs,iter) {
        if (erase) { iter--; end--; erase = false;}
        switch ((*iter)._action) {
        case IAddrSpec::eBreak:
          // breakpoints may or may not have counts associated with them.  If not, we break
          // everytime, if so we only break when the counter is >= the count.
          if ((*iter)._count) {
            bkptHit =  (++(*iter)._counter >= (*iter)._count) ? true : false;
          } else {
            bkptHit = true;
          }
          if (bkptHit) {
            _cli._pendingActions.push_back((*iter));
          }
          if (VerboseMode) {
            cout << "NOTE: Break point hit, counter=" << (*iter)._counter << 
              " count=" << (*iter)._count << "  InstrCount=" <<  instr_count << endl;
          }
          break;
        case IAddrSpec::eSliceOn:
          sliceOnHit = (++(*iter)._counter == (*iter)._count) ? true : false;
          if (sliceOnHit) {
            _cli._pendingActions.push_back((*iter));
            specs.erase(iter);
            erase = true;
          }
          if (VerboseMode) {
            cout << "NOTE: Slice point hit, counter=" << (*iter)._counter << 
              " count=" << (*iter)._count << "  InstrCount=" <<  instr_count << endl;
          }
          break;
        case IAddrSpec::eSliceOff:
          sliceOffHit = (++(*iter)._counter == (*iter)._count) ? true : false;
          if (sliceOffHit) {
            _cli._pendingActions.push_back((*iter));
            specs.erase(iter);
            erase = true;
          }
          if (VerboseMode) {
            cout << "NOTE: Slice off point hit, counter=" << (*iter)._counter << 
              " count=" << (*iter)._count << "  InstrCount=" <<  instr_count << endl;
          } 
          break;
        case IAddrSpec::eAnnotate:
          _cli._pendingActions.push_back((*iter));
          if (VerboseMode) {
            cout << "NOTE: Annotate point hit at" << (*iter)._address << endl;
          }
          break;
        default:
          cerr << "ERROR: Unsupported break action encountered!" << endl;
        }
      }
      if (VerboseMode) {
        cout << scripter() << endl;
      }
    }

    // Stop the simulation if we have actions which should stop it.
    if (!_cli._pendingActions.empty()) {
      throw SimInterrupt();
    } 
  }

  ScriptingIface &scripter()
  {
    return theInstance;
  }

  void Tcl_Func_Init(Tcl_Interp *interp,const vector<string> *args);

  // Local global so that the interactive system and the scripting system can
  // communicate via the same interpreter.
  static Tcl_Interp *tcl_interp = 0;

  extern "C" void adl_tcl_exit_handler(ClientData data)
  {
    // Guard against multiple calls of the shutdown command.
    if (theInstance.active()) {
      theInstance.shutdown();    
      finalize();
      theInstance.deleteRoot();
      clearLoggers();
    }
  }

  // This is just lifted from TCL, since this function is only present in tclsh
  // and is not exported in any of its libraries.
  int Tcl_AppInit(Tcl_Interp *interp)
  {
    assert(!tcl_interp);
    tcl_interp = interp;

    if (Tcl_Init(interp) == TCL_ERROR) {
      return TCL_ERROR;
    }

#ifdef TCL_TEST
#ifdef TCL_XT_TEST
    if (Tclxttest_Init(interp) == TCL_ERROR) {
      return TCL_ERROR;
    }
#endif
    if (Tcltest_Init(interp) == TCL_ERROR) {
      return TCL_ERROR;
    }
    Tcl_StaticPackage(interp, "Tcltest", Tcltest_Init,
                      (Tcl_PackageInitProc *) NULL);
    if (TclObjTest_Init(interp) == TCL_ERROR) {
      return TCL_ERROR;
    }
    if (Procbodytest_Init(interp) == TCL_ERROR) {
      return TCL_ERROR;
    }
    Tcl_StaticPackage(interp, "procbodytest", Procbodytest_Init,
                      Procbodytest_SafeInit);
#endif /* TCL_TEST */

    // Install all ADL-specific functions.  Arguments already handled via Tcl_Main.
    Tcl_Func_Init(interp,0);

#ifdef DJGPP
    Tcl_SetVar(interp, "tcl_rcFileName", "~/tclsh.rc", TCL_GLOBAL_ONLY);
#else
    Tcl_SetVar(interp, "tcl_rcFileName", "~/.tclshrc", TCL_GLOBAL_ONLY);
#endif

    return TCL_OK;
  }

  // This starts an interactive TCL interpreter.  It does not return.
  // Upon exit, the finalize() function is invoked by an exit handler.
  void interactiveTcl(const vector<string> *args)
  {
    static const char *buf[] = { "-" };
    Tcl_CreateExitHandler(adl_tcl_exit_handler,0);
    int argc = 1;
    const char **argv = buf;
    if (args) {
      argc = args->size()+1;
      argv = new const char *[argc];
      int arg = 0;
      argv[arg++] = buf[0];
      ForEach((*args),i) {
        argv[arg++] = i->c_str();
      }
    }
    Tcl_Main(argc,(char**)argv,Tcl_AppInit);
    if (args) {
      delete argv;
    }
  }

  void quitTcl()
  {
    Tcl_Exit(0);
  }

  //---------------------------------------------------------------------------
  // evalTcl - Loads TCL interpreter and evaluates the tcl script.
  //---------------------------------------------------------------------------
  bool evalTcl(string &result,const string &cmd,const vector<string> *args)
  {
    if ( tcl_interp == 0 ) {
      tcl_interp = Tcl_CreateInterp();
      Tcl_CreateExitHandler(adl_tcl_exit_handler,0);
      Tcl_Func_Init(tcl_interp,args);
    }
   
    DBPrint("Evaluating: " << line);
      
    int status = Tcl_Eval(tcl_interp,cmd.c_str());

    if (const char * res = Tcl_GetStringResult(tcl_interp)) {
      result = res;
    }

    return (status == TCL_OK);
  }

  // Standard TCL wrapper for our functions.  The client data holds the pointer to
  // the function to be executed.  Arguments are converted to an Args form, then
  // the routine is called.
  int standard_wrapper(ClientData clientData, Tcl_Interp *interp, int argc, const char *argv[])
  {
    auto cmd = (tCmd*) clientData;

    int rc = TCL_OK;
    string result;
    Args args;
    for (int arg = 0; arg < argc; ++arg) {
      args.push_back(argv[arg]);
    }

    // Invoke the function.
    try {
      result = (cmd->func)(cmd->obj,cmd,args);
    }
    catch (exception &err) {
      result = err.what();
      rc = TCL_ERROR;
    }

    // Set up the result if a result is to be returned.
    if (!result.empty()) {
      char *s = Tcl_Alloc(result.size()+1);
      strcpy(s,result.c_str());
      Tcl_SetResult(interp,s,TCL_DYNAMIC);
    }

    return rc;
  }

  // This registers all commands into the TCL interpreter as string-based
  // commands.  Yes, I know, this is considered lame by current TCL programming
  // standards: We should be using the object-based interface.  Well, there is a
  // reason that we're doing this: We're bolting this onto the existing
  // interaction-manager, which is also string based.  Eventually, we'll want to
  // make everything TCL based and convert to the object-oriented approach, but
  // this was the simplest to get working.
  void Tcl_Func_Init(Tcl_Interp *interp,const vector<string> *args)
  {
    const Scripting::CommandMap &commands = Scripting::commands();
   
    if (args) {
      // If we're given arguments, then insert them into the interpreter as an
      // argv list.  Code taken from tclMain.c
      Tcl_SetVar2Ex(interp, "argc", NULL, Tcl_NewIntObj(args->size()), TCL_GLOBAL_ONLY);

      Tcl_Obj *argvPtr = Tcl_NewListObj(0, NULL);
      ForEach((*args),arg) {
        Tcl_DString ds;
        Tcl_ExternalToUtfDString(NULL, arg->c_str(), -1, &ds);
        Tcl_ListObjAppendElement(NULL, argvPtr, Tcl_NewStringObj(Tcl_DStringValue(&ds), Tcl_DStringLength(&ds)));
        Tcl_DStringFree(&ds);
      }
      Tcl_SetVar2Ex(interp, "argv", NULL, argvPtr, TCL_GLOBAL_ONLY);
    }

    // Now insert all of our ADL commands into the interpreter.
    ForEach (commands,i) {
      const tCmd &cmd = *i->second;
      Tcl_CreateCommand(interp,cmd.name,standard_wrapper,(ClientData)&cmd,NULL);
    }
  } 

  // Call this to check to see if a config script should be run.  It then
  // executes it if there is one.
  void checkForCliConfig(IssNode &root,ScriptingControlIface &runctrl)
  {
    // Check to see if we have any TCL arguments.  If so, store them for use by the interpreter.
    if (options().haveOption("script-arg")) {
      vector<string> args;
      auto script_args = options().getValues("script-arg");
      ForEach(script_args,i) {
        args.push_back(*i);
      }
      theInstance.set_args(args);
    }
    if (options().getFlag("config")) {
      // If we have this option then execute the script, then proceed with
      // normal operation.
      theInstance.evalFile( options().getOption("config") );
    }
  }

  // If the appropriate command-line argument is set, return a pointer to the CLI
  // object, which is a singleton class.  This is called by ExecSupport when looking
  // for alternate main loops.
  DbgInterface *checkForCliMode(IssNode &root,ScriptingControlIface &runctrl)
  {
    if ((options().getFlag("cli")) || (options().getFlag("script")) ) {
      // If we have one of these options then we return the interface object
      // so that we don't proceed with normal operation.
      theInstance.scripting_initialize(root,runctrl);
      theInstance.set_script(options().getOption("script"));
      return &theInstance;
    } else {
      return 0;
    }
  }

  // Starts up the CLI mode directly.  Useful for shared-object invocation.
  DbgInterface *startCliMode(IssNode &root,ScriptingControlIface &runctrl,const string &script,
                             const vector<string> &args)
  {
    theInstance.scripting_initialize(root,runctrl);
    theInstance.set_script(script);
    theInstance.set_args(args);
    return &theInstance;
  }

  // Evaluates a command using the scripting interface.  Useful for special case handling
  // in shared objects.
  string evalCliCommand(const string &cmd)
  {
    string result;
    theInstance.eval(result,cmd);
    return result;
  }

  Scripting::TraceEventsMap Scripting::_traceEvents;

# define HandlerDecl(cmd,doc) { #cmd, 0, cmd, doc }

  //
  // Keep this at the bottom of the file for easy viewing!
  //
  // This data structure contains all commands which will be registered with TCL.
  //
  tCmd Scripting::_commands[] = {
    HandlerDecl(simload,        "Load a file.  Usage:  simload filename [format] [setpc (if set, file's entry-point is used)]"),
    HandlerDecl(mem,            "Display memory contents.  Usage:  mem start_address length [read-size-bytes (1,2,4,8)] [words-per-line (Default is 4)]"),
    HandlerDecl(memlist,        "Return a list of memory contents.  Usage:  memlist start_address length [read-size-bytes (1,2,4,8)] [radix (char,dec,hex)]"),
    HandlerDecl(setmem,         "Set memory contents.  Usage:  setmem address value [length in bytes (1-4)]"),
    HandlerDecl(pc,             "Retrieve the current program counter value.  Usage:  pc [path]"),
    HandlerDecl(cia,            "Retrieve the current instruction address.  Usage:  cia [path]"),
    HandlerDecl(setpc,          "Set the program counter to a specified value.  Usage:  setpc [path] value"),
    HandlerDecl(reg,            "Show a specific register.  Usage:  reg [path:]reg_name [index]" ),
    HandlerDecl(logreg,         "Send the specified register's value to the logging system.  Usage:  logreg [path:]reg_name [index]" ),
    HandlerDecl(readreg,        "Read a specific register using its in-model read method.  Usage:  readreg [path:]reg_name [index]"),
    HandlerDecl(regs,           "Show all registers.  Usage:  regs [path]" ),
    HandlerDecl(setreg,         "Set a register.  Usage:  setreg [path:]reg_name [index] value" ),
    HandlerDecl(setregext,      "Set a register, executing an external-write hook if present.  Usage:  setregext [path:]reg_name [index] value" ),
    HandlerDecl(writereg,       "Write a register.  This will trigger a write hook, if present.  Usage:  writereg [path:]reg_name [index] value" ),
    HandlerDecl(path,           "Display the current modification node path.  Usage:  path" ),
    HandlerDecl(setpath,        "Set a new modification node path.  Usage:  setpath path" ),
    HandlerDecl(pathlist,       "Return a list of all paths in the system.  Usage:  pathlist [sys|cores|threads]" ),
    HandlerDecl(step,           "Step instruction(s).  Usage:  step [path] [count]" ),
    HandlerDecl(go,             "Run until halt or break.  Usage:  go" ),
    HandlerDecl(ijam,           "Directly execute the specified words.  Usage:  ijam [path] <word> [word...]"),
    HandlerDecl(active,         "Return the active count for the current or specified path.  Usage:  active [path]" ),
    HandlerDecl(setactive,      "Set the active count for the current or specified path.  Returns the prior value.  Usage:  setactive [path] value" ),
    HandlerDecl(setbrk,         "Set a breakpoint.  Usage:  setbrk {address | symbol} [count]" ),
    HandlerDecl(clrbrk,         "Clear a breakpoint.  Usage:  clrbrk {address | symbol}" ),
    HandlerDecl(setfilter,      "Turn on specified tracing/slicing events.  Usage:  setfilter event [events...]" ),
    HandlerDecl(clrfilter,      "Turn off specified tracingslicing events.   Usage:  clrfilter event [events...]" ),
    HandlerDecl(filterlist,     "Return a list of all current tracing/slicing events.  usage:  filterlist"),
    HandlerDecl(traceon,        "Turn on a trace immediately.  Usage:  traceon filename"),
    HandlerDecl(traceoff,       "Turn off a trace or all traces immediately.  Usage:  traceoff [filename]" ),
    HandlerDecl(tracemsg,       "Send an annotation message to the logging system.  Usage:  tracemsg [path] <level> <msg>" ),
    HandlerDecl(straceon,       "Turn on a stream trace immediately.  Usage:  traceon <writer type> [stdout|stderr]"),
    HandlerDecl(straceget,      "Return the current stream-trace contents and reset the stream.  Usage:  straceget"),
    HandlerDecl(straceoff,      "Turn off a stream trace.  Usage:  streamtraceoff" ),
    HandlerDecl(tracelist,      "List all files being traced.  Usage:  tracelist"),
    HandlerDecl(inputlist,      "List all files that have been read for this simulation.  Usage:  inputlist"),
    HandlerDecl(sliceon,        "Turn on a trace at a specific location.  Usage:  sliceon {address | symbol} [count] filename"),
    HandlerDecl(sliceoff,       "Turn off a trace or all traces at a specific location.  Usage:  sliceoff {address | symbol} [count] [filename]" ),
    HandlerDecl(annotate,       "Annotate the slice file.  Usage:  annotate {address | symbol} \"msg\""),
    HandlerDecl(triggerexcpt,   "Trigger exception.  Usage:  triggerexcpt [path:]exception-name" ),
    HandlerDecl(cancelexcpt,    "Cancel a pending exception.  Usage:  cancelexcpt [path:]exception-name" ),
    HandlerDecl(excptlist,      "Return a list of all valid exceptions.  Usage:  excptlist [path]"),
    HandlerDecl(icount,         "Display the instruction count for the current or specified node.  Usage:  icount [path]" ),
    HandlerDecl(symbol,         "Return the address of a symbol from the symbol table (if one exists).  Usage:  symbol [symbol]" ),
    HandlerDecl(setregmap,      "Set a debugger register mapping.  Usage:  setregmap path <reg> ... where <reg> is reg-name, reg-name:width, or width (for a placeholder)."),
    HandlerDecl(setregendianness,"Set the endianness to use when communicating register values with a debugger.  Usage:  setregmap path <big|little>."),
    HandlerDecl(simreset,       "Reset the simulation.  Usage:  simreset" ),
    HandlerDecl(modelreset,     "Reset the models but not memory.  Usage:  modelreset" ),
    HandlerDecl(setlogcmd,      "Add a logging call-back function.  Usage:  setlogcmd <event> <func> <filter (optional)>"),
    HandlerDecl(clrlogcmd,      "Remove a logging call-back function.  Usage:  clrlogcmd <event> <func>"),
    HandlerDecl(setbreakcmd,    "Add a breakpoint call-back function.  Usage:  setbreakcmd <address | symbol> <func>"),
    HandlerDecl(clrbreakcmd,    "Remove a breakpoint call-back function.  Usage:  clrbreakcmd <func>"),
    HandlerDecl(setwatchcmd,    "Add a watchpoint call-back function.  Usage:  setwatchcmd <address | symbol> <'read' | 'write' | 'access'> <func>"),
    HandlerDecl(clrwatchcmd,    "Remove a watchpoint call-back function.  Usage:  clrwatchcmd <func>"),
    HandlerDecl(simmode,        "Set or query type type of simulator to use.  Returns the simulator type.  Usage:  simmode [normal|dc|jit]"),
    HandlerDecl(setdynparm,     "Set a dynamic model parameter.  Usage:  setdynparm <parm-name> <value>"),
    HandlerDecl(getdynparm,     "Get a dynamic model parameter.  Usage:  getdynparm <parm-name> <value>"),
    HandlerDecl(listdynparm,    "List available dynamic parameters.  Usage:  listdynparm [parm-name]"),
    HandlerDecl(simstat,        "Retrieve information about a simulation statistic.  Usage:  simstat <stat-name>"),
    HandlerDecl(settd,          "Set a temporal-decoupling parameter (threshold, count, or incr) for a core or system; returns prior value.  Usage:  settd [path:]parm <value>"),
    HandlerDecl(gettd,          "Get a temporal-decoupling parameter (threshold, count, or incr) for a core or system.  Usage:  gettd [path:]parm"),
    HandlerDecl(help,           "Help information.  Usage:  help [command-name]" ),
    HandlerDecl(quit,           "Quit the program" ),
    { 0, 0, 0, 0 }
  };

}

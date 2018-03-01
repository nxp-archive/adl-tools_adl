//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// ADL specific code for the GDB interface.
// The RemoteGdbServer class implements both the DbgInterface interface and
// uses the GdbServer class for implementing the GDB protocol.
//

#include <cstring>
#include <iostream>
#include <iomanip>
#include <deque>

#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>

#include "rnumber/RNumber.h"

#include "helpers/Macros.h" 
#include "helpers/AnyOption.h" 

#include "StringHelpers.h"
#include "SimInterface.h"
#include "DbgInterface.h"
#include "ModelInterface.h"
#include "LoggingMgr.h"
#include "NodeHelpers.h"
#include "ScriptingIface.h"
#include "GdbServer.h"
#include "DbgRegMap.h"

using namespace std;
using namespace rnumber;

typedef boost::mutex::scoped_lock    scoped_lock;

namespace adl {
  
  // Add command-line options to the option database.  Note the use of logical &&
  // to ensure that we evaluate all of the additions.
  static bool dummy ATTRIBUTE_UNUSED = options().setFlag("gdb","g","GDB Debug mode.") && 
    options().setOption("port","p","Port to listen for GDB connections on.  0 (default) means pick a port.") && 
    options().setOption("pipe",    "Pipe file to use for GDB connections when using UNIX pipes.") && 
    options().setOption("port-file","pf","Writes to the specified file a 'target remote' gdb command.") &&
    options().setOption("gdb-hostname","Use the specified hostname when creating a port file.  Default = look up hostname.") &&
    options().setFlag("gdb-udp","Use UDP sockets for communicating with gdb.  Default = false (use TCP).") &&
    options().setFlag("gdb-halt","Start the simulator in a halted state.  Default = true.") &&
    options().setFlag("gdb-ext-mode","GDB should use extended-remote mode.  Default = true.");

  class GdbBrkParent;
  class RemoteGdbServer;

  // This is registered with the logging manager and is called when a breakpoint
  // or watchpoint occurs.  It records necessary information so that the debugger
  // interface can know what happened.
  class GdbBreakpoint : public LoggingIface {
  public:
    GdbBreakpoint(GdbBrkParent &parent,IssCore &core) :
      _parent(parent),
      _core(core),
      _ctx_id(-1),
      _ctx_num(-1),
      _address(0),
      _type(wpNone)
    {
    }

    void logBreakpoint(addr_t ea,unsigned handle,PtBase *);
    void logWatchpoint(addr_t ea,bool pre,WatchType type,int num_bytes,uint64_t value,unsigned handle,PtBase *);

    GdbBrkParent &_parent;
    IssCore      &_core;
    int           _ctx_id, _ctx_num;
    addr_t        _address;
    WatchType     _type;
    int           _cia_id;
  };

  struct GdbPt : public PtBase {
    addr_t    _address;
    WatchType _type;
    GdbPt(addr_t a,WatchType t) : _address(a), _type(t) {};
  };
  
  // This creates gdb-breakpoint handling classes, one per core.
  class GdbBrkParent : public LogParentIface {
  public:
    GdbBrkParent(RemoteGdbServer &server) : _server(server), _cur(0)
    {};

    virtual LoggingIface *register_core(IssCore &core)
    {
      unique_ptr<GdbBreakpoint> p(new GdbBreakpoint(*this,core));
      _breakpoints.push_back(move(p));
      return _breakpoints.back().get();;
    }
    
    Target get_and_clear_cur_core()
    {
      if (_cur) {
        auto c = _cur;
        _cur = 0;
        return Target(&c->_core,c->_ctx_id,c->_ctx_num);
      } else {
        return Target();
      }
    }

    addr_t    address() const { assert(_cur); return _cur->_address; };
    WatchType type() const    { assert(_cur); return _cur->_type; };
    
  protected:
    friend class GdbBreakpoint;

    RemoteGdbServer                    &_server;
    vector<unique_ptr<GdbBreakpoint> >  _breakpoints;
    GdbBreakpoint                      *_cur;
  };

  typedef unordered_map<int,string> Threadmap;

  struct GdbTarget;
  typedef vector<GdbTarget*> Children;

  // Stores core/context target information, with an additional value that is
  // the GDB thread ID number.  We also cache information about activity so that
  // we can temporarily set a node as inactive when the debugger doesn't want a
  // node to cycle, even though it would normally be active.
  struct GdbTarget : public Target {

    enum Type { TrgBad, TrgSys, TrgCore, TrgCtx };

    Type               _type;    
    string             _path;
    bool               _big_endian;
    unsigned           _gdb_id;
    unsigned           _num_threads;       // Number of threads in system/core.
    int                _active_thread;     // Active thread number (1..)
    int                _thread_id;         // The thread id of this item.
    bool               _has_ctx;           // True if core has any contexts.
    unsigned           _old_active_count;
    uint64_t           _orig_td_threshold; // Original td threshold from sim startup.
    uint64_t           _td_threshold;      // Current saved value.
    bool               _temp_halt;    
    bool               _cont;              // Used in non-stop mode to indicate
                                           // that a thread will be continued.
                                           // This way, we know what threads
                                           // have stopped, in order to avoid a
                                           // duplicate stop reply.
    unsigned           _io_active_count;
    Threadmap          _thread_map;        // Maps thread number to node name
                                           // and parent core (if just a
                                           // context).
    Children           _cores;             // Just points to child cores.
    const DbgRegInfos *_rinfo;

    GdbTarget() : 
      Target(), _rinfo(0) 
    { 
      init(); 
    };

    GdbTarget(IssNode *n,const DbgRegInfos *ri) : 
      Target(n), _gdb_id(0), _rinfo(ri) 
    { 
      init(); 
    };

    GdbTarget(IssNode *n,unsigned id,unsigned num,unsigned gdb_id,const DbgRegInfos *ri) : 
      Target(n,id,num), _gdb_id(gdb_id), _rinfo(ri) 
    { 
      init(); 
    };  

    const GdbTarget *getCore(IssCore *core) const
    {
      ForEach(_cores,i) {
        GdbTarget *trg = *i;
        if (trg->node() == core) return trg;
      }
      return 0;
    }

    GdbTarget *getCoreFromThreadId(unsigned thread_id)
    {
      assert(thread_id);
      return _cores[thread_id-1];
    }

    bool is_big_endian() const {
      return _big_endian;
    }

    bool is_core() const {
      return _type == TrgCore;
    }

    bool is_sys() const {
      return _type == TrgSys;
    }

    bool is_ctx() const {
      return _type == TrgCtx;
    }

    bool is_node() const { 
      return is_core() || is_sys();
    }

    bool has_ctx() const { return _has_ctx; };

    unsigned num_regs() const { 
      assert(_rinfo); 
      return _rinfo->size(); 
    };

    pair<int,int> reg_id(unsigned index) const { 
      assert(_rinfo && index < _rinfo->size()); 
      return make_pair((*_rinfo)[index]._id,(*_rinfo)[index]._index); 
    };

    unsigned reg_width(unsigned index) const {
      assert(_rinfo && index < _rinfo->size()); 
      return (*_rinfo)[index]._width; 
    };

    // This is the node's true activity, w/o considering the temporary halt.
    bool is_active() const {
      bool active = false;
      ForEach(_cores,i) {
        GdbTarget &trg = **i;
        active = active || (trg._temp_halt ? trg._old_active_count : trg.node()->is_active());
      }
      return active;
    }

    // If we're temporarily halted, then we don't consider ourself done, since
    // there's evidently more work to do (otherwise, we wouldn't have
    // temporarily halted ourself.  Otherwise, we just return the node's
    // activity- if not active, then we consider the simulation to have ended.
    bool is_done() const {
      bool active = false;
      ForEach(_cores,i) {
        GdbTarget &trg = **i;        
        if (trg._temp_halt) {
          active = true;
          break;
        } else {
          active = active || trg.node()->is_active();
        }
      }
      return !active;
    }

    // Recursively set the active count for systems.  Each core gets the same
    // count.  The value returned is the count for the last item.
    unsigned set_active_count(unsigned c)
    {
      unsigned r;
      ForEach(_cores,i) {
        GdbTarget &trg = **i;
        r = trg.node()->set_active_count(c);
      }
      return r;
    }

    void reset_temp_halt()
    {
      ForEach(_cores,i) {
        GdbTarget &trg = **i;
        trg._temp_halt = false;
      }      
    }

    // Note that individual contexts can't be active by themselves, so we ignore
    // contexts, since there will be a parent node that controls the overall
    // core's activation.
    void set_inactive() {
      ForEach(_cores,i) {
        GdbTarget &trg = **i;
        if (!trg.is_ctx() && !trg._temp_halt) {
          trg._temp_halt = true;
          trg._old_active_count = trg.node()->set_active_count(0);
        }
      }
    }

    // Same as above, but uses a bool vector to specify which items should be
    // set active.  A true means that we toggled activity during restore_active,
    // so we should toggle back if relevant.
    void set_inactive(vector<bool> &activity) {
      assert(activity.size() == _cores.size());
      unsigned count = 0;

      ForEach(_cores,i) {
        GdbTarget &trg = **i;
        if (!trg.is_ctx() && !trg._temp_halt && activity[count]) {
          trg._temp_halt = true;
          trg._old_active_count = trg.node()->set_active_count(0);
        }
        ++count;
      }      
    }

    // This returns a bool vector representing the prior activity state.
    vector<bool> restore_active() {
      vector<bool> prior_states(_cores.size(),false);
      int count = 0;

      ForEach(_cores,i) {
        GdbTarget &trg = **i;
        if (trg._temp_halt) {
          prior_states[count] = true;
          DPrint( "Restoring temp active.");
          trg._temp_halt = false;
          trg.node()->set_active_count(trg._old_active_count);
        }
        ++count;
      }

      return prior_states;
    }

    // If we're temporarily halted, then check to see if the node is still
    // inactive.  If not, then make it inactive.
    void check_inactive() {
      ForEach(_cores,i) {
        GdbTarget &trg = **i;
        if (trg._temp_halt) {
          DPrint( "We should be halted- making sure.");
          if (trg.node()->is_active()) {
            trg._old_active_count = node()->set_active_count(0);
          }
        }
      }
    }

    void save_td_threshold() {
      ForEach(_cores,i) {
        GdbTarget &trg = **i;
        // Set back to our default.  For single core, this will be the
        // instruction max, so that we don't hit a small value when gdb expects
        // us to simulate until a breakpoint.  For MP/MT, we want to keep a
        // smaller value so that we switch between cores.
        trg._td_threshold = trg.node()->set_td_threshold(trg._orig_td_threshold);
      }
    }

    void restore_td_threshold() {
      ForEach(_cores,i) {
        GdbTarget &trg = **i;
        trg.node()->set_td_threshold(_td_threshold);
      }
    }

    // Simply set any active cores to be inactive temporarily, in order to halt
    // simulation due to in-coming I/O.  We then restore it as soon as possible,
    // so as not to interfere with the active/in-active work we do for stepping
    // and continuing.
    void set_io_inactive() {
      ForEach(_cores,i) {
        GdbTarget &trg = **i;
        IssNode &node = *trg.node();
        if (node.get_active_count()) {
          DPrint("  " << node.name() << ":  Making inactive due to I/O.");
          trg._io_active_count = trg.node()->set_active_count(0);
        }
      }
    }

    void restore_from_io() {
      ForEach(_cores,i) {
        GdbTarget &trg = **i;
        IssNode &node = *trg.node();
        if (trg._io_active_count) {
          DPrint( "  " << node.name() << ":  Restoring from I/O inactive.  New active count:  " << trg._io_active_count);
          node.set_active_count(trg._io_active_count);
          trg._io_active_count = 0;
        }
      }
    }

  private:
    void init() {
      _big_endian = true;
      _orig_td_threshold = 0;
      _td_threshold = 0;
      _type = TrgBad;
      _num_threads = 1;
      _active_thread = 1;
      _thread_id = -1;
      _old_active_count = 0;
      _io_active_count = 0;
      _temp_halt = false;
      _has_ctx = false;
      _cont = false;
    }
  };

  // KEY_FUNC: class RemoteGdbServer
  class RemoteGdbServer : public DbgInterface, public GdbServer 
  {
    typedef std::unordered_map<string,GdbTarget> CoreMap;
    typedef std::vector<GdbTarget*> PortMap;

  public:

    RemoteGdbServer(IssNode &root,RunControlIface &runctrl,const string &hostname,const string &port,const string &port_file,
                    GdbTransportType type,bool verbose,bool start_halted,bool extended_mode,bool has_waveform=false) :
      GdbServer(type,hostname,verbose,has_waveform),
      _rootnode(root),
      _runctrl(runctrl),
      _root(0),
      _breakpointLogger(*this),
      _running(false),
      _io_halted(false),
      _use_ext_mode(extended_mode)
    {
      // Our max packet size is based upon the amount of space required to
      // send/receive info on all registers.
      try {
        auto p = populateCoreMap("",root);
        setMaxPacketSize(p.first+4);
      }
      catch (exception &err) {
        RError(err.what() << "  Did you remember to set up register mappings in a config file?");
      }

      _theMgr.setRoot(&root);
      registerLogger(&_breakpointLogger);  

      // This parses the port and port-file specification and then provides this information
      // to the base class.
      parse_port_info(port,port_file);

      set_all_inactive();
    }

    // Main entry point:  Run the debugger until it tells us to quite.
    void exec() 
    {
      rundebugger();
    }

    bool use_extended_mode() const
    {
      return _use_ext_mode;
    }

    bool remoteExtendedSupport() const 
    { 
      return use_extended_mode(); 
    }

    void setup_dbg_target(GdbTarget *trg,unsigned port,const std::string &port_file)
    {
      // We don't allow a debugger to point directly to a context thread.
      if (trg->is_ctx()) {
        RError("Only cores or systems may be debugger targets.");
      }

      // Add on port data and push on an entry on our port map.  This yields a
      // 1-1 mapping between GdbTarget objects and, on the server side, the port
      // and port-file to be used.
      addPortData(port,port_file);
      _portMap.push_back(trg);
    }

    // Parse the port and port-file specification.  
    // The format for port is:
    //   <int>
    // or
    //   <path>=<int>[,<path>=<int>...]
    //
    // The format for port-file is:
    //   <file-name>
    // or
    //   <path>=<file-name>[,<path>=<file-name>...]
    //
    void parse_port_info(const string &port,const string &port_file)
    {
      // First, parse each item into lists based upon the ',' separator.
      auto ports = tokenize(port,",");
      auto port_files = tokenize(port_file,",");

      if (!ports.empty()) {
        // Else, we should have the same number of ports and port-files, assuming any port-files were specified.
        if (!port_files.empty() && (ports.size() != port_files.size())) {
          RError("Size mismatch between the number of ports and port-files specified.");
        }
      }

      if (ports.size() == 1) {
        // Special case- allow for a single, integer port value.
        unsigned p;
        if (getUInt(p,ports[0])) {
          // If we have a port-file, use it.  Otherwise, it's OK.
          string pf = (!port_files.empty()) ? port_files[0] : string();
          // For this case, since we have only one node, we just use the default node that's setup.
          setup_dbg_target(_root,p,pf);
          return;
        }
      } else if (ports.empty() && port_file.find('=') == string::npos) {
        // Another special case:  No port, but just a single port-file.
        // For this case, since we have only one node, we just use the default node that's setup.
        setup_dbg_target(_root,GdbServerDefaultPort,port_file);
        return;
      }

      map<string,pair<unsigned,string> > pmap;

      // General case:  Parse each entry as a path=port value.  We expect the same for port-files.
      // Parse everything into a map, first.
      ForEach(ports,piter) {
        auto ppair = tokenize(*piter,"=");
        if (ppair.size() != 2) {
          RError("Expected a path=port pair but found '" << *piter << "'.");
        }
        unsigned p;
        if (!getUInt(p,ppair[1])) {
          RError("Expected an integer for a port with '" << *piter << "'.");
        }
        pmap[ppair[0]] = make_pair(p,string());
      }

      ForEach(port_files,piter) {
        auto ppair = tokenize(*piter,"=");
        if (ppair.size() != 2) {
          RError("Expected a path=port-file pair but found '" << *piter << "'.");
        }
        // Try to insert a new element.
        auto ip = pmap.insert(make_pair(ppair[0],make_pair((unsigned)GdbServerDefaultPort,ppair[1])));        
        // If not inserted, then just update the filename (second element of the pair).
        if (!ip.second) {
          ip.first->second.second = ppair[1];
        }
      }

      // Now go through and make sure that all paths are valid and add port-data
      // to the base class and add an element to our port-index -> target map.
      ForEach(pmap,i) {
        auto citer = _coreMap.find(i->first);

        if (citer == _coreMap.end()) {
          RError("Unknown path specified for port/port-file:  " << i->first);
        }

        setup_dbg_target(&(citer->second),i->second.first,i->second.second);
      }
    }

    // This returns the size required to store all register data for this node, which
    // corresponds to the largest packet we'll encounter.
    unsigned calcMaxPacketSize(const DbgRegInfos &ri) const
    {
      if (ri.empty()) { 
        RError ("Unable to create GdbServer for 0 registers.  Did you forget to setup a register mapping?"); 
      }
      int size = 0;
      ForEach(ri,i) {
        size += i->_width / 4;
      }
      return size;
    }

    // Walks the model hierarchy and builds our mirror hierarchy of CoreInfos.
    // Returns the max packet size encountered, a list of the nodes created, and
    // true if we had contexts, false otherwise.
    pair<unsigned,Children> populateCoreMap( const string &parent_path,IssNode &node) 
    {
      // Construct a CoreInfo for this node.
      string path = add_to_path(parent_path,node,PathDelim);

      unsigned max_packet_size = 0;

      GdbTarget *trg = 0;
      unsigned num_threads = 0;

      Children new_trgs;
      
      if (node.is_core()) {
        // Find a register mapping object.  We need this in order to talk with the debugger.
        const DbgRegInfos &ri = dbgRegMap().find(node);
        // If any contexts exist with the "thread" attribute, then add those as children as well.
        auto ctx = getThreadContext(node);
        if (ctx) { 
          trg = &(_coreMap.insert(make_pair(path,GdbTarget(&node,-1,-1,0,&ri))).first->second);
          new_trgs.push_back(trg);
          ForRange(ctx->_num_contexts,i) {
            ostringstream ss;
            ss << ctx->_name << i;
            string tpath = add_to_path(path,ss.str(),PathDelim);
            GdbTarget *c_trg = &(_coreMap.insert(make_pair(tpath,GdbTarget(&node,ctx->_id,i,++num_threads,&ri))).first->second);
            new_trgs.push_back(c_trg);
            c_trg->_type = GdbTarget::TrgCtx;
            c_trg->_path = tpath;
            c_trg->_big_endian = trg->node()->isBigEndian();
            c_trg->_thread_id = num_threads;
            trg->_thread_map.insert(make_pair(num_threads,tpath)); 
            // Contexts' cores just point back to the parent.
            c_trg->_cores.push_back(trg);
          }
          trg->_has_ctx = true;
        } else {
          // No threads for this core, so just add the item itself as a thread.
          trg = &(_coreMap.insert(make_pair(path,GdbTarget(&node,-1,-1,++num_threads,&ri))).first->second);
          trg->_thread_map.insert(make_pair(num_threads,path)); 
          trg->_path = path;
          trg->_thread_id = num_threads;
          new_trgs.push_back(trg);
        }
        trg->_type = GdbTarget::TrgCore;
        trg->_big_endian = trg->node()->isBigEndian();
        trg->_num_threads = num_threads;
        trg->_orig_td_threshold = node.get_td_threshold();
        trg->_cores.push_back(trg);
        max_packet_size = calcMaxPacketSize(ri);
      } else {
        // If we have children, construct them, too.
        trg = &(_coreMap.insert(make_pair(path,GdbTarget(&node,-1,-1,0,0))).first->second);
        trg->_type = GdbTarget::TrgSys;
        trg->_path = path;
        unsigned num_threads = 0;
        unsigned tcount = 1;
        ForEach(node.children(),iter) {
          auto p = populateCoreMap( path, **iter);
          max_packet_size = max(max_packet_size,p.first);
          ForEach(p.second,c_iter) {
            // If this element has contexts, then it represents the parent and
            // the rest of the elements are the contexts.  These we use as the
            // threads and we ignore this one.
            auto & ctrg = **c_iter;
            if (ctrg._has_ctx) continue;
            num_threads += ctrg._num_threads;
            ctrg._thread_id = tcount;
            trg->_thread_map.insert(make_pair(tcount++,ctrg._path));
          }
          new_trgs.insert(new_trgs.end(),p.second.begin(),p.second.end());
          // Then add on elements which are actually cores.
          ForEach(p.second,c_iter) {
            if ((*c_iter)->is_core()) {
              trg->_cores.push_back(*c_iter);
            }
          }
        }
        trg->_num_threads = num_threads;
      }

      trg->_old_active_count = node.get_active_count();

      // Store a pointer to the root target if that's what we've just added to
      // the map.
      if (&node == &_rootnode) {
        _root = trg;
      }

      return make_pair(max_packet_size,new_trgs);
    }

    // Checks all items for their temporary halt status.  This is necessary b/c
    // a register modification may have activated a core, so we must make sure
    // that we keep our status consistent.
    void check_temp_halt()
    {
      ForEach(_coreMap,i) {
        if (i->second.is_core()) {
          i->second.check_inactive();
        }
      }
    }

    // Sets all nodes as inactive.
    void set_all_inactive()
    {
      scoped_lock sl(_io_mutex);

      ForEach(_coreMap,i) {
        if (i->second.is_core()) {
          i->second.reset_temp_halt();
          i->second.set_inactive();
        }
      }
    }

    void save_td_threshold()
    {
      ForEach(_coreMap,i) {
        if (i->second.is_core()) {
          i->second.save_td_threshold();
        }
      }
    }

    void restore_td_threshold()
    {
      ForEach(_coreMap,i) {
        if (i->second.is_core()) {
          i->second.restore_td_threshold();
        }
      }
    }



    //
    // This represents a top-level node, i.e. something which maps to a gdb
    // connection.
    //
    class RemoteGdbServerNode : public GdbServerNode {
      
      RunControlIface       &_runctrl;
      GdbTarget             &_cur;              // Current core/thread target.    
      RemoteGdbServer       &_parent;
      bool                   _finished, _interrupt;
      unsigned               _breakpoint;       // 0 => no breakpoint, else
                                                // thread id of item which
                                                // caused breakpoint.

      deque<string>          _stop_seq;         // In non-stop mode, stores a
                                                // list of stopped threads. We
                                                // then send these out in
                                                // response to vStopped queries
                                                // from gdb.
      
    public:
      RemoteGdbServerNode(RemoteGdbServer &server,bool verbose,GdbTarget &cur) :
        GdbServerNode(server,verbose),
        _runctrl(server._runctrl),
        _cur(cur),
        _parent(server),
        _finished(false),
        _interrupt(false),
        _breakpoint(0)
      {
        
      }

      const std::string &name() const
      {
        return _cur.node()->name();
      }

      const GdbTarget *getTargetFromCore(IssCore *core)
      {
        return _cur.getCore(core);
      }

      void setBreakpoint(IssCore *core)
      {
        if (const GdbTarget *trg = getTargetFromCore(core)) {
          _breakpoint = trg->_thread_id;
        }
      }

      //---------------------------------------------------------------------------
      // Return a node, based upon current threading information, if applicable,
      // i.e. we're modeling threads as cores, rather than contexts.  You need to
      // call this to get a "current" thread before doing any command which is
      // affected by the H command.
      //
      // first_core:  If true, then if _active_thread is -1, pick the first real core.
      //              Otherwise, return the system.
      //---------------------------------------------------------------------------
      GdbTarget *cur_core(bool first_core = true)
      {     
        if (_cur._active_thread < 0) {
          if (first_core) {
            return _cur._cores.front();
          } else {
            return &_cur;
          }
        }
        IfFind(t_iter,_cur._thread_map,_cur._active_thread) {
          IfFind(c_iter,_parent._coreMap,t_iter->second) {
            return &c_iter->second;
          }
        }
        assert(0);
      }

      // Gets (and clears) the thread which caused a breakpoint, or else returns
      // the current active thread if the breakpoint logger doesn't have anything.
      // Used for when we model threads as cores.
      unsigned getThreadFromBreakptLogger()
      {
        auto btrg = _parent._breakpointLogger.get_and_clear_cur_core();
        if (btrg.node()) {
          // We don't have a good way to find thread numbers from core pointers,
          // so we have to do a linear search.  First, find the core in the core
          // map.
          ForEach(_parent._coreMap,citer) {
            if (citer->second == btrg) {
              // Found a match, so then use that name to look up, in our current
              // thread map, the thread number.
              ForEach(_cur._thread_map,titer) {
                if (citer->first == titer->second) {
                  // Found it.
                  return titer->first;
                }
              }
            }
          }
        }
        return _cur._active_thread;
      }

      void updateActiveContexts(GdbTarget &trg)
      {
        if (trg.is_core()) {
          if (trg._num_threads > 1) {
            auto ctx = getThreadContext(*_cur.node());
            if (ctx) { 
              trg._active_thread = trg.node()->getActiveCtx(ctx->_id)+1;
            }
          }
        } else if (trg.is_sys()) {
          ForEach(trg._cores,i) {
            updateActiveContexts(**i);
          }
        }
      }

      // Return the active node in the model.  This first does a recursive scan
      // to handle any coarse-grained threading, then looks to see if the active
      // thread changed due to a breakpoint.
      unsigned updateActiveNode()
      {
        if(_cur._num_threads > 1) {
          updateActiveContexts(_cur);
          _cur._active_thread = getThreadFromBreakptLogger();
        }
        return _cur._active_thread;
      }

      bool remoteNonStopSupported() const
      {
        return true;
      }

      void remoteNonStopMode(bool entering)
      {
        setNonStopMode(entering);
        makeGdbPacket("OK");
      }
      
      // KEY_FUNC: remoteReverseExecSupported in remoteGdbServerNode
      bool remoteReverseExecSupported()
      {
        // return true;  
        auto ct = cur_core();
        auto support_reverse = _runctrl.hasReverseExecSupport(*ct->node());
        
        return support_reverse;
      }

      void remoteIOHalt()
      {
        _parent.ioHalt();
      }

      void remoteInterrupt()
      {
        scoped_lock sl(_parent._io_mutex);

        _cur.set_inactive();
        _interrupt = true;
      }

      void setupForContinue()
      {
        _interrupt = false;
        _breakpoint = 0;

        updateActiveNode();
      }

      void remoteSetupContinue(bool valid_addr,addr_t new_addr)
      {
        auto ct = cur_core();

        if (valid_addr) {
          ct->node()->setProgramCounter(new_addr);
        }

        ct->restore_active();
        ct->_cont = true;

        setupForContinue();
      }

      // Updates the current core information, given a thread number.
      void updateFromThreadNum(unsigned threadNo)
      {
        Threadmap::iterator iterT = _cur._thread_map.find(threadNo);
        if (iterT != _cur._thread_map.end()) {
          CoreMap::iterator iterC =  _parent._coreMap.find(iterT->second);
          if (iterC == _parent._coreMap.end()) {
            RError("Bad core identifier '" <<  "'.");
          } else {
            GdbTarget &trg = iterC->second;
            // We set the debug context to this thread so that subsequent
            // register updates will target this thread.  Note that we don't
            // change _cur, since we really just want _cur to reflect the
            // current core.
            if (trg.is_ctx()) {
              trg.setDbgCtx();
            }
            _cur._active_thread = threadNo;
          }
        }
      }

      // detect thread switch relative to a breakpoint
      bool syncThread(unsigned *threadNum)
      {
        unsigned priorThreadNum = _cur._active_thread; 
        *threadNum = updateActiveNode();
        if (*threadNum != priorThreadNum) {
          updateFromThreadNum(*threadNum);
          return true;
        } else {
          return false;
        }
      }

      bool remoteStepFinishResponse()
      {
        unsigned threadNum;

        if (!nonStopMode()) {
          if (_finished) {
            makeGdbPacket("X00"); 
            // Disconnect if we're done and not in extended mode.
            if (!extended_mode()) {
              serverFinish();
            }
          } else if (_cur._num_threads > 1 && syncThread(&threadNum)) {
            makeGdbPacket("T05thread:%x;",threadNum);
            return true;
          } else {
            makeGdbPacket("S05");
           }
        }
        return false;
      }
      
      // KEY_FUNC: remoteRStepFinishResponse
      void remoteRStepFinishResponse()
      {
        unsigned threadNum;

        if (!nonStopMode()) {
          if (_finished) {
            makeGdbPacket("X00"); 
            // Disconnect if we're done and not in extended mode.
            if (!extended_mode()) {
              serverFinish();
            }
          } else if (_cur._num_threads > 1 && syncThread(&threadNum)) {
            makeGdbPacket("T05thread:%x;",threadNum);
          } else {
            makeGdbPacket("S05");
          }
        }
      }
      
      // KEY_FUNC: remoteRContinueFinishResponse
      void remoteRContinueFinishResponse()
      {
        unsigned threadNum;

        if (!nonStopMode()) {
          if (_finished) {
            makeGdbPacket("X00"); 
            // Disconnect if we're done and not in extended mode.
            if (!extended_mode()) {
              serverFinish();
            }
          } else if (_cur._num_threads > 1 && syncThread(&threadNum)) {
            makeGdbPacket("T05thread:%x;",threadNum);
          } else {
            makeGdbPacket("S05");
          }
        }
      }

      void addThreadStop(unsigned id)
      {
        // In non-stop mode, we add this stepped item to our stopped list to
        // report back to gdb.
        ostringstream ss;
        ss << "T05thread:" << id << ";";
        _stop_seq.push_back(ss.str());
      }

      void setFinished()
      {
        _finished = true;
        updateFromThreadNum(1);
      }
      
      // Called to step the current node.
      void remoteStep()
      {
        scoped_lock sl(_parent._io_mutex);

        updateActiveNode();	   
        auto ct = cur_core(); 

        // We're restoring all cores, in case the client (which might be a
        // different scheduler using a different run-control interface) needs to
        // advance any other cores.  In that case, we don't want them to appear
        // halted.
        auto actives = _cur.restore_active();
        //ct->restore_active();

        auto status = _runctrl.stepn(1,*ct->node());

        // Now restore everything back to being inactive.
        _cur.set_inactive(actives);
        //ct->set_inactive();

        if (status == HALT) {
          setFinished();
        }

        if (nonStopMode()) {
          addThreadStop(ct->_thread_id);
        }
      }

      // KEY_FUNC: remoteRStep
      // Called to reverse step the current node.
      void remoteRStep()
      {
        scoped_lock sl(_parent._io_mutex);

        updateActiveNode();	   
        auto ct = cur_core(); 
        ct->restore_active();
        auto status = _runctrl.rStepn(1,*ct->node());
        ct->set_inactive();
        if (status == HALT) {
          setFinished();
        }

        if (nonStopMode()) {
          addThreadStop(ct->_thread_id);
        }
      }      
      
      // KEY_FUNC: remoteRContinue
      void remoteRContinue()
      {
        scoped_lock sl(_parent._io_mutex);

        updateActiveNode();	   
        auto ct = cur_core(); 
        ct->restore_active();
        auto status = _runctrl.rRun(*ct->node(),simMode());
        ct->set_inactive();
        if (status == HALT) {
          setFinished();
        }

        if (nonStopMode()) {
          addThreadStop(ct->_thread_id);
        }
      }
      
      // Alert GDB to the first stopped thread.  It'll then query via vStopped
      // to get all status.
      void remoteAsyncStop()
      {
        if (!_stop_seq.empty()) {
          putGdbAsyncPacket("Stop:"+getNextStopItem());
        } else {
          // If nothing to send, then we just want to listen for the next data
          // from gdb.
          noResponse();
        }
      }

      // GDB's behavior with stop sequences and get-last-signal in non-stop mode
      // is kinda weird.  It's treated as a sequence, where, in the case of
      // get-last-signal ('?'), we first send a synchronous reply with the first
      // stop response.  GDB keeps sending vStopped until it gets "OK".  Each
      // stop-sequence reply can only refer to a single thread and we cannot
      // repeat.  This function is used to query for *all* stopped threads.
      void startStopSequence()
      {
        _stop_seq.clear();

        // Iterate over each of the threads.  If we find a stopped one, add
        // it to the list of stopped threads.
        ForEach(_cur._cores,i) {
          const GdbTarget &thrd = **i;
          // We actually want the real activity here, since gdb needs to know if
          // a thread is "stopped" or not, which includes being temporarily
          // halted by us.
          if (!thrd.node()->is_active()) {
            addThreadStop(thrd._thread_id);
          }
        }        
      }

      string getNextStopItem()
      {
        assert (!_stop_seq.empty());
        string s = _stop_seq.front();
        _stop_seq.pop_front();
        return s;
      }

      // This is only ever called as a result of a vStopped command and thus
      // simply continues a sequence.
      void remoteSendNextStopSequence()
      {
        if (_stop_seq.empty()) {
          makeGdbPacket("OK");
        } else {
          putGdbPacket(getNextStopItem(),true);
        }
      }

      void remoteGetLastSignal()
      {
        if (_finished) {
          makeGdbPacket("X00");
          // Disconnect if we're done and not in extended mode.
          if (!extended_mode()) {
            serverFinish();
          }
        } else {
          // In non-stop mode, we have to describe the state of all threads.
          // This has to be done as a sequence of writes, one per thread,
          // followed by "OK".
          if (nonStopMode()) {
            startStopSequence();
            remoteSendNextStopSequence();
          } else {
            // Currently hardcoded to signal 5, unless we're finished executing.
            makeGdbPacket("S%02X",5);
          }
        }
      }

      // remote_get_memory: get memory values 
      // format of data is a packed character string with 2 hex digits per byte       
      // for example, a command to get 8 bytes of memory containined 0xabababab and   
      // 0xdeadbeef would be return with this data: "ABABABABDEADBEEF"                
      void remoteGetMemory(addr_t start_address,unsigned long length_in_bytes,bool direct) 
      {
        auto ct = cur_core();

        uint32_t memVal = 0;
        unsigned int addrOffset = 0;
        unsigned long localLength = length_in_bytes;

        ostringstream ss;
        ss << noshowbase << hex;

        // GDB's remote serial protocol is endian neutral for memory reads, so
        // this has to be done byte-by-byte.
        while (localLength != 0) {
          addr_t a = start_address + addrOffset;

          if (direct) {
            memVal = ct->node()->getMem(GlobalMemId,a,8);
          } else {
            ct->node()->debug_data_read(memVal,GlobalMemId,true,a,8);
          }

          ss << setw(2) << setfill('0') << memVal;

          localLength -= 1;
          addrOffset += 1;
        }

        putGdbPacket(ss.str());
      }

      // Remote_set_memory: Set memory values .
      //
      // Format of data is a packed character string with 2 hex digits per byte
      // for example, a command to set 8 bytes o memory to 0xabababab and
      // 0xdeadbeef would be supplied with this data: "ABABABABDEADBEEF"
      void remoteSetMemory(addr_t start_address,
                           unsigned long length_in_bytes,
                           char *data,bool direct) 
      {
        auto ct = cur_core();

        unsigned tempLength = 0;
        char tmp[9];
        unsigned tmpInt = 0;

        // GDB's remote serial protocol is endian neutral for memory writes, so
        // this has to be done byte-by-byte.
        while (tempLength < length_in_bytes) {
          strncpy(tmp, data + (2 * tempLength), 2);
          tmp[2] = 0;
          sscanf((const char *)tmp, "%X", &tmpInt);

          addr_t addr = start_address + tempLength;

          if (direct) {
            ct->node()->setMem(GlobalMemId,addr,tmpInt,8);
          } else {
            ct->node()->debug_data_write(GlobalMemId,true,addr,tmpInt,8);
          }
          tempLength++;
        }

        makeGdbPacket("OK");
      }

      void get_next_byte(uint32_t &val,int &escaped,char *data)
      {
        if (*data != 0x7d) {        
          val |= (0x000000FF & ((unsigned int) *data + (escaped * 0x20)));
          escaped = 0;
        } else {
          escaped = 1;
        }
      }

      // remote_set_memory_binary: Set memory values.
      //
      // Format of data is a packed character string with binary data, where 0x7d
      // is the escape character and means that the next character should have
      // 0x20 added to it.  The escape character does not factor into the byte
      // count.
      void remoteSetMemoryBinary(addr_t start_address,unsigned long length_in_bytes,char *data) 
      {
        auto ct = cur_core();

        unsigned tempLength = 0;
        uint32_t tmp = 0;
        int escaped = 0;

        while (tempLength < length_in_bytes) {
          do {
            get_next_byte(tmp,escaped,data++);
          } while (escaped);

          addr_t addr = start_address + tempLength;
          VPrint ("remoteSetMemoryBinary:  0x" << hex << addr << " = 0x" << tmp);
          ct->node()->debug_data_write(GlobalMemId,true,addr,tmp,8);

          ++tempLength;
          tmp = 0;
        }

        makeGdbPacket("OK");
      }

      // for "mon reg" command
      // KEY_FUNC: writeMonRegValue
      bool writeMonRegValue(ostream &o,GdbTarget *trg,int reg)
      {
        auto ri = trg->reg_id(reg);
        bool regExists = ri.first >= 0;
        unsigned register_width = trg->reg_width(reg);

        if (regExists) {
          RNumber value(0,register_width);        
          trg->node()->getReg(ri.first,ri.second,value);
          writeValue(o,value,trg->is_big_endian());
          VPrint("Register get:  " << ri.first << "," << ri.second << ":  " << hex << value << dec);
          //cout << i << ": Register " << ri.first << "(" << ri.second << ") " << register_width << " bits:  " << tmp.str().substr(start) << endl;
        } else {
          o << string(register_width/4,'0');
        }

        return regExists;
        
      }
      
      // Given a register id and a target object, writes register information as a
      // hex string to the specified stream. Writes 0s if the register doesn't
      // exist.  Returns true if the register exists, false if not.
      bool writeRegValue(ostream &o,GdbTarget *trg,int reg)
      {
        auto ri = trg->reg_id(reg);
        bool regExists = ri.first >= 0;
        unsigned register_width = trg->reg_width(reg);

        if (regExists) {
          RNumber value(0,register_width);        
          trg->node()->getReg(ri.first,ri.second,value);
          writeValue(o,value,trg->is_big_endian());
          VPrint("Register get:  " << ri.first << "," << ri.second << ":  " << hex << value << dec);
          //cout << i << ": Register " << ri.first << "(" << ri.second << ") " << register_width << " bits:  " << tmp.str().substr(start) << endl;
        } else {
          o << string(register_width/4,'0');
        }

        return regExists;
      }

      // remote_get_registers: get all register values 
      // Format of data is a packed character string with 2 hex digits per byte
      // and no radix prefix.  For example, a processor with two registers with
      // 0xabababab and 0xdeadbeef would return this data: "ABABABABDEADBEEF"
      // KEY_FUNC: remoteGetRegisters
      void remoteGetRegisters() 
      {
        ostringstream ss;        
                
        auto ct = cur_core();

        VPrint ("Register source:  " << ct->node()->name());
        
        if (_parent.has_waveform){
          uint32_t time = ct->node()->get_td_counter();
          setWaveformTime(time);
        }
          
        int regs = ct->num_regs();

        ss << noshowbase << hex;

        for (int i = 0; i < regs; i++) {
          writeRegValue(ss,ct,i);
          //VPrint ("Register get so far: " << ss.str());
        }                 
  
        putGdbPacket(ss.str());
      }

      unsigned getNibble(char c)
      {
        if (c >= '0' && c <= '9') {
          return (c - '0');
        }
        else if (c >= 'a' && c <= 'f') {
          return (c - 'a') + 10;
        }
        else if (c >= 'A' && c <= 'F') {
          return (c - 'A') + 10;
        }
        else {
          RError("Unknown nibble value encountered: " << c);
        }
      }

      // Convert an array of nibbles into a value.  We get endianness
      // information from the core in order to convert appropriately.
      RNumber getValue(const char *data,int len,bool is_big_endian)
      {
        RNumber val(0,len*4);

        // Should be an even number of nibbles, since this is a string of bytes.
        assert(len % 2 == 0);
  
        if (is_big_endian) {
          // Big-endian, so msb is on the left.
          for (int i = 0; i != len; i += 2) {
            val <<= 8;
            unsigned b = (getNibble(data[i]) << 4) | getNibble(data[i+1]);
            val |= b;
          }          
        } else {
          // Little-endian, so msb is on right.
          for (int i = len-1; i >= 0; i -= 2) {
            val <<= 8;
            unsigned b = (getNibble(data[i-1]) << 4) | getNibble(data[i]);
            val |= b;
          }
        }

        return val;
      }

      // Writes a stream of hex digits in the target core's endianness to the
      // specified stream.
      void writeValue(ostream &o,const RNumber &value,bool is_big_endian)
      {
        int len = value.size()/8;
        unsigned char bytes[len];
  
        value.bigEndianArrayOfBytes(bytes);

        o << hex;
        if (is_big_endian) {
          for (int i = 0; i != len; ++i) {
            o << setfill('0') << setw(2) << (unsigned)bytes[i];
          }
        } else {
          for (int i = len-1; i >= 0; --i) {
            o << setfill('0') << setw(2) << (unsigned)bytes[i];
          }
        }
      }
      
         
      // remote_set_registers: set all the registers in the processor 
      void remoteSetRegisters(const char *data) 
      {
        const char *ptr = data;
        
        auto ct = cur_core();
    
        for(unsigned register_number=0; register_number != ct->num_regs(); register_number++) {
          // For each of the registers get the register info from the iss
          auto ri = ct->reg_id(register_number);
          bool regExists = ri.first >= 0;
          int register_width = ct->reg_width(register_number) / 4;

          RNumber value = getValue(ptr,register_width,ct->is_big_endian());
          ptr += register_width;
          
          VPrint("Register set:  " << dec << ri.first << "," << ri.second << ":  0x" << hex << value);
          if (regExists) {
            ct->node()->setReg(ri.first, ri.second, value);
          }
        }

        // Node activity may have been affected by setting a register, so we
        // make sure that there's nothing extra that suddenly turned active and
        // we check for any thread switches.
        _parent.check_temp_halt();
        updateActiveNode();

        makeGdbPacket("OK");
      }

      void remoteGetRegister(unsigned reg)
      {
        auto ct = cur_core();
  
        ostringstream ss;
        ss << noshowbase << hex;

        if (reg >= ct->num_regs()) {
          ss << "00000000";
        } else {
          writeRegValue(ss,ct,reg);
        }

        putGdbPacket(ss.str());
      }

      // Required to support all register sizes 
      // remote_set_register: set a register in the processor 
      void remoteSetRegister(unsigned reg, const string &val) 
      {
        auto ct = cur_core();

        if (reg >= ct->num_regs()) {
          makeGdbPacket("E00");
          return;
        }

        auto ri = ct->reg_id(reg);
        bool regExists = ri.first >= 0;

        RNumber value = getValue(val.c_str(),val.size(),ct->is_big_endian());
        
        //---------------------------------
        // something failed if setReg fails
        //---------------------------------
        if (regExists) {
          ct->node()->setReg(ri.first, ri.second, value);
        }

        // Node activity may have been affected by setting a register, so we
        // make sure that there's nothing extra that suddenly turned active and
        // we check for any thread switches.
        _parent.check_temp_halt();
        updateActiveNode();
      
        makeGdbPacket("OK");
      }

      // remoteSetThread: switch the current core
      void remoteSetThread(int threadNo,bool reply) 
      {
        // Change the current core.  The action for c and g is the same, as we
        // mainly rely upon vCont for per-thread control of simulation.  We ignore
        // -1 thread numbers, which mean "all threads".
        if (threadNo >= 0) {
          updateFromThreadNum(threadNo);
        }
        if (reply) {
          makeGdbPacket("OK");
        }
      }

      void remoteSetThreadDirect(int threadNum)
      {
        _cur._active_thread = threadNum;
      }

      // Stop the specified thread (make inactive).
      void remoteStopThread()
      {
        GdbTarget *thrd = _cur.getCoreFromThreadId(_cur._active_thread);
        assert(thrd && thrd->_thread_id == _cur._active_thread);

        thrd->set_inactive();

        // Report on a thread stoppage if the thread was continuing but has now
        // stopped.
        if (thrd->_cont) {
          addThreadStop(thrd->_thread_id);
          thrd->_cont = false;
        }
      }
      
      // KEY_FUNC: remoteKill
      void remoteKill()
      {
        if (_parent.has_waveform) {
          waveform_ptr()->_w_io_service.post(bind(&WaveformThread::finish,waveform_ptr()));  
        }
        
        // We're done with the program at this point.
        setFinished();
      }

      // If there are any cores which are active, then we should simulate.
      bool remoteSimable() const
      {
        ForEach(_parent._coreMap,i) {
          if (i->second.is_core() && i->second.is_active()) {
            return true;
          }
        }
        return false;
      }

      // We do support restart and other extended features.
      bool remoteExtendedSupport() const
      {
        return _parent.use_extended_mode();
      }

      // Return 0 if we're SMT, or the active thread if we're course-grained.
      int remoteGetSimableThreadId()
      {
        if (_cur.has_ctx()) {
          return _cur._active_thread;
        } else {
          return 0;
        }
      }

      int remoteGetActiveThreadId()
      {
        return _cur._active_thread;
      }


      // remoteGetThreadInfo: Return a list of valid thread IDs.
      ThreadIdList remoteGetThreadInfo() 
      {
        ThreadIdList t(1,1);
      
        // Default case is 1
        for(unsigned i=1; i != _cur._num_threads; i++){
          t.push_back(i+1);
        }
      
        return t;
      }

      // We return true for a thread, even if its corresponding core is inactive,
      // since threads are mapped to cores and thus
      void remoteGetThreadStatus(int threadNo)
      {
        if (threadNo >= 0) {
          if (getTrgFromThreadNo(threadNo)) {
            makeGdbPacket("OK");
            return;
          }
        }
        makeGdbPacket("E00");
      }

      GdbTarget *getTrgFromThreadNo(int threadNo)
      {
        Threadmap::iterator iterT = _cur._thread_map.find(threadNo);
        if (iterT != _cur._thread_map.end()) {
          CoreMap::iterator iterC =  _parent._coreMap.find(iterT->second);
          if (iterC == _parent._coreMap.end()) {
            RError("Bad thread number:  " << threadNo);
          } else {
            return &iterC->second;
          }
        }
        return 0;
      }

      void remoteGetThreadExtraInfo(int threadNo)
      {
        if (threadNo >= 0) {
          // For now, we'll just return the full path of the thread, as some
          // helpful information.
          if (GdbTarget *trg = getTrgFromThreadNo(threadNo)) {
            string path = Data2ASCIIhex(getPathName(trg->node(),":"));
            putGdbPacket(path);
            return;
          }
        }

        // Nothing known, so return an empty packet.
        putGdbPacket(string());
      }
      
      // Handle a gdb 'monitor <args>' command.  This should assign to the result
      // string the data to be sent, which may then be broken into multiple packets,
      // as necessary.
      // KEY_FUNC: remoteHandleCommand
      string remoteHandleCommand(const string &command) 
      {
        VPrint("Command to be executed: '" << command << "'.");

        string result; 
        string connect_str = "wconnect";      
        auto ct = cur_core();

        if (command == "exit") {
          // Directly respond with an OK, then shutdown and exit.
          makeGdbPacket("OK");
          serverFinish();
          return string();
        }
        if (command == "timestamp") {
          ostringstream ss;
          ss << to_string(ct->node()->get_td_counter());
          
          return ss.str();
        }
        // else if (command == "wsetup") { // set up waveform thread
          // ostringstream ss;
          // if (_parent.has_waveform) {
            // setupWaveform();
          // }
          // return ss.str();          
        // }
        // else if (command == "wconnect") { // connect to waveform
        else if (command.substr(0, connect_str.size()) == "wconnect") { // connect to waveform using timestamp
          ostringstream ss;
          std::string timestamp = command.substr(connect_str.size()); // get timestamp
          if (_parent.has_waveform) {
            connectWaveform(timestamp);
          }
          return ss.str();
        }
        else if (command == "wcheck") { // for constantly inquiry waveform cursor 
          ostringstream ss; 
          if (_parent.has_waveform){
            // this arrangement causes reg info updates next cycle, but is safe
            uint32_t time = ct->node()->get_td_counter(); 
            unsigned node_no = 0;
            for (unsigned i=0; i<_parent._portMap.size(); i++)
            {
              if (_parent._portMap.at(i) == ct) {
                node_no = i;
                break;
              }
            }
              
            checkWaveformTime(node_no, time);
          }          
          
          return ss.str();
        }
        else if (command == "reg") { // for constantly inquiry registers
          ostringstream ss;                          
          int regs = ct->num_regs();
          ss << noshowbase << hex;
          
          for (int i = 0; i < regs; i++) {
            writeMonRegValue(ss,ct,i);
          }
          ss << "end";          
          
          return ss.str();
          
        }
        else if (command == "trace-run-exit") {
          // if we exit in the trace-run mode, although we are in extended-remote mode,
          // we still want to use the kill handler for remote mode
          if (_parent.has_waveform) {
            remoteKill();    
          }
          serverFinish();
        }
       
        _parent._theMgr.eval(result,command);

        // Node activity may have been affected by the monitor command, so we
        // check to see what's halted and if thread switches occurred.
        _parent.check_temp_halt();
        updateActiveNode();

        return result;
      }

      // Right now, this is a global reset, even if we're in an MP situation.
      // This isn't really what everybody really wants, since a user would
      // probably like to reset a single core.  Unfortunately, we don't have a way
      // right now to read input wrt. just a single core.  We'd have to add that
      // functionality first, then modify this routine to only reset _cur.
      //
      // Note:  This command does not send a reply.
      void remoteRestart()
      {
        scoped_lock sl(_parent._io_mutex);

        resetSimulation(_parent._rootnode,&_runctrl);
        processAllInput(_parent._rootnode);
        _cur.reset_temp_halt();
        _cur.set_inactive();
        _finished = false;

        makeGdbPacket("OK");
      }

      //
      // Note: Currently, GDB breakpoints are *not* thread specific at the
      // remote-protocol level.
      //

      // remote_add_hardware_breakpoint: add hardware break point 
      void remoteAddHardwareBreakpoint(addr_t address,unsigned length_in_bytes) 
      {
        ForEach(_cur._cores,i) {
          (*i)->node()->setBreakpoint(address);
        }
        makeGdbPacket("OK");
      }

      // remote_add_software_breakpoint: add software break point 
      void remoteAddSoftwareBreakpoint(addr_t address,unsigned length_in_bytes) 
      {
        ForEach(_cur._cores,i) {
          (*i)->node()->setBreakpoint(address);
        }
        makeGdbPacket("OK");
      }

      // remote_add_read_watchpoint: add read watch point 
      void remoteAddReadWatchpoint(addr_t address,unsigned length_in_bytes) 
      {
        ForEach(_cur._cores,i) {
          (*i)->node()->setWatchpoint(address,wpRead);
        }
        makeGdbPacket("OK");
      }

      // remote_add_write_watchpoint: add write watch point 
      void remoteAddWriteWatchpoint(addr_t address,unsigned length_in_bytes) 
      {
        ForEach(_cur._cores,i) {
          (*i)->node()->setWatchpoint(address,wpWrite);
        }
        makeGdbPacket("OK");
      }

      // remote_add_access_watchpoint: remove access watch point 
      void remoteAddAccessWatchpoint(addr_t address,unsigned length_in_bytes) 
      {
        ForEach(_cur._cores,i) {
          (*i)->node()->setWatchpoint(address,wpAccess);
        }
        makeGdbPacket("OK");
      }

      // remote_remove_hardware_breakpoint: remove hardware break point 
      void remoteRemoveHardwareBreakpoint(addr_t address,unsigned length_in_bytes) 
      {
        ForEach(_cur._cores,i) {
          (*i)->node()->clearBreakpointByAddr(address);
        }
        makeGdbPacket("OK");
      }

      // remote_remove_software_breakpoint: add software break point 
      void remoteRemoveSoftwareBreakpoint(addr_t address,unsigned length_in_bytes) 
      {
        ForEach(_cur._cores,i) {
          (*i)->node()->clearBreakpointByAddr(address);
        }
        makeGdbPacket("OK");
      }

      // remote_remove_read_watchpoint: remove read watch point 
      void remoteRemoveReadWatchpoint(addr_t address,unsigned length_in_bytes) 
      {
        ForEach(_cur._cores,i) {
          (*i)->node()->clearWatchpointByAddr(address,wpRead);
        }
        makeGdbPacket("OK");
      }

      // remote_remove_write_watchpoint: remove write watch point 
      void remoteRemoveWriteWatchpoint(addr_t address,unsigned length_in_bytes) 
      {
        ForEach(_cur._cores,i) {
          (*i)->node()->clearWatchpointByAddr(address,wpWrite);
        }
        makeGdbPacket("OK");
      }

      // remote_remove_access_watchpoint: remove access watch point 
      void remoteRemoveAccessWatchpoint(addr_t address,unsigned length_in_bytes) 
      {
        ForEach(_cur._cores,i) {
          (*i)->node()->clearWatchpointByAddr(address,wpAccess);
        }
        makeGdbPacket("OK");
      }
      
      // waveform time change
      // KEY_FUNC: remoteHandleWaveformTimeChange
      void remoteHandleWaveformTimeChange(uint32_t time)
      {
        auto ct = cur_core();
        uint32_t cur_time = ct->node()->get_td_counter();
        
        if ((int)time - (int)cur_time > 0){
          // run forward to time
          _runctrl.runUntilTime(*ct->node(), time);
        }
        else if ((int)time - (int)cur_time < 0) {
          // run backward to time
          _runctrl.rRunUntilTime(*ct->node(), time);
        }

        cur_time = ct->node()->get_td_counter();
        setWaveformTime(cur_time);        
      }

      // Sends a response if an interrupt occurred.
      void makeInterruptResponse()
      {
        if (_interrupt) {
          makeGdbPacket("S02");
          _interrupt = false;
        }
      }

      string nonStopId()
      {
        if (nonStopMode()) {
          ostringstream ss;
          ss << "thread:" << _breakpoint;
          return ss.str();
        } else {
          return string();
        }
      }

      // Make and put a response packet for when a watchpoint is hit.  Returns
      // true if we send a response, false if we don't have a watchpoint.
      string makeWatchFinishResponse()
      {
        ostringstream ss;
        switch (_parent._breakpointLogger.type()) {
        case wpRead :
          ss << "T05rwatch:" << hex << _parent._breakpointLogger.address() << nonStopId() << ";";
          break;
        case wpWrite :
          ss << "T05watch:" << hex << _parent._breakpointLogger.address() << nonStopId() << ";";
          break;
        case wpAccess :
          ss << "T05awatch:" << hex << _parent._breakpointLogger.address() << nonStopId() << ";";
          break;
        default:
          ;
        }
        return ss.str();
      }

      void makeAllStopBreakpointResponse()
      {
        string wfr = makeWatchFinishResponse();
        if (!wfr.empty()) {
          putGdbPacket(wfr);
        } else {
          // Otherwise, we're at a breakpoint, so send back the relevant
          // response.
          remoteStepFinishResponse();
        }
      }

      void makeNonStopBreakpointResponse()
      {
        string wfr = makeWatchFinishResponse();
        if (!wfr.empty()) {
          _stop_seq.push_back(wfr);
        } else {
          // Otherwise, we have a breakpoint, so signal that fact.
          addThreadStop(_breakpoint);
        }
      }

      // In non-stop mode, we only de-activate the core which had the
      // breakpoint.  In all-stop mode, we stop all cores.
      void handleBreakpoint()
      {
        if (nonStopMode()) {
          auto trg = _cur.getCoreFromThreadId(_breakpoint);
          trg->set_inactive();
          trg->_cont = false;
        } else {
          _cur.set_inactive();
        }
      }

      // Report on any threads which were active as a result of a continue, but
      // have now stopped.
      void makeNonStopContinueResponse()
      {
        ForEach(_cur._cores,i) {
          GdbTarget &trg = **i;
          if (trg._cont && trg.is_done()) {
            addThreadStop(trg._thread_id);
            trg._cont = false;
          }
        }
      }

      // Sends an appropriate finish response if the node has reached a stopping
      // point (interrupt, breakpoint, halted).  Otherwise, sends back nothing.
      // If we do send back a response, then we have to cancel the outstanding
      // read request so that the protocol doesn't get confused.  
      //
      // TODO: Encapsulate this inside of GdbServer.
      void makeFinishResponse()
      {
        if (_breakpoint) {
          handleBreakpoint();
          cancelReads();
          if (nonStopMode()) {
            makeNonStopBreakpointResponse();
          } else {
            makeAllStopBreakpointResponse();
          }
          _breakpoint = 0;
        } else if (nonStopMode()) {
          // In non-stop mode, report on any threads which have just finished.
          makeNonStopContinueResponse();
        } else if (_cur.is_done()) {
          _finished = true;
          _cur.set_inactive();
          cancelReads();
          remoteStepFinishResponse();
        }

        // If in non-stop mode, restore activity so that we'll continue executing.
        if (nonStopMode()) {
          setupForContinue();
        }

        remoteAsyncStop();
      }


    };

    // Create a new server node.  Use the index to match up with the
    // corresponding target object.
    RemoteGdbServerNode *createNode(unsigned index,bool verbose)
    {
      assert(index < _portMap.size());
      _rnodes.push_back(new RemoteGdbServerNode(*this,verbose,*_portMap[index]));
      return _rnodes.back();
    }

    // This is called when new I/O comes in and the system is running (in the
    // middle of the long-running remoteContinue operation).  We record who was
    // performing the continue and de-activate all cores in order to temporarily
    // end the simulation.
    void ioHalt() 
    {
      scoped_lock sl(_io_mutex);

      if (!_io_halted && _running) {
        DPrint("Setting inactive from I/O.");
        print_activity("Inactive");
        _io_halted = true;
        _root->set_io_inactive();
      }
    }

    // Immediately after returning from an interrupted simulation, we restore
    // everything.
    void ioRestore()
    {
      scoped_lock sl(_io_mutex);

      if (_io_halted) {
        DPrint("Restoring activity after I/O.");
        _root->restore_from_io();
        print_activity("Restored");
        _io_halted = false;
      }
    }

    bool haveSimable()
    {
      scoped_lock sl(_io_mutex);

      ForEach(_coreMap,i) {
        if (i->second.is_core()) {
          if (i->second.node()->get_active_count()) return true;
        }
      }
      return false;
    }

    void makeInterruptResponses()
    {
      ForEach(_rnodes,i) {
        (*i)->makeInterruptResponse();
      }
    }

    void makeFinishResponses()
    {
      scoped_lock sl(_io_mutex);

      print_activity("Finish");
      ForEach(_rnodes,i) {
        (*i)->makeFinishResponse();
      }
    }

    void print_activity(const char *msg)
    {
      ForEach(_coreMap,i) {
        if (i->second.is_core()) {
          DPrint(msg << ":  " << i->second.node()->name() << ":  " << i->second.node()->get_active_count());
        }
      }
    }

    void remoteContinue()
    {     
      // If we had an interrupt, then we respond here, since we will have
      // handled the remoteInterrupt already and now we're resuming after I/O.
      makeInterruptResponses();

      // Don't do anything if we don't have any active cores.
      if (!haveSimable()) return;

      {
        scoped_lock sl(_io_mutex);        
        DPrint("Entering simulator.");
        save_td_threshold();
        print_activity("Start");
      }
      _running = true;
      _runctrl.run(_rootnode,simMode());
      _running = false;
      restore_td_threshold();

      // If we were interrupted by I/O, restore any active state.
      ioRestore();

      // Respond for any cores which stopped.
      makeFinishResponses();
    }

    // Record the fact that a breakpoint has occured.
    void recordBreakpoint(IssCore &core)
    {
      ForEach(_rnodes,i) {
        RemoteGdbServerNode &node = **i;
        node.setBreakpoint(&core);
      }
    }

    typedef vector<RemoteGdbServerNode*> RemoteNodes;

    IssNode               &_rootnode;         // Stores the node passed from DbgInterface.
    RunControlIface       &_runctrl;          // Abstraction used for stepping and
                                              // running the model.
    CoreMap                _coreMap;          // Map of node name and core pointer.
    GdbTarget             *_root;             // Root target.
    PortMap                _portMap;          // Maps port indices to target nodes.
    GdbBrkParent           _breakpointLogger;
    RemoteNodes            _rnodes;           // Our own list of node objects.
    static ScriptingIface& _theMgr;
    
    bool                   _running, _io_halted;
    bool                   _use_ext_mode;     // True => GDB should use extended mode.

    boost::mutex           _io_mutex;         // Mutex used when handling multiplexed I/O.
  };

  // If we have a payload, and it's of type GdbPt, then we assume that this is a
  // temporary breakpoint set by a watchpoint.
  void GdbBreakpoint::logBreakpoint(addr_t ea,unsigned handle,PtBase *payload) 
  {
    DPrint ("Logging breakpoint at 0x" << hex << ea << " on " << _core.name());
    _parent._cur = this;
    _parent._server.recordBreakpoint(_core);

    // Record the current thread context, if relevant.
    if (auto ctx = getThreadContext(_core)) {
      _ctx_id  = ctx->_id;
      _ctx_num = _core.getActiveCtx(_ctx_id);
    } else {
      _ctx_id  = -1;
      _ctx_num = -1;
    }

    if (GdbPt *p = dynamic_cast<GdbPt*>(payload)) {
      // Temporary breakpoint for handling a watchpoint.
      _address = p->_address;
      _type = p->_type;
    } else {
      // Normal breakpoint.
      _address = ea;
      _type = wpNone;
    }
    throw SimInterrupt();
  }

  // On a watchpoint, we cannot interrupt the instruction, so we set a
  // breakpoint at the CIA, then remove it when we hit that and that's where we
  // signal the watchpoint.
  void GdbBreakpoint::logWatchpoint(addr_t ea,bool pre,WatchType type,int num_bytes,
                                    uint64_t value,unsigned handle,PtBase *) 
  {
    // Ignore if we've already handled this one (in the pre-phase).
    if (pre) {
      auto pt = new GdbPt(ea,type); 
      _core.setTmpBreakpoint(_core.getProgramCounter(),pt);
    }
  }


  ScriptingIface& RemoteGdbServer::_theMgr = scripter();

  // KEY_FUNC: checkForGdbMode
  DbgInterface *checkForGdbMode(IssNode &root,RunControlIface &runctrl)
  {
    if (options().getFlag("gdb") || options().getFlag("gdb-udp")) {
      string port = options().getOption("port",string());

      string port_file = options().getOption("port-file");

      string hostname = options().getOption("gdb-hostname");

      bool use_udp = options().getFlag("gdb-udp",false);

      bool start_halted = options().getFlag("gdb-halt",true);

      bool extended_mode = options().getFlag("gdb-ext-mode",true);

      GdbTransportType type = (use_udp) ? GdbUdp : GdbTcp;
      
      // check if there is any waveform file
      
      bool has_waveform = false;      
      string waveform_file = options().getOption("w-file");      
      string waveform_port = options().getOption("w-port");      
      string waveform_unit = options().getOption("w-unit");      
      if (!waveform_file.empty()) {
        has_waveform = true;
        WaveformThread* waveform = new WaveformThread(waveform_port, waveform_file, waveform_unit); // for now only support one waveform for all gdb remote server node
        RemoteGdbServer* remote_gdb_server = new RemoteGdbServer(root,runctrl,hostname,port,port_file,type,VerboseMode,start_halted,extended_mode,has_waveform);
        remote_gdb_server->assignWaveform(waveform); // assigned waveform pointer to GdbServer
        return remote_gdb_server;
      }
      else {
        return new RemoteGdbServer(root,runctrl,hostname,port,port_file,type,VerboseMode,start_halted,extended_mode); 
      }
    } else {
      return 0;
    }
  }

}

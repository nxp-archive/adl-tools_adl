//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Implements the GDB remote protocol.  Derive from this class and implement
// the various virtual functions in order to get this to work for your
// particular ISS.
//
// NOTE:  The GdbServer class SHOULD NOT contain any ISS specific code.  All
//        ISS specific stuff should be implemented by derived classes.  It
//        should be possible to take this class and use it for any ISS w/o
//        modification to the GdbServer class itself
//

#ifndef _GDBSERVER_H_
#define _GDBSERVER_H_

#include <string>
#include <vector>
#include <stdexcept>
#include <stdarg.h>

#include <boost/asio.hpp>
#include <boost/thread/mutex.hpp>

#include "helpers/BasicTypes.h"
#include "WaveformThread.h"

// Define this to get more debug information than just using verbose-mode.
//#define DEBUG

#ifdef DPrint
#  undef DPrint
#endif

#ifdef DEBUG
#  define DPrint(x) { std::cerr << x << '\n'; }
#else
#  define DPrint(x)
#endif

#undef VPrint
#define VPrint(x) { boost::mutex::scoped_lock m(gdb_verbose_mutex); if (verbose()) { cerr << x << '\n'; } }
#define VTPrint(x) { boost::mutex::scoped_lock m(gdb_verbose_mutex); if (_server.verbose()) { cerr << x << '\n'; } }

namespace adl {
  
  class WaveformThread;

  extern boost::mutex gdb_verbose_mutex;

  // This describes the transport type used for communicating with the GDB
  // client.
  enum GdbTransportType {
    GdbTcp,
    GdbUdp
  };

  enum GdbProgramStatus {
    GDB_ACTIVE,
    GDB_BREAK,
    GDB_WATCH,
    GDB_HALT  
  };

  enum {
    GdbServerMinPacketSize = 5024,
    GdbServerDefaultPort   = 0,
  };

  std::string ASCIIhex2Data(const std::string& s);
  std::string Data2ASCIIhex(const std::string& s);
  void toLowerPacket(char *packet);

  class GdbCommThread;
  class GdbServer;

  // This represents a single GDB connection.  So, for example, if two separate
  // debuggers are connected, then there will be two GdbServerNode objects.  If
  // only one gdb is connected, even if this is pointing to a system of cores,
  // e.g. an SMT design, then there will be one GdbServerNode object.
  class GdbServerNode {
  public:
    typedef std::map<int,char> CommandList;
    typedef std::vector<int> ThreadIdList;

    GdbServerNode(GdbServer &server,bool verbose);
    virtual ~GdbServerNode() {};

    void setCommThread(GdbCommThread *);
    
    void setWaveformThread(WaveformThread *);

    bool verbose() const { return _verbose; };

    bool extended_mode() const { return _extended_mode; };

    // For debugging purposes- return a useful name for this node.
    virtual const std::string &name() const = 0;

    // Do a join on the comm thread.
    void join();

    // Tell the comm thread to quit if inactive.
    void finishIfInactive();

    // Cancel a command read, if one is currently pending, so that we can send a
    // response to the gdb client and not have it interfere.
    void cancelReads();

    // Signals that IO is coming in from a comm thread and the main service
    // routine must temporarily halt in order to service it.  Only needed when
    // supporting multiple simultaneous gdb instances.
    virtual void remoteIOHalt() {};
    virtual void remoteIOResume() {};

    // Signals an interrupt of a running program.
    virtual void remoteInterrupt() = 0;

    // Setup the current core/thread to continue, possibly at a new address.
    virtual void remoteSetupContinue(bool valid_addr,addr_t new_addr) = 0;

    // Step the current node.
    //
    // NOTE: This should not send a response, since it is sometimes called
    // individually (as part of an s command) and sometimes called as part of a
    // larger vCont command.  The response is sent by
    // remoteStepFinishResponse().
    virtual void remoteStep() = 0;
    
    // Reverse step the current node.
    virtual void remoteRStep() = 0;

    // Reverse run the current node.
    virtual void remoteRContinue() = 0;
    
    // Send a response after a step or vCont command.  Return true if there was an explicit thread switch.
    virtual bool remoteStepFinishResponse() = 0;
    
    // Send a response after a reverse step command.
    virtual void remoteRStepFinishResponse() = 0;
    
    // Send a response after a reverse run command.
    virtual void remoteRContinueFinishResponse() = 0;

    // Should return true if there's anything active to
    // simulate.
    virtual bool remoteSimable() const = 0;
    
    // remote_get_memory: Get memory values.
    //
    // Format of data is a packed character string with 2 hex digits per byte
    // for example, a command to get 8 bytes of memory contained 0xabababab
    // and 0xdeadbeef would be return with this data: "ABABABABDEADBEEF"
    //
    // If direct is true, then the access should be made directly to memory,
    // bypassing all translation and caches.
    //
    // When done, call putGdbPacket to initiate the response.  A size of 0
    // causes an error message to be sent.
    virtual void remoteGetMemory(addr_t start_address,
                                 unsigned long length_in_bytes,
                                 bool direct) = 0;
    // remote_set_memory: set memory values 
    // format of data is a packed character string with 2 hex digits per byte       
    // for example, a command to set 8 bytes of memory to 0xabababab and 0xdeadbeef 
    // would be supplied with this data: "ABABABABDEADBEEF"                         
    // If direct is true, then the access should be made directly to memory, bypassing
    // all translation and caches.
    virtual void remoteSetMemory(addr_t start_address,
                                unsigned long length_in_bytes,
                                char *data,
                                bool direct) = 0;

    // remote_set_memory_binary: set memory values 
    // format of data is a packed character string with binary data                 
    virtual void remoteSetMemoryBinary(addr_t start_address,
                                      unsigned long length_in_bytes,
                                      char* data) = 0;

    // remote_get_registers: Get all register values.
    //
    // Format of data is a packed character string with 2 hex digits per byte
    // per register.  For example, a processor with two registers with
    // 0xabababab and 0xdeadbeef would return this data: "ABABABABDEADBEEF"
    virtual void remoteGetRegisters() = 0;

    // Set all registers. Returns OK or E00 on error.
    virtual void remoteSetRegisters(const char *data) = 0;

    // Get/set a specific register.
    virtual void remoteGetRegister(unsigned reg) = 0;
    virtual void remoteSetRegister(unsigned reg,const std::string &val) = 0;

    // Set the thread number for subsequent commands.
    virtual void remoteSetThread(int threadNo,bool reply) = 0;

    // Simpler direct version used by vCont command.
    virtual void remoteSetThreadDirect(int threadNo) = 0;

    // Handle a kill command.
    virtual void remoteKill() = 0;

    // Should return true if extended-mode is supported.  This should not send a response.
    virtual bool remoteExtendedSupport() const = 0;

    // Return the current active thread id.
    virtual int remoteGetActiveThreadId() = 0;

    // Return thread id of a thread ready to simulate.
    virtual int remoteGetSimableThreadId() = 0;

    // Return a string containing thread information (the m packet).  The string is sent back as-is, but this 
    // information is also used by the 
    virtual ThreadIdList remoteGetThreadInfo() = 0;

    // Return additional information about a thread, given a thread id.
    virtual void remoteGetThreadExtraInfo(int threadNo) = 0;

    // Send info on thread status.
    virtual void remoteGetThreadStatus(int threadNo) = 0;

    // This is for handling GDB 'monitor' commands.  The result text should be
    // placed into 'result', where it will be sent to GDB and broken up into
    // packets as necessary.
    virtual std::string remoteHandleCommand(const std::string &command) = 0;

    // Reset and restart the simulation.
    virtual void remoteRestart() {};

    // Send info about the last signal received.
    virtual void remoteGetLastSignal() {};

    // Non-stop mode support.
    virtual bool remoteNonStopSupported() const { return false; };
    virtual void remoteNonStopMode(bool entering) = 0;
    
    // KEY_FUNC: reverse execution support
    virtual bool remoteReverseExecSupported() { return false; };
    
    // Do not reply in this function, since we always reply with OK for any vCont command in non-stop mode.
    virtual void remoteStopThread() {};   

    // Post an asynchronous message listing any stopped threads.
    virtual void remoteAsyncStop() {};

    // This is only ever called as a result of a vStopped command and thus
    // simply continues a sequence.
    virtual void remoteSendNextStopSequence() {};

    // Addition of break/watchpoints.
    virtual void remoteAddHardwareBreakpoint(addr_t  address,unsigned length_in_bytes) = 0;
    virtual void remoteAddSoftwareBreakpoint(addr_t address,unsigned length_in_bytes) = 0;
    virtual void remoteAddReadWatchpoint(addr_t address,unsigned length_in_bytes) = 0;
    virtual void remoteAddWriteWatchpoint(addr_t address,unsigned length_in_bytes) = 0;
    virtual void remoteAddAccessWatchpoint(addr_t address,unsigned length_in_bytes) = 0;

    // Removal of break/watchpoints.
    virtual void remoteRemoveHardwareBreakpoint(addr_t address,unsigned length_in_bytes) = 0;
    virtual void remoteRemoveSoftwareBreakpoint(addr_t address,unsigned length_in_bytes) = 0;
    virtual void remoteRemoveReadWatchpoint(addr_t address,unsigned length_in_bytes) = 0;
    virtual void remoteRemoveWriteWatchpoint(addr_t address,unsigned length_in_bytes) = 0;
    virtual void remoteRemoveAccessWatchpoint(addr_t address,unsigned length_in_bytes) = 0;
    
    // waveform relevant
    virtual void remoteHandleWaveformTimeChange(uint32_t time) = 0;

  protected:
    friend class GdbCommThread;
    friend class WaveformThread;

    // Causes us to shut down the I/O event loops.
    void serverFinish();

    // Causes us to expect another command from gdb without sending a response.
    void noResponse();

    // General response function for use by derived classes.  This posts a write
    // to the client.  If is_response is false, then we don't expect a new
    // command from gdb, e.g. console output messages.
    void putGdbPacket(const std::string &out,bool is_response = true);

    // General asynchronous response function.
    void putGdbAsyncPacket(const std::string &out,bool is_response = true);

    // Same as above, but create a packet with the specified args.  This is
    // assumed to be a response.
    void makeGdbPacket(const char *fmt, ... );
    
    // waveform relevant methods (if any)
    void setWaveformTime(uint32_t time);
    void checkWaveformTime(unsigned node_no, uint32_t time);
    void setupWaveform();
    void connectWaveform(std::string timestamp);
    
    void getMemory(char *data,bool direct);
    void setMemory(char *data,bool direct) ;
    void setMemoryBinary(char *data);

    void getRegisters();
    void setRegisters(char *data);

    void getRegister(char *data);
    void setRegister(char *data);

    void setThread(char *data);

    void handleContinue(char *data);
    
    void handleKill(bool sendOk,bool reallyDoIt);

    void handleQuery(char *data);

    void handleQCmd(char *data);

    void handleRemoteCommand(char *data);

    void handleRestart();
    // KEY_FUNC: handleReverseExecution in .h
    void handleReverseExecution(char *data);

    void handleSupportQuery(char *data);
    
    void handleStep();

    void getThreadStatus(char *data);

    void handleVCmd(char *data);

    void handleVContCommand(char *data);
    
    void handleWaveformTimeChange(uint32_t time);

    void parseVContPacket(CommandList &commandList,char *data);

    void addBreakOrWatch(char *data);

    void removeBreakOrWatch(char *data);

    void setExtendedMode();

    bool nonStopMode() const{ return _non_stop; };
    void setNonStopMode(bool ns) { _non_stop = ns; };
    
    WaveformThread* waveform_ptr()
    {
      return _waveform_thread;
    }

  private:
    GdbServer                                   &_server;
    bool                                         _verbose;      // General verbosity control.
    std::auto_ptr<GdbCommThread>                 _comm_thread;  // The thread running the comm service.
    WaveformThread*                              _waveform_thread;  // The thread running the waveform service.
    bool                                         _extended_mode;// True if we're in gdb extended-mode.
    bool                                         _non_stop;     // True if in non-stop mode.
  };

  // This is the main routine for implementing the GDB server.  Derive from this
  // in order to add ISS implementation-specific details.  This is the overall
  // master which spawns off comm threads and creates GdbServerNode objects for
  // handling the gdb traffic.
  class GdbServer {
  public:

    GdbServer(GdbTransportType type,const std::string &hostname,bool verbose,bool has_waveform);
    virtual ~GdbServer();

    bool verbose() const { return _verbose; };

    // Write out a port file, based upon the type of comm used, port selected, etc.
    void writePortFile(unsigned port,const std::string &port_file);

    // Main entry point:  When this returns, it means that the debugger has told the
    // simulator to exit.
    void rundebugger();
    
    void assignWaveform(WaveformThread *waveform)
    {
      _waveform = waveform;
    }

    unsigned maxPacketSize() const { return _max_packet_size; };

    // Continue the simulation, optionally at a new address.
    //
    // NOTE: The server starts to listen for a new command at this point, in
    // order to catch an interrupt.  So, before sending a response, remember to
    // call cancelReads() so that the pending asynchronous read doesn't
    // interfere with the packet send/ack process.
    virtual void remoteContinue() = 0;
    
    // // KEY_FUNC: // bool reverseSupport
    // bool reverseSupport;

  protected:
    friend class GdbCommThread;
    friend class WaveformThread;

    struct NodeElm {
      unsigned       _port;
      std::string    _port_file;
      GdbServerNode *_node;

      NodeElm (unsigned p,const std::string &pf) : _port(p), _port_file(pf), _node(0) {};
    };
    typedef std::vector<NodeElm> ServerNodes;
    
    bool has_waveform;

    // Generic function used by the comm thread to post transactions to the main thread.
    template <typename Handler>
    void post(const Handler&h)
    {
      _sim_service.post(h);
    }
        
    WaveformThread* getWaveformPtr()
    {
      return _waveform;
    }

    // A comm thread calls this to indicate that it is now active.
    void commThreadActive();

    // Decrements the number of active comms threads.  When the count reaches 0,
    // the event loop in this main thread is finished.
    void commThreadFinished();

    GdbCommThread *createCommThread(GdbServerNode &node,unsigned port,const std::string &port_file);

    // Overload this to create a factory for creating per-connection node objects.
    virtual GdbServerNode *createNode(unsigned index,bool verbose) = 0;

    // Overload this to specify whether we should enable extended mode by
    // default in the generated port file.
    virtual bool remoteExtendedSupport() const { return true; };

    // The derived class should call this to add a port/port-file pair for each
    // top-node (each unique connection which will be supported).  At minimum,
    // you need one of these.
    void addPortData(unsigned port,const std::string &port_file);

    // Call this to specify the max packet size needed.
    void setMaxPacketSize(unsigned);

  private:
    GdbTransportType                             _type;         // Communication type to use.
    bool                                         _verbose;      // General verbosity control.
    const std::string                            _hostname;     // Hostname override when creating port file.
    boost::asio::io_service                      _sim_service;  // Event loop for controlling the simulator.
    std::auto_ptr<boost::asio::io_service::work> _sim_work;     // Controls the _sim_service object.
    unsigned                                     _max_packet_size; // Max size of packet we'll send.
    ServerNodes                                  _nodes;        // An entry for each top-node (unique connection).
    unsigned                                     _active_comms; // A count of the active number of comm threads.
    WaveformThread*                              _waveform;     // waveform instance pointer
  };

}

#endif

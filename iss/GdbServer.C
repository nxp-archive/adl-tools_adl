//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//


#include <iostream>
#include <fstream>

#include <boost/thread.hpp>
#include <boost/bind.hpp>

#include "helpers/Macros.h"

#include "StringHelpers.h"

#include "GdbServer.h"

using namespace std;
using namespace boost;
using namespace boost::asio;

namespace adl {

  boost::mutex gdb_verbose_mutex;

  static const char g_hex_chars[] =           // Quick ascii to hex conversion.
    "0123456789abcdef";

  // Thread numbers are always specified in hex, except for the literal -1.
  int getThreadNum(const char *str)
  {
    if (strlen(str) == 2 && str[0] == '-' && str[1] == '1') {
      return -1;
    } else {
      return strtol(str,0,16);
    }
  }
  


  // A packet is introduced with the character `$', the actual packet-data, and
  // the terminating character `#' followed by a two-digit checksum:
  //
  //    $packet-data#checksum
  //
  // The two-digit checksum is computed as the modulo 256 sum of all characters
  // between the leading `$' and the trailing `#' (an eight bit unsigned
  // checksum).
  //
  // We assume that out contains space for the input contents plus four extra
  // characters for the leading packet character plus separator and checksum.
  void constructGdbPacket(char *out,const char *in,unsigned len,bool is_async)
  {
    *out++ = (is_async) ? '%' : '$';

    unsigned char checksum = 0;
    for (unsigned i = 0; i != len; ++i) {
      *out++ = in[i];
      checksum += in[i];
    }
    *out++ = '#';
    *out++ = g_hex_chars[checksum >> 4];
    *out++ = g_hex_chars[checksum & 0xf];
  }

  // A reference-counted non-modifiable buffer class (from the Boost::asio
  // documentation).  This can create a valid gdb packet upon creation, avoiding
  // an extra memory copy if 'raw' is false (default).  If raw is true, then we
  // just use the data as-is.
  class SharedConstBuffer
  {
  public:
    // Construct from a std::string.
    explicit SharedConstBuffer(const string& data,bool is_async = false, bool raw = false) : 
      _data((raw) ? makeRawPacket(data.c_str(),data.size()) : makeGdbPacket(data.c_str(),data.size(),is_async)),
      _buffer(asio::buffer(*_data))
    {}

    // Construct from a raw char buf.
    explicit SharedConstBuffer(const char *data,unsigned len,bool is_async = false, bool raw = false) : 
      _data((raw) ? makeRawPacket(data,len) : makeGdbPacket(data,len,is_async)),
      _buffer(asio::buffer(*_data))
    {}

    // Implement the ConstBufferSequence requirements.
    typedef boost::asio::const_buffer value_type;
    typedef const boost::asio::const_buffer* const_iterator;
    const boost::asio::const_buffer* begin() const { return &_buffer; }
    const boost::asio::const_buffer* end() const { return &_buffer + 1; }

  private:
    boost::shared_ptr<std::vector<char> > makeRawPacket(const char *buf,unsigned len)
    {
      return boost::shared_ptr<std::vector<char> >(new std::vector<char>(&buf[0],&buf[len]));
    }

    boost::shared_ptr<std::vector<char> > makeGdbPacket(const char *buf,unsigned len,bool is_async)
    {
      boost::shared_ptr<std::vector<char> > nbuf(new std::vector<char>(len+4));
      constructGdbPacket(&(*nbuf)[0],buf,len,is_async);
      return nbuf;
    }

    boost::shared_ptr<std::vector<char> > _data;
    boost::asio::const_buffer _buffer;
  };


  //------
  // crc32
  //------
  unsigned long crc32 (unsigned char *buf, int len, unsigned int crc) 
  {
    /* Table used by the crc32 function to calcuate the checksum. */
    static unsigned long crc32_table[256] = {0, 0};
  
    if (!crc32_table[1]) {
      /* Initialize the CRC table and the decoding table. */
      int i, j;
      unsigned int c;
    
      for (i = 0; i < 256; i++) {
        for (c = i << 24, j = 8; j > 0; --j)
          c = c & 0x80000000 ? (c << 1) ^ 0x04c11db7 : (c << 1);
        crc32_table[i] = c;
      }
    }

    while (len--) {
      crc = (crc << 8) ^ crc32_table[((crc >> 24) ^ *buf) & 255];
      buf++;
    }
    return crc;
  }

  //--------
  // hexChar
  //--------
  int hexChar(char ch) 
  {
    if (ch >= 'a' && ch <= 'f')
      return ch - 'a' + 10;
    if (ch >= '0' && ch <= '9')
      return ch - '0';
    if (ch >= 'A' && ch <= 'F')
      return ch - 'A' + 10;
    return -1;
  }

  addr_t hexStr(const char* ptr) 
  {
    char *err;
    addr_t returnValue = strtoull(ptr,&err,16);
    return returnValue;
  }

  void toLowerPacket(char *packet)
  {
    /* gdb barfs on upper case hex digits! */
    for (int idx = 0; packet[idx]; ++idx) {
      packet[idx] = tolower(packet[idx]);
    }
  }

  // ASCIIhex2Data : A helper function for ASCII Hex to Data
  string ASCIIhex2Data(const string &S) 
  {
    string Data;
    Data.reserve(S.size());
    for(int i=0; i<int(S.length()); i=i+2) {
      char hi=S[i];
      hi-=(hi<'A' ? '0' : 'A'-10);
      char lo=S[i+1];
      lo-=(lo<'A' ? '0' : 'A'-10);
      Data += (hi<<4) | (lo & 0x0F);// " & 0x0F" deals with lower-case characters

    }
    return Data;
  }

  // Data2ASCIIhex : A helper function for Data to ASCII Hex
  string Data2ASCIIhex(const string &S) 
  {
    string Data;
    Data.reserve(S.size());
    const char* src=S.c_str();
    for(int i=S.length(); i--;) {
      unsigned char lo=*src++;
      unsigned char hi=lo>>4;
      lo&=0x0F;
      Data +=hi+(hi>9 ? 'A'-10 : '0');
      Data +=lo+(lo>9 ? 'A'-10 : '0');
    }
    return Data;
  }



  // This object is used internally and represents a thread designed to handle
  // communication traffic.  It runs its own asio io_service and posts events to
  // the main thread for communicating with the model.  The main thread then
  // posts back to reply.  This is a base class; we derive from it to implement
  // tcp and unix socket support.
  class GdbCommThread {
  public:
    GdbCommThread(GdbServer &server,GdbServerNode &node) :
      _server(server),
      _node(node),
      _work(new io_service::work(_io_service)),
      _in_buf(new char [ _server.maxPacketSize() ]),
      _active(false)
    {
      _thread.reset(new boost::thread(bind(&io_service::run,&_io_service)));     
    }

    virtual ~GdbCommThread() 
    {
      delete [] _in_buf;
    };

    void join()
    {
      _thread->join();
    }

    // Overload this to post an asynchronous read to the relevant socket.  This
    // causes us to expect another command from the gdb client.
    virtual void getCommand() = 0;

    // This causes us to exit our run loops, thus shutting down this thread.  We
    // also signal this to the server, so that it can shut down itself if all
    // connections are finished.
    void commFinish()
    {
      _active = false;
      _work.reset();
      _io_service.stop();
      _server.post(bind(&GdbServer::commThreadFinished,&_server));
    }

    void finishIfInactive()
    {
      if (!_active) {
        VTPrint ("Quiting inactive comm thread.");
        _work.reset();
        _io_service.stop();
      }
    }

  protected:

    void handleAccept(const system::error_code& error)
    {
      if (!error) {
        getCommand();
      } else {
        RError("Unable to accept socket connection.");
      }
    }

    // Overload this to implement a synchronous write.
    virtual void doWrite(const SharedConstBuffer &buf) = 0;

    // Overload this to implement a synchronous write (simpler form).
    virtual void doWrite(const char *data,unsigned len) = 0;

    // Overload this to implement a synchronous read.
    virtual unsigned doRead(char *data,unsigned len) = 0;

    // Cancel any asynchronous read operations.
    virtual void doCancel() = 0;

    // A packet is introduced with the character `$', the actual packet-data, and the
    // terminating character `#' followed by a two-digit checksum: 
    // 
    //    $packet-data#checksum
    //
    // The two-digit checksum is computed as the modulo 256 sum of all characters between 
    // the leading `$' and the trailing `#' (an eight bit unsigned checksum).
    //
    //  $<packet info>#<checksum>.
    //
    // Exceptions to this rule are that we can receive a null character, which seems to indicate 
    // that the debugger is done and we can receive a Ctrl-C as an interrupt.
    void handleRead(const system::error_code& error,
                     size_t bytes_transferred)
    {
      //cout << "... in handleRead" << endl;
      if (error) {
        if (error == asio::error::operation_aborted) {
          //cout << "Got a cancel.\n";
        }
        return;
      }

      // This could be placed in handleAccept for TCP connections, but since UDP
      // isn't connection oriented, it doesn't work for all connection types.
      if (!_active) {
        _active = true;
        _server.post(bind(&GdbServer::commThreadActive,&_server));
      }

      _in_buf[bytes_transferred] = 0;
      //cout << "Got a read of " << bytes_transferred << ":  '" << _in_buf << "'\n";

      char *ptr = &_in_buf[0];
      char *in_data = 0;
      if (*ptr == 0 || *ptr == 3) {
        // Indicates that we should now exit or we have an interrupt.
        in_data = &_in_buf[0];
        _in_buf[1] = 0;
        handlePacket(in_data);
        return;
      } else if (*ptr == '$') {
        // Start of a real message.
        ++ptr;
      retry:       
        unsigned char checksum = 0;
        unsigned char xmitcsum = 0;
        unsigned count = 0;
        while (count < bytes_transferred) {
          if (*ptr == '$') {
            goto retry;
          }
          if (*ptr == '#') {
            break;
          }
          checksum = checksum + *ptr;
          ++count;
          ++ptr;
        }

        if (*ptr == '#') {
          // Terminate the input string.
          *ptr = 0;

          // Then we extract the transmitted checksum.
          xmitcsum = (hexChar(*++ptr) << 4);
          xmitcsum += hexChar(*++ptr);
          
          if (checksum != xmitcsum) {
            replyBadCsum();
          } else {
            replyGoodCsum();

            // If we have a sequence number, reply 
            if (_in_buf[3] == ':') {
              replySequenceNum();
              in_data = &_in_buf[4];
            } else {
              in_data = &_in_buf[1];
            }
            handlePacket(in_data);

            // Don't immediately look for a new command b/c we have to handle
            // the reply handshake.
            return;
          }
        }
      }
      // Got something weird, so just ignore it and read more.
      getCommand();
    }

    // Synchronously writes a message to gdb, handling the ack protocol.  If
    // is_response is true, then posts an async-read to get the next command.
    void writeResponse(SharedConstBuffer buf,bool ack,bool is_response)
    {
      do {
        doWrite(buf);
      } while (ack && !handleAck());

      if (is_response) {
        getCommand();
      }
    }

    // Returns true if we read a valid acknowledgement, false otherwise.
    bool handleAck()
    {
      // Do a synchronous read to try and get the acknowledgement.
      char buf[2];
      size_t bytes_transferred = doRead(buf,1);

      // If we get a valid ack, then we're done and can try to read another command.
      buf[bytes_transferred] = 0;
      //cout << "Got an ack read of " << bytes_transferred << ":  " << buf << "\n";

      bool rc = (bytes_transferred > 0 && buf[0] == '+');

      //cout << "  ack is:  " << rc << endl;

      return rc;
    }

    void replyBadCsum()
    {
      const char *bad = "-";
      doWrite(bad,1);
    }

    void replyGoodCsum()
    {
      const char *good = "+";
      doWrite(good,1);
    }

    void replySequenceNum()
    {
      char out[2] = { _in_buf[1], _in_buf[2] };
      doWrite(out,2);
    }

    // This is called with a valid packet received from the debugger.  The first
    // character is the unique code which indicates what to do.  In general,
    // this routine should simply post events to the main thread.  The main
    // thread's routines should do all of the heavy lifting: Parsing data in the
    // inbound buffer, calling the appropriate remote command, and posting a
    // reply.  Since this protocol is very synchronous, we can get by with a
    // single input buffer.
    // KEY_FUNC: handlePacket
    void handlePacket(char *data)
    {
      VTPrint("Server (" << _node.name() << ") received packet: [" << data << "]");

      _node.remoteIOHalt();

      switch (*data++) {
      case '@':
      case '\000':
        // A null means we should exit immediately.  By deleting the work
        // object, we cause the main thread's service routine to exit.
        commFinish();
        // We schedule no more work so that we will exit as well.
        break;
      case '\003':
        // We received an interrupt.  We've already exited any long running
        // tasks (a continue), so we simply record this fact so that when we
        // resume, we return the correct signal.
        _server.post(bind(&GdbServerNode::remoteInterrupt,&_node));
        break;
      case '!':
        _server.post(bind(&GdbServerNode::setExtendedMode,&_node));
        break;
      case '?':
        _server.post(bind(&GdbServerNode::remoteGetLastSignal,&_node));
        break;
      case 'b':
        // KEY_FUNC: -- reverse execution request function inside handlePacket
        _server.post(bind(&GdbServerNode::handleReverseExecution,&_node,data));
        break;
      case 'c':
        // Continue command.
        _server.post(bind(&GdbServerNode::handleContinue,&_node,data));
        break;
      case 'D':
        // We treat a detach as a message to end the simulation.
        _server.post(bind(&GdbServerNode::handleKill,&_node,true,true));
        break;
      case 'g':		
        // Return the value of the CPU registers.
        _server.post(bind(&GdbServerNode::getRegisters,&_node));
        break;    
      case 'G':	   
        // Set the value of the CPU registers - return OK.
        _server.post(bind(&GdbServerNode::setRegisters,&_node,data));
        break;
      case 'H':	   
        // Set the current thread to subsequent commands.
        _server.post(bind(&GdbServerNode::setThread,&_node,data));
        break;
      case 'k':	   
        /* we are being killed! */
        _server.post(bind(&GdbServerNode::handleKill,&_node,false,false));
        break;
      case 'm':
        // Memory query.
        _server.post(bind(&GdbServerNode::getMemory,&_node,data,false));
        break;
      case 'M':
        // Memory query.
        _server.post(bind(&GdbServerNode::setMemory,&_node,data,false));
        break;
      case 'p':		
        // Return the value of the specified CPU register.
        _server.post(bind(&GdbServerNode::getRegister,&_node,data));
        break;    
      case 'P':	   
        // Set the value of the specified CPU register.
        _server.post(bind(&GdbServerNode::setRegister,&_node,data));
        break;
      case 'q':
        // A general query command.
        _server.post(bind(&GdbServerNode::handleQuery,&_node,data));
        break;
      case 'Q':
        // Various mode operation commands.
        _server.post(bind(&GdbServerNode::handleQCmd,&_node,data));
        break;
      case 'R':
        // Reset command.
        _server.post(bind(&GdbServerNode::handleRestart,&_node));
        break;
      case 's':
        // Step command.
        _server.post(bind(&GdbServerNode::handleStep,&_node));
        break;        
      case 'T':
        // Thread status check command.
        _server.post(bind(&GdbServerNode::getThreadStatus,&_node,data));
        break;
      case 'v':
        // Step/continue commands via the v command.
        _server.post(bind(&GdbServerNode::handleVCmd,&_node,data));
        break;
      case 'X':
        // Memory binary set.
        _server.post(bind(&GdbServerNode::setMemoryBinary,&_node,data));
        break;
      case 'z':
        // Remove break or watch point.
        _server.post(bind(&GdbServerNode::removeBreakOrWatch,&_node,data));
        break;        
      case 'Z':
        // Add break or watch point.
        _server.post(bind(&GdbServerNode::addBreakOrWatch,&_node,data));
        break;        
      default:
        // Send a null/empty packet on an unknown command.
        VTPrint("Unknown command from gdb [" << data << "].");
        writeResponse(SharedConstBuffer(string()),true,true);
        getCommand();
        break;
      }

      _server.post(bind(&GdbServer::remoteContinue,&_server));
    }

  protected:
    friend class GdbServerNode;

    GdbServer                                   &_server;
    GdbServerNode                               &_node;
    io_service                                   _io_service;    // Runs in a separate thread; handles debugger communication.
    std::unique_ptr<boost::asio::io_service::work> _work;
    unique_ptr<boost::thread>                      _thread;
    char                                        *_in_buf;        // Input buffer for raw incoming data.
    bool                                         _active;
  };

  // The tcp implementation of the comm thread.
  class GdbTcpCommThread : public GdbCommThread {
  public:
    GdbTcpCommThread(GdbServer &server,GdbServerNode &node,unsigned port,const string &port_file) :
      GdbCommThread(server,node),
      _acceptor(_io_service,ip::tcp::endpoint(ip::tcp::v4(),port)),
      _socket(_io_service)
    {
      _acceptor.async_accept(_socket, bind(&GdbTcpCommThread::handleAccept, this, asio::placeholders::error));
      cerr << "Listening for a TCP connection on port " << dec << _acceptor.local_endpoint().port() << '\n';
      _server.writePortFile(_acceptor.local_endpoint().port(),port_file);
    }

    void getCommand()
    {
      _socket.async_read_some(buffer(_in_buf, _server.maxPacketSize()),
                              bind(&GdbTcpCommThread::handleRead, this,
                                   asio::placeholders::error,
                                   asio::placeholders::bytes_transferred));    
    }

    void getAck()
    {
      //cout << "... expecting an ack..." << endl;
      read(_socket,buffer(_in_buf, 1));
    }

    void doWrite(const SharedConstBuffer &buf)
    {
      asio::write(_socket,buf);
    }

    void doWrite(const char *buf,unsigned len)
    {
      asio::write(_socket, buffer(buf,len));
    }

    unsigned doRead(char *buf,unsigned len)
    {
      return asio::read(_socket,buffer(buf,len));
    }

    void doCancel()
    {
      _socket.cancel();
    }

  private:
    ip::tcp::acceptor               _acceptor;      // For accepting tcp connections.
    ip::tcp::socket                 _socket;        // Socket used for communication.
  };

  // The udp implementation of the comm thread.
  class GdbUdpCommThread : public GdbCommThread {
  public:
    GdbUdpCommThread(GdbServer &server,GdbServerNode &node,unsigned port,const string &port_file) :
      GdbCommThread(server,node),
      _socket(_io_service,ip::udp::endpoint(ip::udp::v4(),port))
    {
      cerr << "Listening for UDP traffic on port " << dec << _socket.local_endpoint().port() << '\n';
      _server.writePortFile(_socket.local_endpoint().port(),port_file);
      getCommand();
    }

    void getCommand()
    {
      //cout << "Back to getting a command..." << endl;
      _socket.async_receive_from(buffer(_in_buf, _server.maxPacketSize()),_remote_endpoint,
                                 bind(&GdbUdpCommThread::handleRead, this,
                                      asio::placeholders::error,
                                      asio::placeholders::bytes_transferred));    
    }

    void doWrite(const SharedConstBuffer &buf)
    {
      _socket.send_to(buf,_remote_endpoint);
    }

    void doWrite(const char *buf,unsigned len)
    {
      _socket.send_to(buffer(buf,len),_remote_endpoint);
    }

    unsigned doRead(char *buf,unsigned len)
    {
      return _socket.receive(buffer(buf,len));
    }

    void doCancel()
    {
      _socket.cancel();
    }

  private:
    ip::udp::socket   _socket;          // Socket used for communication.
    ip::udp::endpoint _remote_endpoint; // Endpoint from last receive.
  };

  GdbServer::GdbServer(GdbTransportType type,const string &hostname,bool verbose,bool has_waveform) :
    has_waveform(has_waveform),
    _type(type),
    _verbose(verbose),
    _hostname(hostname),
    _sim_work(new io_service::work(_sim_service)),
    _max_packet_size(GdbServerMinPacketSize),
    _active_comms(0)
  {
  }

  GdbServer::~GdbServer()
  {
    ForEach(_nodes,i) {
      delete i->_node;
    }
    _nodes.clear();
  }

  void GdbServer::commThreadActive()
  {
    ++_active_comms;
    VPrint("Comm thread now active.  Total = " << _active_comms);
  }

  void GdbServer::commThreadFinished()
  {
    --_active_comms;
    VPrint("Comm thread now finished.  Total = " << _active_comms);
    if (_active_comms == 0) {
      VPrint("All comm threads done.  Shutting down.");
      // For any remaining threads, signal that they should quit.
      ForEach (_nodes,i) {
        i->_node->finishIfInactive();
      }
      _sim_work.reset();      
    }
  }

  void GdbServer::addPortData(unsigned p,const string &pf)
  {
    _nodes.push_back(NodeElm(p,pf));
  }

  void GdbServer::writePortFile(unsigned port,const string &port_file)
  {
    if (!port_file.empty()) {
      ofstream of(port_file.c_str());
      if (!of) {
        throw runtime_error ("Unable to open GDB port file '"+port_file+"'.");
      }

      string hostname = _hostname;
      if (hostname.empty()) {
        const int BufSize = 1024;
        char hn[BufSize];
        if (gethostname(hn,BufSize) < 0) {
          hostname = "localhost";
        } else {
          hostname = hn;
        }
      }
      of << "target " << (remoteExtendedSupport() ? "extended-" : "") << "remote " << ( (_type == GdbUdp) ? "udp" : "tcp" ) << ":" << hostname << ":" << port << "\n";
    }
  }

  void GdbServer::setMaxPacketSize(unsigned m)
  {
    if (m > GdbServerMinPacketSize) {
      _max_packet_size = m;
    }
  }

  GdbCommThread *GdbServer::createCommThread(GdbServerNode &node,unsigned port,const string &port_file)
  {
    switch (_type) {
    case GdbTcp:
      return new GdbTcpCommThread(*this,node,port,port_file);
    case GdbUdp:
      return new GdbUdpCommThread(*this,node,port,port_file);
    default:
      // Should never get here!
      assert(0);
    }
  }

  // This initiates the comms threads and starts up the main service event
  // handler.
  void GdbServer::rundebugger()
  {
    if (_nodes.empty()) {
      RError("No port information specified.");
    }

    unsigned index = 0;
    ForEach(_nodes,i) {
      i->_node = createNode(index++,_verbose);
      i->_node->setCommThread(createCommThread(*i->_node,i->_port,i->_port_file));
      if (has_waveform) {
        i->_node->setWaveformThread(getWaveformPtr()); // for now everyone uses one same waveform thread
        _waveform->assignGdbNodes(i->_node);
      }
    }
    if (has_waveform) {
      _waveform->assignGdbServer(this);
      // _waveform->setup();
    }

    // This runs the main thread's event handler, which receives events from the
    // comms threads.
    _sim_service.run();

    // We're done when all threads are done.
    ForEach(_nodes,i) {
      i->_node->join();
    }
    if (has_waveform) {
      _waveform->join();
    }
  }





  GdbServerNode::GdbServerNode(GdbServer &server,bool verbose) :
    _server(server),
    _verbose(verbose),
    _extended_mode(false),
    _non_stop(false)
  {
  }

  void GdbServerNode::setCommThread(GdbCommThread *comm_thread)
  {
    _comm_thread.reset(comm_thread);
  }
  
  void GdbServerNode::setWaveformThread(WaveformThread *waveform_thread)
  {
    _waveform_thread = waveform_thread;
  }

  void GdbServerNode::join()
  {
    _comm_thread->join();
  }

  void GdbServerNode::finishIfInactive()
  {
    _comm_thread->_io_service.post(bind(&GdbCommThread::finishIfInactive,_comm_thread.get()));    
  }

  // Post a shutdown command.
  void GdbServerNode::serverFinish()
  {
    _comm_thread->_io_service.post(bind(&GdbCommThread::commFinish,_comm_thread.get()));    
  }

  // This causes us to not send anything and just expect another command from
  // gdb.
  void GdbServerNode::noResponse()
  {
    _comm_thread->_io_service.post(bind(&GdbCommThread::getCommand,_comm_thread.get()));
  }

  // Cancels any pending asynchronous reads.
  void GdbServerNode::cancelReads()
  {
    _comm_thread->_io_service.post(bind(&GdbCommThread::doCancel,_comm_thread.get()));
  }

  // If is_response is false, then we won't consider this to be a "response" and
  // thus won't expect another message from gdb.
  void GdbServerNode::putGdbPacket(const string &out,bool is_response)
  {
    VPrint ("  Reply (" << name() << "):  [" << out << "]");
    _comm_thread->_io_service.post(bind(&GdbCommThread::writeResponse,_comm_thread.get(),SharedConstBuffer(out),true,is_response));
  }

  void GdbServerNode::putGdbAsyncPacket(const string &out,bool is_response)
  {
    VPrint ("  Async (" << name() << "):  [" << out << "]");
    _comm_thread->_io_service.post(bind(&GdbCommThread::writeResponse,_comm_thread.get(),SharedConstBuffer(out,true),false,is_response));
  }

  // This is expected to be for relatively small response messages, so we're just
  // hardcoding in a buffer limit.
  void GdbServerNode::makeGdbPacket(const char *fmt, ...)
  {
    va_list ap;

    const int n = 1024;
    char buf[n];
    va_start(ap,fmt);
    unsigned len = vsnprintf(buf,n,fmt,ap);
    VPrint ("  Reply (" << name() << "):  [" << buf << "]");
    _comm_thread->_io_service.post(bind(&GdbCommThread::writeResponse,_comm_thread.get(),SharedConstBuffer(buf,len),true,true));
  }
  
  // to put in remoteGetRegisters
  void GdbServerNode::setWaveformTime(uint32_t time)
  {
    _waveform_thread->_w_io_service.post(bind(&WaveformThread::setCursor, _waveform_thread, time));
  }
  
  // to put in mon reg
  void GdbServerNode::checkWaveformTime(unsigned node_no, uint32_t time)
  {
    _waveform_thread->_w_io_service.post(bind(&WaveformThread::checkTime, _waveform_thread, node_no, time));
  }
   
  // setup waveform thread, to put in mon wsetup
  void GdbServerNode::setupWaveform()
  {
    _waveform_thread->setup();
  }
  
  // to put in mon wconnect
  void GdbServerNode::connectWaveform(string timestamp)
  {
    _waveform_thread->_w_io_service.post(bind(&WaveformThread::connect, _waveform_thread, timestamp));
  }
  
  // Extracts relevant info from a get-memory request and posts the request to the main thread.
  void GdbServerNode::getMemory(char *data,bool direct)
  {
    addr_t start_address = 0;
    int length_in_bytes = 0;
    
    /* mAA..AA,LLLL  Read LLLL bytes at address AA..AA */
    
    /* format is 'address,length', both in hex */
    start_address = hexStr(strtok(data,","));
    length_in_bytes  = hexStr(strtok(NULL,","));
    
    VPrint("Read memory: [addr=0x" << hex << start_address << 
           ", length(bytes)=0x" << length_in_bytes << "]");

    remoteGetMemory(start_address,length_in_bytes,direct);
  }

  // Set memory.
  void GdbServerNode::setMemory(char *data,bool direct) 
  {
    addr_t start_address = 0;
    int length_in_bytes = 0;
    char *value = 0;

    /* 'M' command - Write LLLL bytes at address AA.AA return OK */

    /* format is 'adress,length', both in hex */
    start_address = hexStr(strtok(data,",:"));
    length_in_bytes  = hexStr(strtok(NULL,",:"));
    value = strtok(NULL,",:");

    VPrint("Set memory: [addr=0x" << hex << start_address << 
           ", length(bytes)=0x" << length_in_bytes << 
           ", data=" << value << "]");

    remoteSetMemory(start_address,length_in_bytes,value,direct);
  }
    
  // Retrieve the complete set of registers from the current core.
  void GdbServerNode::getRegisters()
  {
    VPrint("Get registers.");

    remoteGetRegisters();
  }

  // Set all registers.
  void GdbServerNode::setRegisters(char *data) 
  {
    VPrint("Set registers: [" << data << "]");

    remoteSetRegisters(data);
  }

  void GdbServerNode::getRegister(char *data)
  {
    int register_number = 0;

    register_number = hexStr(data);

    VPrint("Get register [reg=" << dec << register_number << "]");

    remoteGetRegister(register_number);
  }

  //-------------
  // set_register
  //-------------
  void GdbServerNode::setRegister(char *data) 
  {
    int register_number = 0;
    string reg_val;

    /* format is 'register=value', both in hex */
    register_number = hexStr(strtok(data,"="));
    reg_val = strtok(NULL,"=");

    VPrint("Set register [reg=" << dec << register_number << ", value= 0x" << reg_val << "]");

    remoteSetRegister(register_number,reg_val);
  }

  void GdbServerNode::setThread(char *data)
  {
    // Note:  Input radix is hex.
    int threadNo = getThreadNum(&data[1]);
    
    VPrint("Set thread [threadno=" << dec << threadNo << "]");
    
    remoteSetThread(threadNo,true);
  }

  void GdbServerNode::handleContinue(char *data)
  {
    addr_t new_addr = 0;
    bool valid_addr = false;
    if (strlen(data) > 1) {
      // cAA..AA:  Continue at address AA.AA (optional).
      new_addr = hexStr(data);
      valid_addr = true;
    }
    remoteSetupContinue(valid_addr,new_addr);
    noResponse();
  }

  void GdbServerNode::handleStep()
  {
    remoteStep();
    remoteStepFinishResponse();
  }
                                             
  void GdbServerNode::handleKill(bool sendOk,bool reallyDoIt)
  {
    VPrint("Server received kill command from gdb.");
    remoteKill();
    if (_extended_mode && !reallyDoIt) {
      if (sendOk) {
        makeGdbPacket("OK");
      } else {
        VPrint ("  ... ignoring b/c we're in extended mode.");
        noResponse();
      }
    } else {
      if (sendOk) {
        makeGdbPacket("OK");
      }
      serverFinish();
    }    
  }

  // This handles remote commands, i.e. monitor commands.  The remote does
  // whatever it needs to with it, e.g. calls the scripting interpreter in ADL,
  // and then we send the result back using console output packets.  If we're
  // using Udp and the result is larger than the max packet size, we use O
  // commands to transmit it back and then just send OK.
  // KEY_FUNC: handleRemoteCommand
  void GdbServerNode::handleRemoteCommand(char *data)
  {
    // Execute the code and return whatever TCL returns.  We don't bother with
    // error handling b/c we want the message to be displayed to the user.  If
    // we sent back "E00" on a bad TCL command, the user wouldn't know what went
    // wrong.
    string result = remoteHandleCommand(ASCIIhex2Data(data));

    // If the result is empty then just send OK.  This is the official response
    // for a valid command that returns no data.
    if (result.empty()) {
      makeGdbPacket("OK");
      return;
    }

    // Add a trailing newline if missing.
    if (*result.rbegin() != '\n') {
      result.push_back('\n');
    }

    string output = Data2ASCIIhex(result);

    unsigned max = _server.maxPacketSize();
    if (output.size() > max) {      
      // To big for a single packet, so use console output.  For TCP, we could
      // just send it all back as one, but it seemed weird to have different
      // responses based upon what protocol is being used.
      for (unsigned i = 0; i < output.size(); i += _server.maxPacketSize()) {
        unsigned n = min(max,(unsigned)output.size()-i);
        string out = "O" + output.substr(i,n);
        putGdbPacket(out,false); 
      }     
      makeGdbPacket("OK");
    } else {
      // We got output, so send it back as a hex-encoded string.
      putGdbPacket(output);
    }
  }

  void GdbServerNode::handleQCmd(char *data)
  {
    if (strncmp(data,"NonStop",7) == 0) {
      // The remote actually sets our flag.  It can also set any other state
      // that it needs to.
      remoteNonStopMode(atoi(&data[8]));
    } else {
      VPrint("Unsupported Q command command from gdb [" << data << "]");
      putGdbPacket(string());
    }
  }

  void GdbServerNode::handleQuery(char *data)
  {
    if (strncmp(data,"adl.m",5) == 0) {
      // Our custom direct-memory get function.
      getMemory(&data[6],true);
    }
    else if (strncmp(data,"adl.M",5) == 0) {
      // Our custom direct-memory set function.
      setMemory(&data[6],true);
    }
    else if (strncmp(data,"Rcmd",4) == 0) {
      // This function itself takes care of sending back any output, so we
      // just send a final acknowledgement of error if there's a problem.
      handleRemoteCommand(&data[5]);
    }
    else if (strncmp(data,"Symbol", 6) == 0) {
      makeGdbPacket("OK");
    }
    else if (strncmp(data,"C",1) == 0) {
      makeGdbPacket("QC%x",remoteGetActiveThreadId());
    }
    else if (strncmp(data,"fThreadInfo",11) == 0) {
      ThreadIdList t = remoteGetThreadInfo();
      ostringstream ss;
      ss << "m";
      bool first = true;
      ForEach(t,iter) {
        if (!first) ss << ',';
        // Very important:  These are expected to be in hex!
        ss << hex << *iter;
        first = false;
      }
      putGdbPacket(ss.str());
    }
    else if (strncmp(data,"sThreadInfo",11) == 0) {
      /* indicate an end of list as for now we just will have 3 -6 threads */
      makeGdbPacket("l");
    }
    else if (strncmp(data,"ThreadExtraInfo",15) == 0) {
      // Provide thread-specific information back to gdb.
      int threadNo = getThreadNum(&data[16]);
      remoteGetThreadExtraInfo(threadNo);
    }
    else if (strncmp(data,"P",1) == 0) {
      makeGdbPacket("c");
    }
    else if (strncmp(data,"Attached",8) == 0) {
      // We consider gdb to always be attaching to an existing process.
      makeGdbPacket("1");
    } else if (strncmp(data,"Supported",9) == 0) {
      // Check to see what it's wondering about and reply appropriately.
      handleSupportQuery(&data[10]);
    } else {
      // GDB expects a null packet for an unsupported command.
      VPrint("Unsupported query command from gdb [" << data << "]");
      putGdbPacket(string());
    }
  }
  
  // KEY_FUNC: handleReverseExecution
  void GdbServerNode::handleReverseExecution(char *data)
  {
    if (strncmp(data,"c",1) == 0) {
      // reverse continue
      remoteRContinue();
      remoteRContinueFinishResponse();
    }
    else if (strncmp(data,"s",1) == 0) {
      // reverse step
      remoteRStep();
      remoteRStepFinishResponse();
    }
    else {
      // GDB expects a null packet for an unsupported command.
      VPrint("Unsupported query command from gdb [" << data << "]");
      putGdbPacket(string());
    }
  }

  // Respond to a support query.
  // KEY_FUNC: handleSupportQuery - respond RC and RS only when in trace mode
  void GdbServerNode::handleSupportQuery(char *data)
  {
    // Currently, we just handle (optionally) non-stop mode as the only extension.
    string reply;

    if (remoteNonStopSupported()) {
      reply += "QNonStop+;";
    }
    if (remoteReverseExecSupported()) {
      reply += "ReverseContinue+;ReverseStep+;";
    }

    // Empty response is OK- means nothing is supported.
    putGdbPacket(reply);

  }

  void GdbServerNode::handleRestart()
  {
    VPrint ("Remote restart received.");
    remoteRestart();
  }

  // This parses the vCont packet, using thread information obtained from remoteGetThreadInfo.
  void GdbServerNode::parseVContPacket(CommandList &commandList,char *data)
  {
    // Get thread list and populate the command list with uninitialized entries.
    ThreadIdList tl = remoteGetThreadInfo();
    commandList.insert(make_pair(0,'x'));
    ForEach(tl,iter0) {
      commandList.insert(make_pair(*iter0,'x'));
    }

    string str1(data);
    Args tokens1 = tokenize(str1,";");
    char default_cmd;

    ForEach(tokens1,iter1) {
      string str0 = *iter1;
      Args tmp0 = tokenize(str0,":");

      if(tmp0.size() == 1){
        default_cmd = tmp0[0][0];
      }	    
      else {
        int no = hexStr(tmp0[1].c_str());
        if (no < 0) {
          // A negative number means to apply the command to all threads.
          ForEach(commandList,iterT) {
            iterT->second = tmp0[0][0];
          }
        } else {
          // It seems that sometimes we'll get conflicting info, e.g. s:0;c:0,
          // so we take the first and ignore subsequent commands.
          map<int,char>::iterator iterT = commandList.find(no);
          if (iterT->second == 'x') {
            iterT->second = tmp0[0][0];
          }
        }
      }	    
    }
    auto iterT = commandList.begin();
    // The 0th thread means "pick a thread", so we always pick the first.
    if (commandList[0] != 'x') {
      commandList[1] = commandList[0];
    }
    // Anything not already handled gets the default command.
    for (; iterT != commandList.end(); iterT++) {
      if(iterT->second == 'x') {
        iterT->second = default_cmd;	    
      }
    }	  
  }

  void GdbServerNode::handleVContCommand(char *data)
  {
    CommandList commandList;
  
    // Unthreaded models will receive [vCont;s]
    // Threaded models will receive [vCont;s:1;c]

    // If we get a vCont;s:1;c and we're not in non-stop mode, then I currently
    // ignore the continue.  What does it mean to step a simulator for one core
    // and let the rest continue?  We don't want background execution in this
    // case, so it seems rather confusing.

    // First parse the packet and then process them. This is because step needs to 
    // execute last  and control given to the host.
    // Identify default action (flag error if > 1) Identify threads with no action
    // Get Thread list to apply default action to threads not specified in the command
  
    parseVContPacket(commandList,data);

    // If we have blocking threads (course grained), then we only modify the
    // currently active thread, otherwise we process all threads.
    int active_thread = remoteGetSimableThreadId();

    int old_tn = remoteGetActiveThreadId();

    bool had_continue = false;
    bool had_step = false;
    bool had_stop = false;

    // Do we have any steps?  Then in all-stop mode, we just step and we don't
    // continue anything.
    if (!nonStopMode()) {
      // Skip the default 0 command.
      auto iterT = commandList.begin();
      if (iterT->first == 0) ++iterT;

      for ( ; iterT != commandList.end(); ++iterT ) {
        if (iterT->second == 's') {
          had_step = true;
        } else if (iterT->second == 'c') {
          had_continue = true;
        } else if (iterT->second == 't') {
          had_stop = true;
        }
      }
    }
 
    int step_id = -1;
    int step_count = 0;
    
    // Do all the steps/continues on all threads.

    // Skip the default 0 command.
    auto iterT = commandList.begin();
    if (iterT->first == 0) ++iterT;
    while(iterT != commandList.end()) {
      if ( (active_thread == 0) || (iterT->first == active_thread)) {
        remoteSetThreadDirect(iterT->first);
        if (iterT->second == 'c') {
          // Continue:  Set core to be active.
          if (nonStopMode() || !had_step) {
            remoteSetupContinue(false,0);
          }
        } else if (iterT->second == 's') {
          // Step:  Step the core.
          remoteStep();
          step_id = iterT->first;
          ++step_count;
        } else if (iterT->second == 't') {
          // Stop the thread.
          remoteStopThread();
          step_id = -1;
        }
      }
      iterT++;
    }

    // We only want to update the thread if we had a vCont command with a single
    // step command in it.
    if (step_count > 1) step_id = -1;

    bool docont = true;
    bool skip_update = false;
    
    // If we had steps, then send a response.  Otherwise, we'll send the
    // response once we continue execution and return (for whatever reason).
    if (!had_continue || had_step || !remoteSimable()) {
      if (remoteStepFinishResponse()){
        // If we explicitly change threads, then don't do this implicit update.
        step_id = -1;
      }
      docont = false;

      // This seems odd, but it seems as though, if we just had a step, then we
      // need to make that the "current" thread, equivalent to an Hg command.
      // What I see from gdb is something like:
      //
      //  Hg8
      //  g
      //  vCont;s:2 or vCont;s:2;c
      //  g
      //
      // Then gdb expects 'g' to correspond to thread 2 and *not* thread 8, even
      // though that was the last Hg command.  On the other hand, if we
      // continue, then when we return, we specify what thread is active, so
      // then gdb knows to switch.
      if (step_id > 0) {
        VPrint ("Setting thread to " << step_id << " due to vCont step.");
        remoteSetThread(step_id,false);
        skip_update = true;
      }
    }

    // Course-grained MT hack: Don't restore the thread if we're course grained,
    // since one thread might have activated another.  Only do this for SMT.
    if (active_thread == 0 && !skip_update) {
      remoteSetThreadDirect(old_tn);
    }

    if (nonStopMode()) {
      // In non-stop mode, we reply immediately with "OK" and expect an
      // asynchronous reply when a thread stops.  We don't want this treated
      // as a response.
      putGdbPacket("OK",false);
      remoteAsyncStop();
      return;
    }

    // At this point, we do a continue, since we had one or more continues in
    // the packet.
    if (docont) {
      // Now we wait for more input (either an interrupt, or in non-stop mode,
      // further commands).
      noResponse();
    }

  }

  void GdbServerNode::handleVCmd(char *data)
  {
    if (strncmp(data, "Cont?", 5) == 0) {
      makeGdbPacket("vCont;c;C;s;S;t");
    }
    else if (strncmp(data, "Cont;", 5) == 0) {
      handleVContCommand(data+5);
    }	
    else if (strncmp(data, "Run;",4) == 0) {
      // We ignore filename and argument information- we're just re-running the
      // same program.
      handleRestart();
    }
    else if (strncmp(data, "Kill", 4) == 0) {
      // We don't support multi-process extensions right now, so if we get a
      // vKill, then we treat it like a kill.
      handleKill(true,false);
    }
    else if (strncmp(data,"Stopped",7) == 0) {
      // Otherwise, send the current thread status.
      remoteSendNextStopSequence();
    }
    else {
      VPrint("Unsupported v command command from gdb [" << data << "]");
      putGdbPacket(string());
    }
  }
  
  void GdbServerNode::handleWaveformTimeChange(uint32_t time)
  {
    remoteHandleWaveformTimeChange(time);
  }

  void GdbServerNode::getThreadStatus(char *data)
  {
    int threadNo = getThreadNum(data);
    remoteGetThreadStatus(threadNo);
  }

  void GdbServerNode::setMemoryBinary(char *data)
  {
    addr_t start_address = 0;
    int length_in_bytes = 0;
    int i = 0;
    int escaped = 0;
    char *value = 0;

    /* 'M' command - Write LLLL bytes at address AA.AA return OK */

    /* format is 'adress,length', both in hex */
    start_address = hexStr(strtok(data,",:"));
    char *l_start = strtok(NULL,",:");
    length_in_bytes  = hexStr(l_start);
    if (length_in_bytes > 0) {
      value = strtok(NULL,",:");
      if (!value) {
        // If we have a null at the end of the string, then we move to the end 
        // of the last string, since we know that the data comes after that.
        value = l_start + strlen(l_start)+1;
      }    
     
      if (verbose()) {
        char *vptr = value;
        cerr << "Set memory binary: [addr=0x" << hex << start_address
             << ", length(bytes)=0x" << length_in_bytes
             << ", data=";
        for (i = 0; i < length_in_bytes; vptr++) {
          /* the characters '$', '#', and 0x7d are escaped */
          /* special processing here to detect that        */
          if (*vptr != 0x7d) {
            cerr << setfill('0') << setw(2) << (0x000000FF & ((unsigned int) *vptr + (escaped * 0x20)));
            escaped = 0;
            i++;
          } 
          else {
            escaped = 1;
          }
        }
        cerr << "]\n";
      }

      remoteSetMemoryBinary(start_address,length_in_bytes,value);
    } else {
      makeGdbPacket("OK");
    }
  }

  void GdbServerNode::addBreakOrWatch(char *data) 
  {
    /* 
       format is: t,addr,length  (insert)
     
       t is type: 
       `0' - software breakpoint, 
       `1' - hardware breakpoint, 
       `2' - write watchpoint, 
       `3' - read watchpoint, 
       `4' - access watchpoint; 
     
       addr is address; 
     
       length is in bytes. 
     
       For a software breakpoint, length specifies the size of the instruction to be patched.
       For hardware breakpoints and watchpoints length specifies the memory region to be monitored.
    */

    unsigned type = hexStr(strtok(data,","));
    addr_t address = hexStr(strtok(NULL,","));
    unsigned length_in_bytes = hexStr(strtok(NULL,","));

    VPrint("Add break/watch point: [type=" << dec << type << 
           ", address=0x" << hex << address << 
           ", length_in_bytes=" << dec << length_in_bytes << "]");

    switch (type) {
    case 0 :
      remoteAddSoftwareBreakpoint(address,length_in_bytes);
      break;
    case 1 :
      remoteAddHardwareBreakpoint(address,length_in_bytes);
      break;
    case 2 :
      remoteAddWriteWatchpoint(address,length_in_bytes);
      break;
    case 3 :
      remoteAddReadWatchpoint(address,length_in_bytes);
      break;
    case 4 :
      remoteAddAccessWatchpoint(address,length_in_bytes);
      break;
    default:
      makeGdbPacket("E00");
      break;
    }
  }

  void GdbServerNode::removeBreakOrWatch(char *data) 
  {
    /* 
       format is: t,addr,length  (insert)
     
       t is type: 
       `0' - software breakpoint, 
       `1' - hardware breakpoint, 
       `2' - write watchpoint, 
       `3' - read watchpoint, 
       `4' - access watchpoint; 
     
       addr is address; 
     
       length is in bytes. 
     
       For a software breakpoint, length specifies the size of the instruction to be patched.
       For hardware breakpoints and watchpoints length specifies the memory region to be monitored.
    */

    unsigned type = hexStr(strtok(data,","));
    addr_t address = hexStr(strtok(NULL,","));
    unsigned length_in_bytes = hexStr(strtok(NULL,","));

    VPrint("Remove break/watch point: [type=" << dec << type << 
           ", address=0x" << hex << address << 
           ", length_in_bytes=" << dec << length_in_bytes << "]");

    switch (type) {
    case 0 :
      remoteRemoveSoftwareBreakpoint(address,length_in_bytes);
      break;
    case 1 :
      remoteRemoveHardwareBreakpoint(address,length_in_bytes);
      break;
    case 2 :
      remoteRemoveWriteWatchpoint(address,length_in_bytes);
      break;
    case 3 :
      remoteRemoveReadWatchpoint(address,length_in_bytes);
      break;
    case 4 :
      remoteRemoveAccessWatchpoint(address,length_in_bytes);
      break;
    default:
      makeGdbPacket("E00");
      break;
    }
  }

  void GdbServerNode::setExtendedMode()
  {
    if (remoteExtendedSupport()) {
      VPrint("Extended-mode is supported.");
      _extended_mode = true;
      makeGdbPacket("OK");
    } else {
      VPrint("Extended-mode is NOT supported.");
      makeGdbPacket("E00");
    }
  }

}

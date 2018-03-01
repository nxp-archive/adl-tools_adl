//
// Copyright (C) 2015 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef WAVEFORM_CLIENT_H
#define WAVEFORM_CLIENT_H

#include <boost/thread.hpp>
#include "GdbServer.h"

using namespace std;

namespace adl {
  
  class GdbServer;
  class GdbServerNode;
          
  // KEY_FUNC: WaveformThread 
  class WaveformThread {
    
  // KEY_FUNC: public: 
  public:
    boost::asio::io_service                        _w_io_service;
  
    WaveformThread(string &port, string file_to_open, string time_unit);
        
    void setup();
    
    void connect(string timestamp);
    
    void assignGdbNodes(GdbServerNode* node);
    
    void assignGdbServer(GdbServer* server);
    
    void join();  
        
    uint32_t cursor();
    
    // KEY_FUNC: -- setCursor
    void setCursor(uint32_t time);
    
    // KEY_FUNC: -- checkTime
    void checkTime(unsigned node_no, uint32_t server_time);
    
    // KEY_FUNC: -- finished 
    void finish();
   
  // KEY_FUNC: private:    
  private:
  
    friend class GdbServerNode;
    
    std::unique_ptr<boost::asio::io_service::work> _w_work;
    unsigned                                       _port;
    boost::asio::ip::tcp::socket*                  _socket;
    boost::thread                                  _w_thread;
    static const int                               _requestSize = 256;
    static const int                               _responseSize = 1024;
    char                                           _requestBuf[_requestSize];
	  char                                           _responseBuf[_responseSize] = {0};
    string                                         _file_to_open;
    string                                         _time_unit;
    uint32_t                                       _cursor;
    
    GdbServer*                                     _server;
    vector<GdbServerNode*>                         _nodes;
    
    string*                                        _prefix = new string("ADLrequest {");
    string*                                        _suffix = new string("}");
     
    // KEY_FUNC: -- handle_connect
    void handle_connect(const boost::system::error_code& err, string timestamp);
    
    // KEY_FUNC: -- closeSocket 
    void close_socket(); 
    
    // KEY_FUNC: -- init
    void init(string timestamp);    
    
    // KEY_FUNC: -- get_cursor
    void get_cursor();
    
    void reset_response_buffer();
    
    // KEY_FUNC: -- send_command 
    bool send_command(string cmd);
    bool send_command_simple(string cmd);
    
    // KEY_FUNC: -- build_request 
    string build_request(string cmd);
    
    // KEY_FUNC: -- parse_time 
    void parse_time(string response);
    
    // KEY_FUNC: -- parse_port_info 
    void parse_port_info(string &port);
  };
}

#endif
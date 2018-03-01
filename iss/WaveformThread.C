//
// Copyright (C) 2015 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#include <algorithm>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>

#include "StringHelpers.h"
#include "helpers/AnyOption.h" 
#include "WaveformThread.h"

using namespace boost;
using namespace boost::asio;

namespace adl {
  
  AnyOption &options();
  
  static bool dummy ATTRIBUTE_UNUSED = 
    options().setOption("w-port","wp","Waveform Port to communicate to the waveform viewer.") && 
    options().setOption("w-file","wf","Waveform file to open.") &&
    options().setOption("w-unit","wu","Waveform time unit.");
  
  WaveformThread::WaveformThread(string &port, string file_to_open, string time_unit) :
    _w_work(new io_service::work(_w_io_service)),
    _file_to_open(file_to_open),
    _time_unit(time_unit.empty()?"ns":time_unit),
    _cursor(0)
  {    
    _socket = new ip::tcp::socket(_w_io_service);
    _w_thread = boost::thread(bind(&io_service::run,&_w_io_service));  
    parse_port_info(port);
  }
  
  void WaveformThread::setup()
  {        
  }
  
  void WaveformThread::connect(string timestamp)
  {
    // _socket = new ip::tcp::socket(_w_io_service);
    boost::system::error_code option_ec;
    _socket->set_option(ip::tcp::no_delay(true), option_ec);
    
    boost::system::error_code ec;
        
    int connect_times = 0;
    do {
      _socket->connect(ip::tcp::endpoint(ip::address::from_string("127.0.0.1"), _port), ec); 
      boost::this_thread::sleep(boost::posix_time::seconds(2));
      cerr << "connecting to SimVision ..." << '\n';
      // cout << ec << endl;
      connect_times ++;
      // cout << connect_times << endl;
    } while (ec == boost::asio::error::connection_refused && connect_times < 50);
    // cout << ec << endl;
    handle_connect(ec, timestamp);
    // _timer.expires_from_now(boost::posix_time::seconds(5));
    // _timer.async_wait(boost::bind(&WaveformThread::closeSocket, this));
    
  }
  
  
  void WaveformThread::assignGdbNodes(GdbServerNode* node)
  {
    _nodes.push_back(node);
  }
    
  void WaveformThread::assignGdbServer(GdbServer* server)
  {
    _server = server;
  }
  
  void WaveformThread::join()
  {
    _w_thread.join();
  }
    
  uint32_t WaveformThread::cursor() {
    return _cursor;
  }
  
  // KEY_FUNC: -- setCursor
  void WaveformThread::setCursor(uint32_t time) {
    
    std::string time_s = std::to_string(time);
    send_command("cursor set -time " + time_s + _time_unit);
    send_command("waveform xview moveto -cursor Running_point");
    
  }
  
  // KEY_FUNC: -- checkTime
  void WaveformThread::checkTime(unsigned node_no, uint32_t server_time) {
    get_cursor();
    if (_cursor != server_time) {
      if (_cursor != 0) {
        _server->post(bind(&GdbServerNode::handleWaveformTimeChange, _nodes.at(node_no), _cursor));        
      }
    }
  }
  
  // KEY_FUNC: -- finish 
  void WaveformThread::finish() { 
    send_command("exit");   
    close_socket();
    _w_work.reset();
    // _w_io_service->stop();
  }

  // KEY_FUNC: -- handle_connect
  void WaveformThread::handle_connect(const boost::system::error_code& err, string timestamp)
  {
    if (err){
      cerr << "Connect to SimVision error: " << err.message() << "\n";      
      exit(1);
    }
    else {
      cerr << "Successfully connect to SimVision.\n";
      init(timestamp);
    }
  }
  
  // KEY_FUNC: -- closeSocket 
  void WaveformThread::close_socket() {
    _socket->close();
  }     
  
  // KEY_FUNC: -- init
  void WaveformThread::init(string timestamp)
  {
    send_command("database open " + _file_to_open);
    send_command("set w [waveform new -name Lumina_wave -units " + _time_unit + "]");
    send_command("cursor new -name Running_point");
    send_command("waveform set -primarycursor Running_point");
    send_command("cursor delete TimeA");
    send_command("window iconify Console");
    // set initial time    
    send_command("cursor set -time " + timestamp + _time_unit);
    send_command("waveform xview moveto -cursor Running_point");
  }
  
  // KEY_FUNC: -- get_cursor
  void WaveformThread::get_cursor() {
    
    send_command("cursor get -using Running_point -time -units " + _time_unit);
    string response = (string)_responseBuf;
    parse_time(response);    
    
  }
  
  // KEY_FUNC: -- reset_response_buffer
  void WaveformThread::reset_response_buffer()
  {
    memset(&_responseBuf[0], 0, _responseSize);        
  }
  
  // KEY_FUNC: -- send_command 
  bool WaveformThread::send_command(string cmd)
  {
    // synchronous send and receive because it needs to get result immediately
    if (_socket->is_open()) { 
      reset_response_buffer();
      cerr << _responseBuf;
      string request = build_request(cmd);
      _socket->write_some(buffer(request, request.length()+1));  
      _socket->read_some(buffer(_responseBuf, _responseSize));  
    }
    else {
      return false;
    }
    
    return true;
    
  }
  
  // KEY_FUNC: -- send_command_simple
  bool WaveformThread::send_command_simple(string cmd)
  {
    string request = build_request(cmd);
    _socket->write_some(buffer(request, request.length()+1));  
    return true;
    
  }
    
  // KEY_FUNC: -- build_request 
  string WaveformThread::build_request(string cmd)
  {
    string formated_cmd = *_prefix + cmd + *_suffix + '\n';
    return formated_cmd;
  }
  
  // KEY_FUNC: -- parse_time 
  void WaveformThread::parse_time(string response)
  {  
    // response ok {-name Running_point -time 1224ns -marching 0 -database {}}
    string time_s;
    uint32_t time;
    size_t found = response.find_last_of("ok");
    if (found != std::string::npos) {
      time_s = response.substr(found+2,response.length()-found-2); 
      // strip chars in numbers
      char strip_chars[] = "\r\n, ";
      for (unsigned int i = 0; i < strlen(strip_chars); ++i){
        time_s.erase(std::remove(time_s.begin(), time_s.end(), strip_chars[i]), time_s.end());
      }
      if (getUInt(time, time_s)) {
        _cursor = time;
      }
      else {
        _cursor = 0;
      }
    }
    else {
      _cursor = 0;
    }
  }
  
  // KEY_FUNC: -- parse_port_info 
  void WaveformThread::parse_port_info(string &port)
  {
    unsigned p;
    if (getUInt(p, port)) {
      _port = p;
    }
    else {
      _port = 5678; // default port number
    }
  }
}

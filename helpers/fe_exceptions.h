//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Various exception classes.
//

#ifndef _FE_EXCEPTIONS_H_
#define _FE_EXCEPTIONS_H_

#include <string>
#include <stdexcept>

#include "opencxx/parser/Msg.h"

namespace Opencxx {
  class Ptree;
}

namespace adl {

  struct MsgData {
    Opencxx::Ptree        *_loc;
    std::string            _msg;
    Opencxx::Msg::Severity _severity;
  
    MsgData(Opencxx::Ptree *loc,const std::string &msg, Opencxx::Msg::Severity s = Opencxx::Msg::Error) :
      _loc(loc), _msg(msg), _severity(s) {};
  };

  class parse_error : public std::exception
  {
  public:

    explicit parse_error ( Opencxx::Ptree *p,const std::string &msg, 
                           Opencxx::Msg::Severity s = Opencxx::Msg::Error) :
      _data(p,msg,s) { };
    parse_error(const MsgData &d) : _data(d) {};
    ~parse_error() throw() {};
    virtual const char * what () const throw () { return _data._msg . c_str (); }
    Opencxx::Ptree *loc() const { return _data._loc; };
    Opencxx::Msg::Severity severity() const { return _data._severity; };
    const MsgData &data() const { return _data; };
  private:
    MsgData _data;
  };

  class parse_warning : public parse_error
  {
  public:
    explicit parse_warning ( Opencxx::Ptree *p,const std::string &what_arg ) :
      parse_error ( p,"warning: "+what_arg,Opencxx::Msg::Warning) { };
    ~parse_warning() throw() {};
  };

  struct decode_error : public std::runtime_error 
  {
    explicit decode_error ( const std::string &msg ) : std::runtime_error(msg) {};
  };

  // Throws a parse error.  User must include sstream for this to work.
#define PError(p,x) { std::ostringstream ss; ss << x; throw parse_error(p,ss.str()); }
#define PWarn(p,x) { std::ostringstream ss; ss << x; throw parse_warning(p,ss.str()); }

}

#endif


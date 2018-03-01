//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// A basic error message class.  An option allows for the file name to have
// only its basename printed.  This is useful for regression tests, where you
// want to run diff against a reference copy of the output.
//

#ifndef _BASE_MSG_H_
#define _BASE_MSG_H_

#include <string>

#include <opencxx/parser/Msg.h>
#include <opencxx/parser/SourceLocation.h>

#include "Helpers.h"

namespace adl {

  class BaseMsg : public Opencxx::Msg
  {
  public:
    BaseMsg(Severity severity, const Opencxx::SourceLocation& location, 
            const std::string& msg, bool fullpath = true) : 
      _severity(severity), _location(location), _msg(msg), _fullpath(fullpath)
    {}

    virtual ~BaseMsg() {};

    Severity GetSeverity() const { return _severity;  }
    
    void PrintOn(std::ostream& os) const;

  private:
    Severity                _severity;
    Opencxx::SourceLocation _location;
    std::string             _msg;
    bool                    _fullpath;
  };

}

#endif

//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Simple logging mechanism class.
//


#include "MsgLog.h"

namespace adl {

  MsgLog::MsgLog() : _count(0), _error_count(0), _pass_exceptions(false)
  {
  }

  void MsgLog::add(const MsgData &md)
  {
    _messages.push_back(md);
    ++_count;
    if (md._severity != Opencxx::Msg::Warning) {
      ++_error_count;
    }
  }

  void MsgLog::add(const parse_error &pe)
  {
    add(pe.data());
  }

  void MsgLog::add(const parse_warning &pw)
  {
    add(pw.data());
  }

  void MsgLog::clear()
  {
    _messages.clear();
    _count = 0;
  }

}

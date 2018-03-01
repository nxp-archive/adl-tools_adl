//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Simple logging mechanism class.
//

#ifndef _MSGLOG_H_
#define _MSGLOG_H_

#include <iosfwd>

#include "helpers/gc_list.h"
#include "helpers/fe_exceptions.h"

namespace adl {

  class MsgLog {
    typedef gc_list<MsgData> Messages;
  public:
    typedef Messages::const_iterator const_iterator;
    typedef Messages::const_reverse_iterator const_reverse_iterator;

    MsgLog();

    void add(const MsgData &);
    void add(const parse_error &);
    void add(const parse_warning &);

    // The count of all messages:  warnings + errors.
    unsigned count() const { return _count; };
    // The count of just errors.
    unsigned error_count() const{ return _error_count; };

    // Returns true if there are no messages.
    bool empty() const {return _count == 0; };

    // Used to signal if exceptions should be passed or caught at a specific point.
    bool pass_exceptions() const { return _pass_exceptions; };
    void set_pass(bool s) { _pass_exceptions = s; };

    const_iterator begin() const { return _messages.begin(); };
    const_iterator end() const { return _messages.end(); };

    const_reverse_iterator rbegin() const { return _messages.rbegin(); };
    const_reverse_iterator rend() const { return _messages.rend(); };

    void clear();

  private:  
    Messages _messages;
    unsigned _count;
    unsigned _error_count;
    bool     _pass_exceptions;
  };

#define LogPError(l,x) try { x; } catch (parse_error &err) { l.add(err); }
#define AddError(l,p,x) { std::ostringstream ss; ss << x; l.add(parse_error(p,ss.str())); }
#define AddWarning(l,p,x) { std::ostringstream ss; ss << x; l.add(parse_warning(p,ss.str())); }

}

#endif

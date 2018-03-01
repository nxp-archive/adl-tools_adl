//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Generic parser which creates a tree of SimpleHandler objects
// after parsing a file.
//

#include "SimpleParser.h"
#include "SimpleHandler.h"

namespace adl {

  SimpleParser::SimpleParser() :
    _data(0)
  {
  }

  Handler *SimpleParser::make_initial_handler(Opencxx::Environment *env)
  {
    return _data = new SimpleHandler(env,0,"outer","outer",false);
  }

}

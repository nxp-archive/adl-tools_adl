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

#ifndef _SIMPLE_PARSER_H_
#define _SIMPLE_PARSER_H_

#include "BaseParser.h"

namespace adl {

  class SimpleHandler;

  class SimpleParser : public BaseParser {
  public:
    SimpleParser();

    const SimpleHandler *data() const { return _data; };

  protected:
    // Creates the initial outer-handler.
    virtual Handler *make_initial_handler(Opencxx::Environment *);

    // This is the data model object.
    SimpleHandler *_data;
  };

}

#endif

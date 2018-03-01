//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Main entry point for the front-end parsing library.
//

#ifndef _ADL_PARSER_H_
#define _ADL_PARSER_H_

#include <cassert>

#include "BaseParser.h"

namespace adl {

  class Data;

  // The parser class does all of the parsing, including running the preprocessor.
  class AdlParser : public BaseParser {
  public:
    AdlParser();

    // We allow non-const access so that subsequent users of the data model may
    // add declarations, helpers, etc.
    Data &data() const { assert(_data); return *_data; };

  protected:
    // Creates the initial outer-handler.
    virtual Handler *make_initial_handler(Opencxx::Environment *);

    // Creates the initial data model.
    virtual void setup(Opencxx::Environment *env);

    // Stores outer-most-scope functions.
    virtual void add_function(Opencxx::Ptree *);

    // Called at end of first-pass in order to store any declarations encountered.
    virtual void add_declaration(Opencxx::Ptree *);

    // Creates the data model, handles the second pass.
    virtual void second_pass(MsgLog &);

    // This is the data model object.
    Data         *_data;

  };

}

#endif

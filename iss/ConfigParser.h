//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Parser for time-tag configuration information.
//

#ifndef _CONFIG_PARSER_H_
#define _CONFIG_PARSER_H_

#include "fe/BaseParser.h"

namespace adl {

  class Data;
  class ConfigDB;

  // The parser class.
  class ConfigParser : public BaseParser {
  public:
    ConfigParser(ConfigDB &,const Data &data,bool check_aux_funcs);

  protected:
    // Creates the initial outer-handler.
    virtual Handler *make_initial_handler(Opencxx::Environment *);

    void second_pass(MsgLog &msglog);

    // This is the data model object.
    ConfigDB   &_config;
    const Data &_data;
    bool        _check_aux_funcs;
  };

}

#endif

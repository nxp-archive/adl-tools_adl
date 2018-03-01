//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Main entry point for the front-end parsing library.
//

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <cerrno>

#include "opencxx/parser/CerrErrorLog.h"
#include "opencxx/parser/Lex.h"
#include "opencxx/parser/Parser.h"
#include "opencxx/parser/Program.h"
#include "opencxx/parser/ProgramFile.h"
#include "opencxx/parser/ErrorLog.h"
#include "opencxx/parser/TheErrorLog.h"
#include "opencxx/Class.h"
#include "opencxx/driver2.h"
#include "opencxx/OpencxxConfiguration.h"
#include "opencxx/UnknownCliOptionException.h"
#include "opencxx/PtreeTypeUtil.h"

#include "helpers/fe_exceptions.h"
#include "helpers/Macros.h"

#include "AdlParser.h"
#include "Handlers.h"
#include "Data.h"
#include "Helpers.h"
#include "MsgLog.h"

using namespace std;
using namespace Opencxx;
using namespace PtreeUtil;

namespace adl {

  AdlParser::AdlParser() :
    BaseParser(),
    _data(0)
  {
  }

  Handler *AdlParser::make_initial_handler(Environment *env)
  {
    return new OuterHandler(env,0);
  }

  void AdlParser::setup(Opencxx::Environment *env)
  {
    _data = new Data(env);
  }

  void AdlParser::add_function(Ptree *exp)
  {
    _data->add_decl(exp);
  }

  // For now, we'll just allow anything to pass through.
  // We might want to tighten this up in the future.
  void AdlParser::add_declaration(Ptree *exp)
  {
    _data->add_decl(exp);
  }

  // Create the data model during the second pass.
  void AdlParser::second_pass(MsgLog &msglog)
  {
    OuterHandler &outer = dynamic_cast<OuterHandler&>(*_handler);

    // This handles the key/value pairs and does an initial
    // populating of the data model.
    outer._data = _data;
    outer.process_keys(msglog);

    // Note: We proceeed even if we get errors in order to be able to show more
    // issues to the user in one session.

    // The second pass is handled by the data model and does
    // final checks and populating of data structures.
    //print_mem_usage("Before finalization");
    _data->finalize(msglog);
    //print_mem_usage("After finalization");
  }
}

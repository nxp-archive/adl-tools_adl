//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Driver for the front-end parsing library.
//

#include <iostream>

#include "helpers/AnyOption.h"

#include "fe/BaseParser.h"
#include "fe/AdlParser.h"
#include "fe/Data.h"

using namespace std;
using namespace Opencxx;
using namespace adl;

int main(int argc, const char** argv)
{
  try {
    AnyOption options(argc,argv);
    options.addUsage(":  [options] <source>");
    options.setFlag("help","h","Display help");
    options.setFlag("data-model-debug","dmd","Turn on extra debug info when printing the data model.");

    standardParserOptionSetup(options);

    if (!options.processCommandLine()) {
      return 1;
    }

    if (options.getFlag("h")) {
      options.printUsage();
      return 0;    
    };

    if (!options.getArgc()) {
      cerr << "Error:  No source file specified.\n";
      options.printUsage();
      return 1;
    }

    AdlParser parser;

    setDataModelDebug(options.getFlag("dmd",dataModelDebug()));

    standardParserSetup(parser,options,"");

    if (!parser.parse(options.getArgv(0))) {
      return 1;
    }

    const Data &data = parser.data();
    if (data.cores().empty() && data.systems().empty()) {
      cerr << "Error:  No cores or systems were defined.\n";
      return 1;
    }

    parser.data().dump(cout,"");
  }
  catch (exception &err) {
    cerr << "Error:  " << err.what() << "\n";
    return 1;
  }

  return 0;
}


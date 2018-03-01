//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Driver for the testing the simple parser.
//

#include <iostream>

#include "helpers/AnyOption.h"

#include "fe/SimpleParser.h"
#include "fe/SimpleHandler.h"

using namespace std;
using namespace Opencxx;
using namespace adl;

int main(int argc, const char** argv)
{
  AnyOption options(argc,argv);
  options.addUsage(":  [options] <source>");
  options.setFlag("help","h","Display help");

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

  SimpleParser parser;

  if (!parser.parse(options.getArgv(0))) {
    return 1;
  }

  const SimpleHandler *data = parser.data();

  if (!data) {
    cerr << "Error:  No data was found.\n";
    return 1;
  }

  cout << parser.data() << "\n\n";

  return 0;
}


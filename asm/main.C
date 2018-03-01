//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Wrapper program for invoking the assembler/disassembler generation function.
//

#include <iostream>

#include "fe/AdlParser.h"

#include "helpers/AnyOption.h"
#include "helpers/FilenameUtils.h"
#include "helpers/Macros.h"
#include "helpers/LogUsage.h"

#include "GenAsm.h"

using namespace std;
using namespace adl;

int main(int argc, const char** argv)
{
  log_usage_at_exit(argc,argv,"tool");

  AnyOption options(argc,argv);

  standardParserOptionSetup(options);  
  options.addUsage(":  [options] <source>");
  options.setFlag("help","h",                 "Display help.");
  options.setFlag("dis",                      "Generate a disassembler (default is 1).");
  options.setFlag("asm",                      "Generate an assembler (default is 1).");
  options.setFlag("dbg",                      "Generate debugger assist information (default is 0).");
  options.setFlag("print-data",               "Print the data model.");
  options.setFlag("ext-signed",               "Treat positive signed immediate's size as unsigned");
  options.setFlag("keep-ws",                  "Keep all whitespace in syntax stsrings.");
  options.setFlag("prefix-shift",             "Shift instruction by its prefix bits during disassembly.  Default is true.");
  options.setFlag("big-mem",                  "For unresolved symbols - uses the instruction with largest immediate.  Default is true."); 
  options.setOption("asm-output","ao",        "Specify an output name for the assembler.  Default is as-<input-base>.c.");
  options.setOption("dis-output","do",        "Specify an output name for the disassembler.  Default is dis-<input-base>.c.");
  options.setOption("hdr-output","ho",        "Specify an output name for the debugger header.  Default is instr-info-<input-base>.h.");
  options.setOption("arch-pfx","ap",          "Specify the architecture prefix to use.");
  options.setOption("arch-name",              "Specify the architecture's name.");
  options.setOption("arch-print-name",        "Specify an alternative printable name for the architecture.");
  options.setOption("core",                   "Specify the core for which to produce an assembler/disassembler.");
  options.setOption("reloc-extra",            "Specify a reloc (by name) which needs extra handling by custom code.");
  options.setOption("log-usage",              "Enable usage logging.  The argument takes the form of program-name:version.  The option\n"
                                              "may be negated to turn it off.  The default is on, where the program name defaults to\n."
                                              "the root of the input-file name and the version is '<year>.<month>.<day>'.");
  options.setOption("asm-ver",                "Specify a version string for the assembler.");
  options.setOption("elf-machine-code","emc", "Specify a machine code to use in the input or output ELF file.");
  options.setOptionalValue("log-usage");
  
  if (!options.processCommandLine()) {
    return 1;
  }

  if (options.getFlag("h")) {
    options.printUsage();
    return 0;    
  };

  if (!options.getArgc()) {
    cerr << "No source file specified.\n";
    options.printUsage();
    return 1;
  }

  // Create the data model, exit if there's a problem.
  AdlParser parser;

  string input = options.getArgv(0);

  // If the user hasn't specified an architectural prefix, then use the input
  // filename's base.
  string archpfx = options.getOption("ap");
  if (archpfx.empty()) {
    archpfx = gen_default_name(input,"");
  }

  string archname = options.getOption("arch-name");
  if (archname.empty()) {
    archname = archpfx;
  }

  string archpname = options.getOption("arch-print-name");
  if (archpname.empty()) {
    archpname = archname;
  }

  // The output is based upon the input, unless otherwise specified.

  standardParserSetup(parser,options,archpfx);

  // This parses the ADL file and creates the data model.
  if (!parser.parse(input)) {
    return 1;
  }

  try {
    genAsmDisasm(parser.data(),archpfx,archname,archpname,input,options);
  }
  catch (exception &err) {
    cerr << "Error:  " << err.what() << "\n";
    return 1;
  }

  return 0;
}

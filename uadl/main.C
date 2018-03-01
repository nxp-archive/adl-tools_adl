//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#include <fstream>
#include <stdexcept>
#include <sstream>
#include <iostream>

#include "helpers/FilenameUtils.h"
#include "helpers/Macros.h"
#include "helpers/AnyOption.h"
#include "helpers/LogUsage.h"
#include "helpers/pfxstream.h"

#include "fe/AdlParser.h"

#include "iss/GenDriver.h"
#include "iss/ConfigDB.h"
#include "iss/GenInstrPacket.h"

#include "UadlDriver.h"
#include "UadlParser.h"
#include "GenCode.h"

using namespace std;
using namespace adl;
using namespace uadl;

namespace uadl {

class UadlIr;

  GenDriverIF *create_adl_gen(const AnyOption &options, AdlParser &parser,
                              const gc_string &outFilename,const gc_string &realOutFilename,
                              const gc_string &uadlHFilename,const gc_string &ISSnamespace,
                              bool fullpatherrors);

  void miniADLParse(const UadlIr &ir,GenDriverIF *driver,GenConfig &config);

  void setDataStrictness(bool s);
}

void setup_cmdline_options(AnyOption &options)
{
  standardParserOptionSetup(options);

  options.addUsage(": [options] <adl file> <uadl file>\n");
  options.setFlag("help", "h", "display help");
  options.setOption("verbose",           "Enable verbose mode.  May be given an integer value for a verbose-level.  Default is 0.");
  options.setOptionalValue("verbose","1");
  options.setOption("systemc",           "Create a SystemC module.  May be given an argument of 'thread' or 'method' to indicate\n"
                                         "the type of interface to create.  Defaults to 'method' if no argument is given.  The\n"
                                         "option may be negated.  The default is off.");
  options.setOptionalValue("systemc","method");
  options.setFlag("data-model-debug",    "Turn on extra debug info when printing the data model.");
  options.setFlag("print-data",          "Print the data model.");
  options.setFlag("check-cr-args",       "Parse custom-resource headers and check arguments.  False by default if --print-data is true.");
  options.setFlag("print-all-data",      "Print both the ADL and uADL data models.");
  options.setFlag("safe-mode",           "Decouple functionality and timing.\nDefault: false");
  options.setFlag("iss-mem-mode",        "For safe-mode models, use only the ISSs memory interface.\nDefault: false");
  options.setFlag("extern-mem",          "Use external memory.\nDefault: false");
  options.setFlag("extern-event-bus",    "Enable event buses broadcasting via an installed handler.  Default is true.");
  options.setFlag("extern-cycle-mem",    "Cycle the external memory during the pre and post phases.\nDefault: false");
  options.setFlag("single-file",         "Generate a uADL source file so that a single compilation will suffice.\nDefault: true");
  options.setFlag("rnumber",             "Turn on or off rnumber support.");
  options.setFlag("adl-strict",          "Set strict-parsing for ADL only.");
  options.setFlag("uadl-strict",         "Set strict-parsing for uADL only.");
  options.setFlag("disassembler",        "Generate a disassembler function in the model.\nDefault is false.");
  options.setFlag("instr-class-vd",      "Use virtual dispatch for instruction classes, rather than templates.");
  options.setOption("preamble",          "Add a preamble string.");
  options.setOption("output-base", "o",  "Specify the basename of output files\n"
                                         "Default:  Basename of the input uadl file.");
  options.setOption("namespace",         "Specify the model namespace.");
  options.setOption("detect-deadlock",   "Specify a cycle-count for detecting deadlock.  Valid only for safe-mod models.\n"
                                         "A value of 0 disables this feature.  The default is 0.\n");
  options.setOptionalValue("detect-deadlock","1000");

  // ADL options.
  options.setFlag("trace-mode",          "Toggle the insertion of trace-mode code.\nDefault is ON.");
  options.setFlag("debug-mode",          "Toggle debug support.\nDefault is ON.");
  options.setFlag("syscall-enabled",     "Toggle the insertion of system-call code.  Default is ON.");
  options.setFlag("line-directives",     "Toggle the insertion of line-directives.");
  options.setFlag("show-config", "sc",   "Display configuration information related to ISS generation.");
  options.setFlag("full-path-errors",    "Display full-paths for filename locations in error messages.\n"
                                         "Default is true.");
  options.setFlag("low-mem","lm",        "Enable ADL code generation that requires less memory to compile.");
  options.setFlag("uadl-low-mem",        "Enable uADL code generation that requires less memory to compile.");
  options.setOption("top-level", "tl",   "Specify the top-level element for which to produce an ISS.");
  options.setOption("config", "cf",      "Specify an ISS configuration file.");
  options.setOption("trans-pool-size",   "Specify the maximum number of instruction packets for a transactional ISS.");
  options.setOption("mem-pool-size",     "Specify the maximum number of outstanding memory transactions for the model.");
  options.setOption("jobs","j"           "Specify the number of compile jobs to break the ISS into.");
  options.setOption("log-usage",         "Enable usage logging.  The argument takes the form of program-name:version.  The option\n"
                                         "may be negated to turn it off.  The default is on, where the program name defaults to\n."
                                         "the root of the input-file name and the version is '<year>.<month>.<day>'.");
  options.setOptionalValue("log-usage");  
  options.setFlag("instr-cache",         "Turn on or off instruction cache generation.  The default is OFF.\n"
                                         "This option may be negated to turn off instruction cache generation.");
  options.setOption("instr-cache-size",  "Specify the size of the instruction object cache (number of pages).");
  options.setOption("instr-cache-page-size", "Specify the size of a page (maps to number of instructions,\n"
                                         "based upon instruction size.");
  options.setOption("fn-suffix",         "Add the specified suffix to each generated file.");

  options.setFlag("extern-dmi",          "Toggle support for an external direct-memory-interface cache.");
  options.setOption("extern-dmi-entry-size","Specify the entry size of the external dmi cache in bits, e.g. 7 = 128 byte entries.");
  options.setOption("extern-dmi-size",   "Specify the number of entries in the external dmi cache.");  
  options.setFlag("tlm2-endianness",     "Use address swizzling for endianness handling, rather than byte swapping.");
  options.setFlag("tags-only-caches",    "True => only track tags in the cache and do not store data.\nDefault is false.");
  options.setFlag("mt-support",          "Add necessary features to ensure thread safety.");
  options.setFlag("mt-locked-mem",       "Fully lock memory during multi-threaded simulation.");
}

int main(int argc, const char *argv[])
{
  // Setup usage logging, if appropriate.
  log_usage_at_exit(argc,argv,"tool");

  try {
    AnyOption options(argc, argv);

    // Instal all command-line options.
    setup_cmdline_options(options);

    // Do command-line parsing.
    if (!options.processCommandLine()) {
      return 1;
    }

    if (options.getFlag("h")) {
      options.printUsage();
      return 0;    
    }

    if (options.getArgc() < 2) {
      cerr << "No source files specified.\n";
      options.printUsage();
      return 1;
    }

    const gc_string ADLFilename(options.getArgv(0));
    const gc_string uADLFilename(options.getArgv(1));

    GenConfig config;

    config._fn_suffix    = options.getOption("fn-suffix").c_str();
    config._debugMode    = options.getFlag("debug-mode", config._debugMode);
    config._safeMode     = options.getFlag("safe-mode", config._safeMode);
    config._issMemMode   = options.getFlag("iss-mem-mode", config._issMemMode);
    config._memPoolSize  = options.getOption("mem-pool-size", config._memPoolSize);

    config._ic_enabled   = options.getFlag("instr-cache", config._ic_enabled);
    config._ic_size      = options.getOption("instr-cache-size", config._ic_size);
    config._ic_page_size = options.getOption("instr-cache-page-size", config._ic_page_size);

    // We don't support safe-mode and instruction-caches, since we want the ISS
    // to always be driving the functional part of the simulation.
    if (config._safeMode && config._ic_enabled) {
      cerr << "warning:  The instruction cache is not supported with safe-mode.\n\n";
      config._ic_enabled = false;
    }

    config._vd_enabled   = options.getFlag("instr-class-vd", config._vd_enabled);

    string sc = options.getOption("systemc");
    if (sc.empty()) {
      config._systemc = scNone;
    }
    else if (sc == "method") {
      config._systemc = scMethod;
    }
    else if (sc == "thread") {
      config._systemc = scThread;
    }
    else {
      RError("Unknown type of SystemC interface specified:  " << sc);
    }

    config._extMemMode = options.getFlag("extern-mem", config._extMemMode);
    config._extCycleMem = options.getFlag("extern-cycle-mem", config._extCycleMem);
    config._tracing = options.getFlag("tracing-mode",config._tracing);
    config._sep_ics = options.getFlag("uadl-low-mem",config._sep_ics);
    config._deadlock = options.getOption("detect-deadlock",config._deadlock);
    
    const bool print_data = options.getFlag("print-data",false);
    const bool print_all_data = options.getFlag("print-all-data",false);
    const bool adl_strict = options.getFlag("adl-strict",false);
    const bool uadl_strict = options.getFlag("uadl-strict",false);

    bool check_cr_args = options.getFlag("check-cr-args",false);

    const gc_string basename = options.getOption("output-base", gen_default_name(uADLFilename.c_str(), "")).c_str();
    config._preamble = options.getOption("preamble", "").c_str();
    config._nameSpace = options.getOption("namespace", getUadlNamespace().c_str()).c_str();
    config._basename = basename;

    setDataModelDebug(options.getFlag("data-model-debug",dataModelDebug()));

    if (config._issMemMode && !config._safeMode) {
      RError("Error: The iss-mem-mode option may only be used with safe-mode.");
    }

    // Check ADL description filename.
    if (getExtension(ADLFilename.c_str()) != "adl")
      RError("Error: ADL description file must have .adl suffix\n");

    // Check uADL description filename.
    if (getExtension(uADLFilename.c_str()) != "uadl")
      RError("Error: uADL description file must have .uadl suffix\n");

    // Construct output filenames.
    const gc_string uArchHFilename = basename + ".h";
    const gc_string uArchCFilename = basename + config._fn_suffix + ".cc";
    const gc_string iss_cfilename  = basename + "-iss.cc";
    const gc_string iss_rcfilename = basename + "-iss" + config._fn_suffix + ".cc";

    // Parse ADL description file.
    AdlParser adl_parser;

    // This parses the ADL file and creates the data model.
    bool fullpatherrors = standardParserSetup(adl_parser,options,iss_cfilename.c_str());
    bool unset_strict = false;
    if (adl_strict) {
      unset_strict = true;
      setDataStrictness(true);
    }
    if (!adl_parser.parse(ADLFilename.c_str())) {
      RError("Error:  Could not parse ADL file " << ADLFilename << ".\n");
    }
    if (unset_strict) {
      setDataStrictness(false);
    }

    // Create the configuration database.
    GenDriverIF *gendriver = create_adl_gen(options,adl_parser,iss_rcfilename,iss_cfilename,
                                            uArchHFilename,getIssNamespace(config._nameSpace),
                                            fullpatherrors);

    // Parse uADL description file and create uADL IR.
    UadlDriver driver;
    if (uadl_strict) {
      setDataStrictness(true);
    }

    // Parse custom-resource headers if told to do so or if we're not just
    // dumping the data model.
    if (!(print_data || print_all_data)) {
      check_cr_args = true;
    }

    UadlIr *ir = driver.parse(options,adl_parser.data(),gendriver,uADLFilename,
                              iss_cfilename,uArchHFilename,uArchCFilename,
                              check_cr_args,fullpatherrors);

    if (print_data || print_all_data) {
      driver.dump(cout,print_all_data);
      return 0;
    }

    // Turn off the generation of instruction size checks if the fetch-entry
    // size is the same as the instruction size.
    if (ir->maxInstrSize() == ir->res().fetcher()->entrySize()) {
      gendriver->config().set_instr_size_check(false);
    }
    
    // Create the transactional ISS.  This is done after all parsing so that the
    // ADL data model will have all decls and functions added by uADL.
    miniADLParse(*ir,gendriver,config);

    config._ld_okay = gendriver->config().ld_okay();
    
    if (config._extMemMode && ir->res().memories().size() > 2) {
      RError("Error: This model has " << ir->res().memories().size() << " memories" <<
             ", while external memory mode can only support 2.  No code is " <<
             "generated.\n");
    }

    ofstream outH(uArchHFilename.c_str());
    if (!outH) RError("Error: Cannot write to file " << uArchHFilename << "\n");

    ofstream outC(uArchCFilename.c_str());
    if (!outC) RError("Error: Cannot write to file " << uArchCFilename << "\n");

    // The line-count stream enables us to create line directives by
    // tracking our current line number.
    opfxstream outHpfx(outH.rdbuf(),2,'{','}');
    opfxstream outCpfx(outC.rdbuf(),2,'{','}');

    // Generate microarchitecture model.
    GenCodeIF gencode(*ir, outHpfx, outCpfx, config);

    gencode.generate();
  }
  catch (exception &err) {
    cerr << err.what() << "\n";
    return 1;
  }
  return 0;
}


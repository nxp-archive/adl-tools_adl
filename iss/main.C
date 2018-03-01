//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Entry point for the ISS generation client.
//

#include <iostream>
#include <fstream>
#include <stdexcept>
#include <sstream>

#include "helpers/AnyOption.h"
#include "helpers/Macros.h"
#include "helpers/FilenameUtils.h"
#include "helpers/LogUsage.h"

#include "fe/AdlParser.h"
#include "fe/Data.h"

#include "ConfigDB.h"
#include "GenDriver.h"

#include "adl_config.h"

using namespace std;
using namespace Opencxx;
using namespace adl;

ConfigDB *create_configdb(const AnyOption &options,Data &data)
{
  string conf_name = options.getOption("config");

  ConfigDB *configdb = new ConfigDB;

  // We do want to check aux funcs, since they might have been added for the
  // time-tagged ISSs.
  configdb->parse(conf_name,data,options,true);

  if (options.getFlag("sc")) {
    cout << "Configuration:\n";
    configdb->dump(cout);
  }

  return configdb;
}

// Opens the output file and initiates the model creation process.
void process(const string &input,const AnyOption &options)
{
  // Create the data model, exit if there's a problem.
  AdlParser parser;

  // If the user hasn't specified an output name, then use the
  // input file's base name.
  string output = options.getOption("output");
  if (output.empty()) {
    if (input.rfind(".adl") == (input.size()-4)) {
      output = gen_default_name(input,".cc");
    } else {
      output = input + ".cc";
    }
  }

  standardParserSetup(parser,options,output);

  // This parses the ADL file and creates the data model.
  if (!parser.parse(input)) {
    RError("Could not parse " << input);
  }

  Data &data = parser.data();

  if (options.getFlag("print-data")) {
    data.dump(cout,"");
    return;
  }

  ConfigDB *configdb = create_configdb(options,data);

  auto &preamble = options.getValues("preamble");
  ForEach(preamble,i) {
    configdb->_preamble += gc_string(i->c_str()) + '\n';
  }

  if (options.haveOption("transactional-iss")) {
    configdb->set_iss_type(options.getFlag("transactional-iss") ? tTransactional : tNormal );
  }
  if (options.haveOption("hybrid-iss")) {
    configdb->set_iss_type(options.getFlag("hybrid-iss") ? tHybrid : tNormal );
  }

  // Set defaults for memory-interface, based upon ISS types.

  // If in hybrid mode, then we default to logging-mem.
  if (configdb->iss_type() == tHybrid) {
    configdb->set_mem_if(mLogging);
  }

  // Handle explicit memory-interface overrides.
  if (options.haveOption("blocking-mem")) {
    configdb->set_mem_if(options.getFlag("blocking-mem") ? mBlocking : mNonBlocking );
  }
  if (options.haveOption("logging-mem")) {
    configdb->set_mem_if(options.getFlag("logging-mem") ? mLogging : mNonBlocking );
  }

  configdb->set_dc_support(options.getFlag("dc"));
  configdb->set_jit_support(options.getFlag("jit"));

  bool fast_model = (configdb->jit_support() || configdb->dc_support());

  configdb->set_jit_bcname(options.getOption("jit-bcname"));
  configdb->set_jit_ihname(options.getOption("jit-ihname"));
  configdb->set_track_reads(options.getFlag("track-reads",configdb->track_reads()));
  configdb->set_log_reg_reads(options.getFlag("log-reg-reads",configdb->log_reg_reads()));
  configdb->set_log_reg_masks(options.getFlag("log-reg-masks",configdb->log_reg_masks()));
  configdb->set_log_unarch(options.getFlag("log-unarch",configdb->log_unarch()));
  configdb->set_trace_mode(options.getFlag("trace-mode",configdb->trace_mode()));
  configdb->set_log_instr_args_mode(options.getFlag("log-instr-args",configdb->log_instr_args_mode()));
  configdb->set_debug_mode(options.getFlag("debug-mode",configdb->debug_mode()));
  configdb->set_syscall_enabled(options.getFlag("syscall-enabled",configdb->syscall_enabled()));

  if (options.haveOption("icache-touch")) {
    configdb->set_icache_touch(options.getFlag("icache-touch") ? tTrue : tFalse);
  }

  configdb->set_rnumber_support(options.getFlag("rnumber",configdb->rnumber_support()));
  configdb->set_extern_mem(options.getFlag("extern-mem",configdb->extern_mem()));
  configdb->set_extern_dmi(options.getFlag("extern-dmi",configdb->extern_dmi()));
  configdb->set_extern_dmi_entry_size(options.getOption("extern-dmi-entry-size",configdb->extern_dmi_entry_size()));
  configdb->set_extern_dmi_size(options.getOption("extern-dmi-size",configdb->extern_dmi_size()));
  configdb->set_time_tagged(options.getFlag("time-tagged",configdb->time_tagged()));
  configdb->set_ld_okay(options.getFlag("line-directives",configdb->ld_okay()));
  configdb->set_verbose_mode(options.getOption("verbose",configdb->verbose_mode()));
  configdb->set_trans_pool_size(options.getOption("trans-pool-size",configdb->trans_pool_size()));
  configdb->set_iss_namespace(options.getOption("iss-namespace",configdb->iss_namespace_str()));
  configdb->set_num_compile_jobs(options.getOption("jobs",configdb->num_compile_jobs()));
  configdb->set_low_mem(options.getFlag("low-mem",configdb->low_mem()));
  configdb->set_gen_instr_attr(options.getFlag("instr-info",configdb->gen_instr_attr()));
  configdb->set_disassembler(options.getFlag("disassembler",configdb->disassembler()));
  configdb->set_library_mode(options.getFlag("lib-mode",configdb->library_mode()));
  configdb->set_mt_support(options.getFlag("mt-support",configdb->mt_support()));
  configdb->set_mt_rw_locked_mem(options.getFlag("mt-rw-locked-mem",configdb->mt_rw_locked_mem()));
  configdb->set_mt_locked_mem(options.getFlag("mt-locked-mem",configdb->mt_locked_mem()));
  configdb->set_log_td_commit(options.getFlag("log-td-commit",configdb->log_td_commit()));

  configdb->set_decode_rtype(options.getOption("decode-rtype"));
  configdb->set_decode_argtype(options.getOption("decode-arg-type"));
  configdb->set_decode_arg(options.getOption("decode-arg"));
  configdb->set_decode_rpfx(options.getOption("decode-rpfx"));

  configdb->set_event_bus_send(options.getFlag("event-bus-send",configdb->event_bus_send()));
  configdb->set_extern_event_bus(options.getFlag("extern-event-bus",configdb->extern_event_bus()));
  if (!configdb->event_bus_send() && configdb->extern_event_bus()) {
    RError("You may not use --extern-event-bus and --no-event-bus-send at the same time.  The options are mutually exclusiove.");
  }

  if (options.haveOption("cpp-cmd")) {
    const auto &cmds = options.getValues("cpp-cmd");
    ForEach(cmds,i) {
      configdb->get_cpp_cmds().push_back(i->c_str());
    }
  }
  
  configdb->set_graphite_support(options.getFlag("graphite"));

  if (configdb->graphite_support()) {
    if (configdb->time_tagged()) {
      cout << "The Graphite performance model integration is not compatible with a time-tagged ISS.\n"
           << "Disabling time-tagged ISS creation in favor of Graphite.\n";
      configdb->set_time_tagged(false);
    }
  }

  bool mt_model = false;
  if (configdb->mt_support()) {
    mt_model = true;
  }

  bool dmi_cache_default = (fast_model) ? true : configdb->dmi_cache_support();
  configdb->set_dmi_cache_support(options.getFlag("dmi-cache",dmi_cache_default));

  // We want the TD api to be on by default for MT or fast models, since for
  // non-MT fast models it's used for time-slicing MP sims and for MT models,
  // it's used for stopping other cores.
  bool td_api_default = (fast_model || mt_model) ? true : configdb->td_api();
  configdb->set_td_api(options.getFlag("td-api",td_api_default));

  configdb->set_dep_tracking(options.getFlag("dep-tracking",configdb->dep_tracking()));
  configdb->set_log_usage(options.getOption("log-usage"));
  configdb->set_ignore_mode(options.getFlag("ignore-mode"));
  configdb->set_skip_caches(options.getFlag("skip-caches"));
  configdb->set_tags_only_caches(options.getFlag("tags-only-caches"));
  configdb->set_log_writereg(options.getFlag("log-writeregs",configdb->log_writereg()));
  configdb->set_incl_unused_helpers(options.getFlag("incl-unused-helpers",configdb->incl_unused_helpers()));
  configdb->set_tlm2_endianness(options.getFlag("tlm2-endianness",configdb->tlm2_endianness()));
  configdb->set_per_instr_td_incr(options.getFlag("per-instr-td-incr",configdb->per_instr_td_incr()));

  if (configdb->iss_type() == tHybrid || configdb->iss_type() == tNormal) {
    configdb->set_rollback_mode(options.getFlag("rollback-mode",configdb->rollback_mode()));
  }

  if (configdb->jit_support()) {
    if (configdb->jit_bcname().empty()) {
      configdb->set_jit_bcname(gen_default_name(output,".bc"));
    }
    if (configdb->jit_ihname().empty()) {
      configdb->set_jit_ihname(dir_name(output)+"/"+gen_default_name(output,"-jit.cc"));
    }
  }

  string tlname = options.getOption("tl");
  
  GenDriverIF driver(data,tlname.c_str(),output.c_str(),"",options,*configdb);
  driver.generate();
}

int main(int argc, const char** argv)
{
  // Setup usage logging, if appropriate.
  log_usage_at_exit(argc,argv,"tool");
  try {
    AnyOption options(argc,argv);

    standardParserOptionSetup(options);
    options.addUsage(":  [options] <source>");
    options.setFlag("help","h",            "Display help.");
    options.setFlag("print-data",          "Print the data model.");
    options.setFlag("trace-mode",          "Toggle the insertion of trace-mode code.  Default is ON.");
    options.setFlag("debug-mode",          "Toggle debug support.  Default is ON.");
    options.setFlag("line-directives",     "Toggle the insertion of line-directives.");
    options.setFlag("time-tagged","tt",    "Generate a time-tagged ISS.");
    options.setFlag("show-config","sc",    "Display configuration information related to ISS generation.");
    options.setFlag("rnumber",             "Toggle support for rnumber (dynamic arbitrary-sized integer support for ISS debug).");
    options.setFlag("dmi-cache",           "Toggle support for a DMI (direct-memory interface) cache for improved \n"
                                           "memory performance with a trace cache.\nDefault is false.");
    options.setFlag("icache-touch",        "Toggle support for icache touching for JIT models.\nDefault is true.");
    options.setFlag("transactional-iss",   "Toggle support for generating a transactional ISS.\nDefault is false.");
    options.setFlag("hybrid-iss",          "Toggle support for generating a hybrid transactional ISS.\nDefault is false.");
    options.setFlag("blocking-mem",        "Toggle support for generating a blocking-memory interface for non-normal-mode ISSs.\nDefault is false.");
    options.setFlag("logging-mem",         "Toggle support for generating a logging-memory interface for non-normal-mode ISSs.\nDefault is false.");
    options.setFlag("skip-caches",         "True => skip caches in the memory hierarchy.  Caches data structures and support functions are still generated.\nDefault is false.");
    options.setFlag("tags-only-caches",    "True => only track tags in the cache and do not store data.\nDefault is false.");
    options.setFlag("track-reads",         "Toggle support for tracking read information in a hybrid or transactional ISS.\nDefault is false.");
    options.setFlag("log-reg-reads",       "Log all register-read accesses.  Default is false.");
    options.setFlag("log-reg-masks",       "Log partial masks for register read/write operations.  Default is false.");
    options.setFlag("log-unarch",          "Log unarchitected resources (registers and exceptions with the 'unarchitected' attribute).  Default is true.");
    options.setFlag("rollback-mode",       "Toggle support for restoring state after an instruction is executed.\n"
                                           "Only relevant for normal or hybrid-ISS modes.  Default is false.");
    options.setFlag("ignore-mode",         "Toggle support for being able to ignore register and memory updates via the ignore API.\n");
    options.setFlag("extern-mem","em",     "Toggle support for global memory being defined externally.\nThe default is false.");
    options.setFlag("syscall-enabled",     "Toggle the insertion of system-call code.  Default is ON.");
    options.setFlag("log-instr-args",      "Toggle the insertion of instruction arguments logging code.  Default is OFF.");
    options.setFlag("low-mem","lm",        "Enable code generation that requires less memory to compile.");
    options.setFlag("instr-info",          "Generate instructions' attribute information in tranasctional ISS.\nDefault is false.");
    options.setFlag("disassembler",        "Generate a disassembler function in the model.\nDefault is false.");
    options.setFlag("lib-mode",            "Generate a model for use by an external driver.  The model does not have most\n"
                                           "simulation support functions and register data is stored externally.  Default is false.");
    options.setFlag("event-bus-send",      "Enable event buses broadcasting their data to all other cores in the system.  Default is true.");
    options.setFlag("extern-event-bus",    "Enable event buses broadcasting via an installed handler.  Default is true.");
    options.setOption("verbose",           "Enable verbose mode.  May be given an integer value for a verbose-level.  Default is 0.");
    options.setOptionalValue("verbose","1");
    options.setOption("preamble",          "Add a preamble string, which is printed to stderr when the model starts.\nThis option may be repeated.");
    options.setOption("output","o",        "Specify output filename.");
    options.setOption("top-level","tl",    "Specify the top-level element to produce an ISS for.");
    options.setOption("config","cf",       "Specify an ISS configuration file.");
    options.setOption("trans-pool-size",   "Specify the maximum number of instruction packets for a transactional ISS.");
    options.setOption("iss-namespace",     "Specify a different namespace for wrapping the generated ISS.");
    options.setOption("jobs","j"           "Specify the number of compile jobs to break the ISS into.");

    options.setFlag("extern-dmi",          "Toggle support for an external direct-memory-interface cache.");
    options.setOption("extern-dmi-entry-size","Specify the entry size of the external dmi cache in bits, e.g. 7 = 128 byte entries.");
    options.setOption("extern-dmi-size",   "Specify the number of entries in the external dmi cache.");

    options.setOption("log-usage",         "Enable usage logging.  The argument takes the form of program-name:version.  The option\n"
                                           "may be negated to turn it off.  The default is on, where the program name defaults to\n."
                                           "the root of the input-file name and the version is '<year>.<month>.<day>'.");
    options.setOptionalValue("log-usage");

    options.setFlag("td-api",              "Add support for temporal decoupling, which allows a model to execute extra instructions within\n"
                                           "a platform model as long as they are performed within a certain time quantum.  This option may\n"
                                           "be negated.  The default is OFF.");
    options.setFlag("dep-tracking",        "Add support for data-dependency trackling.  The default is false.");

    options.setFlag("log-writeregs",       "Turn on logging during external register modifications (writeReg calls).  The default is true.");
    options.setFlag("incl-unused-helpers", "Include unused helper functions.  Default is false.");

    options.setFlag("tlm2-endianness",     "Use address swizzling for endianness handling, rather than byte swapping.");

    options.setFlag("per-instr-td-incr",   "Enable per-instruction temporal-decoupling counter increments for decode-cache and JIT models.\n"
                                           "False means that increments are grouped at the end of a basic block.  If true, then fidelity is higher\n"
                                           "at a cost of some performance.  Default is true.");

    options.setFlag("mt-support",          "Add necessary features to ensure thread safety.");
    options.setFlag("mt-locked-mem",       "Fully lock memory during multi-threaded simulation.");

    options.setFlag("decode-cache","dc",   "Turn on decode-cache model generation.");

    options.setFlag("jit",                 "Turn on JIT model generation.");
    options.setOption("jit-ihname",        "Specify the JIT instruction handler file name.");
    options.setOption("jit-bcname",        "Specify the JIT LLVM bitcode file name to be used by the model.");

    options.setFlag("graphite",            "Generate a model with support for a Graphite performance model.");

    options.setFlag("log-td-commit",       "Log instruction completion times using the temporal-decoupling counter value.");  

    options.setOption("decode-arg-type",   "Specify decode-function argument type for hybrid ISSs.\n");
    options.setOption("decode-arg",        "Specify decode-function argument name for hybrid ISSs.\n");
    options.setOption("decode-rtype",      "Specify decode-function return type for hybrid ISSs.\n");
    options.setOption("decode-rpfx",       "Specify decode-function packet prefix for hybrid ISSs.\n");

    options.setFlag("gen-icache-touch",    "Always generate an icache touch operation for JIT/decode-cache models, even if no cache is present.");

    options.setOption("cpp-cmd",           "Specify an additional C preprocessor directive to be added to the top of the generated C++ code.\n"
                                           "By default, the string is taken to be an include filename, but if it starts with 'define:' then it is\n"
                                           "added as a #define (with the prefix removed).\n");

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

    string input = options.getArgv(0);

    process(input,options);
  }
  catch (exception &err) {
    cerr << "Error:  " << err.what() << "\n";
    return 1;
  }

  return 0;
}

//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#include <iostream>
#include <fstream>
#include <stdexcept>

#include "helpers/pfxstream.h"
#include "helpers/AnyOption.h"
#include "helpers/Macros.h"
#include "helpers/FilenameUtils.h"

#include "fe/AdlParser.h"
#include "fe/Data.h"

#include "iss/GenInstrPacket.h"
#include "iss/ConfigDB.h"
#include "iss/GenDriver.h"
#include "iss/MiscTypes.h"

#include "UadlParser.h"
#include "UadlCommon.h"
#include "GenCode.h"

using namespace std;
using namespace Opencxx;
using namespace adl;

namespace uadl {

  GenDriverIF * create_adl_gen(const AnyOption &options, AdlParser &parser,
                               const gc_string &outFilename,const gc_string &realOutFilename,
                               const gc_string &uadlHFilename,const gc_string &ISSnamespace,
                               bool fullpatherrors)
  {
    string conf_name = options.getOption("config");

    ConfigDB *configdb = new ConfigDB;
    // FIXME: magic number 80
    configdb->set_coro_pool_size(80);
    configdb->set_trans_pool_size(80);
    configdb->parse(conf_name, parser.data(), options);

    // The uADL model will take care of logging instruction fetch info.
    configdb->set_log_instr_reads(false);

    // We don't omit helper functions which are considered unused.  Hack, since
    // this is uADL1 and I don't feel like figuring out what functions are used,
    // propagating that data, etc.
    configdb->set_incl_unused_helpers(true);

    // We want exceptions to be rethrown so that the state action functions can
    // catch and process them.
    configdb->set_trans_rethrow(true);

    auto preamble =  options.getValues("preamble");
    ForEach(preamble, i) { configdb->_preamble += gc_string(i->c_str()) + '\n'; }
  
    const bool safeMode      = options.getFlag("safe-mode", false);
    const bool issMemMode    = options.getFlag("iss-mem-mode", false);

    configdb->set_iss_namespace(ISSnamespace);
    configdb->set_trace_mode(options.getFlag("trace-mode",
                                             configdb->trace_mode()));
    configdb->set_debug_mode(options.getFlag("debug-mode",
                                             configdb->debug_mode()));
    configdb->set_syscall_enabled(options.getFlag("syscall-enabled",
                                                  configdb->syscall_enabled()));
    configdb->set_rnumber_support(options.getFlag("rnumber",
                                                  configdb->rnumber_support()));
    configdb->set_extern_mem(options.getFlag("extern-mem",
                                             configdb->extern_mem()));
    configdb->set_time_tagged(options.getFlag("time-tagged",
                                              configdb->time_tagged()));
    configdb->set_ld_okay(options.getFlag("line-directives",
                                          configdb->ld_okay()));
    configdb->set_verbose_mode(options.getFlag("verbose",
                                               configdb->verbose_mode()));
    configdb->set_trans_pool_size(options.getOption("trans-pool-size",
                                                    configdb->trans_pool_size()));

    configdb->set_num_compile_jobs(options.getOption("jobs",configdb->num_compile_jobs()));

    configdb->set_low_mem(options.getFlag("low-mem",configdb->low_mem()));

    configdb->set_verbose_mode(options.getOption("verbose",configdb->verbose_mode()));

    configdb->set_disassembler(options.getFlag("disassembler",configdb->disassembler()));

    configdb->set_log_usage(options.getOption("log-usage"));

    configdb->set_mt_support(options.getFlag("mt-support",configdb->mt_support()));
    configdb->set_mt_locked_mem(options.getFlag("mt-locked-mem",configdb->mt_locked_mem()));

    configdb->set_extern_dmi(options.getFlag("extern-dmi",configdb->extern_dmi()));
    configdb->set_extern_dmi_entry_size(options.getOption("extern-dmi-entry-size",configdb->extern_dmi_entry_size()));
    configdb->set_extern_dmi_size(options.getOption("extern-dmi-size",configdb->extern_dmi_size()));

    configdb->set_tlm2_endianness(options.getFlag("tlm2-endianness",configdb->tlm2_endianness()));

    configdb->set_extern_event_bus(options.getFlag("extern-event-bus",configdb->extern_event_bus()));

    configdb->set_tags_only_caches(options.getFlag("tags-only-caches"));

    // We manage the current-instruction-address register ourselves, rather than
    // have it done by the prefetch routine.
    configdb->set_prefetch_cia_incr(false);

    // We always want level-sensitive exception support in order to be able to
    // handle external exceptions.
    configdb->set_add_level_sensitive_support(true);

    // We don't want pending exceptions to be cleared, because we model external
    // exceptions as level-sensitive.  Rather than have our own, extra mask, we
    // just have a single mask in the ISS.
    configdb->set_clear_pending_excepts(false);

    // We turn off register aliasing resolving except for sparse register files,
    // since for uADL, hooks evaluate at operand read/commit time, and thus we
    // don't care about them.
    configdb->set_all_reg_aliases(false);

    // The ISS will actually return an instruction class object obtained from the
    // uADL class.  so we set the appropriate flags here in order to do that.
    configdb->get_cpp_cmds().push_back("define:UADL_HEADER");
    configdb->get_cpp_cmds().push_back(gc_string("include:\"") + base_name(uadlHFilename.c_str()).c_str() + "\"");  

    // The fetch unit handles misaligned accesses, so we only need one possible
    // instruction fetch access.  This is really only relevant for normal-mode
    // models.
    configdb->set_trans_ifetch_accesses(1);

    // Our fetches are never split- we always read contiguously from the fetch buffer.
    configdb->set_trans_split_fetch(false);
     
    if (safeMode) {
      // In safe-mode, use a hyrid ISS with the logging-memory interface
      // (default for hybrid).  Skip caches, since all cache management is done
      // by the uadl model, unless we're in --iss-mem-mode, in which case we do
      // want the ISS to do cache management.  In that case, uADL uses its own
      // performance caches for calculating latency.
      configdb->set_iss_type(tHybrid);
      configdb->set_mem_if(mLogging);
      configdb->set_skip_caches(!issMemMode);
      configdb->set_log_fetches(!issMemMode);
    } else {
      // In normal mode, use a transactional ISS with a non-blocking
      // (co-routine) interface.  Caches are still managed by the performance
      // model, but we don't need to set anything for that to happen.
      configdb->set_iss_type(tTransactional);
      configdb->set_mem_if(mNonBlocking);
    }

    const gc_string tlname = options.getOption("tl").c_str();
    GenDriverIF *driver = new GenDriverIF(parser.data(), tlname, outFilename,
                                          realOutFilename, options, *configdb);

    // We need to do slot allocations *before* parsing the uADL model so that we
    // can get all usage information, including extra registers, such as those
    // used for MMU communication.
    driver->doSlotAllocations();

    return driver;
  }

  // If we have value queries, then in safe-mode we need to make sure that the
  // packet is generated with values and that the relevant instructions read
  // their operands.  Also, set the memory split size based upon the data width
  // of the top-level memory.
  void checkValueQueries(const UadlIr &ir,CoreConfig &coreconfig,bool safeMode)
  {
    if (ir.has_value_queries()) {
      coreconfig.set_need_packet_values(true);
    }

    if (auto m = ir.firstLevelDataMem()) {
      // Basically, 32-bit width or less results on splitting on 32-bit
      // boundaries, otherwise split on 64-bit boundaries.
      if (m->dataWidth() <= 32) {
        coreconfig.set_mem_split_size( 4 );
      } else {
        coreconfig.set_mem_split_size( 8 );
      }
    }

    ForEach(ir.instClasses(),ic_iter) {
      const InstClass &ic = *ic_iter->second;
      if (has_value_query(ic)) {
        ForEach(ic.instrs(),i) {
          coreconfig.readOpInstrs().insert(i->_name);
        }
      }
    }
  }

  // Opens the output file and initiates the model creation process.
  void miniADLParse(const UadlIr &ir,GenDriverIF *driver,GenConfig &config)
  {
    ConfigDB &configdb = driver->config();

    // Construct a path to the core (might have hierarchy if we're dealing with
    // a multi-threaded core).
    Path p;
    for (const Thread *t = &ir.top(); t->is_group(); t = t->threads().begin()->second) {
      p.push_back(t->item().name());
    }
    CoreConfig &coreconfig = configdb.makeCoreConfig(p,ir.core());

    gc_string uarch = getuArchClassName(ir);
    configdb.set_decode_rtype(config._nameSpace+"::"+getInstBaseName(ir) + " *");
    configdb.set_decode_argtype(config._nameSpace+"::"+ uarch + " &");
    configdb.set_decode_arg(gc_string("uarch"));
    configdb.set_decode_rpfx(gc_string("uarch.get_instr_"));
    if (config._ic_enabled) {
      // If we're using the instruction-object cache, then we don't want to use
      // a pool-based allocator for instruction packets, since we store the
      // cached instruction packet with the instruction object.
      configdb.set_trans_pool_size(0);
    }

    // We'll generate implementations, rather than using the virtual interface.
    coreconfig.set_virtual_mem_interface(false);

    // Ignore latency information unless we're in iss-mem-mode.
    if (config._safeMode && !config._issMemMode) {
      coreconfig.set_ignore_latency(true);
    }

    // We only need the branch-hook if we're in safe-mode.
    configdb.set_add_branch_hook(config._safeMode);

    // We'll need a pointer to the parent uarch object for those implementations.
    coreconfig.get_extra_members().push_back(config._nameSpace+"::"+uarch + " *_uarch");
    coreconfig.get_extra_inits().push_back("_uarch(0)");

    // If we set up tags only caches, then uADL needs to know about it.  We
    // currently only support this for safe-mode models, but we should extend it
    // to normal mode models as well.
    if (config._safeMode) {
      config._tagsOnlyCaches = configdb.tags_only_caches();
    }

    if (coreconfig.post_sim()) {
      config._postSimHook = true;
    }

    checkValueQueries(ir,coreconfig,config._safeMode);

    driver->generate();
  }

  void setDataStrictness(bool s)
  {
    Data::setStrict(s);
  }  

}

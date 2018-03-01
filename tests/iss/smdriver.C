//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// A simple driver used to test a safe-mode transactional ISS.
//
// In order to use this with a generated transactional ISS, several defines must be set:
// 
//  MODEL:  Set to the quoted name of the include file, e.g. "mod1.cc.h"
//
//  MNAME:  The model's name- should be a quoted string, e.g. "Zen".
//
//  MTYPE:  The name of the model's type to instantiate, e.g. Zen.
//
//  Hybrid:    If defined, then we expect for the model to be compiled as a hybrid transactional ISS.
//
// Additional defines:
//
//  RestoreStart: Instruction ID in which to start saving instructions for a
//  restore.
//
//  RestoreStop: Instruction ID which indicates that saved instructions should
//  be used to restore state.  May be the same ID as RestoreStart.
//
//  LogOrigMem: Compile in support for original-memory-access logging.

#include <iostream>

struct Parent {
  Parent() : _opc(0) {};
  
  void signalFetchException() {};
  void signalException(void *) {};
  void set_cur_instr(void *instr_p);
  void decode_update(bool,unsigned) {};

  uint32_t _opc;
};

#include MODEL

#include "helpers/AnyOption.h"
#include "helpers/Macros.h"

#include "iss/SimInterface.h"

using namespace std;
using namespace adliss;

ttime_t systime;
unsigned id;

bool Trace     = false;
bool LogMem    = false;
bool LogFetch  = false;
bool LogOrig   = false;
bool LogInsts  = false;

vector<adl::RestoreStack> Restores;  // For saving restore data.
bool Saving    = false;              // True => We're saving restore data onto Restores.
bool IgnoreSv  = false;              // Used to ignore the current instruction,
                                     // so that restore start/stop can be the
                                     // same instruction.

#if defined(RestoreStart) && defined(RestoreStop)
#  define Rollback
#endif


// With a blocking memory interface, we must derive a class from the basic model
// type in order to implement the handler.
typedef MTYPE BaseModelType;
struct ModelType : public BaseModelType, Parent {

  ModelType(const string &n, const IssNode *p, unsigned &id,unsigned child_id,const ttime_t *t) : 
    BaseModelType(n,p,id,child_id,t),
    _ic(0)
  {};

  virtual void handle_memory_request(u_int memId, const MemAccess &ma) 
  {
    if (LogMem) {
      if (ma._type == CacheIFetch) {
        cout << "Instr Fetch:  " << ma << "\n";
      } else {
        cout << "Instr 0x" << std::hex << _opc << std::dec << ":  Mem:  " << ma;
#       ifdef LogOrigMem
        if (LogOrig) {
          cout << "  Original Addr:  " << std::hex << get_orig_access_addr() 
               << "  Original Size:  " << std::dec << get_orig_access_size();
      }
#       endif
        cout << "\n";
      }
    }
  };

  unsigned _ic;
};

void Parent::set_cur_instr(void *instr_p)
{
  _opc = ((ModelType::InstrPacket*)(instr_p))->opc()[0];
}


ModelType core(MNAME,0,id,0,&systime);

#ifdef Hybrid
# define DecodeArg ,core
#else
# define DecodeArg
#endif

ModelType::InstrPacket *fetch()
{
  core.logger().log_current_instr(0);
# ifdef PRE_CYCLE
  core.pre_cycle_handler();
# endif  

  // Now proceed with execution.
  addr_t ra = 0;
  core.prefetch<4,false>(0,ra);
  if (LogFetch) {
    MemAccess ma(core.getProgramCounter(),core.getProgramCounter(),4,4,CacheIFetch);
    cout << "Instr Fetch:  " << ma << "\n";
  }
  if (ModelType::InstrPacket *instr = core.decode(ra,4,false DecodeArg)) {
    // log instruction issue time
    if (Trace) core.logger().log_instr_issue_time(systime);

    // We set the instruction's exception state to the core's current exception state
    // so that they can all be handled at once.
    instr->setExceptionState(core.getExceptionFlag());

#     ifdef Rollback
    if (instr->id() == RestoreStart && !Saving) {
      core.enable_rollback(true);
      Saving = true;
      IgnoreSv = true;
    }
#     endif
    return instr;
  }
  else {
    core.commit_exceptions();
    return 0;
  }
  return 0;
}

#ifndef Hybrid
void exec(ModelType::InstrPacket *instr)
{
  MemAccess::OpType op;
  core._opc = instr->opc()[0];
  while ((op = instr->exec(core)) != MemAccess::opNone) {
  }
# ifdef POST_CYCLE
  core.post_cycle_handler();
# endif  
}
#endif

#ifdef LogInstrFields
const string &getFieldName(int field_id)
{
  const adl::IndexMap &field_data = core.getInstrFieldData();
  ForEach (field_data,i) {
    if (i->second == field_id) {
      return i->first;
    }
  }
  static string unknown("<unknown-field>");
  return unknown;
}
#endif

void commit(ModelType::InstrPacket *instr)
{
# ifndef Hybrid
  for (unsigned i = 0; i != instr->getNumOps(); ++i) {
    instr->commit(core,i);
  }
# else
  if (LogInsts) {
    cout << "Instr:  ";
    // We use this form so that we can test our ability to resolve aliases and
    // sparse-register indirection.
    printInstrPacketResolved(cout,*instr);
    cout << '\n';
#   ifdef LogInstrFields
    // If turned on, then the model must be generated with --instr-info.
    cout << "  Fields: ";
    for (unsigned i = 0; i != instr->getNumOps(); ++i) {
      if (instr->getOp(i)._type != OpInfo::opNone) {
        int field_id;
        if ((field_id = instr->field_info(i)) >= 0) {
          cout << " " << getFieldName(field_id);
        }
      }
    }
    cout << '\n';
#   endif
  }
# endif
  instr->commit_exceptions(core);

  // Flush any delayed commits (if applicable).
  core.empty_commit_queue();

# ifdef Rollback
  if (Saving) {
    Restores.push_back(core.get_rollback_data());
  }
# endif

# ifdef Rollback
  if (instr->id() == RestoreStop) {
    if (IgnoreSv) {
      IgnoreSv = false;
    } else {
      core.enable_rollback(false);
      Saving = false;
      addr_t nia = core.NIA_read().uint64();
      while (!Restores.empty()) {
        Restores.back().commit();
        Restores.pop_back();
      }
      core.NIA_write(nia);
    }
  }
# endif

# ifdef ParallelExecution
  core.packetPositionReset();
# endif

  core.free_packet(instr);

}

#ifndef Hybrid

void read_operands(ModelType::InstrPacket *instr)
{
  for (unsigned i = 0; i != instr->getNumOps(); ++i) {
    instr->read_operand(core,i);
  }
  if (LogInsts) {
    cout << "Instr:  " << *instr << '\n';
  }
}

#endif

// Main execution loop for non-pipelined execution.
void non_pipelined_exec()
{
  while (core.is_active()) {
    try {
      if (ModelType::InstrPacket *x = fetch()) {
        unsigned ed ATTRIBUTE_UNUSED = x->_exdata;
#       ifndef Hybrid
        read_operands(x);
        if (!x->_exdata != ed) {
          // Don't execute if exceptions were generated during the operand-read
          // process.
          exec(x);
        }
#       endif
        commit(x);
      }
    }
    catch (ModelType::Exception e) {
      // Got a fetch exception.
      core.commit_exceptions();
    }
  }
}

int main(int argc,const char *argv[])
{
  cout << "Model:  " << MNAME << "\n\n";

  try {
    options().addUsage(":  [options] <dat file>");
    options().setFlag  ("help","h",    "Display help.");
    options().setFlag  ("trace","t",   "Enable tracing.");
    options().setFlag  ("lax","l",     "Lax mode.");
    options().setFlag  ("log-mem",     "Log all memory accesses.");
    options().setFlag  ("log-fetch",   "Log fetches.");
    options().setFlag  ("log-orig-mem","Log data on top-level, original memory accesses.");
    options().setFlag  ("log-instr",   "Log all instructions.");
    options().setOption("output","o",  "Specify an output file.");
    options().useCommandArgs(argc,argv);
    if (argc < 2 || !options().processCommandLine()) {
      throw runtime_error("Bad command-line");
    }

    Trace     = options().getFlag("trace",Trace);
    LogMem    = options().getFlag("log-mem",LogMem);
    LogFetch  = options().getFlag("log-fetch",LogFetch);
    LogOrig   = options().getFlag("log-orig-mem",LogOrig);
    LogInsts  = options().getFlag("log-instr",LogInsts);
    LaxMode   = options().getFlag("lax",LaxMode);

    if (options().getFlag("h")) {
      options().printUsage();
      return 0;
    };
  
    if (const char *n = options().getArgv(0)) {
      readInput(&core,n);
    } else {
      throw runtime_error("No input file specified.");
    }

    installLoggers(&core);
    WriterPtr w = createWriter(core,options().getOption("o"));
    addWriter(w);

    if (Trace) {
      startLogging(core);
    } else {
      dumpInitialState();
    }

    non_pipelined_exec();

    if (Trace) {
      stopLogging(core);
    } else {
      dumpFinalState();
    }
    removeAllWriters();
    cout << "Instructions issued:  " << core.instr_count() << "\n\n";
  }
  catch (exception &err) {
    cout << err.what() << "\n";
    options().printUsage();
    return 1;
  }
}

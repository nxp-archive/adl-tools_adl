//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// A simple driver used to test a transactional ISS.
//
// In order to use this with a generated transactional ISS, several defines must be set:
// 
//  MODEL:  Set to the quoted name of the include file, e.g. "mod1.cc.h"
//
//  MNAME:  The model's name- should be a quoted string, e.g. "Zen".
//
//  MTYPE:  The name of the model's type to instantiate, e.g. Zen.
//
//  EnableL1:  If set, then L1 cache logic will be compiled.  Defaults to 0 (off).
//             A split and unified cache configurations are supported.  The name of
//             the L1 instruction cache should be set using L1InstrName and defaults to L1i.
//             Likewise, the L1 data cache is specified using L1DataName and defaults to L1d.            
//
//  EnableL2:  If set, then L2 cache logic will be compiled.  Defaults to 0 (off).
//             A unified cache configuration is expected, named L2.
//

#include <iostream>
#include <deque>

#include MODEL

#include "helpers/AnyOption.h"
#include "helpers/Macros.h"

#include "iss/SimInterface.h"

#ifndef EnableL1
#  define EnableL1 0
#endif

#ifndef EnableL2
#  define EnableL2 0
#endif

#ifndef L1InstrName
#  define L1InstrName L1i
#endif

#ifndef L1DataName
#  define L1DataName L1d
#endif

// Two levels of macro expansion are required due to the behavior of macro
// arguments with concatenation and stringification: The argument prescan does
// not occur with concat/stringification operations, so you have to addd an
// extra layer of macro to ensure expansion.
#define MkCacheOpN(x,y) x ## _ ## y
#define MkCacheOp(x,y) MkCacheOpN(x,y)

#define L1iOp(x) MkCacheOp(L1InstrName,x)
#define L1dOp(x) MkCacheOp(L1DataName,x)

using namespace std;
using namespace adliss;

ttime_t systime;
unsigned id = 0;

void do_mem_stuff(MemAccess::OpType op,const MemAccess &ma,uint32_t opc);

// With a blocking memory interface, we must derive a class from the basic model
// type in order to implement the handler.
typedef MTYPE BaseModelType;
struct ModelType : public BaseModelType {

  ModelType(const string &n, const IssNode *p, unsigned &id, unsigned child_id, const ttime_t *t) : 
    BaseModelType(n,p,id,child_id,t),
    _opc(0)
  {};

  virtual void do_memory_operation() 
  {
    const MemAccess &ma = mem_access();
    do_mem_stuff(ma._op,ma,_opc);
    remove_mem_access();
  };

  virtual void handle_memory_request(u_int memId, const MemAccess &ma) 
  {
    _accesses.push_back(ma);
  };

  const MemAccess &mem_access()
  {
    assert (!_accesses.empty());
    return _accesses.front();
  }

  void remove_mem_access()
  {
    _accesses.pop_front();
  }

  deque<MemAccess> _accesses;
  uint32_t          _opc;
};



ModelType core(MNAME,0,id,0,&systime);

bool Trace     = false;
bool LogMem    = false;
bool LogBr     = false;
bool LogInsts  = false;
bool Pipelined = false;

#if EnableL2 != 0

bool setup_l2_access(const CacheStatus &cs,CacheAccess l2_start_ca,CacheAccess l2_end_ca,int &l2_set,int &l2_way,
                     addr_t addr,bool direct = false)
{
  CacheAction l2_action;
  bool replace;

  if (core.L2_cache_begin(l2_start_ca,l2_action,cs,l2_set,l2_way,addr,replace,
                          direct))
  {
    switch (l2_action) {
    case CacheEvict:
      // Evicting a line:  Move data from cache to memory.
      core.L2_cache_write_to_next(l2_start_ca,l2_set,l2_way,0,0,addr);
      if (direct && (l2_start_ca == CacheFlush || l2_start_ca == CacheStore)) {
        // We're just flushing out a line, rather than doing a real eviction, so
        // we don't want to do a load after this.
        break;
      }
    case CacheMiss:
      // Miss (and evict):  Move data from memory to cache.
      core.L2_cache_read_from_mem(l2_start_ca,l2_set,l2_way,addr);
    default:
      break;
    }
    core.L2_cache_end(l2_end_ca,l2_action,l2_set,l2_way,addr,direct);
    return true;
  } else {
    return false;
  }
}

#endif

void setup_instr_mem_access(const CacheStatus &cs,const MemAccess &ma,int seq)
{
# if EnableL1 != 0
  // We have a cache, so we must do the cache data movement ourselves.
  int l1_set,l1_way;
  bool replace;
  CacheAction l1_action;

  if (core. L1iOp(cache_begin)(ma._type,l1_action,cs,l1_set,l1_way,ma._ra,replace)) {
    switch (l1_action) {
    case CacheEvict: {
      addr_t evict_tag = core.L1iOp(cache_get_old_tag)(l1_set,l1_way);
      // Evicting a line: Move data from cache to memory.  In terms of the final
      // access, we always consider this to be a write, since we're moving data
      // to the L2.
#     if EnableL2 != 0
      int l2_set,l2_way;
      if (setup_l2_access(cs,ma._type,CacheWrite,l2_set,l2_way,evict_tag)) {
        // Hit/miss/evict:  Move data from L2 cache to L1 cache.
        core.L1iOp(cache_write_to_next)(ma._type,l1_set,l1_way,l2_set,l2_way,evict_tag);
      }
#     else
        // If no L2, then dump directly to memory.
      core.L1dOp(cache_write_to_next)(ma._type,l1_set,l1_way,0,0,evict_tag);
#     endif
    }
    case CacheMiss:
      // Missed in the cache: Move data from L2 or memory to cache.  We consider
      // this to be a read, since we're moving data into the L2 for access by
      // the L1.
#     if EnableL2 != 0
      int l2_set,l2_way;
      if (setup_l2_access(cs,ma._type,CacheRead,l2_set,l2_way,ma._ra)) {
        // Hit/miss/evict:  Move data from L2 cache to L1 cache.
        core.L1iOp(cache_read_from_next)(ma._type,l1_set,l1_way,l2_set,l2_way,ma._ra);
      } else {
#     endif      
        // If no L2, or L2 is inactive, then we load from memory.
        core.L1iOp(cache_read_from_mem)(ma._type,l1_set,l1_way,ma._ra);
#     if EnableL2 != 0
      }
#     endif
      break;
    default:
      break;
    }
    core.L1iOp(cache_end)(ma._type,l1_action,l1_set,l1_way,ma._ra);
    core.set_memory_result(seq,core.L1iOp(cache_data_array)(l1_set,l1_way),core.L1iOp(cache_get_offset)(ma._ra));
  } else {
# endif     
    // No cache- just read/write memory.
    core.set_memory_result(seq,GlobalMemory.getPage(ma._ra),GlobalMemoryType::pageOffset(ma._ra));
# if EnableL1 != 0
  }
# endif
}

void setup_data_mem_access(const CacheStatus &cs,const MemAccess &ma,bool direct = false)
{
# if EnableL1 != 0
  // We have a cache, so we must do the cache data movement ourselves.
  int l1_set = ma._c._set, l1_way = ma._c._way;
  bool replace;
  CacheAction l1_action;

  if (core.L1dOp(cache_begin)(ma._type,l1_action,cs,l1_set,l1_way,ma._ra,replace,direct)) {
    switch (l1_action) {
    case CacheEvict: {
      addr_t evict_tag = core.L1dOp(cache_get_old_tag)(l1_set,l1_way);
      // Evicting a line: Move data from cache to memory.  In terms of the final
      // access, we always consider this to be a write, since we're moving data
      // to the L2.
#     if EnableL2 != 0
      int l2_set,l2_way;
      if (setup_l2_access(cs,ma._type,CacheWrite,l2_set,l2_way,evict_tag)) {
        // Hit/miss/evict:  Move data from L2 cache to L1 cache.
        core.L1dOp(cache_write_to_next)(ma._type,l1_set,l1_way,l2_set,l2_way,evict_tag);
      }
#     else      
        // If no L2, then dump directly to memory.
      core.L1dOp(cache_write_to_next)(ma._type,l1_set,l1_way,0,0,evict_tag);
#     endif
      if (direct) {
        break;
      }
    }
    case CacheMiss:
      // Missed in the cache: Move data from L2 or memory to cache.  We consider
      // this to be a read, since we're moving data into the L2 for access by
      // the L1.
#     if EnableL2 != 0
      int l2_set,l2_way;
      if (setup_l2_access(cs,ma._type,CacheRead,l2_set,l2_way,ma._ra)) {
        // Hit/miss/evict:  Move data from L2 cache to L1 cache.
        core.L1dOp(cache_read_from_next)(ma._type,l1_set,l1_way,l2_set,l2_way,ma._ra);
      } else {
#     endif      
        // If no L2, or L2 is inactive, then we load from memory.
        core.L1dOp(cache_read_from_mem)(ma._type,l1_set,l1_way,ma._ra);
#     if EnableL2 != 0
      }
#     endif
      break;
    default:
      break;
    }
    core.L1dOp(cache_end)(ma._type,l1_action,l1_set,l1_way,ma._ra,direct);
    core.set_memory_result(0,core.L1dOp(cache_data_array)(l1_set,l1_way),core.L1dOp(cache_get_offset)(ma._ra));
  } else {
# endif     
    // No cache- just read/write memory.
    core.set_memory_result(0,GlobalMemory.getPage(ma._ra),GlobalMemoryType::pageOffset(ma._ra));
# if EnableL1 != 0
  }
# endif
}

void setup_data_cache_access(const MemAccess &ma)
{
  CacheStatus cs;
  core.update_data_cache_status(cs,ma._type,ma._ra);
  switch (ma._id) {
  case 0:
    // If we're addressing cache 0, we assume it's the L1d cache.
    setup_data_mem_access(cs,ma,true);
    break;
# if EnableL2
  case 1: {
    // We assume that this is the L2.
    int l2_set = ma._c._set, l2_way = ma._c._way;
    setup_l2_access(cs,ma._type,ma._type,l2_set,l2_way,ma._ra,true);
  } break;
# endif
  }
}


ModelType::InstrPacket *fetch()
{
  core.logger().log_current_instr(0);
# ifdef PRE_CYCLE
  core.pre_cycle_handler();
# endif  
  PrefetchReturnType rc = core.prefetch<4>();
  switch (rc) {
  case prActive: {
    // Perform the instruction fetch.
    const MemAccess &ma1 = core.memory_request(0);
    if (LogMem) {
      cout << "Instr Fetch:  " << ma1 << "\n";
    }
    CacheStatus cs;
    core.update_data_cache_status(cs,ma1._type,ma1._ra);
    setup_instr_mem_access(cs,ma1,0);
    const MemAccess &ma2 = core.memory_request(1);
    if (ma2) {
      core.update_data_cache_status(cs,ma2._type,ma2._ra);
      setup_instr_mem_access(cs,ma2,1);
    }
    // Now proceed with execution.
    if (ModelType::InstrPacket *instr = core.decode()) {
      // log instruction issue time
      if (Trace) core.logger().log_instr_issue_time(systime);

      // We set the instruction's exception state to the core's current exception state
      // so that they can all be handled at once.
      instr->setExceptionState(core.getExceptionFlag());
      return instr;
    }
  } break;
  case prException: {
    core.commit_exceptions();
    return 0;
  } break;
  default:
    ;
  }
  return 0;
}

void do_mem_stuff(MemAccess::OpType op,const MemAccess &ma,uint32_t opc)
{
  if (LogMem) {
    cout << "Instr 0x" << hex << opc << dec << ":  Mem:  " << ma << "\n";
  }
  switch (op) {
  case MemAccess::opCache:
    setup_data_cache_access(ma);
    break;
  case MemAccess::opMem: {
    CacheStatus cs;
    core.update_data_cache_status(cs,ma._type,ma._ra);
    setup_data_mem_access(cs,ma);
  } break;
  default:
    break;
  }
}

void exec(ModelType::InstrPacket *instr)
{
  MemAccess::OpType op;
# ifdef BLOCKINGMEM
  core._opc = instr->opc()[0];
# endif
  while ((op = instr->exec(core)) != MemAccess::opNone) {
#   ifndef BLOCKINGMEM
    // If we get a memory request, process it.
    do_mem_stuff(op,core.mem_access(),instr->opc()[0]);
    core.remove_mem_access();
#   endif
  }
# ifdef POST_CYCLE
  core.post_cycle_handler();
# endif  
  if (LogBr) {
    auto bi = instr->branch_info(core);
    if (bi.first) {
      cout << "Instr 0x" << hex << instr->opc()[0] << ":  " << ((bi.second) ? "Taken" : "NotTaken") << "\n";
    }
  }
}

void commit(ModelType::InstrPacket *instr)
{
# ifdef BLOCKINGMEM
  for (unsigned i = 0; i != instr->getNumOps(); ++i) {
    instr->commit(core,i);
  }
# else
  MemAccess::OpType op;
  for (unsigned i = 0; i != instr->getNumOps(); ++i) {
    while ((op = instr->commit(core,i)) != MemAccess::opNone) {
      // If we get a memory request, process it.
#     ifndef BLOCKINGMEM
      // If we get a memory request, process it.
      do_mem_stuff(op,core.mem_access(),instr->opc()[0]);
      core.remove_mem_access();
#     endif
    }
  }
# endif

  instr->commit_exceptions(core);

  // Flush any delayed commits (if applicable).
  core.empty_commit_queue();

  core.check_breakpoints(instr->_addr);
  core.free_packet(instr);
}

void read_operands(ModelType::InstrPacket *instr)
{
  for (unsigned i = 0; i != instr->getNumOps(); ++i) {
    instr->read_operand(core,i);
  }
  if (LogInsts) {
    cout << "Instr:  ";
    // We use this form so that we can test our ability to resolve aliases and
    // sparse-register indirection.
    printInstrPacketResolved(cout,*instr);
    cout << '\n';
  }
}

// Main execution loop for pipelined execution.  Note that this is a *really*
// simple pipeline: No forwarding paths are present and there is no logic to
// handle any hazards.
void pipelined_exec()
{
  enum Stages { Fetch, ReadOps, Exec, NumStages };
  ModelType::InstrPacket *stages[NumStages];

  for (int i = 0; i != NumStages; ++i) {
    stages[i] = 0;
  }

  int outstanding_packets = 0;
  while (core.is_active() || outstanding_packets) {

    if (stages[Exec]) {
      commit(stages[Exec]);
      stages[Exec] = 0;
      --outstanding_packets;
    }

    if (stages[ReadOps]) {
      stages[Exec] = stages[ReadOps];
      stages[ReadOps] = 0;
      // Don't execute if exceptions were generated during the operand-read
      // process.
      if (!stages[Exec]->has_exceptions()) {
        exec(stages[Exec]);
      }
    }

    if (stages[Fetch]) {
      stages[ReadOps] = stages[Fetch];
      stages[Fetch] = 0;
      read_operands(stages[ReadOps]);
    }

    if ( (stages[Fetch] = fetch()) ) {
      ++outstanding_packets;
    }

    ++systime;
  }
}

// Main execution loop for non-pipelined execution.
void non_pipelined_exec()
{
  while (core.is_active()) {
    if (ModelType::InstrPacket *x = fetch()) {
      unsigned ed = x->_exdata;
      read_operands(x);
      if (!(x->_exdata != ed)) {
        // Don't execute if exceptions were generated during the operand-read
        // process.
        exec(x);
      }
      commit(x);
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
    options().setFlag  ("log-instr",   "Log all fetched instructions.");
    options().setFlag  ("log-branches","Log all branches and whether or not they were taken.");
    options().setFlag  ("pipelined",   "Execute in a pipelined fashion.");
    options().setOption("output","o",  "Specify an output file.");
    options().useCommandArgs(argc,argv);
    if (argc < 2 || !options().processCommandLine()) {
      throw runtime_error("Bad command-line");
    }

    Trace     = options().getFlag("trace",Trace);
    LogMem    = options().getFlag("log-mem",LogMem);
    LogBr     = options().getFlag("log-branches",LogBr);
    LogInsts  = options().getFlag("log-instr",LogInsts);
    LaxMode   = options().getFlag("lax",LaxMode);
    Pipelined = options().getFlag("pipelined",Pipelined);

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

    if (Pipelined) {
      pipelined_exec();
    } else {
      non_pipelined_exec();
    }

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

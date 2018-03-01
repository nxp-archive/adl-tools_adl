#ifndef SIMPLE_CORE_MODEL_H
#define SIMPLE_CORE_MODEL_H

#include "graphite/GraphiteCore.h"

#include "one_bit_branch_predictor.h"

namespace adl {
  class IssCore;
}

class SimpleCoreModel : public adl::CoreModel
{
public:
  SimpleCoreModel(adl::IssCore &core,double frequency);
  ~SimpleCoreModel();

  void outputSummary(std::ostream &os,const std::string &name);

  // Should be called with a branch result.
  virtual void handleBranch(adl::addr_t instr_addr,bool taken,adl::addr_t target);

  // Should be called by memory to set a latency, e.g. on a cache miss.
  virtual void handleMem(adl::CacheAccess ca,adl::addr_t addr,unsigned latency);

  // Called by the core to handle an instruction.  When done, should clear
  // any temporary state, such as for memory handling.  This is called after the
  // instruction has executed.
  virtual void handleInstruction(adl::Instruction &instruction);

private:
  void clearLatencies() 
  {
    _branch_latency       = 0;
    _icache_read_latency  = 0;
    _dcache_read_latency  = 0;
    _dcache_write_latency = 0;
  }

  BranchPredictor *_bp;

  unsigned _icache_read_latency;
  unsigned _dcache_read_latency;
  unsigned _dcache_write_latency;
  unsigned _branch_latency;
};

#endif

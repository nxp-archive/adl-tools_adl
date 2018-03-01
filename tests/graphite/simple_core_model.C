//
// This is the Graphite simple_core_model with some modifications for the ADL
// integration.  With ADL, the core model contains all knowledge of instruction
// cost, branch predictor behavior, etc., rather than it being part of the
// instruction, as it is with Graphite.
//

#include <sstream>

#include "iss/ModelInterface.h"

#include "simple_core_model.h"
#include "branch_predictor.h"

#include "ppc-graphite-cache-graphite.h"

using namespace std;
using namespace adl;

const int MispredictPenalty = 7;
const int PredictorSize     = 1024;

// Hard-coded instruction costs for our simple example.
const int MulCost = 3;
const int DivCost = 18;
const int IntCost = 1;

SimpleCoreModel::SimpleCoreModel(IssCore &core,double frequency) : 
  CoreModel(core, frequency),
  _bp(new OneBitBranchPredictor(MispredictPenalty,PredictorSize))
{
  clearLatencies();
}

SimpleCoreModel::~SimpleCoreModel()
{}

void SimpleCoreModel::outputSummary(std::ostream &os,const string &name)
{
  CoreModel::outputSummary(os,name);
   
  _bp->outputSummary(os);
}

void SimpleCoreModel::handleBranch(addr_t instr_addr,bool taken,addr_t target)
{
  bool prediction = _bp->predict(instr_addr,target);
  bool correct = (prediction == taken);

  _bp->update(prediction,taken,instr_addr,target);

  _branch_latency = correct ? 0 : _bp->getMispredictPenalty();

  if (_core.tracing_on(ttAnnotation)) {
    ostringstream ss;
    ss << "Branch prediction " << ((correct) ? "correct" : "incorrect");
    _core.logger().log_annotation(adl::mInfo,10,ss.str());
  }
}

void SimpleCoreModel::handleMem(CacheAccess ca,addr_t addr,unsigned latency)
{
  switch (ca) {
  case CacheIFetch:
    _icache_read_latency += latency;
    break;
  case CacheRead:
    _dcache_read_latency += latency;
    break;
  case CacheWrite:
    _dcache_write_latency += latency;
    break;
  default:
    break;
  }
}

static unsigned getInstrCost(unsigned type)
{
  switch (type) {
  case adliss::divide_instrs:
    return DivCost;
  case adliss::multiply_instrs:
    return MulCost;
  default:
    return IntCost;
  }
}

void SimpleCoreModel::handleInstruction(Instruction &instruction)
{
  // Execute this first so that instructions have the opportunity to
  // abort further processing (via AbortInstructionException)
  uint64_t cost = getInstrCost(instruction.type());

  unsigned memory_stall_cycles = _dcache_write_latency + _dcache_read_latency;

  unsigned execution_unit_stall_cycles = cost;

  uint64_t total_cost = _icache_read_latency + memory_stall_cycles + execution_unit_stall_cycles;

  _cycle_count += total_cost;

  // update counters
  _instruction_count++;

  // Update Common Counters.  I'm subtracting one from the execution unit
  // stalls, because a cost of 1 doesn't really indicate a stall- it's the
  // minimum time that can pas for an instruction going through the pipeline.
  updatePipelineStallCounters(getLatency(_icache_read_latency), 
                              getLatency(memory_stall_cycles),
                              Time(0),
                              Time(0),
                              getLatency(execution_unit_stall_cycles - 1),
                              Time(0),
                              Time(0));

  clearLatencies();
}


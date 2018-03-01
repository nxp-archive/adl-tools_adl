//
// Base class for the Graphite performance model.  This contains some useful
// counters and such, but it is expected that users will derive from this to
// create their own models.
//

#ifndef _GRAPHITE_CORE_H
#define _GRAPHITE_CORE_H

#include <assert.h>
#include <queue>
#include <iostream>
#include <string>

#include "helpers/BasicTypes.h"

#include "iss/ModelInterface.h"

#include "GraphiteTime.h"
#include "GraphiteInstr.h"

namespace adl {

  class CoreModel
  {
  public:

    CoreModel(IssCore &core,double frequency);
    virtual ~CoreModel() {};

    const IssCore &core() const { return _core; };

    double frequency() const { return _frequency; };

    // Should be called with a branch result.
    virtual void handleBranch(addr_t instr_addr,bool taken,addr_t target) = 0;

    // Should be called by memory to set a latency, e.g. on a cache miss.
    virtual void handleMem(CacheAccess ca,addr_t addr,unsigned latency) = 0;

    // Called by the core to handle an instruction.  When done, should clear
    // any temporary state, such as for memory handling.  This is called after the
    // instruction has executed.
    virtual void handleInstruction(Instruction &instruction) = 0;

    // Called upon core model reset.
    virtual void reset() {};

    virtual void outputSummary(std::ostream &os,const std::string &name) = 0;

    virtual void flushPipeline();

    void setDVFS(double old_frequency, double new_voltage, double new_frequency, const Time& curr_time);
    virtual void recomputeAverageFrequency(double frequency); 

    Time getLatency(unsigned lat) const;
    const Time& get_ONE_CYCLE() const            { return _ONE_CYCLE; }

    uint64_t getCurrTime() const                 { return _cycle_count; };
    void setCurrTime(uint64_t t);

    uint64_t getStoreIssueTime() const           { assert(_store_issue_time > 0); return _store_issue_time; }
    void setStoreIssueTime(uint64_t t)           { _store_issue_time = t; }

  protected:
    virtual void updatePipelineStallCounters(const Time& instruction_fetch__stall_time,
                                             const Time& memory_access__stall_time,
                                             const Time& load_queue__stall_time,
                                             const Time& store_queue__stall_time,
                                             const Time& execution_unit__stall_time,
                                             const Time& branch_speculation__violation_stall_time,
                                             const Time& load_speculation__violation_stall_time);
    virtual void initializeStallCounters();

    // Instruction latency table
    virtual void initializeLatencyTable(double frequency);
    virtual void updateLatencyTable(double frequency);

    IssCore  &_core;
    uint64_t &_cycle_count;      // This points to the core object's td counter.

    uint64_t  _dispatch_time;
    uint64_t  _commit_time;
    uint64_t  _store_issue_time;
    uint64_t  _checkpointed_time;

    uint64_t  _instruction_count;
    double    _frequency;
    double    _average_frequency;   
    Time      _total_time;
    uint64_t  _total_cycles;

    // Latency table
    Time _latency_table[16];
    Time _ONE_CYCLE;

    // Pipeline stall counters
    Time _total_instruction_fetch__stall_time;
    Time _total_memory_access__stall_time;
    Time _total_load_queue__stall_time;
    Time _total_store_queue__stall_time;
    Time _total_execution_unit__stall_time;
    // Pipeline flushes
    Time _total_pipeline_flush__stall_time;
    // Branch/load speculation
    Time _total_branch_speculation_violation__stall_time;
    Time _total_load_speculation_violation__stall_time;
  };

}

#endif

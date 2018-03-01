
#include <iostream>

#include "GraphiteCore.h"

using namespace std;

namespace adl {

  CoreModel::CoreModel(IssCore &core,double frequency) : 
    _core(core),
    _cycle_count(*core.get_td_counter_ptr()),
    _dispatch_time(0),
    _commit_time(0),
    _store_issue_time(0),
    _checkpointed_time(0),
    _instruction_count(0),
    _frequency(frequency),
    _average_frequency(frequency),
    _total_time(0),
    _total_cycles(0)
  {
    // Initialize Pipeline Stall Counters
    initializeStallCounters();
    
    // Initialize instruction costs
    initializeLatencyTable(_frequency);
  }
  
  void CoreModel::flushPipeline()
  {
    assert(_commit_time >= _dispatch_time);
    assert(_dispatch_time >= _cycle_count);
    _total_pipeline_flush__stall_time += Time(1 + _commit_time - _dispatch_time);
    _cycle_count = _commit_time;
  }

  Time CoreModel::getLatency(uint32_t lat) const
  {
    return (lat < 16) ? _latency_table[lat] : Time( Latency((uint64_t) lat, _frequency) );
  }
  
  void CoreModel::initializeLatencyTable(double frequency)
  {
    for (uint64_t i = 0; i < 16; i++) {
      _latency_table[i] = Time( Latency(i, frequency) );
    }
    _ONE_CYCLE = Time( Latency(1, _frequency) );
  }

  void CoreModel::updateLatencyTable(double frequency)
  {
    for (uint64_t i = 0; i < 16; i++)
      _latency_table[i] = Time( Latency(i, frequency) );
    _ONE_CYCLE = Time( Latency(1, _frequency) );
  }
  
  void CoreModel::outputSummary(ostream& os,const string &name)
  {
    if (name.empty()) {
      os << "Core";
    }
    else {
      os << name;
    }
    os << " Summary:" << endl;
    os << "    Total Instructions: " << _instruction_count << endl;
    os << "    Completion Time (in nanoseconds): " << getLatency(_cycle_count).toNanosec() << endl;
    os << "    Average Frequency (in GHz): " << _average_frequency << endl;
    // Pipeline stall counters
    os << "    Stall Time Breakdown (in nanoseconds): " << endl;
    os << "      Instruction Fetch: "   << _total_instruction_fetch__stall_time.toNanosec()            << endl;
    os << "      Memory Access: "       << _total_memory_access__stall_time.toNanosec()                << endl;
    os << "        Load Queue: "        << _total_load_queue__stall_time.toNanosec()                   << endl;
    os << "        Store Queue: "       << _total_store_queue__stall_time.toNanosec()                  << endl;
    os << "      Execution Unit: "      << _total_execution_unit__stall_time.toNanosec()               << endl;
    os << "      Pipeline Flush: "      << _total_pipeline_flush__stall_time.toNanosec()               << endl;
    os << "      Branch Speculation: "  << _total_branch_speculation_violation__stall_time.toNanosec() << endl;
    os << "      Load Speculation: "    << _total_load_speculation_violation__stall_time.toNanosec()   << endl;
  }

  // This function is called:
  // 1) Whenever frequency is changed
  void CoreModel::setDVFS(double old_frequency, double new_voltage, double new_frequency, const Time& curr_time)
  {
    recomputeAverageFrequency(old_frequency);
    updateLatencyTable(new_frequency);
    _frequency = new_frequency;
  }

  void CoreModel::setCurrTime(uint64_t t)
  {
    _cycle_count = t;
    _checkpointed_time = t;
  }

  // This function is called:
  // 1) On thread exit
  // 2) Whenever frequency is changed
  void CoreModel::recomputeAverageFrequency(double old_frequency)
  {
    _total_cycles += _cycle_count - _checkpointed_time;
    _total_time += getLatency(_cycle_count - _checkpointed_time);
    _average_frequency = ((double) _total_cycles)/((double) _total_time.toNanosec());
    
    _checkpointed_time = _cycle_count;
  }

  void CoreModel::initializeStallCounters()
  {
    _total_instruction_fetch__stall_time = Time(0);
    _total_memory_access__stall_time = Time(0);
    _total_load_queue__stall_time = Time(0);
    _total_store_queue__stall_time = Time(0);
    _total_execution_unit__stall_time = Time(0);
    _total_pipeline_flush__stall_time = Time(0);
    _total_branch_speculation_violation__stall_time = Time(0);
    _total_load_speculation_violation__stall_time = Time(0);
  }

  void CoreModel::updatePipelineStallCounters(const Time& instruction_fetch__stall_time,
                                              const Time& memory_access__stall_time,
                                              const Time& load_queue__stall_time,
                                              const Time& store_queue__stall_time,
                                              const Time& execution_unit__stall_time,
                                              const Time& branch_speculation__violation_stall_time,
                                              const Time& load_speculation__violation_stall_time)
  {
    _total_instruction_fetch__stall_time += instruction_fetch__stall_time;
    _total_memory_access__stall_time += memory_access__stall_time;
    _total_load_queue__stall_time += load_queue__stall_time;
    _total_store_queue__stall_time += store_queue__stall_time;
    _total_execution_unit__stall_time += execution_unit__stall_time;
    _total_branch_speculation_violation__stall_time += branch_speculation__violation_stall_time;
    _total_load_speculation_violation__stall_time += load_speculation__violation_stall_time;
  }

}

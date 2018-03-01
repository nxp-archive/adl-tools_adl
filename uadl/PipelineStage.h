//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef _PIPELINE_STAGE_H
#define _PIPELINE_STAGE_H

#include <sstream>
#include <vector>
#include <cassert>

#include "Logger.h"

namespace uadl {

  class StageLogger;
  class StageInstrLogger;
  class IssueLogger;
  class UadlArch;

  struct StageInstrInfo {
    const InstrBase            *_instr;
    const adl::InstrPacketBase *_pkt;
    bool                        _has_threads;
    unsigned                    _tid;

    StageInstrInfo(const InstrBase *instr,const adl::InstrPacketBase *pkt,bool ht,unsigned tid) :
      _instr(instr), _pkt(pkt), _has_threads(ht), _tid(tid)
    {}
  };

  //
  // Base class for pipeline stages.  This contains design-invariant code and can thus
  // be implemented as a non-template.
  //
  class PipelineStageBase {
  public:
    PipelineStageBase(const std::string &name);
    virtual ~PipelineStageBase() {
      removeLogger();
    }

    const std::string &name() const { return _name; };

    virtual StageInstrInfo getInstrInfo(unsigned i) const = 0;

    void createLogger(unsigned count,LogClient *lc,unsigned tflags);
    void removeLogger();
    bool report_and_clear(std::ostream &o,const std::string &pfx);

    StageInstrLogger *logger(unsigned index);
    StageLogger *logobj() const { return _logger; };

    virtual bool has_instr(unsigned index) const = 0;

  protected:
    std::string          _name;
    mutable StageLogger *_logger;
  };

  //
  // A pipeline stage can hold a fixed number of instructions and can act as a queue
  // manner or in a lock-step manner.
  // 
  template <size_t Capacity,bool Rigid,class ArchType,class InstrType,bool HasThreads>
  class PipelineStage : public PipelineStageBase {
  public:
    PipelineStage(const std::string &name) :
      PipelineStageBase(name)
    {
      reset();
    }

    void reset()
    {
      ForRange(Capacity,i) {
        _instrs[i] = 0;
      }
      _count = 0;
      _head = 0;
      _tail = 0;
    }

    // This puts the stage back into a consistent state, after instructions
    // might have been deleted from the middle.
    void cleanup(unsigned orig_size)
    {
      InstrType *tmp[Capacity];
      unsigned tsize = 0, i = 0;
      for ( ; i != orig_size; ++i) {
        if (InstrType *instr = (*this)[i]) {
          tmp[tsize++] = instr;
        }
      }
      reset();
      _count = tsize;
      _head = 0;
      _tail = (_head + tsize) % Capacity;
      for (i = 0; i != tsize; ++i) {
        _instrs[i] = tmp[i];
      }
    }

    void createLogger(LogClient *lc,unsigned tflags) { PipelineStageBase::createLogger(Capacity,lc,tflags); };
   
    // Returns the nth instruction from the current head.
    InstrType *operator[](unsigned i) { return _instrs[pos(i)]; };

    // Returns the nth instruction, in absolute terms.
    InstrType *get(unsigned i) { return _instrs[i]; };

    InstrType *head() const { return _instrs[_head]; };
    InstrType *tail() const { return _instrs[_tail-1]; };

    // Returns relevant indices.
    unsigned headp() const { return _head; };
    unsigned pos(unsigned i) const { return (_head + i) % Capacity; };

    unsigned capacity() const { return Capacity; };

    // This checks to make sure that a move is possible.  For rigid pipelines,
    // this checks the stall flag.
    bool can_move(const ArchType &uarch) const { return (Rigid) ? uarch.can_move() : true; };

    bool has_space() const { return _count < Capacity; };
    bool has_space(const ArchType &uarch,InstrBase &,Logger *) const { return can_move(uarch) && has_space(); };

    bool has_space(unsigned n) const { return (_count+n) <= Capacity; };
    bool has_space(const ArchType &uarch,InstrBase &,Logger *,unsigned n) const { return can_move(uarch) && has_space(n); };
    
    bool empty() const { return _count == 0; };
    bool empty(const ArchType &,InstrBase &,Logger *) const { return empty(); };

    // Dummy function to allow the user to avoid a capacity check if it's not needed.
    bool no_capacity_check(const ArchType &,InstrBase &,Logger *) const { return true; };

    unsigned instr_class(const ArchType &uarch,InstrBase &,Logger *,unsigned index = 0) const {
      unsigned i = pos(index);
      if (!has_instr(i)) {
        return 0;
      } else {
        return _instrs[i]->instr_class();
      }
    };

    bool has_attr(const ArchType &uarch,InstrBase &,Logger *,unsigned index,unsigned attr) const {
      unsigned i = pos(index);
      if (!has_instr(i)) {
        return 0;
      } else {
        return _instrs[i]->has_attr(attr);
      }
    };

    bool has_attr(const ArchType &uarch,InstrBase &ic,Logger *l,unsigned attr) const {
      return has_attr(uarch,ic,l,0,attr);
    };

    virtual bool has_instr(unsigned index) const { return _instrs[index]; };

    unsigned size() const { return _count; };
    bool size(const ArchType &,InstrBase &,Logger *) const { return size(); };

    // For use by instructions, if they want to explicitly add themselves to a
    // shared stage.
    void add_instr(const ArchType &uarch,InstrType &instr,Logger *) { add_instr(&instr); };

    void add_instr(InstrType *instr) 
    {
      assert(_count < Capacity);
      ++_count;
      _instrs[_tail] = instr;
      _tail = (_tail + 1) % Capacity;
    }

    void remove_instr()
    {
      --_count;
      _instrs[_head] = 0;
      _head = (_head + 1) % Capacity;
    }

    InstrType *remove_instr(unsigned index)
    {
      unsigned i = pos(index);
      if (_instrs[i]) {
        return remove_instr_internal(i);
      }
      return 0;
    }

    InstrType *remove_instr(unsigned index,unsigned tid)
    {
      unsigned i = pos(index);
      if (_instrs[i] && (_instrs[i]->thread_id() == tid)) {
        return remove_instr_internal(i);
      }
      return 0;
    }

    // Used for independent-scheduling removal, where the pipeline stage is
    // treated as a queue, but removal can occur from the middle.  The head is
    // always at position 0 and we copy instructions forward to remove any empty
    // slots.
    InstrType *remove_instr_indep(unsigned index)
    {
      --_count;
      InstrType *t = _instrs[index];
      for (_tail = index; _tail < Capacity-1 && _instrs[_tail+1]; ++_tail) {
        _instrs[_tail] = _instrs[_tail+1];
      }
      _instrs[_tail] = 0;
      return t;
    }

    // Set the tail to point to the next position after the last instructin,
    // based upon the current head.
    void update_tail()
    {
      _tail = (_head + _count) % Capacity;
    }

    virtual StageInstrInfo getInstrInfo(unsigned i) const { 
		return StageInstrInfo(reinterpret_cast<InstrBase*>(_instrs[i]),reinterpret_cast<adl::InstrPacketBase*>(_instrs[i]->_instr),
				HasThreads,_instrs[i]->thread_id());
    }

  protected:
    InstrType *remove_instr_internal(unsigned index)
    {
      --_count;
      InstrType *t = _instrs[index];
      _instrs[index] = 0;
      return t;
    }

    InstrType *_instrs[Capacity];
    unsigned   _count;
    unsigned   _head;
    unsigned   _tail;    
  };

  //
  // The issue object is just a special-purpose single-instruction item, since
  // our "issue" is really connected to fetch and executes immediately.
  // 
  class IssueStageBase {
  public:
    IssueStageBase(const std::string &pfx);
    virtual ~IssueStageBase() {
      removeLogger();
    }

    virtual StageInstrInfo getInstrInfo() const = 0;

    void createLogger(LogClient *lc,unsigned tflags);
    void removeLogger();
    bool report_and_clear(std::ostream &o,const std::string &pfx);

    virtual bool has_instr() const = 0;

    IssueLogger *logger(bool predTaken,addr_t predTrg);

  protected:
    std::string  _pfx;
    IssueLogger *_logger;
  };


  template <class InstrType,bool HasThreads>
  class IssueStage : public IssueStageBase {
  public:
    IssueStage(const std::string &pfx) :
      IssueStageBase(pfx)
    {
      reset();
    }

    void reset()
    {
      _instr = 0;
    }

    void createLogger(LogClient *lc,unsigned tflags) { IssueStageBase::createLogger(lc,tflags); };

    InstrType *instr() const { return _instr; };
    bool add_instr(InstrType *instr) { return (_instr = instr); }
    void remove_instr() { _instr = 0; }
       
    bool empty() const { return !_instr; };
    virtual bool has_instr() const { return _instr; };

    bool has_attr(const UadlArch &,InstrBase &,Logger *,unsigned attr) const {
      if (!_instr) {
        return 0;
      } else {
        return _instr->has_attr(attr);
      }
    };

    virtual StageInstrInfo getInstrInfo() const { 
		return StageInstrInfo(reinterpret_cast<InstrBase*>(_instr),reinterpret_cast<adl::InstrPacketBase*>(_instr->_instr),
				HasThreads,_instr->thread_id());
    }

  protected:
    InstrType *_instr;
  };

  void instructionReport(std::ostream &o,const InstrBase &instr,const adl::InstrPacketBase *ip);

  // Logger for a single position in a pipeline.
  class StageInstrLogger : public Logger {
  public:
    StageInstrLogger(const PipelineStageBase *stage,unsigned index,LogClient *lc,unsigned tflags) : 
      Logger(std::string(),stage->name(),lc,tflags),
      _stage(stage), 
      _index(index),
      _next(0)
    {};

    virtual void clear() { _instr.str(std::string()); Logger::clear(); _next = 0; };

    virtual bool report(std::ostream &o,const std::string &pfx) const;

    StageInstrLogger *next() const { return _next; };

    bool hasStuff() const { return hasLog() || !_instr.str().empty(); };

    void logInstruction();

  protected:
    friend class StageLogger;

    const PipelineStageBase *_stage;  // The stage to which we are attached.
    unsigned                 _index;  // The position within the stage we're logging.
    std::ostringstream       _instr;  // Instruction report for the current cycle.
    StageInstrLogger        *_next;   // Next item in reporting chain.
  };

  // This is the container which holds all of the individual loggers in a stage.
  // A stage maintains a pointer to this object.
  class StageLogger {
  public:
	typedef std::vector<StageInstrLogger*> StageInstrVect;

    StageLogger(const PipelineStageBase *stage,unsigned count,const std::string &name,LogClient *lc,unsigned tflags);
    ~StageLogger();

    void setClient(LogClient *lc,unsigned tflags);

    StageInstrLogger *logger(unsigned index);

    bool report(std::ostream &o,const std::string &pfx) const;
    bool report_and_clear(std::ostream &o,const std::string &pfx);

    // Add a logger object to the report list.  We use the report list so that
    // we accurately list instructions in the order they're present in the stage
    // for the cycle.
    void add_logger_to_list(StageInstrLogger *l) {
      if (_tail) {
        // Something's in the list, so point last item to new tail.
        _tail->_next = l;
      } else {
        // If tail is 0, then _head is also 0, since it implies that the list is
        // empty.
        _head = l;
      }
      _tail = l;
    }
  protected:
    StageInstrVect     _loggers;  // All loggers.
    const std::string &_name;     // The stage's name.

    StageInstrLogger  *_head;     // Start of report list for the cycle.
    StageInstrLogger  *_tail;     // End of report list for the cycle.
  };

  // This maintains a simple array of all stage instruction loggers known to the
  // core, for easy access by the core.  That is, this stores loggers for all
  // instruction positions in all stages.
  class StageInstrLoggers : private std::vector<StageInstrLogger *> {
    typedef std::vector<StageInstrLogger *> Base;
  public:
    StageInstrLoggers(unsigned c) : Base(c) {};

    void set(unsigned index,StageInstrLogger *l) {
      (*this)[index] = l;
    }

    // Accessing a logger automatically logs the instruction associated with it.
    // This way, we capture the instruction before it's moved via execution.
    StageInstrLogger *get(unsigned index) {
      StageInstrLogger *l = (*this)[index];
      l->logInstruction();
      return l;
    };

    void reset() {
      for (unsigned i = 0; i != size(); ++i) {
        (*this)[i] = 0;
      }
    }
  };


  class IssueLogger : public Logger {
  public:
    IssueLogger(const IssueStageBase *issue,const std::string &pfx,LogClient *lc,unsigned tflags) : 
      Logger(std::string(),lc,tflags),
      _issue(issue),
      _pfx(pfx)
    {}

    void logInstruction(bool predTaken,addr_t predTrg);

    virtual void clear() { _instr.str(std::string()); Logger::clear(); };
    virtual bool report(std::ostream &o,const std::string &pfx) const;

  protected:
    const IssueStageBase *_issue;
    std::string           _pfx;
    std::ostringstream    _instr;
  };


}

#endif

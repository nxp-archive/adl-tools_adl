//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef _BRANCH_PREDICTOR_H
#define _BRANCH_PREDICTOR_H

#include <string>
#include <sstream>

#include "helpers/BasicTypes.h"
#include "helpers/Macros.h"

#include "iss/CircularQueue.h"

#include "DynParms.h"

using std::string;
using adl::addr_t;

// Set this to true to do queue range checking.
#define BPQueueDebugCheck true

namespace uadl {

  // Type of data maintained in BTB 
  enum {
    ADDRESS,  
    INSTRUCTION 
  };

  // Branch predictor enablement predicate
  class BPredEnableDefault {
  public:
    bool operator()() { return true; }
  };

  /*******************************************************************************
   * branch predictor base class
   *
   * NOTE:
   *   - A branch predictor is not bandwidth constrained, i.e., it should be able
   *     to accept as many requests as the pipeline can produce.
   *
   *   - The class that is instantiated is a template and is called directly, i.e.
   *     no virtual function overhead for predicting or updating is present.
   *
   *   - Refer to BranchPredictorCounter for the basic interface.
   ******************************************************************************/
  class BranchPredictor : public DynParms {
  public:
    BranchPredictor(Timer &timer,unsigned id) : _id(id) 
    {
      timer.register_dyn_parms(*this);
    }

    virtual ~BranchPredictor() {}

    unsigned id() const { return _id; };

    virtual bool v_enabled() { return true; }

  protected:
    const unsigned _id;
  };

  // Predictor-data base class.  Additional tag data is implemented by
  // sub-classing from this base and then instantiating the relevant predictor
  // w/that class as a data type.
  struct PredictData {
    addr_t _addr;    // instruction address (ea)
    addr_t _target;  // target address
    unsigned  _state;   // prediction state
    unsigned  _size;    // instruction size (in bytes)
    Prediction _pred;   // last prediction.

    // uarch isn't used, but is present for argument compatibility when the data
    // type is subclassed in order to read architectural state.
    PredictData() {};
    PredictData(UadlArch &uarch,addr_t a) : _addr(a), _target(0), _state(0), _size(0), _pred(NoPrediction) {};

    bool operator==(const PredictData &x) const { return _addr == x._addr; };
  };

  /*******************************************************************************
   * single counter branch predictor
   *
   * TODO:
   *   - remove default parameter values
   ******************************************************************************/
  template <class ModelType,class InstrType,class BPData,class BPredEnable = BPredEnableDefault>
  class BranchPredictorCounter : public BranchPredictor {
  public:
    typedef BranchPredictor                                   base_type;
    typedef BPData                                            value_type;
    typedef BPredEnable                                       enable_type;
    typedef adl::DCircularQueue<value_type,BPQueueDebugCheck> container_type;

    BranchPredictorCounter(Timer &timer,unsigned id,const enable_type &enable,unsigned sz = 8,
                           unsigned counterWidth = 2) : 
      BranchPredictor(timer,id), _size(sz), _states(1 << counterWidth),
      _thresh(_states >> 1), _preds(sz), _enable(enable) 
    {
    }

    void reset() { _preds.clear(); }

    std::pair<Prediction,value_type *> do_predict(ModelType &uarch, addr_t addr, addr_t &target, unsigned *size)
    {
      value_type x(uarch,addr);
      if (enabled()) {
        int it = _preds.find(x);
        if (it >= 0) {
          value_type &pred = _preds[it];
          if (pred._state > _thresh) {
            pred._pred = NotTaken; 
          } else {
            target = pred._target;
            if (size) *size = pred._size;
            pred._pred = Taken;
          }
          return std::make_pair(pred._pred,&pred);
        }
        return std::make_pair(Miss,(value_type*)0);
      } else {
        return std::make_pair(NoPrediction,(value_type*)0);
      }
    }

    // Transaction interface:  Make a prediction.
    //
    // x:       Query data structure.  This should match against an entry via operator==.
    // target:  Target address (output).
    // size:    If non-null and a prediction is made., should be set to the size of the prediction.
    Prediction predict(ModelType &uarch, addr_t addr, addr_t &target, unsigned *size = NULL)
    {
      return do_predict(uarch,addr,target,size).first;
    }

    void log_predict(Logger *logger,Prediction p,addr_t trg)
    {
      if (logger) {
        switch (p) {
        case Miss:
          logger->logMessage("branch prediction miss");
          break;
        case Taken: {
          std::ostringstream ss;
          ss << "branch predicted taken, target 0x" << std::hex << trg;
          logger->logMessage(ss.str());
          break;
        }
        case NotTaken:
          logger->logMessage("branch predicted not taken");
          break;
        case NoPrediction:
          break;
        }
      }
    }

    std::pair<Prediction,addr_t> predict(ModelType &uarch,InstrType &instr,Logger *logger)
    {
      addr_t trg = 0;
      Prediction p = predict(uarch,instr.addr(),trg);
      log_predict(logger,p,trg);

      return std::make_pair(p,trg);
    }

    int get_last_prediction(ModelType &uarch,InstrType &instr)
    {
      BPData x(uarch,instr.addr());
      return _preds.find(x);      
    }

    bool last_predict_taken(ModelType &uarch,InstrType &instr,Logger *logger = 0)
    {
      int it = get_last_prediction(uarch,instr);
      return (it >= 0 && _preds[it]._pred == Taken);
    }

    bool last_predict_taken(ModelType &uarch,InstrType &instr,Logger *logger,addr_t &target)
    {
      int it = get_last_prediction(uarch,instr);
      if (it >= 0 && _preds[it]._pred == Taken) {
        target = _preds[it]._target;
        return true;
      } else {
        return false;
      }
    }

    void update(ModelType &uarch,InstrType &instr,Logger *logger,const value_type &x)
    {
      addr_t target = instr.branch_target();
      bool taken    = instr.branch_taken(uarch);
      unsigned size = instr.size();

      if (enabled()) {
        int it = _preds.find(x);
        if (it < 0) {          
          if (_preds.full()) {
            _preds.pop_front();
          }
          _preds.push_back(x);
          _preds.back()._target = target;
          _preds.back()._state  = taken ? _thresh : _thresh + 1;
          _preds.back()._size   = size;
        } else if (taken) {
          decrState(_preds[it]._state);
          // update target
          _preds[it]._target = target;
        } else {
          incrState(_preds[it]._state);
        }
      }
    }

    // Transaction interface:  Update the predictor.
    //
    // uarch:   The pipeline model (used for querying state, if applicable).
    // target:  Target address for the update.
    // taken:   Taken data for the update.
    // size:    Size data for the update.
    void update(ModelType &uarch,InstrType &instr,Logger *logger = 0)
    {
      BPData x(uarch,instr.addr());
      update(uarch,instr,logger,x);
    }

    bool enabled() { return _enable(); };

    bool enabled(ModelType &uarch,InstrType &instr,Logger *logger = 0) { return enabled(); };

    virtual bool v_enabled() { return enabled(); };

    // For dynamic parameter manipulation.

    virtual bool set_dyn_parm(const std::string &parm,unsigned value)
    {
      if (parm == "bp-size") {
        _size = value;
        return true;
      } else {
        return false;
      }
    }

    virtual bool get_dyn_parm(unsigned &value,const std::string &parm) const
    {
      if (parm == "bp-size") {
        value = _size;
        return true;
      } else {
        return false;
      }
    }

    virtual void     list_dyn_parm(adl::StrPairs &parms) const
    {
      parms.push_back(make_pair(string("bp-size"),string("Number of entries in the branch predictor.")));
    }

  protected:
    void incrState(unsigned& s) { if (s < _states) ++s; } 
    void decrState(unsigned& s) { if (s > 1) --s; } 

    // data members
    unsigned          _size;    // Number of entries.
    const unsigned	  _states;
    const unsigned	  _thresh;
    container_type    _preds;
    enable_type       _enable;
  };

} // namespace uadl

#endif // _BRANCH_PREDICTOR_H

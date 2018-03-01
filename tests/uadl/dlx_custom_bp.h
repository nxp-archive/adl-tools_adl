//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Test of a custom branch predictor.  This is just a copy of the standard predictor, but
// we're testing proper code generation here.
//

#include <deque>

#ifndef _DLX_BP_FETCH_BP_H_

/*******************************************************************************
 * single counter branch predictor
 *
 * TODO:
 *   - remove default parameter values
 ******************************************************************************/
template <class ModelType,class InstrType,class BPData,class BPredEnable>
class dlx_custom_bp : public uadl::BranchPredictor {
public:
  typedef BranchPredictor                   base_type;
  typedef BPData                            value_type;
  typedef BPredEnable                       enable_type;
  typedef std::deque<value_type>            container_type;
  typedef typename container_type::iterator iterator;

  dlx_custom_bp(uadl::Timer &timer,unsigned id,const enable_type &enable,unsigned sz = 8,
                unsigned counterWidth = 2) : 
    BranchPredictor(timer,id), _size(sz), _states(1 << counterWidth),
    _thresh(_states >> 1), _preds(), _enable(enable) 
  {
  }

  virtual void reset() { _preds.clear(); }

  // Transaction interface:  Make a prediction.
  //
  // x:       Query data structure.  This should match against an entry via operator==.
  // target:  Target address (output).
  // size:    If non-null and a prediction is made., should be set to the size of the prediction.
  uadl::Prediction predict(ModelType &uarch, addr_t addr, addr_t &target, unsigned *size = NULL)
  {
    BPData x(uarch,addr);
    if (enabled()) {
      iterator it = find(_preds.begin(),_preds.end(),x);
      if (it != _preds.end()) {
        if (it->_state > _thresh) {
          it->_pred = uadl::NotTaken; 
        } else {
          target = it->_target;
          if (size) *size = it->_size;
          return it->_pred = uadl::Taken;
        }
      }
      return uadl::Miss;      
    } else {
      return uadl::NoPrediction;
    }
  }

  iterator get_last_prediction(ModelType &uarch,InstrType &instr)
  {
    BPData x(uarch,instr.addr());
    return find(_preds.begin(),_preds.end(),x);
  }

  bool last_predict_taken(ModelType &uarch,InstrType &instr,uadl::Logger *logger = 0)
  {
    iterator it = get_last_prediction(uarch,instr);
    return (it != _preds.end() && it->_pred == uadl::Taken);
  }

  bool last_predict_taken(ModelType &uarch,InstrType &instr,uadl::Logger *logger,addr_t &target)
  {
    iterator it = get_last_prediction(uarch,instr);
    if (it != _preds.end() && it->_pred == uadl::Taken) {
      target = it->_target;
      return true;
    } else {
      return false;
    }
  }

  // Transaction interface:  Update the predictor.
  //
  // uarch:   The pipeline model (used for querying state, if applicable).
  // target:  Target address for the update.
  // taken:   Taken data for the update.
  // size:    Size data for the update.
  void update(ModelType &uarch,InstrType &instr,uadl::Logger *logger)
  {
    BPData x(uarch,instr.addr());
    addr_t target = instr.branch_target();
    bool taken    = instr.branch_taken(uarch);
    unsigned size = instr.size();

    if (enabled()) {
      iterator it = find(_preds.begin(),_preds.end(),x);
      if (it == _preds.end()) {
        if (_preds.size() == _size) {
          _preds.pop_front();
        }
        _preds.push_back(x);
        _preds.back()._target = target;
        _preds.back()._state  = taken ? _thresh : _thresh + 1;
        _preds.back()._size   = size;
      } else if (taken) {
        decrState(it->_state);
        // update target
        it->_target = target;
      } else {
        incrState(it->_state);
      }
    }
  }

  bool enabled() { return _enable(); };

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

private:
  void incrState(unsigned& s) { if (s < _states) ++s; } 
  void decrState(unsigned& s) { if (s > 1) --s; } 

  // data members
  unsigned          _size;    // Number of entries.
  const unsigned	  _states;
  const unsigned	  _thresh;
  container_type    _preds;
  enable_type       _enable;
  uadl::Prediction  _last;
};

#endif


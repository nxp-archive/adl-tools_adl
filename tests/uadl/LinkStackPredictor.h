//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// A branch predictor whose address predictions come from a stack.  The stack
// size can either be set by the model or adjusted dynamically using the
// dynamic-parameter interface.  Note that the model code must explicitly pop
// the stack entry after a query, by calling pop_stack().  Branch taken/not-taken
// values are pushed on via update(), but you must call push_stack() to update
// the stack.
//
// This predictor is constructed as a wrapper around two predictors, one for
// normal branches and one for return-style branches.  We use the instruction
// class's attributes to select which predictor to use- if the instruction has
// the "return_type" attribute, then we use the link stack predictor, otherwise
// we use the standard predictor.

#include <deque>

#include <iss/CircularQueue.h>

#ifndef _LINK_STACK_PREDICTOR_H
#define _LINK_STACK_PREDICTOR_H

// The link-stack predictor.
template <class ModelType,class InstrType,class BPData,class BPredEnable>
class LinkStackPredictor : public uadl::BranchPredictorCounter<ModelType,InstrType,BPData,BPredEnable> {
public:
  typedef uadl::BranchPredictorCounter<ModelType,InstrType,BPData,BPredEnable> base_type;
  typedef BPData                                                               value_type;
  typedef BPredEnable                                                          enable_type;

  LinkStackPredictor(uadl::Timer &timer,unsigned id,const enable_type &enable,unsigned size,
                     unsigned counter_width,unsigned stack_size) : 
    base_type(timer,id,enable,size,counter_width), 
    _stack_size(stack_size),
    _stack(_stack_size)
  {
  }

  virtual void reset() { base_type::reset(); _stack.clear(); }

  // Transaction interface: Make a prediction.  We ignore the target data in the
  // BPData object and use our link stack instead.
  //
  // x:       Query data structure.  This should match against an entry via operator==.
  // target:  Target address (output).
  // size:    If non-null and a prediction is made., should be set to the size of the prediction.
  uadl::Prediction predict(ModelType &uarch, addr_t addr, addr_t &target, unsigned *size = NULL)
  {
    std::pair<uadl::Prediction,value_type*> p = base_type::do_predict(uarch,addr,target,size);
    if (p.first == uadl::Taken) {
      p.second->_target = target = top_stack();
      pop_stack();
      // This is just here so that we can force a bad prediction to occur, to
      // make sure that safe-mode models can recover.
#     ifdef BAD_PREDICTION
      target = 0;
#     endif
    }
    return p.first;
  }

  addr_t top_stack() const { 
    return (!_stack.empty()) ? _stack.back() : 0;
  }

  std::pair<uadl::Prediction,addr_t> predict(ModelType &uarch,InstrType &instr,uadl::Logger *logger)
  {
    addr_t trg = 0;
    uadl::Prediction p = predict(uarch,instr.addr(),trg);
    base_type::log_predict(logger,p,trg);
    return std::make_pair(p,trg);
  }

  // We actually read from the predicate's stored target, as that's updated by
  // predict() before the stack is popped.
  bool last_predict_taken(ModelType &uarch,InstrType &instr,uadl::Logger *logger,addr_t &target)
  {
    int it = get_last_prediction(uarch,instr);
    if (it >= 0 && base_type::_preds[it]._pred == uadl::Taken) {
      target = base_type::_preds[it]._target;
      return true;
    } else {
      return false;
    }
  }

  void push_stack(addr_t target)
  {
    if (_stack.full()) _stack.pop_front();
    _stack.push_back(target);
  }

  // Remove the last entry from the stack.
  void pop_stack()
  {
    if (!_stack.empty()) {
      _stack.pop_back();
    }
  }

  // For dynamic parameter manipulation.

  virtual bool set_dyn_parm(const std::string &parm,unsigned value)
  {
    if (parm == "bp-stack-size") {
      _stack_size = value;
      _stack.resize(_stack_size);
      return true;
    } else {
      return base_type::set_dyn_parm(parm,value);
    }
  }

  virtual bool get_dyn_parm(unsigned &value,const std::string &parm) const
  {
    if (parm == "bp-stack-size") {
      value = _stack_size;
      return true;
    } else {
      return base_type::get_dyn_parm(value,parm);
    }
  }

  virtual void     list_dyn_parm(adl::StrPairs &parms) const
  {
    parms.push_back(make_pair(string("bp-stack-size"),string("Number of entries in the link stack.")));
    base_type::list_dyn_parm(parms);
  }

private:
  typedef adl::DCircularQueue<addr_t,true> StackType;

  // data members
  unsigned          _stack_size; // Number of entries in the link stack.
  StackType         _stack;
};

// This is the wrapper predictor which contains the two individual predictors.
template <class ModelType,class InstrType,class BPData,class BPredEnable>
class ComboPredictor {
  typedef uadl::BranchPredictorCounter<ModelType,InstrType,BPData,BPredEnable> Standard;
  typedef LinkStackPredictor<ModelType,InstrType,BPData,BPredEnable>           LinkStack;
  typedef BPredEnable                                                          enable_type;

public:
  ComboPredictor(uadl::Timer &timer,unsigned id,const enable_type &enable,unsigned size,
                 unsigned counter_width,unsigned stack_size) :
    _return_type_attr(ModelType::getInstrClassAttrByName("return_type")),
    _link_stack(timer,id,enable,size,counter_width,stack_size),
    _standard(timer,id,enable,size,counter_width)
  {
    if (_return_type_attr == 0) {
      // Something's wrong if we don't get an attribute- it either means
      // no return-type attributes exist or else we used the wrong attribute.
      throw std::runtime_error("No return-type instructions exist ('return_type' attribute not defined).");
    }
  }

  virtual void reset()
  {
    _link_stack.reset();
    _standard.reset();
  }  

  bool enabled() { return _standard.enabled(); };

  uadl::Prediction predict(ModelType &uarch, addr_t addr, addr_t &target, unsigned *size = NULL)
  {
    uadl::Prediction p;

    p = _standard.predict(uarch,addr,target,size);
    if (p != uadl::Miss && p != uadl::NoPrediction) return p;

    return _link_stack.predict(uarch,addr,target,size);
  }

  void update_normal(ModelType &uarch,InstrType &instr,uadl::Logger *logger)
  {
    _standard.update(uarch,instr,logger);
  }

  void update_link_type(ModelType &uarch,InstrType &instr,uadl::Logger *logger)
  {
    _standard.update(uarch,instr,logger);
    _link_stack.push_stack(instr.addr()+instr.size());
  }

  void update_return_type(ModelType &uarch,InstrType &instr,uadl::Logger *logger)
  {
    _link_stack.update(uarch,instr,logger);
    //    _link_stack.pop_stack();
  }

  bool last_predict_taken(ModelType &uarch,InstrType &instr,uadl::Logger *logger,addr_t &target)
  {
    if (instr.has_attr(_return_type_attr)) {
      return _link_stack.last_predict_taken(uarch,instr,logger,target);
    } else {
      return _standard.last_predict_taken(uarch,instr,logger,target);
    }
  }

  virtual bool set_dyn_parm(const std::string &parm,unsigned value)
  {
    _standard.set_dyn_parm(parm,value);
    return _link_stack.set_dyn_parm(parm,value);
  }  

  virtual bool get_dyn_parm(unsigned &value,const std::string &parm) const
  {
    return _link_stack.get_dyn_parm(value,parm);
  }

  virtual void list_dyn_parm(adl::StrPairs &parms) const  
  {
    return _link_stack.list_dyn_parm(parms);
  }

protected:
  unsigned  _return_type_attr;

  LinkStack _link_stack;
  Standard  _standard;
};

#endif


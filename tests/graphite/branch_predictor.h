#ifndef BRANCH_PREDICTOR_H
#define BRANCH_PREDICTOR_H

#include <iostream>

#include "helpers/BasicTypes.h"

class BranchPredictor
{
public:
  BranchPredictor(unsigned mispredict_penalty);
  virtual ~BranchPredictor();

  virtual bool predict(adl::addr_t ip, adl::addr_t target) = 0;
  virtual void update(bool predicted, bool actual, adl::addr_t ip, adl::addr_t target) = 0;

  uint64_t getMispredictPenalty();

  virtual void outputSummary(std::ostream &os);
  uint64_t getNumCorrectPredictions() { return _correct_predictions; }
  uint64_t getNumIncorrectPredictions() { return _incorrect_predictions; }

protected:
  void updateCounters(bool predicted, bool actual);

private:
  uint64_t _correct_predictions;
  uint64_t _incorrect_predictions;

  unsigned _mispredict_penalty;

  void initializeCounters();
};

#endif

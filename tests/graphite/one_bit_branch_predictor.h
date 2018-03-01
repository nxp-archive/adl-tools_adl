#ifndef ONE_BIT_BRANCH_PREDICTOR_H
#define ONE_BIT_BRANCH_PREDICTOR_H

#include "helpers/BasicTypes.h"

#include "branch_predictor.h"

#include <vector>

class OneBitBranchPredictor : public BranchPredictor
{
public:
  OneBitBranchPredictor(unsigned mispredict_penalty,unsigned size);
  ~OneBitBranchPredictor();

  bool predict(adl::addr_t ip, adl::addr_t target);
  void update(bool predicted, bool actual, adl::addr_t ip, adl::addr_t target);
  
  void outputSummary(std::ostream &os);
  
private:
  std::vector<bool> _bits;
};

#endif

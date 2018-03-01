
#include "one_bit_branch_predictor.h"

using namespace std;
using namespace adl;

OneBitBranchPredictor::OneBitBranchPredictor(unsigned mispredict_penalty,unsigned size) : 
  BranchPredictor(mispredict_penalty),
  _bits(size)
{
}

OneBitBranchPredictor::~OneBitBranchPredictor()
{
}

bool OneBitBranchPredictor::predict(addr_t ip, addr_t target)
{
  unsigned index = ip % _bits.size();
  return _bits[index];
}

void OneBitBranchPredictor::update(bool predicted, bool actual, addr_t ip, addr_t target)
{
   updateCounters(predicted, actual);
   unsigned index = ip % _bits.size();
   _bits[index] = actual;
}

void OneBitBranchPredictor::outputSummary(std::ostream &os)
{
   BranchPredictor::outputSummary(os);
   os << "    type: one-bit (" << _bits.size() << ")" << endl;
}

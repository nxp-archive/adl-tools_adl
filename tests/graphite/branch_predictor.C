
#include "branch_predictor.h"
#include "one_bit_branch_predictor.h"

using namespace std;
using namespace adl;

BranchPredictor::BranchPredictor(unsigned mispredict_penalty) :
  _mispredict_penalty(mispredict_penalty)
{
   initializeCounters();
}

BranchPredictor::~BranchPredictor()
{ }

uint64_t BranchPredictor::getMispredictPenalty()
{
   return _mispredict_penalty;
}

void BranchPredictor::updateCounters(bool predicted, bool actual)
{
   if (predicted == actual)
      ++_correct_predictions;
   else
      ++_incorrect_predictions;
}

void BranchPredictor::initializeCounters()
{
   _correct_predictions = 0;
   _incorrect_predictions = 0;
}

void BranchPredictor::outputSummary(std::ostream &os)
{
   os << "  Branch predictor stats:" << endl
      << "    num correct: " << _correct_predictions << endl
      << "    num incorrect: " << _incorrect_predictions << endl;
}

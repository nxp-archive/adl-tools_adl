#ifndef _BANDWIDTH_CONTROL_H
#define _BANDWIDTH_CONTROL_H


// TODO: have to have iss/ModelInterface.h for unsigned from helpers/BasicTypes.h
#include "helpers/BasicTypes.h"

#include "Timer.h"


namespace uadl {

/*******************************************************************************
 * bandwidth control class
 *
 * NOTE:
 *   - 0 bandwidth means infinite bandwidth
 ******************************************************************************/
class BandwidthControl {
public:
  BandwidthControl(unsigned bandwidth = 0) :
    _bandwidth(bandwidth), _consumed(0), _lastConsumeTime(0) {}

  void setBandwidth(unsigned bw) { _bandwidth = bw; };

  // bandwidth control/query methods
  bool hasBandwidth(ttime_t currTime) const ;
  bool consumeBandwidth(ttime_t currTime);

private:
  // data members
  // TODO: _bandwidth should be const
  unsigned _bandwidth;         // amount of total bandwidth
  unsigned _consumed;          // amount of consumed bandwidth
  ttime_t _lastConsumeTime; // last time when bandwidth is consumed
};


inline bool BandwidthControl::hasBandwidth(ttime_t currTime) const
{
  return !_bandwidth || currTime != _lastConsumeTime || _consumed < _bandwidth;
}

} // namespace uadl

#endif // _BANDWIDTH_CONTROL_H

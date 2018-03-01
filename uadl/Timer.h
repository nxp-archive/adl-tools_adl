//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

/*******************************************************************************
 * NOTE:
 *   - It is strongly recommended that any time comparison should be done
 *     through the Timer API, because it properly handles integer overflow under
 *     a realistic assumption.
 ******************************************************************************/

#ifndef _TIMER_H
#define _TIMER_H

#include <helpers/BasicTypes.h>


namespace uadl {

  class DynParms;

/*******************************************************************************
 * basic counter class
 *
 * NOTE:
 *   - We assume the valid count range is less than half of the type (64-bit
 *     integer) range.
 ******************************************************************************/
class Counter {
public:
  static bool isGreater(uint64_t newCount, uint64_t oldCount);
  static bool isLesser(uint64_t newCount, uint64_t oldCount);
  static bool isGreaterOrEqual(uint64_t newCount, uint64_t oldCount);
  static bool isLesserOrEqual(uint64_t newCount, uint64_t oldCount);
};


inline bool Counter::isGreater(uint64_t newCount, uint64_t oldCount)
{ return newCount > oldCount; }
// consider integer overflow
// { return isGreaterOrEqual(newCount, oldCount) && newCount != oldCount; }


inline bool Counter::isLesser(uint64_t newCount, uint64_t oldCount)
{ return isGreater(oldCount, newCount); }


inline bool Counter::isGreaterOrEqual(uint64_t newCount, uint64_t oldCount)
{ return newCount >= oldCount; }
// consider integer overflow
// { return ((int64_t)(newCount - oldCount) > 0) == (newCount > oldCount); }


inline bool Counter::isLesserOrEqual(uint64_t newCount, uint64_t oldCount)
{ return isGreaterOrEqual(oldCount, newCount); }



/*******************************************************************************
 * basic timer class
 ******************************************************************************/

// time type
using adl::ttime_t;

class Timer : public Counter {
public:
  Timer(ttime_t startTime = 0) :
    _time(startTime), _invalidTime(startTime - 1) {}

  virtual ~Timer() {}

  // access/query methods
  const ttime_t &currTime() const { return _time; }
  ttime_t invalidTime() const { return _invalidTime; }

  bool timePassed(ttime_t time) const { return isNewTime(currTime(), time); }
  bool timeAtOrPassed(ttime_t time) const;

  static bool isNewTime(ttime_t newTime, ttime_t oldTime);
  static bool isOldTime(ttime_t oldTime, ttime_t newTime);
  static bool isNewOrSameTime(ttime_t newTime, ttime_t oldTime);
  static bool isOldOrSameTime(ttime_t oldTime, ttime_t newTime);

  void incrTime(ttime_t step = 1) { _time += step; }
  void setTime(ttime_t new_time) { _time = new_time; };

  // Kind of a hack having the dynamic-parameter registration in this class,
  // but it's the class that gets passed around, so it makes implementation
  // easier.  By default, does nothing.
  virtual void register_dyn_parms(DynParms &dp) {};

private:
  ttime_t _time;              // current time
  const ttime_t _invalidTime; // time that takes the longest time to reach
};


inline bool Timer::timeAtOrPassed(ttime_t time) const
{ return isNewOrSameTime(currTime(), time); }


inline bool Timer::isNewTime(ttime_t newTime, ttime_t oldTime)
{ return isGreater(newTime, oldTime); }


inline bool Timer::isOldTime(ttime_t oldTime, ttime_t newTime)
{ return isLesser(oldTime, newTime); }


inline bool Timer::isNewOrSameTime(ttime_t newTime, ttime_t oldTime)
{ return isGreaterOrEqual(newTime, oldTime); }


inline bool Timer::isOldOrSameTime(ttime_t oldTime, ttime_t newTime)
{ return isLesserOrEqual(oldTime, newTime); }

} // namespace uadl

#endif // _TIMER_H

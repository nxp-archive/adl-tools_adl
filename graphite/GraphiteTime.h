
#ifndef _GRAPHITE_TIME_H_
#define _GRAPHITE_TIME_H_

#include <cmath>
#include <sstream>
#include <stdexcept>

#include "helpers/Macros.h"

namespace adl {

  class Latency
  {
  public:
    Latency(uint64_t cycles = 0, double frequency = 0)
      : _cycles(cycles), _frequency(frequency) {}
    Latency(const Latency& lat)
    :_cycles(lat._cycles), _frequency(lat._frequency) {}
    ~Latency() {}

    Latency operator+(const Latency& lat) const;
    Latency operator+=(const Latency& lat);

    uint64_t toPicosec() const;

    uint64_t getCycles() const { return _cycles; }

  private:
    uint64_t _cycles;
    double _frequency;
  };

  class Time
  {
  public:
    explicit Time(uint64_t picosec = 0): _picosec(picosec) {}
    Time(const Time& time): _picosec(time._picosec) {}
    Time(const Latency& lat): _picosec(lat.toPicosec()) {}
    ~Time(){};

    Time operator+(const Latency& lat) const
    { return Time (_picosec + lat.toPicosec()); }

    Time operator+(const Time& time) const
    { return Time(_picosec + time._picosec); }

    Time operator-(const Time& time) const
    { return Time(_picosec - time._picosec); }

    bool operator==(const Time& time) const
    { return _picosec == time._picosec; }

    bool operator!=(const Time& time) const
    { return _picosec != time._picosec; }

    bool operator<(const Time& time) const
    { return _picosec < time._picosec; }

    bool operator>(const Time& time) const
    { return _picosec > time._picosec; }

    bool operator<=(const Time& time) const
    { return _picosec <= time._picosec; }

    bool operator>=(const Time& time) const
    { return _picosec >= time._picosec; }

    bool operator==(const uint64_t& picosec) const
    { return _picosec == picosec; }

    bool operator!=(const uint64_t& picosec) const
    { return _picosec != _picosec; }

    bool operator<(const uint64_t& picosec) const
    { return _picosec < picosec; }

    bool operator>(const uint64_t& picosec) const
    { return _picosec > picosec; }

    void operator+=(const Time& time)
    { _picosec += time._picosec; }

    void operator-=(const Time& time)
    { _picosec -= time._picosec; }

    uint64_t toCycles(double frequency) const;
    uint64_t getTime() const { return _picosec; }
      
    uint64_t toPicosec() const { return _picosec; }
    uint64_t toNanosec() const;
    double toSec() const;

  private:
    uint64_t _picosec;
  };


  inline uint64_t Latency::toPicosec() const
  {
    uint64_t picosec = (uint64_t) ceil( ((double) 1000*_cycles) /  ((double) _frequency) );

    return picosec;
  }

  inline Latency Latency::operator+(const Latency& lat) const
  {
    RAssert(_frequency == lat._frequency,"Attempting to add latencies from different frequencies");

    return Latency(_cycles + lat._cycles, _frequency);
  }

  inline Latency Latency::operator+=(const Latency& lat)
  {
    RAssert(_frequency == lat._frequency,"Attempting to add latencies from different frequencies");
    _cycles += lat._cycles;
    return *this;
  }

  inline uint64_t Time::toCycles(double frequency) const
  {
    uint64_t cycles = (uint64_t) ceil(((double) (_picosec) * ((double) frequency)) / double(1.0e3));

    return cycles;
  }

  inline uint64_t Time::toNanosec() const
  {
    return (uint64_t) ceil(((double) _picosec)/double(1.0e3));
  }

  inline double Time::toSec() const
  {
    return ((double) _picosec) / double(1.0e12);
  }

}

#endif

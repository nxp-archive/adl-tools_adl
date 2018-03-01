//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Test of a simple custom resource.  This basically is just a semaphore.
//

#include <sstream>

#include "uadl/UadlTypes.h"
#include "uadl/Logger.h"

#ifndef _DLX_CUSTOM_CR_
#define _DLX_CUSTOM_CR_

template <class ModelType>
class MyCounter : public uadl::Resource {
public:
  MyCounter(uadl::Timer &model,int max) : Resource(model), _count(0), _max(max) {};

  void reset() { _count = 0; };

  virtual void createLogger(uadl::LogClient *lc,unsigned tflags) {
    if (!_logger) {
      _logger = new uadl::Logger("MyCounter",lc,tflags);
    } else {
      _logger->setClient(lc,tflags);
    }
  };

  virtual void removeLogger() {
    delete _logger;
    _logger = 0;
  };

  virtual bool report_and_clear(std::ostream &o,const std::string &pfx) {
    if (_logger) {
      return _logger->report_and_clear(o,pfx);
    } else {
      return false;
    }    
  }

  void preCycle()
  {
    if (_logger) {
      _logger->logMessage("In preCycle.");
    }
  }
  
  void postCycle()
  {
    if (_logger) {
      _logger->logMessage("In postCycle.");
    }
  }

  bool can_allocate(ModelType &uarch,uadl::InstrBase &,uadl::Logger *logger) const { 
    bool ok = (_count < _max);
    if (logger && !ok) {
      logger->logMessage("Failed to acquire counter.");
    }
    return ok; 
  };

  void allocate(ModelType &uarch,uadl::InstrBase &instr,uadl::Logger *logger) { 
    ++_count; 
    if (logger) {
      std::ostringstream ss;
      ss << "Allocated counter:  " << std::dec << _count << " / " << _max;
      logger->logMessage(ss.str());
    }
  };
  void deallocate(ModelType &uarch,uadl::InstrBase &instr,uadl::Logger *logger) { 
    --_count; 
    if (logger) {
      std::ostringstream ss;
      ss << "Deallocated counter:  " << std::dec << _count << " / " << _max;
      logger->logMessage(ss.str());
    }
  };

protected:
  int _count, _max;

  uadl::Logger *_logger;
};

#endif

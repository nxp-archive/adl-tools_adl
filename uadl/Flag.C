//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#include <sstream>

#include "helpers/Macros.h"

#include "UadlArch.h"
#include "Logger.h"
#include "Flag.h"

namespace uadl {
	struct FlagLogItem;
}

using namespace std;

namespace uadl {

  // Tracks ownership of semaphore resources.
  struct FlagLogItem : public LogQueueItem {
    FlagLogItem(const Flag *f,unsigned count,uint64_t id) :
      _f(f), _count(count), _id(id) {};

    bool matching_alloc(const Flag *f) const {
      return (f == _f) && (_count > 0);
    }

    const Flag      *_f;
    unsigned         _count;
    uint64_t         _id;
  };

  // We weren't able to get the flag, so look backwards over the central
  // resource log, finding who still has the flag.  We just report on the
  // first one we find.
  void Flag::log_acquire_fail(UadlArch &uarch,Logger *logger) const
  {
    LogQueue &lq = uarch.log_queue();

    ostringstream ss;
    ss << "cannot acquire flag " << name();
    ForEach(lq,i) {
      const LogQueueItem *li = *i;
      if (auto fi = li->isa<FlagLogItem>()) {
        if (fi->matching_alloc(this)) {
          ss << " (holder:  iid=" << fi->_id;
          if (_time != numeric_limits<ttime_t>::max()) {
            ss << ", available at time " << _time;
          }
          ss << ")";
          break;
        }
      }
    }
    logger->logStall(ss.str());
  }

  void Flag::log_action(UadlArch &uarch,Logger *logger,const char *msg,int count,uint64_t id)
  {
    logger->logMessage(msg + name());
    uarch.log_queue().add(new FlagLogItem(this,count,id));
  }

}

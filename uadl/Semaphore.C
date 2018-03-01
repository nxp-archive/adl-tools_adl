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
#include "Semaphore.h"

using namespace std;

namespace uadl {
	struct SemaLogItem;
}

namespace uadl {

  // Tracks ownership of semaphore resources.
  struct SemaLogItem : public LogQueueItem {
    SemaLogItem(const Semaphore *s,unsigned count,uint64_t id) :
      _s(s), _count(count), _id(id) {};

    bool matching_alloc(const Semaphore *s) const {
      return (s == _s) && (_count > 0);
    }

    const Semaphore *_s;
    unsigned         _count;
    uint64_t         _id;
  };

  // We weren't able to get the semaphore, so look backwards over the central
  // resource log, finding who still has the semaphore.  We just report on the
  // first one we find.
  void Semaphore::log_acquire_fail(UadlArch &uarch,Logger *logger) const
  {
    LogQueue &lq = uarch.log_queue();

    ostringstream ss;
    ss << "cannot acquire semaphore " << name();
    ForEach(lq,i) {
      const LogQueueItem *li = *i;
      if (auto si = li->isa<SemaLogItem>()) {
        if (si->matching_alloc(this)) {
          ss << " (count: " << _count << ", holder: iid=" << si->_id << ")";
          break;
        }
      }
    }
    logger->logStall(ss.str());
  }

  void Semaphore::log_action(UadlArch &uarch,Logger *logger,const char *msg,unsigned cur_count,int count,uint64_t id)
  {
    ostringstream ss;
    ss << msg << name() << " (count: " << cur_count << ")";
    logger->logMessage(ss.str());
    uarch.log_queue().add(new SemaLogItem(this,count,id));
  }

}

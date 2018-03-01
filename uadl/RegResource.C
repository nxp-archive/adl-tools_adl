//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#include <sstream>
#include <string>

#include "helpers/Macros.h"

#include "UadlArch.h"
#include "RegResource.h"

using namespace std;

namespace uadl {
	struct RegLogItem;
}

namespace uadl {

  // Tracks ownership of register/register-file resources.
  struct RegLogItem : public LogQueueItem {
    RegLogItem(const RegResourceBase *reg,int index,uint64_t mask,uint64_t id) :
      _reg(reg), _index(index), _mask(mask), _id(id) {};

    bool matching_alloc(const RegResourceBase *reg,int index,uint64_t mask) const {
      return ((reg == _reg) && (index == _index) && (mask & _mask));
    }

    const RegResourceBase *_reg;
    int                    _index;
    uint64_t               _mask;
    uint64_t               _id;
  };


  void RegResourceBase::fullName(ostream &o,int index) const
  {
    o << name();
    if (index >= 0) {
      o << "[" << dec << index << "]";
    }
  }

  void RegResourceBase::findOwner(ostream &o,UadlArch &uarch,int index,uint64_t mask) const
  {
    LogQueue &lq = uarch.log_queue();

    ForEach(lq,i) {
      const LogQueueItem *li = *i;
      if (auto ri = li->isa<RegLogItem>()) {
        if (ri->matching_alloc(this,index,mask)) {
          o << " (RAW hazard, holder:  id=" << dec << ri->_id << ")";
          return;
        }
      }
    }
  }

  void RegResourceBase::logStall(UadlArch &uarch,Logger *logger,int index,const char *msg) const
  {
    ostringstream ss;
    ss << "cannot " << msg << " ";
    fullName(ss,index);
    findOwner(ss,uarch,index,-1);
    logger->logStall(ss.str());
  }

  void RegResourceBase::logStall(UadlArch &uarch,Logger *logger,int index,const char *msg,uint64_t mask) const
  {
    ostringstream ss;
    ss << "cannot " << msg << " ";
    fullName(ss,index);
    ss << "(0x" << hex << mask << ")";
    findOwner(ss,uarch,index,mask);
    logger->logStall(ss.str());
  }

  void RegResourceBase::logAlloc(UadlArch &uarch,Logger *logger,uint64_t id,int index,bool alloc) const
  {
    ostringstream ss;
    ss << ((alloc) ? "allocate" : "deallocate") << " ";
    fullName(ss,index);
    logger->logMessage(ss.str());
    if (alloc) {
      uarch.log_queue().add(new RegLogItem(this,index,-1ull,id));
    }
  }

  void RegResourceBase::logAlloc(UadlArch &uarch,Logger *logger,uint64_t id,int index,bool alloc,uint64_t mask) const
  {
    ostringstream ss;
    ss << ((alloc) ? "allocate" : "deallocate") << " ";
    fullName(ss,index);
    ss << "(0x" << hex << mask << ")";
    logger->logMessage(ss.str());
    if (alloc) {
      uarch.log_queue().add(new RegLogItem(this,index,mask,id));
    }
  }

  string getRegName(const string &name,unsigned index)
  {
    ostringstream ss;
    ss << name << '[' << index << ']';
    return ss.str();
  }

}

//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef _LOG_QUEUE_H_
#define _LOG_QUEUE_H_

#include <list>

namespace uadl {

  //
  // The LogQueue is used as a centralized resource within a core to track who
  // owns what.  It's only used during logging, as otherwise we don't care- we
  // just know whether a resource is or is not available.  This just consists of
  // a queue of base elements, where the queue has a maximum size (normally
  // large).  When a resource wants to know who currently owns something, it
  // traverses backwards, finding items of the appropriate type, then examining
  // them as needed.
  //
  // Note: The queue owns all items once added.
  struct LogQueueItem {
    virtual ~LogQueueItem() {};

    template <class T>
    const T* isa() const { return dynamic_cast<const T*>(this); };
  };

  struct LogQueue : private std::list<LogQueueItem *> {
    typedef std::list<LogQueueItem *> Base;
	typedef Base::iterator iterator;
	typedef Base::const_iterator const_iterator;

    LogQueue(unsigned maxsize) : _size(0), _maxsize(maxsize) {};
    ~LogQueue() { reset(); };

    void add(LogQueueItem *item) {
      push_front(item);
      if ( ++_size > _maxsize ) {
        --_size;
        LogQueueItem *tmp = back();
        pop_back();
        delete tmp;
      }
    }

    using Base::begin;
    using Base::end;

    void reset();
  protected:
    unsigned _size;
    unsigned _maxsize;
  };


}

#endif

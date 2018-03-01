//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
// This implements a simple circular queue of objects.  Memory allocation is
// only done at construction time, after which the queue is fixed in size.
//
// Note:  Default operation is on the dangerous side, as no size checks are performed.  
// These checks can be enabled via the template parm CheckSize.
//

// 
// Simple regression program for testing the circular queue classes.
//

#include <cstdlib>
#include <assert.h>
#include <iostream>
#include <new>

#include "iss/CircularQueue.h"

using namespace std;
using namespace adl;


// Basic test of a fixed-sized queue.
void cqtest1()
{
  CircularQueue<int,16,true> cq;

  assert(cq.empty());

  cq.push_back(10);
  cq.push_back(20);
  cq.erase_middle(0,2);
  assert(cq.empty());

  cq.push_back(1);
  cq.push_back(2);

  int *x;
  x = cq.push_back();
  new (x) int(3);

  x = cq.push_back();
  new (x) int(4);

  assert(cq[2] == 3);
  assert(!cq.full());
  assert(!cq.empty());
  assert(cq.size() == 4);

  cq.push_back(5);
  cq.push_back(6);
  cq.push_back(7);
  cq.push_back(8);
  cq.push_back(9);
  cq.push_back(10);
  cq.push_back(11);
  cq.push_back(12);
  cq.push_back(13);
  cq.push_back(14);
  cq.push_back(15);

  assert(cq.full());

  cq.pop_back();
  cq.pop_back();
  assert(cq.back() == 13);
  assert(cq.front() == 1);
  
  cq.pop_back();
  cq.pop_front();
  cq.pop_back();
  cq.pop_front();

  // Should now be:  3 4 5 6 7 8 9 10 11
  assert(cq.back() == 11);
  assert(cq.front() == 3);
  
  cq.erase_middle(3,6);
  cq.erase(2);

  // Should now be 3 4 9 10 11
  assert(cq[2] == 9);
  assert(cq[3] == 10);

  cq.pop_front();
  cq.pop_front();
  cq.pop_back();
  cq.pop_back();
  cq.pop_back();

  assert(cq.empty());
}

// Another test, this time of a queue sized at construction time.
void dcqtest1()
{
  DCircularQueue<int,true> cq(12);

  assert(cq.empty());

  cq.push_back(10);
  cq.push_back(20);
  cq.erase_middle(0,2);
  assert(cq.empty());


  cq.push_back(1);
  cq.push_back(2);
  cq.push_back(3);
  cq.push_back(4);

  assert(cq.capacity() == 12);
  assert(cq[2] == 3);
  assert(!cq.full());
  assert(!cq.empty());
  assert(cq.size() == 4);

  cq.push_back(5);
  cq.push_back(6);
  cq.push_back(7);
  cq.push_back(8);
  cq.push_back(9);
  cq.push_back(10);
  cq.push_back(11);
  cq.push_back(12);

  assert(cq.full());

  cq.pop_back();
  cq.pop_back();
  assert(cq.back() == 10);
  assert(cq.front() == 1);

  cq.push_back(11);
  cq.pop_front();
  cq.pop_front();

  // Should now be:  3 4 5 6 7 8 9 10 11
  assert(cq.back() == 11);
  assert(cq.front() == 3);
  
  cq.erase_middle(3,6);
  cq.erase(2);

  // Should now be 3 4 9 10 11
  assert(cq[2] == 9);
  assert(cq[3] == 10);

  cq.pop_front();
  cq.pop_front();
  cq.pop_back();
  cq.pop_back();
  cq.pop_back();

  assert(cq.empty());

  // Now add on items; this will test that we handle the circular nature
  // correctly, since head has been advanced due to various pop_front
  // operations.
  int *x;
  x = cq.push_back();
  *x = 1;

  x = cq.push_back();
  *x = 2;

  x = cq.push_back();
  *x = 3;

  cq.push_back(4);
  cq.push_back(5);
  cq.push_back(6);
  cq.push_back(7);
  cq.push_back(8);
  cq.push_back(9);
  cq.push_back(10);
  cq.push_back(11);
  cq.push_back(12);

  assert(cq.full());
  assert(!cq.empty());
  assert(cq.size() == 12);

  cq.pop_back();
  cq.pop_back();
  cq.pop_front();
  cq.pop_front();

  // Queue is now 3 4 5 6 7 8 9 10
  assert(cq.front() == 3);
  assert(cq.back() == 10);

  cq.erase_middle(4,6);

  // Queue is now 3 4 5 6 9 10
  assert(cq.size() == 6);
  assert(cq[4] == 9);
  assert(cq.back() == 10);

  cq.clear();
  cq.push_back(1);
  cq.push_back(2);
  cq.push_back(3);

  assert(cq.find(2) == 1);

  cq.pop_front();
  cq.pop_front();

  cq.push_back(4);
  cq.push_back(5);
  cq.push_back(6);

  assert(cq.find(6) == 3);

  // Another test of find, this time with a wrap-around.
  // The current size of the queue is 12.
  cq.pop_front();
  cq.pop_front();
  cq.push_back(7);
  cq.push_back(8);
  cq.push_back(9);
  cq.push_back(10);
  cq.push_back(11);
  cq.push_back(12);
  cq.push_back(13);
  cq.push_back(14);
  cq.push_back(15);

  assert(cq.find(13) == 8 && cq.find(14) == 9 && cq.find(15) == 10);

  // Make sure that we're only checking valid data.
  cq.clear();
  assert(cq.find(13) == -1);
}

// Make sure we can use the queue without checking, where we silently drop items
// if we get full.
void dcqtest2()
{
  DCircularQueue<int,true> cq(12);

  // Resize to a new, smaller size, then push a bunch of items on, and then
  // check.
  cq.resize(3);
  if (cq.full()) cq.pop_front();
  cq.push_back(1);
  if (cq.full()) cq.pop_front();
  cq.push_back(2);
  if (cq.full()) cq.pop_front();
  cq.push_back(3);
  if (cq.full()) cq.pop_front();
  cq.push_back(4);
  if (cq.full()) cq.pop_front();
  cq.push_back(5);
  if (cq.full()) cq.pop_front();
  cq.push_back(6);

  assert(cq.back() == 6);
  assert(cq.front() == 4);
}

// Another test, this time of a dynamically-sized queue.
void dscqtest1()
{
  DSCircularQueue<int,true> cq;

  assert(cq.empty());
  assert(!cq.full());

  cq.push_back(10);
  cq.push_back(20);
  cq.erase_middle(0,2);
  assert(cq.empty());

  cq.push_back(1);
  cq.push_back(2);
  cq.push_back(3);
  cq.push_back(4);

  assert(cq[2] == 3);
  assert(!cq.full());
  assert(!cq.empty());
  assert(cq.size() == 4);

  cq.push_back(5);
  cq.push_back(6);
  cq.push_back(7);
  cq.push_back(8);
  cq.push_back(9);
  cq.push_back(10);
  cq.push_back(11);
  cq.push_back(12);

  cq.pop_back();
  cq.pop_back();
  assert(cq.back() == 10);
  assert(cq.front() == 1);

  cq.push_back(11);
  cq.pop_front();
  cq.pop_front();

  // Should now be:  3 4 5 6 7 8 9 10 11
  assert(cq.back() == 11);
  assert(cq.front() == 3);
  
  cq.erase_middle(3,6);
  cq.erase(2);

  // Should now be 3 4 9 10 11
  assert(cq[2] == 9);
  assert(cq[3] == 10);

  cq.pop_front();
  cq.pop_front();
  cq.pop_back();
  cq.pop_back();
  cq.pop_back();

  assert(cq.empty());

  // Now add on items; this will test that we handle the circular nature
  // correctly, since head has been advanced due to various pop_front
  // operations.
  int *x;
  x = cq.push_back();
  *x = 1;

  x = cq.push_back();
  *x = 2;

  x = cq.push_back();
  *x = 3;

  cq.push_back(4);
  cq.push_back(5);
  cq.push_back(6);
  cq.push_back(7);
  cq.push_back(8);
  cq.push_back(9);
  cq.push_back(10);
  cq.push_back(11);
  cq.push_back(12);

  assert(!cq.empty());
  assert(cq.size() == 12);

  cq.pop_back();
  cq.pop_back();
  cq.pop_front();
  cq.pop_front();

  // Queue is now 3 4 5 6 7 8 9 10
  assert(cq.front() == 3);
  assert(cq.back() == 10);

  cq.erase_middle(4,6);

  // Queue is now 3 4 5 6 9 10
  assert(cq.size() == 6);
  assert(cq[4] == 9);
  assert(cq.back() == 10);

  cq.clear();
  
  // Now, push on a whole bunch of stuff to make sure we resize properly.
  for (int oloop = 0; oloop != 5; ++oloop) {
    for (int i = 0; i != 1000; ++i) {
      cq.push_back(i);
    }
    
    int checker = 0;
    while (!cq.empty()) {
      if ( checker++ != cq.front() ) {
        cout << "Bad pop_front value:  " << checker << endl;
        abort();
      }
      cq.pop_front();
    }
  }

  // Since we pushed on 1000 items, the closest power of 2 is 1024, with one
  // item wasted.
  assert(cq.capacity() == 1023);
}

// Another test of dynamically sized queues.
void dscqtest2()
{
  DSCircularQueue<int,true> cq;

  // Add an item, then remove it to advance the head.
  cq.push_back(1);
  assert(cq.front() == 1);
  cq.pop_front();
  
  // Now add 4 items to force a resize, then make sure that the items are
  // correct.
  cq.push_back(10);
  cq.push_back(11);
  cq.push_back(12);
  cq.push_back(13);

  assert(cq.front() == 10);
  cq.pop_front();
  assert(cq.front() == 11);
  cq.pop_front();
  assert(cq.front() == 12);
  cq.pop_front();
  assert(cq.front() == 13);
  cq.pop_front();

  assert(cq.empty());
}

// Another test of dynamically sized queues.
void dscqtest3()
{
  DSCircularQueue<int,true> cq;

  // Add some items, then remove them to advance the head.
  cq.push_back(1);
  cq.push_back(2);
  assert(cq.front() == 1);
  assert(cq.back() == 2);
  cq.pop_front();
  cq.pop_front();
  
  // Now add 4 items to force a resize, then make sure that the items are
  // correct.
  cq.push_back(10);
  cq.push_back(11);
  cq.push_back(12);
  cq.push_back(13);

  assert(cq.front() == 10);
  cq.pop_front();
  assert(cq.front() == 11);
  cq.pop_front();
  assert(cq.front() == 12);
  cq.pop_front();
  assert(cq.front() == 13);
  cq.pop_front();

  assert(cq.empty());
}

int main()
{
  cqtest1();
  dcqtest1();
  dcqtest2();
  dscqtest1();
  dscqtest2();
  dscqtest3();
  return 0;
}

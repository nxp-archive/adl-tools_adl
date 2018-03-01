//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
//
// External resources used by mod73.adl
//

#ifndef _MOD73_EXTRES_H_
#define _MOD73_EXTRES_H_

// Used by mod73.  Performs mathematical functions, plus counts the number of
// operations performed.
class Foo {
public:
  Foo(int,int,int) : _op_count(0) {};

  uint32_t add(uint32_t x,uint32_t y)      { ++_op_count; return (x + y); };
  uint32_t multiply(uint32_t x,uint32_t y) { ++_op_count; return (x * y); };

  void set_op_count(int o) { _op_count = o; };
  int op_count() const { return _op_count; };

  void reset(bool,bool) { _op_count = 0; };

protected:
  int _op_count;
};

// Not actually used.
class Bar {
public:
  Bar() {};

  void reset(bool,bool) {};
};

#endif

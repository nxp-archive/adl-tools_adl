//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#include <iostream>

#include "helpers/bits.h"

using namespace std;
using namespace adl;

int main()
{
  bits<20> x("100");
  bits<48> y("256");

  x += y;

  if (x != 356) {
    cout << "Did not compare." << endl;
    return 1;
  } else {
    cout << "Did compare." << endl;
  }

  if (x != y) {
    cout << "Good- did not compare." << endl;
  } else {
    cout << "Bad- did compare." << endl;
    return 1;
  }

  x.set(0,1);
  if (!x(0)) {
    cout << "Error:  did not set bit." << endl;
    return 1;
  }

  x.setUndefined(0,10);

  bits<64> z = x + y;

  bits<48> w = y - x;

  bits<64> a("0x7fffffffffffffff");
  //  bits<64> a("0xccccccccccccccc");
  bits<64> b("0xa");

  //  bits<64> res = a / b;
  //  cout << hex << "result:  " << res << endl;

  //#if 0
  cout << "Any part of x undefined?  " << x.isUndefined() << endl;
  cout << "[5:9] undefined?  " << x.isUndefined(5,9) << endl;
  cout << "[11:15] undefined?  " << x.isUndefined(11,15) << endl;
  cout << "Any part of z undefined?  " << z.isUndefined() << endl;
  cout << "z:  " << z << endl;
  cout << dec << "a:  " << a << endl;
  //  cout << hex << "w:  " << w << endl;
  //#endif

  bits<64> zz("9223372036854775807");
  cout << hex << "zz:  " << zz << endl;
}


int pMain(int, char const**)
{ return 0; }

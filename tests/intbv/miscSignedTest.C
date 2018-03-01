// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
//  
// This is a simple test of various corner cases for signed bit-vector
// arithmetic.
//

#include <iostream>

#include "Macros.h"
#include "sintbv.h"
#include "le_sintbv.h"

using namespace std;
using namespace adl;

#define sbits    sintbv
#define bits     intbv
#define le_sbits le_sintbv
#define le_bits  le_intbv

// Just add on various tests, as they come up.  Use an assert to test that the
// outcome is as expected.
int main()
{
  {
    //
    // Test: An addition of a signed bit vector with a signed immediate should
    // result in a signed intermediate value, so the shift right should be
    // sign-propagating.
    //
    sbits <41> mult (0xffcd25903e00ull);
    mult = (mult + (sbits<32>) 0x8000 )>>16;

    sbits <41> mult1 (0xffcd25903e00ull);
    mult1 = (mult1 + 0x8000 )>>16;

    assert(mult == mult1);
  }

  {
    //
    // Test: Same test as above, using the little-endian version.
    //
    le_sbits <41> mult (0xffcd25903e00ull);
    mult = (mult + (le_sbits<32>) 0x8000 )>>16;

    le_sbits <41> mult1 (0xffcd25903e00ull);
    mult1 = (mult1 + 0x8000 )>>16;

    assert(mult == mult1);
  }

  {
    //
    // Test: Make sure that the sbitsSignedMultiply exists and works.  It should
    // return a signed value, so that a shift performs an arithmetic shift.
    //
    bits<32> x = -10;
    bits<32> y = 20;
    
    auto r = sbitsSignedMultiply(x,y) >> 2;

    assert(r == -50);
  }

  {
    //
    // Test: Same as above, using little-endian.  We take a slice in order to
    // make sure that this works.
    //
    le_bits<32> x = -20;
    le_bits<32> y = 30;
    
    auto r = (le_sbitsSignedMultiply(x,y) >> 2).get<12>(11,0);

    assert(r == -150);
  }

  {
    //
    // Test:  A sign-extended right shift should always sign extend, even if it shifts
    // off the end of the bit vector.
    //

    // Sign bit not set, shift < bit vector size.
    bits<32> x = 0x12345678;
    x.signedRightShiftEqual(12);
    assert(x == 0x12345);

    // Sign bit set, shift < bit vector size.
    x = 0x92345678;
    x.signedRightShiftEqual(12);
    assert(x == 0xfff92345);

    // Sign bit not set, shift > bit vector size.
    x = 0x12345678;
    x.signedRightShiftEqual(34);
    assert(x == 0);

    // Sign bit set, shift > bit vector size.
    x = 0x92345678;
    x.signedRightShiftEqual(34);
    assert(x == 0xffffffff);
  }
  
  {
    //
    // Same as above, using little-endian values.
    //

    // Sign bit not set, shift < bit vector size.
    le_bits<32> x = 0x12345678;
    x.signedRightShiftEqual(12);
    assert(x == 0x12345);

    // Sign bit set, shift < bit vector size.
    x = 0x92345678;
    x.signedRightShiftEqual(12);
    assert(x == 0xfff92345);

    // Sign bit not set, shift > bit vector size.
    x = 0x12345678;
    x.signedRightShiftEqual(34);
    assert(x == 0);

    // Sign bit set, shift > bit vector size.
    x = 0x92345678;
    x.signedRightShiftEqual(34);
    assert(x == 0xffffffff);
  }

  {
    //
    // Same as above, using signed bit vectors.
    //

    // Sign bit not set, shift < bit vector size.
    sbits<32> x = 0x12345678;
    x >>= 12;
    assert(x == 0x12345);

    // Sign bit set, shift < bit vector size.
    x = 0x92345678;
    x >>= 12;
    assert(x == 0xfff92345);

    // Sign bit not set, shift > bit vector size.
    x = 0x12345678;
    x >>= 34;
    assert(x == 0);

    // Sign bit set, shift > bit vector size.
    x = 0x92345678;
    x >>= 34;
    assert(x == 0xffffffff);
  }

  {
    //
    // Same as above, using little-endian signed bit vectors.
    //

    // Sign bit not set, shift < bit vector size.
    le_sbits<32> x = 0x12345678;
    x >>= 12;
    assert(x == 0x12345);

    // Sign bit set, shift < bit vector size.
    x = 0x92345678;
    x >>= 12;
    assert(x == 0xfff92345);

    // Sign bit not set, shift > bit vector size.
    x = 0x12345678;
    x >>= 34;
    assert(x == 0);

    // Sign bit set, shift > bit vector size.
    x = 0x92345678;
    x >>= 34;
    assert(x == 0xffffffff);
  }

  {
    //
    // Sanity check- make sure this works for 64-bits as well.
    //

    bits<64> x = 0x1234567812345678ull;
    x.signedRightShiftEqual(16);
    assert(x == 0x123456781234ull);

    bits<64> y = 0x9234567812345678ull;
    y.signedRightShiftEqual(16);
    assert(y == 0xffff923456781234ull);
  }


  {
    //
    // We should be able to create a 0-sized integer; it should just always
    // return 0.
    //

    bits<0> x = 0;
    bits<32> y = x;

    assert(x == 0);
    assert(y == 0);
  }

  {
    //
    // We should be able to extract a single bit from a signed intbv and test it
    // against 1.
    //
    sbits<16>    s1(0x8000);
    le_sbits<16> s2(0x8000);
    
    assert(s1(0) == 1);
    assert(s2(15) == 1);
  }

}

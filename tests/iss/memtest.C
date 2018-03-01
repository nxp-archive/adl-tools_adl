//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Tests the functionality of the ISS sparse memory model.
//

#include <ctime>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include <boost/random/mersenne_twister.hpp>
#include <boost/random/uniform_int_distribution.hpp>

typedef boost::random::mt19937 random_gen;
typedef boost::random::uniform_int_distribution<unsigned> random_dist;

const bool Tracing = 0;

#include "helpers/Macros.h"

// Note: Normally this file should never be included directly by anything other
// than a generated test.  However, this is a special case, as it's just a
// regression driver program.
#define __IN_IMPLEMENTATION__
#include "iss/ModelSupport.h"

#ifndef InterleavedFlag
# error "No value specified for InterleavedFlag (expected true or false)."
#endif

#ifndef SplitSizeFlag
# error "No value specified for SplitSizeFlag (expected 32 or 64)."
#endif

#ifdef USE_INTBV
#define bits intbv
#else
#define bits bits
#endif


#include "iss/ModelInterface.h"
#include "iss/MemoryInterface.h"
#include "iss/DefaultMem.h"
#include "iss/GlobalMemoryName.h"

using namespace std;
using namespace adl;

const int Debug = 0;

const unsigned NumTests = 1000;
unsigned Count = 0;

namespace adl {

  const AttrData &getAttrData()
  {
    static AttrData dummy;
    return dummy;
  }

}

typedef  SimpleCache< decltype(GlobalMemory)& > NullCache;

uint64_t data_read64(addr_t addr)
{
  NullTranslate t;
  NullReadLog l;
  NullCache c(GlobalMemory);
  MemoryAccess<NullTranslate,NullCache,NullReadLog,InterleavedFlag,SplitSizeFlag> ma;
  return ma.read64(t,c,l,addr);
}

uint32_t data_read32(addr_t addr)
{
  NullTranslate t;
  NullReadLog l;
  NullCache c(GlobalMemory);
  MemoryAccess<NullTranslate,NullCache,NullReadLog,InterleavedFlag,SplitSizeFlag> ma;
  return ma.read32(t,c,l,addr);
}

uint16_t data_read16(addr_t addr)
{
  NullTranslate t;
  NullReadLog l;
  NullCache c(GlobalMemory);
  MemoryAccess<NullTranslate,NullCache,NullReadLog,InterleavedFlag,SplitSizeFlag> ma;
  return ma.read16(t,c,l,addr);
}

uint8_t data_read8(addr_t addr)
{
  NullTranslate t;
  NullReadLog l;
  NullCache c(GlobalMemory);
  MemoryAccess<NullTranslate,NullCache,NullReadLog,InterleavedFlag,SplitSizeFlag> ma;
  return ma.read8(t,c,l,addr);
}

void data_write64(addr_t addr,uint64_t value)
{
  NullTranslate t;
  NullWriteLog l;
  NullCache c(GlobalMemory);
  MemoryAccess<NullTranslate,NullCache,NullWriteLog,InterleavedFlag,SplitSizeFlag> ma;
  return ma.write64(t,c,l,addr,addr,value);
}

void data_write32(addr_t addr,uint32_t value)
{
  NullTranslate t;
  NullWriteLog l;
  NullCache c(GlobalMemory);
  MemoryAccess<NullTranslate,NullCache,NullWriteLog,InterleavedFlag,SplitSizeFlag> ma;
  return ma.write32(t,c,l,addr,addr,value);
}

void data_write16(addr_t addr,uint16_t value)
{
  NullTranslate t;
  NullWriteLog l;
  NullCache c(GlobalMemory);
  MemoryAccess<NullTranslate,NullCache,NullWriteLog,InterleavedFlag,SplitSizeFlag> ma;
  return ma.write16(t,c,l,addr,addr,value);
}

void data_write8(addr_t addr,uint8_t value)
{
  NullTranslate t;
  NullWriteLog l;
  NullCache c(GlobalMemory);
  MemoryAccess<NullTranslate,NullCache,NullWriteLog,InterleavedFlag,SplitSizeFlag> ma;
  ma.write8(t,c,l,addr,addr,value);
}

template <size_t NB>
bits<NB*8> mem_read (addr_t addr)
{
  NullTranslate t;
  NullReadLog l;
  NullCache c(GlobalMemory);
  read_internal<NullTranslate,NullCache,NullReadLog,NB,InterleavedFlag,SplitSizeFlag> ri;
  return ri(t,c,l,addr);
};
  
template <size_t NB,size_t Nb>
void mem_write (addr_t addr,bits<Nb> value)
{
  NullTranslate t;
  NullWriteLog l;
  NullCache c(GlobalMemory);
  write_internal<NullTranslate,NullCache,NullWriteLog,NB,Nb,InterleavedFlag,SplitSizeFlag> wi;
  wi(t,c,l,addr,addr,value);
};


// create a functor to pass to intbv for use in
//  generating random intbv's
class rand_functor {
private:
  mutable random_gen _gen;
  random_dist        _dist;
  unsigned           _seed;
public:
  // ctor
  rand_functor(unsigned s) : _gen(s), _seed(s) {}

  // the functor
  unsigned operator()() const { return _dist(_gen); } 
};

void test64(addr_t addr,uint64_t value)
{
  data_write64(addr,value);
  uint64_t result = data_read64(addr);
  DPrint(hex << "0x" << addr << ":  0x" << result << " (" << dec << result << ")\n");
  if (value != result) {
    RError(hex << "0x" << addr << ":  Wrote 0x" << value << ", read 0x" << result);
  }
  ++Count;
}

void check32(addr_t addr,uint32_t expect)
{
  uint32_t result = data_read32(addr);
  if (result != expect) {
    RError("Read from 0x" << hex << addr << " and found 0x" << result << " but expected to find " << expect);
  }
}

void test32(addr_t addr,uint32_t value)
{
  data_write32(addr,value);
  uint32_t result = data_read32(addr);
  DPrint(hex << "0x" << addr << ":  0x" << result << " (" << dec << result << ")\n");
  if (value != result) {
    RError(hex << "0x" << addr << ":  Wrote 0x" << value << ", read 0x" << result);
  }
  ++Count;
}

void check16(addr_t addr,uint16_t expect)
{
  uint16_t result = data_read16(addr);
  if (result != expect) {
    RError("Read from 0x" << hex << addr << " and found 0x" << result << " but expected to find " << expect);
  }
}

void test16(addr_t addr,uint16_t value)
{
  data_write16(addr,value);
  uint16_t result = data_read16(addr);
  DPrint(hex << "0x" << addr << ":  0x" << result << " (" << dec << result << ")\n");
  if (value != result) {
    RError(hex << "0x" << addr << ":  Wrote 0x" << value << ", read 0x" << result);
  }
  ++Count;
}

void test8(addr_t addr,uint8_t value)
{
  data_write8(addr,value);
  uint8_t result = data_read8(addr);
  DPrint(hex << "0x" << addr << ":  0x" << result << " (" << dec << result << ")\n");
  if (value != result) {
    RError(hex << "0x" << addr << ":  Wrote 0x" << value << ", read 0x" << result);
  }
  ++Count;
}

template <size_t NB>
void arbtest(rand_functor &rf,addr_t addr)
{
  bits<NB*8> value(0,rf);
  mem_write<NB>(addr,value);

  bits<NB*8> result = mem_read<NB>(addr);
  DPrint(hex << "0x" << addr << ":  wrote " << value << ", read " << result << "\n");

  if (value != result) {
    RError(hex << "0x" << addr << ":  wrote " << value << ", read " << result);
  }
  ++Count;
}

// I put this in a separate function b/c gcc 4.5.4 and gcc 4.6.3, with this when
// optimized, produce a failing test.  Seems like a compiler bug to me, though.
void check_result(byte_t *misaligned) ATTRIBUTE_NOINLINE;
void check_result(byte_t *misaligned)
{
  uint32_t fnd = byte_read32(misaligned,0x4);
  assert(fnd == 0xdeadbeef);
}

// Simple sanity check, which mainly matters for the TLM2 tests.
void check_simple_32()
{
  byte_t buf[2000];
  byte_t *misaligned = &buf[0x10c];
  // Bump this to make sure that we're not aligned on a double-word boundary.
  while ( ((unsigned long)misaligned & 0xf) != 0xc) ++misaligned;
  uint64_t orig = 0x12345678deadbeefull;
  byte_write64(misaligned,0x0,orig);
  //cout << "misaligned:  " << (void*)misaligned << '\n';
  check_result(misaligned);
}

// Check individual byte reads of a larger write and vice versa.
void check_byte_rw(addr_t addr)
{
  uint64_t d = 0xdeadbeef12345678ull;
  data_write64(addr,d);

  for (int i = 0; i != 8; ++i) {
    byte_t b = data_read8(addr+i);
    assert(b == ((d >> (8-i-1)*8) & 0xff));
  }

  addr += 0x10;
  for (int i = 0; i != 8; ++i) {
    data_write8(addr+i, ((d >> (8-i-1)*8) & 0xff) );
  }

  uint64_t f = data_read64(addr);
  assert(f == d);
  
}


int main(int argc,const char *argv[])
{
  try {
    //DebugMode = true;
    unsigned seed = std::time(0);
    if (argc > 1) {
      seed = strtoul(argv[1],0,0);
    }

    random_gen  gen(seed);
    random_dist dist;

    cout << "Seed:  0x" << hex << seed << dec << "\n";

    check_simple_32();

    test64(0x1008,0x12345678deadbeefULL);
    test64(0x1009,0x12345678deadbeefULL);
    test64(0x100a,0x12345678deadbeefULL);
    test64(0x100b,0x12345678deadbeefULL);
    test64(0x100c,0x12345678deadbeefULL);
    test64(0x100d,0x12345678deadbeefULL);
    test64(0x100e,0x12345678deadbeefULL);
    test64(0x100f,0x12345678deadbeefULL);
    test64 (0x200e,0x12345678deadbeefULL);
    check32(0x200c,0x00001234);
    check32(0x2014,0xbeef0000);

    check_byte_rw(0x20000);

    // A few specific tests.
    test32(0x1000,50);
    test32(0x1102,0xdeadbeef);

    test32(0x6,0xdeadbeef);

    test32(0xfffc,0x12345678);
    test32(0xfffd,0x12345678);
    test32(0xfffe,0x12345678);
    test32(0xffff,0x12345678);

    check32(0xfffc,0x12121212);

    test32(0x10000,0x12345678);
    test32(0xfffffffc,0x12345678);
    test32(0xfffffffd,0x12345678);
    test32(0xfffffffe,0x12345678);
    test32(0xffffffff,0x12345678);

    // Random tests.
    for (unsigned i = 0; i != NumTests; ++i) {
      test32(dist(gen),dist(gen));
    }

    test16(0x6008ffff,0x1d08);
    test16(0x1000,50);
    test16(0x1002,0xa5a5);
    test16(0x6638fbe9,0x8e6a);

    mem_write16(0x1004,0x0743);
    mem_write16(0x1006,0xf7fa);
    uint32_t addr = 0x1004;
    uint32_t result = mem_read32(addr);
    if (result != 0x0743f7fa) {
      RError("Endian issue:  Wrote (0x1004,0x0743) and (0x1006,0xf7fa), got 0x" << hex << result << "\n");
    }

    // Make sure that we don't overwrite when writing a split 16-bit value.
    test32(0x2000,0x12345678);
    test32(0x2004,0xdeadbeef);
    test16(0x2003,0xa54c);
    check32(0x2000,0x123456a5);
    check32(0x2004,0x4cadbeef);

    // Random tests.
    for (unsigned i = 0; i != NumTests; ++i) {
      test16(dist(gen),dist(gen));
    }

    for (unsigned i = 0; i != NumTests; ++i) {
      test8(dist(gen),dist(gen));
    }

    rand_functor rf(seed);

    arbtest<1> (rf,0x1000);
    arbtest<2> (rf,0x1100);
    arbtest<3> (rf,0x1200);
    arbtest<4> (rf,0x1300);
    arbtest<8> (rf,0x1400);
    arbtest<16>(rf,0x15ed);

    return 0;
  }
  catch (exception &err) {
    cerr << "Error:  " << err.what() << "\n";
    return 1;
  }
}

// Stubs required by ModuleSupport
void simulate()
{
}

// Stub required by libplasma.
int pMain(int argc,const char**)
{
  return 0;
}

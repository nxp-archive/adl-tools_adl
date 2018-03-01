//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// A simple driver used to test a library-mode ISS.
//
// In order to use this with a generated ISS, several defines must be set:
// 
//  MODEL:  Set to the quoted name of the include file, e.g. "mod1-lib.cc.h"
//
//  MNAME:  The model's name- should be a quoted string, e.g. "P".
//
//  MTYPE:  The name of the model's type to instantiate, e.g. P.
//

#include <iostream>

#include MODEL

#include "helpers/Macros.h"

using namespace std;
using namespace adl;

#define DummyReg(REG) \
  bits < 32 > REG ## _external_read (  ) const { return 0; }; \
  void REG ## _external_write ( const bits < 32 > &x ) { };

struct CoreImpl {

  enum { NumGprs = 32 };

  bits < 32 > CIA_external_read (  ) const { return _cia; }
  void CIA_external_write ( const bits < 32 > &x ) { _cia = x.uint32(); }

  bits < 32 > NIA_external_read (  ) const { return _nia; }
  void NIA_external_write ( const bits < 32 > &x ) { _nia = x.uint32(); }

  bits < 32 > MSR_external_read (  ) const { return _msr; }
  void MSR_external_write ( const bits < 32 > &x ) { _msr = x.uint32(); }

  bits < 32 > GPR_external_read ( unsigned i ) const  { return _gprs[i]; }
  void GPR_external_write ( unsigned i , const bits < 32 > &x ) { _gprs[i] = x.uint64(); }

  // Unused registers for this example, but these functions are required by gcc 4.4.7.
  DummyReg(CR);
  DummyReg(CTR);
  DummyReg(HID0);
  DummyReg(HID1);
  DummyReg(LR);
  DummyReg(XER);

  uint64_t _gprs[NumGprs];
  uint32_t _cr;
  uint32_t _cia;
  uint32_t _nia;
  uint32_t _msr;

  CoreImpl() : _cr(0), _cia(0), _nia(0), _msr(0)
  {
    for (int i = 0; i != 32; ++i) {
      _gprs[i] = 0;
    }
  }

  void display(ostream &o)
  {
    o << "GPRs:\n" << dec;
    ForRange(NumGprs,i) {
      o << '\t' << i << ":\t" << _gprs[i] << '\n';
    }
    o << hex
      << "CIA:\t0x" << _cia << '\n'
      << "NIA:\t0x" << _nia << '\n'
      << "MSR:\t0x" << _msr << '\n'
      << '\n';
  }
};

unsigned id = 0;
adliss::MTYPE<CoreImpl> core(MNAME,0,id);

int main( int argc, char* argv[] )
{
  core._gprs[2] = 10;
  core._gprs[1] = 20;
  core._gprs[5] = 0xdeadbeef;

  core.instr_handler_add(3,2,1);
  core.instr_handler_mullw(4,3,2);
  core.instr_handler_srw(5,7,2);

  cout << "Processor Results\n\n";

  core.display(cout);
};




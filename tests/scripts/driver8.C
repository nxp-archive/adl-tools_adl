//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Simple driver program to test that we can use an ISS with caches in a
// generate/simulate fashion, along the lines of what a testcase generator does.
// This requires us to be able to write to the top of the memory hierarchy so
// that the next simulate cycle will see the new instruction.
//

#include <assert.h>
#include <iostream>

#include "helpers/Macros.h"

#include "iss/SimInterface.h"
#include "iss/ModelInterface.h"

using namespace std;
using namespace adl;

struct Instr {
  uint32_t _op;
  addr_t   _data_addr;
  uint32_t _data;
  bool     _valid;

  Instr() : _op(0), _data_addr(0), _data(0), _valid(false) {};
  Instr(uint32_t o,addr_t da = 0,uint32_t d = 0) : _op(o), _data_addr(da), _data(d), _valid(true) {};
};

// The test program.
const Instr Program[] = {
    Instr(0x38211000),	                    //	addi r1,r1,0x1000
    Instr(0x80610000,0x1000,0xdeadbeef),	//	lwz r3,0x0(r1)
    Instr(0x80810004,0x1004,0x12345678),	//	lwz r4,0x4(r1)
    Instr(0x38630100),	                    //	addi r3,r3,0x100
    Instr(0x90601008),	                    //	stw r3,0x1008(r0)
    Instr(0x38410100),	                    //	addi r2,r1,0x100
    Instr(0x90820000),	                    //	stw r4,0(r2)
    Instr(0x7C0203AC),	                    //	dcbi r2,r0
    Instr(0x80A20000),	                    //	lwz r5,0(r2)
    Instr(0x7C0100AC),	                    //	dcbf r1,r0
    Instr(0x00000000),	                    //	.long 0x0
    Instr(),
};

int main(int argc,const char *argv[])
{
  if (argc < 2) {
    cout << "usage:  " << argv[0] << ":  <output file>" << endl;
    return 1;
  }

  unsigned id = 0;
  IssNode *root = adliss::createTopLevelNode(id);
  assert(root);

  installLoggers(root);
  addWriter(createWriter(*root,argv[1]));

  uint32_t r;

  // Some register setup.
  addr_t start = 0x100;
  root->setReg("NIA",0,start);
  root->setReg("CCR",0,0xc0000000);

  startLogging(*root);

  // This is the generate/simulate loop.
  for (int i = 0; Program[i]._valid; ++i) {
    //    auto pl = clear_tracing();
    root->debug_instr_write(false,start,Program[i]._op,32);
    root->debug_instr_read(r,false,start,32);
    assert(r == Program[i]._op);
    start += 4;
    if (Program[i]._data_addr) {
      root->debug_data_write(0,false,Program[i]._data_addr,Program[i]._data,32);
      root->debug_data_read(r,0,false,Program[i]._data_addr,32);
      assert(r == Program[i]._data);
    }
    //    set_tracing(pl);
    root->step();
  }

  stopLogging(*root);
  removeAllWriters();

}

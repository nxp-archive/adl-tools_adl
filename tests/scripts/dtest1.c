//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Simple test of a disassembler shared object.
//

#include <stdio.h>
#include <stdlib.h>

#include "asm/disassembler.h"


int main(int argc,char *argv[])
{
  const char input[] = {0x38, 0xE0, 0x00, 0x64, 0x7C, 0xE9, 0x03, 0xA6, 0x3D, 0x20, 0x00, 0x00 };
  int input_length = 12;
  char output[1024];

  disassemble(output,0x0,input,input_length,1,0,"\n");

  printf ("Output:\n\n%s\n",output);

  return 0;
}

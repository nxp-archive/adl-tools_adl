//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// This is a simple interface for shared-object disassemblers.
//

#ifndef _DISASSEMBLER_H_
#define _DISASSEMBLER_H_

#ifdef __cplusplus
#define  EXTERN_C extern "C"
#else 
#define  EXTERN_C  
#endif

//
// Interface for disassembling a stream to a buffer.
//
// output:     Output buffer (disassembled instruction text).
// memaddr:    Memory address of disassembly point.  Only relevant if the generated
//             disassembler contains instruction fields with semantic knowledge about
//             addresses or if print_addr is true.
// input:      The input buffer (binary data).
// length:     Length (in bytes) of input buffer.  The routine will keep
//             disassembling until 'length' is reached.
// bigendian:  If true, the input character stream is in big-endian format..
//             False: In little-endian format.
// print_addr: True:  Print a leading memory address before each disassembled instruction.  This starts with
//             memaddr and advances it as necessary.
// sep:        Separator string for multiple instructions.  This string is printed after each instruction
//             is disassembled.  May be 0, in which case it is ignored.
// return:     Number of bytes disassembled.
EXTERN_C int disassemble(char *output,unsigned memaddr,const char *input,int length,int bigendian,
			   int print_addr,const char *sep);


#endif

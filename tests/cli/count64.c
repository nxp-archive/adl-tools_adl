
#include "stdio.h"

int main()
{
  int *incr, *buf;
  buf = 0xa0000;
  incr = 0xa0010;
  *incr = 1;
  int i;
  for (i = 0; i != 20; ++i) {
    *buf = *buf + *incr;
  }
}

// We don't need to do anything, since there's no OS.
int brk()
{
}

// Only valid when the halt instruction is defined.
int exit()
{
  asm (".long 0x0");
}


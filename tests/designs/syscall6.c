//
// We should ignore attempts to open or close standard in, out, error.
//
#include <stdio.h>


int main (int argc, char* argv[])
{
  printf("Hello, world!\n");
  close (0);
  close (1);
  close (2);
  printf("Hello again!\n");
  return 0;
}

                                                                                                                                                          


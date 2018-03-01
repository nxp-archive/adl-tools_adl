//
// Test large reads.
//

#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

int main (int argc, char* argv[])
{
  char *fn       = "hamlet.txt";
  unsigned size  = 371957;
  unsigned start = 371890;

  printf("Filename:  %s\n\n",fn);
  int fd = open (fn,O_RDONLY);

  if (fd < 0) {
    printf ("Could not open file %s\n",fn);
    return 1;
  }

  char buf[size];
  size_t cnt = read(fd,buf,size);
  printf ("Read %d bytes.\n",cnt);
  close (fd);

  while (buf[start] && (start < size)) {
    putchar(buf[start++]);
  }
  putchar('\n');

  return 0;
}

                                                                                                                                                          


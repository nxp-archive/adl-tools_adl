
#include "stdio.h"

int  *os_write_len = (int*) 0xa0000; // Buffer length for write.
char *os_write_buf = (char*)0xa0004; // Start of buffer for write.

int  *os_read_len = (int*) 0xb0000; // Buffer length for read.
char *os_read_buf = (char*)0xb0004; // Start of buffer for read.

int main()
{
  int len;
  char buf[200];

  printf ("hello, world!\n");

  while ( (len = read(0,buf,200)) ) {
    printf ("Read %d bytes:  %s\n",len,buf);
  }

  printf ("All done!\n");
  return 0;
}

// Copies the string to the output buffer.  The write to the length location
// should trigger any output.
int write(int file, char *ptr, int len) 
{
  int i;
    
  char *dest = os_write_buf;
  for (i = 0; i < len; ++i) {
    *dest++ = *ptr++;
  }
  *os_write_len = len;
  return len;
}

// Copies the string from the input buffer to the specified destination.
// Returns bytes read, which is the min of the destination buffer or the string
// length.
int read(int file,char *ptr,int len)
{
  char *src = os_read_buf;
  int src_len = *os_read_len;
  int i;

  len = (src_len < len) ? src_len : len;
  for (i = 0; i < len;  ++i) {
    *ptr++ = *src++;
  }
  return len;
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

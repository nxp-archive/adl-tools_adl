/*

 A very simple producer/consumer example used to demonstrate independent debug.
 Thread 0 produces characters which are consumed by thread 1.  This is extremely
 simple, with no real spin locks, since we're running on a very simple model.

*/

#include <stdio.h>

// Data produced by the producer
char producer_data = 0;

// The producer only produces a new value when this has been incremented, so that it knows 
// that the data has been consumed.
unsigned consumed_count = 0;

int get_tid()
{
  int tid;

  asm("mfspr %0,286\n"
      : "=r" (tid));
  return tid;
}

void heartbeat(char c)
{
  char buf[1] = {c};
  write (1,buf,1);
}

void msg1(char *m,char t)
{
  char buf[strlen(m)+1];
  strcpy(buf,m);
  char *p = strchr(buf,'%');
  *p = t;
  write(1,buf,strlen(buf));
}

void consumer()
{
  unsigned char d = 0;
  do {
    while (producer_data == d) {
      //heartbeat('.');
    }
    d = producer_data;
    msg1("Data:  %\n",d);
    consumed_count = consumed_count + 1;
  } while (d != 'z');
}

void producer()
{
  char d = 'a';
  do {
    unsigned cnt = consumed_count;
    producer_data = d++;
    while (consumed_count == cnt);
  } while (d <= 'z');
}

// We get different stacks due to the use of pids.dat, which modifies r1 (stack pointer).
int main()
{
  int tid = get_tid();
  if (tid == 0) {
    producer();
  } else {
    consumer();
  }
}

void exit(int rc)
{
  asm(".long 0x0");
}

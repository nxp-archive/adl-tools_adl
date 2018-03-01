
#include <stdio.h>

typedef void (*thread_func)();

int get_tid()
{
  int tid;

  asm("mfspr %0,286\n"
      : "=r" (tid));
  return tid;
}

char get_tid_n(int tid)
{
  return (tid) ? '1' : '0';
}

void msg1(char *m,char t)
{
  char buf[strlen(m)+1];
  strcpy(buf,m);
  char *p = strchr(buf,'%');
  *p = t;
  write(1,buf,strlen(buf));
}

void foo()
{
  msg1 ("foo!\n",0);
}

void bar()
{
  msg1 ("bar!\n",0);
}

void stuff()
{
  int tid = get_tid();

  msg1("Core %\n",get_tid_n(tid));

  if (tid == 0) {
    foo();
  } else {
    bar();
  }

  msg1("About done.\n",0);
  msg1("Last msg.\n",0);
}

// We get different stacks due to the use of pids.dat, which modifies r1 (stack pointer).
int main()
{
  stuff();
}

void exit(int rc)
{
  asm(".long 0x0");
}

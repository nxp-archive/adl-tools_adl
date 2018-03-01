//
// Used to test basic-block tracing using the logging interface.
//

void b1()
{
}

void b2()
{
}

void c1()
{
}

void c2()
{
}

void a1()
{
  b1();
  b2();
}

void a2()
{
  int i;

  for (i = 0; i != 5; ++i) {
    c1();  
  }
  for (i = 0; i != 3; ++i) {  
    c2();
  }
}

int main()
{
  a1();
  a2();
}

int exit()
{
  asm (".long 0x0");
}

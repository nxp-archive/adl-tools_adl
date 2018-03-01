
int *count1 = (int*)0x1004;
int *count2 = (int*)0x1008;

int *bad1 = (int*)0x100c;
int *bad2 = (int*)0x1010;

int p1()
{
  int i;

  *bad1 = 0;
  *count1 = 0;
  for (i = 0; i != 20; ++i) {
    *count1 = *count1 + 2;
  }
  e1();
}

int e1(void)
{
  while (1) { ++(*bad1); };
}

int p2()
{
  int i;

  *bad2 = 0;
  *count2 = 1;
  for (i = 0; i != 10; ++i) {
    *count2 = *count2 * 3;
  }
  e2();
}

int e2(void)
{
  while (1) {  ++(*bad2); };
}



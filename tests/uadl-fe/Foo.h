
#include <vector>

template <class ModelType,class InstrType,int N1,bool N2 = true>
class Foo {

  Foo(uadl::Timer &model,int parm1,int parm2 = 10) {};

  void preCycle();

protected:
  int _x;
  int _y;
};

class B_t {
  B_t(uadl::Timer &model,int x,int y,int z);

  void preCycle();
  void postCycle() { int i = 10; };
  void postSim();

  unsigned x,y;
};


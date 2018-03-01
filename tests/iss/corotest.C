//
// Very simple test of coroutines to verify their basic operation.
//

#include <iostream>

#include "helpers/BasicTypes.h"

#include "iss/Coroutine.h"

using namespace std;

struct Foo;

typedef Coroutine<Foo,const char *> MyCoro;

struct Foo {

  Foo(unsigned d) : _data(d), _done(false) {};

  void foo(MyCoro *c,const char *str)
  {
    for (int i = 0; i != 20; ++i) {
      cout << str << "Data:  " << _data << ", Iteration " << i << endl;
      c->resume();
    }
    _done = true;
  }

  bool done() const { return _done; };

  void reset() { _done = false; };

  unsigned _data;
  bool     _done;
};

void driver (MyCoro *c,Foo *f,const char *str)
{
  f->foo(c,str);
}

int main()
{
  const int stack_size = 8192;
  char *stack = new char[stack_size];

  const char *str = "Hello there:  ";

  Foo f(69);

  MyCoro mycoro;
  MyCoro::context_t main_ctx{};
  mycoro.reset(&driver,&f,str,main_ctx,stack,stack_size);

  cout << "Starting coroutine." << endl;

  while (!f.done()) {
    mycoro.call();
    cout << "----\n";
  }

  cout << "Finished first test." << endl;

  f.reset();
  mycoro.reset(&driver,&f,str,main_ctx,stack,stack_size);

  while (!f.done()) {
    mycoro.call();
    cout << "----\n";
  }

  cout << "Finished with coroutine." << endl;

}

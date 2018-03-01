//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//
//
// Simple coroutine implementation using Boost Context.
//

#ifndef _COROUTINE_H_
#define _COROUTINE_H_

#include <boost/context/all.hpp>
#include <boost/version.hpp>

template <class Class,typename Data>
class Coroutine {
  typedef void (*Func)(Coroutine<Class,Data> *,Class *,Data);
public:
  Coroutine() : _this_context(0), _yield_context(0), _func(0), _c(0) {}
  
  typedef boost::context::fcontext_t context_t;

# if BOOST_VERSION >= 105600
# define CORO_DEREF &
  typedef context_t context_ptr;
  typedef context_t context_ref;

# else
# define CORO_DEREF 
  typedef context_t* context_ptr;
  typedef context_t& context_ref;
  
# endif  
  
  // Arguments:
  // func:           Function pointer to driver function.  This is what the coroutine actually calls.
  // c   :           Class pointer.  A coroutine stores a pointer to this main class, which the driver 
  //                 then usually calls a method of.
  // data:           Additional data object.
  // yield_context:  Storage of the calling (main) context.  External in order to save space.
  // stack:          The coroutine's stack.
  // stack_size:     The size of the stack, in bytes.
  void reset(Func func,Class *c,const Data &data,context_ref yield_context,char *stack,unsigned stack_size)
  {
    _this_context = boost::context::make_fcontext(stack + stack_size,stack_size,Coroutine::dispatch);
#   if BOOST_VERSION >= 105600
    _yield_context = yield_context;
#   else
    _yield_context = &yield_context;
#   endif    
    _func = func;
    _c = c;
    _data = data;
  }
  

  void call() {
    boost::context::jump_fcontext(CORO_DEREF _yield_context, _this_context, reinterpret_cast<intptr_t>(this));
  }

  void resume() {
    boost::context::jump_fcontext(CORO_DEREF _this_context, _yield_context, 0);
  }  
  
private:
  static void dispatch(intptr_t coroutine_ptr) 
  {
    Coroutine *coroutine = reinterpret_cast<Coroutine *>(coroutine_ptr);
    coroutine->_func(coroutine,coroutine->_c,coroutine->_data);
    // Loop once done so that calling a completed coroutine doesn't result in
    // undefined behavior- it simply returns immediately.
    while (true) coroutine->resume();
  }

  context_ptr                      _this_context;
  context_ptr                      _yield_context;
  Func                             _func;
  Class                           *_c;
  Data                             _data;
};

#endif

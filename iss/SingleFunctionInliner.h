
#ifndef _SINGLEFUNCTIONINLINER_H
#define _SINGLEFUNCTIONINLINER_H


namespace llvm {
  class FunctionPass;
  class ModuleProvider;
}

llvm::FunctionPass *createSingleFunctionInliningPass();

#endif


//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#include "llvm/Attributes.h"
#include "llvm/BasicBlock.h"
#include "llvm/Function.h"
#include "llvm/Instructions.h"
#include "llvm/Module.h"
#include "llvm/Pass.h"
#include "llvm/Transforms/Utils/Cloning.h"

namespace {

  namespace Attribute = llvm::Attribute;
  using llvm::BasicBlock;
  using llvm::CallInst;
  using llvm::Function;
  using llvm::FunctionPass;
  using llvm::InlineFunctionInfo;
  using llvm::InvokeInst;
  using llvm::Module;
  using llvm::dyn_cast;
  using llvm::isa;

  // This came form Unladen Swallow, the Python compiler.  It's a per-function
  // inliner designed to just be run on the basic-block functions generated from
  // the simulator.  Thus, it inlines everything, since all it calls are just
  // instruction handlers.
  class SingleFunctionInliner : public FunctionPass {
  public:
    static char ID;
    SingleFunctionInliner() : FunctionPass(ID) 
    {}

    virtual bool runOnFunction(Function &f)
    {
      bool changed = false;
      // Scan through and identify all call sites ahead of time so
      // that we only inline call sites in the original functions,
      // not call sites that result from inlining other functions.
      std::vector<CallInst*> call_sites;
      for (Function::iterator bb = f.begin(), e = f.end(); bb != e; ++bb) {
        for (BasicBlock::iterator inst = bb->begin(); inst != bb->end(); ++inst) {
          CallInst *call = dyn_cast<CallInst>(inst);
          if (!call) {
            continue;
          }
          // This may miss inlining indirect calls that become
          // direct after inlining something else.
          Function *called_function = call->getCalledFunction();
          if (!called_function) {
            continue;
          }
          if (called_function->isMaterializable()) {
            if (called_function->Materialize()) {
              continue;
            }
          }
          if (!called_function->isDeclaration()) {
            call_sites.push_back(call);
          }
        }
      }

      // Actually inline the calls we found.
      for (size_t i = 0; i != call_sites.size(); ++i) {
        InlineFunctionInfo junked;
        changed |= InlineFunction(call_sites[i], junked);
      }
      return changed;
    }
  };

  // The address of this variable identifies the pass.  See
  // http://llvm.org/docs/WritingAnLLVMPass.html#basiccode.
  char SingleFunctionInliner::ID = 0;

}  // anonymous namespace

FunctionPass *createSingleFunctionInliningPass()
{
    return new SingleFunctionInliner();
}

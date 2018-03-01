//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Support classes and stuff for JIT models.
//

#include <stdexcept>
#include <iostream>
#include <cstdio>

#include "helpers/BasicTypes.h"
#include "helpers/Macros.h"
#include "helpers/FilenameUtils.h"

#include "llvm/Bitcode/ReaderWriter.h"
#include "llvm/Support/system_error.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/TypeBuilder.h"
#include "llvm/Support/Debug.h"
#include "llvm/Support/CommandLine.h"
#ifdef LLVM_29
# include "llvm/Target/TargetSelect.h"
#else
# include "llvm/Support/TargetSelect.h"
#endif
#include "llvm/Target/TargetOptions.h"
#include "llvm/Target/TargetData.h"
#include "llvm/Analysis/Verifier.h"
#include "llvm/Analysis/Passes.h"
#include "llvm/Transforms/IPO.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/ExecutionEngine/ExecutionEngine.h"
#include "llvm/ExecutionEngine/JITMemoryManager.h"
#include "llvm/ExecutionEngine/JIT.h"
#include "llvm/Module.h"
#include "llvm/Instructions.h"
#include "llvm/Type.h"
#include "llvm/PassManager.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/IRBuilder.h"

#include "SingleFunctionInliner.h"

#include "JitSupport.h"

#include <deque>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

#include <assert.h>

//#define THREAD_DEBUG1
//#define THREAD_DEBUG2

#ifdef THREAD_DEBUG1
# define TRDBG1(x) { scoped_lock my_io_lock(_io_mutex); std::cout << x; }
#else
# define TRDBG1(x) 
#endif

#ifdef THREAD_DEBUG2
# define TRDBG2(x) { scoped_lock my_io_lock(_io_mutex); std::cout << x; }
#else
# define TRDBG2(x) 
#endif


using namespace std;
using namespace llvm;

extern "C" {

  int branch_stub() { return adl::JitNopCode; };

}

namespace adl {

  struct JitBuilder : public IRBuilder<> {
    JitBuilder(LLVMContext &context) : IRBuilder<>(context) {};
  };

  void JitBasicBlock::dump(ostream &o) const
  {
    o << hex << "[0x" << _start_ra << ":0x" << _end_ra << "]: ";
    if (_fp == branch_stub) {
      o << "<branch-stub>";
    } else if (!is_compiled()) {
      o << "<empty>";
    } else {
      o << _func->getName().str();
    }
    o << dec;
  }

  Function *JitSupport::start_bb(JitBasicBlock *jb)
  {
    assert(!jb->is_compiled());

    // No code allocated, so create a bb function.
    Function *f = Function::Create(_bb_type, Function::ExternalLinkage, "", _module);
    BasicBlock *bb = BasicBlock::Create(*_context,"entry", f);
    _builder->SetInsertPoint(bb);       
    create_br_target(jb);

    _has_ret = false;
    _has_tdc = false;

    return f;
  }

  void JitSupport::dump()
  {
    _module->dump();
  }

  void JitSupport::reset()
  {
    _bb_count = 0;
    _jit_compiles = 0;
  }


  void JitSupport::reset_bbs()
  {
    _bb_count = 0;
  }

  void JitSupport::finish_internal(JitBasicBlock *jb,Function *f,int icount,bool target_br_stub,int retcode)
  {
    add_td_check(jb,icount);
    if (!_has_ret) {
      _builder->CreateRet(ConstantInt::get(*_context,APInt(32,retcode)));
      _has_ret = true;
    }
    verifyFunction(*f);
    _fpm->run(*f);
    jb->_fp = (Dest)_engine->getPointerToFunction(f);
    if (target_br_stub) {
      set_br_target_to_stub(jb);
    } else {
      set_br_target(jb);
    }
    //    _module->dump(); 
    // Last thing we do: Update the internal pointer!  This
    // way, another thread sees a single atomic update when the function is
    // ready.
    jb->_func = f;
  }

  void JitSupport::finish_bb_jump(JitBasicBlock *jb,Function *f,JitBasicBlock *target,int icount)
  {
    add_td_check(jb,icount);

    // Add on a jump to the successor.
    add_trg_call(target);
    _has_ret = true;

    // Then finish the block.
    finish_bb(jb,f,-1,false);  
  }

  void JitSupport::finish_bb_brkpt(JitBasicBlock *jb,Function *f)
  {
    finish_internal(jb,f,-1,false,JitBreakPtCode);
  }

  // A standard finish of a basic block.  Basically, keep executing, use the NIA
  // to look up the next block, etc.  If target_br_stub is true, we set the
  // branch target to the branch stub, to force a return from anything chained
  // to us.  This is generally just used due to breakpoints.
  void JitSupport::finish_bb(JitBasicBlock *jb,Function *f,int icount,bool target_br_stub)
  {
    finish_internal(jb,f,icount,target_br_stub,JitNopCode);
  }

  bool JitSupport::targets_branch_stub(JitBasicBlock *jb)
  {
    return (jb->_fp == branch_stub);
  }

  void JitSupport::delete_bb_func(JitBasicBlock *jb)
  {
    if (!targets_branch_stub(jb) && jb->is_compiled()) {
      _engine->freeMachineCodeForFunction(jb->_func);
      jb->_fp = branch_stub;
    }
    if (jb->is_compiled()) {
      jb->_func->eraseFromParent();
      jb->_func = (llvm::Function*)JitBasicBlock::NotCompiled;
    }
  }
  
  void JitSupport::invalidate_bb(JitBasicBlock *jb)
  {
    delete_bb_func(jb);
    create_br_stub(jb);
  }

  void JitSupport::reset_bb(JitBasicBlock *jb)
  {
    delete_bb_func(jb);
    if (jb->_bt) {
      jb->_bt->eraseFromParent();
      jb->_bt = 0;
    }
  }

  void JitSupport::set_br_target(JitBasicBlock *jb)
  {
    Dest *d = (Dest*)_engine->getPointerToGlobal(jb->_bt);
    *d = jb->_fp;
  }

  void JitSupport::set_br_target_to_stub(JitBasicBlock *jb)
  {
    Dest *d = (Dest*)_engine->getPointerToGlobal(jb->_bt);
    *d = branch_stub;
  }

  void JitSupport::create_br_target(JitBasicBlock *jb)
  {
    if (!jb->_bt) {
      jb->_bt = new GlobalVariable(*_module,_bb_ptr,false,GlobalValue::ExternalLinkage,_const_null,"");
    }
  }

  void JitSupport::create_br_stub(JitBasicBlock *jb)
  {
    jb->_fp = branch_stub;
    create_br_target(jb);
    set_br_target(jb);
  }

  // Note: Opcodes are passed as 64-bit integers simply because they're stored
  // in the decode-cache argument array, which must be of uniform size.  They
  // can actually be used by the JIT as 32-bit integeres.
  void JitSupport::add_log_call(int index,uint64_t ea,uint64_t *opc,unsigned n_opc,unsigned max_opc)
  {
    assert(index > 0);
    _params.clear();

    _params.push_back(ConstantInt::get(*_context,APInt(64,ea)));
    unsigned i = 0;
    for ( ; i != n_opc; ++i) {
      _params.push_back(ConstantInt::get(*_context,APInt(32,opc[i])));
    }
    for ( ; i != max_opc; ++i) {
      _params.push_back(ConstantInt::get(*_context,APInt(32,0)));
    }

    Function *instr = get_handler(index);
    _builder->CreateCall(instr,_params);
  }

  // Simply store the post-logger call info for now.  Will be called by add_call
  // if set.
  void JitSupport::add_post_log_call(int index,uint64_t ea)
  {
    _plog_id = index;
    _plog_ea = ea;
  }

  void JitSupport::insert_post_log_call(int index,uint64_t ea)
  {
    if (index > 0) {
      _params.clear();
      _params.push_back(ConstantInt::get(*_context,APInt(64,ea)));

      Function *instr = get_handler(index);
      _builder->CreateCall(instr,_params);
    }
  }

  CallInst *JitSupport::add_call(int index,int argc,uint64_t *args,void *extra_p,int index_p)
  {
    assert(index > 0);

    _params.clear();

    for (int i = 0; i != argc; ++i) {
      _params.push_back(ConstantInt::get(*_context,APInt(64,args[i])));
    }
    if (extra_p) {
      ConstantInt* e  = ConstantInt::get(*_context, APInt(64, (uint64_t)extra_p));
      Constant* ptr_e = ConstantExpr::getCast(Instruction::IntToPtr, e, _vd_ptr);
      _params.push_back(ptr_e);
    }
    if (index_p >= 0) {
      _params.push_back(ConstantInt::get(*_context,APInt(32,index_p)));
    }

    Function *instr = get_handler(index);
    CallInst *c = _builder->CreateCall(instr,_params);

    // Add on the post-logger call, if there is one.
    if (_plog_id > 0) {
      insert_post_log_call(_plog_id,_plog_ea);
      _plog_id = 0;
      _plog_ea = 0;
    }
    
    return c;
  }

  CallInst *JitSupport::add_call(int index,addr_t a,uint32_t v)
  {
    assert(index > 0);

    _params.clear();

    _params.push_back(ConstantInt::get(*_context,APInt(64,a)));
    _params.push_back(ConstantInt::get(*_context,APInt(32,v)));

    Function *instr = get_handler(index);
    return _builder->CreateCall(instr,_params);
  }

  CallInst *JitSupport::add_call(int index,addr_t a)
  {
    assert(index > 0);

    _params.clear();

    _params.push_back(ConstantInt::get(*_context,APInt(64,a)));

    Function *instr = get_handler(index);
    return _builder->CreateCall(instr,_params);
  }

  void JitSupport::add_uncond_branch(JitBasicBlock *jb,int index,JitBasicBlock *taken,int icount,
                                     int argc,uint64_t *args,void *extra)
  {
    add_call(index,argc,args,extra);
    // We add in the TD check right before the branch.  Note that the count is
    // already +1 due to the decode-cache translation work.
    add_td_check(jb,icount);
    add_trg_call(taken);
    _has_ret = true;
  }

  // In this case, even though the branch, from a simulation point of view,
  // simply falls through if not taken, from a direct-threading point of view,
  // we jump to the successor block if the branch is not taken.
  void JitSupport::add_cond_branch(JitBasicBlock *jb,int index,JitBasicBlock *taken,JitBasicBlock *untaken,
                                   int icount,int argc,uint64_t *args,void *extra)
  {
    Function *func = _builder->GetInsertBlock()->getParent();

    Value *cond = add_call(index,argc,args,extra);

    // We add in the TD check right before the branch.  Note that the count is
    // already +1 due to the decode-cache translation work.
    add_td_check(jb,icount);
 
    add_branch_handler(func,cond,taken,untaken);
  }

  void JitSupport::add_trg_call(JitBasicBlock *jb)
  {
    if ((jb > (JitBasicBlock*)JitNullCode) && (jb < (JitBasicBlock*)JitLastCode)) {
      // If it has a known, special value, then deal with it.  In this case, we
      // just add in a return call probably because we got an exception when
      // trying to look up an address.
      _builder->CreateRet(ConstantInt::get(*_context,APInt(32,(JitReturnCodes)(long)jb)));
    } else {
      // Otherwise, add in a target call.  The block might not have a branch
      // target if it's new, so create one if necessary.
      if (!jb->_bt) {
        create_br_stub(jb);
      }
      GlobalVariable *gv = jb->_bt;
      LoadInst *li = _builder->CreateLoad(gv,"");
      CallInst *call = _builder->CreateCall(li);
      call->setTailCall(true);
      _builder->CreateRet(call);
    }
  }

  // Add a time-quanta check to the basic block.  This just calls the
  // time-quanta check function, and if true, returns with a JitRetCode code so
  // that the simulation kernel knows to check to make sure that the time quanta
  // has expired.  If false, the code just continues on.  The check function
  // increments the counter, based upon the count supplied, then compares the
  // counter against the theshold.
  void JitSupport::add_td_check(JitBasicBlock *jb,int icount)
  {
    // Do nothing if the temporal-decoupling API is not enabled.
    if (!td_enabled() || _has_tdc || (icount < 0)) {
      return;
    }
    Function *func = _builder->GetInsertBlock()->getParent();

    Value *cond = _builder->CreateCall(get_handler(_td_index),ConstantInt::get(*_context,APInt(32,icount)));
    cond = _builder->CreateICmpNE(cond,ConstantInt::get(*_context,APInt(1,  0)));

    BasicBlock *tBB = BasicBlock::Create(*_context, "then");
    BasicBlock *mBB = BasicBlock::Create(*_context, "ifcont");

    _builder->CreateCondBr(cond,tBB,mBB);

    func->getBasicBlockList().push_back(tBB);
    _builder->SetInsertPoint(tBB);
    _builder->CreateRet(ConstantInt::get(*_context,APInt(32,JitRetCode)));
    tBB = _builder->GetInsertBlock();    

    func->getBasicBlockList().push_back(mBB);
    _builder->SetInsertPoint(mBB);
    _has_tdc = true;
  }

  // Adds on the code to conditionally jump to one or another basic blocks based
  // upon whether the branch is taken or not.
  void JitSupport::add_branch_handler(Function *func,Value *cond,JitBasicBlock *taken,JitBasicBlock *untaken)
  {
    cond = _builder->CreateICmpNE(cond,ConstantInt::get(*_context,APInt(1,  0)));

    BasicBlock *tBB = BasicBlock::Create(*_context, "then");
    BasicBlock *eBB = BasicBlock::Create(*_context, "else");
    BasicBlock *mBB = BasicBlock::Create(*_context, "ifcont");

    _builder->CreateCondBr(cond,tBB,eBB);

    func->getBasicBlockList().push_back(tBB);
    _builder->SetInsertPoint(tBB);
    add_trg_call(taken);
    tBB = _builder->GetInsertBlock();    

    func->getBasicBlockList().push_back(eBB);
    _builder->SetInsertPoint(eBB);
    add_trg_call(untaken);
    eBB = _builder->GetInsertBlock();

    func->getBasicBlockList().push_back(mBB);
    _builder->SetInsertPoint(mBB);
  }

  JitSupport::ParmId JitSupport::get_parm_handle(const char *n,void *parm,unsigned init_value)
  {
    GlobalVariable *gv = _module->getNamedGlobal(n);
    if (!gv)
      ErrExit("Unknown global:  " << n);
    gv->setConstant(true);    
    _engine->addGlobalMapping(gv,parm);
    set_parm_value(gv,init_value);
    return gv;
  }

  void JitSupport::set_parm_value(ParmId id,unsigned val)
  {
    id->setInitializer(ConstantInt::get(*_context,APInt(32,val)));
  }

  bool llvmInit(bool setup_mt)
  {
    //cout << "LLVM init routine called." << endl;
    InitializeNativeTarget();
    if (setup_mt) {
      if (!llvm_start_multithreaded()) {
        ErrExit("Could not start LLVM in multi-threaded mode.");
      }
    }
    return true;
  }

  void JitSupport::createModule(const string &bcname)
  {
    std::string error;

    // A new context for this core.
    _context = new LLVMContext();

    // Load in the bitcode file containing the functions for each
    // bytecode operation.
    OwningPtr<MemoryBuffer> buffer;
    if (llvm::error_code ec = MemoryBuffer::getFile(bcname, buffer)) {
      ErrExit("Could not read bitcode file '" << bcname << "':  " << ec.message());
    } else {
      _module = getLazyBitcodeModule(buffer.take(), *_context, &error);
      //      _module = ParseBitcodeFile(buffer.take(), *_context, &error);
      _builder = new JitBuilder(*_context);
    } 
    if (!_module) {
      ErrExit("Could not create LLVM module from file '" << bcname << "':  " << error);
    }

    
    _bb_type    = FunctionType::get(Type::getInt32Ty(*_context),false);
    _bb_ptr     = PointerType::get(_bb_type,0);
    _vd_ptr     = PointerType::get(IntegerType::get(*_context, 8), 0);
    _const_null = ConstantPointerNull::get(_bb_ptr);
  }

  void JitSupport::createEngine()
  {
    // Setup for JIT
    // Notes on setup:
    //
    // * setEngineKind(EngineKind::JIT): I definitely want the JIT, so this
    // * ensures that's what I get.  If it's not available, I want to know it
    // * with an error produced.
    //
    // * The setAllocateGVsWithCode(false) means that GVs (used for storing
    //   branch targets) are not allocated with machine code, thus allowing us
    //   to free up the machine code.  The default is apparently a bit of an
    //   anachronism, so the recommendation is to set this to false.
    //
    // * setJitMemorymanager: Create our own memory manager, then pass ownership
    //   to the enginee.  I just do this so that I can grab a handle to the
    //   memory manager in order to be able to query it for memory usage.
    llvm::TargetOptions Opts;
    Opts.JITExceptionHandling = true;

    std::string errstr;
    _engine = EngineBuilder(_module).
      setEngineKind(EngineKind::JIT).
      setErrorStr(&errstr).
      setAllocateGVsWithCode(false).
      setTargetOptions(Opts).
      create();
    if (!_engine) {
      ErrExit("Could not create ExecutionEngine:  " << errstr);
    }
  }

  void JitSupport::createPassMgr()
  {
    // Add in optimizations. These were taken from a list that 'opt', LLVMs optimization tool, uses.
    _fpm = new FunctionPassManager(_module);

    // /* Comment out optimize
    _fpm->add(new TargetData(_module));
    
    _fpm->add(createSingleFunctionInliningPass());
    _fpm->add(createTypeBasedAliasAnalysisPass());
    _fpm->add(createBasicAliasAnalysisPass());
    _fpm->add(createPromoteMemoryToRegisterPass());
    _fpm->add(createConstantPropagationPass());
    _fpm->add(createGVNPass());
    _fpm->add(createCFGSimplificationPass());
    _fpm->add(createDeadStoreEliminationPass());
    _fpm->add(createInstructionCombiningPass());

    _fpm->doInitialization();
  }

  void JitSupport::installCorePtr(const char *core_name,void *core_ptr)
  {
    GlobalVariable *gv = _module->getNamedGlobal(core_name);
    assert(gv);
    _engine->addGlobalMapping(gv,core_ptr);
  }

  void JitSupport::installTracePtr(const char *trace_name,uint64_t *trace_ptr)
  {
    _trace_gv = _module->getNamedGlobal(trace_name);
    assert(_trace_gv);
    // We're making this variable constant, so that it'll be folded out by the
    // inlining pass.
    _trace_gv->setConstant(true);    
    _engine->addGlobalMapping(_trace_gv,trace_ptr);
    // Initially, tracing will be off.
    set_tracing(0);
  }

  void JitSupport::set_tracing(uint64_t trace_flags)
  {
    _trace_gv->setInitializer(ConstantInt::get(*_context,APInt(64,trace_flags)));
  }

  void JitSupport::loadHandlers(const char *handler_pfx,int entry_start,int entry_end,int instr_start)
  {
    char buf[strlen(handler_pfx) + 10];

    assert(entry_end > entry_start);
    _hoffset = entry_start;
    for (int i = entry_start; i != entry_end; ++i) {
      sprintf(buf,"%s%d",handler_pfx,i);
      Function *func = _module->getFunction(buf);
      // We might have null pointers if instructions were removed b/c they
      // weren't used, so we don't do error checking here.
      _handlers.push_back(func);
    }
  }

  JitSupport::JitSupport(const char *bcname,int entry_start,int entry_end,
                         int instr_start,int td_index,const char *handler_pfx,
                         const char *core_name,void *core_ptr,
                         const char *trace_name,uint64_t *trace_ptr,bool setup_mt) :
    _context(0),
    _module(0),
    _builder(0),
    _fpm(0),
    _engine(0),
    _td_index(td_index),
    _has_ret(false),
    _has_tdc(false),
    _bb_count(0),
    _jit_compiles(0),
    _plog_id(0),
    _plog_ea(0)
  {
    // Turn this on for massive amounts of debugging data.  Only works if we're
    // linking against the debug version of LLVM (in which case NDEBUG won't be
    // defined).
#   ifndef NDEBUG
    //DebugFlag = true;
#   endif
    // This allows command-line parms to be sent to LLVM, e.g. use
    // -jit-emit-debug to have the JIT create debug frames for better gdb
    // debugging.
    cl::ParseEnvironmentOptions("adl-jit","ADL_JIT_CMDLINE_OPTIONS");

    static bool startup ATTRIBUTE_UNUSED = llvmInit(setup_mt);

    // We look for the bitcode file in the same location as our own executable.
    string bcfn = dir_name(current_exe_name()) + "/" + bcname;
    // Basic setup of generation resources.
    createModule(bcfn);
    createEngine();
    createPassMgr();

    installCorePtr(core_name,core_ptr);

    installTracePtr(trace_name,trace_ptr);

    loadHandlers(handler_pfx,entry_start,entry_end,instr_start);
  }

  JitSupport::~JitSupport()
  {
    delete _builder;
    delete _fpm;
    delete _engine;
    delete _context;
  }

  // Basically just a simple queue.  This originally had locking semantics, but
  // I realized it was redundant, since the JitCompilerImpl had to have its own
  // lock for other things.  So, right now, there is no locking on this object.
  template <class C>
  class WorkQueue {
  public:
    typedef std::deque<C*>             Queue;
    

    void clear() 
    {
      _q.clear();
    }

    void push(C *c) 
    {
      _q.push_back(c);
    }

    // Returns 0 if empty.
    C *pop()
    {
      if (_q.empty()) return 0;
      C *tmp = _q.front();
      _q.pop_front();
      return tmp;
    }

  private:
    Queue        _q;
  };

  // The JIT support class with the compiler routine (translates decode-cache to
  // LLVM).  Templated so that we get proper types for the decode-cache
  // elements.
  class JitCompilerImpl {
  public:
    typedef boost::mutex::scoped_lock    scoped_lock;
    typedef std::vector<boost::thread *> Threads;

    // Status conveyed from the main thread to the worker thread.
    enum Status { sWork, sQuit };

    // thread_count: If 0, do not use MT.  Otherwise, specify number of compiler
    // threads to create.
    JitCompilerImpl (void *core,void *br_func,void *brkpt_func,JitSupport &jit,unsigned num_threads) :
      _status(sWork),
      _core(core),
      _br_func(br_func),
      _brkpt_func(brkpt_func),
      _jit(jit),
      _num_threads(num_threads)
    {
      create_threads();
    };

    bool is_mt() const
    {
      return !_workers.empty();
    }

    // Called by the main thread: Signal a shutdown.  Returns once the shutdown
    // is complete, which might take a little while, since we wait for the
    // thread to finish the compilation job it's working on.
    void signal_shutdown()
    {
      if (is_mt()) {
        TRDBG1("main:  signal quit...\n");
        {
          // Only lock when modifying the queue, then unlock, else we'll get a
          // deadlock.
          scoped_lock my_lock(_mutex);
          _wq.clear();
          _status = sQuit;
        }
        ForEach(_workers,i) {
          // Five minute timeout, but this really should never occur, since we
          // expect the threads to be compiling relatively small jobs.
          (*i)->interrupt();
          if (!(*i)->timed_join(boost::posix_time::minutes(2))) {
            ErrExit("Exceeded timeout waiting for JIT thread to shutdown.\n");
          }
          delete *i;
        }
        _workers.clear();
      }
    }

    // Called to indicate a reset condition.  This function clears the work
    // queue and deletes all threads, then starts new ones up again.
    void signal_reset()
    {
      if (is_mt()) {
        TRDBG1("main:  signal reset...\n");
        signal_shutdown();
        create_threads();
      }
    }

    // Add work to the queue.  The block's _func pointer will be updated when
    // done, so that is_compiled() will return true.
    void launch_compile(JitBasicBlock *bb)
    {
      assert(is_mt());
      scoped_lock my_lock(_mutex);
      bb->set_being_compiled();
      _wq.push(bb);      
      _cond.notify_all();
    }
    
    // Main function for compiling a basic block.  This is made public so that a
    // non-mt model can directly call this.  Otherwise, a worker thread calls
    // this, taking data from the work queue.  Do not mix the usage!  Either an
    // ISS is MT or not, but don't mix.
    void compile_bb(JitBasicBlock *bb,int tid = 0)
    {
      TRDBG1("worker " << tid << ":  start compiling 0x" << std::hex << bb->start_addr() << std::dec << "...\n");
      llvm::Function *f = _jit.start_bb(bb);
      addr_t addr = bb->start_addr();
      DecodeItemBase *di = bb->start_item();
      unsigned count = 0;
      for (; count != bb->dc_count(); ++count) {
        if (di->func_matches(_br_func)) {
          _jit.finish_bb_jump(bb,f,bb->bt_t(),count);
          return;
        }
        addr = di->jit_trans(_core,bb,di,addr);
        if (di->func_matches(_brkpt_func)) {
          TRDBG2("worker " << tid << ":  breakpoint at 0x" << std::hex << addr << std::dec << "\n");
          _jit.finish_bb_brkpt(bb,f);
          return;
        }
        di = di->next();
      }
      ++_jit._jit_compiles;
      ++_jit._bb_count;
      _jit.finish_bb(bb,f,count,bb->brkpt());
      TRDBG1("worker " << tid << ":  done compiling 0x" << std::hex << bb->start_addr() << std::dec << ", brkpt:  " << bb->brkpt() << "...\n");
    }

  protected:

    void create_threads()
    {      
      _status = sWork;
      ForRange(_num_threads,i) {
        _workers.push_back(new boost::thread(&JitCompilerImpl::do_work,this,i));
      }
    }

    // The main worker thread.  Reads from the queue if there is data available,
    // otherwise waits on a condition variable until there's work to be done.
    //
    // Notes:
    // 
    // The item is immediately removed from the queue when work begins and held
    // in locals while in flight.  Once done, the block's code pointer is
    // updated.  Since a pointer write is an atomic operation, and the core's
    // thread simply reads it, there's no need for a lock to update it.
    //
    // The JIT translation function *must* *not* modify any core state.  The
    // translation function just generates LLVM stuff.  The compile thread is
    // the only thing doing this, so there's no need to lock LLVM.
    //
    void do_work(int tid)
    {
      try {
        while (JitBasicBlock *bb = get_work(tid)) {
          compile_bb(bb,tid);
        }
      }
      catch (boost::thread_interrupted &x) {
        // We've been interrupted, so we just quit.
        TRDBG1("worker " << tid << ":  interrupted and quitting!\n");
      }
    }

    // Gets basic blocks from the work queue.  Returns 0 if a shutdown request was made.
    JitBasicBlock *get_work(int tid)
    {
      scoped_lock lock(_mutex);

      while (1) {
        switch (_status) {
        case sQuit:
          // Got a shutdown signal.
          TRDBG1("worker " << tid << ":  got quit.\n");
          return 0;
        case sWork:
          if (JitBasicBlock *bb = _wq.pop()) {
            // Got something to compile.
            TRDBG1("worker " << tid << ":  got work.\n");
            return bb;
          } else {
            // Nothing to do, so wait.
            TRDBG1("worker " << tid << ":  waiting for work.\n");
            _cond.wait(lock);
          }
        }
      }
    }

    Status                   _status;       // Note: Updated by the main thread,
                                            // read by the worker thread.
    boost::mutex             _io_mutex;     // In case you want to do any I/O for
                                            // debugging, use this mutex.
    boost::mutex              _mutex;       // Primary mutex used by the condition variable.
    boost::condition_variable _cond;        // Condition variable for thread- waits
                                            // if there's no work in the queue.
    WorkQueue<JitBasicBlock> _wq;           // Basic blocks to compile.  Note:
                                            // No locking in container- you have
                                            // to do it yourself.
    void                    *_core;
    void                    *_br_func;
    void                    *_brkpt_func;
    JitSupport              &_jit;
    unsigned                 _num_threads;
    Threads                  _workers;      // The worker thread(s), if MT.
  };

  JitCompiler::JitCompiler(void *core,void *br_func,void *brkpt_func,JitSupport &jit,unsigned thread_count) :
    _impl(new JitCompilerImpl(core,br_func,brkpt_func,jit,thread_count))
  {}

  JitCompiler::~JitCompiler()
  {
    delete _impl;
  }

  void JitCompiler::signal_shutdown()
  {
    _impl->signal_shutdown();
  }

  void JitCompiler::signal_reset()
  {
    _impl->signal_reset();
  }

  void JitCompiler::launch_compile(JitBasicBlock *bb)
  {
    _impl->launch_compile(bb);
  }

  void JitCompiler::compile_bb(JitBasicBlock *bb)
  {
    _impl->compile_bb(bb);
  }


}


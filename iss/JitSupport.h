//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Support classes and stuff for JIT models.
//

#ifndef _JIT_SUPPORT_H_
#define _JIT_SUPPORT_H_

#include <vector>
#include <string>

#include "BlockHash.h"
#include "DecodeCacheSupport.h"

namespace llvm {

  class LLVMContext;
  class Function;
  class Module;
  class FunctionPassManager;
  class ExecutionEngine;
  class CallInst;
  class Value;
  class FunctionType;
  class PointerType;
  class GlobalVariable;
  class ConstantPointerNull;

}

extern "C" {

  int branch_stub();

}

namespace adl {

  class JitBuilder;

  typedef addr_t (*JitBaseTransFunc)(void *core,void *block,void *decode_item,addr_t pc);  

  // Decode element for when the JIT is used.  Stores information about the
  // byte-coded instruction, plus how to then translate it for the JIT.  No
  // direct threading is supported, since this is just used as an intermediate
  // form.  The direct-threading is only implemented in the JIT itself.
  template <class Core,typename ArgType,size_t NumArgs,class Parm = EmptyClass>
  struct TJitDecodeItem : public TDecodeItem<typename Core::JitDecodeFunc,ArgType,NumArgs,Parm>  {

    typedef TDecodeItem<typename Core::JitDecodeFunc,ArgType,NumArgs,Parm> Base;
    typedef TJitDecodeItem<Core,ArgType,NumArgs,Parm> This;

    typedef typename Core::JitDecodeFunc   DecodeFunc;
    typedef typename Core::JitVoidFunc     VoidFunc;
    typedef typename Core::JitDecodePair   DecodePair;
    typedef typename Core::MyJitBasicBlock MyJitBasicBlock;
    typedef typename Core::JitDecodeItem   MyJitDecodeItem;

    // Type used for translating form a decode item to JIT code.
    typedef typename Core::JitTransFunc   JitTransFunc;

    TJitDecodeItem() : _jit_trans(0) {};
    TJitDecodeItem(DecodeFunc f) : Base(f) {};

    virtual This *next() { return &this[1]; };

    void set_jit_trans(JitTransFunc jf) { _jit_trans = (JitBaseTransFunc)jf; };
    virtual addr_t jit_trans(void *core,void *bb,void *di,addr_t pc) const 
    { 
      return ((JitTransFunc)_jit_trans)(*((Core*)core),(MyJitBasicBlock*)bb,(MyJitDecodeItem*)di,pc); 
    };

  protected:    
    JitBaseTransFunc _jit_trans;
  };
  

  // A basic block looks like a function which takes no arguments and returns an
  // integer code (actually a JitReturnCodes value).
  typedef int (*Dest)();

  // Stores information about a single basic-block.
  class JitBasicBlock : public AdlBasicBlock {
  public:
    enum { NotCompiled = 0, BeingCompiled = 1, IsCompiled = 2 };

    JitBasicBlock() : 
      _func((llvm::Function*)NotCompiled), _bt(0), _fp(), _dc_count(0), _brkpt(false) 
    {};
    JitBasicBlock(addr_t ra) : 
      AdlBasicBlock(ra), 
      _func((llvm::Function*)NotCompiled), 
      _bt(0), _fp(0), _dc_count(0), _brkpt(false) 
    {};
    virtual ~JitBasicBlock() {};

    bool is_compiled() const { return _func >= (llvm::Function*)IsCompiled; };

    void set_being_compiled() { _func = (llvm::Function*)BeingCompiled; };
    bool being_compiled() const { return (_func == (llvm::Function*)BeingCompiled); };

    llvm::Function *get_func() { return _func; };

    int exec() { return _fp(); };
    
    void dump(std::ostream &o) const;
    void dump() const { dump(std::cerr); };

    unsigned dc_count() const { return _dc_count; };
    void clear_dc_count() { _dc_count = 0; };
    void incr_dc_count() { ++_dc_count; };

    bool brkpt() const { return _brkpt; };
    void set_brkpt(bool b) { _brkpt = b; };

    virtual DecodeItemBase *start_item() const = 0;
    virtual JitBasicBlock *bt_t() const = 0;
    virtual JitBasicBlock *bt_f() const = 0;

    void reset()
    {
      _brkpt = false;
    }

  protected:
    friend class JitSupport;

    llvm::Function       *_func;          // The LLVM representation of the basic block.
    llvm::GlobalVariable *_bt;            // Global variable storing the branch
                                          // target, used by anything which
                                          // branches to this bb.
    Dest                  _fp;            // Pointer to machine code.
    unsigned              _dc_count;      // Number of decode items in this sequenc.
    bool                  _brkpt;         // True => This BB has a breakpoint at its beginning.
  };

  inline std::ostream &operator<<(std::ostream &o,const JitBasicBlock &jb)
  {
    jb.dump(o);
    return o;
  }

  // This is the basic block with extra information about the byte encoding.
  // This is the type actually used by the ISS.
  template <class DecodeElm,class Parm = EmptyClass>
  class TJitBasicBlock : public JitBasicBlock, public Parm {
  public:
    typedef typename DecodeElm::VoidFunc       VoidFunc;
    typedef typename DecodeElm::DecodePair     DecodePair;
    typedef TJitBasicBlock<DecodeElm,Parm>     This;

    TJitBasicBlock() : _start(0), _bt_t(0), _bt_f(0) {};
    TJitBasicBlock(addr_t ra) : JitBasicBlock(ra), _start(0), _bt_t(0), _bt_f(0) {};

    bool valid() const { return _start; };

    DecodeElm *start() const { return _start; };
    void set_start(DecodeElm *s) { _start = s; };

    virtual This *bt_t() const { return _bt_t; };
    void set_bt_t(This *t) { _bt_t = t;};

    virtual This *bt_f() const { return _bt_f; };
    void set_bt_f(This *f) { _bt_f = f;};

    // The virtual version of start().
    virtual DecodeItemBase *start_item() const { return _start; };   

    // Given an element in this basic block, determine its index from the
    // starting element.
    unsigned index(DecodeElm *x) const { return (x - _start); };

    void reset()
    {
      Parm::reset();
      JitBasicBlock::reset();
      _start = 0;
      _bt_t = _bt_f = 0;
    }

    DecodePair get_pair() const { return std::make_pair((VoidFunc)_start->func(),_start); };

  protected:
    DecodeElm     *_start;        // Start of the byte-code sequence for this BB.
    This          *_bt_t, *_bt_f; // Taken/not-taken pointers for the end of this basic block.
                                  // we store the target ptr.
  };

  // Main JIT support class, used for generating code via LLVM.  The model instantiates
  // one of these objects if it supports dynamic-binary-translation (a JIT).
  class JitSupport {

  public:

    // Opaque type used to identify a constant parameter.
    typedef llvm::GlobalVariable* ParmId;

    // bcname:      Name of the bitcode file to read.
    // entry_begin: First handler index.
    // entry_end:   Last index (1 past the end).
    // instr_start: Starting index for instruction handlers.
    // td_index:    Index for the temporal-decoupling function.
    // handler_pfx: Prefix used for fetching instruction handlers from the bitcode file.
    // core_name:   Name of the core global.
    // core_ptr:    This is a pointer to a pointer of the ISS, where the pointer is
    //              probably a member variable.  Remember that this is a pointer to a pointer!
    // trace_name:  Name of the global trace-mode flag variable.
    // trace_ptr:   Pointer to a pointer to the ISS object's trace-mode variable.
    JitSupport (const char *bcname,int entry_begin,int entry_end,int instr_start,
                int td_index,const char *handler_pfx,
                const char *core_name,void *core_ptr,
                const char *trace_name,uint64_t *trace_ptr,bool setup_mt);
    ~JitSupport();

    unsigned bb_count() const { return _bb_count; };

    unsigned jit_compiles() const { return _jit_compiles; };

    void reset();

    void reset_bbs();

    // Dumps the module to stderr.
    void dump();

    // Start basic block creation.  Creates an underlying function if one
    // doesn't exist; resets the basic block if a branch stub.  Returns the new
    // function object and does not update the block itself.  This is very
    // important: When used in a multi-threaded system, the block's _func
    // pointer must only be updated when all compilation is complete.
    llvm::Function *start_bb(JitBasicBlock *jb);

    // Initializes the basic block to point to the branch stub (including its
    // global variable target).
    void create_br_stub(JitBasicBlock *jb);

    // Destroy any generated code.  The branch-target GV remains and points to
    // the branch-stub, in case anybody points to this BB.
    void invalidate_bb(JitBasicBlock *jb);

    // Destroy generated code and remove the branch target variable.
    void reset_bb(JitBasicBlock *jb);

    // Add a call to an instruction logger function.
    // n_opc:   Number of opcodes for this instruction.
    // max_opc: Max for all instructions (we pad with 0).
    void add_log_call(int index,uint64_t ea,uint64_t *opc,unsigned n_opc,unsigned max_opc);

    // Set up the post-log call, if relevant.  This just stores the function to
    // be called- we actually call it after the actual instruction handler is
    // called.
    void add_post_log_call(int index,uint64_t ea);

    // Add an instruction call with the given parameters.  The instruction is added to the builder's current
    // target block.
    llvm::CallInst *add_call(int index,int argc,uint64_t *args,void *extra_p = 0,int index_p = -1);

    // Generally used for calling decode-miss- calls a method with an addr_t and
    // an unsigned.
    llvm::CallInst *add_call(int index,addr_t a,uint32_t v);

    // Another type of handler call, generally used by the icache touch function.  Calls
    // a core method with just an address.
    llvm::CallInst *add_call(int index,addr_t a);

    // Add an unconditional branch instruction.
    void add_uncond_branch(JitBasicBlock *jb,int index,JitBasicBlock *taken,int icount,
                           int argc,uint64_t *args,void *extra = 0);

    // Add a conditional branch instruction.  "taken" and "untaken" are the
    // taken and untaken destination blocks of the branch.
    void add_cond_branch(JitBasicBlock *jb,int index,JitBasicBlock *taken,JitBasicBlock *untaken,int icount,
                         int argc,uint64_t *args,void *extra = 0);

    // Complete the setup of a basic block.
    void finish_bb(JitBasicBlock *jb,llvm::Function *f,int icount,bool target_br_stub);

    // Completes a basic block, adding a jump to the end to another basic block.
    // Only needed when a basic block has exceeded its maximum size- this allows
    // for direct threading to its successor.
    void finish_bb_jump(JitBasicBlock *jb,llvm::Function *f,JitBasicBlock *target,int icount);

    // Adds a breakpoint to the end of a basic block and completes the block.
    // This is basically just a return statement which indicates that a
    // breakpoint has occurred.
    void finish_bb_brkpt(JitBasicBlock *jb,llvm::Function *f);

    // Note that this just changes the value used for creating new basic blocks,
    // so you need to invalidate any existing basic blocks in order to see the
    // change.
    void set_tracing(uint64_t trace_flags);

    // For dealing with constant globals, we return an opaque type used as a handler.
    ParmId get_parm_handle(const char *n,void *parm,unsigned init_value);

    // Call this to update the value of a constant parameter- this value will
    // then be used from then on for new basic blocks.
    void set_parm_value(ParmId id,unsigned val);

  protected:
    friend class JitCompilerImpl;

    void finish_internal(JitBasicBlock *jb,llvm::Function *f,int icount,bool target_br_stub,int retcode);
    void createModule(const std::string &bcname);
    void createEngine();
    void createPassMgr();
    void installCorePtr(const char *core_name,void *core_ptr);
    void installTracePtr(const char *trace_name,uint64_t *trace_ptr);
    void loadHandlers(const char *handler_pfx,int entries_start,int entries_end,int instr_start);

    void add_td_check(JitBasicBlock *jb,int icount);
    void add_trg_call(JitBasicBlock *);
    void add_branch_handler(llvm::Function *func,llvm::Value *cond,
                            JitBasicBlock *taken,JitBasicBlock *untaken);
    void create_br_target(JitBasicBlock *jb);
    void set_br_target(JitBasicBlock *jb);
    void set_br_target_to_stub(JitBasicBlock *jb);
    void delete_bb_func(JitBasicBlock *jb);

    void insert_post_log_call(int index,uint64_t ea);

    bool targets_branch_stub(JitBasicBlock *jb);

    bool td_enabled() const { return _td_index >= 0; };

    llvm::Function *get_handler(unsigned index) 
    {
      llvm::Function *h =  _handlers[ index - _hoffset ];
      assert(h);
      return h;
    }

    // Would be nice to have these objects be contained within unique_ptr, but
    // this causes problems with LLVM right now.

    llvm::LLVMContext             *_context;   // Unique context for this processor.
    llvm::Module                  *_module;    // Module which stores our 
    JitBuilder                    *_builder;   // IR builder for dynamic generation of code.
    llvm::FunctionPassManager     *_fpm;       // Function-pass manager for optimization. 
    llvm::ExecutionEngine         *_engine;    // The JIT itself.
    llvm::FunctionType            *_bb_type;   // Basic-block function type.
    llvm::PointerType             *_bb_ptr;    // Type for a pointer to a bb function.
    llvm::PointerType             *_vd_ptr;    // A void* type for handling the void* extra argumeent.
    llvm::ConstantPointerNull     *_const_null;
    llvm::GlobalVariable          *_trace_gv;  // Pointer to tracing-control global.
    int                            _td_index;  // Temporal-decoupling API enabled- index to check function.
    int                            _hoffset;   // Offset used for converting
                                               // external handler index to an
                                               // array index.
    std::vector<llvm::Function *>  _handlers;  // All of our handlers.

    llvm::Function                *_taken_stub, *_untaken_stub;
    
    typedef std::vector<llvm::Value *> CallParams;
    CallParams                     _params;    // For instruction-handler call creation.

    bool                           _has_ret;   // True => Current bb has final terminator.
    bool                           _has_tdc;   // True => Current bb's temporal-decoupling check already inserted.
    bool                           _dump;      // True => Dump generated code at end of simulation.

    unsigned                       _bb_count;     // Current number of compiled
                                                  // basic blocks.  Will be
                                                  // reset back to 0 on a reset,
                                                  // whereas _jit_compiles
                                                  // always increments, since
                                                  // it's primarily for
                                                  // statistics purposes.
    unsigned                       _jit_compiles; // Number of JIT compiles
                                                  // (actually perfomed by
                                                  // derived templated class.
    unsigned                       _plog_id;      // If non-zero, call this
                                                  // index as a post-instruction
                                                  // logger call.
    addr_t                         _plog_ea;      // The argument for the post-logger call.
  };

  class JitCompilerImpl;

  // The JIT compiler, which can act either in a multi-threaded or immediate,
  // single-threaded manner.
  class JitCompiler {
  public:
    JitCompiler(void *core,void *br_func,void *brkpt_func,JitSupport &jit,unsigned thread_count);
    ~JitCompiler();

    // Signal a shutdown condition to the compiler.
    void signal_shutdown();

    // Signal a reset condition to the compiler.
    void signal_reset();

    // Add work to the queue.  The block's _func pointer will be updated when
    // done, so that is_compiled() will return true.  This should only be called
    // for multi-threaded operation.
    void launch_compile(JitBasicBlock *bb);

    // The main compilation routine.  For single-threaded operation, call this.
    // For multi-threaded operation, *don't* call this- it's called by the
    // worker thread.
    void compile_bb(JitBasicBlock *bb);

  protected:
    JitCompilerImpl *_impl;
  };

}

#endif

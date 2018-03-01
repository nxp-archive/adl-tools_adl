//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Support classes and stuff for models which implement a decode cache.
//

#ifndef _TRACE_CACHE_SUPPORT_H_
#define _TRACE_CACHE_SUPPORT_H_

#include "helpers/BasicTypes.h"

#include "BlockHash.h"

namespace adl {

  // Base class for decode items.
  struct DecodeItemBase {
    DecodeItemBase() : _func(0) {};
    DecodeItemBase(void *f) : _func(f) {};
    virtual ~DecodeItemBase() {};

    virtual unsigned num_args() const = 0;

    bool func_matches(void *f) const { return _func == f; };

    virtual addr_t jit_trans(void *core,void *bb,void *di,addr_t pc) const { return 0; };

    void reset() { _func = 0; };

    // Use this to increment through an array of decode items, when you don't
    // have access to the final type.
    virtual DecodeItemBase *next() { return 0; };
    
  protected:
    void             *_func;           // Pointer to the execution function for
                                       // this instruction.  At this point, it's
                                       // just a void*- we wrap it in derived
                                       // classes.
  };

  // Stores information about a single instruction.  The argument is a templated
  // type.
  template <typename DecodeFunc,typename ArgType,size_t NumArgs,class Parm>
  struct TDecodeItem : public DecodeItemBase, public Parm {

    TDecodeItem() {};
    TDecodeItem(DecodeFunc f) : DecodeItemBase((void*)f) {};

    DecodeFunc func() const { return (DecodeFunc)_func; };
    void set_func(DecodeFunc f) { _func = (void*)f; };
    
    virtual unsigned num_args() const { return NumArgs; };

    void reset() {
      for (int i = 0; i != NumArgs; ++i) _args[i] = 0;
      DecodeItemBase::reset();
      Parm::reset();
    }

    // Default, for storing simple integers.  The derived classes in the
    // generated code contain versions specific to the architecture which take
    // intbv objects.
    void set_arg(unsigned i,uint32_t x) { _args[i] = x; };

    ArgType          _args[NumArgs];  // Arguments (opcodes, td-count, operands).
  };

  // Empty base class, used as a default for any extra parameters needed in a
  // basic block.  We use inheritence so that we get Empty Base Class
  // Optimization: When inheriting from an empty base class, the size can be 0.
  struct EmptyClass {
    void reset() {};
  };

  // Variant for the standalone decode cache.  Contains branch target
  // information for branch threading.
  template <class Core,class BranchTarget,typename ArgType,size_t NumArgs,class Parm = EmptyClass>
  struct TDcDecodeItem : public TDecodeItem<typename Core::DcDecodeFunc,ArgType,NumArgs,Parm>  {
    // The type of a trace-cache instruction function.
    typedef typename Core::DcDecodeFunc   DecodeFunc;
    typedef typename Core::DcVoidFunc     VoidFunc;
    typedef typename Core::DcDecodePair   DecodePair;

    typedef TDecodeItem<typename Core::DcDecodeFunc,ArgType,NumArgs,Parm> Base;

    TDcDecodeItem() : _bt_t(0), _bt_f(0) {};
    TDcDecodeItem(DecodeFunc f) : Base(f), _bt_t(0), _bt_f(0) {};

    BranchTarget     *_bt_t, *_bt_f;
  };

  // Stores information about a single basic-block.
  template <class DecodeElm,class Parm = EmptyClass>
  class TDcBasicBlock : public AdlBasicBlock, public Parm {
  public:
    typedef typename DecodeElm::VoidFunc   VoidFunc;
    typedef typename DecodeElm::DecodePair DecodePair;

    TDcBasicBlock() { reset(); };
    TDcBasicBlock(addr_t ra) : AdlBasicBlock(ra) { reset(); };

    bool valid() const { return _start; };

    DecodeElm *start() const { return _start; };
    void set_start(DecodeElm *s) { _start = _bt = s; };

    // Generally, the branch target is going to point to the same item as start,
    // but we have two pointers so that we can handle breakpoints.  The idea is
    // that any branch uses bt() as its target, so that we can point to the
    // branch-stub to bounce us back to the kernel in case of a breakpoint at
    // the start of the bb.
    DecodeElm *bt() const { return _bt; };
    void set_bt(DecodeElm *s) { _bt = s; };

    DecodePair get_pair() const { return std::make_pair((VoidFunc)_start->func(),_start); };

    // Given an element in this basic block, determine its index from the
    // starting element.
    unsigned index(DecodeElm *x) const { return (x - _start); };

    // No need to deallocate items here- when the decode cache becomes full, we
    // just reset everything.
    void reset() 
    {
      Parm::reset();
      _start = _bt = 0;
    }

  private:
    DecodeElm   *_start;  // Start of basic block decode-item sequence.
    DecodeElm   *_bt;     // Branch target (used by branches to branch into the BB.
  };

  // The translated decode-cache items.  Basically, just a big page of items.
  // When we fill up, we reset by simply starting again at the beginning.
  template <class DecodeElm_t,class BlockType_t>
  struct TDecodeCache {

    enum DcException { DcCacheFull };

    typedef DecodeElm_t DecodeElm;
    typedef BlockType_t BlockType;

    typedef typename DecodeElm::DecodeFunc DecodeFunc;

    const size_t  _size;       // Number of items in the decode cache.
    DecodeElm    *_items;      // Stores the translation items.
    unsigned      _end;

    DecodeElm    *_br_stub;    // Branch stub- bounces the caller back to the sim kernel.
    DecodeElm    *_brkpt_stub; // Breakpoint stub- bounces the caller back to the sim kernel.

    DecodeFunc    _br_func;
    DecodeFunc    _brkpt_func;

    TDecodeCache(DecodeElm *br_stub,DecodeElm *brkpt_stub,size_t size) :    
      _size(size),
      _items(new DecodeElm[_size]),
      _br_stub(br_stub),
      _brkpt_stub(brkpt_stub),
      _br_func(_br_stub->func()),
      _brkpt_func(_brkpt_stub->func())
    {
      reset();
      // Clear the first item, since it's never used.
      _items[0].reset();
#     ifdef DC_DEBUG
      std::cerr << "Decode-cache translation buffer size:  " << std::dec << _size << "\n";
#     endif
    };

    ~TDecodeCache() {
      delete [] _items;
    }

    size_t buf_size() const { return _size; };

    DecodeElm *operator[](unsigned index) const { return &_items[index]; };
   
    // Allocate an item and return a pointer to it.
    DecodeElm *add_item() 
    {
      DecodeElm *di = &_items[_end++];
      return di;
    }

    // Next item to be used.
    DecodeElm *cur() const
    {
      return &_items[_end];
    }

    // Returns true if the buffer is full.  We always have to allow for the max
    // basic-block length, since we check for full before starting a new
    // translation.
    bool full(unsigned max_bb_size) const
    {
      return ( _end >= (buf_size() - max_bb_size) );
    }

    // We start at element 1 so that an index of 0 can be used as a
    // known-illegal value.
    void reset()
    {
      _end = 1;
    }

    void start_bb(BlockType *bb)
    {
      bb->set_start(cur());
    }

    bool is_br_stub(const DecodeElm *di) const
    {
      return di->func_matches(_br_func);
    }

    bool is_brkpt_stub(const DecodeElm *di) const
    {
      return di->func_mathes(_brkpt_func);
    }

    // Add a branch-stub (bounce back to sim kernel) for the next item.
    DecodeElm *add_br_stub(BlockType *bb)
    {
      DecodeElm *di = add_item();
      di->set_func(_br_func);
      return di;
    }

    // Add a breakpoint-stub (bounce back to sim kernel) for the next item.
    DecodeElm *add_brkpt_stub(BlockType *bb)
    {
      DecodeElm *di = add_item();
      di->set_func(_brkpt_func);
      return di;
    }

    // Not needed for the decode cache.
    void reset_bb(BlockType *) {};

    // Create a branch stub for this decode item.
    void create_br_stub(BlockType *bb)
    {
      bb->set_bt(_br_stub);
    }

    // Nothing needed- reset handled by BlockType::reset().
    void invalidate_bb(BlockType *bb)
    {
    }

  };

  // Variant of the decode store used for standalone decode caches.  Contains stuff for handling
  // branch direct threading.
  //
  // This object is also used as the support object by the block hash, even
  // though most of those functions are just stubbed out (not relevant for the
  // decode-cache, but relevant for the JIT).
  template <class DecodeElm,class BlockType>
  class TDcDecodeCache : public TDecodeCache<DecodeElm,BlockType> {

    typedef TDecodeCache<DecodeElm,BlockType> Base;

    BlockType     _br_blk;

  public:
    TDcDecodeCache(DecodeElm *br_stub,DecodeElm *brkpt_stub,size_t size) :    
      Base(br_stub,brkpt_stub,size)
    {
      _br_blk.set_start(Base::_br_stub);
    };

    // If target_brkpt_stub is true, then we set the branch-target for the cur
    // item to the branch stub.  This is primarily used for breakpoints.
    void start_bb(BlockType *bb,bool target_br_stub)
    {
      Base::start_bb(bb);
      if (target_br_stub) {
        this->create_br_stub(bb);
      }
    }

    // If the di pointer is in fact a standard return code, then we target the
    // branch stub rather than the item given.
    void set_br_trg(DecodeElm *di,BlockType *bt,bool taken)
    {
      BlockType **b = (taken) ? &di->_bt_t : &di->_bt_f;
      if ((bt > (BlockType*)JitNullCode) && (bt < (BlockType*)JitLastCode)) {
        *b = &_br_blk;
      } else {
        *b = bt;
      }
    }

    // When using threading, it's *very* important that when we invalidate a
    // basic block, we set its branch-taken pointer to the branch trampoline.
    // This way, any basic blocks which point to this basic block will get
    // bounced back to the simulation kernel, where we'll then proceed to
    // translate the basic block.
    void invalidate_bb(BlockType *bb)
    {
      bb->set_bt(Base::_br_stub);
    }

  };

  // Variant of the decode store used for JIT decode caches.
  template <class DecodeElm,class BlockType>
  class TJitDecodeCache : public TDecodeCache<DecodeElm,BlockType> {

    typedef TDecodeCache<DecodeElm,BlockType> Base;

  public:
    TJitDecodeCache(DecodeElm *br_stub,DecodeElm *brkpt_stub,size_t size) :    
      Base(br_stub,brkpt_stub,size)
    {
    };

    // If target_brkpt_stub is true, then we set the branch-target for the cur
    // item to the branch stub.  This is primarily used for breakpoints.
    void start_bb(BlockType *bb)
    {
      Base::start_bb(bb);
      bb->clear_dc_count();
    }

  };

}

#endif


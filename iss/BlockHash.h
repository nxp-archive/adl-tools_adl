//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// A generic hash for storing basic block information.  Used by both the decode
// cache the the JIT.
//

#ifndef _BLOCK_HASH_H_
#define _BLOCK_HASH_H_

//#define USE_MAP

// Sometimes it's easier to use a map rather than a hash, since GDB has printers
// for the maps.
#ifdef USE_MAP
# include <map>
# define TBlockAssoc      std::map
# define TBlockMultiAssoc std::multimap
#elif _MSC_VER
# include <unordered_map>
# define TBlockAssoc      std::unordered_map
# define TBlockMultiAssoc std::unordered_multimap
# define OPTIONAL_TR1_NAMESPACE_START
# define OPTIONAL_TR1_NAMESPACE_END
#else
# include <tr1/unordered_map>
# define TBlockAssoc      std::tr1::unordered_map
# define TBlockMultiAssoc std::tr1::unordered_multimap
# define OPTIONAL_TR1_NAMESPACE_START namespace tr1 {
# define OPTIONAL_TR1_NAMESPACE_END   }
#endif


// Ugh.  For MSVC we can use the C++11 hash, and we could for gcc as well, but
// unfortunately, the JIT is so old, that we can't use it, b/c LLVM's library
// won't parse the newer header files provided by gcc.  So, we have to default
// to using the system header files, which means using the tr1 hash.  We need to
// fix this by moving to a newer LLVM for the JIT.
namespace std {

  OPTIONAL_TR1_NAMESPACE_START
  
  // Hash/set support for address/template pairs.
  template<> struct hash<std::pair<adl::addr_t,unsigned> >
  {
    size_t operator()(const std::pair<adl::addr_t,unsigned> x) const { 
      return x.first ^ (x.second << 1);
    }
  };

  OPTIONAL_TR1_NAMESPACE_END
  
}




namespace adl {

  // These codes are returned by the JIT code buffer and indicate why control
  // was returned to the kernel.  Also used by the decode cache, hence why
  // they're here.
  enum JitReturnCodes { 
    JitNullCode,     // Not used by the JIT- I just want the enums to start with
                     // 1 so that they can be used as special values in place of
                     // JitBasicBlock pointers, in some cases. 
    JitRetCode,      // Exit the simulator loop, returning ACTIVE.  Used, for
                     // example, when a temporal-decoupling time-quantum is
                     // exceeded.
    JitBreakPtCode,  // A breakpoint was hit.
    JitNopCode,      // No specific reason- continue executing.  For the
                     // decode-cache, this indicates to jump out of the inner
                     // loop in order to do another BB lookup, e.g. used by
                     // calculated branches.
    JitContCode,     // Used by branches in the decode-cache as a 'fire escape'
                     // to exit out of the current trace sequence, but stay in
                     // the inner loop.  Note: This *must* be larger than the
                     // other codes, because we compare >= against this code to
                     // see if we should stay in the inner loop.
    JitLastCode,     // Not used, but should always represent the last enum in this list.
  };

  // Stores information about a single basic-block.  This is a generic version,
  // which is then specialized for the JIT or decode cache.
  class AdlBasicBlock {
  public:
    AdlBasicBlock() : _hot_count(0) {};
    AdlBasicBlock(addr_t ra) : _start_ra(ra), _end_ra(ra), _hot_count(0) {};

    void incr_hot_count() { ++_hot_count; };
    unsigned hot_count() const { return _hot_count; };
    addr_t start_addr() const { return _start_ra; };
    addr_t end_addr() const { return _end_ra; };
    bool matching_addr(addr_t ra) const { return ra >= _start_ra && ra <= _end_ra; };
    void set_end_ra(addr_t ra) { _end_ra = ra; };

  protected:
    addr_t                _start_ra;      // Starting real address of this bb.
    addr_t                _end_ra;        // Ending real address of this bb.
    unsigned              _hot_count;     // Incremented on lookup to determine frequency of use.
  };


  // This stores the set of code translation pages used by the JIT.
  // Core:          The main core class.
  // BlockType:     Type used for storing basic blocks (JitBasicBlock for JITs, etc.)
  // Support:       Support class used by the JIT.
  // Translate:     Translation functor object.
  // InstrShift:    Alignment of instruction addresses (amount we can shift an
  //                instruction address to the right).
  // EaHashSize:    Size of the effective-address hash, in bits, i.e. real size is 2^EaHashSize.
  // ExceptionType: The core's exception type.  
  template <class Core,class BlockType,class Support,class Translate,
            size_t InstrShift,size_t EaHashBits,typename ExceptionType = int>
  class TBasicBlockHash {

  public:
    enum { 
      BlockPageMask = 0xfff,
      EaHashSize = (1 << EaHashBits),
      EaHashMask = (EaHashSize-1),
    };

    // For storing start-of-basic-block addresses to blocks.
    typedef TBlockAssoc<std::pair<addr_t,unsigned>,BlockType> BlockHash;

    // Hashes on a "page" address so that we can find arbitrary addresses
    // contained within basic blocks.
    typedef TBlockMultiAssoc<addr_t,BlockType*> BlockPtrHash;
    typedef typename BlockPtrHash::iterator bph_iterator;
    typedef std::pair<bph_iterator,bph_iterator> IterPair;

    TBasicBlockHash(Core &core) : _core(core)
    {
    }

    void reset(Support &s,bool quick_reset)
    {
      // The decode-cache can use the quick-reset, as we don't need to reset
      // anything within the basic blocks themselves.
      if (!quick_reset) {
        ForEach(_blocks,i) {
          s.reset_bb(&i->second);
        }
      }
      _blocks.clear();
      _blocks_by_page.clear();
      flush_ea_hash();
      s.reset();
    }

    // Performs a reset just on the basic block, but doesn't clear the cache.
    void reset_bbs(Support &s)
    {
      ForEach(_blocks,i) {
        s.reset_bb(&i->second);
      }
      s.reset_bbs();
    }

    unsigned size() const { return _blocks.size(); };

    void dump(std::ostream &o)
    {
      o << "Basic Block Hash:\n" << std::hex;
      ForEach (_blocks,i) {
        o << "  0x" << i->first << " -> " << &(i->second) << ": " << i->second << '\n';
      }
      o << "\nBlocks By Page:\n";
      ForEach (_blocks_by_page,i) {
        o << "  0x" << i->first << " -> " << i->second << '\n';
      }
    }

    void dump() const { dump(std::cerr); };

    BlockType * get_bb_ra_no_translate(addr_t ra,unsigned mask)
    {
      std::pair<typename BlockHash::iterator,bool> ip = _blocks.insert(std::make_pair(std::make_pair(ra,mask),BlockType(ra)));
      return &(ip.first->second);
    }

    // Looks up a basic block, returning an empty block if not found.  Since an
    // empty block is init'd to point to the branch stub, this will, by default,
    // just bounce back to the kernel.
    BlockType *get_bb_ea_no_translate(addr_t ea,unsigned mask,bool active)
    {
      if (BlockType *bb = lookup_ea_hash(ea,mask)) {
        if (active) bb->incr_hot_count();
        return bb;
      }
      addr_t ra = _core.instrReadTranslate(ea);
      BlockType *bb = get_bb_ra_no_translate(ra,mask);
      insert_ea_hash(ea,mask,bb);
      if (active) bb->incr_hot_count();
      return bb;
    }

    // Look up and return a code basic block based upon an address.  If not found, or the
    // target is not valid, then perform a translation.
    BlockType* get_bb(Translate &translate,addr_t ea,unsigned mask,unsigned hot_threshold)
    {
      BlockType *bb = get_bb_ea_no_translate(ea,mask,true);
      if (!bb->valid()) {
        if (translate.full(_core)) {
          // If jit memory is full, then we have to re-insert this block, since it was erased.
          bb = get_bb_ea_no_translate(ea,mask,true);
        }
        if (bb->hot_count() < hot_threshold) {
          // Not hot, so just interpret the block.  Return 0 so that we know not
          // to try and execute the JIT code and that we should interpret.  We
          // assume the NIA is at the correct address, so we just interpret from
          // our current location.
          return 0;
        }
        // No valid code at this address, so we must translate.
        translate(_core,ea,bb);
        // Then add to the block page hash.
        add_to_page_hash(bb);
      }
      return bb;
    }

    // Looks up a basic block by ea, creates a branch stub if not found.
    BlockType *get_bb_or_stub(addr_t ea,unsigned mask,Support &s)
    {
      BlockType *bb = 0;
      try {
        bb = get_bb_ea_no_translate(ea,mask,false);
      } catch (ExceptionType) {
        // If we catch an exception, then we've got a situation where the target
        // address is not executable.  However, keypoint here, we can't install
        // a stub into the hash b/c other code might be able to execute this.
        // For example, supervisor code that's accidentally targeted by user
        // code isn't executable by the user code, but is executable by other
        // supervisor code.  Thus, for this situation, the return value is a
        // special value which indicates that the calling basic block, rather
        // than chaining, will just return to the kernel.  This way, the next
        // lookup will cause the exception.  However, in some cases, it might no
        // longer cause an exception, so we do want to allow for that, e.g. if
        // we're running code which does some sort of dynamic linking via iTLB
        // exceptions, or something like that.
        return (BlockType*)JitNopCode;
      }
      if (!bb->valid()) {
        s.create_br_stub(bb);
      }
      return bb;
    }

    // For a given real address, invalidate all basic blocks which contain the
    // specified address.  Returns the number of items invalidated.
    unsigned invalidate_ra(addr_t ra,Support &s)
    {
      unsigned count = 0;
      // Get all items which match the page pointed to by ra.
      IterPair p = _blocks_by_page.equal_range(get_page(ra));
      // If we have any items, we have to iterate through them to find any that
      // truly match, since the initial search was just by page number.
      bph_iterator tmp;
      while ( p.first != p.second ) {
        if (p.first->second->matching_addr(ra)) {
          p.first->second->reset();
          s.invalidate_bb(p.first->second);
          tmp = p.first++;
          _blocks_by_page.erase(tmp);
          ++count;
        } else {
          ++p.first;
        }
      }
      return count;
    }

    // Invalidates matching addresses by ea.
    unsigned invalidate_ea(addr_t ea,Support &s)
    {
      addr_t ra;
      if (_core.extInstrReadTranslate(ra,ea)) {
        return invalidate_ra(ra,s);
      } else {
        return 0;
      }
    }

    // Flush the EA hash, such as for when a context swap occurs.
    void flush_ea_hash()
    {
      memset(_ea_hash,0,sizeof(EaHashEntry)*EaHashSize);
      _ea_hash_collisions = 0;
    }

    // This is only tracked if the model is compiled with JIT_DEBUG.
    unsigned ea_hash_collisions() const { return _ea_hash_collisions; };

  protected:

    // For a given basic block which has been translated, and thus has a start
    // and end address, add entries for all page numbers within that range.
    void add_to_page_hash(BlockType *jb)
    {
      addr_t s = get_page(jb->start_addr());
      addr_t e = get_page(jb->end_addr());

      for ( ; s <= e; s += (BlockPageMask+1)) {
        _blocks_by_page.insert(std::make_pair(s,jb));
      }
    }

    addr_t get_page(addr_t a)
    {
      return (a & ~BlockPageMask);
    }

    // Hash function for effective addresses into the small, fast hash.
    // Basically, I just xor some bits togther- we need to analyze its
    // performance at some point.
    unsigned ea_hash_func(addr_t a,unsigned mask)
    {
      a >>= InstrShift;
      addr_t tmp = ((a >> (2*EaHashBits)) ^ (a >> EaHashBits) ^ a) + mask;
      return (unsigned)(tmp & EaHashMask);
    }

    // Looks up an entry in the small, fast ea hash, returns 0 if not found, bb
    // ptr if found.
    BlockType *lookup_ea_hash(addr_t ea,unsigned mask)
    {
      unsigned index = ea_hash_func(ea,mask);
      if (_ea_hash[index].match(ea,mask)) {
        return _ea_hash[index].get_bb();
      }
      return 0;
    }

    // Inserts an entry into the fast hash.
    void insert_ea_hash(addr_t ea,unsigned mask,BlockType *bb)
    {
      unsigned index = ea_hash_func(ea,mask);
#     ifdef JIT_DEBUG
      if (_ea_hash[index].get_bb()) ++_ea_hash_collisions;
#     endif
      _ea_hash[index].set(ea,mask,bb);
    }

  private:
    struct EaHashEntry {
      addr_t     _ea;
      unsigned   _mask;
      BlockType *_bb;

      bool match(addr_t a,unsigned mask) const { return (_bb && (a == _ea) && (mask == _mask)); };
      BlockType *get_bb() const { return _bb; };

      void set(addr_t ea,unsigned mask,BlockType *bb) 
      { 
        _ea   = ea;
        _mask = mask;
        _bb   = bb; 
      };
    };

    BlockHash      _blocks;
    BlockPtrHash   _blocks_by_page;
    EaHashEntry    _ea_hash[EaHashSize];
    unsigned       _ea_hash_collisions;
    Core          &_core;
  };

}

#endif


//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

#ifndef _UADL_TYPES_H_
#define _UADL_TYPES_H_

#include "iss/ModelInterface.h"

#include "Timer.h"
#include "BandwidthControl.h"

namespace uadl {

  using adl::byte_t;
  using adl::addr_t;

  typedef uint64_t mask_t;

  // This is used to identify instructions within the core for the purpose of
  // resource allocation.  The assumption is that we'll have less than 32k
  // instructions within a core at the same time, so it's OK to use a small
  // value.
  typedef short tag_t;

  enum { InvalidTag = -1 };

  class CacheStatusHint;
  struct LogClient;
  class Logger;

  // Branch prediction results.
  enum Prediction { Miss, Taken, NotTaken, NoPrediction };

  // Used to identify register operands.  NotReg is returned when something
  // isn't a register, e.g. an immediate, vs. InvalidReg, meaning that we don't
  // have a valid item.
  enum OpId { NotRegOp = -2, InvalidOp = -1 };

  // Stores information useful for reporting in a memory request wrt. the instruction
  // responsible for the request.
  struct ReqInfo {
    int64_t  _instId;   // Issue ID of instruction generating request; -1 if not valid.
    int      _threadId; // Thread ID (if relevant) of instruction; -1 if not valid.

    ReqInfo() : _instId(-1), _threadId(-1) {};
    ReqInfo(int64_t iid,int tid) : _instId(iid), _threadId(tid) {};

    bool valid_iid() const { return _instId >= 0; };
    bool valid_tid() const { return _threadId >= 0; };
  };


  // Stores information about a single memory access/request.  We derive from
  // the one in ADL in order to add some extra information, such as an ID, etc.
  // Data space is allocated at startup time.
  class UadlMemAccess : public adl::MemAccess {
  public:
    UadlMemAccess() : 
      _id(0), 
      _request_id(-1), 
      _offset(0), 
      _serialized(0),
      _dvalid(false), 
      _data(0), 
      _hint(0),
      _next(0) 
    {};

    ~UadlMemAccess();

    void alloc(unsigned bufsize) {
      _data = new byte_t[bufsize];
    }

    void reset(unsigned id,const adl::MemAccess &ma) 
    {
      _id = id;
      _request_id = -1;
      _next = 0;
      _dvalid = false;
      _offset = 0;
      _serialized = 0;
      ((adl::MemAccess&)(*this)) = ma;
    }

    unsigned id() const { return _id; };
    unsigned offset() const { return _offset; };
    byte_t *data() { return _data; };
    CacheStatusHint *hint() { return _hint; };
    UadlMemAccess *next() { return _next; };

    void set_request_id(int i) { _request_id = i; };
    int request_id() const { return _request_id; };
    bool valid_request_id() const { return _request_id >= 0; };

    void set_valid_data() { _dvalid = true; };
    bool has_valid_data() const { return _dvalid; };

    // Specify that the access should be serialized at the specified level in
    // the hierarchy.
    void set_serialized(unsigned s) { _serialized |= s; };
    // Set serialized for the speciied level in the hierarchy, e.g. 0 means
    // first level of memory should be serialized.
    void set_serialized_level(unsigned l) { _serialized |= (1 << l); };
    void clr_serialized(unsigned s) { _serialized &= ~s; };

    unsigned         _id;
    int              _request_id;  // ID within the memory subsystem.
    unsigned         _offset;
    unsigned         _serialized;
    bool             _dvalid;      // True if data is valid.
    byte_t          *_data;
    CacheStatusHint *_hint;        // Cache status info (used by safe-mode models).
    UadlMemAccess   *_next;
  };  

  struct FetchExceptionData : public adl::PacketLogBase {
    FetchExceptionData(unsigned exdata,const adl::ExHandle &exitems,addr_t addr,adl::LogType l) : _addr(addr)
    {
      _exdata = exdata;
      _exitems = exitems;
      _log = l;
    }

    addr_t       _addr;
  };

  // Used to identify prediction status for branches.
  enum PredResult { 
    PredNone,      // Initial value.
    PredTaken,     // Instruction is predicted taken.
    PredNotTaken,  // Instruction is predicted not-taken.
    PredCorrect,   // Final prediction was correct.
    PredIncorrect  // Final prediction not correct.
  };

  class InstrBase {
  public:
    InstrBase() : _iid(0), _cid(0), _instr_class(0), _latency(0), _fedata(0), 
                  _status(adl::MemAccess::opNone), _rqhead(0), _mhead(0), _mtail(0) {};

    unsigned iid() const { return _iid; };

    unsigned cid() const { return _cid; };

    unsigned latency() const { return _latency; };

    unsigned instr_class() const { return _instr_class; };

    void set_latency(unsigned l) { _latency = l; };

    void clear_latency() { _latency = 0; };

    bool check_latency(Logger *l);

    void decr_latency() { if (_latency) --_latency; };

    UadlMemAccess *rq_mem() { return _rqhead; };
    const UadlMemAccess *req_mem() const { return _rqhead; };

    UadlMemAccess *mem() { return _mhead; };
    const UadlMemAccess *mem() const { return _mhead; };

    UadlMemAccess *mtail() { return _mtail; };

    void next_req_mem() { if (_rqhead) _rqhead = _rqhead->_next; };

    bool multiple_mem() const { return _mhead && _mhead != _mtail; };

    bool has_next_request() const { return _rqhead; };

    bool has_more_mem() const { return _mhead; };

    void add_mem(UadlMemAccess *mem) { 
      if (_mhead == 0) {
        _mhead = mem;
        _rqhead = _mhead;
      } else {
        _mtail->_next = mem;
        if (!_rqhead) _rqhead = mem;
      }
      _mtail = mem;
    };

    UadlMemAccess *get_mem_and_clear() { 
      UadlMemAccess *tmp = _mhead;
      _mhead = tmp->_next;
      // Note: _mtail can be stale because we only check _mhead on enqueue
      // operations.
      return tmp;
    };

    // Returns true if we have a memory access and it is misaligned.
    bool is_misaligned(unsigned mask) const {
      return (_mhead && (_mhead->_ra & mask));
    }

    // Returns true if we have a serialized memory access.
    bool is_serialized(unsigned mask = -1) const {
      return (_mhead && (_mhead->_serialized & mask));
    }

    void set_serialized_level(unsigned l) const {
      if (_mhead) {
        _mhead->set_serialized(l);
      }
    }

    void set_serialized(unsigned s) const {
      if (_mhead) {
        _mhead->set_serialized(s);
      }
    }

    void clr_serialized(unsigned s) const {
      if (_mhead) {
        _mhead->clr_serialized(s);
      }
    }

    bool is_fetch_exception() const { return _fedata; };

    // Returns true if we have a memory access and its request has been sent.
    bool has_requested_mem_op() const {
      return (_mhead && _mhead->valid_request_id());
    }

    void set_fetch_exception(unsigned exdata,const adl::ExHandle &exitems,addr_t addr,adl::LogType l) {
      _fedata = new FetchExceptionData(exdata,exitems,addr,l);
    }

    void discard_fetch_exception() {
      delete _fedata;
      _fedata = 0;
    }

    void reset(unsigned iid,unsigned cid) {
      _iid = iid;
      _cid = cid;
      _status = adl::MemAccess::opNone;
      _fedata = 0;
      _mhead = _mtail = _rqhead = 0;
      _latency = 0;
    }

    void reset(unsigned iid,unsigned cid,unsigned iclass) {      
      _instr_class = iclass;
      reset(iid,cid);
    }
    
    unsigned               _iid;        // Unique ID for the instruction.  This
                                        // is the issue ID and thus may include
                                        // instructions speculatively executed.
    unsigned               _cid;        // Commit-ID, to make it easy to correlate with ADL traces.
    unsigned               _instr_class;// Identifier for the instruction class.
    unsigned               _latency;    // Latency counter.
    FetchExceptionData    *_fedata;     // Relevant if the core has support for fetch exceptions.
    adl::MemAccess::OpType _status;     // Only used for memory ops.
    UadlMemAccess             *_rqhead;     // Head of requests.
    UadlMemAccess             *_mhead;      // If we have memory ops, next-access/deletions occurs here.
    UadlMemAccess             *_mtail;      // Insertions occur here for memory ops.
  };

  //
  // Base resource class- contains a link to a timer and infrastructure for logging.
  //
  class Resource {
  public:
    Resource(const Timer &timer) : _timer(timer) {}
    virtual ~Resource() {};

    // Shortcut method for time
    ttime_t currTime() const { return timer().currTime(); }

    // Bandwidth management methods
    bool hasBandwidth(const BandwidthControl &bandwidth) const;
    bool consumeBandwidth(BandwidthControl &bandwidth);

    // Access/query methods
    const Timer &timer() const { return _timer; }

    // Create any internal logging objects, if relevant.  If a logger exists
    // internally, then the LogClient object should be updated.
    virtual void createLogger(LogClient *lc,uint32_t tflags) {};

    // Disable and remove any internal logging objects, if relevant.
    virtual void removeLogger() {};

    // Display any logging messages for this cycle, if relevant, then clear
    // whatever is necessary in order to prepare for the next cycle.
    virtual bool report_and_clear(std::ostream &o,const std::string &pfx) { return false; };

  private:

    const Timer &_timer;
  };

  inline bool Resource::hasBandwidth(const BandwidthControl &bandwidth) const
  { return bandwidth.hasBandwidth(currTime()); }
  

  inline bool Resource::consumeBandwidth(BandwidthControl &bandwidth)
  { return bandwidth.consumeBandwidth(currTime()); }



  //
  // Instruction object cache.  This is implemented as a simple, direct-mapped
  // cache.  The target of the cache is a ring of entries, where each entry
  // contains an array of elements.  Each element is a linked list of
  // instruction objects.
  //
  // On a lookup, we index into the appropriate target and compare the lookup
  // address to the tag.  If it doesn't match, we search the ring for a match.
  // If it's then found, that element becomes the first entry for the target
  // address.  This way, we never have to discard an entry in the cache; it just
  // becomes an element in the ring and we might simply switch between what
  // element the target initially points to.
  //
  // Each item in the ring is an array of elements; we take lower bits of the
  // address and use that to index into the array.  Each element of the array is
  // a linked list of instruction objects b/c a core's pipeline might have
  // multiple instruction objects in-flight for the same address, e.g. for a
  // tight loop of a branch and an arithmetic instruction.
  //
  // Final note: We hash usin real-addresses so that we don't have to worry
  // about aliasing.  Since this is for a cycle-accurate model, we can't easily
  // skip the modeling of the hardware cache and its various updates.
  //
  // We also don't really handle self-modifying code at this point.  It will
  // need to somehow invalidate the cache, but that's not well handled.  Since
  // we're mainly targeting embedded systems and benchmarks right now, this
  // probably isn't a big deal, but it's something to keep in mind.
  //
  // So, the basic steps in using this cache are:
  // 
  // 1.  Given an address, shift and mask to extract the set.
  //
  // 2.  For the specified set, does the first element's tag match the address?
  //
  //     1.  If yes, then proceed to 3.
  //
  //     2.  If no, then search through the ring until we get back to the first item,
  //         trying to find a match.  If not found, return false.
  //
  // 3.  Extract index by shifting by the instruction-shift (alignment of
  //     instruction, e.g.  32-bit instructions allow a 2-bit shift b/c they're
  //     always aligned) and mask.
  //
  // 4.  If element contains a non-zero pointer, then remove from list and return item.
  //     Otherwise, return 0.
  //
  // If the result is 0 (not found), then call normal decode routine.
  //
  // At time of instruction commit, return the instruction to the cache (it's
  // possibly its first time going into the cache).
  //
  // 1.  Normal lookup of set and search of ring.
  //
  // 2.  If not found, add new element to appropriate set, making it first element in ring.
  //
  // 3.  Return to linked list at appropriate index in array element.
  //
  // Template Parameters:
  //
  // Instr:      Instruction object type.
  // CacheSize:  Number of entries in the outer, direct-mapped cache.  Must be a power of 2.
  // CacheShift: Shift amount, in bits, to use, before extracting the entry index.  Note that this
  //             incorporates the InstrShift value.
  // EntrySize:  Number of elements in each entry.  Must be a power of 2.
  // InstrShift: Number of bits to shift before extracting the final index.
  template <typename Instr,unsigned CacheSize,unsigned CacheShift,unsigned EntrySize,unsigned InstrShift>
  class InstructionCache {

    enum {
      CacheMask  = (CacheSize-1),         // Mask for extracting the cache set entry.
      TagMask    = ((1 << CacheShift)-1), // Mask used for tag extraction.
      EntryMask  = (EntrySize-1),         // Mask for extracting the final index.
    };

    // Each of these elements represents a 'way' in a ring buffer for a given
    // set.
    struct Entry {
      Entry(addr_t addr) : 
        _tag(addr & ~TagMask), _next(this), _prev(this)
      {
        memset(_items,0,sizeof(Instr*)*EntrySize);
      }

      Entry(addr_t addr,Entry *prior) : 
        _tag(addr & ~TagMask), 
        _next(prior), 
        _prev(prior->_prev)
      {
        prior->_prev = this;
        _prev->_next = this;
        memset(_items,0,sizeof(Instr*)*EntrySize);
      }

      // Delete all elements.  This is just a single-linked list of elements.
      ~Entry() 
      {
        for (int i = 0; i != EntrySize; ++i) {
          while (_items[i]) {
            Instr *tmp = _items[i];
            _items[i] = tmp->_next;
            delete tmp;
          }
        }
      }

      // If an item exists at the specified index, then remove it from the
      // linked list and return it.  Otherwise return 0.
      Instr *find(unsigned index) 
      {
        if (_items[index]) {
          Instr *tmp = _items[index];
          _items[index] = tmp->_next;
          return tmp;
        }
        return 0;
      }

      void insert(unsigned index,Instr *instr) 
      {
        Instr *tmp = _items[index];
        _items[index] = instr;
        instr->_next = tmp;
      }

      addr_t  _tag;
      Instr  *_items[EntrySize];
      Entry  *_next;
      Entry  *_prev;
    };

  public:

    InstructionCache()
    {
      memset(_entries,0,sizeof(Entry*)*CacheSize);
    }

    ~InstructionCache()
    {
      reset();
    }

    // The reset routine frees all elements in the cache, completely resetting
    // everything.  The tricky thing here is that the list is circular.
    void reset()
    {
      for (int i = 0; i != CacheSize; ++i) {
        if (Entry *first = _entries[i]) {
          do {
            Entry *tmp = _entries[i];
            _entries[i] = tmp->_next;
            delete tmp;
          } while (_entries[i] != first);
          _entries[i] = 0;
        }
      }
    }

    unsigned get_set(addr_t addr)
    {
      return (addr >> CacheShift) & CacheMask;
    }

    unsigned get_index(addr_t addr)
    {
      return (addr >> InstrShift) & EntryMask;
    }

    bool match_tag(Entry *entry,addr_t addr)
    {
      return ((entry->_tag ^ addr) & ~TagMask) == 0;
    }

    // When performing a lookup, if we find an entry which matches, but it's not
    // at the front, we move it to the front.  Since this is a circular list, we
    // needen't mess with anything else.
    //
    // Returns 0 if item is not found.
    Instr *find(addr_t addr)
    {
      unsigned set = get_set(addr);
      Entry *entry = _entries[set];
      if (entry) {
        if (match_tag(entry,addr)) {
          // Match at first entry.
          return entry->find(get_index(addr));
        } else {
          // Might be in the ring, but it's not at the first point.
          Entry *iter = entry->_next;
          while (iter && iter != entry) {
            if (match_tag(entry,addr)) {
              // Found a matching entry, so boost it to the current spot.
              _entries[set] = iter;
              return iter->find(get_index(addr));
            }
            iter = iter->_next;
          }
        }
      }
      // Not found in cache.
      return 0;
    }

    // Insert an instruction at the given address.  We need to do a complete
    // lookup, in case the cache has changed due to subsequent find operations.
    // However, here we don't modify the ring b/c this is normally called by
    // commit; we're done with this instruction, so we might never need to look
    // it up again.
    void insert(addr_t addr,Instr *instr)
    {
      unsigned set = get_set(addr);
      Entry *entry = _entries[set];
      if (entry) {
        if (match_tag(entry,addr)) {
          // Match at first entry.
          entry->insert(get_index(addr),instr);
        } else {
          // Might be in the ring, but it's not at the first point.
          Entry *iter = entry->_next;
          while (iter && iter != entry) {
            if (match_tag(entry,addr)) {
              iter->insert(get_index(addr),instr);
              return;
            }
            iter = iter->_next;
          }
          // At this point, it's not found, so we need a new entry in the ring.
          Entry *entry = new Entry(addr,_entries[set]);
          entry->insert(get_index(addr),instr);
        }
      } else {
        Entry *entry = _entries[set] = new Entry(addr);
        entry->insert(get_index(addr),instr);
      }
    }

  protected:
    Entry *_entries[CacheSize];
  };

}

#endif

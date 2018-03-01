//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

// 
// Miscellaneous data structure definitions and support routines used by the
// testwriters.
//

#ifndef MISCWRITER_H
#define MISCWRITER_H 

#include <string>
#include <cassert>
#include <list>
#include <vector>
#include <map>
#include <set>
#include <memory>

#include "rnumber/RNumber.h"

#include "ModelInterface.h"
#include "LoggingMgr.h"
#include "NodeHelpers.h"

//
// Note that in general, the data structures used by the UVP writer are not
// garbage collected because of the fact that many of them use RNumber, which is
// an external class and is not GC'd.  Thus, they must be cleaned up manually,
// meaning that we would need finalized objects.  However, if we do that, then
// we want to clear out internal containers at the end of a test so that
// constituent elements can be collected, e.g. the list of instruction entries
// should be cleared because otherwise, they would be considered reachable,
// since the parent object, CoreInfo, is finalizable and if a finalizable object
// points to some object, then that object is not considered collectable.  In
// order to clear out these internal data structures, the code looks very
// similar to a destructor, so I just decided to simply forego garbage
// collection.
//
namespace adl {
  struct FieldData;

  //
  // the type of the intermediate result
  //
  enum UpdateType {register_write=1,memory_write,memory_read,interrupt,mmu_write,mmu_access,annotation};

  //----------------------------------------------------------------------
  // An intermediate result. Supports memory, tlb, annd registers.
  //----------------------------------------------------------------------
  class InterResult {  
  public:
    // a reg update
    InterResult (const std::string &reg,const rnumber::RNumber &value) : 
      _ir_type (register_write), _name(reg), _ea(0), _ra(0), _wimg(0), _mem_value(0), _reg_value(value) {};
    // a mem update
    InterResult (UpdateType type, addr_t ea,addr_t ra, unsigned wimg,uint32_t value) : 
      _ir_type (type), _ea(ea), _ra(ra), _wimg(wimg), _mem_value(value), _reg_value(0) {};
    // an interrupt
    InterResult (const std::string& msg) :
      _ir_type (interrupt), _ea(0), _ra(0), _wimg(0), _mem_value(0), _reg_value(0),_msg(msg) {};
    // An annotation or message.
    InterResult (UpdateType t,const std::string& msg) :
      _ir_type (t), _ea(0), _ra(0), _wimg(0), _mem_value(0), _reg_value(0),_msg(msg) {};
    // an mmu write
    InterResult (const std::string &lookup,uint32_t id,addr_t epn,int set,int way) :
      _ir_type (mmu_write), _name(lookup), _ea(epn), _ra(0), _wimg(id), _set(set), _way(way) {};
    InterResult (UpdateType type, const std::string &lookup,uint32_t id,addr_t epn,int set,int way) :
      _ir_type (type), _name(lookup), _ea(epn), _ra(0), _wimg(id), _set(set), _way(way) {};
  
    UpdateType ir_type() { return _ir_type; }
    const std::string& name () { return _name;}
    const std::string& msg () { return _msg;}
    addr_t ea() { return _ea;}
    addr_t ra() { return _ra;}
    uint32_t wimg() { return _wimg;}
    uint32_t id() { return _wimg; };
    const rnumber::RNumber& reg_value() { return _reg_value;}
    addr_t mem_value() { return _mem_value;}
    int way() const { return _way; };
    int set() const { return _set; };
  
    FieldData        _fd;
  private:
    UpdateType       _ir_type;
    std::string      _name;
    addr_t           _ea;
    addr_t           _ra;
    uint32_t         _wimg;
    union {
      uint32_t       _mem_value;
      struct {
        uint16_t     _set;
        uint16_t     _way;
      };
    };
    rnumber::RNumber _reg_value;
    std::string      _msg;
  };

  typedef std::list <InterResult> InterResults;

  struct TargetAnnotation {
    
    TargetAnnotation(addr_t tea, addr_t tra, unsigned tta) :
      _tea(tea), _tra(tra), _tta(tta)
    {}

    addr_t   _tea, _tra;
    unsigned _tta;
  };

  typedef std::unique_ptr<TargetAnnotation> TrgAnnoPtr;

  //----------------------------------------------------------------------
  // an instruction
  //----------------------------------------------------------------------
  class InstrEntry { 
  public:
    struct IDataEl {
      IDataEl() : _ra(0), _d(0), _wimg(0) {};
      IDataEl(addr_t ra,uint32_t d,uint32_t w) : _ra(ra), _d(d), _wimg(w) {};

      addr_t   _ra;
      uint32_t _d;    // The instruction's data, as represented in memory (may be byte-swapped).
      uint32_t _wimg;
    };

    typedef std::vector<IDataEl> IData;

    // Items in the trace section of a UVP (E:I, E:RI) can have different
    // meaning- they're not all just instructions.
    enum Type { 
      Instr,     // Standard instruction.
      Asynch,    // Asynchronous exception.
      ExtWrite   // An external register write event.
    };

    // Note: We always start off with one element of data storage so that we can
    // rely upon there always being an element 0.
    InstrEntry() : _type(Instr), _asml(""),_ea(0), _d(1), _opc(0), _nhb(0), _squashed(false), 
                   _from_buf(false), _flags(0), _trg_anno(nullptr), _taken(false), _gcount(0) {}
    
    void setFromBuffer(bool b) 
    { 
      _from_buf = b; 
    }

    void setEffAddr(addr_t ea) 
    {
      _ea = ea;
    }

    void setTrgAnno(addr_t tea,addr_t tra,unsigned tta)
    {
      if (!_trg_anno.get()) {
        _trg_anno = TrgAnnoPtr(new TargetAnnotation(tea,tra,tta));
      }
    }

    const TargetAnnotation *getTrgAnno() const
    {
      return _trg_anno.get();
    }
    
    // Returns true if it is a new memory request
    bool setAddrAndData(addr_t ra, uint32_t d,uint32_t wimg,unsigned index) 
    {
      if (index == 0) {
        // The first ra should be aligned with the ea.
        ra = (ra & ~0xfULL) | (_ea & 0xfULL);
        _d[0] = IDataEl(ra,d,wimg);
        return true;
      } else {
        if (ra != _d[index-1]._ra) {
          _d.push_back(IDataEl(ra,d,wimg));
          return true;
        } else {
          return false;
        }
      }
    }

    void setInfo(const std::string& a, const uint32_t *opc = 0, int num_half_bytes = 0, unsigned gcount = 0, uint32_t flags = 0) 
    {   
      // num bytes will refer to the fetch_width.
      int num_bytes = num_half_bytes/2;     
      
      // Assembler
      _asml = a;
      
      _gcount = gcount;
      
      _flags = flags;

      unsigned w = num_bytes > 0 ? (num_bytes+3)/4 : 1;
      
      _opc.resize(32*w);
      
      if (opc) {
        _opc = rnumber::RNumber(opc,w,32*w);
      }
      
      unsigned shift = (32*_opc.wordCount() - 8*num_bytes);       
      _nhb = num_half_bytes;
     
      _opc >>= shift;
      if (num_bytes) {
        _opc.resize(num_bytes*8);      
      }
      int start = (_ea & 0x3ULL);
      int nb1 = 4-start;
      // Remove any unnecessary upper bits.
      uint32_t mask = (uint32_t)-1;

      _d[0]._d &= ( mask >> nb1*8 );

      // This is only relevant if we have more than one item in our data list.
      if (nb1 >= num_bytes) {
        // If the number of bytes is such that there's no actual data in the
        // second instruction data entry, then clear that value out and adjust
        // the data value in the first entry to right-justify.
        _d[0]._d >>= (num_bytes - start)*8;
        _d.erase(_d.begin()+1,_d.end());
      } else {
        // Otherwise, more entries are valid, so we must adjust all data values.
        int stop = -1;
        for (unsigned i = 1; i != _d.size(); ++i) {
          int remained = num_bytes - (4*(i-1) + nb1);
          if (remained > 0) {
            if (remained <= 4) {
              //last
              _d[i]._d >>= (remained*8);
            } else {
              //not last - do nothing
              ;
            }	  
          } else {
            // not used- we'll erase from here on.
            stop = i;
            break;
          }
        }
        if (stop >= 0) {
          // Erase any unused items.
          _d.erase(_d.begin()+stop,_d.end());
        }
      }
    }

    void setBranchTaken(bool t) { _taken = t; };

    // For external writes, we need to store the written data and the count.
    void setExtWriteInfo(const std::string &n,const rnumber::RNumber &d,unsigned gcount) { _asml = n; _opc = d; _gcount = gcount; };

    InterResult &add_ir (InterResult ir) {
      _irs.push_back(ir); 
      return _irs.back();
    }

    void set_squashed(bool s) {
      _squashed = s;
    }

    bool squashed() const { return _squashed; };

    bool from_buf() const { return _from_buf; };

    Type type() const { return _type; };
    void set_type(Type t) { _type = t; };

    const InterResults &irs() const { return _irs; }
    const std::string& asm_() const { return _asml; }
    addr_t ea() const { return _ea;}
    unsigned gcount() const { return _gcount; };
    const IDataEl &data(unsigned index) const { assert(index < _d.size()); return _d[index];}
    const IData &data() const { return _d; }
    bool valid_instr() const { return _nhb != 0; };
    bool branch_taken() const { return _taken; };
    uint32_t flags() const { return _flags; };
    // Note:  if 0, indicates that the instruction was never initialized.
    int num_half_bytes() const { return _nhb; };
    const rnumber::RNumber& opc()const  {return _opc;}

    const std::string& ext_write_name() const { return _asml; };
    const rnumber::RNumber &ext_write_value() const { return _opc; };

  private:
    Type                 _type;
    std::string          _asml;
    addr_t               _ea;
    IData                _d;
    rnumber::RNumber     _opc;       // The instruction's opcode, as disassembled by the core.
    int                  _nhb;       // Instruction's length in bytes.
    bool                 _squashed;  // If set, this instruction will not be printed.
    bool                 _from_buf;  // Indicates that instr was executed via exec_from_buffer.
    uint32_t             _flags;     // Instruction-information flags.
    TrgAnnoPtr           _trg_anno;  // Target annotation information.
    bool                 _taken;     // True => branch was taken.
    InterResults         _irs;       // Its intermediate results     
    unsigned             _gcount;    // Global count:  Relative ordering of the instruction in the trace.
  };

  //----------------------------------------------------------------------
  // a register entry (holds both initial and final values)
  //----------------------------------------------------------------------
  class RegEntry {
  public:
    RegEntry (const char* n, const rnumber::RNumber& v,bool shared,const RegFieldInfos *fields) : 
      _name(n),_init_value(v),_value(v),_umask(""),_shared(shared), _fields(fields) {}
    RegEntry (const char* n, const rnumber::RNumber& v,const rnumber::RNumber& u,bool shared,const RegFieldInfos *fields) : 
      _name(n),_init_value(v),_value(v),_umask(u),_shared(shared), _fields(fields) {}
    const std::string& name () const {return _name;}
    const rnumber::RNumber& umask () const {return _umask;}
    void   value (const rnumber::RNumber& v)  {_value = v; }
    const rnumber::RNumber& value () const {return _value;}
    const rnumber::RNumber& init_value () const {return _init_value;}
    bool shared () const {return _shared;}
    const RegFieldInfos *fields() const { return _fields; };
  private:
    std::string          _name;
    rnumber::RNumber     _init_value;
    rnumber::RNumber     _value;
    rnumber::RNumber     _umask;
    bool                 _shared;
    const RegFieldInfos *_fields;
  };

  //----------------------------------------------------------------------
  // An mmu translation entry
  //----------------------------------------------------------------------
  class TransEntry {
  public:
    TransEntry(const char *l,unsigned id,unsigned s,unsigned w,const FieldData &ld,addr_t epn,bool ve,addr_t rpn,bool vr) : 
      _ld(ld),_lookup(l),_id(id),_set(s),_way(w),_epn(epn), _valid_epn(ve), _rpn(rpn),_valid_rpn(vr) {}
    const char* lookup() { return _lookup;}
    unsigned id() { return _id; };
    unsigned set() { return _set;}
    unsigned way() { return _way;}
    addr_t rpn() { return _rpn;}
    addr_t epn() { return _epn;}
    bool valid_epn() { return _valid_epn;}
    bool valid_rpn() { return _valid_rpn;}

    FieldData  _ld;
  private:
    const char *_lookup;
    unsigned    _id;
    unsigned    _set;
    unsigned    _way;
    addr_t      _epn;
    bool        _valid_epn;
    addr_t      _rpn;
    bool        _valid_rpn;
  };

  typedef std::unique_ptr<InstrEntry> InstrEntryPtr;
  typedef std::list<InstrEntryPtr> Instrs;

  typedef std::unique_ptr<TransEntry> TransEntryPtr;
  typedef std::list<TransEntryPtr> Translations;

  struct Regs  : public std::list<RegEntry> {};
  class CoreInfo;
  typedef std::shared_ptr<CoreInfo> CoreInfoPtr;
  typedef std::vector<CoreInfoPtr> Cores;

  // The first array is indexed by MMU lookup id.  The second contains a sequence
  // of field indices which are translation attributes.  The AttrList vector's
  // elements are tuples, where the first element is an order value, the second
  // element is a field index (from a FieldData object) and the second element is
  // a bit width of the element.
  struct AttrObj {
    int _order;
    int _index;
    int _width;
    AttrObj(int o,int i,int w) : _order(o), _index(i), _width(w) {};
  };
  typedef std::vector<AttrObj> AttrList;
  typedef std::vector<AttrList> TransAttrs;

  // For storing information about register suppression, i.e. the ability to
  // turn off reporting about certain registers.
  enum IrSuppress { isNone, isIr, isAll };
  struct RegSuppressHash : public std::map<unsigned,IrSuppress> {};
  typedef std::map<unsigned,const RegFieldInfos *> RegFieldInfoMap;

  // We re-use the register-suppress hash and put it in a vector, indexed by the
  // MMU lookup id.
  typedef std::vector<RegSuppressHash> TlbSuppressHash;

  // We allow exception to be suppressed and to squash instructions.
  struct ExcptData {
    IrSuppress _ir_action;
    bool       _squash;
    bool       _asynch;
    ExcptData() : _ir_action(isNone), _squash(false), _asynch(false) {};
  };
  typedef std::map<unsigned,ExcptData> ExcptDataHash;

  IrSuppress ignore_type(const RegSuppressHash &ignores,unsigned id);

  //----------------------------------------------------------------------
  // a core info, this holds the resources (regs,insts,mem,mmu) for this core
  //----------------------------------------------------------------------
  class CoreInfo {
  public:
    // node :  Simulation node that this object represents.
    // tid  :  Thread id, if applicable.  If an ID is set (>= 0), then it means that either this node
    //         has threads as children, or that this is a child that represents a thread.
    // tinst:  Thread instance, if applicable.  If set (>= 0), then it means that this is a node that
    //         represents a thread.
    CoreInfo(IssNode* node,int tid = -1,int tinst = -1);
    ~CoreInfo();

    void reset();

    const std::string &name() const { return _name; };
    const std::string &type() const { return _type; };
    const std::string &path() const { return _path; };
    std::string curpath() const { return _path.substr(_path_start); };

    bool print_ta() const { return _print_ta; };

    CoreInfo *parent() const { return _parent.get(); };
    IssNode* node() const { return _trg.node();} // has to be a non-const in order to be able to setback the pc
    IssCore* core() const { return dynamic_cast<IssCore*>(_trg.node()); };

    const Instrs &get_instrlist () const {return _instrs;} 
    Instrs &get_instrlist () {return _instrs;} 

    void remove_instrs();

    const Regs &get_reglist()  const { return _regs;}
    Regs &get_reglist() { return _regs;}

    // This gets the program counter from the current node.  This function will do
    // the necessary thread-context switching, so use it instead of directly
    // calling getProgramCounter() on the node.  It will then also call
    // extInstrReadTranslate so that the return value is a pair of (ea,ra).
    std::pair<addr_t,addr_t> getProgramCounter() const;

    // Sets the program counter of the current node.  Does context switching as
    // necessary.
    void setProgramCounter(addr_t a) const;

    const Translations &get_init_trans() const { return _init_trans;}
    const Translations &get_final_trans() const { return _final_trans;}
    InstrEntry *add_instr (InstrEntryPtr &&e);
    void add_translation (TransEntryPtr &&t,bool init);
    void add_child(CoreInfoPtr c) { _children.push_back(c); };
    const Cores &children() const { return _children; };
    const TransAttrs &transAttrs() const { return _transAttrs; };

    // This shifts the EPN and RPN fields of a translation, as needed, to account
    // for Raptor's weird behavior.
    void addrHack(FieldData &fd,unsigned id);

    const RegSuppressHash &tlb_suppress(unsigned id) const { return _tlb_suppress[id]; };
    IrSuppress reg_suppress(unsigned id) const;
    ExcptData excpt_data(unsigned id) const;

    const RegFieldInfos *reg_fields(unsigned id) const;

    void setParent(CoreInfoPtr p) { _parent = p; };
    void setNameData(const std::string &n,const std::string &t,const std::string &p,unsigned ps);
    void setThreadInfo(int id) { _trg.set_ctx_id(id); };

    // Returns true if this node represents a thread.
    bool is_thread_instance() const { return _trg.ctx_num() >= 0; };

    // Returns true if this node represents the parent of a group of threads.
    bool is_thread_parent() const { return (_trg.ctx_id() >= 0) && (_trg.ctx_num() < 0); };

    // On a switch to a core that has threads, this returns the CoreInfo object of the 
    // active thread.
    CoreInfo *get_thread(const IssNode *parent);

    // This switches to thread 'cn' if the context id represents a thread context.
    CoreInfo *switch_thread(unsigned id,unsigned cn);

    // If this is a thread instance, then this returns the parent, otherwise it returns itself.
    CoreInfo *switch_to_parent();

  private:   
    void setup(const IssNode &);

    std::string _name, _type, _path;

    bool _print_ta;

    // Index of the start of the path for this object in the full path name in
    // _path.
    unsigned _path_start;

    // the list of instructions 
    Instrs _instrs;
    // the list of regs with initial & final values
    Regs _regs;
    // the list of initial translations
    Translations _init_trans;
    // the list of final translations
    Translations _final_trans;

    // Translation attributes that we care about.
    TransAttrs _transAttrs;

    // TLB fields we want to suppress.
    TlbSuppressHash    _tlb_suppress;
  
    // What registers to suppress.
    RegSuppressHash    _reg_suppress;
    ExcptDataHash      _excpt_suppress;

    // What registers have fields.
    RegFieldInfoMap    _reg_fields;

    // This core's children.
    Cores _children;

    // The node to which this item refers.  Contains both a node pointer and
    // any context information refering to threads.
    Target    _trg;

    CoreInfoPtr _parent;

    // Needed for modifying the EPN/RPN fields to be Raptor compliant.
    // Major hack!  Should be fixed someday!
    struct HackFields {
      int _epn, _rpn;
      int _shift;
    
      HackFields() : _epn(-1), _rpn(-1), _shift(0) {};
    };

    std::vector<HackFields> _hacks;         // integer handles to the mmu field info  
  };

  // Builds a CoreInfo hierarchy which mirrors an IssNode hierarchy.
  CoreInfoPtr buildHierarchy( IssNode &node );

  //----------------------------------------------------------------------
  // This is used to contain all the information needed for a given real address, including the
  // list of mappings associated with it.
  //----------------------------------------------------------------------

  // This is used to represent a single mapping (ea/ta/owner info) when writing out the real addresses.
  class MapNode {
  public:
    MapNode(addr_t ea, unsigned ta, CoreInfo* c) : _ea(ea),_ta(ta),_owner(c) {}
    addr_t ea() const { return _ea; }
    uint32_t ta() const { return _ta; }
    CoreInfo* owner() const { return _owner; }
 
  private:
    addr_t    _ea;
    uint32_t    _ta;
    CoreInfo* _owner;    
  };

  // The real address info (contains the mappings list of MapNode's)
  class RaInfo {
  public:
    typedef std::list <MapNode> Mappings;

    RaInfo() : _owner(0),_id(0),_init_value(0),_umask(0) {}
  
    void id(unsigned id) { _id = id; };
    unsigned id() const { return _id; };
    void owner(CoreInfo* o) {_owner = o; }
    CoreInfo* owner() const {return _owner;}
    void init_value(addr_t v) {_init_value = v;}
    addr_t init_value() const {return _init_value;}
    void umask (addr_t m) {_umask = m; }
    addr_t umask() const {return _umask;}

    void add_mapping (addr_t &ea,unsigned &ta,CoreInfo* c);

    bool get_mapping (addr_t &ea,unsigned &ta,const CoreInfo* c) const;

  private:
    CoreInfo*     _owner;
    unsigned      _id;
    addr_t        _init_value;
    addr_t        _umask; 
    Mappings      _mappings;
  };

}

#endif

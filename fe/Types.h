//
// Copyright (C) 2005 by Freescale Semiconductor Inc.  All rights reserved.
//
// You may distribute under the terms of the Artistic License, as specified in
// the COPYING file.
//

//
// Various types used in the front-end.
//

#ifndef _TYPES_H_
#define _TYPES_H_

#include <sstream>



#include "opencxx/parser/ptree-generated.h"
#include "opencxx/parser/Leaf.h"
#include "opencxx/parser/PtreeArray.h"
#include "opencxx/parser/PtreeUtil.h"
#include "opencxx/parser/PtreeBrace.h"
#include "opencxx/Bind.h"

#include "rnumber/RNumber.h"

#include "helpers/gc_list.h"
#include "helpers/gc_string.h"
#include "helpers/gc_vector.h"
#include "helpers/gc_set.h"
#include "helpers/gc_hash_set.h"
#include "helpers/gc_map.h"
#include "helpers/stringhash.h"

namespace Opencxx {
  class Ptree;
};

namespace std {
  namespace tr1 {

    // Hash/set support for Ptree objects, by name.  This *only* supports hashing leaves.
    template<> struct hash<Opencxx::Ptree *>
    {
      size_t operator()(Opencxx::Ptree *x) const { 
        const char* __s = x->GetPosition();
        unsigned long __h = 0;
        for (int i = 0; i != x->GetLength(); ++i) {
          __h = 5 * __h + *__s;
        }
        return size_t(__h);      
      }
    };
  }
}

namespace adl {

  class Aspects;
  class Reg;
  class RegFile;
  class RegBase;
  class RegField;
  class InstrField;
  class SubInstr;
  class SubInstrGrp;
  class Instr;
  class InstrAlias;
  class Memory;
  
  // This is a more deterministic symbol generator than the one in Plasma.  Use
  // this one so that files will generate the same way, so that comparisons are
  // easier.
  Opencxx::Ptree *AdlGenSym();

  // Use this for retrieving the parent define block in an environment.
  extern Opencxx::Ptree *parentObj;

  // Turn on extra debugging when printing an ADL data model.
  bool dataModelDebug();
  void setDataModelDebug(bool d);

  // This sorts Ptree objects by stringifying the value and then comparing the
  // value.  There's probably a more efficient way to sort these things, but
  // this seemed fairly straightforward for now.
  int compare(Opencxx::Ptree *x,Opencxx::Ptree *y);

  // This is a more complex comparison which will apply Equal, and thus may be
  // used to compare complex structures.
  struct PtreeCmp {
    bool operator()(Opencxx::Ptree *x,Opencxx::Ptree *y) const { return compare(x,y) < 0; };
  };

  typedef gc_vector<unsigned> UIntVect;
  typedef gc_vector<uint64_t> UInt64Vect;
  typedef gc_vector<int> IntVect;
  typedef gc_list<int> IntList;

  typedef std::set<int> IntSet;
  
  typedef gc_vector<gc_string> StrVect;

  struct StrSet : public gc_set<gc_string> {
    template <class C>
    void add(const C &x) {
      insert(x.begin(),x.end());
    }

    StrSet() {};

    template <class _InputIterator>
    StrSet(_InputIterator __f, _InputIterator __l) : gc_set<gc_string>(__f,__l) {};
  };

  typedef gc_hash_set<gc_string> StrHashSet;

  struct StrStrMap : public gc_map<gc_string,gc_string> {};

  typedef gc_vector<Opencxx::Ptree *> PVect;

  std::ostream &operator<<(std::ostream &o,const StrVect &sv);  

  struct StrList : public gc_list<gc_string> {};

  struct StrBoolList : public gc_list<std::pair<gc_string,bool> > {};

  struct StrPairList : public gc_list<std::pair<gc_string,gc_string> > {};

  struct StrVecList : public gc_list<StrVect> {};

  std::ostream &operator<<(std::ostream &o,const StrList &sl);  

  struct PList : public gc_list<Opencxx::Ptree *> {};

  struct PMap : public gc_map<gc_string,Opencxx::Ptree *> {};

  typedef tr_list<std::pair<Opencxx::Ptree *,gc_string> > VarList;

  typedef gc_list<std::pair<Opencxx::Ptree *,Opencxx::Ptree *> > PPairList;

  struct IntMap : public gc_map<gc_string,unsigned> {};

  struct IntOrFunc {
    int             _val;
    Opencxx::Ptree *_func;

    IntOrFunc() : _val(0), _func(0) {};
    IntOrFunc(int v) : _val(v), _func(0) {};
    IntOrFunc(Opencxx::Ptree *f) : _val(0), _func(f) {};

    operator bool() const { return (_val || _func); };
    bool is_int()  const { return !_func; };
    bool is_func() const { return _func; };
  };

  std::ostream &operator<<(std::ostream &o,const IntOrFunc &);

  struct IntOrFuncMap : public gc_map<gc_string,IntOrFunc> {};

  std::ostream &operator<<(std::ostream &o,const StrSet &ss);

  // For storing parent information within an environment.
  class BindParent : public Opencxx::Bind {
  public:
    BindParent(const Aspects *parent) : _parent(parent) {}
    Opencxx::Bind::Kind What();
    void GetType(Opencxx::TypeInfo&, Opencxx::Environment*);
    const char* GetEncodedType();
    bool IsType();

    const Aspects *parent() const { return _parent; };
  private:
    const Aspects     *_parent;
    static const char *_type;
  };

  // Add a parent object pointer to the given environment.
  void addParentObj(Opencxx::Environment *env,Aspects *parent);

  // Stores information about a single resource used within an instruction.
  // This is typically either a register, a register-file element, a slice of a
  // register-file, or an entire register file.
  struct UsedReg {
  protected:
    const RegBase    *_reg;

  public:
    Opencxx::Ptree   *_begin; // Start of a slice, or else the entire index.
    Opencxx::Ptree   *_end;   // The end of a slice, or else 0 if just a single
                              // index.
    int               _parm;  // If >= 0, then _begin is a parm.

    // Minor hack: This can be modified b/c it's not part of the sorting order
    // defined by operator<.  So we allow it to be modified in case subsequent
    // passes determine that something should be treated as conditional or a
    // partial-write.
    mutable rnumber::RNumber _mask;    // Stores which bits of the register are accessed.
    mutable bool             _partial; // Indicates that this is a field/slice read/write.
    mutable Opencxx::Ptree  *_delay;   // Indicates that this is a delayed write.  Expression is the delay expression.
    mutable bool             _direct;  // Indiciates that this is a direct
                                       // access (shouldn't be virtualized or
                                       // tracked).
    mutable bool             _cond;    // Indicates that this is in a conditional.

    UsedReg();
    UsedReg(const RegBase *r);
    UsedReg(const RegBase *r,Opencxx::Ptree *b);
    UsedReg(const RegBase *r,Opencxx::Ptree *v,int p);
    UsedReg(const RegBase *r,Opencxx::Ptree *b,Opencxx::Ptree *e);

    bool empty() const{ return _reg == 0; };

    // Returns true if this represents a parm.
    bool is_parm() const { return _parm >= 0; };

    void clear_parm() { _parm = -1; };
 
    // Returns true if this has a single, fixed element associated with it.
    bool is_fixed() const;

    // Returns true if this represents a range of elements.
    bool is_range() const;

    void clear_fixed() { _begin = 0; };

    bool conditional() const { return _cond; };

    bool partial() const { return _partial; };

    bool delayed() const { return _delay; };
    Opencxx::Ptree *delay() const { return _delay; };

    bool direct() const { return _direct; };

    // This clears the partial-access status if a mask is fully set.
    void check_if_still_partial() const;

    // Updates the mask in the correct manner.  Masks are only combined if
    // they're in use (non-zero).  Otherwise, the mask is cleared, which
    // signifies that the entire register is being accessed.
    void update_mask(const UsedReg &ur) const;

    // This updates the mutable aspects of this class.
    void update_mutables(const UsedReg &ur) const;

    // Returns the increment value for the next slot.
    int next_slot(bool &fixed) const;

    void setreg(const RegBase *r);
    const Reg *reg() const;
    const RegFile *regfile() const;
    const RegBase *regb() const { return _reg; };

    friend std::ostream &operator<<(std::ostream &o,const UsedReg &ur);
    void display() const;
  };

  // Less-than comparison which does not compare masks.
  bool operator<(const UsedReg &,const UsedReg &);

  // A less-than comparison that does take into account the mask value.
  bool compare_lt_w_mask(const UsedReg &x,const UsedReg &y);

  // A less-than comparison which simply detects if a mask exists.
  bool compare_lt_if_mask(const UsedReg &x,const rnumber::RNumber &x_mask,const UsedReg &y,const rnumber::RNumber &y_mask);

  bool operator==(const UsedReg &,const UsedReg &);
  inline bool operator!=(const UsedReg &x,const UsedReg &y) { return !(x == y); };

  struct SortByParm {
    bool operator()(const UsedReg *x,const UsedReg *y) {
      return x->_parm < y->_parm;
    };
  };

  struct UsedRegs : public gc_set<UsedReg> {
    template <class C>
      void add(const C &x) {
      insert(x.begin(),x.end());
    }
    void update_mutables(const UsedRegs &x);

    friend std::ostream &operator<<(std::ostream &o,const UsedRegs &urs);
    void display() const;
  };

  typedef gc_vector<UsedReg> UsedRegList;
  std::ostream &operator<<(std::ostream &o,const UsedRegList &urs);



  // Stores information about a memory access used by a resource.
  struct UsedMem {
  protected:
    gc_string         _name;

  public:
    mutable Opencxx::Ptree  *_delay;   // Indicates that this is a delayed write.  Expression is the delay expression.

    UsedMem(const gc_string &name,Opencxx::Ptree *delay = 0);

    const gc_string &name() const { return _name; };

    bool delayed() const { return _delay; };
    Opencxx::Ptree *delay() const { return _delay; };

    friend std::ostream &operator<<(std::ostream &o,const UsedMem &um);
    void display() const;
  };

  bool operator<(const UsedMem &,const UsedMem &);

  bool operator==(const UsedMem &,const UsedMem &);
  inline bool operator!=(const UsedMem &x,const UsedMem &y) { return !(x == y); };

  struct UsedMems : public gc_set<UsedMem> {
    template <class C>
      void add(const C &x) {
      insert(x.begin(),x.end());
    }

    friend std::ostream &operator<<(std::ostream &o,const UsedMems &urs);
    void display() const;
  };


  struct ResExclude : public UsedReg {
    Opencxx::Ptree *_src;
    mutable bool    _handled;

    ResExclude(Opencxx::Ptree *p) : _src(p), _handled(false) {};
    ResExclude(const UsedReg &ur) : UsedReg(ur), _src(0), _handled(false) {};

    ResExclude &operator=(const UsedReg &ur)
    {
      static_cast<UsedReg &>(*this) = ur;
      return *this;
    }
  };

  typedef gc_set<ResExclude> ResExcludes;

  // This type is used to store information about a helper-function call within a piece of action code.
  // The outer call is still a PtreeFuncallExpr, but the function's name (Car) will be of this type.
  class PtreeHelperCall
  {
  public:
    PtreeHelperCall(UsedRegList &parms);

  protected:
    UsedRegList _parms;
  };

  class PtreeHelperCallLeaf : public Opencxx::Leaf, public PtreeHelperCall
  {
  public:
    PtreeHelperCallLeaf(Ptree *p,UsedRegList &parms);

    int Write(std::ostream&, int);    
  };

  class PtreeHelperCallNonLeaf : public Opencxx::NonLeaf, public PtreeHelperCall
  {
  public:
    PtreeHelperCallNonLeaf(Ptree *p,UsedRegList &parms);

    int Write(std::ostream&, int);    
  };

  // Similar to a PtreeBrace, but doesn't include the braces, so it lets us use
  // it as a generic sequence of statements which does not introduce a new
  // scope.
  class PtreeStmts : public Opencxx::PtreeBrace
  {
  public:
    PtreeStmts(Ptree* p, Ptree* q) : Opencxx::PtreeBrace(p,q) {};
    PtreeStmts(Ptree* body) : Opencxx::PtreeBrace(0,body,0) {};

    void Print(std::ostream&, int, int);
    int Write(std::ostream&, int);

    virtual Opencxx::Ptree* Translate(Opencxx::AbstractTranslatingWalker* walker);    
  };

  struct UsageInfo : public gc {
    UsedRegs _src_regs,   _trg_regs;     // Source and target registers used.
    UsedMems _src_mems,   _trg_mems;     // Source and target memories used.
    StrSet   _src_caches, _trg_caches;   // Source and target memories used.
    StrSet   _helpers;                   // Any helpers referenced in this function.
    bool     _exceptions;                // Raises exceptions.
    bool     _halts;                     // Calls the halt function.
    bool     _explicit_srcs;             // True => sources have been explicitly stated by user.
    bool     _explicit_trgs;             // True => targets have been explicitly stated by user.

    UsageInfo() : _exceptions(false), _halts(false), _explicit_srcs(false), _explicit_trgs(false) {};

    // Merge usage information into this item.  If merge_cond is true, then
    // duplicates which are both conditional are marked as non-conditional while
    // elements which are unique are marked as conditional.
    void update(const UsageInfo &x,bool merge_cond = false);
    void update_non_regs(const UsageInfo &x);

    bool operator==(const UsageInfo &) const;

    // This just returns true if any of the internal sets contains anything.
    bool empty() const;

    // This marks all registers as conditional.
    void mark_as_cond();

    void clear();

    void display();
  };

  void printUsageInfo(std::ostream &o,const UsageInfo &ui,const gc_string &pfx);
  std::ostream &operator<<(std::ostream &o,const UsageInfo &ui);
 
  void printUsageInfo(std::ostream &o,const UsageInfo &ui,const gc_string &pfx);
  std::ostream &operator<<(std::ostream &o,const UsageInfo &ui);

  // Stores information about a helper function.
  struct HelperInfo : public UsageInfo {
    // State variable used for handling possible recursion.
    enum State { sNone, sInProcess, sDone };

    Opencxx::Ptree *_func;                 // The helper itself.
    State           _state;

    HelperInfo() : _func(0), _state(sNone) {};
    HelperInfo(Opencxx::Ptree *f) : _func(f), _state(sNone) {};

    operator bool() const { return _func; };

    bool in_process() const { return _state == sInProcess; };
    bool not_processed() const { return _state != sInProcess; };
    bool done() const { return _state == sDone; };

    void set_in_process() { _state = sInProcess; };
    void set_clear() { _state = sNone; };
    void set_done() { _state = sDone; };

    void print(std::ostream &o,const gc_string &pfx) const;
  };

  // Used for identifying a helper by name and arguments so that we can cache
  // translation results.
  struct HelperArgs {
    gc_string                   _name;     // Function name.
    bool                        _has_args; // Whether args were given.
    gc_vector<Opencxx::Ptree *> _args;     // The args, if given.

    HelperArgs(const gc_string &n,Opencxx::PtreeArray *args);

    bool operator<(const HelperArgs &) const;
  };

  // Maps name+args -> helper-function info (translated code + usage info).  We
  // use this as a cache to avoid having to repeatedly translate helper
  // functions which are called again and again with the same arguments.
  typedef gc_map<HelperArgs,HelperInfo> HelperArgsMap;

  std::ostream &operator<< (std::ostream &,const HelperInfo &);

  struct HelperMap : public gc_map<gc_string,HelperInfo> {};

  enum RegUsedFlags { 
    RegRead            = 0x1,
    RegWrite           = 0x2,
    RegSliceRead       = 0x4,
    RegSliceWrite      = 0x8,
    RegFieldRead       = 0x10,
    RegFieldWrite      = 0x20,
    RegFieldHelperUsed = 0x40,
  };

  // This stores information about a particular register access in action code.
  struct RegAccess {
    RegAccess() : _write(false), _direct(false), _thisreg(false), _reg(0), _regfile(0), _field(0), _index(0), _rindex(0),
                  _findex(0), _left(0), _right(0), _context(0), _delay(0)
    {};

    const RegBase *rb() const;

    bool is_thisreg() const { return _thisreg; };
    bool is_reg() const{ return _reg; };
    bool is_regfile() const{ return _regfile; };
    bool has_field() const { return _field; };
    bool is_slice() const { return _left; };
    bool is_delayed() const { return _delay; };
    bool is_direct() const { return _direct; };

    bool            _write;    // True => This is a write.
    bool            _direct;   // True => This is a direct access.
    bool            _thisreg;  // True => Was a 'ThisReg' reference.
    const Reg      *_reg;      // If set, then the register being accessed.
    const RegFile  *_regfile;  // If set, then the register-file being accessed.
    const RegField *_field;    // A register-field, if applicable.

    Opencxx::Ptree *_index;    // The index, for a register-file access.
    Opencxx::Ptree *_rindex;   // The resource index, i.e. an instruction field.  May differ from _index due
                               // to data-flow.
    Opencxx::Ptree *_findex;   // The index, for an indexed field access.
    Opencxx::Ptree *_left;     // Left operand, for a slice.
    Opencxx::Ptree *_right;    // Right operand, for a slice with two indices (0 if just a single index).
    Opencxx::Ptree *_context;  // A context identifier, if applicable.
    Opencxx::Ptree *_delay;    // A delay value, if applicable.
  };

  std::ostream &operator<< (std::ostream &,const RegAccess &);

  // For register reads.
  class PtreeRegRead : public Opencxx::NonLeaf
  {
  public:
    PtreeRegRead(Ptree *p,bool thisreg);

    virtual Opencxx::Ptree* Translate(Opencxx::AbstractTranslatingWalker* walker);

    int Write(std::ostream&, int);    

    RegAccess _regaccess;

    PtreeRegRead *_next;
  };

  // This type stores information about a register assignment.  Rather than matching
  // on syntax, you can just match against this object type and then extract all
  // relevant information.
  class PtreeRegWrite : public PtreeRegRead
  {
  public:
    PtreeRegWrite(Ptree *p,bool thisreg);

    // Return the rhs of the assignment.
    Opencxx::Ptree *get_expr() { return _expr; };

    virtual Opencxx::Ptree* Translate(Opencxx::AbstractTranslatingWalker* walker);

    int Write(std::ostream&, int); 

    Opencxx::Ptree *_expr;      // The rhs of the assignment.
  };

  // This stores information about a particular memory access in action code.
  struct MemAccess {
    MemAccess() : _write(false), _size(0), _mem(0), _addr(0), _type(0), _cw_addr(0), _delay(0)
    {};

    bool is_global() const { return _mem == 0; };
    const Memory *mem() const { return _mem; };

    bool is_delayed() const { return _delay; };

    bool            _write;    // True => This is a write.
    unsigned        _size;     // Access size (always a constant).
    const Memory   *_mem;      // If set, then the memory being accessed.  If 0, then global memory.

    Opencxx::Ptree *_addr;     // Address expression.
    Opencxx::Ptree *_type;     // Optional type-specifier.
    Opencxx::Ptree *_cw_addr;  // Optional critical-word address.
    Opencxx::Ptree *_delay;    // A delay value, if applicable.
  };

  std::ostream &operator<< (std::ostream &,const MemAccess &);

  // For memory reads.
  class PtreeMemRead : public Opencxx::NonLeaf
  {
  public:
    PtreeMemRead(Ptree *p);

    virtual Opencxx::Ptree* Translate(Opencxx::AbstractTranslatingWalker* walker);

    int Write(std::ostream&, int);    

    MemAccess _memaccess;
  };

  // This type stores information about a memory write.  Rather than matching
  // on syntax, you can just match against this object type and then extract all
  // relevant information.
  class PtreeMemWrite : public PtreeMemRead
  {
  public:
    PtreeMemWrite(Ptree *p);

    // Return the rhs of the assignment.
    Opencxx::Ptree *get_expr() { return _expr; };

    virtual Opencxx::Ptree* Translate(Opencxx::AbstractTranslatingWalker* walker);

    int Write(std::ostream&, int); 

    Opencxx::Ptree *_expr;      // The rhs of the assignment.
  };

  // Similar to a PtreeHelperCall, but stores an entire UsageInfo object.  Used
  // for sub-instructions so that their resources can be tracked properly when
  // scanning a generated instruction.
  class PtreeSubInstrCall : public Opencxx::NonLeaf
  {
  public:
    PtreeSubInstrCall(Opencxx::Ptree *p,const SubInstr &subinstr,const SubInstrGrp &subinstrgrp,const UsageInfo &uinfo);

    virtual Opencxx::Ptree* Translate(Opencxx::AbstractTranslatingWalker* walker);
    int Write(std::ostream&, int); 

    const SubInstr    &subinstr() const { return _subinstr; };
    const SubInstrGrp &subinstrgrp() const { return _subinstrgrp; };
    const UsageInfo   &uinfo() const { return _uinfo; };
  protected:
    const SubInstr     &_subinstr;
    const SubInstrGrp  &_subinstrgrp;
    const UsageInfo    &_uinfo;
  };

  // Helper function which throws a null-pointer runtime_error.
  void null_ptr_exception();

  // For situations where an object is a Ptree originally, but
  // then is replaced by a pointer to a data-model object.
  template <class T>
  union Ptr {
    Opencxx::Ptree *_p;
    const T        *_d;

    Ptr() : _p(0) {};
    operator bool() const { return _d; };
    const T& operator()() const { if (!_d) null_ptr_exception(); return *_d; };
    const T* dptr() const { return _d; };
    void set_dptr(const T *d) { _d = d; };
  };

  // Auxiliary functions are simply functions with unique names which are added
  // by external users of the front-end class.  They consist of a symbol name, a
  // function, and a comment, which is inserted into the code.
  struct AuxFunc : public gc {
    AuxFunc(Opencxx::Ptree *func,const gc_string &rtype,
            const gc_string &comment,bool isConst = false);

    const StrList &arg_mods() const { return _arg_mods; };
    void add_arg_mod(const gc_string &new_arg) { _arg_mods.push_back(new_arg); };

    Opencxx::Ptree     *name() const    { return _name; };
    Opencxx::Ptree     *func() const    { return _func; };
    bool                isConst() const { return _isConst; }
    const gc_string  &rtype() const   { return _rtype; };
    const gc_string  &comment() const { return _comment; };

    void print(std::ostream &o,const gc_string &pfx) const;    

    Opencxx::Ptree *_name;    // Unique name generated upon creation.
    Opencxx::Ptree *_func;    // The function's code (parameters + body).
    const bool      _isConst; // Whether the function is const or not
    gc_string     _rtype;   // The return-type.
    gc_string     _comment; // A comment about the function.
    StrList         _arg_mods;// A list of arguments to transform.
  };

  // Auxiliary functions are just stored in a list.
  struct AuxFuncList : public gc_vector<AuxFunc *> {};

  // Use this type for user-defined hooks and predicates.  An AuxFunc is an
  // auxiliary function- it's read from the uADL file but then injected into the
  // ADL data model so that it can access architectural resources.  This type is
  // a union of a Ptree and an AuxFunc.  During parsing, we store it as a Ptree;
  // during finalization, we insert it into the data model and store a pointer
  // to the returned object so that we can then reference it during code
  // generation.
  typedef Ptr<AuxFunc> PAuxFunc;

  // We use vectors of bools for storing bit masks of arbitrary size.
  // This structure also allows you to print out a string with a boolean
  // representation.
  struct BitVect : public  gc_vector<bool> {

    // Query- returns true if all set or clear within the range (or
    // entire vector).
    bool is_set() const;
    bool is_set(unsigned start,unsigned stop) const;

    bool is_reset() const;
    bool is_reset(unsigned start,unsigned stop) const;

    // Sets a range or the entire vector to true (inclusive).
    void set();
    void set(unsigned start,unsigned stop);

    // Sets a range or the entire vector to false (inclusive).
    void reset();
    void reset(unsigned start,unsigned stop);

    gc_string str() const;
  };

  struct Path : public StrList {
    virtual ~Path() {};

    virtual const char *get_sep() const { return "::"; };
    
  };

  std::ostream &operator<< (std::ostream &,const Path &);
  gc_string getPathStr(const Path &,const char *sep = 0);

  // A class for simplifying C++-class code generation.  Add items to the
  // specified lists, then call the appropriate write routines when generating
  // the final class.
  struct ClassData {
    // Class member variables.
    StrList    _member_vars;
    // These items are added to the constructor's argument list.
    StrList    _constr_args;
    // These items are added to the constructor's init list.
    StrList    _constr_inits; 
    // These statements are added to the constructor's body.
    StrList    _constr_stmts;
    // These statements are added to the resetSimulation() method.
    StrList    _reset_stmts;
    // These functions are added to the dynamic-parameter get/set/list routines.
    StrList    _dyn_parms;
    // Statements added to the destructor.
    StrList    _dstr_stmts;

    void writeMemberVars(std::ostream &o) const;
    void writeConstrArgs(std::ostream &o) const;
    void writeConstrInits(std::ostream &o) const;
    void writeConstrStmts(std::ostream &o) const;
    void writeResetStmts(std::ostream &o) const;
    void writeDestrStmts(std::ostream &o) const;
  };

  //
  // Reify functions to handle type conversion.  This includes some reify types
  // which don't exist in the underlying Opencxx library.  These are setup in a
  // uniform manner to make them amenable to use by templates.
  //

  bool reifyPtree(unsigned &res,Opencxx::Ptree *p);
  bool reifyPtree(int &res,Opencxx::Ptree *p);
  bool reifyPtree(uint64_t &res,Opencxx::Ptree *p);
  bool reifyPtree(double &res,Opencxx::Ptree *p);
  bool reifyPtree(bool &res,Opencxx::Ptree *p);

  // Creates a leaf node of the specified type, containing the textual version of the
  // integer argument.
  template <typename L,typename N>
  L *make_leaf(N n)
  {
    std::ostringstream ss;
    ss << n;
    char *str = new (GC) char[ss.str().size()+1];
    strcpy(str,ss.str().c_str());
    return new L(str,ss.str().size());
  }

  // Leaf creation functions for specific types.
  Opencxx::Ptree *make_leaf(bool n);
  Opencxx::Ptree *make_leaf(unsigned n);
  Opencxx::Ptree *make_leaf(int n);
  Opencxx::Ptree *make_leaf(uint64_t n);
  Opencxx::Ptree *make_leaf(double n);
  
  
  // Arithmetic on integers.  This is templated so that we can handle different basic
  // numerical types.  Returns true if we can evaluate it, false otherwise.
  template <class N>
  bool evalExpr(N &r,N x,N y,Opencxx::Ptree *oper) 
  {
    using namespace Opencxx::PtreeUtil;

    if (oper->GetLength() == 1) {
      char op = *oper->GetPosition();
      switch (op) {
      case '+':
        r = x + y;
        return true;
      case '-':
        r = x - y;
        return true;
      case '*':
        r = x * y;
        return true;
      case '/':
        r = x / y;
        return true;
      case '%':
        r = x % y;
        return true;
      case '&':
        r = x & y;
        return true;
      case '|':
        r = x | y;
        return true;
      case '^':
        r = x ^ y;
        return true;
      case '>':
        r = x > y;
        return true;
      case '<':
        r = x < y;
        return true;
      }
    } else {
      if (Eq(oper,"==")) {
        r = (x == y);
        return true;
      }
      else if (Eq(oper,"!=")) {
        r = (x != y);
        return true;
      }
      else if (Eq(oper,"<=")) {
        r = (x <= y);
        return true;
      }
      else if (Eq(oper,">=")) {
        r = (x >= y);
        return true;
      }
      else if (Eq(oper,"&&")) {
        r = (x && y);
        return true;
      }
      else if (Eq(oper,"||")) {
        r = (x || y);
        return true;
      }
    }
    return false;
  }

  // Unary arithmetic ops.
  template <class N>
  bool evalExpr(N &r,N x,Opencxx::Ptree *oper) 
  {
    using namespace Opencxx::PtreeUtil;

    if (oper->GetLength() == 1) {
      char op = *oper->GetPosition();
      switch (op) {
      case '+':
        r = x;
        return true;
      case '-':
        r = -x;
        return true;
      case '!':
        r = !x;
        return true;
      case '~':
        r = ~x;
        return true;
      }
    }
    return false;
  }

  // Specialization for doubles, since not all operators are supported for
  // doubles.
  inline bool evalExpr(double &r,double x,double y,Opencxx::Ptree *oper) 
  {
    using namespace Opencxx::PtreeUtil;

    if (oper->GetLength() == 1) {
      char op = *oper->GetPosition();
      switch (op) {
      case '+':
        r = x + y;
        return true;
      case '-':
        r = x - y;
        return true;
      case '*':
        r = x * y;
        return true;
      case '/':
        r = x / y;
        return true;
      case '>':
        r = x > y;
        return true;
      case '<':
        r = x < y;
        return true;
      }
    } else {
      if (Eq(oper,"==")) {
        r = (x == y);
        return true;
      }
      else if (Eq(oper,"!=")) {
        r = (x != y);
        return true;
      }
      else if (Eq(oper,"<=")) {
        r = (x <= y);
        return true;
      }
      else if (Eq(oper,">=")) {
        r = (x >= y);
        return true;
      }
      else if (Eq(oper,"&&")) {
        r = (x && y);
        return true;
      }
      else if (Eq(oper,"||")) {
        r = (x || y);
        return true;
      }
    }
    return false;
  }

  // Unary arithmetic ops.
  inline bool evalExpr(double &r,double x,Opencxx::Ptree *oper) 
  {
    using namespace Opencxx::PtreeUtil;

    if (oper->GetLength() == 1) {
      char op = *oper->GetPosition();
      switch (op) {
      case '+':
        r = x;
        return true;
      case '-':
        r = -x;
        return true;
      case '!':
        r = !x;
        return true;
      }
    }
    return false;
  }  

  // Attempts to evaluate a unary or binary expression.  Operands should have
  // already been translated to handle variables.
  template <typename N>
  struct AttemptExprEval {
    virtual ~AttemptExprEval() {};

    // For binary expressions.
    Opencxx::Ptree *eval(Opencxx::Ptree *left,Opencxx::Ptree *op,Opencxx::Ptree *right)
    {
      N l,r;
      // First, see if the left and/or right are constants.  If not, then try
      // and get their values.  If we succeed for both, then evaluate.
      bool got_left  = reifyPtree(l,left);

      if (right) {
        // Binary expression situation.
        bool got_right = reifyPtree(r,right);
        if (got_left && got_right) {
          N res;
          if ( evalExpr(res,l,r,op)) {
            return make_leaf(res);
          }
        }
      } else if (got_left) {
        // Unary op.
        N res;
        if ( evalExpr(res,l,op) ) {
          return make_leaf(res);
        }
      }
      return 0;
    }
      
    // For unary expressions.
    Opencxx::Ptree *eval(Opencxx::Ptree *left,Opencxx::Ptree *op)
    {
      return eval(left,op,0);
    }
    
  };

  // replaceAllEqual() substitutes SUBST[i].second for all occurences of ORIG[i].first in
  // LIST.  It recursively searches LIST for ORIG.
  Opencxx::Ptree* replaceAllEqual(Opencxx::Ptree* list, const PPairList &subst );

  // Populate field-substitution and constant-value lists with information from
  // an alias.
  void getAliasSubstitutions(PPairList &subst,IntMap &parms,const InstrAlias *alias);

  // Same as above, except maps the other direction, from target to alias.
  void getRevAliasSubstitutions(PPairList &subst,IntMap &parms,const InstrAlias *alias);

  // Attemps to clone a Ptree object.  This is kind of a hack, b/c there's no
  // way to do this directly in C++.  Instead, we do a memcpy to copy over the
  // type information, then set the car and cdr directly.
  Opencxx::Ptree *clonePtree(Opencxx::Ptree *p,Opencxx::Ptree *car,Opencxx::Ptree *cdr);

  // This is an adapter class, designed to work with process_named_handler in
  // ParsingHelpers.  The idea is that process_items expects an associative
  // container, e.g. a map, but there are times when we want our data model to
  // store just a single item, basically just a pointer to a single object.
  // This wraps a pointer to an object with an interface to allow it to be used
  // with process_named_handler.
  template <class mapped_type>
  struct ContainerPtr {
    typedef gc_string  key_type;
    typedef std::pair<const key_type,mapped_type> value_type;
    typedef value_type* iterator;
    typedef const value_type* const_iterator;

    ContainerPtr() : _d(gc_string(),0) {};

    std::pair<iterator,bool> insert(const value_type &d) 
    { 
      if (d.first == _d.first) {
        return make_pair(begin(),false);
      } else {
        _d = d;
        return make_pair(begin(),true);
      }
    };

    iterator find(const key_type &k) { return (k == _d.first) ? begin() : end(); };

    bool empty() const { return _d.second == 0; };

    operator bool() const { return !empty(); };

    const key_type &name() const { return _d.first; };

    const mapped_type operator()() const { return _d.second; };
    mapped_type operator()() { return _d.second; };

    iterator begin() { return &_d; };
    iterator end() { return 0; };

    const_iterator begin() const { return &_d; };
    const_iterator end() const { return 0; };
    
    std::pair<key_type,mapped_type*> _d;
  };

  // Swap words in an RNumber.
  rnumber::RNumber swap_words(const rnumber::RNumber &x);
  
}

#endif
